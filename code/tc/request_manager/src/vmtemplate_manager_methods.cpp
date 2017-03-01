/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：vmtemplate_api_method.cpp
* 文件标识：
* 内容摘要：VmTemplate 模块XML-RPC注册方法类的实现文件
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
#include "vmtemplate_manager_methods.h"
#include "mid.h"
#include "event.h"
#include "sky.h"
#include "vmtpl_api.h"
#include "log_agent.h"

static int vtm_print = 0;
DEFINE_DEBUG_VAR(vtm_print);
#define Debug(fmt,arg...) \
        do \
        { \
            if(vtm_print) \
            { \
                OutPut(SYS_DEBUG,"[%s:%d]"fmt,__func__,__LINE__, ##arg);\
            } \
        }while(0)

using namespace xmlrpc_c;

namespace zte_tecs
{

static STATUS VmTemplateInit(VmTemplateOperate *op_inst,
                             ostringstream  &err_oss)
{
    op_inst = VmTemplateOperate::GetInstance();
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

#define VT_INIT(op_inst, err_oss)\
        do \
        {\
            if (SUCCESS != VmTemplateInit(op_inst,err_oss )) \
            { \
                xRET2(xINT(RpcError(ERROR)),xSTR(err_oss.str())); \
                return ;\
            }\
        }while (0)\


/******************************************************************************/
void VmTemplateAllocateMethod::MethodEntry(const paramList& paramList,
        value* const retval)
{
    string              error_str;
    ostringstream       err_oss;
    VmTemplateOperate *op_inst=NULL;

    /* 1. 获取参数 */
    int64  uid = get_userid();

    ApiVtAllocateData  allocate_data;

    allocate_data.from_rpc(paramList.getStruct(1));
    Debug("\n++++ %s:\n%s\n", _method_name.c_str(),allocate_data.toString().c_str());
    bool  success = allocate_data.Validate(error_str);
    if (!success)//非法参数
    {
        xRET2(xINT(RpcError(ERROR_INVALID_ARGUMENT)),xSTR(error_str));
        return ;
    }

    VT_INIT(op_inst, err_oss);
    STATUS  tRet = op_inst->DealAllocate(allocate_data, uid, err_oss);

    xRET2(xINT(RpcError(tRet)),xSTR(err_oss.str()));
    Debug("\n%s, tRet = %d, err_str = %s\n", _method_name.c_str(),
          tRet, err_oss.str().c_str());
    return;
}


/******************************************************************************/
void VmTemplateDeleteMethod::MethodEntry(const paramList& paramList,
        value* const retval)
{
    ostringstream       err_oss;
    VmTemplateOperate *op_inst=NULL;

    /* 1. 获取参数 */
    int64  uid      = get_userid();
    string vt_name = value_string(paramList.getString(1));

    VT_INIT(op_inst, err_oss);
    STATUS  tRet = op_inst->DealDelete(uid, vt_name, err_oss);

    xRET2(xINT(RpcError(tRet)),xSTR(err_oss.str()));
    Debug("\n%s, tRet = %d, err_str = %s\n", _method_name.c_str(),
          tRet, err_oss.str().c_str());
    return;
}


/******************************************************************************/
void VmTemplateModifyMethod::MethodEntry(const paramList& paramList,
        value* const retval)
{
    ostringstream       err_oss;
    string              error_str;
    VmTemplateOperate *op_inst = NULL;

    /* 1. 获取参数 */
    int64 uid        = get_userid();

    ApiVtModifyData  modify_data;

    modify_data.from_rpc(paramList.getStruct(1));
    Debug("\n++++ %s:\n%s\n", _method_name.c_str(),modify_data.toString().c_str());
    bool  success = modify_data.Validate(error_str);
    if (!success)//非法参数
    {
        xRET2(xINT(RpcError(ERROR_INVALID_ARGUMENT)),xSTR(error_str));
        return;
    }

    VT_INIT(op_inst, err_oss);
    STATUS  tRet = op_inst->DealModify(modify_data, uid, err_oss);

    xRET2(xINT(RpcError(tRet)),xSTR(err_oss.str()));
    Debug("\n%s, tRet = %d, err_str = %s\n", _method_name.c_str(),
          tRet, err_oss.str().c_str());
    return;
}

/******************************************************************************/
void VmTemplateQueryMethod::MethodEntry(const paramList& paramList,
                                        value* const retval)
{
    ostringstream   err_oss;
    string          err_str;
    VmTemplateOperate *op_inst = NULL;

    ApiVtQuery    req;

    /* 1. 获取参数 */

    req.start_index = paramList.getI8(1);
    req.count       = paramList.getI8(2);
    req.type        = paramList.getInt(3);
    req.vt_name     = paramList.getString(4);

    Debug("\n++++ %s:\n%s\n ++++\n\n", _method_name.c_str(),req.toString().c_str());

    /* 2. 校验参数 */
    bool  success = req.Validate(err_str);
    if (!success)
    {
        xRET2(xINT(RpcError(ERROR_INVALID_ARGUMENT)),xSTR(err_str));
        return;
    }

    vector<ApiVtInfo>  vec_vt;
    int64   max_count  = 0;
    int64   next_index = -1;
    int64  uid = get_userid();

    VT_INIT(op_inst, err_oss);
    STATUS tRet = op_inst->DealQuery(req,
                                     uid,
                                     vec_vt,
                                     max_count,
                                     next_index,
                                     err_oss);

    if (SUCCESS != tRet)
    {
        xRET2(xINT(RpcError(tRet)),xSTR(err_oss.str()));
    }
    else
    {
        vector<value>                 array_vt;
        vector<ApiVtInfo>::const_iterator iter;

        for (iter = vec_vt.begin(); iter != vec_vt.end(); iter++)
        {
            array_vt.push_back(iter->to_rpc());
            Debug("\n %s:\n%s\n\n", _method_name.c_str(),iter->toString().c_str());
        }

        xRET4(xINT(RpcError(SUCCESS)),
              xI8(next_index),
              xI8(max_count),
              xARRAY(array_vt));
    }

    Debug("\n%s: tRet = %d, err_str = %s, max_count = %d, next_index = %d\n",
          _method_name.c_str(),
          tRet,
          err_oss.str().c_str(),
          max_count,
          next_index);
    return;
}


/******************************************************************************/
void VmTemplatePublishMethod::MethodEntry(const paramList& paramList,
        value* const retval)
{
    ostringstream       err_oss;
    VmTemplateOperate *op_inst = NULL;

    /* 1. 获取参数 */
    int64  uid         = get_userid();
    string vt_name     = paramList.getString(1);
    bool   enable_flag = paramList.getBoolean(2);

    VT_INIT(op_inst, err_oss);
    STATUS  tRet = op_inst->DealPublish(uid, vt_name, enable_flag, err_oss);

    xRET2(xINT(RpcError(tRet)),xSTR(err_oss.str()));
    Debug("\n%s, tRet = %d, err_str = %s\n", _method_name.c_str(),
          tRet, err_oss.str().c_str());
    return ;
}


/******************************************************************************/
void VmTemplateSetDiskMethod::MethodEntry(const paramList& paramList,
        value* const retval)
{
    vector<value>       array_disk;
    ostringstream       err_oss;
    string              err_str;
    VmTemplateOperate *op_inst = NULL;

    /* 1. 获取参数 */
    int64   uid     = get_userid();
    string  vt_name = paramList.getString(1);
    array_disk     = value_array(paramList.getArray(2)).vectorValueValue();

    vector<value>::iterator iter;
    vector<VmBaseDisk> vec_disk;
    for (iter = array_disk.begin();
            iter != array_disk.end();
            ++iter)
    {
        ApiVmDisk     api_disk;
        api_disk.from_rpc(value_struct(*iter));
        if (false == api_disk.Validate(err_str))
        {
            xRET2(xINT(RpcError(ERROR_INVALID_ARGUMENT)),xSTR(err_str));
            return;
        }

        VmBaseDisk vm_base_disk(INVALID_OID,
                                api_disk.target,
                                api_disk.position,
                                api_disk.bus,
                                api_disk.size,
                                api_disk.fstype);
        vec_disk.push_back(vm_base_disk);
    }

    VT_INIT(op_inst, err_oss);
    STATUS tRet = op_inst->SetDisks(vec_disk,
                                    vt_name,
                                    uid,
                                    err_oss);

    xRET2(xINT(RpcError(tRet)),xSTR(err_oss.str()));
    Debug("\n%s, tRet = %d, err_str = %s\n", _method_name.c_str(),
          tRet, err_oss.str().c_str());
    return ;
}


/******************************************************************************/
void VmTemplateSetImageDiskMethod :: MethodEntry(
    const paramList& paramList,
    value* const retval)
{
    vector<value>       array_disk;
    ostringstream       err_oss;
    string              err_str;
    VmTemplateOperate *op_inst = NULL;

    /* 1. 获取参数 */
    int64   uid     = get_userid();
    string  vt_name = paramList.getString(1);
    array_disk     = value_array(paramList.getArray(2)).vectorValueValue();

    vector<value>::iterator iter;
    vector<VmBaseImage> vec_image;
    for (iter = array_disk.begin();
            iter != array_disk.end();
            ++iter)
    {
        ApiVmDisk     api_disk;

        api_disk.from_rpc(value_struct(*iter));

        if (false == api_disk.Validate(err_str))
        {
            xRET2(xINT(RpcError(ERROR_INVALID_ARGUMENT)),xSTR(err_str));
            return;
        }

        VmBaseImage   vm_base_image(INVALID_OID,
                                    api_disk.target,
                                    api_disk.position,
                                    api_disk.id,
                                    api_disk.reserved_backup,
                                    api_disk.type);

        vec_image.push_back(vm_base_image);
    }

    VT_INIT(op_inst, err_oss);
    STATUS tRet = op_inst->SetImageDisks(vec_image,
                                         vt_name,
                                         uid,
                                         err_oss);

    xRET2(xINT(RpcError(tRet)),xSTR(err_oss.str()));
    Debug("\n%s, tRet = %d, err_str = %s\n", _method_name.c_str(),
          tRet, err_oss.str().c_str());
    return ;
}


/******************************************************************************/
void VmTemplateSetNicMethod :: MethodEntry(
    const paramList& paramList,
    value* const retval)
{
    vector<value>        array_nic;
    ostringstream        err_oss;
    string               err_str;
    VmTemplateOperate *op_inst = NULL;

    /* 1. 获取参数 */
    int64   uid      = get_userid();
    string  vt_name  = paramList.getString(1);
    array_nic        = value_array(paramList.getArray(2)).vectorValueValue();

    vector<VmBaseNic>   vec_nic;
    vector<value>::iterator iter;
    for (iter  = array_nic.begin();
            iter != array_nic.end();
            ++iter)
    {
        ApiVmNic    api_nic;
        VmNicConfig vm_nic;
        api_nic.from_rpc(value_struct(*iter));

        bool success = api_nic.Validate(err_str);
        if (!success)//非法参数
        {
            xRET2(xINT(RpcError(ERROR_INVALID_ARGUMENT)),xSTR(err_str));
            return ;
        }

        VmBaseNic   vm_base_nic(INVALID_OID,
                                api_nic.nic_index,
                                api_nic.sriov,
                                api_nic.loop,
                                api_nic.logic_network_id,
                                api_nic.model,
                                "");

        vec_nic.push_back(vm_base_nic);

    }

    VT_INIT(op_inst, err_oss);
    STATUS tRet = op_inst->SetNics(vec_nic,
                                   vt_name,
                                   uid,
                                   err_oss);

    xRET2(xINT(RpcError(tRet)),xSTR(err_oss.str()));
    Debug("\n%s, tRet = %d, err_str = %s\n", _method_name.c_str(),
          tRet, err_oss.str().c_str());
    return ;
}


}

