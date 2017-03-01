/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�affinity_region_pool.h
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��affinity_region��Ķ����ļ�
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

#ifndef AFFINITY_REGION_POOL_H
#define AFFINITY_REGION_POOL_H

#include "affinity_region.h"
#include "pool_sql.h"
#include "affinity_region_api.h"


namespace zte_tecs
{

/**
@brief ��������: �׺�����Դ�ص�ʵ����\n
@li @b ����˵��: ��
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

