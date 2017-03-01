/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�cluster_manager_with_host_manager.h
* �ļ���ʶ��
* ����ժҪ����Ⱥ�������������������ͷ�ļ�
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
#ifndef COMM_CLUSTER_MANAGER_WITH_HOST_MANAGER_H
#define COMM_CLUSTER_MANAGER_WITH_HOST_MANAGER_H

#include "sky.h"
#include <string.h>

using namespace std;
namespace zte_tecs
{
/*****************************************************************************/
/* ��Ⱥ��Դ�����࣬��װ������ҪΪ�˷���QPID��Ϣ��ʱ�򷽱����л� */
/*****************************************************************************/
class ClusterResType : public Serializable
{
public:
    ClusterResType() 
    {
        _core_free_max=0;      // ����״̬����������������
        _tcu_unit_free_max=0;  // ����״̬�������������ļ���������
    
        _tcu_max_total=0;       // ����״̬�����������ܼ�������
        _tcu_free_total=0;      // ����״̬���������Ŀ��м�������
        _tcu_free_max=0;        // ����״̬����������ʣ�����ļ�������
        
        _disk_max_total=0;      // ����״̬���������ı��ش������������ܺͣ���λB
        _disk_free_total=0;     // ����״̬���������ı��ش��̿����������ܺͣ���λB
        _disk_free_max=0;       // ����״̬���������ı��ش��̿������������ֵ����λB
        
        _share_disk_max_total=0; // ��ǰ��Ⱥӵ�еĹ�������ռ��ܺͣ���λB
        _share_disk_free_total=0;// ��ǰ��Ⱥӵ�еĿ��й�������ռ��ܺͣ���λB
        
        _mem_max_total=0;       // ����״̬�����������ڴ����������ܺͣ���λB
        _mem_free_total=0;      // ����״̬�����������ڴ�����������ܺͣ���λB
        _mem_free_max=0;        // ����״̬�����������ڴ�������������ֵ����λB
        
        _cpu_usage_average=0;   // ����״̬����������30��������cpu������ƽ��ֵ
        _cpu_usage_max=0;       // ����״̬����������30��������cpu���������ֵ
        _cpu_usage_min=0;       // ����״̬����������30��������cpu��������Сֵ

        _db_server_port = 0;   //�����ݿ�����ֶ�����

        _host_count = 0;  //��host�����ֶ�����
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
        _core_free_max=0;      // ����״̬����������������
        _tcu_unit_free_max=0;  // ����״̬�������������ļ���������

        _tcu_max_total=0;       // ����״̬�����������ܼ�������
        _tcu_free_total=0;      // ����״̬���������Ŀ��м�������
        _tcu_free_max=0;        // ����״̬����������ʣ�����ļ�������
        
        _disk_max_total=0;      // ����״̬���������ı��ش������������ܺͣ���λB
        _disk_free_total=0;     // ����״̬���������ı��ش��̿����������ܺͣ���λB
        _disk_free_max=0;       // ����״̬���������ı��ش��̿������������ֵ����λB
        
        _share_disk_max_total=0; // ��ǰ��Ⱥӵ�еĹ�������ռ��ܺͣ���λB
        _share_disk_free_total=0;// ��ǰ��Ⱥӵ�еĿ��й�������ռ��ܺͣ���λB
        
        _mem_max_total=0;       // ����״̬�����������ڴ����������ܺͣ���λB
        _mem_free_total=0;      // ����״̬�����������ڴ�����������ܺͣ���λB
        _mem_free_max=0;        // ����״̬�����������ڴ�������������ֵ����λB
        
        _cpu_usage_average=0;   // ����״̬����������30��������cpu������ƽ��ֵ
        _cpu_usage_max=0;       // ����״̬����������30��������cpu���������ֵ
        _cpu_usage_min=0;       // ����״̬����������30��������cpu��������Сֵ

        _net_plane.clear();
        _node.clear();

        _db_server_port = 0;   //�����ݿ�����ֶ�����
        _db_name.clear();
        _db_server_url.clear();
        _db_user.clear();
        _db_passwd.clear();

        _host_count = 0;   //��host�����ֶ�����

        _img_usage.clear();
        _img_srctype.clear();
        _img_spcexp = 0;
        _share_img_usage.clear();
    }
    
    int32 _core_free_max;      // ����״̬����������������
    int32 _tcu_unit_free_max;  // ����״̬�������������ļ���������

