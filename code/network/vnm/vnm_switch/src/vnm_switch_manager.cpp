/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：vnm_switch_manager.cpp
* 文件标识：
* 内容摘要：vnm上交换管理模块
* 当前版本：1.0
* 作    者： 
* 完成日期： 
******************************************************************************/
#include "vnet_error.h"
#include "vnetlib_msg.h"
#include "vnetlib_event.h"
#include "vnet_event.h"
#include "vnet_vnic.h"
#include "ipv4_addr.h"
#include "ipv4_addr_range.h"
#include "ipv4_addr_pool.h"
#include "ipv4_pool_mgr.h"
#include "logic_network.h"
#include "logic_network_mgr.h"
#include "vnm_switch.h"
#include "vnm_switch_manager.h"
#include "vnet_mid.h"
#include "port_group.h"
#include "vnet_portmgr.h"
#include "vnm_vxlan_mgr.h"

namespace zte_tecs
{

CSwitchMgr* CSwitchMgr::m_pInstance = NULL;
CSwitchMgr::CSwitchMgr()
{
    m_pSwitch  = NULL;
    m_bOpenDbg = VNET_FALSE;
}
CSwitchMgr::~CSwitchMgr()
{   
    delete m_pSwitch;
    m_pInstance = NULL;
    m_pMU       = NULL;
}
    
/*******************************************************************************
* 函数名称： Init
* 功能描述： 初始化函数
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* ------------------------------------------------------------------------------
* 2013/3          V1.0       lvech         创建
*******************************************************************************/
int32 CSwitchMgr::Init(MessageUnit *mu)
{
    if (mu == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::Init: pMu is NULL.\n");
        return -1;
    }
    
    m_pPGMgr = CPortGroupMgr::GetInstance();
    if(NULL == m_pPGMgr)
    {
        VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::Init: m_pPGMgr is NULL.\n");
        return -1;
    }
    
    m_pMU = mu;

    m_pSwitch = GetSwitchInstance(SOFT_VIR_SWITCH_TYPE);
    if(NULL == m_pSwitch)
    {
        VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::Init: m_pSwitch is NULL.\n");
        return -1;
    }

    m_pWCMgr = CWildcastMgr::GetInstance();
    if(NULL == m_pWCMgr)
    {
        VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::Init: m_pWCMgr is NULL.\n");
        return -1;
    }
    
    //清除所有vna的交换信息，VNA上线后重新配置
    int32 nRet = m_pSwitch->ModifySwitchPort();
    if(SWITCH_OPER_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::Init: call ModifySwitchPort fail, ret = %d\n", nRet);
    }
    
    return 0;
}

/*******************************************************************************
* 函数名称： MessageEntry
* 功能描述： 消息入口函数
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* ------------------------------------------------------------------------------
* 2013/3          V1.0       lvech         创建
*******************************************************************************/
void CSwitchMgr::MessageEntry(const MessageFrame& message)
{
    switch(message.option.id())
    {
        case EV_VNETLIB_SWITCH_CFG:
        {
            ProcSwitchReqMsg(message);
            break;
        }
        case EV_VNETLIB_SWITCH_PORT_CFG:
        {
            ProcSwitchPortMsg(message);
            break;
        }
        case EV_VNETLIB_SWITCH_CFG_ACK:
        case EV_VNETLIB_SWITCH_PORT_CFG_ACK:
        {
            ProcSwitchAckMsg(message);
            break;
        }
        case EV_SWITCH_UPDATE_VNA_TIMER:
        {
            ProcUpdateVNA();
            break;
        }
        case EV_SWITCH_MODIFY_TIMER:
        {
            ProcUpdateSwitch();
            break;
        }
        default:
            break;
    }
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
* 2013/3          V1.0      zhongchsh      创 建
*******************************************************************************/
int32 CSwitchMgr::ProcSwitchReqMsg(const MessageFrame& message)
{
    CSwitchCfgMsg    cMsg;
    CSwitchCfgAckMsg cAck;
    cMsg.deserialize(message.data);
    string strRetInfo;
    int32 nRet = SWITCH_OPER_SUCCESS;
    MID   mid  = message.option.sender();
    MessageOption option(message.option.sender(), EV_VNETLIB_SWITCH_CFG_ACK, 0, 0);

    if(m_bOpenDbg)
    {
        cMsg.Print();
    }
    
    //入参检查
    if(cMsg.m_strName.empty() && cMsg.m_strUuid.empty())
    {
        VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::ProcSwitchReqMsg Switch name/uuid is invalid.\n");
        VNetFormatString(strRetInfo, "Switch name(%d) is invalid.", cMsg.m_strName.c_str());
        nRet = ERROR_SWITCH_NAME_INVALID;
        goto PROC_SWITCH_ACK;
    }

    switch(cMsg.m_nOper)
    {
        case EN_ADD_SWITCH:
        {
            nRet = DoSwitchAddReq(cMsg, strRetInfo);
            break;
        }
        case EN_MODIFY_SWITCH:
        {
            nRet = DoSwitchModifyReq(cMsg, strRetInfo);
            break;
        }
        case EN_DEL_SWITCH:
        {
            nRet = DoSwitchDelReq(cMsg, mid, strRetInfo);
            if(SWITCH_OPER_SUCCESS == nRet)
            {
                return nRet; //消息发给vNA，等待VNA应答。   
            }
            break;
        }
        default:
        {
            VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::ProcSwitchReqMsg: Msg operation code (%d) is invalid.\n", cMsg.m_nOper);  
            nRet = ERROR_SWITCH_OPERCODE_INVALID;
            VNetFormatString(strRetInfo, "Switch operator(%d) is invalid.", cMsg.m_nOper);
        }    
    }
    
PROC_SWITCH_ACK:
    cAck.m_strName  = cMsg.m_strName;
    cAck.m_strUuid  = cMsg.m_strUuid;
    cAck.m_nOper    = cMsg.m_nOper;
    cAck.SetResult(nRet);
    cAck.SetResultinfo(strRetInfo);
    return m_pMU->Send(cAck, option);
}

int32 CSwitchMgr::ProcSwitchPortMsg(const MessageFrame& message)
{
    CSwitchPortCfgMsg cMsg;
    CSwitchCfgAckMsg  cAck;
    cMsg.deserialize(message.data);
    
    string strRetInfo;
    int32 nRet =  SWITCH_OPER_SUCCESS;
    MID   mid  =  message.option.sender();
    MessageOption option(mid, EV_VNETLIB_SWITCH_PORT_CFG_ACK, 0, 0);

    //入参检查
    if(cMsg.m_strSwName.empty() && cMsg.m_strSwUuid.empty())
    {
        VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::ProcSwitchPortMsg Switch name&uuid is invalid.\n");
        VNetFormatString(strRetInfo, "Switch name(%s) is invalid.", cMsg.m_strSwName.empty());
        nRet = ERROR_SWITCH_NAME_INVALID;
        goto PROC_SWITCHPORT_ACK;
    }
    
    if(cMsg.m_vecHostPort.empty())
    {
        VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::ProcSwitchPortMsg: host Port is empty.\n");  
        VNetFormatString(strRetInfo, "Switch uplink port is empty.");
        nRet = ERROR_SWTICH_PORT_INVALID;
        goto PROC_SWITCHPORT_ACK;
    }
    
    if(m_bOpenDbg)
    {
        cMsg.Print();
    }
    
    switch(cMsg.m_nOper)
    {
        case EN_ADD_SWITCH_PORT:
        {
            nRet = DoSwitchPortAddReq(cMsg, mid, strRetInfo);
            if(SWITCH_OPER_SUCCESS == nRet)
            {
                return nRet; //消息发给vNA，等待VNA应答。   
            }
            break;
        }
        case EN_DEL_SWITCH_PORT:
        {
            nRet = DoSwitchPortDelReq(cMsg, mid, strRetInfo);
            if(SWITCH_OPER_SUCCESS == nRet)
            {
                return nRet; //消息发给vNA，等待VNA应答。   
            }
            break;
        }
        default:
        {
            VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::ProcSwitchPortMsg: Msg operation code (%d) is invalid.\n", 
            cMsg.m_nOper);  
            VNetFormatString(strRetInfo, "Switch operator(%d) is invalid.", cMsg.m_nOper);
            nRet = ERROR_SWITCH_OPERCODE_INVALID;
        }    
    }
    
PROC_SWITCHPORT_ACK:
    cAck.m_strName  = cMsg.m_strSwName;
    cAck.m_strUuid  = cMsg.m_strSwUuid;
    cAck.m_strReqId = cMsg.m_strReqId;
    cAck.m_nOper    = cMsg.m_nOper;
    cAck.SetResult(nRet);
    cAck.SetResultinfo(strRetInfo);
    return m_pMU->Send(cAck, option);
}

int32 CSwitchMgr::ProcSwitchAckMsg(const MessageFrame& message)
{
    CSwitchCfgMsgToVNA cVNAMsg;
    cVNAMsg.deserialize(message.data);

    CSwitchCfgAckMsg cAck;
    MID receiver = cVNAMsg.m_apisvr;
    MessageOption option(receiver, message.option.id(), 0, 0);

    if(m_bOpenDbg)
    {
        cVNAMsg.Print();
    }

    if(cVNAMsg.m_strName.empty() || cVNAMsg.m_strUuid.empty())
    {
        VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::ProcSwitchAckMsg Switch name is empty.\n");
        cVNAMsg.m_nRetCode = ERROR_SWITCH_NAME_INVALID;
        goto SWITCH_CFG_ACK;
    }

    switch(cVNAMsg.m_nOper)
    {
        case EN_MODIFY_SWITCH:
        {
            //内部处理，处理异常则告警
            if(SWITCH_OPER_SUCCESS == cVNAMsg.m_nRetCode)
            {
                DoSwitchModifyAck(cVNAMsg);
            }
            //这种情况已经产生脏数据可以不销毁?
            #if 0
            //vxlan资源释放    
            if((SWITCH_OPER_SUCCESS != cVNAMsg.m_nRetCode) && (!cVNAMsg.m_strVtepPort.empty()))
            {
                CPortMgr *m_pPortMgr = CPortMgr::GetInstance();
                if(NULL != m_pPortMgr)
                {
                    m_pPortMgr->DelVtepPort(cVNAMsg.m_strVnaUuid, cVNAMsg.m_strVtepPort);
                } 
            }
            #endif 
            return 0;
        }
        case EN_DEL_SWITCH:
        {
            //有一个VNA删除失败，则交换删除失败
            if((SWITCH_OPER_SUCCESS == cVNAMsg.m_nRetCode))
            {
                cVNAMsg.m_nRetCode = DoSwitchDelAck(cVNAMsg);
                if(ERROR_SWITCH_DEL_NOT_FINISH == cVNAMsg.m_nRetCode)
                {
                    //尚未删除所有VNA上DVS
                    return 0; 
                }
            }
            
            break;
        }
        case EN_ADD_SWITCH_PORT:
        {
            if(SWITCH_OPER_SUCCESS == cVNAMsg.m_nRetCode) 
            {
                cVNAMsg.m_nRetCode = DoSwitchPortAddAck(cVNAMsg);
            }
            
            //vxlan资源释放    
            if((SWITCH_OPER_SUCCESS != cVNAMsg.m_nRetCode) && 
                (EN_ISOLATE_TYPE_VLAN != cVNAMsg.m_cPGInfo.m_nIsolateType))
            {
                CPortMgr *m_pPortMgr = CPortMgr::GetInstance();
                if(NULL != m_pPortMgr)
                {
                    m_pPortMgr->DelVtepPort(cVNAMsg.m_strVnaUuid, cVNAMsg.m_strVtepPort);
                } 
            }
            
            //通配消息应答
            if (!(cVNAMsg.m_strWCTaskUuid.empty()))
            {
                m_pWCMgr->WildcastTaskMsgAck(WILDCAST_TASK_TYPE_SWITCH, cVNAMsg.m_strWCTaskUuid, cVNAMsg.m_nRetCode);
                return 0;
            }
            break;
        }
        case EN_DEL_SWITCH_PORT:
        {
            if((SWITCH_OPER_SUCCESS == cVNAMsg.m_nRetCode))//不考虑进度?
            {
                cVNAMsg.m_nRetCode = DoSwitchPortDelAck(cVNAMsg);
            }  
            break;
        }      
        case EN_ADD_SWITCH:
        default:
        {
            VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::ProcSwitchAckMsg: Msg operation code (%d) is invalid.\n", 
            cVNAMsg.m_nOper);  
            VNetFormatString(cVNAMsg.m_strRetInfo, "Switch operator(%d) is invalid.", cVNAMsg.m_nOper);
            cVNAMsg.m_nRetCode = ERROR_SWITCH_OPERCODE_INVALID;
        }    
    }
    
SWITCH_CFG_ACK:
    cAck.m_strName  = cVNAMsg.m_strName;
    cAck.m_strUuid  = cVNAMsg.m_strUuid;
    cAck.SetResult(cVNAMsg.m_nRetCode);
    cAck.SetResultinfo(cVNAMsg.m_strRetInfo);
    cAck.m_nOper    = cVNAMsg.m_nOper;
    cAck.m_strReqId = cVNAMsg.m_strReqId;
    cAck.m_nProgress= cVNAMsg.m_nProgress; 
    return m_pMU->Send(cAck, option);
}

