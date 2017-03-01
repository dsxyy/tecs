/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：vnet_wdg.cpp
* 文件标识：
* 内容摘要：CVNetWatchDog类实现文件
* 当前版本：1.0
* 作    者：liuhx
* 完成日期：2012年4月20日
******************************************************************************/

#include "vnet_comm.h"
#include "vnet_wdg.h"
#include "vnet_event.h"
#include "vnetlib_event.h"
#include "vnet_function.h"
#include "vnet_libnet.h"
#include "vnet_error.h"
#include <fstream>

//extern STATUS GetRegisteredSystem(string &system);

namespace zte_tecs
{
//存放MAC地址的内存文件
#define WDG_DIR   "/dev/shm/tecs/watchdog/"
//最大7200秒，即2小时
#define WDG_MAX_TIMEOUT (uint32)(7200)
//喂狗超时误差,2秒
#define WDG_TIMEOUT_ERROR (uint32)(2)

//WDG实例
CVNetWatchDog* CVNetWatchDog::s_pInstance = NULL;

//打印开关
BYTE g_ucPktRcvFlag = 0;

CVNetWatchDog::CVNetWatchDog()
{
    m_nStateMachines = 0;
    m_bOpenDbgInf = VNET_FALSE;
    m_pMacPool = NULL;
    mu = NULL;
};

CVNetWatchDog::~CVNetWatchDog()
{
    vector<T_WdTimerInfo>::iterator itr = m_vecWDGTM.begin();
    for ( ; itr != m_vecWDGTM.end(); ++itr)
    {
        mu->KillTimer(itr->tTimerID);
    }

    m_vecWDGTM.clear();

    if (NULL != m_pMacPool)
    {
        free(m_pMacPool);
    }

    delete mu;
    delete s_pInstance;
    pthread_mutex_destroy(&m_mutex);
    pthread_mutex_destroy(&m_mutex_timer);
    pthread_mutex_destroy(&m_mutex_mac);
    pthread_mutex_destroy(&m_mutex_wdgfile);
};

STATUS CVNetWatchDog::StartMu(const string& name)
{
    if (mu)
    {
        SkyAssert(false);
        return ERROR_DUPLICATED_OP;
    }

    // 消息单元的创建和初始化
    mu = new MessageUnit(name);
    if (!mu)
    {
        VNET_LOG(VNET_LOG_ERROR, "Create mu %s failed!\n",name.c_str());
        return ERROR_NO_MEMORY;
    }

    STATUS ret = mu->SetHandler(this);
    if (SUCCESS != ret)
    {
        VNET_LOG(VNET_LOG_ERROR, "SetHandler mu %s failed! ret = %d\n",name.c_str(),ret);
        return ret;
    }

    ret = mu->Run();
    if (SUCCESS != ret)
    {
        VNET_LOG(VNET_LOG_ERROR, "Run mu %s failed! ret = %d\n",name.c_str(),ret);
        return ret;
    }

    ret = mu->Register();
    if (SUCCESS != ret)
    {
        VNET_LOG(VNET_LOG_ERROR, "Register mu %s failed! ret = %d\n",name.c_str(),ret);
        return ret;
    }

    //给自己发送上电消息，促使消息单元状态切换到工作态
    MessageFrame message(SkyInt32(0), EV_VNET_POWER_ON);
    ret = mu->Receive(message);
    if (SUCCESS != ret)
    {
        SkyAssert(0);
        return ret;
    }
    mu->Print();
    return SUCCESS;
}

int32 CVNetWatchDog::OperWatchDog(const MessageFrame& message)
{
    CVNetWdgMsg       cMsg;
    cMsg.deserialize(message.data);

    switch (cMsg.m_nOperator)
    {
    case EN_START_WDG:
    {
        cout << "OperWatchDog[Start]:" <<cMsg.m_nOperator << endl;
        StartWatchDog(message);
        break;
    }
    case EN_STOP_WDG:
    {
        cout << "OperWatchDog[Stop]:" <<cMsg.m_nOperator << endl;
        StopWatchDog(message);
        break;
    }
    default:
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::OperWatchDog: Oper[%d] is not right .\n", cMsg.m_nOperator);
        return -1;
        break;
    }
    }

    return 0;
}

int32 CVNetWatchDog::OperWatchDogTimer(const MessageFrame& message)
{
    CVNetWdgMsg       cMsg;
    cMsg.deserialize(message.data);

    switch (cMsg.m_nOperator)
    {
    case EN_START_WDGTIMER:
    {
        cout << "OperWatchDogTimer[Start]:" <<cMsg.m_nOperator << endl;
        StartWDGTimer(message);
        break;
    }
    case EN_STOP_WDGTIMER:
    {
        cout << "OperWatchDogTimer[Stop]:" <<cMsg.m_nOperator << endl;
        StopWDGTimer(message);
        break;
    }
    default:
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::OperWatchDogTimer: Oper[%d] is not right .\n", cMsg.m_nOperator);
        return -1;
        break;
    }
    }

    return 0;
}


//部署虚拟机时候调用
int32 CVNetWatchDog::StartWatchDog(const MessageFrame& message)
{
    CVNetWdgMsg       cMsg;
    CVNetWdgAckMsg  cAckMsg;

    cMsg.deserialize(message.data);
    cMsg.Print();

    stringstream ss;
    uint8 ucMac[6];
    uint32 dwMacNum;
    int32 iMac3, iMac4,iMac5;

    string mac;

    MessageOption option(message.option.sender(), EV_VNETLIB_WDG_OPER_ACK, 0, 0);
    cAckMsg.m_cMid = cMsg.m_cMid;
    cAckMsg.m_cVnaWdgMid = cMsg.m_cVnaWdgMid;
    cAckMsg.state = WDG_SUCCESS;
    cAckMsg.m_nOperator = EN_START_WDG_ACK;
    cAckMsg.m_qwVmId = cMsg.m_qwVmId;
    cAckMsg.action_id = cMsg.action_id;
    //cAckMsg.progress = 100;

    //不启用看门狗
    if (0 == cMsg.m_nTimerOut)
    {
        cAckMsg.state = ERROR_WDG_TIMEROUT;
        cAckMsg.progress = 0;
        cAckMsg.Print();
        return mu->Send(cAckMsg, option);
    }

    if (cMsg.m_nTimerOut > WDG_MAX_TIMEOUT)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::StartWatchDog: timeout value abnormal.\n");
        cAckMsg.state = ERROR_WDG_TIMEROUT;
        cAckMsg.progress = 0;
        cAckMsg.Print();
        return mu->Send(cAckMsg, option);
    }

    if (0 == GetVMMacById(cMsg.m_qwVmId, mac))
    {
        if (0 != vnet_mac_str_to_byte(mac, ucMac))
        {
            cAckMsg.state = ERROR_WDG_MAC;
            cAckMsg.progress = 0;
            cAckMsg.Print();
            return mu->Send(cAckMsg, option);
        }

        cout << "StartWatchDog: VM Wdg is exit:" << cMsg.m_qwVmId<< endl;

        if (0 != AddWDGTimer(cMsg.m_nTimerOut, ucMac, cMsg.m_qwVmId, "", 0))
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::StartWatchDog(Exit) AddWDGTimer failed.\n");
            cAckMsg.state = ERROR_WDG_START;
            cAckMsg.progress = 0;
            cAckMsg.Print();
            return mu->Send(cAckMsg, option);
        }

        cAckMsg.m_cVnic.m_strMac.assign(mac);
        cAckMsg.m_cVnic.m_strBridgeName = "br_wd";
        cAckMsg.state = WDG_SUCCESS;
        cAckMsg.progress = 100;
        cAckMsg.Print();
        return mu->Send(cAckMsg, option);
    }

    //分配MAC地址，即先获取一个数值，再转换成MAC地址
    dwMacNum = AllocMacNum();
    if (0 == dwMacNum)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::StartWatchDog: AllocMacNum failed.\n");
        cAckMsg.state = ERROR_WDG_MAC;
        cAckMsg.progress = 0;
        cAckMsg.Print();
        return mu->Send(cAckMsg, option);
    }

    ucMac[0] = 0x00;
    ucMac[1] = 0x00;
    ucMac[2] = 0x01;
    ucMac[3] = 0x00;
    ucMac[4] = ((dwMacNum>>8) & 0xFF);
    ucMac[5] = (dwMacNum & 0xFF);

    iMac3=ucMac[3];
    iMac4=ucMac[4];
    iMac5=ucMac[5];

    ss.str("");
    ss << "00:00:01:"
    << hex << setfill('0') << setw(2) << iMac3 << ":"
    << hex << setfill('0') << setw(2) << iMac4 << ":"
    << hex << setfill('0') << setw(2) << iMac5;

    //添加定时器
    if (0 != AddWDGTimer(cMsg.m_nTimerOut, ucMac, cMsg.m_qwVmId, "", 0))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::StartWatchDog AddWDGTimer failed.\n");
        //MAC地址回收
        FreeMacNum(dwMacNum);
        cAckMsg.state = ERROR_WDG_MAC;
        cAckMsg.progress = 0;
        cAckMsg.Print();
        return mu->Send(cAckMsg, option);
    }

    if (0 != WriteWdgFile(ss.str(), cMsg.m_qwVmId, cMsg.m_nTimerOut, 0))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::StartWatchDog WriteWdgFile failed.\n");
        //MAC地址回收
        FreeMacNum(dwMacNum);
        KillWDGTimer(cMsg.m_qwVmId);
        cAckMsg.state = ERROR_WDG_MAC;
        cAckMsg.progress = 0;
        cAckMsg.Print();
        return mu->Send(cAckMsg, option);
    }

    cAckMsg.m_cVnic.m_strMac.assign(ss.str());
    cAckMsg.m_cVnic.m_strBridgeName = "br_wd";
    cAckMsg.state = WDG_SUCCESS;
    cAckMsg.progress = 100;
    cAckMsg.Print();
	
    return mu->Send(cAckMsg, option);
}


