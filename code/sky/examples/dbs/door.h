/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�door.h
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��Door��Ķ����ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�Bob
* ������ڣ�2012��9��18��
*
* �޸ļ�¼1��
*    �޸����ڣ�2012/9/18
*    �� �� �ţ�V1.0
*    �� �� �ˣ�Bob
*    �޸����ݣ�����
*******************************************************************************/

#ifndef DOOR_H        
#define DOOR_H

#include "sky.h"

using namespace std;

namespace zte_tecs
{

/**
@brief ��������: Doorʵ����\n
@li @b ����˵��: ��
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


    int64    _id; //�����ID��ģ��ID
    string   _target;
    int32    _position ;
    int64    _image_id;
    string   _cfg_type;
};

}
#endif /* DOOR_H */

