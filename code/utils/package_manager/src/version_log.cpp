/******************************************************************************
* Copyright (c) 2012������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�version_log.cpp
* �ļ���ʶ��
* ����ժҪ���汾��������ʱ��־����
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

#include "version_log.h"
#include "pool_object_sql.h"
#include "pool_sql.h"

namespace zte_tecs
{
const char * UpgradeLog::_table     = "upgrade_log";
const char * UpgradeLog::_col_names = "oid,name,ip_address,node_type,package,repo_version,old_version,local_version,state,percent,result,data";

int UpgradeLog::Insert(SqlDB * db)
{
    int rc;

    SqlCommand sql(db,_table);
    sql.Add("oid", _oid);
    sql.Add("node_name", _node_name); 
    sql.Add("node_type", _node_type);
    sql.Add("package", _package);
    sql.Add("repo_version", _repo_version);
    sql.Add("old_version", _old_version);
    sql.Add("begin_time", _begin_time);
    sql.Add("end_time", _end_time);
    sql.Add("percent",  _percent);
    sql.Add("result", _return_code);
    sql.Add("error_string", _error_string); 
    sql.Add("detail", _detail);
    rc=sql.Insert();
    if(0 != rc)
    {
        SkyAssert(false);
        return rc;
    }
    return rc;
}

int UpgradeLog::Update(SqlDB * db)
{
    int rc;

    SqlCommand sql(db,_table);
    sql.Add("node_name", _node_name); 
    sql.Add("node_type", _node_type);
    sql.Add("package", _package);
    sql.Add("repo_version", _repo_version);
    sql.Add("old_version", _old_version);
    sql.Add("begin_time", _begin_time);
    sql.Add("end_time", _end_time);
    sql.Add("percent",  _percent);
    sql.Add("result", _return_code); 
    sql.Add("error_string", _error_string); 
    sql.Add("detail", _detail);
    ostringstream oss;
    oss << "WHERE oid = " << _oid;
    rc = sql.Update(oss.str());
    
    //rc = sql.Update("WHERE name = " + _name + " and " + "ip_address= "+_ip_address);
    if(0 != rc)
    {
        SkyAssert(false);
        return rc;
    }
    return rc;
}

}


