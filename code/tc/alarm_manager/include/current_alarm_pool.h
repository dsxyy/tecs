/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�current_alarm_pool.h
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��CurrentAlarmPoll�ඨ���ļ�
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


#ifndef ALARM_CURRENT_ALARM_POOL_H
#define ALARM_CURRENT_ALARM_POOL_H

#include "sky.h"
#include "sqldb.h"
#include "pool_sql.h"
#include "current_alarm.h"

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

#define SYNC_AGENT_MAX_NUMBER     200

typedef struct
{
    unsigned int   AlarmCode;       // �澯��
    unsigned int   dwCRCCode0;    // CRCУ����
    unsigned int   dwCRCCode1;    // CRCУ����
    unsigned int   dwCRCCode2;    // CRCУ����
    unsigned int   dwCRCCode3;    // CRCУ����
    time_t         AlarmTime;    //�澯ʱ��
    string         Location;  //�澯λ��
    string         AddInfo;	       //�澯������Ϣ
}T_SyncCurrentAlarm;

typedef struct
{
    unsigned int          dwAlarmNum;                 /* �澯���� */
    T_SyncCurrentAlarm    tCurrentAlarm[SYNC_AGENT_MAX_NUMBER];
}T_CurrentAlarmPool;


class CurrentAlarmPool : public PoolSQL
{

public:
    ~CurrentAlarmPool(){};         // Ĭ����������

    /**************************************************************************/
    /**
    @brief ��������: ����CurrentAlarm����

    @li @b ����б�
    @verbatim
        os          CurrentAlarm����ת��XML��ʽ����ַ���
    @endverbatim

    @li @b �����б�
    @verbatim
        error_str   ������Ϣ
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
    int64 Allocate(int64* oid, const T_CurrentAlarm& cur_alarm, string& error_str);

    /**************************************************************************/
    /**
    @brief ��������: �������ݿ��¼ID��ȡCurrentAlarm����

    @li @b ����б�
    @verbatim
        oid         ���ݿ��¼ID
        lock        �Ƿ����
    @endverbatim

    @li @b �����б�
    @verbatim
        ��
    @endverbatim

    @li @b ����ֵ�б�
    @verbatim
        CurrentAlarm����ָ��
    @endverbatim

    @li @b �ӿ����ƣ���
    @li @b ����˵������
    */
    /**************************************************************************/
    CurrentAlarm * Get(int64 oid, bool lock)
    {
        CurrentAlarm * alarm = static_cast<CurrentAlarm *>(PoolSQL::Get(oid,lock));

        return alarm;
    };

    /**************************************************************************/
    /**
    @brief ��������: ���ݸ澯�ؼ��ֻ�ȡCurrentAlarm����

    @li @b ����б�
    @verbatim
        alarmkey        �澯�ؼ���
        lock            �Ƿ����
    @endverbatim

    @li @b �����б�
    @verbatim
        ��
    @endverbatim

    @li @b ����ֵ�б�
    @verbatim
        CurrentAlarm����ָ��
    @endverbatim

    @li @b �ӿ����ƣ���
    @li @b ����˵������
    */
    /**************************************************************************/
    CurrentAlarm * Get(string& alarmkey, bool lock)
    {
        map<string, int64>::iterator index;

        index = _known_alarms.find(alarmkey);

        if (_known_alarms.end() != index)
        {
            return Get((int64)index->second,lock);
        }

        return 0;
    };

