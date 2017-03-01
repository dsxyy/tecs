/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：mac_pool_mgr.cpp
* 文件标识：
* 内容摘要：CMACPoolMgr类实现文件
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

#include "vnet_db_proc.h"
#include "vnet_db_mgr.h"
#include "vnet_db_netplane_macpool.h"
#include "vnet_db_netplane_macrange.h"

#include "ipv4_addr.h"
#include "ipv4_addr_range.h"

#include "vnet_vnic.h"

#include "logic_network.h"
#include "logic_network_mgr.h"

#include "port_group_mgr.h"

#include "mac_addr.h"
#include "mac_range.h"
#include "mac_pool.h"
#include "mac_pool_mgr.h"


namespace zte_tecs
{
    CMACPoolMgr *CMACPoolMgr::s_pInstance = NULL;
    
    CMACPoolMgr::CMACPoolMgr()
    {
        m_bOpenDbgInf = VNET_FALSE;
        m_pMU = NULL;
    }
    
    CMACPoolMgr::~CMACPoolMgr()
    {
        m_pMU = NULL;
    }

    int32 CMACPoolMgr::AllocMACForVnic(CVNetVmMem & cVNetVmMem)
    {
        CPortGroupMgr *pPGMgr = CPortGroupMgr::GetInstance();
        if(NULL == pPGMgr)
        {        
            VNET_LOG(VNET_LOG_ERROR, "CMACPoolMgr::AllocMACForVnic: CPortGroupMgr instance is NULL.\n");
            return -1;
        }      
        
        // Need transaction;
        vector<CVNetVnicMem> vecVNetVnic;
        cVNetVmMem.GetVecVnicMem(vecVNetVnic);
        vector<CVNetVnicMem>::iterator itr = vecVNetVnic.begin();
        for( ; itr != vecVNetVnic.end(); ++itr)
        {
            // 防止重复调用重复分配;
            string strVNicMAC = itr->GetMac();
            if(!strVNicMAC.empty())
            {
                VNET_LOG(VNET_LOG_INFO, "CMACPoolMgr::AllocMACForVnic: The MAC(%s) is allocated.\n", 
                    strVNicMAC.c_str()); 
                continue ;
            }

            string strNetplaneUUID;
            string strPGID = itr->GetVmPgUuid();
            if(PORTGROUP_OPER_SUCCESS != pPGMgr->GetPGNetplane(strPGID, strNetplaneUUID))
            {        
                VNET_LOG(VNET_LOG_ERROR, "CMACPoolMgr::AllocMACForVnic: Call GetPGNetplane(%s) failed.\n", 
                    strPGID.c_str());            
                return -1;
            }
            
            CMACRange cOUI1MACRange;            
            CMACRange cOUI2MACRange;
            if(0 != GetMACRange(strNetplaneUUID, cOUI1MACRange, cOUI2MACRange))
            {
                VNET_LOG(VNET_LOG_ERROR, "CMACPoolMgr::AllocMACForVnic: Call GetMACRange(%s) failed.\n", 
                    strNetplaneUUID.c_str());  
                return -1;
            }
            
            CMACPool cMACPool;
            if(0 != cMACPool.Init(strNetplaneUUID, &cOUI1MACRange, &cOUI2MACRange))
            {
                VNET_LOG(VNET_LOG_ERROR, "CMACPoolMgr::AllocMACForVnic: Call cMACPool.Init(%s...) failed.\n", 
                    strNetplaneUUID.c_str());  
                return -1;
            } 
            
            string strMAC;
            if(0 != cMACPool.AllocMAC(itr->GetNicIndex(), strMAC))
            {
                VNET_LOG(VNET_LOG_ERROR, "CMACPoolMgr::AllocMACForVnic: Allocate mac(vmid:%ld, NicIndex:%d) failed.\n", 
                    itr->GetVmId(), itr->GetNicIndex());  
                return -1;
            }
            itr->SetMac(strMAC); 
            cVNetVmMem.SetVecVnicMem(vecVNetVnic);
            VNET_LOG(VNET_LOG_INFO, "CMACPool::AllocMACForVnic: The MAC [%s] is allocated!\n", 
                strMAC.c_str());
        }
        
        //cVNetVmMem.SetVecVnicMem(vecVNetVnic);
        
        return 0;
    }

    int32 CMACPoolMgr::FreeMACForVnic(CVNetVmMem & cVNetVmMem)
    {
        CPortGroupMgr *pPGMgr = CPortGroupMgr::GetInstance();
        if(NULL == pPGMgr)
        {        
            VNET_LOG(VNET_LOG_ERROR, "CMACPoolMgr::FreeMACForVnic: CPortGroupMgr instance is NULL.\n");
            return -1;
        }      
        
        // Need transaction;
        vector<CVNetVnicMem> vecVNetVnic;
        cVNetVmMem.GetVecVnicMem(vecVNetVnic);
        vector<CVNetVnicMem>::iterator itr = vecVNetVnic.begin();
        for( ; itr != vecVNetVnic.end(); ++itr)
        {
            string strVNicMAC = itr->GetMac();
            if(strVNicMAC.empty())
            {
                VNET_LOG(VNET_LOG_INFO, "CMACPoolMgr::FreeMACForVnic: The MAC is empty, visid:%s.\n", 
                    itr->GetVsiIdValue().c_str()); 
                continue ;
            }           
            
            string strNetplaneUUID;
            string strPGID = itr->GetVmPgUuid();
            if(PORTGROUP_OPER_SUCCESS != pPGMgr->GetPGNetplane(strPGID, strNetplaneUUID))
            {        
                VNET_LOG(VNET_LOG_ERROR, "CMACPoolMgr::FreeMACForVnic: Call GetPGNetplane(%s) failed.\n", 
                    strPGID.c_str());            
                return -1;
            }

            CMACPool cMACPool;
            if(0 != cMACPool.Init(strNetplaneUUID))
            {
                VNET_LOG(VNET_LOG_ERROR, "CMACPoolMgr::FreeMACForVnic: Call cMACPool.Init(%s...) failed.\n", 
                    strNetplaneUUID.c_str());  
                return -1;
            } 
            
            if(0 != cMACPool.FreeMAC(strVNicMAC))
            {
                VNET_LOG(VNET_LOG_ERROR, "CMACPoolMgr::FreeMACForVnic: Free VNIC(VSIID:%s) MAC(%s) failed.\n", 
                    itr->GetVsiIdValue().c_str(), strVNicMAC.c_str());  
                return -1;
            }            
            VNET_LOG(VNET_LOG_INFO, "CMACPoolMgr::FreeMACForVnic: Free VNIC(VSIID:%s) MAC(%s) successfully.\n", 
                itr->GetVsiIdValue().c_str(), strVNicMAC.c_str()); 
            strVNicMAC.clear();
            itr->SetMac(strVNicMAC);

        }
        cVNetVmMem.SetVecVnicMem(vecVNetVnic);
        
        return 0;
    }
 
