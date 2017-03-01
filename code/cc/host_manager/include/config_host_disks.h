/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�config_host_disks.h
* �ļ���ʶ��
* ����ժҪ��ConfigHostDisks ��Ķ����ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�liuyi
* ������ڣ�2013��7��25��
*
* �޸ļ�¼1��
*     �޸����ڣ�2013/7/25
*     �� �� �ţ�V1.0
*     �� �� �ˣ�xiett
*     �޸����ݣ�����
*******************************************************************************/
#ifndef CONFIG_HOST_DISKS_H
#define CONFIG_HOST_DISKS_H
#include "sky.h"
#include "sqldb.h"
#include "db_config.h"
#include "pool_sql.h"
#include "msg_host_manager_with_host_agent.h"

using namespace std;
namespace zte_tecs {

/**
@brief ��������: HCӲ����Ϣ��
@li @b ����˵������
*/
class ConfigHostDisks : public Callbackable
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    static ConfigHostDisks *GetInstance()
    {
        return _instance;
    }

    static ConfigHostDisks* CreateInstance(SqlDB *db)
    {
        if(NULL == _instance)
        {
            _instance = new ConfigHostDisks(db); 
        }
        
        return _instance;
    };

    virtual ~ConfigHostDisks() { _instance = NULL; };
    int32 Insert(const DiskInfo& disk_info, int64 hid);
    int32 Delete(int64 hid);

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
        TYPE,
        STATUS,
        LIMIT
    };
    static const char *_table;
    static const char *_db_names;
    static SqlDB      *_db;

    static ConfigHostDisks *_instance;
    ConfigHostDisks() {};
    ConfigHostDisks(SqlDB *pDb);
    DISALLOW_COPY_AND_ASSIGN(ConfigHostDisks);
};

}
#endif
