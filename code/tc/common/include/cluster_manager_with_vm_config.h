/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：cluster_manager_with_vm_config.h
* 文件标识：
* 内容摘要：集群管理与虚拟机配置的相关头文件
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
#ifndef TC_CLUSTER_MANAGER_WITH_VM_CONFIG_H
#define TC_CLUSTER_MANAGER_WITH_VM_CONFIG_H
#include "sky.h"
#include "workflow.h"

namespace zte_tecs
{
/*****************************************************************************/
/* 用户向Cluster manger发送资源查找请求 */
/*该消息结构的消息ID为: EV_CLUSTER_ACTION_FIND_REQ */
/*****************************************************************************/
class FilterClusterByComputeReq : public WorkReq
{
public:
    FilterClusterByComputeReq() {};
    FilterClusterByComputeReq( const string &id_of_action,
	                                     int32  cpunum,
                                         int32  tcu_num,
                                         int64  memsize,
                                         int64  local_disk_size,
                                         const string &appointed_cluster):WorkReq(id_of_action)
    { 
         _cpunum     = cpunum;
         _tcu_num    = tcu_num;
	 _memsize    = memsize;
	 _disksize   = local_disk_size;
         _appointed_cluster = appointed_cluster;
    };
    ~FilterClusterByComputeReq() {};
    
    SERIALIZE 
    {
        SERIALIZE_BEGIN(FilterClusterByComputeReq);
        WRITE_DIGIT(_cpunum);
        WRITE_DIGIT(_tcu_num);
        WRITE_DIGIT(_memsize);
        WRITE_DIGIT(_disksize);           
        WRITE_STRING(_appointed_cluster);
        SERIALIZE_PARENT_END(WorkReq);    
    };
    
    DESERIALIZE 
    {
        DESERIALIZE_BEGIN(FilterClusterByComputeReq);
        READ_DIGIT(_cpunum);
        READ_DIGIT(_tcu_num);
        READ_DIGIT(_memsize);
        READ_DIGIT(_disksize);      
        READ_STRING(_appointed_cluster);
        DESERIALIZE_PARENT_END(WorkReq);    
    };


    int32    _cpunum;            // 请求的CPU数量
    int32    _tcu_num;           // TCU的数量
    int64    _memsize;           // 请求的内存大小
    int64    _disksize;    // 请求的本地硬盘大小
    string   _appointed_cluster;      // 指定查询的集群名称

};

/*****************************************************************************/
/* Cluster manger向用户发送资源查找结果，返回查找到的集群名称 */
/* 该消息结构的消息ID为: EV_CLUSTER_ACTION_FIND_ACK */
/*****************************************************************************/
class FilterClusterByComputeAck : public WorkAck
{
public:
    FilterClusterByComputeAck() {};
    FilterClusterByComputeAck(const vector<string> &cluster_list)
    { 
	    _cluster_list = cluster_list;
    };
    ~FilterClusterByComputeAck() {};
    
    SERIALIZE 
    {
        SERIALIZE_BEGIN(FilterClusterByComputeAck);
        WRITE_VALUE(_cluster_list);
        SERIALIZE_PARENT_END(WorkAck);    
    };
    
    DESERIALIZE 
    {
        DESERIALIZE_BEGIN(FilterClusterByComputeAck);
        READ_VALUE(_cluster_list);
        DESERIALIZE_PARENT_END(WorkAck);    
    };    

     vector<string>   _cluster_list;       // 查找到的集群名称
};


} //namespace zte_tecs


#endif // end TC_CLUSTER_MANAGER_WITH_VM_CONFIG_H

