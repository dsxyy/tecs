/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�vnet_wdg.cpp
* �ļ���ʶ��
* ����ժҪ��CVNetWatchDog��ʵ���ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�liuhx
* ������ڣ�2012��4��20��
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
//���MAC��ַ���ڴ��ļ�
#define WDG_DIR   "/dev/shm/tecs/watchdog/"
//���7200�룬��2Сʱ
#define WDG_MAX_TIMEOUT (uint32)(7200)
//ι����ʱ���,2��
#define WDG_TIMEOUT_ERROR (uint32)(2)

//WDGʵ��
CVNetWatchDog* CVNetWatchDog::s_pInstance = NULL;

//��ӡ����
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

    // ��Ϣ��Ԫ�Ĵ����ͳ�ʼ��
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

    //���Լ������ϵ���Ϣ����ʹ��Ϣ��Ԫ״̬�л�������̬
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


//���������ʱ�����
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

    //�����ÿ��Ź�
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

    //����MAC��ַ�����Ȼ�ȡһ����ֵ����ת����MAC��ַ
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

    //��Ӷ�ʱ��
    if (0 != AddWDGTimer(cMsg.m_nTimerOut, ucMac, cMsg.m_qwVmId, "", 0))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::StartWatchDog AddWDGTimer failed.\n");
        //MAC��ַ����
        FreeMacNum(dwMacNum);
        cAckMsg.state = ERROR_WDG_MAC;
        cAckMsg.progress = 0;
        cAckMsg.Print();
        return mu->Send(cAckMsg, option);
    }

    if (0 != WriteWdgFile(ss.str(), cMsg.m_qwVmId, cMsg.m_nTimerOut, 0))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::StartWatchDog WriteWdgFile failed.\n");
        //MAC��ַ����
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


