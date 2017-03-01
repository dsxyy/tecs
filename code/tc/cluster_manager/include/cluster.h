/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�cluster.h
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��cluster��������ļ�
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
#ifndef TC_CLUSTER_H
#define TC_CLUSTER_H
#include "mid.h"
#include "event.h"
#include "pool_object_sql.h"
#include "pool_sql.h"
#include "cluster_manager_with_host_manager.h"
#include "tecs_pub.h"

// ���ÿ�������궨��
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
@brief ��������: ��Ⱥ�ඨ��
@li @b ����˵������
*/
class Cluster: public PoolObjectSQL   
{

public:
/*****************************************************************************/
/**
@brief ��������: ��Ⱥ�Ĺ��캯��
    
@li @b ����б�
@verbatim
name ��Ⱥ�����ƣ���CC�ϱ������û��·������ģ���CC��Ψһ���ʶ��
stat ��Ⱥ��ע��״̬��Ĭ����unregedit  
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
    Cluster(const string &name );
    Cluster();
    ~Cluster();
    
 /*****************************************************************************/
/**
@brief ��������: ��Ⱥ�ĸ澯�ָ�����
    
@li @b ����б�
@verbatim
alarmcode     �澯��
alarmbuf      �澯����
alarmbuf_len  �澯���ݳ���
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
void AlarmResume(int32 alarmcode);

/*****************************************************************************/
/**
@brief ��������: ��Ⱥ�ĸ澯����
    
@li @b ����б�
@verbatim
alarmcode     �澯��
alarmbuf      �澯����
alarmbuf_len  �澯���ݳ���
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
void Alarm(int32 alarmcode);

/*****************************************************************************/
/**
@brief ��������: ��ȡ�ü�Ⱥ������
    
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
���ص�ǰ��Ⱥ�����ƣ����ص�����Ϊstring 
@endverbatim
          
@li @b �ӿ����ƣ���
@li @b ����˵������
*/
/*****************************************************************************/  
    void get_name(string *name) 
    {
        *name =  _cluster_name;
    }
    
/*****************************************************************************/
/**
@brief ��������: �жϵ�ǰ��Ⱥ�Ƿ���У���û�в��������
    
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
true  �ü�Ⱥ�Ѿ�����
false �ü�Ⱥû�п���
@endverbatim
          
@li @b �ӿ����ƣ���
@li @b ����˵������
*/
/*****************************************************************************/  
bool is_free();

/*****************************************************************************/
/**
@brief ��������: ��ȡ���һ�μ�ص�ʱ��
    
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
����һ��time_t�ṹ��ʱ��
@endverbatim
          
@li @b �ӿ����ƣ���
@li @b ����˵������
*/
/*****************************************************************************/
    time_t get_last_moni_time()
    {
        return _last_moni_time.tointeger();
    }
/*****************************************************************************/
/**
@brief ��������: ��ȡ��ǰ��Ⱥע���ʱ��
    
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
����һ��time_t�ṹ��ʱ��
@endverbatim
          
@li @b �ӿ����ƣ���
@li @b ����˵������
*/
/*****************************************************************************/
    time_t get_regesiter_time()
    {
        return _register_time.tointeger();
    }
/*****************************************************************************/
/**
@brief ��������: ���µ�ǰ��Ⱥ�����¼��ʱ��
    
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
��
@endverbatim
          
@li @b �ӿ����ƣ���
@li @b ����˵�����ڿ����������յ��ü�Ⱥ�ϱ����������͵��øú���������ʱ�����
*/
/*****************************************************************************/
    void update_last_moni_time()
    {
        _last_moni_time.refresh();
    }
/*****************************************************************************/
/**
@brief ��������: ��ȡ��ǰ��Ⱥ��Դ����Ϣ
    
@li @b ����б�
@verbatim
��
@endverbatim
      
@li @b �����б�
@verbatim
cluster_resource ��ǰ��Ⱥ��Դ�Ľṹ��
@endverbatim
        
@li @b ����ֵ�б�
@verbatim
true  �ɹ�
false ʧ�ܣ�����ʧ�ܣ�һ��Ϊ��ǰ��Ⱥ����״̬�����ߵ�����£�����ʧ�ܣ�
@endverbatim
          
@li @b �ӿ����ƣ���
@li @b ����˵������
*/
/*****************************************************************************/
    bool get_cluster_resource(ClusterResType &cluster_resource);

/*****************************************************************************/
/**
@brief ��������: ���õ�ǰ��Ⱥ��Դ����Ϣ
    
@li @b ����б�
@verbatim
cluster_resource ��ǰ��Ⱥ��Դ�Ľṹ��

@endverbatim
      
@li @b �����б�
@verbatim
��
@endverbatim
        
@li @b ����ֵ�б�
@verbatim
true  �ɹ�
false ʧ��
@endverbatim
          
@li @b �ӿ����ƣ���
@li @b ����˵������
*/
/*****************************************************************************/
    bool set_cluster_resource(const ClusterResType & cluster_resource);
    
   
/*****************************************************************************/
/**
@brief ��������: ���ü�Ⱥ��ע��״̬
    
@li @b ����б�
@verbatim
cc_register_state  ���ü�Ⱥע��̬��ʹ�õ�ֵ�μӸ����е�enum reg_state �Ķ���
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
    void set_register_state(int32 cc_register_state)
    {
        ;
    }
    
/*****************************************************************************/
/**
@brief ��������: ��ȡ��Ⱥ������״̬
    
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
 bool true:���ߣ� false:���� 
@endverbatim
          
@li @b �ӿ����ƣ���
@li @b ����˵������
*/
/*****************************************************************************/
    bool get_is_online(void)
    {
        return _is_online;
    }
    
/*****************************************************************************/
/**
@brief ��������: ��ȡ��Ⱥ��ά��״̬
    
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
 bool true: ʹ�ܣ� false:ά��
@endverbatim
          
@li @b �ӿ����ƣ���
@li @b ����˵������
*/
/*****************************************************************************/
    bool get_enabled(void)
    {
        return _enabled;
    }
 
/*****************************************************************************/
/**
@brief ��������: ��ȡ��Ⱥ��IP��Ϣ
    
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
int32 ���͵�ֵ�����嶨�����ʹ�ø����е�enum run_state ��״̬����
@endverbatim
          
@li @b �ӿ����ƣ���
@li @b ����˵������
*/
/*****************************************************************************/
    int get_ip(string &ip);
/*****************************************************************************/
/**
@brief ��������: ��ȡ��Ⱥ�ĸ�����Ϣ
    
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
int32 ���͵�ֵ�����嶨�����ʹ�ø����е�enum run_state ��״̬����
@endverbatim
          
@li @b �ӿ����ƣ���
@li @b ����˵������
*/
/*****************************************************************************/
    void get_description(string *description)
    {
        *description = _description;
    }


/*****************************************************************************/
/**
@brief ��������: ���ü�Ⱥ������״̬
    
@li @b ����б�
@verbatim
 online  true: ���ߣ� false:����
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
    void set_is_online(bool online)
    {
        _is_online  = online;
    }    

/*****************************************************************************/
/**
@brief ��������: ���ü�Ⱥ��ʹ��״̬
    
@li @b ����б�
@verbatim
 online  true: ʹ�ܣ� false:ά��
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
    void set_enabled(bool enb)
    {
        _enabled  = enb;
    }    
 
/*****************************************************************************/
/**
@brief ��������: ���ü�Ⱥ�澯��־
    
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
��
@endverbatim
          
@li @b �ӿ����ƣ���
@li @b ����˵������
*/
/*****************************************************************************/
    void set_cc_alarm_flag(int32 flag)
    {
        _cc_alarm_flag |= flag;
    } 

/*****************************************************************************/
/**
@brief ��������: �����Ⱥ�澯��־
    
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
��
@endverbatim
          
@li @b �ӿ����ƣ���
@li @b ����˵������
*/
/*****************************************************************************/
    void clear_cc_alarm_flag(int32 flag)
    {
        _cc_alarm_flag &= ~flag;
    } 
    
/*****************************************************************************/
/**
@brief ��������: ���ü�Ⱥ���߸澯��־
    
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
��
@endverbatim
          
@li @b �ӿ����ƣ���
@li @b ����˵������
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

    string                   _cluster_name;      /* Cluster�� */
    string                   _description;       /* ��Ⱥ������Ϣ */
    bool                     _enabled;           /* �Ƿ�ʹ�� */
    bool                     _is_online;         /* �Ƿ����� */
    Datetime                 _register_time;     /* ��Ⱥע��ʱ�� */
    Datetime                 _last_moni_time;    /* ��Ⱥ����ʱ�� */
    ClusterResType           _cluster_resource;  /* ��Ⱥ��Я������Դ */
    vector<ClusterNode>      _cluster_node;      /* cluster�����������ڵ���Ϣ */
    int32                    _cc_alarm_flag;     /* �Ƿ�澯�ı�־ */
    string                   _cluster_verfify_id; /*  ��Ⱥ��֤�� */
 
///////////////////////////////////////////////////////////////////////////////
/*             �����public�ĺ�ֵ���壬�Լ������Ķ��� start                  */                
///////////////////////////////////////////////////////////////////////////////

