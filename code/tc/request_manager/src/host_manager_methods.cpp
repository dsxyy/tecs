/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：host_manager_methods.cpp
* 文件标识：见配置管理计划书
* 内容摘要：物理机管理模块涉及的XML-RPC method类的实现文件
* 当前版本：1.0
* 作    者：袁进坤
* 完成日期：2011年8月30日
*
* 修改记录1：
*     修改日期：2011/8/30
*     版 本 号：V1.0
*     修 改 人：袁进坤
*     修改内容：创建
*******************************************************************************/
#include "host_manager_methods.h"
#include "sky.h"
#include "mid.h"
#include "event.h"
#include "host_api.h"
#include "msg_host_manager_with_api_method.h"
#include "user_pool.h"
#include "authrequest.h"
#include "phy_port_pub.h"
#include "api_pub.h"

using namespace xmlrpc_c;
namespace zte_tecs {

#define TEMP_MU_NAME_PRE            "host_manager_api"

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
        array_data.push_back(value_int(RpcError(ERR_HM_AUTHORIZE_FAILED))); \
        array_data.push_back(value_string(CodeToStr(ERR_HM_AUTHORIZE_FAILED)));\
        XML_RPC_RETURN(); \
    }

#define WAITE_MSG_ACK(msg_id) \
    MessageUnit temp_mu(TempUnitName(TEMP_MU_NAME_PRE)); \
    temp_mu.Register(); \
    MID receiver; \
    receiver._application = cluster_name; \
    receiver._process = PROC_CC; \
    receiver._unit = MU_HOST_MANAGER; \
    MessageOption option(receiver, (msg_id), 0, 0); \
    temp_mu.Send(req, option); \
    MessageFrame message; \
    if(SUCCESS == temp_mu.Wait(&message, 5000))

#define ACK_CLUSTER_TIMEOUT() \
    array_data.push_back(value_int(RpcError(ERR_CLUSTER_TIMEOUT))); \
    array_data.push_back(value_string(CodeToStr(ERR_CLUSTER_TIMEOUT)));

#define ACK_PARA_INVALID(para) \
    array_data.push_back(value_int(RpcError(ERR_HM_PARA_INVALID))); \
    array_data.push_back(value_string(CodeToStr(ERR_HM_PARA_INVALID, (para))));

static const string CodeToStr(int32 code, const string &info = "")
{
    switch (code)
    {
        case ERR_HOST_FORCED_DISABLE:
            return "OK, host is forced disabled, please waite VMs to be deleted.";
        case ERR_HOST_OK:
        case ERR_HANDLE_OK:
        case ERR_TRUNK_OK:
        case ERR_CLUSTER_CFG_OK:
        case ERR_TCU_OK:
            return "OK.";
        case ERR_HOST_NOT_EXIST:
            return "Failed, host is not exist.";
        case ERR_HOST_VMS_RUNNING:
            return "Failed, there are some VMs running on the host.";
        case ERR_CLUSTER_TIMEOUT:
            return "Failed, Cluster Controller is not answered.";
        case ERR_HANDLE_EMPTY:
            return "Error, command is empty.";
        case ERR_HANDLE_FAILED:
            return "Failed, command execute failed.";
        case ERR_CLUSTER_NOT_REGISTER:
            return "Error, cluster have not registered into the Tecs Center.";
        case ERR_CLUSTER_CFG_NAME_INVALID:
            return "Error, cluster config name is illegal.";
        case ERR_CLUSTER_CFG_SET_FAILD:
            return "Failed, set cluster config failed.";
        case ERR_CLUSTER_CFG_DEL_FAILD:
            return "Failed, delete cluster config failed.";
        case ERR_CPUINFO_ID_NOT_EXIST:
            return "Error, cpu_info_id is not exist.";
        case ERR_HM_AUTHORIZE_FAILED :
            return "Error, failed to authorize usr's operation.";
        case ERR_HM_PARA_INVALID :
            return "Error, invalide parameter with " + info + ".";
        case ERR_HOST_NOT_SUPPORT : 
            return "Failed, the host not support the operation.";
        case ERR_HOST_THREAD_POOL_FAIL :
            return "Failed, host_manager create thread pool failed.";
        case ERR_HOST_DB_FAIL :
            return "Failed, host_manager update config failed.";
        case ERR_HOST_CTRL_HANDLE_FAIL:
            return "Failed, host_manager extern ctrl failed.";
        
        default:
            ostringstream oss;
            oss << "Undefined return code=" << code << ".";
            return oss.str();
    }
}
/******************************************************************************/
void HostSetMethod::MethodEntry(const paramList &paramList,
                                value * const retval)
{
    vector<value>     array_data;
    value_array       *array_result = NULL;

    /* 1. 操作授权 */
    AUTHORIZE(AuthRequest::HOST, AuthRequest::MANAGE);

    /* 2. 获取参数 */
    MessageHostSetReq req;
    string cluster_name = paramList.getString(1);
    req._host_name = paramList.getString(2);
    req._description = paramList.getString(3);

    /* 3. 检查参数 */
    if (!StringCheck::CheckNormalName(cluster_name, 1, STR_LEN_64))
    {
        ACK_PARA_INVALID("cluster_name");
        XML_RPC_RETURN();
    }
    if (!StringCheck::CheckNormalName(req._host_name, 1, STR_LEN_64))
    {
        ACK_PARA_INVALID("host_name");
        XML_RPC_RETURN();
    }
    if (!StringCheck::CheckSize(req._description, 0, STR_LEN_512))
    {
        ACK_PARA_INVALID("description");
        XML_RPC_RETURN();
    }

    /* 4. 消息转发并等应答 */
    WAITE_MSG_ACK(EV_HOST_SET_REQ)
    {
        MessageHostSetAck ack;
        ack.deserialize(message.data);
        if (TECS_SUCCESS == RpcError(ack._ret_code))
        {
            array_data.push_back(value_int(TECS_SUCCESS));
        }
        else
        {
            array_data.push_back(value_int(RpcError(ack._ret_code)));
            array_data.push_back(value_string(CodeToStr(ack._ret_code)));
        }
    }
    else
    {
        ACK_CLUSTER_TIMEOUT();
    }

    /* 5. XML-RPC返回  */
    XML_RPC_RETURN();
}

