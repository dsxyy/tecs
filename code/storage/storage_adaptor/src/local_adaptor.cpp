#include "sky.h"
#include "mid.h"
#include "event.h"
#include "storage_adaptor_impl.h"
#include "errcode.h"
#include "log_agent.h"
#include "tecs_config.h"

using namespace zte_tecs;

STATUS LocalAdaptor::Init(const sa_cfg &cfg)
{
    _cfg=cfg;
    
    list<TLvmVgInfo>     VgList;
    getVGListInfo(VgList);
 
    for (list<TLvmVgInfo>::iterator itDev=VgList.begin();itDev!=VgList.end();++itDev)
    {
        string::size_type loc = itDev->sVGName.find( "tecs_volume_");

         if( loc != string::npos )
         {            
            doVgchange(itDev->sVGName);
         }
    }
    return SUCCESS;
}

STATUS LocalAdaptor::RunFind(const string &cmd,const string &sub)
{
    string result;
    //RunCmd(cmd, result);    
    storage_command(cmd, result);
    if(result.empty())
    {
        return SUCCESS;
    }
    if(sub.empty())
    {
        return ERROR;
    }
    string::size_type find_pose = result.find(sub);
    if (find_pose == string::npos)
    {
        OutPut(SYS_DEBUG, "RunFind result is %s\n",result.c_str());
        return ERROR_SA_OPERATE_ERR;
    }
    else
    {
        return SUCCESS;
    }
}

STATUS LocalAdaptor::AllocVol(const StAllocVolReq & req ,string &vol_uuid)
{

    string sResSourc;
    string sLVName;
    sLVName=req._volname;
    int result;
    int ret=ERROR;
    string devname;
    int64  size=MBToBytes(req._volsize);
    result=createLV(req._vgname, sLVName, "",size,sResSourc);
    if (ERR_EXIST == result)
    {
        devname ="/dev/"+req._vgname+"/"+sLVName;//返回LV
        /*逻辑卷LV 名称已经被占用*/
        int64 lvSize = 0;
        
        getsizeofLV(lvSize,req._vgname,sLVName);
        if (lvSize != size)
        {            
            OutPut(SYS_CRITICAL,"LocalAdaptor: EV_SA_CREATE_VOL vol exist and size dismatch %ld--%ld\n",lvSize,size);
            ret=ERROR_PERMISSION_DENIED;
        }
        else
        {
            OutPut(SYS_DEBUG,"storage_svr blocklv %s exist and size match\n",sLVName.c_str());
            ret=SUCCESS;
        }
    }
    else
    {
        ret=result;
    }
    if (ret==SUCCESS)
    {
        ret=getLVuuid(req._vgname,sLVName,vol_uuid);
    }
    OutPut(SYS_NOTICE,"LocalAdaptor::AllocVol name %s size is %ld MB vol_uuid is %s ret is %d \n",sLVName.c_str(),req._volsize,vol_uuid.c_str(),ret);
    return ret;
}

STATUS LocalAdaptor::DeleteVol(const StDelVolReq & req )
{
    int  ret;
    ret=deleteLV(req._vgname,req._volname);
    if(ret==ERROR_DEVICE_NOT_EXIST)
        ret=SUCCESS;
    OutPut(SYS_NOTICE,"LocalAdaptor::DeleteVol name %s   ret is %d \n",req._volname.c_str(),ret);
    return ret;
}

STATUS LocalAdaptor::AllocSnapshot(const StAllocSnapshotReq & req,string &vol_uuid)
{
    int ret = ERROR;
    string res;
    int64  size=MBToBytes(req._volsize);
    ret = createLV(req._vgname, req._volname, req._base_name, size,res);
    if (ERR_EXIST == ret)
    {
        int64 size = 0;
        getsizeofLV(size, req._vgname, req._volname);
        if (size != req._volsize)
        {
            OutPut(SYS_WARNING,"AllocSnapshot createLV %s failed for size mismatch(should:%lld,but:%lld)\n",req._volname.c_str(),req._volsize,size);
        }
    }
    else if (SUCCESS != ret)
    {
        return ret;
    }
    
    ret=getLVuuid(req._vgname,req._volname,vol_uuid);    
    OutPut(SYS_NOTICE,"LocalAdaptor::AllocSnapshot name %s size is %ld MB vol_uuid is %s ret is %d \n",req._volname.c_str(),req._volsize,vol_uuid.c_str(),ret);
    return ret;
}

STATUS LocalAdaptor::DeleteSnapshot(const StDelVolReq & req )
{
    int  ret;
    ret=deleteLV(req._vgname,req._volname);
    if(ret==ERROR_DEVICE_NOT_EXIST)
        ret=SUCCESS;
    OutPut(SYS_NOTICE,"LocalAdaptor::DeleteSnapshot name %s   ret is %d \n",req._volname.c_str(),ret);
    return ret;
}

