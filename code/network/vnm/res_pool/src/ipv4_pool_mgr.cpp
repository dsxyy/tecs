/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：ipv4_pool_mgr.cpp
* 文件标识：
* 内容摘要：CIPv4PoolMgr类实现文件
* 当前版本：1.0
* 作    者：
* 完成日期：2013年1月18日
*
* 修改记录1：
*     修改日期：2013/1/18
*     版 本 号：V1.0
*     修 改 人：Wang.Lule
*     修改内容：创建
******************************************************************************/

#include "vnet_comm.h"
#include "vnet_mid.h"
#include "vnet_error.h"
#include "vnetlib_event.h"
#include "vnetlib_msg.h"

#include "vnet_db_mgr.h"
#include "vnet_db_proc.h"
#include "vnet_db_netplane_iprange.h"
#include "vnet_db_logicnetwork_iprange.h"

#include "vnet_vnic.h"

#include "ipv4_addr.h"
#include "ipv4_addr_range.h"

#include "logic_network.h"
#include "logic_network_mgr.h"

#include "ipv4_addr_pool.h"
#include "ipv4_pool_mgr.h"

namespace zte_tecs
{
    CIPv4PoolMgr *CIPv4PoolMgr::s_pInstance = NULL;
    
    CIPv4PoolMgr::CIPv4PoolMgr()
    {
        m_pMU = NULL;
    }
    
    CIPv4PoolMgr::~CIPv4PoolMgr()
    {
        m_pMU = NULL;
    }

    int32 CIPv4PoolMgr::AllocIPAddrForVnic(CVNetVmMem &cVNetVmMem)
    {
        CLogicNetworkMgr *pLGMgr = CLogicNetworkMgr::GetInstance();
        if (NULL == pLGMgr)
        {
            VNET_LOG(VNET_LOG_ERROR, "CIPv4PoolMgr::AllocIPAddrForVnic: call CLogicNetworkMgr::GetInstance() failed.\n");
            return -1;
        }
        // Need transaction;
        vector<CVNetVnicMem> vecVNetVnic;
        cVNetVmMem.GetVecVnicMem(vecVNetVnic);
        vector<CVNetVnicMem>::iterator itr = vecVNetVnic.begin();
        for( ; itr != vecVNetVnic.end(); ++itr)
        { 
            CLogicNetwork cLogicNetwork("");
            string strLogiNetID = itr->GetLogicNetworkUuid();
            if (0 != pLGMgr->GetLogicNetwork(strLogiNetID, cLogicNetwork))
            {
                VNET_LOG(VNET_LOG_ERROR, "CIPv4PoolMgr::AllocIPAddrForVnic: call GetLogicNetwork(%s) failed.\n", 
                    strLogiNetID.c_str());
                return -1;
            }            
            if(EN_IP_BP_MODE_STATIC != cLogicNetwork.GetIPBootProtoMode())
            {
                continue;
            }
            
            string strIP = itr->GetIp();
            if(!strIP.empty())
            {
                // 防止重复调用重复分配;
                VNET_LOG(VNET_LOG_INFO, "CIPv4PoolMgr::AllocIPAddrForVnic: The IP(%s %s) is allocated.\n", 
                    strIP.c_str(), itr->GetMask().c_str()); 
                continue ;
            }
            CIPv4AddrPool cIPv4Pool;
            CIPv4AddrRange cIPv4AddrRange;            
            if(0 != GetLogiNetIPv4Range(strLogiNetID, cIPv4AddrRange))
            {
                VNET_LOG(VNET_LOG_ERROR, "CIPv4PoolMgr::AllocIPAddrForVnic: Call GetLogiNetIPv4Range(%s) failed.\n", 
                    strLogiNetID.c_str());  
                return -1;
            }

            if(cIPv4AddrRange.GetIPv4AddrRange().empty())
            {
                VNET_LOG(VNET_LOG_ERROR, "CIPv4PoolMgr::AllocIPAddrForVnic: The logic network's(ID:%s) IP range is not exist.\n", 
                    strLogiNetID.c_str());  
                return -1;
            }            
 
            cIPv4Pool.Init(strLogiNetID, &cIPv4AddrRange);
            TIPv4Addr tItem;
            if(0 != cIPv4Pool.AllocIPv4Addr(itr->GetNicIndex(), tItem))
            {
                VNET_LOG(VNET_LOG_ERROR, "CIPv4PoolMgr::AllocIPAddrForVnic: Allocate the logic network's(ID:%s) NIC(%d) IP failed .\n", 
                    strLogiNetID.c_str(), itr->GetNicIndex());              
                return -1;
            }
            CIPv4Addr cIPv4Addr;
            cIPv4Addr.Set(tItem);
            itr->SetIp(cIPv4Addr.GetIP());
            itr->SetMask(cIPv4Addr.GetMask());
            cVNetVmMem.SetVecVnicMem(vecVNetVnic);
            VNET_LOG(VNET_LOG_INFO, "CIPv4PoolMgr::AllocIPAddrForVnic: The IP address [%s] is allocated!\n", 
                cIPv4Addr.GetIPAndMask().c_str());

        }
      //  cVNetVmMem.SetVecVnicMem(vecVNetVnic);
        return 0;
    }
    
    int32 CIPv4PoolMgr::FreeIPAddrForVnic(CVNetVmMem &cVNetVmMem)
    {
        CLogicNetworkMgr *pLGMgr = CLogicNetworkMgr::GetInstance();
        if (NULL == pLGMgr)
        {
            VNET_LOG(VNET_LOG_ERROR, "CIPv4PoolMgr::FreeIPAddrForVnic: call CLogicNetworkMgr::GetInstance() failed.\n");
            return -1;
        }
        // Need transaction;
        vector<CVNetVnicMem> vecVNetVnic;
        cVNetVmMem.GetVecVnicMem(vecVNetVnic);
        vector<CVNetVnicMem>::iterator itr = vecVNetVnic.begin();
        for( ; itr != vecVNetVnic.end(); ++itr)
        {   
            CLogicNetwork cLogicNetwork("");
            string strLogiNetID = itr->GetLogicNetworkUuid();                    
            if (0 != pLGMgr->GetLogicNetwork(strLogiNetID, cLogicNetwork))
            {
                VNET_LOG(VNET_LOG_ERROR, "CIPv4PoolMgr::CIPv4PoolMgr: call GetLogicNetwork(%s) failed.\n", 
                    strLogiNetID.c_str());
                return -1;
            }

            if(EN_IP_BP_MODE_STATIC != cLogicNetwork.GetIPBootProtoMode())
            {
                continue;
            }
            
            string strIP = itr->GetIp();
            string strMask = itr->GetMask();
            if(strIP.empty() || strMask.empty())
            {
                VNET_LOG(VNET_LOG_INFO, "CIPv4PoolMgr::FreeIPAddrForVnic: The host IP(%s %s) is error.\n", 
                    strIP.c_str(), strMask.c_str()); 
                continue ;
            }
            CIPv4AddrPool cIPv4Pool;
            CIPv4AddrRange cIPv4AddrRange;
            
            cIPv4Pool.Init(strLogiNetID);
            TIPv4Addr tItem;
            CIPv4Addr cIPv4Addr;
            if(0 != cIPv4Addr.Set(itr->GetIp(), itr->GetMask()))
            {
                VNET_LOG(VNET_LOG_INFO, "CIPv4PoolMgr::FreeIPAddrForVnic: The IP(%s %s) format is invalid.\n", 
                     strIP.c_str(), strMask.c_str()); 
            }
            cIPv4Addr.Get(tItem);
            if(0 != cIPv4Pool.FreeIPv4Addr(tItem))
            {
                VNET_LOG(VNET_LOG_ERROR, "CIPv4PoolMgr::FreeIPAddrForVnic: Free VSIID(%s) IP(%s %s) failed .\n", 
                    itr->GetVsiIdValue().c_str(), strIP.c_str(), strMask.c_str());              
                return -1;
            }
            itr->SetIp(string(""));
            itr->SetMask(string(""));
            VNET_LOG(VNET_LOG_INFO, "CIPv4PoolMgr::FreeIPAddrForVnic: Free the IP address [%s] is successfully!\n", 
                cIPv4Addr.GetIPAndMask().c_str());

        }

        cVNetVmMem.SetVecVnicMem(vecVNetVnic);

        return 0;
    }

    int32 CIPv4PoolMgr::NetplaneAllocIPAddr(const string strNetplaneID, string &strIP, string &strMask)
    {
        CIPv4AddrPool cIPv4Pool;
        CIPv4AddrRange cIPv4AddrRange;   

        if(0 != GetNetplaneIPv4Range(strNetplaneID, cIPv4AddrRange))
        {
            VNET_LOG(VNET_LOG_ERROR, "CIPv4PoolMgr::AllocIPAddrForNic: Call GetNetplaneIPv4Range(%s) failed.\n", 
                strNetplaneID.c_str());  
            return -1;
        }

        if(cIPv4AddrRange.GetIPv4AddrRange().empty())
        {
            VNET_LOG(VNET_LOG_ERROR, "CIPv4PoolMgr::AllocIPAddrForNic: The netplane 's(ID:%s) IP range is not exist.\n", 
                strNetplaneID.c_str());  
            return -1;
        }            

        cIPv4Pool.Init(strNetplaneID, &cIPv4AddrRange, NETPLANE);
       
        TIPv4Addr tItem;
        if(0 != cIPv4Pool.AllocIPv4Addr((int32)random(), tItem))
        {
            VNET_LOG(VNET_LOG_ERROR, "CIPv4PoolMgr::AllocIPAddrForNic: Allocate the netplane's(ID:%s) IP failed .\n", 
                strNetplaneID.c_str());              
            return -1;
        }
        CIPv4Addr cIPv4Addr;
        cIPv4Addr.Set(tItem);
        strIP = cIPv4Addr.GetIP();
        strMask = cIPv4Addr.GetMask();

        VNET_LOG(VNET_LOG_INFO, "CIPv4PoolMgr::AllocIPAddrForNic: The IP address [%s] is allocated!\n", 
            cIPv4Addr.GetIPAndMask().c_str());

        return 0;
    }
    
    int32 CIPv4PoolMgr::NetplaneFreeIPAddr(const string strNetplaneID, const string strIP, const string strMask)
    {
        if(strIP.empty() || strMask.empty())
        {
            VNET_LOG(VNET_LOG_INFO, "CIPv4PoolMgr::FreeIPAddrForNic: The host IP(%s %s) is error.\n", 
                strIP.c_str(), strMask.c_str()); 
            return -1;
        }
        
        CIPv4AddrPool cIPv4Pool;        

        cIPv4Pool.Init(strNetplaneID, NETPLANE);
        TIPv4Addr tItem;
        CIPv4Addr cIPv4Addr;
        if(0 != cIPv4Addr.Set(strIP, strMask))
        {
            VNET_LOG(VNET_LOG_INFO, "CIPv4PoolMgr::FreeIPAddrForNic: The IP(%s %s) format is invalid.\n", 
                 strIP.c_str(), strMask.c_str()); 
        }
        cIPv4Addr.Get(tItem);
        if(0 != cIPv4Pool.FreeIPv4Addr(tItem))
        {
            VNET_LOG(VNET_LOG_ERROR, "CIPv4PoolMgr::FreeIPAddrForNic: Free IP(%s %s) failed .\n", 
                strIP.c_str(), strMask.c_str());              
            return -1;
        }

        VNET_LOG(VNET_LOG_INFO, "CIPv4PoolMgr::FreeIPAddrForNic: Free the IP address [%s] is successfully!\n", 
            cIPv4Addr.GetIPAndMask().c_str());

        return 0;
        
        
    }

    
    int32 CIPv4PoolMgr::Init(MessageUnit *pMu)
    {
        if (pMu == NULL)
        {
            VNET_LOG(VNET_LOG_ERROR, "CIPv4PoolMgr::Init: pMu is NULL.\n");
            return -1;
        }
        
        m_pMU = pMu;
        
        return 0;
    }
        
    void CIPv4PoolMgr::MessageEntry(const MessageFrame &message)
    {
        switch (message.option.id())
        {
        case EV_VNETLIB_LOGI_NET_IPV4_RANGE_CFG:
            {
                ProcLogicNetworkIPv4RangeMsg(message);
            }
            break;
        case EV_VNETLIB_NP_IPV4_RANGE_CFG:
            {
                ProcNetplaneIPv4RangeMsg(message);
            }
            break;
        default:
            break;            
        }
        
        return ;
    }
    
