/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：host_storage.cpp
* 文件标识：见配置管理计划书
* 内容摘要：物理机存储管理实现文件
* 当前版本：1.0

* 作    者：颜伟
* 完成日期：2012.06.11

* 修改记录1：
*    修改日期：2012.06.11
*    版 本 号：V2.0
*    修 改 人：yanwei
*    修改内容：整改
*******************************************************************************/

/****************************************************************************************
*                                 头文件                                                *
****************************************************************************************/
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <unistd.h> 
#include <sys/mount.h>
#include <ctype.h>
#include <netdb.h>
#include <boost/algorithm/string.hpp>
#include "sky.h"
#include "storage.h"
#include "event.h"
#include "lvm.h"
#include "smart.h"
#include "volume.h"
#include "nfs.h"
#include "host_storage.h"
#include "vm_messages.h"
#include "log_agent.h"
#include "hypervisor.h"

using namespace zte_tecs;

/******************************************************************************/ 
StorageOperation *StorageOperation::_instance = NULL;
/******************************************************************************/  
int StorageOperation::sys_rm(const string &file)
{
    string cmd,res;
    
    if (file.empty())
    {
        return ERROR;
    }
    cmd = "rm -rf "+file+" 2>&1";
    if (SUCCESS != storage_command(cmd,res))
    {
        return ERROR;
    }
    if (string::npos != res.find("busy"))
    {
        return ERROR_DEVICE_BUSY;
    }
    return SUCCESS;
}
/******************************************************************************/  
int StorageOperation::sys_rmdir(const string &dir)
{
    string cmd,res;
    
    if (dir.empty())
    {
        return ERROR;
    }
    cmd = "rmdir "+dir+" 2>&1";
    if (SUCCESS != storage_command(cmd,res))
    {
        return ERROR;
    }
    if (string::npos != res.find("not empty"))
    {
        return ERROR_DEVICE_BUSY;
    }
    return SUCCESS;
}
/******************************************************************************/  
int StorageOperation::sys_mkdir(const string &dir)
{
    string cmd,res;
    
    if (dir.empty())
    {
        return ERROR;
    }
    cmd = "mkdir -p "+dir;
    return storage_command(cmd,res);
}
/******************************************************************************/  
int StorageOperation::sys_umount(const string &filename,const string &option)
{
    int flag = 0;
    string cmd,res;

    if (!check_mount(filename))
    {
        return SUCCESS;
    }
    if (option.empty())
    {
        return umount(filename.c_str());
    }
    else
    {
        if (option == "-l")
        {
            flag |= MNT_DETACH;
        }
        return umount2(filename.c_str(),flag);
    }
}
/******************************************************************************/  
int StorageOperation::sys_mount(const string &fsname,const string &mountpoint,const string &fstype)
{
    int ret = ERROR;
    string cmd,res;

    if (fsname.empty() || mountpoint.empty())
    {
        return ERROR;
    }
    ret = mount(fsname.c_str(),mountpoint.c_str(),fstype.c_str(),0,NULL);
    if (SUCCESS != ret)
    {
        OutPut(SYS_ERROR,"mount %s to %s with fstype:%s failed for %s,ret:%d",fsname.c_str(),mountpoint.c_str(),fstype.c_str(),strerror(errno),ret);
    }
    return ret;
}
/******************************************************************************/  
bool StorageOperation::file_isexist(const string &file)
{
    if (0 != access(file.c_str(), F_OK))
    {
        return false;
    }
    else
    {
        return true;
    }
}
/******************************************************************************/  
void StorageOperation::print_upload(void)
{
    vector<UploadCtl>::iterator it;
    for (it = _upload_ctl.begin(); it != _upload_ctl.end(); it++)
    {
        printf("<upload>vid:%lld,iid:%lld,state:%d,size:%lld,finish:%d\n",it->_vid,it->_disk._id,it->_state,it->_disk._size,it->_finish);
    }
}
/******************************************************************************/  
int StorageOperation::inc_upload(int64 vid,const VmSaveReq &req,const VmDiskConfig disk,const ImageStoreOption opt)
{
    int ret = ERROR;
    
    if (INVALID_VID == vid)
    {
        return ret;
    }
    lock_upload();
    vector<UploadCtl>::iterator it;
    for (it = _upload_ctl.begin(); it != _upload_ctl.end(); it++)
    {
        if ((it->_vid == vid)
            && (it->_disk._id == disk._id)
            && (it->_disk._target == disk._target))
        {
            break;
        }
    }
    if (it == _upload_ctl.end())
    {
        UploadCtl ulc;
        ulc._vid = vid;
        ulc._req = req;
        ulc._disk = disk;
        ulc._opt = opt;
        _upload_ctl.push_back(ulc);
        ret = SUCCESS;
    }
    unlock_upload();
    return ret;
}
/******************************************************************************/  
int StorageOperation::dec_upload(int64 vid,const VmDiskConfig disk)
{
    if (INVALID_VID == vid)
    {
        return ERROR;
    }
    lock_upload();
    vector<UploadCtl>::iterator it;
    for (it = _upload_ctl.begin(); it != _upload_ctl.end(); it++)
    {
        if ((it->_vid == vid)
            && (it->_disk._id == disk._id)
            && (it->_disk._target == disk._target))
        {
            break;
        }
    }
    if (it != _upload_ctl.end())
    {
        _upload_ctl.erase(it);
    }
    unlock_upload();
    return SUCCESS;
}
/******************************************************************************/  
int StorageOperation::stop_upload(int64 vid,const VmDiskConfig disk)
{
    int ret = SUCCESS;
    if (INVALID_VID == vid)
    {
        return ERROR;
    }
    lock_upload();
    vector<UploadCtl>::iterator it;
    for (it = _upload_ctl.begin(); it != _upload_ctl.end(); it++)
    {
        if ((it->_vid == vid)
            && (it->_disk._id == disk._id)
            && (it->_disk._target == disk._target))
        {
            break;
        }
    }
    if (it != _upload_ctl.end())
    {
        _upload_ctl.erase(it);
    }
    unlock_upload();
    return ret;
}
/******************************************************************************/  
int StorageOperation::update_upload(int64 vid,const VmDiskConfig disk,int64 finish)
{
    if (INVALID_VID == vid)
    {
        return ERROR;
    }
    lock_upload();
    vector<UploadCtl>::iterator it;
    for (it = _upload_ctl.begin(); it != _upload_ctl.end(); it++)
    {
        if ((it->_vid == vid)
            && (it->_disk._id == disk._id)
            && (it->_disk._target == disk._target))
        {
            break;
        }
    }
    if (it != _upload_ctl.end())
    {
        it->_finish = finish;
    }
    unlock_upload();
    return SUCCESS;
}
/******************************************************************************/  
int StorageOperation::update_upload(int64 vid,const VmDiskConfig disk,int state)
{
    if (INVALID_VID == vid)
    {
        return ERROR;
    }
    lock_upload();
    vector<UploadCtl>::iterator it;
    for (it = _upload_ctl.begin(); it != _upload_ctl.end(); it++)
    {
        if ((it->_vid == vid)
            && (it->_disk._id == disk._id)
            && (it->_disk._target == disk._target))
        {
            break;
        }
    }
    if (it != _upload_ctl.end())
    {
        if (UPLOAD_STATE_UNKNOWN != state)
        {
            it->_state  = state;
        }
    }
    unlock_upload();
    return SUCCESS;
}
/******************************************************************************/  
int StorageOperation::check_upload(int64 vid,const VmDiskConfig disk,UploadCtl &ulc)
{
    int ret = ERROR;
    
    if (INVALID_VID == vid)
    {
        return ret;
    }
    lock_upload();
    vector<UploadCtl>::iterator it;
    for (it = _upload_ctl.begin(); it != _upload_ctl.end(); it++)
    {
        if ((it->_vid == vid)
            && (it->_disk._id == disk._id)
            && (it->_disk._target == disk._target))
        {
            break;
        }
    }
    if (it != _upload_ctl.end())
    {
        ulc = *it;
        ret = SUCCESS;
    }
    unlock_upload();
    return ret;
}
/******************************************************************************/  
bool StorageOperation::iscancel_upload(int64 vid,const VmDiskConfig disk)
{
    bool ret = false;
    
    if (INVALID_VID == vid)
    {
        return ret;
    }
    lock_upload();
    vector<UploadCtl>::iterator it;
    for (it = _upload_ctl.begin(); it != _upload_ctl.end(); it++)
    {
        if ((it->_vid == vid)
            && (it->_disk._id == disk._id)
            && (it->_disk._target == disk._target))
        {
            break;
        }
    }
    if ((it == _upload_ctl.end())
        || (it != _upload_ctl.end() && (UPLOAD_STATE_CANCEL == it->_state)))
    {
        ret = true;
    }
    unlock_upload();
    return ret;
}
/******************************************************************************/  
void StorageOperation::print_download(void)
{
    if (0 == _download_ctl.size())
    {
        printf("no image down load\n");
        return;
    }

    printf("--------image download info--------\n");
    vector<DownLoadCtl>::iterator it; 
    vector<VmInfoOfDLC>::iterator itvm;
    for (it = _download_ctl.begin(); it != _download_ctl.end(); it++)
    {
        printf("<image>id:%lld,vid:%lld,pid:%d,ref:%d,finish:%d\n",it->_iid,it->_vid,it->_pid,it->_refcount,it->_finish);
        for (itvm = it->_vminfo.begin(); itvm != it->_vminfo.end(); itvm++)
        {
            printf("<vminfo>id:%lld,state:%d,pid:%d,source:%s,type:%s\n",itvm->_vid,itvm->_state,itvm->_pid,itvm->_disk._source.c_str(),itvm->_disk._source_type.c_str()); 
        }
    }
}
/******************************************************************************/  
int StorageOperation::inc_download(int64 iid,int64 vid,bool &first,const VmDiskConfig &disk,const ImageStoreOption &opt)
{  
    if ((INVALID_FILEID == iid) || (INVALID_VID == vid))
    {
        return ERROR;
    }
    lock_download();
    vector<DownLoadCtl>::iterator it;
    bool first_add = false;
    first = false;
    for (it = _download_ctl.begin(); it != _download_ctl.end(); it++)
    {
        if (it->_iid == iid)
        {
            break;
        }
    }
    VmInfoOfDLC vminfo(STORAGE_VM_STATE_DEPLOY,vid,-1,disk,opt);
    VmInfoOfDLC firstvminfo;    
    if (it != _download_ctl.end())
    {
        vector<VmInfoOfDLC>::iterator itvm;        
        bool exist = false;

        for (itvm = it->_vminfo.begin(); itvm != it->_vminfo.end(); itvm++)
        {
            /*找到自己*/
            if (itvm->_vid == vminfo._vid)
            {
                exist = true;
            }

            /*找到first*/
            if (it->_vid == itvm->_vid)
            {
                firstvminfo = *itvm;                
            }
        }

        /*未找到自己，则加入到队列中*/
        if (!exist)
        {
            it->_vminfo.push_back(vminfo);
            it->_refcount++;
        }

        /*对比first 与 自己，以确定是否是真的first，如果入参是direct的，first返回不使用
          只有是snapshot的方式的对比了，才会使用first值*/
        if (vminfo._disk._position == firstvminfo._disk._position
            && (0 == vminfo._opt._img_usage.compare(firstvminfo._opt._img_usage))
            && (0 == vminfo._opt._share_img_usage.compare(firstvminfo._opt._share_img_usage))
            && (0 == vminfo._opt._img_srctype.compare(firstvminfo._opt._img_srctype))
            && (vminfo._opt._img_spcexp == firstvminfo._opt._img_spcexp))
        {
            first = false;
        }
        else
        {
            first = true;
        }
    }
    else
    {
        DownLoadCtl dlc(iid,vid,-1,1,false);
        dlc._vminfo.push_back(vminfo);
        _download_ctl.push_back(dlc);
        first = true;
        first_add = true;
    }

    /*找到first，但是已经执行完了，设置为ready态了，则设置first为true*/
    if (!first)
    {
       if (STORAGE_VM_STATE_READY == firstvminfo._state || STORAGE_VM_STATE_FAILED== firstvminfo._state)
       {
           first = true;
       }
    }

    /*未找到first，且不是刚加进来的，则设置本vid为first,非本地快照就不加入first的争夺了*/
    if (first && !first_add && 
        ((IMAGE_POSITION_LOCAL == vminfo._disk._position) && (IMAGE_USE_SNAPSHOT == vminfo._opt._img_usage)))
    {
        if(it->_vid != vminfo._vid)
        {
            it->_vid = vminfo._vid;
        }
    }

    unlock_download();
    return SUCCESS;
}
/******************************************************************************/  
int StorageOperation::dec_download(int64 iid, int64 vid)
{
    if ((INVALID_FILEID == iid) || (INVALID_VID == vid))
    {
        return ERROR;
    }
    
    lock_download();
    vector<DownLoadCtl>::iterator it;
    for (it = _download_ctl.begin(); it != _download_ctl.end(); it++)
    {
        if (it->_iid == iid)
        {
            break;
        }
    }
    if (it != _download_ctl.end())
    {
        vector<VmInfoOfDLC>::iterator itvm;
        for (itvm = it->_vminfo.begin(); itvm != it->_vminfo.end(); itvm++)
        {
            if (itvm->_vid == vid)
            {
                break;
            }
        }
        if (itvm != it->_vminfo.end())
        {
            it->_vminfo.erase(itvm);
            it->_refcount--;
        }
        if (0 == it->_refcount)
        {
            _download_ctl.erase(it);    
        }
    }
    unlock_download();
    return SUCCESS;
}
/******************************************************************************/  
int StorageOperation::update_download(int64 iid,int64 vid,pid_t pid,bool finish)
{
    int ret = ERROR;

    if ((INVALID_FILEID == iid) || (INVALID_VID == vid))
    {
        return ret;
    }
    lock_download();
    vector<DownLoadCtl>::iterator it;
    for (it = _download_ctl.begin(); it != _download_ctl.end(); it++)
    {
        if (it->_iid == iid)
        {
            break;
        }
    }
    if (it != _download_ctl.end())
    {
        it->_pid = pid;
        it->_finish = finish;
        vector<VmInfoOfDLC>::iterator itvm;
        for (itvm = it->_vminfo.begin(); itvm != it->_vminfo.end(); itvm++)
        {
            if (itvm->_vid == vid)
            {
                break;
            }
        }
        if (itvm != it->_vminfo.end())
        {
            itvm->_pid = pid;
        }
        ret = SUCCESS;
    }
    else
    {/*没找到 直接返回失败*/
        ret = ERROR;
    }
    unlock_download();
    return ret;
}
/******************************************************************************/  
int StorageOperation::update_download(int64 iid,const VmInfoOfDLC &vminfo)
{
    int ret = ERROR;
    
    if ((INVALID_FILEID == iid) || (INVALID_VID == vminfo._vid))
    {
        return ret;
    }
    
    lock_download();
    vector<DownLoadCtl>::iterator it;
    for (it = _download_ctl.begin(); it != _download_ctl.end(); it++)
    {
        if (it->_iid == iid)
        {
            break;
        }
    }
    if (it != _download_ctl.end())
    {
        vector<VmInfoOfDLC>::iterator itvm;
        for (itvm = it->_vminfo.begin(); itvm != it->_vminfo.end(); itvm++)
        {
            if (itvm->_vid == vminfo._vid)
            {
                break;
            }
        }
        if (itvm != it->_vminfo.end())
        {
            itvm->_disk._source = vminfo._disk._source;
            itvm->_disk._source_type = vminfo._disk._source_type;
            itvm->_state = vminfo._state;
            itvm->_pid = vminfo._pid;
            ret = SUCCESS;
        }
        }
    unlock_download();
    return ret;
}
/******************************************************************************/  
int StorageOperation::check_download(int64 iid,VmInfoOfDLC &vminfo)
{
    int ret = ERROR;

    if ((INVALID_FILEID == iid) || (INVALID_VID == vminfo._vid))
    {
        return ret;
    }
    
    lock_download();
    vector<DownLoadCtl>::iterator it;
    for (it = _download_ctl.begin(); it != _download_ctl.end(); it++)
    {
        if (it->_iid == iid)
        {
            break;
        }
    }
    if (it != _download_ctl.end())
    {
        vector<VmInfoOfDLC>::iterator itvm;
        for (itvm = it->_vminfo.begin(); itvm != it->_vminfo.end(); itvm++)
        {
            if (itvm->_vid == vminfo._vid)
            {
                break;
            }
        }
        if (itvm != it->_vminfo.end())
        {/*在队列中找到该虚拟机*/
            vminfo._disk._source = itvm->_disk._source;
            vminfo._disk._source_type = itvm->_disk._source_type;
            vminfo._state = itvm->_state;
            vminfo._pid = itvm->_pid;
            vminfo._disk = itvm->_disk;
            vminfo._opt = itvm->_opt;
            ret = SUCCESS;
        }
    }
 
    unlock_download();
    return ret;
}
/******************************************************************************/  
void StorageOperation::stop_download(int64 vid,const VmDiskConfig &disk,const ImageStoreOption &opt)
{
    lock_download();
    vector<DownLoadCtl>::iterator it;
    for (it = _download_ctl.begin(); it != _download_ctl.end(); it++)
    {
        if (it->_iid == disk._id)
        {
            break;
        }
    }
    if (it != _download_ctl.end())
    {
        string cmd,res;
        if (!is_usesnapshot(disk,opt))
        {
            vector<VmInfoOfDLC>::iterator itvm;
            for (itvm = it->_vminfo.begin(); itvm != it->_vminfo.end(); itvm++)
            {
                if (itvm->_vid == vid)
                {
                    break;
                }
            }
            if ((itvm != it->_vminfo.end()) && (-1 != itvm->_pid))
            {/*在队列中找到该虚拟机*/
                kill(itvm->_pid,9);
            }
        }
        else
        {
            if ((1 == it->_refcount) && (-1 != it->_pid))
            {/*只有一个虚拟机在等待该映像 则直接杀掉映像下载进程*/
                kill(it->_pid,9);
            }
        }
    }
    /*该虚拟机从等待队列中删除*/
    dec_download(disk._id,vid);
    unlock_download();
}
/******************************************************************************/  
int StorageOperation::clear_download(int64 vid,const VmDiskConfig &disk,const ImageStoreOption &opt)
{
    int ret = SUCCESS;
    bool needcleartmp = false;

    lock_download();
    vector<DownLoadCtl>::iterator it;
    for (it = _download_ctl.begin(); it != _download_ctl.end(); it++)
    {
        if (it->_iid == disk._id)
        {
            break;
        }
    }
    if (it == _download_ctl.end())
    {/*没有登记项则清除挂载目录等*/
        needcleartmp = true;
    }
    else
    {
        vector<VmInfoOfDLC>::iterator itvm;
        for (itvm = it->_vminfo.begin(); itvm != it->_vminfo.end(); itvm++)
        {
            if (itvm->_vid == vid)
            {
                break;
            }
        }
        if (itvm == it->_vminfo.end() && !is_usesnapshot(disk,opt))
        {/*直接使用映像的模式*/
            needcleartmp = true;
        }
    }
    unlock_download();
    if (needcleartmp && STORAGE_POSITION_LOCAL == disk._position)
    {
        ret = clear_tmpresource(vid,disk,opt);
    }
    return ret;
}
/******************************************************************************/  
bool StorageOperation::iscancel_download(int64 iid,int64 vid)
{
    if ((INVALID_FILEID == iid) || (INVALID_VID == vid))
    {
        return false;
    }
    lock_download();
    bool ret = true;
    vector<DownLoadCtl>::iterator it;
    for (it = _download_ctl.begin(); it != _download_ctl.end(); it++)
    {
        if (it->_iid == iid)
        {
            break;
        }
    }
    if (it != _download_ctl.end())
    {/*没有登记项则清除挂载目录等*/
        vector<VmInfoOfDLC>::iterator itvm;
        for (itvm = it->_vminfo.begin(); itvm != it->_vminfo.end(); itvm++)
        {
            if (itvm->_vid == vid)
            {
                break;
            }
        }
        if (itvm != it->_vminfo.end())
        {
            if (STORAGE_VM_STATE_CANCEL != itvm->_state)
            {
                ret = false;
            }
        }
    }
    unlock_download();
    return ret;
}
/******************************************************************************/  
void StorageOperation::kill_download(void)
{
    pid_t tmppid;
    string cmd,res,pid,pidlist;

    cmd = "ps x | grep curl | grep "+STORAGE_HC_MOUNT_DIR+" | grep -v grep | awk '{ print $1}'";
    storage_command(cmd,pidlist);
    OutPut(SYS_DEBUG,"kill_download all pidlist:%s\n",pidlist.c_str());
    stringstream iss(pidlist);
    while (getline(iss,pid))
    {
        #if 0
        cmd = "kill -9 "+pid;
        storage_command(cmd,res);
        #endif
        from_string<pid_t>(tmppid,pid,dec);
        kill(tmppid,9);
    }
}
/******************************************************************************/  
void StorageOperation::kill_download(int64 vid,const VmDiskConfig &disk,const ImageStoreOption &opt)
{
    bool needkill = true;
    pid_t pid = -1;

    lock_download();
    vector<DownLoadCtl>::iterator it;
    for (it = _download_ctl.begin(); it != _download_ctl.end(); it++)
    {
        if (it->_iid == disk._id)
        {
            break;
        }
    }
    if (it != _download_ctl.end())
    {
        vector<VmInfoOfDLC>::iterator itvm;
        for (itvm = it->_vminfo.begin(); itvm != it->_vminfo.end(); itvm++)
        {
            if (itvm->_vid == vid)
            {
                break;
            }
        }
        if (itvm != it->_vminfo.end())
        {
            if (!is_usesnapshot(disk,opt))
            {
                pid = itvm->_pid;
            }
            else
            {
                if (1 < it->_refcount)
                {/*基于快照方式说明有其他虚拟机等待下载 不能杀*/
                    needkill = false;
                }
                else
                {
                    pid = it->_pid;
                }
            }
        }
        else
        {
            if (is_usesnapshot(disk,opt))
            {/*基于快照方式说明有其他虚拟机等待下载 不能杀*/
                needkill = false;
            }
        }
    }
    unlock_download();
    if (needkill)
    {
        if (-1 == pid)
        {
            string cmd,res,pidlist,pidfound;
            string mountpoint;
            if (!is_usesnapshot(disk,opt))
            {
                mountpoint = STORAGE_HC_MOUNT_DIR+"/"+to_string<int64>(vid, dec);
            }
            else
            {
                mountpoint = STORAGE_HC_MOUNT_DIR+"/"+to_string<int64>(disk._id, dec);
            }
            
            cmd = "ps x | grep curl | grep "+mountpoint+" | grep -v grep | awk '{ print $1}'";
            storage_command(cmd,pidlist);
            OutPut(SYS_DEBUG,"kill_download vid:%lld pidlist:%s\n",vid,pidlist.c_str());
            stringstream iss(pidlist);
            while (getline(iss,pidfound))
            {
                from_string<pid_t>(pid,pidfound,dec);
                kill(pid,9);
            }
        }
        else
        {
            kill(pid,9);
        }
    }
}
/******************************************************************************/  
int StorageOperation::release_download(int64 vid,const VmDiskConfig &disk,const ImageStoreOption &opt)
{
    int ret = SUCCESS;

    /*1:加锁*/
    lock_download();
    /*2:遍历*/
    vector<DownLoadCtl>::iterator it;
    for (it = _download_ctl.begin(); it != _download_ctl.end(); it++)
    {
        if (it->_iid == disk._id)
        {
            break;
        }
    }
    if (it != _download_ctl.end())
    {
        if ((is_usesnapshot(disk,opt)) 
            && (1 < it->_refcount))
        {
           /*2.1:符合下面条件的 直接把自己从镜像处理队列里面拿掉即可*/
           /*条件
               1:快照方式
               2:镜像处理队列里面的虚拟机超过1个
               */
            OutPut(SYS_DEBUG,"release_storage dequeue for vm:%lld\n",vid);
            dec_download(disk._id,vid);
            unlock_download();
            return ERR_HS_DEQUEUE_OK;
        }
        else
        {
           /*2.2:从队列里面删除该虚拟机
               *带快照的情况下 如果是该虚拟机在下载镜像的话 则这儿使用锁来控制是否会创建该虚拟机的快照
               *如果这儿获得了锁 将该虚拟机从队列中删除 则在创建快照的地方不会创建该虚拟机快照
               *如果在创建快照那儿获得了锁 则会先创建快照 然后这儿往下走 在free_img那儿删掉快照
               */
            stop_download(vid,disk,opt);
        }
    }
    else
    {
        /*2.3:镜像处理队列中没有该登记项*/
        kill_download(vid,disk,opt);
    }
    /*3:杀掉checksum进程*/
    kill_checksum(vid,disk,opt);

    /*4:清掉download相关的资源*/
    if (SUCCESS != clear_download(vid,disk,opt))
    {
        ret = ERROR;
    }
    unlock_download();
    return ret;
}
/******************************************************************************/  
void StorageOperation::kill_checksum(int64 vid,const VmDiskConfig &disk,const ImageStoreOption &opt)
{
    bool needkill = true;

    lock_download();
    vector<DownLoadCtl>::iterator it;
    for (it = _download_ctl.begin(); it != _download_ctl.end(); it++)
    {
        if (it->_iid == disk._id)
        {
            break;
        }
    }
    if (it != _download_ctl.end())
    {
        vector<VmInfoOfDLC>::iterator itvm;
        for (itvm = it->_vminfo.begin(); itvm != it->_vminfo.end(); itvm++)
        {
            if (itvm->_vid == vid)
            {
                break;
            }
        }
        if (is_usesnapshot(disk,opt))
        {
            if (((itvm != it->_vminfo.end()) && (1 < it->_refcount))
                || (itvm == it->_vminfo.end()))
            {/*基于快照方式说明有其他虚拟机等待下载 不能杀*/
                needkill = false;
            }
        }
    }
    unlock_download();
    if (!needkill)
    {
        return;
    }
    string cmd,pidlist,pid,res;
    string imgdir;
    if (!is_usesnapshot(disk,opt))
    {
        make_distdirname(vid, disk, STORAGE_INSTANCES_DIR, imgdir);
    }
    else
    {
        if (IMAGE_TYPE_DATABLOCK != disk._type)
        {
            imgdir = STORAGE_INSTANCES_DIR+to_string<int64>(disk._id,dec);
        }
        else
        {
            imgdir = STORAGE_INSTANCES_DIR+to_string<int64>(disk._id,dec)+"_"+disk._target;
        }
    }
    cmd = "ps x | grep md5sum | grep "+imgdir+" | grep -v grep | awk '{ print $1}'";
    storage_command(cmd,pidlist);
    OutPut(SYS_DEBUG,"kill_checksum vid:%lld pidlist:%s\n",vid,pidlist.c_str());
    stringstream iss(pidlist);
    pid_t tmppid;
    while (getline(iss,pid))
    {
        from_string<pid_t>(tmppid,pid,dec);
        kill(tmppid,9);
    }
}
/******************************************************************************/  
void StorageOperation::kill_checksum(void)
{
    pid_t tmppid;
    string cmd,res,pid,pidlist;

    cmd = "ps x | grep md5sum | grep "+STORAGE_INSTANCES_DIR+" | grep -v grep | awk '{ print $1}'";
    storage_command(cmd,pidlist);
    OutPut(SYS_DEBUG,"kill_checksum all pidlist:%s\n",pidlist.c_str());
    stringstream iss(pidlist);
    while (getline(iss,pid))
    {
        #if 0
        cmd = "kill -9 "+pid;
        storage_command(cmd,res);
        #endif
        from_string<pid_t>(tmppid,pid,dec);
        kill(tmppid,9);
    }
}
/******************************************************************************/  
int StorageOperation::free_loop(int64 vid)
{
    int ret = SUCCESS;
    string cmd,res,loop;

    cmd = "losetup -a | grep -E 'instances/"+to_string<int64>(vid,dec)+"|instances/instance_"+to_string<int64>(vid,dec)+"'"+"|awk -F : '{print $1}'";
    storage_command(cmd,res);
    stringstream iss(res);
    while (getline(iss,loop))
    {
        cmd = "losetup -d "+loop;
        ret = storage_command(cmd,res);
    }
    return ret;
}
/******************************************************************************/  
int StorageOperation::free_tap(int64 vid)
{
    int ret = SUCCESS;
    int pid = -1,minor= -1;
    string cmd,res,tap;

    cmd = "/usr/sbin/tap-ctl list | grep -E '/instances/"+to_string<int64>(vid,dec)+"/|/instances/instance_"+to_string<int64>(vid,dec)+"/'"+"|sed -e 's/=/ /g'|awk '{print $2\" \"$4}'";
    if (SUCCESS != storage_command(cmd,res))
    {
        ret = ERROR;
    }
    else
    {
        stringstream iss(res);
        while (getline(iss,tap))
        {
            sscanf(tap.c_str(),"%d %d",&pid,&minor);
            #if 0
            cmd = "/usr/sbin/tap-ctl close -p "+to_string<int>(pid,dec)+" -m "+to_string<int>(minor,dec)+" -f";
            if (SUCCESS != storage_command(cmd,res))
            {
                ret = ERROR;
            }
            else
            {
                OutPut(SYS_DEBUG,"free_tap %s success\n",cmd.c_str());
            }
            #endif
            #if 0
            cmd = "/usr/sbin/tap-ctl detach -p "+to_string<int>(pid,dec)+" -m "+to_string<int>(minor,dec);
            #endif 
            #if 1
            cmd = "kill -9 "+to_string<int>(pid,dec);
            #endif
            if (SUCCESS != storage_command(cmd,res))
            {
                ret = ERROR;
            }
            else
            {
                OutPut(SYS_DEBUG,"free_tap %s success\n",cmd.c_str());
            }
            cmd = "/usr/sbin/tap-ctl free -m "+to_string<int>(minor,dec);
            if (SUCCESS != storage_command(cmd,res))
            {
                ret = ERROR;
            }
            else
            {
                OutPut(SYS_DEBUG,"free_tap %s success\n",cmd.c_str());
            }
        }
    }
    return ret;
}
/******************************************************************************/  
int StorageOperation::free_imglv(const string &imglv)
{
    int ret = ERROR;
    int64 vid;
    VmDiskConfig disk;
    ImageStoreOption opt;

    opt._img_srctype = IMAGE_SOURCE_TYPE_BLOCK;
    if (SUCCESS == fill_disk(imglv,vid,disk,opt))
    {
        ret = free_imglv(vid,disk);
    }
    return ret;
}
/******************************************************************************/  
int StorageOperation::free_imglv(int64 vid,const VmDiskConfig &disk)
{
    int ret = SUCCESS,rettmp = SUCCESS;
    int retry = 0;
    
    OutPut(SYS_WARNING,"free_imglv delay %d seconds for save free\n",SAVE_FREE_DELAY_TIME/1000);
    Delay(SAVE_FREE_DELAY_TIME);
    do
    {
        rettmp = free_loop(vid);
        if(SUCCESS != rettmp)
        {
            //并发释放的时候 可能会出现因xend来不及处理导致loop释放不了的情况，这儿做特殊处理，延时1秒 再尝试
            ret = ERROR;
            Delay(1000);
            OutPut(SYS_WARNING,"free_loop failed! vid:%lld \n",vid);
        }
        retry++;
    }while((retry < 10) && (SUCCESS != rettmp));

    if(IsXenKernel() && (IMAGE_FORMAT_VHD == disk._disk_format))
    {
        retry = 0;
        do
        {
            rettmp = free_tap(vid);
            if(SUCCESS != rettmp)
            {
                //并发释放的时候 可能会出现因xend来不及处理导致tap释放不了的情况，这儿做特殊处理，延时1秒 再尝试
                ret = ERROR;
                Delay(1000);
                OutPut(SYS_WARNING,"free_tap failed! vid:%lld \n",vid);
            }
            retry++;
        }while((retry < 10) && (SUCCESS != rettmp));
    }
    
    //umount操作 并清除相关目录
    string vmimgdir;
    make_distdirname(vid,disk,STORAGE_INSTANCES_DIR,vmimgdir);
    if (file_isexist(vmimgdir) && check_mount(vmimgdir))
    {
        if (SUCCESS != sys_umount(vmimgdir))
        {
            OutPut(SYS_ERROR,"free_imglv sys_umount %s failed!\n",vmimgdir.c_str());
            ret = ERROR;
        }
    }
    sys_rm(vmimgdir);
    string vmiddir = STORAGE_INSTANCES_DIR + to_string<int64>(vid,dec);
    
    /* 处理镜像备份对应的文件夹*/
    DIR                 *pDir; 
    struct dirent       *pDirEnt;   /* ptr to dirent */
    if ((pDir = opendir (vmiddir.c_str())) == NULL)
    {
        OutPut(SYS_ERROR,"Can't open \"%s\".\r\n", vmiddir.c_str());
    }
    else
    {
        string vmbakupdir;
        while ( (OK == ret) && (NULL != (pDirEnt = readdir(pDir))) )
        {
            vmbakupdir = pDirEnt->d_name;
            vmbakupdir = vmiddir + "/" + vmbakupdir;
            if (file_isexist(vmbakupdir) && check_mount(vmbakupdir))
            {
                if (SUCCESS != sys_umount(vmbakupdir))
                {
                    OutPut(SYS_ERROR,"free_imglv sys_umount %s failed!\n",vmbakupdir.c_str());
                    ret = ERROR;
                }
            }
            sys_rm(vmbakupdir);
        }
        closedir (pDir);
    }

    sys_rmdir(vmiddir);

    if (STORAGE_POSITION_LOCAL == disk._position)
    {//本地的需要回收imglv 重命名baselv到freelv
        string imglv;
        make_distname(vid,disk,LVM_IMG_PREFIX,imglv);
        if (isExistLVName(imglv,false))
        {
            /* 删除lv */
            int delret = SUCCESS;
            delret = deleteLV(imglv,false);
            if((SUCCESS != delret) && (ERROR_DEVICE_NOT_EXIST != delret))
            {
                /*释放逻辑卷失败*/
                OutPut(SYS_ERROR,"free_imglv deleteLV %s failed!\n",imglv.c_str());
                ret = ERROR;
            }
            else
            {
                OutPut(SYS_WARNING,"free_imglv deleteLV %s success!\n",imglv.c_str());
            }
        }

        /*  删除镜像备份对应的lv */
        string res,backuplv,reservlv;
        string backup_tag=LVM_BACKUP_PREFIX+to_string<int64>(vid,dec)+"_"+disk._target+"_";
        string reserv_tag=LVM_RESERVED_PREFIX+to_string<int64>(vid,dec)+"_"+disk._target+"_";

        if (SUCCESS == findLV(backup_tag,res))
        {
            stringstream iss(res);
            while (getline(iss,backuplv))
            {
                if (isExistLVName(backuplv,false))
                {
                    /* 删除lv */
                    int delret = SUCCESS;
                    delret = deleteLV(backuplv,false);
                    if((SUCCESS != delret) && (ERROR_DEVICE_NOT_EXIST != delret))
                    {
                        /*释放逻辑卷失败*/
                        OutPut(SYS_ERROR,"free_imglv deleteLV %s failed!\n",backuplv.c_str());
                        ret = ERROR;
                    }
                    else
                    {
                        OutPut(SYS_WARNING,"free_imglv deleteLV %s success!\n",backuplv.c_str());
                    }
                }
            }
        }

        res ="";
        /*  删除镜像备份对应的预留lv */
        if (SUCCESS == findLV(reserv_tag,res))
        {
            stringstream iss(res);
            while (getline(iss,reservlv))
            {
                if (isExistLVName(reservlv,false))
                {
                    /* 删除lv */
                    int delret = SUCCESS;
                    delret = deleteLV(reservlv,false);
                    if((SUCCESS != delret) && (ERROR_DEVICE_NOT_EXIST != delret))
                    {
                        /*释放逻辑卷失败*/
                        OutPut(SYS_ERROR,"free_imglv deleteLV %s failed!\n",reservlv.c_str());
                        ret = ERROR;
                    }
                    else
                    {
                        OutPut(SYS_WARNING,"free_imglv deleteLV %s success!\n",reservlv.c_str());
                    }
                }
            }
        }

        /*如果该base没有快照了 则命名为free 可回收*/
        if (SUCCESS != rename_freelv(disk._id))
        {
            ret = ERROR;
        }
    }
    
    return ret;
}
/******************************************************************************/  
int StorageOperation::recover_imglv(int64 vid,const VmDiskConfig &disk)
{
    /*1 判断是否存在备份lv */
    string res,backuplv;
    string backup_tag=LVM_BACKUP_PREFIX+to_string<int64>(vid,dec)+"_"+disk._target+"_";

    if (SUCCESS == findLV(backup_tag,res))
    {
        stringstream iss(res);
        while (getline(iss,backuplv))
        {
            if (isExistLVName(backuplv,false))
            {
                OutPut(SYS_DEBUG,"vm:%lld exist backup image,not allow to recover \n",vid);
                return ERROR;
            }
        }
    }

    /*2 删除镜像快照 */
    int ret = SUCCESS,rettmp = SUCCESS;
    int retry = 0;
    
    OutPut(SYS_WARNING,"free_imglv delay %d seconds for save free\n",SAVE_FREE_DELAY_TIME/1000);
    Delay(SAVE_FREE_DELAY_TIME);
    do
    {
        rettmp = free_loop(vid);
        if(SUCCESS != rettmp)
        {
            //并发释放的时候 可能会出现因xend来不及处理导致loop释放不了的情况，这儿做特殊处理，延时1秒 再尝试
            ret = ERROR;
            Delay(1000);
            OutPut(SYS_WARNING,"free_loop failed! vid:%lld \n",vid);
        }
        retry++;
    }while((retry < 10) && (SUCCESS != rettmp));

    if(IsXenKernel() && (IMAGE_FORMAT_VHD == disk._disk_format))
    {
        retry = 0;
        do
        {
            rettmp = free_tap(vid);
            if(SUCCESS != rettmp)
            {
                //并发释放的时候 可能会出现因xend来不及处理导致tap释放不了的情况，这儿做特殊处理，延时1秒 再尝试
                ret = ERROR;
                Delay(1000);
                OutPut(SYS_WARNING,"free_tap failed! vid:%lld \n",vid);
            }
            retry++;
        }while((retry < 10) && (SUCCESS != rettmp));
    }
    
    //umount操作 并清除相关目录
    string vmimgdir;
    make_distdirname(vid,disk,STORAGE_INSTANCES_DIR,vmimgdir);
    if (file_isexist(vmimgdir) && check_mount(vmimgdir))
    {
        if (SUCCESS != sys_umount(vmimgdir))
        {
            OutPut(SYS_ERROR,"free_imglv sys_umount %s failed!\n",vmimgdir.c_str());
            ret = ERROR;
        }
    }
    sys_rm(vmimgdir);
    string vmiddir = STORAGE_INSTANCES_DIR + to_string<int64>(vid,dec);
    sys_rmdir(vmiddir);

    if (STORAGE_POSITION_LOCAL == disk._position)
    {//本地的需要回收imglv 重命名baselv到freelv
        string imglv;
        make_distname(vid,disk,LVM_IMG_PREFIX,imglv);
        if (isExistLVName(imglv,false))
        {
            /* 删除lv */
            int delret = SUCCESS;
            delret = deleteLV(imglv,false);
            if((SUCCESS != delret) && (ERROR_DEVICE_NOT_EXIST != delret))
            {
                /*释放逻辑卷失败*/
                OutPut(SYS_ERROR,"free_imglv deleteLV %s failed!\n",imglv.c_str());
                ret = ERROR;
            }
            else
            {
                OutPut(SYS_WARNING,"free_imglv deleteLV %s success!\n",imglv.c_str());
            }
        }        

        /*如果该base没有快照了 则命名为free 可回收*/
        if (SUCCESS != rename_freelv(disk._id))
        {
            ret = ERROR;
        }
    }
    
    return ret;
}

