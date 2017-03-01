/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�cluster_pool.h
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��cluster���ݹ������������ļ�
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
@brief ��������: ���ݼ�Ⱥ���ƣ��ڼ�Ⱥ���д���һ����Ա
    
@li @b ����б�
@verbatim
string& cluster_name    ����ļ�Ⱥ����
@endverbatim
      
@li @b �����б�
@verbatim
int64 *  oid           ���ص������е�����oid
string&  error_str     �������ʧ�ܣ����صĴ���ԭ��
@endverbatim
        
@li @b ����ֵ�б�
@verbatim
0       �ɹ�
-1      ʧ��
@endverbatim
          
@li @b �ӿ����ƣ���
@li @b ����˵������
*/
/*****************************************************************************/
    int64 Allocate(int64 *  oid, 
                     const string &cluster_name, 
                     const string &cluster_appendinfo, 
                     string &error_str);

/*****************************************************************************/
/**
@brief ��������: ���ݼ�Ⱥ���ƣ���ȡһ����Ⱥ�Ľṹ

@li @b ����б�
@verbatim
const string&  cluster_name     ��Ⱥ����
int32  lock                      true ��ʾ������false��ʾ������
@endverbatim
      
@li @b �����б�
@verbatim
��
@endverbatim
        
@li @b ����ֵ�б�
@verbatim
����һ����Ⱥ���ָ��
@endverbatim
          
@li @b �ӿ����ƣ���
@li @b ����˵������
*/
/*****************************************************************************/
    Cluster * GetCluster(const string&  cluster_name, int32  lock);

/*****************************************************************************/
/**
@brief ��������: ͨ��CPU�������ڴ��С��Ӳ�̴�С������һ����Ⱥ

@li @b ����б�
@verbatim
int32 cpu_num     ָ��CPU����
int64 mem_size    ָ���ڴ��С
int64 disk_size   ָ��Ӳ�̴�С
@endverbatim
      
@li @b �����б�
@verbatim
��
@endverbatim
        
@li @b ����ֵ�б�
@verbatim
����һ����Ⱥ���ָ��
@endverbatim
          
@li @b �ӿ����ƣ���
@li @b ����˵������
*/
/*****************************************************************************/
    Cluster * FindCluster(int32  cpu_num, int32 tcu_num, int64 mem_size, int64 disk_size, int64 share_disk_size, int32 *findnum);
    int       FindCluster(const string &where, vector<Cluster*> &result);
   

    Cluster   *SelectClusterUsePolicy(const vector<Cluster*> &cluster_in);
    Cluster   *SelectClusterUsePolicy(const vector<Cluster*> &cluster_in,const vector<string> &cluster_name);

    // ��ȡ��ǰ��Ⱥ��������������
    int FindHcMaxCore();

    // ��ȡ��ǰ��Ⱥ�����������TCU��
    int FindHcMaxTcu();


    int GetClusterNamesWithEnoughResource(const string & where,vector<string> &cluster_name);

/*****************************************************************************/
/**
@brief ��������: ͨ������ʱ�䣬ˢ�µ�ǰ���ݿ��м�Ⱥ��״̬

@li @b ����б�
@verbatim
now_time  ��ǰˢ�µ�ϵͳʱ��
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
void RefreshCluster(time_t now_time);

/*****************************************************************************/
/**
@brief ��������: ����Դ�������еļ�Ⱥ�����ֶ���ȡ����
    
@li @b ����б�
@verbatim
��
@endverbatim
      
@li @b �����б�
@verbatim
vector<string> *cluster_name_tab   ������пɼ��ļ�Ⱥ����
@endverbatim
        
@li @b ����ֵ�б�
@verbatim
��
@endverbatim
          
@li @b �ӿ����ƣ���
@li @b ����˵������
*/
/*****************************************************************************/
void GetAllClusterName(vector<string> &cluster_name_tab);

/*****************************************************************************/
/**
@brief ��������: ��ָ����Ⱥ�����еĳ�Ա���ݸ��µ�������
    
@li @b ����б�
@verbatim
Cluster * cluster    ��Ⱥ��ָ��
@endverbatim
      
@li @b �����б�
@verbatim
��
@endverbatim
        
@li @b ����ֵ�б�
@verbatim
0   �ɹ�
-1  ʧ��
@endverbatim
          
@li @b �ӿ����ƣ���
@li @b ����˵������
*/
/*****************************************************************************/
    int32 Update(Cluster * cluster);

