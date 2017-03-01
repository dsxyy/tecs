/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：affinity_region_operate.h
* 文件标识：
* 内容摘要：AffinityRegionOperate类的定义文件
* 当前版本：1.0
* 作    者：Bob
* 完成日期：2011年7月26日
*
* 修改记录1：
*    修改日期：2013/8/26
*    版 本 号：V1.0
*    修 改 人：
*    修改内容：创建
*******************************************************************************/
#ifndef AFFINITY_REGION_OPERATE_H
#define AFFINITY_REGION_OPERATE_H
#include "sky.h"
#include "mid.h"
#include "event.h"
#include "affinity_region_pool.h"
#include "affinity_region_api.h"


namespace zte_tecs
{


class AffinityRegionOperate
{
public:

    static AffinityRegionOperate *GetInstance( )
    {
        SkyAssert(NULL != _op_inst);
        SkyAssert(NULL != _ppool);

        return _op_inst;
    };

    static AffinityRegionOperate *CreateInstance()
    {
        if(NULL == _op_inst)
        {
            _op_inst = new AffinityRegionOperate();
            _ppool   = AffinityRegionPool::GetInstance();
            SkyAssert(NULL != _ppool);
        }

        return _op_inst;
    };

    ~AffinityRegionOperate(){};
    STATUS DealModify(const ApiAffinityRegionModifyData  &req,
                      int64          uid,
                      ostringstream &err_oss);

    STATUS DealDelete(const ApiAffinityRegionDelete &req,
                                      int64  uid,
                                      ostringstream &err_oss);

    STATUS DealQuery(const ApiAffinityRegionQuery  &req,
                        int64                    uid,
                        int64                    appointed_uid,
                        vector<ApiAffinityRegionInfo>  &vec_ar,
                        int64                   &max_count,
                        int64                   &next_index,
                        ostringstream           &err_oss);

    bool Authorize(int64 opt_uid, AffinityRegion *buf, int oprate);

    STATUS Str2QueryType(const string   &query_name,
                          QUERY_TYPE    &type,
                          ostringstream  &err_oss);

private:

    static  AffinityRegionOperate * _op_inst;
    static  AffinityRegionPool  * _ppool;

    AffinityRegionOperate( ) { };

    DISALLOW_COPY_AND_ASSIGN(AffinityRegionOperate);

};



}

#endif /* AFFINITY_REGION_OPERATE_H */