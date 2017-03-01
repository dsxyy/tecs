/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：vnet_agent.cpp
* 文件标识：见配置管理计划书
* 内容摘要：CVNetAgent类实现文件
* 当前版本：1.0
* 作    者：lvech
* 完成日期：2012年12月15日
*
* 修改记录1：
*     修改日期：2012/12/15
*     版 本 号：V1.0
*     修 改 人：lvech
*     修改内容：创建
*******************************************************************************/
#include "tecs_config.h"
#include "registered_system.h"
#include "vnet_portagent.h"
#include "dev_monitor.h"
#include "dev_mgr.h"
#include "vna_switch_module.h"
#include "vna_switch_agent.h"
#include "vna_common.h"
#include "vnetlib_common.h"
#include "vna_agent.h"
#include <stdlib.h>

namespace zte_tecs
{

#define DEFAULT_HOST_NAME    ""
#define DEFAULT_VNA_APP      ""
#define DEFAULT_VNM_APP      ""

string g_strHostName = DEFAULT_HOST_NAME;
string g_strVNAApplication = DEFAULT_VNA_APP;
string g_strVNMApplication = DEFAULT_VNM_APP;

CVNetAgent *CVNetAgent::m_pInstance = NULL;


/************************************************************
* 函数名称： CVNetAgent
* 功能描述： CVNetAgent构造函数
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1        V1.0       lvech         创建
***************************************************************/
CVNetAgent::CVNetAgent()
{
    m_bOpenDbgInf = VNET_FALSE;
    m_nStateMachines = 0;
    m_pMU = NULL;
    m_nTimerID = INVALID_TIMER_ID;
    m_nModuleCheckTimerID = INVALID_TIMER_ID;
}

/************************************************************
* 函数名称： ~CVNetAgent
* 功能描述： CVNetAgent析构函数
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1        V1.0       lvech         创建
***************************************************************/
CVNetAgent::~CVNetAgent()
{
    if (INVALID_TIMER_ID != m_nTimerID)
    {
        m_pMU->KillTimer(m_nTimerID);
        m_nTimerID = INVALID_TIMER_ID;
    }
    
    if (INVALID_TIMER_ID != m_nModuleCheckTimerID)
    {
        m_pMU->KillTimer(m_nModuleCheckTimerID);
        m_nModuleCheckTimerID = INVALID_TIMER_ID;
    }
    
    if (NULL != m_pInstance)
    {
        delete m_pInstance;
    }
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
* 2013/1        V1.0       lvech         创建
***************************************************************/
STATUS CVNetAgent::StartMu(const string& name)
{
    if(m_pMU)
    {
        SkyAssert(false);
        return ERROR_DUPLICATED_OP;
    }
    
    // 消息单元的创建和初始化
    m_pMU = new MessageUnit(name);
    if (!m_pMU)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetAgent::StartMu: Create mu %s failed!\n",name.c_str());
        return ERROR_NO_MEMORY;
    }

    STATUS ret = m_pMU->SetHandler(this);
    if (SUCCESS != ret)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetAgent::StartMu: SetHandler mu %s failed! ret = %d\n",name.c_str(),ret);
        return ret;
    }

