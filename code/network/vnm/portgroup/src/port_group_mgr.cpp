/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：port_group_mgr.cpp
* 文件标识： 
* 内容摘要：端口组管理类CPorgGroupMgr实现
* 当前版本：1.0
* 作    者： 
* 完成日期： 
*******************************************************************************/
#include "vnet_comm.h"
#include "vnm_pub.h"
#include "vnet_mid.h"
#include "vnet_error.h"
#include "vnetlib_event.h"
#include "vnetlib_msg.h"
#include "vnet_db_pg_trunkvlanrange.h"
#include "vnet_vnic.h"
#include "vlan_pool_mgr.h"
#include "vnet_db_vlan_map.h"
#include "vnet_db_segment_map.h"
#include "vnet_db_quantum_cfg.h"
#include "vnet_db_quantum_port.h"
#include "vnet_db_quantum_network.h"

#include <restrpc-c/girerr.hpp>
#include <restrpc-c/base.hpp>
#include <restrpc-c/client_simple.hpp>
#include <restrpc-c/json.h>

using namespace restrpc_c;


namespace zte_tecs
{

CPortGroupMgr *CPortGroupMgr::m_pInstance = NULL;

CPortGroupMgr::CPortGroupMgr()
{
    m_pMU       = NULL;
    m_pDbMgr    = NULL;
    m_pDbPG     = NULL;    
    m_pVlanRange = NULL;
}

CPortGroupMgr::~CPortGroupMgr()
{
    m_pMU       = NULL;
    m_pDbMgr    = NULL;
    m_pDbPG     = NULL;
    m_pInstance = NULL;  
    m_pVlanRange = NULL;
}

string int2string(const int& intvar)
{
    stringstream strStream;
    strStream << intvar;
    return strStream.str();
}


/*******************************************************************************
* 函数名称： Init
* 功能描述： 初始化函数
* 访问的表： 无
* 修改的表： 无
* 输入参数： mu --消息单元
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* ------------------------------------------------------------------------------
* 2013/3          V1.0       lvech         创建
*******************************************************************************/
int32 CPortGroupMgr::Init(MessageUnit *mu)
{
    if (mu == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::Init: pMu is NULL.\n");
        return -1;
    }
    
    m_pMU = mu;

    m_pDbMgr = CVNetDbMgr::GetInstance();
    if(NULL == m_pDbMgr)
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::Init: CVNetDbMgr::GetInstance() is NULL.\n");
        return -1;
    }    
    
    m_pDbPG = m_pDbMgr->GetIPortGroup();
    if(NULL == m_pDbPG)
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::Init: GetIPortGroup() is NULL.\n");
        return -1;
    }

    m_pVlanRange = m_pDbMgr->GetIPgTrunkVlanRange();
    if(NULL == m_pVlanRange)
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::Init: GetIPgTrunkVlanRange() is NULL.\n");
        return -1;
    }   

    //创建默认UPLINK PG
    string strRetInfo;
    CPortGroupMsg cUplinkPG;
    cUplinkPG.m_strName.assign("DEFAULT_UPLINK_PG");
    cUplinkPG.m_nSwitchPortMode  = EN_DB_SWITCHPORTMODE_TYPE_TRUNK;
    cUplinkPG.m_nPGType          = EN_DB_PORTGROUP_TYPE_UPLINK;
    cUplinkPG.m_nVersion         = 1;
    cUplinkPG.m_nMTU             = VNET_COMMON_MTU;
    cUplinkPG.m_nTrunkNativeVlan = 1;
    
    int32 nRet = AddPortGroup(cUplinkPG, strRetInfo);
    if(!((PORTGROUP_OPER_SUCCESS == nRet)||(ERROR_PORTGROUP_IS_EXIST == nRet)))
    {
        VNET_LOG(VNET_LOG_INFO, "CPortGroupMgr::Init: Add Default Uplink PG failed, ret = %d\n", nRet);
        //不影响上电
    }
    
    //创建默认kernel PG
    CPortGroupMsg cKernelPG;
    cKernelPG.m_strName.assign("DEFAULT_KERNEL_PG");
    cKernelPG.m_nSwitchPortMode  = EN_DB_SWITCHPORTMODE_TYPE_ACCESS;
    cKernelPG.m_nPGType          = EN_DB_PORTGROUP_TYPE_KERNEL_MGR;
    cKernelPG.m_nVersion         = 1;
    cKernelPG.m_nMTU             = VNET_COMMON_MTU;
    cKernelPG.m_nAccessVlanNum   = 1;
    
    nRet = AddPortGroup(cKernelPG, strRetInfo);
    if(!((PORTGROUP_OPER_SUCCESS == nRet)||(ERROR_PORTGROUP_IS_EXIST == nRet)))
    {
        VNET_LOG(VNET_LOG_INFO, "CPortGroupMgr::Init: Add Default Kernel PG failed, ret = %d\n", nRet);
        //不影响上电
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
void CPortGroupMgr::MessageEntry(const MessageFrame &message)
{
    switch(message.option.id())
    {
        case EV_VNETLIB_PORT_GROUP_CFG:
        {
            ProcPortGroupMsg(message);
            break;
        }
        case EV_VNETLIB_PG_TRUNK_VLANRANGE_CFG:
        {
            ProcPGTrunkVlanRangeMsg(message);
        }
        case EV_VNETLIB_PG_QUERY:
        {
            ProcPortGroupQuery(message);
        }
        default:
            break;
    }
}

/*******************************************************************************
* 函数名称： ProcPortGroupCfgMsg
* 功能描述： 端口组配置消息处理函数
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
int32 CPortGroupMgr::ProcPortGroupMsg(const MessageFrame &message)
{
    int32 nRet = 0;
    string strRetInfo;
    CPortGroupMsg cMsg;
    cMsg.deserialize(message.data);

    MessageOption option(message.option.sender(), EV_VNETLIB_PORT_GROUP_CFG_ACK, 0, 0);

    if(!cMsg.IsValidOper())
    {
        VNET_LOG(VNET_LOG_WARN, "CPortGroupMgr::ProcPortGroupCfgMsg: Msg operation code (%d) is invalid.\n", 
            cMsg.m_nOper);
        nRet = ERROR_PORTGROUP_OPERCODE_INVALID;
        VNetFormatString(strRetInfo, "PortGroup operator(%d) is invalid.", cMsg.m_nOper);
        goto PG_OPER_ACK;
    }
   
    if(m_bOpenDbg)
    {
        cMsg.Print();
    }
    
    switch(cMsg.m_nOper)
    {
        case EN_ADD_PORT_GROUP:
        {
            nRet = AddPortGroup(cMsg, strRetInfo);
            break;
        }
        case EN_DEL_PORT_GROUP:
        {
            nRet = DelPortGroup(cMsg, strRetInfo);
            break;
        }
        case EN_SET_PORT_GROUP:
        {
            nRet = SetPortGroup(cMsg, strRetInfo);
            break;
        }
        case EN_SET_PG_NATIVE_VLAN:
        {
            nRet = SetPGNativeVlan(cMsg, strRetInfo);
            break;
        }
        default:
        {
            VNET_ASSERT(0);
        }
    }

PG_OPER_ACK:
    cMsg.SetResult(nRet);
    cMsg.SetResultinfo(strRetInfo);
    return m_pMU->Send(cMsg, option);
}

/*******************************************************************************
* 函数名称： QueryPGByName
* 功能描述： 查询PG
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
int32 CPortGroupMgr::QueryPGByName(const string &strPGName, string &strPGUuid)
{
    vector<CDbPortGroupSummary> outVInfo;
    int32 nRet = m_pDbPG->QuerySummary(outVInfo);
    if(VNET_DB_SUCCESS == nRet)
    {
        vector<CDbPortGroupSummary>::iterator iter;
        for(iter = outVInfo.begin(); iter != outVInfo.end(); ++iter)
        {
            if(0 == (iter->GetName()).compare(strPGName))  
            {
                strPGUuid = iter->GetUuid();
                return 0;
            }
        }
    }
    return -1;
}

/*******************************************************************************
* 函数名称： QueryPGByUUID
* 功能描述： 查询PG
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
int32 CPortGroupMgr::QueryPGByUUID(const string &strPGUuid, string &strPGName)
{
    vector<CDbPortGroupSummary> outVInfo;
    int32 nRet = m_pDbPG->QuerySummary(outVInfo);
    if(VNET_DB_SUCCESS == nRet)
    {
        vector<CDbPortGroupSummary>::iterator iter;
        for(iter = outVInfo.begin(); iter != outVInfo.end(); ++iter)
        {
            if(iter->GetUuid() == strPGUuid)  
            {
                strPGName = iter->GetName();
                return PORTGROUP_OPER_SUCCESS;
            }
        }
    }
    VNET_LOG(VNET_LOG_INFO, "CPortGroupMgr::QueryPGByUUID: PortGroup(%s) is not exist.\n", 
              strPGUuid.c_str()); 
    return ERROR_PORTGROUP_NOT_EXIST;
}
/*******************************************************************************
* 函数名称： ProcPortGroupQuery
* 功能描述： 端口组配置查询消息处理函数
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* ------------------------------------------------------------------------------
* 2014/2          V1.0       chengmj     创 建
*******************************************************************************/
int32 CPortGroupMgr::ProcPortGroupQuery(const MessageFrame &message)
{
    int32 index = 0;
    int32 count = 0;
    int32 nRet = 0;
    int32 sum = 0;
    string strRetInfo;
    CVnetPortGroupLists ack_msg;
    //vector<value> array_pg;
    vector<CDbPortGroupSummary> outVInfo;

    CVNetPortGroupQuery cMsg;
    cMsg.deserialize(message.data);

    MessageOption option(message.option.sender(), EV_VNETLIB_PG_QUERY_ACK, 0, 0);

    if(cMsg.start_index < 1)
    {
        VNET_LOG(VNET_LOG_WARN, "CPortGroupMgr::ProcPortGroupQuery: start_index (%d) is invalid.\n", 
            cMsg.start_index);
        nRet = ERROR_PORTGROUP_INPARAM_INVALID;
        VNetFormatString(strRetInfo, "start_index begin from 1.");
        goto PG_OPER_ACK;
    }
   
    if(m_bOpenDbg)
    {
        cMsg.Print();
    }
    
    nRet = m_pDbPG->QuerySummary(outVInfo);
    if(VNET_DB_SUCCESS == nRet)
    {
        sum = outVInfo.size();
        cout<<"pg number is "<<sum<<endl;
        vector<CDbPortGroupSummary>::iterator iter;
        for(iter = outVInfo.begin(); iter != outVInfo.end(); ++iter)
        {
            index++;
            if(index < cMsg.start_index)
            {
                continue;
            }
            CVnetPortGroup ack_pg;
            CDbPortGroup cDbPG;
            cDbPG.SetUuid(iter->GetUuid().c_str());
            nRet = m_pDbPG->Query(cDbPG); 
            if(VNET_DB_SUCCESS == nRet)
            {
                ack_pg.access_isolate_num = cDbPG.GetAccessIsolateNo();
                ack_pg.access_vlan_num = cDbPG.GetAccessVlanNum();
                ack_pg.isolate_type = cDbPG.GetIsolateType();
                ack_pg.is_cfg_netplane = cDbPG.GetIsCfgNetplane();
                ack_pg.mtu = cDbPG.GetMtu();
                ack_pg.netplane_uuid = cDbPG.GetNetplaneUuid();
                ack_pg.pg_name = cDbPG.GetName();
                ack_pg.pg_type = cDbPG.GetPgType();
                ack_pg.pg_uuid = cDbPG.GetUuid();
                ack_pg.segment_id = cDbPG.GetSegmentIdNum();
                ack_pg.switch_port_mode = cDbPG.GetSwithportMode();
                ack_pg.trunk_native_vlan = cDbPG.GetTrunkNatvieVlanNum();
                ack_pg.m_nVxlanIsolateNo = cDbPG.GetVxlanIsolateNo();
                ack_pg.m_nIsSdn = cDbPG.GetIsSdn();
                
                 ack_msg.pg_info.push_back(ack_pg);
            }
            else
            {
                goto PG_OPER_ACK;
            }
            count++;
            if(count >= cMsg.count)
            {
                goto PG_OPER_ACK;
                //break;
            }
        }
    }
    
PG_OPER_ACK:
    if(index < sum)
    {
        ack_msg.set_next_index(index+1);
    }
    else
    {
        ack_msg.set_next_index(-1);
    }
    ack_msg.set_max_count(count);
    ack_msg.set_ret_code(nRet);
    ack_msg.SetResult(nRet);
    ack_msg.SetResultinfo(strRetInfo);
    return m_pMU->Send(ack_msg, option);
}

int32 CPortGroupMgr::CheckPGParam(CPortGroupMsg &cMsg, string &strRetInfo)
{
    //入参合法性检查
    if(0 != cMsg.CheckPortGroupType())
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::CheckPGParam: PG Type(%d) is invalid.\n", cMsg.m_nPGType);
        VNetFormatString(strRetInfo, "PortGroup type(%d) is invalid.", cMsg.m_nPGType);
        return -1;
    }   
    if(0 != cMsg.CheckPGMTU())
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::CheckPGParam: PG MTU(%d) is invalid.\n", cMsg.m_nMTU);
        VNetFormatString(strRetInfo, "PortGroup MTU(%d) is invalid.", cMsg.m_nMTU);
        return -1;        
    }
    if(0 != cMsg.CheckSwitchPortMode())
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::CheckPGParam: PG Switchport Type(%d) is invalid.\n", cMsg.m_nSwitchPortMode);
        VNetFormatString(strRetInfo, "PortGroup switchmode(%d) is invalid.", cMsg.m_nSwitchPortMode);
        return -1;        
    } 
    if(0 != cMsg.CheckIsolateType())
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::CheckPGParam: PG IsolateType Type(%d) is invalid.\n", cMsg.m_nIsolateType);
        VNetFormatString(strRetInfo, "PortGroup IsolateType(%d) is invalid.", cMsg.m_nIsolateType);
        return -1;        
    } 
    
    return 0;
}

