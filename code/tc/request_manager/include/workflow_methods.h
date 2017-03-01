/******************************************************************************
* Copyright (c) 2014，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：workflow_methods.h
* 文件标识：
* 内容摘要：工作流XMl配置接口申明
* 当前版本：1.0
* 作    者：zhangyb
* 完成日期：2014年02月20日
*
* 修改记录1：
*     修改日期：2014/02/20
*     版 本 号：V1.0
*     修 改 人：zhangyb
*     修改内容：创建
******************************************************************************/
#ifndef TC_WORKFLOW_METHODS_H
#define TC_WORKFLOW_METHODS_H

#include "sky.h"
#include "request_manager.h"
#include "tecs_rpc_method.h"
#include "user_pool.h"
#include "mid.h"
#include "event.h"
#include "tecs_errcode.h"
#include "workflow.h"

namespace zte_tecs
{


class WorkflowQuery : public TecsRpcMethod
{
public:
    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP(ERROR,TECS_ERROR)
        ERR_MAP(ERROR_TIME_OUT,TECS_ERR_TIMEOUT)
        ERR_MAP(ERROR_INVALID_ARGUMENT,TECS_ERR_PARA_INVALID)
        ERR_MAP_END
    }
    
    WorkflowQuery (UserPool *upool=0)
    {
        _method_name = "WorkflowQueryMethod";
        _user_id = -1;
        _upool = upool;
    };

    ~WorkflowQuery (){};

/*****************************************************************************/
/**
@brief 功能描述:版本查询XML方法接口

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
    void MethodEntry( xmlrpc_c::paramList const& paramList,
                      xmlrpc_c::value    *const  retval);

    void ToApiState(int state,int &api_state)
    {
        #define API_WORKFLOW_SUCCESS    0
        #define API_WORKFLOW_FAILED     1
        #define API_WORKFLOW_RUNNING    2
        #define API_WORKFLOW_CANCELED   3
        #define API_WORKFLOW_UNKNOWN    4
        
        api_state = API_WORKFLOW_UNKNOWN;
        if (WorkflowAck::WORKFLOW_SUCCESS == state)
            api_state = API_WORKFLOW_SUCCESS;        
        else if (WorkflowAck::WORKFLOW_FAILED == state)
            api_state = API_WORKFLOW_FAILED;        
        else if (WorkflowAck::WORKFLOW_RUNNING == state)
            api_state = API_WORKFLOW_RUNNING;        
        else if (WorkflowAck::WORKFLOW_CANCELED == state)
            api_state = API_WORKFLOW_CANCELED;        
        else if (WorkflowAck::WORKFLOW_UNKNOWN == state)        
            api_state = API_WORKFLOW_UNKNOWN;            
        
    }

private:

};



} //end namespace zte_tecs

#endif //end TC_WORKFLOW_METHODS_H

