/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：vnet_portmgr.cpp
* 文件标识：
* 内容摘要：CPortMgr类实现文件
* 当前版本：1.0
* 作    者：liuhx
* 完成日期：2013年2月25日
******************************************************************************/

#include "vnet_comm.h"
#include "vnet_msg.h"
#include "vnet_event.h"
#include "vnet_error.h"
#include "ipv4_addr.h"
#include "ipv4_addr_range.h"
#include "ipv4_addr_pool.h"
#include "ipv4_pool_mgr.h"
#include "logic_network.h"
#include "logic_network_mgr.h"
#include "vnet_portmgr.h"
#include "vnetlib_msg.h"
#include "vnetlib_event.h"
#include "vnet_mid.h"
#include "port_group_mgr.h"
#include "vnm_vxlan_mgr.h"
#include <fstream>

namespace zte_tecs
{
CPortMgr* CPortMgr::s_pInstance = NULL;

CPortMgr::CPortMgr()
{
    m_bOpenDbgInf = 0;
    m_nPhyPortDbgInf = 1;
    m_nSRIOVDbgInf = 1;
    m_nBondDbgInf = 1;
    m_nUplinkLpDbgInf = 1;
    m_nKrPortDbgInf = 0;
    m_pMu = NULL;

    tTimerID = INVALID_TIMER_ID;
	
    m_vKernelPortInfo.clear();
    m_vLoopPortInfo.clear();
};

CPortMgr::~CPortMgr()
{
    m_pMu = NULL;

    if (s_pInstance)
    {
        delete s_pInstance;
        s_pInstance = NULL;
    }

    m_bOpenDbgInf = 0;
    m_nPhyPortDbgInf = 0;
    m_nSRIOVDbgInf = 0;
    m_nBondDbgInf = 0;
    m_nUplinkLpDbgInf = 0;
    m_nKrPortDbgInf = 0;

    m_vKernelPortInfo.clear();
    m_vLoopPortInfo.clear();

    if (INVALID_TIMER_ID != tTimerID)
    {
        m_pMu->KillTimer(tTimerID);
        tTimerID = INVALID_TIMER_ID;
    }		
};

//设置打印开关
void CPortMgr::SetDbgPrint(int32 phy, int32 sriov, int32 bond, int32 krport, int32 uplinkLpport, int32 OpenDbgInf)
{
    m_nPhyPortDbgInf = phy;
    m_nSRIOVDbgInf = sriov;
    m_nBondDbgInf = bond;
    m_nKrPortDbgInf = krport;
    m_nUplinkLpDbgInf = uplinkLpport;
    m_bOpenDbgInf = OpenDbgInf;
}

//初始化
STATUS CPortMgr::Init(MessageUnit *mu)
{
    if (NULL == mu)
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::Init: mu is NULL.\n");
        return -1;
    }

    m_pMu = mu;

    /*m_tPortTimerId = m_pMu->CreateTimer();
    if (INVALID_TIMER_ID == m_tPortTimerId)
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::Init Create m_tPortTimerId failed.\n");
        return -1;
    }*/

    return 0;
}

void CPortMgr::MessageEntry(const MessageFrame& message)
{
    switch (message.option.id())
    {
    case EV_VNETLIB_KERNALPORT_CFG:
    {
        ProcKernelPortReqMsg(message);
        break;
    }
    case EV_VNETLIB_KERNALPORT_CFG_ACK:
    {
        ProcKernelPortAckMsg(message);
        break;
    }
    case EV_VNETLIB_CFG_LOOPBACK:
    {
        ProcLoopBackPortReqMsg(message);
        break;
    }
    case EV_VNETLIB_CFG_LOOPBACK_ACK:
    {
        ProcLoopBackPortAckMsg(message);
        break;
    }
    case TIMER_VNA_RESET_PORTINFO:
    {
        ProcPortCfgToVna();
        break;
    }
    case TIMER_VNA_RESET_PORTINFO_ACK:
    {
        ProcPortCfgToVnaAck(message);
        break;
    }
    default:
    {
        VNET_LOG(VNET_LOG_WARN, "CPortMgr::MessageEntry receive unknown msg[%d] \n", message.option.id());
        break;
    }
    }
}

int32 CPortMgr::ProcVnaPortInfo(CMessageVNAPortInfoReport &msg)
{
    if (0 != DoPortInfoPrepare(msg))
    {
        //return -1;
    }

    if (0 != DoUplinkLoopPortInfoReport(msg))
    {
        //return -1;
    }

    if (0 != DoKernelPortInfoReport(msg))
    {
        //return -1;
    }

    if (0 != DoKernelPortCfgInfoReport(msg))
    {
        //return -1;
    }

    if (0 != DoTrunkPortInfoReport(msg))
    {
        //return -1;
    }

    if (0 != DoVtepPortCfgInfoReport(msg))
    {
        //return -1;
    }
	
    return 0;
}

//非sriov和sriov处理之前，判断port类型是否变化，即非sriov和sriov之间有无切换
//清理完相关数据，然后再进行端口上报处理
int32 CPortMgr::DoPortInfoPrepare(CMessageVNAPortInfoReport &msg)
{
    int32  nRet;
    vector<CPhyPortReport> vPhyPort;
    vector<CPhyPortReport> vSriovPhyPort;

    if (msg._vna_id.empty())
    {
        return 0;
    }

    if (msg._phy_port.empty())
    {
        DoPhyPortInfoReport(msg._vna_id, vPhyPort);
        DoSriovPortInfoReport(msg._vna_id, vSriovPhyPort);
        return 0;
    }

    CDBOperatePortPhysical * pOper = GetDbIPortPhy();
    if (NULL == pOper)
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::DoPortInfoPrepare GetDbIPortPhy failed \n");
        //VNET_ASSERT(0);
        return -1;
    }

    /*CDBOperatePortSriovVf * pVFOper = GetDbIPortSriovVf();
    if ( NULL == pOper )
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::DoPortInfoPrepare GetDbIPortSriovVf failed \n");
        VNET_ASSERT(0);
        return -1;
    }*/


    vector<CDbPortSummary> outVInfo;
    //采用新的summary
    nRet = pOper->QueryAllSummary(msg._vna_id.c_str(), outVInfo);
    if (nRet != 0)
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] DoPortInfoPrepare QuerySummary failed \n", msg._vna_id.c_str());
        //VNET_ASSERT(0);
        return -1;
    }

    vector<CPhyPortReport>::iterator it_phy = msg._phy_port.begin();

    if (outVInfo.empty())
    {
        for (; it_phy != msg._phy_port.end(); ++it_phy)
        {
            if (it_phy->_is_sriov == 0)
            {
                vPhyPort.push_back(*it_phy);
            }
            else
            {
                vSriovPhyPort.push_back(*it_phy);
            }
        }

        DoPhyPortInfoReport(msg._vna_id, vPhyPort);
        DoSriovPortInfoReport(msg._vna_id, vSriovPhyPort);
        return 0;
    }

    vector<CDbPortSummary>::iterator it_info = outVInfo.begin();

    for (; it_phy != msg._phy_port.end(); ++it_phy)
    {
        for (it_info = outVInfo.begin(); it_info != outVInfo.end(); ++it_info)
        {
            if (it_phy->_basic_info._name == it_info->GetName())
            {
                if (it_phy->_is_sriov == 0)
                {
                    vPhyPort.push_back(*it_phy);
                }
                else
                {
                    vSriovPhyPort.push_back(*it_phy);
                }
#if 0

                //开始做清理动作
                CDbPortPhysical info;
                info.SetUuid(it_info->GetUuid().c_str());
                nRet = pOper->Query(info);
                if (nRet != 0)
                {
                    //查询失败，不做清理，记录日志
                    VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s %s] DoPortInfoPrepare Query failed for empty \n", msg._vna_id.c_str(), it_info->GetName().c_str());
                    break;
                }

                if (it_phy->_is_sriov == info.GetIsSriov())
                {
                    if (it_phy->_is_sriov == 0)
                    {
                        vPhyPort.push_back(*it_phy);
                    }
                    else
                    {
                        vSriovPhyPort.push_back(*it_phy);
                    }
                }
                else
                {
                    if (it_phy->_is_sriov == 0)
                    {
                        //清理掉先前的sriov信息
                        nRet = pVFOper->Del(it_info->GetUuid().c_str());
                        if ( nRet != 0 )
                        {
                            VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s %s] DoPortInfoPrepare Del failed for old VF info \n", msg._vna_id.c_str(), it_info->GetName().c_str());
                            break;
                        }
                        nRet = pOper->Del(it_info->GetUuid().c_str());
                        if ( nRet != 0 )
                        {
                            VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s %s] DoPortInfoPrepare Del failed for old Sriov Port info \n", msg._vna_id.c_str(), it_info->GetName().c_str());
                            break;
                        }
                        vPhyPort.push_back(*it_phy);
                    }
                    else
                    {
                        //清理掉先前的物理端口信息
                        nRet = pOper->Del(it_info->GetUuid().c_str());
                        if ( nRet != 0 )
                        {
                            VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s %s] DoPortInfoPrepare Del failed for old Phy Port info \n", msg._vna_id.c_str(), it_info->GetName().c_str());
                            break;
                        }
                        vSriovPhyPort.push_back(*it_phy);
                    }
                }
#endif
                break;
            }
        }

        if (it_info == outVInfo.end())
        {
            if (it_phy->_is_sriov == 0)
            {
                vPhyPort.push_back(*it_phy);
            }
            else
            {
                vSriovPhyPort.push_back(*it_phy);
            }

        }
    }

    DoPhyPortInfoReport(msg._vna_id, vPhyPort);
    DoSriovPortInfoReport(msg._vna_id, vSriovPhyPort);

    return 0;
}

//物理端口上报
int32 CPortMgr::DoPhyPortInfoReport(const string strVnaUuid, vector<CPhyPortReport> &vPhyPort)
{
    int32  nRet;

    CDBOperatePortPhysical * pOper = GetDbIPortPhy();
    if (NULL == pOper)
    {
        //VNET_ASSERT(0);
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::DoPhyPortInfoReport GetDbIPortPhy failed \n");
        return -1;
    }

    vector<CDbPortSummary> outVInfo;
    nRet = pOper->QuerySummary(strVnaUuid.c_str(), 0, outVInfo);
    if ( nRet != 0 )
    {
        //VNET_ASSERT(0);
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] DoPhyPortInfoReport QuerySummary failed \n", strVnaUuid.c_str());
        return -1;
    }

    //vna 上报物理端口为空，置online为0，这个查出来的全是本vna的物理端口号
    //设置online状态时候并没有把一些关键字段重置，如state
    if (vPhyPort.empty())
    {
        if (outVInfo.empty())
        {
            return 0;
        }

        vector<CDbPortSummary>::iterator it_null = outVInfo.begin();
        for (; it_null != outVInfo.end(); ++it_null)
        {
            CDbPortPhysical info;
            info.SetUuid(it_null->GetUuid().c_str());
            nRet = pOper->Query(info);
            if ( nRet != 0 )
            {
                if (m_nPhyPortDbgInf)
                {
                    info.DbgShow();
                }
                VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] DoPhyPortInfoReport Query failed for empty \n", it_null->GetName().c_str());
                //VNET_ASSERT(0);
                continue;
            }

            //上报为空，设置离线同时设置一致性，短暂离线不考虑，只针对kernel、loop配置部分
            info.SetIsOnline(0);
            info.SetIsConsistency(0);
            nRet = pOper->Modify(info);
            if ( nRet != 0 )
            {
                if (m_nPhyPortDbgInf)
                {
                    info.DbgShow();
                }
                VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] DoPhyPortInfoReport Modify failed for empty \n", info.GetName().c_str());
                //VNET_ASSERT(0);
            }
        }

        return 0;
    }

    //为空直接添加
    if (outVInfo.empty())
    {
        vector<CPhyPortReport>::iterator it_phy = vPhyPort.begin();
        for (; it_phy != vPhyPort.end(); ++it_phy)
        {
            CDbPortPhysical info;
            SetPhyPortInfo(strVnaUuid, info, *it_phy);
            info.SetIsOnline(1);
            info.SetIsConsistency(1);
            nRet = pOper->Add(info);
            if ( nRet != 0 )
            {
                if (m_nPhyPortDbgInf)
                {
                    info.DbgShow();
                }
                VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] DoPhyPortInfoReport Add for null failed \n", info.GetName().c_str());
                //VNET_ASSERT(0);
            }

            //add ip/mask
            AddPortIp(info.GetUuid(), info.GetName(), (*it_phy)._basic_info._ip, (*it_phy)._basic_info._mask, 0, 1);
        }

        return 0;
    }

    //执行modfiy或者del操作
    vector<CDbPortSummary>::iterator it_a = outVInfo.begin();
    vector<CPhyPortReport>::iterator it_phy_a;
    for (; it_a != outVInfo.end(); ++it_a)
    {
        for (it_phy_a = vPhyPort.begin(); it_phy_a != vPhyPort.end(); ++it_phy_a)
        {
            if (it_a->GetName() == it_phy_a->_basic_info._name)
            {
                CDbPortPhysical info;
                info.SetUuid(it_a->GetUuid().c_str());
                nRet = pOper->Query(info);
                if ( nRet != 0 )
                {
                    if (m_nPhyPortDbgInf)
                    {
                        info.DbgShow();
                    }
                    VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] DoPhyPortInfoReport Query failed \n", info.GetName().c_str());
                    //VNET_ASSERT(0);
                    break;
                }

                if (info.GetIsPortCfg())
                {
                    //首先比较可配置的属性
                    if (!CmpPhyPortCfgInfo(info, *it_phy_a))
                    {
                        info.SetIsConsistency(0);
                        nRet = pOper->Modify(info);
                        if ( nRet != 0 )
                        {
                            if (m_nPhyPortDbgInf)
                            {
                                info.DbgShow();
                            }
                            //VNET_ASSERT(0);
                            VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] DoPhyPortInfoReport Modify failed \n", info.GetName().c_str());
                        }
                    }
                    else
                    {
                        if (!(info.GetIsConsistency()))
                        {
                            info.SetIsConsistency(1);
                            nRet = pOper->Modify(info);
                            if ( nRet != 0 )
                            {
                                if (m_nPhyPortDbgInf)
                                {
                                    info.DbgShow();
                                }
                                //VNET_ASSERT(0);
                                VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] DoPhyPortInfoReport Modify failed \n", info.GetName().c_str());
                            }
                        }
                    }
                }

                //再比较没有配置的属性
                if (!CmpPhyPortInfo(info, *it_phy_a))
                {
                    //统一设置了，是否考虑单个设置?
                    SetPhyPortInfo(strVnaUuid, info, *it_phy_a);
                    nRet = pOper->Modify(info);
                    if ( nRet != 0 )
                    {
                        if (m_nPhyPortDbgInf)
                        {
                            info.DbgShow();
                        }
                        //VNET_ASSERT(0);
                        VNET_LOG(VNET_LOG_ERROR, "[%s] CPortMgr::DoPhyPortInfoReport Modify failed \n", info.GetName().c_str());
                    }
                }

                //最后判断是否复活
                if (!(info.GetIsOnline()))
                {
                    info.SetIsOnline(1);
                    info.SetIsConsistency(1);
                    nRet = pOper->Modify(info);
                    if ( nRet != 0 )
                    {
                        if (m_nPhyPortDbgInf)
                        {
                            info.DbgShow();
                        }
                        //VNET_ASSERT(0);
                        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] DoPhyPortInfoReport Modify failed \n", info.GetName().c_str());
                    }
                }

                //modify ip/mask
                ModPortIp(info.GetUuid(), info.GetName(), (*it_phy_a)._basic_info._ip, (*it_phy_a)._basic_info._mask, info.GetIsPortCfg(), info.GetIsOnline());
                break;
            }
        }

        if (it_phy_a == vPhyPort.end())
        {
            CDbPortPhysical info;
            info.SetUuid(it_a->GetUuid().c_str());
            nRet = pOper->Query(info);
            if ( nRet != 0 )
            {
                if (m_nPhyPortDbgInf)
                {
                    info.DbgShow();
                }
                //VNET_ASSERT(0);
                VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] DoPhyPortInfoReport Query failed \n", info.GetName().c_str());
            }

            //此条记录不存在了
            info.SetIsOnline(0);
            info.SetIsConsistency(0);
            nRet = pOper->Modify(info);
            if ( nRet != 0 )
            {
                if (m_nPhyPortDbgInf)
                {
                    info.DbgShow();
                }
                //VNET_ASSERT(0);
                VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] DoPhyPortInfoReport Modify failed \n", info.GetName().c_str());
            }

            //del ip/mask
            DelPortIp(it_a->GetUuid(), it_a->GetName());
        }
    }

    //判断新来的端口
    vector<CDbPortSummary>::iterator it_b;
    vector<CPhyPortReport>::iterator it_phy_b = vPhyPort.begin();
    for (; it_phy_b != vPhyPort.end(); ++it_phy_b)
    {
        for (it_b = outVInfo.begin(); it_b != outVInfo.end(); ++it_b)
        {
            if (it_b->GetName() == it_phy_b->_basic_info._name)
            {
                break;
            }
        }

        if (it_b == outVInfo.end())
        {
            CDbPortPhysical info;
            SetPhyPortInfo(strVnaUuid, info, *it_phy_b);
            info.SetIsConsistency(1);
            info.SetIsOnline(1);
            nRet = pOper->Add(info);
            if ( nRet != 0 )
            {
                if (m_nPhyPortDbgInf)
                {
                    info.DbgShow();
                }
                //VNET_ASSERT(0);
                VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s  %s] DoPhyPortInfoReport Add failed \n", strVnaUuid.c_str(), info.GetName().c_str());
            }

            //add ip/mask
            AddPortIp(info.GetUuid(), info.GetName(), (*it_phy_b)._basic_info._ip, (*it_phy_b)._basic_info._mask, 0, 1);
        }
    }

    return 0;
}

int32 CPortMgr::DoSriovPortInfoReport(const string strVnaUuid, vector<CPhyPortReport> &vSriovPhyPort)
{
    int32  nRet;

    CDBOperatePortPhysical * pOper = GetDbIPortPhy();
    if ( NULL == pOper )
    {
        //VNET_ASSERT(0);
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::DoSriovPortInfoReport GetDbIPortPhy failed \n");
        return -1;
    }

    vector<CDbPortSummary> outVInfo;
    nRet = pOper->QuerySummary(strVnaUuid.c_str(), 1, outVInfo);
    if ( nRet != 0 )
    {
        //VNET_ASSERT(0);
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] DoSriovPortInfoReport QuerySummary failed \n", strVnaUuid.c_str());
        return -1;
    }

    if (vSriovPhyPort.empty())
    {
        if (outVInfo.empty())
        {
            return 0;
        }

        vector<CDbPortSummary>::iterator it_null = outVInfo.begin();
        for (; it_null != outVInfo.end(); ++it_null)
        {
            CDbPortPhysical info;
            info.SetUuid(it_null->GetUuid().c_str());
            nRet = pOper->Query(info);
            if ( nRet != 0 )
            {
                //VNET_ASSERT(0);
                VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] DoSriovPortInfoReport Query failed for empty \n", it_null->GetUuid().c_str());
                continue;
            }

            info.SetIsOnline(0);
            info.SetIsConsistency(0);
            nRet = pOper->Modify(info);
            if ( nRet != 0 )
            {
                if (m_nSRIOVDbgInf)
                {
                    info.DbgShow();
                }
                VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] DoSriovPortInfoReport Modify failed for empty \n",it_null->GetUuid().c_str());
                //VNET_ASSERT(0);
                continue;
            }

            DoVFPortInfoReport(strVnaUuid, vSriovPhyPort, info.GetUuid(), EN_DB_PORT_TYPE_SRIOVVF, info.GetName(), 0);
        }

        return 0;
    }

    //为空直接添加
    if (outVInfo.empty())
    {
        vector<CPhyPortReport>::iterator it_phy = vSriovPhyPort.begin();
        for (; it_phy != vSriovPhyPort.end(); ++it_phy)
        {
            CDbPortPhysical info;
            SetPhySriovInfo(strVnaUuid, info, *it_phy);
            info.SetIsOnline(1);
            info.SetIsConsistency(1);
            nRet = pOper->Add(info);
            if ( nRet != 0 )
            {
                if (m_nSRIOVDbgInf)
                {
                    info.DbgShow();
                }
                VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] DoSriovPortInfoReport Add for null failed \n", info.GetName().c_str());
                //VNET_ASSERT(0);
            }

            //add ip/mask
            AddPortIp(info.GetUuid(), info.GetName(), (*it_phy)._basic_info._ip, (*it_phy)._basic_info._mask, 0, 1);
        }

        //查询做vf动作
        vector<CDbPortSummary> outVF;
        nRet = pOper->QuerySummary(strVnaUuid.c_str(), 1, outVInfo);
        if ( nRet != 0 )
        {
            VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] DoSriovPortInfoReport QuerySummary for VF failed \n", strVnaUuid.c_str());
            //VNET_ASSERT(0);
            return -1;
        }

        //重新获取uuid
        vector<CDbPortSummary>::iterator itVF_a = outVF.begin();
        for (; itVF_a != outVF.end(); ++itVF_a)
        {
            CDbPortPhysical info;
            info.SetUuid(itVF_a->GetUuid().c_str());
            nRet = pOper->Query(info);
            if ( nRet != 0 )
            {
                VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] DoSriovPortInfoReport Query failed \n", itVF_a->GetUuid().c_str());
                //VNET_ASSERT(0);
                continue;
            }

            DoVFPortInfoReport(strVnaUuid, vSriovPhyPort, itVF_a->GetUuid(), EN_DB_PORT_TYPE_SRIOVVF, itVF_a->GetName(), info.GetIsOnline());
        }

        return 0;
    }

    //执行modfiy或者del操作
    vector<CDbPortSummary>::iterator it_a = outVInfo.begin();
    vector<CPhyPortReport>::iterator it_phy_a;
    for (; it_a != outVInfo.end(); ++it_a)
    {
        for (it_phy_a = vSriovPhyPort.begin(); it_phy_a != vSriovPhyPort.end(); ++it_phy_a)
        {
            if (it_a->GetName() == it_phy_a->_basic_info._name)
            {
                CDbPortPhysical info;
                info.SetUuid(it_a->GetUuid().c_str());
                nRet = pOper->Query(info);
                if ( nRet != 0 )
                {
                    VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] DoSriovPortInfoReport Query failed \n", it_a->GetName().c_str());
                    //VNET_ASSERT(0);
                    break;
                }

                if (info.GetIsPortCfg())
                {
                    //首先比较可配置的属性
                    if (!CmpPhySriovCfgInfo(info, *it_phy_a))
                    {
                        info.SetIsConsistency(0);
                        nRet = pOper->Modify(info);
                        if ( nRet != 0 )
                        {
                            if (m_nSRIOVDbgInf)
                            {
                                info.DbgShow();
                            }
                            VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] DoSriovPortInfoReport Modify failed \n", info.GetName().c_str());
                            //VNET_ASSERT(0);
                        }
                    }
                    else
                    {
                        if (!(info.GetIsConsistency()))
                        {
                            info.SetIsConsistency(1);
                            nRet = pOper->Modify(info);
                            if ( nRet != 0 )
                            {
                                if (m_nSRIOVDbgInf)
                                {
                                    info.DbgShow();
                                }
                                VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] DoSriovPortInfoReport Modify failed \n", info.GetName().c_str());
                                //VNET_ASSERT(0);
                            }
                        }
                    }
                }

                //再比较没有配置的属性
                if (!CmpPhySriovInfo(info, *it_phy_a))
                {
                    SetPhySriovInfo(strVnaUuid, info, *it_phy_a);
                    nRet = pOper->Modify(info);
                    if ( nRet != 0 )
                    {
                        if (m_nSRIOVDbgInf)
                        {
                            info.DbgShow();
                        }
                        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] DoSriovPortInfoReport Modify failed \n", info.GetName().c_str());
                        //VNET_ASSERT(0);
                    }
                }

                //最后判断是否复活
                if (!(info.GetIsOnline()))
                {
                    info.SetIsOnline(1);
                    info.SetIsConsistency(1);
                    nRet = pOper->Modify(info);
                    if ( nRet != 0 )
                    {
                        if (m_nSRIOVDbgInf)
                        {
                            info.DbgShow();
                        }
                        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] DoSriovPortInfoReport Modify failed \n", info.GetName().c_str());
                        //VNET_ASSERT(0);
                    }
                }

                //modify ip/mask
                ModPortIp(info.GetUuid(), info.GetName(), (*it_phy_a)._basic_info._ip, (*it_phy_a)._basic_info._mask, info.GetIsPortCfg(), info.GetIsOnline());

                DoVFPortInfoReport(strVnaUuid, vSriovPhyPort, info.GetUuid(), EN_DB_PORT_TYPE_SRIOVVF, info.GetName(), info.GetIsOnline());
                break;
            }
        }

        if (it_phy_a == vSriovPhyPort.end())
        {
            CDbPortPhysical info;
            info.SetUuid(it_a->GetUuid().c_str());
            nRet = pOper->Query(info);
            if ( nRet != 0 )
            {
                if (m_nSRIOVDbgInf)
                {
                    info.DbgShow();
                }
                VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] DoSriovPortInfoReport Query failed \n", info.GetName().c_str());
                //VNET_ASSERT(0);
            }

            //此条记录不存在了
            info.SetIsOnline(0);
            info.SetIsConsistency(0);
            nRet = pOper->Modify(info);
            if ( nRet != 0 )
            {
                if (m_nSRIOVDbgInf)
                {
                    info.DbgShow();
                }
                VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] DoSriovPortInfoReport Modify failed \n", info.GetName().c_str());
                //VNET_ASSERT(0);
            }

            //del ip/mask
            DelPortIp(it_a->GetUuid(), it_a->GetName());

            DoVFPortInfoReport(strVnaUuid, vSriovPhyPort, info.GetUuid(), EN_DB_PORT_TYPE_SRIOVVF, info.GetName(), info.GetIsOnline());
        }
    }

    //判断新来的端口
    vector<CDbPortSummary>::iterator it_b;
    vector<CPhyPortReport>::iterator it_phy_b = vSriovPhyPort.begin();
    for (; it_phy_b != vSriovPhyPort.end(); ++it_phy_b)
    {
        if (it_phy_b->_is_sriov)
        {
            for (it_b = outVInfo.begin(); it_b != outVInfo.end(); ++it_b)
            {
                if (it_b->GetName() == it_phy_b->_basic_info._name)
                {
                    break;
                }
            }

            if (it_b == outVInfo.end())
            {
                CDbPortPhysical info;
                SetPhySriovInfo(strVnaUuid, info, *it_phy_b);
                info.SetIsOnline(1);
                info.SetIsConsistency(1);
                nRet = pOper->Add(info);
                if ( nRet != 0 )
                {
                    if (m_nSRIOVDbgInf)
                    {
                        info.DbgShow();
                    }
                    VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] DoSriovPortInfoReport Add  failed \n", info.GetName().c_str());
                    //VNET_ASSERT(0);
                }

                //add ip/mask
                AddPortIp(info.GetUuid(), info.GetName(), (*it_phy_b)._basic_info._ip, (*it_phy_b)._basic_info._mask, 0, 1);

                DoVFPortInfoReport(strVnaUuid, vSriovPhyPort, info.GetUuid(), EN_DB_PORT_TYPE_SRIOVVF, info.GetName(), info.GetIsOnline());
            }
        }
    }

    return 0;
}

