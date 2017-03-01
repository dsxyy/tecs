///////////////////////////////////////////////////////////
//  AlarmReport.h
//  Implementation of the Class AlarmReport
//  Created on:      15-七月-2011 12:17:38
//  Original author: mazhaomian
///////////////////////////////////////////////////////////
#ifndef ALARM_REPORT_H
#define ALARM_REPORT_H
#include "oam_pub_type.h"
#include "oam_alarm_pub.h"
#include "sky.h"

typedef map<string,string> Dict;

class AlarmLocation : public Serializable
{

public:
    AlarmLocation()
    {
      
    };
	virtual ~AlarmLocation(){};
    
    string strLevel1;
    string strLevel2;
    string strLevel3;
    string strLocation;
    string strSubSystem;
    SERIALIZE
    {
        SERIALIZE_BEGIN(AlarmLocation);  //序列化开始
        WRITE_STRING(strLevel1);
        WRITE_STRING(strLevel2);
        WRITE_STRING(strLevel3);
        WRITE_STRING(strLocation);
        WRITE_STRING(strSubSystem);
        SERIALIZE_END();
    }

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(AlarmLocation);
        READ_STRING(strLevel1);
        READ_STRING(strLevel2);
        READ_STRING(strLevel3);
        READ_STRING(strLocation);
        READ_STRING(strSubSystem);
        DESERIALIZE_END();
    }
    
};

class AlarmReport : public Serializable
{

public:
	AlarmReport();

	virtual ~AlarmReport();	
	WORD32  dwAlarmCode;
	BYTE    ucAlarmFlag;
	WORD16  wKeyLen;
	WORD16  wObjType;
	string  alarm_address;
    AlarmLocation alarm_location;
	string  aucMaxAddInfo;
	SERIALIZE
    {
        SERIALIZE_BEGIN(AlarmReport);  //序列化开始
        WRITE_DIGIT(dwAlarmCode);
        WRITE_DIGIT(ucAlarmFlag);
        WRITE_DIGIT(wKeyLen);
        WRITE_DIGIT(wObjType);
        WRITE_STRING(alarm_address);
        WRITE_OBJECT(alarm_location);
        WRITE_STRING(aucMaxAddInfo);
        SERIALIZE_END();
    }

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(AlarmReport);
        READ_DIGIT(dwAlarmCode);
        READ_DIGIT(ucAlarmFlag);
        READ_DIGIT(wKeyLen);
        READ_DIGIT(wObjType);
        READ_STRING(alarm_address);
        READ_OBJECT(alarm_location);
        READ_STRING(aucMaxAddInfo);
        DESERIALIZE_END();
    }

private:
	

};
#endif