/******************************************************************************/  
int StorageOperation::rename_freelv(int64 iid)
{
    int ret = SUCCESS;    
    Mutex  *pLvMutex = NULL;
    if(SUCCESS == getLvMutex(&pLvMutex))
    {
        /*加lv锁*/
        pLvMutex->Lock();
        string baselv = LVM_BASE_PREFIX+to_string<int64>(iid,dec);
        if (isExistLVName(baselv,false) && (0 == getSnapShotCount(baselv,false)))
        {
            /*baselv 存在 且没有快照 则命名为 freelv 表示该base空闲可被回收*/
            string freelv = LVM_FREE_PREFIX+to_string<int64>(iid,dec);
            if (renameLV(baselv,freelv))
            {
                OutPut(SYS_WARNING,"rename_freelv %s to %s success!\n",baselv.c_str(),freelv.c_str());
            }
            else
            {
                OutPut(SYS_ERROR,"rename_freelv %s to %s failed!\n",baselv.c_str(),freelv.c_str());
                ret = ERROR;
            }
        }
        pLvMutex->Unlock();
    }
    else
    {
        OutPut(SYS_ERROR,"rename_freelv getLvMutex failed!\n");
        ret = ERROR;
    } 
    return ret;
}
/******************************************************************************/  
int StorageOperation::rename_baselv(void)
{
    int64 iid = INVALID_FILEID;
    string cmd,res;
    string fmt;
    string baselv,freelv;
    int ret = SUCCESS;    
    Mutex  *pLvMutex = NULL;
    if(SUCCESS == getLvMutex(&pLvMutex))
    {
        /*加lv锁*/
        pLvMutex->Lock();

        cmd = "lvs  --noheadings -o lv_name 2> /dev/null | grep "+LVM_BASE_PREFIX+" | grep -v " + LVM_TMP_PREFIX;
        
        if (SUCCESS != storage_command(cmd, res))
        {
            pLvMutex->Unlock();
            return ERROR;
        }
        stringstream iss(res);
        while (getline(iss,baselv))
        {
            if (isExistLVName(baselv,false) && (0 == getSnapShotCount(baselv,false)))
            {
                /*baselv 存在 且没有快照 则命名为 freelv 表示该base空闲可被回收*/
                fmt = LVM_BASE_PREFIX+"%lld";
                sscanf(baselv.c_str(),fmt.c_str(),&iid);
                freelv = LVM_FREE_PREFIX+to_string<int64>(iid,dec);
                if (renameLV(baselv,freelv))
                {
                    OutPut(SYS_WARNING,"rename_baselv renameLV %s to %s success!\n",baselv.c_str(),freelv.c_str());
                }
                else
                {
                    OutPut(SYS_ERROR,"rename_baselv renameLV %s to %s failed!\n",baselv.c_str(),freelv.c_str());
                    ret = ERROR;
                }
            }
        }
        pLvMutex->Unlock();
    }
    else
    {
        OutPut(SYS_ERROR,"rename_baselv getLvMutex failed!\n");
        ret = ERROR;
    } 
    return ret;
}
/******************************************************************************/  
int StorageOperation::check_lv_exist(int64 iid, int basefree)
{
    int ret = ERROR;    
    Mutex  *pLvMutex = NULL;
    string lv;
    int baseorfree = basefree;

    if(SUCCESS == getLvMutex(&pLvMutex))
    {
        /*加lv锁*/
        pLvMutex->Lock();

        if(1 == baseorfree)
        {
            lv = LVM_BASE_PREFIX+to_string<int64>(iid,dec);
        }
        else
        {
            lv = LVM_FREE_PREFIX+to_string<int64>(iid,dec);
        }
        if (isExistLVName(lv,false))
        {
            /*lv 存在 */
            ret = SUCCESS;
        }
        else
        {
            ret = ERROR;
        }
        
        pLvMutex->Unlock();
    } 
    return ret;
}
/******************************************************************************/  
int StorageOperation::create_lv(const string &lv,int64 size,const string &base,int64 id,string &res)
{
    uint32 freespace = 0;
    uint32 needspace = (size % LVM_MBYTE ==0) ?(size / LVM_MBYTE) :(size / LVM_MBYTE + 1);

    getVgFreeSpace(freespace);
    if (freespace < needspace)
    {
        bool ret = false;
        Mutex  *pLvMutex = NULL;
        if(SUCCESS != getLvMutex(&pLvMutex))
        {
            return ERROR;
        }
        pLvMutex->Lock();
        ret = recycleBaseLV(needspace,id);
        pLvMutex->Unlock();
        if (!ret)
        {
            OutPut(SYS_ERROR,"create_lv failed for no space\n"); 
            return ERROR_NO_SPACE;
        }
    }
    int ret = createLV(lv,base,size,res);
    if (ERR_EXIST == ret)
    {
        if (!check_lvsize(lv,size,0))
        {//大小不等 直接返回失败
            OutPut(SYS_ERROR,"create_lv failed for %s exist but size mismatch\n",lv.c_str());
            return ERROR;
        }
        else
        {
            fullnameofLV(lv,res);
            return SUCCESS;
        }
    }
    return ret;
}
/******************************************************************************/  
int StorageOperation::alloc_lv(int64 vid,VmDiskConfig& disk,const ImageStoreOption& opt)
{
    string lv = LVM_BLOCK_PREFIX+to_string<int64>(vid,dec)+"_"+disk._target; 

    if (isExistLVName(lv,false))
    {
        if (!check_lvsize(lv,disk._size,0))
        {//大小不等 直接返回失败
            OutPut(SYS_ERROR,"alloc_lv failed for %s exist but size mismatch\n",lv.c_str());
            return ERROR;
        }
        else
        {
            fullnameofLV(lv,disk._source);
            return SUCCESS;
        }
    }
    return create_lv(lv,disk._size,"",(int64)INVALID_FILEID,disk._source);

}
/******************************************************************************/  
void StorageOperation::calc_lvsize(int64 size,int exp,int64 &lvsize)
{
    lvsize = (size*(exp+100))/100;
}
bool StorageOperation::check_lvsize(const string &lv,int64 size,int factor)
{
    int64 lvsize = 0,lvshouldsize = 0,lvexpandsize = 0;

    if (lv.empty())
    {
        return false;
    }

    lvexpandsize = (factor*size)/100;
    lvshouldsize = lvexpandsize + size;
    alignsizeofLV(lvshouldsize);
    getsizeofLV(lvsize,lv);
    OutPut(SYS_DEBUG,"check_lvsize %s should:%lld,but:%lld\n",lv.c_str(),lvshouldsize,lvsize);
    return (lvsize == lvshouldsize)?true:false;
}
/******************************************************************************/  
bool StorageOperation::check_lvvalid(string lv,int64 size,int exp)
{
    bool vaild = false;

    if (lv.empty())
    {
        return vaild;
    }
    if (isExistLVName(lv,false))
    {
        if (check_lvsize(lv,size,exp))
        {/*合法*/
            OutPut(SYS_DEBUG,"%s exist and size match\n",lv.c_str());
            vaild = true;
        }
    }
    return vaild;
}
/******************************************************************************/  
int StorageOperation::load_share(int64 vid,VmDiskConfig &disk)
{
    int ret = ERROR;
    string device;
    ret = LoadShareDisk(vid,disk._target,disk._share_url.access_option,disk._fstype,device);   
    if (SUCCESS == ret ||ret ==ERROR_LOAD_NEED_REFRESH)
    {
        disk._source_type = STORAGE_SOURCE_TYPE_BLOCK;
        disk._source = device;
    }
    return ret;
}
/******************************************************************************/  
int StorageOperation::unload_share(int64 vid,const VmDiskConfig &disk)
{
    int ret = SUCCESS;
    ret = UnLoadShareDisk(vid, disk._target, disk._share_url.access_option);
    return ret;
}
/******************************************************************************/  
int StorageOperation::assign_work(int64 vid,const VmDiskConfig &disk,const ImageStoreOption &opt)
{
    string name;
    make_distname(vid,disk,"ImgDownLoadTask_",name);
    ImgDownLoadTask *task = new ImgDownLoadTask(name,vid,disk,opt);
    if (task)
    {
        return _thread_pool->AssignWork(task);
    }

    return ERROR;
}
/******************************************************************************/  
int StorageOperation::cancel_work(int64 vid,const VmDiskConfig &disk)
{
    string name;
    make_distname(vid,disk,"ImgDownLoadTask_",name);

    return _thread_pool->CancelWork(name);
}
/******************************************************************************/  
bool StorageOperation::find_work(int64 vid,const VmDiskConfig &disk)
{
    string name;
    make_distname(vid,disk,"ImgDownLoadTask_",name);

    return _thread_pool->FindWork(name);
}
/******************************************************************************/  
int StorageOperation::assign_upload_work(int64 vid,const VmSaveReq &req,const VmDiskConfig &disk,const ImageStoreOption &opt)
{
    string name;
    make_distname(vid,disk,"ImgUpLoadTask_",name);
    ImgUpLoadTask *task = new ImgUpLoadTask("ImgUpLoadTask",vid,req,disk,opt);
    if (task)
    {
        return _thread_pool_upload->AssignWork(task);
    }

    return ERROR;
}
/******************************************************************************/  
int StorageOperation::cancel_upload_work(int64 vid,const VmDiskConfig &disk)
{
    string name;
    make_distname(vid,disk,"ImgUpLoadTask_",name);

    return _thread_pool_upload->CancelWork(name);
}
/******************************************************************************/  
int StorageOperation::clear_tmpresource(int64 vid,const VmDiskConfig &disk,const ImageStoreOption &opt)
{
    int ret = SUCCESS;
    string imgdir,imgname,tmplv;
    if (!is_usesnapshot(disk,opt))
    {
        if (SUCCESS != clear_nfsmount(vid))
        {
            ret = ERROR;
        }
        make_imgname(vid,disk,imgdir,imgname);
        make_distname(vid,disk,LVM_TMP_PREFIX+LVM_IMG_PREFIX,tmplv);
    }
    else
    {
        if (SUCCESS != clear_nfsmount(disk._id))
        {
            ret = ERROR;
        }
        make_basename(disk._id, imgdir, imgname);
        tmplv = LVM_TMP_PREFIX+LVM_BASE_PREFIX+to_string<int64>(disk._id,dec);
    }
    if (check_mount(imgdir))
    {
        if (SUCCESS != sys_umount(imgdir,"-l"))
        {
            ret = ERROR;
        }
    }
    if (isExistLVName(tmplv))
    {
        int delret = SUCCESS;
        delret = deleteLV(tmplv,false);
        if((SUCCESS != delret) && (ERROR_DEVICE_NOT_EXIST != delret))
        {
            OutPut(SYS_ERROR,"clear_tmpresource deleteLV %s failed!\n",tmplv.c_str());
            ret = ERROR;
        }
        else
        {
            OutPut(SYS_WARNING,"clear_tmpresource deleteLV %s success!\n",tmplv.c_str());
        }
    }
    return ret;
}
/******************************************************************************/  
int StorageOperation::clear_nfsmount(void)
{
    int ret = SUCCESS;
    struct mntent tMnt;
    FILE *fp = NULL;
    string nfsmountpoint;
    char acBuf[4096];

    fp = setmntent("/proc/mounts", "r");
    if (NULL == fp)
    {
        return ERROR;
    }

    while ((NULL != getmntent_r(fp, &tMnt, acBuf, 4096)))
    {
        if (NULL != strstr(tMnt.mnt_dir,STORAGE_HC_MOUNT_DIR.c_str())
            && 0 == strcmp(tMnt.mnt_type,MNTTYPE_NFS))   
        {
            nfsmountpoint = tMnt.mnt_dir;
            ret = UnMountNfs(nfsmountpoint);
            if (SUCCESS != ret)
            {
                break;
            }
        }
    }

    endmntent(fp);
    return ret;
}
/******************************************************************************/  
int StorageOperation::clear_nfsmount(int64 id)
{
    int ret = SUCCESS;
    string cmd,res;
    string nfsmountpoint = STORAGE_HC_MOUNT_DIR+"/"+to_string<int64>(id, dec);

    if(file_isexist(nfsmountpoint))
    {
        if (SUCCESS != UnMountNfs(nfsmountpoint))
        {
            OutPut(SYS_ERROR,"UnMountNfs %s failed\n",nfsmountpoint.c_str());
            ret = ERROR;
        }
        else
        {
            OutPut(SYS_DEBUG,"UnMountNfs %s successfully\n",nfsmountpoint.c_str());
        }

        if (!check_mount(nfsmountpoint))
        {
            struct stat64 s;
            lstat64(nfsmountpoint.c_str(),&s);
            if(!S_ISDIR(s.st_mode))
            {
                sys_rm(nfsmountpoint);
            }
            else
            {
                sys_rmdir(nfsmountpoint);
            }
        }
    }
    return ret;
}
/******************************************************************************/  
void StorageOperation::make_distname(int64 vid,const VmDiskConfig &disk,const string &prefix,string &name)
{
    if (IMAGE_TYPE_DATABLOCK == disk._type)
    {
        name = prefix+to_string<int64>(vid,dec)+"_"+to_string<int64>(disk._id,dec)+"_"+disk._target;
    }
    else
    {
        name = prefix+to_string<int64>(vid,dec)+"_"+to_string<int64>(disk._id,dec);
    }  
}
/******************************************************************************/  
void StorageOperation::make_distdirname(int64 vid,const VmDiskConfig &disk,const string &prefix,string &name)
{
    if (IMAGE_TYPE_DATABLOCK != disk._type)
    {
        name = prefix + to_string<int64>(vid,dec)+"/"+to_string<int64>(disk._id,dec);
    }
    else
    {
        name = prefix + to_string<int64>(vid,dec)+"/"+to_string<int64>(disk._id,dec)+"_"+disk._target;
    }
}
/******************************************************************************/  
int StorageOperation::make_basename(int64 iid,string &baseimgdir,string &baseimg)
{
    string dirstr;
 
    /*创建虚拟机挂载基础映像的目录*/
    dirstr = STORAGE_INSTANCES_DIR+to_string<int64>(iid,dec);
    if (!file_isexist(dirstr))
    {
        if (SUCCESS != sys_mkdir(dirstr))
        {
            return ERROR;
        }
    }

    baseimgdir = dirstr;
    baseimg = baseimgdir+"/"+to_string<int64>(iid,dec)+".img";
    return SUCCESS;
}
/******************************************************************************/  
int StorageOperation::make_imgname(int64 vid,const VmDiskConfig &disk,string & imgdir,string & img)
{
    string dirstr;

    make_distdirname(vid,disk,STORAGE_INSTANCES_DIR,dirstr);
    if (!file_isexist(dirstr))
    {
        if (SUCCESS != sys_mkdir(dirstr))
        {
            return ERROR;
        }
    }
    imgdir = dirstr;
    img = imgdir+"/"+to_string<int64>(disk._id,dec)+".img";
    return SUCCESS;
}
/******************************************************************************/  
void StorageOperation::make_imglvname(int64 vid,const VmDiskConfig &disk,bool fullname,string &imglv)
{
    string name;
    if (IMAGE_POSITION_LOCAL == disk._position)
    {//本地存储的镜像lv名
        make_distname(vid,disk,LVM_IMG_PREFIX,name);
    }
    else
    {//共享存储的镜像lv名
        imglv=STORAGE_DIR;       
        imglv.append("iscsi_");
        if (INVALID_VID != vid)
        {
            imglv.append(to_string<int64>(vid,dec));
            imglv.append("_");
        }
        imglv.append(disk._target);
        return;
    }

    if (fullname)
    {
        fullnameofLV(name, imglv);
    }
    else
    {
        imglv = name;
    }
}
/******************************************************************************/  
int StorageOperation::get_diskstate(vector<DiskStateInfo> &state_info)
{
    vector<T_DmPdSmartInfo> smartInfo;
    T_DmPdSmartInfo         tmpSmartInfo;
    DiskStateInfo           diskState;
    int ret;
   
    state_info.clear();
  
    /*611002948726 临时去掉smart检测 接口直接返回空*/
    return SUCCESS;


    ret = DiskSmart::getSmartInfo(smartInfo);
    if(_SMART_SUCCESS != ret)
    {
        cout << "DiskSmart::getSmartInfo failed ret:" << ret << endl;
        return ERROR;
    }
 
    for(vector<T_DmPdSmartInfo>::iterator it = smartInfo.begin(); it != smartInfo.end(); it++)
    {
        tmpSmartInfo = *it;
        diskState._name = tmpSmartInfo.aucPdName;
        diskState._state = (PD_GOOD == tmpSmartInfo.ucSmartStatus) ? DISKSTATE_GOOD : DISKSTATE_BROKEN;
        state_info.push_back(diskState);
    }
    return SUCCESS;
}
/******************************************************************************/  
int StorageOperation::get_vginfo(TVGSpaceTotalInfo &vginfo)
{
    if (getVGTotalInfo(vginfo))
    {
        return SUCCESS;
    }
    else
    {
        return ERROR;
    }
}
/******************************************************************************/  
bool StorageOperation::is_usesnapshot(const VmDiskConfig &disk, const ImageStoreOption &opt)
{
    return ((IMAGE_POSITION_LOCAL == disk._position) && (IMAGE_USE_SNAPSHOT == opt._img_usage))
        || ((IMAGE_POSITION_SHARE == disk._position) && (IMAGE_USE_SNAPSHOT == opt._share_img_usage));
}
/******************************************************************************/  
bool StorageOperation::is_imageformatraw(const string& img)
{
    string disk_format = IMAGE_FORMAT_RAW;
    int64 disk_size;
    if (SUCCESS != CheckImgFormatAndSize(img,disk_format,disk_size))
    {
        return true;
    }
    return (IMAGE_FORMAT_RAW == disk_format);
}
/******************************************************************************/  
int StorageOperation::check_imgvalid(const string &name,const VmDiskConfig &disk,bool need)
{
    bool sizeok = false;
    int ret = ERROR;
    int filetype = FILETYPE_UNKNOWN;

    /*1:检查大小是否对*/
    if ((SUCCESS == get_filetype(name,filetype))
        && (FILETYPE_REG == filetype))
    {    
        struct stat64 tStat;
        if (-1 != lstat64(name.c_str(),&tStat))
        {
            if (!is_imageformatraw(name))
            {
                sizeok = true;
                OutPut(SYS_DEBUG,"check_imgvalid img %s not raw\n",name.c_str());
            }
            else
            {
                OutPut(SYS_DEBUG,"check_imgvalid img %s raw\n",name.c_str());
                if (disk._size == tStat.st_size)
                {
                    sizeok = true;
                }
                else
                {
                    OutPut(SYS_ERROR,"check_imgvalid img %s raw size mismatch,should:%ld,but:%ld\n",disk._size,tStat.st_size);
                }
            }
        }  
        else
        {
            OutPut(SYS_ERROR,"check_imgvalid lstat64 %s failed for %s\n",name.c_str(),strerror(errno));
        }
    }
    else
    {
        OutPut(SYS_ERROR,"check_imgvalid get_filetype %s failed for %s\n",name.c_str(),strerror(errno));
    }
    /*2:如果大小对的话*/
    if (sizeok)
    {
        /*2.1:判断是否需要检查md5 当url中的md5为空则不需要 need为false也不需要*/
        if (!disk._url.checksum.empty() && need)
        {
            /*2.1.1:计算md5*/
            OutPut(SYS_DEBUG,"call sys_md5 calc %s checksum,should %s\n",name.c_str(),disk._url.checksum.c_str());
            string checksum;
            if(SUCCESS == sys_md5(name,checksum))
            {
                /*2.1.1.1:md5计算成功,判断md5是否一致*/
                if (checksum == disk._url.checksum)
                {
                    ret = SUCCESS;        
                }
                else
                {
                    OutPut(SYS_ERROR,"%s checksum mismatch,should:%s,but:%s\n",name.c_str(),disk._url.checksum.c_str(),checksum.c_str());
                }
            }
            else
            {
                /*2.1.1.2:md5计算失败*/
                OutPut(SYS_ERROR,"sys_md5 failed\n");
            }
        }
        else
        {
            /*2.1.2:大小相同且不需要计算checksum*/
            ret = SUCCESS;
        }
    }
    return ret;
}
/******************************************************************************/  
int StorageOperation::img_source_file(int64 vid,const VmDiskConfig &disk,const string &target,string &imgname)
{
    string imgdir,tmpimgname;
    string  real_target;

    /*1:构建挂载点和挂载后的镜像名*/
    if (SUCCESS != make_imgname(vid,disk,imgdir,tmpimgname))
    {
        return ERROR;
    }   
    
    string name=LVM_BACKUP_PREFIX+to_string<uint32>(vid,dec)+"_"+disk._target+"_"+disk._request_id;
    if(isExistLVName (name,false))
    {
        imgdir=STORAGE_INSTANCES_DIR + to_string<int64>(vid,dec)+"/"+disk._request_id;
        if (!file_isexist(imgdir))
        {
            if (SUCCESS != sys_mkdir(imgdir))
            {
                return ERROR;
            }
        }
        imgname = imgdir+"/"+to_string<int64>(disk._id,dec)+".img";
        fullnameofLV(name,real_target);
    }
    else
    {
        if (imgname.empty())
        {
            imgname = tmpimgname;
        }
        
        real_target=target;
    }
    
    /*2:检查挂载点是否mount过*/
    if (!check_mount(imgdir))
    {
        /*2.1:如果没有mount过 则执行mount操作*/
        if (SUCCESS != sys_mount(real_target,imgdir))
        {
            return ERROR;
        }   
    }

    /*3:检查快照里面的映像文件是否合法 通过检查大小是否匹配来看*/
    if (SUCCESS != check_imgvalid(imgname, disk, false))
    {
        /*3.1:快照里面的映像文件不存在或者大小不对 直接返回创建失败 定位问题*/
        OutPut(SYS_ERROR,"img_source_file imglv %s exist but img %s invalid\n",target.c_str(),imgname.c_str());
        return ERROR;
    }
    
    return SUCCESS;
}
/******************************************************************************/  
void StorageOperation::clear_disk(VmDiskConfig &disk)
{
    disk._id = INVALID_FILEID;
    disk._size = 0;
    disk._disk_size = 0;
    disk._disk_format.clear();
    disk._position = -1;
    disk._type.clear();
    disk._bus.clear();
    disk._target.clear();
    disk._source.clear();
    disk._driver.clear();
    disk._fstype.clear();
    disk._source_type.clear();
    disk._url.path.clear();
    disk._url.checksum.clear();
    disk._url.access_type.clear();
    disk._url.access_option.clear();
}
/******************************************************************************/  
int StorageOperation::fill_disk(const string &lv,int64 &vid,VmDiskConfig &disk,const ImageStoreOption &opt)
{
    int ret = SUCCESS;
    string cmd,res;

    cmd = "echo "+lv+" | awk -F '_' '{print $1\"\\n\"$2\"\\n\"$3\"\\n\"$4}'";
    if (SUCCESS != storage_command(cmd,res))
    {
        OutPut(SYS_ERROR,"fill_disk storage_command,cmd:%s,res:%s\n",cmd.c_str(),res.c_str());
        return ERROR;
    }
    stringstream iss(res);
    int i = 0;
    string tag,id,type,target;
    while (getline(iss,tag))
    {
        switch(i)
        {
            case 0:
                if ("i" == tag)
                {
                    type = STORAGE_SOURCE_TYPE_FILE;
                }
                else
                {
                    type = STORAGE_SOURCE_TYPE_BLOCK;
                }
                break;
            case 1:
                from_string<int64>(vid,tag,dec);
                break;
            case 2:
                if (STORAGE_SOURCE_TYPE_FILE == type)
                {
                    id = tag;
                }
                else
                {
                    target = tag;
                }
                break;
            case 3: 
                if (!tag.empty())
                {
                    target = tag;
                }
                break;
            default:
                break;
        }
        i++;
    }
    if (STORAGE_SOURCE_TYPE_FILE == type)
    {
        if(!from_string<int64>(disk._id,id,dec))
        {
            OutPut(SYS_ERROR,"fill_disk from_string %s failed\n",id.c_str());
            return ERROR;
        } 
        if (!target.empty())
        {
            disk._type = IMAGE_TYPE_DATABLOCK;
        }
    }
    else
    {
        disk._id = INVALID_FILEID;
    }
    if (!target.empty())
    {
        disk._target = target;
    }
    getsizeofLV(disk._size,lv);
    if ((IMAGE_SOURCE_TYPE_FILE == opt._img_srctype) 
        && (STORAGE_SOURCE_TYPE_FILE == type))
    {
        bool needumount = false;
        string imgdir,imgname;
        struct stat64 tStat;
        make_imgname(vid,disk,imgdir,imgname);
        if (!check_mount(imgdir))
        {
            string fullname;
            fullnameofLV(lv,fullname);
            ret = sys_mount(fullname,imgdir);
            if (SUCCESS != ret)
            {
                OutPut(SYS_ERROR,"fill_disk sys_mount %s to %s failed\n",fullname.c_str(),imgdir.c_str());
            }   
            needumount = true;
        }
        if (SUCCESS == ret)
        {
            if (-1 != lstat64(imgname.c_str(),&tStat))
            {
                disk._size = tStat.st_size;
                if (INVALID_FILEID != disk._id)
                {
                    string disk_format = IMAGE_FORMAT_RAW;
                    int64 disk_size;
                    CheckImgFormatAndSize(imgname,disk_format,disk_size);
                    if (IMAGE_FORMAT_RAW != disk_format)
                    {
                        disk._disk_size = disk_size;
                        disk._disk_format = disk_format;
                    }
                    else
                    {
                        disk._disk_size = disk._size;
                        disk._disk_format = IMAGE_FORMAT_RAW;
                    }
                }
            } 
            else
            {
                OutPut(SYS_ERROR,"fill_disk lstat64 %s failed,errno:%d\n",imgname.c_str(),errno);
                ret = ERROR;
            }
        }
        if (needumount)
        {
            sys_umount(imgdir);
        }
    }
    if (SUCCESS == ret)
    {
        disk._position = STORAGE_POSITION_LOCAL;
    }
    return ret;
}
/******************************************************************************/  
void StorageOperation::find_disk(const string &tag,vector<VmDiskConfig> &disks,const ImageStoreOption &opt)
{
    int64 vid;
    string res,lv;
    VmDiskConfig disk;
    
    if (SUCCESS == findLV(tag,res))
    {
        stringstream iss(res);
        while (getline(iss,lv))
        {
            clear_disk(disk);
            if (SUCCESS == fill_disk(lv,vid,disk,opt))
            {//如果填充失败，就认为没有找到
                disks.push_back(disk);
            }
        }
    }
}
/******************************************************************************/  
int StorageOperation::scan_storage(int64 vid,vector<VmDiskConfig> &disks,const ImageStoreOption &opt)
{
    string imglvpre,disklvpre;

    OutPut(SYS_DEBUG,"scan_storage for vm:%lld\n",vid);
    imglvpre = LVM_IMG_PREFIX+to_string<int64>(vid,dec)+"_";
    find_disk(imglvpre,disks,opt);
    
    disklvpre = LVM_BLOCK_PREFIX+to_string<int64>(vid,dec)+"_";
    find_disk(disklvpre,disks,opt);
    return SUCCESS;
}
/******************************************************************************/  
void StorageOperation::fail_storage(int64 iid,int64 vid,bool isalone)
{
    if ((INVALID_FILEID == iid) && (INVALID_VID == vid))
    {
        return;
    }
    lock_download();
    vector<DownLoadCtl>::iterator it;
    for (it = _download_ctl.begin(); it != _download_ctl.end(); it++)
    {
        if (it->_iid == iid)
        {
            break;
        }
    }
    if (it != _download_ctl.end())
    {
        vector<VmInfoOfDLC>::iterator itvm;
        if(false == isalone)
        {//对于是first的节点，还需要设置其他交予它负责的节点状态为fail
            for (itvm = it->_vminfo.begin(); itvm != it->_vminfo.end(); itvm++)
            {
                /*如果有非本地快照方式的在队列里，不设置*/
                if(!((IMAGE_POSITION_LOCAL == itvm->_disk._position) && (IMAGE_USE_SNAPSHOT == itvm->_opt._img_usage)))            
                {                
                    continue;
                }                
                itvm->_state = STORAGE_VM_STATE_FAILED;
            }
        }
        else
        {
            for (itvm = it->_vminfo.begin(); itvm != it->_vminfo.end(); itvm++)
            {
    
                if (itvm->_vid == vid)
                {
                    break;
                }
            }
            if (itvm != it->_vminfo.end())
            {
                itvm->_state = STORAGE_VM_STATE_FAILED;
            }
        }
    }
    unlock_download();
}
/******************************************************************************/  
void StorageOperation::cancel_storage(int64 iid,int64 vid)
{
    if ((INVALID_FILEID == iid) && (INVALID_VID == vid))
    {
        return;
    }
    lock_download();
    vector<DownLoadCtl>::iterator it;
    for (it = _download_ctl.begin(); it != _download_ctl.end(); it++)
    {
        if (it->_iid == iid)
        {
            break;
        }
    }

    if (it != _download_ctl.end())
    {
        vector<VmInfoOfDLC>::iterator itvm;
        for (itvm = it->_vminfo.begin(); itvm != it->_vminfo.end(); itvm++)
        {
            if (itvm->_vid == vid)
            {
                break;
            }
        }
        if (itvm != it->_vminfo.end())
        {
            itvm->_state = STORAGE_VM_STATE_CANCEL;
        }
    }
    unlock_download();
}
/******************************************************************************/  
int StorageOperation::get_storage(int64 vid,VmDiskConfig &disk,const ImageStoreOption &opt)
{
    int ret = ERROR;

    switch (disk._position)
    {
        case STORAGE_POSITION_LOCAL:
            if(INVALID_FILEID != disk._id)
            {/*下载映像*/
                OutPut(SYS_DEBUG,"%s\n",disk._url.serialize().c_str());
                if ((IMAGE_ACCESS_NFS == disk._url.access_type)
                  ||(IMAGE_ACCESS_HTTP == disk._url.access_type))
                {
                    ret = assign_work(vid,disk,opt);
                }
                else
                {
                    OutPut(SYS_ALERT,"get_storage failed for Unknown access type:%s\n",disk._url.access_type.c_str());
                    ret = ERROR_INVALID_ARGUMENT;
                }
            }
            else
            {/*分配本地lv*/
                ret = alloc_lv(vid,disk,opt);
                if (SUCCESS != ret)
                {
                    OutPut(SYS_ERROR,"alloc_lv %s failed\n",disk._source.c_str());
                    return ERROR;
                }
                if (!disk._fstype.empty() 
                    && !check_filesystem(disk._source,disk._fstype))
                {/*fstype是否为空以及块设备是否格式化*/
                    if (SUCCESS != make_filesystem(disk._source,disk._fstype))
                    {
                        OutPut(SYS_ERROR,"make_filesystem %s to %s failed\n",disk._source.c_str(),disk._fstype.c_str());
                        return ERROR;
                    }
                }
                disk._source_type = STORAGE_SOURCE_TYPE_BLOCK; 
            }
            break;
        case STORAGE_POSITION_SHARE:
            /*加载共享盘*/
            if ((INVALID_FILEID != disk._id)
                && (!is_usesnapshot(disk,opt))
                && (find_work(vid,disk)))
            {//本地已经在下载中了 直接返回成功
                return SUCCESS;
            }
            int ret_load;
            ret_load = load_share(vid,disk);
            ret=SUCCESS;
            if ((SUCCESS == ret_load || ERROR_LOAD_NEED_REFRESH == ret_load ) && (INVALID_FILEID != disk._id))
            {//是一个镜像盘 共享镜像盘只有非快照方式的才需要由HC下载镜像
                if (!is_usesnapshot(disk,opt))
                {
                    OutPut(SYS_DEBUG,"%s\n",disk._url.serialize().c_str());
                    if ((IMAGE_ACCESS_NFS == disk._url.access_type)
                      ||(IMAGE_ACCESS_HTTP == disk._url.access_type))
                    {
                        ret = assign_work(vid,disk,opt);
                    }
                    else
                    {
                        OutPut(SYS_ALERT,"get_storage failed for Unknown access type:%s\n",disk._url.access_type.c_str());
                        ret = ERROR_INVALID_ARGUMENT;
                    }
                }
                else
                {
                    if (IMAGE_SOURCE_TYPE_FILE == opt._img_srctype)
                    {
                        string imgname;
                        if (SUCCESS != img_source_file(vid,disk,disk._source,imgname))
                        {
                            ret = ERROR;
                        }
                        else
                        {
                            disk._source = imgname;
                            disk._source_type = STORAGE_SOURCE_TYPE_FILE;
                        }
                    }  
                }
            }
            if(SUCCESS==ret)
            {
                ret=ret_load;
            }
            break;
        default:
            OutPut(SYS_ALERT,"get_storage failed for unknown position:%d\n",disk._position);
            ret = ERROR_INVALID_ARGUMENT;
            break;
    }

    return ret;

}
/******************************************************************************/  
int StorageOperation::release_storage(int64 vid)
{
    int ret = SUCCESS;
    string res,lv;
    string imglvpre,disklvpre;

    OutPut(SYS_DEBUG,"release_storage for vm:%lld\n",vid);
    imglvpre = LVM_IMG_PREFIX+to_string<int64>(vid,dec)+"_";
    if (SUCCESS == findLV(imglvpre,res))
    {
        stringstream iss(res);
        while (getline(iss,lv))
        {
            if (SUCCESS != free_imglv(lv))
            {
                ret = ERROR;
                OutPut(SYS_ERROR,"release_storage free_imglv %s failed\n",lv.c_str());
            }
            else
            {
                OutPut(SYS_NOTICE,"release_storage free_imglv %s success\n",lv.c_str());
            }
        }
    }
    
    disklvpre = LVM_BLOCK_PREFIX+to_string<int64>(vid,dec)+"_";
    if (SUCCESS == findLV(disklvpre,res))
    {
        int delret = SUCCESS;
        stringstream iss(res);
        while (getline(iss,lv))
        {
            delret = deleteLV(lv,false);
            if((SUCCESS != delret) && (ERROR_DEVICE_NOT_EXIST != delret))
            {
                ret = ERROR;
                OutPut(SYS_ERROR,"release_storage deleteLV %s failed\n",lv.c_str());
            }
            else
            {
                OutPut(SYS_WARNING,"release_storage deleteLV %s success\n",lv.c_str());
            }   
        }
    }
    return ret;
}
/******************************************************************************/  
int StorageOperation::release_storage(int64 vid,const VmDiskConfig &disk,const ImageStoreOption &opt)
{
    int ret = SUCCESS;
    
    OutPut(SYS_DEBUG,"release_storage for vm:%lld,id:%lld,source:%s\n",vid,disk._id,disk._source.c_str());
    if (INVALID_FILEID != disk._id)
    {
        cancel_work(vid,disk);
        /*1.1:操作镜像处理队列*/
        if (ERR_HS_DEQUEUE_OK == release_download(vid,disk,opt))
        {
            return SUCCESS;
        }
        
        /*1.2:删除imglv*/
        if (SUCCESS != free_imglv(vid,disk))
        {
            OutPut(SYS_ERROR,"free_imglv vid:%lld iid:%lld failed\n",vid,disk._id);
            ret = ERROR;
        }
        else
        {
            OutPut(SYS_WARNING,"free_imglv vid:%lld iid:%lld success\n",vid,disk._id);
        }
    }
    switch (disk._position)
    {
        case STORAGE_POSITION_LOCAL:
            if(INVALID_FILEID == disk._id)
            {
                bool isfullname = true;
                string source = disk._source;
                if (disk._source.empty())
                {
                    if (disk._target.empty())
                    {
                        OutPut(SYS_ERROR,"release_storage failed for no target\n");
                        return SUCCESS;
                    }
                    source = LVM_BLOCK_PREFIX+to_string<int64>(vid,dec)+"_"+disk._target;
                    isfullname = false;
                }
                if (isExistLVName(source,isfullname))
                {
                    int delret = SUCCESS;
                    delret = deleteLV(source,isfullname);
                    if((SUCCESS != delret) && (ERROR_DEVICE_NOT_EXIST != delret))
                    {
                        OutPut(SYS_ERROR,"release_storage deleteLV %s failed\n",source.c_str());
                        ret = ERROR;
                    }
                    else
                    {
                        OutPut(SYS_WARNING,"release_storage deleteLV %s success\n",source.c_str());
                    }
                }
            }
            break;
        case STORAGE_POSITION_SHARE:
            if ((INVALID_FILEID != disk._id)
                && (!is_usesnapshot(disk,opt))
                && (IMAGE_SOURCE_TYPE_BLOCK == opt._img_srctype))
            {//由于释放lv的时候,里面的数据不会清掉，下次再申请的时候有可能数据还在，所以这里清一下尾部签名.临时方案 逻辑上不是很严谨
             //后续优化以cc下发请求中带的进度为准
                char buf[IMG2BLOCK_SIGNATURE.size()];
                string device = "/dev/"+LVM_TECS_VGNAME+"/"+"iscsi_"+to_string<int64>(vid,dec)+"_"+disk._target;
                bzero(buf,IMG2BLOCK_SIGNATURE.size());
                ret = SetTailBytes(device, buf, IMG2BLOCK_SIGNATURE.size());
                if (SUCCESS != ret)
                {
                    OutPut(SYS_ERROR,"SetTailBytes %s zero failed\n",device.c_str());
                    return ret;
                }
            }
            if (SUCCESS != unload_share(vid,disk))
            {
                OutPut(SYS_ERROR,"unload_share %s failed\n",disk._source.c_str());
                ret = ERROR;
            }
            else
            {
                OutPut(SYS_WARNING,"unload_share %s success\n",disk._source.c_str());
            }
            break;
        default:
            break;
    }

    return ret;

}
/******************************************************************************/  
void StorageOperation::query_storage(int64 vid,VmDiskConfig &disk,const ImageStoreOption &opt,int64 &size,int &state)
{
    if ((INVALID_FILEID == disk._id)
        || ((STORAGE_POSITION_SHARE == disk._position) && (IMAGE_USE_SNAPSHOT == opt._share_img_usage)))
    {/*对于共享快照存储和本地块设备直接返回准备好*/
        size = disk._size;
        state = STORAGE_ITEM_READY;
    }
    else
    {
        VmInfoOfDLC vminfo;
        vminfo._vid = vid;
        if (SUCCESS == check_download(disk._id,vminfo))
        {
            string lv;
            switch (vminfo._state)
            {
                case STORAGE_VM_STATE_FAILED:
                    OutPut(SYS_WARNING,"<progress>get storage failed\n");
                    dec_download(disk._id,vid);
                    state = STORAGE_ITEM_FAILED;
                    break;
                case STORAGE_VM_STATE_READY:
                    OutPut(SYS_DEBUG,"<progress>img storage of %lld for vm%lld ok\n",disk._id,vid);
                    if (STORAGE_POSITION_SHARE == disk._position)
                    {
                        if (IMAGE_SOURCE_TYPE_BLOCK == opt._img_srctype)
                        {//下载好后 块方式在块尾部写签名字节 表示该块上有完整的镜像
                            if (SUCCESS != SetTailBytes(vminfo._disk._source, IMG2BLOCK_SIGNATURE.c_str(), IMG2BLOCK_SIGNATURE.size()))
                            {
                                OutPut(SYS_ERROR,"QueryProgress SetTailBytes failed for %s!",strerror(errno));
                                return;
                            }
                        }
                        else
                        {//文件方式 则将.img.tmp重命名回.img
                            struct stat64 s;
                            string imgsuffix;//镜像后缀，用于判断是否是tmp文件
                            imgsuffix = vminfo._disk._source.substr(vminfo._disk._source.find_last_of('.')+1);
                            
                            if ((0 != lstat64(vminfo._disk._source.c_str(),&s)) || (0==imgsuffix.compare("tmp")))
                            {//.img文件不存在或者为img.tmp文件,需要重命名
                                string tmpfile;
                                if(0==imgsuffix.compare("tmp"))
                                {//是tmp文件
                                    tmpfile = vminfo._disk._source;
                                    vminfo._disk._source = vminfo._disk._source.substr(0,vminfo._disk._source.find_last_of('.'));
						            OutPut(SYS_ERROR,"QueryProgress imgsuffix  %s,source %s\n!",imgsuffix.c_str(),vminfo._disk._source.c_str());
                                }
                                else
                                {//.img文件不存在
                                    tmpfile = vminfo._disk._source+".tmp";
                                }                                
                                
                                if (0 != lstat64(tmpfile.c_str(),&s) || s.st_size != vminfo._disk._size)
                                {
                                    OutPut(SYS_ERROR,"QueryProgress lstat64 failed for %s!",strerror(errno));
                                    return;
                                }
                                else
                                {//重命名.img.tmp文件为.img文件
                                    if (0 != rename(tmpfile.c_str(),vminfo._disk._source.c_str()))  
                                    {
                                        OutPut(SYS_ERROR,"QueryProgress rename failed for %s!",strerror(errno));
                                        return;
                                    }
                                }
                            }
                            else
                            {
                                if (is_imageformatraw(vminfo._disk._source))
                                {
                                    if (s.st_size != vminfo._disk._size)
                                    {//.img文件存在 但大小不等
                                        OutPut(SYS_ALERT,"QueryProgress file %s exist but size mismatch!",vminfo._disk._source.c_str());
                                        return;
                                    }
                                }
                            }
                        }
                    }
                    size = disk._size;
                    state = STORAGE_ITEM_READY;
                    disk._source = vminfo._disk._source;
                    disk._source_type = vminfo._disk._source_type;
                    dec_download(disk._id,vid);
                    break;
                case STORAGE_VM_STATE_DEPLOY:
                    if (STORAGE_POSITION_LOCAL == disk._position)
                    {
                        make_distname(vid,disk,LVM_IMG_PREFIX,lv);
                        if (isExistLVName(lv,false))
                        {/*映像存储已经存在*/
                            OutPut(SYS_DEBUG,"<progress>img storage %s ready\n",lv.c_str());
                            size = disk._size;
                            state = STORAGE_ITEM_SS_READY;
                            return;
                        }
                        if (is_usesnapshot(disk,opt))
                        {
                            lv = LVM_BASE_PREFIX+to_string<int64>(disk._id,dec);
                            if (isExistLVName(lv,false))
                            {/*base存在 肯定已经下载完成*/
                                OutPut(SYS_DEBUG,"<progress>base %s ready\n",lv.c_str());
                                size = disk._size;     
                                state = STORAGE_ITEM_BASE_READY;
                                return;
                            }
                        }
                        size = 0;
                        if (is_usesnapshot(disk,opt))
                        {
                            lv = LVM_TMP_PREFIX+LVM_BASE_PREFIX+to_string<int64>(disk._id,dec);
                        }
                        else
                        {
                            make_distname(vid,disk,LVM_TMP_PREFIX+LVM_IMG_PREFIX,lv);
                        }
                        if (!isExistLVName(lv,false))
                        {/*tmp不存在*/
                            OutPut(SYS_DEBUG,"<progress>tmp %s not ready\n",lv.c_str());
                            state = STORAGE_ITEM_NOT_READY;
                            return;
                        }
                    }
                    /*tmp存在计算大小*/
                    if (STORAGE_POSITION_LOCAL == disk._position)
                    {
                        OutPut(SYS_DEBUG,"<progress>tmp %s ready\n",lv.c_str());
                    }
                    else
                    {
                        OutPut(SYS_DEBUG,"<progress>tmp %s ready\n",disk._source.c_str());
                    }
                    state = STORAGE_ITEM_TEMP_READY;
                    if (IMAGE_SOURCE_TYPE_FILE == opt._img_srctype)
                    {
                        string imgdir,imgname;
                        struct stat64 tStat;
                        if (is_usesnapshot(disk,opt))
                        {
                            make_basename(disk._id,imgdir,imgname);
                        }
                        else
                        {
                            make_imgname(vid,disk,imgdir,imgname);
                            if (STORAGE_POSITION_SHARE == disk._position)
                            {
                                imgname += ".tmp";
                            }
                        }
                        if (-1 != lstat64(imgname.c_str(),&tStat))
                        {
                            size = tStat.st_size;
                            OutPut(SYS_DEBUG,"<progress>vid:%lld,image:%lld,finish %lld of %lld\n",vid,disk._id,size,disk._size);
                        }
                    }
                    break;
                default:
                    OutPut(SYS_WARNING,"<progress>invalid state:%d\n",vminfo._state);
                    break;
            }
        }
        else
        {
            state = STORAGE_ITEM_NOT_READY;
        }
    }
    return;
}
/******************************************************************************/  
int StorageOperation::recover_image(int64 vid,VmDiskConfig &disk)
{
    //判断是否镜像存储在本地
    if (STORAGE_POSITION_LOCAL != disk._position)
    {
        OutPut(SYS_ERROR,"recover_image: storage is not local!\n");
        return ERROR;
    }

    int ret_base = check_lv_exist(disk._id, 1);
    int ret_free =  check_lv_exist(disk._id, 0);
    
    if(SUCCESS != ret_base)
    {
         if(SUCCESS != ret_free)
         {
             //base不存在free也不存在返回失败
             OutPut(SYS_DEBUG,"recover_image error: do not have baselv and freelv! \n");
             return ERROR;
         }
         else
         {
             //base不存在但free存在返回成功
             return SUCCESS;
         }
    }    

    //删除快照
    return recover_imglv(vid, disk);
}
/******************************************************************************/  
int StorageOperation::start_upload_img(int64 vid,const VmSaveReq &req,const VmDiskConfig disk,const ImageStoreOption opt)
{
    return assign_upload_work(vid,req,disk,opt);
}
/******************************************************************************/  
int StorageOperation::stop_upload_img(int64 vid,const VmDiskConfig disk)
{
    cancel_upload_work(vid,disk);
    return stop_upload(vid,disk);
}
/******************************************************************************/  
int StorageOperation::cancel_upload_img(int64 vid,const VmDiskConfig disk)
{
    return update_upload(vid,disk,UPLOAD_STATE_CANCEL);
}
/******************************************************************************/  
int StorageOperation::query_upload_img(int64 vid,const VmDiskConfig disk,int64 &finish,int &state)
{
    int ret = ERROR;
    UploadCtl ulc;

    ret = check_upload(vid,disk,ulc);
    if (SUCCESS == ret)
    {
        switch (ulc._state)
        {
            case UPLOAD_STATE_FAILED: 
            case UPLOAD_STATE_FINISH:
                dec_upload(vid, disk);
                break;
            default:
                break;
        }
        finish = ulc._finish;
        state  = ulc._state;
        OutPut(SYS_DEBUG,"<progress>img upload of %lld for vm%lld finish:%lld\n",disk._id,vid,finish);
    }

    return ret;
}
/******************************************************************************/  
StorageOperation::StorageOperation()
{
    _download_mutex.SetName("DownLoadLock");
    _download_mutex.Init();
    _upload_mutex.SetName("UploadLock");
    _upload_mutex.Init();
}
/******************************************************************************/  
int StorageOperation::init()
{
    OutPut(SYS_DEBUG,"StorageOperation Init...\n");
    _thread_pool = new ThreadPool(MAX_IMGDOWNLOAD_THREADS);
    if (!_thread_pool)
    {
        OutPut(SYS_EMERGENCY,"failed to create image download thread pool!\n");
        SkyAssert(false);
        return ERROR_NO_MEMORY;
    }
    OutPut(SYS_DEBUG,"StorageOperation create thread pool...\n");
    if (SUCCESS != _thread_pool->CreateThreads("ImageDownloader"))
    {
        OutPut(SYS_EMERGENCY,"failed to CreateThreads for image download!\n");
        SkyAssert(false);
        return ERROR;
    }

    _thread_pool_upload = new ThreadPool(MAX_IMGUPLOAD_THREADS);
    if (!_thread_pool_upload)
    {
        OutPut(SYS_EMERGENCY,"failed to create image upload thread pool!\n");
        SkyAssert(false);
        return ERROR_NO_MEMORY;
    }
    OutPut(SYS_DEBUG,"StorageOperation create thread pool for upload...\n");
    if (SUCCESS != _thread_pool_upload->CreateThreads("ImageUploader"))
    {
        OutPut(SYS_EMERGENCY,"failed to CreateThreads for image upload!\n");
        SkyAssert(false);
        return ERROR;
    }

    kill_checksum();
    kill_download();
    if (SUCCESS != clear_nfsmount())
    {
        OutPut(SYS_WARNING,"StorageOperation Init clear_nfsmount failed\n");
    }
    if (!generateDiskInfo())
    {
        return ERROR;
    }
    
    if (SUCCESS != rename_baselv())
    {
        OutPut(SYS_WARNING,"StorageOperation Init rename_baselv failed\n");
    }
    if (!recycleTmpLv())
    {
        OutPut(SYS_WARNING,"StorageOperation Init recycleTmpLv failed\n");
    }
    OutPut(SYS_DEBUG,"StorageOperation Init OK\n");
    return SUCCESS;
}
/******************************************************************************/  
StorageOperation:: ~StorageOperation()
{
    _thread_pool->DestroyPool(2);
}

