/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：sc_db.h
* 文件标识：见配置管理计划书
* 内容摘要：sc数据库接口头文件
* 当前版本：1.0
* 作    者：颜伟
* 完成日期：2012年9月27日
*
* 修改记录1：
*     修改日期：2012/9/27
*     版 本 号：V1.0
*     修 改 人：颜伟
*     修改内容：创建
*******************************************************************************/
#ifndef SC_DB_H
#define SC_DB_H

#include "sky.h"
#include "pool_sql.h"
#include "sc_db_messages.h"

// 禁用拷贝构造宏定义
#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
            TypeName(const TypeName&); \
            void operator=(const TypeName&)
#endif

using namespace std;
namespace zte_tecs {

const int DEL_HOST_TBL=0x01;
const int DEL_LUN_TBL=0x02;
const int DEL_TARGET_TBL=0x04;

/**
@brief 功能描述: StorageAdaptorAddress数据库操作类\n
@li @b 其它说明: 无
*/
class StorageAdaptorAddressPool: public SqlCallbackable
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    static StorageAdaptorAddressPool *GetInstance()
    {
        SkyAssert(NULL != _instance);
        return _instance;
    };
    
    static StorageAdaptorAddressPool *GetInstance(SqlDB *database)
    {
        if(NULL == _instance)
        {
            _instance = new StorageAdaptorAddressPool(database);
        }
        
        return _instance;
    };
     
    int Allocate(const StorageAdaptorAddress  &var);
    int Drop(const StorageAdaptorAddress  &var);
    int Update(const StorageAdaptorAddress  &var);
    int Select(vector<StorageAdaptorAddress> &vec_var, const string &where);
    /* 继续添加数据库访问函数 */
    int GetIpByAppName (const string &app_name, string & ip);
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
         APPLICATION = 0,
         IP_ADDRESS = 1,
         LIMIT      = 2
     };

    StorageAdaptorAddressPool(SqlDB *pDb):SqlCallbackable(pDb)
    {        
    };
    static StorageAdaptorAddressPool *_instance;
    static const char   *_table;
    static const char   *_col_names;
    DISALLOW_COPY_AND_ASSIGN(StorageAdaptorAddressPool);
};

/**
@brief 功能描述: StorageAdaptor数据库操作类\n
@li @b 其它说明: 无
*/
class StorageAdaptorPool: public SqlCallbackable
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    static StorageAdaptorPool *GetInstance()
    {
        SkyAssert(NULL != _instance);
        return _instance;
    };
    
    static StorageAdaptorPool *GetInstance(SqlDB *database)
    {
        if(NULL == _instance)
        {
            _instance = new StorageAdaptorPool(database);
        }
        
        if ((NULL != _instance) && (SUCCESS != _instance->Init()))
        {
            delete _instance;
            _instance = NULL;
        }

        return _instance;
    };
    virtual STATUS Init();
    int Allocate(StorageAdaptor  &var);
    int Drop(const StorageAdaptor  &var);
    int Update(const StorageAdaptor  &var);
    int Select(vector<StorageAdaptor> &vec_var, const string &where);
    /* 继续添加数据库访问函数 */

    int SelectCallback(void *nil, SqlColumn * columns);    
    /* 继续添加回调函数 */

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
         SID           = 0,
         NAME          = 1,
         APPLICATION   = 2,
         CTRL_ADDRESS  = 3,
         TYPE          = 4,
         IS_ONLINE     = 5,
         ENABLED       = 6,
         DESCRIPTION   = 7,
         REGISTER_TIME = 8,
         REFRESH_TIME  = 9,
         LIMIT         = 10
     };

    StorageAdaptorPool(SqlDB *pDb):SqlCallbackable(pDb)
    {
    };
    Mutex _mutex;
    static int64 _lastSID;
    static StorageAdaptorPool *_instance;
    static const char   *_table;
    static const char   *_col_names;
    DISALLOW_COPY_AND_ASSIGN(StorageAdaptorPool);
};

/**
@brief 功能描述: StorageAddress数据库操作类\n
@li @b 其它说明: 无
*/
class StorageAddressPool: public SqlCallbackable
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    static StorageAddressPool *GetInstance()
    {
        SkyAssert(NULL != _instance);
        return _instance;
    };
    
    static StorageAddressPool *GetInstance(SqlDB *database)
    {
        if(NULL == _instance)
        {
            _instance = new StorageAddressPool(database);
        }
        
        return _instance;
    };
     
    int Allocate(const StorageAddress  &var);
    int Drop(const StorageAddress  &var);
    int Select(vector<StorageAddress> &vec_var, const string &where);
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
         SID           = 0,
         MEDIA_ADDRESS = 1,
         LIMIT         = 2
     };

    StorageAddressPool(SqlDB *pDb):SqlCallbackable(pDb)
    {
    };
    static StorageAddressPool *_instance;
    static const char   *_table;
    static const char   *_col_names;
    DISALLOW_COPY_AND_ASSIGN(StorageAddressPool);
};

