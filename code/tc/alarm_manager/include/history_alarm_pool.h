/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�history_alarm_pool.h
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��HistoryAlarmPoll�ඨ���ļ�
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


#ifndef ALARM_HISTORY_ALARM_POOL_H
#define ALARM_HISTORY_ALARM_POOL_H

#include "sky.h"
#include "sqldb.h"
#include "pool_sql.h"
#include "history_alarm.h"
#include "alarm_api.h"
#include "pool_object_sql.h"

//#include <time.h>
//#include <sstream>

//#include <iostream>

//#include <vector>

// ���ÿ�������궨��
#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
            TypeName(const TypeName&); \
            void operator=(const TypeName&)
#endif

using namespace std;


namespace zte_tecs
{

class HistoryAlarmPool : public SqlCallbackable
{

public:

    // ���ݿ��ֶζ���
    enum ColNames
    {
        OID            = 0,   // �����ʶ
        ALARMID        = 1,   // �澯��ˮ��
        ALARMCODE      = 2,   // �澯��
        ALARMTIME      = 3,   // �澯ԭ��
        ALARMADDR      = 4,   // �澯����
        LEVEL1         = 5,   // 
        LEVEL2         = 6,   // 
        LEVEL3         = 7,   // 
        LOCATION       = 8,   // �澯λ��
        ADDINFO        = 9,   // 
        RESTORETIME    = 10,  // �澯�ָ�ʱ��
        RESTORETYPE    = 11,  // �澯�ָ�����
        SUBSYSTEM      = 12,
        LIMIT          = 13
    };


    ~HistoryAlarmPool(){};         // Ĭ����������

    STATUS Init();

    /**************************************************************************/
    /**
    @brief ��������: ����HistoryAlarm����

    @li @b ����б�
    @verbatim
        alarm    HistoryAlarm����ָ��
    @endverbatim

    @li @b �����б�
    @verbatim
    @endverbatim

    @li @b ����ֵ�б�
    @verbatim
        0           �ɹ�
        1          ʧ��
    @endverbatim

    @li @b �ӿ����ƣ���
    @li @b ����˵������
    */
    /**************************************************************************/
    STATUS Allocate(HistoryAlarm &alarm);

    //�����ӿڲ�ѯ
    STATUS Get(int64 oid, HistoryAlarm &history_alarm);

    //������ѯ
    STATUS Show(vector<ApiHistoryAlarmInfo> &vec_api_alarm, const string& where);


    /**************************************************************************/
    /**
    @brief ��������: �����ݿ�ɾ��HistoryAlarm����

    @li @b ����б�
    @verbatim
        alarm    HistoryAlarm����ָ��
    @endverbatim

    @li @b �����б�
    @verbatim
        ��
    @endverbatim

    @li @b ����ֵ�б�
    @verbatim
        0           �ɹ�
        -1          ʧ��
    @endverbatim

    @li @b �ӿ����ƣ���
    @li @b ����˵������
    */
    /**************************************************************************/
    STATUS Drop(const HistoryAlarm &alarm);

    /**************************************************************************/
    /**
    @brief ��������: ��ȡHistoryAlarmPool����ʵ��

    @li @b ����б�
    @verbatim
        ��
    @endverbatim

    @li @b �����б�
    @verbatim
        ��
    @endverbatim

    @li @b ����ֵ�б�
    @verbatim
        HistoryAlarmPool����ʵ��
    @endverbatim

    @li @b �ӿ����ƣ���
    @li @b ����˵������
    */
    /**************************************************************************/    
    static HistoryAlarmPool *GetInstance()
    {
        SkyAssert(NULL != instance);
        return instance;
    };

    /**************************************************************************/
    /**
    @brief ��������: �������߻�ȡHistoryAlarmPool����ʵ��

    @li @b ����б�
    @verbatim
        db ���ӵ����ݿ����ָ��
    @endverbatim

    @li @b �����б�
    @verbatim
        ��
    @endverbatim

    @li @b ����ֵ�б�
    @verbatim
        HistoryAlarmPool����ʵ��
    @endverbatim

    @li @b �ӿ����ƣ���
    @li @b ����˵������
    */
    /**************************************************************************/   
    static HistoryAlarmPool* CreateInstance(SqlDB *db)
    {
        if(NULL == instance)
        {
            instance = new HistoryAlarmPool(db); 
        }
        
        return instance;
    };

    /**************************************************************************/
    /**
    @brief ��������: ����HistoryAlarm�������ݿ�

    @li @b ����б�
    @verbatim
        alarm    HistoryAlarm����ָ��
    @endverbatim

    @li @b �����б�
    @verbatim
        ��
    @endverbatim

    @li @b ����ֵ�б�
    @verbatim
        0    �ɹ�
        -1   ʧ��
    @endverbatim

    @li @b �ӿ����ƣ���
    @li @b ����˵������
    */
    /**************************************************************************/
    STATUS Update(const HistoryAlarm &alarm); 

    static void Lock()
    {
        pthread_mutex_lock(&_mutex);
    };

    static void UnLock()
    {
        pthread_mutex_unlock(&_mutex);
    };


private:

    HistoryAlarmPool(SqlDB *pDb):SqlCallbackable(pDb){};   // Ĭ�Ϲ��캯��
    DISALLOW_COPY_AND_ASSIGN(HistoryAlarmPool);
    static HistoryAlarmPool *instance;

    int GetHistoryAlarmCallback(void *nil, SqlColumn *columns);
    int ShowHistoryAlarmCallback(void * arg, SqlColumn * columns);


    //�������ʶ
    static int64 _lastOID;
    static pthread_mutex_t  _mutex;

protected:

    static const char* _db_names;       // ���ݿ��ֶζ���
    static const char* _table;          // ���ݿ����

};

}  // namespace zte_tecs
#endif // #ifndef ALARM_HISTORY_ALARM_POOL_H