//�������������ʱ�����
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

    //������Կ��Ź���nic
    if (!vnet_mac_str_to_byte(mac, ucMac))
    {
        //��Ϊ��������Կ��Ź���nic���ݽṹ���жϿ���û��Ҫ
        if ((ucMac[0] == 0x00) && (ucMac[1] == 0x00) && (ucMac[2] == 0x01))
        {
            dwTmp = ((ucMac[4]<<8) | ucMac[5]);
            //�ͷ�MAC
            FreeMacNum(dwTmp);
            //���ڴ��ļ���ɾ��
            DelWdgFile(mac);
            //ɾ����ʱ��
            if (0 != KillWDGTimer(cMsg.m_qwVmId))
            {
                VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::StopWatchDog: Kill WDGTimer failed.\n");
                //ɾ����ʱʧ��Ӱ��ͦ�󣬷���ʧ��
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

//CVNetWatchDog�������������
int32 CVNetWatchDog::AddWDGTimer(uint32 timerout, const BYTE *paucMac, int64 vmid, const string& vm_name, const int32 vm_state)
{
    T_WdTimerInfo sWdStartTimerInfo;
    TIMER_ID tWdStartTimerId;

    if (0 != pthread_mutex_lock(&m_mutex_timer))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::AddWDGTimer: pthread_mutex_lock(0x%x) failed.\n", &m_mutex_timer);
        return -1;
    }

    //ʹ��vector���������ܿ��ܲ��ߣ���ͬ
    vector<T_WdTimerInfo>::iterator itr = m_vecWDGTM.begin();
    for ( ; itr != m_vecWDGTM.end(); ++itr)
    {
        //�ظ�����жϣ�MAC��ַ����Ψһ�ԣ�VM��NAME��ID�������ж�
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

    //������ʱ��
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
    //��λΪ��
    tTmOut.duration = timerout*1000;
    tTmOut.msgid = EV_VNET_WDG_TIMER;
    tTmOut.type = LOOP_TIMER;
    tTmOut.arg = vmid;

    //���ö�ʱ��
    if (SUCCESS != mu->SetTimer(tWdStartTimerId, tTmOut))
    {
        //����timerid
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
        //��ͣ�����Ⱥ���������
        if (SUCCESS != mu->StopTimer(tWdStartTimerId))
        {
            //����timerid
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
�հ��̵߳���
����ʧ��:
1.���ݹ�����MAC��VECTOR������
2.���쳣
3.����timer�쳣
**/
int32 CVNetWatchDog::SetWDGTimer(uint32 timerout, const BYTE *paucMac)
{
    //int64 nVMID;
    string vm_name;
    string mac;

    //stringstream ss;
    // int32 iMac3, iMac4,iMac5;

    //�޶����ι��ʱ��
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
            //���ռ���
            itr->nWdFeedCnt++;

            //ֹͣι��
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

            //���tecs������ֹͣ��ʱ�������յ�ι�����Ĳ�������
            if (0 == itr->tecsrunflag)
            {
                if (0 != pthread_mutex_unlock(&m_mutex_timer))
                {
                    VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::SetWDGTimer: pthread_mutex_unlock(0x%x) failed.\n", &m_mutex_timer);
                }
                return 0;
            }

            TimeOut  tTmOut;
            //��λΪ�룬�����Լ������
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
CVNetWatchDog�����������������
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

//������������������ָ�����������
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
            //��λΪ��
            //���������ͳһʹ��tecs���õ�ʱ��
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

            //����tecs��������ʱ����־
            itr->tecsrunflag = 1;

            //д�ļ�
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
�����������ֹͣ����������
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
            //�Ѿ�Ϊ0����ʾ�Ѿ�ֹͣ��ʱ������������÷���ʧ��
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

            //д�ļ�
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
�������������������
**/
int32 CVNetWatchDog::StopRRTimer(const MessageFrame& message)
{
    CVNetWdgNotifyVMRRAckMsg       cMsg;
    cMsg.deserialize(message.data);

    int64 vmid = cMsg.m_qwVmId;

    cout<<"StopRRTimer[Stop]:"<<vmid<<" State:"<<cMsg.state<<" Progress:"<<cMsg.progress<<endl;

    //δ�ɹ�ֱ�ӷ���
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
            //�Ѿ�Ϊ0����ʾ�Ѿ�ֹͣ��ʱ������������÷���ʧ��
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
        //���շ����MAC��ַ
        FreeMacNum(dwMacNum);
        return 0;
    }

    return dwMacNum;
}

//free mac num
//����������ʱ������MAC��������ȥ
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
//HC�����󣬰��Դ��ڵ�����������MAC����������һ��
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

//��MAC  VMID��д���ڴ��ļ�
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

//ɾ���ڴ��ļ��е�MAC  VMID
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

    ofstream fout("/dev/shm/tecs/watchdog/temp.txt", ios::app); //������ʱ�ļ������ַ������ݱȽϴ����Ч�ʲ��ߣ������Ż�

    while (getline(fin, str))
    {
        istringstream is(str);
        is>>file_mac;
        if (0 == mac.compare(file_mac))
        {
            continue;
        }
        //û�ҵ��ģ����浽��ʱ�ļ���
        fout << str <<endl;
    }

    fin.close();
    fout.close();
    remove(file.str().c_str()); //ɾ��ԭ�����ļ�
    rename("/dev/shm/tecs/watchdog/temp.txt", file.str().c_str());//��������

    if (0 != pthread_mutex_unlock(&m_mutex_wdgfile))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::DelWdgFile: pthread_mutex_unlock(0x%x) failed.\n", &m_mutex_wdgfile);
        return -1;
    }

    return 0;
}

//���ڴ��ļ��ж�MAC VMID
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

    //������ķ�ʽ���ļ�
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

    //���ж�ȡ
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

                //����֮������ļ��ж��Ƿ���Ҫ������Ӧvm�Ŀ��Ź�
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

//���ڴ��ļ��ж�MAC VMID
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

    //������ķ�ʽ���ļ�
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

    //���ж�ȡ
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

//���ڴ��ļ��ж�VM Info
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

    //������ķ�ʽ���ļ�
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

    //���ж�ȡ
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

    //���ù̶�MAC��ַ
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

        //�������Ź�����
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
            //����δ���ÿ��Ź�ʱ��δ���յ����ģ��߳����ȼ����ߣ�����CPU�����ʳ�ߣ���ʱһ��
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
            1.���ͳ�ƽ��ձ�����Ŀ���簴��ԴMAC�������۲��Ƿ���յ�ι������
            2.�ڲ����ö�ʱ��ʱ������ι���������۲�������Ŀ
            3.�����ж������о�������
            **/
            if ((pPkt[0] == 0x00) && (pPkt[1] == 0x00) && (pPkt[2] == 0x01) && (pPkt[12] == 0x01) &&(pPkt[13] == 0x01))
            {
                //����ԴMAC
                memcpy(ucMac, pPkt + 6, 6);
                //��ȡ���Ź���ʱ����timeout
                timeout = rawsocket_ntohl(*((unsigned int*)(pPkt+14)));

                //ÿ�ζ���ȡЧ�ʲ��ߣ������������ȡһ��
                CVNetWatchDog *pNetWDG = CVNetWatchDog::GetInstance();
                if (NULL != pNetWDG)
                {
                    //��������2S�Ƿ����?
                    pNetWDG->SetWDGTimer(timeout + WDG_TIMEOUT_ERROR, ucMac);
                    //��ӵ��Լ�¼?
                }
                else
                {
                    VNET_LOG(VNET_LOG_INFO, "WdgThreadProcess get wd instance failed! \n");
                }
            }

            //�ͷű���
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

    //��ʼ����
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

    //���������������ĿС��0xFFFF
    m_pMacPool = (BYTE*)malloc(MAC_SIZE);
    if (NULL == m_pMacPool)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetWatchDog::InitWDG: malloc failed.\n");
        return -1;
    }
    memset(m_pMacPool, 0, MAC_SIZE);

    //����ļ��Ƿ���ڣ��������򴴽�һ��
    if (0 != access(WDG_DIR, 0))
    {
        system("mkdir -p "WDG_DIR);
    }
    else
    {
        //��ȡ�ļ������Ѿ����ڵ�MAC�ٷ������
        ReadWdgFile();
    }

    //dom0����һ�߳�
    //�߳����ȼ����ö��ٺ���?
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

        //�����յ�����Ϣid���д���
        switch (message.option.id())
        {
        //�������ʱ����ʱ
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
                    //��ʱ�󣬰ѳ�ʱʱ�����ó�tecs���õ�ʱ�䣬��stop��set
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

    //�����������࣬��ӡ��Ҫ��ҳ?
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

//��Ҫ���򻯴���
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

