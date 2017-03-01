/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：sc_db.cpp
* 文件标识：
* 内容摘要：sc数据库接口实现文件
* 当前版本：1.0
* 作    者：颜伟
* 完成日期：2012年09月27日
*
* 修改记录1：
*     修改日期：2012/9/27
*     版 本 号：V1.0
*     修 改 人：颜伟
*     修改内容：创建
*******************************************************************************/
#include "sc_db.h"
#include "errcode.h"
#include "log_agent.h"

namespace zte_tecs {

/******************************************************************************/  
const char * StorageAdaptorAddressPool::_table = 
        "storage_adaptor_address";

const char * StorageAdaptorAddressPool::_col_names = 
        "application,"
        "ip_address";

StorageAdaptorAddressPool *StorageAdaptorAddressPool::_instance = NULL;

/******************************************************************************/
int StorageAdaptorAddressPool::Allocate(const StorageAdaptorAddress &var)
{
    SqlCommand sql(_db, _table, SqlCommand::WITH_TRANSACTION);
    
    sql.Add("application", var._application);
    sql.Add("ip_address", var._ip_address);

    if (0 != sql.Insert())
    {
        return ERROR_DB_INSERT_FAIL;
    }

    return SUCCESS;
}

/******************************************************************************/
int StorageAdaptorAddressPool::Drop(const StorageAdaptorAddress  &var)
{
    SqlCommand sql(_db, _table, SqlCommand::WITH_TRANSACTION);

    int rc = sql.Delete(" WHERE application = '" + var._application+"'");
    if (0 != rc && SQLDB_RESULT_EMPTY != rc)
    {
        return ERROR_DB_DELETE_FAIL;
    }
    return SUCCESS;
}

/******************************************************************************/
int StorageAdaptorAddressPool::Update(const StorageAdaptorAddress &var)
{
    SqlCommand sql(_db, _table, SqlCommand::WITH_TRANSACTION);
    
    sql.Add("ip_address",  var._ip_address);
    
    int rc = sql.Update(" WHERE application = '" + var._application+"'");
    if (0 != rc)
    {
        return ERROR_DB_UPDATE_FAIL;
    }
    return SUCCESS;
}

/******************************************************************************/
int StorageAdaptorAddressPool::Select(vector<StorageAdaptorAddress> &vec_var,const string &where)
{
    ostringstream   oss;

    SetCallback(static_cast<Callbackable::Callback>(&StorageAdaptorAddressPool::SelectCallback),
                static_cast<void *> (&vec_var));

    oss << "SELECT " << _col_names << 
           " FROM " << _table << 
           " WHERE  " << where;
 
    int rc = _db->Execute(oss, this);

    UnsetCallback();
    if ( (SQLDB_OK != rc) && (SQLDB_RESULT_EMPTY != rc)  )
    {
        //数据库系统出错
        SkyAssert(false);
        return ERROR_DB_SELECT_FAIL;
    } 
  
    return SUCCESS;
}

/******************************************************************************/
int StorageAdaptorAddressPool::SelectCallback(void * nil, SqlColumn * columns)
{
    if (NULL == columns ||          
        LIMIT != columns->get_column_num())    
    {        
        return -1;   
    }   

    vector<StorageAdaptorAddress> *p_vector;

    p_vector = static_cast<vector<StorageAdaptorAddress> *>(nil);

    StorageAdaptorAddress var;
    columns->GetValue(APPLICATION,var._application);
    columns->GetValue(IP_ADDRESS, var._ip_address);

    p_vector->push_back(var);

    return 0;
}

int StorageAdaptorAddressPool::GetIpByAppName (const string &app_name, string & ip)
{

    int ret;
    ostringstream where; 
    where << " application = '"<<app_name<<"' ";
    SqlCallbackable sqlcb(_db);  
    ret = sqlcb.SelectColumn(_table, "ip_address", where.str(),ip);
    if (SQLDB_ERROR == ret)
    {
        OutPut(SYS_ERROR, "Get ip from storage_adaptor_address err\n");
        return ERROR_DB_SELECT_FAIL;
    }   
    if(SQLDB_RESULT_EMPTY==ret)
    {
        return ERROR_OBJECT_NOT_EXIST;
    }
    return SUCCESS;
}

/******************************************************************************/  
const char * StorageAdaptorPool::_table = 
        "storage_adaptor";

const char * StorageAdaptorPool::_col_names = 
        "sid,"
        "name,"
        "application,"
        "ctrl_address,"
        "type,"
        "is_online,"
        "enabled,"
        "description,"
        "register_time,"
        "refresh_time";
        
StorageAdaptorPool *StorageAdaptorPool::_instance = NULL;

int64 StorageAdaptorPool::_lastSID = INVALID_SID;

/******************************************************************************/
/* 初始化：获取max oid                                                                    */
/******************************************************************************/
int StorageAdaptorPool::Init()
{
    string  column  = " MAX(sid) ";
    string  where   = " 1=1 ";
    int64   tmp_sid = INVALID_SID;

    // 获取最大的oid
    SqlCallbackable sqlcb(_db);
    int rc = sqlcb.SelectColumn(_table,
                            column,
                            where,
                            tmp_sid);

    if((SQLDB_OK != rc)
       &&(SQLDB_RESULT_EMPTY != rc))
    { 
        OutPut(SYS_ERROR, "*** Select record from StorageAdaptorPool failed %d ***\n",rc);
        SkyAssert(false);
        return ERROR_DB_SELECT_FAIL;
    }

    _lastSID = tmp_sid;

    if (SUCCESS != _mutex.Init())
    {
        OutPut(SYS_ERROR, "storage_adaptor mutex init failed\n");
        SkyAssert(false);
        return ERROR;
    }
    
    return SUCCESS;
}

/******************************************************************************/
int StorageAdaptorPool::Allocate(StorageAdaptor &var)
{
    MutexLock lock(_mutex);
    SqlCommand sql(_db, _table, SqlCommand::WITH_TRANSACTION);

    var._sid = ++_lastSID;
    sql.Add("sid",          var._sid);
    sql.Add("name",         var._name);
    sql.Add("application",  var._application);
    sql.Add("ctrl_address", var._ctrl_address);
    sql.Add("type",         var._type);
    sql.Add("is_online",    var._is_online);
    sql.Add("enabled",      var._enabled);
    sql.Add("description",  var._description);
    sql.Add("register_time",var._register_time);
    sql.Add("refresh_time", var._refresh_time);

    if (0 != sql.Insert())
    {
        --_lastSID;
        return ERROR_DB_INSERT_FAIL;
    }

    return SUCCESS;
}

/******************************************************************************/
int StorageAdaptorPool::Drop(const StorageAdaptor  &var)
{
    SqlCommand sql(_db, _table, SqlCommand::WITH_TRANSACTION);

    int rc = sql.Delete(" WHERE sid = " + to_string<int64>(var._sid,dec));
    if (0 != rc && SQLDB_RESULT_EMPTY != rc)
    {
        return ERROR_DB_DELETE_FAIL;
    }
    return SUCCESS;
}

/******************************************************************************/
int StorageAdaptorPool::Update(const StorageAdaptor &var)
{
    SqlCommand sql(_db, _table, SqlCommand::WITH_TRANSACTION);
    
    sql.Add("name",         var._name);
    sql.Add("application",  var._application);
    sql.Add("ctrl_address", var._ctrl_address);
    sql.Add("type",         var._type);
    sql.Add("is_online",    var._is_online);
    sql.Add("enabled",      var._enabled);
    sql.Add("description",  var._description);
    //sql.Add("register_time",var._register_time);
    sql.Add("refresh_time", var._refresh_time);
    
    int rc = sql.Update(" WHERE sid = " + to_string<int64>(var._sid,dec));
    if (0 != rc)
    {
        return ERROR_DB_UPDATE_FAIL;
    }
    return SUCCESS;
}

/******************************************************************************/
int StorageAdaptorPool::Select(vector<StorageAdaptor> &vec_var,const string &where)
{
    ostringstream   oss;

    SetCallback(static_cast<Callbackable::Callback>(&StorageAdaptorPool::SelectCallback),
                static_cast<void *> (&vec_var));

    oss << "SELECT " << _col_names << 
           " FROM " << _table << 
           " WHERE  " << where;

    int rc = _db->Execute(oss, this);

    UnsetCallback();
    if ( (SQLDB_OK != rc) && (SQLDB_RESULT_EMPTY != rc)  )
    {
        //数据库系统出错
        SkyAssert(false);
        return ERROR_DB_SELECT_FAIL;
    } 
  
    return SUCCESS;
}

/******************************************************************************/
int StorageAdaptorPool::SelectCallback(void * nil, SqlColumn * columns)
{
    if (NULL == columns ||          
        LIMIT != columns->get_column_num())    
    {        
        return -1;   
    }   

    vector<StorageAdaptor> *p_vector;

    p_vector = static_cast<vector<StorageAdaptor> *>(nil);

    StorageAdaptor var;
    
    columns->GetValue(SID,           var._sid);
    columns->GetValue(NAME,          var._name);
    columns->GetValue(APPLICATION,   var._application);
    columns->GetValue(CTRL_ADDRESS,  var._ctrl_address);
    columns->GetValue(TYPE,          var._type);
    columns->GetValue(IS_ONLINE,     var._is_online);
    columns->GetValue(ENABLED,       var._enabled);
    columns->GetValue(DESCRIPTION,   var._description);
    columns->GetValue(REGISTER_TIME, var._register_time);
    columns->GetValue(REFRESH_TIME,  var._refresh_time);
    
    p_vector->push_back(var);

    return 0;
}
/******************************************************************************/  
void StorageAdaptorPool::PrintLastId(void)
{
    cout<<"lastSID:"<<_lastSID<<endl;
}
/******************************************************************************/  
const char * StorageAddressPool::_table = 
        "storage_address";

const char * StorageAddressPool::_col_names = 
        "sid,"
        "media_address";

StorageAddressPool *StorageAddressPool::_instance = NULL;

/******************************************************************************/
int StorageAddressPool::Allocate(const StorageAddress &var)
{
    SqlCommand sql(_db, _table, SqlCommand::WITH_TRANSACTION);
    
    sql.Add("sid", var._sid);
    sql.Add("media_address", var._media_address);

    if (0 != sql.Insert())
    {
        return ERROR_DB_INSERT_FAIL;
    }

    return SUCCESS;
}

/******************************************************************************/
int StorageAddressPool::Drop(const StorageAddress  &var)
{
    SqlCommand sql(_db, _table, SqlCommand::WITH_TRANSACTION);

    int rc = sql.Delete(" WHERE (sid = " + to_string<int64>(var._sid,dec)
                   +") and (media_address = '" + var._media_address +"')");
    if (0 != rc && SQLDB_RESULT_EMPTY != rc)
    {
        return ERROR_DB_DELETE_FAIL;
    }
    return SUCCESS;
}

/******************************************************************************/
int StorageAddressPool::Select(vector<StorageAddress> &vec_var,const string &where)
{
    ostringstream   oss;

    SetCallback(static_cast<Callbackable::Callback>(&StorageAddressPool::SelectCallback),
                static_cast<void *> (&vec_var));

    oss << "SELECT " << _col_names << 
           " FROM " << _table << 
           " WHERE  " << where;
 
    int rc = _db->Execute(oss, this);

    UnsetCallback();
    if ( (SQLDB_OK != rc) && (SQLDB_RESULT_EMPTY != rc)  )
    {
        //数据库系统出错
        SkyAssert(false);
        return ERROR_DB_SELECT_FAIL;
    } 
  
    return SUCCESS;
}

/******************************************************************************/
int StorageAddressPool::SelectCallback(void * nil, SqlColumn * columns)
{
    if (NULL == columns ||          
        LIMIT != columns->get_column_num())    
    {        
        return -1;   
    }   

    vector<StorageAddress> *p_vector;

    p_vector = static_cast<vector<StorageAddress> *>(nil);

    StorageAddress var;
    columns->GetValue(SID,           var._sid);
    columns->GetValue(MEDIA_ADDRESS, var._media_address);

    p_vector->push_back(var);

    return 0;
}

/******************************************************************************/  
const char * StorageVgPool::_table = 
        "storage_vg";

const char * StorageVgPool::_col_names = 
        "sid,"
        "name,"
        "size";

StorageVgPool *StorageVgPool::_instance = NULL;

/******************************************************************************/
int StorageVgPool::Allocate(const StorageVg &var)
{
    SqlCommand sql(_db, _table, SqlCommand::WITH_TRANSACTION);
    
    sql.Add("sid", var._sid);
    sql.Add("name", var._name);
    sql.Add("size", var._size);

    if (0 != sql.Insert())
    {
        return ERROR_DB_INSERT_FAIL;
    }

    return SUCCESS;
}

/******************************************************************************/
int StorageVgPool::Drop(const StorageVg  &var)
{
    SqlCommand sql(_db, _table, SqlCommand::WITH_TRANSACTION);

    int rc = sql.Delete(" WHERE (sid = " + to_string<int64>(var._sid,dec)
                   +") and (name = '" + var._name +"')");
    if (0 != rc && SQLDB_RESULT_EMPTY != rc)
    {
        return ERROR_DB_DELETE_FAIL;
    }
    return SUCCESS;
}

/******************************************************************************/
int StorageVgPool::Update(const StorageVg &var)
{
    SqlCommand sql(_db, _table, SqlCommand::WITH_TRANSACTION);
    
    sql.Add("size",  var._size);
    
    int rc = sql.Update(" WHERE (sid = " + to_string<int64>(var._sid,dec)
                   +") and (name = '" + var._name +"')");
    if (0 != rc)
    {
        return ERROR_DB_UPDATE_FAIL;
    }
    return SUCCESS;
}

/******************************************************************************/
int StorageVgPool::Select(vector<StorageVg> &vec_var,const string &where)
{
    ostringstream   oss;

    SetCallback(static_cast<Callbackable::Callback>(&StorageVgPool::SelectCallback),
                static_cast<void *> (&vec_var));

    oss << "SELECT " << _col_names << 
           " FROM " << _table << 
           " WHERE  " << where;
 
    int rc = _db->Execute(oss, this);

    UnsetCallback();
    if ( (SQLDB_OK != rc) && (SQLDB_RESULT_EMPTY != rc)  )
    {
        //数据库系统出错
        SkyAssert(false);
        return ERROR_DB_SELECT_FAIL;
    } 
  
    return SUCCESS;
}

/******************************************************************************/
int StorageVgPool::SelectCallback(void * nil, SqlColumn * columns)
{
    if (NULL == columns ||          
        LIMIT != columns->get_column_num())    
    {        
        return -1;   
    }   

    vector<StorageVg> *p_vector;

    p_vector = static_cast<vector<StorageVg> *>(nil);

    StorageVg var;
    columns->GetValue(SID,  var._sid);
    columns->GetValue(NAME, var._name);
    columns->GetValue(SIZE, var._size);
         
    p_vector->push_back(var);

    return 0;
}

/******************************************************************************/  
const char * StorageTargetPool::_table = 
        "storage_target";

const char * StorageTargetPool::_col_names = 
        "sid,"
        "name,"
        "target_id";

StorageTargetPool *StorageTargetPool::_instance = NULL;

map<int64,int64> StorageTargetPool::_lastID;
    
/******************************************************************************/
/* 初始化：获取max id                                                                    */
/******************************************************************************/
int StorageTargetPool::Init()
{
    string  column  = " sid, MAX(target_id) ";
    string  groupby = " GROUP BY sid ";
    ostringstream   oss;
    ostringstream   oss1;

    SetCallback(static_cast<Callbackable::Callback>(&StorageTargetPool::SelectLastIdCallback),NULL);

    oss << "SELECT " << column << 
           " FROM " << _table << groupby;
 
    int rc = _db->Execute(oss, this);
    
    UnsetCallback();
    
    if ( (SQLDB_OK != rc) && (SQLDB_RESULT_EMPTY != rc)  )
    {
        //数据库系统出错
        SkyAssert(false);
        return ERROR_DB_SELECT_FAIL;
    }   

    SetCallback(static_cast<Callbackable::Callback>(&StorageTargetPool::SelectLastIdCallback),NULL);

    oss1 << "SELECT " << column << 
           " FROM " << "storage_target_action_view" << groupby;
 
    rc = _db->Execute(oss1, this);

    UnsetCallback();
    if ( (SQLDB_OK != rc) && (SQLDB_RESULT_EMPTY != rc)  )
    {
        //数据库系统出错
        SkyAssert(false);
        return ERROR_DB_SELECT_FAIL;
    } 
    
    if (SUCCESS != _mutex.Init())
    {
        OutPut(SYS_ERROR, "storage_target mutex init failed\n");
        SkyAssert(false);
        return ERROR;
    }
    
    return SUCCESS;

}

/******************************************************************************/
int StorageTargetPool::SelectLastIdCallback(void * nil, SqlColumn * columns)
{
    if (NULL == columns ||          
        2 != columns->get_column_num())    
    {        
        return -1;   
    }   

    int64 sid,max;
    columns->GetValue(0, sid);
    columns->GetValue(1, max);
    
    map<int64,int64>::iterator iter = _lastID.find(sid);
    if( iter != _lastID.end() ) 
    {
        if(iter->second < max)
        {
            iter->second=max;
        }        
    }
    else
    {
        _lastID.insert(make_pair(sid,max));
    }
    return 0;
}

/******************************************************************************/
int StorageTargetPool::Allocate(const StorageTarget &var)
{
    SqlCommand sql(_db, _table, SqlCommand::WITH_TRANSACTION);

    sql.Add("sid",  var._sid);
    sql.Add("name", var._name);
    sql.Add("target_id", var._target_id);

    if (0 != sql.Insert())
    {
        return ERROR_DB_INSERT_FAIL;
    }
    return SUCCESS;
}

/******************************************************************************/
int StorageTargetPool::Drop(const StorageTarget  &var)
{
    SqlCommand sql(_db, _table, SqlCommand::WITH_TRANSACTION);

    int rc = sql.Delete(" WHERE (sid = " + to_string<int64>(var._sid,dec)
                   +") and (name = '" + var._name +"')");
    if (0 != rc && SQLDB_RESULT_EMPTY != rc)
    {
        return ERROR_DB_DELETE_FAIL;
    }
    return SUCCESS;
}

/******************************************************************************/
int StorageTargetPool::Select(vector<StorageTarget> &vec_var,const string &where)
{
    ostringstream   oss;

    SetCallback(static_cast<Callbackable::Callback>(&StorageTargetPool::SelectCallback),
                static_cast<void *> (&vec_var));

    oss << "SELECT " << _col_names << 
           " FROM " << _table << 
           " WHERE  " << where;

    int rc = _db->Execute(oss, this);

    UnsetCallback();
    if ( (SQLDB_OK != rc) && (SQLDB_RESULT_EMPTY != rc)  )
    {
        //数据库系统出错
        SkyAssert(false);
        return ERROR_DB_SELECT_FAIL;
    } 
  
    return SUCCESS;
}

/******************************************************************************/
int StorageTargetPool::SelectCallback(void * nil, SqlColumn * columns)
{
    if (NULL == columns ||          
        LIMIT != columns->get_column_num())    
    {        
        return -1;   
    }   

    vector<StorageTarget> *p_vector;

    p_vector = static_cast<vector<StorageTarget> *>(nil);

    StorageTarget var;
    columns->GetValue(SID,  var._sid);
    columns->GetValue(NAME, var._name);
    columns->GetValue(TARGET_ID,  var._target_id);
    
    p_vector->push_back(var);

    return 0;
}

/******************************************************************************/  
void StorageTargetPool::PrintLastId(void)
{
    if (0 != _lastID.size())
    {
        map<int64,int64>::iterator it;

        for (it = _lastID.begin(); it != _lastID.end(); it++)
        {
            cout<<"sid:"<<it->first<<",lastId:"<<it->second<<endl;
        }
    }
    else
    {
        cout<<"no record!"<<endl;
    }
}

/******************************************************************************/  
const char * StorageAuthorizePool::_table = 
        "storage_authorize";

const char * StorageAuthorizePool::_col_names =  
        "sid,"
        "target_name,"
        "ip_address,"
        "iscsi_initiator,"
        "auth_uuid";

StorageAuthorizePool *StorageAuthorizePool::_instance = NULL;

/******************************************************************************/
int StorageAuthorizePool::Allocate(const StorageAuthorize &var)
{
    SqlCommand sql(_db, _table, SqlCommand::WITH_TRANSACTION);

    sql.Add("sid", var._sid);
    sql.Add("target_name", var._target_name);
    sql.Add("ip_address", var._ip_address);
    sql.Add("iscsi_initiator", var._iscsi_initiator);
    sql.Add("auth_uuid", var._auth_uuid);
    
    if (0 != sql.Insert())
    {
        return ERROR_DB_INSERT_FAIL;
    }

    return SUCCESS;
}

/******************************************************************************/
int StorageAuthorizePool::Drop(const StorageAuthorize  &var)
{
    SqlCommand sql(_db, _table, SqlCommand::WITH_TRANSACTION);

    int rc = sql.Delete(" WHERE (sid = " + to_string<int64>(var._sid,dec)
                   +") and (target_name = '" + var._target_name 
                   +"') and (ip_address = '" + var._ip_address
                   +"') and (iscsi_initiator = '" + var._iscsi_initiator + "')");
    if (0 != rc && SQLDB_RESULT_EMPTY != rc)
    {
        return ERROR_DB_DELETE_FAIL;
    }
    return SUCCESS;
}

/******************************************************************************/
int StorageAuthorizePool::DropByWhere(const string  &where)
{
    SqlCommand sql(_db, _table, SqlCommand::WITH_TRANSACTION);

    int rc = sql.Delete(" WHERE " + where);
    if (0 != rc && SQLDB_RESULT_EMPTY != rc)
    {
        return ERROR_DB_DELETE_FAIL;
    }
    return SUCCESS;
}

/******************************************************************************/
int StorageAuthorizePool::Select(vector<StorageAuthorize> &vec_var,const string &where)
{
    ostringstream   oss;

    SetCallback(static_cast<Callbackable::Callback>(&StorageAuthorizePool::SelectCallback),
                static_cast<void *> (&vec_var));

    oss << "SELECT " << _col_names << 
           " FROM " << _table << 
           " WHERE  " << where;

    int rc = _db->Execute(oss, this);

    UnsetCallback();
    if ( (SQLDB_OK != rc) && (SQLDB_RESULT_EMPTY != rc)  )
    {
        //数据库系统出错
        SkyAssert(false);
        return ERROR_DB_SELECT_FAIL;
    } 
  
    return SUCCESS;
}

/******************************************************************************/
int StorageAuthorizePool::SelectCallback(void * nil, SqlColumn * columns)
{
    if (NULL == columns ||          
        LIMIT != columns->get_column_num())    
    {        
        return -1;   
    }   

    vector<StorageAuthorize> *p_vector;

    p_vector = static_cast<vector<StorageAuthorize> *>(nil);

    StorageAuthorize var;
    columns->GetValue(SID, var._sid);
    columns->GetValue(TARGET_NAME, var._target_name);
    columns->GetValue(IP_ADDRESS, var._ip_address);
    columns->GetValue(ISCSI_INITIATOR, var._iscsi_initiator);
    columns->GetValue(AUTH_UUID, var._auth_uuid);
    
    p_vector->push_back(var);

    return 0;
}

/******************************************************************************/  
const char * StorageClusterPool::_table = 
        "storage_cluster";

const char * StorageClusterPool::_col_names = 
        "sid,"
        "cluster_name";

StorageClusterPool *StorageClusterPool::_instance = NULL;

/******************************************************************************/
int StorageClusterPool::Allocate(const StorageCluster &var)
{
    SqlCommand sql(_db, _table, SqlCommand::WITH_TRANSACTION);
    
    sql.Add("sid", var._sid);
    sql.Add("cluster_name", var._cluster_name);

    if (0 != sql.Insert())
    {
        return ERROR_DB_INSERT_FAIL;
    }

    return SUCCESS;
}

/******************************************************************************/
int StorageClusterPool::Drop(const StorageCluster  &var)
{
    SqlCommand sql(_db, _table, SqlCommand::WITH_TRANSACTION);

    int rc = sql.Delete(" WHERE (sid = " + to_string<int64>(var._sid,dec)
                   +") and (cluster_name = '" + var._cluster_name +"')");
    if (0 != rc && SQLDB_RESULT_EMPTY != rc)
    {
        return ERROR_DB_DELETE_FAIL;
    }
    return SUCCESS;
}

/******************************************************************************/
int StorageClusterPool::Select(vector<StorageCluster> &vec_var,const string &where)
{
    ostringstream   oss;

    SetCallback(static_cast<Callbackable::Callback>(&StorageClusterPool::SelectCallback),
                static_cast<void *> (&vec_var));

    oss << "SELECT " << _col_names << 
           " FROM " << _table << 
           " WHERE  " << where;

    int rc = _db->Execute(oss, this);

    UnsetCallback();
    if ( (SQLDB_OK != rc) && (SQLDB_RESULT_EMPTY != rc)  )
    {
        //数据库系统出错
        SkyAssert(false);
        return ERROR_DB_SELECT_FAIL;
    } 
  
    return SUCCESS;
}

/******************************************************************************/
int StorageClusterPool::SelectCallback(void * nil, SqlColumn * columns)
{
    if (NULL == columns ||          
        LIMIT != columns->get_column_num())    
    {        
        return -1;   
    }   

    vector<StorageCluster> *p_vector;

    p_vector = static_cast<vector<StorageCluster> *>(nil);

    StorageCluster var;
    columns->GetValue(SID,          var._sid);
    columns->GetValue(CLUSTER_NAME, var._cluster_name);
         
    p_vector->push_back(var);

    return 0;
}

/******************************************************************************/  
StorageVolumePool * StorageVolumePool::_instance = NULL;

int64 StorageVolumePool::_lastID = INVALID_OID;

const char * StorageVolumePool::_table = 
        "storage_volume";

const char * StorageVolumePool::_col_names = 
        "sid,"
        "vg_name,"
        "name,"
        "size,"
        "volume_id,"
        "volume_uuid,"
        "request_id";
/******************************************************************************/
/* 初始化：获取max oid                                                                    */
/******************************************************************************/
int StorageVolumePool::Init()
{
    string  column  = " MAX(volume_id) ";
    string  where   = " 1=1 ";
    int64   tmp_oid = INVALID_OID;

    // 获取最大的oid
    SqlCallbackable sqlcb(_db);
    int rc = sqlcb.SelectColumn(_table,
                            column,
                            where,
                            tmp_oid);

    if((SQLDB_OK != rc)
       &&(SQLDB_RESULT_EMPTY != rc))
    { 
        OutPut(SYS_ERROR, "*** Select record from storage_volume failed %d ***\n",rc);
        SkyAssert(false);
        return ERROR_DB_SELECT_FAIL;
    }

    _lastID = tmp_oid;
    tmp_oid=0;
    rc = sqlcb.SelectColumn("storage_volume_action_view",
                            column,
                            where,
                            tmp_oid);

    if((SQLDB_OK != rc)
       &&(SQLDB_RESULT_EMPTY != rc))
    { 
        OutPut(SYS_ERROR, "*** Select record from storage_volume failed %d ***\n",rc);
        SkyAssert(false);
        return ERROR_DB_SELECT_FAIL;
    }

    if(_lastID<tmp_oid)
    {
        _lastID=tmp_oid;
    }
    
    if (SUCCESS != _mutex.Init())
    {
        OutPut(SYS_ERROR, "storage_volume mutex init failed\n");
        SkyAssert(false);
        return ERROR;
    }
    
    return SUCCESS;
}

/******************************************************************************/
int StorageVolumePool::Allocate(const StorageVolume &var)
{
    SqlCommand sql(_db, _table, SqlCommand::WITH_TRANSACTION);
    
    sql.Add("sid", var._sid);
    sql.Add("vg_name", var._vg_name);    
    sql.Add("name", var._name); 
    sql.Add("size", var._size); 
    sql.Add("volume_id", var._volume_id);
    sql.Add("volume_uuid", var._volume_uuid);
    sql.Add("request_id", var._request_id);    

    if (0 != sql.Insert())
    {
        return ERROR_DB_INSERT_FAIL;
    }

    return SUCCESS;
}

/******************************************************************************/
int StorageVolumePool::Drop(const StorageVolume  &var)
{
    SqlCommand sql(_db, _table, SqlCommand::WITH_TRANSACTION);

    int rc = sql.Delete(" WHERE (sid = " + to_string<int64>(var._sid,dec)
                  + ") and (vg_name = '" + var._vg_name
                  + "') and (name = '" + var._name+"')");
    if (0 != rc && SQLDB_RESULT_EMPTY != rc)
    {
        return ERROR_DB_DELETE_FAIL;
    }
    return SUCCESS;
}

/******************************************************************************/
int StorageVolumePool::Drop(const string &where)
{
    SqlCommand sql(_db, _table, SqlCommand::WITH_TRANSACTION);

    int rc = sql.Delete(where);
    if (0 != rc && SQLDB_RESULT_EMPTY != rc)
    {
        return ERROR_DB_DELETE_FAIL;
    }
    return SUCCESS;
}

/******************************************************************************/
int StorageVolumePool::Update(const StorageVolume &var)
{
    SqlCommand sql(_db, _table, SqlCommand::WITH_TRANSACTION);
    
    sql.Add("size",  var._size);
    sql.Add("volume_uuid", var._volume_uuid);
    
    int rc = sql.Update(" WHERE (sid = " + to_string<int64>(var._sid,dec)
                    + ") and (vg_name = '" + var._vg_name
                    + "') and (name = '" + var._name+"')");
    if (0 != rc)
    {
        return ERROR_DB_UPDATE_FAIL;
    }
    return SUCCESS;
}

/******************************************************************************/
int StorageVolumePool::UpdateUuid(const StorageVolume &var)
{
    SqlCommand sql(_db, _table, SqlCommand::WITH_TRANSACTION);
    
    sql.Add("volume_uuid", var._volume_uuid);
    
    int rc = sql.Update(" WHERE (sid = " + to_string<int64>(var._sid,dec)
                    + ") and (vg_name = '" + var._vg_name
                    + "') and (name = '" + var._name+"')");
    if (0 != rc)
    {
        return ERROR_DB_UPDATE_FAIL;
    }
    return SUCCESS;
}

/******************************************************************************/
int StorageVolumePool::Select(vector<StorageVolume> &vec_var,const string &where)
{
    ostringstream   oss;

    SetCallback(static_cast<Callbackable::Callback>(&StorageVolumePool::SelectCallback),
                static_cast<void *> (&vec_var));

    oss << "SELECT " << _col_names << 
           " FROM " << _table << 
           " WHERE  " << where;

    int rc = _db->Execute(oss, this);

    UnsetCallback();
    if ( (SQLDB_OK != rc) && (SQLDB_RESULT_EMPTY != rc)  )
    {
        //数据库系统出错
        SkyAssert(false);
        return ERROR_DB_SELECT_FAIL;
    } 
  
    return SUCCESS;
}

/******************************************************************************/
int StorageVolumePool::SelectCallback(void * nil, SqlColumn * columns)
{
    if (NULL == columns ||          
        LIMIT != columns->get_column_num())    
    {        
        return -1;   
    }   

    vector<StorageVolume> *p_vector;

    p_vector = static_cast<vector<StorageVolume> *>(nil);

    StorageVolume var;
    columns->GetValue(SID,         var._sid);
    columns->GetValue(VG_NAME,     var._vg_name);
    columns->GetValue(NAME,        var._name);
    columns->GetValue(SIZE,        var._size);
    columns->GetValue(VOLUME_ID,   var._volume_id);
    columns->GetValue(VOLUME_UUID, var._volume_uuid);
    columns->GetValue(REQUEST_ID,  var._request_id);

    p_vector->push_back(var);

    return 0;
}
/******************************************************************************/  
void StorageVolumePool::PrintLastId(void)
{
    cout<<"lastVolumeID:"<<_lastID<<endl;
}

/******************************************************************************/  
int StorageVolumePool::GetVolNumOfVg(int sid, const string& vgname,int& volnum)
{
    string  column  = " COUNT(vg_name) ";
    string where = "(sid = "+to_string<int64>(sid,dec)
                    +") and (vg_name = '"+vgname+"')";       

    int tmp_num;
    SqlCallbackable sqlcb(_db);

    int rc = sqlcb.SelectColumn(_table,
        column,
        where,
        tmp_num);
    if (SQLDB_OK != rc)
    {
        OutPut(SYS_ERROR,"GetVolNumOfVg err: rc\n",rc);
        SkyAssert(false);
        return ERROR_DB_SELECT_FAIL;
    }
    volnum=tmp_num;
    return SUCCESS;
    
}

/******************************************************************************/  
const char * StorageLunPool::_table = 
        "storage_lun";

const char * StorageLunPool::_col_names = 
        "sid,"
        "target_name,"
        "lun_id,"
        "lun_uuid,"
        "volume_uuid";

StorageLunPool *StorageLunPool::_instance = NULL;
/******************************************************************************/
int StorageLunPool::Init()
{   
    if (SUCCESS != _mutex.Init())
    {
        OutPut(SYS_ERROR, "storage_lun mutex init failed\n");
        SkyAssert(false);
        return ERROR;
    }
    
    return SUCCESS;
}
/******************************************************************************/
int StorageLunPool::Allocate(const StorageLun &var)
{
    SqlCommand sql(_db, _table, SqlCommand::WITH_TRANSACTION);
    
    sql.Add("sid", var._sid);
    sql.Add("target_name", var._target_name);
    sql.Add("lun_id", var._lun_id);
    sql.Add("lun_uuid", var._lun_uuid);
    sql.Add("volume_uuid", var._volume_uuid);
    
    if (0 != sql.Insert())
    {
        return ERROR_DB_INSERT_FAIL;
    }

    return SUCCESS;
}

/******************************************************************************/
int StorageLunPool::Drop(const StorageLun  &var)
{
    SqlCommand sql(_db, _table, SqlCommand::WITH_TRANSACTION);

    int rc = sql.Delete(" WHERE (sid = " + to_string<int64>(var._sid,dec)
                   +") and (target_name = '" + var._target_name 
                   +"') and (lun_id = " + to_string<int64>(var._lun_id,dec)
                   +")");
    if (0 != rc && SQLDB_RESULT_EMPTY != rc)
    {
        return ERROR_DB_DELETE_FAIL;
    }
    return SUCCESS;
}
/******************************************************************************/
int StorageLunPool::UpdateLunUuid(const StorageLun  &var)
{
    SqlCommand sql(_db, _table, SqlCommand::WITH_TRANSACTION);
    
    sql.Add("lun_uuid", var._lun_uuid);
    
    int rc = sql.Update(" WHERE (sid = " + to_string<int64>(var._sid,dec)
                    + ") and (target_name = '" + var._target_name
                    + "') and (lun_id = " + to_string<int64>(var._lun_id,dec) + ")");
    if (0 != rc)
    {
        return ERROR_DB_UPDATE_FAIL;
    }
    return SUCCESS;
}
/******************************************************************************/
int StorageLunPool::Select(vector<StorageLun> &vec_var,const string &where)
{
    ostringstream   oss;

    SetCallback(static_cast<Callbackable::Callback>(&StorageLunPool::SelectCallback),
                static_cast<void *> (&vec_var));

    oss << "SELECT " << _col_names << 
           " FROM " << _table << 
           " WHERE " << where;

    int rc = _db->Execute(oss, this);

    UnsetCallback();
    if ((SQLDB_OK != rc) && (SQLDB_RESULT_EMPTY != rc))
    {
        //数据库系统出错
        SkyAssert(false);
        return ERROR_DB_SELECT_FAIL;
    } 
  
    return SUCCESS;
}

/******************************************************************************/
int StorageLunPool::SelectCallback(void * nil, SqlColumn * columns)
{
    if (NULL == columns ||          
        LIMIT != columns->get_column_num())    
    {        
        return -1;   
    }   

    vector<StorageLun> *p_vector;

    p_vector = static_cast<vector<StorageLun> *>(nil);

    StorageLun var;
    columns->GetValue(SID,         var._sid);
    columns->GetValue(TARGET_NAME, var._target_name);
    columns->GetValue(LUN_ID,      var._lun_id);
    columns->GetValue(LUN_UUID,    var._lun_uuid);
    columns->GetValue(VOLUME_UUID, var._volume_uuid);
         
    p_vector->push_back(var);

    return 0;
}
/******************************************************************************/ 
int StorageLunPool::AllocateFreeLunid(StorageLun  &var)
{
    MutexLock lock(_mutex);
    string where = "(sid = "+to_string<int64>(var._sid,dec)
                  +") and (target_name = '"+var._target_name+"')";
    vector<StorageLun> vec_sl;
    int rc = ERROR;
    rc = Select(vec_sl,where);
    if (SUCCESS != rc)
    {
        SkyAssert(false);
        return rc;
    }

    int tmp_lunid = 0;
    bool bfind=false;
    int lastid=0;
    if (0 != vec_sl.size())
    {
        vector<StorageLun>::iterator it_vec;
        list<int>lunid_list;
        for (it_vec = vec_sl.begin(); it_vec != vec_sl.end(); it_vec++)
        {
            lunid_list.push_back(it_vec->_lun_id);
        }
        DbGetActionLunid(lunid_list,var._sid,var._target_name);        
        map<string,int>::iterator itmap = _last_mapid.find(var._target_name);
        if( itmap == _last_mapid.end() ) 
        {       
            lunid_list.sort();
            list<int>::reverse_iterator rit=lunid_list.rbegin();
            _last_mapid[var._target_name]=*rit;
        }
        lastid=_last_mapid[var._target_name];
        
        list<int>::iterator it_list;                
        int iLoop;
        
        for (iLoop = lastid+1; iLoop < lastid+1+MAX_STORAGE_LUN_ID; iLoop++)
        {
            tmp_lunid=iLoop%MAX_STORAGE_LUN_ID;
            it_list=find(lunid_list.begin(),lunid_list.end(),tmp_lunid);
            if(it_list==lunid_list.end())
            {
                bfind=true;
                break;
            }
        } 
        
    } 
    else
    {
        map<string,int>::iterator itmap = _last_mapid.find(var._target_name);
        if( itmap != _last_mapid.end() )
        {            
            lastid=_last_mapid[var._target_name];
            tmp_lunid=(lastid+1)%MAX_STORAGE_LUN_ID;
        }
        
        bfind=true;
    }
    
    if (!bfind)
    {
        SkyAssert(false);
        return ERROR_NO_SPACE;
    }
    var._lun_id = tmp_lunid; 
    _last_mapid[var._target_name]=tmp_lunid;

    return SUCCESS;
}

/******************************************************************************/ 
int StorageLunPool::ScanLunid(StorageLun  &var)
{
    MutexLock lock(_mutex);
    string where = "(sid = "+to_string<int64>(var._sid,dec)
                  +") and (target_name = '"+var._target_name+"')";
    vector<StorageLun> vec_sl;
    int rc = ERROR;
    rc = Select(vec_sl,where);
    if (SUCCESS != rc)
    {
        SkyAssert(false);
        return rc;
    }    
    if (0 != vec_sl.size())
    {
        vector<StorageLun>::iterator it_vec;
        list<int>lunid_list;
        for (it_vec = vec_sl.begin(); it_vec != vec_sl.end(); it_vec++)
        {
            lunid_list.push_back(it_vec->_lun_id);
        }
        DbGetActionLunid(lunid_list,var._sid,var._target_name);        
        map<string,int>::iterator itmap = _last_mapid.find(var._target_name);
        if( itmap == _last_mapid.end() ) 
        {       
            lunid_list.sort();
            list<int>::reverse_iterator rit=lunid_list.rbegin();
            _last_mapid[var._target_name]=*rit;
        }            
    }
    return SUCCESS;
}

/******************************************************************************/ 
int StorageLunPool::GetLastLunid(string target_name,int &lun_id)
{
    int lastid=0;
    lun_id=0;
    map<string,int>::iterator itmap = _last_mapid.find(target_name);
    if( itmap != _last_mapid.end() )
    {            
        lastid=_last_mapid[target_name];
        lun_id=(lastid)%MAX_STORAGE_LUN_ID;
    }
    return SUCCESS;
}

/******************************************************************************/  
const char * StorageAuthorizeLunPool::_table = 
        "storage_authorize_lun";

const char * StorageAuthorizeLunPool::_col_names = 
        "auth_uuid,"
        "lun_uuid";

StorageAuthorizeLunPool *StorageAuthorizeLunPool::_instance = NULL;

/******************************************************************************/
int StorageAuthorizeLunPool::Allocate(const StorageAuthorizeLun &var)
{
    SqlCommand sql(_db, _table, SqlCommand::WITH_TRANSACTION);
    
    sql.Add("auth_uuid", var._auth_uuid);
    sql.Add("lun_uuid", var._lun_uuid);
    
    if (0 != sql.Insert())
    {
        return ERROR_DB_INSERT_FAIL;
    }

    return SUCCESS;
}

/******************************************************************************/
int StorageAuthorizeLunPool::Drop(const StorageAuthorizeLun  &var)
{
    SqlCommand sql(_db, _table, SqlCommand::WITH_TRANSACTION);

    int rc = sql.Delete(" WHERE (auth_uuid = '" + var._auth_uuid
                   +"') and (lun_uuid = '" + var._lun_uuid + "')");
    
    if (0 != rc && SQLDB_RESULT_EMPTY != rc)
    {
        return ERROR_DB_DELETE_FAIL;
    }
    return SUCCESS;
}
/******************************************************************************/
int StorageAuthorizeLunPool::DropByWhere(const string &where)
{
    SqlCommand sql(_db, _table, SqlCommand::WITH_TRANSACTION);

    int rc = sql.Delete(" WHERE " + where);
    
    if (0 != rc && SQLDB_RESULT_EMPTY != rc)
    {
        return ERROR_DB_DELETE_FAIL;
    }
    return SUCCESS;
}
/******************************************************************************/
int StorageAuthorizeLunPool::Select(vector<StorageAuthorizeLun> &vec_var,const string &where)
{
    ostringstream   oss;

    SetCallback(static_cast<Callbackable::Callback>(&StorageAuthorizeLunPool::SelectCallback),
                static_cast<void *> (&vec_var));

    oss << "SELECT " << _col_names << 
           " FROM " << _table << 
           " WHERE " << where;

    int rc = _db->Execute(oss, this);

    UnsetCallback();
    if ((SQLDB_OK != rc) && (SQLDB_RESULT_EMPTY != rc))
    {
        //数据库系统出错
        SkyAssert(false);
        return ERROR_DB_SELECT_FAIL;
    } 
  
    return SUCCESS;
}

/******************************************************************************/
int StorageAuthorizeLunPool::SelectCallback(void * nil, SqlColumn * columns)
{
    if (NULL == columns ||          
        LIMIT != columns->get_column_num())    
    {        
        return -1;   
    }   

    vector<StorageAuthorizeLun> *p_vector;

    p_vector = static_cast<vector<StorageAuthorizeLun> *>(nil);

    StorageAuthorizeLun var;
    columns->GetValue(AUTH_UUID, var._auth_uuid);
    columns->GetValue(LUN_UUID, var._lun_uuid);

    p_vector->push_back(var);

    return 0;
}

/******************************************************************************/  
const char * StorageUserVolumePool::_table = 
        "storage_user_volume";

const char * StorageUserVolumePool::_col_names = 
        "uid,"
        "request_id,"
        "usage,"
        "description";

StorageUserVolumePool *StorageUserVolumePool::_instance = NULL;

/******************************************************************************/
int StorageUserVolumePool::Allocate(const StorageUserVolume &var)
{
    SqlCommand sql(_db, _table, SqlCommand::WITH_TRANSACTION);
    
    sql.Add("uid", var._uid);
    sql.Add("request_id", var._request_id);
    sql.Add("usage", var._usage);
    sql.Add("description", var._description);
    
    if (0 != sql.Insert())
    {
        return ERROR_DB_INSERT_FAIL;
    }

    return SUCCESS;
}

/******************************************************************************/
int StorageUserVolumePool::Drop(const StorageUserVolume  &var)
{
    SqlCommand sql(_db, _table, SqlCommand::WITH_TRANSACTION);

    int rc = sql.Delete(" WHERE (uid = " + to_string<int64>(var._uid,dec)
                   +") and (request_id = '" + var._request_id + "')");
    
    if (0 != rc && SQLDB_RESULT_EMPTY != rc)
    {
        return ERROR_DB_DELETE_FAIL;
    }
    return SUCCESS;
}
/******************************************************************************/
int StorageUserVolumePool::DropByWhere(const string &where)
{
    SqlCommand sql(_db, _table, SqlCommand::WITH_TRANSACTION);

    int rc = sql.Delete(" WHERE " + where);
    
    if (0 != rc && SQLDB_RESULT_EMPTY != rc)
    {
        return ERROR_DB_DELETE_FAIL;
    }
    return SUCCESS;
}
/******************************************************************************/
int StorageUserVolumePool::UpdateUsage(const string &where, const string &usage)
{
    SqlCommand sql(_db, _table, SqlCommand::WITH_TRANSACTION);
    
    sql.Add("usage", usage);
    
    int rc = sql.Update(" WHERE " + where);
    if (0 != rc)
    {
        return ERROR_DB_UPDATE_FAIL;
    }
    return SUCCESS;
}
/******************************************************************************/
int StorageUserVolumePool::Select(vector<StorageUserVolume> &vec_var,const string &where)
{
    ostringstream   oss;

    SetCallback(static_cast<Callbackable::Callback>(&StorageUserVolumePool::SelectCallback),
                static_cast<void *> (&vec_var));

    oss << "SELECT " << _col_names << 
           " FROM " << _table << 
           " WHERE " << where;

    int rc = _db->Execute(oss, this);

    UnsetCallback();
    if ((SQLDB_OK != rc) && (SQLDB_RESULT_EMPTY != rc))
    {
        //数据库系统出错
        SkyAssert(false);
        return ERROR_DB_SELECT_FAIL;
    } 
  
    return SUCCESS;
}

/******************************************************************************/
int StorageUserVolumePool::SelectCallback(void * nil, SqlColumn * columns)
{
    if (NULL == columns ||          
        LIMIT != columns->get_column_num())    
    {        
        return -1;   
    }   
         
    vector<StorageUserVolume> *p_vector;

    p_vector = static_cast<vector<StorageUserVolume> *>(nil);

    StorageUserVolume var;
    columns->GetValue(UID, var._uid);
    columns->GetValue(REQUEST_ID, var._request_id);
    columns->GetValue(USAGE, var._usage);
    columns->GetValue(DESCRIPTION, var._description);
    
    p_vector->push_back(var);

    return 0;
}

/******************************************************************************/  
const char * StorageSnapShotPool::_table = 
        "storage_snapshot";

const char * StorageSnapShotPool::_col_names = 
        "base_uuid,"
        "snapshot_uuid";
      

StorageSnapShotPool *StorageSnapShotPool::_instance = NULL;

/******************************************************************************/
int StorageSnapShotPool::Allocate(const StorageSnapshot &var)
{
    SqlCommand sql(_db, _table, SqlCommand::WITH_TRANSACTION);
    
    sql.Add("base_uuid", var._base_uuid);
    sql.Add("snapshot_uuid", var._snap_uuid);
 
    
    if (0 != sql.Insert())
    {
        return ERROR_DB_INSERT_FAIL;
    }

    return SUCCESS;
}

/******************************************************************************/
int StorageSnapShotPool::Drop(const StorageSnapshot  &var)
{
    SqlCommand sql(_db, _table, SqlCommand::WITH_TRANSACTION);

    int rc = sql.Delete(" WHERE (snapshot_uuid = '" + var._snap_uuid+"')"); 

    
    if (0 != rc && SQLDB_RESULT_EMPTY != rc)
    {
        return ERROR_DB_DELETE_FAIL;
    }
    return SUCCESS;
}

int StorageSnapShotPool::GetSnapNum(const StorageSnapshot  &var,int& num)
{
    string  column  = " COUNT(snapshot_uuid) ";
    string where = " (base_uuid = '"+var._base_uuid+"')";       

    int tmp_num;
    SqlCallbackable sqlcb(_db);

    int rc = sqlcb.SelectColumn(_table,
        column,
        where,
        tmp_num);
    if (SQLDB_OK != rc)
    {
        OutPut(SYS_ERROR,"GetSnapNum err: rc\n",rc);
        SkyAssert(false);
        return ERROR_DB_SELECT_FAIL;
    }
    num=tmp_num;
    return SUCCESS;   
}

/******************************************************************************/
int StorageSnapShotPool::Select(vector<StorageSnapshot> &vec_var,const string &where)
{
    ostringstream   oss;

    SetCallback(static_cast<Callbackable::Callback>(&StorageSnapShotPool::SelectCallback),
                static_cast<void *> (&vec_var));

    oss << "SELECT " << _col_names << 
           " FROM " << _table << 
           " WHERE " << where;

    int rc = _db->Execute(oss, this);

    UnsetCallback();
    if ((SQLDB_OK != rc) && (SQLDB_RESULT_EMPTY != rc))
    {
        //数据库系统出错
        SkyAssert(false);
        return ERROR_DB_SELECT_FAIL;
    } 
  
    return SUCCESS;
}

/******************************************************************************/
int StorageSnapShotPool::SelectCallback(void * nil, SqlColumn * columns)
{
    if (NULL == columns ||          
        LIMIT != columns->get_column_num())    
    {        
        return -1;   
    }   
         
    vector<StorageSnapshot> *p_vector;

    p_vector = static_cast<vector<StorageSnapshot> *>(nil);

    StorageSnapshot var;
    columns->GetValue(BASE_UUID, var._base_uuid);
    columns->GetValue(SNAP_UUID, var._snap_uuid);

    
    p_vector->push_back(var);

    return 0;
}


/******************************************************************************/  
const char * StorageVolumeWithLunPool::_table = 
        "storage_volume_with_lun_view";

const char * StorageVolumeWithLunPool::_col_names = 
        "sid,"
        "vg_name,"
        "volume_name,"
        "volume_uuid,"
        "target_name,"
        "lun_id,"
        "lun_uuid";

StorageVolumeWithLunPool *StorageVolumeWithLunPool::_instance = NULL;
/******************************************************************************/
int StorageVolumeWithLunPool::Select(vector<StorageVolumeWithLun> &vec_var,const string &where)
{
    ostringstream   oss;

    SetCallback(static_cast<Callbackable::Callback>(&StorageVolumeWithLunPool::SelectCallback),
                static_cast<void *> (&vec_var));

    oss << "SELECT " << _col_names << 
           " FROM " << _table << 
           " WHERE " << where;

    int rc = _db->Execute(oss, this);

    UnsetCallback();
    if ( (SQLDB_OK != rc) && (SQLDB_RESULT_EMPTY != rc)  )
    {
        //数据库系统出错
        SkyAssert(false);
        return ERROR_DB_SELECT_FAIL;
    } 
  
    return SUCCESS;
}

/******************************************************************************/
int StorageVolumeWithLunPool::SelectCallback(void * nil, SqlColumn * columns)
{
    if (NULL == columns ||          
        LIMIT != columns->get_column_num())    
    {        
        return -1;   
    }   

    vector<StorageVolumeWithLun> *p_vector;

    p_vector = static_cast<vector<StorageVolumeWithLun> *>(nil);

    StorageVolumeWithLun var;
    columns->GetValue(SID, var._sid);
    columns->GetValue(VG_NAME, var._vg_name);
    columns->GetValue(VOLUME_NAME, var._volume_name);
    columns->GetValue(TARGET_NAME, var._target_name);
    columns->GetValue(LUN_ID, var._lun_id);
    
    p_vector->push_back(var);

    return 0;
}

/******************************************************************************/  
const char * StorageAdaptorWithVgAndClusterPool::_table = 
        "storage_adaptor_with_vg_and_cluster_view";

const char * StorageAdaptorWithVgAndClusterPool::_col_names = 
        "sid,"
        "cluster_name,"
        "application,"
        "unit,"
        "type,"
        "vg_name,"
        "sa_total_free_size,"
        "max_vg_free_size";

StorageAdaptorWithVgAndClusterPool *StorageAdaptorWithVgAndClusterPool::_instance = NULL;
    
/******************************************************************************/
int StorageAdaptorWithVgAndClusterPool::Select(vector<StorageAdaptorWithVgAndCluster> &vec_var,const string &where)
{
    ostringstream   oss;

    SetCallback(static_cast<Callbackable::Callback>(&StorageAdaptorWithVgAndClusterPool::SelectCallback),
                static_cast<void *> (&vec_var));

    oss << "SELECT " << _col_names << 
           " FROM " << _table << 
           " WHERE " << where;

    int rc = _db->Execute(oss, this);

    UnsetCallback();
    if ( (SQLDB_OK != rc) && (SQLDB_RESULT_EMPTY != rc)  )
    {
        //数据库系统出错
        SkyAssert(false);
        return ERROR_DB_SELECT_FAIL;
    } 
  
    return SUCCESS;
}

/******************************************************************************/
int StorageAdaptorWithVgAndClusterPool::SelectCallback(void * nil, SqlColumn * columns)
{
    if (NULL == columns ||          
        LIMIT != columns->get_column_num())    
    {        
        return -1;   
    }   

    vector<StorageAdaptorWithVgAndCluster> *p_vector;

    p_vector = static_cast<vector<StorageAdaptorWithVgAndCluster> *>(nil);

    StorageAdaptorWithVgAndCluster var;
    columns->GetValue(SID,          var._sid);
    columns->GetValue(CLUSTER_NAME, var._cluster_name);
    columns->GetValue(APPLICATION,  var._application);
    columns->GetValue(UNIT,         var._unit);
    columns->GetValue(TYPE,         var._type);
    columns->GetValue(VG_NAME,      var._vg_name);
    columns->GetValue(SA_TOTAL_FREE_SIZE, var._sa_total_free_size);
    columns->GetValue(MAX_VG_FREE_SIZE, var._max_vg_free_size);
    
    p_vector->push_back(var);

    return 0;
}


/******************************************************************************/  
const char * StorageAdaptorWithMultivgAndClusterPool::_table = 
        "storage_adaptor_with_multivg_and_cluster_view";

const char * StorageAdaptorWithMultivgAndClusterPool::_col_names = 
        "sid,"
        "cluster_name,"
        "application,"
        "unit,"
        "type,"
        "vg_name,"
        "sa_total_free_size,"
        "vg_free_size";

StorageAdaptorWithMultivgAndClusterPool *StorageAdaptorWithMultivgAndClusterPool::_instance = NULL;
    
/******************************************************************************/
int StorageAdaptorWithMultivgAndClusterPool::Select(vector<StorageAdaptorWithMultivgAndCluster> &vec_var,const string &where)
{
    ostringstream   oss;

    SetCallback(static_cast<Callbackable::Callback>(&StorageAdaptorWithMultivgAndClusterPool::SelectCallback),
                static_cast<void *> (&vec_var));

    oss << "SELECT " << _col_names << 
           " FROM " << _table << 
           " WHERE " << where;

    int rc = _db->Execute(oss, this);

    UnsetCallback();
    if ( (SQLDB_OK != rc) && (SQLDB_RESULT_EMPTY != rc)  )
    {
        //数据库系统出错
        SkyAssert(false);
        return ERROR_DB_SELECT_FAIL;
    } 
  
    return SUCCESS;
}

/******************************************************************************/
int StorageAdaptorWithMultivgAndClusterPool::SelectCallback(void * nil, SqlColumn * columns)
{
    if (NULL == columns ||          
        LIMIT != columns->get_column_num())    
    {        
        return -1;   
    }   

    vector<StorageAdaptorWithMultivgAndCluster> *p_vector;

    p_vector = static_cast<vector<StorageAdaptorWithMultivgAndCluster> *>(nil);

    StorageAdaptorWithMultivgAndCluster var;
    columns->GetValue(SID,          var._sid);
    columns->GetValue(CLUSTER_NAME, var._cluster_name);
    columns->GetValue(APPLICATION,  var._application);
    columns->GetValue(UNIT,         var._unit);
    columns->GetValue(TYPE,         var._type);
    columns->GetValue(VG_NAME,      var._vg_name);
    columns->GetValue(SA_TOTAL_FREE_SIZE, var._sa_total_free_size);
    columns->GetValue(VG_FREE_SIZE, var._vg_free_size);
    
    p_vector->push_back(var);

    return 0;
}


/******************************************************************************/  
const char * StorageVolumeWithLunAndAuthAndAdaptorPool::_table = 
        "storage_volume_with_lun_and_auth_and_adaptor_view";

const char * StorageVolumeWithLunAndAuthAndAdaptorPool::_col_names = 
        "vol_uuid,"
        "target_name,"
        "lun_id,"
        "ip_address,"
        "iscsi_initiator,"
        "application,"
        "unit,"
        "type";

StorageVolumeWithLunAndAuthAndAdaptorPool *StorageVolumeWithLunAndAuthAndAdaptorPool::_instance = NULL;

/******************************************************************************/
int StorageVolumeWithLunAndAuthAndAdaptorPool::Select(StorageVolumeWithLunAndAuthAndAdaptor &var,const string &where)
{
    ostringstream   oss;

    SetCallback(static_cast<Callbackable::Callback>(&StorageVolumeWithLunAndAuthAndAdaptorPool::SelectCallback),
                static_cast<void *> (&var));

    oss << "SELECT " << _col_names << 
           " FROM " << _table << 
           " WHERE " << where;

    int rc = _db->Execute(oss, this);

    UnsetCallback();
    if ( (SQLDB_OK != rc) && (SQLDB_RESULT_EMPTY != rc)  )
    {
        //数据库系统出错
        SkyAssert(false);
        return ERROR_DB_SELECT_FAIL;
    } 
    else if (SQLDB_RESULT_EMPTY == rc)
    {
        return ERROR_OBJECT_NOT_EXIST;
    }
    return SUCCESS;
}

/******************************************************************************/
int StorageVolumeWithLunAndAuthAndAdaptorPool::SelectCallback(void * nil, SqlColumn * columns)
{
    if (NULL == columns ||          
        LIMIT != columns->get_column_num())    
    {        
        return -1;   
    }   

    StorageVolumeWithLunAndAuthAndAdaptor *p_var;

    p_var = static_cast<StorageVolumeWithLunAndAuthAndAdaptor *>(nil);

    columns->GetValue(VOL_UUID,         p_var->_vol_uuid);
    columns->GetValue(TARGET_NAME,      p_var->_target_name);
    columns->GetValue(LUN_ID,           p_var->_lun_id);
    columns->GetValue(IP_ADDRESS,       p_var->_ip_address);
    columns->GetValue(ISCSI_INITIATIOR, p_var->_iscsi_initiator);
    columns->GetValue(APPLICATION,      p_var->_application);
    columns->GetValue(UNIT,             p_var->_unit);
    columns->GetValue(TYPE,             p_var->_type);
    
    return 0;
}


/******************************************************************************/
int StorageVolumeWithLunAndAuthAndAdaptorPool::GetLunHostMapNum(const string & target_name,const string &vol_uuid,const string &ip,const string & iscsi_initiator,int64 &num_lun,int64 &num_host)
{

    string  column  = " COUNT(ip_address) ";
    string  where = "(vol_uuid = '"+vol_uuid
                    +"')"+" and (target_name = '"+target_name+"')";
    int64   tmp_oid = INVALID_OID;    
    
    SqlCallbackable sqlcb(_db);              
    int rc = sqlcb.SelectColumn(_table,
                            column,
                            where,
                            tmp_oid);  
    num_host=tmp_oid;  
    
    column  = " COUNT(vol_uuid) ";
    where = "(iscsi_initiator = '"+iscsi_initiator
           +"')"+" and (target_name = '"+target_name+"')";
    
    rc = sqlcb.SelectColumn(_table,
                            column,
                            where,
                            tmp_oid);  
    num_lun=tmp_oid;
    return SUCCESS;
}

/******************************************************************************/
int StorageVolumeWithLunAndAuthAndAdaptorPool::GetTargetMapNum(const string & target_name,int64 &num)
{

    string  column  = " COUNT(vol_uuid) ";
    string  where = "(target_name = '"+target_name+"')";
    int64   tmp_oid = INVALID_OID;    
    
    SqlCallbackable sqlcb(_db);                     
   
    where = "(target_name = '"+target_name+"')";          
    
    int rc = sqlcb.SelectColumn(_table,
                            column,
                            where,
                            tmp_oid);  
    if (SQLDB_OK != rc)
    {
        OutPut(SYS_ERROR,"GetTargetMapNum err: rc\n",rc);
        SkyAssert(false);
        return ERROR_DB_SELECT_FAIL;
    }
    num=tmp_oid;
    return SUCCESS;
}

int StorageVolumeWithLunAndAuthAndAdaptorPool::GetLunMapNum(const string &vol_uuid,int64 &num)
{

    string  column  = " COUNT(vol_uuid) ";
    string  where = "(vol_uuid = '"+vol_uuid+"')";
    int64   tmp_oid = INVALID_OID;    
    
    SqlCallbackable sqlcb(_db);              
    int rc = sqlcb.SelectColumn(_table,
                            column,
                            where,
                            tmp_oid);
    if (SQLDB_OK != rc)
    {
        OutPut(SYS_ERROR,"GetLunMapNum err: rc\n",rc);
        SkyAssert(false);
        return ERROR_DB_SELECT_FAIL;
    }
    num = tmp_oid;
    return SUCCESS;
}

/******************************************************************************/  
const char * StorageVolumeWithLunAndAddressPool::_table = 
        "storage_volume_with_lun_and_address_view";

const char * StorageVolumeWithLunAndAddressPool::_col_names = 
        "vol_uuid,"
        "lun_id,"
        "media_address";

StorageVolumeWithLunAndAddressPool *StorageVolumeWithLunAndAddressPool::_instance = NULL;
    
/******************************************************************************/
int StorageVolumeWithLunAndAddressPool::Select(vector<StorageVolumeWithLunAndAddress> &vec_var,const string &where)
{
    ostringstream   oss;

    SetCallback(static_cast<Callbackable::Callback>(&StorageVolumeWithLunAndAddressPool::SelectCallback),
                static_cast<void *> (&vec_var));

    oss << "SELECT " << _col_names << 
           " FROM " << _table << 
           " WHERE " << where;

    int rc = _db->Execute(oss, this);

    UnsetCallback();
    if ( (SQLDB_OK != rc) && (SQLDB_RESULT_EMPTY != rc)  )
    {
        //数据库系统出错
        SkyAssert(false);
        return ERROR_DB_SELECT_FAIL;
    } 
  
    return SUCCESS;
}

/******************************************************************************/
int StorageVolumeWithLunAndAddressPool::SelectCallback(void * nil, SqlColumn * columns)
{
    if (NULL == columns ||          
        LIMIT != columns->get_column_num())    
    {        
        return -1;   
    }   

    vector<StorageVolumeWithLunAndAddress> *p_vector;

    p_vector = static_cast<vector<StorageVolumeWithLunAndAddress> *>(nil);

    StorageVolumeWithLunAndAddress var;
    columns->GetValue(VOL_UUID, var._vol_uuid);
    columns->GetValue(LUN_ID, var._lun_id);
    columns->GetValue(MEDIA_ADDRESS, var._media_address);
    
    p_vector->push_back(var);

    return 0;
}

/******************************************************************************/  
const char * StorageVolumeWithAdaptorPool::_table = 
        "storage_volume_with_adaptor_view";

const char * StorageVolumeWithAdaptorPool::_col_names =
        "vol_uuid,"
        "sid,"
        "vg_name,"
        "vol_name,"
        "application,"
        "unit,"
        "type";

StorageVolumeWithAdaptorPool *StorageVolumeWithAdaptorPool::_instance = NULL;
    
/******************************************************************************/
int StorageVolumeWithAdaptorPool::Select(vector<StorageVolumeWithAdaptor> &vec_var,const string &where)
{
    ostringstream   oss;

    SetCallback(static_cast<Callbackable::Callback>(&StorageVolumeWithAdaptorPool::SelectCallback),
                static_cast<void *> (&vec_var));

    oss << "SELECT " << _col_names << 
           " FROM " << _table << 
           " WHERE " << where;

    int rc = _db->Execute(oss, this);

    UnsetCallback();
    if ( (SQLDB_OK != rc) && (SQLDB_RESULT_EMPTY != rc)  )
    {
        //数据库系统出错
        SkyAssert(false);
        return ERROR_DB_SELECT_FAIL;
    } 
  
    return SUCCESS;
}

/******************************************************************************/
int StorageVolumeWithAdaptorPool::SelectCallback(void * nil, SqlColumn * columns)
{
    if (NULL == columns ||          
        LIMIT != columns->get_column_num())    
    {        
        return -1;   
    }   

    vector<StorageVolumeWithAdaptor> *p_vector;

    p_vector = static_cast<vector<StorageVolumeWithAdaptor> *>(nil);

    StorageVolumeWithAdaptor var;
    columns->GetValue(VOL_UUID,     var._vol_uuid);
    columns->GetValue(SID,          var._sid);
    columns->GetValue(VG_NAME,      var._vg_name);
    columns->GetValue(VOL_NAME,     var._vol_name);
    columns->GetValue(APPLICATION,  var._application);
    columns->GetValue(UNIT,         var._unit);
    columns->GetValue(TYPE,         var._type);

    p_vector->push_back(var);
    
    return 0;
}

/******************************************************************************/  
const char * StorageAuthorizeWithTargetIdPool::_table = 
        "storage_authorize_with_target_id_view";

const char * StorageAuthorizeWithTargetIdPool::_col_names =
        "target_id,"
        "sid,"
        "target_name,"
        "ip_address,"
        "iscsi_initiator,"
        "auth_uuid";

StorageAuthorizeWithTargetIdPool *StorageAuthorizeWithTargetIdPool::_instance = NULL;
    
/******************************************************************************/
int StorageAuthorizeWithTargetIdPool::Select(vector<StorageAuthorizeWithTargetId> &vec_var,const string &where)
{
    ostringstream   oss;

    SetCallback(static_cast<Callbackable::Callback>(&StorageAuthorizeWithTargetIdPool::SelectCallback),
                static_cast<void *> (&vec_var));

    oss << "SELECT " << _col_names << 
           " FROM " << _table << 
           " WHERE " << where;

    int rc = _db->Execute(oss, this);

    UnsetCallback();
    if ( (SQLDB_OK != rc) && (SQLDB_RESULT_EMPTY != rc)  )
    {
        //数据库系统出错
        SkyAssert(false);
        return ERROR_DB_SELECT_FAIL;
    } 
  
    return SUCCESS;
}

/******************************************************************************/
int StorageAuthorizeWithTargetIdPool::SelectCallback(void * nil, SqlColumn * columns)
{
    if (NULL == columns ||          
        LIMIT != columns->get_column_num())    
    {        
        return -1;   
    }   

    vector<StorageAuthorizeWithTargetId> *p_vector;

    p_vector = static_cast<vector<StorageAuthorizeWithTargetId> *>(nil);

    StorageAuthorizeWithTargetId var;
    columns->GetValue(TARGET_ID,       var._target_id);
    columns->GetValue(SID,             var._sid);
    columns->GetValue(TARGET_NAME,     var._target_name);
    columns->GetValue(IP_ADDRESS,      var._ip_address);
    columns->GetValue(ISCSI_INITIATOR, var._iscsi_initiator);
    columns->GetValue(AUTH_UUID,       var._auth_uuid);
    
    p_vector->push_back(var);
    
    return 0;
}

/******************************************************************************/ 
const char * StorageLunActionView::_table = 
        "storage_lun_action_view";

const char * StorageLunActionView::_col_names = 
        "sid,"
        "target_name,"
        "lunid";
      

StorageLunActionView *StorageLunActionView::_instance = NULL;
/******************************************************************************/
int StorageLunActionView::Select(list<int> &p_list,const string &where)
{
    ostringstream   oss;

    SetCallback(static_cast<Callbackable::Callback>(&StorageLunActionView::SelectCallback),
                static_cast<void *> (&p_list));

    oss << "SELECT " << _col_names << 
           " FROM " << _table << 
           " WHERE " << where;

    int rc = _db->Execute(oss, this);

    UnsetCallback();
    if ((SQLDB_OK != rc) && (SQLDB_RESULT_EMPTY != rc))
    {
        //数据库系统出错
        SkyAssert(false);
        return ERROR_DB_SELECT_FAIL;
    } 
  
    return SUCCESS;
}

/******************************************************************************/
int StorageLunActionView::SelectCallback(void * nil, SqlColumn * columns)
{
    if (NULL == columns ||          
        LIMIT != columns->get_column_num())    
    {        
        return -1;   
    }   

    list<int> *p_list;

    p_list = static_cast<list<int> *>(nil);
    
    int64    lun_id;
    StorageLun var;  
    columns->GetValue(LUN_ID,      lun_id); 
         
    p_list->push_back(lun_id);

    return 0;
}

/******************************************************************************/ 
const char * StorageClusterMaxSizeView::_table = 
        "storage_cluster_max_free_size_view";

const char * StorageClusterMaxSizeView::_col_names = 
        "cluster_name,"
        "max_free_size";       
      

StorageClusterMaxSizeView *StorageClusterMaxSizeView::_instance = NULL;
/******************************************************************************/
int StorageClusterMaxSizeView::GetClusters(string & appoint_cluster,int64 size, vector<string> &cluster_list)
{
    int ret;
    ostringstream where;
    if(!appoint_cluster.empty())
    {
        where << " max_free_size >=  " << size
        <<"and cluster_name = '"<<appoint_cluster<< "'";

    }
    else
    {
        where << " max_free_size >=  " << size;
    }
    SqlCallbackable sqlcb(_db);  
    ret = sqlcb.SelectColumn(_table, "cluster_name", where.str(), cluster_list);
    if (SQLDB_ERROR == ret)
    {
        OutPut(SYS_ERROR, "Get info from storage_cluster_max_free_size_view err\n");
       
    }    
    return ret;
}

/******************************************************************************/  
ScDbOperationSet *ScDbOperationSet::_instance = NULL;
/******************************************************************************/
int ScDbOperationSet::init(SqlDB *pDb)
{   
    if (NULL == pDb)
    {
        return ERROR_INVALID_ARGUMENT;
    }
    if (NULL == (_adaptor_address = StorageAdaptorAddressPool::GetInstance(pDb)))
    {
        return ERROR;
    }
    if (NULL == (_adaptor = StorageAdaptorPool::GetInstance(pDb)))
    {
        return ERROR;
    }
    if (NULL == (_address = StorageAddressPool::GetInstance(pDb)))
    {
        return ERROR;
    }
    if (NULL == (_vg = StorageVgPool::GetInstance(pDb)))
    {
        return ERROR;
    }
    if (NULL == (_target = StorageTargetPool::GetInstance(pDb)))
    {
        return ERROR;
    }
    if (NULL == (_authorize = StorageAuthorizePool::GetInstance(pDb)))
    {
        return ERROR;
    }
    if (NULL == (_cluster = StorageClusterPool::GetInstance(pDb)))
    {
        return ERROR;
    }
    if (NULL == (_volume = StorageVolumePool::GetInstance(pDb)))
    {
        return ERROR;
    }
    if (NULL == (_lun = StorageLunPool::GetInstance(pDb)))
    {
        return ERROR;
    }
    if (NULL == (_authorize_lun = StorageAuthorizeLunPool::GetInstance(pDb)))
    {
        return ERROR;
    }
    if (NULL == (_user_volume = StorageUserVolumePool::GetInstance(pDb)))
    {
        return ERROR;
    }
    if (NULL == (_snap_shot = StorageSnapShotPool::GetInstance(pDb)))
    {
        return ERROR;
    }
    
    if (NULL == (_volume_with_lun = StorageVolumeWithLunPool::GetInstance(pDb)))
    {
        return ERROR;
    }
    if (NULL == (_adaptor_with_vg_and_cluster = StorageAdaptorWithVgAndClusterPool::GetInstance(pDb)))
    {
        return ERROR;
    }

    if (NULL == (_adaptor_with_multivg_and_cluster = StorageAdaptorWithMultivgAndClusterPool::GetInstance(pDb)))
    {
        return ERROR;
    }

    if (NULL == (_volume_with_lun_and_auth_and_adaptor = StorageVolumeWithLunAndAuthAndAdaptorPool::GetInstance(pDb)))
    {
        return ERROR;
    }
    if (NULL == (_volume_with_lun_and_address = StorageVolumeWithLunAndAddressPool::GetInstance(pDb)))
    {
        return ERROR;
    }
    if (NULL == (_volume_with_adaptor = StorageVolumeWithAdaptorPool::GetInstance(pDb)))
    {
        return ERROR;
    }
    if (NULL == (_authorize_with_target_id = StorageAuthorizeWithTargetIdPool::GetInstance(pDb)))
    {
        return ERROR;
    }
    if (NULL == (_action_lunid = StorageLunActionView::GetInstance(pDb)))
    {
        return ERROR;
    }
    if (NULL == (_cluster_ms = StorageClusterMaxSizeView::GetInstance(pDb)))
    {
        return ERROR;
    }
    return SUCCESS;
}

/******************************************************************************/  
int DbAddVolumeToTarget(const StorageLun &sl)
{
    int rc = ERROR;
    ScDbOperationSet *pSet = ScDbOperationSet::GetInstance();
    if (NULL == pSet)
    {
        SkyAssert(false);
        return rc;
    }

    rc = pSet->_lun->Allocate(sl);
    if(SUCCESS != rc)
    {
        SkyAssert(false);
    } 
    
    return rc;
}

/******************************************************************************/  
int DbDelVolumeFromTarget(const StorageLun &sl)
{
    int rc = ERROR;
    ScDbOperationSet *pSet = ScDbOperationSet::GetInstance();
    if (NULL == pSet)
    {
        SkyAssert(false);
        return rc;
    }

    rc = pSet->_lun->Drop(sl);  
    if(SUCCESS != rc)
    {
        SkyAssert(false);
    } 
    
    return rc;
}

/******************************************************************************/
int DbGetStorageAdaptorByMid(const MID &mid, StorageAdaptor &sa)
{
    int rc = ERROR;
    ScDbOperationSet *pSet = ScDbOperationSet::GetInstance(); 
    if (NULL == pSet)
    {
        SkyAssert(false);
        return rc;
    }

    string where = " (application = '" + mid._application 
                   +"') and (name = '" + mid._unit
                   +"')";
    vector<StorageAdaptor> vec_sa;

    rc = pSet->_adaptor->Select(vec_sa,where);
    if (SUCCESS == rc)
    {
        if (0 != vec_sa.size())
        {
            sa = *vec_sa.begin();
        }
        else
        {
            return ERROR_OBJECT_NOT_EXIST;
        }
    }
    else
    {
        SkyAssert(false);
    }

    return rc;
}

/******************************************************************************/  
int DbGetStorageAdaptorBySid(int64 sid, StorageAdaptor &sa)
{
    int rc = ERROR;
    ScDbOperationSet *pSet = ScDbOperationSet::GetInstance(); 
    if (NULL == pSet)
    {
        SkyAssert(false);
        return rc;
    }

    string where = " (sid = " +to_string<int64>(sid,dec)
                 + ")";
                   
    vector<StorageAdaptor> vec_sa;

    rc = pSet->_adaptor->Select(vec_sa,where);
    if (SUCCESS == rc)
    {
        if (0 != vec_sa.size())
        {
            sa = *vec_sa.begin();
        }
        else
        {
            return ERROR_OBJECT_NOT_EXIST;
        }
    }
    else
    {
        SkyAssert(false);
    }

    return rc;
}

/******************************************************************************/  
int DbAddStorageAdaptor(StorageAdaptor &sa, const StorageAdaptorAddress &saa, vector<StorageAddress> &vec_sadd, vector<StorageVg> &vec_sv)
{
    int       rc = ERROR;
    ScDbOperationSet *pSet = ScDbOperationSet::GetInstance();
    if (NULL == pSet)
    {
        SkyAssert(false);
        return rc;
    }

    string where = " (application = '" +saa._application
                 + "') and (ip_address = '"+saa._ip_address
                 + "')";
                   
    
    vector < StorageAdaptorAddress >  vec_var;
    pSet->_adaptor_address->Select(vec_var, where);
    
    rc = Transaction::Begin();
    if (SUCCESS != rc)
    {
        SkyAssert(false);
        return rc;
    }
    if(vec_var.empty())
    {
        rc = pSet->_adaptor_address->Allocate(saa);
        if(0 != rc)
        {
            Transaction::Cancel();
            SkyAssert(false);
            return rc;
        } 
    }
    rc = pSet->_adaptor->Allocate(sa);
    if(0 != rc)
    {
        Transaction::Cancel();
        SkyAssert(false);
        return rc;
    } 

    for (vector<StorageAddress>::iterator it = vec_sadd.begin(); it != vec_sadd.end(); it++)
    {
        it->_sid = sa._sid;
        rc = pSet->_address->Allocate(*it); 
        if(0 != rc)
        {
            Transaction::Cancel();
            SkyAssert(false);
            return rc;
        } 
    }

    for (vector<StorageVg>::iterator it = vec_sv.begin();it != vec_sv.end();it++)
    {
        it->_sid = sa._sid;
        rc = pSet->_vg->Allocate(*it); 
        if(0 != rc)
        {
            Transaction::Cancel();
            SkyAssert(false);
            return rc;
        } 
    }
    
    rc = Transaction::Commit();
    if (0 != rc)
    {
        SkyAssert(false);
    }
    
    return rc;
}

/******************************************************************************/  
int DbUpdateStorageAdaptor(const StorageAdaptor &sa, const StorageAdaptorAddress &saa, vector<StorageAddress> &vec_sadd, vector<StorageVg> &vec_sv)
{
    int       rc = ERROR;
    ScDbOperationSet *pSet = ScDbOperationSet::GetInstance();
    if (NULL == pSet)
    {
        SkyAssert(false);
        return ERROR;
    }
    
    string where = " sid = " + to_string<int64>(sa._sid,dec);
    vector<StorageAddress> vec_sadd_db;
    vector<StorageVg> vec_sv_db;
    
    rc = pSet->_address->Select(vec_sadd_db, where);
    if(SUCCESS != rc)
    {
        SkyAssert(false);
        return rc;
    } 
    
    rc = pSet->_vg->Select(vec_sv_db, where);
    if(SUCCESS != rc)
    {
        SkyAssert(false);
        return rc;
    } 
    
    rc = Transaction::Begin();
    if (SUCCESS != rc)
    {
        SkyAssert(false);
        return rc;
    }

    rc = pSet->_adaptor_address->Update(saa);
    if(0 != rc)
    {
        Transaction::Cancel();
        SkyAssert(false);
        return rc;
    } 

    rc = pSet->_adaptor->Update(sa);
    if(0 != rc)
    {
        Transaction::Cancel();
        SkyAssert(false);
        return rc;
    } 

    bool needdrop = true;
    vector<StorageAddress>::iterator it_db;
    vector<StorageAddress>::iterator it;

    for (it_db = vec_sadd_db.begin(); it_db != vec_sadd_db.end(); it_db++)
    {
        needdrop = true;
        for (it = vec_sadd.begin(); it != vec_sadd.end(); it++)
        {
            if ((it_db->_sid == it->_sid) 
                && (it_db->_media_address == it->_media_address))
            {/*存在 不需要新增*/
                vec_sadd.erase(it);
                needdrop = false;
                break;
            }
        }
        if (needdrop)
        {/*需要从数据库中删除*/
            rc = pSet->_address->Drop(*it_db);
            if((SQLDB_OK != rc) && (SQLDB_RESULT_EMPTY != rc))
            {
                Transaction::Cancel();
                SkyAssert(false);
                return rc;
            } 
        }
    }
    for (it = vec_sadd.begin(); it != vec_sadd.end(); it++)
    {/*新增*/
        rc = pSet->_address->Allocate(*it);
        if(0 != rc)
        {
            Transaction::Cancel();
            SkyAssert(false);
            return rc;
        } 
    }

    vector<StorageVg>::iterator vg_it_db;
    vector<StorageVg>::iterator vg_it;
    vector<StorageVg> vec_vg_update;

    for (vg_it_db = vec_sv_db.begin(); vg_it_db != vec_sv_db.end(); vg_it_db++)
    {
        needdrop = true;
        for (vg_it = vec_sv.begin(); vg_it != vec_sv.end(); vg_it++)
        {
            if ((vg_it_db->_sid == vg_it->_sid) 
                && (vg_it_db->_name == vg_it->_name))
            {/*存在 不需要新增*/
                if (vg_it_db->_size != vg_it->_size)
                {/*不一样需要更新*/
                    vec_vg_update.push_back(*vg_it);
                }
                vec_sv.erase(vg_it);
                needdrop = false;
                break;
            }
        }
        if (needdrop)
        {/*需要从数据库中删除*/
            /*需先删除vg下面的vol/lun/authorize_lun*/
            where = "(sid = "+to_string<int64>(vg_it_db->_sid,dec)
                    +") and (vg_name = '"+vg_it_db->_name
                    +"')";
            #if 0
            rc = pSet->_volume_with_lun->Drop(where);
            if((SQLDB_OK != rc) && (SQLDB_RESULT_EMPTY != rc))
            {
                Transaction::Cancel();
                SkyAssert(false);
                return rc;
            } 
            #endif
            vector<StorageVolume> vec_var;
            rc=pSet->_volume->Select(vec_var,where);
            if(0 != rc)
            {
                Transaction::Cancel();
                SkyAssert(false);
                return rc;
            } 
            if(vec_var.size()>0)
            {
                Transaction::Cancel();
                SkyAssert(false);
                OutPut(SYS_ERROR, "sid : %d  vg : %s still use but  lost \n",
                    vg_it_db->_sid,vg_it_db->_name.c_str());
                return rc;
            }

            where = "where "+where;
            
            rc = pSet->_volume->Drop(where);
            if((SQLDB_OK != rc) && (SQLDB_RESULT_EMPTY != rc))
            {
                Transaction::Cancel();
                SkyAssert(false);
                return rc;
            } 
            
            rc = pSet->_vg->Drop(*vg_it_db);
            if((SQLDB_OK != rc) && (SQLDB_RESULT_EMPTY != rc))
            {
                Transaction::Cancel();
                SkyAssert(false);
                return rc;
            } 
        }
    }
    
    for (vg_it = vec_sv.begin(); vg_it != vec_sv.end(); vg_it++)
    {/*新增*/
        rc = pSet->_vg->Allocate(*vg_it);
        if(0 != rc)
        {
            Transaction::Cancel();
            SkyAssert(false);
            return rc;
        } 
    }
    
    if (0 != vec_vg_update.size())
    {
        for (vg_it = vec_vg_update.begin(); vg_it != vec_vg_update.end(); vg_it++)
        {/*更新*/
            rc = pSet->_vg->Update(*vg_it);
            if(0 != rc)
            {
                Transaction::Cancel();
                SkyAssert(false);
                return rc;
            } 
        }
    }
    
    rc = Transaction::Commit();
    if (SUCCESS != rc)
    {
        SkyAssert(false);
    }
    
    return rc;
}

int DbUpdateStorageAdaptor(const StorageAdaptor &sa)
{
    int       rc = ERROR;
    ScDbOperationSet *pSet = ScDbOperationSet::GetInstance();
    if (NULL == pSet)
    {
        SkyAssert(false);
        return ERROR;
    }   
   
    rc = pSet->_adaptor->Update(sa);
    if(0 != rc)
    {      
        SkyAssert(false);
        return rc;
    }  
    return rc;
}

bool DbIsSaBusy(int64    sid)
{
    ScDbOperationSet *pSet = ScDbOperationSet::GetInstance();
    if (NULL == pSet)
    {
        SkyAssert(false);
        return ERROR;
    }    
    string where = " (sid = "+to_string<int64>(sid,dec)+" )";
    vector<StorageCluster> vec_cluster;
    vector<StorageVolume>  vec_volume;
    pSet->_cluster->Select(vec_cluster, where);    
    pSet->_volume->Select(vec_volume, where);
    return (vec_cluster.empty()&&vec_volume.empty())?false:true;
}


int DbDropStorageAdaptor(StorageAdaptor &sa)
{
    int       rc = ERROR;
    ScDbOperationSet *pSet = ScDbOperationSet::GetInstance();
    if (NULL == pSet)
    {
        SkyAssert(false);
        return ERROR;
    }    
    
    string where = " (application = '" +sa._application
                 + "')";
                   
    if(DbIsSaBusy(sa._sid))
    {
        return ERROR_PERMISSION_DENIED;
    }
    vector < StorageAdaptor >  vec_var;
    pSet->_adaptor->Select(vec_var, where);

    rc = Transaction::Begin();
    if (SUCCESS != rc)
    {
        SkyAssert(false);
        return rc;
    }
      
    rc = pSet->_adaptor->Drop(sa);
    if(0 != rc)
    {
        Transaction::Cancel();
        SkyAssert(false);
        return rc;
    }    

    if(vec_var.size()==1)
    {
        StorageAdaptorAddress saa;
        saa._application=sa._application;
        rc = pSet->_adaptor_address->Drop(saa);
        if(0 != rc)
        {
            Transaction::Cancel();
            SkyAssert(false);
            return rc;
        } 
    }
    
    rc = Transaction::Commit();
    if (0 != rc)
    {
        SkyAssert(false);
    }
    return  rc; 
    
}

int DbAddClusterMap(const StorageCluster &sc)
{
    int       rc = ERROR;    
    ScDbOperationSet *pSet = ScDbOperationSet::GetInstance();
    if (NULL == pSet)
    {
        SkyAssert(false);
        return ERROR;
    }   
    vector < StorageCluster >  vec_sc;       
    string where = "(sid = "+to_string<int64>(sc._sid,dec)
                    +") and (cluster_name = '"+sc._cluster_name
                    +"')";
   
    rc=pSet->_cluster->Select(vec_sc,  where);
    if(SUCCESS != rc)
    {
        SkyAssert(false);
        return rc;
    } 
    if(0!=vec_sc.size())
    {
        return ERR_EXIST;
    }
    
    rc=pSet->_cluster->Allocate(sc);
    return rc;
}

int DbDropClusterMap(const StorageCluster &sc)
{
    int       rc = ERROR;    
    ScDbOperationSet *pSet = ScDbOperationSet::GetInstance();
    if (NULL == pSet)
    {
        SkyAssert(false);
        return ERROR;
    }   
    vector < StorageCluster >  vec_sc;       
    string where = "(sid = "+to_string<int64>(sc._sid,dec)
                    +") and (cluster_name = '"+sc._cluster_name
                    +"')";
   
    rc=pSet->_cluster->Select(vec_sc,  where);
    if(SUCCESS != rc)
    {
        SkyAssert(false);
        return rc;
    } 
    if(0 ==vec_sc.size())
    {
        return ERR_OBJECT_NOT_EXIST;
    }
    
    rc=pSet->_cluster->Drop(sc);
    return rc;
}

/******************************************************************************/  
int DbGetVolumeWithAdaptorInfoByUUID(const string &uuid, StorageVolumeWithAdaptor &svwa)
{
    int       rc = ERROR;
    ScDbOperationSet *pSet = ScDbOperationSet::GetInstance();
    if (NULL == pSet)
    {
        SkyAssert(false);
        return ERROR;
    }
    
    string where = "vol_uuid = '"+uuid+"'";
    vector<StorageVolumeWithAdaptor> vec_svwa;
    
    rc = pSet->_volume_with_adaptor->Select(vec_svwa, where);
    if(SUCCESS != rc)
    {
        SkyAssert(false);
        return rc;
    } 

    if (0 != vec_svwa.size())
    {
        svwa = *vec_svwa.begin();
    }
    else
    {
        return ERROR_OBJECT_NOT_EXIST;
    }

    return SUCCESS;
    
}
/******************************************************************************/  
int DbGetVolumeWithAdaptorInfoBySid(int64 sid, StorageVolumeWithAdaptor &svwa)
{
    int       rc = ERROR;
    ScDbOperationSet *pSet = ScDbOperationSet::GetInstance();
    if (NULL == pSet)
    {
        SkyAssert(false);
        return ERROR;
    }

    string where = " sid = "+to_string<int64>(sid,dec);
    vector<StorageVolumeWithAdaptor> vec_svwa;
    
    rc = pSet->_volume_with_adaptor->Select(vec_svwa, where);
    if(SUCCESS != rc)
    {
        SkyAssert(false);
        return rc;
    } 

    if (0 != vec_svwa.size())
    {
        svwa = *vec_svwa.begin();
    }
    else
    {
        return ERROR_OBJECT_NOT_EXIST;
    }

    return SUCCESS;
    
}
/******************************************************************************/  
int DbGetVolumeWithLunInfoBySid(int64 sid, vector<StorageVolumeWithLun> &vec_svwl, vector<StorageAuthorizeWithTargetId> &vec_sawt)
{
    int       rc = ERROR;
    ScDbOperationSet *pSet = ScDbOperationSet::GetInstance();
    if (NULL == pSet)
    {
        SkyAssert(false);
        return rc;
    }
    
    string where = "sid = " + to_string<int64>(sid,dec);
    
    rc = pSet->_volume_with_lun->Select(vec_svwl, where);
    if(SUCCESS != rc)
    {
        SkyAssert(false);
        return rc;
    } 

    rc = pSet->_authorize_with_target_id->Select(vec_sawt, where);
    if(SUCCESS != rc)
    {
        SkyAssert(false);
    } 

    return rc;
}

/******************************************************************************/
int DbGetMediaAddressBySid(int64 sid, vector<string> &media_address)
{
    int rc = ERROR;
    if (INVALID_SID == sid)
    {
        SkyAssert(false);
        return rc;
    }
    
    ScDbOperationSet *pSet = ScDbOperationSet::GetInstance();
    if (NULL == pSet)
    {
        SkyAssert(false);
        return rc;
    }

    string where = "sid = "+to_string<int64>(sid,dec);
    vector<StorageAddress> vec_sadd;
    
    rc = pSet->_address->Select(vec_sadd,where);
    if (SUCCESS != rc)
    {
        SkyAssert(false);
        return rc;
    }
    media_address.clear();
    for (vector<StorageAddress>::iterator it = vec_sadd.begin(); it != vec_sadd.end(); it++)
    {
        media_address.push_back(it->_media_address);
    }

    return SUCCESS;
}

/******************************************************************************/  
int DbGetClusterNameBySid(int64 sid, vector<string> &cluster_name)
{
    int rc = ERROR;
    if (INVALID_SID == sid)
    {
        SkyAssert(false);
        return rc;
    }
    
    ScDbOperationSet *pSet = ScDbOperationSet::GetInstance();
    if (NULL == pSet)
    {
        SkyAssert(false);
        return rc;
    }
    
    string where = "sid = "+to_string<int64>(sid,dec);
    vector<StorageCluster> vec_sclu;
    
    rc = pSet->_cluster->Select(vec_sclu,where);
    if (SUCCESS != rc)
    {
        SkyAssert(false);
        return rc;
    }

    for (vector<StorageCluster>::iterator it = vec_sclu.begin(); it != vec_sclu.end(); it++)
    {
        cluster_name.push_back(it->_cluster_name);
    }

    return SUCCESS;
}

/******************************************************************************/  
int DbGetTargetIndexByKey(int64 sid, const string &name, int64 &index)
{
    int rc = ERROR;
    if (INVALID_SID == sid)
    {
        SkyAssert(false);
        return rc;
    }
    
    ScDbOperationSet *pSet = ScDbOperationSet::GetInstance();
    if (NULL == pSet)
    {
        SkyAssert(false);
        return rc;
    }
    
    string where = "(sid = "+to_string<int64>(sid,dec)
                  +") and (name = '"+name+"')";
    vector<StorageTarget> vec_stgt;

    rc = pSet->_target->Select(vec_stgt,where);
    if (SUCCESS != rc)
    {
        SkyAssert(false);
        return rc;
    }

    if (0 != vec_stgt.size())
    {
        index = vec_stgt.begin()->_target_id;
        return SUCCESS;
    }
    else
    {
        return ERROR_OBJECT_NOT_EXIST;
    }
}  
/******************************************************************************/
int DbGetFreeLunidByTarget(StorageLun &var)
{
    int rc = ERROR;
    ScDbOperationSet *pSet = ScDbOperationSet::GetInstance();
    if (NULL == pSet)
    {
        SkyAssert(false);
        return rc;
    }
    rc = pSet->_lun->AllocateFreeLunid(var);
    if (SUCCESS != rc)
    {
        SkyAssert(false);
    }
    return rc;
}

/******************************************************************************/
int DbSelectStorageAdaptor(const string &clustername, int64 volsize, vector<StorageAdaptorWithMultivgAndCluster> & vec_sawvac)
{
    int rc = ERROR;
    ScDbOperationSet *pSet = ScDbOperationSet::GetInstance();
    if (NULL == pSet)
    {
        SkyAssert(false);
        return rc;
    }
    
    string where = " (cluster_name = '"+clustername
                  +"') and (vg_free_size > "+to_string<int64>(volsize,dec)
                  +")";

    rc = pSet->_adaptor_with_multivg_and_cluster->Select(vec_sawvac,where);
    if (SUCCESS == rc)
    {
        if (0 == vec_sawvac.size())
        {
            rc = ERROR_OBJECT_NOT_EXIST;
        }
       
    }
    else
    {
        SkyAssert(false);
    }

    return rc;
}

/******************************************************************************/
int DbGetTargetInfo(const string &vol_uuid,StorageVolumeWithLunAndAuthAndAdaptor &svwlaa)
{
    ScDbOperationSet *pSet = ScDbOperationSet::GetInstance();
    if (NULL == pSet)
    {
        SkyAssert(false);
        return ERROR;
    }
    /*获取的ip可能存在多个*/
    string where = "(vol_uuid = '"+vol_uuid+"')";    
      
    return pSet->_volume_with_lun_and_auth_and_adaptor->Select(svwlaa,where);
}

/******************************************************************************/
int DbGetTargetInfo(const string &vol_uuid,const string &ip,const string &iscsi_initiator, StorageVolumeWithLunAndAuthAndAdaptor &svwlaa)
{
    ScDbOperationSet *pSet = ScDbOperationSet::GetInstance();
    if (NULL == pSet)
    {
        SkyAssert(false);
        return ERROR;
    }
    
    string where = "(vol_uuid = '"+vol_uuid                   
                   +"')"+"and (iscsi_initiator = '"+iscsi_initiator+"')";
      
    return pSet->_volume_with_lun_and_auth_and_adaptor->Select(svwlaa,where);
}

/******************************************************************************/
int DbGetMediaAddress(const string &vol_uuid, int &lunid, vector<string> &media_address_vec)
{
    int rc = ERROR;
    ScDbOperationSet *pSet = ScDbOperationSet::GetInstance();
    if (NULL == pSet)
    {
        SkyAssert(false);
        return rc;
    }
    
    vector<StorageVolumeWithLunAndAddress> vec_svwla;
    string where = " vol_uuid = '"+vol_uuid+"'";
    
    rc = pSet->_volume_with_lun_and_address->Select(vec_svwla,where);
    if (SUCCESS == rc)
    {
        if (0 == vec_svwla.size())
        {
            rc = ERROR_OBJECT_NOT_EXIST;
        }
        else
        {
            vector<StorageVolumeWithLunAndAddress>::iterator it = vec_svwla.begin();
            lunid = it->_lun_id;
            for (; it != vec_svwla.end(); it++)
            {
                media_address_vec.push_back(it->_media_address);
            }
        }
    }
    else
    {
        SkyAssert(false);
    }

    return rc;
}

/******************************************************************************/
int DbGetStorageVolumeIndex(int64 &index)
{

    ScDbOperationSet *pSet = ScDbOperationSet::GetInstance();
    if (NULL == pSet)
    {
        SkyAssert(false);
        return ERROR;
    }
    
    pSet->_volume->Lock();
    
    index = pSet->_volume->GetLastId()+1;

    pSet->_volume->SetLastId(index);

    pSet->_volume->UnLock();
    
    return SUCCESS;
}

/******************************************************************************/  
int DbGetStorageTargetIndex(int64 sid, int64 &index)
{
    ScDbOperationSet *pSet = ScDbOperationSet::GetInstance();
    if (NULL == pSet)
    {
        SkyAssert(false);
        return ERROR;
    }
    
    pSet->_target->Lock();
    
    index = pSet->_target->GetLastId(sid)+1;

    pSet->_target->SetLastId(sid, index);
    
    pSet->_target->UnLock();
    
    return SUCCESS;
}
/******************************************************************************/ 
int DbGetStorageVolumeByRequestId(const string &request_id, StorageVolume &sv)
{
    ScDbOperationSet *pSet = ScDbOperationSet::GetInstance();
    if (NULL == pSet)
    {
        SkyAssert(false);
        return ERROR;
    }

    int rc = ERROR;
    vector<StorageVolume> vec_sv;
    string where = " request_id = '" + request_id + "'";

    rc = pSet->_volume->Select(vec_sv,where);
    if (SUCCESS != rc)
    {
        SkyAssert(false);
        return rc;
    }
    if (0 == vec_sv.size())
    {
        rc = ERROR_OBJECT_NOT_EXIST;
    }
    else
    {
        sv = vec_sv[0];
    }
    return rc;
}
/******************************************************************************/
int DbGetStorageLun(StorageLun &sl)
{
    ScDbOperationSet *pSet = ScDbOperationSet::GetInstance();
    if (NULL == pSet)
    {
        SkyAssert(false);
        return ERROR;
    }

    int rc = ERROR;
    vector<StorageLun> vec_sl;
    string where = " sid = " + to_string<int64>(sl._sid,dec)
                  +" And target_name = '"+sl._target_name
                  +"' And lun_id = "+to_string<int64>(sl._lun_id,dec);

    rc = pSet->_lun->Select(vec_sl,where);
    if (SUCCESS != rc)
    {
        SkyAssert(false);
        return rc;
    }
    if (0 == vec_sl.size())
    {
        rc = ERROR_OBJECT_NOT_EXIST;
    }
    else
    {
        sl = vec_sl[0];
    }
    return rc;
}
/******************************************************************************/ 
int DbGetStorageAuthorize(StorageAuthorize &sa)
{
    ScDbOperationSet *pSet = ScDbOperationSet::GetInstance();
    if (NULL == pSet)
    {
        SkyAssert(false);
        return ERROR;
    }

    int rc = ERROR;
    vector<StorageAuthorize> vec_sa;
    string where = " sid = " + to_string<int64>(sa._sid,dec)
                  +" And target_name = '"+sa._target_name                  
                  +"' And iscsi_initiator = '"+sa._iscsi_initiator+"'";

    rc = pSet->_authorize->Select(vec_sa,where);
    if (SUCCESS != rc)
    {
        SkyAssert(false);
        return rc;
    }
    if (0 == vec_sa.size())
    {
        rc = ERROR_OBJECT_NOT_EXIST;
    }
    else
    {
        sa = vec_sa[0];
    }
    return rc;
}
/******************************************************************************/ 
int DbGetStorageLunByVolumeUuid(StorageLun &sl,const string &volume_uuid)
{
    ScDbOperationSet *pSet = ScDbOperationSet::GetInstance();
    if (NULL == pSet)
    {
        SkyAssert(false);
        return ERROR;
    }

    int rc = ERROR;
    vector<StorageLun> vec_sl;
    string where = " volume_uuid = '"+volume_uuid+"'";

    rc = pSet->_lun->Select(vec_sl,where);
    if (SUCCESS != rc)
    {
        SkyAssert(false);
        return rc;
    }
    if (0 == vec_sl.size())
    {
        rc = ERROR_OBJECT_NOT_EXIST;
    }
    else
    {
        sl = vec_sl[0];
    }
    return rc;
}
/******************************************************************************/ 
int DbUpdateLunAndAddAuthorizeLun(const StorageLun &sl,const StorageAuthorizeLun &sal)
{
    int       rc = ERROR;
    ScDbOperationSet *pSet = ScDbOperationSet::GetInstance();
    if (NULL == pSet)
    {
        SkyAssert(false);
        return rc;
    }

    rc = Transaction::Begin();
    if (SUCCESS != rc)
    {
        SkyAssert(false);
        return rc;
    }

    rc = pSet->_lun->UpdateLunUuid(sl);
    if(0 != rc)
    {
        Transaction::Cancel();
        SkyAssert(false);
        return rc;
    } 

    rc = pSet->_authorize_lun->Allocate(sal);
    if(0 != rc)
    {
        Transaction::Cancel();
        SkyAssert(false);
        return rc;
    } 

    rc = Transaction::Commit();
    if (0 != rc)
    {
        SkyAssert(false);
    }
    
    return rc;
}
/******************************************************************************/ 
int DbDelLunAndAuthorizeLun(const StorageLun &sl,const StorageAuthorizeLun &sal)
{
    int       rc = ERROR;
    ScDbOperationSet *pSet = ScDbOperationSet::GetInstance();
    if (NULL == pSet)
    {
        SkyAssert(false);
        return rc;
    }

    rc = Transaction::Begin();
    if (SUCCESS != rc)
    {
        SkyAssert(false);
        return rc;
    }

    rc = pSet->_authorize_lun->Drop(sal);
    if(0 != rc)
    {
        Transaction::Cancel();
        SkyAssert(false);
        return rc;
    } 
    
    rc = pSet->_lun->Drop(sl);
    if(0 != rc)
    {
        Transaction::Cancel();
        SkyAssert(false);
        return rc;
    } 

    rc = Transaction::Commit();
    if (0 != rc)
    {
        SkyAssert(false);
    }
    
    return rc;
}
/******************************************************************************/  
int DbDelLunAndAuthorizeLunAndAuthorize(const StorageLun &sl)
{
    int       rc = ERROR;
    ScDbOperationSet *pSet = ScDbOperationSet::GetInstance();
    if (NULL == pSet)
    {
        SkyAssert(false);
        return rc;
    }
    vector<StorageAuthorizeLun> vec_sal;
    string where = " lun_uuid = '"+sl._lun_uuid+"'";
    rc = pSet->_authorize_lun->Select(vec_sal, where);
    if (SUCCESS != rc)
    {
        SkyAssert(false);
        return rc;
    }
    if (0 == vec_sal.size())
    {
        return SUCCESS;
    }
    
    rc = Transaction::Begin();
    if (SUCCESS != rc)
    {
        SkyAssert(false);
        return rc;
    }
    vector<StorageAuthorizeLun>::iterator it;
    for (it = vec_sal.begin(); it != vec_sal.end(); it++)
    {
        where = " auth_uuid = '"+it->_auth_uuid+"'";
        rc = pSet->_authorize_lun->DropByWhere(where);
        if(0 != rc)
        {
            Transaction::Cancel();
            SkyAssert(false);
            return rc;
        } 
        rc = pSet->_authorize->DropByWhere(where);
        if(0 != rc)
        {
            Transaction::Cancel();
            SkyAssert(false);
            return rc;
        } 
    }
    
    rc = pSet->_lun->Drop(sl);
    if(0 != rc)
    {
        Transaction::Cancel();
        SkyAssert(false);
        return rc;
    } 

    rc = Transaction::Commit();
    if (0 != rc)
    {
        SkyAssert(false);
    }
    
    return rc;
}
/******************************************************************************/  
int DbDelAuthorizeLunAndAuthorize(const StorageAuthorize &sa)
{
    int rc = ERROR;
    ScDbOperationSet *pSet = ScDbOperationSet::GetInstance();
    if (NULL == pSet)
    {
        SkyAssert(false);
        return rc;
    }

    rc = Transaction::Begin();
    if (SUCCESS != rc)
    {
        SkyAssert(false);
        return rc;
    }
    
    string where = " auth_uuid = '"+sa._auth_uuid+"'";
        
    rc = pSet->_authorize_lun->DropByWhere(where);
    if(0 != rc)
    {
        Transaction::Cancel();
        SkyAssert(false);
        return rc;
    } 
    
    rc = pSet->_authorize->DropByWhere(where);
    if(0 != rc)
    {
        Transaction::Cancel();
        SkyAssert(false);
        return rc;
    } 

    rc = Transaction::Commit();
    if (0 != rc)
    {
        SkyAssert(false);
    }
    
    return rc;
}
/******************************************************************************/ 
int DbAddAuthorizeAndAuthorizeLun(const StorageAuthorize &sa, const vector<StorageAuthorizeLun> &vec_sal)
{
    int rc = ERROR;
    ScDbOperationSet *pSet = ScDbOperationSet::GetInstance();
    if (NULL == pSet)
    {
        SkyAssert(false);
        return rc;
    }

    rc = Transaction::Begin();
    if (SUCCESS != rc)
    {
        SkyAssert(false);
        return rc;
    }
    
    rc = pSet->_authorize->Allocate(sa);
    if(SUCCESS != rc)
    {
        Transaction::Cancel();
        SkyAssert(false);
        return rc;
    } 

    vector <StorageAuthorizeLun>::const_iterator it;
    StorageAuthorizeLun sal;
    for (it = vec_sal.begin(); it != vec_sal.end(); it++)
    {
        sal._auth_uuid = sa._auth_uuid;
        sal._lun_uuid = it->_lun_uuid;
        rc = pSet->_authorize_lun->Allocate(sal);
        if(SUCCESS != rc)
        {
            Transaction::Cancel();
            SkyAssert(false);
            return rc;
        } 
    }

    rc = Transaction::Commit();
    if (SUCCESS != rc)
    {
        SkyAssert(false);
    }
    
    return rc;
}
/******************************************************************************/  
bool DbIsVolumeExist(const string &uuid)
{
    ScDbOperationSet *pSet = ScDbOperationSet::GetInstance();
    if (NULL == pSet)
    {
        SkyAssert(false);
        return false;
    }
    
    int rc = ERROR;
    string where = "volume_uuid = '"+uuid+"'";
    vector<StorageVolume> vec_sv;
    
    
    rc = pSet->_volume->Select(vec_sv,where);
    if (SUCCESS != rc)
    {
        SkyAssert(false);
        return false;
    }
    
    if (0 == vec_sv.size())
    {
        return false;
    }
    
    return true;
}
/******************************************************************************/  
bool DbIsStorageTargetExist(StorageTarget &st)
{
    ScDbOperationSet *pSet = ScDbOperationSet::GetInstance();
    if (NULL == pSet)
    {
        SkyAssert(false);
        return false;
    }
    
    int rc = ERROR;
    string where = "(sid = "+to_string<int64>(st._sid,dec)
                  +") and (name = '"+st._name+"')";
    vector<StorageTarget> vec_stgt;
    

    rc = pSet->_target->Select(vec_stgt,where);
    if (SUCCESS != rc)
    {
        SkyAssert(false);
        return false;
    }

    if (0 != vec_stgt.size())
    {
        st = vec_stgt[0];
        return true;
    }
    else
    {
        return false;
    }
}

/******************************************************************************/  
bool DbIsHostAuthorized(StorageAuthorize &sa)
{
    ScDbOperationSet *pSet = ScDbOperationSet::GetInstance();  
    if (NULL == pSet)
    {
        SkyAssert(false);
        return false;
    }
    
    int rc = ERROR;
    string where = "(sid = "+to_string<int64>(sa._sid,dec)
                  +") and (target_name = '"+sa._target_name+"'"                  
                  +") and (iscsi_initiator = '"+sa._iscsi_initiator+"')";
    vector<StorageAuthorize> vec_sa;

    rc = pSet->_authorize->Select(vec_sa,where);
    if (SUCCESS != rc)
    {
        SkyAssert(false);
        return false;
    }

    if (0 != vec_sa.size())
    {
        sa = vec_sa[0];
        return true;
    }
    else
    {
        return false;
    }
}
/******************************************************************************/
bool DbIsStorageLunExist(StorageLun &sl)
{
    ScDbOperationSet *pSet = ScDbOperationSet::GetInstance();  
    if (NULL == pSet)
    {
        SkyAssert(false);
        return false;
    }
    
    int rc = ERROR;
    string where = "(sid = "+to_string<int64>(sl._sid,dec)
                  +") and (target_name = '"+sl._target_name+"'"
                  +") and (volume_uuid = '"+sl._volume_uuid+"')";
    vector<StorageLun> vec_sl;

    rc = pSet->_lun->Select(vec_sl,where);
    if (SUCCESS != rc)
    {
        SkyAssert(false);
        return false;
    }

    if (0 != vec_sl.size())
    {
        sl = vec_sl[0];
        return true;
    }
    else
    {
        return false;
    }
}
/******************************************************************************/
bool DbIsStorageTargetExistLun(const StorageTarget &st)
{
    ScDbOperationSet *pSet = ScDbOperationSet::GetInstance();  
    if (NULL == pSet)
    {
        SkyAssert(false);
        return false;
    }
    
    int rc = ERROR;
    string where = "(sid = "+to_string<int64>(st._sid,dec)
                  +") and (target_name = '"+st._name+"')";
    vector<StorageLun> vec_sl;

    rc = pSet->_lun->Select(vec_sl,where);
    if (SUCCESS != rc)
    {
        SkyAssert(false);
        return false;
    }

    if (0 != vec_sl.size())
    {
        return true;
    }
    else
    {
        return false;
    }
}

/******************************************************************************/
bool DbIsStorageTargetExistHost(const StorageTarget &st)
{
    ScDbOperationSet *pSet = ScDbOperationSet::GetInstance();  
    if (NULL == pSet)
    {
        SkyAssert(false);
        return false;
    }
    
    int rc = ERROR;
    string where = "(sid = "+to_string<int64>(st._sid,dec)
                  +") and (target_name = '"+st._name+"')";
    vector<StorageAuthorize> vec_sa;


    rc = pSet->_authorize->Select(vec_sa,where);
    if (SUCCESS != rc)
    {
        SkyAssert(false);
        return false;
    }

    if (0 != vec_sa.size())
    {
        return true;
    }
    else
    {
        return false;
    }
}
/******************************************************************************/
bool DbIsVolumeInStorageTarget(const StorageVolume &sv)
{
    ScDbOperationSet *pSet = ScDbOperationSet::GetInstance();  
    if (NULL == pSet)
    {
        SkyAssert(false);
        return false;
    }
    
    int rc = ERROR;
    string where = "(sid = "+to_string<int64>(sv._sid,dec)
                  +") and (volume_uuid = '"+sv._volume_uuid+"')";
    vector<StorageLun> vec_sl;

    rc = pSet->_lun->Select(vec_sl,where);
    if (SUCCESS != rc)
    {
        SkyAssert(false);
        return false;
    }

    if (0 != vec_sl.size())
    {
        return true;
    }
    else
    {
        return false;
    }
}
/******************************************************************************/ 
bool DbIsAuthorizeExistLun(const StorageAuthorize &sa)
{
    ScDbOperationSet *pSet = ScDbOperationSet::GetInstance();  
    if (NULL == pSet)
    {
        SkyAssert(false);
        return false;
    }
    
    int rc = ERROR;
    string where = " auth_uuid = '"+sa._auth_uuid+"'";
    vector<StorageAuthorizeLun> vec_sal;

    rc = pSet->_authorize_lun->Select(vec_sal,where);
    if (SUCCESS != rc)
    {
        SkyAssert(false);
        return false;
    }

    if (0 != vec_sal.size())
    {
        return true;
    }
    else
    {
        return false;
    }
}
/******************************************************************************/
bool DbIsTargetInfoExist(const string &vol_uuid,const string &ip,const string &iscsi_initiator)
{
    ScDbOperationSet *pSet = ScDbOperationSet::GetInstance();
    if (NULL == pSet)
    {
        SkyAssert(false);
        return false;
    }
    
    int rc = ERROR;
    StorageVolumeWithLunAndAuthAndAdaptor svwlaa;
    string where = "(vol_uuid = '"+vol_uuid                   
                   +"')"+"and (iscsi_initiator = '"+iscsi_initiator+"')";
      
    rc = pSet->_volume_with_lun_and_auth_and_adaptor->Select(svwlaa,where);
    if (SUCCESS == rc)
    {
        return true;
    }
    else if (ERROR_OBJECT_NOT_EXIST != rc)
    {
        SkyAssert(false);
    }
    return false;
}
/******************************************************************************/ 
bool DbIsLastLunInAuthorize(StorageLun &sl, vector<StorageAuthorize> &vec_sa)
{
    ScDbOperationSet *pSet = ScDbOperationSet::GetInstance();
    if (NULL == pSet)
    {
        SkyAssert(false);
        return false;
    }
    
    int rc = ERROR;
    vector<StorageLun> vec_lun;
    string where = " (sid = " + to_string<int64>(sl._sid,dec)
                    + ") and (target_name = '" + sl._target_name
                    + "') and (lun_id = " + to_string<int64>(sl._lun_id,dec) + ")";;
      
    rc = pSet->_lun->Select(vec_lun,where);
    if (SUCCESS != rc)
    {
        SkyAssert(false);
        return false;
    }
    if (0 == vec_lun.size())
    {
        return false;
    }
    sl = vec_lun[0];
    vector <StorageAuthorizeLun> vec_authorize_lun;
    where = " lun_uuid = '" + sl._lun_uuid + "'";
    rc = pSet->_authorize_lun->Select(vec_authorize_lun, where);
    if (SUCCESS != rc)
    {
        SkyAssert(false);
        return false;
    }
    if (0 == vec_authorize_lun.size())
    {
        return false;
    }

    vector <StorageAuthorizeLun> vec_tmp;
    where = " auth_uuid = '" + vec_authorize_lun.begin()->_auth_uuid+ "'";
    rc = pSet->_authorize_lun->Select(vec_tmp, where);
    if (SUCCESS != rc)
    {
        SkyAssert(false);
        return false;
    }
    if (1 < vec_tmp.size())
    {
        return false;
    }
    vector <StorageAuthorizeLun>::iterator it;
    vector <StorageAuthorize> vec_sa_tmp;
    for (it = vec_authorize_lun.begin(); it != vec_authorize_lun.end(); it++)
    {
        where = " auth_uuid = '" + it->_auth_uuid+ "'";
        rc = pSet->_authorize->Select(vec_sa_tmp, where);
        if (SUCCESS != rc)
        {
            SkyAssert(false);
            return false;
        }
        vec_sa.push_back(vec_sa_tmp[0]);
    }
    return true;
}

/******************************************************************************/  
int DbGetActionLunid(list < int > & p_list,int64 sid, string & target_name)
{
    int rc = ERROR;
    ScDbOperationSet *pSet = ScDbOperationSet::GetInstance();
    if (NULL == pSet)
    {
        SkyAssert(false);
        return rc;
    }
    string where = "(sid = "+to_string<int64>(sid,dec)
                  +") and (target_name = '"+target_name+"')";
    rc = pSet->_action_lunid->Select(p_list, where);  
    if(SUCCESS != rc)
    {
        SkyAssert(false);
    } 
    
    return rc;
}

/******************************************************************************/ 
int DbDelAuthorizeLun(const StorageAuthorizeLun &sal)
{
    int       rc = ERROR;
    ScDbOperationSet *pSet = ScDbOperationSet::GetInstance();
    if (NULL == pSet)
    {
        SkyAssert(false);
        return rc;
    }   

    rc = pSet->_authorize_lun->Drop(sal);
    if(0 != rc)
    {
    
        SkyAssert(false);
        return rc;
    }     
    
    return rc;
}

/******************************************************************************/ 
int DbAddAuthorizeLun(const StorageAuthorizeLun &sal)
{
    int       rc = ERROR;
    ScDbOperationSet *pSet = ScDbOperationSet::GetInstance();
    if (NULL == pSet)
    {
        SkyAssert(false);
        return rc;
    }   

    rc = pSet->_authorize_lun->Allocate(sal);
    if(0 != rc)
    {        
        SkyAssert(false);
        return rc;
    }     
    
    return rc;
}

/******************************************************************************/
int DbAddAuthTbl(StorageTarget &st,   StorageAuthorize &sa,  StorageLun &sl)
{
    int rc = ERROR;
    ScDbOperationSet *pSet = ScDbOperationSet::GetInstance();
    if (NULL == pSet)
    {
        SkyAssert(false);
        return rc;
    } 
    if(!DbIsStorageTargetExist(st))
    {
        rc = pSet->_target->Allocate(st);
        if(SUCCESS != rc)
        {       
            SkyAssert(false);
            return rc;
        } 
    }
    if(!DbIsHostAuthorized(sa))
    {
        sa._auth_uuid=GenerateUUID();
        rc = pSet->_authorize->Allocate(sa);
        if(SUCCESS != rc)
        {       
            SkyAssert(false);
            return rc;
        } 
    }
    if(!DbIsStorageLunExist(sl))
    {
        sl._lun_uuid=GenerateUUID();
        rc = pSet->_lun->Allocate(sl);
        if(SUCCESS != rc)
        {       
            SkyAssert(false);
            return rc;
        } 
    }   

    StorageAuthorizeLun sal;
    sal._auth_uuid = sa._auth_uuid;
    sal._lun_uuid = sl._lun_uuid;
    rc = pSet->_authorize_lun->Allocate(sal);    
    return rc;
}

/******************************************************************************/
int DbDelAuthTbl(StorageTarget &st,  StorageAuthorize &sa, StorageLun &sl,int type)
{
    int rc = ERROR;
    ScDbOperationSet *pSet = ScDbOperationSet::GetInstance();
    if (NULL == pSet)
    {
        SkyAssert(false);
        return rc;
    } 
    
    bool host_exist=DbIsHostAuthorized(sa);
    bool lun_exist =DbIsStorageLunExist(sl);
    
    StorageAuthorizeLun sal;
    sal._auth_uuid = sa._auth_uuid;
    sal._lun_uuid = sl._lun_uuid;
    rc = pSet->_authorize_lun->Drop(sal);  
    
    if(host_exist && type& DEL_HOST_TBL)
    {
       
        rc = pSet->_authorize->Drop(sa);
        if(SUCCESS != rc)
        {       
            SkyAssert(false);
            return rc;
        } 
    }
    if(lun_exist&& type &DEL_LUN_TBL)
    {
        rc = pSet->_lun->Drop(sl);
        if(SUCCESS != rc)
        {       
            SkyAssert(false);
            return rc;
        } 
    }   
    if(DbIsStorageTargetExist(st)&& type &DEL_TARGET_TBL)
    {
        rc = pSet->_target->Drop(st);
        if(SUCCESS != rc)
        {       
            SkyAssert(false);
            return rc;
        } 
    }
  
    return rc;
}

/******************************************************************************/
int DbGetLunHostMapNum(const string & target_name,const string &vol_uuid,const string &ip,const string &iscsi_initiator,int64 &num_lun,int64 &num_host)
{
    ScDbOperationSet *pSet = ScDbOperationSet::GetInstance();
    if (NULL == pSet)
    {
        SkyAssert(false);
        return ERROR;
    }   
  
    return pSet->_volume_with_lun_and_auth_and_adaptor->GetLunHostMapNum(target_name,vol_uuid,ip,iscsi_initiator,num_lun,num_host);
}

/******************************************************************************/
int DbGetTargetMapNum(const string & target_name,int64 &num)
{
    ScDbOperationSet *pSet = ScDbOperationSet::GetInstance();
    if (NULL == pSet)
    {
        SkyAssert(false);
        return ERROR;
    }   
  
    return pSet->_volume_with_lun_and_auth_and_adaptor->GetTargetMapNum(target_name,num);
}
int DbGetLunMapNum(const string &vol_uuid,int64 &num)
{
    ScDbOperationSet *pSet = ScDbOperationSet::GetInstance();
    if (NULL == pSet)
    {
        SkyAssert(false);
        return ERROR;
    }   
  
    return pSet->_volume_with_lun_and_auth_and_adaptor->GetLunMapNum(vol_uuid,num);
}
 
/******************************************************************************/
int DbAddVolumeAndUserVolume(const StorageVolume& sv, const StorageUserVolume& suv)
{
    int rc = ERROR;
    ScDbOperationSet *pSet = ScDbOperationSet::GetInstance();
    if (NULL == pSet)
    {
        SkyAssert(false);
        return rc;
    }

    rc = Transaction::Begin();
    if (SUCCESS != rc)
    {
        SkyAssert(false);
        return rc;
    }
    
    rc = pSet->_volume->Allocate(sv);
    if(SUCCESS != rc)
    {
        Transaction::Cancel();
        SkyAssert(false);
        return rc;
    } 

    rc = pSet->_user_volume->Allocate(suv);
    if(SUCCESS != rc)
    {
        Transaction::Cancel();
        SkyAssert(false);
        return rc;
    } 

    rc = Transaction::Commit();
    if (SUCCESS != rc)
    {
        SkyAssert(false);
    }
    
    return rc;
}

/******************************************************************************/
int DbDelVolumeAndUserVolume(const StorageVolume& sv, const StorageUserVolume& suv)
{
    int rc = ERROR;
    ScDbOperationSet *pSet = ScDbOperationSet::GetInstance();
    if (NULL == pSet)
    {
        SkyAssert(false);
        return rc;
    }

    rc = Transaction::Begin();
    if (SUCCESS != rc)
    {
        SkyAssert(false);
        return rc;
    }
    
    rc = pSet->_user_volume->Drop(suv);
    if(SUCCESS != rc)
    {
        Transaction::Cancel();
        SkyAssert(false);
        return rc;
    } 
    
    rc = pSet->_volume->Drop(sv);
    if(SUCCESS != rc)
    {
        Transaction::Cancel();
        SkyAssert(false);
        return rc;
    } 

    rc = Transaction::Commit();
    if (SUCCESS != rc)
    {
        SkyAssert(false);
    }
    
    return rc;
}

/******************************************************************************/
int DbUpdateUsageOfUserVolume(const string& request_id, const string& usage)
{
    int rc = ERROR;
    ScDbOperationSet *pSet = ScDbOperationSet::GetInstance();
    if (NULL == pSet)
    {
        SkyAssert(false);
        return rc;
    }

    string where = " request_id = '" + request_id +"'";
    
    rc = pSet->_user_volume->UpdateUsage(where, usage);
    if(SUCCESS != rc)
    {
        SkyAssert(false);
        return rc;
    } 
    
    return rc;
}
/******************************************************************************/
int DbUpdateUsageOfUserVolume(const string& request_id, const string& vol_uuid,const string& usage)
{
    int rc = ERROR;
    ScDbOperationSet *pSet = ScDbOperationSet::GetInstance();
    if (NULL == pSet)
    {
        SkyAssert(false);
        return rc;
    }
    if(0 == usage.size())
    {
        int64 num=0;
        DbGetLunMapNum(vol_uuid,num);
 
        if(num>1)   //vol在使用,与删表在同一个事务内执行，做预判处理
        {
            return rc;
        }

    }

    string where = " request_id = '" + request_id +"'";
    
    rc = pSet->_user_volume->UpdateUsage(where, usage);
    if(SUCCESS != rc)
    {
        SkyAssert(false);
        return rc;
    } 
    
    return rc;
}

int DbGetClustersBySize(string & appoint_cluster,int64 size, vector<string> &cluster_list)
{
    ScDbOperationSet *pSet = ScDbOperationSet::GetInstance();
    if (NULL == pSet)
    {
        SkyAssert(false);
        return false;
    }
    
    return pSet->_cluster_ms->GetClusters(appoint_cluster, size, cluster_list);
    
}
/******************************************************************************/
int DbAddSnapshot(const StorageSnapshot & var)
{
    ScDbOperationSet *pSet = ScDbOperationSet::GetInstance();
    if (NULL == pSet)
    {
        SkyAssert(false);
        return ERROR;
    }   
  
    return pSet->_snap_shot->Allocate(var);
}

/******************************************************************************/
int DbDropSnapshot(const StorageSnapshot & var)
{
    ScDbOperationSet *pSet = ScDbOperationSet::GetInstance();
    if (NULL == pSet)
    {
        SkyAssert(false);
        return ERROR;
    }   
  
    return pSet->_snap_shot->Drop(var);
}

int  DbGetSaAddress(const string& app_name,string &ip)
{
    ScDbOperationSet *pSet = ScDbOperationSet::GetInstance();
    if (NULL == pSet)
    {
        SkyAssert(false);
        return ERROR;
    }   
   
    return pSet->_adaptor_address->GetIpByAppName(app_name,ip);
}

int DbScanTargetLunid()
{
    int rc=ERROR;
    ScDbOperationSet *pSet = ScDbOperationSet::GetInstance();
    if (NULL == pSet)
    {
        SkyAssert(false);
        return rc;
    }
    
    string  where   = " 1=1 ";
    vector<StorageTarget> vec_stgt;

    rc = pSet->_target->Select(vec_stgt,where);
    if (SUCCESS != rc)
    {      
        return ERROR;
    }

    if (0 == vec_stgt.size())
    {        
        return SUCCESS;
    }
    
    vector<StorageTarget>::iterator it_target;
    StorageLun  var;
    for(it_target=vec_stgt.begin();it_target!=vec_stgt.end();it_target++)
    {
        var._target_name=it_target->_name;
        var._sid=it_target->_sid;
        pSet->_lun->ScanLunid(var);
    }
    
    return SUCCESS;
}

int DbGetVolNumOfVg(int sid, const string& vgname,int& volnum)
{  
    int rc = ERROR;
    ScDbOperationSet *pSet = ScDbOperationSet::GetInstance();
    if (NULL == pSet)
    {
        SkyAssert(false);
        return rc;
    }

    rc = pSet->_volume->GetVolNumOfVg(sid,vgname,volnum);
    if(SUCCESS != rc)
    {
        SkyAssert(false);
    }     
    return rc;
}

int DbGetSnapNum(const StorageSnapshot  &var,int& num)
{  
    int rc = ERROR;
    ScDbOperationSet *pSet = ScDbOperationSet::GetInstance();
    if (NULL == pSet)
    {
        SkyAssert(false);
        return rc;
    }

    rc = pSet->_snap_shot->GetSnapNum(var,num);
    if(SUCCESS != rc)
    {
        SkyAssert(false);
    }     
    return rc;
}

void DbgPrintVolNumOfVg(int sid, char * vgname)
{
    int volnum=0;

    if(NULL == vgname)
    {
        cout<<"the inpara is invalid." <<endl;
        return;
    }

    DbGetVolNumOfVg(sid,vgname,volnum);
    cout<<"the num of vg is " <<volnum << endl;
}

DEFINE_DEBUG_FUNC(DbgPrintVolNumOfVg);

/******************************************************************************/ 
VOID DbgScDbPrintLastId(void)
{
    ScDbOperationSet *pSet = ScDbOperationSet::GetInstance();
    if (NULL == pSet)
    {
        SkyAssert(false);
        return ;
    }
    
    pSet->_adaptor->PrintLastId(); 
    pSet->_target->PrintLastId();
    pSet->_volume->PrintLastId();
}
DEFINE_DEBUG_FUNC(DbgScDbPrintLastId);

VOID DbgScShowLastLunId(char * target_name)
{
    ScDbOperationSet *pSet = ScDbOperationSet::GetInstance();
    if (NULL == pSet)
    {
        SkyAssert(false);
        return ;
    }
    int lun_id;
    
    pSet->_lun->GetLastLunid(target_name,lun_id); 
    cout<<"lastlunid:"<<lun_id<<endl;
}
DEFINE_DEBUG_FUNC(DbgScShowLastLunId);


} // namespace zte_tecs

