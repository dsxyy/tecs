/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：project_api_method.cpp
* 文件标识：
* 内容摘要：project 模块XML-RPC注册方法类的实现文件
* 当前版本：1.0
* 作    者：Bob
* 完成日期：2011年9月5日
*
* 修改记录1：
*    修改日期：2011/9/5
*    版 本 号：V1.0
*    修 改 人：Bob
*    修改内容：创建
*******************************************************************************/
#include "project_manager_methods.h"
#include "mid.h"
#include "event.h"
#include "sky.h"
#include "project_api.h"
#include "vmcfg_api.h"
#include "usergroup.h"
#include "log_agent.h"
#include "api_pub.h"
#include "authrequest.h"

static int pmm_print = 1;
DEFINE_DEBUG_VAR(pmm_print);
#define Debug(fmt,arg...) \
        do \
        { \
            if(pmm_print) \
            { \
                OutPut(SYS_DEBUG,"[%s:%d]"fmt,__func__,__LINE__, ##arg);\
            } \
        }while(0)

using namespace xmlrpc_c;

namespace zte_tecs
{
#define TEMP_MU_NAME_PRE            "project_manager_api"

#define XML_RPC_RETURN() \
    array_result = new value_array(array_data); \
    *retval = *array_result; \
    delete array_result; \
    return;

#define AUTHORIZE(ob, op) \
    AuthRequest ar(get_userid()); \
    ar.AddAuthorize((ob), 0, (op), 0, false); \
    if (-1 == UserPool::Authorize(ar)) \
    { \
        array_data.push_back(value_int(RpcError(ERR_PROJECT_AUTH_FAIL))); \
        array_data.push_back(value_string(CodeToStr(ERR_PROJECT_AUTH_FAIL)));\
        XML_RPC_RETURN(); \
    }
    
#define WAITE_MSG_ACK(msg_id) \
    MessageUnit temp_mu(TempUnitName(TEMP_MU_NAME_PRE)); \
    temp_mu.Register(); \
    MID receiver; \
    receiver._application = ApplicationName(); \
    receiver._process = PROC_TC; \
    receiver._unit = MU_PROJECT_MANAGER; \
    MessageOption option(receiver, (msg_id), 0, 0); \
    temp_mu.Send(req, option); \
    MessageFrame message; \
    if(SUCCESS == temp_mu.Wait(&message, 5000))

#define WAITE_CC_MSG_ACK(msg_id) \
    MessageUnit temp_mu(TempUnitName(TEMP_MU_NAME_PRE)); \
    temp_mu.Register(); \
    MID receiver; \
    receiver._application = cluster_name; \
    receiver._process = PROC_CC; \
    receiver._unit = MU_VM_LIFECYCLE_MANAGER; \
    MessageOption option(receiver, (msg_id), 0, 0); \
    temp_mu.Send(req, option); \
    MessageFrame message; \
    if(SUCCESS == temp_mu.Wait(&message, 5000))
	
#define ACK_PROJECT_TIMEOUT() \
    array_data.push_back(value_int(RpcError(ERR_RPOJECT_TIMEOUT))); \
    array_data.push_back(value_string(CodeToStr(ERR_RPOJECT_TIMEOUT)));

#define ACK_PARA_INVALID(para) \
    array_data.push_back(value_int(RpcError(ERR_RPOJECT_PARA_INVALID))); \
    array_data.push_back(value_string(CodeToStr(ERR_RPOJECT_PARA_INVALID, (para))));

static const string CodeToStr(int32 code, const string &info = "")
{
    switch (code)
    {      
        case ERR_RPOJECT_TIMEOUT:
            return "Failed, project manager is not answered.";   
        case ERR_RPOJECT_PARA_INVALID:
            return "Error, invalid para info, " + info + ".";
        case ERR_PROJECT_AUTH_FAIL:
            return "Failed, project manager authorize failed. ";   
        default:
            ostringstream oss;
            oss << "Undefined return code=" << code << ".";
            return oss.str();
    }
}

static STATUS ProjectInit(ProjectOperate *op_inst,
                              ostringstream  &err_oss)
{
    op_inst = ProjectOperate::GetInstance();
    if (NULL == op_inst)
    {
        err_oss<< "Get op_inst instance failed.";
        return ERROR;
    }
        
    STATUS ret = Transaction::Enable(GetDB());
    if (SUCCESS != ret)
    {
        err_oss << "Enable transaction failed.";
        return ERROR;
    }

    return SUCCESS;
}

#define PROJECT_INIT(op_inst, err_oss)\
        do \
        {\
            if (SUCCESS != ProjectInit(op_inst,err_oss )) \
            { \
                xRET2(xINT(RpcError(ERROR)),xSTR(err_oss.str())); \
                return ;\
            }\
        }while (0)\
        
/******************************************************************************/
void ProjectAllocateMethod::MethodEntry(const paramList& paramList,
                                               value* const retval)
{
    string              err_str;
    ostringstream       err_oss;
    ApiProjectAllocateData  api_data;

    /* 1. 获取参数 */
    api_data.name = value_string(paramList.getString(1));
    api_data.des  = value_string(paramList.getString(2));

    /*参数检验*/
    //api_data.from_rpc(paramList.getStruct(1));
    bool success = api_data.Validate(err_str);
    if (!success)
    {
        xRET2(xINT(RpcError(ERROR_INVALID_ARGUMENT)),xSTR(err_str));
        return ;
    }
    
    MessageProjectAllocateReq req;
    
    req._uid = get_userid();
    req._project_name = api_data.name;
    req._description  = api_data.des;
    
    /* 4. 消息转发并等应答 */
    WAITE_MSG_ACK(EV_PROJECT_ALLOCATE_REQ)
    {
        MessageProjectOperateAck ack;
        ack.deserialize(message.data);
        if (SUCCESS != ack.state)
        {
            xRET2(xINT(RpcError(ack.state)),xSTR(ack.detail));
        }
        else
        {
            xRET2(xINT(RpcError(ack.state)),xI8(ack.project_id));
        }
    }
    else
    {
        
        xRET2(xINT(RpcError(ERR_RPOJECT_TIMEOUT)),xSTR("create project timeout"));
    }
    return;
}

/******************************************************************************/
void ProjectCloneMethod::MethodEntry(const paramList& paramList,
                                               value* const retval)
{
    string              err_str;
    ostringstream       err_oss;

    ApiProjectCloneData  api_data;

    /* 1. 获取参数 */
    api_data.old_name = value_string(paramList.getString(1));
    api_data.new_name = value_string(paramList.getString(2));
    api_data.des  = value_string(paramList.getString(3));

    bool success = api_data.Validate(err_str);
    if (!success)
    {
        xRET2(xINT(RpcError(ERROR_INVALID_ARGUMENT)),xSTR(err_str));
        return ;
    }    
    
    int64  uid = get_userid();
    int64  project_id = INVALID_OID;

    /* 2. 授权 */   

    Project  project;
    ProjectPool *ppp = ProjectPool::GetInstance();
    STATUS   tRet = ppp->GetProject(uid, api_data.old_name,project); 
    if( SUCCESS != tRet)
    {
        err_oss<<"Error: user ("<<uid << 
                 ") does not has the project of name (" << api_data.old_name <<") !";
        xRET2(xINT(RpcError(ERR_PROJECT_NOT_EXIST)),xSTR(err_oss.str()));
        return;
    }

    ProjectOperate *op_inst = NULL;
    PROJECT_INIT(op_inst, err_oss);
    if(false == op_inst->Authorize(uid, &project, AuthRequest::MANAGE))
    {
        err_oss<<"Authorize Fail";
        xRET2(xINT(RpcError(ERR_PROJECT_AUTH_FAIL)),xSTR(err_oss.str()));
        return;
    }
    
    tRet = DealClone(api_data, uid, project_id, err_oss);
    if (SUCCESS != tRet)
    {
        xRET2(xINT(RpcError(tRet)),xSTR(err_oss.str()));
    }
    else
    {
        xRET2(xINT(RpcError(tRet)),xI8(project_id));
    }
    Debug("\n%s, tRet = %d, err_str = %s\n", _method_name.c_str(),
                                            tRet, err_oss.str().c_str());
    return;
}

STATUS ProjectCloneMethod::DealClone(const ApiProjectCloneData  &apidata, 
                                 int64          uid,
                                 int64         &project_id,
                                 ostringstream &err_oss)
{
    STATUS            ret;

    /* 2. 获取参数 */  

    ProjectCloneReq req;
    req.old_name= apidata.old_name;
    req.new_name= apidata.new_name;
    req.des= apidata.des;
    req._uid = get_userid();
    
    /* 4. 消息转发并等应答 */
    WAITE_MSG_ACK(EV_PROJECT_CLONE_REQ)
    {
        ProjectCloneAck ack;
        ack.deserialize(message.data);
        if ((SUCCESS == ack._state))
        {
            ret= SUCCESS;
            project_id = ack._pid;
        }
        else
        {
            ret= ack._state;
            err_oss << ack._detail;
        }
    }
    else
    {
        /*  超时 or 其他错误 ? */
        err_oss << "clone vm timeout";
        ret= ERR_RPOJECT_TIMEOUT;
    }

    /* 5. XML-RPC返回  */
    return ret;
} 
/******************************************************************************/
void ProjectDeleteMethod :: MethodEntry(const paramList& paramList,
                                        value* const retval)
{
    ostringstream    err_oss;
    int64            owner_uid = -1;
    ProjectOperate	*op_inst = NULL;

    /* 1. 获取参数 */
    int64   uid = get_userid();
    string  project_name   = paramList.getString(1);
    string  appointed_user = paramList.getString(2);

    /* 校验参数 */
    if (StringCheck::CheckNormalName(project_name,1, STR_LEN_32) != true)
    {
        err_oss << "Error, invalide parameter with  " << project_name;
        xRET2(xINT(ERROR_INVALID_ARGUMENT),xSTR(err_oss.str()));
        return;
    }

    /* 租户不填写用户名,当租户请求时，该查询id等于租户自己 */
    if (appointed_user.empty())
    {
        owner_uid = uid;
    }
    else
    {
        if (!_upool->GetUserIDByName(appointed_user, owner_uid))
        {
            err_oss << "user:" << appointed_user<<") does not exist!";
            xRET2(xINT(ERR_OBJECT_NOT_EXIST),xSTR(err_oss.str()));
            return;
        }
    }
    
    PROJECT_INIT(op_inst, err_oss);
    
    STATUS tRet = op_inst->DealDelete(uid, owner_uid, project_name,  err_oss);

    xRET2(xINT(RpcError(tRet)),xSTR(err_oss.str()));
    Debug("\n%s, tRet = %d, err_str = %s\n", _method_name.c_str(),
                                            tRet, err_oss.str().c_str());
    return;
}


/******************************************************************************/
void ProjectModifyMethod::MethodEntry(const paramList& paramList,
                                        value* const retval)
{
    ostringstream    err_oss;
    ProjectOperate	*op_inst = NULL;

    /* 1. 获取参数 */
    int64  uid       = get_userid();

    ApiProjectModifyData  api_data;
    api_data.name    = paramList.getString(1);
    api_data.new_des = paramList.getString(2);

    PROJECT_INIT(op_inst, err_oss);
    STATUS  tRet = op_inst->DealModify(api_data, uid, err_oss);

    xRET2(xINT(RpcError(tRet)),xSTR(err_oss.str()));
    Debug("\n%s, tRet = %d, err_str = %s\n", _method_name.c_str(),
                                            tRet, err_oss.str().c_str());
    return;

}

/******************************************************************************/
void ProjectQueryMethod :: MethodEntry(const paramList& paramList,
                                       value* const retval)
{
    ostringstream     err_oss;
    string            err_str;
    ProjectOperate	*op_inst = NULL;

    /* 1. 获取参数 */
    ApiProjectQuery  api_data;

    api_data.start_index       = paramList.getI8(1);
    api_data.count             = paramList.getI8(2);
    api_data.type              = paramList.getI8(3);
    api_data.appointed_project = paramList.getString(4);
    api_data.appointed_user    = paramList.getString(5);
    //api_data.from_rpc(paramList.getStruct(1));

    Debug("\n++++ %s:\n%s\n\n", _method_name.c_str(),api_data.toString().c_str());
    bool success = api_data.Validate(err_str);
    if (!success)
    {
        xRET2(xINT(RpcError(ERROR_INVALID_ARGUMENT)),xSTR(err_str));
        return ;
    }

    vector<ApiProjectInfo>  vec_prj;
    int64                   max_count     = 0;
    int64                   next_index    = -1;
    int64                   appointed_uid = -1;

    int64   uid = get_userid();
    if (PROJECT_USER_APPOINTED == static_cast<ApiProjectQueryType>(api_data.type))
    {
        if (!_upool->GetUserIDByName(api_data.appointed_user, appointed_uid))
        {
            err_oss << "user:" << api_data.appointed_user<<"does not exist!";

            xRET2(xINT(RpcError(ERR_OBJECT_NOT_EXIST)),
                  xSTR(err_oss.str()));
        }
    }
    PROJECT_INIT(op_inst, err_oss);
    STATUS tRet = op_inst->DealQuery(api_data,
                                      uid,
                                      appointed_uid,
                                      vec_prj,
                                      max_count,
                                      next_index,
                                      err_oss);

    if (SUCCESS != tRet)
    {
        xRET2(xINT(RpcError(tRet)),
              xSTR(err_oss.str()));
    }
    else
    {
        vector<value>                          array_project;
        vector<ApiProjectInfo>::const_iterator iter;

        for (iter = vec_prj.begin(); iter != vec_prj.end(); iter++)
        {
            array_project.push_back(iter->to_rpc());
            Debug("\n %s:\n%s\n\n", _method_name.c_str(),iter->toString().c_str());
        }

        xRET4(xINT(RpcError(SUCCESS)),
              xI8(next_index),
              xI8(max_count),
              xARRAY(array_project));
    }
    Debug("\n%s, tRet = %d, err_str = %s, max_count = %d, next_index = %d\n", 
               _method_name.c_str(),
               tRet, 
               err_oss.str().c_str(),
               max_count,
               next_index);
    return;
}
/******************************************************************************/
STATUS ProjectActionMethod::DealDeploy(const ApiProjectActionData  &apidata, 
                                 int64          uid,
                                 int64         &project_id,
                                 ostringstream &err_oss,
                                 string &workflow_engine)
{
    STATUS            ret;

    /* 1. 获取参数 */  

    ProjectPool *ppp = ProjectPool::GetInstance();
    project_id = ppp->GetProjectIdByName(uid, apidata.project_name); 
    if (INVALID_OID == project_id)
    {
        err_oss<<"Project ( "<<apidata.project_name<<" ) not exist";
        return ERR_PROJECT_NOT_EXIST;
    }

    ProjectOperationReq req;
    req._operation = PROJECT_DEPLOY;
    req._proid = project_id;
    req._project_name = apidata.project_name;
    
    /* 2. 消息转发并等应答 */
    WAITE_MSG_ACK(EV_PROJECT_DEPLOY_REQ)
    {
        ProjectOperationAck ack;
        ack.deserialize(message.data);
        if ((SUCCESS == ack.state) || (ERROR_ACTION_RUNNING == ack.state))
        {
            ret= SUCCESS;
            workflow_engine =  ack._workflow_engine;
            err_oss << ack._workflow_id;
        }
        else
        {
            ret= ack.state;
            err_oss << ack.detail;
        }
    }
    else
    {
        /*  超时 or 其他错误 ? */
        err_oss << "deploy vm timeout";
        ret= ERR_RPOJECT_TIMEOUT;
    }

    /* 5. XML-RPC返回  */
    return ret;

}

STATUS ProjectActionMethod::DealCancel(const ApiProjectActionData  &apidata, 
                                 int64          uid,
                                 int64         &project_id,
                                 ostringstream &err_oss,
                                 string &workflow_engine)
{
    STATUS            ret;

    /* 1. 获取参数 */  

    ProjectPool *ppp = ProjectPool::GetInstance();
    project_id = ppp->GetProjectIdByName(uid, apidata.project_name); 
    if (INVALID_OID == project_id)
    {
        err_oss<<"Project ( "<<apidata.project_name<<" ) not exist";
        return ERR_PROJECT_NOT_EXIST;
    }

    ProjectOperationReq req;
    req._operation = PROJECT_CENCEL;
    req._proid = project_id;
    req._project_name = apidata.project_name;

    OutPut(SYS_DEBUG, "DealCancel ProjectId %d reqid %s\n",project_id,req._project_name.c_str());
    
    /* 2. 消息转发并等应答 */
    WAITE_MSG_ACK(EV_PROJECT_CANCEL_REQ)
    {
        ProjectOperationAck ack;
        ack.deserialize(message.data);
        if ((SUCCESS == ack.state) || (ERROR_ACTION_RUNNING == ack.state))
        {
            ret= SUCCESS;
            workflow_engine =  ack._workflow_engine;
            err_oss << ack._workflow_id;
        }
        else
        {
            ret= ack.state;
            err_oss << ack.detail;
        }
    }
    else
    {
        /*  超时 or 其他错误 ? */
        err_oss << "cancel vm timeout";
        ret= ERR_RPOJECT_TIMEOUT;
    }

    /* 5. XML-RPC返回  */
    return ret;
} 
STATUS ProjectActionMethod::DealOperate(const ApiProjectActionData  &apidata, 
                                 int64          uid,
                                 int64         &project_id,
                                 ProjectOperation op,
                                 ostringstream &err_oss,
                                 string &workflow_engine)
{
    STATUS            ret;

    /* 1. 获取参数 */  

    ProjectPool *ppp = ProjectPool::GetInstance();
    project_id = ppp->GetProjectIdByName(uid, apidata.project_name); 
    if (INVALID_OID == project_id)
    {
        err_oss<<"Project ( "<<apidata.project_name<<" ) not exist";
        return ERR_PROJECT_NOT_EXIST;
    }

    ProjectOperationReq req;
    req._operation = op;
    req._proid = project_id;
    req._project_name = apidata.project_name;

    OutPut(SYS_DEBUG, "Operate %d ProjectId %d reqid %s\n",req._operation,project_id,req._project_name.c_str());
    
    /* 2. 消息转发并等应答 */
    WAITE_MSG_ACK(EV_PROJECT_OPERATE_REQ)
    {
        ProjectOperationAck ack;
        ack.deserialize(message.data);
        if ((SUCCESS == ack.state) || (ERROR_ACTION_RUNNING == ack.state))
        {
            ret= SUCCESS;
            workflow_engine = ack._workflow_engine;
            err_oss << ack._workflow_id;
        }
        else
        {
            ret= ack.state;
            err_oss << ack.detail;
        }
    }
    else
    {
        /*  超时 or 其他错误 ? */
        err_oss <<req._operation<< " project timeout";
        ret= ERR_RPOJECT_TIMEOUT;
    }

    /* 5. XML-RPC返回  */
    return ret;
} 

void ProjectActionMethod::MethodEntry(const paramList& paramList,
                                       value* const retval)
{
    vector<value>     array_data;
    value_array       *array_result = NULL;

    ostringstream     err_oss;
    string            err_str;
    string            action_name = paramList.getString(2);
    ProjectOperation  action_type;
    int64             project_id;
    string            target;

    /* 1. 获取参数 */

    int64   uid = get_userid();
    ApiProjectActionData  api_data;

    api_data.project_name      = paramList.getString(1);

    /* 2. 检查参数 */
    if (!StringCheck::CheckNormalName(api_data.project_name, 1, STR_LEN_32))
    {
        ACK_PARA_INVALID("project_name");
        XML_RPC_RETURN();
    }

    /* 3. 授权 */   
    Project  project;
    ProjectPool *ppp = ProjectPool::GetInstance();
    STATUS   tRet = ppp->GetProject(uid, api_data.project_name,project); 
    if( SUCCESS != tRet)
    {
        err_oss<<"Error: user ("<<uid << 
                 ") does not has the project of name (" << api_data.project_name <<") !";
        xRET2(xINT(RpcError(ERR_PROJECT_NOT_EXIST)),xSTR(err_oss.str()));
        return;
    }

    ProjectOperate *op_inst = NULL;
    PROJECT_INIT(op_inst, err_oss);
    if(false == op_inst->Authorize(uid, &project, AuthRequest::MANAGE))
    {
        err_oss<<"Authorize Fail";
        xRET2(xINT(RpcError(ERR_PROJECT_AUTH_FAIL)),xSTR(err_oss.str()));
        return;
    }

    Debug("\n++++ %s:\n%s\n\n", _method_name.c_str(),api_data.toString().c_str());   

    err_oss.str("");    
    tRet = op_inst->Str2ProjOp(action_name, action_type, err_oss);

     /* 4. 操作 */
    if (PROJECT_DEPLOY == action_type)
    {
        tRet = DealDeploy(api_data, uid , project_id,err_oss,target);
    }
    else if (PROJECT_CENCEL== action_type)
    {
        tRet = DealCancel(api_data, uid,project_id,err_oss,target);
    }
    else if (PROJECT_REBOOT== action_type || PROJECT_RESET == action_type)
    {
        tRet = DealOperate(api_data, uid,project_id,action_type,err_oss,target);
    }

    if(SUCCESS == tRet)
    {
        OutPut(SYS_DEBUG, "\n %s, tRet = %d, err_str= %s\n",
              _method_name.c_str(),tRet,
              err_oss.str().c_str());
        xRET3(xINT(RpcError(tRet)),xSTR(err_oss.str()),xSTR(target));
    }
    else
    {
        OutPut(SYS_DEBUG, "\n %s, tRet = %d, err_str= %s\n",
              _method_name.c_str(),
              tRet, err_oss.str().c_str());
        xRET2(xINT(RpcError(tRet)),xSTR(err_oss.str()));
    }

    return;
}

/******************************************************************************/
void ProjectShowVmMethod :: MethodEntry(const paramList& paramList,
                                        value* const retval)
{
    ostringstream     err_oss;
    string            err_str;
    ProjectOperate	*op_inst = NULL;

    /* 1. 获取参数 */
    ApiProjectVmQuery  api_data;

    api_data.start_index       = paramList.getI8(1);
    api_data.count             = paramList.getI8(2);
    api_data.appointed_project = paramList.getString(3);
    api_data.appointed_user    = paramList.getString(4);
    //api_data.from_rpc(paramList.getStruct(1));

    Debug("\n++++ %s:\n%s\n", _method_name.c_str(),api_data.toString().c_str());
    bool  success = api_data.Validate(err_str);

    if (!success)
    {
        xRET2(xINT(RpcError(ERROR_INVALID_ARGUMENT)),xSTR(err_str));
        return ;
    }

    int64   uid = get_userid();
    int64   appointed_uid = INVALID_OID;
    if (!api_data.appointed_user.empty())
    {
        if (!_upool->GetUserIDByName(api_data.appointed_user, appointed_uid))
        {
            err_oss << "user:" << api_data.appointed_user<<"does not exist!";

            xRET2(xINT(RpcError(ERR_OBJECT_NOT_EXIST)),
                  xSTR(err_oss.str()));
        }
    }

    vector<ApiProjectVmInfo>  vec_vm;
    int64                   max_count     = 0;
    int64                   next_index    = -1;

    PROJECT_INIT(op_inst, err_oss);
    STATUS tRet = op_inst->DealProjectVmQuery(api_data,
                                              uid,
                                              appointed_uid,
                                              vec_vm,
                                              max_count,
                                              next_index,
                                              err_oss);

    if (SUCCESS != tRet)
    {
        xRET2(xINT(RpcError(tRet)),
              xSTR(err_oss.str()));
    }
    else
    {
        vector<value>                            array_vm;
        vector<ApiProjectVmInfo>::const_iterator iter;

        for (iter = vec_vm.begin(); iter != vec_vm.end(); iter++)
        {
            array_vm.push_back(iter->to_rpc());
            Debug("\n %s:\n%s\n\n", _method_name.c_str(),iter->toString().c_str());
        }

        xRET4(xINT(RpcError(SUCCESS)),
              xI8(next_index),
              xI8(max_count),
              xARRAY(array_vm));
    }
    Debug("\n%s, tRet = %d, err_str = %s\n", _method_name.c_str(),
                                            tRet, err_oss.str().c_str());
    return;
}

/******************************************************************************/
void ProjectSaveAsImageMethod::MethodEntry(const paramList& paramList,
                                        value* const retval)
{
    vector<value>     array_data;
    value_array       *array_result = NULL;

#if 0//租户自己和管理员都可以操作
    /* 1. 操作授权 */
    AUTHORIZE(AuthRequest::PROJECT, AuthRequest::MANAGE);
#endif

    /* 2. 获取参数 */  
    int64   uid = get_userid();
    ApiProjectSaveAsImageData  api_data ;
    api_data.from_rpc(paramList.getStruct(1));
    
    /* 3. 检查参数 */
    if (!StringCheck::CheckNormalName(api_data.project_name, 1, STR_LEN_32))
    {
        ACK_PARA_INVALID("project_name");
        XML_RPC_RETURN();
    }
    
    if (!StringCheck::CheckSize(api_data.description, 0, STR_LEN_512))
    {
        ACK_PARA_INVALID("description");
        XML_RPC_RETURN();
    }

    MessageSaveAsImageReq req;
    req._request_id = GenerateUUID();
    _upool->GetUserNameByID(uid, req._user_name);
    req._uid = uid;
    req._project_name = api_data.project_name;
    req._is_deployed_only = api_data.is_deployed_only;
    req._is_image_save = api_data.is_image_save;
    req._description = api_data.description;
    
    /* 4. 消息转发并等应答 */
    WAITE_MSG_ACK(EV_PROJECT_SAVE_AS_IMAGE_REQ)
    {
        MessageSaveAsImageAck ack;
        ack.deserialize(message.data);
        if ((SUCCESS == ack.state) || (ERROR_ACTION_RUNNING == ack.state))
        {
            array_data.push_back(value_int(TECS_SUCCESS));
            array_data.push_back(value_string(ack._workflow_id));
        }
        else
        {
            array_data.push_back(value_int(RpcError(ack.state)));
            array_data.push_back(value_string(ack.detail));
        }
    }
    else
    {
        ACK_PROJECT_TIMEOUT();
    }

    /* 5. XML-RPC返回  */
    XML_RPC_RETURN();

}

/******************************************************************************/
void ProjectCreateByImageMethod::MethodEntry(const paramList& paramList,
                                        value* const retval)
{
    vector<value>     array_data;
    value_array       *array_result = NULL;

#if 0//租户自己和管理员都可以操作
    /* 1. 操作授权 */
    AUTHORIZE(AuthRequest::PROJECT, AuthRequest::MANAGE);
#endif

    /* 2. 获取参数 */  
    int64   uid = get_userid();
    ApiProjectCreateByImageData  api_data ;
    api_data.from_rpc(paramList.getStruct(1));
    
    /* 3. 检查参数 */
    if (INVALID_IID == api_data.image_id)
    {
        ACK_PARA_INVALID("image_id");
        XML_RPC_RETURN();
    }
    if (!StringCheck::CheckNormalName(api_data.project_name, 1, STR_LEN_32))
    {
        ACK_PARA_INVALID("project_name");
        XML_RPC_RETURN();
    }

    MessageCreateByImageReq req;
    req._request_id = GenerateUUID();
    _upool->GetUserNameByID(uid, req._user_name);
    req._uid = uid;
    req._image_id = api_data.image_id;
    req._project_name = api_data.project_name;
    
    /* 4. 消息转发并等应答 */
    WAITE_MSG_ACK(EV_PROJECT_CREATE_BY_IMAGE_REQ)
    {
        MessageCreateByImageAck ack;
        ack.deserialize(message.data);
        if ((SUCCESS == ack.state) || (ERROR_ACTION_RUNNING == ack.state))
        {
            array_data.push_back(value_int(TECS_SUCCESS));
            array_data.push_back(value_string(ack._workflow_id));
        }
        else
        {
            array_data.push_back(value_int(RpcError(ack.state)));
            array_data.push_back(value_string(ack.detail));
        }
    }
    else
    {
        ACK_PROJECT_TIMEOUT();
    }

    /* 5. XML-RPC返回  */
    XML_RPC_RETURN();

}

/******************************************************************************/
void ProjectStatisticsStaticMethod :: MethodEntry(const paramList& paramList,
                                       value* const retval)
{
    ostringstream     err_oss;
    string            err_str;
    ProjectOperate    *op_inst = NULL;
    int64             max_count = 0;
    int64             next_index = -1;



    /* 1. 获取参数 */
    ApiProjectQuery  api_data;

    api_data.start_index       = paramList.getI8(1);
    api_data.count             = paramList.getI8(2);
    api_data.type              = paramList.getI8(3);
    api_data.appointed_project = paramList.getString(4);
    api_data.appointed_user    = paramList.getString(5);


    Debug("\n++++ %s:\n%s\n\n", _method_name.c_str(),api_data.toString().c_str());
    /* 2. 校验参数 */
    bool success = api_data.Validate(err_str);
    if (!success)
    {
        xRET2(xINT(RpcError(ERROR_INVALID_ARGUMENT)),xSTR(err_str));
        return ;
    }

    vector<ApiProjectStatisticsStaticData>  vec_prj;
    int64                   appointed_uid = -1;

    int64   uid = get_userid();
    if (PROJECT_USER_APPOINTED == static_cast<ApiProjectQueryType>(api_data.type))
    {
        if (!_upool->GetUserIDByName(api_data.appointed_user, appointed_uid))
        {
            err_oss << "user:" << api_data.appointed_user<<"does not exist!";

            xRET2(xINT(RpcError(ERR_OBJECT_NOT_EXIST)),
                  xSTR(err_oss.str()));
            return;
        }
    }
    PROJECT_INIT(op_inst, err_oss);
    /* 3. 获取统计 */
    STATUS tRet = op_inst->DealStatisticsStaticData(api_data,
                                                    uid,
                                                    appointed_uid,
                                                    vec_prj,
                                                    max_count,
                                                    next_index,
                                                    err_oss);

    if (SUCCESS != tRet)
    {
        xRET2(xINT(RpcError(tRet)),
              xSTR(err_oss.str()));
        return;
    }
    else
    {    /* 7. 获取结果并返回 */
        vector<value>                          array_project;
        vector<ApiProjectStatisticsStaticData>::const_iterator iter;

        for (iter = vec_prj.begin(); iter != vec_prj.end(); iter++)
        {
            array_project.push_back(iter->to_rpc());
            Debug("\n %s:\n%s\n\n", _method_name.c_str(),iter->toString().c_str());
        }

        xRET4(xINT(RpcError(SUCCESS)),
              xI8(next_index),
              xI8(max_count),
              xARRAY(array_project));
    }
    Debug("\n%s, tRet = %d, err_str = %s, max_count = %d, next_index = %d\n", 
               _method_name.c_str(),
               tRet, 
               err_oss.str().c_str(),
               max_count,
               next_index);
    return;
}


/******************************************************************************/
void ProjectStatisticsDynamicMethod :: MethodEntry(const paramList& paramList,
                                       value* const retval)
{
    value_array       *array_result = NULL;
    vector<value>     array_data;
    ostringstream     err_oss;
    string            err_str;
    ProjectOperate	*op_inst = NULL;

    /* 1. 获取参数 */
    ApiProjectDataQuery  api_data;
    MessageProjectDataReq req;

    req._start_index       = paramList.getI8(1);
    req._count             = paramList.getI8(2);
    req._project_name      = paramList.getString(3);

    api_data.start_index        = paramList.getI8(1);
    api_data.count              = paramList.getI8(2);	
    api_data.appointed_project  = paramList.getString(3);


   /* 2. 验证参数 */
    Debug("\n++++ %s:\n%s\n", _method_name.c_str(),api_data.toString().c_str());
    bool  success = api_data.Validate(err_str);

    if (!success)
    {
        xRET2(xINT(RpcError(ERROR_INVALID_ARGUMENT)),xSTR(err_str));
        return ;
    }

    int64   uid = get_userid();

   /* 3. 获取cluster */
    string                  cluster_name;
    int64                   max_count     = 0;
    int64                   next_index    = -1;
    PROJECT_INIT(op_inst, err_oss);
    STATUS tRet = op_inst->DealProjectClusterQuery(api_data,
                                                   uid,
                                                   cluster_name,
                                                   max_count,
                                                   next_index,
                                                   err_oss);

    if (SUCCESS != tRet)
    {
        xRET2(xINT(RpcError(tRet)),
              xSTR(err_oss.str()));
        return;
    }
    else
    {
        if(cluster_name.empty())
        {
            err_oss << "project:" << api_data.appointed_project << ",user:" << api_data.appointed_user << " does not deploy vm ,cluster = "<< cluster_name <<"!";

            xRET2(xINT(RpcError(ERR_OBJECT_NOT_EXIST)),
                  xSTR(err_oss.str()));
            return;
        }

        /* 4. 消息转发并等应答 */
        WAITE_CC_MSG_ACK(EV_PROJECT_DYNAMIC_DATA_REQ)
        {
            MessageProjectDataAck ack;
            ack.deserialize(message.data);
            if (TECS_SUCCESS == RpcError(ack._ret_code))
            {
                array_data.push_back(value_int(TECS_SUCCESS));
                array_data.push_back(value_string(ack._project_name));
                array_data.push_back(value_i8(ack._next_index));
                array_data.push_back(value_i8(ack._max_count));

                vector<value> array_project_data;
                vector<ProjectDynamicData>::const_iterator it = ack._project_data.begin();
                for (; it != ack._project_data.end(); ++it)
                {
                    ApiProjectDynamicData h;
                    h.vm_id = it->_vm_id;
//                  h.vm_name = it->_vm_name;
                    h.nic_used_rate = it->_nic_used_rate;
                    h.cpu_used_rate = it->_cpu_used_rate;
                    h.mem_used_rate = it->_mem_used_rate;
                    array_project_data.push_back(h.to_rpc());
                }
                array_data.push_back(value_array(array_project_data));
            }
            else
            {
                array_data.push_back(value_int(ERR_PROJECT_QUERY_FAIL));
                array_data.push_back(value_string(CodeToStr(ERR_PROJECT_QUERY_FAIL)));
            }
        }
        else
        {
            ACK_PROJECT_TIMEOUT();
        }

        /* 5. XML-RPC返回  */
        XML_RPC_RETURN();
    }

}


}

