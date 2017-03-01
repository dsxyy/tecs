/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：vmcfg_manager.h
* 文件标识：
* 内容摘要：VmCfgManager类的定义文件
* 当前版本：1.0
* 作    者：Bob
* 完成日期：2011年7月26日
*
* 修改记录1：
*    修改日期：2011/7/26
*    版 本 号：V1.0
*    修 改 人：Bob
*    修改内容：创建
*******************************************************************************/
#ifndef VMCFG_MANGER_VMCFG_MANAGER_H        
#define VMCFG_MANGER_VMCFG_MANAGER_H
#include "sky.h"
#include "mid.h"
#include "event.h"
#include "vmcfg_pool.h"
#include "vm_messages.h"
#include "image_manager_with_api_method.h"
#include "cluster_manager_with_vm_config.h"
#include "vmcfg_manager_with_file_manager.h"
#include "image_manager_with_api_method.h"
#include "vnet_libnet.h"
#include "affinity_region_pool.h"

namespace zte_tecs
{

enum FeedBackFunc
{
    NONE = 0,
    FEEDBACK_RESTART_ACTION,
    FEEDBACK_ROLLBACK_WORKFLOW,
    FEEDBACK_DELETE_WORKFLOW,
    FEEDBACK_MODIFY_SUCCESS,
    FEEDBACK_NULL,//空操作，增加这个的目的是为了不让engine自动将action结束
    FEEDBACK_MODIFY_FAILED,
};

class VmCfgDeployWorkflowLabels: public WorkflowLabels
{
public:

    VmCfgDeployWorkflowLabels(): vid(label_int64_1){};

    int64 &vid;
};

class VmCfgModifyAction : public Action
{
public:
    VmCfgModifyAction() : _vid(labels.label_int64_1) {};
    VmCfgModifyAction(const VmCfgModifyIndividualItem &modification)
                            : Action(VM_ACT_CATEGORY, VM_ACT_MODIFY_VMCFG_INDIVIDUAL_ITEM, EV_VM_OP, 
                            MID(MU_VMCFG_MANAGER), MID(MU_VMCFG_MANAGER)), _vid(labels.label_int64_1)
    {
        success_feedback = 0;
        failure_feedback = 0;
        timeout_feedback = 0;

        _vid = modification._vid;

        VmModifyReq req(modification);
        req.action_id = get_id();
        message_req = req.serialize();

        failure_feedback = FEEDBACK_DELETE_WORKFLOW;
    }

    VmCfgModifyAction(const string& _category,const string& _name,int _message_id,const MID& _sender,const MID& _receiver):
                            Action(_category, _name, _message_id, _sender, _receiver), _vid(labels.label_int64_1)
    {
        success_feedback = 0;
        failure_feedback = 0;
        timeout_feedback = 0;
    }

    void GenPrepareImageAction(const VmBaseImage &image, const string &cluster)
    {
        ImagePrepareMsg prepare_img_msg(get_id(), image._id, image._image_id, cluster);
        message_req = prepare_img_msg.serialize();

        success_feedback = 0;
        failure_feedback = FEEDBACK_DELETE_WORKFLOW;

        skipped = (image._position == IMAGE_POSITION_LOCAL);
    }

    //类的内部有了引用，就没有默认赋值函数，而vector操作必须要支持赋值函数
    VmCfgModifyAction &  operator=(const VmCfgModifyAction &src)
    {
        Action::operator=(src);
        return *this;
    }

    ~VmCfgModifyAction(){};

    int64   &_vid;
};

class VmCfgModifyWorkflow : public Workflow
{
public:
    VmCfgModifyWorkflow(const string& _name) : Workflow(VM_WF_CATEGORY, _name), _vid(labels.label_int64_1)
    {
    }
    VmCfgModifyWorkflow(const string& _category, const string& _name) : Workflow(_category, _name), _vid(labels.label_int64_1)
    {
    }
    ~VmCfgModifyWorkflow(){};

