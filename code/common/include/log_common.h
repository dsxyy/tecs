/******************************************************************************
* Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：log_agent.h
* 文件标识：见配置管理计划书
* 内容摘要：日志文件通用接口定义
* 当前版本：1.0
* 作    者：张业兵
* 完成日期：2012年8月23日
*
* 修改记录1：
*     修改日期：2012年8月23日
*     版 本 号：V1.0
*     修 改 人：张业兵
*     修改内容：创建
******************************************************************************/

#ifndef LOG_COMMON_H
#define LOG_COMMON_H
#include "sky.h"
#include "event.h"
namespace zte_tecs
{

#define LOCAL_LOG_PATH  "/var/log/tecs/LocalExcLog"
#define HC_LOG_PATH     "/var/log/tecs/HcExcLog"
class SysLogMessage:public Serializable
{
public:
    SysLogMessage() {};
    SysLogMessage(const string& log_time, 
                int log_level,
                const string& pro_name,
                const string& host_name,
                const string& log_content)
                :_log_time(log_time),
                _log_level_count(log_level),
                _pro_name(pro_name),
                _host_name(host_name),
                _log_content(log_content){};
    ~SysLogMessage() {};
    
    SERIALIZE
    {
        SERIALIZE_BEGIN(SysLogMessage);  //序列化开始，括号中填写类名
                
        WRITE_STRING(_log_time);            //C++中的string容器必须用WRITE_STRING宏来序列化
        WRITE_DIGIT(_log_level_count);        
        WRITE_STRING(_pro_name);
        WRITE_STRING(_host_name);
        WRITE_STRING(_log_content);
        SERIALIZE_END();        //序列化结束
    }

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(SysLogMessage); //反向序列化开始，括号中填写类名
        
        READ_STRING(_log_time);             //C++中的string容器必须用WRITE_STRING宏来序列化
        READ_DIGIT(_log_level_count);
        READ_STRING(_pro_name);
        READ_STRING(_host_name);
        READ_STRING(_log_content);
        DESERIALIZE_END();      //反向序列化结束
    }

    string _log_time;
    int    _log_level_count;    
    string _pro_name;
    string _host_name;
    string _log_content;
};
}

#endif



