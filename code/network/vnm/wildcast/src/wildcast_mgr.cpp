/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：wildcast_mgr.cpp
* 文件标识：
* 内容摘要：CWildcastMgr类实现文件
* 当前版本：1.0
* 作    者：
* 完成日期：2013年1月18日
*
* 修改记录1：
*     修改日期：2013/1/18
*     版 本 号：V1.0
*     修 改 人：gongxiefeng
*     修改内容：创建
******************************************************************************/

#include "vnet_comm.h"
#include "vnet_event.h"
#include "vnet_msg.h"
#include "vnet_mid.h"
#include "vnetlib_event.h"
#include "vnet_error.h"
#include "wildcast_mgr.h"

namespace zte_tecs
{
#define MC_VNM_WILDCAST_INTERVAL    (10000)  // 10s扫描 扫描一次VNM通配信息;

CWildcastMgr *CWildcastMgr::s_pInstance = NULL;


CWildcastMgr::CWildcastMgr()
{
    m_tTimerID = INVALID_TIMER_ID;
    m_pMU = NULL;
    m_bOpenDbg = false;
}

CWildcastMgr::~CWildcastMgr()
{
    if(INVALID_TIMER_ID != m_tTimerID)
    {
        if(m_pMU)
        {
            m_pMU->KillTimer(m_tTimerID);  // 析构中此操作是否集中到一个下电函数中???;
            m_tTimerID = INVALID_TIMER_ID;
        }
    }
    m_pMU = NULL;
}

int32 CWildcastMgr::Init(MessageUnit *pMu)
{
    if (pMu == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastMgr::Init: pMu is NULL.\n");
        return -1;
    }

    m_pMU = pMu;

    // Init VNM information Muticast timer
    m_tTimerID = m_pMU->CreateTimer();
    if (INVALID_TIMER_ID == m_tTimerID)
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastMgr::Init: call CreateTimer failed.\n");
        return -1;
    }

    TimeOut timeout;
    timeout.type = LOOP_TIMER;
    timeout.duration = MC_VNM_WILDCAST_INTERVAL;
    timeout.msgid = EV_VNM_WILDCAST_TIMER;
    timeout.arg = 0;
    if(SUCCESS != m_pMU->SetTimer(m_tTimerID, timeout))
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastMgr::Init: call SetTimer failed.\n");
        m_pMU->KillTimer(m_tTimerID);
        m_tTimerID = INVALID_TIMER_ID;
        return -1;
    }
    
    VNET_LOG(VNET_LOG_INFO, "CWildcastMgr::Init: success.\n");
    return 0;
}
    
void CWildcastMgr::MessageEntry(const MessageFrame &message)
{
    if(m_bOpenDbg)
    {
        cout << "CWildcastMgr::MessageEntry recv msg id : "<< message.option.id() << endl;
    }
    switch (message.option.id())
    {
    case EV_VNM_WILDCAST_TIMER:
        {
            ProcTimeOut();
        }
        break;
    case EV_VNETLIB_WILDCAST_SWITCH_CFG:
        {
            ProcSwitchMsg(message);
        }
        break;  
    case EV_VNETLIB_WILDCAST_SWITCH_PORT_CFG:
        {
            ProcSwitchPortMsg(message);
        }
        break;  

    case EV_VNETLIB_WILDCAST_CREATE_VPORT_CFG:
        {
            ProcCreateVPortMsg(message);
        }
        break;  
    case EV_VNETLIB_WILDCAST_LOOPBACK_CFG:
        {
            ProcLoopbackMsg(message);
        }
        break;          

    // task 
    case EV_VNETLIB_WILDCAST_SWITCH_TASK_DEL:
        {
            ProcSwitchTaskMsg(message);
        }
        break; 
    case EV_VNETLIB_WILDCAST_CREATE_VPORT_TASK_DEL:
        {
            ProcCreateVPortTaskMsg(message);
        }
        break; 
    case EV_VNETLIB_WILDCAST_LOOPBACK_TASK_DEL:
        {
            ProcLoopbackTaskMsg(message);
        }
        break;
    default:
        break;            
    }
    
    return ;
} 


