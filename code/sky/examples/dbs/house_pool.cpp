/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�house_pool.cpp
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��HousePool���ʵ���ļ�
* ��ǰ�汾��1.0 
* ��    �ߣ�Bob
* ������ڣ�2012��9��18��
*
* �޸ļ�¼1��
*    �޸����ڣ�2012/9/18
*    �� �� �ţ�V1.0
*    �� �� �ˣ�Bob
*    �޸����ݣ�����
*******************************************************************************/

#include "house_pool.h"
#include "door_pool.h"

namespace zte_tecs{


HousePool * HousePool::_instance = NULL;

int64 HousePool::_lastOID = -1;

const char * HousePool::_table_house  = 
        "house_pool"; //����
        
         
const char * HousePool::_col_names_house = 
        "oid,"
	    "name,"
	    "uid ";

/******************************************************************************/
/* ��ʼ������ȡmax oid                                                                    */
/******************************************************************************/
int HousePool::Init()
{
        int         rc  = -1;
        string  column  = " MAX(oid) ";
        string   where  = " 1=1 ";
        int64   tmp_oid = -1;
    
        // ��ȡ����oid
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
    
    // 1. house_pool��
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

    // 2. house_door ��
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
    
    // 1. house_pool��
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

    // 2. house_door ��
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
        //���ݿ�ϵͳ����
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
	


