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
    /*针对单个租户授权暂时无法处理，对所有租户放开权限*/
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

    /* 2. 创建临时消息单元 */
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
        /* 3. 发消息给  各个进程 */
        if (it->engine.empty() || it->id.empty())
        {
            continue;
        }
        MID mid;
        mid.deserialize(it->engine);
        MessageOption option(mid, EV_QUERY_WORKFLOW,0,0);
        // //重新定义消息ID号从100开始zyb
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

    //开始逐个等待各个进程的应答
    uint32 wait_count = 0; 
    MessageFrame ack;
    ApiWorkflowQueryAck api_ack;
    while(1)
    {
        if(wait_count >= wait_limit)
        {
            //所有进程都已经等待过了，即便定时器没到，也返回
            //申请的定时器资源会随着临时消息单元的析构自动释放
            //cout << "=== All ack received! ===" << endl;
            Debug("=== All ack received! ===\n");
            break;
        }

        //因为已经设置了终极定时器，这里可以一直等下去，不用担心无法结束        
        ret = mu.Wait(&ack,WAIT_FOREVER);
        if(SUCCESS != ret)
        {
            //等待操作失败
            cerr << "Wait failed! ret = " << ret << endl;
            oss << "Wait failed! ret = " << ret << endl;
            break;
        }

        if(ack.option.id() == EV_QUERY_TIMEOUT)
        {
            //终极定时器超时了, 但是这里超时并不返回失败 
            cerr << "Time out! " << wait_count << " ack received!" << endl;
            oss << "Time out! " << wait_count << " ack received!" << endl;
            break;
        }

        if(ack.option.id() != EV_QUERY_WORKFLOW_ACK)
        {
            //收到的不是我们真正要等的消息
            cerr << "Unknown message! id = " << ack.option.id() << endl;
            continue;
        }

        //收到一条应答了
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

    /* 根据返回结果返回给后台 */
    array_data.push_back(xmlrpc_c::value_int(RpcError(TECS_SUCCESS)));
    array_data.push_back(xmlrpc_c::value_struct(api_ack.to_rpc()));


    /* 返回  */
    array_result = new xmlrpc_c::value_array(array_data);
    *retval = *array_result;

    delete array_result;
    return;
}

} //end namespace zte_tecs