    int32 CIPv4PoolMgr::ProcNetplaneIPv4RangeMsg(const MessageFrame &message)
    {
        if(NULL == m_pMU)
        {
            VNET_ASSERT(0);
            return -1;
        }
        
        CNetplaneIPv4RangeMsg cMsg;
        cMsg.deserialize(message.data);
        string strRetInfo;
        int32 nRet = 0;
        MessageOption option(message.option.sender(), EV_VNETLIB_NP_IPV4_RANGE_CFG, 0, 0);
        
        if(m_bOpenDbgInf)
        {
            cMsg.Print();
        }
        
        if(!cMsg.IsValidOper())
        {
            VNET_LOG(VNET_LOG_WARN, "CIPv4PoolMgr::ProcNetplaneIPv4RangeMsg: Msg operation code (%d) is invalid.\n", 
                cMsg.m_dwOper);
            VNetFormatString(strRetInfo, "Msg operation code (%d) is invalid.\n", cMsg.m_dwOper);
            cMsg.SetResult(IPV4_RANGE_CFG_PARAM_ERROR);
            cMsg.SetResultinfo(strRetInfo);
            m_pMU->Send(cMsg, option);
            return -1;
        }
        
        switch(cMsg.m_dwOper)
        {
        case EN_ADD_IPV4_RANGE:
            {
                nRet = AddNetplaneIPv4Range(cMsg, strRetInfo);
            }
            break;
        case EN_DEL_IPV4_RANGE:
            {
                nRet = DelNetplaneIPv4Range(cMsg, strRetInfo);
            }
            break;
        default:
            {
                VNET_ASSERT(0);
                nRet = IPV4_RANGE_CFG_PARAM_ERROR;
                strRetInfo = "The IP address range is invalid.";
            }
        }
        
        cMsg.SetResult(nRet);
        cMsg.SetResultinfo(strRetInfo);
        m_pMU->Send(cMsg, option);
        
        return 0;
    }
    
    int32 CIPv4PoolMgr::ProcLogicNetworkIPv4RangeMsg(const MessageFrame &message)
    {
        if(NULL == m_pMU)
        {
            VNET_ASSERT(0);
            return -1;
        }
        
        CLogicNetworkIPv4RangeMsg cMsg;
        cMsg.deserialize(message.data);
        string strRetInfo;
        int32 nRet = 0;
        MessageOption option(message.option.sender(), EV_VNETLIB_LOGI_NET_IPV4_RANGE_CFG, 0, 0);
        
        if(m_bOpenDbgInf)
        {
            cMsg.Print();
        }
        
        if(!cMsg.IsValidOper())
        {
            VNET_LOG(VNET_LOG_WARN, 
                "CIPv4PoolMgr::ProcLogicNetworkIPv4RangeMsg: Msg operation code (%d) is invalid.\n", 
                cMsg.m_dwOper);
            VNetFormatString(strRetInfo, "Msg operation code (%d) is invalid.\n", cMsg.m_dwOper);
            cMsg.SetResult(IPV4_RANGE_CFG_PARAM_ERROR);
            cMsg.SetResultinfo(strRetInfo);
            m_pMU->Send(cMsg, option);
            return -1;
        }
        
        switch(cMsg.m_dwOper)
        {
        case EN_ADD_IPV4_RANGE:
            {
                nRet = AddLogicNetworkIPv4Range(cMsg, strRetInfo);
            }
            break;
        case EN_DEL_IPV4_RANGE:
            {
                nRet = DelLogicNetworkIPv4Range(cMsg, strRetInfo);
            }
            break;
        default:
            {
                VNET_ASSERT(0);
                nRet = IPV4_RANGE_CFG_PARAM_ERROR;
                strRetInfo = "The IP address range is invalid.";
            }
        }
        
        cMsg.SetResult(nRet);
        cMsg.SetResultinfo(strRetInfo);
        m_pMU->Send(cMsg, option);
        
        return 0;
    }

