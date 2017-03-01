/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�cluster_api.h
* �ļ���ʶ��
* ����ժҪ����Ⱥ����ģ�����ӿ�
* ��ǰ�汾��1.0
* ��    �ߣ�lixiaocheng
* ������ڣ�2011��10��19��
*
* �޸ļ�¼1��
*    �޸����ڣ�2011/10/19
*    �� �� �ţ�V1.0
*    �� �� �ˣ�lixiaocheng
*    �޸����ݣ�����
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
    
    string     cluster_name;                     // ��Ⱥ����
    string     ip;                               // ��Ⱥ��IP
    string     cluster_append;                   // ��Ⱥ������Ϣ
    bool       is_online;                        // ��Ⱥ����״̬
    bool       enabled;                         //ʹ��״̬
    int        core_free_max;                    // ��Ⱥӵ�еĿ��к���
    int        tcu_unit_free_max;                // ��Ⱥ�е���ӵ�е�����������
    int        tcu_max_total;                    // ��ȺTCU����
    int        tcu_free_total;                   // ��ȺTCU��������
    int        tcu_free_max;                     // ��Ⱥ��HOST��TCU��������������
    long long  disk_max_total;                   // �����������
    long long  disk_free_total;                  // ���̿�������
    long long  disk_free_max;                    // ������������
    long long  share_disk_max_total;             // ��������ܴ�С
    long long  share_disk_free_total;            // ������̿����ܴ�С
    long long  mem_max_total;                    // �ڴ���ܴ�С
    long long  mem_free_total;                   // ʣ���ڴ��ܺ�
    long long  mem_free_max;                     // ��Ⱥ�е�HCӵ�е�����ڴ�ֵ
};

#endif // TECS_CLUSTER_API_H

