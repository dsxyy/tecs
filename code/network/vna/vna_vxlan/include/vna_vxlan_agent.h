/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：vna_vxlan_agent.h
* 文件标识： 
* 内容摘要： 
* 当前版本：1.0
* 作    者： 
* 完成日期： 
*******************************************************************************/
#ifndef VNA_VXLAN_AGENT_H
#define VNA_VXLAN_AGENT_H

#include "vnet_msg.h"

namespace zte_tecs
{
class CVxlanAgent
{
public:
    CVxlanAgent();
    virtual ~CVxlanAgent();
    static CVxlanAgent *GetInstance()
    {
        if(NULL == m_pInstance)
        {    
            m_pInstance = new CVxlanAgent();
        }
        return m_pInstance;
    }
    
    int32  Init(MessageUnit *mu);
    void   MessageEntry(const MessageFrame &message); 
    void   SetDbgPrintFlag(BOOL bOpen){ m_bOpenDbg = bOpen;}
    int32  RcvMcGroupInfo(const MessageFrame &message);
    void   DoDeleteMcGroup(CVxlanMcGroupTable &NewVxlanMcGroupTable);
    void   DoUpdateMcGroup(CVxlanMcGroupTable &NewVxlanMcGroupTable);
    void   DoAddMcGroup(CVxlanMcGroupTable &NewVxlanMcGroupTable);
    int32  UpdateVxlanMcGroup(CVxlanMcGroupTable &NewVxlanMcGroupTable);
    int32  SendMcGroupInfoReq(void);
    int32  CreateVxlanBridge(string strVxlanBridge);
    int32  DestroyVxlanBridge(string strVxlanBridge);
    BOOL   IsVxlanBridgeExist(string strVxlanBridge);
    string GetVxlanBridgeName(string strNetplaneNickName, string strSegmentId);
    int32  PrepareDeployVxlanVm(CVMVSIInfo &vmVSIInfo);
    int32  CreateVxlanTunnel(string strVxlanBridge);
    string GetSegmentId(string strVxlanBridge);
    string GetNetplaneNickName(string strVxlanBridge);    
    string GetVxlanPortName(string strNetplaneId, string strSegmentId, string strRemoteIP);
    int32  ConvertNum2string(unsigned int nNum, string &s);
    string GetVtepNameBySwitchName(const string strSwitchName);
    int32  MonitorVxlanTunnel(string &strVxlanBridge);
    void   ShowMcGroupInfo(void);
    int32  UpdateVxlanTunnel(CVxlanMcGroup &VxlanMcGroup);
    int32  UpdateVxlanTunnel(map<CVxlanMcGroup, CVxlanMcGroup> &mapUpdate);
    BOOL   IsMemberOfGroup(string strIp, vector<string>& vecVtepIp);
    
private:
    BOOL                   m_bOpenDbg;
    MessageUnit           *m_pMU;
    static CVxlanAgent    *m_pInstance;
    CVxlanMcGroupTable     m_VxlanMcGroupTable;   
    
private:
    DISALLOW_COPY_AND_ASSIGN(CVxlanAgent);
};
}

#endif

