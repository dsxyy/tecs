/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：power_manager_methods.cpp
* 文件标识：见配置管理计划书
* 内容摘要：物理机管理模块涉及的XML-RPC method类的实现文件
* 当前版本：1.0
* 作    者：liuyi
* 完成日期：2013年4月30日
*
* 修改记录1：
*     修改日期：2013/4/30
*     版 本 号：V1.0
*     修 改 人：liuyi
*     修改内容：创建
*******************************************************************************/
#include "power_manager_methods.h"
#include "sky.h"
#include "mid.h"
#include "event.h"
#include "host_api.h"
#include "msg_power_manager_with_api_method.h"
#include "user_pool.h"
#include "authrequest.h"
#include "phy_port_pub.h"
#include "api_pub.h"


using namespace xmlrpc_c;
namespace zte_tecs {

#define TEMP_MU_NAME_PRE            "power_manager_api"

#define FANTRAY_INDEX_MIN     (int)1
#define FANTRAY_INDEX_MAX     (int)2
#define FANTRAY_RPM_MIN_LEVEL (int)1
#define FANTRAY_RPM_MAX_LEVEL (int)13

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
    receiver._unit = MU_POWER_MANAGER; \
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
        case ERR_HOST_OK:
        case ERR_HANDLE_OK:
        case ERR_CLUSTER_CFG_OK:
        case ERR_TCU_OK:
            return "OK.";
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
        case ERR_TCU_CFG_INVALID:
            return "Error, cpu_info and cpu_info_id are both valid.";
        case ERR_TCU_SET_FAILD:
            return "Failed, set tcu config failed.";
        case ERR_TCU_DEL_FAILD :
            return "Failed, delete tcu config failed.";
        case ERR_TCU_QUERY_FAILD :
            return "Failed, query tcu config failed.";
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
        case ERR_CMM_ADD_FAIL:
            return "Failed, cluster agent add cmm fail.";
        case ERR_CMM_SET_FAIL:
            return "Failed, cluster agent set cmm fail.";
        case ERR_CMM_DEL_FAIL:
            return "Failed, cluster agent delete cmm fail.";
        case ERR_CMM_QUERY_FAIL:
            return "Failed, cluster agent query cmm fail.";
        case ERR_TCU_VMS_RUNNING:
            return "Failed, set tcu num config failed because there are some VMs running.";
        case ERR_OPERATE_NOT_SUPPORT:
            return "Failed, operation not support.";
        case ERR_CMM_IP_NULL:
            return "Failed, ip of cmm not configured.";
        case ERR_CMM_CFG_INCORRECT:
            return "Failed, cmm configured incorrect.";
        
        default:
            ostringstream oss;
            oss << "Undefined return code=" << code << ".";
            return oss.str();
    }
}
/******************************************************************************/
void HostExternCtrlCmdMethod::MethodEntry(const paramList &paramList,
                                      value * const retval)
{
    vector<value>     array_data;
    value_array       *array_result = NULL;

    /* 1. 操作授权 */
    AUTHORIZE(AuthRequest::HOST, AuthRequest::INFO);

    /* 2. 获取参数 */
    MessageHostExternCtrlCmdReq req;
    string cluster_name = paramList.getString(1);
    req._host_name = paramList.getString(2);
    req._cmd       = paramList.getInt(3);
    req._arg_info  = paramList.getString(4);

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
    WAITE_MSG_ACK(EV_HOST_EXTERN_CTRL_REQ)
    {
        MessageHostExternCtrlCmdAck ack;
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

void HostExternCtrlQueryMethod::MethodEntry(const paramList &paramList,
                                      value * const retval)
{
    vector<value>     array_data;
    value_array       *array_result = NULL;

    /* 1. 操作授权 */
    AUTHORIZE(AuthRequest::HOST, AuthRequest::INFO);

    /* 2. 获取参数 */
    MessageHostExternCtrlQueryReq req;
    string cluster_name = paramList.getString(1);
    req._host_name = paramList.getString(2);
    req._arg_info  = paramList.getString(3);

    /* 3. 检查参数 */
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
    WAITE_MSG_ACK(EV_HOST_EXTERN_QUERY_REQ)
    {
        MessageHostExternCtrlQueryAck ack;
        ack.deserialize(message.data);
        
        // cout << "MessageHostExternCtrlQueryAck:"
        // << " host = "     << ack._host_name 
        // << " ret_code = " << ack._ret_code 
        // << " Steate =  "  << ack._state
        // << " ." << endl;
        // 成功和失败是分开解析的
        if (TECS_SUCCESS == RpcError(ack._ret_code)) 
        {
            array_data.push_back(value_int(TECS_SUCCESS));
            array_data.push_back(value_int(ack._state)); // 状态码
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
void FantrayRPMCtlMethod::MethodEntry(const paramList &paramList,
                                      value * const retval)
{
    vector<value>     array_data;
    value_array       *array_result = NULL;

    /* 1. 操作授权 */
    AUTHORIZE(AuthRequest::CLUSTER, AuthRequest::INFO);

    /* 2. 获取参数 */
    MessageFanRPMCtrlCmdReq req;
    string cluster_name = paramList.getString(1);
    req._cmm_bureau     = paramList.getInt(2);
    req._cmm_rack       = paramList.getInt(3);
    req._cmm_shelf      = paramList.getInt(4);
    req._fantray_index  = paramList.getInt(5);
    req._rpm_level      = paramList.getInt(6);

    /* 3. 检查参数 */
    if (!StringCheck::CheckNormalName(cluster_name, 1, STR_LEN_64))
    {
        ACK_PARA_INVALID("cluster_name");
        XML_RPC_RETURN();
    }
    if ((FANTRAY_INDEX_MIN > req._fantray_index)
        || (FANTRAY_INDEX_MAX < req._fantray_index))
    {// 1-2
        ACK_PARA_INVALID("fantray_index");
        XML_RPC_RETURN();
    }
    if ((FANTRAY_RPM_MIN_LEVEL > req._rpm_level)
        || (FANTRAY_RPM_MAX_LEVEL < req._rpm_level))
    {// 1-13
        ACK_PARA_INVALID("rpm_level");
        XML_RPC_RETURN();
    }

    /* 4. 消息转发并等应答 */
    MessageUnit temp_mu(TempUnitName(TEMP_MU_NAME_PRE));
    temp_mu.Register();
    MID receiver;
    receiver._application = cluster_name;
    receiver._process = PROC_CC;
    receiver._unit = MU_POWER_AGENT;
    MessageOption option(receiver, EV_FANTRAY_RPMCTL_REQ, 0, 0);
    temp_mu.Send(req, option);
    MessageFrame message;
    if(SUCCESS == temp_mu.Wait(&message, 5000))
    {
        MessageFanRPMCtrlCmdAck ack;
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

} //end namespace zte_tecs

