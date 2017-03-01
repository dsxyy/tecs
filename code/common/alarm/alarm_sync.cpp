///////////////////////////////////////////////////////////
//  AlarmSync.cpp
//  Implementation of the Class AlarmSync
//  Created on:      15-ÆßÔÂ-2011 12:17:54
//  Original author: mazhaomian
///////////////////////////////////////////////////////////

#include "alarm_sync.h"


AlarmSync::AlarmSync()
{
    wAlarmIDNum = 0;
    wAlarmMsgLen = 0;
    ucPhyOrLog = ALARM_COMPARE_PHYSICAL;
    ucFrameCount = 1; 
    ucFrameNo = 1;
}


AlarmSync::~AlarmSync()
{

}

