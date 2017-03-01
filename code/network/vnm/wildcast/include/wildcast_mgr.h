/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：vnm wildcast manager
* 文件标识：
* 内容摘要：CVNMCtrl类的定义文件
* 当前版本：1.0
* 作    者：
* 完成日期：2013年1月18日
*
* 修改记录1：
*     修改日期：2013/1/18
*     版 本 号：V2.0
*     修 改 人：gongxiefeng
*     修改内容：创建
******************************************************************************/

#if !defined(VNM_WILDCAST_MANAGER_INCLUDE_H__)
#define VNM_WILDCAST_MANAGER_INCLUDE_H__

#include "wildcast_switch.h"
#include "wildcast_create_vport.h"
#include "wildcast_loopback.h"
namespace zte_tecs
{
class CWildcastMgr
{    
public:
    explicit CWildcastMgr();
    virtual ~CWildcastMgr();
    
public:    
    static CWildcastMgr *GetInstance()
    {
        if(NULL == s_pInstance)
        {
            s_pInstance = new CWildcastMgr();
        }
        
        return s_pInstance;
    };

public:
    int32 Init(MessageUnit *pMu);
    void MessageEntry(const MessageFrame &message);
    void SetDbgPrintFlag(BOOL bOpen){ m_bOpenDbg = bOpen;}
    void DbgShow(void);

    // 新VNA 注册上来后，调用接口
    void NewVnaRegist(const string & strVnaUuid);
    void WildcastTaskMsgAck(int32 wcTaskType, const string & strTaskUuid, int32 nResult);

    // 供switch 模块调用 
    int32 SwitchNameIsConflict( const string & swName, int32 & outResult);
    int32 SwitchPortIsConflict(const string & vnaUuid, const string & swName, int32 swType, 
            vector<string> & vPortName, int32 & outResult);

    // 供port模块调用(创建虚拟端口)
    int32 CreateVPortIsConflict(const string & vnaUuid, const string & portName,int32 & outResult);

    // 供port模块调用(设置环回端口)
    int32 LoopbackIsConflict(const string & vnaUuid, const string & portName,int32 & outResult);
private:
    int32 ProcTimeOut();
    STATUS ProcSwitchMsg(const MessageFrame& message);
    STATUS ProcSwitchPortMsg(const MessageFrame& message);

    STATUS ProcCreateVPortMsg(const MessageFrame& message);
    STATUS ProcLoopbackMsg(const MessageFrame& message);

    STATUS ProcSwitchTaskMsg(const MessageFrame& message);
    STATUS ProcCreateVPortTaskMsg(const MessageFrame& message);
    STATUS ProcLoopbackTaskMsg(const MessageFrame& message);
private:    
    static CWildcastMgr * s_pInstance;
    MessageUnit *m_pMU;

    CWildcastVSwitch       m_VSwitch; //暂时只考虑DVS
    CWildcastCreateVPort   m_CreateVPort;
    CWildcastLoopback      m_Loopback;

    TIMER_ID m_tTimerID;
    BOOL   m_bOpenDbg;
    
private:
    DISALLOW_COPY_AND_ASSIGN(CWildcastMgr);
};

}// namespace zte_tecs

#endif

