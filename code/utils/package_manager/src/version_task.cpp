/******************************************************************************
* Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：version_task.cpp
* 文件标识：
* 内容摘要：版本管理任务管理相关的实现
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

#include "version_task.h"
#include "tecs_errcode.h"
#include "tecs_config.h"
#include "event.h"
#include "mid.h"
#include "log_agent.h"
#include "pool_sql.h" 

namespace zte_tecs
{
static int version_task_print_on = 0;
DEFINE_DEBUG_VAR(version_task_print_on);
#define Debug(fmt,arg...) \
                do \
                { \
                    if(version_task_print_on) \
                    { \
                            OutPut(SYS_DEBUG,fmt,##arg); \
                    } \
                }while(0)

vector<string>  VersionUpgradeImpl::_upgrade_step;
map<string,int>  VersionUpgradeImpl::_step_percent;                   
int64 VersionUpgradeImpl:: _oid_count   = 0;
string VersionQueryImpl::_cloudcmd      = "/opt/tecs/tc/scripts/cloudcmd.sh ";
string VersionQueryImpl::_cloudupgrade  = "/opt/tecs/tc/scripts/upgradetecs.sh ";

void VersionQueryImpl::SndQueryResult()
{
    MessageVersionQueryAck msg(_retcode,
                        _detail,
                        _node_type,
                        _cloud_name,
                        _cloud_ip,
                        _cluster_name,
                        _cluster_ip,
                        _storage_adapter_name,
                        _storage_adapter_ip,
                        _host_name,
                        _host_ip,                       
                        _versions);

    MessageUnit temp_mu(TempUnitName("ack query"));
    temp_mu.Register();

    MessageOption option(_operater, EV_VERSION_QUERY_ACK, 0, 0);
    temp_mu.Send(msg,option);
    vector<VersionInfo>::const_iterator it;
    for(it = _versions.begin(); it != _versions.end(); it++)
    {
        Debug("%s:%s %s\n",it->_package.c_str(),it->_repo_version.c_str(),it->_local_version.c_str());
    }

    return;
}

STATUS VersionQueryImpl::ParseError()
{   
    string::size_type pos;
 
    //下面主要是升级时出现的一些错误，发现这些错误，则升级动作立即停止
    //todo:后面可能还有一些不可以预测的错误，目前还没有办法一一处理
    //只能到时试具体的情况而定
    pos = _detail.find( "Your input error:",_data_pos);
    if( pos != string::npos )
    {
        return ERROR;
    }

    pos = _detail.find( "Fail to run",_data_pos);
    if( pos != string::npos )
    {
        return ERROR;
    }


    //如果通过检查，则说明目前的步骤正常
    return SUCCESS;
} 

STATUS VersionQueryImpl::ProcessData()
{
    STATUS ret=ParseError();
    if (SUCCESS != ret)
    {
        OutPut(SYS_EMERGENCY,"Version query task fail:%u!\n",ret);
        return ret;
    }

    if (GetSubStr("repo_version:","{","}",_repo_version))
    {
        Debug("get repo version success:%s\n",_repo_version.c_str());
    }
    if (GetSubStr("local_version:","{","}",_old_version))
    {
        Debug("get local version success:%s\n",_old_version.c_str());
    }
    if (GetSubStr("repo_common_version:","{","}",_repo_common_version))
    {
        Debug("get repo common version success:%s\n",_repo_common_version.c_str());
    }
    if (GetSubStr("local_common_version:","{","}",_old_common_version))
    {
        Debug("get local common version success:%s\n",_old_common_version.c_str());
    }

    return SUCCESS;
}

STATUS VersionQueryImpl::Execute()
{
    STATUS ret=ExecutePopen(_command);
    if (SUCCESS != ret)
    {
        OutPut(SYS_EMERGENCY,"Version excute query task fail!\n");
        SndQueryResult();
        return ret;
    }

    VersionInfo node_package("tecs-"+_node_type,_repo_version,_old_version);
    _versions.push_back(node_package);

    VersionInfo common_package("tecs-common",_repo_common_version,_old_common_version);

    _versions.push_back(common_package);

    SndQueryResult();
    return SUCCESS;
}

void VersionUpgradeImpl::UpdateData(bool is_finish)
{

    if (is_finish)
    {
        Datetime end(time(0));
        _end_time = end;
        _percent=100;
        GetResult();
    }
    else if (_percent >= 100)
    {
        _percent=99; 
    }

    UpgradeLog 
    log(_oid,
        _cluster_name + "_" +_host_name,
        "hc",
        _package,
        _repo_version,
        _old_version,
        _begin_time,
        _end_time,
        _percent,
        _return_code,
        _error_string,
        _detail);
    log.Update(GetDB());

    return;
}

void VersionUpgradeImpl::InsertData()
{
    UpgradeLog 
    log(_oid,
        _cluster_name + "_" +_host_name,
        "hc",
        _package,
        _repo_version,
        _old_version,
        _begin_time,
        _end_time,
        _percent,
        _return_code,
        _error_string,
        _detail);

    log.Insert(GetDB());
    Debug("Version succeed to insert %s task!\n",_host_name.c_str());

    return;
}

STATUS VersionUpgradeImpl::InitOid()
{
    SqlCallbackable sql(GetDB());
    sql.SelectColumn(UpgradeLog::_table, "MAX(oid)","",VersionUpgradeImpl ::_oid_count);
    
    return SUCCESS;
}

void VersionUpgradeImpl::GetResult()
{
    /*转换成数据库中的错误码与字符串*/
    switch (_retcode)
    {
        case ERROR_NOT_READY:
        {
            _error_string="unkown error";
            _return_code=TECS_ERR_UNKNOWN;                
        }
        break;
        case ERROR:
        {
            _error_string="fail";
             _return_code=TECS_ERROR; 
        }
        break;
        case SUCCESS:
        {
            _error_string=="success";
             _return_code=TECS_SUCCESS; 
        }
        break;
        case ERROR_PROCESS_WILL_EXIT:
        {
            _error_string="cancel operation";
             _return_code=TECS_ERR_VERSION_CANCEL_OPERATION; 
        }
        break;
        case ERROR_NO_UPDATE_PACKAGE:
        {
            _error_string="there is no upgraded package";
             _return_code=TECS_ERR_VERSION_NO_UPGRADE_PACKAGE; 
        }
        break;
        case ERROR_NO_INSTALLED_PACKAGE:
        {
            _error_string="there is no installed package";
             _return_code=TECS_ERR_VERSION_NO_INSTALLED_PACKAGE; 
        }
        break;
        case ERROR_POPEN_FAIL:
        {
            _error_string="fail to start operation";
             _return_code=TECS_ERR_VERSION_START_OPERATION_FAIL; 
        }
        break;
        case ERROR_POPEN_SELECT_FAIL:
        {
            _error_string="the operation is abnormal";
             _return_code=TECS_ERR_VERSION_OPERATION_ABNORMAL_EXIT; 
        }
        break;
        case ERROR_TIME_OUT:
        {
            _error_string="the operation time out";
            _return_code=TECS_ERR_TIMEOUT; 
        }
        break;
        case ERROR_OP_RUNNING:
        {
            _error_string="the operation is running";
            _return_code=TECS_ERR_OPRUNNING; 
        }
        break;
        default:
        {
            _error_string="unkown error";
             _return_code=TECS_ERR_UNKNOWN; 
        }
        break;
    }
}

