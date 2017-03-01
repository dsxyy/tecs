
/******************************************************************************
* Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：vnm_schedule.h
* 文件标识：
* 内容摘要：CVNAReportMgr类的定义文件
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
#if !defined(VNA_REPORT_MGR_INCLUDE_H__)
#define VNA_REPORT_MGR_INCLUDE_H__

namespace zte_tecs
{


class CVNetVmDB;
class CVNetVnicDB;
class CVSIProfileMgr;
class CSwitchMgr;
class CVNAManager;
class CDbSchedule;

#if 1




// VNM XMLRPC 查询主控类
class CVNAReportMgr: public MessageHandler
{
public:
    explicit CVNAReportMgr();
    virtual ~CVNAReportMgr();
    
    int32 Init();
    STATUS StartMu(string strMsgUnitName);
    void MessageEntry(const MessageFrame &message);
    void DbgShowData(void);
    void SetDbgFlag(int bDbgFlag);

    static CVNAReportMgr *GetInstance()
    {
        if(NULL == s_pInstance)
        {
            s_pInstance = new CVNAReportMgr();
        }

        return s_pInstance;
    };
public:
    
private:  
    void ProcVNAReport(const MessageFrame& message);
    void ProcNewVnaNotify(const MessageFrame &message);
    void ProcUpdateVNA(const MessageFrame &message);
    int32 DispatchMsg(CVNAReportMsg *pMsg);
    
private:
    BOOL m_bOpenDbgInf;
    MessageUnit *m_pMU;
    CVNetVmDB * m_pVNetVmDB;
    CVNetVnicDB * m_pVNetVnicDB;
    CVSIProfileMgr * m_pVSIProfileMgr;
    CSwitchMgr * m_pSwitchMgr;
    CVNAManager * m_pVNAMgr;
    CLogicNetworkMgr * m_pLogicNetworkMgr;
    CPortMgr   *m_pPortMgr;
private:
    static CVNAReportMgr *s_pInstance;

    DISALLOW_COPY_AND_ASSIGN(CVNAReportMgr);

};


#endif 
}

#endif