/*******************************************************************************
* 函数名称： AddPortGroup
* 功能描述： 添加端口组
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
int32 CPortGroupMgr::AddPortGroup(CPortGroupMsg &cMsg, string &strRetInfo)
{
    //入参合法性检查
    int32 nRet = CheckPGParam(cMsg, strRetInfo);
    if(0 != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::AddPortGroup: call CheckPGParam failed.\n");
        return ERROR_PORTGROUP_INPARAM_INVALID;
    }
    
    //检查PG是否存在
    string strPGUuid;
    nRet = QueryPGByName(cMsg.m_strName, strPGUuid);
    if(!nRet)
    {
        VNET_LOG(VNET_LOG_INFO, "CPortGroupMgr::AddPortGroup: PG(%s) is already exist.\n", 
              cMsg.m_strName.c_str()); 
        VNetFormatString(strRetInfo, "PortGroup(%s) is already exist.", cMsg.m_strName.c_str());
        return ERROR_PORTGROUP_IS_EXIST;
    }

    CDbPortGroup cDbPG;
    cDbPG.SetName(cMsg.m_strName.c_str());
    cDbPG.SetPgType(cMsg.m_nPGType);
    cDbPG.SetAcl(cMsg.m_strACL.c_str());
    cDbPG.SetQos(cMsg.m_strQOS.c_str());
    cDbPG.SetAllowPriorities(cMsg.m_strPrior.c_str());
    cDbPG.SetRcvBandwidthLmt(cMsg.m_strRcvBWLimit.c_str());
    cDbPG.SetRcvBandwidthRsv(cMsg.m_strRcvBWRsv.c_str());
    cDbPG.SetDftVlanId(cMsg.m_nDefaultVlan);
    cDbPG.SetPromiscuous(cMsg.m_nPromisc);
    cDbPG.SetMacVlanFltEnable(cMsg.m_nMacVlanFilter);
    cDbPG.SetAllowTrmtMacs(cMsg.m_strTxMacs.c_str());
    cDbPG.SetAllowTrmtMacVlans(cMsg.m_strTxMacvlans.c_str());
    cDbPG.SetPlyBlkOver(cMsg.m_strPolicyBlkOver.c_str());
    cDbPG.SetPlyVlanOver(cMsg.m_strPolicyVlanOver.c_str());
    cDbPG.SetVersion(cMsg.m_nVersion);
    cDbPG.SetMgrId(cMsg.m_strMgrId.c_str());
    cDbPG.SetTypeId(cMsg.m_strVSIType.c_str());
    cDbPG.SetAllowMacChg(cMsg.m_nMacChange);
    cDbPG.SetSwithportMode(cMsg.m_nSwitchPortMode);
    cDbPG.SetIsCfgNetplane(cMsg.m_nIsCfgNetPlane);
    cDbPG.SetNetplaneUuid(cMsg.m_strNetPlaneUUID.c_str());
    cDbPG.SetMtu(cMsg.m_nMTU);
    cDbPG.SetIsSdn(cMsg.m_nIsSdn);
    cDbPG.SetDesc(cMsg.m_strdescription.c_str());
    cDbPG.SetTrunkNatvieVlanNum(cMsg.m_nTrunkNativeVlan); 
    cDbPG.SetAccessVlanNum(cMsg.m_nAccessVlanNum); 
    cDbPG.SetIsolateType(cMsg.m_nIsolateType); 
    cDbPG.SetSegmentIdNum(cMsg.m_nSegmentId);      

    if (EN_ISOLATE_TYPE_VXLAN == cMsg.m_nIsolateType)
    {
        cDbPG.SetVxlanIsolateNo(cMsg.m_nAccessIsolateNum);
        
        if(cMsg.m_nIsCfgNetPlane)
        {
            CSegmentPoolMgr *pSegmentPoolMgr = CSegmentPoolMgr::GetInstance();
            if(NULL == pSegmentPoolMgr)
            {
                VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::AddPortGroup: CSegmentPoolMgr::GetInstance() is NULL.\n");
                strRetInfo = "The handler of pSegmentPoolMgr is NULL.";
                return ERROR_PORTGROUP_GET_INSTANCE_FAIL;
            } 
        
            if(pSegmentPoolMgr->IsExistSegmentRange(cMsg.m_strNetPlaneUUID))
            {
                //网络平面配了SEGMENT范围才能配PG SEGMENT
                if(cMsg.m_nPGType == EN_PORTGROUP_TYPE_VM_SHARE)
                {
                    if(pSegmentPoolMgr->IsInSegmentMapRanges(cMsg.m_nSegmentId, cMsg.m_strNetPlaneUUID))
                    {
                        VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::AddPortGroup: m_nSegmentId<%d> is in NetPlane<%s>'segment range.\n", 
                                cMsg.m_nAccessVlanNum, cMsg.m_strNetPlaneUUID.c_str());  
                        VNetFormatString(strRetInfo, "PortGroup segment (%d) is in NetPlane's segmentrange.", cMsg.m_nSegmentId);
                        return ERROR_PORTGROUP_SEGMENT_INVALID;
                    }
                }
            }
            else
            {
                if(cMsg.m_nPGType == EN_PORTGROUP_TYPE_VM_PRIVATE)
                {
                    VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::AddPortGroup: Don't config portgroup as NetPlane segmentrange is empty.\n");   
                    VNetFormatString(strRetInfo, "Fail to config private segment(%d) as NetPlane's segmentrange is empty.", cMsg.m_nSegmentId);
                    return ERROR_PORTGROUP_PRIVATE_SEGMENT_INVALID;                        
                }
            }
        }        
    }
    else
    {
        cDbPG.SetAccessIsolateNo(cMsg.m_nAccessIsolateNum);
        switch(cMsg.m_nSwitchPortMode)
        {
            case EN_SWITCHPORT_MODE_ACCESS:
            {
                if(cMsg.m_nIsCfgNetPlane)
                {
                    CVlanPoolMgr *pVlanMgr = CVlanPoolMgr::GetInstance();
                    if(NULL == pVlanMgr)
                    {
                        VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::AddPortGroup: CVlanPoolMgr::GetInstance() is NULL.\n");
                        strRetInfo = "The handler of CVlanPoolMgr is NULL.";
                        return ERROR_PORTGROUP_GET_INSTANCE_FAIL;
                    } 
                
                    if(pVlanMgr->IsExistVlanRange(cMsg.m_strNetPlaneUUID))
                    {
                        //网络平面配了VLAN范围才能配PG VLAN
                        if(cMsg.m_nPGType == EN_PORTGROUP_TYPE_VM_SHARE)
                        {
                            if(pVlanMgr->IsInVlanMapRanges(cMsg.m_nAccessVlanNum, cMsg.m_strNetPlaneUUID))
                            {
                                VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::AddPortGroup: AccessVlan<%d> is in NetPlane<%s>'vlan range.\n", 
                                        cMsg.m_nAccessVlanNum, cMsg.m_strNetPlaneUUID.c_str());  
                                VNetFormatString(strRetInfo, "Access VLAN(%d) of PortGroup is in NetPlane's vlanrange.", cMsg.m_nAccessVlanNum);
                                return ERROR_PORTGROUP_ACCESS_VLAN_INVALID;
                            }
                        }
                    }
                    else
                    {
                        if(cMsg.m_nPGType == EN_PORTGROUP_TYPE_VM_PRIVATE)
                        {
                            VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::AddPortGroup: Don't config portgroup as NetPlane vlanrange is empty.\n");   
                            VNetFormatString(strRetInfo, "Fail to config private vlan(%d) as NetPlane's vlanrange is empty.", cMsg.m_nAccessIsolateNum);
                            return ERROR_PORTGROUP_PRIVATE_VLAN_INVALID;                        
                        }
                    }
                }
                break;
            }
            case EN_SWITCHPORT_MODE_TRUNK:
            default:
                break;
        }
    }
    //Add PG
    nRet = m_pDbPG->Add(cDbPG);
    if(VNET_DB_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::AddPortGroup: Add portgroup to DB failed, ret = %d\n", nRet);
        VNetFormatString(strRetInfo, "Fail to add PortGroup(%s) in database.", cMsg.m_strName.c_str());
        return ERROR_PORTGROUP_OPER_DB_FAIL;
    }
    cMsg.m_strUUID = cDbPG.GetUuid();
    return PORTGROUP_OPER_SUCCESS;        
}

/*******************************************************************************
* 函数名称： DelPortGroup
* 功能描述： 删除端口组
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
int32 CPortGroupMgr::DelPortGroup(const CPortGroupMsg &cMsg, string &strRetInfo)
{
    string strPGName;
    int32 nRet = QueryPGByUUID(cMsg.m_strUUID, strPGName);
    if(PORTGROUP_OPER_SUCCESS != nRet)
    {
        VNetFormatString(strRetInfo, "PortGroup(UUID:%s) is not exist.", cMsg.m_strUUID.c_str());
        return nRet;
    }
    
    //默认PG不能被删除
    if((0 == strPGName.compare("DEFAULT_UPLINK_PG")) || (0 == strPGName.compare("DEFAULT_KERNEL_PG")))
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::DelPortGroup: <%s> is Default PG, Can`t Delete.\n", strPGName.c_str());
        VNetFormatString(strRetInfo, "Default PortGroup(%s) cannot delete.", strPGName.c_str());
        return ERROR_PORTGROUP_OPER_DEFAULT_PG_FAIL;
    }
    
    //PG是否被用
    nRet = m_pDbPG->IsUsing(cMsg.m_strUUID.c_str());
    if(VNET_DB_SUCCESS != nRet)
    {
        if(VNET_DB_IS_RESOURCE_USING(nRet))
        {
            VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::DelPortGroup: PG is using, ret = %d\n", nRet);
            VNetFormatString(strRetInfo, "Do not delete %s as PortGroup is inusing.", strPGName.c_str());
            return ERROR_PORTGROUP_IS_INUSING;
        }
        else
        {
            VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::DelPortGroup: call IsUsing failed, ret = %d\n", nRet);
            strRetInfo = "Fail to database check for PortGroup is inusing.";
            return ERROR_PORTGROUP_OPER_DB_FAIL;
        }
    }    
    
    //级联删除trunk vlan range表
    nRet = m_pDbPG->Del(cMsg.m_strUUID.c_str());
    if(VNET_DB_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::DelPortGroup: Del portgroup in DB failed, ret = %d\n", nRet);
        VNetFormatString(strRetInfo, "Fail to delete PortGroup(%s) in database.", strPGName.c_str());
        return ERROR_PORTGROUP_OPER_DB_FAIL;
    }
    
    return PORTGROUP_OPER_SUCCESS;          
}

/*******************************************************************************
* 函数名称： SetPortGroup
* 功能描述： 修改端口组属性
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
int32 CPortGroupMgr::SetPortGroup(CPortGroupMsg &cMsg, string &strRetInfo)
{
    string strPGName;
    int32 nRet = QueryPGByUUID(cMsg.m_strUUID, strPGName);
    if(PORTGROUP_OPER_SUCCESS != nRet)
    {
        VNetFormatString(strRetInfo, "PortGroup(UUID:%s) is not exist.", cMsg.m_strUUID.c_str());
        return nRet;
    }  
    
    nRet = CheckPGParam(cMsg, strRetInfo);
    if(0 != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::SetPortGroup: call CheckPGParam failed.\n");
        return ERROR_PORTGROUP_INPARAM_INVALID;
    }
    
    //查询数据库信息
    CDbPortGroup cDbPG;
    cDbPG.SetUuid(cMsg.m_strUUID.c_str());
    nRet = m_pDbPG->Query(cDbPG);
    if(VNET_DB_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::SetPortGroup: Query PortGroup DB failed. ret = %d\n", nRet);
        VNetFormatString(strRetInfo, "Fail to query PortGroup(%s) in database.", strPGName.c_str());
        return ERROR_PORTGROUP_OPER_DB_FAIL;
    }
    
    //修改属性
    if(VNET_TRUE == CmpPGData(cDbPG, cMsg))
    {
        //默认PG不能被修改
        if((0 == strPGName.compare("DEFAULT_UPLINK_PG")) || (0 == strPGName.compare("DEFAULT_KERNEL_PG")))
        {
            VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::SetPortGroup: <%s> is Default PG, Can`t Modify.\n", strPGName.c_str());
            VNetFormatString(strRetInfo, "Default PortGroup(%s) cannot be modified.", strPGName.c_str());
            return ERROR_PORTGROUP_OPER_DEFAULT_PG_FAIL;
        } 
    
        //PG是否被用
        nRet = m_pDbPG->IsUsing(cMsg.m_strUUID.c_str());
        if(VNET_DB_SUCCESS != nRet)
        {
            if(VNET_DB_IS_RESOURCE_USING(nRet))
            {
                VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::SetPortGroup: PG<%s> is using, ret = %d\n", strPGName.c_str(), nRet);
                VNetFormatString(strRetInfo, "Do not modify PortGroup(%s) as inusing.", strPGName.c_str());
                return ERROR_PORTGROUP_IS_INUSING;
            }
            else
            {
                VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::SetPortGroup: call IsUsing failed, ret = %d\n", nRet);
                strRetInfo = "Fail to database check for PortGroup is using.";
                return ERROR_PORTGROUP_OPER_DB_FAIL;
            }
        }    

        if (EN_ISOLATE_TYPE_VXLAN == cMsg.m_nIsolateType)
        {
            //先校验SEGMENT
            CSegmentPoolMgr *pSegmentPoolMgr = CSegmentPoolMgr::GetInstance();
            if(NULL == pSegmentPoolMgr)
            {
                VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::SetPortGroup: CSegmentPoolMgr::GetInstance() is NULL.\n");
                strRetInfo = "The handler of CSegmentPoolMgr is NULL.";
                return ERROR_PORTGROUP_GET_INSTANCE_FAIL;
            } 
            
            if(cMsg.m_nIsCfgNetPlane)
            {
                if(pSegmentPoolMgr->IsExistSegmentRange(cMsg.m_strNetPlaneUUID))
                {
                    if(cMsg.m_nPGType == EN_PORTGROUP_TYPE_VM_SHARE)
                    {
                        if(pSegmentPoolMgr->IsInSegmentMapRanges(cMsg.m_nSegmentId, cMsg.m_strNetPlaneUUID))
                        {
                            VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::SetPortGroup: SegmentId <%d> is in NetPlane<%s>'segmentrange.\n", 
                                 cMsg.m_nSegmentId, cMsg.m_strNetPlaneUUID.c_str());  
                            VNetFormatString(strRetInfo, "PortGroup m_nSegmentId (%d) is in NetPlane's segmentrange.", cMsg.m_nSegmentId);
                            return ERROR_PORTGROUP_SEGMENT_INVALID;
                        }
                    }
                }
                else
                {
                    if(cMsg.m_nPGType == EN_PORTGROUP_TYPE_VM_PRIVATE)
                    {
                        VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::SetPortGroup: No NetPlane segmentrange, Not Config IsolateSegment.\n");  
                        VNetFormatString(strRetInfo, "Fail to modify private segment(%d) as NetPlane's segmentrange is empty.", cMsg.m_nSegmentId);
                        return ERROR_PORTGROUP_PRIVATE_SEGMENT_INVALID;                        
                    }
                }                
            }             
        }
        else
        {    
            //先校验Access VLAN
            CVlanPoolMgr *pVlanMgr = CVlanPoolMgr::GetInstance();
            if(NULL == pVlanMgr)
            {
                VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::SetPortGroup: CVlanPoolMgr::GetInstance() is NULL.\n");
                strRetInfo = "The handler of CVlanPoolMgr is NULL.";
                return ERROR_PORTGROUP_GET_INSTANCE_FAIL;
            } 

            if(cMsg.m_nSwitchPortMode == EN_SWITCHPORT_MODE_ACCESS)
            {
                if(cMsg.m_nIsCfgNetPlane)
                {
                    if(pVlanMgr->IsExistVlanRange(cMsg.m_strNetPlaneUUID))
                    {
                        if(cMsg.m_nPGType == EN_PORTGROUP_TYPE_VM_SHARE)
                        {
                            if(pVlanMgr->IsInVlanMapRanges(cMsg.m_nAccessVlanNum, cMsg.m_strNetPlaneUUID))
                            {
                                VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::SetPortGroup: AccessVlan<%d> is in NetPlane<%s>'vlan range.\n", 
                                     cMsg.m_nAccessVlanNum, cMsg.m_strNetPlaneUUID.c_str());  
                                VNetFormatString(strRetInfo, "PortGroup access VLAN(%d) is in NetPlane's vlanrange.", cMsg.m_nAccessVlanNum);
                                return ERROR_PORTGROUP_ACCESS_VLAN_INVALID;
                            }
                        }
                    }
                    else
                    {
                        if(cMsg.m_nPGType == EN_PORTGROUP_TYPE_VM_PRIVATE)
                        {
                            VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::SetPortGroup: No NetPlane vlanrange, Not Config IsolateVlan.\n");  
                            VNetFormatString(strRetInfo, "Fail to modify private vlan(%d) as NetPlane's vlanrange is empty.", cMsg.m_nAccessIsolateNum);
                            return ERROR_PORTGROUP_PRIVATE_VLAN_INVALID;                        
                        }
                    }
                }
            }
            else
            {
                //和网络平面vlan range是否冲突
                if(cMsg.m_nIsCfgNetPlane)
                {
                    if(cMsg.m_nPGType != EN_PORTGROUP_TYPE_UPLINK)
                    {
                        vector<CTrunkVlanRange> vecTrunkVlanRange;
                        if(0 != GetPGTrunkVlanRange(cMsg.m_strUUID, vecTrunkVlanRange))
                        {
                            VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::SetPortGroup: GetPGTrunkVlanRange<PG: %s> failed.\n", cMsg.m_strUUID.c_str());
                            strRetInfo = "Do not modify as getting PortGroup's vlanrange fail.";
                            return ERROR_PORTGROUP_GET_VLANRANGE_FAIL;
                        }
                        vector<CTrunkVlanRange>::iterator itRange = vecTrunkVlanRange.begin();
                        for(; itRange != vecTrunkVlanRange.end(); ++itRange)
                        {
                            if(pVlanMgr->IsRangeCrossRanges(itRange->m_nVlanBegin, itRange->m_nVlanEnd, cMsg.m_strNetPlaneUUID))
                            {
                                VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::SetPortGroup: PG vlanrange is conflict witch NetPlane's.\n");
                                VNetFormatString(strRetInfo, "PortGroup's vlanrange[%d-%d] is conflict witch NetPlane's.", 
                                    itRange->m_nVlanBegin,
                                    itRange->m_nVlanEnd);
                                return ERROR_PG_VLANRANGE_IS_CONFLICT;
                            }             
                        }
                    }
                }
            }
        }

        nRet = m_pDbPG->Modify(cDbPG);
        if(VNET_DB_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::SetPortGroup: Modify PortGroup DB failed. ret = %d\n", nRet);
            VNetFormatString(strRetInfo, "Fail to modify PortGroup(%s) in database.", strPGName.c_str());
            return ERROR_PORTGROUP_OPER_DB_FAIL;
        }
    }

    //描述信息可以单独修改
    if(cDbPG.GetDesc() != cMsg.m_strdescription)
    {
        cDbPG.SetDesc(cMsg.m_strdescription.c_str());
        nRet = m_pDbPG->Modify(cDbPG);
        if(VNET_DB_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::SetPortGroup: Modify PG description failed. ret = %d\n", nRet);
            VNetFormatString(strRetInfo, "Fail to modify the description of PortGroup(%s) in database.", strPGName.c_str());
            return ERROR_PORTGROUP_OPER_DB_FAIL;
        }        
    }
    
    return PORTGROUP_OPER_SUCCESS;          
}

int32 CPortGroupMgr::SetPGNativeVlan(CPortGroupMsg &cMsg, string &strRetInfo)
{
    string strPGName;
    int32 nRet = QueryPGByUUID(cMsg.m_strUUID, strPGName);
    if(PORTGROUP_OPER_SUCCESS != nRet)
    {
        VNetFormatString(strRetInfo, "PortGroup(UUID:%s) is not exist.", cMsg.m_strUUID.c_str());
        return nRet;
    }  
    
    CDbPortGroup cDbPG;
    cDbPG.SetUuid(cMsg.m_strUUID.c_str());
    nRet = m_pDbPG->Query(cDbPG);
    if(VNET_DB_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::SetPGNativeVlan: Query PortGroup DB failed. ret = %d\n", nRet);
        VNetFormatString(strRetInfo, "Fail to query PortGroup(UUID:%s) in database.", cMsg.m_strUUID.c_str());
        return ERROR_PORTGROUP_OPER_DB_FAIL;
    }

    //检查nativevlan是否在vlan range内
    if((EN_PORTGROUP_TYPE_VM_SHARE == cDbPG.GetPgType()) && (EN_SWITCHPORT_MODE_TRUNK == cDbPG.GetSwithportMode()))
    {
    //现在改为只检查vlan是否有效即可
        if(cMsg.m_nTrunkNativeVlan < VNET_MIN_VLAN_ID || cMsg.m_nTrunkNativeVlan > VNET_MAX_VLAN_ID )
    	{
            VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::SetPGNativeVlan: NativeVlan<%d> is invalid.\n", cMsg.m_nTrunkNativeVlan);  
            VNetFormatString(strRetInfo, "PortGroup native VLAN(%d) is invalid.", cMsg.m_nTrunkNativeVlan);
            return ERROR_PORTGROUP_NATIVE_VLAN_INVALID;
    	}
    #if 0
        if(IsInTrunkVlanRange(cMsg.m_strUUID, cMsg.m_nTrunkNativeVlan))
        {
            VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::SetPGNativeVlan: NativeVlan<%d> isn't in Trunk vlanrange.\n", cMsg.m_nTrunkNativeVlan);  
            VNetFormatString(strRetInfo, "PortGroup native VLAN(%d) is not in trunk vlanrange.", cMsg.m_nTrunkNativeVlan);
            return ERROR_PORTGROUP_NATIVE_VLAN_INVALID;
        }
    #endif
    }

    if(cMsg.m_nTrunkNativeVlan != cDbPG.GetTrunkNatvieVlanNum())
    {
        cDbPG.SetTrunkNatvieVlanNum(cMsg.m_nTrunkNativeVlan);
        nRet = m_pDbPG->Modify(cDbPG);
        if(VNET_DB_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::SetPGNativeVlan: Modify PG Native Vlan failed. ret = %d\n", nRet);
            VNetFormatString(strRetInfo, "Fail to modify PortGroup(UUID:%s) native VLAN in database.", cMsg.m_strUUID.c_str());
            return ERROR_PORTGROUP_OPER_DB_FAIL;
        }  
    }

    return PORTGROUP_OPER_SUCCESS;
}

/*******************************************************************************
* 函数名称： CmpPGData
* 功能描述： 比较PG属性是否被修改
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
BOOL CPortGroupMgr::CmpPGData(CDbPortGroup &cDbPG, const CPortGroupMsg &cMsg)
{
    BOOL bIsUpdate = VNET_FALSE;
    if(cMsg.m_strName != cDbPG.GetName())
    {
        cDbPG.SetName(cMsg.m_strName.c_str());
        bIsUpdate = VNET_TRUE;
    }
    if(cMsg.m_nPGType != cDbPG.GetPgType())
    {
        cDbPG.SetPgType(cMsg.m_nPGType);
        bIsUpdate = VNET_TRUE;
    }
    if(cMsg.m_strACL != cDbPG.GetAcl())
    {
        cDbPG.SetAcl(cMsg.m_strACL.c_str());
        bIsUpdate = VNET_TRUE;
    }
    if(cMsg.m_strQOS != cDbPG.GetQos())
    {
        cDbPG.SetQos(cMsg.m_strQOS.c_str());
        bIsUpdate = VNET_TRUE;
    }  
    if(cMsg.m_strPrior != cDbPG.GetAllowPriorities())
    {
        cDbPG.SetAllowPriorities(cMsg.m_strPrior.c_str());
        bIsUpdate = VNET_TRUE;
    }
    if(cMsg.m_strRcvBWLimit != cDbPG.GetRcvBandwidthLmt())
    {
        cDbPG.SetRcvBandwidthLmt(cMsg.m_strRcvBWLimit.c_str());
        bIsUpdate = VNET_TRUE;
    }
    if(cMsg.m_strRcvBWRsv != cDbPG.GetRcvBandwidthRsv())
    {
        cDbPG.SetRcvBandwidthRsv(cMsg.m_strRcvBWRsv.c_str());
        bIsUpdate = VNET_TRUE;
    }
    if(cMsg.m_nDefaultVlan != cDbPG.GetDftVlanId())
    {
        cDbPG.SetDftVlanId(cMsg.m_nDefaultVlan);
        bIsUpdate = VNET_TRUE;
    } 
    if(cMsg.m_nPromisc != cDbPG.GetPromiscuous())
    {
        cDbPG.SetPromiscuous(cMsg.m_nPromisc);
        bIsUpdate = VNET_TRUE;
    }
    if(cMsg.m_nMacVlanFilter != cDbPG.GetMacVlanFltEnable())
    {
        cDbPG.SetMacVlanFltEnable(cMsg.m_nMacVlanFilter);
        bIsUpdate = VNET_TRUE;
    }
    if(cMsg.m_strTxMacs != cDbPG.GetAllowTrmtMacs())
    {
        cDbPG.SetAllowTrmtMacs(cMsg.m_strTxMacs.c_str());
        bIsUpdate = VNET_TRUE;
    }
    if(cMsg.m_strTxMacvlans != cDbPG.GetAllowTrmtMacVlans())
    {
        cDbPG.SetAllowTrmtMacVlans(cMsg.m_strTxMacvlans.c_str());
        bIsUpdate = VNET_TRUE;
    } 
    if(cMsg.m_strPolicyBlkOver != cDbPG.GetPlyBlkOver())
    {
        cDbPG.SetPlyBlkOver(cMsg.m_strPolicyBlkOver.c_str());
        bIsUpdate = VNET_TRUE;
    }
    if(cMsg.m_strPolicyVlanOver != cDbPG.GetPlyVlanOver())
    {
        cDbPG.SetPlyVlanOver(cMsg.m_strPolicyVlanOver.c_str());
        bIsUpdate = VNET_TRUE;
    }
    if(cMsg.m_nVersion != cDbPG.GetVersion())
    {
        cDbPG.SetVersion(cMsg.m_nVersion);
        bIsUpdate = VNET_TRUE;
    }
    if(cMsg.m_strMgrId != cDbPG.GetMgrId())
    {
        cDbPG.SetMgrId(cMsg.m_strMgrId.c_str());
        bIsUpdate = VNET_TRUE;
    } 
    if(cMsg.m_strVSIType != cDbPG.GetTypeId())
    {
        cDbPG.SetTypeId(cMsg.m_strVSIType.c_str());
        bIsUpdate = VNET_TRUE;
    }
    if(cMsg.m_nMacChange != cDbPG.GetAllowMacChg())
    {
        cDbPG.SetAllowMacChg(cMsg.m_nMacChange);
        bIsUpdate = VNET_TRUE;
    }
    if(cMsg.m_nSwitchPortMode != cDbPG.GetSwithportMode())
    {
        cDbPG.SetSwithportMode(cMsg.m_nSwitchPortMode);
        bIsUpdate = VNET_TRUE;
    }
    if(cMsg.m_nIsCfgNetPlane != cDbPG.GetIsCfgNetplane())
    {
        cDbPG.SetIsCfgNetplane(cMsg.m_nIsCfgNetPlane);
        bIsUpdate = VNET_TRUE;
    }    
    if(cMsg.m_strNetPlaneUUID != cDbPG.GetNetplaneUuid())
    {
        cDbPG.SetNetplaneUuid(cMsg.m_strNetPlaneUUID.c_str());
        bIsUpdate = VNET_TRUE;
    }
    if(cMsg.m_nMTU != cDbPG.GetMtu())
    {
        cDbPG.SetMtu(cMsg.m_nMTU);
        bIsUpdate = VNET_TRUE;
    }

    if(cMsg.m_nTrunkNativeVlan != cDbPG.GetTrunkNatvieVlanNum())
    {
        cDbPG.SetTrunkNatvieVlanNum(cMsg.m_nTrunkNativeVlan);  
        bIsUpdate = VNET_TRUE;
    }

    if(cMsg.m_nAccessVlanNum != cDbPG.GetAccessVlanNum())
    {
        cDbPG.SetAccessVlanNum(cMsg.m_nAccessVlanNum);
        bIsUpdate = VNET_TRUE;
    }    
    if (EN_ISOLATE_TYPE_VLAN==cDbPG.GetIsolateType())
    {
        if(cMsg.m_nAccessIsolateNum != cDbPG.GetAccessIsolateNo())
        {
            cDbPG.SetAccessIsolateNo(cMsg.m_nAccessIsolateNum);
            bIsUpdate = VNET_TRUE;
        } 
    }
    else
    {
        if(cMsg.m_nAccessIsolateNum != cDbPG.GetVxlanIsolateNo())
        {
            cDbPG.SetVxlanIsolateNo(cMsg.m_nAccessIsolateNum);
            bIsUpdate = VNET_TRUE;
        } 
    }
    
    if(cMsg.m_nIsolateType != cDbPG.GetIsolateType())
    {
        cDbPG.SetIsolateType(cMsg.m_nIsolateType);
        bIsUpdate = VNET_TRUE;
    } 
    if(cMsg.m_nSegmentId != cDbPG.GetSegmentIdNum())
    {
        cDbPG.SetSegmentIdNum(cMsg.m_nSegmentId);
        bIsUpdate = VNET_TRUE;
    } 
 
    return bIsUpdate;
}

int32 CPortGroupMgr::IsUplinkPG(const string &strPGUuid)
{
    CDbPortGroup cDbPG;
    cDbPG.SetUuid(strPGUuid.c_str());
    if(VNET_DB_SUCCESS == m_pDbPG->Query(cDbPG))
    {
        if(cDbPG.GetPgType() == EN_PORTGROUP_TYPE_UPLINK)
        {
            return 0;
        }        
    }
    return -1;
}

int32 CPortGroupMgr::IsNeedVlanMap(const string &strPGUuid, int32 &nIsolateNo)
{
    CDbPortGroup cDbPG;
    cDbPG.SetUuid(strPGUuid.c_str());
    int32 nRet = m_pDbPG->Query(cDbPG);
    if(VNET_DB_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::IsNeedVlanMap: Query portgroupDB failed. ret = %d\n", nRet);   
        return -1;
    }    
    if((EN_SWITCHPORT_MODE_ACCESS == cDbPG.GetSwithportMode())&&
        (EN_PORTGROUP_TYPE_VM_PRIVATE == cDbPG.GetPgType()) &&
        (EN_ISOLATE_TYPE_VLAN == cDbPG.GetIsolateType()))
    {
        nIsolateNo = cDbPG.GetAccessIsolateNo();
        return PORTGROUP_OPER_SUCCESS;
    }
    return -1;
}

int32 CPortGroupMgr::IsNeedSegmentMap(const string &strPGUuid, int32 &nIsolateNo)
{
    CDbPortGroup cDbPG;
    cDbPG.SetUuid(strPGUuid.c_str());
    int32 nRet = m_pDbPG->Query(cDbPG);
    if(VNET_DB_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::IsNeedVlanMap: Query portgroupDB failed. ret = %d\n", nRet);   
        return -1;
    }    

    if(m_bOpenDbg)
    {
        cout << " pg uuid: " << cDbPG.GetUuid() << "    "
             << " pg type: " << cDbPG.GetPgType() << "    "
             << " pg isolate type: " << cDbPG.GetIsolateType() << endl;
    }     
    
    if(EN_PORTGROUP_TYPE_VM_PRIVATE == cDbPG.GetPgType() &&
       (EN_ISOLATE_TYPE_VXLAN == cDbPG.GetIsolateType()))
    {
        nIsolateNo = cDbPG.GetVxlanIsolateNo();

        if(m_bOpenDbg)
        {
            cout << "get private vxlan pg isolate: " << nIsolateNo << " ok!" << endl;
        }        
        return PORTGROUP_OPER_SUCCESS;
    }
   
    return -1;
}

BOOL CPortGroupMgr::IsPortGroupSdn(const string &strPGUuid)
{
    CDbPortGroup cDbPG;
    cDbPG.SetUuid(strPGUuid.c_str());
    int32 nRet = m_pDbPG->Query(cDbPG);
    if(VNET_DB_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::IsPortGroupSdn: Query portgroupDB failed. ret = %d\n", nRet);   
        return FALSE;
    }

    if(1 == cDbPG.GetIsSdn())
    {
        return TRUE;
    }

    return FALSE;
}

int32  CPortGroupMgr::CreateQuantumPort(CVNetVmMem &cVnetVmMem)
{
    vector<CVNetVnicMem> vecNic;
    cVnetVmMem.GetVecVnicMem(vecNic);
    int32 iRet = PORT_OPER_SUCCESS;
    string strSvrIP = "";
    
    CLogicNetworkMgr *pLGMgr = CLogicNetworkMgr::GetInstance();
    if (NULL == pLGMgr)
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::CreateQuantumPort: call CLogicNetworkMgr::GetInstance() failed.\n");
        return -1;
    }

    CVNetDbMgr * pMgr = CVNetDbMgr::GetInstance();
    if(NULL == pMgr)
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::CreateQuantumPort: GetInstance() is NULL.\n");
        return DB_ERROR_GET_INST_FAILED;
    }

    if(vecNic.empty())
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::CreateQuantumPort: No Exist Vnics.\n");
        return PORT_OPER_SUCCESS;
    }

    vector<CVNetVnicMem>::iterator itrVnic = vecNic.begin();
    for( ; itrVnic != vecNic.end(); ++itrVnic)
    {
        string strPortGroupUUID;
        iRet = pLGMgr->GetPortGroupUUID(itrVnic->GetLogicNetworkUuid(), strPortGroupUUID);
        if (0 != iRet)
        {
            VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::CreateQuantumPort: call GetPortGroupUUID(%s) failed.\n", itrVnic->GetLogicNetworkUuid().c_str());
            return iRet;
        }

        CPortGroup cPortGroup;
        cPortGroup.SetUuid(strPortGroupUUID);
        GetPGDetail(cPortGroup);

        if(1 == cPortGroup.GetIsSdn())
        {
            string strQuantumPortUuid = "";
            CDBOperateSdnCfg * pSdn = pMgr->GetISdnCfg();
            if(NULL != pSdn)
            {
                CDbQuantumRestfulCfg cQuantumRestful;
                int32 nRet = pSdn->QueryQuantum(cQuantumRestful);
                if(VNET_DB_SUCCESS == nRet)
                {
                    strSvrIP = cQuantumRestful.GetIp();
                    cout<<"server IP: "<<strSvrIP<<endl;
                }
            }

            string strPortName = "port" + int2string(itrVnic->GetVmId()) + "." + int2string(itrVnic->GetNicIndex());

            cout<<"port name: "<<strPortName<<endl;
                
            /* 调用RESTful接口创建端口,network从vsi中获取 */
            string strTenantId;
            if(EN_PORTGROUP_TYPE_VM_SHARE == cPortGroup.GetPgType())
            {
                strTenantId = int2string(0);

                //共享网络,network从数据库中获取
                vector<CDbQuantumNetwork> vecQNInfo;
                CDBOperateQuantumNetwork *pQuantumNetwork = pMgr->GetIQuantumNetwork();
                if(NULL != pQuantumNetwork)
                {
                    pQuantumNetwork->QueryAll(vecQNInfo);                        
                }

                vector<CDbQuantumNetwork>::iterator itrQuantumNetwork = vecQNInfo.begin();
                for( ; itrQuantumNetwork != vecQNInfo.end(); ++itrQuantumNetwork)
                {
                    if(itrQuantumNetwork->GetLnUuid() == itrVnic->GetLogicNetworkUuid())
                    {
                        itrVnic->SetQuantumNetworkUuid(itrQuantumNetwork->GetUuid().c_str());
                        break;
                    }
                }
            }
            else
            {
                strTenantId = int2string(cVnetVmMem.GetProjectId());
            }

            cout<<"Quantum Network uuid: "<<itrVnic->GetQuantumNetworkUuid()<<endl;
            cout<<"Tenant ID:"<<strTenantId<<endl;
            
            RESTfulCreatePort(strSvrIP, strPortName, itrVnic->GetQuantumNetworkUuid(), strTenantId, strQuantumPortUuid);

            itrVnic->SetQuantumPortUuid(strQuantumPortUuid);

            cout<<"Port UUid: "<<strQuantumPortUuid<<endl;

            if(0 != strQuantumPortUuid.size())
            {
                CDbQuantumPort cDbQuantumPort;
                cDbQuantumPort.SetUuid(strQuantumPortUuid.c_str());
                cDbQuantumPort.SetQNUuid(itrVnic->GetQuantumNetworkUuid().c_str());

                CDBOperateQuantumPort *pQuantumPort = pMgr->GetIQuantumPort();
                if(NULL != pQuantumPort)
                {
                    pQuantumPort->Add(cDbQuantumPort);
                }
            }
        }
    }

    cVnetVmMem.SetVecVnicMem(vecNic);

    return 0;
}