    void AddModifyActions(const vector<VmCfgModifyAction> &actions)
    {
        if (actions.size() > 1)
        {
            vector<VmCfgModifyAction>::const_iterator it;
            for (it = actions.begin(); it != actions.end() - 1; it ++)
            {
                Workflow::Add(*it, *(it+1));
            }
        }
        else if(actions.size() == 1)
        {
            Workflow::Add(*actions.begin());
        }
    }

    int64   &_vid;
};

class FuncResponseFeedback
{
public:
    int ResponseWorkAck(const WorkAck &ack);
    int CommonResponseFeedback(const int func, const WorkAck &ack);
    virtual int ResponseFeedback(const int func, const WorkAck &ack)
    {
        return 0;
    };
};

class CandidateClusters: Serializable
{
public:
    CandidateClusters()
    {};

    CandidateClusters(const vector<string>  &clusters)
    {
        _clusters = clusters;
    };
    
    SERIALIZE
    {
        SERIALIZE_BEGIN(CandidateClusters);
        WRITE_VALUE(_clusters);            
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(CandidateClusters);
        READ_VALUE(_clusters);           
        DESERIALIZE_END();
    };
    vector<string>  _clusters;
};

class  SelectClusterLabels: public ActionLabels
{
public:
    SelectClusterLabels ( ):_cluster_by_compute(label_string_1),
                              _cluster_by_share_storage(label_string_2),
                              _is_need_share_storage(label_int64_1),
                              _cluster_by_network(label_string_3),
                              _is_need_network(label_int64_2),
                              _des_cluster(label_string_4)
    {
        label_int64_1 = 0;
        label_int64_2 = 0;
    };
    
    string   &_cluster_by_compute;
    
    string   &_cluster_by_share_storage;
    int64    &_is_need_share_storage;
    
    string   &_cluster_by_network;
    int64    &_is_need_network;

    string   &_des_cluster;
};

class  GetCoreDumpUrlLabels: public ActionLabels
{
public:

    GetCoreDumpUrlLabels(): str_url(label_string_1){};

    string &str_url;
};

class AllocatedNetworkRes: Serializable
{
public:
    AllocatedNetworkRes()
    {};

    AllocatedNetworkRes(const vector<CVNetVnicConfig>  &nic)
    {
        _nics = nic;
    };
    
    SERIALIZE
    {
        SERIALIZE_BEGIN(AllocatedNetworkRes);
        WRITE_OBJECT_ARRAY(_nics);            
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(AllocatedNetworkRes);
        READ_OBJECT_ARRAY(_nics);           
        DESERIALIZE_END();
    };
    vector<CVNetVnicConfig>  _nics;
};


class  AllocateNetworkLabels: public ActionLabels
{
public:
    AllocateNetworkLabels ( ):_allocated_nics(label_string_1)
    {       
    };

    string   &_allocated_nics;
};


class  PrepareDeployMsg: public  WorkReq
{     
public:
    int64  _vid; 
    PrepareDeployMsg(){};
    PrepareDeployMsg(const string &id_of_action):WorkReq(id_of_action){};
    PrepareDeployMsg(const string &id_of_action, int64 vid):WorkReq(id_of_action)
    {
        _vid = vid;
    };
    
    ~PrepareDeployMsg(){};

    SERIALIZE
    {
        SERIALIZE_BEGIN(PrepareDeployMsg);
        WRITE_DIGIT(_vid);
        SERIALIZE_PARENT_END(WorkReq);    
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(PrepareDeployMsg);
        READ_DIGIT(_vid);
        DESERIALIZE_PARENT_END(WorkReq);
    };
};

/**
@brief 功能描述: 映像资源池的实体类\n
@li @b 其它说明: 无
*/
class VmCfgManager: public MessageHandler, public FuncResponseFeedback
{
public:
    static VmCfgManager *GetInstance()
    {
        if(NULL == instance)
        {
            instance = new VmCfgManager(); 
        }
        
        return instance;
    };
    
    ~VmCfgManager( )
    {
        if (NULL != _vcpool)
        {
            delete _vcpool;
        }
    
        if (NULL != _mu)
        {
            delete _mu;
        } 
    };

