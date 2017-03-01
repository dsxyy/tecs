#include "config_host_hwmon.h"

namespace zte_tecs{

/* host_hwmon配置使用的表名称 */
const char *ConfigHostHwmon::_table = "host_hwmon";

/* host_hwmon配置中的表字段 */
const char *ConfigHostHwmon::_db_names = 
    "hid, temp1_name, temp1_value, temp2_name, temp2_value, temp3_name, temp3_value, "
    "temp4_name, temp4_value, temp5_name, temp5_value, temp6_name, temp6_value, "
    "temp7_name, temp7_value, temp8_name, temp8_value, cpu0_vid_name, cpu0_vid_value, "
    "cpu1_vid_name, cpu1_vid_value, fan1_rpm_name, fan1_rpm_value, fan2_rpm_name, fan2_rpm_value, "
    "fan3_rpm_name, fan3_rpm_value, fan4_rpm_name, fan4_rpm_value, fan5_rpm_name, fan5_rpm_value, "
    "fan6_rpm_name, fan6_rpm_value, fan7_rpm_name, fan7_rpm_value, fan8_rpm_name, fan8_rpm_value, "
    "in0_name, in0_value, in1_name, in1_value, in2_name, in2_value, in3_name, in3_value, "
    "in4_name, in4_value, in5_name, in5_value, in6_name, in6_value, in7_name, in7_value, "
    "in8_name, in8_value, in9_name, in9_value, in10_name, in10_value, in11_name, in11_value, "
    "in12_name, in12_value, in13_name, in13_value, in14_name, in14_value ";

SqlDB *ConfigHostHwmon::_db = NULL;

ConfigHostHwmon *ConfigHostHwmon::_instance = NULL;

/******************************************************************************/
ConfigHostHwmon::ConfigHostHwmon(SqlDB *pDb)
{
    _db = pDb;
}


/******************************************************************************/
int32 ConfigHostHwmon::Insert(const vector<HostHwmon>& vec_host_hwmon,
                              int64 hid)
{
#define CASEITEM(type, x, post1, post2)\
    case x:\
    {\
        sql.Add(#type#x#post1, it->_name);\
        sql.Add(#type#x#post2, it->_value);\
        break;\
    }
    SqlCommand sql(_db,  _table);
    sql.Add("hid", hid);
    vector<HostHwmon>::const_iterator it;
    int32 temp_index = 0, cpuvid_index = 0, fan_index = 0, in_index = 0;
    for (it  = vec_host_hwmon.begin();
         it != vec_host_hwmon.end();
         it++)
    {
        switch (it->_type)
        {
            case SENSOR_TYPE_TEMP:
            {
                temp_index++;
                switch (temp_index)
                {
                    CASEITEM(temp, 1, _name, _value)
                    CASEITEM(temp, 2, _name, _value)
                    CASEITEM(temp, 3, _name, _value)
                    CASEITEM(temp, 4, _name, _value)
                    CASEITEM(temp, 5, _name, _value)
                    CASEITEM(temp, 6, _name, _value)
                    CASEITEM(temp, 7, _name, _value)
                    CASEITEM(temp, 8, _name, _value)
                    default: break;
                }
                break;
            }
            case SENSOR_TYPE_CPUVID:
            {
                switch (cpuvid_index)
                {
                    CASEITEM(cpu, 0, _vid_name, _vid_value)
                    CASEITEM(cpu, 1, _vid_name, _vid_value)
                    default: break;
                }
                cpuvid_index++;
                break;
            }
            case SENSOR_TYPE_FANRPM:
            {
                fan_index++;
                switch (fan_index)
                {
                    CASEITEM(fan, 1, _rpm_name, _rpm_value)
                    CASEITEM(fan, 2, _rpm_name, _rpm_value)
                    CASEITEM(fan, 3, _rpm_name, _rpm_value)
                    CASEITEM(fan, 4, _rpm_name, _rpm_value)
                    CASEITEM(fan, 5, _rpm_name, _rpm_value)
                    CASEITEM(fan, 6, _rpm_name, _rpm_value)
                    CASEITEM(fan, 7, _rpm_name, _rpm_value)
                    CASEITEM(fan, 8, _rpm_name, _rpm_value)
                    default: break;
                }
                break;
            }
            case SENSOR_TYPE_IN:
            {
                switch (in_index)
                {
                    CASEITEM(in, 0, _name, _value)
                    CASEITEM(in, 1, _name, _value)
                    CASEITEM(in, 2, _name, _value)
                    CASEITEM(in, 3, _name, _value)
                    CASEITEM(in, 4, _name, _value)
                    CASEITEM(in, 5, _name, _value)
                    CASEITEM(in, 6, _name, _value)
                    CASEITEM(in, 7, _name, _value)
                    CASEITEM(in, 8, _name, _value)
                    CASEITEM(in, 9, _name, _value)
                    CASEITEM(in, 10, _name, _value)
                    CASEITEM(in, 11, _name, _value)
                    CASEITEM(in, 12, _name, _value)
                    CASEITEM(in, 13, _name, _value)
                    CASEITEM(in, 14, _name, _value)
                    default: break;
                }
                in_index++;
                break;
            }
            default: break;
        }
    }

    return sql.Insert();
}

/******************************************************************************/
int32 ConfigHostHwmon::Delete(const int64 hid)
{
    SqlCommand sql(_db,  _table);
    sql.Add("hid",  hid);

    ostringstream where ;
    where << " WHERE hid = " << hid;        
    return sql.Delete(where.str());
}

/******************************************************************************/
// 调试函数的地盘

}
