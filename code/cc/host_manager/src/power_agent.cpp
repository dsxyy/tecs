/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：power_agent.cpp
* 文件标识：见配置管理计划书
* 内容摘要：power_agent类实现文件
* 当前版本：1.0
* 作    者：liuyi
* 完成日期：2013年4月19日
*
* 修改记录1：
*     修改日期：2013/4/19
*     版 本 号：V1.0
*     修 改 人：liuyi
*     修改内容：创建
*******************************************************************************/
#include "sky.h"
#include "tecs_config.h"
#include "config_cluster.h"
#include "power_agent.h"
#include <fstream>

static map<int32,int32>    atca_slot_map;     //atca物理槽位与逻辑槽位映射关系
static map<string,int32>   host_state_map;    //cmm查询返回值sting与状态的映射关系


namespace zte_tecs 
{

// 定时器消息号
#define TIMER_QUERY_HOST_STATE      EV_TIMER_1  // 周期组播归属的tece系统名称
#define TIMER_CHECK_POWER_STATE     EV_TIMER_2  // 周期检查操作状态与cmm状态是否一致
#define TIMER_QUERY_POWER_DISSIPATION     EV_TIMER_3  // 周期查询单板功耗

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
    // 计算命令
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
    // 计算命令
    cmd << IPMITOOL_PARA<< _cmm_ip<< CMM_UESR_PASSWORD<< slot_info.str()<<IPMITOOL_CMD<< HOST_QUERY_CMD << FRUID ; 
    cmd << " 2>/dev/NULL";
    if(SUCCESS != RunCmd(cmd.str(),result))
    {
       return ERROR;
    }

    
    int32 board_state_cmm; //cmm记录的单板状态
    if( (result.find("rsp=0xc3") != string::npos)
        || (result.find("Timeout") != string::npos) ) // 超时，为是单板不在位
    {
        board_state_cmm = M7_CommunicationLost; // M7  也是可以的
    }
    else if(result.substr(0, 2) != string("00"))// 命令执行失败
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
    int32 rate_power = 0; // 不在位或下电功率更新为0
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
    // 计算命令
    cmd << IPMITOOL_PARA << _cmm_ip
        << CMM_UESR_PASSWORD << slot_info.str()
        << IPMITOOL_CMD << HOST_POWER_DISSIPATION; 
    cmd << " 2>/dev/NULL";
    if ((SUCCESS != RunCmd(cmd.str(), result))
        || (true == result.empty()))
    {
       return ERROR;
    }

    /* ipmitool里是把completion code屏掉的，dc已经是第二个字节了。 00 a4 是功率
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
    /* 计算风扇的 IPMB 地址
     * 1: 0x21*2; 2: 0x22*2
     */
    ostringstream ipmb_addr, cmd;
    ipmb_addr << showbase << hex << ((0x20 + _fan_index) * 2);

    /* 命令执行前, 需要telnet登陆cmm进行开启手动和关闭手动,
     * 这部分需要人工完成, cmmset -l system -d fanmancontrol -v 1
     * 1 : 开启手动, 0 : 关闭手动
     */
    cmd << IPMITOOL_PARA << _cmm_ip
        << CMM_UESR_PASSWORD << ipmb_addr.str()
        << IPMITOOL_CMD << FANTRAY_RPM_CONTROL
        << _rpm_level << " 2>/dev/NULL";
    string result;
    if ((SUCCESS != RunCmd(cmd.str(), result))
        || (true == result.empty()))
    {// 命令执行成功返回( 00)
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
    
    // 消息单元的创建和初始化
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

    //给自己发送上电消息，促使消息单元状态切换到工作态
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
    /* 用途: 降低invalid user name的问题出现概率
     * 方案: power_agent消息单元上电时,遍历所有的CMM,
     *       使用账号 -U "" -P zte 对1槽进行查询, 
     *       -U zte -P zte 跟 -U "" -P zte 能够互解
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
        // 计算命令
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
    {// 处理均托管给 HostPowerDissipationQueryTask
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

