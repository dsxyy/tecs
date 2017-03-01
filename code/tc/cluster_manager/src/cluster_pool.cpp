/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：cluster_pool.cpp
* 文件标识：
* 内容摘要：cluster_pool类的定义文件
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
#include "cluster_pool.h"

#include "cluster.h"
#include "sqldb.h"
#include "log_agent.h"
#include "pub_alarm.h"

namespace zte_tecs
{
ClusterPool *ClusterPool::instance = NULL;
/*****************************************************************************/
/**
@brief 功能描述: 集群池构造函数
    
@li @b 入参列表：
@verbatim
SqlDB * db       数据库指针
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
STATUS ClusterPool::Init()
{
    ostringstream   sql;
    int             rc = SQLDB_OK;
    
    SetCallback(static_cast<Callbackable::Callback>(&ClusterPool::Init_Callback));
    sql  << "select oid, name from " <<  Cluster::_table;
    rc = PoolSQL::_db->Execute(sql, this);
    UnsetCallback();
    _unregedit_keep_time = 60;    /* 集群在未注册的时候时候，如果离线删除记录的时长 60s */
    _regedit_keep_time   = 60;    /* 集群从在线到离线的检测时长60s */

    if (SQLDB_OK == rc || SQLDB_RESULT_EMPTY == rc)
    {
        return SUCCESS;
    }
    
    OutPut(SYS_EMERGENCY, "ClusterPool execute db fail ,err code : %d", rc);
    return ERROR;
}

/*****************************************************************************/
/**
@brief 功能描述: 根据集群名称，在集群池中创建一个成员
    
@li @b 入参列表：
@verbatim
string& cluster_name    传入的集群名称
@endverbatim
      
@li @b 出参列表：
@verbatim
int32 *  oid           返回的数据中的索引oid
string&  error_str     如果操作失败，返回的错误原因
@endverbatim
        
@li @b 返回值列表：
@verbatim
0       成功
-1      失败
@endverbatim
          
@li @b 接口限制：无
@li @b 其它说明：无
*/
/*****************************************************************************/
int64 ClusterPool::Allocate(int64 *oid,  
                              const string&  cluster_name, 
                              const string&  cluster_appendinfo, 
                              string&  error_str)
{
    Cluster  *clu;

    clu = new Cluster(cluster_name);

    clu->_description = cluster_appendinfo;
    *oid = PoolSQL::Allocate(clu, error_str);
    if (*oid != INVALID_OID)
    {
        // Add the user to the map of known_users
        _cluster_names.insert(make_pair(cluster_name,*oid));
    }

    
    return *oid;
}

/*****************************************************************************/
/**
@brief 功能描述: 根据集群名称，获取一个集群的结构

@li @b 入参列表：
@verbatim
const string&  cluster_name     集群名称
int32  lock                      true 表示加锁，false表示不加锁
@endverbatim
      
@li @b 出参列表：
@verbatim
无
@endverbatim
        
@li @b 返回值列表：
@verbatim
返回一个集群类的指针
@endverbatim
          
@li @b 接口限制：无
@li @b 其它说明：无
*/
/*****************************************************************************/
Cluster* ClusterPool::GetCluster(const string&  cluster_name, int32  lock)
{
    map<string, int>::iterator       index;

    index = _cluster_names.find(cluster_name);

    if (index != _cluster_names.end())
    {
        /* 这里面的Get函数，是调用基类中的map来获取指针 */
        return static_cast<Cluster*>(PoolSQL::Get(index->second, lock));
    }

    return NULL;
}


