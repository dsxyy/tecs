/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：vnm_controller.h
* 文件标识：
* 内容摘要：CVNMCtrl类的定义文件
* 当前版本：1.0
* 作    者：
* 完成日期：2013年1月18日
*
* 修改记录1：
*     修改日期：2013/1/18
*     版 本 号：V2.0
*     修 改 人：Wang.Lule
*     修改内容：创建
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