/**
@brief 功能描述: StorageVg数据库操作类\n
@li @b 其它说明: 无
*/
class StorageVgPool: public SqlCallbackable
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    static StorageVgPool *GetInstance()
    {
        SkyAssert(NULL != _instance);
        return _instance;
    };
    
    static StorageVgPool *GetInstance(SqlDB *database)
    {
        if(NULL == _instance)
        {
            _instance = new StorageVgPool(database);
        }
        
        return _instance;
    };
     
    int Allocate(const StorageVg  &var);
    int Drop(const StorageVg  &var);
    int Update(const StorageVg  &var);
    int Select(vector<StorageVg> &vec_var, const string &where);
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
         SID        = 0,
         NAME       = 1,
         SIZE       = 2,
         LIMIT      = 3
     };

    StorageVgPool(SqlDB *pDb):SqlCallbackable(pDb)
    {
    };
    static StorageVgPool *_instance;
    static const char   *_table;
    static const char   *_col_names;
    DISALLOW_COPY_AND_ASSIGN(StorageVgPool);
};

/**
@brief 功能描述: StorageTarget数据库操作类\n
@li @b 其它说明: 无
*/
class StorageTargetPool: public SqlCallbackable
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    static StorageTargetPool *GetInstance()
    {
        SkyAssert(NULL != _instance);
        return _instance;
    };
    
    static StorageTargetPool *GetInstance(SqlDB *database)
    {
        if(NULL == _instance)
        {
            _instance = new StorageTargetPool(database);
        }
        if ((NULL != _instance) && (SUCCESS != _instance->Init()))
        {
            delete _instance;
            _instance = NULL;
        }
        return _instance;
    };
    virtual STATUS Init();
    int64 GetLastId(int64 sid)
    {
        map<int64,int64>::iterator it = _lastID.find(sid);
        if (it == _lastID.end())
        {
            _lastID[sid] = -1;
        }
        return _lastID[sid];
    }
    void SetLastId(int64 sid,int64 lastID)
    {
        _lastID[sid] = lastID;
    }
    void Lock(void)
    {
        _mutex.Lock();
    }
    void UnLock(void)
    {
        _mutex.Unlock();
    }
    void PrintLastId(void);
    int Allocate(const StorageTarget  &var);
    int Drop(const StorageTarget  &var);
    int Select(vector<StorageTarget> &vec_var, const string &where);
    /* 继续添加数据库访问函数 */
     
    int SelectCallback(void *nil, SqlColumn * columns); 
    int SelectLastIdCallback(void * nil, SqlColumn * columns);
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
         SID       = 0,
         NAME      = 1,
         TARGET_ID = 2,
         LIMIT     = 3
     };

    StorageTargetPool(SqlDB *pDb):SqlCallbackable(pDb)
    {
    };
    Mutex _mutex;
    static map<int64,int64> _lastID;
    static StorageTargetPool *_instance;
    static const char   *_table;
    static const char   *_col_names;
    DISALLOW_COPY_AND_ASSIGN(StorageTargetPool);
};

/**
@brief 功能描述: StorageAuthorize数据库操作类\n
@li @b 其它说明: 无
*/
class StorageAuthorizePool: public SqlCallbackable
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    static StorageAuthorizePool *GetInstance()
    {
        SkyAssert(NULL != _instance);
        return _instance;
    };
    
    static StorageAuthorizePool *GetInstance(SqlDB *database)
    {
        if(NULL == _instance)
        {
            _instance = new StorageAuthorizePool(database);
        }
        
        return _instance;
    };
    int Allocate(const StorageAuthorize  &var);
    int Drop(const StorageAuthorize  &var);
    int DropByWhere(const string  &where);
    int Update(const StorageAuthorize  &var);
    int Select(vector<StorageAuthorize> &vec_var, const string &where);
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
         SID             = 0,
         TARGET_NAME     = 1,
         IP_ADDRESS      = 2,
         ISCSI_INITIATOR = 3,
         AUTH_UUID       = 4,
         LIMIT           = 5
     };

    StorageAuthorizePool(SqlDB *pDb):SqlCallbackable(pDb)
    {
    };
    static StorageAuthorizePool *_instance;
    static const char   *_table;
    static const char   *_col_names;
    DISALLOW_COPY_AND_ASSIGN(StorageAuthorizePool);
};