/*****************************************************************************/
/**
@brief 功能描述: 通过CPU数量，内存大小，硬盘大小来查找一个集群

@li @b 入参列表：
@verbatim
int32 cpu_num     指定CPU数量
int64 mem_size    指定内存大小
int64 disk_size   指定硬盘大小
@endverbatim
      
@li @b 出参列表：
@verbatim
无
@endverbatim
        
@li @b 返回值列表：
@verbatim
返回一个集群类的指针
@endverbatim
          
@li @b 接口限制：无
@li @b 其它说明：无
*/
/*****************************************************************************/
Cluster *ClusterPool::FindCluster(int32 cpu_num, int32 tcu_num, int64 mem_size, 
                                  int64 disk_size, int64 share_disk_size, 
                                  int32 *findnum)
{
    vector<int64>     oid;
    ostringstream   oss;
    time_t          temp;
    int32           postion = 0;

    oss << "core_free_max >= " << cpu_num \
      <<" and " << "tcu_unit_free_max >= " << tcu_num \
      <<" and " << "tcu_free_max >= " << tcu_num*cpu_num \
      <<" and "<<" mem_free_max >= "<< mem_size \
      <<" and "<<" disk_free_max >= " <<disk_size \
      <<" and "<<" is_online = " << true \
      <<" and "<<" enabled = " << true;

    OutPut(SYS_DEBUG,"FindCluster: %s\n",oss.str().c_str());
    if (SQLDB_OK != PoolSQL::Search(oid, Cluster::_table, oss.str()))
    {
        return NULL;
    }

    *findnum = oid.size();
    
    /* 如果发现数目为0，则没有查找到资源 */
    if (0 == *findnum)
    {
        return NULL;
    }
    /* 获取一个当前的系统运行秒数 */
    time(&temp);
    
    /* 根据随机数取当前位置 */
    postion = temp%(*findnum);

    return static_cast<Cluster*>(PoolSQL::Get(oid.at(postion), false));
}


/*****************************************************************************/
/**
@brief 功能描述: 通过给定条件查询集群
@li @b 入参列表：
@endverbatim
      
@li @b 出参列表：
@verbatim
无
@endverbatim
        
@li @b 返回值列表：
@verbatim
返回一个集群类的指针
@endverbatim
          
@li @b 接口限制：无
@li @b 其它说明：无
*/
/*****************************************************************************/

int ClusterPool::FindCluster(const string &where, vector<Cluster*> &result)
{
    vector<int64>     oid;
    uint32            i;
    Cluster           *pCluster;

    OutPut(SYS_DEBUG,"FindCluster: %s\n",where.c_str());
    if (SQLDB_OK != PoolSQL::Search(oid, Cluster::_table, where))
    {
        return 0;
    }

    /* 如果发现数目为0，则没有查找到资源 */
    for (i = 0; i < oid.size(); i++)
    {
        pCluster = static_cast<Cluster*>(PoolSQL::Get(oid.at(i), false));
        result.push_back(pCluster);
    }
    
    return oid.size();
}


/******************************************************************************/
int ClusterPool::GetClusterNamesWithEnoughResource(const string & where,vector<string> &cluster_name)
{

    SqlCallbackable sqlcb(_db);
    int ret = sqlcb.SelectColumn("cluster_pool","name",where,cluster_name);

    if (0 != ret || cluster_name.empty())
    {
        OutPut(SYS_DEBUG, "Cluster_pool:can not find cluster that has enough resource,ret is %d \n",ret);
        return -1;
    }
    else
    {
        return 0;
    }
}

/*****************************************************************************/
int32 ClusterPool::SelectInt_Callback(void *nil, SqlColumn * columns)
{
    if ( NULL == columns  ||
        1 != columns->get_column_num()
        )
    {
        return -1;
    }

    int *p = static_cast<int *>(nil);

    *p = columns->GetValueInt(0); 
   
    return 0;
}

/*****************************************************************************/
int ClusterPool::FindHcMaxCore()
{
    ostringstream   sql;
    int             rc = SQLDB_OK;
    int             core_max = 0;
    
    SetCallback(static_cast<Callbackable::Callback>(&ClusterPool::SelectInt_Callback), 
                &core_max);
    sql  << "select max(core_free_max) from " <<  Cluster::_table;
    rc = _db->Execute(sql, this);
    UnsetCallback();

    if ( 0 != rc && SQLDB_RESULT_EMPTY != rc) 
    {
        SkyAssert(false);
    }
    return core_max;
}

/*****************************************************************************/
int ClusterPool::FindHcMaxTcu()
{
    ostringstream   sql;
    int             rc = SQLDB_OK;
    int             tcu_max = 0;
    
    SetCallback(static_cast<Callbackable::Callback>(&ClusterPool::SelectInt_Callback), 
                &tcu_max);
    sql  << "select max(tcu_unit_free_max) from " <<  Cluster::_table;
    rc = _db->Execute(sql, this);
    UnsetCallback();

    if ( 0 != rc && SQLDB_RESULT_EMPTY != rc) 
    {
        SkyAssert(false);
    }
    return tcu_max;
}


