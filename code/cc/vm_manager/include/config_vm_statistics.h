/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�config_vm_statistics.h
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��ConfigVmStatistics �� ConfigVmStatisticsNics ���ͷ�ļ�
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
#ifndef CONFIG_VM_STATISTICS_H
#define CONFIG_VM_STATISTICS_H

#include "sky.h"
#include "sqldb.h"
#include "db_config.h"
#include "pool_sql.h"
#include "vm_messages.h"
#include "msg_project_manager_with_api_method.h"

using namespace std;
namespace zte_tecs {

/**
@brief ��������: ConfigVmStatistics�ඨ��
@li @b ����˵�����������Դʹ�ü����ͳ��
*/
class ConfigVmStatistics : public Callbackable
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    static ConfigVmStatistics *GetInstance()
    {
        return _instance;
    }

    static ConfigVmStatistics* CreateInstance(SqlDB *db)
    {
        if(NULL == _instance)
        {
            _instance = new ConfigVmStatistics(db); 
        }
        
        return _instance;
    };
    virtual ~ConfigVmStatistics() { _instance = NULL; };
    int32 SelectCallback(void *nil, SqlColumn *columns);
    int32 Search(vector<ResourceStatistics>& vec_vm_statistics,
                 const string where);
    int32 Insert(const ResourceStatistics& vm_statistics,
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
        VID = 0,
        STATISTICS_TIME,
        SAVE_TIME,
        CPU_USED_RATE,
        MEM_USED_RATE,
        LIMIT
    };
    static const char *_table;
    static const char *_db_names;
    static SqlDB      *_db;

    static ConfigVmStatistics *_instance;

    ConfigVmStatistics() {};
    ConfigVmStatistics(SqlDB *pDb);
    DISALLOW_COPY_AND_ASSIGN(ConfigVmStatistics);
};

/**
@brief ��������: ConfigVmStatisticsNics�ඨ��
@li @b ����˵�����������Դʹ�ü����ͳ��
*/
class ConfigVmStatisticsNics : public Callbackable
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    static ConfigVmStatisticsNics *GetInstance()
    {
        return _instance;
    }

    static ConfigVmStatisticsNics* CreateInstance(SqlDB *db)
    {
        if(NULL == _instance)
        {
            _instance = new ConfigVmStatisticsNics(db); 
        }
        
        return _instance;
    };
    virtual ~ConfigVmStatisticsNics() { _instance = NULL; };
    int32 SelectCallback(void *nil, SqlColumn *columns);
    //int32 Search(vector<NicInfo>& vec_nic_info, const string where);
    int32 Insert(const NicInfo& nic_info, 
                 int64 vid,
                 const string& statistics_time,
                 const string& save_time);
    int32 Delete(const string& statistics_time);
    int GetVmStatisticsNics(vector<VmStatisticsNics>& vec_info,const string where);

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
        VID = 0,
        NAME,
        STATISTICS_TIME,
        SAVE_TIME,
        USED_RATE,
        LIMIT
    };
    static const char *_table;
    static const char *_db_names;
    static SqlDB      *_db;

    static ConfigVmStatisticsNics *_instance;

    ConfigVmStatisticsNics() {};
    ConfigVmStatisticsNics(SqlDB *pDb);
    DISALLOW_COPY_AND_ASSIGN(ConfigVmStatisticsNics);
};

/**
@brief ��������: VmDynamicData�ඨ��
@li @b ����˵����vm��̬����ͳ��
*/
class VmDynamicData : public Callbackable
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    static VmDynamicData *GetInstance()
    {
        return _instance;
    }

    static VmDynamicData* CreateInstance(SqlDB *db)
    {
        if(NULL == _instance)
        {
            _instance = new VmDynamicData(db); 
        }
        
        return _instance;
    };

    virtual ~VmDynamicData() { _instance = NULL; };
    VmDynamicData() {};
    int32 GetProjectDynamicData(const string &where, vector<ProjectDynamicData> &project_data);
    int32 ProjectDataCallback(void *nil, SqlColumn *columns);
    int32 CountProjectDynamicData(void);
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
        VID = 0,
        NIC_USED_RATE,
        CPU_USED_RATE,
        MEM_USED_RATE,
        LIMIT
    };
    static const char *_table;
    static const char *_db_names;
    static SqlDB      *_db;
    static VmDynamicData *_instance;
    VmDynamicData(SqlDB *pDb);	
    DISALLOW_COPY_AND_ASSIGN(VmDynamicData);
};

}
#endif
