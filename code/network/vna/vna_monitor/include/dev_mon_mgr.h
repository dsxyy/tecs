/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�dev_mon_mgr.h
* �ļ���ʶ��
* ����ժҪ��dev��ع�����Ķ����ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�
* ������ڣ�2013��1��16��
*
* �޸ļ�¼1��
*     �޸����ڣ�2013/1/16
*     �� �� �ţ�V1.0
*     �� �� �ˣ�lverchun
*     �޸����ݣ�����
******************************************************************************/
#ifndef DEV_MON_MGR_INCLUDE_H__
#define DEV_MON_MGR_INCLUDE_H__

namespace zte_tecs
{

/**
@brief �������������������
@li @b ����˵������
*/
class CDevMonTask
{
public:
    CDevMonTask(CNetDevMgr *pDevMgr, int32 nMonInterval);
    virtual ~CDevMonTask();

    void SetDevMgr(CNetDevMgr *pDevMgr){m_pDevMgr = pDevMgr;};
    void SetMonInterval(int32 nMonInterval){m_nMonitorInterval = nMonInterval;};
    void SetLastMon(time_t nLastMonTime){m_nLastMonTime = nLastMonTime;};
    CNetDevMgr * GetDevMgr(){return m_pDevMgr;};
    int32 GetMonInterval(){return m_nMonitorInterval;};
    time_t GetLastMon(){return m_nLastMonTime;};
    void DbgShowData();
    
private:
    CNetDevMgr * m_pDevMgr;
    int32 m_nMonitorInterval;
    time_t m_nLastMonTime;
};

/**
@brief ����������������������
@li @b ����˵������
*/
class CDevMonMgr
{
public:
    CDevMonMgr();
    virtual ~CDevMonMgr();

    static CDevMonMgr *GetInstance()
    {
        if (NULL == s_pInstance)
        {
            s_pInstance = new CDevMonMgr();
        }

        return s_pInstance;
    };
    
    STATUS Init(MessageUnit  *mu);   
    void MessageEntry(const MessageFrame& message);
    void AddMonitorTask(CNetDevMgr * pDevMgr, CNetDevVisitor *cDevVisitor, int32 nMonInterval = DEV_MON_DEV_INTERVAL);
    void AddMonitorTask(CNetDevMgr * pDevMgr, CBridgeVisitor *cBridgeVisitor, int32 nMonInterval = DEV_MON_BRIDGE_INTERVAL);
    void AddMonitorTask(CNetDevMgr * pDevMgr, CPortVisitor *cPortVisitor, int32 nMonInterval = DEV_MON_PORT_INTERVAL);
    void AddMonitorTask(CNetDevMgr * pDevMgr, CVnicVisitor *cVnicVisitor, int32 nMonInterval = DEV_MON_VNIC_INIT_INTERVAL);
    void DoMonitorTask();
    void SendVNAReport(const MessageFrame &message);
    void DbgShowData();
    void SetDbgFlag(int bDbgFlag);

private:
    static CDevMonMgr *s_pInstance;
    BOOL m_bOpenDbgInf;
    MessageUnit  *m_pMU;                      // ��Ϣ��Ԫ
    TIMER_ID m_nTimerID;
    
    map<CDevMonTask *, CNetDevVisitor *> m_vecDevMonTask;
    map<CDevMonTask *, CBridgeVisitor *> m_vecBridgeMonTask;
    map<CDevMonTask *, CPortVisitor *> m_vecPortMonTask;
    map<CDevMonTask *, CVnicVisitor *> m_vecVnictMonTask;
};


}
#endif // #ifndef DEV_MON_MGR_INCLUDE_H__


