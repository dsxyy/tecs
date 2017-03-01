/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�power_agent.cpp
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��power_agent��ʵ���ļ�
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
#include "power_agent.h"
#include <fstream>

static map<int32,int32>    atca_slot_map;     //atca�����λ���߼���λӳ���ϵ
static map<string,int32>   host_state_map;    //cmm��ѯ����ֵsting��״̬��ӳ���ϵ


namespace zte_tecs 
{

// ��ʱ����Ϣ��
#define TIMER_QUERY_HOST_STATE      EV_TIMER_1  // �����鲥������teceϵͳ����
#define TIMER_CHECK_POWER_STATE     EV_TIMER_2  // ���ڼ�����״̬��cmm״̬�Ƿ�һ��
#define TIMER_QUERY_POWER_DISSIPATION     EV_TIMER_3  // ���ڲ�ѯ���幦��

PowerAgent *PowerAgent::_instance = NULL;

static void InitDecodeMxStateMap()
{
    host_state_map.insert(make_pair(("01"), M0_NotInstall));
    host_state_map.insert(make_pair(("02"), M1_Inactive));
    host_state_map.insert(make_pair(("04"), M2_ActivationRequest));
    host_state_map.insert(make_pair(("08"), M3_ActivationInProcess));
    host_state_map.insert(make_pair(("10"), M4_Active));
    host_state_map.insert(make_pair(("20"), M5_DeactivationRequest));
    host_state_map.insert(make_pair(("40"), M6_DeactivationInProcess));
    host_state_map.insert(make_pair(("80"), M7_CommunicationLost));
}
/******************************************************************************/
static int32 GetDecodeMxState(const string &result)
{
    map<string, int32>::iterator it;
    it = host_state_map.find(result);
    if (it != host_state_map.end())
    {
        return it->second;
    }
    return State_Unknown;
}
/******************************************************************************/
static void InitAtcaLogicSlotMap()
{
    atca_slot_map.insert(make_pair(1, 13));
    atca_slot_map.insert(make_pair(2, 11));
    atca_slot_map.insert(make_pair(3, 9));
    atca_slot_map.insert(make_pair(4, 7));
    atca_slot_map.insert(make_pair(5, 5));
    atca_slot_map.insert(make_pair(6, 3));
    atca_slot_map.insert(make_pair(7, 1));
    atca_slot_map.insert(make_pair(8, 2));
    atca_slot_map.insert(make_pair(9, 4));
    atca_slot_map.insert(make_pair(10, 6));
    atca_slot_map.insert(make_pair(11, 8));
    atca_slot_map.insert(make_pair(12, 10));
    atca_slot_map.insert(make_pair(13, 12));
    atca_slot_map.insert(make_pair(14, 14));

}
/******************************************************************************/
static int32 GeAtcaLogicSlot(const int32 &phy_slot)
{
    map<int32, int32>::iterator it;
    it = atca_slot_map.find(phy_slot);
    if (it != atca_slot_map.end())
    {
        return it->second;
    }

    return INVALID_BOARD_TYPE;
}

/******************************************************************************/
static bool CheckNetStateWithCmm(const string &cmm_ip)
{
    string result ;
    ostringstream cmd;
    
    cmd << " ping " << cmm_ip << " -c 1 -W 1 2>/dev/null";
    RunCmd(cmd.str(),result);
    if(result.npos == result.find("ttl"))
    {
       return false;
    }
    return true;
}

/******************************************************************************/
STATUS  HostPowerOperateTask::Execute()
{
    string result ;
    ostringstream cmd;
    ostringstream slot_info ;

    if(!CheckNetStateWithCmm(_cmm_ip))
    {
        return ERROR;
    }

    string operate;
    if(HostAtca::OPERATE_POWER_ON == _opt)
    {
        operate = HOST_POWERON_CMD;
    }
    else if(HostAtca::OPERATE_POWER_OFF == _opt)
    {
        operate = HOST_POWEROFF_CMD;
    }
    else
    {
        return ERROR;
    }
    
    slot_info << showbase << hex << ((0x40 + _logic_slot) * 2);
    // ��������
    cmd << IPMITOOL_PARA<< _cmm_ip<< CMM_UESR_PASSWORD<< slot_info.str()<<IPMITOOL_CMD<< operate; 
    cmd << " 2>&1";

    if(SUCCESS != RunCmd(cmd.str(),result))
    {
       return ERROR;
    }
    
    return SUCCESS;
}
/******************************************************************************/
STATUS  HostPowerQueryTask::Execute()
{
    //Print();
    string result, result1;
    ostringstream cmd;
    ostringstream slot_info ;

    HostAtca *_host_atca;
    if (NULL == (_host_atca = HostAtca::GetInstance()))
    {
        return ERROR;
    }    
    if(!CheckNetStateWithCmm(_cmm_ip))
    {
        return ERROR;
    }
    slot_info << showbase << hex << ((0x40 + _logic_slot) * 2);
    // ��������
    cmd << IPMITOOL_PARA<< _cmm_ip<< CMM_UESR_PASSWORD<< slot_info.str()<<IPMITOOL_CMD<< HOST_QUERY_CMD << FRUID ; 
    cmd << " 2>/dev/NULL";
    if(SUCCESS != RunCmd(cmd.str(),result))
    {
       return ERROR;
    }

    
    int32 board_state_cmm; //cmm��¼�ĵ���״̬
    if( (result.find("rsp=0xc3") != string::npos)
        || (result.find("Timeout") != string::npos) ) // ��ʱ��Ϊ�ǵ��岻��λ
    {
        board_state_cmm = M7_CommunicationLost; // M7  Ҳ�ǿ��Ե�
    }
    else if(result.substr(0, 2) != string("00"))// ����ִ��ʧ��
    {
        board_state_cmm = State_Unknown; 
    }
    else 
    {
        board_state_cmm = GetDecodeMxState(result.substr(6,2));
    }

    HostAtca::Power_State board_state; 
    switch (board_state_cmm)
    {
        case M0_NotInstall:
        case M7_CommunicationLost:
        {
            board_state = HostAtca::BOARD_ABSENT;
            break;
        }

        case M1_Inactive:
        {
            board_state = HostAtca::BOARD_POWER_OFF;
            break;
        }

        case M4_Active:
        {
            board_state = HostAtca::BOARD_POWER_ON;
            break;
        }

        default:
            board_state = HostAtca::UNKOUWN;
            break;
        
    }

    _host_atca->UpdatePowerState(_hid,board_state);
    return SUCCESS;
}

/******************************************************************************/
STATUS HostPowerDissipationQueryTask::Execute()
{
    string result;
    ostringstream slot_info, cmd;

    HostAtca *host_atca = HostAtca::GetInstance();
    if (NULL == host_atca)
        return ERROR;

    int32 state = HostAtca::UNKOUWN;
    int32 rate_power = 0; // ����λ���µ繦�ʸ���Ϊ0
    if ((SUCCESS != host_atca->GetPoweStateByHostId(_hid, state))
        || (HostAtca::UNKOUWN == state))
    {
        return ERROR;
    }
        
    if ((HostAtca::BOARD_ABSENT == state)
        || (HostAtca::BOARD_POWER_OFF == state))
    {
        host_atca->UpdateRatePower(_hid, rate_power);
        return SUCCESS;
    }

    if (false == CheckNetStateWithCmm(_cmm_ip))
    {
        return ERROR;
    }

    slot_info << showbase << hex << ((0x40 + _logic_slot) * 2);
    // ��������
    cmd << IPMITOOL_PARA << _cmm_ip
        << CMM_UESR_PASSWORD << slot_info.str()
        << IPMITOOL_CMD << HOST_POWER_DISSIPATION; 
    cmd << " 2>/dev/NULL";
    if ((SUCCESS != RunCmd(cmd.str(), result))
        || (true == result.empty()))
    {
       return ERROR;
    }

    /* ipmitool���ǰ�completion code�����ģ�dc�Ѿ��ǵڶ����ֽ��ˡ� 00 a4 �ǹ���
     * [root@atca_1_1_1_1 /]# ipmitool -I lan -H 138.0.0.249 -U "" -P zte -t 0x92 raw 0x2c 0x02 0xdc 0x0 0x0 0x0
     * dc 00 a4 ff ff ff ff ff ff d9 c1 95 4c ff ff ff
     * ff 40
     */
    string str_power = result.substr(3, 2) + result.substr(6, 2);
    from_string<int>(rate_power, str_power, hex);
    host_atca->UpdateRatePower(_hid, rate_power);
    return SUCCESS;
}

/******************************************************************************/
STATUS FantrayRPMCtlTask::Execute()
{
    /* ������ȵ� IPMB ��ַ
     * 1: 0x21*2; 2: 0x22*2
     */
    ostringstream ipmb_addr, cmd;
    ipmb_addr << showbase << hex << ((0x20 + _fan_index) * 2);

    /* ����ִ��ǰ, ��Ҫtelnet��½cmm���п����ֶ��͹ر��ֶ�,
     * �ⲿ����Ҫ�˹����, cmmset -l system -d fanmancontrol -v 1
     * 1 : �����ֶ�, 0 : �ر��ֶ�
     */
    cmd << IPMITOOL_PARA << _cmm_ip
        << CMM_UESR_PASSWORD << ipmb_addr.str()
        << IPMITOOL_CMD << FANTRAY_RPM_CONTROL
        << _rpm_level << " 2>/dev/NULL";
    string result;
    if ((SUCCESS != RunCmd(cmd.str(), result))
        || (true == result.empty()))
    {// ����ִ�гɹ�����( 00)
       return ERROR;
    }
    return SUCCESS;
}

/******************************************************************************/
PowerAgent::PowerAgent():_mu(NULL),_have_startup(false)
{
    _thread_pool = NULL;
    InitDecodeMxStateMap();
    InitAtcaLogicSlotMap();
}
/******************************************************************************/
STATUS PowerAgent::StartMu(const string& name)
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
void PowerAgent::MessageEntry(const MessageFrame &message)
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
            case TIMER_CHECK_POWER_STATE:
            {
                DoCheckPowerStateTimer();
                break;
            }

            
            case TIMER_QUERY_HOST_STATE:
            {
                DoQueryHostStateTimer();
                break;
            }

