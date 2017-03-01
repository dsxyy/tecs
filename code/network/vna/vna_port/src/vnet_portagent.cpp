/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：vnet_portagent.cpp
* 文件标识：
* 内容摘要：CPortAgent类实现文件
* 当前版本：1.0
* 作    者：liuhx
* 完成日期：2013年2月25日
******************************************************************************/

#include "vnet_comm.h"
#include "vna_common.h"
#include "vnetlib_msg.h"
#include "vnetlib_event.h"
#include "vnet_error.h"
#include "vnet_portagent.h"
#include "dev_monitor.h"
#include "dev_mgr.h"
#include "vnet_function.h"
#include "vnet_trunkport_manager.h"
#include "vnet_phyport_manager.h"
#include "vnet_uplinkloopport_manager.h"
#include <fstream>

namespace zte_tecs
{
CPortAgent* CPortAgent::s_pInstance = NULL;

CPortAgent::CPortAgent()
{
    m_pMU = NULL;
    m_bOpenDbgInf = 0;
};

CPortAgent::~CPortAgent()
{
    delete m_pMU;
    delete s_pInstance;
    m_bOpenDbgInf = 0;
};

STATUS CPortAgent::Init(MessageUnit *mu)
{
    if (NULL == mu)
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortAgent::Init: mu is NULL.\n");
        //VNET_ASSERT(0);
        return ERROR;
    }

    m_pMU = mu;

    return SUCCESS;
}

//设置打印开关
void CPortAgent::SetDbgPrint(int32 nFlag)
{
    m_bOpenDbgInf = nFlag;
}

void CPortAgent::MessageEntry(const MessageFrame& message)
{
    switch (message.option.id())
    {
        case EV_VNETLIB_KERNALPORT_CFG:
        {
            ProcKernelPortCfg(message);
            break;
        }
        case EV_VNETLIB_CFG_LOOPBACK:
        {
            ProcLoopBackPortCfg(message);
            break;
        }
        case TIMER_VNA_RESET_PORTINFO:
        {
            ProcPortCfg(message);			
            break;
        }
        default:
        {
            VNET_LOG(VNET_LOG_INFO, "CPortAgnet rec unknown message: %d.\n", message.option.id());
            break;
        }
    }
}

//处理下发的端口配置
STATUS CPortAgent::ProcPortCfg(const MessageFrame& message)
{
    int32 nRet = SUCCESS;
    string strBridge = "";
    uint32 nVlan = 0;
    //uint32 nMtu = 0;

    CPortCfgMsgToVNA cMsg;
    cMsg.deserialize(message.data);

    if (m_bOpenDbgInf)
    {
        cMsg.Print();
    }

    MessageOption option(message.option.sender(), TIMER_VNA_RESET_PORTINFO_ACK, 0, 0);

    //入参检查
    if (cMsg.m_strName.empty())
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortAgent::ProcPortCfg port name is empty.\n");
        nRet = ERROR_PORT_NAME_EMPTY;
        goto PORT_CFG_ACK;
    }

    switch (cMsg.m_nType)
    {
    case PORT_TYPE_KERNEL:
    {
        if((cMsg.m_strSwitchName.empty()) || (cMsg.m_strUplinkPort.empty()))
        {
            nRet = ERROR;
            break;
        }

        //检查交换是否存在
        vector<string> vecBridge;
        VNetGetAllBridges(vecBridge);
		
        vector<string>::iterator iter;
        iter = find(vecBridge.begin(), vecBridge.end(), cMsg.m_strSwitchName);
        if(iter == vecBridge.end())
        {
            nRet = ERROR;
            break;
        }

        //交换已经存在，如果该kernel已经绑定到交换上返回
        if (0 == VNetGetBridgeOfIf(cMsg.m_strName, strBridge))
        {
            if (strBridge == cMsg.m_strSwitchName)
            {
                //继续判断vlan和mtu，增加以上判断为了解决同步操作时间过长问题
                //如果已经存在，vna上不再做kernel操作
                if (/*(0 == VNetGetMtu(cMsg.m_strName.c_str(), nMtu)) &&*/ (0 == VNetGetVlanAccess(cMsg.m_strName, nVlan)))
                {
                    if (/*(nMtu == cMsg.m_nMtu) &&*/ (nVlan == cMsg.m_nVlan))
                    {
                        break;
                    }
                }
            }
        }

        if (0 != VNetCreateKernelPort(cMsg.m_strName, cMsg.m_strSwitchName, cMsg.m_strUplinkPort, cMsg.m_strIP, cMsg.m_strMask, cMsg.m_nIsDhcp, cMsg.m_nVlan, cMsg.m_nMtu))
        {
            nRet = ERROR;
        }
        else
        {
            /*if(0 != VNetSetVlan(cMsg.m_strName, cMsg.m_nVlan))
            {
                 VNET_LOG(VNET_LOG_ERROR, "CPortAgent::ProcPortCfg:[%s  %d] VNetSetVlan failed.\n", cMsg.m_strName.c_str(), cMsg.m_nVlan);
                 nRet = ERROR;
            }*/

            /*if(0 != VNetSetMtu(cMsg.m_strName, cMsg.m_nMtu))
            {
                 VNET_LOG(VNET_LOG_ERROR, "CPortAgent::ProcPortCfg:[%s  %d] VNetSetMtu failed.\n", cMsg.m_strName.c_str(), cMsg.m_nMtu);
                 nRet = ERROR;
            }*/
        }		
        break;
    }
    case PORT_TYPE_UPLINKLOOP:
    {
        if (0 != VNetCreateLoopBackPort(cMsg.m_strName))
        {
            nRet = ERROR;
        }
        break;
    }
    case PORT_TYPE_PHYSICAL:
    {
        if (0 != VNetSetPorLoopback(cMsg.m_strName, 1))
        {
            nRet = ERROR;
        }
        break;
    }
    default:
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortAgent::ProcPortCfg: Msg type (%d) is invalid.\n", cMsg.m_nType);
        nRet = ERROR_PORT_OPERCODE_INVALID;
    }
    }

    if (m_bOpenDbgInf)
    {
        cout << "port cfg operation result:" << nRet << endl;
    }

