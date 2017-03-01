/******************************************************************************
* Copyright (c) 2012������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�version_task.cpp
* �ļ���ʶ��
* ����ժҪ���汾�������������ص�ʵ��
* ��ǰ�汾��1.0
* ��    �ߣ�zhangyb
* ������ڣ�2012��9��26��
*
* �޸ļ�¼1��
*     �޸����ڣ�2012��9��26��
*     �� �� �ţ�V1.0
*     �� �� �ˣ�zhangyb
*     �޸����ݣ�����
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
 
    //������Ҫ������ʱ���ֵ�һЩ���󣬷�����Щ������������������ֹͣ
    //todo:������ܻ���һЩ������Ԥ��Ĵ���Ŀǰ��û�а취һһ����
    //ֻ�ܵ�ʱ�Ծ�����������
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


    //���ͨ����飬��˵��Ŀǰ�Ĳ�������
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
    /*ת�������ݿ��еĴ��������ַ���*/
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
 

    //������Ҫ������ʱ���ֵ�һЩ���󣬷�����Щ������������������ֹͣ
    //todo:������ܻ���һЩ������Ԥ��Ĵ���Ŀǰ��û�а취һһ����
    //ֻ�ܵ�ʱ�Ծ�����������

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

    //�����ʼ��û�����ݽ��������������Ĳ���Ҳ����Ҫ����
    pos = _detail.find( "local_version:{no installed version}",0);
    //cout << _data_pos<< ":"<<_data<<endl;
    if( pos != string::npos )
    {
        return ERROR_NO_INSTALLED_PACKAGE;
    }

    //�����ʼ��û�����ݽ��������������Ĳ���Ҳ����Ҫ����
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

    //���ͨ����飬��˵��Ŀǰ�Ĳ�������
    return SUCCESS;
} 

void VersionUpgradeImpl::InitStepPercent()
{
    _upgrade_step.clear();
    _step_percent.clear();
    //�������̴��·�Ϊ��飬���أ�У�飬��װ
    _upgrade_step.push_back("Setting up Upgrade Process");
    _upgrade_step.push_back("Dependencies Resolved");
    _upgrade_step.push_back("Total download size");
    _upgrade_step.push_back("Downloading Packages");
    _upgrade_step.push_back("Transaction Test Succeeded");
    _upgrade_step.push_back("Running Transaction");
    _upgrade_step.push_back("Complete!");

    //Ϊ��飬���أ���װÿһ��������µ���ɰٷֱ�
    //todo:���������������һ�������������Ŀǰ�ļ��㷨���������!!!!!!!
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
    //����Ľ��Ȱٷֱȣ���Ϊ�ɿصģ���ͨ���ű�ֱ�ӷ�����ʾ�ٷֱ�
    //����Ľ��Ȳ��ɿأ�����Ҫ����һ�����ֶΣ�����֪��������е�ʲô�̶�
    //��ѯ���е���һ����������Ӧ�İٷֱȷݶ������
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
    //���û�в�ѯ�����ȱ�ǩ���򷵻�false,
    //��ʾ�ϲ㲻��Ҫ�������ݿ⣬�������ݿ�Ĳ���
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
        //�������︳ֵ��ԭ����
        //���ڽ�����һֱ���ڣ�����ֻ�������鵽�ˣ��Ű�ԭֵ�����ϵġ�
        //�����ϵ�ֵ���ڽ��������У������µ�ֵ���ǣ�����ֵ��ȫһ����
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

    //������ִ�����ִ�е�popen����ᱻkill,���ٱ�ִ��
    STATUS ret=ParseError();
    if (SUCCESS != ret)
    {
        UpdateData();
        return ret;
    }
    //����ͨ���ű�ֱ�ӷ�����ʾ�ٷֱ�
    if (GetPercent())
    {   
        //ÿ�εĽ������̶���Ҫȥ�������ݿ�
        UpdateData();
        //�˴����룬��ʾ������ִ���У��������
        return ERROR_OP_RUNNING;
    }

    //����Ľ��Ȳ��ɿأ�����Ҫ����һ�����ֶΣ�����֪��������е�ʲô�̶�
    //���û�н�������˵�����ȱ�ǩ��û�е����ʱ���������ݿ�
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

    //����ִ�п�ʼ�����Ȱٷֱȿ�ʼ����
    _percent=1;
    UpdateData();

    //�������κνű�ִ��ʧ�ܣ�������ִ��
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