STATUS VersionUpgradeImpl::ParseError()
{   
    string::size_type pos;
 

    //下面主要是升级时出现的一些错误，发现这些错误，则升级动作立即停止
    //todo:后面可能还有一些不可以预测的错误，目前还没有办法一一处理
    //只能到时试具体的情况而定

    pos = _detail.find( "Your input error:",_data_pos);
    if( pos != string::npos )
    {
        return ERROR;
    }

    pos = _detail.find( "Fail to run",_data_pos);
    if( pos != string::npos )
    {
        return ERROR;
    }

    //如果开始到没有数据进行升级，则后面的步骤也不需要再做
    pos = _detail.find( "local_version:{no installed version}",0);
    //cout << _data_pos<< ":"<<_data<<endl;
    if( pos != string::npos )
    {
        return ERROR_NO_INSTALLED_PACKAGE;
    }

    //如果开始到没有数据进行升级，则后面的步骤也不需要再做
    pos = _detail.find( "repo_version:{no upgraded version}",0);
    printf("");
    if( pos != string::npos )
    {
        return ERROR_NO_UPDATE_PACKAGE;
    }

    pos = _detail.find( "No package upgrade available",0);
    if( pos != string::npos )
    {
        return ERROR_NO_UPDATE_PACKAGE;
    }

    //如果通过检查，则说明目前的步骤正常
    return SUCCESS;
} 

