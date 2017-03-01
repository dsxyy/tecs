/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：project.h
* 文件标识：见配置管理计划书
* 内容摘要：project类的定义文件
* 当前版本：1.0
* 作    者：Bob
* 完成日期：2011年7月26日
*
* 修改记录1：
*    修改日期：2011/7/26
*    版 本 号：V1.0
*    修 改 人：Bob
*    修改内容：创建
*******************************************************************************/
#ifndef PROJECT_MANGER_PROJECT_H        
#define PROJECT_MANGER_PROJECT_H

#include "sky.h"
#include "tecs_pub.h"

namespace zte_tecs
{


/**
@brief 功能描述: 工程管理的实体类\n
@li @b 其它说明: 无
*/
class Project : public Serializable
{

/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    Project(int64 uid,const string &name= "", const string &des= "")
    {
        _oid = INVALID_OID;
        _name= name;
        _description = des;
        _create_time.refresh();
        _uid = uid ;
        return;

    }
    
    Project()
    {
        _oid = INVALID_OID;
        _create_time.refresh();
        _uid = INVALID_OID ;
        return;

    }
    ~Project(){};
    
    int64 get_oid()const
    {
        return _oid;
    }

    int64 get_uid()const
    {
        return _uid;
    }

    string get_name()const
    {
        return _name;
    }

    string get_description()const
    {
        return _description;
    }
    
    void set_description(const string& new_des) 
    {
         _description = new_des;
    };

    
    SERIALIZE
    {
        SERIALIZE_BEGIN(Project);
        WRITE_VALUE(_oid);
        WRITE_VALUE(_uid);
        WRITE_VALUE(_name);
        WRITE_VALUE(_description);
        WRITE_OBJECT(_create_time);
        
        SERIALIZE_END();
    };
    
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(Project);
        READ_VALUE(_oid);
        READ_VALUE(_uid);
        READ_VALUE(_name);
        READ_VALUE(_description);
        READ_OBJECT(_create_time);
        
        DESERIALIZE_END();
    };


    int64  _oid;
    int64  _uid;
    string _name;
    string _description;
    Datetime _create_time;

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
    

   //DISALLOW_COPY_AND_ASSIGN(Project); 

};

}
#endif