/*******************************************************************************
* 函数名称： ProcSwitchReqMsg
* 功能描述： 处理交换配置请求消息
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* ------------------------------------------------------------------------------
* 2013/3          V1.0      gongxf      创 建
*******************************************************************************/

STATUS CWildcastMgr::ProcSwitchMsg(const MessageFrame& message)
{
    CWildcastSwitchCfgMsg    cMsg;
    CWildcastSwitchCfgAckMsg cAck;
    cMsg.deserialize(message.data);
    MID   mid  = message.option.sender();
    MessageOption option(message.option.sender(), EV_VNETLIB_WILDCAST_SWITCH_CFG_ACK, 0, 0);

    if(m_bOpenDbg)
    {
        cout << "recv wildcast switch msg......" << endl;
        cMsg.Print();
    }
    
    int32 nRet = WC_SWITCH_OPER_SUCCESS;

    nRet = m_VSwitch.ProcSwitchMsg(cMsg);
    if(WC_SWITCH_OPER_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_INFO, "CWildcastMgr::ProcSwitchMsg Switch->ProcSwitchReqMsg fail, ret = %d\n", nRet);
    }
    else
    {
        goto PROC_SWITCH_ACK;
    }
    
PROC_SWITCH_ACK:
    cAck.m_strName  = cMsg.m_strName;
    cAck.m_strUuid  = cMsg.m_strUuid;
    cAck.m_nOper    = cMsg.m_nOper;
    cAck.m_strReqId    = cMsg.m_strReqId;
    cAck.SetResult(nRet);
    cAck.SetResultinfo(GetVnetErrorInfo(nRet));
    if(m_bOpenDbg)
    {
        cout << "send wildcast switch ack msg......" << endl;
        cAck.Print();
    }
    return m_pMU->Send(cAck, option);
}

STATUS CWildcastMgr::ProcSwitchTaskMsg(const MessageFrame& message)
{
    CWildcastTaskDelMsg    cMsg;
    CWildcastTaskDelAckMsg cAck;
    cMsg.deserialize(message.data);
    MID   mid  = message.option.sender();
    MessageOption option(message.option.sender(), EV_VNETLIB_WILDCAST_SWITCH_TASK_DEL_ACK, 0, 0);

    if(m_bOpenDbg)
    {
        cout << "recv wildcast switch task msg......" << endl;
        cMsg.Print();
    }
    
    int32 nRet = WC_SWITCH_OPER_SUCCESS;
    
    nRet = m_VSwitch.ProcSwitchTaskMsg(cMsg.m_strUuid);
    if(WC_SWITCH_OPER_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_INFO, "CWildcastMgr::ProcSwitchTaskMsg Switch->ProcSwitchTaskMsg(%s) fail, ret = %d\n",
          cMsg.m_strUuid.c_str(), nRet);
    }
    else
    {
        goto PROC_SWITCH_TASK_ACK;
    }
    
PROC_SWITCH_TASK_ACK:
    cAck.m_strUuid  = cMsg.m_strUuid;
    cAck.m_nOper    = cMsg.m_nOper;
    cAck.m_strReqId    = cMsg.m_strReqId;
    cAck.SetResult(nRet);
    cAck.SetResultinfo(GetVnetErrorInfo(nRet));
    if(m_bOpenDbg)
    {
        cout << "send wildcast switch task ack msg......" << endl;
        cAck.Print();
    }
    return m_pMU->Send(cAck, option);
}

