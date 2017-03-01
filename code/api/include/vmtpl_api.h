/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�vmtpl_api.h
* �ļ���ʶ��
* ����ժҪ�������ģ��ģ�����ӿ�
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

#ifndef TECS_VMTPL_API_H
#define TECS_VMTPL_API_H
#include "api_vmbase.h"

class ApiVtAllocateData : public Rpcable
{
public:
    ApiVtAllocateData(){};
    
    ~ApiVtAllocateData(){};
    
	bool Validate(string &err_str)
    {
        if (StringCheck::CheckNormalName(vt_name,1, STR_LEN_32) != true)
        {
            err_str = "Error, invalide parameter with  " + vt_name;
            return false;
        }
        
        return cfg_info.base_info.Validate(err_str);
    };
    
  TO_RPC
    {
        TO_RPC_BEGIN();
        TO_STRUCT(cfg_info);
        TO_VALUE(vt_name);
        TO_VALUE(is_public);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_STRUCT(cfg_info);
        FROM_VALUE(vt_name);
        FROM_VALUE(is_public);
        FROM_RPC_END();
    };
    
    //ApiVmBase  base_info;

    ApiVtCfgInfo cfg_info;

    //��������е��ֶ�

    /**
    @brief ��������: �����ģ������
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: ��
    @li @b ����˵��: 
    */
    string    vt_name;

    /**
    @brief ��������: �����ģ���Ƿ���
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: ��
    @li @b ����˵��: 
    */
    bool      is_public;
};


class ApiVtModifyData : public Rpcable
{
public:
    ApiVtModifyData(){};
    ~ApiVtModifyData(){};
    
    bool Validate(string &err_str)
    {
        if (StringCheck::CheckNormalName(vt_name,1, STR_LEN_32) != true)
        {
            err_str = "Error, invalide parameter with  " + vt_name;
            return false;
        }
        return cfg_info.base_info.Validate(err_str);
    };
    
  TO_RPC
    {
        TO_RPC_BEGIN();
        TO_STRUCT(cfg_info);
        TO_VALUE(vt_name);
        TO_VALUE(is_public);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_STRUCT(cfg_info);
        FROM_VALUE(vt_name);
        FROM_VALUE(is_public);
        FROM_RPC_END();
    };

    
    //ApiVmBase  base_info;

    ApiVtCfgInfo cfg_info;

    //��������е��ֶ�

    /**
    @brief ��������: �����ģ������
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: ��
    @li @b ����˵��: 
    */
    string    vt_name;

    /**
    @brief ��������: �����ģ���Ƿ���
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: ��
    @li @b ����˵��: 
    */
    bool      is_public;
};

enum ApiVtQueryType
{
   VT_USER_CREATE   = 1, /** < User's  VmTemplate   */
   VT_USER_VISIBLE  = 2, /** < User's  and public VmTemplates  */
   VT_APPOINTED     = 3   /** < VmTemplate ID, user's VmTemplate */
};


class ApiVtQuery:public Rpcable
{
public:

    ApiVtQuery(long long tmp_start=0, long long tmp_count =0,long long tmp_type=0)
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

        ApiVtQueryType  tmp_type = static_cast <ApiVtQueryType>(type);
        if ((tmp_type < VT_USER_CREATE)
             &&(tmp_type > VT_APPOINTED))
        {
            oss << "Incorrect query type:" << type<< " .";
            err_str = oss.str();
            return false;
        }

        if(VT_APPOINTED ==  tmp_type)
        {
           if(StringCheck::CheckNormalName(vt_name,1, STR_LEN_32) != true)
           {
               oss << "Incorrect vmtemplate name :" << vt_name<< ".";
               err_str = oss.str();
               return false;
           }
        }
        return true;
    };

    TO_RPC
    {
        TO_RPC_BEGIN();
        TO_I8(start_index);
        TO_I8(count);
        TO_VALUE(type);
        TO_VALUE(vt_name);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_I8(start_index);
        FROM_I8(count);
        FROM_VALUE(type);
        FROM_VALUE(vt_name);
        FROM_RPC_END();
    };

    long long   start_index;
    long long   count;
    int         type;
    string      vt_name;
};

class ApiVtInfo : public Rpcable
{
public:
    ApiVtInfo(){};
    ~ApiVtInfo(){};

  TO_RPC
    {
        TO_RPC_BEGIN();
        TO_STRUCT(cfg_info);
        TO_I8(vt_id);
        TO_VALUE(vt_name);
        TO_I8(uid);
        TO_VALUE(user_name);
        TO_VALUE(create_time);
        TO_VALUE(is_public);
        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        FROM_STRUCT(cfg_info);
        FROM_I8(vt_id);
        FROM_VALUE(vt_name);
        FROM_I8(uid);
        FROM_VALUE(user_name);
        FROM_VALUE(create_time);
        FROM_VALUE(is_public);
        FROM_RPC_END();
    };
    
     //�����ģ����е��ֶ�
    int64     vt_id;
    string    vt_name;
    int64     uid;
    string    user_name;
    string    create_time;
    bool      is_public; 
    
    ApiVtCfgInfo cfg_info;
    //ApiVmBase  base_info;

};

#endif