//实际操作还是索引了物理端口vector
//VF没有考虑可设置属性
int32 CPortMgr::DoVFPortInfoReport(const string &strVnaUuid, vector<CPhyPortReport> &vSriovPhyPort, string  strSriovUuid,
                                   int32 nPorType, const string &strPortName, int32 nIsOnline)
{
    int32  nRet;

    CDBOperatePortSriovVf * pOper = GetDbIPortSriovVf();
    if ( NULL == pOper )
    {
        //VNET_ASSERT(0);
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::DoVFPortInfoReport GetDbIPortSriovVf failed \n");
        return -1;
    }

    vector<CDbPortSummary> outVInfo;
    nRet = pOper->QuerySummary(strVnaUuid.c_str(), strSriovUuid.c_str(), outVInfo);
    if ( nRet != 0 )
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s %s] DoVFPortInfoReport QuerySummary failed \n",strVnaUuid.c_str(), strSriovUuid.c_str());
        //VNET_ASSERT(0);
        return -1;
    }

    vector<CPhyPortReport>::iterator it_phyport = vSriovPhyPort.begin();
    for (; it_phyport != vSriovPhyPort.end(); ++it_phyport)
    {
        //找到和端口名称相同的结构下的VF
        if (it_phyport->_basic_info._name == strPortName)
        {
            //为空直接添加
            if (outVInfo.empty())
            {
                vector<CSriovPortReport>::iterator sriovport = it_phyport->_sriov_port.begin();
                for (; sriovport != it_phyport->_sriov_port.end(); ++sriovport)
                {
                    CDbPortSriovVf info;
                    info.SetVnaUuid(strVnaUuid.c_str());
                    info.SetPortType(nPorType);

                    //name暂且写成物理端口名称+vf
                    info.SetName((strPortName + sriovport->_pci).c_str());

                    //物理口的online
                    info.SetIsOnline(nIsOnline);
                    info.SetIsConsistency(nIsOnline);
                    info.SetPhySriovUuid(strSriovUuid.c_str());
                    info.SetPci(sriovport->_pci.c_str());
                    info.SetVlanNum(sriovport->_vlan_num);

                    nRet = pOper->Add(info);
                    if ( nRet != 0 )
                    {
                        if (m_nSRIOVDbgInf)
                        {
                            info.DbgShow();
                        }
                        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] DoVFPortInfoReport Add  failed \n", info.GetName().c_str());
                        //VNET_ASSERT(0);
                    }
                }

                return 0;
            }

            //已经存在VF
            vector<CDbPortSummary>::iterator it_a = outVInfo.begin();
            vector<CSriovPortReport>::iterator sriovport_a;

            for (; it_a != outVInfo.end(); ++it_a)
            {
                CDbPortSriovVf info;
                info.SetUuid(it_a->GetUuid().c_str());
                nRet = pOper->Query(info);
                if ( nRet != 0 )
                {
                    //VNET_ASSERT(0);
                    VNET_LOG(VNET_LOG_ERROR, "CPortMgr::DoVFPortInfoReport Query failed \n");
                    continue;
                }

                for (sriovport_a = it_phyport->_sriov_port.begin(); sriovport_a != it_phyport->_sriov_port.end(); ++sriovport_a)
                {
                    if (info.GetPci()  == sriovport_a->_pci)
                    {
                        if ((info.GetIsOnline() != nIsOnline) || (info.GetVlanNum() != sriovport_a->_vlan_num))
                        {
                            info.SetIsOnline(nIsOnline);
                            info.SetIsConsistency(nIsOnline);
                            info.SetVlanNum(sriovport_a->_vlan_num);
                            nRet = pOper->Modify(info);
                            if ( nRet != 0 )
                            {
                                if (m_nSRIOVDbgInf)
                                {
                                    info.DbgShow();
                                }
                                VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] DoVFPortInfoReport Modify failed \n", info.GetName().c_str());
                                //VNET_ASSERT(0);
                            }
                        }

                        break;
                    }
                }

                if (sriovport_a == it_phyport->_sriov_port.end())
                {
                    info.SetIsOnline(0);
                    info.SetIsConsistency(0);
                    nRet = pOper->Modify(info);
                    if ( nRet != 0 )
                    {
                        if (m_nSRIOVDbgInf)
                        {
                            info.DbgShow();
                        }
                        //VNET_ASSERT(0);
                        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::DoVFPortInfoReport Modify failed");
                    }
                }
            }

            vector<CDbPortSummary>::iterator it;
            vector<CSriovPortReport>::iterator it_sriovport = it_phyport->_sriov_port.begin();
            for (; it_sriovport != it_phyport->_sriov_port.end(); ++it_sriovport)
            {
                for (it = outVInfo.begin(); it != outVInfo.end(); ++it)
                {
                    CDbPortSriovVf info;
                    info.SetUuid(it->GetUuid().c_str());
                    nRet = pOper->Query(info);
                    if ( nRet != 0 )
                    {
                        //VNET_ASSERT(0);
                        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::DoVFPortInfoReport Query failed");
                        continue;
                    }

                    if (info.GetPci() == it_sriovport->_pci)
                    {
                        break;
                    }
                }

                if (it == outVInfo.end())
                {
                    CDbPortSriovVf info;
                    info.SetVnaUuid(strVnaUuid.c_str());
                    info.SetPortType(nPorType);
                    info.SetName((strPortName + it_sriovport->_pci).c_str());
                    info.SetIsOnline(nIsOnline);
                    info.SetIsConsistency(nIsOnline);
                    info.SetPhySriovUuid(strSriovUuid.c_str());
                    info.SetPci(it_sriovport->_pci.c_str());
                    info.SetVlanNum(it_sriovport->_vlan_num);

                    nRet = pOper->Add(info);
                    if ( nRet != 0 )
                    {
                        if (m_nSRIOVDbgInf)
                        {
                            info.DbgShow();
                        }
                        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] DoVFPortInfoReport Add failed", info.GetName().c_str());
                        //VNET_ASSERT(0);
                    }
                }
            }

            return 0;
        }
    }

    //对应的物理端口都没找到，把每个vf online置成0
    vector<CDbPortSummary>::iterator it_null = outVInfo.begin();
    for (; it_null != outVInfo.end(); ++it_null)
    {
        CDbPortSriovVf info;
        info.SetUuid(it_null->GetUuid().c_str());
        nRet = pOper->Query(info);
        if ( nRet != 0 )
        {
            VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] DoVFPortInfoReport Query failed \n", info.GetName().c_str());
            //VNET_ASSERT(0);
            continue;
        }

        info.SetIsOnline(0);
        info.SetIsConsistency(0);
        nRet = pOper->Modify(info);
        if ( nRet != 0 )
        {
            if (m_nSRIOVDbgInf)
            {
                info.DbgShow();
            }
            VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] DoVFPortInfoReport Modify failed \n", info.GetName().c_str());
            //VNET_ASSERT(0);
        }
    }

    return 0;
}

int32 CPortMgr::DoUplinkLoopPortInfoReport(CMessageVNAPortInfoReport &msg)
{
    string  vna_uuid;
    int32  ret;

    vna_uuid = msg._vna_id;

    CDBOperatePortUplinkLoop * pOper = GetDbIPortUplinkLoop();
    if ( NULL == pOper )
    {
        //VNET_ASSERT(0);
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::DoUplinkLoopPortInfoReport GetDbIPortPhy failed \n");
        return -1;
    }

    vector<CDbPortSummary> outVInfo;
    ret = pOper->QuerySummary(vna_uuid.c_str(), outVInfo);
    if ( ret != 0 )
    {
        //VNET_ASSERT(0);
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::DoUplinkLoopPortInfoReport [%s] QuerySummary failed \n", vna_uuid.c_str());
        return -1;
    }

    if (msg._uplink_info.empty())
    {
        if (outVInfo.empty())
        {
            return 0;
        }

        vector<CDbPortSummary>::iterator it_null = outVInfo.begin();
        for (; it_null != outVInfo.end(); ++it_null)
        {
            CDbPortUplinkLoop info;
            info.SetUuid(it_null->GetUuid().c_str());
            ret = pOper->Query(info);
            if ( ret != 0 )
            {
                if (m_nUplinkLpDbgInf)
                {
                    info.DbgShow();
                }
                //VNET_ASSERT(0);
                VNET_LOG(VNET_LOG_ERROR, "CPortMgr::DoUplinkLoopPortInfoReport Query failed \n");
                continue;
            }

            info.SetIsOnline(0);
            info.SetIsConsistency(0);
            ret = pOper->Modify(info);
            if ( ret != 0 )
            {
                if (m_nUplinkLpDbgInf)
                {
                    info.DbgShow();
                }
                //VNET_ASSERT(0);
                VNET_LOG(VNET_LOG_ERROR, "CPortMgr::DoUplinkLoopPortInfoReport Modify failed \n");
            }
        }

        return 0;
    }

    //执行modfiy或者del操作
    vector<CDbPortSummary>::iterator it_a = outVInfo.begin();
    vector<CUplinkLoopPortReport>::iterator it_loop_a;
    for (; it_a != outVInfo.end(); ++it_a)
    {
        for (it_loop_a = msg._uplink_info.begin(); it_loop_a != msg._uplink_info.end(); ++it_loop_a)
        {
            if ((!(it_a->GetName().compare(it_loop_a->_basic_info._name))))
            {
                CDbPortUplinkLoop info;
                info.SetUuid(it_a->GetUuid().c_str());
                ret = pOper->Query(info);
                if ( ret != 0 )
                {
                    //VNET_ASSERT(0);
                    VNET_LOG(VNET_LOG_ERROR, "CPortMgr::DoUplinkLoopPortInfoReport Query failed");
                    break;
                }

                if (info.GetIsPortCfg())
                {
                    //首先比较可配置的属性
                    if (!CmpUplinkLoopPortCfgInfo(info, *it_loop_a))
                    {
                        info.SetIsConsistency(0);
                        ret = pOper->Modify(info);
                        if ( ret != 0 )
                        {
                            if (m_nUplinkLpDbgInf)
                            {
                                info.DbgShow();
                            }
                            //VNET_ASSERT(0);
                            VNET_LOG(VNET_LOG_ERROR, "CPortMgr::DoUplinkLoopPortInfoReport Modify failed");
                        }
                    }
                    else
                    {
                        if (!(info.GetIsConsistency()))
                        {
                            //此处设置cfg，考虑短暂离线，cfg与consistent绑定设置
                            info.SetIsPortCfg(1);
                            info.SetIsConsistency(1);
                            ret = pOper->Modify(info);
                            if ( ret != 0 )
                            {
                                if (m_nUplinkLpDbgInf)
                                {
                                    info.DbgShow();
                                }
                                //VNET_ASSERT(0);
                                VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] DoUplinkLoopPortInfoReport Modify failed \n", info.GetName().c_str());
                            }
                        }
                    }
                }

                //再比较没有配置的属性
                if (!CmpUplinkLoopPortInfo(info, *it_loop_a))
                {
                    SetUplinkLoopPortInfo(vna_uuid, info, *it_loop_a);
                    ret = pOper->Modify(info);
                    if ( ret != 0 )
                    {
                        //VNET_ASSERT(0);
                        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] DoUplinkLoopPortInfoReport Modify failed \n", info.GetName().c_str());
                    }
                }

                //最后判断是否复活
                if (!(info.GetIsOnline()))
                {
                    info.SetIsOnline(1);
                    info.SetIsConsistency(1);
                    ret = pOper->Modify(info);
                    if ( ret != 0 )
                    {
                        if (m_nUplinkLpDbgInf)
                        {
                            info.DbgShow();
                        }
                        //VNET_ASSERT(0);
                        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] DoUplinkLoopPortInfoReport Modify failed \n", info.GetName().c_str());
                    }
                }

                //modify ip/mask
                //ModPortIp(info.GetUuid(), info.GetName(), (*it_a)._basic_info._ip, (*it_a)._basic_info._mask, info.GetIsPortCfg(), info.GetIsOnline());

                break;
            }
        }

        if (it_loop_a == msg._uplink_info.end())
        {
            CDbPortUplinkLoop info;
            info.SetUuid(it_a->GetUuid().c_str());
            ret = pOper->Query(info);
            if ( ret != 0 )
            {
                if (m_nUplinkLpDbgInf)
                {
                    info.DbgShow();
                }
                //VNET_ASSERT(0);
                VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] DoUplinkLoopPortInfoReport Query failed \n", info.GetName().c_str());
            }

            //此条记录不存在了
            info.SetIsOnline(0);
            info.SetIsConsistency(0);
            ret = pOper->Modify(info);
            if ( ret != 0 )
            {
                if (m_nUplinkLpDbgInf)
                {
                    info.DbgShow();
                }
                //VNET_ASSERT(0);
                VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] DoUplinkLoopPortInfoReport Modify failed \n", info.GetName().c_str());
            }

            //del ip/mask
            //DelPortIp(it_a->GetUuid(), it_a->GetName());
        }
    }

    return 0;
}

int32 CPortMgr::DoKernelPortInfoReport(CMessageVNAPortInfoReport &msg)
{
    int32  ret;
    string  vna_uuid;

    //和交换接口入参
    string strBond;
    vector<string> vecPort;

    string strPgId = "";
    string strSwId = "";
    string strPortId = "";

    vna_uuid = msg._vna_id;

    CDBOperateKernelReport * pOper = GetDbIKernelReport();
    if ( NULL == pOper )
    {
        //VNET_ASSERT(0);
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::DoKernelPortInfoReport GetDbIKernelReport failed \n");
        return -1;
    }

    //判断的是上行口
    if ("" == msg._kernel_port._name)
    {
        //为空直接返回
        return 0;
    }

    //安装的kernel采用默认PG
    CPortGroupMgr *pPgHandle = CPortGroupMgr::GetInstance();
    if (pPgHandle)
    {
        if (0 != pPgHandle->QueryPGByName("DEFAULT_KERNEL_PG", strPgId))
        {
            //VNET_ASSERT(0);
            VNET_LOG(VNET_LOG_ERROR, "CPortMgr::DoKernelPortInfoReport QueryPGByName failed \n");
            return -1;
        }
    }

    //phyport
    if (0 == msg._kernel_port._type)
    {
        strBond = "";
        vecPort.push_back(msg._kernel_port._name);
    }

    //bondport
    if (1 == msg._kernel_port._type)
    {
        strBond = msg._kernel_port._name;
    }

    CDbKernelReport info;
    info.SetVnaUuid(vna_uuid.c_str());
    ret = pOper->QueryByVna(info);
    if ( ret != 0 )
    {
        //VNET_ASSERT(0);
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::DoKernelPortInfoReport [%s] QueryByVna failed \n", vna_uuid.c_str());
        return -1;
    }

    //为空直接添加
    if ("" == info.GetUplinkPortName())
    {
        CDbKernelReport info;
        info.SetVnaUuid(vna_uuid.c_str());
        info.SetSwitchName(msg._kernel_port._switch_name.c_str());
        info.SetIp(msg._kernel_port._ip.c_str());
        info.SetMask(msg._kernel_port._mask.c_str());
        info.SetKernelPortName((msg._kernel_port._krport_name).c_str());
        info.SetUplinkPortName((msg._kernel_port._name).c_str());
        info.SetUplinkPortType(msg._kernel_port._type);
        info.SetIsOnline(1);
        info.SetBondMode(msg._kernel_port._bond_mode);

        ret = pOper->Add(info);
        if ( ret != 0 )
        {
            if (m_nKrPortDbgInf)
            {
                info.DbgShow();
            }
            //VNET_ASSERT(0);
            VNET_LOG(VNET_LOG_ERROR, "CPortMgr::DoKernelPortInfoReport [%s  %s] Add failed \n", vna_uuid.c_str(), (msg._kernel_port._name).c_str());
            return -1;
        }

        //反查uuid
        CDbKernelReport info1;
        info1.SetVnaUuid(vna_uuid.c_str());
        ret = pOper->QueryByVna(info1);
        if (0 != ret)
        {
            if (m_nKrPortDbgInf)
            {
                info1.DbgShow();
            }
            //VNET_ASSERT(0);
            VNET_LOG(VNET_LOG_ERROR, "CPortMgr::DoKernelPortInfoReport [%s] QueryByVna failed \n", vna_uuid.c_str());
            return -1;
        }

        //如果是bond，继续添加bondmap
        if (msg._kernel_port._type == 1)
        {
            vector<string>::iterator it = msg._kernel_port._bond_map.begin();
            for (; it != msg._kernel_port._bond_map.end(); ++it)
            {
                ret = pOper->AddBondMap(info1.GetUuid().c_str(), (*it).c_str());
                if ( ret != 0 )
                {
                    if (m_nKrPortDbgInf)
                    {
                        info1.DbgShow();
                    }
                    VNET_LOG(VNET_LOG_ERROR, "CPortMgr::DoKernelPortInfoReport [%s %s] AddBondMap failed \n", info1.GetUplinkPortName().c_str(), (*it).c_str());
                    //VNET_ASSERT(0);
                }
                else
                {
                    vecPort.push_back(*it);
                }
            }
        }

        CSwitchMgr *pSwHandle = CSwitchMgr::GetInstance();
        if (pSwHandle)
        {
            pSwHandle->DoKernelDVSCfg(vna_uuid, msg._kernel_port._switch_name, vecPort, strBond, msg._kernel_port._bond_mode);

            if (0 != pSwHandle->GetSwitchUUID(vna_uuid, msg._kernel_port._switch_name, strSwId))
            {
                //VNET_ASSERT(0);
                VNET_LOG(VNET_LOG_DEBUG, "CPortMgr::DoKernelPortInfoReport [%s] GetSwitchUUID failed \n", msg._kernel_port._switch_name.c_str());
                return -1;
            }
        }

        AddKernelPort(vna_uuid, EN_DB_PORT_TYPE_KERNEL, msg._kernel_port._krport_name, 10, strSwId, strPgId, 0);

        return 0;
    }

    //该vna存在记录，比较和先前不一样的地方
    if ((msg._kernel_port._type != info.GetUplinkPortType()) || (msg._kernel_port._name != info.GetUplinkPortName()))
    {
        //类型不一样或者名称不一致，如果先前是bond由db清理
        CDbKernelReport info2;
        info2.SetVnaUuid(vna_uuid.c_str());
        info2.SetUuid(info.GetUuid().c_str());
        info2.SetSwitchName(msg._kernel_port._switch_name.c_str());
        info2.SetIp(msg._kernel_port._ip.c_str());
        info2.SetMask(msg._kernel_port._mask.c_str());
        info2.SetKernelPortName((msg._kernel_port._krport_name).c_str());
        info2.SetUplinkPortName((msg._kernel_port._name).c_str());
        info2.SetUplinkPortType(msg._kernel_port._type);
        info2.SetBondMode(msg._kernel_port._bond_mode);
        info2.SetIsOnline(1);

        ret = pOper->Modify(info2);
        if ( ret != 0 )
        {
            if (m_nKrPortDbgInf)
            {
                info2.DbgShow();
            }
            VNET_LOG(VNET_LOG_ERROR, "CPortMgr::DoKernelPortInfoReport [%s] Modify failed \n", (msg._kernel_port._name).c_str());
            //VNET_ASSERT(0);
            return -1;
        }

        //如果现在上报的bond写bondmap
        vector<string>::iterator it = msg._kernel_port._bond_map.begin();
        for (; it != msg._kernel_port._bond_map.end(); ++it)
        {
            ret = pOper->AddBondMap(info2.GetUuid().c_str(), (*it).c_str());
            if ( ret != 0 )
            {
                if (m_nKrPortDbgInf)
                {
                    info2.DbgShow();
                }
                //VNET_ASSERT(0);
                VNET_LOG(VNET_LOG_ERROR, "CPortMgr::DoKernelPortInfoReport [%s %s] AddBondMap failed \n", info2.GetUplinkPortName().c_str(), (*it).c_str());
            }
            else
            {
                vecPort.push_back(*it);
            }
        }

        CSwitchMgr *pSwHandle = CSwitchMgr::GetInstance();
        if (pSwHandle)
        {
            pSwHandle->DoKernelDVSCfg(vna_uuid, msg._kernel_port._switch_name, vecPort, strBond, msg._kernel_port._bond_mode);

            if (0 != pSwHandle->GetSwitchUUID(vna_uuid, msg._kernel_port._switch_name, strSwId))
            {
                //VNET_ASSERT(0);
                VNET_LOG(VNET_LOG_DEBUG, "CPortMgr::DoKernelPortInfoReport [%s] GetSwitchUUID failed \n", msg._kernel_port._switch_name.c_str());
                return -1;
            }
        }

        AddKernelPort(vna_uuid, EN_DB_PORT_TYPE_KERNEL, msg._kernel_port._krport_name, 10, strSwId, strPgId, 0);

        return 0;
    }

    //类型和名称是一样的
    CDbKernelReport info3;
    info3.SetVnaUuid(vna_uuid.c_str());
    info3.SetUuid(info.GetUuid().c_str());
    info3.SetSwitchName(msg._kernel_port._switch_name.c_str());
    info3.SetIp(msg._kernel_port._ip.c_str());
    info3.SetMask(msg._kernel_port._mask.c_str());
    info3.SetKernelPortName((msg._kernel_port._krport_name).c_str());
    info3.SetUplinkPortName((msg._kernel_port._name).c_str());
    info3.SetUplinkPortType(msg._kernel_port._type);
    info3.SetBondMode(msg._kernel_port._bond_mode);
    info3.SetIsOnline(1);

    ret = pOper->Modify(info3);
    if ( ret != 0 )
    {
        if (m_nKrPortDbgInf)
        {
            info3.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] DoKernelPortInfoReport Modify failed \n", (msg._kernel_port._name).c_str());
        //VNET_ASSERT(0);
        return -1;
    }

    //如果是bondmap，检查一下
    if (1 == info.GetUplinkPortType())
    {
        vector<CDbKernelReportBondMap> vInfo;
        ret = pOper->QueryBondMapByVna(vna_uuid.c_str(),vInfo);
        if ( ret != 0 )
        {
            return -1;
        }

        if (vInfo.empty())
        {
            vector<string>::iterator it = msg._kernel_port._bond_map.begin();
            for (; it != msg._kernel_port._bond_map.end(); ++it)
            {
                ret = pOper->AddBondMap(info3.GetUuid().c_str(), (*it).c_str());
                if ( ret != 0 )
                {
                    if (m_nKrPortDbgInf)
                    {
                        info3.DbgShow();
                    }
                    VNET_LOG(VNET_LOG_ERROR, "CPortMgr::DoKernelPortInfoReport [%s] AddBondMap failed \n", (*it).c_str());
                    //VNET_ASSERT(0);
                }
                else
                {
                    vecPort.push_back(*it);
                }
            }
        }
        else
        {
            vector<CDbKernelReportBondMap>::iterator it = vInfo.begin();
            vector<string>::iterator it_a;
            for (; it != vInfo.end(); ++it)
            {
                for (it_a = msg._kernel_port._bond_map.begin(); it_a != msg._kernel_port._bond_map.end(); ++it_a)
                {
                    if (it->GetBondPhyName() == (*it_a))
                    {
                        vecPort.push_back(*it_a);
                        break;
                    }
                }

                //不存在删除，map可以删除
                if (it_a == msg._kernel_port._bond_map.end())
                {
                    ret = pOper->DelBondMap(info3.GetUuid().c_str(), (it->GetBondPhyName()).c_str());
                    if ( ret != 0 )
                    {
                        if (m_nKrPortDbgInf)
                        {
                            info3.DbgShow();
                        }
                        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] DoKernelPortInfoReport DelBondMap failed \n", (it->GetBondPhyName()).c_str());
                        //VNET_ASSERT(0);
                    }
                }
            }

            //找到新增的
            vector<string>::iterator it_b = msg._kernel_port._bond_map.begin();
            vector<CDbKernelReportBondMap>::iterator it_c;
            for (it_b = msg._kernel_port._bond_map.begin(); it_b != msg._kernel_port._bond_map.end(); ++it_b)
            {
                for (it_c = vInfo.begin(); it_c != vInfo.end(); ++it_c)
                {
                    if (it_c->GetBondPhyName() == (*it_b))
                    {
                        break;
                    }
                }

                //不存在添加
                if (it_c == vInfo.end())
                {
                    ret = pOper->AddBondMap(info3.GetUuid().c_str(), (*it_b).c_str());
                    if ( ret != 0 )
                    {
                        if (m_nKrPortDbgInf)
                        {
                            info3.DbgShow();
                        }
                        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] DoKernelPortInfoReport AddBondMap failed \n", (*it_b).c_str());
                        //VNET_ASSERT(0);
                    }
                    else
                    {
                        vecPort.push_back(*it_b);
                    }
                }
            }
        }
    }

    CSwitchMgr *pSwHandle = CSwitchMgr::GetInstance();
    if (pSwHandle)
    {
        pSwHandle->DoKernelDVSCfg(vna_uuid, msg._kernel_port._switch_name, vecPort, strBond, msg._kernel_port._bond_mode);

        if (0 != pSwHandle->GetSwitchUUID(vna_uuid, msg._kernel_port._switch_name, strSwId))
        {
            //VNET_ASSERT(0);
            VNET_LOG(VNET_LOG_DEBUG, "CPortMgr::DoKernelPortInfoReport [%s] GetSwitchUUID failed \n", msg._kernel_port._switch_name.c_str());
            return -1;
        }
    }

    AddKernelPort(vna_uuid, EN_DB_PORT_TYPE_KERNEL, msg._kernel_port._krport_name, 10, strSwId, strPgId, 0);

    return 0;
}

int32 CPortMgr::AddPortIp(const string port_uuid, const string port_ip_name, const string ip, const string mask, int32 iscfg, int32 isonline)
{
    int32 ret;

    if ((port_uuid.empty()) || (port_ip_name.empty()))
    {
        //VNET_ASSERT(0);
        return -1;
    }

    CDBOperatePortIp *pOper = GetDbIPortIp();
    if ( NULL == pOper )
    {
        //VNET_ASSERT(0);
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::AddPortIp GetIPortIp failed \n");
        return -1;
    }

    //先查询后添加
    vector<CDbPortIp> outVInfo;
    ret = pOper->QueryPortIp(port_uuid.c_str(), outVInfo);
    if ( ret != 0 )
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] AddPortIp QueryPortIp failed \n", port_uuid.c_str());
        return -1;
    }

    //暂且考虑一个IP
    if (outVInfo.empty())
    {
        //kernel口的dhcp方式，可能传ip/mask为空
        if (("" == ip) || ("" == mask))
        {
            return 0;
        }

        ret = pOper->Add(port_uuid.c_str(), port_ip_name.c_str(), ip.c_str(), mask.c_str(), iscfg, isonline);
        if ( ret != 0 )
        {
            VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s %s %s %s %d %d] AddPortIp Add failed \n",
                     port_uuid.c_str(), port_ip_name.c_str(), ip.c_str(), mask.c_str(), iscfg, isonline);
            //VNET_ASSERT(0);
            return -1;
        }

        return 0;
    }
    else
    {
        /*if (("" == ip) || ("" == mask))
        {
            if (0 != DelPortIp(port_uuid, port_ip_name))
            {
                return -1;
            }
            else
            {
                return 0;
            }
        }*/

        return ModPortIp(port_uuid, port_ip_name, ip, mask, iscfg, isonline);
    }
}

int32 CPortMgr::ModPortIp(string port_uuid, string port_ip_name, string ip, string mask, int32 iscfg, int32 isonline)
{
    int32 ret;

    if ((port_uuid.empty()) || (port_ip_name.empty()))
    {
        //VNET_ASSERT(0);
        return -1;
    }

    CDBOperatePortIp * pOper = GetDbIPortIp();
    if ( NULL == pOper )
    {
        //VNET_ASSERT(0);
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::ModPortIp GetIPortIp failed \n");
        return -1;
    }

    //先查询
    vector<CDbPortIp> outVInfo;
    ret = pOper->QueryPortIp(port_uuid.c_str(), outVInfo);
    if ( ret != 0 )
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] ModPortIp QueryPortIp failed \n", port_uuid.c_str());
        return 0;
    }

    //先前为空，现在有ip和mask，变为添加
    if (outVInfo.empty())
    {
        if (("" == ip) || ("" == mask))
        {
            return 0;
        }
        return AddPortIp(port_uuid.c_str(), port_ip_name.c_str(), ip.c_str(),mask.c_str(), iscfg, isonline);
    }

    if (("" == ip) || ("" == mask))
    {
        if (0 != DelPortIp(port_uuid, port_ip_name))
        {
            return -1;
        }
        else
        {
            return 0;
        }
    }

    //暂且认为只有一个ip
    ret = pOper->Modify(port_uuid.c_str(), port_ip_name.c_str(), ip.c_str(),mask.c_str(), iscfg, isonline);
    if ( ret != 0 )
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s %s %s %s %d %d] ModPortIp Modify failed \n",
                 port_uuid.c_str(), port_ip_name.c_str(), ip.c_str(),mask.c_str(), iscfg, isonline);
        //VNET_ASSERT(0);
        return -1;
    }

    return 0;
}

int32 CPortMgr::DelPortIp(string port_uuid, string port_ip_name)
{
    int32 ret;

    if ((port_uuid.empty()) || (port_ip_name.empty()))
    {
        //VNET_ASSERT(0);
        return -1;
    }

    CDBOperatePortIp * pOper = GetDbIPortIp();
    if ( NULL == pOper )
    {
        //VNET_ASSERT(0);
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::DelPortIp GetIPortIp failed \n");
        return -1;
    }

    //先查询
    vector<CDbPortIp> outVInfo;
    ret = pOper->QueryPortIp(port_uuid.c_str(), outVInfo);
    if ( ret != 0 )
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] ModPortIp QueryPortIp failed \n", port_uuid.c_str());
        return 0;
    }

    //为空直接返回
    if (outVInfo.empty())
    {
        return 0;
    }

    ret = pOper->Del(port_uuid.c_str(), port_ip_name.c_str());
    if ( ret != 0 )
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s  %s]DelPortIp Del failed \n", port_uuid.c_str(), port_ip_name.c_str());
        //VNET_ASSERT(0);
        return -1;
    }

    return 0;
}

//对于配置的部分，只上报变化的部分，即vnm添加db成功才修改
int32 CPortMgr::DoKernelPortCfgInfoReport(CMessageVNAPortInfoReport &msg)
{
    int32  nRet;
    string  strVnaUuid;
    string strSwId = "";

    strVnaUuid = msg._vna_id;

    CDBOperatePortKernel *pOper = GetDbIPortKernel();
    if ( NULL == pOper )
    {
        //VNET_ASSERT(0);
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::DoKernelPortCfgInfoReport GetDbIPortKernel failed [vna_uuid:%s] \n", strVnaUuid.c_str());
        return -1;
    }

    vector<CDbPortSummary> outPortKernel;
    nRet = pOper->QuerySummary(strVnaUuid.c_str(),outPortKernel);
    if (nRet != 0)
    {
        //VNET_ASSERT(0);
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::DoKernelPortCfgInfoReport QuerySummary failed [vna_uuid:%s] \n", strVnaUuid.c_str());
        return -1;
    }

    //没有可比较的，返回
    if (outPortKernel.empty())
    {
        return 0;
    }

    if (msg._kernel_cfgport.empty())
    {
        vector<CDbPortSummary>::iterator it = outPortKernel.begin();
        for (; it != outPortKernel.end(); ++it)
        {
            //先查询，然后修改online
            CDbPortKernel info;
            info.SetUuid(it->GetUuid().c_str());
            nRet = pOper->Query(info);
            if (0 == nRet)
            {
                info.SetIsOnline(0);
                info.SetIsConsistency(0);
                info.SetIsCfg(0);
                nRet = pOper->Modify(info);
                if ( nRet != 0 )
                {
                    info.DbgShow();
                    //VNET_ASSERT(0);
                }
            }
        }

        return 0;
    }

    vector<CDbPortSummary>::iterator it_kernelport;
    vector<CKernelPortCfgReport>::iterator it_tap = msg._kernel_cfgport.begin();
    for (; it_tap != msg._kernel_cfgport.end(); ++it_tap)
    {
        for (it_kernelport = outPortKernel.begin(); it_kernelport != outPortKernel.end(); ++it_kernelport)
        {
            if (it_tap->_krport_name == it_kernelport->GetName())
            {
                //先查询，然后修改online
                CDbPortKernel info;
                info.SetUuid(it_kernelport->GetUuid().c_str());
                nRet = pOper->Query(info);
                if (0 == nRet)
                {
                    info.SetIsOnline(1);
                    info.SetIsCfg(1);

                    if (info.GetIsPortCfg())
                    {
                        info.SetIsConsistency(1);
                        info.SetIsPortCfg(1);
                    }

                    info.SetState(it_tap->_basic_info._state);

                    CSwitchMgr *pSwHandle = CSwitchMgr::GetInstance();
                    if (pSwHandle)
                    {
                        strSwId = "";
                        if (0 != pSwHandle->GetSwitchUUID(strVnaUuid, it_tap->_switch_name, strSwId))
                        {
                            VNET_LOG(VNET_LOG_ERROR, "CPortMgr::DoKernelPortCfgInfoReport GetSwitchUUID failed [%s %s] \n", it_tap->_switch_name.c_str(),it_tap->_krport_name.c_str());
                        }
                    }

                    if (strSwId.empty())
                    {
                        info.SetIsCfgVswitch(0);
                    }
                    else
                    {
                        info.SetIsCfgVswitch(1);
                        info.SetVswitchUuid(strSwId.c_str());
                    }

                    nRet = pOper->Modify(info);
                    if ( nRet != 0 )
                    {
                        info.DbgShow();
                        //VNET_ASSERT(0);
                    }

                    ModPortIp(info.GetUuid(), it_tap->_krport_name, it_tap->_ip, it_tap->_mask, info.GetIsPortCfg(), 1);
                }
                else
                {
                    info.DbgShow();
                    //VNET_ASSERT(0);
                }

                break;
            }
        }
    }

    //反过来设置离线
    vector<CKernelPortCfgReport>::iterator it_report;
    vector<CDbPortSummary>::iterator it = outPortKernel.begin();
    for (; it != outPortKernel.end(); ++it)
    {
        for (it_report = msg._kernel_cfgport.begin(); it_report != msg._kernel_cfgport.end(); ++it_report)
        {
            if (it_report->_krport_name  == it->GetName())
            {
                break;
            }
        }

        if (it_report == msg._kernel_cfgport.end())
        {
            //先查询，然后修改online
            CDbPortKernel info;
            info.SetUuid(it->GetUuid().c_str());
            nRet = pOper->Query(info);
            if (0 == nRet)
            {
                info.SetIsOnline(0);
                info.SetIsConsistency(0);
                info.SetIsCfg(0);
                nRet = pOper->Modify(info);
                if ( nRet != 0 )
                {
                    info.DbgShow();
                    //VNET_ASSERT(0);
                }
            }
            else
            {
                info.DbgShow();
                //VNET_ASSERT(0);
            }
        }
    }

    return 0;
}

