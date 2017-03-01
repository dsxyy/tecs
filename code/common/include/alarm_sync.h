///////////////////////////////////////////////////////////
//  AlarmSync.h
//  Implementation of the Class AlarmSync
//  Created on:      15-七月-2011 12:17:53
//  Original author: mazhaomian
///////////////////////////////////////////////////////////

#ifndef ALARM_SYNC_H
#define ALARM_SYNC_H

#include "oam_pub_type.h"
#include "oam_alarm_pub.h"
#include "sky.h"

class AlarmSync : public Serializable
{

public:
	AlarmSync();
	virtual ~AlarmSync();
	
	string tAlarmAddr; //测试单板告警位置
	WORD16 wAlarmIDNum;//包中告警总个数
	WORD16 wAlarmMsgLen;//消息长度
	BYTE   ucPhyOrLog;//逻辑告警池还是物理告警池
	BYTE   ucFrameCount;//同步的包总数
	BYTE   ucFrameNo;//当前的包号
	vector<WORD32>   dwData; //数据流
	
	SERIALIZE
        {
            SERIALIZE_BEGIN(AlarmSync);  //序列化开始
            WRITE_STRING(tAlarmAddr);
            WRITE_DIGIT(wAlarmIDNum);
            WRITE_DIGIT(wAlarmMsgLen);
            WRITE_DIGIT(ucPhyOrLog);
            WRITE_DIGIT(ucFrameCount);
            WRITE_DIGIT(ucFrameNo);
            WRITE_DIGIT_VECTOR(dwData);
            SERIALIZE_END();
        }
	
	DESERIALIZE
        {
            DESERIALIZE_BEGIN(AlarmSync);  //序列化开始
            READ_STRING(tAlarmAddr);
            READ_DIGIT(wAlarmIDNum);
            READ_DIGIT(wAlarmMsgLen);
            READ_DIGIT(ucPhyOrLog);
            READ_DIGIT(ucFrameCount);
            READ_DIGIT(ucFrameNo);
            READ_DIGIT_VECTOR(dwData);
            DESERIALIZE_END();
        }
	

private:

};
#endif 
