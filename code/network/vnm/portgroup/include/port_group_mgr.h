/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：port_group_mgr.h
* 文件标识： 
* 内容摘要：端口组管理类CPorgGroupMgr定义
* 当前版本：1.0
* 作    者：zhongchsh 
* 完成日期： 
*******************************************************************************/
#ifndef PORT_GROUP_MGR_H
#define PORT_GROUP_MGR_H

#include "vnet_db_portgroup.h"
#include "vnet_db_mgr.h"
#include "vnet_msg.h"


namespace zte_tecs
{

class CPortGroupMgr
{
public:
    explicit CPortGroupMgr();
    virtual ~CPortGroupMgr();
    static CPortGroupMgr* GetInstance()
    {
        if(NULL == m_pInstance)
        {
            m_pInstance = new CPortGroupMgr();
        }
        return m_pInstance;
    }

    int32  Init(MessageUnit *mu);
    void   MessageEntry(const MessageFrame &message);
    int32  QueryPGByName(const string &strPGName, string &strPGUuid);
    int32  IsUplinkPG(const string &strPGUuid);
    int32  IsNeedVlanMap(const string &strPGUuid, int32 &nIsolateNo);
    int32  IsNeedSegmentMap(const string &strPGUuid, int32 &nIsolateNo);    
    int32  GetPGDetail(CPortGroup &cPGInfo);
    int32  GetVMPGDetail(vector<CVNetVnicDetail> &vecVnicDetail);
    int32  GetPGNetplane(const string &strPGUuid, string &strNPUuid);
    int32  GetPGIsolateType(const string &strPGUuid, int32 &nIsoType);
    int32  GetPGTrunkNativeVlan(const string &strPGUuid, int32 &nTrunkNativeVlan);
    int32  GetPGAccessVlan(int64 nProjID, const string &strPGUuid, int32 &nAccessVlan);
    int32  GetPGSegmentId(int64 nProjID, const string &strPGUuid, int32 &nSegmentId);
    BOOL   IsPortGroupSdn(const string &strPGUuid);
    int32  RESTfulCreatePort(const string &cstrQuantumSvrIP, const string &cstrPortName, const string &cstrNetworkUuid, const string &cstrTenantid, string &strPortUuid);
    int32  RESTfulDeletePort(const string &cstrQuantumSvrIP, const string &cstrPortUuid);
    int32  CreateQuantumPort(CVNetVmMem &cVnetVmMem);
    int32  DeleteQuantumPort(CVNetVmMem &cVnetVmMem);

    int32  GetPGTrunkVlanRange(const string &strPGUuid, vector<CTrunkVlanRange> &vecCTrunkVlanRange);
    int32  IsMaxTrunkVlanRange(const string &strPGuuid);
    void   SetDbgPrintFlag(BOOL bOpen){ m_bOpenDbg = bOpen;}
    void   DbgShowPortGroup();
    void   DbgShowPGVlanRange();
    
private:    
    int32  ProcPortGroupMsg(const MessageFrame &message);
    int32  ProcPGTrunkVlanRangeMsg(const MessageFrame &message);
    int32  QueryPGByUUID(const string &strPGUuid, string &strPGName);
    int32  AddPortGroup(CPortGroupMsg &cMsg, string &strRetInfo);
    int32  DelPortGroup(const CPortGroupMsg &cMsg, string &strRetInfo);
    int32  SetPortGroup(CPortGroupMsg &cMsg, string &strRetInfo);
    int32  SetPGNativeVlan(CPortGroupMsg &cMsg, string &strRetInfo);
    int32  AddPGTrunkVlanRange(const CPGTrunkVlanRangeMsg &cMsg, string &strRetInfo);
    int32  DelPGTrunkVlanRange(const CPGTrunkVlanRangeMsg &cMsg, string &strRetInfo);
    int32  CheckPGParam(CPortGroupMsg &cMsg, string &strRetInfo);
    BOOL   CmpPGData(CDbPortGroup &cDbPG, const CPortGroupMsg &cMsg);
    BOOL   IsValidVlanRange(int32 nVlanBegin, int32 nVlanEnd);
    BOOL   IsValidSegmentRange(int32 nSegmentBegin, int32 nSegmentEnd);    
    int32  ProcPortGroupQuery(const MessageFrame &message);
    
private:
    BOOL                       m_bOpenDbg;
    MessageUnit                *m_pMU;
    CVNetDbMgr                 *m_pDbMgr;
    CDBOperatePortGroup        *m_pDbPG;
    CDBOperatePgTrunkVlanRange *m_pVlanRange;
    static CPortGroupMgr       *m_pInstance;
    
private:
    DISALLOW_COPY_AND_ASSIGN(CPortGroupMgr);    

};

}

#endif

