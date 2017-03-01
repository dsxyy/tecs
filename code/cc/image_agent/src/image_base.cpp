/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：image_base.cpp
* 文件标识：
* 内容摘要：ImageBase类的定义文件
* 当前版本：1.0
* 作    者：颜伟
* 完成日期：2013年1月5日
*
* 修改记录1：
*     修改日期：2013/1/5
*     版 本 号：V1.0
*     修 改 人：颜伟
*     修改内容：创建
*******************************************************************************/
#include "storage.h"
#include "log_agent.h"
#include "image_base.h"

namespace zte_tecs {

/******************************************************************************/  
const char * ImageBasePool::_table = 
        "image_base";

const char * ImageBasePool::_col_names = 
        "base_uuid,"
        "image_id,"
        "size,"
        "access_url";

ImageBasePool *ImageBasePool::_instance = NULL;

/******************************************************************************/
int ImageBasePool::Allocate(const ImageBase &var)
{
    int rc = -1;
    SqlCommand sql(_db, _table, SqlCommand::WITH_TRANSACTION);
    
    sql.Add("base_uuid", var._base_uuid);
    sql.Add("image_id", var._image_id);
    sql.Add("size", var._size);
    sql.Add("access_url", var._access_url);
    
    rc = sql.Insert();
    if (0 != rc)
    {
        //数据库系统出错
        SkyAssert(false);
        return ERROR_DB_INSERT_FAIL;
    }

    return SUCCESS;
}

/******************************************************************************/
int ImageBasePool::Drop(const string& base_uuid)
{
    int rc = -1;
    SqlCommand sql(_db, _table, SqlCommand::WITH_TRANSACTION);

    rc = sql.Delete(" WHERE base_uuid = '" + base_uuid + "'");
    if ((0 != rc) && (SQLDB_RESULT_EMPTY != rc))
    {
        SkyAssert(false);
        return ERROR_DB_DELETE_FAIL;
    }
    
    return SUCCESS;
}

/******************************************************************************/
int ImageBasePool::Drop(const ImageBase &var)
{
    int rc = -1;
    SqlCommand sql(_db, _table, SqlCommand::WITH_TRANSACTION);

    rc = sql.Delete(" WHERE base_uuid = '" + var._base_uuid + "'");
    if ((0 != rc) && (SQLDB_RESULT_EMPTY != rc))
    {
        SkyAssert(false);
        return ERROR_DB_DELETE_FAIL;
    }
    
    return SUCCESS;
}

/******************************************************************************/
int ImageBasePool::Select(vector<ImageBase> &vec_var,const string &where)
{
    ostringstream   oss;
    int             rc = -1;

    SetCallback(static_cast<Callbackable::Callback>(&ImageBasePool::SelectCallback),
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
int ImageBasePool::SelectCallback(void * nil, SqlColumn * columns)
{
    if (NULL == columns ||          
        LIMIT != columns->get_column_num())    
    {        
        return -1;   
    }   

    vector<ImageBase> *p_vector;

    p_vector = static_cast<vector<ImageBase> *>(nil);

    ImageBase var;
    columns->GetValue(BASE_UUID, var._base_uuid);
    columns->GetValue(IMAGE_ID, var._image_id);
    columns->GetValue(SIZE, var._size);
    columns->GetValue(ACCESS_URL, var._access_url);
    
    p_vector->push_back(var);

    return 0;
}
/******************************************************************************/  
ImageBaseOperationSet *ImageBaseOperationSet::_instance = NULL;
/******************************************************************************/
int ImageBaseOperationSet::Init(SqlDB *pDb)
{   
    if (NULL == pDb)
    {
        return ERROR_INVALID_ARGUMENT;
    }
    if (NULL == (_image_base = ImageBasePool::GetInstance(pDb)))
    {
        return ERROR;
    }
    return SUCCESS;
}
/******************************************************************************/
int AllocateImageBase(const ImageBase& var)
{
    int ret = ERROR;
    ImageBaseOperationSet *_pSet = ImageBaseOperationSet::GetInstance();
    if (NULL == _pSet)
    {

        OutPut(SYS_ERROR,"ImageBaseOperationSet GetInstance failed");
        return ret;
    }
    
    ret = _pSet->_image_base->Allocate(var);
    if (SUCCESS != ret)
    {
        OutPut(SYS_ERROR,"Allocate image base  failed");
    }
    return ret;
}
/******************************************************************************/
int AllocateImageBase(const string& base_uuid,
                           int64 image_id,
                           int64 size,
                           const string& url)
{
    int ret = ERROR;
    ImageBaseOperationSet *_pSet = ImageBaseOperationSet::GetInstance();
    if (NULL == _pSet)
    {

        OutPut(SYS_ERROR,"ImageBaseOperationSet GetInstance failed");
        return ret;
    }
    ImageBase var(base_uuid,image_id,size,url);
    ret = _pSet->_image_base->Allocate(var);
    if (SUCCESS != ret)
    {
        OutPut(SYS_ERROR,"Allocate image base  failed");
    }
    return ret;
}
/******************************************************************************/
int DeleteImageBase(const string& base_uuid)
{
    int ret = ERROR;
    ImageBaseOperationSet *_pSet = ImageBaseOperationSet::GetInstance();
    if (NULL == _pSet)
    {

        OutPut(SYS_ERROR,"ImageBaseOperationSet GetInstance failed");
        return ret;
    }
    
    ret = _pSet->_image_base->Drop(base_uuid);
    if (SUCCESS != ret)
    {
        OutPut(SYS_ERROR,"Allocate image base  failed");
    }
    return ret;
}
/******************************************************************************/
int GetImageBaseFromTable(const string &base_uuid,vector<ImageBase> &vec_var)
{
    int ret = ERROR;
    ImageBaseOperationSet *_pSet = ImageBaseOperationSet::GetInstance();
    if (NULL == _pSet)
    {

        OutPut(SYS_ERROR,"ImageBaseOperationSet GetInstance failed");
        return ret;
    }

    string where = " base_uuid = '" + base_uuid + "'";
    
    ret = _pSet->_image_base->Select(vec_var, where);
    if (SUCCESS != ret)
    {
        OutPut(SYS_ERROR,"Select image base  failed");
    }
    return ret;
}

/******************************************************************************/
int GetImageBaseByBaseUuid(const string &base_uuid,ImageBase &var)
{
    int ret = ERROR;
    ImageBasePool *_pool = ImageBasePool::GetInstance();
    if (NULL == _pool)
    {

        OutPut(SYS_ERROR,"ImageBasePool GetInstance Failed");
        return ret;
    }

    string where = " base_uuid = '" + base_uuid + "'";
    vector<ImageBase> vec_var;
    
    ret = _pool->Select(vec_var, where);
    if (SUCCESS != ret)
    {
        OutPut(SYS_ERROR,"ImageBasePool Select Failed");
        return ret;
    }
    if (0 == vec_var.size())
    {
        ret = ERROR_OBJECT_NOT_EXIST;
    }
    else
    {
        var = vec_var[0];
    }
    return ret;
}

/******************************************************************************/
int GetImageBaseByImageid(int64 image_id, vector<string> &base_uuid_vec, int64 &size)
{
    int ret = ERROR;
    ImageBasePool *_pool = ImageBasePool::GetInstance();
    if (NULL == _pool)
    {

        OutPut(SYS_ERROR,"ImageBasePool GetInstance Failed");
        return ret;
    }

    string where = " image_id = " + to_string<int64>(image_id,dec);
    vector<ImageBase> vec_var;
    
    ret = _pool->Select(vec_var, where);
    if (SUCCESS != ret)
    {
        OutPut(SYS_ERROR,"ImageBasePool Select Failed");
        return ret;
    }
    if (0 == vec_var.size())
    {
        ret = ERROR_OBJECT_NOT_EXIST;
    }
    else
    {
        vector<ImageBase>::iterator it;
        for (it = vec_var.begin(); it != vec_var.end(); it++)
        {
            base_uuid_vec.push_back(it->_base_uuid);
        }
        size = vec_var[0]._size;
    }
    return ret;
}

}// namespace zte_tecs

