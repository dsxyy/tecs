/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：vmcfg_manager_with_file_manager.h
* 文件标识：
* 内容摘要：虚拟机配置管理与文件管理的相关头文件
* 当前版本：1.0
* 作    者：
* 完成日期：2013年4月10日
*
* 修改记录1：
*    修改日期：
*    版 本 号：
*    修 改 人：
*    修改内容：
*******************************************************************************/
#ifndef TC_VMCFG_MANAGER_WITH_FILE_MANAGER_H
#define TC_VMCFG_MANAGER_WITH_FILE_MANAGER_H
#include "sky.h"
#include "workflow.h"

namespace zte_tecs
{

class  CoreDumpUrlAck: public  WorkAck
{     
public:
    string ip;
    int    port;
    string path;
    string user;
    string passwd;
    string access_type;
    string access_option;
    
    CoreDumpUrlAck(){};
    CoreDumpUrlAck(const string &id_of_action):WorkAck(id_of_action){};
    ~CoreDumpUrlAck(){};
    SERIALIZE
    {
        SERIALIZE_BEGIN(CoreDumpUrlAck);
        WRITE_VALUE(ip);
        WRITE_VALUE(port);
        WRITE_VALUE(path);
        WRITE_VALUE(user);
        WRITE_VALUE(passwd);
        WRITE_VALUE(access_type);
        WRITE_VALUE(access_option);
        SERIALIZE_PARENT_END(WorkAck);
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(CoreDumpUrlAck);
        READ_VALUE(ip);
        READ_VALUE(port);
        READ_VALUE(path);
        READ_VALUE(user);
        READ_VALUE(passwd);
        READ_VALUE(access_type);
        READ_VALUE(access_option);
        DESERIALIZE_PARENT_END(WorkAck);
    };
};



} //namespace zte_tecs


#endif // end TC_VMCFG_MANAGER_WITH_FILE_MANAGER_H

