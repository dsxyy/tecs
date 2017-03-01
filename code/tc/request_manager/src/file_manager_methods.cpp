/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�file_manager_methods.cpp
* �ļ���ʶ��
* ����ժҪ���ļ�����ģ��XML-RPC�������ʵ���ļ�
* ��ǰ�汾��1.0 
* ��    ��       ��lixch
* ������ڣ�2013��2��7��
*
*******************************************************************************/
#include "file_manager_methods.h"
#include "mid.h"
#include "event.h"
#include "sky.h"
#include "file_api.h"
#include "authrequest.h"


static int file_print = 1;  
DEFINE_DEBUG_VAR(file_print);
#define Dbg_Prn(fmt,arg...) \
        do \
        { \
            if(file_print) \
            { \
                printf("[%s:%d] "fmt,__func__,__LINE__, ##arg); \
            } \
        }while(0)

using namespace xmlrpc_c;


            
namespace zte_tecs{

#define AUTHORIZE(ob, op) \
    AuthRequest ar(get_userid()); \
    ar.AddAuthorize((ob), 0, (op), 0, false); \
    if (-1 == UserPool::Authorize(ar)) \
    { \
        xRET2(xINT(TECS_ERR_AUTHENTICATE_FAILED),xSTR("authenticate  failed"));\
        return;\
    }

#define   ERR_TIME_OUT   2
const  int k_file_wait_time = 3000;

/******************************************************************************/
void FileGetCoredumpUrlMethod :: MethodEntry(
                        const paramList& paramList,
                        value* const retval)
{
    ostringstream       oss;

    // �ж��Ƿ��ǹ���Ա�����ǹ���Ա���򷵻�ʧ��
    AUTHORIZE(AuthRequest::FILE, AuthRequest::INFO)

    /* 1. ������ʱ��Ϣ��Ԫ */
    MessageUnit temp_mu(TempUnitName("tecs_file_get_coredump_url"));
    WorkReq req_info(GenerateUUID());
    temp_mu.Register();

    
    MID recv(PROC_FILE_MANAGER, MU_FILE_MANAGER);
    MessageOption option(recv, EV_FILE_GET_URL_REQ,0,0);
    temp_mu.Send(req_info, option);

    /* 3. �ȴ�PROC_FILE_MANAGER��Ӧ */
    MessageFrame message;
    int          ret_code = 0;
    if(SUCCESS == temp_mu.Wait(&message,k_file_wait_time))
    {
        /*  �յ�Ӧ�����֮ǰ��Ϣ��Ӧ�� ?  */
        Dbg_Prn("%s : rcv ack msg success!\n", _method_name.c_str());
        ApiCoredumpUrlInfo Ack;
        UploadUrl recv_msg;
        recv_msg.deserialize(message.data);
        
        Ack.url = recv_msg.ip;
        Ack.path = recv_msg.path;
        Ack.access_type = recv_msg.access_type;
        xRET2(xINT(TECS_SUCCESS), Ack.to_rpc());
    }
    else
    {
        ret_code = TECS_ERR_TIMEOUT;
        oss <<_method_name <<" time out";
        xRET2(xINT(ret_code),xSTR(oss.str()));
    }
}

}


