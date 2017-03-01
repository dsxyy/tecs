/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�cluster_pool.cpp
* �ļ���ʶ��
* ����ժҪ��cluster_pool��Ķ����ļ�
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
@brief ��������: ��Ⱥ�ع��캯��
    
@li @b ����б�
@verbatim
SqlDB * db       ���ݿ�ָ��
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
STATUS ClusterPool::Init()
{
    ostringstream   sql;
    int             rc = SQLDB_OK;
    
    SetCallback(static_cast<Callbackable::Callback>(&ClusterPool::Init_Callback));
    sql  << "select oid, name from " <<  Cluster::_table;
    rc = PoolSQL::_db->Execute(sql, this);
    UnsetCallback();
    _unregedit_keep_time = 60;    /* ��Ⱥ��δע���ʱ��ʱ���������ɾ����¼��ʱ�� 60s */
    _regedit_keep_time   = 60;    /* ��Ⱥ�����ߵ����ߵļ��ʱ��60s */

    if (SQLDB_OK == rc || SQLDB_RESULT_EMPTY == rc)
    {
        return SUCCESS;
    }
    
    OutPut(SYS_EMERGENCY, "ClusterPool execute db fail ,err code : %d", rc);
    return ERROR;
}

/*****************************************************************************/
/**
@brief ��������: ���ݼ�Ⱥ���ƣ��ڼ�Ⱥ���д���һ����Ա
    
@li @b ����б�
@verbatim
string& cluster_name    ����ļ�Ⱥ����
@endverbatim
      
@li @b �����б�
@verbatim
int32 *  oid           ���ص������е�����oid
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
Cluster* ClusterPool::GetCluster(const string&  cluster_name, int32  lock)
{
    map<string, int>::iterator       index;

    index = _cluster_names.find(cluster_name);

    if (index != _cluster_names.end())
    {
        /* �������Get�������ǵ��û����е�map����ȡָ�� */
        return static_cast<Cluster*>(PoolSQL::Get(index->second, lock));
    }

    return NULL;
}


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
    
    /* ���������ĿΪ0����û�в��ҵ���Դ */
    if (0 == *findnum)
    {
        return NULL;
    }
    /* ��ȡһ����ǰ��ϵͳ�������� */
    time(&temp);
    
    /* ���������ȡ��ǰλ�� */
    postion = temp%(*findnum);

    return static_cast<Cluster*>(PoolSQL::Get(oid.at(postion), false));
}


/*****************************************************************************/
/**
@brief ��������: ͨ������������ѯ��Ⱥ
@li @b ����б�
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

    /* ���������ĿΪ0����û�в��ҵ���Դ */
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
@brief ��������: �����㷨ѡ��Ⱥ
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
Cluster   *ClusterPool::SelectClusterUsePolicy(const vector<Cluster*> &cluster_in)
{
    time_t          temp;
    uint32          findnum = cluster_in.size();
    uint32          postion;

    if (findnum == 0)
    {
        return NULL;
    }
    
    /* ��ȡһ����ǰ��ϵͳ�������� */
    time(&temp);
    
    /* ���������ȡ��ǰλ�� */
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

    /* ��ȡһ����ǰ��ϵͳ�������� */
    time(&temp);

    /* ���������ȡ��ǰλ�� */
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
            {    /* ע����ļ�Ⱥ����һ��ʱ��û���յ����������̬����OFFLINE */ 
                if ((now_time - pCluster->_last_moni_time.tointeger()) >= _regedit_keep_time)
                {
                    if (true == pCluster->_is_online)
                    {
                        pCluster->_is_online = false;
						OutPut(SYS_NOTICE, "cluster %s is offline\r\n", pCluster->_cluster_name.c_str());
                        /* �澯��Ⱥ������ */
                        pCluster->Alarm((int32)ALM_TECS_CLUSTER_OFF_LINE);
                        /* �����Ѿ�������� */
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
int32 ClusterPool::Update(Cluster * cluster)
{
    return cluster->Update(_db);
}

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
int32 ClusterPool::Dump_Callback(void * oss, SqlColumn * columns)
{
    ostringstream * temp_oss;
    
    temp_oss = static_cast<ostringstream *>(oss);
    
   return Cluster::Dump(*temp_oss, columns);
}

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