PORT_CFG_ACK:
    cMsg.m_nRetCode = nRet;
    return m_pMU->Send(cMsg, option);
}

//处理kernel口配置
STATUS CPortAgent::ProcKernelPortCfg(const MessageFrame& message)
{
    string strBridge = "";
    uint32 nVlan = 0;
    //uint32 nMtu = 0;

    int32 nRet = SUCCESS;
    CPortCfgMsgToVNA cMsg;
    cMsg.deserialize(message.data);

    if (m_bOpenDbgInf)
    {
        cMsg.Print();
    }

    MessageOption option(message.option.sender(), EV_VNETLIB_KERNALPORT_CFG_ACK, 0, 0);

    //入参检查
    if(cMsg.m_strName.empty())
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortAgent::ProcKernelPortCfg port name is empty.\n");
        nRet = ERROR_PORT_NAME_EMPTY;
        goto PORT_CFG_ACK;
    }
    
    switch (cMsg.m_nOper)
    {
    case EN_ADD_KERNALPORT:
    {

        if((cMsg.m_strSwitchName.empty()) || (cMsg.m_strUplinkPort.empty()))
        {
            nRet = ERROR;
            break;
        }

        //检查交换是否存在
        vector<string> vecBridge;
        VNetGetAllBridges(vecBridge);
		
        vector<string>::iterator iter;
        iter = find(vecBridge.begin(), vecBridge.end(), cMsg.m_strSwitchName);
        if(iter == vecBridge.end())
        {
            nRet = ERROR;
            break;
        }

        //交换已经存在，如果该kernel已经绑定到交换上返回
        if (0 == VNetGetBridgeOfIf(cMsg.m_strName, strBridge))
        {
            if (strBridge == cMsg.m_strSwitchName)
            {
                //继续判断vlan和mtu，增加以上判断为了解决同步操作时间过长问题
                //如果已经存在，vna上不再做kernel操作
                if (/*(0 == VNetGetMtu(cMsg.m_strName.c_str(), nMtu)) &&*/ (0 == VNetGetVlanAccess(cMsg.m_strName, nVlan)))
                {
                    if (/*(nMtu == cMsg.m_nMtu) &&*/ (nVlan == cMsg.m_nVlan))
                    {
                        break;
                    }
                }
            }
        }


        if (0 != VNetCreateKernelPort(cMsg.m_strName, cMsg.m_strSwitchName, cMsg.m_strUplinkPort, cMsg.m_strIP, cMsg.m_strMask, cMsg.m_nIsDhcp, cMsg.m_nVlan, cMsg.m_nMtu))
        {
            nRet = ERROR;
        }
        else
        {
            /*if(0 != VNetSetVlan(cMsg.m_strName, cMsg.m_nVlan))
            {
                 VNET_LOG(VNET_LOG_ERROR, "CPortAgent::ProcKernelPortCfg:[%s  %d] VNetSetVlan failed.\n", cMsg.m_strName.c_str(), cMsg.m_nVlan);
                 nRet = ERROR;
            }*/

            /*if(0 != VNetSetMtu(cMsg.m_strName, cMsg.m_nMtu))
            {
                 VNET_LOG(VNET_LOG_ERROR, "CPortAgent::ProcKernelPortCfg:[%s  %d] VNetSetMtu failed.\n", cMsg.m_strName.c_str(), cMsg.m_nMtu);
                 nRet = ERROR;
            }*/
        }
        break;
    }
    case EN_DEL_KERNALPORT:
    {
        if (0 != VNetDeleteKernelPort(cMsg.m_strSwitchName, cMsg.m_strName))
        {
            nRet = ERROR;
        }
        break;
    }
    case EN_MOD_KERNALPORT:
    {
        if (0 != VNetModifyKernelPort(cMsg.m_strName, cMsg.m_strSwitchName, cMsg.m_strUplinkPort, cMsg.m_strIP, cMsg.m_strMask))
        {
            nRet = ERROR;
        }
        break;
    }
    default:
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortAgent::ProcKernelPortCfg:Msg operation code (%d) is invalid.\n", cMsg.m_nOper);
        nRet = ERROR_PORT_OPERCODE_INVALID;
    }
    }

    if (m_bOpenDbgInf)
    {
        cout << "kernel cfg operation result:" << nRet << endl;
    }

