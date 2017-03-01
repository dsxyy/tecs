/******************************************************************************
* Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：image_download.cpp
* 文件标识：
* 内容摘要：image下载管理实现文件
* 当前版本：1.0
* 作    者：颜伟
* 完成日期：2013年3月12日
*
* 修改记录1：
*     修改日期：2013/3/12
*     版 本 号：V1.0
*     修 改 人：颜伟
*     修改内容：创建
******************************************************************************/
#include "image_download.h"
#include "log_agent.h"
#include "storage.h"
#include "event.h"
#include <sys/types.h>
#include <sys/mount.h>

namespace zte_tecs
{
/******************************************************************************/  
STATUS ImageDownloadTask::Init()
{
    _work_thread_id = INVALID_THREAD_ID;
    _agent = ImageDownloadAgent::GetInstance();
    if (NULL == _agent)
    {
        return ERROR;
    }
    return SUCCESS;
}
/******************************************************************************/  
void ImageDownloadTask::Print()
{
    if (_work_thread_id == INVALID_THREAD_ID)
    {
        cout << "task " << getName() << " is still waiting in queue." << endl;
        return;
    }
    
    cout << " downloading task " << getName() << " id = 0x" << hex << _work_thread_id << endl;
    cout << "source: " << _para._source << endl;
    cout << "target: " << _para._target << endl;
    cout << "source size : " << dec << _para._size << " bytes" << endl;
    cout << "source checksum : " << dec << _para._checksum << " bytes" << endl;
    
    struct stat64 s;
    if (0 == lstat64(_para._target.c_str(),&s))
    {
        cout << "target size : " << dec << s.st_size << " bytes" << endl;
        cout << "finished: "<< setw(2) << (s.st_size*100)/_para._size << "%" << endl;
    }
}
/******************************************************************************/  
int ImageDownloadTask::do_system(const string& command)
{
    int status = ERROR;
    pid_t pid = -1;

    if (command.empty())
    {
        return status;
    }

    if ((pid = fork()) < 0)
    {
        OutPut(SYS_DEBUG,"do_system fork fail errno %d!\n",errno);
        return status;
    }
    else if (0 == pid)
    {
        execl("/bin/sh","sh","-c",command.c_str(),(char *)0);
    }
    else
    {
        int ret = -1;
        pid_t w = -1;
        _agent->UpdateList(getName(), pid);
        #if 0
        while(waitpid(pid,&status,0) < 0)
        {
            if (EINTR == errno)
            {
                status = ERROR;
                break;
            }
        }
        #endif  
        do {
           w = waitpid(pid, &ret, WUNTRACED | WCONTINUED);
           if (w == -1) 
           {
               status = ERROR;
               break;
           }
           
           if (WIFEXITED(ret)) 
           {
               status = WEXITSTATUS(ret);
           } 
           else if (WIFSIGNALED(ret) || WIFSTOPPED(ret)) 
           {
               status = ERROR;
           } 
        } while (!WIFEXITED(ret) && !WIFSIGNALED(ret) && !WIFSTOPPED(ret));
        _agent->UpdateList(getName(), -1);
    }

    return status;
}
/******************************************************************************/  
STATUS ImageDownloadTask::Execute(void)
{
    STATUS ret = ERROR;
    ostringstream oss;
    
    _work_thread_id = pthread_self();
    //由线程池线程入队 防止出现已经入队 而线程池线程起不来的情况
    //这样入队的ctl就僵死了
    ImageDownloadCtl ctl(getName(),_para,_req);
    if (_agent->QueryAndAddList(getName(),ctl))
    {//控制队列里面已经有了 说明已经有搬运工干过活了而且已经完成了
        OutPut(SYS_NOTICE,"Task %s is done,I am unnecessary\n",getName().c_str());
        return SUCCESS;
    }
    
    _agent->UpdateList(getName(), IDS_DOWNLOADING, "Start Download");
    string command = "curl -s file://" + _para._source + " -o " + _para._target;
    OutPut(SYS_NOTICE,"Task 0x%lx is running command: \"%s\"!\n",_work_thread_id,command.c_str());
    ret = do_system(command.c_str());
    if (-1 == ret)
    {
        oss<<"system command ["<<command<<"] failed! errno = %d"<<errno<<endl;
        goto EXEC_RET;
    }
    else if (0 != ret)
    {
        OutPut(SYS_NOTICE,"system command [%s] returns %d\n",command.c_str(),ret);
    }
    
    if((IMAGE_SOURCE_TYPE_FILE == _para._option._img_srctype) && !_para._checksum.empty())
    {
        string  checksum;
        _agent->UpdateList(getName(), IDS_CHECKSUMING, "Start Checksum");
        OutPut(SYS_DEBUG,"call sys_md5 calc %s checksum,should %s\n",_para._target.c_str(),_para._checksum.c_str());
        if(SUCCESS == sys_md5(_para._target,checksum))
        { 
            if (checksum != _para._checksum)
            {
                oss<<getName()<<"checksum "<<_para._target<<" mismatch,should:"<<_para._checksum<<",but:"<<checksum<<endl;
                goto EXEC_RET;
            }
        }
        else
        { 
            oss<<"checksum "<<_para._target<<" failed! errno = %d"<<errno<<endl;
            goto EXEC_RET;
        }
    }
    
    oss<<"Download image("<<_para._target<<") size = "<<_para._size<<" is ok!"<<endl;
    ret = SUCCESS;
    
EXEC_RET:    
    if (SUCCESS == ret)
    {
        OutPut(SYS_DEBUG,"%s\n",oss.str().c_str());
        _agent->UpdateList(getName(), IDS_FINISH, oss.str());
    }
    else
    {
        OutPut(SYS_ERROR,"%s\n",oss.str().c_str());
        _agent->UpdateList(getName(), IDS_ERROR, oss.str());
    }
    Datetime dt;
    _agent->UpdateList(getName(), dt);
    return ret;
}
/******************************************************************************/  
ImageDownloadAgent* ImageDownloadAgent::_instance = NULL;
/******************************************************************************/  
STATUS ImageDownloadAgent::Init()
{
    STATUS ret = ERROR;
    ret = _mutex.Init();
    if (SUCCESS != ret)
    {
        OutPut(SYS_ERROR,"Init Mutex Failed!\n");
        return ret;
    }
    _tpool = new ThreadPool(MAX_IMAGE_DOWNLOAD_TASKS);
    if (!_tpool)
    {
        OutPut(SYS_EMERGENCY,"failed to create ThreadPool!\n");
        return ERROR;
    }
    ret = _tpool->CreateThreads("ImageDownloader");
    if (SUCCESS != ret)
    {
        OutPut(SYS_EMERGENCY,"failed to start ThreadPool threads! ret = %d\n",ret);
        return ret;
    }
    ostringstream oss;
    KillProcess("md5sum", TARGET_MOUNT_DIR, oss);
    KillProcess("curl", SOURCE_MOUNT_DIR, oss);
    OutPut(SYS_DEBUG,"Image download init kill process\n");
    OutPut(SYS_DEBUG,"%s\n",oss.str().c_str());
    
    _timer = INVALID_TIMER_ID;
    _last_query_times = 0;
    _current_query_times = 0;

    ret = Start(MU_IMAGE_DOWNLOAD_AGENT);
    if(SUCCESS != ret)
    {
        return ret;
    }  
    
    //给自己发一个上电通知消息
    MessageOption option(MU_IMAGE_DOWNLOAD_AGENT,EV_POWER_ON,0,0);
    MessageFrame frame(SkyInt(0),option);
    return m->Receive(frame);
}
/******************************************************************************/  
STATUS ImageDownloadAgent::DeleteMountDir(const string& name, ostringstream& oss)
{
    oss<<"...Delete mountdir..."<<endl;
    if (is_regular(name))
    {
        if (0 != unlink(name.c_str()))
        {
            oss<<"Delete mountdir unlink "<<name<<" failed! errno="<<errno<<endl;
            return ERROR;
        }
    }
    else if (is_directory(name))
    {
        if (0 != rmdir(name.c_str()))
        {
            oss<<"Delete mountdir rmdir "<<name<<" failed! errno="<<errno<<endl;
            return ERROR;
        }
    }
    return SUCCESS;
}
/******************************************************************************/  
string ImageDownloadAgent::MakeSourceMountDir(const ImageDownloadReq& req, ostringstream& oss)
{
    oss<<"...Make source mountdir..."<<endl;
    string mountdir;
    if (IMAGE_ACCESS_NFS == req._source_url.access_type)
    {
        mountdir = SOURCE_MOUNT_DIR+req._request_id;
        if (check_fileexist(mountdir))
        {
            if (!is_directory(mountdir))
            {
                if (0 != unlink(mountdir.c_str()))
                {
                    oss<<"Make source unlink "<<mountdir<<" failed! errno="<<errno<<endl;
                    return "";
                }
            }
            else
            {
                return mountdir;
            }
        }
        string cmd,res;
        cmd = "mkdir -p "+mountdir;
        if (SUCCESS != storage_command(cmd,res))
        {
            oss<<"Make source command "<<cmd<<" failed! res:"<<res<<" errno="<<errno<<endl;
            mountdir.clear();
        }
    }
    return mountdir;
}
/******************************************************************************/  
string ImageDownloadAgent::MakeTargetMountDir(const ImageDownloadReq& req, const string& device, ostringstream& oss)
{
    oss<<"...Make target mountdir..."<<endl;
    string mountdir = TARGET_MOUNT_DIR;
    string cmd,res;
    if (check_fileexist(mountdir))
    {
        if (!is_directory(mountdir))
        {
            if (0 != unlink(mountdir.c_str()))
            {
                oss<<"Make target mountdir unlink file:"<<mountdir<<" failed ! errno="<<errno<<endl;
                return "";
            }
        }
    }
    if (!is_directory(mountdir))
    {
        cmd = "mkdir -p "+mountdir;
        if (SUCCESS != storage_command(cmd,res))
        {
            oss<<"Make target mountdir "<<cmd<<" failed! result:"<<res<<" errno="<<errno<<endl;
            return "";
        }
    }
    
    mountdir = TARGET_MOUNT_DIR + req._request_id;
    if (IMAGE_SOURCE_TYPE_BLOCK == req._option._img_srctype)
    {//块方式 这个device在重新加载后可能会变 建一个软链接
        if (0 != symlink(device.c_str(),mountdir.c_str()))
        {
            oss<<"Make target mountdir symlink failed! errno="<<errno<<endl;
            mountdir.clear();
        }
    }
    else
    {
        if (check_fileexist(mountdir))
        {
            if (!is_directory(mountdir))
            {
                if (0 != unlink(mountdir.c_str()))
                {
                    oss<<"Make target mountdir unlink "<<mountdir<<" failed! errno="<<errno<<endl;
                    return "";
                }
            }
            else
            {
                return mountdir;
            }
        }
        cmd = "mkdir -p "+mountdir;
        if (SUCCESS != storage_command(cmd,res))
        {
            oss<<"Make target mountdir "<<cmd<<" failed! res:"<<res<<" errno="<<errno<<endl;
            mountdir.clear();
        }
    }
    return mountdir;
}
/******************************************************************************/ 
STATUS ImageDownloadAgent::ReleaseNfsSource(const ImageDownloadReq& req, ostringstream& oss)
{
    oss<<"...Clear source..."<<endl;
    string mountdir = SOURCE_MOUNT_DIR+req._request_id;
    struct stat64 stat;
    if (0 == lstat64(mountdir.c_str(),&stat))
    {
        if(S_ISLNK(stat.st_mode))
        {
            if (0 != unlink(mountdir.c_str()))
            {
                oss<<"Unlink source "<<mountdir<<" failed! errno="<<errno<<endl;
                return ERROR;
            }
            else
            {
                oss<<"Unlink source "<<mountdir<<" success"<<endl;
            }
        }
        else if (S_ISDIR(stat.st_mode))
        {
            if (check_mount(mountdir.c_str()))
            {
                if (SUCCESS != UnMountNfs(mountdir))
                {
                    oss<<"Umount nfs source "<<mountdir<<" failed! errno="<<errno<<endl;
                    return ERROR;
                } 
                else
                {
                    oss<<"Umount nfs source "<<mountdir<<" success"<<endl;
                }
            }
            else
            {
                oss<<"Umount nfs source "<<mountdir<<" success for not mounted"<<endl;
            }
        }
        DeleteMountDir(mountdir,oss);  
    }
    else
    {
        if (ENOENT != errno)
        {
            oss<<"Clear source "<<mountdir<<" failed for lstat64! errno="<<errno<<endl;
            return ERROR;  
        }
        else
        {
            oss<<"Clear source "<<mountdir<<" success for not exist"<<endl;
        }
    }
    return SUCCESS;
}

/******************************************************************************/  
STATUS ImageDownloadAgent::PrepareNfsSource(const NfsAccessOption& option, 
                                              const string& path,
                                              string& mountdir, 
                                              string& source,
                                              ostringstream& oss)
{   
    oss<<"...Prepare nfs source..."<<endl;
    if (!check_mount(mountdir))
    {
        int ret = ERROR;
        ret = MountNfs(option.server_ip,option.shared_dir,mountdir,TO_FORCE_DIR);
        if (SUCCESS != ret)
        {
            oss<<"Prepare nfs source mount failed for mount nfs "<<option.server_ip<<":"<<option.shared_dir<<" to "<<mountdir<<" failed!,ret = "<<ret<<endl;
            return ret;
        }
        else
        {
            oss<<"Prepare nfs source mount success,mount nfs "<<option.server_ip<<":"<<option.shared_dir<<" to "<<mountdir<<endl;
        }
    }
    else
    {  
        oss<<"Prepare nfs source mount success for "<<mountdir<<" mounted "<<endl;
    }
    string nfs_server_uri = option.server_ip + ":" + option.shared_dir;
    source = path;
    source.erase(0,nfs_server_uri.size());
    source = mountdir + source;
    oss<<"Prepare nfs source result:"<<source<<endl;
    return SUCCESS;
}
/******************************************************************************/  
STATUS ImageDownloadAgent::PrepareSource(const ImageDownloadReq& req, string& source, ostringstream& oss)
{
    oss<<"..Prepare source.."<<endl;
    if (IMAGE_ACCESS_NFS == req._source_url.access_type)
    {
        NfsAccessOption option;
        if (false == option.deserialize(req._source_url.access_option))
        {
            oss<<"Prepare source deserialize nfs option failed"<<endl;
            return ERROR_INVALID_ARGUMENT;
        }
        if (true == IsLocalUrl(option.server_ip))
        {
            /* nfs server就是自己，不需要mount*/
            source = req._source_url.path.substr(req._source_url.path.find_first_of(':')+1);
            oss<<"Prepare source nfs server is self"<<endl;
            return SUCCESS;
        }
        string mountdir = MakeSourceMountDir(req,oss);
        if (!mountdir.empty())
        {
            string cmd = "mkdir -p "+mountdir;
            string result;
            if (SUCCESS != RunCmd(cmd, result))
            {
                oss<<"Prepare source "<<cmd<<" failed for "<<result<<endl;
                return ERROR;
            }
            return PrepareNfsSource(option,req._source_url.path,mountdir,source,oss);
        }
        else
        {
            oss<<"Prepare source failed for make mountdir! errno="<<errno<<endl;
            return ERROR;
        }
        
    }
    else if (IMAGE_ACCESS_HTTP == req._source_url.access_type)
    {
        oss<<"Prepare source failed for http access type not support now"<<endl;
        return ERROR_NOT_SUPPORT;
    }
    else
    {
        oss<<"Prepare source failed for unknown source access type "<<req._source_url.access_type<<endl;
        return ERROR;
    }
}
/******************************************************************************/  
STATUS ImageDownloadAgent::ReleaseSource(const ImageDownloadReq& req, ostringstream& oss)
{
    oss<<"..Release source.."<<endl;
    if (IMAGE_ACCESS_NFS == req._source_url.access_type)
    {
        NfsAccessOption option;
        if (false == option.deserialize(req._source_url.access_option))
        {
            oss<<"Release source deserialize nfs option failed"<<endl;
            return ERROR_INVALID_ARGUMENT;
        }
        if (true == IsLocalUrl(option.server_ip))
        {
            oss<<"Release source success nfs server is self"<<endl;
            return SUCCESS;
        }
        return ReleaseNfsSource(req,oss);
    }
    else if (IMAGE_ACCESS_HTTP == req._source_url.access_type)
    {
        oss<<"Http access type not support now"<<endl;
        return ERROR_NOT_SUPPORT;
    }
    else
    {
        oss<<"Unknown source access type:"<<req._source_url.access_type<<endl;
        return ERROR;
    }
}
/****************************************************************************************/
STATUS ImageDownloadAgent::MountTargetDisk(const ImageDownloadReq& req, const string& device, string &target, ostringstream& oss)
{
    oss<<"...Mount target disk..."<<endl;
    string mountdir = MakeTargetMountDir(req,device,oss);
    if (mountdir.empty())
    {
        return ERROR;
    }
    
    if (IMAGE_SOURCE_TYPE_BLOCK == req._option._img_srctype)
    {//块方式 这个device在重新加载后可能会变 建一个软链接
        target = mountdir;
    }
    else
    {
        if (!check_mount(mountdir))
        {
            if (0 != mount(device.c_str(),mountdir.c_str(),IMAGE_DEFAULT_FSTYPE,0,NULL))
            {
                oss<<"Mount target "<<device<<" to "<<mountdir<<" for "<<req._request_id<<" failed! errno="<<errno<<endl;
                return ERROR;
            }
        }
        target = mountdir + "/" + to_string<int64>(req._image_id,dec) + ".img.tmp";
    }
    
    oss<<"Mount "<<device<<" to "<<target<<" for "<<req._request_id<<" success" << endl;
    return SUCCESS;
}
/******************************************************************************/  
STATUS ImageDownloadAgent::UmountTargetDisk(const ImageDownloadReq& req, ostringstream& oss)
{
    oss<<"...Umount target disk..."<<endl;
    string mountdir = TARGET_MOUNT_DIR+req._request_id;
    if (IMAGE_SOURCE_TYPE_BLOCK == req._option._img_srctype)
    {
        struct stat64 stat;
        if (0 == lstat64(mountdir.c_str(),&stat))
        {
            if(S_ISLNK(stat.st_mode))
            {
                if (0 != unlink(mountdir.c_str()))
                {
                    oss<<"Unlink target disk "<<mountdir<<" failed! errno="<<errno<<endl;
                    return ERROR;
                }
                else
                {
                    oss<<"Unlink target disk "<<mountdir<<" success"<<endl;
                }
            }
        }
        else
        {
            if (ENOENT != errno)
            {
                oss<<"Unlink target disk "<<mountdir<<" failed for lstat64! errno="<<errno<<endl;
                return ERROR;
            }
            else
            {
                oss<<"Unlink target disk "<<mountdir<<" success for not exist"<<endl;
            }
        }
    }
    else
    {
        if (check_mount(mountdir))
        {
            if (0 != umount(mountdir.c_str()))
            {
                oss<<"Umount target disk "<<mountdir<<" failed! errno="<<errno<<endl;
                return ERROR;
            }
            else
            {
                oss<<"Umount target disk "<<mountdir<<" success"<<endl;
            }
        }
    }

    DeleteMountDir(mountdir,oss); 
    return SUCCESS;
}
/******************************************************************************/  
STATUS ImageDownloadAgent::PrepareTarget(const ImageDownloadReq& req, string& target, ostringstream& oss)
{
    oss<<"..Prepare target.."<<endl;
    STATUS ret = ERROR;
    if (IMAGE_ACCESS_ISCSI == req._target_url.access_type)
    {
        string device;
        ret = LoadShareDisk(-1,req._request_id,req._target_url.access_option,IMAGE_DEFAULT_FSTYPE,device);
        if (SUCCESS != ret && ret !=ERROR_LOAD_NEED_REFRESH)
        {
            oss<<"Load share disk for "<<req._request_id<<" failed!,ret="<<ret<<",option:"<<req._target_url.access_option<<endl;
            return ret; 
        }
        else
        {
            oss<<"Load share disk for "<<req._request_id<<" success"<<endl;
        }
        ret = MountTargetDisk(req,device,target,oss);
    }
    else
    {
        oss<<"UnSupport target access type:"<<req._target_url.access_type<<endl;
    }
    return ret;
}
/******************************************************************************/  
STATUS ImageDownloadAgent::ReleaseTarget(const ImageDownloadReq& req, ostringstream& oss)
{
    oss<<"..Release target.."<<endl;
    STATUS ret = SUCCESS;
    
    if (req._target_url.access_option.empty() || req._target_url.access_type.empty())
    {
        oss<<"UnLoad target disk success for empty target url"<<endl;
        return ret;
    }
    
    if (IMAGE_ACCESS_ISCSI == req._target_url.access_type)
    {
        ret = UmountTargetDisk(req,oss);
        if (SUCCESS != ret)
        {
            return ret;
        }
        ret = UnLoadShareDisk(-1,req._request_id,req._target_url.access_option);
        if (SUCCESS != ret)
        {
            oss<<"UnLoad target disk for "<<req._request_id<<" failed"<<endl;
        }
        else
        {
            oss<<"UnLoad target disk for "<<req._request_id<<" success"<<endl;
        }
    }
    else
    {
        oss<<"UnSupport target access type:"<<req._target_url.access_type<<endl;
    }
    return ret;
}
/******************************************************************************/  
STATUS ImageDownloadAgent::SetFinish(const ImageDownloadReq& req, const string& target, ostringstream& oss)
{
    oss<<"..Set finish.."<<endl;
    //1.完成文件下载(包括重命名或者设置标志) 
    if (IMAGE_SOURCE_TYPE_BLOCK == req._option._img_srctype)
    {//下载好后 块方式在块尾部写签名字节 表示该块上有完整的镜像
        if (SUCCESS != SetTailBytes(target, req._request_id.c_str(), req._request_id.size()))
        {
            oss<<"Set finish set "<<target<<" tail bytes failed! errno="<<errno<<endl;
            return ERROR;
        }
        else
        {
            oss<<"Set finish set "<<target<<" tail bytes success"<<endl;
        }
    }
    else
    {//文件方式 则将.img.tmp重命名回.img
        struct stat64 s;
        string target_ok = TARGET_MOUNT_DIR+req._request_id+ "/" + to_string<int64>(req._image_id,dec) + ".img";
        if (0 != lstat64(target_ok.c_str(),&s))
        {//.img文件不存在
            if (0 != lstat64(target.c_str(),&s) || s.st_size != req._size)
            {
                oss<<"Set finish lstat64 "<<target<<" failed! errno="<<errno<<endl;
                return ERROR;
            }
            else
            {//重命名.img.tmp文件为.img文件
                if (0 != rename(target.c_str(),target_ok.c_str()))  
                {
                    oss<<"Set finish rename "<<target<<" to "<<target_ok<<" failed! errno="<<errno<<endl;
                    return ERROR;
                }
                else
                {
                    oss<<"Set finish rename "<<target<<" to "<<target_ok<<" success"<<endl;
                }
            }
        }
        else
        {
            if (s.st_size != req._size)
            {//.img文件存在 但大小不等
                oss<<"Set finish file "<<target_ok<<" exist but size mismatch"<<endl;
                return ERROR;
            }
            else
            {
                oss<<"Set finish file "<<target_ok<<" exist and size match"<<endl;
            }
        }
    }
    return SUCCESS;
}
/******************************************************************************/  
bool ImageDownloadAgent::CheckFinish(const ImageDownloadReq& req, const string& target, ostringstream& oss)
{
    oss<<"..Check finish.."<<endl;
    if (IMAGE_SOURCE_TYPE_BLOCK == req._option._img_srctype)
    {//块方式
        char acBuf[1024];
        int64 size = req._request_id.size();
        if (SUCCESS != GetTailBytes(target, acBuf, size))
        {
            SkyAssert(false);
            oss<<"Check finish failed for get tail bytes"<<endl;
            return false;
        }
        acBuf[size] = 0;
        if (0 != req._request_id.compare(acBuf))
        {
            oss<<"Check finish unfind "<<req._request_id<<endl;
            return false;
        }
        else
        {
            oss<<"Check finish ready for found "<<req._request_id<<endl;
        }
    }
    else
    {
        struct stat64 s;
        string target_ok = TARGET_MOUNT_DIR+req._request_id+ "/" + to_string<int64>(req._image_id,dec) + ".img";
        if (0 != lstat64(target_ok.c_str(),&s) || s.st_size != req._size)
        {
            oss<<"Check finish unfind "<<target_ok<<endl;
            return false;
        }
        else
        {
            oss<<"Check finish ready for found "<<target_ok<<endl;
        }
    }
    return true;
}
/******************************************************************************/  
STATUS ImageDownloadAgent::StartDownload(const ImageDownloadReq& req, const string& source, const string& target, ostringstream& oss)
{
    oss<<"..Start download.."<<endl;
    if (_tpool->FindWork(req._request_id))
    {
        oss<<"Image download start success for task "<<req._request_id<<" exist"<<endl;
        return SUCCESS;
    }
    //创建新下载任务，首先加入工作看板
    ImageDownloadPara para(source,req._size,req._source_url.checksum,target,req._option);
    ImageDownloadTask* task = new ImageDownloadTask(req._request_id,para,req);
    if (!task)
    {
        oss<<"New image download task failed"<<endl;
        return ERROR_NO_MEMORY;
    }

    if (SUCCESS != task->Init())
    {
        oss<<"Image download task init failed"<<endl;
        return ERROR;
    }
    
    STATUS ret = _tpool->AssignWork(task);
    if (SUCCESS != ret)
    {
        oss<<"Start image download task:"<<req._request_id<<" failed! ret="<<ret<<endl;
    }
    else
    {
        oss<<"Start image download task:"<<req._request_id<<" success"<<endl;
    }
    return ret;
}
/******************************************************************************/  
STATUS ImageDownloadAgent::KillProcess(const string& process,const string& tag, ostringstream& oss)
{
    oss<<"...Kill "<<process<<" process dist by "<<tag<<"..."<<endl;
    if (process.empty() || tag.empty())
    {
        return ERROR_INVALID_ARGUMENT;
    }
    STATUS ret = SUCCESS;
    pid_t pid = -1;
    string cmd,res,pidlist,pidfound;
    cmd = "ps x | grep "+process+" | grep "+tag+" | grep -v grep | awk '{ print $1}'";
    storage_command(cmd,pidlist);
    stringstream iss(pidlist);
    while (getline(iss,pidfound))
    {
        from_string<pid_t>(pid,pidfound,dec);
        ret = kill(pid,9);
        if (0 != ret)
        {
            oss<<"KillProcess "<<pid<<" failed! errno="<<errno<<endl;
            ret = ERROR;
        }
        else
        {
            oss<<"KillProcess "<<pid<<" success"<<endl;
        }
    }

    return ret;
}
/******************************************************************************/  
STATUS ImageDownloadAgent::StopDownload(const ImageDownloadReq& req, ostringstream& oss)
{
    oss<<"..Stop download.."<<endl;
    STATUS ret = SUCCESS;
    ImageDownloadCtl ctl;
    if (QueryList(req._request_id,ctl)
        && ctl._request_id == req._request_id 
        && -1 != ctl._pid)
    {
        if (0 != kill(ctl._pid,9))
        {
            oss<<"Kill process "<<ctl._pid<<" failed! errno="<<errno<<endl;
            ret = ERROR;
        }
        else
        {
            oss<<"Kill process "<<ctl._pid<<" success"<<endl;
        }
    }
    else
    {
        if (SUCCESS != KillProcess("md5sum",TARGET_MOUNT_DIR + req._request_id,oss))
        {
            ret = ERROR;
        }
        if (SUCCESS != KillProcess("curl",SOURCE_MOUNT_DIR + req._request_id,oss))
        {
            ret = ERROR;
        }
    }
    return ret;
}
/******************************************************************************/  
bool ImageDownloadAgent::CheckCtlList(const ImageDownloadReq& req, ImageDownloadAck& ack, ostringstream& oss)
{
    oss<<"..Check ctl list.."<<endl;
    ImageDownloadCtl ctl;
    bool ret = QueryList(req._request_id,ctl);
    if (ret)
    {
        switch(ctl._state)
        {
            case IDS_INIT:
            case IDS_DOWNLOADING:
            case IDS_CHECKSUMING:
            case IDS_FINISH:
                ack.progress = ctl._progress;
                ack.state = ERROR_ACTION_RUNNING;
                break;
            case IDS_READY:
                ack.progress = 100;
                ack.state = SUCCESS;
                break;
            case IDS_ERROR:
            case IDS_UNKNOWN:
            case IDS_CANCELING:
                ack.state = ERROR;
                break;
            default:
                ack.state = ERROR;
                break;
        }
        ack.detail = ctl._detail;
        oss<<"Image download request:"<<req._request_id<<" in ctl list"<<endl;
    }
    else
    {
        oss<<"Image download request:"<<req._request_id<<" not in ctl list"<<endl;
    }
    return ret;
}
/******************************************************************************/  
bool ImageDownloadAgent::CheckReqValid(const ImageDownloadReq& req, ostringstream& oss)
{
    if (req._request_id.empty()
        || INVALID_IID == req._image_id
        || 0 == req._size
        || !req._option.IsValid()
        || req._source_url.access_type.empty()
        || req._source_url.access_option.empty()
        || req._target_url.access_type.empty()
        || req._target_url.access_option.empty())
    {
        oss<<"Image download request invalid:"
           <<"request_id = " << req._request_id 
           <<",image_id = " << req._image_id 
           <<",size = " << req._size
           <<",spcexp = " << req._option._img_spcexp
           <<",srctype = " << req._option._img_srctype
           <<",usage = " << req._option._img_usage
           <<",share_usage = " << req._option._share_img_usage
           <<",src type = " << req._source_url.access_type 
           <<",src option = " << req._source_url.access_option
           <<",tgt type = " << req._target_url.access_type
           <<",tgt option = " << req._target_url.access_option 
           << endl ;
        return false;
    }
    oss<<"Image download request:"<<req._request_id<<" valid"<<endl;
    return true;
}
/******************************************************************************/  
STATUS ImageDownloadAgent::PrepareDownloadImage(const ImageDownloadReq& req)
{
    STATUS ret = SUCCESS;
    ostringstream oss;
    string source,target;
    ImageDownloadAck ack(req.action_id,req._request_id);
    MID sender = m->CurrentMessageFrame()->option.sender();
    
    oss<<".Prepare download image."<<endl;
    if (!CheckReqValid(req, oss))
    {
        ret = ERROR;
        ack.state = ERROR;
        ack.detail = oss.str();
        goto MSG_RET;
    }
    if (CheckCtlList(req,ack,oss))
    {
        goto MSG_RET;
    }
    ret = PrepareTarget(req,target,oss);
    if (SUCCESS != ret)
    {
        goto ERROR_RET;
    }
    if (CheckFinish(req,target,oss))
    {
        ReleaseTarget(req,oss);
        ack.progress = 100;
        ack.state = SUCCESS;
        goto MSG_RET;
    }
    ret = PrepareSource(req,source,oss);
    if (SUCCESS != ret)
    {
        goto ERROR_RET;
    }
    ret = StartDownload(req,source,target,oss);
    
ERROR_RET:
    if (SUCCESS != ret)
    {
        ack.state = ERROR;
    }
    else
    {
        ack.progress = 0;
        ack.state = ERROR_ACTION_RUNNING;
    }
    ack.detail = oss.str();
    AddSender(req._request_id,sender);
MSG_RET:
    if (SUCCESS != ret)
    {
        OutPut(SYS_ERROR,"%s\n",oss.str().c_str());
    }
    else
    {
        OutPut(SYS_DEBUG,"%s\n",oss.str().c_str());
    }
    MessageOption option(sender,EV_IMAGE_DOWNLOAD_ACK,0,0);
    return m->Send(ack,option); 
}
/******************************************************************************/  
STATUS ImageDownloadAgent::ReleaseDownloadImage(const ImageDownloadReq& req)
{
    STATUS ret = SUCCESS;
    string source,target;
    ostringstream oss;
    ImageDownloadAck ack(req.action_id,req._request_id);
    
    oss<<".Release download image."<<endl;
    #if 0
    if (!CheckReqValid(req, oss))
    {
        ret = ERROR;
        goto MSG_RET;
    }
    #endif
    UpdateList(req._request_id, IDS_CANCELING, "CANCELING");
    ret = StopDownload(req,oss);
    if (SUCCESS != ret)
    {
        goto MSG_RET;
    }
    ret = ReleaseTarget(req,oss);
    if (SUCCESS != ret)
    {
        goto MSG_RET;
    }
    ret = ReleaseSource(req,oss);
    if (SUCCESS != ret)
    {
        goto MSG_RET;
    } 
    RemoveSender(req._request_id);
    RemoveFromList(req._request_id);
    ack.state = SUCCESS;
    ack.progress = 100;
MSG_RET:
    ack.detail = oss.str();
    if (SUCCESS != ret)
    {
        ack.state = ERROR;
        OutPut(SYS_ERROR,"%s\n",oss.str().c_str());
    }
    else
    {
        OutPut(SYS_DEBUG,"%s\n",oss.str().c_str());
    }
    MID sender = m->CurrentMessageFrame()->option.sender();
    MessageOption option(sender,EV_IMAGE_CANCEL_DOWNLOAD_ACK,0,0);
    return m->Send(ack,option);
}
/******************************************************************************/  
STATUS ImageDownloadAgent::CalcProgress(const ImageDownloadCtl& ctl, int& progress)
{
    _current_query_times ++;
    progress = ctl._progress;
    if (IMAGE_SOURCE_TYPE_BLOCK == ctl._para._option._img_srctype)
    {//块方式 每5秒左右更新一次进度 进度更新到90 大于90还没下载完 则进度停在90
        if (QUERY_STATIS_INTERVAL <= _current_query_times - _last_query_times)
        {
            if (progress < 90)
            {
                progress ++;
            }
            _last_query_times = _current_query_times;
        }
    }
    else
    {//文件方式 根据已经下载的文件大小 计算进度
        struct stat64 s;
        if (0 != lstat64(ctl._para._target.c_str(),&s))
        {
            OutPut(SYS_ERROR,"CalcProgress lstat64 %s failed or download not start for %s!\n",ctl._para._target.c_str(), strerror(errno));
            return ERROR;
        }
        if (ctl._para._size <= 0)
        {
            SkyAssert(false);
            OutPut(SYS_ERROR,"CalcProgress failed for zero size!\n");
            return ERROR;
        }
        int tmp_progress = (s.st_size * 100) / ctl._para._size;
        if (tmp_progress < 100)
        {//下载进度更新到85
            progress = (tmp_progress * 85) / 100;
        }
        else
        {//等于100 则在计算校验和 每20秒左右进度加1
            if (5*QUERY_STATIS_INTERVAL <= _current_query_times - _last_query_times)
            {//85~99为计算校验和进度 如果到了99 校验和还没计算完 则停在99
                if (progress < 99)
                {
                    progress++;
                }
                _last_query_times = _current_query_times;
            }
        }
    }
    return SUCCESS;
}
/******************************************************************************/  
void ImageDownloadAgent::QueryDownloadImage(void)
{
    MutexLock lock(_mutex);
    if (0 == _ctl_list.size())
    {
        return;
    }
    bool jump = false;
    int progress = 0;
    MID sender;
    ostringstream oss;
    ImageDownloadAck ack;

    oss<<".Query download image."<<endl;
    vector<ImageDownloadCtl>::iterator it = _ctl_list.begin();
    do
    {
        jump = false;
        switch(it->_state)
        {
            case IDS_INIT:
            case IDS_DOWNLOADING:
            case IDS_CHECKSUMING:
                if (SUCCESS == CalcProgress(*it, progress))
                {//调用UpdateProgress更新进度 防止队列变了?
                    UpdateProgress(it->_request_id, progress);
                }
                break;
            case IDS_FINISH:
                if (SUCCESS != SetFinish(it->_req, it->_para._target, oss))
                {
                    UpdateList(it->_request_id, it->_state, oss.str());
                    break;
                }
                if (SUCCESS != ReleaseTarget(it->_req,oss))
                {//下载成功了 但是清理失败了 更新一下detail 下次继续尝试
                    UpdateList(it->_request_id, it->_state, oss.str());
                    break;
                }
                if (SUCCESS != ReleaseSource(it->_req,oss))
                {//下载成功了 但是清理失败了 更新一下detail 下次继续尝试
                    UpdateList(it->_request_id, it->_state, oss.str());
                    break;
                }
                //清理成功 进度为100 并主动发消息
                UpdateProgress(it->_request_id, 100);
                ack._request_id = it->_request_id;
                ack.state = SUCCESS;
                ack.progress = 100;
                ack.detail = "OK";
                UpdateList(it->_request_id, IDS_READY,ack.detail);
                if (GetSender(it->_request_id,sender))
                {
                    MessageOption option(sender,EV_IMAGE_DOWNLOAD_ACK,0,0);
                    if (SUCCESS == m->Send(ack,option))
                    {//发消息成功了
                        RemoveSender(it->_request_id);
                        _ctl_list.erase(it);
                        jump = true;
                    }
                }
                OutPut(SYS_DEBUG,"%s",oss.str().c_str());
                break;
            case IDS_READY:
                //上次消息没发成功?
                ack.state = SUCCESS;
                ack.progress = 100;
                ack.detail = "OK";
                if (GetSender(it->_request_id,sender))
                {
                    MessageOption option(sender,EV_IMAGE_DOWNLOAD_ACK,0,0);
                    if (SUCCESS == m->Send(ack,option))
                    {//发消息成功了
                        RemoveSender(it->_request_id);
                        _ctl_list.erase(it);
                        jump = true;
                    }
                }
                break;
            case IDS_CANCELING:
                //上次cancel失败了发出去的应答可能丢了 
                ack.state = ERROR;
                if (GetSender(it->_request_id,sender))
                {
                    MessageOption option(sender,EV_IMAGE_DOWNLOAD_ACK,0,0);
                    m->Send(ack,option);
                }
                break;
            case IDS_ERROR:
            case IDS_UNKNOWN:
                break;
            default:
                break;
        }
        if (!jump)
        {
            it++;
        }
    }while(it != _ctl_list.end());
}
/******************************************************************************/  
void ImageDownloadAgent::MessageEntry(const MessageFrame& message)
{
    ostringstream error;
    switch(m->get_state())
    {
        case S_Startup:
        {
            switch(message.option.id())
            {
                case EV_POWER_ON:
                {
                    _timer = m->CreateTimer();
                    if (INVALID_TIMER_ID == _timer)
                    {
                        OutPut(SYS_ALERT, "CreateTimer failed in ImageDownloadAgent::MessageEntry()!\n");
                        SkyExit(-1,"CreateTimer failed in ImageDownloadAgent::MessageEntry()!\n");
                    }
                    
                    TimeOut timeout;
                    timeout.type = LOOP_TIMER;
                    timeout.duration = 2000;    // 默认定时1S
                    timeout.msgid = EV_TIMER_QUERY;
                    STATUS rc = m->SetTimer(_timer, timeout);
                    if (SUCCESS != rc)
                    {
                        OutPut(SYS_ALERT, "ImageDownloadAgent SetTimer failed rc = %d!\n",rc);
                        SkyAssert(false);
                    }
                    m->set_state(S_Work);
                    OutPut(SYS_NOTICE, "%s power on!\n",m->name().c_str());
                    break;
                }
                default:
                    break;
            }
            break;
        }

        case S_Work:
        {
            switch(message.option.id())
            {
                case EV_IMAGE_DOWNLOAD_REQ:
                {
                    ImageDownloadReq req;
                    if(req.deserialize(message.data))
                    {
                        PrepareDownloadImage(req);
                    }
                    else
                    {
                        OutPut(SYS_ERROR,"failed to deserialize ImageDownloadReq message:\n%s\n",
                            message.data.c_str());
                    }
                    break;
                }
                case EV_IMAGE_CANCEL_DOWNLOAD_REQ:
                {
                    ImageDownloadReq req;
                    if(req.deserialize(message.data))
                    {
                        ReleaseDownloadImage(req);
                    }
                    else
                    {
                        OutPut(SYS_ERROR,"failed to deserialize ImageDownloadReq message:\n%s\n",
                            message.data.c_str());
                    }
                    break;
                }
                case EV_TIMER_QUERY:
                {
                    QueryDownloadImage();
                    break;
                }
                default:
                    break;
            }
        }
    }
}
/******************************************************************************/
void DbgShowImageDownload()
{
    ImageDownloadAgent *pInst = ImageDownloadAgent::GetInstance();
    if (NULL == pInst)
    {
        cout << "Get image download agent instance failed!"<<endl;
        return;
    }
    pInst->Print();
}
DEFINE_DEBUG_CMD(imagedownloading,DbgShowImageDownload);

}