//撤销部署虚拟机时候调用
int32 CVNetWatchDog::StopWatchDog(const MessageFrame& message)
{
    CVNetWdgMsg      cMsg;
    CVNetWdgAckMsg cAckMsg;

    cMsg.deserialize(message.data);
    cMsg.Print();

    BYTE ucMac[6] = {0};
    uint32 dwTmp = 0;
    string mac = "";
    int32 ret = 0;

    MessageOption option(message.option.sender(), EV_VNETLIB_WDG_OPER_ACK, 0, 0);
    cAckMsg.m_cMid = cMsg.m_cMid;
    cAckMsg.m_cVnaWdgMid = cMsg.m_cVnaWdgMid;
    cAckMsg.state = WDG_SUCCESS;
    cAckMsg.m_nOperator = EN_STOP_WDG_ACK;
    cAckMsg.m_qwVmId = cMsg.m_qwVmId;
    cAckMsg.action_id = cMsg.action_id;

    ret = GetVMMacById(cMsg.m_qwVmId, mac);
    if (0 != ret)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::StopWatchDog GetVMMacById failed.\n");
        cAckMsg.state = ERROR_WDG_MAC;
        cAckMsg.progress = 0;
        cAckMsg.Print();
        return mu->Send(cAckMsg, option);
    }

    //遍历针对看门狗的nic
    if (!vnet_mac_str_to_byte(mac, ucMac))
    {
        //因为增加了针对看门狗的nic数据结构，判断可能没必要
        if ((ucMac[0] == 0x00) && (ucMac[1] == 0x00) && (ucMac[2] == 0x01))
        {
            dwTmp = ((ucMac[4]<<8) | ucMac[5]);
            //释放MAC
            FreeMacNum(dwTmp);
            //从内存文件中删除
            DelWdgFile(mac);
            //删除定时器
            if (0 != KillWDGTimer(cMsg.m_qwVmId))
            {
                VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::StopWatchDog: Kill WDGTimer failed.\n");
                //删除定时失败影响挺大，返回失败
                cAckMsg.state = ERROR_WDG_STOP;
                cAckMsg.progress = 0;
                cAckMsg.Print();
                return mu->Send(cAckMsg, option);
            }
        }
    }
    else
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::StopWatchDog: [%s] vnet_mac_str_to_byte failed.\n", mac.c_str());
        cAckMsg.state = ERROR_WDG_STOP;
        cAckMsg.progress = 0;
        cAckMsg.Print();
        return mu->Send(cAckMsg, option);
    }

    cAckMsg.state = WDG_SUCCESS;
    cAckMsg.progress = 100;
    cAckMsg.Print();
    return mu->Send(cAckMsg, option);
}

//CVNetWatchDog部署虚拟机调用
int32 CVNetWatchDog::AddWDGTimer(uint32 timerout, const BYTE *paucMac, int64 vmid, const string& vm_name, const int32 vm_state)
{
    T_WdTimerInfo sWdStartTimerInfo;
    TIMER_ID tWdStartTimerId;

    if (0 != pthread_mutex_lock(&m_mutex_timer))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::AddWDGTimer: pthread_mutex_lock(0x%x) failed.\n", &m_mutex_timer);
        return -1;
    }

    //使用vector搜索，性能可能不高，下同
    vector<T_WdTimerInfo>::iterator itr = m_vecWDGTM.begin();
    for ( ; itr != m_vecWDGTM.end(); ++itr)
    {
        //重复添加判断，MAC地址存在唯一性，VM的NAME和ID不参与判断
        if (0 == memcmp(itr->aucMac, paucMac, 6))
        {
            //VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::AddWDGTimer(VM:%d): timer already exits and start timer failed.\n", vmid);
            if (0 != pthread_mutex_unlock(&m_mutex_timer))
            {
                VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::AddWDGTimer: pthread_mutex_unlock(0x%x) failed.\n", &m_mutex_timer);
                return -1;
            }
            return 0;
        }
    }

    //创建定时器
    tWdStartTimerId = mu->CreateTimer();
    if (INVALID_TIMER_ID == tWdStartTimerId)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::AddWDGTimer(VM:%d): Create wd start timer failed.\n", vmid);
        if (0 != pthread_mutex_unlock(&m_mutex_timer))
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::AddWDGTimer: pthread_mutex_unlock(0x%x) failed.\n", &m_mutex_timer);
        }
        return -1;
    }

    TimeOut  tTmOut;
    //单位为秒
    tTmOut.duration = timerout*1000;
    tTmOut.msgid = EV_VNET_WDG_TIMER;
    tTmOut.type = LOOP_TIMER;
    tTmOut.arg = vmid;

    //设置定时器
    if (SUCCESS != mu->SetTimer(tWdStartTimerId, tTmOut))
    {
        //回收timerid
        mu->KillTimer(tWdStartTimerId);

        VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::AddWDGTimer(VM:%d): Set timer(id:%d) failed.\n", vmid, tWdStartTimerId);
        if (0 != pthread_mutex_unlock(&m_mutex_timer))
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::AddWDGTimer: pthread_mutex_unlock(0x%x) failed.\n", &m_mutex_timer);
        }

        return -1;
    }

    sWdStartTimerInfo.tTimerID = tWdStartTimerId;
    sWdStartTimerInfo.nWdFeedCnt = 0;
    sWdStartTimerInfo.vmid = vmid;
    sWdStartTimerInfo.apprunflag = 1;

    if (vm_state)
    {
        sWdStartTimerInfo.tecsrunflag = 0;
        //先停掉，等后面再启动
        if (SUCCESS != mu->StopTimer(tWdStartTimerId))
        {
            //回收timerid
            mu->KillTimer(tWdStartTimerId);

            VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::AddWDGTimer(VM:%d): StopTimer(id:%d) failed.\n", vmid, tWdStartTimerId);
            if (0 != pthread_mutex_unlock(&m_mutex_timer))
            {
                VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::AddWDGTimer: pthread_mutex_unlock(0x%x) failed.\n", &m_mutex_timer);
            }

            return -1;
        }
    }
    else
    {
        sWdStartTimerInfo.tecsrunflag = 1;
    }

    sWdStartTimerInfo.apptime = timerout*1000;
    sWdStartTimerInfo.tecstime = timerout*1000;
    sWdStartTimerInfo.vm_name = vm_name;
    memcpy(&(sWdStartTimerInfo.aucMac[0]), paucMac, 6);
    m_vecWDGTM.push_back(sWdStartTimerInfo);

    if (0 != pthread_mutex_unlock(&m_mutex_timer))
    {
        VNET_LOG(VNET_LOG_WARN, "CVNetWatchDog::AddWDGTimer: pthread_mutex_unlock(0x%x) failed.\n", &m_mutex_timer);
    }

    return 0;
}

