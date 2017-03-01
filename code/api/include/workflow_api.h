/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�version_api.h
* �ļ���ʶ��
* ����ժҪ���汾����Ķ���ӿ�
* ��ǰ�汾��1.0
* ��    �ߣ�zhangyb
* ������ڣ�2012��12��19��
*
* �޸ļ�¼1��
*     �޸����ڣ�2012/12/19
*     �� �� �ţ�V1.0
*     �� �� �ˣ�zhangyb
*     �޸����ݣ�����
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


