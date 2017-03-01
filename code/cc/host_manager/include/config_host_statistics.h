/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�config_host_statistics.h
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��ConfigHostStatistics �� ConfigHostStatisticsNics ���ͷ�ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�xiett
* ������ڣ�2013��7��30��
*
* �޸ļ�¼1��
*     �޸����ڣ�2011/7/30
*     �� �� �ţ�V1.0
*     �� �� �ˣ�xiett
*     �޸����ݣ�����
*******************************************************************************/
#ifndef CONFIG_HOST_STATISTICS_H
#define CONFIG_HOST_STATISTICS_H

#include "sky.h"
#include "sqldb.h"
#include "db_config.h"
#include "pool_sql.h"
#include "msg_host_manager_with_host_agent.h"

using namespace std;
namespace zte_tecs {

/**
@brief ��������: ConfigHostStatistics�ඨ��
@li @b ����˵�����������Դʹ�ü����ͳ��
*/
class ConfigHostStatistics : public Callbackable
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    static ConfigHostStatistics *GetInstance()
    {
        return _instance;
    }

    static ConfigHostStatistics* CreateInstance(SqlDB *db)
    {
        if(NULL == _instance)
        {
            _instance = new ConfigHostStatistics(db); 
        }
        
        return _instance;
    };

    virtual ~ConfigHostStatistics() { _instance = NULL; };
    int32 SelectCallback(void *nil, SqlColumn *columns);
    int32 Search(vector<ResourceStatistics>& vec_host_statistics,
                                   const string where);
    int32 Insert(const ResourceStatistics& host_statistics,
                 int64 hid,
                 const string& save_time);
    int32 Delete(const string& statistics_time);

/*******************************************************************************
* 2. protected section
*******************************************************************************/
// protected:

/*******************************************************************************
* 3. private section
*******************************************************************************/
private:
    /* ����Ա�����ݿ���е���λ�� */
    enum ColNames
    {
        HID = 0,
        STATISTICS_TIME,
        SAVE_TIME,
        CPU_USED_RATE,
        MEM_USED_RATE,
        LIMIT
    };
    static const char *_table;
    static const char *_db_names;
    static SqlDB      *_db;

    static ConfigHostStatistics *_instance;

    ConfigHostStatistics() {};
    ConfigHostStatistics(SqlDB *pDb);
    DISALLOW_COPY_AND_ASSIGN(ConfigHostStatistics);
};

/**
@brief ��������: ConfigHostStatisticsNics�ඨ��
@li @b ����˵�����������Դʹ�ü����ͳ��
*/
class ConfigHostStatisticsNics : public Callbackable
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    static ConfigHostStatisticsNics *GetInstance()
    {
        return _instance;
    }

    static ConfigHostStatisticsNics* CreateInstance(SqlDB *db)
    {
        if(NULL == _instance)
        {
            _instance = new ConfigHostStatisticsNics(db); 
        }
        
        return _instance;
    };

    virtual ~ConfigHostStatisticsNics() { _instance = NULL; };
    int32 SelectCallback(void *nil, SqlColumn *columns);
    int32 Search(vector<NicInfo>& vec_nic_info, const string where);
    int32 Insert(const NicInfo& nic_info, 
                 int64 hid,
                 const string& statistics_time,
                 const string& save_time);
    int32 Delete(const string& statistics_time);

/*******************************************************************************
* 2. protected section
*******************************************************************************/
// protected:

/*******************************************************************************
* 3. private section
*******************************************************************************/
private:
    /* ����Ա�����ݿ���е���λ�� */
    enum ColNames
    {
        HID = 0,
        NAME,
        STATISTICS_TIME,
        SAVE_TIME,
        USED_RATE,
        LIMIT
    };
    static const char *_table;
    static const char *_db_names;
    static SqlDB      *_db;

    static ConfigHostStatisticsNics *_instance;

    ConfigHostStatisticsNics() {};
    ConfigHostStatisticsNics(SqlDB *pDb);
    DISALLOW_COPY_AND_ASSIGN(ConfigHostStatisticsNics);
};

}
#endif