            case EV_HOST_RESET_REQ:
            {
                DoHostResetProc(message);
                break;
            }

            case TIMER_QUERY_POWER_DISSIPATION:
            {
                DoQueryPowerDissipation();
                break;
            }

            case EV_FANTRAY_RPMCTL_REQ:
            {
                DoFantrayRPMCtl(message);
                break;
            }

            default:
            {
                cout << "PowerAgent receives an unkown message!" << endl;
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
void PowerAgent::DoStartUp()
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

    TIMER_ID    timer;
    TimeOut     timeout;

    if (INVALID_TIMER_ID == (timer = _mu->CreateTimer()))
    {
        OutPut(SYS_EMERGENCY, "Create timer for PowerAgent check power state failed!\n");
        SkyAssert(INVALID_TIMER_ID != timer);
        SkyExit(-1, "PowerAgent::DoStartUp: Create timer for PowerAgent info "
                "check power state failed.");
    }
    timeout.type = LOOP_TIMER;
    timeout.duration = 10 * 1000;
    timeout.msgid = TIMER_CHECK_POWER_STATE;
    if (SUCCESS != (ret = _mu->SetTimer(timer, timeout)))
    {
        OutPut(SYS_EMERGENCY, "Set timer for check  power state failed!\n");
        SkyAssert(SUCCESS == ret);
        SkyExit(-1, "PowerAgent::DoStartUp: Set timer for cluster info report "
                "failed.");
    }

    timeout.type = LOOP_TIMER;
    timeout.duration = 10 * 1000;
    timeout.msgid = TIMER_QUERY_HOST_STATE;
    if (SUCCESS != _mu->SetTimer(_mu->CreateTimer(), timeout))
    {
        OutPut(SYS_EMERGENCY, "Create query host state from cmm timer!\n");
        SkyAssert(0);
        SkyExit(-1, "PowerAgent::DoStartUp: Create timer get host state from cmm fail. ");
    }

    timeout.type = LOOP_TIMER;
    timeout.duration = 5 * 60 * 1000;
    timeout.msgid = TIMER_QUERY_POWER_DISSIPATION;
    if (SUCCESS != _mu->SetTimer(_mu->CreateTimer(), timeout))
    {
        OutPut(SYS_EMERGENCY, "Create timer for querying power dissipation failed!\n");
        SkyAssert(0);
        SkyExit(-1, "PowerAgent::DoStartUp: Create timer for querying power dissipation failed.");
    }
    
    if (SUCCESS != Transaction::Enable(GetDB()))
    {
        OutPut(SYS_ERROR,"PowerAgent enable transaction fail! ret = %d",ret);
        SkyExit(-1, "PowerAgent::MessageEntry: call "
                "Transaction::Enable(GetDB()) failed.");
    }

    CheckCmmUserName();
}
/******************************************************************************/
int32 PowerAgent::DoGetAssignWorkPara(const int64 &hid,string &cmm_ip,int32 &logic_slot)
{
    string shelf_type;    
    host_atca_table_info info;

    _host_atca->SearchByHostId(info,hid);
    if(!info.host_position.Validate())
    {
        return ERROR;
    }

    cmm_ip = _cmm_config->GetCmmIP(info.host_position.bureau,
                                   info.host_position.rack,
                                   info.host_position.shelf);

    shelf_type = _cmm_config->GetShelfType(info.host_position.bureau,
                                           info.host_position.rack,
                                           info.host_position.shelf);
                                           
    transform(shelf_type.begin(), shelf_type.end(), shelf_type.begin(), ::toupper);    

    logic_slot = 0;
    if("ETCA" == shelf_type)
    {
        logic_slot = info.host_position.slot;
    }
    else if("ATCA" == shelf_type)
    {
        logic_slot = GeAtcaLogicSlot(info.host_position.slot);
    }

    if(0 == logic_slot)
    {
        return ERROR;
    }

    return SUCCESS;
    
}
/******************************************************************************/
void PowerAgent::DoQueryHostStateTimer()
{
    vector<int64> hids;
    _host_atca->GetAllQueryHosts(hids);

    vector<int64>::iterator iter = hids.begin();
    for ( ; iter != hids.end(); ++iter)
    {   
        string cmm_ip;
        int32  logic_slot = 0;
        if(ERROR == DoGetAssignWorkPara(*iter,cmm_ip,logic_slot))
        {
            continue;
        }
        
        HostPowerQueryTask *task = new HostPowerQueryTask("HostPowerQueryTask",*iter,logic_slot,cmm_ip);
        if (task)
        {
            _thread_pool->AssignWork(task);
        }
    }

    return;
}
/******************************************************************************/
void PowerAgent::DoCheckPowerStateTimer()
{
    vector<int64> hids;
    _host_atca->GetPowerStateNotMatchHost(hids);

    vector<int64>::iterator iter = hids.begin();
    for ( ; iter != hids.end(); ++iter)
    {     
        int32 power_operate;
        if(ERROR == _host_atca->GetPowerOperateByHostId(*iter,power_operate))
        {
            continue;
        }

        if((power_operate != HostAtca::OPERATE_POWER_ON)&&(power_operate != HostAtca::OPERATE_POWER_OFF))
        {
            continue;
        }

        string cmm_ip;
        int32  logic_slot = 0;
        if(ERROR == DoGetAssignWorkPara(*iter,cmm_ip,logic_slot))
        {
            continue;
        }
        
        HostPowerOperateTask *task = new HostPowerOperateTask("HostPowerOperateTask",
                                                     *iter,logic_slot,cmm_ip,power_operate);
        if (task)
        {
            _thread_pool->AssignWork(task);
        }
    }

    return;
}
/******************************************************************************/
void PowerAgent::DoHostResetProc(const MessageFrame &message)
{
    MessageHostResetReq req;
    req.deserialize(message.data);

    MessageHostResetAck ack;
    ack._hid = req._hid;
    ack.action_id = req.action_id;
    
    MessageOption option(message.option.sender(), EV_HOST_RESET_ACK, 0, 0);

    int32 state;
    if(SUCCESS != _host_atca->GetPoweStateByHostId(req._hid,state))
    {
        ack.state = ERR_HOST_NOT_SUPPORT_RESET;
        _mu->Send(ack, option);
        return ;        
        
    }

    if(HostAtca::BOARD_POWER_ON != state)
    {
        ack.state = ERR_HOST_NOT_SUPPORT_RESET;
        _mu->Send(ack, option);
        return ;               
    }

    string cmm_ip;
    int32  logic_slot = 0;
    if(ERROR == DoGetAssignWorkPara(req._hid,cmm_ip,logic_slot))
    {
        ack.state = ERR_HOST_NOT_SUPPORT_RESET;
        _mu->Send(ack, option);    
        return;
    }
    
    HostPowerOperateTask *task = new HostPowerOperateTask("HostPowerOperateTask",
                                                 req._hid,logic_slot,cmm_ip,HostAtca::OPERATE_RESET);
    if (task)
    {
        _thread_pool->AssignWork(task);
    }

    ack.state = ERR_POWER_AGENT_SUCCESS;
    _mu->Send(ack, option);


}

/******************************************************************************/
void PowerAgent::CheckCmmUserName()
{
    /* ��;: ����invalid user name��������ָ���
     * ����: power_agent��Ϣ��Ԫ�ϵ�ʱ,�������е�CMM,
     *       ʹ���˺� -U "" -P zte ��1�۽��в�ѯ, 
     *       -U zte -P zte �� -U "" -P zte �ܹ�����
     */
    vector<CmmConfig> cmm_infos;
    _cmm_config->Search(cmm_infos, "");
    vector<CmmConfig>::iterator iter;
    int32  logic_slot;
    string result;
    ostringstream cmd, slot_info;
    for (iter  = cmm_infos.begin(); 
         iter != cmm_infos.end(); 
         ++iter)
    {
        if (false == CheckNetStateWithCmm((*iter)._ip_address))
            continue;

        transform((*iter)._type.begin(), (*iter)._type.end(), (*iter)._type.begin(), ::toupper);
        if ("ETCA" == (*iter)._type)
        {
            logic_slot = 1;
        }
        else if ("ATCA" == (*iter)._type)
        {
            logic_slot = 13;
        }
        else
        {
            continue;
        }
        slot_info << showbase << hex << ((0x40 + logic_slot) * 2);
        // ��������
        cmd << IPMITOOL_PARA << (*iter)._ip_address 
            << " -U \"\" -P zte -t " << slot_info.str()
            << IPMITOOL_CMD << HOST_QUERY_CMD << FRUID;
        cmd << " 2>/dev/NULL";
        cout << cmd.str() << endl;
        RunCmd(cmd.str(), result);
        cmd.str("");
        slot_info.str("");
    }
}

/******************************************************************************/
void PowerAgent::DoQueryPowerDissipation()
{
    vector<int64> vec_hid;
    _host_atca->GetAllQueryHosts(vec_hid);

    string cmm_ip;
    int32  logic_slot = 0;
    vector<int64>::iterator it;
    for (it  = vec_hid.begin();
         it != vec_hid.end();
         it++)
    {// ������йܸ� HostPowerDissipationQueryTask
        cmm_ip.clear();
        logic_slot = 0;
        if (ERROR == DoGetAssignWorkPara(*it, cmm_ip, logic_slot))
            continue;

        HostPowerDissipationQueryTask *task
                = new HostPowerDissipationQueryTask("HostPowerDissipationQueryTask",
                                                    *it, logic_slot, cmm_ip);
        if (task)
        {
            _thread_pool->AssignWork(task);
        }
    }
}

/******************************************************************************/
void PowerAgent::DoFantrayRPMCtl(const MessageFrame &message)
{
    MessageFanRPMCtrlCmdReq msg_req;
    msg_req.deserialize(message.data);

    MessageFanRPMCtrlCmdAck msg_ack;
    msg_ack._ret_code = ERR_POWER_AGENT_SUCCESS;
    MessageOption option(message.option.sender(),
                         EV_FANTRAY_RPMCTL_ACK, 0, 0);

    string cmm_ip = _cmm_config->GetCmmIP(msg_req._cmm_bureau,
                                          msg_req._cmm_rack,
                                          msg_req._cmm_shelf);
    if (cmm_ip.empty())
    {
        msg_ack._ret_code = ERR_CMM_IP_NULL;
    }
    else if (false == CheckNetStateWithCmm(cmm_ip))
    {
        msg_ack._ret_code = ERR_CMM_CFG_INCORRECT;
    }
    else
    {
        FantrayRPMCtlTask *task = new FantrayRPMCtlTask("FantrayRPMCtlTask", 
                                                        cmm_ip,
                                                        msg_req._fantray_index,
                                                        msg_req._rpm_level);

        if (task)
            _thread_pool->AssignWork(task);
    }

    _mu->Send(msg_ack, option);
}

} // namespace zte_tecs