//对于配置的部分，只上报变化的部分，即vnm添加db成功才修改
int32 CPortMgr::DoVtepPortCfgInfoReport(CMessageVNAPortInfoReport &msg)
{
    int32  nRet;
    string  strVnaUuid;
    string strSwId = "";

    strVnaUuid = msg._vna_id;

    CDBOperatePortVtep *pOper = GetDbIPortVtep();
    if ( NULL == pOper )
    {
        //VNET_ASSERT(0);
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::DoVtepPortCfgInfoReport GetDbIPortVtep failed [vna_uuid:%s] \n", strVnaUuid.c_str());
        return -1;
    }

    vector<CDbPortSummary> outPortVtep;
    nRet = pOper->QuerySummary(strVnaUuid.c_str(),outPortVtep);
    if (nRet != 0)
    {
        //VNET_ASSERT(0);
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::DoVtepPortCfgInfoReport QuerySummary failed [vna_uuid:%s] \n", strVnaUuid.c_str());
        return -1;
    }

    //没有可比较的，返回
    if (outPortVtep.empty())
    {
        return 0;
    }

    if (msg._vtep_cfgport.empty())
    {
        vector<CDbPortSummary>::iterator it = outPortVtep.begin();
        for (; it != outPortVtep.end(); ++it)
        {
            //先查询，然后修改online
            CDbPortVtep info;
            info.SetUuid(it->GetUuid().c_str());
            nRet = pOper->Query(info);
            if (0 == nRet)
            {
                info.SetIsOnline(0);
                info.SetIsConsistency(0);
                nRet = pOper->Modify(info);
                if ( nRet != 0 )
                {
                    info.DbgShow();
                }
            }
        }

        return 0;
    }

    vector<CDbPortSummary>::iterator it_vtepport;
    vector<CVtepPortCfgReport>::iterator it_tap = msg._vtep_cfgport.begin();
    for (; it_tap != msg._vtep_cfgport.end(); ++it_tap)
    {
        for (it_vtepport = outPortVtep.begin(); it_vtepport != outPortVtep.end(); ++it_vtepport)
        {
            if (it_tap->_vtep_name == it_vtepport->GetName())
            {
                //先查询，然后修改online
                CDbPortVtep info;
                info.SetUuid(it_vtepport->GetUuid().c_str());
                nRet = pOper->Query(info);
                if (0 == nRet)
                {
                    info.SetIsOnline(1);

                    if (info.GetIsPortCfg())
                    {
                        info.SetIsConsistency(1);
                        info.SetIsPortCfg(1);
                    }

                    info.SetState(it_tap->_basic_info._state);

                    CSwitchMgr *pSwHandle = CSwitchMgr::GetInstance();
                    if (pSwHandle)
                    {
                        strSwId = "";
                        if (0 != pSwHandle->GetSwitchUUID(strVnaUuid, it_tap->_switch_name, strSwId))
                        {
                            VNET_LOG(VNET_LOG_ERROR, "CPortMgr::DoKernelPortCfgInfoReport GetSwitchUUID failed [%s %s] \n", it_tap->_switch_name.c_str(),it_tap->_vtep_name.c_str());
                        }
                    }

                    if (strSwId.empty())
                    {
                        info.SetVswitchUuid("");
                    }
                    else
                    {
                        info.SetVswitchUuid(strSwId.c_str());
                    }

                    nRet = pOper->Modify(info);
                    if ( nRet != 0 )
                    {
                        info.DbgShow();
                    }

                    //ModPortIp(info.GetUuid(), it_tap->_vtep_name, it_tap->_ip, it_tap->_mask, info.GetIsPortCfg(), 1);
                }
                else
                {
                    info.DbgShow();
                }

                break;
            }
        }
    }

    //反过来设置离线
    vector<CVtepPortCfgReport>::iterator it_report;
    vector<CDbPortSummary>::iterator it = outPortVtep.begin();
    for (; it != outPortVtep.end(); ++it)
    {
        for (it_report = msg._vtep_cfgport.begin(); it_report != msg._vtep_cfgport.end(); ++it_report)
        {
            if (it_report->_vtep_name  == it->GetName())
            {
                break;
            }
        }

        if (it_report == msg._vtep_cfgport.end())
        {
            //先查询，然后修改online
            CDbPortVtep info;
            info.SetUuid(it->GetUuid().c_str());
            nRet = pOper->Query(info);
            if (0 == nRet)
            {
                info.SetIsOnline(0);
                info.SetIsConsistency(0);
                nRet = pOper->Modify(info);
                if ( nRet != 0 )
                {
                    info.DbgShow();
                }
            }
            else
            {
                info.DbgShow();
            }
        }
    }

    return 0;
}

//add vtep to db
int32 CPortMgr::AddVtepPort(const string strVnaUuid, const string strVtepName, const string strSwitchUuid, const string strIp, const string strMask)
{
    int32 ret;

    if ((strVnaUuid.empty()) || (strVtepName.empty()))
    {
        //VNET_ASSERT(0);
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::AddVtepPort Input empty \n");
        return -1;
    }

    CDBOperatePortVtep * pOper = GetDbIPortVtep();
    if ( NULL == pOper )
    {
        //VNET_ASSERT(0);
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::AddVtepPort GetDbIPortVtep failed \n");
        return -1;
    }

    vector<CDbPortSummary> outVInfo;
    ret = pOper->QuerySummary(strVnaUuid.c_str(),outVInfo);
    if (ret != 0)
    {
        //VNET_ASSERT(0);
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] AddVtepPort QuerySummary failed \n", strVnaUuid.c_str());
        return -1;
    }

    if (TRUE != outVInfo.empty())
    {
        vector<CDbPortSummary>::iterator it = outVInfo.begin();
        for (; it != outVInfo.end(); ++it)
        {
            //例如上报先于应答写DB
            if (it->GetName() == strVtepName)
            {
                CDbPortVtep info;
                info.SetUuid(it->GetUuid().c_str());
                ret = pOper->Query(info);
                if (0 == ret)
                {
                    info.SetIsOnline(1);
                    info.SetIsPortCfg(1);
                    info.SetIsConsistency(1);

                    info.SetPortType(PORT_TYPE_VTEP);
                    info.SetVswitchUuid(strSwitchUuid.c_str());

                    ret = pOper->Modify(info);
                    if ( ret != 0 )
                    {
                        info.DbgShow();
                        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[vna_uuid:%s name:%s] AddVtepPort Modify failed \n", strVnaUuid.c_str(), strVtepName.c_str());
                        return -1;
                    }

                    return 0;
                }
                else
                {
                    info.DbgShow();
                    return -1;
                }
            }
        }
    }

    CDbPortVtep info;
    info.SetVnaUuid(strVnaUuid.c_str());
    info.SetPortType(PORT_TYPE_VTEP);
    info.SetName(strVtepName.c_str());
    
    info.SetIp(strIp.c_str());
    info.SetMask(strMask.c_str());

    TIPv4Addr tItem;
    CIPv4Addr cIPv4Addr;
    if (0 != cIPv4Addr.Set(strIp, strMask))
    {
        VNET_LOG(VNET_LOG_INFO, "CPortMgr::[vna_uuid:%s name:%s] AddVtepPort  cIPv4Addr.Set  failed \n", 
             strIp.c_str(), strMask.c_str());
        return -1;
    }
    cIPv4Addr.Get(tItem);
    
    info.SetIpNum((int64)CIPv4Addr::N2HL(tItem.uIP.dwAddr));
    info.SetMaskNum((int64)CIPv4Addr::N2HL(tItem.uMask.dwAddr));
    
    info.SetIsOnline(0);
    info.SetIsConsistency(0);
    info.SetIsPortCfg(1);

    info.SetVswitchUuid(strSwitchUuid.c_str());

    ret = pOper->Add(info);
    if ( ret != 0 )
    {
        info.DbgShow();
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[vna_uuid:%s name:%s] AddVtepPort Add failed \n", strVnaUuid.c_str(), strVtepName.c_str());
        return -1;
    }

    return 0;
}

//del vtep to db
int32 CPortMgr::DelVtepPort(const string &strVnaUuid, const string strVtepName)
{
    int32 ret;
    CDbPortVtep info;

    ret = GetDbPortVtep(strVnaUuid, strVtepName, info);
    if (-1==ret)
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::DelVtepPort[vna_uuid:%s name:%s]] pOper->Query failed \n", 
            strVnaUuid.c_str(), strVtepName.c_str());
        return -1;
    }

    if (-2==ret)
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::DelVtepPort[vna_uuid:%s name:%s]] ok! \n", 
            strVnaUuid.c_str(), strVtepName.c_str());
        return 0;
    }

    /*释放vtep申请的ip地址*/
    CVxlanMgr *pVxlanMgr = CVxlanMgr::GetInstance();
    if (pVxlanMgr)
    {
        string strNetplaneUUID; 
        strNetplaneUUID = pVxlanMgr->GetNetplaneIdByVtep(strVtepName);
        if (!strNetplaneUUID.empty())
        {
            pVxlanMgr->FreeVtepIP(strNetplaneUUID, info.GetIp(), info.GetMask());
        }
    }

    CDBOperatePortVtep * pOper = GetDbIPortVtep();
    if ( NULL == pOper )
    {
        //VNET_ASSERT(0);
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::DelVtepPort GetDbIPortVtep failed \n");
        return -1;
    }
    
    ret = pOper->Del((info.GetUuid()).c_str());
    if ( ret != 0 )
    {
        //VNET_ASSERT(0);
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::DelVtepPort[vna_uuid:%s name:%s]] Del failed \n", 
            strVnaUuid.c_str(), strVtepName.c_str());
        return -1;
    }
                
    if (pVxlanMgr)
    {
        pVxlanMgr->SendMcGroupInfo(strVnaUuid);
    }

    return 0;
}

int32 CPortMgr::QueryVtepInfo(const string strVnaUuid, const string strSwitchUuid, string &strVtepName, string &strIp, string &strMask)
{
    CVxlanMgr *pVxlanMgr = CVxlanMgr::GetInstance();
    if (!pVxlanMgr)
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] QueryVtepInfo failed, pVxlanMgr == NULL \n", strVnaUuid.c_str());
        return -1;
    }
    
    strVtepName = pVxlanMgr->GetVtepNameBySwitchUuid(strSwitchUuid);
    if (""==strVtepName)
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] QueryVtepInfo pVxlanMgr->GetVtepNameBySwitchUuid failed, strVtepName == "" \n", 
            strVnaUuid.c_str());
        return -1;
    }

    /*从数据库中查找，找到直接返回*/
    CDbPortVtep DbPortVtep;
    int32 ret;
    if (0!=(ret=GetDbPortVtep(strVnaUuid, strVtepName, DbPortVtep)))
    {
        if (-1==ret)
        {
            VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] QueryVtepInfo GetDbPortVtep failed, strVtepName:%s"" \n", 
                strVnaUuid.c_str(), strVtepName.c_str());
        }
        return -1;
    }
    
    strIp   = DbPortVtep.GetIp();
    strMask = DbPortVtep.GetMask();

    if (m_nKrPortDbgInf)
    {    
        VNET_LOG(VNET_LOG_INFO, "CPortMgr::[%s] QueryVtepInfo success!! strVtepName:%s, strIp:%s, strMask:%s\n", 
            strVnaUuid.c_str(), strVtepName.c_str(), strIp.c_str(), strMask.c_str());   
    }
    
    return 0;
}

int32 CPortMgr::GetVtepInfo(const string strVnaUuid, const string strSwitchUuid, 
                               const string strPgUuid, string &strVtepName, string &strIp, string &strMask)
{        
    /*从数据库中查找，找到直接返回*/
    if (0==QueryVtepInfo(strVnaUuid, strSwitchUuid, strVtepName, strIp, strMask))
    {
        if (m_nKrPortDbgInf)
        {
            VNET_LOG(VNET_LOG_INFO, "CPortMgr::[%s] GetVtepInfo success!! strVtepName:%s, strIp:%s, strMask:%s\n", 
                strVnaUuid.c_str(), strVtepName.c_str(), strIp.c_str(), strMask.c_str());    
        }        
        return 0;   
    }

    /*没找到，获取PG的网络平面id*/
    CPortGroupMgr *pPGMgr = CPortGroupMgr::GetInstance();
    if(NULL == pPGMgr)
    {        
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::GetVtepInfo: pPGMgr is NULL.\n");
        return -1;
    }
    string strNetplaneUUID;
    if(0 != pPGMgr->GetPGNetplane(strPgUuid, strNetplaneUUID))
    {        
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::GetVtepInfo: Call GetPGNetplane(%s) failed.\n", strPgUuid.c_str());            
        return -1;
    }

    /*分配IP*/
    CVxlanMgr *pVxlanMgr = CVxlanMgr::GetInstance();
    if (!pVxlanMgr)
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] GetVtepInfo failed, pVxlanMgr == NULL \n", strVnaUuid.c_str());
        return -1;
    }    
    if (0!=pVxlanMgr->AllocVtepIP(strNetplaneUUID, strIp, strMask))
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] pVxlanMgr->AllocVtepIP failed! \n", strVnaUuid.c_str());        
        return -1;
    }

    /*写库*/
    if (0!=AddVtepPort(strVnaUuid, strVtepName, strSwitchUuid, strIp, strMask))
    {
        /*回滚*/
        if (0!=pVxlanMgr->FreeVtepIP(strNetplaneUUID, strIp, strMask))
        {
            VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] pVxlanMgr->FreeVtepIP failed! \n", strVnaUuid.c_str());        
        }
        
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::GetVtepInfo[%s] AddVtepPort fail!! strVtepName:%s, strIp:%s, strMask:%s\n", 
            strVnaUuid.c_str(), strVtepName.c_str(), strIp.c_str(), strMask.c_str());    
        return -1;
    }

    if (m_nKrPortDbgInf)
    {    
        VNET_LOG(VNET_LOG_INFO, "CPortMgr::[%s] GetVtepInfo success!! strVtepName:%s, strIp:%s, strMask:%s\n", 
            strVnaUuid.c_str(), strVtepName.c_str(), strIp.c_str(), strMask.c_str());    
    }
    
    return 0;   

}


int32 CPortMgr::SetVtepCfg(const string strVnaUuid, const string strVtepName)
{
    int32 ret;
    CDbPortVtep info;
    
    if (0!=GetDbPortVtep(strVnaUuid, strVtepName, info))
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::SetVtepCfg[vna_uuid:%s name:%s]] pOper->Query failed \n", 
            strVnaUuid.c_str(), strVtepName.c_str());
        return -1;                
    }

    /*info.SetIsPortCfg(2);
    if (info.GetIsOnline())
    {
        info.SetIsConsistency(1);
    }*/

    info.SetIsOnline(0);
    info.SetIsConsistency(0);
    info.SetIsPortCfg(1);

    CDBOperatePortVtep * pOper = GetDbIPortVtep();
    if ( NULL == pOper )
    {
        //VNET_ASSERT(0);
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::SetVtepCfg GetDbIPortVtep failed \n");
        return -1;
    }    

    ret = pOper->Modify(info);
    if ( ret != 0 )
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] SetVtepCfg Modify failed for Cfg \n", strVtepName.c_str());
        //VNET_ASSERT(0);
        return -1;
    }

    return 0;
}

int32 CPortMgr::GetDbPortVtep(const string strVnaUuid, const string strVtepName, CDbPortVtep &DbPortVtep)
{
    int32 ret;
    
    if ((strVnaUuid.empty()) || (strVtepName.empty()))
    {
        //VNET_ASSERT(0);
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::GetDbPortVtep Input empty \n");
        return -1;
    }

    CDBOperatePortVtep * pOper = GetDbIPortVtep();
    if ( NULL == pOper )
    {
        //VNET_ASSERT(0);
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::GetDbPortVtep GetDbIPortVtep failed \n");
        return -1;
    }

    vector<CDbPortSummary> outVInfo;
    ret = pOper->QuerySummary(strVnaUuid.c_str(),outVInfo);
    if ( ret != 0 )
    {
        //VNET_ASSERT(0);
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] GetDbPortVtep QuerySummary failed \n", strVnaUuid.c_str());
        return -1;
    }

    if (outVInfo.empty())
    {
        return -2;
    }

    vector<CDbPortSummary>::iterator it = outVInfo.begin();
    for (; it != outVInfo.end(); ++it)
    {
        if (strVtepName == it->GetName())
        {
            break;
        }
    }

    if (it == outVInfo.end())
    {
        return -2;
    }    

    DbPortVtep.SetUuid((it->GetUuid()).c_str());
    ret = pOper->Query(DbPortVtep);
    if ( ret != 0 )
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::GetDbPortVtep[vna_uuid:%s name:%s]] pOper->Query failed \n", 
            strVnaUuid.c_str(), strVtepName.c_str());
        return -1;                
    }
    
    return 0;
}

//添加多个lacp nics
int32 CPortMgr::AddBondLacpMap4Report(string &vna_uuid, string &bond_name, CBondReport &report, CDBOperateBond *pOper)
{
    int32 ret;

    //添加成功的
    vector<string>::iterator it_succ = report._bond_lacp._suc_nics.begin();
    for (; it_succ != report._bond_lacp._suc_nics.end(); ++it_succ)
    {
        //开始添加map表
        CDBBondMap info;
        string bond_uuid = "";
        string phy_uuid = "";

        //uuid获取失败传递到后面操作失败
        pOper->QueryUuid(vna_uuid.c_str(), (*it_succ).c_str(), EN_DB_PORT_TYPE_PHYSICAL, phy_uuid);
        pOper->QueryUuid(vna_uuid.c_str(), (bond_name.c_str()), EN_DB_PORT_TYPE_BOND, bond_uuid);

        info.SetVnaUuid(vna_uuid.c_str());
        info.SetBondUuid(bond_uuid.c_str());
        info.SetPhyUuid(phy_uuid.c_str());
        info.SetPhyName((*it_succ).c_str());
        info.SetBondName(bond_name.c_str());
        info.SetBondMode(EN_BOND_MODE_802);
        info.SetIsCfg(0);
        info.SetIsSelect(1);
        ret = pOper->AddMap(info);
        if ( ret != 0 )
        {
            if (m_nBondDbgInf)
            {
                info.DbgShow();
            }
            //VNET_ASSERT(0);
            VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s  %s] AddBodLacpMap4Report add succ lacp failed \n", (bond_name).c_str(), (*it_succ).c_str());
        }
        else
        {
            pOper->SetLacpSuccessNic(bond_uuid.c_str(), phy_uuid.c_str());
        }
    }

    //添加失败的
    vector<string>::iterator it_fail = report._bond_lacp._fail_nics.begin();
    for (; it_fail != report._bond_lacp._fail_nics.end(); ++it_fail)
    {
        //开始添加map表
        CDBBondMap info;
        string bond_uuid = "";
        string phy_uuid = "";

        //uuid获取失败传递到后面操作失败
        pOper->QueryUuid(vna_uuid.c_str(), (*it_fail).c_str(), EN_DB_PORT_TYPE_PHYSICAL, phy_uuid);
        pOper->QueryUuid(vna_uuid.c_str(), (bond_name).c_str(), EN_DB_PORT_TYPE_BOND, bond_uuid);

        info.SetVnaUuid(vna_uuid.c_str());
        info.SetBondUuid(bond_uuid.c_str());
        info.SetPhyUuid(phy_uuid.c_str());
        info.SetPhyName((*it_fail).c_str());
        info.SetBondName(bond_name.c_str());
        info.SetBondMode(EN_BOND_MODE_802);
        info.SetIsCfg(0);
        info.SetIsSelect(1);
        ret = pOper->AddMap(info);
        if ( ret != 0 )
        {
            if (m_nBondDbgInf)
            {
                info.DbgShow();
            }
            //VNET_ASSERT(0);
            VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s   %s] AddBodLacpMap4Report add fail lacp failed for null \n", (bond_name).c_str(), (*it_fail).c_str());
        }
        else
        {
            pOper->UnsetLacpSuccessNic(bond_uuid.c_str(), phy_uuid.c_str());
        }
    }

    return 0;
}

int32 CPortMgr::AddBondBackupMap4Report(string &vna_uuid, string &bond_name, CBondReport &report, CDBOperateBond *pOper)
{
    int32 ret;

    //开始添加map表，添加active
    CDBBondMap info;
    string bond_uuid = "";
    string phy_uuid = "";

    //2013.05.20 modify none problem
    if ("None" != report._bond_backup._active_nic)
    {
        pOper->QueryUuid(vna_uuid.c_str(), (report._bond_backup._active_nic).c_str(), EN_DB_PORT_TYPE_PHYSICAL, phy_uuid);
        pOper->QueryUuid(vna_uuid.c_str(), (bond_name).c_str(), EN_DB_PORT_TYPE_BOND, bond_uuid);

        info.SetVnaUuid(vna_uuid.c_str());
        info.SetBondUuid(bond_uuid.c_str());
        info.SetPhyUuid(phy_uuid.c_str());
        info.SetPhyName((report._bond_backup._active_nic).c_str());
        info.SetBondName(bond_name.c_str());
        info.SetBondMode(EN_BOND_MODE_BACKUP);
        info.SetIsCfg(0);
        info.SetIsSelect(1);
        ret = pOper->AddMap(info);
        if ( ret == 0 )
        {
            pOper->SetBackupActiveNic(bond_uuid.c_str(), phy_uuid.c_str());
        }
        else
        {
            if (m_nBondDbgInf)
            {
                info.DbgShow();
            }
            //VNET_ASSERT(0);
            VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s   %s] AddBondBackupMap4Report add active backup failed \n", (bond_name).c_str(), (report._bond_backup._active_nic).c_str());
        }

    }

    //添加其他的
    vector<string>::iterator it_other = report._bond_backup._other_nic.begin();
    for (; it_other != report._bond_backup._other_nic.end(); ++it_other)
    {
        CDBBondMap info;
        string bond_uuid = "";
        string phy_uuid = "";

        pOper->QueryUuid(vna_uuid.c_str(), (*it_other).c_str(), EN_DB_PORT_TYPE_PHYSICAL, phy_uuid);
        pOper->QueryUuid(vna_uuid.c_str(), (bond_name).c_str(), EN_DB_PORT_TYPE_BOND, bond_uuid);

        info.SetVnaUuid(vna_uuid.c_str());
        info.SetBondUuid(bond_uuid.c_str());
        info.SetPhyUuid(phy_uuid.c_str());
        info.SetPhyName((*it_other).c_str());
        info.SetBondName(bond_name.c_str());
        info.SetBondMode(EN_BOND_MODE_BACKUP);
        info.SetIsCfg(0);
        info.SetIsSelect(1);
        ret = pOper->AddMap(info);
        if ( ret == 0 )
        {
            pOper->UnsetBackupActiveNic(bond_uuid.c_str(), phy_uuid.c_str());
        }
        else
        {
            if (m_nBondDbgInf)
            {
                info.DbgShow();
            }
            //VNET_ASSERT(0);
            VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s   %s]AddBondBackupMap4Report add other backup failed \n", (bond_name).c_str(), (*it_other).c_str());
        }
    }

    return 0;
}

int32 CPortMgr::CompBondLacpMap4Report(string &vna_uuid, string &bond_name, vector<CDBBondMap> vBondMap, CBondReport &report, CDBOperateBond *pOper)
{
    int32 ret;

    vector<CDBBondMap>::iterator it = vBondMap.begin();
    for (; it != vBondMap.end(); ++it)
    {
        vector<string>::iterator it_succ;
        for ( it_succ = report._bond_lacp._suc_nics.begin(); it_succ !=  report._bond_lacp._suc_nics.end(); ++it_succ)
        {
            if (it->GetPhyName() == (*it_succ))
            {
                if (it->GetLacpIsSuc() != 1)
                {
                    pOper->SetLacpSuccessNic((it->GetBondUuid()).c_str(), (it->GetPhyUuid()).c_str());
                }

                //it->SetIsCfg(1);
                it->SetIsSelect(1);
                ret = pOper->ModifyMap(*it);
                if ( ret != 0 )
                {
                    if (m_nBondDbgInf)
                    {
                        (*it).DbgShow();
                    }
                }
                
                break;
            }
        }

        vector<string>::iterator it_fail;
        for ( it_fail = report._bond_lacp._fail_nics.begin(); it_fail !=  report._bond_lacp._fail_nics.end(); ++it_fail)
        {
            if (it->GetPhyName() == (*it_fail))
            {
                if (it->GetLacpIsSuc() != 0)
                {
                    pOper->UnsetLacpSuccessNic((it->GetBondUuid()).c_str(), (it->GetPhyUuid()).c_str());
                }

                //it->SetIsCfg(1);
                it->SetIsSelect(1);
                ret = pOper->ModifyMap(*it);
                if ( ret != 0 )
                {
                    if (m_nBondDbgInf)
                    {
                        (*it).DbgShow();
                    }
                }

                break;
            }
        }

        //未找到相应成员口，删除
        if ((it_succ ==  report._bond_lacp._suc_nics.end()) && (it_fail ==  report._bond_lacp._fail_nics.end()))
        {
            it->SetIsSelect(0);
            ret = pOper->ModifyMap(*it);
            if ( ret != 0 )
            {
                //VNET_ASSERT(0);
                VNET_LOG(VNET_LOG_ERROR, "CPortMgr::CompBondLacpMap4Report Modify BondMap failed for lacp map not online \n");
            }
        }
    }

    //下面开始操作增加的成员口
    vector<string>::iterator it_succ_add;
    for ( it_succ_add = report._bond_lacp._suc_nics.begin(); it_succ_add !=  report._bond_lacp._suc_nics.end(); ++it_succ_add)
    {
        vector<CDBBondMap>::iterator it_map_add;
        for (it_map_add = vBondMap.begin(); it_map_add != vBondMap.end(); ++it_map_add)
        {
            if (*it_succ_add == it_map_add->GetPhyName()) break;
        }

        if (it_map_add == vBondMap.end())
        {
            CDBBondMap info;
            string bond_uuid = "";
            string phy_uuid = "";

            pOper->QueryUuid(vna_uuid.c_str(), (*it_succ_add).c_str(), EN_DB_PORT_TYPE_PHYSICAL, phy_uuid);
            pOper->QueryUuid(vna_uuid.c_str(), (bond_name.c_str()), EN_DB_PORT_TYPE_BOND, bond_uuid);

            info.SetVnaUuid(vna_uuid.c_str());
            info.SetBondUuid(bond_uuid.c_str());
            info.SetPhyUuid(phy_uuid.c_str());
            info.SetPhyName((*it_succ_add).c_str());
            info.SetBondName(bond_name.c_str());
            info.SetBondMode(EN_BOND_MODE_802);
            info.SetIsCfg(0);
            info.SetIsSelect(1);

            ret = pOper->AddMap(info);
            if ( ret != 0 )
            {
                if (m_nBondDbgInf)
                {
                    info.DbgShow();
                }
                //VNET_ASSERT(0);
                VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s  %s] CompBondLacpMap4Report add succ lacp failed \n", bond_name.c_str(), (*it_succ_add).c_str());
            }
            else
            {
                pOper->SetLacpSuccessNic(bond_uuid.c_str(), phy_uuid.c_str());
            }
        }

    }

    vector<string>::iterator it_fail_add;
    for ( it_fail_add = report._bond_lacp._fail_nics.begin(); it_fail_add !=  report._bond_lacp._fail_nics.end(); ++it_fail_add)
    {
        vector<CDBBondMap>::iterator it_map_add;
        for (it_map_add = vBondMap.begin(); it_map_add != vBondMap.end(); ++it_map_add)
        {
            if (*it_fail_add == it_map_add->GetPhyName()) break;
        }

        if (it_map_add == vBondMap.end())
        {
            CDBBondMap info;
            string bond_uuid = "";
            string phy_uuid = "";

            pOper->QueryUuid(vna_uuid.c_str(), (*it_fail_add).c_str(), EN_DB_PORT_TYPE_PHYSICAL, phy_uuid);
            pOper->QueryUuid(vna_uuid.c_str(), (bond_name.c_str()), EN_DB_PORT_TYPE_BOND, bond_uuid);

            info.SetVnaUuid(vna_uuid.c_str());
            info.SetBondUuid(bond_uuid.c_str());
            info.SetPhyUuid(phy_uuid.c_str());
            info.SetPhyName((*it_fail_add).c_str());
            info.SetBondName(bond_name.c_str());
            info.SetBondMode(EN_BOND_MODE_802);
            info.SetIsCfg(0);
            info.SetIsSelect(1);
            ret = pOper->AddMap(info);
            if ( ret != 0 )
            {
                if (m_nBondDbgInf)
                {
                    info.DbgShow();
                }
                //VNET_ASSERT(0);
                VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s   %s] CompBondLacpMap4Report add fail lacp failed \n", bond_name.c_str(), (*it_fail_add).c_str());
            }
            else
            {
                pOper->UnsetLacpSuccessNic(bond_uuid.c_str(), phy_uuid.c_str());
            }
        }

    }

    return 0;
}

