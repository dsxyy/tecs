/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：vm_handler.cpp
* 文件标识：
* 内容摘要：VmHandler类的实现文件
* 当前版本：1.0
* 作    者：mhb
* 完成日期：2011年7月25日
*
* 修改记录1：
*     修改日期：2011/7/25
*     版 本 号：V1.0
*     修 改 人：mhb
*     修改内容：创建
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
 *                           宏                                                   *
 ***************************************************************************************/
// vm_handler上电虚拟机配置请求超时定时器
#define EV_VMCFG_REQ_TIMER EV_TIMER_1
#define EV_RECLAIM_DEV_TIMER  EV_TIMER_2

/****************************************************************************************
 *                           工具函数                                                   *
 ***************************************************************************************/


/******************************************************************************
 *                 全局变量                                                   *
 *****************************************************************************/
VmHandler* VmHandler::_instance = NULL;

/****************************************************************************************
* 函数名称：Init
* 功能描述：初始化虚拟机转发模块
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lixch         创建
* 2013/06/20    V1.1    johnir        增加dom0绑定物理CPU
****************************************************************************************/
STATUS VmHandler::Init()
{
    _vm_driver = VmDriver::GetInstance();
    if (NULL == _vm_driver)
    {
        OutPut(SYS_ALERT, "VmHandler start fail! VmDriver is null!\n");
        return ERROR;
    }

    // 目前只支持xen
    _vm_driver->Init();

    // dom0绑定物理核
    HostAgent *host_agent = HostAgent::GetInstance();
    if(NULL == host_agent)
    {
        OutPut(SYS_ALERT, "host_agent return null instance!\n");
        return ERROR;
    }
    uint32 num = host_agent->GetDom0Cpus();
    string name = _vm_driver->GetDomainNameByDomid(0);
    _vm_driver->VcpuAffinitySet(name, num);

    // 把dom0的权重调整和domu 一样，不设置cap
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
* 函数名称：StartMu
* 功能描述：启动消息单元
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lixch         创建
****************************************************************************************/
STATUS VmHandler::StartMu(const string& name)
{
    if(_mu)
    {
        SkyAssert(false);
        return ERROR_DUPLICATED_OP;
    }

    // 消息单元的创建和初始化
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
* 函数名称：DoCreateVmcfgAck
* 功能描述：处理收到的创建配置信息
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lixch         创建
* 2013/9/17     V1.1    johnir        handler/instance 解耦
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
        // 如果虚拟机实例已经存在了，则继续查找下一个
        if (NULL != _instance_pool->GetInstanceByID(it->_vid))
        {
            // 这个是配置请求的处理流程，是VMHANDLER的第一个收到配置的地方
            // 不应该出现这个情况，如果出现，加个异常打印
            OutPut(SYS_ALERT,
                   "In DoCreateVmcfgAck, instance %d exist!\n", it->_vid);
            continue;
        }

        // 申请一个虚拟机实例
        VmInstance *inst = new VmInstance(it->_config._vid);
        if (NULL == inst)
        {
            SkyExit(-1,"DoCreateVmcfgAck new VmInstance fail!\n");
            return ERROR;
        }

        // 恢复虚拟机的配置信息
        inst->SetVmCfg(it->_config);

        // 恢复option信息 /
        inst->SetVmOption(it->_option);

        // 初始化这个实例类
        if (SUCCESS != inst->Init())
        {
            SkyExit(-1,"DoCreateVmcfgAck VmInstances Init Fail!\n");
            return ERROR;
        }

        string inst_name = "instance_" + to_string<int64>(it->_config._vid,dec);
        MessageOption mo(inst_name, EV_POWER_ON, 0, 0);
        _mu->Send(inst_name, mo);

        //  加入到虚拟机管理池里面去
        _instance_pool->AddInstance(inst);

    }

    return VMOP_SUCC;
}

