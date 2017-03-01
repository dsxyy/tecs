/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：msg_version_with_api_method.h
* 文件标识：
* 内容摘要：version_manager 与 api_method 之间的消息体结构定义
* 当前版本：1.0
* 作    者：zhangyb
* 完成日期：2012年12月18日
*
* 修改记录1：
*     修改日期：2012/8/18
*     版 本 号：V1.0
*     修 改 zhangyb
*     修改内容：创建
*******************************************************************************/
#ifndef VERSION_WITH_API_METHOD_H
#define VERSION_WITH_API_METHOD_H
#include "sky.h"
#include "tecs_errcode.h"
#include "version_api.h"


namespace zte_tecs
{
class VersionInfo:public Serializable
{
public:
    VersionInfo()
    {
    };
    VersionInfo( string  package, string  repo_version,string  local_version)
    {
        _package = package;
        _repo_version = repo_version;
        _local_version = local_version;
    };
    string  _package;
    string  _repo_version;
    string  _local_version;
    void ToApi(ApiVersionInfo &api)
    {
        api._package_name=_package;
        api._repo_version=_repo_version;
        api._local_version=_local_version;
    }

    SERIALIZE
    {
        SERIALIZE_BEGIN(VersionInfo);
        WRITE_STRING(_package);
        WRITE_STRING(_repo_version);
        WRITE_STRING(_local_version);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(VersionInfo);
        READ_STRING(_package);
        READ_STRING(_repo_version);
        READ_STRING(_local_version);
        DESERIALIZE_END();
    };
private:
};

class MessageVersionGetRepoAddrAck: public Serializable
{
public:
    MessageVersionGetRepoAddrAck()
    {
        _ack_result=ERROR;
    };
    MessageVersionGetRepoAddrAck(int ack_result,const string &repo_addr)
    {
        _ack_result=ack_result;
        _repo_addr=repo_addr;
    };
    MessageVersionGetRepoAddrAck(int ack_result,const string &error_string,const string &repo_addr)
    {
        _ack_result=ack_result;
        _error_string=error_string;
        _repo_addr=repo_addr;
    };
    ~MessageVersionGetRepoAddrAck()
    {};


    int _ack_result;
    string  _error_string;
    string  _repo_addr;

    SERIALIZE
    {
        SERIALIZE_BEGIN(MessageVersionGetRepoAddrAck);
        WRITE_DIGIT(_ack_result);
        WRITE_STRING(_error_string);
        WRITE_STRING(_repo_addr);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(MessageVersionGetRepoAddrAck);
        READ_DIGIT(_ack_result);
        READ_STRING(_error_string);
        READ_STRING(_repo_addr);
        DESERIALIZE_END();
    };
};

class MessageVersionSetRepoAddrAck: public Serializable
{
public:
    MessageVersionSetRepoAddrAck()
    {
        _ack_result=ERROR;
    };
    MessageVersionSetRepoAddrAck(int ack_result)
    {
        _ack_result=ack_result;
    };
    MessageVersionSetRepoAddrAck(int ack_result,const string &error_string)
    {
        _ack_result=ack_result;
        _error_string=error_string;
    };
    ~MessageVersionSetRepoAddrAck()
    {};


    int _ack_result;
    string _error_string;

    SERIALIZE
    {
        SERIALIZE_BEGIN(MessageVersionSetRepoAddrAck);
        WRITE_DIGIT(_ack_result);
        WRITE_STRING(_error_string);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(MessageVersionSetRepoAddrAck);
        READ_DIGIT(_ack_result);
        READ_STRING(_error_string);
        DESERIALIZE_END();
    };
};

class MessageVersionQueryReq : public Serializable
{
public:
    MessageVersionQueryReq()
    {
    };
    ~MessageVersionQueryReq()
    {};

    string _node_type;
    string _cloud_name;
    string _cloud_ip;
    string _cluster_name;
    string _cluster_ip;
    string _storage_adapter_name;
    string _storage_adapter_ip;
    string _host_name;
    string _host_ip;
    