/*******************************************************************************
* 函数名称： ProcUpdateVNA
* 功能描述： VNA重新注册后，进行交换配置更新
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* ------------------------------------------------------------------------------
* 2013/3          V1.0      zhongchsh      创 建
*******************************************************************************/
int32 CSwitchMgr::ProcUpdateVNA()
{
    string   strVNAUuid = m_pMU->GetTimerStrArg();
    TIMER_ID tTime = m_pMU->GetTimerId(); 

    //检查数据库标记是否更新    
    vector<CSwitchCfgMsgToVNA> vecMsg;
    int32 nRet = m_pSwitch->GetNeedCfgSwitch("", strVNAUuid, vecMsg); 
    if(SWITCH_OPER_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::ProcUpdateVNA: call GetNeedCfgSwitch failed. ret = %d\n", nRet);
        return -1;
    }

    //完成了所有数据的更新，杀死定时器
    if(vecMsg.empty())
    {
        m_pMU->KillTimer(tTime);
        return 0;
    }
    
    //继续下发数据库配置
    vector<CSwitchCfgMsgToVNA>::iterator it = vecMsg.begin();
    for(; it != vecMsg.end(); ++it)
    {
        MID receiver;
        receiver._application = strVNAUuid;
        receiver._process     = PROC_VNA;
        receiver._unit        = MU_VNA_CONTROLLER;
        MessageOption option_vna(receiver, EV_VNETLIB_SWITCH_PORT_CFG, 0, 0); 
        
        it->m_nOper  = EN_ADD_SWITCH_PORT;
        it->m_apisvr = receiver;
        m_pMU->Send((*it), option_vna);
    }    
    return 0;
}

