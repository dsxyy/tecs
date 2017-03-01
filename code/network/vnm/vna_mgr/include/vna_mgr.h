/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：vna_mgr.h
* 文件标识：
* 内容摘要：CVNAManager类的定义文件
* 当前版本：1.0
* 作    者：
* 完成日期：2012年1月18日
*
* 修改记录1：
*     修改日期：2013/1/18
*     版 本 号：V1.0
*     修 改 人：Wang.Lule
*     修改内容：创建
*
******************************************************************************/

#if !defined(VNA_MGR_INCLUDE_H__)
#define VNA_MGR_INCLUDE_H__

namespace zte_tecs
{
    class CVNAManager
    {
    public:
        explicit CVNAManager();
        virtual ~CVNAManager();

    public:
        static CVNAManager *GetInstance()
        {
            if(NULL == s_pInstance)
            {
                s_pInstance = new CVNAManager();
            }
            
            return s_pInstance;
        };

        int32 Init(MessageUnit *pMu, const string &cstrVNMApp);
        int32 QueryVNAUUID(const string strCCApp, const string &strHCApp, string &strVNAUUID);
        int32 QueryVNAApp(const string strCCApp, const string &strHCApp, string &strVNAApp);
        int32 QueryVNAModInfo(const string &strVNAUUID, string &strHCApp, string &strCCApp);
        int32 QueryVNAModRole(const string &strVNAUUID, const uint8 &ucRole);
    public:
        void MessageEntry(const MessageFrame &message);
        
        void DbgShowVNAApps(void);
        void DbgShowVNA(const string &strVNAApp);
        void SetDbgInfFlg(BOOL bOpen){ m_bOpenDbgInf = bOpen; return ;}

        
    protected:    
        int32 ProcVNARegReq(const MessageFrame &message);
        int32 ProcVNAReport(const MessageFrame &message);
        int32 ProcVNAHeartbeat(const MessageFrame &message);
        int32 ProcVNAMoniTmOut(void);
        int32 ProcVNADelete(const MessageFrame &message);
        
        CVNAInfo *GetVNAInfo(const string &cstrApp);
        int32 DelVNAInfo(const string &cstrApp);        
        int32 QueryVNA(const string strCCApp, const string &strHCApp, CVNAInfo &cVNAInfo);
        int32 NotifyStateChanged(const string &cstrVNAUUID, ENObjState enState);
        int32 DispatchMsg(CVNAReportMsg *pMsg);

    private:
        int32 UpdateVNAInfo(CVNAInfo *pVNA, CVNAReportMsg *pMsg);
        int32 LoadVNAInfoFromDB(void);
        int32 AddVNA2DB(CVNAInfo *pVNAInfo, int32 &bNewVna);
        int32 UpdateVNA2DB(CVNAInfo *pVNAInfo);
        int32 DeleteVNA2DB(CVNAInfo *pVNAInfo);
        int32 GetVNAInfoFromDB(const string &cstrUUID, CVNAInfo &cVNAInfo);
        int32 ClearAllVNAInfo(void);

        void DbgPrintVNA(CVNAInfo *pVNA);
        
    private:
        static CVNAManager *s_pInstance;

        MessageUnit *m_pMU;
        TIMER_ID m_tVNAMoniTimerID;
        map<string, CVNAInfo *> m_mapVNAInfo;

        BOOL m_bOpenDbgInf;
        string m_strVNMApp;
        
        DISALLOW_COPY_AND_ASSIGN(CVNAManager);
    };
}// namespace zte_tecs

#endif

