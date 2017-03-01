/******************************************************************************
* Copyright (c) 2012������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�project_image.cpp
* �ļ���ʶ��
* ����ժҪ�����̾������ʵ���ļ�
* ��ǰ�汾��1.0
* ��    �ߣ���ΰ
* ������ڣ�2013��3��12��
*
* �޸ļ�¼1��
*     �޸����ڣ�2013/3/12
*     �� �� �ţ�V1.0
*     �� �� �ˣ���ΰ
*     �޸����ݣ�����
******************************************************************************/
#include "project_image.h"
#include "log_agent.h"
#include "storage.h"
#include "workflow.h"
#include "misc.h"
#include "event.h"
#include <sys/types.h>
#include <sys/mount.h>
#include <dirent.h>
#include <fstream> 
#include "image_store.h"
#include "image_pool.h"
#include "user_pool.h"
#include "license_common.h"
#include "db_config.h"

namespace zte_tecs
{
#define CLEAR_AND_SEND_ACK(msg, ack) \
        if (GetSender(it->_request_id,sender))\
        {\
            MessageOption option(sender,msg,0,0);\
            if (SUCCESS == m->Send(ack,option))\
            {\
                RemoveSender(it->_request_id);\
                ReleaseTempFiles(ack._request_id,oss);\
                _ctl_list.erase(it);\
                jump = true;\
            }\
        }\
/******************************************************************************/  
STATUS TarballTask::Init()
{
    _work_thread_id = INVALID_THREAD_ID;
    _agent = ProjectImageAgent::GetInstance();
    if (NULL == _agent)
    {
        return ERROR;
    }
    return SUCCESS;
}
/******************************************************************************/  
void TarballTask::Print()
{
    if (_work_thread_id == INVALID_THREAD_ID)
    {
        cout << "task " << getName() << " is still waiting in queue." << endl;
        return;
    }
    
    cout << " tarball task " << getName() << " id = 0x" << hex << _work_thread_id << endl;
    cout << "dir: " << _para._dir << endl;
    cout << "file: " << _para._file << endl;

    struct stat64 s;
    if (0 == lstat64(_para._file.c_str(),&s))
    {
        cout << "file size : " << dec << s.st_size << " bytes" << endl;
    }
}
/******************************************************************************/  
int TarballTask::do_system(const string& command)
{
    int ret = ERROR;
    pid_t pid = -1;

    if (command.empty())
    {
        return ret;
    }

    if ((pid = fork()) < 0)
    {
        return ret;
    }
    else if (0 == pid)
    {
        execl("/bin/sh","sh","-c",command.c_str(),(char *)0);
    }
    else
    {
        int status = -1;
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
           w = waitpid(pid, &status, WUNTRACED | WCONTINUED);
           if (w == -1) {
               ret = ERROR;
               break;
           }
           if (WIFEXITED(status)) {
               ret = WEXITSTATUS(status);
           } else if (WIFSIGNALED(status) || WIFSTOPPED(status)) {
               ret = ERROR;
           } 
        } while (!WIFEXITED(status) && !WIFSIGNALED(status) && !WIFSTOPPED(status));
        _agent->UpdateList(getName(), -1);
    }