/*******************************************************************************
* 函数名称： ProcUpdateSwitch
* 功能描述： Switch属性发生修改，跟踪变化完成情况
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* ------------------------------------------------------------------------------
* 2013/3          V1.0      zhongchsh      创 建
*******************************************************************************/
int32 CSwitchMgr::ProcUpdateSwitch()
{
    string   strDVSUuid = m_pMU->GetTimerStrArg();
    TIMER_ID tTime = m_pMU->GetTimerId(); 
    
    //检查数据库标记是否更新    
    vector<CSwitchCfgMsgToVNA> vecMsg;
    int32 nRet = m_pSwitch->GetNeedCfgSwitch(strDVSUuid, "", vecMsg); 
    if(SWITCH_OPER_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::ProcUpdateSwitch: call GetNeedCfgSwitch failed. ret = %d\n", nRet);
        return -1;
    }

    //完成了所有数据的更新，杀死定时器
    if(vecMsg.empty())
    {
        m_pMU->KillTimer(tTime);
        return 0;
    }
    
    //继续下发数据库配置
    vector<CSwitchCfgMsgToVNA>::iterator it = vecMsg.begin();
    for(; it != vecMsg.end(); ++it)
    {
        MID receiver;
        receiver._application = it->m_strVnaUuid;
        receiver._process     = PROC_VNA;
        receiver._unit        = MU_VNA_CONTROLLER;
        MessageOption option_vna(receiver, EV_VNETLIB_SWITCH_CFG, 0, 0);    

        it->m_nOper  = EN_MODIFY_SWITCH;
        it->m_apisvr = receiver;
        m_pMU->Send((*it), option_vna);

        if(it->m_cPGInfo.m_nIsolateType)
        {
            CVxlanMgr *pVxlanMgr = CVxlanMgr::GetInstance();
            if (pVxlanMgr)
            {
                pVxlanMgr->SendMcGroupInfo(it->m_strVnaUuid);
            }   
        }
    }    
    return 0;
}

/*******************************************************************************
* 函数名称： GetSwitchInstance
* 功能描述： 获取交换控制模块实例
* 访问的表： 无
* 修改的表： 无
* 输入参数： nSwitchType --交换类型 
* 输出参数： 无
* 返 回 值： CSwitch* --实例化对象指针
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* ------------------------------------------------------------------------------
* 2013/3          V1.0       zhongchsh     创 建
*******************************************************************************/
CSwitch * CSwitchMgr::GetSwitchInstance(const int32 nSwitchType)
{
    CSwitch *pSwitch = NULL;
    switch(nSwitchType)
    {
        case PHYSICAL_SWITCH_TYPE:
        {
            pSwitch = new CPhySwitch();
            break;
        }
        case SOFT_VIR_SWITCH_TYPE:
        case EMB_VIR_SWITCH_TYPE:
        {
            pSwitch = new CVirtualSwitch();
            break;
        }
        default:
        {
            VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::GetSwitchInstance: switch type is %d\n", nSwitchType);
            break;
        }
    }
    return pSwitch;    
}

/*******************************************************************************
* 函数名称： DoSwitchAddReq
* 功能描述： 创建交换，检查和通配是否冲突
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* ------------------------------------------------------------------------------
* 2013/3          V1.0       zhongchsh     创 建
*******************************************************************************/
int32 CSwitchMgr::DoSwitchAddReq(CSwitchCfgMsg &cMsg, string &strRetInfo)
{
    int32 nRet = SWITCH_OPER_SUCCESS;
    int32 nOutResult;
    
    if(DEFAULT_KERNEL_SDVS == cMsg.m_strName)
    {
        VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::DoSwitchAddReq: sdvs_00 cannot add for user.\n");   
        VNetFormatString(strRetInfo, "Dont add switch(%s). that is default.", cMsg.m_strName.c_str());
        return ERROR_SWITCH_OPER_KERNEL_DVS_FAIL;        
    }
    
    //检查是否和通配冲突
    nRet = m_pWCMgr->SwitchNameIsConflict(cMsg.m_strName, nOutResult);
    if((0 != nRet) || (true == nOutResult))
    {
        VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::DoSwitchAddReq: call SwitchNameIsConflict<%s> failed, ret = %d\n", 
                  cMsg.m_strName.c_str(), nRet);
        VNetFormatString(strRetInfo, "Switch name is conflict<%s> .", cMsg.m_strName.c_str());
        return ERROR_SWTICH_CONFLICT_WITH_WILDCAST;
    }
    
    nRet = m_pSwitch->AddSwitch(cMsg, strRetInfo);
    if(SWITCH_OPER_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::DoSwitchAddReq: call <AddSwitch> fail, ret = %d\n", nRet);
    }
    return nRet;
}

/*******************************************************************************
* 函数名称： DoSwitchDelReq
* 功能描述： 删除交换，此时不删除数据库
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* ------------------------------------------------------------------------------
* 2013/3          V1.0       zhongchsh     创 建
*******************************************************************************/
int32 CSwitchMgr::DoSwitchDelReq(CSwitchCfgMsg &cMsg, const MID &mid, string &strRetInfo)
{
    //kernel DVS不能删除
    if(DEFAULT_KERNEL_SDVS == cMsg.m_strName)
    {
        VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::DoSwitchPortAddReq: sdvs_00 cannot delete.\n");   
        VNetFormatString(strRetInfo, "Dont delete default switch(%s).", cMsg.m_strName.c_str());
        return ERROR_SWITCH_OPER_KERNEL_DVS_FAIL;        
    }
    
    //检查是否可以删除
    vector<CSwitchCfgMsgToVNA> vecVNAMsg;
    int32 nRet = m_pSwitch->CanSwitchDel(cMsg.m_strUuid, vecVNAMsg, strRetInfo);
    if(SWITCH_OPER_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::DoSwitchDelReq call <CanSwitchDel> failed. ret = %d\n", nRet);
        return nRet;
    }

    MID receiver;
    if(vecVNAMsg.empty())
    {
        //自发消息，删除数据库记录
        CSwitchCfgMsgToVNA cVNAMsg;
        cVNAMsg.m_apisvr    = mid;
        cVNAMsg.m_strName   = cMsg.m_strName;
        cVNAMsg.m_strUuid   = cMsg.m_strUuid;
        cVNAMsg.m_nOper     = cMsg.m_nOper;
        cVNAMsg.m_nRetCode  = SWITCH_OPER_SUCCESS;
        cVNAMsg.m_nProgress = 100;
        
        receiver._application = ApplicationName();
        receiver._process     = PROC_VNM;
        receiver._unit        = MU_VNM;
        MessageOption option_vnm(receiver, EV_VNETLIB_SWITCH_CFG_ACK, 0, 0);   
        m_pMU->Send(cVNAMsg, option_vnm);        
    }
    else
    {
        //VNA上已经创建交换，发消息给VNA删除
        vector<CSwitchCfgMsgToVNA>::iterator iter = vecVNAMsg.begin();
        for(; iter != vecVNAMsg.end(); ++iter)
        {
            nRet = m_pSwitch->CanSwitchPortDel(*iter);
            if(SWITCH_OPER_SUCCESS != nRet)
            {
                VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::DoSwitchDelReq call <CanSwitchPortDel> failed. ret = %d\n", nRet);
                strRetInfo = iter->m_strRetInfo;
                return nRet;
            }
            
            iter->m_nOper  = cMsg.m_nOper;
            iter->m_apisvr = mid;
            
            //VNA离线，直接删除
            if(iter->m_nIsOffline)
            {
                iter->m_nRetCode  = SWITCH_OPER_SUCCESS;
                iter->m_nProgress = 100;
                
                receiver._application = ApplicationName();
                receiver._process     = PROC_VNM;
                receiver._unit        = MU_VNM;
                MessageOption option_vnm(receiver, EV_VNETLIB_SWITCH_CFG_ACK, 0, 0);   
                m_pMU->Send(*iter, option_vnm); 
            }
            else
            {
                receiver._application = iter->m_strVnaUuid;
                receiver._process     = PROC_VNA;
                receiver._unit        = MU_VNA_CONTROLLER;
                MessageOption option_vna(receiver, EV_VNETLIB_SWITCH_CFG, 0, 0);   
                m_pMU->Send(*iter, option_vna);
            }
        }
    }

    return SWITCH_OPER_SUCCESS;
}

