/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�sky.cpp
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��sky��ʼ���ӿ�ʵ���ļ�
* ��ǰ�汾��1.0
* ��    �ߣ����Ľ�
* ������ڣ�2011��8��16��
*
* �޸ļ�¼1��
*     �޸����ڣ�2011/07/01
*     �� �� �ţ�V1.0
*     �� �� �ˣ����Ľ�
*     �޸����ݣ�����
*******************************************************************************/
#include "sky.h"
#include "message_agent.h"
static bool sky_init_flag = false;
STATUS SkyInit(const SkyConfig& config)
{
    STATUS ret;
    //����ظ���ʼ������
    if(sky_init_flag)
    {
        printf("Error! SkyInit is called again!\n");
        return ERROR_DUPLICATED_OP;
    }

    //��ʼ���쳣����
    ret = ExcInit(config.exccfg);
    if(SUCCESS != ret)
    {
        printf("ExcInit failed! ret = %d\n",ret);
        return ret;
    }

    //������ʱ������ģ��
    ret = TimerInit(config.timecfg);
    if(SUCCESS != ret)
    {
        printf("Timer failed! ret = %d\n",ret);
        return ret;
    }

    //��������Ϣ������������  
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