/******************************************************************************/
void HostForgetMethod::MethodEntry(const paramList &paramList,
                                   value * const retval)
{
    vector<value>     array_data;
    value_array       *array_result = NULL;

    /* 1. 操作授权 */
    AUTHORIZE(AuthRequest::HOST, AuthRequest::DELETE);

    /* 2. 获取参数 */
    MessageHostForgetReq req;
    string cluster_name = paramList.getString(1);
    req._host_name = paramList.getString(2);

    /* 3. 检查参数 */
    if (!StringCheck::CheckNormalName(cluster_name, 1, STR_LEN_64))
    {
        ACK_PARA_INVALID("cluster_name");
        XML_RPC_RETURN();
    }
    if (!StringCheck::CheckNormalName(req._host_name, 1, STR_LEN_64))
    {
        ACK_PARA_INVALID("host_name");
        XML_RPC_RETURN();
    }

    /* 4. 消息转发并等应答 */
    WAITE_MSG_ACK(EV_HOST_FORGET_REQ)
    {
        MessageHostForgetAck ack;
        ack.deserialize(message.data);
        if (TECS_SUCCESS == RpcError(ack._ret_code))
        {
            array_data.push_back(value_int(TECS_SUCCESS));
        }
        else
        {
            array_data.push_back(value_int(RpcError(ack._ret_code)));
            array_data.push_back(value_string(CodeToStr(ack._ret_code)));
        }
    }
    else
    {
        ACK_CLUSTER_TIMEOUT();
    }

    /* 5. XML-RPC返回  */
    XML_RPC_RETURN();
}

/******************************************************************************/
void HostDisableMethod::MethodEntry(const paramList &paramList,
                                    value * const retval)
{
    vector<value>     array_data;
    value_array       *array_result = NULL;

    /* 1. 操作授权 */
    AUTHORIZE(AuthRequest::HOST, AuthRequest::MANAGE);

    /* 2. 获取参数 */
    MessageHostDisableReq req;
    string cluster_name = paramList.getString(1);
    req._host_name = paramList.getString(2);
    req._is_forced = paramList.getBoolean(3);

    /* 3. 检查参数 */
    if (!StringCheck::CheckNormalName(cluster_name, 1, STR_LEN_64))
    {
        ACK_PARA_INVALID("cluster_name");
        XML_RPC_RETURN();
    }
    if (!StringCheck::CheckNormalName(req._host_name, 1, STR_LEN_64))
    {
        ACK_PARA_INVALID("host_name");
        XML_RPC_RETURN();
    }

    /* 4. 消息转发并等应答 */
    WAITE_MSG_ACK(EV_HOST_DISABLE_REQ)
    {
        MessageHostDisableAck ack;
        ack.deserialize(message.data);
        if (TECS_SUCCESS == RpcError(ack._ret_code))
        {
            array_data.push_back(value_int(TECS_SUCCESS));
        }
        else
        {
            array_data.push_back(value_int(RpcError(ack._ret_code)));
            array_data.push_back(value_string(CodeToStr(ack._ret_code)));
        }
    }
    else
    {
        ACK_CLUSTER_TIMEOUT();
    }

    /* 5. XML-RPC返回  */
    XML_RPC_RETURN();
}