/*****************************************************************************/
/**
@brief 功能描述: 策略算法选择集群
@li @b 入参列表：
@verbatim
int32 cpu_num     指定CPU数量
int64 mem_size    指定内存大小
int64 disk_size   指定硬盘大小
@endverbatim
      
@li @b 出参列表：
@verbatim
无
@endverbatim
        
@li @b 返回值列表：
@verbatim
返回一个集群类的指针
@endverbatim
          
@li @b 接口限制：无
@li @b 其它说明：无
*/
/*****************************************************************************/
Cluster   *ClusterPool::SelectClusterUsePolicy(const vector<Cluster*> &cluster_in)
{
    time_t          temp;
    uint32          findnum = cluster_in.size();
    uint32          postion;

    if (findnum == 0)
    {
        return NULL;
    }
    
    /* 获取一个当前的系统运行秒数 */
    time(&temp);
    
    /* 根据随机数取当前位置 */
    postion = temp%(findnum);

    return cluster_in.at(postion);
}

Cluster   *ClusterPool::SelectClusterUsePolicy(const vector<Cluster*> &cluster_in,const vector<string> &cluster_name)
{
    time_t          temp;
    uint32          findnum_pcluster = cluster_in.size();
    uint32          findnum_cluster_name = cluster_name.size();
    uint32          postion;
    string          cluster_name_selected;

    if ((0 == findnum_pcluster) || (0 ==findnum_cluster_name))
    {
        return NULL;
    }

    /* 获取一个当前的系统运行秒数 */
    time(&temp);

    /* 根据随机数取当前位置 */
    postion = temp%(findnum_cluster_name);

    cluster_name_selected =  cluster_name.at(postion);
    uint32 i;
    for (i = 0;i < findnum_pcluster ; i++)
    {
        if (cluster_in.at(i)->_cluster_name == cluster_name_selected)
        {
            return cluster_in.at(i);
            break;

        }
    }
    return NULL;
}

/*****************************************************************************/
/**
@brief 功能描述: 通过传入时间，刷新当前数据库中集群的状态

@li @b 入参列表：
@verbatim
now_time  当前刷新的系统时间
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
void ClusterPool::RefreshCluster(time_t now_time)
{
    vector<int64>     oid;
    string          a("oid >= 0");
    uint32           i;
    Cluster        *pCluster;
    string         table;

    table = Cluster::_table;
    
    
    if (SQLDB_OK != PoolSQL::Search(oid, table.c_str(), a))
    {
        return ;
    }
    
    for (i = 0; i < oid.size(); i++)
    {
        pCluster = static_cast<Cluster*>(PoolSQL::Get(oid.at(i), true));

        if (NULL != pCluster)
        {
            {    /* 注册过的集群，当一段时间没有收到保活，则运行态进入OFFLINE */ 
                if ((now_time - pCluster->_last_moni_time.tointeger()) >= _regedit_keep_time)
                {
                    if (true == pCluster->_is_online)
                    {
                        pCluster->_is_online = false;
						OutPut(SYS_NOTICE, "cluster %s is offline\r\n", pCluster->_cluster_name.c_str());
                        /* 告警集群离线了 */
                        pCluster->Alarm((int32)ALM_TECS_CLUSTER_OFF_LINE);
                        /* 设置已经告过警了 */
                        pCluster->set_cc_alarm_flag(Cluster::CC_OFFLINE_ALARM);
                        this->Update(pCluster);
                    } 
                }
            }
            pCluster->UnLock();
        }
    }
}

/*****************************************************************************/
/**
@brief 功能描述: 把资源池中所有的集群的名字都获取出来
    
@li @b 入参列表：
@verbatim
无
@endverbatim
      
@li @b 出参列表：
@verbatim
vector<string> *cluster_name_tab   输出所有可见的集群名称
@endverbatim
        
@li @b 返回值列表：
@verbatim
无
@endverbatim
          
@li @b 接口限制：无
@li @b 其它说明：无
*/
/*****************************************************************************/
void ClusterPool::GetAllClusterName(vector<string> &cluster_name_tab)
{
    vector<int64>     oid;
    string          a("oid >= 0 order by oid");
    uint32          i;
    Cluster        *pCluster;
    string         table;

    table = Cluster::_table;
    
    
    if (SQLDB_OK != PoolSQL::Search(oid, table.c_str(), a))
    {
        return;
    }
    
    for (i = 0; i < oid.size(); i++)
    {
        pCluster = static_cast<Cluster*>(PoolSQL::Get(oid.at(i), false));
        
        if (NULL != pCluster)
        {
            cluster_name_tab.push_back(pCluster->_cluster_name);
        }
    }

}

