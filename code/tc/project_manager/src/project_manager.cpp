/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：project_manager.cpp
* 文件标识：见配置管理计划书
* 内容摘要：ProjectManager类的实现文件
* 当前版本：1.0 
* 作    者：yw
* 完成日期：2013年7月28日
*
* 修改记录1：
*    修改日期：2013/7/28
*    版 本 号：V1.0
*    修 改 人：yw
*    修改内容：创建
*******************************************************************************/
#include "project_manager.h"
#include "project.h"
#include "sky.h"
#include "db_config.h"
#include "workflow.h"
#include "volume.h"
#include "project_api.h"
#include "project_operate.h"
#include "authrequest.h"
#include "authmanager.h"
#include "affinity_region_pool.h"

namespace zte_tecs
{
/******************************************************************************/
ProjectManager  *ProjectManager::instance = NULL;

/************************************************************
* 函数名称： StartMu
* 功能描述： 启动mu的函数
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无  
* 返 回 值： 无
* 其它说明： 
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2012/5         V1.0       yw         创建
***************************************************************/
STATUS ProjectManager::StartMu(const string& name)
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
/******************************************************************************/
void ProjectManager::ChangeVmRepels(const vector<VidChange>& vcs, vector<VmRepel>& repels)
{
    vector<VmRepel>::iterator repel_it;
    vector<VidChange>::const_iterator vc_it;
    for (repel_it = repels.begin(); repel_it != repels.end(); repel_it++)
    {
        for (vc_it = vcs.begin(); vc_it != vcs.end(); vc_it++)
        {
            //先轮询一遍vid1
            if (vc_it->_old_vid == repel_it->_vid1)
            {
                //找到改好退出
                repel_it->_vid1 = vc_it->_new_vid;
                break;
            }
        }
        for (vc_it = vcs.begin(); vc_it != vcs.end(); vc_it++)
        {
            if (vc_it->_old_vid == repel_it->_vid2)
            {
                repel_it->_vid2 = vc_it->_new_vid;
                break;
            }
        }
    }
}
/******************************************************************************/
STATUS ProjectManager::ChangeVmAffregs(const vector<VidChange>& vcs, vector<VmAffReg>& affregs)
{
    vector<VmAffReg>::iterator affreg_it;
    vector<int64>::iterator vid;
    vector<VidChange>::const_iterator vc_it;
    VmCfgPool     *vcpool = NULL;
    int64 ar_id;
    int     rc  = -1;
    vcpool = VmCfgPool::GetInstance();

    AffinityRegionPool *_ppool = AffinityRegionPool::GetInstance();

    for (affreg_it = affregs.begin(); affreg_it != affregs.end(); affreg_it++)
    {        
        affreg_it->aff_region._create_time.refresh();
        rc = _ppool->Allocate(affreg_it->aff_region);
        if (rc < 0)
        {
            OutPut(SYS_DEBUG,"Allocate affreg fail");
            return ERR_AFFINITY_REGION_ALLOC_FAIL;
        }
        ar_id = affreg_it->aff_region.get_oid();

        for(vid = affreg_it->_vid.begin(); vid != affreg_it->_vid.end(); vid++)
        {
            for (vc_it = vcs.begin(); vc_it != vcs.end(); vc_it++)
            {
                if (vc_it->_old_vid == *vid)
                {
                    rc = vcpool->AddVmInAR(vc_it->_new_vid, ar_id);
                    if (rc < 0)
                    {            
                        OutPut(SYS_DEBUG,"Add vm to affreg fail");
                        return TECS_ERR_DB_INSERT_FAILED;
                    }
                    break;
                }
            }
        }    
    }
    return SUCCESS;
}
/******************************************************************************/
STATUS ProjectManager::GetProjectVmsAndRelate(const MessageSaveAsImageReq &req, Project& project, vector<VmCfg>& vms, vector<VmRepel>& repels,vector<VmAffReg>& affregs)
{
    //查询工程下所有的虚拟机
    STATUS ret = ERROR;
    if (req._project_name.empty())
    {
        return ret;
    }
    
    ret = _ppool->GetProject(req._uid, req._project_name, project);
    if (SUCCESS != ret)
    {
        return ret;
    }
    
    vector<int64> vids;
    string where = " uid = " + to_string<int64>(req._uid,dec) + " And project_name = '" + req._project_name + "'";
    if (req._is_deployed_only)
    {
        where += " AND deployed_cluster IS NOT NULL";
    }
    ret = _ppool->GetVidsByWhere(where,vids);
    if (SUCCESS != ret)
    {
        return ret;
    }
    if (0 == vids.size())
    {
        return SUCCESS;
    }
    VmCfg vm;
    vector<int64>::iterator it;
    vector<VmBaseImage>::iterator it_image;
    for (it = vids.begin(); it != vids.end(); it++)
    {
        ret = _vpool->GetVmById(vm,*it);
        if (SUCCESS != ret)
        {
            return ret;
        }
        if (!req._is_image_save)
        {
            for (it_image = vm._images.begin(); it_image != vm._images.end(); it_image++)
            {
                it_image->_image_id = INVALID_IID;
            }
        }
        vms.push_back(vm);
        vm._images.clear();
        vm._disks.clear();
        vm._nics.clear();
        vm._devices.clear();
    }

    ret = _vpool->GetVmRepels(vids,repels);
    if (SUCCESS != ret)
    {
        return ret;
    }

    ret = _vpool->GetVmAffregs(vids,affregs);
    if (SUCCESS != ret)
    {
        return ret;
    }

    return SUCCESS;
    
}
STATUS ProjectManager::GetPjOpWFName(int op, string &action_name,VmOperation &vm_operate)
{
    switch(op)
    {
        case PROJECT_REBOOT:            
            action_name = PROJECT_ACT_REBOOT;
            vm_operate = REBOOT;
            break;
        case PROJECT_RESET:            
            action_name = PROJECT_ACT_RESET;
            vm_operate = RESET;
            break;
        default:
            return ERROR;
    }
    return OK;    
}
STATUS ProjectManager::GetPjOpString(int op, string &action_str)
{
    switch(op)
    {
        case PROJECT_REBOOT:            
            action_str = "reboot";
            break;
        case PROJECT_RESET:            
            action_str = "reset";
            break;
        case PROJECT_DEPLOY:            
            action_str = "deploy";
            break;
        case PROJECT_CENCEL:            
            action_str = "cancel";
            break;
        default:
            action_str = "unknow op";
    }

    return OK;
}
/******************************************************************************/
STATUS ProjectManager::GetProjectVms(const ProjectOperationReq &req, vector<int64>& vids)
{
    //查询工程下所有的虚拟机
    STATUS ret = ERROR;        
  
    string where = "project_id = '" + to_string<int64>(req._proid,dec) + "'";

    ret = _ppool->GetVidsByWhere(where,vids);
    if (SUCCESS != ret)
    {
        return ret;
    }

    return SUCCESS;
    
}
/******************************************************************************/
STATUS ProjectManager::GetCluster(int64 vid, string &name)
{
    //查询工程下所有的虚拟机
    VmCfgPool     *vcpool = NULL;
    VmCfg cfg;

    vcpool = VmCfgPool::GetInstance();
  
    if(NULL == vcpool)
    {
        OutPut(SYS_DEBUG,"Get VmCfgPool instance failed.\n");
        return ERROR;
    }
    STATUS ret = vcpool->GetVmById(cfg, vid);
    if (SUCCESS!= ret)
    {
        OutPut(SYS_DEBUG,"VM %d not exist.\n",vid);
        return ERR_OBJECT_NOT_EXIST;
    }
    name =cfg.get_cluster_name();
    if (name.empty())
    {
        OutPut(SYS_DEBUG,"VM %d Has not deploy\n",vid);
        return  ERROR_NOT_READY;
    }
    return SUCCESS;
}
/******************************************************************************/
STATUS ProjectManager::CreateExportWorkflow(const MID &sender, const MessageSaveAsImageReq &req, string& workflow_id, ostringstream &oss)
{
    STATUS ret = ERROR;

    Action save_action(PROJECT_CATEGORY,SAVE_AS_IMAGE,EV_IMAGE_TPI_EXPORT_REQ,MID(_mu->name()),MID(PROC_IMAGE_MANAGER,MU_PROJECT_IMAGE_AGENT));

    Project project;
    vector<VmCfg> vms;
    vector<VmRepel> repels;
    vector<VmAffReg> affregs;
    ret = GetProjectVmsAndRelate(req,project,vms,repels,affregs);
    if (SUCCESS != ret)
    {
        oss<<"Get Project Vms And Repels Failed!" << endl ;
        return ret;
    }
    ExportProjectImageReq save_req(save_action.get_id(), req._request_id, req._uid, req._user_name, project, vms, repels,affregs, req._description);
    save_action.message_req = save_req.serialize();

    save_action.timeout_feedback = false;

    SaveProjectActionLabels a_labels;
    a_labels._request_id = req._request_id;
    a_labels._uid = req._uid;
    a_labels._description = req._description;
    save_action.labels = a_labels;


    Workflow w(PROJECT_CATEGORY,SAVE_AS_IMAGE);
    w.Add(&save_action);

    if(!req._user_name.empty())
    {
        w.originator = req._user_name;
    }

    w.upstream_action_id = req.action_id;
    w.upstream_sender = sender;


    SaveProjectLabels w_labels;
    w_labels._request_id = req._request_id;
    w_labels._uid = req._uid;
    w_labels._project_name = req._project_name;
    w.labels = w_labels;

    ret = CreateWorkflow(w);
    if(SUCCESS != ret)
    {
        oss<<"Create Save Project Workflow Fail with ret " <<ret<< endl ;
    }
    else
    {
        workflow_id = w.get_id();
    }

    return ret;

}
/******************************************************************************/
STATUS ProjectManager::CreateDeployWorkflow(const MID &sender, const ProjectOperationReq &req, string& workflow_id, ostringstream &oss)
{
    STATUS ret = ERROR;

    //获取工程下vm
    vector<int64> vids;
    ret = GetProjectVms(req,vids);    
    if (SUCCESS != ret)
    {
        oss<<"Get Project vids Failed!" << endl ;
        return ret;
    }

    if (0 == vids.size())
    {
        oss<<"Project ("<<req._project_name<<") no vm!" << endl; 
        return ERROR;        
    }

    Workflow deploy(PROJECT_CATEGORY,PROJECT_ACT_DEPLOY);    
    ProjectOperateLabels labels;
    labels._project_name =req._project_name;
    labels._pid =req._proid;
    labels._total = vids.size();
    labels._operate= req._operation; 
    deploy.labels = labels;
    workflow_id = deploy.get_id();

    //创建汇总action
    ProOperationReq pro_opreq;
    Action finish_action(PROJECT_CATEGORY,PROJECT_ACT_DEPLOY,EV_PROJECT_OPERATE_END,MID(_mu->name()),MID(PROC_TC,MU_PROJECT_MANAGER));
    pro_opreq._operation=DEPLOY;
    pro_opreq._pid=req._proid;
    pro_opreq._workflow_id=workflow_id;
    pro_opreq.action_id = finish_action.get_id();
    finish_action.message_req = pro_opreq.serialize();
    finish_action.success_feedback = false;
    finish_action.failure_feedback = true;

    //创建各vm部署工作流
    vector<Action> prevs;
    VmOperationReq deploy_req;  
    vector<int64>::iterator it;
    for (it = vids.begin(); it != vids.end(); it++)
    {       
        OutPut(SYS_DEBUG,"CreateDeployWorkflow vid %d!\n",*it);

        Action deploy_action(PROJECT_CATEGORY,PROJECT_ACT_DEPLOY,EV_VM_OP,MID(_mu->name()),MID(PROC_TC,MU_VMCFG_MANAGER));
        deploy_req._operation = DEPLOY;
        deploy_req._vid = *it;      
        deploy_req.action_id = deploy_action.get_id();
        deploy_action.message_req = deploy_req.serialize();
        deploy_action.success_feedback = false;
        deploy_action.failure_feedback = false;

        prevs.push_back(deploy_action);

    }   
    deploy.Add(prevs,finish_action);  

    //创建工作流
    ret = CreateWorkflow(deploy);
    if(SUCCESS != ret)
    {        
        oss<<"Create Deploy Project Workflow Fail with ret " <<ret<< endl ;
    }

    return ret;

}
/******************************************************************************/
STATUS ProjectManager::CreateCancelWorkflow(const MID &sender, const ProjectOperationReq &req, string& workflow_id, ostringstream &oss)
{
    STATUS ret = ERROR;

    //获取工程下vm
    vector<int64> vids;
    ret = GetProjectVms(req,vids);    
    if (SUCCESS != ret)
    {
        oss<<"Get Project vids Failed!" << endl ;
        return ret;
    }

    if (0 == vids.size())
    {
        oss<<"Project ("<<req._project_name<<") no vm!" << endl; 
        return ERROR;        
    }


    Workflow cancel(PROJECT_CATEGORY,PROJECT_ACT_CANCEL);
    ProjectOperateLabels labels;
    labels._project_name =req._project_name;
    labels._pid =req._proid;
    labels._total = vids.size();
    labels._operate= req._operation; 
    cancel.labels = labels;
    workflow_id = cancel.get_id();

    //创建汇总action
    ProOperationReq pro_opreq;
    Action finish_action(PROJECT_CATEGORY,PROJECT_ACT_CANCEL,EV_PROJECT_OPERATE_END,MID(_mu->name()),MID(PROC_TC,MU_PROJECT_MANAGER));
    pro_opreq._operation=CANCEL;
    pro_opreq._pid=req._proid;
    pro_opreq._workflow_id=workflow_id;
    pro_opreq.action_id = finish_action.get_id();
    finish_action.message_req = pro_opreq.serialize();
    finish_action.success_feedback = false;
    finish_action.failure_feedback = true;

    //创建各vm撤销工作流
    vector<Action> prevs;
    VmOperationReq cancel_req;    
    vector<int64>::iterator it;

    for (it = vids.begin(); it != vids.end(); it++)
    {       
        OutPut(SYS_DEBUG,"CreateCancelWorkflow vid %d!\n",*it);

        Action cancel_action(PROJECT_CATEGORY,PROJECT_ACT_CANCEL,EV_VM_OP,MID(_mu->name()),MID(PROC_TC,MU_VMCFG_MANAGER));
        cancel_req._operation = CANCEL;
        cancel_req._vid = *it;
        cancel_req.action_id = cancel_action.get_id();
        cancel_action.message_req = cancel_req.serialize();

        cancel_action.success_feedback = false;
        cancel_action.failure_feedback = false;

        prevs.push_back(cancel_action);
    }   

    cancel.Add(prevs,finish_action);  
    ret = CreateWorkflow(cancel);
    if(SUCCESS != ret)
    {
        oss<<"Create Cancel Project Workflow Fail with ret " <<ret<< endl ;
    }

    return ret;

}

/******************************************************************************/
STATUS ProjectManager::CreateOperateWorkflow(const MID &sender, const ProjectOperationReq &req, string& workflow_id, ostringstream &oss)
{
    STATUS ret = ERROR;

    //获取工程下vm
    vector<int64> vids;
    ret = GetProjectVms(req,vids);    
    if (SUCCESS != ret)
    {
        oss<<"Get Project vids Failed!" << endl ;
        return ret;
    }

    if (0 == vids.size())
    {
        oss<<"Project ("<<req._project_name<<") no vm!" << endl; 
        return ERROR;        
    }

    string action_name;
    VmOperation vm_operate;
    if(OK != GetPjOpWFName(req._operation, action_name,vm_operate))
    {
        oss<<"illeage Project operate "<<req._operation<<" !" << endl; 
        return ERROR; 
    }
    
    Workflow operate(PROJECT_CATEGORY,PROJECT_WF_OPERATE);
    ProjectOperateLabels labels;
    labels._project_name =req._project_name;
    labels._pid =req._proid;
    labels._total = vids.size();    
    labels._operate= req._operation;  
    workflow_id = operate.get_id();

    //创建汇总action
    ProOperationReq pro_opreq;
    Action finish_action(PROJECT_CATEGORY,action_name,EV_PROJECT_OPERATE_END,MID(_mu->name()),MID(PROC_TC,MU_PROJECT_MANAGER));
    pro_opreq._operation=vm_operate;
    pro_opreq._pid=req._proid;
    pro_opreq._workflow_id=workflow_id;
    pro_opreq.action_id = finish_action.get_id();
    finish_action.message_req = pro_opreq.serialize();
    finish_action.success_feedback = false;
    finish_action.failure_feedback = true;

    //创建各vm操作工作流
    vector<Action> prevs;
    VmOperationReq operate_req;    
    vector<int64>::iterator it;
    string   cluster_name;

    for (it = vids.begin(); it != vids.end(); it++)
    {       
        OutPut(SYS_DEBUG,"CreateOperateWorkflow vid %d!\n",*it);

        ret = GetCluster(*it, cluster_name);
        if(SUCCESS != ret)
        {
            OutPut(SYS_DEBUG,"CreateOperateWorkflow vid %d GetCluster fail!\n",*it);
            labels._total--;
            continue;
        }
        Action operate_action(PROJECT_CATEGORY,action_name,EV_VM_OP,MID(_mu->name()),MID(cluster_name,PROC_CC,MU_VM_MANAGER));
        operate_req._operation = vm_operate;
        operate_req._vid = *it;
        operate_req.action_id = operate_action.get_id();
        operate_action.message_req = operate_req.serialize();

        operate_action.success_feedback = false;
        operate_action.failure_feedback = false;

        prevs.push_back(operate_action);
    }   
    
    if(0 != labels._total)
    {   
        operate.labels = labels;
        operate.Add(prevs,finish_action);  
        ret = CreateWorkflow(operate);
        if(SUCCESS != ret)
        {
            oss<<"Create Project operate Workflow Fail with ret " <<ret<< endl ;
        }
    }
    else
    {//没有部署的虚拟机，不需要创建工作流，返回失败，提示没有部署的vm   
        oss<<"Project ("<<req._project_name<<") no deployed vm!" << endl ;  
        return ERROR;
    }
    return ret;

}
/******************************************************************************/
STATUS ProjectManager::CreateImportWorkflow(const MID &sender, const MessageCreateByImageReq &req, string& workflow_id, ostringstream &oss)
{
    STATUS ret = ERROR;

    Action import_action(PROJECT_CATEGORY,CREATE_BY_IMAGE,EV_IMAGE_TPI_IMPORT_REQ,MID(_mu->name()),MID(PROC_IMAGE_MANAGER,MU_PROJECT_IMAGE_AGENT));

    ImportProjectImageReq import_req(import_action.get_id(), req._request_id, req._uid, req._user_name, req._image_id);
    import_action.message_req = import_req.serialize();

    import_action.timeout_feedback = false;

    ImportProjectActionLabels a_labels;
    a_labels._request_id = req._request_id;
    a_labels._uid = req._uid;
    a_labels._image_id = req._image_id;
    import_action.labels = a_labels;


    Workflow w(PROJECT_CATEGORY,CREATE_BY_IMAGE);
    w.Add(&import_action);

    if(!req._user_name.empty())
    {
        w.originator = req._user_name;
    }

    w.upstream_action_id = req.action_id;
    w.upstream_sender = sender;


    ImportProjectLabels w_labels;
    w_labels._request_id = req._request_id;
    w_labels._uid = req._uid;
    w_labels._image_id = req._image_id;
    w_labels._project_name = req._project_name;
    w.labels = w_labels;

    ret = CreateWorkflow(w);
    if(SUCCESS != ret)
    {
        oss<<"Create Import Project Workflow Fail with ret " <<ret<< endl ;
    }
    else
    {
        workflow_id = w.get_id();
    }

    return ret;

}
/******************************************************************************/
STATUS ProjectManager::CheckWorkflow(const string& request_id,
                                           bool &exist,
                                           int &progress,
                                           string &workflow_id,
                                           ostringstream &oss)
{
    int rc = ERROR;
    ostringstream where;
    where << "label_string_1 = '" << request_id << "'";

    vector<string> workflows;
    rc = FindWorkflow(workflows,PROJECT_CATEGORY, SAVE_AS_IMAGE,where.str());
    if (SUCCESS != rc)
    {
        oss<<"Find workflow ("<< PROJECT_CATEGORY <<","<< SAVE_AS_IMAGE << ") failed! "<< endl ;
        return rc;
    }
    if (0 != workflows.size())
    {
        exist = true;
        workflow_id = workflows[0];
        progress = GetWorkflowProgress(workflow_id);
        oss<<"Workflow "<< workflow_id <<"("<< PROJECT_CATEGORY <<","<< SAVE_AS_IMAGE << ") exist,progress = " << progress <<endl;
    }

    return SUCCESS;
}
bool ProjectManager::FindProjectWorkFlow(vector<string>& workflows,
                                              const string& workflow_name,
                                              int64    pid,                                            
                                              bool     is_rollback)
{
    ostringstream where;
    where <<" label_int64_1 = " << pid ;

    STATUS ret = FindWorkflow(workflows,
                                    PROJECT_CATEGORY,
                                    workflow_name,
                                    where.str(),
                                    is_rollback);//(有回滚流)
    if((SUCCESS == ret)
        &&(0 != workflows.size()))
    {
        return true;
    }

    return false;

}
/******************************************************************************/
STATUS ProjectManager::CheckWorkflow(const string& request_id,
                                           int64 uid,
                                           const string& project_name,
                                           bool &exist,
                                           int &progress,
                                           string &workflow_id,
                                           ostringstream &oss)
{
    int rc = ERROR;
    ostringstream where;
    where << "((label_string_1 = '" << request_id << "')"
          << " OR (label_int64_1 = " << uid << " AND label_string_2 = '"
          << project_name << "'))";

    vector<string> workflows;
    rc = FindWorkflow(workflows,PROJECT_CATEGORY, CREATE_BY_IMAGE,where.str());
    if (SUCCESS != rc)
    {
        oss<<"Find workflow ("<< PROJECT_CATEGORY <<","<< CREATE_BY_IMAGE << ") failed! "<< endl ;
        return rc;
    }
    if (0 != workflows.size())
    {
        exist = true;
        workflow_id = workflows[0];
        progress = GetWorkflowProgress(workflow_id);
        oss<<"Workflow "<< workflow_id <<"("<< PROJECT_CATEGORY <<","<< CREATE_BY_IMAGE << ") exist,progress = " << progress <<endl;
    }

    return SUCCESS;
}
/******************************************************************************/
STATUS ProjectManager::DealProjectAllocateReq(const MessageProjectAllocateReq& req)
{
    
    STATUS rc = ERROR;
    string              err_str;
    ostringstream       err_oss;
    
    int64  project_id = INVALID_OID;

    MID sender = _mu->CurrentMessageFrame()->option.sender();
    MessageOption ack_option(sender, EV_PROJECT_OPERATE_ACK, 0, 0);
    MessageProjectOperateAck ack_msg;
  
    Project  prj;
    
      /* 授权 */
    if (Authorize(req._uid, NULL,  AuthRequest::CREATE) == false)
    {
        err_oss<<"Authorize Fail";
        rc = ERR_PROJECT_AUTH_FAIL;
        goto error_exit;
    }

    rc = _ppool->GetProject(req._uid, req._project_name, prj);
    if (SUCCESS==rc)
    {
        err_oss <<"Error: user ("<<req._uid \
                << ") already has the project of name (" <<req._project_name <<")";
        rc = ERR_PROJECT_EXIST;
        goto error_exit;
    }
    
    prj._uid  = req._uid;
    prj._name = req._project_name;
    prj._description = req._description;

    rc = _ppool->Allocate(prj);
    if (SUCCESS != rc)
    {
        rc = ERR_PROJECT_ALLOC_FAIL;
        err_oss<<"allocate fail!";
        goto error_exit;
    }

    project_id = prj._oid;
    err_oss<<"Create project success. The project id = "<<prj._oid;
    

error_exit:
    ack_msg.detail = err_oss.str();
    ack_msg.state = rc;
    ack_msg.project_id = project_id;

    if (SUCCESS == rc)
    {       
        OutPut(SYS_DEBUG,"Create Project(%d) Success,state:%d,detail:%s\n",
                         project_id,
                         rc,
                         ack_msg.detail.c_str());
    }
    else
    {
        OutPut(SYS_ERROR,"Create Project(%s) Failed,state:%d,detail:%s\n",
                         req._project_name.c_str(),
                         ack_msg.state,
                         ack_msg.detail.c_str());
    }
    rc = _mu->Send(ack_msg, ack_option);
    return rc;
}
/******************************************************************************/
STATUS ProjectManager::DealProjectSaveAsImageReq(const MessageSaveAsImageReq& req)
{
    STATUS rc = ERROR;
    ostringstream oss;

    MID sender = _mu->CurrentMessageFrame()->option.sender();
    MessageOption ack_option(sender, EV_PROJECT_SAVE_AS_IMAGE_ACK, 0, 0);
    MessageSaveAsImageAck ack_msg(req.action_id, req._request_id);

    bool exist = false;
    int progress = 0;
    string workflow_id;

    rc = CheckWorkflow(req._request_id,exist,progress,workflow_id,oss);
    if (SUCCESS != rc)
    {//查询
        ack_msg.state = rc;
    }
    else
    {

        if (exist)
        {//有正向
            ack_msg.state = ERROR_ACTION_RUNNING;
            ack_msg.progress = progress;
            ack_msg._workflow_id = workflow_id;
            oss<<"Project Exporting Exist"<<endl ;
        }
        else
        {//无正向
            rc = CreateExportWorkflow(sender, req, ack_msg._workflow_id, oss);
            if (SUCCESS != rc)
            {
                ack_msg.state = rc;
                oss<<"Create Export Project Workflow Failed,ret:"<<rc<<endl ;
            }
            else
            {
                ack_msg.state = ERROR_ACTION_RUNNING;
                ack_msg.progress = 0;
                oss<<"Ready To Export Project"<<endl ;
            }
        }
    }

    ack_msg.detail = oss.str();
    if (SUCCESS == rc)
    {
        OutPut(SYS_DEBUG,"Export Project(%s) Success,workflow_id:%s,state:%d,detail:%s\n",
                         req._request_id.c_str(),
                         ack_msg._workflow_id.c_str(),
                         ack_msg.state,
                         ack_msg.detail.c_str());
    }
    else
    {
        OutPut(SYS_ERROR,"Export Project(%s) Failed,workflow_id:%s,state:%d,detail:%s\n",
                         req._request_id.c_str(),
                         ack_msg._workflow_id.c_str(),
                         ack_msg.state,
                         ack_msg.detail.c_str());
    }
    rc = _mu->Send(ack_msg, ack_option);
    return rc;
}
/******************************************************************************/
STATUS ProjectManager::DealProjectDeployReq(const ProjectOperationReq& req)
{
    STATUS ret = ERROR;
    ostringstream oss;

    MID sender = _mu->CurrentMessageFrame()->option.sender();
    MessageOption ack_option(sender, EV_PROJECT_DEPLOY_ACK, 0, 0);
    ProjectOperationAck ack;

    vector<string> deploy_workflows;
    vector<string> cancel_workflows;  

    bool is_deploying = FindProjectWorkFlow(deploy_workflows,
                                             PROJECT_ACT_DEPLOY,
                                             req._proid,
                                             false);//(有部署流)
    bool is_canceling = FindProjectWorkFlow(cancel_workflows,
                                             PROJECT_ACT_CANCEL,
                                             req._proid,
                                             false);//(有撤销流)  
    if(is_deploying)  
    {
        // 正在部署中
        oss<<"*** project "<<req._proid<<" is deploying now.***"<<endl ;
        ack.state    = ERROR_ACTION_RUNNING;
        ack.progress = GetWorkflowProgress(*deploy_workflows.begin());
        ack._workflow_id = *deploy_workflows.begin();
    }
    else if(is_canceling)  //(有撤销流)
    {
         oss<<"*** project "<<req._proid<<" is canceling now.***"<<endl ;
         ret = ERROR;
         ack.state    = ret;
    }
    else //(无表)
    {
        //创建部署工作流
        oss<<"project "<<req._proid<<" create deploy workflow "<<endl ;
        ret = CreateDeployWorkflow(sender, req, ack._workflow_id, oss);
        if(SUCCESS == ret)
        {
            ack.progress = 0;
            ack.state = ERROR_ACTION_RUNNING;
        }
        else
        {
            ack.state = ret;
        }
    }
   
    GetWorkflowEngine(ack._workflow_id,ack._workflow_engine);
    ack.detail = oss.str();
    if (SUCCESS == ret)
    {
        OutPut(SYS_DEBUG,"Deploy Project(%s) Success,workflow_id:%s,state:%d,detail:%s\n",
                         req._project_name.c_str(),
                         ack._workflow_id.c_str(),
                         ack.state,
                         ack.detail.c_str());
    }
    else
    {
        OutPut(SYS_ERROR,"Deploy Project(%s) Failed,workflow_id:%s,state:%d,detail:%s\n",
                         req._project_name.c_str(),
                         ack._workflow_id.c_str(),
                         ack.state,
                         ack.detail.c_str());
    }
    ret = _mu->Send(ack, ack_option);
    return ret;
}
/******************************************************************************/
STATUS ProjectManager::DealProjectCancelReq(const ProjectOperationReq& req)
{
    STATUS ret = ERROR;
    ostringstream oss;

    MID sender = _mu->CurrentMessageFrame()->option.sender();
    MessageOption ack_option(sender, EV_PROJECT_CANCEL_ACK, 0, 0);
    ProjectOperationAck ack;

    vector<string> cancel_workflows;
    vector<string> deploy_workflows;
    vector<string> operate_workflows;

    bool is_deploying = FindProjectWorkFlow(deploy_workflows,
                                             PROJECT_ACT_DEPLOY,
                                             req._proid,
                                             false);//(有部署流)
    bool is_canceling = FindProjectWorkFlow(cancel_workflows,
                                             PROJECT_ACT_CANCEL,
                                             req._proid,
                                             false);//(有撤销流)
    bool is_operating = FindProjectWorkFlow(operate_workflows,
                                             PROJECT_WF_OPERATE,
                                             req._proid,
                                             false);//(有操作流)
    if(is_canceling)  
    {
        // 正在撤销中
        oss<<"*** project "<<req._proid<<" is canceling now.***"<<endl ;
        ack.state    = ERROR_ACTION_RUNNING;
        ack.progress = GetWorkflowProgress(*cancel_workflows.begin());
    }
    else //(无表)
    {

        if(is_deploying)  //(有部署流)
        {
            // 正在部署中，删除部署工作流
            DeleteWorkflow(*deploy_workflows.begin());            
        }
        if(is_operating)  //(有操作流)
        {
            // 正在操作中，删除操作工作流
            DeleteWorkflow(*operate_workflows.begin());            
        }

        //创建撤销工作流
        oss<<"project "<<req._proid<<" create cancel workflow"<<endl ;
        ret = CreateCancelWorkflow(sender, req, ack._workflow_id, oss);
        if(SUCCESS == ret)
        {
            ack.progress = 0;
            ack.state = ERROR_ACTION_RUNNING;
        }
        else
        {
            ack.state = ret;
        }
    }   
    GetWorkflowEngine(ack._workflow_id,ack._workflow_engine);
    ack.detail = oss.str();
    if (SUCCESS == ret)
    {
        OutPut(SYS_DEBUG,"Cancel Project(%s) Success,workflow_id:%s,state:%d,detail:%s\n",
                         req._project_name.c_str(),
                         ack._workflow_id.c_str(),
                         ack.state,
                         ack.detail.c_str());
    }
    else
    {
        OutPut(SYS_ERROR,"Cancel Project(%s) Failed,workflow_id:%s,state:%d,detail:%s\n",
                         req._project_name.c_str(),
                         ack._workflow_id.c_str(),
                         ack.state,
                         ack.detail.c_str());
    }
    ret = _mu->Send(ack, ack_option);
    return ret;
}
/******************************************************************************/
STATUS ProjectManager::DealProjectOperateReq(const ProjectOperationReq& req)
{
    STATUS ret = ERROR;
    ostringstream oss;

    MID sender = _mu->CurrentMessageFrame()->option.sender();
    MessageOption ack_option(sender, EV_PROJECT_OPERATE_ACK, 0, 0);
    ProjectOperationAck ack;

    vector<string> cancel_workflows;
    vector<string> deploy_workflows;
    vector<string> operate_workflows;

    bool is_deploying = FindProjectWorkFlow(deploy_workflows,
                                             PROJECT_ACT_DEPLOY,
                                             req._proid,
                                             false);//(有部署流)
    bool is_canceling = FindProjectWorkFlow(cancel_workflows,
                                             PROJECT_ACT_CANCEL,
                                             req._proid,
                                             false);//(有撤销流)
    bool is_operating = FindProjectWorkFlow(operate_workflows,
                                             PROJECT_WF_OPERATE,
                                             req._proid,
                                             false);//(有操作流)
    if(is_operating)  
    {
        ProjectOperateLabels labels;
        GetWorkflowLabels(*operate_workflows.begin(),labels);

        if(labels._operate == req._operation)
        {
            // 正在进行相同操作中
            oss<<"*** A same project("<<req._proid<<") operate is executing now.***"<<endl ;
            ack.state    = ERROR_ACTION_RUNNING;
            ack._workflow_id = *operate_workflows.begin();
            ack.progress = GetWorkflowProgress(*operate_workflows.begin());
        }
        else
        {
            // 正在进行其他操作中
            string cur_op;           
            GetPjOpString(labels._operate, cur_op);
            oss<<"*** Another project("<<req._proid<<") operate("<<cur_op<<") is executing.***"<<endl ;
            ret = ERROR;
            ack.state    = ret;
        }
    }
    else if(is_canceling)  
    {
        // 正在撤销中
        oss<<"*** project "<<req._proid<<" is canceling now.***"<<endl ;
        ret = ERROR;
        ack.state    = ret;
    }
    else if(is_deploying)  
    {
        // 正在部署中
        oss<<"*** project "<<req._proid<<" is deploying now.***"<<endl ;
        ret = ERROR;
        ack.state    = ret;
    }
    else //(无表)
    {
        //创建操作工作流
        ret = CreateOperateWorkflow(sender, req, ack._workflow_id, oss);
        if(SUCCESS == ret)
        {
            ack.progress = 0;
            ack.state = ERROR_ACTION_RUNNING;
        }
        else
        {
            ack.state = ret;
        }
    }    
    GetWorkflowEngine(ack._workflow_id,ack._workflow_engine);
    ack.detail = oss.str();
    if (SUCCESS == ret)
    {
        OutPut(SYS_DEBUG,"Operate %d Project(%s) Success,workflow_id:%s,state:%d,detail:%s\n",
                         req._operation,
                         req._project_name.c_str(),
                         ack._workflow_id.c_str(),
                         ack.state,
                         ack.detail.c_str());
    }
    else
    {
        OutPut(SYS_ERROR,"Operate %d Project(%s) Failed,workflow_id:%s,state:%d,detail:%s\n",
                         req._operation,
                         req._project_name.c_str(),
                         ack._workflow_id.c_str(),
                         ack.state,
                         ack.detail.c_str());
    }
    ret = _mu->Send(ack, ack_option);
    return ret;
}
/******************************************************************************/
STATUS ProjectManager::DealProjectCopy(Project& project,
                                   vector <VmCfg>& vms,
                                   vector <VmRepel>&  repels,
                                   vector <VmAffReg>& affregs,
                                   ostringstream &err_oss)
{
    STATUS rc = ERROR;
    VidChange vc;
    vector<VmCfg>::iterator vm_it;
    vector<VidChange> vcs;

    rc = Transaction::Begin();
    if(SUCCESS != rc)
    {
        SkyAssert(false);
        return rc;
    }
    //创建工程
    rc = _ppool->Allocate(project);
    if(SUCCESS != rc)
    {
        SkyAssert(false);
        Transaction::Cancel();
        err_oss<<"Allocate new project fail!" << endl ;
        return rc;
    }

    //创建工程下的虚拟机    
    if (0 != vms.size())
    {
        for (vm_it = vms.begin(); vm_it != vms.end(); vm_it++)
        {
            vc._old_vid = vm_it->get_vid();
            vm_it->set_project_id(project.get_oid());
            vm_it->set_config_version(0);
            vm_it->clear_appointed_cluster();
            vm_it->clear_appointed_host();
            vm_it->clear_deployed_cluster();
            rc = _vpool->Allocate(*vm_it);
            if(SUCCESS != rc)
            {
                SkyAssert(false);
                Transaction::Cancel();
                err_oss<<"Allocate the vm fail!" << endl ;
                return rc;
            }
            vc._new_vid = vm_it->get_vid();
            
            vcs.push_back(vc);
        }
    }
    //插入虚拟机互斥表
    if (0 != repels.size())
    {
        ChangeVmRepels(vcs,repels);
        rc = _vpool->SetVmRepels(repels);
        if(SUCCESS != rc)
        {
            SkyAssert(false);
            Transaction::Cancel();
            err_oss<<"Set vm Repels fail!" << endl ;
            return rc;
        }
    }

    //插入虚拟机亲和域
    if (0 != affregs.size())
    {
        rc = ChangeVmAffregs(vcs,affregs);  
        if(SUCCESS != rc)
        {
            SkyAssert(false);
            Transaction::Cancel();
            err_oss<<"Set vm Affregs fail!" << endl ;
            return rc;
        }
    }

    rc = Transaction::Commit();
    if(SUCCESS != rc)
    {
        SkyAssert(false);
        Transaction::Cancel();
        err_oss<<"Database commit fail!" << endl ;
        return rc;
    }    

    return rc;
}
/******************************************************************************/
STATUS ProjectManager::DealProjectCloneReq(const ProjectCloneReq& req)
{
    STATUS rc = ERROR;
    ostringstream err_oss;
    
    MID sender = _mu->CurrentMessageFrame()->option.sender();
    MessageOption ack_option(sender, EV_PROJECT_CLONE_ACK, 0, 0);
    ProjectCloneAck ack;

    MessageSaveAsImageReq get_req;
    get_req._project_name = req.old_name;
    get_req._is_deployed_only = false;
    get_req._uid = req._uid;
    get_req._is_image_save = true;

    vector < VmCfg >  vms;
    vector < VmRepel >  repels;
    vector < VmAffReg >  affregs;

    Project prj(req._uid,req.new_name,req.des);
    Project project;

    //工程检测
    if (INVALID_OID == _ppool->GetProjectIdByName(req._uid, req.old_name))
    {
        err_oss<<"Project ( "<<req.old_name<<" ) not exist";
        rc =  ERR_PROJECT_NOT_EXIST;
        goto error_exit;
    }

    if (INVALID_OID != _ppool->GetProjectIdByName(req._uid, req.new_name))
    {
        rc = ERR_PROJECT_EXIST;
        err_oss <<"Error: user ("<<req._uid \
                << ") aready has the project of name (" <<req.new_name <<")";
        goto error_exit;
    }

    //获取工程信息
    rc = GetProjectVmsAndRelate(get_req, project, vms, repels, affregs);
    if (SUCCESS != rc)
    {
        err_oss<<"Get Project Vms And Repels Failed!" << endl ;
        goto error_exit;
    }

    //生成新的工程
    rc = DealProjectCopy(prj, vms,repels, affregs, err_oss);
    if (SUCCESS != rc)
    {          
        goto error_exit;
    } 

error_exit:    
    ack._state = rc;
    ack._pid = prj.get_oid();
    if (SUCCESS == rc)
    {        
        OutPut(SYS_DEBUG,"Clone Project(%s) Success,projectid:%d,state:%d\n",
                         req.old_name.c_str(),
                         ack._pid,
                         ack._state);
    }
    else
    {
        ack._detail = err_oss.str();
        OutPut(SYS_DEBUG,"Clone Project(%s) Fail,state:%d,detail:%s\n",
                         req.old_name.c_str(),                   
                         ack._state,
                         ack._detail.c_str());
    }
    _mu->Send(ack, ack_option);

    return rc;
}
/******************************************************************************/
STATUS ProjectManager::DealProjectCreateByImageReq(const MessageCreateByImageReq& req)
{
    STATUS rc = ERROR;
    ostringstream oss;

    MID sender = _mu->CurrentMessageFrame()->option.sender();
    MessageOption ack_option(sender, EV_PROJECT_CREATE_BY_IMAGE_ACK, 0, 0);
    MessageCreateByImageAck ack_msg(req.action_id, req._request_id);

    bool exist = false;
    int progress = 0;
    string workflow_id;

    //先查一下工程名是否合法
    if (req._project_name.empty() 
        || (INVALID_OID != _ppool->GetProjectIdByName(req._uid,req._project_name)))
    {
        oss<<"Invalid Project Name"<<endl;
        
    }
    else
    {
        rc = CheckWorkflow(req._request_id,req._uid,req._project_name,exist,progress,workflow_id,oss);
        if (SUCCESS != rc)
        {//查询
            ack_msg.state = rc;
        }
        else
        {
            if (exist)
            {//有正向
                ack_msg.state = ERROR_ACTION_RUNNING;
                ack_msg.progress = progress;
                ack_msg._workflow_id = workflow_id;
                oss<<"Project Importing Exist"<<endl ;
            }
            else
            {//无正向
                rc = CreateImportWorkflow(sender, req, ack_msg._workflow_id, oss);
                if (SUCCESS != rc)
                {
                    ack_msg.state = rc;
                    oss<<"Create Import Project Workflow Failed,ret:"<<rc<<endl ;
                }
                else
                {
                    ack_msg.state = ERROR_ACTION_RUNNING;
                    ack_msg.progress = 0;
                    oss<<"Ready To Import Project"<<endl ;
                }
            }
        }
    }

    ack_msg.detail = oss.str();
    if (SUCCESS == rc)
    {
        OutPut(SYS_DEBUG,"Import Project(%s) Success,workflow_id:%s,state:%d,detail:%s\n",
                         req._request_id.c_str(),
                         ack_msg._workflow_id.c_str(),
                         ack_msg.state,
                         ack_msg.detail.c_str());
    }
    else
    {
        OutPut(SYS_ERROR,"Import Project(%s) Failed,workflow_id:%s,state:%d,detail:%s\n",
                         req._request_id.c_str(),
                         ack_msg._workflow_id.c_str(),
                         ack_msg.state,
                         ack_msg.detail.c_str());
    }
    rc = _mu->Send(ack_msg, ack_option);
    return rc;
}
/******************************************************************************/
STATUS ProjectManager::DealExportProjectImageAck(const ExportProjectImageAck& ack)
{
    if(true == IsActionFinished(ack.action_id))
    {
        return SUCCESS;
    }

    STATUS rc = ERROR;
    Action action;
    rc = GetActionById(ack.action_id,action);
    if(SUCCESS != rc)
    {
        return rc;
    }

    OutPut(SYS_DEBUG,"ProjectManager receives action %s(%s) feedback!\n",
        ack.action_id.c_str(),
        action.name.c_str());

    SaveProjectLabels labels;
    string upstream_action_id;
    MID sender;

    switch(ack.state)
    {
        case SUCCESS:
        {
            rc = GetWorkflowLabels(ack.action_id,labels);
            if(SUCCESS != rc)
            {
                SkyAssert(false);
                break;
            }

            rc = FinishAction(ack.action_id);
            if(SUCCESS != rc)
            {
                SkyAssert(false);
                break;
            }
            OutPut(SYS_DEBUG,"ProjectManager receives save ack,request_id:%s,image_id:%lld!\n",
                ack._request_id.c_str(),
                ack._image_id);
            break;
        }
        case ERROR_ACTION_RUNNING:
        {
            OutPut(SYS_DEBUG,"action %s is already running, ok!\n",ack.action_id.c_str());
            rc = SUCCESS;
            break;
        }
        default:
        {
            OutPut(SYS_ERROR,"action %s failed!\n",ack.action_id.c_str());
            rc = FinishAction(ack.action_id);
            if(SUCCESS != rc)
            {
                SkyAssert(false);
            }
            break;
        }
    }

    return rc;
}
/******************************************************************************/
STATUS ProjectManager::DealImportProjectImageAck(ImportProjectImageAck& ack)
{
    if(true == IsActionFinished(ack.action_id))
    {
        return SUCCESS;
    }

    STATUS rc = ERROR;
    Action action;
    rc = GetActionById(ack.action_id,action);
    if(SUCCESS != rc)
    {
        return rc;
    }

    OutPut(SYS_DEBUG,"ProjectManager receives action %s(%s) feedback!\n",
        ack.action_id.c_str(),
        action.name.c_str());

    ImportProjectLabels labels;
    string upstream_action_id;
    MID sender;

    switch(ack.state)
    {
        case SUCCESS:
        {
            rc = GetWorkflowLabels(ack.action_id,labels);
            if(SUCCESS != rc)
            {
                SkyAssert(false);
                break;
            }
      
            Project prj(labels._uid,labels._project_name,ack._project._description);
            ostringstream err_oss;

            rc = DealProjectCopy(prj, ack._vms,ack._repels, ack._affregs, err_oss);
            if (SUCCESS != rc)
            {  
                OutPut(SYS_DEBUG,"DealProjectCopy fail detail: %s!\n",err_oss.str().c_str());
                break;
            }

            rc = FinishAction(ack.action_id);
            if(SUCCESS != rc)
            {
                SkyAssert(false);                
                break;
            }

            OutPut(SYS_DEBUG,"Deal Import Image Success,request_id:%s!\n",ack._request_id.c_str());
            break;
        }
        case ERROR_ACTION_RUNNING:
        {
            OutPut(SYS_DEBUG,"action %s is already running, ok!\n",ack.action_id.c_str());
            rc = SUCCESS;
            break;
        }
        default:
        {
            OutPut(SYS_ERROR,"action %s failed!\n",ack.action_id.c_str());
            rc = FinishAction(ack.action_id);
            if(SUCCESS != rc)
            {
                SkyAssert(false);
            }
            break;
        }
    }

    return rc;
}
/******************************************************************************/
void ProjectManager::DealProjectOpEndReq( const  ProOperationReq &req)
{    

    if(true == IsActionFinished(req.action_id))
    {
        OutPut(SYS_DEBUG, "DealProjectOpReq: rcv finished action:\n %s \n", \
              req.action_id.c_str());
        return ;
    }

    ProOperationAck ack;
    int state;    

    MID sender = _mu->CurrentMessageFrame()->option.sender();
    ack._operation = req._operation;
    ack._pid= req._pid;
    ack._workflow_id= req._workflow_id;
    ack.action_id = req.action_id;

    if(SUCCESS == GetActionState(req.action_id,state))
    {
        OutPut(SYS_DEBUG, "DealProjectOpReq state :%d\n", state);
        if(SUCCESS == state || ERROR == state)
        {
            //已经进行过处理，不再处理，直接返回
            return;
        }
    }    

    int count,succpre;
    //获取失败action个数
    count = GetUnsuccessedActionsCount(req.action_id)-1;

    OutPut(SYS_DEBUG, "DealProjectOpReq fail count:%d state :%d\n", count,state);
   
    if(0 == count)
    {//所有的action都成功了，直接返回成功
        ack.state = SUCCESS; 
        ack.progress = 100;    
    }
    else
    {//存在失败的action,设置该action失败，detail中带上成功比例
        ProjectOperateLabels labels;
        ostringstream detail;
        string action_str;
    
        //获取总action个数
        GetWorkflowLabels(ack.action_id, labels);  

        if(0 == labels._total)
        {
            SkyAssert(false);
            succpre = 100;            
        }
        else
        {
            succpre = 100 - ((count*100)/labels._total);
        }

        GetPjOpString(labels._operate, action_str);
        
        ack.state = ERROR; 
        detail<<"Project operate("<<action_str<<") fail, success "<<succpre<<"%";
        ack.detail =detail.str(); 
    }   

    MessageOption option(sender, EV_PROJECT_OPERATE_ACK, 0, 0);
    _mu->Send(ack, option);

    return;
}
/******************************************************************************/
void ProjectManager::DealProjectOpAck( const  ProOperationAck &ack)
{    
    DeleteWorkflow(ack.action_id);
    return;
}
/******************************************************************************/
bool ProjectManager::Authorize(int64 opt_uid, Project *buf, int oprate)
{   
    /* 获取授权 */
    AuthRequest ar(opt_uid);
    int64       oid = INVALID_OID;
    int64       self_uid = INVALID_OID;

    if (buf == NULL)
    {
        oid = 0;
        self_uid = 0;
    }
    else
    {
        oid      = buf->get_oid();
        self_uid = buf->get_uid();
        if (INVALID_UID == self_uid)
        {
           _ppool->Drop(*buf); 
           return false;
        }
    }    
    ar.AddAuthorize(
                    AuthRequest::PROJECT,           //授权对象类型,用user做例子
                    oid,                             //对象的ID,oid
                    (AuthRequest::Operation)oprate,  //操作类型
                    self_uid,                        //对象拥有者的用户ID
                    false                            //此对象是否为公有
                    );
    
    if (-1 == UserPool::GetInstance()->Authorize(ar))
    {   /* 授权失败 */
        
        return false;
    }

    return true;

}
/******************************************************************************/
void ProjectManager::MessageEntry(const MessageFrame &message)
{
    STATUS  ret = ERROR;

    switch (_mu->get_state())
    {
    case S_Startup:
    {
        switch (message.option.id())
        {
        case EV_POWER_ON:
        {
            OutPut(SYS_DEBUG,"ProjectManager power on!\n");
            ret = Transaction::Enable(GetDB());
            if (SUCCESS != ret)
            {
                OutPut(SYS_ERROR,"ProjectManager enable transaction fail! ret = %d",ret);
                SkyExit(-1, "ProjectManager::MessageEntry: call Transaction::Enable(GetDB()) failed.");
            }
            _mu->set_state(S_Work);
            OutPut(SYS_NOTICE, "%s power on!\n",_mu->name().c_str());
            break;
        }
        default:
            break;
        }
        break;
    }

    case S_Work:
    {
    switch(message.option.id())
    {          
        case EV_PROJECT_ALLOCATE_REQ:
        {            
            MessageProjectAllocateReq allocate_req;
            allocate_req.deserialize(message.data);    
            DealProjectAllocateReq(allocate_req);
            break;
        }      
        case EV_PROJECT_SAVE_AS_IMAGE_REQ:
        {
            MessageSaveAsImageReq save_req;
            save_req.deserialize(message.data);    
            DealProjectSaveAsImageReq(save_req);
            break;
        }

        case EV_PROJECT_CREATE_BY_IMAGE_REQ:
        {
            MessageCreateByImageReq import_req;
            import_req.deserialize(message.data);    
            DealProjectCreateByImageReq(import_req);
            break;
        }

        case EV_IMAGE_TPI_EXPORT_ACK:
        {
            ExportProjectImageAck save_ack;
            save_ack.deserialize(message.data);
            DealExportProjectImageAck(save_ack);
            break;
        }

        case EV_IMAGE_TPI_IMPORT_ACK:
        {
            ImportProjectImageAck import_ack;
            import_ack.deserialize(message.data);
            DealImportProjectImageAck(import_ack);
            break;
        }
        case EV_PROJECT_DEPLOY_REQ:
        {
            ProjectOperationReq deploy_req;
            deploy_req.deserialize(message.data);
            DealProjectDeployReq(deploy_req);
            break;
        }
        case EV_PROJECT_CANCEL_REQ:
        {
            ProjectOperationReq cancel_req;
            cancel_req.deserialize(message.data);
            DealProjectCancelReq(cancel_req);
            break;
        }
        case EV_PROJECT_OPERATE_REQ:
        {
            ProjectOperationReq operate_req;
            operate_req.deserialize(message.data);
            DealProjectOperateReq(operate_req);
            break;
        }
        case EV_PROJECT_CLONE_REQ:
        {
            ProjectCloneReq clone_req;
            clone_req.deserialize(message.data);
            DealProjectCloneReq(clone_req);
            break;
        }
        case EV_PROJECT_OPERATE_END:
        {
            //到最后一个总结action了
            ProOperationReq req;
            req.deserialize(message.data);
            DealProjectOpEndReq(req);
            break;
        }
        case EV_PROJECT_OPERATE_ACK:
        {    
            //删除工作流
            ProOperationAck ack;
            ack.deserialize(message.data);
            DealProjectOpAck(ack);
            break;
        }
        default:
            OutPut(SYS_DEBUG, "ProjectManager rcv a unexpected message %d !\n",\
                            message.option.id());
            break;
        }
        break;
    }
    default:
        break;
    }
}

}