/*****************************************************************************/
/**
@brief ��������: ��ָ����Ⱥ�����ݴ����ݿ���ɾ��
    
@li @b ����б�
@verbatim
Cluster * cluster    ��Ⱥ��ָ��
@endverbatim
      
@li @b �����б�
@verbatim
��
@endverbatim
        
@li @b ����ֵ�б�
@verbatim
0    �ɹ�
-1   ʧ��
@endverbatim
          
@li @b �ӿ����ƣ���
@li @b ����˵������
*/
/*****************************************************************************/
    int32 Drop(Cluster * cluster);

/*****************************************************************************/
/**
@brief ��������: ����ָ���ֶΣ������ѯ������ַ���
    
@li @b ����б�
@verbatim
const string& where     �����ѯ����
@endverbatim
      
@li @b �����б�
@verbatim
ostringstream& oss      �����������ַ�����
@endverbatim
        
@li @b ����ֵ�б�
@verbatim
0   �ɹ�
-1  ʧ��
@endverbatim
          
@li @b �ӿ����ƣ���
@li @b ����˵������
*/
/*****************************************************************************/
    int32 Dump(ostringstream& oss, const string& where);


/*****************************************************************************/
/**
@brief ��������: ����δע�ἯȺ��δ�յ����ɾ��ʱ��ֵ
    
@li @b ����б�
@verbatim
int32 unregedit_keep_time ʱ��ֵ,��λS
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
void set_unregedit_keep_time(int32 unregedit_keep_time)
{
    _unregedit_keep_time = unregedit_keep_time;
}

/*****************************************************************************/
/**
@brief ��������: ����ע�ἯȺ��δ�յ��������ʱ��ֵ
    
@li @b ����б�
@verbatim
int32 regedit_keep_time ʱ��ֵ����λS
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
void set_regedit_keep_time(int32 regedit_keep_time)
{
    _regedit_keep_time = regedit_keep_time;
}

private:    
    static ClusterPool *instance; 

    ClusterPool(SqlDB *db):PoolSQL(db,Cluster::_table)
    {

    };
    
    map<string,int32> _cluster_names;       /* ��Ⱥ���ƺ�oid֮���ӳ���ϵ */
    int32             _unregedit_keep_time; /* ���ݿ�س�Աδע��ı���ʱ�� */
    int32             _regedit_keep_time;   /* ���ݿ�س�Աע��ı���ʱ�� */

/*****************************************************************************/
/**
@brief ��������: ��Ⱥ�ؽ������ص�����
    
@li @b ����б�
@verbatim
SqlColumn * columns    ��ѯ�Ľ����Աָ��
@endverbatim
      
@li @b �����б�
@verbatim
void * _oss            �����������ָ��
@endverbatim
        
@li @b ����ֵ�б�
@verbatim
0    �ɹ�
-1   ʧ��
@endverbatim
          
@li @b �ӿ����ƣ���
@li @b ����˵������
*/
/*****************************************************************************/
    int32 Dump_Callback(void * oss, SqlColumn * columns);

/*****************************************************************************/
/**
@brief ��������: ��Ⱥ�س�ʼ���ص�����
    
@li @b ����б�
@verbatim
void *nil              ���ݿ��ָ��
SqlColumn * columns    ��ѯ���ָ��
@endverbatim
      
@li @b �����б�
@verbatim
��
@endverbatim
        
@li @b ����ֵ�б�
@verbatim
0    �ɹ�
-1   ʧ��
@endverbatim
          
@li @b �ӿ����ƣ���
@li @b ����˵�����ýӿ�Ϊ��Ⱥ�ص��ã��൱�ڰ����ݿ��еļ�Ⱥ���ƺ�oid�Ķ�Ӧ��ϵ
                 �ڼ�Ⱥ���г�ʼ��һ��mapӳ���ϵ
*/
/*****************************************************************************/
    int32 Init_Callback(void *nil, SqlColumn * columns);

    // ��ȡ��Ⱥ��Դ����Int���͵Ļص�
    int32 SelectInt_Callback(void *nil, SqlColumn * columns); 

/*****************************************************************************/
/**
@brief ��������: ʵ�ֻ�����creat���麯������ҪΪ���������һ��ʵ��
    
@li @b ����б�
@verbatim
��
@endverbatim
      
@li @b �����б�
@verbatim
��
@endverbatim
        
@li @b ����ֵ�б�
@verbatim
NULL    ����ʧ��
�ǿ�    �����ɹ�ʵ���ָ��
@endverbatim
          
@li @b �ӿ����ƣ���
@li @b ����˵������
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