    STATUS Init(SqlDB *db)
    {
        // 创建 VmCfgPool对象
        _vcpool = VmCfgPool::CreateInstance(db);
        if(!_vcpool)
        {
            return ERROR_NOT_READY;
        }
         _vcpool->Init(false);

        _arpool = AffinityRegionPool::CreateInstance(db);
        if(!_arpool)
        {
            return ERROR_NOT_READY;
        }

        //启动消息单元工作线程
        return StartMu(MU_VMCFG_MANAGER);
    }


private:
    static VmCfgManager *instance;
    VmCfgManager();

    STATUS StartMu(const string& name);
    virtual void MessageEntry(const MessageFrame &message); 
    
    void DealVmOp(MID receiver, VmOperationReq &req, const MessageFrame &message);
    void DealAllocate(MID receiver,const VmAllocateReq &req);
    void DealDeploy(MID receiver, const VmOperationReq &req, VmCfg &vmcfg, VmOperationAck &ack);
    void DealCancel(MID receiver, const VmOperationReq &req, VmCfg &vmcfg, VmOperationAck &ack);
    void DealAffregAllocateReq(AffinityRegion& req);
    STATUS CreateDeployWF(MID receiver, VmCfg &vm, const VmOperationReq &req, string &workflow_id);
    STATUS CreateCancelWF(MID receiver, VmCfg &vm, const VmOperationReq &req, string &workflow_id);    
    void DealImagePrepareAck(const ImagePrepareAckMsg &ack);
    void DealCoreDumpUrlAck(const UploadUrl &ack);
    void DealPrepareDeployMsgReq(const PrepareDeployMsg &req);
    STATUS GenerateDeployMsg(const PrepareDeployMsg &req, VMDeployInfo &req_msg);
    void DealAck(const VmOperationAck &ack);
    void DealCCAck(const VmOperationAck &ack);
    bool FindVmCfgWorkFlow(vector<string>& workflows,
                                    const string& workflow_name,
                                    int64    vid,
                                    bool     is_rollback);

    void VmRollbackWorkflow(string action_id);
    void Ack2Api(MID receiver, VmOperationAck &ack);
    void Ack2Api(const string &action_id, VmOperationAck &ack);
    bool Authorize(int64 opt_uid, VmCfg *buf, int oprate);
    bool CheckVmIsAllowDeploy(int64 vid,ostringstream &err_oss);
    STATUS DealFilterComputeAck(const FilterClusterByComputeAck &msg);

    STATUS DealFliterShareStorageAck(const FilterClusterByShareStorageAck &msg);

    STATUS DealFilterNetworkAck(const CVNetSelectCCListMsgAck &msg);

    STATUS DealSelectCluster(const VmOperationReq &msg);

    STATUS DealAllocateNetworkAck(const CVNetGetResourceCCMsgAck &msg);
    
    STATUS SelectPolicy(const vector<string> &cluster_in, string &cluster_out);
    
    STATUS DealFreeNetworkAck(const CVNetFreeResourceCCMsgAck &msg);
    STATUS BatchModifyVmCfg(const MessageFrame &message);
    STATUS BatchModifyVmCfg(const VmCfgBatchModify &modify_req, VmOperationAck &result);
    STATUS CheckModifyValid(const VmCfgBatchModify &modify_req, ostringstream &err_oss);
    void DealModify(const MessageFrame &message);
    int ResponseFeedback(const int func, const WorkAck &ack);
    STATUS AcktoWorkflowUpstreamSender(const WorkAck &ack);
    STATUS EffectModificationThisLayer(const WorkAck &ack);
    STATUS CheckVmIsAllowCancel(int64 vid, ostringstream &err_oss);


    MessageUnit *_mu;
    VmCfgPool   *_vcpool;
    AffinityRegionPool * _arpool;
 
    DISALLOW_COPY_AND_ASSIGN(VmCfgManager); 

};

}
#endif /* VMCFG_MANGER_VMCFG_MANAGER_H */

