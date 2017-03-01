/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：cluster_config.cpp
* 文件标识：
* 内容摘要：cluster管理XMl接口文件实现
* 当前版本：1.0
* 作    者：李孝成
* 完成日期：2011年7月25日
*
* 修改记录1：
*     修改日期：2011/7/25
*     版 本 号：V1.0
*     修 改 人：李孝成
*     修改内容：创建
******************************************************************************/
#include "cluster_manager_methods.h"
#include "user_pool.h"
#include "authrequest.h"
#include "cluster_api.h"
#include "api_pub.h"

using namespace xmlrpc_c;
namespace zte_tecs
{

/*****************************************************************************/
/**
@brief 功能描述: 集群注销XML方法接口

@li @b 入参列表：
@verbatim
paramList    这个是XML传入的请求内容
@endverbatim
      
@li @b 出参列表：
@verbatim
retval      这个是该请求内置执行的结果
@endverbatim
        
@li @b 返回值列表：
@verbatim
无
@endverbatim
          
@li @b 接口限制：无
@li @b 其它说明：无
*/
/*****************************************************************************/
void ClusterSet::MethodEntry(xmlrpc_c::paramList const& paramList, 
                                  xmlrpc_c::value * const retval)
{
    int64                 uid = -1;
    string              cluster_name;
    string              cluster_appendinfo;
    ostringstream       oss;
    int                 ret_code = 0;
    
    /* 1. 获取参数 */
    uid = get_userid();
    
    /* 获取授权 */
    AuthRequest ar(uid);
    
    ar.AddAuthorize(
                    AuthRequest::CLUSTER, //授权对象类型,用user做例子
                    0,                    //对象的ID
                    AuthRequest::CREATE,  //操作类型
                    0,                    //对象拥有者的用户ID
                    false                 //此对象是否为公有
                    );
    
    if (-1 == UserPool::Authorize(ar))
    {   /* 授权失败 */
        oss <<"user can't access!";
        xRET2(xINT(TECS_ERR_AUTHORIZE_FAILED),xSTR(oss.str()));
        return;
    }
    
    /* 获取参数 */
    cluster_name = xmlrpc_c::value_string(paramList.getString(1));
    cluster_appendinfo = xmlrpc_c::value_string(paramList.getString(2));
    
    if (StringCheck::CheckNormalName(cluster_name,1,STR_LEN_64) != true)
    {
        oss << "Error, invalide parameter with  " << cluster_name;
        xRET2(xINT(TECS_ERR_PARA_INVALID),xSTR(oss.str()));
        return;       
    }
    
     if (StringCheck::CheckSize(cluster_appendinfo,0,STR_LEN_512) != true)
    {
        oss << "Error, invalide parameter with  " << cluster_appendinfo;
        xRET2(xINT(TECS_ERR_PARA_INVALID),xSTR(oss.str()));
        return;       
    }   
    
    /* 2. 创建临时消息单元 */
    MessageUnit temp_mu(TempUnitName("Cluster_Manager_Agent"));
    temp_mu.Register();
    
    /* 3. 发消息给 Cluster_Manager  */
    MessageClusterSetReq ReqMsg(cluster_name, cluster_appendinfo);
    MessageOption option(MID(PROC_TC, MU_CLUSTER_MANAGER), 
                         EV_CLUSTER_ACTION_SET_REQ,0,0);
    temp_mu.Send(ReqMsg, option);
    
    /* 4. 等待Cluster_MANAGER回应 */
    MessageFrame message;
    if(SUCCESS == temp_mu.Wait(&message,3000))
    {
        /*  收到应答，针对之前消息的应答 ?  */
        printf("temp_mu.Wait success!\n");
        MessageClusterSetAck  ack_message;
        ack_message.deserialize(message.data);
               
        /* 成功收到应答，返回操作结果 */
        ret_code = RpcError(ack_message._err_code);
        if (RpcError(ack_message._err_code) != TECS_SUCCESS)
        {
            oss << ack_message._result;
        }   
        else
        {
            oss << "";
        }
    }
    else
    {
        /*  超时 or 其他错误 ? */
        printf("temp_mu.Wait failed!\n");
        oss <<"Set Cluster TimeOut";
        ret_code = TECS_ERROR_CM_REQ_TIMEOUT;
    }
    
    /* 返回  */
    xRET2(xINT(ret_code),xSTR(oss.str()));
    return;
}



/*****************************************************************************/
/**
@brief 功能描述: 集群注销XML方法接口

@li @b 入参列表：
@verbatim
paramList    这个是XML传入的请求内容
@endverbatim
      
@li @b 出参列表：
@verbatim
retval      这个是该请求内置执行的结果
@endverbatim
        
@li @b 返回值列表：
@verbatim
无
@endverbatim
          
@li @b 接口限制：无
@li @b 其它说明：无
*/
/*****************************************************************************/
void ClusterForget::MethodEntry(xmlrpc_c::paramList const& paramList, 
                                  xmlrpc_c::value * const retval)
{
    int64                 uid = -1;
    string              cluster_name;
    ostringstream       oss;
    int                 ret_code = 0;
    
    /* 1. 获取参数 */
    uid = get_userid();
    
    /* 获取授权 */
    AuthRequest ar(uid);
    
    ar.AddAuthorize(
                    AuthRequest::CLUSTER, //授权对象类型,用user做例子
                    0,                    //对象的ID
                    AuthRequest::DELETE,  //操作类型
                    0,                    //对象拥有者的用户ID
                    false                 //此对象是否为公有
                    );
    
    if (-1 == UserPool::Authorize(ar))
    {   /* 授权失败 */
        oss <<"user can't access!";
        xRET2(xINT(TECS_ERR_AUTHORIZE_FAILED),xSTR(oss.str()));
        return;
    }
    
    /* 获取参数 */
    cluster_name = xmlrpc_c::value_string(paramList.getString(1));

    if (StringCheck::CheckNormalName(cluster_name,1,STR_LEN_64) != true)
    {
        oss << "Error, invalide parameter with  " << cluster_name;
        xRET2(xINT(TECS_ERR_PARA_INVALID),xSTR(oss.str()));
        return;       
    }
    
    /* 2. 创建临时消息单元 */
    MessageUnit temp_mu(TempUnitName("Cluster_Manager_Agent"));
    temp_mu.Register();
    
    /* 3. 发消息给 Cluster_Manager  */
    MessageClusterForgetReq ReqMsg(cluster_name);
    MessageOption option(MID(PROC_TC, MU_CLUSTER_MANAGER), 
                         EV_CLUSTER_ACTION_FORGET_REQ,0,0);
    temp_mu.Send(ReqMsg, option);
    
    /* 4. 等待Cluster_MANAGER回应 */
    MessageFrame message;
    if(SUCCESS == temp_mu.Wait(&message,3000))
    {
        /*  收到应答，针对之前消息的应答 ?  */
        printf("temp_mu.Wait success!\n");
        MessageClusterForgetAck  ack_message;
        ack_message.deserialize(message.data);
               
        /* 成功收到应答，返回操作结果 */
        ret_code = RpcError(ack_message._err_code);
        if (RpcError(ack_message._err_code) != TECS_SUCCESS)
        {
            oss << ack_message._result;
        }        
        else
        {
            oss << "";
        }
    }
    else
    {
        /*  超时 or 其他错误 ? */
        printf("temp_mu.Wait failed! \n");
        oss <<"Forget Cluster TimeOut";
        ret_code = TECS_ERROR_CM_REQ_TIMEOUT;
    }
    
    /* 返回  */
    xRET2(xINT(ret_code),xSTR(oss.str()));
    return;
}


/*****************************************************************************/
/**
@brief 功能描述: 集群信息请求XML方法接口

@li @b 入参列表：
@verbatim
paramList    这个是XML传入的请求内容
@endverbatim
      
@li @b 出参列表：
@verbatim
retval      这个是该请求内置执行的结果
@endverbatim
        
@li @b 返回值列表：
@verbatim
无
@endverbatim
          
@li @b 接口限制：无
@li @b 其它说明：无
*/
/*****************************************************************************/
void ClusterQuery::MethodEntry(xmlrpc_c::paramList const& paramList, 
                               xmlrpc_c::value * const retval)
{
    int64                 uid = -1;
    int64                 start_index =0;
    int64                 query_count = 0;
    int32               is_syn;
    uint32               i;
    ostringstream       oss;
    int                 ret_code = 0;
    
    /* 1. 获取参数 */
    uid = get_userid();
    
    /* 获取授权 */
    AuthRequest ar(uid);
    
    ar.AddAuthorize(
                    AuthRequest::CLUSTER, //授权对象类型,用user做例子
                    0,                    //对象的ID
                    AuthRequest::INFO,  //操作类型
                    0,                    //对象拥有者的用户ID
                    false                 //此对象是否为公有
                    );
    
    if (-1 == UserPool::Authorize(ar))
    {   /* 授权失败 */
        oss <<"user can't access!";
        xRET2(xINT(TECS_ERR_AUTHORIZE_FAILED),xSTR(oss.str()));
        return;
    }
    
    /* 获取参数 */
    start_index = xmlrpc_c::value_i8(paramList.getI8(1));
    query_count = xmlrpc_c::value_i8(paramList.getI8(2));
    is_syn = xmlrpc_c::value_int(paramList.getInt(3));

	/* 单包数量限值在200条记录 */
	if (((uint64)query_count > 200) || ((uint64)query_count == 0))
	{
        oss <<"query count too large,range 1-200";
        xRET2(xINT(TECS_ERR_PARA_INVALID),xSTR(oss.str()));
        return;
	}
	
    /* 2. 创建临时消息单元 */
    MessageUnit temp_mu(TempUnitName("Cluster_Manager_Agent"));
    temp_mu.Register();
    
    /* 3. 发消息给 Cluster_Manager  */
    MessageClusterInfoReq ReqMsg(is_syn);
	ReqMsg._StartIndex = start_index;
	ReqMsg._Query_count = query_count;
    MessageOption option(MID(PROC_TC, MU_CLUSTER_MANAGER), 
                         EV_CLUSTER_ACTION_INFO_REQ,0,0);
	
    temp_mu.Send(ReqMsg, option);
    
    /* 4. 等待Cluster_MANAGER回应 */
    MessageFrame message;
    if(SUCCESS == temp_mu.Wait(&message,4000))
    {
        /*  收到应答，针对之前消息的应答 ?  */
        printf("temp_mu.Wait success!\n");
        MessageClusterInfoAck  ack_message;
        ack_message.deserialize(message.data);
               
        /* 成功收到应答，返回操作结果 */
        vector<xmlrpc_c::value> array_cluster;
        for (i = 0; i < ack_message._cluster_resource.size(); i++)
        {
            ApiCluster  data;
            
            data.cluster_name = ack_message._cluster_resource.at(i)._cluster_name;
            data.ip = ack_message._cluster_resource.at(i)._ip;
            data.cluster_append = ack_message._cluster_resource.at(i)._cluster_append;
            data.is_online = ack_message._cluster_resource.at(i)._is_online;
            data.enabled = ack_message._cluster_resource.at(i)._enabled;

            data.core_free_max = ack_message._cluster_resource.at(i)._resource._core_free_max;
            data.tcu_unit_free_max = ack_message._cluster_resource.at(i)._resource._tcu_unit_free_max;
            
            data.tcu_max_total = ack_message._cluster_resource.at(i)._resource._tcu_max_total;
            data.tcu_free_total = ack_message._cluster_resource.at(i)._resource._tcu_free_total;
            data.tcu_free_max = ack_message._cluster_resource.at(i)._resource._tcu_free_max;
            
            data.disk_max_total = ack_message._cluster_resource.at(i)._resource._disk_max_total;
            data.disk_free_total = ack_message._cluster_resource.at(i)._resource._disk_free_total;
            data.disk_free_max = ack_message._cluster_resource.at(i)._resource._disk_free_max;
            
            data.share_disk_max_total = ack_message._cluster_resource.at(i)._resource._share_disk_max_total;
            data.share_disk_free_total = ack_message._cluster_resource.at(i)._resource._share_disk_free_total;

            data.mem_max_total = ack_message._cluster_resource.at(i)._resource._mem_max_total;
            data.mem_free_total = ack_message._cluster_resource.at(i)._resource._mem_free_total;
            data.mem_free_max = ack_message._cluster_resource.at(i)._resource._mem_free_max;
            
            array_cluster.push_back(data.to_rpc());
        }
        xRET4(xINT(TECS_SUCCESS), xI8(ack_message._next_index), xI8(ack_message._max_count), xARRAY(array_cluster));
    }
    else
    {
        /*  超时 or 其他错误 ? */
        printf("temp_mu.Wait failed!\n");
        oss <<"query Cluster TimeOut";
        ret_code = TECS_ERROR_CM_REQ_TIMEOUT;
        xRET2(xINT(ret_code),xSTR(oss.str()));
    }
    
    /* 返回  */
    return;
}

/*****************************************************************************/
/**
@brief 功能描述: 集群信息请求XML方法接口

@li @b 入参列表：
@verbatim
paramList    这个是XML传入的请求内容
@endverbatim
      
@li @b 出参列表：
@verbatim
retval      这个是该请求内置执行的结果
@endverbatim
        
@li @b 返回值列表：
@verbatim
无
@endverbatim
          
@li @b 接口限制：无
@li @b 其它说明：无
*/
/*****************************************************************************/
void ClusterQueryCoreAndTcu::MethodEntry(xmlrpc_c::paramList const& paramList, 
                               xmlrpc_c::value * const retval)
{
    int64                 uid = -1;
    ostringstream       oss;
    
    /* 1. 获取参数 */
    uid = get_userid();
    
    /* 不需要授权，所有用户都可以查看 */

    /* 2. 创建临时消息单元 */
    MessageUnit temp_mu(TempUnitName("Cluster_Manager_Agent"));
    temp_mu.Register();
    
    /* 3. 发消息给 Cluster_Manager,  */
    MessageClusterInfoReq ReqMsg(0);
    MessageOption option(MID(PROC_TC, MU_CLUSTER_MANAGER), 
                         EV_CLUSTER_ACTION_QUERY_CORE_TCU,0,0);
	
    temp_mu.Send(ReqMsg, option);
    
    /* 4. 等待Cluster_MANAGER回应 */
    MessageFrame message;
    if(SUCCESS == temp_mu.Wait(&message,4000))
    {
        /*  收到应答，针对之前消息的应答 ?  */
        printf("temp_mu.Wait success!\n");
        MessageClusterQueryCoreTcuAck  ack_message(0,0);
        ack_message.deserialize(message.data);
               
        /* 成功收到应答，返回操作结果 */
        xRET3(xINT(TECS_SUCCESS),xINT(ack_message._core_num), xINT(ack_message._tcu_num));
    }
    else
    {
        /*  超时 or 其他错误 ? */
        printf("temp_mu.Wait failed!\n");
        oss <<"query Cluster TimeOut";
        xRET2(xINT(TECS_ERROR_CM_REQ_TIMEOUT),xSTR(oss.str()));
    }
    
    /* 返回  */
    return;
}

/*****************************************************************************/
/**
@brief 功能描述: 集群信息请求XML方法接口

@li @b 入参列表：
@verbatim
paramList    这个是XML传入的请求内容
@endverbatim
      
@li @b 出参列表：
@verbatim
retval      这个是该请求内置执行的结果
@endverbatim
        
@li @b 返回值列表：
@verbatim
无
@endverbatim
          
@li @b 接口限制：无
@li @b 其它说明：无
*/
/*****************************************************************************/
void ClusterQueryNetPlane::MethodEntry(xmlrpc_c::paramList const& paramList, 
                               xmlrpc_c::value * const retval)
{
    int64                 uid = -1;
    uint32               i;
    ostringstream       oss;
    string              cluster_name;
    int                 ret_code = 0;
    
    /* 1. 获取参数 */
    uid = get_userid();
    cluster_name = xmlrpc_c::value_string(paramList.getString(1));
    
    /* 不需要授权，所有用户都可以查看 */

    /* 2. 创建临时消息单元 */
    MessageUnit temp_mu(TempUnitName("Cluster_Manager_Agent"));
    temp_mu.Register();
    
    /* 3. 发消息给 Cluster_Manager,  */
    MessageClusterNetPlanReq ReqMsg(cluster_name);
    MessageOption option(MID(PROC_TC, MU_CLUSTER_MANAGER), 
                         EV_CLUSTER_ACTION_QUERY_NETPLAN,0,0);
	
    temp_mu.Send(ReqMsg, option);
    
    /* 4. 等待Cluster_MANAGER回应 */
    MessageFrame message;
    if(SUCCESS == temp_mu.Wait(&message,4000))
    {
        /*  收到应答，针对之前消息的应答 ?  */
        printf("temp_mu.Wait success!\n");
        MessageClusterQueryNetPlaneAck  ack_message;
        ack_message.deserialize(message.data);
        
        sort(ack_message._net_plane.begin(),ack_message._net_plane.end());  //sort
        ack_message._net_plane.erase(unique(ack_message._net_plane.begin(), ack_message._net_plane.end()),ack_message._net_plane.end()); //delete repeated item

        vector<value> array_netplane;
        
        /* 成功收到应答，返回操作结果 */
        ret_code = TECS_SUCCESS;
        for (i = 0; i < ack_message._net_plane.size(); i++)
        {
            array_netplane.push_back(xmlrpc_c::value_string(ack_message._net_plane.at(i)));
        }
        
        xRET2(xINT(ret_code), xARRAY(array_netplane));
    }
    else
    {
        /*  超时 or 其他错误 ? */
        printf("temp_mu.Wait failed!\n");
        oss <<"query Cluster TimeOut";
        xRET2(xINT(TECS_ERROR_CM_REQ_TIMEOUT),xSTR(oss.str()));
    }
    
    /* 返回  */
    return;
}



/*****************************************************************************/
/**
@brief 功能描述: 集群开始维护XML方法接口

@li @b 入参列表：
@verbatim
paramList    这个是XML传入的请求内容
@endverbatim
      
@li @b 出参列表：
@verbatim
retval      这个是该请求内置执行的结果
@endverbatim
        
@li @b 返回值列表：
@verbatim
无
@endverbatim
          
@li @b 接口限制：无
@li @b 其它说明：无
*/
/*****************************************************************************/
void ClusterPreserveStart::MethodEntry(xmlrpc_c::paramList const& paramList, 
                                  xmlrpc_c::value * const retval)
{
    int64                 uid = -1;
    string              cluster_name;
    ostringstream       oss;
    int                 ret_code = 0;
    
    /* 1. 获取参数 */
    uid = get_userid();
    
    /* 获取授权 */
    AuthRequest ar(uid);
    
    ar.AddAuthorize(
                    AuthRequest::CLUSTER, //授权对象类型,用user做例子
                    0,                    //对象的ID
                    AuthRequest::MANAGE,  //操作类型
                    0,                    //对象拥有者的用户ID
                    false                 //此对象是否为公有
                    );
    
    if (-1 == UserPool::Authorize(ar))
    {   /* 授权失败 */
        oss <<"user can't access!";
        xRET2(xINT(TECS_ERR_AUTHORIZE_FAILED),xSTR(oss.str()));
        return;
    }
    
    /* 获取参数 */
    cluster_name = xmlrpc_c::value_string(paramList.getString(1));
  
    if (StringCheck::CheckNormalName(cluster_name,1,STR_LEN_64) != true)
    {
        oss << "Error, invalide parameter with  " << cluster_name;
        xRET2(xINT(TECS_ERR_PARA_INVALID),xSTR(oss.str()));
        return;       
    }
  
    /* 2. 创建临时消息单元 */
    MessageUnit temp_mu(TempUnitName("Cluster_Manager_Agent"));
    temp_mu.Register();
    
    /* 3. 发消息给 Cluster_Manager  */
    MessageClusterRunStateModifyReq ReqMsg(cluster_name);
    MessageOption option(MID(PROC_TC, MU_CLUSTER_MANAGER), 
                         EV_CLUSTER_ACTION_STOP_REQ,0,0);
    temp_mu.Send(ReqMsg, option);
    
    /* 4. 等待Cluster_MANAGER回应 */
    MessageFrame message;
    if(SUCCESS == temp_mu.Wait(&message,3000))
    {
        /*  收到应答，针对之前消息的应答 ?  */
        printf("temp_mu.Wait success!\n");
        MessageClusterRunStateModifyAck  ack_message;
        ack_message.deserialize(message.data);
               
        /* 成功收到应答，返回操作结果 */
        ret_code = RpcError(ack_message._err_code);
        if (RpcError(ack_message._err_code) != TECS_SUCCESS)
        {
            oss << ack_message._result;
        }        
        else
        {
            oss << "";
        }
    }
    else
    {
        /*  超时 or 其他错误 ? */
        printf("temp_mu.Wait failed!\n");
        oss <<"Stop Cluster TimeOut";
        ret_code = TECS_ERROR_CM_REQ_TIMEOUT;
    }
    
    /* 返回  */
    xRET2(xINT(ret_code),xSTR(oss.str()));
    return;
}

/*****************************************************************************/
/**
@brief 功能描述: 集群启动提供服务XML

@li @b 入参列表：
@verbatim
paramList    这个是XML传入的请求内容
@endverbatim
      
@li @b 出参列表：
@verbatim
retval      这个是该请求内置执行的结果
@endverbatim
        
@li @b 返回值列表：
@verbatim
无
@endverbatim
          
@li @b 接口限制：无
@li @b 其它说明：无
*/
/*****************************************************************************/
void ClusterPreserveStop::MethodEntry(xmlrpc_c::paramList const& paramList, 
                                  xmlrpc_c::value * const retval)
{
    int64                 uid = -1;
    string              cluster_name;
    ostringstream       oss;
    int                 ret_code = 0;
    
    /* 1. 获取参数 */
    uid = get_userid();
    
    /* 获取授权 */
    AuthRequest ar(uid);
    
    ar.AddAuthorize(
                    AuthRequest::CLUSTER, //授权对象类型,用user做例子
                    0,                    //对象的ID
                    AuthRequest::MANAGE,  //操作类型
                    0,                    //对象拥有者的用户ID
                    false                 //此对象是否为公有
                    );
    
    if (-1 == UserPool::Authorize(ar))
    {   /* 授权失败 */
        oss <<"user can't access!";
        xRET2(xINT(TECS_ERR_AUTHORIZE_FAILED),xSTR(oss.str()));
        return;
    }
    
    /* 获取参数 */
    cluster_name = xmlrpc_c::value_string(paramList.getString(1));
    
    if (StringCheck::CheckNormalName(cluster_name,1,STR_LEN_64) != true)
    {
        oss << "Error, invalide parameter with  " << cluster_name;
        xRET2(xINT(TECS_ERR_PARA_INVALID),xSTR(oss.str()));
        return;       
    }
  
    /* 2. 创建临时消息单元 */
    MessageUnit temp_mu(TempUnitName("Cluster_Manager_Agent"));
    temp_mu.Register();
    
    /* 3. 发消息给 Cluster_Manager  */
    MessageClusterRunStateModifyReq ReqMsg(cluster_name);
    MessageOption option(MID(PROC_TC, MU_CLUSTER_MANAGER), 
                         EV_CLUSTER_ACTION_START_REQ,0,0);
    temp_mu.Send(ReqMsg, option);
    
    /* 4. 等待Cluster_MANAGER回应 */
    MessageFrame message;
    if(SUCCESS == temp_mu.Wait(&message,3000))
    {
        /*  收到应答，针对之前消息的应答 ?  */
        printf("temp_mu.Wait success!\n");
        MessageClusterRunStateModifyAck  ack_message;
        ack_message.deserialize(message.data);
               
        /* 成功收到应答，返回操作结果 */
        ret_code = RpcError(ack_message._err_code);
        if (RpcError(ack_message._err_code) != TECS_SUCCESS)
        {
            oss << ack_message._result;
        }        
        else
        {
            oss << "";
        }
    }
    else
    {
        /*  超时 or 其他错误 ? */
        printf("temp_mu.Wait failed!\n");
        oss <<"Start Cluster TimeOut";
        ret_code = TECS_ERROR_CM_REQ_TIMEOUT;
    }
    
    /* 返回  */
    xRET2(xINT(ret_code),xSTR(oss.str()));
    return;
}

} //end namespace zte_tecs