int32  CPortGroupMgr::DeleteQuantumPort(CVNetVmMem &cVnetVmMem)
{
    vector<CVNetVnicMem> vecNic;
    cVnetVmMem.GetVecVnicMem(vecNic);
    int32 iRet = PORT_OPER_SUCCESS;
    string strSvrIP = "";
    
    CLogicNetworkMgr *pLGMgr = CLogicNetworkMgr::GetInstance();
    if (NULL == pLGMgr)
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::DeleteQuantumPort: call CLogicNetworkMgr::GetInstance() failed.\n");
        return -1;
    }

    CVNetDbMgr * pMgr = CVNetDbMgr::GetInstance();
    if(NULL == pMgr)
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::DeleteQuantumPort: GetInstance() is NULL.\n");
        return DB_ERROR_GET_INST_FAILED;
    }

    if(vecNic.empty())
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::DeleteQuantumPort: No Exist Vnics.\n");
        return PORT_OPER_SUCCESS;
    }

    vector<CVNetVnicMem>::iterator itrVnic = vecNic.begin();
    for( ; itrVnic != vecNic.end(); ++itrVnic)
    {
        string strPortGroupUUID;
        iRet = pLGMgr->GetPortGroupUUID(itrVnic->GetLogicNetworkUuid(), strPortGroupUUID);
        if (0 != iRet)
        {
            VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::DeleteQuantumPort: call GetPortGroupUUID(%s) failed.\n", itrVnic->GetLogicNetworkUuid().c_str());
            return iRet;
        }

        CPortGroup cPortGroup;
        cPortGroup.SetUuid(strPortGroupUUID);
        GetPGDetail(cPortGroup);

        if(1 == cPortGroup.GetIsSdn())
        {
            CDBOperateSdnCfg * pSdn = pMgr->GetISdnCfg();
            if(NULL != pSdn)
            {
                CDbQuantumRestfulCfg cQuantumRestful;
                int32 nRet = pSdn->QueryQuantum(cQuantumRestful);
                if(VNET_DB_SUCCESS == nRet)
                {
                    strSvrIP = cQuantumRestful.GetIp();
                    cout<<"server IP: "<<strSvrIP<<endl;
                }
            }

            CDBOperateQuantumPort *pQuantumPort = pMgr->GetIQuantumPort();
            if(NULL != pQuantumPort)
            {
                pQuantumPort->Del(itrVnic->GetQuantumPortUuid().c_str());
            }

            cout<<"delete Quantum port uuid: "<<itrVnic->GetQuantumPortUuid()<<endl;

            RESTfulDeletePort(strSvrIP, itrVnic->GetQuantumPortUuid());

            itrVnic->SetQuantumPortUuid("");            
        }
    }

    cVnetVmMem.SetVecVnicMem(vecNic);

    return 0;
}

