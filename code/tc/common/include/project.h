/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�project.h
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��project��Ķ����ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�Bob
* ������ڣ�2011��7��26��
*
* �޸ļ�¼1��
*    �޸����ڣ�2011/7/26
*    �� �� �ţ�V1.0
*    �� �� �ˣ�Bob
*    �޸����ݣ�����
*******************************************************************************/
#ifndef PROJECT_MANGER_PROJECT_H        
#define PROJECT_MANGER_PROJECT_H

#include "sky.h"
#include "tecs_pub.h"

namespace zte_tecs
{


/**
@brief ��������: ���̹����ʵ����\n
@li @b ����˵��: ��
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
     *  У��һ������Ȩ���Ƿ�
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