int32 CPortMgr::CompBondBackupMap4Report(string &vna_uuid, string &bond_name, vector<CDBBondMap> vBondMap, CBondReport &report, CDBOperateBond *pOper)
{
    int32 ret;
    int32 flag = 0;

    vector<CDBBondMap>::iterator it = vBondMap.begin();
    for (; it != vBondMap.end(); ++it)
    {
        flag = 0;

        if (it->GetPhyName() == report._bond_backup._active_nic)
        {
            if (it->GetBackupIsActive() != 1)
            {
                pOper->SetBackupActiveNic((it->GetBondUuid()).c_str(), (it->GetPhyUuid()).c_str());
            }

            //it->SetIsCfg(1);
            it->SetIsSelect(1);
            ret = pOper->ModifyMap(*it);
            if ( ret != 0 )
            {
                if (m_nBondDbgInf)
                {
                    (*it).DbgShow();
                }
            }

            flag = 1;
        }

        //前面比完了，认为此循环不会循环到
        vector<string>::iterator it_other;
        for ( it_other = report._bond_backup._other_nic.begin(); it_other !=  report._bond_backup._other_nic.end(); ++it_other)
        {
            if (it->GetPhyName() == *it_other)
            {
                if (it->GetBackupIsActive() != 0)
                {
                    pOper->UnsetBackupActiveNic((it->GetBondUuid()).c_str(), (it->GetPhyUuid()).c_str());
                }

                //it->SetIsCfg(1);
                it->SetIsSelect(1);
                ret = pOper->ModifyMap(*it);
                if ( ret != 0 )
                {
                    if (m_nBondDbgInf)
                    {
                        (*it).DbgShow();
                    }
                }
                
                break;
            }
        }

        if ((flag == 0) && (it_other ==  report._bond_backup._other_nic.end()))
        {
            it->SetIsSelect(0);
            ret = pOper->ModifyMap(*it);
            if ( ret != 0 )
            {
                //VNET_ASSERT(0);
                VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s  %s]CompBondBackupMap4Report Modify BondMap failed for backup map \n", bond_name.c_str(), (it->GetPhyName()).c_str());
            }
        }
    }

    vector<string>::iterator it_other_add;
    for ( it_other_add = report._bond_backup._other_nic.begin(); it_other_add !=  report._bond_backup._other_nic.end(); ++it_other_add)
    {
        vector<CDBBondMap>::iterator it_map_add;
        for (it_map_add = vBondMap.begin(); it_map_add != vBondMap.end(); ++it_map_add)
        {
            if (*it_other_add == it_map_add->GetPhyName()) break;
        }

        if ((it_map_add == vBondMap.end()))
        {
            CDBBondMap info;
            string bond_uuid = "";
            string phy_uuid = "";

            pOper->QueryUuid(vna_uuid.c_str(), (*it_other_add).c_str(), EN_DB_PORT_TYPE_PHYSICAL, phy_uuid);
            pOper->QueryUuid(vna_uuid.c_str(), (bond_name).c_str(), EN_DB_PORT_TYPE_BOND, bond_uuid);

            info.SetVnaUuid(vna_uuid.c_str());
            info.SetBondUuid(bond_uuid.c_str());
            info.SetPhyUuid(phy_uuid.c_str());
            info.SetPhyName((*it_other_add).c_str());
            info.SetBondName(bond_name.c_str());
            info.SetBondMode(EN_BOND_MODE_BACKUP);
            info.SetIsCfg(0);
            info.SetIsSelect(1);
            ret = pOper->AddMap(info);
            if ( ret == 0 )
            {
                pOper->UnsetBackupActiveNic(bond_uuid.c_str(), phy_uuid.c_str());
            }
            else
            {
                if (m_nBondDbgInf)
                {
                    info.DbgShow();
                }
                //VNET_ASSERT(0);
                VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s  %s]CompBondBackupMap4Report add other backup failed \n",bond_name.c_str(), (*it_other_add).c_str());
            }
        }
    }

    vector<CDBBondMap>::iterator it_map_add;
    for (it_map_add = vBondMap.begin(); it_map_add != vBondMap.end(); ++it_map_add)
    {
        if (report._bond_backup._active_nic == it_map_add->GetPhyName()) break;
    }

    if ((it_map_add == vBondMap.end()))
    {
        CDBBondMap info;
        string bond_uuid;
        string phy_uuid;

        //2013.05.20 modify none problem
        if ("None" == report._bond_backup._active_nic)
        {
            return 0;
        }

        pOper->QueryUuid(vna_uuid.c_str(), (report._bond_backup._active_nic).c_str(), EN_DB_PORT_TYPE_PHYSICAL, phy_uuid);
        pOper->QueryUuid(vna_uuid.c_str(), (bond_name).c_str(), EN_DB_PORT_TYPE_BOND, bond_uuid);

        info.SetVnaUuid(vna_uuid.c_str());
        info.SetBondUuid(bond_uuid.c_str());
        info.SetPhyUuid(phy_uuid.c_str());
        info.SetPhyName((report._bond_backup._active_nic).c_str());
        info.SetBondName(bond_name.c_str());
        info.SetBondMode(EN_BOND_MODE_BACKUP);
        info.SetIsCfg(0);
        info.SetIsSelect(1);
        ret = pOper->AddMap(info);
        if ( ret == 0 )
        {
            pOper->SetBackupActiveNic(bond_uuid.c_str(), phy_uuid.c_str());
        }
        else
        {
            if (m_nBondDbgInf)
            {
                info.DbgShow();
            }
            //VNET_ASSERT(0);
            VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] CompBondBackupMap4Report add active backup failed \n", bond_name.c_str());
        }
    }

    return 0;
}


int32 CPortMgr::DoTrunkPortInfoReport(CMessageVNAPortInfoReport &msg)
{
    string  strVnaUuid;
    int32  nRet;
    int32  nFlag = 1;

    strVnaUuid = msg._vna_id;

    CDBOperateBond * pOper = GetDbIBond();
    if ( NULL == pOper )
    {
        //VNET_ASSERT(0);
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::DoTrunkPortInfoReport GetDbIBond failed \n");
        return -1;
    }

    vector<CDbBondSummary> bond_outVInfo;
    nRet = pOper->QuerySummary(strVnaUuid.c_str(),bond_outVInfo);
    if ( nRet != 0 )
    {
        //VNET_ASSERT(0);
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::DoTrunkPortInfoReport CDbBondSummary failed \n");
        return -1;
    }

    //bond map summary by vna_uuid, phy port, bond_uuid
    vector<CDBBondMap> map_outVInfo;
    nRet = pOper->QueryMapByVna(strVnaUuid.c_str(),map_outVInfo);
    if ( nRet != 0 )
    {
        //VNET_ASSERT(0);
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::DoTrunkPortInfoReport CDBBondMap failed \n");
        return -1;
    }

    //先获取数据库中的bond与bondmap情况，然后和上报相比
    if ((bond_outVInfo.empty()) && (TRUE !=map_outVInfo.empty()))
    {
        //VNET_ASSERT(0);
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::DoTrunkPortInfoReport Abnormal occur \n");
        return -1;
    }

    //上报发现为空直接返回不做处理，不增加新的bond，只比较存在的
    if ((bond_outVInfo.empty()) && (map_outVInfo.empty()))
    {
        return 0;
    }

    if (msg._bond_info.empty())
    {
        vector<CDbBondSummary>::iterator it_null = bond_outVInfo.begin();
        for (; it_null != bond_outVInfo.end(); ++it_null)
        {
            CDbBond info;
            info.SetUuid(it_null->GetBondUuid().c_str());
            nRet = pOper->Query(info);
            if ( nRet != 0 )
            {
                if (m_nPhyPortDbgInf)
                {
                    info.DbgShow();
                }
                VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] DoTrunkPortInfoReport Query failed for empty \n", it_null->GetBondName().c_str());
                //VNET_ASSERT(0);
                continue;
            }

            //上报为空，设置离线同时设置一致性，短暂离线不考虑，只针对kernel、loop配置部分
            info.SetIsOnline(0);
            //bondmap不设置一致性，以bond本身为准
            info.SetIsConsistency(0);
            nRet = pOper->Modify(info);
            if ( nRet != 0 )
            {
                if (m_nPhyPortDbgInf)
                {
                    info.DbgShow();
                }
                VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] DoPhyPortInfoReport Modify failed for empty \n", info.GetName().c_str());
                //VNET_ASSERT(0);
            }
        }

        return 0;
    }


    //执行modfiy或者del操作
    //存在先配置而后上报，这样上报先删除原来的?????
    vector<CDbBondSummary>::iterator it_a = bond_outVInfo.begin();
    vector<CBondReport>::iterator it_bond_a;
    for (; it_a != bond_outVInfo.end(); ++it_a)
    {
        nFlag = 1;

        for (it_bond_a = msg._bond_info.begin(); it_bond_a != msg._bond_info.end(); ++it_bond_a)
        {
            if (!(it_a->GetBondName().compare(it_bond_a->_bond_name)))
            {
                CDbBond info;
                info.SetUuid((it_a->GetBondUuid()).c_str());
                nRet = pOper->Query(info);
                if ( nRet != 0 )
                {
                    if (m_nBondDbgInf)
                    {
                        info.DbgShow();
                    }
                    //VNET_ASSERT(0);
                    VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] DoTrunkPortInfoReport Query failed \n", info.GetName().c_str());
                    break;
                }

                //首先比较可配置的属性
                if (info.GetIsPortCfg())
                {
                    if (!CmpTrunkPortCfgInfo(info, *it_bond_a))
                    {
                        info.SetIsConsistency(0);
                        nRet = pOper->Modify(info);
                        if ( nRet != 0 )
                        {
                            if (m_nBondDbgInf)
                            {
                                info.DbgShow();
                            }
                            //VNET_ASSERT(0);
                            VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] DoTrunkPortInfoReport Modify failed for Cfg \n", info.GetName().c_str());
                        }
                    }
                    else
                    {
                        if (!(info.GetIsConsistency()))
                        {
                            //这个地方先写consistency，后面判断bondmap时候可能再修改
                            info.SetIsConsistency(1);
                            nRet = pOper->Modify(info);
                            if ( nRet != 0 )
                            {
                                if (m_nBondDbgInf)
                                {
                                    info.DbgShow();
                                }
                                //VNET_ASSERT(0);
                                VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] DoTrunkPortInfoReport Modify failed for Cons \n", info.GetName().c_str());
                            }
                        }
                    }
                }

                //再比较没有配置的属性
                if (!CmpTrunkPortInfo(info, *it_bond_a))
                {
                    SetTrunkPortInfo(strVnaUuid, info, *it_bond_a);
                    nRet = pOper->Modify(info);
                    if ( nRet != 0 )
                    {
                        //VNET_ASSERT(0);
                        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] DoTrunkPortInfoReport Modify failed for Info \n", info.GetName().c_str());
                    }
                }

                //最后判断是否复活
                if (!(info.GetIsOnline()))
                {
                    info.SetIsOnline(1);
                    info.SetIsConsistency(1);
                    nRet = pOper->Modify(info);
                    if ( nRet != 0 )
                    {
                        if (m_nBondDbgInf)
                        {
                            info.DbgShow();
                        }
                        //VNET_ASSERT(0);
                        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] DoTrunkPortInfoReport Modify failed for Online \n", info.GetName().c_str());
                    }
                }

                //进入到bond map处理当中
                //用bond id查询map表
                vector<CDBBondMap> outVInfo;
                nRet = pOper->QueryMapByBond(it_a->GetBondUuid().c_str(),outVInfo);
                if ( nRet != 0 )
                {
                    VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] DoTrunkPortInfoReport QueryMapByBond failed \n",  it_a->GetBondUuid().c_str());
                    //VNET_ASSERT(0);
                    break;
                }

                if (it_bond_a->_bond_report_mode == EN_BOND_MODE_802)
                {
                    if (outVInfo.empty())
                    {
                        AddBondLacpMap4Report(strVnaUuid, it_bond_a->_bond_name, *it_bond_a, pOper);
                    }
                    else
                    {
                        CompBondLacpMap4Report(strVnaUuid, it_bond_a->_bond_name, outVInfo, *it_bond_a, pOper);
                    }
                }

                if (it_bond_a->_bond_report_mode == EN_BOND_MODE_BACKUP)
                {
                    if (outVInfo.empty())
                    {
                        AddBondBackupMap4Report(strVnaUuid, it_bond_a->_bond_name, *it_bond_a, pOper);
                    }
                    else
                    {
                        CompBondBackupMap4Report(strVnaUuid, it_bond_a->_bond_name, outVInfo, *it_bond_a, pOper);
                    }
                }

                //该bondmap处理完后判断bondmap的在线和配置情况以决定bond的一致性
                vector<CDBBondMap> MapInfo;
                nRet = pOper->QueryMapByBond(it_a->GetBondUuid().c_str(),MapInfo);
                if ( nRet != 0 )
                {
                    VNET_LOG(VNET_LOG_ERROR, "CPortMgr::DoTrunkPortInfoReport QueryMapByBond failed");
                    //VNET_ASSERT(0);
                    break;
                }

                vector<CDBBondMap>::iterator it = MapInfo.begin();
                for (; it != MapInfo.end(); ++it)
                {
                    if ((0 == it->GetIsCfg()) || (0 == it->GetIsSelect()))
                    {
                        nFlag = 0;
                        break;
                    }
                }

                info.SetIsConsistency(nFlag);
                nRet = pOper->Modify(info);
                if ( nRet != 0 )
                {
                    if (m_nBondDbgInf)
                    {
                        info.DbgShow();
                    }
                    VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] DoTrunkPortInfoReport Modify failed for Consistency \n", info.GetName().c_str());
                    //VNET_ASSERT(0);
                }

                //modify ip/mask
                ModPortIp(info.GetUuid(), info.GetName(), (*it_bond_a)._basic_info._ip, (*it_bond_a)._basic_info._mask, info.GetIsPortCfg(), info.GetIsOnline());
                break;
            }
        }

        if (it_bond_a == msg._bond_info.end())
        {
            CDbBond info;
            info.SetUuid((it_a->GetBondUuid()).c_str());
            nRet = pOper->Query(info);
            if ( nRet != 0 )
            {
                if (m_nBondDbgInf)
                {
                    info.DbgShow();
                }
                VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] DoTrunkPortInfoReport Query failed \n", (it_a->GetBondUuid()).c_str());
                //VNET_ASSERT(0);
            }

            //此条记录不存在了
            info.SetIsOnline(0);
            info.SetIsConsistency(0);
            nRet = pOper->Modify(info);
            if ( nRet != 0 )
            {
                if (m_nBondDbgInf)
                {
                    info.DbgShow();
                }
                VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] DoTrunkPortInfoReport Modify failed \n", info.GetName().c_str());
                //VNET_ASSERT(0);
            }

            //del ip/mask
            DelPortIp(it_a->GetBondUuid(), it_a->GetBondName());
        }
    }

    return 0;
}


/**************************** Info Set**************************************************/
//不同端口的信息设置还可能不一样
//type 可以考虑不设置
int32 CPortMgr::SetPhyPortInfo(string vna_uuid, CDbPortPhysical &info, CPhyPortReport &report)
{
    //基础信息
    info.SetVnaUuid(vna_uuid.c_str());
    info.SetName(report._basic_info._name.c_str());
    //info.SetPortType(report._basic_info._port_type);
    info.SetPortType(EN_DB_PORT_TYPE_PHYSICAL);
    info.SetState(report._basic_info._state);
    info.SetIsLinked(report._is_linked);
    info.SetSpeed(report._speed.c_str());
    info.SetDuplex(report._duplex.c_str());
    info.SetAutoNeg(report._auto_negotiate.c_str());
    info.SetPromiscuous(report._basic_info._promiscuous);
    info.SetMtu(report._basic_info._mtu);
    info.SetIsSriov(report._is_sriov);

    return 0;
}

int32 CPortMgr::SetUplinkLoopPortInfo(string vna_uuid, CDbPortUplinkLoop &info, CUplinkLoopPortReport &report)
{
    //基础信息
    info.SetVnaUuid(vna_uuid.c_str());

    info.SetName(report._basic_info._name.c_str());
    //info.SetPortType(report._basic_info._port_type);
    info.SetPortType(EN_DB_PORT_TYPE_UPLINKLOOP);
    info.SetState(report._basic_info._state);
    info.SetPromiscuous(report._basic_info._promiscuous);
    info.SetMtu(report._basic_info._mtu);

    return 0;
}

int32 CPortMgr::SetPhySriovInfo(string vna_uuid, CDbPortPhysical &info, CPhyPortReport &report)
{
    //基础信息
    info.SetVnaUuid(vna_uuid.c_str());

    info.SetName(report._basic_info._name.c_str());
    //info.SetPortType(report._basic_info._port_type);
    info.SetPortType(EN_DB_PORT_TYPE_PHYSICAL);

    info.SetState(report._basic_info._state);

    info.SetIsLinked(report._is_linked);
    info.SetSpeed(report._speed.c_str());
    info.SetDuplex(report._duplex.c_str());
    info.SetAutoNeg(report._auto_negotiate.c_str());
    info.SetPromiscuous(report._basic_info._promiscuous);
    info.SetMtu(report._basic_info._mtu);

    info.SetIsSriov(report._is_sriov);

    //留给配置设置
    //info.SetIsloopCfg(report._is_loop_cfg);
    info.SetIsloopRep(report._is_loop_report);

    return 0;
}

int32 CPortMgr::SetTrunkPortInfo(string vna_uuid, CDbBond &info, CBondReport &report)
{
    info.SetVnaUuid(vna_uuid.c_str());
    info.SetPortType(EN_DB_PORT_TYPE_BOND);
    info.SetName((report._bond_name).c_str());
    info.SetState(report._basic_info._state);

    //需要设置port的基本属性，底层也应该上报??
    //info.SetIsPortCfg(0);
    info.SetBondMode(report._bond_report_mode);

    //LACP信息添加
    if (report._bond_report_mode == EN_BOND_MODE_802)
    {
        info.SetIsHasLacpInfo(1);
        info.SetLacpAggrId((report._bond_lacp._aggregate_id).c_str());
        info.SetLacpPartnerMac((report._bond_lacp._partner_mac).c_str());
        info.SetLacpState(report._bond_lacp._state);
    }

    return 0;
}

//不同端口可能不一样
//注意此处比较的是可配置部分
//配置信息比较
int32 CPortMgr::CmpPhyPortCfgInfo(CDbPortPhysical &info, CPhyPortReport &report)
{
    if (info.GetMtu() != report._basic_info._mtu) return 0;

    return -1;
}

int32 CPortMgr::CmpUplinkLoopPortCfgInfo(CDbPortUplinkLoop &info, CUplinkLoopPortReport &report)
{
    return -1;
}

int32 CPortMgr::CmpPhySriovCfgInfo(CDbPortPhysical &info, CPhyPortReport &report)
{
    //if (info.GetMtu() != report._basic_info._mtu) return 0;
    if (info.GetIsloopCfg() != report._is_loop_report) return 0;

    return -1;
}

int32 CPortMgr::CmpTrunkPortCfgInfo(CDbBond &info, CBondReport &report)
{
    //if (info.GetBondMode()!= report._bond_report_mode) return 0;

    return -1;
}

//非配置信息比较
int32 CPortMgr::CmpPhyPortInfo(CDbPortPhysical &info, CPhyPortReport &report)
{
    if (info.GetState() != report._basic_info._state) return 0;
    if (info.GetIsLinked() != report._is_linked) return 0;
    if (info.GetSpeed() != report._speed) return 0;
    if (info.GetDuplex() != report._duplex) return 0;
    if (info.GetAutoNeg() != report._auto_negotiate) return 0;
    if (info.GetPromiscuous() != report._basic_info._promiscuous) return 0;

    return -1;
}

int32 CPortMgr::CmpUplinkLoopPortInfo(CDbPortUplinkLoop &info, CUplinkLoopPortReport &report)
{
    if (info.GetState() != report._basic_info._state) return 0;
    if (info.GetPromiscuous() != report._basic_info._promiscuous) return 0;

    return -1;
}

int32 CPortMgr::CmpPhySriovInfo(CDbPortPhysical &info, CPhyPortReport &report)
{
    if (info.GetState() != report._basic_info._state) return 0;
    if (info.GetIsLinked() != report._is_linked) return 0;
    if (info.GetSpeed() != report._speed) return 0;
    if (info.GetDuplex() != report._duplex) return 0;
    if (info.GetAutoNeg() != report._auto_negotiate) return 0;
    if (info.GetPromiscuous() != report._basic_info._promiscuous) return 0;
    //if (info.GetIsloopCfg() != report._is_loop_cfg) return 0;
    if (info.GetIsloopRep() != report._is_loop_report) return 0;

    return -1;
}

int32 CPortMgr::CmpTrunkPortInfo(CDbBond &info, CBondReport &report)
{
    if (info.GetState() != report._basic_info._state) return 0;
    if (info.GetBondMode()!= report._bond_report_mode) return 0;

    if (report._bond_report_mode == EN_BOND_MODE_802)
    {
        if (info.GetLacpAggrId() != report._bond_lacp._aggregate_id) return 0;
        if (info.GetLacpPartnerMac() != report._bond_lacp._partner_mac) return 0;
        if (info.GetLacpState() != report._bond_lacp._state) return 0;
    }

    return -1;
}


//phy port info query，出参以db定义的为准??
int32 CPortMgr::QueryPhyPortInfo(string vna_uuid, vector<CDbPortPhysical> &phyportinfo)
{
    int32 ret;

    if (vna_uuid.empty())
    {
        return -1;
    }

    CDBOperatePortPhysical * pOper = GetDbIPortPhy();
    if ( NULL == pOper )
    {
        //VNET_ASSERT(0);
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::QueryPhyPortInfo GetDbIPortPhy failed");
        return -1;
    }

    vector<CDbPortSummary> outVInfo;
    ret = pOper->QuerySummary(vna_uuid.c_str(), 0, outVInfo);
    if ( ret != 0 )
    {
        //VNET_ASSERT(0);
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::QueryPhyPortInfo QuerySummary failed");
        return -1;
    }

    vector<CDbPortSummary>::iterator it = outVInfo.begin();
    for (; it != outVInfo.end(); ++it)
    {
        CDbPortPhysical info;
        info.SetUuid(it->GetUuid().c_str());
        ret = pOper->Query(info);
        if ( ret != 0 )
        {
            //VNET_ASSERT(0);
            VNET_LOG(VNET_LOG_ERROR, "CPortMgr::QueryPhyPortInfo Query failed");
            continue;
        }

        phyportinfo.push_back(info);
    }

    return 0;
}

//VF---->PCI
int32 CPortMgr::GetPciByVF(const string &vf_uuid, string &vf_pci)
{
    int32  ret;

    CDBOperatePortSriovVf * pOper = GetDbIPortSriovVf();
    if ( NULL == pOper )
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::GetPciByVF GetDbIPortSriovVf failed \n");
        //VNET_ASSERT(0);
        return -1;
    }

    if (vf_uuid.empty())
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::GetPciByVF vf_uuid empty \n");
        //VNET_ASSERT(0);
        return -1;
    }

    CDbPortSriovVf info;
    info.SetUuid(vf_uuid.c_str());
    ret = pOper->Query(info);
    if ( ret != 0 )
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] GetPciByVF Query failed \n", vf_uuid.c_str());
        return -1;
    }

    vf_pci = info.GetPci();

    return 0;
}

//提供sriov pci分配接口
int32 CPortMgr::GetSriovVF(const string &vna_uuid, string &port_name, int32 port_type, string &vf_uuid)
{
    int32  ret = 0;
    string port_uuid = "";

    int32 vf_num = 0;
    string bond_map = "";
    int32 vf_tmp = 0;

    CDBOperatePortPhysical * pOperPhy;
    CDBOperateBond * pOperBond;
    CDBOperatePortSriovVf * pOperSr;

    pOperPhy = GetDbIPortPhy();
    pOperBond = GetDbIBond();
    pOperSr = GetDbIPortSriovVf();

    if ((vna_uuid.empty()) || (port_name.empty()))
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::GetSriovVF vna_uuid port_name empty \n");
        //VNET_ASSERT(0);
        return -1;
    }

    if ((NULL == pOperPhy) || (NULL == pOperBond) || (NULL == pOperSr))
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::GetSriovVF Oper empty \n");
        //VNET_ASSERT(0);
        return -1;
    }

    if (port_type == EN_DB_PORT_TYPE_PHYSICAL)
    {
        //首先获取该物理口的uuid
        ret = pOperPhy->QueryUuid(vna_uuid.c_str(), port_name.c_str(), EN_DB_PORT_TYPE_PHYSICAL, port_uuid);
        if ( ret != 0 )
        {
            VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s %s] GetSriovVF QueryUuid PhyPort failed \n", vna_uuid.c_str(), port_name.c_str());
            //VNET_ASSERT(0);
            return -1;
        }

        //获取该物理口下空闲VF
        vector<CDbPortFreeSriovVf> outVInfo;
        ret = pOperSr->QueryFree(port_uuid.c_str(),outVInfo);
        if ( ret != 0 )
        {
            VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s %s] GetSriovVF QueryFree PhyPort failed \n", vna_uuid.c_str(), port_name.c_str());
            return -1;
        }

        if (outVInfo.empty())
        {
            VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s %s] GetSriovVF FreeVF is empty \n", vna_uuid.c_str(), port_name.c_str());
            return -1;
        }

        //如果有空闲资源，丛中拿一个出来
        vector<CDbPortFreeSriovVf>::iterator it = outVInfo.begin();
        for (; it != outVInfo.end(); ++it)
        {
            vf_uuid = it->GetSriovVfUuid();
            break;
        }

        return 0;
    }

    if (port_type == EN_DB_PORT_TYPE_BOND)
    {
        //首先获取该bond口的uuid
        ret = pOperBond->QueryUuid(vna_uuid.c_str(), port_name.c_str(), EN_DB_PORT_TYPE_BOND, port_uuid);
        if ( ret != 0 )
        {
            VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s %s] GetSriovVF QueryUuid Bond failed \n", vna_uuid.c_str(), port_name.c_str());
            //VNET_ASSERT(0);
            return -1;
        }

        //获取该bond下的map
        vector<CDBBondMap> outVInfo;
        ret = pOperBond->QueryMapByBond(port_uuid.c_str(),outVInfo);
        if ( ret != 0 )
        {
            VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s %s] GetSriovVF QueryMapByBond failed \n", vna_uuid.c_str(), port_name.c_str());
            //VNET_ASSERT(0);
            return -1;
        }

        if (outVInfo.empty())
        {
            VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s %s] GetSriovVF BondMap is empty \n", vna_uuid.c_str(), port_name.c_str());
            return -1;
        }

        vector<CDBBondMap>::iterator it_map = outVInfo.begin();
        for (; it_map != outVInfo.end(); ++it_map)
        {
            port_uuid = it_map->GetPhyUuid();
            vector<CDbPortFreeSriovVf> outSrivoVF;
            ret = pOperSr->QueryFree(port_uuid.c_str(),outSrivoVF);
            if ( ret != 0 )
            {
                continue;
            }

            //计算该物理口下的空闲VF数量
            vf_tmp = outSrivoVF.size();

            //找出拥有最多资源的map
            if (vf_tmp > vf_num)
            {
                vf_num = vf_tmp;
                bond_map = it_map->GetPhyUuid();
            }
        }

        //前面循环结束了，判断有无找到可用资源
        if (bond_map.empty())
        {
            VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s %s] BondMap FreeVF is empty \n", vna_uuid.c_str(), port_name.c_str());
            return -1;
        }

        //再次获取该物理口下空闲VF
        vector<CDbPortFreeSriovVf> outFreeSriov;
        ret = pOperSr->QueryFree(bond_map.c_str(),outFreeSriov);
        if ( ret != 0 )
        {
            VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s %s %s] GetSriovVF QueryFree Bond failed \n", vna_uuid.c_str(), port_name.c_str(), bond_map.c_str());
            //VNET_ASSERT(0);
            return -1;
        }

        //如果有空闲资源，丛中拿一个出来
        vector<CDbPortFreeSriovVf>::iterator it = outFreeSriov.begin();
        for (; it != outFreeSriov.end(); ++it)
        {
            vf_uuid = it->GetSriovVfUuid();
            break;
        }

        return 0;
    }

    return -1;
}

int32 CPortMgr::GetPhySriovVF(const string &port_uuid, string &vf_uuid)
{
    int32  ret = 0;

    CDBOperatePortSriovVf * pOperSr;

    if (port_uuid.empty())
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::GetPhySriovVF port_uuid empty \n");
        //VNET_ASSERT(0);
        return -1;
    }

    pOperSr = GetDbIPortSriovVf();
    if (NULL == pOperSr)
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::GetPhySriovVF Oper empty \n");
        //VNET_ASSERT(0);
        return -1;
    }

    //获取该物理口下空闲VF
    vector<CDbPortFreeSriovVf> outVInfo;
    ret = pOperSr->QueryFree(port_uuid.c_str(),outVInfo);
    if ( ret != 0 )
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] GetPhySriovVF QueryFree failed \n", port_uuid.c_str());
        return -1;
    }

    if (outVInfo.empty())
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] GetPhySriovVF FreeVF is empty \n", port_uuid.c_str());
        return -1;
    }

    //如果有空闲资源，丛中拿一个出来
    vector<CDbPortFreeSriovVf>::iterator it = outVInfo.begin();
    for (; it != outVInfo.end(); ++it)
    {
        vf_uuid = it->GetSriovVfUuid();
        return 0;
    }

    return -1;
}