/*******************************************************************************
* 函数名称： DoSwitchDelAck
* 功能描述： 向web应答删除
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* ------------------------------------------------------------------------------
* 2013/3          V1.0       zhongchsh     创 建
*******************************************************************************/
int32 CSwitchMgr::DoSwitchDelAck(CSwitchCfgMsgToVNA &cMsg)
{
    int32 nRet = SWITCH_OPER_SUCCESS;

    //删除某一个VNA上的交换
    if(!cMsg.m_strVnaUuid.empty())
    {
        //vxlan资源释放    
        if(EN_ISOLATE_TYPE_VLAN != cMsg.m_cPGInfo.GetIsolateType())
        {
            CPortMgr *m_pPortMgr = CPortMgr::GetInstance();
            if(NULL != m_pPortMgr)
            {
                nRet = m_pPortMgr->DelVtepPort(cMsg.m_strVnaUuid, cMsg.m_strVtepPort);
                if(SWITCH_OPER_SUCCESS != nRet)
                {
                    VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::DoSwitchDelAck: call <DelVtepPort> <VNA: %s> failed. ret = %d\n", 
                        cMsg.m_strVnaUuid.c_str(), nRet);
                    return ERROR_SWITCH_DEL_VTEP_FAIL;
                }                
            } 
        }
        
        nRet = m_pSwitch->DelSwitchPortMap(cMsg);
        if(SWITCH_OPER_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::DoSwitchDelAck: call <DelSwitchPortMap> <VNA: %s> failed. ret = %d\n", 
                      cMsg.m_strVnaUuid.c_str(), nRet);
            return nRet;
        }        
    }
    
    //是否所有VNA上的DVS都已经删除?
    vector<CSwitchCfgMsgToVNA> vecVNAMsg;
    nRet = m_pSwitch->CanSwitchDel(cMsg.m_strUuid, vecVNAMsg, cMsg.m_strRetInfo);
    if(SWITCH_OPER_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::DoSwitchDelAck: call <CanSwitchDel> failed. ret = %d\n", nRet);
        return nRet;
    }
    
    //删除数据库
    if(vecVNAMsg.empty())
    {
        nRet = m_pSwitch->DelSwitch(cMsg.m_strUuid, cMsg.m_strRetInfo);
        if(SWITCH_OPER_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::DoSwitchDelAck: call <DelSwitch> delete %s failed. ret = %d\n", cMsg.m_strName.c_str(), nRet);
        }
        return nRet;
    }
    
    //继续响应其他VNA ACK
    return ERROR_SWITCH_DEL_NOT_FINISH;
}

/*******************************************************************************
* 函数名称： DoSwitchModifyReq
* 功能描述： 交换常规属性设置
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* ------------------------------------------------------------------------------
* 2013/3          V1.0       zhongchsh     创 建
*******************************************************************************/
int32 CSwitchMgr::DoSwitchModifyReq(CSwitchCfgMsg &cMsg, string &strRetInfo)
{
    int32 nRet = SWITCH_OPER_SUCCESS;

    //先修改数据库
    nRet = m_pSwitch->ModifySwitch(cMsg, strRetInfo);
    if(SWITCH_OPER_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::DoSwitchModifyReq: ModifySwitch <%s> failed. ret = %d\n",
                 cMsg.m_strName.c_str(), nRet);
        return nRet;
    }  
    
    //收集需要更新VNA数据
    vector<CSwitchCfgMsgToVNA> vecVNAMsg;
    nRet = m_pSwitch->GetSwitchDetail(cMsg.m_strUuid, "", vecVNAMsg, strRetInfo); 
    if(0 != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::DoSwitchModifyReq: call GetSwitchDetail <%s> failed.\n", 
                  cMsg.m_strUuid.c_str());
        return ERROR_SWITCH_GET_VNAINFO_FAIL;
    }

    //没有加入端口，直接返回成功
    if(vecVNAMsg.empty())
    {
        return SWITCH_OPER_SUCCESS;
    }

    //下发给VNA处理
    vector<CSwitchCfgMsgToVNA>::iterator it = vecVNAMsg.begin();
    for(; it != vecVNAMsg.end(); ++it)
    {
        MID receiver;
        receiver._application = it->m_strVnaUuid;
        receiver._process     = PROC_VNA;
        receiver._unit        = MU_VNA_CONTROLLER;
        MessageOption option_vna(receiver, EV_VNETLIB_SWITCH_CFG, 0, 0);  
        
        it->m_nOper  = EN_MODIFY_SWITCH;
        it->m_apisvr = receiver;
        m_pMU->Send((*it), option_vna);
    }    
    
    //启动定时器，跟踪VNA配置生效
    TIMER_ID tDVSTimerID = m_pMU->CreateTimer();
    if (INVALID_TIMER_ID == tDVSTimerID)
    {
        VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::DoSwitchModifyReq: call CreateTimer failed.\n");
        VNetFormatString(strRetInfo, "Create timer failed.");
        return ERROR_SWITCH_TIMER_CONFIG_FAIL;
    }

    TimeOut timeout;
    timeout.type     = LOOP_TIMER;
    timeout.duration = (15000);
    timeout.msgid    = EV_SWITCH_MODIFY_TIMER;
    timeout.str_arg  = cMsg.m_strUuid;
    if(SUCCESS != m_pMU->SetTimer(tDVSTimerID, timeout))
    {
        VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::DoSwitchModifyReq: call SetTimer failed.\n");
        m_pMU->KillTimer(tDVSTimerID);
        tDVSTimerID = INVALID_TIMER_ID;
        VNetFormatString(strRetInfo, "Setting timer failed.");
        return ERROR_SWITCH_TIMER_CONFIG_FAIL;
    } 
    
    return SWITCH_OPER_SUCCESS;
}

/*******************************************************************************
* 函数名称： DoSwitchDelAck
* 功能描述： 修改数据库标记，内部处理
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* ------------------------------------------------------------------------------
* 2013/3          V1.0       zhongchsh     创 建
*******************************************************************************/
int32 CSwitchMgr::DoSwitchModifyAck(CSwitchCfgMsgToVNA &cMsg)
{
    int32 nRet = m_pSwitch->ModifySwitch(cMsg);
    if(SWITCH_OPER_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::DoSwitchDelAck: ModifySwitch <%s> failed. ret = %d\n",
                 cMsg.m_strName.c_str(), nRet);
    }
    return nRet;
}

