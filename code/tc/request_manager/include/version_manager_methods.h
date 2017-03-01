/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�version_manager_methods.h
* �ļ���ʶ��
* ����ժҪ���汾����XMl���ýӿ�����
* ��ǰ�汾��1.0
* ��    �ߣ�zhangyb
* ������ڣ�2012��10��13��
*
* �޸ļ�¼1��
*     �޸����ڣ�2012/10/13
*     �� �� �ţ�V1.0
*     �� �� �ˣ�zhangyb
*     �޸����ݣ�����
******************************************************************************/
#ifndef TC_VERSION_MANAGER_METHODS_H
#define TC_VERSION_MANAGER_METHODS_H

#include "sky.h"
#include "request_manager.h"
#include "tecs_rpc_method.h"
#include "user_pool.h"
#include "mid.h"
#include "event.h"
#include "tecs_errcode.h"
#include "msg_version_with_api_method.h"

namespace zte_tecs
{
class VersionGetRepoAddr : public TecsRpcMethod
{
public:
    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP(ERROR,TECS_ERROR)
        ERR_MAP(ERROR_TIME_OUT,TECS_ERR_TIMEOUT)
        ERR_MAP(ERROR_INVALID_ARGUMENT,TECS_ERR_PARA_INVALID)
        ERR_MAP_END
    }

    VersionGetRepoAddr (UserPool *upool=0)
    {
        _method_name = "VmmGetRepoAddrMethod";
        _user_id = -1;
        _upool = upool;
    };

    ~VersionGetRepoAddr (){};

/*****************************************************************************/
/**
@brief ��������:�����ѯXML�����ӿ�

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

class VersionSetRepoAddr : public TecsRpcMethod
{
public:
    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP(ERROR,TECS_ERROR)
        ERR_MAP(ERROR_TIME_OUT,TECS_ERR_TIMEOUT)
        ERR_MAP(ERROR_INVALID_ARGUMENT,TECS_ERR_PARA_INVALID)
        ERR_MAP_END
    }

    VersionSetRepoAddr (UserPool *upool=0)
    {
        _method_name = "VmmSetRepoAddrMethod";
        _user_id = -1;
        _upool = upool;
    };

    ~VersionSetRepoAddr (){};

/*****************************************************************************/
/**
@brief ��������:�����ѯXML�����ӿ�

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

class VersionQuery : public TecsRpcMethod
{
public:
    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP(ERROR,TECS_ERROR)
        ERR_MAP(ERROR_TIME_OUT,TECS_ERR_TIMEOUT)
        ERR_MAP(ERROR_INVALID_ARGUMENT,TECS_ERR_PARA_INVALID)
        ERR_MAP(ERROR_PROCESS_WILL_EXIT,TECS_ERR_VERSION_CANCEL_OPERATION)
        ERR_MAP(ERROR_NO_UPDATE_PACKAGE,TECS_ERR_VERSION_NO_UPGRADE_PACKAGE)
        ERR_MAP(ERROR_NO_INSTALLED_PACKAGE,TECS_ERR_VERSION_NO_INSTALLED_PACKAGE)
        ERR_MAP(ERROR_POPEN_FAIL,TECS_ERR_VERSION_START_OPERATION_FAIL)
        ERR_MAP(ERROR_POPEN_SELECT_FAIL,TECS_ERR_VERSION_OPERATION_ABNORMAL_EXIT)
        ERR_MAP_END
    }

    VersionQuery (UserPool *upool=0)
    {
        _method_name = "VersionQueryMethod";
        _user_id = -1;
        _upool = upool;
    };

    ~VersionQuery (){};

/*****************************************************************************/
/**
@brief ��������:�汾��ѯXML�����ӿ�

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

class VersionUpdate : public TecsRpcMethod
{
public:
    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP(ERROR_TIME_OUT,TECS_ERR_TIMEOUT)
        ERR_MAP(ERROR_INVALID_ARGUMENT,TECS_ERR_PARA_INVALID)
        ERR_MAP(ERROR_DUPLICATED_OP,TECS_ERR_DUPLICATED_OP)
        ERR_MAP_END
    }

    VersionUpdate (UserPool *upool=0)
    {
        _method_name = "VmmUpdateMethod";
        _user_id = -1;
        _upool = upool;
    };

    ~VersionUpdate (){};

/*****************************************************************************/
/**
@brief ��������:�����ѯXML�����ӿ�

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

class VersionCancel : public TecsRpcMethod
{
public:
    RPC_ERROR_MAP
    {
        ERR_MAP_BEGIN
        ERR_MAP_PUB
        ERR_MAP(ERROR,TECS_ERROR)
        ERR_MAP(ERROR_TIME_OUT,TECS_ERR_TIMEOUT)
        ERR_MAP(ERROR_INVALID_ARGUMENT,TECS_ERR_PARA_INVALID)
        ERR_MAP(ERROR_OBJECT_NOT_EXIST,TECS_ERR_ACTION_FAILED)
        ERR_MAP_END
    }

    VersionCancel (UserPool *upool=0)
    {
        _method_name = "VmmUninstallMethod";
        _user_id = -1;
        _upool = upool;
    };

    ~VersionCancel (){};

/*****************************************************************************/
/**
@brief ��������:�����ѯXML�����ӿ�

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

#endif //end TC_VERSION_MANAGER_METHODS_H
