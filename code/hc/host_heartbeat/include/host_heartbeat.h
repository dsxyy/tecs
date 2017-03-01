/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�host_agent.h
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��HostAgent�ඨ���ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�Ԭ����
* ������ڣ�2011��8��9��
*
* �޸ļ�¼1��
*     �޸����ڣ�2011/8/9
*     �� �� �ţ�V1.0
*     �� �� �ˣ�Ԭ����
*     �޸����ݣ�����
*******************************************************************************/
#ifndef HOST_HEARTBEAT_H
#define HOST_HEARTBEAT_H
#include "sky.h"
#include "mid.h"

namespace zte_tecs 
{

/**
@brief ��������: ����������ʵ���࣬ʵ��״̬���ģ��Ĺ���\n
@li @b ����˵��: ��
*/
class HostHeartbeat : public MessageHandler
{
public:
    static HostHeartbeat *GetInstance()
    {
        if (NULL == _instance)
        {
            _instance = new HostHeartbeat();
        }
        return _instance;
    };

    virtual ~HostHeartbeat() 
    {

    };
    
    STATUS Init()
    {
        return StartMu(MU_HOST_HEARTBEAT);
    }
    
    virtual void MessageEntry(const MessageFrame &message); //��Ϣ������ں���
private:
    MessageUnit *_mu;               // ��Ϣ��Ԫ
    string      _cluster_name;      // �����������ļ�Ⱥ����
    TIMER_ID    _timer_id;          // ��ģ��ʹ�õĶ�ʱ��
    int32       _timer_duration;    // ��ģ��ʹ�õĶ�ʱ��ʱ��

    static HostHeartbeat *_instance;  
    HostHeartbeat();
    STATUS StartMu(const string& name);
    void DoStartUp();
    void DoHeartbeatReport();

    // ���ÿ�������
    DISALLOW_COPY_AND_ASSIGN(HostHeartbeat);   
};

} // namespace zte_tecs
#endif // #ifndef HA_HOST_AGENT_H

