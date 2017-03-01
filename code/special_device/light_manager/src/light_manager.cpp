/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：light_manager.cpp
* 文件标识：无
* 内容摘要：点灯管理进程实现文件
* 当前版本：1.0
* 作    者：颜伟
* 完成日期：2011年8月22日
*
* 修改记录1：
*     修改日期：2011/08/016
*     版 本 号：V1.0
*     修 改 人：颜伟
*     修改内容：创建
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

#define LIGHT_MANAGER_SCAN_DURATION   (2000) /*lightmanager 扫描定时器间隔*/
#define EV_LIGHT_MANAGER_SCAN_TIMEER   EV_TIMER_1 /*扫描定时号*/
#define LIGHT_PROCESS_TIME_OUT         (10)   /*lightprocess 对象超时时间*/

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
@brief 功能描述:点灯管理线程处理点灯代理线程的应答消息

@li @b 入参列表：
@verbatim
const LightInfo &info 应答消息
@endverbatim

@li @b 出参列表：
@verbatim
无
@endverbatim

@li @b 返回值列表：
@verbatim
ERROR 失败
SUCCESS 成功
@endverbatim

@li @b 接口限制：无
@li @b 其它说明：无
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
@brief 功能描述:点灯管理线程遍历本地点灯信息 选取优先级最高的 执行点灯操作

@li @b 入参列表：
@verbatim
无
@endverbatim

@li @b 出参列表：
@verbatim
无
@endverbatim

@li @b 返回值列表：
@verbatim
ERROR 失败
SUCCESS 成功
@endverbatim

@li @b 接口限制：无
@li @b 其它说明：无
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
        {/*删除超时超过10秒的lightprocess*/
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
@brief 功能描述:点灯管理线程初始化本地点灯信息链表

@li @b 入参列表：
@verbatim
无
@endverbatim

@li @b 出参列表：
@verbatim
无
@endverbatim

@li @b 返回值列表：
@verbatim
ERROR 失败
SUCCESS 成功
@endverbatim

@li @b 接口限制：无
@li @b 其它说明：无
************************************************************************/
STATUS LightManager::UpdateLightProcess(void)
{
    Lock();
    /*首先删除超时超过10秒的lightprocess*/
    time_t cur_time = time(NULL);
    list<LightProcess>::iterator it = _proc_list.begin();
    while(it != _proc_list.end())
    {
        if (LIGHT_PROCESS_TIME_OUT < (cur_time - it->_last_time))
        {
            /*erase方法返回下一个位置的迭代器*/
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
    struct dirent *ptDirent = NULL; /* readdir函数的返回值 */

    ptDir = opendir(LIGHT_AGENT_SOCKET_PATH);
    if (NULL == ptDir)
    {
        printf("opendir %s failed\n",LIGHT_AGENT_SOCKET_PATH);
        return ERROR;
    }
    while (NULL != (ptDirent = readdir(ptDir)))
    {
        /*把当前目录.，上一级目录..及隐藏文件都去掉，避免死循环遍历目录*/
        if (strncmp(ptDirent->d_name, ".", 1) == 0)
        {
            continue;
        }

        memset(acFullPath,0,sizeof(acFullPath));
        MakeFullPath(LIGHT_AGENT_SOCKET_PATH, ptDirent->d_name, acFullPath,LI_MAX_SOCKFILE_LEN);
        if (stat(acFullPath, &tStat) != 0)
        {
            continue;  //对于单个出错的文件忽略跳过
        }
        if (!S_ISSOCK(tStat.st_mode))
        {
            continue; //对于单个出错的文件忽略跳过
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
@brief 功能描述:点灯管理线程入口

@li @b 入参列表：
@verbatim
uint32 id        消息号
void * data      消息体
uint32 length    消息长度
uint32 version   消息版本号
@endverbatim

@li @b 出参列表：
@verbatim
无
@endverbatim

@li @b 返回值列表：
@verbatim
无
@endverbatim

@li @b 接口限制：无
@li @b 其它说明：无
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