    ret = m_pMU->Run();
    if (SUCCESS != ret)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetAgent::StartMu: Run mu %s failed! ret = %d\n",name.c_str(),ret);
        return ret;
    }
    
    ret = m_pMU->Register();
    if (SUCCESS != ret)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetAgent::StartMu: Register mu %s failed! ret = %d\n",name.c_str(),ret);
        return ret;
    }

    //给自己发送上电消息，促使消息单元状态切换到工作态
    MessageFrame message(SkyInt32(0), EV_VNET_POWER_ON);
    ret = m_pMU->Receive(message);
    if (SUCCESS != ret)
    {
        SkyAssert(0);
        return ret;
    }
    m_pMU->Print();
    return SUCCESS;
}
/************************************************************
* 函数名称： Init
* 功能描述： 初始化函数
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2012/5         V1.0       lvech         创建
***************************************************************/
STATUS CVNetAgent::DoStartUp()
{
    // 加入归属信息组播组
    STATUS ret = m_pMU->JoinMcGroup(string(ApplicationName()) + 
                                  MUTIGROUP_VNA_REG_SYSTEM);
    if (SUCCESS != ret)
    {
        SkyAssert(false);
        string lastwords("VNetAgent failed to join group! ret = ");
        lastwords.append(to_string(ret, dec));
        SkyExit(-1,lastwords);
    }

    // 设置归属集群信息周期组播定时器
    TimeOut timeout;
    timeout.type = LOOP_TIMER;
    timeout.duration = TIMER_INTERVAL_VNA_REGISTERED_VNM;
    timeout.msgid = TIMER_VNA_REGISTERED_VNM;
    if (SUCCESS != m_pMU->SetTimer(m_pMU->CreateTimer(), timeout))
    {
        VNET_LOG(VNET_LOG_ERROR, "Create timer for send registered VNM!\n");
        SkyAssert(0);
    }

    // 创建VNA发现或信息上报共用的定时器
    m_nTimerID = m_pMU->CreateTimer();
    if (INVALID_TIMER_ID == m_nTimerID)
    {
        VNET_LOG(VNET_LOG_ERROR, 
               "Create timer failed for send vna discover or report!\n");
        SkyAssert(0);
    }
    timeout.type = LOOP_TIMER;
    timeout.duration = TIMER_INTERVAL_VNA_REGISTER;
    timeout.msgid = TIMER_VNA_REGISTER;
    m_pMU->SetTimer(m_nTimerID, timeout);    

    // 设置定时器: 用于维护HC/CC/DHCP等业务进程的运行状态是否离线
    m_nModuleCheckTimerID = m_pMU->CreateTimer();
    if (INVALID_TIMER_ID == m_nModuleCheckTimerID)
    {
        VNET_LOG(VNET_LOG_ERROR, "Create timer for vna offline check failed!\n");
        SkyAssert(INVALID_TIMER_ID != m_nModuleCheckTimerID);
    }
    timeout.type = LOOP_TIMER;
    timeout.duration = TIMER_INTERVAL_MODULE_STATE_CHECK;
    timeout.msgid = TIMER_MODULE_STATE_CHECK;
    if (SUCCESS != (ret = m_pMU->SetTimer(m_nModuleCheckTimerID, timeout)))
    {
        VNET_LOG(VNET_LOG_ERROR, "Set timer for module offline check failed!\n");
        SkyAssert(SUCCESS == ret);
    }
    
    // 给VNA上其他进程也告知归属集群为空
    BroadcastRegedVNM();

    // 进行VNA发现
    SendRegToVNM();

    return 0;
}

/************************************************************
* 函数名称： MessageEntry
* 功能描述： 消息主入口函数
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2012/5         V1.0       lvech         创建
***************************************************************/
void CVNetAgent::MessageEntry(const MessageFrame &message)
{
    TimeOut  tTmOut;
    if (VNET_TRUE == m_bOpenDbgInf)
    {
        cout << "StateMachines: " << m_pMU->get_state() <<" receive msg id: " << message.option.id() << endl;
    }
    
    switch (m_pMU->get_state())
    {
    case S_Startup:
    {
        switch (message.option.id())
        {
        case EV_VNET_POWER_ON:
        {
            if(0 != DoStartUp())
            {
                SkyExit(-1,"CVNetAgent power on DoStartUp failed!");
            }

            //重设一下状态
            m_nStateMachines = VNET_STATE_MACHINE_WORK;
            m_pMU->set_state(S_Work);
            OutPut(SYS_NOTICE, "%s power on!\n",m_pMU->name().c_str());
            VNET_LOG(VNET_LOG_INFO,
                     "CVNetAgent::MessageEntry: entering VNET_STATE_MACHINE_WORK.\n");
        }
        break;
        }// end VNET_STATE_MACHINE_START
        break;
    }

    case S_Work:
    {
        
        switch (message.option.id())
        {
            //向VNM发送组播注册消息
            case TIMER_VNA_REGISTER:
            {
                SendRegToVNM();
                break;
            }

            //VNM收到组播注册消息后，向VNA应答注册
            case EV_VNA_REGISTER_REQ:
            {
                RecvRegReqFromVNM(message);
                break;
            }

            //VNM取消VNA的注册
            case EV_VNA_UNREGISTER_REQ:
            {
                RecvUnRegReqFromVNM(message);
                break;
            }

            //VNA定时通报自己注册VNM信息
            case TIMER_VNA_REGISTERED_VNM:
            {
                BroadcastRegedVNM();
                break;
            }

            //查询VNA所注册的VNM消息
            case EV_REGISTERED_VNM_QUERY:
            {
                RecvRegedVNMReq(message);
                break;
            }
            
            case EV_REGISTERED_SYSTEM:
            {
                break;
            }  
            
            //向VNM上报信息
            case TIMER_VNA_INFO_REPORT:
            {
                SendVNAReport();
                break;
            }

            case TIMER_VNA_INFO_REPORT_FIRST:
            {
                SendVNAReportFirst();
                break;
            }

            //CC/HC等业务进程向VNA发送的登记信息
            case EV_VNETLIB_REGISTER_MSG:
            {
                RecvRegFromModule(message);
                break;
            }

            //CC/HC等业务进程向VNA发送的取消登记信息
            case EV_MODULE_UNREGISTER_TO_VNA:
            {
                RecvUnRegFromModule(message);
                break;
            }

            //处理CC/HC等业务进程向VNA发送的心跳信息
            case EV_VNETLIB_HEART_BEAT_MSG:
            {
                RecvHeartFromModule(message);
                break;
            }

            //检查CC/HC等业务进程的状态
            case TIMER_MODULE_STATE_CHECK:
            {
                CheckModuleState();
                break;
            }
        
            //当HC向CC注册成功，由HC进程调用NETWORK API中NotifyRegisteredInfoToVNA接口通知VNA相关进程
            case EV_VNETLIB_HC_REGISTER_MSG:
            {
                HCRegisteredSystem(message);
                break;
            }

            default:
            {
                VNET_LOG(VNET_LOG_INFO, "Unknown message: %d.\n", message.option.id());
                break;
            }
        }
    }
    // POWERON
    break;

    default:
        break;

    }
}