STATUS CWildcastMgr::ProcSwitchPortMsg(const MessageFrame& message)
{
    CWildcastSwitchPortCfgMsg cMsg;
    CWildcastSwitchCfgAckMsg  cAck;
    cMsg.deserialize(message.data);
    int32 nRet =  WC_SWITCH_OPER_SUCCESS;
    MID   mid  =  message.option.sender();
    MessageOption option(mid, EV_VNETLIB_WILDCAST_SWITCH_PORT_CFG_ACK, 0, 0);

    //入参检查
    if(cMsg.m_strSwName.empty() && cMsg.m_strSwUuid.empty())
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastMgr::ProcSwitchPortMsg Switch name&uuid is invalid.\n");
        nRet = ERROR_WC_SWITCH_NAME_EMPTY;
        goto PROC_SWITCHPORT_ACK;
    }

    if(m_bOpenDbg)
    {
        cout << "recv wildcast switch port msg......" << endl;
        cMsg.Print();
    }

    nRet = m_VSwitch.ProcSwitchPortMsg(cMsg);
    if(SWITCH_OPER_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_INFO, "CWildcastMgr::DoSwitchAddReq Switch->ProcSwitchReqMsg fail, ret = %d\n", nRet);
    }
    else
    {
        goto PROC_SWITCHPORT_ACK;
    }    
        
PROC_SWITCHPORT_ACK:
    cAck.m_strName  = cMsg.m_strSwName;
    cAck.m_strUuid  = cMsg.m_strSwUuid;
    cAck.m_strReqId = cMsg.m_strReqId;
    cAck.m_nOper    = cMsg.m_nOper;    
    cAck.SetResult(nRet);
    cAck.SetResultinfo(GetVnetErrorInfo(nRet));
    if(m_bOpenDbg)
    {
        cout << "send wildcast switch port ack msg......" << endl;
        cAck.Print();
    }
    return m_pMU->Send(cAck, option);
}

STATUS CWildcastMgr::ProcCreateVPortMsg(const MessageFrame& message)
{
    CWildcastCreateVPortCfgMsg    cMsg;
    CWildcastCreateVPortCfgAckMsg cAck;
    cMsg.deserialize(message.data);
    MID   mid  = message.option.sender();
    MessageOption option(message.option.sender(), EV_VNETLIB_WILDCAST_CREATE_VPORT_CFG_ACK, 0, 0);

    if(m_bOpenDbg)
    {
        cout << "recv wildcast create vport msg......" << endl;
        cMsg.Print();
    }
    
    int32 nRet = WC_CREATE_VPORT_OPER_SUCCESS;
    
    nRet = m_CreateVPort.ProcMsg(cMsg);
    if(WC_SWITCH_OPER_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_INFO, "CWildcastMgr::ProcCreateVPortMsg m_CreateVPort.ProcMsg fail, ret = %d\n", nRet);
    }
    else
    {
        goto PROC_CREATE_VPORT_ACK;
    }    
    
PROC_CREATE_VPORT_ACK:
    cAck.m_strPortName  = cMsg.m_strPortName;
    cAck.m_strUuid  = cMsg.m_strUuid;
    cAck.m_nOper    = cMsg.m_nOper;

    cAck.m_nIsDhcp  = cMsg.m_nIsDhcp;
    cAck.m_nIsHasKenelType = cMsg.m_nIsHasKenelType;
    cAck.m_nKernelType = cMsg.m_nKernelType;
    cAck.m_nPortType = cMsg.m_nPortType;
    cAck.m_strKernelPgUuid =cMsg.m_strKernelPgUuid;
    cAck.m_strSwitchUuid = cMsg.m_strSwitchUuid;
    
    cAck.SetResult(nRet);
    cAck.SetResultinfo(GetVnetErrorInfo(nRet));   
    if(m_bOpenDbg)
    {
        cout << "send wildcast create vport ack msg......" << endl;
        cAck.Print();
    }
    return m_pMU->Send(cAck, option);
}

STATUS CWildcastMgr::ProcCreateVPortTaskMsg(const MessageFrame& message)
{
    CWildcastTaskDelMsg    cMsg;
    CWildcastTaskDelAckMsg cAck;
    cMsg.deserialize(message.data);
    MID   mid  = message.option.sender();
    MessageOption option(message.option.sender(), EV_VNETLIB_WILDCAST_CREATE_VPORT_TASK_DEL_ACK, 0, 0);

    if(m_bOpenDbg)
    {
        cout << "recv wildcast create vport task msg......" << endl;
        cMsg.Print();
    }
    
    int32 nRet = WC_CREATE_VPORT_OPER_SUCCESS;
    
    nRet = m_CreateVPort.ProcTaskMsg(cMsg.m_strUuid);
    if(WC_SWITCH_OPER_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_INFO, "CWildcastMgr::ProcCreateVPortMsg m_CreateVPort.ProcTaskMsg(%s) fail, ret = %d\n",
            cMsg.m_strUuid.c_str(),nRet);
    }
    else
    {
        goto PROC_CREATE_VPORT_ACK;
    }    
    