    /* �澯��־,�Ժ������澯���Բ������������ֵ */
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

    /* �ó�Ա�����ݿ���е���λ�� */
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
/*             �����public�ĺ�ֵ���壬�Լ������Ķ��� end                    */                
///////////////////////////////////////////////////////////////////////////////

protected:    
    friend class ClusterPool; 

/*****************************************************************************/
/**
@brief ��������: Cluster������ݿ�ѡȡֵ����ṹ�еĺ���
    
@li @b ����б�
@verbatim
SqlDB *db    ���ݿ��ָ��
@endverbatim
      
@li @b �����б�
@verbatim
��
@endverbatim
        
@li @b ����ֵ�б�
@verbatim
0     ��ȡ���ݳɹ�
-1    select ʧ��
@endverbatim
          
@li @b �ӿ����ƣ���
@li @b ����˵�����ú��������ᱻCluster_poolͨ��sql_pool����
*/
/*****************************************************************************/
    int32 Select(SqlDB *db);

/*****************************************************************************/
/**
@brief ��������: Cluster�������ݿ��в�������
    
@li @b ����б�
@verbatim
SqlDB *db          ���ݿ��ָ��
string & errorStr  ������󣬷��صĴ���ԭ��
@endverbatim
      
@li @b �����б�
@verbatim
��
@endverbatim
        
@li @b ����ֵ�б�
@verbatim
0     �ɹ�
-1    ʧ��
@endverbatim
          
@li @b �ӿ����ƣ���
@li @b ����˵�����ú��������ᱻCluster_poolͨ��sql_pool����
*/
/*****************************************************************************/
    int32 Insert(SqlDB * db, string & errorStr);

/*****************************************************************************/
/**
@brief ��������: Cluster�������ݿ��и�������
    
@li @b ����б�
@verbatim
SqlDB *db          ���ݿ��ָ��
@endverbatim
      
@li @b �����б�
@verbatim
��
@endverbatim
        
@li @b ����ֵ�б�
@verbatim
0     �ɹ�
-1    ʧ��
@endverbatim
          
@li @b �ӿ����ƣ���
@li @b ����˵�����ú��������ᱻCluster_poolͨ��sql_pool����
*/
/*****************************************************************************/
    int32 Update(SqlDB *db);

/*****************************************************************************/
/**
@brief ��������: Cluster�������ݿ���ɾ������
    
@li @b ����б�
@verbatim
SqlDB *db          ���ݿ��ָ��
@endverbatim
      
@li @b �����б�
@verbatim
��
@endverbatim
        
@li @b ����ֵ�б�
@verbatim
0     �ɹ�
-1    ʧ��
@endverbatim
          
@li @b �ӿ����ƣ���
@li @b ����˵�����ú��������ᱻCluster_poolͨ��sql_pool����
*/
/*****************************************************************************/
    int32 Drop(SqlDB *db);

/*****************************************************************************/
/**
@brief ��������: Cluster����������ַ�������
    
@li @b ����б�
@verbatim
SqlColumn *columns    ��ѯ������
@endverbatim
      
@li @b �����б�
@verbatim
ostringstream& oss   ������ַ������ṹ
@endverbatim
        
@li @b ����ֵ�б�
@verbatim
0     �ɹ�
-1    ʧ��
@endverbatim
          
@li @b �ӿ����ƣ���
@li @b ����˵�����ú��������ᱻCluster_poolͨ��sql_pool����
*/
/*****************************************************************************/
    static int32 Dump(ostringstream& oss, SqlColumn *columns);


private:

//    DISALLOW_COPY_AND_ASSIGN(Cluster);


    static const char * _table;              /* ���ݿ��д��Cluster����ı��� */
    static const char * _db_names;           /* ����Cluster��Դ��ʱ�õ����ֶ��� */
    static const char * _extended_db_names;  /* ����Cluster��Դ��ʱ�õ�����չ�ֶ��� */


/*****************************************************************************/
/**
@brief ��������: Cluster���ѯ�ص�����
    
@li @b ����б�
@verbatim
void *nil               ��ָ��
SqlColumn * columns     ��ѯ���Ľ����ָ��
@endverbatim
      
@li @b �����б�
@verbatim
��
@endverbatim
        
@li @b ����ֵ�б�
@verbatim
0     �ɹ�
-1    ʧ��
@endverbatim
          
@li @b �ӿ����ƣ���
@li @b ����˵�����ýӿ������ݲ�ѯ�����У������ݿ��е����ݷŵ����Ա��ȥ
*/
/*****************************************************************************/
    int32 SelectCallback(void *nil, SqlColumn * columns);


};

}  /* end namespace zte_tecs */


#endif /* end TC_CLUSTER_H */

