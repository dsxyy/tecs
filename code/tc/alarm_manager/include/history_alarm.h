/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�history_alarm.h
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��HistoryAlarm�ඨ���ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�������
* ������ڣ�2011��7��27��
*
* �޸ļ�¼1��
*     �޸����ڣ�2011/7/27
*     �� �� �ţ�V1.0
*     �� �� �ˣ�������
*     �޸����ݣ�����
*******************************************************************************/


#ifndef ALARM_HISTORY_ALARM_H
#define ALARM_HISTORY_ALARM_H

#include "sky.h"
#include "alarm_api.h"
#include "tecs_pub.h"

//#include <sstream>
//#include <ctime>

// ���ÿ�������궨��
#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
            TypeName(const TypeName&); \
            void operator=(const TypeName&)
#endif

using namespace std;


namespace zte_tecs
{

class HistoryAlarm : public Serializable
{
    friend class HistoryAlarmPool;

public:

    //����Ψһ��ʶ
    int64  _oid;


    int        AlarmID;         // �澯��ˮ��
    int        AlarmCode;       // �澯��
    int        AlarmTime;       // �澯ʱ��
    string     AlarmAddr;       // �澯λ��
    string     SubSystem;
    string     Level1;          
    string     Level2;          
    string     Level3;          
    string     Location;        // �澯λ��������Ϣ    
    string     AddInfo;         // �澯������Ϣ
    int        RestoreTime;     // �ָ�ʱ��
    int        RestoreType;     // �澯�ָ�����


   // Ĭ�Ϲ��캯��
    HistoryAlarm()
    {
        _oid = INVALID_OID;
        AlarmID = 0;
        AlarmCode = 0;
        AlarmTime = 0;
        RestoreTime = 0;
        RestoreType = 0;
    };

    HistoryAlarm(int aAlarmID,
                    int aAlarmCode,
                    int aAlarmTime,
                    const string&  aAlarmAddr,
                    const string&  aSubSystem,
                    const string&  aLevel1,
                    const string&  aLevel2,
                    const string&  aLevel3,
                    const string&  aLocation,
                    const string&  aAddInfo,
                    int         aRestoreTime,
                    int  aRestoreType)
    {
        AlarmID = aAlarmID;
        AlarmCode = aAlarmCode;
        AlarmTime = aAlarmTime;
        AlarmAddr = aAlarmAddr;
        SubSystem = aSubSystem;
        Level1 = aLevel1;
        Level2 = aLevel2;
        Level3 = aLevel3;
        Location = aLocation;
        AddInfo = aAddInfo;
        RestoreTime = aRestoreTime;
        RestoreType = aRestoreType;
    };

    // Ĭ����������
    ~HistoryAlarm(){};

    SERIALIZE
    {
        SERIALIZE_BEGIN(HistoryAlarm);
        WRITE_VALUE(AlarmID);
        WRITE_VALUE(AlarmCode);
        WRITE_VALUE(AlarmTime);
        WRITE_VALUE(AlarmAddr);
        WRITE_VALUE(SubSystem);
        WRITE_VALUE(Level1);        
        WRITE_VALUE(Level2);        
        WRITE_VALUE(Level3);        
        WRITE_VALUE(Location);
        WRITE_VALUE(AddInfo);
        WRITE_VALUE(RestoreTime);
        WRITE_VALUE(RestoreType);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(HistoryAlarm);
        READ_VALUE(AlarmID);
        READ_VALUE(AlarmCode);
        READ_VALUE(AlarmTime);
        READ_VALUE(AlarmAddr);
        READ_VALUE(SubSystem);
        READ_VALUE(Level1);        
        READ_VALUE(Level2);        
        READ_VALUE(Level3);        
        READ_VALUE(Location);
        READ_VALUE(AddInfo);
        READ_VALUE(RestoreTime);
        READ_VALUE(RestoreType);        
        DESERIALIZE_END();
    };

private:

    // ���ÿ�������
    DISALLOW_COPY_AND_ASSIGN(HistoryAlarm);

};

}  //namespace zte_tecs

#endif // #ifndefALARM_HISTORY_ALARM_H
