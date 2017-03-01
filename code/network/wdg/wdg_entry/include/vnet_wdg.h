/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�vnet_wdg.h
* �ļ���ʶ��
* ����ժҪ��CVNetWatchDog��Ķ����ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�liuhx
* ������ڣ�2013��4��16��
******************************************************************************/

#if !defined(VNET_WDG_INCLUDE_H__)
#define VNET_WDG_INCLUDE_H__

//#include "vm_messages.h"
//#include "vnet_msg_data.h"
//#include "sr_iov_manager.h"
#include "vnet_mid.h"
#include "vnetlib_msg.h"

namespace zte_tecs
{

#define MAC_SIZE (8192)

class CVNetWatchDog : public MessageHandler
{
public:
    virtual ~CVNetWatchDog();

public:
    int32 StartWatchDog(const MessageFrame& message);
    int32 StopWatchDog(const MessageFrame& message);

    int32 OperWatchDog(const MessageFrame& message);
    int32 OperWatchDogTimer(const MessageFrame& message);

    int32 StopRRTimer(const MessageFrame& message);

    //�����(�����������)���õĿ��Ź���ʱ����������
    int32 StartWDGTimer(const MessageFrame& message);
    int32 StopWDGTimer(const MessageFrame& message);


    //�ڲ��������Ź���ʱ��
    int32 AddWDGTimer(uint32 timerout, const BYTE *paucMac, int64 vmid, const string& vm_name, const int32 vm_state);
    int32 SetWDGTimer(uint32 timerout, const BYTE *paucMac);
    int32 KillWDGTimer(int64 vmid);

    //MAC��ַ������ͷ�
    uint32 AllocMacNum(void);
    int32 FreeMacNum(uint32 dwMacNum);
    //�ڴ��ļ��е�MAC��ַ����
    int32 SetMacNum(uint32 dwMacNum);
    int32 WriteWdgFile(const string &mac, const int64 &nVMID, const int64 &nTimerOut, const int32 &nState);

    int32 DelWdgFile(const string &mac);
    int32 ReadWdgFile(void);
    int32 GetVMMacById(const int64 &vm_id, string &mac);
    int32 GetVMInfoById(const int64 &vm_id, string &vm_mac, uint32 &vm_timerout, int32 &vm_state);

    //�������Ź�����
    int32 CreateWdgBridge(void);

    friend void *StartupDom0WdgThread(void* arg);

    static CVNetWatchDog *GetInstance()
    {
        if (NULL == s_pInstance)
        {
            s_pInstance = new CVNetWatchDog();
        }

        return s_pInstance;
    };

    STATUS Init()
    {
        return StartMu(MU_VNET_WDG);
    }


public:
    int32 DbgShowData(void);
    int32 DbgWdgTimer(int64 vmid, int64 operation);
    void SetDbgInfFlg(BOOL bOpen)
    {
        m_bOpenDbgInf = bOpen;
        return ;
    }


protected:
    STATUS Receive(const MessageFrame& message);
    void MessageEntry(const MessageFrame& message);

    int32 InitWDG(void);

private:
    CVNetWatchDog();
    STATUS StartMu(const string& name);

private:
    static CVNetWatchDog *s_pInstance;

private:
    MessageUnit *mu;
    pthread_mutex_t m_mutex; //��Ϣ����
    pthread_mutex_t m_mutex_timer; //��ʱ��
    pthread_mutex_t m_mutex_mac; //MAC����
    pthread_mutex_t m_mutex_wdgfile; //�ڴ��ļ���д

    int32 m_nStateMachines;
    BYTE *m_pMacPool;

    typedef struct tagVnetTimerInfo
    {
        TIMER_ID tTimerID;        //��ʱ��ID
        uint64 nWdFeedCnt;        //ι������
        unsigned char aucMac[6];  //�Ϳ��Ź����Ź�����MAC
        int64 vmid;               //�����ID
        int32 apprunflag;         //��ʱ������1��ֹͣ0(Ӧ��ʹ��)
        int32 tecsrunflag;        //��ʱ������1��ֹͣ0(tecsʹ��)
        uint32 apptime;           //Ӧ�����õ�ι��ʱ��(��V4Ӧ��)
        uint32 tecstime;          //tecs�������õ�ʱ�������������ʱ����
        string vm_name;           //���������
    }
    T_WdTimerInfo;

    vector<T_WdTimerInfo> m_vecWDGTM;

    BOOL m_bOpenDbgInf;

private:

};
}// namespace zte_tecs

#endif // VNET_WDG_INCLUDE_H__

