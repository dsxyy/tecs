/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：image_api_method.cpp
* 文件标识：
* 内容摘要：Image模块XML-RPC注册方法类的实现文件
* 当前版本：1.0 
* 作    者：Bob
* 完成日期：2011年9月5日
*
* 修改记录1：
*    修改日期：2011/9/5
*    版 本 号：V1.0
*    修 改 人：Bob
*    修改内容：创建
*******************************************************************************/
#include "image_manager_methods.h"
#include "image_manager_with_api_method.h"
#include "mid.h"
#include "event.h"
#include "image.h"
#include "sky.h"
#include "tecs_pub.h"
#include "license_common.h"
#include "api_pub.h"
#include "user_pool.h"
#include "authrequest.h"
#include "authmanager.h"

static int imm_print = 1;  
DEFINE_DEBUG_VAR(imm_print);
#define Dbg_Prn(fmt,arg...) \
        do \
        { \
            if(imm_print) \
            { \
                printf("[%s:%d] "fmt,__func__,__LINE__, ##arg); \
            } \
        }while(0)

using namespace xmlrpc_c;
            
namespace zte_tecs{

#define   ERR_TIME_OUT   2
const  int k_image_wait_time = 3000;

/******************************************************************************/
void ImageRegisterMethod :: MethodEntry(
                        const paramList& paramList,
                        value* const retval)
{
    ostringstream       oss;
    string           error_str;
    int          ret_code = 0;
    
    /* 1. 获取参数 */
    int64 uid = get_userid();
  
    ApiImageReg  regiseter_data;
    
    regiseter_data.from_rpc(paramList.getStruct(1));    

    bool  success = regiseter_data.Validate(error_str);
    if (!success)//非法参数
    {
        ret_code = ERROR;
        xRET2(xINT(RpcError(ret_code)),xSTR(error_str));
        return ;
    }

    if(ValidateLicense(LICOBJ_IMAGES, 1) != 0)
    {
        ret_code = TECS_ERR_LICENSE_INVALID;
        xRET2(xINT(RpcError(ret_code)),xSTR(error_str));
        return;
    }          
    
    if (StringCheck::CheckComplexName(regiseter_data.name,1,STR_LEN_256) != true)
    {
        oss << "Error, invalide parameter with  " << regiseter_data.name;
        xRET2(xINT(TECS_ERR_PARA_INVALID),xSTR(oss.str()));
        return;       
    }

    if (StringCheck::CheckSize(regiseter_data.description,0,STR_LEN_512) != true)
    {
        oss << "Error, invalide parameter with  " << regiseter_data.description;
        xRET2(xINT(TECS_ERR_PARA_INVALID),xSTR(oss.str()));
        return;       
    }
    
    /* 2. 创建临时消息单元 */
    MessageUnit temp_mu(TempUnitName("tecs_image_register"));
    temp_mu.Register();

    /* 3. 发消息给 IMAGE_MANAGE  */

    ImageRegisterMsg request_message(uid, 
                                     regiseter_data.id,
                                     regiseter_data.name,
                                     regiseter_data.type,
                                     regiseter_data.arch,
                                     regiseter_data.is_public ,
                                     regiseter_data.is_available);

    request_message._description = regiseter_data.description;

    MessageOption option(MID(PROC_IMAGE_MANAGER, MU_IMAGE_MANAGER), 
                         EV_IMAGE_REGISTER_REQ,0,0);
    temp_mu.Send(request_message, option);

    /* 4. 等待MU_IMAGE_MANAGER回应 */
    MessageFrame message;
     if(SUCCESS == temp_mu.Wait(&message,k_image_wait_time))
    {
        /*  收到应答，针对之前消息的应答 ?  */
        Dbg_Prn("%s : rcv ack msg success!\n", _method_name.c_str());
        ImageOperateAckMsg  ack_message;
        ack_message.deserialize(message.data);
        
        ack_message.Print();

        /* 成功收到应答，返回操作结果 */
        ret_code = RpcError(ack_message._ret_code);
        oss << ack_message._result;
    }
    else
    {
        /*  超时 or 其他错误 ? */
        Dbg_Prn("%s : time out\n",_method_name.c_str());
        oss <<_method_name <<" time out";
        ret_code = RpcError(ERROR_TIME_OUT);
    }

    /* 返回  */
    xRET2(xINT(ret_code),xSTR(oss.str()));
    return;    
}


/******************************************************************************/
void ImageDeregisterMethod :: MethodEntry(
                        const paramList& paramList,
                        value* const retval)
{
    int64                 uid = -1;
    int64                 image_id = -1;
    string              image_cfg;
    ostringstream       oss;
    int                 ret_code = 0;
	
    /* 1. 获取参数 */
    uid = get_userid();
    image_id = value_i8(paramList.getI8(1));

    /* 2. 创建临时消息单元 */
    MessageUnit temp_mu(TempUnitName("tecs_image_ungister"));
    temp_mu.Register();

    /* 3. 发消息给 IMAGE_MANAGE  */
    ImageDeregisterMsg request_message(uid,  image_id);
    MessageOption option(MID(PROC_IMAGE_MANAGER, MU_IMAGE_MANAGER),
                         EV_IMAGE_DEREGISTER_REQ,0,0);
    temp_mu.Send(request_message, option);

    /* 4. 等待MU_IMAGE_MANAGER回应 */
    MessageFrame message;
    if(SUCCESS == temp_mu.Wait(&message,k_image_wait_time))
    {
        /*  收到应答，针对之前消息的应答 ?  */
        Dbg_Prn("%s : rcv ack msg success!\n", _method_name.c_str());
        ImageOperateAckMsg  ack_message;
        ack_message.deserialize(message.data);
        
        ack_message.Print();
        
        /* 成功收到应答，返回操作结果 */
        ret_code = RpcError(ack_message._ret_code);
        oss << ack_message._result;
    }
    else
    {
        /*  超时 or 其他错误 ? */
        Dbg_Prn("%s : time out\n",_method_name.c_str());
        oss <<_method_name <<" time out";
        ret_code = RpcError(ERROR_TIME_OUT);
    }

    /* 返回  */
    xRET2(xINT(ret_code),xSTR(oss.str()));
    return;    
}

void ImageDeleteMethod :: MethodEntry(
                        const paramList& paramList,
                        value* const retval)
{
    int64                 uid = -1;
    int64                 image_id = -1;
    string              image_cfg;
    ostringstream       oss;
    
    /* 1. 获取参数 */
    uid = get_userid();
    image_id = value_i8(paramList.getI8(1));

    /* 2. 创建临时消息单元 */
    MessageUnit temp_mu(TempUnitName("tecs_image_delete"));
    temp_mu.Register();

    /* 3. 发消息给 IMAGE_MANAGE  */
    ImageDeleteMsg request_message(uid,  image_id);
    MessageOption option(MID(PROC_IMAGE_MANAGER, MU_IMAGE_MANAGER),
                         EV_IMAGE_DELETE_REQ,0,0);
    temp_mu.Send(request_message, option);

    /* 4. 等待MU_IMAGE_MANAGER回应 */
    MessageFrame message;
    if(SUCCESS == temp_mu.Wait(&message,k_image_wait_time))
    {
        /*  收到应答，针对之前消息的应答 ?  */
        Dbg_Prn("%s : rcv ack msg success!\n", _method_name.c_str());
        ImageOperateAckMsg  ack_message;
        ack_message.deserialize(message.data);
        ack_message.Print();
        
        /* 成功收到应答，返回操作结果 */     
        xRET2(xINT(RpcError(ack_message._ret_code)),xSTR(ack_message._result));
        return;
    }
    else
    {
        /*  超时 or 其他错误 ? */
        oss <<_method_name <<" time out. image_id is "<< image_id <<".";
        Dbg_Prn("%s\n",oss.str().c_str());
        xRET2(xINT(RpcError(ERROR_TIME_OUT)),xSTR(oss.str()));
        return;
    }   
}

/******************************************************************************/
void ImageModifyMethod :: MethodEntry(
                        const paramList& paramList,
                        value* const retval)
{
    ostringstream       oss;
    ImageModifyMsg request_message;
    int          ret_code = 0;
    
    /* 1. 获取参数 */
    request_message._uid   = get_userid();

    request_message._image_id  = value_i8(paramList.getI8(1));    
    request_message._name = value_string(paramList.getString(2));
    request_message._type = value_string(paramList.getString(3));
    request_message._arch = value_string(paramList.getString(4));
    request_message._bus  = value_string(paramList.getString(5));
    request_message._description = value_string(paramList.getString(6));
    request_message._os_type= value_string(paramList.getString(7));
    request_message._container_format = value_string(paramList.getString(8));
    /*  发消息给 IMAGE_MANAGE 的选项  */
    MessageOption option(MID(PROC_IMAGE_MANAGER, MU_IMAGE_MANAGER), 
                         EV_IMAGE_MODIFY_REQ,0,0);

    /*  等待MU_IMAGE_MANAGER回应 结构 */
    MessageFrame message;
    
    /* 2. 创建临时消息单元 */
    MessageUnit temp_mu(TempUnitName("tecs_image_set"));
    if(SUCCESS != temp_mu.Register())
    {
        oss << "Error, failed to register mu!";
        goto para_err_out;
    }

    // 映像类型
    if (request_message._type.size() != 0)
    {
        if (! ImageParaCheck::Type(request_message._type))
        {
            oss << "Invalid image type (" <<request_message._type << ")";
            goto para_err_out;
        }
    }

    // 映像体系结构
    if (request_message._arch.size() != 0)
    {
        if(!ImageParaCheck::Arch(request_message._arch))
        {
            oss << "Invalid image arch (" <<request_message._type << ")";
            goto para_err_out;
        }
    }

    // 名称长度合法性校验
    if (request_message._name.size() != 0)
    {
        if (StringCheck::CheckComplexName(request_message._name,1,STR_LEN_256) != true)
        {
            oss << "Error, invalid parameter with " << request_message._name;
            goto para_err_out;
        }     
    }

    //总线类型合法性校验
    if (ImageParaCheck::Bus(request_message._bus) != true)
    {
        oss << "Error, invalid bus " << request_message._bus;
        goto para_err_out;
    }     

    if (StringCheck::CheckSize(request_message._description,0,STR_LEN_512) != true)
    {
        oss << "Error, invalid parameter with " << request_message._description;
        goto para_err_out;
    }
    

    // 参数校验通过，开始发送消息给执行体
    temp_mu.Send(request_message, option);

    
    if(SUCCESS == temp_mu.Wait(&message,k_image_wait_time))
    {
        /*  收到应答，针对之前消息的应答 ?  */
        Dbg_Prn("%s : rcv ack msg success!\n", _method_name.c_str());
        ImageOperateAckMsg  ack_message;
        ack_message.deserialize(message.data);
        
        ack_message.Print();
        
        /* 成功收到应答，返回操作结果 */
        ret_code = RpcError(ack_message._ret_code);
        oss << ack_message._result;
    }
    else
    {
        /*  超时 or 其他错误 ? */
        Dbg_Prn("%s : time out\n",_method_name.c_str());
        oss <<_method_name <<" time out";
        ret_code = RpcError(ERROR_TIME_OUT);
    }

    /* 返回  */
    xRET2(xINT(ret_code),xSTR(oss.str()));
    return;   

para_err_out:
    xRET2(xINT(TECS_ERR_PARA_INVALID),xSTR(oss.str()));
    return;       
    
}

/******************************************************************************/
void ImageQueryMethod :: MethodEntry(
                        const paramList& paramList,
                        value* const retval)
{
    ostringstream       oss;
    int64               start_index =0;
    int64               query_count = 0;
    int64               query_user_id;
    int64               query_image_id = -1;
    string              query_filename;
    int                 ret_code = 0;
    
    /* 1. 获取参数 */
    int64  uid             = get_userid();
    start_index            = xmlrpc_c::value_i8(paramList.getI8(1));
    query_count            = xmlrpc_c::value_i8(paramList.getI8(2));
    int  query_type        = value_int(paramList.getInt(3));
    string query_user_name = value_string(paramList.getString(4));
    query_image_id         = value_i8(paramList.getI8(5));
    query_filename         = value_string(paramList.getString(6));

    /* 租户不填写id,当租户请求时，该查询id等于租户自己 */
    query_user_id = uid;
    if (query_type == ImageQueryMsg::APPOINTED)
    {
        if (query_user_name.size() == 0)
        {
            query_user_id = uid;
        }
        else
        {
            if (!_upool->GetUserIDByName(query_user_name, query_user_id))
            {
                oss << "user:" << query_user_name<<"does not exist!";
                xRET2(xINT(TECS_ERR_OBJECT_NOT_EXIST),xSTR(oss.str()));
                return;       
            }
        }
    }

    /* 单包数量限值在200条记录 */
    if ((uint64)query_count > 200 || ((uint64)query_count == 0))
    {
        oss <<"query count too large,range 1-200";
        xRET2(xINT(TECS_ERR_PARA_INVALID),xSTR(oss.str()));
        return;
    }
    
    /* 2. 创建临时消息单元 */
    MessageUnit temp_mu(TempUnitName("tecs_image_query"));
    temp_mu.Register();

    ImageQueryMsg request_message(uid, query_type);
    request_message._start_index = start_index;
    request_message._query_count = query_count;
    /* 3. 发消息给 IMAGE_MANAGE  */
    request_message._query_uid   = query_user_id;
    request_message._query_image_id = query_image_id;
    request_message._query_filename = query_filename;
    
    MessageOption option(MID(PROC_IMAGE_MANAGER, MU_IMAGE_MANAGER), 
                         EV_IMAGE_QUERY_REQ,0,0);
    temp_mu.Send(request_message, option);

    /* 4. 等待MU_IMAGE_MANAGER回应 */
    MessageFrame message;
    if(SUCCESS == temp_mu.Wait(&message,k_image_wait_time))
    {
        /*  收到应答，针对之前消息的应答 ?  */
        Dbg_Prn("%s : rcv ack msg success!\n", _method_name.c_str());
        ImageQueryAckMsg  ack_message;
        ack_message.deserialize(message.data);
        
    
        /* 成功收到应答，返回操作结果 */
        ret_code = RpcError(ack_message._ret_code);
        if (RpcError(ack_message._ret_code) != TECS_SUCCESS)
        {
            oss << ack_message._err_str;
            goto query_out;
        }         
        
        vector<value> array_image;
        vector<ImageInfo>::const_iterator iter;
           
        for (iter = ack_message._image_info.begin();
                iter != ack_message._image_info.end();
                iter++)
        {
              ApiImageInfo  info;
              
              info.id      = iter->_image_id;
              _upool->GetUserNameByID(iter->_owner, info.user_name);
              info.name    = iter->_name;
              info.file    = iter->_file;
              info.md5sum  = iter->_md5sum;
              info.type    = iter->_type;
              info.is_public   = iter->_is_public;
              info.is_enabled     = iter->_is_enabled;
              info.register_time = iter->_register_time;
              info.size            = iter->_size;
              info.running_vms     = iter->_running_vms;
              info.ref_count_by_private_vt = iter->_ref_count_by_private_vt;
              info.ref_count_by_public_vt = iter->_ref_count_by_public_vt;
              info.arch            = iter->_os_arch;
              info.bus             = iter->_bus;
              info.description     = iter->_description;
              info.os_type         = iter->_os_type;
              info.disk_size       = iter->_disk_size;
              info.disk_format     = iter->_disk_format;
              info.container_format = iter->_container_format;
              array_image.push_back(info.to_rpc());
        }
        xRET4(xINT(ret_code), xI8(ack_message._next_index), xI8(ack_message._max_count), xARRAY(array_image));
        return;    
    }    
    else
    {
        /*  超时 or 其他错误 ? */
        Dbg_Prn("%s : time out\n",_method_name.c_str());
        oss <<_method_name <<" time out";
        xRET2(xINT(RpcError(ERROR_TIME_OUT)),xSTR(oss.str()));
        return;    
    }

    /* 返回  */
query_out:
    xRET2(xINT(ret_code),xSTR(oss.str()));
    return;    
}


/******************************************************************************/
void ImageFtpSvrInfoGetMethod :: MethodEntry(
                        const paramList& paramList,
                        value* const retval)
{
    vector<value>  array_data;
    value_array    *array_result = NULL;
    ostringstream  oss;
    
    /* 1. 获取参数 */
    int64  uid = get_userid();
    
    /* 2. 创建临时消息单元 */
    MessageUnit temp_mu(TempUnitName("tecs_image_FtpSvrInfoGet"));
    temp_mu.Register();

    ImageFtpSvrInfoGetMsg request_message(uid);

    /* 3. 发消息给 IMAGE_MANAGE  */    
    MessageOption option(MID(PROC_IMAGE_MANAGER, MU_IMAGE_MANAGER), 
                         EV_IMAGE_FTP_SVR_INFO_REQ,0,0);

    temp_mu.Send(request_message, option);

    /* 4. 等待MU_IMAGE_MANAGER回应 */
    MessageFrame message;
    if(SUCCESS == temp_mu.Wait(&message,k_image_wait_time))
    {
        /*  收到应答，针对之前消息的应答 ?  */
        Dbg_Prn("%s : rcv ack msg success!\n", _method_name.c_str());
        ImageFtpSvrInfoAckMsg  ack_message;
        ack_message.deserialize(message.data);        
    
        /* 成功收到应答，返回操作结果 */
        array_data.push_back(value_int(RpcError(ack_message._ret_code)));
        if (RpcError(ack_message._ret_code) != TECS_SUCCESS)
        {
            array_data.push_back(value_string(ack_message._err_str));
            goto query_out;
        }         
                   
        ApiImageFtpSvrInfo  info(0,"");

        info.ftp_svr_port = ack_message._image_ftp_svr_info._ftp_svr_port;
        info.ftp_svr_ip = ack_message._image_ftp_svr_info._ftp_svr_ip;

        array_data.push_back(info.to_rpc());      
    }    
    else
    {
        /*  超时 or 其他错误 ? */
        Dbg_Prn("%s : time out\n",_method_name.c_str());
        oss <<_method_name <<" time out";
        array_data.push_back(value_int(RpcError(ERROR_TIME_OUT)));
        array_data.push_back(value_string(oss.str()) );
    }

    /* 返回  */
query_out:
    array_result = new value_array(array_data);
    *retval = *array_result;

    delete array_result; 

    return;    
}


/******************************************************************************/
void ImageEnableMethod :: MethodEntry(
                        const paramList& paramList,
                        value* const retval)
{
    int64                 uid = -1;
    int64                 image_id = -1;
    bool                enable_flag ;
    ostringstream       oss;
    int                 ret_code = 0;
    
    /* 1. 获取参数 */
    uid         = get_userid();
    image_id    = value_i8(paramList.getI8(1));
    enable_flag = value_boolean(paramList.getBoolean(2));

    /* 2. 创建临时消息单元 */
    MessageUnit temp_mu(TempUnitName("tecs_image_enable"));
    temp_mu.Register();

    /* 3. 发消息给 IMAGE_MANAGE  */
    ImageEnableMsg request_message(uid, image_id,enable_flag);
    MessageOption option(MID(PROC_IMAGE_MANAGER, MU_IMAGE_MANAGER), 
                         EV_IMAGE_ENABLE_REQ,0,0);
    temp_mu.Send(request_message, option);

    /* 4. 等待MU_IMAGE_MANAGER回应 */
    MessageFrame message;
    if(SUCCESS == temp_mu.Wait(&message,k_image_wait_time))
    {
        /*  收到应答，针对之前消息的应答 ?  */
        Dbg_Prn("%s : rcv ack msg success!\n", _method_name.c_str());
        ImageOperateAckMsg  ack_message;
        ack_message.deserialize(message.data);
        
        ack_message.Print();
        
        /* 成功收到应答，返回操作结果 */
        ret_code = RpcError(ack_message._ret_code);
        oss << ack_message._result;
    }
    else
    {
        /*  超时 or 其他错误 ? */
        Dbg_Prn("%s : time out\n",_method_name.c_str());
        oss <<_method_name <<" time out";
        ret_code = RpcError(ERROR_TIME_OUT);
    }

    /* 返回  */
    xRET2(xINT(ret_code),xSTR(oss.str()));
    return;    
}


/******************************************************************************/
void ImagePublishMethod :: MethodEntry(
                        const paramList& paramList,
                        value* const retval)
{
    int64                 uid = -1;
    int64                 image_id = -1;
    bool                publish_flag ;
    ostringstream       oss;
    int                 ret_code = 0;
    
    /* 1. 获取参数 */
    uid         = get_userid();
    image_id    = value_i8(paramList.getI8(1));
    publish_flag = value_boolean(paramList.getBoolean(2));

    /* 2. 创建临时消息单元 */
    MessageUnit temp_mu(TempUnitName("tecs_image_publish"));
    temp_mu.Register();

    /* 3. 发消息给 IMAGE_MANAGE  */
    ImagePublishMsg request_message(uid,  image_id,publish_flag);
    MessageOption option(MID(PROC_IMAGE_MANAGER, MU_IMAGE_MANAGER),
                         EV_IMAGE_PUBLISH_REQ,0,0);
    temp_mu.Send(request_message, option);

    /* 4. 等待MU_IMAGE_MANAGER回应 */
    MessageFrame message;
    if(SUCCESS == temp_mu.Wait(&message,k_image_wait_time))
    {
        /*  收到应答，针对之前消息的应答 ?  */
        Dbg_Prn("%s : rcv ack msg success!\n", _method_name.c_str());
        ImageOperateAckMsg  ack_message;
        ack_message.deserialize(message.data);
        
        ack_message.Print();
        
        /* 成功收到应答，返回操作结果 */
        ret_code = RpcError(ack_message._ret_code);
        oss << ack_message._result;
    }
    else
    {
        /*  超时 or 其他错误 ? */
        Dbg_Prn("%s : time out\n",_method_name.c_str());
        oss <<_method_name <<" time out";
        ret_code = RpcError(ERROR_TIME_OUT);
    }

    /* 返回  */
    xRET2(xINT(ret_code),xSTR(oss.str()));
    return;    
}

/******************************************************************************/
void ImageDeployMethod :: MethodEntry(
                        const paramList& paramList,
                        value* const retval)
{
    ostringstream oss;
    string workflowid;
    ImageDeployMsg request_message;
   

    
    /* 1. 获取参数 */
    request_message._uid = get_userid();
    request_message._image_id = value_i8(paramList.getI8(1));    
    request_message._cluster_name = value_string(paramList.getString(2));
    request_message._base_uuid = GenerateUUID();//value_string(paramList.getString(3));  

    AuthRequest ar(request_message._uid);
    
    /*  发消息给 IMAGE_MANAGE 的选项  */
    MessageOption option(MID(PROC_IMAGE_MANAGER, MU_IMAGE_MANAGER), 
                         EV_IMAGE_DEPLOY_REQ,0,0);

    /*  等待MU_IMAGE_MANAGER回应 结构 */
    MessageFrame message;

    /* 2. 创建临时消息单元 */
    MessageUnit temp_mu(TempUnitName("tecs_image_deploy"));
    if(SUCCESS != temp_mu.Register())
    {
        oss << "Error, failed to register mu!";
        goto para_err_out;
    }
    
    ar.AddAuthorize(
        AuthRequest::IMAGE,           //授权对象类型,用user做例子
        request_message._image_id,    //对象的ID,image_id
        AuthRequest::MANAGE,          //操作类型
        0,                            //对象拥有者的用户ID
        false                         //此对象是否为公有
    );

    if (-1 == UserPool::Authorize(ar))
    {   
        /* 授权失败 */
        oss << "Error, failed to Authorize!";
        goto auth_err_out;
    }
    
    // 名称长度合法性校验
    if (request_message._cluster_name.size() != 0)
    {
        if (StringCheck::CheckComplexName(request_message._cluster_name,1,STR_LEN_64) != true)
        {
            oss << "Error, invalid cluster name:" << request_message._cluster_name;
            goto para_err_out;
        }     
    }
    else
    {
        oss << "Error, cluster name is null";
        goto para_err_out;
    }
    
    _upool->GetUserNameByID(request_message._uid, request_message._user_name);
    
    // 参数校验通过，开始发送消息给执行体
    temp_mu.Send(request_message, option);

    
    if(SUCCESS == temp_mu.Wait(&message,k_image_wait_time))
    {
        /*  收到应答，针对之前消息的应答 ?  */
        Dbg_Prn("%s : rcv ack msg success!\n", _method_name.c_str());
        ImageDeployAckMsg  ack_message;
        ack_message.deserialize(message.data);
        
        ack_message.Print();

        if ((SUCCESS != ack_message.state) && (ERROR_ACTION_RUNNING != ack_message.state))
        {
            oss << ack_message.detail;
            xRET2(xINT(RpcError(ack_message.state)),xSTR(oss.str()));  
        }
        else
        {
            ack_message.state = SUCCESS;
            oss << ack_message._base_uuid;
            workflowid = ack_message._workflow_id;
            xRET3(xINT(RpcError(ack_message.state)),xSTR(workflowid),xSTR(ack_message._workflow_engine));
        }
    
    }
    else
    {
        /*  超时 or 其他错误 ? */
        Dbg_Prn("%s : time out\n",_method_name.c_str());
        oss <<_method_name <<" time out";
        xRET2(xINT(RpcError(ERROR_TIME_OUT)),xSTR(oss.str()));  
    }

    /* 返回  */
    return;   
auth_err_out:
    xRET2(xINT(TECS_ERR_AUTHORIZE_FAILED),xSTR(oss.str()));
    return;    
para_err_out:
    xRET2(xINT(TECS_ERR_PARA_INVALID),xSTR(oss.str()));
    return;       
    
}

/******************************************************************************/
void ImageCancelMethod :: MethodEntry(
                        const paramList& paramList,
                        value* const retval)
{
    ostringstream oss;
    ImageCancelMsg request_message;
    int                 ret_code = 0;
   
    /* 1. 获取参数 */
    request_message._uid   = get_userid();
    request_message._base_uuid  = value_string(paramList.getString(1));    
    request_message._cluster_name = value_string(paramList.getString(2));
    
    AuthRequest ar(request_message._uid);
    
    /*  发消息给 IMAGE_MANAGE 的选项  */
    MessageOption option(MID(PROC_IMAGE_MANAGER, MU_IMAGE_MANAGER), 
                         EV_IMAGE_CANCEL_REQ,0,0);

    /*  等待MU_IMAGE_MANAGER回应 结构 */
    MessageFrame message;

    /* 2. 创建临时消息单元 */
    MessageUnit temp_mu(TempUnitName("tecs_image_set"));
    if(SUCCESS != temp_mu.Register())
    {
        oss << "Error, failed to register mu!";
        goto para_err_out;
    }
    
    ar.AddAuthorize(
        AuthRequest::IMAGE,           //授权对象类型,用user做例子
        request_message._base_uuid,   //对象的ID,image_id
        AuthRequest::MANAGE,          //操作类型
        0,                            //对象拥有者的用户ID
        false                         //此对象是否为公有
    );

    if (-1 == UserPool::Authorize(ar))
    {   
        /* 授权失败 */
        oss << "Error, failed to Authorize!";
        goto auth_err_out;
    }
    
    // 名称长度合法性校验
    if (request_message._base_uuid.size() != 0)
    {
        if (StringCheck::CheckComplexName(request_message._base_uuid,1,STR_LEN_64) != true)
        {
            oss << "Error, invalid parameter with " << request_message._base_uuid;
            goto para_err_out;
        }     
    }
    
    // 名称长度合法性校验
    if (request_message._cluster_name.size() != 0)
    {
        if (StringCheck::CheckComplexName(request_message._cluster_name,1,STR_LEN_64) != true)
        {
            oss << "Error, invalid parameter with " << request_message._cluster_name;
            goto para_err_out;
        }     
    }

    _upool->GetUserNameByID(request_message._uid, request_message._user_name);

    // 参数校验通过，开始发送消息给执行体
    temp_mu.Send(request_message, option);

    if(SUCCESS == temp_mu.Wait(&message,k_image_wait_time))
    {
        /*  收到应答，针对之前消息的应答 ?  */
        Dbg_Prn("%s : rcv ack msg success!\n", _method_name.c_str());
        ImageCancelAckMsg  ack_message;
        ack_message.deserialize(message.data);
        
        ack_message.Print();
        
        /* 成功收到应答，返回操作结果 */
        


        if ((SUCCESS != ack_message.state) && (ERROR_ACTION_RUNNING != ack_message.state))
        {
            oss << ack_message.detail;
        }
        else
        {
            ack_message.state = SUCCESS;
            oss << ack_message._workflow_id;
        }

        ret_code = RpcError(ack_message.state);
        xRET3(xINT(ret_code),xSTR(oss.str()), xSTR( ack_message._workflow_engine));
          
    }
    else
    {
        /*  超时 or 其他错误 ? */
        Dbg_Prn("%s : time out\n",_method_name.c_str());
        oss <<_method_name <<" time out";
        ret_code = RpcError(ERROR_TIME_OUT);
         /* 返回  */
        xRET2(xINT(ret_code),xSTR(oss.str()));
         
    }
    return;   
auth_err_out:
    xRET2(xINT(TECS_ERR_AUTHORIZE_FAILED),xSTR(oss.str()));
    return;    
para_err_out:
    xRET2(xINT(TECS_ERR_PARA_INVALID),xSTR(oss.str()));
    return;       
    
}

/******************************************************************************/
void ImagePrepareMethod :: MethodEntry(
                        const paramList& paramList,
                        value* const retval)
{
    ostringstream oss;
    ImagePrepareMsg request_message;
    int ret_code = ERROR;

    
    /* 1. 获取参数 */
    request_message._uid = get_userid();
    request_message._image_id = value_i8(paramList.getI8(1));    
    request_message._cluster_name = value_string(paramList.getString(2));

    AuthRequest ar(request_message._uid);
    
    /*  发消息给 IMAGE_MANAGE 的选项  */
    MessageOption option(MID(PROC_IMAGE_MANAGER, MU_IMAGE_MANAGER), 
                         EV_IMAGE_PREPARE_REQ,0,0);

    /*  等待MU_IMAGE_MANAGER回应 结构 */
    MessageFrame message;

    /* 2. 创建临时消息单元 */
    MessageUnit temp_mu(TempUnitName("tecs_image_prepare"));
    if(SUCCESS != temp_mu.Register())
    {
        oss << "Error, failed to register mu!";
        goto para_err_out;
    }
    
    ar.AddAuthorize(
        AuthRequest::IMAGE,           //授权对象类型,用user做例子
        request_message._image_id,    //对象的ID,image_id
        AuthRequest::MANAGE,          //操作类型
        0,                            //对象拥有者的用户ID
        false                         //此对象是否为公有
    );

    if (-1 == UserPool::Authorize(ar))
    {   
        /* 授权失败 */
        oss << "Error, failed to Authorize!";
        goto auth_err_out;
    }
    
    // 名称长度合法性校验
    if (request_message._cluster_name.size() != 0)
    {
        if (StringCheck::CheckComplexName(request_message._cluster_name,1,STR_LEN_64) != true)
        {
            oss << "Error, invalid cluster name:" << request_message._cluster_name;
            goto para_err_out;
        }     
    }
    else
    {
        oss << "Error, cluster name is null";
        goto para_err_out;
    }
    

    
    // 参数校验通过，开始发送消息给执行体
    temp_mu.Send(request_message, option);

    
    if(SUCCESS == temp_mu.Wait(&message,k_image_wait_time))
    {
        /*  收到应答，针对之前消息的应答 ?  */
        Dbg_Prn("%s : rcv ack msg success!\n", _method_name.c_str());
        ImagePrepareAckMsg  ack_message;
        ack_message.deserialize(message.data);
        
        ack_message.Print();
        
        /* 成功收到应答，返回操作结果 */
        ret_code = RpcError(ack_message.state);
        oss << ack_message.detail;
    }
    else
    {
        /*  超时 or 其他错误 ? */
        Dbg_Prn("%s : time out\n",_method_name.c_str());
        oss <<_method_name <<" time out";
        ret_code = RpcError(ERROR_TIME_OUT);
    }

    /* 返回  */
    xRET2(xINT(ret_code),xSTR(oss.str()));
    return;   
auth_err_out:
    xRET2(xINT(TECS_ERR_AUTHORIZE_FAILED),xSTR(oss.str()));
    return;    
para_err_out:
    xRET2(xINT(TECS_ERR_PARA_INVALID),xSTR(oss.str()));
    return;       
    
}

/******************************************************************************/
void ImageGetSpaceMethod :: MethodEntry(
                        const paramList& paramList,
                        value* const retval)
{
    ostringstream       oss;
    int                 ret_code = 0;
    
    /* 1. 获取参数 */
    int64  uid          = get_userid();
    
    /* 2. 创建临时消息单元 */
    MessageUnit temp_mu(TempUnitName("tecs_image_getspace"));
    temp_mu.Register();

    ImageGetSpaceMsg request_message(uid);

    /* 3. 发消息给 IMAGE_MANAGE  */  
    MessageOption option(MID(PROC_IMAGE_MANAGER, MU_IMAGE_MANAGER), 
                         EV_IMAGE_GET_SPACE_REQ,0,0);
    temp_mu.Send(request_message, option);

    /* 4. 等待MU_IMAGE_MANAGER回应 */
    MessageFrame message;
    if(SUCCESS == temp_mu.Wait(&message,k_image_wait_time))
    {
        /*  收到应答，针对之前消息的应答 ?  */
        Dbg_Prn("%s : rcv ack msg success!\n", _method_name.c_str());
        ImageGetSpaceAckMsg  ack_message;
        ack_message.deserialize(message.data);
		
        /* 成功收到应答，返回操作结果 */
        ret_code = RpcError(ack_message._ret_code);
        if (ack_message._ret_code != SUCCESS)
        {
            oss << "RpcError isn't SUCCESS";
            xRET2(xINT(RpcError(ret_code)),xSTR(oss.str()));
            return;
        }     
        
        xRET3(xINT(RpcError(ret_code)),xINT(ack_message._freespace),xINT(ack_message._totalspace));
    }    
    else
    {
        /*  超时 or 其他错误 ? */
        Dbg_Prn("%s : time out\n",_method_name.c_str());
        oss << _method_name <<" time out";
        xRET2(xINT(RpcError(ERROR_TIME_OUT)),xSTR(oss.str()));
    }      
    /* 返回  */
    return;
}


}