int32 CPortMgr::GetPhyMultiSriovVF(const string &strPortUuid, uint32 nVfNum, vector<string> &vVfUuid)
{
    int32  nRet = 0;
    uint32 nLoop = 0;

    CDBOperatePortSriovVf * pOperSr;

    if ((strPortUuid.empty()) || (0 == nVfNum))
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::GetPhyMultiSriovVF port_uuid empty or vfnum is zero \n");
        //VNET_ASSERT(0);
        return -1;
    }

    pOperSr = GetDbIPortSriovVf();
    if (NULL == pOperSr)
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::GetPhyMultiSriovVF GetDbIPortSriovVf failed \n");
        //VNET_ASSERT(0);
        return -1;
    }

    //获取该物理口下空闲VF
    vector<CDbPortFreeSriovVf> outVInfo;
    nRet = pOperSr->QueryFree(strPortUuid.c_str(),outVInfo);
    if ( nRet != 0 )
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] GetPhySriovVF QueryFree failed \n", strPortUuid.c_str());
        return -1;
    }

    if (outVInfo.empty())
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] GetPhySriovVF FreeVF is empty \n", strPortUuid.c_str());
        return -1;
    }

    if (outVInfo.size() < nVfNum)
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] GetPhySriovVF VF is not enough \n", strPortUuid.c_str());
        return -1;
    }

    //如果有空闲资源，丛中拿一个出来
    vector<CDbPortFreeSriovVf>::iterator it = outVInfo.begin();
    for (; it != outVInfo.end(); ++it)
    {
        nLoop++;

        if (nLoop <= nVfNum)
        {
            vVfUuid.push_back(it->GetSriovVfUuid());
        }
        else
        {
            break;
        }
    }

    return 0;
}

//kernel 端口配置
int32 CPortMgr::AddKernelPort(string vna_uuid,int32 port_type, string name, int32 type, string switchid, string pgid, int32 isdhcp)
{
    int32 ret;

    if ((vna_uuid.empty()) || (name.empty()))
    {
        //VNET_ASSERT(0);
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::AddKernelPort Input empty \n");
        return -1;
    }

    CDBOperatePortKernel * pOper = GetDbIPortKernel();
    if ( NULL == pOper )
    {
        //VNET_ASSERT(0);
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::AddKernelPort GetDbIPortKernel failed \n");
        return -1;
    }

    vector<CDbPortSummary> outVInfo;
    ret = pOper->QuerySummary(vna_uuid.c_str(),outVInfo);
    if (ret != 0)
    {
        //VNET_ASSERT(0);
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] AddKernelPort QuerySummary failed \n", vna_uuid.c_str());
        return -1;
    }

    if (TRUE != outVInfo.empty())
    {
        vector<CDbPortSummary>::iterator it = outVInfo.begin();
        for (; it != outVInfo.end(); ++it)
        {
            //例如上报先于应答写DB
            if (it->GetName() == name)
            {
                CDbPortKernel info;
                info.SetUuid(it->GetUuid().c_str());
                ret = pOper->Query(info);
                if (0 == ret)
                {
                    info.SetIsOnline(1);
                    info.SetIsCfg(1);
                    info.SetIsPortCfg(1);
                    info.SetIsConsistency(1);
                    info.SetIsDhcp(isdhcp);

                    if (switchid.empty())
                    {
                        info.SetIsCfgVswitch(0);
                    }
                    else
                    {
                        info.SetIsCfgVswitch(1);
                    }

                    if (pgid.empty())
                    {
                        info.SetIsCfgPg(0);
                    }
                    else
                    {
                        info.SetIsCfgPg(1);
                    }

                    info.SetType(type);
                    info.SetVswitchUuid(switchid.c_str());
                    info.SetPgUuid(pgid.c_str());

                    ret = pOper->Modify(info);
                    if ( ret != 0 )
                    {
                        info.DbgShow();
                        //VNET_ASSERT(0);
                        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[vna_uuid:%s name:%s] AddKernelPort Modify failed \n", vna_uuid.c_str(), name.c_str());
                        return -1;
                    }

                    return 0;
                }
                else
                {
                    info.DbgShow();
                    //VNET_ASSERT(0);
                    return -1;
                }
            }
        }
    }

    CDbPortKernel info;
    info.SetVnaUuid(vna_uuid.c_str());
    info.SetPortType(port_type);
    info.SetName(name.c_str());
    info.SetType(type);
    info.SetIsOnline(1);
    info.SetIsConsistency(1);
    info.SetIsCfg(1);
    info.SetIsPortCfg(1);
    info.SetIsDhcp(isdhcp);

    if (switchid.empty())
    {
        info.SetIsCfgVswitch(0);
    }
    else
    {
        info.SetIsCfgVswitch(1);
    }

    if (pgid.empty())
    {
        info.SetIsCfgPg(0);
    }
    else
    {
        info.SetIsCfgPg(1);
    }

    info.SetVswitchUuid(switchid.c_str());
    info.SetPgUuid(pgid.c_str());

    ret = pOper->Add(info);
    if ( ret != 0 )
    {
        info.DbgShow();
        //VNET_ASSERT(0);
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[vna_uuid:%s name:%s] AddKernelPort Add failed \n", vna_uuid.c_str(), name.c_str());
        return -1;
    }

    return 0;
}

int32 CPortMgr::ModKernelPort(string vna_uuid,int32 port_type, string name, int32 type, string switchid, string pgid)
{
    int32 ret;

    if ((vna_uuid.empty()) || (name.empty()))
    {
        //VNET_ASSERT(0);
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::ModKernelPort Input empty \n");
        return -1;
    }

    CDBOperatePortKernel * pOper = GetDbIPortKernel();
    if ( NULL == pOper )
    {
        //VNET_ASSERT(0);
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::ModKernelPort GetDbIPortKernel failed \n");
        return -1;
    }

    vector<CDbPortSummary> outVInfo;
    ret = pOper->QuerySummary(vna_uuid.c_str(),outVInfo);
    if (ret != 0)
    {
        //VNET_ASSERT(0);
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] ModKernelPort QuerySummary failed \n", vna_uuid.c_str());
        return -1;
    }

    if (outVInfo.empty())
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] ModKernelPort Info is NULL \n", vna_uuid.c_str());
        return -1;
    }

    vector<CDbPortSummary>::iterator it = outVInfo.begin();
    for (; it != outVInfo.end(); ++it)
    {
        //例如上报先于应答写DB
        if (it->GetName() == name)
        {
            CDbPortKernel info;
            info.SetUuid(it->GetUuid().c_str());
            ret = pOper->Query(info);
            if (0 == ret)
            {
                //修改哪些属性????
                info.SetType(type);

                if (switchid.empty())
                {
                    info.SetIsCfgVswitch(0);
                }
                else
                {
                    info.SetIsCfgVswitch(1);
                }

                if (pgid.empty())
                {
                    info.SetIsCfgPg(0);
                }
                else
                {
                    info.SetIsCfgPg(1);
                }

                info.SetVswitchUuid(switchid.c_str());
                info.SetPgUuid(pgid.c_str());

                ret = pOper->Modify(info);
                if ( ret != 0 )
                {
                    info.DbgShow();
                    //VNET_ASSERT(0);
                    VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[vna_uuid:%s name:%s] ModKernelPort Modify failed \n", vna_uuid.c_str(), name.c_str());
                    return -1;
                }

                return 0;
            }
            else
            {
                info.DbgShow();
                //VNET_ASSERT(0);
                return -1;
            }

        }
    }

    return -1;
}

int32 CPortMgr::DelKernelPort(string vna_uuid, string name)
{
    int32 ret;

    if ((vna_uuid.empty()) || (name.empty()))
    {
        //VNET_ASSERT(0);
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::DelKernelPort Input empty \n");
        return -1;
    }

    CDBOperatePortKernel * pOper = GetDbIPortKernel();
    if ( NULL == pOper )
    {
        //VNET_ASSERT(0);
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::DelKernelPort GetDbIPortKernel failed \n");
        return -1;
    }

    vector<CDbPortSummary> outVInfo;
    ret = pOper->QuerySummary(vna_uuid.c_str(),outVInfo);
    if ( ret != 0 )
    {
        //VNET_ASSERT(0);
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] DelKernelPort QuerySummary failed \n", vna_uuid.c_str());
        return -1;
    }

    if (outVInfo.empty())
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] DelKernelPort Info is NULL \n", vna_uuid.c_str());
        return -1;
    }

    vector<CDbPortSummary>::iterator it = outVInfo.begin();
    for (; it != outVInfo.end(); ++it)
    {
        if (name == it->GetName())
        {
            ret = pOper->Del((it->GetUuid()).c_str());
            if ( ret != 0 )
            {
                //VNET_ASSERT(0);
                VNET_LOG(VNET_LOG_ERROR, "CPortMgr::DelKernelPort[vna_uuid:%s name:%s]] Del failed \n", vna_uuid.c_str(), name.c_str());
                return -1;
            }

            return 0;
        }
    }

    return -1;
}

//uplink port config
int32 CPortMgr::AddUplinkLoopPort(const string &vna_uuid, const int32 port_type, const string port_name)
{
    int32 ret;

    if ((vna_uuid.empty()) || (port_name.empty()))
    {
        //VNET_ASSERT(0);
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::AddUplinkLoopPort input empty \n");
        return -1;
    }

    CDBOperatePortUplinkLoop * pOper = GetDbIPortUplinkLoop();
    if ( NULL == pOper )
    {
        //VNET_ASSERT(0);
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::AddUplinkLoopPort GetDbIPortUplinkLoop failed \n");
        return -1;
    }

    vector<CDbPortSummary> outVInfo;
    ret = pOper->QuerySummary(vna_uuid.c_str(), outVInfo);
    if ( ret != 0 )
    {
        //VNET_ASSERT(0);
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] AddUplinkLoopPort QuerySummary failed \n", vna_uuid.c_str());
        return -1;
    }

    vector<CDbPortSummary>::iterator it = outVInfo.begin();
    for (; it != outVInfo.end(); ++it)
    {
        //例如上报先于应答写DB
        if (it->GetName() == port_name)
        {
            CDbPortUplinkLoop info;
            info.SetUuid((it->GetUuid()).c_str());
            ret = pOper->Query(info);
            if ( 0 == ret )
            {
                info.SetIsOnline(1);
                info.SetIsCfg(1);
                info.SetIsPortCfg(1);
                info.SetIsConsistency(1);

                ret = pOper->Modify(info);
                if ( ret != 0 )
                {
                    info.DbgShow();
                    //VNET_ASSERT(0);
                    VNET_LOG(VNET_LOG_ERROR, "CPortMgr::AddUplinkLoopPort [%s] Modify failed \n", port_name.c_str());
                    return -1;
                }

                return 0;
            }
            else
            {
                info.DbgShow();
                //VNET_ASSERT(0);
                return -1;
            }
        }
    }

    //直接添加
    CDbPortUplinkLoop info;
    info.SetVnaUuid(vna_uuid.c_str());
    info.SetPortType(port_type);
    info.SetName(port_name.c_str());
    info.SetIsOnline(1);
    info.SetIsCfg(1);
    info.SetIsPortCfg(1);
    info.SetIsConsistency(1);

    ret = pOper->Add(info);
    if ( ret != 0 )
    {
        info.DbgShow();
        //VNET_ASSERT(0);
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::AddUplinkLoopPort [%s] Add failed \n", port_name.c_str());
        return -1;
    }

    return 0;
}

int32 CPortMgr::ModUplinkLoopPort(const string &vna_uuid, const int32 port_type, const string port_name)
{
    int32 ret;

    if ((vna_uuid.empty()) || (port_name.empty()))
    {
        //VNET_ASSERT(0);
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::ModUplinkLoopPort input empty \n");
        return -1;
    }

    CDBOperatePortUplinkLoop * pOper = GetDbIPortUplinkLoop();
    if ( NULL == pOper )
    {
        //VNET_ASSERT(0);
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::ModUplinkLoopPort GetDbIPortUplinkLoop failed \n");
        return -1;
    }

    vector<CDbPortSummary> outVInfo;
    ret = pOper->QuerySummary(vna_uuid.c_str(), outVInfo);
    if ( ret != 0 )
    {
        //VNET_ASSERT(0);
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] ModUplinkLoopPort QuerySummary failed \n", vna_uuid.c_str());
        return -1;
    }

    vector<CDbPortSummary>::iterator it = outVInfo.begin();
    for (; it != outVInfo.end(); ++it)
    {
        CDbPortUplinkLoop info;
        info.SetUuid((it->GetUuid()).c_str());
        ret = pOper->Query(info);
        if ( 0 == ret )
        {
            info.SetIsOnline(1);
            info.SetIsCfg(1);
            info.SetIsPortCfg(1);
            info.SetIsConsistency(1);

            ret = pOper->Modify(info);
            if ( ret != 0 )
            {
                return -1;
            }

            return 0;
        }
    }

    return -1;
}

int32 CPortMgr::SetSriovPortLoop(const string &vna_uuid, const string port_name)
{
    int32  ret;

    CDBOperatePortPhysical * pOper = GetDbIPortPhy();
    if ( NULL == pOper )
    {
        //VNET_ASSERT(0);
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::SetSriovPortLoop GetDbIPortPhy failed \n");
        return -1;
    }

    vector<CDbPortSummary> outVInfo;
    ret = pOper->QuerySummary(vna_uuid.c_str(), 1, outVInfo);
    if ( ret != 0 )
    {
        //VNET_ASSERT(0);
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] SetSriovPortLoop QuerySummary failed \n", vna_uuid.c_str());
        return -1;
    }

    vector<CDbPortSummary>::iterator it = outVInfo.begin();
    for (; it != outVInfo.end(); ++it)
    {
        if (it->GetName() == port_name)
        {
            CDbPortPhysical info;
            info.SetUuid(it->GetUuid().c_str());
            ret = pOper->Query(info);
            if ( ret != 0 )
            {
                info.DbgShow();
                //VNET_ASSERT(0);
                VNET_LOG(VNET_LOG_ERROR, "CPortMgr::SetSriovPortLoop [%s] Query failed \n", port_name.c_str());
                return -1;
            }

            //此处单独设置loop cfg标志，其他配置标志在add操作设置
            info.SetIsloopCfg(1);
            ret = pOper->Modify(info);
            if ( ret != 0 )
            {
                info.DbgShow();
                //VNET_ASSERT(0);
                VNET_LOG(VNET_LOG_ERROR, "CPortMgr::SetSriovPortLoop [%s] Modify failed \n", port_name.c_str());
                return -1;
            }

            return 0;
        }
    }

    return -1;
}

int32 CPortMgr::UnSetSriovPortLoop(const string &vna_uuid, const string port_name)
{
    int32  ret;

    CDBOperatePortPhysical * pOper = GetDbIPortPhy();
    if ( NULL == pOper )
    {
        //VNET_ASSERT(0);
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::UnSetSriovPortLoop GetDbIPortPhy failed \n");
        return -1;
    }

    vector<CDbPortSummary> outVInfo;
    ret = pOper->QuerySummary(vna_uuid.c_str(), 1, outVInfo);
    if ( ret != 0 )
    {
        //VNET_ASSERT(0);
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::UnSetSriovPortLoop QuerySummary failed \n");
        return -1;
    }

    vector<CDbPortSummary>::iterator it = outVInfo.begin();
    for (; it != outVInfo.end(); ++it)
    {
        if (it->GetName() == port_name)
        {
            CDbPortPhysical info;
            info.SetUuid(it->GetUuid().c_str());
            ret = pOper->Query(info);
            if ( ret != 0 )
            {
                info.DbgShow();
                //VNET_ASSERT(0);
                VNET_LOG(VNET_LOG_ERROR, "CPortMgr::UnSetSriovPortLoop [%s] Query failed \n", port_name.c_str());
                return -1;
            }

            //info.SetIsOnline(1);
            //info.SetIsPortCfg(1);
            info.SetIsloopCfg(0);
            ret = pOper->Modify(info);
            if ( ret != 0 )
            {
                info.DbgShow();
                //VNET_ASSERT(0);
                VNET_LOG(VNET_LOG_ERROR, "CPortMgr::UnSetSriovPortLoop [%s] Modify failed \n", port_name.c_str());
                return -1;
            }

            return 0;
        }
    }

    return -1;
}

int32 CPortMgr::DelUplinkLoopPort(const string &vna_uuid, const string port_name)
{
    int32 ret;

    if ((vna_uuid.empty()) || (port_name.empty()))
    {
        //VNET_ASSERT(0);
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::DelUplinkLoopPort input empty \n");
        return -1;
    }

    CDBOperatePortUplinkLoop * pOper = GetDbIPortUplinkLoop();
    if ( NULL == pOper )
    {
        //VNET_ASSERT(0);
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::DelUplinkLoopPort QuerySummary failed \n");
        return -1;
    }

    vector<CDbPortSummary> outVInfo;
    ret = pOper->QuerySummary(vna_uuid.c_str(),outVInfo);
    if ( ret != 0 )
    {
        //VNET_ASSERT(0);
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] DelUplinkLoopPort QuerySummary failed \n", vna_uuid.c_str());
        return -1;
    }

    vector<CDbPortSummary>::iterator it = outVInfo.begin();
    for (; it != outVInfo.end(); ++it)
    {
        if (port_name == it->GetName())
        {
            ret = pOper->Del((it->GetUuid()).c_str());
            if ( ret != 0 )
            {
                //VNET_ASSERT(0);
                VNET_LOG(VNET_LOG_ERROR, "CPortMgr::DelUplinkLoopPort [%s  %s] Del failed \n", port_name.c_str(), (it->GetUuid()).c_str());
                return -1;
            }

            return 0;
        }
    }

    return -1;
}

int32 CPortMgr::AddBondLacpMap4Cfg(string &vna_uuid, string &bond_name, vector<string> &report, CDBOperateBond *pOper)
{
    int32 ret;

    vector<string>::iterator it = report.begin();
    for (; it != report.end(); ++it)
    {
        //开始添加map表
        CDBBondMap info;
        string bond_uuid = "";
        string phy_uuid = "";

        pOper->QueryUuid(vna_uuid.c_str(), (*it).c_str(), EN_DB_PORT_TYPE_PHYSICAL, phy_uuid);
        pOper->QueryUuid(vna_uuid.c_str(), (bond_name.c_str()), EN_DB_PORT_TYPE_BOND, bond_uuid);

        info.SetVnaUuid(vna_uuid.c_str());
        info.SetBondUuid(bond_uuid.c_str());
        info.SetPhyUuid(phy_uuid.c_str());
        info.SetPhyName((*it).c_str());
        info.SetBondName(bond_name.c_str());
        info.SetBondMode(EN_BOND_MODE_802);
        info.SetIsCfg(1);
        info.SetIsSelect(1);
        ret = pOper->AddMap(info);
        if ( ret != 0 )
        {
            if (m_nBondDbgInf)
            {
                info.DbgShow();
            }
            VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s  %s] AddBondLacpMap4Cfg add lacp failed \n", bond_name.c_str(), (*it).c_str());
            //VNET_ASSERT(0);
        }
    }

    return 0;
}

int32 CPortMgr::AddBondBackupMap4Cfg(string &vna_uuid, string &bond_name, vector<string> &report, CDBOperateBond *pOper)
{
    int32 ret;

    vector<string>::iterator it = report.begin();
    for (; it != report.end(); ++it)
    {
        CDBBondMap info;
        string bond_uuid = "";
        string phy_uuid = "";

        pOper->QueryUuid(vna_uuid.c_str(), (*it).c_str(), EN_DB_PORT_TYPE_PHYSICAL, phy_uuid);
        pOper->QueryUuid(vna_uuid.c_str(), (bond_name).c_str(), EN_DB_PORT_TYPE_BOND, bond_uuid);

        info.SetVnaUuid(vna_uuid.c_str());
        info.SetBondUuid(bond_uuid.c_str());
        info.SetPhyUuid(phy_uuid.c_str());
        info.SetPhyName((*it).c_str());
        info.SetBondName(bond_name.c_str());
        info.SetBondMode(EN_BOND_MODE_BACKUP);
        info.SetIsCfg(1);
        info.SetIsSelect(1);
        ret = pOper->AddMap(info);
        if ( ret != 0 )
        {
            if (m_nBondDbgInf)
            {
                info.DbgShow();
            }
            VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s  %s] AddBondBackupMap4Cfg add backup failed \n",bond_name.c_str(), (*it).c_str());
            //VNET_ASSERT(0);
        }
    }

    return 0;
}

int32 CPortMgr::CompBondLacpMap4Cfg(string &vna_uuid, string &bond_name, vector<CDBBondMap> vBondMap, vector<string> &report, CDBOperateBond *pOper)
{
    int32 ret;

    vector<CDBBondMap>::iterator it = vBondMap.begin();
    for (; it != vBondMap.end(); ++it)
    {
        vector<string>::iterator it_cfg;
        for ( it_cfg = report.begin(); it_cfg !=  report.end(); ++it_cfg)
        {
            if (it->GetPhyName() == (*it_cfg))
            {
                //可能上报快于配置时写DB
                it->SetIsCfg(1);
                it->SetIsSelect(1);
                ret = pOper->ModifyMap(*it);
                if ( ret != 0 )
                {
                    if (m_nBondDbgInf)
                    {
                        (*it).DbgShow();
                    }
                    VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s %s] CompBondLacpMap4Cfg modify failed \n", bond_name.c_str(), it->GetPhyName().c_str());
                    //VNET_ASSERT(0);
                }

                break;
            }
        }

        //未找到相应成员口，删除
        if (it_cfg ==  report.end())
        {
            string phy_uuid = "";
            pOper->QueryUuid(vna_uuid.c_str(), (it->GetPhyName()).c_str(), EN_DB_PORT_TYPE_PHYSICAL, phy_uuid);
            ret = pOper->DelMap(vna_uuid.c_str(),it->GetBondUuid().c_str(),phy_uuid.c_str());
            if ( ret != 0 )
            {
                VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s %s %s] CompBondLacpMap4Cfg Del BondMap failed for lacp map \n", vna_uuid.c_str(),it->GetBondUuid().c_str(),phy_uuid.c_str());
                //VNET_ASSERT(0);
            }
        }
    }

    //下面开始操作增加的成员口
    vector<string>::iterator it_succ_add;
    for ( it_succ_add = report.begin(); it_succ_add !=  report.end(); ++it_succ_add)
    {
        vector<CDBBondMap>::iterator it_map_add;
        for (it_map_add = vBondMap.begin(); it_map_add != vBondMap.end(); ++it_map_add)
        {
            if (*it_succ_add == it_map_add->GetPhyName()) break;
        }

        if (it_map_add == vBondMap.end())
        {
            CDBBondMap info;
            string bond_uuid = "";
            string phy_uuid = "";

            pOper->QueryUuid(vna_uuid.c_str(), (*it_succ_add).c_str(), EN_DB_PORT_TYPE_PHYSICAL, phy_uuid);
            pOper->QueryUuid(vna_uuid.c_str(), (bond_name.c_str()), EN_DB_PORT_TYPE_BOND, bond_uuid);

            info.SetVnaUuid(vna_uuid.c_str());
            info.SetBondUuid(bond_uuid.c_str());
            info.SetPhyUuid(phy_uuid.c_str());
            info.SetPhyName((*it_succ_add).c_str());
            info.SetBondName(bond_name.c_str());
            info.SetBondMode(EN_BOND_MODE_802);
            info.SetIsCfg(1);
            info.SetIsSelect(1);
            ret = pOper->AddMap(info);
            if ( ret != 0 )
            {
                if (m_nBondDbgInf)
                {
                    info.DbgShow();
                }
                VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s %s] CompBondLacpMap4Cfg add lacp failed \n", bond_name.c_str(), (*it_succ_add).c_str());
                //VNET_ASSERT(0);
            }
        }
    }

    return 0;
}

int32 CPortMgr::CompBondBackupMap4Cfg(string &vna_uuid, string &bond_name, vector<CDBBondMap> vBondMap, vector<string> &report, CDBOperateBond *pOper)
{
    int32 ret;

    vector<CDBBondMap>::iterator it = vBondMap.begin();
    for (; it != vBondMap.end(); ++it)
    {
        vector<string>::iterator it_cfg;
        for ( it_cfg = report.begin(); it_cfg !=  report.end(); ++it_cfg)
        {
            if (it->GetPhyName() == *it_cfg)
            {
                //可能上报快于配置时写DB
                it->SetIsCfg(1);
                it->SetIsSelect(1);
                ret = pOper->ModifyMap(*it);
                if ( ret != 0 )
                {
                    if (m_nBondDbgInf)
                    {
                        (*it).DbgShow();
                    }
                    VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s %s] CompBondBackupMap4Cfg modify failed \n", bond_name.c_str(), it->GetPhyName().c_str());
                    //VNET_ASSERT(0);
                }

                break;
            }
        }

        if (it_cfg ==  report.end())
        {
            string phy_uuid;
            pOper->QueryUuid(vna_uuid.c_str(), (it->GetPhyName()).c_str(), EN_DB_PORT_TYPE_PHYSICAL, phy_uuid);
            ret = pOper->DelMap(vna_uuid.c_str(),it->GetBondUuid().c_str(),phy_uuid.c_str());
            if ( ret != 0 )
            {
                VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s %s %s] CompBondBackupMap4Cfg Del BondMap failed for backup map \n", vna_uuid.c_str(),it->GetBondUuid().c_str(),phy_uuid.c_str());
                //VNET_ASSERT(0);
            }
        }
    }

    vector<string>::iterator it_other_add;
    for ( it_other_add = report.begin(); it_other_add !=  report.end(); ++it_other_add)
    {
        vector<CDBBondMap>::iterator it_map_add;
        for (it_map_add = vBondMap.begin(); it_map_add != vBondMap.end(); ++it_map_add)
        {
            if (*it_other_add == it_map_add->GetPhyName()) break;
        }

        if ((it_map_add == vBondMap.end()))
        {
            CDBBondMap info;
            string bond_uuid = "";
            string phy_uuid = "";

            pOper->QueryUuid(vna_uuid.c_str(), (*it_other_add).c_str(), EN_DB_PORT_TYPE_PHYSICAL, phy_uuid);
            pOper->QueryUuid(vna_uuid.c_str(), (bond_name).c_str(), EN_DB_PORT_TYPE_BOND, bond_uuid);

            info.SetVnaUuid(vna_uuid.c_str());
            info.SetBondUuid(bond_uuid.c_str());
            info.SetPhyUuid(phy_uuid.c_str());
            info.SetPhyName((*it_other_add).c_str());
            info.SetBondName(bond_name.c_str());
            info.SetBondMode(EN_BOND_MODE_BACKUP);
            info.SetIsCfg(1);
            info.SetIsSelect(1);
            ret = pOper->AddMap(info);
            if ( ret != 0 )
            {
                if (m_nBondDbgInf)
                {
                    info.DbgShow();
                }
                VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s %s]  CompBondBackupMap4Report add backup failed \n", bond_name.c_str(), (*it_other_add).c_str());
                //VNET_ASSERT(0);
            }
        }
    }

    return 0;
}

//提供写bond数据库接口
int32 CPortMgr::AddBond2Db(string vna_uuid, string bond_name, vector<string> ifs, int32 mode, string &bond_uuid)
{
    int32  ret;

    bond_uuid = "";

    if ((vna_uuid.empty()) || (bond_name.empty()))
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::AddBond2Db input is empty \n");
        return -1;
    }

    CDBOperateBond * pOper = GetDbIBond();
    if ( NULL == pOper )
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::AddBond2Db CDBOperateBond failed \n");
        //VNET_ASSERT(0);
        return -1;
    }

    vector<CDbBondSummary> bond_outVInfo;
    ret = pOper->QuerySummary(vna_uuid.c_str(),bond_outVInfo);
    if ( ret != 0 )
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] AddBond2Db CDbBondSummary failed \n", vna_uuid.c_str());
        //VNET_ASSERT(0);
        return -1;
    }

    //new add
    if (bond_outVInfo.empty())
    {
        CDbBond info;
        info.SetVnaUuid(vna_uuid.c_str());
        info.SetPortType(EN_DB_PORT_TYPE_BOND);
        info.SetName(bond_name.c_str());

        //需要设置port的基本属性，底层也应该上报??
        info.SetIsOnline(1);
        info.SetIsPortCfg(1);
        info.SetIsConsistency(1);
        info.SetBondMode(mode);

        //LACP信息添加
        if (mode == EN_BOND_MODE_802)
        {
            info.SetIsHasLacpInfo(1);
        }

        ret = pOper->Add(info);
        if ( ret != 0 )
        {
            info.DbgShow();
            VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s %s] AddBond2Db add failed for null \n", vna_uuid.c_str(), bond_name.c_str());
            //VNET_ASSERT(0);
            return -1;
        }

        bond_uuid = info.GetUuid();

        if (mode == EN_BOND_MODE_802)
        {
            return AddBondLacpMap4Cfg(vna_uuid, bond_name, ifs, pOper);
        }

        if (mode == EN_BOND_MODE_BACKUP)
        {
            return AddBondBackupMap4Cfg(vna_uuid, bond_name, ifs, pOper);
        }

        return -1;
    }

    vector<CDbBondSummary>::iterator it = bond_outVInfo.begin();
    for (; it != bond_outVInfo.end(); ++it)
    {
        //modify，主要修改成员口
        if (it->GetBondName() == (bond_name))
        {
            CDbBond info;
            info.SetUuid((it->GetBondUuid()).c_str());
            ret = pOper->Query(info);
            if ( ret != 0 )
            {
                VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] AddBond2Db Query failed \n", (it->GetBondName()).c_str());
                //VNET_ASSERT(0);
                return -1;
            }

            info.SetIsPortCfg(1);
            ret = pOper->Modify(info);
            if ( ret != 0 )
            {
                //VNET_ASSERT(0);
                VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] AddBond2Db Modify failed \n", (it->GetBondUuid()).c_str());
            }

            vector<CDBBondMap> outVInfo;
            ret = pOper->QueryMapByBond(it->GetBondUuid().c_str(),outVInfo);
            if ( ret != 0 )
            {
                VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] AddBond2Db QueryMapByBond failed \n", it->GetBondUuid().c_str());
                //VNET_ASSERT(0);
                return -1;
            }

            bond_uuid = it->GetBondUuid();

            if (mode == EN_BOND_MODE_802)
            {
                if (outVInfo.empty())
                {
                    return AddBondLacpMap4Cfg(vna_uuid, bond_name, ifs, pOper);
                }
                else
                {
                    return CompBondLacpMap4Cfg(vna_uuid, bond_name, outVInfo, ifs, pOper);
                }
            }

            if (mode == EN_BOND_MODE_BACKUP)
            {
                if (outVInfo.empty())
                {
                    return AddBondBackupMap4Cfg(vna_uuid, bond_name, ifs, pOper);
                }
                else
                {
                    return CompBondBackupMap4Cfg(vna_uuid, bond_name, outVInfo, ifs, pOper);
                }
            }

            return -1;
        }
    }

    //new add
    if (it == bond_outVInfo.end())
    {
        CDbBond info;
        info.SetVnaUuid(vna_uuid.c_str());
        info.SetPortType(EN_DB_PORT_TYPE_BOND);
        info.SetName(bond_name.c_str());

        //需要设置port的基本属性，底层也应该上报??
        info.SetIsOnline(1);
        info.SetIsPortCfg(1);
        info.SetIsConsistency(1);
        info.SetBondMode(mode);

        //LACP信息添加
        if (mode == EN_BOND_MODE_802)
        {
            info.SetIsHasLacpInfo(1);
        }

        ret = pOper->Add(info);
        if ( ret != 0 )
        {
            info.DbgShow();
            VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s %s] AddBond2Db add failed \n", vna_uuid.c_str(), bond_name.c_str());
            //VNET_ASSERT(0);
            return -1;
        }

        bond_uuid = info.GetUuid();

        if (mode == EN_BOND_MODE_802)
        {
            return AddBondLacpMap4Cfg(vna_uuid, bond_name, ifs, pOper);
        }

        if (mode == EN_BOND_MODE_BACKUP)
        {
            return AddBondBackupMap4Cfg(vna_uuid, bond_name, ifs, pOper);
        }
    }

    return -1;
}

