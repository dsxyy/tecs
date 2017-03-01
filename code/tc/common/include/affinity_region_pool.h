/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：affinity_region_pool.h
* 文件标识：见配置管理计划书
* 内容摘要：affinity_region类的定义文件
* 当前版本：1.0
* 作    者：
* 完成日期：2013年8月26日
*
* 修改记录1：
*    修改日期：2013/8/26
*    版 本 号：V1.0
*    修 改 人：Bob
*    修改内容：创建
*******************************************************************************/

#ifndef AFFINITY_REGION_POOL_H
#define AFFINITY_REGION_POOL_H

#include "affinity_region.h"
#include "pool_sql.h"
#include "affinity_region_api.h"


namespace zte_tecs
{

/**
@brief 功能描述: 亲和域资源池的实体类\n
@li @b 其它说明: 无
*/
class AffinityRegionPool : public SqlCallbackable
{

/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    static AffinityRegionPool* GetInstance()
    {
        SkyAssert(NULL != _instance);
        return _instance;
    };

    static AffinityRegionPool* CreateInstance(SqlDB *db)
    {
        if(NULL == _instance)
        {
            _instance = new AffinityRegionPool(db);
        }

        return _instance;
    };

    static void Lock()
    {
        pthread_mutex_lock(&_mutex);
    };

    static void UnLock()
    {
        pthread_mutex_unlock(&_mutex);
    };

    STATUS Init(const bool &isReadOnly);

     ~AffinityRegionPool(){};

    STATUS   Allocate (AffinityRegion &affinity_region);

    STATUS GetAffinityRegion(int64 oid, AffinityRegion &affinity_region);

    int SelectCallback(void * nil, SqlColumn * columns);

    int Update(const AffinityRegion &ar);

    int Drop(const AffinityRegion &ar);

    int Show(vector<ApiAffinityRegionInfo> &vec_ar_info, const string& where);

    int64 Count(const string &where);

    bool Authorize(int64 opt_uid, AffinityRegion *buf, int oprate);



/*******************************************************************************
* 2. protected section
*******************************************************************************/
protected:
    enum ColNames
    {
        OID              = 0,    /* AffinityRegion identifier (IID)      */
        NAME             = 1,    /* AffinityRegion name                  */
        DESCRIPTION      = 2,    
        CREATE_TIME      = 3,
        UID              = 4,    /* AffinityRegion owner id              */
        LEVEL            = 5,    /* AffinityRegion level              */
        IS_FORCED        = 6,
        LIMIT            = 7
    };

    enum ViewColNames
    {
        VIEW_OID              = 0,    /* AffinityRegion identifier (IID)      */
        VIEW_NAME             = 1,    /* AffinityRegion name                  */
        VIEW_DESCRIPTION      = 2,    
        VIEW_USER_NAME        = 3,
        VIEW_LEVEL            = 4,
        VIEW_FORCED           = 5,
        VIEW_UID              = 6,
        VIEW_LIMIT            = 7
    };

    int ShowCallback(void * arg, SqlColumn * columns);

/*******************************************************************************
* 3. private section
*******************************************************************************/
private:
    static AffinityRegionPool *_instance;
    static int64            _lastOID;
    static pthread_mutex_t  _mutex;

    static const char *     _table_affinity_region;
    static const char *     _col_names_affinity_region;
    static const char *     _view_affinity_region;
    static const char *     _col_names_view_affinity_region;

    AffinityRegionPool(SqlDB *pDb):SqlCallbackable(pDb){ };

    DISALLOW_COPY_AND_ASSIGN(AffinityRegionPool);

};

}
#endif /* AFFINITY_REGION_POOL_H */

