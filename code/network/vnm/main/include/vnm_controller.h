/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�vnm_controller.h
* �ļ���ʶ��
* ����ժҪ��CVNMCtrl��Ķ����ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�
* ������ڣ�2013��1��18��
*
* �޸ļ�¼1��
*     �޸����ڣ�2013/1/18
*     �� �� �ţ�V2.0
*     �� �� �ˣ�Wang.Lule
*     �޸����ݣ�����
******************************************************************************/

#if !defined(VNM_CONTROLLER_INCLUDE_H__)
#define VNM_CONTROLLER_INCLUDE_H__

namespace zte_tecs
{
    class CVNMCtrl
    {    
    public:
        explicit CVNMCtrl();
        virtual ~CVNMCtrl();
        
    public:    
        static CVNMCtrl *GetInstance()
        {
            if(NULL == s_pInstance)
            {
                s_pInstance = new CVNMCtrl();
            }
            
            return s_pInstance;
        };

    public:
        int32 Init(MessageUnit *pMu, const string &cstrVNMApp);
        void MessageEntry(const MessageFrame &message);
        void DbgShowVNMCtrlInfo(void);
        void DbgShowSdnInfo(void);
        
    protected:
        int32 ProcVNMInfo(const MessageFrame &message);
        int32 ProcMcVNMInfoTmOut(void);
        int32 ProcSdnInfo(const MessageFrame &message);
        int32 AddSdnCtrl(const CSdnCfgMsg &cMsg, string &strRetInfo);
        int32 DelSdnCtrl(const CSdnCfgMsg &cMsg, string &strRetInfo);
        int32 ModSdnCtrl(const CSdnCfgMsg &cMsg, string &strRetInfo);
#if 0
        int32 AddQuantumRestful(const CSdnCfgMsg &cMsg, string &strRetInfo);
        int32 DelQuantumRestful(const CSdnCfgMsg &cMsg, string &strRetInfo);
        int32 ModQuantumRestful(const CSdnCfgMsg &cMsg, string &strRetInfo);
#endif        
    private:    
        static CVNMCtrl * s_pInstance;
        MessageUnit *m_pMU;

        TIMER_ID m_tMcVNMInfoTimerID;
        string m_strApp;
        uint8 m_ucMasterOrSlave;
        uint8 m_ucPriority;

        DISALLOW_COPY_AND_ASSIGN(CVNMCtrl);
    };
    
}// namespace zte_tecs

#endif