int32 CPortMgr::DelBondMem2Db(string vna_uuid, string bond_name, vector<string> ifs)
{
    int32  ret;

    if ((vna_uuid.empty()) || (bond_name.empty()))
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::DelBondMem2Db input is empty \n");
        return -1;
    }

    CDBOperateBond * pOper = GetDbIBond();
    if ( NULL == pOper )
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::DelBondMem2Db GetDbIBond failed \n");
        //VNET_ASSERT(0);
        return -1;
    }

    vector<CDbBondSummary> bond_outVInfo;
    ret = pOper->QuerySummary(vna_uuid.c_str(),bond_outVInfo);
    if ( ret != 0 )
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] DelBondMem2Db CDbBondSummary failed \n", vna_uuid.c_str());
        //VNET_ASSERT(0);
        return -1;
    }

    if (bond_outVInfo.empty())
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s %s] DelBondMem2Db BondName has not exit \n", vna_uuid.c_str(), bond_name.c_str());
        return -1;
    }

    vector<CDbBondSummary>::iterator it = bond_outVInfo.begin();
    for (; it != bond_outVInfo.end(); ++it)
    {
        //modify，主要修改成员口
        if (it->GetBondName() == (bond_name))
        {
            vector<string>::iterator it_map;
            for ( it_map = ifs.begin(); it_map !=  ifs.end(); ++it_map)
            {
                string phy_uuid = "";
                pOper->QueryUuid(vna_uuid.c_str(), (*it_map).c_str(), EN_DB_PORT_TYPE_PHYSICAL, phy_uuid);
                ret = pOper->DelMap(vna_uuid.c_str(),it->GetBondUuid().c_str(),phy_uuid.c_str());
                if ( ret != 0 )
                {
                    VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s %s %s] DelBondMem2Db Del BondMap failed \n", vna_uuid.c_str(),it->GetBondUuid().c_str(),phy_uuid.c_str());
                }
            }
        }
    }

    return 0;
}

int32 CPortMgr::DelBond2Db(string vna_uuid, string bond_name)
{
    int32  ret;

    if ((vna_uuid.empty()) || (bond_name.empty()))
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::DelBond2Db input is empty \n");
        return -1;
    }

    CDBOperateBond * pOper = GetDbIBond();
    if ( NULL == pOper )
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::DelBond2Db GetDbIBond failed \n");
        //VNET_ASSERT(0);
        return -1;
    }

    vector<CDbBondSummary> bond_outVInfo;
    ret = pOper->QuerySummary(vna_uuid.c_str(),bond_outVInfo);
    if ( ret != 0 )
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] DelBond2Db CDbBondSummary failed \n", vna_uuid.c_str());
        //VNET_ASSERT(0);
        return -1;
    }

    if (bond_outVInfo.empty())
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s %s] DelBond2Db BondName has not exit \n", vna_uuid.c_str(), bond_name.c_str());
        return -1;
    }

    vector<CDbBondSummary>::iterator it = bond_outVInfo.begin();
    for (; it != bond_outVInfo.end(); ++it)
    {
        //modify，主要修改成员口
        if (it->GetBondName() == (bond_name))
        {
            CDbBond info;
            info.SetUuid((it->GetBondUuid()).c_str());
            ret = pOper->Query(info);
            if ( ret != 0 )
            {
                VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] DelBond2Db Query failed \n", (it->GetBondUuid()).c_str());
                //VNET_ASSERT(0);
                return -1;
            }

            //查该bond下的map
            vector<CDBBondMap> outVInfo;
            ret = pOper->QueryMapByBond(it->GetBondUuid().c_str(),outVInfo);
            if ( ret != 0 )
            {
                VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] DelBond2Db QueryMapByBond failed \n", (it->GetBondUuid()).c_str());
                //VNET_ASSERT(0);
                return -1;
            }

            vector<CDBBondMap>::iterator it_map;
            for ( it_map = outVInfo.begin(); it_map !=  outVInfo.end(); ++it_map)
            {
                string phy_uuid;
                pOper->QueryUuid(vna_uuid.c_str(), (it_map->GetPhyName()).c_str(), EN_DB_PORT_TYPE_PHYSICAL, phy_uuid);
                ret = pOper->DelMap(vna_uuid.c_str(),it_map->GetBondUuid().c_str(),phy_uuid.c_str());
                if ( ret != 0 )
                {
                    VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s %s %s] DelBond2Db Del BondMap failed \n",vna_uuid.c_str(),it_map->GetBondUuid().c_str(),phy_uuid.c_str());
                }
            }

            ret = pOper->Del((it->GetBondUuid()).c_str());
            if ( ret != 0 )
            {
                VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] DelBond2Db Del failed \n", (it->GetBondUuid()).c_str());
                //VNET_ASSERT(0);
                return -1;
            }
        }
    }

    return 0;
}

int32 CPortMgr::GetBondMem(const string &vna_uuid, const string &bond_name, int32 &bond_mode, vector<string> &ifs)
{
    int32  ret;
    string bond_uuid;

    ifs.clear();

    if ((vna_uuid.empty()) || (bond_name.empty()))
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::GetBondMem input is empty \n");
        return -1;
    }

    CDBOperateBond * pOper = GetDbIBond();
    if ( NULL == pOper )
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::GetBondMem CDBOperateBond failed \n");
        //VNET_ASSERT(0);
        return -1;
    }

    ret = pOper->QueryUuid(vna_uuid.c_str(), (bond_name.c_str()), EN_DB_PORT_TYPE_BOND, bond_uuid);
    if (ret != 0)
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s  %s] GetBondMem QueryUuid failed \n", vna_uuid.c_str(), bond_name.c_str());
        //VNET_ASSERT(0);
        return -1;
    }

    //先获取bond模式
    CDbBond info;
    info.SetUuid(bond_uuid.c_str());
    ret = pOper->Query(info);
    if ( ret != 0 )
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s %s] GetBondMem Query failed \n", vna_uuid.c_str(), bond_name.c_str());
        return -1;
    }

    bond_mode = info.GetBondMode();

    //再获取bond map
    vector<CDBBondMap> outVInfo;
    ret = pOper->QueryMapByBond(bond_uuid.c_str(),outVInfo);
    if ( ret != 0 )
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] GetBondMem QueryMapByBond failed \n", bond_uuid.c_str());
        //VNET_ASSERT(0);
        return -1;
    }

    if (outVInfo.empty())
    {
        return 0;
    }

    vector<CDBBondMap>::iterator it = outVInfo.begin();
    for (; it != outVInfo.end(); ++it)
    {
        ifs.push_back(it->GetPhyName());
    }

    return 0;
}

int32 CPortMgr::SetBondCfg(const string &port_uuid)
{
    int32  ret;

    if (port_uuid.empty())
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::SetBondCfg input is empty \n");
        return -1;
    }

    CDBOperateBond * pOper = GetDbIBond();
    if ( NULL == pOper )
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::SetBondCfg GetDbIBond failed \n");
        //VNET_ASSERT(0);
        return -1;
    }

    CDbBond info;
    info.SetUuid(port_uuid.c_str());
    ret = pOper->Query(info);
    if ( ret != 0 )
    {
        if (m_nBondDbgInf)
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] SetBondCfg Query failed \n", port_uuid.c_str());
        //VNET_ASSERT(0);
        return -1;
    }

    info.SetIsOnline(0);
    info.SetIsPortCfg(2);
    info.SetIsConsistency(0);

    ret = pOper->Modify(info);
    if ( ret != 0 )
    {
        if (m_nBondDbgInf)
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] SetBondCfg Modify failed for Cfg \n", port_uuid.c_str());
        //VNET_ASSERT(0);
        return -1;
    }

    return 0;
}

int32 CPortMgr::GetPortUUID(const string &vna_uuid, const string &port_name, const int32 port_type, string &port_uuid)
{
    int32 ret;

    if ((vna_uuid.empty()) || (port_name.empty()))
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::GetPortUUID input is empty \n");
        return -1;
    }

    switch (port_type)
    {
    case EN_DB_PORT_TYPE_PHYSICAL:
    {
        CDBOperatePortPhysical * pOper = GetDbIPortPhy();
        if ( NULL == pOper )
        {
            return -1;
        }

        ret = pOper->QueryUuid(vna_uuid.c_str(), port_name.c_str(), EN_DB_PORT_TYPE_PHYSICAL, port_uuid);
        if ( ret != 0 )
        {
            return -1;
        }

        break;
    }
    case EN_DB_PORT_TYPE_BOND:
    {
        CDBOperateBond * pOper = GetDbIBond();
        if ( NULL == pOper )
        {
            return -1;
        }

        ret = pOper->QueryUuid(vna_uuid.c_str(), port_name.c_str(), EN_DB_PORT_TYPE_BOND, port_uuid);
        if (VNET_DB_IS_ITEM_NO_EXIST(ret))
        {
            return 0;
        }
        if ( ret != 0 )
        {
            return -1;
        }

        break;
    }
    case EN_DB_PORT_TYPE_SRIOVVF:
    {
        CDBOperatePortSriovVf * pOper = GetDbIPortSriovVf();
        if ( NULL == pOper )
        {
            return -1;
        }

        ret = pOper->QueryUuid(vna_uuid.c_str(), port_name.c_str(), EN_DB_PORT_TYPE_SRIOVVF, port_uuid);
        if ( ret != 0 )
        {
            return -1;
        }

        break;
    }
    case EN_DB_PORT_TYPE_KERNEL:
    {
        CDBOperatePortKernel * pOper = GetDbIPortKernel();
        if ( NULL == pOper )
        {
            return -1;
        }

        ret = pOper->QueryUuid(vna_uuid.c_str(), port_name.c_str(), EN_DB_PORT_TYPE_KERNEL, port_uuid);
        if ( ret != 0 )
        {
            return -1;
        }

        break;
    }
    case EN_DB_PORT_TYPE_UPLINKLOOP:
    {
        CDBOperatePortUplinkLoop * pOper = GetDbIPortUplinkLoop();
        if ( NULL == pOper )
        {
            return -1;
        }

        ret = pOper->QueryUuid(vna_uuid.c_str(), port_name.c_str(), EN_DB_PORT_TYPE_UPLINKLOOP, port_uuid);
        if ( ret != 0 )
        {
            return -1;
        }
        break;
    }
    default:
    {
        return -1;
        break;
    }
    }

    return 0;
}

//get port type by port name
int32 CPortMgr::GetPortType(const string &vna_uuid, const string &port_name, int32 &port_type, string &port_uuid)
{
    int32 ret;

    if ((vna_uuid.empty()) || (port_name.empty()))
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::GetPortType input is empty \n");
        return -1;
    }

    //通过查uuid反向判断端口是哪种类型的
    CDBOperatePortPhysical * pOperPhy = GetDbIPortPhy();
    if (NULL == pOperPhy)
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::GetPortType GetDbIPortPhy failed \n");
        //VNET_ASSERT(0);
        return -1;
    }
    else
    {
        ret = pOperPhy->QueryUuid(vna_uuid.c_str(), port_name.c_str(), EN_DB_PORT_TYPE_PHYSICAL, port_uuid);
        if (ret == 0)
        {
            port_type = EN_DB_PORT_TYPE_PHYSICAL;
            return 0;
        }
    }

    CDBOperateBond * pOperBond = GetDbIBond();
    if ( NULL == pOperBond )
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::GetPortType GetDbIBond failed \n");
        //VNET_ASSERT(0);
        return -1;
    }
    else
    {
        ret = pOperBond->QueryUuid(vna_uuid.c_str(), port_name.c_str(), EN_DB_PORT_TYPE_BOND, port_uuid);
        if (ret == 0)
        {
            port_type = EN_DB_PORT_TYPE_BOND;
            return 0;
        }
    }

    CDBOperatePortUplinkLoop * pOperUpLp = GetDbIPortUplinkLoop();
    if ( NULL == pOperUpLp )
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::GetPortType GetDbIPortUplinkLoop failed \n");
        //VNET_ASSERT(0);
        return -1;
    }
    else
    {
        ret = pOperBond->QueryUuid(vna_uuid.c_str(), port_name.c_str(), EN_DB_PORT_TYPE_UPLINKLOOP, port_uuid);
        if (ret == 0)
        {
            port_type = EN_DB_PORT_TYPE_UPLINKLOOP;
            return 0;
        }
    }

    CDBOperatePortKernel *pOperKer = GetDbIPortKernel();
    if ( NULL == pOperKer )
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::GetPortType GetDbIPortKernel failed \n");
        //VNET_ASSERT(0);
        return -1;
    }
    else
    {
        ret = pOperKer->QueryUuid(vna_uuid.c_str(), port_name.c_str(), EN_DB_PORT_TYPE_KERNEL, port_uuid);
        if (ret == 0)
        {
            port_type = EN_DB_PORT_TYPE_KERNEL;
            return 0;
        }
    }

    return -1;
}

int32 CPortMgr::IsSriovPort(const string &vna_uuid, const string &port_name)
{
    int32 ret;
    string port_uuid;

    if ((vna_uuid.empty()) || (port_name.empty()))
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::IsSriovPort input is empty \n");
        return -1;
    }

    //通过查uuid反向判断端口是哪种类型的
    CDBOperatePortPhysical * pOperPhy = GetDbIPortPhy();
    if (NULL == pOperPhy)
    {
        return -1;
    }
    else
    {
        ret = pOperPhy->QueryUuid(vna_uuid.c_str(), port_name.c_str(), EN_DB_PORT_TYPE_PHYSICAL, port_uuid);
        if (ret == 0)
        {
            //存在判断是否是sriov
            CDbPortPhysical info;
            info.SetUuid(port_uuid.c_str());
            ret = pOperPhy->Query(info);
            if ( ret != 0 )
            {
                return -1;
            }

            if (info.GetIsSriov())
            {
                return 0;
            }
        }
    }

    return -1;
}

int32 CPortMgr::GetSriovPortVfNum(string &vna_uuid, string &port_name, int32 port_type, int32 &vf_num)
{
    int32 ret;
    int32 bond_mode;
    string port_uuid;
    vector<string> ifs;

    int32 tmp = 0;

    if ((vna_uuid.empty()) || (port_name.empty()))
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::GetSriovPortVfNum input is empty \n");
        return -1;
    }

    CDBOperatePortSriovVf * pOper = GetDbIPortSriovVf();
    if ( NULL == pOper )
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::GetSriovPortVfNum GetDbIPortSriovVf failed \n");
        //VNET_ASSERT(0);
        return -1;
    }

    if (EN_DB_PORT_TYPE_PHYSICAL == port_type)
    {
        ret = GetPortUUID(vna_uuid, port_name, port_type, port_uuid);
        if (0 != ret)
        {
            VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s %s %d] GetSriovPortVfNum GetPortUUID failed", vna_uuid.c_str(),port_name.c_str(),port_type);
            return -1;
        }

        vector<CDbPortSummary> outVInfo;
        ret = pOper->QuerySummary(vna_uuid.c_str(), port_uuid.c_str(), outVInfo);
        if ( ret != 0 )
        {
            VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s %s] GetSriovPortVfNum QuerySummary failed \n", vna_uuid.c_str(), port_uuid.c_str());
            //VNET_ASSERT(0);
            return -1;
        }

        if (outVInfo.empty())
        {
            vf_num = 0;
            return 0;
        }

        vector<CDbPortSummary>::iterator it = outVInfo.begin();
        for (; it != outVInfo.end(); ++it)
        {
            tmp++;
        }

        vf_num = tmp;
        return 0;
    }

    if (EN_DB_PORT_TYPE_BOND == port_type)
    {
        ret = GetBondMem(vna_uuid, port_name, bond_mode, ifs);
        if (0 != ret)
        {
            VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s %s %d] GetSriovPortVfNum GetBondMem failed", vna_uuid.c_str(),port_name.c_str(),port_type);
            return -1;
        }

        vector<string>::iterator it_ifs = ifs.begin();
        for (; it_ifs != ifs.end(); ++it_ifs)
        {
            ret = GetPortUUID(vna_uuid, *it_ifs, EN_DB_PORT_TYPE_PHYSICAL, port_uuid);
            if (0 != ret)
            {
                VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s %s] GetSriovPortVfNum GetPortUUID failed", vna_uuid.c_str(), (*it_ifs).c_str());
                continue;
            }

            vector<CDbPortSummary> outVInfo;
            ret = pOper->QuerySummary(vna_uuid.c_str(), port_uuid.c_str(), outVInfo);
            if ( ret != 0 )
            {
                VNET_LOG(VNET_LOG_ERROR, "CPortMgr::GetSriovPortVfNum [%s %s] QuerySummary failed \n", vna_uuid.c_str(), port_uuid.c_str());
                continue;
            }

            if (outVInfo.empty())
            {
                continue;
            }

            vector<CDbPortSummary>::iterator it = outVInfo.begin();
            for (; it != outVInfo.end(); ++it)
            {
                tmp++;
            }
        }

        vf_num = tmp;
        return 0;
    }

    return -1;
}

int32 CPortMgr::GetSriovPortVfFreeNum(const string &vna_uuid, const string &port_name, const int32 port_type, int32 &freevf_num)
{
    int32 ret;
    int32 bond_mode;
    string port_uuid;
    vector<string> ifs;

    int32 tmp = 0;

    if ((vna_uuid.empty()) || (port_name.empty()))
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::GetSriovPortVfFreeNum input is empty \n");
        return -1;
    }

    CDBOperatePortSriovVf * pOper = GetDbIPortSriovVf();
    if ( NULL == pOper )
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::GetSriovPortVfFreeNum GetDbIPortSriovVf failed \n");
        //VNET_ASSERT(0);
        return -1;
    }

    if (EN_DB_PORT_TYPE_PHYSICAL == port_type)
    {
        ret = GetPortUUID(vna_uuid, port_name, port_type, port_uuid);
        if (0 != ret)
        {
            VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s %s %d] GetSriovPortVfFreeNum GetPortUUID failed \n", vna_uuid.c_str(),port_name.c_str(),port_type);
            return -1;
        }

        //获取该物理口下空闲VF
        vector<CDbPortFreeSriovVf> outVInfo;
        ret = pOper->QueryFree(port_uuid.c_str(),outVInfo);
        if ( ret != 0 )
        {
            VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] GetSriovPortVfFreeNum QueryFree failed \n", port_name.c_str());
            return -1;
        }

        if (outVInfo.empty())
        {
            freevf_num = 0;
            return 0;
        }

        vector<CDbPortFreeSriovVf>::iterator it = outVInfo.begin();
        for (; it != outVInfo.end(); ++it)
        {
            tmp++;
        }

        freevf_num = tmp;
        return 0;
    }

    if (EN_DB_PORT_TYPE_BOND == port_type)
    {
        ret = GetBondMem(vna_uuid, port_name, bond_mode, ifs);
        if (0 != ret)
        {
            VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s %s %d] GetSriovPortVfFreeNum GetBondMem failed \n", vna_uuid.c_str(),port_name.c_str(),port_type);
            return -1;
        }

        vector<string>::iterator it_ifs = ifs.begin();
        for (; it_ifs != ifs.end(); ++it_ifs)
        {
            ret = GetPortUUID(vna_uuid, *it_ifs, EN_DB_PORT_TYPE_PHYSICAL, port_uuid);
            if (0 != ret)
            {
                VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s %s] GetSriovPortVfFreeNum GetPortUUID failed", vna_uuid.c_str(), (*it_ifs).c_str());
                continue;
            }

            //获取该物理口下空闲VF
            vector<CDbPortFreeSriovVf> outVInfo;
            ret = pOper->QueryFree(port_uuid.c_str(),outVInfo);
            if ( ret != 0 )
            {
                VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] GetSriovPortVfFreeNum QueryFree failed", port_uuid.c_str());
                return -1;
            }

            if (outVInfo.empty())
            {
                continue;
            }

            vector<CDbPortFreeSriovVf>::iterator it = outVInfo.begin();
            for (; it != outVInfo.end(); ++it)
            {
                tmp++;
            }
        }

        freevf_num = tmp;
        return 0;
    }

    return -1;
}

//判断vna上一个物理口是否加入了bond
int32 CPortMgr::IsJoinBond(const string &strVnaUuid, const string &strPhyPortName, string &strBondName)
{
    int32 nRet;
    string strPortUuid;

    if ((strVnaUuid.empty()) || (strPhyPortName.empty()))
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::IsJoinBond input is empty \n");
        return -1;
    }

    CDBOperateBond * pOper = GetDbIBond();
    if ( NULL == pOper )
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::IsJoinBond GetDbIBond failed \n");
        //VNET_ASSERT(0);
        return -1;
    }

    if (0 != GetPortUUID(strVnaUuid, strPhyPortName, EN_DB_PORT_TYPE_PHYSICAL, strPortUuid))
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::IsJoinBond GetPortUUID failed [%s %s] \n", strVnaUuid.c_str(), strPhyPortName.c_str());
        return -1;
    }

    vector<CDBBondMap> map_outVInfo;
    vector<CDBBondMap>::iterator it;
    nRet = pOper->QueryMapByPhy(strPortUuid.c_str(),map_outVInfo);
    if ( nRet != 0 )
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::IsJoinBond QueryMapByPhy failed[%s] \n", strPortUuid.c_str());
        return -1;
    }

    if (map_outVInfo.empty())
    {
        return -1;
    }

    it = map_outVInfo.begin();
    strBondName = it->GetBondName();

    return 0;
}

//判断sriov口是否设置了环回
int32 CPortMgr::IsSriovPortLoop(const string &strVnaUuid, const string &strPhyPortName, int32 &nIsLoop)
{
    string strPortUuid;

    nIsLoop = 0;

    if ((strVnaUuid.empty()) || (strPhyPortName.empty()))
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::IsSriovPortLoop input is empty \n");
        return -1;
    }

    CDBOperatePortPhysical * pOperPhy = GetDbIPortPhy();
    if (NULL == pOperPhy)
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::IsSriovPortLoop GetDbIPortPhy failed \n");
        //VNET_ASSERT(0);
        return -1;
    }

    if (0 != GetPortUUID(strVnaUuid, strPhyPortName, EN_DB_PORT_TYPE_PHYSICAL, strPortUuid))
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::IsSriovPortLoop GetPortUUID failed [%s %s] \n", strVnaUuid.c_str(), strPhyPortName.c_str());
        return -1;
    }

    CDbPortPhysical info;
    info.SetUuid(strPortUuid.c_str());
    if (0 != pOperPhy->Query(info))
    {
        info.DbgShow();
        return -1;
    }

    if (info.GetIsloopCfg())
    {
        nIsLoop = 1;
    }

    return 0;
}


/*********************************web config********************************/
STATUS CPortMgr::ProcKernelPortReqMsg(const MessageFrame& message)
{
    CKerNalPortMsg cMsg;
    CKerNalPortMsg cAck;
    int32 result;

    int32 nRet = PORT_OPER_SUCCESS;

    cMsg.deserialize(message.data);
    MID   mid  = message.option.sender();
    //send ack to web when req fail
    MessageOption option(message.option.sender(), EV_VNETLIB_KERNALPORT_CFG_ACK, 0, 0);

    if (m_bOpenDbgInf)
    {
        cMsg.Print();
    }

    switch (cMsg.m_dwOper)
    {
    case EN_ADD_KERNALPORT:
    {
        //入参检查
        if ((cMsg.m_strVNAUUID.empty()) || (cMsg.m_strName.empty()) || (cMsg.m_strSwitchName.empty()) || (cMsg.m_strPGUUID.empty()))
        {
            VNET_LOG(VNET_LOG_ERROR, "CPortMgr::ProcKernelPortReqMsg port name is empty.\n");
            nRet = ERROR_PORT_NAME_EMPTY;
            goto PROC_PORT_ACK;
        }

        CWildcastMgr *pHandle = CWildcastMgr::GetInstance();

        //检查通配是否冲突
        if (pHandle)
        {
            if (0 == pHandle->CreateVPortIsConflict(cMsg.m_strVNAUUID, cMsg.m_strName, result))
            {
                if (TRUE == result)
                {
                    nRet = ERROR_PORT_WC_CONFLICT;
                    goto PROC_PORT_ACK;
                }
            }
        }

        nRet = DoKernelPortAddReq(cMsg, mid);
        if (SUCCESS == nRet)
        {
            return nRet;
        }
        nRet = ERROR_PORT_OPER_FAIL;
        break;
    }
    case EN_DEL_KERNALPORT:
    {
        //入参检查
        if (cMsg.m_strUUID.empty())
        {
            VNET_LOG(VNET_LOG_ERROR, "CPortMgr::ProcKernelPortReqMsg port name is empty.\n");
            nRet = ERROR_PORT_NAME_EMPTY;
            goto PROC_PORT_ACK;
        }

        nRet = DoKernelPortDelReq(cMsg, mid);
        if (SUCCESS == nRet)
        {
            return nRet;
        }
        nRet = ERROR_PORT_OPER_FAIL;
        break;
    }
    case EN_MOD_KERNALPORT:
    {
        nRet = DoKernelPortModReq(cMsg, mid);
        if (SUCCESS == nRet)
        {
            return nRet;
        }
        nRet = ERROR_PORT_OPER_FAIL;
        break;
    }
    default:
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::ProcKernelPortReqMsg: Msg operation code (%d) is invalid.\n", cMsg.m_dwOper);
        nRet = ERROR_PORT_OPERCODE_INVALID;
    }
    }

PROC_PORT_ACK:
    cAck.m_strName  = cMsg.m_strName;
    //需要吗？
    cAck.m_strUUID  = cMsg.m_strUUID;
    cAck.m_dwOper   = cMsg.m_dwOper;
    cAck.SetResult(nRet);
    cAck.SetResultinfo(GetVnetErrorInfo(nRet));
    return m_pMu->Send(cAck, option);
}

STATUS CPortMgr::ProcKernelPortAckMsg(const MessageFrame& message)
{
    int32 nRet = PORT_OPER_SUCCESS;
    string strPgId = "";
    string strSwId = "";
    string port_uuid = "";
    string strSwUuid = "";

    CPortCfgMsgToVNA cVNAMsg;
    cVNAMsg.deserialize(message.data);

    CKerNalPortMsg cAck;
    MID receiver = cVNAMsg.m_apisvr;
    MessageOption option(receiver, message.option.id(), 0, 0);

    if (m_bOpenDbgInf)
    {
        cVNAMsg.Print();
    }

    if (cVNAMsg.m_strName.empty())
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::ProcKernelPortAckMsg port name is empty.\n");
        nRet = ERROR_PORT_NAME_EMPTY;
        goto PORT_CFG_ACK;
    }

    switch (cVNAMsg.m_nOper)
    {
    case EN_ADD_KERNALPORT:
    {
        if (SUCCESS == cVNAMsg.m_nRetCode)
        {
            CSwitchMgr *pSwHandle = CSwitchMgr::GetInstance();
            if (pSwHandle)
            {
                pSwHandle->GetSwitchUUID(cVNAMsg.m_strVnaUuid, cVNAMsg.m_strSwitchName, strSwUuid);
            }

            //添加kernel口
            if (0 != AddKernelPort(cVNAMsg.m_strVnaUuid, EN_DB_PORT_TYPE_KERNEL, cVNAMsg.m_strName, cVNAMsg.m_nType, strSwUuid, cVNAMsg.m_strPgUuid/*strPgId*/, cVNAMsg.m_nIsDhcp))
            {
                nRet = ERROR_PORT_OPERDB_FAIL;
                break;
            }

            //添加kernel口的ip/mask
            if (0 != GetPortUUID(cVNAMsg.m_strVnaUuid, cVNAMsg.m_strName, EN_DB_PORT_TYPE_KERNEL, port_uuid))
            {
                nRet = ERROR_PORT_OPERDB_FAIL;
                break;
            }
            if (0 != AddPortIp(port_uuid, cVNAMsg.m_strName, cVNAMsg.m_strIP, cVNAMsg.m_strMask, 1, 1))
            {
                nRet = ERROR_PORT_OPERDB_FAIL;
                break;
            }
        }
        else
        {
            nRet = ERROR_PORT_OPER_FAIL;
        }
        break;
    }
    case EN_DEL_KERNALPORT:
    {
        if (SUCCESS == cVNAMsg.m_nRetCode)
        {
            if (0 != GetPortUUID(cVNAMsg.m_strVnaUuid, cVNAMsg.m_strName, EN_DB_PORT_TYPE_KERNEL, port_uuid))
            {
                nRet = ERROR_PORT_OPERDB_FAIL;
                break;
            }

            if (0 != DelPortIp(port_uuid, cVNAMsg.m_strName))
            {
                nRet = ERROR_PORT_OPERDB_FAIL;
                break;
            }

            if (0 != DelKernelPort(cVNAMsg.m_strVnaUuid, cVNAMsg.m_strName))
            {
                nRet = ERROR_PORT_OPERDB_FAIL;
                break;
            }
        }
        else
        {
            nRet = ERROR_PORT_OPER_FAIL;
        }		
        break;
    }
    case EN_MOD_KERNALPORT:
    {
        if (SUCCESS == cVNAMsg.m_nRetCode)
        {
            CPortGroupMgr *pPgHandle = CPortGroupMgr::GetInstance();
            if (pPgHandle)
            {
                pPgHandle->QueryPGByName("DEFAULT_KERNEL_PG", strPgId);
            }

            CSwitchMgr *pSwHandle = CSwitchMgr::GetInstance();
            if (pSwHandle)
            {
                pSwHandle->GetSwitchUUID(cVNAMsg.m_strVnaUuid, cVNAMsg.m_strSwitchName, strSwUuid);
            }

            if (0 != ModKernelPort(cVNAMsg.m_strVnaUuid, EN_DB_PORT_TYPE_KERNEL, cVNAMsg.m_strName, cVNAMsg.m_nType, strSwUuid, cVNAMsg.m_strPgUuid/*strPgId*/))
            {
                nRet = ERROR_PORT_OPERDB_FAIL;
                break;
            }

            if (0 != GetPortUUID(cVNAMsg.m_strVnaUuid, cVNAMsg.m_strName, EN_DB_PORT_TYPE_KERNEL, port_uuid))
            {
                nRet = ERROR_PORT_OPERDB_FAIL;
                break;
            }

            if (0 != ModPortIp(port_uuid, cVNAMsg.m_strName, cVNAMsg.m_strIP, cVNAMsg.m_strMask, 1, 1))
            {
                nRet = ERROR_PORT_OPERDB_FAIL;
                break;
            }
        }
        else
        {
            nRet = ERROR_PORT_OPER_FAIL;
        }		
        break;
    }
    default:
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::ProcKernelPortAckMsg: Msg operation code (%d) is invalid.\n", cVNAMsg.m_nOper);
        nRet = ERROR_PORT_OPER_FAIL;
    }
    }