    int32 CIPv4PoolMgr::AddNetplaneIPv4Range(const CNetplaneIPv4RangeMsg &cMsg, string &strRetInfo)
    {
        if(!cMsg.IsValidRange())
        {
            VNET_LOG(VNET_LOG_ERROR, "CIPv4PoolMgr::AddNetplaneIPv4Range: The range config message is Invalid.\n");
            strRetInfo = "The IP address range config param is invalid.";
            return IPV4_RANGE_CFG_PARAM_ERROR;
        }
        
        CIPv4AddrRange cIPv4Range;
        // Get DB oper;
        CDBOperateNetplaneIpRange *pOper = GetDbINetplaneIpRange();
        if(NULL == pOper)
        {
            VNET_LOG(VNET_LOG_ERROR, "CIPv4PoolMgr::AddNetplaneIPv4Range: GetDbINetplaneIpRange() is NULL.\n");            
            strRetInfo = "Get the IP address range database interface handler failed.";
            return IPV4_RANGE_GET_NET_PLANE_DB_INTF_ERROR;
        }

        // IP translate;
        CIPv4Addr cIPStart;        
        CIPv4Addr cIPEnd;
        if(0 != cIPStart.Set(cMsg.m_strIPv4Start, cMsg.m_strIPv4StartMask) || 
           0 != cIPEnd.Set(cMsg.m_strIPv4End, cMsg.m_strIPv4EndMask))
        {
        
            VNET_LOG(VNET_LOG_ERROR, 
                "CIPv4PoolMgr::AddNetplaneIPv4Range: The netplane's(UUID: %s) IP range[%s-%s:%s] format is invalid.\n",
                cMsg.m_strNetplaneUUID.c_str(),
                cMsg.m_strIPv4Start.c_str(),
                cMsg.m_strIPv4End.c_str(),
                cMsg.m_strIPv4EndMask.c_str());
            VNetFormatString(strRetInfo, "The net plane's(UUID: %s) IP address range[%s-%s:%s] format is invalid.",
                cMsg.m_strNetplaneUUID.c_str(),
                cMsg.m_strIPv4Start.c_str(),
                cMsg.m_strIPv4End.c_str(),
                cMsg.m_strIPv4EndMask.c_str());
            return IPV4_RANGE_CFG_PARAM_ERROR;
        }

        TIPV4AddrRangeItem tNewItem;
        cIPStart.Get(tNewItem.tIPStart);
        cIPEnd.Get(tNewItem.tIPEnd);
        if(!CIPv4AddrRange::IsValidIPv4Range(tNewItem))
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CIPv4PoolMgr::AddNetplaneIPv4Range: The netplane's(UUID: %s) IP range[%s-%s:%s] is invalid.\n",
                cMsg.m_strNetplaneUUID.c_str(),
                cMsg.m_strIPv4Start.c_str(),
                cMsg.m_strIPv4End.c_str(),
                cMsg.m_strIPv4EndMask.c_str());
            VNetFormatString(strRetInfo, "The net plane's(UUID: %s) IP address range[%s-%s:%s] is invalid.",
                cMsg.m_strNetplaneUUID.c_str(),
                cMsg.m_strIPv4Start.c_str(),
                cMsg.m_strIPv4End.c_str(),
                cMsg.m_strIPv4EndMask.c_str());
            return IPV4_RANGE_INVALID;
        }

        if(!CIPv4AddrRange::IsSameSubnet(tNewItem))
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CIPv4PoolMgr::AddNetplaneIPv4Range: The netplane's(UUID: %s) IP range[%s-%s:%s] isn't same subnet.\n",
                cMsg.m_strNetplaneUUID.c_str(),
                cMsg.m_strIPv4Start.c_str(),
                cMsg.m_strIPv4End.c_str(),
                cMsg.m_strIPv4EndMask.c_str());
            VNetFormatString(strRetInfo, "The net plane's(UUID: %s) IP address range[%s-%s:%s] is not same subnet.",
                cMsg.m_strNetplaneUUID.c_str(),
                cMsg.m_strIPv4Start.c_str(),
                cMsg.m_strIPv4End.c_str(),
                cMsg.m_strIPv4EndMask.c_str());
            return IPV4_RANGE_NOT_SAME_SUBNET;
        }
        // DB range Load;
        vector<CDbNetplaneIpRange> outVInfo;
        vector<CDbNetplaneIpRange> vecRelatedIPRange;
        int32 nRet = pOper->Query(cMsg.m_strNetplaneUUID.c_str(), outVInfo);
        if(VNET_DB_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CIPv4PoolMgr::AddNetplaneIPv4Range: DB Query the netplane's(UUID: %s) IP range failed, ret:%d.\n",
                cMsg.m_strNetplaneUUID.c_str(), nRet);
            VNetFormatString(strRetInfo, "Get the net plane's(UUID: %s) IP address range failed.",
                cMsg.m_strNetplaneUUID.c_str());
            return nRet;
        } 

        CDbNetplaneIpRange cDbIPRange;        
        vector<CDbNetplaneIpRange>::iterator itrDBRange;
        for(itrDBRange = outVInfo.begin(); itrDBRange != outVInfo.end(); ++itrDBRange)
        {
            TIPV4AddrRangeItem tItem;
            tItem.tIPStart.uIP.dwAddr = CIPv4Addr::H2NL((int32)(itrDBRange->GetBeginNum()));
            tItem.tIPStart.uMask.dwAddr = CIPv4Addr::H2NL((int32)(itrDBRange->GetMaskNum()));
            tItem.tIPEnd.uIP.dwAddr = CIPv4Addr::H2NL((int32)(itrDBRange->GetEndNum()));
            tItem.tIPEnd.uMask.dwAddr = tItem.tIPStart.uMask.dwAddr;
            // shortest mask match network segment;
            if(tItem.tIPStart.uMask.dwAddr != tNewItem.tIPEnd.uMask.dwAddr)
            {
                uint32 dwMinMask = (tItem.tIPStart.uMask.dwAddr < tNewItem.tIPStart.uMask.dwAddr) ? 
                                    tItem.tIPStart.uMask.dwAddr : tNewItem.tIPStart.uMask.dwAddr;
                if((tNewItem.tIPStart.uIP.dwAddr & dwMinMask) == 
                   (tItem.tIPStart.uIP.dwAddr & dwMinMask))
                {
                    VNET_LOG(VNET_LOG_ERROR, 
                        "CIPv4PoolMgr::AddNetplaneIPv4Range: the net plane's(UUID: %s) IP range[%s-%s:%s] conflict with [%s-%s:%s].\n",
                        cMsg.m_strNetplaneUUID.c_str(),
                        cMsg.m_strIPv4Start.c_str(),
                        cMsg.m_strIPv4End.c_str(),
                        cMsg.m_strIPv4EndMask.c_str(),
                        itrDBRange->GetBeginStr().c_str(),
                        itrDBRange->GetEndStr().c_str(),
                        itrDBRange->GetMaskStr().c_str());
                    VNetFormatString(strRetInfo, "The net plane's(UUID: %s) IP range[%s-%s:%s] conflict with [%s-%s:%s].",
                        cMsg.m_strNetplaneUUID.c_str(),
                        cMsg.m_strIPv4Start.c_str(),
                        cMsg.m_strIPv4End.c_str(),
                        cMsg.m_strIPv4EndMask.c_str(),
                        itrDBRange->GetBeginStr().c_str(),
                        itrDBRange->GetEndStr().c_str(),
                        itrDBRange->GetMaskStr().c_str());
                    return IPV4_RANGE_INVALID;
                }
            }
            else
            {
                if((tNewItem.tIPStart.uIP.dwAddr & tNewItem.tIPStart.uMask.dwAddr) != 
                   (tItem.tIPStart.uIP.dwAddr & tItem.tIPStart.uMask.dwAddr))
                {
                    continue ;
                }
                vecRelatedIPRange.push_back(*itrDBRange);
                cIPv4Range.AddValidRange(tItem);
            } 
        }
        
        if(outVInfo.empty() || cIPv4Range.GetIPv4AddrRange().empty())
        {// only one record, save it to DB;
            cDbIPRange.SetUuid(cMsg.m_strNetplaneUUID.c_str());
            cDbIPRange.SetBeginNum(CIPv4Addr::N2HL(tNewItem.tIPStart.uIP.dwAddr));
            cDbIPRange.SetEndNum(CIPv4Addr::N2HL(tNewItem.tIPEnd.uIP.dwAddr));
            cDbIPRange.SetMaskNum(CIPv4Addr::N2HL(tNewItem.tIPEnd.uMask.dwAddr));                   
            cDbIPRange.SetBeginStr(cMsg.m_strIPv4Start.c_str());
            cDbIPRange.SetEndStr(cMsg.m_strIPv4End.c_str());
            cDbIPRange.SetMaskStr(cMsg.m_strIPv4EndMask.c_str());
            nRet = pOper->Add(cDbIPRange);
            if(VNET_DB_SUCCESS != nRet)
            {
                VNET_LOG(VNET_LOG_ERROR, 
                    "CIPv4PoolMgr::AddNetplaneIPv4Range: DB Add the netplane's(UUID: %s) IP range[%s-%s:%s] failed, ret:%d.\n",
                    cMsg.m_strNetplaneUUID.c_str(),
                    cMsg.m_strIPv4Start.c_str(),
                    cMsg.m_strIPv4End.c_str(),
                    cMsg.m_strIPv4EndMask.c_str(),
                    nRet);
                VNetFormatString(strRetInfo, "Add the net plane's(UUID: %s) IP address range[%s-%s:%s] to the database failed.",
                    cMsg.m_strNetplaneUUID.c_str(),
                    cMsg.m_strIPv4Start.c_str(),
                    cMsg.m_strIPv4End.c_str(),
                    cMsg.m_strIPv4EndMask.c_str());
                return nRet;
            }
            return IPV4_RANGE_OPER_SUCCESS;
        }      

        // range add;
        if(0 != cIPv4Range.AddRange(tNewItem))
        {            
            VNET_LOG(VNET_LOG_ERROR, 
                "CIPv4PoolMgr::AddNetplaneIPv4Range: The IP range[0x%8.8x-0x%8.8x:0x%8.8x] calculate failed.\n",
                tNewItem.tIPStart.uIP.dwAddr, 
                tNewItem.tIPEnd.uIP.dwAddr, 
                tNewItem.tIPStart.uMask.dwAddr);
            VNetFormatString(strRetInfo, "Combining the calculation of net plane's(UUID: %s) IP address range[%s-%s:%s] failed.",
                cMsg.m_strNetplaneUUID.c_str(),
                cMsg.m_strIPv4Start.c_str(),
                cMsg.m_strIPv4End.c_str(),
                cMsg.m_strIPv4EndMask.c_str());
            return IPV4_RANGE_CALC_ERROR;
        }
        
        // DB Compare and save;Need Transaction;
        vector<TIPV4AddrRangeItem> &vecCalcRange = cIPv4Range.GetIPv4AddrRange();
        vector<TIPV4AddrRangeItem>::iterator itrCalcRange = vecCalcRange.begin();
        for(; itrCalcRange != vecCalcRange.end(); ++itrCalcRange)
        {            
            int32 bFind = 0;
            for(itrDBRange = vecRelatedIPRange.begin(); itrDBRange != vecRelatedIPRange.end(); ++itrDBRange)
            {
                TIPV4AddrRangeItem tIPRange;
                tIPRange.tIPStart.uIP.dwAddr = CIPv4Addr::H2NL((int32)(itrDBRange->GetBeginNum()));
                tIPRange.tIPStart.uMask.dwAddr = CIPv4Addr::H2NL((int32)(itrDBRange->GetMaskNum()));
                tIPRange.tIPEnd.uIP.dwAddr = CIPv4Addr::H2NL((int32)(itrDBRange->GetEndNum()));
                tIPRange.tIPEnd.uMask.dwAddr = tIPRange.tIPStart.uMask.dwAddr;
                if(tIPRange.tIPStart.uIP.dwAddr == itrCalcRange->tIPStart.uIP.dwAddr &&
                   tIPRange.tIPStart.uMask.dwAddr == itrCalcRange->tIPStart.uMask.dwAddr &&
                   tIPRange.tIPEnd.uIP.dwAddr == itrCalcRange->tIPEnd.uIP.dwAddr &&
                   tIPRange.tIPEnd.uMask.dwAddr == itrCalcRange->tIPEnd.uMask.dwAddr)
                {
                    bFind = 1;
                    vecRelatedIPRange.erase(itrDBRange);
                    break;
                }
            }
            
            if(0 == bFind)
            {                
                // Add To DB
                cDbIPRange.SetUuid(cMsg.m_strNetplaneUUID.c_str());
                cDbIPRange.SetBeginNum(CIPv4Addr::N2HL(itrCalcRange->tIPStart.uIP.dwAddr));
                cDbIPRange.SetEndNum(CIPv4Addr::N2HL(itrCalcRange->tIPEnd.uIP.dwAddr));
                cDbIPRange.SetMaskNum(CIPv4Addr::N2HL(itrCalcRange->tIPEnd.uMask.dwAddr));   
                cIPStart.Set(itrCalcRange->tIPStart);
                cDbIPRange.SetBeginStr(cIPStart.GetIP().c_str());                
                cIPEnd.Set(itrCalcRange->tIPEnd);
                cDbIPRange.SetEndStr(cIPEnd.GetIP().c_str());
                cDbIPRange.SetMaskStr(cIPEnd.GetMask().c_str());
                nRet = pOper->Add(cDbIPRange);
                if(VNET_DB_SUCCESS != nRet)
                {
                    VNET_LOG(VNET_LOG_ERROR, 
                        "CIPv4PoolMgr::AddNetplaneIPv4Range: DB Merge(add) Netplane's(UUID: %s) IP range[%s-%s:%s] failed, ret:%d.\n",
                        cMsg.m_strNetplaneUUID.c_str(),
                        cMsg.m_strIPv4Start.c_str(),
                        cMsg.m_strIPv4End.c_str(),
                        cMsg.m_strIPv4EndMask.c_str(),
                        nRet);
                    VNetFormatString(strRetInfo, "Add the net plane's(UUID: %s) IP address range[%s-%s:%s] to the database failed.",
                        cMsg.m_strNetplaneUUID.c_str(),
                        cMsg.m_strIPv4Start.c_str(),
                        cMsg.m_strIPv4End.c_str(),
                        cMsg.m_strIPv4EndMask.c_str());
                    return nRet;                
                }             
            }
        }
        // delete invalid record from db;
        for(itrDBRange = vecRelatedIPRange.begin(); itrDBRange != vecRelatedIPRange.end(); ++itrDBRange)
        {
            nRet = pOper->ForceDel(*itrDBRange);
            if(VNET_DB_SUCCESS != nRet)
            {
                VNET_LOG(VNET_LOG_ERROR, 
                    "CIPv4PoolMgr::AddNetplaneIPv4Range: DB Merge(del) netplane's(UUID: %s) IP range[%s-%s:%s] failed, ret:%d.\n",
                    cMsg.m_strNetplaneUUID.c_str(),
                    cMsg.m_strIPv4Start.c_str(),
                    cMsg.m_strIPv4End.c_str(),
                    cMsg.m_strIPv4EndMask.c_str(),
                    nRet);
                VNetFormatString(strRetInfo, "Merge the net plane's(UUID: %s) IP address range[%s-%s:%s] to the database failed.",
                    cMsg.m_strNetplaneUUID.c_str(),
                    cMsg.m_strIPv4Start.c_str(),
                    cMsg.m_strIPv4End.c_str(),
                    cMsg.m_strIPv4EndMask.c_str());
                return nRet;  
            }             
        }
        
        return IPV4_RANGE_OPER_SUCCESS;
    }
    
    int32 CIPv4PoolMgr::DelNetplaneIPv4Range(const CNetplaneIPv4RangeMsg &cMsg, string &strRetInfo)
    {
        if(!cMsg.IsValidRange())
        {
            VNET_LOG(VNET_LOG_ERROR, "CIPv4PoolMgr::DelNetplaneIPv4Range: The range config message is Invalid.\n");
            strRetInfo = "The IP address range config param is invalid.";
            return IPV4_RANGE_CFG_PARAM_ERROR;
        }
        
        CIPv4AddrRange cIPv4Range;
        // Get DB oper;
        CDBOperateNetplaneIpRange *pOper = GetDbINetplaneIpRange();
        if(NULL == pOper)
        {
            VNET_LOG(VNET_LOG_ERROR, "CIPv4PoolMgr::DelNetplaneIPv4Range: GetDbINetplaneIpRange() is NULL.\n");
            strRetInfo = "Get the IP address range database interface handler failed.";
            return IPV4_RANGE_GET_NET_PLANE_DB_INTF_ERROR;
        }

        // IP translate;
        CIPv4Addr cIPStart;        
        CIPv4Addr cIPEnd;   
        if(0 != cIPStart.Set(cMsg.m_strIPv4Start, cMsg.m_strIPv4StartMask) || 
           0 != cIPEnd.Set(cMsg.m_strIPv4End, cMsg.m_strIPv4EndMask))
        {
        
            VNET_LOG(VNET_LOG_ERROR, 
                "CIPv4PoolMgr::DelNetplaneIPv4Range: The netplane's(UUID: %s) IP range[%s-%s:%s] format is invalid.\n",
                cMsg.m_strNetplaneUUID.c_str(),
                cMsg.m_strIPv4Start.c_str(),
                cMsg.m_strIPv4End.c_str(),
                cMsg.m_strIPv4EndMask.c_str());
            VNetFormatString(strRetInfo, "The net plane's(UUID: %s) IP address range[%s-%s:%s] format is invalid.",
                cMsg.m_strNetplaneUUID.c_str(),
                cMsg.m_strIPv4Start.c_str(),
                cMsg.m_strIPv4End.c_str(),
                cMsg.m_strIPv4EndMask.c_str());
            return IPV4_RANGE_CFG_PARAM_ERROR;
        }
        TIPV4AddrRangeItem tNewItem;
        cIPStart.Get(tNewItem.tIPStart);
        cIPEnd.Get(tNewItem.tIPEnd);
        if(!CIPv4AddrRange::IsValidIPv4Range(tNewItem))
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CIPv4PoolMgr::DelNetplaneIPv4Range: The netplane's(UUID: %s) IP range[%s-%s:%s] is invalid.\n",
                cMsg.m_strNetplaneUUID.c_str(),
                cMsg.m_strIPv4Start.c_str(),
                cMsg.m_strIPv4End.c_str(),
                cMsg.m_strIPv4EndMask.c_str());
            VNetFormatString(strRetInfo, "The net plane's(UUID: %s) IP address range[%s-%s:%s] is invalid.",
                cMsg.m_strNetplaneUUID.c_str(),
                cMsg.m_strIPv4Start.c_str(),
                cMsg.m_strIPv4End.c_str(),
                cMsg.m_strIPv4EndMask.c_str());
            return IPV4_RANGE_INVALID;
        }
        
        if(!CIPv4AddrRange::IsSameSubnet(tNewItem))
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CIPv4PoolMgr::DelNetplaneIPv4Range: The netplane's(UUID: %s) IP range[%s-%s:%s] isn't same subnet.\n",
                cMsg.m_strNetplaneUUID.c_str(),
                cMsg.m_strIPv4Start.c_str(),
                cMsg.m_strIPv4End.c_str(),
                cMsg.m_strIPv4EndMask.c_str());
            VNetFormatString(strRetInfo, "The net plane's(UUID: %s) IP address range[%s-%s:%s] is not same subnet.",
                cMsg.m_strNetplaneUUID.c_str(),
                cMsg.m_strIPv4Start.c_str(),
                cMsg.m_strIPv4End.c_str(),
                cMsg.m_strIPv4EndMask.c_str()); 
            return IPV4_RANGE_NOT_SAME_SUBNET;
        }
        // DB range Load;
        vector<CDbNetplaneIpRange> outVInfo;
        int32 nRet = pOper->Query(cMsg.m_strNetplaneUUID.c_str(), outVInfo);
        if(VNET_DB_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CIPv4PoolMgr::DelNetplaneIPv4Range: DB Query the netplane's(UUID: %s) IP range failed, ret:%d.\n",
                cMsg.m_strNetplaneUUID.c_str(),
                nRet);
            VNetFormatString(strRetInfo, "Get the net plane's(UUID: %s) IP address range failed.",
                cMsg.m_strNetplaneUUID.c_str());
            return nRet;
        } 

        CDbNetplaneIpRange cDbIPRange;        
        vector<CDbNetplaneIpRange>::iterator itrDBRange;
        if(outVInfo.empty())
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CIPv4PoolMgr::DelNetplaneIPv4Range: the netplane's(UUID: %s) IP range[%s-%s:%s] is not exist.\n",
                cMsg.m_strNetplaneUUID.c_str(),
                cMsg.m_strIPv4Start.c_str(),
                cMsg.m_strIPv4End.c_str(),
                cMsg.m_strIPv4EndMask.c_str());
            VNetFormatString(strRetInfo, "The net plane's(UUID: %s) IP address range[%s-%s:%s] is not configured.",
                cMsg.m_strNetplaneUUID.c_str(),
                cMsg.m_strIPv4Start.c_str(),
                cMsg.m_strIPv4End.c_str(),
                cMsg.m_strIPv4EndMask.c_str());
            return IPV4_RANGE_NOT_EXIST;
        }

        // Check the range is used by IP pool or not.
        int32 nOccNum = 0;
        nRet = pOper->QueryIsUsing(cMsg.m_strNetplaneUUID.c_str(), 
                             CIPv4Addr::N2HL(tNewItem.tIPStart.uIP.dwAddr),
                             CIPv4Addr::N2HL(tNewItem.tIPEnd.uIP.dwAddr),
                             nOccNum);
        if(VNET_DB_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CIPv4PoolMgr::DelNetplaneIPv4Range: call QueryAllocNum (%s, 0x%x, 0x%x) failed, ret:%d.\n",
                cMsg.m_strNetplaneUUID.c_str(),
                CIPv4Addr::N2HL(tNewItem.tIPStart.uIP.dwAddr),
                CIPv4Addr::N2HL(tNewItem.tIPEnd.uIP.dwAddr),
                nRet);
            VNetFormatString(strRetInfo, "Get the net plane's(UUID: %s) IP address range used state failed.",
                cMsg.m_strNetplaneUUID.c_str());
            return nRet;
        }

        if(0 != nOccNum)
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CIPv4PoolMgr::DelNetplaneIPv4Range: the netplane's(UUID: %s) IP range[%s-%s:%s] is occupied, num: %d.\n",
                cMsg.m_strNetplaneUUID.c_str(),
                cMsg.m_strIPv4Start.c_str(),
                cMsg.m_strIPv4End.c_str(),
                cMsg.m_strIPv4EndMask.c_str(),
                nOccNum);
            VNetFormatString(strRetInfo, "The net plane's(UUID: %s) IP address range[%s-%s:%s] is used.",
                cMsg.m_strNetplaneUUID.c_str(),
                cMsg.m_strIPv4Start.c_str(),
                cMsg.m_strIPv4End.c_str(),
                cMsg.m_strIPv4EndMask.c_str()); 
            return IPV4_RANGE_OCCUPIED;
        }

        vector<CDbNetplaneIpRange> vecRelatedIPRange;
        for(itrDBRange = outVInfo.begin(); itrDBRange != outVInfo.end(); ++itrDBRange)
        {
            TIPV4AddrRangeItem tItem;
            tItem.tIPStart.uIP.dwAddr = CIPv4Addr::H2NL((int32)(itrDBRange->GetBeginNum()));
            tItem.tIPStart.uMask.dwAddr = CIPv4Addr::H2NL((int32)(itrDBRange->GetMaskNum()));
            tItem.tIPEnd.uIP.dwAddr = CIPv4Addr::H2NL((int32)(itrDBRange->GetEndNum()));
            tItem.tIPEnd.uMask.dwAddr = tItem.tIPStart.uMask.dwAddr;
            // shortest mask match network segment;
            if(tItem.tIPStart.uMask.dwAddr != tNewItem.tIPEnd.uMask.dwAddr)
            {
                uint32 dwMinMask = (tItem.tIPStart.uMask.dwAddr < tNewItem.tIPStart.uMask.dwAddr) ? 
                                    tItem.tIPStart.uMask.dwAddr : tNewItem.tIPStart.uMask.dwAddr;
                if((tNewItem.tIPStart.uIP.dwAddr & dwMinMask) == 
                   (tItem.tIPStart.uIP.dwAddr & dwMinMask))
                {
                    VNET_LOG(VNET_LOG_ERROR, 
                        "CIPv4PoolMgr::DelNetplaneIPv4Range: the net plane's(UUID: %s) IP range[%s-%s:%s] conflict with [%s-%s:%s].\n",
                        cMsg.m_strNetplaneUUID.c_str(),
                        cMsg.m_strIPv4Start.c_str(),
                        cMsg.m_strIPv4End.c_str(),
                        cMsg.m_strIPv4EndMask.c_str(),
                        itrDBRange->GetBeginStr().c_str(),
                        itrDBRange->GetEndStr().c_str(),
                        itrDBRange->GetMaskStr().c_str());
                    VNetFormatString(strRetInfo, "The net plane's(UUID: %s) IP range[%s-%s:%s] conflict with [%s-%s:%s].",
                        cMsg.m_strNetplaneUUID.c_str(),
                        cMsg.m_strIPv4Start.c_str(),
                        cMsg.m_strIPv4End.c_str(),
                        cMsg.m_strIPv4EndMask.c_str(),
                        itrDBRange->GetBeginStr().c_str(),
                        itrDBRange->GetEndStr().c_str(),
                        itrDBRange->GetMaskStr().c_str());
                    return IPV4_RANGE_INVALID;
                }
            }
            else
            {
                if((tNewItem.tIPStart.uIP.dwAddr & tNewItem.tIPStart.uMask.dwAddr) != 
                   (tItem.tIPStart.uIP.dwAddr & tItem.tIPStart.uMask.dwAddr))
                {
                    continue ;
                }
                vecRelatedIPRange.push_back(*itrDBRange);
                cIPv4Range.AddValidRange(tItem);
            }
        }

        if(cIPv4Range.GetIPv4AddrRange().empty())
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CIPv4PoolMgr::DelNetplaneIPv4Range: the netplane's(UUID: %s) IP range[%s-%s:%s] is not exist.\n",
                cMsg.m_strNetplaneUUID.c_str(),
                cMsg.m_strIPv4Start.c_str(),
                cMsg.m_strIPv4End.c_str(),
                cMsg.m_strIPv4EndMask.c_str());
            VNetFormatString(strRetInfo, "The net plane's(UUID: %s) IP address range[%s-%s:%s] is not configured.",
                cMsg.m_strNetplaneUUID.c_str(),
                cMsg.m_strIPv4Start.c_str(),
                cMsg.m_strIPv4End.c_str(),
                cMsg.m_strIPv4EndMask.c_str());
            return IPV4_RANGE_NOT_EXIST;
        }

        // range del;
        if(0 != cIPv4Range.DelRange(tNewItem))
        {            
            VNET_LOG(VNET_LOG_ERROR, 
                "CIPv4PoolMgr::DelNetplaneIPv4Range: The IP range[0x%8.8x-0x%8.8x:0x%8.8x] calculate failed.\n",
                tNewItem.tIPStart.uIP.dwAddr, 
                tNewItem.tIPEnd.uIP.dwAddr, 
                tNewItem.tIPStart.uMask.dwAddr);
            VNetFormatString(strRetInfo, "Combining the calculation of net plane's(UUID: %s) IP address range[%s-%s:%s] failed.",
                cMsg.m_strNetplaneUUID.c_str(),
                cMsg.m_strIPv4Start.c_str(),
                cMsg.m_strIPv4End.c_str(),
                cMsg.m_strIPv4EndMask.c_str());
            return IPV4_RANGE_CALC_ERROR;
        }
        
        // DB Compare and save;Need Transaction;
        vector<TIPV4AddrRangeItem> &vecCalcRange = cIPv4Range.GetIPv4AddrRange();
        vector<TIPV4AddrRangeItem>::iterator itrCalcRange = vecCalcRange.begin();
        for(; itrCalcRange != vecCalcRange.end(); ++itrCalcRange)
        {            
            int32 bFind = 0;
            for(itrDBRange = vecRelatedIPRange.begin(); itrDBRange != vecRelatedIPRange.end(); ++itrDBRange)
            {
                TIPV4AddrRangeItem tIPRange;
                tIPRange.tIPStart.uIP.dwAddr = CIPv4Addr::H2NL((int32)(itrDBRange->GetBeginNum()));
                tIPRange.tIPStart.uMask.dwAddr = CIPv4Addr::H2NL((int32)(itrDBRange->GetMaskNum()));
                tIPRange.tIPEnd.uIP.dwAddr = CIPv4Addr::H2NL((int32)(itrDBRange->GetEndNum()));
                tIPRange.tIPEnd.uMask.dwAddr = tIPRange.tIPStart.uMask.dwAddr;
                if(tIPRange.tIPStart.uIP.dwAddr == itrCalcRange->tIPStart.uIP.dwAddr &&
                   tIPRange.tIPStart.uMask.dwAddr == itrCalcRange->tIPStart.uMask.dwAddr &&
                   tIPRange.tIPEnd.uIP.dwAddr == itrCalcRange->tIPEnd.uIP.dwAddr &&
                   tIPRange.tIPEnd.uMask.dwAddr == itrCalcRange->tIPEnd.uMask.dwAddr)
                {
                    bFind = 1;
                    vecRelatedIPRange.erase(itrDBRange);
                    break;
                }
            }
            
            if(0 == bFind)
            {                
                // Add To DB
                cDbIPRange.SetUuid(cMsg.m_strNetplaneUUID.c_str());
                cDbIPRange.SetBeginNum(CIPv4Addr::N2HL(itrCalcRange->tIPStart.uIP.dwAddr));
                cDbIPRange.SetEndNum(CIPv4Addr::N2HL(itrCalcRange->tIPEnd.uIP.dwAddr));
                cDbIPRange.SetMaskNum(CIPv4Addr::N2HL(itrCalcRange->tIPEnd.uMask.dwAddr));   
                cIPStart.Set(itrCalcRange->tIPStart);
                cDbIPRange.SetBeginStr(cIPStart.GetIP().c_str());                
                cIPEnd.Set(itrCalcRange->tIPEnd);
                cDbIPRange.SetEndStr(cIPEnd.GetIP().c_str());
                cDbIPRange.SetMaskStr(cIPEnd.GetMask().c_str());
                nRet = pOper->Add(cDbIPRange);
                if(VNET_DB_SUCCESS != nRet)
                {
                    VNET_LOG(VNET_LOG_ERROR, 
                        "CIPv4PoolMgr::DelNetplaneIPv4Range: DB Merge(add) the netplane's(UUID: %s) IP range[%s-%s:%s] failed, ret:%d.\n",
                        cMsg.m_strNetplaneUUID.c_str(),
                        cMsg.m_strIPv4Start.c_str(),
                        cMsg.m_strIPv4End.c_str(),
                        cMsg.m_strIPv4EndMask.c_str(),
                        nRet);
                    VNetFormatString(strRetInfo, "Add the net plane's(UUID: %s) IP address range[%s-%s:%s] to the database failed.",
                         cMsg.m_strNetplaneUUID.c_str(),
                         cMsg.m_strIPv4Start.c_str(),
                         cMsg.m_strIPv4End.c_str(),
                         cMsg.m_strIPv4EndMask.c_str());
                    return nRet;                
                }             
            }
        }
        // delete invalid record from db;
        for(itrDBRange = vecRelatedIPRange.begin(); itrDBRange != vecRelatedIPRange.end(); ++itrDBRange)
        {
            nRet = pOper->Del(*itrDBRange);
            if(VNET_DB_SUCCESS != nRet)
            {
                VNET_LOG(VNET_LOG_ERROR, 
                    "CIPv4PoolMgr::DelNetplaneIPv4Range: DB Merge(del) the netplane's(UUID: %s) IP range[%s-%s:%s] failed, ret:%d.\n",
                    cMsg.m_strNetplaneUUID.c_str(),
                    cMsg.m_strIPv4Start.c_str(),
                    cMsg.m_strIPv4End.c_str(),
                    cMsg.m_strIPv4EndMask.c_str(),
                    nRet);
                VNetFormatString(strRetInfo, "Merge the net plane's(UUID: %s) IP address range[%s-%s:%s] to the database failed.",
                     cMsg.m_strNetplaneUUID.c_str(),
                     cMsg.m_strIPv4Start.c_str(),
                     cMsg.m_strIPv4End.c_str(),
                     cMsg.m_strIPv4EndMask.c_str());
                return nRet; 
            }             
        }
        
        return IPV4_RANGE_OPER_SUCCESS;
    } 
    
    int32 CIPv4PoolMgr::AddLogicNetworkIPv4Range(const CLogicNetworkIPv4RangeMsg &cMsg, string &strRetInfo)
    {
        if(!cMsg.IsValidRange())
        {
            VNET_LOG(VNET_LOG_ERROR, "CIPv4PoolMgr::AddLogicNetworkIPv4Range: The range config message is Invalid.\n");
            strRetInfo = "The IP address range config param is invalid.";
            return IPV4_RANGE_CFG_PARAM_ERROR;
        }
        
        CIPv4AddrRange cIPv4Range;
        // Get DB oper;
        CDBOperateLogicNetworkIpRange *pOper = GetDbILogicNetworkIpRange();
        if(NULL == pOper)
        {
            VNET_LOG(VNET_LOG_ERROR, "CIPv4PoolMgr::AddLogicNetworkIPv4Range: GetDbILogicNetworkIpRange() is NULL.\n");
            strRetInfo = "Get the IP address range database interface handler failed.";
            return IPV4_RANGE_GET_LG_NET_DB_INTF_ERROR;
        }

        // IP translate;
        CIPv4Addr cIPStart;        
        CIPv4Addr cIPEnd;
        if(0 != cIPStart.Set(cMsg.m_strIPv4Start, cMsg.m_strIPv4StartMask) || 
           0 != cIPEnd.Set(cMsg.m_strIPv4End, cMsg.m_strIPv4EndMask))
        {
        
            VNET_LOG(VNET_LOG_ERROR, 
                "CIPv4PoolMgr::AddLogicNetworkIPv4Range: The logic_network's(UUID: %s) IP range[%s-%s:%s] format is invalid.\n",
                cMsg.m_strLogicNetworkUUID.c_str(),
                cMsg.m_strIPv4Start.c_str(),
                cMsg.m_strIPv4End.c_str(),
                cMsg.m_strIPv4EndMask.c_str());
            VNetFormatString(strRetInfo, "The logic network's(UUID: %s) IP address range[%s-%s:%s] format is invalid.",
                cMsg.m_strLogicNetworkUUID.c_str(),
                cMsg.m_strIPv4Start.c_str(),
                cMsg.m_strIPv4End.c_str(),
                cMsg.m_strIPv4EndMask.c_str());
            return IPV4_RANGE_CFG_PARAM_ERROR;
        }

        TIPV4AddrRangeItem tNewItem;
        cIPStart.Get(tNewItem.tIPStart);
        cIPEnd.Get(tNewItem.tIPEnd);
        if(!CIPv4AddrRange::IsValidIPv4Range(tNewItem))
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CIPv4PoolMgr::AddLogicNetworkIPv4Range: The logic_network's(UUID: %s) IP range[%s-%s:%s] is invalid.\n",
                cMsg.m_strLogicNetworkUUID.c_str(),
                cMsg.m_strIPv4Start.c_str(),
                cMsg.m_strIPv4End.c_str(),
                cMsg.m_strIPv4EndMask.c_str());
            VNetFormatString(strRetInfo, "The logic network's(UUID: %s) IP address range[%s-%s:%s] is invalid.",
                cMsg.m_strLogicNetworkUUID.c_str(),
                cMsg.m_strIPv4Start.c_str(),
                cMsg.m_strIPv4End.c_str(),
                cMsg.m_strIPv4EndMask.c_str());
            return IPV4_RANGE_INVALID;
        }

        if(!CIPv4AddrRange::IsSameSubnet(tNewItem))
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CIPv4PoolMgr::AddLogicNetworkIPv4Range: The logic_network's(UUID: %s) IP range[%s-%s:%s] isn't same subnet.\n",
                cMsg.m_strLogicNetworkUUID.c_str(),
                cMsg.m_strIPv4Start.c_str(),
                cMsg.m_strIPv4End.c_str(),
                cMsg.m_strIPv4EndMask.c_str());
            VNetFormatString(strRetInfo, "The logic network's(UUID: %s) IP address range[%s-%s:%s] is not same subnet.",
                cMsg.m_strLogicNetworkUUID.c_str(),
                cMsg.m_strIPv4Start.c_str(),
                cMsg.m_strIPv4End.c_str(),
                cMsg.m_strIPv4EndMask.c_str()); 
            return IPV4_RANGE_NOT_SAME_SUBNET;
        }
        // Logic IP range must be in the netplane's IP range;        
        CDbLogicNetworkIpRange cDbIPRange;        
        cDbIPRange.SetUuid(cMsg.m_strLogicNetworkUUID.c_str());
        cDbIPRange.SetBeginNum(CIPv4Addr::N2HL(tNewItem.tIPStart.uIP.dwAddr));
        cDbIPRange.SetEndNum(CIPv4Addr::N2HL(tNewItem.tIPEnd.uIP.dwAddr));
        cDbIPRange.SetMaskNum(CIPv4Addr::N2HL(tNewItem.tIPEnd.uMask.dwAddr));                   
        cDbIPRange.SetBeginStr(cMsg.m_strIPv4Start.c_str());
        cDbIPRange.SetEndStr(cMsg.m_strIPv4End.c_str());
        cDbIPRange.SetMaskStr(cMsg.m_strIPv4EndMask.c_str());
        int32 nRet = 0;
        #if 0
        int32 nRet = pOper->CheckAdd(cDbIPRange);
        if(VNET_DB_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CIPv4PoolMgr::AddLogicNetworkIPv4Range: DB Check add the logic network's(UUID: %s) IP range(%s %s : %s %s) failed, ret:%d.\n",
                cMsg.m_strLogicNetworkUUID.c_str(),
                cMsg.m_strIPv4Start.c_str(),
                cMsg.m_strIPv4StartMask.c_str(),
                cMsg.m_strIPv4End.c_str(),
                cMsg.m_strIPv4EndMask.c_str(),
                nRet);
            VNetFormatString(strRetInfo, "Add the logic network's(UUID: %s) IP address range[%s-%s:%s] to the database check failed.",
                cMsg.m_strLogicNetworkUUID.c_str(),
                cMsg.m_strIPv4Start.c_str(),
                cMsg.m_strIPv4End.c_str(),
                cMsg.m_strIPv4EndMask.c_str());
            return nRet;
        }
        #endif
        // DB range Load;
        vector<CDbLogicNetworkIpRange> outVInfo;
        vector<CDbLogicNetworkIpRange> vecRelatedIPRange;
        nRet = pOper->Query(cMsg.m_strLogicNetworkUUID.c_str(), outVInfo);
        if(VNET_DB_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CIPv4PoolMgr::AddLogicNetworkIPv4Range: DB Query the logic_network's(UUID: %s) IP range failed, ret:%d.\n",
                cMsg.m_strLogicNetworkUUID.c_str(), nRet);
            VNetFormatString(strRetInfo, "Get the logic network's(UUID: %s) IP address range failed.",
                cMsg.m_strLogicNetworkUUID.c_str());
            return nRet;
        }
        
        vector<CDbLogicNetworkIpRange>::iterator itrDBRange;
        for(itrDBRange = outVInfo.begin(); itrDBRange != outVInfo.end(); ++itrDBRange)
        {
            TIPV4AddrRangeItem tItem;
            tItem.tIPStart.uIP.dwAddr = CIPv4Addr::H2NL((int32)(itrDBRange->GetBeginNum()));
            tItem.tIPStart.uMask.dwAddr = CIPv4Addr::H2NL((int32)(itrDBRange->GetMaskNum()));
            tItem.tIPEnd.uIP.dwAddr = CIPv4Addr::H2NL((int32)(itrDBRange->GetEndNum()));
            tItem.tIPEnd.uMask.dwAddr = tItem.tIPStart.uMask.dwAddr;
            // shortest mask match network segment;
            if(tItem.tIPStart.uMask.dwAddr != tNewItem.tIPEnd.uMask.dwAddr)
            {
                uint32 dwMinMask = (tItem.tIPStart.uMask.dwAddr < tNewItem.tIPStart.uMask.dwAddr) ? 
                                    tItem.tIPStart.uMask.dwAddr : tNewItem.tIPStart.uMask.dwAddr;
                if((tNewItem.tIPStart.uIP.dwAddr & dwMinMask) == 
                   (tItem.tIPStart.uIP.dwAddr & dwMinMask))
                {
                    VNET_LOG(VNET_LOG_ERROR, 
                        "CIPv4PoolMgr::AddLogicNetworkIPv4Range: the logic_network's(UUID: %s) IP range[%s-%s:%s] conflict with [%s-%s:%s].\n",
                        cMsg.m_strLogicNetworkUUID.c_str(),
                        cMsg.m_strIPv4Start.c_str(),
                        cMsg.m_strIPv4End.c_str(),
                        cMsg.m_strIPv4EndMask.c_str(),
                        itrDBRange->GetBeginStr().c_str(),
                        itrDBRange->GetEndStr().c_str(),
                        itrDBRange->GetMaskStr().c_str());
                    VNetFormatString(strRetInfo, "The logic_network's(UUID: %s) IP range[%s-%s:%s] conflict with [%s-%s:%s].",
                        cMsg.m_strLogicNetworkUUID.c_str(),
                        cMsg.m_strIPv4Start.c_str(),
                        cMsg.m_strIPv4End.c_str(),
                        cMsg.m_strIPv4EndMask.c_str(),
                        itrDBRange->GetBeginStr().c_str(),
                        itrDBRange->GetEndStr().c_str(),
                        itrDBRange->GetMaskStr().c_str());
                    return IPV4_RANGE_INVALID;
                }
            }
            else
            {
                if((tNewItem.tIPStart.uIP.dwAddr & tNewItem.tIPStart.uMask.dwAddr) != 
                   (tItem.tIPStart.uIP.dwAddr & tItem.tIPStart.uMask.dwAddr))
                {
                    continue ;
                }
                vecRelatedIPRange.push_back(*itrDBRange);
                cIPv4Range.AddValidRange(tItem);
            }
        }
        
        if(outVInfo.empty() || cIPv4Range.GetIPv4AddrRange().empty())
        {// only one record, save it to DB;
            nRet = pOper->Add(cDbIPRange);
            if(VNET_DB_SUCCESS != nRet)
            {
                VNET_LOG(VNET_LOG_ERROR, 
                    "CIPv4PoolMgr::AddLogicNetworkIPv4Range: DB Add the logic_network's(UUID: %s) IP range[%s-%s:%s] failed, ret:%d.\n",
                    cMsg.m_strLogicNetworkUUID.c_str(),
                    cMsg.m_strIPv4Start.c_str(),
                    cMsg.m_strIPv4End.c_str(),
                    cMsg.m_strIPv4EndMask.c_str(),
                    nRet);
                VNetFormatString(strRetInfo, "Add the logic network's(UUID: %s) IP address range[%s-%s:%s] to the database failed.",
                      cMsg.m_strLogicNetworkUUID.c_str(),
                      cMsg.m_strIPv4Start.c_str(),
                      cMsg.m_strIPv4End.c_str(),
                      cMsg.m_strIPv4EndMask.c_str());
                return nRet;
            }
            return IPV4_RANGE_OPER_SUCCESS;
        }      

        // range add;
        if(0 != cIPv4Range.AddRange(tNewItem))
        {            
            VNET_LOG(VNET_LOG_ERROR, 
                "CIPv4PoolMgr::AddLogicNetworkIPv4Range: The IP range[0x%8.8x-0x%8.8x:0x%8.8x] calculate failed.\n",
                tNewItem.tIPStart.uIP.dwAddr, 
                tNewItem.tIPEnd.uIP.dwAddr, 
                tNewItem.tIPStart.uMask.dwAddr);
            VNetFormatString(strRetInfo, "Combining the calculation of logic network's(UUID: %s) IP address range[%s-%s:%s] failed.",
                cMsg.m_strLogicNetworkUUID.c_str(),
                cMsg.m_strIPv4Start.c_str(),
                cMsg.m_strIPv4End.c_str(),
                cMsg.m_strIPv4EndMask.c_str());
            return IPV4_RANGE_CALC_ERROR;
        }
        
        // DB Compare and save;Need Transaction;
        vector<TIPV4AddrRangeItem> &vecCalcRange = cIPv4Range.GetIPv4AddrRange();
        vector<TIPV4AddrRangeItem>::iterator itrCalcRange = vecCalcRange.begin();
        for(; itrCalcRange != vecCalcRange.end(); ++itrCalcRange)
        {            
            int32 bFind = 0;
            for(itrDBRange = vecRelatedIPRange.begin(); itrDBRange != vecRelatedIPRange.end(); ++itrDBRange)
            {
                TIPV4AddrRangeItem tIPRange;
                tIPRange.tIPStart.uIP.dwAddr = CIPv4Addr::H2NL((int32)(itrDBRange->GetBeginNum()));
                tIPRange.tIPStart.uMask.dwAddr = CIPv4Addr::H2NL((int32)(itrDBRange->GetMaskNum()));
                tIPRange.tIPEnd.uIP.dwAddr = CIPv4Addr::H2NL((int32)(itrDBRange->GetEndNum()));
                tIPRange.tIPEnd.uMask.dwAddr = tIPRange.tIPStart.uMask.dwAddr;
                if(tIPRange.tIPStart.uIP.dwAddr == itrCalcRange->tIPStart.uIP.dwAddr &&
                   tIPRange.tIPStart.uMask.dwAddr == itrCalcRange->tIPStart.uMask.dwAddr &&
                   tIPRange.tIPEnd.uIP.dwAddr == itrCalcRange->tIPEnd.uIP.dwAddr &&
                   tIPRange.tIPEnd.uMask.dwAddr == itrCalcRange->tIPEnd.uMask.dwAddr)
                {
                    bFind = 1;
                    vecRelatedIPRange.erase(itrDBRange);
                    break;
                }
            }
            
            if(0 == bFind)
            {                
                // Add To DB
                cDbIPRange.SetUuid(cMsg.m_strLogicNetworkUUID.c_str());
                cDbIPRange.SetBeginNum(CIPv4Addr::N2HL(itrCalcRange->tIPStart.uIP.dwAddr));
                cDbIPRange.SetEndNum(CIPv4Addr::N2HL(itrCalcRange->tIPEnd.uIP.dwAddr));
                cDbIPRange.SetMaskNum(CIPv4Addr::N2HL(itrCalcRange->tIPEnd.uMask.dwAddr));   
                cIPStart.Set(itrCalcRange->tIPStart);
                cDbIPRange.SetBeginStr(cIPStart.GetIP().c_str());                
                cIPEnd.Set(itrCalcRange->tIPEnd);
                cDbIPRange.SetEndStr(cIPEnd.GetIP().c_str());
                cDbIPRange.SetMaskStr(cIPEnd.GetMask().c_str());
                nRet = pOper->Add(cDbIPRange);
                if(VNET_DB_SUCCESS != nRet)
                {
                    VNET_LOG(VNET_LOG_ERROR, 
                        "CIPv4PoolMgr::AddLogicNetworkIPv4Range: DB Merge(add) logic_network's(UUID: %s) IP range[%s-%s:%s] failed, ret:%d.\n",
                        cMsg.m_strLogicNetworkUUID.c_str(),
                        cMsg.m_strIPv4Start.c_str(),
                        cMsg.m_strIPv4End.c_str(),
                        cMsg.m_strIPv4EndMask.c_str(),
                        nRet);
                    VNetFormatString(strRetInfo, "Add the logic network's(UUID: %s) IP address range[%s-%s:%s] to the database failed.",
                         cMsg.m_strLogicNetworkUUID.c_str(),
                         cMsg.m_strIPv4Start.c_str(),
                         cMsg.m_strIPv4End.c_str(),
                         cMsg.m_strIPv4EndMask.c_str());
                    return nRet;                
                }             
            }
        }
        // delete invalid record from db;
        for(itrDBRange = vecRelatedIPRange.begin(); itrDBRange != vecRelatedIPRange.end(); ++itrDBRange)
        {
            nRet = pOper->Del(*itrDBRange);
            if(VNET_DB_SUCCESS != nRet)
            {
                VNET_LOG(VNET_LOG_ERROR, 
                    "CIPv4PoolMgr::AddLogicNetworkIPv4Range: DB Merge(del) logic_network's(UUID: %s) IP range[%s-%s:%s] failed, ret:%d.\n",
                    cMsg.m_strLogicNetworkUUID.c_str(),
                    cMsg.m_strIPv4Start.c_str(),
                    cMsg.m_strIPv4End.c_str(),
                    cMsg.m_strIPv4EndMask.c_str(),
                    nRet);
                VNetFormatString(strRetInfo, "Merge the logic network's(UUID: %s) IP address range[%s-%s:%s] to the database failed.",
                     cMsg.m_strLogicNetworkUUID.c_str(),
                     cMsg.m_strIPv4Start.c_str(),
                     cMsg.m_strIPv4End.c_str(),
                     cMsg.m_strIPv4EndMask.c_str());
                return nRet;  
            }             
        }
        
        return IPV4_RANGE_OPER_SUCCESS;
    }

    int32 CIPv4PoolMgr::DelLogicNetworkIPv4Range(const CLogicNetworkIPv4RangeMsg &cMsg, string &strRetInfo)
    {
        if(!cMsg.IsValidRange())
        {
            VNET_LOG(VNET_LOG_ERROR, "CIPv4PoolMgr::DelLogicNetworkIPv4Range: The range config message is Invalid.\n");
            strRetInfo = "The IP address range config param is invalid.";
            return IPV4_RANGE_CFG_PARAM_ERROR;
        }
        
        CIPv4AddrRange cIPv4Range;
        // Get DB oper;
        CDBOperateLogicNetworkIpRange *pOper = GetDbILogicNetworkIpRange();
        if(NULL == pOper)
        {
            VNET_LOG(VNET_LOG_ERROR, "CIPv4PoolMgr::DelLogicNetworkIPv4Range: GetDbILogicNetworkIpRange() is NULL.\n");
            strRetInfo = "Get the IP address range database interface handler failed.";
            return IPV4_RANGE_GET_LG_NET_DB_INTF_ERROR;
        }

        // IP translate;
        CIPv4Addr cIPStart;        
        CIPv4Addr cIPEnd;   
        if(0 != cIPStart.Set(cMsg.m_strIPv4Start, cMsg.m_strIPv4StartMask) || 
           0 != cIPEnd.Set(cMsg.m_strIPv4End, cMsg.m_strIPv4EndMask))
        {
        
            VNET_LOG(VNET_LOG_ERROR, 
                "CIPv4PoolMgr::DelLogicNetworkIPv4Range: The logic_network's(UUID: %s) IP range[%s-%s:%s] format is invalid.\n",
                cMsg.m_strLogicNetworkUUID.c_str(),
                cMsg.m_strIPv4Start.c_str(),
                cMsg.m_strIPv4End.c_str(),
                cMsg.m_strIPv4EndMask.c_str());
            VNetFormatString(strRetInfo, "The logic network's(UUID: %s) IP address range[%s-%s:%s] format is invalid.",
                cMsg.m_strLogicNetworkUUID.c_str(),
                cMsg.m_strIPv4Start.c_str(),
                cMsg.m_strIPv4End.c_str(),
                cMsg.m_strIPv4EndMask.c_str());
            return IPV4_RANGE_CFG_PARAM_ERROR;
        }
        TIPV4AddrRangeItem tNewItem;
        cIPStart.Get(tNewItem.tIPStart);
        cIPEnd.Get(tNewItem.tIPEnd);
        if(!CIPv4AddrRange::IsValidIPv4Range(tNewItem))
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CIPv4PoolMgr::DelLogicNetworkIPv4Range: The logic_network's(UUID: %s) IP range[%s-%s:%s] is invalid.\n",
                cMsg.m_strLogicNetworkUUID.c_str(),
                cMsg.m_strIPv4Start.c_str(),
                cMsg.m_strIPv4End.c_str(),
                cMsg.m_strIPv4EndMask.c_str());
            VNetFormatString(strRetInfo, "The logic network's(UUID: %s) IP address range[%s-%s:%s] is invalid.",
                cMsg.m_strLogicNetworkUUID.c_str(),
                cMsg.m_strIPv4Start.c_str(),
                cMsg.m_strIPv4End.c_str(),
                cMsg.m_strIPv4EndMask.c_str());
            return IPV4_RANGE_INVALID;
        }
        
        if(!CIPv4AddrRange::IsSameSubnet(tNewItem))
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CIPv4PoolMgr::DelLogicNetworkIPv4Range: The logic_network's(UUID: %s) IP range[%s-%s:%s] isn't same subnet.\n",
                cMsg.m_strLogicNetworkUUID.c_str(),
                cMsg.m_strIPv4Start.c_str(),
                cMsg.m_strIPv4End.c_str(),
                cMsg.m_strIPv4EndMask.c_str());
            VNetFormatString(strRetInfo, "The logic network's(UUID: %s) IP address range[%s-%s:%s] is not same subnet.",
                cMsg.m_strLogicNetworkUUID.c_str(),
                cMsg.m_strIPv4Start.c_str(),
                cMsg.m_strIPv4End.c_str(),
                cMsg.m_strIPv4EndMask.c_str());
            return IPV4_RANGE_NOT_SAME_SUBNET;
        }
        // DB range Load;
        vector<CDbLogicNetworkIpRange> outVInfo;
        int32 nRet = pOper->Query(cMsg.m_strLogicNetworkUUID.c_str(), outVInfo);
        if(VNET_DB_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CIPv4PoolMgr::DelLogicNetworkIPv4Range: DB Query the logic_network's(UUID: %s) IP range failed, ret:%d.\n",
                cMsg.m_strLogicNetworkUUID.c_str(),
                nRet);
            VNetFormatString(strRetInfo, "Get the logic network's(UUID: %s) IP address range failed.",
                cMsg.m_strLogicNetworkUUID.c_str());
            return nRet;
        } 

        CDbLogicNetworkIpRange cDbIPRange;        
        vector<CDbLogicNetworkIpRange>::iterator itrDBRange;
        if(outVInfo.empty())
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CIPv4PoolMgr::DelLogicNetworkIPv4Range: The logic_network's(UUID: %s) IP range[%s-%s:%s] is not exist.\n",
                cMsg.m_strLogicNetworkUUID.c_str(),
                cMsg.m_strIPv4Start.c_str(),
                cMsg.m_strIPv4End.c_str(),
                cMsg.m_strIPv4EndMask.c_str());
            VNetFormatString(strRetInfo, "The logic network's(UUID: %s) IP address range[%s-%s:%s] is not configured.",
                  cMsg.m_strLogicNetworkUUID.c_str(),
                  cMsg.m_strIPv4Start.c_str(),
                  cMsg.m_strIPv4End.c_str(),
                  cMsg.m_strIPv4EndMask.c_str()); 
            return IPV4_RANGE_NOT_EXIST;
        }

        // Check the range is used by IP pool or not.
        int32 nOccNum = 0;
        nRet = pOper->QueryAllocNum(cMsg.m_strLogicNetworkUUID.c_str(), 
                             CIPv4Addr::N2HL(tNewItem.tIPStart.uIP.dwAddr),
                             CIPv4Addr::N2HL(tNewItem.tIPEnd.uIP.dwAddr),
                             nOccNum);
        if(VNET_DB_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CIPv4PoolMgr::DelLogicNetworkIPv4Range: call QueryAllocNum (%s, 0x%x, 0x%x) failed, ret:%d.\n",
                cMsg.m_strLogicNetworkUUID.c_str(),
                CIPv4Addr::N2HL(tNewItem.tIPStart.uIP.dwAddr),
                CIPv4Addr::N2HL(tNewItem.tIPEnd.uIP.dwAddr),
                nRet);
            VNetFormatString(strRetInfo, "Get the logic network's(UUID: %s) IP address range[%s-%s:%s] used state failed.",
                  cMsg.m_strLogicNetworkUUID.c_str(),
                  cMsg.m_strIPv4Start.c_str(),
                  cMsg.m_strIPv4End.c_str(),
                  cMsg.m_strIPv4EndMask.c_str()); 
            return nRet;
        }

        if(0 != nOccNum)
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CIPv4PoolMgr::DelLogicNetworkIPv4Range: The logic_network's(UUID: %s) IP range[%s-%s:%s] is occupied, num: %d.\n",
                cMsg.m_strLogicNetworkUUID.c_str(),
                cMsg.m_strIPv4Start.c_str(),
                cMsg.m_strIPv4End.c_str(),
                cMsg.m_strIPv4EndMask.c_str(),
                nOccNum);
            VNetFormatString(strRetInfo, "The logic network's(UUID: %s) IP address range[%s-%s:%s] is used.",
                  cMsg.m_strLogicNetworkUUID.c_str(),
                  cMsg.m_strIPv4Start.c_str(),
                  cMsg.m_strIPv4End.c_str(),
                  cMsg.m_strIPv4EndMask.c_str()); 
            return IPV4_RANGE_OCCUPIED;
        }
        
        vector<CDbLogicNetworkIpRange> vecRelatedIPRange;
        for(itrDBRange = outVInfo.begin(); itrDBRange != outVInfo.end(); ++itrDBRange)
        {
            TIPV4AddrRangeItem tItem;
            tItem.tIPStart.uIP.dwAddr = CIPv4Addr::H2NL((int32)(itrDBRange->GetBeginNum()));
            tItem.tIPStart.uMask.dwAddr = CIPv4Addr::H2NL((int32)(itrDBRange->GetMaskNum()));
            tItem.tIPEnd.uIP.dwAddr = CIPv4Addr::H2NL((int32)(itrDBRange->GetEndNum()));
            tItem.tIPEnd.uMask.dwAddr = tItem.tIPStart.uMask.dwAddr;
            // shortest mask match network segment;
            if(tItem.tIPStart.uMask.dwAddr != tNewItem.tIPEnd.uMask.dwAddr)
            {
                uint32 dwMinMask = (tItem.tIPStart.uMask.dwAddr < tNewItem.tIPStart.uMask.dwAddr) ? 
                                    tItem.tIPStart.uMask.dwAddr : tNewItem.tIPStart.uMask.dwAddr;
                if((tNewItem.tIPStart.uIP.dwAddr & dwMinMask) == 
                   (tItem.tIPStart.uIP.dwAddr & dwMinMask))
                {
                    VNET_LOG(VNET_LOG_ERROR, 
                        "CIPv4PoolMgr::DelLogicNetworkIPv4Range: the logic_network's(UUID: %s) IP range[%s-%s:%s] conflict with [%s-%s:%s].\n",
                        cMsg.m_strLogicNetworkUUID.c_str(),
                        cMsg.m_strIPv4Start.c_str(),
                        cMsg.m_strIPv4End.c_str(),
                        cMsg.m_strIPv4EndMask.c_str(),
                        itrDBRange->GetBeginStr().c_str(),
                        itrDBRange->GetEndStr().c_str(),
                        itrDBRange->GetMaskStr().c_str());
                    VNetFormatString(strRetInfo, "The logic_network's(UUID: %s) IP range[%s-%s:%s] conflict with [%s-%s:%s].",
                        cMsg.m_strLogicNetworkUUID.c_str(),
                        cMsg.m_strIPv4Start.c_str(),
                        cMsg.m_strIPv4End.c_str(),
                        cMsg.m_strIPv4EndMask.c_str(),
                        itrDBRange->GetBeginStr().c_str(),
                        itrDBRange->GetEndStr().c_str(),
                        itrDBRange->GetMaskStr().c_str());
                    return IPV4_RANGE_INVALID;
                }
            }
            else
            {
                if((tNewItem.tIPStart.uIP.dwAddr & tNewItem.tIPStart.uMask.dwAddr) != 
                   (tItem.tIPStart.uIP.dwAddr & tItem.tIPStart.uMask.dwAddr))
                {
                    continue ;
                }
                vecRelatedIPRange.push_back(*itrDBRange);
                cIPv4Range.AddValidRange(tItem);
            }            
        }    

        if(cIPv4Range.GetIPv4AddrRange().empty())
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CIPv4PoolMgr::DelLogicNetworkIPv4Range: The logic_network's(UUID: %s) IP range[%s-%s:%s] is not exist.\n",
                cMsg.m_strLogicNetworkUUID.c_str(),
                cMsg.m_strIPv4Start.c_str(),
                cMsg.m_strIPv4End.c_str(),
                cMsg.m_strIPv4EndMask.c_str());
            VNetFormatString(strRetInfo, "The logic network's(UUID: %s) IP address range[%s-%s:%s] is not configured.",
                  cMsg.m_strLogicNetworkUUID.c_str(),
                  cMsg.m_strIPv4Start.c_str(),
                  cMsg.m_strIPv4End.c_str(),
                  cMsg.m_strIPv4EndMask.c_str()); 
            return IPV4_RANGE_NOT_EXIST;
        }
        
        // range del;
        if(0 != cIPv4Range.DelRange(tNewItem))
        {            
            VNET_LOG(VNET_LOG_ERROR, 
                "CIPv4PoolMgr::DelLogicNetworkIPv4Range: The IP range[0x%8.8x-0x%8.8x:0x%8.8x] calculate failed.\n",
                tNewItem.tIPStart.uIP.dwAddr, 
                tNewItem.tIPEnd.uIP.dwAddr, 
                tNewItem.tIPStart.uMask.dwAddr);
            VNetFormatString(strRetInfo, "Combining the calculation of logic network's(UUID: %s) IP address range[%s-%s:%s] failed.",
                cMsg.m_strLogicNetworkUUID.c_str(),
                cMsg.m_strIPv4Start.c_str(),
                cMsg.m_strIPv4End.c_str(),
                cMsg.m_strIPv4EndMask.c_str());
            return IPV4_RANGE_CALC_ERROR;
        }
        
        // DB Compare and save;Need Transaction;
        vector<TIPV4AddrRangeItem> &vecCalcRange = cIPv4Range.GetIPv4AddrRange();
        vector<TIPV4AddrRangeItem>::iterator itrCalcRange = vecCalcRange.begin();
        for(; itrCalcRange != vecCalcRange.end(); ++itrCalcRange)
        {            
            int32 bFind = 0;
            for(itrDBRange = vecRelatedIPRange.begin(); itrDBRange != vecRelatedIPRange.end(); ++itrDBRange)
            {
                TIPV4AddrRangeItem tIPRange;
                tIPRange.tIPStart.uIP.dwAddr = CIPv4Addr::H2NL((int32)(itrDBRange->GetBeginNum()));
                tIPRange.tIPStart.uMask.dwAddr = CIPv4Addr::H2NL((int32)(itrDBRange->GetMaskNum()));
                tIPRange.tIPEnd.uIP.dwAddr = CIPv4Addr::H2NL((int32)(itrDBRange->GetEndNum()));
                tIPRange.tIPEnd.uMask.dwAddr = tIPRange.tIPStart.uMask.dwAddr;
                if(tIPRange.tIPStart.uIP.dwAddr == itrCalcRange->tIPStart.uIP.dwAddr &&
                   tIPRange.tIPStart.uMask.dwAddr == itrCalcRange->tIPStart.uMask.dwAddr &&
                   tIPRange.tIPEnd.uIP.dwAddr == itrCalcRange->tIPEnd.uIP.dwAddr &&
                   tIPRange.tIPEnd.uMask.dwAddr == itrCalcRange->tIPEnd.uMask.dwAddr)
                {
                    bFind = 1;
                    vecRelatedIPRange.erase(itrDBRange);
                    break;
                }
            }
            
            if(0 == bFind)
            {                
                // Add To DB
                cDbIPRange.SetUuid(cMsg.m_strLogicNetworkUUID.c_str());
                cDbIPRange.SetBeginNum(CIPv4Addr::N2HL(itrCalcRange->tIPStart.uIP.dwAddr));
                cDbIPRange.SetEndNum(CIPv4Addr::N2HL(itrCalcRange->tIPEnd.uIP.dwAddr));
                cDbIPRange.SetMaskNum(CIPv4Addr::N2HL(itrCalcRange->tIPEnd.uMask.dwAddr));   
                cIPStart.Set(itrCalcRange->tIPStart);
                cDbIPRange.SetBeginStr(cIPStart.GetIP().c_str());                
                cIPEnd.Set(itrCalcRange->tIPEnd);
                cDbIPRange.SetEndStr(cIPEnd.GetIP().c_str());
                cDbIPRange.SetMaskStr(cIPEnd.GetMask().c_str());
                nRet = pOper->Add(cDbIPRange);
                if(VNET_DB_SUCCESS != nRet)
                {
                    VNET_LOG(VNET_LOG_ERROR, 
                        "CIPv4PoolMgr::DelLogicNetworkIPv4Range: DB Merge(add) the logic_network's(UUID: %s) IP range[%s-%s:%s] failed, ret:%d.\n",
                        cMsg.m_strLogicNetworkUUID.c_str(),
                        cMsg.m_strIPv4Start.c_str(),
                        cMsg.m_strIPv4End.c_str(),
                        cMsg.m_strIPv4EndMask.c_str(),
                        nRet);
                    VNetFormatString(strRetInfo, "Add the logic network's(UUID: %s) IP address range[%s-%s:%s] to the database failed.",
                         cMsg.m_strLogicNetworkUUID.c_str(),
                         cMsg.m_strIPv4Start.c_str(),
                         cMsg.m_strIPv4End.c_str(),
                         cMsg.m_strIPv4EndMask.c_str());
                    return nRet;                
                }             
            }
        }
        // delete invalid record from db;
        for(itrDBRange = vecRelatedIPRange.begin(); itrDBRange != vecRelatedIPRange.end(); ++itrDBRange)
        {
            nRet = pOper->Del(*itrDBRange);
            if(VNET_DB_SUCCESS != nRet)
            {
                VNET_LOG(VNET_LOG_ERROR, 
                    "CIPv4PoolMgr::DelLogicNetworkIPv4Range: DB Merge(del) the logic_network's(UUID: %s) IP range[%s-%s:%s] failed, ret:%d.\n",
                    cMsg.m_strLogicNetworkUUID.c_str(),
                    cMsg.m_strIPv4Start.c_str(),
                    cMsg.m_strIPv4End.c_str(),
                    cMsg.m_strIPv4EndMask.c_str(),
                    nRet);
                VNetFormatString(strRetInfo, "Merge the logic network's(UUID: %s) IP address range[%s-%s:%s] to the database failed.",
                     cMsg.m_strLogicNetworkUUID.c_str(),
                     cMsg.m_strIPv4Start.c_str(),
                     cMsg.m_strIPv4End.c_str(),
                     cMsg.m_strIPv4EndMask.c_str());
                return nRet; 
            }             
        }
        
        return IPV4_RANGE_OPER_SUCCESS;
    }  

    int32 CIPv4PoolMgr::GetLogiNetIPv4Range(const string &cstrLogicNetworkID, CIPv4AddrRange &cIPv4Range)
    {
        if(cstrLogicNetworkID.empty())
        {
            VNET_LOG(VNET_LOG_ERROR, "CIPv4PoolMgr::GetLogiNetIPv4Range: cstrLogicNetworkID is NULL.\n");
            return -1;
        }
        // Get DB oper;
        CDBOperateLogicNetworkIpRange *pOper = GetDbILogicNetworkIpRange();
        if(NULL == pOper)
        {
            VNET_LOG(VNET_LOG_ERROR, "CIPv4PoolMgr::GetLogiNetIPv4Range: GetDbILogicNetworkIpRange() is NULL.\n");
            return -1;
        }
        // Load Range;
        vector<CDbLogicNetworkIpRange> outVInfo;
        int32 nRet = pOper->Query(cstrLogicNetworkID.c_str(), outVInfo);
        if(VNET_DB_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CIPv4PoolMgr::GetLogiNetIPv4Range: DB Query logic_network's(UUID: %s) IP range failed, ret:%d.\n",
                cstrLogicNetworkID.c_str(), nRet);
            return nRet;
        } 
        
        vector<CDbLogicNetworkIpRange>::iterator itrDBRange;
        for(itrDBRange = outVInfo.begin(); itrDBRange != outVInfo.end(); ++itrDBRange)
        {
            TIPV4AddrRangeItem tItem;
            tItem.tIPStart.uIP.dwAddr = CIPv4Addr::H2NL((int32)(itrDBRange->GetBeginNum()));
            tItem.tIPStart.uMask.dwAddr = CIPv4Addr::H2NL((int32)(itrDBRange->GetMaskNum()));
            tItem.tIPEnd.uIP.dwAddr = CIPv4Addr::H2NL((int32)(itrDBRange->GetEndNum()));
            tItem.tIPEnd.uMask.dwAddr = tItem.tIPStart.uMask.dwAddr;
            cIPv4Range.AddValidRange(tItem);
        }
        return 0;
    }

    int32 CIPv4PoolMgr::GetNetplaneIPv4Range(const string &cstrNetplaneID, CIPv4AddrRange &cIPv4Range)
    {
        if(cstrNetplaneID.empty())
        {
            VNET_LOG(VNET_LOG_ERROR, "CIPv4PoolMgr::GetNetplaneIPv4Range: cstrNetplaneID is NULL.\n");
            return -1;
        }
        // Get DB oper;
        CDBOperateNetplaneIpRange *pOper = GetDbINetplaneIpRange();
        if(NULL == pOper)
        {
            VNET_LOG(VNET_LOG_ERROR, "CIPv4PoolMgr::GetNetplaneIPv4Range: GetDbINetplaneIpRange() is NULL.\n");
            return -1;
        }
        // Load Range;
        vector<CDbNetplaneIpRange> outVInfo;
        int32 nRet = pOper->Query(cstrNetplaneID.c_str(), outVInfo);
        if(VNET_DB_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CIPv4PoolMgr::GetNetplaneIPv4Range: DB Query Netplane's(UUID: %s) IP range failed, ret:%d.\n",
                cstrNetplaneID.c_str(), nRet);
            return nRet;
        } 
        
        vector<CDbNetplaneIpRange>::iterator itrDBRange;
        for(itrDBRange = outVInfo.begin(); itrDBRange != outVInfo.end(); ++itrDBRange)
        {
            TIPV4AddrRangeItem tItem;
            tItem.tIPStart.uIP.dwAddr = CIPv4Addr::H2NL((int32)(itrDBRange->GetBeginNum()));
            tItem.tIPStart.uMask.dwAddr = CIPv4Addr::H2NL((int32)(itrDBRange->GetMaskNum()));
            tItem.tIPEnd.uIP.dwAddr = CIPv4Addr::H2NL((int32)(itrDBRange->GetEndNum()));
            tItem.tIPEnd.uMask.dwAddr = tItem.tIPStart.uMask.dwAddr;
            cIPv4Range.AddValidRange(tItem);
        }
        return 0;
    }

    void CIPv4PoolMgr::DbgShowNetplaneIPRange(const string &cstrNetplaneUUID)
    {
        // show IP range;
        CIPv4AddrRange cIPRange;            

        if(0 != GetNetplaneIPv4Range(cstrNetplaneUUID, cIPRange))
        {
            cout << " call GetNetplaneIPv4Range(" << cstrNetplaneUUID <<") failed." << endl;
            return ;
        }
        if(!cIPRange.GetIPv4AddrRange().empty())
        {
            cIPRange.DbgShowIPv4AddrRange();
            cout << "--------------------------------------------------------" << endl;
        }
        
        return ;
    }

    void CIPv4PoolMgr::DbgShowLogiNetIPPool(const string &cstrLogiNetUUID)
    {
        // show IP range;
        CIPv4AddrRange cIPRange; 

        if(0 != GetLogiNetIPv4Range(cstrLogiNetUUID, cIPRange))
        {
            cout << " call GetLogiNetIPv4Range(" << cstrLogiNetUUID <<") failed." << endl;
            return ;
        }
        
        if(cIPRange.GetIPv4AddrRange().empty())
        {        
            cout << "LN:" << cstrLogiNetUUID << ", The IP range data isn't exist." << endl;
            return ;
        }
        
        cIPRange.DbgShowIPv4AddrRange();
        cout << "--------------------------------------------------------" << endl;
        
        // show IP addr;
        CIPv4AddrPool cIPv4Pool;
        if(0 != cIPv4Pool.Init(cstrLogiNetUUID))
        {
            cout << "Init(" << cstrLogiNetUUID << ") IP pool failed." << endl;
            return ;
        }
        cIPv4Pool.DbgShowData();
        return ;
    }

    void VNetDbgTestNetplaneIPRangeOper(const char* cstrNetplaneUUID, 
                                 const char* cstrIPStart, 
                                 const char* cstrIPEnd, 
                                 const char* cstrIPMask, 
                                 uint32 dwOper)
    {
        if(NULL == cstrNetplaneUUID || 
           NULL == cstrIPStart ||
           NULL == cstrIPEnd ||
           NULL == cstrIPMask)
        {
            cout << "Input param error." << endl;
            return ;
        }

        if((dwOper <= EN_INVALID_IPV4_RANGE_OPER) || (dwOper >= EN_IPV4_RANGE_OPER_ALL))
        {
            cout << "Operation Code: Add[" << EN_ADD_IPV4_RANGE
                 << "], Del[" << EN_DEL_IPV4_RANGE
                 << "]." <<endl;
            return ;
        }
        
        MessageUnit tempmu(TempUnitName("TestNPIPRangeOper"));
        tempmu.Register();
        CNetplaneIPv4RangeMsg cMsg;
        string strTmp;
        strTmp.assign(cstrNetplaneUUID);
        cMsg.m_strNetplaneUUID = strTmp;
        strTmp.clear();
        strTmp.assign(cstrIPStart);
        cMsg.m_strIPv4Start = strTmp;
        strTmp.clear();
        strTmp.assign(cstrIPEnd);
        cMsg.m_strIPv4End = strTmp;
        strTmp.clear();
        strTmp.assign(cstrIPMask);
        cMsg.m_strIPv4StartMask = cMsg.m_strIPv4EndMask = strTmp;
        cMsg.m_dwOper = dwOper;
        MessageOption option(MU_VNM, EV_VNETLIB_NP_IPV4_RANGE_CFG, 0, 0);
        tempmu.Send(cMsg,option);
        
        return ;
    }
    DEFINE_DEBUG_FUNC(VNetDbgTestNetplaneIPRangeOper);

    void VNetDbgTestLogiNetIPRangeOper(const char* cstrLogiNetUUID, 
                                 const char* cstrIPStart, 
                                 const char* cstrIPEnd,
                                 const char* cstrIPMask, 
                                 uint32 dwOper)
    {
        if(NULL == cstrLogiNetUUID || 
           NULL == cstrIPStart ||
           NULL == cstrIPEnd ||
           NULL == cstrIPMask)
        {
            cout << "Input param error." << endl;
            return ;
        }

        if((dwOper <= EN_INVALID_IPV4_RANGE_OPER) || (dwOper >= EN_IPV4_RANGE_OPER_ALL))
        {
            cout << "Operation Code: Add[" << EN_ADD_IPV4_RANGE
                 << "], Del[" << EN_DEL_IPV4_RANGE
                 << "]." <<endl;
            return ;
        }
        
        MessageUnit tempmu(TempUnitName("TestLogiNetIPRangeOper"));
        tempmu.Register();
        CLogicNetworkIPv4RangeMsg cMsg;
        string strTmp;
        strTmp.assign(cstrLogiNetUUID);
        cMsg.m_strLogicNetworkUUID = strTmp;
        strTmp.clear();
        strTmp.assign(cstrIPStart);
        cMsg.m_strIPv4Start = strTmp;
        strTmp.clear();
        strTmp.assign(cstrIPEnd);
        cMsg.m_strIPv4End = strTmp;
        strTmp.clear();
        strTmp.assign(cstrIPMask);
        cMsg.m_strIPv4StartMask = cMsg.m_strIPv4EndMask = strTmp;
        cMsg.m_dwOper = dwOper;
        MessageOption option(MU_VNM, EV_VNETLIB_LOGI_NET_IPV4_RANGE_CFG, 0, 0);       
        tempmu.Send(cMsg,option);
        
        return ;
    }
    DEFINE_DEBUG_FUNC(VNetDbgTestLogiNetIPRangeOper);

    void VNetDbgTestAllocIPAddr(const char *cstrLogicNetworkID)
    {
        if(NULL == cstrLogicNetworkID)
        {
            cout << "Input param error." << endl;
            return ;
        }
        CVNetVnicMem cVNic;
        string strTmp;
        strTmp.assign(cstrLogicNetworkID);
        cVNic.SetLogicNetworkUuid(strTmp);
        
        CIPv4PoolMgr *pMgr = CIPv4PoolMgr::GetInstance();
        vector<CVNetVnicMem> vecVNetVnic;

        for(int32 i = 1; i <= 10; ++i)
        {            
            cVNic.SetNicIndex(i);
            vecVNetVnic.push_back(cVNic);
        }
        
        if(pMgr)
        {
            CVNetVmMem cVNetVmMem;
            cVNetVmMem.SetVecVnicMem(vecVNetVnic);
            pMgr->AllocIPAddrForVnic(cVNetVmMem);
        }
        return ;
    }
    DEFINE_DEBUG_FUNC(VNetDbgTestAllocIPAddr);

    void VNetDbgTestFreeIPAddr(const char *cstrLogicNetworkID, const char *cstrIP, const char *cstrMask)
    {
        if(NULL == cstrLogicNetworkID || 
           NULL == cstrIP || 
           NULL == cstrMask)
        {
            cout << "Input param error." << endl;
            return ;
        }
        CVNetVnicMem cVNic;
        string strTmp;
        strTmp.assign(cstrLogicNetworkID);
        cVNic.SetLogicNetworkUuid(strTmp);
        
        strTmp.clear();
        strTmp.assign(cstrIP);
        cVNic.SetIp(strTmp);
        strTmp.clear();
        strTmp.assign(cstrMask);
        cVNic.SetMask(strTmp);
        cVNic.SetNicIndex(1);
        
        vector<CVNetVnicMem> vecVNetVnic;
        vecVNetVnic.push_back(cVNic);
        CIPv4PoolMgr *pMgr = CIPv4PoolMgr::GetInstance();
        if(pMgr)
        {
            CVNetVmMem cVNetVmMem;
            cVNetVmMem.SetVecVnicMem(vecVNetVnic);
            pMgr->FreeIPAddrForVnic(cVNetVmMem);
        }
        return ;
    }
    DEFINE_DEBUG_FUNC(VNetDbgTestFreeIPAddr);
    
    void VNetDbgShowNetplaneIPRange(const char *cstrNetplaneUUID)
    {
        if(NULL == cstrNetplaneUUID)
        {
            cout << "Input param error." << endl;
            return ;
        }
        
        CIPv4PoolMgr *pMgr = CIPv4PoolMgr::GetInstance();
        if(pMgr)
        {
            string strTmp;
            strTmp.assign(cstrNetplaneUUID);
            pMgr->DbgShowNetplaneIPRange(strTmp);
        }
        
        return ;
    }
    DEFINE_DEBUG_FUNC(VNetDbgShowNetplaneIPRange);

    void VNetDbgShowLogiNetIPPool(const char *cstrLogiNetUUID)
    {
        if(NULL == cstrLogiNetUUID)
        {
            cout << "Input param error." << endl;
            return ;
        }
        
        CIPv4PoolMgr *pMgr = CIPv4PoolMgr::GetInstance();
        if(pMgr)
        {
            string strTmp;
            strTmp.assign(cstrLogiNetUUID);
            pMgr->DbgShowLogiNetIPPool(strTmp);
        }
        
        return ;
    }
    DEFINE_DEBUG_FUNC(VNetDbgShowLogiNetIPPool);

    void VNetDbgSetIPPoolPrint(BOOL bOpen)
    {
        CIPv4PoolMgr *pMgr = CIPv4PoolMgr::GetInstance();
        if(pMgr)
        {
            pMgr->SetDbgInfFlg(bOpen);
        }
        
        return ;
    }
    DEFINE_DEBUG_FUNC(VNetDbgSetIPPoolPrint);

    void VNetDbgTestNetplaneAllocIPAddr(const char *cstrNetplaneID)
    {
        if(NULL == cstrNetplaneID)
        {
            cout << "Input param error." << endl;
            return ;
        }

        string strTmp;
        strTmp.assign(cstrNetplaneID);

        string strIP;
        string strMask;
        
        CIPv4PoolMgr *pMgr = CIPv4PoolMgr::GetInstance();
        if(!pMgr)
        {
            cout << "CIPv4PoolMgr::GetInstance error." << endl;
            return ;            
        }

        if (0!=pMgr->NetplaneAllocIPAddr(strTmp, strIP, strMask))
        {
            cout << "CIPv4PoolMgr::NetplaneAllocIPAddr error." << endl;
            return ;            
        }

        cout << "IP:" << strIP << "  Mask:" << strMask << " has been alloced!" << endl;

        return ;
    }
    DEFINE_DEBUG_FUNC(VNetDbgTestNetplaneAllocIPAddr);


    void VNetDbgTestNetplaneFreeIPAddr(const char *cstrNetplaneID, const char *cstrIP, const char *cstrMask)
    {
        if(NULL == cstrNetplaneID || 
           NULL == cstrIP || 
           NULL == cstrMask)
        {
            cout << "Input param error." << endl;
            return ;
        }
        CVNetVnicMem cVNic;
        string strNetplaneUuid;
        strNetplaneUuid.assign(cstrNetplaneID);
        
        string strIP;
        strIP.assign(cstrIP);
        
        string strMask;
        strMask.assign(cstrMask);
        
        CIPv4PoolMgr *pMgr = CIPv4PoolMgr::GetInstance();
        if(!pMgr)
        {
            cout << "CIPv4PoolMgr::GetInstance error." << endl;
            return ;            
        }

        if (0!=pMgr->NetplaneFreeIPAddr(strNetplaneUuid, strIP, strMask))
        {
            cout << "CIPv4PoolMgr::NetplaneFreeIPAddr error." << endl;
            return ;            
        }

        cout << "IP:" << strIP << "  Mask:" << strMask << " has been freed!" << endl;
        
        return ;
    }
    DEFINE_DEBUG_FUNC(VNetDbgTestNetplaneFreeIPAddr);
    

}// end namespace zte_tecs

