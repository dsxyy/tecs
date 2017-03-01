/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�current_alarm.h
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��CurrentAlarm�ඨ���ļ�
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


#ifndef ALARM_CURRENT_ALARM_H
#define ALARM_CURRENT_ALARM_H

#include "sky.h"
#include "sqldb.h"
#include "pool_object_sql.h"
#include "tecs_rpc_method.h"

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

// T_CurrentAlarm���ݽṹ����
struct T_CurrentAlarm
{
    unsigned int   AlarmID;         // �澯��ˮ��
    unsigned int   AlarmCode;       // �澯��
    time_t         AlarmTime;       // �澯ʱ��
    string         AlarmAddr;        // �澯����λ��
    string         SubSystem;
    string         Level1;           
    string         Level2;          
    string         Level3;          
    string         Location;     // �澯λ��������Ϣ
    string         AddInfo;         // �澯������Ϣ
    string         AlarmKey;        // �澯�ؼ���, �澯������ݸ澯������Ϣ����
    unsigned int   dwCRCCode0;      // CRCУ����
    unsigned int   dwCRCCode1;      // CRCУ����
    unsigned int   dwCRCCode2;      // CRCУ����
    unsigned int   dwCRCCode3;      // CRCУ����
    
    // ���췽��

    T_CurrentAlarm (const T_CurrentAlarm& ca) :
                    AlarmID(ca.AlarmID),
                    AlarmCode(ca.AlarmCode),
                    AlarmTime(ca.AlarmTime),
                    AlarmAddr(ca.AlarmAddr),
                    SubSystem(ca.SubSystem),
                    Level1(ca.Level1),
                    Level2(ca.Level2),
                    Level3(ca.Level3),
                    Location(ca.Location),
                    AddInfo(ca.AddInfo),
                    AlarmKey(ca.AlarmKey),
                    dwCRCCode0(ca.dwCRCCode0),
                    dwCRCCode1(ca.dwCRCCode1),
                    dwCRCCode2(ca.dwCRCCode2),
                    dwCRCCode3(ca.dwCRCCode3){};

    // ���췽��
    T_CurrentAlarm (unsigned int   aAlarmID = 0,
                    unsigned int   aAlarmCode = 0,
                    time_t         aAlarmTime = 0,
                    const string&  aAlarmAddr = "",
                    const string&  aSubSystem = "",
                    const string&  aLevel1 = "",
                    const string&  aLevel2 = "",
                    const string&  aLevel3 = "",
                    const string&  aLocation = "",
                    const string&  aAddInfo = "",
                    const string&  aAlarmKey = "",
                    unsigned int   aCRCCode0 = 0,
                    unsigned int   aCRCCode1 = 0,
                    unsigned int   aCRCCode2 = 0,
                    unsigned int   aCRCCode3 = 0) :
                    AlarmID(aAlarmID),
                    AlarmCode(aAlarmCode),
                    AlarmTime(aAlarmTime),
                    AlarmAddr(aAlarmAddr),
                    SubSystem(aSubSystem),
                    Level1(aLevel1),
                    Level2(aLevel2),
                    Level3(aLevel3),
                    Location(aLocation),
                    AddInfo(aAddInfo),
                    AlarmKey(aAlarmKey),
                    dwCRCCode0(aCRCCode0),
                    dwCRCCode1(aCRCCode1),
                    dwCRCCode2(aCRCCode2),
                    dwCRCCode3(aCRCCode3){};

};


class CurrentAlarm : public PoolObjectSQL
{

public:

    /**************************************************************************/
    /**
    @brief ��������: CurrentAlarm����ת��XML��ʽ������

    @li @b ����б�
    @verbatim
        os    CurrentAlarm����ת��XML��ʽ����ַ���
    @endverbatim

    @li @b �����б�
    @verbatim
        ��
    @endverbatim

    @li @b ����ֵ�б�
    @verbatim
        CurrentAlarm����ת��XML��ʽ����ַ���
    @endverbatim

    @li @b �ӿ����ƣ���
    @li @b ����˵������
    */
    /**************************************************************************/
    friend ostream& operator<<(ostream& os, CurrentAlarm& alarm);


    /**************************************************************************/
    /**
    @brief ��������: ת��CurrentAlarm���󵽸�ʽ���ַ���

    @li @b ����б�
    @verbatim
        xml   CurrentAlarm�����ʽ���ַ���
    @endverbatim

    @li @b �����б�
    @verbatim
        ��
    @endverbatim

    @li @b ����ֵ�б�
    @verbatim
        CurrentAlarm�����ʽ���ַ���
    @endverbatim

    @li @b �ӿ����ƣ���
    @li @b ����˵������
    */
    /**************************************************************************/
    string& to_str(string& str) const;

    /**************************************************************************/
    /**
    @brief ��������: ת��CurrentAlarm����XML�ַ���

    @li @b ����б�
    @verbatim
        xml   CurrentAlarm����XML�ַ���
    @endverbatim

    @li @b �����б�
    @verbatim
        ��
    @endverbatim

    @li @b ����ֵ�б�
    @verbatim
        CurrentAlarm����XML�ַ���
    @endverbatim

    @li @b �ӿ����ƣ���
    @li @b ����˵������
    */
    /**************************************************************************/
    string& to_xml(string& xml) const;

    /**************************************************************************/
    /**
    @brief ��������: ��ȡCurrentAlarm�������ݿ��¼��ʶ

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
        CurrentAlarm�������ݿ��¼��ʶ
    @endverbatim

    @li @b �ӿ����ƣ���
    @li @b ����˵������
    */
    /**************************************************************************/
    int64 get_id() const
    {
        return _oid;
    };
    