int StorageOperation::CreateImgBackup(VmImageBackupReq &req)
{
    string name=LVM_BACKUP_PREFIX+to_string<int64>(req._vid,dec)+"_"+req._disk._target+"_"+req._disk._request_id;
    string backup_lv=LVM_BACKUP_PREFIX+to_string<int64>(req._vid,dec)+"_"+req._disk._target+"_"+req._request_id;
    string res_name=LVM_RESERVED_PREFIX+to_string<int64>(req._vid,dec)+"_"+req._disk._target;
    string  res,parent_lv;   
    bool   isBase=false;
    int64 size;
    if(isExistLVName (name,false))
    {        
        parent_lv=name;
    }
    else
    {
        parent_lv="i_"+to_string<int64>(req._vid,dec)+"_"+to_string<int64>(req._disk._id,dec);       
        isBase=true;        
    }
    if(isExistLVName(backup_lv,false))
    {
        return SUCCESS;
    }

    if(SUCCESS!=findLV(res_name, res))
    {
        OutPut(SYS_ERROR,"CreateBackupImg :findLV err,res_name:%s ",res_name.c_str());
        return ERROR;
    }    
   
    stringstream iss(res);
    STATUS ret;
    string lv;
    if(getline(iss,lv))
    {
        if(renameLV(lv,backup_lv))
        {
            ret = SUCCESS;
        }
        else
        {
            OutPut(SYS_ERROR,"CreateBackupImg :rename %s to %s fail\n",lv.c_str(),backup_lv.c_str());
            ret = ERROR;
        }
    }    
    else
    {        
        getsizeofLV(size,parent_lv);    
        string base;
        ret=createLV(backup_lv,  base,size , res);
        if(ret==ERR_EXIST)
            ret=SUCCESS;
    }
    if(ret!=SUCCESS)
    {
        OutPut(SYS_ERROR,"CreateBackupImg :create lv:%s, err:%d ",backup_lv.c_str(),ret);
        return ret;
    }
    fullnameofLV(backup_lv,backup_lv);
    fullnameofLV(parent_lv,parent_lv);
    make_filesystem(backup_lv);
    string backup_point=STORAGE_INSTANCES_DIR+to_string<int64>(req._vid,dec)+"/"+req._request_id;
    string parent_point=STORAGE_INSTANCES_DIR+to_string<int64>(req._vid,dec)+"/";
    if(isBase)
    {
        parent_point+=to_string<int64>(req._disk._id,dec);
    }
    else
    {
        parent_point+=req._disk._request_id;
    }
        
    sys_mkdir(backup_point);
    sys_mount(parent_lv,parent_point);
    sys_mount(backup_lv,backup_point);    
    string img=parent_point+"/"+to_string<int64>(req._disk._id,dec)+".img";
    string format;    
    string cmd = " vhd-util snapshot ";
    string destname= backup_point+"/"+to_string<int64>(req._disk._id,dec)+".img";
    cmd=cmd+"-n "+destname+"  -p  "+img;    
    ret=CheckImgFormatAndSize(img,format,size);
    if(ret!=SUCCESS)
    {
         OutPut(SYS_ERROR,"CreateBackupImg :CheckImgFormatAndSize img:%s, err:%d ",img.c_str(),ret);
         return ret;
    }
    
    if(format==IMAGE_FORMAT_RAW)
    {
        if(true == IsKvmKernel())
        {
            cmd = "qemu-img create -f qcow2 -b "+img+" "+destname;
        }
        else if(true == IsXenKernel())
        {
            cmd+=" -m ";    
        }
        else
        {
            cmd="cp "+img+" "+destname;
        }   
    }
    else if(format == IMAGE_FORMAT_QCOW2)
    {        
        cmd = "qemu-img create -f qcow2 -b "+img+" "+destname;
    }
    else if (format!=IMAGE_FORMAT_VHD)
    {
        cmd="cp "+img+" "+destname;
    }
    
    ret= RunCmd(cmd,res);
        
    return ret;
}