    void FromApi(ApiVersionQueryReq&api)
    {
        _node_type = api._node_type;
        _cloud_ip = api._cloud_ip;
        _cluster_name = api._cluster_name;
        _cluster_ip = api._cluster_ip;
        _storage_adapter_name = api._storage_adapter_name;
        _storage_adapter_ip = api._storage_adapter_ip;
        _host_name = api._host_name;
        _host_ip = api._host_ip;
    }
    SERIALIZE
    {
        SERIALIZE_BEGIN(MessageVersionQueryReq);
        WRITE_STRING(_node_type);
        WRITE_STRING(_cloud_name);
        WRITE_STRING(_cloud_ip);
        WRITE_STRING(_cluster_name);
        WRITE_STRING(_cluster_ip);
        WRITE_STRING(_storage_adapter_name);
        WRITE_STRING(_storage_adapter_ip);
        WRITE_STRING(_host_name);
        WRITE_STRING(_host_ip);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(MessageVersionQueryReq);
        READ_STRING(_node_type);
        READ_STRING(_cloud_name);
        READ_STRING(_cloud_ip);
        READ_STRING(_cluster_name);
        READ_STRING(_cluster_ip);
        READ_STRING(_storage_adapter_name);
        READ_STRING(_storage_adapter_ip);
        READ_STRING(_host_name);
        READ_STRING(_host_ip);
        DESERIALIZE_END();
    };
};

class MessageVersionQueryAck : public Serializable
{
public:
    MessageVersionQueryAck()
    {
        _ack_result=ERROR;
    };
    MessageVersionQueryAck(int  result)
    {
        _ack_result = result;
    };
    MessageVersionQueryAck(int  result,
        const string &error_string,
        const string &node_type,
        const string &cloud_name,
        const string &cloud_ip,
        const string &cluster_name,
        const string &cluster_ip,
        const string &storage_adapter_name,
        const string &storage_adapter_ip,
        const string &host_name,
        const string &host_ip,
        const vector<VersionInfo>&version_info)
    {
        _ack_result = result;
        _error_string = error_string;
        _node_type = node_type;
        _cloud_name = cloud_name;
        _cloud_ip = cloud_ip;
        _cluster_name = cluster_name;
        _cluster_ip = cluster_ip;
        _storage_adapter_name = storage_adapter_name;
        _storage_adapter_ip = storage_adapter_ip;
        _host_name = host_name;
        _host_ip = host_ip;
        _version_info = version_info;
    };
    ~MessageVersionQueryAck()
    {};
    int  _ack_result;
    string  _error_string;
    string _node_type;
    string _cloud_name;
    string _cloud_ip;
    string _cluster_name;
    string _cluster_ip;
    string _storage_adapter_name;
    string _storage_adapter_ip;
    string _host_name;
    string _host_ip;
    vector<VersionInfo>_version_info;
    
    void ToApi(ApiVersionQueryAck&api)
    {
        api._node_type=_node_type;
        api._cloud_name=_cloud_name;
        api._cloud_ip=_cloud_ip;
        api._cluster_name=_cluster_name;
        api._cluster_ip=_cluster_ip;
        api._storage_adapter_name=_storage_adapter_name;
        api._storage_adapter_ip=_storage_adapter_ip;
        api._host_name=_host_name;
        api._host_ip=_host_ip;
        vector<VersionInfo>::iterator it;
        ApiVersionInfo api_version;
        for(it = _version_info.begin(); it != _version_info.end(); it++)
        {
            it->ToApi(api_version);
            api._version_info.push_back(api_version);
        }
    }

    SERIALIZE
    {
        SERIALIZE_BEGIN(MessageVersionQueryAck);
        WRITE_DIGIT(_ack_result);
        WRITE_STRING(_error_string);
        WRITE_STRING(_node_type);
        WRITE_STRING(_cloud_name);
        WRITE_STRING(_cloud_ip);
        WRITE_STRING(_cluster_name);
        WRITE_STRING(_cluster_ip);
        WRITE_STRING(_storage_adapter_name);
        WRITE_STRING(_storage_adapter_ip);
        WRITE_STRING(_host_name);
        WRITE_STRING(_host_ip);
        WRITE_OBJECT_VECTOR(_version_info);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(MessageVersionQueryAck);
        READ_DIGIT(_ack_result);
        READ_STRING(_error_string);
        READ_STRING(_node_type);
        READ_STRING(_cloud_name);
        READ_STRING(_cloud_ip);
        READ_STRING(_cluster_name);
        READ_STRING(_cluster_ip);
        READ_STRING(_storage_adapter_name);
        READ_STRING(_storage_adapter_ip);
        READ_STRING(_host_name);
        READ_STRING(_host_ip);
        READ_OBJECT_VECTOR(_version_info);
        DESERIALIZE_END();
    };
};

class MessageVersionUpdateReq : public Serializable
{
public:
    MessageVersionUpdateReq()
    {
    };
    ~MessageVersionUpdateReq()
    {};

