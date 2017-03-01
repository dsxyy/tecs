/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�affinity_region_pool.cpp
* �ļ���ʶ��
* ����ժҪ��affinity_region��Դ�����ʵ���ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�
* ������ڣ�2013��8��26��
*
* �޸ļ�¼1��
*    �޸����ڣ�2013/8/26
*    �� �� �ţ�V1.0
*    �� �� �ˣ�Bob
*    �޸����ݣ�����
*******************************************************************************/
#include "affinity_region_pool.h"
#include "pub.h"
#include "log_agent.h"
#include "api_const.h"
#include "authrequest.h"
#include "affinity_region_operate.h"
#include "user_pool.h"

namespace zte_tecs
{

int64        AffinityRegionPool::_lastOID  = INVALID_OID;
AffinityRegionPool *AffinityRegionPool::_instance = NULL;
pthread_mutex_t    AffinityRegionPool::_mutex;

const char * AffinityRegionPool::_table_affinity_region = "affinity_region";
const char * AffinityRegionPool::_col_names_affinity_region =
                                              "oid, "
                                              "name, "
                                              "description, "
                                              "create_time, "
                                              "uid, "
                                              "level, "
                                              "is_forced";


const char * AffinityRegionPool::_view_affinity_region = "view_affinity_region";
const char * AffinityRegionPool::_col_names_view_affinity_region =
                                              "oid, "
                                              "name, "
                                              "description, "
                                              "user_name, "
                                              "level, "
                                              "is_forced, "
                                              "uid";

STATUS AffinityRegionPool::Init(const bool &isReadOnly)
{
    if(false == isReadOnly)
    {
        int64    tmp_oid = 0;
        string   where   = " 1=1 ";
        string   column  = " MAX(oid) ";
        int             rc;
    
        // 1.��ȡ���_oid
        SqlCallbackable sqlcb(_db);
        rc = sqlcb.SelectColumn( _table_affinity_region, column, where, tmp_oid);
        if((SQLDB_OK != rc) &&(SQLDB_RESULT_EMPTY != rc))
        {
                OutPut(SYS_DEBUG, "*** Select record from affinity_region_pool failed ***\n");
                return ERROR_DB_SELECT_FAIL;
        }
    
        _lastOID = tmp_oid;
    
        // ��ʼ��������
        pthread_mutex_init(&_mutex,0);
    }
    return SUCCESS;
}

/******************************************************************************/
int AffinityRegionPool::Allocate(AffinityRegion &affinity_region)
{
    int rc = -1;

    Lock();
    _lastOID++;
    affinity_region._oid = _lastOID;
    UnLock();

    SqlCommand sql(_db,_table_affinity_region);
    sql.Add("oid",affinity_region._oid);
    sql.Add("name",affinity_region._name);
    sql.Add("description",affinity_region._description);
    sql.Add("create_time",affinity_region._create_time);
    sql.Add("uid",affinity_region._uid);
    sql.Add("level",affinity_region._level);
    sql.Add("is_forced",affinity_region._is_forced);

    rc = sql.Insert();
    if (0 != rc)
    {
        SkyAssert(false);
        return ERROR_DB_INSERT_FAIL;
    }

    return SUCCESS;

}

/******************************************************************************/
STATUS AffinityRegionPool::GetAffinityRegion(int64 oid, AffinityRegion &affinity_region)
{
    ostringstream   oss;
    int             rc = -1;
    int64           tmp_oid = INVALID_OID;

    SetCallback(static_cast<Callbackable::Callback>(&AffinityRegionPool::SelectCallback),static_cast<void *>(&affinity_region));

    oss << "SELECT " << _col_names_affinity_region << " FROM " << _table_affinity_region << " WHERE oid = " << oid;

    tmp_oid = oid;
    affinity_region._oid  = INVALID_OID;

    rc = _db->Execute(oss, this);
    UnsetCallback();

    if (SQLDB_RESULT_EMPTY == rc)
    {
        //���ݿ�Ϊ��
        return  ERROR_OBJECT_NOT_EXIST;
    }

    if (SQLDB_OK != rc)
    {
        //���ݿ�ϵͳ����
        SkyAssert(false);
        return ERROR_DB_SELECT_FAIL;
    }

    if (affinity_region._oid != tmp_oid )
    {
        //���ݿ�ϵͳû���⣬����û�ҵ���¼
        return ERROR_DB_SELECT_FAIL;
    }

    return SUCCESS;
};

/******************************************************************************/
int AffinityRegionPool::SelectCallback(void * nil, SqlColumn * columns)
{
    if (NULL == columns ||
        LIMIT != columns->get_column_num())
    {
        return -1;
    }

    AffinityRegion* affinity_region = static_cast<AffinityRegion*>(nil);

    columns->GetValue(OID, affinity_region->_oid);
    columns->GetValue(NAME, affinity_region->_name);
    columns->GetValue(DESCRIPTION, affinity_region->_description);
    columns->GetValue(CREATE_TIME, affinity_region->_create_time);
    columns->GetValue(UID, affinity_region->_uid);
    columns->GetValue(LEVEL, affinity_region->_level);
    columns->GetValue(IS_FORCED, affinity_region->_is_forced);

    return 0;
}

/******************************************************************************/
int AffinityRegionPool::Update(const AffinityRegion &ar)
{
    SqlCommand sql(_db,_table_affinity_region);
    sql.Add("oid",ar._oid);
    sql.Add("name",ar._name);
    sql.Add("description",ar._description);
    sql.Add("create_time",ar._create_time);
    sql.Add("uid",ar._uid);
    sql.Add("level",ar._level);
    sql.Add("is_forced",ar._is_forced);

    return sql.Update(" WHERE oid = " +  to_string<int64>(ar._oid,dec));
};

/******************************************************************************/
int AffinityRegionPool::Drop(const AffinityRegion &ar)
{
    SqlCommand sql(_db, _table_affinity_region);

    return sql.Delete(" WHERE oid = " + to_string(ar._oid, dec));

};


/******************************************************************************/
int AffinityRegionPool::Show(vector<ApiAffinityRegionInfo> &vec_ar_info, const string &where)
{
    int             rc;
    ostringstream   cmd;

   SetCallback(static_cast<Callbackable::Callback>(&AffinityRegionPool::ShowCallback),
               static_cast<void *>(&vec_ar_info));

   cmd << "SELECT "<< _col_names_view_affinity_region << " FROM "
        << _view_affinity_region;

    if (!where.empty())
    {
        cmd << " WHERE " << where;
    }

    rc = _db->Execute(cmd, this);

    UnsetCallback();

    if ((SQLDB_OK != rc)
         &&(SQLDB_RESULT_EMPTY != rc))
    {
        return -1;
    }

    return 0;
}

/******************************************************************************/
int AffinityRegionPool::ShowCallback(void * arg, SqlColumn * columns)
{
    vector<ApiAffinityRegionInfo> *ptAgRecord;

    ptAgRecord = static_cast<vector<ApiAffinityRegionInfo> *>(arg);

    ApiAffinityRegionInfo  tInfo;

    if ( NULL == columns ||
         VIEW_LIMIT != columns->get_column_num())
    {
        return -1;
    }

    columns->GetValue(VIEW_OID, tInfo.id);
    columns->GetValue(VIEW_NAME, tInfo.name );
    columns->GetValue(VIEW_DESCRIPTION, tInfo.description);
    columns->GetValue(VIEW_USER_NAME, tInfo.user_name);
    columns->GetValue(VIEW_LEVEL, tInfo.level);
    columns->GetValue(VIEW_FORCED, tInfo.is_forced);

    ptAgRecord->push_back(tInfo);

    return 0;

}

/******************************************************************************/
int64 AffinityRegionPool::Count(const string &where)
{
    ostringstream   sql;
    int64 count = 0;
    int rc;

    rc = SelectColumn(_table_affinity_region,"COUNT(*)",where,count);

    if ((SQLDB_OK != rc )
        &&(SQLDB_RESULT_EMPTY!= rc))
    {
        return -1;
    }

    if(SQLDB_RESULT_EMPTY == rc)
    {
       return 0;
    }

    return count;
}
/******************************************************************************/
bool AffinityRegionPool::Authorize(int64 opt_uid, AffinityRegion *buf, int oprate)
{
    /* ��ȡ��Ȩ */
    AuthRequest ar(opt_uid);
    int64       oid = INVALID_OID;
    int64       self_uid = INVALID_OID;

    if (buf == NULL)
    {
        oid = 0;
        self_uid = 0;
    }
    else
    {
        oid      = buf->get_oid();
        self_uid = buf->get_uid();
        if (INVALID_UID == self_uid)
        {
           Drop(*buf);
           return false;
        }
    }
    ar.AddAuthorize(
                    AuthRequest::AFFINITY_REGION,           //��Ȩ��������,��user������
                    oid,                             //�����ID,oid
                    (AuthRequest::Operation)oprate,  //��������
                    self_uid,                        //����ӵ���ߵ��û�ID
                    false                            //�˶����Ƿ�Ϊ����
                    );

    if (-1 == UserPool::GetInstance()->Authorize(ar))
    {   /* ��Ȩʧ�� */

        return false;
    }

    return true;

}

}




