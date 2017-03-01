/******************************************************************************
* Copyright (c) 2014������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�workflow_methods.h
* �ļ���ʶ��
* ����ժҪ��������XMl���ýӿ�����
* ��ǰ�汾��1.0
* ��    �ߣ�zhangyb
* ������ڣ�2014��02��20��
*
* �޸ļ�¼1��
*     �޸����ڣ�2014/02/20
*     �� �� �ţ�V1.0
*     �� �� �ˣ�zhangyb
*     �޸����ݣ�����
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
@brief ��������:�汾��ѯXML�����ӿ�

@li @b ����б�
@verbatim
paramList    �����XML�������������
@endverbatim

@li @b �����б�
@verbatim
retval      ����Ǹ���������ִ�еĽ��
@endverbatim

@li @b ����ֵ�б�
@verbatim
��
@endverbatim

@li @b �ӿ����ƣ���
@li @b ����˵������
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

