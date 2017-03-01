/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�vmcfg_api.h
* �ļ���ʶ��
* ����ժҪ�����������ģ�����ӿ�
* ��ǰ�汾��1.0
* ��    �ߣ�Bob
* ������ڣ�2011��10��19��
*
* �޸ļ�¼1��
*    �޸����ڣ�2011/7/26
*    �� �� �ţ�V1.0
*    �� �� �ˣ�Bob
*    �޸����ݣ�����
*******************************************************************************/

#ifndef TECS_VMCFG_API_H
#define TECS_VMCFG_API_H
#include "api_vmbase.h"

// ���������api�ӿ����ݿ�FULLQUERY��ѯʱ���ȡֵ
#define U_VMCFG_FULLQUERY_ALL_STATE  -1
#define U_VMCFG_FULLQUERY_CONFIG     0
#define U_VMCFG_FULLQUERY_DEPLOY     1


/**
@brief ��������: �û������������״̬ \n
@li @b ����˵��: �޸ı�enumʱ����ͬ���޸�vm_messages.h�е�VmInstanceState״̬!!!
*/
enum ApiVmUserState
{
    USER_STATE_RUNNING  = 2,  /* ���� */
    USER_STATE_PAUSED   = 3,  /* ��ͣ */
    USER_STATE_SHUTOFF  = 4,  /* �ػ� */
    USER_STATE_CRASHED  = 5,  /* ���� */
    USER_STATE_UNKNOWN  = 6,   /* δ֪ */ 
    USER_STATE_CONFIGURATION  = 7   /* �������� */ 
};

/**
@brief ��������: �������ѯ������ \n
@li @b ����˵��: 
*/
enum ApiVmCfgQueryType
{
    VMCFG_USER_CREATE     = -5, /** < ��ǰ�û���  */
    VMCFG_USER_APPOINTED  = -4, /** < ָ���û���, ��admin�û����� */
    VMCFG_USER_ALL        = -3, /** < �����û���, ��admin�û����� */
    VMCFG_APPOINTED       = 0   /** < ָ���������ѯ */
};

class ApiVmCfgAllocateData : public Rpcable
{
public:
    ApiVmCfgAllocateData():vm_num(1){};
    ~ApiVmCfgAllocateData(){};
     bool Validate(string &err_str)
    {
        ostringstream    oss;
        
        if (StringCheck::CheckNormalName(cfg_info.vm_name,0, STR_LEN_32) != true)
        {
            err_str = "Error, invalide vm_name: " + cfg_info.vm_name;
            return false;
        }

        if (StringCheck::CheckNormalName(cfg_info.project_name,1, STR_LEN_32) != true)
        {
            err_str = "Error, invalide project name: " + cfg_info.project_name;
            return false;
        }

        if (0 == vm_num)
        {
            vm_num =1;
        }

        return cfg_info.base_info.Validate(err_str);
    };

  TO_RPC
    {
        TO_RPC_BEGIN();
        TO_STRUCT(cfg_info);
        TO_VALUE(vm_num);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_STRUCT(cfg_info);
        FROM_VALUE(vm_num);
        FROM_RPC_END();
    };

    //��������е��ֶ�


    ApiVmCfgInfo cfg_info;

    /**
    @brief ��������: ���������������
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: 1
    @li @b ����˵��: ��
    */
    int       vm_num;

};

class ApiVmCfgAllocateByVt : public Rpcable
{
public:
    ApiVmCfgAllocateByVt():vm_num(1){};
    ~ApiVmCfgAllocateByVt(){};
     bool Validate(string &err_str)
    {
        ostringstream    oss;

        if (StringCheck::CheckNormalName(vt_name,1, STR_LEN_32) != true)
        {
            err_str = "Error, invalide vm_name: " + vt_name;
            return false;
        }  

        if (StringCheck::CheckNormalName(project_name,1, STR_LEN_32) != true)
        {
            err_str = "Error, invalide project name: " + project_name;
            return false;
        }

        if (0 == vm_num)
        {
            vm_num =1;
        }
  
        return true;
    };

  TO_RPC
    {
        TO_RPC_BEGIN();
        TO_VALUE(vt_name);
        TO_VALUE(project_name);
        TO_VALUE(vm_num);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_VALUE(vt_name);
        FROM_VALUE(project_name);
        FROM_VALUE(vm_num);
        FROM_RPC_END();
    };
    

