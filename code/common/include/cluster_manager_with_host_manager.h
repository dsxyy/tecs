/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：cluster_manager_with_host_manager.h
* 文件标识：
* 内容摘要：集群管理与物理机管理的相关头文件
* 当前版本：1.0
* 作    者：lixiaocheng
* 完成日期：2011年10月19日
*
* 修改记录1：
*    修改日期：2011/10/19
*    版 本 号：V1.0
*    修 改 人：lixiaocheng
*    修改内容：创建
*******************************************************************************/
#ifndef COMM_CLUSTER_MANAGER_WITH_HOST_MANAGER_H
#define COMM_CLUSTER_MANAGER_WITH_HOST_MANAGER_H

#include "sky.h"
#include <string.h>

using namespace std;
namespace zte_tecs
{
/*****************************************************************************/
/* 集群资源类型类，封装成类主要为了发送QPID消息的时候方便序列化 */
/*****************************************************************************/
class ClusterResType : public Serializable
{
public:
    ClusterResType() 
    {
        _core_free_max=0;      // 所有状态正常主机中最大核数
        _tcu_unit_free_max=0;  // 所有状态正常主机中最大的计算能力数
    
        _tcu_max_total=0;       // 所有状态正常主机的总计算能力
        _tcu_free_total=0;      // 所有状态正常主机的空闲计算能力
        _tcu_free_max=0;        // 所有状态正常主机中剩余最大的计算能力
        
        _disk_max_total=0;      // 所有状态正常主机的本地磁盘总容量的总和，单位B
        _disk_free_total=0;     // 所有状态正常主机的本地磁盘空闲容量的总和，单位B
        _disk_free_max=0;       // 所有状态正常主机的本地磁盘空闲容量的最大值，单位B
        
        _share_disk_max_total=0; // 当前集群拥有的共享存贮空间总和，单位B
        _share_disk_free_total=0;// 当前集群拥有的空闲共享存贮空间总和，单位B
        
        _mem_max_total=0;       // 所有状态正常主机的内存总容量的总和，单位B
        _mem_free_total=0;      // 所有状态正常主机的内存空闲容量的总和，单位B
        _mem_free_max=0;        // 所有状态正常主机的内存空闲容量的最大值，单位B
        
        _cpu_usage_average=0;   // 所有状态正常主机的30分钟物理cpu利用率平均值
        _cpu_usage_max=0;       // 所有状态正常主机的30分钟物理cpu利用率最大值
        _cpu_usage_min=0;       // 所有状态正常主机的30分钟物理cpu利用率最小值

        _db_server_port = 0;   //将数据库相关字段清零

        _host_count = 0;  //将host数量字段置零
    };
    ~ClusterResType() {};
    
    SERIALIZE 
    {
        SERIALIZE_BEGIN(ClusterResType);
        WRITE_DIGIT(_core_free_max);
        WRITE_DIGIT(_tcu_unit_free_max);
        WRITE_DIGIT(_tcu_max_total);
        WRITE_DIGIT(_tcu_free_total);
        WRITE_DIGIT(_tcu_free_max);    
        WRITE_DIGIT(_disk_max_total);
        WRITE_DIGIT(_disk_free_total);
        WRITE_DIGIT(_disk_free_max);
        WRITE_DIGIT(_share_disk_max_total);
        WRITE_DIGIT(_share_disk_free_total);       
        WRITE_DIGIT(_mem_max_total);
        WRITE_DIGIT(_mem_free_total);
        WRITE_DIGIT(_mem_free_max);
        WRITE_DIGIT(_cpu_usage_average);    
        WRITE_DIGIT(_cpu_usage_max);
        WRITE_DIGIT(_cpu_usage_min);
        WRITE_STRING_VECTOR(_net_plane);
        WRITE_STRING_MAP(_node);
        WRITE_STRING(_db_name);
        WRITE_STRING(_db_server_url);    
        WRITE_DIGIT(_db_server_port);
        WRITE_STRING(_db_user);
        WRITE_STRING(_db_passwd);
        WRITE_DIGIT(_host_count);
        WRITE_STRING(_img_usage);
        WRITE_STRING(_img_srctype);
        WRITE_DIGIT(_img_spcexp);
        WRITE_STRING(_share_img_usage);
        SERIALIZE_END();    
    };
    
