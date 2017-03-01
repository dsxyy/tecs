/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：house_pool.cpp
* 文件标识：见配置管理计划书
* 内容摘要：HousePool类的实现文件
* 当前版本：1.0 
* 作    者：Bob
* 完成日期：2012年9月18日
*
* 修改记录1：
*    修改日期：2012/9/18
*    版 本 号：V1.0
*    修 改 人：Bob
*    修改内容：创建
*******************************************************************************/

#include "house_pool.h"
#include "door_pool.h"

namespace zte_tecs{


HousePool * HousePool::_instance = NULL;

int64 HousePool::_lastOID = -1;

const char * HousePool::_table_house  = 
        "house_pool"; //表名
        
         
const char * HousePool::_col_names_house = 
        "oid,"
	    "name,"
	    "uid ";

/******************************************************************************/
/* 初始化：获取max oid                                                                    */
/******************************************************************************/
int HousePool::Init()
{
        int         rc  = -1;
        string  column  = " MAX(oid) ";
        string   where  = " 1=1 ";
        int64   tmp_oid = -1;
    
        // 获取最大的oid
        SqlCallbackable sqlcb(_db);
        rc = sqlcb.SelectColumn(_table_house,
                                column,
                                where,
                                tmp_oid);
        
        if((SQLDB_OK != rc)
           &&(SQLDB_RESULT_EMPTY != rc))
        { 
            printf("*** Select record from house_pool failed ***\n");
            SkyAssert(false);
            return ERROR_DB_SELECT_FAIL;
        }
    
       _lastOID = tmp_oid;
       return SUCCESS;

}

/******************************************************************************/
int HousePool::Allocate(House  &house)
{
    int         rc;

    Transaction::Begin();
    
    // 1. house_pool表
    SqlCommand sql(_db, _table_house, SqlCommand::WITH_TRANSACTION);
    
    house._oid = ++_lastOID;
    sql.Add("oid",  house._oid);
    sql.Add("name", house._name);
    sql.Add("uid",  house._uid);    

    rc = sql.Insert();
	if(0 != rc)
    {
	    --_lastOID;
        Transaction::Cancel();
        SkyAssert(false);
        return rc;
    } 

    // 2. house_door 表
    DoorPool *p_door = DoorPool::GetInstance(_db);
	if (NULL == p_door)
    {
	    --_lastOID;
        Transaction::Cancel();
        SkyAssert(false);
        return rc;        
    }
	
    rc = p_door->Insert(house._doors, house._oid, true);
    if(0 != rc)
    {
	    --_lastOID;
        Transaction::Cancel();
        SkyAssert(false);
        return rc;
    }
	
    rc = Transaction::Commit();
    if (SUCCESS != rc)
    {
      --_lastOID;
    }
    
    return rc;
}

/******************************************************************************/
int HousePool::Drop(const House  &house)
{
    int           rc = -1;
    SqlCommand sql(_db, _table_house);

    rc = sql.Delete(" WHERE oid =" +  to_string(house._oid,dec));

    return rc;
}


/******************************************************************************/
int HousePool::Update(House  &house)
{
    int             rc;

    Transaction::Begin();
    
    // 1. house_pool表
    SqlCommand sql(_db, _table_house);
    sql.Add("oid",  house._oid);
    sql.Add("name", house._name);
    sql.Add("uid",  house._uid);
    
    rc = sql.Update("WHERE oid = " + to_string(house._oid,dec));
    if(0 != rc)
    {
        Transaction::Cancel();
        SkyAssert(false);
        return rc;
    } 

    // 2. house_door 表
    DoorPool *p_door = DoorPool::GetInstance(_db);
    if (NULL == p_door)
    {
        Transaction::Cancel();
        SkyAssert(false);
        return rc;        
    }
    
    rc = p_door->Update(house._doors, house._oid, true);
    if(0 != rc)
    {
        Transaction::Cancel();
        SkyAssert(false);
        return rc;
    }
 
    return Transaction::Commit();
  
}

/******************************************************************************/
int HousePool::Select(vector<House> &vec_house,const string &where)
{
    ostringstream   oss;
    int             rc;

 
    SetCallback(static_cast<Callbackable::Callback>(&HousePool::SelectCallback),
                static_cast<void *> (&vec_house));

    oss << "SELECT " << _col_names_house << 
           " FROM " << _table_house << 
           " WHERE  " << where;

 
    rc = _db->Execute(oss, this);

    UnsetCallback();
    if ( (SQLDB_OK != rc) && (SQLDB_RESULT_EMPTY != rc)  )
    {
        //数据库系统出错
        SkyAssert(false);
        return rc;
    } 
  
    return SUCCESS;
}


/******************************************************************************/
int HousePool::SelectCallback(void * nil, SqlColumn * columns)
{
    if (NULL == columns ||          
        LIMIT != columns->get_column_num())    
    {        
        return -1;   
    }   

    vector<House> *  p_vector;

    p_vector = static_cast<vector<House> *>(nil);
    
    House house;
    columns->GetValue(OID,  house._oid);
    columns->GetValue(NAME, house._name);
    columns->GetValue(UID, house._uid);

    p_vector->push_back(house);
	
    return 0;
}

/******************************************************************************/

}
	


