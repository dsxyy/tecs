/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：version_api.h
* 文件标识：
* 内容摘要：版本管理的对外接口
* 当前版本：1.0
* 作    者：zhangyb
* 完成日期：2012年12月19日
*
* 修改记录1：
*     修改日期：2012/12/19
*     版 本 号：V1.0
*     修 改 人：zhangyb
*     修改内容：创建
******************************************************************************/

#ifndef TECS_WORKFLOW_API_H
#define TECS_WORKFLOW_API_H
#include "rpcable.h"
#include "api_error.h"

class ApiTargetWorkflow: public Rpcable
{
public:
    ApiTargetWorkflow()
    {
    };

    string engine;
    string id;

    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_VALUE(engine);
        TO_VALUE(id);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN(); 
        FROM_VALUE(engine);
        FROM_VALUE(id);
        FROM_RPC_END();
    };
};

class ApiWorkflowState: public Rpcable
{
public:
    ApiWorkflowState()
    {
    };

    ApiWorkflowState(ApiTargetWorkflow &worflow)
    {
        engine = worflow.engine;
        id = worflow.id;
        state = ERROR;
        progress = 0;
    };

    string engine;//zyb
    string id;
    int    state;
    int    progress;

    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_VALUE(engine);
        TO_VALUE(id);
        TO_VALUE(state);
        TO_VALUE(progress);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN(); 
        FROM_VALUE(engine);
        FROM_VALUE(id);
        FROM_VALUE(state);
        FROM_VALUE(progress);
        FROM_RPC_END();
    };
};

class ApiWorkflowQueryReq: public Rpcable
{
public:
    ApiWorkflowQueryReq() {};
    ~ApiWorkflowQueryReq() {};

public:
    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_STRUCT_ARRAY(workflows);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN(); 
        FROM_STRUCT_ARRAY(workflows);
        FROM_RPC_END();
    };


public:
    vector<ApiTargetWorkflow>workflows;
};

class ApiWorkflowQueryAck: public Rpcable
{
public:
    ApiWorkflowQueryAck() {};
    ~ApiWorkflowQueryAck() {};

public:
    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_STRUCT_ARRAY(workflow_states);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_STRUCT_ARRAY(workflow_states);
        FROM_RPC_END();
};


public:
    vector<ApiWorkflowState> workflow_states;
};


#endif