    /**************************************************************************/
    /**
    @brief ��������: ����CurrentAlarm�������ݿ�

    @li @b ����б�
    @verbatim
        cur_alarm    CurrentAlarm����ָ��
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
    int Update(CurrentAlarm* cur_alarm)
    {
        return cur_alarm->Update(_db);
    };

    /**************************************************************************/
    /**
    @brief ��������: �����ݿ�ɾ��CurrentAlarm����

    @li @b ����б�
    @verbatim
        cur_alarm    CurrentAlarm����ָ��
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
    int Drop(CurrentAlarm* cur_alarm)
    {
        int rc = PoolSQL::Drop(cur_alarm);

        if (0 == rc)
        {
            _known_alarms.erase(cur_alarm->get_alarmkey());
        }

        return rc;
    };
  
    /**************************************************************************/
    /**
    @brief ��������: ����CurrentAlarm���ݿ��ʼ��

    @li @b ����б�
    @verbatim
        db    SQL���ݿ�ָ��
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
    int Init(SqlDB* db)
    {
        _known_alarms.clear();
        CurrentAlarm* cur_alarm = new CurrentAlarm();
        int rc = cur_alarm->DeleteTable(db);
        delete cur_alarm;
        return rc;
    };
    
    /**************************************************************************/
    /**
    @brief ��������: ת������CurrentAlarm����

    @li @b ����б�
    @verbatim
        oss     ����ת���ַ�����
        where   λ���ַ���
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
    int Dump(ostringstream& oss, const string& where);
    
    
    /**************************************************************************/
    /**
    @brief ��������: ת������CurrentAlarm����

    @li @b ����б�
    @verbatim
        tCurrentAlarmPool     ����ת���ṹ
        where   λ���ַ���
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
    int Dump(vector<xmlrpc_c::value>& arrayAlarm, const string& where);

    /**************************************************************************/
    /**
    @brief ��������: ��ȡCurrentAlarmPool����ʵ��

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
        CurrentAlarmPool����ʵ��
    @endverbatim

    @li @b �ӿ����ƣ���
    @li @b ����˵������
    */
    /**************************************************************************/    
    static CurrentAlarmPool *GetInstance()
    {
        return instance;
    };

     /**************************************************************************/
    /**
    @brief ��������: �������߻�ȡCurrentAlarmPool����ʵ��

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
        CurrentAlarmPool����ʵ��
    @endverbatim

    @li @b �ӿ����ƣ���
    @li @b ����˵������
    */
    /**************************************************************************/    
	static CurrentAlarmPool* CreateInstance(SqlDB *db)
    {
        if(NULL == instance)
        {
            instance = new CurrentAlarmPool(db); 
        }
        
        return instance;
    };
    
    void ShowMemoryAlarm();

private:

    map<string, int64> _known_alarms;  //�澯�ؼ���-���ݿ��¼IDӳ��
    CurrentAlarmPool(SqlDB* db);   // Ĭ�Ϲ��캯��
    DISALLOW_COPY_AND_ASSIGN(CurrentAlarmPool);
    static CurrentAlarmPool *instance;

    /**************************************************************************/
    /**
    @brief ��������: PoolSQL����ӿ�ʵ��

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
        PoolObjectSQL����ָ��
    @endverbatim

    @li @b �ӿ����ƣ���
    @li @b ����˵������
    */
    /**************************************************************************/
    PoolObjectSQL * Create()
    {
        return  new CurrentAlarm();
    };

    /**************************************************************************/
    /**
    @brief ��������: ����ת�����ƻص�����

    @li @b ����б�
    @verbatim
        num         ��������
        values      ����ȡֵ����ָ��
        names       �ֶ���������ָ��
    @endverbatim

    @li @b �����б�
    @verbatim
      oss         ostringstream����ָ��
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
    int DumpCallback(void* oss, SqlColumn* columns);
    
    /**************************************************************************/
    /**
    @brief ��������: ����ת�����ƻص�����

    @li @b ����б�
    @verbatim
        num         ��������
        values      ����ȡֵ����ָ��
        names       �ֶ���������ָ��
    @endverbatim

    @li @b �����б�
    @verbatim
      T_CurrentAlarmPool         pCurrentAlarmPool�澯�ؽṹָ��
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
    int DumpStructCallback(void *arrayAlarm, SqlColumn* columns);

    /**************************************************************************/
    /**
    @brief ��������: CurrentAlarm���ݿ��ʼ���ص�����

    @li @b ����б�
    @verbatim
        num         ��������
        values      ����ȡֵ����ָ��
        names       �ֶ���������ָ��    @endverbatim
        @li @b �����б�
    @verbatim
        nil         ��ָ��
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
    int InitCallback(void* nil, SqlColumn* columns);

};

}  // namespace zte_tecs
#endif // #ifndef ALARM_CURRENT_ALARM_POOL_H
