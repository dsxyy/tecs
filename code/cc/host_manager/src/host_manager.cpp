/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：host_manager.cpp
* 文件标识：见配置管理计划书
* 内容摘要：HostManager类实现文件
* 当前版本：1.0
* 作    者：袁进坤
* 完成日期：2011年8月9日
*
* 修改记录1：
*     修改日期：2011/8/9
*     版 本 号：V1.0
*     修 改 人：袁进坤
*     修改内容：创建
*******************************************************************************/
#include "sky.h"
#include "host_manager.h"
#include "tecs_config.h"
#include "alarm_agent.h"
#include "vstorage_manager.h"
#include "config_cluster.h"
#include "light_agent.h"
#include "vm_manager.h"
// 与本模块交互的消息结构定义
#include "cluster_manager_with_host_manager.h"
#include "msg_host_manager_with_api_method.h"
#include "msg_host_manager_with_host_agent.h"
#include "msg_host_manager_with_host_handler.h"
#include "msg_host_manager_with_high_available.h"
#include "dns_building.h"
 
#include <fstream>

namespace zte_tecs 
{

// 定时器消息号
#define TIMER_HOST_RUNSTATE_CHECK   EV_TIMER_1  // 检查主机是否离线
#define TIMER_HOST_HA_CHECK         EV_TIMER_2  // 检查是否是否有主机需要触发HA
#define TIMER_HOST_CPU_USEAGE       EV_TIMER_3  // 统计物理机各种粒度的CPU利用率
#define TIMER_HOST_STATISTICES_SCAN EV_TIMER_4  // 物理机资源统计信息定时扫描清理

extern int32 statistics_reserve;

HostManager *HostManager::_instance = NULL;

/******************************************************************************/
HostManager::HostManager():
        _have_startup(false),
        _tecs_name()
{
    _mu = NULL;
}

/************************************************************
* 函数名称： StartMu
* 功能描述： 启动mu的函数
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2012/5         V1.0       lixiaocheng         创建
***************************************************************/
STATUS HostManager::StartMu(const string& name)
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
        OutPut(SYS_EMERGENCY, "SetHandler mu %s failed! ret = %d\n",
               name.c_str(),ret);
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
        OutPut(SYS_EMERGENCY, "Register mu %s failed! ret = %d\n", name.c_str(),
               ret);
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
void HostManager::MessageEntry(const MessageFrame &message)
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
            case EV_HOST_SET_REQ:
            {
                DoHostSet(message);
                break;
            }
            case EV_HOST_FORGET_REQ:
            {
                DoHostForget(message);
                break;
            }
            case EV_HOST_DISABLE_REQ:
            {
                DoHostDisable(message);
                break;
            }
            case EV_HOST_ENABLE_REQ:
            {
                DoHostEnable(message);
                break;
            }
            case EV_HOST_REBOOT_REQ:
            {
                DoHostRebootReq(message);
                break;
            }
            case EV_HOST_SHUTDOWN_REQ:
            {
                DoHostShutdownReq(message);
                break;
            }
            case EV_HOST_DEF_HANDLE_ACK:
            {
                DoHostDefHandleAck(message);
                break;
            }
            case EV_HOST_QUERY_REQ:
            {
                DoHostQueryReq(message);
                break;
            }        
            case EV_HOST_DISCOVER:
            {
                DoHostDiscover(message);
                break;
            }
        case EV_HOST_INFO_REPORT:
        {
            DoHostInfoReport(message);
            break;
        }
        case EV_HOST_HEARTBEAT_REPORT:
        {
            DoHeartbeatReport(message);
            break;
        }
        case TIMER_HOST_RUNSTATE_CHECK:
        {
            DoHostRunStateCheck();
            break;
        }
        case TIMER_HOST_CPU_USEAGE:
        {
            DoHostCpuUsageCalc();
            break;
        }
        case TIMER_HOST_HA_CHECK:
        {
            DoHostHaCheck();
            break;
        }
        case TIMER_HOST_STATISTICES_SCAN:
        {
            DoScanForDrop();
            break;
        }
        
        
        case EV_REGISTERED_SYSTEM:
        {
            SetClusterName(message);
            break;
        }        
        
        case EV_REGISTERED_SYSTEM_QUERY:
        {
            break;
        }
        
