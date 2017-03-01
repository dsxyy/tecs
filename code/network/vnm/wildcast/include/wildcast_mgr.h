/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�vnm wildcast manager
* �ļ���ʶ��
* ����ժҪ��CVNMCtrl��Ķ����ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�
* ������ڣ�2013��1��18��
*
* �޸ļ�¼1��
*     �޸����ڣ�2013/1/18
*     �� �� �ţ�V2.0
*     �� �� �ˣ�gongxiefeng
*     �޸����ݣ�����
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

    // ��VNA ע�������󣬵��ýӿ�
    void NewVnaRegist(const string & strVnaUuid);
    void WildcastTaskMsgAck(int32 wcTaskType, const string & strTaskUuid, int32 nResult);

    // ��switch ģ����� 
    int32 SwitchNameIsConflict( const string & swName, int32 & outResult);
    int32 SwitchPortIsConflict(const string & vnaUuid, const string & swName, int32 swType, 
            vector<string> & vPortName, int32 & outResult);

    // ��portģ�����(��������˿�)
    int32 CreateVPortIsConflict(const string & vnaUuid, const string & portName,int32 & outResult);

    // ��portģ�����(���û��ض˿�)
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

    CWildcastVSwitch       m_VSwitch; //��ʱֻ����DVS
    CWildcastCreateVPort   m_CreateVPort;
    CWildcastLoopback      m_Loopback;

    TIMER_ID m_tTimerID;
    BOOL   m_bOpenDbg;
    
private:
    DISALLOW_COPY_AND_ASSIGN(CWildcastMgr);
};

}// namespace zte_tecs

#endif

