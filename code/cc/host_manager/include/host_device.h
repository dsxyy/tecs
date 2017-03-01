/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�host_device.h
* �ļ���ʶ��
* ����ժҪ��host_device��������ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�ҦԶ
* ������ڣ�2012��11��1��
*
* �޸ļ�¼1��
*     �޸����ڣ�2012/11/1
*     �� �� �ţ�V1.0
*     �� �� �ˣ�ҦԶ
*     �޸����ݣ�����
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