/**
@brief 功能描述: StorageCluster数据库操作类\n
@li @b 其它说明: 无
*/
class StorageClusterPool: public SqlCallbackable
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    static StorageClusterPool *GetInstance()
    {
        SkyAssert(NULL != _instance);
        return _instance;
    };
    
    static StorageClusterPool *GetInstance(SqlDB *database)
    {
        if(NULL == _instance)
        {
            _instance = new StorageClusterPool(database);
        }
        
        return _instance;
    };
     
    int Allocate(const StorageCluster  &var);
    int Drop(const StorageCluster  &var);
    int Select(vector<StorageCluster> &vec_var, const string &where);
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
         SID          = 0,
         CLUSTER_NAME = 1,
         LIMIT        = 2
     };

    StorageClusterPool(SqlDB *pDb):SqlCallbackable(pDb)
    {
    };
    static StorageClusterPool *_instance;
    static const char   *_table;
    static const char   *_col_names;
    DISALLOW_COPY_AND_ASSIGN(StorageClusterPool);
};

/**
@brief 功能描述: StorageVolume数据库操作类\n
@li @b 其它说明: 无
*/
class StorageVolumePool: public SqlCallbackable
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    static StorageVolumePool *GetInstance()
    {
        SkyAssert(NULL != _instance);
        return _instance;
    };
    
    static StorageVolumePool *GetInstance(SqlDB *database)
    {
        if(NULL == _instance)
        {
            _instance = new StorageVolumePool(database);
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
        return _lastID;
    }
    void SetLastId(int64 lastID)
    {
        _lastID = lastID;
    }
    void Lock(void)
    {
        _mutex.Lock();
    }
    void UnLock(void)
    {
        _mutex.Unlock();
    }
    int Allocate(const StorageVolume  &var);
    int Drop(const StorageVolume  &var);
    int Drop(const string &where);
    int Update(const StorageVolume  &var);
    int UpdateUuid(const StorageVolume &var);
    int Select(vector<StorageVolume> &vec_var, const string &where);
    /* 继续添加数据库访问函数 */

    int SelectCallback(void *nil, SqlColumn * columns);    
    /* 继续添加回调函数 */
    
    void PrintLastId(void);
    int GetVolNumOfVg(int sid, const string& vgname,int& volnum);
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
        SID         = 0,
        VG_NAME     = 1,
        NAME        = 2,
        SIZE        = 3,
        VOLUME_ID   = 4,
        VOLUME_UUID = 5,
        REQUEST_ID  = 6,
        LIMIT       = 7
    };

    StorageVolumePool(SqlDB *pDb):SqlCallbackable(pDb)
    {
    };
    Mutex _mutex;
    static int64 _lastID;
    static StorageVolumePool *_instance;
    static const char   *_table;
    static const char   *_col_names;
    DISALLOW_COPY_AND_ASSIGN(StorageVolumePool);
};

/**
@brief 功能描述: StorageLun数据库操作类\n
@li @b 其它说明: 无
*/
class StorageLunPool: public SqlCallbackable
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    #define MAX_STORAGE_LUN_ID 255
    static StorageLunPool *GetInstance()
    {
        SkyAssert(NULL != _instance);
        return _instance;
    };
    
    static StorageLunPool *GetInstance(SqlDB *database)
    {
        if(NULL == _instance)
        {
            _instance = new StorageLunPool(database);
        }
        if ((NULL != _instance) && (SUCCESS != _instance->Init()))
        {
            delete _instance;
            _instance = NULL;
        }

        return _instance;
    };
    virtual STATUS Init(); 
    int Allocate(const StorageLun  &var);
    int Drop(const StorageLun  &var);
    int UpdateLunUuid(const StorageLun  &var);
    int Select(vector<StorageLun> &vec_var, const string &where);
    /* 继续添加数据库访问函数 */
     
    int SelectCallback(void *nil, SqlColumn * columns);    
    /* 继续添加回调函数 */

    int AllocateFreeLunid(StorageLun  &var);
    int GetLastLunid(string target_name,int &lun_id);
    int ScanLunid(StorageLun  &var);
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
         SID         = 0,
         TARGET_NAME = 1,
         LUN_ID      = 2,
         LUN_UUID    = 3,
         VOLUME_UUID = 4,
         LIMIT       = 5
     };

    StorageLunPool(SqlDB *pDb):SqlCallbackable(pDb)
    {
    };
    Mutex _mutex;
    map<string,int> _last_mapid;
    static StorageLunPool *_instance;
    static const char   *_table;
    static const char   *_col_names;
    DISALLOW_COPY_AND_ASSIGN(StorageLunPool);
};