    /**
    @brief ��������: �����ģ������
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: ��
    @li @b ����˵��:
    */
    string    vt_name;

    /**
    @brief ��������: ����������Ĺ�������
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: default
    @li @b ����˵��: �û���ָ���������Ĭ�Ϲ�����
    */
    string    project_name;

    /**
    @brief ��������: ���������������
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: 1
    @li @b ����˵��: ��
    */
    int       vm_num;

};

class ApiVmCfgModifyData : public Rpcable
{
public:
    ApiVmCfgModifyData(){};
    ~ApiVmCfgModifyData(){};

    bool Validate(string &err_str)
    {
        ostringstream    oss;
        
        if (StringCheck::CheckNormalName(cfg_info.vm_name,0, STR_LEN_32) != true)
        {
            err_str = "Error, invalide parameter with " +cfg_info.vm_name;
            return false;
        }  

        if (StringCheck::CheckNormalName(cfg_info.project_name,1, STR_LEN_32) != true)
        {
            err_str ="Error, invalide parameter with " +cfg_info.project_name;
            return false;
        }
        
        return cfg_info.base_info.Validate(err_str);
    };
    
    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_I8(vid);
        TO_STRUCT(cfg_info);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_I8(vid);
        FROM_STRUCT(cfg_info);
        FROM_RPC_END();
    };

    /**
    @brief ��������: �����ID
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: ��
    @li @b ����˵��:��
    */
    long long  vid;
    
    ApiVmCfgInfo cfg_info;


 };

enum VmCfgElement
{
    VM_CFG_UNKNOWN                      = 0,
    VM_CFG_NAME                         = 1,
    VM_CFG_DESCRIPTION                  = 2,
    VM_CFG_PROJECT                      = 3,
    VM_CFG_VIRT_TYPE                    = 4,
    VM_CFG_HYPERVISOR                   = 5,
    VM_CFG_APPOINTED_CLUSTER            = 6,
    VM_CFG_APPOINTED_HOST               = 7,
    VM_CFG_SERIAL                       = 8,
    VM_CFG_COREDUMP                     = 9,
    VM_CFG_ENABLE_LIVEMIGRATE           = 10,
    VM_CFG_CPU                          = 11,
    VM_CFG_MEMORY                       = 12,
    VM_CFG_VNC_PASSWORD                 = 13,
    VM_CFG_MACHINE                      = 14,
    VM_CFG_WATCH_DOG                    = 15,
    VM_CFG_MUTEX                        = 16,

    VM_CFG_MAX
};

enum VmCfgOperation
{
    INSERT_VMCFG                        = 1,
    DELETE_VMCFG                        = 2,
    UPDATE_VMCFG                        = 3,
};

class ApiVmCfgModifyIndividualItem : public Rpcable
{
public:
    ApiVmCfgModifyIndividualItem(){};
    ~ApiVmCfgModifyIndividualItem(){};

    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_I8(vid);
        TO_VALUE(operation);
        TO_VALUE(element);
        TO_VALUE(args);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_I8(vid);
        FROM_VALUE(operation);
        FROM_VALUE(element);
        FROM_VALUE(args);
        FROM_RPC_END();
    };

    long long           vid;
    int                 operation;
    int                 element;
    map<string, string> args;
};

class ApiVmCfgBatchModify : public Rpcable
{
public:
    ApiVmCfgBatchModify(){};
    ~ApiVmCfgBatchModify(){};

    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_STRUCT_ARRAY(modifications);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_STRUCT_ARRAY(modifications);
        FROM_RPC_END();
    };

    vector<ApiVmCfgModifyIndividualItem> modifications;
};

class ApiVmCfgQuery:public Rpcable
{
public:

    ApiVmCfgQuery(long long tmp_start=0, long long tmp_count =0,long long tmp_type=0)
    {
        start_index = tmp_start;
        count = tmp_count;
        type  = tmp_type;
    };

