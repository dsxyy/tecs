
#include "vna_common.h"

#include "vnet_monitor.h"

int g_monitor_times = 1;
void t_monitor_times(int i)
{
    if( i > 0 )
    g_monitor_times = i;
    cout << "monitor time: " << g_monitor_times;
}
DEFINE_DEBUG_FUNC(t_monitor_times);

namespace zte_tecs
{
CVNetMonitorMgr* CVNetMonitorMgr::s_pInstance = NULL;

#define BC_NET_MONITOR_TIMER_INTERVAL (1000) // 1s 测试10s

int g_dbgVNetMonFlag = false;
void DbgSetVNetMonitor(int i)
{
    g_dbgVNetMonFlag = i;
}
DEFINE_DEBUG_FUNC(DbgSetVNetMonitor);

CVNetMsgHandler::CVNetMsgHandler():_msg_unit(NULL)
{
    _state_machines = 0;    
}

CVNetMsgHandler::~CVNetMsgHandler()
{
    if(_msg_unit )
    {        
        delete _msg_unit;
        _msg_unit = NULL;
    }
}

STATUS CVNetMsgHandler::StartMu(const string& name)
{
    // 消息单元的创建和初始化
    _msg_unit = new MessageUnit(name);
    if (!_msg_unit)
    {
        VNET_LOG(VNET_LOG_ERROR, "Create mu %s failed!\n",name.c_str());
        return ERROR_NO_MEMORY;
    }

    STATUS ret = _msg_unit->SetHandler(this);
    if (SUCCESS != ret)
    {
        VNET_LOG(VNET_LOG_ERROR, "SetHandler mu %s failed! ret = %d\n",name.c_str(),ret);
        return ret;
    }

    ret = _msg_unit->Run();
    if (SUCCESS != ret)
    {
        VNET_LOG(VNET_LOG_ERROR, "Run mu %s failed! ret = %d\n",name.c_str(),ret);
        return ret;
    }

    ret = _msg_unit->Register();
    if (SUCCESS != ret)
    {
        VNET_LOG(VNET_LOG_ERROR, "Register mu %s failed! ret = %d\n",name.c_str(),ret);
        return ret;
    }

    _msg_unit->set_state(S_Startup);
    _state_machines = VNET_STATE_MACHINE_START;

    //给自己发送上电消息，促使消息单元状态切换到工作态
    MessageFrame message(SkyInt32(0), EV_VNET_POWER_ON);
    ret = _msg_unit->Receive(message);
    if (SUCCESS != ret)
    {
        SkyAssert(0);
        return ret;
    }
    _msg_unit->Print();
    return SUCCESS;
}

STATUS CVNetMsgHandler::Receive(const MessageFrame& message)
{
    return _msg_unit->Receive(message);
}


CVNetMonitorMgr::CVNetMonitorMgr()
{
    _timer_id = INVALID_TIMER_ID;
}

CVNetMonitorMgr::~CVNetMonitorMgr()
{
    list<CVNetMonitorBase *>::iterator it = _lst_monitor.begin();
    list<CVNetMonitorBase *>::iterator it_tmp;
    for(; it != _lst_monitor.end();)
    {   
        it_tmp = it;
        it = _lst_monitor.erase(it);
        delete (*it_tmp);   
    }

    if(_msg_unit )
    {
        if ( INVALID_TIMER_ID != _timer_id )
        {
            _msg_unit->KillTimer(_timer_id);
            _timer_id = INVALID_TIMER_ID;
        }
    }
}

int32 CVNetMonitorMgr::VNetInit(void * arg)
{       
    // 初始化Monitor list
    // bond monitor object 
    CVNetMonitorBase * pBond = new CVNetMonitorBond();
    if(pBond && (0==pBond->Init()))
    {
        _lst_monitor.push_back(pBond);
    }

    CVNetMonitorBase * pNic = new CVNetMonitorNic();
    if(pNic && (0==pNic->Init()))
    {
        _lst_monitor.push_back(pNic);
    }
    
        
    // 启动定时器 
    _timer_id = _msg_unit->CreateTimer();
    if (INVALID_TIMER_ID == _timer_id)
    {
         VNET_LOG(VNET_LOG_ERROR, "CVNetMonitorMgr::InitVNetMonitorMgr: Create monitor timer failed.\n");
         return -1;
    }  
    
    SetMonitorTimer();
    return 0;
}

int32 CVNetMonitorMgr::SetMonitorTimer()
{
    TimeOut  tTmOut;
    tTmOut.duration = BC_NET_MONITOR_TIMER_INTERVAL *g_monitor_times;
    tTmOut.msgid = TIMER_VNA_MONITOR_MGR_TIMER;
    tTmOut.type = RELATIVE_TIMER;
    tTmOut.arg = 0;
    if( _msg_unit)
    {
        if (SUCCESS != _msg_unit->SetTimer(_timer_id, tTmOut))
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNetMonitorMgr::SetMonitorTimer: Set timer(id:%u) failed.\n", _timer_id);
        }
    }
    return 0;
}

void CVNetMonitorMgr::MessageEntry(const MessageFrame& message)
{
    switch (_msg_unit->get_state())
    {
        case S_Startup:
        {
            switch (message.option.id())
            {
                case EV_VNET_POWER_ON:
                {                    
                    // 完成初始化
                    if (0 != VNetInit(NULL))
                    {
                        VNET_LOG(VNET_LOG_ERROR, "CVNetMonitorMgr::MessageEntry failed.\n");
                        SkyAssert(false);
                        SkyExit(-1,"CVNetMonitorMgr VNetInit failed!\n");
                    }
    
                    OutPut(SYS_NOTICE, "%s power on!\n",_msg_unit->name().c_str());
                     
                    _msg_unit->set_state(S_Work);
                    VNET_LOG(VNET_LOG_INFO, "CVNetMonitorMgr: entering VNET_STATE_MACHINE_WORK.\n");   
                }
                break;
            
            default:
                break;
            }
        } // end VNET_STATE_MACHINE_START
        break;
        case S_Work:
        {
            // 处于WORK状态 已初始化成功
            if( TIMER_VNA_MONITOR_MGR_TIMER == message.option.id() )
            {
                Handle(); 

                SetMonitorTimer();
                return;
            }        

            VNET_LOG(VNET_LOG_INFO, "CVNetMonitorMgr: unknown message(ID=%d).\n", message.option.id());

        } // end 
        break;
        default:
        {}
        break;
    }
}

void CVNetMonitorMgr::Handle()
{
    list<CVNetMonitorBase *>::iterator it = _lst_monitor.begin();
    for(; it != _lst_monitor.end();++it)
    {   
        (*it)->MonitorHandle();
    }
}

int32 CVNetMonitorMgr::DbgShowData(void)
{
    cout << "--------------------------VNet Monitor Info------------------------" << endl;
    list<CVNetMonitorBase *>::iterator it = _lst_monitor.begin();
    for(; it != _lst_monitor.end();++it)
    {   
        (*it)->Print();
    }
    return 0;
}

void DBG_VNA_MONITOR_SHOW()
{
    CVNetMonitorMgr * pInst = CVNetMonitorMgr::GetInstance();
    if(NULL != pInst)
    {
        pInst->DbgShowData();
    }
}
DEFINE_DEBUG_FUNC(DBG_VNA_MONITOR_SHOW);

}



