/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：cluster_agent_methods.cpp
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
#include "cluster_agent_methods.h"
#include "sky.h"
#include "mid.h"
#include "event.h"
#include "host_api.h"
#include "msg_cluster_agent_with_api_method.h"
#include "user_pool.h"
#include "authrequest.h"
#include "phy_port_pub.h"
#include "api_pub.h"
#include "vm_messages.h"

using namespace xmlrpc_c;
namespace zte_tecs {

#define TEMP_MU_NAME_PRE            "cluster_agent_api"

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
    receiver._unit = MU_CLUSTER_AGENT; \
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

        
        default:
            ostringstream oss;
            oss << "Undefined return code=" << code << ".";
            return oss.str();
    }
}

/******************************************************************************/
void ClusterConfigSetMethod::MethodEntry(const paramList &paramList,
                                         value * const retval)
{
    vector<value>     array_data;
    value_array       *array_result = NULL;

    /* 1. 权 */
    AUTHORIZE(AuthRequest::CLUSTER, AuthRequest::MANAGE);

    /* 2. 取 */
    MessageClusterCfgReq req;
    req._command = "SET";
    string cluster_name = paramList.getString(1);
    req._config_name = paramList.getString(2);
    req._config_value = paramList.getString(3);

    /* 3.  */
    if (!StringCheck::CheckNormalName(cluster_name, 1, STR_LEN_64))
    {
        ACK_PARA_INVALID("cluster_name");
        XML_RPC_RETURN();
    }
    if (!StringCheck::CheckNormalName(req._config_name, 1, STR_LEN_64))
    {
        ACK_PARA_INVALID("config_name");
        XML_RPC_RETURN();
    }

    /* 4. 息转应 */
    WAITE_MSG_ACK(EV_CLUSTER_CONFIG_REQ)
    {
        MessageClusterCfgAck ack;
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

    /* 5. XML-RPC  */
    XML_RPC_RETURN();
}

/******************************************************************************/
void ClusterConfigDelMethod::MethodEntry(const paramList &paramList,
                                         value * const retval)
{
    vector<value>     array_data;
    value_array       *array_result = NULL;

    /* 1. 权 */
    AUTHORIZE(AuthRequest::CLUSTER, AuthRequest::MANAGE);

    /* 2. 取 */
    MessageClusterCfgReq req;
    req._command = "DEL";
    string cluster_name = paramList.getString(1);
    req._config_name = paramList.getString(2);
    req._config_value = "";

    /* 3.  */
    if (!StringCheck::CheckNormalName(cluster_name, 1, STR_LEN_64))
    {
        ACK_PARA_INVALID("cluster_name");
        XML_RPC_RETURN();
    }
    if (!StringCheck::CheckNormalName(req._config_name, 1, STR_LEN_64))
    {
        ACK_PARA_INVALID("config_name");
        XML_RPC_RETURN();
    }

    /* 4. 息转应 */
    WAITE_MSG_ACK(EV_CLUSTER_CONFIG_REQ)
    {
        MessageClusterCfgAck ack;
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

    /* 5. XML-RPC  */
    XML_RPC_RETURN();
}

/******************************************************************************/
void ClusterConfigQueryMethod::MethodEntry(const paramList &paramList,
                                           value * const retval)
{
    vector<value>     array_data;
    value_array       *array_result = NULL;

    /* 1. 权 */
    AUTHORIZE(AuthRequest::CLUSTER, AuthRequest::INFO);

    /* 2. 取 */
    MessageClusterCfgQueryReq req;
    string cluster_name = paramList.getString(1);
    req._config_name = paramList.getString(2);

    /* 3.  */
    if (!StringCheck::CheckNormalName(cluster_name, 1, STR_LEN_64))
    {
        ACK_PARA_INVALID("cluster_name");
        XML_RPC_RETURN();
    }
    if (!StringCheck::CheckNormalName(req._config_name, 0, STR_LEN_64))
    {
        ACK_PARA_INVALID("config_name");
        XML_RPC_RETURN();
    }

    /* 4. 息转应 */
    WAITE_MSG_ACK(EV_CLUSTER_CONFIG_QUERY_REQ)
    {
        MessageClusterCfgQueryAck ack;
        ack.deserialize(message.data);
        if (TECS_SUCCESS == RpcError(ack._ret_code))
        {
            array_data.push_back(value_int(TECS_SUCCESS));

            vector<value> array_config;
            map<string, string>::const_iterator it = ack._config_infos.begin();
            for (; it != ack._config_infos.end(); ++it)
            {
                ApiClusterConfigInfo c;
                c.config_name = it->first;
                c.config_value = it->second;
                array_config.push_back(c.to_rpc());
            }
            array_data.push_back(value_array(array_config));

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

    /* 5. XML-RPC  */
    XML_RPC_RETURN();
}

/******************************************************************************/
void TcuConfigSetMethod::MethodEntry(const paramList &paramList,
                                       value * const retval)
{
    vector<value>     array_data;
    value_array       *array_result = NULL;

    /* 1. 权 */
    AUTHORIZE(AuthRequest::CLUSTER, AuthRequest::MANAGE);

    /* 2. 取 */
    MessageClusterTcuCfgReq req;
    req._command = "SET";
    string cluster_name = paramList.getString(1);
    req._cpu_info_id = paramList.getI8(2);
    req._tcu_num = paramList.getInt(3);
    req._description = paramList.getString(4);

    /* 3.  */
    if (!StringCheck::CheckNormalName(cluster_name, 1, STR_LEN_64))
    {
        ACK_PARA_INVALID("cluster_name");
        XML_RPC_RETURN();
    }
    if (!StringCheck::CheckSize(req._description, 0, STR_LEN_512))
    {
        ACK_PARA_INVALID("description");
        XML_RPC_RETURN();
    }    

    /* 4. 息转应 */
    WAITE_MSG_ACK(EV_CLUSTER_TCU_CONFIG_REQ)
    {
        MessageClusterTcuCfgAck ack;
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

    /* 5. XML-RPC  */
    XML_RPC_RETURN();
}

/******************************************************************************/
void TcuConfigDelMethod::MethodEntry(const paramList &paramList,
                                         value * const retval)
{
    vector<value>     array_data;
    value_array       *array_result = NULL;

    /* 1. 权 */
    AUTHORIZE(AuthRequest::CLUSTER, AuthRequest::MANAGE);

    /* 2. 取 */
    MessageClusterTcuCfgReq req;
    req._command = "DEL";
    string cluster_name = paramList.getString(1);
    req._cpu_info_id = paramList.getI8(2);
    req._tcu_num = 0;
    req._description = "";

    /* 3.  */
    if (!StringCheck::CheckNormalName(cluster_name, 1, STR_LEN_64))
    {
        ACK_PARA_INVALID("cluster_name");
        XML_RPC_RETURN();
    }

    /* 4. 息转应 */
    WAITE_MSG_ACK(EV_CLUSTER_TCU_CONFIG_REQ)
    {
        MessageClusterTcuCfgAck ack;
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

    /* 5. XML-RPC  */
    XML_RPC_RETURN();
}

/******************************************************************************/
void TcuConfigQueryMethod::MethodEntry(const paramList &paramList,
                                           value * const retval)
{
    vector<value>     array_data;
    value_array       *array_result = NULL;

    /* 1. 权 */
    AUTHORIZE(AuthRequest::CLUSTER, AuthRequest::INFO);

    /* 2. 取 */
    MessageClusterTcuQueryReq req;
    req._start_index = paramList.getI8(1);
    req._query_count = paramList.getI8(2);
    string cluster_name = paramList.getString(3);
    req._cpu_info = paramList.getString(4);
    req._cpu_info_id = paramList.getI8(5);

    /* 3.  */
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

    /* 4. 息转应 */
    WAITE_MSG_ACK(EV_CLUSTER_TCU_QUERY_REQ)
    {
        MessageClusterTcuQueryAck ack;
        ack.deserialize(message.data);
        if (TECS_SUCCESS == RpcError(ack._ret_code))
        {
            array_data.push_back(value_int(TECS_SUCCESS));
            array_data.push_back(value_i8(ack._next_index));
            array_data.push_back(value_i8(ack._max_count));

            vector<value> array_config;
            vector<TcuInfoType>::const_iterator it = ack._tcu_infos.begin();
            for (; it != ack._tcu_infos.end(); ++it)
            {
                ApiTcuInfo s;
                s.cpu_info_id = it->_cpu_info_id;
                s.tcu_num = it->_tcu_num;
                s.commend_tcu_num = it->_commend_tcu_num;
                s.cpu_info = it->_cpu_info;
                s.description = it->_description;
                array_config.push_back(s.to_rpc());
            }
            array_data.push_back(value_array(array_config));

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

    /* 5. XML-RPC  */
    XML_RPC_RETURN();
}

/******************************************************************************/
void CmmConfigAddMethod::MethodEntry(const paramList &paramList,
                                       value * const retval)
{
    vector<value>     array_data;
    value_array       *array_result = NULL;

    /* 1. 权 */
    AUTHORIZE(AuthRequest::CLUSTER, AuthRequest::MANAGE);

    /* 2. 取 */
    MessageCmmConfigReq req;
    string cluster_name = paramList.getString(1);
    req._bureau= paramList.getInt(2);
    req._rack= paramList.getInt(3);
    req._shelf= paramList.getInt(4);
    req._type= paramList.getString(5);
    req._ip_address= paramList.getString(6);
    req._description = paramList.getString(7);

    /* 3.  */
    if (!StringCheck::CheckNormalName(cluster_name, 1, STR_LEN_64))
    {
        ACK_PARA_INVALID("cluster_name");
        XML_RPC_RETURN();
    }

    if (0 >= req._bureau)
    {
        ACK_PARA_INVALID("bureau");
        XML_RPC_RETURN();
    }

    if (0 >= req._rack)
    {
        ACK_PARA_INVALID("rack");
        XML_RPC_RETURN();
    }

    if (0 >= req._shelf)
    {
        ACK_PARA_INVALID("shelf");
        XML_RPC_RETURN();
    }


    /* 4. 息转应 */
    WAITE_MSG_ACK(EV_CLUSTER_CMM_ADD_REQ)
    {
        MessageCmmConfigCfgAck ack;
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

    /* 5. XML-RPC  */
    XML_RPC_RETURN();
}

void CmmConfigSetMethod::MethodEntry(const paramList &paramList,
                                       value * const retval)
{
    vector<value>     array_data;
    value_array       *array_result = NULL;

    /* 1. 权 */
    AUTHORIZE(AuthRequest::CLUSTER, AuthRequest::MANAGE);

    /* 2. 取 */
    MessageCmmConfigReq req;
    string cluster_name = paramList.getString(1);
    req._bureau= paramList.getInt(2);
    req._rack= paramList.getInt(3);
    req._shelf= paramList.getInt(4);
    req._type= paramList.getString(5);
    req._ip_address= paramList.getString(6);
    req._description = paramList.getString(7);

    /* 3.  */
    if (!StringCheck::CheckNormalName(cluster_name, 1, STR_LEN_64))
    {
        ACK_PARA_INVALID("cluster_name");
        XML_RPC_RETURN();
    }

    if (0 >= req._bureau)
    {
        ACK_PARA_INVALID("bureau");
        XML_RPC_RETURN();
    }

    if (0 >= req._rack)
    {
        ACK_PARA_INVALID("rack");
        XML_RPC_RETURN();
    }

    if (0 >= req._shelf)
    {
        ACK_PARA_INVALID("shelf");
        XML_RPC_RETURN();
    }


    /* 4. 息转应 */
    WAITE_MSG_ACK(EV_CLUSTER_CMM_SET_REQ)
    {
        MessageCmmConfigCfgAck ack;
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

    /* 5. XML-RPC  */
    XML_RPC_RETURN();
}

/******************************************************************************/
void CmmConfigDeleteMethod::MethodEntry(const paramList &paramList,
                                       value * const retval)
{
    vector<value>     array_data;
    value_array       *array_result = NULL;

    /* 1. 权 */
    AUTHORIZE(AuthRequest::CLUSTER, AuthRequest::MANAGE);

    cout<<" CmmConfigDeleteMethod"<<endl;
    /* 2. 取 */
    MessageCmmDeleteReq req;
    string cluster_name = paramList.getString(1);
    req._bureau= paramList.getInt(2);
    req._rack= paramList.getInt(3);
    req._shelf= paramList.getInt(4);

    /* 3.  */
    if (!StringCheck::CheckNormalName(cluster_name, 1, STR_LEN_64))
    {
        cout<<" 3333333333333333333333333333333333"<<endl;

        ACK_PARA_INVALID("cluster_name");
        XML_RPC_RETURN();
    }

    if (0 >= req._bureau)
    {
            cout<<" 4444444444444444444444444444"<<endl;

        ACK_PARA_INVALID("bureau");
        XML_RPC_RETURN();
    }

    if (0 >= req._rack)
    {
            cout<<" 55555555555555555555555555555555555"<<endl;

        ACK_PARA_INVALID("rack");
        XML_RPC_RETURN();
    }

    if (0 >= req._shelf)
    {
            cout<<" 666666666666666666666666666666666666"<<endl;

        ACK_PARA_INVALID("shelf");
        XML_RPC_RETURN();
    }

    cout<<" !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!CmmConfigDeleteMethod"<<endl;

    /* 4. 息转应 */
    WAITE_MSG_ACK(EV_CLUSTER_CMM_DELETE_REQ)
    {
        MessageCmmDeleteAck ack;
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

    /* 5. XML-RPC  */
    XML_RPC_RETURN();
}

/******************************************************************************/
void CmmConfigQueryMethod::MethodEntry(const paramList &paramList,
                                           value * const retval)
{
    vector<value>     array_data;
    value_array       *array_result = NULL;

    /* 1. 权 */
    AUTHORIZE(AuthRequest::CLUSTER, AuthRequest::INFO);

    /* 2. 取 */
    MessageCmmQueryReq req;
    req._start_index = paramList.getI8(1);
    req._query_count = paramList.getI8(2);
    string cluster_name = paramList.getString(3);

    /* 3.  */
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

    /* 4. 息转应 */
    WAITE_MSG_ACK(EV_CLUSTER_CMM_QUERY_REQ)
    {
        MessageCmmQueryAck ack;
        ack.deserialize(message.data);
        if (TECS_SUCCESS == RpcError(ack._ret_code))
        {
            array_data.push_back(value_int(TECS_SUCCESS));
            array_data.push_back(value_i8(ack._next_index));
            array_data.push_back(value_i8(ack._max_count));

            vector<value> array_config;
            vector<CmmConfig>::const_iterator it = ack._cmm_infos.begin();
            for (; it != ack._cmm_infos.end(); ++it)
            {
                CmmConfigInfo s;
                s.bureau = it->_bureau;
                s.rack = it->_rack;
                s.shelf= it->_shelf;
                s.type= it->_type;
                s.ip_address= it->_ip_address;                
                s.description = it->_description;
                array_config.push_back(s.to_rpc());
            }
            array_data.push_back(value_array(array_config));

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

    /* 5. XML-RPC  */
    XML_RPC_RETURN();
}

/******************************************************************************/
void ClusterConfigSetSaveEnergyParaMethod::MethodEntry(const paramList &paramList,
                                           value * const retval)
{
    vector<value>     array_data;
    value_array       *array_result = NULL;

    /* 1. 操作授权 */
    AUTHORIZE(AuthRequest::CLUSTER, AuthRequest::MANAGE);

    /* 2. 获取参数 */
    MessageClusterSaveEnergyParaCfgReq req;
    req._command = "SET";
    string cluster_name = paramList.getString(1);
    req._save_energy_enable = paramList.getInt(2);
    req._max_host_num = paramList.getInt(3);
    req._min_host_num = paramList.getInt(4);
    req._host_percent = paramList.getInt(5);
    req._save_energy_interval = paramList.getInt(6);

    /* 3. 检查参数 */
    if (!StringCheck::CheckNormalName(cluster_name, 1, STR_LEN_64))
    {
        ACK_PARA_INVALID("cluster_name");
        XML_RPC_RETURN();
    }

    if (req._max_host_num < req._min_host_num)
    {
        ACK_PARA_INVALID("max_host_num");
        XML_RPC_RETURN();    
    }

    /* 4. 消息转发并等应答 */
    WAITE_MSG_ACK(EV_CLUSTER_SAVE_ENERGY_CONFIG_REQ)
    {
        MessageClusterCfgAck ack;
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
void ClusterConfigSaveEnergyParaQueryMethod::MethodEntry(const paramList &paramList,
                                           value * const retval)
{
    vector<value>     array_data;
    value_array       *array_result = NULL;

    /* 1. 鎿嶄綔鎺堟潈 */
    AUTHORIZE(AuthRequest::CLUSTER, AuthRequest::MANAGE);

    /* 2. 鑾峰彇鍙傛暟 */
    MessageClusterSaveEnergyParaQueryReq req;
    req._query_type = "SAVE_ENERGY";
    string cluster_name = paramList.getString(1);

    /* 3. 妫�鏌ュ弬鏁?*/
    if (!StringCheck::CheckNormalName(cluster_name, 1, STR_LEN_64))
    {
        ACK_PARA_INVALID("cluster_name");
        XML_RPC_RETURN();
    }


    /* 4. 娑堟伅杞彂骞剁瓑搴旂瓟 */
    WAITE_MSG_ACK(EV_CLUSTER_SAVE_ENERGY_QUERY_REQ)
    {
        MessageClusterSaveEnergyParaQueryAck ack;
        ack.deserialize(message.data);
        if (TECS_SUCCESS == RpcError(ack._ret_code))
        {
            array_data.push_back(value_int(TECS_SUCCESS));        
            array_data.push_back(value_int(ack._save_energy_enable));
            array_data.push_back(value_int(ack._max_host_num));            
            array_data.push_back(value_int(ack._min_host_num));
            array_data.push_back(value_int(ack._host_percent));
            array_data.push_back(value_int(ack._save_energy_interval));
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

    /* 5. XML-RPC杩斿洖  */
    XML_RPC_RETURN();
}

/******************************************************************************/
void SchedulerPolicySetMethod::MethodEntry(const paramList &paramList,
                                           value * const retval)
{
    vector<value>     array_data;
    value_array       *array_result = NULL;

    /* 1. 鎿嶄綔鎺堟潈 */
    AUTHORIZE(AuthRequest::CLUSTER, AuthRequest::MANAGE);

    /* 2. 鑾峰彇鍙傛暟 */
    string cluster_name = paramList.getString(1);
    MessageSchedulerPolicySetReq req;
    req._policy = paramList.getString(2);
	req._cycle_time = paramList.getInt(3);
	req._influence_vms = paramList.getInt(4);
	
    /* 3. 妫�鏌ュ弬鏁?*/
    if (!StringCheck::CheckNormalName(cluster_name, 1, STR_LEN_64))
    {
        ACK_PARA_INVALID("cluster_name");
        XML_RPC_RETURN();
    }

    /* 4. 娑堟伅杞彂骞剁瓑搴旂瓟 */
    WAITE_MSG_ACK(EV_VM_SCH_POLICY_SET_REQ)
	{
        MessageClusterCfgAck ack;
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

    /* 5. XML-RPC杩斿洖  */
    XML_RPC_RETURN();
}

/******************************************************************************/
void SchedulerPolicyQueryMethod::MethodEntry(const paramList &paramList,
                                           value * const retval)
{
    vector<value>     array_data;
    value_array       *array_result = NULL;

    /* 1. 鎿嶄綔鎺堟潈 */
    AUTHORIZE(AuthRequest::CLUSTER, AuthRequest::MANAGE);

    /* 2. 鑾峰彇鍙傛暟 */
    string cluster_name = paramList.getString(1);
    MessageClusterCfgReq req;	//鏌ヨ鐨勬椂鍊欙紝涓嶉渶瑕佸叆鍙傦紝绌烘秷鎭嵆鍙紝姝ゅ瀹氫箟req锛屾槸涓轰簡涓嬮潰鐨勫畯鍙互缂栬瘧閫氳繃
	
    /* 3. 妫�鏌ュ弬鏁?*/
    if (!StringCheck::CheckNormalName(cluster_name, 1, STR_LEN_64))
    {
        ACK_PARA_INVALID("cluster_name");
        XML_RPC_RETURN();
    }

    /* 4. 娑堟伅杞彂骞剁瓑搴旂瓟 */
    WAITE_MSG_ACK(EV_VM_SCH_POLICY_QUERY_REQ)
	{
        MessageSchedulerPolicyQueryAck ack;
        ack.deserialize(message.data);
        if (TECS_SUCCESS == RpcError(ack._ret_code))
        {
            array_data.push_back(value_int(TECS_SUCCESS));
            array_data.push_back(value_string(ack._policy));
            array_data.push_back(value_int(ack._cycle_time));
            array_data.push_back(value_int(ack._influence_vms));        }
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

    /* 5. XML-RPC杩斿洖  */
    XML_RPC_RETURN();
}

/******************************************************************************/
void VMHAPolicySetMethod::MethodEntry(const paramList &paramList,
                                           value * const retval)
{
    vector<value>     array_data;
    value_array       *array_result = NULL;

    /* 1. 鎿嶄綔鎺堟潈 */
    AUTHORIZE(AuthRequest::CLUSTER, AuthRequest::MANAGE);

    /* 2. 鑾峰彇鍙傛暟 */
    string cluster_name = paramList.getString(1);
    VmHAPolicy req;
    req.vm_crash = paramList.getString(2);
    req.host_down = paramList.getString(3);
    req.host_fault = paramList.getString(4);
    req.host_eth_down = paramList.getString(5);
    req.host_storage_fault = paramList.getString(6);

    /* 3. 妫�鏌ュ弬鏁?*/
    if (!StringCheck::CheckNormalName(cluster_name, 1, STR_LEN_64))
    {
        ACK_PARA_INVALID("cluster_name");
        XML_RPC_RETURN();
    }

    /* 4. 娑堟伅杞彂骞剁瓑搴旂瓟 */
    WAITE_MSG_ACK(EV_VM_HA_POLICY_SET_REQ)
	{
        MessageClusterCfgAck ack;
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

    /* 5. XML-RPC杩斿洖  */
    XML_RPC_RETURN();
}

/******************************************************************************/
void VMHAPolicyQueryMethod::MethodEntry(const paramList &paramList,
                                           value * const retval)
{
    vector<value>     array_data;
    value_array       *array_result = NULL;

    /* 1. 鎿嶄綔鎺堟潈 */
    AUTHORIZE(AuthRequest::CLUSTER, AuthRequest::MANAGE);

    /* 2. 鑾峰彇鍙傛暟 */
    string cluster_name = paramList.getString(1);
    MessageClusterCfgReq req;	//鏌ヨ鐨勬椂鍊欙紝涓嶉渶瑕佸叆鍙傦紝绌烘秷鎭嵆鍙紝姝ゅ瀹氫箟req锛屾槸涓轰簡涓嬮潰鐨勫畯鍙互缂栬瘧閫氳繃
	
    /* 3. 妫�鏌ュ弬鏁?*/
    if (!StringCheck::CheckNormalName(cluster_name, 1, STR_LEN_64))
    {
        ACK_PARA_INVALID("cluster_name");
        XML_RPC_RETURN();
    }

    /* 4. 娑堟伅杞彂骞剁瓑搴旂瓟 */
    WAITE_MSG_ACK(EV_VM_HA_POLICY_QUERY_REQ)
	{
        MessageVMHAPolicyQueryAck ack;
        ack.deserialize(message.data);
        if (TECS_SUCCESS == RpcError(ack._ret_code))
        {
            array_data.push_back(value_int(TECS_SUCCESS));
            array_data.push_back(value_string(ack.vm_crash));
            array_data.push_back(value_string(ack.host_down));
            array_data.push_back(value_string(ack.host_fault));
            array_data.push_back(value_string(ack.host_eth_down));
            array_data.push_back(value_string(ack.host_storage_fault));
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

    /* 5. XML-RPC杩斿洖  */
    XML_RPC_RETURN();
}

} //end namespace zte_tecs