    bool Validate(string &err_str)
    {
        ostringstream    oss;

        if(((unsigned long long)count > 50 )
            ||((unsigned long long)count == 0) )
        {
            err_str = "query count too large,range [1,50]";
            return false;
        }

        if ((RPC_QUERY_ALL_SELF != type)
             &&(RPC_QUERY_SELF_APPOINTED != type)
             &&(RPC_QUERY_ALL != type))
        {
            oss << "Incorrect query type:" << type;
            err_str = oss.str();
            return false;
        }
        
        if ((RPC_QUERY_SELF_APPOINTED == type)
             &&(appointed_vid <= 0))
        {
            oss << "query type is :" << type<< " and appointed vid is invalid.";
            err_str = oss.str();
            return false;
        }

        return true;
    };

    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_I8(start_index);
        TO_I8(count);
        TO_VALUE(type);
        TO_I8(appointed_vid);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_I8(start_index);
        FROM_I8(count);
        FROM_VALUE(type);
        FROM_I8(appointed_vid);
        FROM_RPC_END();
    };

    long long   start_index;
    long long   count;
    string      type;        //��ѯ������
    long long   appointed_vid;  //ָ����ѯ��vid
};

class ApiVmCfgFullQuery:public Rpcable
{
public:

    bool Validate(string &err_str)
    {
        ostringstream    oss;

        if (((unsigned long long)count > 100 )
             ||((unsigned long long)count == 0) )
        {
            err_str = "query count too large,range [1,100]";
            return false;
        }

        /* 2. У������Ƿ�Ϸ� */
        if (vm_user_state != U_VMCFG_FULLQUERY_ALL_STATE &&
            vm_user_state != U_VMCFG_FULLQUERY_CONFIG &&
            vm_user_state != U_VMCFG_FULLQUERY_DEPLOY)
        {
            /* -1:��ѯ����״̬, 0: ��������״̬, 1: �Ѳ���״̬ */
            oss << "vm state invalid: " << vm_user_state;
            err_str = oss.str();
            return false;
        }

        if ((type < VMCFG_USER_CREATE)
             ||((type > VMCFG_USER_ALL)&&(type < 0)))
        {
            oss << "Incorrect query type:" << type<< " .";
            err_str = oss.str();
            return false;
        }        

        return true;
    };

    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_I8(start_index);
        TO_I8(count);
        TO_I8(type);
        TO_VALUE(user);
        TO_VALUE(project);
        TO_VALUE(vm_user_state);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_I8(start_index);
        FROM_I8(count);
        FROM_I8(type);
        FROM_VALUE(user);
        FROM_VALUE(project);
        FROM_VALUE(vm_user_state);
        FROM_RPC_END();
    };

    long long   start_index;
    long long   count;
    long long   type;   //��ѯ������
    string      user ;  //ָ����ѯ���û�
    string      project;//ָ����ѯ�Ĺ���
    int         vm_user_state; //ָ����ѯ�������״̬
};


class ApiGetVidByName:public Rpcable
{
public:

    bool Validate(string &err_str)
    {
        ostringstream    oss;
        //��rpc������ ������ָ��vid��ѯ
        if ((query_scope < VMCFG_USER_CREATE)
             ||((query_scope > VMCFG_USER_ALL)))
        {
            oss << "Incorrect query type:" << query_scope<< " .";
            err_str = oss.str();
            return false;
        }        

        if(vm_name.empty())
        {
            oss << "VM name to query is empty!";
            err_str = oss.str();
            return false;
        }
        
        return true;
    };

    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_I8(query_scope);
        TO_VALUE(vm_name);
        TO_VALUE(vm_user);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_I8(query_scope);
        FROM_VALUE(vm_name);
        FROM_VALUE(vm_user);
        FROM_RPC_END();
    };

    long long   query_scope;
    string      vm_name ;  //ָ����ѯ���û�
    string      vm_user;//ָ����ѯ�Ĺ���
};

class ApiVmCfgMap:public Rpcable
{
public:
    ApiVmCfgMap() {};
    ApiVmCfgMap(long long _src, long long _des)
    {
        src = _src;
        des = _des;
    };

    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_I8(src);
        TO_I8(des);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_I8(src);
        FROM_I8(des);
        FROM_RPC_END();
    };
    
    long long   src; 
    long long   des;
};

/**
@brief ��������: ��������������ϵ����Ϣ��\n
@li @b ����˵��: ��
*/
class ApiVmCfgRelationData: public Rpcable
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:

    enum OperationType
    {
        RELATION_ADD    = 0, /** < ���ӻ����ϵ */
        RELATION_DEL    = 1  /** < ɾ�������ϵ */
    };
		
    ApiVmCfgRelationData() {};
	
    ApiVmCfgRelationData(int                  _type,
                                  vector<ApiVmCfgMap>  _vec_vm)
	{ 
	    vec_vm   = _vec_vm;
	    type     = _type; 
    };
	
    ~ApiVmCfgRelationData() {};

    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_VALUE(type);
        TO_STRUCT_ARRAY(vec_vm);        
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_VALUE(type);
        FROM_STRUCT_ARRAY(vec_vm);
        FROM_RPC_END();
    };
    
    int                   type;
	vector<ApiVmCfgMap>   vec_vm;   
	
};

