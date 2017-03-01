/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�TecsRpcMethod.h
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��TecsRpcMethod��Ķ����ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�������
* ������ڣ�2011��7��22��
*
* �޸ļ�¼1��
*     �޸����ڣ�2011/7/22
*     �� �� �ţ�V1.0
*     �� �� �ˣ�������
*     �޸����ݣ�����
*******************************************************************************/
#ifndef RM_TECSRPCMETHOD_H
#define RM_TECSRPCMETHOD_H
#include "user_pool.h"
#include "log_agent.h"
#include "operator_log.h"
#include "api_error.h"
#include <xmlrpc-c/base.hpp>
#include <xmlrpc-c/registry.hpp>
#include <xmlrpc-c/server_abyss.hpp>

// ���ÿ�������궨��
#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
            TypeName(const TypeName&); \
            void operator=(const TypeName&)
#endif
using namespace xmlrpc_c;

namespace zte_tecs
{
#define RPC_ERROR_MAP int RpcError(int inner_error)
#define ERR_MAP_BEGIN switch(inner_error) {
#define ERR_MAP_PUB case SUCCESS: return TECS_SUCCESS; break;
#define ERR_MAP(inner,rpc) case inner: return rpc; break;
#define ERR_MAP_END default: return TECS_ERR_UNKNOWN; break;}
#define METHOD_SET            0
#define METHOD_READONLY       1

#define xINT(x)   (xmlrpc_c::value_int(x))
#define xSTR(x)   (xmlrpc_c::value_string(x))
#define xI8(x)    (xmlrpc_c::value_i8(x))
#define xARRAY(x) (xmlrpc_c::value_array(x))

#define xRET1(a) \
{\
    *retval = a;\
}

#define xRET2(a,b) \
{\
    vector<value> array_data;\
    array_data.push_back(a);\
    array_data.push_back(b);\
    value_array *array_result = new xmlrpc_c::value_array(array_data);\
    *retval = *array_result;\
    delete array_result;\
}

#define xRET3(a,b,c) \
{\
    vector<value> array_data;\
    array_data.push_back(a);\
    array_data.push_back(b);\
    array_data.push_back(c);\
    value_array *array_result = new xmlrpc_c::value_array(array_data);\
    *retval = *array_result;\
    delete array_result;\
}

#define xRET4(a,b,c,d) \
{\
    vector<value> array_data;\
    array_data.push_back(a);\
    array_data.push_back(b);\
    array_data.push_back(c);\
    array_data.push_back(d);\
    value_array *array_result = new xmlrpc_c::value_array(array_data);\
    *retval = *array_result;\
    delete array_result;\
}

#define xRET5(a,b,c,d,e) \
{\
    vector<value> array_data;\
    array_data.push_back(a);\
    array_data.push_back(b);\
    array_data.push_back(c);\
    array_data.push_back(d);\
    array_data.push_back(e);\
    value_array *array_result = new xmlrpc_c::value_array(array_data);\
    *retval = *array_result;\
    delete array_result;\
}

class TecsRpcMethod : public xmlrpc_c::method2
{
public:
    TecsRpcMethod() { _method_type = METHOD_SET; };             //Ĭ�Ϲ��캯��
    virtual ~TecsRpcMethod(){};    //Ĭ����������
    virtual RPC_ERROR_MAP { return TECS_ERR_UNKNOWN;};     //������ת������
    
    /**************************************************************************/
    /**
    @brief ��������: xmlrpc_c::method����ִ�����

    @li @b ����б�
    @verbatim
        paramList  XMLRPC�����б�
        callInfoP  XMLRPC����������
    @endverbatim

    @li @b �����б�
    @verbatim
        retval     XMLRPC���ز���ָ��
    @endverbatim

    @li @b ����ֵ�б�
    @verbatim
        ��
    @endverbatim

    @li @b �ӿ����ƣ���
    @li @b ����˵������
    */
    /**************************************************************************/
    void execute(const xmlrpc_c::paramList& paramList,
                 const xmlrpc_c::callInfo * const  callInfoP,
                 xmlrpc_c::value* const retval);