PROC_CREATE_VPORT_ACK:
    cAck.m_strUuid  = cMsg.m_strUuid;
    cAck.m_nOper    = cMsg.m_nOper;
    cAck.m_strReqId    = cMsg.m_strReqId;
    cAck.SetResult(nRet);
    cAck.SetResultinfo(GetVnetErrorInfo(nRet));   
    if(m_bOpenDbg)
    {
        cout << "send wildcast create vport task ack msg......" << endl;
        cAck.Print();
    }
    return m_pMU->Send(cAck, option);
}

STATUS CWildcastMgr::ProcLoopbackMsg(const MessageFrame& message)
{
    CWildcastLoopbackCfgMsg    cMsg;
    CWildcastLoopbackCfgAckMsg cAck;
    cMsg.deserialize(message.data);
    MID   mid  = message.option.sender();
    MessageOption option(message.option.sender(), EV_VNETLIB_WILDCAST_LOOPBACK_CFG_ACK, 0, 0);

    if(m_bOpenDbg)
    {
        cout << "recv wildcast sriov loopback msg......" << endl;
        cMsg.Print();
    }
    
    int32 nRet = WC_LOOPBACK_OPER_SUCCESS;
    
    nRet = m_Loopback.ProcMsg(cMsg);
    if(WC_LOOPBACK_OPER_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_INFO, "CWildcastMgr::ProcLoopbackMsg m_Loopback.ProcMsg fail, ret = %d\n", nRet);
    }
    else
    {
        goto PROC_LOOPBACK_ACK;
    }    
    
PROC_LOOPBACK_ACK:
    cAck.m_strPortName  = cMsg.m_strPortName;
    cAck.m_strUuid  = cMsg.m_strUuid;
    cAck.m_nIsLoop  = cMsg.m_nIsLoop;
    cAck.m_nOper    = cMsg.m_nOper;
    cAck.SetResult(nRet);
    cAck.SetResultinfo(GetVnetErrorInfo(nRet));   
    if(m_bOpenDbg)
    {
        cout << "recv wildcast sriov loopback ack msg......" << endl;
        cAck.Print();
    }
    return m_pMU->Send(cAck, option);
}


STATUS CWildcastMgr::ProcLoopbackTaskMsg(const MessageFrame& message)
{
    CWildcastTaskDelMsg    cMsg;
    CWildcastTaskDelAckMsg cAck;
    cMsg.deserialize(message.data);
    MID   mid  = message.option.sender();
    MessageOption option(message.option.sender(), EV_VNETLIB_WILDCAST_LOOPBACK_TASK_DEL_ACK, 0, 0);

    if(m_bOpenDbg)
    {
        cout << "recv wildcast sriov loopback task msg......" << endl;
        cMsg.Print();
    }
    
    int32 nRet = WC_LOOPBACK_OPER_SUCCESS;
    
    nRet = m_Loopback.ProcTaskMsg(cMsg.m_strUuid);
    if(WC_SWITCH_OPER_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_INFO, "CWildcastMgr::ProcLoopbackTaskMsg m_Loopback.ProcTaskMsg(%s) fail, ret = %d\n",
            cMsg.m_strUuid.c_str(), nRet);
    }
    else
    {
        goto PROC_LOOPBACK_ACK;
    }    
    
PROC_LOOPBACK_ACK:
    cAck.m_strUuid  = cMsg.m_strUuid;
    cAck.m_nOper    = cMsg.m_nOper;
    cAck.m_strReqId    = cMsg.m_strReqId;
    cAck.SetResult(nRet);
    cAck.SetResultinfo(GetVnetErrorInfo(nRet));  
    if(m_bOpenDbg)
    {
        cout << "send wildcast sriov loopback task ack msg......" << endl;
        cAck.Print();
    }
    return m_pMU->Send(cAck, option);
}