int StorageOperation::DeleteImgBackup(VmImageBackupReq &req)
{
    vector<string> son_request_id;
    string del_lv=LVM_BACKUP_PREFIX+to_string<int64>(req._vid,dec)+"_"+req._disk._target+"_"+req._request_id;
    string  res;   
    int64 size;
    STATUS ret;

    if(!isExistLVName(del_lv,false))
    {
        return SUCCESS;
    }

    string del_point=STORAGE_INSTANCES_DIR+to_string<int64>(req._vid,dec)+"/"+req._request_id;

    son_request_id.assign(req._son_request_id.begin(),req._son_request_id.end());
    
    if(0 == son_request_id.size())
    {
        if (SUCCESS != sys_umount(del_point))
        {
            OutPut(SYS_ERROR,"DeleteImgBackup :sys_umount %s failed!\n",del_point.c_str());
            return ERROR;
        }
        sys_rm(del_point);
        ret = deleteLV(del_lv,false);
        if(SUCCESS != ret)
        {
            OutPut(SYS_ERROR,"DeleteImgBackup :deleteLV %s error!ret: %d!\n ",del_lv.c_str(),ret);
            return ret;
        }
    }
    else
    {
        string del_img=del_point+"/"+to_string<int64>(req._disk._id,dec)+".img";
        string parent_img,format,cmd;

        //获取镜像格式
        ret=CheckImgFormatAndSize(del_img,format,size);
        if(ret!=SUCCESS)
        {//获取镜像格式失败，则直接删除LV
             OutPut(SYS_DEBUG,"DeleteImgBackup :CheckImgFormatAndSize img:%s, err:%d ",del_img.c_str(),ret);             
        }
        else
        {//正常获取到镜像格式，进行镜像操作
    
            //提交节点信息到父节点
            if(IMAGE_FORMAT_QCOW2 == format)
            {
                cmd = " qemu-img commit "+del_img;
            } 
            else
            {
                cmd = " vhd-util coalesce ";
                cmd = cmd + " -n "+ del_img;            
            }
            ret = RunCmd(cmd, res);
            if(SUCCESS != ret)
            {
                OutPut(SYS_ERROR,"DeleteImgBackup :merge lv:%s, err:%d ",del_lv.c_str(), ret);
                return ret;
            }
    
            //查询父节点信息
            if(IMAGE_FORMAT_QCOW2 == format)
            {
                cmd = " qemu-img info "+del_img+" |grep 'backing file' |cut -d ' ' -f 3";            
            } 
            else
            {
                cmd = " vhd-util query ";
                cmd = cmd + "  -n  " + del_img + "  -p  ";            
            }
    
            res ="";
            ret = RunCmd(cmd, res);
            if(SUCCESS != ret)
            {
                OutPut(SYS_ERROR,"DeleteImgBackup :query paraent lv:%s, err:%d ",del_lv.c_str(), ret);
                return ret;
            }
            else
            {
                
            }
            parent_img = res;

            string son_img,son_point;
            vector<string>::const_iterator it;
            for(it=son_request_id.begin();it!=son_request_id.end();it++)
            {
                son_point=STORAGE_INSTANCES_DIR+to_string<int64>(req._vid,dec)+"/"+*it;
                son_img=son_point+"/"+to_string<int64>(req._disk._id,dec)+".img";                
    
                //重新指定子节点的父节点
                if(IMAGE_FORMAT_QCOW2 == format)
                {
                    cmd = " qemu-img rebase "+son_img+" -b "+parent_img;
                } 
                else
                {
                    cmd = " vhd-util modify ";
                    cmd=cmd+"-n "+ son_img+ " -p "+ parent_img; 
                    if(format==IMAGE_FORMAT_RAW)
                    {
                        cmd+=" -m ";    
                    }            
                }
        
                ret= RunCmd(cmd, res);
                if(SUCCESS != ret)
                {
                    OutPut(SYS_ERROR,"DeleteImgBackup :rebase lv:%s, err:%d ",del_lv.c_str(),ret);
                    return ret;
                }
            }
        }
        if (SUCCESS != sys_umount(del_point))
        {
            OutPut(SYS_ERROR,"DeleteImgBackup: sys_umount %s failed!\n",del_point.c_str());
            return ERROR;
        }
        sys_rm(del_point);
        ret = deleteLV(del_lv,false);
        if(SUCCESS != ret)
        {
            OutPut(SYS_ERROR,"DeleteImgBackup :deleteLV %s error!ret: %d!\n ",del_lv.c_str(),ret);
            return ret;
        }    
    }

    return SUCCESS;
}