PORT_CFG_ACK:

    //先考虑通配
    if (TRUE != (cVNAMsg.m_strWCTaskUuid.empty()))
    {
        CWildcastMgr *pHandle = CWildcastMgr::GetInstance();
        if (pHandle)
        {
            pHandle->WildcastTaskMsgAck(WILDCAST_TASK_TYPE_CREATE_VPORT, cVNAMsg.m_strWCTaskUuid, nRet);
            return SUCCESS;
        }
    }

    cAck.m_strName  = cVNAMsg.m_strName;
    cAck.m_strUUID  = cVNAMsg.m_strUuid;
    cAck.SetResult(nRet);
    cAck.m_dwOper    = cVNAMsg.m_nOper;
    //cAck.m_strDescription = cVNAMsg.m_strReqId;
    //cAck.m_nProgress= cVNAMsg.m_nProgress;
    cAck.SetResultinfo(GetVnetErrorInfo(nRet));
    return m_pMu->Send(cAck, option);
}

STATUS CPortMgr::DoKernelPortAddReq(const CKerNalPortMsg &cMsg, const MID &mid)
{
    CPortCfgMsgToVNA cVNAMsg;
    cVNAMsg.m_apisvr    = mid;

    cVNAMsg.m_strName   = cMsg.m_strName;
    cVNAMsg.m_strUuid   = cMsg.m_strUUID;
    cVNAMsg.m_strVnaUuid = cMsg.m_strVNAUUID;
    cVNAMsg.m_nType     = cMsg.m_nType;
    cVNAMsg.m_strIP     = cMsg.m_strip;
    cVNAMsg.m_strMask     = cMsg.m_strmask;
    cVNAMsg.m_nOper     = cMsg.m_dwOper;
    cVNAMsg.m_strSwitchName = cMsg.m_strSwitchName;
    cVNAMsg.m_strPgUuid = cMsg.m_strPGUUID;
    cVNAMsg.m_nIsDhcp = cMsg.m_isdhcp;

    //获取pg属性[mtu vlan]
    CPortGroupMgr *pPgHandle = CPortGroupMgr::GetInstance();
    if (pPgHandle)
    {
        CPortGroup cPGInfo;
        cPGInfo.SetUuid(cVNAMsg.m_strPgUuid);
        pPgHandle->GetPGDetail(cPGInfo);
        //判断模式
        if (cPGInfo.m_nSwitchPortMode)
        {
            cVNAMsg.m_nVlan = cPGInfo.m_nAccessVlanNum;
            cVNAMsg.m_nMtu  = cPGInfo.m_nMTU;
        }
        else
        {
            cVNAMsg.m_nVlan = cPGInfo.m_nTrunkNativeVlan;
            cVNAMsg.m_nMtu  = cPGInfo.m_nMTU;
        }
    }

    //根据交换名称获取上行口
    CSwitchMgr *pSwHandle = CSwitchMgr::GetInstance();
    if (pSwHandle)
    {
        pSwHandle->GetSwitchPort(cMsg.m_strVNAUUID, cMsg.m_strSwitchName, cVNAMsg.m_strUplinkPort);

        CPortGroup cUplinkPGInfo;
        if (0 == pSwHandle->GetSwitchPGInfo(cMsg.m_strSwitchName, cUplinkPGInfo))
        {
            if (((int32)cVNAMsg.m_nMtu) > cUplinkPGInfo.m_nMTU)
            {
                return ERROR;
            }
        }
    }

    //对上行口进行检查，如果是创建的loop口或者sriov还回，则返回错误
    int32 nPortType = 0;
    int32 nIsLoop = 0;
    string strPortUuid = "";

    if (0 != GetPortType(cMsg.m_strVNAUUID, cVNAMsg.m_strUplinkPort, nPortType, strPortUuid))
    {
        return ERROR;
    }

    if (EN_DB_PORT_TYPE_UPLINKLOOP == nPortType)
    {
        return ERROR;
    }

    if (EN_DB_PORT_TYPE_PHYSICAL == nPortType)
    {
        if (0 != IsSriovPortLoop(cMsg.m_strVNAUUID,  cVNAMsg.m_strUplinkPort, nIsLoop))
        {
            return ERROR;
        }

        if(1 == nIsLoop)
        {
            return ERROR;
        }
    }

    MID receiver;
    receiver._application = cMsg.m_strVNAUUID;
    receiver._process     = PROC_VNA;
    receiver._unit        = MU_VNA_CONTROLLER;
    MessageOption option_vna(receiver, EV_VNETLIB_KERNALPORT_CFG, 0, 0);

    return m_pMu->Send(cVNAMsg, option_vna);
}

STATUS CPortMgr::DoKernelPortDelReq(const CKerNalPortMsg &cMsg, const MID &mid)
{
    CDBOperatePortKernel *pOper = GetDbIPortKernel();
    if ( NULL == pOper )
    {
        //VNET_ASSERT(0);
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::DoKernelPortDelReq GetDbIPortKernel failed \n");
        return ERROR;
    }

    //根据uuid获取name
    CDbPortKernel info;
    info.SetUuid(cMsg.m_strUUID.c_str());
    if (0 != pOper->Query(info))
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[uuid %s] DoKernelPortDelReq Query failed \n", cMsg.m_strUUID.c_str());
        return ERROR;
    }

    if("dvs_kernel" == info.GetName())
    {
        //dvs_kernel 不容许删除
        return ERROR;
    }

    CPortCfgMsgToVNA cVNAMsg;
    cVNAMsg.m_apisvr    = mid;

    //cVNAMsg.m_strName   = cMsg.m_strName;
    cVNAMsg.m_strName   = info.GetName();
    cVNAMsg.m_strUuid   = cMsg.m_strUUID;
    cVNAMsg.m_strVnaUuid = cMsg.m_strVNAUUID;
    cVNAMsg.m_nType     = cMsg.m_nType;
    cVNAMsg.m_strIP     = cMsg.m_strip;
    cVNAMsg.m_strMask     = cMsg.m_strmask;
    cVNAMsg.m_nOper     = cMsg.m_dwOper;
    cVNAMsg.m_strSwitchName = cMsg.m_strSwitchName;
    cVNAMsg.m_strPgUuid = cMsg.m_strPGUUID;
    cVNAMsg.m_nIsDhcp = cMsg.m_isdhcp;

    MID receiver;
    receiver._application = cMsg.m_strVNAUUID;
    receiver._process     = PROC_VNA;
    receiver._unit        = MU_VNA_CONTROLLER;
    MessageOption option_vna(receiver, EV_VNETLIB_KERNALPORT_CFG, 0, 0);

    return m_pMu->Send(cVNAMsg, option_vna);
}

STATUS CPortMgr::DoKernelPortModReq(const CKerNalPortMsg &cMsg, const MID &mid)
{
    CPortCfgMsgToVNA cVNAMsg;
    cVNAMsg.m_apisvr    = mid;

    cVNAMsg.m_strName   = cMsg.m_strName;
    cVNAMsg.m_strUuid   = cMsg.m_strUUID;
    cVNAMsg.m_strVnaUuid = cMsg.m_strVNAUUID;
    cVNAMsg.m_nType     = cMsg.m_nType;
    cVNAMsg.m_strIP     = cMsg.m_strip;
    cVNAMsg.m_strMask     = cMsg.m_strmask;
    cVNAMsg.m_nOper     = cMsg.m_dwOper;
    cVNAMsg.m_strSwitchName = cMsg.m_strSwitchName;
    cVNAMsg.m_strPgUuid = cMsg.m_strPGUUID;
    cVNAMsg.m_nIsDhcp = cMsg.m_isdhcp;

    //根据交换名称获取上行口
    CSwitchMgr *pSwHandle = CSwitchMgr::GetInstance();
    if (pSwHandle)
    {
        pSwHandle->GetSwitchPort(cMsg.m_strVNAUUID, cMsg.m_strSwitchName, cVNAMsg.m_strUplinkPort);
    }

    MID receiver;
    receiver._application = cMsg.m_strVNAUUID;
    receiver._process     = PROC_VNA;
    receiver._unit        = MU_VNA_CONTROLLER;
    MessageOption option_vna(receiver, EV_VNETLIB_KERNALPORT_CFG, 0, 0);

    return m_pMu->Send(cVNAMsg, option_vna);
}



//增加VNA重启，配置信息下发，直接发消息给VNA
int32 CPortMgr::SendKernelPortCfgToVna(const string &strVNAUuid)
{
    int32 ret;
    T_KernelPortInfo tInfo;

    CDBOperatePortKernel * pOper = GetDbIPortKernel();
    if ( NULL == pOper )
    {
        //VNET_ASSERT(0);
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] SendKernelPortCfgToVna GetDbIPortKernel failed \n", strVNAUuid.c_str());
        return -1;
    }

    vector<CDbPortSummary> outVInfo;
    ret = pOper->QuerySummary(strVNAUuid.c_str(), outVInfo);
    if ( ret != 0 )
    {
        //VNET_ASSERT(0);
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] SendKernelPortCfgToVna QuerySummary failed \n", strVNAUuid.c_str());
        return -1;
    }

    vector<CDbPortSummary>::iterator it = outVInfo.begin();
    for (; it != outVInfo.end(); ++it)
    {
        //过滤安装kernel
        if ("dvs_kernel" == (it->GetName()))
        {
            continue;
        }

        CDbPortKernel info;
        info.SetUuid((it->GetUuid()).c_str());
        ret = pOper->Query(info);
        if ( ret != 0 )
        {
            continue;
        }

        CPortCfgMsgToVNA cVNAMsg;

        cVNAMsg.m_strName   = info.GetName();
        cVNAMsg.m_strVnaUuid = strVNAUuid;
        cVNAMsg.m_strUuid   = info.GetUuid();
        cVNAMsg.m_nOper     = EN_ADD_KERNALPORT;
        cVNAMsg.m_nType = PORT_TYPE_KERNEL;
        cVNAMsg.m_nIsDhcp    = info.GetIsDhcp();//同步时如果vna上没有kernel，只好再DHCP请求

        //根据交换名称获取上行口
        CSwitchMgr *pSwHandle = CSwitchMgr::GetInstance();
        if (pSwHandle)
        {
            if(0 != pSwHandle->GetSwitchName(info.GetVswitchUuid(), cVNAMsg.m_strSwitchName)) 
            {
                VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s  %s] SendKernelPortCfgToVna GetSwitchName failed \n", strVNAUuid.c_str(), info.GetVswitchUuid().c_str());
                continue;
            }
            if(0 != pSwHandle->GetSwitchPort(cVNAMsg.m_strVnaUuid, cVNAMsg.m_strSwitchName, cVNAMsg.m_strUplinkPort))
            {
                VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s  %s] SendKernelPortCfgToVna GetSwitchPort failed \n", strVNAUuid.c_str(), cVNAMsg.m_strSwitchName.c_str());
                continue;
            }
        }

        //获取ip/mask
        CDBOperatePortIp * pOper = GetDbIPortIp();
        if (pOper)
        {
            vector<CDbPortIp> outVInfo;
            vector<CDbPortIp>::iterator it;
            ret = pOper->QueryPortIp((cVNAMsg.m_strUuid).c_str(), outVInfo);
            if (0 == ret)
            {
                if (TRUE != outVInfo.empty())
                {
                    it = outVInfo.begin();
                    cVNAMsg.m_strIP   = (*it).GetIp();
                    cVNAMsg.m_strMask = (*it).GetMask();
                }
            }
        }

        //获取pg属性[mtu vlan]
        CPortGroupMgr *pPgHandle = CPortGroupMgr::GetInstance();
        if (pPgHandle)
        {
            CPortGroup cPGInfo;
            cPGInfo.SetUuid(info.GetPgUuid());
            pPgHandle->GetPGDetail(cPGInfo);
            //判断模式
            if (cPGInfo.m_nSwitchPortMode)
            {
                cVNAMsg.m_nVlan = cPGInfo.m_nAccessVlanNum;
                cVNAMsg.m_nMtu  = cPGInfo.m_nMTU;
            }
            else
            {
                cVNAMsg.m_nVlan = cPGInfo.m_nTrunkNativeVlan;
                cVNAMsg.m_nMtu  = cPGInfo.m_nMTU;
            }
        }

        MID receiver;
        receiver._application = strVNAUuid;
        receiver._process     = PROC_VNA;
        receiver._unit        = MU_VNA_CONTROLLER;
        MessageOption option_vna(receiver, TIMER_VNA_RESET_PORTINFO, 0, 0);

        tInfo.strVnaUuid = strVNAUuid;
        tInfo.port_name = cVNAMsg.m_strName;
        tInfo.nFlag = 1;
        m_vKernelPortInfo.push_back(tInfo);

        m_pMu->Send(cVNAMsg, option_vna);
    }

    return 0;
}

STATUS CPortMgr::ProcLoopBackPortReqMsg(const MessageFrame& message)
{
    CLoopPortMsg cMsg;
    CLoopPortMsg cAck;
    int32 ret;
    int32 result;

    int32 nRet = PORT_OPER_SUCCESS;

    cMsg.deserialize(message.data);
    MID   mid  = message.option.sender();
    //send ack to web when req fail
    MessageOption option(message.option.sender(), EV_VNETLIB_CFG_LOOPBACK_ACK, 0, 0);

    if (m_bOpenDbgInf)
    {
        cMsg.Print();
    }

    CWildcastMgr *pHandle = CWildcastMgr::GetInstance();

    //入参检查
    if (cMsg.m_strName.empty())
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::ProcLoopBackPortReqMsg port name.\n");
        nRet = ERROR_PORT_NAME_EMPTY;
        goto PROC_PORT_ACK;
    }

    if (NULL == pHandle)
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::ProcLoopBackPortReqMsg CWildcastMgr::GetInstance() is NULL.\n");
        nRet = ERROR_PORT_WC_CONFLICT;
        goto PROC_PORT_ACK;
    }

    switch (cMsg.m_dwOper)
    {
    case EN_ADD_LOOPPORT:
    {

        //检查通配是否冲突
        if (pHandle)
        {
            ret = pHandle->CreateVPortIsConflict(cMsg.m_strvnaUUID, cMsg.m_strName, result);
            if (0 == ret)
            {
                if (TRUE == result)
                {
                    nRet = ERROR_PORT_WC_CONFLICT;
                    goto PROC_PORT_ACK;
                }
            }
        }

        nRet = DoLoopBackPortAddReq(cMsg, mid);
        if (SUCCESS == nRet)
        {
            return nRet;
        }
        nRet = ERROR_PORT_OPER_FAIL;
        break;
    }
    case EN_DEL_LOOPPORT:
    {
        int32 ret;
        string port_uuid;

        CDBOperatePortUplinkLoop * pOper = GetDbIPortUplinkLoop();
        if ( NULL == pOper )
        {
            //VNET_ASSERT(0);
            VNET_LOG(VNET_LOG_ERROR, "CPortMgr::DoLoopBackPortDelReq GetDbIPortUplinkLoop failed \n");
            nRet = ERROR_PORT_OPERDB_FAIL;
            break;
        }

        //根据名称获取uuid
        ret = GetPortUUID(cMsg.m_strvnaUUID, cMsg.m_strName, EN_DB_PORT_TYPE_UPLINKLOOP, port_uuid);
        if (0 != ret)
        {
            nRet = ERROR_PORT_OPERDB_FAIL;
            break;
        }

        ret = pOper->CheckDel(port_uuid.c_str());
        if (0 != ret)
        {
            nRet = ERROR_PORT_OPERDB_FAIL;
            break;
        }

        nRet = DoLoopBackPortDelReq(cMsg, mid);
        if (SUCCESS == nRet)
        {
            return nRet;
        }
        nRet = ERROR_PORT_OPER_FAIL;
        break;
    }
    case EN_SET_LOOPPORT:
    {
        //检查通配是否冲突
        if (pHandle)
        {
            ret = pHandle->LoopbackIsConflict(cMsg.m_strvnaUUID, cMsg.m_strName, result);
            if (0 == ret)
            {
                if (TRUE == result)
                {
                    nRet = ERROR_PORT_WC_CONFLICT;
                    goto PROC_PORT_ACK;
                }
            }
        }

        nRet = DoLoopBackPortSetLoopReq(cMsg, mid);
        if (SUCCESS == nRet)
        {
            return nRet;
        }
        nRet = ERROR_PORT_OPER_FAIL;
        break;
    }
    case EN_UNSET_LOOPPORT:
    {
        //检查通配是否冲突
        if (pHandle)
        {
            ret = pHandle->LoopbackIsConflict(cMsg.m_strvnaUUID, cMsg.m_strName, result);
            if (0 == ret)
            {
                if (TRUE == result)
                {
                    nRet = ERROR_PORT_WC_CONFLICT;
                    goto PROC_PORT_ACK;
                }
            }
        }

        nRet = DoLoopBackPortUnSetLoopReq(cMsg, mid);
        if (SUCCESS == nRet)
        {
            return nRet;
        }
        nRet = ERROR_PORT_OPER_FAIL;
        break;
    }
    default:
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::ProcLoopBackPortReqMsg: Msg operation code (%d) is invalid.\n", cMsg.m_dwOper);
        nRet = ERROR_PORT_OPERCODE_INVALID;
    }
    }

PROC_PORT_ACK:
    cAck.m_strName  = cMsg.m_strName;
    //需要吗？
    cAck.m_strportUUID  = cMsg.m_strportUUID;
    cAck.m_dwOper   = cMsg.m_dwOper;
    cAck.SetResult(nRet);
    cAck.SetResultinfo(GetVnetErrorInfo(nRet));
    return m_pMu->Send(cAck, option);
}

STATUS CPortMgr::ProcLoopBackPortAckMsg(const MessageFrame& message)
{
    int32 nRet = PORT_OPER_SUCCESS;
    int32 nWildcastType = WILDCAST_TASK_TYPE_LOOPBACK;

    CPortCfgMsgToVNA cVNAMsg;
    cVNAMsg.deserialize(message.data);

    CLoopPortMsg cAck;
    MID receiver = cVNAMsg.m_apisvr;
    //MessageOption option(receiver, message.option.id(), 0, 0);
    MessageOption option(receiver, EV_VNETLIB_CFG_LOOPBACK_ACK, 0, 0);

    if (m_bOpenDbgInf)
    {
        cVNAMsg.Print();
    }

    if (cVNAMsg.m_strName.empty())
    {
        VNET_LOG(VNET_LOG_ERROR, "CSwitchMgr::ProcLoopBackPortAckMsg port name is empty.\n");
        nRet = ERROR_PORT_NAME_EMPTY;
        goto PORT_CFG_ACK;
    }

    switch (cVNAMsg.m_nOper)
    {
    case EN_ADD_LOOPPORT:
    {
        nWildcastType = WILDCAST_TASK_TYPE_CREATE_VPORT;
        if (SUCCESS == cVNAMsg.m_nRetCode)
        {
            if (0 != AddUplinkLoopPort(cVNAMsg.m_strVnaUuid, EN_DB_PORT_TYPE_UPLINKLOOP, cVNAMsg.m_strName))
            {
                nRet = ERROR_PORT_OPERDB_FAIL;
            }
        }
        else
        {
            nRet = ERROR_PORT_OPER_FAIL;
        }
        break;
    }
    case EN_DEL_LOOPPORT:
    {
        if (SUCCESS == cVNAMsg.m_nRetCode)
        {
            if (0 != DelUplinkLoopPort(cVNAMsg.m_strVnaUuid, cVNAMsg.m_strName))
            {
                nRet = ERROR_PORT_OPERDB_FAIL;
            }
        }
        else
        {
            nRet = ERROR_PORT_OPER_FAIL;
        }		
        break;
    }
    case EN_SET_LOOPPORT:
    {
        nWildcastType = WILDCAST_TASK_TYPE_LOOPBACK;
        if (SUCCESS == cVNAMsg.m_nRetCode)
        {
            if (0 != SetSriovPortLoop(cVNAMsg.m_strVnaUuid, cVNAMsg.m_strName))
            {
                nRet = ERROR_PORT_OPERDB_FAIL;
            }
        }
        else
        {
            nRet = ERROR_PORT_OPER_FAIL;
        }		
        break;
    }
    case EN_UNSET_LOOPPORT:
    {
        nWildcastType = WILDCAST_TASK_TYPE_LOOPBACK;
        if (SUCCESS == cVNAMsg.m_nRetCode)
        {
            if (0 != UnSetSriovPortLoop(cVNAMsg.m_strVnaUuid, cVNAMsg.m_strName))
            {
                nRet = ERROR_PORT_OPERDB_FAIL;
            }
        }
        else
        {
            nRet = ERROR_PORT_OPER_FAIL;
        }		
        break;
    }
    default:
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::ProcLoopBackPortAckMsg: Msg operation code (%d) is invalid.\n", cVNAMsg.m_nOper);
        nRet = ERROR_PORT_OPERCODE_INVALID;
    }
    }

PORT_CFG_ACK:

    //先考虑通配
    if (TRUE != (cVNAMsg.m_strWCTaskUuid.empty()))
    {
        CWildcastMgr *pHandle = CWildcastMgr::GetInstance();
        if (pHandle)
        {
            pHandle->WildcastTaskMsgAck(nWildcastType, cVNAMsg.m_strWCTaskUuid, nRet);
            return SUCCESS;
        }
    }

    cAck.m_strName  = cVNAMsg.m_strName;
    cAck.m_strportUUID  = cVNAMsg.m_strUuid;
    cAck.SetResult(nRet);
    cAck.m_dwOper   = cVNAMsg.m_nOper;
    cAck.SetResultinfo(GetVnetErrorInfo(nRet));
    //cAck.m_strDescription = cVNAMsg.m_strReqId;
    //cAck.m_nProgress= cVNAMsg.m_nProgress;
    return m_pMu->Send(cAck, option);
}

STATUS CPortMgr::DoLoopBackPortAddReq(const CLoopPortMsg &cMsg, const MID &mid)
{
    CPortCfgMsgToVNA cVNAMsg;
    cVNAMsg.m_apisvr    = mid;

    cVNAMsg.m_strName   = cMsg.m_strName;
    cVNAMsg.m_strUuid   = cMsg.m_strportUUID;
    cVNAMsg.m_strVnaUuid = cMsg.m_strvnaUUID;
    //cVNAMsg.m_nType     = cMsg.m_nType;
    //cVNAMsg.m_strIP     = cMsg.m_strip;
    //cVNAMsg.m_strMask     = cMsg.m_strmask;
    cVNAMsg.m_nOper     = cMsg.m_dwOper;

    MID receiver;
    receiver._application = cMsg.m_strvnaUUID;
    receiver._process     = PROC_VNA;
    receiver._unit        = MU_VNA_CONTROLLER;
    MessageOption option_vna(receiver, EV_VNETLIB_CFG_LOOPBACK, 0, 0);

    return m_pMu->Send(cVNAMsg, option_vna);
}

STATUS CPortMgr::DoLoopBackPortDelReq(const CLoopPortMsg &cMsg, const MID &mid)
{
    CPortCfgMsgToVNA cVNAMsg;
    cVNAMsg.m_apisvr    = mid;

    cVNAMsg.m_strName   = cMsg.m_strName;
    cVNAMsg.m_strUuid   = cMsg.m_strportUUID;
    cVNAMsg.m_strVnaUuid = cMsg.m_strvnaUUID;
    //cVNAMsg.m_nType     = cMsg.m_nType;
    //cVNAMsg.m_strIP     = cMsg.m_strip;
    //cVNAMsg.m_strMask     = cMsg.m_strmask;
    cVNAMsg.m_nOper     = cMsg.m_dwOper;

    MID receiver;
    receiver._application = cMsg.m_strvnaUUID;
    receiver._process     = PROC_VNA;
    receiver._unit        = MU_VNA_CONTROLLER;
    MessageOption option_vna(receiver, EV_VNETLIB_CFG_LOOPBACK, 0, 0);

    return m_pMu->Send(cVNAMsg, option_vna);
}

STATUS CPortMgr::DoLoopBackPortSetLoopReq(const CLoopPortMsg &cMsg, const MID &mid)
{
    int32 nRet;
    string strBondName;
    string strPortUuid;
    CPortCfgMsgToVNA cVNAMsg;

    CDBOperatePortPhysical * pOper = GetDbIPortPhy();
    if ( NULL == pOper )
    {
        //VNET_ASSERT(0);
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::DoLoopBackPortModReq GetDbIPortPhy failed \n");
        return ERROR;
    }

    vector<CDbPortSummary> outVInfo;
    nRet = pOper->QuerySummary(cMsg.m_strvnaUUID.c_str(), 1, outVInfo);
    if ( nRet != 0 )
    {
        //VNET_ASSERT(0);
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::DoLoopBackPortModReq QuerySummary failed \n");
        return ERROR;
    }

    if (outVInfo.empty())
    {
        return ERROR;
    }

    vector<CDbPortSummary>::iterator it = outVInfo.begin();
    for (; it != outVInfo.end(); ++it)
    {
        if (cMsg.m_strName == it->GetName())
        {
            break;
        }
    }

    //先判断是否是已经存在的sriov口
    if (it == outVInfo.end())
    {
        return ERROR;
    }

    //检查
    if (0 != GetPortUUID(cMsg.m_strvnaUUID, cMsg.m_strName, EN_DB_PORT_TYPE_PHYSICAL, strPortUuid))
    {
        return ERROR;
    }

    /*CDbPortPhysical info;
    info.SetUuid(strPortUuid.c_str());
    nRet = pOper->Query(info);
    if ( nRet != 0 )
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] DoLoopBackPortSetLoopReq Query failed \n", strPortUuid.c_str());
        return ERROR;
    }

    if(info.GetIsloopCfg())
    {
        return ERROR;
    }*/

    nRet = pOper->CheckSetSriovLoop(strPortUuid.c_str(), 1);
    if (nRet > 0)
    {
        return ERROR;
    }

    cVNAMsg.m_apisvr    = mid;
    cVNAMsg.m_strName   = cMsg.m_strName;
    cVNAMsg.m_strUuid   = cMsg.m_strportUUID;
    cVNAMsg.m_strVnaUuid = cMsg.m_strvnaUUID;
    //cVNAMsg.m_nType     = cMsg.m_nType;
    //cVNAMsg.m_strIP     = cMsg.m_strip;
    //cVNAMsg.m_strMask     = cMsg.m_strmask;
    cVNAMsg.m_nOper     = cMsg.m_dwOper;

    MID receiver;
    receiver._application = cMsg.m_strvnaUUID;
    receiver._process     = PROC_VNA;
    receiver._unit        = MU_VNA_CONTROLLER;
    MessageOption option_vna(receiver, EV_VNETLIB_CFG_LOOPBACK, 0, 0);

    return m_pMu->Send(cVNAMsg, option_vna);
}

STATUS CPortMgr::DoLoopBackPortUnSetLoopReq(const CLoopPortMsg &cMsg, const MID &mid)
{
    int32 nRet;
    string strBondName;
    string strPortUuid;
    CPortCfgMsgToVNA cVNAMsg;

    CDBOperatePortPhysical * pOper = GetDbIPortPhy();
    if ( NULL == pOper )
    {
        //VNET_ASSERT(0);
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::DoLoopBackPortModReq GetDbIPortPhy failed \n");
        return ERROR;
    }

    vector<CDbPortSummary> outVInfo;
    nRet = pOper->QuerySummary(cMsg.m_strvnaUUID.c_str(), 1, outVInfo);
    if ( nRet != 0 )
    {
        //VNET_ASSERT(0);
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::DoLoopBackPortModReq QuerySummary failed \n");
        return ERROR;
    }

    if (outVInfo.empty())
    {
        return ERROR;
    }

    vector<CDbPortSummary>::iterator it = outVInfo.begin();
    for (; it != outVInfo.end(); ++it)
    {
        if (cMsg.m_strName == it->GetName())
        {
            break;
        }
    }

    //先判断是否是已经存在的sriov口
    if (it == outVInfo.end())
    {
        return ERROR;
    }

    //检查
    if (0 != GetPortUUID(cMsg.m_strvnaUUID, cMsg.m_strName, EN_DB_PORT_TYPE_PHYSICAL, strPortUuid))
    {
        return ERROR;
    }

    /*CDbPortPhysical info;
    info.SetUuid(strPortUuid.c_str());
    nRet = pOper->Query(info);
    if ( nRet != 0 )
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] DoLoopBackPortUnSetLoopReq Query failed \n", strPortUuid.c_str());
        return ERROR;
    }

    if(!info.GetIsloopCfg())
    {
        return ERROR;
    }*/

    nRet = pOper->CheckSetSriovLoop(strPortUuid.c_str(), 0);
    if (nRet > 0)
    {
        return ERROR;
    }

    cVNAMsg.m_apisvr    = mid;
    cVNAMsg.m_strName   = cMsg.m_strName;
    cVNAMsg.m_strUuid   = cMsg.m_strportUUID;
    cVNAMsg.m_strVnaUuid = cMsg.m_strvnaUUID;
    //cVNAMsg.m_nType     = cMsg.m_nType;
    //cVNAMsg.m_strIP     = cMsg.m_strip;
    //cVNAMsg.m_strMask     = cMsg.m_strmask;
    cVNAMsg.m_nOper     = cMsg.m_dwOper;

    MID receiver;
    receiver._application = cMsg.m_strvnaUUID;
    receiver._process     = PROC_VNA;
    receiver._unit        = MU_VNA_CONTROLLER;
    MessageOption option_vna(receiver, EV_VNETLIB_CFG_LOOPBACK, 0, 0);

    return m_pMu->Send(cVNAMsg, option_vna);
}


