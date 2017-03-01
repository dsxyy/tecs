/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：vna_switch_agent.h
* 文件标识： 
* 内容摘要： 
* 当前版本：1.0
* 作    者： 
* 完成日期： 
*******************************************************************************/
#ifndef VNA_SWITCH_AGENT_H
#define VNA_SWITCH_AGENT_H

namespace zte_tecs
{

class CSwitchAgent : public CNetDevMgr
{

public:
    CSwitchAgent();
    virtual ~CSwitchAgent();

    static CSwitchAgent *GetInstance()
    {
        if(NULL == m_pInstance)
        {    
            m_pInstance = new CSwitchAgent();
        }
        return m_pInstance;
    }
    
    int32 Init(MessageUnit *mu);
    void  MessageEntry(const MessageFrame &message); 
    void  DoSwitchReport(vector<CSwitchReportInfo> &vecReport);
    void  DoMonitor(CNetDevVisitor *dev_visit);
    void  DoMonitor(CBridgeVisitor *dev_visit);
    void  DbgShowSwitchDev();
    void  SetDbgPrintFlag(BOOL bOpen){ m_bOpenDbg = bOpen;}
    int32 AllocDVSRes(vector<CVNetVnicDetail> &vecVnicInfo);
    int32 FreeDVSRes(vector<CVNetVnicDetail> &vecVnicInfo);
    int32 GetEVBMode(const string &strDVSName, int32 &nEVBMode);

    CSwitchDev * GetSwitchDev(const string &strSwName);	
    
private:
    CSwitchDev * GetSwitchInstance(const int32 nSwType);
    //CSwitchDev * GetSwitchDev(const string &strSwName);
    int32 ProcSwitchCfg(const MessageFrame& message);
    int32 ProcSwitchPortCfg(const MessageFrame& message);
    int32 DoSwitchPortAdd(CSwitchCfgMsgToVNA &cMsg);
    int32 DoSwitchPortDel(CSwitchCfgMsgToVNA &cMsg);
    int32 DoSwitchDel(CSwitchCfgMsgToVNA &cMsg);
    int32 DoSwitchModify(CSwitchCfgMsgToVNA &cMsg);

private:
    BOOL                  m_bOpenDbg;
    list<CSwitchDev*>     m_lstSwitchDev;
    MessageUnit          *m_pMU;
    static CSwitchAgent *m_pInstance;

private:
    DISALLOW_COPY_AND_ASSIGN(CSwitchAgent);
};

}
#endif

