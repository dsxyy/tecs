/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：affinity_region_manager_methods.cpp
* 文件标识：
* 内容摘要：affinity_region 模块XML-RPC注册方法类的实现文件
* 当前版本：1.0
* 作    者：
* 完成日期：2013年8月27日
*
* 修改记录1：
*    修改日期：2013/8/27
*    版 本 号：V1.0
*    修 改 人：
*    修改内容：创建
*******************************************************************************/
#include "affinity_region_manager_methods.h"
//#include "msg_project_manager_with_api_method.h"
#include "mid.h"
#include "event.h"
#include "sky.h"
#include "affinity_region_api.h"
#include "vmcfg_api.h"
#include "usergroup.h"
#include "log_agent.h"
#include "api_pub.h"
#include "authrequest.h"

static int armm_print = 1;
DEFINE_DEBUG_VAR(armm_print);
#define Debug(fmt,arg...) \
        do \
        { \
            if(armm_print) \
            { \
                OutPut(SYS_DEBUG,"[%s:%d]"fmt,__func__,__LINE__, ##arg);\
            } \
        }while(0)

using namespace xmlrpc_c;

namespace zte_tecs
{
#if 0
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
#endif
static STATUS AffinityRegionInit(AffinityRegionOperate *op_inst,
                              ostringstream  &err_oss)
{
    op_inst = AffinityRegionOperate::GetInstance();
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

#define AR_INIT(op_inst, err_oss)\
        do \
        {\
            if (SUCCESS != AffinityRegionInit(op_inst,err_oss )) \
            { \
                xRET2(xINT(RpcError(ERROR)),xSTR(err_oss.str())); \
                return ;\
            }\
        }while (0)\

/******************************************************************************/
void AffinityRegionAllocateMethod::MethodEntry(const paramList& paramList,
                                               value* const retval)
{
    string              err_str;
    ostringstream       err_oss;

    ApiAffinityRegionAllocateData  api_data;

    /* 1. 获取参数 */
    api_data.name = value_string(paramList.getString(1));
    api_data.description  = value_string(paramList.getString(2));
    api_data.level  = value_string(paramList.getString(3));
    api_data.is_forced = paramList.getInt(4);

    bool success = api_data.Validate(err_str);
    if (!success)
    {
        xRET2(xINT(RpcError(ERR_AFFINITY_REGION_PARA_INVALID)),xSTR(err_str));
        return ;
    }

    AffinityRegion  req;

    req._uid  = get_userid();
    req._name = api_data.name;
    req._description = api_data.description;
    req._level = api_data.level;
    req._is_forced= api_data.is_forced;

    MessageUnit    temp_mu(TempUnitName("tecs_affreg_allocate"));
    temp_mu.Register();

    MID            dest(PROC_TC, MU_VMCFG_MANAGER);
    MessageOption  option(dest, EV_VM_ALLOCATE_AFFREG_REQ, 0, 0);

    int32   wait_time = 3000;

    temp_mu.Send(req, option);
    /*  等待TC回应 */
    MessageFrame  msg;
    if (SUCCESS == temp_mu.Wait(&msg, wait_time))
    {
        /*  收到应答，针对之前消息的应答 ?  */
        OutPut(SYS_DEBUG,"AffinityRegionAllocateMethod : rcv affreg allocate ack msg:\n  %s \n\n",
                msg.data.c_str());
        VmAffregAllocateAck  tc_ack;
        tc_ack.deserialize(msg.data);
        if (SUCCESS != tc_ack.state)
        {
            xRET2(xINT(RpcError(tc_ack.state)),xSTR(tc_ack.detail));
        }
        else
        {
            xRET2(xINT(RpcError(tc_ack.state)),xI8(tc_ack.arid));
        }
    }
    else
    {
        xRET2(xINT(RpcError(ERR_RPOJECT_TIMEOUT)),xSTR("create affreg timeout"));
    }
    return;
}


/******************************************************************************/
void AffinityRegionDeleteMethod :: MethodEntry(const paramList& paramList,
                                        value* const retval)
{
    ostringstream    err_oss;
    AffinityRegionOperate *op_inst = NULL;

    ApiAffinityRegionDelete api_data;

    /* 1. 获取参数 */
    int64   uid                   = get_userid();
    api_data.affinity_region_id   = paramList.getI8(1);


    AR_INIT(op_inst, err_oss);

    STATUS tRet = op_inst->DealDelete(api_data, uid, err_oss);

    xRET2(xINT(RpcError(tRet)),xSTR(err_oss.str()));
    Debug("\n%s, tRet = %d, err_str = %s\n", _method_name.c_str(),
                                            tRet, err_oss.str().c_str());
    return;
}


/******************************************************************************/
void AffinityRegionModifyMethod::MethodEntry(const paramList& paramList,
                                        value* const retval)
{
    ostringstream    err_oss;
    string            err_str;
    AffinityRegionOperate   *op_inst = NULL;

    /* 1. 获取参数 */
    int64  uid       = get_userid();

    ApiAffinityRegionModifyData  api_data;
    api_data.id   = paramList.getI8(1);
    api_data.name    = paramList.getString(2);
    api_data.new_des = paramList.getString(3);
    api_data.level = paramList.getString(4);
    api_data.is_forced = paramList.getInt(5);

    bool success = api_data.Validate(err_str);
    if (!success)
    {
        xRET2(xINT(RpcError(ERR_AFFINITY_REGION_PARA_INVALID)),xSTR(err_str));
        return ;
    }

    AR_INIT(op_inst, err_oss);
    STATUS  tRet = op_inst->DealModify(api_data, uid, err_oss);

    xRET2(xINT(RpcError(tRet)),xSTR(err_oss.str()));
    Debug("\n%s, tRet = %d, err_str = %s\n", _method_name.c_str(),
                                            tRet, err_oss.str().c_str());
    return;

}

/******************************************************************************/
void AffinityRegionQueryMethod :: MethodEntry(const paramList& paramList,
                                       value* const retval)
{
    ostringstream     err_oss;
    string            err_str;
    AffinityRegionOperate	*op_inst = NULL;
    int64   appointed_uid;

    /* 1. 获取参数 */
    ApiAffinityRegionQuery  api_data;

    api_data.start_index       = paramList.getI8(1);
    api_data.count             = paramList.getI8(2);
    api_data.type              = paramList.getString(3);
    api_data.appointed_id      = paramList.getI8(4);
    api_data.appointed_user    = paramList.getString(5);
    //api_data.appointed_level   = paramList.getString(6);
    //api_data.from_rpc(paramList.getStruct(1));

    Debug("\n++++ %s:\n%s\n\n", _method_name.c_str(),api_data.toString().c_str());
    bool success = api_data.Validate(err_str);
    if (!success)
    {
        xRET2(xINT(RpcError(ERR_AFFINITY_REGION_PARA_INVALID)),xSTR(err_str));
        return ;
    }

    vector<ApiAffinityRegionInfo>  vec_ar;
    int64                   max_count     = 0;
    int64                   next_index    = -1;

    int64   uid = get_userid();
    bool is_admin = (UserGroup::CLOUD_ADMINISTRATOR == _upool->Get(uid,false)->get_role())?true: false;

    if (( RPC_QUERY_ALL_APPOINTED== api_data.type)||( RPC_QUERY_ALL== api_data.type))
    {
        if (!is_admin)
        {
            err_oss << "Error, only administrators can query appoined user affinity region!";
            xRET2(xINT(RpcError(ERR_AUTHORIZE_FAILED)),
                  xSTR(err_oss.str()));
        }

        if ((RPC_QUERY_ALL_APPOINTED == api_data.type)&&
            (!_upool->GetUserIDByName(api_data.appointed_user, appointed_uid)))
        {
            err_oss << "user:" << api_data.appointed_user<<"does not exist!";

            xRET2(xINT(RpcError(ERR_OBJECT_NOT_EXIST)),
                  xSTR(err_oss.str()));
        }

    }



    AR_INIT(op_inst, err_oss);
    STATUS tRet = op_inst->DealQuery(api_data,
                                      uid,
                                      appointed_uid,
                                      vec_ar,
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
        vector<value>                          array_ar;
        vector<ApiAffinityRegionInfo>::const_iterator iter;

        for (iter = vec_ar.begin(); iter != vec_ar.end(); iter++)
        {
            array_ar.push_back(iter->to_rpc());
            Debug("\n %s:\n%s\n\n", _method_name.c_str(),iter->toString().c_str());
        }

        xRET4(xINT(RpcError(SUCCESS)),
              xI8(next_index),
              xI8(max_count),
              xARRAY(array_ar));
    }
    Debug("\n%s, tRet = %d, err_str = %s, max_count = %d, next_index = %d\n",
               _method_name.c_str(),
               tRet,
               err_oss.str().c_str(),
               max_count,
               next_index);
    return;
}



}

