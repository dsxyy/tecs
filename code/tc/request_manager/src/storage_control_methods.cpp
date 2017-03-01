/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：storage_control_method.cpp
* 文件标识：见配置管理计划书
* 内容摘要：SC模块涉及的XML-RPC method类的实现文件
* 当前版本：1.0
* 作    者：
* 完成日期：2013年5月3日
*
* 修改记录1：
*     修改日期：2013/5/3
*     版 本 号：V1.0
*     修 改 人：
*     修改内容：创建
*******************************************************************************/
#include "storage_control_methods.h"
#include "sky.h"
#include "mid.h"
#include "event.h"
#include "user_pool.h"
#include "authrequest.h"
#include "phy_port_pub.h"
#include "api_pub.h"
#include "volume.h"

static int sc_dbg_print = 1;
DEFINE_DEBUG_VAR(sc_dbg_print);
#define Dbg_Prn(fmt,arg...) \
        do \
        { \
            if(sc_dbg_print) \
            { \
                printf("[%s:%d] "fmt,__func__,__LINE__, ##arg); \
            } \
        }while(0)

using namespace xmlrpc_c;
namespace zte_tecs {

const  int portdisk_opt_wait_time = 5000;


#define XML_RPC_RETURN() \
    array_result = new value_array(array_data); \
    *retval = *array_result; \
    delete array_result; \
    return;


/******************************************************************************/
void PortableDiskCreateMethod::MethodEntry(const paramList &paramList,
                                    value * const retval)
{
    vector<value>     array_data;
    value_array       *array_result = NULL;
    ostringstream     oss;
    ostringstream     oss_request_id;
    CreateBlockReq request_message;
    MessageFrame message;

    /* 1、获取参数*/
    request_message._uid = get_userid();
    request_message._volsize = value_i8(paramList.getI8(1));
    request_message._cluster_name = value_string(paramList.getString(2));
    request_message._description = value_string(paramList.getString(3));
    request_message._request_id = GenerateUUID();
    /*  发消息给 SC 的选项  */
    MessageOption option(MID(PROC_SC, MU_SC_CONTROL),
                         EV_STORAGE_CREATE_BLOCK_REQ,0,0);


    UserPool *pup = UserPool::GetInstance();
    if(pup != NULL)
    {
        pup->GetUserNameByID(request_message._uid, request_message._user);
    }  

    /* 2. 创建临时消息单元 */
    MessageUnit temp_mu(TempUnitName("tecs_sc_create_portabledisk"));
    if(SUCCESS != temp_mu.Register())
    {
        oss << "Error, failed to register mu!";
        goto para_err_out;
    }

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

    /* 3. 发消息给 SC  */
    temp_mu.Send(request_message, option);

    if(SUCCESS == temp_mu.Wait(&message,portdisk_opt_wait_time))
    {
        /*  收到应答，针对之前消息的应答 ?  */
        Dbg_Prn("%s : rcv ack msg success!\n", _method_name.c_str());

        CreateBlockAck ack_message;
        ack_message.deserialize(message.data);

        Dbg_Prn("ret :%d , detail:%s, progress:%d\n", ack_message.state,ack_message.detail.c_str(),ack_message.progress);
        //ack_message.Print();


         //后续错误码需要整改
        if((SUCCESS!=ack_message.state)&&(ERROR_ACTION_RUNNING!=ack_message.state))
        {
            oss << ack_message.detail;
            array_data.push_back(value_int(RpcError(ack_message.state)));
            array_data.push_back(value_string(oss.str()));

        }
        else
        {
            ack_message.state = SUCCESS;
            oss << ack_message._workflow_id;
            oss_request_id << ack_message._request_id;
            array_data.push_back(value_int(RpcError(ack_message.state)));
            array_data.push_back(value_string(oss.str()));
            array_data.push_back(value_string(oss_request_id.str()));
        }

    }
    else
    {
        /*  超时 or 其他错误 ? */
        Dbg_Prn("%s : time out\n",_method_name.c_str());
        oss <<_method_name <<" create portdisk time out";
        array_data.push_back(value_int(RpcError(ERR_SC_TIMEOUT)));
        array_data.push_back(value_string(oss.str()));
    }

    /* 返回  */
    XML_RPC_RETURN();
    return;

para_err_out:
    array_data.push_back(value_int(ERR_SC_PARA_INVALID));
    array_data.push_back(value_string(oss.str()));
    XML_RPC_RETURN();
    return;
}

/******************************************************************************/
void PortableDiskDeleteMethod::MethodEntry(const paramList &paramList,
                                   value * const retval)
{
    vector<value>     array_data;
    value_array       *array_result = NULL;
    ostringstream     oss;
    ostringstream     oss_request_id;
    DeleteBlockReq    request_message;
    MessageFrame message;

    /* 1、获取参数*/
    request_message._uid = get_userid();
    request_message._request_id = value_string(paramList.getString(1));
    /*  发消息给 SC 的选项  */
    MessageOption option(MID(PROC_SC, MU_SC_CONTROL),
                         EV_STORAGE_DELETE_BLOCK_REQ,0,0);

    UserPool *pup = UserPool::GetInstance();
    if(pup != NULL)
    {
        pup->GetUserNameByID(request_message._uid, request_message._user);
    }  

    /* 2. 创建临时消息单元 */
    MessageUnit temp_mu(TempUnitName("tecs_sc_delete_portabledisk"));
    temp_mu.Register();

    if (request_message._request_id.size() != 0)
    {
        if (StringCheck::CheckComplexName(request_message._request_id,1,STR_LEN_64) != true)
        {
            oss << "Error, invalid request_id:" << request_message._request_id;
            goto para_err_out;
        }
    }
    else
    {
        oss << "Error, request_id is null";
        goto para_err_out;
    }

    /* 3. 发消息给 SC  */
    temp_mu.Send(request_message, option);

    if(SUCCESS == temp_mu.Wait(&message,portdisk_opt_wait_time))
    {
        /*  收到应答，针对之前消息的应答 ?  */
        Dbg_Prn("%s : rcv ack msg success!\n", _method_name.c_str());

        DeleteBlockAck ack_message;
        ack_message.deserialize(message.data);

        Dbg_Prn("ret :%d , detail:%s, progress:%d\n", ack_message.state,ack_message.detail.c_str(),ack_message.progress);


        //后续错误码需要整改
        if((SUCCESS!=ack_message.state)&&(ERROR_ACTION_RUNNING!=ack_message.state))
        {
            oss << ack_message.detail;
            array_data.push_back(value_int(RpcError(ack_message.state)));
            array_data.push_back(value_string(oss.str()));

        }
        else
        {
            ack_message.state = SUCCESS;
            oss << ack_message._workflow_id;
            oss_request_id << ack_message._request_id;
            array_data.push_back(value_int(RpcError(ack_message.state)));
            array_data.push_back(value_string(oss.str()));
            array_data.push_back(value_string(oss_request_id.str()));
        }

    }
    else
    {
        /*  超时 or 其他错误 ? */
        Dbg_Prn("%s : time out\n",_method_name.c_str());
        oss <<_method_name <<" delete portdisk time out";
        array_data.push_back(value_int(RpcError(ERR_SC_TIMEOUT)));
        array_data.push_back(value_string(oss.str()));
    }

    /* 返回  */
    XML_RPC_RETURN();
    return;
para_err_out:
    array_data.push_back(value_int(ERR_SC_PARA_INVALID));
    array_data.push_back(value_string(oss.str()));
    XML_RPC_RETURN();
    return;
}


/******************************************************************************/
void PortableDiskBlockInfoGetMethod::MethodEntry(const paramList &paramList,
                                    value * const retval)
{
    vector<value>     array_data;
    value_array       *array_result = NULL;
    ostringstream     oss;
    ostringstream     oss_request_id;
    GetBlockInfoReq blockinfo_req;
    MessageFrame message;
    vector<value>     array_ip;
    vector<string>::iterator it;

    /* 1、获取参数*/
    blockinfo_req._request_id = value_string(paramList.getString(1));   //ipara

    /*  发消息给 SC 的选项  */
    MessageOption option(MID(PROC_SC, MU_SC_CONTROL),
                         EV_STORAGE_GET_BLOCKINFO_REQ,0,0);


    /*  创建临时消息单元 */
    MessageUnit temp_mu(TempUnitName("tecs_sc_get_blockinfo"));
    if(SUCCESS != temp_mu.Register())
    {
        oss << "Error, failed to register mu!";
        goto para_err_out;
    }

    /* 2. 检查参数 */
    if (!StringCheck::CheckComplexName( blockinfo_req._request_id, 1, STR_LEN_64))
    {
        oss << "Error, request_id invalid";
        goto para_err_out;
    }

    /* 3. 发消息给 SC  */
    temp_mu.Send(blockinfo_req, option);

    if(SUCCESS == temp_mu.Wait(&message,portdisk_opt_wait_time))
    {
        /*  收到应答  */
        Dbg_Prn("%s : rcv ack msg success!\n", _method_name.c_str());

        GetBlockInfoAck ack_message;
        ack_message.deserialize(message.data);

        Dbg_Prn("ret :%d , detail:%s, progress:%d\n", ack_message.state,ack_message.detail.c_str(),ack_message.progress);

        if((SUCCESS!=ack_message.state)&&(ERROR_ACTION_RUNNING!=ack_message.state))
        {
            array_data.push_back(value_int(RpcError(ack_message.state)));
            array_data.push_back(value_string(ack_message.detail));

        }
        else/* 返回的是RUNNING  */
        {
       
            for(it=ack_message._dataip_vec.begin();it!=ack_message._dataip_vec.end();it++)
            {
                array_ip.push_back(value_string(*it));
            }
            ack_message.state = SUCCESS;
            array_data.push_back(value_int(RpcError(ack_message.state)));
            array_data.push_back(value_string(ack_message._type));
            array_data.push_back(value_string(ack_message._target_name));
            array_data.push_back(value_int(ack_message._lunid));
            array_data.push_back(value_array(array_ip));           

        }

    }
    else
    {
        /*  超时 or 其他错误 ? */
        Dbg_Prn("%s : time out\n",_method_name.c_str());
        oss <<_method_name <<" get Block info time out";
        array_data.push_back(value_int(RpcError(ERR_SC_TIMEOUT)));
        array_data.push_back(value_string(oss.str()));
    }

    /* 返回  */
    XML_RPC_RETURN();
    return;

para_err_out:
    array_data.push_back(value_int(ERROR_INVALID_ARGUMENT));
    array_data.push_back(value_string(oss.str()));
    XML_RPC_RETURN();
    return;
}



/******************************************************************************/
void PortableDiskAttachHostMethod::MethodEntry(const paramList &paramList,
                                    value * const retval)
{
    vector<value>     array_data;
    value_array       *array_result = NULL;
    ostringstream     oss;
    ostringstream     oss_request_id;
    AuthBlockReq auth_req;
    MessageFrame message;
    string _target_name;

    GetBlockInfoReq blockinfo_req;
    MessageFrame messageblockinfo;

    /* 1、先查询移动盘是否已经attach  */

    /* 获取参数*/
    blockinfo_req._request_id = value_string(paramList.getString(1));   //ipara

    /*  发消息给 SC 的选项  */
    MessageOption option01(MID(PROC_SC, MU_SC_CONTROL),
                         EV_STORAGE_GET_BLOCKINFO_REQ,0,0);

    /*  创建临时消息单元 */
    MessageUnit temp_mu01(TempUnitName("tecs_sc_get_blockinfo"));

    /*  发消息给 SC 的选项  */
    MessageOption option(MID(PROC_SC, MU_SC_CONTROL),
                         EV_STORAGE_AUTH_BLOCK_REQ,0,0);

    /*  创建临时消息单元 */
    MessageUnit temp_mu(TempUnitName("tecs_sc_attach_host"));


    if((SUCCESS != temp_mu01.Register())||(SUCCESS != temp_mu.Register()))
    {
        oss << "Error, failed to register mu!";
        goto para_err_out;
    }


    /*  检查参数 */
    if (!StringCheck::CheckComplexName( blockinfo_req._request_id, 1, STR_LEN_64))
    {
        oss << "Error, request_id invalid";
        goto para_err_out;
    }

    /*  发消息给 SC  */
    temp_mu01.Send(blockinfo_req, option01);

    if(SUCCESS == temp_mu01.Wait(&messageblockinfo,portdisk_opt_wait_time))
    {
        /*  收到应答  */
        Dbg_Prn("%s : rcv ack msg success!\n", _method_name.c_str());

        GetBlockInfoAck ack_BlockInfo;
        ack_BlockInfo.deserialize(messageblockinfo.data);

        Dbg_Prn("ret :%d , detail:%s, progress:%d\n", ack_BlockInfo.state,ack_BlockInfo.detail.c_str(),ack_BlockInfo.progress);

        if((SUCCESS!=ack_BlockInfo.state)&&(ERROR_ACTION_RUNNING!=ack_BlockInfo.state))
        {
            array_data.push_back(value_int(RpcError(ack_BlockInfo.state)));
            array_data.push_back(value_string(ack_BlockInfo.detail));
            XML_RPC_RETURN();
            return;
        }
        else
        {
            ack_BlockInfo.state = SUCCESS;
            if (ack_BlockInfo._target_name.size() != 0)//_target_name非空表示已挂载
            {
                Dbg_Prn("%s : portabledisk has been attached yet.\n",_method_name.c_str());
                oss <<_method_name <<" portabledisk has attached yet ";
                array_data.push_back(value_int(RpcError(ERROR_PERMISSION_DENIED)));
                array_data.push_back(value_string(oss.str()));
                XML_RPC_RETURN();
                return;
            }
        }
    }
    else
    {
        oss << "Error, request_id invalid";
        goto para_err_out;
    }

    /* 2 、go to attach  */

    /* 获取参数*/
    auth_req._request_id = value_string(paramList.getString(1));   //ipara
    auth_req._userip = value_string(paramList.getString(2));   //ipara
    auth_req._iscsiname = value_string(paramList.getString(3));     //ipara

    auth_req._vid = -1;
    auth_req._dist_id = -1; 
    auth_req._usage ="tohost:"+auth_req._iscsiname;

    /*  检查参数 */
    if (!StringCheck::CheckComplexName( auth_req._request_id, 1, STR_LEN_64))
    {
        oss << "Error, request_id invalid";
        goto para_err_out;
    }
    if (!StringCheck::CheckComplexName(auth_req._userip, 1, STR_LEN_64))
    {
        oss << "Error, userip is invalid";
        goto para_err_out;
    }

    if ((auth_req._iscsiname.size() < 1)||(auth_req._iscsiname.size() > STR_LEN_64)) //含: 符号
    {
        oss << "Error, iscsi_name invalid";
        goto para_err_out;
    }     

    /*  发消息给 SC  */
    temp_mu.Send(auth_req, option);

    if(SUCCESS == temp_mu.Wait(&message,portdisk_opt_wait_time))
    {
        /*  收到应答  */
        Dbg_Prn("%s : rcv ack msg success!\n", _method_name.c_str());

        AuthBlockAck ack_message;
        ack_message.deserialize(message.data);

        Dbg_Prn("ret :%d , detail:%s, progress:%d\n", ack_message.state,ack_message.detail.c_str(),ack_message.progress);

        if((SUCCESS!=ack_message.state)&&(ERROR_ACTION_RUNNING!=ack_message.state))
        {
            array_data.push_back(value_int(RpcError(ack_message.state)));
            array_data.push_back(value_string(ack_message.detail));

        }
        else
        {
            ack_message.state = SUCCESS;
            array_data.push_back(value_int(RpcError(ack_message.state)));                       
        }

    }
    else
    {
        /*  超时 or 其他错误 ? */
        Dbg_Prn("%s : time out\n",_method_name.c_str());
        oss <<_method_name <<" attach disk for host time out";
        array_data.push_back(value_int(RpcError(ERR_SC_TIMEOUT)));
        array_data.push_back(value_string(oss.str()));
    }

    /* 返回  */
    XML_RPC_RETURN();
    return;

para_err_out:
    array_data.push_back(value_int(ERROR_INVALID_ARGUMENT));
    array_data.push_back(value_string(oss.str()));
    XML_RPC_RETURN();
    return;
}

/******************************************************************************/
void PortableDiskDetachHostMethod::MethodEntry(const paramList &paramList,
                                   value * const retval)
{
    vector<value>     array_data;
    value_array       *array_result = NULL;
    ostringstream     oss;
    ostringstream     oss_request_id;
    DeAuthBlockReq    deauth_req;
    MessageFrame message;

    GetBlockInfoReq blockinfo_req;
    MessageFrame messageblockinfo;
    GetBlockInfoAck ack_BlockInfo;


    /* 1、先查询移动盘是否已经detach  */

    /* 获取参数*/
    blockinfo_req._request_id = value_string(paramList.getString(1));   //ipara

    /*  发消息给 SC 的选项  */
    MessageOption option01(MID(PROC_SC, MU_SC_CONTROL),
                         EV_STORAGE_GET_BLOCKINFO_REQ,0,0);

    MessageOption option(MID(PROC_SC, MU_SC_CONTROL),
                         EV_STORAGE_DEAUTH_BLOCK_REQ,0,0);

    /*  创建临时消息单元 */
    MessageUnit temp_mu01(TempUnitName("tecs_sc_get_blockinfo"));

    MessageUnit temp_mu(TempUnitName("tecs_sc_detach_host"));


    if((SUCCESS != temp_mu01.Register())||(SUCCESS != temp_mu.Register()))
    {
        oss << "Error, failed to register mu!";
        goto para_err_out;
    }


    /*  检查参数 */
    if (!StringCheck::CheckComplexName( blockinfo_req._request_id, 1, STR_LEN_64))
    {
        oss << "Error, request_id invalid";
        goto para_err_out;
    }

    /*  发消息给 SC  */
    temp_mu01.Send(blockinfo_req, option01);

    if(SUCCESS == temp_mu01.Wait(&messageblockinfo,portdisk_opt_wait_time))
    {
        /*  收到应答  */
        Dbg_Prn("%s : rcv ack msg success!\n", _method_name.c_str());


        ack_BlockInfo.deserialize(messageblockinfo.data);

        Dbg_Prn("ret :%d , detail:%s, progress:%d\n", ack_BlockInfo.state,ack_BlockInfo.detail.c_str(),ack_BlockInfo.progress);

        if((SUCCESS!=ack_BlockInfo.state)&&(ERROR_ACTION_RUNNING!=ack_BlockInfo.state))
        {
            array_data.push_back(value_int(RpcError(ack_BlockInfo.state)));
            array_data.push_back(value_string(ack_BlockInfo.detail));
            XML_RPC_RETURN();
            return;
        }
        else
        {
            ack_BlockInfo.state = SUCCESS;
            if (ack_BlockInfo._target_name.size() == 0)
            {
                Dbg_Prn("%s : block has not attached yet.\n",_method_name.c_str());
                array_data.push_back(value_int(RpcError(ack_BlockInfo.state)));
                XML_RPC_RETURN();
                return;
            }
        }
    }
    else
    {
        oss << "Error, request_id invalid";
        goto para_err_out;
    }


    /* 2 、go to detach  */

    /* 获取参数*/
    deauth_req._request_id = value_string(paramList.getString(1));  //ipara    
    deauth_req._iscsiname = ack_BlockInfo._iscsi_name;
    deauth_req._userip = ack_BlockInfo._ip_address;
    deauth_req._vid = -1;
    deauth_req._dist_id = -1;     

    /*  检查参数 */
    if (!StringCheck::CheckComplexName( deauth_req._request_id, 1, STR_LEN_64))
    {
        oss << "Error, request_id invalid";
        goto para_err_out;
    }

    /*  发消息给 SC  */
    temp_mu.Send(deauth_req, option);

    if(SUCCESS == temp_mu.Wait(&message,portdisk_opt_wait_time))
    {
        /*  收到应答  */
        Dbg_Prn("%s : rcv ack msg success!\n", _method_name.c_str());

        DeAuthBlockAck ack_message;
        ack_message.deserialize(message.data);

        Dbg_Prn("ret :%d , detail:%s, progress:%d\n", ack_message.state,ack_message.detail.c_str(),ack_message.progress);


        if((SUCCESS!=ack_message.state)&&(ERROR_ACTION_RUNNING!=ack_message.state))
        {
            array_data.push_back(value_int(RpcError(ack_message.state)));
            array_data.push_back(value_string(ack_message.detail));

        }
        else
        {
            ack_message.state = SUCCESS;
            array_data.push_back(value_int(RpcError(ack_message.state)));            
        }

    }
    else
    {
        /*  超时 or 其他错误 ? */
        Dbg_Prn("%s : time out\n",_method_name.c_str());
        oss <<_method_name <<" detach disk for host time out";
        array_data.push_back(value_int(RpcError(ERR_SC_TIMEOUT)));
        array_data.push_back(value_string(oss.str()));

    }
    /* 返回  */
    XML_RPC_RETURN();
    return;
para_err_out:
    array_data.push_back(value_int(ERROR_INVALID_ARGUMENT));
    array_data.push_back(value_string(oss.str()));
    XML_RPC_RETURN();
    return;
}


/***************************调试函数***************************************************/
void DbgAttachHostDisk(const char* request_id, const char* userip, const char* iscsiname)
{
    AuthBlockReq auth_req;
    MessageFrame message;
    AuthBlockAck ack_message;

    if (NULL == request_id || NULL == userip || NULL == iscsiname)
    {
        cout << "Invalid para!" <<endl;
        return;
    }

    auth_req._request_id = request_id; 
    auth_req._userip =  userip;
    auth_req._iscsiname = iscsiname;

    auth_req._vid = -1;
    auth_req._dist_id = -1; 
    auth_req._usage ="tohost:"+auth_req._iscsiname;

    /*  发消息给 SC 的选项  */
    MessageOption option(MID(PROC_SC, MU_SC_CONTROL),
                         EV_STORAGE_AUTH_BLOCK_REQ,0,0);


    /*  创建临时消息单元 */
    MessageUnit temp_mu(TempUnitName("tecs_sc_attach_host"));
    if(SUCCESS != temp_mu.Register())
    {
        cout << " Fail ,Creat Mu " << endl;
        return;
    }

    /* 3. 发消息给 SC  */
    temp_mu.Send(auth_req, option);

    if(SUCCESS == temp_mu.Wait(&message,portdisk_opt_wait_time))
    {

        ack_message.deserialize(message.data);

        if((SUCCESS!=ack_message.state)&&(ERROR_ACTION_RUNNING!=ack_message.state))
        {
            cout << " Fail ,ret = "<< ack_message.state << ",Detail:" << ack_message.detail <<endl;
        }
        else
        {
            cout << " Suc ,attach finished " <<endl;                  
        }

    }
    else
    {
        /*  超时 or 其他错误 ? */
        cout << " Fail ,Wait timeout " <<endl;
    }
    return;

}
DEFINE_DEBUG_FUNC(DbgAttachHostDisk);

/***************************调试函数***************************************************/
void DbgDetachHostDisk(const char* request_id)
{
    DeAuthBlockReq    deauth_req;
    MessageFrame message;
    DeAuthBlockAck ack_message;

    if (NULL == request_id)
    {
        cout << "Invalid para!" <<endl;
        return;
    }

    GetBlockInfoReq blockinfo_req;
    MessageFrame messageblockinfo;
    GetBlockInfoAck ack_BlockInfo;

    /* 1、获取参数*/
    blockinfo_req._request_id = request_id;

    /*  发消息给 SC 的选项  */
    MessageOption option01(MID(PROC_SC, MU_SC_CONTROL),
                         EV_STORAGE_GET_BLOCKINFO_REQ,0,0);

    MessageOption option(MID(PROC_SC, MU_SC_CONTROL),
                         EV_STORAGE_DEAUTH_BLOCK_REQ,0,0); 

    /*  创建临时消息单元 */
    MessageUnit temp_mu01(TempUnitName("tecs_sc_get_blockinfo"));

    MessageUnit temp_mu(TempUnitName("tecs_sc_detach_host"));

    if((SUCCESS != temp_mu.Register())||(SUCCESS != temp_mu01.Register()))
    {
        cout << " Fail ,Creat Mu " << endl;
        return;
    }
    temp_mu01.Send(blockinfo_req, option01);

    if(SUCCESS == temp_mu01.Wait(&messageblockinfo,portdisk_opt_wait_time))
    {
        ack_BlockInfo.deserialize(messageblockinfo.data);

        if((SUCCESS!=ack_BlockInfo.state)&&(ERROR_ACTION_RUNNING!=ack_BlockInfo.state))
        {
            cout << " Fail ,ret = "<< ack_message.state << ",Detail:" << ack_message.detail <<endl;
            return;
        }
        else
        {
            ack_BlockInfo.state = SUCCESS;
            if (ack_BlockInfo._target_name.size() == 0)
            {
                cout << " Fail ,target_name is null " <<endl;
                return;
            }
        }
    }
    else
    {
            cout << " Fail ,ret = "<< ack_message.state << ",Detail:" << ack_message.detail <<endl;
            return;
    }

    deauth_req._request_id = request_id; 
    deauth_req._iscsiname = ack_BlockInfo._iscsi_name;
    deauth_req._userip = ack_BlockInfo._ip_address;
    deauth_req._vid = -1;
    deauth_req._dist_id = -1; 


    /* 3. 发消息给 SC  */
    temp_mu.Send(deauth_req, option);

    if(SUCCESS == temp_mu.Wait(&message,portdisk_opt_wait_time))
    {

        ack_message.deserialize(message.data);

        if((SUCCESS!=ack_message.state)&&(ERROR_ACTION_RUNNING!=ack_message.state))
        {
            cout << " Fail ,ret = "<< ack_message.state << ",Detail:" << ack_message.detail <<endl;
        }
        else
        {
            cout << " Suc ,detach finished " <<endl;                  
        }

    }
    else
    {
        /*  超时 or 其他错误 ? */
        cout << " Fail ,Wait timeout " <<endl;
    }
    return;

}

DEFINE_DEBUG_FUNC(DbgDetachHostDisk);

/***************************调试函数***************************************************/
void DbgGetBlockInfo(const char* request_id)
{
    GetBlockInfoReq blockinfo_req;
    MessageFrame message;
    GetBlockInfoAck ack_message;

    if (NULL == request_id)
    {
        cout << "Invalid para!" <<endl;
        return;
    }

    blockinfo_req._request_id = request_id; 


    /*  发消息给 SC 的选项  */
    MessageOption option(MID(PROC_SC, MU_SC_CONTROL),
                         EV_STORAGE_GET_BLOCKINFO_REQ,0,0);


    /*  创建临时消息单元 */
    MessageUnit temp_mu(TempUnitName("tecs_sc_get_blockinfo"));
    if(SUCCESS != temp_mu.Register())
    {
        cout << " Fail ,Creat Mu " << endl;
        return;
    }

    /* 3. 发消息给 SC  */
    temp_mu.Send(blockinfo_req, option);

    if(SUCCESS == temp_mu.Wait(&message,portdisk_opt_wait_time))
    {

        ack_message.deserialize(message.data);

        if((SUCCESS!=ack_message.state)&&(ERROR_ACTION_RUNNING!=ack_message.state))
        {
            cout << " Fail ,ret = "<< ack_message.state << ",Detail:" << ack_message.detail <<endl;
        }
        else
        {
            cout << " Suc ,getblock!Type = " 
                   << ack_message._type
                   << ",TargetName=" 
                   << ack_message._target_name
                   << ",LunId=" 
                   << ack_message._lunid 
                   << ",IscsiName=" 
                   << ack_message._iscsi_name 
                   << ",IpAddress=" 
                   << ack_message._ip_address
                   << endl;
        }

    }
    else
    {
        /*  超时 or 其他错误 ? */
        cout << " Fail ,Wait timeout " <<endl;
    }
    return;

}

DEFINE_DEBUG_FUNC(DbgGetBlockInfo);
/******************************************************************************/


}