/*******************************************************************************
* 函数名称： DoSwitchPortAddReq
* 功能描述： 交换添加主机端口
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* ------------------------------------------------------------------------------
* 2013/3          V1.0       zhongchsh     创 建
*******************************************************************************/
int32 CSwitchMgr::DoSwitchPortAddReq(CSwitchPortCfgMsg &cMsg, const MID &mid, string &strRetInfo)
{
    if(DEFAULT_KERNEL_SDVS == cMsg.m_strSwName)
    {
        VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::DoSwitchPortAddReq: sdvs_00 cannot add port.\n");   
        VNetFormatString(strRetInfo, "Dont add uplink port to switch(%s).", cMsg.m_strSwName.c_str());
        return ERROR_SWITCH_OPER_KERNEL_DVS_FAIL;        
    }

    //检查是否和通配冲突
    int32 nOutResult;
    int32 nRet = m_pWCMgr->SwitchPortIsConflict(cMsg.m_strVnaUuid, cMsg.m_strSwName, cMsg.m_nSwType, cMsg.m_vecHostPort, nOutResult);
    if((0 != nRet) || (true == nOutResult))
    {
        VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::DoSwitchPortAddReq: SwitchIsConflict failed, ret = %d\n", nRet);
        VNetFormatString(strRetInfo, "The operator is conflict with wildcast's.");
        return ERROR_SWTICH_CONFLICT_WITH_WILDCAST;
    }
    
    //获取交换常规属性 
    CSwitchCfgMsgToVNA cVNAMsg;
    cVNAMsg.m_strUuid = cMsg.m_strSwUuid;
    nRet = m_pSwitch->QuerySwitch(cVNAMsg);
    if(SWITCH_OPER_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::DoSwitchPortAddReq: QuerySwitch failed. ret = %d\n", nRet);
        strRetInfo = cVNAMsg.m_strRetInfo;
        return nRet;
    } 
    
    cVNAMsg.m_strVnaUuid  = cMsg.m_strVnaUuid;
    cVNAMsg.m_nBondMode   = cMsg.m_nBondMode;
    cVNAMsg.m_apisvr      = mid;
    cVNAMsg.m_nOper       = cMsg.m_nOper;
    cVNAMsg.m_strReqId    = cMsg.m_strReqId;
    cVNAMsg.m_nProgress   = 0;

    //获取交换下带已有端口
    nRet = m_pSwitch->QuerySwitchPort(cVNAMsg);
    if(SWITCH_OPER_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::DoSwitchPortAddReq: call<QuerySwitchPort> failed. ret = %d\n", nRet);
        strRetInfo = cVNAMsg.m_strRetInfo;
        return nRet;
    } 
    
    //获取当前待添加端口
    vector<string>::iterator iter = cMsg.m_vecHostPort.begin();
    for(; iter != cMsg.m_vecHostPort.end(); ++iter)
    {
        if(find(cVNAMsg.m_vecPort.begin(), cVNAMsg.m_vecPort.end(), (*iter)) == cVNAMsg.m_vecPort.end())
        {
            cVNAMsg.m_vecPort.push_back(*iter);
        }
    }
    
    //获取PG详细信息
    cVNAMsg.m_cPGInfo.m_strUUID = cVNAMsg.m_strPGUuid;
    if(0 != m_pPGMgr->GetPGDetail(cVNAMsg.m_cPGInfo))
    {   
        VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::DoSwitchPortAddReq: call<GetPGDetail> failed.\n");
        VNetFormatString(strRetInfo, "Get switch portgroup's detail failed.");
        return ERROR_SWTICH_GET_PGINFO_FAIL;
    }     

    //检查端口有效性
    nRet = m_pSwitch->CanSwitchPortAdd(cVNAMsg);
    if(SWITCH_OPER_SUCCESS != nRet)
    {   
        VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::DoSwitchPortAddReq: call<CanSwitchPortAdd> failed.\n");
        strRetInfo = cVNAMsg.m_strRetInfo;
        return nRet;
    }
    
    //Vxlan属性
    if(EN_ISOLATE_TYPE_VLAN != cVNAMsg.m_cPGInfo.GetIsolateType())
    {
        CPortMgr *m_pPortMgr = CPortMgr::GetInstance();
        if(NULL == m_pPortMgr)
        {
            VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::DoSwitchPortAddReq: m_pPortMgr is NULL.\n");
            VNetFormatString(strRetInfo, "Get portmgr instance failed.");
            return ERROR_SWITCH_ADD_VTEP_FAIL;
        }   
        if(0 != m_pPortMgr->GetVtepInfo(cVNAMsg.m_strVnaUuid, cVNAMsg.m_strUuid, cVNAMsg.m_cPGInfo.GetUuid(), 
                  cVNAMsg.m_strVtepPort, cVNAMsg.m_strVtepIp, cVNAMsg.m_strVtepMask))
        {
            VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::DoSwitchPortAddReq: call<GetVtepInfo> failed.\n");
            VNetFormatString(strRetInfo, "Create vtep port failed.");
            return ERROR_SWITCH_ADD_VTEP_FAIL;
        }
    }

    //查询Openflow controller
    nRet = m_pSwitch->QuerySdnCfg(cVNAMsg);
    if(SWITCH_OPER_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::DoSwitchPortAddReq: QuerySdnCfg failed, ret = %d\n", nRet);
        return ERROR_SWITCH_GET_SDN_CFG;
    }

    //发送当前最新数据给VNA
    MID receiver;
    receiver._application = cMsg.m_strVnaUuid;
    receiver._process     = PROC_VNA;
    receiver._unit        = MU_VNA_CONTROLLER;
    MessageOption option_vna(receiver, EV_VNETLIB_SWITCH_PORT_CFG, 0, 0);  
    m_pMU->Send(cVNAMsg, option_vna); 
    
    return SWITCH_OPER_SUCCESS;
}

/*******************************************************************************
* 函数名称： DoSwitchPortAddAck
* 功能描述： 写数据库
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* ------------------------------------------------------------------------------
* 2013/3          V1.0       zhongchsh     创 建
*******************************************************************************/
int32 CSwitchMgr::DoSwitchPortAddAck(CSwitchCfgMsgToVNA &cMsg)
{
    //添加到数据库
    int32 nRet = m_pSwitch->AddSwitchPort(cMsg);
    if(SWITCH_OPER_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::DoSwitchPortAddAck: call <AddSwitchPort> Add DB failed. ret = %d\n", nRet);
    }

    if(cMsg.m_cPGInfo.m_nIsolateType)
    {
        CVxlanMgr *pVxlanMgr = CVxlanMgr::GetInstance();
        if (pVxlanMgr)
        {
            pVxlanMgr->SendMcGroupInfo(cMsg.m_strVnaUuid);
        }   
    }
    
    return nRet;
}

/*******************************************************************************
* 函数名称： DoSwitchPortDelReq
* 功能描述： 删除端口
* 访问的表： 无
* 修改的表： 无
* 输入参数： message --消息结构
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* ------------------------------------------------------------------------------
* 2013/3          V1.0       zhongchsh     创 建
*******************************************************************************/
int32 CSwitchMgr::DoSwitchPortDelReq(CSwitchPortCfgMsg &cMsg, const MID &mid, string &strRetInfo)
{
    if(DEFAULT_KERNEL_SDVS == cMsg.m_strSwName)
    {
        VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::DoSwitchPortDelReq: sdvs_00 cannot delete port.\n");   
        VNetFormatString(strRetInfo, "Dont delete uplink port in switch(%s).", cMsg.m_strSwName.c_str());
        return ERROR_SWITCH_OPER_KERNEL_DVS_FAIL;        
    }
    
    CSwitchCfgMsgToVNA cVNAMsg;
    cVNAMsg.m_strUuid   = cMsg.m_strSwUuid;
    cVNAMsg.m_nOper     = cMsg.m_nOper;
    cVNAMsg.m_strReqId  = cMsg.m_strReqId;
    cVNAMsg.m_apisvr    = mid;
    cVNAMsg.m_strVnaUuid= cMsg.m_strVnaUuid;

    int32 nRet = m_pSwitch->QuerySwitch(cVNAMsg);
    if(SWITCH_OPER_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::DoSwitchPortDelReq: call <QuerySwitch> failed. ret = %d\n", nRet);
        strRetInfo = cVNAMsg.m_strRetInfo;
        return nRet;
    } 
        
    vector<string>::iterator iter = cMsg.m_vecHostPort.begin();
    for(; iter != cMsg.m_vecHostPort.end(); ++iter)
    {
        cVNAMsg.m_vecPort.push_back(*iter);
    }

    //检查端口是否可以删除
    nRet = m_pSwitch->CanSwitchPortDel(cVNAMsg);
    if(SWITCH_OPER_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::DoSwitchPortDelReq: call <CanSwitchPortDel> failed. ret = %d\n", nRet);
        strRetInfo = cVNAMsg.m_strRetInfo;
        return nRet;
    }

    //获取PG详细信息
    cVNAMsg.m_cPGInfo.m_strUUID = cVNAMsg.m_strPGUuid;
    if(0 != m_pPGMgr->GetPGDetail(cVNAMsg.m_cPGInfo))
    {   
        VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::DoSwitchPortDelReq: call<GetPGDetail> failed.\n");
        VNetFormatString(strRetInfo, "Get switch portgroup's detail failed.");
        return ERROR_SWTICH_GET_PGINFO_FAIL;
    }   

    //Vxlan属性
    if(EN_ISOLATE_TYPE_VLAN != cVNAMsg.m_cPGInfo.GetIsolateType())
    {
        CPortMgr *m_pPortMgr = CPortMgr::GetInstance();
        if(NULL == m_pPortMgr)
        {
            VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::DoSwitchPortDelReq: m_pPortMgr is NULL.\n");
            VNetFormatString(strRetInfo, "Get portmgr instance failed.");
            return ERROR_SWITCH_QUERY_VTEP_FAIL;
        }   
        if(0 != m_pPortMgr->GetVtepInfo(cVNAMsg.m_strVnaUuid, cVNAMsg.m_strUuid, cVNAMsg.m_cPGInfo.GetUuid(),
                   cVNAMsg.m_strVtepPort, cVNAMsg.m_strVtepIp, cVNAMsg.m_strVtepMask))
        {
            VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::DoSwitchPortDelReq: call<GetVtepInfo> failed.\n");
            VNetFormatString(strRetInfo, "Query vtep port failed.");
            return ERROR_SWITCH_QUERY_VTEP_FAIL;
        }
    }
    
    MID receiver;
    //VNA离线，不发消息给VNA
    if(cVNAMsg.m_nIsOffline)
    {
        cVNAMsg.m_nRetCode    = SWITCH_OPER_SUCCESS;
        cVNAMsg.m_nProgress   = 100;
        receiver._application = ApplicationName();
        receiver._process     = PROC_VNM;
        receiver._unit        = MU_VNM;
        MessageOption option_vnm(receiver, EV_VNETLIB_SWITCH_PORT_CFG_ACK, 0, 0);    
        m_pMU->Send(cVNAMsg, option_vnm);  
    }
    else 
    {
        receiver._application = cMsg.m_strVnaUuid;
        receiver._process     = PROC_VNA;
        receiver._unit        = MU_VNA_CONTROLLER;
        MessageOption option_vna(receiver, EV_VNETLIB_SWITCH_PORT_CFG, 0, 0);    
        m_pMU->Send(cVNAMsg, option_vna);
    }
    return SWITCH_OPER_SUCCESS;
}