STATUS LocalAdaptor::CreateTarget(const StCreateTargetReq & req)
{
    int    ret=-1;
    string sLVName;
    sLVName=req._targetname;
    string   cmd;    
    int64    tid;
    tid=Index2Id(req._targetindex);
    cmd="tgtadm --lld iscsi  --mode target  --op new  --tid ";
    cmd+=to_string<int64>(tid,dec);
    cmd+=" -T ";
    cmd+=req._targetname;
    cmd+="  2>&1";
    
    ret=RunFind(cmd,"exists"); 
    if(ret==SUCCESS)
    {
        cmd="tgtadm  --lld iscsi --mode target --op bind --tid ";
        cmd+=to_string<int64>(tid,dec);
        cmd+=" -Q tecs_iscsiname_unkown ";
        cmd+="  2>&1";
        ret=RunFind(cmd,"exists");
        
        cmd="tgtadm  --lld iscsi --mode target --op bind --tid ";
        cmd+=to_string<int64>(tid,dec);
        cmd+=" -I ALL ";        
        cmd+="  2>&1";
        ret=RunFind(cmd,"exists");
        
    }
    
    OutPut(SYS_NOTICE,"LocalAdaptor::CreateTarget target name is %s ret is %d \n",sLVName.c_str(),ret);
    return ret;
}

STATUS LocalAdaptor::DelTarget(const StDelTargetReq & req )
{
    string cmd;
    string result;
    int    ret;
    int64  tid;
    tid=Index2Id(req._targetindex);
    cmd="tgtadm --lld iscsi  --mode target  --op delete --force  --tid ";
    cmd+=to_string<int64>(tid,dec);
    cmd+="  2>&1";
    ret=RunFind(cmd,"can't");
    OutPut(SYS_NOTICE,"LocalAdaptor::DelTargetname target name is %s  ret is %d\n",req._targetname.c_str(),ret);
    return ret;
}

STATUS LocalAdaptor::AddVol2Target(const StAddToTarReq& req)
{
    string  devname;
    string  cmd;    
    int     ret;
    int     lunid;
    int64   tid;
    tid=Index2Id(req._targetindex);
    devname="/dev/"+req._vgname+"/"+req._volname;
    struct stat dev_state;
    if(0 != stat(devname.c_str(),&dev_state))
    {        
        OutPut(SYS_ERROR,"LocalAdaptor::AddVol2Target target name is  %s  dev: %s  not exist\n",req._targetname.c_str(),devname.c_str());  
        return ERROR;
    }   
    cmd="tgtadm --lld iscsi --op new --mode logicalunit --tid ";
    cmd+=to_string<int64>(tid,dec);
    cmd+="  --lun ";
    lunid =Index2Id(req._lunid);// req._lunid 从 0 开始
    cmd+=to_string<int64>(lunid,dec);
    cmd+="  -b ";
    cmd+=devname;
    cmd+="  2>&1";
    ret=RunFind(cmd,"exists");
    OutPut(SYS_NOTICE,"LocalAdaptor::AddVol2Target target name is  %s  lun name is %s ret is %d\n",req._targetname.c_str(),req._volname.c_str(),ret);
    return ret;
}

STATUS LocalAdaptor::AddHost2Target(const StAddToTarReq& req)
{
    string  cmd;    
    int     ret;
    int64   tid;
    tid=Index2Id(req._targetindex);
    cmd="tgtadm  --lld iscsi --mode target --op bind --tid ";
    cmd+=to_string<int64>(tid,dec);
    cmd+=" -Q  ";
    cmd+=req._iscsiname;
    cmd+="  2>&1";
    ret=RunFind(cmd,"exists");
    OutPut(SYS_NOTICE,"LocalAdaptor::AddHost2Target target index : %ld, iscsiname :%s, ret : %d\n",req._targetindex,req._iscsiname.c_str(),ret);
    return ret;
}

STATUS LocalAdaptor::DelVolFromTarget(const StDelFromTarReq & req)
{
    string  cmd;
    string  result;
    int     ret;
    int64   tid;
    tid=Index2Id(req._targetindex);
    int    lunid;
    lunid =Index2Id(req._lunid);// req._lunid 从 0 开始
    cmd="tgtadm --lld iscsi --op delete --mode logicalunit --tid ";
    cmd+=to_string<int64>(tid,dec);
    cmd+=" --lun ";
    cmd+=to_string<int64>(lunid,dec);
    cmd+="  2>&1";
    ret=RunFind(cmd,"can't");
    OutPut(SYS_NOTICE,"LocalAdaptor::DelVolFromtarget target index is %ld  ret is %d \n",req._targetindex,ret);
    return ret;
}

