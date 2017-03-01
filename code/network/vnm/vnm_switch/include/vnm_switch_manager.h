/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：vnm_switch_manager.h
* 文件标识： 
* 内容摘要：vnm switch管理类
* 当前版本：1.0
* 作    者： 
* 完成日期： 
*******************************************************************************/
#ifndef VNM_SWITCH_MANAGER_H
#define VNM_SWITCH_MANAGER_H

#include "vnm_switch.h"
#include "vnet_db_mgr.h"
#include "port_group_mgr.h"
#include "wildcast_mgr.h"


namespace zte_tecs
{

class CSwitchMgr : public MessageHandler
{

public:
    CSwitchMgr();
    virtual ~CSwitchMgr();
    static CSwitchMgr* GetInstance()
    {
        if(NULL == m_pInstance)
        {
            m_pInstance = new CSwitchMgr();
        }
        return m_pInstance;
    }
 
public:    
    int32  Init(MessageUnit *mu);
    void   MessageEntry(const MessageFrame& message);
    int32  AllocDVSRes(CVNetVmMem &cVnetVmMem);
    int32  FreeDVSRes(CVNetVmMem &cVnetVmMem);
    int32  GetDVSDetail(const string &strVNAUuid, vector<CVNetVnicDetail> &vecVnicDetail);
    int32  UpdateVNASwitch(const string &strVNAUuid);
    void   UpdateVNMSwitch(const string &strVNAUuid, vector<CSwitchReportInfo> &vecReport);
    int32  UpdateSwitchToOffline(const string &strVNAUuid);
    int32  CheckWildCast(CSwitchCfgMsgToVNA &cSwitchWCMsg);
    int32  DoSwitchWildcast(CSwitchCfgMsgToVNA &cSwitchWCMsg);
    int32  DoKernelDVSCfg(const string &strVNAUuid, const string &strDVSName, const vector<string> &vecPortName,
                                                  const string &strBondName, const int32 &nBondMode);
    int32  GetSwitchUUID(const string &strVNAUuid, const string &strSwName, string &strSwUuid);
    int32  GetSwitchPort(const string &strVNAUuid, const string &strSwName, string &strPortName);
    int32  GetSwitchName(const string &strSwUuid, string &strSwName);
    int32  GetSwitchOnlineVNA(const string &strDVSUuid, set<string> &setVNAUuid);
    int32  GetSwitchPGInfo(const string &strDVSName, CPortGroup &cPGInfo);
    void   SetDbgPrintFlag(BOOL bOpen){ m_bOpenDbg = bOpen;}
    void   DbgShowSwitch();
    
private:    
    CSwitch * GetSwitchInstance(const int32 nSwitchType);
    int32  ProcSwitchReqMsg(const MessageFrame& message);
    int32  ProcSwitchPortMsg(const MessageFrame& message);
    int32  ProcSwitchAckMsg(const MessageFrame& message);
    int32  ProcUpdateVNA();
    int32  ProcUpdateSwitch();
    int32  DoSwitchAddReq(CSwitchCfgMsg &cMsg, string &strRetInfo);
    int32  DoSwitchDelReq(CSwitchCfgMsg &cMsg, const MID &mid, string &strRetInfo);
    int32  DoSwitchDelAck(CSwitchCfgMsgToVNA &cMsg);
    int32  DoSwitchModifyReq(CSwitchCfgMsg &cMsg, string &strRetInfo);
    int32  DoSwitchModifyAck(CSwitchCfgMsgToVNA &cMsg);
    int32  DoSwitchPortAddReq(CSwitchPortCfgMsg &cMsg, const MID &mid, string &strRetInfo);
    int32  DoSwitchPortAddAck(CSwitchCfgMsgToVNA &cMsg);
    int32  DoSwitchPortDelReq(CSwitchPortCfgMsg &cMsg, const MID &mid, string &strRetInfo);
    int32  DoSwitchPortDelAck(CSwitchCfgMsgToVNA &cMsg);
    
private:
    BOOL          m_bOpenDbg;
    CSwitch       *m_pSwitch; //暂时只考虑DVS
    CPortGroupMgr *m_pPGMgr;
    CWildcastMgr  *m_pWCMgr;
    MessageUnit   *m_pMU;
    static CSwitchMgr *m_pInstance;
    
private:
    DISALLOW_COPY_AND_ASSIGN(CSwitchMgr);
};

}

#endif

