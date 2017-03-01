/******************************************************************************
* Copyright (c) 2012������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�vna_vm_agent.h
* �ļ���ʶ��
* ����ժҪ��CVNetVmAgent��Ķ����ļ�
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
#if !defined(VNA_VM_AGENT_INCLUDE_H__)
#define VNA_VM_AGENT_INCLUDE_H__

namespace zte_tecs
{

class CVNetVmAgent
{
public:
    explicit CVNetVmAgent();
    virtual ~CVNetVmAgent();

public:
    int32 Init(MessageUnit *mu);
    void MessageEntry(const MessageFrame &message);
    void DbgShowData(void);
    void SetDbgFlag(int bDbgFlag);
    static CVNetVmAgent *GetInstance()
    {
        if(NULL == s_pInstance)
        {
            s_pInstance = new CVNetVmAgent();
        }

        return s_pInstance;
    };

    // startnetwork ��̨��ʱ���EVBЭ��״̬ 
    void CheckEvbStatus(CVNAVMVnicInfo* cVMVnicInfo);

 private:    
 
    int32 StartEvbProc(CVNAVMVnicInfo& cVMVnicInfo,int32 noNeedEvbTlv = FALSE);
    int32 StopEvbProc(CVNAVMVnicInfo& cVMVnicInfo);    
    int32 StartNetworkProc(CVNAVMVnicInfo& cVMVnicInfo);
    
    void StartNetwork(const MessageFrame& message);    
    void StopNetwork(const MessageFrame& message);
    void NotifyDomID(const MessageFrame& message);
    void AddVMVSIInfo(const MessageFrame& message);
    void DelVMVSIInfo(const MessageFrame& message);
    void RcvVsiReqAckFromVNM(const MessageFrame& message);
    void RcvAllVMVnicInfoFromHC(const MessageFrame& message);
    void RcvAllVMVSIInfoFromVNM(const MessageFrame& message);
    void RcvWdgOperAckMsgFromWdg(const MessageFrame& message);
    void RcvWdgTimerOperMsgFromHC(const MessageFrame& message);
    void RcvWdgTimerOperAckMsgFromWdg(const MessageFrame& message);
    void RcvVMRestartMsgFromWDG(const MessageFrame& message);
    void RcvVMRestartAckMsgFromHC(const MessageFrame& message);
    void SetExternalId(const int64 Vmid, const int Domid);

private:  
    BOOL m_bOpenDbgInf;
    MessageUnit *m_pMU;
    CVNetAgent *m_pVNetAgent;
    CSwitchAgent *m_pSwitchAgent;
    CEvbMgr *m_pEvbMgr;
    CVNAVnicPool *m_pVNAVnicPool;

private:
    static CVNetVmAgent *s_pInstance;

    DISALLOW_COPY_AND_ASSIGN(CVNetVmAgent);
};

}// namespace zte_tecs

#endif // VNET_VMNIC_AGENT_INCLUDE_H__

