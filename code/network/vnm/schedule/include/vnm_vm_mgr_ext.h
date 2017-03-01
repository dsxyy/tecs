/******************************************************************************
* Copyright (c) 2012������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�vnet_vm_mgr.h
* �ļ���ʶ��
* ����ժҪ��CVNetVmMgr��Ķ����ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�
* ������ڣ�2013��2��22��
*
* �޸ļ�¼1��
*     �޸����ڣ�2013/2/22
*     �� �� �ţ�V1.0
*     �� �� �ˣ�lverchun
*     �޸����ݣ�����
******************************************************************************/
#if !defined(VNM_VM_MGR_EXT_INCLUDE_H__)
#define VNM_VM_MGR_EXT_INCLUDE_H__

namespace zte_tecs
{

typedef struct tagVnetTimerInfo
{
    TIMER_ID tTimerID;          //��ʱ��ID
    string strVNAUuid;
    int32 nTimerOutCnt;         //��ʱ����    
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
    vector<T_VSISynTimerInfo> m_vecVMVSISynTimer;  //ɨ���²���VM
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

