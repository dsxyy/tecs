#include "tecs_config.h"
#include "log_agent.h"
#include "storage_adaptor.h"
#include "mid.h"
#include "event.h"
#include "storage_messages.h"
#include "ntp_building.h"
#include "dns_building.h"

namespace zte_tecs
{

#define EV_REFERESH_TIMEID          EV_TIMER_1  //
#define TIMER_SA_REPORT             EV_TIMER_2  // 周期上报发现或信息

Storage_Adaptor::Storage_Adaptor(sa_cfg &cfg):_cfg(cfg)
{
    _impl=NULL;
    _have_startup=false;
}

STATUS Storage_Adaptor::Init(void)
{
    string name=MU_STORAGE_ADAPTOR+_cfg._name;
    STATUS ret = Start(name);
    if (SUCCESS != ret)
    {
        return ret;
    }
    AddKeyMu(name);
    _impl=StorageAdaptorFactory::GetImpl(_cfg._type);

    ret = _impl->Init(_cfg);
    if (SUCCESS != ret)
    {
        SkyAssert(false);
        return ret;
    }
    //给自己发一个上电通知消息
    MessageFrame frame(SkyInt(0),EV_POWER_ON);
    return m->Receive(frame);
}

STATUS Storage_Adaptor::AllocVol(const StAllocVolReq & req,string &vol_uuid)
{
    return _impl->AllocVol(req,vol_uuid);
}

STATUS Storage_Adaptor::DeleteVol(const StDelVolReq & req)
{
    return _impl->DeleteVol(req);
}

STATUS Storage_Adaptor::AllocSnapshot(const StAllocSnapshotReq & req,string &vol_uuid)
{
    return _impl->AllocSnapshot(req,vol_uuid);
}

STATUS Storage_Adaptor::DeleteSnapshot(const StDelVolReq & req)
{
    return _impl->DeleteSnapshot(req);
}

STATUS Storage_Adaptor::CreateTarget(const StCreateTargetReq & req)
{
    return _impl->CreateTarget(req);
}

STATUS Storage_Adaptor::DelTarget(const StDelTargetReq & req)
{
    return _impl->DelTarget(req);
}

STATUS Storage_Adaptor::AddVol2Target(const StAddToTarReq& req)
{
    return _impl->AddVol2Target(req);
}

STATUS Storage_Adaptor::AddHost2Target(const StAddToTarReq& req)
{
    return _impl->AddHost2Target(req);
}

STATUS Storage_Adaptor::DelVolFromTarget(const StDelFromTarReq & req)
{
    return _impl->DelVolFromTarget(req);
}

STATUS Storage_Adaptor::DelHostFromTarget(const StDelFromTarReq & req)
{
    return _impl->DelHostFromTarget(req);
}

STATUS Storage_Adaptor::GetStorageInfo(capacity & info)
{
    return _impl->GetStorageInfo(info);
}

STATUS Storage_Adaptor::KeepAlive(void)
{
    return _impl->KeepAlive();
}

STATUS Storage_Adaptor::GetDataIp(vector<string>  &dataip)
{
    return _impl->GetDataIp(dataip);
}

STATUS Storage_Adaptor::GetVgInfo(vector<Vginfo> &vginfo)
{
    return _impl->GetVgInfo(vginfo);
}

STATUS Storage_Adaptor::ReBuildTarget(sa_target_info &target)
{
    return _impl->ReBuildTarget(target);
}

void Storage_Adaptor::MessageEntry(const MessageFrame& message)
{
    string cmd;
    string result;
    int    ret=-1;
    bool   need_ack=true;    
    capacity    info;
    MID  sender;
    sender = message.option.sender();
    WorkReq wreq;
    wreq.deserialize(message.data);
    StStorageSvrAck ack(wreq.action_id);
    switch (m->get_state())
    {
    case S_Startup:
    {
        switch (message.option.id())
        {
        case EV_POWER_ON:
        {
            DoStartUp();
        }
        break;
        }
    }
    break;

    case S_Work:
    {
        switch (message.option.id())
        {
        case EV_SA_CREATE_VOL:
        {
            StAllocVolReq req;
            string        vol_uuid;
            req.deserialize(message.data);

            if(bSaRegister())
            {
                ret=AllocVol(req,vol_uuid);
            }
            else
            {
                ret = ERROR_SA_UNREGISTER;
            }
            ack.last_op=OP_CREATE_VOL;
            if (ret==SUCCESS)
            {
                ack.vol_uuid=vol_uuid;
            }
            if(ret!=SUCCESS)
            {
                ostringstream oss;
                oss<<"create vol err,ret:"<<ret<<endl;
                ack.detail=oss.str();
            }
        }
        break;

        case EV_SA_DELETE_VOL:       
        {
            StDelVolReq req;
            req.deserialize(message.data);
            if(bSaRegister())
            {
                ret=DeleteVol(req);
            }
            else
            {
                ret = ERROR_SA_UNREGISTER;
            }

            ack.last_op=OP_DEL_VOL;
            if(ret!=SUCCESS)
            {
                ostringstream oss;
                oss<<"delete vol err,ret:"<<ret<<endl;
                ack.detail=oss.str();
            }
        }
        break;

        case EV_SA_CREATE_SNAPSHOT:
        {
            StAllocSnapshotReq req;
            string        vol_uuid;
            req.deserialize(message.data);
            if(bSaRegister())
            {
                ret=AllocSnapshot(req,vol_uuid);
            }
            else
            {
                ret = ERROR_SA_UNREGISTER;
            }

            ack.last_op=OP_CREATE_SVOL;
            if (ret==SUCCESS)
            {
                ack.vol_uuid=vol_uuid;
            }
            if(ret!=SUCCESS)
            {
                ostringstream oss;
                oss<<"create svol err,ret:"<<ret<<endl;
                ack.detail=oss.str();
            }
        }
        break;

        case EV_SA_DELETE_SNAPSHOT:
        {
            StDelVolReq req;
            req.deserialize(message.data);
            if(bSaRegister())
            {
                ret=DeleteSnapshot(req);
            }
            else
            {
                ret = ERROR_SA_UNREGISTER;
            }

            ack.last_op=OP_DEL_SVOL;
            if(ret!=SUCCESS)
            {
                ostringstream oss;
                oss<<"delete svol err,ret:"<<ret<<endl;
                ack.detail=oss.str();
            }
        }       
        break;

        case EV_SA_CREATE_TARGET:
        {
            StCreateTargetReq req;
            req.deserialize(message.data);
            if(bSaRegister())
            {
                ret=CreateTarget(req);
            }
            else
            {
                ret = ERROR_SA_UNREGISTER;
            }

            ack.last_op=OP_CREATE_TARGET;
            if(ret!=SUCCESS)
            {
                ostringstream oss;
                oss<<"create target err,ret:"<<ret<<endl;
                ack.detail=oss.str();
            }
        }
        break;

        case  EV_SA_DELETE_TARGET:
        {
            StDelTargetReq req;
            req.deserialize(message.data);
            if(bSaRegister())
            {
                ret=DelTarget(req);
            }
            else
            {
                ret = ERROR_SA_UNREGISTER;
            }

            ack.last_op=OP_DEL_TARGET;    
            if(ret!=SUCCESS)
            {
                ostringstream oss;
                oss<<"delete target err,ret:"<<ret<<endl;
                ack.detail=oss.str();
            }
         
        }
        break;

        case EV_SA_ADDVOLTO_TARGET:
        {
            StAddToTarReq req;
            req.deserialize(message.data);
            if(bSaRegister())
            {
                ret=AddVol2Target(req);
            }
            else
            {
                ret = ERROR_SA_UNREGISTER;
            }

            ack.last_op=OP_ADD_VOL2TARGET;
            if(ret!=SUCCESS)
            {
                ostringstream oss;
                oss<<"addvol2target  err,ret:"<<ret<<endl;
                ack.detail=oss.str();
            }
        }
        break;

        case EV_SA_ADDHOSTTO_TARGET:
        {
            StAddToTarReq req;
            req.deserialize(message.data);
            if(bSaRegister())
            {
                ret=AddHost2Target(req);
            }
            else
            {
                ret = ERROR_SA_UNREGISTER;
            }

            ack.last_op=OP_ADD_HOST2TARGET;
            if(ret!=SUCCESS)
            {
                ostringstream oss;
                oss<<"addhost2target err,ret:"<<ret<<endl;
                ack.detail=oss.str();
            }
        }
        break;

        case EV_SA_DELVOLFROM_TARGET:
        {
            StDelFromTarReq req;
            req.deserialize(message.data);
            if(bSaRegister())
            {
                ret=DelVolFromTarget(req);
            }
            else
            {
                ret = ERROR_SA_UNREGISTER;
            }

            ack.last_op=OP_DEL_VOLFROMTARGET;
            if(ret!=SUCCESS)
            {
                ostringstream oss;
                oss<<"delvolfromtarget err,ret:"<<ret<<endl;
                ack.detail=oss.str();
            }
        }
        break;

        case EV_SA_DELHOSTFROM_TARGET:
        {
            StDelFromTarReq req;
            req.deserialize(message.data);
            if(bSaRegister())
            {
                ret=DelHostFromTarget(req);
            }
            else
            {
                ret = ERROR_SA_UNREGISTER;
            }

            ack.last_op=OP_DEL_HOSTFROMTARGET;
            if(ret!=SUCCESS)
            {
                ostringstream oss;
                oss<<"delhostfromtarget err,ret:"<<ret<<endl;
                ack.detail=oss.str();
            }
        }
        break;        
        
        case  EV_REFERESH_TIMEID:
        {
            KeepAlive();
            need_ack=false;
        }
        break;
        
        case TIMER_SA_REPORT :
        {
            DoSaInfoReport();
            need_ack=false;
        }
        break;

        case EV_SA_REGISTER_REQ:
        {
            DoSaRegister(message);
            need_ack=false;
        }
        break;

        case EV_SA_UNREGISTER_REQ:
        {
            DoSaUnregister(message);
            need_ack=false;
        }
        break;

        default:
        {
            need_ack=false;
            OutPut(SYS_ALERT,"storage_adaptor: receive a wrong msg, MsgID: %d  \n",message.option.id());
        }
        break;
        }

        if (need_ack)
        {
            ack.state=ret;
            if(ack.state==SUCCESS)
            {
                ack.progress=100;
            }
            else
            {
                ack.progress=0;
            }
            OutPut(SYS_DEBUG,"storage_adaptor Send Ack voluuid:%s,lastop:%d,ret:%d,to (%s,%s,%s)\n",                            
                             ack.vol_uuid.c_str(),
                             ack.last_op,                            
                             ack.state,
                             sender._application.c_str(),
                             sender._process.c_str(),
                             sender._unit.c_str());
            MessageOption option(sender, EV_SA_OPERATE_ACK, 0, 0);
            m->Send(ack, option);
        }
    }
    break;

    default:
        OutPut(SYS_ALERT,"storage_adaptor: receive a msg in wrong status, MsgID:  %d  \n",message.option.id());
        break;
    }
}

void Storage_Adaptor::DoStartUp()
{
    STATUS ret;

    // 防止重入
    if (_have_startup)
    {
        OutPut(SYS_ALERT, "Have start up, can not start up again!\n");
        SkyAssert(false == _have_startup);
        return;
    }
    else
    {
        _have_startup = true;
    }

    TIMER_ID    timer;
    TimeOut     timeout;

    // 设置定时器: 用于周期上报信息给sc
    if (INVALID_TIMER_ID == (timer = m->CreateTimer()))
    {
        OutPut(SYS_EMERGENCY, "Create timer for sa report failed!\n");
        SkyAssert(INVALID_TIMER_ID != timer);
        SkyExit(-1, "Storage_Adaptor::DoStartUp: Create timer for sa info "
                "report failed.");
    }
    timeout.type = LOOP_TIMER;
    timeout.duration = 10 * 1000;
    timeout.msgid = TIMER_SA_REPORT;
    if (SUCCESS != (ret = m->SetTimer(timer, timeout)))
    {
        OutPut(SYS_EMERGENCY, "Set timer for sa info report failed!\n");
        SkyAssert(SUCCESS == ret);
        SkyExit(-1, "Storage_Adaptor::DoStartUp: Set timer for sa info report "
                "failed.");
    }

    if (INVALID_TIMER_ID ==(timer=m->CreateTimer()))
    {
        OutPut(SYS_EMERGENCY, "Create timer for refreshtime failed!\n");
        SkyAssert(INVALID_TIMER_ID != timer);
        SkyExit(-1, "Storage_Adaptor::DoStartUp: Create timer for refreshtime "
                " failed.");
    }
    timeout.duration =20000;
    timeout.msgid = EV_REFERESH_TIMEID;
    timeout.type = LOOP_TIMER;
    if (SUCCESS != (ret = m->SetTimer(timer, timeout)))
    {
        OutPut(SYS_EMERGENCY, "Set timer for refreshtime failed!\n");
        SkyAssert(SUCCESS == ret);
        SkyExit(-1, "Storage_Adaptor::DoStartUp: Set timer for  refreshtime "
                "failed.");
    }
    DoSaInfoReport();
    m->set_state(S_Work);
}

void Storage_Adaptor::DoSaInfoReport()
{
    MessageSaInfoReport report;
    STATUS ret;
    capacity info;

    if (0!=(ret=GetStorageInfo(info)))
    {
        OutPut(SYS_ERROR,"GetStorageInfo err ret is %d\n",ret);
        return;
    }
    report._total_size=info.iTotalCapacity;
    report._max_size  =info.iAllocMaxCapacity;
    report._control_ip=_cfg._control_ip;
    report._type=_cfg._type;
    report._description=_cfg._description;
    TecsConfig *config = TecsConfig::Get();
    if (SUCCESS != (ret=GetIpByConnectBroker(config->get_sky_conf().messaging.connects[0].broker_url,report._sa_ip)))
    {
        OutPut(SYS_ERROR,"Storage_Adaptor :GetIpByConnectBroker err ret is %d\n",ret);
    }
    GetDataIp(report._dataip);   
    GetVgInfo(report._vginfo);
    if (_sc_name.empty())
    {
        MID receiver("group", MUTIGROUP_SA_DISCOVER);
        MessageOption option(receiver, EV_SA_DISCOVER, 0, 0);
        m->Send(report, option);
    }
    else
    {
        MID receiver(_sc_name, PROC_SC, MU_SA_MANAGER);
        MessageOption option(receiver, EV_SA_INFO_REPORT, 0, 0);
        m->Send(report, option);
    }
}

void Storage_Adaptor::DoSaRegister(const MessageFrame &message)
{
    MessageSaRegisterReq req;
    int   ret;
    req.deserialize(message.data);

    if (req._name != ApplicationName())
    {
        OutPut(SYS_ALERT, "DoSaRegister: sa name is err, %s != %s!\n",
               req._name.c_str(), ApplicationName().c_str());
        SkyAssert(0);
        return;
    }
    MessageSaRegisterAck  ack;
    ack._appendinfo = req._appendinfo;
    string sc_name = message.option.sender()._application;
    // 发送方为空，忽略
    if (sc_name.empty())
    {
        OutPut(SYS_ALERT, "DoSaRegister: sc name is empty!\n");
        SkyAssert(0);
        return;
    }

    string ip_connected;
    map<string, string>::iterator it;
    for (it  = req._sc_ifs_info.begin();
         it != req._sc_ifs_info.end();
         it++)
    {
        if (it->second.empty())
            continue;
        ip_connected.clear();
        if (SUCCESS != GetIpByConnect(it->second, ip_connected))
            continue;
        if (_sa_ip == ip_connected)
        {
            _sc_ip = it->second;
            break;
        }
    }
    if (!_sc_ip.empty())
    {
        // 设置 ntp, 向 sc 时间同步
        SetNtpClientConfig("register", _sc_ip, true);
        /* SC 一般与 TC 在一个主机上, TC 上已经设置了 DNS 服务器
         * 注: 目前 SC 暂无域名需要解析, 后续有域名进行解析, 此处需要增加 server 配置
         *     如 SetDnsServerConfig("register", _sc_ip, domain_name)
         */
        SetDnsClientConfig("register", _sc_ip, ApplicationName());
    }

    if (_sc_name.empty())
    {
        _sc_name=sc_name;
    }   

    /*重构target*/
    ret=ReBuildTarget(req._sa_target);
    if (ret!=SUCCESS)
    {
        OutPut(SYS_EMERGENCY, "DoSaRegister: BuildTarget err! ret is %d\n",ret);       
        SkyExit(-1, "Storage_Adaptor::ReBuildTarget err!!!.");
    }
}

void Storage_Adaptor::DoSaUnregister(const MessageFrame &message)
{
    MessageSaUnRegisterReq req;
    req.deserialize(message.data);
    // 发错了，忽略
    if (req._name != ApplicationName())
    {
        OutPut(SYS_ALERT, "DoSaUnregister: sa name is err, %s != %s!\n",
               req._name.c_str(), ApplicationName().c_str());
        SkyAssert(0);
        return;
    }

    string sc_name = message.option.sender()._application;
    // 发送方为空，忽略
    if (sc_name.empty())
    {
        OutPut(SYS_ALERT, "DoSaUnregister: tecs name is empty!\n");
        SkyAssert(0);
        return;
    }

    if (sc_name != _sc_name)
    {
        return;
    }

    _sc_name.clear();

    if (!_sc_ip.empty())
    {
        SetNtpClientConfig("unregister", _sc_ip, true);
        SetDnsClientConfig("unregister", _sc_ip, ApplicationName());
    }
  
    OutPut(SYS_EMERGENCY,
           "Storage_Adaptor: sa  is unregistered from %s!\n",
           sc_name.c_str());
    SkyExit(-1, "Storage_Adaptor::DoSaUnregister: sa is unregistered.");
}

void   Storage_Adaptor::GetScname(string &scname)
{
    scname=_sc_name;
}

bool Storage_Adaptor::bSaRegister(void)
{
    if (_sc_name.empty())
    {
        OutPut(SYS_DEBUG, "sa is unregister!\n");
        return false;
    }
    return true;
}

}

