///////////////////////////////////////////////////////////
//  AlarmReport.h
//  Implementation of the Class AlarmReport
//  Created on:      15-����-2011 12:17:38
//  Original author: mazhaomian
///////////////////////////////////////////////////////////
#ifndef ALARM_AGENT_REPORT_H
#define ALARM_AGENT_REPORT_H
#include "oam_pub_type.h"
#include "oam_alarm_pub.h"
#include "alarm_report.h"
#include "sky.h"

class AgentAlarm : public Serializable
{
    public:
	AgentAlarm();

	virtual  ~AgentAlarm();	
	uint32   dwAlarmCode;
	uint16   wObjType;
	uint16   wKeyLen;
	uint16   wFilterFlag;
	Datetime tAlarmTime;
	Datetime tRestoreTime;
	string   tAlarmAddr;
    AlarmLocation alarm_location;
	uint32   dwCRCCode[4];
	uint16   wSendNum;  //�澯�����ڼ䶶������
	uint16   wIsLogAlm; //�Ƿ�Ϊ�߼��澯��0-����澯 1-�߼��澯
	uint16   wAlarmFlag; //1:�澯  2:�ָ�
	uint32   dwAlarmID;  //�澯��ˮ��
	uint16   wRestoreType; //�ָ�����
	string   aucMaxAddInfo;
	
	SERIALIZE
        {
            SERIALIZE_BEGIN(AgentAlarm);  //���л���ʼ
            WRITE_DIGIT(dwAlarmCode);
            WRITE_DIGIT(wObjType);
            WRITE_DIGIT(wKeyLen);
            WRITE_DIGIT(wFilterFlag);
            WRITE_DIGIT(dwCRCCode[0]);
            WRITE_DIGIT(dwCRCCode[1]);
            WRITE_DIGIT(dwCRCCode[2]);
            WRITE_DIGIT(dwCRCCode[3]);
            WRITE_DIGIT(wSendNum);
            WRITE_DIGIT(wIsLogAlm);
            WRITE_DIGIT(wAlarmFlag);
            WRITE_DIGIT(dwAlarmID);
            WRITE_DIGIT(wRestoreType);
            WRITE_STRING(tAlarmAddr);
            WRITE_OBJECT(tAlarmTime);
            WRITE_OBJECT(tRestoreTime);
            WRITE_OBJECT(alarm_location);
            WRITE_STRING(aucMaxAddInfo);
            SERIALIZE_END();
        }

        DESERIALIZE
        {
            DESERIALIZE_BEGIN(AgentAlarm);
            READ_DIGIT(dwAlarmCode);
            READ_DIGIT(wObjType);
            READ_DIGIT(wKeyLen);
            READ_DIGIT(wFilterFlag);
            READ_DIGIT(dwCRCCode[0]);
            READ_DIGIT(dwCRCCode[1]);
            READ_DIGIT(dwCRCCode[2]);
            READ_DIGIT(dwCRCCode[3]);
            READ_DIGIT(wSendNum);
            READ_DIGIT(wIsLogAlm);
            READ_DIGIT(wAlarmFlag);
            READ_DIGIT(dwAlarmID);
            READ_DIGIT(wRestoreType);
            READ_STRING(tAlarmAddr);
            READ_OBJECT(tAlarmTime);
            READ_OBJECT(tRestoreTime);
            READ_OBJECT(alarm_location);
            READ_STRING(aucMaxAddInfo);
            DESERIALIZE_END();
        }
	
};

class AlarmAgentReport : public Serializable
{

public:
	AlarmAgentReport();

	virtual ~AlarmAgentReport();	
	int64         iAlarmNum;
	AgentAlarm    atAlarmInfo[ALARM_PER_PACKET_MAX];
	
	SERIALIZE
        {
            SERIALIZE_BEGIN(AlarmAgentReport);  //���л���ʼ
            WRITE_DIGIT(iAlarmNum);
            for(int i=0;i<ALARM_PER_PACKET_MAX;i++)
                WRITE_OBJECT(atAlarmInfo[i]);
            SERIALIZE_END();
        }

        DESERIALIZE
        {
            DESERIALIZE_BEGIN(AlarmAgentReport);
            READ_DIGIT(iAlarmNum);
            for(int i=0;i<ALARM_PER_PACKET_MAX;i++)
                READ_OBJECT(atAlarmInfo[i]);
            DESERIALIZE_END();
        }

private:
	

};
#endif