void VersionUpgradeImpl::InitStepPercent()
{
    _upgrade_step.clear();
    _step_percent.clear();
    //升级过程大致分为检查，下载，校验，安装
    _upgrade_step.push_back("Setting up Upgrade Process");
    _upgrade_step.push_back("Dependencies Resolved");
    _upgrade_step.push_back("Total download size");
    _upgrade_step.push_back("Downloading Packages");
    _upgrade_step.push_back("Transaction Test Succeeded");
    _upgrade_step.push_back("Running Transaction");
    _upgrade_step.push_back("Complete!");

    //为检查，下载，安装每一步分配大致的完成百分比
    //todo:将来如果有依赖包一起进行升级，则目前的简单算法是有问题的!!!!!!!
    _step_percent.insert(make_pair("Setting up Upgrade Process",2));
    _step_percent.insert(make_pair("Dependencies Resolved",2));
    _step_percent.insert(make_pair("Total download size",2));
    _step_percent.insert(make_pair("Downloading Packages",10));
    _step_percent.insert(make_pair("Transaction Test Succeeded",2));
    _step_percent.insert(make_pair("Running Transaction",10));
    _step_percent.insert(make_pair("Complete!",5));
}

bool VersionUpgradeImpl::ParsePercent()
{
    //任务的进度百分比，分为可控的，则通过脚本直接返回显示百分比
    //任务的进度不可控，则需要解析一定的字段，才能知道任务进行到什么程度
    //查询进行到哪一步，做出相应的百分比份额的增加
    vector<string>::iterator it = _upgrade_step.begin();
    for ( ; it != _upgrade_step.end(); ++it)
    {
        string::size_type pos;
        pos = _detail.find(*it,_data_pos);
        if( pos == string::npos)
        {
            return false;
        }
        _data_pos=pos;
        _percent += _step_percent[*it];      
    }
    //如果没有查询到进度标签，则返回false,
    //提示上层不需要更新数据库，减少数据库的操作
    if (it == _upgrade_step.end())
    {
        return false;
    }
    
    return true;
}

STATUS VersionUpgradeImpl::ParseVersion()
{
    if (GetSubStr("repo_version:","{","}",_repo_version))
    {
        Debug("get repo version success\n");
    }
    if (GetSubStr("local_version:","{","}",_old_version))
    {
        //放在这里赋值的原因是
        //由于解析是一直存在，所以只有真正查到了，才把原值赋给老的。
        //否则，老的值，在解析过程中，立马被新的值覆盖，新老值完全一样。
        Debug("get local version success\n");
    }
    /*if (_local_version == "no version")
    {
        return ERROR_NO_LOCAL_PACKAGE;
    }*/
    return SUCCESS;
}

STATUS VersionUpgradeImpl::ProcessData()
{
    ParseVersion();

    //如果发现错误，则执行的popen任务会被kill,比再被执行
    STATUS ret=ParseError();
    if (SUCCESS != ret)
    {
        UpdateData();
        return ret;
    }
    //解析通过脚本直接返回显示百分比
    if (GetPercent())
    {   
        //每次的解析过程都需要去更新数据库
        UpdateData();
        //此错误码，表示任务在执行中，不算错误
        return ERROR_OP_RUNNING;
    }

    //任务的进度不可控，则需要解析一定的字段，才能知道任务进行到什么程度
    //如果没有解析到，说明进度标签还没有到达，暂时不更新数据库
    if (ParsePercent())
    {
        UpdateData();
    }
    //UpdateData();
    ParseVersion();
    return ERROR_OP_RUNNING;
}

STATUS VersionUpgradeImpl::Execute()
{
    STATUS ret=SUCCESS;

    //任务执行开始，进度百分比开始设置
    _percent=1;
    UpdateData();

    //下面有任何脚本执行失败，则不再向执行
    ret = ExecutePopen(_command);
    if (SUCCESS != ret)
    {
        UpdateData(true);
        return ret;
    }

    UpdateData(true);
    return SUCCESS;
}

} // namespace zte_tecs