int32 CPortGroupMgr::GetPGDetail(CPortGroup &cPGInfo)
{
    CDbPortGroup cDbPG;
    cDbPG.SetUuid(cPGInfo.GetUuid().c_str());
    int32 nRet = m_pDbPG->Query(cDbPG);
    if(VNET_DB_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::GetPGDetail: Query PG DB failed. ret = %d\n", nRet);   
        return -1;
    }  

    cPGInfo.SetName(cDbPG.GetName());              
    cPGInfo.SetPgType(cDbPG.GetPgType());            
    cPGInfo.SetAcl(cDbPG.GetAcl());               
    cPGInfo.SetQos(cDbPG.GetQos());               
    cPGInfo.SetRcvBandwidthLmt(cDbPG.GetRcvBandwidthLmt());   
    cPGInfo.SetRcvBandwidthRsv(cDbPG.GetRcvBandwidthRsv());    
    cPGInfo.SetDftVlanId(cDbPG.GetDftVlanId());        
    cPGInfo.SetPromiscuous(cDbPG.GetPromiscuous());       
    cPGInfo.SetMacVlanFltEnable(cDbPG.GetMacVlanFltEnable());  
    cPGInfo.SetAllowTrmtMacs(cDbPG.GetAllowTrmtMacs());     
    cPGInfo.SetAllowTrmtMacVlans(cDbPG.GetAllowTrmtMacVlans());  
    cPGInfo.SetPlyBlkOver(cDbPG.GetPlyBlkOver());       
    cPGInfo.SetPlyVlanOver(cDbPG.GetPlyVlanOver());     
    cPGInfo.SetVersion(cDbPG.GetVersion());         
    cPGInfo.SetMgrId(cDbPG.GetMgrId());            
    cPGInfo.SetTypeId(cDbPG.GetTypeId());           
    cPGInfo.SetAllowMacChg(cDbPG.GetAllowMacChg());       
    cPGInfo.SetSwithportMode(cDbPG.GetSwithportMode());     
    cPGInfo.SetIsCfgNetplane(cDbPG.GetIsCfgNetplane());    
    cPGInfo.SetNetplaneUuid(cDbPG.GetNetplaneUuid());
    cPGInfo.SetIsSdn(cDbPG.GetIsSdn());
    cPGInfo.SetMtu(cDbPG.GetMtu());             
    cPGInfo.SetTrunkNatvieVlanNum(cDbPG.GetTrunkNatvieVlanNum()); 
    cPGInfo.SetAccessVlanNum(cDbPG.GetAccessVlanNum());   
    if (EN_DB_VMPG_ISOLATE_TYPE_VLAN == cDbPG.GetIsolateType())
    {
        cPGInfo.SetAccessIsolateNo(cDbPG.GetAccessIsolateNo());
    }
    if (EN_DB_VMPG_ISOLATE_TYPE_VXLAN == cDbPG.GetIsolateType())
    {
        cPGInfo.SetAccessIsolateNo(cDbPG.GetVxlanIsolateNo());
    }
    cPGInfo.SetIsolateType(cDbPG.GetIsolateType());   
    cPGInfo.SetSegmentId(cDbPG.GetSegmentIdNum());

    vector<CDbPgTrunkVlanRange> vecDbTrunkVRange;
    nRet = m_pVlanRange->Query(cPGInfo.GetUuid().c_str(), vecDbTrunkVRange);
    if(VNET_DB_SUCCESS == nRet)
    {
        vector<CDbPgTrunkVlanRange>::iterator iter = vecDbTrunkVRange.begin();
        for( ; iter != vecDbTrunkVRange.end(); ++iter)
        {
            CTrunkVlanRange tCTrunkVRange; 
            tCTrunkVRange.m_nVlanBegin = iter->GetVlanBegin();
            tCTrunkVRange.m_nVlanEnd   = iter->GetVlanEnd();
            cPGInfo.SetTrunkVlanRange(tCTrunkVRange);
        }
    }
    return 0;
}