    int32 _tcu_max_total;       // ����״̬�����������ܼ�������
    int32 _tcu_free_total;      // ����״̬���������Ŀ��м�������
    int32 _tcu_free_max;        // ����״̬����������ʣ�����ļ�������
    
    int64 _disk_max_total;      // ����״̬���������ı��ش������������ܺͣ���λB
    int64 _disk_free_total;     // ����״̬���������ı��ش��̿����������ܺͣ���λB
    int64 _disk_free_max;       // ����״̬���������ı��ش��̿������������ֵ����λB
    
    int64 _share_disk_max_total; // ��ǰ��Ⱥӵ�еĹ�������ռ��ܺͣ���λB
    int64 _share_disk_free_total;// ��ǰ��Ⱥӵ�еĿ��й�������ռ��ܺͣ���λB
    
    int64 _mem_max_total;       // ����״̬�����������ڴ����������ܺͣ���λB
    int64 _mem_free_total;      // ����״̬�����������ڴ�����������ܺͣ���λB
    int64 _mem_free_max;        // ����״̬�����������ڴ�������������ֵ����λB
    
    int32 _cpu_usage_average;   // ����״̬����������30��������cpu������ƽ��ֵ
    int32 _cpu_usage_max;       // ����״̬����������30��������cpu���������ֵ
    int32 _cpu_usage_min;       // ����״̬����������30��������cpu��������Сֵ
    vector<string>  _net_plane; // ��Ⱥ����ƽ����Ϣ
    map<string,int>  _node;     // cluster node��Ϣ

    string _db_name;         //���ݿ���
    string _db_server_url;   // ������URL
    int32 _db_server_port;       // �������˿�
    string _db_user;       // �û���
    string _db_passwd;   // ����

    int32 _host_count;  //host����
    
    string _img_usage;  //����ʹ�÷�ʽ,���ջ��߷ǿ���
    string _img_srctype; //�����ʹ�þ����ļ����� �ļ����߿�
    int _img_spcexp; //�����ŵĿ��豸��С��չ����
    string _share_img_usage;  //������ʹ�÷�ʽ,���ջ��߷ǿ���
};

/*****************************************************************************/
/* ��Ⱥ��Ϣ�࣬����������Ϣ�࣬��Cluster Manger��Agentת����ȥ�� */
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
        _resource.clear();  //���restype�е��ֶ���Ϣ
       
    }
    
    string _cluster_name;       // ��Ⱥ����
    string _ip;                 // ��Ⱥ����IP
    string _cluster_append;     // ��Ⱥ������Ϣ
    bool  _is_online;           // ����״̬
    bool  _enabled;             // ʹ��״̬
    ClusterResType _resource;   // ������Դ��Ϣ
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
    

    string _name;             // Ҫע��ļ�Ⱥ����
    string _cluster_append;   /* ��Ⱥ�ĸ�����Ϣ */
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
    

    int    _err_code;          // ������
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
    

    string _name;              // Ҫɾ��ע��ļ�Ⱥ����
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


    int    _err_code;       // ������
    string _result;         // ��Ⱥ��TECSϵͳע���Ĵ�����
    string _appendinfo;     // ��ŵ��Ƿ��͹�������Ϣ��Ԫ��
};

/*****************************************************************************/
/* Agent��Cluster manger �� manger �� CC����ע������ʱ�����Ϣ��ṹ */
/* ����Ϣ�ṹ����ϢIDΪ��EV_CLUSTER_REGISTER_REQ ��EV_CLUSTER_ACTION_REG_REQ */
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
    

    string _name;             // Ҫע��ļ�Ⱥ����
    string _cluster_append;   /* ��Ⱥ�ĸ�����Ϣ */
    string _appendinfo;
    map<string, string> _tc_ifs_info; // TC ���о��� IP �� interface ��Ϣ, Ŀǰ�� name �� ip
};

