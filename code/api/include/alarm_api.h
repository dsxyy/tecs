/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�alarm_api.h
* �ļ���ʶ��
* ����ժҪ��ApiHistoryAlarmInfo�ඨ���ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�
* ������ڣ�
*
* �޸ļ�¼1��
*     �޸����ڣ�
*     �� �� �ţ�V1.0
*     �� �� �ˣ�
*     �޸����ݣ�����
*******************************************************************************/


#ifndef ALARM_API_H
#define ALARM_API_H
#include "rpcable.h"

using namespace std;

namespace zte_tecs
{

class ApiHistoryAlarmInfo : public Rpcable
{
public:
    ApiHistoryAlarmInfo(){};

    ~ApiHistoryAlarmInfo(){};

  TO_RPC
    {
        TO_RPC_BEGIN();

        TO_VALUE(AlarmID);
        TO_VALUE(AlarmCode);
        TO_VALUE(AlarmTime);
        TO_VALUE(AlarmAddr);
        TO_VALUE(SubSystem);
        TO_VALUE(Level1);
        TO_VALUE(Level2);
        TO_VALUE(Level3);
        TO_VALUE(Location);
        TO_VALUE(AddInfo);
        TO_VALUE(RestoreTime);
        TO_VALUE(RestoreType);

        TO_RPC_END();
    };

    FROM_RPC
    {
        FROM_RPC_BEGIN();
        
        FROM_VALUE(AlarmID);
        FROM_VALUE(AlarmCode);
        FROM_VALUE(AlarmTime);
        FROM_VALUE(AlarmAddr);
        FROM_VALUE(SubSystem);
        FROM_VALUE(Level1);
        FROM_VALUE(Level2);
        FROM_VALUE(Level3);
        FROM_VALUE(Location);
        FROM_VALUE(AddInfo);
        FROM_VALUE(RestoreTime);
        FROM_VALUE(RestoreType);

        FROM_RPC_END();
    };
    
    //��ʷ�澯�ֶ�
    int      AlarmID;         // �澯��ˮ��
    int      AlarmCode;       // �澯��
    int      AlarmTime;       // �澯ʱ��
    string   AlarmAddr;       // �澯λ��
    string   SubSystem;
    string   Level1;          // TC����
    string   Level2;          // CC����
    string   Level3;          // HC����
    string   Location;        // �澯λ��������Ϣ    
    string   AddInfo;         // �澯������Ϣ
    int      RestoreTime;     // �ָ�ʱ��
    int      RestoreType;     // �澯�ָ�����

};

}  //namespace zte_tecs

#endif // #ifndefALARM_API_H
