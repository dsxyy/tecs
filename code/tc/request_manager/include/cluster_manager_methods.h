/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�cluster_config.h
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��cluster����XMl���ýӿ�����
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
#ifndef TC_CLUSTER_MANAGER_METHODS_H
#define TC_CLUSTER_MANAGER_METHODS_H

#include "sky.h"
#include "cluster_pool.h"
#include "request_manager.h"
#include "tecs_rpc_method.h"
#include "user_pool.h"
#include "mid.h"
#include "event.h"
#include "api_error.h"
#include "tecs_errcode.h"

namespace zte_tecs
{

class ClusterSet:public TecsRpcMethod {
public:
    
    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP(ERR_CM_CREATE_FAIL,TECS_ERR_CM_CREATE_FAIL)
        ERR_MAP(ERR_CM_CLUSTER_REGISTERD,TECS_ERR_CM_CLUSTER_REGISTERD)
        ERR_MAP(ERR_CM_CLUSTER_NOT_EXIST,TECS_ERR_CM_CLUSTER_NOT_EXIST)
        ERR_MAP(ERR_CM_CLUSTER_NOT_FREE,TECS_ERR_CM_CLUSTER_NOT_FREE)
        ERR_MAP_END
    }

    ClusterSet(UserPool *upool=0)
    {
        _method_name = "ClusterSetMethod";
        _user_id = -1;
        _upool = upool;
    };

    ~ClusterSet(){};

/*****************************************************************************/
/**
@brief ��������: ��Ⱥע��XML�����ӿ�

@li @b ����б�
@verbatim
paramList    �����XML�������������
@endverbatim
      
@li @b �����б�
@verbatim
retval      ����Ǹ���������ִ�еĽ��
@endverbatim
        
@li @b ����ֵ�б�
@verbatim
��
@endverbatim
          
@li @b �ӿ����ƣ���
@li @b ����˵������
*/
/*****************************************************************************/
    void MethodEntry( xmlrpc_c::paramList const& paramList,
                      xmlrpc_c::value *   const  retval);
        
private:

};


class ClusterForget:public TecsRpcMethod {
public:
    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP(ERR_CM_CREATE_FAIL,TECS_ERR_CM_CREATE_FAIL)
        ERR_MAP(ERR_CM_CLUSTER_REGISTERD,TECS_ERR_CM_CLUSTER_REGISTERD)
        ERR_MAP(ERR_CM_CLUSTER_NOT_EXIST,TECS_ERR_CM_CLUSTER_NOT_EXIST)
        ERR_MAP(ERR_CM_CLUSTER_NOT_FREE,TECS_ERR_CM_CLUSTER_NOT_FREE)
        ERR_MAP_END
    }
    
    ClusterForget(UserPool *upool=0)
    {
        _method_name = "ClusterForgetMethod";
        _user_id = -1;
        _upool = upool;
    };

    ~ClusterForget (){};

/*****************************************************************************/
/**
@brief ��������: ��Ⱥע��XML�����ӿ�

@li @b ����б�
@verbatim
paramList    �����XML�������������
@endverbatim
      
@li @b �����б�
@verbatim
retval      ����Ǹ���������ִ�еĽ��
@endverbatim
        
@li @b ����ֵ�б�
@verbatim
��
@endverbatim
          
@li @b �ӿ����ƣ���
@li @b ����˵������
*/
/*****************************************************************************/
    void MethodEntry( xmlrpc_c::paramList const& paramList,
                      xmlrpc_c::value *   const  retval);
        
private:
 
};

class ClusterQuery : public TecsRpcMethod 
{
public:
    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP(ERR_CM_CREATE_FAIL,TECS_ERR_CM_CREATE_FAIL)
        ERR_MAP(ERR_CM_CLUSTER_REGISTERD,TECS_ERR_CM_CLUSTER_REGISTERD)
        ERR_MAP(ERR_CM_CLUSTER_NOT_EXIST,TECS_ERR_CM_CLUSTER_NOT_EXIST)
        ERR_MAP(ERR_CM_CLUSTER_NOT_FREE,TECS_ERR_CM_CLUSTER_NOT_FREE)
        ERR_MAP_END
    }

    
    ClusterQuery (UserPool *upool=0)
    {
        _method_name = "ClusterQueryMethod";
        _user_id = -1;
        _upool = upool;
    };

    ~ClusterQuery (){};

/*****************************************************************************/
/**
@brief ��������: ��Ⱥע��XML�����ӿ�

@li @b ����б�
@verbatim
paramList    �����XML�������������
@endverbatim
      
@li @b �����б�
@verbatim
retval      ����Ǹ���������ִ�еĽ��
@endverbatim
        
@li @b ����ֵ�б�
@verbatim
��
@endverbatim
          
@li @b �ӿ����ƣ���
@li @b ����˵������
*/
/*****************************************************************************/
    void MethodEntry( xmlrpc_c::paramList const& paramList,
                      xmlrpc_c::value    *const  retval);
        
private:

};