/**
收包线程调用
返回失败:
1.传递过来的MAC在VECTOR不存在
2.锁异常
3.设置timer异常
**/
int32 CVNetWatchDog::SetWDGTimer(uint32 timerout, const BYTE *paucMac)
{
    //int64 nVMID;
    string vm_name;
    string mac;

    //stringstream ss;
    // int32 iMac3, iMac4,iMac5;

    //限定最大喂狗时长
    if (timerout > (WDG_MAX_TIMEOUT + WDG_TIMEOUT_ERROR))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::SetWDGTimer: timeout too big.\n", timerout);
        return -1;
    }

    if (0 != pthread_mutex_lock(&m_mutex_timer))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::SetWDGTimer: pthread_mutex_lock(0x%x) failed.\n", &m_mutex_timer);
        return -1;
    }

    vector<T_WdTimerInfo>::iterator itr = m_vecWDGTM.begin();
    for ( ; itr != m_vecWDGTM.end(); ++itr)
    {
        if (0 == memcmp(itr->aucMac, paucMac, 6))
        {
            //接收计数
            itr->nWdFeedCnt++;

            //停止喂狗
            if (WDG_TIMEOUT_ERROR == timerout)
            {
                //stop timer
                if (SUCCESS != mu->StopTimer(itr->tTimerID))
                {
                    VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::SetWDGTimer: Set timer(id:%d) stop failed.\n", itr->tTimerID);
                    if (0 != pthread_mutex_unlock(&m_mutex_timer))
                    {
                        VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::SetWDGTimer: pthread_mutex_unlock(0x%x) failed.\n", &m_mutex_timer);
                    }
                    return -1;
                }
                //timer status change to stop(0)
                itr->apprunflag = 0;
                itr->apptime = 0;

                if (0 != pthread_mutex_unlock(&m_mutex_timer))
                {
                    VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::SetWDGTimer: pthread_mutex_unlock(0x%x) failed.\n", &m_mutex_timer);
                    return -1;
                }

                return 0;
            }

            //如果tecs侧设置停止定时器，接收到喂狗报文不做处理
            if (0 == itr->tecsrunflag)
            {
                if (0 != pthread_mutex_unlock(&m_mutex_timer))
                {
                    VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::SetWDGTimer: pthread_mutex_unlock(0x%x) failed.\n", &m_mutex_timer);
                }
                return 0;
            }

            TimeOut  tTmOut;
            //单位为秒，外层可以加上误差
            tTmOut.duration = timerout*1000;
            tTmOut.msgid = EV_VNET_WDG_TIMER;
            tTmOut.type = LOOP_TIMER;
            tTmOut.arg = itr->vmid;

            //reset timer
            if (SUCCESS != mu->SetTimer(itr->tTimerID, tTmOut))
            {
                VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::SetWDGTimer: Set timer(id:%d) reset failed.\n", itr->tTimerID);
                if (0 != pthread_mutex_unlock(&m_mutex_timer))
                {
                    VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::SetWDGTimer: pthread_mutex_unlock(0x%x) failed.\n", &m_mutex_timer);
                }

                return -1;
            }

            itr->apprunflag = 1;
            itr->apptime = timerout*1000;

            if (0 != pthread_mutex_unlock(&m_mutex_timer))
            {
                VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::SetWDGTimer: pthread_mutex_unlock(0x%x) failed.\n", &m_mutex_timer);
                return -1;
            }

            return 0;
        }//end memcmp
    }//end for

    if (0 != pthread_mutex_unlock(&m_mutex_timer))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::SetWDGTimer: pthread_mutex_unlock(0x%x) failed.\n", &m_mutex_timer);
    }

    return -1;
}

/**
CVNetWatchDog撤销部署虚拟机调用
**/
int32 CVNetWatchDog::KillWDGTimer(int64 vmid)
{
    if (0 != pthread_mutex_lock(&m_mutex_timer))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::KillWDGTimer: pthread_mutex_lock(0x%x) failed.\n", &m_mutex_timer);
        return -1;
    }

    vector<T_WdTimerInfo>::iterator itr = m_vecWDGTM.begin();
    for ( ; itr != m_vecWDGTM.end(); ++itr)
    {
        if (vmid == itr->vmid)
        {
            //kill timer
            if (SUCCESS != mu->KillTimer(itr->tTimerID))
            {
                VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::KillWDGTimer: kill timer(id:%d) failed.\n", vmid);
                if (0 != pthread_mutex_unlock(&m_mutex_timer))
                {
                    VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::KillWDGTimer: pthread_mutex_unlock(0x%x) failed.\n", &m_mutex_timer);
                }
                return -1;
            }

            m_vecWDGTM.erase(itr);

            if (0 != pthread_mutex_unlock(&m_mutex_timer))
            {
                VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::KillWDGTimer: pthread_mutex_unlock(0x%x) failed.\n", &m_mutex_timer);
                return -1;
            }
            return 0;
        }//end if
    }//end for

    if (0 != pthread_mutex_unlock(&m_mutex_timer))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::KillWDGTimer: pthread_mutex_unlock(0x%x) failed.\n", &m_mutex_timer);
        return -1;
    }

    return 0;
}

