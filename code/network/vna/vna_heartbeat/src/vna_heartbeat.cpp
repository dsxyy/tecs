/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：vna_heartbeat.cpp
* 文件标识：见配置管理计划书
* 内容摘要：CVNetHeartbeat类实现文件
* 当前版本：1.0
* 作    者：lvech
* 完成日期：2013年1月9日
*
* 修改记录1：
*     修改日期：2013/1/9
*     版 本 号：V1.0
*     修 改 人：lvech
*     修改内容：创建
*******************************************************************************/
#include "tecs_config.h"
#include "registered_system.h"
#include "vna_common.h"
#include "vna_heartbeat.h"

namespace zte_tecs
{

CVNetHeartbeat *CVNetHeartbeat::_instance = NULL;

/************************************************************
* 函数名称： CVNetHeartbeat
* 功能描述： CVNetHeartbeat构造函数
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
CVNetHeartbeat::CVNetHeartbeat():
        m_strVNMName()
{
    m_bOpenDbgInf = VNET_FALSE;
    m_pMU = NULL;
    m_nHeartTimer = INVALID_TIMER_ID;
}

/************************************************************
* 函数名称： ~CVNetHeartbeat
* 功能描述： CVNetHeartbeat析构函数
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
CVNetHeartbeat::~CVNetHeartbeat()
{
    if (NULL != m_pMU)
    {
        delete m_pMU;
    }

    m_nHeartTimer = INVALID_TIMER_ID;
}

/************************************************************
* 函数名称： Init
* 功能描述： CVNetHeartbeat对象初始化函数
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
STATUS CVNetHeartbeat::Init()
{
        return StartMu(MU_VNA_HEARTBEAT);
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
STATUS CVNetHeartbeat::StartMu(const string& name)
{
    if (m_pMU)
    {
        SkyAssert(false);
        return ERROR_DUPLICATED_OP;
    }
    
    // 消息单元的创建和初始化
    m_pMU = new MessageUnit(name);
    if (!m_pMU)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetHeartbeat::StartMu Create mu %s failed!\n", name.c_str());
        return ERROR_NO_MEMORY;
    }

    STATUS ret = m_pMU->SetHandler(this);
    if (SUCCESS != ret)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetHeartbeat::StartMu SetHandler mu %s failed! ret = %d\n", name.c_str(),ret);
        return ret;
    }

    ret = m_pMU->Run();
    if (SUCCESS != ret)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetHeartbeat::StartMu Run mu %s failed! ret = %d\n", name.c_str(),ret);
        return ret;
    }

    ret = m_pMU->Register();
    if (SUCCESS != ret)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetHeartbeat::StartMu Register mu %s failed! ret = %d\n", name.c_str(),ret);
        return ret;
    }

    m_nHeartTimer = m_pMU->CreateTimer();
    if (INVALID_TIMER_ID == m_nHeartTimer)
    {
        return ERROR_NO_TIMER;
    }
    
    //给自己发送上电消息，促使消息单元状态切换到工作态
    MessageFrame message(SkyInt32(0), EV_POWER_ON);
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
* 函数名称： MessageEntry
* 功能描述： 消息主分发函数
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
void CVNetHeartbeat::MessageEntry(const MessageFrame &message)
{
    switch (m_pMU->get_state())
    {
    case S_Startup:
    {
        switch (message.option.id())
        {
        case EV_POWER_ON:
        {
            DoStartUp();
            m_pMU->set_state(S_Work);
            VNET_LOG(VNET_LOG_INFO, "CVNetHeartbeat::MessageEntry %s power on!\n", m_pMU->name().c_str());
            break;
        }
        default:
            VNET_LOG(VNET_LOG_INFO, "CVNetHeartbeat::MessageEntry Ignore any message except power on!\n");
            break;
        }
        break;
    }

    case S_Work:
    {
        switch (message.option.id())
        {
        case TIMER_VNA_HEARTBEAT_REPORT:
        {
            DoHeartbeatReport();
            break;
        }
        
        case EV_REGISTERED_SYSTEM:
        {
            DoRegisterSystem(message);
            break;
        }
                
        default:
        {
            cout << "CVNetHeartbeat::MessageEntry: receive an unkown message!"
            << endl;
            VNET_LOG(VNET_LOG_INFO, "CVNetHeartbeat::MessageEntry Unknown message: %d.\n", message.option.id());
            break;
        }
        }
        break;
    }

    default:
        break;
    }
}

/************************************************************
* 函数名称： DoStartUp
* 功能描述： 启动心跳
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
void CVNetHeartbeat::DoStartUp()
{
    // 设置归属集群信息周期组播定时器
    TimeOut timeout;
    timeout.type = LOOP_TIMER;
    timeout.duration = TIMER_INTERVAL_VNA_HEARTBEAT_REPORT;
    timeout.msgid = TIMER_VNA_HEARTBEAT_REPORT;
    if (SUCCESS != m_pMU->SetTimer(m_nHeartTimer, timeout))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetHeartbeat::MessageEntry Create timer for send registered vnm failed!\n");
        SkyAssert(0);
        SkyExit(-1, "CVNetHeartbeat::DoStartUp: call _mu->SetTimer failed.");
    }

    // 加入组播组
    STATUS ret = m_pMU->JoinMcGroup(string(ApplicationName()) + MUTIGROUP_SUF_REG_SYSTEM);
    if (SUCCESS != ret)
    {
        SkyAssert(false);
        string lastwords("CVNetHeartbeat failed to join group! ret = ");
        lastwords.append(to_string(ret,dec));
        SkyExit(-1,lastwords);
    }

    // 获取归属的VNM名称
    GetRegisteredSystem(m_strVNMName);

    if (m_strVNMName.empty())
    {
        return;
    }
    else
    {
        VNET_LOG(VNET_LOG_INFO, "CVNetHeartbeat::MessageEntry Goto report, have registered into %s.\n",
               m_strVNMName.c_str());
        DoHeartbeatReport();
    }
}

/************************************************************
* 函数名称： DoRegisterSystem
* 功能描述： 设置注册的VNM信息
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
void CVNetHeartbeat::DoRegisterSystem(const MessageFrame &message)
{
    // 对消息发送方进行合法性检查
    if (message.option.sender()._application != ApplicationName())
    {
        SkyAssert(0);
        return;
    }

    MessageRegisteredSystem msg;
    msg.deserialize(message.data);
    m_strVNMName = msg._system_name;
}

/************************************************************
* 函数名称： DoHeartbeatReport
* 功能描述： 上报心跳
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
void CVNetHeartbeat::DoHeartbeatReport()
{
    GetRegisteredSystem(m_strVNMName);

    if (m_strVNMName.empty())
    {
        VNET_LOG(VNET_LOG_INFO, "CVNetHeartbeat::DoHeartbeatReport  VNM is NULL.\n");   
        return;
    }

    // 发送消息
    MID receiver;
    receiver._application = m_strVNMName;
    receiver._process = PROC_VNM;
    receiver._unit = MU_VNM;
    MessageOption option(receiver, EV_VNA_HEARTBEAT_REPORT, 0, 0);
    m_pMU->Send("", option);

    if (m_bOpenDbgInf)
    {
        cout << "CVNetHeartbeat::DoHeartbeatReport send msg to VNM(" << m_strVNMName << ")" <<endl;  
    }
}

//供开发测试用，停止开启心跳定时器
//nSwitchFlag 1:开启；0 :关闭
void CVNetHeartbeat::SetReportSwitch(int32 nSwitchFlag)
{
    if (0 == nSwitchFlag)
    {
        m_pMU->StopTimer(m_nHeartTimer);
    }
    else
    {
        // 设置归属集群信息周期组播定时器
        TimeOut timeout;
        timeout.type = LOOP_TIMER;
        timeout.duration = TIMER_INTERVAL_VNA_HEARTBEAT_REPORT;
        timeout.msgid = TIMER_VNA_HEARTBEAT_REPORT;
        if (SUCCESS != m_pMU->SetTimer(m_nHeartTimer, timeout))
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNetHeartbeat::MessageEntry Create timer for send registered vnm failed!\n");
            SkyAssert(0);
            SkyExit(-1, "CVNetHeartbeat::DoStartUp: call _mu->SetTimer failed.");
        }
    }  
}

int VNET_DBG_SET_VNA_HEART_SWITCH(int32 nSwitchFlag)
{
    if ((0 != nSwitchFlag) && (1 != nSwitchFlag))
    {
        cout << "Invalid input parameter, only 0 and 1 is valid!" << endl;
        return -1;
    }
        
    CVNetHeartbeat *pVNetHandle = CVNetHeartbeat::GetInstance();
    if (pVNetHandle)
    {
        pVNetHandle->SetReportSwitch(nSwitchFlag);
    }
    
    return 0;
}
DEFINE_DEBUG_FUNC(VNET_DBG_SET_VNA_HEART_SWITCH);
} // namespace zte_tecs