class ApiAffinityRegionMemberAction: public Rpcable
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:

    ApiAffinityRegionMemberAction()
    {
        affinity_region_id   = INVALID_OID;
    };

    ApiAffinityRegionMemberAction(long long _id, vector<long long>  _vids)
    {
        affinity_region_id   = _id;
        vids     = _vids;
    };

    ~ApiAffinityRegionMemberAction() {};

    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_I8(affinity_region_id);
        TO_I8_ARRAY(vids);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_I8(affinity_region_id);
        FROM_I8_ARRAY(vids);
        FROM_RPC_END();
    };

    long long          affinity_region_id;
    vector<long long>  vids;

};




class ApiVmStaticInfo : public Rpcable
{
public:
    ApiVmStaticInfo()
    {};

    ~ApiVmStaticInfo()
    {};


    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_I8(vid);
        TO_I8(project_id);
        TO_I8(uid);
        TO_VALUE(user_name);
        TO_I8(config_version);
        TO_VALUE(create_time);
        TO_STRUCT(cfg_info);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_I8(vid);
        FROM_I8(project_id);
        FROM_I8(uid);
        FROM_VALUE(user_name);
        FROM_VALUE(config_version);
        FROM_VALUE(create_time);
        FROM_STRUCT(cfg_info);
        FROM_RPC_END();
    };

    int64     vid;
    int64     project_id;
    int64     uid;
    string    user_name;
    int       config_version;
    string    create_time;

    ApiVmCfgInfo cfg_info;
};

class ApiVmComputationInfo : public Rpcable
{
public:
    ApiVmComputationInfo()
    {
        deployed_cluster = "";
        deployed_hc = "";
        vnc_port = -1;
        serial_port = -1;
        cpu_rate = -1;
        memory_rate = -1;
    };

    ~ApiVmComputationInfo()
    {};

    TO_RPC
    {
        TO_RPC_BEGIN();
      //  TO_I8(vid);
        TO_VALUE(state);
      //  TO_I8(disk_size);
        TO_VALUE(deployed_cluster);
        TO_VALUE(deployed_hc);
        TO_VALUE(vnc_ip);
        TO_VALUE(vnc_port);
        TO_VALUE(serial_port);
        TO_VALUE(cpu_rate);
        TO_VALUE(memory_rate);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
      //  FROM_I8(vid);
        FROM_VALUE(state);
      //  FROM_I8(disk_size);
        FROM_VALUE(deployed_cluster);
        FROM_VALUE(deployed_hc);
        FROM_VALUE(vnc_ip);
        FROM_VALUE(vnc_port);
        FROM_VALUE(serial_port);
        FROM_VALUE(cpu_rate);
        FROM_VALUE(memory_rate);
        FROM_RPC_END();
    };

   //��������е��ֶ�
 //   int64     vid;
    int       state;
  //  int64     disk_size;
    string    deployed_cluster;
    string    deployed_hc;
    string    vnc_ip;
    int       vnc_port;
    int       serial_port;
    int       cpu_rate;
    int       memory_rate;
};
class ApiVmNetWorkInfo : public Rpcable
{
public:
    ApiVmNetWorkInfo()
    {
        use_rate  = 0;
    };

    ~ApiVmNetWorkInfo()
    {};

    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_VALUE(use_rate);
        TO_STRUCT_ARRAY(nic_info);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_VALUE(use_rate);
        FROM_STRUCT_ARRAY(nic_info);
        FROM_RPC_END();
    };
    vector<ApiVmNic>      nic_info;
    int                   use_rate;
};

class ApiVmStorageInfo : public Rpcable
{
public:
    ApiVmStorageInfo()
    {
        disk_size = 0;
    };

    ~ApiVmStorageInfo()
    {};

    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_I8(disk_size);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_I8(disk_size);
        FROM_RPC_END();
    };

    int64     disk_size;
};


class ApiVmRunningInfo : public Rpcable
{
public:
    ApiVmRunningInfo(){};