/************************************************************
* 函数名称： ExitVNMGroup
* 功能描述： 退出VNM所在组播组
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2012/5         V1.0       lvech         创建
***************************************************************/
void CVNetAgent::ExitVNMGroup()
{
    STATUS ret = 0;
    
    //字段为空时，防止上电的时候就是为空，这个时候不需要进行操作
    if (m_strVNMApp.empty())
    {
        return;
    }
    
    ret = m_pMU->ExitMcGroup(m_strVNMApp+MUTIGROUP_SSH_TRUST);
    if (SUCCESS != ret)
    {
        VNET_LOG(VNET_LOG_ERROR, "VNetAgent exit tecs group %s failed!\n",
            (m_strVNMApp+MUTIGROUP_SSH_TRUST).c_str());
    }
    
    VNET_LOG(VNET_LOG_DEBUG, "VNetAgent exit tecs group %s successfully!\n",
        (m_strVNMApp+MUTIGROUP_SSH_TRUST).c_str());
}

/************************************************************
* 函数名称： JoinVNMGroup
* 功能描述： 加入VNM所在组播组
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2012/5         V1.0       lvech         创建
***************************************************************/
void CVNetAgent::JoinVNMGroup()
{
    STATUS ret = 0;
    
    //字段为空时，防止上电的时候就是为空，这个时候不需要进行操作
    if (m_strVNMApp.empty())
    {
        return;
    }
    
    ret = m_pMU->JoinMcGroup(m_strVNMApp+MUTIGROUP_SSH_TRUST);
    if (SUCCESS != ret)
    {
        VNET_LOG(VNET_LOG_ERROR, "VNetAgent join VNM group %s failed!\n",
               (m_strVNMApp+MUTIGROUP_SSH_TRUST).c_str());
        SkyAssert(SUCCESS == ret);
    }
    
    VNET_LOG(VNET_LOG_INFO, "VNetAgent join tecs group %s successfully!\n",
        (m_strVNMApp+MUTIGROUP_SSH_TRUST).c_str());
}


/************************************************************
* 函数名称： GoToSendRegToVNM
* 功能描述： 开启VNM发现流程，创建定时器及发送消息
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1         V1.0       lvech         创建
***************************************************************/
void CVNetAgent::GoToSendRegToVNM(void)
{
    // 设置主机发现循环定时器
    //m_pMU->StopTimer(m_nTimerID);
    
    TimeOut timeout;
    timeout.type = LOOP_TIMER;
    timeout.duration = TIMER_INTERVAL_VNA_REGISTER;
    timeout.msgid = TIMER_VNA_REGISTER;
    m_pMU->SetTimer(m_nTimerID, timeout);    

    // 处理VNM发现流程
    SendRegToVNM();
}

