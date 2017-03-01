/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：instance_manager.cpp
* 文件标识：
* 内容摘要：InstanceManager类的实现文件
* 当前版本：1.0
* 作    者：mhb
* 完成日期：2011年7月26日
*
* 修改记录1：
*     修改日期：2011/7/26
*     版 本 号：V1.0
*     修 改 人：mhb
*     修改内容：创建
* 修改记录2：
*     修改日期：2011/8/17
*     版 本 号：V1.0
*     修 改 人：jixp
*     修改内容：实现虚拟机实例状态检测及虚拟机看门狗检测
*******************************************************************************/
#include "instance_manager.h"
#include "vm_messages.h"
#include "sky.h"
#include "registered_system.h"
#include "vm_handler.h"

using namespace zte_tecs;

//虚拟机状态轮询5s 定时器
const int k_vm_status_moni_timer          = 5000;


InstanceManager *InstanceManager::instance = NULL;
/******************************************************************************/
/* InstanceManagerer :: Constructor/Destructor                                */
/******************************************************************************/
InstanceManager::InstanceManager():_mu(NULL),_vm_driver(NULL),_instance_pool_ctrl(NULL),
        _vm_moni_tid(INVALID_TIMER_ID)
{
}

InstanceManager::~InstanceManager()
{
    if (NULL != _mu)
    {
        delete(_mu);
    }
}

/****************************************************************************************
* 函数名称：Init
* 功能描述：虚拟机实例管理初始化
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch        创建
****************************************************************************************/
STATUS InstanceManager::Init()
{
    // 获取VM的驱动句柄的实例
    _vm_driver      = VmDriver::GetInstance();

    if (NULL == _vm_driver)
    {
        OutPut(SYS_ALERT, "VmDriver::GetInstance()  failed in InstanceManager:: init() !\n");
        return ERROR;
    }

    // 获取虚拟机实例池的句柄
    _instance_pool_ctrl =InstancePool::GetSelf();
    if (NULL == _instance_pool_ctrl)
    {
        OutPut(SYS_ALERT, "InstancePool GetSelf()  failed in InstanceManager:: init() !\n");
        return ERROR;
    }

    // 启动MU
    if (SUCCESS != StartMu())
    {
        OutPut(SYS_ALERT, "InstancePool StartMu  failed in InstanceManager:: init() !\n");
        return ERROR;
    }
    
    return SUCCESS;
}

/****************************************************************************************
* 函数名称：StartMu
* 功能描述：启动消息单元
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch        创建
****************************************************************************************/ 
STATUS InstanceManager::StartMu()
{
    if(_mu)
    {
        SkyAssert(false);
        return ERROR_DUPLICATED_OP;
    }
    
    // 消息单元的创建和初始化
    _mu = new MessageUnit(INSTANCE_MANAGER);
    if (!_mu)
    {
        OutPut(SYS_EMERGENCY, "Create mu %s failed!\n","InstanceManager");
        return ERROR_NO_MEMORY;
    }

    STATUS ret = _mu->SetHandler(this);
    if (SUCCESS != ret)
    {
        OutPut(SYS_EMERGENCY, "SetHandler mu %s failed! ret = %d\n","InstanceManager",ret);
        return ret;
    }

    ret = _mu->Run();
    if (SUCCESS != ret)
    {
        OutPut(SYS_EMERGENCY, "Run mu %s failed! ret = %d\n","InstanceManager",ret);
        return ret;
    }

    ret = _mu->Register();
    if (SUCCESS != ret)
    {
        OutPut(SYS_EMERGENCY, "Register mu %s failed! ret = %d\n","InstanceManager",ret);
        return ret;
    }

    //给自己发送上电消息，促使消息单元状态切换到工作态
    MessageFrame message(SkyInt32(0), EV_POWER_ON);
    ret = _mu->Receive(message);
    if (SUCCESS != ret)
    {
        SkyAssert(0);
        return ret;
    }
    _mu->Print();
    return SUCCESS;
}

