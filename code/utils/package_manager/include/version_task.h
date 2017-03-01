/******************************************************************************
* Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：version_task.h
* 文件标识：
* 内容摘要：版本管理任务管理相关的实现头文件
* 当前版本：1.0
* 作    者：zhangyb
* 完成日期：2012年9月26日
*
* 修改记录1：
*     修改日期：2012年9月26日
*     版 本 号：V1.0
*     修 改 人：zhangyb
*     修改内容：创建
******************************************************************************/

#ifndef TC_VERSION_TASK_H
#define TC_VERSION_TASK_H
#include "tecs_errcode.h"
#include "tecs_config.h"
#include "log_agent.h"
#include "version_log.h"
#include "version_popen.h"
#include "version_api.h"
#include "msg_version_with_api_method.h"


namespace zte_tecs
{

class VersionQueryImpl:public VersionPopen
{
public:
    VersionQueryImpl(int time_out,
                const string& repo_addr,
                const MID &operater,                
                const string& cloud_ip):
        _repo_addr(repo_addr),
        _cloud_ip(cloud_ip)
    {

        TecsConfig *config = TecsConfig::Get();
        _operater = operater;
        _node_type="tc";
        _cloud_name=config->get_application_name();        
        _command= _cloudcmd + " " + _node_type + " " + _cloud_ip+
                  " -f " + _cloudupgrade + " query " + _repo_addr + " tecs-"+_node_type;
        SetTimeOut(time_out);
    };

    VersionQueryImpl(int time_out,
                const string& repo_addr,
                const MID &operater,
                const string& node_type,
                const string& name,
                const string& ip):
        _repo_addr(repo_addr)
    {
        if ("cc" == node_type)
        {
            _cluster_name = name;
            _cluster_ip = ip;
        }
        else
        {
            _storage_adapter_name = name;
            _storage_adapter_ip = ip;
        }
        _node_type=node_type;
        TecsConfig *config = TecsConfig::Get();
        _cloud_name=config->get_application_name();
        _operater = operater;
        _command= _cloudcmd + " " + node_type + " " + name + " " + ip+
                  " -f " + _cloudupgrade + " query " + _repo_addr + " tecs-"+node_type;
        SetTimeOut(time_out);
    };

    VersionQueryImpl(int time_out,
                    const string& repo_addr,
                    const MID &operater,
                    const string& cluster_name,
                    const string& cluster_ip,
                    const string& hostname,                    
                    const string& host_ip):
        _repo_addr(repo_addr),      
        _cluster_name(cluster_name),
        _cluster_ip(cluster_ip),
        _host_name(hostname),
        _host_ip(host_ip)
    {
        TecsConfig *config = TecsConfig::Get();
        _cloud_name=config->get_application_name();
        _operater = operater;
        _node_type="hc";
        _command= _cloudcmd + " " + _node_type + " " + _cluster_ip + " " + _host_ip+
                  " -f " + _cloudupgrade + " query " + _repo_addr + " tecs-"+_node_type;
        SetTimeOut(time_out);
    }; 
    STATUS Execute();
private:
    VersionQueryImpl()
    {};
    static  string                  _cloudcmd;
    static  string                  _cloudupgrade;
    void SndQueryResult();
    STATUS ParseError();
    STATUS ProcessData();
    MID                 _operater;
    string              _repo_addr;
    string              _node_type;
    string              _cloud_name;
    string              _cloud_ip;
    string              _cluster_name;
    string              _cluster_ip;
    string              _storage_adapter_name;
    string              _storage_adapter_ip;
    string              _host_name;
    string              _host_ip;
    string              _command;
    string              _repo_version;
    string              _old_version;
    string              _repo_common_version;
    string              _old_common_version;
    vector<VersionInfo> _versions;
    DISALLOW_COPY_AND_ASSIGN(VersionQueryImpl);    
};

class VersionQueryTask:public Worktodo
{
public:
    VersionQueryTask(int time_out,
                const string& repo_addr,
                const MID &operater,                
                const string& cloud_ip):
    Worktodo("tc")
    {

        _query_impl= new VersionQueryImpl(time_out,repo_addr,operater,cloud_ip);
    };

    VersionQueryTask(int time_out,                
                const string& repo_addr,
                const MID &operater,
                const string& node_type,
                const string& name,
                const string& ip):
    Worktodo(name)
    {

        _query_impl= new VersionQueryImpl(time_out,repo_addr,operater,node_type,name,ip);
    };

