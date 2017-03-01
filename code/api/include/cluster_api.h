/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：cluster_api.h
* 文件标识：
* 内容摘要：集群管理模块对外接口
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
#ifndef TECS_CLUSTER_API_H
#define TECS_CLUSTER_API_H
#include "rpcable.h"
#include "api_error.h"


class ApiCluster: public Rpcable
{
public:
    ApiCluster() 
    {
        is_online = false;
        enabled   = false;
        core_free_max = 0;
        tcu_unit_free_max = 0;
        tcu_max_total = 0;
        tcu_free_total = 0;
        tcu_free_max = 0;
    };
    ~ApiCluster() {};

    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_VALUE(cluster_name);
        TO_VALUE(ip);
        TO_VALUE(cluster_append);
        TO_VALUE(is_online);
        TO_VALUE(enabled);
        TO_VALUE(core_free_max);
        TO_VALUE(tcu_unit_free_max);
        TO_VALUE(tcu_max_total);
        TO_VALUE(tcu_free_total);
        TO_VALUE(tcu_free_max);
        
        TO_I8(disk_max_total);
        TO_I8(disk_free_total);
        TO_I8(disk_free_max);
        
        TO_I8(share_disk_max_total);
        TO_I8(share_disk_free_total);
        
        TO_I8(mem_max_total);
        TO_I8(mem_free_total);
        TO_I8(mem_free_max);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_VALUE(cluster_name);
        FROM_VALUE(ip);
        FROM_VALUE(cluster_append);
        FROM_VALUE(is_online);
        FROM_VALUE(enabled);
        FROM_VALUE(core_free_max);
        FROM_VALUE(tcu_unit_free_max);
        FROM_VALUE(tcu_max_total);
        FROM_VALUE(tcu_free_total);
        FROM_VALUE(tcu_free_max);
        
        FROM_I8(disk_max_total);
        FROM_I8(disk_free_total);
        FROM_I8(disk_free_max);
        
        FROM_I8(share_disk_max_total);
        FROM_I8(share_disk_free_total);
        
        FROM_I8(mem_max_total);
        FROM_I8(mem_free_total);
        FROM_I8(mem_free_max);
        FROM_RPC_END();
    };
    
    string     cluster_name;                     // 集群名称
    string     ip;                               // 集群的IP
    string     cluster_append;                   // 集群描述信息
    bool       is_online;                        // 集群运行状态
    bool       enabled;                         //使能状态
    int        core_free_max;                    // 集群拥有的空闲核数
    int        tcu_unit_free_max;                // 集群中单核拥有的最大计算能力
    int        tcu_max_total;                    // 集群TCU总量
    int        tcu_free_total;                   // 集群TCU空闲总量
    int        tcu_free_max;                     // 集群单HOST，TCU总量的最大空闲数
    long long  disk_max_total;                   // 磁盘最大总量
    long long  disk_free_total;                  // 磁盘空闲总数
    long long  disk_free_max;                    // 磁盘最大空闲数
    long long  share_disk_max_total;             // 共享磁盘总大小
    long long  share_disk_free_total;            // 共享磁盘空闲总大小
    long long  mem_max_total;                    // 内存的总大小
    long long  mem_free_total;                   // 剩余内存总和
    long long  mem_free_max;                     // 集群中单HC拥有的最大内存值
};

#endif // TECS_CLUSTER_API_H

