/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：version_manager_methods.cpp
* 文件标识：
* 内容摘要：版本管理XMl接口文件实现
* 当前版本：1.0
* 作    者：zhangyb
* 完成日期：2012年12月12日
*
* 修改记录1：
*     修改日期：2012/12/12
*     版 本 号：V1.0
*     修 改 人：zhangyb
*     修改内容：创建
******************************************************************************/
#include "version_manager_methods.h"
#include "tecs_errcode.h"
#include "authrequest.h"
#include "version_api.h"
#include "log_agent.h"

namespace zte_tecs
{
void VersionGetRepoAddr::MethodEntry(xmlrpc_c::paramList const& paramList,
                                  xmlrpc_c::value * const retval)
{

    int64                        uid = -1;
    int                          ret_code;

    vector<xmlrpc_c::value>        array_data;
    xmlrpc_c::value_array        *array_result = NULL;


    /* 1. 获取参数 */
    uid = get_userid();
    /* 获取授权 */
    AuthRequest ar(uid);

    ar.AddAuthorize(
                    AuthRequest::VERSION, //授权对象类型,用user做例子
                    0,                    //对象的ID
                    AuthRequest::INFO,  //操作类型
                    0,                    //对象拥有者的用户ID
                    false                 //此对象是否为公有
                    );

    if (-1 == UserPool::Authorize(ar))
    {   /* 授权失败 */
        //oss <<"user can't access!";
        array_data.push_back(xmlrpc_c::value_int(TECS_ERR_AUTHORIZE_FAILED));
        array_data.push_back(xmlrpc_c::value_string("user can't access!"));

        array_result = new xmlrpc_c::value_array(array_data);
        *retval = *array_result;

        delete array_result;
        return;
    }

    /* 2. 创建临时消息单元 */
    MessageUnit temp_mu(TempUnitName("Vmm_Temp"));
    temp_mu.Register();

    /* 3. 发消息给 Version_Manager_Agent  */
    MID mid(PROC_VERSION_MANAGER, MU_VERSION_MANAGER);
    MessageOption option(mid, EV_VERSION_GET_REPOADDR_REQ,0,0);
    temp_mu.Send("no data", option);
     //增加调试信息
    OutPut(SYS_DEBUG, "send EV_VERSION_GET_REPOADDR_REQ\n" );

    /* 4. 等待Cluster_MANAGER回应 */
    MessageFrame message;
    if(SUCCESS == temp_mu.Wait(&message,5000))
    {
        /*  收到应答，针对之前消息的应答 ?  */
        MessageVersionGetRepoAddrAck ack_message;
        ack_message.deserialize(message.data);
        /* 根据返回结果返回给后台 */
        ret_code = RpcError(ack_message._ack_result);
        array_data.push_back(xmlrpc_c::value_int(ret_code));
        array_data.push_back(xmlrpc_c::value_string(ack_message._error_string));
        array_data.push_back(xmlrpc_c::value_string(ack_message._repo_addr));
    }
    else
    {
        /*  超时 or 其他错误 ? */
        array_data.push_back(xmlrpc_c::value_int(TECS_ERR_TIMEOUT));
        array_data.push_back(xmlrpc_c::value_string("Version get repo addr timeout") );
        OutPut(SYS_ERROR, "Version get repo addr timeout");
    }

    /* 返回  */
    array_result = new xmlrpc_c::value_array(array_data);
    *retval = *array_result;

    delete array_result;

    return;
}

void VersionSetRepoAddr::MethodEntry(xmlrpc_c::paramList const& paramList,
                                  xmlrpc_c::value * const retval)
{

    int64                        uid = -1;
    int                          ret_code;

    vector<xmlrpc_c::value>        array_data;
    xmlrpc_c::value_array        *array_result = NULL;


    /* 1. 获取参数 */
    uid = get_userid();
    /* 获取授权 */
    AuthRequest ar(uid);

    ar.AddAuthorize(
                    AuthRequest::VERSION, //授权对象类型,用user做例子
                    0,                    //对象的ID
                    AuthRequest::INFO,  //操作类型
                    0,                    //对象拥有者的用户ID
                    false                 //此对象是否为公有
                    );

    if (-1 == UserPool::Authorize(ar))
    {   /* 授权失败 */
        //oss <<"user can't access!";
        array_data.push_back(xmlrpc_c::value_int(TECS_ERR_AUTHORIZE_FAILED));
        array_data.push_back(xmlrpc_c::value_string("user can't access!"));

        array_result = new xmlrpc_c::value_array(array_data);
        *retval = *array_result;

        delete array_result;
        return;
    }

    string repo_addr = value_string(paramList.getString(1));

    /* 2. 创建临时消息单元 */
    MessageUnit temp_mu(TempUnitName("Vmm_Temp"));
    temp_mu.Register();

    /* 3. 发消息给 Version_Manager_Agent  */
    MID mid(PROC_VERSION_MANAGER, MU_VERSION_MANAGER);
    MessageOption option(mid, EV_VERSION_SET_REPOADDR_REQ,0,0);
    temp_mu.Send(repo_addr, option);
     //增加调试信息
    OutPut(SYS_DEBUG, "send EV_VERSION_SET_REPOADDR_REQ " );

    /* 4. 等待Cluster_MANAGER回应 */
    MessageFrame message;
    if(SUCCESS == temp_mu.Wait(&message,5000))
    {
        /*  收到应答，针对之前消息的应答 ?  */
        MessageVersionSetRepoAddrAck ack_message;
        ack_message.deserialize(message.data);
        /* 根据返回结果返回给后台 */
        ret_code = RpcError(ack_message._ack_result);
        array_data.push_back(xmlrpc_c::value_int(ret_code));
        array_data.push_back(xmlrpc_c::value_string(ack_message._error_string));
    }
    else
    {
        /*  超时 or 其他错误 ? */
        array_data.push_back(xmlrpc_c::value_int(TECS_ERR_TIMEOUT));
        array_data.push_back(xmlrpc_c::value_string("Version set repo addr timeout") );
        OutPut(SYS_ERROR, "Version set repo addr timeout");
    }

    /* 返回  */
    array_result = new xmlrpc_c::value_array(array_data);
    *retval = *array_result;

    delete array_result;

    return;
}

void VersionQuery::MethodEntry(xmlrpc_c::paramList const& paramList,
                                  xmlrpc_c::value * const retval)
{

    int64                        uid = -1;
    int                          ret_code;

    vector<xmlrpc_c::value>        array_data;
    xmlrpc_c::value_array        *array_result = NULL;


    /* 1. 获取参数 */
    uid = get_userid();
    /* 获取授权 */
    AuthRequest ar(uid);

    ar.AddAuthorize(
                    AuthRequest::VERSION, //授权对象类型,用user做例子
                    0,                    //对象的ID
                    AuthRequest::INFO,  //操作类型
                    0,                    //对象拥有者的用户ID
                    false                 //此对象是否为公有
                    );

    if (-1 == UserPool::Authorize(ar))
    {   /* 授权失败 */
        //oss <<"user can't access!";
        array_data.push_back(xmlrpc_c::value_int(TECS_ERR_AUTHORIZE_FAILED));
        array_data.push_back(xmlrpc_c::value_string("user can't access!"));

        array_result = new xmlrpc_c::value_array(array_data);
        *retval = *array_result;

        delete array_result;
        return;
    }

    ApiVersionQueryReq  api_query;
    MessageVersionQueryReq version_query;

    api_query.from_rpc(paramList.getStruct(1));
    version_query.FromApi(api_query);

    /* 2. 创建临时消息单元 */
    MessageUnit temp_mu(TempUnitName("Vmm_Temp"));
    temp_mu.Register();

    /* 3. 发消息给 Version_Manager_Agent  */
    MID mid(PROC_VERSION_MANAGER, MU_VERSION_MANAGER);
    MessageOption option(mid, EV_VERSION_QUERY_REQ,0,0);
    temp_mu.Send(version_query, option);
     //增加调试信息
    OutPut(SYS_DEBUG, "send EV_VERSION_QUERY_REQ\n" );


    /* 4. 等待Cluster_MANAGER回应 */
    MessageFrame message;
    if(SUCCESS == temp_mu.Wait(&message,50000))
    {
        /*  收到应答，针对之前消息的应答 ?  */
        MessageVersionQueryAck ack_message;
        ApiVersionQueryAck api_ack_message;        
        ack_message.deserialize(message.data);
        ack_message.ToApi(api_ack_message);
        /* 根据返回结果返回给后台 */
        ret_code = RpcError(ack_message._ack_result);
        array_data.push_back(xmlrpc_c::value_int(ret_code));
        array_data.push_back(xmlrpc_c::value_string(ack_message._error_string));
        array_data.push_back(xmlrpc_c::value_struct(api_ack_message.to_rpc())); 
        printf("send EV_VERSION_QUERY_REQ ok:%d %s,%s,%s,%s,%s,%s,%s,%s,%s!\n",ret_code,
            api_ack_message._node_type.c_str(),
            api_ack_message._cloud_name.c_str(),
            api_ack_message._cloud_ip.c_str(),
            api_ack_message._cluster_name.c_str(),
            api_ack_message._cluster_ip.c_str(),
            api_ack_message._storage_adapter_name.c_str(),
            api_ack_message._storage_adapter_ip.c_str(),
            api_ack_message._host_name.c_str(),
            api_ack_message._host_ip.c_str());

    }
    else
    {
        /*  超时 or 其他错误 ? */
        array_data.push_back(xmlrpc_c::value_int(TECS_ERR_TIMEOUT));
        array_data.push_back(xmlrpc_c::value_string("Version query timeout") );
        OutPut(SYS_ERROR, "Version query timeout");
    }

    /* 返回  */
    array_result = new xmlrpc_c::value_array(array_data);
    *retval = *array_result;

    delete array_result;

    return;
}

void VersionUpdate::MethodEntry(xmlrpc_c::paramList const& paramList,
                                  xmlrpc_c::value * const retval)
{
    int64                        uid = -1;
    int                          ret_code;

    vector<xmlrpc_c::value>        array_data;
    xmlrpc_c::value_array        *array_result = NULL;


    /* 1. 获取参数 */
    uid = get_userid();
    /* 获取授权 */
    AuthRequest ar(uid);

    ar.AddAuthorize(
                    AuthRequest::VERSION, //授权对象类型,用user做例子
                    0,                    //对象的ID
                    AuthRequest::INFO,  //操作类型
                    0,                    //对象拥有者的用户ID
                    false                 //此对象是否为公有
                    );

    if (-1 == UserPool::Authorize(ar))
    {   /* 授权失败 */
        //oss <<"user can't access!";
        array_data.push_back(xmlrpc_c::value_int(TECS_ERR_AUTHORIZE_FAILED));
        array_data.push_back(xmlrpc_c::value_string("user can't access!"));

        array_result = new xmlrpc_c::value_array(array_data);
        *retval = *array_result;

        delete array_result;
        return;
    }


    ApiVersionUpdateReq  api_update;
    MessageVersionUpdateReq version_update;

    api_update.from_rpc(paramList.getStruct(1));
    version_update.FromApi(api_update);

    /* 2. 创建临时消息单元 */
    MessageUnit api_mu(TempUnitName("Vmm_Temp"));
    api_mu.Register();

    /* 3. 发消息给 Version_Manager  */
    MID mid(PROC_VERSION_MANAGER, MU_VERSION_MANAGER);
    MessageOption option(mid, EV_VERSION_UPDATE_REQ,0,0);
    api_mu.Send(version_update, option);
     //增加调试信息
    OutPut(SYS_DEBUG, "send EV_VERSION_UPDATE_REQ\n" );

    /* 4. 等待Version_Manager回应 */
    MessageFrame message;
    if(SUCCESS == api_mu.Wait(&message,5000))
    {
        /*  收到应答，针对之前消息的应答 ?  */
        MessageVersionUpdateAck ack_message;
        ack_message.deserialize(message.data);
        /* 根据返回结果返回给后台 */
        ret_code = RpcError(ack_message._ack_result);
        array_data.push_back(xmlrpc_c::value_int(ret_code));
        array_data.push_back(xmlrpc_c::value_string(ack_message._error_string));
        array_data.push_back(xmlrpc_c::value_i8(ack_message._oid));
    }
    else
    {
        /*  超时 or 其他错误 ? */
        array_data.push_back(xmlrpc_c::value_int(TECS_ERR_TIMEOUT));
        array_data.push_back(xmlrpc_c::value_string("Version update timeout") );
        OutPut(SYS_ERROR, "Version query timeout");
    }

    /* 返回  */
    array_result = new xmlrpc_c::value_array(array_data);
    *retval = *array_result;

    delete array_result;

    return;
}

void VersionCancel::MethodEntry(xmlrpc_c::paramList const& paramList,
                                  xmlrpc_c::value * const retval)
{
    int64                        uid = -1;
    int                          ret_code;

    vector<xmlrpc_c::value>        array_data;
    xmlrpc_c::value_array        *array_result = NULL;


    /* 1. 获取参数 */
    uid = get_userid();
    /* 获取授权 */
    AuthRequest ar(uid);

    ar.AddAuthorize(
                    AuthRequest::VERSION, //授权对象类型,用user做例子
                    0,                    //对象的ID
                    AuthRequest::INFO,  //操作类型
                    0,                    //对象拥有者的用户ID
                    false                 //此对象是否为公有
                    );

    if (-1 == UserPool::Authorize(ar))
    {   /* 授权失败 */
        //oss <<"user can't access!";
        array_data.push_back(xmlrpc_c::value_int(TECS_ERR_AUTHORIZE_FAILED));
        array_data.push_back(xmlrpc_c::value_string("user can't access!"));

        array_result = new xmlrpc_c::value_array(array_data);
        *retval = *array_result;

        delete array_result;
        return;
    }
    ApiVersionCancelReq  api_cancel;
    MessageVersionCancelReq version_cancel;
    api_cancel.from_rpc(paramList.getStruct(1));
    version_cancel.FromApi(api_cancel);

    /* 2. 创建临时消息单元 */
    MessageUnit temp_mu(TempUnitName("Vmm_Temp"));
    temp_mu.Register();

    /* 3. 发消息给 Version_Manager_Agent  */
    MID mid(PROC_VERSION_MANAGER, MU_VERSION_MANAGER);
    MessageOption option(mid, EV_VERSION_CANCEL_REQ,0,0);
    temp_mu.Send(version_cancel, option);
     //增加调试信息
    OutPut(SYS_DEBUG, "send EV_VERSION_UNINSTALL_REQ\n" );

    /* 4. 等待Cluster_MANAGER回应 */
    MessageFrame message;
    if(SUCCESS == temp_mu.Wait(&message,5000))
    {
        /*  收到应答，针对之前消息的应答 ?  */
        MessageVersionCancelAck ack_message;

        ack_message.deserialize(message.data);
        /* 根据返回结果返回给后台 */
        ret_code = RpcError(ack_message._ack_result);
        array_data.push_back(xmlrpc_c::value_int(ret_code));
        array_data.push_back(xmlrpc_c::value_string(ack_message._error_string));
    }
    else
    {
        /*  超时 or 其他错误 ? */
        array_data.push_back(xmlrpc_c::value_int(TECS_ERR_TIMEOUT));
        array_data.push_back(xmlrpc_c::value_string("Version cancel timeout") );
        OutPut(SYS_ERROR, "Version cancel timeout");
    }

    /* 返回  */
    array_result = new xmlrpc_c::value_array(array_data);
    *retval = *array_result;

    delete array_result;
    return;
}

} //end namespace zte_tecs