    /**************************************************************************/
    /**
    @brief ��������: ��ȡCurrentAlarm����澯��Ϣ�ṹ

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
        T_CurrentAlarm�ṹ
    @endverbatim

    @li @b �ӿ����ƣ���
    @li @b ����˵������
    */
    /**************************************************************************/
    const T_CurrentAlarm & get_current_alarm() const
    {
        return _current_alarm;
    }
    
    /**************************************************************************/
    /**
    @brief ��������: ����CurrentAlarm����澯��Ϣ

    @li @b ����б�
    @verbatim
        current_alarm  T_CurrentAlarm�ṹ
    @endverbatim

    @li @b �����б�
    @verbatim
        ��
    @endverbatim

    @li @b ����ֵ�б�
    @verbatim
        ��
    @endverbatim

    @li @b �ӿ����ƣ���
    @li @b ����˵������
    */
    /**************************************************************************/
    void set_current_alarm(const T_CurrentAlarm & current_alarm)
    {
        _current_alarm = current_alarm;
    }

    /**************************************************************************/
    /**
    @brief ��������: ��ȡCurrentAlarm����澯�ؼ���

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
        CurrentAlarm����澯�ؼ���
    @endverbatim

    @li @b �ӿ����ƣ���
    @li @b ����˵������
    */
    /**************************************************************************/
    const string& get_alarmkey() const
    {
        return _current_alarm.AlarmKey;
    };

protected:

   // Ĭ�Ϲ��캯��
    CurrentAlarm(int64 id = -1, const T_CurrentAlarm& current_alarm = 0);
    // Ĭ����������
    virtual ~CurrentAlarm(){};

    // ���ݿ��ֶζ���
    enum ColNames
    {
        OID            = 0,   // �����ʶ
        ALARMID        = 1,   // �澯��ˮ��
        ALARMCODE      = 2,   // �澯��
        ALARMTIME      = 3,   // �澯ʱ��
        ALARMADDR      = 4,   // �澯λ��
        LEVEL1         = 5,   
        LEVEL2         = 6,   
        LEVEL3         = 7,   
        LOCATION       = 8,   
        ADDINFO        = 9,   // �澯������Ϣ
        ALARMKEY       = 10,   // �澯�ؼ���
        CRCCODE0       = 11,   // CRCУ����0
        CRCCODE1       = 12,  // CRCУ����1	
        CRCCODE2       = 13,  // CRCУ����2
        CRCCODE3       = 14,  // CRCУ����3
        SUBSYSTEM      = 15,
        LIMIT          = 16
    };

    static const char* _db_names;       // ���ݿ��ֶζ���
    static const char* _table;          // ���ݿ����

    /**************************************************************************/
    /**
    @brief ��������: �����ݿ��ȡCurrentAlarm����

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
    virtual int Select(SqlDB* db);

    /**************************************************************************/
    /**
    @brief ��������: ����CurrentAlarm�������ݿ�

    @li @b ����б�
    @verbatim
        db         SQL���ݿ�ָ��
        error_str  ������Ϣ�ַ���
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
    virtual int Insert(SqlDB* db, string& error_str);

    /**************************************************************************/
    /**
    @brief ��������: ����CurrentAlarm�������ݿ�

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
    virtual int Update(SqlDB* db);

    /**************************************************************************/
    /**
    @brief ��������: �����ݿ�ɾ��CurrentAlarm����

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
    virtual int Drop(SqlDB* db);
    
    /**************************************************************************/
    /**
    @brief ��������: �����ݿ�ɾ���������ж���

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
    int DeleteTable(SqlDB* db);

    /**************************************************************************/
    /**
    @brief ��������: ת��CurrentAlarm����XML�ַ���

    @li @b ����б�
    @verbatim
        columns  ���ݿ��ֶ�ָ��
    @endverbatim

    @li @b �����б�
    @verbatim
        oss     ת���ַ�����
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
    static  int Dump(ostringstream& oss, SqlColumn * columns);
    
        /**************************************************************************/
    /**
    @brief ��������: ת��CurrentAlarm����T_CurrentAlarm�ṹ

    @li @b ����б�
    @verbatim
        columns  ���ݿ��ֶ�ָ��
    @endverbatim

    @li @b �����б�
    @verbatim
        T_CurrentAlarm     �ṹ
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
    static  int Dump(T_CurrentAlarm *pCurrentAlarm, SqlColumn * columns);
    
    static  int Dump(vector<xmlrpc_c::value>& arrayAlarm, SqlColumn* columns);

private:

    friend class CurrentAlarmPool;

    T_CurrentAlarm _current_alarm;     // ��ǰ�澯�ṹ

    /**************************************************************************/
    /**
    @brief ��������: ִ�����ݿ�Insert��Replace SQL����

    @li @b ����б�
    @verbatim
        db        SQL���ݿ�ָ��
        replace   �Ƿ��滻
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
    int Insert_Replace(SqlDB* db, bool replace);

    /**************************************************************************/
    /**
    @brief ��������: ���ݿ��ֶε�CurrentAlarm����ת���ص�����

    @li @b ����б�
    @verbatim
        nil      ��Чָ��
        columns  ���ݿ��ֶ�ָ��
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
    int SelectCallback(void* nil, SqlColumn * columns);

    // ���ÿ�������
    DISALLOW_COPY_AND_ASSIGN(CurrentAlarm);
};

}  //namespace zte_tecs

#endif // #ifndefALARM_CURRENT_ALARM_H