/******************************************************************************/
void HostEnableMethod::MethodEntry(const paramList &paramList,
                                   value * const retval)
{
    vector<value>     array_data;
    value_array       *array_result = NULL;

    /* 1. 操作授权 */
    AUTHORIZE(AuthRequest::HOST, AuthRequest::MANAGE);

    /* 2. 获取参数 */
    MessageHostEnableReq req;
    string cluster_name = paramList.getString(1);
    req._host_name = paramList.getString(2);

    /* 3. 检查参数 */
    if (!StringCheck::CheckNormalName(cluster_name, 1, STR_LEN_64))
    {
        ACK_PARA_INVALID("cluster_name");
        XML_RPC_RETURN();
    }
    if (!StringCheck::CheckNormalName(req._host_name, 1, STR_LEN_64))
    {
        ACK_PARA_INVALID("host_name");
        XML_RPC_RETURN();
    }

    /* 4. 消息转发并等应答 */
    WAITE_MSG_ACK(EV_HOST_ENABLE_REQ)
    {
        MessageHostEnableAck ack;
        ack.deserialize(message.data);
        if (TECS_SUCCESS == RpcError(ack._ret_code))
        {
            array_data.push_back(value_int(TECS_SUCCESS));
        }
        else
        {
            array_data.push_back(value_int(RpcError(ack._ret_code)));
            array_data.push_back(value_string(CodeToStr(ack._ret_code)));
        }
    }
    else
    {
        ACK_CLUSTER_TIMEOUT();
    }

    /* 5. XML-RPC返回  */
    XML_RPC_RETURN();
}

/******************************************************************************/
void HostRebootMethod::MethodEntry(const paramList &paramList,
                                   value * const retval)
{
    vector<value>     array_data;
    value_array       *array_result = NULL;

    /* 1. 操作授权 */
    AUTHORIZE(AuthRequest::HOST, AuthRequest::MANAGE);

    /* 2. 获取参数 */
    MessageHostRebootReq req;
    string cluster_name = paramList.getString(1);
    req._host_name = paramList.getString(2);
    req._is_forced = paramList.getBoolean(3);

    /* 3. 检查参数 */
    if (!StringCheck::CheckNormalName(cluster_name, 1, STR_LEN_64))
    {
        ACK_PARA_INVALID("cluster_name");
        XML_RPC_RETURN();
    }
    if (!StringCheck::CheckNormalName(req._host_name, 1, STR_LEN_64))
    {
        ACK_PARA_INVALID("host_name");
        XML_RPC_RETURN();
    }

    /* 4. 消息转发并等应答 */
    WAITE_MSG_ACK(EV_HOST_REBOOT_REQ)
    {
        MessageHostRebootAck ack;
        ack.deserialize(message.data);
        if (TECS_SUCCESS == RpcError(ack._ret_code))
        {
            array_data.push_back(value_int(TECS_SUCCESS));
        }
        else
        {
            array_data.push_back(value_int(RpcError(ack._ret_code)));
            array_data.push_back(value_string(CodeToStr(ack._ret_code)));
        }
    }
    else
    {
        ACK_CLUSTER_TIMEOUT();
    }

    /* 5. XML-RPC返回  */
    XML_RPC_RETURN();
}