    DESERIALIZE 
    {
        DESERIALIZE_BEGIN(ClusterResType);
        READ_DIGIT(_core_free_max);
        READ_DIGIT(_tcu_unit_free_max);
        READ_DIGIT(_tcu_max_total);
        READ_DIGIT(_tcu_free_total);
        READ_DIGIT(_tcu_free_max);    
        READ_DIGIT(_disk_max_total);
        READ_DIGIT(_disk_free_total);
        READ_DIGIT(_disk_free_max);
        READ_DIGIT(_share_disk_max_total);
        READ_DIGIT(_share_disk_free_total); 
        READ_DIGIT(_mem_max_total);
        READ_DIGIT(_mem_free_total);
        READ_DIGIT(_mem_free_max);
        READ_DIGIT(_cpu_usage_average);    
        READ_DIGIT(_cpu_usage_max);
        READ_DIGIT(_cpu_usage_min);
        READ_STRING_VECTOR(_net_plane);
        READ_STRING_MAP(_node);
        READ_STRING(_db_name);
        READ_STRING(_db_server_url);    
        READ_DIGIT(_db_server_port);
        READ_STRING(_db_user);
        READ_STRING(_db_passwd);
        READ_DIGIT(_host_count);
        READ_STRING(_img_usage);
        READ_STRING(_img_srctype);
        READ_DIGIT(_img_spcexp);
        READ_STRING(_share_img_usage);
        DESERIALIZE_END();    
    };

    void clear()
    {
        _core_free_max=0;      // 所有状态正常主机中最大核数
        _tcu_unit_free_max=0;  // 所有状态正常主机中最大的计算能力数

        _tcu_max_total=0;       // 所有状态正常主机的总计算能力
        _tcu_free_total=0;      // 所有状态正常主机的空闲计算能力
        _tcu_free_max=0;        // 所有状态正常主机中剩余最大的计算能力
        
        _disk_max_total=0;      // 所有状态正常主机的本地磁盘总容量的总和，单位B
        _disk_free_total=0;     // 所有状态正常主机的本地磁盘空闲容量的总和，单位B
        _disk_free_max=0;       // 所有状态正常主机的本地磁盘空闲容量的最大值，单位B
        
        _share_disk_max_total=0; // 当前集群拥有的共享存贮空间总和，单位B
        _share_disk_free_total=0;// 当前集群拥有的空闲共享存贮空间总和，单位B
        
        _mem_max_total=0;       // 所有状态正常主机的内存总容量的总和，单位B
        _mem_free_total=0;      // 所有状态正常主机的内存空闲容量的总和，单位B
        _mem_free_max=0;        // 所有状态正常主机的内存空闲容量的最大值，单位B
        
        _cpu_usage_average=0;   // 所有状态正常主机的30分钟物理cpu利用率平均值
        _cpu_usage_max=0;       // 所有状态正常主机的30分钟物理cpu利用率最大值
        _cpu_usage_min=0;       // 所有状态正常主机的30分钟物理cpu利用率最小值

        _net_plane.clear();
        _node.clear();

        _db_server_port = 0;   //将数据库相关字段清零
        _db_name.clear();
        _db_server_url.clear();
        _db_user.clear();
        _db_passwd.clear();

        _host_count = 0;   //将host数量字段清零

        _img_usage.clear();
        _img_srctype.clear();
        _img_spcexp = 0;
        _share_img_usage.clear();
    }
    
    int32 _core_free_max;      // 所有状态正常主机中最大核数
    int32 _tcu_unit_free_max;  // 所有状态正常主机中最大的计算能力数

    int32 _tcu_max_total;       // 所有状态正常主机的总计算能力
    int32 _tcu_free_total;      // 所有状态正常主机的空闲计算能力
    int32 _tcu_free_max;        // 所有状态正常主机中剩余最大的计算能力
    
    int64 _disk_max_total;      // 所有状态正常主机的本地磁盘总容量的总和，单位B
    int64 _disk_free_total;     // 所有状态正常主机的本地磁盘空闲容量的总和，单位B
    int64 _disk_free_max;       // 所有状态正常主机的本地磁盘空闲容量的最大值，单位B
    
    int64 _share_disk_max_total; // 当前集群拥有的共享存贮空间总和，单位B
    int64 _share_disk_free_total;// 当前集群拥有的空闲共享存贮空间总和，单位B
    
    int64 _mem_max_total;       // 所有状态正常主机的内存总容量的总和，单位B
    int64 _mem_free_total;      // 所有状态正常主机的内存空闲容量的总和，单位B
    int64 _mem_free_max;        // 所有状态正常主机的内存空闲容量的最大值，单位B
    
