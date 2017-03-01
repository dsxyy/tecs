/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：door.h
* 文件标识：见配置管理计划书
* 内容摘要：Door类的定义文件
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

#ifndef DOOR_H        
#define DOOR_H

#include "sky.h"

using namespace std;

namespace zte_tecs
{

/**
@brief 功能描述: Door实体类\n
@li @b 其它说明: 无
*/
class Door : public Serializable
{
public:
    Door()
    {
      _id       = -1;
      _image_id = -1;
      _position = 0;
    };
    
    Door(int64  id, 
                       const string& target, 
                       int32 position, 
                       int64 image_id,
                       const string& cfg_type)   
    {
        _id     = id;
        _target = target;
        _position = position;
        _image_id = image_id;
        _cfg_type = cfg_type;
    };

	void DoSomeThing(){};
	
    SERIALIZE
    {
        SERIALIZE_BEGIN(Door);
        WRITE_VALUE(_id);
        WRITE_VALUE(_target);
        WRITE_VALUE(_position);
        WRITE_VALUE(_image_id);
        WRITE_VALUE(_cfg_type);        

        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(Door);
        READ_VALUE(_id);
        READ_VALUE(_target);
        READ_VALUE(_position);
        READ_VALUE(_image_id);
        READ_VALUE(_cfg_type);        

        DESERIALIZE_END();
    };


    int64    _id; //虚拟机ID或模板ID
    string   _target;
    int32    _position ;
    int64    _image_id;
    string   _cfg_type;
};

}
#endif /* DOOR_H */