/************************************************************
* 函数名称： SendRegToVNM
* 功能描述： 向VNM发送注册消息
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1         V1.0       lvech         创建
***************************************************************/
void CVNetAgent::SendRegToVNM()
{
    CVNAReportMsg msg;
    msg.m_strHostName = g_strHostName;
    msg.m_strVNAApp = g_strVNAApplication;

    //调用host_manager接口获取所有端口信息
    //HC_GetPortInitInfo(msg._portinfo);

    // 给主机发现组播组发送主机发现消息
    MID receiver("group", MUTIGROUP_VNA_REG);
    MessageOption option(receiver, EV_VNA_REGISTER_TO_VNM, 0, 0);
    int32 ret = m_pMU->Send(msg, option);
    if (SUCCESS != ret)
    {
        VNET_LOG(VNET_LOG_WARN, "SendRegToVNM: Send discover failed!ret=%d\n", ret);
        SkyAssert(SUCCESS == ret);
    }

    if (m_bOpenDbgInf)
    {
        cout << "Send SendRegToVNM message:" <<endl;  
        msg.Print();
    }
}

/************************************************************
* 函数名称： GoToSendVNAReport
* 功能描述： 开启向VNM上报信息，包括创建上报定时器及上报消息
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1         V1.0       lvech         创建
***************************************************************/
void CVNetAgent::GoToSendVNAReport(void)
{
    //需要检查上报定时器是否已经创建，已创建则返回
    //if (INVALID_TIMER_ID != m_nTimerID)
    //{
    //    return ;
    //}

    // 创建网络信息上报循环定时器
    //if (INVALID_TIMER_ID == (m_nTimerID = m_pMU->CreateTimer()))
    //{
    //    VNET_LOG(VNET_LOG_ERROR, 
    //           "Create timer failed for send VNA information report!\n");
    //    SkyAssert(0);
    //}

    // 设置主机发现循环定时器
    m_pMU->StopTimer(m_nTimerID);
    
    TimeOut timeout;
    timeout.type = LOOP_TIMER;

    //第一次上报消息，需要添加偏移值，避免在VNM上报风暴
    timeout.duration = TIMER_INTERVAL_VNA_REGISTER + rand() % 60;
    timeout.msgid = TIMER_VNA_INFO_REPORT_FIRST;
    m_pMU->SetTimer(m_nTimerID, timeout);

    // 处理主机信息上报流程，在DoReport中无定时器处理
    SendVNAReport();
}



/************************************************************
* 函数名称： SendVNAReport
* 功能描述： 向VNM上报信息 第一次 
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1         V1.0       lvech         创建
***************************************************************/
void CVNetAgent::SendVNAReportFirst(void)
{
    // 设置主机发现循环定时器
    m_pMU->StopTimer(m_nTimerID);
    
    TimeOut timeout;
    timeout.type = LOOP_TIMER;
    timeout.duration = TIMER_INTERVAL_VNA_REGISTER;
    timeout.msgid = TIMER_VNA_INFO_REPORT;
    m_pMU->SetTimer(m_nTimerID, timeout);

    // 处理主机信息上报流程，在DoReport中无定时器处理
    SendVNAReport();
}

/************************************************************
* 函数名称： SendVNAReport
* 功能描述： 向VNM上报信息
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1         V1.0       lvech         创建
***************************************************************/
void CVNetAgent::SendVNAReport()
{
    CVNAReportMsg msg;

    msg.m_strHostName = g_strHostName;
    msg.m_strVNAApp = g_strVNAApplication;

    //需要包括自己的角色信息
    GetModuleReportInfo(msg.m_vecRegistedModInfo);

    // 发送消息
    MID receiver;
    receiver._application = ApplicationName();
    receiver._process = PROC_VNA;
    receiver._unit = MU_VNA_CONTROLLER;
    MessageOption option(receiver, EV_VNET_VNA_REPORT_INFO, 0, 0);
    m_pMU->Send(msg, option);

    if (m_bOpenDbgInf)
    {
        cout << "CVNetAgent::Send CVNAReportMsg message:\n" << endl;  
        msg.Print();
    }
}