//虚拟机操作：启动，恢复，重启调用
int32 CVNetWatchDog::StartWDGTimer(const MessageFrame& message)
{
    CVNetWdgMsg       cMsg;
    CVNetWdgAckMsg  cAckMsg;

    cMsg.deserialize(message.data);
    cMsg.Print();

    int64 vmid = cMsg.m_qwVmId;

    MessageOption option(message.option.sender(), EV_VNETLIB_WDG_TIMER_OPER_ACK, 0, 0);
    cAckMsg.m_cMid = cMsg.m_cMid;
    cAckMsg.m_cVnaWdgMid = cMsg.m_cVnaWdgMid;
    cAckMsg.state = WDG_SUCCESS;
    cAckMsg.m_nOperator = EN_START_WDGTIMER_ACK;
    cAckMsg.m_qwVmId = cMsg.m_qwVmId;
    cAckMsg.action_id = cMsg.action_id;

    if (0 != pthread_mutex_lock(&m_mutex_timer))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::StartWDGTimer: pthread_mutex_lock(0x%x) failed.\n", &m_mutex_timer);
        cAckMsg.state = ERROR_WDG_LOCK;
        cAckMsg.progress = 0;
        cAckMsg.Print();
        return mu->Send(cAckMsg, option);
    }

    vector<T_WdTimerInfo>::iterator itr = m_vecWDGTM.begin();
    for ( ; itr != m_vecWDGTM.end(); ++itr)
    {
        if (vmid == itr->vmid)
        {
            TimeOut  tTmOut;
            //单位为秒
            //虚拟机启动统一使用tecs配置的时间
            tTmOut.duration = itr->tecstime;
            tTmOut.msgid = EV_VNET_WDG_TIMER;
            tTmOut.type = LOOP_TIMER;
            tTmOut.arg = itr->vmid;

            //reset timer
            if (SUCCESS != mu->SetTimer(itr->tTimerID, tTmOut))
            {
                VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::StartWDGTimer: set timer(id:%d) failed.\n", vmid);
                if (0 != pthread_mutex_unlock(&m_mutex_timer))
                {
                    VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::StartWDGTimer: pthread_mutex_unlock(0x%x) failed.\n", &m_mutex_timer);
                }

                cAckMsg.state = ERROR_WDG_TIMER;
                cAckMsg.progress = 0;
                cAckMsg.Print();
                return mu->Send(cAckMsg, option);
            }

            //重设tecs侧启动定时器标志
            itr->tecsrunflag = 1;

            //写文件
            string vm_mac;
            uint32 vm_timerout;
            int32 vm_state;
            if(0 != GetVMInfoById(vmid, vm_mac, vm_timerout, vm_state))
            {
                VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::StartWDGTimer:GetVMInfoById failed.\n");
                if (0 != pthread_mutex_unlock(&m_mutex_timer))
                {
                    VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::StartWDGTimer: pthread_mutex_unlock(0x%x) failed.\n", &m_mutex_timer);
                }
                cAckMsg.state = ERROR_WDG_MAC;
                cAckMsg.progress = 0;
                cAckMsg.Print();
                return mu->Send(cAckMsg, option);
            }

            DelWdgFile(vm_mac);
            WriteWdgFile(vm_mac, vmid, vm_timerout, 0);
            			
            if (0 != pthread_mutex_unlock(&m_mutex_timer))
            {
                VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::StartWDGTimer: pthread_mutex_unlock(0x%x) failed.\n", &m_mutex_timer);
                cAckMsg.state = ERROR_WDG_LOCK;
                cAckMsg.progress = 0;
                cAckMsg.Print();
                return mu->Send(cAckMsg, option);
            }

            cAckMsg.state = WDG_SUCCESS;
            cAckMsg.progress = 100;
            cAckMsg.Print();
            return mu->Send(cAckMsg, option);
        }//end if
    }//end for

    if (0 != pthread_mutex_unlock(&m_mutex_timer))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::StartWDGTimer: pthread_mutex_unlock(0x%x) failed.\n", &m_mutex_timer);
        cAckMsg.state = ERROR_WDG_LOCK;
        cAckMsg.progress = 0;
        cAckMsg.Print();
        return mu->Send(cAckMsg, option);
    }

    cAckMsg.state = ERROR_WDG_STARTTIMER;
    cAckMsg.progress = 0;
    cAckMsg.Print();
    return mu->Send(cAckMsg, option);
}

/**
虚拟机操作：停止、重启调用
**/
int32 CVNetWatchDog::StopWDGTimer(const MessageFrame& message)
{
    CVNetWdgMsg       cMsg;
    CVNetWdgAckMsg  cAckMsg;

    cMsg.deserialize(message.data);
    cMsg.Print();

    int64 vmid = cMsg.m_qwVmId;

    MessageOption option(message.option.sender(), EV_VNETLIB_WDG_TIMER_OPER_ACK, 0, 0);
    cAckMsg.m_cMid = cMsg.m_cMid;
    cAckMsg.m_cVnaWdgMid = cMsg.m_cVnaWdgMid;
    cAckMsg.state = WDG_SUCCESS;
    cAckMsg.m_nOperator = EN_STOP_WDGTIMER_ACK;
    cAckMsg.m_qwVmId = cMsg.m_qwVmId;
    cAckMsg.action_id = cMsg.action_id;

    if (0 != pthread_mutex_lock(&m_mutex_timer))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::StopWDGTimer: pthread_mutex_lock(0x%x) failed.\n", &m_mutex_timer);
        cAckMsg.state = ERROR_WDG_LOCK;
        cAckMsg.progress = 0;
        cAckMsg.Print();
        return mu->Send(cAckMsg, option);
    }

    vector<T_WdTimerInfo>::iterator itr = m_vecWDGTM.begin();
    for ( ; itr != m_vecWDGTM.end(); ++itr)
    {
        if (vmid == itr->vmid)
        {
            //已经为0，表示已经停止定时器，如继续设置返回失败
            if (0 == itr->tecsrunflag)
            {
                if (0 != pthread_mutex_unlock(&m_mutex_timer))
                {
                    VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::StopWDGTimer: pthread_mutex_unlock(0x%x) failed.\n", &m_mutex_timer);
                }

                cAckMsg.state = WDG_SUCCESS;
                cAckMsg.progress = 100;
                cAckMsg.Print();
                return mu->Send(cAckMsg, option);
            }

            //stop timer
            if (SUCCESS != mu->StopTimer(itr->tTimerID))
            {
                VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::StopWDGTimer:stop timer(id:%d) failed.\n", vmid);
                if (0 != pthread_mutex_unlock(&m_mutex_timer))
                {
                    VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::StopWDGTimer: pthread_mutex_unlock(0x%x) failed.\n", &m_mutex_timer);
                }
                cAckMsg.state = ERROR_WDG_TIMER;
                cAckMsg.progress = 0;
                cAckMsg.Print();
                return mu->Send(cAckMsg, option);
            }

            itr->tecsrunflag = 0;

            //写文件
            string vm_mac;
            uint32 vm_timerout;
            int32 vm_state;
            if(0 != GetVMInfoById(vmid, vm_mac, vm_timerout, vm_state))
            {
                VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::StopWDGTimer:GetVMInfoById failed.\n");
                if (0 != pthread_mutex_unlock(&m_mutex_timer))
                {
                    VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::StopWDGTimer: pthread_mutex_unlock(0x%x) failed.\n", &m_mutex_timer);
                }
                cAckMsg.state = ERROR_WDG_MAC;
                cAckMsg.progress = 0;
                cAckMsg.Print();
                return mu->Send(cAckMsg, option);
            }

            DelWdgFile(vm_mac);
            WriteWdgFile(vm_mac, vmid, vm_timerout, 1);			

            if (0 != pthread_mutex_unlock(&m_mutex_timer))
            {
                VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::StopWDGTimer: pthread_mutex_unlock(0x%x) failed.\n", &m_mutex_timer);
                cAckMsg.state = ERROR_WDG_LOCK;
                cAckMsg.progress = 0;
                cAckMsg.Print();
                return mu->Send(cAckMsg, option);
            }

            cAckMsg.state = WDG_SUCCESS;
            cAckMsg.progress = 100;
            cAckMsg.Print();
            return mu->Send(cAckMsg, option);
        }//end if
    }//end for

    if (0 != pthread_mutex_unlock(&m_mutex_timer))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::StopWDGTimer: pthread_mutex_unlock(0x%x) failed.\n", &m_mutex_timer);
        cAckMsg.state = ERROR_WDG_LOCK;
        cAckMsg.progress = 0;
        cAckMsg.Print();
        return mu->Send(cAckMsg, option);
    }

    cAckMsg.state = WDG_SUCCESS;
    cAckMsg.progress = 100;
    cAckMsg.Print();
    return mu->Send(cAckMsg, option);
}

