
/******************************************************************************
* Copyright (c) 2012������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�vnm_schedule.h
* �ļ���ʶ��
* ����ժҪ��CVNAReportMgr��Ķ����ļ�
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




// VNM XMLRPC ��ѯ������
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