/*******************************************************************************
* 函数名称： DoSwitchPortDelAck
* 功能描述： 写数据库
* 访问的表： 无
* 修改的表： 无
* 输入参数： message --消息结构
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* ------------------------------------------------------------------------------
* 2013/3          V1.0       zhongchsh     创 建
*******************************************************************************/
int32 CSwitchMgr::DoSwitchPortDelAck(CSwitchCfgMsgToVNA &cMsg)
{
    int32 nRet = m_pSwitch->DelSwitchPort(cMsg);
    if(SWITCH_OPER_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::DoSwitchPortAddAck: call <DelSwitchPort> failed. ret = %d\n", nRet);
    }

    return nRet;
}

/*******************************************************************************
* 函数名称： AllocDVSRes
* 功能描述： 申请DVS资源
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* ------------------------------------------------------------------------------
* 2013/3          V1.0       zhongchsh     创 建
*******************************************************************************/
int32 CSwitchMgr::AllocDVSRes(CVNetVmMem &cVnetVmMem)
{    
    int32 nRet = m_pSwitch->GetDVSResource(cVnetVmMem);
    if(SWITCH_OPER_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::AllocDVSRes: call <GetDVSResource> failed. ret = %d\n", nRet);
    }
    return nRet;
}

/*******************************************************************************
* 函数名称： FreeDVSRes
* 功能描述： 释放DVS资源
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* ------------------------------------------------------------------------------
* 2013/3          V1.0       zhongchsh     创 建
*******************************************************************************/
int32 CSwitchMgr::FreeDVSRes(CVNetVmMem &cVnetVmMem)
{
	vector<CVNetVnicMem> vecVnicMem;
	cVnetVmMem.GetVecVnicMem(vecVnicMem);

    vector<CVNetVnicMem>::iterator iter = vecVnicMem.begin();
    for(; iter != vecVnicMem.end(); ++iter)
    {
        iter->SetVSwitchUuid("");
        iter->SetSriovVfPortUuid("");
    }

	cVnetVmMem.SetVecVnicMem(vecVnicMem);

    return 0;
}

/*******************************************************************************
* 函数名称： GetDVSDetail
* 功能描述： 获取DVS信息，供调度使用
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* ------------------------------------------------------------------------------
* 2013/3          V1.0       zhongchsh     创 建
*******************************************************************************/
int32 CSwitchMgr::GetDVSDetail(const string &strVNAUuid, vector<CVNetVnicDetail> &vecVnicDetail)
{
    int32 nRet;
    vector<CVNetVnicDetail>::iterator iter = vecVnicDetail.begin();
    for(; iter != vecVnicDetail.end(); ++iter)
    {
        nRet = m_pSwitch->GetDVSInfo(strVNAUuid, *iter);
        if(SWITCH_OPER_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::GetDVSDetail GetDVSInfo failed. ret = %d\n", nRet);
            return -1;
        } 
    }
    return 0;   
}

/*******************************************************************************
* 函数名称： UpdateVNASwitch
* 功能描述： VNA注册后下发当前配置信息
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* ------------------------------------------------------------------------------
* 2013/3          V1.0       zhongchsh     创 建
*******************************************************************************/
int32 CSwitchMgr::UpdateVNASwitch(const string &strVNAUuid)
{
    vector<CSwitchCfgMsgToVNA> vecMsg;
    string strRetInfo;
    int32 nRet = m_pSwitch->GetSwitchDetail("", strVNAUuid, vecMsg, strRetInfo); 
    if(SWITCH_OPER_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::UpdateVNASwitch: call GetSwitchDetail failed. ret = %d\n", nRet);
        return ERROR_SWITCH_GET_VNAINFO_FAIL;
    }
    
    //下发数据库配置
    vector<CSwitchCfgMsgToVNA>::iterator it = vecMsg.begin();
    for(; it != vecMsg.end(); ++it)
    {
        MID receiver;
        receiver._application = strVNAUuid;
        receiver._process     = PROC_VNA;
        receiver._unit        = MU_VNA_CONTROLLER;
        MessageOption option_vna(receiver, EV_VNETLIB_SWITCH_PORT_CFG, 0, 0);    

        it->m_nOper  = EN_ADD_SWITCH_PORT;
        it->m_apisvr = receiver;
        m_pMU->Send((*it), option_vna);
    }
    
    //启动定时器,继续跟踪下发VNA配置是否生效
    TIMER_ID tDVSTimerID = m_pMU->CreateTimer();
    if (INVALID_TIMER_ID == tDVSTimerID)
    {
        VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::UpdateVNASwitch: call CreateTimer failed.\n");
        return ERROR_SWITCH_TIMER_CONFIG_FAIL;
    }

    TimeOut timeout;
    timeout.type     = LOOP_TIMER;
    timeout.duration = (15000);
    timeout.msgid    = EV_SWITCH_UPDATE_VNA_TIMER;
    timeout.str_arg  = strVNAUuid;
    if(SUCCESS != m_pMU->SetTimer(tDVSTimerID, timeout))
    {
        VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::UpdateVNASwitch: call SetTimer failed.\n");
        m_pMU->KillTimer(tDVSTimerID);
        tDVSTimerID = INVALID_TIMER_ID;
        return ERROR_SWITCH_TIMER_CONFIG_FAIL;
    }    
    
    return SWITCH_OPER_SUCCESS;
}

/*******************************************************************************
* 函数名称： UpdateVNMSwitch
* 功能描述： 处理VNM上报的消息
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* ------------------------------------------------------------------------------
* 2013/3          V1.0       zhongchsh     创 建
*******************************************************************************/
void CSwitchMgr::UpdateVNMSwitch(const string &strVNAUuid, vector<CSwitchReportInfo> &vecReport)
{
    vector<CSwitchReportInfo>::iterator iter = vecReport.begin();
    for(; iter != vecReport.end(); ++iter)
    {
        m_pSwitch->AddReportSwitch(strVNAUuid, *iter);
    }
}

/*******************************************************************************
* 函数名称： UpdateSwitchToOffline
* 功能描述： VNA离线时交换处理
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* ------------------------------------------------------------------------------
* 2013/3          V1.0       zhongchsh     创 建
*******************************************************************************/
int32 CSwitchMgr::UpdateSwitchToOffline(const string &strVNAUuid)
{
    if(NULL == m_pSwitch)
    {
        VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::UpdateSwitchToOffline: m_pSwitch is NULL.\n");
        return -1; 
    }
    
    int32 nRet = m_pSwitch->ModifySwitchPort(strVNAUuid);
    if(SWITCH_OPER_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::UpdateSwitchToOffline: call ModifySwitchPort fail, ret = %d\n", nRet);
    }
    return nRet;
}

