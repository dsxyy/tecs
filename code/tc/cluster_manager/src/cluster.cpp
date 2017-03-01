/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：cluster.cpp
* 文件标识：
* 内容摘要：cluster类的定义文件
* 当前版本：1.0
* 作    者：李孝成
* 完成日期：2011年7月25日
*
* 修改记录1：
*     修改日期：2011/7/25
*     版 本 号：V1.0
*     修 改 人：李孝成
*     修改内容：创建
******************************************************************************/
#include "cluster.h"
#include "cluster_pool.h"
#include "alarm_report.h"
#include "alarm_agent.h"
#include "pub_alarm.h"
#include "db_config.h"


namespace zte_tecs{



const char * ClusterNodePool::_table = "cluster_node";

const char * ClusterNodePool::_col_names = "cluster_name," \
                                           "ip_address," \
                                           "is_master";
       
ClusterNodePool *ClusterNodePool::instance = NULL;

/******************************************************************************/
int ClusterNodePool::SelectCallback(void *nil, SqlColumn * columns)
{
    if ( NULL == columns || LIMIT != columns->get_column_num())
    {
        return -1;
    }
    
    ClusterNode  node;
    columns->GetValue(ClusterNodePool::CLUSTER_NAME, node._cluster_name);
    columns->GetValue(ClusterNodePool::IP_ADDRESS,   node._ip_address);
    columns->GetValue(ClusterNodePool::IS_MASTER,     node._is_master);
    
    vector<ClusterNode>* p = static_cast<vector<ClusterNode> *>(nil);
    p->push_back(node);
    return 0;
}

/******************************************************************************/
int ClusterNodePool::SelectClusterNodes(vector<ClusterNode>& nodes, 
                                        const string &cluster)
{
    ostringstream   oss;
    int             rc;


    oss << "SELECT " << _col_names<< 
           " FROM " << _table <<
           " WHERE cluster_name = '" << cluster<<"'";


    SetCallback(static_cast<Callbackable::Callback>(&ClusterNodePool::SelectCallback),
                &nodes);
    rc = _db->Execute(oss, this);
    UnsetCallback();
    if ( 0 != rc && SQLDB_RESULT_EMPTY != rc) 
    {
        SkyAssert(false);
    }
    return rc;
}

/******************************************************************************/
int ClusterNodePool::UpdateClusterNodes(const vector<ClusterNode>& nodes, 
                                        const string &cluster)
{
    int rc = -1;    

    rc = DropClusterNodes(cluster);    
    
    if ( (0 != rc) 
         &&(SQLDB_RESULT_EMPTY != rc) )
    {
        SkyAssert(false);
        return rc;
    }

    rc = InsertClusterNodes(nodes,cluster);

    return rc;
}

/******************************************************************************/
int ClusterNodePool::DropClusterNodes(const string &cluster)
{
    int rc = -1;

    SqlCommand sql(_db,_table);

    rc = sql.Delete(" WHERE cluster_name = '" + cluster + "'"); 

    if ( (0 != rc) 
         &&(SQLDB_RESULT_EMPTY != rc) )
    {
        SkyAssert(false);
        return rc;
    }

    return rc;
}

/******************************************************************************/
int ClusterNodePool::InsertClusterNodes(const vector<ClusterNode>& nodes,
                                        const string &cluster)
{
    int rc = 0;

    SqlCommand sql(_db,_table);

    vector<ClusterNode>::const_iterator it;
    for(it = nodes.begin(); it != nodes.end(); it++)
    {
        sql.Clear();
        sql.Add("cluster_name", cluster);
        sql.Add("ip_address",   it->_ip_address);
        sql.Add("is_master",     it->_is_master);

        rc = sql.Insert();
        if(rc != 0)
        {
            SkyAssert(false);
            break;
        }
    }
    return rc;
}


/******************************************************************************/
int ClusterNodePool::GetIp(string &ip, const string &where)
{
    return SelectColumn(_table, " ip_address ",where,ip);
}

/* 集群使用的表名称 */
const char * Cluster::_table = "cluster_pool";

/* 集群中的表字段 */
const char * Cluster::_db_names = "oid, name,description,register_time, " \
                                   "enabled, is_online, " \
                                   "core_free_max,"         \
                                   "tcu_unit_free_max, tcu_max_total,"         \
                                   "tcu_free_total, tcu_free_max,"         \
                                   "disk_max_total,"                         \
                                   "disk_free_total, disk_free_max,"         \
                                   "mem_max_total,"                          \
                                   "mem_free_total, mem_free_max,"           \
                                   "cpu_userate_average,"                    \
                                   "cpu_userate_max, cpu_userate_min,"
                                   "db_name,"
                                   "db_server_url, db_server_port,"
                                   "db_user, db_passwd,"
                                   "host_count,"
                                   "img_usage,"
                                   "cluster_verify_id,"
                                   "img_srctype,"
                                   "img_spcexp,"
                                   "share_img_usage";


/*****************************************************************************/
/**
@brief 功能描述: 集群的构造函数
    
@li @b 入参列表：
@verbatim
name 集群的名称，由CC上报或者用户下发下来的，是CC的唯一身份识别
stat 集群的注册状态，默认是unregedit  
@endverbatim
      
@li @b 出参列表：
@verbatim
无 
@endverbatim
        
@li @b 返回值列表：
@verbatim
无
@endverbatim
          
@li @b 接口限制：无
@li @b 其它说明：无
*/
/*****************************************************************************/   
Cluster::Cluster(const string &name):PoolObjectSQL(-1)
{
    _oid          = INVALID_OID;
    _cluster_name = name;
    _is_online    = true;
    _enabled      = true;

    _register_time.refresh();
    Datetime  time_temp((time_t)0);
    _last_moni_time = time_temp;
    _cc_alarm_flag  = 0;
    _cluster_resource._img_srctype = "file";
    _cluster_resource._img_spcexp = 0;
    _cluster_resource._img_usage = "snapshot";
    _cluster_resource._share_img_usage = "snapshot";
}

Cluster::Cluster():PoolObjectSQL(-1)
{
    _oid        = INVALID_OID;
    _is_online  = true;
    _enabled    = true;
    
    _register_time.refresh();
    Datetime      time_temp((time_t)0);
    _last_moni_time = time_temp;
    _cc_alarm_flag  = 0;
}

/* 析够函数，后面有可能需要释放该类申请的资源 */
Cluster::~Cluster()
{

}


/*****************************************************************************/
/**
@brief 功能描述: 集群的告警函数
    
@li @b 入参列表：
@verbatim
alarmcode     告警码
alarmbuf      告警内容
alarmbuf_len  告警内容长度
@endverbatim
      
@li @b 出参列表：
@verbatim
无 
@endverbatim
        
@li @b 返回值列表：
@verbatim
无
@endverbatim
          
@li @b 接口限制：无
@li @b 其它说明：无
*/
/*****************************************************************************/   
void Cluster::Alarm(int32 alarmcode)
{
    AlarmReport ar;
    AlarmAgent *temp = AlarmAgent::GetInstance();

    ar.alarm_location.strSubSystem = "computing";
    ar.alarm_location.strLevel1 = ApplicationName();
    ar.alarm_location.strLevel2 = _cluster_name;
    
    ar.dwAlarmCode = alarmcode;
    ar.ucAlarmFlag = 1;
    temp->Send(ar);
}

/*****************************************************************************/
/**
@brief 功能描述: 集群的告警恢复函数
    
@li @b 入参列表：
@verbatim
alarmcode     告警码
alarmbuf      告警内容
alarmbuf_len  告警内容长度
@endverbatim
      
@li @b 出参列表：
@verbatim
无 
@endverbatim
        
@li @b 返回值列表：
@verbatim
无
@endverbatim
          
@li @b 接口限制：无
@li @b 其它说明：无
*/
/*****************************************************************************/   
void Cluster::AlarmResume(int32 alarmcode)
{
    AlarmReport ar;
    AlarmAgent *temp = AlarmAgent::GetInstance();

    ar.alarm_location.strSubSystem = "computing";
    ar.alarm_location.strLevel1 = ApplicationName();
    ar.alarm_location.strLevel2 = _cluster_name;
    
    ar.dwAlarmCode = alarmcode;
    ar.ucAlarmFlag = 2;  
    temp->Send(ar);
}

/*****************************************************************************/
/**
@brief 功能描述: 判断当前集群是否空闲，即没有部署虚拟机
    
@li @b 入参列表：
@verbatim
无
@endverbatim
      
@li @b 出参列表：
@verbatim
无
@endverbatim
        
@li @b 返回值列表：
@verbatim
true  该集群已经空闲
false 该集群没有空闲
@endverbatim
          
@li @b 接口限制：无
@li @b 其它说明：无
*/
/*****************************************************************************/  
bool Cluster::is_free()
{
    if (_cluster_resource._tcu_max_total == _cluster_resource._tcu_free_total && \
        _cluster_resource._disk_max_total == _cluster_resource._disk_free_total && \
        _cluster_resource._mem_max_total == _cluster_resource._mem_free_total)
    {
        return true;
    }

    return false;
}

/*****************************************************************************/
/**
@brief 功能描述: 获取当前集群资源的信息
    
@li @b 入参列表：
@verbatim
无
@endverbatim
      
@li @b 出参列表：
@verbatim
cluster_resource 当前集群资源的结构体
@endverbatim
        
@li @b 返回值列表：
@verbatim
true  成功
false 失败，返回失败，一般为当前集群运行状态不在线的情况下，返回失败！
@endverbatim
          
@li @b 接口限制：无
@li @b 其它说明：无
*/
/*****************************************************************************/
bool Cluster::get_cluster_resource(ClusterResType &cluster_resource)
{
    cluster_resource = _cluster_resource;
   
    vector<ClusterNode>::iterator node_it;
    for(node_it = _cluster_node.begin();
        node_it != _cluster_node.end();
        node_it++)
    {
       cluster_resource._node.insert(make_pair(node_it->_ip_address,node_it->_is_master));
    }

    return true;
}

/*****************************************************************************/
/**
@brief 功能描述: 设置当前集群资源的信息
    
@li @b 入参列表：
@verbatim
cluster_resource 当前集群资源的结构体

@endverbatim
      
@li @b 出参列表：
@verbatim
无
@endverbatim
        
@li @b 返回值列表：
@verbatim
true  成功
false 失败
@endverbatim
          
@li @b 接口限制：无
@li @b 其它说明：无
*/
/*****************************************************************************/
bool Cluster::set_cluster_resource(const ClusterResType & cluster_resource)
{
    _cluster_resource = cluster_resource;
    
    vector<string>::const_iterator  it;

    map<string,int>::const_iterator  node_it;
    for(node_it =  cluster_resource._node.begin();
        node_it != cluster_resource._node.end();
        node_it++)
    {
       ClusterNode node(_cluster_name, node_it->first,node_it->second);
       _cluster_node.push_back(node);
    }

    return true;    
}

int Cluster::get_ip(string &ip)
{
    int rc = -1;
    ClusterNodePool *p_node = ClusterNodePool::GetInstance(GetDB());
    if (NULL == p_node)
    {
        SkyAssert(false);
        return -1;        
    }
    
    ostringstream where;
    where<<"cluster_name= '" << _cluster_name << "' and " << "is_master=" << ClusterNode::MASTER;
    rc = p_node->GetIp(ip, where.str());
    if ( (SQLDB_OK!= rc) 
      && (SQLDB_RESULT_EMPTY != rc))    
    {       
        SkyAssert(false);        
        return rc;    
    }

    return SUCCESS;        
}

/*****************************************************************************/
/**
@brief 功能描述: Cluster类从数据库选取值到类结构中的函数
    
@li @b 入参列表：
@verbatim
SqlDB *db    数据库的指针
@endverbatim
      
@li @b 出参列表：
@verbatim
无
@endverbatim
        
@li @b 返回值列表：
@verbatim
0     获取数据成功
-1    select 失败
@endverbatim
          
@li @b 接口限制：无
@li @b 其它说明：该函数后续会被Cluster_pool通过sql_pool调用
*/
/*****************************************************************************/
int32 Cluster::Select(SqlDB *db)
{
    ostringstream   oss;
    int             rc;
    int64           oid_temp;
    
    SetCallback(static_cast<Callbackable::Callback>(&Cluster::SelectCallback));
    
    //避免使用SELECT *，应该明确要查询的列，这样在表结构升级时才不会有问题
    oss << "SELECT " << _db_names << " FROM " << _table << " WHERE oid = " << _oid;
    
    oid_temp = _oid;
    _oid  = INVALID_OID;
    
    rc = db->Execute(oss, this);
    
    UnsetCallback();
    
    if ((rc != 0) || (_oid != oid_temp ))
    {
        return -1;
    }
    
     
    // 3. 查 cluster_node 表
    ClusterNodePool *p_node = ClusterNodePool::GetInstance(db);
    if (NULL == p_node)
    {
        SkyAssert(false);
        return -1;
    }
    rc = p_node ->SelectClusterNodes(_cluster_node, _cluster_name);

    if ( (SQLDB_OK!= rc)
          && (SQLDB_RESULT_EMPTY != rc))    
    {       
        SkyAssert(false);        
        return rc;    
    }

 
    return 0;
}

/******************************************************************************/
/**
@brief 功能描述: Cluster类向数据库中插入内容
    
@li @b 入参列表：
@verbatim
SqlDB *db          数据库的指针
string & errorStr  插入错误，返回的错误原因
@endverbatim
      
@li @b 出参列表：
@verbatim
无
@endverbatim
        
@li @b 返回值列表：
@verbatim
0     成功
-1    失败
@endverbatim
          
@li @b 接口限制：无
@li @b 其它说明：该函数后续会被Cluster_pool通过sql_pool调用
*/
/*****************************************************************************/
int32 Cluster::Insert(SqlDB * db, string& error_str)
{
    ostringstream   oss;
    int  rc = -1;

    Transaction::Begin();
    SqlCommand sql(db, _table, SqlCommand::WITH_TRANSACTION);
    sql.Add("oid",          _oid);
    sql.Add("name",         _cluster_name);
    sql.Add("description",  _description);
    sql.Add("register_time",  _register_time.tointeger());
    sql.Add("enabled",       _enabled);
    sql.Add("is_online",     _is_online);
  
    sql.Add("core_free_max",  _cluster_resource._core_free_max);
    sql.Add("tcu_unit_free_max",_cluster_resource._tcu_unit_free_max);
    sql.Add("tcu_max_total",    _cluster_resource._tcu_max_total);
    sql.Add("tcu_free_total",   _cluster_resource._tcu_free_total);
    sql.Add("tcu_free_max",     _cluster_resource._tcu_free_max);
    sql.Add("disk_max_total",   _cluster_resource._disk_max_total);
    sql.Add("disk_free_total",  _cluster_resource._disk_free_total);
    sql.Add("disk_free_max",    _cluster_resource._disk_free_max);
   
    sql.Add("mem_max_total",   _cluster_resource._mem_max_total);
    sql.Add("mem_free_total",  _cluster_resource._mem_free_total);
    sql.Add("mem_free_max",    _cluster_resource._mem_free_max);
    sql.Add("cpu_userate_average", _cluster_resource._cpu_usage_average);
    sql.Add("cpu_userate_max",   _cluster_resource._cpu_usage_max);
    sql.Add("cpu_userate_min",   _cluster_resource._cpu_usage_min);
    sql.Add("db_name",           _cluster_resource._db_name);
    sql.Add("db_server_url",     _cluster_resource._db_server_url);
    sql.Add("db_server_port",    _cluster_resource._db_server_port);
    sql.Add("db_user",           _cluster_resource._db_user);
    sql.Add("db_passwd",         _cluster_resource._db_passwd);
    sql.Add("host_count",         _cluster_resource._host_count);
    sql.Add("img_usage",         _cluster_resource._img_usage);
    sql.Add("cluster_verify_id", _cluster_verfify_id);
    sql.Add("img_srctype",       _cluster_resource._img_srctype);
    sql.Add("img_spcexp",        _cluster_resource._img_spcexp);
    sql.Add("share_img_usage",   _cluster_resource._share_img_usage);
    
    rc = sql.Insert();
    if(0 != rc)
    {
        Transaction::Cancel();
        SkyAssert(false);
        return rc;
    }


    // 3. cluster node 表
    ClusterNodePool *p_node = ClusterNodePool::GetInstance(db);
    if (NULL == p_node)
    {
        Transaction::Cancel();
        SkyAssert(false);
        return -1;        
    }
    
    rc = p_node->InsertClusterNodes(_cluster_node, _cluster_name);
    if(0 != rc)
    {
        Transaction::Cancel();
        SkyAssert(false);
        return rc;
    }


    return Transaction::Commit();
}

/*****************************************************************************/
/**
@brief 功能描述: Cluster类向数据库中更新内容
    
@li @b 入参列表：
@verbatim
SqlDB *db          数据库的指针
@endverbatim
      
@li @b 出参列表：
@verbatim
无
@endverbatim
        
@li @b 返回值列表：
@verbatim
0     成功
-1    失败
@endverbatim
          
@li @b 接口限制：无
@li @b 其它说明：该函数后续会被Cluster_pool通过sql_pool调用
*/
/*****************************************************************************/
int32 Cluster::Update(SqlDB *db)
{
    ostringstream   oss;
    int   rc = -1;
    
    Transaction::Begin();
    SqlCommand sql(db, _table, SqlCommand::WITH_TRANSACTION);
    sql.Add("oid",          _oid);
    sql.Add("name",         _cluster_name);
    sql.Add("description",  _description);
    sql.Add("register_time",  _register_time.tointeger());
    sql.Add("enabled",       _enabled);
    sql.Add("is_online",     _is_online);
  
    sql.Add("core_free_max",  _cluster_resource._core_free_max);
    sql.Add("tcu_unit_free_max",_cluster_resource._tcu_unit_free_max);
    sql.Add("tcu_max_total",    _cluster_resource._tcu_max_total);
    sql.Add("tcu_free_total",   _cluster_resource._tcu_free_total);
    sql.Add("tcu_free_max",     _cluster_resource._tcu_free_max);
    sql.Add("disk_max_total",   _cluster_resource._disk_max_total);
    sql.Add("disk_free_total",  _cluster_resource._disk_free_total);
    sql.Add("disk_free_max",    _cluster_resource._disk_free_max);

    sql.Add("mem_max_total",   _cluster_resource._mem_max_total);
    sql.Add("mem_free_total",  _cluster_resource._mem_free_total);
    sql.Add("mem_free_max",    _cluster_resource._mem_free_max);
    sql.Add("cpu_userate_average", _cluster_resource._cpu_usage_average);
    sql.Add("cpu_userate_max",   _cluster_resource._cpu_usage_max);
    sql.Add("cpu_userate_min",   _cluster_resource._cpu_usage_min);
    
    sql.Add("db_name",           _cluster_resource._db_name);
    sql.Add("db_server_url",     _cluster_resource._db_server_url);
    sql.Add("db_server_port",    _cluster_resource._db_server_port);
    sql.Add("db_user",           _cluster_resource._db_user);
    sql.Add("db_passwd",         _cluster_resource._db_passwd);

    sql.Add("host_count",         _cluster_resource._host_count);

    sql.Add("img_usage",          _cluster_resource._img_usage);
    sql.Add("cluster_verify_id",  _cluster_verfify_id);
    sql.Add("img_srctype",        _cluster_resource._img_srctype);
    sql.Add("img_spcexp",         _cluster_resource._img_spcexp);
    sql.Add("share_img_usage",    _cluster_resource._share_img_usage);

    rc = sql.Update("WHERE oid = " + to_string(_oid,dec));
    if(0 != rc)
    {
        SkyAssert(false);
        return rc;
    }

    
    // 3. cluster node表
    ClusterNodePool *p_node = ClusterNodePool::GetInstance(db);
    if (NULL == p_node)
    {
         Transaction::Cancel();
         SkyAssert(false);
         return -1;        
    }

    rc = p_node->UpdateClusterNodes(_cluster_node, _cluster_name);
    if(0 != rc)
    {
         Transaction::Cancel();
         SkyAssert(false);
         return rc;
    }


     return Transaction::Commit();
}

/*****************************************************************************/
/**
@brief 功能描述: Cluster类向数据库中删除内容
    
@li @b 入参列表：
@verbatim
SqlDB *db          数据库的指针
@endverbatim
      
@li @b 出参列表：
@verbatim
无
@endverbatim
        
@li @b 返回值列表：
@verbatim
0     成功
-1    失败
@endverbatim
          
@li @b 接口限制：无
@li @b 其它说明：该函数后续会被Cluster_pool通过sql_pool调用
*/
/*****************************************************************************/
int32 Cluster::Drop(SqlDB *db)
{
    ostringstream   oss;
    
    oss << "delete from " << _table << " where oid = " << _oid ;
    
    int rc = db->Execute(oss);
    
    if ( 0 == rc )
    {
        set_valid(false);
    }
    
    return rc;      
}

/*****************************************************************************/
/**
@brief 功能描述: Cluster类向外输出字符串内容
    
@li @b 入参列表：
@verbatim
SqlColumn *columns    查询到的行
@endverbatim
      
@li @b 出参列表：
@verbatim
ostringstream& oss   输出的字符串流结构
@endverbatim
        
@li @b 返回值列表：
@verbatim
0     成功
-1    失败
@endverbatim
          
@li @b 接口限制：无
@li @b 其它说明：该函数后续会被Cluster_pool通过sql_pool调用
*/
/*****************************************************************************/
int32 Cluster::Dump(ostringstream& oss, SqlColumn *columns)
{

    return 0;
}

/*****************************************************************************/
/**
@brief 功能描述: Cluster类查询回调函数
    
@li @b 入参列表：
@verbatim
void *nil               表指针
SqlColumn * columns     查询到的结果行指针
@endverbatim
      
@li @b 出参列表：
@verbatim
无
@endverbatim
        
@li @b 返回值列表：
@verbatim
0     成功
-1    失败
@endverbatim
          
@li @b 接口限制：无
@li @b 其它说明：该接口在数据查询过成中，把数据库中的内容放到类成员中去
*/
/*****************************************************************************/
int32 Cluster::SelectCallback(void *nil, SqlColumn * columns)
{
    if ( NULL == columns || 
        LIMIT != columns->get_column_num()
        )
    {
        return -1;
    }
          
    int32 tmp_int = 0;
    columns->GetValue(OID , _oid);
    columns->GetValue(NAME , _cluster_name);
    columns->GetValue(DESCRIPTION, _description);
    columns->GetValue(REGEDIT_TIME, _register_time);
//    columns->GetValue(IP_ADDRESS, _ip);
    
    columns->GetValue(ENABLED, tmp_int);
    _enabled = static_cast<bool>(tmp_int);
    
    columns->GetValue(IS_ONLINE, tmp_int);
    _is_online = static_cast<bool>(tmp_int);
    
    columns->GetValue(CORE_FREE_MAX , _cluster_resource._core_free_max);
    columns->GetValue(TCU_UNIT_FREE_MAX , _cluster_resource._tcu_unit_free_max);
    columns->GetValue(TCU_MAX_TOTAL , _cluster_resource._tcu_max_total);
    columns->GetValue(TCU_FREE_TOTAL, _cluster_resource._tcu_free_total);
    columns->GetValue(TCU_FREE_MAX , _cluster_resource._tcu_free_max);
    
    columns->GetValue(DISK_MAX_TOTAL , _cluster_resource._disk_max_total);
    columns->GetValue(DISK_FREE_TOTAL , _cluster_resource._disk_free_total);
    columns->GetValue(DISK_FREE_MAX , _cluster_resource._disk_free_max);
    
    columns->GetValue(MEM_MAX_TOTAL , _cluster_resource._mem_max_total);
    columns->GetValue(MEM_FREE_TOTAL , _cluster_resource._mem_free_total);
    columns->GetValue(MEM_FREE_MAX , _cluster_resource._mem_free_max);
    
    columns->GetValue(CPU_USERATE_AVR , _cluster_resource._cpu_usage_average);
    columns->GetValue(CPU_USERATE_MAX , _cluster_resource._cpu_usage_max);
    columns->GetValue(CPU_USERATE_MIN , _cluster_resource._cpu_usage_min);

    columns->GetValue(DB_NAME, _cluster_resource._db_name);
    columns->GetValue(DB_SERVER_URL, _cluster_resource._db_server_url);
    columns->GetValue(DB_SERVER_PORT, _cluster_resource._db_server_port);
    columns->GetValue(DB_USER, _cluster_resource._db_user);
    columns->GetValue(DB_PASSWD, _cluster_resource._db_passwd);

    columns->GetValue(HOST_COUNT, _cluster_resource._host_count);

    columns->GetValue(IMG_USAGE, _cluster_resource._img_usage);
    columns->GetValue(CLUSTER_VERIFY_ID, _cluster_verfify_id);
    columns->GetValue(IMG_SRCTYPE, _cluster_resource._img_srctype);
    columns->GetValue(IMG_SPCEXP, _cluster_resource._img_spcexp);
    columns->GetValue(SHARE_IMG_USAGE, _cluster_resource._share_img_usage);
    return 0;
}

} /* end namespace zte_tecs */