    /**************************************************************************/
    /**
    @brief ��������: TecsRpcMethod����ִ���������

    @li @b ����б�
    @verbatim
        paramList  XMLRPC�����б�
    @endverbatim

    @li @b �����б�
    @verbatim
        retval     XMLRPC���ز���ָ��
    @endverbatim

    @li @b ����ֵ�б�
    @verbatim
        ��
    @endverbatim

    @li @b �ӿ����ƣ���
    @li @b ����˵������
    */
    /**************************************************************************/
    virtual void MethodEntry(const xmlrpc_c::paramList& paramList,
                             xmlrpc_c::value* const retval) = 0;

    /**************************************************************************/
    /**
    @brief ��������: ��ȡע�᷽������

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
        ע�᷽������
    @endverbatim

    @li @b �ӿ����ƣ���
    @li @b ����˵������
    */
    /**************************************************************************/
    string get_method_name() const { return _method_name; };
    /**************************************************************************/
    /**
    @brief ��������: ��ȡע�᷽������

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
        ע�᷽������
    @endverbatim

    @li @b �ӿ����ƣ���
    @li @b ����˵������
    */
    /**************************************************************************/
    int get_method_type() const { return _method_type; };
    /**************************************************************************/
    /**
    @brief ��������: ����ע�᷽������

    @li @b ����б�
    @verbatim
        method_name  ע�᷽������
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
    /**************************************************************************/
    void set_method_name(const string& method_name) 
    { 
        _method_name = method_name; 
        if (string::npos != _method_name.find(".query", 0))
        {
            _method_type = METHOD_READONLY;
        }
        else
            _method_type = METHOD_SET;
    };

    /**************************************************************************/
    /**
    @brief ��������: ����ע�᷽��������Ϣ

    @li @b ����б�
    @verbatim
        method_help  ע�᷽��������Ϣ
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
    /**************************************************************************/
    void set_method_help(const string& method_help) 
    {
        if (method_help.empty())
            return;
        _help = method_help; 
    };

    
    /**************************************************************************/
    /**
    @brief ��������: ��ȡ����������ID

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
        ����������ID
    @endverbatim

    @li @b �ӿ����ƣ���
    @li @b ����˵������
    */
    /**************************************************************************/
    int64 get_userid() const { return _user_id; };

    /**************************************************************************/
    /**
    @brief ��������: ��ȡ��������������

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
        ��������������
    @endverbatim

    @li @b �ӿ����ƣ���
    @li @b ����˵������
    */
    /**************************************************************************/
    string get_username() const { return _user_name; };
    
protected:

    UserPool * _upool;        //�û���Դ��ָ��
    string     _method_name;  //����ע������
    int64      _user_id;      //����������ID
    string     _user_name;    //��������������
    int        _method_type;   //��������
    /**************************************************************************/
    /**
    @brief ��������: �����Ȩ����ӿڴ����ʽ�����

    @li @b ����б�
    @verbatim
        method    ע�᷽������
    @endverbatim

    @li @b �����б�
    @verbatim
          ��
    @endverbatim

    @li @b ����ֵ�б�
    @verbatim
        ��Ȩʧ�ܴ����ʽ���ַ���
    @endverbatim

    @li @b �ӿ����ƣ���̬��Ա����
    @li @b ����˵������
    */
    /**************************************************************************/
    static string HandleError(const string& method);

    /**************************************************************************/
    /**
    @brief ��������: �����Ȩʧ�ܴ����ʽ�����

    @li @b ����б�
    @verbatim
        method    ע�᷽������
    @endverbatim

    @li @b �����б�
    @verbatim
          ��
    @endverbatim

    @li @b ����ֵ�б�
    @verbatim
        ��Ȩʧ�ܴ����ʽ���ַ���
    @endverbatim

    @li @b �ӿ����ƣ���̬��Ա����
    @li @b ����˵������
    */
    /**************************************************************************/
    static string AuthenticateError(const string& method);

    /**************************************************************************/
    /**
    @brief ��������: ������Ȩʧ�ܴ����ʽ�����

    @li @b ����б�
    @verbatim
        method     ע�᷽������
        object     ��������
        action     ��������
        user_id    �û���ʶ
        object_id  �����ʶ
    @endverbatim

    @li @b �����б�
    @verbatim
          ��
    @endverbatim

    @li @b ����ֵ�б�
    @verbatim
        ��Ȩʧ�ܴ����ʽ���ַ���
    @endverbatim

    @li @b �ӿ����ƣ���̬��Ա����
    @li @b ����˵������
    */
    /**************************************************************************/
    static string AuthorizeError(const string& method,
                                 const string& object,
                                 const string& action,
                                 int64 user_id,
                                 int64 object_id);