/*******************************************************************************
* 函数名称： CheckWildCast
* 功能描述： 检查是否可以进行通配，通配接口
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* ------------------------------------------------------------------------------
* 2013/3          V1.0       zhongchsh     创 建
*******************************************************************************/
int32 CSwitchMgr::CheckWildCast(CSwitchCfgMsgToVNA &cSwitchWCMsg)
{
    int32 nRet = m_pSwitch->CanSwitchAdd(cSwitchWCMsg);
    if(ERROR_SWITCH_ALREADY_EXIST == nRet)
    {
        CSwitchCfgMsgToVNA cVNAMsg;
        cVNAMsg.m_strUuid = cSwitchWCMsg.m_strUuid;
        nRet = m_pSwitch->QuerySwitch(cVNAMsg);
        if(SWITCH_OPER_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::CheckWildCast: QuerySwitch<%s> failed. ret = %d\n", 
                      cSwitchWCMsg.m_strUuid.c_str(), nRet);
            return -1;
        } 
        
        if((cSwitchWCMsg.m_strPGUuid != cVNAMsg.m_strPGUuid) ||
            (cSwitchWCMsg.m_nEvbMode != cVNAMsg.m_nEvbMode) ||
            ( (cSwitchWCMsg.m_nType == SOFT_VIR_SWITCH_TYPE ) && 
              (cSwitchWCMsg.m_nMaxVnic != cVNAMsg.m_nMaxVnic)))
        {
            VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::CheckWildCast: switch's attribute isn't consistent with database's.\n");
            return -1;            
        }
    }
    else if(SWITCH_OPER_SUCCESS != nRet) 
    {
        VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::CheckWildCast: call CanSwitchAdd fail, ret = %d\n", nRet);
        return -1;
    }   
    
    cSwitchWCMsg.m_cPGInfo.GetUuid() = cSwitchWCMsg.m_strPGUuid;
    nRet = m_pPGMgr->GetPGDetail(cSwitchWCMsg.m_cPGInfo);
    if(0 != nRet)
    {   
        VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::CheckWildCast: call GetPGDetail failed. ret = %d\n", nRet);
        return -1;
    }  
    #if 0
    //VXLAN不支持通配。
    if(EN_ISOLATE_TYPE_VLAN != cSwitchWCMsg.m_cPGInfo.GetIsolateType())
    {
        VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::CheckWildCast: Not support vxlan in wildcast mode.\n");
        return -1;
    }
    #endif
    
    string strPort;
    nRet = GetSwitchPort(cSwitchWCMsg.m_strVnaUuid, cSwitchWCMsg.m_strName, strPort);
    if(!strPort.empty())
    {
        VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::CheckWildCast: call GetSwitchPort failed, ret = %d\n", nRet);
        return -1;
    }   
    
    nRet = m_pSwitch->IsSwitchPortValid(cSwitchWCMsg);
    if(0 != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::CheckWildCast: call IsSwitchPortValid failed. ret = %d\n", nRet);   
        return -1;
    }
    
    return 0;
}

int32 CSwitchMgr::GetSwitchUUID(const string &strVNAUuid, const string &strSwName, string &strSwUuid)
{
    return  m_pSwitch->GetDVSUuid(strSwName, strSwUuid);
}

int32 CSwitchMgr::GetSwitchName(const string &strSwUuid, string &strSwName)
{
    return m_pSwitch->GetDVSName(strSwUuid, strSwName);  
}

int32 CSwitchMgr::GetSwitchPort(const string &strVNAUuid, const string &strSwName, string &strPortName)
{
    return m_pSwitch->GetDVSUplinkPort(strVNAUuid, strSwName, strPortName);  
}

int32 CSwitchMgr::GetSwitchOnlineVNA(const string &strDVSUuid, set<string> &setVNAUuid)
{
    int32 nRet;
    vector<string> vecVNAInfo;
    
    nRet = m_pSwitch->GetDVSVNA(strDVSUuid, vecVNAInfo);
    if(0 != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::GetSwitchVNA: call GetDVSVNA failed, ret = %d\n", nRet);
        return -1;
    }
    
    vector<string>::iterator iter = vecVNAInfo.begin();
    for(; iter != vecVNAInfo.end(); ++iter)
    {
        setVNAUuid.insert(*iter);
    }
    return 0;    
}

int32 CSwitchMgr::GetSwitchPGInfo(const string &strDVSName, CPortGroup &cPGInfo)
{
    if(strDVSName.empty())
    {
        VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::GetSwitchPGInfo: DVS name is empty. \n");
        return -1;
    }
    
    string strDVSUuid;
    int32 nRet = m_pSwitch->GetDVSUuid(strDVSName, strDVSUuid);
    if(0 != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::GetSwitchPGInfo: call GetDVSUuid(%s) failed.\n", strDVSName.c_str());
        return -1;
    }
    
    string strPGUuid;
    nRet = m_pSwitch->GetDVSPGUuid(strDVSUuid, strPGUuid);
    if(SWITCH_OPER_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::GetSwitchPGInfo: call GetDVSPGUuid failed, ret = %d\n", nRet);
        return -1;
    }
    
    cPGInfo.SetUuid(strPGUuid);
    nRet = m_pPGMgr->GetPGDetail(cPGInfo);
    if (0 != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::GetSwitchPGInfo: call GetPGDetail(%s) failed.\n", strPGUuid.c_str());
        return -1;
    }
    
    return 0;
}

/*******************************************************************************
* 函数名称： ProcSwitchWildcastCfg
* 功能描述： 通配处理接口，完成交换创建和端口添加
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* ------------------------------------------------------------------------------
* 2013/3          V1.0       zhongchsh     创 建
*******************************************************************************/
int32 CSwitchMgr::DoSwitchWildcast(CSwitchCfgMsgToVNA &cSwitchWCMsg)
{
    //先创建交换，写数据库
    CSwitchCfgMsg cDVSCfgMsg;
    cDVSCfgMsg.m_strName    = cSwitchWCMsg.m_strName;
    cDVSCfgMsg.m_nType      = cSwitchWCMsg.m_nType;
    cDVSCfgMsg.m_nMTU       = cSwitchWCMsg.m_nMTU;
    cDVSCfgMsg.m_nEvbMode   = cSwitchWCMsg.m_nEvbMode;
    cDVSCfgMsg.m_nMaxVnic   = cSwitchWCMsg.m_nMaxVnic;
    cDVSCfgMsg.m_strPGUuid  = cSwitchWCMsg.m_strPGUuid;
    cDVSCfgMsg.m_nOper      = EN_ADD_SWITCH;
    
    int32 nRet = m_pSwitch->AddSwitch(cDVSCfgMsg, cSwitchWCMsg.m_strRetInfo);
    if((SWITCH_OPER_SUCCESS != nRet) && (ERROR_SWITCH_ALREADY_EXIST != nRet))
    {
        VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::DoSwitchWildcast: call AddSwitch fail, ret = %d\n", nRet);
        return -1;
    }    
    
    //填充消息体，发送给VNA处理    
    cSwitchWCMsg.m_strUuid = cDVSCfgMsg.m_strUuid;
    cSwitchWCMsg.m_nOper   = EN_ADD_SWITCH_PORT;
    cSwitchWCMsg.m_cPGInfo.m_strUUID = cSwitchWCMsg.m_strPGUuid;
    nRet = m_pPGMgr->GetPGDetail(cSwitchWCMsg.m_cPGInfo);
    if(0 != nRet)
    {   
        VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::DoSwitchWildcast: call GetPGDetail failed. ret = %d\n", nRet);
        return -1;
    }      

    //Vxlan属性
    if(EN_ISOLATE_TYPE_VLAN != cSwitchWCMsg.m_cPGInfo.GetIsolateType())
    {
        CPortMgr *pPortMgr = CPortMgr::GetInstance();
        if(NULL == pPortMgr)
        {
            VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::DoSwitchWildcast: pPortMgr is NULL.\n");
            return -1;
        }   
        if(0 != pPortMgr->GetVtepInfo(cSwitchWCMsg.m_strVnaUuid, cSwitchWCMsg.m_strUuid, cSwitchWCMsg.m_cPGInfo.GetUuid(), 
            cSwitchWCMsg.m_strVtepPort, cSwitchWCMsg.m_strVtepIp, cSwitchWCMsg.m_strVtepMask))
        {
            VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::DoSwitchWildcast: call<GetVtepInfo> failed.\n");
            return -1;
        }
    }    

    MID receiver;
    receiver._application = cSwitchWCMsg.m_strVnaUuid;
    receiver._process     = PROC_VNA;
    receiver._unit        = MU_VNA_CONTROLLER;
    MessageOption option_vna(receiver, EV_VNETLIB_SWITCH_PORT_CFG, 0, 0);  
    m_pMU->Send(cSwitchWCMsg, option_vna);  
    return 0;
}