/*****************************************************************************/
/**
@brief 功能描述: 把指定集群的类中的成员内容更新到数据中
    
@li @b 入参列表：
@verbatim
Cluster * cluster    集群类指针
@endverbatim
      
@li @b 出参列表：
@verbatim
无
@endverbatim
        
@li @b 返回值列表：
@verbatim
0   成功
-1  失败
@endverbatim
          
@li @b 接口限制：无
@li @b 其它说明：无
*/
/*****************************************************************************/
int32 ClusterPool::Update(Cluster * cluster)
{
    return cluster->Update(_db);
}

/*****************************************************************************/
/**
@brief 功能描述: 把指定集群的内容从数据库中删除
    
@li @b 入参列表：
@verbatim
Cluster * cluster    集群类指针
@endverbatim
      
@li @b 出参列表：
@verbatim
无
@endverbatim
        
@li @b 返回值列表：
@verbatim
0    成功
-1   失败
@endverbatim
          
@li @b 接口限制：无
@li @b 其它说明：无
*/
/*****************************************************************************/
int32 ClusterPool::Drop(Cluster * cluster)
{
    int rc = PoolSQL::Drop(cluster);

    if ( rc == 0)
    {
        string tempname;
        cluster->get_name(&tempname);
        _cluster_names.erase(tempname);
    }
    
    return rc;
}

/*****************************************************************************/
/**
@brief 功能描述: 根据指定字段，输出查询结果的字符串
    
@li @b 入参列表：
@verbatim
const string& where     这个查询条件
@endverbatim
      
@li @b 出参列表：
@verbatim
ostringstream& oss      这个是输出的字符串流
@endverbatim
        
@li @b 返回值列表：
@verbatim
0   成功
-1  失败
@endverbatim
          
@li @b 接口限制：无
@li @b 其它说明：无
*/
/*****************************************************************************/
int32 ClusterPool::Dump(ostringstream& oss, const string& where)
{
    int             rc;
    ostringstream   cmd;
    
    oss << "<CLUSTER_POOL>";
    
    SetCallback(static_cast<Callbackable::Callback>(&ClusterPool::Dump_Callback),
        static_cast<void *>(&oss));
    
    cmd << "SELECT " << Cluster::_db_names << " FROM " << Cluster::_table;
    
    if ( !where.empty() )
    {
        cmd << " WHERE " << where;
    }
    
    rc = _db->Execute(cmd, this);
    
    UnsetCallback();
    
    oss << "</CLUSTER_POOL>";
    
    return rc;
}

/*****************************************************************************/
/**
@brief 功能描述: 集群池结果输出回调函数
    
@li @b 入参列表：
@verbatim
SqlColumn * columns    查询的结果成员指针
@endverbatim
      
@li @b 出参列表：
@verbatim
void * _oss            输出的数据刘指针
@endverbatim
        
@li @b 返回值列表：
@verbatim
0    成功
-1   失败
@endverbatim
          
@li @b 接口限制：无
@li @b 其它说明：无
*/
/*****************************************************************************/
int32 ClusterPool::Dump_Callback(void * oss, SqlColumn * columns)
{
    ostringstream * temp_oss;
    
    temp_oss = static_cast<ostringstream *>(oss);
    
   return Cluster::Dump(*temp_oss, columns);
}

/*****************************************************************************/
/**
@brief 功能描述: 集群池初始化回调函数
    
@li @b 入参列表：
@verbatim
void *nil              数据库表指针
SqlColumn * columns    查询结果指针
@endverbatim
      
@li @b 出参列表：
@verbatim
无
@endverbatim
        
@li @b 返回值列表：
@verbatim
0    成功
-1   失败
@endverbatim
          
@li @b 接口限制：无
@li @b 其它说明：该接口为集群池调用，相当于把数据库中的集群名称和oid的对应关系
                 在集群池中初始化一个map映射关系
*/
/*****************************************************************************/
int32 ClusterPool::Init_Callback(void *nil, SqlColumn * columns)
{
    if ( NULL == columns  ||
        2 != columns->get_column_num()
        )
    {
        return -1;
    }
    int64 oid;
    string name;
    
    oid  = columns->GetValueInt(Cluster::OID); 
    name = columns->GetValueText(Cluster::NAME); 
    
    _cluster_names.insert(make_pair(name,oid));
    
    return 0;
}

} /* end namespace zte_tecs */
