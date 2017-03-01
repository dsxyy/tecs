/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：host_manager_methods.cpp
* 文件标识：见配置管理计划书
* 内容摘要：磁阵管理模块涉及的XML-RPC method类的实现文件
* 当前版本：1.0
* 作    者：冯骏
* 完成日期：2012年11月20日
*
* 修改记录1：
*     修改日期：2012/11/20
*     版 本 号：V1.0
*     修 改 人：冯骏
*     修改内容：创建
*******************************************************************************/
#include "sa_manager_methods.h"
#include "msg_sa_manager_with_api_method.h"
#include "sky.h"
#include "mid.h"
#include "event.h"
//#include "host_api.h"
#include "user_pool.h"
#include "authrequest.h"
#include "phy_port_pub.h"
#include "api_pub.h"

using namespace xmlrpc_c;
namespace zte_tecs {

#define TEMP_MU_NAME_PRE            "sa_manager_api"

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
        array_data.push_back(value_int(RpcError(ERR_SC_AUTHORIZE_FAILED))); \
        array_data.push_back(value_string(CodeToStr(ERR_SC_AUTHORIZE_FAILED)));\
        XML_RPC_RETURN(); \
    }

#define WAITE_MSG_ACK(msg_id) \
    MessageUnit temp_mu(TempUnitName(TEMP_MU_NAME_PRE)); \
    temp_mu.Register(); \
    MID receiver; \
    receiver._application = ApplicationName(); \
    receiver._process = PROC_SC; \
    receiver._unit = MU_SA_MANAGER; \
    MessageOption option(receiver, (msg_id), 0, 0); \
    temp_mu.Send(req, option); \
    MessageFrame message; \
    if(SUCCESS == temp_mu.Wait(&message, 5000))

#define ACK_SC_TIMEOUT() \
    array_data.push_back(value_int(RpcError(ERR_SC_TIMEOUT))); \
    array_data.push_back(value_string(CodeToStr(ERR_SC_TIMEOUT)));

#define ACK_PARA_INVALID(para) \
    array_data.push_back(value_int(RpcError(ERR_SC_PARA_INVALID))); \
    array_data.push_back(value_string(CodeToStr(ERR_SC_PARA_INVALID, (para))));

static const string CodeToStr(int32 code, const string &info = "")
{
    switch (code)
    {      
        case ERR_SC_TIMEOUT:
            return "Failed, sa manager is not answered.";   
        case ERR_SC_PARA_INVALID:
            return "Error, invalid para info, " + info + ".";
        case ERR_SC_AUTHORIZE_FAILED:
            return "Failed, sa manager authorize failed. ";   
        default:
            ostringstream oss;
            oss << "Undefined return code=" << code << ".";
            return oss.str();
    }
}


/******************************************************************************/
void SaDisableMethod::MethodEntry(const paramList &paramList,
                                    value * const retval)
{
    vector<value>     array_data;
    value_array       *array_result = NULL;

    /* 1. 操作授权 */
    AUTHORIZE(AuthRequest::SA, AuthRequest::MANAGE);

    /* 2. 获取参数 */    
    MessageSaEnableReq req;
    req._sid = paramList.getInt(1);   
    req._enable=false; 
    /* 3. 检查参数 */
  
    /* 4. 消息转发并等应答 */
    WAITE_MSG_ACK(EV_SA_ENABLE_REQ)
    {
        MessageSaCommonAck ack;
        ack.deserialize(message.data);
        if (TECS_SUCCESS == RpcError(ack._ret_code))
        {
            array_data.push_back(value_int(TECS_SUCCESS));
        }
        else
        {
            array_data.push_back(value_int(RpcError(ack._ret_code)));
            array_data.push_back(value_string(ack._result));
        }
    }
    else
    {
        ACK_SC_TIMEOUT();
    }

    /* 5. XML-RPC返回  */
    XML_RPC_RETURN();
}

/******************************************************************************/
void SaEnableMethod::MethodEntry(const paramList &paramList,
                                   value * const retval)
{
    vector<value>     array_data;
    value_array       *array_result = NULL;

    /* 1. 操作授权 */
    AUTHORIZE(AuthRequest::SA, AuthRequest::MANAGE);

    /* 2. 获取参数 */    
    MessageSaEnableReq req;
    req._sid = paramList.getInt(1);   
    req._enable=true; 
    /* 3. 检查参数 */
  
    /* 4. 消息转发并等应答 */
    WAITE_MSG_ACK(EV_SA_ENABLE_REQ)
    {
        MessageSaCommonAck ack;
        ack.deserialize(message.data);
        if (TECS_SUCCESS == RpcError(ack._ret_code))
        {
            array_data.push_back(value_int(TECS_SUCCESS));
        }
        else
        {
            array_data.push_back(value_int(RpcError(ack._ret_code)));
            array_data.push_back(value_string(ack._result));
        }
    }
    else
    {
        ACK_SC_TIMEOUT();
    }

    /* 5. XML-RPC返回  */
    XML_RPC_RETURN();
}

void SaForgetIdMethod::MethodEntry(const paramList &paramList,
                                   value * const retval)
{
    vector<value>     array_data;
    value_array       *array_result = NULL;

    /* 1. 操作授权 */
    AUTHORIZE(AuthRequest::SA, AuthRequest::DELETE);

    /* 2. 获取参数 */      
    MessageSaForgetIdReq req;
    req._sid = paramList.getInt(1);   
    
    /* 3. 检查参数 */
  
    /* 4. 消息转发并等应答 */
    WAITE_MSG_ACK(EV_SA_FORGET_ID_REQ)
    {
        MessageSaCommonAck ack;
        ack.deserialize(message.data);
        if (TECS_SUCCESS == RpcError(ack._ret_code))
        {
            array_data.push_back(value_int(TECS_SUCCESS));
        }
        else
        {
            array_data.push_back(value_int(RpcError(ack._ret_code)));
            array_data.push_back(value_string(ack._result));
        }
    }
    else
    {
        ACK_SC_TIMEOUT();
    }

    /* 5. XML-RPC返回  */
    XML_RPC_RETURN();
}


void SaMapClusterMethod::MethodEntry(const paramList &paramList,
                                   value * const retval)
{
    vector<value>     array_data;
    value_array       *array_result = NULL;

    /* 1. 操作授权 */
    AUTHORIZE(AuthRequest::SA, AuthRequest::CREATE);

    /* 2. 获取参数 */       
    MessageSaClusterMapReq req;
    req._sid = paramList.getInt(1);   
    req._cluster_name= paramList.getString(2);
    req._is_add= paramList.getBoolean(3);
    /* 3. 检查参数 */
    if (!StringCheck::CheckNormalName(req._cluster_name, 1, STR_LEN_64))
    {
        ACK_PARA_INVALID("cluster_name");
        XML_RPC_RETURN();
    }
    /* 4. 消息转发并等应答 */
    WAITE_MSG_ACK(EV_SA_CLUSTER_MAP_REQ)
    {
        MessageSaCommonAck ack;
        ack.deserialize(message.data);
        if (TECS_SUCCESS == RpcError(ack._ret_code))
        {
            array_data.push_back(value_int(TECS_SUCCESS));
        }
        else
        {
            array_data.push_back(value_int(RpcError(ack._ret_code)));
            array_data.push_back(value_string(ack._result));
        }
    }
    else
    {
        ACK_SC_TIMEOUT();
    }

    /* 5. XML-RPC返回  */
    XML_RPC_RETURN();
}

}

