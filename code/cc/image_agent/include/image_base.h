/******************************************************************************
* Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：image_base.h
* 文件标识：
* 内容摘要：image_base表相关数据库操作结构头文件
* 当前版本：1.0
* 作    者：颜伟
* 完成日期：2013年3月12日
*
* 修改记录1：
*     修改日期：2013/3/12
*     版 本 号：V1.0
*     修 改 人：颜伟
*     修改内容：创建
******************************************************************************/
#ifndef TECS_IMAGE_BASE_H
#define TECS_IMAGE_BASE_H
#include "pool_sql.h"

namespace zte_tecs 
{

/**
@brief 功能描述: ImageBase action标签\n
@li @b 其它说明: 无
*/
class ImageBaseLabels: public WorkflowLabels
{
public:
    ImageBaseLabels():
        _base_uuid(label_string_1),
        _cluster_name(label_string_2),
        _image_id(label_int64_1),
        _size(label_int64_2)
    {
    };

    string& _base_uuid;
    string& _cluster_name;
    int64& _image_id;
    int64& _size;
};

/**
@brief 功能描述: 创建块action标签\n
@li @b 其它说明: 无
*/
class CreateBlockLabels: public ActionLabels
{
public:
    CreateBlockLabels():
        _request_id(label_string_1),
        _size(label_int64_1),
        _cluster_name(label_string_2)
    {
    };

    string& _request_id;
    int64& _size;
    string& _cluster_name;
};

/**
@brief 功能描述: 授权块action标签\n
@li @b 其它说明: 无
*/
class AuthBlockLabels: public ActionLabels
{
public:
    AuthBlockLabels():
        _request_id(label_string_1),
        _cluster_name(label_string_2),
        _vid(label_int64_1),
        _ip(label_string_3),
        _iscsi(label_string_4),
        _dist_id(label_int64_2)
    {
    };

    string& _request_id;
    string& _cluster_name;
    int64& _vid;
    string& _ip;
    string& _iscsi;
    int64& _dist_id;
};

/**
@brief 功能描述:下载镜像action标签\n
@li @b 其它说明: 无
*/
class DownloadImageLabels: public ActionLabels
{
public:
    DownloadImageLabels():
        _request_id(label_string_1),
        _source_url(label_string_2),
        _target_url(label_string_3)
    {
    };

    string& _request_id;
    string& _source_url;
    string& _target_url;
};

/**
@brief 功能描述: ImageBase实体类\n
@li @b 其它说明: 无
*/
class ImageBase
{
public:
    ImageBase() 
    {
        _image_id = INVALID_FILEID;
        _size = 0;
    };
    
    ImageBase(const string& base_uuid,int64 image_id,int64 size,const string& url):
    _base_uuid(base_uuid),_image_id(image_id),_size(size),_access_url(url)
    {
    };
    
    string _base_uuid;
    int64 _image_id;
    int64 _size;
    string _access_url;
};

/**
@brief 功能描述: ImageBase数据库操作类\n
@li @b 其它说明: 无
*/
class ImageBasePool : public SqlCallbackable
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    static ImageBasePool *GetInstance()
    {
        SkyAssert(NULL != _instance);
        return _instance;
    };
    
    static ImageBasePool *GetInstance(SqlDB *database)
    {
        if(NULL == _instance)
        {
            _instance = new ImageBasePool(database);
        }
        
        return _instance;
    };
     
    int Allocate(const ImageBase  &var);
    int Drop(const string& base_uuid);
    int Drop(const ImageBase  &var);
    int Select(vector<ImageBase> &vec_var, const string &where);
    /* 继续添加数据库访问函数 */

    int SelectCallback(void *nil, SqlColumn * columns);    
    /* 继续添加回调函数 */

/*******************************************************************************
* 2. protected section
*******************************************************************************/
// protected:

/*******************************************************************************
* 3. private section
*******************************************************************************/	
private:
     enum ColNames 
     {
         BASE_UUID = 0,
         IMAGE_ID = 1,
         SIZE = 2,
         ACCESS_URL = 3,
         LIMIT = 4
     };

    ImageBasePool(SqlDB *pDb):SqlCallbackable(pDb)
    {
        
    };
    static ImageBasePool *_instance;
    static const char   *_table;
    static const char   *_col_names;
    DISALLOW_COPY_AND_ASSIGN(ImageBasePool);
};


/**
@brief 功能描述: ImageBaseOperationSet数据库操作接口封装类\n
@li @b 其它说明: 无
*/
class ImageBaseOperationSet: public SqlCallbackable
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    static ImageBaseOperationSet *GetInstance()
    {
        SkyAssert(NULL != _instance);
        return _instance;
    };

    static ImageBaseOperationSet *GetInstance(SqlDB *database)
    {
        if(NULL == _instance)
        {
            _instance = new ImageBaseOperationSet(database);
        }
        if ((NULL != _instance) && (SUCCESS != _instance->Init(database)))
        {
            delete _instance;
            _instance = NULL;
        }
        return _instance;
    };

    ImageBaseOperationSet()
    {
        _image_base = NULL;
    }
    int Init(SqlDB *pDb); 

    ImageBaseOperationSet(SqlDB *pDb):SqlCallbackable(pDb)
    {
    };

    ImageBasePool *_image_base;
    static ImageBaseOperationSet *_instance;
    DISALLOW_COPY_AND_ASSIGN(ImageBaseOperationSet);
};
int AllocateImageBase(const ImageBase& var);
int AllocateImageBase(const string& base_uuid,
                           int64 image_id,
                           int64 size,
                           const string& url);
int DeleteImageBase(const string& base_uuid);
int GetImageBaseFromTable(const string &base_uuid,vector<ImageBase> &vec_var);
int GetImageBaseByBaseUuid(const string &base_uuid,ImageBase &var);
int GetImageBaseByImageid(int64 image_id, vector<string> &base_uuid_vec, int64 &size);

}

#endif