/**
虚拟机操作：重启调用
**/
int32 CVNetWatchDog::StopRRTimer(const MessageFrame& message)
{
    CVNetWdgNotifyVMRRAckMsg       cMsg;
    cMsg.deserialize(message.data);

    int64 vmid = cMsg.m_qwVmId;

    cout<<"StopRRTimer[Stop]:"<<vmid<<" State:"<<cMsg.state<<" Progress:"<<cMsg.progress<<endl;

    //未成功直接返回
    if ((cMsg.state != SUCCESS) || (cMsg.progress != 100))
    {
        return 0;
    }

    if (0 != pthread_mutex_lock(&m_mutex_timer))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::StopRRTimer: pthread_mutex_lock(0x%x) failed.\n", &m_mutex_timer);
        return -1;
    }

    vector<T_WdTimerInfo>::iterator itr = m_vecWDGTM.begin();
    for ( ; itr != m_vecWDGTM.end(); ++itr)
    {
        if (vmid == itr->vmid)
        {
            //已经为0，表示已经停止定时器，如继续设置返回失败
            if (0 == itr->tecsrunflag)
            {
                if (0 != pthread_mutex_unlock(&m_mutex_timer))
                {
                    VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::StopRRTimer: pthread_mutex_unlock(0x%x) failed.\n", &m_mutex_timer);
                }
                return 0;
            }

            //stop timer
            if (SUCCESS != mu->StopTimer(itr->tTimerID))
            {
                VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::StopRRTimer:stop timer(id:%d) failed.\n", vmid);
                if (0 != pthread_mutex_unlock(&m_mutex_timer))
                {
                    VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::StopRRTimer: pthread_mutex_unlock(0x%x) failed.\n", &m_mutex_timer);
                }
                return -1;
            }

            itr->tecsrunflag = 0;

            if (0 != pthread_mutex_unlock(&m_mutex_timer))
            {
                VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::StopRRTimer: pthread_mutex_unlock(0x%x) failed.\n", &m_mutex_timer);
                return -1;
            }
            return 0;
        }//end if
    }//end for

    if (0 != pthread_mutex_unlock(&m_mutex_timer))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::StopRRTimer: pthread_mutex_unlock(0x%x) failed.\n", &m_mutex_timer);
        return -1;
    }

    return 0;
}

//alloc mac num , if ret is 0, failed.
uint32 CVNetWatchDog::AllocMacNum(void)
{
    uint32 dwMacNum = 0;
    BYTE *pucData = NULL;
    uint32 i = 0;
    BYTE j = 0;

    if (NULL == m_pMacPool)
    {
        return 0;
    }

    if (0 != pthread_mutex_lock(&m_mutex_mac))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::AllocMacNum: pthread_mutex_lock(0x%x) failed.\n", &m_mutex_mac);
        return 0;
    }

    for (i = 0, pucData = m_pMacPool; i < MAC_SIZE; i++, pucData++)
    {
        if (*pucData != 0xFF)
        {
            BYTE ucTmp = 0x80;
            for (j = 0; j < 8; j++)
            {
                if (0 == (ucTmp & (*pucData)))
                {
                    *pucData = (*pucData) | ucTmp;
                    break;
                }
                ucTmp = ucTmp >> 1;
            }
            dwMacNum = i * 8 + j + 1;
            break;
        }
    }

    if (0 != pthread_mutex_unlock(&m_mutex_mac))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::AllocMacNum: pthread_mutex_unlock(0x%x) failed.\n", &m_mutex_mac);
        //回收分配的MAC地址
        FreeMacNum(dwMacNum);
        return 0;
    }

    return dwMacNum;
}

//free mac num
//撤销部署慢时，分配MAC可能跳过去
int32 CVNetWatchDog::FreeMacNum(uint32 dwMacNum)
{
    BYTE *pucData = NULL;
    uint32 i = 0;
    BYTE j = 0;
    uint32 uiIdx = 0;
    BYTE ucMask = 0;

    if (NULL == m_pMacPool)
    {
        return -1;
    }

    if (0 == dwMacNum)
    {
        return -1;
    }

    uiIdx = dwMacNum - 1;
    i = uiIdx >> 3;
    j = uiIdx % 8;
    if (i >= MAC_SIZE)
    {
        return -1;
    }

    if (0 != pthread_mutex_lock(&m_mutex_mac))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::FreeMacNum: pthread_mutex_lock(0x%x) failed.\n", &m_mutex_mac);
        return -1;
    }

    pucData = m_pMacPool;
    pucData = pucData + i;

    ucMask = 0x80 >> j;
    ucMask = ~ucMask;
    *pucData = (*pucData) & ucMask;

    if (0 != pthread_mutex_unlock(&m_mutex_mac))
    {
        VNET_LOG(VNET_LOG_WARN, "CVNetWatchDog::FreeMacNum: pthread_mutex_unlock(0x%x) failed.\n", &m_mutex_mac);
        return -1;
    }

    return 0;
}

//set mac num
//HC重启后，把以存在的虚拟机申请的MAC再重新申请一遍
int32 CVNetWatchDog::SetMacNum(uint32 dwMacNum)
{
    BYTE *pucData = NULL;
    uint32 i = 0;
    BYTE j = 0;
    uint32 uiIdx = 0;
    BYTE ucMask = 0;

    if (NULL == m_pMacPool)
    {
        return -1;
    }

    if (0 == dwMacNum)
    {
        return -1;
    }

    uiIdx = dwMacNum - 1;
    i = uiIdx >> 3;
    j = uiIdx % 8;
    if (i >= MAC_SIZE)
    {
        return -1;
    }

    if (0 != pthread_mutex_lock(&m_mutex_mac))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::SetMacNum: pthread_mutex_lock(0x%x) failed.\n", &m_mutex_mac);
        return -1;
    }

    pucData = m_pMacPool;
    pucData = pucData + i;

    ucMask = 0x80 >> j;
    *pucData = (*pucData) | ucMask;

    if (0 != pthread_mutex_unlock(&m_mutex_mac))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::SetMacNum: pthread_mutex_unlock(0x%x) failed.\n", &m_mutex_mac);
        return -1;
    }

    return 0;
}

//把MAC  VMID等写到内存文件
int32 CVNetWatchDog::WriteWdgFile(const string &mac, const int64 &nVMID, const int64 &nTimerOut, const int32 &nState)
{
    if (0 != access(WDG_DIR, 0))
    {
        system("mkdir -p "WDG_DIR);
    }

    if (0 != pthread_mutex_lock(&m_mutex_wdgfile))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::WriteWdgFile: pthread_mutex_lock(0x%x) failed.\n", &m_mutex_wdgfile);
        return -1;
    }

    ostringstream file;
    file << WDG_DIR <<"wdg";
    ofstream fout(file.str().c_str(), ios::app);
    fout <<mac<<" "<<nVMID<<" "<<nTimerOut<<" "<<nState<<endl;

    if (0 != pthread_mutex_unlock(&m_mutex_wdgfile))
    {
        VNET_LOG(VNET_LOG_WARN, "CVNetWatchDog::WriteWdgFile: pthread_mutex_unlock(0x%x) failed.\n", &m_mutex_wdgfile);
        return -1;
    }

    return 0;
}