class ClusterQueryCoreAndTcu : public TecsRpcMethod 
{
public:
    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP_END
    }

    
    ClusterQueryCoreAndTcu (UserPool *upool=0)
    {
        _method_name = "ClusterQueryCoreAndTcuMethod";
        _user_id = -1;
        _upool = upool;
    };

    ~ClusterQueryCoreAndTcu (){};

/*****************************************************************************/
/**
@brief ��������: ��Ⱥע��XML�����ӿ�

@li @b ����б�
@verbatim
paramList    �����XML�������������
@endverbatim
      
@li @b �����б�
@verbatim
retval      ����Ǹ���������ִ�еĽ��
@endverbatim
        
@li @b ����ֵ�б�
@verbatim
��
@endverbatim
          
@li @b �ӿ����ƣ���
@li @b ����˵������
*/
/*****************************************************************************/
    void MethodEntry( xmlrpc_c::paramList const& paramList,
                      xmlrpc_c::value    *const  retval);
        
private:

};

class ClusterQueryNetPlane : public TecsRpcMethod 
{
public:
    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP_END
    }

    
    ClusterQueryNetPlane (UserPool *upool=0)
    {
        _method_name = "ClusterQueryNetPlaneMethod";
        _user_id = -1;
        _upool = upool;
    };

    ~ClusterQueryNetPlane (){};

/*****************************************************************************/
/**
@brief ��������: ��Ⱥע��XML�����ӿ�

@li @b ����б�
@verbatim
paramList    �����XML�������������
@endverbatim
      
@li @b �����б�
@verbatim
retval      ����Ǹ���������ִ�еĽ��
@endverbatim
        
@li @b ����ֵ�б�
@verbatim
��
@endverbatim
          
@li @b �ӿ����ƣ���
@li @b ����˵������
*/
/*****************************************************************************/
    void MethodEntry( xmlrpc_c::paramList const& paramList,
                      xmlrpc_c::value    *const  retval);
        
private:

};



class ClusterPreserveStart : public TecsRpcMethod 
{
public:

    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP(ERR_CM_CREATE_FAIL,TECS_ERR_CM_CREATE_FAIL)
        ERR_MAP(ERR_CM_CLUSTER_REGISTERD,TECS_ERR_CM_CLUSTER_REGISTERD)
        ERR_MAP(ERR_CM_CLUSTER_NOT_EXIST,TECS_ERR_CM_CLUSTER_NOT_EXIST)
        ERR_MAP(ERR_CM_CLUSTER_NOT_FREE,TECS_ERR_CM_CLUSTER_NOT_FREE)
        ERR_MAP_END
    }
    
    ClusterPreserveStart (UserPool *upool=0)
    {
        _method_name = "ClusterPreserveStart";
        _user_id = -1;
        _upool = upool;
    };

    ~ClusterPreserveStart (){};

/*****************************************************************************/
/**
@brief ��������: ��Ⱥ��ʼά��XML�����ӿ�

@li @b ����б�
@verbatim
paramList    �����XML�������������
@endverbatim
      
@li @b �����б�
@verbatim
retval      ����Ǹ���������ִ�еĽ��
@endverbatim
        
@li @b ����ֵ�б�
@verbatim
��
@endverbatim
          
@li @b �ӿ����ƣ���
@li @b ����˵������
*/
/*****************************************************************************/
    void MethodEntry( xmlrpc_c::paramList const& paramList,
                      xmlrpc_c::value    *const  retval);
        
private:

};

class ClusterPreserveStop : public TecsRpcMethod 
{
public:

    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP(ERR_CM_CREATE_FAIL,TECS_ERR_CM_CREATE_FAIL)
        ERR_MAP(ERR_CM_CLUSTER_REGISTERD,TECS_ERR_CM_CLUSTER_REGISTERD)
        ERR_MAP(ERR_CM_CLUSTER_NOT_EXIST,TECS_ERR_CM_CLUSTER_NOT_EXIST)
        ERR_MAP(ERR_CM_CLUSTER_NOT_FREE,TECS_ERR_CM_CLUSTER_NOT_FREE)
        ERR_MAP_END
    }
    
    ClusterPreserveStop (UserPool *upool=0)
    {
        _method_name = "ClusterPreserveStop";
        _user_id = -1;
        _upool = upool;
    };

    ~ClusterPreserveStop (){};

/*****************************************************************************/
/**
@brief ��������: ��Ⱥ��ʼά��XML�����ӿ�

@li @b ����б�
@verbatim
paramList    �����XML�������������
@endverbatim
      
@li @b �����б�
@verbatim
retval      ����Ǹ���������ִ�еĽ��
@endverbatim
        
@li @b ����ֵ�б�
@verbatim
��
@endverbatim
          
@li @b �ӿ����ƣ���
@li @b ����˵������
*/
/*****************************************************************************/
    void MethodEntry( xmlrpc_c::paramList const& paramList,
                      xmlrpc_c::value    *const  retval);
        
private:

};


} //end namespace zte_tecs




#endif //end TC_CLUSTER_MANAGER_METHODS_H