    string _node_type;
    string _cluster_name;
    string _cluster_ip;
    string _host_name;
    string _host_ip;
    string _package;
    
    void FromApi(const ApiVersionUpdateReq&api)
    {
        _node_type = api._node_type;
        _cluster_name = api._cluster_name;
        _cluster_ip = api._cluster_ip;
        _host_name = api._host_name;
        _host_ip = api._host_ip;
        _package = api._package;
    }

    SERIALIZE
    {
        SERIALIZE_BEGIN(MessageVersionUpdateReq);

        WRITE_STRING(_node_type);
        WRITE_STRING(_cluster_name);
        WRITE_STRING(_cluster_ip);
        WRITE_STRING(_host_name);
        WRITE_STRING(_host_ip);
        WRITE_STRING(_package);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(MessageVersionUpdateReq);
        READ_STRING(_node_type);
        READ_STRING(_cluster_name);
        READ_STRING(_cluster_ip);
        READ_STRING(_host_name);
        READ_STRING(_host_ip);
        READ_STRING(_package);
        DESERIALIZE_END();
    };
};

class MessageVersionUpdateAck : public Serializable
{
public:
    MessageVersionUpdateAck()
    {
        _ack_result=ERROR;
        _oid=0;
        
    };
    MessageVersionUpdateAck(STATUS  result,int64 oid)
    {
        _ack_result=result;
        _oid=oid;        
    };
    MessageVersionUpdateAck(STATUS  result,const string &error_string,int64 oid)
    {
        _ack_result=result;
        _error_string=error_string;
        _oid=oid;        
    };
    ~MessageVersionUpdateAck()
    {};
    int     _ack_result;
    string  _error_string;
    int64   _oid;
    
    SERIALIZE
    {
        SERIALIZE_BEGIN(MessageVersionUpdateAck);
        WRITE_DIGIT(_ack_result);
        WRITE_STRING(_error_string);
        WRITE_DIGIT(_oid);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(MessageVersionUpdateAck);
        READ_DIGIT(_ack_result);
        READ_STRING(_error_string);
        READ_DIGIT(_oid);
        DESERIALIZE_END();
    };
};
class MessageVersionCancelReq : public Serializable
{
public:
    MessageVersionCancelReq()
    {
    };
    ~MessageVersionCancelReq()
    {};

    string _node_type;
    string _cluster_name;
    string _cluster_ip;
    string _host_name;
    string _host_ip;
    string _package;
    void FromApi(const ApiVersionCancelReq&api)
    {
        _node_type = api._node_type;
        _cluster_name = api._cluster_name;
        _cluster_ip = api._cluster_ip;
        _host_name = api._host_name;
        _host_ip = api._host_ip;
        _package = api._package;
    }
    SERIALIZE
    {
        SERIALIZE_BEGIN(MessageVersionCancelReq);
        WRITE_STRING(_node_type);
        WRITE_STRING(_cluster_name);
        WRITE_STRING(_cluster_ip);
        WRITE_STRING(_host_name);
        WRITE_STRING(_host_ip);
        WRITE_STRING(_package);
        SERIALIZE_END();
    };


    DESERIALIZE
    {
        DESERIALIZE_BEGIN(MessageVersionCancelReq);
        READ_STRING(_node_type);
        READ_STRING(_cluster_name);
        READ_STRING(_cluster_ip);
        READ_STRING(_host_name);
        READ_STRING(_host_ip);
        READ_STRING(_package);
        DESERIALIZE_END();
    };
};

class MessageVersionCancelAck : public Serializable
{
public:
    MessageVersionCancelAck()
    {
        _ack_result=ERROR;
    };
    MessageVersionCancelAck(STATUS  result)
    {
        _ack_result=result;
    };
    MessageVersionCancelAck(STATUS  result,const string &error_string)
    {
        _ack_result=result;
        _error_string=error_string;
    };
    ~MessageVersionCancelAck()
    {};
    int  _ack_result;
    string  _error_string;

    SERIALIZE
    {
        SERIALIZE_BEGIN(MessageVersionCancelAck);
        WRITE_DIGIT(_ack_result);
        WRITE_STRING(_error_string);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(MessageVersionCancelAck);
        READ_DIGIT(_ack_result);
        READ_STRING(_error_string);
        DESERIALIZE_END();
    };
};
}

#endif
