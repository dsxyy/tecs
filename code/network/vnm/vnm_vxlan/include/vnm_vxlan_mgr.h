/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：vnm_vxlan_mgr.h
* 文件标识： 
* 内容摘要：vxlan管理类CVxlanMgr定义
* 当前版本：1.0
* 作    者：zhaojin 
* 完成日期： 
*******************************************************************************/
#ifndef VXLAN_MGR_H
#define VXLAN_MGR_H

#include "vnet_db_portgroup.h"
#include "vnet_db_netplane_segmentpool.h"
#include "vnet_db_mgr.h"
#include "vnet_msg.h"


namespace zte_tecs
{

class CVxlanMgr
{
public:
    CVxlanMgr(void);
    virtual ~CVxlanMgr(void);
    static CVxlanMgr* GetInstance(void)
    {
        if(NULL == m_pInstance)
        {
            m_pInstance = new CVxlanMgr();
        }
        return m_pInstance;
    }

    int32  Init(MessageUnit *mu);
    void   MessageEntry(const MessageFrame &message);
    void   SetDbgPrintFlag(BOOL bOpen){ m_bOpenDbg = bOpen;}    
    int32  SendMcGroupInfo(const string &strVnaUuid);
    int32  RcvMcGroupInfoReq(const MessageFrame &message);
    int32  AllocVtepIP(const string strNetplaneId, string &strIP, string &strMask);
    int32  FreeVtepIP(const string strNetplaneId, const string strIP, const string strMask);
    string GetNetplaneIdByVtep(const string strVtepName);
    string GetVtepNameBySwitchName(const string strSwitchName);
    string GetVtepNameBySwitchUuid(const string strSwitchUuid);    
    string GetSwitchNameByVtepName(const string strVtepName);
    int32  GetMcGroupInfo(const string &strVNAUuid, CVxlanMcGroupTable &VxlanMcGroupTable);
    void   GetRemoteVteps(vector<CDbVnaNpSegmentId> &vecDbMcVteps, vector<CDbVnaNpSegmentId> vecDbLocalVtep);
    int32  FillVxlanMcGroup(CVxlanMcGroup &VxlanMcGroup, vector<CDbVnaNpSegmentId> vecDbLocalVtep, vector<CDbVnaNpSegmentId> vecDbVteps);
    
private:
    BOOL                       m_bOpenDbg;
    MessageUnit                *m_pMU;
    CVNetDbMgr                 *m_pDbMgr;
    static CVxlanMgr           *m_pInstance;
    
private:
    DISALLOW_COPY_AND_ASSIGN(CVxlanMgr);    

};

}

#endif

