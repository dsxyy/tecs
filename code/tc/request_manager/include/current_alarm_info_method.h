/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：current_alarm_info_method.h
* 文件标识：见配置管理计划书
* 内容摘要：CurrentAlarm类定义文件
* 当前版本：1.0
* 作    者：马兆勉
* 完成日期：2011年7月27日
*
* 修改记录1：
*     修改日期：2011/7/27
*     版 本 号：V1.0
*     修 改 人：曹亮
*     修改内容：创建
*******************************************************************************/


#ifndef CURRENT_ALARM_INFO_METHOD_H
#define CURRENT_ALARM_INFO_METHOD_H

#include "tecs_rpc_method.h"
#include "user_pool.h"
#include "current_alarm_pool.h"
#include "history_alarm_pool.h"

#include <sstream>
#include <ctime>


// 禁用拷贝构造宏定义
#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
            TypeName(const TypeName&); \
            void operator=(const TypeName&)
#endif

using namespace std;

#define ALARM_DATABASE_NOT_EXIST 1
#define ALARM_DATABASE_FAIL_QUERY 2

namespace zte_tecs
{

class CurrentAlarmPoolInfoMethod : public TecsRpcMethod
{

public:
    CurrentAlarmPoolInfoMethod(UserPool *upool);
    ~CurrentAlarmPoolInfoMethod(){};
	virtual void MethodEntry(xmlrpc_c::paramList const& paramList,
                             xmlrpc_c::value *   const  retval);

   
private:
    DISALLOW_COPY_AND_ASSIGN(CurrentAlarmPoolInfoMethod); 
    CurrentAlarmPool* _cur_alarm_pool;
};



class HistoryAlarmPoolInfoMethod :public TecsRpcMethod
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:

    HistoryAlarmPoolInfoMethod (UserPool *upool);
	
    ~HistoryAlarmPoolInfoMethod (){};

   virtual void MethodEntry(const xmlrpc_c::paramList  &paramList,
                            xmlrpc_c::value  *const retval);

/*******************************************************************************
* 3. private section
*******************************************************************************/
private:
   DISALLOW_COPY_AND_ASSIGN(HistoryAlarmPoolInfoMethod); 
   HistoryAlarmPool* _his_alarm_pool;

};

}
#endif

