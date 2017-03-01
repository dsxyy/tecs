/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：host_device.h
* 文件标识：
* 内容摘要：host_device类的声明文件
* 当前版本：1.0
* 作    者：姚远
* 完成日期：2012年11月1日
*
* 修改记录1：
*     修改日期：2012/11/1
*     版 本 号：V1.0
*     修 改 人：姚远
*     修改内容：创建
*******************************************************************************/
#ifndef HM_HOST_DEVICE_H
#define HM_HOST_DEVICE_H

#include "sky.h"
#include "object_sql.h"
#include "msg_host_manager_with_host_agent.h"

using namespace std;
namespace zte_tecs {

class HostDevicePool: public Callbackable
{
public:
    static HostDevicePool* GetInstance(SqlDB *db = NULL)
    {   
        if(NULL == _instance)
        {
            SkyAssert(NULL != db);
            _instance = new HostDevicePool(db);  
        }
        
        return _instance;
    };
    
    int SelectCallback(void *nil, SqlColumn * columns);
    int GetHostDevices(vector<PciPthDevInfo> &devices, int64 hid);
    int InsertHostDevices(const vector<PciPthDevInfo> &devices, int64 hid);
    int DropHostDevices(const vector<PciPthDevInfo> &devices, int64 hid);
    int DropHostDevice(const PciPthDevInfo &device, int64 hid);

private:
    enum ColNames 
    {
        HID             = 0,
        TYPE            = 1,
        NUMBER          = 2,
        DESCRIPTION     = 3,
        LIMIT           = 4,
    };
    
    HostDevicePool(SqlDB *pDb)
    {
        db = pDb;
    };
    
    //DISALLOW_COPY_AND_ASSIGN(HostDevicePool);
    static HostDevicePool *_instance;
    SqlDB              *db;
    static const char *  _table_name;
    
    static const char *  _col_names;    
};

}  /* end namespace zte_tecs */

#endif /* end HM_HOST_H */

