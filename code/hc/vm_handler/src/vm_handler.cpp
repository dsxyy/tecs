/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�vm_handler.cpp
* �ļ���ʶ��
* ����ժҪ��VmHandler���ʵ���ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�mhb
* ������ڣ�2011��7��25��
*
* �޸ļ�¼1��
*     �޸����ڣ�2011/7/25
*     �� �� �ţ�V1.0
*     �� �� �ˣ�mhb
*     �޸����ݣ�����
*******************************************************************************/
#include "vm_handler.h"
#include "vm_instance.h"
#include "tecs_config.h"
#include "registered_system.h"
#include "vm_driver.h"
#include "vm_messages.h"
#include "vnet_libnet.h"
#include "host_agent.h"

/****************************************************************************************
 *                           ��                                                   *
 ***************************************************************************************/
// vm_handler�ϵ��������������ʱ��ʱ��
#define EV_VMCFG_REQ_TIMER EV_TIMER_1
#define EV_RECLAIM_DEV_TIMER  EV_TIMER_2

/****************************************************************************************
 *                           ���ߺ���                                                   *
 ***************************************************************************************/


/******************************************************************************
 *                 ȫ�ֱ���                                                   *
 *****************************************************************************/
VmHandler* VmHandler::_instance = NULL;

/****************************************************************************************
* �������ƣ�Init
* ������������ʼ�������ת��ģ��
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lixch         ����
* 2013/06/20    V1.1    johnir        ����dom0������CPU
****************************************************************************************/
STATUS VmHandler::Init()
{
    _vm_driver = VmDriver::GetInstance();
    if (NULL == _vm_driver)
    {
        OutPut(SYS_ALERT, "VmHandler start fail! VmDriver is null!\n");
        return ERROR;
    }

    // Ŀǰֻ֧��xen
    _vm_driver->Init();

    // dom0�������
    HostAgent *host_agent = HostAgent::GetInstance();
    if(NULL == host_agent)
    {
        OutPut(SYS_ALERT, "host_agent return null instance!\n");
        return ERROR;
    }
    uint32 num = host_agent->GetDom0Cpus();
    string name = _vm_driver->GetDomainNameByDomid(0);
    _vm_driver->VcpuAffinitySet(name, num);

    // ��dom0��Ȩ�ص�����domu һ����������cap
    _vm_driver->Sched_credit_weight_set(0, 100);

    _instance_pool = InstancePool::GetSelf();
    if (NULL == _instance_pool)
    {
        OutPut(SYS_ALERT, "VmHandler start fail! InstancePool is null!\n");
        return ERROR;
    }

    return StartMu(MU_VM_HANDLER);
}

