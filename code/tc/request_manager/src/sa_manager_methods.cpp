/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�host_manager_methods.cpp
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ���������ģ���漰��XML-RPC method���ʵ���ļ�
* ��ǰ�汾��1.0
* ��    �ߣ��뿥
* ������ڣ�2012��11��20��
*
* �޸ļ�¼1��
*     �޸����ڣ�2012/11/20
*     �� �� �ţ�V1.0
*     �� �� �ˣ��뿥
*     �޸����ݣ�����
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

    /* 1. ������Ȩ */
    AUTHORIZE(AuthRequest::SA, AuthRequest::MANAGE);

    /* 2. ��ȡ���� */    
    MessageSaEnableReq req;
    req._sid = paramList.getInt(1);   
    req._enable=false; 
    /* 3. ������ */
  
    /* 4. ��Ϣת������Ӧ�� */
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

    /* 5. XML-RPC����  */
    XML_RPC_RETURN();
}

/******************************************************************************/
void SaEnableMethod::MethodEntry(const paramList &paramList,
                                   value * const retval)
{
    vector<value>     array_data;
    value_array       *array_result = NULL;

    /* 1. ������Ȩ */
    AUTHORIZE(AuthRequest::SA, AuthRequest::MANAGE);

    /* 2. ��ȡ���� */    
    MessageSaEnableReq req;
    req._sid = paramList.getInt(1);   
    req._enable=true; 
    /* 3. ������ */
  
    /* 4. ��Ϣת������Ӧ�� */
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

    /* 5. XML-RPC����  */
    XML_RPC_RETURN();
}

void SaForgetIdMethod::MethodEntry(const paramList &paramList,
                                   value * const retval)
{
    vector<value>     array_data;
    value_array       *array_result = NULL;

    /* 1. ������Ȩ */
    AUTHORIZE(AuthRequest::SA, AuthRequest::DELETE);

    /* 2. ��ȡ���� */      
    MessageSaForgetIdReq req;
    req._sid = paramList.getInt(1);   
    
    /* 3. ������ */
  
    /* 4. ��Ϣת������Ӧ�� */
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

    /* 5. XML-RPC����  */
    XML_RPC_RETURN();
}


void SaMapClusterMethod::MethodEntry(const paramList &paramList,
                                   value * const retval)
{
    vector<value>     array_data;
    value_array       *array_result = NULL;

    /* 1. ������Ȩ */
    AUTHORIZE(AuthRequest::SA, AuthRequest::CREATE);

    /* 2. ��ȡ���� */       
    MessageSaClusterMapReq req;
    req._sid = paramList.getInt(1);   
    req._cluster_name= paramList.getString(2);
    req._is_add= paramList.getBoolean(3);
    /* 3. ������ */
    if (!StringCheck::CheckNormalName(req._cluster_name, 1, STR_LEN_64))
    {
        ACK_PARA_INVALID("cluster_name");
        XML_RPC_RETURN();
    }
    /* 4. ��Ϣת������Ӧ�� */
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

    /* 5. XML-RPC����  */
    XML_RPC_RETURN();
}

}

