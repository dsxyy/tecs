/******************************************************************************
* Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：version_log.h
* 文件标识：
* 内容摘要：版本管理升级时日志处理头文件
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

#ifndef TC_VERSION_UPGRADE_LOG_H
#define TC_VERSION_UPGRADE_LOG_H
#include "sky.h"
#include "pool_object_sql.h"

namespace zte_tecs
{

class UpgradeLog: public Serializable
{        
public:
    UpgradeLog(int64 oid,
            const string &node_name,
            const string &node_type,
            const string &package,
            const string &repo_version,
            const string &old_version,
            const Datetime&begin_time,
            const Datetime &end_time,
            int percent,
            int &return_code,
            const string &error_string,
            const string &detail):
      _oid(oid),
     _node_name(node_name),
     _node_type(node_type),
     _package(package),
     _repo_version(repo_version),
     _old_version(old_version),
     _begin_time(begin_time),
     _end_time(end_time),
     _percent(percent),
     _return_code(return_code),
     _error_string(error_string),
     _detail(detail)
    {
    }        
    
    SERIALIZE 
    {
        SERIALIZE_BEGIN(UpgradeLog);
        WRITE_VALUE(_oid);
        WRITE_VALUE(_node_name);
        WRITE_VALUE(_node_type);
        WRITE_VALUE(_package);
        WRITE_VALUE(_repo_version);
        WRITE_VALUE(_old_version);
        WRITE_OBJECT(_begin_time);           
        WRITE_OBJECT(_end_time);
        WRITE_VALUE(_percent);
        WRITE_VALUE(_return_code);
        WRITE_VALUE(_error_string);
        WRITE_VALUE(_detail);
        SERIALIZE_END();    
    };
    
    DESERIALIZE 
    {
        DESERIALIZE_BEGIN(UpgradeLog);
        READ_VALUE(_oid);
        READ_VALUE(_node_name);
        READ_VALUE(_node_type);
        READ_VALUE(_package);
        READ_VALUE(_repo_version);
        READ_VALUE(_old_version);
        READ_OBJECT(_begin_time);
        READ_OBJECT(_end_time);
        READ_VALUE(_percent);
        READ_VALUE(_return_code);
        READ_VALUE(_error_string);
        READ_VALUE(_detail);
        DESERIALIZE_END();
    };

    int Insert(SqlDB * db);
    
    int Update(SqlDB * db);
    
    enum ColNames 
    {
        OID             = 0,
        NODE_NAME       = 1,
        NODE_TYPE       = 2,
        PACKAGE         = 3,
        REPO_VERSION    = 4,
        OLD_VERSION     = 5,
        BEGIN_TIME      = 6,
        END_TIME        = 7,
        PERCENT         = 8,
        RETURN_CODE     = 9,
        ERROR_STRING    = 10,
        DETAIL          = 11,
        LIMIT           = 12
    };
    
    static const char * _table;
    static const char * _col_names;
    int64               _oid;
    string              _node_name;
    string              _node_type;
    string              _package;
    string              _repo_version;
    string              _old_version;
    Datetime            _begin_time;
    Datetime            _end_time;
    int                 _percent;
    int                 _return_code;
    string              _error_string;
    string              _detail;
};

}

#endif /* end TC_VERSION_UPGRADE_LOG_H */

