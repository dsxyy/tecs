/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：cluster.h
* 文件标识：见配置管理计划书
* 内容摘要：cluster类的声明文件
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
#ifndef TC_CLUSTER_H
#define TC_CLUSTER_H
#include "mid.h"
#include "event.h"
#include "pool_object_sql.h"
#include "pool_sql.h"
#include "cluster_manager_with_host_manager.h"
#include "tecs_pub.h"

// 禁用拷贝构造宏定义
#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName&); \
void operator=(const TypeName&)
#endif

namespace zte_tecs
{



class ClusterNode: public Serializable
{        
public:
    enum MsState
    {        
        NO_MASTER   = 0,
        MASTER      = 1
    };
    ClusterNode(){_is_master=MASTER;};
    ClusterNode(const string & cluster, const string & ip_address,int is_master)
    {
       _cluster_name    = cluster;
       _ip_address      = ip_address;
       _is_master        = is_master;
    }        
    
    SERIALIZE 
    {
        SERIALIZE_BEGIN(ClusterNode);
        WRITE_VALUE(_cluster_name);
        WRITE_VALUE(_ip_address);
        WRITE_VALUE(_is_master);
        SERIALIZE_END();    
    };
    
    DESERIALIZE 
    {
        DESERIALIZE_BEGIN(ClusterNode);
        READ_VALUE(_cluster_name);
        READ_VALUE(_ip_address);
        READ_VALUE(_is_master);
        DESERIALIZE_END();    
    };

    string   _cluster_name;
    string   _ip_address;
    int      _is_master;
};

class ClusterNodePool: public SqlCallbackable
{
public:
    static ClusterNodePool* GetInstance(SqlDB *db = NULL)
    {
        if(NULL == instance)
        {
            SkyAssert(NULL != db);
            instance = new ClusterNodePool(db); 
        }
        
        return instance;
    };
    
    int SelectClusterNodes(vector<ClusterNode>& nodes, const string &cluster);
    int UpdateClusterNodes(const vector<ClusterNode>& nodes, const string &cluster);
    int DropClusterNodes(const string &cluster);
    int InsertClusterNodes(const vector<ClusterNode> &nodes, const string &cluster); 
    int GetIpCallback(void *nil, SqlColumn * columns);
    int GetIp(string &ip, const string &where);
private:
    enum ColNames 
    {
        CLUSTER_NAME    = 0,
        IP_ADDRESS      = 1,
        IS_MASTER       = 2,
        LIMIT           = 3
    };
    
    ClusterNodePool(SqlDB *pDb):
        SqlCallbackable(pDb)
    {
        _db = pDb;
    };
    int SelectCallback(void *nil, SqlColumn * columns);