/****************************************************************************************
* 函数名称：TranceCCMsgToInstance
* 功能描述：转发消息到虚拟机实例的线程上去
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lixch         创建
****************************************************************************************/
void VmHandler::TranceCCMsgToInstance(const MessageFrame& message, uint32 vid)
{
    string recv = "instance_" + to_string<int64>(vid,dec);
    MID            receiver(recv);

    uint32 msgid    = message.option.id();

    // 构造消息接收者
    MessageOption option(receiver, msgid, 0, 0);

    // 构造转发消息
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
* 函数名称：TranceCCMsgToInstance
* 功能描述：转发消息到CC上去
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lixch         创建
****************************************************************************************/
void VmHandler::TranceInstanceMsgToCC(const MessageFrame& message, MID &receiver)
{
    // 构造消息接收者
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
* 函数名称：OpRunningInit
* 功能描述：设置实例的状态、转发消息给实例、回应答给命令下发者
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lixch         创建
*****************************************************************************************/
void VmHandler::OpRunningInit(VmInstance *inst, uint32 cmd,
                              const MessageFrame& message,
                              VmOperationReq &op_msg)
{
    //需要设置的标志给inst使用，统一放在这里
    if(CANCEL == cmd)
    {
        /* cancel抢占start时，可以让start结束申请存储的循环 */
        inst->VmSetDestroyFlag();
    }
    if(CANCEL_UPLOAD_IMG == cmd)
    {

        // 设置unsave标志，让实例正在执行的SAVE的时候能够停下来
        inst->SetInstanceUnSave();
    }

    // 转发消息到INSTANCES
    TranceCCMsgToInstance(message, op_msg._vid);

    // 回应答给命令下发这比如CC
}

/****************************************************************************************
* 函数名称：DoOpratePre
* 功能描述：虚拟机操作前处理，主要对虚拟机的实例和操作码进行
*                             检查
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：无
*
* 其它说明：如果lastcmd为空，设置初始信息并转发消息给instance处理；
*           lastcmd==newcmd?返回进度:(命令是否抢占?设置初始信息并转发消息给instance处理:返回busy)
*
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lixch         创建
****************************************************************************************/
void VmHandler::DoOpratePre(VmInstance     *inst, const MessageFrame& message,
                            uint32 op_cmd, VmOperationReq &op_msg)
{
    uint32  lastCmd = inst->GetOpCmd();

    /* 检查上一条运行的命令是否为空，命令结束后需要清空 */
    if(UNKNOWN_OP == lastCmd)
    {
        SkyAssert(!inst->IsOpRunning());

        //虚拟机非运行态，不能执行RR操作
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

    /* 检查新请求与上一次请求是否相同 */
    if(lastCmd == op_cmd)
    {
        inst->SendVmOprAckMsg(message.option.sender(), op_msg.action_id, _mu);
    }
    else
    {
        /* 检查是否可以抢占 */
        if(LegalOperation(op_cmd, lastCmd))
        {
            OpRunningInit(inst, op_cmd, message, op_msg);
        }
        else
        {
            // 发送系统正忙的应答给CC
            inst->SendVmOprAckMsg(message.option.sender(), VMOP_VM_AGT_BUSY,
                                  op_cmd, 1, op_msg.action_id, _mu);
        }
    }

    return;
}

/****************************************************************************************
* 函数名称：DoOprate
* 功能描述：VMHANDLER收到各类消息的处理流程
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lixch         创建
****************************************************************************************/
void VmHandler::DoRecvNormalOp(const MessageFrame& message, uint32 op_cmd)
{
    VmInstance     *inst = NULL;
    VmOperationReq op_msg;

    op_msg.deserialize(message.data);

    // 获取虚拟机实例
    inst = _instance_pool->GetInstanceByID(op_msg._vid);
    if (NULL == inst)
    {
        OutPut(SYS_NOTICE,"DoRecvNormalOp: GetInstanceByID fail vid = %d op_cmd = %d!\n",
               op_msg._vid, op_cmd);
        return;
    }

    // 清除UNSAVE状态，该接口会被SAVE调用
    inst->ClearInstanceUnSave();

    // 预处理失败，则返回不继续走了
    DoOpratePre(inst, message, op_cmd, op_msg);

    return;    
}

/****************************************************************************************
* 函数名称：DoRecvUnsave
* 功能描述：VMHANDLER收到CANCEL_UPLOAD_IMG消息的处理流程
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lixch         创建
****************************************************************************************/
void VmHandler::DoRecvUnsave(const MessageFrame& message)
{
    VmInstance     *inst = NULL;
    VmOperationReq op_msg;

    op_msg.deserialize(message.data);

    // 获取虚拟机实例
    inst = _instance_pool->GetInstanceByID(op_msg._vid);
    if (NULL == inst)
    {
        OutPut(SYS_NOTICE,"DoRecvUnsave: GetInstanceByID fail vid = %d!\n", op_msg._vid);
        return;
    }

    // 预处理失败，则返回不继续走了
    DoOpratePre(inst, message, CANCEL_UPLOAD_IMG, op_msg);

    return;
}


/****************************************************************************************
* 函数名称：DoRecvReset
* 功能描述：VMHANDLER收到RESET消息的处理流程
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lixch         创建
****************************************************************************************/
void VmHandler::DoRecvReset(const MessageFrame& message)
{
    VmInstance     *inst = NULL;
    VmOperationReq op_msg;

    op_msg.deserialize(message.data);

    // 获取虚拟机实例
    inst = _instance_pool->GetInstanceByID(op_msg._vid);
    if (NULL == inst)
    {
        OutPut(SYS_NOTICE,"DoRecvReset: GetInstanceByID fail vid = %d!\n", op_msg._vid);
        return;
    }

    // 预处理失败，则返回不继续走了
    DoOpratePre(inst, message, RESET, op_msg);

    return;
}

/****************************************************************************************
* 函数名称：DoRecvDestroy
* 功能描述：VMHANDLER收到DESTROY消息的处理流程
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lixch         创建
****************************************************************************************/
void VmHandler::DoRecvDestroy(const MessageFrame& message)
{
    VmInstance     *inst = NULL;
    VmOperationReq op_msg;

    op_msg.deserialize(message.data);

    // 获取虚拟机实例
    inst = _instance_pool->GetInstanceByID(op_msg._vid);
    if (NULL == inst)
    {
        OutPut(SYS_NOTICE,"DoRecvDestroy: GetInstanceByID fail vid = %d!\n", op_msg._vid);
        return;
    }

    // 预处理失败，则返回不继续走了
    DoOpratePre(inst, message, DESTROY, op_msg);
    
    return;
}

/****************************************************************************************
* 函数名称：DoRecvCreate
* 功能描述：VMHANDLER收到CREATE消息的处理流程
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lixch         创建
****************************************************************************************/
void VmHandler::DoRecvCreate(const MessageFrame& message)
{
    VmInstance     *inst = NULL;
    VmOperationReq op_msg;

    op_msg.deserialize(message.data);

    // 获取虚拟机实例
    inst = _instance_pool->GetInstanceByID(op_msg._vid);
    if (NULL == inst)
    {
        // 申请一个虚拟机实例
        inst = new VmInstance(op_msg._vid);
        if (NULL == inst)
        {
            SkyExit(-1,"DoRecvCreate new VmInstance fail!\n");
            return;
        }

        // 初始化这个实例类
        if (SUCCESS != inst->Init())
        {
            SkyExit(-1,"DoRecvCreate VmInstances Init Fail!\n");
            return;
        }

        string inst_name = "instance_" + to_string<int64>(op_msg._vid, dec);
        MessageOption mo(inst_name, EV_POWER_ON, 0, 0);
        _mu->Send(inst_name, mo);

        //  加入到虚拟机管理池里面去
        _instance_pool->AddInstance(inst);
        // 设置这个实例类的相关参数 转发消息给实例
        OpRunningInit(inst, CREATE, message, op_msg);
        return;
    }

    // 预处理失败，则返回不继续走了
    DoOpratePre(inst, message, CREATE, op_msg);
    
    return;
}

/****************************************************************************************
* 函数名称：DoRecvCancel
* 功能描述：VMHANDLER收到CANCEL消息的处理流程
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lixch         创建
****************************************************************************************/
void VmHandler::DoRecvCancel(const MessageFrame& message)
{
    VmInstance     *inst = NULL;
    VmOperationReq op_msg;

    op_msg.deserialize(message.data);

    // 获取虚拟机实例
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
     
    // 预处理失败，则返回不继续走了
    DoOpratePre(inst, message, CANCEL, op_msg);
    
    return ;
}

/****************************************************************************************
* 函数名称：VmHandlerExecVmOp
* 功能描述：执行虚拟机操作
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/07/16    V1.0    lixch         创建
****************************************************************************************/
void VmHandler::VmHandlerExecVmOp(const MessageFrame& message)
{
    VmOperationReq  req;
    VmOperation     opr_id;

    // 解析消息结构，获取操作命令
    req.deserialize(message.data);
    opr_id        = (VmOperation)(req._operation);
    MID   sender  = message.option.sender();
    // 调试打印，记录当前系统收到什么样的命令
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
    // 目的端准备迁移
    case PREP_M:
    {
        DoVmMigrationPerpare(message);
        break;
    }
    // 源端执行迁移 转发消息到instance
    case MIGRATE:
    {
        DoVmMigrationExec(message);
        break;
    }
    // 源端迁移成功
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
* 函数名称：SendVmAgtCfgReq
* 功能描述：上报VM配置信息请求信息给CC
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/07/10    V1.0    lixch      创建
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
* 函数名称：SendVmAgtCfgReq
* 功能描述：上报VM配置信息请求信息给CC
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/07/10    V1.0    lixch      创建
****************************************************************************************/
void VmHandler::StartVmAgetCfgReqTimer()
{
    // 创建定时器或者停止定时器
    if (INVALID_TIMER_ID == _cfg_req_timer_id)
    {
        if (INVALID_TIMER_ID == (_cfg_req_timer_id = _mu->CreateTimer()))
        {
            OutPut(SYS_EMERGENCY, "StartVmAgetCfgReqTimer: CreateTimer failed!\n");
            SkyExit(-1, "VmHandler::StartVmAgetCfgReqTimer: CreateTimer failed!");
            SkyAssert(0);
        }
    }

    // 设置主机信息上报循环定时器
    TimeOut timeout;
    timeout.type = RELATIVE_TIMER;
    timeout.duration = 5 * 1000;// 5s发送一次配置请求
    timeout.msgid = EV_VMCFG_REQ_TIMER;
    _mu->SetTimer(_cfg_req_timer_id, timeout);
}

void VmHandler::StartReclaimDevTimer()
{
    // 创建定时器或者停止定时器
    if (INVALID_TIMER_ID == _reclaimdev_timer_id)
    {
        if (INVALID_TIMER_ID == (_reclaimdev_timer_id = _mu->CreateTimer()))
        {
            OutPut(SYS_ERROR, "StartReclaimDevTimer: CreateTimer failed!\n");
            SkyExit(-1, "VmHandler::StartReclaimDevTimer: CreateTimer failed!");
            SkyAssert(0);
        }
    }

    // 设置回收设备定时器
    TimeOut timeout;
    timeout.type = LOOP_TIMER;
    timeout.duration = 10 * 1000;//
    timeout.msgid = EV_RECLAIM_DEV_TIMER;
    _mu->SetTimer(_reclaimdev_timer_id, timeout);
}

/****************************************************************************************
* 函数名称：DoVmMigrationPerpare
* 功能描述：处理目的端准备虚拟机迁移
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/7/16    V1.0    lixch         创建
****************************************************************************************/
void VmHandler::DoVmMigrationPerpare(const MessageFrame& message)
{
    int64           vm_id;
    VmMigrateData   prepare;
    VmInstance      *inst  = NULL;

    prepare.deserialize(message.data);
    vm_id         = prepare._vid;

    OutPut(SYS_DEBUG, "In DoVmMigrationPerpare, recv req --- vm_id: %lld\n", vm_id);

    // 先看是否之前部署过
    inst = _instance_pool->GetInstanceByID(vm_id);
    // 部署过该instance,不能进行迁移准备
    if (NULL != inst)
    {
        // 预处理失败，则返回不继续走了
        DoOpratePre(inst, message, PREP_M, prepare);
        return;
    }

    // 申请一个虚拟机实例
    inst = new VmInstance(vm_id);
    if (NULL == inst)
    {
        SkyExit(-1,"DoVmMigrationPerpare new VmInstance fail!\n");
        return;
    }

    // 初始化这个实例类
    if (SUCCESS != inst->Init())
    {
        SkyExit(-1,"DoVmMigrationPerpare VmInstances Init Fail!\n");
        return;
    }

    string inst_name = "instance_" + to_string<int64>(prepare._vid, dec);
    MessageOption mo(inst_name, EV_POWER_ON, 0, 0);
    _mu->Send(inst_name, mo);

    //  加入到虚拟机管理池里面去
    _instance_pool->AddInstance(inst);

    OpRunningInit(inst, PREP_M, message, prepare);
}


/****************************************************************************************
* 函数名称：DoVmMigrationExec
* 功能描述：源端执行虚拟机迁移的VMHANDLER处理
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/7/16     V1.0    lixch         创建
****************************************************************************************/
void VmHandler::DoVmMigrationExec(const MessageFrame& message)
{
    int64          vm_id;
    VmMigrateData  migrate;

    migrate.deserialize(message.data);
    vm_id         = migrate._vid;

    OutPut(SYS_DEBUG, "In DoVmMigrationExec, recv req --- vm_id: %lld\n", vm_id);

    VmInstance *inst = _instance_pool->GetInstanceByID(vm_id);
    // 如果实例不存在，说明迁移成功了，直接返回成功
    if (NULL == inst)
    {
        OutPut(SYS_DEBUG, "DoVmMigrationExec: inst does not exist!\n");
        // 申请一个虚拟机实例
        inst = new VmInstance(vm_id);
        if (NULL == inst)
        {
            SkyExit(-1,"DoRecvCreate new VmInstance fail!\n");
            return;
        }
        // 发送成功应答给CC
        inst->SendVmOprAckMsg(message.option.sender(),
                              VMOP_SUCC, MIGRATE, 100, migrate.action_id, _mu);
        // 删除实例
        delete inst;

        return;
    }

    // 预处理失败，则返回不继续走了
    DoOpratePre(inst, message, MIGRATE, migrate);
    
    return;
}

/****************************************************************************************
* 函数名称：DoDelVmIntanceReq
* 功能描述：销毁instance线程
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/7/16     V1.0    lixch         创建
****************************************************************************************/
void VmHandler::DoDelVmIntanceReq(const MessageFrame& message)
{
    int64           vm_id;
    VmOperationAck  msg;

    msg.deserialize(message.data);
    vm_id         = msg._vid;

    OutPut(SYS_DEBUG, "DoDelVmIntanceReq: rcv req from vm_id = %lld.", vm_id);

    VmInstance *inst = _instance_pool->GetInstanceByID(vm_id);
    // 实例不存在，断言，记录错误，不回应答
    if (NULL == inst)
    {
        OutPut(SYS_EMERGENCY, "DoDelVmIntanceReq: inst does not exist!\n");
        SkyAssert(false);
        return;
    }

    // 只处理 MIGRATE 和CANCEL 反之断言
    if ((MIGRATE != msg._operation) && (CANCEL != msg._operation))
    {
        OutPut(SYS_EMERGENCY, "DoDelVmIntanceReq: recv err operate %d (%s)!\n", \
               msg._operation, Enum2Str::VmOpStr(msg._operation));
        SkyAssert(false);
    }

    // 获取发送地址
    MID receiver;
    inst->GetOpSender(receiver);

    // 如果删除实例失败，这个不可能存在，断言，记录错误
    if (!_instance_pool->DelInstance(vm_id))
    {
        OutPut(SYS_EMERGENCY, "DoDelVmIntanceReq: inst del fail!\n");
        SkyAssert(false);
    }

    //多余? 进度和结果不是100&succ，实例删除无法设置，需要跟催时根据null设置
    TranceInstanceMsgToCC(message, receiver);
}

/****************************************************************************************
* 函数名称：TransVmMigrateSucc
* 功能描述：转发到instance,源端迁移成功，通知目的端网络进行切换
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/7/17     V1.0    lixch         创建
****************************************************************************************/
void VmHandler::DoVmMigrationPost_M(const MessageFrame& message)
{
    int64         vm_id;
    VmMigrateData  inform;

    inform.deserialize(message.data);
    vm_id         = inform._vid;

    OutPut(SYS_DEBUG, "In DoVmMigrationPost_M, recv req --- vm_id: %lld\n", vm_id);

    VmInstance *inst = _instance_pool->GetInstanceByID(vm_id);
    // 如果实例不存在，这个不可能存在，断言，记录错误，不回应答
    if (NULL == inst)
    {
        OutPut(SYS_EMERGENCY, "DoVmMigrationExec: inst does not exist!\n");
        SkyAssert(false);
        return;
    }

    // 预处理失败，则返回不继续走了
    DoOpratePre(inst, message, POST_M, inform);
    
    return;
}


/****************************************************************************************
* 函数名称：DoRecvQueryUsbInfo
* 功能描述：获取USB信息的查询函数
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/7/17     V1.0    lixch         创建
****************************************************************************************/
void VmHandler::DoRecvQueryUsbInfo(const MessageFrame& message)
{
    VmUsbListAck usb_result_list;

    VmUsbListAck recv_req;
    recv_req.deserialize(message.data);

    // 先赋值CC传过来的内容
    usb_result_list._stamp = recv_req._stamp;
    usb_result_list._ret = SUCCESS;

    GetUsbList(usb_result_list._info);

    MessageOption option(message.option.sender(), EV_VMCFG_USB_INFO_QUERY_ACK,0,0);
    _mu->Send(usb_result_list, option);
}


/****************************************************************************************
* 函数名称：DoVmWdgOperAck
* 功能描述：收到看门狗操作应答的处理函数(hc <- vna)
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/7/16     V1.0    lixch         创建
****************************************************************************************/
void VmHandler::DoVmWdgOperAck(const MessageFrame& message)
{
    int64           vm_id;
    CVNetWatchDogMsgAck  msg;

    msg.deserialize(message.data);
    vm_id         = msg.vm_id;

    OutPut(SYS_DEBUG, "In DoVmWdgOperAck, recv OpAck --- vm_id: %lld\n", vm_id);

    VmInstance *inst = _instance_pool->GetInstanceByID(vm_id);
    // 如果实例不存在，这个不可能存在，断言，记录错误，不回应答
    if (NULL == inst)
    {
        OutPut(SYS_EMERGENCY, "DoRecvOpAck: inst does not exist, vid:%d. discard!\n", vm_id);
        //SkyAssert(false);
        return;
    }


    string recv = "instance_" + to_string<int64>(vm_id,dec);
    MID            receiver(recv);

    // 构造消息接收者
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
* 函数名称：GetUsbList
* 功能描述：获取USB信息到列表上去
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/7/17     V1.0    lixch         创建
****************************************************************************************/
void VmHandler::GetUsbList(vector<VmQueryUsbInfo> &list)
{
    // 获取HC上的USB信息
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
        // 先获取一个段落组合
        if (result.find("T:") != string::npos)
        {
            // 会有一个NULL记录，后面去除
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

        // 查找是否是控制器
        string::size_type loc = result.find("B:");
        if (loc != string::npos)
        {
            is_usb_controller = true;
            continue;
        }

        // 查找是否是控制器
        loc = result.find("D:");
        if (loc != string::npos)
        {
            if (string::npos != result.find("hub"))
            {
                is_usb_controller = true;
            }
            continue;
        }

        // 查找Manufacturer
        loc = result.find("Manufacturer=");
        if (loc != string::npos)
        {
            usb_info.manufacturer =
                result.substr(loc+strlen("Manufacturer="));
        }

        // 查找vendor_id
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
    // 保存最后一个USB信息
    if (!is_usb_controller)
    {
        RecodUsbToList(usb_info, list);
    }
}

/****************************************************************************************
* 函数名称：RecodUsbList
* 功能描述：记录USB信息到记录列表中的函数
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2012/7/17     V1.0    lixch         创建
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
* 函数名称：MessageEntry
* 功能描述：虚拟机操作消息入口
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------------------------
* 2011/08/12    V1.0    lixch         创建
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
            // 发送虚拟机配置请求给CC
            SendVmAgtCfgReq();
            // 启动定时器发送虚拟机配置请求
            StartVmAgetCfgReqTimer();
            break;
        }
        case EV_VMAGT_CFG_ACK:
        {
            // 删除定时器
            _mu->KillTimer(_cfg_req_timer_id);
            // 开始根据配置信息来创建对应的虚拟机实例
            DoCreateVmcfgAck(message);
#if 0
            StartReclaimDevTimer();
#endif
            // 进入到WORK态
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
        // 收到VM_instances 的删除请求
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


/* 检查命令是否可以抢占，需要与CC上的VMManager::LegalOperation()保持一致 */
bool VmHandler::LegalOperation(uint32 op, uint32 last_op)
{
    OutPut(SYS_DEBUG, "hc LegalOperation:newcmd(%d),oldcmd(%d).\n", op, last_op);

    switch (op)
    {
        case CANCEL:
        {
            return (MIGRATE != last_op) && (COLD_MIGRATE != last_op) && (LIVE_MIGRATE != last_op)   //撤销虚拟机，必须等热迁移有个结果
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

    /* 2. 创建临时消息单元 */
    MessageUnit temp_mu(TempUnitName("vm_handler_test"));
    temp_mu.Register();

    /* 3. 发消息给 VM_HANDLER  */
    MessageOption option(MID(PROC_HC, MU_VM_HANDLER),
                         EV_VM_OP,0,0);
    temp_mu.Send(temp, option);

    /* 4. 等待MU_VMCFG_MANAGER回应 */
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

    /* 2. 创建临时消息单元 */
    MessageUnit temp_mu(TempUnitName("test_vm_op"));
    temp_mu.Register();

    /* 3. 发消息给 VM_HANDLER  */
    MessageOption option(MID(PROC_HC, MU_VM_HANDLER),
                         EV_VM_OP,0,0);
    temp_mu.Send(req, option);

    /* 4. 等待MU_VMCFG_MANAGER回应 */
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