int32 CWildcastMgr::ProcTimeOut()
{
    //cout << "CWildcastMgr::ProcTimeOut~~~~" << endl;
    if(m_bOpenDbg)
    {
        cout << "CWildcastMgr::ProcTimeOut~~~~" << endl;
    }
    //VNET_LOG(VNET_LOG_INFO, "CWildcastMgr::ProcTimeOut~~~.\n");
        
    m_VSwitch.ProcTimeout();
    m_CreateVPort.ProcTimeout();
    m_Loopback.ProcTimeout();
    return 0;
}

void CWildcastMgr::NewVnaRegist(const string & strVnaUuid)
{
    // 扫描switch,loop port, create vport,
    if(m_bOpenDbg)
    {
        cout << "CWildcastMgr::NewVnaRegist(" << strVnaUuid << ")~~~~" << endl;
    }
    VNET_LOG(VNET_LOG_INFO, "CWildcastMgr::NewVnaRegist(%s).\n",strVnaUuid.c_str());
    m_VSwitch.NewVnaRegist(strVnaUuid);
    m_CreateVPort.NewVnaRegist(strVnaUuid);
    m_Loopback.NewVnaRegist(strVnaUuid);
}

void CWildcastMgr::WildcastTaskMsgAck(int32 wcTaskType,const string & strTaskUuid, int32 nResult)
{
    if( wcTaskType < WILDCAST_TASK_TYPE_SWITCH || wcTaskType > WILDCAST_TASK_TYPE_LOOPBACK )
    {
        VNET_LOG(VNET_LOG_ERROR, "CWildcastMgr::WildcastTaskMsgAck wcTaskType(%d) invalid.\n",wcTaskType);
        return;
    }
    if(m_bOpenDbg)
    {
        VNET_LOG(VNET_LOG_INFO, "CWildcastMgr::WildcastTaskMsgAck(%d,%s,%d).\n",
           wcTaskType,strTaskUuid.c_str(),nResult);
    }

    if( wcTaskType == WILDCAST_TASK_TYPE_SWITCH )
    {
        m_VSwitch.WildcastTaskMsgAck(strTaskUuid,nResult);
    }
    if( wcTaskType == WILDCAST_TASK_TYPE_CREATE_VPORT )
    {
        m_CreateVPort.WildcastTaskMsgAck(strTaskUuid,nResult);
    }
    if( wcTaskType == WILDCAST_TASK_TYPE_LOOPBACK )
    {
        m_Loopback.WildcastTaskMsgAck(strTaskUuid,nResult);
    }
}

int32 CWildcastMgr::SwitchNameIsConflict( const string & swName, int32 & outResult)
{
    return m_VSwitch.IsConflictSwitchName(swName,outResult);
}

int32 CWildcastMgr::SwitchPortIsConflict(const string & vnaUuid, const string & swName, int32 swType, 
            vector<string> & vPortName, int32 & outResult)
{
    return m_VSwitch.IsConflictSwitchPort(vnaUuid,swName,swType,vPortName,outResult);
}

int32 CWildcastMgr::CreateVPortIsConflict(const string & vnaUuid, const string & portName,int32 & outResult)
{
    return m_CreateVPort.IsConflict(vnaUuid,portName,outResult);
}

int32 CWildcastMgr::LoopbackIsConflict(const string & vnaUuid, const string & portName,int32 & outResult)
{
    return m_Loopback.IsConflict(vnaUuid,portName,outResult);
}


void CWildcastMgr::DbgShow(void)
{
    cout << "----------------------------------------------------------------------------" << endl;
    
    return ;
}

void VNetDbgWildcastPrint(int32 i)
{
    CWildcastMgr *pWcMgr = CWildcastMgr::GetInstance();
    if(NULL == pWcMgr)
    {
        cout << "CWildcastMgr::GetInstance() is NULL." << endl;
    }
    pWcMgr->SetDbgPrintFlag(i);
    return ;
}
DEFINE_DEBUG_FUNC(VNetDbgWildcastPrint);



}// end namespace zte_tecs