/****************************************************************************************
* �������ƣ�StartMu
* ����������������Ϣ��Ԫ
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lixch         ����
****************************************************************************************/
STATUS VmHandler::StartMu(const string& name)
{
    if(_mu)
    {
        SkyAssert(false);
        return ERROR_DUPLICATED_OP;
    }

    // ��Ϣ��Ԫ�Ĵ����ͳ�ʼ��
    _mu = new MessageUnit(name);
    if (!_mu)
    {
        OutPut(SYS_EMERGENCY, "Create mu %s failed!\n",name.c_str());
        return ERROR_NO_MEMORY;
    }

    STATUS ret = _mu->SetHandler(this);
    if (SUCCESS != ret)
    {
        OutPut(SYS_EMERGENCY, "SetHandler mu %s failed! ret = %d\n",name.c_str(),ret);
        return ret;
    }

    ret = _mu->Run();
    if (SUCCESS != ret)
    {
        OutPut(SYS_EMERGENCY, "Run mu %s failed! ret = %d\n",name.c_str(),ret);
        return ret;
    }

    ret = _mu->Register();
    if (SUCCESS != ret)
    {
        OutPut(SYS_EMERGENCY, "Register mu %s failed! ret = %d\n",name.c_str(),ret);
        return ret;
    }

    //���Լ������ϵ���Ϣ����ʹ��Ϣ��Ԫ״̬�л�������̬
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
* �������ƣ�DoCreateVmcfgAck
* ���������������յ��Ĵ���������Ϣ
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lixch         ����
* 2013/9/17     V1.1    johnir        handler/instance ����
****************************************************************************************/
STATUS VmHandler::DoCreateVmcfgAck(const MessageFrame& message)
{
    VmHandlerCfgAckInfo  create_msg;
    vector<VmCreateReq>::iterator it;

    create_msg.deserialize(message.data);

    for (it = create_msg._create_info.begin(); \
            it != create_msg._create_info.end();   \
            it++)
    {
        // ��������ʵ���Ѿ������ˣ������������һ��
        if (NULL != _instance_pool->GetInstanceByID(it->_vid))
        {
            // �������������Ĵ������̣���VMHANDLER�ĵ�һ���յ����õĵط�
            // ��Ӧ�ó�����������������֣��Ӹ��쳣��ӡ
            OutPut(SYS_ALERT,
                   "In DoCreateVmcfgAck, instance %d exist!\n", it->_vid);
            continue;
        }

        // ����һ�������ʵ��
        VmInstance *inst = new VmInstance(it->_config._vid);
        if (NULL == inst)
        {
            SkyExit(-1,"DoCreateVmcfgAck new VmInstance fail!\n");
            return ERROR;
        }

        // �ָ��������������Ϣ
        inst->SetVmCfg(it->_config);

        // �ָ�option��Ϣ /
        inst->SetVmOption(it->_option);

        // ��ʼ�����ʵ����
        if (SUCCESS != inst->Init())
        {
            SkyExit(-1,"DoCreateVmcfgAck VmInstances Init Fail!\n");
            return ERROR;
        }

        string inst_name = "instance_" + to_string<int64>(it->_config._vid,dec);
        MessageOption mo(inst_name, EV_POWER_ON, 0, 0);
        _mu->Send(inst_name, mo);

        //  ���뵽��������������ȥ
        _instance_pool->AddInstance(inst);

    }

    return VMOP_SUCC;
}

/****************************************************************************************
* �������ƣ�TranceCCMsgToInstance
* ����������ת����Ϣ�������ʵ�����߳���ȥ
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lixch         ����
****************************************************************************************/
void VmHandler::TranceCCMsgToInstance(const MessageFrame& message, uint32 vid)
{
    string recv = "instance_" + to_string<int64>(vid,dec);
    MID            receiver(recv);

    uint32 msgid    = message.option.id();

    // ������Ϣ������
    MessageOption option(receiver, msgid, 0, 0);

    // ����ת����Ϣ
    TranceMsg Msg(message.data, message.option.sender());

    STATUS ret = _mu->Send(Msg, option);

    if(SUCCESS != ret)
    {
        OutPut(SYS_ALERT,
               "[vm_handler] TranceCCMsgToInstance fail, vid:%ld return: %d --- recv: %s, msg_id: %d, sender: %s!\n",
               vid, ret, receiver._unit.c_str(), msgid, message.option.sender()._unit.c_str());
    }
    else
    {
        OutPut(SYS_INFORMATIONAL,
               "[vm_handler] TranceCCMsgToInstance ok,  vid:%ld return: %d --- recv: %s, msg_id: %d, sender: %s!\n",
               vid,  ret, receiver._unit.c_str(), msgid, message.option.sender()._unit.c_str());
    }
}

/****************************************************************************************
* �������ƣ�TranceCCMsgToInstance
* ����������ת����Ϣ��CC��ȥ
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lixch         ����
****************************************************************************************/
void VmHandler::TranceInstanceMsgToCC(const MessageFrame& message, MID &receiver)
{
    // ������Ϣ������
    MessageOption option(receiver, EV_VM_OP_ACK, 0, 0);

    STATUS ret = _mu->Send(message.data, option);

    if(SUCCESS != ret)
    {
        OutPut(SYS_ALERT,
               "In TranceInstanceMsgToCC, trance msg fail, return: %d --- recv: %s, msg_id: %d, sender: %s!\n",
               ret, receiver._unit.c_str(), EV_VM_OP_ACK, message.option.sender()._unit.c_str());
    }
    else
    {
        OutPut(SYS_INFORMATIONAL,
               "In TranceInstanceMsgToCC, trance msg ok, return: %d --- recv: %s, msg_id: %d, sender: %s!\n",
               ret, receiver._unit.c_str(), EV_VM_OP_ACK, message.option.sender()._unit.c_str());
    }
}

/****************************************************************************************
* �������ƣ�OpRunningInit
* ��������������ʵ����״̬��ת����Ϣ��ʵ������Ӧ��������·���
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lixch         ����
*****************************************************************************************/
void VmHandler::OpRunningInit(VmInstance *inst, uint32 cmd,
                              const MessageFrame& message,
                              VmOperationReq &op_msg)
{
    //��Ҫ���õı�־��instʹ�ã�ͳһ��������
    if(CANCEL == cmd)
    {
        /* cancel��ռstartʱ��������start��������洢��ѭ�� */
        inst->VmSetDestroyFlag();
    }
    if(CANCEL_UPLOAD_IMG == cmd)
    {

        // ����unsave��־����ʵ������ִ�е�SAVE��ʱ���ܹ�ͣ����
        inst->SetInstanceUnSave();
    }

    // ת����Ϣ��INSTANCES
    TranceCCMsgToInstance(message, op_msg._vid);

    // ��Ӧ��������·��⪣����CC
}

/****************************************************************************************
* �������ƣ�DoOpratePre
* �������������������ǰ������Ҫ���������ʵ���Ͳ��������
*                             ���
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ����
*
* ����˵�������lastcmdΪ�գ����ó�ʼ��Ϣ��ת����Ϣ��instance����
*           lastcmd==newcmd?���ؽ���:(�����Ƿ���ռ?���ó�ʼ��Ϣ��ת����Ϣ��instance����:����busy)
*
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lixch         ����
****************************************************************************************/
void VmHandler::DoOpratePre(VmInstance     *inst, const MessageFrame& message,
                            uint32 op_cmd, VmOperationReq &op_msg)
{
    uint32  lastCmd = inst->GetOpCmd();

    /* �����һ�����е������Ƿ�Ϊ�գ������������Ҫ��� */
    if(UNKNOWN_OP == lastCmd)
    {
        SkyAssert(!inst->IsOpRunning());

        //�����������̬������ִ��RR����
        if((RR == op_cmd) && VM_RUNNING != inst->GetVmState() )
        {
            OutPut(SYS_INFORMATIONAL,
                   "Warning:vm %s is %d, cannot excute RR.\n",
                   inst->GetInstanceName().c_str(), inst->GetVmState());
            return;
        }
    
        OpRunningInit(inst, op_cmd, message, op_msg);
        return;
    }

    /* �������������һ�������Ƿ���ͬ */
    if(lastCmd == op_cmd)
    {
        inst->SendVmOprAckMsg(message.option.sender(), op_msg.action_id, _mu);
    }
    else
    {
        /* ����Ƿ������ռ */
        if(LegalOperation(op_cmd, lastCmd))
        {
            OpRunningInit(inst, op_cmd, message, op_msg);
        }
        else
        {
            // ����ϵͳ��æ��Ӧ���CC
            inst->SendVmOprAckMsg(message.option.sender(), VMOP_VM_AGT_BUSY,
                                  op_cmd, 1, op_msg.action_id, _mu);
        }
    }

    return;
}

/****************************************************************************************
* �������ƣ�DoOprate
* ����������VMHANDLER�յ�������Ϣ�Ĵ�������
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lixch         ����
****************************************************************************************/
void VmHandler::DoRecvNormalOp(const MessageFrame& message, uint32 op_cmd)
{
    VmInstance     *inst = NULL;
    VmOperationReq op_msg;

    op_msg.deserialize(message.data);

    // ��ȡ�����ʵ��
    inst = _instance_pool->GetInstanceByID(op_msg._vid);
    if (NULL == inst)
    {
        OutPut(SYS_NOTICE,"DoRecvNormalOp: GetInstanceByID fail vid = %d op_cmd = %d!\n",
               op_msg._vid, op_cmd);
        return;
    }

    // ���UNSAVE״̬���ýӿڻᱻSAVE����
    inst->ClearInstanceUnSave();

    // Ԥ����ʧ�ܣ��򷵻ز���������
    DoOpratePre(inst, message, op_cmd, op_msg);

    return;    
}

/****************************************************************************************
* �������ƣ�DoRecvUnsave
* ����������VMHANDLER�յ�CANCEL_UPLOAD_IMG��Ϣ�Ĵ�������
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lixch         ����
****************************************************************************************/
void VmHandler::DoRecvUnsave(const MessageFrame& message)
{
    VmInstance     *inst = NULL;
    VmOperationReq op_msg;

    op_msg.deserialize(message.data);

    // ��ȡ�����ʵ��
    inst = _instance_pool->GetInstanceByID(op_msg._vid);
    if (NULL == inst)
    {
        OutPut(SYS_NOTICE,"DoRecvUnsave: GetInstanceByID fail vid = %d!\n", op_msg._vid);
        return;
    }

    // Ԥ����ʧ�ܣ��򷵻ز���������
    DoOpratePre(inst, message, CANCEL_UPLOAD_IMG, op_msg);

    return;
}


/****************************************************************************************
* �������ƣ�DoRecvReset
* ����������VMHANDLER�յ�RESET��Ϣ�Ĵ�������
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lixch         ����
****************************************************************************************/
void VmHandler::DoRecvReset(const MessageFrame& message)
{
    VmInstance     *inst = NULL;
    VmOperationReq op_msg;

    op_msg.deserialize(message.data);

    // ��ȡ�����ʵ��
    inst = _instance_pool->GetInstanceByID(op_msg._vid);
    if (NULL == inst)
    {
        OutPut(SYS_NOTICE,"DoRecvReset: GetInstanceByID fail vid = %d!\n", op_msg._vid);
        return;
    }

    // Ԥ����ʧ�ܣ��򷵻ز���������
    DoOpratePre(inst, message, RESET, op_msg);

    return;
}

/****************************************************************************************
* �������ƣ�DoRecvDestroy
* ����������VMHANDLER�յ�DESTROY��Ϣ�Ĵ�������
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lixch         ����
****************************************************************************************/
void VmHandler::DoRecvDestroy(const MessageFrame& message)
{
    VmInstance     *inst = NULL;
    VmOperationReq op_msg;

    op_msg.deserialize(message.data);

    // ��ȡ�����ʵ��
    inst = _instance_pool->GetInstanceByID(op_msg._vid);
    if (NULL == inst)
    {
        OutPut(SYS_NOTICE,"DoRecvDestroy: GetInstanceByID fail vid = %d!\n", op_msg._vid);
        return;
    }

    // Ԥ����ʧ�ܣ��򷵻ز���������
    DoOpratePre(inst, message, DESTROY, op_msg);
    
    return;
}

/****************************************************************************************
* �������ƣ�DoRecvCreate
* ����������VMHANDLER�յ�CREATE��Ϣ�Ĵ�������
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lixch         ����
****************************************************************************************/
void VmHandler::DoRecvCreate(const MessageFrame& message)
{
    VmInstance     *inst = NULL;
    VmOperationReq op_msg;

    op_msg.deserialize(message.data);

    // ��ȡ�����ʵ��
    inst = _instance_pool->GetInstanceByID(op_msg._vid);
    if (NULL == inst)
    {
        // ����һ�������ʵ��
        inst = new VmInstance(op_msg._vid);
        if (NULL == inst)
        {
            SkyExit(-1,"DoRecvCreate new VmInstance fail!\n");
            return;
        }

        // ��ʼ�����ʵ����
        if (SUCCESS != inst->Init())
        {
            SkyExit(-1,"DoRecvCreate VmInstances Init Fail!\n");
            return;
        }

        string inst_name = "instance_" + to_string<int64>(op_msg._vid, dec);
        MessageOption mo(inst_name, EV_POWER_ON, 0, 0);
        _mu->Send(inst_name, mo);

        //  ���뵽��������������ȥ
        _instance_pool->AddInstance(inst);
        // �������ʵ�������ز��� ת����Ϣ��ʵ��
        OpRunningInit(inst, CREATE, message, op_msg);
        return;
    }

    // Ԥ����ʧ�ܣ��򷵻ز���������
    DoOpratePre(inst, message, CREATE, op_msg);
    
    return;
}

/****************************************************************************************
* �������ƣ�DoRecvCancel
* ����������VMHANDLER�յ�CANCEL��Ϣ�Ĵ�������
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lixch         ����
****************************************************************************************/
void VmHandler::DoRecvCancel(const MessageFrame& message)
{
    VmInstance     *inst = NULL;
    VmOperationReq op_msg;

    op_msg.deserialize(message.data);

    // ��ȡ�����ʵ��
    inst = _instance_pool->GetInstanceByID(op_msg._vid);
    if (NULL == inst)
    {
        OutPut(SYS_INFORMATIONAL, "*** DoRecvCancel: vm(%lld) Instance not exist, send vm already canceled***\n",\
               op_msg._vid);

        VmOperationAck AckMsg(op_msg.action_id,
                              VMOP_SUCC,
                              100,
                              "vm already canceled",
                              op_msg._vid,
                              op_msg._operation,
                              VM_INIT);

        MessageOption option(message.option.sender(), EV_VM_OP_ACK, 0, 0);

        STATUS ret = _mu->Send(AckMsg, option);
        if(SUCCESS != ret)
        {
            OutPut(SYS_ERROR, "*** DoRecvCancel: send vm(%lld) already canceled msg to cc failed ***\n",
                   op_msg._vid);
        }

        return;
    }
     
    // Ԥ����ʧ�ܣ��򷵻ز���������
    DoOpratePre(inst, message, CANCEL, op_msg);
    
    return ;
}

/****************************************************************************************
* �������ƣ�VmHandlerExecVmOp
* ����������ִ�����������
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵����
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/07/16    V1.0    lixch         ����
****************************************************************************************/
void VmHandler::VmHandlerExecVmOp(const MessageFrame& message)
{
    VmOperationReq  req;
    VmOperation     opr_id;

    // ������Ϣ�ṹ����ȡ��������
    req.deserialize(message.data);
    opr_id        = (VmOperation)(req._operation);
    MID   sender  = message.option.sender();
    // ���Դ�ӡ����¼��ǰϵͳ�յ�ʲô��������
    OutPut(SYS_DEBUG, "VmHandlerExecVmOp rcv request from %s, vm_id: %lld, operation: %s\n",
           sender.serialize().c_str(), req._vid, Enum2Str::VmOpStr(opr_id));

    switch (opr_id)
    {
    case CREATE:
    {
        DoRecvCreate(message);
        break;
    }
    case START:
    case STOP:
    case REBOOT:
    case PAUSE:
    case RESUME:
    case RR:
    case UPLOAD_IMG:
    case PLUG_IN_USB:
    case PLUG_OUT_USB:
    case OUT_IN_USB:
    case PLUG_IN_PDISK:
    case PLUG_OUT_PDISK:
    case RECOVER_IMAGE:
    case CREATE_IMAGE_BACKUP:
    case DELETE_IMAGE_BACKUP:
    {
        DoRecvNormalOp(message, opr_id);
        break;
    }
    case CANCEL_UPLOAD_IMG:
    {
        DoRecvUnsave(message);
        break;
    }
    case CANCEL:
    {
        DoRecvCancel(message);
        break;
    }
    case RESET:
    {
        DoRecvReset(message);
        break;
    }
    case DESTROY:
    {
        DoRecvDestroy(message);
        break;
    }
    // Ŀ�Ķ�׼��Ǩ��
    case PREP_M:
    {
        DoVmMigrationPerpare(message);
        break;
    }
    // Դ��ִ��Ǩ�� ת����Ϣ��instance
    case MIGRATE:
    {
        DoVmMigrationExec(message);
        break;
    }
    // Դ��Ǩ�Ƴɹ�
    case POST_M:
    {
        DoVmMigrationPost_M(message);
        break;
    }

    default:
    {
        OutPut(SYS_ALERT,"VmHandlerExecVmOp rcv unknown cmd: %d\n", opr_id);
        break;
    }
    }
}

/****************************************************************************************
* �������ƣ�SendVmAgtCfgReq
* �����������ϱ�VM������Ϣ������Ϣ��CC
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/07/10    V1.0    lixch      ����
****************************************************************************************/
void VmHandler::SendVmAgtCfgReq()
{
    MID receiver;
    STATUS ret = ERROR;

    ret = GetRegisteredSystem(receiver._application);
    if (SUCCESS != ret)
    {
        OutPut(SYS_ALERT, "In VmHandler::SendVmAgtCfgReq: GetRegisteredSystem failed! ret=%d\n",ret);
    }
    receiver._process = PROC_CC;
    receiver._unit = MU_VM_LIFECYCLE_MANAGER;

    MessageOption option(receiver, EV_VMAGT_CFG_REQ, 0, 0);
    STATUS sdresult=_mu->Send(ApplicationName(), option);
    if (SUCCESS != sdresult)
    {
        OutPut(SYS_ALERT, "In VmHandler::SendVmAgtCfgReq: send to cc failed! ret=%d\n",sdresult);
    }
}

/****************************************************************************************
* �������ƣ�SendVmAgtCfgReq
* �����������ϱ�VM������Ϣ������Ϣ��CC
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/07/10    V1.0    lixch      ����
****************************************************************************************/
void VmHandler::StartVmAgetCfgReqTimer()
{
    // ������ʱ������ֹͣ��ʱ��
    if (INVALID_TIMER_ID == _cfg_req_timer_id)
    {
        if (INVALID_TIMER_ID == (_cfg_req_timer_id = _mu->CreateTimer()))
        {
            OutPut(SYS_EMERGENCY, "StartVmAgetCfgReqTimer: CreateTimer failed!\n");
            SkyExit(-1, "VmHandler::StartVmAgetCfgReqTimer: CreateTimer failed!");
            SkyAssert(0);
        }
    }

    // ����������Ϣ�ϱ�ѭ����ʱ��
    TimeOut timeout;
    timeout.type = RELATIVE_TIMER;
    timeout.duration = 5 * 1000;// 5s����һ����������
    timeout.msgid = EV_VMCFG_REQ_TIMER;
    _mu->SetTimer(_cfg_req_timer_id, timeout);
}

void VmHandler::StartReclaimDevTimer()
{
    // ������ʱ������ֹͣ��ʱ��
    if (INVALID_TIMER_ID == _reclaimdev_timer_id)
    {
        if (INVALID_TIMER_ID == (_reclaimdev_timer_id = _mu->CreateTimer()))
        {
            OutPut(SYS_ERROR, "StartReclaimDevTimer: CreateTimer failed!\n");
            SkyExit(-1, "VmHandler::StartReclaimDevTimer: CreateTimer failed!");
            SkyAssert(0);
        }
    }

    // ���û����豸��ʱ��
    TimeOut timeout;
    timeout.type = LOOP_TIMER;
    timeout.duration = 10 * 1000;//
    timeout.msgid = EV_RECLAIM_DEV_TIMER;
    _mu->SetTimer(_reclaimdev_timer_id, timeout);
}

/****************************************************************************************
* �������ƣ�DoVmMigrationPerpare
* ��������������Ŀ�Ķ�׼�������Ǩ��
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/7/16    V1.0    lixch         ����
****************************************************************************************/
void VmHandler::DoVmMigrationPerpare(const MessageFrame& message)
{
    int64           vm_id;
    VmMigrateData   prepare;
    VmInstance      *inst  = NULL;

    prepare.deserialize(message.data);
    vm_id         = prepare._vid;

    OutPut(SYS_DEBUG, "In DoVmMigrationPerpare, recv req --- vm_id: %lld\n", vm_id);

    // �ȿ��Ƿ�֮ǰ�����
    inst = _instance_pool->GetInstanceByID(vm_id);
    // �������instance,���ܽ���Ǩ��׼��
    if (NULL != inst)
    {
        // Ԥ����ʧ�ܣ��򷵻ز���������
        DoOpratePre(inst, message, PREP_M, prepare);
        return;
    }

    // ����һ�������ʵ��
    inst = new VmInstance(vm_id);
    if (NULL == inst)
    {
        SkyExit(-1,"DoVmMigrationPerpare new VmInstance fail!\n");
        return;
    }

    // ��ʼ�����ʵ����
    if (SUCCESS != inst->Init())
    {
        SkyExit(-1,"DoVmMigrationPerpare VmInstances Init Fail!\n");
        return;
    }

    string inst_name = "instance_" + to_string<int64>(prepare._vid, dec);
    MessageOption mo(inst_name, EV_POWER_ON, 0, 0);
    _mu->Send(inst_name, mo);

    //  ���뵽��������������ȥ
    _instance_pool->AddInstance(inst);

    OpRunningInit(inst, PREP_M, message, prepare);
}


/****************************************************************************************
* �������ƣ�DoVmMigrationExec
* ����������Դ��ִ�������Ǩ�Ƶ�VMHANDLER����
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/7/16     V1.0    lixch         ����
****************************************************************************************/
void VmHandler::DoVmMigrationExec(const MessageFrame& message)
{
    int64          vm_id;
    VmMigrateData  migrate;

    migrate.deserialize(message.data);
    vm_id         = migrate._vid;

    OutPut(SYS_DEBUG, "In DoVmMigrationExec, recv req --- vm_id: %lld\n", vm_id);

    VmInstance *inst = _instance_pool->GetInstanceByID(vm_id);
    // ���ʵ�������ڣ�˵��Ǩ�Ƴɹ��ˣ�ֱ�ӷ��سɹ�
    if (NULL == inst)
    {
        OutPut(SYS_DEBUG, "DoVmMigrationExec: inst does not exist!\n");
        // ����һ�������ʵ��
        inst = new VmInstance(vm_id);
        if (NULL == inst)
        {
            SkyExit(-1,"DoRecvCreate new VmInstance fail!\n");
            return;
        }
        // ���ͳɹ�Ӧ���CC
        inst->SendVmOprAckMsg(message.option.sender(),
                              VMOP_SUCC, MIGRATE, 100, migrate.action_id, _mu);
        // ɾ��ʵ��
        delete inst;

        return;
    }

    // Ԥ����ʧ�ܣ��򷵻ز���������
    DoOpratePre(inst, message, MIGRATE, migrate);
    
    return;
}

/****************************************************************************************
* �������ƣ�DoDelVmIntanceReq
* ��������������instance�߳�
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/7/16     V1.0    lixch         ����
****************************************************************************************/
void VmHandler::DoDelVmIntanceReq(const MessageFrame& message)
{
    int64           vm_id;
    VmOperationAck  msg;

    msg.deserialize(message.data);
    vm_id         = msg._vid;

    OutPut(SYS_DEBUG, "DoDelVmIntanceReq: rcv req from vm_id = %lld.", vm_id);

    VmInstance *inst = _instance_pool->GetInstanceByID(vm_id);
    // ʵ�������ڣ����ԣ���¼���󣬲���Ӧ��
    if (NULL == inst)
    {
        OutPut(SYS_EMERGENCY, "DoDelVmIntanceReq: inst does not exist!\n");
        SkyAssert(false);
        return;
    }

    // ֻ���� MIGRATE ��CANCEL ��֮����
    if ((MIGRATE != msg._operation) && (CANCEL != msg._operation))
    {
        OutPut(SYS_EMERGENCY, "DoDelVmIntanceReq: recv err operate %d (%s)!\n", \
               msg._operation, Enum2Str::VmOpStr(msg._operation));
        SkyAssert(false);
    }

    // ��ȡ���͵�ַ
    MID receiver;
    inst->GetOpSender(receiver);

    // ���ɾ��ʵ��ʧ�ܣ���������ܴ��ڣ����ԣ���¼����
    if (!_instance_pool->DelInstance(vm_id))
    {
        OutPut(SYS_EMERGENCY, "DoDelVmIntanceReq: inst del fail!\n");
        SkyAssert(false);
    }

    //����? ���Ⱥͽ������100&succ��ʵ��ɾ���޷����ã���Ҫ����ʱ����null����
    TranceInstanceMsgToCC(message, receiver);
}

/****************************************************************************************
* �������ƣ�TransVmMigrateSucc
* ����������ת����instance,Դ��Ǩ�Ƴɹ���֪ͨĿ�Ķ���������л�
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/7/17     V1.0    lixch         ����
****************************************************************************************/
void VmHandler::DoVmMigrationPost_M(const MessageFrame& message)
{
    int64         vm_id;
    VmMigrateData  inform;

    inform.deserialize(message.data);
    vm_id         = inform._vid;

    OutPut(SYS_DEBUG, "In DoVmMigrationPost_M, recv req --- vm_id: %lld\n", vm_id);

    VmInstance *inst = _instance_pool->GetInstanceByID(vm_id);
    // ���ʵ�������ڣ���������ܴ��ڣ����ԣ���¼���󣬲���Ӧ��
    if (NULL == inst)
    {
        OutPut(SYS_EMERGENCY, "DoVmMigrationExec: inst does not exist!\n");
        SkyAssert(false);
        return;
    }

    // Ԥ����ʧ�ܣ��򷵻ز���������
    DoOpratePre(inst, message, POST_M, inform);
    
    return;
}


/****************************************************************************************
* �������ƣ�DoRecvQueryUsbInfo
* ������������ȡUSB��Ϣ�Ĳ�ѯ����
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/7/17     V1.0    lixch         ����
****************************************************************************************/
void VmHandler::DoRecvQueryUsbInfo(const MessageFrame& message)
{
    VmUsbListAck usb_result_list;

    VmUsbListAck recv_req;
    recv_req.deserialize(message.data);

    // �ȸ�ֵCC������������
    usb_result_list._stamp = recv_req._stamp;
    usb_result_list._ret = SUCCESS;

    GetUsbList(usb_result_list._info);

    MessageOption option(message.option.sender(), EV_VMCFG_USB_INFO_QUERY_ACK,0,0);
    _mu->Send(usb_result_list, option);
}


/****************************************************************************************
* �������ƣ�DoVmWdgOperAck
* �����������յ����Ź�����Ӧ��Ĵ�����(hc <- vna)
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/7/16     V1.0    lixch         ����
****************************************************************************************/
void VmHandler::DoVmWdgOperAck(const MessageFrame& message)
{
    int64           vm_id;
    CVNetWatchDogMsgAck  msg;

    msg.deserialize(message.data);
    vm_id         = msg.vm_id;

    OutPut(SYS_DEBUG, "In DoVmWdgOperAck, recv OpAck --- vm_id: %lld\n", vm_id);

    VmInstance *inst = _instance_pool->GetInstanceByID(vm_id);
    // ���ʵ�������ڣ���������ܴ��ڣ����ԣ���¼���󣬲���Ӧ��
    if (NULL == inst)
    {
        OutPut(SYS_EMERGENCY, "DoRecvOpAck: inst does not exist, vid:%d. discard!\n", vm_id);
        //SkyAssert(false);
        return;
    }


    string recv = "instance_" + to_string<int64>(vm_id,dec);
    MID            receiver(recv);

    // ������Ϣ������
    MessageOption option(receiver, message.option.id(), 0, 0);

    STATUS ret = _mu->Send(msg, option);

    if(SUCCESS != ret)
    {
        OutPut(SYS_ALERT,
               "DoVmWdgOperAck fail, vid:%ld return: %d --- recv: %s, msg_id: %d, sender: %s!\n",
               vm_id, ret, receiver._unit.c_str(),  message.option.id(), message.option.sender()._unit.c_str());
    }
    else
    {
        OutPut(SYS_INFORMATIONAL,
               "DoVmWdgOperAck ok,  vid:%ld return: %d --- recv: %s, msg_id: %d, sender: %s!\n",
               vm_id,  ret, receiver._unit.c_str(),  message.option.id(), message.option.sender()._unit.c_str());
    }
}

/****************************************************************************************
* �������ƣ�GetUsbList
* ������������ȡUSB��Ϣ���б���ȥ
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/7/17     V1.0    lixch         ����
****************************************************************************************/
void VmHandler::GetUsbList(vector<VmQueryUsbInfo> &list)
{
    // ��ȡHC�ϵ�USB��Ϣ
    ifstream fin("/proc/bus/usb/devices");
    if (!fin)
    {
        return;
    }

    string                  result;
    VmQueryUsbInfo  usb_info;
    bool                    is_usb_controller = false;
    while(getline(fin, result))
    {
        // �Ȼ�ȡһ���������
        if (result.find("T:") != string::npos)
        {
            // ����һ��NULL��¼������ȥ��
            if (!is_usb_controller)
            {
                RecodUsbToList(usb_info, list);
            }
            else
            {
                is_usb_controller = false;
            }
            usb_info.clear();
            continue;
        }

        // �����Ƿ��ǿ�����
        string::size_type loc = result.find("B:");
        if (loc != string::npos)
        {
            is_usb_controller = true;
            continue;
        }

        // �����Ƿ��ǿ�����
        loc = result.find("D:");
        if (loc != string::npos)
        {
            if (string::npos != result.find("hub"))
            {
                is_usb_controller = true;
            }
            continue;
        }

        // ����Manufacturer
        loc = result.find("Manufacturer=");
        if (loc != string::npos)
        {
            usb_info.manufacturer =
                result.substr(loc+strlen("Manufacturer="));
        }

        // ����vendor_id
        loc = result.find("Vendor=");
        if (loc != string::npos)
        {
            usb_info.vendor_id =
                strtoul(result.substr(loc+strlen("Vendor="),4).c_str(),NULL, 16);
        }

        loc = result.find("ProdID=");
        if (loc != string::npos)
        {
            usb_info.product_id =
                strtoul(result.substr(loc+strlen("ProdID="),4).c_str(),NULL, 16);
            continue;
        }

        loc = result.find("Product=");
        if (loc != string::npos)
        {
            usb_info.name = result.substr(loc+strlen("Product="));
        }
    }
    // �������һ��USB��Ϣ
    if (!is_usb_controller)
    {
        RecodUsbToList(usb_info, list);
    }
}

/****************************************************************************************
* �������ƣ�RecodUsbList
* ������������¼USB��Ϣ����¼�б��еĺ���
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2012/7/17     V1.0    lixch         ����
****************************************************************************************/
void VmHandler::RecodUsbToList(VmQueryUsbInfo &info, vector<VmQueryUsbInfo> &usb_list)
{
    if (info.vendor_id == 0 && info.vendor_id == 0)
    {
        return;
    }

    uint32 i;
    for (i = 0; i < usb_list.size(); i++)
    {
        if (info.vendor_id   == usb_list.at(i).vendor_id &&
                info.product_id == usb_list.at(i).product_id &&
                info.name         == usb_list.at(i).name )
        {
            usb_list.at(i).is_conflict = true;
            return;
        }
    }

    info.online_states = USB_ON_LINE;
    usb_list.push_back(info);
}


/****************************************************************************************
* �������ƣ�MessageEntry
* ���������������������Ϣ���
* ���ʵı���
* �޸ĵı���
* �����������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lixch         ����
****************************************************************************************/
void VmHandler::MessageEntry(const MessageFrame& message)
{
    switch (_mu->get_state())
    {
    case S_Startup:
    {
        switch (message.option.id())
        {
        case EV_POWER_ON:
        case EV_VMCFG_REQ_TIMER:
        {
            // ������������������CC
            SendVmAgtCfgReq();
            // ������ʱ�������������������
            StartVmAgetCfgReqTimer();
            break;
        }
        case EV_VMAGT_CFG_ACK:
        {
            // ɾ����ʱ��
            _mu->KillTimer(_cfg_req_timer_id);
            // ��ʼ����������Ϣ��������Ӧ�������ʵ��
            DoCreateVmcfgAck(message);
#if 0
            StartReclaimDevTimer();
#endif
            // ���뵽WORK̬
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
        case EV_VM_OP:
        {
            VmHandlerExecVmOp(message);
            break;
        }
        // �յ�VM_instances ��ɾ������
        case EV_DEL_VM_INSTANCE_REQ:
        {
            DoDelVmIntanceReq(message);
            break;
        }
        case EV_VMCFG_USB_INFO_QUERY:
        {
            DoRecvQueryUsbInfo(message);
            break;
        }

        case EV_VNETLIB_WATCHDOG_OPER_ACK:
        {
            DoVmWdgOperAck(message);
            break;
        }

        case EV_RECLAIM_DEV_TIMER:
        {
            ReclaimDevice();
            break;
        }

        default:
        {
            OutPut(SYS_NOTICE, "VmHandler receive unknown message,id=%d.\n", message.option.id());
            break;
        }
        }
        break;
    }
    default:
        break;
    }
}


/* ��������Ƿ������ռ����Ҫ��CC�ϵ�VMManager::LegalOperation()����һ�� */
bool VmHandler::LegalOperation(uint32 op, uint32 last_op)
{
    OutPut(SYS_DEBUG, "hc LegalOperation:newcmd(%d),oldcmd(%d).\n", op, last_op);

    switch (op)
    {
        case CANCEL:
        {
            return (MIGRATE != last_op) && (COLD_MIGRATE != last_op) && (LIVE_MIGRATE != last_op)   //������������������Ǩ���и����
                    && (PLUG_IN_PDISK != last_op);
        }

        case RESET:
        {
            return (REBOOT == last_op);
        }

        case STOP:
        {
            return (PLUG_IN_PDISK == last_op) || (PLUG_OUT_PDISK == last_op);
        }

        case DESTROY:
        {
            return (STOP == last_op) || (PLUG_IN_PDISK == last_op) || (PLUG_OUT_PDISK == last_op);
        }

        case CANCEL_UPLOAD_IMG:
        {
                return (UPLOAD_IMG == last_op);
        }

        case RESTORE_IMAGE_BACKUP:
        {
            return (RECOVER_IMAGE !=last_op && UPLOAD_IMG != last_op && DELETE_IMAGE_BACKUP != last_op && CREATE_IMAGE_BACKUP != last_op);
        }

        default:
        {
            return false;
        }
    }
}

void testRR(int32 vid, int32 time)
{
    VmRRReq  temp;

    temp._delay_time = time;
    temp._vid        = vid;
    temp._operation  = RR;
    temp._stamp      = "test";

    /* 2. ������ʱ��Ϣ��Ԫ */
    MessageUnit temp_mu(TempUnitName("vm_handler_test"));
    temp_mu.Register();

    /* 3. ����Ϣ�� VM_HANDLER  */
    MessageOption option(MID(PROC_HC, MU_VM_HANDLER),
                         EV_VM_OP,0,0);
    temp_mu.Send(temp, option);

    /* 4. �ȴ�MU_VMCFG_MANAGER��Ӧ */
    MessageFrame message;
    if (SUCCESS == temp_mu.Wait(&message,5000))
    {
        cout << "wait RR Ack Success" << endl;
        return;
    }

    cout << "wait RR Ack TimeOut" << endl;
}


void testOpVm(int32 vid, int32 op, int32 action_id)
{
    string action("action_"+ to_string<int32>(action_id,dec));

    VmOperationReq  req(action, vid, op, "");

    cout<<"will "<<Enum2Str::VmOpStr(op)<<" VM ("<<vid<<") "<<endl;

    /* 2. ������ʱ��Ϣ��Ԫ */
    MessageUnit temp_mu(TempUnitName("test_vm_op"));
    temp_mu.Register();

    /* 3. ����Ϣ�� VM_HANDLER  */
    MessageOption option(MID(PROC_HC, MU_VM_HANDLER),
                         EV_VM_OP,0,0);
    temp_mu.Send(req, option);

    /* 4. �ȴ�MU_VMCFG_MANAGER��Ӧ */
    MessageFrame message;
    if (SUCCESS == temp_mu.Wait(&message,10000))
    {
        cout << "wait  Ack Success" << endl;
        return;
    }

    cout << "wait  Ack TimeOut" << endl;
}

DEFINE_DEBUG_FUNC(testRR);


DEFINE_DEBUG_FUNC(testOpVm);

