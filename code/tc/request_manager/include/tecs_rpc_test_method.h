/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：tecs_rpc_test_method.h
* 文件标识：见配置管理计划书
* 内容摘要：TecsRpcTestMethod类定义文件
* 当前版本：1.0
* 作    者：马兆勉
* 完成日期：2011年7月22日
*
* 修改记录1：
*     修改日期：2011/7/22
*     版 本 号：V1.0
*     修 改 人：马兆勉
*     修改内容：创建
*******************************************************************************/
#ifndef RM_TECSRPCTESTMOTHER_H
#define RM_TECSRPCTESTMOTHER_H
#include "tecs_rpc_method.h"
#include "user_pool.h"

// 禁用拷贝构造宏定义
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

