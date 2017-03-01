/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：current_alarm_pool.cpp
* 文件标识：见配置管理计划书
* 内容摘要：CurrentAlarmPoll类实现文件
* 当前版本：1.0
* 作    者：马兆勉
* 完成日期：2011年7月27日
*
* 修改记录1：
*     修改日期：2011/7/27
*     版 本 号：V1.0
*     修 改 人：马兆勉
*     修改内容：创建
*******************************************************************************/


#include "current_alarm_info_method.h"

namespace zte_tecs
{

using namespace xmlrpc_c;
CurrentAlarmPoolInfoMethod::CurrentAlarmPoolInfoMethod(UserPool *upool=0)
{
    _method_name = "CurrentAlarmMethod";
    _user_id = -1;
    _upool = upool;
    _cur_alarm_pool = CurrentAlarmPool::GetInstance();
}

/******************************************************************************/

void CurrentAlarmPoolInfoMethod::MethodEntry(xmlrpc_c::paramList const& paramList,xmlrpc_c::value *   const  retval)
{
    int                 uid = -1;
    int64 start_index = 0;
    int64 query_count = 0;
    int64 next_index = 0;
    int64 max_count = 0;
    vector<xmlrpc_c::value> arrayAlarm;
    vector<xmlrpc_c::value> arrayAlarm1;
    ostringstream       oss;
    string              str;
    string              str1;
    ostringstream       os;
    int ret_code  = 0;
    
    start_index = xmlrpc_c::value_i8(paramList.getI8(1));
    query_count = xmlrpc_c::value_i8(paramList.getI8(2));
	
    uid = get_userid();
    oss << " 1 = 1 " << " order by oid limit " << query_count 
        << " offset " << start_index;
    str = oss.str();
    
    str1 = "1=1";
	
    if (_cur_alarm_pool == NULL)
    {
        ret_code = ALARM_DATABASE_NOT_EXIST;
        xRET2(xINT(ret_code), xSTR("Alarm database not exist. \n"));
        return;
    }
    else
    {
        if (_cur_alarm_pool->Dump(arrayAlarm1,str1) < 0 ) //首先查全表，获取max_count
        {
            ret_code = ALARM_DATABASE_FAIL_QUERY;
            xRET2(xINT(ret_code), xSTR("Fail to query alarm database. \n"));
            return;
        }
        else
        {
            max_count = arrayAlarm1.size();
            if (_cur_alarm_pool->Dump(arrayAlarm,str) < 0) //分包查询
            {
                ret_code = ALARM_DATABASE_FAIL_QUERY;
                xRET2(xINT(ret_code), xSTR("Fail to query alarm database. \n"));
                return;
            }
            else
            {
                if(arrayAlarm.size() < (unsigned int)(query_count) )
                {
                    next_index = -1;
                    xRET5(xINT(ret_code), xI8(next_index), xI8(max_count), xINT(arrayAlarm.size()), xARRAY(arrayAlarm));
                }
                else
                {
                    /* map<string, xmlrpc_c::value> p = value_struct(arrayAlarm[arrayAlarm.size()-1]); 
                    map<string, xmlrpc_c::value>::iterator it;
        	
                    if(p.end() != (it = p.find("oid")))
                    {
                        next_index = xmlrpc_c::value_int(it->second);
                    } */
                    
                    next_index =  start_index + query_count;
                    xRET5(xINT(ret_code), xI8(next_index), xI8(max_count), xINT(query_count), xARRAY(arrayAlarm));
                }
            }
            
        }
    }	
    /* 返回  */
}

/******************************************************************************/

HistoryAlarmPoolInfoMethod::HistoryAlarmPoolInfoMethod (UserPool *upool=0)
{
    _method_name = "HistoryAlarmMethod";
    _user_id = -1;
    _upool = upool;
    _his_alarm_pool = HistoryAlarmPool::GetInstance();
		
}

void HistoryAlarmPoolInfoMethod::MethodEntry(xmlrpc_c::paramList const& paramList,xmlrpc_c::value *   const  retval)
{
    int                 uid = -1;
    int64 start_index = 0;
    int64 query_count = 0;
    int64 next_index = 0;
    int64 max_count = 0;
    ostringstream       oss;
    string              str;
    string              str1;
    ostringstream       os;
    int ret_code  = 0;
    vector<ApiHistoryAlarmInfo> vec_api_alarm;   
    vector<value>         array_alarms;
    vector<ApiHistoryAlarmInfo>::const_iterator    iter_in;
    
    start_index = xmlrpc_c::value_i8(paramList.getI8(1));
    query_count = xmlrpc_c::value_i8(paramList.getI8(2));
	
    uid = get_userid();

    oss << " 1 = 1 " << " order by oid limit " << query_count 
        << " offset " << start_index;
    str = oss.str();
    
    str1 = "1=1";
    
    if (_his_alarm_pool == NULL)
    {
        ret_code = ALARM_DATABASE_NOT_EXIST;
        xRET2(xINT(ret_code), xSTR("Alarm database not exist. \n"));
        return;		
    }
    else
    {
        if (_his_alarm_pool->Show(vec_api_alarm, str1) < 0 )
        {
            ret_code = ALARM_DATABASE_FAIL_QUERY;
            xRET2(xINT(ret_code), xSTR("Fail to query alarm database. \n"));
            return; 	
        }
        else
        {
            max_count = vec_api_alarm.size();
            if (_his_alarm_pool->Show(vec_api_alarm, str) < 0) //分包查询
            {
                ret_code = ALARM_DATABASE_FAIL_QUERY;
                xRET2(xINT(ret_code), xSTR("Fail to query alarm database. \n"));
                return;
            }
            else
            {
                for (iter_in = vec_api_alarm.begin();
                        iter_in != vec_api_alarm.end();
                        ++iter_in)
                {
                    array_alarms.push_back(iter_in->to_rpc());
                }
                if(vec_api_alarm.size() < (unsigned int)(query_count) )
                {
                    next_index = -1;
                    xRET5(xINT(ret_code), xI8(next_index), xI8(max_count), xINT(vec_api_alarm.size()), xARRAY(array_alarms));
                }
                else
                {
                  /*  map<string, xmlrpc_c::value> p = value_struct(arrayAlarm[arrayAlarm.size()-1]); 
                    map<string, xmlrpc_c::value>::iterator it;
        	
                    if(p.end() != (it = p.find("oid")))
                    {
                        next_index = xmlrpc_c::value_int(it->second);
                    } */
                    
                    next_index =  start_index + query_count;
                    xRET5(xINT(ret_code), xI8(next_index), xI8(max_count), xINT(query_count), xARRAY(array_alarms));
                }
            }
        }
    }
	
    /* 返回  */
}

} // namespace zte_tecs