/**
@brief 功能描述: StorageAuthorizeLun数据库操作类\n
@li @b 其它说明: 无
*/
class StorageAuthorizeLunPool: public SqlCallbackable
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    static StorageAuthorizeLunPool *GetInstance()
    {
        SkyAssert(NULL != _instance);
        return _instance;
    };
    
    static StorageAuthorizeLunPool *GetInstance(SqlDB *database)
    {
        if(NULL == _instance)
        {
            _instance = new StorageAuthorizeLunPool(database);
        }
        
        return _instance;
    };
     
    int Allocate(const StorageAuthorizeLun  &var);
    int Drop(const StorageAuthorizeLun  &var);
    int DropByWhere(const string &where); 
    int Select(vector<StorageAuthorizeLun> &vec_var, const string &where);
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
         AUTH_UUID = 0,
         LUN_UUID = 1,
         LIMIT = 2
     };

    StorageAuthorizeLunPool(SqlDB *pDb):SqlCallbackable(pDb)
    {
    };
    static StorageAuthorizeLunPool *_instance;
    static const char   *_table;
    static const char   *_col_names;
    DISALLOW_COPY_AND_ASSIGN(StorageAuthorizeLunPool);
};

/**
@brief 功能描述: StorageUserVolume数据库操作类\n
@li @b 其它说明: 无
*/
class StorageUserVolumePool: public SqlCallbackable
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    static StorageUserVolumePool *GetInstance()
    {
        SkyAssert(NULL != _instance);
        return _instance;
    };
    
    static StorageUserVolumePool *GetInstance(SqlDB *database)
    {
        if(NULL == _instance)
        {
            _instance = new StorageUserVolumePool(database);
        }
        
        return _instance;
    };
     
    int Allocate(const StorageUserVolume  &var);
    int Drop(const StorageUserVolume  &var);
    int DropByWhere(const string &where);
    int UpdateUsage(const string &where, const string &usage);
    int Select(vector<StorageUserVolume> &vec_var, const string &where);
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
         UID = 0,   
         REQUEST_ID = 1,
         USAGE = 2,
         DESCRIPTION = 3,
         LIMIT = 4
     };

    StorageUserVolumePool(SqlDB *pDb):SqlCallbackable(pDb)
    {
    };
    static StorageUserVolumePool *_instance;
    static const char   *_table;
    static const char   *_col_names;
    DISALLOW_COPY_AND_ASSIGN(StorageUserVolumePool);
};

/**
@brief 功能描述: StorageSnapShotPool数据库操作类\n
@li @b 其它说明: 无
*/
class StorageSnapShotPool: public SqlCallbackable
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    static StorageSnapShotPool *GetInstance()
    {
        SkyAssert(NULL != _instance);
        return _instance;
    };
    
    static StorageSnapShotPool *GetInstance(SqlDB *database)
    {
        if(NULL == _instance)
        {
            _instance = new StorageSnapShotPool(database);
        }
        
        return _instance;
    };
     
    int Allocate(const StorageSnapshot  &var);
    int Drop(const StorageSnapshot  &var); 
    int Select(vector<StorageSnapshot> &vec_var, const string &where);
    int GetSnapNum(const StorageSnapshot  &var,int& num);
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
         SNAP_UUID = 1,         
         LIMIT = 2
     };

    StorageSnapShotPool(SqlDB *pDb):SqlCallbackable(pDb)
    {
    };
    static StorageSnapShotPool *_instance;
    static const char   *_table;
    static const char   *_col_names;
    DISALLOW_COPY_AND_ASSIGN(StorageSnapShotPool);
};



/**
@brief 功能描述: StorageVolumeWithLun数据库操作类\n
@li @b 其它说明: 无
*/
class StorageVolumeWithLunPool: public SqlCallbackable
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    static StorageVolumeWithLunPool *GetInstance()
    {
        SkyAssert(NULL != _instance);
        return _instance;
    };
    
    static StorageVolumeWithLunPool *GetInstance(SqlDB *database)
    {
        if(NULL == _instance)
        {
            _instance = new StorageVolumeWithLunPool(database);
        }
        
        return _instance;
    };
     
    int Select(vector<StorageVolumeWithLun> &vec_var, const string &where);
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
         SID         = 0,
         VG_NAME     = 1,
         VOLUME_NAME = 2,
         VOLUME_UUID = 3,
         TARGET_NAME = 4,
         LUN_ID      = 5,
         LUN_UUID    = 6,
         LIMIT       = 7
     };

    StorageVolumeWithLunPool(SqlDB *pDb):SqlCallbackable(pDb)
    {
    };
    static StorageVolumeWithLunPool *_instance;
    static const char   *_table;
    static const char   *_col_names;
    DISALLOW_COPY_AND_ASSIGN(StorageVolumeWithLunPool);
};