    int32 _cpu_usage_average;   // 所有状态正常主机的30分钟物理cpu利用率平均值
    int32 _cpu_usage_max;       // 所有状态正常主机的30分钟物理cpu利用率最大值
    int32 _cpu_usage_min;       // 所有状态正常主机的30分钟物理cpu利用率最小值
    vector<string>  _net_plane; // 集群网络平面信息
    map<string,int>  _node;     // cluster node信息

    string _db_name;         //数据库名
    string _db_server_url;   // 服务器URL
    int32 _db_server_port;       // 服务器端口
    string _db_user;       // 用户名
    string _db_passwd;   // 密码

    int32 _host_count;  //host数量
    
    string _img_usage;  //镜像使用方式,快照或者非快照
    string _img_srctype; //虚拟机使用镜像文件类型 文件或者块
    int _img_spcexp; //镜像存放的块设备大小扩展比例
    string _share_img_usage;  //共享镜像使用方式,快照或者非快照
};

/*****************************************************************************/
/* 集群信息类，该类属于消息类，由Cluster Manger向Agent转发过去的 */
/*****************************************************************************/
class ClusterInfo : public Serializable
{
public:
    ClusterInfo() {};
    ~ClusterInfo() {};
    
    SERIALIZE 
    {
        SERIALIZE_BEGIN(ClusterInfo);
        WRITE_VALUE(_cluster_name);
        WRITE_VALUE(_ip);
        WRITE_VALUE(_cluster_append);
        WRITE_VALUE(_is_online);
        WRITE_VALUE(_enabled);
        WRITE_OBJECT(_resource);
        SERIALIZE_END();    
    };
    
    DESERIALIZE 
    {
        DESERIALIZE_BEGIN(ClusterInfo);
        READ_VALUE(_cluster_name);
        READ_VALUE(_ip);
        READ_VALUE(_cluster_append);
        READ_VALUE(_is_online);
        READ_VALUE(_enabled);
        READ_OBJECT(_resource);
        DESERIALIZE_END();    
    };

    void clear()
    {
        _resource.clear();  //清除restype中的字段信息
       
    }
    
    string _cluster_name;       // 集群名称
    string _ip;                 // 集群所在IP
    string _cluster_append;     // 集群附加信息
    bool  _is_online;           // 在线状态
    bool  _enabled;             // 使能状态
    ClusterResType _resource;   // 具体资源信息
};

class MessageClusterSetReq : public Serializable
{
public:
    MessageClusterSetReq(const string& name, const string& appendinfo)
    {
        _name = name;
        _cluster_append = appendinfo;
    }
    MessageClusterSetReq(){};
    ~MessageClusterSetReq() {};
    
    SERIALIZE 
    {
        SERIALIZE_BEGIN(MessageClusterSetReq);
        WRITE_STRING(_name);
        WRITE_STRING(_cluster_append);
        WRITE_STRING(_appendinfo);
        SERIALIZE_END();
    };
    
    DESERIALIZE  
    {
        DESERIALIZE_BEGIN(MessageClusterSetReq);
        READ_STRING(_name);
        READ_STRING(_cluster_append);
        READ_STRING(_appendinfo);
        DESERIALIZE_END();
    };
    

    string _name;             // 要注册的集群名称
    string _cluster_append;   /* 集群的附加信息 */
    string _appendinfo;       
};

class MessageClusterSetAck : public Serializable
{
public:
    MessageClusterSetAck() {};
    MessageClusterSetAck(const string& s, const string& appendinfo)
    {
        _result = s;
        _appendinfo = appendinfo;
    }
    ~MessageClusterSetAck() {};
    
    SERIALIZE 
    {
        SERIALIZE_BEGIN(MessageClusterSetAck);
        WRITE_DIGIT(_err_code);
        WRITE_STRING(_result);
        WRITE_STRING(_appendinfo);
        SERIALIZE_END();
    };
    
    DESERIALIZE  
    {
        DESERIALIZE_BEGIN(MessageClusterSetAck);
        READ_DIGIT(_err_code);
        READ_STRING(_result);
        READ_STRING(_appendinfo);
        DESERIALIZE_END();
    };
    