/*****************************************************************************/
/* Agent��Cluster manger �� manger �� CC����ע������ʱ�����Ϣ��ṹ */
/*����Ϣ�ṹ����ϢIDΪ:EV_CLUSTER_UNREGISTER_REQ��
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
    

    string _name;              // Ҫɾ��ע��ļ�Ⱥ����
    string _appendinfo;       
};

/*****************************************************************************/
/* Agent��Cluster manger ���ͼ�Ⱥ��Ϣ��ѯ����ʱ�����Ϣ��ṹ */
/*����Ϣ�ṹ����ϢIDΪ: EV_CLUSTER_INFO_REQ ��
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

    int32  _ReqMode;            // ����ģʽ��0��ʾ�첽����1��ʾͬ������
    int64  _StartIndex;         // ��ѯ����ʼλ��
    int64  _Query_count;        // ��ѯ����
    string _appendinfo;         // ��ŵ��Ƿ��͹�������Ϣ��Ԫ��
};

/*****************************************************************************/
/* Agent��Cluster manger ���ͼ�Ⱥ��Ϣ��ѯ����ʱ�����Ϣ��ṹ */
/*����Ϣ�ṹ����ϢIDΪ: EV_CLUSTER_INFO_REQ ��
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

    string _cluster_name;         // ��ѯ�ļ�Ⱥ���ƣ����ַ�����ʾ��ѯ���м�Ⱥ������ƽ��
};


/*****************************************************************************/
/* Cluster manger ��Agent���ͼ�Ⱥ��Ϣ��ѯӦ�����Ϣ��ṹ */
/*����Ϣ�ṹ����ϢIDΪ: EV_CLUSTER_INFO_ACK ��
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
/* Cluster manger ��Agent���ͼ�Ⱥ��Ϣ��ѯӦ�����Ϣ��ṹ */
/*����Ϣ�ṹ����ϢIDΪ:  EV_CLUSTER_ACTION_INFO_ACK */
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
/*  CC�� Manger���ͼ�Ⱥ��ǰ��Ϣ����Ϣ��ṹ */
/*����Ϣ�ṹ����ϢIDΪ: EV_CLUSTER_INFO_REPORT/EV_CLUSTER_DISCOVER */
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
    

    int32 _core_free_max;      // ����״̬����������������
    int32 _tcu_unit_free_max;  // ����״̬�������������ļ���������

    int32 _tcu_max_total;       // ����״̬�����������ܼ�������
    int32 _tcu_free_total;      // ����״̬���������Ŀ��м�������
    int32 _tcu_free_max;        // ����״̬����������ʣ�����ļ�������
    
    int64 _disk_max_total;      // ����״̬���������ı��ش������������ܺͣ���λB
    int64 _disk_free_total;     // ����״̬���������ı��ش��̿����������ܺͣ���λB
    int64 _disk_free_max;       // ����״̬���������ı��ش��̿������������ֵ����λB
  
    
    int64 _mem_max_total;       // ����״̬�����������ڴ����������ܺͣ���λB
    int64 _mem_free_total;      // ����״̬�����������ڴ�����������ܺͣ���λB
    int64 _mem_free_max;        // ����״̬�����������ڴ�������������ֵ����λB
   
    int32 _cpu_usage_average;   // ����״̬����������30��������cpu������ƽ��ֵ
    int32 _cpu_usage_max;       // ����״̬����������30��������cpu���������ֵ
    int32 _cpu_usage_min;       // ����״̬����������30��������cpu��������Сֵ

    //string _ip_address;         // ��Ⱥ��ǰ��IP��ַ������CC�ĵ����ᷢ���仯
    map<int, string>  _node;     // �����ڵ���Ϣ������������ʱ��ֻ��һ����Ϊ��

    string _db_name;         //���ݿ���
    string _db_server_url;   // ������URL
    int32 _db_server_port;       // �������˿�
    string _db_user;       // �û���
    string _db_passwd;   // ����

    int32 _host_count;    //host����
    
    string _img_usage;    //����ʹ�÷�ʽ�����ջ��߷ǿ���
    string _cluster_verify_id;    //��Ⱥ�����֤��
    string _img_srctype; //�����ʹ�þ����ļ����� �ļ����߿�
    int _img_spcexp; //�����ŵĿ��豸��С��չ����
    string _share_img_usage; //������ʹ�÷�ʽ�����ջ��߷ǿ���
};

/*****************************************************************************/
/* Agent��Cluster manger ���ͼ�Ⱥά������Ϣ��ṹ */
/* ����Ϣ�ṹ����ϢIDΪ��EV_CLUSTER_ACTION_START_REQ �� EV_CLUSTER_ACTION_STOP_REQ */
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
    

    string _name;             // ��Ⱥ����
};

/*****************************************************************************/
/* Cluster manger �� Agent���ͼ�Ⱥά������ϢӦ����ṹ */
/* ����Ϣ�ṹ����ϢIDΪ��EV_CLUSTER_ACTION_START_ACK �� EV_CLUSTER_ACTION_STOP_ACK */
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
    

    int    _err_code;           // ������
    string _result;             // ��Ⱥ����
};

} //namespace zte_tecs


#endif // COMM_CLUSTER_MANAGER_WITH_HOST_MANAGER_H