/**
@brief 功能描述: StorageAdaptorWithVgAndCluster数据库操作类\n
@li @b 其它说明: 无
*/
class StorageAdaptorWithVgAndClusterPool: public SqlCallbackable
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    static StorageAdaptorWithVgAndClusterPool *GetInstance()
    {
        SkyAssert(NULL != _instance);
        return _instance;
    };
    
    static StorageAdaptorWithVgAndClusterPool *GetInstance(SqlDB *database)
    {
        if(NULL == _instance)
        {
            _instance = new StorageAdaptorWithVgAndClusterPool(database);
        }
        
        return _instance;
    };
    
    int Select(vector<StorageAdaptorWithVgAndCluster> &vec_var, const string &where);
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
         SID          = 0, 
         CLUSTER_NAME = 1,
         APPLICATION  = 2,
         UNIT         = 3,
         TYPE         = 4,
         VG_NAME      = 5,
         SA_TOTAL_FREE_SIZE = 6,
         MAX_VG_FREE_SIZE = 7,
         LIMIT        = 8
     };

    StorageAdaptorWithVgAndClusterPool(SqlDB *pDb):SqlCallbackable(pDb)
    {
    };
    static StorageAdaptorWithVgAndClusterPool *_instance;
    static const char   *_table;
    static const char   *_col_names;
    DISALLOW_COPY_AND_ASSIGN(StorageAdaptorWithVgAndClusterPool);
};

/**
@brief 功能描述: StorageAdaptorWithMultivgAndClusterPool数据库操作类\n
@li @b 其它说明: 无
*/
class StorageAdaptorWithMultivgAndClusterPool: public SqlCallbackable
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    static StorageAdaptorWithMultivgAndClusterPool *GetInstance()
    {
        SkyAssert(NULL != _instance);
        return _instance;
    };
    
    static StorageAdaptorWithMultivgAndClusterPool *GetInstance(SqlDB *database)
    {
        if(NULL == _instance)
        {
            _instance = new StorageAdaptorWithMultivgAndClusterPool(database);
        }
        
        return _instance;
    };
    
    int Select(vector<StorageAdaptorWithMultivgAndCluster> &vec_var, const string &where);
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
         SID          = 0, 
         CLUSTER_NAME = 1,
         APPLICATION  = 2,
         UNIT         = 3,
         TYPE         = 4,
         VG_NAME      = 5,
         SA_TOTAL_FREE_SIZE = 6,
         VG_FREE_SIZE = 7,
         LIMIT        = 8
     };

    StorageAdaptorWithMultivgAndClusterPool(SqlDB *pDb):SqlCallbackable(pDb)
    {
    };
    static StorageAdaptorWithMultivgAndClusterPool *_instance;
    static const char   *_table;
    static const char   *_col_names;
    DISALLOW_COPY_AND_ASSIGN(StorageAdaptorWithMultivgAndClusterPool);
};


/**
@brief 功能描述: StorageVolumeWithLunAndAuthAndAdaptor数据库操作类\n
@li @b 其它说明: 无
*/
class StorageVolumeWithLunAndAuthAndAdaptorPool: public SqlCallbackable
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    static StorageVolumeWithLunAndAuthAndAdaptorPool *GetInstance()
    {
        SkyAssert(NULL != _instance);
        return _instance;
    };
    
    static StorageVolumeWithLunAndAuthAndAdaptorPool *GetInstance(SqlDB *database)
    {
        if(NULL == _instance)
        {
            _instance = new StorageVolumeWithLunAndAuthAndAdaptorPool(database);
        }
        
        return _instance;
    };
    
    int Select(StorageVolumeWithLunAndAuthAndAdaptor &var, const string &where);
    /* 继续添加数据库访问函数 */
     
    int SelectCallback(void *nil, SqlColumn * columns);   
    int GetLunHostMapNum(const string & target_name,const string &vol_uuid,const string &ip,const string & iscsi_initiator,int64 &num_lun,int64 &num_host);
    int GetTargetMapNum(const string & target_name,int64 &num);
    int GetLunMapNum(const string &vol_uuid,int64 &num);

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
         VOL_UUID         = 0,
         TARGET_NAME      = 1,
         LUN_ID           = 2,
         IP_ADDRESS       = 3,
         ISCSI_INITIATIOR = 4,
         APPLICATION      = 5,
         UNIT             = 6,
         TYPE             = 7,
         LIMIT            = 8
     };

    StorageVolumeWithLunAndAuthAndAdaptorPool(SqlDB *pDb):SqlCallbackable(pDb)
    {
    };
    static StorageVolumeWithLunAndAuthAndAdaptorPool *_instance;
    static const char   *_table;
    static const char   *_col_names;
    DISALLOW_COPY_AND_ASSIGN(StorageVolumeWithLunAndAuthAndAdaptorPool);
};