    int    _err_code;          // 错误码
    string _result;            // 
    string _appendinfo;
};

class MessageClusterForgetReq : public Serializable
{
public:
    MessageClusterForgetReq() {};
    MessageClusterForgetReq(const string& s)
    {
        _name = s;
    }
    ~MessageClusterForgetReq() {};
    
    SERIALIZE 
    {
        SERIALIZE_BEGIN(MessageClusterForgetReq);
        WRITE_STRING(_name);
        WRITE_STRING(_appendinfo);
        SERIALIZE_END();
    };
    
    DESERIALIZE  
    {
        DESERIALIZE_BEGIN(MessageClusterForgetReq);
        READ_STRING(_name);
        READ_STRING(_appendinfo);
        DESERIALIZE_END();
    };
    

    string _name;              // 要删除注册的集群名称
    string _appendinfo;       
};

class MessageClusterForgetAck : public Serializable
{
public:
    MessageClusterForgetAck() {};
    MessageClusterForgetAck(const string& s, const string& appendinfo)
    {
        _result = s;
        _appendinfo = appendinfo;
    }
    ~MessageClusterForgetAck() {};
    
    SERIALIZE 
    {
        SERIALIZE_BEGIN(MessageClusterForgetAck);
        WRITE_DIGIT(_err_code);
        WRITE_STRING(_result);
        WRITE_STRING(_appendinfo);
        SERIALIZE_END();
    };
    
    DESERIALIZE  
    {
        DESERIALIZE_BEGIN(MessageClusterForgetAck);
        READ_DIGIT(_err_code);
        READ_STRING(_result);
        READ_STRING(_appendinfo);
        DESERIALIZE_END();
    };


    int    _err_code;       // 错误码
    string _result;         // 集群从TECS系统注销的处理结果
    string _appendinfo;     // 存放的是发送过来的消息单元，
};

/*****************************************************************************/
/* Agent向Cluster manger 和 manger 向 CC发送注册请求时候的消息体结构 */
/* 该消息结构的消息ID为：EV_CLUSTER_REGISTER_REQ 和EV_CLUSTER_ACTION_REG_REQ */
/*****************************************************************************/
class MessageClusterRegisterReq : public Serializable
{
public:
    MessageClusterRegisterReq(const string& name, const string& appendinfo)
    {
        _name = name;
        _cluster_append = appendinfo;
    }
    MessageClusterRegisterReq(){};
    ~MessageClusterRegisterReq() {};
    
    SERIALIZE 
    {
        SERIALIZE_BEGIN(MessageClusterRegisterReq);
        WRITE_STRING(_name);
        WRITE_STRING(_cluster_append);
        WRITE_STRING(_appendinfo);
        WRITE_VALUE(_tc_ifs_info);
        SERIALIZE_END();
    };
    
    DESERIALIZE  
    {
        DESERIALIZE_BEGIN(MessageClusterRegisterReq);
        READ_STRING(_name);
        READ_STRING(_cluster_append);
        READ_STRING(_appendinfo);
        READ_VALUE(_tc_ifs_info);
        DESERIALIZE_END();
    };
    

    string _name;             // 要注册的集群名称
    string _cluster_append;   /* 集群的附加信息 */
    string _appendinfo;
    map<string, string> _tc_ifs_info; // TC 所有具有 IP 的 interface 信息, 目前仅 name 和 ip
};

/*****************************************************************************/
/* Agent向Cluster manger 和 manger 向 CC发送注销请求时候的消息体结构 */
/*该消息结构的消息ID为:EV_CLUSTER_UNREGISTER_REQ和
                       EV_CLUSTER_ACTION_UNREG_REQ*/
/*****************************************************************************/
class MessageClusterUnRegisterReq : public Serializable
{
public:
    MessageClusterUnRegisterReq() {};
    MessageClusterUnRegisterReq(const string& s)
    {
        _name = s;
    }
    ~MessageClusterUnRegisterReq() {};
    
    SERIALIZE 
    {
        SERIALIZE_BEGIN(MessageClusterUnRegisterReq);
        WRITE_STRING(_name);
        WRITE_STRING(_appendinfo);
        SERIALIZE_END();
    };
    
    DESERIALIZE  
    {
        DESERIALIZE_BEGIN(MessageClusterUnRegisterReq);
        READ_STRING(_name);
        READ_STRING(_appendinfo);
        DESERIALIZE_END();
    };
    

