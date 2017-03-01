/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�config_coefficient.cpp
* �ļ���ʶ��
* ����ժҪ��config_coefficient��Ķ����ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�Ԭ����
* ������ڣ�2011��8��21��
*
* �޸ļ�¼1��
*     �޸����ڣ�2011/8/21
*     �� �� �ţ�V1.0
*     �� �� �ˣ�Ԭ����
*     �޸����ݣ�����
*******************************************************************************/
#include "config_coefficient.h"
#include "host_cpuinfo.h"
#include "sky.h"

namespace zte_tecs {

/* coefficient����ʹ�õı����� */
const char *config_coefficient::_table = "config_coefficient";

/* coefficient�����еı��ֶ� */
const char *config_coefficient::_db_names = 
    "tecs_bench, description";

SqlDB *config_coefficient::_db = NULL;

config_coefficient *config_coefficient::_instance = NULL;

/******************************************************************************/ 
config_coefficient::config_coefficient()
{
}

/******************************************************************************/ 
config_coefficient::config_coefficient(SqlDB *pDb)
{
    _db = pDb;
};

/******************************************************************************/ 
int32 config_coefficient::Set(const string &coefficient_type, 
                               int32        coefficient_value, 
                       const string &description)
{
    InsertReplace(coefficient_type, coefficient_value, description, false);
    return InsertReplace(coefficient_type, coefficient_value, description, true);
}

/******************************************************************************/
int32 config_coefficient::InsertReplace(const string &coefficient_type, 
                                            int32 coefficient_value, 
                                            const string &description, 
                                            bool replace)
{
    char *sql_description;
    char *sql_coefficient_type;
    
    if (NULL == (sql_description = _db->EscapeString(description.c_str())))
    {
        return -1;
    }
    if (NULL == (sql_coefficient_type = _db->EscapeString(coefficient_type.c_str())))
    {
        return -1;
    }
    
    ostringstream   oss;

    if (replace)
    {
        oss << "UPDATE " << _table << " SET "
            << "tecs_bench = " << coefficient_value << ", "
            << "description = '" << sql_description << "'";
    }
    else
    {
        oss << "INSERT INTO " << _table << " ("<< _db_names <<") VALUES (" 
            << "tecs_bench=" << "," << coefficient_value << ", '" 
            << sql_description << "')";
    }
    
    _db->FreeString(sql_description);
    _db->FreeString(sql_coefficient_type);
    
    return _db->Execute(oss);
}

} // namespace zte_tecs

