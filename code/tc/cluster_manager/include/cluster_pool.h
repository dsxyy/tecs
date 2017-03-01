/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：cluster_pool.h
* 文件标识：见配置管理计划书
* 内容摘要：cluster数据管理池类的声明文件
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
#ifndef TC_CLUSTER_POOL_H
#define TC_CLUSTER_POOL_H
#include "cluster.h"
#include "pool_sql.h"
#include "mid.h"
#include "event.h"

#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName&); \
void operator=(const TypeName&)
#endif

namespace zte_tecs
{

class ClusterPool : public PoolSQL 
{
public:
    static ClusterPool* GetInstance()
    {
        SkyAssert(NULL != instance);
        return instance;
    };
      
    static ClusterPool* CreateInstance(SqlDB *db)
    {
        if(NULL == instance)
        {
            instance = new ClusterPool(db); 
        }
        
        return instance;
    };
          
    ~ClusterPool()
    {
    }

    STATUS Init();

/*****************************************************************************/
/**
@brief 功能描述: 根据集群名称，在集群池中创建一个成员
    
@li @b 入参列表：
@verbatim
string& cluster_name    传入的集群名称
@endverbatim
      
@li @b 出参列表：
@verbatim
int64 *  oid           返回的数据中的索引oid
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
    int64 Allocate(int64 *  oid, 
                     const string &cluster_name, 
                     const string &cluster_appendinfo, 
                     string &error_str);

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
    Cluster * GetCluster(const string&  cluster_name, int32  lock);

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
    Cluster * FindCluster(int32  cpu_num, int32 tcu_num, int64 mem_size, int64 disk_size, int64 share_disk_size, int32 *findnum);
    int       FindCluster(const string &where, vector<Cluster*> &result);
   

    Cluster   *SelectClusterUsePolicy(const vector<Cluster*> &cluster_in);
    Cluster   *SelectClusterUsePolicy(const vector<Cluster*> &cluster_in,const vector<string> &cluster_name);

    // 获取当前集群单主机的最大核数
    int FindHcMaxCore();

    // 获取当前集群单主机的最大TCU数
    int FindHcMaxTcu();


    int GetClusterNamesWithEnoughResource(const string & where,vector<string> &cluster_name);

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
void RefreshCluster(time_t now_time);

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
void GetAllClusterName(vector<string> &cluster_name_tab);

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
    int32 Update(Cluster * cluster);

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
    int32 Drop(Cluster * cluster);

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
    int32 Dump(ostringstream& oss, const string& where);


/*****************************************************************************/
/**
@brief 功能描述: 设置未注册集群的未收到保活，删除时长值
    
@li @b 入参列表：
@verbatim
int32 unregedit_keep_time 时间值,单位S
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
void set_unregedit_keep_time(int32 unregedit_keep_time)
{
    _unregedit_keep_time = unregedit_keep_time;
}

/*****************************************************************************/
/**
@brief 功能描述: 设置注册集群的未收到保活，离线时长值
    
@li @b 入参列表：
@verbatim
int32 regedit_keep_time 时间值，单位S
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
void set_regedit_keep_time(int32 regedit_keep_time)
{
    _regedit_keep_time = regedit_keep_time;
}

private:    
    static ClusterPool *instance; 

    ClusterPool(SqlDB *db):PoolSQL(db,Cluster::_table)
    {

    };
    
    map<string,int32> _cluster_names;       /* 集群名称和oid之间的映射关系 */
    int32             _unregedit_keep_time; /* 数据库池成员未注册的保持时长 */
    int32             _regedit_keep_time;   /* 数据库池成员注册的保持时长 */

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
    int32 Dump_Callback(void * oss, SqlColumn * columns);

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
    int32 Init_Callback(void *nil, SqlColumn * columns);

    // 获取集群资源池中Int类型的回调
    int32 SelectInt_Callback(void *nil, SqlColumn * columns); 

/*****************************************************************************/
/**
@brief 功能描述: 实现基类中creat的虚函数，主要为创建该类的一个实体
    
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
NULL    创建失败
非空    创建成功实体的指针
@endverbatim
          
@li @b 接口限制：无
@li @b 其它说明：无
*/
/*****************************************************************************/
    PoolObjectSQL *Create()
    {
        string  name("NULL");
        return new Cluster(name);
    }
    
    DISALLOW_COPY_AND_ASSIGN(ClusterPool);
};

} /* end namespace zte_tecs */

#endif //TC_CLUSTER_POOL_H