    string _name;              // 要删除注册的集群名称
    string _appendinfo;       
};

/*****************************************************************************/
/* Agent向Cluster manger 发送集群信息查询请求时候的消息体结构 */
/*该消息结构的消息ID为: EV_CLUSTER_INFO_REQ 和
                        EV_CLUSTER_ACTION_INFO_REQ */
/*****************************************************************************/
class MessageClusterInfoReq : public Serializable
{
public:
    MessageClusterInfoReq() {};
    MessageClusterInfoReq(int32 ReqMode)
    {
        _ReqMode = ReqMode;
        _StartIndex = 0;
        _Query_count = 0;
    };
    ~MessageClusterInfoReq() {};
    
    SERIALIZE 
    {
        SERIALIZE_BEGIN(MessageClusterInfoReq);
        WRITE_DIGIT(_ReqMode);
        WRITE_DIGIT(_StartIndex);
        WRITE_DIGIT(_Query_count);
        WRITE_STRING(_appendinfo);
        SERIALIZE_END();
    };
    
    DESERIALIZE  
    {
        DESERIALIZE_BEGIN(MessageClusterInfoReq);
        READ_DIGIT(_ReqMode);
        READ_DIGIT(_StartIndex);
        READ_DIGIT(_Query_count);
        READ_STRING(_appendinfo);
        DESERIALIZE_END();
    };

    int32  _ReqMode;            // 请求模式，0表示异步请求，1表示同步请求
    int64  _StartIndex;         // 查询的起始位置
    int64  _Query_count;        // 查询数量
    string _appendinfo;         // 存放的是发送过来的消息单元，
};

/*****************************************************************************/
/* Agent向Cluster manger 发送集群信息查询请求时候的消息体结构 */
/*该消息结构的消息ID为: EV_CLUSTER_INFO_REQ 和
                        EV_CLUSTER_ACTION_INFO_REQ */
/*****************************************************************************/
class MessageClusterNetPlanReq : public Serializable
{
public:
    MessageClusterNetPlanReq() {};
    MessageClusterNetPlanReq(string name)
    {
        _cluster_name = name;
    };
    ~MessageClusterNetPlanReq() {};
    
    SERIALIZE 
    {
        SERIALIZE_BEGIN(MessageClusterInfoReq);
        WRITE_STRING(_cluster_name);
        SERIALIZE_END();
    };
    
    DESERIALIZE  
    {
        DESERIALIZE_BEGIN(MessageClusterInfoReq);
        READ_STRING(_cluster_name);
        DESERIALIZE_END();
    };

    string _cluster_name;         // 查询的集群名称，空字符串表示查询所有集群的网络平面
};


/*****************************************************************************/
/* Cluster manger 向Agent发送集群信息查询应答的消息体结构 */
/*该消息结构的消息ID为: EV_CLUSTER_INFO_ACK 和
                        EV_CLUSTER_ACTION_INFO_ACK */
/*****************************************************************************/
class MessageClusterInfoAck : public Serializable
{
public:
    MessageClusterInfoAck() 
    {
        _next_index = -1;
        _max_count = 0;
    }

    ~MessageClusterInfoAck() {};
    
    SERIALIZE 
    {
        SERIALIZE_BEGIN(MessageClusterInfoAck);
        WRITE_DIGIT(_next_index);
        WRITE_DIGIT(_max_count);
        WRITE_OBJECT_VECTOR(_cluster_resource);
        SERIALIZE_END();    
    };
    
    DESERIALIZE 
    {
        DESERIALIZE_BEGIN(MessageClusterInfoAck);
        READ_DIGIT(_next_index);
        READ_DIGIT(_max_count);
        READ_OBJECT_VECTOR(_cluster_resource);
        DESERIALIZE_END();    
    };
    
    
    int64  _next_index;
    int64  _max_count;
    vector <ClusterInfo> _cluster_resource;

};


/*****************************************************************************/
/* Cluster manger 向Agent发送集群信息查询应答的消息体结构 */
/*该消息结构的消息ID为:  EV_CLUSTER_ACTION_INFO_ACK */
/*****************************************************************************/
class MessageClusterQueryCoreTcuAck : public Serializable
{
public:
    MessageClusterQueryCoreTcuAck(int32 corenum, int32 tcu_num) 
    {
        _core_num = corenum;
        _tcu_num = tcu_num;
    }

    ~MessageClusterQueryCoreTcuAck() {};
    
    SERIALIZE 
    {
        SERIALIZE_BEGIN(MessageClusterQueryCoreTcuAck);
        WRITE_DIGIT(_core_num);
        WRITE_DIGIT(_tcu_num);
        SERIALIZE_END();    
    };
    