/************************************************************
* 函数名称： RecvRegFromModule
* 功能描述： 处理CC/HC/DHCP等业务进程向VNA的登记消息
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1         V1.0       lvech         创建
***************************************************************/
void CVNetAgent::RecvRegFromModule(const MessageFrame &message)
{
    time_t cur_time = time(0);

    MID sender = message.option.sender();
    
    SkyAssert(!sender._application.empty());

    CVNetModuleRegMsg req;
    req.deserialize(message.data);

    if (m_bOpenDbgInf)
    {
        cout << "Receive CVNetModuleRegMsg message:\n" << endl;  
        req.Print();
    }
    
    //更新MODULE信息
    vector<TModuleInfo>::iterator it;
    for (it = m_vecModuleInfo.begin(); it != m_vecModuleInfo.end(); it++)
    {
        if ((req.m_module_app == it->GetModuleName())
            && (req.m_module_role == it->GetRole()))
        {
            it->SetRole(req.m_module_role);
            it->SetState(VNET_MODULE_ONLINE);
            it->SetLastHeartTime(cur_time);
            it->SetExtInfo(req.m_module_ext_info);
            it->SetModuleLibMid(sender);
            break;
        }
    }

    if (it == m_vecModuleInfo.end())
    {
        TModuleInfo module_node;
        module_node.SetModuleName(req.m_module_app);
        module_node.SetRole(req.m_module_role);
        module_node.SetState(VNET_MODULE_ONLINE);
        module_node.SetLastHeartTime(cur_time);
        module_node.SetExtInfo(req.m_module_ext_info);
        module_node.SetModuleLibMid(sender);

        m_vecModuleInfo.push_back(module_node);
    }
    
    CVNetModuleACKRegMsg cAck;
    cAck.m_module_app = req.m_module_app;

    // 回复注册;
    MessageOption option(message.option.sender(),EV_VNETLIB_REGISTER_MSG_ACK,0,0);
    m_pMU->Send(cAck, option);    
}
/************************************************************
* 函数名称： RecvUnRegFromModule
* 功能描述： 处理CC/HC/DHCP等业务进程向VNA的取消登记消息
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1         V1.0       lvech         创建
***************************************************************/
void CVNetAgent::RecvUnRegFromModule(const MessageFrame &message)
{
    MID sender = message.option.sender();
    
    SkyAssert(!sender._application.empty());

    CModuleUnRegisterToVNAMsg req;
    req.deserialize(message.data);

    if (m_bOpenDbgInf)
    {
        cout << "Receive CModuleUnRegisterToVNAMsg message:\n" << endl;  
        req.Print();
    }
    
    //删除MODULE信息
    vector<TModuleInfo>::iterator it;
    for (it = m_vecModuleInfo.begin(); it != m_vecModuleInfo.end(); it++)
    {
        if ((req.m_strModuleApp == it->GetModuleName())
            && (req.m_nModuleRole == it->GetRole()))
        {
            m_vecModuleInfo.erase(it);
            break;
        }
    }
}

/************************************************************
* 函数名称： RecvRegReqFromVNM
* 功能描述： 处理VNM发给VNA的注册应答消息
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1         V1.0       lvech         创建
***************************************************************/
void CVNetAgent::RecvRegReqFromVNM(const MessageFrame &message)
{
    MID sender = message.option.sender();
    
    VNET_LOG(VNET_LOG_INFO, "CVNetAgent::DoRegister receive msg from VNM.\n");   
    
    SkyAssert(!sender._application.empty());
    SkyAssert(sender._process == PROC_VNM);
    SkyAssert(sender._unit == MU_VNM);

    CVNARegModReqMsg req;
    req.deserialize(message.data);

    if (m_bOpenDbgInf)
    {
        cout << "Receive CVNARegModReqMsg message:\n" << endl;  
        req.Print();
    }
    
    // 对消息内容进行合法性检查
    if (req.m_strVNAName != string(ApplicationName()))
    {
        VNET_LOG(VNET_LOG_WARN, "DoRegister: vna name is err, %s != %s!\n",
               req.m_strVNAName.c_str(), ApplicationName().c_str());
        SkyAssert(req.m_strVNAName == string(ApplicationName()));
        return;
    }

    // 如果没有归属，则设置为发送方
    if (m_strVNMApp.empty())
    {
        SetVNM(sender._application);
        GoToSendVNAReport();
    }
}

