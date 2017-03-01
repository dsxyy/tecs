/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：cluster_agent.cpp
* 文件标识：见配置管理计划书
* 内容摘要：cluster_agent类实现文件
* 当前版本：1.0
* 作    者：liuyi
* 完成日期：2012年11月19日
*
* 修改记录1：
*     修改日期：2011/11/19
*     版 本 号：V1.0
*     修 改 人：liuyi
*     修改内容：创建
*******************************************************************************/
#include "sky.h"
#include "cluster_agent.h"
#include "tecs_config.h"
#include "config_cluster.h"
#include "light_agent.h"
#include "vstorage_manager.h"
// 与本模块交互的消息结构定义
#include "cluster_manager_with_host_manager.h"
#include "msg_host_manager_with_api_method.h"
#include "msg_host_manager_with_host_agent.h"
#include "msg_host_manager_with_host_handler.h"
#include "msg_host_manager_with_high_available.h"
#include "msg_cluster_agent_with_api_method.h"
#include "dns_building.h"
#include "ntp_building.h"

#include <fstream>

namespace zte_tecs 
{

// 定时器消息号
#define TIMER_REGISTERED_SYSTEM     EV_TIMER_1  // 周期组播归属的tece系统名称
#define TIMER_CLUSTER_INFO_REPORT   EV_TIMER_2  // 周期上报集群发现或信息

ClusterAgent *ClusterAgent::_instance = NULL;

/******************************************************************************/
ClusterAgent::ClusterAgent():
        _have_startup(false),
        _tecs_name(),
        _cluster_verify_id(),
        _cc_ip_address()
{
    _mu = NULL;
}

STATUS ClusterAgent::StartMu(const string& name)
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
void ClusterAgent::MessageEntry(const MessageFrame &message)
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
            case EV_PUBLISH_SSH_PUBKEY:
            {
                //从TC接收到的组播key，直接转发到本集群的组播组中即集群中的cc与hc成员
                //即本节点的key不在本进程处理，会在cc_ssh_trust的进程中处理
                MID receiver("group", ApplicationName()+MUTIGROUP_SSH_TRUST);
                MessageOption option(receiver, EV_PUBLISH_SSH_PUBKEY, 0, 0);
                _mu->Send(message.data, option); 
                break;
            }
            case EV_CLUSTER_REGISTER_REQ:
            {
                DoClusterRegister(message);
                break;
            }
            case EV_CLUSTER_UNREGISTER_REQ:
            {
                DoClusterUnregister(message);
                break;
            }
            case EV_CLUSTER_CONFIG_REQ:
            {
                DoClusterConfig(message);
                break;
            }
            case EV_CLUSTER_CONFIG_QUERY_REQ:
            {
                DoClusterConfigQuery(message);
                break;
            }
            case EV_CLUSTER_TCU_CONFIG_REQ:
            {
                DoClusterTcuConfig(message);
                break;
            }
            case EV_CLUSTER_TCU_QUERY_REQ:
            {
                DoClusterTcuQuery(message);
                break;
            }
            case EV_CLUSTER_INFO_REQ:
            {
                DoClusterInfoReport();
                break;
            }
            case TIMER_CLUSTER_INFO_REPORT:
            {
                DoClusterInfoReport();
                break;
            }
            case TIMER_REGISTERED_SYSTEM:
            {
                DoRegisteredSystem();
                break;
            }
            case EV_REGISTERED_SYSTEM_QUERY:
            {
                DoRegisteredSystem(message);
                break;
            }

            case EV_CLUSTER_CMM_ADD_REQ:
            {
                DoClusterCmmAdd(message);
                break;
            }
            
            case EV_CLUSTER_CMM_SET_REQ:
            {
                DoClusterCmmSet(message);
                break;
            }
            
            case EV_CLUSTER_CMM_DELETE_REQ:
            {
                DoClusterCmmDelete(message);
                break;
            }

            case EV_CLUSTER_CMM_QUERY_REQ:
            {
                DoClusterCmmQuery(message);
                break;
            }
 
            case EV_CLUSTER_SAVE_ENERGY_CONFIG_REQ:
            {
                DoClusterSaveEnergyConfig(message);
                break;
            }

            case EV_CLUSTER_SAVE_ENERGY_QUERY_REQ:
            {
                DoClusterSaveEnergyQuery(message);
                break;
            }

            case EV_REGISTERED_SYSTEM:
            {
                break;
            }        

            case EV_VM_SCH_POLICY_SET_REQ:
            {
                SetSchPolicy(message);
                break;
            }

            case EV_VM_SCH_POLICY_QUERY_REQ:
            {
                QuerySchPolicy(message);
                break;
            }

            case EV_VM_HA_POLICY_SET_REQ:
            {
                SetVMHAPolicy(message);
                break;
            }

            case EV_VM_HA_POLICY_QUERY_REQ:
            {
                QueryVMHAPolicy(message);
                break;
            }