//删除内存文件中的MAC  VMID
int32 CVNetWatchDog::DelWdgFile(const string &mac)
{
    string str;
    string file_mac;
    ostringstream file;

    if (0 != access(WDG_DIR, 0))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::DelWdgFile file not exit.\n");
        return -1;
    }

    if (0 != pthread_mutex_lock(&m_mutex_wdgfile))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::DelWdgFile: pthread_mutex_lock(0x%x) failed.\n", &m_mutex_wdgfile);
        return -1;
    }

    file << WDG_DIR <<"wdg";
    ifstream fin(file.str().c_str());

    ofstream fout("/dev/shm/tecs/watchdog/temp.txt", ios::app); //创建临时文件，这种方法数据比较大可能效率不高，考虑优化

    while (getline(fin, str))
    {
        istringstream is(str);
        is>>file_mac;
        if (0 == mac.compare(file_mac))
        {
            continue;
        }
        //没找到的，保存到临时文件中
        fout << str <<endl;
    }

    fin.close();
    fout.close();
    remove(file.str().c_str()); //删除原来的文件
    rename("/dev/shm/tecs/watchdog/temp.txt", file.str().c_str());//改下名字

    if (0 != pthread_mutex_unlock(&m_mutex_wdgfile))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::DelWdgFile: pthread_mutex_unlock(0x%x) failed.\n", &m_mutex_wdgfile);
        return -1;
    }

    return 0;
}

//从内存文件中读MAC VMID
int32 CVNetWatchDog::ReadWdgFile(void)
{
    string str;
    BYTE ucMac[6] = {0};
    uint32 dwTmp = 0;
    string mac;
    string vmid;
    string state;
    string timerout;

    stringstream strStream1;
    stringstream strStream2;
    stringstream strStream3;

    if (0 != access(WDG_DIR, 0))
    {
        return -1;
    }

    //以输入的方式打开文件
    ostringstream file;
    file << WDG_DIR << "wdg";
    ifstream fin(file.str().c_str());

    if (!fin)
    {
        VNET_LOG(VNET_LOG_DEBUG, "CVNetWatchDog::ReadWdgFile: get file failed\n");
        return -1;
    }

    if (0 != pthread_mutex_lock(&m_mutex_wdgfile))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::ReadWdgFile: pthread_mutex_lock(0x%x) failed.\n", &m_mutex_wdgfile);
        return -1;
    }

    //逐行读取
    while (getline(fin, str))
    {
        istringstream is(str);
        is>>mac>>vmid>>timerout>>state;

        if (!vnet_mac_str_to_byte(mac, ucMac))
        {
            if ((ucMac[0] == 0x00) && (ucMac[1] == 0x00) && (ucMac[2] == 0x01))
            {
                dwTmp = ((ucMac[4]<<8) | ucMac[5]);
                SetMacNum(dwTmp);

                //重启之后根据文件判断是否需要启动对应vm的看门狗
                strStream1.clear();
                strStream2.clear();
                strStream3.clear();

                uint32 retvar;
                int64 retvar1;
                int32 retvar2;
                strStream1 << timerout;
                strStream1 >>retvar;
                strStream2 << vmid;
                strStream2 >>retvar1;

                strStream3 << state;
                strStream3 >>retvar2;

                cout << "read file ......:" << vmid << endl;
                cout << "read var ......:" << retvar1 << endl;

                AddWDGTimer(retvar, ucMac, retvar1, "", retvar2);
            }
        }
    }

    if (0 != pthread_mutex_unlock(&m_mutex_wdgfile))
    {
        VNET_LOG(VNET_LOG_WARN, "CVNetWatchDog::ReadWdgFile: pthread_mutex_unlock(0x%x) failed.\n", &m_mutex_wdgfile);
        return -1;
    }

    return 0;
}

//从内存文件中读MAC VMID
int32 CVNetWatchDog::GetVMMacById(const int64 &vm_id, string &mac)
{
    string file_mac;
    string vmid;
    string timerout;
    string state;
    string str;
    stringstream strStream;

    if (0 != access(WDG_DIR, 0))
    {
        return -1;
    }

    //以输入的方式打开文件
    ostringstream file;
    file << WDG_DIR << "wdg";
    ifstream fin(file.str().c_str());

    if (!fin)
    {
        //VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::GetVMInfoByName: get vm info failed\n");
        return -1;
    }

    if (0 != pthread_mutex_lock(&m_mutex_wdgfile))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::GetVMInfoByName: pthread_mutex_lock(0x%x) failed.\n", &m_mutex_wdgfile);
        return -1;
    }

    //逐行读取
    while (getline(fin, str))
    {
        istringstream is(str);
        is>>file_mac>>vmid>>timerout>>state;

        strStream.clear();
        int64 retvar;
        strStream << vmid;
        strStream >>retvar;

        if (retvar == vm_id)
        {
            mac = file_mac;
            if (0 != pthread_mutex_unlock(&m_mutex_wdgfile))
            {
                VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::GetVMInfoByName: pthread_mutex_unlock(0x%x) failed.\n", &m_mutex_wdgfile);
            }
            return 0;
        }
    }

    if (0 != pthread_mutex_unlock(&m_mutex_wdgfile))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::GetVMInfoByName: pthread_mutex_unlock(0x%x) failed.\n", &m_mutex_wdgfile);
        return -1;
    }

    return -1;
}

//从内存文件中读VM Info
int32 CVNetWatchDog::GetVMInfoById(const int64 &vm_id, string &vm_mac, uint32 &vm_timerout, int32 &vm_state)
{
    string file_mac;
    string vmid;
    string timerout;
    string state;
    string str;
    stringstream strStream;	
    stringstream strStream1;
    stringstream strStream2;	
	
    if (0 != access(WDG_DIR, 0))
    {
        return -1;
    }

    //以输入的方式打开文件
    ostringstream file;
    file << WDG_DIR << "wdg";
    ifstream fin(file.str().c_str());

    if (!fin)
    {
        //VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::GetVMInfoByName: get vm info failed\n");
        return -1;
    }

    if (0 != pthread_mutex_lock(&m_mutex_wdgfile))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::GetVMInfoByName: pthread_mutex_lock(0x%x) failed.\n", &m_mutex_wdgfile);
        return -1;
    }

    //逐行读取
    while (getline(fin, str))
    {
        istringstream is(str);
        is>>file_mac>>vmid>>timerout>>state;

        strStream.clear();
        int64 retvar;
        strStream << vmid;
        strStream >>retvar;

        strStream1.clear();
        int64 retvar1;
        strStream1 << timerout;
        strStream1 >>retvar1;

        strStream2.clear();
        int64 retvar2;
        strStream2 << state;
        strStream2 >>retvar2;

        if (retvar == vm_id)
        {
            vm_mac = file_mac;
            vm_timerout = retvar1;
            vm_state	= retvar2	;
            if (0 != pthread_mutex_unlock(&m_mutex_wdgfile))
            {
                VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::GetVMInfoByName: pthread_mutex_unlock(0x%x) failed.\n", &m_mutex_wdgfile);
            }
            return 0;
        }
    }

    if (0 != pthread_mutex_unlock(&m_mutex_wdgfile))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::GetVMInfoByName: pthread_mutex_unlock(0x%x) failed.\n", &m_mutex_wdgfile);
        return -1;
    }

    return -1;
}


int32 CVNetWatchDog::CreateWdgBridge(void)
{
    string strBrName = "";
    ostringstream oss;
    oss.str("");
    oss << "br_wd";
    strBrName.assign(oss.str());

    if (0 != VNetCreateBridge(strBrName))
    {
        return -1;
    }

    //配置固定MAC地址
    oss.str("");
    oss << "ifconfig br_wd down";
    if (0 != vnet_system(oss.str()))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::CreateWdgBridge: config watchdog bridge br_wd down failed\n");
        return -1;
    }

    oss.str("");
    oss << "ifconfig br_wd hw ether 00:00:01:00:00:00";
    if (0 != vnet_system(oss.str()))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::CreateWdgBridge: config watchdog bridge ether address failed\n");
        return -1;
    }

    oss.str("");
    oss << "ifconfig br_wd up";
    if (0 != vnet_system(oss.str()))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::CreateWdgBridge: config watchdog bridge br_wd up failed\n");
        return -1;
    }

    return 0;
}