    DISALLOW_COPY_AND_ASSIGN(ClusterNodePool);
    static ClusterNodePool  *instance;
    SqlDB                   *_db;
    static const char       *_table;    
    static const char       *_col_names;    
};

/**
@brief 功能描述: 集群类定义
@li @b 其它说明：无
*/
class Cluster: public PoolObjectSQL   
{

public:
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
    Cluster(const string &name );
    Cluster();
    ~Cluster();
    
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
void AlarmResume(int32 alarmcode);

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
void Alarm(int32 alarmcode);

/*****************************************************************************/
/**
@brief 功能描述: 获取该集群的名字
    
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
返回当前集群的名称，返回的类型为string 
@endverbatim
          
@li @b 接口限制：无
@li @b 其它说明：无
*/
/*****************************************************************************/  
    void get_name(string *name) 
    {
        *name =  _cluster_name;
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
bool is_free();

/*****************************************************************************/
/**
@brief 功能描述: 获取最后一次监控的时间
    
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
返回一个time_t结构的时间
@endverbatim
          
@li @b 接口限制：无
@li @b 其它说明：无
*/
/*****************************************************************************/
    time_t get_last_moni_time()
    {
        return _last_moni_time.tointeger();
    }
/*****************************************************************************/
/**
@brief 功能描述: 获取当前集群注册的时间
    
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
返回一个time_t结构的时间
@endverbatim
          
@li @b 接口限制：无
@li @b 其它说明：无
*/
/*****************************************************************************/
    time_t get_regesiter_time()
    {
        return _register_time.tointeger();
    }
/*****************************************************************************/
/**
@brief 功能描述: 更新当前集群的最新监控时间
    
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
无
@endverbatim
          
@li @b 接口限制：无
@li @b 其它说明：在控制流程中收到该集群上报的心跳，就调用该函数来进行时间更新
*/
/*****************************************************************************/
    void update_last_moni_time()
    {
        _last_moni_time.refresh();
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
    bool get_cluster_resource(ClusterResType &cluster_resource);

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
    bool set_cluster_resource(const ClusterResType & cluster_resource);
    
   
/*****************************************************************************/
/**
@brief 功能描述: 设置集群的注册状态
    
@li @b 入参列表：
@verbatim
cc_register_state  设置集群注册态，使用的值参加该类中的enum reg_state 的定义
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
    void set_register_state(int32 cc_register_state)
    {
        ;
    }
    
/*****************************************************************************/
/**
@brief 功能描述: 获取集群的在线状态
    
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
 bool true:在线， false:离线 
@endverbatim
          
@li @b 接口限制：无
@li @b 其它说明：无
*/
/*****************************************************************************/
    bool get_is_online(void)
    {
        return _is_online;
    }
    
/*****************************************************************************/
/**
@brief 功能描述: 获取集群的维护状态
    
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
 bool true: 使能， false:维护
@endverbatim
          
@li @b 接口限制：无
@li @b 其它说明：无
*/
/*****************************************************************************/
    bool get_enabled(void)
    {
        return _enabled;
    }
 
/*****************************************************************************/
/**
@brief 功能描述: 获取集群的IP信息
    
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
int32 类型的值，具体定义可以使用该类中的enum run_state 的状态定义
@endverbatim
          
@li @b 接口限制：无
@li @b 其它说明：无
*/
/*****************************************************************************/
    int get_ip(string &ip);
/*****************************************************************************/
/**
@brief 功能描述: 获取集群的附加信息
    
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
int32 类型的值，具体定义可以使用该类中的enum run_state 的状态定义
@endverbatim
          
@li @b 接口限制：无
@li @b 其它说明：无
*/
/*****************************************************************************/
    void get_description(string *description)
    {
        *description = _description;
    }


/*****************************************************************************/
/**
@brief 功能描述: 设置集群的在线状态
    
@li @b 入参列表：
@verbatim
 online  true: 在线， false:离线
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
    void set_is_online(bool online)
    {
        _is_online  = online;
    }    

/*****************************************************************************/
/**
@brief 功能描述: 设置集群的使能状态
    
@li @b 入参列表：
@verbatim
 online  true: 使能， false:维护
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
    void set_enabled(bool enb)
    {
        _enabled  = enb;
    }    
 
/*****************************************************************************/
/**
@brief 功能描述: 设置集群告警标志
    
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
无
@endverbatim
          
@li @b 接口限制：无
@li @b 其它说明：无
*/
/*****************************************************************************/
    void set_cc_alarm_flag(int32 flag)
    {
        _cc_alarm_flag |= flag;
    } 

/*****************************************************************************/
/**
@brief 功能描述: 清除集群告警标志
    
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
无
@endverbatim
          
@li @b 接口限制：无
@li @b 其它说明：无
*/
/*****************************************************************************/
    void clear_cc_alarm_flag(int32 flag)
    {
        _cc_alarm_flag &= ~flag;
    } 
    
/*****************************************************************************/
/**
@brief 功能描述: 设置集群离线告警标志
    
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
无
@endverbatim
          
@li @b 接口限制：无
@li @b 其它说明：无
*/
/*****************************************************************************/
    bool offline_alarm_exist(void)
    {
        if ((_cc_alarm_flag & Cluster::CC_OFFLINE_ALARM) != 0)
        {
            return true;
        }
        
        return false;
    }

    bool duplicate_name_exist(void)
    {
        if ((_cc_alarm_flag & Cluster::CC_DUPLICATE_NAME) != 0)
        {
            return true;
        }
        
        return false;
    }

    string                   _cluster_name;      /* Cluster名 */
    string                   _description;       /* 集群附加信息 */
    bool                     _enabled;           /* 是否使能 */
    bool                     _is_online;         /* 是否在线 */
    Datetime                 _register_time;     /* 集群注册时间 */
    Datetime                 _last_moni_time;    /* 集群保活时间 */
    ClusterResType           _cluster_resource;  /* 集群所携带的资源 */
    vector<ClusterNode>      _cluster_node;      /* cluster的主备各个节点信息 */
    int32                    _cc_alarm_flag;     /* 是否告警的标志 */
    string                   _cluster_verfify_id; /*  集群验证码 */
 
///////////////////////////////////////////////////////////////////////////////
/*             该类的public的宏值定义，以及变量的定义 start                  */                
///////////////////////////////////////////////////////////////////////////////

    /* 告警标志,以后其他告警可以采用添加其他码值 */
    enum alarm_value
    {
        CC_OFFLINE_ALARM  = 1,
        CC_DUPLICATE_NAME = 2
    };

     static const char* enable_state_str(bool state)
    {
        if (state)
    {
           return "ENABLE";
        }
        else
        {
           return "DISABLE";
        }
    };
    static const char* run_state_str(bool state)
    {
        if (state)
    {
           return "ONLINE";
        }
        else
        {
           return "OFFLINE";
        }
    };

    /* 该成员在数据库表中的列位置 */
    enum ColNames {
        OID             = 0,
        NAME            = 1,
        DESCRIPTION     = 2,
        REGEDIT_TIME    = 3,
        ENABLED         = 4,
        IS_ONLINE       = 5,
        CORE_FREE_MAX   = 6,
        TCU_UNIT_FREE_MAX = 7,
        TCU_MAX_TOTAL  = 8,
        TCU_FREE_TOTAL = 9,
        TCU_FREE_MAX   = 10,
        DISK_MAX_TOTAL  = 11,
        DISK_FREE_TOTAL = 12,
        DISK_FREE_MAX   = 13,
         
        MEM_MAX_TOTAL   = 14,
        MEM_FREE_TOTAL  = 15,
        MEM_FREE_MAX    = 16,
        CPU_USERATE_AVR = 17,
        CPU_USERATE_MAX = 18,
        CPU_USERATE_MIN = 19,
        DB_NAME         = 20,
        DB_SERVER_URL   = 21,
        DB_SERVER_PORT  = 22,
        DB_USER         = 23,
        DB_PASSWD       = 24,
        HOST_COUNT      = 25,
        IMG_USAGE       = 26,
        CLUSTER_VERIFY_ID = 27,
        IMG_SRCTYPE     = 28,
        IMG_SPCEXP      = 29,
        SHARE_IMG_USAGE = 30,
        LIMIT           = 31
    };
///////////////////////////////////////////////////////////////////////////////
/*             该类的public的宏值定义，以及变量的定义 end                    */                
///////////////////////////////////////////////////////////////////////////////

protected:    
    friend class ClusterPool; 

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
    int32 Select(SqlDB *db);

/*****************************************************************************/
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
    int32 Insert(SqlDB * db, string & errorStr);

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
    int32 Update(SqlDB *db);

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
    int32 Drop(SqlDB *db);

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
    static int32 Dump(ostringstream& oss, SqlColumn *columns);


private:

//    DISALLOW_COPY_AND_ASSIGN(Cluster);


    static const char * _table;              /* 数据库中存放Cluster对象的表名 */
    static const char * _db_names;           /* 查找Cluster资源表时用到的字段名 */
    static const char * _extended_db_names;  /* 查找Cluster资源表时用到的扩展字段名 */


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
    int32 SelectCallback(void *nil, SqlColumn * columns);


};

}  /* end namespace zte_tecs */


#endif /* end TC_CLUSTER_H */

