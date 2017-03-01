/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：project_manager.h
* 文件标识：
* 内容摘要：ProjectManager类的定义文件
* 当前版本：1.0
* 作    者：yanwei
* 完成日期：2013年7月3日
*
* 修改记录1：
*    修改日期：2013/7/3
*    版 本 号：V1.0
*    修 改 人：yanwei
*    修改内容：创建
*******************************************************************************/
#ifndef PROJECT_MANGER_H        
#define PROJECT_MANGER_H
#include "sky.h"
#include "mid.h"
#include "event.h"
#include "project_pool.h"
#include "vmcfg_pool.h"
#include "vm_messages.h"
#include "msg_project_manager_with_api_method.h"
#include "project_manager_with_image_manager.h"

namespace zte_tecs
{

class SaveProjectLabels: public WorkflowLabels
{
public:
    SaveProjectLabels():
        _request_id(label_string_1),
        _uid(label_int64_1),
        _project_name(label_string_2),
        _image_id(label_int64_2)


    {
    };
        
    string& _request_id;
    int64& _uid;
    string& _project_name;
    int64& _image_id;
};

class SaveProjectActionLabels: public ActionLabels
{
public:
    SaveProjectActionLabels():
        _request_id(label_string_1),
        _uid(label_int64_1),
        _description(label_string_2)
    {
    };
        
    string& _request_id;
    int64& _uid;
    string& _description;
};

class ProjectOperateLabels: public WorkflowLabels
{
public:       
    ProjectOperateLabels():
        _request_id(label_string_1),        
        _pid(label_int64_1),
        _total(label_int64_2),  
        _operate(label_int64_3),
        _project_name(label_string_2)
    {
    };
        
    string& _request_id;    
    int64& _pid;
    int64& _total;
    int64& _operate;
    string& _project_name;
    
};

class ImportProjectLabels: public WorkflowLabels
{
public:
    ImportProjectLabels():
        _request_id(label_string_1),
        _uid(label_int64_1),
        _image_id(label_int64_2),
        _project_name(label_string_2)
    {
    };
        
    string& _request_id;
    int64& _uid;
    int64& _image_id;
    string& _project_name;
};

class ImportProjectActionLabels: public ActionLabels
{
public:
    ImportProjectActionLabels():
        _request_id(label_string_1),
        _uid(label_int64_1),
        _image_id(label_int64_2)
    {
    };
        
    string& _request_id;
    int64& _uid;
    int64& _image_id;
};

class VidChange
{
public: 
    int64 _old_vid;
    int64 _new_vid;
};

/**
@brief 功能描述: 工程管理的实体类\n
@li @b 其它说明: 无
*/
class ProjectManager: public MessageHandler
{
public:
    static ProjectManager *GetInstance()
    {
        if(NULL == instance)
        {
            instance = new ProjectManager(); 
        }
        
        return instance;
    };
    
    ~ProjectManager( )
    {
        if (NULL != _ppool)
        {
            delete _ppool;
        }
        
        if (NULL != _vpool)
        {
            delete _vpool;
        }

        if (NULL != _mu)
        {
            delete _mu;
        } 
    };

    STATUS Init(SqlDB *db)
    {
        // 创建 ProjectPool对象
        _ppool = ProjectPool::CreateInstance(db);
        if(!_ppool)
        {
            return ERROR_NOT_READY;
        }
        
        if(SUCCESS != _ppool->Init(false))
        {
            return ERROR;
        }

        _vpool = VmCfgPool::GetInstance();
        if(!_vpool)
        {
            return ERROR_NOT_READY;
        }

        //启动消息单元工作线程
        return StartMu(MU_PROJECT_MANAGER);
    }

    void ChangeVmRepels(const vector<VidChange>& vcs, vector<VmRepel>& repels);
    STATUS ChangeVmAffregs(const vector<VidChange>& vcs, vector<VmAffReg>& affregs);
    STATUS GetProjectVmsAndRelate(const MessageSaveAsImageReq &req, Project& project, vector<VmCfg>& vms, vector<VmRepel>& repels,vector<VmAffReg>& affregs);
    STATUS CreateExportWorkflow(const MID &sender, const MessageSaveAsImageReq &req, string& workflow_id, ostringstream &oss);
    STATUS CreateImportWorkflow(const MID &sender, const MessageCreateByImageReq &req, string& workflow_id, ostringstream &oss);
    STATUS DealProjectAllocateReq(const MessageProjectAllocateReq& req);
    STATUS CreateDeployWorkflow(const MID &sender, const ProjectOperationReq &req, string& workflow_id, ostringstream &oss);
    STATUS CreateCancelWorkflow(const MID &sender, const ProjectOperationReq &req, string& workflow_id, ostringstream &oss);
    STATUS CreateOperateWorkflow(const MID &sender, const ProjectOperationReq &req, string& workflow_id, ostringstream &oss);
    STATUS GetPjOpWFName(int op, string &action_name,VmOperation &vm_operate);
    STATUS GetPjOpString(int op, string &action_str);

    STATUS CheckWorkflow(const string& request_id,
                               bool &exist,
                               int &progress,
                               string &workflow_id,
                               ostringstream &oss);
    STATUS CheckWorkflow(const string& request_id,
                               int64 uid,
                               const string& project_name,
                               bool &exist,
                               int &progress,
                               string &workflow_id,
                               ostringstream &oss);
    bool FindProjectWorkFlow(vector<string>& workflows,
                              const string& workflow_name,
                              int64    pid,
                              bool     is_rollback);
    STATUS DealProjectSaveAsImageReq(const MessageSaveAsImageReq& req);
    STATUS DealProjectCreateByImageReq(const MessageCreateByImageReq& req);
    STATUS DealProjectDeployReq(const ProjectOperationReq& req);
    STATUS DealProjectCancelReq(const ProjectOperationReq& req);
    STATUS DealProjectOperateReq(const ProjectOperationReq& req);
    STATUS DealProjectCloneReq(const ProjectCloneReq& req);
    STATUS DealExportProjectImageAck(const ExportProjectImageAck& ack);
    STATUS DealImportProjectImageAck(ImportProjectImageAck& ack);
    STATUS GetProjectVms(const ProjectOperationReq &req, vector<int64> &vids);
    STATUS DealProjectCopy(Project& project,
                           vector <VmCfg>& vms,
                           vector <VmRepel>&  repels,
                           vector <VmAffReg>& affregs,
                           ostringstream &err_oss);
    STATUS GetCluster(int64 vid, string &name);
    void   DealProjectOpEndReq( const  ProOperationReq &req);
    void   DealProjectOpAck( const  ProOperationAck &req);
    bool Authorize(int64 opt_uid, Project *buf, int oprate);
private:
    ProjectManager()
    {
        _mu = NULL;
    }

    STATUS StartMu(const string& name);
    virtual void MessageEntry(const MessageFrame &message); 

    MessageUnit *_mu;
    ProjectPool *_ppool;
    VmCfgPool *_vpool;
    static ProjectManager *instance;

    DISALLOW_COPY_AND_ASSIGN(ProjectManager); 

};

}
#endif /* PROJECT_MANGER_H */

