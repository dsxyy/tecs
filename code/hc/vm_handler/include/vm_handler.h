/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�vm_handler.h
* �ļ���ʶ��
* ����ժҪ��VmHandler��Ķ����ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�lixiaocheng
* ������ڣ�2012��7��03��
*
* �޸ļ�¼1��
*     �޸����ڣ�2012/7/03
*     �� �� �ţ�V1.0
*     �� �� �ˣ�lixiaocheng
*     �޸����ݣ�����
*******************************************************************************/
#ifndef HC_VMHANDLER_H
#define HC_VMHANDLER_H
#include "sky.h"
#include "mid.h"
#include "vm_pub.h"
#include "instance_pool.h"
#include "vm_driver.h"
#include "msg_host_manager_with_host_agent.h"
#include "vm_messages.h"

// ���������ʵ���Ĺ������
class InstancePool;

// vmhandlerת����Ϣ��instance�Ľṹ
class TranceMsg : public Serializable 
{
    public:
    TranceMsg(const string &data, const MID &sender):
        _data(data), _sender(sender) {};
    
    TranceMsg(){};
    
    ~TranceMsg(){};

    SERIALIZE
    {
        SERIALIZE_BEGIN(TranceMsg);
        WRITE_VALUE(_data);
        WRITE_OBJECT(_sender);
        SERIALIZE_END();
    };
    
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(TranceMsg);
        READ_VALUE(_data);
        READ_OBJECT(_sender);
        DESERIALIZE_END();
    };
    
    string _data;
    MID    _sender;
};


// ����VmHandler����
class VmHandler : public MessageHandler
{
public:
    static VmHandler *GetInstance()
    {
        if(NULL == _instance)
        {
            _instance = new VmHandler();
        }

        return _instance;
    };
       
    //��������
    ~VmHandler()
    {
        delete _mu;
    };

    STATUS Init();

    // �����յ������������ACK
    STATUS DoCreateVmcfgAck(const MessageFrame& message);

    // ת��CC����Ϣ����Ӧ�������ʵ����ȥ
    void TranceCCMsgToInstance(const MessageFrame& message, uint32 vid);

    // ת�������ʵ���ϵ���Ϣ��CC,һ��ֻת��CANCEL��Ӧ���Ǩ�ƽ�����Ӧ��
    void TranceInstanceMsgToCC(const MessageFrame& message, MID &receiver);

    // �յ������������Ϣ�Ĵ�������
    void DoRecvNormalOp(const MessageFrame& message, uint32 op_cmd);

    // �յ������������Ϣ�Ĵ�������
    void DoRecvUnsave(const MessageFrame& message);

    // �յ������RESET��Ϣ�Ĵ�������
    void DoRecvReset(const MessageFrame& message);

    // �յ������Destroy��Ϣ�Ĵ�������
    void DoRecvDestroy(const MessageFrame& message);

    // �յ������Create��Ϣ�Ĵ�������
    void DoRecvCreate(const MessageFrame& message);

    // �յ������Cancel��Ϣ�Ĵ�������
    void DoRecvCancel(const MessageFrame& message);

    // �յ������OPRATE��Ϣ�������
    void VmHandlerExecVmOp(const MessageFrame& message);

    // �������������������
    void SendVmAgtCfgReq();

    // �����������������Ķ�ʱ��
    void StartVmAgetCfgReqTimer();
    //����������ն�ʱ��
    void StartReclaimDevTimer();

    // ���������Ǩ��׼������Ϣ
    void DoVmMigrationPerpare(const MessageFrame& message);

    // ���������Ǩ��ִ����Ϣ
    void DoVmMigrationExec(const MessageFrame& message);

    // �����յ������ʵ���ع�����Ӧ����Ϣ��һ�㴦��CANCEL��POST_M��Ϣ����Ҫ�ͷ�ʵ��
    void DoDelVmIntanceReq(const MessageFrame& message);

    // ���������Ǩ�Ƴɹ���֪ͨ�����л�����Ϣ
    void DoVmMigrationPost_M(const MessageFrame& message);

    // ��ȡVmHandler�Ĺ���״̬
    int GetVmHandlerWorkState()
    {
        return _mu->get_state();
    };

    bool LegalOperation(uint32 op, uint32 last_op);

    // ��Ϣ��Ԫ���
    void MessageEntry(const MessageFrame& frame);

    void DoRecvQueryUsbInfo(const MessageFrame& message);

    void DoVmWdgOperAck(const MessageFrame& message);

    static void GetUsbList(vector<VmQueryUsbInfo> &list);

    static void RecodUsbToList(VmQueryUsbInfo &info, vector<VmQueryUsbInfo> &usb_list);

    
private:
    static VmHandler  *_instance;

    // ������Ϣ��Ԫ
    STATUS StartMu(const string& name);

    // VMHANDLER��OP���������еĳ�ʼ������
    void OpRunningInit(VmInstance *inst, uint32 cmd, 
                                         const MessageFrame& message,
                                          VmOperationReq &op_msg);  

    void DoOpratePre(VmInstance     *inst, const MessageFrame& message,
                               uint32 op_cmd, VmOperationReq &op_msg);
    
    // ���캯��
    VmHandler():_mu(NULL),_vm_driver(NULL),_instance_pool(NULL),_cfg_req_timer_id(INVALID_TIMER_ID),_reclaimdev_timer_id(INVALID_TIMER_ID)
    {
    }
    
    MessageUnit       *_mu;                      // ��Ϣ��Ԫ
    VmDriver          *_vm_driver;               // ���������ָ��
    InstancePool      *_instance_pool;           // �����ʵ����
    TIMER_ID          _cfg_req_timer_id;         // �������������ʱ��
    TIMER_ID          _reclaimdev_timer_id;         // �������������ʱ��
};
#endif // #ifndef HC_VMHANDLER_H