//增加VNA重启，配置信息下发，直接发消息给VNA
int32 CPortMgr::SendLoopBackPortCfgToVna(const string &strVNAUuid)
{
    int32 ret;
    T_LoopPortInfo tInfo;

    //上行还回口
    CDBOperatePortUplinkLoop * pOper = GetDbIPortUplinkLoop();
    if ( NULL == pOper )
    {
        //VNET_ASSERT(0);
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] SendLoopBackPortCfgToVna GetDbIPortUplinkLoop failed \n", strVNAUuid.c_str());
        return -1;
    }

    vector<CDbPortSummary> outVInfo;
    ret = pOper->QuerySummary(strVNAUuid.c_str(), outVInfo);
    if ( ret != 0 )
    {
        //VNET_ASSERT(0);
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] SendLoopBackPortCfgToVna QuerySummary failed \n", strVNAUuid.c_str());
        return -1;
    }

    //loop口db异常，不同步sriov loop属性了

    vector<CDbPortSummary>::iterator it = outVInfo.begin();
    for (; it != outVInfo.end(); ++it)
    {
        CDbPortUplinkLoop info;
        info.SetUuid((it->GetUuid()).c_str());
        ret = pOper->Query(info);
        if ( ret != 0 )
        {
            continue;
        }

        CPortCfgMsgToVNA cVNAMsg;

        cVNAMsg.m_strName   = info.GetName();
        cVNAMsg.m_strUuid   = info.GetUuid();
        cVNAMsg.m_strVnaUuid = strVNAUuid;
        cVNAMsg.m_nOper     = EN_ADD_LOOPPORT;
        cVNAMsg.m_nType = PORT_TYPE_UPLINKLOOP;

        MID receiver;
        receiver._application = strVNAUuid;
        receiver._process     = PROC_VNA;
        receiver._unit        = MU_VNA_CONTROLLER;
        MessageOption option_vna(receiver, TIMER_VNA_RESET_PORTINFO, 0, 0);

        tInfo.strVnaUuid = strVNAUuid;
        tInfo.port_name = cVNAMsg.m_strName;
        tInfo.nFlag =1 ;
        m_vLoopPortInfo.push_back(tInfo);

        m_pMu->Send(cVNAMsg, option_vna);
    }

    //sriov设置还回口
    CDBOperatePortPhysical * pSriovOper = GetDbIPortPhy();
    if ( NULL == pSriovOper )
    {
        //VNET_ASSERT(0);
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] SendLoopBackPortCfgToVna GetDbIPortPhy failed \n", strVNAUuid.c_str());
        return -1;
    }

    vector<CDbPortSummary> Sriov_outVInfo;
    ret = pSriovOper->QuerySummary(strVNAUuid.c_str(), 1, Sriov_outVInfo);
    if ( ret != 0 )
    {
        //VNET_ASSERT(0);
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] SendLoopBackPortCfgToVna QuerySummary failed \n", strVNAUuid.c_str());
        return -1;
    }

    vector<CDbPortSummary>::iterator it_sriov = Sriov_outVInfo.begin();
    for (; it_sriov != Sriov_outVInfo.end(); ++it_sriov)
    {
        CDbPortPhysical info;
        info.SetUuid((it_sriov->GetUuid()).c_str());
        ret = pSriovOper->Query(info);
        if ( ret != 0 )
        {
            continue;
        }

        if (info.GetIsloopCfg())
        {
            CPortCfgMsgToVNA cVNAMsg;

            cVNAMsg.m_strName   = info.GetName();
            cVNAMsg.m_strUuid   = info.GetUuid();
            cVNAMsg.m_strVnaUuid = strVNAUuid;
            cVNAMsg.m_nOper     = EN_SET_LOOPPORT;
            cVNAMsg.m_nType = PORT_TYPE_PHYSICAL;

            MID receiver;
            receiver._application = strVNAUuid;
            receiver._process     = PROC_VNA;
            receiver._unit        = MU_VNA_CONTROLLER;
            MessageOption option_vna(receiver, TIMER_VNA_RESET_PORTINFO, 0, 0);

            tInfo.strVnaUuid = strVNAUuid;
            tInfo.port_name = cVNAMsg.m_strName;
            tInfo.nFlag = 1;
            m_vLoopPortInfo.push_back(tInfo);

            m_pMu->Send(cVNAMsg, option_vna);
        }
    }

    return 0;
}

int32 CPortMgr::SendPortCfgToVna(const string &strVNAUuid)
{
    int32 ret;

    //loop port
    ret = SendLoopBackPortCfgToVna(strVNAUuid);
    if (0 != ret)
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] SendPortCfgToVna SendLoopBackPortCfgToVna failed \n", strVNAUuid.c_str());
    }

    //kernel port
    ret = SendKernelPortCfgToVna(strVNAUuid);
    if (0 != ret)
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] SendPortCfgToVna SendKernelPortCfgToVna failed \n", strVNAUuid.c_str());
    }

    return 0;
}

int32 CPortMgr::ProcPortCfgToVna(void)
{
    string strPortUuid = "";
    int32  nKrFlag = 0;
    int32  nLpFlag = 0;
    int32  nRet = 0;

    vector <T_KernelPortInfo> ::iterator it_kr = m_vKernelPortInfo.begin();
    for (; it_kr != m_vKernelPortInfo.end(); ++it_kr)
    {
        if (1 == it_kr->nFlag)
        {
            nKrFlag = 1;

            CDBOperatePortKernel * pKrOper = GetDbIPortKernel();
            if ( NULL == pKrOper )
            {
                continue;
            }

            if (0 == GetPortUUID(it_kr->strVnaUuid, it_kr->port_name, EN_DB_PORT_TYPE_KERNEL, strPortUuid))
            {
                CPortCfgMsgToVNA cVNAMsg;

                CDbPortKernel info;
                info.SetUuid(strPortUuid.c_str());
                nRet = pKrOper->Query(info);
                if ( nRet != 0 )
                {
                    continue;
                }

                cVNAMsg.m_strName   = it_kr->port_name;
                cVNAMsg.m_strVnaUuid = it_kr->strVnaUuid;
                cVNAMsg.m_strUuid   = strPortUuid;
                cVNAMsg.m_nOper     = EN_ADD_KERNALPORT;
                cVNAMsg.m_nType = PORT_TYPE_KERNEL;
                cVNAMsg.m_nIsDhcp    = info.GetIsDhcp();

                //根据交换名称获取上行口
                CSwitchMgr *pSwHandle = CSwitchMgr::GetInstance();
                if (pSwHandle)
                {
                    if(0 != pSwHandle->GetSwitchName(info.GetVswitchUuid(), cVNAMsg.m_strSwitchName))
                    {
                        continue;
                    }
                    if(0 != pSwHandle->GetSwitchPort(cVNAMsg.m_strVnaUuid, cVNAMsg.m_strSwitchName, cVNAMsg.m_strUplinkPort))
                    {
                        continue;
                    }
                }

                //获取ip/mask
                CDBOperatePortIp * pOper = GetDbIPortIp();
                if (pOper)
                {
                    vector<CDbPortIp> outVInfo;
                    vector<CDbPortIp>::iterator it;
                    nRet = pOper->QueryPortIp((cVNAMsg.m_strUuid).c_str(), outVInfo);
                    if (0 == nRet)
                    {
                        if (TRUE != outVInfo.empty())
                        {
                            it = outVInfo.begin();
                            cVNAMsg.m_strIP   = (*it).GetIp();
                            cVNAMsg.m_strMask = (*it).GetMask();
                        }
                    }
                }

                //获取pg属性[mtu vlan]
                CPortGroupMgr *pPgHandle = CPortGroupMgr::GetInstance();
                if (pPgHandle)
                {
                    CPortGroup cPGInfo;
                    cPGInfo.SetUuid(info.GetPgUuid());
                    pPgHandle->GetPGDetail(cPGInfo);
                    //判断模式
                    if (cPGInfo.m_nSwitchPortMode)
                    {
                        cVNAMsg.m_nVlan = cPGInfo.m_nAccessVlanNum;
                        cVNAMsg.m_nMtu  = cPGInfo.m_nMTU;
                    }
                    else
                    {
                        cVNAMsg.m_nVlan = cPGInfo.m_nTrunkNativeVlan;
                        cVNAMsg.m_nMtu  = cPGInfo.m_nMTU;
                    }
                }


                MID receiver;
                receiver._application = it_kr->strVnaUuid;
                receiver._process     = PROC_VNA;
                receiver._unit        = MU_VNA_CONTROLLER;
                MessageOption option_vna(receiver, TIMER_VNA_RESET_PORTINFO, 0, 0);

                m_pMu->Send(cVNAMsg, option_vna);
            }
            else
            {
                //查不到uuid，认为不存在了，不再需要同步了
                it_kr->nFlag = 0;
            }
        }
    }

    vector <T_LoopPortInfo> ::iterator it_lp = m_vLoopPortInfo.begin();
    for (; it_lp != m_vLoopPortInfo.end(); ++it_lp)
    {
        if (1 == it_lp->nFlag)
        {
            nLpFlag = 1;

            //区分一下是loop还回口还是sriov设置的还回口
            if (0 == (it_lp->port_name.find("loop",0) ))
            {
                if (0 == GetPortUUID(it_lp->strVnaUuid, it_lp->port_name, EN_DB_PORT_TYPE_UPLINKLOOP, strPortUuid))
                {
                    CPortCfgMsgToVNA cVNAMsg;

                    cVNAMsg.m_strName   = it_lp->port_name;
                    cVNAMsg.m_strVnaUuid = it_lp->strVnaUuid;
                    cVNAMsg.m_nOper     = EN_ADD_LOOPPORT;
                    cVNAMsg.m_nType = PORT_TYPE_UPLINKLOOP;
                    MID receiver;
                    receiver._application = it_lp->strVnaUuid;
                    receiver._process     = PROC_VNA;
                    receiver._unit        = MU_VNA_CONTROLLER;
                    MessageOption option_vna(receiver, TIMER_VNA_RESET_PORTINFO, 0, 0);

                    m_pMu->Send(cVNAMsg, option_vna);
                }
                else
                {
                    it_lp->nFlag = 0;
                }
            }
            else
            {
                CDBOperatePortPhysical * pPhyOper = GetDbIPortPhy();
                if ( NULL == pPhyOper )
                {
                    continue;
                }

                if (0 == GetPortUUID(it_lp->strVnaUuid, it_lp->port_name, EN_DB_PORT_TYPE_PHYSICAL, strPortUuid))
                {

                    CDbPortPhysical info;
                    info.SetUuid(strPortUuid.c_str());
                    if (0 == pPhyOper->Query(info))
                    {
                        continue;
                    }

                    if (info.GetIsloopCfg())
                    {
                        CPortCfgMsgToVNA cVNAMsg;

                        cVNAMsg.m_strName   = it_lp->port_name;
                        cVNAMsg.m_strVnaUuid = it_lp->strVnaUuid;
                        cVNAMsg.m_nOper     = EN_SET_LOOPPORT;
                        cVNAMsg.m_nType = PORT_TYPE_PHYSICAL;
                        MID receiver;
                        receiver._application = it_lp->strVnaUuid;
                        receiver._process     = PROC_VNA;
                        receiver._unit        = MU_VNA_CONTROLLER;
                        MessageOption option_vna(receiver, TIMER_VNA_RESET_PORTINFO, 0, 0);

                        m_pMu->Send(cVNAMsg, option_vna);
                    }
                }
                else
                {
                    it_lp->nFlag = 0;
                }
            }
        }
    }

    if ((0 == nKrFlag) && (0 == nLpFlag ))
    {
        m_pMu->KillTimer(tTimerID);
        tTimerID = INVALID_TIMER_ID;
        m_vKernelPortInfo.clear();
        m_vLoopPortInfo.clear();
    }

    return 0;
}

int32 CPortMgr::ProcPortCfgToVnaAck(const MessageFrame& message)
{
    string strPortUuid;
    //int32 nRet;

    CPortCfgMsgToVNA cVNAMsg;
    cVNAMsg.deserialize(message.data);

    string strVNAUuid = cVNAMsg.m_strVnaUuid;
    string strPortName = cVNAMsg.m_strName;

    if (m_bOpenDbgInf)
    {
        cVNAMsg.Print();
    }

    vector <T_KernelPortInfo> ::iterator it_kr = m_vKernelPortInfo.begin();
    for (; it_kr != m_vKernelPortInfo.end(); ++it_kr)
    {
        if ((it_kr->strVnaUuid == strVNAUuid) && (it_kr->port_name ==strPortName ) && (cVNAMsg.m_nRetCode == 0))
        {
            /*CDBOperatePortKernel * pKrOper = GetDbIPortKernel();
            if ( NULL == pKrOper )
            {
                break;
            }

            if (0 == GetPortUUID(it_kr->strVnaUuid, it_kr->port_name, EN_DB_PORT_TYPE_KERNEL, strPortUuid))
            {
                CDbPortKernel info;
                info.SetUuid(strPortUuid.c_str());
                nRet = pKrOper->Query(info);
                if ( nRet == 0 )
                {
                    if (info.GetIsOnline())
                    {
                        it_kr->nFlag = 0;
                    }
                }
            }*/

            it_kr->nFlag = 0;
            //m_vKernelPortInfo.erase(it_kr);			

            return 0;
        }
    }

    vector <T_LoopPortInfo> ::iterator it_lp = m_vLoopPortInfo.begin();
    for (; it_lp != m_vLoopPortInfo.end(); ++it_lp)
    {
        if ((it_lp->strVnaUuid == strVNAUuid) && (it_lp->port_name ==strPortName ) && (cVNAMsg.m_nRetCode == 0))
        {
            /*CDBOperatePortUplinkLoop * pOper = GetDbIPortUplinkLoop();
            if ( NULL == pOper )
            {
                break;
            }

            if (0 == GetPortUUID(it_lp->strVnaUuid, it_lp->port_name, EN_DB_PORT_TYPE_UPLINKLOOP, strPortUuid))
            {
                CDbPortUplinkLoop info;
                info.SetUuid(strPortUuid.c_str());
                nRet = pOper->Query(info);
                if ( nRet == 0 )
                {
                    if (info.GetIsOnline())
                    {
                        it_lp->nFlag = 0;
                    }
                }
            }


            if (0 == GetPortUUID(it_lp->strVnaUuid, it_lp->port_name, EN_DB_PORT_TYPE_PHYSICAL, strPortUuid))
            {
                CDbPortUplinkLoop info;
                info.SetUuid(strPortUuid.c_str());
                nRet = pOper->Query(info);
                if ( nRet == 0 )
                {
                    if (info.GetIsOnline())
                    {
                        it_lp->nFlag = 0;
                    }
                }
            }*/

            it_lp->nFlag = 0;
            //m_vLoopPortInfo.erase(it_lp);		

            return 0;
        }
    }

    return 0;
}

int32 CPortMgr::SetPortCfgToVnaTimer(const string &strVnaUuid)
{
    int32 nRet;

    if (strVnaUuid.empty())
    {
        return 0;
    }

    //先设置离线
    SetPortCfgOffline(strVnaUuid);

    //直接下发
    nRet = SendPortCfgToVna(strVnaUuid);
    if (!nRet)
    {
        //所有vna同步使用同一个定时器，如果先前有删除，重新创建
        if (INVALID_TIMER_ID != tTimerID)
        {
            m_pMu->KillTimer(tTimerID);
            tTimerID = INVALID_TIMER_ID;
        }

        TimeOut  tTmOut;
        tTmOut.duration = 15000;
        tTmOut.msgid = TIMER_VNA_RESET_PORTINFO;
        tTmOut.type = LOOP_TIMER;
        tTmOut.arg = NULL;
        tTmOut.str_arg = strVnaUuid;

        tTimerID = m_pMu->CreateTimer();
        if (INVALID_TIMER_ID == tTimerID)
        {
            VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] SetPortCfgToVnaTimer Create PortTimerId failed.\n", strVnaUuid.c_str());
            return -1;
        }

        //设置定时器
        if (SUCCESS != m_pMu->SetTimer(tTimerID, tTmOut))
        {
            m_pMu->KillTimer(tTimerID);
            tTimerID = INVALID_TIMER_ID;
            VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] SetPortCfgToVnaTimer Set tPortTimerId failed.\n", strVnaUuid.c_str());
            return -1;
        }

        return 0;
    }

    VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] SetPortCfgToVnaTimer SendPortCfgToVna failed.\n", strVnaUuid.c_str());
    return -1;
}


//vna重启，port管理设置loop和kernel短暂离线
int32 CPortMgr::SetPortCfgOffline(const string vna_uuid)
{
    int32 ret;

    CDBOperatePortKernel * pOperKernel = GetDbIPortKernel();
    if ( NULL == pOperKernel )
    {
        //VNET_ASSERT(0);
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] SetPortCfgOffline GetDbIPortKernel failed \n", vna_uuid.c_str());
        return -1;
    }

    CDBOperatePortUplinkLoop * pOperLoop = GetDbIPortUplinkLoop();
    if ( NULL == pOperLoop )
    {
        //VNET_ASSERT(0);
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] SetPortCfgOffline GetDbIPortUplinkLoop failed \n", vna_uuid.c_str());
        return -1;
    }

    vector<CDbPortSummary> outVInfo_Kernel;
    ret = pOperKernel->QuerySummary(vna_uuid.c_str(), outVInfo_Kernel);
    if (ret != 0)
    {
        //VNET_ASSERT(0);
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] SetPortCfgOffline Kernel QuerySummary failed \n", vna_uuid.c_str());
        return -1;
    }

    vector<CDbPortSummary> outVInfo_Loop;
    ret = pOperLoop->QuerySummary(vna_uuid.c_str(), outVInfo_Loop);
    if (ret != 0)
    {
        //VNET_ASSERT(0);
        VNET_LOG(VNET_LOG_ERROR, "CPortMgr::[%s] SetPortCfgOffline Loop QuerySummary failed \n", vna_uuid.c_str());
        return -1;
    }

    //修改kernel
    vector<CDbPortSummary>::iterator it_kernel = outVInfo_Kernel.begin();
    for (; it_kernel != outVInfo_Kernel.end(); ++it_kernel)
    {
        CDbPortKernel info;
        info.SetUuid(it_kernel->GetUuid().c_str());
        ret = pOperKernel->Query(info);
        if ( ret != 0 )
        {
            if (m_nKrPortDbgInf)
            {
                info.DbgShow();
            }
            //VNET_ASSERT(0);
            VNET_LOG(VNET_LOG_ERROR, "CPortMgr::SetPortCfgOffline Kernel Query failed \n");
            continue;
        }

        info.SetIsOnline(0);
        info.SetIsConsistency(0);
        info.SetIsPortCfg(2);
        ret = pOperKernel->Modify(info);
        if ( ret != 0 )
        {
            if (m_nKrPortDbgInf)
            {
                info.DbgShow();
            }
            VNET_LOG(VNET_LOG_ERROR, "CPortMgr::SetPortCfgOffline Kernel Modify failed [%s] \n", info.GetName().c_str());
            //VNET_ASSERT(0);
        }
    }

    //修改loop
    vector<CDbPortSummary>::iterator it_loop = outVInfo_Loop.begin();
    for (; it_loop != outVInfo_Loop.end(); ++it_loop)
    {
        CDbPortUplinkLoop info;
        info.SetUuid(it_loop->GetUuid().c_str());
        ret = pOperLoop->Query(info);
        if ( ret != 0 )
        {
            if (m_nUplinkLpDbgInf)
            {
                info.DbgShow();
            }
            VNET_LOG(VNET_LOG_ERROR, "CPortMgr::SetPortCfgOffline Loop Query failed \n");
            //VNET_ASSERT(0);
            continue;
        }

        info.SetIsOnline(0);
        info.SetIsConsistency(0);
        info.SetIsPortCfg(2);
        ret = pOperLoop->Modify(info);
        if ( ret != 0 )
        {
            if (m_nUplinkLpDbgInf)
            {
                info.DbgShow();
            }
            //VNET_ASSERT(0);
            VNET_LOG(VNET_LOG_ERROR, "CPortMgr::SetPortCfgOffline Loop Modify failed [%s] \n", info.GetName().c_str());
        }
    }

    //vna离线时，如果同步的有此vna的记录删除，等上线时候重置。
    vector <T_KernelPortInfo>::iterator it_ker = m_vKernelPortInfo.begin();
    for (;it_ker != m_vKernelPortInfo.end();)
    {
        if(vna_uuid == it_ker->strVnaUuid)
        {
            it_ker = m_vKernelPortInfo.erase(it_ker);
            continue ;
        }

        ++it_ker;		
    }

    vector <T_LoopPortInfo>::iterator it_lp = m_vLoopPortInfo.begin();
    for (;it_lp != m_vLoopPortInfo.end();)
    {
        if(vna_uuid == it_lp->strVnaUuid)
        {
            it_lp = m_vLoopPortInfo.erase(it_lp);
            continue ;
        }

        ++it_lp;
    }

    return 0;
}


//PORT 通配考虑
//给通配提供接口
//外层赋值vnauuid、name、oper、标识通配的id
int32 CPortMgr::ProcKernelPortWildcast(const CPortCfgMsgToVNA &msg)
{
    MID receiver;
    receiver._application = msg.m_strVnaUuid;
    receiver._process     = PROC_VNA;
    receiver._unit        = MU_VNA_CONTROLLER;
    MessageOption option_vna(receiver, EV_VNETLIB_KERNALPORT_CFG, 0, 0);

    return m_pMu->Send(msg, option_vna);
}

int32 CPortMgr::ProcLoopBackPortWildcast(const CPortCfgMsgToVNA &msg)
{
    MID receiver;
    receiver._application = msg.m_strVnaUuid;
    receiver._process     = PROC_VNA;
    receiver._unit        = MU_VNA_CONTROLLER;
    MessageOption option_vna(receiver, EV_VNETLIB_CFG_LOOPBACK, 0, 0);

    return m_pMu->Send(msg, option_vna);
}

void CPortMgr::DbgShowPortCfg(void)
{
    vector <T_KernelPortInfo>::const_iterator it_ker = m_vKernelPortInfo.begin();
    for (;it_ker != m_vKernelPortInfo.end();it_ker++)
    {
        cout << it_ker->strVnaUuid <<  ":" <<it_ker->port_name << ":" << it_ker->nFlag << endl;
    }

    vector <T_LoopPortInfo>::const_iterator it_lp = m_vLoopPortInfo.begin();
    for (;it_lp != m_vLoopPortInfo.end();it_lp++)
    {
        cout << it_lp->strVnaUuid <<  ":" <<it_lp->port_name << ":" << it_lp->nFlag << endl;
    }

    cout << "Timer:" <<tTimerID  << endl;
	
}

//test code
void TestDbPortInfo()
{
    MessageFrame message;

    CMessageVNAPortInfoReport msg;
    CPhyPortReport info;

    msg._phy_port.clear();

    info._basic_info._name = "lhx_sriov_05";
    info._basic_info._port_type = 0;
    info._basic_info._state = 1;

    info._is_sriov = 1;

    CSriovPortReport sriov;
    sriov._pci = "0000000";
    sriov._vf  = "0000000";
    info._sriov_port.push_back(sriov);

    CSriovPortReport sriov_b;
    sriov_b._pci = "1111111";
    sriov_b._vf  = "1111111";
    info._sriov_port.push_back(sriov_b);

    msg._phy_port.push_back(info);

    msg._vna_id = "20130301";

    CPortMgr *pHandle = CPortMgr::GetInstance();
    if (pHandle)
    {
        pHandle->DoPortInfoPrepare(msg);
    }
    return;
}

DEFINE_DEBUG_FUNC(TestDbPortInfo);

void TestPortUUID()
{

    string port_uuid;
    string vna_uuid = "3b9746b5-11c5-41a4-a53e-c525f7790e96";
    string port_name = "eth4";

    CPortMgr *pHandle = CPortMgr::GetInstance();
    if (pHandle)
    {
        pHandle->GetPortUUID(vna_uuid, port_name, 0, port_uuid);
    }

    cout << "port uuid:" << port_uuid << endl;

    return;
}

DEFINE_DEBUG_FUNC(TestPortUUID);

void TestGetVtepInfo(const char *pstrVnaUuid, const char *pstrSwitchUuid, const char *pstrPgUuid)
{   
    if (NULL==pstrVnaUuid || NULL==pstrSwitchUuid || NULL==pstrPgUuid)
    {
        return;
    } 

    CPortMgr *pPortMgr = CPortMgr::GetInstance();
    if (!pPortMgr)
    {
        return;
    }

    string strVnaUuid;
    strVnaUuid.assign(pstrVnaUuid);
    
    string strSwitchUuid;
    strSwitchUuid.assign(pstrSwitchUuid);
    
    string strPgUuid;
    strPgUuid.assign(pstrPgUuid);    

    string strVtepName;
    string strIp;
    string strMask;

    VNET_ASSERT(0==pPortMgr->GetVtepInfo(strVnaUuid, strSwitchUuid, strPgUuid, strVtepName, strIp, strMask));

    cout << "strVtepName:" << strVtepName << "  strIp:" << strIp << "  strMask:" << strMask << endl;

    return;
}

DEFINE_DEBUG_FUNC(TestGetVtepInfo);


void TestCfgAddBond()
{
    vector<string> ifs;
    string bond_uuid = "";

    ifs.push_back("peth8");
    ifs.push_back("peth9");

    string vna_uuid = "3b9746b5-11c5-41a4-a53e-c525f7790e96";
    string port_name = "pbond0";

    CPortMgr *pHandle = CPortMgr::GetInstance();
    if (pHandle)
    {
        pHandle->AddBond2Db(vna_uuid, port_name, ifs, 4, bond_uuid);
    }
    return;
}

DEFINE_DEBUG_FUNC(TestCfgAddBond);

void TestCfgDelBond()
{
    //vector<string> ifs;

    //ifs.push_back("peth8");
    //ifs.push_back("peth9");

    string vna_uuid = "3b9746b5-11c5-41a4-a53e-c525f7790e96";
    string port_name = "pbond0";

    CPortMgr *pHandle = CPortMgr::GetInstance();
    if (pHandle)
    {
        pHandle->DelBond2Db(vna_uuid, port_name);
    }
    return;
}

DEFINE_DEBUG_FUNC(TestCfgDelBond);

void SetPortMgrPrint(int32 phy, int32 sriov, int32 bond, int32 krport, int32 uplinkLpport, int32 OpenDbgInf)
{
    CPortMgr *pHandle = CPortMgr::GetInstance();
    if (pHandle)
    {
        pHandle->SetDbgPrint(phy, sriov, bond, krport, uplinkLpport, OpenDbgInf);
    }
    return;
}

DEFINE_DEBUG_FUNC(SetPortMgrPrint);


void VNetTestPortCfg()
{
    MessageUnit tempmu(TempUnitName("TestPortCfg"));
    tempmu.Register();

    CKerNalPortMsg cMsg;
    //string strTmp;
    //strTmp.assign(cstrName);

    cMsg.m_strName = "kernel_zy";
    //strTmp.clear();
    //strTmp.assign(cstrUUID);
    cMsg.m_strUUID = "12345678";
    //strTmp.clear();
    //cMsg.m_nType = nType;
    //cMsg.m_nMTU = nMTU;
    //cMsg.m_nEvbMode = nEvb;
    //cMsg.m_nMaxVnic = nMaxvnic;
    //cMsg.m_strPGUuid = cstrPGuuid;
    cMsg.m_dwOper = 1;
    MessageOption option(MU_VNM, EV_VNETLIB_KERNALPORT_CFG, 0, 0);
    tempmu.Send(cMsg,option);
    return ;
}

DEFINE_DEBUG_FUNC(VNetTestPortCfg);

void VnetTestLoopCfg(string vna_uuid, string port_name, int32 oper)
{
    MessageUnit tempmu(TempUnitName("VnetTestLoopCfg"));
    tempmu.Register();

    CLoopPortMsg cMsg;
    cMsg.m_dwOper = oper;
    cMsg.m_strName = port_name;
    cMsg.m_strvnaUUID = vna_uuid;

    MessageOption option(MU_VNM, EV_VNETLIB_CFG_LOOPBACK, 0, 0);
    tempmu.Send(cMsg,option);
    return ;
}

DEFINE_DEBUG_FUNC(VnetTestLoopCfg);

void VnetTestKerCfg()
{
    MessageUnit tempmu(TempUnitName("VnetTestKerCfg"));
    tempmu.Register();

    CKerNalPortMsg cMsg;

    cMsg.m_dwOper = EN_ADD_KERNALPORT;
    cMsg.m_nType = 10;
    cMsg.m_strip = "100.100.100.100";
    cMsg.m_strmask = "255.255.255.0";
    cMsg.m_strName = "kernel8";
    cMsg.m_strSwitchName = "sdvs_00";
    cMsg.m_strVNAUUID = "73723572eb39472f839620742301a271a271";

    MessageOption option(MU_VNM, EV_VNETLIB_KERNALPORT_CFG, 0, 0);
    tempmu.Send(cMsg,option);
    return ;
}

DEFINE_DEBUG_FUNC(VnetTestKerCfg);

void VnetTestKerDelCfg()
{
    MessageUnit tempmu(TempUnitName("VnetTestKerDelCfg"));
    tempmu.Register();

    CKerNalPortMsg cMsg;

    cMsg.m_dwOper = EN_DEL_KERNALPORT;
    cMsg.m_nType = 10;
    cMsg.m_strip = "100.100.100.100";
    cMsg.m_strmask = "255.255.255.0";
    cMsg.m_strName = "kernel8";
    cMsg.m_strSwitchName = "sdvs_00";
    cMsg.m_strVNAUUID = "73723572eb39472f839620742301a271a271";

    MessageOption option(MU_VNM, EV_VNETLIB_KERNALPORT_CFG, 0, 0);
    tempmu.Send(cMsg,option);
    return ;
}

DEFINE_DEBUG_FUNC(VnetTestKerDelCfg);

void VnetDbgShowPortCfg()
{
    CPortMgr *pHandle = CPortMgr::GetInstance();
    if (pHandle)
    {
        pHandle->DbgShowPortCfg();
    }
    return;
}

DEFINE_DEBUG_FUNC(VnetDbgShowPortCfg);

}


