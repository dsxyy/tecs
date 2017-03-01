/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�house.h
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��House��Ķ����ļ�
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

#ifndef HOUSE_H        
#define HOUSE_H

#include "sky.h"
#include "door.h"

using namespace std;

namespace zte_tecs
{

/**
@brief ��������: Houseʵ����\n
@li @b ����˵��: ��
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

