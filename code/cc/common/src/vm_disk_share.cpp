/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：vm_disk_share.cpp
* 文件标识：
* 内容摘要：VmDiskShare类的定义文件
* 当前版本：1.0
* 作    者：颜伟
* 完成日期：2013年1月5日
*
* 修改记录1：
*     修改日期：2013/1/5
*     版 本 号：V1.0
*     修 改 人：颜伟
*     修改内容：创建
*******************************************************************************/
#include "storage.h"
#include "vm_disk_share.h"
#include "image_cache.h"
#include "log_agent.h"
#include "event.h"

namespace zte_tecs {

/******************************************************************************/  
const char * VmDiskSharePool::_table = 
        "vm_disk_share";

const char * VmDiskSharePool::_col_names = 
        "request_id,"
        "hid,"
        "vid,"
        "target,"
        "volume_uuid,"
        "disk_url,"
        "cache_id,"        
        "state,"
        "detail,"
        "last_op";

VmDiskSharePool *VmDiskSharePool::_instance = NULL;

/******************************************************************************/
int VmDiskSharePool::Allocate(const VmDiskShare &vds)
{
    int rc = -1;
    SqlCommand sql(_db, _table, SqlCommand::WITH_TRANSACTION);
    
    sql.Add("request_id", vds._request_id);
    sql.Add("hid", vds._hid);
    sql.Add("vid", vds._vid);
    sql.Add("target", vds._target);
    sql.Add("volume_uuid", vds._volume_uuid);
    sql.Add("disk_url", vds._disk_url);
    sql.Add("cache_id", vds._cache_id);
    sql.Add("state", vds._state);
    sql.Add("detail", vds._detail);
    sql.Add("last_op", vds._last_op);
    
    rc = sql.Insert();
    if (0 != rc)
    {
        //数据库系统出错
        SkyAssert(false);
        return ERROR_DB_INSERT_FAIL;
    }

    return SUCCESS;
}

/******************************************************************************/
int VmDiskSharePool::Drop(const VmDiskShare  &vds)
{
    int rc = -1;
    SqlCommand sql(_db, _table, SqlCommand::WITH_TRANSACTION);

    rc = sql.Delete(" WHERE request_id = '" + vds._request_id + "'");
    if ((0 != rc) && (SQLDB_RESULT_EMPTY != rc))
    {
        //数据库系统出错
        SkyAssert(false);
        return ERROR_DB_DELETE_FAIL;
    }
    
    return SUCCESS;
}


/******************************************************************************/
int VmDiskSharePool::UpdateVolumeUuidAndState(const VmDiskShare &vds)
{
    int rc = -1;
    SqlCommand sql(_db, _table, SqlCommand::WITH_TRANSACTION);
    
    sql.Add("volume_uuid",  vds._volume_uuid);
    sql.Add("state",  vds._state);
    sql.Add("detail", vds._detail);
    sql.Add("last_op",  vds._last_op);
    
    rc = sql.Update(" WHERE request_id = '" + vds._request_id +"'");
    if (0 != rc)
    {
        //数据库系统出错
        SkyAssert(false);
        return ERROR_DB_UPDATE_FAIL;
    }
    
    return SUCCESS;
}
/******************************************************************************/
int VmDiskSharePool::UpdateUrlAndState(const VmDiskShare &vds)
{
    int rc = -1;
    SqlCommand sql(_db, _table, SqlCommand::WITH_TRANSACTION);
    
    sql.Add("disk_url", vds._disk_url);
    sql.Add("state",  vds._state);
    sql.Add("detail", vds._detail);
    sql.Add("last_op",  vds._last_op);
    
    rc = sql.Update(" WHERE request_id = '" + vds._request_id +"'");
    if (0 != rc)
    {
        //数据库系统出错
        SkyAssert(false);
        return ERROR_DB_UPDATE_FAIL;
    }
    
    return SUCCESS;
}
/******************************************************************************/
int VmDiskSharePool::UpdateState(const VmDiskShare &vds)
{
    int rc = -1;
    SqlCommand sql(_db, _table, SqlCommand::WITH_TRANSACTION);
    
    sql.Add("state",  vds._state);
    sql.Add("detail", vds._detail);
    sql.Add("last_op",  vds._last_op);
    
    rc = sql.Update(" WHERE request_id = '" + vds._request_id +"'");
    if (0 != rc)
    {
        //数据库系统出错
        SkyAssert(false);
        return ERROR_DB_UPDATE_FAIL;
    }
    
    return SUCCESS;
}
/******************************************************************************/
int VmDiskSharePool::Select(vector<VmDiskShare> &vec_vds,const string &where)
{
    ostringstream   oss;
    int             rc = -1;

    SetCallback(static_cast<Callbackable::Callback>(&VmDiskSharePool::SelectCallback),
                static_cast<void *> (&vec_vds));

    oss << "SELECT " << _col_names << 
           " FROM " << _table << 
           " WHERE  " << where;

 
    rc = _db->Execute(oss, this);

    UnsetCallback();
    if ( (SQLDB_OK != rc) && (SQLDB_RESULT_EMPTY != rc)  )
    {
        //数据库系统出错
        SkyAssert(false);
        return ERROR_DB_SELECT_FAIL;
    } 
  
    return SUCCESS;
}

/******************************************************************************/
int VmDiskSharePool::SelectCallback(void * nil, SqlColumn * columns)
{
    if (NULL == columns ||          
        LIMIT != columns->get_column_num())    
    {        
        return -1;   
    }   

    vector<VmDiskShare> *p_vector;

    p_vector = static_cast<vector<VmDiskShare> *>(nil);

    VmDiskShare vds;
    columns->GetValue(REQUEST_ID, vds._request_id);
    columns->GetValue(HID, vds._hid);
    columns->GetValue(VID, vds._vid);
    columns->GetValue(TARGET, vds._target);
    columns->GetValue(VOLUME_UUID, vds._volume_uuid);
    columns->GetValue(DISK_URL, vds._disk_url);
    columns->GetValue(CACHE_ID, vds._cache_id);
    columns->GetValue(STATE, vds._state);
    columns->GetValue(DETAIL, vds._detail);
    columns->GetValue(LAST_OP, vds._last_op);
    
    p_vector->push_back(vds);

    return 0;
}
/******************************************************************************/
int VmDiskSharePool::SelectHostIpAndIscsiByHid(int64 hid, string &ip, string &iscsi)
{
    string where = " oid = " + to_string<int64>(hid,dec);
    if (0 != SelectColumn("host_pool", "media_address", where, ip))
    {
        return ERROR;        
    }
    if (ip.empty())
    {
        if (0 != SelectColumn("host_pool", "ip_address", where, ip))
        {
            return ERROR;
        }
    }    
    if (0 != SelectColumn("host_pool", "iscsiname", where, iscsi))
    {
        return ERROR;
    }
    return SUCCESS;
}
/******************************************************************************/
int GetHostIpAndIscsiByHid(int64 hid, string &ip, string &iscsi)
{
    int ret = ERROR;
    VmDiskSharePool *pInst = VmDiskSharePool::GetInstance();
    if (NULL == pInst)
    {
        OutPut(SYS_ERROR,"GetHostIpAndIscsiByHid GetInstance Failed\n");
        return ret;
    }

    ret = pInst->SelectHostIpAndIscsiByHid(hid, ip, iscsi);
    if (SUCCESS != ret)
    {
        OutPut(SYS_ERROR,"GetHostIpAndIscsiByHid Select Failed\n");
    }

    return ret;
}
/******************************************************************************/
int GetSnapshotByCacheId(int64 id, vector<VmDiskShare> &vec_vds)
{
    int ret = ERROR;
    VmDiskSharePool *pInst = VmDiskSharePool::GetInstance();
    if (NULL == pInst)
    {
        OutPut(SYS_ERROR,"GetSnapshotByCacheId GetInstance Failed\n");
        return ret;
    }

    string where = " cache_id = " + to_string<int64>(id,dec);

    ret = pInst->Select(vec_vds, where);
    if (SUCCESS != ret)
    {
        OutPut(SYS_ERROR,"GetSnapshotByCacheId Select Failed\n");
    }

    return ret;
}
/******************************************************************************/
int GetShareDiskByVmInfo(int64 hid, int64 vid, const string &target, VmDiskShare &vds)
{
    int ret = ERROR;
    VmDiskSharePool *share_pool = VmDiskSharePool::GetInstance();
    if (NULL == share_pool)
    {

        OutPut(SYS_ERROR,"VmDiskSharePool GetInstance Failed");
        return ret;
    }

    string where = " hid = " + to_string<int64>(hid,dec)
                  +" AND vid = " + to_string<int64>(vid,dec)
                  +" AND target = '" + target + "'";
    vector<VmDiskShare> vec_vds;
    
    ret = share_pool->Select(vec_vds, where);
    if (SUCCESS != ret)
    {
        OutPut(SYS_ERROR,"VmDiskSharePool Select Failed");
        return ret;
    }
    if (0 == vec_vds.size())
    {
        ret = ERROR_OBJECT_NOT_EXIST;
    }
    else
    {
        vds = vec_vds[0];
    }
    return ret;
}
/******************************************************************************/
int GetShareDiskByVidAndTarget(int64 vid, const string &target, VmDiskShare &vds)
{
    int ret = ERROR;
    VmDiskSharePool *share_pool = VmDiskSharePool::GetInstance();
    if (NULL == share_pool)
    {

        OutPut(SYS_ERROR,"VmDiskSharePool GetInstance Failed");
        return ret;
    }

    string where = " vid = " + to_string<int64>(vid,dec)
                  +" AND target = '" + target + "'";
    vector<VmDiskShare> vec_vds;
    
    ret = share_pool->Select(vec_vds, where);
    if (SUCCESS != ret)
    {
        OutPut(SYS_ERROR,"VmDiskSharePool Select Failed");
        return ret;
    }
    if (0 == vec_vds.size())
    {
        ret = ERROR_OBJECT_NOT_EXIST;
    }
    else
    {
        vds = vec_vds[0];
    }
    return ret;
}

/******************************************************************************/
int GetShareDiskByRequestId(const string &request_id,VmDiskShare &vds)
{
    int ret = ERROR;
    VmDiskSharePool *share_pool = VmDiskSharePool::GetInstance();
    if (NULL == share_pool)
    {

        OutPut(SYS_ERROR,"VmDiskSharePool GetInstance Failed");
        return ret;
    }

    string where = " request_id = '" + request_id + "'";
    vector<VmDiskShare> vec_vds;
    
    ret = share_pool->Select(vec_vds, where);
    if (SUCCESS != ret)
    {
        OutPut(SYS_ERROR,"VmDiskSharePool Select Failed");
        return ret;
    }
    if (0 == vec_vds.size())
    {
        ret = ERROR_OBJECT_NOT_EXIST;
    }
    else
    {
        vds = vec_vds[0];
    }
    return ret;
}
/******************************************************************************/
int AllocateShareDisk(const VmDiskShare &vds)
{
    int ret = ERROR;
    VmDiskSharePool *share_pool = VmDiskSharePool::GetInstance();
    if (NULL == share_pool)
    {

        OutPut(SYS_ERROR,"VmDiskSharePool GetInstance Failed");
        return ret;
    }
    ret = share_pool->Allocate(vds);
    if (SUCCESS != ret)
    {
        OutPut(SYS_ERROR,"VmDiskSharePool Allocate Failed");
    }

    return ret;
}
/******************************************************************************/
int UpdateShareDiskVolumeUuid(const VmDiskShare &vds)
{
    int ret = ERROR;
    VmDiskSharePool *share_pool = VmDiskSharePool::GetInstance();
    if (NULL == share_pool)
    {

        OutPut(SYS_ERROR,"VmDiskSharePool GetInstance Failed");
        return ret;
    }
    ret = share_pool->UpdateVolumeUuidAndState(vds);
    if (SUCCESS != ret)
    {
        OutPut(SYS_ERROR,"VmDiskSharePool Update Failed");
    }

    return ret;
}
/******************************************************************************/
int UpdateShareDiskUrl(const VmDiskShare &vds)
{
    int ret = ERROR;
    VmDiskSharePool *share_pool = VmDiskSharePool::GetInstance();
    if (NULL == share_pool)
    {

        OutPut(SYS_ERROR,"VmDiskSharePool GetInstance Failed");
        return ret;
    }
    ret = share_pool->UpdateUrlAndState(vds);
    if (SUCCESS != ret)
    {
        OutPut(SYS_ERROR,"VmDiskSharePool Update Failed");
    }

    return ret;
}
/******************************************************************************/
int UpdateShareDiskState(const VmDiskShare &vds)
{
    int ret = ERROR;
    VmDiskSharePool *share_pool = VmDiskSharePool::GetInstance();
    if (NULL == share_pool)
    {

        OutPut(SYS_ERROR,"VmDiskSharePool GetInstance Failed");
        return ret;
    }
    ret = share_pool->UpdateState(vds);
    if (SUCCESS != ret)
    {
        OutPut(SYS_ERROR,"VmDiskSharePool Update Failed");
    }

    return ret;
}
/******************************************************************************/
int DeleteShareDiskFromDb(const VmDiskShare &vds)
{
    int ret = ERROR;
    VmDiskSharePool *share_pool = VmDiskSharePool::GetInstance();
    if (NULL == share_pool)
    {

        OutPut(SYS_ERROR,"VmDiskSharePool GetInstance Failed");
        return ret;
    }
    ret = share_pool->Drop(vds);
    if (SUCCESS != ret)
    {
        OutPut(SYS_ERROR,"VmDiskSharePool Drop Failed");
    }

    return ret;
}
/******************************************************************************/
STATUS CreateVmShareDisk(MessageUnit *mu, 
                              const VmDiskConfig &disk,
                              const VmDiskShare &vds,
                              const ImageStoreOption &option)
{
    int64 size = disk._size;
    string base_id;
    if (INVALID_FILEID != disk._id)
    {
        if (IMAGE_USE_SNAPSHOT == option._share_img_usage)
        {
            ImageCache ic;
            if (SUCCESS != GetCacheByCacheId(vds._cache_id,ic))
            {
                OutPut(SYS_ERROR,"CreateShareDisk for hid:%lld,vid:%lld,target:%s request_id:%s failed Select\n",vds._hid,vds._vid,disk._target.c_str(),vds._request_id.c_str());
                return ERROR_DB_SELECT_FAIL;
            }
            base_id = ic._request_id;
        }
        if (IMAGE_SOURCE_TYPE_BLOCK == option._img_srctype)
        {//块方式增加4M 用于存放结束标记
            size += 4*1024*1024;
        }
        else
        {
            size = (size*(option._img_spcexp+100))/100;
        }
    }
    
    int ret = CreateShareDisk(mu,vds._request_id,base_id,size);
    if (SUCCESS != ret)
    {
        OutPut(SYS_ERROR,"CreateShareDisk for hid:%lld,vid:%lld,target:%s request_id:%s failed ret:%d\n",vds._hid,vds._vid,disk._target.c_str(),vds._request_id.c_str(),ret);
        return ret;
    }
    
    OutPut(SYS_DEBUG,"CreateShareDisk for hid:%lld,vid:%lld,target:%s request_id:%s\n",vds._hid,vds._vid,disk._target.c_str(),vds._request_id.c_str());
    return SUCCESS;
}
/******************************************************************************/
STATUS CreateVmShareDisk(MessageUnit *mu, 
                              const VmDiskUrlReq& req, 
                              const VmDiskConfig &disk, 
                              VmDiskShare &vds, 
                              const ImageStoreOption &option)
{
    STATUS ret = ERROR;
    vds._request_id = GenerateUUID();
    vds._hid = req._hid;
    vds._vid = req._vid;
    vds._target = disk._target;
    int64 size = disk._size;
    string base_id;
    if (INVALID_FILEID == disk._id)
    {
        vds._cache_id = INVALID_OID;
    }
    else
    {
        if (IMAGE_USE_NOSNAPSHOT == option._share_img_usage)
        {
            vds._cache_id = INVALID_OID;
        }
        else
        {
            vector<ImageCache> vec_ic;
            ret = SelectCacheByImageId(disk._id, vec_ic);
            if (SUCCESS != ret)
            {
                OutPut(SYS_ERROR,"CreateShareDisk for hid:%lld,vid:%lld,target:%s request_id:%s failed Select Base ret:%d\n",req._hid,req._vid,disk._target.c_str(),vds._request_id.c_str(),ret);
                return ret;
            }
            
            vector<VmDiskShare> vec_vds;
            vector<ImageCache>::iterator it;
            for (it = vec_ic.begin(); it != vec_ic.end(); it++)
            {
                vec_vds.clear();
                ret = GetSnapshotByCacheId(it->_cache_id,vec_vds);
                if (SUCCESS != ret)
                {
                    OutPut(SYS_ERROR,"CreateShareDisk for hid:%lld,vid:%lld,target:%s request_id:%s failed Select SS ret:%d\n",req._hid,req._vid,disk._target.c_str(),vds._request_id.c_str(),ret);
                    return ERROR_DB_SELECT_FAIL;
                }
                if (MAX_SNAPSHOT_PER_CACHE <= vec_vds.size())
                {
                    continue;
                }
                else
                {
                    break;
                }
            }
            if (it != vec_ic.end())
            {
                vds._cache_id = it->_cache_id;
                base_id = it->_request_id;
            }
            else
            {
                OutPut(SYS_ERROR,"CreateShareDisk for hid:%lld,vid:%lld,target:%s request_id:%s failed for All Base Full\n",req._hid,req._vid,disk._target.c_str(),vds._request_id.c_str());
                return ERROR_OBJECT_NOT_EXIST;
            }
        }
        if (IMAGE_SOURCE_TYPE_BLOCK == option._img_srctype)
        {//块方式增加4M 用于存放结束标记
            size += 4*1024*1024;
        }
        else
        {
            size = (size*(option._img_spcexp+100))/100;
        }
    }

    vds._last_op = VDSO_CREATE;
    vds._state = VDSS_CREATEING;
    if(SUCCESS != AllocateShareDisk(vds))
    {
        OutPut(SYS_ERROR,"CreateShareDisk for hid:%lld,vid:%lld,target:%s request_id:%s failed for Insert\n",req._hid,req._vid,disk._target.c_str(),vds._request_id.c_str());
        return ERROR_DB_INSERT_FAIL;
    }

    ret = CreateShareDisk(mu,vds._request_id,base_id,size);
    if(SUCCESS != ret)
    {     
        OutPut(SYS_ERROR,"CreateShareDisk for hid:%lld,vid:%lld,target:%s request_id:%s failed ret:%d\n",req._hid,req._vid,disk._target.c_str(),vds._request_id.c_str(),ret);
        return ret;
    }
    
    OutPut(SYS_DEBUG,"CreateShareDisk for hid:%lld,vid:%lld,target:%s request_id:%s\n",req._hid,req._vid,disk._target.c_str(),vds._request_id.c_str());
    return SUCCESS;
}
/******************************************************************************/
STATUS AuthVmShareDisk(MessageUnit *mu, const VmDiskUrlReq& req,const VmDiskShare &vds)
{
    auth_info auth;
    auth.userip = req._host_ip;
    auth.iscsiname = req._iscsi_initiator;
    auth.cluster_name = ApplicationName();
    auth.request_id = vds._request_id;
    auth.vid = vds._vid;
    auth.dist_id = vds._cache_id;
    
    STATUS ret = AuthShareDisk(mu,auth);
    if (SUCCESS != ret)
    {
        OutPut(SYS_ERROR, "AuthShareDisk request_id %s failed ret %d\n",auth.request_id.c_str(),ret);
        return ret;
    }
    
    OutPut(SYS_DEBUG,"AuthShareDisk ip:%s,iscsi:%s,cluster:%s,request_id:%s,vid:%lld,dist_id:%lld!\n",
          auth.userip.c_str(),
          auth.iscsiname.c_str(),
          auth.cluster_name.c_str(),
          auth.request_id.c_str(),
          auth.vid,
          auth.dist_id);
    return SUCCESS;
}
/******************************************************************************/
STATUS PrepareVmShareDisk(MessageUnit *mu, const VmDiskUrlReq& req, const ImageStoreOption &option)
{
    uint32 ready_num = 0;
    int index = 0;
    STATUS ret = SUCCESS;
    VmDiskShare vds;
    VmDiskUrlAck ack(req); 
    vector<VmDiskConfig>::const_iterator it;
    
    OutPut(SYS_DEBUG,"PrepareVmShareDisk for hid:%lld,vid:%lld,ip:%s,iscsi:%s\n",req._hid,req._vid,req._host_ip.c_str(),req._iscsi_initiator.c_str());

    for(it = req._disks.begin();it != req._disks.end();it++)
    {
        ret = GetShareDiskByVmInfo(req._hid, req._vid, it->_target, vds);
        if (ERROR_OBJECT_NOT_EXIST == ret)
        {
            ret = CreateVmShareDisk(mu,req,*it,vds,option);
        }
        else if (SUCCESS == ret)
        {
            if (VDSS_CREATEING == vds._state
                || (VDSS_FAILED == vds._state && VDSO_CREATE == vds._last_op))
            {
                ret = CreateVmShareDisk(mu,*it,vds,option);
            }
            else if (VDSS_AUTHING == vds._state
                || (VDSS_FAILED == vds._state && VDSO_AUTH == vds._last_op))
            {
                ret = AuthVmShareDisk(mu,req,vds);
            }
            else if (VDSS_OK == vds._state)
            {
                ack._disks[index]._share_url = vds._disk_url;
                ready_num++;
            }
            else
            {
                OutPut(SYS_ERROR,"PrepareVmShareDisk for hid:%lld,vid:%lld failed for Invalid state:%s or last_op:%s\n",req._hid,req._vid,VmDiskShareStateStr(vds._state),VmDiskShareOpStr(vds._last_op));
                ret = ERROR;
                break;
            }
        }
        else
        {
            OutPut(SYS_ERROR,"PrepareVmShareDisk for hid:%lld,vid:%lld failed for Select\n",req._hid,req._vid);
            ret = ERROR;
            break;
        }
        index ++;
    }

    if (ready_num == req._disks.size())
    {
        //准备全了再发应答消息
        ack._result = SUCCESS;
        ack._error = "OK";
        uint32 msg_id;
        if (INVALID_FILEID != req._disks.begin()->_id)
        {
            msg_id = EV_VM_IMAGE_URL_PREPARE_ACK;
        }
        else
        {
            msg_id = EV_VM_DISK_URL_PREPARE_ACK;
        }
        MessageOption msg_option(mu->CurrentMessageFrame()->option.sender(),msg_id,0,0);
        ret = mu->Send(ack,msg_option); 
    }
    
    return ret;
}
/******************************************************************************/
STATUS DeAuthVmShareDisk(MessageUnit *mu, const VmDiskUrlReq& req,const VmDiskShare &vds)
{
    auth_info auth;
    auth.userip = req._host_ip;
    auth.iscsiname = req._iscsi_initiator;
    auth.cluster_name = ApplicationName();
    auth.request_id = vds._request_id;
    auth.vid = vds._vid;
    auth.dist_id = vds._cache_id;
    
    STATUS ret = DeAuthShareDisk(mu,auth);
    if (SUCCESS != ret)
    {
        OutPut(SYS_ERROR, "DeAuthShareDisk request_id %s failed ret %d\n",auth.request_id.c_str(),ret);
        return ret;
    }
    
    OutPut(SYS_DEBUG,"DeAuthShareDisk ip:%s,iscsi:%s,cluster:%s,request_id:%s,vid:%lld,dist_id:%lld!\n",
          auth.userip.c_str(),
          auth.iscsiname.c_str(),
          auth.cluster_name.c_str(),
          auth.request_id.c_str(),
          auth.vid,
          auth.dist_id);
    return SUCCESS;
}
/******************************************************************************/
STATUS ReleaseVmShareDisk(MessageUnit *mu, const VmDiskUrlReq &req)
{
    uint32 ok_num = 0;
    STATUS ret = SUCCESS;
    VmDiskShare vds;
    VmDiskUrlAck ack(req);
    vector<VmDiskConfig>::const_iterator it;

    OutPut(SYS_DEBUG,"ReleaseVmShareDisk for hid:%lld,vid:%lld,ip:%s,iscsi:%s\n",req._hid,req._vid,req._host_ip.c_str(),req._iscsi_initiator.c_str());
    for(it = req._disks.begin();it != req._disks.end();it++)
    {
        ret = GetShareDiskByVmInfo(req._hid, req._vid, it->_target, vds);
        if (ERROR_OBJECT_NOT_EXIST == ret)
        {//数据库中没有认为已经释放了
            OutPut(SYS_NOTICE,"ReleaseVmShareDisk Ok for db not exist\n");
            ok_num++;
            continue;
        }
        else if (SUCCESS == ret)
        {
            if (VDSS_DEAUTHING == vds._state
                || (VDSS_FAILED == vds._state && VDSO_DEAUTH == vds._last_op))
            {
                //发消息跟催
                ret = DeAuthVmShareDisk(mu,req,vds);
            }
            else if (VDSS_DELETEING == vds._state
                || (VDSS_FAILED == vds._state && VDSO_DELETE == vds._last_op))
            {
                ret = DeleteShareDisk(mu,vds._request_id);
            }
            else
            {
                //其他任何状态都开始释放的流程
                vds._state = VDSS_DEAUTHING;
                vds._last_op = VDSO_DEAUTH;
                ret = UpdateShareDiskState(vds);
                if (SUCCESS == ret)
                {
                    ret = DeAuthVmShareDisk(mu,req,vds);
                }
            }
        }
        else
        {
            OutPut(SYS_ERROR,"ReleaseVmShareDisk for hid:%lld,vid:%lld failed for Db Select Failed\n",req._hid,req._vid);
            ret = ERROR;
            break;
        }
    }
    
    if (ok_num == req._disks.size())
    {
        //释放全了再发应答消息
        ack._result = SUCCESS;
        ack._error = "OK";
        uint32 msg_id;
        if (INVALID_FILEID != req._disks.begin()->_id)
        {
            msg_id = EV_VM_IMAGE_URL_RELEASE_ACK;
        }
        else
        {
            msg_id = EV_VM_DISK_URL_RELEASE_ACK;
        }
        MessageOption option(mu->CurrentMessageFrame()->option.sender(),msg_id,0,0);
        ret = mu->Send(ack,option);
    }
    
    return ret;
}
/******************************************************************************/
STATUS DealVmShareDiskAck(MessageUnit *mu)
{
    return SUCCESS;
}

}// namespace zte_tecs

