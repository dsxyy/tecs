
#include "sa_manager.h"
#include "log_agent.h"
#include "mid.h"
#include "event.h"
#include "db_config.h"
#include "sc_db_messages.h"
#include "storage_messages.h"
#include "msg_sa_manager_with_api_method.h"
#include "alarm_report.h"
#include "alarm_agent.h"
#include "pub_alarm.h"

namespace zte_tecs
{

#define EV_SA_KEEP_STATE_TIMER          EV_TIMER_1     /* sa状态刷新定时器 */

SaManager *SaManager::_instance = NULL;

void FillSdInfo(MID sender,MessageSaInfoReport &Msg,
                StorageAdaptor &sa,StorageAdaptorAddress &saa,
                vector<StorageAddress> &vec_sadd,vector<StorageVg> &vec_sv)
{
    StorageAddress sadd;
    StorageVg sv;

    saa._application = sender._application;
    saa._ip_address = Msg._sa_ip;
    sa._name = sender._unit;
    sa._application = saa._application;
    sa._ctrl_address = Msg._control_ip;
    sa._type = Msg._type;    
    sa._description = Msg._description;
    sa._refresh_time.refresh();
    for (vector<string>::iterator it = Msg._dataip.begin(); it != Msg._dataip.end(); it++)
    {
        sadd._sid = sa._sid;
        sadd._media_address = *it;
        vec_sadd.push_back(sadd);
    }
    for (vector<Vginfo>::iterator it = Msg._vginfo.begin(); it != Msg._vginfo.end(); it++)
    {
        sv._sid = sa._sid;
        sv._name= it->_vgname;
        sv._size = it->_total_size;
        vec_sv.push_back(sv);
    }
}

STATUS SaManager::Init()
{   
    _regedit_keep_time = 60;
    STATUS ret = Start(MU_SA_MANAGER);
    if (SUCCESS != ret)
    {
        return ret;
    }

    if (NULL == (_db_set = ScDbOperationSet::GetInstance(GetDB())))
    {
        return ERROR;
    }
    MessageFrame message(SkyInt32(0), EV_POWER_ON);
    ret = m->Receive(message);
    if (SUCCESS != ret)
    {
        SkyAssert(0);

    }
    return ret;
}

void SaManager::MessageEntry(const MessageFrame& message)
{
    switch (m->get_state())
    {
    case S_Startup:
    {
        switch (message.option.id())
        {
        case EV_POWER_ON:
        {
            /* 设置sa老化定时器，该函数内部会控制，只是创建一次定时器 */
            SetSaKeepTimer();
            /* 设置组播组，只进入一次 */
            STATUS ret = m->JoinMcGroup(MUTIGROUP_SA_DISCOVER);
            if (SUCCESS != ret)
            {
                SkyAssert(false);
                string lastwords("SaManager failed to join group! ret = ");
                lastwords.append(to_string(ret,dec));
                SkyExit(-1,lastwords);
            }
            Transaction::Enable(GetDB());
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
        switch (message.option.id())
        {
        case EV_SA_DISCOVER:
        {
            DoSaDiscover(message);
            break;
        }

        case EV_SA_INFO_REPORT:
        {
            DoSaInfoReport(message);
            break;
        }
        
        case EV_SA_KEEP_STATE_TIMER:
        {           
            RefreshSa();
            break;
        }

        case EV_SA_ENABLE_REQ:
        {
            DoEnableSa(message);
            break;
        }

        case EV_SA_FORGET_ID_REQ:
        {
            DoForgetSaById(message);
            break;
        }      

        case EV_SA_CLUSTER_MAP_REQ:
        {
            DoMapCluster(message);
            break;
        }

        default:
            break;
        }
    }
    
    default:
        break;
    }
}

void SaManager::SetSaKeepTimer()
{
    TimeOut timeout;

    if (_manager_period_timer != INVALID_TIMER_ID)
    {
        return;
    }

    _manager_period_timer = m->CreateTimer();
    timeout.type = LOOP_TIMER;
    timeout.duration = SA_PERIOD_TIME_LEN;
    timeout.msgid = EV_SA_KEEP_STATE_TIMER;
    if (SUCCESS != m->SetTimer(_manager_period_timer,timeout))
    {
        OutPut(SYS_EMERGENCY, "ClusterManager SetLoopTime Create timer fail!\n");
        SkyAssert(0);
        SkyExit(-1, "ClusterManager::SetLoopTime: call mu->SetTimer(_manager_period_timer,timeout) failed.");
    }
}

void SaManager::DoSaDiscover(const MessageFrame& message)
{
    int ret = ERROR;
    string err;
    StorageAdaptor sa,sa_db;
    StorageAdaptorAddress saa;
    vector<StorageAddress> vec_sadd;
    vector<StorageVg> vec_sv;
    MessageSaInfoReport Msg;
    MessageSaRegisterReq MsgReq;
    MID    dest;
    MID sender;

    Msg.deserialize(message.data);
    sender = message.option.sender();

    OutPut(SYS_DEBUG, "recv discover addr from %s\r\n", sender._application.c_str());
    /* 根据该集群发送来的信息，来查找该集群是否已经注册了 */
    ret = DbGetStorageAdaptorByMid(sender,sa_db);
    if (ERROR_OBJECT_NOT_EXIST == ret)
    {        
        FillSdInfo(sender,Msg,sa,saa,vec_sadd,vec_sv);
        sa._is_online = true;
        sa._enabled   = true;
        sa._register_time.refresh();
        DbAddStorageAdaptor(sa,saa,vec_sadd,vec_sv);
        OutPut(SYS_NOTICE,"deal discover info\n");
    }
    else if (SUCCESS == ret)
    {
        sa._sid = sa_db._sid;
        FillSdInfo(sender,Msg,sa,saa,vec_sadd,vec_sv);
        sa._enabled = sa_db._enabled;
        DbUpdateStorageAdaptor(sa,saa,vec_sadd,vec_sv);

        MsgReq._name = sa._application;
        if (_ifs_info.empty())
        {
            if (SUCCESS != GetAllIfsInfo(_ifs_info))
            {
                _ifs_info.clear();
            }
        }
        MsgReq._sc_ifs_info = _ifs_info;
        dest._application = sa._application;
        dest._process = PROC_SA;
        dest._unit    = sa._name;
        DbGetVolumeWithLunInfoBySid(sa_db._sid,MsgReq._sa_target._volume_with_lun,MsgReq._sa_target._authorize_with_target_id);
        MessageOption option(dest,EV_SA_REGISTER_REQ,0,0);
        m->Send(MsgReq, option);
    }
    else
    {
        OutPut(SYS_ERROR, "DoSaDiscover DbGetStorageAdaptorByMid Failed ret:%d\r\n",ret);
    }
}

void SaManager::DoSaInfoReport(const MessageFrame& message)
{
    int ret = ERROR;
    string err;
    StorageAdaptor sa,sa_db;
    StorageAdaptorAddress saa;
    vector<StorageAddress> vec_sadd;
    vector<StorageVg> vec_sv;
    MessageSaInfoReport Msg;
    MessageSaUnRegisterReq MsgReq;   
    MID sender;

    Msg.deserialize(message.data);
    sender = message.option.sender();

    ret = DbGetStorageAdaptorByMid(sender,sa_db);
    if (ERROR_OBJECT_NOT_EXIST == ret)
    {
        MsgReq._name =sender._application;             
        MessageOption option(sender,EV_SA_UNREGISTER_REQ,0,0);
        m->Send(MsgReq, option);
    }
    else if (SUCCESS == ret)
    {
        sa._sid = sa_db._sid;
        FillSdInfo(sender,Msg,sa,saa,vec_sadd,vec_sv);
        sa._enabled = sa_db._enabled;
        if(sa_db._is_online==false)
        {
            string address;
            DbGetSaAddress(sa_db._application,address);
            SendAlarm(address,ALM_TECS_SA_OFF_LINE,OAM_REPORT_RESTORE);
                
        }
        sa._is_online = true;
        DbUpdateStorageAdaptor(sa,saa,vec_sadd,vec_sv);
    }
    else
    {
        OutPut(SYS_ERROR, "DoSaInfoReport DbGetStorageAdaptorByMid Failed ret:%d\r\n",ret);
    }
}

void SaManager::RefreshSa(void)
{
    vector<StorageAdaptor> vec_sa;
    string where = "(is_online = 1) and (enabled = 1)";

    if (SUCCESS != _db_set->_adaptor->Select(vec_sa,where))
    {
        OutPut(SYS_ERROR, "sdp select failed\r\n");
        return;
    }
    Datetime now_time; /* 获取当前的系统时间 */
    
    /* 调用数据池的刷新接口进行数据内容刷新 */
    vector<StorageAdaptor>::iterator it;
    for (it = vec_sa.begin(); it != vec_sa.end(); it++)
    {
        if ((now_time.tointeger() - it->_refresh_time.tointeger()) >= _regedit_keep_time)
        {
            if (TRUE == it->_is_online)
            {
                it->_is_online = false;
                string address;
                DbGetSaAddress(it->_application,address);
                SendAlarm(address,ALM_TECS_SA_OFF_LINE,OAM_REPORT_ALARM);
                
                if (SUCCESS != _db_set->_adaptor->Update(*it))
                {
                    OutPut(SYS_ERROR, "sdp update failed\r\n");
                }
            }
        }
    }
}

void SaManager::DoEnableSa(const MessageFrame& message)
{
    MID sender;
    int ret;
    MessageSaEnableReq req;
    MessageSaCommonAck ack;    
    req.deserialize(message.data);
    sender = message.option.sender();
    MessageOption option(sender,EV_SA_ENABLE_ACK,0,0);
    
    StorageAdaptor  sa;
    ret=DbGetStorageAdaptorBySid(req._sid,sa);
    
    if(ret!=SUCCESS)
    {
        ack._ret_code=ret;
        ack._result="DbGetStorageAdaptorBySid err,sid invalid";
        m->Send(ack, option);
        return ;
    }
    sa._enabled=req._enable;
    ret=DbUpdateStorageAdaptor(sa);
    if(ret!=SUCCESS)
        ret=ERROR_DB_UPDATE_FAIL;
    ack._ret_code=ret;
    ack._result=(ret==SUCCESS)?"ok":"DbUpdateStorageAdaptor err ";
    m->Send(ack, option);
   
}

void SaManager::DoForgetSaById(const MessageFrame& message)
{
    MID sender;
    int ret;
    MessageSaForgetIdReq req;
    MessageSaCommonAck ack;    
    req.deserialize(message.data);
    sender = message.option.sender();
    MessageOption option(sender,EV_SA_FORGET_ID_ACK,0,0);
    
    StorageAdaptor sa;    
    ret=DbGetStorageAdaptorBySid(req._sid,sa);
    
    if(ret!=SUCCESS)
    {
        ack._ret_code=ret;
        ack._result="DbGetStorageAdaptorBySid err,sid invalid";
        m->Send(ack, option);
        return ;
    }     
    ret=DbDropStorageAdaptor(sa);
    if(ret!=SUCCESS && ret!=ERROR_PERMISSION_DENIED)
    {
        ret=ERROR_DB_DELETE_FAIL;
    }
    ack._ret_code=ret;
    if(ret==SUCCESS)
    {
        ack._result="ok";    
    }
    else if(ret==ERROR_PERMISSION_DENIED)
    {
        ack._result="sa is still use,can't remove ";  
    }
    else
    {
        ack._result="DbDropStorageAdaptorAddress err ";
    }    
    m->Send(ack, option);
}


void SaManager::DoMapCluster(const MessageFrame& message)
{
    MID sender;
    int ret;
    MessageSaClusterMapReq req;
    MessageSaCommonAck ack;    
    req.deserialize(message.data);
    sender = message.option.sender();
    MessageOption option(sender,EV_SA_CLUSTER_MAP_ACK,0,0);
    
    
    StorageCluster sc;    
    sc._cluster_name=req._cluster_name;
    sc._sid         =req._sid;
    if(req._is_add)
    {
        ret=DbAddClusterMap(sc); 
        if(ret!=SUCCESS && ret!=ERR_EXIST)   
        {
            ret=ERROR_DB_INSERT_FAIL;        
        }
    }
    else
    {
        ret=DbDropClusterMap(sc); 
        if(ret!=SUCCESS && ret!=ERR_OBJECT_NOT_EXIST)   
        {
            ret=ERROR_DB_DELETE_FAIL;
        }        
    }        
    
    ack._ret_code=ret;
    ack._result=(ret==SUCCESS)?"ok":"DbAddClusterMap err ";
    m->Send(ack, option);
    

}

void SaManager::SendAlarm(string &ip, uint32 code, byte flag)
{  
    AlarmReport ar;
    ar.alarm_location.strLocation=ip;
    ar.alarm_location.strLevel1=ApplicationName();
    ar.dwAlarmCode = code;
    ar.ucAlarmFlag = flag;
   
    AlarmAgent *aa = AlarmAgent::GetInstance();
    SkyAssert(NULL != aa);
    aa->Send(ar);

}
}

