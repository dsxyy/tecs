/******************************************************************************
* Copyright (c) 2012������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�log_agent.h
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ����־�ļ�ͨ�ýӿڶ���
* ��ǰ�汾��1.0
* ��    �ߣ���ҵ��
* ������ڣ�2012��8��23��
*
* �޸ļ�¼1��
*     �޸����ڣ�2012��8��23��
*     �� �� �ţ�V1.0
*     �� �� �ˣ���ҵ��
*     �޸����ݣ�����
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
        SERIALIZE_BEGIN(SysLogMessage);  //���л���ʼ����������д����
                
        WRITE_STRING(_log_time);            //C++�е�string����������WRITE_STRING�������л�
        WRITE_DIGIT(_log_level_count);        
        WRITE_STRING(_pro_name);
        WRITE_STRING(_host_name);
        WRITE_STRING(_log_content);
        SERIALIZE_END();        //���л�����
    }

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(SysLogMessage); //�������л���ʼ����������д����
        
        READ_STRING(_log_time);             //C++�е�string����������WRITE_STRING�������л�
        READ_DIGIT(_log_level_count);
        READ_STRING(_pro_name);
        READ_STRING(_host_name);
        READ_STRING(_log_content);
        DESERIALIZE_END();      //�������л�����
    }

    string _log_time;
    int    _log_level_count;    
    string _pro_name;
    string _host_name;
    string _log_content;
};
}

#endif



