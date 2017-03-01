/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：config_host_atca.cpp
* 文件标识：
* 内容摘要：ConfigHostAtca类的定义文件
* 当前版本：1.0
* 作    者：liuyi
* 完成日期：2013年4月21日
*
* 修改记录1：
*     修改日期：2013/4/21
*     版 本 号：V1.0
*     修 改 人：liuyi
*     修改内容：创建
*******************************************************************************/
#include "config_host_atca.h"
#include "db_config.h"
#include "pool_sql.h"

namespace zte_tecs {

const char *HostAtca::_table = "host_atca";

const char *HostAtca::_host_atca_with_host_pool_view = "host_atca_with_host_pool";
const char *HostAtca::_host_position_cmmip_valid_view = "host_position_cmmip_valid";
const char *HostAtca::_host_position_delete_repeat_view = "host_position_delete_repeat";
/* host_atca配置中的表字段 */
const char *HostAtca::_db_names = 
    "hid, bureau, rack, shelf, slot, board_type, "
    "power_state, power_operate, rate_power, poweroff_time, total_poweroff_time,description";

SqlDB *HostAtca::_db = NULL;

HostAtca *HostAtca::_instance = NULL;

/******************************************************************************/
HostAtca::HostAtca(SqlDB *pDb)
{
    _db = pDb;
}
/******************************************************************************/ 
int32 HostAtca::SelectCallback(void *nil, SqlColumn * columns)
{
    if ( NULL == columns )
    {
        return -1;
    }
    
    host_atca_table_info info;
    columns->GetValue(HID,          info.hid);
    columns->GetValue(BUREAU,       info.host_position.bureau);
    columns->GetValue(RACK,         info.host_position.rack);
    columns->GetValue(SHELF,        info.host_position.shelf);
    columns->GetValue(SLOT,         info.host_position.slot);
    columns->GetValue(BOARD_TYPE,   info.board_type);
    vector<host_atca_table_info>* config = static_cast<vector<host_atca_table_info> *>(nil);
    config->push_back(info);
    return 0;
}
/******************************************************************************/ 
int32 HostAtca::Search(vector<host_atca_table_info> &infos,const string  where)
{
    ostringstream   oss;
    int             rc;

    oss << "SELECT " << _db_names << " FROM " << _table<< where;

    SetCallback(static_cast<Callbackable::Callback>(&HostAtca::SelectCallback),&infos);
    rc = _db->Execute(oss, this);
    UnsetCallback();

    if(0 == rc || SQLDB_RESULT_EMPTY == rc)
    {
        return 0;
    }
    return -1;
}
/******************************************************************************/
int32 HostAtca::Delete(const int64  &hid)
{
    SqlCommand sql(_db,  _table);

    ostringstream where ;
    where << " WHERE hid = " << hid;        
    return sql.Delete(where.str());
}
/******************************************************************************/ 
int32 HostAtca::Count()
{
    int64   num = 0;
    SqlCallbackable sqlcb(_db);
    sqlcb.SelectColumn(_table,"COUNT(*)","",num);
    return num;
}
/******************************************************************************/
int32 HostAtca::Update(const host_atca_table_info &info)
{ 
    SqlCommand sql(_db,  _table);
    sql.Add("bureau",     info.host_position.bureau);
    sql.Add("rack",       info.host_position.rack);
    sql.Add("shelf",      info.host_position.shelf);
    sql.Add("slot",       info.host_position.slot);
    sql.Add("board_type", info.board_type);
    //sql.Add("power_state", info.power_state);
    //sql.Add("power_operate", info.power_operate);
    //sql.Add("rate_power", info.rate_power);
    //sql.Add("poweroff_time", info.poweroff_time);
    //sql.Add("total_poweroff_time", info.total_poweroff_time);
    //sql.Add("description", "");

    ostringstream where ;
    where << " WHERE hid = " << info.hid;        

    return sql.Update(where.str());     
}

/******************************************************************************/
int32 HostAtca::UpdatePowerState(int64 &hid,const int32 &power_state)
{
    SqlCommand sql(_db,  _table);
    sql.Add("power_state",    power_state);

    ostringstream where ;
    where << " WHERE hid = " << hid;        
    return sql.Update(where.str());
}

/******************************************************************************/
int32 HostAtca::UpdatePowerOperate(int64 &hid,const int32 &power_operate)
{
    SqlCommand sql(_db,  _table);
    sql.Add("power_operate",    power_operate);

    ostringstream where ;
    where << "where  hid = " << hid;        
    return sql.Update(where.str());
}
/******************************************************************************/
int32 HostAtca::UpdatePowerOffTime(int64 &hid,int64 &time)
{
    SqlCommand sql(_db,  _table);
    sql.Add("poweroff_time",    time);

    ostringstream where ;
    where << "where  hid = " << hid;        
    return sql.Update(where.str());
}
/******************************************************************************/
int32 HostAtca::UpdateTotalPowerOffTime(int64 &hid,int64 &time)
{
    SqlCommand sql(_db,  _table);
    sql.Add("total_poweroff_time",    time);

    ostringstream where ;
    where << "where  hid = " << hid;        
    return sql.Update(where.str());
}

/******************************************************************************/
int32 HostAtca::UpdateRatePower(int64 &hid, int32& rate_power)
{
    SqlCommand sql(_db,  _table);
    sql.Add("rate_power", rate_power);

    ostringstream where ;
    where << "where hid = " << hid;
    return sql.Update(where.str());
}

/******************************************************************************/
void HostAtca::GetAllQueryHosts(vector<int64> &hids)
{
    SqlCallbackable sqlcb(_db);
    sqlcb.SelectColumn(_host_position_cmmip_valid_view,"hid","",hids);

    return ;
}
/******************************************************************************/ 
int32 HostAtca::SelectByHostIdCallback(void *nil, SqlColumn * columns)
{
    if ((NULL == columns) || (LIMIT != columns->get_column_num()))
    {
        return -1;
    }

    host_atca_table_info *info = static_cast<host_atca_table_info *>(nil);
    
    columns->GetValue(HID,                   info->hid);
    columns->GetValue(BUREAU,                info->host_position.bureau);
    columns->GetValue(RACK,                  info->host_position.rack);
    columns->GetValue(SHELF,                 info->host_position.shelf);
    columns->GetValue(SLOT,                  info->host_position.slot);
    columns->GetValue(BOARD_TYPE,            info->board_type);
    columns->GetValue(POWER_STATE,           info->power_state);
    columns->GetValue(POWER_OPERATE,         info->power_operate);
    columns->GetValue(RATE_POWER,            info->rate_power);
    columns->GetValue(POWEROFF_TIME,         info->poweroff_time);
    columns->GetValue(TOTAL_POWEROFF_TIME,   info->total_poweroff_time);
    columns->GetValue(DESCRIPTION,           info->decription);
    
    return 0;
}
/******************************************************************************/
int32 HostAtca::SearchByHostId(host_atca_table_info &info,const int64 &hid)
{
    ostringstream   oss;
    int             rc;

    oss << "SELECT " << _db_names << " FROM " << _table<< " where hid = "<<hid;

    SetCallback(static_cast<Callbackable::Callback>(&HostAtca::SelectByHostIdCallback),&info);
    rc = _db->Execute(oss, this);
    UnsetCallback();

    if(0 == rc || SQLDB_RESULT_EMPTY == rc)
    {
        return SUCCESS;
    }
    return ERROR;
}
/******************************************************************************/
int32 HostAtca::GetPowerStateNotMatchHost(vector<int64> &hids)
{
    int  rc = -1;
    
    ostringstream   where;
    where  <<" ((power_operate = "<<HostAtca::OPERATE_POWER_ON
           <<" and power_state = "<<HostAtca::BOARD_POWER_OFF
           <<") or (power_operate = "<<HostAtca::OPERATE_POWER_OFF
           <<" and power_state = "<<HostAtca::BOARD_POWER_ON<<")) ";
    SqlCallbackable sqlcb(_db);
    rc = sqlcb.SelectColumn(_host_atca_with_host_pool_view,
                            "hid",
                            where.str(),
                            hids);

    return rc;
}
/******************************************************************************/
int32 HostAtca::GetPowerOperateByHostId(int64 &hid,int32 &state)
{
    int  rc = -1;
    
    ostringstream   where;
    where  << "  hid = " << hid ;
    SqlCallbackable sqlcb(_db);
    state = -1;
    rc = sqlcb.SelectColumn(_table,
                            "power_operate",
                            where.str(),
                            state);

    if((SQLDB_OK != rc)
       &&(SQLDB_RESULT_EMPTY != rc))
    { 
        return ERROR;
    }


    return SUCCESS;
}
/******************************************************************************/    
int32 HostAtca::GetPoweStateByHostId(int64 &hid,int32 &state)
{
    int  rc = -1;
    
    ostringstream   where;
    where  << "  hid = " << hid ;
    SqlCallbackable sqlcb(_db);
    state = -1;
    rc = sqlcb.SelectColumn(_table,"power_state",where.str(),state);

    if((SQLDB_OK != rc)
       &&(SQLDB_RESULT_EMPTY != rc))
    { 
        return ERROR;
    }

    return SUCCESS;
}
/******************************************************************************/
int32 HostAtca::GetRunningVmsHosts()
{
    int32 num = 0;
    SqlCallbackable sqlcb(_db);
    sqlcb.SelectColumn(_host_atca_with_host_pool_view,
                       "COUNT(hid)",
                       "running_vms > 0 ",
                       num);
    return num;
}
/******************************************************************************/
int32 HostAtca::GetOffLineAndPoweroffOperateHosts(vector<int64> &hids)
{
    int  rc = -1;
    ostringstream   where;
    where << " power_operate = "<< HostAtca::OPERATE_POWER_OFF 
          << " and run_state = "<< Host::OFF_LINE 
          << " and running_vms = "<< 0 ;
          
    SqlCallbackable sqlcb(_db);
    sqlcb.SelectColumn(_host_atca_with_host_pool_view,
                       "hid",
                       where.str(),
                       hids);
    return rc;
}
/******************************************************************************/
int32 HostAtca::GetOnLineAndPoweronOperateHosts(vector<int64> &hids)
{
    int  rc = -1;
    ostringstream   where;
    where << " power_operate = "<< HostAtca::OPERATE_POWER_ON 
          << " and run_state = "<< Host::ON_LINE 
          << " and running_vms = "<< 0 ;
    SqlCallbackable sqlcb(_db);
    sqlcb.SelectColumn(_host_atca_with_host_pool_view,
                       "hid",
                       where.str(),
                       hids);
    return rc;
}
/******************************************************************************/
int32 HostAtca::GetAllPoweroffOperateHosts(vector<int64> &hids)
{
    int  rc = -1;
    ostringstream   where;
    where  << " power_operate = "<< HostAtca::OPERATE_POWER_OFF << " and running_vms = "<< 0;
    SqlCallbackable sqlcb(_db);
    sqlcb.SelectColumn(_table,
                       "hid",
                       where.str(),
                       hids);

    return rc;
}
/******************************************************************************/
int32 HostAtca::GetOnlineAndNoVmsHostNum()
{
    int32  count = 0;
    ostringstream   where;
    where  << " run_state = "<< Host::ON_LINE << " and running_vms = "<< 0 ;
    SqlCallbackable sqlcb(_db);
    sqlcb.SelectColumn(_host_atca_with_host_pool_view,
                       "count(*)",
                       where.str(),
                       count);

    return count;
}

/******************************************************************************/
int32 HostAtca::GetPowerOffTime(int64 &hid,int64 &time)
{
    int  rc = -1;
    
    ostringstream   where;
    where  << "  hid = " << hid ;
    SqlCallbackable sqlcb(_db);
    time = 0;
    rc = sqlcb.SelectColumn(_table,
                            "poweroff_time",
                            where.str(),
                            time);

    if((SQLDB_OK != rc)
       &&(SQLDB_RESULT_EMPTY != rc))
    { 
        return ERROR;
    }

    return SUCCESS;
}
/******************************************************************************/
bool HostAtca::CheckPositionIsRepeat(int64 &hid)
{
    int  rc = -1;
    int64 num = 0;
    ostringstream   where;
    where  << "  hid = " << hid ;
    SqlCallbackable sqlcb(_db);
    rc = sqlcb.SelectColumn(_host_position_delete_repeat_view,
                            "COUNT(hid)",
                            where.str(),
                            num);

    if((SQLDB_OK != rc)
       &&(SQLDB_RESULT_EMPTY != rc))
    { 
        return true;
    }

    return false;
}
/******************************************************************************/
int32 HostAtca::GetPowerState(int64 &hid)
{
    int32 state = 0;    
    ostringstream   where;
    where  << "  hid = " << hid ;
    SqlCallbackable sqlcb(_db);
    sqlcb.SelectColumn(_table,
                        "power_state",
                        where.str(),
                        state);
    return state;
}

/******************************************************************************/
int32 HostAtca::GetTotalPowerOffTime(int64 &hid,int64 &time)
{
    int  rc = -1;
    
    ostringstream   where;
    where  << "  hid = " << hid ;
    SqlCallbackable sqlcb(_db);
    time = 0;
    rc = sqlcb.SelectColumn(_table,
                            "total_poweroff_time",
                            where.str(),
                            time);

    if((SQLDB_OK != rc)
       &&(SQLDB_RESULT_EMPTY != rc))
    { 
        return ERROR;
    }

    return SUCCESS;
}
/******************************************************************************/
int32 HostAtca::Add(const host_atca_table_info &info)
{ 
    SqlCommand sql(_db,  _table);
    sql.Add("hid",        info.hid);
    sql.Add("bureau",     info.host_position.bureau);
    sql.Add("rack",       info.host_position.rack);
    sql.Add("shelf",      info.host_position.shelf);
    sql.Add("slot",       info.host_position.slot);
    sql.Add("board_type", info.board_type);
    sql.Add("power_state", info.power_state);
    sql.Add("power_operate", info.power_operate);
    sql.Add("rate_power", info.rate_power);
    sql.Add("poweroff_time", info.poweroff_time);
    sql.Add("total_poweroff_time", info.total_poweroff_time);
    sql.Add("description", "");

    return sql.Insert();     
}

/******************************************************************************/
int32 HostAtca::GetRatePower(int64& hid, int32& rate_power)
{
    int rc = -1;
    ostringstream where;
    where << "  hid = " << hid ;
    SqlCallbackable sqlcb(_db);
    rate_power = 0;
    rc = sqlcb.SelectColumn(_table,
                            "rate_power",
                            where.str(),
                            rate_power);
    if ((SQLDB_OK != rc) && (SQLDB_RESULT_EMPTY != rc))
    { 
        return ERROR;
    }
    return SUCCESS;
}

} // namespace zte_tecs