/****************************************************************************************
* 函数名称：MessageEntry
* 功能描述：虚拟机实例消息入口
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/07/09    V1.0    lixch        创建
****************************************************************************************/
void InstanceManager::MessageEntry(const MessageFrame& message)
{
    switch (_mu->get_state())
    {
        case S_Startup:
        {
            switch (message.option.id())
            {
                case EV_POWER_ON:
                {
                    VmHandler *pHandler = VmHandler::GetInstance();

                    if (NULL == pHandler)
                    {
                        SkyExit(-1,"InstanceManager: Get VmHandler Instance fail!\n");
                        return;
                    }
                    
                    // 监控VmHandler是否启动成功
                    while(1)
                    {
                        Delay(2000); //延迟2S后检测
                        if (S_Work == pHandler->GetVmHandlerWorkState())
                        {
                            // 如果VmHandler启动成功，则监控线程继续往下走
                            break;
                        }
                    }
                    
                    //创建实例状态监控定时器
                    _vm_moni_tid = _mu->CreateTimer();
                    if (INVALID_TIMER_ID == _vm_moni_tid)
                    {
                        SkyExit(-1,"CreateTimer _vm_moni_tid failed in InstanceManager:: init() !\n");
                        return;
                    }
                    
                    StartMoniterTimer();
                    
                    _mu->set_state(S_Work);
                    OutPut(SYS_NOTICE, "%s power on!\n",_mu->name().c_str());
                    break;
                }
                default:
                    OutPut(SYS_NOTICE, "Ignore any message except power on!\n");
                    break;
            }
            break;
        }

        case S_Work:
        {
            switch (message.option.id())
            {
                case EV_TIMER_VM_MONI:
                {
                    //检测虚拟机状态，并上报状态
                    MonitorAllInstanceState();
                    StartMoniterTimer();
                    break;
                }

                default:
                {
                    OutPut(SYS_NOTICE, "unknown message %d !\n", message.option.id());
                    break;
                }
            }
            break;
        }

        default:
            break;
    }
}

/****************************************************************************************
* 函数名称：StartMoniterTimer
* 功能描述：启动监控定时器
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/07/12    V1.0    lixiaocheng  创建
****************************************************************************************/
void InstanceManager::StartMoniterTimer()
{
    TimeOut timeout;
    timeout.type     = RELATIVE_TIMER;
    timeout.duration = k_vm_status_moni_timer;
    timeout.msgid    = EV_TIMER_VM_MONI;
    STATUS rc = _mu->SetTimer(_vm_moni_tid, timeout);
    if (SUCCESS != rc)
    {
        OutPut(SYS_ALERT, "InstanceManager StartMoniterTimer failed rc = %d!\n", 
               rc);
        SkyAssert(false);
    }
}

/****************************************************************************************
* 函数名称：SendMoniterDate
* 功能描述：发送监控的结果
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/07/12    V1.0    lixiaocheng  创建
****************************************************************************************/
void InstanceManager::SendMoniterDate(MessageVmInfoReport &info)
{
    MID receiver;
    STATUS ret = ERROR;
    ret = GetRegisteredSystem(receiver._application);
    if (SUCCESS != ret)
    {
        OutPut(SYS_ALERT, "MonitorAllInstanceState: GetRegisteredSystem failed! ret=%d\n",ret);
    }
    
    receiver._process = PROC_CC;
    receiver._unit = MU_VM_LIFECYCLE_MANAGER;
    MessageOption option(receiver, EV_VM_INFO_REPORT, 0, 0);
    
    STATUS sdresult=ERROR;
    sdresult=_mu->Send(info, option);
    if (SUCCESS != sdresult)
    {
        OutPut(SYS_ALERT, "SendMoniterDate: send to cc failed! ret=%d\n",sdresult);
    }
}