STATUS LocalAdaptor::DelHostFromTarget(const StDelFromTarReq & req)
{
    string  cmd;
    int     ret;
    int64   tid;    
    tid=Index2Id(req._targetindex);
    cmd="tgtadm  --lld iscsi --mode target --op unbind --tid ";
    cmd+=to_string<int64>(tid,dec);
    cmd+=" -Q  ";
    cmd+=req._iscsiname;
    cmd+="  2>&1";
    ret=RunFind(cmd,"not");    
    OutPut(SYS_NOTICE,"LocalAdaptor::DelHostFromtarget target index : %ld ,iscsiname :%s, ret : %d\n",req._targetindex,req._iscsiname.c_str(),ret);
    return ret;
}

STATUS LocalAdaptor::GetStorageInfo(capacity & info)
{
    list<TLvmVgInfo>     VgList;
    getVGListInfo(VgList);

    //lvm分区信息lvmDiskDevList
    for (list<TLvmVgInfo>::iterator itDev=VgList.begin();itDev!=VgList.end();++itDev)
    {
        info.iTotalCapacity  += itDev->uiTotalPENum * itDev->uiPESize;        
        if (info.iTotalCapacity>info.iAllocMaxCapacity)
        {
            info.iAllocMaxCapacity=info.iTotalCapacity;
        }
    }

    //OutPut(SYS_NOTICE,"rcv EV_STORAGE_GETINFO total %lld alloc %lld free  %lld   \n",info.iTotalCapacity,info.iAllocMaxCapacity,info.iFreeCapacity);
    return  SUCCESS;
}

STATUS LocalAdaptor::GetDataIp(vector<string>  &dataip)
{
    dataip=_cfg._data_ip;
    return SUCCESS;
}

STATUS LocalAdaptor::GetVgInfo(vector<Vginfo> &vginfo)
{
    list<TLvmVgInfo>     VgList;
    getVGListInfo(VgList);

    //lvm分区信息lvmDiskDevList
    Vginfo info;
    for (list<TLvmVgInfo>::iterator itDev=VgList.begin();itDev!=VgList.end();++itDev)
    {
        string::size_type loc = itDev->sVGName.find( "tecs_volume_");

         if( loc != string::npos )
         {
            info._vgname=itDev->sVGName;
            info._total_size=itDev->uiTotalPENum * itDev->uiPESize;
            info._max_size  =itDev->uiTotalPENum * itDev->uiPESize;
            vginfo.push_back(info);
         }
    }
    return SUCCESS;
}

STATUS LocalAdaptor::ReBuildTarget(sa_target_info &target)
{
    StCreateTargetReq  create;
    StAddToTarReq      addvol;
    StAddToTarReq      addhost;
    map<string,int64>  target_indexname;
    map<string,int64>::iterator itmap;
    vector<StorageAuthorizeWithTargetId>::iterator ithost;
    int  ret;

    for (ithost=target._authorize_with_target_id.begin();ithost!=target._authorize_with_target_id.end();ithost++)
    {
        itmap=target_indexname.find(ithost->_target_name);
        if (itmap==target_indexname.end())
        {
            target_indexname.insert(make_pair(ithost->_target_name,ithost->_target_id));
        }
    }

    if (target_indexname.empty())
    {
        return SUCCESS;
    }

    for (itmap=target_indexname.begin();itmap!=target_indexname.end();itmap++)
    {
        create._targetname=itmap->first;
        create._targetindex=itmap->second;
        ret=CreateTarget(create);
        if(ret!=SUCCESS)
        {
            OutPut(SYS_ERROR,"LocalAdaptor: ReBuildTarget CreateTarget index %d  name %s err\n",create._targetindex,create._targetname.c_str());
            return ret;
        }
    }

    vector<StorageVolumeWithLun>::iterator itlun;
    for (itlun=target._volume_with_lun.begin();itlun!=target._volume_with_lun.end();itlun++)
    {
        addvol._lunid=itlun->_lun_id;
        addvol._vgname=itlun->_vg_name;
        addvol._volname=itlun->_volume_name;
        itmap=target_indexname.find(itlun->_target_name);
        if (itmap==target_indexname.end())
        {
            OutPut(SYS_ERROR,"LocalAdaptor: ReBuildTarget cand find targetindex for name%s\n",itlun->_target_name.c_str());
            return ERROR;
        }
        addvol._targetindex=itmap->second;
        ret=AddVol2Target(addvol);
        if(ret!=SUCCESS)
        {
            OutPut(SYS_ERROR,"LocalAdaptor: ReBuildTarget AddVol2Target  err\n");
            return ret;
        }
    }
    for (ithost=target._authorize_with_target_id.begin();ithost!=target._authorize_with_target_id.end();ithost++)
    {
        addhost._ip=ithost->_ip_address;
        addhost._iscsiname=ithost->_iscsi_initiator;
        addhost._targetindex=ithost->_target_id;
        ret=AddHost2Target(addhost);
        if(ret!=SUCCESS)
        {
            OutPut(SYS_ERROR,"LocalAdaptor: ReBuildTarget AddHost2Target  err\n");
            return ret;
        }
    }
    return SUCCESS;
}