    /**************************************************************************/
    /**
    @brief ��������: �����ȡ����ʧ�ܴ����ʽ�����

    @li @b ����б�
    @verbatim
        method     ע�᷽������
        object     ��������
        object_id  �����ʶ
    @endverbatim

    @li @b �����б�
    @verbatim
          ��
    @endverbatim

    @li @b ����ֵ�б�
    @verbatim
        ��ȡ����ʧ�ܴ����ʽ���ַ���
    @endverbatim

    @li @b �ӿ����ƣ���̬��Ա����
    @li @b ����˵������
    */
    /**************************************************************************/
    static string GetObjectError(const string& method,
                                 const string& object,
                                 int64 object_id);

    /**************************************************************************/
    /**
    @brief ��������: ���춯��ʧ�ܴ����ʽ�����

    @li @b ����б�
    @verbatim
        method     ע�᷽������
        object     ��������
        action     ��������
        object_id  �����ʶ
        error_code ������
    @endverbatim

    @li @b �����б�
    @verbatim
          ��
    @endverbatim

    @li @b ����ֵ�б�
    @verbatim
        ����ʧ�ܴ����ʽ���ַ���
    @endverbatim

    @li @b �ӿ����ƣ���̬��Ա����
    @li @b ����˵������
    */
    /**************************************************************************/
    static string ActionError(const string& method,
                              const string& object,
                              const string& action,
                              int64 object_id,
                              int64 error_code);

    /**************************************************************************/
    /**
    @brief ��������: ����Ự�������ʽ�����

    @li @b ����б�
    @verbatim
        method     ע�᷽������
    @endverbatim

    @li @b �����б�
    @verbatim
          ��
    @endverbatim

    @li @b ����ֵ�б�
    @verbatim
        �Ự���ʧ�ܴ����ʽ���ַ���
    @endverbatim

    @li @b �ӿ����ƣ���̬��Ա����
    @li @b ����˵������
    */
    /**************************************************************************/
    static string SessionError(const string& method);
    
    /**************************************************************************/
    /**
    @brief ��������: ��ӡ�����б�XML�ַ���

    @li @b ����б�
    @verbatim
        paramList     �����б�
    @endverbatim

    @li @b �����б�
    @verbatim
          ��
    @endverbatim

    @li @b ����ֵ�б�
    @verbatim
        �����б�XML�ַ���
    @endverbatim

    @li @b �ӿ����ƣ���̬��Ա����
    @li @b ����˵������
    */
    /**************************************************************************/
    static string ParamListToXml(const xmlrpc_c::paramList& paramList);
    
    /**************************************************************************/
    /**
    @brief ��������: ��ӡXMLRPC������XML�ַ���

    @li @b ����б�
    @verbatim
        xmlrpc_value     XMLRPC����
    @endverbatim

    @li @b �����б�
    @verbatim
          ��
    @endverbatim

    @li @b ����ֵ�б�
    @verbatim
        ����XML��ʽ���ַ���
    @endverbatim

    @li @b �ӿ����ƣ���̬��Ա����
    @li @b ����˵������
    */
    /**************************************************************************/
    static string ParamValue(const xmlrpc_c::value xmlrpc_value);

private:
    
    
    /**************************************************************************/
    /**
    @brief ��������: ���÷���������ID

    @li @b ����б�
    @verbatim
        user_id   ����������ID
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
    /**************************************************************************/
    void set_userid(int64 user_id) { _user_id = user_id; };
    
   /**************************************************************************/
    /**
    @brief ��������: ���÷�������������

    @li @b ����б�
    @verbatim
        user_name   ��������������
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
    /**************************************************************************/
    void set_username(const string& user_name) { _user_name = user_name; };
    
    // ���ÿ�������
    DISALLOW_COPY_AND_ASSIGN(TecsRpcMethod);

};

}  //namespace zte_tecs
#endif // #ifndef RM_TECSRPCMETHOD_H

