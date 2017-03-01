#include "tecs_config.h"
#include "log_agent.h"
#include "mid.h"
#include "event.h"
#include "storage_messages.h"
#include "storage_control.h"

namespace zte_tecs
{
const int  MBYTE             = (1024 * 1024);  /*M字节数*/
const int64  LVM_SIZE      = ((int64)4*MBYTE);  /*lv块大小 lvm2存储空间管理的最小单位*/

const int  MIN_ALLOC_SIZE    =64;
static   map<string,  Mutex *>   s_tTargetMutex;
static Mutex *s_ptMapLock = NULL;

StorageController *StorageController::_instance = NULL;

MID Siv2Mid(const StorageVolumeWithAdaptor &si)
{
    MID mid;

    mid._application = si._application;
    mid._unit = si._unit;
    mid._process = PROC_SA;

    return mid;
}

int GetMapMutex(Mutex ** ppmutex)
{
    if (NULL == s_ptMapLock)
    {
        s_ptMapLock = new Mutex("LvMapLock");
        if (NULL == s_ptMapLock)
        {
            return ERROR;
        }
        else
        {
            if (SUCCESS != s_ptMapLock->Init())
            {
                delete s_ptMapLock;
                s_ptMapLock = NULL;
                return ERROR;
            }
        }
    }
    *ppmutex = s_ptMapLock;
    return SUCCESS;
}

int GetTargetMutex(const string target_name,Mutex ** ppmutex)
{
    Mutex *pMutex = NULL;
    Mutex *pMapMutex = NULL;

    if (SUCCESS != GetMapMutex(&pMapMutex))
    {
        return ERROR;
    }
    pMapMutex->Lock();
    map<string, Mutex *>::iterator it = s_tTargetMutex.find(target_name);
    if (it != s_tTargetMutex.end())
    {
        *ppmutex=it->second;
        pMapMutex->Unlock();
        return SUCCESS;
    }
    pMutex=new Mutex(target_name.c_str());
    if (NULL != pMutex)
    {
        if (SUCCESS != pMutex->Init())
        {
            delete pMutex;
        }
        *ppmutex=pMutex;
        s_tTargetMutex.insert(make_pair(target_name,pMutex));
    }
    pMapMutex->Unlock();
    return SUCCESS;
}



STATUS StorageController::Init(void)
{
    if (NULL == (_db_set = ScDbOperationSet::GetInstance(GetDB())))
    {
        return ERROR;
    }

    if (SUCCESS != StartMu())
    {
        OutPut(SYS_ALERT, "StartMu failed in StorageInstance::init() !\n");
        return ERROR;
    }
    MessageFrame frame(SkyInt(0),EV_POWER_ON);
    return m->Receive(frame);
}

STATUS StorageController::StartMu()
{
    if(m)
    {
        SkyAssert(false);
        return ERROR_DUPLICATED_OP;
    }

    // 消息单元的创建和初始化
    m = new MessageUnit(MU_SC_CONTROL);
    if (!m)
    {
        OutPut(SYS_EMERGENCY, "StorageController: Create mu %s failed!\n");
        return ERROR_NO_MEMORY;
    }

    STATUS ret = m->SetHandler(this);
    if (SUCCESS != ret)
    {
        OutPut(SYS_EMERGENCY, "StorageController: SetHandler mu  failed! ret = %d\n",ret);
        return ret;
    }

    ret = m->Run();
    if (SUCCESS != ret)
    {
        OutPut(SYS_EMERGENCY, "StorageController:Run mu  failed! ret = %d\n",ret);
        return ret;
    }

    ret = m->Register();
    if (SUCCESS != ret)
    {
        OutPut(SYS_EMERGENCY, "StorageController:Register mu  failed! ret = %d\n",ret);
        return ret;
    }
    m->Print();
    return SUCCESS;
}

STATUS StorageController::GenerateTargetName(const string  &type ,const AuthBlockReq &req, string &targetname)
{
    if (type==LOCALSAN && ((INVALID_VID != req._vid) || (INVALID_VID !=req._dist_id)))
    {
        if (INVALID_VID == req._vid)
        {
            targetname = "targethost_"+req._userip+"_"+to_string<int64>(req._dist_id,dec);
            replace(targetname.begin(), targetname.end(), '.', '_');
            replace(targetname.begin(), targetname.end(), ':', '_');
        }
        else
        {
            targetname="targetvm_"+to_string<int64>(req._vid,dec);
        }
    }
    else
    {

        string name="t_"+req._iscsiname;
        string cmd = "echo "+name;
        cmd+=  " | sed 's/[^a-zA-Z0-9_]/_/g'" ;
        string result;
        if(SUCCESS!=RunCmd(cmd, result))
        {
            OutPut(SYS_ERROR,"GenerateTargetName err! cmd:%s ,result:%s \n",cmd.c_str(),result.c_str());
            return ERROR;
        }

        if (result.empty())
        {
            return ERROR;
        }

		targetname=result.substr(0,result.size()-1);
    }
    return SUCCESS;
}

void StorageController::ScAckSend(uint32 req_msgid,MID &sender,req_info &rinfo,int64 &uid,WorkAck & w_ack,string &workflow_id)
{
    string reqstr;
    uint32 ack_msgid;

    if(w_ack.state==SUCCESS)
    {
        w_ack.progress=100;
    }
    else
    {
        w_ack.progress=0;
    }
    switch (req_msgid)
    {
        case EV_STORAGE_CREATE_BLOCK_REQ:
        {

            reqstr="create_volume";
            ack_msgid=EV_STORAGE_CREATE_BLOCK_ACK;
            CreateBlockAck ack(w_ack.action_id,w_ack.state,w_ack.progress,rinfo.request_id,uid,workflow_id);
            MessageOption option(sender,ack_msgid , 0, 0);
            m->Send(ack, option);

        }
        break;
        case EV_STORAGE_DELETE_BLOCK_REQ:
        {
            reqstr="delete_vol";
            ack_msgid=EV_STORAGE_DELETE_BLOCK_ACK;
            DeleteBlockAck ack(w_ack.action_id,w_ack.state,w_ack.progress,rinfo.request_id,uid,workflow_id);
            MessageOption option(sender,ack_msgid , 0, 0);
            m->Send(ack, option);
        }
        break;

        default:
        break;
    }

    OutPut(SYS_DEBUG,"ScAckSend: reqstr: %s,ret:%d requestid:%s,uid is %d \n",reqstr.c_str(),w_ack.state,rinfo.request_id.c_str(),uid);
}


void StorageController::ScAckSend(uint32 req_msgid,MID &sender,req_info &rinfo,int64 &uid,WorkAck & w_ack)
{
    string reqstr;
    uint32 ack_msgid;
    int ret;
    string workflow_id;

    if(w_ack.state==SUCCESS)
    {
        w_ack.progress=100;
    }
    else
    {
        w_ack.progress=0;
    }
    switch (req_msgid)
    {
        case EV_STORAGE_CREATE_BLOCK_REQ:
        {

            reqstr="create_volume";
            ack_msgid=EV_STORAGE_CREATE_BLOCK_ACK;
            if(w_ack.action_id.empty())
            {
                CreateBlockAck ack(w_ack.action_id,w_ack.state,w_ack.progress,rinfo.request_id,uid);
                MessageOption option(sender,ack_msgid , 0, 0);
                m->Send(ack, option);
            }
            else
            {
                GetWorkflowByActionId(w_ack.action_id,workflow_id);
                CreateBlockAck ack(w_ack.action_id,w_ack.state,w_ack.progress,rinfo.request_id,uid,workflow_id,w_ack.detail);
                MessageOption option(sender,ack_msgid , 0, 0);
                m->Send(ack, option);
            }
            

        }
        break;
        case EV_STORAGE_DELETE_BLOCK_REQ:
        {
            reqstr="delete_vol";
            ack_msgid=EV_STORAGE_DELETE_BLOCK_ACK;
            DeleteBlockAck ack(w_ack.action_id,w_ack.state,w_ack.progress,rinfo.request_id,uid);
            MessageOption option(sender,ack_msgid , 0, 0);
            m->Send(ack, option);
        }
        break;
        case EV_STORAGE_CREATE_SNAPSHOT_REQ:
        {
            reqstr="create_svol";
            ack_msgid=EV_STORAGE_CREATE_SNAPSHOT_ACK;
            if(w_ack.action_id.empty())
            {
                CreateSnapshotAck ack(w_ack.action_id,w_ack.state,w_ack.progress,rinfo.request_id);
                MessageOption option(sender,ack_msgid , 0, 0);
                m->Send(ack, option);
            }
            else
            {
                GetWorkflowByActionId(w_ack.action_id,workflow_id);
                CreateSnapshotAck ack(w_ack.action_id,w_ack.state,w_ack.progress,rinfo.request_id,uid,workflow_id,w_ack.detail);
                MessageOption option(sender,ack_msgid , 0, 0);
                m->Send(ack, option);
            }
            
        }
        break;
        case EV_STORAGE_DELETE_SNAPSHOT_REQ:
        {
            reqstr="delete_svol";
            ack_msgid=EV_STORAGE_DELETE_SNAPSHOT_ACK;
            DeleteSnapshotAck ack(w_ack.action_id,w_ack.state,w_ack.progress,rinfo.request_id);
            MessageOption option(sender,ack_msgid , 0, 0);
            m->Send(ack, option);

        }
        break;
        case EV_STORAGE_AUTH_BLOCK_REQ:
        {
            reqstr="auth_vol";
            ack_msgid=EV_STORAGE_AUTH_BLOCK_ACK;
            MessageOption option(sender,ack_msgid , 0, 0);
            AuthBlockAck ack(w_ack.action_id,w_ack.state,w_ack.progress,rinfo.request_id);
            if(w_ack.state!=SUCCESS)
            {
                 m->Send(ack, option);
                 break;
            }
            StorageVolumeWithLunAndAuthAndAdaptor svwlaa;
            StorageVolume sv;
            DbGetStorageVolumeByRequestId(rinfo.request_id,sv);
            ret=DbGetTargetInfo(sv._volume_uuid,rinfo.ip,rinfo.iscsi_name,svwlaa);
            if(ret==SUCCESS)
            {
                if (svwlaa._type==LOCALSAN)
                {
                    ack._lunid=svwlaa._lun_id+1;
                }
                else
                {
                    ack._lunid=svwlaa._lun_id;
                }
                ack._target_name=svwlaa._target_name;
                ack._type=svwlaa._type;
                ack._request_id = rinfo.request_id;
                DbGetMediaAddressBySid(sv._sid,ack._dataip_vec);
            }
            ack.state=ret;
            ack.progress=(ret==SUCCESS?100:0);
            m->Send(ack, option);
        }
        break;
        case EV_STORAGE_DEAUTH_BLOCK_REQ:
        {
            reqstr="deauth_vol";
            ack_msgid=EV_STORAGE_DEAUTH_BLOCK_ACK;
            DeAuthBlockAck ack(w_ack.action_id,w_ack.state,w_ack.progress,rinfo.request_id);
            MessageOption option(sender,ack_msgid , 0, 0);
            m->Send(ack, option);
        }
        break;

        case EV_STORAGE_GET_BLOCKINFO_REQ:
        {
            reqstr="get_volinfo";
            ack_msgid=EV_STORAGE_GET_BLOCKINFO_ACK;
            MessageOption option(sender,ack_msgid , 0, 0);
            GetBlockInfoAck ack(w_ack.action_id,w_ack.state,w_ack.progress,rinfo.request_id);
            if(w_ack.state!=SUCCESS)
            {
                 m->Send(ack, option);
                 break;
            }
            StorageVolumeWithLunAndAuthAndAdaptor svwlaa;
            StorageVolume sv;
            ret=DbGetStorageVolumeByRequestId(rinfo.request_id,sv);
            if(SUCCESS!=ret)
            {
                OutPut(SYS_DEBUG, "reqstr:%s requestid:%s not exist!\n",reqstr.c_str(),rinfo.request_id.c_str());
                ack.state=ret;
                ack.progress=0;
                m->Send(ack, option);
                break;
            }
            ret=DbGetTargetInfo(sv._volume_uuid,svwlaa);
            if(ret==SUCCESS)
            {
                if (svwlaa._type==LOCALSAN)
                {
                    ack._lunid=svwlaa._lun_id+1;
                }
                else
                {
                    ack._lunid=svwlaa._lun_id;
                }
                ack._target_name=svwlaa._target_name;
                ack._type=svwlaa._type;
                ack._request_id = rinfo.request_id;
                ack._iscsi_name=svwlaa._iscsi_initiator;
                ack._ip_address=svwlaa._ip_address;
                DbGetMediaAddressBySid(sv._sid,ack._dataip_vec);
            }
            ack.state=0;
            ack.progress=100;
            m->Send(ack, option);     
        }
        break;
        default:
        break;
    }

    OutPut(SYS_DEBUG,"ScAckSend: reqstr: %s,ret:%d requestid:%s,uid is %d \n",reqstr.c_str(),w_ack.state,rinfo.request_id.c_str(),uid);

}

STATUS StorageController::GetSnapBaseVolume(vector<string> &vec_baseid,StorageVolume &sv)
{
    vector<string>::iterator it=vec_baseid.begin();
    int64 baseid_size=-1;
    STATUS  ret=SUCCESS;
    StorageVolume  sv_tmp;
    int64 index=rand()%vec_baseid.size();
    for(;it!=vec_baseid.end();it++)
    {
        ret = DbGetStorageVolumeByRequestId(*it,sv_tmp);
        if (SUCCESS != ret)
        {
            if (ERROR_OBJECT_NOT_EXIST == ret)
            {
                //base不存在
                OutPut(SYS_ERROR, "GetSnapBaseVolume:DbGetStorageVolumeByRequestId Failed for Base %s Not Exist\n",(*it).c_str());

            }
            else
            {
                OutPut(SYS_ERROR, "GetSnapBaseVolume:DbGetStorageVolumeByRequestId Failed\n");
            }
            return ret;
        }
        if(baseid_size==-1)
        {
            baseid_size=sv_tmp._size;
        }
        else if(baseid_size!=sv_tmp._size )
        {
            OutPut(SYS_ERROR, "GetSnapBaseVolume:base id mismatch\n");
            return  ERROR_INVALID_ARGUMENT;

        }

    }
    return DbGetStorageVolumeByRequestId(vec_baseid[index],sv);
}

void StorageController::MessageEntry(const MessageFrame& message)
{
    StStorageSvrAck  sa_ack;
    MID         sa;
    MID  sender;
    StorageVolumeWithAdaptor si;
    int        ret=SUCCESS;
    sender = message.option.sender();
    WorkReq wreq;
    wreq.deserialize(message.data);
    WorkAck w_ack(wreq.action_id);
    int64 uid = 0;
    switch (m->get_state())
    {
    case S_Startup:
    {
        switch (message.option.id())
        {
        case EV_POWER_ON:
        {
            Transaction::Enable(GetDB());
            DbScanTargetLunid();
            m->set_state(S_Work);
        }

        break;
        default:
            break;
        }
        break;
    }
    case S_Work:
    {
        switch (message.option.id())
        {
        case EV_STORAGE_CREATE_BLOCK_REQ:
        {
            //反序列化请求消息
            CreateBlockReq req;
            req.deserialize(message.data);
            string workflow_id;

            //填充storage adaptor请求消息
            StAllocVolReq  sa_req;
            int64  size = (req._volsize % MBYTE ==0)?(req._volsize / MBYTE):(req._volsize / MBYTE + 1);
            if(size<MIN_ALLOC_SIZE)
            {
                size=MIN_ALLOC_SIZE;
            }
            sa_req._volsize=size;
            StorageVolume sv;
            req_info      rinfo(req._request_id);
            ret = DbGetStorageVolumeByRequestId(req._request_id,sv);
            if(ret!=SUCCESS && ret !=ERROR_OBJECT_NOT_EXIST)
            {
                OutPut(SYS_ERROR, "GetStorageVolumeByRequestId Failed\n");
                w_ack.state = ERROR_DB_SELECT_FAIL;
                w_ack.progress=0;
                ScAckSend(message.option.id(), sender, rinfo, uid,w_ack);
                break;
            }
            uid = req._uid;
            CreateVolWorkflowLabels cvwf;
            cvwf.request_id=req._request_id;
            vector<string> wf_plus,wf_minus,wf_rollback;
            FindWorkflow(wf_plus,STORAGE_CATEGORY,CREATE_VOLUME,cvwf.get_label_where());
            FindWorkflow(wf_minus,STORAGE_CATEGORY,DELETE_VOLUME,cvwf.get_label_where(),false);
            FindWorkflow(wf_rollback,STORAGE_CATEGORY,DELETE_VOLUME,cvwf.get_label_where(),true);
            if(!wf_plus.empty())
            {
                w_ack.progress= GetWorkflowProgress(wf_plus[0]);
                w_ack.state=ERROR_ACTION_RUNNING;
                workflow_id = wf_plus[0];
            }
            else if(!wf_rollback.empty())
            {
                w_ack.progress= GetWorkflowProgress(wf_rollback[0]);
                w_ack.state=ERROR_ACTION_RUNNING;
                workflow_id = wf_rollback[0];
            }
            else if(!wf_minus.empty())
            {
                w_ack.progress=0;
                w_ack.state=ERROR_PERMISSION_DENIED;
            }
            else if (ret==SUCCESS)
            {//工作流跟催会走到这
                vector<string> wf_history;
                w_ack.state = SUCCESS;
                w_ack.progress=100;
                FindWorkflowHistory(wf_history,STORAGE_CATEGORY,CREATE_VOLUME,cvwf.get_label_where());
                if(!wf_history.empty())
                {
                    workflow_id = wf_history[0];
                }
                OutPut(SYS_DEBUG,"volume is exist , workflow_id is %s \n",workflow_id.c_str());
            }
            else if(ret==ERROR_OBJECT_NOT_EXIST)
            {
                ret=ScheduletoAdaptor(req._cluster_name,size,si);;
                if (SUCCESS!=ret)
                {
                    w_ack.state = ret;
                    w_ack.progress=0;
                    ScAckSend(message.option.id(), sender, rinfo,uid,w_ack);
                    OutPut(SYS_ERROR, "storage can't select the sa ,with name %s size %ld \n",req._cluster_name.c_str(),req._volsize);
                    break;
                }

                DbGetStorageVolumeIndex(sv._volume_id);
                sa_req._vgname=si._vg_name;
                sa_req._volname="lvm_"+to_string<int64>(sv._volume_id,dec);
                ret = CreateVol(si,req,sa_req,sv._volume_id,sender,workflow_id);
                if(ret == SUCCESS)
                {
                    w_ack.state=ERROR_ACTION_RUNNING;
                }
                else
                {
                    w_ack.state=ret;
                    w_ack.detail = "create workflow fail";
                }
                w_ack.progress=0;
            }
            ScAckSend(message.option.id(), sender, rinfo, uid,w_ack,workflow_id);
            OutPut(SYS_DEBUG,"rcv EV_CLIENT_CREATE_BLOCK size is %ld ret is %d \n",req._volsize,ret);
        }
        break;

        case EV_STORAGE_CREATE_SNAPSHOT_REQ:
        {
            //反序列化请求消息
            CreateSnapshotReq req;
            req.deserialize(message.data);
            //获取base的卷信息
            StorageVolume sv;
            req_info      rinfo(req._request_id);
            ret=GetSnapBaseVolume(req._vec_baseid,sv);
            if(ret!=SUCCESS)
            {
                w_ack.state = ret;
                w_ack.progress=0;
                ScAckSend(message.option.id(), sender, rinfo, uid,w_ack);
                break;
            }
            StorageAdaptor sa;
            DbGetStorageAdaptorBySid(sv._sid,sa);
            if(sa._enabled==false || sa._is_online==false)
            {

                w_ack.state  = ERROR_PERMISSION_DENIED;
                w_ack.progress=0;
                OutPut(SYS_ERROR, "corresponding sa is maintained, can't alloc vol\n");
                ScAckSend(message.option.id(), sender, rinfo,uid, w_ack);
                break;
            }
            //获取storage adaptor信息备用
            StorageVolumeWithAdaptor svwa;
            ret = DbGetVolumeWithAdaptorInfoByUUID(sv._volume_uuid,svwa);
            if (SUCCESS != ret)
            {
                if (ERROR_OBJECT_NOT_EXIST == ret)
                {
                    //base不存在
                    OutPut(SYS_ERROR, "DbGetVolumeWithAdaptorInfoByUUID Failed for Base %s Not Exist\n",sv._request_id.c_str());
                    w_ack.state = ERROR;
                }
                else
                {
                    OutPut(SYS_ERROR, "DbGetVolumeWithAdaptorInfoByUUID Failed\n");
                    w_ack.state  = ERROR_DB_SELECT_FAIL;
                }
                w_ack.progress=0;
                ScAckSend(message.option.id(), sender, rinfo, uid,w_ack);
                break;
            }

            //填充请求消息
            int64  size = (req._volsize % MBYTE ==0)?(req._volsize / MBYTE):(req._volsize / MBYTE + 1);
            if(size<MIN_ALLOC_SIZE)
            {
                size=MIN_ALLOC_SIZE;
            }
            StAllocSnapshotReq  sa_req;
            sa_req._volsize = size;
            sa_req._base_name = sv._name;
            sa_req._vgname = svwa._vg_name;

             //判断数据库中是否存在卷
            StorageVolume sv_snap;
            ret = DbGetStorageVolumeByRequestId(req._request_id,sv_snap);
            if(ret!=SUCCESS && ret !=ERROR_OBJECT_NOT_EXIST)
            {
                OutPut(SYS_ERROR, "GetStorageVolumeByRequestId Failed\n");
                w_ack.state  = ERROR_DB_SELECT_FAIL;
                w_ack.progress=0;
                ScAckSend(message.option.id(), sender, rinfo, uid,w_ack);
                break;
            }

            CreateVolWorkflowLabels cvwf;
            cvwf.request_id=req._request_id;
            vector<string> wf_plus,wf_minus,wf_rollback;
            FindWorkflow(wf_plus,STORAGE_CATEGORY,CREATE_VOLUME,cvwf.get_label_where());
            FindWorkflow(wf_minus,STORAGE_CATEGORY,DELETE_VOLUME,cvwf.get_label_where(),false);
            FindWorkflow(wf_rollback,STORAGE_CATEGORY,DELETE_VOLUME,cvwf.get_label_where(),true);
            if(!wf_plus.empty())
            {
                w_ack.progress= GetWorkflowProgress(wf_plus[0]);
                w_ack.state  = ERROR_ACTION_RUNNING;
            }
            else if(!wf_rollback.empty())
            {
                w_ack.progress= GetWorkflowProgress(wf_rollback[0]);
                w_ack.state  = ERROR_ACTION_RUNNING;
            }
            else if(!wf_minus.empty())
            {
                w_ack.progress= 0;
                w_ack.state  = ERROR_PERMISSION_DENIED;
            }
            else if (ret==SUCCESS)
            {
                w_ack.progress=100;
                w_ack.state  = SUCCESS;
            }
            else if(ret==ERROR_OBJECT_NOT_EXIST)
            {
                 //begin creatework
                DbGetStorageVolumeIndex(sv_snap._volume_id);
                sa_req._volname="lvm_"+to_string<int64>(sv_snap._volume_id,dec);
                CreateSnapshot(svwa,req,sa_req,sv_snap._volume_id,sv._request_id,sender);
                w_ack.progress=0;
                w_ack.state  = ERROR_ACTION_RUNNING;
            }
            ScAckSend(message.option.id(), sender, rinfo, uid,w_ack);
            OutPut(SYS_DEBUG,"rcv EV_CLIENT_CREATE_SNAPSHOT size is %ld ret is %d \n",req._volsize,ret);
        }
        break;

        case EV_STORAGE_DELETE_BLOCK_REQ:
        {
            string workflow_id;
            DeleteBlockReq req;
            req.deserialize(message.data);
            uid = req._uid;
            OutPut(SYS_DEBUG,"rcv EV_CLIENT_DEL_BLOCK request_id is %s \n",req._request_id.c_str());
            StorageVolume sv;
            req_info      rinfo(req._request_id);
            ret = DbGetStorageVolumeByRequestId(req._request_id, sv);
            if(ret!=SUCCESS && ret !=ERROR_OBJECT_NOT_EXIST)
            {
                OutPut(SYS_ERROR, "GetStorageVolumeByRequestId Failed\n");
                w_ack.state  = ERROR_DB_SELECT_FAIL;
                w_ack.progress=0;
                ScAckSend(message.option.id(), sender, rinfo, uid,w_ack);
                break;
            }
            StorageSnapshot  snapvar;
            int   snap_num=0;
            snapvar._base_uuid=req._request_id;
            DbGetSnapNum(snapvar,snap_num);
            if(snap_num>0)
            {
                OutPut(SYS_ERROR, "request_id:%s still has snapshot,can't delete \n");
                w_ack.state  = ERROR_PERMISSION_DENIED;
                w_ack.progress=0;
                ScAckSend(message.option.id(), sender, rinfo, uid,w_ack);
                break;
            }
            DelVolWorkflowLabels dvwf;
            dvwf.request_id=req._request_id;
            vector<string> wf_plus,wf_minus,wf_rollback;
            FindWorkflow(wf_plus,STORAGE_CATEGORY,CREATE_VOLUME,dvwf.get_label_where());
            FindWorkflow(wf_minus,STORAGE_CATEGORY,DELETE_VOLUME,dvwf.get_label_where(),false);
            FindWorkflow(wf_rollback,STORAGE_CATEGORY,DELETE_VOLUME,dvwf.get_label_where(),true);
            if(!wf_plus.empty())
            {
                string r_id;
                GetRollbackWorkflow(wf_plus[0],r_id);
                RollbackWorkflow(wf_plus[0]);
                UnsetRollbackAttr(r_id,sender,req.action_id);
                w_ack.state  = ERROR_ACTION_RUNNING;
                w_ack.progress=GetWorkflowProgress(r_id);
                workflow_id=r_id;

            }
            else if(!wf_rollback.empty())
            {
                UnsetRollbackAttr(wf_rollback[0],sender,req.action_id);
                w_ack.state  = ERROR_ACTION_RUNNING;
                w_ack.progress=GetWorkflowProgress(wf_rollback[0]);
                workflow_id = wf_rollback[0];
            }
            else if(!wf_minus.empty())
            {
                w_ack.state  = ERROR_ACTION_RUNNING;
                w_ack.progress=GetWorkflowProgress(wf_minus[0]);
                workflow_id = wf_minus[0];
            }
            else if (ret==ERROR_OBJECT_NOT_EXIST)
            {
                w_ack.state  = SUCCESS;
                w_ack.progress=100;

                //工作流存在历史工作里或根本没有工作流暂不赋值workflow_id
            }
            else if(ret==SUCCESS)
            {
                if (DbIsVolumeInStorageTarget(sv))
                {//这儿不能删 因为卷还在映射组中
                    OutPut(SYS_ERROR,"DbIsVolumeInStorageTarget true: vol still use ,can't be delete\n");
                    w_ack.state  = ERROR_NOT_PERMITTED;
                    w_ack.progress=0;
                }
                else
                {
                    StDelVolReq  sa_req;
                    if (SUCCESS != DbGetVolumeWithAdaptorInfoByUUID(sv._volume_uuid,si))
                    {
                        OutPut(SYS_DEBUG, "EV_CLIENT_DEL_BLOCK: uuid not exist ! \n");
                        w_ack.state  = ERROR;
                        w_ack.progress=0;
                        ScAckSend(message.option.id(), sender, rinfo, uid,w_ack);
                        break;
                    }
                    sa_req._volname = si._vol_name;
                    sa_req._vgname= si._vg_name;
                    ret = DeleteVol(si,message.option.id(),req,sa_req,sv,sender,workflow_id);
                    if(ret == SUCCESS)
                    {
                        w_ack.state  = ERROR_ACTION_RUNNING;
                    }
                    else
                    {
                        w_ack.state=ret;
                        w_ack.detail = "create workflow fail";
                    }
                    w_ack.progress=0;

                }
            }
            if(message.option.id()==EV_STORAGE_DELETE_BLOCK_REQ)
            {
	        ScAckSend(message.option.id(), sender, rinfo, uid,w_ack,workflow_id);
            }
            else
            {
                ScAckSend(message.option.id(), sender, rinfo, uid,w_ack);
            }
            OutPut(SYS_DEBUG,"rcv EV_CLIENT_DEL_BLOCK  ret is %d\n",ret);
        }
        break;

        case EV_STORAGE_DELETE_SNAPSHOT_REQ:
        {
            string workflow_id;
            DeleteSnapshotReq req;
            req.deserialize(message.data);
            uid = req._uid;
            OutPut(SYS_DEBUG,"rcv EV_CLIENT_DEL_SNAPSHOT request_id is %s \n",req._request_id.c_str());
            StorageVolume sv;
            req_info      rinfo(req._request_id);
            ret = DbGetStorageVolumeByRequestId(req._request_id, sv);
            if(ret!=SUCCESS && ret !=ERROR_OBJECT_NOT_EXIST)
            {
                OutPut(SYS_ERROR, "GetStorageVolumeByRequestId Failed\n");
                w_ack.state  = ERROR_DB_SELECT_FAIL;
                w_ack.progress=0;
                ScAckSend(message.option.id(), sender, rinfo, uid,w_ack);
                break;
            }
            StorageSnapshot  snapvar;
            int   snap_num=0;
            snapvar._base_uuid=req._request_id;
            DbGetSnapNum(snapvar,snap_num);
            if(snap_num>0)
            {
                OutPut(SYS_ERROR, "request_id:%s still has snapshot,can't delete \n");
                w_ack.state  = ERROR_PERMISSION_DENIED;
                w_ack.progress=0;
                ScAckSend(message.option.id(), sender, rinfo, uid,w_ack);
                break;
            }
            DelVolWorkflowLabels dvwf;
            dvwf.request_id=req._request_id;
            vector<string> wf_plus,wf_minus,wf_rollback;
            FindWorkflow(wf_plus,STORAGE_CATEGORY,CREATE_VOLUME,dvwf.get_label_where());
            FindWorkflow(wf_minus,STORAGE_CATEGORY,DELETE_VOLUME,dvwf.get_label_where(),false);
            FindWorkflow(wf_rollback,STORAGE_CATEGORY,DELETE_VOLUME,dvwf.get_label_where(),true);
            if(!wf_plus.empty())
            {
                string r_id;
                GetRollbackWorkflow(wf_plus[0],r_id);
                RollbackWorkflow(wf_plus[0]);
                UnsetRollbackAttr(r_id,sender,req.action_id);
                w_ack.state  = ERROR_ACTION_RUNNING;
                w_ack.progress=GetWorkflowProgress(r_id);
                workflow_id=r_id;

            }
            else if(!wf_rollback.empty())
            {
                UnsetRollbackAttr(wf_rollback[0],sender,req.action_id);
                w_ack.state  = ERROR_ACTION_RUNNING;
                w_ack.progress=GetWorkflowProgress(wf_rollback[0]);
                workflow_id = wf_rollback[0];
            }
            else if(!wf_minus.empty())
            {
                w_ack.state  = ERROR_ACTION_RUNNING;
                w_ack.progress=GetWorkflowProgress(wf_minus[0]);
                workflow_id = wf_minus[0];
            }
            else if (ret==ERROR_OBJECT_NOT_EXIST)
            {
                w_ack.state  = SUCCESS;
                w_ack.progress=100;

                //工作流存在历史工作里或根本没有工作流暂不赋值workflow_id
            }
            else if(ret==SUCCESS)
            {
                if (DbIsVolumeInStorageTarget(sv))
                {//这儿不能删 因为卷还在映射组中
                    OutPut(SYS_ERROR,"DbIsVolumeInStorageTarget true: vol still use ,can't be delete\n");
                    w_ack.state  = ERROR_NOT_PERMITTED;
                    w_ack.progress=0;
                }
                else
                {
                    StDelVolReq  sa_req;
                    DeleteBlockReq del_req;
                    if (SUCCESS != DbGetVolumeWithAdaptorInfoByUUID(sv._volume_uuid,si))
                    {
                        OutPut(SYS_DEBUG, "EV_CLIENT_DEL_BLOCK: uuid not exist ! \n");
                        w_ack.state  = ERROR;
                        w_ack.progress=0;
                        ScAckSend(message.option.id(), sender, rinfo, uid,w_ack);
                        break;
                    }
                    sa_req._volname = si._vol_name;
                    sa_req._vgname= si._vg_name;

                    del_req._uid = req._uid;
                    del_req._request_id= req._request_id;
                    del_req._user= req._user;
                    del_req.action_id = req.action_id;
                    del_req.retry = req.retry;

                    ret = DeleteVol(si,message.option.id(),del_req,sa_req,sv,sender,workflow_id);
                    if(ret == SUCCESS)
                    {
                        w_ack.state  = ERROR_ACTION_RUNNING;
                    }
                    else
                    {
                        w_ack.state=ret;
                        w_ack.detail = "create workflow fail";
                    }
                    w_ack.progress=0;

                }
            }
            if(message.option.id()==EV_STORAGE_DELETE_BLOCK_REQ)
            {
                ScAckSend(message.option.id(), sender, rinfo, uid,w_ack,workflow_id);
            }
            else
            {
                ScAckSend(message.option.id(), sender, rinfo, uid,w_ack);
            }
            OutPut(SYS_DEBUG,"rcv EV_CLIENT_DEL_SNAPSHOT  ret is %d\n",ret);
        }
        break;

        case EV_STORAGE_AUTH_BLOCK_REQ:
        {
            AuthBlockReq req;
            string        targetname;
            StorageVolume sv;
            req.deserialize(message.data);
            req_info      rinfo(req._request_id);
            rinfo.ip=req._userip;
            rinfo.iscsi_name=req._iscsiname;

            if(req._iscsiname.empty())
            {
                OutPut(SYS_ERROR, "EV_CLIENT_AUTH_BLOCK:  iscsiname is  NULL ! \n");
                w_ack.state  = ERROR_INVALID_ARGUMENT;
                w_ack.progress=0;
                ScAckSend(message.option.id(), sender, rinfo, uid,w_ack);
                break;
            }

            OutPut(SYS_DEBUG,"rcv EV_CLIENT_AUTH_BLOCK vid is %ld request_id is %s \n",req._vid,req._request_id.c_str());
            ret = DbGetStorageVolumeByRequestId(req._request_id, sv);
            if (ERROR_OBJECT_NOT_EXIST == ret)
            {
                OutPut(SYS_DEBUG, "EV_CLIENT_AUTH_BLOCK: request_id %s not exist ! \n",req._request_id.c_str());

                w_ack.state  = ERROR_OBJECT_NOT_EXIST;
                w_ack.progress=0;
                ScAckSend(message.option.id(), sender, rinfo, uid,w_ack);
                break;
            }
            else if (SUCCESS != ret)
            {
                w_ack.state  = ERROR_DB_SELECT_FAIL;
                w_ack.progress=0;
                ScAckSend(message.option.id(), sender, rinfo, uid,w_ack);
                break;
            }
            ret=DbGetVolumeWithAdaptorInfoByUUID(sv._volume_uuid, si);
            if(ret!=SUCCESS)
            {
                /*对象已经不存在*/
                OutPut(SYS_DEBUG, "EV_CLIENT_AUTH_BLOCK: uuid not exist ! \n");
                w_ack.state  = ERROR_DB_SELECT_FAIL;
                w_ack.progress=0;
                ScAckSend(message.option.id(), sender, rinfo, uid,w_ack);
                break;
            }
            GenerateTargetName(si._type,req,targetname);
            bool tbl_exist=DbIsTargetInfoExist(sv._volume_uuid,req._userip,req._iscsiname);

            AuthWorkflowLabels auwf;
            auwf.request_id=req._request_id;
            auwf.ip_address=req._userip;
            auwf.iscsi_initiator=req._iscsiname;
            auwf.target_name=targetname;
            auwf.usage = req._usage;

            vector<string> wf_plus,wf_minus,wf_rollback;
            FindWorkflow(wf_plus,STORAGE_CATEGORY,AUTH_VOLUME,auwf.get_label_where());
            FindWorkflow(wf_minus,STORAGE_CATEGORY,DEAUTH_VOLUME,auwf.get_label_where(),false);
            FindWorkflow(wf_rollback,STORAGE_CATEGORY,DEAUTH_VOLUME,auwf.get_label_where(),true);
            if(!wf_plus.empty())
            {
                w_ack.state  = ERROR_ACTION_RUNNING;
                w_ack.progress=GetWorkflowProgress(wf_plus[0]);


            }
            else if(!wf_rollback.empty())
            {
                w_ack.state  = ERROR_ACTION_RUNNING;
                w_ack.progress=GetWorkflowProgress(wf_rollback[0]);
            }
            else if(!wf_minus.empty())
            {
                w_ack.state  = ERROR_PERMISSION_DENIED;
                w_ack.progress=0;
            }
            else if (tbl_exist)
            {
                w_ack.state  = SUCCESS;
                w_ack.progress=100;
            }
            else if(!tbl_exist)
            {
                wf_plus.clear();
                wf_minus.clear();
                FindWorkflow(wf_plus,STORAGE_CATEGORY,AUTH_VOLUME,auwf.get_target_where());
                FindWorkflow(wf_minus,STORAGE_CATEGORY,DEAUTH_VOLUME,auwf.get_target_where());
                if(!(wf_plus.empty()&& wf_minus.empty()))
                {
                    w_ack.state  = ERROR_ACTION_RUNNING;
                    w_ack.progress=0;
                    ScAckSend(message.option.id(), sender, rinfo, uid,w_ack);
                    break;
                }

                Mutex  *pTargetMutex = NULL;
                GetTargetMutex(targetname,&pTargetMutex);
                pTargetMutex->Lock();
                int64  target_id;
                Authinfo info;
                info.target_name=targetname;

                StorageTarget st;
                st._sid=si._sid;
                st._name=targetname;
                info.create_target_skiped=DbIsStorageTargetExist(st);
                if(info.create_target_skiped)
                {
                    info.target_id=st._target_id;
                }
                else
                {
                    DbGetStorageTargetIndex(si._sid,target_id);
                    info.target_id=target_id;
                }

                StorageAuthorize sa;
                sa._sid=si._sid;
                sa._ip_address=req._userip;
                sa._iscsi_initiator=req._iscsiname;
                sa._target_name=targetname;
                info.add_host_skiped=DbIsHostAuthorized(sa);

                StorageLun sl;
                sl._sid=si._sid;
                sl._volume_uuid=si._vol_uuid;
                sl._target_name=targetname;
                info.add_vol_skiped=DbIsStorageLunExist(sl);
                if(!info.add_vol_skiped)
                {
                    DbGetFreeLunidByTarget(sl);
                }
                info.lunid=sl._lun_id;

                if(info.create_target_skiped && info.add_host_skiped && info.add_vol_skiped)
                {

                    StorageAuthorizeLun sal;
                    sal._auth_uuid=sa._auth_uuid;
                    sal._lun_uuid=sl._lun_uuid;
                    DbAddAuthorizeLun(sal);
                    w_ack.state  = SUCCESS;
                    w_ack.progress=100;
                    ScAckSend(message.option.id(), sender, rinfo, uid,w_ack);
                    break;
                }
                info.sender=sender;
                AuthVol(si, req,info);
                w_ack.state  = ERROR_ACTION_RUNNING;
                w_ack.progress=0;
                pTargetMutex->Unlock();
            }
            ScAckSend(message.option.id(), sender, rinfo, uid,w_ack);
            OutPut(SYS_DEBUG,"rcv EV_CLIENT_AUTH_BLOCK ret is %d ,target name is %s request_id:%s\n",ret,targetname.c_str(),req._request_id.c_str());
        }
        break;

        case EV_STORAGE_DEAUTH_BLOCK_REQ:
        {
            StorageVolumeWithLunAndAuthAndAdaptor  svwlaa;
            DeAuthBlockReq req;
            int64            targetindex;
            string          targetname;
            req.deserialize(message.data);
            OutPut(SYS_DEBUG,"rcv EV_CLIENT_DEAUTH_BLOCK request_id is %s \n",req._request_id.c_str());

            StorageVolume sv;
            req_info      rinfo(req._request_id);
            ret = DbGetStorageVolumeByRequestId(req._request_id, sv);
            if (ERROR_OBJECT_NOT_EXIST == ret)
            {

                OutPut(SYS_DEBUG, "EV_CLIENT_DEAUTH_BLOCK: request_id %s not exist ! \n",req._request_id.c_str());
                w_ack.state  = SUCCESS;
                w_ack.progress=100;
                ScAckSend(message.option.id(), sender, rinfo, uid,w_ack);
                break;
            }
            else if (SUCCESS != ret)
            {
                w_ack.state  = ERROR_DB_SELECT_FAIL;
                w_ack.progress=0;
                ScAckSend(message.option.id(), sender, rinfo, uid,w_ack);
                break;
            }
            ret=DbGetVolumeWithAdaptorInfoByUUID(sv._volume_uuid, si);
            if(ret==ERROR_OBJECT_NOT_EXIST)
            {
                /*对象已经不存在，则直接认为成功*/
                OutPut(SYS_DEBUG, "EV_CLIENT_DEAUTH_BLOCK: uuid not exist ! \n");
                w_ack.state  = SUCCESS;
                w_ack.progress=100;
                ScAckSend(message.option.id(), sender, rinfo, uid,w_ack);
                break;
            }
            GenerateTargetName(si._type,req,targetname);
            StorageAuthorize sa_info;
            sa_info._target_name=targetname;
            sa_info._sid=si._sid;
            sa_info._iscsi_initiator=req._iscsiname;
            if(SUCCESS==DbGetStorageAuthorize(sa_info))
            {
                OutPut(SYS_DEBUG, "EV_CLIENT_DEAUTH_BLOCK: userip %s change to  %s\n",req._userip.c_str(),sa_info._ip_address.c_str());
                req._userip=sa_info._ip_address;
            }
            bool tbl_exist=DbIsTargetInfoExist(sv._volume_uuid,req._userip,req._iscsiname);
            DbGetTargetInfo(sv._volume_uuid,req._userip,req._iscsiname, svwlaa);
            DeAuthWorkflowLabels deauwf;
            deauwf.request_id=req._request_id;
            deauwf.ip_address=req._userip;
            deauwf.iscsi_initiator=req._iscsiname;
            deauwf.target_name=targetname;

            vector<string> wf_plus,wf_minus,wf_rollback;
            FindWorkflow(wf_plus,STORAGE_CATEGORY,AUTH_VOLUME,deauwf.get_label_where());
            FindWorkflow(wf_minus,STORAGE_CATEGORY,DEAUTH_VOLUME,deauwf.get_label_where(),false);
            FindWorkflow(wf_rollback,STORAGE_CATEGORY,DEAUTH_VOLUME,deauwf.get_label_where(),true);
            if(!wf_plus.empty())
            {
                string r_id;
                GetRollbackWorkflow(wf_plus[0],r_id);
                RollbackWorkflow(wf_plus[0]);
                UnsetRollbackAttr(r_id,sender,req.action_id);
                w_ack.state  = ERROR_ACTION_RUNNING;
                w_ack.progress=GetWorkflowProgress(r_id);
            }
            else if(!wf_rollback.empty())
            {
                UnsetRollbackAttr(wf_rollback[0],sender,req.action_id);
                w_ack.state  = ERROR_ACTION_RUNNING;
                w_ack.progress=GetWorkflowProgress(wf_rollback[0]);

            }
            else if(!wf_minus.empty())
            {
                w_ack.state  = ERROR_ACTION_RUNNING;
                w_ack.progress=GetWorkflowProgress(wf_minus[0]);
            }
            else if (!tbl_exist)
            {
               OutPut(SYS_DEBUG, "EV_CLIENT_DEAUTH_BLOCK: tbl not exist\n");
               w_ack.state  = SUCCESS;
               w_ack.progress=100;
            }
            else if(tbl_exist)
            {
                wf_plus.clear();
                wf_minus.clear();
                FindWorkflow(wf_plus,STORAGE_CATEGORY,AUTH_VOLUME,deauwf.get_target_where());
                FindWorkflow(wf_minus,STORAGE_CATEGORY,DEAUTH_VOLUME,deauwf.get_target_where());
                if(!(wf_plus.empty()&&wf_minus.empty()))
                {
                    w_ack.state  = ERROR_ACTION_RUNNING;
                    w_ack.progress=0;
                    ScAckSend(message.option.id(), sender, rinfo, uid,w_ack);
                    break;
                }
                DbGetTargetIndexByKey(si._sid,svwlaa._target_name,targetindex);
                Authinfo info;
                info.lunid=svwlaa._lun_id;
                info.target_id=targetindex;
                info.target_name=svwlaa._target_name;
                int64  num_lun;//host 对应的卷数目
                int64  num_host;//lun对应的host数目
                DbGetLunHostMapNum(svwlaa._target_name,sv._volume_uuid,req._userip,req._iscsiname ,num_lun,num_host);
                if(num_lun>1 && num_host>1)
                {
                    StorageAuthorize sa;
                    sa._sid=si._sid;
                    sa._ip_address=req._userip;
                    sa._iscsi_initiator=req._iscsiname;
                    sa._target_name=svwlaa._target_name;
                    DbIsHostAuthorized(sa);
                    StorageLun sl;
                    sl._sid=si._sid;
                    sl._volume_uuid=si._vol_uuid;
                    sl._target_name=svwlaa._target_name;
                    DbIsStorageLunExist(sl);
                    StorageAuthorizeLun sal;
                    sal._auth_uuid=sa._auth_uuid;
                    sal._lun_uuid=sl._lun_uuid;
                    DbDelAuthorizeLun(sal);

                    w_ack.state  = SUCCESS;
                    w_ack.progress=100;
                    ScAckSend(message.option.id(), sender, rinfo, uid,w_ack);
                    break;
                }
                int64  num_target;/*映射组里面还有多少卷*/
                DbGetTargetMapNum(svwlaa._target_name,num_target);

                info.del_target=(num_target==1);
                info.del_host=(num_lun==1); //host对应的lun为1，表明此host可以删除了。
                info.del_vol=(num_host==1);
                info.sender =sender;
                DeAuthVol(si,req,info);
                w_ack.state  = ERROR_ACTION_RUNNING;
                w_ack.progress=0;

            }
            ScAckSend(message.option.id(), sender, rinfo, uid,w_ack);
            OutPut(SYS_DEBUG,"rcv EV_CLIENT_DEAUTH_BLOCK ret is %d \n",ret);

        }
        break;

        case EV_STORAGE_GET_CLUSTERS_REQ:
        {
            int ret;
            FilterClusterByShareStorageReq req;
            req.deserialize(message.data);
            FilterClusterByShareStorageAck ack(req.action_id);

            int64  size = (req._share_disksize % MBYTE ==0)?(req._share_disksize / MBYTE):(req._share_disksize / MBYTE + 1);
            ret=DbGetClustersBySize(req._appointed_cluster, size, ack._cluster_list);
            ack.state=ret;
            ack.progress=(ret==SUCCESS?100:0);
            ostringstream oss;
            oss<<"lack  of storage resource"<<endl;
            ack.detail=(ret==SUCCESS?"SUCCESS":oss.str());
            MessageOption option(sender, EV_STORAGE_GET_CLUSTERS_ACK, 0, 0);
            m->Send(ack, option);
            OutPut(SYS_DEBUG,"rcv EV_STORAGE_GET_CLUSTERS_REQ ret is %d \n",ret);

        }
        break;

        case EV_STORAGE_GET_BLOCKINFO_REQ:
        {
            GetBlockInfoReq req;
            req.deserialize(message.data);
            w_ack.state = SUCCESS;
            w_ack.progress=100;
            req_info      rinfo(req._request_id);
            ScAckSend(message.option.id(), sender, rinfo, uid,w_ack);

        }
        break;

        case EV_SA_OPERATE_ACK:
        {
            HandleSaAck(message);

        }
        break;

        default:
            break;
        }

    }
    break;
    default:
        break;
    }
}



STATUS StorageController::CreateVol(StorageVolumeWithAdaptor &si,CreateBlockReq &req,StAllocVolReq & sa_req,int64 volume_id,MID &sender, string & workflow_id)
{
    Action action1(STORAGE_CATEGORY,CREATE_VOLUME,EV_SA_CREATE_VOL,MID(m->name()),Siv2Mid(si));
    sa_req.action_id=action1.get_id();
    action1.message_req = sa_req.serialize();
    CreateVolLabels step_A_labels;
    step_A_labels.sid = si._sid;
    step_A_labels.size = sa_req._volsize;
    step_A_labels.vgname = sa_req._vgname;
    step_A_labels.name = sa_req._volname;
    step_A_labels.volume_id =volume_id ;
    step_A_labels.request_id = req._request_id;
    step_A_labels.uid = req._uid;
    step_A_labels.description = req._description;
    action1.labels = step_A_labels;

    Action action1_rollback(STORAGE_CATEGORY,DELETE_VOLUME,EV_SA_DELETE_VOL,MID(m->name()),Siv2Mid(si));
    StDelVolReq delvol;
    delvol.action_id=action1_rollback.get_id();
    delvol._vgname=sa_req._vgname;
    delvol._volname=sa_req._volname;
    action1_rollback.message_req = delvol.serialize();
    DelVolLabels step_o_labels;
    step_o_labels.sid = si._sid;
    step_o_labels.size = sa_req._volsize;
    step_o_labels.vgname = sa_req._vgname;
    step_o_labels.name = sa_req._volname;
    step_o_labels.volume_id =volume_id ;
    step_o_labels.request_id = req._request_id;

    action1_rollback.labels=step_o_labels;

    Workflow w_rollback(STORAGE_CATEGORY,DELETE_VOLUME);
    w_rollback.Add(&action1_rollback);
    DelVolWorkflowLabels  dv_labels;
    dv_labels.request_id=req._request_id;
    dv_labels.uid = req._uid;
    w_rollback.labels=dv_labels;
    w_rollback.upstream_sender=sender;
    w_rollback.upstream_action_id=req.action_id;

    Workflow w(STORAGE_CATEGORY,CREATE_VOLUME);

    if((req._uid != INVALID_UID)&&(!req._user.empty()))
    {
        w.originator = req._user;
    }
    w.Add(&action1);
    CreateVolWorkflowLabels cv_labels;
    cv_labels.request_id = req._request_id;
    cv_labels.uid = req._uid;
    cv_labels.cluster_name = req._cluster_name;
    cv_labels.vol_size=sa_req._volsize;
    w.labels = cv_labels;
    w.upstream_sender=sender;
    w.upstream_action_id=req.action_id;
    workflow_id = w.get_id();
    return CreateWorkflow(w,w_rollback);

}

STATUS StorageController::DeleteVol(StorageVolumeWithAdaptor &si,int req_msgid,DeleteBlockReq &req,StDelVolReq & sa_req,StorageVolume &sv,MID &sender, string & workflow_id)
{
    int  msgid;
    if(req_msgid==EV_STORAGE_DELETE_BLOCK_REQ)
    {
        msgid=EV_SA_DELETE_VOL;
    }
    else
    {
        msgid=EV_SA_DELETE_SNAPSHOT;
    }
    Action action1(STORAGE_CATEGORY,DELETE_VOLUME,msgid,MID(m->name()),Siv2Mid(si));
    sa_req.action_id=action1.get_id();
    action1.message_req = sa_req.serialize();
    DelVolLabels step_A_labels;
    step_A_labels.sid = si._sid;
    step_A_labels.size = sv._size;
    step_A_labels.vgname = sa_req._vgname;
    step_A_labels.name = sa_req._volname;
    step_A_labels.volume_id =sv._volume_id ;
    step_A_labels.request_id = sv._request_id;
    step_A_labels.uid = req._uid;
    action1.labels = step_A_labels;

    Workflow w(STORAGE_CATEGORY,DELETE_VOLUME);
    if((req_msgid==EV_STORAGE_DELETE_BLOCK_REQ)&&(req._uid != INVALID_UID)&&(!req._user.empty()))
    {
        w.originator = req._user;
    }
    w.Add(&action1);
    DelVolWorkflowLabels dv_labels;
    dv_labels.request_id = sv._request_id;
    dv_labels.uid = req._uid;
    w.labels = dv_labels;
    w.upstream_sender=sender;
    w.upstream_action_id=req.action_id;
    workflow_id = w.get_id();
    return CreateWorkflow(w);
}

STATUS StorageController::CreateSnapshot(StorageVolumeWithAdaptor &si,CreateSnapshotReq &req,StAllocSnapshotReq & sa_req,int64 volume_id,string &base_uuid, MID &sender)
{
    Action action1(STORAGE_CATEGORY,CREATE_VOLUME,EV_SA_CREATE_SNAPSHOT,MID(m->name()),Siv2Mid(si));
    sa_req.action_id=action1.get_id();
    action1.message_req = sa_req.serialize();
    CreateVolLabels step_A_labels;
    step_A_labels.sid = si._sid;
    step_A_labels.size = sa_req._volsize;
    step_A_labels.vgname = sa_req._vgname;
    step_A_labels.name = sa_req._volname;
    step_A_labels.volume_id =volume_id ;
    step_A_labels.request_id = req._request_id;
    action1.labels = step_A_labels;

    Action action1_rollback(STORAGE_CATEGORY,DELETE_VOLUME,EV_SA_DELETE_SNAPSHOT,MID(m->name()),Siv2Mid(si));
    StDelVolReq delvol;
    delvol.action_id=action1_rollback.get_id();
    delvol._vgname=sa_req._vgname;
    delvol._volname=sa_req._volname;
    action1_rollback.message_req = delvol.serialize();
    DelVolLabels step_o_labels;
    step_o_labels.request_id=req._request_id;
    action1_rollback.labels=step_o_labels;

    Workflow w_rollback(STORAGE_CATEGORY,DELETE_VOLUME);
    w_rollback.Add(&action1_rollback);
    DelVolWorkflowLabels  dv_labels;
    dv_labels.request_id=req._request_id;
    w_rollback.labels=dv_labels;
    w_rollback.upstream_sender=sender;
    w_rollback.upstream_action_id=req.action_id;

    Workflow w(STORAGE_CATEGORY,CREATE_VOLUME);
    w.Add(&action1);
    CreateVolWorkflowLabels cv_labels;
    cv_labels.request_id = req._request_id;
    cv_labels.base_id=base_uuid;
    cv_labels.vol_size=sa_req._volsize;
    w.labels = cv_labels;
    w.upstream_sender=sender;
    w.upstream_action_id=req.action_id;
    return CreateWorkflow(w,w_rollback);

}

STATUS StorageController::AuthVol(StorageVolumeWithAdaptor &si,AuthBlockReq &req, Authinfo &info)
{

    Action action1(STORAGE_CATEGORY,CREATE_TARGET,EV_SA_CREATE_TARGET,MID(m->name()),Siv2Mid(si));
    StCreateTargetReq create_req;
    create_req.action_id=action1.get_id();
    create_req._targetname=info.target_name;
    create_req._targetindex=info.target_id;
    action1.message_req = create_req.serialize();
    CreateTargetLabels step_a_labels;
    step_a_labels.sid = si._sid;
    step_a_labels.target_id=info.target_id;
    step_a_labels.target_name=info.target_name;
    action1.labels = step_a_labels;
    action1.skipped=info.create_target_skiped;

    Action action2(STORAGE_CATEGORY,ADD_VOLUME_TO_TARGET,EV_SA_ADDVOLTO_TARGET,MID(m->name()),Siv2Mid(si));
    StAddToTarReq add_req(action2.get_id());
    add_req._lunid=info.lunid;
    add_req._targetindex=info.target_id;
    add_req._targetname=info.target_name;
    add_req._vgname=si._vg_name;
    add_req._volname=si._vol_name;
    add_req._ip=req._userip;
    add_req._iscsiname=req._iscsiname;
    action2.message_req = add_req.serialize();
    AddVol2TargetLabels step_b_labels;
    step_b_labels.sid = si._sid;
    step_b_labels.lunid=info.lunid;
    step_b_labels.target_name=info.target_name;
    step_b_labels.volume_uuid=si._vol_uuid;
    action2.labels = step_b_labels;
    action2.skipped=info.add_vol_skiped;

    Action action3(STORAGE_CATEGORY,ADD_HOST_TO_TARGET,EV_SA_ADDHOSTTO_TARGET,MID(m->name()),Siv2Mid(si));
    add_req.action_id=action3.get_id();
    action3.message_req = add_req.serialize();
    AddVol2TargetLabels step_c_labels;
    step_c_labels.sid = si._sid;
    step_c_labels.lunid=info.lunid;
    step_c_labels.target_name=info.target_name;
    step_c_labels.volume_uuid=si._vol_uuid;
    action3.labels = step_c_labels;
    action3.skipped=info.add_host_skiped;

    Action action3_rollback(STORAGE_CATEGORY,DELETE_HOST_FROM_TARGET,EV_SA_DELHOSTFROM_TARGET,MID(m->name()),Siv2Mid(si));
    StDelFromTarReq remove_req(action3_rollback.get_id());
    remove_req._sid=si._sid;
    remove_req._targetindex=info.target_id;
    remove_req._targetname=info.target_name;
    remove_req._lunid=info.lunid;
    remove_req._ip=req._userip;
    remove_req._iscsiname=req._iscsiname;
    action3_rollback.message_req = remove_req.serialize();
    action3_rollback.skipped=info.add_host_skiped;

    DelHost4TargetLabels step_o_labels;
    step_o_labels.sid=si._sid;
    step_o_labels.target_name=info.target_name;
    step_o_labels.ip_address=req._userip;
    step_o_labels.iscsi_initiator=req._iscsiname;
    action3_rollback.labels=step_o_labels;

    Action action2_rollback(STORAGE_CATEGORY,DELETE_VOLUME_FROM_TARGET,EV_SA_DELVOLFROM_TARGET,MID(m->name()),Siv2Mid(si));
    remove_req.action_id=action2_rollback.get_id();
    action2_rollback.message_req = remove_req.serialize();
    DelVol4TargetLabels step_p_labels;
    step_p_labels.sid=si._sid;
    step_p_labels.target_name=info.target_name;
    step_p_labels.lunid=info.lunid;
    step_p_labels.volume_uuid=si._vol_uuid;
    action2_rollback.labels=step_p_labels;
    action2_rollback.skipped=info.add_vol_skiped;

    Action action1_rollback(STORAGE_CATEGORY,DELETE_TARGET,EV_SA_DELETE_TARGET,MID(m->name()),Siv2Mid(si));
    StDelTargetReq del_req(action1_rollback.get_id());
    del_req._sid=si._sid;
    del_req._targetindex=info.target_id;
    del_req._targetname=info.target_name;
    action1_rollback.message_req = del_req.serialize();
    DelTargetLabels step_q_labels;
    step_q_labels.sid=si._sid;
    step_q_labels.target_id=info.target_id;
    step_q_labels.target_name=info.target_name;
    action1_rollback.labels=step_q_labels;
    action1_rollback.skipped=info.create_target_skiped;

    Workflow w_rollback(STORAGE_CATEGORY,DEAUTH_VOLUME);
    w_rollback.Add(&action3_rollback,&action2_rollback);
    w_rollback.Add(&action2_rollback,&action1_rollback);
    DeAuthWorkflowLabels deau_labels;
    deau_labels.ip_address=req._userip;
    deau_labels.iscsi_initiator=req._iscsiname;
    deau_labels.request_id=req._request_id;
    deau_labels.target_name=info.target_name;
    w_rollback.labels = deau_labels;
    w_rollback.upstream_sender=info.sender;
    w_rollback.upstream_action_id=req.action_id;

    Workflow w(STORAGE_CATEGORY,AUTH_VOLUME);
    w.Add(&action1,&action2);
    w.Add(&action2,&action3);
    AuthWorkflowLabels au_labels;
    au_labels.ip_address=req._userip;
    au_labels.iscsi_initiator=req._iscsiname;
    au_labels.request_id=req._request_id;
    au_labels.target_name=info.target_name;
    au_labels.usage = req._usage;
    w.labels = au_labels;
    w.upstream_sender=info.sender;
    w.upstream_action_id=req.action_id;
    return CreateWorkflow(w,w_rollback);

}

STATUS StorageController::DeAuthVol(StorageVolumeWithAdaptor &si,DeAuthBlockReq &req, Authinfo &info)
{
    Action action1(STORAGE_CATEGORY,DELETE_HOST_FROM_TARGET,EV_SA_DELHOSTFROM_TARGET,MID(m->name()),Siv2Mid(si));
    StDelFromTarReq remove_req(action1.get_id());
    remove_req._sid=si._sid;
    remove_req._targetindex=info.target_id;
    remove_req._targetname=info.target_name;
    remove_req._lunid=info.lunid;
    remove_req._ip=req._userip;
    remove_req._iscsiname=req._iscsiname;
    action1.message_req = remove_req.serialize();
    action1.skipped=!info.del_host;

    DelHost4TargetLabels step_a_labels;
    step_a_labels.sid=si._sid;
    step_a_labels.target_name=info.target_name;
    step_a_labels.ip_address=req._userip;
    step_a_labels.iscsi_initiator=req._iscsiname;
    action1.labels=step_a_labels;

    Action action2(STORAGE_CATEGORY,DELETE_VOLUME_FROM_TARGET,EV_SA_DELVOLFROM_TARGET,MID(m->name()),Siv2Mid(si));
    remove_req.action_id=action2.get_id();
    action2.message_req = remove_req.serialize();
    DelVol4TargetLabels step_b_labels;
    step_b_labels.sid=si._sid;
    step_b_labels.target_name=info.target_name;
    step_b_labels.lunid=info.lunid;
    step_b_labels.volume_uuid=si._vol_uuid;
    action2.labels=step_b_labels;
    action2.skipped=!info.del_vol;

    Action action3(STORAGE_CATEGORY,DELETE_TARGET,EV_SA_DELETE_TARGET,MID(m->name()),Siv2Mid(si));
    StDelTargetReq del_req(action3.get_id());
    del_req._sid=si._sid;
    del_req._targetindex=info.target_id;
    del_req._targetname=info.target_name;
    action3.message_req = del_req.serialize();
    DelTargetLabels step_c_labels;
    step_c_labels.sid=si._sid;
    step_c_labels.target_id=info.target_id;
    step_c_labels.target_name=info.target_name;
    action3.labels=step_c_labels;
    action3.skipped=!info.del_target;

    Workflow w(STORAGE_CATEGORY,DEAUTH_VOLUME);
    w.Add(&action1,&action2);
    w.Add(&action2,&action3);
    DeAuthWorkflowLabels deau_labels;
    deau_labels.ip_address=req._userip;
    deau_labels.iscsi_initiator=req._iscsiname;
    deau_labels.request_id=req._request_id;
    deau_labels.target_name=info.target_name;
    w.labels = deau_labels;
    w.upstream_sender=info.sender;
    w.upstream_action_id=req.action_id;
    return CreateWorkflow(w);
}


STATUS StorageController::HandleCreateVolAck(const StStorageSvrAck &sa_ack,MID &sender,string &action_id,req_info & rinfo ,int64 &uid)
{
    StorageVolume sv;
    StorageUserVolume suv;
    int           ret=SUCCESS;
    CreateVolWorkflowLabels wf_labels;
    WorkAck  w_ack;
    ret = GetWorkflowLabels(sa_ack.action_id,wf_labels);
    if(SUCCESS != ret)
    {
        SkyAssert(false);
    }
    rinfo.request_id=wf_labels.request_id;
    uid = wf_labels.uid;

    CreateVolLabels labels;
    ret = GetActionLabels(sa_ack.action_id,CREATE_VOLUME,labels);
    if(SUCCESS != ret)
    {
        SkyAssert(false);
    }
    GetUpstreamSender(sa_ack.action_id,sender);
    GetUpstreamActionId(sa_ack.action_id,action_id);

    sv._name=labels.name;
    sv._request_id=labels.request_id;
    sv._sid=labels.sid;
    sv._size=labels.size;
    sv._vg_name=labels.vgname;
    sv._volume_uuid=sa_ack.vol_uuid;
    sv._volume_id=labels.volume_id;

    StorageVolumeWithLunAndAuthAndAdaptor svwlaa;
    ret=DbGetTargetInfo(sv._volume_uuid,rinfo.ip,rinfo.iscsi_name,svwlaa);
    if(ret==SUCCESS)
    {
        if (svwlaa._type==LOCALSAN)
        {
            if (0 != sv._size%LVM_SIZE)
            {
                sv._size = ((sv._size/LVM_SIZE) + 1)*LVM_SIZE;
            }
        }
    }

    suv._uid = labels.uid;
    suv._description = labels.description;
    suv._request_id=labels.request_id;

    Transaction::Begin();
    ret =DbAddVolumeAndUserVolume(sv,suv);
    if(SUCCESS != ret)
    {
        Transaction::Cancel();
        SkyAssert(false);
        return ret;
    }
    if(OP_CREATE_SVOL==sa_ack.last_op)
    {
        StorageSnapshot  shot(wf_labels.base_id,labels.request_id);
        DbAddSnapshot(shot);
    }
    FinishAction(sa_ack.action_id);
    ret = Transaction::Commit();

    return ret;

}

STATUS StorageController::HandleDelVolAck(const StStorageSvrAck &sa_ack,MID &sender,string &action_id,req_info & rinfo,int64 &uid,bool &isRollBack)
{
    StorageVolume sv;
    StorageUserVolume suv;
    int           ret=SUCCESS;
    WorkAck   w_ack;
    isRollBack=false;
    DelVolWorkflowLabels wf_labels;
    ret = GetWorkflowLabels(sa_ack.action_id,wf_labels);
    if(SUCCESS != ret)
    {
        SkyAssert(false);
    }
    rinfo.request_id=wf_labels.request_id;
    uid = wf_labels.uid;

    DelVolLabels labels;
    ret = GetActionLabels(sa_ack.action_id,DELETE_VOLUME,labels);
    if(SUCCESS != ret)
    {
        SkyAssert(false);
    }

    GetUpstreamSender(sa_ack.action_id,sender);
    GetUpstreamActionId(sa_ack.action_id,action_id);
     if(IsRollbackWorkflow(sa_ack.action_id))
    {
        FinishAction(sa_ack.action_id);
        isRollBack=true;
        return ERROR;
    }

    ret = DbGetStorageVolumeByRequestId(labels.request_id, sv);
    if (ERROR_OBJECT_NOT_EXIST == ret)
    {
        //这边跑不到
        FinishAction(sa_ack.action_id);
        return SUCCESS;

    }
    else if (SUCCESS != ret)
    {
        return ret;
    }

    suv._uid = labels.uid;
    suv._request_id=labels.request_id;

    Transaction::Begin();
    ret = DbDelVolumeAndUserVolume(sv,suv);
    if(SUCCESS !=ret)
    {
        Transaction::Cancel();
        SkyAssert(false);
        return ret;
    }

    if(OP_DEL_SVOL==sa_ack.last_op)
    {
        StorageSnapshot shot;
        shot._snap_uuid=sv._request_id;
        DbDropSnapshot(shot);
    }
    FinishAction(sa_ack.action_id);
    ret = Transaction::Commit();

    return ret;
}

STATUS StorageController::HandleAuthAck(const StStorageSvrAck &sa_ack,bool &finish,MID& sender,string &action_id,req_info & rinfo)
{
    int           ret=SUCCESS;
    AuthWorkflowLabels wf_labels;
    ret = GetWorkflowLabels(sa_ack.action_id,wf_labels);
    if(SUCCESS != ret)
    {
        SkyAssert(false);
    }

    CreateTargetLabels ct_labels;
    ret = GetActionLabels(sa_ack.action_id,CREATE_TARGET,ct_labels);
    if(SUCCESS != ret)
    {
        SkyAssert(false);
    }

    AddVol2TargetLabels av_labels;
    ret = GetActionLabels(sa_ack.action_id,ADD_VOLUME_TO_TARGET,av_labels);
    if(SUCCESS != ret)
    {
        SkyAssert(false);
    }
    GetUpstreamSender(sa_ack.action_id,sender);
    GetUpstreamActionId(sa_ack.action_id,action_id);
    vector<string> actions;
    GetNextActions(sa_ack.action_id,actions);
    while(!actions.empty())
    {
        Action  action;
        string  action_id=actions[0];
        GetActionById(action_id, action);
        if(!action.skipped)
        {
            finish=false;
            FinishAction(sa_ack.action_id);
            return SUCCESS;
        }
        actions.clear();
        GetNextActions(action_id,actions);
    }
    finish=true;
     StorageTarget st;
    st._sid = ct_labels.sid;
    st._name = ct_labels.target_name;
    st._target_id=ct_labels.target_id;

    StorageAuthorize sa;
    sa._ip_address=wf_labels.ip_address;
    sa._iscsi_initiator=wf_labels.iscsi_initiator;
    sa._sid=ct_labels.sid;
    sa._target_name=ct_labels.target_name;

    StorageLun sl;
    sl._lun_id=av_labels.lunid;
    sl._sid=av_labels.sid;
    sl._target_name=av_labels.target_name;
    sl._volume_uuid=av_labels.volume_uuid;

    Transaction::Begin();
    DbUpdateUsageOfUserVolume(wf_labels.request_id, wf_labels.usage);
    DbAddAuthTbl(st,sa,sl);
    FinishAction(sa_ack.action_id);
    ret = Transaction::Commit();
    rinfo.request_id=wf_labels.request_id;
    rinfo.iscsi_name=wf_labels.iscsi_initiator;
    rinfo.ip=wf_labels.ip_address;
    return ret;

}

STATUS StorageController::HandleDeauthAck(const StStorageSvrAck &sa_ack,bool &finish,MID &sender,string &action_id,req_info & rinfo,bool &isRollBack)
{
    StorageVolume sv;
    int           ret=SUCCESS;
    DeAuthWorkflowLabels wf_labels;
    isRollBack=false;
    ret = GetWorkflowLabels(sa_ack.action_id,wf_labels);
    if(SUCCESS != ret)
    {
        SkyAssert(false);
    }

    DelTargetLabels dt_labels;
    ret = GetActionLabels(sa_ack.action_id,DELETE_TARGET,dt_labels);
    if(SUCCESS != ret)
    {
        SkyAssert(false);
    }

    DelVol4TargetLabels dv_labels;
    ret = GetActionLabels(sa_ack.action_id,DELETE_VOLUME_FROM_TARGET,dv_labels);
    if(SUCCESS != ret)
    {
        SkyAssert(false);
    }
    GetUpstreamSender(sa_ack.action_id,sender);
    GetUpstreamActionId(sa_ack.action_id,action_id);
    vector<string> actions;
    GetNextActions(sa_ack.action_id,actions);
    vector<string>::iterator it;
    for(it=actions.begin();it!=actions.end();it++)
    {
        Action  action;
        GetActionById(*it, action);
        if(!action.skipped)
        {
            finish=false;
            FinishAction(sa_ack.action_id);
            return SUCCESS;
        }
    }
    finish=true;
    if(IsRollbackWorkflow(sa_ack.action_id))
    {
        isRollBack=true;
        FinishAction(sa_ack.action_id);
        return ERROR;
    }
    actions.clear();
    GetFinishedActions(sa_ack.action_id,actions);
    int  type=0;
    actions.push_back(sa_ack.action_id);
    for(it=actions.begin();it!=actions.end();it++)
    {
        Action  action;
        GetActionById(*it, action);
        if(action.skipped)
        {
            continue;
        }
        if(action.name==DELETE_HOST_FROM_TARGET)
        {
            type|=DEL_HOST_TBL;
        }
        else if(action.name==DELETE_VOLUME_FROM_TARGET)
        {
            type|=DEL_LUN_TBL;
        }
        else if(action.name==DELETE_TARGET)
        {
            type|=DEL_TARGET_TBL;
        }
    }

    StorageTarget st;
    st._sid = dt_labels.sid;
    st._name = dt_labels.target_name;
    st._target_id=dt_labels.target_id;

    StorageAuthorize sa;
    sa._ip_address=wf_labels.ip_address;
    sa._iscsi_initiator=wf_labels.iscsi_initiator;
    sa._sid=dt_labels.sid;
    sa._target_name=dt_labels.target_name;

    StorageLun sl;
    sl._lun_id=dv_labels.lunid;
    sl._sid=dv_labels.sid;
    sl._target_name=dv_labels.target_name;
    sl._volume_uuid=dv_labels.volume_uuid;

    Transaction::Begin();
    DbDelAuthTbl(st,sa,sl,type);
    DbUpdateUsageOfUserVolume(wf_labels.request_id,dv_labels.volume_uuid, "");
    FinishAction(sa_ack.action_id);
    ret = Transaction::Commit();
    rinfo.request_id=wf_labels.request_id;
    return ret;
}




STATUS StorageController::HandleSaAck(const MessageFrame &message)
{
    int  ret;
    StStorageSvrAck sa_ack ;
    sa_ack.deserialize(message.data);
    WorkAck w_ack;
    uint32  msgid;
    string  request_id;
    int64   uid = 0;
    string  action_id;
    bool need_ack=true;
    MID  sender;
    bool isRollBack;
    req_info      rinfo;

    if(true == IsActionFinished(sa_ack.action_id))
    {
        //有可能收到已经过时的Action，这里一定要先过滤一遍
        return SUCCESS;
    }

    Action action;
    if(SUCCESS != GetActionById(sa_ack.action_id,action))
    {
        return ERROR;
    }

    OutPut(SYS_WARNING,"HandleSaAck receives action %s (%s) feedback!\n",
        sa_ack.action_id.c_str(),action.name.c_str());

    ret=sa_ack.state;

    switch(sa_ack.last_op)
    {
    case OP_CREATE_VOL:
    case OP_CREATE_SVOL:
        if(SUCCESS == sa_ack.state)
        {
            ret=HandleCreateVolAck(sa_ack,sender,action_id,rinfo,uid);
            if(sa_ack.last_op==OP_CREATE_VOL)
            {
                msgid=EV_STORAGE_CREATE_BLOCK_REQ;
            }
            else
            {
                msgid=EV_STORAGE_CREATE_SNAPSHOT_REQ;
            }
        }
        else
        {
             OutPut(SYS_ERROR,"action %s failed! begin rollback\n",sa_ack.action_id.c_str());
             string w_id;
             GetWorkflowByActionId(sa_ack.action_id,w_id);
             RollbackWorkflow(sa_ack.action_id);
             UpdateWorkflowHistory(w_id,sa_ack.detail);
             need_ack=false;
        }

    break;

    case OP_DEL_VOL:
    case OP_DEL_SVOL:
        if(SUCCESS == sa_ack.state)
        {
            ret=HandleDelVolAck(sa_ack,sender,action_id,rinfo,uid,isRollBack);
            if(sa_ack.last_op==OP_DEL_VOL)
            {
                if(isRollBack)
                {
                    msgid=EV_STORAGE_CREATE_BLOCK_REQ;
                    w_ack.detail = "create block fail.";
                }
                else
                {
                    msgid=EV_STORAGE_DELETE_BLOCK_REQ;
                }
            }
            else
            {
                if(isRollBack)
                {
                    msgid=EV_STORAGE_CREATE_SNAPSHOT_REQ;
                    w_ack.detail = "create snapshot fail.";
                }
                else
                {
                    msgid=EV_STORAGE_DELETE_SNAPSHOT_REQ;
                }
            }
        }
        else
        {
            RestartAction(sa_ack.action_id);
            need_ack=false;
        }

    break;

    case OP_CREATE_TARGET:
    case OP_ADD_VOL2TARGET:
    case OP_ADD_HOST2TARGET:
       if(SUCCESS != sa_ack.state)
       {
            OutPut(SYS_ERROR,"action %s failed! begin rollback\n",sa_ack.action_id.c_str());
            string w_id;
            GetWorkflowByActionId(sa_ack.action_id,w_id);
            RollbackWorkflow(sa_ack.action_id);
            UpdateWorkflowHistory(w_id,sa_ack.detail);
            need_ack=false;
       }
       else
       {
            bool finish;
            ret=HandleAuthAck(sa_ack,finish,sender,action_id,rinfo);
            msgid=EV_STORAGE_AUTH_BLOCK_REQ;
            if(!finish)
            {
                need_ack=false;
            }

       }
    break;

    case OP_DEL_VOLFROMTARGET:
    case OP_DEL_HOSTFROMTARGET:
    case OP_DEL_TARGET:
       if(SUCCESS != sa_ack.state)
       {
            OutPut(SYS_ERROR,"action %s failed!  \n",sa_ack.action_id.c_str());
            RestartAction(sa_ack.action_id);
            need_ack=false;
       }
       else
       {
            bool finish;
            ret=HandleDeauthAck(sa_ack,finish,sender,action_id,rinfo,isRollBack);
            if(isRollBack)
            {
                msgid=EV_STORAGE_AUTH_BLOCK_REQ;
            }
            else
            {
                msgid=EV_STORAGE_DEAUTH_BLOCK_REQ;
            }
            if(!finish)
            {
                need_ack=false;
            }
       }
    break;

    default:
        need_ack=false;
        break;
    }

    if(need_ack)
    {
        w_ack.state=ret;
        w_ack.progress=(ret==SUCCESS?100:0);
        w_ack.action_id=action_id;
        ScAckSend(msgid, sender,rinfo, uid,w_ack);
        OutPut(SYS_DEBUG, "HandleSaAck: lastop :%d ,request_id :%s ,uid is %d, ret is %d\n",sa_ack.last_op,request_id.c_str() ,uid,ret);
    }
    return ret;
}

void StorageController::GetCreateVolWorkFlowNum(int64 sid,const string &vgname,int32 &workflownum)
{
    string workflow_id;
    vector<string> wf_plus;
    vector<string>::iterator it;
    CreateVolLabels labels;
    STATUS ret = ERROR;
    FindWorkflow(wf_plus,STORAGE_CATEGORY,CREATE_VOLUME,"1 = 1");

    if(!wf_plus.empty())
    {
        for (it = wf_plus.begin(); it != wf_plus.end(); it++)
        {
            workflow_id = *it;
            ret = GetActionLabels(workflow_id,CREATE_VOLUME,labels);
            if(SUCCESS == ret)
            {
                if((sid == labels.sid)&&(vgname==labels.vgname))
                {
                    workflownum ++;
                }
            }
        }
    }
}


STATUS StorageController::ScheduletoAdaptor(const string &clustername, int64 volsize, StorageVolumeWithAdaptor &si)
{
    int ret;
    int32 workflownum;
    vector<StorageAdaptorWithMultivgAndCluster>  vec_sawvac;
    ret=DbSelectStorageAdaptor(clustername,volsize , vec_sawvac);
    if(ret!=SUCCESS)
        return ret;
    vector<StorageAdaptorWithMultivgAndCluster>::iterator it,itAdaptor;
    int64 maxsa=0;
    int64 maxvg = 0;
    int64 sid = INVALID_SID;
    bool  found=false;

    /*目前采用简单的负载均衡算法*/
    for (it = vec_sawvac.begin(); it != vec_sawvac.end(); it++)
    {
        if(it->_type==IPSAN)
        {
            int volnum=100;//数据库访问失败默认不访问此vg,初始值大于等于32即可
            DbGetVolNumOfVg(it->_sid,it->_vg_name,volnum);
            if(volnum >= IPSAN_VG_MAX_VOLUME) //IPSAN 一个vg只能建立不超过32个卷
            {
                OutPut(SYS_NOTICE, "ScheduletoAdaptor:IpSan: sid %ld,vg_name %s,volnum %d continue .\n",
                          it->_sid,
                          it->_vg_name.c_str(),
                          volnum);
                continue;
            }
            else
            {
                workflownum = 0;
                GetCreateVolWorkFlowNum(it->_sid,it->_vg_name,workflownum);
                if((volnum + workflownum)>= IPSAN_VG_MAX_VOLUME) //workflownum为在创建中的个数
                {
                    OutPut(SYS_NOTICE, "ScheduletoAdaptor:IpSan: sid %ld,vg_name %s,volnum %d ,workflownum %d .\n",
                              it->_sid,
                              it->_vg_name.c_str(),
                              volnum,
                              workflownum);

                    continue;
                }
            }
        }

        //选择maxsa，负载均衡，再从中选择maxvg.
        if(it->_sa_total_free_size>=maxsa)
        {

            if(sid != it->_sid)//查到新的max sa
            {
                if(it->_sa_total_free_size > maxsa)
                {
                    maxsa=it->_sa_total_free_size ;
                    sid = it->_sid;
                    maxvg = it->_vg_free_size;
                    itAdaptor=it;
                    found=true;
                }
                else
                {
                    if(it->_vg_free_size>maxvg)
                    {
                        maxsa=it->_sa_total_free_size ;
                        sid = it->_sid;
                        maxvg = it->_vg_free_size;
                        itAdaptor=it;
                        found=true;
                    }

                }

            }
            else
            {
                if(it->_vg_free_size>maxvg)
                {
                    maxvg = it->_vg_free_size;
                    itAdaptor=it;
                    found=true;
                }

            }

        }
    }
    if(found)
    {
        si._sid = itAdaptor->_sid;
        si._vg_name  = itAdaptor->_vg_name;
        si._application = itAdaptor->_application;
        si._unit = itAdaptor->_unit;
        si._type = itAdaptor->_type;
        return SUCCESS;
    }
    else
    {
        return ERROR_RESOURCE_UNAVAILABLE;
    }

}

void DbgCreateBlock(int64 uid,const char* cluster, int64 size, const char* description)
{
    if (INVALID_IID == uid)
    {
        cout << "Invalid vid " << uid <<endl;
        return;
    }

    if (NULL == cluster || NULL == description || 0 == size)
    {
        cout << "Invalid para!" <<endl;
        return;
    }

    MessageUnit    temp_mu(TempUnitName("storage_create_block_dbg"));
    temp_mu.Register();
    string  reg_system ;
    GetRegisteredSystem(reg_system);
    MID  dest_mid(reg_system, PROC_CC, MU_SC_CONTROL);
    MessageOption  req_option(dest_mid, EV_STORAGE_CREATE_BLOCK_REQ, 0, 0);

    CreateBlockReq req;
    req.action_id = GenerateUUID();
    req._uid = uid;
    req._request_id = GenerateUUID();
    req._cluster_name = cluster;
    req._volsize = size;
    req._description = description;

    STATUS ret = temp_mu.Send(req, req_option);
    if (SUCCESS != ret)
    {
        cerr << "Fail to send create request! ret = " << ret << endl;
    }
    else
    {
        cout << "Create request send success,action_id:" <<req.action_id<<" request_id:"<<req._request_id<< endl;
    }

    MessageFrame message;
    if(SUCCESS == temp_mu.Wait(&message,3000))
    {
        CreateBlockAck  ack;
        ack.deserialize(message.data);
        cout << " CreateBlockAck: "
             << " uid: " << ack._uid
             << " request_id:" << ack._request_id
             << " action_id: " << ack.action_id
             << " state:" << ack.state
             << " progress:" << ack.progress << endl;
    }
    else
    {
        cout<<"Wait ack timeout"<<endl;
    }
}
DEFINE_DEBUG_FUNC(DbgCreateBlock);

void DbgDeleteBlock(int64 uid,const char* request_id,bool snap)
{

    if (NULL == request_id)
    {
        cout << "Invalid para!" <<endl;
        return;
    }

    MessageUnit    temp_mu(TempUnitName("storage_delete_block_dbg"));
    temp_mu.Register();
    string  reg_system ;
    GetRegisteredSystem(reg_system);
    MID  dest_mid(reg_system, PROC_CC, MU_SC_CONTROL);
    MessageOption  req_option(dest_mid, EV_STORAGE_DELETE_BLOCK_REQ, 0, 0);
    if(snap)
    {
        req_option.setId(EV_STORAGE_DELETE_SNAPSHOT_REQ);
    }


    DeleteBlockReq req;
    req.action_id = GenerateUUID();
    req._uid = uid;
    req._request_id = request_id;

    STATUS ret = temp_mu.Send(req, req_option);
    if (SUCCESS != ret)
    {
        cerr << "Fail to send delete request! ret = " << ret << endl;
    }
    else
    {
        cout << "Delete request send success,action_id:" <<req.action_id<<" request_id:"<<req._request_id<< endl;
    }

    MessageFrame message;
    if(SUCCESS == temp_mu.Wait(&message,3000))
    {
        DeleteBlockAck  ack;
        ack.deserialize(message.data);
        cout << " DeleteBlockAck: "
             << " uid: " << ack._uid
             << " request_id:" << ack._request_id
             << " action_id: " << ack.action_id
             << " state:" << ack.state
             << " progress:" << ack.progress << endl;
    }
    else
    {
        cout<<"Wait ack timeout"<<endl;
    }
}
DEFINE_DEBUG_FUNC(DbgDeleteBlock);

void DbgCreateSnapBlock(const char *base1,const char *base2, int64 size)
{

    if (NULL==base1 || NULL==base2 || 0 == size)
    {
        cout << "Invalid para!" <<endl;
        return;
    }

    MessageUnit    temp_mu(TempUnitName("storage_create_block_dbg"));
    temp_mu.Register();
    string  reg_system ;
    GetRegisteredSystem(reg_system);
    MID  dest_mid(reg_system, PROC_SC, MU_SC_CONTROL);
    MessageOption  req_option(dest_mid, EV_STORAGE_CREATE_SNAPSHOT_REQ, 0, 0);

    CreateSnapshotReq req;
    req.action_id = GenerateUUID();
    req._request_id = GenerateUUID();
    req._volsize = size;
    req._vec_baseid.push_back(base1);
    req._vec_baseid.push_back(base2);

    STATUS ret = temp_mu.Send(req, req_option);
    if (SUCCESS != ret)
    {
        cerr << "Fail to send create request! ret = " << ret << endl;
    }
    else
    {
        cout << "Create request send success,action_id:" <<req.action_id<<" request_id:"<<req._request_id<< endl;
    }

    MessageFrame message;
    if(SUCCESS == temp_mu.Wait(&message,3000))
    {
        CreateSnapshotAck  ack;
        ack.deserialize(message.data);
        cout << " CreateSnapshotAck: "
             << " uid: " << ack._uid
             << " request_id:" << ack._request_id
             << " action_id: " << ack.action_id
             << " state:" << ack.state
             << " progress:" << ack.progress << endl;
    }
    else
    {
        cout<<"Wait ack timeout"<<endl;
    }
}
DEFINE_DEBUG_FUNC(DbgCreateSnapBlock);


void DbgGetCreateVolWorkFlowNum(int64 sid,const char *vgname)
{
    int32 workflownum = 0;
    vector<string> workflows;
    vector<string>::iterator it;
    CreateVolLabels labels;
    STATUS ret = ERROR;
    if(NULL == vgname)
    {
        return;
    }

    ret = FindWorkflow(workflows,STORAGE_CATEGORY,CREATE_VOLUME,"1 = 1");
    if(SUCCESS != ret)
    {
        return;
    }

    if(!workflows.empty())
    {
        string workflow_id;
        for (it = workflows.begin(); it != workflows.end(); it++)
        {
            workflow_id = *it;

            ret = GetActionLabels(workflow_id,CREATE_VOLUME,labels);
            cout << "workflow_id = " << workflow_id<<",sid = " << labels.sid<<",vgname = " << labels.vgname.c_str()<< endl;
            if(SUCCESS == ret)
            {
                if((sid == labels.sid)&&(vgname==labels.vgname))
                {
                    workflownum ++;
                }
            }
        }
    }
    cout<<"workflownum = "<< workflownum << endl;
}
DEFINE_DEBUG_FUNC(DbgGetCreateVolWorkFlowNum);


}