/************************************************************
* 函数名称： RecvUnRegReqFromVNM
* 功能描述： 处理VNM发给VNA的取消注册应答消息
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1         V1.0       lvech         创建
***************************************************************/
void CVNetAgent::RecvUnRegReqFromVNM(const MessageFrame &message)
{
    MID sender = message.option.sender();

    SkyAssert(!sender._application.empty());
    SkyAssert(sender._process == PROC_VNM);
    SkyAssert(sender._unit == MU_VNM);

    CVNAUnRegModReqMsg req;
    req.deserialize(message.data);

    if (m_bOpenDbgInf)
    {
        cout << "Receive CVNAUnRegModReqMsg message:\n" << endl;  
        req.Print();
    }
    
    // 对消息内容进行合法性检查
    if (req.m_strVNAName != string(ApplicationName()))
    {
        VNET_LOG(VNET_LOG_WARN, "DoUnregister: vna name is err, %s != %s!\n",
               req.m_strVNAName.c_str(), ApplicationName().c_str());
        SkyAssert(req.m_strVNAName == string(ApplicationName()));
        return;
    }

    // 如果不是归属的集群发过来的消息，则过滤掉
    if (m_strVNMApp != sender._application)
    {
        return;
    }

    SetVNM("");
    
    // HC进程退出，重新发现以更新系统的归属信息
    SkyExit(-1, "VNA is unregistered from the vnm, exit!\n");
}

/************************************************************
* 函数名称： SetVNM
* 功能描述： 设置VNA所属VNM信息
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1         V1.0       lvech         创建
***************************************************************/
void CVNetAgent::SetVNM(const string &vnm_name)
{
    if (m_strVNMApp != vnm_name)
    {
        //但归属的集群变化时，需要退出原来的集群，加入新的集群
        ExitVNMGroup();
        m_strVNMApp = vnm_name;
        JoinVNMGroup();
        VNET_LOG(VNET_LOG_INFO, "Set regitered VNM %s.\n", vnm_name.c_str());
    }

    g_strVNMApplication = vnm_name;

    BroadcastRegedVNM();
}

/************************************************************
* 函数名称： RecvHeartFromModule
* 功能描述： 处理MODULE模块向VNA发送心跳消息
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1         V1.0       lvech         创建
***************************************************************/
void CVNetAgent::RecvHeartFromModule(const MessageFrame &message)
{
    time_t cur_time = time(0);
    int32 iRet = 0;

    CVNetVNAHeartBeatMsg msg;
    msg.deserialize(message.data);

    if (m_bOpenDbgInf)
    {
        cout << "Receive CVNetVNAHeartBeatMsg message:\n" << endl;  
        msg.print();
    }
    
    // 更新MODULE心跳状态
    vector<TModuleInfo>::iterator it;
    for (it = m_vecModuleInfo.begin(); it != m_vecModuleInfo.end(); it++)
    {
        if ((msg.m_module_app == it->GetModuleName())
            && (msg.m_module_role == it->GetRole()))
        {
           it->SetLastHeartTime(cur_time); 
           it->SetState(VNET_MODULE_ONLINE); 
           break;
        }
    }

    //找不到对应module模块
    if (it == m_vecModuleInfo.end())
    {
        iRet = 1;
    }
    
    CVNetVNAHeartBeatAckMsg cAck;
    cAck.m_ret_code = iRet;
    cAck.m_module_app = msg.m_module_app;

    // 回复注册;
    MessageOption option(message.option.sender(), EV_VNETLIB_HEART_BEAT_MSG_ACK,0,0);
    m_pMU->Send(cAck, option);

    if (m_bOpenDbgInf)
    {
        cout << "Send CVNetVNAHeartBeatMsg ack:\n" << endl;  
        msg.print();
    }
}