    DESERIALIZE 
    {
        DESERIALIZE_BEGIN(MessageClusterQueryCoreTcuAck);
        READ_DIGIT(_core_num);
        READ_DIGIT(_tcu_num);
        DESERIALIZE_END();    
    };
    
    int32  _core_num;
    int32  _tcu_num;

};


class MessageClusterQueryNetPlaneAck : public Serializable
{
public:
    MessageClusterQueryNetPlaneAck(vector<string> &net_plane) 
    {
        _net_plane = net_plane;
    }

    MessageClusterQueryNetPlaneAck(){};

    ~MessageClusterQueryNetPlaneAck() {};
    
    SERIALIZE 
    {
        SERIALIZE_BEGIN(MessageClusterQueryNetPlaneAck);
        WRITE_VALUE(_net_plane);
        SERIALIZE_END();    
    };
    
    DESERIALIZE 
    {
        DESERIALIZE_BEGIN(MessageClusterQueryNetPlaneAck);
        READ_VALUE(_net_plane);
        DESERIALIZE_END();    
    };
    
    vector<string> _net_plane;

};



/*****************************************************************************/
/*  CC向 Manger发送集群当前信息的消息体结构 */
/*该消息结构的消息ID为: EV_CLUSTER_INFO_REPORT/EV_CLUSTER_DISCOVER */
/*****************************************************************************/
class MessageClusterInfoReport : public Serializable
{
public:
    MessageClusterInfoReport() {};
    ~MessageClusterInfoReport() {};
    
    SERIALIZE 
    {
        SERIALIZE_BEGIN(MessageClusterInfoReport);
        WRITE_DIGIT(_core_free_max);
        WRITE_DIGIT(_tcu_unit_free_max);
        WRITE_DIGIT(_tcu_max_total);
        WRITE_DIGIT(_tcu_free_total);
        WRITE_DIGIT(_tcu_free_max);    
        WRITE_DIGIT(_disk_max_total);
        WRITE_DIGIT(_disk_free_total);
        WRITE_DIGIT(_disk_free_max);
        WRITE_DIGIT(_mem_max_total);
        WRITE_DIGIT(_mem_free_total);
        WRITE_DIGIT(_mem_free_max);
        WRITE_DIGIT(_cpu_usage_average);    
        WRITE_DIGIT(_cpu_usage_max);
        WRITE_DIGIT(_cpu_usage_min);
        WRITE_STRING_MAP(_node);
        WRITE_STRING(_db_name);
        WRITE_STRING(_db_server_url);    
        WRITE_DIGIT(_db_server_port);
        WRITE_STRING(_db_user);
        WRITE_STRING(_db_passwd);
        WRITE_DIGIT(_host_count);
        WRITE_STRING(_img_usage);
        WRITE_STRING(_cluster_verify_id);
        WRITE_STRING(_img_srctype);
        WRITE_DIGIT(_img_spcexp);
        WRITE_STRING(_share_img_usage);
        SERIALIZE_END();    
    };
    
    DESERIALIZE 
    {
        DESERIALIZE_BEGIN(MessageClusterInfoReport);
        READ_DIGIT(_core_free_max);
        READ_DIGIT(_tcu_unit_free_max);
        READ_DIGIT(_tcu_max_total);
        READ_DIGIT(_tcu_free_total);
        READ_DIGIT(_tcu_free_max);    
        READ_DIGIT(_disk_max_total);
        READ_DIGIT(_disk_free_total);
        READ_DIGIT(_disk_free_max);
        READ_DIGIT(_mem_max_total);
        READ_DIGIT(_mem_free_total);
        READ_DIGIT(_mem_free_max);
        READ_DIGIT(_cpu_usage_average);    
        READ_DIGIT(_cpu_usage_max);
        READ_DIGIT(_cpu_usage_min);
        READ_STRING_MAP(_node);
        READ_STRING(_db_name);
        READ_STRING(_db_server_url);    
        READ_DIGIT(_db_server_port);
        READ_STRING(_db_user);
        READ_STRING(_db_passwd);
        READ_DIGIT(_host_count);
        READ_STRING(_img_usage);
        READ_STRING(_cluster_verify_id);
        READ_STRING(_img_srctype);
        READ_DIGIT(_img_spcexp);
        READ_STRING(_share_img_usage);
        DESERIALIZE_END();    
    };
    

