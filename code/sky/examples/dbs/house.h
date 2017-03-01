/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：house.h
* 文件标识：见配置管理计划书
* 内容摘要：House类的定义文件
* 当前版本：1.0
* 作    者：Bob
* 完成日期：2012年9月18日
*
* 修改记录1：
*    修改日期：2012/9/18
*    版 本 号：V1.0
*    修 改 人：Bob
*    修改内容：创建
*******************************************************************************/

#ifndef HOUSE_H        
#define HOUSE_H

#include "sky.h"
#include "door.h"

using namespace std;

namespace zte_tecs
{

/**
@brief 功能描述: House实体类\n
@li @b 其它说明: 无
*/
class House : public Serializable
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    House()
    {
        _oid   = -1;
    };
    
    House(int64  id, const string& name, int64 uid)   
    {
        _oid  = id;
        _name = name;
        _uid = uid;
    };
	
    void DoSomeThing(){};
	
    SERIALIZE
    {
        SERIALIZE_BEGIN(House);
        WRITE_VALUE(_oid);
        WRITE_VALUE(_name);
        WRITE_VALUE(_uid);
        WRITE_OBJECT_VECTOR(_doors); 
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(House);
        READ_VALUE(_oid);
        READ_VALUE(_name);
        READ_VALUE(_uid); 
        READ_OBJECT_VECTOR(_doors); 
        DESERIALIZE_END();
    };

    int64    _oid; 
    string   _name;
    int64    _uid;
    vector<Door> _doors;
};
 
}
#endif /* House_H */