int32 CPortGroupMgr::GetVMPGDetail(vector<CVNetVnicDetail> &vecVnicDetail)
{
    int32 nRet;
    vector<CVNetVnicDetail>::iterator iter = vecVnicDetail.begin();
    for(; iter != vecVnicDetail.end(); ++iter)
    {
        CDbPortGroup cDbPG;
        cDbPG.SetUuid(iter->GetPGUuid().c_str());
        nRet = m_pDbPG->Query(cDbPG);
        if(VNET_DB_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::GetVMPGDetail: Query portgroupDB failed. ret = %d\n", nRet);   
            return -1;
        }   

        CNetplaneMgr *pMgr = CNetplaneMgr::GetInstance();
        if (NULL == pMgr)
        {
            VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::GetVMPGDetail: CNetplaneMgr::GetInstance() fail.\n");
            return -1;
        }        

        int32 nId;
        string strNetplaneUuid = cDbPG.GetNetplaneUuid();
        if(0!=pMgr->GetNetplaneId(strNetplaneUuid, nId))
        {
            VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::GetVMPGDetail: (Netplane %s)GetNetplaneId fail.\n", strNetplaneUuid.c_str());
            return -1;
        }

        string strNickName;
        if(0!=pMgr->ConvertIdToNickName(nId, strNickName))
        {
            VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::GetVMPGDetail: (Netplane %s, nId %d)ConvertIdToNickName fail.\n", 
                strNetplaneUuid.c_str(), nId);
            return -1;
        }

        iter->SetAcl(cDbPG.GetAcl()); 
        iter->SetQos(cDbPG.GetQos()); 
        iter->SetAllowedPriorities(cDbPG.GetAllowPriorities());  
        iter->SetReceiveBandwidthLimit(cDbPG.GetRcvBandwidthLmt());  
        iter->SetReceiveBandwidthReserve(cDbPG.GetRcvBandwidthRsv()) ;
        iter->SetDefaultVlanID(cDbPG.GetDftVlanId());
        iter->SetPromiscuous(cDbPG.GetPromiscuous());
        iter->SetMacVlanFilterEnable(cDbPG.GetMacVlanFltEnable());
        iter->SetAllowedTransmitMacs(cDbPG.GetAllowTrmtMacs());
        iter->SetAllowedTransmitMacvlans(cDbPG.GetAllowTrmtMacVlans());
        iter->SetPolicyBlockOverride(cDbPG.GetPlyBlkOver()); 
        iter->SetPolicyVlanOverride(cDbPG.GetPlyVlanOver());  
        iter->SetAllowMacChange(cDbPG.GetAllowMacChg()) ;
        iter->SetMTU(cDbPG.GetMtu()) ;
        iter->SetSwitchportMode(cDbPG.GetSwithportMode());
        iter->SetPGType(cDbPG.GetPgType());
        iter->SetAccessVlan(cDbPG.GetAccessVlanNum());
        iter->SetNetplaneNickName(strNickName);
        iter->SetSegmentId(cDbPG.GetSegmentIdNum());
        iter->SetIsolateType(cDbPG.GetIsolateType());

    }
    return 0;
}

int32 CPortGroupMgr::GetPGTrunkNativeVlan(const string &strPGUuid, int32 &nTrunkNativeVlan)
{
    CDbPortGroup cDbPG;
    cDbPG.SetUuid(strPGUuid.c_str());
    int32 nRet = m_pDbPG->Query(cDbPG);
    if(VNET_DB_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::GetPGTrunkNativeVlan: Query portgroupDB failed. ret = %d\n", nRet);   
        return -1;
    }

    if(EN_SWITCHPORT_MODE_TRUNK == cDbPG.GetSwithportMode())
    {
        nTrunkNativeVlan = cDbPG.GetTrunkNatvieVlanNum(); 
        return 0;
    }
    else
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::GetPGTrunkNativeVlan: switchport mode<%d> is invalid.\n", cDbPG.GetSwithportMode());
        return -1;
    }
}

int32 CPortGroupMgr::GetPGAccessVlan(int64 nProjID, const string &strPGUuid, int32 &nAccessVlan)
{
    CDbPortGroup cDbPG;
    cDbPG.SetUuid(strPGUuid.c_str());
    int32 nRet = m_pDbPG->Query(cDbPG);
    if(VNET_DB_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::GetPGAccessVlan: Query portgroupDB failed. ret = %d\n", nRet);   
        return -1;
    }

    if(EN_SWITCHPORT_MODE_ACCESS != cDbPG.GetSwithportMode())
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::GetPGAccessVlan: switchport mode<%d> is invalid.\n", cDbPG.GetSwithportMode());
        return -1;
    }    

    //EN_PORTGROUP_TYPE_VM_PRIVATE
    if(EN_PORTGROUP_TYPE_VM_PRIVATE == cDbPG.GetPgType())
    {
        CDBOperateVlanMap *pOperVlanMap = GetDbIVlanMap();
        if(NULL == pOperVlanMap)
        {
            VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::GetPGAccessVlan: GetDbIVlanMap is NULL .\n");
            return -1;
        } 
        
        int32 nIsolationNo = 0;
        if(PORTGROUP_OPER_SUCCESS != IsNeedVlanMap(strPGUuid, nIsolationNo))
        {
            VNET_LOG(VNET_LOG_INFO, "CPortGroupMgr::GetPGAccessVlan: Get PG(ProjID:0x%llx, PGID:%s) isolateNo failed.\n", 
                nProjID, strPGUuid.c_str()); 
            return -1;
        }             
        // Get the vlan num;                
        nRet = pOperVlanMap->QueryByProjectIdPGIsolateNo(nProjID, strPGUuid.c_str(), nIsolationNo, nAccessVlan);                                                             
        if(VNET_DB_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CPortGroupMgr::GetPGAccessVlan: DB Query Vlan Map(Proj:%d, PG:%s, nIsolationNo: %d) failed.ret: %d.\n",
                nProjID, strPGUuid.c_str(), nIsolationNo, nRet);
            return -1;
        }        
    }
    //EN_PORTGROUP_TYPE_VM_SHARE
    else if(EN_PORTGROUP_TYPE_VM_SHARE == cDbPG.GetPgType())
    {        
        nAccessVlan = cDbPG.GetAccessVlanNum();        
    }
    else
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::GetPGAccessVlan: PG type(%d) error, GetPgType failed.\n",cDbPG.GetPgType());   
        return -1;
    }
    
    return 0;
}

