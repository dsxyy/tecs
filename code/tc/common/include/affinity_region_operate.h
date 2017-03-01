/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�affinity_region_operate.h
* �ļ���ʶ��
* ����ժҪ��AffinityRegionOperate��Ķ����ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�Bob
* ������ڣ�2011��7��26��
*
* �޸ļ�¼1��
*    �޸����ڣ�2013/8/26
*    �� �� �ţ�V1.0
*    �� �� �ˣ�
*    �޸����ݣ�����
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