/**
@brief 功能描述: StorageVolumeWithLunAndAddress数据库操作类\n
@li @b 其它说明: 无
*/
class StorageVolumeWithLunAndAddressPool: public SqlCallbackable
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    static StorageVolumeWithLunAndAddressPool *GetInstance()
    {
        SkyAssert(NULL != _instance);
        return _instance;
    };
    
    static StorageVolumeWithLunAndAddressPool *GetInstance(SqlDB *database)
    {
        if(NULL == _instance)
        {
            _instance = new StorageVolumeWithLunAndAddressPool(database);
        }
        
        return _instance;
    };
    
    int Select(vector<StorageVolumeWithLunAndAddress> &vec_var, const string &where);
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
         VOL_UUID      = 0,
         LUN_ID        = 1,
         MEDIA_ADDRESS = 2,
         LIMIT         = 3
     };

    StorageVolumeWithLunAndAddressPool(SqlDB *pDb):SqlCallbackable(pDb)
    {
    };
    static StorageVolumeWithLunAndAddressPool *_instance;
    static const char   *_table;
    static const char   *_col_names;
    DISALLOW_COPY_AND_ASSIGN(StorageVolumeWithLunAndAddressPool);
};

/**
@brief 功能描述: StorageVolumeWithAdaptor数据库操作类\n
@li @b 其它说明: 无
*/
class StorageVolumeWithAdaptorPool: public SqlCallbackable
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    static StorageVolumeWithAdaptorPool *GetInstance()
    {
        SkyAssert(NULL != _instance);
        return _instance;
    };
    
    static StorageVolumeWithAdaptorPool *GetInstance(SqlDB *database)
    {
        if(NULL == _instance)
        {
            _instance = new StorageVolumeWithAdaptorPool(database);
        }
        
        return _instance;
    };

    int Select(vector<StorageVolumeWithAdaptor> &vec_var,const string &where);
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
         VOL_UUID      = 0,
         SID           = 1,
         VG_NAME       = 2,
         VOL_NAME      = 3,
         APPLICATION   = 4,
         UNIT          = 5,
         TYPE          = 6,
         LIMIT         = 7
     };

    StorageVolumeWithAdaptorPool(SqlDB *pDb):SqlCallbackable(pDb)
    {
    };
    static StorageVolumeWithAdaptorPool *_instance;
    static const char   *_table;
    static const char   *_col_names;
    DISALLOW_COPY_AND_ASSIGN(StorageVolumeWithAdaptorPool);
};

/**
@brief 功能描述: StorageAuthorizeWithTargetId数据库操作类\n
@li @b 其它说明: 无
*/
class StorageAuthorizeWithTargetIdPool: public SqlCallbackable
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    static StorageAuthorizeWithTargetIdPool *GetInstance()
    {
        SkyAssert(NULL != _instance);
        return _instance;
    };
    
    static StorageAuthorizeWithTargetIdPool *GetInstance(SqlDB *database)
    {
        if(NULL == _instance)
        {
            _instance = new StorageAuthorizeWithTargetIdPool(database);
        }
        
        return _instance;
    };

    int Select(vector<StorageAuthorizeWithTargetId> &vec_var,const string &where);
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
         TARGET_ID       = 0,
         SID             = 1,
         TARGET_NAME     = 2,
         IP_ADDRESS      = 3,
         ISCSI_INITIATOR = 4,
         AUTH_UUID       = 5,
         LIMIT           = 6

     };

    StorageAuthorizeWithTargetIdPool(SqlDB *pDb):SqlCallbackable(pDb)
    {
    };
    static StorageAuthorizeWithTargetIdPool *_instance;
    static const char   *_table;
    static const char   *_col_names;
    DISALLOW_COPY_AND_ASSIGN(StorageAuthorizeWithTargetIdPool);
};

/**
@brief 功能描述: StorageLunActionView数据库操作类\n
@li @b 其它说明: 无
*/
class StorageLunActionView: public SqlCallbackable
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:  
    static StorageLunActionView *GetInstance()
    {
        SkyAssert(NULL != _instance);
        return _instance;
    };
    
    static StorageLunActionView *GetInstance(SqlDB *database)
    {
        if(NULL == _instance)
        {
            _instance = new StorageLunActionView(database);
        }        

        return _instance;
    };
    
    int Select(list<int> &p_list, const string &where);
    /* 继续添加数据库访问函数 */
     
    int SelectCallback(void *nil, SqlColumn * columns);    

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
         SID         = 0,
         TARGET_NAME = 1,
         LUN_ID      = 2,      
         LIMIT       = 3
     };

    StorageLunActionView(SqlDB *pDb):SqlCallbackable(pDb)
    {
    };  
    static StorageLunActionView *_instance;
    static const char   *_table;
    static const char   *_col_names;
    DISALLOW_COPY_AND_ASSIGN(StorageLunActionView);
};

