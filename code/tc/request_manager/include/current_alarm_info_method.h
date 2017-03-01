/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�current_alarm_info_method.h
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��CurrentAlarm�ඨ���ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�������
* ������ڣ�2011��7��27��
*
* �޸ļ�¼1��
*     �޸����ڣ�2011/7/27
*     �� �� �ţ�V1.0
*     �� �� �ˣ�����
*     �޸����ݣ�����
*******************************************************************************/


#ifndef CURRENT_ALARM_INFO_METHOD_H
#define CURRENT_ALARM_INFO_METHOD_H

#include "tecs_rpc_method.h"
#include "user_pool.h"
#include "current_alarm_pool.h"
#include "history_alarm_pool.h"

#include <sstream>
#include <ctime>


// ���ÿ�������궨��
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