/************************************************************
* 函数名称： CheckModuleState
* 功能描述： 检查MODULE模块的心跳状态
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1         V1.0       lvech         创建
***************************************************************/
void CVNetAgent::CheckModuleState()
{
    time_t cur_time = time(0);

    // 将心跳超时的MODULE置为离线状态
    vector<TModuleInfo>::iterator it;
    for (it = m_vecModuleInfo.begin(); it != m_vecModuleInfo.end(); it++)
    {
        if (cur_time - it->GetLastHeartTime() > VNET_MODULE_MAX_OFFLINE_TIME)
        {
            it->SetState(VNET_MODULE_OFFLINE); 
        }
    }
}

/************************************************************
* 函数名称： BroadcastRegedVNM
* 功能描述： 通告本VNA所属的VNM信息
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1         V1.0       lvech         创建
***************************************************************/
void CVNetAgent::BroadcastRegedVNM()
{
    MessageRegisteredSystem msg(m_strVNMApp);
    MID receiver("group", string(ApplicationName()) + MUTIGROUP_SUF_REG_SYSTEM);
    MessageOption option(receiver, EV_REGISTERED_SYSTEM, 0, 0);
    m_pMU->Send(msg, option);
}

/************************************************************
* 函数名称： RecvRegedVNMReq
* 功能描述： 处理请求VNA所属的VNM查询消息
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1         V1.0       lvech         创建
***************************************************************/
void CVNetAgent::RecvRegedVNMReq(const MessageFrame &message)
{
    if (m_bOpenDbgInf)
    {
        cout << "Receive RecvRegedVNMReq message:\n" << endl;  
    }
    
    MessageRegisteredSystem msg(m_strVNMApp);
    MessageOption option(message.option.sender(), EV_REGISTERED_SYSTEM, 0, 0);
    m_pMU->Send(msg, option);
}

/************************************************************
* 函数名称： HCRegisteredSystem
* 功能描述： HC通知注册成功函数
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1         V1.0       lvech         创建
***************************************************************/
int CVNetAgent::HCRegisteredSystem(const MessageFrame &message)
{
    time_t cur_time = time(0);

    MID sender = message.option.sender();
    
    SkyAssert(!sender._application.empty());
    
    CvnetVNAClusterMsg msg;
    msg.deserialize(message.data);

    if (m_bOpenDbgInf)
    {
        cout << "Receive HCRegisteredSystem message:\n" << endl;  
        msg.print();
    }
    
    //更新MODULE信息
    vector<TModuleInfo>::iterator it;
    for (it = m_vecModuleInfo.begin(); it != m_vecModuleInfo.end(); it++)
    {
        if (msg.m_module_app == it->GetModuleName())
        {
            it->SetExtInfo(msg.m_cluster_app);
            it->SetState(VNET_MODULE_ONLINE);
            it->SetLastHeartTime(cur_time);
            break;
        }
    }

    return 0;
}

string _hc_app = "";
string _cc_app = "";

/************************************************************
* 函数名称： GetModuleReportInfo
* 功能描述： 获取module上报信息
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1         V1.0       lvech         创建
***************************************************************/
int CVNetAgent::GetModuleReportInfo(vector<CVNARegModMsg> &vecModInfo)
{
    vecModInfo.clear();
    
    //添加MODULE信息
    vector<TModuleInfo>::iterator it;
    for (it = m_vecModuleInfo.begin(); it != m_vecModuleInfo.end(); it++)
    {
        CVNARegModMsg module;
        module.m_ucModRole = it->GetRole();     
        module.m_ucModState = it->GetState(); 
        module.m_strModApp = it->GetModuleName(); 
        module.m_strExtInfo = it->GetExtInfoe(); 

        vecModInfo.push_back(module);
    }

    //调试代码
    if ("" != _hc_app)
    {
        CVNARegModMsg module;
        module.m_ucModRole = VNA_MODULE_ROLE_HC;     
        module.m_ucModState = 1; 
        module.m_strModApp = _hc_app; 
        module.m_strExtInfo = _cc_app; 

        vecModInfo.push_back(module);
    }

    return 0;
};

int32 CVNetAgent::GetModuleInfo(int32 m_nRole, TModuleInfo &cModule)
{
    //添加MODULE信息
    vector<TModuleInfo>::iterator it;
    for (it = m_vecModuleInfo.begin(); it != m_vecModuleInfo.end(); it++)
    {
        if (m_nRole == it->GetRole())
        {
            break;
        }
    }

    if (it != m_vecModuleInfo.end())
    {
        if (VNET_MODULE_ONLINE != it->GetState())
        {
            return ERROR_VNA_AGENT_MODULE_OFFLINE;
        }
        else
        {
            cModule = *it;
            return 0;
        }
    }
    else
    {
        return ERROR_VNA_AGENT_MODULE_NOT_REG;
    }
};