    VersionQueryTask(int time_out,
                    const string& repo_addr,
                    const MID &operater,
                    const string& cluster_name,
                    const string& cluster_ip,
                    const string& hostname,                    
                    const string& host_ip):
    Worktodo(cluster_name+"_"+hostname)
    {
        _query_impl= new VersionQueryImpl(time_out,repo_addr,operater,cluster_name,cluster_ip,hostname,host_ip);
    }; 

    ~VersionQueryTask()
    {
        delete _query_impl;
    }; 

    STATUS virtual Execute()
    {
        if (NULL == _query_impl)
        {
            return ERROR_NO_MEMORY;
        }
        return _query_impl->Execute();
    }
private:
    VersionQueryImpl *_query_impl;
};

class VersionUpgradeImpl:public VersionPopen
{
public:
    VersionUpgradeImpl(const string& repo_addr,
                    const string& cluster_name,
                    const string& cluster_ip,
                    const string& hostname,                    
                    const string& host_ip,
                    const string& package):        
        _repo_addr(repo_addr),
        _cluster_name(cluster_name),
        _cluster_ip(cluster_ip),
        _host_name(hostname),
        _host_ip(host_ip),
        _begin_time(time(0))
    {
         _percent=0;
        _return_code=TECS_ERR_OPRUNNING;        
        _package=package;
        _oid =++_oid_count;
        InitCmd();
    };
    STATUS Execute();
    static STATUS StaticInit()
    {
        InitStepPercent();
        InitOid();

        return SUCCESS;
    }
    void InsertData();
    int64 GetOid()
    {
        return _oid;
    }
    STATUS Cancel()
    {
        //与excute的数据互斥，不能在excute中再次写
        SetCancel();
        return SUCCESS;
    }
private:
    VersionUpgradeImpl()
    {};
    static int64               _oid_count;
    static vector<string>      _upgrade_step;    
    static map<string,int>     _step_percent;   
    static void InitStepPercent();
    static STATUS InitOid();
    void GetResult();
    void UpdateData(bool is_finish=false);    
    STATUS ParseError();
    STATUS ParseVersion();
    bool ParsePercent();
    STATUS ProcessData();

    bool GetPercent()
    {   
        return GetSubStr("yum upgrade percent:",":","%",_percent);
    }
    
    STATUS InitCmd()
    {
        _command= "/opt/tecs/tc/scripts/cloudcmd.sh -v hc " + 
                  _cluster_ip + " " + _host_ip + " -f " + 
                  "/opt/tecs/tc/scripts/upgradetecs.sh " + 
                  " upgrade " + _repo_addr + " " + _package;
        return SUCCESS;
    }
      
    int64               _oid;
    string              _repo_addr;
    string              _cluster_name;
    string              _cluster_ip;
    string              _host_name;
    string              _host_ip;
    string              _package;
    
    string              _repo_version;
    string              _old_version;
    Datetime            _begin_time;
    Datetime            _end_time;
    int                 _percent;
    int                 _return_code;
    string              _error_string;
    string              _command;

    DISALLOW_COPY_AND_ASSIGN(VersionUpgradeImpl);
};

class VersionUpgradeTask:public Worktodo
{
public:
    VersionUpgradeTask(const string& repo_addr,
                    const string& cluster_name,
                    const string& cluster_ip,
                    const string& hostname,                    
                    const string& host_ip,
                    const string& package):
    Worktodo(cluster_name + "_" +hostname + "_"+package)
    {
        _upgrade_impl= new VersionUpgradeImpl(repo_addr,cluster_name,cluster_ip,hostname,host_ip,package);
    }; 

    ~VersionUpgradeTask()
    {
        delete _upgrade_impl;
    }; 
    
    int64 GetOid()
    {
        return _upgrade_impl->GetOid();
    }
    void InsertData()
    {
        _upgrade_impl->InsertData();
    }

    STATUS virtual Execute()
    {
        if (NULL == _upgrade_impl)
        {
            return ERROR_NO_MEMORY;
        }
        return _upgrade_impl->Execute();
    }
    STATUS virtual Cancel()
    {
        if (NULL == _upgrade_impl)
        {
            return ERROR_NO_MEMORY;
        }
        return _upgrade_impl->Cancel();
    }
private:
    VersionUpgradeImpl *_upgrade_impl;
};

} // namespace zte_tecs

#endif //end TC_VERSION_TASK_H

