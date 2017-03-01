/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�version_manager_methods.cpp
* �ļ���ʶ��
* ����ժҪ���汾����XMl�ӿ��ļ�ʵ��
* ��ǰ�汾��1.0
* ��    �ߣ�zhangyb
* ������ڣ�2012��12��12��
*
* �޸ļ�¼1��
*     �޸����ڣ�2012/12/12
*     �� �� �ţ�V1.0
*     �� �� �ˣ�zhangyb
*     �޸����ݣ�����
******************************************************************************/
#include "workflow_methods.h"
#include "tecs_errcode.h"
#include "authrequest.h"
#include "workflow_api.h"
#include "log_agent.h"

static int workflow_print = 0;  

DEFINE_DEBUG_VAR(workflow_print);
#define Debug(fmt,arg...) \
        do \
        { \
            if(workflow_print) \
            { \
                printf("[%s:%d]"fmt,__func__,__LINE__, ##arg); \
            } \
        }while(0)


namespace zte_tecs
{
void WorkflowQuery::MethodEntry(xmlrpc_c::paramList const& paramList,
                                  xmlrpc_c::value * const retval)
{
    int                         ret;
    vector<xmlrpc_c::value>     array_data;
    xmlrpc_c::value_array       *array_result = NULL;
    /*��Ե����⻧��Ȩ��ʱ�޷������������⻧�ſ�Ȩ��*/
    ApiWorkflowQueryReq  api_req;
    api_req.from_rpc(paramList.getStruct(1));
    if (api_req.workflows.size() > 10 || api_req.workflows.size() <= 0)
    {
        xRET2(xINT(TECS_ERR_PARA_INVALID),xSTR("parameter is invalid,numbers is over 10"));
        return;
    }
    
    if (api_req.workflows.empty())
    {
        xRET2(xINT(TECS_ERR_PARA_INVALID),xSTR("parameter is invalid,the numbers is 0"));
        return;
    }

    /* 2. ������ʱ��Ϣ��Ԫ */
    MessageUnit mu(TempUnitName("Workflow_Temp"));
    mu.Register();

    TIMER_ID timer_id = mu.CreateTimer();
    if(INVALID_TIMER_ID == timer_id)
    {
        xRET2(xINT(TECS_ERROR),xSTR("failed to create timer"));
        return;
    }
    
    uint32 wait_limit = 0;
    vector<ApiTargetWorkflow>::iterator it = api_req.workflows.begin();
    for(; it != api_req.workflows.end(); it ++)
    {
        /* 3. ����Ϣ��  �������� */
        if (it->engine.empty() || it->id.empty())
        {
            continue;
        }
        MID mid;
        mid.deserialize(it->engine);
        MessageOption option(mid, EV_QUERY_WORKFLOW,0,0);
        // //���¶�����ϢID�Ŵ�100��ʼzyb
        mu.Send(it->id, option);
        wait_limit++;
        Debug("target:%s workflowid:%s\n",
            it->engine.c_str(),
            it->id.c_str());
    }
    
    if (0 == wait_limit)
    {
        xRET2(xINT(TECS_ERR_PARA_INVALID),xSTR("the number that need to query is null"));
        return;
    }

    #define EV_QUERY_TIMEOUT EV_TIMER_1
    TimeOut timeout(EV_QUERY_TIMEOUT,5000);
    ret = mu.SetTimer(timer_id,timeout);
    ostringstream oss;
    if(SUCCESS != ret)
    {
        xRET2(xINT(TECS_ERROR),xSTR("fail to set timer"));
        return;
    }

    //��ʼ����ȴ��������̵�Ӧ��
    uint32 wait_count = 0; 
    MessageFrame ack;
    ApiWorkflowQueryAck api_ack;
    while(1)
    {
        if(wait_count >= wait_limit)
        {
            //���н��̶��Ѿ��ȴ����ˣ����㶨ʱ��û����Ҳ����
            //����Ķ�ʱ����Դ��������ʱ��Ϣ��Ԫ�������Զ��ͷ�
            //cout << "=== All ack received! ===" << endl;
            Debug("=== All ack received! ===\n");
            break;
        }

        //��Ϊ�Ѿ��������ռ���ʱ�����������һֱ����ȥ�����õ����޷�����        
        ret = mu.Wait(&ack,WAIT_FOREVER);
        if(SUCCESS != ret)
        {
            //�ȴ�����ʧ��
            cerr << "Wait failed! ret = " << ret << endl;
            oss << "Wait failed! ret = " << ret << endl;
            break;
        }

        if(ack.option.id() == EV_QUERY_TIMEOUT)
        {
            //�ռ���ʱ����ʱ��, �������ﳬʱ��������ʧ�� 
            cerr << "Time out! " << wait_count << " ack received!" << endl;
            oss << "Time out! " << wait_count << " ack received!" << endl;
            break;
        }

        if(ack.option.id() != EV_QUERY_WORKFLOW_ACK)
        {
            //�յ��Ĳ�����������Ҫ�ȵ���Ϣ
            cerr << "Unknown message! id = " << ack.option.id() << endl;
            continue;
        }

        //�յ�һ��Ӧ����
        WorkflowAck work_ack;
        Debug("work ack msg:%s\n",ack.data.c_str());
            work_ack.deserialize(ack.data);
        Debug("rcv app:%s process:%s unit:%s workflowid:%s state:%d progress:%d\n",
            ack.option.sender()._application.c_str(),
            ack.option.sender()._process.c_str(),
            ack.option.sender()._unit.c_str(),
            work_ack.workflow_id.c_str(),
            work_ack.state,
            work_ack.progress);
        vector<ApiTargetWorkflow>::iterator wit = api_req.workflows.begin();
        for(; wit != api_req.workflows.end(); wit ++)
        {
            MID mid;
            mid.deserialize(wit->engine);
            if(wit->id == work_ack.workflow_id &&
                mid._application == ack.option.sender()._application &&
                mid._process == ack.option.sender()._process)
            {
                ApiWorkflowState api_state(*wit);
                
                ToApiState(work_ack.state,api_state.state);
                api_state.progress = work_ack.progress;
                api_ack.workflow_states.push_back(api_state);
                wait_count++;
                api_req.workflows.erase(wit);
                break;
            }
        }        
    }

    mu.KillTimer(timer_id);

    if (0 == wait_count)
    {
        xRET2(xINT(TECS_ERROR),xSTR("fail to get workflow state:"+oss.str()));
        return;
    }

    /* ���ݷ��ؽ�����ظ���̨ */
    array_data.push_back(xmlrpc_c::value_int(RpcError(TECS_SUCCESS)));
    array_data.push_back(xmlrpc_c::value_struct(api_ack.to_rpc()));


    /* ����  */
    array_result = new xmlrpc_c::value_array(array_data);
    *retval = *array_result;

    delete array_result;
    return;
}

} //end namespace zte_tecs