void* WdgThreadProcess(void *arg)
{
    int32 nRet = 0;
    BYTE ucMac[6];
    BYTE *pPkt = NULL;
    int32 dwLen = 0;
    uint32 timeout = 0;
    int socketflag = 0;
    int wdgbridgeflag = 0;

    while (1)
    {
        pPkt = NULL;
        dwLen = 0;

        //创建看门狗网桥
        if (0 == wdgbridgeflag)
        {
            CVNetWatchDog *pNetWDG = CVNetWatchDog::GetInstance();
            if (0 != pNetWDG->CreateWdgBridge())
            {
                Delay(1000);
                continue;
            }

            wdgbridgeflag = 1;
        }

        nRet = tecs_wd_rcv(&pPkt, &dwLen, &socketflag);
        if (nRet <= 0)
        {
            //例如未启用看门狗时，未接收到报文，线程优先级过高，担心CPU利用率冲高，延时一下
            Delay(1000);

            if (g_ucPktRcvFlag)
            {
                cout << "wd rx is NULL !!" <<endl;
            }

            continue;
        }

        if (NULL != pPkt)
        {
            if (g_ucPktRcvFlag)
            {
                cout << "DST MAC: " << (*(BYTE*)(pPkt+0)) << ":" << (*(BYTE*)(pPkt+1))  << ":"<<(*(BYTE*)(pPkt+2))  << ":" << (*(BYTE*)(pPkt+3))  <<":" <<(*(BYTE*)(pPkt+4))  << ":" << (*(BYTE*)(pPkt+5))  <<endl;
                cout << "SRC MAC: " << (*(BYTE*)(pPkt+6))  << ":" << (*(BYTE*)(pPkt+7))  << ":"<<(*(BYTE*)(pPkt+8))  << ":" << (*(BYTE*)(pPkt+9))  <<":" <<(*(BYTE*)(pPkt+10))  << ":" << (*(BYTE*)(pPkt+11))  <<endl;
                cout << "PKT TYPE: " << (*(BYTE*)(pPkt+12))  << ":" << (*(BYTE*)(pPkt+13))  <<endl;
                cout << "TIMEOUT: " << rawsocket_ntohl(*((unsigned int*)(pPkt+14))) <<endl;
                //printf("\n MAC :  %d    %d   %d   %d", pPkt[0], pPkt[1],pPkt[2],rawsocket_ntohl(*((unsigned int*)(pPkt+14))));
            }

            /**
            1.外层统计接收报文数目，如按照源MAC计数，观察是否接收到喂狗报文
            2.内层设置定时器时，设置喂狗次数，观察两者数目
            3.下面判断条件感觉不够好
            **/
            if ((pPkt[0] == 0x00) && (pPkt[1] == 0x00) && (pPkt[2] == 0x01) && (pPkt[12] == 0x01) &&(pPkt[13] == 0x01))
            {
                //拷贝源MAC
                memcpy(ucMac, pPkt + 6, 6);
                //获取看门狗定时器的timeout
                timeout = rawsocket_ntohl(*((unsigned int*)(pPkt+14)));

                //每次都获取效率不高，保险起见都获取一下
                CVNetWatchDog *pNetWDG = CVNetWatchDog::GetInstance();
                if (NULL != pNetWDG)
                {
                    //考虑误差加2S是否可行?
                    pNetWDG->SetWDGTimer(timeout + WDG_TIMEOUT_ERROR, ucMac);
                    //添加调试记录?
                }
                else
                {
                    VNET_LOG(VNET_LOG_INFO, "WdgThreadProcess get wd instance failed! \n");
                }
            }

            //释放报文
            free(pPkt);
        }
    }

    return NULL;
}

STATUS CVNetWatchDog::Receive(const MessageFrame& message)
{
    return mu->Receive(message);
}

int32 CVNetWatchDog::InitWDG()
{
    mu->set_state(S_Startup);
    m_nStateMachines = VNET_STATE_MACHINE_START;

    //初始化锁
    if (0 != pthread_mutex_init(&m_mutex, NULL))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::InitWDG: call pthread_mutex_init for thread failed.\n");
        return -1;
    }

    if (0 != pthread_mutex_init(&m_mutex_timer, NULL))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::InitWDG: call pthread_mutex_init for timer failed.\n");
        return -1;
    }

    if (0 != pthread_mutex_init(&m_mutex_mac, NULL))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::InitWDG: call pthread_mutex_init failed for mac alloc.\n");
        return -1;
    }

    if (0 != pthread_mutex_init(&m_mutex_wdgfile, NULL))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::InitWDG: call pthread_mutex_init failed for wdgfile process.\n");
        return -1;
    }

    //if (SUCCESS != GetRegisteredSystem(m_strCCName))
    //{
    //VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::InitWDG: call GetRegisteredSystem failed.\n");
    //return -1;
    //}

    //假设虚拟机分配数目小于0xFFFF
    m_pMacPool = (BYTE*)malloc(MAC_SIZE);
    if (NULL == m_pMacPool)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::InitWDG: malloc failed.\n");
        return -1;
    }
    memset(m_pMacPool, 0, MAC_SIZE);

    //检查文件是否存在，不存在则创建一个
    if (0 != access(WDG_DIR, 0))
    {
        system("mkdir -p "WDG_DIR);
    }
    else
    {
        //读取文件，把已经存在的MAC再分配出来
        ReadWdgFile();
    }

    //dom0上启一线程
    //线程优先级设置多少合适?
    if (INVALID_THREAD_ID == StartThreadEx("vnet_wdg", 35, 0, WdgThreadProcess, (void *)this))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::InitWDG: create wdg thread failed.\n");
        free(m_pMacPool);
        return -1;
    }

    VNET_LOG(VNET_LOG_DEBUG, "Init InitWDG successfully.\n");
    return 0;
}

