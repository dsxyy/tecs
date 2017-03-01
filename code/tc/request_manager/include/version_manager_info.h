/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�version_manager_methods.h
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ���汾����XMl���ýӿ�����
* ��ǰ�汾��1.0
* ��    �ߣ����ΰ
* ������ڣ�2011��10��13��
*
* �޸ļ�¼1��
*     �޸����ڣ�2011/10/13
*     �� �� �ţ�V1.0
*     �� �� �ˣ����ΰ
*     �޸����ݣ�����
******************************************************************************/
#ifndef TC_VERSION_MANAGER_INFO_H
#define TC_VERSION_MANAGER_INFO_H

#include "sky.h"
#include "mid.h"
#include "event.h"

namespace zte_tecs
{

/*****************************************************************************/
/* �汾��Ϣ�� */
/*****************************************************************************/
class VersionInfo : public Serializable
{
public:
    VersionInfo() {};
    ~VersionInfo() {};
    
    SERIALIZE 
    {
        SERIALIZE_BEGIN(VersionInfo);
        WRITE_STRING(_destination);
        WRITE_DIGIT(_grade);
        WRITE_STRING(_filename);
        WRITE_DIGIT(_flag);
        WRITE_STRING(_version);
         SERIALIZE_END();    
    };
    
    DESERIALIZE 
    {
        DESERIALIZE_BEGIN(VersionInfo);
        READ_STRING(_destination);
        READ_DIGIT(_grade);
        READ_STRING(_filename);
        READ_DIGIT(_flag);
        READ_STRING(_version);
        DESERIALIZE_END();    
    };
	//Ŀ�ı�ʾ����ʾ�汾���մ�Ż����е���������application��������
	string _destination;
       int _grade;
	//�����ļ�����
	string _filename;
	//����load������Ƿ�ִ�а�װ����,0��ʾ�����а汾��װ��1��ʾ���а汾��װ
	int _flag;       // �汾���ر��
       string _version;                 // �汾��
};

/*****************************************************************************/
/* �汾��Ϣ�� */
/*****************************************************************************/
class VersionInfoReq : public Serializable
{
public:
    VersionInfoReq() {};
    VersionInfoReq(const string& destination,int32 grade,const string& filename,const string& host,const string& user,const string& password)
	{
           _destination = destination;
           _grade = grade;
           _filename = filename;
           _host_address = host;
           _username = user;
           _password = password;
	};	
    ~VersionInfoReq() {};
    
    SERIALIZE 
    {
        SERIALIZE_BEGIN(VersionInfo);
        WRITE_STRING(_destination);
        WRITE_DIGIT(_grade);
        WRITE_STRING(_filename);
        WRITE_STRING(_host_address);
        WRITE_STRING(_username);
        WRITE_STRING(_password);		
         SERIALIZE_END();    
    };
    
    DESERIALIZE 
    {
        DESERIALIZE_BEGIN(VersionInfo);
        READ_STRING(_destination);
        READ_DIGIT(_grade);
        READ_STRING(_filename);
        READ_STRING(_host_address);
        READ_STRING(_username);		
        READ_STRING(_password);
        DESERIALIZE_END();    
    };
	//Ŀ�ı�ʾ����ʾ�汾���մ�Ż����е���������application��������
	string _destination;
       int32 _grade;
	//�����ļ�����
	string _filename;
       string   _host_address;
       string   _username;
       string  _password;       
};

/*****************************************************************************/
//�������û����ز�ѯ���İ汾��Ϣ
/*****************************************************************************/
class VersionInfoAck : public Serializable
{
public:
    VersionInfoAck(){};	
    VersionInfoAck(int32 num) 
    {
        _num = num;
    }

    ~VersionInfoAck() {};
    
    SERIALIZE 
    {
        SERIALIZE_BEGIN(VersionInfoAck);
        WRITE_DIGIT(_result);		
        WRITE_DIGIT(_num);
        WRITE_OBJECT_VECTOR(_version_resource);
        SERIALIZE_END();    
    };
    
    DESERIALIZE 
    {
        DESERIALIZE_BEGIN(VersionInfoAck);
        READ_DIGIT(_result);		
        READ_DIGIT(_num);
        READ_OBJECT_VECTOR(_version_resource);
        DESERIALIZE_END();    
    };
    int _result;    
    int  _num;
    vector <VersionInfo> _version_resource;

};

} //end namespace zte_tecs

#endif //end TC_VERSION_MANAGER_INFO_H
