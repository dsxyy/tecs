/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：sky.cpp
* 文件标识：见配置管理计划书
* 内容摘要：sky初始化接口实现文件
* 当前版本：1.0
* 作    者：张文剑
* 完成日期：2011年8月16日
*
* 修改记录1：
*     修改日期：2011/07/01
*     版 本 号：V1.0
*     修 改 人：张文剑
*     修改内容：创建
*******************************************************************************/
#include "sky.h"
#include "message_agent.h"
static bool sky_init_flag = false;
STATUS SkyInit(const SkyConfig& config)
{
    STATUS ret;
    //检查重复初始化操作
    if(sky_init_flag)
    {
        printf("Error! SkyInit is called again!\n");
        return ERROR_DUPLICATED_OP;
    }

    //初始化异常处理
    ret = ExcInit(config.exccfg);
    if(SUCCESS != ret)
    {
        printf("ExcInit failed! ret = %d\n",ret);
        return ret;
    }

    //启动定时器管理模块
    ret = TimerInit(config.timecfg);
    if(SUCCESS != ret)
    {
        printf("Timer failed! ret = %d\n",ret);
        return ret;
    }

    //建立与消息服务器的连接  
    if(config.messaging.application.empty() != true &&
       config.messaging.process.empty() != true &&
       config.messaging.connects.empty() != true)
    {
        if(false == MessageAgent::CreateInstance())
        {
            printf("failed to create message agent instance!\n");
            return ERROR;
        }

        MessageAgent *pagent = MessageAgent::GetInstance();
        pagent->SetCommId(config.messaging.application,config.messaging.process);

        for (vector<ConnectConfig>::const_iterator it = config.messaging.connects.begin(); 
             it != config.messaging.connects.end(); ++it)
        {
            if (it->broker_url.empty())
            {
                continue;
            }
            
            ostringstream broker;
            broker << it->broker_url << ":" << it->broker_port;
            printf("connecting to message broker %s,option=%s,require_ack=%d...\n",
                broker.str().c_str(),
                it->broker_option.c_str(),
                it->enable_broker_require_ack);

            ret = pagent->Connect(broker.str(),it->broker_option,it->enable_broker_require_ack);
            if(SUCCESS != ret)
            {
                printf("Failed to connect to message broker %s! ret = %d\n",
                    it->broker_url.c_str(), ret);
                return ret;
            }
        }
        ret = pagent->Start();
        if(SUCCESS != ret)
        {
            printf("Failed to start message agent! ret = %d\n",ret);
            return ret;
        }
    }
    #if 0
    else
    {
        printf("Warning! sky not connected with message broker!\n");
        return ERROR_INVALID_ARGUMENT;
    }
    #endif

    sky_init_flag = true;
    return SUCCESS;
}