    return ret;
}
/******************************************************************************/  
STATUS TarballTask::Execute(void)
{
    STATUS ret = ERROR;
    ostringstream oss;
    
    _work_thread_id = pthread_self();
    //���̳߳��߳���� ��ֹ�����Ѿ���� ���̳߳��߳����������
    //������ӵ�ctl�ͽ�����
    ProjectImageCtl ctl(getName(),_req, _para);
    if (_agent->QueryAndAddList(getName(),ctl))
    {//���ƶ��������Ѿ����� ˵���Ѿ��а��˹��ɹ����˶����Ѿ������
        OutPut(SYS_NOTICE,"Task %s is done,I am unnecessary\n",getName().c_str());
        return SUCCESS;
    }
    
    _agent->UpdateList(getName(), PIS_SAVE_TARBALLING, "Start Save Tarballing");
    string command = "cd "+ _para._dir + ";tar zcvf " + _para._file + " * ; cd -";
    oss << ".Task "<<_work_thread_id<<" is running command: \""<<command<<"\"!."<<endl;
    ret = do_system(command.c_str());
    if (-1 == ret)
    {
        oss<<".system command ["<<command<<"] failed! errno = "<<errno<<endl;
        goto EXEC_RET;
    }
    else if (0 != ret)
    {
        oss<<".system command ["<<command<<"] returns "<<ret<<"."<<endl;
        goto EXEC_RET;
    }
    
    oss<<".Tarball "<<_agent->GetImageDir()<<" to "<<_para._file<<" finish!."<<endl;
    ret = SUCCESS;
    
EXEC_RET:    
    if (SUCCESS == ret)
    {
        OutPut(SYS_DEBUG,"%s\n",oss.str().c_str());
        _agent->UpdateList(getName(), PIS_SAVE_FINISH, oss.str());
    }
    else
    {
        OutPut(SYS_ERROR,"%s\n",oss.str().c_str());
        _agent->UpdateList(getName(), PIS_SAVE_ERROR, oss.str());
    }
    Datetime dt;
    _agent->UpdateList(getName(), dt);
    return ret;
}
/******************************************************************************/  
STATUS UnTarballTask::Init()
{
    _work_thread_id = INVALID_THREAD_ID;
    _agent = ProjectImageAgent::GetInstance();
    if (NULL == _agent)
    {
        return ERROR;
    }
    return SUCCESS;
}
/******************************************************************************/  
void UnTarballTask::Print()
{
    if (_work_thread_id == INVALID_THREAD_ID)
    {
        cout << "task " << getName() << " is still waiting in queue." << endl;
        return;
    }
    
    cout << " untarball task " << getName() << " id = 0x" << hex << _work_thread_id << endl;
    cout << "dir: " << _para._dir << endl;
    cout << "file: " << _para._file << endl;
}
/******************************************************************************/  
int UnTarballTask::do_system(const string& command)
{
    int ret = ERROR;
    pid_t pid = -1;

    if (command.empty())
    {
        return ret;
    }

    if ((pid = fork()) < 0)
    {
        return ret;
    }
    else if (0 == pid)
    {
        execl("/bin/sh","sh","-c",command.c_str(),(char *)0);
    }
    else
    {
        int status = -1;
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
           w = waitpid(pid, &status, WUNTRACED | WCONTINUED);
           if (w == -1) {
               ret = ERROR;
               break;
           }
           if (WIFEXITED(status)) {
               ret = WEXITSTATUS(status);
           } else if (WIFSIGNALED(status) || WIFSTOPPED(status)) {
               ret = ERROR;
           } 
        } while (!WIFEXITED(status) && !WIFSIGNALED(status) && !WIFSTOPPED(status));
        _agent->UpdateList(getName(), -1);
    }

    return ret;
}
/******************************************************************************/  
STATUS UnTarballTask::Execute(void)
{
    STATUS ret = ERROR;
    ostringstream oss;
    
    _work_thread_id = pthread_self();
    //���̳߳��߳���� ��ֹ�����Ѿ���� ���̳߳��߳����������
    //������ӵ�ctl�ͽ�����
    ProjectImageCtl ctl(getName(),_req, _para);
    if (_agent->QueryAndAddList(getName(),ctl))
    {//���ƶ��������Ѿ����� ˵���Ѿ��а��˹��ɹ����˶����Ѿ������
        OutPut(SYS_NOTICE,"Task %s is done,I am unnecessary\n",getName().c_str());
        return SUCCESS;
    }
    
    _agent->UpdateList(getName(), PIS_IMPORT_TARBALLING, "Start Import Tarballing");
    string command = "cd "+ _para._dir + ";tar zxvf " + _para._file + " ;cd - ";
    oss << ".Task "<<_work_thread_id<<" is running command: \""<<command<<"\"!."<<endl;
    ret = do_system(command.c_str());
    if (-1 == ret)
    {
        oss<<".system command ["<<command<<"] failed! errno = "<<errno<<"."<<endl;
        goto EXEC_RET;
    }
    else if (0 != ret)
    {
        oss<<".system command ["<<command<<"] returns "<<ret<<"."<<endl;
        goto EXEC_RET;
    }
    
    oss<<".UnTarball "<<_para._file<<" to "<<_agent->GetImageDir()<<" finish!."<<endl;
    ret = SUCCESS;
    
EXEC_RET:    
    if (SUCCESS == ret)
    {
        OutPut(SYS_DEBUG,"%s\n",oss.str().c_str());
        _agent->UpdateList(getName(), PIS_IMPORT_FINISH, oss.str());
    }
    else
    {
        OutPut(SYS_ERROR,"%s\n",oss.str().c_str());
        _agent->UpdateList(getName(), PIS_IMPORT_ERROR, oss.str());
    }
    Datetime dt;
    _agent->UpdateList(getName(), dt);
    return ret;
}
/******************************************************************************/  
ProjectImageAgent* ProjectImageAgent::_instance = NULL;
/******************************************************************************/  
STATUS ProjectImageAgent::Init()
{
    STATUS ret = ERROR;
    ret = _mutex.Init();
    if (SUCCESS != ret)
    {
        OutPut(SYS_ERROR,"Init Mutex Failed!\n");
        return ret;
    }
    _tpool = new ThreadPool(MAX_PROJECT_IMAGE_TASKS);
    if (!_tpool)
    {
        OutPut(SYS_EMERGENCY,"failed to create ThreadPool!\n");
        return ERROR;
    }
    ret = _tpool->CreateThreads("ProjectImage");
    if (SUCCESS != ret)
    {
        OutPut(SYS_EMERGENCY,"failed to start ThreadPool threads! ret = %d\n",ret);
        return ret;
    }
    ImageStore* store = ImageStore::GetInstance();
    if (NULL == store)
    {
        OutPut(SYS_EMERGENCY,"failed to get image store!\n");
        return ERROR;
    }
    ret = store->GetStorageDir(_image_dir);
    if (SUCCESS != store->GetStorageDir(_image_dir))
    {
        OutPut(SYS_EMERGENCY,"failed to get image store dir\n");
        return ret;
    }
    ostringstream oss;
    KillProcess("tar", _image_dir, oss);
    OutPut(SYS_DEBUG,"project image init kill process\n");
    OutPut(SYS_DEBUG,"%s\n",oss.str().c_str());
    MakeTempDir(TECS_TEMP_DIR);
    RmSoftlinkInTempDir(TECS_TEMP_DIR);
    _timer = INVALID_TIMER_ID;
    _last_query_times = 0;
    _current_query_times = 0;

    ret = Start(MU_PROJECT_IMAGE_AGENT);
    if(SUCCESS != ret)
    {
        return ret;
    }  
    
    //���Լ���һ���ϵ�֪ͨ��Ϣ
    MessageOption option(MU_PROJECT_IMAGE_AGENT,EV_POWER_ON,0,0);
    MessageFrame frame(SkyInt(0),option);
    return m->Receive(frame);
}
/******************************************************************************/  
string ProjectImageAgent::MakeUserDir(const string& user_name)
{
    string dir = _image_dir + "/" + user_name;
    if (!is_directory(dir))
    {
        string cmd,res;
        cmd = "mkdir -p "+dir;
        if (0 != RunCmd(cmd,res))
        {
            return "";
        }
    }
    return dir;
}
/******************************************************************************/  
string ProjectImageAgent::File2Location(const string& user_name,const string& file_name)
{
    return MakeUserDir(user_name) + "/" + file_name;
}
/******************************************************************************/  
void ProjectImageAgent::RmSoftlinkInTempDir(const string& dir)
{
    if (!is_directory(dir))
    {
        return;
    }
    DIR *pDir = opendir(dir.c_str());
    if (NULL == pDir)
    {
        OutPut(SYS_ERROR,"Open %s failed for %s\n",dir.c_str(),strerror(errno));
        return;
    }

    struct stat64 stat;
    char buf[MAX_IMAGE_NAME_LENGTH] = {'\0'};
    string file_name,file_full_name,linked_file;
    string cmd,res;
    struct dirent *pDirent;
    for(;;)
    {
        pDirent = readdir(pDir);
        if (NULL == pDirent)
        {
            break;
        }
        
        if ((0 == strcmp(pDirent->d_name, "."))
             || (0 == strcmp(pDirent->d_name, "..")))
        {
            continue;
        }

        file_name = pDirent->d_name;
        file_full_name = dir + "/" + file_name;
        lstat64(file_full_name.c_str(),&stat);
        if (!S_ISLNK(stat.st_mode))
        {
            continue;
        }
        readlink(file_full_name.c_str(),buf,sizeof(buf));
        buf[MAX_IMAGE_NAME_LENGTH-1] = 0;
        linked_file = buf;
        cmd = "rm -rf " + linked_file;
        RunCmd(cmd,res);
        cmd = "rm -rf " + file_full_name;
        RunCmd(cmd,res);
    }
    closedir(pDir);
    return;
}
/******************************************************************************/  
STATUS ProjectImageAgent::MakeSoftlink2TempDir(const string& request_id, const string& dir)
{
    string softlink = TECS_TEMP_DIR;
    string cmd,res;
    struct stat64 stat;
    char buf[MAX_IMAGE_NAME_LENGTH] = {'\0'};

    softlink += "/" + request_id;
    if (check_fileexist(softlink))
    {
        lstat64(softlink.c_str(),&stat);
        if (S_ISLNK(stat.st_mode))
        {
            readlink(softlink.c_str(),buf,sizeof(buf));
            buf[MAX_IMAGE_NAME_LENGTH-1] = 0;
            if (!dir.compare(buf))
            {
                return SUCCESS;
            }
        }
        cmd = "rm -f " + softlink;
        if (0 != RunCmd(cmd,res))
        {
            return ERROR;
        }
    }
    cmd = "ln -s "+dir+" "+softlink;
    if (0 != RunCmd(cmd,res))
    {
        return ERROR;
    }
    return SUCCESS;
}
/******************************************************************************/  
STATUS ProjectImageAgent::MakeTempDir(const string& dir)
{
    if (!is_directory(dir))
    {
        string cmd,res;
        cmd = "rm -rf "+dir;
        if (0 != RunCmd(cmd,res))
        {
            return ERROR;
        }
        cmd = "mkdir -p "+dir;
        if (0 != RunCmd(cmd,res))
        {
            return ERROR;
        }
    }
    return SUCCESS;
}
/******************************************************************************/  
string ProjectImageAgent::MakeTempDir(const string& request_id, ostringstream& oss)
{
    string dir = _image_dir + "/" + request_id;

    if (!is_directory(dir))
    {
        string cmd,res;
        cmd = "rm -rf "+dir;
        if (0 != RunCmd(cmd,res))
        {
            return "";
        }
        cmd = "mkdir -p "+dir;
        if (0 != RunCmd(cmd,res))
        {
            return "";
        }
        oss<<"...Make temp dir("<<dir<<") success..."<<endl;
    }
    else
    {

        oss<<"...Make temp dir success for "<<dir<<" already exist..."<<endl;
    }
    MakeSoftlink2TempDir(request_id,dir);
    return dir;
}
/******************************************************************************/  
STATUS ProjectImageAgent::ClearTempDir(const string &dir, ostringstream& oss)
{
    STATUS ret = SUCCESS;
    string cmd,res;
    cmd = "rm -rf "+dir+"/*";
    if (0 != RunCmd(cmd,res))
    {
        oss<<"..Clear temp dir failed,cmd:"<<cmd<<",res:"<<res<<".."<<endl;
        ret = ERROR;
    }
    else
    {
        oss<<"...Clear temp dir success..."<<endl;
    }
    return ret;
}
/******************************************************************************/  
STATUS ProjectImageAgent::GenerateLstFile(const ExportProjectImageReq& req,const string &dir, ostringstream& oss)
{
    oss<<"....Generate lst file...."<<endl;
    string lst_file = dir+"/"+req._request_id+IMAGE_TPI_LST_SUFFIX;
    
    ofstream fout(lst_file.c_str());
    if(!fout.is_open())
    {
        oss<<"....Open file "<<lst_file<<" failed...."<<endl;
        return ERROR;
    }
    fout.close();
    oss<<"....Generate lst file "<<lst_file<<" success...."<<endl;
    return SUCCESS;
}
/******************************************************************************/  
STATUS ProjectImageAgent::GenerateCfgFile(const ExportProjectImageReq& req, const vector<Image>& images, const string &dir, ostringstream& oss)
{
    oss<<"....Generate cfg file...."<<endl;
    string cfg_file = dir+"/"+req._request_id+IMAGE_TPI_CFG_SUFFIX;
    
    ofstream fout;
    fout.open(cfg_file.c_str());
    if(!fout.is_open())
    {
        oss<<"....Open file "<<cfg_file<<" failed...."<<endl;
        return ERROR;
    }

    TecsProjectImageConfig tpi_cfg(req._project,req._vms,req._repels,req._affregs,images);
    
    fout<<tpi_cfg.serialize();
    
    fout.close();
    
    oss<<"....Generate cfg file "<<cfg_file<<" success...."<<endl;
    return SUCCESS;
}
/******************************************************************************/  
STATUS ProjectImageAgent::GatherImagesAndWriteCfgFile(const ExportProjectImageReq& req, const string &dir, ostringstream& oss)
{
    STATUS ret = ERROR;
    oss<<"....Generate image file...."<<endl;
    ExportProjectImageReq tmp_req(req.action_id,
                              req._request_id, 
                              req._uid, 
                              req._user_name,
                              req._project,
                              req._vms, 
                              req._repels,
                              req._affregs,
                              req._description);
    TecsProjectImageFile tpi_file;
    TecsProjectImageFileList tpi_file_list;
    
    string image_file_prefix = dir + "/";
    string dup_file_name,file_full_name,location,dup_location;
    vector<Image> images;
    vector<VmCfg>::iterator it_vms;
    vector<VmBaseImage>::iterator it_images; 
    Image image,dup_image;
    ImagePool *ipool = ImagePool::GetInstance();
    string cmd,res;
    FileUrl url;
    int64 image_id = INVALID_IID;
    map<string, int64> name_id_map; 
    for (it_vms = tmp_req._vms.begin(); it_vms != tmp_req._vms.end(); it_vms++)
    {
        for (it_images = it_vms->_images.begin(); it_images != it_vms->_images.end(); it_images++)
        {
            if (INVALID_IID == it_images->_image_id)
            {
                //����Ҫ���澵��
                continue;
            }
            //image_file = image_file_prefix+to_string<int64>(it_images->_image_id,dec)+IMAGE_TPI_IMG_SUFFIX;
            ret = ipool->GetImageByImageId(it_images->_image_id, image);
            if (SUCCESS != ret)
            {
                oss<<"....Get image by imageid("<<it_images->_image_id<<") failed,ret:"<<ret<<"...."<<endl;
                return ret;
            }
            map<string, int64>::iterator it;
            bool dup = false,match = false;
            location = image.get_location();
            dup_file_name = location.substr(location.find_last_of('/')+1);
            do
            {
                it = name_id_map.find(dup_file_name);
                if (it == name_id_map.end())
                {
                    //�ڴ���û�ҵ�ͬ���� �������dup��
                    break;
                }
                else
                {
                    dup = true;
                }
                //�ҵ�ͬ���Ŀ��Ƿ�ƥ��
                ret = ipool->GetImageByImageId(it->second, dup_image);
                if (SUCCESS != ret)
                {
                    oss<<"....Get image by imageid("<<it->second<<") failed,ret:"<<ret<<"...."<<endl;
                    return ret;
                }
                if (MatchImage(image, dup_image, oss))
                {
                    oss<<"..Dup image "<<dup_file_name<<" found and match.."<<endl;
                    match = true;
                    break;
                }
                dup_file_name = IMAGE_NAME_DUP_PREFIX+dup_file_name;
                if (MAX_IMAGE_NAME_LENGTH <= dup_file_name.length())
                {
                    oss<<"..Dup image ("<<dup_file_name<<") failed for name over length.."<<endl;
                    return ERROR;
                }
            }while(1);

            if (!dup)
            {
                //�ڴ���û��ֱ���÷�dup����
                oss<<"..Not dup image ("<<dup_file_name<<") first create.."<<endl;
            }
            else 
            {
                if (match)
                {
                    //�ڴ�������ƥ������
                    oss<<"..Dup image ("<<dup_file_name<<") match need not create.."<<endl;
                    it_images->_image_id = dup_image.get_iid();
                    continue;
                }
                else
                {
                    //�ڴ����е�ĳһ��dup��û��ֱ����dup����
                    oss<<"..Dup image ("<<dup_file_name<<") first create.."<<endl;
                }
            }
            //����Ӳ����
            oss<<"..Create hard link "<<file_full_name<<" to "<<location<<".."<<endl;
            file_full_name = image_file_prefix + dup_file_name;
            cmd = "ln "+location+" "+file_full_name;
            if (0 != RunCmd(cmd,res))
            {
                oss<<"....RunCmd "<<cmd<<" failed,res:"<<res<<"...."<<endl;
                return ret;
            }
            if (!dup)
            {
                image.get_file_url(url);
                image_id = image.get_iid();
                images.push_back(image);
            }
            else
            {
                dup_image.get_file_url(url);
                image_id = dup_image.get_iid();
                dup_location = dup_image.get_location();
                dup_location = dup_location.substr(0,location.find_last_of('/'))+dup_file_name;
                dup_image.set_location(dup_location);
                images.push_back(dup_image);
            }
            tpi_file.Checksum = url.checksum;
            tpi_file.Name = dup_file_name;
            tpi_file_list.List.push_back(tpi_file);
            name_id_map.insert(make_pair(dup_file_name, image_id)); 
        }
    }
    
    //write cfg file
    oss<<"....Write cfg file...."<<endl;
    ret = GenerateCfgFile(tmp_req,images,dir,oss);
    if (SUCCESS != ret)
    {
        return ret;
    }
    tpi_file.Name = req._request_id+IMAGE_TPI_CFG_SUFFIX;
    sys_md5(dir+"/"+tpi_file.Name, tpi_file.Checksum);
    tpi_file_list.List.push_back(tpi_file);
    
    oss<<"....Write lst file...."<<endl;
    ofstream fout;
    string lst_file = dir+"/"+req._request_id+IMAGE_TPI_LST_SUFFIX;
    fout.open(lst_file.c_str());
    if(!fout.is_open())
    {
        oss<<"....Open file("<<lst_file<<") failed...."<<endl;
        return ERROR;
    }
    fout<<tpi_file_list.serialize();
    fout.close();
    return SUCCESS;
}
/******************************************************************************/  
STATUS ProjectImageAgent::GenerateTempFiles(const ExportProjectImageReq& req,const string &dir, ostringstream& oss)
{
    STATUS ret = ERROR;
    oss<<"...Generate temp files..."<<endl;
    //����request_id.lst�ļ�
    ret = GenerateLstFile(req,dir,oss);
    if (SUCCESS != ret)
    {
        return ret;
    }
    #if 0
    //����request_id.cfg�ļ�
    ret = GenerateCfgFile(req,dir,oss);
    if (SUCCESS != ret)
    {
        return ret;
    }
    #endif
    //�������о����ļ���Ӳ�����ļ���Ϊimage_id.image��д�����ļ�
    ret = GatherImagesAndWriteCfgFile(req,dir,oss);
    return ret;
}
/******************************************************************************/  
STATUS ProjectImageAgent::PrepareTempFiles(const ExportProjectImageReq& req, string &tmp_dir, ostringstream& oss)
{
    oss<<"..Prepare temp files.."<<endl;
    STATUS ret = ERROR;
    
    tmp_dir = MakeTempDir(req._request_id,oss);
    if (tmp_dir.empty())
    {
        return ret;
    }

    ret = ClearTempDir(tmp_dir,oss);
    if (SUCCESS != ret)
    {
        return ret;
    }
    
    ret = GenerateTempFiles(req,tmp_dir,oss);

    return ret;
}
/******************************************************************************/  
STATUS ProjectImageAgent::PrepareTpiFile(const ExportProjectImageReq& req, string &tpi_image, ostringstream& oss)
{
    oss<<"..Prepare tpi file.."<<endl;
    string user_dir = MakeUserDir(req._user_name);
    if (user_dir.empty())
    {
        oss<<"..Make user dir failed.."<<endl;
        return ERROR;
    }

    tpi_image = user_dir+"/"+req._request_id+IMAGE_TPI_SUFFIX;
    
    oss<<"..Prepare tpi file "<<tpi_image<<" success.."<<endl;
    
    return SUCCESS;
}
/******************************************************************************/  
STATUS ProjectImageAgent::ReleaseTempFiles(const string& request_id, ostringstream& oss)
{
    if (request_id.empty())
    {
        oss<<"..Release temp files success for request_id empty.."<<endl;
        return SUCCESS;
    }
    string dir = _image_dir + "/" + request_id;

    string cmd,res;
    cmd = "rm -rf "+dir;
    if (0 != RunCmd(cmd,res))
    {
        oss<<"..Release temp files failed,cmd:"<<cmd<<",res:"<<res<<".."<<endl;
        return ERROR;
    }
    string softlink = TECS_TEMP_DIR;
    softlink += "/" + request_id;
    cmd = "rm -f "+softlink;
    if (0 != RunCmd(cmd,res))
    {
        oss<<"..Release temp files failed,cmd:"<<cmd<<",res:"<<res<<".."<<endl;
        return ERROR;
    }
    oss<<"..Release temp files success,cmd:"<<cmd<<",res:"<<res<<".."<<endl;
    return SUCCESS;
}
/******************************************************************************/  
STATUS ProjectImageAgent::GetTpiImage(int64 image_id, Image& image, ostringstream& oss)
{
    oss<<"..Get image file path.."<<endl;
    ImagePool *pool = ImagePool::GetInstance();
    STATUS ret = pool->GetImageByImageId(image_id,image);
    if (ERROR_OBJECT_NOT_EXIST == ret)
    {
        oss<<"..Image("<<image_id<<") not exist.."<<endl;
    }
    else if (SUCCESS != ret)
    {
        oss<<"..Get image by imageid("<<image_id<<") failed.."<<endl;
        return ret;
    }
    if (IMAGE_CONTAINER_TPI != image.get_container_format())
    {
        oss << "Image "<<image_id<<" is not a tpi image";
        return ERROR;
    }
    return SUCCESS;
}
/******************************************************************************/  
bool ProjectImageAgent::CheckFinish(const string& image_name, ostringstream& oss)
{
    oss<<"..Check finish with location like "<<image_name<<".."<<endl;
    ImagePool *pool = ImagePool::GetInstance();
    string where = " location like '%"+image_name+"%'";
    Image image;
    STATUS ret = ERROR;
    ret = pool->GetImageByWhere(where,image);
    if (SUCCESS == ret)
    {
        oss<<"..Image with name("<<image_name<<") already exist.."<<endl;
        return true;
    }
    return false;
}
/******************************************************************************/  
STATUS ProjectImageAgent::StartTarball(const ExportProjectImageReq& req, const string& tmp_dir, const string& tpi_file, ostringstream& oss)
{
    oss<<"..Start Tarball.."<<endl;
    if (_tpool->FindWork(req._request_id))
    {
        oss<<"..Tarball start success for task "<<req._request_id<<" exist.."<<endl;
        return SUCCESS;
    }
    ProjectImagePara para(tmp_dir,tpi_file);
    TarballTask* task = new TarballTask(req._request_id,req,para);
    if (!task)
    {
        oss<<"..New Tarball task failed.."<<endl;
        return ERROR_NO_MEMORY;
    }

    if (SUCCESS != task->Init())
    {
        oss<<"..Tarball task init failed.."<<endl;
        return ERROR;
    }
    
    STATUS ret = _tpool->AssignWork(task);
    if (SUCCESS != ret)
    {
        oss<<"..Start Tarball task:"<<req._request_id<<" failed! ret="<<ret<<".."<<endl;
    }
    else
    {
        oss<<"..Start Tarball task:"<<req._request_id<<",tmp_dir:"<<tmp_dir<<",tpi_file:"<<tpi_file<<" success.."<<endl;
    }
    return ret;
}
/******************************************************************************/  
STATUS ProjectImageAgent::StartUnTarball(const ImportProjectImageReq& req, const string& tmp_dir, const string& tpi_file, ostringstream& oss)
{
    oss<<"..Start UnTarball.."<<endl;
    if (_tpool->FindWork(req._request_id))
    {
        oss<<"..UnTarball start success for task "<<req._request_id<<" exist.."<<endl;
        return SUCCESS;
    }
    ProjectImagePara para(tmp_dir,tpi_file);
    UnTarballTask* task = new UnTarballTask(req._request_id,req,para);
    if (!task)
    {
        oss<<"..New UnTarball task failed.."<<endl;
        return ERROR_NO_MEMORY;
    }

    if (SUCCESS != task->Init())
    {
        oss<<"..UnTarball task init failed.."<<endl;
        return ERROR;
    }
    
    STATUS ret = _tpool->AssignWork(task);
    if (SUCCESS != ret)
    {
        oss<<"..Start UnTarball task:"<<req._request_id<<" failed! ret="<<ret<<".."<<endl;
    }
    else
    {
        oss<<"..Start UnTarball task:"<<req._request_id<<",tmp_dir:"<<tmp_dir<<",tpi_file:"<<tpi_file<<" success.."<<endl;
    }
    return ret;
}
/******************************************************************************/  
STATUS ProjectImageAgent::KillProcess(const string& process,const string& tag, ostringstream& oss)
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
STATUS ProjectImageAgent::StopTarball(const string& request_id, ostringstream& oss)
{
    oss<<"..Stop download.."<<endl;
    STATUS ret = SUCCESS;
    ProjectImageCtl ctl;
    if (QueryList(request_id,ctl)
        && ctl._request_id == request_id 
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
        if (SUCCESS != KillProcess("tar",_image_dir + "/" + request_id,oss))
        {
            ret = ERROR;
        }
    }
    return ret;
}
/******************************************************************************/  
bool ProjectImageAgent::CheckCtlList(const string& request_id, WorkAck& ack, ostringstream& oss)
{
    oss<<"..Check ctl list.."<<endl;
    ProjectImageCtl ctl;
    bool ret = QueryList(request_id,ctl);
    if (ret)
    {
        switch(ctl._state)
        {
            case PIS_INIT:
            case PIS_SAVE_TARBALLING:
            case PIS_IMPORT_TARBALLING:
            case PIS_SAVE_FINISH:
            case PIS_IMPORT_FINISH:
                ack.progress = ctl._progress;
                ack.state = ERROR_ACTION_RUNNING;
                break;
            case PIS_SAVE_READY:
            case PIS_IMPORT_READY:
                ack.progress = 100;
                ack.state = SUCCESS;
                break;
            case PIS_SAVE_ERROR:
            case PIS_IMPORT_ERROR:
            case PIS_UNKNOWN:
                ack.state = ERROR;
                break;
            default:
                ack.state = ERROR;
                break;
        }
        ack.detail = ctl._detail;
        oss<<"Project image request:"<<request_id<<" in ctl list"<<endl;
    }
    else
    {
        oss<<"Project image request:"<<request_id<<" not in ctl list"<<endl;
    }
    return ret;
}
/******************************************************************************/  
bool ProjectImageAgent::CheckExportReqValid(const ExportProjectImageReq& req, ostringstream& oss)
{
    if (req._request_id.empty()
        || INVALID_UID == req._uid
        || req._user_name.empty())
    {
        oss<<"Save project image request invalid for request_id empty!" << endl ;
        return false;
    }
    oss<<"Save project image request:"<<req._request_id<<" valid"<<endl;
    return true;
}
/******************************************************************************/  
bool ProjectImageAgent::CheckImportReqValid(const ImportProjectImageReq& req, ostringstream& oss)
{
    if (req._request_id.empty()
        || INVALID_UID == req._uid
        || req._user_name.empty()
        || INVALID_IID == req._image_id)
    {
        oss<<"Import project image request invalid for request_id Empty!" << endl ;
        return false;
    }
    oss<<"Import project image request:"<<req._request_id<<" valid"<<endl;
    return true;
}
/******************************************************************************/  
STATUS ProjectImageAgent::ExportProjectImage(const ExportProjectImageReq& req)
{
    STATUS ret = SUCCESS;
    ostringstream oss;
    string image_name,tmp_dir,tpi_image;
    ExportProjectImageAck ack(req.action_id,req._request_id);
    MID sender = m->CurrentMessageFrame()->option.sender();
    
    oss<<".save project image."<<endl;
    if (!CheckExportReqValid(req,oss))
    {
        ret = ERROR;
        ack.state = ERROR;
        ack.detail = oss.str();
        goto MSG_RET;
    }
    
    if (CheckCtlList(req._request_id,ack,oss))
    {
        goto MSG_RET;
    }

    //tpi�����ʽΪrequest_id.tpi,���image�����Ѿ��и��ļ�
    //���ʾ���α����Ѿ����
    image_name = req._request_id+IMAGE_TPI_SUFFIX;
    if (CheckFinish(image_name,oss))
    {
        ack.progress = 100;
        ack.state = SUCCESS;
        goto MSG_RET;
    }
    
    ret = PrepareTempFiles(req,tmp_dir,oss);
    if (SUCCESS != ret)
    {
        ReleaseTempFiles(req._request_id,oss);
        goto ERROR_RET;
    }
    
    ret = PrepareTpiFile(req,tpi_image,oss);
    if (SUCCESS != ret)
    {
        ReleaseTempFiles(req._request_id,oss);
        goto ERROR_RET;
    }
    
    ret = StartTarball(req,tmp_dir,tpi_image,oss);
    
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
    MessageOption option(sender,EV_IMAGE_TPI_EXPORT_ACK,0,0);
    return m->Send(ack,option); 
}
/******************************************************************************/  
STATUS ProjectImageAgent::ImportProjectImage(const ImportProjectImageReq& req)
{
    STATUS ret = SUCCESS;
    ostringstream oss;
    string tmp_dir,tpi_image,tpi_request_id,tpi_image_name;
    Image image;
    ImportProjectImageAck ack(req.action_id,req._request_id);
    MID sender = m->CurrentMessageFrame()->option.sender();
    
    oss<<".import project image."<<endl;
    //������Ϣ�Ϸ��Լ��
    if (!CheckImportReqValid(req,oss))
    {
        ret = ERROR;
        ack.state = ERROR;
        ack.detail = oss.str();
        goto MSG_RET;
    }
    //��Ӧ�������ǲ����Ѿ���ִ����
    if (CheckCtlList(req._request_id,ack,oss))
    {
        goto MSG_RET;
    }
    
    ret = GetTpiImage(req._image_id,image,oss);
    if (SUCCESS != ret)
    {
        goto ERROR_RET;
    }
  
    tpi_image_name = image.get_name();
    tpi_request_id = tpi_image_name.substr(0, tpi_image_name.find_first_of('.') - 1);
    oss<<".Tpi image name("<<tpi_image_name<<"),request_id("<<tpi_request_id<<")."<<endl;
#if 0
    //���ܸ�tpi������û�о����ļ�
    //�����tpi�������о����ļ��Ļ�������֮��ľ����ļ�����ʽΪ
    //request_id_image_id.img,���image������request_id��ͷ�ľ����ļ�
    //���ʾ���ε����Ѿ����
    //���û�о����ļ� ������һ��tar����
    if (CheckFinish(tpi_request_id+"_",oss))
    {
        //����ֻ��˵��ͬһ��tpi������֮ǰ�������
        //��¼һ�´�ӡ
        oss<<".The same tpi image("<<req._image_id<<") has imported before."<<endl;
    }
#endif
    tmp_dir = MakeTempDir(req._request_id,oss);
    if (tmp_dir.empty())
    {
        ret = ERROR;
        goto ERROR_RET;
    }
    tpi_image = image.get_location();
    ret = StartUnTarball(req,tmp_dir,tpi_image,oss);
    
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
    MessageOption option(sender,EV_IMAGE_TPI_IMPORT_ACK,0,0);
    return m->Send(ack,option); 
}
/******************************************************************************/  
STATUS ProjectImageAgent::CalcProgress(const ProjectImageCtl& ctl, int& progress)
{
    _current_query_times ++;
    progress = ctl._progress;
    // ÿ5�����Ҹ���һ�ν��� ���ȸ��µ�90 ����90��û�� �����ͣ��90
    if (QUERY_STATIS_INTERVAL <= _current_query_times - _last_query_times)
    {
        if (progress < 90)
        {
            progress ++;
        }
        _last_query_times = _current_query_times;
    }
    
    return SUCCESS;
}
/******************************************************************************/  
STATUS ProjectImageAgent::GetFileInfo(const string &file,int64& size, ostringstream& oss)
{
    if(access(file.c_str(), F_OK) != 0)
    {
        oss << "...File("<<file<<") not exist! errno="<<errno<<"..."<<endl;
        return ERROR_FILE_NOT_EXIST;
    }

    struct stat64 s;
    if (0 != stat64(file.c_str(),&s))
    {
        oss << "...Stat64("<<file<<") failed! errno="<<errno<<"..."<<endl;
        return ERROR;
    }
    size = s.st_size;
    return SUCCESS;
}
/******************************************************************************/ 
STATUS ProjectImageAgent::RegisterImage(int64 uid, const string& description, const string& file, const string& container_format, int64& image_id, ostringstream& oss)
{
    int64 size = 0;
    STATUS ret = GetFileInfo(file,size,oss);
    if(SUCCESS != ret)
    {
        oss << "failed to get file info!" << endl;
        return ret;
    }

    if(ValidateLicense(LICOBJ_IMAGES, 1) != 0)
    {
        oss << "...license is invalid!..." << endl;
        return ERROR;
    }

    if (container_format.empty())
    {
        oss << "...container format empty!..." << endl;
        return ERROR;
    }
    
    //�Ӵ洢��˻�ȡ�ļ��ϴ���Ĵ��·��
    ImageStore* store = ImageStore::GetInstance();
    FileUrl url;
    ret = store->GetUrlByPath(file,url);
    if(SUCCESS != ret)
    {
        return ret;
    }

    oss << "...ProjectImage will register url:"<<url.path<<","<<url.access_type<<","<<url.access_option<<"..."<<endl;

    Image image(uid,INVALID_OID);
    //image���ֱ��������֣���ĸ���»������
    string image_name(file.substr(file.find_last_of('/')+1));
    for(int i = 0; i < (int)image_name.size(); i++) 
    {
        if(!VALID_NORMAL_NAME_CHAR(image_name.at(i)))
        {
            image_name = image_name.substr(0,i);
            break;
        }
    }

    if(image_name.empty())
    {
        image_name = "noname";
    }

    if(image_name.length()>MAX_IMAGE_NAME_LENGTH)
    {
        image_name = image_name.substr(0,MAX_IMAGE_NAME_LENGTH-1);
    }

    image.set_container_format(container_format);
    image.set_disk_size(size);
    image.set_name(image_name);
    image.set_size(size);
    image.set_file_url(url);
    image.set_location(file);
    image.set_public(false);
    image.set_bus(DISK_BUS_IDE);
    image.set_type(IMAGE_TYPE_MACHINE);
    image.set_arch("");
    image.set_enable(true);
    Datetime dt;
    dt.refresh();
    image.set_register_time(dt.serialize());
    image.set_description(description);
    ImagePool* ipool = ImagePool::GetInstance();
    if(!ipool)
    {
        oss << "...ImagePool not ready!..." << endl;
        return ERROR_NOT_READY;
    }

    string error;
    ret = ipool->Allocate(image,error);
    if(0 != ret)
    {
        oss << error <<endl;
        return ERROR_DB_INSERT_FAIL;
    }
    image_id = image.get_iid();
    oss << "...File " << file << " is registered as image " << image_name 
           << ", id = " << image_id
           << ", size = " << size  <<"..."<< endl;
    return SUCCESS;
}
/******************************************************************************/ 
STATUS ProjectImageAgent::RegisterImage(int64 uid, const string& file, const Image& image, int64& image_id, ostringstream& oss)
{
    string format;
    int64 disk_size = 0;
    int64 size = 0;
    STATUS ret = GetFileInfo(file,size,oss);
    if(SUCCESS != ret)
    {
        oss << "...failed to get file info!..." << endl;
        return ret;
    }

    if(ValidateLicense(LICOBJ_IMAGES, 1) != 0)
    {
        oss << "...license is invalid!..." << endl;
        return ERROR;
    }
    
    //�Ӵ洢��˻�ȡ�ļ��ϴ���Ĵ��·��
    ImageStore* store = ImageStore::GetInstance();
    FileUrl url;
    ret = store->GetUrlByPath(file,url);
    if(SUCCESS != ret)
    {
        return ret;
    }
    
    oss << "...ProjectImage will register url:"<<url.path<<","<<url.access_type<<","<<url.access_option<<"..."<<endl;

    Image image_reg(uid,INVALID_OID);
    //image���ֱ��������֣���ĸ���»������
    string image_name(file.substr(file.find_last_of('/')+1));
    for(int i = 0; i < (int)image_name.size(); i++) 
    {
        if(!VALID_NORMAL_NAME_CHAR(image_name.at(i)))
        {
            image_name = image_name.substr(0,i);
            break;
        }
    }

    if(image_name.empty())
    {
        image_name = "noname";
    }

    if(image_name.length()>MAX_IMAGE_NAME_LENGTH)
    {
        image_name = image_name.substr(0,MAX_IMAGE_NAME_LENGTH-1);
    }

    if(SUCCESS != CheckImgFormatAndSize(file,format,disk_size))
    {
        format = IMAGE_FORMAT_RAW;
        disk_size = size;
    }
    image_reg.set_disk_format(format);
    image_reg.set_disk_size(disk_size);

    
    image_reg.set_name(image_name);
    image_reg.set_size(size);
    image_reg.set_file_url(url);
    image_reg.set_location(file);
    image_reg.set_public(image.get_public());
    image_reg.set_bus(image.get_bus());
    image_reg.set_type(image.get_type());
    image_reg.set_arch(image.get_arch());
    image_reg.set_enable(image.get_enable());
    Datetime dt;
    dt.refresh();
    image_reg.set_register_time(dt.serialize());
    image_reg.set_description(image.get_description());
    image_reg.set_os_type(image.get_os_type());
    ImagePool* ipool = ImagePool::GetInstance();
    if(!ipool)
    {
        oss << "...ImagePool not ready!..." << endl;
        return ERROR_NOT_READY;
    }

    string error;
    ret = ipool->Allocate(image_reg,error);
    if(0 != ret)
    {
        oss << error <<endl;
        return ERROR_DB_INSERT_FAIL;
    }
    image_id = image_reg.get_iid();
    oss << "...File " << file << " is registered as image " << image_name 
           << ", id = " << image_id
           << ", size = " << size  <<"..."<< endl;

    return SUCCESS;
}
/******************************************************************************/ 
STATUS ProjectImageAgent::RegisterImage(int64 uid, const string& src, const string& dst, const Image& image, int64& image_id, ostringstream& oss)
{
    string cmd,res;
    cmd = "rm -f "+dst;
    if (0 != RunCmd(cmd,res))
    {
        oss<<"...RunCmd("<<cmd<<") failed,res("<<res<<")..."<<endl;
        return ERROR;
    }
    cmd = "ln "+src+" "+dst;
    if (0 != RunCmd(cmd,res))
    {
        oss<<"...RunCmd("<<cmd<<") failed,res("<<res<<")..."<<endl;
        return ERROR;
    }
    if (SUCCESS != RegisterImage(uid, dst, image, image_id, oss))
    {
        oss<<"...Register image file("<<dst<<") failed..."<<endl;
        return ERROR;
    }
    return SUCCESS;
}
/******************************************************************************/ 
bool ProjectImageAgent::MatchImage(const Image& src, const Image& dst, ostringstream& oss)
{
    FileUrl src_url,dst_url;
    src.get_file_url(src_url);
    dst.get_file_url(dst_url);

#if 0
    oss<<"..Match Image .."<<endl;
    oss<<"..Src:"<<"arch("<<src.get_arch()<<"),"
                 <<"bus("<<src.get_bus()<<"),"
                 <<"disk_format("<<src.get_disk_format()<<"),"
                 <<"disk_size("<<src.get_disk_size()<<"),"
                 <<"size("<<src.get_size()<<"),"
                 <<"type("<<src.get_type()<<"),"
                 <<"checksum("<<src_url.checksum<<"),"
                 <<"uid("<<src.get_uid()<<").."<<endl;
    oss<<"..Dst:"<<"arch("<<dst.get_arch()<<"),"
                 <<"bus("<<dst.get_bus()<<"),"
                 <<"disk_format("<<dst.get_disk_format()<<"),"
                 <<"disk_size("<<dst.get_disk_size()<<"),"
                 <<"size("<<dst.get_size()<<"),"
                 <<"type("<<dst.get_type()<<"),"
                 <<"checksum("<<dst_url.checksum<<"),"
                 <<"uid("<<dst.get_uid()<<").."<<endl;
#endif

    if (src.get_arch() == dst.get_arch()
        && src.get_bus() == dst.get_bus()
        && src.get_disk_format() == dst.get_disk_format()
        && src.get_disk_size() == dst.get_disk_size()
        && src.get_size() == dst.get_size()
        && src.get_type() == dst.get_type())
    {
        if (!src_url.checksum.empty() && !dst_url.checksum.empty())
        {
            if (src_url.checksum != dst_url.checksum)
            {
                return false;
            }
        }
        return true;
    }
    return false;
}
/******************************************************************************/  
int64 ProjectImageAgent::GetImageIdByChecksum(const string& checksum)
{
    ImagePool *pool = ImagePool::GetInstance();
    string where = " fileurl like '%"+checksum+"%'";
    return pool->GetImageIdByWhere(where);
}
/******************************************************************************/  
int64 ProjectImageAgent::GetImageIdByName(const string& name)
{
    ImagePool *pool = ImagePool::GetInstance();
    string where = " name like '%"+name+"%'";
    return pool->GetImageIdByWhere(where);
}
/******************************************************************************/  
STATUS ProjectImageAgent::GetImageByLocation(const string& location, Image& image)
{
    ImagePool *pool = ImagePool::GetInstance();
    return pool->GetImageByLocation(location,image);
}
/******************************************************************************/  
int64 ProjectImageAgent::GetImageIdByLocation(const string& location)
{
    ImagePool *pool = ImagePool::GetInstance();
    return pool->GetImageIdByLocation(location);
}
/******************************************************************************/ 
STATUS ProjectImageAgent::ParseTpiLstFile(const string& file, TecsProjectImageFileList& lst, ostringstream& oss)
{
    ifstream fin(file.c_str());
    if (!fin)
    {
        oss<<"Open "<<file<<" for read failed"<<endl;
        return ERROR;
    }
    string tmp;
    getline(fin,tmp,'\0');
    fin.close();
    lst.deserialize(tmp);
    return SUCCESS;
}
/******************************************************************************/ 
STATUS ProjectImageAgent::ParseTpiCfgFile(const string& file, TecsProjectImageConfig& cfg, ostringstream& oss)
{
    ifstream fin(file.c_str());
    if (!fin)
    {
        oss<<"Open "<<file<<" for read failed"<<endl;
        return ERROR;
    }
    string tmp;
    getline(fin,tmp,'\0');
    fin.close();
    cfg.deserialize(tmp);
    return SUCCESS;
}
/******************************************************************************/ 
STATUS ProjectImageAgent::ParseTpiImage(const ImportProjectImageReq& req,const string& dir, Project& project, vector<VmCfg>& vms, vector<VmRepel>& repels, vector<VmAffReg>& affregs, ostringstream& oss)
{
    STATUS ret = ERROR;
    DIR *pDir = opendir(dir.c_str());
    if (NULL == pDir)
    {
        oss<<"..Open "<<dir<<" failed.."<<endl;
        return ret;
    }

    //���ҵ�request_id.lst�ļ�
    string file_name,file_full_name;
    string request_id;
    struct dirent *pDirent;
    for(;;)
    {
        pDirent = readdir(pDir);
        if (NULL == pDirent)
        {
            break;
        }
        
        if ((0 == strcmp(pDirent->d_name, "."))
             || (0 == strcmp(pDirent->d_name, "..")))
        {
            continue;
        }

        file_name = pDirent->d_name;
        string::size_type loc = file_name.find(IMAGE_TPI_LST_SUFFIX);
        if( loc != string::npos )
        {
            request_id = file_name.substr(0,loc);
            break;
        }
    }
    closedir(pDir);
    //request_id��tpiЭ���п���Ϊһ��ǿ�ƹ涨�������������һ�����request_id�ĺϷ���
    if (request_id.empty())
    {
        oss<<"..Not find lst file.."<<endl;
        return ERROR;
    }
    //����request_id.lst
    file_full_name = dir + "/" + file_name;
    TecsProjectImageFileList lst;
    if (SUCCESS != ParseTpiLstFile(file_full_name,lst,oss))
    {
        oss<<"..Parse lst file("<<file_full_name<<") failed.."<<endl;
        return ERROR;
    }

    //������û��request_id.cfg�ļ�
    file_name = request_id+IMAGE_TPI_CFG_SUFFIX;
    vector<TecsProjectImageFile>::iterator it_lst;
    for (it_lst = lst.List.begin(); it_lst != lst.List.end(); it_lst++)
    {
        if (it_lst->Name == file_name)
        {
            break;
        }
    }
    if (it_lst == lst.List.end())
    {
        //û�ҵ�cfg�ļ�
        oss<<"..Not find cfg file("<<file_name<<").."<<endl;
        return ERROR;
    }
    //��ʱֻ����cfg�ļ���У���,image�ļ���У����Ȳ�����̫��ʱ��
    string checksum;
    file_full_name = dir + "/" + file_name;
    sys_md5(file_full_name,checksum);
    if (checksum != it_lst->Checksum)
    {
        //�ҵ�cfg�ļ� ����У��Ͳ���
        oss<<"..Cfg file("<<file_full_name<<") checksum mismatch,should("<<it_lst->Checksum<<"),but("<<checksum<<").."<<endl;
        return ERROR;
    }
    //����request_id.cfg�ļ�
    TecsProjectImageConfig cfg;
    if (SUCCESS != ParseTpiCfgFile(file_full_name, cfg, oss))
    {
        oss<<"..Parse cfg file("<<file_full_name<<") failed.."<<endl;
        return ERROR;
    }
    //���Ҳ�ע��.img�ļ����imageʹ������ʽע��
    string cmd,res;
    string location,dup_location,dup_file_name;
    int64 image_id = INVALID_FILEID;
    Image image,dup_image;
    vector<VmCfg>::iterator it_cfg;
    vector<VmBaseImage>::iterator it_base_image;
    vector<Image>::iterator it_image;
    ret = Transaction::Begin();
    if (SUCCESS != ret)
    {
        oss<<"..Transaction begin failed,ret:"<<ret<<".."<<endl;
        return ERROR;
    }
    for (it_cfg = cfg.Instances.begin(); it_cfg != cfg.Instances.end(); it_cfg++)
    {
        for (it_base_image = it_cfg->_images.begin(); it_base_image != it_cfg->_images.end(); it_base_image++)
        {
            if (INVALID_IID == it_base_image->_image_id)
            {
                //û�о���
                continue;
            }
            //�Ȳ�ѯimages�б�
            for (it_image = cfg.Images.begin(); it_image != cfg.Images.end(); it_image++)
            {
                if (it_base_image->_image_id == it_image->get_iid())
                {
                    break;
                }
            }
            if (it_image == cfg.Images.end())
            {
                //��image�б�����û�ҵ���Ӧ����
                oss<<"..Not find image("<<it_base_image->_image_id<<") in images list.."<<endl;
                Transaction::Cancel();
                return ERROR;
            }
            //��һ�°������б��ļ�����û�ж�Ӧ���ļ�
            location = it_image->get_location();
            file_name = location.substr(location.find_last_of('/')+1);
            for (it_lst = lst.List.begin(); it_lst != lst.List.end(); it_lst++)
            {
                if (it_lst->Name == file_name)
                {
                    break;
                }
            }
            if (it_lst == lst.List.end())
            {
                //û�ҵ�img�ļ�
                oss<<"..Not find image file("<<file_name<<") in file list.."<<endl;
                Transaction::Cancel();
                return ERROR;
            }
            //����һ��ʵ�ʽ�ѹ�����İ�������û������ļ�
            location = dir+"/"+file_name;
            if (!check_fileexist(location))
            {

                oss<<"..Not find image file("<<file_name<<") in tarball.."<<endl;
                Transaction::Cancel();
                return ERROR;
            }
            oss<<"..File "<<location<<" exist.."<<endl;
            //�ȿ�һ�����ݿ�������û��ͬ���ļ�
            //�����ͬ���ļ� ��һ��checksum,type���Ƿ�һ�� ���һ���Ͳ���Ҫע��
            //�����һ�� ��ͻ�������ע��һ��
            location = MakeUserDir(req._user_name)+"/"+file_name;
            oss<<"..Check "<<location<<" exist in db.."<<endl;
            ret = GetImageByLocation(location, image);
            if (ERROR_OBJECT_NOT_EXIST == ret)
            {
                //������ ֱ��ע��
                oss<<"..Image "<<location<<" not in db,will register.."<<endl;
                ret = RegisterImage(req._uid, dir+"/"+file_name, location, *it_image, image_id, oss);
                if (SUCCESS != ret)
                {
                    oss<<"..Register image ("<<location<<") failed,ret=("<<ret<<").."<<endl;
                    Transaction::Cancel();
                    return ret;
                }
                oss<<"..Image "<<location<<" register success with imageid("<<image_id<<").."<<endl;
                it_base_image->_image_id = image_id;
            }
            else if (SUCCESS == ret)
            {
                //���ڵĻ� ���һ�²���
                oss<<"..Image "<<location<<" in db,will match.."<<endl;
                if (!MatchImage(*it_image,image,oss))
                {
                    //��ƥ�� ��Ҫ��һ������ǰ���dupǰ׺
                    oss<<"..Image "<<location<<" in db but not match.."<<endl;
                    dup_file_name = file_name;
                    do
                    {
                        dup_file_name = IMAGE_NAME_DUP_PREFIX+dup_file_name;
                        dup_location = File2Location(req._user_name,dup_file_name);
                        if (MAX_IMAGE_NAME_LENGTH <= dup_file_name.length())
                        {
                            oss<<"..Register image ("<<dup_location<<") failed for name over length.."<<endl;
                            Transaction::Cancel();
                            return ERROR;;
                        }
                        oss<<"..Check "<<dup_location<<" exist in db.."<<endl;
                        ret = GetImageByLocation(dup_location, dup_image);
                        if (SUCCESS != ret)
                        {
                            break;
                        }
                        if (MatchImage(*it_image,dup_image,oss))
                        {
                            //�ָ�dup_imageƥ����
                            oss<<"..Image "<<dup_location<<" in db and match.."<<endl;
                            break;
                        }
                    }while(1);
                    
                    if (ERROR_OBJECT_NOT_EXIST == ret)
                    {
                        //dup ������
                        oss<<"..Image "<<dup_location<<" not in db will register.."<<endl;
                        ret = RegisterImage(req._uid, dir+"/"+file_name, dup_location, *it_image, image_id, oss);
                        if (SUCCESS != ret)
                        {
                            oss<<"..Register image ("<<dup_location<<") failed,ret=("<<ret<<").."<<endl;
                            Transaction::Cancel();
                            return ret;
                        }
                        oss<<"..Image "<<dup_location<<" register success with imageid("<<image_id<<").."<<endl;
                        it_base_image->_image_id = image_id;
                    }
                    else if (SUCCESS == ret)
                    {
                        //dup���� ����ƥ������
                        oss<<"..No need register image for ("<<dup_location<<") exist and match.."<<endl;
                        it_base_image->_image_id = dup_image.get_iid();
                    }
                    else
                    {
                        oss<<"..Parse tpi image failed for get image by location("<<dup_location<<"),ret=("<<ret<<").."<<endl;
                        Transaction::Cancel();
                        return ret;
                    }
                }
                else
                {
                    //ƥ��
                    oss<<"..No need register image for ("<<it_image->get_location()<<") exist and match.."<<endl;
                    it_base_image->_image_id = image.get_iid();
                }
            }
            else
            {
                oss<<"..Get image by location("<<location<<") failed,ret=("<<ret<<").."<<endl;
                return ret;
            }
        }
    }
    ret = Transaction::Commit();
    if (SUCCESS != ret)
    {
        oss<<"..Transaction commit failed,ret:"<<ret<<".."<<endl;
        Transaction::Cancel();
        return ERROR;
    }
    //����������Ϣ ���ж�Ӧ��image_idΪ�Ѿ�ע��ľ����ļ�image_id
    project = cfg.ProjectInfo;
    vms = cfg.Instances;
    repels = cfg.Repels;
    affregs= cfg.AffRegs;
    return SUCCESS;
        
}
/******************************************************************************/  
void ProjectImageAgent::QueryProjectImage(void)
{
    MutexLock lock(_mutex);
    if (0 == _ctl_list.size())
    {
        return;
    }
    bool jump = false,deal_success = true,finish = false;
    int progress = 0;
    int64 image_id = INVALID_FILEID;
    string tpi_name;
    MID sender;
    ostringstream oss;

    oss<<".Query project image."<<endl;
    vector<ProjectImageCtl>::iterator it = _ctl_list.begin();
    do
    {
        jump = false;
        switch(it->_state)
        {
            case PIS_INIT:
            case PIS_SAVE_TARBALLING:
            case PIS_IMPORT_TARBALLING:
            {
                if (SUCCESS == CalcProgress(*it, progress))
                {//����UpdateProgress���½��� ��ֹ���б���?
                    UpdateProgress(it->_request_id, progress);
                }
                break;
            }
            case PIS_SAVE_FINISH:
            {
                //������ע��tpi����
                tpi_name = it->_export_req._request_id + IMAGE_TPI_SUFFIX;
                //����Ƿ��Ѿ�ע���
                finish = CheckFinish(tpi_name, oss);
                if (!finish)
                {
                    if (SUCCESS != RegisterImage(it->_export_req._uid,it->_export_req._description,it->_para._file, IMAGE_CONTAINER_TPI,image_id,oss))
                    {//ע��ʧ���� �´μ�������?
                        it->_fail_repeats++;
                        if (it->_fail_repeats < MAX_FAIL_REPEATS)
                        {
                            oss<<"Register image"<<it->_para._file<<" failed "<<it->_fail_repeats<<" times"<<endl;
                            UpdateList(it->_request_id, it->_state, oss.str());
                            break;
                        }
                        else
                        {

                            oss<<"Register image"<<it->_para._file<<" failed over max times"<<endl;
                            UpdateList(it->_request_id, PIS_SAVE_ERROR, oss.str());
                            deal_success = false;
                        }
                    }
                }
                
                //Ҫôע��ɹ���Ҫô������γ���ʧ���� ��������һ����ʱĿ¼
                if (SUCCESS != ReleaseTempFiles(it->_export_req._request_id,oss))
                {//����ʧ���� ����һ��detail 
                    UpdateList(it->_request_id, it->_state, oss.str());
                }

                if (!deal_success)
                {
                    //����ʧ����? ɾ��tpi�ļ�
                    string cmd,res;
                    cmd = "rm -f "+it->_para._file;
                    RunCmd(cmd,res);
                }
                
                if (deal_success)
                {
                    UpdateProgress(it->_request_id, 100);
                    it->_export_ack._image_id = image_id;
                    it->_export_ack.state = SUCCESS;
                    it->_export_ack.progress = 100;
                    it->_export_ack.detail = "OK";
                    UpdateList(it->_request_id, PIS_SAVE_READY,it->_export_ack.detail);
                }
                else
                {
                    it->_export_ack.state = ERROR;
                    it->_export_ack.detail = oss.str();
                    UpdateList(it->_request_id, PIS_SAVE_ERROR,it->_export_ack.detail);
                }
                #if 0
                if (GetSender(it->_request_id,sender))
                {
                    MessageOption option(sender,EV_IMAGE_TPI_EXPORT_ACK,0,0);
                    if (SUCCESS == m->Send(it->_export_ack,option))
                    {//����Ϣ�ɹ���
                        RemoveSender(it->_request_id);
                        //�ͷ���ʱ·�������п����ͷ�ʧ�� ����һ�γ���
                        ReleaseTempFiles(it->_export_ack._request_id,oss);
                        _ctl_list.erase(it);
                        jump = true;
                    }
                }
                #endif
                CLEAR_AND_SEND_ACK(EV_IMAGE_TPI_EXPORT_ACK,it->_export_ack);
                OutPut(SYS_DEBUG,"%s",oss.str().c_str());
                break;
            }
            case PIS_IMPORT_FINISH:
            {
                //��ѹ���,����TPI����
                if (SUCCESS != ParseTpiImage(it->_import_req,it->_para._dir,it->_import_ack._project,it->_import_ack._vms,it->_import_ack._repels,it->_import_ack._affregs,oss))
                {//����ʧ���� �´μ�������?
                    it->_fail_repeats++;
                    if (it->_fail_repeats < MAX_FAIL_REPEATS)
                    {
                        oss<<"Parse image"<<it->_para._file<<" failed "<<it->_fail_repeats<<" times"<<endl;
                        UpdateList(it->_request_id, it->_state, oss.str());
                        break;
                    }
                    else
                    {
                        oss<<"Parse image"<<it->_para._file<<" failed over max times"<<endl;
                        UpdateList(it->_request_id, PIS_IMPORT_ERROR, oss.str());
                        deal_success = false;
                    }
                }
                
                //Ҫô�����ɹ���Ҫô������γ���ʧ���� ��������һ����ʱĿ¼
                if (SUCCESS != ReleaseTempFiles(it->_import_req._request_id,oss))
                {//����ʧ���� ����һ��detail 
                    UpdateList(it->_request_id, it->_state, oss.str());
                }
                
                if (deal_success)
                {
                    UpdateProgress(it->_request_id, 100);
                    it->_import_ack.state = SUCCESS;
                    it->_import_ack.progress = 100;
                    it->_import_ack.detail = "OK";
                    UpdateList(it->_request_id, PIS_IMPORT_READY,it->_import_ack.detail);
                }
                else
                {
                    it->_import_ack.state = ERROR;
                    it->_import_ack.detail = oss.str();
                    UpdateList(it->_request_id, PIS_IMPORT_ERROR,it->_import_ack.detail); 
                }
                #if 0
                if (GetSender(it->_request_id,sender))
                {
                    MessageOption option(sender,EV_IMAGE_TPI_IMPORT_ACK,0,0);
                    if (SUCCESS == m->Send(it->_import_ack,option))
                    {//����Ϣ�ɹ���
                        RemoveSender(it->_request_id);
                        //�ͷ���ʱ·�������п����ͷ�ʧ�� ����һ�γ���
                        ReleaseTempFiles(it->_import_ack._request_id,oss);
                        _ctl_list.erase(it);
                        jump = true;
                    }
                }
                #endif
                CLEAR_AND_SEND_ACK(EV_IMAGE_TPI_IMPORT_ACK,it->_import_ack);
                OutPut(SYS_DEBUG,"%s",oss.str().c_str());
                break;
            }
            case PIS_SAVE_READY:
            {
                //�ϴ���Ϣû���ɹ�?
                #if 0
                if (GetSender(it->_request_id,sender))
                {
                    MessageOption option(sender,EV_IMAGE_TPI_EXPORT_ACK,0,0);
                    if (SUCCESS == m->Send(it->_export_ack,option))
                    {//����Ϣ�ɹ���
                        RemoveSender(it->_request_id);
                        //�ͷ���ʱ·��
                        ReleaseTempFiles(it->_export_ack._request_id,oss);
                        _ctl_list.erase(it);
                        jump = true;
                    }
                }
                #endif
                CLEAR_AND_SEND_ACK(EV_IMAGE_TPI_EXPORT_ACK,it->_export_ack);
                break;
            }
            case PIS_SAVE_ERROR:
            {
                #if 0
                //�ϴ���Ϣû���ɹ�?
                if (GetSender(it->_request_id,sender))
                {
                    MessageOption option(sender,EV_IMAGE_TPI_EXPORT_ACK,0,0);
                    if (SUCCESS == m->Send(it->_export_ack,option))
                    {//����Ϣ�ɹ���
                        RemoveSender(it->_request_id);
                        //�ͷ���ʱ·��
                        ReleaseTempFiles(it->_export_ack._request_id,oss);
                        _ctl_list.erase(it);
                        jump = true;
                    }
                }
                #endif
                //ɾ����ʱ�ļ�
                string tpi_image;
                PrepareTpiFile(it->_export_req,tpi_image,oss);
                string cmd="rm -f "+tpi_image;
                string res;
                RunCmd(cmd,res);
                CLEAR_AND_SEND_ACK(EV_IMAGE_TPI_EXPORT_ACK,it->_export_ack);
                break;
            }
            case PIS_IMPORT_READY:
            {
                //�ϴ���Ϣû���ɹ�?
                #if 0
                if (GetSender(it->_request_id,sender))
                {
                    MessageOption option(sender,EV_IMAGE_TPI_IMPORT_ACK,0,0);
                    if (SUCCESS == m->Send(it->_import_ack,option))
                    {//����Ϣ�ɹ���
                        RemoveSender(it->_request_id);
                        //�ͷ���ʱ·��
                        ReleaseTempFiles(it->_import_ack._request_id,oss);
                        _ctl_list.erase(it);
                        jump = true;
                    }
                }
                #endif
                CLEAR_AND_SEND_ACK(EV_IMAGE_TPI_IMPORT_ACK,it->_import_ack);
                break;
            }
            case PIS_IMPORT_ERROR:
            {
                #if 0
                //�ϴ���Ϣû���ɹ�?
                if (GetSender(it->_request_id,sender))
                {
                    MessageOption option(sender,EV_IMAGE_TPI_IMPORT_ACK,0,0);
                    if (SUCCESS == m->Send(it->_import_ack,option))
                    {//����Ϣ�ɹ���
                        RemoveSender(it->_request_id);
                        //�ͷ���ʱ·��
                        ReleaseTempFiles(it->_import_ack._request_id,oss);
                        _ctl_list.erase(it);
                        jump = true;
                    }
                }
                #endif
                CLEAR_AND_SEND_ACK(EV_IMAGE_TPI_IMPORT_ACK,it->_import_ack);
                break;
            }
            case PIS_UNKNOWN:
                OutPut(SYS_ERROR,"%s in ctl list but state unknown\n",it->_request_id.c_str());
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
void ProjectImageAgent::MessageEntry(const MessageFrame& message)
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
                        OutPut(SYS_ALERT, "CreateTimer failed in ProjectImageAgent::MessageEntry()!\n");
                        SkyExit(-1,"CreateTimer failed in ProjectImageAgent::MessageEntry()!\n");
                    }
                    
                    TimeOut timeout;
                    timeout.type = LOOP_TIMER;
                    timeout.duration = 2000;    
                    timeout.msgid = EV_TIMER_QUERY;
                    STATUS rc = m->SetTimer(_timer, timeout);
                    if (SUCCESS != rc)
                    {
                        OutPut(SYS_ALERT, "ProjectImageAgent SetTimer failed rc = %d!\n",rc);
                        SkyAssert(false);
                    }
                    if (SUCCESS != Transaction::Enable(GetDB()))
                    {
                        OutPut(SYS_ERROR,"ProjectImageAgent enable transaction fail!");
                        SkyExit(-1, "ProjectImageAgent::MessageEntry: call Transaction::Enable(GetDB()) failed.");
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
                case EV_IMAGE_TPI_EXPORT_REQ:
                {
                    ExportProjectImageReq req;
                    if(req.deserialize(message.data))
                    {
                        ExportProjectImage(req);
                    }
                    else
                    {
                        OutPut(SYS_ERROR,"failed to deserialize ExportProjectImageReq message:\n%s\n",
                            message.data.c_str());
                    }
                    break;
                }
                case EV_IMAGE_TPI_IMPORT_REQ:
                {
                    ImportProjectImageReq req;
                    if(req.deserialize(message.data))
                    {
                        ImportProjectImage(req);
                    }
                    else
                    {
                        OutPut(SYS_ERROR,"failed to deserialize ImportProjectImageReq message:\n%s\n",
                            message.data.c_str());
                    }
                    break;
                }
                case EV_TIMER_QUERY:
                {
                    QueryProjectImage();
                    break;
                }
                default:
                    break;
            }
        }
    }
}
/******************************************************************************/
void DbgShowProjectImage()
{
    ProjectImageAgent *pInst = ProjectImageAgent::GetInstance();
    if (NULL == pInst)
    {
        cout << "Get project image agent instance failed!"<<endl;
        return;
    }
    pInst->Print();
}
DEFINE_DEBUG_CMD(projectimageing,DbgShowProjectImage);

}

