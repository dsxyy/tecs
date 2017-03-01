/*******************************************************************************
* Copyright (c) 2012������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�base_image.h
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��image��Ķ����ļ�
* ��ǰ�汾��2.0
* ��    �ߣ�yanwei
* ������ڣ�2012��11��19��
*
* �޸ļ�¼1��
*    �޸����ڣ�2012/11/19
*    �� �� �ţ�V2.0
*    �� �� �ˣ�yanwei
*    �޸����ݣ���base_image������Ϊ��Դ�࣬���ݿ���ز�������base_image_pool����
*******************************************************************************/
#ifndef IMAGE_MANGER_BASE_IMAGE_H        
#define IMAGE_MANGER_BASE_IMAGE_H
#include "sky.h"
#include "tecs_pub.h"
#include "image_url.h"

namespace zte_tecs
{

class BaseImage:public Serializable 
{
public:
    BaseImage(const string &base_uuid, int64 base_id, int64 image_id)
    {
        _base_uuid = base_uuid;
        _base_id = base_id;
        _image_id = image_id; 
    }

    BaseImage(const string &base_uuid)
    {
        _base_uuid = base_uuid;
        _base_id = INVALID_OID;
        _image_id = INVALID_OID;   
    }

    BaseImage()
    {
        _base_id = INVALID_OID;
        _image_id = INVALID_OID;    
    }

    ~BaseImage(){};   

    SERIALIZE
    {
        SERIALIZE_BEGIN(BaseImage);
        WRITE_VALUE(_base_uuid);
        WRITE_VALUE(_base_id);
        WRITE_VALUE(_image_id);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(BaseImage);
        READ_VALUE(_base_uuid);
        READ_VALUE(_base_id);
        READ_VALUE(_image_id);
        DESERIALIZE_END();
    };

    string            _base_uuid;
    int64             _base_id;
    int64             _image_id;
};

class BaseImageCluster:public Serializable 
{
public:
    BaseImageCluster(){};

    BaseImageCluster(const string & base_uuid , const string &cluster_name):
    _base_uuid(base_uuid), _cluster_name(cluster_name)   
    {
    }

    ~BaseImageCluster(){};   

    SERIALIZE
    {
        SERIALIZE_BEGIN(BaseImageCluster);
        WRITE_VALUE(_base_uuid);
        WRITE_VALUE(_cluster_name);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(BaseImageCluster);
        READ_VALUE(_base_uuid);
        READ_VALUE(_cluster_name);
        DESERIALIZE_END();
    };

    string            _base_uuid;
    string            _cluster_name;
};

class BaseImageClusterWithImageId:public Serializable 
{
public:
    BaseImageClusterWithImageId()
    {
        _base_id = INVALID_OID;
        _image_id = INVALID_FILEID;  
    }

    ~BaseImageClusterWithImageId(){};   

    SERIALIZE
    {
        SERIALIZE_BEGIN(BaseImageClusterWithImageId);
        WRITE_VALUE(_base_uuid);
        WRITE_VALUE(_cluster_name);
        WRITE_VALUE(_base_id);    
        WRITE_VALUE(_image_id);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(BaseImageClusterWithImageId);
        READ_VALUE(_base_uuid);
        READ_VALUE(_cluster_name);
        READ_VALUE(_base_id);
        READ_VALUE(_image_id);
        DESERIALIZE_END();
    };

    string            _base_uuid;
    string            _cluster_name;
    int64             _base_id;
    int64             _image_id;
};

class BaseImageLabels: public WorkflowLabels
{
public:
    BaseImageLabels():
        _base_uuid(label_string_1),
        _cluster_name(label_string_2),
        _image_id(label_int64_1),
        _uid(label_int64_2),
        _base_id(label_int64_3)
    {
    };
        
    string& _base_uuid;
    string& _cluster_name;
    int64& _image_id;
    int64& _uid;
    int64& _base_id;

};

class BaseImageActionLabels: public ActionLabels
{
public:
    BaseImageActionLabels():
        _base_uuid(label_string_1),
        _image_id(label_int64_1),
        _cluster_name(label_string_2)
    {
    };
        
    string& _base_uuid;
    int64& _image_id;
    string& _cluster_name;
};


/**
@brief ��������: ӳ����Դ�ص�ʵ����\n
@li @b ����˵��: ��
*/
class BaseImagePool:public SqlCallbackable
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    static BaseImagePool *GetInstance()
    {
        SkyAssert(NULL != _instance);
        return _instance;
    };
    
    static BaseImagePool *GetInstance(SqlDB *database)
    {
        if(NULL == _instance)
        {
            _instance = new BaseImagePool(database);
        }
        
        if ((NULL != _instance) && (SUCCESS != _instance->Init()))
        {
            delete _instance;
            _instance = NULL;
        }

        return _instance;
    };
    virtual STATUS Init();
    int64 GetLastId(void)
    {
        return _last_base_id;
    }
    void SetLastId(int64 lastID)
    {
        _last_base_id = lastID;
    }
    void Lock(void)
    {
        _mutex.Lock();
    }
    void UnLock(void)
    {
        _mutex.Unlock();
    }

    int Allocate(BaseImage  &bi);
    int Drop(const BaseImage  &bi);
    int Update(const BaseImage  &bi);
    int Select(vector<BaseImage> &vec_bi, const string &where);
    /* ����������ݿ���ʺ��� */

