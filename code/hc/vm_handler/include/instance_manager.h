/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：instance_manager.h
* 文件标识：
* 内容摘要：InstanceManager类的定义文件
* 当前版本：1.0
* 作    者：mhb
* 完成日期：2011年7月25日
*
* 修改记录1：
*     修改日期：2011/7/25
*     版 本 号：V1.0
*     修 改 人：mhb
*     修改内容：创建
* 修改记录2：
*     修改日期：2011/9/21
*     版 本 号：V1.0
*     修 改 人：jixp
*     修改内容：代码优化
*******************************************************************************/

#ifndef HC_INSTANCEMANAGER_H
#define HC_INSTANCEMANAGER_H

#include "message.h"
#include "message_queue.h"

#include "vm_instance.h"
#include "vm_driver.h"
#include "instance_pool.h"
#include "vm_messages.h"

using namespace std;


/**************************************************************************************
 * 功能描述:定时遍历InstancePool,                                                     *
 *      1、完成状态迁移；                                                             *
 *      2、对没有及时收到喂狗消息的Instance，通知VmHandler进行重启。                  *
 *      接收两类定时器消息，其一是5s一次的状态轮询，该定时器在初始化时设置；          *
 *      其二是喂狗超时定时器，消息参数为domid，该定时器在收到喂狗使能后设置。         *
 *************************************************************************************/
class InstanceManager : public MessageHandler
{
public:
    //析构函数
    ~InstanceManager();
    static InstanceManager *GetInstance()
    {
        if(NULL == instance)
        {
            instance = new InstanceManager();
        }
        return instance;
    };

    // 初始化
    STATUS Init();

    // 启动MU
    STATUS StartMu();

    // 消息单元入口
    void MessageEntry(const MessageFrame& message);

    void SendMoniterDate(MessageVmInfoReport &info);

    void StartMoniterTimer();

    
private:
    //构造函数
    InstanceManager();

    // 监控所有实例状态的函数和状态发送函数
    STATUS MonitorAllInstanceState(void);
    
    static InstanceManager         *instance;

    MessageUnit                    *_mu;

    VmDriver                       *_vm_driver;

    InstancePool                   *_instance_pool_ctrl;

    TIMER_ID                       _vm_moni_tid;

};


#endif // #ifndef HC_INSTANCEMANAGER_H