        default:
        {
            cout << "HostManager receives an unkown message!" << endl;
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
void HostManager::DoStartUp()
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

    string lastwords;
    // 加入主机发现组播组，接收主机发现消息
    if (SUCCESS != (ret = _mu->JoinMcGroup(MUTIGROUP_HOST_DISCOVER)))
    {
        SkyAssert(false);
        lastwords = "HostManager join host discover group failed! ret = "
                    + to_string(ret, dec) + "\n";
        OutPut(SYS_EMERGENCY, lastwords);
        SkyExit(-1, lastwords);
    }

    // 创建HostManager需要的定时器
    TIMER_ID    timer;
    TimeOut     timeout;

    // 设置定时器: 用于维护主机的运行状态是否离线
    if (INVALID_TIMER_ID == (timer = _mu->CreateTimer()))
    {
        SkyAssert(false);
        lastwords = "HostManager Create timer for host runstate check failed!\n";
        OutPut(SYS_EMERGENCY, lastwords);
        SkyExit(-1, lastwords);
    }
    timeout.type = LOOP_TIMER;
    timeout.duration = 5 * 1000;
    timeout.msgid = TIMER_HOST_RUNSTATE_CHECK;
    if (SUCCESS != (ret = _mu->SetTimer(timer, timeout)))
    {
        SkyAssert(false);
        lastwords = "HostManager Set timer for host runstate check failed!\n";
        OutPut(SYS_EMERGENCY, lastwords);
        SkyExit(-1, lastwords);  
    }

    // 设置定时器: 用于统计主机的物理CPU使用率
    if (INVALID_TIMER_ID == (timer = _mu->CreateTimer()))
    {
        SkyAssert(false);
        lastwords = "HostManager Create timer for calc host cpu usage failed!\n";
        OutPut(SYS_EMERGENCY, lastwords);
        SkyExit(-1, lastwords);
    }
    timeout.type = LOOP_TIMER;
    timeout.duration = 60 * 1000;
    timeout.msgid = TIMER_HOST_CPU_USEAGE;
    if (SUCCESS != (ret = _mu->SetTimer(timer, timeout)))
    {
        SkyAssert(false);
        lastwords = "HostManager Set timer for calc host cpu usage failed!\n";
        OutPut(SYS_EMERGENCY, lastwords);
        SkyExit(-1, lastwords);   
    }

    // 设置定时器: 用于进行高可用性的周期检查
    if (INVALID_TIMER_ID == (timer = _mu->CreateTimer()))
    {
        SkyAssert(false);
        lastwords = "HostManager Create timer for high available check failed!\n";
        OutPut(SYS_EMERGENCY, lastwords);
        SkyExit(-1, lastwords);
    }
    timeout.type = LOOP_TIMER;
    timeout.duration = 30 * 1000;
    timeout.msgid = TIMER_HOST_HA_CHECK;
    if (SUCCESS != (ret = _mu->SetTimer(timer, timeout)))
    {
        SkyAssert(false);
        lastwords = "HostManager Set timer for high available check failed!\n";
        OutPut(SYS_EMERGENCY, lastwords);
        SkyExit(-1, lastwords);  
    }

    // 设置定时器: 用于物理机扫描清理 host_statistics 的记录
    if (INVALID_TIMER_ID == (timer = _mu->CreateTimer()))
    {
        SkyAssert(false);
        lastwords = "HostManager Create timer for host statistics scan failed!\n";
        OutPut(SYS_EMERGENCY, lastwords);
        SkyExit(-1, lastwords);
    }
    timeout.type = LOOP_TIMER;
    timeout.duration = 60 * 60 * 1000; // 1h
    timeout.msgid = TIMER_HOST_STATISTICES_SCAN;
    if (SUCCESS != (ret = _mu->SetTimer(timer, timeout)))
    {
        SkyAssert(false);
        lastwords = "HostManager Set timer for host statistics scan failed!\n";
        OutPut(SYS_EMERGENCY, lastwords);
        SkyExit(-1, lastwords);   
    }

    // 加入归属信息组播组
    ret = _mu->JoinMcGroup(string(ApplicationName()) + MUTIGROUP_SUF_REG_SYSTEM);
    if (SUCCESS != ret)
    {
        SkyAssert(false);
        lastwords = "HostManager join reg group failed! ret = "
                    + to_string(ret, dec) + "\n";
        OutPut(SYS_EMERGENCY, lastwords);
        SkyExit(-1, lastwords);
    }

    if (SUCCESS != Transaction::Enable(GetDB()))
    {
        SkyAssert(false);
        lastwords = "HostManager enable transaction fail! ret = "
                    + to_string(ret, dec) + "\n";
        OutPut(SYS_ERROR, lastwords);
        SkyExit(-1, lastwords);
    }

    /* CC 所在物理机重启后 HC 不会再发 DISCOVER 消息
     * 也就不会开启 ntpd 和 dnsmasq 服务, 故此处增加处理
     */
    string cmd = "service ntpd start 2>/dev/null";
    string result;
    ret = RunCmd(cmd, result);
    if (SUCCESS != ret)
    {
        lastwords = "HostManager start ntpd service failed! ret = ";
        lastwords.append(to_string(ret,dec));
        OutPut(SYS_ERROR, lastwords);
        SkyExit(-1, lastwords);
    }
    cmd.clear();
    result.clear();
    cmd = "service dnsmasq start 2>/dev/null";
    ret = RunCmd(cmd, result);
    if (SUCCESS != ret)
    {
        lastwords = "HostManager start dnsmasq service failed! ret = ";
        lastwords.append(to_string(ret,dec));
        OutPut(SYS_ERROR, lastwords);
        SkyExit(-1, lastwords);
    }
    // 高可用检查, 上电即触发, 加快检查的速度
    DoHostHaCheck();
}


/******************************************************************************/
void HostManager::DoHostSet(const MessageFrame &message)
{
    MessageHostSetReq req;
    req.deserialize(message.data);

    MessageHostSetAck ack;
    ack._host_name = req._host_name;

    MessageOption option(message.option.sender(), EV_HOST_SET_ACK, 0, 0);

    if (message.option.sender()._application != _tecs_name)
    {
        ack._ret_code = ERR_CLUSTER_NOT_REGISTER;
        _mu->Send(ack, option);
        return;
    }

    if (req._host_name.empty())
    {
        ack._ret_code = ERR_HOST_NOT_EXIST;
        _mu->Send(ack, option);
        return;
    }

    Host *host = _host_pool->Get(req._host_name, true);
    if (NULL == host)
    {// 获取不到不会上锁
        ack._ret_code = ERR_HOST_NOT_EXIST;
        _mu->Send(ack, option);
        return;
    }

    // 修改描述
    if (host->get_description() != req._description)
    {
        host->set_description(req._description);
        _host_pool->Update(host);
    }
    host->UnLock();

    ack._ret_code = ERR_HOST_OK;
    _mu->Send(ack, option);

    return;
}

/******************************************************************************/
void HostManager::DoHostForget(const MessageFrame &message)
{
    MessageHostForgetReq req;
    req.deserialize(message.data);

    MessageHostForgetAck ack;
    ack._host_name = req._host_name;

    MessageOption option(message.option.sender(), EV_HOST_FORGET_ACK, 0, 0);

    if (message.option.sender()._application != _tecs_name)
    {
        ack._ret_code = ERR_CLUSTER_NOT_REGISTER;
        _mu->Send(ack, option);
        return;
    }

    Host *host = _host_pool->Get(req._host_name, true);
    if (NULL == host)
    {
        ack._ret_code = ERR_HOST_NOT_EXIST;
        _mu->Send(ack, option);
        return;
    }
    
    if (0 != _host_pool->GetHostRunningVms(host))
    {
        ack._ret_code = ERR_HOST_VMS_RUNNING;
    }
    else
    {
        //尝试恢复告警
        SendAlarmResume(host);
        // 将主机从系统中销毁掉
        _host_pool->Drop(host);

        ack._ret_code = ERR_HOST_OK;
    }
    host->UnLock();

    // 应答处理结果
    _mu->Send(ack, option);
}

/******************************************************************************/
void HostManager::DoHostDisable(const MessageFrame &message)
{
    MessageHostDisableReq req;
    req.deserialize(message.data);

    MessageHostDisableAck ack;
    ack._host_name = req._host_name;

    MessageOption option(message.option.sender(), EV_HOST_DISABLE_ACK, 0, 0);

    if (message.option.sender()._application != _tecs_name)
    {
        ack._ret_code = ERR_CLUSTER_NOT_REGISTER;
        _mu->Send(ack, option);
        return;
    }

    PoolObjectSQLPointer<Host> stup(_host_pool->Get(req._host_name,true), true);
    Host *host = stup.Object();
    if (NULL == host)
    {
        ack._ret_code = ERR_HOST_NOT_EXIST;
    }
    else if (0 != _host_pool->GetHostRunningVms(host))
    {
        if (req._is_forced)
        {
            SetHostIsDisabled(host, true);
            ack._ret_code = ERR_HOST_FORCED_DISABLE;
        }
        else
        {
            ack._ret_code = ERR_HOST_VMS_RUNNING;
        }
    }
    else
    {
        SetHostIsDisabled(host, true);
        ack._ret_code = ERR_HOST_OK;
    }

    _mu->Send(ack, option);
}

/******************************************************************************/
void HostManager::DoHostEnable(const MessageFrame &message)
{
    MessageHostEnableReq req;
    req.deserialize(message.data);

    MessageHostEnableAck ack;
    ack._host_name = req._host_name;

    MessageOption option(message.option.sender(), EV_HOST_ENABLE_ACK, 0, 0);

    if (message.option.sender()._application != _tecs_name)
    {
        ack._ret_code = ERR_CLUSTER_NOT_REGISTER;
        _mu->Send(ack, option);
        return;
    }

    PoolObjectSQLPointer<Host> stup(_host_pool->Get(req._host_name,true), true);
    Host *host = stup.Object();
    if (NULL == host)
    {
        ack._ret_code = ERR_HOST_NOT_EXIST;
    }
    else
    {
        SetHostIsDisabled(host, false);
        ack._ret_code = ERR_HOST_OK;
    }

    _mu->Send(ack, option);
}
/******************************************************************************/
void HostManager::DoHostRebootReq(const MessageFrame &message)
{
    MessageHostRebootReq req;
    req.deserialize(message.data);

    MessageHostRebootAck ack;
    ack._host_name = req._host_name;

    MessageOption option(message.option.sender(), EV_HOST_REBOOT_ACK, 0, 0);

    if (message.option.sender()._application != _tecs_name)
    {
        ack._ret_code = ERR_CLUSTER_NOT_REGISTER;
        _mu->Send(ack, option);
        return;
    }

    Host *host = _host_pool->Get(req._host_name, false);
    if (NULL == host)
    {
        ack._ret_code = ERR_HOST_NOT_EXIST;
        _mu->Send(ack, option);
    }
    else if ((0 != _host_pool->GetHostRunningVms(host)) && \
             (!req._is_forced))
    {// 此处不用加锁，只是象征性判断一下，不能完全保证
        ack._ret_code = ERR_HOST_VMS_RUNNING;
        _mu->Send(ack, option);
    }
    else
    {
        // 将重启消息发送给HC上HostHandler模块
        MessageHostDefHandleReq handle_req;
        handle_req._command = DEF_HANDLE_CMD_REBOOT;
        handle_req._argument = "";
        // 注意下面的赋值, 透传, 确保 API 应答的接收方
        handle_req._append = message.option.sender().serialize();

        MID receiver;
        receiver._application = req._host_name;
        receiver._process = PROC_HC;
        receiver._unit = MU_HOST_HANDLER;

        MessageOption handle_option(receiver, EV_HOST_DEF_HANDLE_REQ, 0, 0);
        _mu->Send(handle_req, handle_option);
        // 收到 HostHandler 的应答后再给 API 的应答
    }

    return;
}

/******************************************************************************/
void HostManager::DoHostShutdownReq(const MessageFrame &message)
{
    MessageHostShutdownReq req;
    req.deserialize(message.data);

    MessageHostShutdownAck ack;
    ack._host_name = req._host_name;

    MessageOption option(message.option.sender(), EV_HOST_SHUTDOWN_ACK, 0, 0);

    if (message.option.sender()._application != _tecs_name)
    {
        ack._ret_code = ERR_CLUSTER_NOT_REGISTER;
        _mu->Send(ack, option);
        return;
    }

    Host *host = _host_pool->Get(req._host_name, false);
    if (NULL == host)
    {
        ack._ret_code = ERR_HOST_NOT_EXIST;
        _mu->Send(ack, option);
    }
    else if (0 != _host_pool->GetHostRunningVms(host))
    {// 此处不用加锁，只是象征性判断一下，不能完全保证
        ack._ret_code = ERR_HOST_VMS_RUNNING;
        _mu->Send(ack, option);
    }
    else
    {
        // 将关机消息发送给HC上HostHandler模块
        MessageHostDefHandleReq handle_req;
        handle_req._command = DEF_HANDLE_CMD_SHUTDOWN;
        handle_req._argument = "";
        // 注意下面的赋值, 透传, 确保 API 应答的接收方
        handle_req._append = message.option.sender().serialize();

        MID receiver;
        receiver._application = req._host_name;
        receiver._process = PROC_HC;
        receiver._unit = MU_HOST_HANDLER;

        MessageOption handle_option(receiver, EV_HOST_DEF_HANDLE_REQ, 0, 0);
        _mu->Send(handle_req, handle_option);
        // 收到 HostHandler 的应答后再给 API 的应答
    }

    return;
}

/******************************************************************************/
void HostManager::DoHostDefHandleAck(const MessageFrame &message)
{
    MessageHostDefHandleAck ack;
    ack.deserialize(message.data);

    MID receiver;
    receiver.deserialize(ack._append);
    if (DEF_HANDLE_CMD_REBOOT == ack._command)
    {
        MessageHostRebootAck reboot_ack;
        reboot_ack._host_name = message.option.sender()._application;
        reboot_ack._ret_code = ack._ret_code;
        MessageOption option(receiver, EV_HOST_REBOOT_ACK, 0, 0);
        _mu->Send(reboot_ack, option);
    }
    else if (DEF_HANDLE_CMD_SHUTDOWN == ack._command)
    {
        MessageHostShutdownAck shutdown_ack;
        shutdown_ack._host_name = message.option.sender()._application;
        shutdown_ack._ret_code = ack._ret_code;
        MessageOption option(receiver, EV_HOST_SHUTDOWN_ACK, 0, 0);
        _mu->Send(shutdown_ack, option);
    }
}

/******************************************************************************/
void HostManager::DoHostDiscover(const MessageFrame &message)
{
    string host_name = message.option.sender()._application;

    // 解析收到的主机信息
    MessageHostBaseInfo msg;
    msg.deserialize(message.data);

    /* cpu_bench 的处理原则: 以第一个非 0 值为准
     * 思路:
     * 1) host_cpuinfo 如果没有记录, 则插入
     * 2) host_cpuinfo 如果有记录, 判断 cpu_bench, 0 则更新
     */
    int64 cpu_info_id = _host_cpuinfo->GetIdByInfo(msg._cpu_info._info);
    if (HostCpuInfo::INVALID_CPU_INFO_ID == cpu_info_id)
    {
        cpu_info_id = _host_cpuinfo->Insert(msg._cpu_info._info);
    }
    else
    {
        map<string, string>::iterator it;
        if (msg._cpu_info._info.end() != (it = msg._cpu_info._info.find("cpu_bench")))
        {
            string tmp = it->second;
            int64 cpu_bench_discover = atoi(tmp.c_str());
            int64 cpu_bench_db = _host_cpuinfo->GetCpuBenchByCpuId(cpu_info_id);
            if ((0 == cpu_bench_db) && (0 != cpu_bench_discover))
            {
                _host_cpuinfo->UpdateCpubenchByCpuId(cpu_bench_discover, cpu_info_id);
            }
        }
    }
    if (HostCpuInfo::INVALID_CPU_INFO_ID == cpu_info_id)
    {
        OutPut(SYS_ALERT, "Get ID from db or Insert new record into db failed!\n");
        return;
    }

    Host *host = _host_pool->Get(host_name, false);
    if (NULL == host)
    {
        int64 oid;
        string error_str;
        _host_pool->Allocate(&oid, host_name, cpu_info_id, error_str);
        if (!error_str.empty())
        {
            OutPut(SYS_ALERT, "Allocate isolate host failed!\n");
            SkyAssert(!error_str.empty());
            return;
        }
    }

    PoolObjectSQLPointer<Host> stup(_host_pool->Get(host_name, true), true);
    host = stup.Object();
    host->set_ip_address(msg._ip_address);
    host->set_media_address(msg._media_address);
    host->set_iscsiname(msg._iscsiname);
    host->set_vmm_info(msg._vmm_info);
    host->set_os_info(msg._os_info);
    host->set_running_time(msg._running_time);
    host->set_mem_total(msg._mem_total);
    host->set_mem_max(msg._mem_max);
    host->set_disk_total(msg._disk_total);
    host->set_disk_max(msg._disk_max);
    host->set_is_support_hvm(msg._is_support_hvm);
    host->SetCpuInfo(msg._cpu_info._info);
    host->set_ssh_key(msg._ssh_key);
    host->set_shelf_type(msg._shelf_type);
    host->set_cpus(msg._cpus);

    // 状态类信息需先恢复原来的告警
    if (true == host->get_mem_state())
    {
        SendAlarm(host, ALM_TECS_HOST_MEMORY_ERR, OAM_REPORT_RESTORE);
    }
    if (Host::OFF_LINE == host->get_run_state())
    {
        SendAlarm(host, ALM_TECS_HOST_OFF_LINE, OAM_REPORT_RESTORE);
    }
    host->set_mem_state(msg._mem_state);
    host->set_run_state(Host::ON_LINE);
    // 更新后还有问题重新告警
    if (true == host->get_mem_state())
    {
        SendAlarm(host, ALM_TECS_HOST_MEMORY_ERR, OAM_REPORT_ALARM);
    }

    // 处理主机的位置location信息，解析map<string,string>
    map<string,string>::iterator iter;
    string location;
    for (iter = msg._location.begin(); iter != msg._location.end(); ++iter)
    {
        location += "<" + iter->first + ">" + iter->second + "</" + iter->first + ">";
    }
    if (false == location.empty())
    {
        host->set_location(location);
    }
    OutPut(SYS_DEBUG, "Manage rev location in DoHostDiscover:  %s.\n", location.c_str());

    host->set_last_moni_time(time(0));
    _host_pool->Update(host); // 注意上述对 host 的操作均是内存, 并未入库

    //架框槽信息写入host_atca表
    if(msg._phy_position.Validate())
    {
        host_atca_table_info host_position_info;
        host_position_info.hid = host->GetOID();
        host_position_info.host_position = msg._phy_position;
        host_position_info.board_type = msg._board_type;
        _host_atca->Add(host_position_info);
    }

    if (_ifs_info.empty())
    {
        if (SUCCESS != GetAllIfsInfo(_ifs_info))
        {
            _ifs_info.clear();
        }
    }

    SendHostRegisterReq(host);

    host->UpdateDevices(msg._sdevice_info);
}

/******************************************************************************/
void HostManager::DoHostInfoReport(const MessageFrame &message)
{
    string host_name = message.option.sender()._application;

    // 主机合法性检查
    PoolObjectSQLPointer<Host> stup(_host_pool->Get(host_name, true), true);
    Host *host = stup.Object();
    if (NULL == host)
    {
        MessageHostUnregisterReq req;
        req._host_name = host_name;
        MID receiver;
        receiver._application = host_name;
        receiver._process = PROC_HC;
        receiver._unit = MU_HOST_AGENT;
        MessageOption option_hc(receiver, EV_HOST_UNREGISTER_REQ, 0, 0);
        _mu->Send(req, option_hc);
        return;
    }

    SetHostRunState(host, Host::ON_LINE);
    host->set_last_moni_time(time(0));    

    // 处理消息内容
    MessageHostInfoReport msg;
    msg.deserialize(message.data);

    // 处理CPU利用率
    host->RecCpuUsage(msg._cpu_usage);

    // 处理Disk信息
    SetHostDiskState(host, msg._vec_disk_info);

    SetHostOsciliatorState(host, msg._oscillator);
    host->set_running_time(msg._running_time);
    host->set_ip_address(msg._ip_address);
    
    _host_pool->Update(host);

    //架框槽信息更新
    if(msg._phy_position.Validate())
    {
        host_atca_table_info host_position_info;
        host_position_info.hid = host->GetOID();
        host_position_info.host_position = msg._phy_position;
        host_position_info.board_type = msg._board_type;
        _host_atca->Update(host_position_info);
    }
    else
    {
        _host_atca->Delete(host->GetOID());
    }

    // 传感器信息入库, 先删除, 后入库
    int64 hid = host->GetOID();
    _host_hwmon->Delete(hid);
    _host_hwmon->Insert(msg._vec_host_hwmon, hid);

    // 更新物理机使用资源统计信息
    SetStatisticsInfoToDB(msg._host_statistics, hid);

    HostFree host_free;
    _host_pool->GetHostFree(hid,host_free);

    if(host_free._free_tcu >= 0)
    {
        SendAlarm(host, ALM_TECS_FREE_CPU_ERR, OAM_REPORT_RESTORE);
    }
    else
    {
        SendAlarm(host, ALM_TECS_FREE_CPU_ERR, OAM_REPORT_ALARM);
    }

    if(host_free._mem_free >= 0)
    {
        SendAlarm(host, ALM_TECS_FREE_MEMORY_ERR, OAM_REPORT_RESTORE);
    }
	else
    {
        SendAlarm(host, ALM_TECS_FREE_MEMORY_ERR, OAM_REPORT_ALARM);
    }

    if(host_free._disk_free >= 0)
    {
        SendAlarm(host, ALM_TECS_FREE_DISK_ERR, OAM_REPORT_RESTORE);
    }
    else
    {
        SendAlarm(host, ALM_TECS_FREE_DISK_ERR, OAM_REPORT_ALARM);
    }

}

/******************************************************************************/
void HostManager::DoHeartbeatReport(const MessageFrame &message)
{
    string host_name = message.option.sender()._application;

    // 主机合法性检查
    PoolObjectSQLPointer<Host> stup(_host_pool->Get(host_name, true), true);
    Host *host = stup.Object();
    if ((NULL == host) )
    {
        return;
    }

    SetHostRunState(host, Host::ON_LINE);
    host->set_last_moni_time(time(0));
    _host_pool->Update(host);
}

/******************************************************************************/
void HostManager::DoHostRunStateCheck()
{
    // 查询所有超过30秒未收到上报消息的Registered主机
    vector<int64> oids;
    _host_pool->List(oids, 30);

    // 将这些主机置为离线状态
    vector<int64>::iterator it;
    for (it = oids.begin(); it != oids.end(); it++)
    {
        Host *host = _host_pool->Get(*it, true);
        if (NULL == host)
        {
            OutPut(SYS_ALERT, "DoHostRunStateCheck: get host %ld failed!\n", *it);
            SkyAssert(NULL != host);
            continue;
        }
        SetHostRunState(host, Host::OFF_LINE);
        _host_pool->Put(host, true);
    }
}

/******************************************************************************/
void HostManager::DoHostCpuUsageCalc()
{
    // 搜索所有状态OK的主机
    vector<int64> oids;
    ostringstream where;
    where << " run_state = " << Host::ON_LINE
    << " AND enabled = " << 0 ;
    _host_pool->Search(oids, where.str());

    // 计算这些主机的大粒度CPU利用率
    vector<int64>::iterator it;
    for (it = oids.begin(); it != oids.end(); it++)
    {
        Host *host = _host_pool->Get(*it, true);
        if (NULL == host)
        {
            OutPut(SYS_ALERT, "DoHostCpuUsageCalc: get host %ld failed!\n",*it);
            SkyAssert(NULL != host);
            continue;
        }

        host->CalcCpuUsage();
        _host_pool->Update(host);
        host->UnLock();
    }
}

/******************************************************************************/
void HostManager::DoHostHaCheck()
{
    vector<int64> oids;
    ostringstream where;
    where << " running_vms > 0 AND ( "
          << " run_state != " << Host::ON_LINE
          << " OR enabled != " << false << " )";
    _host_pool->Search(oids, where.str());

    vector<int64>::iterator it;
    for (it  = oids.begin();
         it != oids.end();
         it++)
    {
        Host *host = _host_pool->Get(*it, true);
        if (NULL == host)
        {
            OutPut(SYS_ALERT, "DoHostHaCheck: get host %ld failed!\n", *it);
            SkyAssert(NULL != host);
            continue;
        }
        SendHighAvailable(host, EV_HOST_OFFLINE);
        _host_pool->Put(host, true);
    }

    oids.clear();
    where.str("");
    // 仅检查在线且不处于维护状态的主机
    where << " running_vms > 0 "
          << " AND run_state = " << Host::ON_LINE
          << " AND enabled = " << false
          << " AND ( all_free_tcu < 0 "
          << " OR mem_free < 0 "
          << " OR hardware_state != 0 )";
    _host_pool->Search(oids, where.str());
    for (it  = oids.begin();
         it != oids.end();
         it++)
    {
        Host *host = _host_pool->Get(*it, true);
        if (NULL == host)
        {
            OutPut(SYS_ALERT, "DoHostHaCheck: get host %ld failed!\n", *it);
            SkyAssert(NULL != host);
            continue;
        }
        SendHighAvailable(host, EV_HOST_FAULT);
        _host_pool->Put(host, true);
    }


    oids.clear();
    where.str("");
    where << " running_vms > 0 "
          << " AND run_state = " << Host::ON_LINE
          << " AND enabled = " << false
          << " AND all_free_tcu >= 0 "
          << " AND mem_free >= 0 "
          << " AND hardware_state = 0 "
          << " AND disk_free < 0 ";
    _host_pool->Search(oids, where.str());
    for (it  = oids.begin();
         it != oids.end();
         it++)
    {
        Host *host = _host_pool->Get(*it, true);
        if (NULL == host)
        {
            OutPut(SYS_ALERT, "DoHostHaCheck: get host %ld failed!\n", *it);
            SkyAssert(NULL != host);
            continue;
        }
        SendHighAvailable(host, EV_HOST_STORAGE_ERROR);
        _host_pool->Put(host, true);
    }
}

/******************************************************************************/
void HostManager::DoScanForDrop()
{
    time_t now = time(0);
    // 所有 hc 一视同仁
    time_t duration;
    duration = statistics_reserve * 60 * 60;

    time_t need_drop = now - duration;

    Datetime drop(need_drop);

    _host_statistics->Delete(drop.tostr());
    _host_statistics_nics->Delete(drop.tostr());
}

/******************************************************************************/
void HostManager::DoHostQueryReq(const MessageFrame &message)
{
    MessageHostQueryReq req;
    req.deserialize(message.data);

    MessageHostQueryAck ack;
    ack._host_name = req._host_name;
    ack._ret_code = ERR_HOST_OK;
    ack._max_count = 0;
    ack._next_index = -1;

    if (req._host_name.empty())
    {// 显示所有主机
        ack._max_count = _host_pool->Count();

        ostringstream oss;
        oss << " (oid != -1) " 
            << " ORDER BY oid LIMIT " << req._query_count 
            << " offset " << req._start_index;
        vector<int64> oids;
        _host_pool->Search(oids, oss.str());

        if (oids.size() >= req._query_count )
        {
            ack._next_index = req._start_index + req._query_count;
        }

        vector<int64>::iterator it;
        for (it = oids.begin(); it != oids.end(); ++it)
        {
            Host *host = _host_pool->Get(*it, true);
            if (NULL == host)
            {
                OutPut(SYS_ALERT, "DoHostQueryReq: get host %ld failed!\n",
                       *it);
                SkyAssert(NULL != host);
                continue;
            }
            HostFree host_free;
            _host_pool->GetHostFree(*it, host_free);
            HostInfoType host_info;
            host->Dump(host_info, host_free);
            _host_pool->Put(host, true);
            _host_atca->GetRatePower(*it, host_info._rate_power);
            ack._host_infos.push_back(host_info);
        }
    }
    else
    {
        Host *host = _host_pool->Get(req._host_name, true);
        if (NULL == host)
        {
            ack._ret_code = ERR_HOST_NOT_EXIST;
        }
        else
        {
            HostFree host_free;
            int64 hid = host->GetOID();
            _host_pool->GetHostFree(hid, host_free);
            HostInfoType host_info;
            host->Dump(host_info, host_free);
            _host_pool->Put(host, true);
            _host_atca->GetRatePower(hid, host_info._rate_power);
            ack._host_infos.push_back(host_info);
            ack._max_count = 1;
        }
    }

    MessageOption option(message.option.sender(), EV_HOST_QUERY_ACK, 0, 0);
    _mu->Send(ack, option);
}

/******************************************************************************/
void HostManager::SendHostRegisterReq(Host *host)
{
    if (NULL == host)
    {
        SkyAssert(NULL != host);
        return;
    }

    // 组织消息内容
    MessageHostRegisterReq req;
    req._host_name = host->get_host_name();
    req._cc_ifs_info = _ifs_info;

    // 组织消息地址
    MID receiver;
    receiver._application = host->get_host_name();
    receiver._process = PROC_HC;
    receiver._unit = MU_HOST_AGENT;

    // 发送注册请求消息
    MessageOption option(receiver, EV_HOST_REGISTER_REQ, 0, 0);
    _mu->Send(req, option);
}

/******************************************************************************/
void HostManager::SendHighAvailable(Host *host, int32 msg_id, void *data)
{

    MID receiver(MU_VM_HA);
    MessageOption option(receiver, msg_id, 0, 0);

    switch (msg_id)
    {
        case EV_HOST_OFFLINE:
        case EV_HOST_FAULT:
        {
            MessageHostFault msg;
            msg._host_id = host->GetOID();
            msg._host_name = host->get_host_name();
            _mu->Send(msg, option);
            break;
        }
        case EV_HOST_STORAGE_ERROR:
        {
            MessageHostFault msg;
            msg._host_id = host->GetOID();
            msg._host_name = host->get_host_name();
            _mu->Send(msg, option);
            break;
        }
        case EV_HOST_ENET_ERROR:
        {
            MessageHostEnetError *msg = static_cast<MessageHostEnetError *>
                                        (data);
            SkyAssert(NULL != msg);
            msg->_host_id = host->GetOID();
            msg->_host_name = host->get_host_name();
            _mu->Send(*msg, option);
            break;
        }
        default:
        {
            SkyAssert(0);
            break;
        }
    }
}

/******************************************************************************/
void HostManager::SendAlarm(Host *host, uint32 code, byte flag,
                            const string &info)
{
    // 未注册到集群中的主机不允许调用该接口，已进入维护模式的主机屏蔽掉告警
    if ((OAM_REPORT_ALARM == flag) || (OAM_REPORT_INFORM == flag))
    {
        if (true == host->get_is_disabled())
        {
            return;
        }
    }

    map<uint32, list<string> >::iterator it_code;
    // 告警信息存入host的alarms中
    if (OAM_REPORT_ALARM == flag)
    {
        it_code = host->get_alarms().find(code);
	    //查询告警是否在缓存中存在
	    if (it_code != host->get_alarms().end())
	    {
	        list<string> &info_list = it_code->second;
	        list<string>::iterator it_info = find(info_list.begin(),
                                                  info_list.end(), info);
	        if (it_info != info_list.end())
	        {
	            return;
	        }
	    }
		
        if (it_code == host->get_alarms().end())
        {
            list<string> info_list;
            info_list.push_back(info);
            host->get_alarms().insert(make_pair(code, info_list));
        }
        else
        {
            list<string> &info_list = it_code->second;
            list<string>::iterator it_info = find(info_list.begin(),
                                                  info_list.end(), info);
            if (it_info == info_list.end())
            {
                info_list.push_back(info);
            }
        }
    }
    // 告警恢复时将告警从host的alarms中删除
    else if (OAM_REPORT_RESTORE == flag)
    {
        it_code = host->get_alarms().find(code);

	    //查询告警是否在缓存中存在
	    if (it_code == host->get_alarms().end())
	    {
            return;
	    }
		else
		{
	        list<string> &info_list = it_code->second;
	        list<string>::iterator it_info = find(info_list.begin(),
                                                  info_list.end(), info);
	        if (it_info == info_list.end())
	        {
	            return;
	        }
		}
		
        if (it_code != host->get_alarms().end())
        {
            list<string> &info_list = it_code->second;
            list<string>::iterator it_info = find(info_list.begin(),
                                                  info_list.end(), info);
            if (it_info != info_list.end())
            {
                info_list.erase(it_info);
            }
            if (info_list.empty())
            {
                host->get_alarms().erase(it_code);
            }
        }
    }

    // 构造告警位置信息
    AlarmLocation location;
    location.strSubSystem = "computing";
    location.strLevel1 = _tecs_name;
    location.strLevel2 = ApplicationName();
    location.strLevel3 = host->get_host_name();
    location.strLocation = host->get_location();//host->get_description();

    // 发送告警或告警恢复或通知
    AlarmReport ar;
    ar.dwAlarmCode = code;
    ar.ucAlarmFlag = flag;
    ar.alarm_location = location;
    ar.aucMaxAddInfo = info;
    AlarmAgent *aa = AlarmAgent::GetInstance();
    SkyAssert(NULL != aa);
    aa->Send(ar);
}

/******************************************************************************/
void HostManager::SendAlarmResume(Host *host)
{
    AlarmLocation location;
    location.strSubSystem = "computing";
    location.strLevel1 = _tecs_name;
    location.strLevel2 = ApplicationName();
    location.strLevel3 = host->get_host_name();
    location.strLocation = host->get_location();

    AlarmAgent *aa = AlarmAgent::GetInstance();
    SkyAssert(NULL != aa);

    AlarmReport ar;
    ar.ucAlarmFlag = OAM_REPORT_RESTORE;
    ar.alarm_location = location;

    map<uint32, list<string> >::iterator it_code = host->get_alarms().begin();
    for (; it_code != host->get_alarms().end(); ++it_code)
    {
        list<string> &info_list = it_code->second;
        list<string>::iterator it_info = info_list.begin();
        for (; it_info != info_list.end(); ++it_info)
        {
            ar.dwAlarmCode = it_code->first;
            ar.aucMaxAddInfo = *it_info;
            aa->Send(ar);
        }
        info_list.clear();
    }
    host->get_alarms().clear();
}

/******************************************************************************/
void HostManager::SendAlarm(Host *host)
{
    SkyAssert(NULL != host);

    // 检查所有触发告警的条件，如果有故障，则上报告警
    if (Host::OFF_LINE == host->get_run_state())
    {
        // 上报主机离线告警
        SendAlarm(host, ALM_TECS_HOST_OFF_LINE, OAM_REPORT_ALARM);
    }
    if (true == host->get_mem_state())
    {
        // 上报内存故障告警
        SendAlarm(host, ALM_TECS_HOST_MEMORY_ERR, OAM_REPORT_ALARM);
    }
    if (true == host->get_oscillator())
    {
        // 上报晶振故障告警
        SendAlarm(host, ALM_TECS_HOST_OSCILLATOR_ERR, OAM_REPORT_ALARM);
    }


    map<string, int32>::const_iterator it_disk = host->get_disks().begin();
    for (; it_disk != host->get_disks().end(); ++it_disk)
    {
        if (0 != it_disk->second)
        {
            // 上报disk故障告警
            AlarmDiskErr info(it_disk->first);
            SendAlarm(host, ALM_TECS_HOST_HARDDISK_ERR, OAM_REPORT_ALARM,
                      info.serialize());
        }
    }
}

/******************************************************************************/
void HostManager::DoHostAlarmCheck()
{
    vector<int64> oids;
    ostringstream where;
    where << " enabled = " << false;
    _host_pool->Search(oids, where.str());

    vector<int64>::iterator it;
    for (it = oids.begin(); it != oids.end(); ++it)
    {
        Host *host = _host_pool->Get(*it, false);
        if (NULL == host)
        {
            OutPut(SYS_ALERT, "DoHostAlarmCheck: get host %ld failed!\n", *it);
            SkyAssert(NULL != host);
            continue;
        }
        SendAlarm(host);
    }
}

/******************************************************************************/
void HostManager::SetHostRunState(Host *host, Host::RunState state,
                                  const string &conflict_cluster)
{
    if (NULL == host)
    {
        SkyAssert(NULL != host);
        return;
    }

    // 状态无变化
    if (host->get_run_state() == state)
    {
        return;
    }

    if (Host::ON_LINE == host->get_run_state())
    {
        SendHighAvailable(host, EV_HOST_OFFLINE);
    }

    if (Host::OFF_LINE == host->get_run_state())
    {
        // 发送离线告警恢复
        SendAlarm(host, ALM_TECS_HOST_OFF_LINE, OAM_REPORT_RESTORE);
    }

    host->set_run_state(state);
    _host_pool->Update(host);

    if (Host::OFF_LINE == host->get_run_state())
    {
        // 发送离线告警
        SendAlarm(host, ALM_TECS_HOST_OFF_LINE, OAM_REPORT_ALARM);
    }
}

/******************************************************************************/
void HostManager::SetHostMemState(Host *host, bool state)
{
    if (NULL == host)
    {
        SkyAssert(NULL != host);
        return;
    }

    if (host->get_mem_state() == state)
    {
        return;
    }


    if (host->get_hardware_state() == 0)
    {
        SendHighAvailable(host, EV_HOST_FAULT);
    }

    if (true == host->get_mem_state())
    {
        // 发送内存故障告警恢复
        SendAlarm(host, ALM_TECS_HOST_MEMORY_ERR, OAM_REPORT_RESTORE);
    }

    host->set_mem_state(state);
    _host_pool->Update(host);   // 此处数据库操作可省掉

    if (true == host->get_mem_state())
    {
        // 发送内存故障告警
        SendAlarm(host, ALM_TECS_HOST_MEMORY_ERR, OAM_REPORT_ALARM);
    }
}

/******************************************************************************/
void HostManager::SetHostOsciliatorState(Host *host, bool state)
{
    if (NULL == host)
    {
        SkyAssert(NULL != host);
        return;
    }

    if (host->get_oscillator() == state)
    {
        return;
    }

    if (host->get_hardware_state() == 0)
    {
        SendHighAvailable(host, EV_HOST_FAULT);
    }

    if (true == host->get_oscillator())
    {
        // 发送晶振故障告警恢复
        SendAlarm(host, ALM_TECS_HOST_OSCILLATOR_ERR, OAM_REPORT_RESTORE);
    }

    host->set_oscillator(state);
    _host_pool->Update(host);    // 此处数据库操作可省掉

    if (true == host->get_oscillator())
    {
        // 发送晶振故障告警
        SendAlarm(host, ALM_TECS_HOST_OSCILLATOR_ERR, OAM_REPORT_ALARM);
    }
}

/******************************************************************************/
void HostManager::SetHostIsDisabled(Host *host, bool is_disabled)
{
    if (NULL == host)
    {
        SkyAssert(NULL != host);
        return;
    }


    // 状态无变化
    if (host->get_is_disabled() == is_disabled)
    {
        return;
    }

    host->set_is_disabled(is_disabled);
    _host_pool->Update(host);

    if (true == host->get_is_disabled())
    {
        SendAlarmResume(host);
        SendHighAvailable(host, EV_HOST_FAULT);
    }
    else
    {
        SendAlarm(host);
    }
}
/******************************************************************************/
void HostManager::SetClusterName(const MessageFrame &message)
{
    MessageRegisteredSystem Msg;
    Msg.deserialize(message.data);

    string tecs_name = Msg._system_name;
    //由于加入的时候，需要使用老的_cluster_name信息，所以需要放在前面处理
    if (_tecs_name != tecs_name)
    {
        _tecs_name = tecs_name;
    }
}

/******************************************************************************/
void HostManager::SetStatisticsInfoToDB(const ResourceStatistics& host_statistics,
                                        int64 hid)
{
    if (0 == statistics_reserve)
    {// 默认配置为 0 不入库
        return;
    }
    // 入库的时间
    Datetime now;
    string save_time = now.tostr();

    ostringstream where;
    where << " WHERE hid = " << hid
          << " AND statistics_time = '" << host_statistics._statistics_time << "'";
    vector<ResourceStatistics> vec_host_statistics;
    if ((0 == _host_statistics->Search(vec_host_statistics, where.str()))
        && (true == vec_host_statistics.empty()))
    {
        /* 因 HC 统计时间可能大于上报时间, 会有重复, 为避免 INSERT 失败
         * 增加数据库的日志, 占用很多的硬盘空间, 故增加检查
         */
        _host_statistics->Insert(host_statistics, hid, save_time);
    }

    vector<NicInfo> vec_nic_info;
    vector<NicInfo>::const_iterator it;
    for (it  = host_statistics._vec_nic_info.begin();
         it != host_statistics._vec_nic_info.end();
         it++)
    {
        where.str("");
        vec_nic_info.clear();
        where << " WHERE hid = " << hid
              << " AND name = '" << it->_name << "'"
              << " AND statistics_time = '" << host_statistics._statistics_time << "'";
        if ((0 == _host_statistics_nics->Search(vec_nic_info, where.str()))
            && (true == vec_nic_info.empty()))
        {
            _host_statistics_nics->Insert(*it,
                                          hid,
                                          host_statistics._statistics_time,
                                          save_time);
        }

    }
    
}

/******************************************************************************/
void HostManager::SetHostDiskState(Host *host, const vector<DiskInfo>& vec_disk_info)
{
    SkyAssert(NULL != host);
    
    /* host硬盘信息更新
     * NOTE: 存在硬盘插拔的情况, 简单处理, 将 hid 的所有记录删除再 insert
     */
    int64 hid = host->GetOID();
    _host_disks->Delete(host->GetOID());
    vector<DiskInfo>::const_iterator it;
    for (it  = vec_disk_info.begin();
         it != vec_disk_info.end();
         it++)
    {
        _host_disks->Insert(*it, hid);
        /* 检查硬盘的健康状态, 若有问题则上报告警
         */
        AlarmDiskErr info(it->_name);
        if (2 == it->_status)
        {
            SendAlarm(host, ALM_TECS_HOST_HARDDISK_WARN, OAM_REPORT_ALARM,
                      info.serialize());            
        }
        else if (3 == it->_status)
        {
            /* 出现更高程度的硬盘状态时, 恢复较低程度的告警,
             * 仅上报较高程度的告警
             */
            SendAlarm(host, ALM_TECS_HOST_HARDDISK_WARN, OAM_REPORT_RESTORE,
                      info.serialize());
            SendAlarm(host, ALM_TECS_HOST_HARDDISK_ERR, OAM_REPORT_ALARM,
                      info.serialize());
        }
        else
        {
            SendAlarm(host, ALM_TECS_HOST_HARDDISK_WARN, OAM_REPORT_RESTORE,
                      info.serialize());
            SendAlarm(host, ALM_TECS_HOST_HARDDISK_ERR, OAM_REPORT_RESTORE,
                      info.serialize());
        }
    }
    
}

} // namespace zte_tecs