    int SelectCallback(void *nil, SqlColumn * columns);    
    /* ������ӻص����� */

    void PrintLastId(void);

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
        BASE_UUID  = 0,
        BASE_ID    = 1,
        IMAGE_ID   = 2,     
        LIMIT      = 3
    };


    BaseImagePool(SqlDB *pDb):SqlCallbackable(pDb)
    {
        
    };
    Mutex _mutex;
    static int64 _last_base_id;
    static BaseImagePool *_instance;
    static const char   *_table;
    static const char   *_col_names;
    static const char   *_table_workflow;    
    DISALLOW_COPY_AND_ASSIGN(BaseImagePool); 
};

/**
@brief ��������: ӳ����Դ�ص�ʵ����\n
@li @b ����˵��: ��
*/
class BaseImageClusterPool:public SqlCallbackable
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    static BaseImageClusterPool *GetInstance()
    {
        SkyAssert(NULL != _instance);
        return _instance;
    };
    
    static BaseImageClusterPool *GetInstance(SqlDB *database)
    {
        if(NULL == _instance)
        {
            _instance = new BaseImageClusterPool(database);
        }

        return _instance;
    };
    int Allocate(BaseImageCluster &bic);
    int Drop(const BaseImageCluster &bic);
    int Update(const BaseImageCluster &bic);
    int Select(vector<BaseImageCluster> &vec_bic, const string &where);
    /* ����������ݿ���ʺ��� */

    int SelectCallback(void *nil, SqlColumn * columns);    
    /* ������ӻص����� */


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
        BASE_UUID    = 0,
        CLUSTER_NAME = 1,
        LIMIT        = 2
    };


    BaseImageClusterPool(SqlDB *pDb):SqlCallbackable(pDb)
    {
    };
    
    static BaseImageClusterPool *_instance;
    static const char   *_table;
    static const char   *_col_names;
    DISALLOW_COPY_AND_ASSIGN(BaseImageClusterPool); 
};

/**
@brief ��������: ӳ����Դ�ص�ʵ����\n
@li @b ����˵��: ��
*/
class BaseImageClusterWithImageIdPool:public SqlCallbackable
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    static BaseImageClusterWithImageIdPool *GetInstance()
    {
        SkyAssert(NULL != _instance);
        return _instance;
    };
    
    static BaseImageClusterWithImageIdPool *GetInstance(SqlDB *database)
    {
        if(NULL == _instance)
        {
            _instance = new BaseImageClusterWithImageIdPool(database);
        }

        return _instance;
    };
    int Select(vector<BaseImageClusterWithImageId> &vec_bicwi, const string &where);
    /* ����������ݿ���ʺ��� */

    int SelectCallback(void *nil, SqlColumn * columns);    
    /* ������ӻص����� */


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
        BASE_UUID    = 0,
        CLUSTER_NAME = 1,
        BASE_ID      = 2,
        IMAGE_ID     = 3,
        LIMIT        = 4
    };


    BaseImageClusterWithImageIdPool(SqlDB *pDb):SqlCallbackable(pDb)
    {
    };
    
    static BaseImageClusterWithImageIdPool *_instance;
    static const char   *_table;
    static const char   *_col_names;
    DISALLOW_COPY_AND_ASSIGN(BaseImageClusterWithImageIdPool); 
};


/**
@brief ��������: BaseImageOperationSet���ݿ�����ӿڷ�װ��\n
@li @b ����˵��: ��
*/
class BaseImageOperationSet: public SqlCallbackable
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    static BaseImageOperationSet *GetInstance()
    {
        SkyAssert(NULL != _instance);
        return _instance;
    };

    static BaseImageOperationSet *GetInstance(SqlDB *database)
    {
        if(NULL == _instance)
        {
            _instance = new BaseImageOperationSet(database);
        }
        if ((NULL != _instance) && (SUCCESS != _instance->Init(database)))
        {
            delete _instance;
            _instance = NULL;
        }
        return _instance;
    };

    BaseImageOperationSet()
    {
        _base_image = NULL;
        _base_image_cluster = NULL;
        _base_image_cluster_with_image_id = NULL;
    }
    int Init(SqlDB *pDb); 

    BaseImageOperationSet(SqlDB *pDb):SqlCallbackable(pDb)
    {
    };

    BaseImagePool *_base_image;
    BaseImageClusterPool *_base_image_cluster;
    BaseImageClusterWithImageIdPool *_base_image_cluster_with_image_id;
    static BaseImageOperationSet *_instance;
    DISALLOW_COPY_AND_ASSIGN(BaseImageOperationSet);
};

STATUS AllocateBaseImage(const string &base_uuid, const string &cluster_name, int64 image_id, int64 base_id);
STATUS DeleteBaseImage(const string &base_uuid,const string &cluster_name);
STATUS GetBaseImageIndex(int64 &index);
STATUS GetBaseImageFromTable(int64 image_id, const string &cluster, vector<BaseImageClusterWithImageId> &vec_var);
STATUS GetBaseImageFromTable(const string &base_uuid, const string &cluster, vector<BaseImageClusterWithImageId> &vec_var);

}
#endif /* IMAGE_MANGER_BASE_IMAGE_H */