    ~ApiVmRunningInfo()
    {};

  TO_RPC
    {
        TO_RPC_BEGIN();
        TO_I8(vid);
        TO_STRUCT(computationinfo);
        TO_STRUCT(storageinfo);
        TO_STRUCT(netinfo);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_I8(vid);
        FROM_STRUCT(computationinfo);
        FROM_STRUCT(storageinfo);
        FROM_STRUCT(netinfo);
        FROM_RPC_END();
    };
    
   //��������е��ֶ�
    int64     vid;
    ApiVmComputationInfo  computationinfo;
    ApiVmStorageInfo      storageinfo;
    ApiVmNetWorkInfo      netinfo;
};


#if 0
class ApiNicSynInfo : public Rpcable
{
public:
    ApiNicSynInfo(){};
    ~ApiNicSynInfo(){};

  TO_RPC
    {
        TO_RPC_BEGIN();
        TO_VALUE(ip);
        TO_VALUE(netmask);
        TO_VALUE(gateway);
        TO_VALUE(mac);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_VALUE(ip);
        FROM_VALUE(netmask);
        FROM_VALUE(gateway);
        FROM_VALUE(mac);
        FROM_RPC_END();
    };
    

    string   ip;
    string   netmask;
    string   gateway;
    string   mac;
};


class ApiVmSynStateInfo : public Rpcable
{
public:
    ApiVmSynStateInfo()
    {
        result       = true;
        vm_id        = -1;
        vm_state     = USER_STATE_UNKNOWN;
        vnc_port     = 0;
        serial_port     = 0;
        cpu_use_rate = 0;
        mem_use_rate = 0;
    };

    ApiVmSynStateInfo(int64 vid) 
    {
        result   = true;
        vm_state = USER_STATE_UNKNOWN;
        vm_id    = vid;
        vnc_port = 0;
        serial_port = 0;
        cpu_use_rate = 0;
        mem_use_rate = 0;
        
    };
    ~ApiVmSynStateInfo()
    {};

  TO_RPC
    {
        TO_RPC_BEGIN();
        TO_VALUE(result);
        TO_I8(vm_id);
        TO_VALUE(vm_state);
        TO_VALUE(vnc_ip);
        TO_VALUE(vnc_port);
        TO_VALUE(serial_port);
        TO_VALUE(cpu_use_rate);
        TO_VALUE(mem_use_rate);
        TO_VALUE(host_name);
        TO_STRUCT_ARRAY(nic_info);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        
        FROM_VALUE(result);
        FROM_I8(vm_id);
        FROM_VALUE(vm_state);
        FROM_VALUE(vnc_ip);
        FROM_VALUE(vnc_port);
        FROM_VALUE(serial_port);
        FROM_VALUE(cpu_use_rate);
        FROM_VALUE(mem_use_rate);
        FROM_VALUE(host_name);
        FROM_STRUCT_ARRAY(nic_info);
        FROM_RPC_END();
    };
    

    bool       result;
    long long  vm_id;
    int      vm_state;
    string   vnc_ip;
    int      vnc_port;
    int      serial_port;
    int      cpu_use_rate;
    int      mem_use_rate;
    string   host_name;
    vector<ApiNicSynInfo>  nic_info;    

};
#endif

/**
@brief ��������: �鿴����������ϵ����Ϣ��\n
@li @b ����˵��: ��
*/
class ApiVmCfgRelationInfo: public Rpcable
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:

    ApiVmCfgRelationInfo() {};
	
    ApiVmCfgRelationInfo(vector<ApiVmCfgMap>  _vec_vm)
	{ 
	    vec_vm   = _vec_vm;
    };
	
    ~ApiVmCfgRelationInfo() {};

    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_STRUCT_ARRAY(vec_vm);        
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_STRUCT_ARRAY(vec_vm);
        FROM_RPC_END();
    };

	vector<ApiVmCfgMap>   vec_vm;   
	
};

// ��ѯ������ܹ�������USB�豸APIӦ��
class ApiVmUsbInfo: public Rpcable
{
public:
    string name;
    string manufacturer;
    int     vendor_id;
    int     product_id;
    int64 use_vid;
    int    online_states;
    bool  is_conflict;    

