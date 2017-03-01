///////////////////////////////////////////////////////////
//  AlarmSync.h
//  Implementation of the Class AlarmSync
//  Created on:      15-����-2011 12:17:53
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
	
	string tAlarmAddr; //���Ե���澯λ��
	WORD16 wAlarmIDNum;//���и澯�ܸ���
	WORD16 wAlarmMsgLen;//��Ϣ����
	BYTE   ucPhyOrLog;//�߼��澯�ػ�������澯��
	BYTE   ucFrameCount;//ͬ���İ�����
	BYTE   ucFrameNo;//��ǰ�İ���
	vector<WORD32>   dwData; //������
	
	SERIALIZE
        {
            SERIALIZE_BEGIN(AlarmSync);  //���л���ʼ
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
            DESERIALIZE_BEGIN(AlarmSync);  //���л���ʼ
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