    int32 CMACPoolMgr::Init(MessageUnit *pMu)
    {
        if (pMu == NULL)
        {
            VNET_LOG(VNET_LOG_ERROR, "CMACPoolMgr::Init: pMu is NULL.\n");
            return -1;
        }
        
        m_pMU = pMu;
        
        return 0;
    }
        
    void CMACPoolMgr::MessageEntry(const MessageFrame &message)
    {
        switch (message.option.id())
        {
        case EV_VNETLIB_MAC_RANGE_CFG:
            {
                ProcMACRangeMsg(message);
            }
            break;
        default:
            {
                VNET_ASSERT(0);
            }
        }
        
        return ;
    }
    
    int32 CMACPoolMgr::ProcMACRangeMsg(const MessageFrame &message)
    {
        if(NULL == m_pMU)
        {
            VNET_ASSERT(0);
            return -1;
        }
        
        CMACRangeMsg cMsg;
        cMsg.deserialize(message.data);
        string strRetInfo;
        int32 nRet = 0;
        MessageOption option(message.option.sender(), EV_VNETLIB_MAC_RANGE_CFG, 0, 0);
        
        if(m_bOpenDbgInf)
        {
            cMsg.Print();
        }
        
        if(!cMsg.IsValidOper())
        {
            VNET_LOG(VNET_LOG_WARN, "CMACPoolMgr::ProcMACRangeMsg: Msg operation code (%d) is invalid.\n", 
                cMsg.m_dwOper);
            VNetFormatString(strRetInfo, "Msg operation code (%d) is invalid.\n", cMsg.m_dwOper);
            cMsg.SetResult(MAC_RANGE_CFG_PARAM_ERROR);
            cMsg.SetResultinfo(strRetInfo);
            m_pMU->Send(cMsg, option);
            return -1;
        }
        
        switch(cMsg.m_dwOper)
        {
        case EN_ADD_MAC_RANGE:
            {
                nRet = AddMACRange(cMsg, strRetInfo);
            }
            break;
        case EN_DEL_MAC_RANGE:
            {
                nRet = DelMACRange(cMsg, strRetInfo);
            }
            break;
        default:
            {
                VNET_ASSERT(0);
                strRetInfo = "The mac range config param is invalid.";
                nRet = MAC_RANGE_CFG_PARAM_ERROR;
            }
        }
        
        cMsg.SetResult(nRet);
        cMsg.SetResultinfo(strRetInfo);
        m_pMU->Send(cMsg, option);
        
        return 0;
    }

    int32 CMACPoolMgr::AddMACRange(const CMACRangeMsg &cMsg, string &strRetInfo)
    {
        if(cMsg.m_strNetplaneUUID.empty())
        {
            VNET_ASSERT(0);
            strRetInfo = "The MAC range config param is invalid.";
            return MAC_RANGE_CFG_PARAM_ERROR;
        }
        
        if(!cMsg.IsValidRange())
        {            
            VNET_LOG(VNET_LOG_WARN, "CMACPoolMgr::AddMACRange: The MAC range(%s-%s) is invalid.\n", 
                cMsg.m_strMACStart.c_str(), cMsg.m_strMACEnd.c_str());
            VNetFormatString(strRetInfo, "The MAC range(%s-%s) is invalid.\n", 
                cMsg.m_strMACStart.c_str(), cMsg.m_strMACEnd.c_str());
            return MAC_RANGE_CFG_PARAM_ERROR;
        }
        // MAC Translate;        
        CMACAddr cMACAddr;
        TMACRangeItem tNewItem;
        if(0 !=cMACAddr.Set(cMsg.m_strMACStart))
        {
            VNET_LOG(VNET_LOG_WARN, "CMACPoolMgr::AddMACRange: The MAC range(%s) is invalid.\n", 
                cMsg.m_strMACStart.c_str());
            VNetFormatString(strRetInfo, "The MAC(%s) is invalid.\n", cMsg.m_strMACStart.c_str());
            return MAC_PARSE_ERROR;
        }        
        cMACAddr.GetUInt32MAC(tNewItem.tStartMAC);
        if(0 != cMACAddr.Set(cMsg.m_strMACEnd))
        {
            VNET_LOG(VNET_LOG_WARN, "CMACPoolMgr::AddMACRange: The MAC range(%s) is invalid.\n", 
                cMsg.m_strMACEnd.c_str());
            VNetFormatString(strRetInfo, "The MAC(%s) is invalid.\n", cMsg.m_strMACEnd.c_str());
            return MAC_PARSE_ERROR;
        }
        cMACAddr.GetUInt32MAC(tNewItem.tEndMAC);
        
        if(!CMACRange::IsValidMACRange(tNewItem))
        {
            VNET_LOG(VNET_LOG_WARN, "CMACPoolMgr::AddMACRange: The MAC range(%s-%s) is invalid.\n", 
                cMsg.m_strMACStart.c_str(), cMsg.m_strMACEnd.c_str());
            VNetFormatString(strRetInfo, "The MAC range(%s-%s) is invalid.\n", 
                cMsg.m_strMACStart.c_str(), cMsg.m_strMACEnd.c_str());
            return MAC_RANGE_INVALID;
        }

        uint32 dwOUI = tNewItem.tStartMAC.dwOUI;
        // DB range Load;
        CDBOperateNetplaneMacRange *pOper = GetDbINetplaneMacRange();
        if(NULL == pOper)
        {
            VNET_LOG(VNET_LOG_ERROR, "CMACPoolMgr::AddMACRange: GetDbINetplaneMacRange() is NULL.\n");
            strRetInfo = "The MAC range database interface is NULL.";
            return MAC_RANGE_GET_DB_INTF_ERROR;
        }
        vector<CDbNetplaneMacRange> outVInfo;
        int32 nRet = pOper->Query(cMsg.m_strNetplaneUUID.c_str(), outVInfo);
        if(VNET_DB_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CMACPoolMgr::AddMACRange: DB Query Netplane's(UUID: %s) MAC range failed, ret:%d.\n",
                cMsg.m_strNetplaneUUID.c_str(),                
                nRet);
            VNetFormatString(strRetInfo, "Get the net plane's(UUID: %s) MAC range failed.\n",
                cMsg.m_strNetplaneUUID.c_str());
            return nRet;
        } 
        
