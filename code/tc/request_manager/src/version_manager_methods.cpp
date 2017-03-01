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
#include "version_manager_methods.h"
#include "tecs_errcode.h"
#include "authrequest.h"
#include "version_api.h"
#include "log_agent.h"

namespace zte_tecs
{
void VersionGetRepoAddr::MethodEntry(xmlrpc_c::paramList const& paramList,
                                  xmlrpc_c::value * const retval)
{

    int64                        uid = -1;
    int                          ret_code;

    vector<xmlrpc_c::value>        array_data;
    xmlrpc_c::value_array        *array_result = NULL;


    /* 1. ��ȡ���� */
    uid = get_userid();
    /* ��ȡ��Ȩ */
    AuthRequest ar(uid);

    ar.AddAuthorize(
                    AuthRequest::VERSION, //��Ȩ��������,��user������
                    0,                    //�����ID
                    AuthRequest::INFO,  //��������
                    0,                    //����ӵ���ߵ��û�ID
                    false                 //�˶����Ƿ�Ϊ����
                    );

    if (-1 == UserPool::Authorize(ar))
    {   /* ��Ȩʧ�� */
        //oss <<"user can't access!";
        array_data.push_back(xmlrpc_c::value_int(TECS_ERR_AUTHORIZE_FAILED));
        array_data.push_back(xmlrpc_c::value_string("user can't access!"));

        array_result = new xmlrpc_c::value_array(array_data);
        *retval = *array_result;

        delete array_result;
        return;
    }

    /* 2. ������ʱ��Ϣ��Ԫ */
    MessageUnit temp_mu(TempUnitName("Vmm_Temp"));
    temp_mu.Register();

    /* 3. ����Ϣ�� Version_Manager_Agent  */
    MID mid(PROC_VERSION_MANAGER, MU_VERSION_MANAGER);
    MessageOption option(mid, EV_VERSION_GET_REPOADDR_REQ,0,0);
    temp_mu.Send("no data", option);
     //���ӵ�����Ϣ
    OutPut(SYS_DEBUG, "send EV_VERSION_GET_REPOADDR_REQ\n" );

    /* 4. �ȴ�Cluster_MANAGER��Ӧ */
    MessageFrame message;
    if(SUCCESS == temp_mu.Wait(&message,5000))
    {
        /*  �յ�Ӧ�����֮ǰ��Ϣ��Ӧ�� ?  */
        MessageVersionGetRepoAddrAck ack_message;
        ack_message.deserialize(message.data);
        /* ���ݷ��ؽ�����ظ���̨ */
        ret_code = RpcError(ack_message._ack_result);
        array_data.push_back(xmlrpc_c::value_int(ret_code));
        array_data.push_back(xmlrpc_c::value_string(ack_message._error_string));
        array_data.push_back(xmlrpc_c::value_string(ack_message._repo_addr));
    }
    else
    {
        /*  ��ʱ or �������� ? */
        array_data.push_back(xmlrpc_c::value_int(TECS_ERR_TIMEOUT));
        array_data.push_back(xmlrpc_c::value_string("Version get repo addr timeout") );
        OutPut(SYS_ERROR, "Version get repo addr timeout");
    }

    /* ����  */
    array_result = new xmlrpc_c::value_array(array_data);
    *retval = *array_result;

    delete array_result;

    return;
}

void VersionSetRepoAddr::MethodEntry(xmlrpc_c::paramList const& paramList,
                                  xmlrpc_c::value * const retval)
{

    int64                        uid = -1;
    int                          ret_code;

    vector<xmlrpc_c::value>        array_data;
    xmlrpc_c::value_array        *array_result = NULL;


    /* 1. ��ȡ���� */
    uid = get_userid();
    /* ��ȡ��Ȩ */
    AuthRequest ar(uid);

    ar.AddAuthorize(
                    AuthRequest::VERSION, //��Ȩ��������,��user������
                    0,                    //�����ID
                    AuthRequest::INFO,  //��������
                    0,                    //����ӵ���ߵ��û�ID
                    false                 //�˶����Ƿ�Ϊ����
                    );

    if (-1 == UserPool::Authorize(ar))
    {   /* ��Ȩʧ�� */
        //oss <<"user can't access!";
        array_data.push_back(xmlrpc_c::value_int(TECS_ERR_AUTHORIZE_FAILED));
        array_data.push_back(xmlrpc_c::value_string("user can't access!"));

        array_result = new xmlrpc_c::value_array(array_data);
        *retval = *array_result;

        delete array_result;
        return;
    }

    string repo_addr = value_string(paramList.getString(1));

    /* 2. ������ʱ��Ϣ��Ԫ */
    MessageUnit temp_mu(TempUnitName("Vmm_Temp"));
    temp_mu.Register();

    /* 3. ����Ϣ�� Version_Manager_Agent  */
    MID mid(PROC_VERSION_MANAGER, MU_VERSION_MANAGER);
    MessageOption option(mid, EV_VERSION_SET_REPOADDR_REQ,0,0);
    temp_mu.Send(repo_addr, option);
     //���ӵ�����Ϣ
    OutPut(SYS_DEBUG, "send EV_VERSION_SET_REPOADDR_REQ " );

    /* 4. �ȴ�Cluster_MANAGER��Ӧ */
    MessageFrame message;
    if(SUCCESS == temp_mu.Wait(&message,5000))
    {
        /*  �յ�Ӧ�����֮ǰ��Ϣ��Ӧ�� ?  */
        MessageVersionSetRepoAddrAck ack_message;
        ack_message.deserialize(message.data);
        /* ���ݷ��ؽ�����ظ���̨ */
        ret_code = RpcError(ack_message._ack_result);
        array_data.push_back(xmlrpc_c::value_int(ret_code));
        array_data.push_back(xmlrpc_c::value_string(ack_message._error_string));
    }
    else
    {
        /*  ��ʱ or �������� ? */
        array_data.push_back(xmlrpc_c::value_int(TECS_ERR_TIMEOUT));
        array_data.push_back(xmlrpc_c::value_string("Version set repo addr timeout") );
        OutPut(SYS_ERROR, "Version set repo addr timeout");
    }

    /* ����  */
    array_result = new xmlrpc_c::value_array(array_data);
    *retval = *array_result;

    delete array_result;

    return;
}

void VersionQuery::MethodEntry(xmlrpc_c::paramList const& paramList,
                                  xmlrpc_c::value * const retval)
{

    int64                        uid = -1;
    int                          ret_code;

    vector<xmlrpc_c::value>        array_data;
    xmlrpc_c::value_array        *array_result = NULL;


    /* 1. ��ȡ���� */
    uid = get_userid();
    /* ��ȡ��Ȩ */
    AuthRequest ar(uid);

    ar.AddAuthorize(
                    AuthRequest::VERSION, //��Ȩ��������,��user������
                    0,                    //�����ID
                    AuthRequest::INFO,  //��������
                    0,                    //����ӵ���ߵ��û�ID
                    false                 //�˶����Ƿ�Ϊ����
                    );

    if (-1 == UserPool::Authorize(ar))
    {   /* ��Ȩʧ�� */
        //oss <<"user can't access!";
        array_data.push_back(xmlrpc_c::value_int(TECS_ERR_AUTHORIZE_FAILED));
        array_data.push_back(xmlrpc_c::value_string("user can't access!"));

        array_result = new xmlrpc_c::value_array(array_data);
        *retval = *array_result;

        delete array_result;
        return;
    }

    ApiVersionQueryReq  api_query;
    MessageVersionQueryReq version_query;

    api_query.from_rpc(paramList.getStruct(1));
    version_query.FromApi(api_query);

    /* 2. ������ʱ��Ϣ��Ԫ */
    MessageUnit temp_mu(TempUnitName("Vmm_Temp"));
    temp_mu.Register();

    /* 3. ����Ϣ�� Version_Manager_Agent  */
    MID mid(PROC_VERSION_MANAGER, MU_VERSION_MANAGER);
    MessageOption option(mid, EV_VERSION_QUERY_REQ,0,0);
    temp_mu.Send(version_query, option);
     //���ӵ�����Ϣ
    OutPut(SYS_DEBUG, "send EV_VERSION_QUERY_REQ\n" );


    /* 4. �ȴ�Cluster_MANAGER��Ӧ */
    MessageFrame message;
    if(SUCCESS == temp_mu.Wait(&message,50000))
    {
        /*  �յ�Ӧ�����֮ǰ��Ϣ��Ӧ�� ?  */
        MessageVersionQueryAck ack_message;
        ApiVersionQueryAck api_ack_message;        
        ack_message.deserialize(message.data);
        ack_message.ToApi(api_ack_message);
        /* ���ݷ��ؽ�����ظ���̨ */
        ret_code = RpcError(ack_message._ack_result);
        array_data.push_back(xmlrpc_c::value_int(ret_code));
        array_data.push_back(xmlrpc_c::value_string(ack_message._error_string));
        array_data.push_back(xmlrpc_c::value_struct(api_ack_message.to_rpc())); 
        printf("send EV_VERSION_QUERY_REQ ok:%d %s,%s,%s,%s,%s,%s,%s,%s,%s!\n",ret_code,
            api_ack_message._node_type.c_str(),
            api_ack_message._cloud_name.c_str(),
            api_ack_message._cloud_ip.c_str(),
            api_ack_message._cluster_name.c_str(),
            api_ack_message._cluster_ip.c_str(),
            api_ack_message._storage_adapter_name.c_str(),
            api_ack_message._storage_adapter_ip.c_str(),
            api_ack_message._host_name.c_str(),
            api_ack_message._host_ip.c_str());

    }
    else
    {
        /*  ��ʱ or �������� ? */
        array_data.push_back(xmlrpc_c::value_int(TECS_ERR_TIMEOUT));
        array_data.push_back(xmlrpc_c::value_string("Version query timeout") );
        OutPut(SYS_ERROR, "Version query timeout");
    }

    /* ����  */
    array_result = new xmlrpc_c::value_array(array_data);
    *retval = *array_result;

    delete array_result;

    return;
}

void VersionUpdate::MethodEntry(xmlrpc_c::paramList const& paramList,
                                  xmlrpc_c::value * const retval)
{
    int64                        uid = -1;
    int                          ret_code;

    vector<xmlrpc_c::value>        array_data;
    xmlrpc_c::value_array        *array_result = NULL;


    /* 1. ��ȡ���� */
    uid = get_userid();
    /* ��ȡ��Ȩ */
    AuthRequest ar(uid);

    ar.AddAuthorize(
                    AuthRequest::VERSION, //��Ȩ��������,��user������
                    0,                    //�����ID
                    AuthRequest::INFO,  //��������
                    0,                    //����ӵ���ߵ��û�ID
                    false                 //�˶����Ƿ�Ϊ����
                    );

    if (-1 == UserPool::Authorize(ar))
    {   /* ��Ȩʧ�� */
        //oss <<"user can't access!";
        array_data.push_back(xmlrpc_c::value_int(TECS_ERR_AUTHORIZE_FAILED));
        array_data.push_back(xmlrpc_c::value_string("user can't access!"));

        array_result = new xmlrpc_c::value_array(array_data);
        *retval = *array_result;

        delete array_result;
        return;
    }


    ApiVersionUpdateReq  api_update;
    MessageVersionUpdateReq version_update;

    api_update.from_rpc(paramList.getStruct(1));
    version_update.FromApi(api_update);

    /* 2. ������ʱ��Ϣ��Ԫ */
    MessageUnit api_mu(TempUnitName("Vmm_Temp"));
    api_mu.Register();

    /* 3. ����Ϣ�� Version_Manager  */
    MID mid(PROC_VERSION_MANAGER, MU_VERSION_MANAGER);
    MessageOption option(mid, EV_VERSION_UPDATE_REQ,0,0);
    api_mu.Send(version_update, option);
     //���ӵ�����Ϣ
    OutPut(SYS_DEBUG, "send EV_VERSION_UPDATE_REQ\n" );

    /* 4. �ȴ�Version_Manager��Ӧ */
    MessageFrame message;
    if(SUCCESS == api_mu.Wait(&message,5000))
    {
        /*  �յ�Ӧ�����֮ǰ��Ϣ��Ӧ�� ?  */
        MessageVersionUpdateAck ack_message;
        ack_message.deserialize(message.data);
        /* ���ݷ��ؽ�����ظ���̨ */
        ret_code = RpcError(ack_message._ack_result);
        array_data.push_back(xmlrpc_c::value_int(ret_code));
        array_data.push_back(xmlrpc_c::value_string(ack_message._error_string));
        array_data.push_back(xmlrpc_c::value_i8(ack_message._oid));
    }
    else
    {
        /*  ��ʱ or �������� ? */
        array_data.push_back(xmlrpc_c::value_int(TECS_ERR_TIMEOUT));
        array_data.push_back(xmlrpc_c::value_string("Version update timeout") );
        OutPut(SYS_ERROR, "Version query timeout");
    }

    /* ����  */
    array_result = new xmlrpc_c::value_array(array_data);
    *retval = *array_result;

    delete array_result;

    return;
}

void VersionCancel::MethodEntry(xmlrpc_c::paramList const& paramList,
                                  xmlrpc_c::value * const retval)
{
    int64                        uid = -1;
    int                          ret_code;

    vector<xmlrpc_c::value>        array_data;
    xmlrpc_c::value_array        *array_result = NULL;


    /* 1. ��ȡ���� */
    uid = get_userid();
    /* ��ȡ��Ȩ */
    AuthRequest ar(uid);

    ar.AddAuthorize(
                    AuthRequest::VERSION, //��Ȩ��������,��user������
                    0,                    //�����ID
                    AuthRequest::INFO,  //��������
                    0,                    //����ӵ���ߵ��û�ID
                    false                 //�˶����Ƿ�Ϊ����
                    );

    if (-1 == UserPool::Authorize(ar))
    {   /* ��Ȩʧ�� */
        //oss <<"user can't access!";
        array_data.push_back(xmlrpc_c::value_int(TECS_ERR_AUTHORIZE_FAILED));
        array_data.push_back(xmlrpc_c::value_string("user can't access!"));

        array_result = new xmlrpc_c::value_array(array_data);
        *retval = *array_result;

        delete array_result;
        return;
    }
    ApiVersionCancelReq  api_cancel;
    MessageVersionCancelReq version_cancel;
    api_cancel.from_rpc(paramList.getStruct(1));
    version_cancel.FromApi(api_cancel);

    /* 2. ������ʱ��Ϣ��Ԫ */
    MessageUnit temp_mu(TempUnitName("Vmm_Temp"));
    temp_mu.Register();

    /* 3. ����Ϣ�� Version_Manager_Agent  */
    MID mid(PROC_VERSION_MANAGER, MU_VERSION_MANAGER);
    MessageOption option(mid, EV_VERSION_CANCEL_REQ,0,0);
    temp_mu.Send(version_cancel, option);
     //���ӵ�����Ϣ
    OutPut(SYS_DEBUG, "send EV_VERSION_UNINSTALL_REQ\n" );

    /* 4. �ȴ�Cluster_MANAGER��Ӧ */
    MessageFrame message;
    if(SUCCESS == temp_mu.Wait(&message,5000))
    {
        /*  �յ�Ӧ�����֮ǰ��Ϣ��Ӧ�� ?  */
        MessageVersionCancelAck ack_message;

        ack_message.deserialize(message.data);
        /* ���ݷ��ؽ�����ظ���̨ */
        ret_code = RpcError(ack_message._ack_result);
        array_data.push_back(xmlrpc_c::value_int(ret_code));
        array_data.push_back(xmlrpc_c::value_string(ack_message._error_string));
    }
    else
    {
        /*  ��ʱ or �������� ? */
        array_data.push_back(xmlrpc_c::value_int(TECS_ERR_TIMEOUT));
        array_data.push_back(xmlrpc_c::value_string("Version cancel timeout") );
        OutPut(SYS_ERROR, "Version cancel timeout");
    }

    /* ����  */
    array_result = new xmlrpc_c::value_array(array_data);
    *retval = *array_result;

    delete array_result;
    return;
}

} //end namespace zte_tecs