PORT_CFG_ACK:
    cMsg.m_nRetCode = nRet;
    return m_pMU->Send(cMsg, option);
}

//处理loopback口配置
STATUS CPortAgent::ProcLoopBackPortCfg(const MessageFrame& message)
{
    int32 nRet = SUCCESS;
    CPortCfgMsgToVNA cMsg;
    cMsg.deserialize(message.data);

    if (m_bOpenDbgInf)
    {
        cMsg.Print();
    }

    MessageOption option(message.option.sender(), EV_VNETLIB_CFG_LOOPBACK_ACK, 0, 0);

    //入参检查
    if(cMsg.m_strName.empty())
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortAgent::ProcLoopBackPortCfg port name is empty.\n");
        nRet = ERROR_PORT_NAME_EMPTY;
        goto PORT_CFG_ACK;
    }
    
    switch(cMsg.m_nOper)
    {
        case EN_ADD_LOOPPORT:
        {
            if(0 != VNetCreateLoopBackPort(cMsg.m_strName))
            {
                nRet = ERROR;
            }
            break;
        }
        case EN_DEL_LOOPPORT:
        {
            if(0 != VNetDeleteLoopBackPort(cMsg.m_strName))
            {
                nRet = ERROR;
            }
            break;
        }		
        case EN_SET_LOOPPORT:
        {
            if(0 != VNetSetPorLoopback(cMsg.m_strName, 1))
            {
                nRet = ERROR;
            }
            break;
        }
        case EN_UNSET_LOOPPORT:
        {
            if(0 != VNetSetPorLoopback(cMsg.m_strName, 0))
            {
                nRet = ERROR;
            }
            break;
        }		
        default:
        {
            VNET_LOG(VNET_LOG_ERROR, "CPortAgent::ProcLoopBackPortCfg: Msg operation code (%d) is invalid.\n", cMsg.m_nOper);
            nRet = ERROR_PORT_OPERCODE_INVALID;
        }
    }

    if (m_bOpenDbgInf)
    {
        cout << "loopback cfg operation result:" << nRet << endl;
    }

PORT_CFG_ACK:
    cMsg.m_nRetCode = nRet;
    return m_pMU->Send(cMsg, option);
}


