/******************************************************************************
* Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：vnet_vm_mgr.h
* 文件标识：
* 内容摘要：CVNetVmMgr类的定义文件
* 当前版本：1.0
* 作    者：
* 完成日期：2013年2月22日
*
* 修改记录1：
*     修改日期：2013/2/22
*     版 本 号：V1.0
*     修 改 人：lverchun
*     修改内容：创建
******************************************************************************/
#if !defined(VNM_VM_MGR_EXT_INCLUDE_H__)
#define VNM_VM_MGR_EXT_INCLUDE_H__

namespace zte_tecs
{

typedef struct tagVnetTimerInfo
{
    TIMER_ID tTimerID;          //定时器ID
    string strVNAUuid;
    int32 nTimerOutCnt;         //超时次数    
}
T_VSISynTimerInfo;

class CVNetVmMgrExt
{
public:
    explicit CVNetVmMgrExt();
    virtual ~CVNetVmMgrExt();
    
    int32 Init(MessageUnit *mu);
    void MessageEntry(const MessageFrame &message);
    void DbgShowData(void);
    void SetDbgFlag(BOOL bOpen){m_bOpenDbgInf = bOpen;}
    static CVNetVmMgrExt *GetInstance()
    {
        if(NULL == s_pInstance)
        {
            s_pInstance = new CVNetVmMgrExt();
        }

        return s_pInstance;
    };
    
private:    
    void SelectHCListToMigrate(const MessageFrame& message);
    void VmMigrateBegin(const MessageFrame& message);
    void VmMigrateEnd(const MessageFrame& message);
    int32 SelectMigrateHC(CVNetSelectHCListMsg &cMsg, vector<string> &vecMigrateHC);
    int32 GetMigrateShareVNA(CVNetSelectHCListMsg &cMsg, set<string> &setVNAUuid);
    void VMVSIInfoReq(const MessageFrame& message);
    void VMVnicInfoReq(const MessageFrame& message);
    int32 AddVMVSISynTask(const string & strVNAUuid);
    void VMVSISynTimerProc(const MessageFrame& message);
    void SynVMVSIInfoToVNA(const string & strVNAUuid);
    void VMVSIInfoSynAck(const MessageFrame& message);
    int32 GetVSIDetailInfo(int64 nVmID, CAddVMVSIInfoMsg &cVsiMsg);

private:  
    BOOL m_bOpenDbgInf;
    MessageUnit *m_pMU;
    vector<T_VSISynTimerInfo> m_vecVMVSISynTimer;  //扫描新部署VM
    CVNetVmDB * m_pVNetVmDB;
    CVNetVnicDB * m_pVNetVnicDB;
    CVSIProfileMgr * m_pVSIProfileMgr;
    CSwitchMgr * m_pSwitchMgr;
    CVNAManager * m_pVNAMgr;
    
private:
    static CVNetVmMgrExt *s_pInstance;

    DISALLOW_COPY_AND_ASSIGN(CVNetVmMgrExt);
};

int32 VnicCfgTrans2Mem(const int64 nVmID, const int64 nProjectID,  vector<CVNetVnicConfig> &vecVnicCfg, CWatchDogInfo   &cWatchDogInfo, CVNetVmMem &cVnetVmMem);
void VnicMemTrans2Cfg( vector<CVNetVnicMem> &vecVmVnicMem, vector<CVNetVnicConfig> &vecVnicCfg);

}// namespace zte_tecs

#endif // VNET_VM_MGR_INCLUDE_H__

