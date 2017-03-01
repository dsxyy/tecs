#ifndef CONFIG_HOST_HWMON_H
#define CONFIG_HOST_HWMON_H
#include "sky.h"
#include "sqldb.h"
#include "db_config.h"
#include "pool_sql.h"
#include "msg_host_manager_with_host_agent.h"

using namespace std;
namespace zte_tecs {

/**
@brief 功能描述: HC硬件监控信息类
@li @b 其它说明：无
*/
class ConfigHostHwmon: public Callbackable
{
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    static ConfigHostHwmon *GetInstance()
    {
        return _instance;
    }

    static ConfigHostHwmon* CreateInstance(SqlDB *db)
    {
        if(NULL == _instance)
        {
            _instance = new ConfigHostHwmon(db); 
        }
        
        return _instance;
    };

    virtual ~ConfigHostHwmon() { _instance = NULL; };
    int32 Insert(const vector<HostHwmon>& vec_host_hwmon, int64 hid);
    int32 Delete(int64 hid);
    
/*******************************************************************************
* 2. protected section
*******************************************************************************/
// protected:

/*******************************************************************************
* 3. private section
*******************************************************************************/
private:
    /* 各成员在数据库表中的列位置 */
    enum ColNames
    {
        HID = 0,
        TEMP1_NAME,
        TEMP1_VALUE,
        TEMP2_NAME,
        TEMP2_VALUE,
        TEMP3_NAME,
        TEMP3_VALUE,
        TEMP4_NAME,
        TEMP4_VALUE,
        TEMP5_NAME,
        TEMP5_VALUE,
        TEMP6_NAME,
        TEMP6_VALUE,
        TEMP7_NAME,
        TEMP7_VALUE,
        TEMP8_NAME,
        TEMP8_VALUE,
        CPU0_VID_NAME,
        CPU0_VID_VALUE,
        CPU1_VID_NAME,
        CPU1_VID_VALUE,
        FAN1_RPM_NAME,
        FAN1_RPM_VALUE,
        FAN2_RPM_NAME,
        FAN2_RPM_VALUE,
        FAN3_RPM_NAME,
        FAN3_RPM_VALUE,
        FAN4_RPM_NAME,
        FAN4_RPM_VALUE,
        FAN5_RPM_NAME,
        FAN5_RPM_VALUE,
        FAN6_RPM_NAME,
        FAN6_RPM_VALUE,
        FAN7_RPM_NAME,
        FAN7_RPM_VALUE,
        FAN8_RPM_NAME,
        FAN8_RPM_VALUE,
        IN0_NAME,
        IN0_VALUE,
        IN1_NAME,
        IN1_VALUE,
        IN2_NAME,
        IN2_VALUE,
        IN3_NAME,
        IN3_VALUE,
        IN4_NAME,
        IN4_VALUE,
        IN5_NAME,
        IN5_VALUE,
        IN6_NAME,
        IN6_VALUE,
        IN7_NAME,
        IN7_VALUE,
        IN8_NAME,
        IN8_VALUE,
        IN9_NAME,
        IN9_VALUE,
        IN10_NAME,
        IN10_VALUE,
        IN11_NAME,
        IN11_VALUE,
        IN12_NAME,
        IN12_VALUE,
        IN13_NAME,
        IN13_VALUE,
        IN14_NAME,
        IN14_VALUE,
        LIMIT
    };

    static const char *_table;
    static const char *_db_names;
    static SqlDB      *_db;

    static ConfigHostHwmon *_instance;
    ConfigHostHwmon() {};
    ConfigHostHwmon(SqlDB *pDb);
    DISALLOW_COPY_AND_ASSIGN(ConfigHostHwmon);
};
}
#endif