        CMACRange cMACRange;
        if(0 != cMACRange.Init(dwOUI))
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CMACPoolMgr::AddMACRange: call cMACRange.Init(0x%x) failed.\n",
                dwOUI);
            VNetFormatString(strRetInfo, "The MAC OUI(0x%x) is invalid.\n", dwOUI);
            return MAC_OUI_INVALID;
        }
        // filter related OUI MAC range;
        int64 nValue = 0;
        CDbNetplaneMacRange cDbMACRange;
        vector<CDbNetplaneMacRange>::iterator itrDBRange;
        if(!outVInfo.empty())
        {
            for(itrDBRange = outVInfo.begin(); itrDBRange != outVInfo.end(); )
            {
                TMACRangeItem tItem;
                nValue = itrDBRange->GetBeginNum();                
                tItem.tStartMAC.dwOUI = (nValue >> 32) & 0x00FFFFFF;
                tItem.tStartMAC.dwData = nValue & 0x00FFFFFF;
                if(tItem.tStartMAC.dwOUI != dwOUI)
                {
                    itrDBRange = outVInfo.erase(itrDBRange);
                    continue ;
                }

                nValue = itrDBRange->GetEndNum();                
                tItem.tEndMAC.dwOUI = (nValue >> 32) & 0x00FFFFFF;
                tItem.tEndMAC.dwData = nValue & 0x00FFFFFF;
                if(tItem.tEndMAC.dwOUI != dwOUI)
                {
                    itrDBRange = outVInfo.erase(itrDBRange);
                    continue ;
                }
                
                cMACRange.AddValidRange(tItem);
                ++itrDBRange;
            }
        }
        
        if(outVInfo.empty() || cMACRange.GetMACRange().empty())
        {// only one record, save it to DB;
            cDbMACRange.SetUuid(cMsg.m_strNetplaneUUID.c_str());
            cDbMACRange.SetBeginStr(cMsg.m_strMACStart.c_str());
            cDbMACRange.SetEndStr(cMsg.m_strMACEnd.c_str());
            nValue = tNewItem.tStartMAC.dwOUI;
            nValue = (nValue << 32) | tNewItem.tStartMAC.dwData;
            cDbMACRange.SetBeginNum(nValue);
            nValue = tNewItem.tEndMAC.dwOUI;
            nValue = (nValue << 32) | tNewItem.tEndMAC.dwData;
            cDbMACRange.SetEndNum (nValue);    
            nRet = pOper->Add(cDbMACRange);
            if(VNET_DB_SUCCESS != nRet)
            {
                VNET_LOG(VNET_LOG_ERROR, 
                    "CMACPoolMgr::AddMACRange: DB Add Netplane's(UUID: %s) MAC range[%s-%s] failed, ret:%d.\n",
                    cMsg.m_strNetplaneUUID.c_str(),
                    cMsg.m_strMACStart.c_str(),
                    cMsg.m_strMACEnd.c_str(),
                    nRet);
                VNetFormatString(strRetInfo, "Add the net plane's(UUID: %s) MAC range[%s-%s] to the database failed.\n",
                    cMsg.m_strNetplaneUUID.c_str(),
                    cMsg.m_strMACStart.c_str(),
                    cMsg.m_strMACEnd.c_str());
                return nRet;
            } 
            return MAC_RANGE_OPER_SUCCESS;
        }  
        
        // range add;
        if(0 != cMACRange.AddRange(tNewItem))
        {            
            VNET_LOG(VNET_LOG_ERROR, 
                "CMACPoolMgr::AddMACRange: MAC range[%s-%s] calculate failed.\n",
                cMsg.m_strMACStart.c_str(), 
                cMsg.m_strMACEnd.c_str());
            VNetFormatString(strRetInfo, "Combining the calculation of net plane's(UUID: %s) MAC range[%d-%d] failed.\n",
                cMsg.m_strNetplaneUUID.c_str(),
                cMsg.m_strMACStart.c_str(), 
                cMsg.m_strMACEnd.c_str());
            return MAC_RANGE_CALC_ERROR;
        }

        // DB Compare and save;Need Transaction;
        vector<TMACRangeItem> &vecCalcRange = cMACRange.GetMACRange();
        vector<TMACRangeItem>::iterator itrCalcRange = vecCalcRange.begin();
        for(; itrCalcRange != vecCalcRange.end(); ++itrCalcRange)
        {            
            int32 bFind = 0;
            for(itrDBRange = outVInfo.begin(); itrDBRange != outVInfo.end(); ++itrDBRange)
            {
                TMACRangeItem tMACRange;
                nValue = itrDBRange->GetBeginNum();                
                tMACRange.tStartMAC.dwOUI = (nValue >> 32) & 0x00FFFFFF;
                tMACRange.tStartMAC.dwData = nValue & 0x00FFFFFF;
                nValue = itrDBRange->GetEndNum();                
                tMACRange.tEndMAC.dwOUI = (nValue >> 32) & 0x00FFFFFF;
                tMACRange.tEndMAC.dwData = nValue & 0x00FFFFFF;
                
                if(tMACRange.tStartMAC.dwOUI == itrCalcRange->tStartMAC.dwOUI &&
                   tMACRange.tStartMAC.dwData == itrCalcRange->tStartMAC.dwData &&
                   tMACRange.tEndMAC.dwOUI == itrCalcRange->tEndMAC.dwOUI &&
                   tMACRange.tEndMAC.dwData == itrCalcRange->tEndMAC.dwData)
                {
                    bFind = 1;
                    outVInfo.erase(itrDBRange);
                    break;
                }
            }
            
            if(0 == bFind)
            {                
                // Add To DB
                cDbMACRange.SetUuid(cMsg.m_strNetplaneUUID.c_str());
                cMACAddr.Set(itrCalcRange->tStartMAC);                
                cDbMACRange.SetBeginStr(cMACAddr.GetStringMAC().c_str());
                cMACAddr.Set(itrCalcRange->tEndMAC);
                cDbMACRange.SetEndStr(cMACAddr.GetStringMAC().c_str());
                nValue = itrCalcRange->tStartMAC.dwOUI;
                nValue = (nValue << 32) | itrCalcRange->tStartMAC.dwData;
                cDbMACRange.SetBeginNum(nValue);
                nValue = itrCalcRange->tEndMAC.dwOUI;
                nValue = (nValue << 32) | itrCalcRange->tEndMAC.dwData;
                cDbMACRange.SetEndNum (nValue);    
                nRet = pOper->Add(cDbMACRange);
                if(VNET_DB_SUCCESS != nRet)
                {
                    VNET_LOG(VNET_LOG_ERROR, 
                        "CMACPoolMgr::AddMACRange: DB Add Netplane's(UUID: %s) MAC range[%s-%s] failed, ret:%d.\n",
                        cMsg.m_strNetplaneUUID.c_str(),
                        cMsg.m_strMACStart.c_str(),
                        cMsg.m_strMACEnd.c_str(),
                        nRet);
                    VNetFormatString(strRetInfo, "Add the net plane's(UUID: %s) MAC range[%d-%d] to the database failed.\n",
                        cMsg.m_strNetplaneUUID.c_str(),
                        cMsg.m_strMACStart.c_str(), 
                        cMsg.m_strMACEnd.c_str());
                    return nRet;                
                }             
            }// end if(0 == ...
        }
        
        // delete invalid record from db;
        for(itrDBRange = outVInfo.begin(); itrDBRange != outVInfo.end(); ++itrDBRange)
        {
            nRet = pOper->Del(*itrDBRange);
            if(VNET_DB_SUCCESS != nRet)
            {
                VNET_LOG(VNET_LOG_ERROR, 
                    "CMACPoolMgr::AddMACRange: DB Merge Netplane's(UUID: %s) MAC range[%s-%s] failed, ret:%d.\n",
                    cMsg.m_strNetplaneUUID.c_str(),
                    cMsg.m_strMACStart.c_str(),
                    cMsg.m_strMACEnd.c_str(), 
                    nRet); 
                VNetFormatString(strRetInfo, "Merge the net plane's(UUID: %s) MAC range[%d-%d] to the database failed.\n",
                    cMsg.m_strNetplaneUUID.c_str(),
                    cMsg.m_strMACStart.c_str(), 
                    cMsg.m_strMACEnd.c_str());
                return nRet;  
            }             
        }

        return MAC_RANGE_OPER_SUCCESS;
    }
    
    int32 CMACPoolMgr::DelMACRange(const CMACRangeMsg &cMsg, string &strRetInfo)
    {
        if(cMsg.m_strNetplaneUUID.empty())
        {
            VNET_ASSERT(0);
            strRetInfo = "The MAC range config param is invalid.";
            return MAC_RANGE_CFG_PARAM_ERROR;
        }
        
        if(!cMsg.IsValidRange())
        {            
            VNET_LOG(VNET_LOG_WARN, "CMACPoolMgr::DelMACRange: The MAC range(%s-%s) is invalid.\n", 
                cMsg.m_strMACStart.c_str(), cMsg.m_strMACEnd.c_str());
            VNetFormatString(strRetInfo, "The MAC range(%s-%s) is invalid.\n",
                cMsg.m_strMACStart.c_str(), 
                cMsg.m_strMACEnd.c_str());
            return MAC_RANGE_CFG_PARAM_ERROR;
        }
        // MAC Translate;        
        CMACAddr cMACAddr;
        TMACRangeItem tNewItem;
        if(0 !=cMACAddr.Set(cMsg.m_strMACStart))
        {
            VNET_LOG(VNET_LOG_WARN, "CMACPoolMgr::DelMACRange: The MAC range(%s) is invalid.\n", 
                cMsg.m_strMACStart.c_str());
            VNetFormatString(strRetInfo, "The MAC(%s) is invalid.\n",
                cMsg.m_strMACStart.c_str());
            return MAC_PARSE_ERROR;
        }        
        cMACAddr.GetUInt32MAC(tNewItem.tStartMAC);
        if(0 != cMACAddr.Set(cMsg.m_strMACEnd))
        {
            VNET_LOG(VNET_LOG_WARN, "CMACPoolMgr::DelMACRange: The MAC range(%s) is invalid.\n", 
                cMsg.m_strMACEnd.c_str());
            VNetFormatString(strRetInfo, "The MAC(%s) is invalid.\n",
                cMsg.m_strMACEnd.c_str());
            return MAC_PARSE_ERROR;
        }
        cMACAddr.GetUInt32MAC(tNewItem.tEndMAC);
        
        if(!CMACRange::IsValidMACRange(tNewItem))
        {
            VNET_LOG(VNET_LOG_WARN, "CMACPoolMgr::DelMACRange: The MAC range(%s-%s) is invalid.\n", 
                cMsg.m_strMACStart.c_str(), cMsg.m_strMACEnd.c_str());
            VNetFormatString(strRetInfo, "The MAC range(%s-%s) is invalid.\n",
                cMsg.m_strMACStart.c_str(), 
                cMsg.m_strMACEnd.c_str());
            return MAC_RANGE_INVALID;
        }

        uint32 dwOUI = tNewItem.tStartMAC.dwOUI;
        // DB range Load;
        CDBOperateNetplaneMacRange *pOper = GetDbINetplaneMacRange();
        if(NULL == pOper)
        {
            VNET_LOG(VNET_LOG_ERROR, "CMACPoolMgr::DelMACRange: GetDbINetplaneMacRange() is NULL.\n");
            strRetInfo = "The MAC range database interface is NULL.";
            return MAC_RANGE_GET_DB_INTF_ERROR;
        }
        vector<CDbNetplaneMacRange> outVInfo;
        int32 nRet = pOper->Query(cMsg.m_strNetplaneUUID.c_str(), outVInfo);
        if(VNET_DB_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CMACPoolMgr::DelMACRange: DB Query Netplane's(UUID: %s) MAC range failed, ret:%d.\n",
                cMsg.m_strNetplaneUUID.c_str(),
                nRet);
            VNetFormatString(strRetInfo, "Get the net plane's(UUID: %s) MAC range failed.\n",
                cMsg.m_strNetplaneUUID.c_str());
            return nRet;
        } 
        
        CMACRange cMACRange;
        if(0 != cMACRange.Init(dwOUI))
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CMACPoolMgr::DelMACRange: call cMACRange.Init(0x%x) failed.\n",
                dwOUI);
            VNetFormatString(strRetInfo, "The MAC OUI(0x%x) is invalid.", dwOUI);
            return MAC_OUI_INVALID;
        }
        // filter related OUI MAC range;
        int64 nValue = 0;
        CDbNetplaneMacRange cDbMACRange;
        vector<CDbNetplaneMacRange>::iterator itrDBRange;
        if(!outVInfo.empty())
        {
            for(itrDBRange = outVInfo.begin(); itrDBRange != outVInfo.end(); )
            {
                TMACRangeItem tItem;
                nValue = itrDBRange->GetBeginNum();                
                tItem.tStartMAC.dwOUI = (nValue >> 32) & 0x00FFFFFF;
                tItem.tStartMAC.dwData = nValue & 0x00FFFFFF;
                if(tItem.tStartMAC.dwOUI != dwOUI)
                {
                    itrDBRange = outVInfo.erase(itrDBRange);
                    continue ;
                }

                nValue = itrDBRange->GetEndNum();                
                tItem.tEndMAC.dwOUI = (nValue >> 32) & 0x00FFFFFF;
                tItem.tEndMAC.dwData = nValue & 0x00FFFFFF;
                if(tItem.tEndMAC.dwOUI != dwOUI)
                {
                    itrDBRange = outVInfo.erase(itrDBRange);
                    continue ;
                }
                
                cMACRange.AddValidRange(tItem);
                ++itrDBRange;
            }
        }
        
        if(outVInfo.empty() || cMACRange.GetMACRange().empty())
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CMACPoolMgr::DelMACRange: Netplane's(UUID: %s) MAC range[%s-%s] is not exist.\n",
                cMsg.m_strNetplaneUUID.c_str(),
                cMsg.m_strMACStart.c_str(),
                cMsg.m_strMACEnd.c_str());
            VNetFormatString(strRetInfo, "The net plane's(UUID: %s) MAC range is not configured.\n",
                cMsg.m_strNetplaneUUID.c_str());
            return MAC_RANGE_NOT_EXIST;
        }  

        // Check the range is used by MAC pool or not.
        int32 nOccNum = 0;
        nValue = tNewItem.tStartMAC.dwOUI;
        nValue = (nValue << 32) | tNewItem.tStartMAC.dwData;
        int64 nBegin = nValue;
        nValue = tNewItem.tEndMAC.dwOUI;
        nValue = (nValue << 32) | tNewItem.tEndMAC.dwData;
        nRet = pOper->QueryAllocNum(cMsg.m_strNetplaneUUID.c_str(), nBegin, nValue, nOccNum);
        if(VNET_DB_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CMACPoolMgr::DelMACRange: call QueryAllocNum(%s, 0x%lx, 0x%lx) failed, ret:%d.\n",
                cMsg.m_strNetplaneUUID.c_str(),
                nBegin,
                nValue,
                nRet);
            VNetFormatString(strRetInfo, "Get the net plane's(UUID: %s) MAC range[%s-%s] used state failed.\n",
                cMsg.m_strNetplaneUUID.c_str(),
                cMsg.m_strMACStart.c_str(),
                cMsg.m_strMACEnd.c_str());
            return nRet;
        }
        if(0 != nOccNum)
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CMACPoolMgr::DelMACRange: Netplane's(UUID: %s) MAC range[%s-%s] is occupied, num:%d.\n",
                cMsg.m_strNetplaneUUID.c_str(),
                cMsg.m_strMACStart.c_str(),
                cMsg.m_strMACEnd.c_str(),
                nOccNum);
            VNetFormatString(strRetInfo, "The net plane's(UUID: %s) MAC range[%s-%s] is used.\n",
                cMsg.m_strNetplaneUUID.c_str(),
                cMsg.m_strMACStart.c_str(),
                cMsg.m_strMACEnd.c_str());
            return MAC_RANGE_OCCUPIED;
        }       
        
        // range Del;
        if(0 != cMACRange.DelRange(tNewItem))
        {            
            VNET_LOG(VNET_LOG_ERROR, 
                "CMACPoolMgr::DelMACRange: MAC range[%s-%s] calculate failed.\n",
                cMsg.m_strMACStart.c_str(), 
                cMsg.m_strMACEnd.c_str());
            VNetFormatString(strRetInfo, "Combining the calculation of MAC range[%s-%s] failed.\n",                
                cMsg.m_strMACStart.c_str(),
                cMsg.m_strMACEnd.c_str());
            return MAC_RANGE_CALC_ERROR;
        }

        // DB Compare and save;Need Transaction;
        vector<TMACRangeItem> &vecCalcRange = cMACRange.GetMACRange();
        vector<TMACRangeItem>::iterator itrCalcRange = vecCalcRange.begin();
        for(; itrCalcRange != vecCalcRange.end(); ++itrCalcRange)
        {            
            int32 bFind = 0;
            for(itrDBRange = outVInfo.begin(); itrDBRange != outVInfo.end(); ++itrDBRange)
            {
                TMACRangeItem tMACRange;
                nValue = itrDBRange->GetBeginNum();                
                tMACRange.tStartMAC.dwOUI = (nValue >> 32) & 0x00FFFFFF;
                tMACRange.tStartMAC.dwData = nValue & 0x00FFFFFF;
                nValue = itrDBRange->GetEndNum();                
                tMACRange.tEndMAC.dwOUI = (nValue >> 32) & 0x00FFFFFF;
                tMACRange.tEndMAC.dwData = nValue & 0x00FFFFFF;
                
                if(tMACRange.tStartMAC.dwOUI == itrCalcRange->tStartMAC.dwOUI &&
                   tMACRange.tStartMAC.dwData == itrCalcRange->tStartMAC.dwData &&
                   tMACRange.tEndMAC.dwOUI == itrCalcRange->tEndMAC.dwOUI &&
                   tMACRange.tEndMAC.dwData == itrCalcRange->tEndMAC.dwData)
                {
                    bFind = 1;
                    outVInfo.erase(itrDBRange);
                    break;
                }
            }
            
            if(0 == bFind)
            {                
                // Add To DB
                cDbMACRange.SetUuid(cMsg.m_strNetplaneUUID.c_str());
                cMACAddr.Set(itrCalcRange->tStartMAC);                
                cDbMACRange.SetBeginStr(cMACAddr.GetStringMAC().c_str());
                cMACAddr.Set(itrCalcRange->tEndMAC);
                cDbMACRange.SetEndStr(cMACAddr.GetStringMAC().c_str());
                nValue = itrCalcRange->tStartMAC.dwOUI;
                nValue = (nValue << 32) | itrCalcRange->tStartMAC.dwData;
                cDbMACRange.SetBeginNum(nValue);
                nValue = itrCalcRange->tEndMAC.dwOUI;
                nValue = (nValue << 32) | itrCalcRange->tEndMAC.dwData;
                cDbMACRange.SetEndNum (nValue);    
                nRet = pOper->Add(cDbMACRange);
                if(VNET_DB_SUCCESS != nRet)
                {
                    VNET_LOG(VNET_LOG_ERROR, 
                        "CMACPoolMgr::DelMACRange: DB Add Netplane's(UUID: %s) MAC range[%s-%s] failed, ret:%d.\n",
                        cMsg.m_strNetplaneUUID.c_str(),
                        cMsg.m_strMACStart.c_str(),
                        cMsg.m_strMACEnd.c_str(),
                        nRet);
                    VNetFormatString(strRetInfo, "Add the net plane's(UUID: %s) MAC range[%s-%s] to the database failed.\n",    
                        cMsg.m_strNetplaneUUID.c_str(),
                        cMsg.m_strMACStart.c_str(),
                        cMsg.m_strMACEnd.c_str());
                    return nRet;                
                }             
            }// end if(0 == ...
        }
        
        // delete invalid record from db;
        for(itrDBRange = outVInfo.begin(); itrDBRange != outVInfo.end(); ++itrDBRange)
        {
            nRet = pOper->Del(*itrDBRange);
            if(VNET_DB_SUCCESS != nRet)
            {
                VNET_LOG(VNET_LOG_ERROR, 
                    "CMACPoolMgr::DelMACRange: DB Merge Netplane's(UUID: %s) MAC range[%s-%s] failed,ret:%d.\n",
                    cMsg.m_strNetplaneUUID.c_str(),
                    cMsg.m_strMACStart.c_str(),
                    cMsg.m_strMACEnd.c_str(),
                    nRet);
                VNetFormatString(strRetInfo, "Merge the net plane's(UUID: %s) MAC range[%s-%s] to the database failed.\n",    
                    cMsg.m_strNetplaneUUID.c_str(),
                    cMsg.m_strMACStart.c_str(),
                    cMsg.m_strMACEnd.c_str());
                return nRet;  
            }             
        }

        return MAC_RANGE_OPER_SUCCESS;
    } 

    
    int32 CMACPoolMgr::GetMACRange(const string &cstrNetplaneID, CMACRange &cOUI1MACRange, CMACRange &cOUI2MACRange)
    {
        if(cstrNetplaneID.empty())
        {
            VNET_LOG(VNET_LOG_ERROR, "CMACPoolMgr::GetMACRange: cstrNetplaneID is NULL.\n");
            return -1;
        }
        
        CDBOperateNetplaneMacRange *pOper = GetDbINetplaneMacRange();
        if(NULL == pOper)
        {
            VNET_LOG(VNET_LOG_ERROR, "CMACPoolMgr::GetMACRange: GetDbINetplaneMacRange() is NULL.\n");
            return -1;
        }
        vector<CDbNetplaneMacRange> outVInfo;
        int32 nRet = pOper->Query(cstrNetplaneID.c_str(), outVInfo);
        if(VNET_DB_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CMACPoolMgr::GetMACRange: DB Query Netplane's(UUID: %s) MAC range failed, nRet:%d.\n",
                cstrNetplaneID.c_str(), nRet);
            return -1;
        } 

        if(outVInfo.empty())
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CMACPoolMgr::GetMACRange: Netplane's(UUID: %s) MAC range is not exist.\n",
                cstrNetplaneID.c_str());
            return -1;
        }

        cOUI1MACRange.Init(CMACAddr::GetZTEMACOUI1());
        cOUI2MACRange.Init(CMACAddr::GetZTEMACOUI2());
        
        int64 nValue = 0;
        CDbNetplaneMacRange cDbMACRange;
        vector<CDbNetplaneMacRange>::iterator itrDBRange;       
        for(itrDBRange = outVInfo.begin(); itrDBRange != outVInfo.end(); ++itrDBRange)
        {
            TMACRangeItem tItem;
            nValue = itrDBRange->GetBeginNum();                
            tItem.tStartMAC.dwOUI = (nValue >> 32) & 0x00FFFFFF;
            tItem.tStartMAC.dwData = nValue & 0x00FFFFFF;
            nValue = itrDBRange->GetEndNum();                
            tItem.tEndMAC.dwOUI = (nValue >> 32) & 0x00FFFFFF;
            tItem.tEndMAC.dwData = nValue & 0x00FFFFFF;
            if(tItem.tStartMAC.dwOUI == CMACAddr::GetZTEMACOUI1())
            {
                if(0 != cOUI1MACRange.AddValidRange(tItem))
                {
                    VNET_LOG(VNET_LOG_ERROR, 
                        "CMACPoolMgr::GetMACRange: Get Netplane's(UUID: %s) OUI1 MAC range[%s-%s] failed.\n",
                        cstrNetplaneID.c_str(),
                        itrDBRange->GetBeginStr().c_str(),
                        itrDBRange->GetEndStr().c_str());
                    return -1;
                }
            }
            if(tItem.tStartMAC.dwOUI == CMACAddr::GetZTEMACOUI2())
            {
                if(0 != cOUI2MACRange.AddValidRange(tItem))
                {
                    VNET_LOG(VNET_LOG_ERROR, 
                        "CMACPoolMgr::GetMACRange: Get Netplane's(UUID: %s) OUI2 MAC range[%s-%s] failed.\n",
                        cstrNetplaneID.c_str(),
                        itrDBRange->GetBeginStr().c_str(),
                        itrDBRange->GetEndStr().c_str());
                    return -1;
                }
            }
        }
        return 0;
    }  
    
    void CMACPoolMgr::DbgShowData(const string &cstrNetplaneUUID)
    {
        // show mac range;
        CMACRange cOUI1MACRange;            
        CMACRange cOUI2MACRange;
        if(0 != GetMACRange(cstrNetplaneUUID, cOUI1MACRange, cOUI2MACRange))
        {
            return ;
        }
        
        if(!cOUI1MACRange.GetMACRange().empty())
        {
            cOUI1MACRange.DbgShowMACRange();
            cout << "--------------------------------------------------------" << endl;
        }
        if(!cOUI2MACRange.GetMACRange().empty())
        {
            cOUI2MACRange.DbgShowMACRange();
            cout << "--------------------------------------------------------" << endl;
        }
        
        // show mac pool;
        CMACPool cMACPool;
        if(0 != cMACPool.Init(cstrNetplaneUUID))
        {
            cout <<"CMACPoolMgr::DbgShowData: call cMACPool.Init(" 
                 << cstrNetplaneUUID 
                 << ") failed." << endl;
            return ;
        } 
        cMACPool.DbgShowData();        
        cout << "--------------------------------------------------------" << endl;
        
        return ;
    }

    void VNetDbgTestNPMACRangeOper(const char* cstrNetplaneUUID, 
                                 const char* cstrMACStart, 
                                 const char* cstrMACEnd, 
                                 uint32 dwOper)
    {
        if(NULL == cstrNetplaneUUID || 
           NULL == cstrMACStart ||
           NULL == cstrMACEnd)
        {
            cout << "Input param error." << endl;
            return ;
        }

        if((dwOper <= EN_INVALID_MAC_RANGE_OPER) || (dwOper >= EN_MAC_RANGE_OPER_ALL))
        {
            cout << "Operation Code: Add[" << EN_ADD_MAC_RANGE
                 << "], Del[" << EN_DEL_MAC_RANGE
                 << "]." <<endl;
            return ;
        }
        
        MessageUnit tempmu(TempUnitName("TestNPMACRangeOper"));
        tempmu.Register();
        CMACRangeMsg cMsg;
        string strTmp;
        strTmp.assign(cstrNetplaneUUID);
        cMsg.m_strNetplaneUUID = strTmp;
        strTmp.clear();
        strTmp.assign(cstrMACStart);
        cMsg.m_strMACStart = strTmp;
        strTmp.clear();
        strTmp.assign(cstrMACEnd);
        cMsg.m_strMACEnd = strTmp;
        cMsg.m_dwOper = dwOper;
        MessageOption option(MU_VNM, EV_VNETLIB_MAC_RANGE_CFG, 0, 0);
        tempmu.Send(cMsg,option);
        
        return ;
    }
    DEFINE_DEBUG_FUNC(VNetDbgTestNPMACRangeOper);

    void VNetDbgTestAllocMAC(const char *cstrLogicNetworkID)
    {
        if(NULL == cstrLogicNetworkID)
        {
            cout << "LogicNetworkID is empty." << endl;
            return ;
        }
        
        CLogicNetworkMgr *pLogiNetMgr = CLogicNetworkMgr::GetInstance();
        if(NULL == pLogiNetMgr)
        {
            cout << "No LogicNetwork instance." << endl;
            return ;
        }

        CMACPoolMgr *pMACPoolMgr = CMACPoolMgr::GetInstance();
        if(NULL == pMACPoolMgr)
        {
            cout << "No MAC pool instance." << endl;
            return ;
        }
        
        CVNetVnicMem cVNic;
        string strTmp;
        strTmp.assign(cstrLogicNetworkID);
        cVNic.SetLogicNetworkUuid(strTmp);

        CLogicNetwork cLogicNetwork(""); 
        string strLogiNetworkID = strTmp;
        if(0 != pLogiNetMgr->GetLogicNetwork(strLogiNetworkID, cLogicNetwork))
        {
            cout << "Can't find " << strLogiNetworkID << " logic network." << endl;
            return ;
        }
        string strPGID = cLogicNetwork.GetPortGroupUUID();
        
        vector<CVNetVnicMem> vecVNetVnic;
        for(int32 i = 1; i <= 10; ++i)
        {            
            cVNic.SetNicIndex(i);
            cVNic.SetVmPgUuid(strPGID);
            vecVNetVnic.push_back(cVNic);
        }
        CVNetVmMem cVNetVmMem;
        cVNetVmMem.SetVecVnicMem(vecVNetVnic);
        pMACPoolMgr->AllocMACForVnic(cVNetVmMem);
        return ;
    }
    DEFINE_DEBUG_FUNC(VNetDbgTestAllocMAC);

    void VNetDbgTestFreeMAC(const char *cstrLogicNetworkID, const char *cstrMAC)
    {
        if(NULL == cstrLogicNetworkID)
        {
            cout << "LogicNetworkID is empty." << endl;
            return ;
        }

        if(NULL == cstrMAC)
        {
            cout << "MAC is empty." << endl;
            return ;
        }
        
        CLogicNetworkMgr *pLogiNetMgr = CLogicNetworkMgr::GetInstance();
        if(NULL == pLogiNetMgr)
        {
            cout << "No LogicNetwork instance." << endl;
            return ;
        }

        CMACPoolMgr *pMACPoolMgr = CMACPoolMgr::GetInstance();
        if(NULL == pMACPoolMgr)
        {
            cout << "No MAC pool instance." << endl;
            return ;
        }
        
        CVNetVnicMem cVNic;
        string strTmp;
        strTmp.assign(cstrLogicNetworkID);
        cVNic.SetLogicNetworkUuid(strTmp);

        CLogicNetwork cLogicNetwork(""); 
        string strLogiNetworkID = strTmp;
        if(0 != pLogiNetMgr->GetLogicNetwork(strLogiNetworkID, cLogicNetwork))
        {
            cout << "Can't find " << strLogiNetworkID << " logic network." << endl;
            return ;
        }
        string strPGID = cLogicNetwork.GetPortGroupUUID(); 
        cVNic.SetVmPgUuid(strPGID);
        cVNic.SetNicIndex(1);
        strTmp.clear();
        strTmp.assign(cstrMAC);
        cVNic.SetMac(strTmp);        
        vector<CVNetVnicMem> vecVNetVnic;
        vecVNetVnic.push_back(cVNic);   
        CVNetVmMem cVNetVmMem;
        cVNetVmMem.SetVecVnicMem(vecVNetVnic);
        pMACPoolMgr->FreeMACForVnic(cVNetVmMem);
        return ;
    }
    DEFINE_DEBUG_FUNC(VNetDbgTestFreeMAC);
    
    void VNetDbgShowNetplaneMACPool(const char *cstrNetplaneUUID)
    {
        if(NULL == cstrNetplaneUUID)
        {
            cout << "Input param error." << endl;
            return ;
        }
        
        CMACPoolMgr *pMgr = CMACPoolMgr::GetInstance();
        if(pMgr)
        {
            string strTmp;
            strTmp.assign(cstrNetplaneUUID);
            pMgr->DbgShowData(strTmp);
        }
        
        return ;
    }
    DEFINE_DEBUG_FUNC(VNetDbgShowNetplaneMACPool);

    void VNetDbgSetMACPoolPrint(BOOL bOpen)
    {
        CMACPoolMgr *pMgr = CMACPoolMgr::GetInstance();
        if(pMgr)
        {
            pMgr->SetDbgInfFlg(bOpen);
        }
        
        return ;
    }
    DEFINE_DEBUG_FUNC(VNetDbgSetMACPoolPrint);
}// end namespace zte_tecs