/******************************************************************************/
void HostShutdownMethod::MethodEntry(const paramList &paramList,
                                     value * const retval)
{
    vector<value>     array_data;
    value_array       *array_result = NULL;

    /* 1. 操作授权 */
    AUTHORIZE(AuthRequest::HOST, AuthRequest::MANAGE);

    /* 2. 获取参数 */
    MessageHostShutdownReq req;
    string cluster_name = paramList.getString(1);
    req._host_name = paramList.getString(2);

    /* 3. 检查参数 */
    if (!StringCheck::CheckNormalName(cluster_name, 1, STR_LEN_64))
    {
        ACK_PARA_INVALID("cluster_name");
        XML_RPC_RETURN();
    }
    if (!StringCheck::CheckNormalName(req._host_name, 1, STR_LEN_64))
    {
        ACK_PARA_INVALID("host_name");
        XML_RPC_RETURN();
    }

    /* 4. 消息转发并等应答 */
    WAITE_MSG_ACK(EV_HOST_SHUTDOWN_REQ)
    {
        MessageHostShutdownAck ack;
        ack.deserialize(message.data);
        if (TECS_SUCCESS == RpcError(ack._ret_code))
        {
            array_data.push_back(value_int(TECS_SUCCESS));
        }
        else
        {
            array_data.push_back(value_int(RpcError(ack._ret_code)));
            array_data.push_back(value_string(CodeToStr(ack._ret_code)));
        }
    }
    else
    {
        ACK_CLUSTER_TIMEOUT();
    }

    /* 5. XML-RPC返回  */
    XML_RPC_RETURN();
}

/******************************************************************************/
void HostQueryMethod::MethodEntry(const paramList &paramList,
                                  value * const retval)
{
    vector<value>     array_data;
    value_array       *array_result = NULL;

    /* 1. 操作授权 */
    AUTHORIZE(AuthRequest::HOST, AuthRequest::INFO);

    /* 2. 获取参数 */
    MessageHostQueryReq req;
    req._start_index = paramList.getI8(1);
    req._query_count = paramList.getI8(2);
    string cluster_name = paramList.getString(3);
    req._host_name = paramList.getString(4);

    /* 3. 检查参数 */
    if (req._query_count > 200)
    {
        ACK_PARA_INVALID("query_count");
        XML_RPC_RETURN();    
    }
    if (!StringCheck::CheckNormalName(cluster_name, 1, STR_LEN_64))
    {
        ACK_PARA_INVALID("cluster_name");
        XML_RPC_RETURN();
    }
    if (!StringCheck::CheckNormalName(req._host_name, 0, STR_LEN_64))
    {
        ACK_PARA_INVALID("host_name");
        XML_RPC_RETURN();
    }

    /* 4. 消息转发并等应答 */
    WAITE_MSG_ACK(EV_HOST_QUERY_REQ)
    {
        MessageHostQueryAck ack;
        ack.deserialize(message.data);
        if (TECS_SUCCESS == RpcError(ack._ret_code))
        {
            array_data.push_back(value_int(TECS_SUCCESS));
            array_data.push_back(value_i8(ack._next_index));
            array_data.push_back(value_i8(ack._max_count));

            vector<value> array_host;
            vector<HostInfoType>::const_iterator it = ack._host_infos.begin();
            for (; it != ack._host_infos.end(); ++it)
            {
                ApiHostInfo h;
                h.oid = it->_oid;
                h.host_name = it->_host_name;
                h.ip_address = it->_ip_address;
                h.run_state = it->_run_state;
                h.is_disabled = it->_is_disabled;
                h.create_time = it->_create_time;
                h.running_time = it->_running_time;
                h.cpu_info_id = it->_cpu_info_id;
                h.vmm_info = it->_vmm_info;
                h.os_info = it->_os_info;
                h.running_vms = it->_running_vms;
                h.tcu_max = it->_tcu_max;
                h.tcu_free = it->_tcu_free;
                h.mem_total = it->_mem_total;
                h.mem_max = it->_mem_max;
                h.mem_free = it->_mem_free;
                h.disk_total = it->_disk_total;
                h.disk_max = it->_disk_max;
                h.disk_free = it->_disk_free;
                h.cpu_usage_1m = it->_cpu_usage_1m;
                h.cpu_usage_5m = it->_cpu_usage_5m;
                h.cpu_usage_30m = it->_cpu_usage_30m;
                h.hardware_state = it->_hardware_state;
                h.description = it->_description;
                h.location   = it->_location;
                h.is_support_hvm = it->_is_support_hvm;
                h.rate_power = it->_rate_power;
                array_host.push_back(h.to_rpc());
            }
            array_data.push_back(value_array(array_host));
        }
        else
        {
            array_data.push_back(value_int(RpcError(ack._ret_code)));
            array_data.push_back(value_string(CodeToStr(ack._ret_code)));
        }
    }
    else
    {
        ACK_CLUSTER_TIMEOUT();
    }

    /* 5. XML-RPC返回  */
    XML_RPC_RETURN();
}

} //end namespace zte_tecs

