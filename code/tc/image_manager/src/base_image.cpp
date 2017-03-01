/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：base_image_pool.cpp
* 文件标识：见配置管理计划书
* 内容摘要：Image类的实现文件
* 当前版本：2.0 
* 作    者：lichun
* 完成日期：2012年10月19日
*
* 修改记录1：
*    修改日期：2012/10/19
*    版 本 号：V2.0
*    修 改 人：lichun
*    修改内容：将image_pool改成非缓存模式
*******************************************************************************/
#include "base_image.h"
#include "log_agent.h"
#include "tecs_errcode.h"


namespace zte_tecs
{
/******************************************************************************/  
const char * BaseImagePool::_table = 
        "base_image";

const char * BaseImagePool::_col_names = 
        "base_uuid,"
        "base_id,"
        "image_id";

const char * BaseImagePool::_table_workflow = 
        "view_workflows";        
        
BaseImagePool *BaseImagePool::_instance = NULL;

int64 BaseImagePool::_last_base_id = INVALID_OID;

/******************************************************************************/
/* 初始化：获取max cache_id                                                   */
/******************************************************************************/
int BaseImagePool::Init()
{
    int rc = -1;
    string column = " MAX(base_id) ";
    string where  = " 1=1 ";
    int64 tmp_base_id = INVALID_OID;

    SqlCallbackable sqlcb(_db);
    rc = sqlcb.SelectColumn(_table,
                            column,
                            where,
                            tmp_base_id);

    if((SQLDB_OK != rc) && (SQLDB_RESULT_EMPTY != rc))
    { 
        OutPut(SYS_ERROR, "Select record from base_image failed %d\n",rc);
        SkyAssert(false);
        return ERROR_DB_SELECT_FAIL;
    }

    _last_base_id = tmp_base_id;

    
    column = " MAX(label_int64_3) ";
    ostringstream osswhere;
    osswhere << " category = '" << IMAGE_CATEGORY << "' AND name = '" << IM_DEPLOY_IMAGE << "'";
    rc = sqlcb.SelectColumn(_table_workflow,
                            column,
                            osswhere.str(),
                            tmp_base_id);

    if((SQLDB_OK != rc) && (SQLDB_RESULT_EMPTY != rc))
    { 
        OutPut(SYS_ERROR, "Select record from base_image_workflow failed %d\n",rc);
        SkyAssert(false);
        return ERROR_DB_SELECT_FAIL;
    }
    
    if (_last_base_id < tmp_base_id)
    {
        _last_base_id = tmp_base_id;
    }

    if (SUCCESS != _mutex.Init())
    {
        OutPut(SYS_ERROR, "base image mutex init failed\n");
        SkyAssert(false);
        return ERROR;
    }
    
    return SUCCESS;
}
/******************************************************************************/
int BaseImagePool::Allocate(BaseImage &var)
{
    int rc = -1;
    SqlCommand sql(_db, _table, SqlCommand::WITH_TRANSACTION);

    sql.Add("base_uuid", var._base_uuid);
    sql.Add("base_id", var._base_id);
    sql.Add("image_id", var._image_id);
    
    rc = sql.Insert();

    if (0 != rc)
    {
        return ERROR_DB_INSERT_FAIL;
    }

    return SUCCESS;
}
/******************************************************************************/
int BaseImagePool::Drop(const BaseImage &var)
{
    int rc = -1;
    SqlCommand sql(_db, _table, SqlCommand::WITH_TRANSACTION);

    rc = sql.Delete(" WHERE base_uuid = '" + var._base_uuid +"'");

    if (0 != rc && SQLDB_RESULT_EMPTY != rc)
    {
        return ERROR_DB_DELETE_FAIL;
    }
    return SUCCESS;
}
/******************************************************************************/
int BaseImagePool::Select(vector<BaseImage> &vec_var,const string &where)
{
    ostringstream oss;
    int rc = -1;

    SetCallback(static_cast<Callbackable::Callback>(&BaseImagePool::SelectCallback),
                static_cast<void *> (&vec_var));

    oss << "SELECT " << _col_names << 
           " FROM " << _table << 
           " WHERE  " << where;

 
    rc = _db->Execute(oss, this);

    UnsetCallback();
    if(SQLDB_RESULT_EMPTY == rc) 
    {
        return ERROR_OBJECT_NOT_EXIST;
    }
    else if (SQLDB_OK != rc)
    {
        SkyAssert(false);
        return ERROR_DB_SELECT_FAIL;
    } 
  
    return SUCCESS;
}
/******************************************************************************/
int BaseImagePool::SelectCallback(void * nil, SqlColumn * columns)
{
    if (NULL == columns ||          
        LIMIT != columns->get_column_num())    
    {        
        return -1;   
    }   

    vector<BaseImage> *p_vector;

    p_vector = static_cast<vector<BaseImage> *>(nil);

    BaseImage var;

    columns->GetValue(BASE_UUID, var._base_uuid);
    columns->GetValue(BASE_ID, var._base_id);
    columns->GetValue(IMAGE_ID, var._image_id);
    
    p_vector->push_back(var);

    return 0;
}
/******************************************************************************/  
void BaseImagePool::PrintLastId(void)
{
    cout<<"LastBaseID:"<<_last_base_id<<endl;
}
/******************************************************************************/  
const char * BaseImageClusterPool::_table = 
        "base_image_cluster";

const char * BaseImageClusterPool::_col_names = 
        "base_uuid,"
        "cluster_name";
        
BaseImageClusterPool *BaseImageClusterPool::_instance = NULL;                                                            

/******************************************************************************/
int BaseImageClusterPool::Allocate(BaseImageCluster &var)
{
    int rc = -1;
    SqlCommand sql(_db, _table, SqlCommand::WITH_TRANSACTION);

    sql.Add("base_uuid", var._base_uuid);
    sql.Add("cluster_name", var._cluster_name);
    
    rc = sql.Insert();

    if (0 != rc)
    {
        SkyAssert(false);
        return ERROR_DB_INSERT_FAIL;
    }

    return SUCCESS;
}
/******************************************************************************/
int BaseImageClusterPool::Drop(const BaseImageCluster &var)
{
    int rc = -1;
    SqlCommand sql(_db, _table, SqlCommand::WITH_TRANSACTION);

    rc = sql.Delete(" WHERE base_uuid = '" + var._base_uuid
                   +"' AND cluster_name = '" + var._cluster_name + "'");

    if (0 != rc && SQLDB_RESULT_EMPTY != rc)
    {
        return ERROR_DB_DELETE_FAIL;
    }
    
    return SUCCESS;
}
/******************************************************************************/
int BaseImageClusterPool::Select(vector<BaseImageCluster> &vec_var,const string &where)
{
    ostringstream oss;
    int rc = -1;

    SetCallback(static_cast<Callbackable::Callback>(&BaseImageClusterPool::SelectCallback),
                static_cast<void *> (&vec_var));

    oss << "SELECT " << _col_names << 
           " FROM " << _table << 
           " WHERE  " << where;

 
    rc = _db->Execute(oss, this);

    UnsetCallback();
    
    if ( (SQLDB_OK != rc) && (SQLDB_RESULT_EMPTY != rc)  )
    {
        SkyAssert(false);
        return ERROR_DB_SELECT_FAIL;
    } 
  
    return SUCCESS;
}
/******************************************************************************/
int BaseImageClusterPool::SelectCallback(void * nil, SqlColumn * columns)
{
    if (NULL == columns ||          
        LIMIT != columns->get_column_num())    
    {        
        return -1;   
    }   

    vector<BaseImageCluster> *p_vector;

    p_vector = static_cast<vector<BaseImageCluster> *>(nil);

    BaseImageCluster var;
    
    columns->GetValue(BASE_UUID, var._base_uuid);
    columns->GetValue(CLUSTER_NAME, var._cluster_name);

    p_vector->push_back(var);

    return 0;
}
/******************************************************************************/  
const char * BaseImageClusterWithImageIdPool::_table = 
        "base_image_cluster_view";

const char * BaseImageClusterWithImageIdPool::_col_names = 
        "base_uuid,"
        "cluster_name,"
        "base_id,"
        "image_id";
        
BaseImageClusterWithImageIdPool *BaseImageClusterWithImageIdPool::_instance = NULL;                                                            
/******************************************************************************/
int BaseImageClusterWithImageIdPool::Select(vector<BaseImageClusterWithImageId> &vec_var,const string &where)
{
    ostringstream   oss;
    int             rc = -1;

    SetCallback(static_cast<Callbackable::Callback>(&BaseImageClusterWithImageIdPool::SelectCallback),
                static_cast<void *> (&vec_var));

    oss << "SELECT " << _col_names << 
           " FROM " << _table << 
           " WHERE  " << where;

 
    rc = _db->Execute(oss, this);

    UnsetCallback();
    
    if ((SQLDB_OK != rc) && (SQLDB_RESULT_EMPTY != rc)  )
    {
        SkyAssert(false);
        return rc;
    } 
  
    return SUCCESS;
}
/******************************************************************************/
int BaseImageClusterWithImageIdPool::SelectCallback(void * nil, SqlColumn * columns)
{
    if (NULL == columns ||          
        LIMIT != columns->get_column_num())    
    {        
        return -1;   
    }   

    vector<BaseImageClusterWithImageId> *p_vector;

    p_vector = static_cast<vector<BaseImageClusterWithImageId> *>(nil);

    BaseImageClusterWithImageId var;
    
    columns->GetValue(BASE_UUID, var._base_uuid);
    columns->GetValue(CLUSTER_NAME, var._cluster_name);
    columns->GetValue(BASE_ID, var._base_id);
    columns->GetValue(IMAGE_ID, var._image_id);
    
    p_vector->push_back(var);

    return 0;
}

/******************************************************************************/  
BaseImageOperationSet *BaseImageOperationSet::_instance = NULL;
/******************************************************************************/
STATUS BaseImageOperationSet::Init(SqlDB *pDb)
{   
    if (NULL == pDb)
    {
        return ERROR_INVALID_ARGUMENT;
    }
    if (NULL == (_base_image = BaseImagePool::GetInstance(pDb)))
    {
        return ERROR;
    }
    if (NULL == (_base_image_cluster = BaseImageClusterPool::GetInstance(pDb)))
    {
        return ERROR;
    }
    if (NULL == (_base_image_cluster_with_image_id = BaseImageClusterWithImageIdPool::GetInstance(pDb)))
    {
        return ERROR;
    }
    return SUCCESS;
}
/******************************************************************************/
STATUS AllocateBaseImage(const string &base_uuid, const string &cluster_name, int64 image_id, int64 base_id)
{
    STATUS rc = ERROR;
    BaseImageOperationSet *pSet = BaseImageOperationSet::GetInstance();
    if (NULL == pSet)
    {
        SkyAssert(false);
        return rc;
    }

    rc = Transaction::Begin();
    if (SUCCESS != rc)
    {
        SkyAssert(false);
        return rc;
    }

    BaseImage bi(base_uuid, base_id, image_id);   
    BaseImageCluster bic(base_uuid, cluster_name);
    // 1. BaseImagePool表
    rc = pSet->_base_image->Allocate(bi);
    if(0 != rc)
    {
        Transaction::Cancel();
        SkyAssert(false);
        return rc;
    } 
    // 2. BaseImageClusterPool表
    rc = pSet->_base_image_cluster->Allocate(bic); 
    if(0 != rc)
    {
        Transaction::Cancel();
        SkyAssert(false);
        return rc;
    } 
    
    rc = Transaction::Commit();
    if (SUCCESS != rc)
    {
        SkyAssert(false);
    }
    
    return rc;
}
/******************************************************************************/
STATUS DeleteBaseImage(const string &base_uuid,const string &cluster_name)
{
    STATUS rc = ERROR;
    BaseImageOperationSet *pSet = BaseImageOperationSet::GetInstance();
    if (NULL == pSet)
    {
        SkyAssert(false);
        return rc;
    }

    rc = Transaction::Begin();
    if (SUCCESS != rc)
    {
        SkyAssert(false);
        return rc;
    }
    BaseImageCluster cic(base_uuid,cluster_name);
    rc = pSet->_base_image_cluster->Drop(cic);
    if(0 != rc)
    {
        Transaction::Cancel();
        SkyAssert(false);
        return rc;
    } 
    
    BaseImage bi(base_uuid);
    rc = pSet->_base_image->Drop(bi);
    if(0 != rc)
    {
        Transaction::Cancel();
        SkyAssert(false);
        return rc;
    } 
    
    rc = Transaction::Commit();
    if (SUCCESS != rc)
    {
        SkyAssert(false);
    }
    
    return rc;
    
}
/******************************************************************************/
STATUS GetBaseImageIndex(int64 &index)
{
    STATUS rc = ERROR;
    BaseImageOperationSet *pSet = BaseImageOperationSet::GetInstance();
    if (NULL == pSet)
    {
        SkyAssert(false);
        return rc;
    }
    
    pSet->_base_image->Lock();
    
    index = pSet->_base_image->GetLastId()+1;

    pSet->_base_image->SetLastId(index);

    pSet->_base_image->UnLock();
    
    return SUCCESS;
}
/******************************************************************************/
STATUS GetBaseImageFromTable(int64 image_id, const string &cluster, vector<BaseImageClusterWithImageId> &vec_var)
{
    STATUS rc = ERROR;
    BaseImageOperationSet *pSet = BaseImageOperationSet::GetInstance();
    if (NULL == pSet)
    {
        SkyAssert(false);
        return rc;
    }

    string where = " image_id = " + to_string<int>(image_id,dec)
                  +" And cluster_name = '" + cluster +"'";
    
    rc = pSet->_base_image_cluster_with_image_id->Select(vec_var, where);
    if(SUCCESS != rc)
    {
        SkyAssert(false);
        return rc;
    } 

    if (0 == vec_var.size())
    {    
        return ERROR_OBJECT_NOT_EXIST;
    }
    
    return SUCCESS;
}
/******************************************************************************/
STATUS GetBaseImageFromTable(const string &base_uuid, const string &cluster, vector<BaseImageClusterWithImageId> &vec_var)
{
    STATUS rc = ERROR;
    BaseImageOperationSet *pSet = BaseImageOperationSet::GetInstance();
    if (NULL == pSet)
    {
        SkyAssert(false);
        return rc;
    }

    string where = " base_uuid = '" + base_uuid
                  +"' And cluster_name = '" + cluster +"'";
    
    rc = pSet->_base_image_cluster_with_image_id->Select(vec_var, where);
    if(SUCCESS != rc)
    {
        SkyAssert(false);
        return rc;
    } 

    if (0 == vec_var.size())
    {    
        return ERROR_OBJECT_NOT_EXIST;
    }
    
    return SUCCESS;
}

}

    
