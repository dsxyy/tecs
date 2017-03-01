/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：file_manager_methods.cpp
* 文件标识：
* 内容摘要：文件管理模块XML-RPC方法类的实现文件
* 当前版本：1.0 
* 作    者       ：lixch
* 完成日期：2013年2月7日
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

    // 判断是否是管理员，不是管理员，则返回失败
    AUTHORIZE(AuthRequest::FILE, AuthRequest::INFO)

    /* 1. 创建临时消息单元 */
    MessageUnit temp_mu(TempUnitName("tecs_file_get_coredump_url"));
    WorkReq req_info(GenerateUUID());
    temp_mu.Register();

    
    MID recv(PROC_FILE_MANAGER, MU_FILE_MANAGER);
    MessageOption option(recv, EV_FILE_GET_URL_REQ,0,0);
    temp_mu.Send(req_info, option);

    /* 3. 等待PROC_FILE_MANAGER回应 */
    MessageFrame message;
    int          ret_code = 0;
    if(SUCCESS == temp_mu.Wait(&message,k_file_wait_time))
    {
        /*  收到应答，针对之前消息的应答 ?  */
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