int32 CPortGroupMgr::GetPGSegmentId(int64 nProjID, const string &strPGUuid, int32 &nSegmentId)
{
    CDbPortGroup cDbPG;
    cDbPG.SetUuid(strPGUuid.c_str());
    int32 nRet = m_pDbPG->Query(cDbPG);
    if (VNET_DB_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::GetPGSegmentId: Query portgroupDB failed. ret = %d\n", nRet);   
        return -1;
    }

    if ( EN_PORTGROUP_TYPE_VM_PRIVATE == cDbPG.GetPgType())
    {
        CDBOperateSegmentMap *pOperSegmentMap = GetDbISegmentMap();
        if (NULL == pOperSegmentMap)
        {
            VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::GetPGSegmentId: GetDbISegmentMap is NULL .\n");
            return -1;
        } 
        
        int32 nIsolationNo = 0;
        if (PORTGROUP_OPER_SUCCESS != IsNeedSegmentMap(strPGUuid, nIsolationNo))
        {
            VNET_LOG(VNET_LOG_INFO, "CPortGroupMgr::GetPGSegmentId: Get PG(ProjID:0x%llx, PGID:%s) isolateNo failed.\n", 
                nProjID, strPGUuid.c_str()); 
            return -1;
        }             
        // Get the Segment num;                
        int32 nRet = pOperSegmentMap->QueryByProjectIdPGIsolateNo(nProjID, strPGUuid.c_str(), nIsolationNo, nSegmentId);                                                         
        if (VNET_DB_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CPortGroupMgr::GetPGSegmentId: DB Query Segment Map(Proj:%d, PG:%s, nIsolationNo: %d) failed.ret: %d.\n",
                nProjID, strPGUuid.c_str(), nIsolationNo, nRet);
            return -1;
        }        
    }
    else if (EN_PORTGROUP_TYPE_VM_SHARE == cDbPG.GetPgType())
    {
        nSegmentId = cDbPG.GetSegmentIdNum();        
    }
    else
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::GetPGSegmentId: PG type(%d) error, GetPgType failed.\n",cDbPG.GetPgType());   
        return -1;
    }

    return 0;
}


int32 CPortGroupMgr::GetPGNetplane(const string &strPGUuid, string &strNPUuid)
{
    CDbPortGroup cDbPG;
    cDbPG.SetUuid(strPGUuid.c_str());
    int32 nRet = m_pDbPG->Query(cDbPG);
    if(VNET_DB_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::GetPGNetplane: Query portgroupDB failed. ret = %d\n", nRet);   
        return -1;
    }  

    strNPUuid.assign("");
    if(cDbPG.GetIsCfgNetplane())
    {
        strNPUuid = cDbPG.GetNetplaneUuid();
    }
    return 0;
}

int32 CPortGroupMgr::RESTfulCreatePort(const string &cstrQuantumSvrIP, const string &cstrPortName, const string &cstrNetworkUuid, const string &cstrTenantid, string &strPortUuid)
{
    restrpc_c::clientSimple myClient;

    if(0 == cstrQuantumSvrIP.size() ||
        0 == cstrPortName.size() ||
        0 == cstrNetworkUuid.size() ||
        0 == cstrTenantid.size())
    {
        return 0;
    }
        
    string serverUrl("http://");        
    serverUrl += cstrQuantumSvrIP;
    serverUrl += ":9696/v2.0/ports.json";

    cout<<"URL: "<<serverUrl<<endl;

    value result;
    /* 构造入参，注意JSON最外层是一个struct */
    map<string, value> paramList;
    map<string, value> substructData;

    pair<string, value> mbr_name("name", value_string(cstrPortName));
    pair<string, value> mbr_state("admin_state_up", value_boolean(true));
    pair<string, value> mbr_networkid("network_id", value_string(cstrNetworkUuid));
    pair<string, value> mbr_tenantid("tenant_id", value_string(cstrTenantid));

    substructData.insert(mbr_name);
    substructData.insert(mbr_state);
    substructData.insert(mbr_networkid);
    substructData.insert(mbr_tenantid);

    carray arrayData;
    arrayData.push_back(value_struct(substructData));

    value_array array(arrayData);
    pair<string, value> mbr_port("ports",value_array(array));
    paramList.insert(mbr_port);

    myClient.call(serverUrl, HTTP_POST, value_struct(paramList), &result);

    map<string, value> returnValue = value_struct(result);
    value_array port = value_array(returnValue["ports"]);

    vector<value> dataReadBack(port.vectorValueValue());
    map<string, value>data = value_struct(dataReadBack[0]);
    
    strPortUuid = value_string(data["id"]);

    return 0;
}

int32 CPortGroupMgr::RESTfulDeletePort(const string &cstrQuantumSvrIP, const string &cstrPortUuid)
{
    restrpc_c::clientSimple myClient;

    if(0 == cstrQuantumSvrIP.size() || 0 == cstrPortUuid.size())
    {
        return 0;
    }
        
    string serverUrl("http://");        
    serverUrl += cstrQuantumSvrIP;
    serverUrl += ":9696/v2.0/ports/";

    cout<<"URL: "<<serverUrl+cstrPortUuid<<endl;

    value result;
    myClient.call(serverUrl+cstrPortUuid, HTTP_DELETE, &result);

    return 0;
}
int32 CPortGroupMgr::GetPGIsolateType(const string &strPGUuid, int32 &nIsoType)
{
    CDbPortGroup cDbPG;
    cDbPG.SetUuid(strPGUuid.c_str());
    int32 nRet = m_pDbPG->Query(cDbPG);
    if(VNET_DB_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::GetPGNetplane: Query portgroupDB failed. ret = %d\n", nRet);   
        return -1;
    }  
    
    nIsoType = cDbPG.GetIsolateType();
    return 0;
}

void CPortGroupMgr::DbgShowPortGroup()
{
    vector<CDbPortGroupSummary> outVInfo;
    int32 nRet = m_pDbPG->QuerySummary(outVInfo);
    if(VNET_DB_SUCCESS == nRet)
    {
        vector<CDbPortGroupSummary>::iterator iter;
        for(iter = outVInfo.begin(); iter != outVInfo.end(); ++iter)
        {
            cout << "--------------------------------------------------------" << endl;
            CDbPortGroup cDbPG;
            cDbPG.SetUuid(iter->GetUuid().c_str());
            nRet = m_pDbPG->Query(cDbPG); 
            if(VNET_DB_SUCCESS == nRet)
            {
                //打印
                cDbPG.DbgShow();
            }
            cout << "--------------------------------------------------------" << endl;
        }
    }
}

