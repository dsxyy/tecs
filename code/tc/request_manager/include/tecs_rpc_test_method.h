/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�tecs_rpc_test_method.h
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��TecsRpcTestMethod�ඨ���ļ�
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
#ifndef RM_TECSRPCTESTMOTHER_H
#define RM_TECSRPCTESTMOTHER_H
#include "tecs_rpc_method.h"
#include "user_pool.h"

// ���ÿ�������궨��
#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
            TypeName(const TypeName&); \
            void operator=(const TypeName&)
#endif

using namespace zte_tecs;

class TecsRpcTestMethod : public TecsRpcMethod
{
public:
    TecsRpcTestMethod(UserPool * upool, 
	                  string method_name="", 
	                  string help="", 
					  string signature = "i:ii")
    {
        _signature = signature;
        _help = help;
        _method_name = method_name;
        _user_id = -1;
        _upool = upool;
    };

    ~TecsRpcTestMethod(){};

    void MethodEntry(const xmlrpc_c::paramList& paramList,
                     xmlrpc_c::value* const retval);

protected:
private:
    DISALLOW_COPY_AND_ASSIGN(TecsRpcTestMethod);
};
#endif  //#ifndef RM_TECSRPCTESTMOTHER_H

