/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�instance_manager.h
* �ļ���ʶ��
* ����ժҪ��InstanceManager��Ķ����ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�mhb
* ������ڣ�2011��7��25��
*
* �޸ļ�¼1��
*     �޸����ڣ�2011/7/25
*     �� �� �ţ�V1.0
*     �� �� �ˣ�mhb
*     �޸����ݣ�����
* �޸ļ�¼2��
*     �޸����ڣ�2011/9/21
*     �� �� �ţ�V1.0
*     �� �� �ˣ�jixp
*     �޸����ݣ������Ż�
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
 * ��������:��ʱ����InstancePool,                                                     *
 *      1�����״̬Ǩ�ƣ�                                                             *
 *      2����û�м�ʱ�յ�ι����Ϣ��Instance��֪ͨVmHandler����������                  *
 *      �������ඨʱ����Ϣ����һ��5sһ�ε�״̬��ѯ���ö�ʱ���ڳ�ʼ��ʱ���ã�          *
 *      �����ι����ʱ��ʱ������Ϣ����Ϊdomid���ö�ʱ�����յ�ι��ʹ�ܺ����á�         *
 *************************************************************************************/
class InstanceManager : public MessageHandler
{
public:
    //��������
    ~InstanceManager();
    static InstanceManager *GetInstance()
    {
        if(NULL == instance)
        {
            instance = new InstanceManager();
        }
        return instance;
    };

    // ��ʼ��
    STATUS Init();

    // ����MU
    STATUS StartMu();

    // ��Ϣ��Ԫ���
    void MessageEntry(const MessageFrame& message);

    void SendMoniterDate(MessageVmInfoReport &info);

    void StartMoniterTimer();

    
private:
    //���캯��
    InstanceManager();

    // �������ʵ��״̬�ĺ�����״̬���ͺ���
    STATUS MonitorAllInstanceState(void);
    
    static InstanceManager         *instance;

    MessageUnit                    *_mu;

    VmDriver                       *_vm_driver;

    InstancePool                   *_instance_pool_ctrl;

    TIMER_ID                       _vm_moni_tid;

};


#endif // #ifndef HC_INSTANCEMANAGER_H