    ApiVmUsbInfo()
    {
        vendor_id = 0;
        product_id = 0;
        use_vid = -1;
        is_conflict = false;
    }
    
    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_VALUE(name);
        TO_VALUE(manufacturer);
        TO_VALUE(vendor_id);
        TO_VALUE(product_id);
        TO_I8(use_vid);
        TO_VALUE(online_states);
        TO_VALUE(is_conflict);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_VALUE(name);
        FROM_VALUE(manufacturer);
        FROM_VALUE(vendor_id);
        FROM_VALUE(product_id);
        FROM_I8(use_vid);
        FROM_VALUE(online_states);
        FROM_VALUE(is_conflict);
        FROM_RPC_END();
    };
};

// ��ѯ������ܹ�������USB�豸APIӦ��
class ApiVmUsbOp: public Rpcable
{
public:
    string     cmd;
    int64  op_vid;
    int     vendor_id;
    int     product_id;
    string name;

    ApiVmUsbOp()
    {
        vendor_id = 0;
        product_id = 0;
        op_vid = -1;
    }

    bool validate(string &err_str)
    {
        ostringstream    oss;
        // �ַ������ȼ��
        if (StringCheck::CheckSize(name,1, STR_LEN_128) != true)
        {
            oss << "Error, invalid parameter with  " << name;
            err_str = oss.str();
            return false;
        }

        // У������Ƿ�����Ҫ��
        if (cmd != VM_ACTION_PLUG_IN_USB &&
            cmd != VM_ACTION_PLUG_OUT_USB &&
            cmd != VM_ACTION_OUT_IN_USB)
        {
            oss << "Error, invalid parameter with  " << cmd;
            err_str = oss.str();
            return false;
        }

        return true;
    }
    
    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_VALUE(cmd);
        TO_I8(op_vid);
        TO_VALUE(vendor_id);
        TO_VALUE(product_id);
        TO_VALUE(name);
        TO_RPC_END();
    }

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_VALUE(cmd);
        FROM_I8(op_vid);
        FROM_VALUE(vendor_id);
        FROM_VALUE(product_id);
        FROM_VALUE(name);
        FROM_RPC_END();
    }
};
#if 0
class ApiVmCfgFullInfo: public Rpcable
{
public:
    long long vid;
    string name;
    long long user_id;
    string user_name;
    long long project_id;
    string project_name;
    int vcpu; //����CPU������ 
    int tcu; //����CPU������ 
    long long memory; //RAM��С,��λ���ֽڡ� 
    long long disk; //disk�ܴ�С,��λ���ֽڡ� 
    string create_time; //���������ʱ��

    ApiVmCfgFullInfo()
    {
        vid = -1;
        user_id = -1;
        project_id = -1;
        vcpu = 0;
        tcu = 0;
        memory = 0;
        disk = 0;
    }
    
    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_I8(vid);
        TO_VALUE(name);
        TO_I8(user_id);
        TO_VALUE(user_name);
        TO_I8(project_id);
        TO_VALUE(project_name);
        TO_VALUE(vcpu);
        TO_VALUE(tcu);
        TO_I8(memory);
        TO_I8(disk);
        TO_VALUE(create_time);
        TO_RPC_END();
    }

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_I8(vid);
        FROM_VALUE(name);
        FROM_I8(user_id);
        FROM_VALUE(user_name);
        FROM_I8(project_id);
        FROM_VALUE(project_name);
        FROM_VALUE(vcpu);
        FROM_VALUE(tcu);
        FROM_I8(memory);
        FROM_I8(disk);
        FROM_VALUE(create_time);
        FROM_RPC_END();
    }
};
#endif
class ApiVmCfgMigrate : public Rpcable
{
public:
    ApiVmCfgMigrate()
    {
        vid = -1;
        hid = -1;
        bForced = false;
        bLive   = false;
    };
    ~ApiVmCfgMigrate(){};

    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_I8(vid);
        TO_I8(hid);
        TO_VALUE(bForced);
        TO_VALUE(bLive);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_I8(vid);
        FROM_I8(hid);
        FROM_VALUE(bForced);
        FROM_VALUE(bLive);
        FROM_RPC_END();
    };
    
    long long   vid;    /* ��Ǩ�Ƶ������id */
    long long   hid;    /* ��Ǩ�Ƶ���Ŀ�������id�����Ϊ-1����ʾ��ָ�������*/
    bool        bForced;    /* ��ָ���������ʱ���Ƿ�ǿ��Ǩ�� */
    bool        bLive;   /* live_migrate or cold_migrate */
};


#endif