//组织一下获取端口上报信息
int32  CPortAgent::GetPortReportInfo(CMessageVNAPortInfoReport &PortInfo)
{
    //获取vna uuid
    if (GetVNAUUIDByVNA(PortInfo._vna_id))
    {
        VNET_LOG(VNET_LOG_ERROR, "GetPortReportInfo Get VnaUUID failed \n");
        return -1;
    }

    CPhyPortManager *pPhyHandle = CPhyPortManager::GetInstance();
    if (pPhyHandle)
    {
        if (!(pPhyHandle->GetPhyPortInfo(PortInfo._phy_port)))
        {
            //continue
        }
        else
        {
            VNET_LOG(VNET_LOG_ERROR, "GetPortReportInfo PhyPort report failed \n");
            return -1;
        }
    }

    CTrunkPortManager *pTrunkHandle = CTrunkPortManager::GetInstance();
    if (pTrunkHandle)
    {
        if (!(pTrunkHandle->GetTrunkInfo(PortInfo._bond_info)))
        {
            //continue
        }
        else
        {
            VNET_LOG(VNET_LOG_ERROR, "GetPortReportInfo TrunkPort report failed \n");
            return -1;
        }
    }

    CUplinkLoopPortManager *pUplinkLoopHandle = CUplinkLoopPortManager::GetInstance();
    if (pUplinkLoopHandle)
    {
        if (!(pUplinkLoopHandle->GetUplinkLoopPortInfo(PortInfo._uplink_info)))
        {
            //continue
        }
        else
        {
            VNET_LOG(VNET_LOG_ERROR, "GetPortReportInfo UplinkLoopPort report failed \n");
            return -1;
        }
    }

    CPhyPortManager *pKernelHandle = CPhyPortManager::GetInstance();
    if (pKernelHandle)
    {
        if (!(pKernelHandle->GetKernelPortInfo(PortInfo._kernel_port)))
        {
            //continue
        }
        else
        {
            VNET_LOG(VNET_LOG_ERROR, "GetPortReportInfo KernelPort report failed \n");
            return -1;
        }
    }

    CPhyPortManager *pKernelCfgHandle = CPhyPortManager::GetInstance();
    if (pKernelCfgHandle)
    {
        if (!(pKernelCfgHandle->GetKernelPortCfgInfo(PortInfo._kernel_cfgport)))
        {
            //continue
        }
        else
        {
            VNET_LOG(VNET_LOG_ERROR, "GetPortReportInfo KernelCfgPort report failed \n");
            return -1;
        }
    }

    CPhyPortManager *pVtepCfgHandle = CPhyPortManager::GetInstance();
    if (pVtepCfgHandle)
    {
        if (!(pVtepCfgHandle->GetVtepPortCfgInfo(PortInfo._vtep_cfgport)))
        {
            //continue
        }
        else
        {
            VNET_LOG(VNET_LOG_ERROR, "GetPortReportInfo VtepCfgPort report failed \n");
            return -1;
        }
    }

    return 0;
}

//提供给evb的端口信息
int32  CPortAgent::GetEvbPortInfo(vector<CEvbPortInfo> &vPortInfo)
{
    map<string, int32> mapTmp;
    CEvbPortInfo info;

    vPortInfo.clear();
    mapTmp.clear();

    //调用接口之前能否确定已经实例化?
    CPhyPortManager *pPhyHandle = CPhyPortManager::GetInstance();
    if (pPhyHandle)
    {
        if (0 == pPhyHandle->GetPortMapInfo(mapTmp))
        {
            map<string, int32>::iterator it = mapTmp.begin();
            for (; it!= mapTmp.end(); ++it)
            {
                info.m_strName = it->first;
                info.m_nStatus = it->second;
                info.m_nType = PORT_TYPE_PHYSICAL;
                vPortInfo.push_back(info);
            }
        }
    }

    mapTmp.clear();

    CTrunkPortManager *pTrunkHandle = CTrunkPortManager::GetInstance();
    if (pTrunkHandle)
    {
        if (0 == pTrunkHandle->GetPortMapInfo(mapTmp))
        {
            map<string, int32>::iterator it = mapTmp.begin();
            for (; it!= mapTmp.end(); ++it)
            {
                info.m_strName = it->first;
                info.m_nStatus = it->second;
                info.m_nType = PORT_TYPE_BOND;
                vPortInfo.push_back(info);
            }
        }
    }
	
    return 0;
}
}

int32 VnetDbgShowVnaPortInfo(void)
{
    CPortAgent *pHandle = CPortAgent::GetInstance();
    if (pHandle)
    {
        cout << "++++++++++++++++  VNA Port Info  ++++++++++++++++++++++" << endl;
        //统计暂且到各个类型端口中查看
        cout << "+++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
    }
    return 0;
}
DEFINE_DEBUG_FUNC(VnetDbgShowVnaPortInfo);

void SetPortAgentPrint(int32 nFlag)
{
    CPortAgent *pHandle = CPortAgent::GetInstance();
    if (pHandle)
    {
        pHandle->SetDbgPrint(nFlag);
    }
    return;
}
DEFINE_DEBUG_FUNC(SetPortAgentPrint);

