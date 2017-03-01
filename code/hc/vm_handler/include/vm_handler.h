/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：vm_handler.h
* 文件标识：
* 内容摘要：VmHandler类的定义文件
* 当前版本：1.0
* 作    者：lixiaocheng
* 完成日期：2012年7月03日
*
* 修改记录1：
*     修改日期：2012/7/03
*     版 本 号：V1.0
*     修 改 人：lixiaocheng
*     修改内容：创建
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

// 引用虚拟机实例的管理池类
class InstancePool;

// vmhandler转发消息到instance的结构
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


// 定义VmHandler的类
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
       
    //析构函数
    ~VmHandler()
    {
        delete _mu;
    };

    STATUS Init();

    // 处理收到的虚拟机配置ACK
    STATUS DoCreateVmcfgAck(const MessageFrame& message);

    // 转发CC的消息到对应的虚拟机实例上去
    void TranceCCMsgToInstance(const MessageFrame& message, uint32 vid);

    // 转发虚拟机实例上的消息到CC,一般只转发CANCEL的应答和迁移结束的应答
    void TranceInstanceMsgToCC(const MessageFrame& message, MID &receiver);

    // 收到虚拟机操作消息的处理流程
    void DoRecvNormalOp(const MessageFrame& message, uint32 op_cmd);

    // 收到虚拟机操作消息的处理流程
    void DoRecvUnsave(const MessageFrame& message);

    // 收到虚拟机RESET消息的处理流程
    void DoRecvReset(const MessageFrame& message);

    // 收到虚拟机Destroy消息的处理流程
    void DoRecvDestroy(const MessageFrame& message);

    // 收到虚拟机Create消息的处理流程
    void DoRecvCreate(const MessageFrame& message);

    // 收到虚拟机Cancel消息的处理流程
    void DoRecvCancel(const MessageFrame& message);

    // 收到虚拟机OPRATE消息的总入口
    void VmHandlerExecVmOp(const MessageFrame& message);

    // 发送虚拟机配置请求函数
    void SendVmAgtCfgReq();

    // 启动虚拟机配置请求的定时器
    void StartVmAgetCfgReqTimer();
    //启动磁阵回收定时器
    void StartReclaimDevTimer();

    // 处理虚拟机迁移准备的消息
    void DoVmMigrationPerpare(const MessageFrame& message);

    // 处理虚拟机迁移执行消息
    void DoVmMigrationExec(const MessageFrame& message);

    // 处理收到虚拟机实例回过来的应答消息，一般处理CANCEL和POST_M消息，主要释放实例
    void DoDelVmIntanceReq(const MessageFrame& message);

    // 处理虚拟机迁移成功后，通知网络切换的消息
    void DoVmMigrationPost_M(const MessageFrame& message);

    // 获取VmHandler的工作状态
    int GetVmHandlerWorkState()
    {
        return _mu->get_state();
    };

    bool LegalOperation(uint32 op, uint32 last_op);

    // 消息单元入口
    void MessageEntry(const MessageFrame& frame);

    void DoRecvQueryUsbInfo(const MessageFrame& message);

    void DoVmWdgOperAck(const MessageFrame& message);

    static void GetUsbList(vector<VmQueryUsbInfo> &list);

    static void RecodUsbToList(VmQueryUsbInfo &info, vector<VmQueryUsbInfo> &usb_list);

    
private:
    static VmHandler  *_instance;

    // 启动消息单元
    STATUS StartMu(const string& name);

    // VMHANDLER上OP操作进行中的初始化函数
    void OpRunningInit(VmInstance *inst, uint32 cmd, 
                                         const MessageFrame& message,
                                          VmOperationReq &op_msg);  

    void DoOpratePre(VmInstance     *inst, const MessageFrame& message,
                               uint32 op_cmd, VmOperationReq &op_msg);
    
    // 构造函数
    VmHandler():_mu(NULL),_vm_driver(NULL),_instance_pool(NULL),_cfg_req_timer_id(INVALID_TIMER_ID),_reclaimdev_timer_id(INVALID_TIMER_ID)
    {
    }
    
    MessageUnit       *_mu;                      // 消息单元
    VmDriver          *_vm_driver;               // 虚拟机驱动指针
    InstancePool      *_instance_pool;           // 虚拟机实例池
    TIMER_ID          _cfg_req_timer_id;         // 虚拟机配置请求定时器
    TIMER_ID          _reclaimdev_timer_id;         // 虚拟机配置请求定时器
};
#endif // #ifndef HC_VMHANDLER_H