/******************************************************************************/  
int ImgDownLoadTask::recycle_lv(const string &lv)
{
    int ret = SUCCESS;
    
    if (isExistLVName(lv,false))
    {
        if (isExistSnapShot(lv,false))
        {
            OutPut(SYS_ERROR,"recycle_lv %s failed with snapshot\n",lv.c_str());
            ret = ERROR;
        }
        else
        {
            int delret = SUCCESS;
            delret = deleteLV(lv,false);
            if((SUCCESS != delret) && (ERROR_DEVICE_NOT_EXIST != delret))
            {
                OutPut(SYS_ERROR,"recycle_lv %s failed for deleteLV failed\n",lv.c_str());
                ret = ERROR;
            }
            else
            {
                OutPut(SYS_WARNING,"recycle_lv %s success\n",lv.c_str());
            }
        }
    }
    
    return ret;
}
/******************************************************************************/  
int ImgDownLoadTask::do_curl(const string &source, const string &target)
{
    int status = ERROR;
    pid_t pid = -1;

    /*1:入参合法性判断*/
    if (source.empty() || target.empty())
    {
        return status;
    }

    /*2:创建子进程*/
    if ((pid = fork()) < 0)
    {
        /*2.1:创建子进程失败直接返回*/
        OutPut(SYS_DEBUG,"do_curl fork fail errno %d!\n",errno);
        return status;
    }
    else if (0 == pid)
    {
        /*2.2:子进程中执行下载命令*/
        string cmd = "curl -s file://"+source+" -o "+target;
        execl("/bin/sh","sh","-c",cmd.c_str(),(char *)0);
    }
    else
    {
        /*2.3:父进程中更新镜像处理队列中该镜像的下载进程id*/
        bool finish = false;
        int ret = -1;
        pid_t w = -1;
        if (SUCCESS != _inst->update_download(_disk._id,_vid,pid,finish))
        {
            /*2.3.1:更新失败 有可能虚拟机已经被撤销了 这儿要不要杀掉下载进程?不能杀
               *也不能返回失败,因为有可能有其他虚拟机在等该映像的下载
               */
            OutPut(SYS_NOTICE,"do_curl update_download pid failed for %lld\n",_vid);
        }
        
        /*2.4:守候等待子进程退出*/
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
               OutPut(SYS_DEBUG,"do_curl waitpid Fail!\n");
               break;
           }
           
           if (WIFEXITED(ret)) 
           {
               status = WEXITSTATUS(ret);
           } 
           else if (WIFSIGNALED(ret) || WIFSTOPPED(ret)) 
           {
               OutPut(SYS_DEBUG,"do_curl waitpid Fail with sig or stop!\n");
               status = ERROR;
           } 
        } while (!WIFEXITED(ret) && !WIFSIGNALED(ret) && !WIFSTOPPED(ret));
        /*2.5:镜像处理队列中该镜像的下载进程id置为-1下载成功 则将finsh置为true */
        if (SUCCESS == status)
        {
            finish = true;    
        }
        if (SUCCESS != _inst->update_download(_disk._id,_vid,-1,finish))
        {
            /*2.5.1:更新失败 有可能虚拟机已经被撤销了 不能返回失败 原因同2.3.1*/
            OutPut(SYS_NOTICE,"do_curl update_download finish failed for %lld\n",_vid);;
        }
    }

    return status;
}
/******************************************************************************/  
int ImgDownLoadTask::do_nfsmount(string &mountpoint)
{
    int ret = ERROR;
    int iFileType = FILETYPE_UNKNOWN;
    NfsAccessOption access_option;

    /*1:_url反序列化*/
    if (false == access_option.deserialize(_disk._url.access_option))
    {
        return ERROR_INVALID_ARGUMENT;
    }

    /*2:先卸载挂载点目录*/
    if (check_mount(STORAGE_HC_MOUNT_DIR))
    {
        if (SUCCESS != _inst->sys_umount(STORAGE_HC_MOUNT_DIR))
        {
            return ret;
        }
    }

    /*3:存在非目录的同名文件直接删除*/
    if (SUCCESS == get_filetype(STORAGE_HC_MOUNT_DIR,iFileType))
    {
        if (FILETYPE_DIR != iFileType)
        { 
            if (SUCCESS != _inst->sys_rm(STORAGE_HC_MOUNT_DIR))
            {
                return ret;
            }
        }
    }

    /*4:挂载点目录不存在 则创建*/
    if (!_inst->file_isexist(STORAGE_HC_MOUNT_DIR))
    {
        if (SUCCESS != _inst->sys_mkdir(STORAGE_HC_MOUNT_DIR))
        {
            return ret;
        }
    }

    /*5:非快照直接使用的方式 下载时挂载点使用虚拟机号互斥,一个虚拟机一个挂载点*/
    /*快照方式映像只需要下载一次 因此多个虚拟机可以共享一个挂载点*/
    if (!_inst->is_usesnapshot(_disk,_opt))
    {
        mountpoint = STORAGE_HC_MOUNT_DIR+"/"+to_string<int64>(_vid,dec);
    }
    else
    {
        mountpoint = STORAGE_HC_MOUNT_DIR+"/"+to_string<int64>(_disk._id,dec);
    }

    /*6:先卸载虚拟机挂载点*/
    if (check_mount(mountpoint))
    {
        if (SUCCESS != UnMountNfs(mountpoint))
        {
            return ret;
        }
    }

    OutPut(SYS_DEBUG,"mounting nfs share %s:%s to %s ...\n",
          access_option.server_ip.c_str(),
          access_option.shared_dir.c_str(),
          mountpoint.c_str());

    /*7:调用MountNfs执行nfs挂载*/
    ret = MountNfs(access_option.server_ip,access_option.shared_dir,mountpoint,TO_FORCE_DIR);
    if (SUCCESS != ret)
    {
        mountpoint.clear();
    }

    return ret;
}
/******************************************************************************/  
bool ImgDownLoadTask::check_imgintegrity(const string &srctype, const string &name)
{
    if (IMAGE_SOURCE_TYPE_BLOCK == srctype)
    {//块方式
        char acBuf[1024];
        int64 size = IMG2BLOCK_SIGNATURE.size();
        if (SUCCESS != GetTailBytes(name, acBuf, size))
        {
            SkyAssert(false);
            return false;
        }
        acBuf[size] = 0;
        if (0 != IMG2BLOCK_SIGNATURE.compare(acBuf))
        {
            return false;
        }
    }
    else
    {
        if(SUCCESS != _inst->check_imgvalid(name, _disk, false))
        {
            return false;
        }
    }
    return true;
}
/******************************************************************************/  
int ImgDownLoadTask::download_use_nfs(const string &target)
{
    int filetype = FILETYPE_UNKNOWN;
    string srcimgname,nfsmountpoint;
    NfsAccessOption access_option;

    /*1:反序列化access_option*/
    if (false == access_option.deserialize(_disk._url.access_option))
    {
        return ERROR_INVALID_ARGUMENT;
    }

    /*2:从access_option中取出原镜像文件名*/
    string nfs_server_uri = access_option.server_ip + ":" + access_option.shared_dir;
    srcimgname = _disk._url.path;
    srcimgname.erase(0,nfs_server_uri.size());
    
    /*3:nfs挂载 生成挂载点nfsmountpoint*/
    if (SUCCESS != do_nfsmount(nfsmountpoint) 
        || nfsmountpoint.empty())
    {
        return ERROR;
    }

    /*4:完整原镜像名*/
    srcimgname = nfsmountpoint + srcimgname;
    
    /*5:检查nfs server上原镜像文件是否合法 包括是否存在以及该名称对应的是否是文件*/
    if ((ERROR == get_filetype(srcimgname,filetype))
          || (FILETYPE_REG != filetype))
    {
        OutPut(SYS_ALERT,"Nfs server don't have img:%s\n",srcimgname.c_str());
        return ERROR;
    }

    /*6:调用do_curl下载镜像文件*/
    if (SUCCESS != do_curl(srcimgname,target))
    {  
        OutPut(SYS_ERROR,"download_use_nfs do_curl %s to %s failed\n",srcimgname.c_str(),target.c_str());
        return ERROR;
    }
    
    /*7:umount掉nfs*/
    UnMountNfs(nfsmountpoint);  

    return SUCCESS;
}
/******************************************************************************/  
int ImgDownLoadTask::download_use_http(const string &target)
{
    int ret = ERROR;

    return ret;
}
/******************************************************************************/  
int ImgDownLoadTask::download_by_url(const string &target)
{
    int ret = ERROR;

    if (IMAGE_ACCESS_NFS == _disk._url.access_type)
    {
        ret = download_use_nfs(target);
    }
    else if (IMAGE_ACCESS_HTTP == _disk._url.access_type)
    {
        ret = download_use_http(target);
    }

    return ret;
}
/******************************************************************************/  
int ImgDownLoadTask::prepare_target(const string &device,string &target,string &imgdir)
{
    string imgname;
    /*1:准备target*/
    if (IMAGE_SOURCE_TYPE_FILE == _opt._img_srctype)
    {
        /*1.1:如果是文件方式的话 则target是一个文件*/
        if (!_inst->is_usesnapshot(_disk,_opt))
        {
            /*1.1.1:非快照方式*/
            if (SUCCESS != _inst->make_imgname(_vid,_disk,imgdir,imgname))
            {
                OutPut(SYS_ERROR,"download_file make_imgname failed,vid:%lld\n",_vid);
                return ERROR;
            }
        }
        else
        {
            /*1.1.2:快照方式*/
            if (SUCCESS != _inst->make_basename(_disk._id,imgdir,imgname))
            {
                OutPut(SYS_ERROR,"download_file make_basename failed,iid:%lld\n",_disk._id);
                return ERROR;
            }
        }
        
        /*1.2:挂载*/
        if (!check_mount(imgdir))
        {
            if (SUCCESS != _inst->sys_mount(device,imgdir))    
            {
                return ERROR;
            }
        }
        /*1.3:赋值target为文件名 绝对路径的*/
        target = imgname;
    }
    else
    {
        /*1.2:如果是块方式的话 则target是一个lv*/
        target = device; 
    }
    return SUCCESS;
}
/******************************************************************************/  
int ImgDownLoadTask::download_file(const string &target)
{
    /*1:调用download_by_url下载镜像到target*/
    if (SUCCESS != download_by_url(target))
    {
        OutPut(SYS_ERROR,"download_file download_by_url %s failed\n",target.c_str());
        return ERROR;
    }
    /*2:如果是文件方式,则检查md5*/
    if (IMAGE_SOURCE_TYPE_FILE == _opt._img_srctype)
    {   
        /*2.1:调用check_imgvalid检查md5*/
        if (SUCCESS != _inst->check_imgvalid(target, _disk))
        {
            OutPut(SYS_WARNING,"download_file img %s invalid\n",target.c_str()); 
            return ERROR;
        }
    }
    OutPut(SYS_DEBUG,"download_file %s success\n",target.c_str());
    return SUCCESS;
}
/******************************************************************************/  
int ImgDownLoadTask::create_tmplv(const string &lv,int64 size,string &res)
{
    string tmplv = LVM_TMP_PREFIX+lv;

    if (isExistLVName(tmplv,false))
    {
        string res;
        string cmd = "cat /proc/mounts 2>/dev/null | grep "+tmplv+" | awk '{print $2}'";
        if (SUCCESS != storage_command(cmd ,res))
        {
            OutPut(SYS_ALERT,"create_tmplv storage_command failed\n");
        }
        else
        {
            string mountpoint;
            stringstream iss(res);
            while (getline(iss,mountpoint))
            {
                _inst->sys_umount(mountpoint);
                OutPut(SYS_DEBUG,"sys_umount %s\n",mountpoint.c_str());
            } 
        }
        int delret = SUCCESS;
        delret = deleteLV(tmplv,false);
        if((SUCCESS != delret) && (ERROR_DEVICE_NOT_EXIST != delret))
        {/*deleteLV里面实现会执行umount操作*/
            OutPut(SYS_ALERT,"deleteLV tmp lv %s failed\n",tmplv.c_str());
            return ERROR;
        }
        OutPut(SYS_WARNING,"deleteLV tmp lv %s success\n",tmplv.c_str());
    }
    return _inst->create_lv(tmplv,size,"",INVALID_FILEID,res);
}
/******************************************************************************/  
int ImgDownLoadTask::create_reservedlv(void)
{
    if (0 == _disk._reserved_backup)
    {
        OutPut(SYS_DEBUG,"vid:%lld,imageid:%lld no need reserve lv\n",_vid,_disk._id);
        return SUCCESS;
    }

    int ret = ERROR;
    string rsv_tag=LVM_RESERVED_PREFIX+to_string<int64>(_vid,dec)+"_"+_disk._target+"_";
    string backup_tag=LVM_BACKUP_PREFIX+to_string<int64>(_vid,dec)+"_"+_disk._target+"_";
    string res,lv;
    stringstream iss;
    int rsvs = 0, backups = 0;

    ret = findLV(rsv_tag,res);
    if (SUCCESS != ret)
    {
        return ret;
    }
    iss << res;
    while (getline(iss,lv))
    {
        rsvs++;
    }
    
    iss.str("");
    ret = findLV(backup_tag,res);
    if (SUCCESS != ret)
    {
        return ret;
    }
    iss << res;
    while (getline(iss,lv))
    {
        backups++;
    }

    int diffs = _disk._reserved_backup - (rsvs+backups);
    if ( 0 <= diffs)
    {//暂不考虑改配置
        int i;
        int64 size;
        string lv;
        for (i = 0; i < diffs; i++)
        {
            lv = rsv_tag+to_string<int>(i,dec);
            _inst->calc_lvsize(_disk._disk_size, _opt._img_spcexp, size);
            OutPut(SYS_DEBUG,"vid:%lld,imageid:%lld create reserve lv:%s\n",_vid,_disk._id,lv.c_str());
            ret = _inst->create_lv(lv,size,"",INVALID_FILEID,res);
            if (SUCCESS != ret)
            {
                break;
            }
        }
    }
    return ret;
}
/******************************************************************************/  
int ImgDownLoadTask::img_bingo(int64 vid,int64 iid,const string &lv,const string &name,const ImageStoreOption &opt)
{
    VmInfoOfDLC vminfo;

    vminfo._vid = vid;
    if (IMAGE_SOURCE_TYPE_FILE == opt._img_srctype)
    {
        vminfo._disk._source = name;
        vminfo._disk._source_type = STORAGE_SOURCE_TYPE_FILE;
    }
    else
    {
        vminfo._disk._source = lv;
        vminfo._disk._source_type = STORAGE_SOURCE_TYPE_BLOCK;
    }
    vminfo._state = STORAGE_VM_STATE_READY;
    return _inst->update_download(iid,vminfo);
}
/******************************************************************************/  
int ImgDownLoadTask::create_snapshot(int64 vid,const VmDiskConfig &disk,const ImageStoreOption &opt)
{
    string sslv,ssres,imgname;

    /*1:构建快照lv名 datablock 映像文件lv格式 i_vid_iid_target;映像文件lv格式 i_vid_iid*/
    _inst->make_distname(vid,disk,LVM_IMG_PREFIX,sslv);
    
    /*2:判断快照是否存在*/
    if (!_inst->check_lvvalid(sslv,disk._disk_size,opt._img_spcexp))
    {
        /*2.1:存在 但是大小不一致 直接返回失败*/
        if (isExistLVName(sslv,false))
        {
            OutPut(SYS_ALERT,"snapshot %s exist and size mismatch\n",sslv.c_str());
            return ERROR;
        }
        
        /*2.2:计算所需lv大小*/
        int64 size = 0;
        _inst->calc_lvsize(disk._disk_size,opt._img_spcexp,size);
        
        /*2.3:构建baselv名*/
        string baselv = LVM_BASE_PREFIX+to_string<int64>(disk._id,dec);
        
        /*2.4:创建快照*/
        if (SUCCESS == _inst->create_lv(sslv,size,baselv,disk._id,ssres))
        {
            OutPut(SYS_DEBUG,"create snapshot %s successfully\n",ssres.c_str());
        }
        else
        {
            OutPut(SYS_ERROR,"create snapshot %s failed\n",ssres.c_str());
            return ERROR;
        }
    }
    else
    {/*ss存在*/
        /*2.5:快照存在 大小也一直 生成快照lv的全名*/
        fullnameofLV(sslv,ssres);
    }

    /*3:如果source类型是文件,则需挂载快照*/
    if (IMAGE_SOURCE_TYPE_FILE == opt._img_srctype)
    {
        /*3.1:挂载快照*/
        if (SUCCESS != _inst->img_source_file(vid,disk,ssres,imgname))
        {
            return ERROR;
        }
    }
    /*4:更新镜像处理队列中的 source和source type 以及state*/    
    return img_bingo(vid,disk._id,ssres,imgname,opt);
}
/******************************************************************************/  
int ImgDownLoadTask::create_allsnapshot(void)
{
    int ret = ERROR;
    int count = 0;
    
    /*1.加锁 遍历镜像处理队列*/
    _inst->lock_download();
    vector<DownLoadCtl>::iterator it;
    for (it = _inst->_download_ctl.begin(); it != _inst->_download_ctl.end(); it++)
    {
        /*1.1:找到对应的镜像*/
        if (it->_iid == _disk._id)
        {
            break;
        }
    }
    /*2:判断是否找到*/
    if (it != _inst->_download_ctl.end())
    {
        /*2.1:遍历该镜像下挂的虚拟机等待队列*/
        int ssret = ERROR;
        vector<VmInfoOfDLC>::iterator itvm;
        for (itvm = it->_vminfo.begin(); itvm != it->_vminfo.end(); itvm++)
        {
            /*如果有非本地快照方式的在队列里，不创建，直接跳过*/
            if(!((IMAGE_POSITION_LOCAL == itvm->_disk._position) && (IMAGE_USE_SNAPSHOT == itvm->_opt._img_usage)))            
            {                
                continue;
            }
            /*2.2:创建快照*/
            ssret = create_snapshot(itvm->_vid,itvm->_disk,itvm->_opt);
            if (SUCCESS != ssret)
            {
                /*2.2.1:创建失败 则将该虚拟机状态置为失败*/
                itvm->_state = STORAGE_VM_STATE_FAILED;
            }
            if (itvm->_vid == _vid)
            {
                /*2.2.2:first虚拟机则记录返回值*/
                ret = ssret;
            }
            count++;
        }
        /*2.3:vid置为-1*/
        it->_vid = INVALID_VID;
    }

    /*3:下载好镜像 重命名为base后 如果没有虚拟机使用 则应该重名为free 即可回收*/
    if (0 == count)
    {
        if (SUCCESS != _inst->rename_freelv(_disk._id))
        {
            ret = ERROR;
        }
    }
    /*4:遍历完毕 释放锁*/
    _inst->unlock_download();
    /*5:返回 如果没遍历到返回失败*/
    return ret;
}
/******************************************************************************/  
int ImgDownLoadTask::use_img_direct_local(void)
{
    bool   first = false;
    int    ret = ERROR;
    int64  lvsize = 0;
    string imglv,tmplv;
    string targetlv,imgname;

    /*1:加入到镜像处理等待队列中 便于进度查询*/
    _inst->inc_download(_disk._id,_vid,first,_disk,_opt);

    /*2:生成lv名 datablock镜像的lv名格式为i_vid_iid_target machine镜像的lv名格式为i_vid_iid*/
    _inst->make_distname(_vid,_disk,LVM_IMG_PREFIX,imglv);
    
    /*3:判断lv是否合法 包括是否存在 或者 存在大小是否一致*/
    if (!_inst->check_lvvalid(imglv,_disk._disk_size,_opt._img_spcexp))
    {
        /*3.1:存在但是大小不一致 直接返回失败*/
        if (isExistLVName(imglv,false))
        {
            goto RET;
        }
        /*3.2:计算所需lv大小*/
        _inst->calc_lvsize(_disk._disk_size,_opt._img_spcexp,lvsize);
        /*3.3:创建tmplv*/
        if (SUCCESS != create_tmplv(imglv,lvsize,tmplv))
        {
            goto RET;
        }  
        /*3.4:判断source类型,如果是文件的话需要格式化*/
        if (IMAGE_SOURCE_TYPE_FILE == _opt._img_srctype)
        {
            if (SUCCESS != make_filesystem(tmplv))
            {
                goto RET;
            }
        }
        string target,imgdir;
        if (SUCCESS != prepare_target(tmplv,target,imgdir))
        {
            goto RET;
        }
        /*3.5:下载镜像文件*/
        if (SUCCESS != download_file(target))
        {
            goto RET;
        }
        else
        {
            #if 0 //这儿可以直接用不需要umount 操作
            if (IMAGE_SOURCE_TYPE_FILE == _opt._img_srctype)
            {
                /*3.2:检查完执行umount操作*/
                _inst->sys_umount(imgdir,"-l");
            }
            #endif
            /*3.5.1:cancel掉了 ,返回失败*/
            if (_inst->iscancel_download(_disk._id,_vid))
            {
                goto RET;
            }
        }
        /*3.6:下载成功后 将tmplv重命名为baselv*/
        tmplv = LVM_TMP_PREFIX+imglv;
        if (!renameLV(tmplv,imglv))
        {
            goto RET;
        }
    }
    else
    {
        /*3.7:baselv存在 更新镜像处理队列,说明一下镜像肯定是下载好了的*/
        _inst->update_download(_disk._id,_vid,-1,true);
    }
    
    /*4:生成imglv的完整名targetlv*/
    fullnameofLV(imglv,targetlv);
    
    /*5:如果是文件类型的话 需要执行挂载操作*/
    if (IMAGE_SOURCE_TYPE_FILE == _opt._img_srctype)
    {
        /*5.1:执行挂载操作*/
        if (SUCCESS != _inst->img_source_file(_vid,_disk,targetlv,imgname))
        {
            goto RET;
        }
    }

    /*6:更新镜像处理队列中的 source和source type*/
    ret = img_bingo(_vid,_disk._id,targetlv,imgname,_opt);
RET:
    /*7:如果本次操作失败 则将镜像处理队列中该虚拟机的状态置为失败*/
    if (SUCCESS != ret)
    {
        _inst->fail_storage(_disk._id,_vid,true);
    }
    else
    {
        ret = create_reservedlv();
    }
    return ret;
}
/******************************************************************************/  
int ImgDownLoadTask::use_img_direct_share(void)
{
    bool   first = false;
    int    ret = ERROR;
    string target,imgdir,imgname;
    
    /*1:加入到镜像处理等待队列中 便于进度查询*/
    _inst->inc_download(_disk._id,_vid,first,_disk,_opt);

    if (IMAGE_SOURCE_TYPE_FILE == _opt._img_srctype)
    {//文件方式才需要检查文件系统 本地和共享的格式化流程不一样
        if (!check_filesystem(_disk._source, "ext4"))
        {
            gen_mkfsscript();
            string cmd,res;
            cmd = "bash "+STORAGE_MKFS_SHELL+" ext4 "+_disk._source;
            if (SUCCESS != storage_command(cmd, res))
            {
                goto RET;
            }
        }
    }

    if (SUCCESS != prepare_target(_disk._source,target,imgdir))
    {
        goto RET;
    }

    if (!check_imgintegrity(_opt._img_srctype,target))
    {
        if (IMAGE_SOURCE_TYPE_FILE == _opt._img_srctype)
        {
            target += ".tmp";
        }
        if (SUCCESS != download_file(target))
        {
            goto RET;
        }
        else
        {
            #if 0 //这儿可以直接用不需要umount 操作
            if (IMAGE_SOURCE_TYPE_FILE == _opt._img_srctype)
            {
                /*3.2:检查完执行umount操作*/
                _inst->sys_umount(imgdir,"-l");
            }
            #endif
            /*3.5.1:cancel掉了返回失败*/
            if (_inst->iscancel_download(_disk._id,_vid))
            {
                goto RET;
            }
        }
    }
    else
    {
        /*3.7:baselv存在 更新镜像处理队列,说明一下镜像肯定是下载好了的*/
        _inst->update_download(_disk._id,_vid,-1,true);
    }

    /*5:如果是文件类型的话 需要执行挂载操作*/
    if (IMAGE_SOURCE_TYPE_FILE == _opt._img_srctype)
    {
        imgname = target;
        /*5.1:执行挂载操作*/
        if (SUCCESS != _inst->img_source_file(_vid,_disk,_disk._source,imgname))
        {
            goto RET;
        }
    }
    ret = img_bingo(_vid,_disk._id,_disk._source,imgname,_opt);
RET:
    /*7:如果本次操作失败 则将镜像处理队列中该虚拟机的状态置为失败*/
    if (SUCCESS != ret)
    {
        _inst->fail_storage(_disk._id,_vid,true);
    }
    return ret;
}
/******************************************************************************/ 
int ImgDownLoadTask::use_img_direct(void)
{
    if (IMAGE_POSITION_LOCAL == _disk._position)
    {
        return use_img_direct_local();
    }
    else
    {
        return use_img_direct_share();
    }
}
/******************************************************************************/  
int ImgDownLoadTask::use_img_snapshot(void)
{
    bool   first = false;
    bool   baselv_exist = false;
    int    ret = ERROR;
    int64  lvsize = 0;
    Mutex  *pLvMutex = NULL;
    string reslv,baselv,freelv;

    /*1:加入到镜像处理等待队列中 便于进度查询*/
    _inst->inc_download(_disk._id,_vid,first,_disk,_opt);
    /*2:加锁
     *判断baselv存在
     *判断freelv存在
     *freelv重命名为baselv
     *需为一个原子操作
     */
    if(SUCCESS != getLvMutex(&pLvMutex))
    {
        goto RET;
    }
    /*3:加lv锁*/
    pLvMutex->Lock();
    /*4:判断base是否存在*/
    baselv = LVM_BASE_PREFIX+to_string<int64>(_disk._id,dec);
    if (!_inst->check_lvvalid(baselv,_disk._disk_size,_opt._img_spcexp))
    {
        /*4.1:如果base lv存在 则大小不等 直接返回失败*/
        if (isExistLVName(baselv,false))
        {
            pLvMutex->Unlock();
            goto RET;
        }

        /*4.2:判断freelv是否存在*/
        freelv = LVM_FREE_PREFIX+to_string<int64>(_disk._id,dec);
        if (!_inst->check_lvvalid(freelv,_disk._disk_size,_opt._img_spcexp))
        {
            /*4.2.1:freelv 如果存在且不合法 直接回收掉,因为如果这儿不回收,其他地方也可能被回收*/
            if (SUCCESS != recycle_lv(freelv))
            {
                pLvMutex->Unlock();
                goto RET;
            }
        }
        else
        {
            /*4.2.2:freelv存在 freelv重命名为baselv*/
            if (!renameLV(freelv,baselv))
            {
                pLvMutex->Unlock();
                goto RET;
            }
            /*4.2.3:标识baselv是存在的*/
            baselv_exist = true;
        }
    }
    else
    {
        /*4.3:baselv 存在且合法 标识baselv是存在的*/
        baselv_exist = true;
    }
    /*5:释放lv锁*/
    pLvMutex->Unlock();
    
    /*6:判断上述流程中查询的baselv是否存在
     *此时因为释放了lv锁 那么base就有可能因为没有快照而被重命名为free
     *这种情况下面的创建快照过程就失败了 继而虚拟机创建失败 
     *而虚拟机创建失败后 cc会重新发送虚拟机创建 所以这里不会因为锁的问题导致虚拟机不能创建
     */
    /* 已经有vm负责创建快照，此处直接跳过*/
    if (!first)
    {
        ret = SUCCESS;
        goto RET;
    }

    if (!baselv_exist)
    {
    #if 0
        /*6.1:已经有虚拟机在下载镜像,该虚拟机就不占用镜像处理线程了，事都交由first那个vm来处理*/
        if (!first)
        {
            ret = SUCCESS;
            goto RET;
        }
    #endif
        /*6.2:需要下载镜像,计算所需lv大小*/
        _inst->calc_lvsize(_disk._disk_size,_opt._img_spcexp,lvsize);
        /*6.3:创建tmp lv*/
        if (SUCCESS != create_tmplv(baselv,lvsize,reslv))
        {
            goto RET;
        }  
        /*6.4:判断source类型,如果是文件的话需要格式化*/
        if (IMAGE_SOURCE_TYPE_FILE == _opt._img_srctype)
        {
            /*6.4.1:格式化tmplv*/
            if (SUCCESS != make_filesystem(reslv))
            {
                goto RET;
            }
        }
        /*6.5:准备target*/
        string target,imgdir;
        if (SUCCESS != prepare_target(reslv,target,imgdir))
        {
            goto RET;
        }
        /*6.6:下载镜像文件*/
        if (SUCCESS != download_file(target))
        {
            goto RET;
        }
        else
        {
            if (IMAGE_SOURCE_TYPE_FILE == _opt._img_srctype)
            {/*6.6.1:如果是文件 下载完后需要执行umount操作*/
                _inst->sys_umount(imgdir,"-l");
            }
        }
        /*6.7:下载成功后 将tmplv重命名为baselv*/
        reslv = LVM_TMP_PREFIX+baselv;
        if (!renameLV(reslv,baselv))
        {
            goto RET;
        }
    }
    else
    {
        /*6.7:baselv存在 更新镜像处理队列,说明一下镜像肯定是下载好了的*/
        _inst->update_download(_disk._id,_vid,-1,true);
    }
    /*7:创建快照，替镜像处理队列中所有使用该镜像并需要创建快照的虚拟机创建好快照*/
    ret = create_allsnapshot();
RET:
    /*8:如果本次操作失败 则将镜像处理队列中该虚拟机的状态置为失败*/
    if (SUCCESS != ret)
    {
        if(true == first)
        {//为first的节点，要负责设置其他节点的状态
            _inst->fail_storage(_disk._id,_vid,false);
        }
        else
        {
            _inst->fail_storage(_disk._id,_vid,true);
        }
    }
    else
    {
        ret = create_reservedlv();
    }
    return ret;
}
/******************************************************************************/  
int ImgDownLoadTask::img_to_lv(void)
{
    if (_inst->is_usesnapshot(_disk, _opt))
    {
        return use_img_snapshot();
    }
    else
    {
        return use_img_direct();
    }
}
/******************************************************************************/  
void ImgUpLoadTask::set_nport(struct sockaddr *sa, unsigned port)
{
    if (sa->sa_family == AF_INET6) 
    {
        struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *) sa;
        sin6->sin6_port = port;
        return;
    }
    if (sa->sa_family == AF_INET) 
    {
        struct sockaddr_in *sin = (struct sockaddr_in *) sa;
        sin->sin_port = port;
        return;
    }
}
/******************************************************************************/  
len_and_sockaddr* ImgUpLoadTask::str2sockaddr(
        const char *host, int port,int ai_flags)
{
    sa_family_t af = AF_INET;
    int rc;
    len_and_sockaddr *r;
    struct addrinfo *result = NULL;
    struct addrinfo *used_res;
    const char *org_host = host; /* only for error msg */
    struct addrinfo hint;

    r = NULL;

    /* Next two if blocks allow to skip getaddrinfo()
     * in case host name is a numeric IP(v6) address.
     * getaddrinfo() initializes DNS resolution machinery,
     * scans network config and such - tens of syscalls.
     */
    /* If we were not asked specifically for IPv6,
     * check whether this is a numeric IPv4 */

    memset(&hint, 0 , sizeof(hint));
    hint.ai_family = af;
    /* Need SOCK_STREAM, or else we get each address thrice (or more)
     * for each possible socket type (tcp,udp,raw...): */
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_flags = ai_flags & ~DIE_ON_ERROR;
    rc = getaddrinfo(host, NULL, &hint, &result);
    if (rc || !result) 
    {
        OutPut(SYS_ERROR,"bad address '%s'", org_host);
        //if (ai_flags & DIE_ON_ERROR)
            //xfunc_die();
        goto ret;
    }
    used_res = result;
    while (1) {
        if (used_res->ai_family == AF_INET)
        {
            break;
        }
        used_res = used_res->ai_next;
        if (!used_res) 
        {
            used_res = result;
            break;
        }
    }

    r = (len_and_sockaddr *)malloc(LSA_LEN_SIZE + used_res->ai_addrlen);
    r->len = used_res->ai_addrlen;
    memcpy(&r->u.sa, used_res->ai_addr, used_res->ai_addrlen);

    set_nport(&r->u.sa, htons(port));
 ret:
    if (result)
        freeaddrinfo(result);
    return r;
}
/******************************************************************************/  
len_and_sockaddr* ImgUpLoadTask::xhost2sockaddr(const char *host, int port)
{
    return str2sockaddr(host, port, AI_CANONNAME);
}
/******************************************************************************/  
int ImgUpLoadTask::xsocket(int domain, int type, int protocol)
{
    int r = socket(domain, type, protocol);

    if (r < 0) 
    {
        OutPut(SYS_ERROR,"xsocket failed for %s\n",strerror(errno));
    }

    return r;
}
/******************************************************************************/  
int ImgUpLoadTask::xconnect(int s, const struct sockaddr *s_addr, socklen_t addrlen)
{
    int ret = connect(s, s_addr, addrlen);
    
    if (ret < 0) 
    {
        OutPut(SYS_ERROR,"xconnect failed for %s\n",strerror(errno));
    }
    return ret;
}
/******************************************************************************/  
int ImgUpLoadTask::xconnect_stream(const len_and_sockaddr *lsa)
{
    int fd = xsocket(lsa->u.sa.sa_family, SOCK_STREAM, 0);
    if (fd < 0)
    {
        OutPut(SYS_ERROR,"xsocket failed for %s\n",strerror(errno));
        return fd;
    }
    if (xconnect(fd, &lsa->u.sa, lsa->len) < 0)
    {
        OutPut(SYS_ERROR,"xconnect failed for %s\n",strerror(errno));
        close(fd);
        fd = -1;
    }
    return fd;
}
/******************************************************************************/  
int ImgUpLoadTask::ftpcmd(const char *s1, const char *s2 ,FtpClintPara &para)
{
    int n = -1;
    int count = 0;

    if (1 == para.verbose_flag) 
    {
        OutPut(SYS_DEBUG,"cmd %s %s", s1, s2);
    }

    if (s1) 
    {
        fprintf(para.control_stream, (s2 ? "%s %s\r\n" : "%s %s\r\n"+3), s1, s2);
        fflush(para.control_stream);
    }
    count = BUFSIZ - 2;
    do {
        strcpy(para.buf, "EOF"); /* for ftp_die */
        if (NULL == fgets(para.buf, count, para.control_stream)) 
        {
            OutPut(SYS_ERROR,"fgets failed for %s", strerror(errno));
            return -1;
        }
    } while (!isdigit(para.buf[0]) || para.buf[3] != ' ');

    para.buf[3] = '\0';
    n = atoi(para.buf);
    para.buf[3] = ' ';
    return n;
}
/******************************************************************************/  
int ImgUpLoadTask::ftp_login(FtpClintPara &para)
{
    /* Connect to the command socket */
    if (para.control_stream == NULL) 
    {
        OutPut(SYS_ERROR,"ftp_login failed control_stream NULL\n");
        return ERROR;
    }

    if (ftpcmd(NULL, NULL, para) != 220) 
    {
        //ftp_die(NULL);
        OutPut(SYS_ERROR,"ftp_die NULL\n");
        return ERROR;
    }

    /*  Login to the server */
    switch (ftpcmd("USER", para.user, para)) 
    {
    case 230:
        break;
    case 331:
        if (ftpcmd("PASS", para.password, para) != 230) 
        {
            OutPut(SYS_ERROR,"ftp_die PASS\n");
            return ERROR;
        }
        break;
    default:
    //ftp_die("USER");
        OutPut(SYS_ERROR,"ftp_die USER\n");
        return ERROR;
    }

    if (ftpcmd("TYPE I", NULL, para) != 200)
    {
        OutPut(SYS_ERROR,"ftp_die TYPE I\n");
        return ERROR;
    }

    return SUCCESS;
}
/******************************************************************************/  
ssize_t ImgUpLoadTask::safe_write(int fd, const void *buf, size_t count)
{
    ssize_t n;

    do {
        n = write(fd, buf, count);
    } while (n < 0 && errno == EINTR);

    return n;
}
/******************************************************************************/  
ssize_t ImgUpLoadTask::full_write(int fd, const void *buf, size_t len)
{
    ssize_t cc;
    ssize_t total;

    total = 0;

    while (len) {
        cc = safe_write(fd, buf, len);

        if (cc < 0) 
        {
            if (total) 
            {
                /* we already wrote some! */
                /* user can do another write to know the error code */
                return total;
            }
            return cc;  /* write() returns -1 on failure. */
        }

        total += cc;
        buf = ((const char *)buf) + cc;
        len -= cc;
    }

    return total;
}
/******************************************************************************/  
ssize_t ImgUpLoadTask::safe_read(int fd, void *buf, size_t count)
{
    ssize_t n;

    do {
        n = read(fd, buf, count);
    } while (n < 0 && errno == EINTR);

    return n;
}
/******************************************************************************/  
int64 ImgUpLoadTask::bb_full_fd_action(int src_fd, int dst_fd, off_t size, FtpClintPara &para)
{
    int status = -1;
    int64 total = 0;
    bool continue_on_write_error = 0;
    char buffer[BUFSIZ];

    if (size < 0) 
    {
        size = -size;
        continue_on_write_error = 1;
    }

    if (src_fd < 0)
    {
        goto out;
    }

    if (!size) 
    {
        size = BUFSIZ;
        status = 1; /* copy until eof */
    }

    while (1) {
        if(_inst->iscancel_upload(_vid,_disk))
        {
            ftpcmd("ABOR", NULL, para);
            break;
        }
        ssize_t rd;
        rd = safe_read(src_fd, buffer, size > BUFSIZ ? BUFSIZ : size);

        if (!rd) 
        { /* eof - all done */
            status = 0;
            break;
        }
        if (rd < 0) 
        {
            OutPut(SYS_ERROR,"safe_read failed for %s\n",strerror(errno));
            break;
        }
        /* dst_fd == -1 is a fake, else... */
        if (dst_fd >= 0) 
        {
            ssize_t wr = full_write(dst_fd, buffer, rd);
            if (wr < rd) 
            {
                if (!continue_on_write_error) 
                {
                    OutPut(SYS_ERROR,"full_write failed for %s\n",strerror(errno));;
                    break;
                }
                dst_fd = -1;
            }
        }
        total += rd;
        _inst->update_upload(_vid, _disk, total);
        if (status < 0) 
        { /* if we aren't copying till EOF... */
            size -= rd;
            if (!size) 
            {
                /* 'size' bytes copied - all done */
                status = 0;
                break;
            }
        }
    }
 out:

    return status ? -1 : total;
}
/******************************************************************************/  
int64 ImgUpLoadTask::bb_copyfd_eof(int fd1, int fd2, FtpClintPara &para)
{
    return bb_full_fd_action(fd1, fd2, 0, para);
}
/******************************************************************************/  
int ImgUpLoadTask::pump_data_and_QUIT(int from, int to, FtpClintPara &para)
{
    int ret = SUCCESS;
    /* copy the file */
    if (bb_copyfd_eof(from, to, para) == -1) 
    {
        /* error msg is already printed by bb_copyfd_eof */
        OutPut(SYS_ERROR,"bb_copyfd_eof failed\n");
        ret = ERROR;
    }
    /* close data connection */
    close(from); /* don't know which one is that, so we close both */
    close(to);

    /* does server confirm that transfer is finished? */
    if (ftpcmd(NULL, NULL, para) != 226) 
    {
        //ftp_die(NULL);
        OutPut(SYS_WARNING,"pump_data_and_QUIT NULL\n");
    }
    
    if (ftpcmd("QUIT", NULL, para) != 221)
    {
        OutPut(SYS_WARNING,"pump_data_and_QUIT QUIT\n");
    }

    return ret;
}
/******************************************************************************/  
int ImgUpLoadTask::xconnect_ftpdata(FtpClintPara &para)
{
    char *buf_ptr;
    unsigned port_num;

/*
TODO: PASV command will not work for IPv6. RFC2428 describes
IPv6-capable "extended PASV" - EPSV.

"EPSV [protocol]" asks server to bind to and listen on a data port
in specified protocol. Protocol is 1 for IPv4, 2 for IPv6.
If not specified, defaults to "same as used for control connection".
If server understood you, it should answer "229 <some text>(|||port|)"
where "|" are literal pipe chars and "port" is ASCII decimal port#.

There is also an IPv6-capable replacement for PORT (EPRT),
but we don't need that.

NB: PASV may still work for some servers even over IPv6.
For example, vsftp happily answers
"227 Entering Passive Mode (0,0,0,0,n,n)" and proceeds as usual.

TODO2: need to stop ignoring IP address in PASV response.
*/

    if (ftpcmd("PASV", NULL, para) != 227) 
    {
        //ftp_die("PASV");
        OutPut(SYS_ERROR,"ftp_die PASV\n");
        return -1;
    }

    /* Response is "NNN garbageN1,N2,N3,N4,P1,P2[)garbage]
     * Server's IP is N1.N2.N3.N4 (we ignore it)
     * Server's port for data connection is P1*256+P2 */
    buf_ptr = strrchr(para.buf, ')');
    if (buf_ptr) *buf_ptr = '\0';

    buf_ptr = strrchr(para.buf, ',');
    *buf_ptr = '\0';
    port_num = atoi(buf_ptr + 1);

    buf_ptr = strrchr(para.buf, ',');
    *buf_ptr = '\0';
    port_num += atoi(buf_ptr + 1) * 256;

    set_nport(&para.lsa->u.sa, htons(port_num));
    return xconnect_stream(para.lsa);
}
/******************************************************************************/  
int ImgUpLoadTask::ftp_send(const char *server_path, const char *local_path, FtpClintPara &para)
{
    int fd_data;
    int fd_local;
    int response;

    /* connect to the data socket */
    fd_data = xconnect_ftpdata(para);
    if (fd_data < 0) 
    {
        OutPut(SYS_ERROR,"xconnect_ftpdata failed for %s\n",strerror(errno));
        return ERROR;
    }
    /* get the local file */
    fd_local = STDIN_FILENO;
    fd_local = open(local_path, O_RDONLY, 0666);
    if (fd_local < 0) 
    {
        close(fd_data);
        OutPut(SYS_ERROR,"can't open %s for %s\n",local_path,strerror(errno));
        return ERROR;
    }
    response = ftpcmd("STOR", server_path, para);
    switch (response) {
    case 125:
    case 150:
        break;
    default:
        //ftp_die("STOR");
        OutPut(SYS_ERROR,"ftp_die STOR\n");
        /* close data connection */
        close(fd_local); /* don't know which one is that, so we close both */
        close(fd_data);
        return ERROR;
    }

    return pump_data_and_QUIT(fd_local, fd_data, para);
}
/******************************************************************************/  
int ImgUpLoadTask::ftp_client(const char *host, 
                               const char *server_path, 
                               const char *local_path, 
                               const char *user, 
                               const char *password, 
                               int port)
{
    int ret = ERROR;
    FtpClintPara para;

    para.lsa = xhost2sockaddr(host, port);
    if (NULL == para.lsa)
    {
        OutPut(SYS_ERROR,"xhost2sockaddr failed for %s\n",strerror(errno));
        goto ERRRET;
    }
    para.control_stream = fdopen(xconnect_stream(para.lsa), "r+");
    if (NULL == para.control_stream)
    {
        OutPut(SYS_ERROR,"fdopen failed for %s\n",strerror(errno));
        goto ERRRET;
    }
    para.user = user;
    para.password = password;
    para.verbose_flag = 1;
    para.do_continue  = 0;
    
    if (SUCCESS != ftp_login(para))
    {
        goto ERRRET;
    }

    ret = ftp_send(server_path, local_path, para);

ERRRET:
    if (NULL != para.control_stream)
    {
        fclose(para.control_stream);
    }

    if (NULL != para.lsa)
    {
        free(para.lsa);
    }
    return ret;
}
/******************************************************************************/  
int ImgUpLoadTask::upload_use_nfs(const string &target)
{
    return ERROR;
}
/******************************************************************************/  
int ImgUpLoadTask::upload_use_http(const string &target)
{
    return ERROR;
}
/******************************************************************************/  
int ImgUpLoadTask::upload_use_ftp(const string &target)
{
    return ftp_client(_req._url.ip.c_str(),
               _req._name.c_str(),
               target.c_str(),
               _req._url.user.c_str(),
               _req._url.passwd.c_str(),
               _req._url.port);
}
/******************************************************************************/  
int ImgUpLoadTask::upload_by_url(const string &target)
{
    int ret = ERROR;

    if (IMAGE_ACCESS_NFS == _req._url.access_type)
    {
        ret = upload_use_nfs(target);
    }
    else if (IMAGE_ACCESS_HTTP == _req._url.access_type)
    {
        ret = upload_use_http(target);
    }
    else if (IMAGE_ACCESS_FTP == _req._url.access_type)
    {
        ret = upload_use_ftp(target);
    }

    return ret;
}
/******************************************************************************/  
int ImgUpLoadTask::upload_file(void)
{
    string target,imglv,progressfile;
    _inst->make_imglvname(_vid,_disk,true,imglv);
    if (IMAGE_SOURCE_TYPE_FILE == _opt._img_srctype)
    {
        string imgdir,img;
        _inst->make_imgname(_vid, _disk, imgdir, img);

        if (!check_mount(imgdir))
        {
            _inst->make_imglvname(_vid,_disk,true,imglv);
            if (SUCCESS != _inst->sys_mount(imglv, imgdir))
            {
                return ERROR;
            }
        }
        if (!_inst->file_isexist(img.c_str()))
        {
            return ERROR;
        }
        
        if (_inst->is_imageformatraw(img))
        {
            struct stat64 tStat;
            if (!((-1 != lstat64(img.c_str(),&tStat)) && (_disk._size == tStat.st_size)))
            {
                return ERROR;
            }
        }
        target = img;
    }
    else
    {
        OutPut(SYS_ERROR,"upload_file not support for image source type not file\n");
        return ERROR;
    }
    return upload_by_url(target);
}
/******************************************************************************/  
int ImgUpLoadTask::img_from_lv(void)
{
    int ret = SUCCESS;
    int state = UPLOAD_STATE_UNKNOWN;
    
    if (SUCCESS == _inst->inc_upload(_vid, _req, _disk, _opt))
    {
        _inst->update_upload(_vid, _disk, UPLOAD_STATE_UPLOAD);
        ret = upload_file();
    }

    if (SUCCESS != ret)
    {
        state = UPLOAD_STATE_FAILED;
    }
    else
    {
        state = UPLOAD_STATE_FINISH;
    }
    _inst->update_upload(_vid, _disk, state);
    return ret;
}
/******************************************************************************/  
void DbgPrintDownLoad(void)
{
    StorageOperation *pInst=StorageOperation::GetInstance();
    if (!pInst)
    {
        printf("StorageOperation::GetInstance failed\n");
        return;
    }
    pInst->print_download();
}
DEFINE_DEBUG_FUNC(DbgPrintDownLoad);
/******************************************************************************/  
void DbgPringUpLoad(void)
{
    StorageOperation *pInst=StorageOperation::GetInstance();
    if (!pInst)
    {
        printf("StorageOperation::GetInstance failed\n");
        return;
    }
    pInst->print_upload();
}
DEFINE_DEBUG_FUNC(DbgPringUpLoad);
/******************************************************************************/  
void DbgDeleteLv(const char* vg_name, const char* lv_name)
{
    if (NULL == vg_name || NULL == lv_name)
    {
        cout << "Invalid Parameter" <<endl;
        return;
    }
    string vg = vg_name;
    string lv = lv_name;
    deleteLV(vg,lv);
}
DEFINE_DEBUG_FUNC(DbgDeleteLv);

/******************************************************************************/  
void DbgDeleteLvFullName(const char* lv_name)
{
    if (NULL == lv_name)
    {
        cout << "Invalid Parameter" <<endl;
        return;
    }
    string lv = lv_name;
    deleteLV(lv);
}
DEFINE_DEBUG_FUNC(DbgDeleteLvFullName);