/****************************************************************************************
* 函数名称：MonitorAllInstanceState
* 功能描述：实例状态监控
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/07/12    V1.0    lixiaocheng  创建
****************************************************************************************/
STATUS InstanceManager::MonitorAllInstanceState(void)
{
    vector<VmInstance*> *instance_pool;
    MessageVmInfoReport report;
    bool   exist_vm_op_running = false;

    instance_pool = _instance_pool_ctrl->GetInstancePool();
    if (NULL == instance_pool)
    {
        return SUCCESS;
    }
    
    // 获取虚拟机池的锁
    _instance_pool_ctrl->LockInstance_pool();

    // 获取对应的虚拟机实例
    vector<VmInstance*>::iterator it;
    for (it = instance_pool->begin(); it != instance_pool->end(); it++)
    {
        VmInfo   tempinfo;
        //获取当前要上报的信息,如果获取失败，则不上报
        // 失败的原因是，有实例里面的数据还没有刷新到
        if (!(*it)->GetInstanceReportInfo(tempinfo))
        {
            _instance_pool_ctrl->UnlockInstance_pool();
            return ERROR;
        }

        // 如果有虚拟机正在操作中,做个标记，下面当发现有虚拟机正在操作
        // 不进行虚拟机是否归属自己的扫描，防止当系统很忙的时候，下面
        // 继续调用xen接口，导致心跳发送阻塞
        if ((*it)->IsOpRunning())
        {            
            OutPut(SYS_DEBUG, "MonitorAllInstanceState vm:%s, _op_ack_end:%d, _op_result:%d.\n", 
                    (*it)->GetInstanceName().c_str(), (*it)->GetOpAckEnd(), (*it)->GetOpResult());
            exist_vm_op_running = true;
        }
        // 把要上报的信息放到vector中去
        report._vm_info.push_back(tempinfo);
    }
    
    _instance_pool_ctrl->UnlockInstance_pool();

    // 存在虚拟机正在操作，不进行多余的虚拟机扫描
    if (exist_vm_op_running)
    {
        SendMoniterDate(report);
        return SUCCESS;
    }
    
    // 然后调用libvirt接口获取当前虚拟机中所有虚拟机
    int domid[MAX_DOM_NUM];
    uint32 i;
    int dom_num = _vm_driver->ListDomains(domid);
    if(dom_num < 0)
    {
        OutPut(SYS_WARNING, "failed to list domains.\n");
    }
    else
    {
        for (i = 0; i < static_cast<uint32>(dom_num); i++)
        {
            VmInfo   tempinfo;
            // 如果domid=0表示当前是DOMAIN0，忽略
            if (0 == domid[i])
            {
                continue;
            }

            // 如果domin是已存在实例，则继续查找下一个,减少对vm_driver的调用
            // 如果ID查找不到，可能是ID变化了，那么再调用下面的名称接口，来查询
            // 看看对应的虚拟机名称是否在实例管理中
            if (NULL != _instance_pool_ctrl->GetInstanceByDomID(domid[i]))
            {
                continue;
            }

            string dom_name = _vm_driver->GetDomainNameByDomid(domid[i]);
            // 如果查找出来的虚拟机名称是空，很大可能是虚拟机不存在了
            if (dom_name.empty())
            {
                continue;
            }
        
            // 如果domin是已存在实例，则继续查找下一个
            if (NULL != _instance_pool_ctrl->GetInstanceByDomName(dom_name))
            {
                continue;
            }

            // 如果查找到的虚拟机名称是Domain-Unnamed的，则强制关机
            if (0 == dom_name.compare(0,strlen("Domain-Unnamed"), "Domain-Unnamed"))
            {
                _vm_driver->DestroyDomain(dom_name);
                OutPut(SYS_WARNING, "find the vm(%s) does not owener tecs, will destroy\n",dom_name.c_str());
                continue;
            }        
    
            // 如果查找到的虚拟机名称不是instances_x类型的，则直接重启HC
            if (0 != dom_name.compare(0,strlen("instance_"), "instance_"))
            {
                // 发现非法虚拟机，则HC直接退出，不启动
                SkyExit(-1, "MonitorAllInstanceState: find a domin without create by Tecs");
                SkyAssert(false);
                continue;
            }

            // 把不归属当前系统管理的虚拟机上报给CC
            from_string(tempinfo._vm_id, dom_name.substr(strlen("instance_")), dec);
            tempinfo._vm_dom_id = domid[i];
            tempinfo._vm_name = dom_name;
            
            OutPut(SYS_DEBUG, "Caution! % need disappear but exist.\n", dom_name.c_str());
    
            // 把填充好的消息压到消息中去
            report._vm_info.push_back(tempinfo);
        }
    }
    SendMoniterDate(report);

    return SUCCESS;
}



