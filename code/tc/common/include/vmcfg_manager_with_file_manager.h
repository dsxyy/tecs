/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�vmcfg_manager_with_file_manager.h
* �ļ���ʶ��
* ����ժҪ����������ù������ļ���������ͷ�ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�
* ������ڣ�2013��4��10��
*
* �޸ļ�¼1��
*    �޸����ڣ�
*    �� �� �ţ�
*    �� �� �ˣ�
*    �޸����ݣ�
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