/**
@brief 功能描述: StorageClusterMaxSizeView数据库操作类\n
@li @b 其它说明: 无
*/
class StorageClusterMaxSizeView: public SqlCallbackable
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:  
    static StorageClusterMaxSizeView *GetInstance()
    {
        SkyAssert(NULL != _instance);
        return _instance;
    };
    
    static StorageClusterMaxSizeView *GetInstance(SqlDB *database)
    {
        if(NULL == _instance)
        {
            _instance = new StorageClusterMaxSizeView(database);
        }     

        return _instance;
    };
    
    int GetClusters(string & appoint_cluster,int64 size, vector<string> &cluster_list);


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
         NAME         = 0,
         SIZE         = 1,        
         LIMIT        = 3 
     };

    StorageClusterMaxSizeView(SqlDB *pDb):SqlCallbackable(pDb)
    {
    };  
    static StorageClusterMaxSizeView *_instance;
    static const char   *_table;
    static const char   *_col_names;
    DISALLOW_COPY_AND_ASSIGN(StorageClusterMaxSizeView);
};


/**
@brief 功能描述: ScDbOperationSet数据库操作接口封装类\n
@li @b 其它说明: 无
*/
class ScDbOperationSet: public SqlCallbackable
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    static ScDbOperationSet *GetInstance()
    {
        SkyAssert(NULL != _instance);
        return _instance;
    };

    static ScDbOperationSet *GetInstance(SqlDB *database)
    {
        if(NULL == _instance)
        {
            _instance = new ScDbOperationSet(database);
        }
        if ((NULL != _instance) && (SUCCESS != _instance->init(database)))
        {
            delete _instance;
            _instance = NULL;
        }
        return _instance;
    };

    ScDbOperationSet()
    {
        _adaptor_address = NULL;
        _adaptor = NULL;
        _address = NULL;
        _vg = NULL;
        _target = NULL;
        _authorize = NULL;
        _cluster = NULL;
        _volume = NULL;
        _lun = NULL;
        _authorize_lun = NULL;
        _user_volume = NULL;
        _snap_shot=NULL;
        _volume_with_lun = NULL;
        _adaptor_with_vg_and_cluster = NULL;
        _volume_with_lun_and_auth_and_adaptor = NULL;
        _volume_with_lun_and_address = NULL;
        _volume_with_adaptor = NULL;
        _authorize_with_target_id = NULL;
    }
    int init(SqlDB *pDb); 

    ScDbOperationSet(SqlDB *pDb):SqlCallbackable(pDb)
    {
    };

    StorageAdaptorAddressPool *_adaptor_address;
    StorageAdaptorPool *_adaptor;
    StorageAddressPool *_address;
    StorageVgPool *_vg;
    StorageTargetPool *_target;
    StorageAuthorizePool *_authorize;
    StorageClusterPool *_cluster;
    StorageVolumePool *_volume;
    StorageLunPool *_lun;
    StorageAuthorizeLunPool *_authorize_lun;
    StorageUserVolumePool *_user_volume;
    StorageSnapShotPool   *_snap_shot;
    StorageVolumeWithLunPool *_volume_with_lun;
    StorageAdaptorWithVgAndClusterPool *_adaptor_with_vg_and_cluster;
    StorageAdaptorWithMultivgAndClusterPool *_adaptor_with_multivg_and_cluster;
    StorageVolumeWithLunAndAuthAndAdaptorPool *_volume_with_lun_and_auth_and_adaptor;
    StorageVolumeWithLunAndAddressPool *_volume_with_lun_and_address;
    StorageVolumeWithAdaptorPool *_volume_with_adaptor;
    StorageAuthorizeWithTargetIdPool *_authorize_with_target_id;
    StorageLunActionView     *_action_lunid;
    StorageClusterMaxSizeView *_cluster_ms;
    static ScDbOperationSet *_instance;
    DISALLOW_COPY_AND_ASSIGN(ScDbOperationSet);
};
int DbAddVolumeToTarget(const StorageLun &sl);
int DbDelVolumeFromTarget(const StorageLun &sl);
int DbGetStorageAdaptorByMid(const MID &mid, StorageAdaptor &sa);
int DbAddStorageAdaptor(StorageAdaptor &sa, const StorageAdaptorAddress &saa, vector<StorageAddress> &vec_sadd, vector<StorageVg> &vec_sv);
int DbUpdateStorageAdaptor(const StorageAdaptor &sa, const StorageAdaptorAddress &saa, vector<StorageAddress> &vec_sadd, vector<StorageVg> &vec_sv);
int DbUpdateStorageAdaptor(const StorageAdaptor &sa);
int DbGetVolumeWithAdaptorInfoByUUID(const string &uuid, StorageVolumeWithAdaptor &siv);
int DbGetVolumeWithAdaptorInfoBySid(int64 sid, StorageVolumeWithAdaptor &siv);
int DbGetVolumeWithLunInfoBySid(int64 sid, vector<StorageVolumeWithLun> &vec_svwl, vector<StorageAuthorizeWithTargetId> &vec_sawt);
int DbGetMediaAddressBySid(int64 sid,vector<string> &media_address);
int DbGetClusterNameBySid(int64 sid,vector<string> &cluster_name);
int DbGetTargetIndexByKey(int64 sid, const string &name, int64 &index);
int DbGetFreeLunidByTarget(StorageLun &var);
int DbSelectStorageAdaptor(const string &clustername, int64 volsize, vector<StorageAdaptorWithMultivgAndCluster> & vec_csv);
int DbGetTargetInfo(const string &vol_uuid,StorageVolumeWithLunAndAuthAndAdaptor &svwlaa);
int DbGetTargetInfo(const string &vol_uuid,const string &ip,const string &iscsi_initiator, StorageVolumeWithLunAndAuthAndAdaptor &svwlaa); 
int DbGetMediaAddress(const string &vol_uuid,int &lunid, vector<string> &media_address_vec); 
int DbGetStorageVolumeIndex(int64 &index);
int DbGetStorageTargetIndex(int64 sid, int64 &index);
int DbGetStorageVolumeByRequestId(const string &request_id, StorageVolume &sv);
int DbGetStorageAdaptorBySid(int64 sid, StorageAdaptor &sa);
int DbDropStorageAdaptor(StorageAdaptor &sa);
int DbAddClusterMap(const StorageCluster &sc);
int DbDropClusterMap(const StorageCluster &sc);
int DbGetStorageLun(StorageLun &sl);
int DbGetStorageAuthorize(StorageAuthorize &sa);
int DbGetStorageLunByVolumeUuid(StorageLun &sl,const string &volume_uuid);
int DbUpdateLunAndAddAuthorizeLun(const StorageLun &sl,const StorageAuthorizeLun &sal);
int DbDelLunAndAuthorizeLun(const StorageLun &sl,const StorageAuthorizeLun &sal);
int DbDelLunAndAuthorizeLunAndAuthorize(const StorageLun &sl);
int DbDelAuthorizeLunAndAuthorize(const StorageAuthorize &sa);
int DbAddAuthorizeAndAuthorizeLun(const StorageAuthorize &sa, const vector<StorageAuthorizeLun> &vec_sal);
bool DbIsVolumeExist(const string &uuid);
bool DbIsStorageTargetExist(StorageTarget &st);
bool DbIsHostAuthorized(StorageAuthorize &sa);
bool DbIsStorageLunExist(StorageLun &sl);
bool DbIsStorageTargetExistLun(const StorageTarget &st);
bool DbIsStorageTargetExistHost(const StorageTarget &st);
bool DbIsVolumeInStorageTarget(const StorageVolume &sv);
bool DbIsAuthorizeExistLun(const StorageAuthorize &sa);
bool DbIsTargetInfoExist(const string &vol_uuid,const string &ip,const string &iscsi_initiator);
bool DbIsLastLunInAuthorize(StorageLun &sl, vector<StorageAuthorize> &vec_sa);
int DbGetActionLunid(list < int > & p_list,int64 sid, string & target_name);
int DbDelAuthorizeLun(const StorageAuthorizeLun &sal);
int DbAddAuthTbl(StorageTarget &st,  StorageAuthorize &sa, StorageLun &sl);
int DbGetLunHostMapNum(const string & target_name,const string &vol_uuid,const string &ip,const string &iscsi_initiator,int64 &map_lun,int64 &map_host);
int DbDelAuthTbl(StorageTarget &st,  StorageAuthorize &sa, StorageLun &sl,int type);
int DbAddVolumeAndUserVolume(const StorageVolume& sv, const StorageUserVolume& suv);
int DbDelVolumeAndUserVolume(const StorageVolume& sv, const StorageUserVolume& suv);
int DbUpdateUsageOfUserVolume(const string& request_id, const string& usage);
int DbUpdateUsageOfUserVolume(const string& request_id, const string& vol_uuid,const string& usage);
bool DbIsSaBusy(int64    sid);
int DbGetClustersBySize(string & appoint_cluster,int64 size, vector<string> &cluster_list);
int DbAddSnapshot(const StorageSnapshot & var);
int DbDropSnapshot(const StorageSnapshot & var);
int DbAddAuthorizeLun(const StorageAuthorizeLun &sal);
int  DbGetSaAddress(const string& app_name,string &ip);
int DbGetTargetMapNum(const string & target_name,int64 &num);
int DbScanTargetLunid();
int DbGetVolNumOfVg(int sid, const string& vgname,int& volnum);
int DbGetSnapNum(const StorageSnapshot  &var,int& num);
}  /* end namespace zte_tecs */

#endif /* end SC_DB_H */

