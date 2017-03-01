#include "volume.h"
#include "lvm.h"
#include "log_agent.h"
#include "mid.h"
#include "event.h"
#include "sky.h"
#include "registered_system.h"


namespace zte_tecs
{
#if 0
STATUS BlockAdaptorControlPlane::CreateBlocks(const MID & mid, const string& request_id, const string& clustername,int64 volsize)
{
    AllocBlocksReq req;
    STATUS  iRet;
    req._volsize=volsize;
    req._clustname=clustername;    
    req._request_id = request_id;
    if (mid._application.empty())
        return ERROR_NOT_READY;    
    MessageOption option(mid, EV_CLIENT_CREATE_BLOCK, 0, 0);
    iRet=_mu->Send(req, option);
    if (iRet!=SUCCESS)
    {
        OutPut(SYS_ERROR,"CreateBlocks send msg err,ret is %d\n",iRet);
   
    }
    return iRet;    
}

STATUS BlockAdaptorControlPlane::DeleteBlocks(const MID & mid,const string  &request_id)
{
    DelBlocksReq req;
    STATUS  iRet;
    req._request_id = request_id;    
    if (mid._application.empty())
        return ERROR_NOT_READY;   
    MessageOption option(mid, EV_CLIENT_DEL_BLOCK, 0, 0);   
    iRet=_mu->Send(req, option);
    if (iRet!=SUCCESS)
    {
        OutPut(SYS_ERROR,"DeleteBlocks send msg err,ret is %d\n",iRet);        
    }
    return iRet;    
}

STATUS BlockAdaptorControlPlane::CreateSnapshot(const MID & mid, const string &request_id, const string& base_id, int64 volsize)
{
    AllocSnapshotReq req;
    STATUS  iRet;
    req._volsize = volsize;
    req._base_id = base_id;
    req._request_id = request_id;
    if (mid._application.empty())
        return ERROR_NOT_READY;    
    MessageOption option(mid, EV_CLIENT_CREATE_SNAPSHOT, 0, 0);
      
    iRet=_mu->Send(req, option);
    if (iRet!=SUCCESS)
    {
        OutPut(SYS_ERROR,"CreateSnapshot send msg err,ret is %d\n",iRet);        
    }
    return iRet;  
    
}

STATUS BlockAdaptorControlPlane::DeleteSnapshot(const MID & mid, const string  &request_id)
{
    DelBlocksReq req;
    STATUS  iRet;
    req._request_id = request_id;
    if (mid._application.empty())
        return ERROR_NOT_READY;    
    MessageOption option(mid, EV_CLIENT_DELETE_SNAPSHOT, 0, 0);    
    iRet=_mu->Send(req, option);
    if (iRet!=SUCCESS)
    {
        OutPut(SYS_ERROR,"DeleteSnapshot send msg err,ret is %d\n",iRet);        
    }
    return iRet;    
}

STATUS BlockAdaptorControlPlane::AuthBlocks(const MID& mid, const auth_info& auth)
{
    AuthBlocksReq req;
    STATUS  iRet;
    req._request_id = auth.request_id;
    req._iscsiname = auth.iscsiname;
    req._userip = auth.userip;
    req._vid= auth.vid;
    req._dist_id = auth.dist_id;
    req._clustername = auth.cluster_name;    
    if (mid._application.empty())
        return ERROR_NOT_READY;  
    MessageOption option(mid, EV_CLIENT_AUTH_BLOCK, 0, 0);
    iRet=_mu->Send(req, option);
    if (iRet!=SUCCESS)
    {
        OutPut(SYS_ERROR,"AuthBlocks send msg err,ret is %d\n",iRet);      
    }
    return iRet;   
}


STATUS BlockAdaptorControlPlane::DeAuthBlocks(const MID & mid, const auth_info& auth)
{
    AuthBlocksReq req;
    STATUS  iRet;
    req._request_id = auth.request_id;
    req._iscsiname = auth.iscsiname;
    req._userip = auth.userip;
    req._vid= auth.vid;
    req._dist_id = auth.dist_id;
    req._clustername = auth.cluster_name;    
    if (mid._application.empty())
        return ERROR_NOT_READY;   
    MessageOption option(mid, EV_CLIENT_DEAUTH_BLOCK, 0, 0);  
    iRet=_mu->Send(req, option);
    if (iRet!=SUCCESS)
    {
        OutPut(SYS_ERROR,"DeAuthBlocks send msg err,ret is %d\n",iRet);   
    }
    return iRet;
}


STATUS BlockAdaptorControlPlane::AddAuthHost(const MID& mid, const AuthBlocksReq& req)
{
    STATUS  iRet;
    if (mid._application.empty())
        return ERROR_NOT_READY;  
    MessageOption option(mid, EV_CLIENT_ADD_AUTH_HOST, 0, 0);
    iRet=_mu->Send(req, option);
    if (iRet!=SUCCESS)
    {
        OutPut(SYS_ERROR,"AddAuthHost send msg err,ret is %d\n",iRet);      
    }
    return iRet;   
}

STATUS BlockAdaptorControlPlane::DelAuthHost(const MID& mid, const AuthBlocksReq& req)
{
    STATUS  iRet;
    if (mid._application.empty())
        return ERROR_NOT_READY;  
    MessageOption option(mid, EV_CLIENT_DEL_AUTH_HOST, 0, 0);
    iRet=_mu->Send(req, option);
    if (iRet!=SUCCESS)
    {
        OutPut(SYS_ERROR,"DelAuthHost send msg err,ret is %d\n",iRet);      
    }
    return iRet;   
}

STATUS BlockAdaptorControlPlane::DealScAck(const MessageFrame &message,Sc_AckInfo &sc_info)
{
    VolumeAck ack;
    ack.deserialize(message.data);
    sc_info.last_op=ack.last_op;
    sc_info.request_id=ack.request_id;
    sc_info.ret=ack.state;

    OutPut(SYS_DEBUG,"DealScAck lastop:%d request_id:%s uuid:%s type:%s,target:%s,lunid:%d,ret is %d\n",
            ack.last_op,
            ack.request_id.c_str(),
            ack.vol_uuid.c_str(),
            ack.type.c_str(),
            ack.target_name.c_str(),
            ack.lunid,
            sc_info.ret);
    vector<string>::iterator it;
    switch(ack.last_op)
    {
    case SC_CREATE_VOL:           
        sc_info.option._vol_uuid=ack.vol_uuid;         
        break;    

    case SC_AUTH_BLOCK:
        sc_info.option._type=ack.type;        
        for (it=ack.dataip_vec.begin();it!=ack.dataip_vec.end();it++)
        {
            sc_info.option._dataip_array.push_back(*it);
        }        
        sc_info.option._lunid=ack.lunid;
        sc_info.option._target_name=ack.target_name;
        sc_info.option._vol_uuid = ack.vol_uuid;       
        break;        

    case SC_GET_VOLINFO:
        sc_info.option._lunid=ack.lunid;
        for (it=ack.dataip_vec.begin();it!=ack.dataip_vec.end();it++)
        {
            sc_info.option._dataip_array.push_back(*it);
        }
        break;
    
    default:       
        break;    
    }   
    return SUCCESS;
}
#endif

STATUS VOS_Stat(const char *pcFileName, T_FileStat *pStat)
{
    struct stat64 tStat;
    STATUS  tState=FILE_OPE_SUCCESS;
    if (-1 == lstat64(pcFileName,&tStat))
    {
        tState= FILE_STAT_ERR;
    }
    else if (NULL != pStat)
    {
        pStat->st_dev   = tStat.st_dev;
        pStat->st_ino   = tStat.st_ino;
        pStat->st_mode  = tStat.st_mode;
        pStat->st_uid   = tStat.st_uid;
        pStat->st_gid   = tStat.st_gid;
        pStat->st_nlink = tStat.st_nlink;
        pStat->st_rdev  = tStat.st_rdev;
        pStat->st_size  = tStat.st_size;
        pStat->dwfs_atime = tStat.st_atime;
        pStat->dwfs_ctime = tStat.st_ctime;
        pStat->dwfs_mtime = tStat.st_mtime;
    }

    return tState;
}

STATUS VOS_Delete(const char *pcFileName)
{
    T_FileStat tFileStat;
    STATUS tState = FILE_OPE_SUCCESS;

    tState = VOS_Stat(pcFileName, &tFileStat);
    if (FILE_OPE_SUCCESS != tState)
    {
        return FILE_NOT_EXIST;
    }
    else
    {
        if (VOS_ISDIR(tFileStat.st_mode))
        {
            return FILE_ERR_NOT_FILE;
        }
    }
    if (-1== unlink(pcFileName))
    {
        OutPut(SYS_ERROR,"VOS_Delete err,errno is %d\n",errno);
        tState=FILE_SYS_ERR;
    }
    return tState;
}

STATUS Vos_SymLinkFile(const char *pcNewName,const char *pcOldName)
{
    STATUS  tState = FILE_OPE_SUCCESS;
    T_FileStat  tFileStat;

    tState = VOS_Stat(pcOldName, &tFileStat);
    if (FILE_OPE_SUCCESS != tState)
    {
        OutPut(SYS_ERROR,"Vos_SymLinkFile pcOldName %s not exist \n",pcOldName);
        tState = FILE_NOT_EXIST;
    }
    else
    {
        tState = VOS_Stat(pcNewName, &tFileStat);
        if (FILE_OPE_SUCCESS == tState)
        {
            VOS_Delete(pcNewName);
        }

        tState =symlink(pcOldName,pcNewName);
        if (tState!=0)
        {
            tState=FILE_SYS_ERR;
            OutPut(SYS_ERROR,"symlink err is %d\n",errno);
        }
    }

    return tState;
}


BlockAdaptorDataPlane *BlockAdaptorDataPlane::_instance = NULL;

STATUS BlockAdaptorDataPlane::LoadBlocks(const string &type, vector<string>& iplist,int64 vmid, string &targetname,vector<T_LOAD_BLOCK_INFO> & loadinfo,int &iLoadNum)
{
    string strDeviceName;
    STATUS ret=SUCCESS;
    bool   bRet=false;
    string volname;
    int iRes;
    string sNewName,oldname;
    string multipath_name;
    int  count=0;
    /*iscsi 装载*/
    struct stat64 tStat;
    if (-1 == lstat64(STORAGE_DIR,&tStat))
    {
        string mkcmd,result;
        mkcmd = "mkdir -p ";
        mkcmd+=STORAGE_DIR;
        if (SUCCESS!=RunCmd(mkcmd,result))
        {
            OutPut(SYS_ERROR,"LoadBlocks  mkdir STORAGE_DIR failed\n");
            return ERROR;
        }
    }
    vector<string >::iterator it_ip;
    string valid_ip,acording_target;
    bool   blogin=false,allload=true;
    vector<int>  vec_lunid;
    vector<T_LOAD_BLOCK_INFO>::iterator it;
    
    for (it = loadinfo.begin(); ((loadinfo.size()!= 0)&&(it!= loadinfo.end())); ++it)
    {
        vec_lunid.push_back(it->lunid);
    }
   
    for (it_ip = iplist.begin();it_ip!= iplist.end(); ++it_ip)
    {
        iRes = iscsiadmScsiServer(type,SCSI_LOGIN,*it_ip,targetname,vec_lunid,_login_mutex);
        if (ISCSIADM_SUCESS!=iRes)
        { 
            ret=iRes;
            allload=false;
            OutPut(SYS_DEBUG,"iscsiadmScsiServer SCSI_LOGIN ip %s  return:%d\n",(*it_ip).c_str(),iRes);
        }
        else
        {
            blogin=true;
            valid_ip=*it_ip;
            acording_target=targetname;
        }        
    }   
    if (!blogin)
    {
        return ret;
    }
    OutPut(SYS_DEBUG,"iscsiadmScsiServer login SUCCESS ,targetname is %s \n",acording_target.c_str());
    /*重新命名设备名*/
   
    for (it = loadinfo.begin(); ((loadinfo.size()!= 0)&&(it!= loadinfo.end())); ++it)
    {
        count=0;
        strDeviceName.clear();
        do
        {
            Delay(1000);          
            iRes=iscsiadmGetLunDeviceName(valid_ip,acording_target,it->lunid,strDeviceName);
            count++;
           
        }while (iRes!=SUCCESS && count<60);
        if (SUCCESS!=iRes)
        {
            OutPut(SYS_ERROR,"iscsiadmGetLunDeviceName return: err ret is %d ,retry:%d \n",iRes,count);
            return iRes;
        }
        OutPut(SYS_DEBUG,"iscsiadmGetLunDeviceName SUCCESS ,dev : %s,retry:%d  \n",strDeviceName.c_str(),count);
        count=0;
        do
        {           
            Delay(1000);  
            bRet=getmultipath_name(strDeviceName,multipath_name);
            count++;

        }while (bRet==false && count<120);
        OutPut(SYS_DEBUG,"getmultipath_name ret:%d ,dev : %s,retry:%d  \n",bRet,multipath_name.c_str(),count);
        if(bRet==false)
        {
            return ERROR;
        }
        
        iLoadNum++;
        //sNewName="/dev/"+"/"+"iscsi_"+to_string<int64>(vmid,dec)+it->sTarget;
        sNewName=STORAGE_DIR;       
        sNewName.append("iscsi_");
        if (INVALID_VID != vmid)
        {
            sNewName.append(to_string<int64>(vmid,dec));
            sNewName.append("_");
        }
        sNewName.append(it->sTarget);
        if (multipath_name.empty())
        {
            oldname="/dev/";
            oldname.append(strDeviceName);
        }
        else
        {
            oldname="/dev/mapper/";
            oldname.append(multipath_name);
        }
        OutPut(SYS_DEBUG,"LoadBlocks newname:%s,oldname:%s!\n",sNewName.c_str(),oldname.c_str());
        /*软链接流程*/
        ret=Vos_SymLinkFile(sNewName.c_str(),oldname.c_str());
        {
            /*如果连接文件已经存在，则认为*/
            if (FILE_ERR_DEST_EXIST==ret)
                ret=FILE_OPE_SUCCESS;

            if (ret!=FILE_OPE_SUCCESS)
            {
                OutPut(SYS_ERROR,"LoadBlocks Vos_SymLinkFile err ret is %d \n",ret);
                break;
            }
        }
        it->sDeviceName=sNewName;

    }
    if(ret==SUCCESS)
    {
        if(!allload)
        {
            ret=ERROR_LOAD_NEED_REFRESH;
        }
    }
    OutPut(SYS_NOTICE,"LoadBlocks VMID:: %ld  targetname is %s ret is %d \n",vmid,acording_target.c_str(),ret);
    return ret;
}

STATUS BlockAdaptorDataPlane::RefreshLoadBlocks(const string &type, vector<string>& iplist, string &targetname,vector<T_LOAD_BLOCK_INFO> & loadinfo)
{
    string strDeviceName;
    STATUS ret=SUCCESS;
    string volname;
    int iRes;
    string sNewName,oldname;
    string multipath_name;   
    /*iscsi 装载*/
    struct stat64 tStat;
    if (-1 == lstat64(STORAGE_DIR,&tStat))
    {
        string mkcmd,result;
        mkcmd = "mkdir -p ";
        mkcmd+=STORAGE_DIR;
        if (SUCCESS!=RunCmd(mkcmd,result))
        {
            OutPut(SYS_WARNING,"LoadBlocks  mkdir STORAGE_DIR failed\n");
            return ERROR;
        }
    }
    vector<string >::iterator it_ip;
    vector<int>  vec_lunid;
    vector<T_LOAD_BLOCK_INFO>::iterator it;
    
    for (it = loadinfo.begin(); ((loadinfo.size()!= 0)&&(it!= loadinfo.end())); ++it)
    {
        vec_lunid.push_back(it->lunid);
    }
   
    
    for (it_ip = iplist.begin();it_ip!= iplist.end(); ++it_ip)
    {
        iRes = iscsiadmScsiServer(type,SCSI_LOGIN,*it_ip,targetname,vec_lunid,_login_mutex);
        if (ISCSIADM_SUCESS!=iRes)
        {
            ret=iRes;            
        }  
        
    }   
    return ret;
}

STATUS BlockAdaptorDataPlane::UnloadBlocks(const string &type ,vector<string>& iplist,int64 vmid,string &targetname, vector<T_UNLOAD_BLOCK_INFO> & unloadinfo)
{
    string strDeviceName;
    string volname,symname;
    STATUS  ret;
    int  count=0;
    
    /*重新命名设备名*/
    vector<T_UNLOAD_BLOCK_INFO>::iterator it;
    for (it = unloadinfo.begin(); ((unloadinfo.size()!= 0)&&(it!= unloadinfo.end())); ++it)
    {
        //symname="/dev/"+STORAGE_VGNAME+"iscsi_"+to_string<int64>(vmid,dec)+it->sTarget;
        symname=STORAGE_DIR;    
        symname.append("iscsi_");
        if (INVALID_VID != vmid)
        {
            symname.append(to_string<int64>(vmid,dec));
            symname.append("_");
        }
        symname.append(it->sTarget);
        string cmd,result;
        cmd = "blockdev --flushbufs  ";
        cmd+=symname;
        RunCmd(cmd,result);        
        /*删除软链接设备*/
        VOS_Delete(symname.c_str());

    }   
    
    vector<string> ::iterator it_ip;
    vector<T_UNLOAD_BLOCK_INFO> ::iterator it_info;
    vector<int>vec_lunid;
    for (it_ip=iplist.begin();it_ip!=iplist.end();it_ip++)
    {
        
        for (it_info=unloadinfo.begin();it_info!=unloadinfo.end();it_info++)
        {
            count=0;
            do
            {                
                ret=iscsiadmDelDevice(type,targetname,*it_ip,it_info->lunid);
                Delay(1000); 
                count++;

            }while (ret!=SUCCESS && count<10);   
            
            OutPut(SYS_NOTICE,"iscsiadmDelDevice VMID: %ld  targetname is %s ,ret :%d,retry :%d\n",vmid,targetname.c_str(),ret,count);
            if(ret!=SUCCESS)
            {               
                return ret;
            }
            
        } 
        bool empty;
        GetTargetStatus(type,targetname,*it_ip,empty);
        if(empty)
        {
            iscsiadmScsiServer(type,SCSI_LOGOUT,*it_ip,targetname,vec_lunid,_login_mutex);
        }
        
    }    
    
    OutPut(SYS_NOTICE,"UnloadBlocks VMID: %ld  targetname is %s successfully",vmid,targetname.c_str());
    return SUCCESS;
}


    
STATUS BlockAdaptorDataPlane::Init()
{
    if(!_init)
    {
        struct stat file_state;
        if(stat(TARGETID_DIR,&file_state)!=0)
        {
            string mkcmd,result;
            mkcmd = "mkdir -p ";
            mkcmd+=TARGETID_DIR;
            if (SUCCESS!=RunCmd(mkcmd,result))
            {
                OutPut(SYS_ERROR," mkdir targetid dir failed\n" );
                return ERROR;
            }
        }      
        _login_mutex.Init();
        _init=true;
    }
    return SUCCESS;
}

void BlockAdaptorDataPlane::ReclaimDev()
{
    guard_iscsirec();
}

 
}