void CVNetWatchDog::MessageEntry(const MessageFrame& message)
{
    TimeOut  tTmOut;
    if (VNET_TRUE == m_bOpenDbgInf)
    {
        cout << "StateMachines: " << mu->get_state() <<" msg id: " << message.option.id() << endl;
    }
    switch (mu->get_state())
    {
    case S_Startup:
    {
        switch (message.option.id())
        {
        case EV_VNET_POWER_ON:
        {
            if (0 != InitWDG())
            {
                SkyExit(-1,"CVNetWatchDog power on InitWDG failed!");
            }

            m_nStateMachines = VNET_STATE_MACHINE_WORK;
            mu->set_state(VNET_STATE_MACHINE_WORK);

            VNET_LOG(VNET_LOG_DEBUG,"CVNetWatchDog::MessageEntry: CVNetWatchDog: entering VNET_STATE_MACHINE_WORK.\n");
            break;
        }
        default:
        {
            break;
        }
        }
        break;
    }

    case S_Work:
    {
        if (0 != pthread_mutex_lock(&m_mutex))
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::MessageEntry: pthread_mutex_lock(0x%x) failed.\n",&m_mutex);
            break;
        }

        //根据收到的消息id进行处理
        switch (message.option.id())
        {
        //虚拟机定时器超时
        case EV_VNET_WDG_TIMER:
        {
            int64 vm_id  = static_cast<int64>(mu->GetTimerArg());
            VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::vm (id:%ld) watch dog time out, send message to hc to reset it.\n", vm_id);

            if (0 != pthread_mutex_lock(&m_mutex_timer))
            {
                VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::MessageEntry: pthread_mutex_lock(0x%x) failed.\n", &m_mutex_timer);
            }

            vector<T_WdTimerInfo>::iterator itr = m_vecWDGTM.begin();
            for ( ; itr != m_vecWDGTM.end(); ++itr)
            {
                if (vm_id == itr->vmid)
                {
                    //超时后，把超时时间设置成tecs配置的时间，先stop再set
                    //if(SUCCESS == mu->StopTimer(mu->GetTimerId()))
                    //{
                        TimeOut  tTmOut;
                        tTmOut.duration = itr->tecstime;
                        tTmOut.msgid = EV_VNET_WDG_TIMER;
                        tTmOut.type = LOOP_TIMER;
                        tTmOut.arg = itr->vmid;

                        if(SUCCESS != mu->SetTimer(mu->GetTimerId(), tTmOut))
                        {
                            VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::[%ld]MessageEntry SetTimer failed.\n", vm_id);
                        }
                    //}
                    //else
                    //{
                    //    VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::[%ld]MessageEntry StopTimer failed.\n", vm_id);
                    //}

                    VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::[%ld %d %d]MessageEntry OK.\n", vm_id, itr->tecstime, itr->apptime);

                    break;
                }
            }

            if (0 != pthread_mutex_unlock(&m_mutex_timer))
            {
                VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::MessageEntry: pthread_mutex_unlock(0x%x) failed.\n", &m_mutex_timer);
            }

            CVNetWdgNotifyVMRRMsg cMsg;
            cMsg.m_qwVmId = vm_id;
            cMsg.m_nOperator = EN_RR_VM;
            cMsg.action_id = GenerateUUID();

            MessageOption option(MU_VNET_LIBNET, EV_VNETLIB_NOTIFY_VM_RR, 0, 0);
            if (SUCCESS != mu->Send(cMsg,option))
            {
                VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::send RR msg failed.\n");
            }
            break;
        }
        case EV_VNETLIB_WDG_OPER:
        {
            OperWatchDog(message);
            break;
        }
        case EV_VNETLIB_WDG_TIMER_OPER:
        {
            OperWatchDogTimer(message);
            break;
        }
        case EV_VNETLIB_NOTIFY_VM_RR_ACK:
        {
            //StopRRTimer(message);
            break;
        }		
        default:
        {
            break;
        }
        }

        if (0 != pthread_mutex_unlock(&m_mutex))
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::MessageEntry: pthread_mutex_unlock(0x%x) failed.\n",&m_mutex);
        }

        break;
    }
    default:
    {
        break;
    }
    }
}

int32 CVNetWatchDog::DbgShowData(void)
{
#if 0
    if (0 != pthread_mutex_lock(&m_mutex))
    {
        cout << "pthread_mutex_lock failed." << endl;
        return -1;
    }
#endif

    //如果虚拟机过多，打印需要分页?
    vector<T_WdTimerInfo>::iterator itr = m_vecWDGTM.begin();
    for ( ; itr != m_vecWDGTM.end(); ++itr)
    {
        cout << "Timer ID: " << itr->tTimerID << endl;
        cout << "Tecs Time: " << itr->tecstime << endl;
        cout << "App Time: " << itr->apptime << endl;
        cout << "VM ID: "     << itr->vmid << endl;
        cout << "VM Name: " << itr->vm_name << endl;
        cout << "App Status: "     << itr->apprunflag << endl;
        cout << "Tecs Status: "     << itr->tecsrunflag << endl;
        cout << "Feed Cnt: " << itr->nWdFeedCnt << endl;
        /*cout << "MAC: "<< *(itr->aucMac)
               << ":"<< itr->aucMac[1] 
               << ":"<< itr->aucMac[2] 
               << ":"<< itr->aucMac[3] 
               << ":"<< itr->aucMac[4] 
               << ":"<< itr->aucMac[5] << endl;*/
        printf("MAC: %x %x %x %x %x %x", itr->aucMac[0],itr->aucMac[1],itr->aucMac[2],itr->aucMac[3] ,itr->aucMac[4] ,itr->aucMac[5]);
        cout << endl;
        cout << "---------------------------------" << endl;
    }

#if 0
    if (0 != pthread_mutex_unlock(&m_mutex))
    {
        cout << "pthread_mutex_unlock failed." << endl;
        return -1;
    }
#endif

    return 0;
}

//需要做简化处理
int32 CVNetWatchDog::DbgWdgTimer(int64 vmid, int64 operation)
{
    vector<T_WdTimerInfo>::iterator itr = m_vecWDGTM.begin();
    for ( ; itr != m_vecWDGTM.end(); ++itr)
    {
        if (itr->vmid == vmid)
        {
            if (0 == operation)
            {
                //stop timer
                if (SUCCESS == mu->StopTimer(itr->tTimerID))
                {
                    itr->tecsrunflag = 0;
                }
            }
            else
            {
                //start timer
                TimeOut  tTmOut;
                tTmOut.duration = itr->tecstime;
                tTmOut.msgid = EV_VNET_WDG_TIMER;
                tTmOut.type = LOOP_TIMER;
                tTmOut.arg = itr->vmid;
                if (SUCCESS == mu->SetTimer(itr->tTimerID, tTmOut))
                {
                    itr->tecsrunflag = 1;
                }
            }
        }
    }

    return 0;
}
}

int vnet_wdg_show_data(void)
{
    CVNetWatchDog *pWdHandle = CVNetWatchDog::GetInstance();
    if (pWdHandle)
    {
        cout << "+++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
        pWdHandle->DbgShowData();
        cout << "+++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
    }
    return 0;
}
DEFINE_DEBUG_FUNC(vnet_wdg_show_data);

int vnet_wdg_set_dbg_flag(int bSet)
{
    CVNetWatchDog *pWdHandle = CVNetWatchDog::GetInstance();
    if (pWdHandle)
    {
        string strInfo = (true == bSet) ? "Start printing dbg info. " : " Stop printing dbg info.";
        cout << strInfo << endl;
        pWdHandle->SetDbgInfFlg(bSet);
    }

    g_ucPktRcvFlag = bSet;

    return 0;
}
DEFINE_DEBUG_FUNC(vnet_wdg_set_dbg_flag);

int vnet_wdg_set_timer(int64 vmid, int64 operation)
{
    CVNetWatchDog *pWdgHandle = CVNetWatchDog::GetInstance();
    if (pWdgHandle)
    {
        pWdgHandle->DbgWdgTimer(vmid, operation);
    }

    return 0;
}
DEFINE_DEBUG_FUNC(vnet_wdg_set_timer);

void VNetTestWdgCfg()
{
    MessageUnit tempmu(TempUnitName("TestWdgCfg"));
    tempmu.Register();

    CVNetWdgMsg cMsg(8, 400, 0);
    MessageOption option(MU_VNET_LIBNET, EV_VNETLIB_WDG_OPER, 0, 0);
    tempmu.Send(cMsg,option);
    return ;
}

DEFINE_DEBUG_FUNC(VNetTestWdgCfg);

void VNetTestWdgCfg1()
{
    MessageUnit tempmu(TempUnitName("TestWdgCfg1"));
    tempmu.Register();

    CVNetWdgMsg cMsg(8, 400, 2);
    MessageOption option(MU_VNET_LIBNET, EV_VNETLIB_WDG_OPER, 0, 0);
    tempmu.Send(cMsg,option);
    return ;
}

DEFINE_DEBUG_FUNC(VNetTestWdgCfg1);

