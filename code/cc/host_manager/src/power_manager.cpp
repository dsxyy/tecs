/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�power_manager.cpp
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��power_manager��ʵ���ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�liuyi
* ������ڣ�2013��4��19��
*
* �޸ļ�¼1��
*     �޸����ڣ�2013/4/19
*     �� �� �ţ�V1.0
*     �� �� �ˣ�liuyi
*     �޸����ݣ�����
*******************************************************************************/
#include "sky.h"
#include "tecs_config.h"
#include "config_cluster.h"
#include "power_manager.h"
// �뱾ģ�齻������Ϣ�ṹ����
#include <fstream>

namespace zte_tecs 
{

// ��ʱ����Ϣ��
#define TIMER_CHECK_POWEROFF        EV_TIMER_1  // �����ϱ���Ⱥ���ֻ���Ϣ

PowerManager *PowerManager::_instance = NULL;

/******************************************************************************/
PowerManager::PowerManager():_mu(NULL),_have_startup(false)
{
    _save_energy_enable = false;
    _timer_id = INVALID_TIMER_ID;
    _timer_duration = GetCheckDuration();
}
/******************************************************************************/
STATUS PowerManager::StartMu(const string& name)
{
    if(_mu)
    {
        SkyAssert(false);
        return ERROR_DUPLICATED_OP;
    }
    
    // ��Ϣ��Ԫ�Ĵ����ͳ�ʼ��
    _mu = new MessageUnit(name);
    if (!_mu)
    {
        OutPut(SYS_EMERGENCY, "Create mu %s failed!\n",name.c_str());
        return ERROR_NO_MEMORY;
    }

    STATUS ret = _mu->SetHandler(this);
    if (SUCCESS != ret)
    {
        OutPut(SYS_EMERGENCY, "SetHandler mu %s failed! ret = %d\n",name.c_str(),ret);
        return ret;
    }

    ret = _mu->Run();
    if (SUCCESS != ret)
    {
        OutPut(SYS_EMERGENCY, "Run mu %s failed! ret = %d\n",name.c_str(),ret);
        return ret;
    }

    ret = _mu->Register();
    if (SUCCESS != ret)
    {
        OutPut(SYS_EMERGENCY, "Register mu %s failed! ret = %d\n", name.c_str(),ret);
        return ret;
    }

    //���Լ������ϵ���Ϣ����ʹ��Ϣ��Ԫ״̬�л�������̬
    MessageFrame message(SkyInt32(0), EV_POWER_ON);
    ret = _mu->Receive(message);
    if (SUCCESS != ret)
    {
        SkyAssert(0);
        return ret;
    }
    _mu->Print();
    return SUCCESS;
}

/******************************************************************************/
void PowerManager::MessageEntry(const MessageFrame &message)
{
    switch (_mu->get_state())
    {
    case S_Startup:
    {
        switch (message.option.id())
        {
            case EV_POWER_ON:
            {
                DoStartUp();
                _mu->set_state(S_Work);
                OutPut(SYS_NOTICE, "%s power on!\n",_mu->name().c_str());
                break;
            }
            default:
                OutPut(SYS_NOTICE, "Ignore any message except power on!\n");
                break;
        }
        break;
    }

    case S_Work:
    {
        switch (message.option.id())
        {
            case TIMER_CHECK_POWEROFF:
            {
                DoSaveEnergyTimer();
                break;
            }
            
            case EV_HOST_EXTERN_CTRL_REQ:
            {
                DoHostExternCtrlReq(message);
                break;            
            }

            case EV_HOST_EXTERN_QUERY_REQ:
            {
                DoHostExternQueryReq(message);
                break;            
            }

            case EV_CLUSTER_SAVE_ENERGY_CONFIG_CHG:
            {
                DoSetEnergyTimer();
                break;
            }

            default:
            {
                cout << "PowerManager receives an unkown message!" << endl;
                OutPut(SYS_NOTICE, "Unknown message: %d.\n", message.option.id());
                break;
            }
        }
        break;
    }

    default:
        break;
    }
}

/******************************************************************************/
void PowerManager::DoStartUp()
{
    STATUS ret;

    // ��ֹ����
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

    // ����PowerManager��Ҫ�Ķ�ʱ��
    TimeOut     timeout;

    // ���ö�ʱ��: �������ڼ���Ƿ�����Ҫ�µ������
    if (INVALID_TIMER_ID == (_timer_id = _mu->CreateTimer()))
    {
        OutPut(SYS_EMERGENCY, "Create timer for power_manager  failed!\n");
        SkyAssert(INVALID_TIMER_ID != _timer_id);
        SkyExit(-1, "power_manager::DoStartUp: Create timer for power_manager info "
                "report failed.");
    }
    timeout.type = LOOP_TIMER;
    timeout.duration = 30 * 60 * 1000;
    timeout.msgid = TIMER_CHECK_POWEROFF;
    if (SUCCESS != (ret = _mu->SetTimer(_timer_id, timeout)))
    {
        OutPut(SYS_EMERGENCY, "Set timer for check power operate failed!\n");
        SkyAssert(SUCCESS == ret);
        SkyExit(-1, "power_manager::DoStartUp: Set timer for power_manager  "
                "failed.");
    }

    if (SUCCESS != Transaction::Enable(GetDB()))
    {
        OutPut(SYS_ERROR,"power_manager enable transaction fail! ret = %d",ret);
        SkyExit(-1, "ClusterAgent::MessageEntry: call "
                "Transaction::Enable(GetDB()) failed.");
    }    
}
/******************************************************************************/
void PowerManager::DoHostExternCtrlReq(const MessageFrame & message)
{
    MessageHostExternCtrlCmdReq req;
    req.deserialize(message.data);

    MessageHostExternCtrlCmdAck ack;
    MessageOption option(message.option.sender(), 
                         EV_HOST_EXTERN_CTRL_ACK, 0, 0);

    // ���л����ж�
    ack._ret_code = ERR_HOST_OK;

    Host *host = _host_pool->Get(req._host_name, false);
    if(NULL == host )
    {
        ack._ret_code = ERR_HOST_NOT_EXIST;
        _mu->Send(ack, option);
        return ;        
    }

    int64 hid = host->GetOID();
    host_atca_table_info info;
    _host_atca->SearchByHostId(info,hid);
    string cmm_ip = _cmm_config->GetCmmIP(info.host_position.bureau,
                                   info.host_position.rack,
                                   info.host_position.shelf);

    if(cmm_ip.empty())
    {
        ack._ret_code = ERR_CMM_IP_NULL;
        _mu->Send(ack, option);
        return ;            
    }

    //�ж�֮ǰ״̬
    if (_host_atca->CheckPositionIsRepeat(hid))// HOST�����ڻ���û�мܿ����Ϣ����֧��ͨ��cmm���в���
    {
        ack._ret_code = ERR_OPERATE_NOT_SUPPORT;
        _mu->Send(ack, option);
        return ;        
    }

    int32 last_poweroff_state=_host_atca->GetPowerState(hid);
    if((req._cmd == MessageHostExternCtrlCmdReq::POWER_OFF && last_poweroff_state ==HostAtca::OPERATE_POWER_OFF )||
      (req._cmd == MessageHostExternCtrlCmdReq::POWER_ON && last_poweroff_state ==HostAtca::OPERATE_POWER_ON ))
    {
        _mu->Send(ack, option);
        return ;        
    }

    int64 current_time = static_cast<int64>(time(0));
    if(req._cmd == MessageHostExternCtrlCmdReq::POWER_OFF)
    {
        _host_atca->UpdatePowerOperate(hid,HostAtca::OPERATE_POWER_OFF);
        _host_atca->UpdatePowerOffTime(hid,current_time);

    }
    else if(req._cmd == MessageHostExternCtrlCmdReq::POWER_ON)
    {
        int64 poweroff_time = 0;
        _host_atca->GetPowerOffTime(hid,poweroff_time);
        _host_atca->UpdatePowerOperate(hid,HostAtca::OPERATE_POWER_ON);
        if(0 != poweroff_time)
        {
             int64 total_poweroff_time = 0;
            _host_atca->GetTotalPowerOffTime(hid,total_poweroff_time);
             int64 tmp = total_poweroff_time+current_time-poweroff_time;
            _host_atca->UpdateTotalPowerOffTime(hid,tmp);
        }
    }

    _mu->Send(ack, option);
}
/******************************************************************************/
void PowerManager::DoHostExternQueryReq(const MessageFrame &message)
{
    MessageHostExternCtrlQueryReq req;
    req.deserialize(message.data);

    MessageHostExternCtrlQueryAck ack;
    ack._host_name = req._host_name;
    ack._state = MessageHostExternCtrlQueryAck::State_Unknown;

    MessageOption option(message.option.sender(), EV_HOST_EXTERN_QUERY_ACK, 0, 0);

    Host *host = NULL;
    if(NULL == (host = _host_pool->Get(req._host_name, false)))
    {
        ack._ret_code = ERR_HOST_NOT_EXIST;
        _mu->Send(ack, option);
        return ;        
    }
    int64 hid = host->GetOID();

    host_atca_table_info info;   
    _host_atca->SearchByHostId(info,hid);
    string cmm_ip = _cmm_config->GetCmmIP(info.host_position.bureau,
                                   info.host_position.rack,
                                   info.host_position.shelf);

    if(cmm_ip.empty())
    {
        ack._ret_code = ERR_CMM_IP_NULL;
        _mu->Send(ack, option);
        return ;            
    }
    
    ack._ret_code = ERR_HOST_OK;
    int32 state = 0;
    if(SUCCESS == _host_atca->GetPoweStateByHostId(hid,state))
    {
        ack._state = state;
    }
    
    _mu->Send(ack, option);
}
/******************************************************************************/
bool PowerManager::DoGetSaveEnergyFlg()
{
    bool ret = false;
    string save_energy_flg =  _cluster_config->Get(CFG_CLUSTER_SAVE_ENERGY_FLG);
    transform(save_energy_flg.begin(), save_energy_flg.end(), save_energy_flg.begin(),::toupper);    

    if("YES" == save_energy_flg)
    {
        ret = true;
    }

    return ret;
}
/******************************************************************************/
void PowerManager::DoSaveEnergyProc()
{
    int32 keep_min;//����ģʽ��С�ϵ�������Ŀ
    int32 keep_max;   //����ģʽ����ϵ��������Ŀ
    int32 keep_per;   //����ģʽ���������������Ŀ�µ�ٷֱ�
    int32 host_with_vm;//�����������host��Ŀ
    int32 need_poweron;
    string _cluster_para;

    _cluster_para = _cluster_config->Get(CFG_CLUSTER_POWERON_MIN);
    keep_min = atoi(_cluster_para.c_str());
    _cluster_para = _cluster_config->Get(CFG_CLUSTER_POWERON_MAX);
    keep_max = atoi(_cluster_para.c_str());
    _cluster_para = _cluster_config->Get(CFG_CLUSTER_POWERON_PER);
    keep_per = atoi(_cluster_para.c_str());
    host_with_vm = _host_atca->GetRunningVmsHosts();

    //���ݽ���������Ŀ�㷨�õ�����Ҫ�µ�������Ŀ
    if (host_with_vm * keep_per /100 >= keep_max)
    {
        need_poweron = keep_max;
    }
    else if (host_with_vm * keep_per /100 < keep_min)
    {
        need_poweron = keep_min;
    }
    else
    {
        need_poweron = host_with_vm * keep_per /100;
    }

    //��ȡϵͳ���ϵ�,����,û�в����������������Ŀ
    int32 have_poweron = _host_atca->GetOnlineAndNoVmsHostNum();
    if(need_poweron == have_poweron)
    {
        return;
    }
    
    int64 i = 0;
    vector<int64> hids;    
    vector<int64>::iterator iter ;
    
    int64 poweroff_time;
    int64 total_poweroff_time ;
    int64 current_time = static_cast<int64>(time(0));

    if((need_poweron - have_poweron) > 0)
    {
        //�����Ҫ�ϵ�������Ŀ�����Ѿ��ϵ���������Ҫѡ�����߲��Ҳ���״̬Ϊ�µ������
        i = need_poweron - have_poweron;
        _host_atca->GetOffLineAndPoweroffOperateHosts(hids);
        for ( iter = hids.begin(); iter != hids.end(); ++iter)
        { 
            if(i == 0)
            {
                return;
            }
            poweroff_time = 0;
            _host_atca->GetPowerOffTime(*iter,poweroff_time);
            _host_atca->UpdatePowerOperate(*iter,HostAtca::OPERATE_POWER_ON);
            if(0 != poweroff_time)
            {
                total_poweroff_time = 0;
                _host_atca->GetTotalPowerOffTime(*iter,total_poweroff_time);
                 int64 tmp = total_poweroff_time+current_time-poweroff_time;                
                _host_atca->UpdateTotalPowerOffTime(*iter,tmp);
            }
            i--;
        }
    }
    else
    {
        //�����Ҫ�ϵ�������ĿС���Ѿ��ϵ���������Ҫѡ�����߲��Ҳ���״̬Ϊ�µ������
        i = have_poweron - need_poweron;
        _host_atca->GetOnLineAndPoweronOperateHosts(hids);
        
        //������Ҫ�µ���������power_operateΪ�µ�
        for ( iter = hids.begin(); iter != hids.end(); ++iter)
        {     
            _host_atca->UpdatePowerOperate(*iter,HostAtca::OPERATE_POWER_OFF);
            _host_atca->UpdatePowerOffTime(*iter,current_time);

            if(i == 0)
            {
                return;
            }
            i--;
        }        
    }

    return;
}
/******************************************************************************/
void PowerManager::DoAllHostPowerOn()
{
    int64 current_time = static_cast<int64>(time(0));
    int64 poweroff_time;
    int64 total_poweroff_time ;

    vector<int64> hids;
    _host_atca->GetAllPoweroffOperateHosts(hids);

    vector<int64>::iterator iter = hids.begin();
    for ( ; iter != hids.end(); ++iter)
    {     
        poweroff_time = 0;
        _host_atca->GetPowerOffTime(*iter,poweroff_time);
        _host_atca->UpdatePowerOperate(*iter,HostAtca::OPERATE_POWER_ON);
        if(0 != poweroff_time)
        {
            total_poweroff_time = 0;
            _host_atca->GetTotalPowerOffTime(*iter,total_poweroff_time);
             int64 tmp = (total_poweroff_time+current_time-poweroff_time);                
            _host_atca->UpdateTotalPowerOffTime(*iter,tmp);
        }
    }

    return;
}
/******************************************************************************/
void PowerManager::DoSaveEnergyTimer()
{
    _save_energy_enable = DoGetSaveEnergyFlg();

    if( _save_energy_enable )
    {
        DoSaveEnergyProc();
    }

    return;
}

/******************************************************************************/
void PowerManager::DoSetEnergyTimer()
{
    uint32 duration = GetCheckDuration();
    if (_timer_duration != duration)
    {
        _timer_duration = duration;
        // ����������Ϣ�ϱ�ѭ����ʱ��
        _mu->StopTimer(_timer_id);
        TimeOut timeout;
        timeout.type = LOOP_TIMER;
        timeout.duration = _timer_duration;
        timeout.msgid = TIMER_CHECK_POWEROFF;
        _mu->SetTimer(_timer_id, timeout);
    }

    return;
}

/******************************************************************************/
uint32 PowerManager::GetCheckDuration()
{
    string _cluster_para;
    uint32 timer_duration = 1800 * 1000;    // Ĭ�϶�ʱ��ʱ��
    ConfigCluster *cluster_config = NULL;

    cluster_config = ConfigCluster::GetInstance();
    if(NULL == cluster_config)
    {
        return timer_duration;
    }

    _cluster_para = cluster_config->Get(CFG_CLUSTER_SAVE_ENERGY_INTERVAL);
    if("" != _cluster_para)
    {
        timer_duration = atoi(_cluster_para.c_str()) * 60 * 1000;
    }

    return timer_duration;
}

void DbgShowSaveOptions()
{
    PowerManager *power_manager = PowerManager::GetInstance();
    if (NULL == power_manager)
    {
        cout << "Please check if power_manager work normally!" << endl;
        return;
    }

    // print params of save energy
    cout << "save_energy_flg : " 
         << (true == power_manager->GetFlagOfSaveEnergy() ? "enable" : "disable") << endl;
    cout << "timer_duration  : " << power_manager->GetSaveTimerDuration() << " s"<< endl;
}
DEFINE_DEBUG_FUNC(DbgShowSaveOptions);

} // namespace zte_tecs

