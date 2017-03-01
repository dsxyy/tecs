/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�cluster_manager_with_vm_config.h
* �ļ���ʶ��
* ����ժҪ����Ⱥ��������������õ����ͷ�ļ�
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
#ifndef TC_CLUSTER_MANAGER_WITH_VM_CONFIG_H
#define TC_CLUSTER_MANAGER_WITH_VM_CONFIG_H
#include "sky.h"
#include "workflow.h"

namespace zte_tecs
{
/*****************************************************************************/
/* �û���Cluster manger������Դ�������� */
/*����Ϣ�ṹ����ϢIDΪ: EV_CLUSTER_ACTION_FIND_REQ */
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


    int32    _cpunum;            // �����CPU����
    int32    _tcu_num;           // TCU������
    int64    _memsize;           // ������ڴ��С
    int64    _disksize;    // ����ı���Ӳ�̴�С
    string   _appointed_cluster;      // ָ����ѯ�ļ�Ⱥ����

};

/*****************************************************************************/
/* Cluster manger���û�������Դ���ҽ�������ز��ҵ��ļ�Ⱥ���� */
/* ����Ϣ�ṹ����ϢIDΪ: EV_CLUSTER_ACTION_FIND_ACK */
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

     vector<string>   _cluster_list;       // ���ҵ��ļ�Ⱥ����
};


} //namespace zte_tecs


#endif // end TC_CLUSTER_MANAGER_WITH_VM_CONFIG_H

