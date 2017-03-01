///////////////////////////////////////////////////////////
//  AlarmReport.cpp
//  Implementation of the Class AlarmReport
//  Created on:      15-七月-2011 12:17:38
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
	wSendNum = 0;  //告警防抖期间抖动次数
	wIsLogAlm = 0; //是否为逻辑告警，0-物理告警 1-逻辑告警
	wAlarmFlag = 0;//1:告警  2:恢复
	dwAlarmID = 0;  //告警流水号
	wRestoreType = 0; //恢复类型
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
