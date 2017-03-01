/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�ConfigCmmPool.cpp
* �ļ���ʶ��
* ����ժҪ��ConfigCmmPool��Ķ����ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�liuyi
* ������ڣ�2011��8��21��
*
* �޸ļ�¼1��
*     �޸����ڣ�2012/12/21
*     �� �� �ţ�V1.0
*     �� �� �ˣ�liuyi
*     �޸����ݣ�����
*******************************************************************************/
#include "config_cmm_pool.h"
#include "sky.h"
#include "db_config.h"
#include "pool_sql.h"
namespace zte_tecs {

/* cmm_pool����ʹ�õı����� */
const char *ConfigCmmPool::_table = "cmm_pool";



/* cmm_pool�����еı��ֶ� */
const char *ConfigCmmPool::_db_names = 
    "bureau, rack, shelf, type, ip_address, description";

SqlDB *ConfigCmmPool::_db = NULL;

ConfigCmmPool *ConfigCmmPool::_instance = NULL;

/******************************************************************************/
ConfigCmmPool::ConfigCmmPool(SqlDB *pDb)
{
    _db = pDb;
}
/******************************************************************************/ 
int32 ConfigCmmPool::SelectCallback(void *nil, SqlColumn * columns)
{
    if ( NULL == columns )
    {
        return -1;
    }
    
    CmmConfig cmm_info;
    columns->GetValue(BUREAU,       cmm_info._bureau);
    columns->GetValue(RACK,         cmm_info._rack);
    columns->GetValue(SHELF,        cmm_info._shelf);
    columns->GetValue(TYPE,         cmm_info._type);
    columns->GetValue(IP_ADDRESS,   cmm_info._ip_address);
    columns->GetValue(DESCRIPTION,  cmm_info._description);
    vector<CmmConfig>* cmm_config = static_cast<vector<CmmConfig> *>(nil);
    cmm_config->push_back(cmm_info);
    return 0;
}

int32 ConfigCmmPool::Search(vector<CmmConfig> &cmm_infos,const string  where)
{
    ostringstream   oss;
    int             rc;


   oss << "SELECT " << _db_names << " FROM " << _table<< where;

   //cout<<" ConfigCmmPool::Search :"<<oss.str()<<endl;

    SetCallback(static_cast<Callbackable::Callback>(&ConfigCmmPool::SelectCallback),&cmm_infos);
    rc = _db->Execute(oss, this);
    UnsetCallback();
   cout<<" ConfigCmmPool::rc :"<<rc<<endl;

    if(0 == rc || SQLDB_RESULT_EMPTY == rc)
    {
        return 0;
    }
    return -1;
}
int32 ConfigCmmPool::Count()
{
    int64   num = 0;
    SqlCallbackable sqlcb(_db);
    sqlcb.SelectColumn(_table,"COUNT(*)","",num);
    return num;
}

string ConfigCmmPool::GetCmmIP(const int32  bureau,const int32  rack,const int32  shelf)
{
    string   ip_address ;
    SqlCallbackable sqlcb(_db);
    ostringstream   where;
    where << "bureau = " << bureau << " AND rack = " << rack<<" AND shelf = "<<shelf;

    sqlcb.SelectColumn(_table,"ip_address",where.str(),ip_address);
    return ip_address;
}

string ConfigCmmPool::GetShelfType(const int32  bureau,const int32  rack,const int32  shelf)
{
    string   shelf_type ;
    SqlCallbackable sqlcb(_db);
    ostringstream   where;
    where << "bureau = " << bureau << " AND rack = " << rack<<" AND shelf = "<<shelf;

    sqlcb.SelectColumn(_table,"type",where.str(),shelf_type);
    return shelf_type;
}
/******************************************************************************/ 
int32 ConfigCmmPool::Set(const CmmConfig &cmm_config)
{
    SqlCommand sql(_db,  _table);
    sql.Add("bureau",  cmm_config._bureau);
    sql.Add("rack",     cmm_config._rack);
    sql.Add("shelf",     cmm_config._shelf);
    sql.Add("type",     cmm_config._type);
    sql.Add("ip_address",     cmm_config._ip_address);
    sql.Add("description", cmm_config._description);

    ostringstream where ;
    where << " WHERE bureau = " << cmm_config._bureau<<" AND rack = " << cmm_config._rack
          << " AND shelf = " <<cmm_config._shelf;        
    return sql.Update(where.str());
}

/******************************************************************************/
int32 ConfigCmmPool::Add(const CmmConfig &cmm_config)
{ 
    SqlCommand sql(_db,  _table);
    sql.Add("bureau",  cmm_config._bureau);
    sql.Add("rack",     cmm_config._rack);
    sql.Add("shelf",     cmm_config._shelf);
    sql.Add("type",     cmm_config._type);
    sql.Add("ip_address",     cmm_config._ip_address);
    sql.Add("description", cmm_config._description);

    return sql.Insert();     

}

/******************************************************************************/
int32 ConfigCmmPool::Delete(const int32  bureau,const int32  rack,const int32  shelf)
{
    SqlCommand sql(_db,  _table);
    sql.Add("bureau",  bureau);
    sql.Add("rack",    rack);
    sql.Add("shelf",   shelf);

    ostringstream where ;
    where << " WHERE bureau = " << bureau<<" AND rack = " << rack<< " AND shelf = " <<shelf;        
    return sql.Delete(where.str());
}
void cmmadd(int32 bureau,int32 rack,int32 shelf,char *ip,char *type)
{
    ConfigCmmPool *cmm_pool = ConfigCmmPool::GetInstance();

    if (NULL == cmm_pool)
    {
        cout << "ConfigCmmPool is not created." << endl;
        return;
    }

    CmmConfig cmm_config;
    cmm_config._bureau=bureau;
    cmm_config._rack=rack;
    cmm_config._shelf=shelf;
    string s1(type);
    string s2(ip);
    cmm_config._type=s1;
    
    cmm_config._ip_address=s2;

    cmm_pool->Add(cmm_config);
    return;

}
DEFINE_DEBUG_FUNC(cmmadd);

void cmmdel(int32 bureau,int32 rack,int32 shelf)
{
    ConfigCmmPool *cmm_pool = ConfigCmmPool::GetInstance();
 
    if (NULL == cmm_pool)
    {
        cout << "ConfigCmmPool is not created." << endl;
        return;
    }
    int32 ret;
    ret = cmm_pool->Delete(bureau,rack,shelf);

    cout<<" cmmdel retcode = "<<ret<<endl;
    return;

}
DEFINE_DEBUG_FUNC(cmmdel);

void cmmset(int32 bureau,int32 rack,int32 shelf,char *ip,char *type)
{
    ConfigCmmPool *cmm_pool = ConfigCmmPool::GetInstance();
 
    if (NULL == cmm_pool)
    {
        cout << "ConfigCmmPool is not created." << endl;
        return;
    }
    
    CmmConfig cmm_config;
    cmm_config._bureau=bureau;
    cmm_config._rack=rack;
    cmm_config._shelf=shelf;
    string s1(type);
    string s2(ip);
    cmm_config._type=s1;
    
    cmm_config._ip_address=s2;

    cmm_pool->Set(cmm_config);
    
    return;

}

DEFINE_DEBUG_FUNC(cmmset);

void cmmgetip(int32 bureau,int32 rack,int32 shelf)
{
    ConfigCmmPool *cmm_pool = ConfigCmmPool::GetInstance();
 
    if (NULL == cmm_pool)
    {
        cout << "ConfigCmmPool is not created." << endl;
        return;
    }
    string ret;
    ret = cmm_pool->GetCmmIP(bureau,rack,shelf);

    cout<<" cmmgetip retcode = "<<ret<<endl;
    return;

}
DEFINE_DEBUG_FUNC(cmmgetip);

} // namespace zte_tecs