/*******************************************************************************
* 函数名称： ProcPGTrunkVlanRangeMsg
* 功能描述： 添加trunk vlan range
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
int32 CPortGroupMgr::ProcPGTrunkVlanRangeMsg(const MessageFrame &message)
{
    int32 nRet = 0;
    string strRetInfo;
    CPGTrunkVlanRangeMsg cPGMsg;
    cPGMsg.deserialize(message.data);

    MessageOption option(message.option.sender(), EV_VNETLIB_PG_TRUNK_VLANRANGE_CFG_ACK, 0, 0);

    if(!cPGMsg.IsValidOper())
    {
        VNET_LOG(VNET_LOG_WARN, "CPortGroupMgr::ProcPGTrunkVlanRangeMsg: Msg operation code (%d) is invalid.\n", cPGMsg.m_nOper);
        nRet = ERROR_PORTGROUP_OPERCODE_INVALID;
        VNetFormatString(strRetInfo, "PortGroup operator(%d) is invalid.", cPGMsg.m_nOper);
        goto PGTRUNK_OPER_ACK;
    }
    
    if(cPGMsg.m_strPGUuid.empty())
    {
        VNET_LOG(VNET_LOG_WARN, "CPortGroupMgr::ProcPGTrunkVlanRangeMsg: The UUID of PortGroup is invalid.\n");
        nRet =  ERROR_PORTGROUP_PG_INVALID;
        strRetInfo = "The UUID of PortGroup is invalid.";
        goto PGTRUNK_OPER_ACK;
    }

    if(m_bOpenDbg)
    {
        cPGMsg.Print();
    }
    
    switch(cPGMsg.m_nOper)
    {   
        case EN_ADD_PG_TRUNK_VLANRANGE:
        {
            nRet = AddPGTrunkVlanRange(cPGMsg, strRetInfo);
            break;
        }
        case EN_DEL_PG_TRUNK_VLANRANGE:
        {
            nRet = DelPGTrunkVlanRange(cPGMsg, strRetInfo);
            break;
        }
        default:
        {
            VNET_ASSERT(0);
        }
    }
    
PGTRUNK_OPER_ACK:
    cPGMsg.SetResult(nRet);
    cPGMsg.SetResultinfo(strRetInfo);
    return m_pMU->Send(cPGMsg, option);    
}

/*******************************************************************************
* 函数名称： AddPGTrunkVlanRange
* 功能描述： 添加端口组trunk vlan range
* 访问的表： CDbPgTrunkVlanRange
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* ------------------------------------------------------------------------------
* 2013/3          V1.0       zhongchsh     创 建
*******************************************************************************/
int32 CPortGroupMgr::AddPGTrunkVlanRange(const CPGTrunkVlanRangeMsg &cMsg, string &strRetInfo)
{
    int32 nRet = PORTGROUP_OPER_SUCCESS;

    nRet = m_pDbPG->IsUsing(cMsg.m_strPGUuid.c_str());
    if(VNET_DB_SUCCESS != nRet)
    {
        if(VNET_DB_IS_RESOURCE_USING(nRet))
        {
            VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::AddPGTrunkVlanRange: PG is using, ret = %d\n", nRet);
            VNetFormatString(strRetInfo, "PortGroup(UUID:%s) is inusing, don't add vlanrange.", cMsg.m_strPGUuid.c_str());
            return ERROR_PORTGROUP_IS_INUSING;
        }
        else
        {
            VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::AddPGTrunkVlanRange: call IsUsing failed, ret = %d\n", nRet);
            strRetInfo = "Fail to database check for PortGroup is using.";
            return ERROR_PORTGROUP_OPER_DB_FAIL;
        }
    }  
    
    if(FALSE == IsValidVlanRange(cMsg.m_nVlanBegin, cMsg.m_nVlanEnd))
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::AddPGTrunkVlanRange: vlan range[%d, %d] is invalid.\n",
                  cMsg.m_nVlanBegin, cMsg.m_nVlanEnd);    
        VNetFormatString(strRetInfo, "The trunk vlanrange[%d-%d] is invalid.", 
            cMsg.m_nVlanBegin, cMsg.m_nVlanEnd);
        return ERROR_PG_VLANRANGE_INVLAID;
    }
    
    //VLAN范围是否在网络平面VLAN范围外
    CDbPortGroup cDbPG;
    cDbPG.SetUuid(cMsg.m_strPGUuid.c_str());
    nRet = m_pDbPG->Query(cDbPG);
    if(VNET_DB_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::AddPGTrunkVlanRange: Query PG DB failed. ret = %d\n", nRet);  
        VNetFormatString(strRetInfo, "Fail to query PortGroup(UUID:%s) in database.", cMsg.m_strPGUuid.c_str());
        return ERROR_PORTGROUP_OPER_DB_FAIL;
    }  
    
    CVlanPoolMgr *pVlanMgr = CVlanPoolMgr::GetInstance();
    if(NULL == pVlanMgr)
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::AddPortGroup: CVlanPoolMgr::GetInstance() is NULL.\n");
        strRetInfo = "The handler of CVlanPoolMgr is NULL.";
        return ERROR_PORTGROUP_GET_INSTANCE_FAIL;
    } 
    
    //配置了网络平面范围需要检查trunk range
    if(cDbPG.GetIsCfgNetplane() && pVlanMgr->IsExistVlanRange(cDbPG.GetNetplaneUuid()))
    {
        if(pVlanMgr->IsRangeCrossRanges(cMsg.m_nVlanBegin, cMsg.m_nVlanEnd, cDbPG.GetNetplaneUuid()))
        {
            VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::AddPGTrunkVlanRange: PG vlanrange is conflict witch NetPlane vlanrange.\n");
            VNetFormatString(strRetInfo, "PortGroup's vlanrange[%d-%d] is conflict witch NetPlane's.", 
                cMsg.m_nVlanBegin,
                cMsg.m_nVlanEnd);
            return ERROR_PG_VLANRANGE_IS_CONFLICT;
        }
    }

    vector<CDbPgTrunkVlanRange> outVInfo;
    nRet = m_pVlanRange->Query(cMsg.m_strPGUuid.c_str(), outVInfo);
    if(VNET_DB_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, 
            "CPortGroupMgr::AddPGTrunkVlanRange: DB Query PG's(UUID: %s) vlan range failed, ret = %d.\n",
            cMsg.m_strPGUuid.c_str(), nRet);
        VNetFormatString(strRetInfo, "Fail to query PortGroup(UUID:%s)'s vlanrange in database.", cMsg.m_strPGUuid.c_str());
        return ERROR_PG_VLANRANGE_OPER_DB_FAIL;
    }     
    
    CVlanRange cVlanRange;
    CDbPgTrunkVlanRange cDbPgRange;
    vector<CDbPgTrunkVlanRange>::iterator itrDBRange;
    if(!outVInfo.empty())
    {
        for(itrDBRange = outVInfo.begin(); itrDBRange != outVInfo.end(); ++itrDBRange)
        {
            TVlanRangeItem tItem;
            tItem.dwVlanIDStart = itrDBRange->GetVlanBegin();
            tItem.dwVlanIDEnd = itrDBRange->GetVlanEnd();
            cVlanRange.AddValidRange(tItem);
        }
    }
    else
    {   
        // only one record, save it to DB;
        cDbPgRange.SetPgUuid(cMsg.m_strPGUuid.c_str());
        cDbPgRange.SetVlanBegin(cMsg.m_nVlanBegin);
        cDbPgRange.SetVlanEnd(cMsg.m_nVlanEnd);    
        nRet = m_pVlanRange->Add(cDbPgRange);
        if(VNET_DB_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CPortGroupMgr::AddPGTrunkVlanRange: DB Add PG's(UUID: %s) vlan range[%d-%d] failed, ret = %d.\n",
                cMsg.m_strPGUuid.c_str(), cMsg.m_nVlanBegin, cMsg.m_nVlanEnd, nRet);
            VNetFormatString(strRetInfo, "Fail to add PortGroup(UUID:%s)'s vlanrange[%d-%d] to database.", 
                cMsg.m_strPGUuid.c_str(), cMsg.m_nVlanBegin, cMsg.m_nVlanEnd);
            return ERROR_PG_VLANRANGE_OPER_DB_FAIL;
        } 
        return PORTGROUP_OPER_SUCCESS;
    }      
    
    // range add;
    TVlanRangeItem tNewItem;
    tNewItem.dwVlanIDStart = cMsg.m_nVlanBegin;
    tNewItem.dwVlanIDEnd   = cMsg.m_nVlanEnd;
    if(0 != cVlanRange.AddRange(tNewItem))
    {            
        VNET_LOG(VNET_LOG_ERROR, 
            "CPortGroupMgr::AddPGTrunkVlanRange: vlan range[%d-%d] calculate failed.\n",
            cMsg.m_nVlanBegin, 
            cMsg.m_nVlanEnd);
        VNetFormatString(strRetInfo, "Combining the calculation of vlanrange[%d-%d] failed.\n",
            cMsg.m_nVlanBegin,
            cMsg.m_nVlanEnd);
        return ERROR_PG_VLANRANGE_CALC_ERROR;
    }

    // DB Compare and save;Need Transaction;
    vector<TVlanRangeItem> &vecCalcRange = cVlanRange.GetVlanRange();
    vector<TVlanRangeItem>::iterator itrCalcRange = vecCalcRange.begin();
    for(; itrCalcRange != vecCalcRange.end(); ++itrCalcRange)
    {            
        int32 bFind = 0;
        for(itrDBRange = outVInfo.begin(); itrDBRange != outVInfo.end(); ++itrDBRange)
        {
            TVlanRangeItem tIPRange;
            tIPRange.dwVlanIDStart = itrDBRange->GetVlanBegin();
            tIPRange.dwVlanIDEnd = itrDBRange->GetVlanEnd();
            if(tIPRange.dwVlanIDStart == itrCalcRange->dwVlanIDStart &&
               tIPRange.dwVlanIDEnd == itrCalcRange->dwVlanIDEnd)
            {
                bFind = 1;
                outVInfo.erase(itrDBRange);
                break;
            }
        }
        
        if(0 == bFind)
        {                
            // Add To DB
            cDbPgRange.SetPgUuid(cMsg.m_strPGUuid.c_str());
            cDbPgRange.SetVlanBegin(itrCalcRange->dwVlanIDStart);
            cDbPgRange.SetVlanEnd(itrCalcRange->dwVlanIDEnd);    
            nRet = m_pVlanRange->Add(cDbPgRange);
            if(VNET_DB_SUCCESS != nRet)
            {
                VNET_LOG(VNET_LOG_ERROR, 
                    "CPortGroupMgr::AddPGTrunkVlanRange: DB Add PG's(UUID: %s) vlan range[%d-%d] failed, nRet = %d.\n",
                    cMsg.m_strPGUuid.c_str(), cMsg.m_nVlanBegin, cMsg.m_nVlanEnd, nRet);
                VNetFormatString(strRetInfo, "Fail to add PortGroup(UUID:%s)'s vlanrange[%d-%d] to database.", 
                    cMsg.m_strPGUuid.c_str(), cMsg.m_nVlanBegin, cMsg.m_nVlanEnd);
                return ERROR_PG_VLANRANGE_OPER_DB_FAIL;
            }              
        }
    }
    // delete invalid record from db;
    for(itrDBRange = outVInfo.begin(); itrDBRange != outVInfo.end(); ++itrDBRange)
    {
        nRet = m_pVlanRange->Del(*itrDBRange);
        if(VNET_DB_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CPortGroupMgr::AddPGTrunkVlanRange: DB Merge PG's(UUID: %s) vlan range[%d-%d] failed, nRet = %d.\n",
                cMsg.m_strPGUuid.c_str(),
                cMsg.m_nVlanBegin,
                cMsg.m_nVlanEnd,
                nRet);
            VNetFormatString(strRetInfo, "Fail to merge PortGroup(UUID:%s)'s vlanrange[%d-%d] in database.", 
                cMsg.m_strPGUuid.c_str(), cMsg.m_nVlanBegin, cMsg.m_nVlanEnd);
            return ERROR_PG_VLANRANGE_OPER_DB_FAIL;
        }             
    }
        
    return PORTGROUP_OPER_SUCCESS;
}

/*******************************************************************************
* 函数名称： DelPGTrunkVlanRange
* 功能描述： 删除端口组trunk vlan range
* 访问的表： CDbPgTrunkVlanRange
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* ------------------------------------------------------------------------------
* 2013/3          V1.0       zhongchsh     创 建
*******************************************************************************/
int32 CPortGroupMgr::DelPGTrunkVlanRange(const CPGTrunkVlanRangeMsg &cMsg, string &strRetInfo)
{
    int32 nRet = PORTGROUP_OPER_SUCCESS;
    
    nRet = m_pDbPG->IsUsing(cMsg.m_strPGUuid.c_str());
    if(VNET_DB_SUCCESS != nRet)
    {
        if(VNET_DB_IS_RESOURCE_USING(nRet))
        {
            VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::DelPGTrunkVlanRange: PG is using, ret = %d\n", nRet);
            VNetFormatString(strRetInfo, "PortGroup(UUID:%s) is inusing, don't delete vlanrange.", cMsg.m_strPGUuid.c_str());
            return ERROR_PORTGROUP_IS_INUSING;
        }
        else
        {
            VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::DelPGTrunkVlanRange: call IsUsing failed, ret = %d\n", nRet);
            strRetInfo = "Fail to database check for PortGroup is using.";
            return ERROR_PORTGROUP_OPER_DB_FAIL;
        }
    }    
    
    if(FALSE == IsValidVlanRange(cMsg.m_nVlanBegin, cMsg.m_nVlanEnd))
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::DelPGTrunkVlanRange: vlan range[%d, %d] is invalid.\n",
                  cMsg.m_nVlanBegin, cMsg.m_nVlanEnd);   
        VNetFormatString(strRetInfo, "The trunk vlanrange[%d-%d] is invalid.", 
            cMsg.m_nVlanBegin, cMsg.m_nVlanEnd);
        return ERROR_PG_VLANRANGE_INVLAID;
    }
    
    vector<CDbPgTrunkVlanRange> outVInfo;
    nRet = m_pVlanRange->Query(cMsg.m_strPGUuid.c_str(), outVInfo);
    if(VNET_DB_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, 
            "CPortGroupMgr::DelPGTrunkVlanRange: DB Query PG's(UUID: %s) vlan range failed, ret = %d.\n",
            cMsg.m_strPGUuid.c_str(), nRet);
        VNetFormatString(strRetInfo, "Fail to query PortGroup(UUID:%s)'s vlanrange in database.", cMsg.m_strPGUuid.c_str());
        return ERROR_PG_VLANRANGE_OPER_DB_FAIL;
    }      
    
    CVlanRange cVlanRange;
    CDbPgTrunkVlanRange cDbPgRange;
    vector<CDbPgTrunkVlanRange>::iterator itrDBRange;
    if(outVInfo.empty())
    {
        VNET_LOG(VNET_LOG_ERROR, 
            "CPortGroupMgr::DelPGTrunkVlanRange: PG's(UUID: %s) vlan range(%d-%d) is not exist.\n",
            cMsg.m_strPGUuid.c_str(),
            cMsg.m_nVlanBegin,
            cMsg.m_nVlanEnd);
        VNetFormatString(strRetInfo, "PortGroup(UUID:%s)'s vlanrange is not exist.", cMsg.m_strPGUuid.c_str());
        return ERROR_PG_VLANRANGE_NOT_EXIST;
    }

    for(itrDBRange = outVInfo.begin(); itrDBRange != outVInfo.end(); ++itrDBRange)
    {
        TVlanRangeItem tItem;
        tItem.dwVlanIDStart = itrDBRange->GetVlanBegin();
        tItem.dwVlanIDEnd = itrDBRange->GetVlanEnd();
        cVlanRange.AddValidRange(tItem);
    }
     
    // range del;
    TVlanRangeItem tNewItem;
    tNewItem.dwVlanIDStart = cMsg.m_nVlanBegin;
    tNewItem.dwVlanIDEnd = cMsg.m_nVlanEnd;
    if(0 != cVlanRange.DelRange(tNewItem))
    {            
        VNET_LOG(VNET_LOG_ERROR, 
            "CPortGroupMgr::DelPGTrunkVlanRange: vlan range[%d-%d] delete failed.\n",
            cMsg.m_nVlanBegin, 
            cMsg.m_nVlanEnd);
        VNetFormatString(strRetInfo, "Combining the calculation of vlanrange[%d-%d] failed.\n",
            cMsg.m_nVlanBegin,
            cMsg.m_nVlanEnd);
        return ERROR_PG_VLANRANGE_CALC_ERROR;
    }

    // DB Compare and save;Need Transaction;
    vector<TVlanRangeItem> &vecCalcRange = cVlanRange.GetVlanRange();
    vector<TVlanRangeItem>::iterator itrCalcRange = vecCalcRange.begin();
    for(; itrCalcRange != vecCalcRange.end(); ++itrCalcRange)
    {            
        int32 bFind = 0;
        for(itrDBRange = outVInfo.begin(); itrDBRange != outVInfo.end(); ++itrDBRange)
        {
            TVlanRangeItem tIPRange;
            tIPRange.dwVlanIDStart = itrDBRange->GetVlanBegin();
            tIPRange.dwVlanIDEnd = itrDBRange->GetVlanEnd();
            if(tIPRange.dwVlanIDStart == itrCalcRange->dwVlanIDStart &&
               tIPRange.dwVlanIDEnd == itrCalcRange->dwVlanIDEnd)
            {
                bFind = 1;
                outVInfo.erase(itrDBRange);
                break;
            }
        }
            
        if(0 == bFind)
        {                
            // Add To DB
            cDbPgRange.SetPgUuid(cMsg.m_strPGUuid.c_str());
            cDbPgRange.SetVlanBegin(itrCalcRange->dwVlanIDStart);
            cDbPgRange.SetVlanEnd(itrCalcRange->dwVlanIDEnd);    
            nRet = m_pVlanRange->Add(cDbPgRange);
            if(VNET_DB_SUCCESS != nRet)
            {
                VNET_LOG(VNET_LOG_ERROR, 
                    "CPortGroupMgr::DelPGTrunkVlanRange: DB Add PG's(UUID: %s) vlan range[%d-%d] failed, nRet = %d.\n",
                    cMsg.m_strPGUuid.c_str(), cMsg.m_nVlanBegin, cMsg.m_nVlanEnd, nRet);
                VNetFormatString(strRetInfo, "Fail to delete PortGroup(UUID:%s)'s vlanrange[%d-%d] in database.", 
                cMsg.m_strPGUuid.c_str(), cMsg.m_nVlanBegin, cMsg.m_nVlanEnd);
                return ERROR_PG_VLANRANGE_OPER_DB_FAIL;
            }             
        }
    }
    // delete invalid record from db;
    for(itrDBRange = outVInfo.begin(); itrDBRange != outVInfo.end(); ++itrDBRange)
    {
        nRet = m_pVlanRange->Del(*itrDBRange);
        if(VNET_DB_SUCCESS != nRet)
        {
            VNET_LOG(VNET_LOG_ERROR, 
                "CPortGroupMgr::DelPGTrunkVlanRange: DB Merge PG's(UUID: %s) vlan range[%d-%d] failed, nRet = %d.\n",
                cMsg.m_strPGUuid.c_str(),
                cMsg.m_nVlanBegin,
                cMsg.m_nVlanEnd,
                nRet);
            VNetFormatString(strRetInfo, "Fail to merge PortGroup(UUID:%s)'s vlanrange[%d-%d] in database.", 
                cMsg.m_strPGUuid.c_str(), cMsg.m_nVlanBegin, cMsg.m_nVlanEnd);
            return ERROR_PG_VLANRANGE_OPER_DB_FAIL;
        }             
    }
    
    return PORTGROUP_OPER_SUCCESS;
}

