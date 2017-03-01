/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�vna_controler.h
* �ļ���ʶ��
* ����ժҪ��CVNAController��Ķ����ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�
* ������ڣ�2013��1��16��
*
* �޸ļ�¼1��
*     �޸����ڣ�2013/1/16
*     �� �� �ţ�V1.0
*     �� �� �ˣ�lvech
*     �޸����ݣ�����
******************************************************************************/
#if !defined(VNA_CONTROLLER_INCLUDE_H__)
#define VNA_CONTROLLER_INCLUDE_H__

#include "vna_vxlan_agent.h"

namespace zte_tecs
{

class CVNAController : public MessageHandler
{
public:
    explicit CVNAController();
    virtual ~CVNAController();
    STATUS Init()
    {
        return StartMu(MU_VNA_CONTROLLER);
    }
    void StartMonitor();
    void SetDbgFlag(BOOL bOpen){m_bOpenDbgInf = bOpen;}
    int32 DbgShowData(void);

public:
    static CVNAController *GetInstance()
    {
        if (NULL == s_pInstance)
        {
            s_pInstance = new CVNAController();
        }

        return s_pInstance;
    };

protected:
    STATUS Receive(const MessageFrame& message);
    void JoinGroup(const string& strMG);
    void ExitGroup(const string& strMG);
    void MessageEntry(const MessageFrame& message);
    int32 InitVNetCtl(void);
    
private:
    STATUS StartMu(const string& name);

private:
    static CVNAController *s_pInstance;
    
    MessageUnit *m_pMU;
    pthread_mutex_t m_mutex;
    BOOL m_bOpenDbgInf;
    int32 m_nStateMachines;
    TIMER_ID m_tReqCfgTimerID;
    CEvbMgr *m_pEvbMgr;
    CVNetVmAgent * m_pVNetVmAgent;
    CVNAVnicPool *m_pVNAVnicPool;
    CPortManager *m_pPortMgr;
    //CBridgeManager *m_cBridgeMgr;
    CDevMonMgr * m_pDevMonMgr;
    CVNetMonitorMgr * m_pMonMgr;
    CSwitchAgent * m_pSwitchAgent;
    CPortAgent  *m_pPortAgent;
    CVxlanAgent *m_pVxlanAgent;

    DISALLOW_COPY_AND_ASSIGN(CVNAController);
};
}// namespace zte_tecs

#endif // VNA_CONTROLLER_INCLUDE_H__