int32 CSwitchMgr::DoKernelDVSCfg(const string &strVNAUuid, const string &strDVSName, 
         const vector<string> &vecPortName, const string &strBondName, const int32 &nBondMode)
{ 
    //入参检查。
    if(strDVSName.empty() || vecPortName.empty())
    {
        VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::DoKernelDVSCfg: (strDVSName.empty() || vecPortName.empty())\n");
        return ERROR_SWITCH_OPER_KERNEL_DVS_FAIL;
    }

    if (SWITCH_OPER_SUCCESS!=m_pSwitch->AddKernelDVS(strVNAUuid, strDVSName, vecPortName, strBondName, nBondMode))
    {
        VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::DoKernelDVSCfg: m_pSwitch->AddKernelDVS fail\n");
        return ERROR_SWITCH_OPER_KERNEL_DVS_FAIL;
    }

    /* 获取PG信息 */
    CSwitchCfgMsgToVNA cVNAMsg;
    int32 nRet = GetSwitchPGInfo(strDVSName, cVNAMsg.m_cPGInfo);
    if(0 != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::DoKernelDVSCfg: GetSwitchPGInfo failed, switch name %s, ret = %d\n", 
            strDVSName.c_str(), nRet);
        return ERROR_SWITCH_OPER_DB_FAIL;
    }
  
    //Vxlan属性
    if(EN_ISOLATE_TYPE_VLAN != cVNAMsg.m_cPGInfo.GetIsolateType())
    {
        string strDVSUuid;
        nRet = m_pSwitch->GetDVSUuid(strDVSName, strDVSUuid);    
        if(nRet != 0)
        {   
            VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::DoKernelDVSCfg: m_pSwitch->GetDVSUuid fail\n");
            return ERROR_SWITCH_OPER_KERNEL_DVS_FAIL;
        }    
        
        CPortMgr *m_pPortMgr = CPortMgr::GetInstance();
        if(NULL == m_pPortMgr)
        {
            VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::DoKernelDVSCfg: m_pPortMgr is NULL.\n");
            return ERROR_SWITCH_ADD_VTEP_FAIL;
        }   
        if(0 != m_pPortMgr->GetVtepInfo(strVNAUuid, strDVSUuid, cVNAMsg.m_cPGInfo.GetUuid(), 
            cVNAMsg.m_strVtepPort, cVNAMsg.m_strVtepIp, cVNAMsg.m_strVtepMask))
        {
            VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::DoKernelDVSCfg: call<GetVtepInfo> failed.\n");
            return ERROR_SWITCH_ADD_VTEP_FAIL;
        }
    }

    return SWITCH_OPER_SUCCESS;
    
}

void CSwitchMgr::DbgShowSwitch()
{
    m_pSwitch->DbgShow();
}

void VNetTestSwitchOper(const char* cstrName, 
                             const char* cstrUUID, 
                             const char* cstrPGuuid,
                             int32 nType, 
                             int32 nMTU,
                             int32 nEvb,
                             int32 nMaxvnic,
                             int32 nOper)
{
    if(NULL == cstrName || 
       NULL == cstrUUID)
    {
        return ;
    }

    if((nOper <= EN_SWITCH_OPER_BEGIN) || (nOper >= EN_SWITCH_OPER_END))
    {
        cout << "Operation Code: Add[" << EN_ADD_SWITCH
             << "], Del[" << EN_DEL_SWITCH
             << "], Mod[" << EN_MODIFY_SWITCH
             << "]." <<endl;
        return ;
    }

    MessageUnit tempmu(TempUnitName("TestSwitchOper"));
    tempmu.Register();
    CSwitchCfgMsg cMsg;
    string strTmp;
    strTmp.assign(cstrName);
    cMsg.m_strName = strTmp;
    strTmp.clear();
    strTmp.assign(cstrUUID);
    cMsg.m_strUuid = strTmp;
    strTmp.clear();
    cMsg.m_nType = nType;
    cMsg.m_nMTU = nMTU;
    cMsg.m_nEvbMode = nEvb;
    cMsg.m_nMaxVnic = nMaxvnic;
    cMsg.m_strPGUuid = cstrPGuuid;
    cMsg.m_nOper = nOper;
    MessageOption option(MU_VNM, EV_VNETLIB_SWITCH_CFG, 0, 0);
    tempmu.Send(cMsg,option);
    return ;
}

void VNetTestSwitchPortOper(const char* cstrSwName,
                             const char* cstrSwUUID, 
                             const char* cstrVNAUUID,
                             const char* cstrPortName,
                             int32 nSwType, 
                             int32 nBondmode,
                             int32 nOper)
{
    if(NULL == cstrSwName || NULL == cstrSwUUID)
    {
        return ;
    }

    if((nOper < EN_ADD_SWITCH_PORT) || (nOper >= EN_SWITCH_OPER_END))
    {
        cout << "Operation Code: Add[" << EN_ADD_SWITCH_PORT
             << "], Del[" << EN_DEL_SWITCH_PORT
             << "]." <<endl;
        return ;
    }

    MessageUnit tempmu(TempUnitName("TestSwitchPortOper"));
    tempmu.Register();
    CSwitchPortCfgMsg cMsg;
    string strTmp;
    strTmp.assign(cstrSwName);
    cMsg.m_strSwName = strTmp;
    strTmp.clear();
    strTmp.assign(cstrSwUUID);
    cMsg.m_strSwUuid = strTmp;
    strTmp.clear();
    strTmp.assign(cstrVNAUUID);
    cMsg.m_strVnaUuid = strTmp;
    strTmp.clear();
    strTmp.assign(cstrPortName);
    cMsg.m_vecHostPort.push_back(strTmp);
    cMsg.m_nSwType   = nSwType;
    cMsg.m_nBondMode = nBondmode;
    cMsg.m_nOper     = nOper;
    MessageOption option(MU_VNM, EV_VNETLIB_SWITCH_PORT_CFG, 0, 0);
    tempmu.Send(cMsg,option);
    return ;
}

void DbgSetSwitchPrint(BOOL bOpen)
{
    CSwitchMgr *pMgr = CSwitchMgr::GetInstance();
    if(pMgr)
    {
        pMgr->SetDbgPrintFlag(bOpen);
    }
}

void DbgShowSwitch()
{
    CSwitchMgr *pMgr = CSwitchMgr::GetInstance();
    if(NULL != pMgr)
    {
        pMgr->DbgShowSwitch();
    }
}

DEFINE_DEBUG_FUNC(VNetTestSwitchOper);
DEFINE_DEBUG_FUNC(VNetTestSwitchPortOper);
DEFINE_DEBUG_FUNC(DbgSetSwitchPrint);
DEFINE_DEBUG_FUNC(DbgShowSwitch);
}