            default:
            {
                cout << "ClusterAgent receives an unkown message!" << endl;
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
void ClusterAgent::DoStartUp()
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

    // 创建ClusterAgent需要的定时器
    TIMER_ID    timer;
    TimeOut     timeout;
    if (INVALID_TIMER_ID == (timer = _mu->CreateTimer()))
    {
        OutPut(SYS_EMERGENCY, "Create timer for cluster info report failed!\n");
        SkyAssert(INVALID_TIMER_ID != timer);
        SkyExit(-1, "ClusterAgent::DoStartUp: Create timer for cluster info "
                "report failed.");
    }
    // 设置定时器: 用于周期上报集群信息给TC
    timeout.type = LOOP_TIMER;
    timeout.duration = 10 * 1000;
    timeout.msgid = TIMER_CLUSTER_INFO_REPORT;
    if (SUCCESS != (ret = _mu->SetTimer(timer, timeout)))
    {
        OutPut(SYS_EMERGENCY, "Set timer for cluster info report failed!\n");
        SkyAssert(SUCCESS == ret);
        SkyExit(-1, "ClusterAgent::DoStartUp: Set timer for cluster info report "
                "failed.");
    }
    // 从数据库中尝试获取本集群所归属的TECS系统名称
    /* 此处有2种情况:
     * 1. 初始安装tecs系统, 数据库中为空, 先走入TIMER_CLUSTER_INFO_REPORT
     *    超时, 组播发送EV_CLUSTER_DISCOVER, TC的cluster_manager收到消息后,
     *    发送EV_CLUSTER_REGISTER_REQ给agent, agent获取到后, 更新数据库;
     * 2. 后续tecs上电后, 都会先读取磁盘上数据库信息, 因1中已存在, 故下面
     *    更新到内存中.
     */
    SetTecsName(_cluster_config->Get(CFG_TECS_NAME));

    //增加一个产生集群验证码的过程，利用GenerateUUID的接口生成
    _cluster_verify_id = _cluster_config->Get(CFG_CLUSTER_VERIFY_ID);
    if (_cluster_verify_id.empty())
    {
        _cluster_verify_id = GenerateUUID();
        _cluster_config->Set(CFG_CLUSTER_VERIFY_ID,_cluster_verify_id);
    }

    // 加入归属信息组播组
    ret = _mu->JoinMcGroup(string(ApplicationName()) + MUTIGROUP_SUF_REG_SYSTEM);
    if (SUCCESS != ret)
    {
        SkyAssert(false);
        string lastwords("ClusterAgent failed to join group! ret = ");
        lastwords.append(to_string(ret,dec));
        SkyExit(-1,lastwords);
    }

    // 设置归属信息周期组播定时器
    timeout.type = LOOP_TIMER;
    timeout.duration = 50 * 1000;
    timeout.msgid = TIMER_REGISTERED_SYSTEM;
    if (SUCCESS != _mu->SetTimer(_mu->CreateTimer(), timeout))
    {
        OutPut(SYS_EMERGENCY, "Create timer for send registered cluster!\n");
        SkyAssert(0);
        SkyExit(-1, "HostManager::DoStartUp: Create timer for send registered "
                "cluster failed.");
    }

    if (SUCCESS != Transaction::Enable(GetDB()))
    {
        OutPut(SYS_ERROR,"ClusterAgent enable transaction fail! ret = %d",ret);
        SkyExit(-1, "ClusterAgent::MessageEntry: call "
                "Transaction::Enable(GetDB()) failed.");
    }

    // 主动发送一次归属信息
    DoRegisteredSystem();

    // 设置CC的IP地址
    string mask;
    if (0 != GetKernelPortInfo("dvs_kernel", _cc_ip_address, mask))
    {
        OutPut(SYS_ALERT, "Get cc ip address info failed!\n");
        _cc_ip_address.clear();
    }
}
/******************************************************************************/
void ClusterAgent::DoClusterRegister(const MessageFrame &message)
{
    MessageClusterRegisterReq req;
    req.deserialize(message.data);
    // 发错集群了，忽略
    if (req._name != ApplicationName())
    {
        OutPut(SYS_ALERT, "DoClusterRegister: cluster name is err, %s != %s!\n",
               req._name.c_str(), ApplicationName().c_str());
        SkyAssert(0);
        return;
    }

    string tecs_name = message.option.sender()._application;
    // 发送方为空，忽略
    if (tecs_name.empty())
    {
        OutPut(SYS_ALERT, "DoClusterRegister: tecs name is empty!\n");
        SkyAssert(0);
        return;
    }

    /* 此处应该是设置客户端(对 TC), 但要满足对 HC 为服务端的要求
     * 利用 ntp server 的分层结构, 不再设置客户端, 自动同步
     */
    string ip_connected;
    map<string, string>::iterator it;
    for (it  = req._tc_ifs_info.begin();
         it != req._tc_ifs_info.end();
         it++)
    {
        if (it->second.empty())
            continue;
        ip_connected.clear();
        if (SUCCESS != GetIpByConnect(it->second, ip_connected))
            continue;
        if (_cc_ip_address == ip_connected)
        {
            _tc_ip_address = it->second;
            break;
        }
    }
    if (!_tc_ip_address.empty())
    {
        SetNtpServerConfig("register", _tc_ip_address, true);
        SetDnsServerConfig("register", _tc_ip_address, DNS_IM_IMAGE_TECS);
        /* 此处为何要加上 application? 考虑 TC/CC/HC 合一的情况,
         * 分不清是 CC/HC 触发增加的, HC 注销时会把 CC 的也删除掉
         */
        SetDnsClientConfig("register", _tc_ip_address, ApplicationName());
    }

    if (_tecs_name.empty())
    {
        SetTecsName(tecs_name);
    }
}

/******************************************************************************/
void ClusterAgent::DoClusterUnregister(const MessageFrame &message)
{
    MessageClusterUnRegisterReq req;
    req.deserialize(message.data);
    // 发错集群了，忽略
    if (req._name != ApplicationName())
    {
        OutPut(SYS_ALERT, "DoClusterUnregister: cluster name is err, %s != %s!\n",
               req._name.c_str(), ApplicationName().c_str());
        SkyAssert(0);
        return;
    }

    string tecs_name = message.option.sender()._application;
    // 发送方为空，忽略
    if (tecs_name.empty())
    {
        OutPut(SYS_ALERT, "DoClusterUnregister: tecs name is empty!\n");
        SkyAssert(0);
        return;
    }

    if (tecs_name != _tecs_name)
    {
        return;
    }

    SetTecsName("");

    if (!_tc_ip_address.empty())
    {
        SetNtpServerConfig("unregister", _tc_ip_address, true);
        SetDnsServerConfig("unregister", _tc_ip_address, DNS_IM_IMAGE_TECS);
        SetDnsClientConfig("unregister", _tc_ip_address, ApplicationName());
    }

    // 进程退出，所有主机的告警被自动清除
    OutPut(SYS_EMERGENCY,
           "DoClusterUnregister: cluster is unregistered from %s!\n",
           tecs_name.c_str());
    SkyExit(-1, "ClusterAgent::DoClusterUnregister: cluster is unregistered.");
}
/******************************************************************************/
void ClusterAgent::DoClusterSaveEnergyConfig(const MessageFrame &message)
{
    MessageClusterSaveEnergyParaCfgReq req;
    req.deserialize(message.data);

    MessageClusterCfgAck ack;
    MessageOption option(message.option.sender(), EV_CLUSTER_CONFIG_ACK, 0, 0);

    if(SUCCESS != req.Validate())
    {
        ack._ret_code = req.Validate();
        _mu->Send(ack, option);
        return;
    }

    string save_energy_enable;
    if(req._save_energy_enable)
    {
        save_energy_enable = "yes";
    }
    else
    {
        save_energy_enable = "no";
    }

    ostringstream oss;
    oss<<req._max_host_num; 
    string  max_host_num = oss.str();   

    oss.str("");
    oss<<req._min_host_num; 
    string  min_host_num = oss.str();   

    oss.str("");
    oss<<req._host_percent; 
    string  host_percent = oss.str();

    oss.str("");
    oss << req._save_energy_interval; 
    string  save_energy_interval = oss.str();
    
    //节能降耗功能新增字段
    _cluster_config->Set(CFG_CLUSTER_SAVE_ENERGY_FLG, save_energy_enable);
    _cluster_config->Set(CFG_CLUSTER_POWERON_MIN, min_host_num);
    _cluster_config->Set(CFG_CLUSTER_POWERON_MAX, max_host_num);
    _cluster_config->Set(CFG_CLUSTER_POWERON_PER, host_percent);
    _cluster_config->Set(CFG_CLUSTER_SAVE_ENERGY_INTERVAL, save_energy_interval);
    
    ack._ret_code = ERR_HOST_OK;
    _mu->Send(ack, option);

    MID receiver(MU_POWER_MANAGER);
    MessageOption option1(receiver, EV_CLUSTER_SAVE_ENERGY_CONFIG_CHG, 0, 0);
    _mu->Send(ack, option1);
}

/******************************************************************************/
void ClusterAgent::DoClusterSaveEnergyQuery(const MessageFrame &message)
{
    MessageClusterSaveEnergyParaQueryReq req;
    req.deserialize(message.data);

    MessageClusterSaveEnergyParaQueryAck ack;
    MessageOption option(message.option.sender(), EV_CLUSTER_SAVE_ENERGY_QUERY_ACK, 0, 0);

    string  save_energy_enable =  _cluster_config->Get(CFG_CLUSTER_SAVE_ENERGY_FLG);  

    if("yes" == save_energy_enable)
    {
        ack._save_energy_enable = true;
    }
    else
    {
        ack._save_energy_enable = false;
    }

    string  max_host_num = _cluster_config->Get(CFG_CLUSTER_POWERON_MAX);
    ack._max_host_num = atoi(max_host_num.c_str()); 

    string  min_host_num = _cluster_config->Get(CFG_CLUSTER_POWERON_MIN);
    ack._min_host_num = atoi(min_host_num.c_str());  

    string  host_percent = _cluster_config->Get(CFG_CLUSTER_POWERON_PER);    
    ack._host_percent = atoi(host_percent.c_str());

    string  save_energy_interval = _cluster_config->Get(CFG_CLUSTER_SAVE_ENERGY_INTERVAL);    
    ack._save_energy_interval = atoi(save_energy_interval.c_str()); 

    ack._ret_code = ERR_HOST_OK;
    _mu->Send(ack, option);
}

/******************************************************************************/
void ClusterAgent::DoClusterConfig(const MessageFrame &message)
{
    MessageClusterCfgReq req;
    req.deserialize(message.data);

    if ((req._command != "SET") && (req._command != "DEL"))
    {
        OutPut(SYS_ALERT, "DoClusterConfig: unknown req command %s!\n",
               req._command.c_str());
        SkyAssert(0);
        return;
    }

    MessageClusterCfgAck ack;
    MessageOption option(message.option.sender(), EV_CLUSTER_CONFIG_ACK, 0, 0);

    // 不是本系统发过来的配置请求
    if (message.option.sender()._application != _tecs_name)
    {
        ack._ret_code = ERR_CLUSTER_NOT_REGISTER;
        _mu->Send(ack, option);
        return;
    }
/**********如下操作都不能设置cc需注意************/
/*********************************************************/
    /* 合法性检测 */
    if (req._config_name != CFG_TECS_NAME )
    {
        ack._ret_code = ERR_CLUSTER_CFG_SET_FAILD;
        _mu->Send(ack, option);
        return;
    }

    // 私有关键字，不允许直接配置
    if (req._config_name == CFG_TECS_NAME)
    {
        ack._ret_code = ERR_CLUSTER_CFG_NAME_INVALID;
        _mu->Send(ack, option);
        return;
    }
/*********************************************************/

    if (req._command == "SET")
    {
        if (0 == _cluster_config->Set(req._config_name, req._config_value))
        {
            ack._ret_code = ERR_HOST_OK;
        }
        else
        {
            ack._ret_code = ERR_CLUSTER_CFG_SET_FAILD;
        }
    }
    else if (req._command == "DEL")
    {
        if (0 == _cluster_config->Drop(req._config_name))
        {
            ack._ret_code = ERR_HOST_OK;
        }
        else
        {
            ack._ret_code = ERR_CLUSTER_CFG_DEL_FAILD;
        }
    }

    _mu->Send(ack, option);
}

/******************************************************************************/
void ClusterAgent::DoClusterConfigQuery(const MessageFrame &message)
{
    MessageClusterCfgQueryReq req;
    req.deserialize(message.data);

    MessageClusterCfgQueryAck ack;
    MessageOption option(message.option.sender(), EV_CLUSTER_CONFIG_QUERY_ACK,
                         0, 0);

    // 不是本系统发过来的配置查询请求
    if (message.option.sender()._application != _tecs_name)
    {
        ack._ret_code = ERR_CLUSTER_NOT_REGISTER;
        _mu->Send(ack, option);
        return;
    }

    if (req._config_name.empty())
    {
        ack._config_infos = _cluster_config->get_configs();
    }
    else
    {
        ack._config_infos.insert(make_pair(req._config_name,
                                           _cluster_config->Get(req._config_name)));
    }

    ack._ret_code = ERR_HOST_OK;
    _mu->Send(ack, option);
}

/******************************************************************************/
void ClusterAgent::DoClusterTcuConfig(const MessageFrame &message)
{
    MessageClusterTcuCfgReq req;
    req.deserialize(message.data);

    if ((req._command != "SET") && (req._command != "DEL"))
    {
        OutPut(SYS_ALERT, "DoClusterTcuConfig: unknown req command %s!\n",
               req._command.c_str());
        SkyAssert(0);
        return;
    }

    MessageClusterTcuCfgAck ack;
    MessageOption option(message.option.sender(), EV_CLUSTER_TCU_CONFIG_ACK,
                         0, 0);

    // 不是本系统发过来的配置请求
    if (message.option.sender()._application != _tecs_name)
    {
        ack._ret_code = ERR_CLUSTER_NOT_REGISTER;
        _mu->Send(ack, option);
        return;
    }

    HostCpuInfo *host_cpu_info = HostCpuInfo::GetInstance();
    if (NULL == host_cpu_info)
    {
        ack._ret_code = ERR_TCU_SET_FAILD;
        _mu->Send(ack, option);
        return;
    }

    /* 判断cpu_info_id是否存在 */
    if(!host_cpu_info->IsExist(req._cpu_info_id))
    {
        ack._ret_code = ERR_CPUINFO_ID_NOT_EXIST;
        _mu->Send(ack, option);
        return;
    }
    
    vector<int64> oids;
    ostringstream where;
    where << " running_vms > 0 AND cpu_info_id = " << req._cpu_info_id;
    _host_pool->Search(oids, where.str());
    /* 如果此cpu已经部署虚拟机，则不运行设置或删除 */
    if (!oids.empty())
    {
        TcuInfoType tcu_info;
        _tcu_config->Get(req._cpu_info_id, tcu_info);

        if(0 == tcu_info._tcu_num)
        {
            int32 system_tcu_num = _tcu_config->GetSystemTcuNumByCpuId(req._cpu_info_id);
            if(system_tcu_num != req._tcu_num)
            {
                ack._ret_code = ERR_TCU_VMS_RUNNING;
                _mu->Send(ack, option);
                return;
            }
        }
        else
        {
            if(tcu_info._tcu_num != req._tcu_num)
            {
                ack._ret_code = ERR_TCU_VMS_RUNNING;
                _mu->Send(ack, option);
                return;
            }
        }
    }

    if (req._command == "SET")
    {
        if (0 == _tcu_config->Set(req._cpu_info_id, req._tcu_num,
                                  req._description))
        {
            ack._ret_code = ERR_HOST_OK;
        }
        else
        {
            ack._ret_code = ERR_TCU_SET_FAILD;
        }
    }
    else if (req._command == "DEL")
    {
        if (0 == _tcu_config->Drop(req._cpu_info_id))
        {
            ack._ret_code = ERR_HOST_OK;
        }
        else
        {
            ack._ret_code = ERR_TCU_DEL_FAILD;
        }
    }

    _mu->Send(ack, option);
}

/******************************************************************************/
void ClusterAgent::DoClusterTcuQuery(const MessageFrame &message)
{
    MessageClusterTcuQueryReq req;
    req.deserialize(message.data);

    MessageClusterTcuQueryAck ack;
    ack._max_count = 0;
    ack._next_index = -1;

    MessageOption option(message.option.sender(), EV_CLUSTER_TCU_QUERY_ACK,
                         0, 0);

    // 不是本系统发过来的配置查询请求
    if (message.option.sender()._application != _tecs_name)
    {
        ack._ret_code = ERR_CLUSTER_NOT_REGISTER;
        _mu->Send(ack, option);
        return;
    }

    if ((!req._cpu_info.empty()) && \
            (req._cpu_info_id != HostCpuInfo::INVALID_CPU_INFO_ID))
    {
        ack._ret_code = ERR_TCU_CFG_INVALID;
        _mu->Send(ack, option);
        return;
    }

    HostCpuInfo *host_cpu_info = HostCpuInfo::GetInstance();
    if (NULL == host_cpu_info)
    {
        return;
    }

    /* && 应该为 || ? */
    if ((req._cpu_info.empty()) && \
            (req._cpu_info_id == HostCpuInfo::INVALID_CPU_INFO_ID))
    {
        ostringstream oss;
        
        oss << " (cpu_info_id != -1) " 
        << " ORDER BY cpu_info_id LIMIT " << req._query_count
        << " offset  "<< req._start_index;

        if (0 != _tcu_config->Search(ack._tcu_infos, oss.str()))
        {
            ack._ret_code = ERR_TCU_QUERY_FAILD;
        }
        else
        {

            ack._ret_code = ERR_HOST_OK;
            ack._max_count = _tcu_config->Count();
            if (ack._tcu_infos.size() >= req._query_count )
            {
                ack._next_index = req._query_count +  req._start_index;
            }
        }
    }
    else if (!req._cpu_info.empty())
    {
        CpuInfo cpu_info;
        ostringstream temp;
        temp << "<CpuInfo>\n\t<_info>\n" << req._cpu_info
        << "\n\t</_info>\n</CpuInfo>";
        cpu_info.deserialize(temp.str());

        TcuInfoType tcu_info;
        if (0 != _tcu_config->Get(cpu_info._info, tcu_info))
        {
            ack._ret_code = ERR_CPUINFO_ID_NOT_EXIST;
        }
        else
        {
            ack._max_count = 1;
            ack._tcu_infos.push_back(tcu_info);
            ack._ret_code = ERR_HOST_OK;
        }
    }
    else // req._cpu_info_id != HostCpuInfo::INVALID_CPU_INFO_ID
    {
        TcuInfoType tcu_info;
        if (0 != _tcu_config->Get(req._cpu_info_id, tcu_info))
        {
            ack._ret_code = ERR_CPUINFO_ID_NOT_EXIST;
        }
        else
        {
            ack._max_count = 1;
            ack._tcu_infos.push_back(tcu_info);
            ack._ret_code = ERR_HOST_OK;
        }
    }

    _mu->Send(ack, option);
}
/******************************************************************************/
void ClusterAgent::DoClusterInfoReport()
{
    TecsConfig *config = TecsConfig::Get();
    int64 host_num = 0;
    MessageClusterInfoReport report;
    
    if (NULL == config)
    {
        OutPut(SYS_ERROR, "Fail to get TecsConfig instance in DoClusterInfoReport.");
        return;
    }

    const DBConfig& dbconf = config->get_db_conf();
    report._db_name        = dbconf.database;            //数据库名

    // 设置CC的IP地址
    string mask;
    if (0 != GetKernelPortInfo("dvs_kernel", _cc_ip_address, mask))
    {
        OutPut(SYS_ALERT, "Get cc ip address info failed!\n");
        _cc_ip_address.clear();
    }
    
    //如果从数据库获取到的server url为空或者为localhost,或者为127.0.0.1,上报StartUp流程中获取的CC IP
    if ((dbconf.server_url.empty()) || ("localhost" == dbconf.server_url) || \
        ("127.0.0.1" == dbconf.server_url))
    {
        report._db_server_url  = _cc_ip_address;       
    }
    else
    {
        report._db_server_url  = dbconf.server_url;
    }
    
    report._db_server_port = dbconf.server_port;         //服务器的端口
    report._db_user        = dbconf.user;                //用户名
    report._db_passwd      = dbconf.passwd;              //密码

    //原先是用gethostid的方法，再取size得到host数量的，用Count更为简单
    report._host_count = _host_pool->Count();

    ConfigCluster *cluster_config = ConfigCluster::GetInstance();
    if (NULL != cluster_config)
    {
        report._img_usage = cluster_config->Get(CFG_IMG_USAGE);
        report._share_img_usage = cluster_config->Get(CFG_SHARE_IMG_USAGE);
        report._img_srctype = cluster_config->Get(CFG_IMG_SRC_TYPE);
        from_string<int>(report._img_spcexp, cluster_config->Get(CFG_IMG_SPC_EXP),dec);
    }

    _host_pool->GetReportClusterInfo(report._tcu_max_total,"sum(all_tcu)","host_pool_in_use");        
    _host_pool->GetReportClusterInfo(report._tcu_free_total,"sum(all_free_tcu)","host_pool_in_use");        
    _host_pool->GetReportClusterInfo(report._core_free_max,"MAX(cpus)","host_pool_in_use");        
    _host_pool->GetReportClusterInfo(report._tcu_unit_free_max,"MAX(tcu_num)","host_pool_in_use");        
    _host_pool->GetReportClusterInfo(report._tcu_free_max,"MAX(all_free_tcu)","host_pool_in_use");        
    _host_pool->GetReportClusterInfo(report._disk_max_total,"SUM(disk_max)","host_pool_in_use");        
    _host_pool->GetReportClusterInfo(report._disk_free_total,"SUM(disk_free)","host_pool_in_use");        
    _host_pool->GetReportClusterInfo(report._disk_free_max,"MAX(disk_free)","host_pool_in_use");        
    _host_pool->GetReportClusterInfo(report._mem_max_total,"SUM(mem_max)","host_pool_in_use");        
    _host_pool->GetReportClusterInfo(report._mem_free_total,"SUM(mem_free)","host_pool_in_use");        
    _host_pool->GetReportClusterInfo(report._mem_free_max,"MAX(mem_free)","host_pool_in_use");        
    _host_pool->GetReportClusterInfo(report._cpu_usage_average,"SUM(cpu_usage_30m)","host_pool");        
    _host_pool->GetReportClusterInfo(report._cpu_usage_max,"MAX(cpu_usage_30m)","host_pool");        
    _host_pool->GetReportClusterInfo(report._cpu_usage_min,"MIN(cpu_usage_30m)","host_pool");        
    _host_pool->GetReportClusterInfo(host_num,"COUNT(oid)","host_pool");        
    if(0 != host_num)
    {
        report._cpu_usage_average /= host_num;
    }

    report._node.insert(make_pair(1, _cc_ip_address));
    
    //CC给TC的消息中增加集群身份验证码的信息
    report._cluster_verify_id = _cluster_verify_id;

    if (_tecs_name.empty())
    {
        MID receiver("group", MUTIGROUP_CLUSTER_DISCOVER);
        MessageOption option(receiver, EV_CLUSTER_DISCOVER, 0, 0);
        _mu->Send(report, option);
    }
    else
    {
        MID receiver(_tecs_name, PROC_TC, MU_CLUSTER_MANAGER);
        MessageOption option(receiver, EV_CLUSTER_INFO_REPORT, 0, 0);
        _mu->Send(report, option);
    }
}

/******************************************************************************/
void ClusterAgent::ExitTecsGroup()
{
    STATUS ret = 0;
    //字段为空时，防止上电的时候就是为空，这个时候不需要进行操作
    if (_tecs_name.empty())
    {
        return;
    }
    ret = _mu->ExitMcGroup(_tecs_name+MUTIGROUP_SSH_TRUST);
    if (SUCCESS != ret)
    {
        OutPut(SYS_EMERGENCY, "ClusterAgent exit tecs group %s failed!\n",
               (_tecs_name+MUTIGROUP_SSH_TRUST).c_str());
    }
    OutPut(SYS_INFORMATIONAL, "ClusterAgent exit tecs group %s successfully!\n",
        (_tecs_name+MUTIGROUP_SSH_TRUST).c_str());
}

/******************************************************************************/
void ClusterAgent::JoinTecsGroup()
{
    STATUS ret = 0;
    //字段为空时，防止上电的时候就是为空，这个时候不需要进行操作
    if (_tecs_name.empty())
    {
        return;
    }
    ret = _mu->JoinMcGroup(_tecs_name+MUTIGROUP_SSH_TRUST);
    if (SUCCESS != ret)
    {
        OutPut(SYS_EMERGENCY, "ClusterAgent join tecs group %s failed!\n",
               (_tecs_name+MUTIGROUP_SSH_TRUST).c_str());
        SkyAssert(SUCCESS == ret);
        SkyExit(-1, "ClusterAgent join tecs group failed!.");
    }
    OutPut(SYS_INFORMATIONAL, "ClusterAgent join tecs group %s successfully!\n",
        (_tecs_name+MUTIGROUP_SSH_TRUST).c_str());
}

/******************************************************************************/
void ClusterAgent::SetTecsName(const string &tecs_name)
{
    //由于加入的时候，需要使用老的_cluster_name信息，所以需要放在前面处理
    if (_tecs_name != tecs_name)
    {
        ExitTecsGroup();    
        _tecs_name = tecs_name;
        JoinTecsGroup();        
    }

    _cluster_config->Set(CFG_TECS_NAME, _tecs_name);
    DoRegisteredSystem();

    // 点灯
    LightInfo light_info;
    string setter;
    light_info._type = LIGHT_TYPE_HOST;
    light_info._state = LIGHT_NO_JOINED;
    light_info._priority = 2;
    setter = "cluster_agent_of_" + ApplicationName() + "_" + ProcessName();
    strncpy(light_info._setter,setter.c_str(),LI_MAX_SETTER_LEN-1);
    LightAgent *light_agent = LightAgent::GetInstance();

    if (_tecs_name.empty())
    {
        strncpy(light_info._remark,"Not registered into TECS.",LI_MAX_REMARK_LEN-1);
        light_agent->TurnOn(light_info);
    }
    else
    {
        strncpy(light_info._remark,"Have registered into TECS.",LI_MAX_REMARK_LEN-1);
        light_agent->TurnOff(light_info);
    }
}

/******************************************************************************/
void ClusterAgent::DoRegisteredSystem()
{
    MessageRegisteredSystem msg(_tecs_name);
    MID receiver("group", string(ApplicationName()) + MUTIGROUP_SUF_REG_SYSTEM);
    MessageOption option(receiver, EV_REGISTERED_SYSTEM, 0, 0);
    _mu->Send(msg, option);
}

/******************************************************************************/
void ClusterAgent::DoRegisteredSystem(const MessageFrame &message)
{
    MessageRegisteredSystem msg(_tecs_name);
    MessageOption option(message.option.sender(), EV_REGISTERED_SYSTEM, 0, 0);
    _mu->Send(msg, option);
}

/******************************************************************************/
void ClusterAgent::DoClusterCmmAdd(const MessageFrame &message)
{
    MessageCmmConfigReq req;
    req.deserialize(message.data);

    MessageCmmConfigCfgAck ack;
    MessageOption option(message.option.sender(), EV_CLUSTER_CMM_ADD_ACK, 0, 0);

    CmmConfig cmm_config;

    cmm_config._bureau = req._bureau;
    cmm_config._rack = req._rack;
    cmm_config._shelf = req._shelf;
    cmm_config._type = req._type;
    cmm_config._ip_address = req._ip_address;
    cmm_config._description = req._description;
    
    if(0 == _cmm_config->Add(cmm_config))
    {
        ack._ret_code = ERR_HOST_OK;
    }
    else
    {
        ack._ret_code = ERR_CMM_ADD_FAIL;
    }
    _mu->Send(ack, option);
}

/******************************************************************************/
void ClusterAgent::DoClusterCmmSet(const MessageFrame &message)
{
    MessageCmmConfigReq req;
    req.deserialize(message.data);

    MessageCmmConfigCfgAck ack;
    MessageOption option(message.option.sender(), EV_CLUSTER_CMM_ADD_ACK, 0, 0);

    CmmConfig cmm_config;

    cmm_config._bureau = req._bureau;
    cmm_config._rack = req._rack;
    cmm_config._shelf = req._shelf;
    cmm_config._type = req._type;
    cmm_config._ip_address = req._ip_address;
    cmm_config._description = req._description;
    
    if(0 == _cmm_config->Set(cmm_config))
    {
        ack._ret_code = ERR_HOST_OK;
    }
    else
    {
        ack._ret_code = ERR_CMM_SET_FAIL;
    }
    _mu->Send(ack, option);
}

/******************************************************************************/
void ClusterAgent::DoClusterCmmDelete(const MessageFrame &message)
{
    MessageCmmDeleteReq req;
    req.deserialize(message.data);

    MessageCmmDeleteAck ack;
    MessageOption option(message.option.sender(), EV_CLUSTER_CMM_ADD_ACK, 0, 0);
    cout<<"  DoClusterCmmDelete  "<<endl;
    if(0 == _cmm_config->Delete(req._bureau,req._rack,req._shelf))
    {
        ack._ret_code = ERR_HOST_OK;
    }
    else
    {
        ack._ret_code = ERR_CMM_SET_FAIL;
    }
    _mu->Send(ack, option);
}

/******************************************************************************/
void ClusterAgent::DoClusterCmmQuery(const MessageFrame &message)
{
    MessageCmmQueryReq req;
    req.deserialize(message.data);

    MessageCmmQueryAck ack;
    ack._max_count = 0;
    ack._next_index = -1;

    MessageOption option(message.option.sender(), EV_CLUSTER_TCU_QUERY_ACK,
                         0, 0);

    ostringstream oss;
    oss << " ORDER BY bureau,rack,shelf LIMIT " << req._query_count
    << " offset  "<< req._start_index;

    //cout<<"  DoClusterCmmQuery _start_index = "<<req._start_index<<" _query_count "<<req._query_count<<endl;

    if (0 != _cmm_config->Search(ack._cmm_infos, oss.str()))
    {
        ack._ret_code = ERR_CMM_QUERY_FAIL;
    }
    else
    {

        ack._ret_code = ERR_HOST_OK;
        ack._max_count = _cmm_config->Count();
        if (ack._cmm_infos.size() >= req._query_count )
        {
            ack._next_index = req._query_count +  req._start_index;
        }
    }

    _mu->Send(ack, option);
}

void ClusterAgent::SetSchPolicy(const MessageFrame &message)
{
    MessageSchedulerPolicySetReq req;
    req.deserialize(message.data);

    _cluster_config->Set(CFG_SCH_POLICY, req._policy);
    _cluster_config->Set(CFG_RUN_SCH_MAX_VMS, to_string<int64>(req._influence_vms,dec));
    _cluster_config->Set(CFG_RUN_SCH_CYCLE, to_string<int64>(req._cycle_time,dec));

    MessageClusterCfgAck ack;
    ack._ret_code = SUCCESS;
    MessageOption option(message.option.sender(), EV_CLUSTER_CONFIG_ACK, 0, 0);
    _mu->Send(ack, option);

    MessageOption option_sch(MU_VM_SCHEDULER, EV_VM_SCH_POLICY_SET_REQ, 0, 0);
    _mu->Send(req, option_sch);
}

void ClusterAgent::QuerySchPolicy(const MessageFrame &message)
{
    MessageSchedulerPolicyQueryAck ack;

    ack._policy = _cluster_config->Get(CFG_SCH_POLICY);
    ack._cycle_time = atoi(_cluster_config->Get(CFG_RUN_SCH_CYCLE).c_str());
    ack._influence_vms = atoi(_cluster_config->Get(CFG_RUN_SCH_MAX_VMS).c_str());

    ack._ret_code = SUCCESS;
    MessageOption option(message.option.sender(), EV_VM_SCH_POLICY_QUERY_ACK, 0, 0);
    _mu->Send(ack, option);
}

void ClusterAgent::SetVMHAPolicy(const MessageFrame &message)
{
    VmHAPolicy req;
    req.deserialize(message.data);

    _cluster_config->Set(CFG_VMHA_HOST_DOWN_POLICY, req.host_down);
    _cluster_config->Set(CFG_VMHA_HOST_FAULT_POLICY, req.host_fault);
    _cluster_config->Set(CFG_VMHA_HOST_STORAGE_FAULT_POLICY, req.host_storage_fault);

    MessageClusterCfgAck ack;
    ack._ret_code = SUCCESS;
    MessageOption option(message.option.sender(), EV_CLUSTER_CONFIG_ACK, 0, 0);
    _mu->Send(ack, option);
}

void ClusterAgent::QueryVMHAPolicy(const MessageFrame &message)
{
    MessageVMHAPolicyQueryAck ack;

    ack.host_down = _cluster_config->Get(CFG_VMHA_HOST_DOWN_POLICY);
    ack.host_fault = _cluster_config->Get(CFG_VMHA_HOST_FAULT_POLICY);
    ack.host_storage_fault = _cluster_config->Get(CFG_VMHA_HOST_STORAGE_FAULT_POLICY);

    ack._ret_code = SUCCESS;
    MessageOption option(message.option.sender(), EV_VM_HA_POLICY_QUERY_ACK, 0, 0);
    _mu->Send(ack, option);
}

/******************************************************************************/
void DbgSetHostTcu(char *host_name, int32 tcu, char *description)
{
    if (NULL == host_name)
    {
        cout << "No input hostname." << endl;
        return;
    }
    if (NULL == description)
    {
        cout << "No input description." << endl;
        return;
    }

    HostPool *host_pool = HostPool::GetInstance();
    if (NULL == host_pool)
    {
        cout << "HostPool is not created." << endl;
        return;
    }

    ConfigTcu *config = ConfigTcu::GetInstance();
    if (NULL == config)
    {
        cout << "ConfigTcu is not created." << endl;
        return;
    }

    Host *host = host_pool->Get(host_name, true);
    if (NULL == host)
    {
        cout << "Host=" << host_name << " is not found." << endl;
        return;
    }

    // 修改 config_tcu 配置表
    if (0 != config->Set(host->get_cpu_info_id(), tcu, description))
    {
        cout << "Set tcu config failed." << endl;
        host->UnLock();
        return;
    }
    OutPut(SYS_INFORMATIONAL, "DbgSetHostTcu: have set tcu config, host=%s "
           "cpu_info_id=%ld, tcu_num=%d.\n",
           host_name, host->get_cpu_info_id(), tcu);

    host->UnLock();
}
DEFINE_DEBUG_FUNC(DbgSetHostTcu);

} // namespace zte_tecs

