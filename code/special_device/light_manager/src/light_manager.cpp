/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�light_manager.cpp
* �ļ���ʶ����
* ����ժҪ����ƹ������ʵ���ļ�
* ��ǰ�汾��1.0
* ��    �ߣ���ΰ
* ������ڣ�2011��8��22��
*
* �޸ļ�¼1��
*     �޸����ڣ�2011/08/016
*     �� �� �ţ�V1.0
*     �� �� �ˣ���ΰ
*     �޸����ݣ�����
*******************************************************************************/
#include "event.h"
#include "light_manager.h"

static int light_manager_print_on = 0;  
DEFINE_DEBUG_VAR(light_manager_print_on);
#define light_manager_debug(fmt,arg...) \
        do \
        { \
            if(light_manager_print_on) \
            { \
                printf("[%s:%d]"fmt,__func__,__LINE__, ##arg); \
            } \
        }while(0)

#define LIGHT_MANAGER_SCAN_DURATION   (2000) /*lightmanager ɨ�趨ʱ�����*/
#define EV_LIGHT_MANAGER_SCAN_TIMEER   EV_TIMER_1 /*ɨ�趨ʱ��*/
#define LIGHT_PROCESS_TIME_OUT         (10)   /*lightprocess ����ʱʱ��*/

LightManager *LightManager::_instance = NULL;

STATUS LightManager::LightLed(const LightInfo &info)
{
    if (!info.valid())
    {
        return ERROR;
    }

    string cmd,res;
    cmd = "/usr/local/bin/ledctrl -s ";
    switch (info._type)
    {
        case LIGHT_TYPE_ACT:
            cmd += "act ";
            break;   
        case LIGHT_TYPE_HOST:
            cmd += "host ";
            break;
        default:
            return ERROR;
            break;
    }
    switch (info._state)
    {
        case LIGHT_NO_WORKING:
            cmd += "off";
            break;   
        case LIGHT_WORKING:
            cmd += "green";
            break;   
        case LIGHT_RUN_OK:
            cmd += "green_1HZ";
            break;   
        case LIGHT_NO_JOINED:
            cmd += "orange_1HZ";
            break;   
        case LIGHT_NIC_ERR:
            cmd += "red_2HZ";
            break;   
        case LIGHT_DISK_ERR:
            cmd += "red_2HZ";
            break;   
        case LIGHT_RAM_ERR:
            cmd += "red_5HZ";
            break;   
        case LIGHT_CRITICAL_ERR:
            cmd += "red_5HZ";
            break;  
        case LIGHT_STATE_MAX:
            cmd += "off";
            break; 
        default:
            return ERROR;
            break;
    }
    cmd += " 2>/dev/null";
    return RunCmd(cmd,res);
}
STATUS LightManager::PowerOn(void)
{
    UdsParam param(EV_POWER_ON, 0, 0, 0);
    return Send(GetSelfPath(), param);
}
STATUS LightManager::SendLightInfoReq(void)
{
    UdsParam param(EV_LIGHT_INFO_REQ, 0, 0, 0);
    return Send(LIGHT_AGENT_SOCKET_PATH, param);
}
/**********************************************************************
@brief ��������:��ƹ����̴߳����ƴ����̵߳�Ӧ����Ϣ

@li @b ����б�
@verbatim
const LightInfo &info Ӧ����Ϣ
@endverbatim

@li @b �����б�
@verbatim
��
@endverbatim

@li @b ����ֵ�б�
@verbatim
ERROR ʧ��
SUCCESS �ɹ�
@endverbatim

@li @b �ӿ����ƣ���
@li @b ����˵������
************************************************************************/
STATUS LightManager::RecvLightInfoAck(const LightInfo &info)
{
    Lock();
    list <LightProcess>::iterator procIt;
    for (procIt = _proc_list.begin(); procIt != _proc_list.end(); procIt++)
    {
        if (!procIt->_socket_file.compare(GetPeerPath()))
        {
            break;
        }
    }
    if (procIt != _proc_list.end())
    {
        list <LightInfo>::iterator infoIt;
        for (infoIt = procIt->_info_list.begin(); infoIt != procIt->_info_list.end(); infoIt++)
        {
            if (infoIt->_type == info._type)
            {
                break;
            }
        }
        if (infoIt != procIt->_info_list.end())
        {
            info.copy(*infoIt);
        }
        else
        {
            procIt->_info_list.push_back(info);
        }
        procIt->_last_time = time(NULL);
    }
    else
    {
        LightProcess process;
        process._socket_file.assign(GetPeerPath());
        process._info_list.push_back(info);
        process._last_time = time(NULL);
        _proc_list.push_back(process);
    }
    UnLock();
    return SUCCESS;
}
/**********************************************************************
@brief ��������:��ƹ����̱߳������ص����Ϣ ѡȡ���ȼ���ߵ� ִ�е�Ʋ���

@li @b ����б�
@verbatim
��
@endverbatim

@li @b �����б�
@verbatim
��
@endverbatim

@li @b ����ֵ�б�
@verbatim
ERROR ʧ��
SUCCESS �ɹ�
@endverbatim

@li @b �ӿ����ƣ���
@li @b ����˵������
************************************************************************/
STATUS LightManager::CheckLightProcess(void)
{
    int index = 0;
    int aiMaxPri[LIGHT_TYPE_MAX];
    LightInfo atLightInfo[LIGHT_TYPE_MAX];
    
    for (index = 0; index < LIGHT_TYPE_MAX; index++)
    {
        aiMaxPri[index] = LIGHT_INFO_MIN_PRIORITY;
        atLightInfo[index]._type = (LightType)index;
    }
    Lock();
    time_t cur_time = time(NULL);
    list <LightInfo>::iterator infoIt;
    list <LightProcess>::iterator procIt = _proc_list.begin();
    while(procIt != _proc_list.end())
    {
        if (LIGHT_PROCESS_TIME_OUT < (cur_time-procIt->_last_time))
        {/*ɾ����ʱ����10���lightprocess*/
            procIt = _proc_list.erase(procIt);
        }
        else
        {
            for (infoIt = procIt->_info_list.begin(); infoIt != procIt->_info_list.end(); infoIt++)
            {
                if (aiMaxPri[infoIt->_type] < infoIt->_priority)
                {
                    aiMaxPri[infoIt->_type] = infoIt->_priority;
                    infoIt->copy(atLightInfo[infoIt->_type]);
                }
            }
            procIt++;
        }
    }

    for (index = 0; index < LIGHT_TYPE_MAX; index++)
    {
        LightLed(atLightInfo[index]);
    }
    UnLock();
    return SUCCESS;
}
/**********************************************************************
@brief ��������:��ƹ����̳߳�ʼ�����ص����Ϣ����

@li @b ����б�
@verbatim
��
@endverbatim

@li @b �����б�
@verbatim
��
@endverbatim

@li @b ����ֵ�б�
@verbatim
ERROR ʧ��
SUCCESS �ɹ�
@endverbatim

@li @b �ӿ����ƣ���
@li @b ����˵������
************************************************************************/
STATUS LightManager::UpdateLightProcess(void)
{
    Lock();
    /*����ɾ����ʱ����10���lightprocess*/
    time_t cur_time = time(NULL);
    list<LightProcess>::iterator it = _proc_list.begin();
    while(it != _proc_list.end())
    {
        if (LIGHT_PROCESS_TIME_OUT < (cur_time - it->_last_time))
        {
            /*erase����������һ��λ�õĵ�����*/
            it = _proc_list.erase(it);
        }
        else
        {
            it++;
        }
    }
    UnLock();
    
    struct stat tStat;
    char acFullPath[LI_MAX_SOCKFILE_LEN];
    DIR *ptDir = NULL;
    struct dirent *ptDirent = NULL; /* readdir�����ķ���ֵ */

    ptDir = opendir(LIGHT_AGENT_SOCKET_PATH);
    if (NULL == ptDir)
    {
        printf("opendir %s failed\n",LIGHT_AGENT_SOCKET_PATH);
        return ERROR;
    }
    while (NULL != (ptDirent = readdir(ptDir)))
    {
        /*�ѵ�ǰĿ¼.����һ��Ŀ¼..�������ļ���ȥ����������ѭ������Ŀ¼*/
        if (strncmp(ptDirent->d_name, ".", 1) == 0)
        {
            continue;
        }

        memset(acFullPath,0,sizeof(acFullPath));
        MakeFullPath(LIGHT_AGENT_SOCKET_PATH, ptDirent->d_name, acFullPath,LI_MAX_SOCKFILE_LEN);
        if (stat(acFullPath, &tStat) != 0)
        {
            continue;  //���ڵ���������ļ���������
        }
        if (!S_ISSOCK(tStat.st_mode))
        {
            continue; //���ڵ���������ļ���������
        }
        Lock();
        for (it = _proc_list.begin(); it != _proc_list.end(); it++)
        {
            if (!it->_socket_file.compare(acFullPath))
            {
                break;
            }
        }
        if (it == _proc_list.end())
        {
            LightProcess process;
            process._last_time = cur_time;
            process._socket_file.assign(acFullPath);
            _proc_list.push_back(process);
        }
        UnLock();
    }
    closedir(ptDir);
    return SUCCESS;
}
/**********************************************************************
@brief ��������:��ƹ����߳����

@li @b ����б�
@verbatim
uint32 id        ��Ϣ��
void * data      ��Ϣ��
uint32 length    ��Ϣ����
uint32 version   ��Ϣ�汾��
@endverbatim

@li @b �����б�
@verbatim
��
@endverbatim

@li @b ����ֵ�б�
@verbatim
��
@endverbatim

@li @b �ӿ����ƣ���
@li @b ����˵������
************************************************************************/
void LightManager::MessageEntry(uint32 id, void * data, uint32 length, uint32 version)
{
    light_manager_debug("LightManager::MessageEntry recv a new message id = %u from %s\n",id,GetPeerPath());
    switch(id)
    {
        case EV_POWER_ON:
            {   
                light_manager_debug("LightManager::MessageEntry EV_POWER_ON!\n"); 
                TIMER_ID tScanTid = CreateTimer();
                TimeOut tTimeOut;
                tTimeOut.duration = LIGHT_MANAGER_SCAN_DURATION;
                tTimeOut.msgid = EV_LIGHT_MANAGER_SCAN_TIMEER;
                tTimeOut.type = LOOP_TIMER;
                tTimeOut.arg = 0;
                SetTimer(tScanTid,tTimeOut);     
                UpdateLightProcess();
                SendLightInfoReq(); 
            }
            break;
    
        case EV_LIGHT_MANAGER_SCAN_TIMEER:
            {
                light_manager_debug("LightManager::MessageEntry EV_LIGHT_MANAGER_SCAN_TIMEER!\n"); 
                SendLightInfoReq();
                CheckLightProcess();
            }
            break;
            
        case EV_LIGHT_INFO_ACK:
            {
                light_manager_debug("LightManager::MessageEntry EV_LIGHT_INFO_ACK!\n");
                RecvLightInfoAck(*((LightInfo *)data));
                CheckLightProcess();
            }
            break;
            
        default:
            break;
    }
}
void DbgShowLightManagerInfo(void)
{
    LightManager *plm = LightManager::GetInstance();
    if(!plm)
    {
        printf("LightAgent::GetInstance Failed\n");
        return;
    }
    int proc_count = 0;
    list<LightProcess> proclist;
    list<LightProcess>::iterator procIt;
    list<LightInfo>::iterator infoIt;    
    plm->get_proc_list(proclist);
    for (procIt = proclist.begin(); procIt != proclist.end(); procIt++)
    {
        cout<<"No."<<proc_count<<" Process"<<endl;
        cout<<"socket file:"<<procIt->_socket_file<<",last time:"<<procIt->_last_time<<endl;
        for (infoIt = procIt->_info_list.begin(); infoIt != procIt->_info_list.end(); infoIt++)
        {
            infoIt->print();
        }
        proc_count++;
    }
    if (0 != proc_count)
    {
        cout<<"Total "<<proc_count<<" Process in List"<<endl;
    }
    else
    {
        cout<<"Process List Empty"<<endl;
    }
    return;
}
DEFINE_DEBUG_FUNC(DbgShowLightManagerInfo);


