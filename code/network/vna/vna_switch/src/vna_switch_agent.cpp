/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：vna_switch_agent.cpp
* 文件标识：
* 内容摘要：vna上交换代理模块
* 当前版本：1.0
* 作    者： 
* 完成日期： 
******************************************************************************/
#include "vna_common.h"
#include "dev_monitor.h"
#include "dev_mgr.h"
#include "vna_switch_module.h"
#include "vna_switch_agent.h"
#include "vna_vxlan_agent.h"
#include "vnet_phyport_manager.h"

namespace zte_tecs
{

CSwitchAgent *CSwitchAgent::m_pInstance = NULL;

CSwitchAgent::CSwitchAgent()
{
    SetMgrInfo("CSwitchAgent");
    m_bOpenDbg = VNET_FALSE;
}

CSwitchAgent::~CSwitchAgent()
{ 
    m_pMU       = NULL;
    m_pInstance = NULL; 
    m_bOpenDbg  = VNET_FALSE;
};

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
int32 CSwitchAgent::Init(MessageUnit *mu)
{
    if (mu == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CSwitchAgent::Init: m_pMu is NULL.\n");
        return -1;
    }
    
    m_pMU = mu;

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
void CSwitchAgent::MessageEntry(const MessageFrame& message)
{
    switch(message.option.id())
    {
        case EV_VNETLIB_SWITCH_CFG:
        {
            ProcSwitchCfg(message);
            break;
        }
        
        case EV_VNETLIB_SWITCH_PORT_CFG:
        {
            ProcSwitchPortCfg(message);
            break;
        }
        
        default:
            break;
    }
}

/*******************************************************************************
* 函数名称： ProcSwitchCfg
* 功能描述： 交换配置处理，添加、删除、修改交换
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* ------------------------------------------------------------------------------
* 2013/3          V1.0       钟春山        创 建
*******************************************************************************/
int32 CSwitchAgent::ProcSwitchCfg(const MessageFrame& message)
{
    int32 nRet = SWITCH_OPER_SUCCESS;
    CSwitchCfgMsgToVNA cMsg;
    cMsg.deserialize(message.data);

    MessageOption option(message.option.sender(), EV_VNETLIB_SWITCH_CFG_ACK, 0, 0);

    //入参检查
    if(cMsg.m_strName.empty() || cMsg.m_strUuid.empty())
    {
        VNET_LOG(VNET_LOG_ERROR, "CSwitchAgent::ProcSwitchCfg Switch name/uuid is empty.\n");
        nRet = ERROR_SWITCH_NAME_INVALID;
        goto SWITCH_CFG_ACK;
    }
    
    if(m_bOpenDbg)
    {
        cMsg.Print();
    }
    
    switch(cMsg.m_nOper)
    {
        case EN_DEL_SWITCH:
        {
            nRet = DoSwitchDel(cMsg);
            break;
        }
        case EN_MODIFY_SWITCH:
        {
            nRet = DoSwitchModify(cMsg);
            break;
        }
        case EN_ADD_SWITCH:
        default:
        {
            VNET_LOG(VNET_LOG_ERROR, "CSwitchAgent::ProcSwitchCfg: Msg operation code (%d) is invalid.\n", cMsg.m_nOper);  
            nRet = ERROR_SWITCH_OPERCODE_INVALID;
            VNetFormatString(cMsg.m_strRetInfo, "Switch operator(%d) is invalid.", cMsg.m_nOper);
        }
    }

SWITCH_CFG_ACK:
    cMsg.m_nRetCode = nRet;
    return m_pMU->Send(cMsg, option);
}

/*******************************************************************************
* 函数名称： ProcSwitchPortCfg
* 功能描述： 交换端口配置处理，添加、删除主机端口
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* ------------------------------------------------------------------------------
* 2013/3          V1.0       钟春山        创 建
*******************************************************************************/
int32 CSwitchAgent::ProcSwitchPortCfg(const MessageFrame& message)
{
    int32 nRet = SWITCH_OPER_SUCCESS;
    CSwitchCfgMsgToVNA cMsg;
    cMsg.deserialize(message.data);

    MessageOption option(message.option.sender(), EV_VNETLIB_SWITCH_PORT_CFG_ACK, 0, 0);
    
    //入参检查
    if(cMsg.m_strName.empty() && cMsg.m_strUuid.empty())
    {
        VNET_LOG(VNET_LOG_ERROR, "CSwitchAgent::ProcSwitchPortCfg Switch is NULL.\n");
        nRet = ERROR_SWITCH_NAME_INVALID;
        goto SWITCHPORT_CFG_ACK;
    }

    if(cMsg.m_vecPort.empty())
    {
        VNET_LOG(VNET_LOG_ERROR, "CSwitchAgent::ProcSwitchPortCfg: UplinkPort is empty.\n");
        nRet = ERROR_SWTICH_PORT_INVALID;
        goto SWITCHPORT_CFG_ACK;
    }
    
    if(m_bOpenDbg)
    {
        cMsg.Print();
    }

    switch(cMsg.m_nOper)
    {
        case EN_ADD_SWITCH_PORT:
        {
            nRet = DoSwitchPortAdd(cMsg);
            break;
        }
        case EN_DEL_SWITCH_PORT:
        {
            nRet = DoSwitchPortDel(cMsg);
            break;
        }
        default:
        {
            VNET_LOG(VNET_LOG_ERROR, "CSwitchAgent::ProcSwitchPortCfg: Msg operation code (%d) is invalid.\n", cMsg.m_nOper);  
            nRet = ERROR_SWITCH_OPERCODE_INVALID;   
            VNetFormatString(cMsg.m_strRetInfo, "Switch operator(%d) is invalid.", cMsg.m_nOper);
            break;
        }
    }

SWITCHPORT_CFG_ACK:
    cMsg.m_nRetCode = nRet;
    return m_pMU->Send(cMsg, option);
}

/*******************************************************************************
* 函数名称： GetSwitchInstance
* 功能描述： 获取交换模块实例
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： CSwitchModule* --实例化对象指针
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* ------------------------------------------------------------------------------
* 2013/3          V1.0       zhongchsh     创 建
*******************************************************************************/
CSwitchDev * CSwitchAgent::GetSwitchInstance(const int32 nSwType)
{
    CSwitchDev *pSwDev = NULL;
    switch(nSwType)
    {
        case PHYSICAL_SWITCH_TYPE:
        {
           // pSwDev = new CPhySwitchModule();
            break;
        }
        case SOFT_VIR_SWITCH_TYPE:
        {
            pSwDev = new CSoftDVSDev();
            break;
        }
        case EMB_VIR_SWITCH_TYPE:
        {
            pSwDev = new CEmbDVSDev();
            break;
        }
        default:
        {
            VNET_LOG(VNET_LOG_INFO, "CSwitchAgent::GetSwitchInstance switch type is %d, Not Support!!\n", nSwType);
            break;
        }
    }
    return pSwDev;
}  

/*******************************************************************************
* 函数名称： GetSwitchModule
* 功能描述： 获取交换模块
* 访问的表： 无
* 修改的表： 无
* 输入参数： switch name
* 输出参数： 无
* 返 回 值： 无
* 其它说明： 无
* 修改日期        版本号     修改人       修改内容
* ------------------------------------------------------------------------------
* 2013/3          V1.0       zhongchsh     创 建
*******************************************************************************/
CSwitchDev * CSwitchAgent::GetSwitchDev(const string &strSwName)
{
    list<CSwitchDev*>::iterator iter = m_lstSwitchDev.begin();
    for(; iter != m_lstSwitchDev.end(); ++iter)
    {
        if(0 == (*iter)->GetDevName().compare(strSwName))
        {
            return (*iter);
        }
    }    
    return NULL;
}

/*******************************************************************************
* 函数名称： DoSwitchPortAdd
* 功能描述： 添加交换uplink端口
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* ------------------------------------------------------------------------------
* 2013/3          V1.0       钟春山        创 建
*******************************************************************************/
int32 CSwitchAgent::DoSwitchPortAdd(CSwitchCfgMsgToVNA &cMsg)
{
    int32 nFind = 0;
    CSwitchDev *pSwitchDev = GetSwitchDev(cMsg.m_strName);
    if(NULL == pSwitchDev)
    {
        pSwitchDev = GetSwitchInstance(cMsg.m_nType);
        if(NULL == pSwitchDev)
        {
            VNET_LOG(VNET_LOG_ERROR, "CSwitchAgent::DoSwitchPortAdd: call GetSwitchInstance Failed.\n");
            return ERROR_SWITCH_INSTANCE_FAIL;
        }   
        nFind = 1;
    }

    //检查bond名字是否冲突
    if(cMsg.m_strBondName.empty())
    {
        cMsg.m_strBondName = pSwitchDev->GetDVSBondName();
    }
    
    //创建交换
    int32 nRet = pSwitchDev->CreateSwitch(cMsg);
    if(SWITCH_OPER_SUCCESS != nRet)
    {
        if(1 == nFind)
        {
            delete pSwitchDev;
        }
        VNET_LOG(VNET_LOG_ERROR, "CSwitchAgent::DoSwitchPortAdd: CreateSwitch[%s] failed, ret = %d\n", 
                      cMsg.m_strName.c_str(), nRet);
        return nRet; 
    } 
    
    if(0 == nFind) 
    { 
        //内存中已经存在该记录
        if(cMsg.m_strBondName != pSwitchDev->GetDVSBondName())
        {
            pSwitchDev->SetDVSBondName(cMsg.m_strBondName);
        }
        if(cMsg.m_nBondMode != pSwitchDev->GetDVSBondMode())
        {
            pSwitchDev->SetDVSBondMode(cMsg.m_nBondMode);
        }   
        if(cMsg.m_nEvbMode != pSwitchDev->GetDVSEvbMode())
        {
            pSwitchDev->SetDVSEvbMode(cMsg.m_nEvbMode);
        }
        if(cMsg.m_nMaxVnic != pSwitchDev->GetDVSMaxVnic())
        {
            pSwitchDev->SetDVSMaxVnic(cMsg.m_nMaxVnic);
        }        
        if(cMsg.m_cPGInfo.GetMtu() != pSwitchDev->GetDevMTU())
        {
            pSwitchDev->SetDevMTU(cMsg.m_cPGInfo.GetMtu()); 
        }
        if(cMsg.m_strSwIf != pSwitchDev->GetDVSInterface())
        {
            pSwitchDev->SetDVSInterface(cMsg.m_strSwIf);
        }
        if(cMsg.m_strVtepPort != pSwitchDev->GetDVSVtepPort())
        {
            pSwitchDev->SetDVSVtepPort(cMsg.m_strVtepPort);
        }
        if(cMsg.m_strVtepIp != pSwitchDev->GetDVSVtepPortIP())
        {
            pSwitchDev->SetDVSVtepPortIP(cMsg.m_strVtepIp);
        }
        if(cMsg.m_strVtepMask != pSwitchDev->GetDVSVtepPortMask())
        {
            pSwitchDev->SetDVSVtepPortMask(cMsg.m_strVtepMask);
        }
	if(cMsg.m_cPGInfo.GetTrunkNatvieVlanNum() != pSwitchDev->GetDVSNativeVlan())
        {
            pSwitchDev->SetDVSNativeVlan(cMsg.m_cPGInfo.GetTrunkNatvieVlanNum());
        }
        pSwitchDev->SetDVSUplinkPort(cMsg.m_vecPort);
        
    }
    else
    {
        pSwitchDev->SetDevName      (cMsg.m_strName);
        pSwitchDev->SetSwitchUuid   (cMsg.m_strUuid);
        pSwitchDev->SetSwitchType   (cMsg.m_nType);
        pSwitchDev->SetDevMTU       (cMsg.m_cPGInfo.GetMtu());  //交换MTU等于端口组MTU
        pSwitchDev->SetDVSEvbMode   (cMsg.m_nEvbMode);
        pSwitchDev->SetDVSMaxVnic   (cMsg.m_nMaxVnic);
        pSwitchDev->SetDVSBondName  (cMsg.m_strBondName);
        pSwitchDev->SetDVSBondMode  (cMsg.m_nBondMode);
        pSwitchDev->SetDVSUplinkPort(cMsg.m_vecPort);
        pSwitchDev->SetDVSInterface (cMsg.m_strSwIf);
        pSwitchDev->SetDVSVtepPort  (cMsg.m_strVtepPort);
        pSwitchDev->SetDVSVtepPortIP(cMsg.m_strVtepIp);
        pSwitchDev->SetDVSVtepPortMask(cMsg.m_strVtepMask);
        pSwitchDev->SetDVSNativeVlan(cMsg.m_cPGInfo.GetTrunkNatvieVlanNum());
        m_lstSwitchDev.push_back(pSwitchDev);
    }
    return SWITCH_OPER_SUCCESS;
}

/*******************************************************************************
* 函数名称： DoSwitchPortDel
* 功能描述： 删除交换中端口
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* ------------------------------------------------------------------------------
* 2013/3          V1.0       钟春山        创 建
*******************************************************************************/
int32 CSwitchAgent::DoSwitchPortDel(CSwitchCfgMsgToVNA &cMsg)
{
    CSwitchDev *pSwitchDev = NULL;
    list<CSwitchDev*>::iterator iter = m_lstSwitchDev.begin();
    for(; iter != m_lstSwitchDev.end(); ++iter)
    {
        if(0 == (*iter)->GetDevName().compare(cMsg.m_strName))
        {
            pSwitchDev = *iter;
            vector<string> vecNew; //删除后剩余端口
            int32 nRet = pSwitchDev->DeleteSwitchPort(cMsg, vecNew);
            if(SWITCH_OPER_SUCCESS != nRet)
            {
                VNET_LOG(VNET_LOG_ERROR, "CSwitchAgent::DoSwitchPortDel: call DeleteSwitchPort failed. ret = %d\n", nRet);
                return nRet;
            }
            
            //更新内存信息
            if(vecNew.empty())
            {
                //清空内存
                m_lstSwitchDev.erase(iter);
                
                delete pSwitchDev;
                return SWITCH_OPER_SUCCESS;
            }
            if(1 == vecNew.size())
            {
                //去掉bond
                pSwitchDev->SetDVSBondName("");
                pSwitchDev->SetDVSBondMode(0);
            }
            
            //更新成员口
            pSwitchDev->SetDVSUplinkPort(vecNew);
            pSwitchDev->SetDVSInterface(cMsg.m_strSwIf);
            return SWITCH_OPER_SUCCESS;
        }
    } 

    //内存无该记录，不能执行任何操作
    VNET_LOG(VNET_LOG_ERROR, "CSwitchAgent::DoSwitchPortDel: no instance existed in VNA, so delete port failed. \n");
    VNetFormatString(cMsg.m_strRetInfo, "No instance existed in VNA, so delete port failed.");

    return ERROR_SWITCH_INSTANCE_FAIL;
}

/*******************************************************************************
* 函数名称： DoSwitchDel
* 功能描述： 删除交换
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* ------------------------------------------------------------------------------
* 2013/3          V1.0       钟春山        创 建
*******************************************************************************/
int32 CSwitchAgent::DoSwitchDel(CSwitchCfgMsgToVNA &cMsg)
{
    int32 nRet;
    CSwitchDev *pSwitchDev = NULL;

    //先从内存中获取句柄
    list<CSwitchDev*>::iterator iter = m_lstSwitchDev.begin();
    for(; iter != m_lstSwitchDev.end(); ++iter)
    {
        if(0 == (*iter)->GetDevName().compare(cMsg.m_strName))
        {
            pSwitchDev = *iter;
            nRet = pSwitchDev->DeleteSwitch(cMsg);
            if(SWITCH_OPER_SUCCESS != nRet)
            {
                VNET_LOG(VNET_LOG_ERROR, "CSwitchAgent::DoSwitchDel DeleteSwitch[%s] failed, ret = %d\n", cMsg.m_strName.c_str(), nRet);
                return nRet; 
            } 
            m_lstSwitchDev.erase(iter);
            break;
        }
    } 
    
    //内存中无改记录，也要删除
    if(NULL == pSwitchDev)
    {
        pSwitchDev = GetSwitchInstance(cMsg.m_nType);
        if(NULL == pSwitchDev)
        {
            VNET_LOG(VNET_LOG_ERROR, "CSwitchAgent::DoSwitchDel: GetSwitchInstance Failed.\n");
            return ERROR_SWITCH_INSTANCE_FAIL;
        }   
        nRet = pSwitchDev->DeleteSwitch(cMsg);
        if(SWITCH_OPER_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, "CSwitchAgent::DoSwitchDel DeleteSwitch failed, ret = %d\n", nRet);
            delete pSwitchDev;
            return nRet; 
        } 
    }   
           
    if(pSwitchDev)
    {
        delete pSwitchDev;
    }
    
    return SWITCH_OPER_SUCCESS;
}

/*******************************************************************************
* 函数名称： DoSwitchModify
* 功能描述： 修改交换属性
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* ------------------------------------------------------------------------------
* 2013/3          V1.0       钟春山        创 建
*******************************************************************************/
int32 CSwitchAgent::DoSwitchModify(CSwitchCfgMsgToVNA &cMsg)
{
    CSwitchDev *pSwitchDev = GetSwitchDev(cMsg.m_strName);
    if(NULL == pSwitchDev)
    {
        VNET_LOG(VNET_LOG_ERROR, "CSwitchAgent::DoSwitchModify GetSwitchDev is NULL, Switch is not exist.\n");
        return ERROR_SWITCH_INSTANCE_FAIL;
    }
    
    //适配PG更改
    if(0 != pSwitchDev->SetSwitchAttr(cMsg))
    {
        VNET_LOG(VNET_LOG_ERROR, "CSwitchAgent::DoSwitchModify SetSwitchAttr <%s> failed.\n", cMsg.m_strName.c_str());
        return ERROR_SWITCH_SET_ATTR_FAIL; 
    } 
    
    if(cMsg.m_cPGInfo.GetMtu() != pSwitchDev->GetDevMTU())
    {
        pSwitchDev->SetDevMTU(cMsg.m_cPGInfo.GetMtu()); 
    }
    if(cMsg.m_nEvbMode != pSwitchDev->GetDVSEvbMode())
    {
        pSwitchDev->SetDVSEvbMode(cMsg.m_nEvbMode);
    }
    if(cMsg.m_nMaxVnic != pSwitchDev->GetDVSMaxVnic())
    {
        pSwitchDev->SetDVSMaxVnic(cMsg.m_nMaxVnic);
    }
    if(cMsg.m_strVtepPort != pSwitchDev->GetDVSVtepPort())
    {
        pSwitchDev->SetDVSVtepPort(cMsg.m_strVtepPort);
    }
    if(cMsg.m_strVtepIp != pSwitchDev->GetDVSVtepPortIP())
    {
        pSwitchDev->SetDVSVtepPortIP(cMsg.m_strVtepIp);
    }
    if(cMsg.m_strVtepMask != pSwitchDev->GetDVSVtepPortMask())
    {
        pSwitchDev->SetDVSVtepPortMask(cMsg.m_strVtepMask);
    }
    if(cMsg.m_cPGInfo.GetTrunkNatvieVlanNum() != pSwitchDev->GetDVSNativeVlan())
    {
        pSwitchDev->SetDVSNativeVlan(cMsg.m_cPGInfo.GetTrunkNatvieVlanNum());
    }    
    return SWITCH_OPER_SUCCESS;
}

/*******************************************************************************
* 函数名称： DoSwitchReport
* 功能描述： 交换上报接口，目前只上报软交换
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
void CSwitchAgent::DoSwitchReport(vector<CSwitchReportInfo> &vecReport)
{    
    CSoftDVSDev *pSoftDev = new CSoftDVSDev();
    if(NULL == pSoftDev)
    {
        VNET_LOG(VNET_LOG_ERROR, "CSwitchAgent::DoSwitchReport CSoftDVSDev Failed.\n");
        return;
    }  
    
    pSoftDev->GetSoftDVS(vecReport);
}

/*******************************************************************************
* 函数名称： AllocDVSRes
* 功能描述： 检查DVS资源是否满足
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
int32 CSwitchAgent::AllocDVSRes(vector<CVNetVnicDetail> &vecVnicInfo)
{
    vector<CVNetVnicDetail>::iterator iter = vecVnicInfo.begin();
    for(; iter != vecVnicInfo.end(); ++iter)
    {
        if (ISOLATE_VXLAN == iter->GetIsolateType())
        {
            CVxlanAgent *pAgent = CVxlanAgent::GetInstance();
            if (!pAgent)
            {
                VNET_LOG(VNET_LOG_ERROR, "CSwitchAgent::AllocDVSRes: CVxlanAgent::GetInstance() failed.\n");
                return -1;
            }
            
            if (false==pAgent->IsVxlanBridgeExist(iter->GetSwitchName())) 
            {
                VNET_LOG(VNET_LOG_ERROR, "CSwitchAgent::AllocDVSRes: IsVxlanBridgeExist <%s>  failed.\n", iter->GetSwitchName().c_str());
                return -1;
            }            
        }
        else
        {             
            CSwitchDev *pSwitchDev = GetSwitchDev(iter->GetSwitchName());
            if(NULL == pSwitchDev)
            {
                VNET_LOG(VNET_LOG_ERROR, "CSwitchAgent::AllocDVSRes: GetSwitchDev <%s> failed.\n", iter->GetSwitchName().c_str());
                return -1;
            }  
            
            if(0 != pSwitchDev->IsSwitchValid(*iter, TRUE))
            {
                VNET_LOG(VNET_LOG_ERROR, "CSwitchAgent::AllocDVSRes: IsSwitchValid failed.\n");
                return -1;
            }   
        }        
    }
    return 0;
}

int32 CSwitchAgent::FreeDVSRes(vector<CVNetVnicDetail> &vecVnicInfo)
{
    string strTmpPortName;
    string strTmpVf;
    CPhyPortManager * pPortMgr = CPhyPortManager::GetInstance();

    if(NULL == pPortMgr)
    {
        VNET_LOG(VNET_LOG_ERROR, "CSwitchAgent::FreeDVSRes: CPhyPortManager::GetInstance() failed.\n"); 
        return -1;
    }
    
    vector<CVNetVnicDetail>::iterator iter = vecVnicInfo.begin();
    for(; iter != vecVnicInfo.end(); ++iter)
    {
        if(iter->GetIsSriov() && !pPortMgr->IsPciExit(iter->GetPciOrder(), strTmpPortName, strTmpVf))
        {
            //清除SR-IOV网卡MAC和VLAN
            if(0 != pPortMgr->SetVFPortMac(iter->GetPciOrder(), VNET_SRIOV_RANDOM_MAC))
            {
                VNET_LOG(VNET_LOG_ERROR, "CSwitchAgent::FreeDVSRes: SetVFPortMac<%s> failed.\n", iter->GetPciOrder().c_str()); 
                return -1;        
            }
            if(0 != pPortMgr->SetVFPortVlan(iter->GetPciOrder(), 0))
            {
                VNET_LOG(VNET_LOG_ERROR, "CSwitchAgent::FreeDVSRes: SetVFPortVlan<%s> failed.\n", iter->GetPciOrder().c_str()); 
                return -1;        
            } 
        }  
    }
    return 0;
}
        
int32 CSwitchAgent::GetEVBMode(const string &strDVSName, int32 &nEVBMode)
{
    CSwitchDev *pSwitchDev = GetSwitchDev(strDVSName);
    if(NULL == pSwitchDev)
    {
        VNET_LOG(VNET_LOG_ERROR, "CSwitchAgent::GetEVBMode: GetSwitchDev <%s> failed.\n", strDVSName.c_str());
        return -1;
    }   

    nEVBMode = pSwitchDev->GetDVSEvbMode();
    return 0;
}

void CSwitchAgent::DoMonitor(CNetDevVisitor *dev_visit)
{
    list<CSwitchDev*>::iterator bridge_dev = m_lstSwitchDev.begin();

    for(; bridge_dev!=m_lstSwitchDev.end(); bridge_dev++)
    {
        (*bridge_dev)->Accept(dev_visit);
    }
};

void CSwitchAgent::DoMonitor(CBridgeVisitor* dev_visit)
{    
    list<CSwitchDev*>::iterator bridge_dev = m_lstSwitchDev.begin();

    for(; bridge_dev!=m_lstSwitchDev.end(); bridge_dev++)
    {
        (*bridge_dev)->Accept(dev_visit);
    }
};

void CSwitchAgent::DbgShowSwitchDev()
{   
    list<CSwitchDev*>::iterator iter = m_lstSwitchDev.begin();
    cout << "------------------------ VNA Switch Info --------------------------" << endl;
    for(; iter != m_lstSwitchDev.end(); ++iter)
    {
        cout << "---------------------------------------------------------" <<endl;
        cout << "Switch UUID: " << (*iter)->GetSwitchUuid() << endl;
        cout << "Switch Name: " << (*iter)->GetDevName() << endl;
        cout << "Switch MTU: " << (*iter)->GetDevMTU() << endl;
        cout << "Switch Type: " << (*iter)->GetSwitchType() << endl;
        cout << "Switch State: " << (*iter)->GetSwitchState() << endl;
        cout << "DVS EVB mode: " << (*iter)->GetDVSEvbMode() << endl;
        cout << "DVS Bond name: " << (*iter)->GetDVSBondName() << endl;
        cout << "DVS Bond Mode: " << (*iter)->GetDVSBondMode() << endl;
        cout << "DVS Max Vnics: " << (*iter)->GetDVSMaxVnic() << endl;
        cout << "DVS Interface: " << (*iter)->GetDVSInterface() <<endl;
        cout << "DVS VTEP: " << (*iter)->GetDVSVtepPort()<<" IP: "
             << (*iter)->GetDVSVtepPortIP() <<" Mask: "<<(*iter)->GetDVSVtepPortMask()<<endl;
        cout << "DVS Native VLAN: "<< (*iter)->GetDVSNativeVlan() <<endl;
        
        vector<string> upport;
        (*iter)->GetDVSUplinkPort(upport);
        vector<string>::iterator it = upport.begin();
        for(; it != upport.end(); ++it)
        {
            cout << "DVS Uplink Port: " << (*it) << endl;
        }
    }
    cout << "-------------------------------------------------------------------" << endl;
}

void DbgSetVNASwitchPrint(BOOL bOpen)
{
    CSwitchAgent *pAgent = CSwitchAgent::GetInstance();
    if(pAgent)
    {
        pAgent->SetDbgPrintFlag(bOpen);
    }
}

void DbgShowVNASwitch()
{
    CSwitchAgent *pMgr = CSwitchAgent::GetInstance();
    if(NULL != pMgr)
    {
        pMgr->DbgShowSwitchDev();
    }
}

DEFINE_DEBUG_FUNC(DbgSetVNASwitchPrint);
DEFINE_DEBUG_FUNC(DbgShowVNASwitch);

}