    int32 _core_free_max;      // 所有状态正常主机中最大核数
    int32 _tcu_unit_free_max;  // 所有状态正常主机中最大的计算能力数

    int32 _tcu_max_total;       // 所有状态正常主机的总计算能力
    int32 _tcu_free_total;      // 所有状态正常主机的空闲计算能力
    int32 _tcu_free_max;        // 所有状态正常主机中剩余最大的计算能力
    
    int64 _disk_max_total;      // 所有状态正常主机的本地磁盘总容量的总和，单位B
    int64 _disk_free_total;     // 所有状态正常主机的本地磁盘空闲容量的总和，单位B
    int64 _disk_free_max;       // 所有状态正常主机的本地磁盘空闲容量的最大值，单位B
  
    
    int64 _mem_max_total;       // 所有状态正常主机的内存总容量的总和，单位B
    int64 _mem_free_total;      // 所有状态正常主机的内存空闲容量的总和，单位B
    int64 _mem_free_max;        // 所有状态正常主机的内存空闲容量的最大值，单位B
   
    int32 _cpu_usage_average;   // 所有状态正常主机的30分钟物理cpu利用率平均值
    int32 _cpu_usage_max;       // 所有状态正常主机的30分钟物理cpu利用率最大值
    int32 _cpu_usage_min;       // 所有状态正常主机的30分钟物理cpu利用率最小值

    //string _ip_address;         // 集群当前的IP地址，随着CC的倒换会发生变化
    map<int, string>  _node;     // 主备节点信息，无主备配置时，只有一个即为主

    string _db_name;         //数据库名
    string _db_server_url;   // 服务器URL
    int32 _db_server_port;       // 服务器端口
    string _db_user;       // 用户名
    string _db_passwd;   // 密码

    int32 _host_count;    //host数量
    
    string _img_usage;    //镜像使用方式，快照或者非快照
    string _cluster_verify_id;    //集群身份验证码
    string _img_srctype; //虚拟机使用镜像文件类型 文件或者块
    int _img_spcexp; //镜像存放的块设备大小扩展比例
    string _share_img_usage; //共享镜像使用方式，快照或者非快照
};

/*****************************************************************************/
/* Agent向Cluster manger 发送集群维护的消息体结构 */
/* 该消息结构的消息ID为：EV_CLUSTER_ACTION_START_REQ 和 EV_CLUSTER_ACTION_STOP_REQ */
/*****************************************************************************/
class MessageClusterRunStateModifyReq : public Serializable
{
public:
    MessageClusterRunStateModifyReq(const string& name)
    {
        _name = name;
    }
    MessageClusterRunStateModifyReq(){};
    ~MessageClusterRunStateModifyReq() {};
    
    SERIALIZE 
    {
        SERIALIZE_BEGIN(MessageClusterRunStateModifyReq);
        WRITE_STRING(_name);
        SERIALIZE_END();
    };
    
    DESERIALIZE  
    {
        DESERIALIZE_BEGIN(MessageClusterRunStateModifyReq);
        READ_STRING(_name);
        DESERIALIZE_END();
    };
    

    string _name;             // 集群名称
};

/*****************************************************************************/
/* Cluster manger 向 Agent发送集群维护的消息应答体结构 */
/* 该消息结构的消息ID为：EV_CLUSTER_ACTION_START_ACK 和 EV_CLUSTER_ACTION_STOP_ACK */
/*****************************************************************************/
class MessageClusterRunStateModifyAck : public Serializable
{
public:
    MessageClusterRunStateModifyAck(const string& result)
    {
        _result = result;
    }
    MessageClusterRunStateModifyAck(){};
    ~MessageClusterRunStateModifyAck() {};
    
    SERIALIZE 
    {
        SERIALIZE_BEGIN(MessageClusterRunStateModifyAck);
        WRITE_DIGIT(_err_code);
        WRITE_STRING(_result);
        SERIALIZE_END();
    };
    
    DESERIALIZE  
    {
        DESERIALIZE_BEGIN(MessageClusterRunStateModifyAck);
        READ_DIGIT(_err_code);
        READ_STRING(_result);
        DESERIALIZE_END();
    };
    

    int    _err_code;           // 错误码
    string _result;             // 集群名称
};

} //namespace zte_tecs


#endif // COMM_CLUSTER_MANAGER_WITH_HOST_MANAGER_H


