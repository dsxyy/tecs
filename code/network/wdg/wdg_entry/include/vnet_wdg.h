/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：vnet_wdg.h
* 文件标识：
* 内容摘要：CVNetWatchDog类的定义文件
* 当前版本：1.0
* 作    者：liuhx
* 完成日期：2013年4月16日
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

    //虚拟机(重启、挂起等)调用的看门狗定时器操作函数
    int32 StartWDGTimer(const MessageFrame& message);
    int32 StopWDGTimer(const MessageFrame& message);


    //内部操作看门狗定时器
    int32 AddWDGTimer(uint32 timerout, const BYTE *paucMac, int64 vmid, const string& vm_name, const int32 vm_state);
    int32 SetWDGTimer(uint32 timerout, const BYTE *paucMac);
    int32 KillWDGTimer(int64 vmid);

    //MAC地址分配和释放
    uint32 AllocMacNum(void);
    int32 FreeMacNum(uint32 dwMacNum);
    //内存文件中的MAC地址操作
    int32 SetMacNum(uint32 dwMacNum);
    int32 WriteWdgFile(const string &mac, const int64 &nVMID, const int64 &nTimerOut, const int32 &nState);

    int32 DelWdgFile(const string &mac);
    int32 ReadWdgFile(void);
    int32 GetVMMacById(const int64 &vm_id, string &mac);
    int32 GetVMInfoById(const int64 &vm_id, string &vm_mac, uint32 &vm_timerout, int32 &vm_state);

    //创建看门狗网桥
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
    pthread_mutex_t m_mutex; //消息处理
    pthread_mutex_t m_mutex_timer; //定时器
    pthread_mutex_t m_mutex_mac; //MAC分配
    pthread_mutex_t m_mutex_wdgfile; //内存文件读写

    int32 m_nStateMachines;
    BYTE *m_pMacPool;

    typedef struct tagVnetTimerInfo
    {
        TIMER_ID tTimerID;        //定时器ID
        uint64 nWdFeedCnt;        //喂狗次数
        unsigned char aucMac[6];  //和看门狗网桥关联的MAC
        int64 vmid;               //虚拟机ID
        int32 apprunflag;         //定时器运行1；停止0(应用使用)
        int32 tecsrunflag;        //定时器运行1；停止0(tecs使用)
        uint32 apptime;           //应用配置的喂狗时长(如V4应用)
        uint32 tecstime;          //tecs自身配置的时长，虚拟机部署时配置
        string vm_name;           //虚拟机名称
    }
    T_WdTimerInfo;

    vector<T_WdTimerInfo> m_vecWDGTM;

    BOOL m_bOpenDbgInf;

private:

};
}// namespace zte_tecs

#endif // VNET_WDG_INCLUDE_H__