/*******************************************************************************
* 函数名称： GetPGTrunkVlanRange
* 功能描述： 根据PG uuid找到TrunkVlanRange，作为接口使用
* 访问的表： CDbPgTrunkVlanRange
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* ------------------------------------------------------------------------------
* 2013/3          V1.0       zhongchsh     创 建
*******************************************************************************/
int32 CPortGroupMgr::GetPGTrunkVlanRange(const string &strPGUuid, vector<CTrunkVlanRange> &vecCTrunkVlanRange)
{
    vector<CDbPgTrunkVlanRange> vecDbTrunkVRange;
    int32 nRet = m_pVlanRange->Query(strPGUuid.c_str(), vecDbTrunkVRange);
    if(VNET_DB_SUCCESS == nRet)
    {
        vector<CDbPgTrunkVlanRange>::iterator iter = vecDbTrunkVRange.begin();
        for( ; iter != vecDbTrunkVRange.end(); ++iter)
        {
            CTrunkVlanRange tCTrunkVRange; 
            tCTrunkVRange.m_nVlanBegin = iter->GetVlanBegin();
            tCTrunkVRange.m_nVlanEnd   = iter->GetVlanEnd();
            vecCTrunkVlanRange.push_back(tCTrunkVRange);
        }
    }
    return nRet;
}

int32 CPortGroupMgr::IsMaxTrunkVlanRange(const string &strPGuuid)
{   
    vector<CDbPgTrunkVlanRange> vecDbTrunkVRange;
    int32 nRet = m_pVlanRange->Query(strPGuuid.c_str(), vecDbTrunkVRange);
    if(VNET_DB_SUCCESS != nRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::IsMaxTrunkVlanRange: Query CDbPgTrunkVlanRange failed, ret = %d\n", nRet);
        return -1;
    }

    if(vecDbTrunkVRange.empty() || (1 != vecDbTrunkVRange.size()))
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::IsMaxTrunkVlanRange: vecDbTrunkVRange.size() out of one.\n");
        return 1;
    }

    if((vecDbTrunkVRange.front().GetVlanBegin() != VNET_MIN_VLAN_ID) &&(vecDbTrunkVRange.front().GetVlanEnd() != VNET_MAX_VLAN_ID))
    {
        VNET_LOG(VNET_LOG_ERROR, "CPortGroupMgr::IsMaxTrunkVlanRange: vlan range is not the maximum.\n");
        return 1;
    }
        
    return 0;
}


/*******************************************************************************
* 函数名称： IsValidVlanRange
* 功能描述： 检查vlan range范围是否正确
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
BOOL CPortGroupMgr::IsValidVlanRange(int32 nVlanBegin, int32 nVlanEnd)
{
    if((nVlanBegin < VNET_MIN_VLAN_ID) || (nVlanBegin > VNET_MAX_VLAN_ID) 
        || (nVlanEnd < VNET_MIN_VLAN_ID) || (nVlanEnd > VNET_MAX_VLAN_ID)
        || (nVlanBegin > nVlanEnd))
    {
        return FALSE;            
    }
    return TRUE;
}

/*******************************************************************************
* 函数名称： IsValidSegmentRange
* 功能描述： 检查segment range范围是否正确
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* ------------------------------------------------------------------------------
* 2013/3          V1.0                    创 建
*******************************************************************************/
BOOL CPortGroupMgr::IsValidSegmentRange(int32 nSegmentBegin, int32 nSegmentEnd)
{
    if((nSegmentBegin < VNET_MIN_SEGMENT_ID) || (nSegmentBegin > VNET_MAX_SEGMENT_ID) 
        || (nSegmentEnd < VNET_MIN_SEGMENT_ID) || (nSegmentEnd > VNET_MAX_SEGMENT_ID)
        || (nSegmentBegin > nSegmentEnd))
    {
        return FALSE;            
    }
    return TRUE;
}


void CPortGroupMgr::DbgShowPGVlanRange()
{
    vector<CDbPortGroupSummary> outVInfo;
    int32 nRet = m_pDbPG->QuerySummary(outVInfo);
    if(VNET_DB_SUCCESS == nRet)
    {
        vector<CDbPortGroupSummary>::iterator iter;
        for(iter = outVInfo.begin(); iter != outVInfo.end(); ++iter)
        {
            cout << "--------------------------------------------------------" << endl;
            vector<CDbPgTrunkVlanRange> vecVlanRange;
            nRet = m_pVlanRange->Query(iter->GetUuid().c_str(), vecVlanRange);
            if(VNET_DB_SUCCESS == nRet)
            {
                cout << "PGUuid    : " << iter->GetUuid()<< endl; 
                vector<CDbPgTrunkVlanRange>::iterator it =vecVlanRange.begin();
                for( ; it != vecVlanRange.end(); ++it)
                {
                    cout << "VlanBegin : " << it->GetVlanBegin()<< endl;   
                    cout << "VlanEnd   : " << it->GetVlanEnd()<< endl;       
                }
            }
            cout << "--------------------------------------------------------" << endl;
        }
    }
}

void VNetDbgTestPGOper(const char* cstrName, 
                       const char* cstrUUID, 
                       const char* cstrNPUuid,
                       int32 nType, 
                       int32 nMTU,
                       int32 nSwPortMode,
                       int32 nVlan,
                       int32 nOper)
{
    if(NULL == cstrName || 
       NULL == cstrUUID)
    {
        return ;
    }

    if((nOper <= EN_PORT_GROUP_OPER_BEGIN) || (nOper >= EN_PORT_GROUP_OPER_END))
    {
        cout << "Operation Code: Add[" << EN_ADD_PORT_GROUP
             << "], Del[" << EN_DEL_PORT_GROUP
             << "], Mod[" << EN_SET_PORT_GROUP
             << "]." <<endl;
        return ;
    }

    MessageUnit tempmu(TempUnitName("TestPGOper"));
    tempmu.Register();
    
    CPortGroupMsg cMsg;
    string strTmp;
    strTmp.assign(cstrName);
    cMsg.m_strName = strTmp;
    strTmp.clear();
    strTmp.assign(cstrUUID);
    cMsg.m_strUUID = strTmp;
    strTmp.clear();
    strTmp.assign(cstrNPUuid);
    if(!strTmp.empty())
    {
        cMsg.m_strNetPlaneUUID = strTmp;
        cMsg.m_nIsCfgNetPlane  = 1;
    }
    strTmp.clear();
    cMsg.m_nPGType = nType;
    cMsg.m_nMTU = nMTU;
    cMsg.m_nSwitchPortMode = nSwPortMode;
    cMsg.m_nOper = nOper;
    switch(cMsg.m_nSwitchPortMode)
    {
        case EN_DB_SWITCHPORTMODE_TYPE_TRUNK:
        {
            cMsg.m_nTrunkNativeVlan = nVlan; 
            break;
        }
        case EN_DB_SWITCHPORTMODE_TYPE_ACCESS:
        {
            if(cMsg.m_nPGType == EN_DB_PORTGROUP_TYPE_VM_PRIVATE)
            {
                cMsg.m_nAccessIsolateNum = nVlan;
            }
            else
            {
                cMsg.m_nAccessVlanNum = nVlan; 
            }
            break;
        }
        default:
        {
            cout<<" error input mode!!"<< endl;
        }
    }
    
    MessageOption option(MU_VNM, EV_VNETLIB_PORT_GROUP_CFG, 0, 0);
    tempmu.Send(cMsg,option); 
    return ;
}

void VNetDbgSetPGPrint(BOOL bOpen)
{
    CPortGroupMgr *pMgr = CPortGroupMgr::GetInstance();
    if(pMgr)
    {
        pMgr->SetDbgPrintFlag(bOpen);
    }
}

void VNetDbgShowPortGroup()
{
    CPortGroupMgr *pMgr = CPortGroupMgr::GetInstance();
    if(NULL != pMgr)
    {
        pMgr->DbgShowPortGroup();
    }
}

void VNetDbgTestPGTrunkVlanRageOper(const char* cstrUUID, 
                              int32 nVlanBegin, int32 nVlanEnd, int32 nOper)
{
    if(NULL == cstrUUID)
    {
        return ;
    }

    if((nOper < EN_ADD_PG_TRUNK_VLANRANGE) || (nOper > EN_DEL_PG_TRUNK_VLANRANGE))
    {
        cout << "Operation Code: Add[" << EN_ADD_PG_TRUNK_VLANRANGE
             << "], Del[" << EN_DEL_PG_TRUNK_VLANRANGE
             << "]." <<endl;
        return ;
    }

    MessageUnit tempmu(TempUnitName("TestPGTrunkVlanRangeOper"));
    tempmu.Register();
    
    CPGTrunkVlanRangeMsg cMsg;
    string strTmp;
    strTmp.assign(cstrUUID);
    cMsg.m_strPGUuid = strTmp;
    strTmp.clear();
    cMsg.m_nVlanBegin = nVlanBegin;
    cMsg.m_nVlanEnd   = nVlanEnd;
    cMsg.m_nOper      = nOper;
    
    MessageOption option(MU_VNM, EV_VNETLIB_PG_TRUNK_VLANRANGE_CFG, 0, 0);
    tempmu.Send(cMsg,option); 
    return ;
}

void VNetDbgShowPGTrunkVlanRange()
{
    CPortGroupMgr *pMgr = CPortGroupMgr::GetInstance();
    if(NULL != pMgr)
    {
        pMgr->DbgShowPGVlanRange();
    }
}

void VNetDbgQuantumPort(const int32 nOper, const char* cLnUuid, const char* cQNUuid, const int64 vmid)
{
    CVNetVmMem cVnetVmMem;
    vector<CVNetVnicMem> vecVnicMem;
    CVNetVnicMem VnicMem;
    string strtmp;

    strtmp = cLnUuid;
    VnicMem.SetLogicNetworkUuid(strtmp);

    strtmp = cQNUuid;
    VnicMem.SetQuantumNetworkUuid(strtmp);

    VnicMem.SetVmId(vmid);
    VnicMem.SetNicIndex(157906);
    

    vecVnicMem.push_back(VnicMem);

    cVnetVmMem.SetVecVnicMem(vecVnicMem);

    CPortGroupMgr *pMgr = CPortGroupMgr::GetInstance();
    if(pMgr)
    {
        if(0 == nOper)
        {
            return;
        }
        else
        {
            pMgr->CreateQuantumPort(cVnetVmMem);
            cVnetVmMem.Print();
        }
    }
}

DEFINE_DEBUG_FUNC(VNetDbgSetPGPrint);
DEFINE_DEBUG_FUNC(VNetDbgTestPGOper);
DEFINE_DEBUG_FUNC(VNetDbgShowPortGroup);
DEFINE_DEBUG_FUNC(VNetDbgTestPGTrunkVlanRageOper);
DEFINE_DEBUG_FUNC(VNetDbgShowPGTrunkVlanRange);
DEFINE_DEBUG_FUNC(VNetDbgQuantumPort);
}


