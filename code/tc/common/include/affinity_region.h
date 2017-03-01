/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：affinity_region.h
* 文件标识：见配置管理计划书
* 内容摘要：affinity_region类的定义文件
* 当前版本：1.0
* 作    者：Bob
* 完成日期：2013年8月26日
*
* 修改记录1：
*    修改日期：2013/8/26
*    版 本 号：V1.0
*    修 改 人：Bob
*    修改内容：创建
*******************************************************************************/
#ifndef AFFINITY_REGION_H
#define AFFINITY_REGION_H

#include "sky.h"
#include "tecs_pub.h"
#include "api_const.h"

namespace zte_tecs
{


/**
@brief 功能描述: 亲和域管理的实体类\n
@li @b 其它说明: 无
*/
class AffinityRegion : public Serializable
{

/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    AffinityRegion(int64 uid,bool is_forced,const string &name= "", const string &des= "", const string &level= "shelf")
    {
        _oid = INVALID_OID;
        _name= name;
        _description = des;
        _create_time.refresh();
        _uid = uid ;
        _level = level ;
        _is_forced = is_forced;
        return;

    }

    AffinityRegion()
    {
        _oid = INVALID_OID;
        _create_time.refresh();
        _uid = INVALID_OID ;
        _level = AFFINITY_REGION_SHELF;
        _is_forced = 0;
        return;

    }
    ~AffinityRegion(){};

    int64 get_oid()const
    {
        return _oid;
    }

    int64 get_uid()const
    {
        return _uid;
    }

    const string& get_name()const
    {
        return _name;
    }

    const string& get_description()const
    {
        return _description;
    }

    void set_description(const string& new_des)
    {
         _description = new_des;
    };

    void set_level(const string& new_level)
    {
         _level = new_level;
    };


    SERIALIZE
    {
        SERIALIZE_BEGIN(AffinityRegion);
        WRITE_VALUE(_oid);
        WRITE_VALUE(_uid);
        WRITE_VALUE(_name);
        WRITE_VALUE(_description);
        WRITE_OBJECT(_create_time);
        WRITE_VALUE(_level);
        WRITE_VALUE(_is_forced);

        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(AffinityRegion);
        READ_VALUE(_oid);
        READ_VALUE(_uid);
        READ_VALUE(_name);
        READ_VALUE(_description);
        READ_OBJECT(_create_time);
        READ_VALUE(_level);
        READ_VALUE(_is_forced);

        DESERIALIZE_END();
    };


    int64  _oid;
    int64  _uid;
    string _name;
    string _description;
    Datetime _create_time;
    string _level;
    bool  _is_forced;
     /**
     *  校验一个操作权限是否够
     */
    int Authorize(int64 opt_uid, int oprate);


/*******************************************************************************
* 2. protected section
*******************************************************************************/
protected:


/*******************************************************************************
* 3. private section
*******************************************************************************/
private:


   //DISALLOW_COPY_AND_ASSIGN(AffinityRegion);

};
class VmAffregAllocateAck : public Serializable
{
public:    
    SERIALIZE
    {
        SERIALIZE_BEGIN(VmAffregAllocateAck);
        WRITE_VALUE(state);
        WRITE_VALUE(arid);
        WRITE_VALUE(detail);
        SERIALIZE_END();
    };
    
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(VmAffregAllocateAck); 
        READ_VALUE(state);
        READ_VALUE(arid);
        READ_VALUE(detail); 
        DESERIALIZE_END();
    };   
    int state;
    int64 arid,;
    string  detail;
};

}
#endif


