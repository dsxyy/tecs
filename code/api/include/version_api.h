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

#ifndef TECS_VERSION_API_H
#define TECS_VERSION_API_H
#include "rpcable.h"
#include "api_error.h"

class ApiVersionInfo: public Rpcable
{
public:
    ApiVersionInfo()
    {
    };
    string  _package_name;
    string  _repo_version;
    string  _local_version;
    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_VALUE(_package_name);
        TO_VALUE(_repo_version);
        TO_VALUE(_local_version);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN(); 
        FROM_VALUE(_package_name);
        FROM_VALUE(_repo_version);
        FROM_VALUE(_local_version);
        FROM_RPC_END();
    };
};

class ApiVersionQueryReq: public Rpcable
{
public:
    ApiVersionQueryReq() {};
    ~ApiVersionQueryReq() {};

public:
    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_VALUE(_node_type);
        TO_VALUE(_cloud_name);
        TO_VALUE(_cloud_ip);
        TO_VALUE(_cluster_name);
        TO_VALUE(_cluster_ip);
        TO_VALUE(_storage_adapter_name);
        TO_VALUE(_storage_adapter_ip);
        TO_VALUE(_host_name);
        TO_VALUE(_host_ip);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN(); 
        FROM_VALUE(_node_type);
        FROM_VALUE(_cloud_name);
        FROM_VALUE(_cloud_ip);
        FROM_VALUE(_cluster_name);
        FROM_VALUE(_cluster_ip);
        FROM_VALUE(_storage_adapter_name);
        FROM_VALUE(_storage_adapter_ip);
        FROM_VALUE(_host_name);
        FROM_VALUE(_host_ip);
        FROM_RPC_END();
};


public:
    string _node_type;
    string _cloud_name;
    string _cloud_ip;
    string _cluster_name;
    string _cluster_ip;
    string _storage_adapter_name;
    string _storage_adapter_ip;
    string _host_name;
    string _host_ip;
};

class ApiVersionQueryAck: public Rpcable
{
public:
    ApiVersionQueryAck() {};
    ~ApiVersionQueryAck() {};

public:
    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_VALUE(_node_type);
        TO_VALUE(_cloud_name);
        TO_VALUE(_cloud_ip);
        TO_VALUE(_cluster_name);
        TO_VALUE(_cluster_ip);
        TO_VALUE(_storage_adapter_name);
        TO_VALUE(_storage_adapter_ip);
        TO_VALUE(_host_name);
        TO_VALUE(_host_ip);
        TO_STRUCT_ARRAY(_version_info);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_VALUE(_node_type);
        FROM_VALUE(_cloud_name);
        FROM_VALUE(_cloud_ip);
        FROM_VALUE(_cluster_name);
        FROM_VALUE(_cluster_ip);
        FROM_VALUE(_storage_adapter_name);
        FROM_VALUE(_storage_adapter_ip);
        FROM_VALUE(_host_name);
        FROM_VALUE(_host_ip);
        FROM_STRUCT_ARRAY(_version_info);
        FROM_RPC_END();
};


public:
    string _node_type;
    string _cloud_name;
    string _cloud_ip;
    string _cluster_name;
    string _cluster_ip;
    string _storage_adapter_name;
    string _storage_adapter_ip;
    string _host_name;
    string _host_ip;
    vector<ApiVersionInfo>_version_info;
};

class ApiVersionUpdateReq: public Rpcable
{
    public:
    ApiVersionUpdateReq() {};
    ~ApiVersionUpdateReq() {};

    public:
        TO_RPC
        {
            TO_RPC_BEGIN();
            TO_VALUE(_node_type);
            TO_VALUE(_cluster_name);
            TO_VALUE(_cluster_ip);
            TO_VALUE(_host_name);
            TO_VALUE(_host_ip);
            TO_VALUE(_package);
            TO_RPC_END();
        };

        FROM_RPC
        {
            FROM_RPC_BEGIN();
            FROM_VALUE(_node_type);
            FROM_VALUE(_cluster_name);
            FROM_VALUE(_cluster_ip);
            FROM_VALUE(_host_name);
            FROM_VALUE(_host_ip);
            FROM_VALUE(_package);
            FROM_RPC_END();
        };


public:
    string _node_type;
    string _cluster_name;
    string _cluster_ip;
    string _host_name;
    string _host_ip;
    string _package;
};

class ApiVersionCancelReq: public Rpcable
{
    public:
    ApiVersionCancelReq() {};
    ~ApiVersionCancelReq() {};

    public:
        TO_RPC
        {
            TO_RPC_BEGIN();
            TO_VALUE(_node_type);
            TO_VALUE(_cluster_name);
            TO_VALUE(_cluster_ip);
            TO_VALUE(_host_name);
            TO_VALUE(_host_ip);
            TO_VALUE(_package);
            TO_RPC_END();
        };

        FROM_RPC
        {
            FROM_RPC_BEGIN();
            FROM_VALUE(_node_type);
            FROM_VALUE(_cluster_name);
            FROM_VALUE(_cluster_ip);
            FROM_VALUE(_host_name);
            FROM_VALUE(_host_ip);
            FROM_VALUE(_package);
            FROM_RPC_END();
        };


public:
    string _node_type;
    string _cluster_name;
    string _cluster_ip;
    string _host_name;
    string _host_ip;
    string _package;
};


#endif

