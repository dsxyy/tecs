///////////////////////////////////////////////////////////
//  AlarmReport.cpp
//  Implementation of the Class AlarmReport
//  Created on:      15-����-2011 12:17:38
//  Original author: mazhaomian
///////////////////////////////////////////////////////////

#include "alarm_agent_report.h"

AgentAlarm::AgentAlarm()
{
    dwAlarmCode = 0;
	wObjType = 0;
	wKeyLen = 0;
	wFilterFlag = 0;
	dwCRCCode[0] = 0;
    dwCRCCode[1] = 0;
    dwCRCCode[2] = 0;
    dwCRCCode[3] = 0;
	wSendNum = 0;  //�澯�����ڼ䶶������
	wIsLogAlm = 0; //�Ƿ�Ϊ�߼��澯��0-����澯 1-�߼��澯
	wAlarmFlag = 0;//1:�澯  2:�ָ�
	dwAlarmID = 0;  //�澯��ˮ��
	wRestoreType = 0; //�ָ�����
}


AgentAlarm::~AgentAlarm()
{

}


AlarmAgentReport::AlarmAgentReport()
{
    
}


AlarmAgentReport::~AlarmAgentReport()
{

}
