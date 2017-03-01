/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：dev_mon_mgr.h
* 文件标识：
* 内容摘要：dev监控管理类的定义文件
* 当前版本：1.0
* 作    者：
* 完成日期：2013年1月16日
*
* 修改记录1：
*     修改日期：2013/1/16
*     版 本 号：V1.0
*     修 改 人：lverchun
*     修改内容：创建
******************************************************************************/
#ifndef DEV_MON_MGR_INCLUDE_H__
#define DEV_MON_MGR_INCLUDE_H__

namespace zte_tecs
{

/**
@brief 功能描述：监控任务类
@li @b 其它说明：无
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
@brief 功能描述：监控任务管理类
@li @b 其它说明：无
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
    MessageUnit  *m_pMU;                      // 消息单元
    TIMER_ID m_nTimerID;
    
    map<CDevMonTask *, CNetDevVisitor *> m_vecDevMonTask;
    map<CDevMonTask *, CBridgeVisitor *> m_vecBridgeMonTask;
    map<CDevMonTask *, CPortVisitor *> m_vecPortMonTask;
    map<CDevMonTask *, CVnicVisitor *> m_vecVnictMonTask;
};


}
#endif // #ifndef DEV_MON_MGR_INCLUDE_H__