void CVNetAgent::DbgShowData(void)
{
    string strVNAApp;
    
    if (GetVNAUUIDByVNA(strVNAApp))
    {
        return;
    }

    //打印VNA相关信息
    cout <<endl;    
    cout << "++++++++++++++++++++++VNA Agent Information BEGIN ++++++++++++++++++++++++" << endl;
    cout << "VNA Agent Information:" << endl;
    cout << "   VNA Application:" << strVNAApp << endl;
    cout << "   Registered VNM App:" << m_strVNMApp << endl;
    cout << endl;

    //打印MODULE相关信息
    cout << "Module Information:" << endl;
    vector<TModuleInfo>::iterator itModule = m_vecModuleInfo.begin();
    for (; itModule != m_vecModuleInfo.end(); itModule++)
    {
        itModule->Print();
    }
    cout << endl;

    cout << "++++++++++++++++++++++VNA Agent Information END ++++++++++++++++++++++++" << endl;
    cout << endl;
    return;
};

/************************************************************
* 函数名称： SetDbgFlag
* 功能描述： 调试信息开关
* 访问的表： 无
* 修改的表： 无
* 输入参数： 
* 输出参数： 
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1         V1.0       lvech         创建
***************************************************************/
void CVNetAgent::SetDbgFlag(int bDbgFlag)
{
    m_bOpenDbgInf = bDbgFlag;
};

//供开发测试用，停止开启心跳定时器
//nSwitchFlag 1:开启；0 :关闭
void CVNetAgent::SetReportSwitch(int32 nSwitchFlag)
{
    if (0 == nSwitchFlag)
    {
        m_pMU->StopTimer(m_nTimerID);
    }
    else
    {
        // 设置归属集群信息周期组播定时器
        TimeOut timeout;
        timeout.type = LOOP_TIMER;
        timeout.duration = TIMER_INTERVAL_VNA_REGISTER;
        timeout.msgid = TIMER_VNA_INFO_REPORT;
        if (SUCCESS != m_pMU->SetTimer(m_nTimerID, timeout))
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNetHeartbeat::MessageEntry Create timer for send registered vnm failed!\n");
            SkyAssert(0);
            SkyExit(-1, "CVNetHeartbeat::DoStartUp: call _mu->SetTimer failed.");
        }
    }  
}

int VNET_DBG_SET_VNA_REPORT_SWITCH(int32 nSwitchFlag)
{
    if ((0 != nSwitchFlag) && (1 != nSwitchFlag))
    {
        cout << "Invalid input parameter, only 0 and 1 is valid!" << endl;
        return -1;
    }
        
    CVNetAgent *pVNetHandle = CVNetAgent::GetInstance();
    if (pVNetHandle)
    {
        pVNetHandle->SetReportSwitch(nSwitchFlag);
    }
    
    return 0;
}
DEFINE_DEBUG_FUNC(VNET_DBG_SET_VNA_REPORT_SWITCH);

int VNET_DBG_SHOW_VNA_AGENT_INFO(void)
{
    CVNetAgent *pVNetHandle = CVNetAgent::GetInstance();
    if (pVNetHandle)
    {
        pVNetHandle->DbgShowData();
        cout << endl;
    }
    
    return 0;
}
DEFINE_DEBUG_FUNC(VNET_DBG_SHOW_VNA_AGENT_INFO);

void VNET_DBG_SET_VNA_CLUSTER_INFO(char * strHC, char *  strCC)
{
     _hc_app = strHC;
     _cc_app = strCC;
}
DEFINE_DEBUG_FUNC(VNET_DBG_SET_VNA_CLUSTER_INFO);


void VNetAgentDbgPrint(int32 i)
{
    CVNetAgent* pInstance =  CVNetAgent::GetInstance();
    if( NULL == pInstance )
    {
        cout << "CVNetAgent::GetInstance() is NULL" << endl;
        return;
    }
    pInstance->SetDbgFlag(i);
}
DEFINE_DEBUG_FUNC(VNetAgentDbgPrint);

} // namespace zte_tecs

