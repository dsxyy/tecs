/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�cluster_manager.h
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��cluster_manager��������ļ�
* ��ǰ�汾��1.0
* ��    �ߣ���Т��
* ������ڣ�2011��7��25��
*
* �޸ļ�¼1��
*     �޸����ڣ�2011/7/25
*     �� �� �ţ�V1.0
*     �� �� �ˣ���Т��
*     �޸����ݣ�����
******************************************************************************/
#ifndef TC_CLUSTER_MANAGER_H
#define TC_CLUSTER_MANAGER_H
#include "sky.h"
#include "cluster_pool.h"
#include "mid.h"
#include "event.h"

namespace zte_tecs
{
/* ���弯Ⱥ������ */
#define CLUSTER_CC_CLUSTER_AGENT_UNIT_NAME   MU_CLUSTER_AGENT    // CC������������Ϣ��Ԫ����
#define CLUSTER_PERIOD_TIME_LEN            10000              // ��Ⱥ�ϻ���ʱ��ʱ����10s

#define CLUSTER_QUERY_SYN                  1                  // ͬ����ѯ
#define CLUSTER_QUERY_ASYN                 0                  // �첽��ѯ

/* �ⲿͬ����ѯ��Ⱥ����Ự����ʱ�ṹ */
class ClusterResourceSynReq 
{
    public:
        MID    _sender;
        int64  _start_index;
        int64  _query_count;
};


/**
@brief ��������: ��Ⱥ��Դ����ģ����࣬��Ҫ������Ⱥ��ע�������Ⱥ״̬����
@li @b ����˵������
*/
class ClusterManager: public MessageHandler
{
public:
    static ClusterManager *GetInstance()
    {
        if(NULL == instance)
        {
            instance = new ClusterManager();
        }
        
        return instance;
    };

    STATUS Init()
    {
        _cpool = ClusterPool::GetInstance();
        if(!_cpool)
        {
            return ERROR_NOT_READY;
        }

        _manager_period_timer = 0;
        _req_cluster_timer    = 0;
        _req_cluster_mid_tab.clear();
        _ClusterNameTab.clear();
        
        //������Ϣ��Ԫ�����߳�
        return StartMu(MU_CLUSTER_MANAGER);
    }
   
    ~ClusterManager()
    {
        delete _mu;
    }
    
    void MessageEntry(const MessageFrame& message);

    void SetIp(string ip)
    {
        _ip_connect_lowstream = ip;
    }

    string GetIp()
    {
        return _ip_connect_lowstream;
    }

private:
    ClusterManager()
    {
        _cpool = NULL;
        _mu = NULL;
    }
    
    static ClusterManager               *instance;
    ClusterPool                         * _cpool;
    MessageUnit                         * _mu;
    TIMER_ID                            _manager_period_timer;
    vector<ClusterResourceSynReq>       _req_cluster_mid_tab;
    map<string,int>                     _ClusterNameTab;
    TIMER_ID                            _req_cluster_timer;
    string                              _ip_connect_lowstream;
    map<string, string>                 _ifs_info;

    STATUS StartMu(const string& name);

    void doClusterDiscover(const MessageFrame& message);

    void UpdateClusterInfo(const string &cluster_name, const MessageFrame& message);
    
    void doAgentRegCluster(const MessageFrame& message);//�ú�������������

    void doAgentSetCluster(const MessageFrame& message);
    
    void doClusterRegAck(const MessageFrame& message);

    void doAgentUnRegCluster(const MessageFrame& message);//�ú�������������
    
    void doAgentForgetCluster(const MessageFrame& message);    

    void doAgentQueryClusterInfo(const MessageFrame& message);

    void doClusterInfoReport(const MessageFrame& message);

    void doVmCfgFindCluster(const MessageFrame& message);

    void doAgentStopCluster(const MessageFrame& message);

    void doAgentStartCluster(const MessageFrame& message);

    void doAgentGetClusterCoreAndTcu(const MessageFrame& message);

    
/*****************************************************************************/
/**
@brief ��������: ��Ⱥ��Դ��ѯӦ���ͺ���
    
@li @b ����б�
@verbatim
sender    ����������Ϣ�������ߵ�ַ  
@endverbatim
      
@li @b �����б�
@verbatim
�� 
@endverbatim
        
@li @b ����ֵ�б�
@verbatim
��
@endverbatim
          
@li @b �ӿ����ƣ���
@li @b ����˵������
*/
/*****************************************************************************/   
    void ClusterResourceAckSend(ClusterResourceSynReq &ReqInfo);
 
/*****************************************************************************/
/**
@brief ��������: ��Ⱥ������̸�����CC���ͼ�Ⱥ��Դ�ϱ�����
    
@li @b ����б�
@verbatim
sender    ����������Ϣ�������ߵ�ַ  
@endverbatim
      
@li @b �����б�
@verbatim
�� 
@endverbatim
        
@li @b ����ֵ�б�
@verbatim
��
@endverbatim
          
@li @b �ӿ����ƣ���
@li @b ����˵������
*/
/*****************************************************************************/    
    void ResourceReqSendToAllCluster(ClusterResourceSynReq &ReqInfo);

/*****************************************************************************/
/**
@brief ��������: Ϊ��Ⱥ�����������һ��ѭ����ʱ����
    
@li @b ����б�
@verbatim
sender    ����������Ϣ�������ߵ�ַ  
@endverbatim
      
@li @b �����б�
@verbatim
�� 
@endverbatim
        
@li @b ����ֵ�б�
@verbatim
��
@endverbatim
          
@li @b �ӿ����ƣ���
@li @b ����˵�����ú������ö�Σ�Ҳֻ������һ��ѭ����ʱ��
*/
/*****************************************************************************/      
    void SetClusterKeepTimer();
};


} /* end namespace zte_tecs */

#endif /* end TC_CLUSTER_MANAGER_H */

