/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：vmtemplate_pool.h
* 文件标识：见配置管理计划书
* 内容摘要：vmtemplate_pool类的定义文件
* 当前版本：1.0
* 作    者：Bob
* 完成日期：2011年7月26日
*
* 修改记录1：
*    修改日期：2011/7/26
*    版 本 号：V1.0
*    修 改 人：Bob
*    修改内容：创建
*******************************************************************************/

#ifndef VMTEMPLATE_MANGER_VMTEMPLATE_POOL_H        
#define VMTEMPLATE_MANGER_VMTEMPLATE_POOL_H

#include "vmtemplate.h"
#include "sky.h"
#include "vm_messages.h"
#include "pool_sql.h"


namespace zte_tecs
{

/**
@brief 功能描述: 模版资源池的实体类\n
@li @b 其它说明: 无
*/
class VmTemplatePool : public SqlCallbackable
{

/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    static VmTemplatePool* GetInstance()
    {
        SkyAssert(NULL != _instance);
        return _instance;
    };

    static VmTemplatePool* CreateInstance(SqlDB *db)
    {
        if(NULL == _instance)
        {
            _instance = new VmTemplatePool(db); 
        }
        
        return _instance;
    };

     ~VmTemplatePool(){};

    static void Lock()
    {
        pthread_mutex_lock(&_mutex);
    };

    static void UnLock()
    {
        pthread_mutex_unlock(&_mutex);
    };
    
    STATUS Init();

    /**
     * Function to allocate a new VmTemplate object
     *  @param uid the user id of the image's owner
     *  @param stemplate template associated with the image
     *  @param oid the id assigned to the VmTemplate
     *  @return the oid assigned to the object,
     *-1 in case of failure
     *-2 in case of template parse failure
     */

    int Allocate (VmTemplate &vt);

    int Drop(VmTemplate * vt);

    int Update(VmTemplate &vt);
    /**
     *Function to get a VmTemplate from the pool, if the object is not in memory
     *it is loaded from the DB
     *  @param oid VmTemplate unique id
     *  @param lock locks the VmTemplate mutex
     *  @return a pointer to the VmTemplate, 0 if the VmTemplate could not be loaded
     */
    STATUS GetVmTemplate(VmTemplate &vt, int64 oid);

    STATUS GetVmTemplate(VmTemplate &vt,const string &name);
   
    int SelectCallback(void * nil, SqlColumn * columns);
  
    int GetTids(vector<int64> &tids, const string& where);

    int GetTidsCallback(void * tids, SqlColumn *pColumns);
    
    int Show(vector<ApiVtInfo> &vec_vt_info, const string& where);

    STATUS ToApiInfo(VmTemplate & vt, ApiVtInfo &msg);

/*******************************************************************************
* 2. protected
*******************************************************************************/
protected:
    enum ColNames
    {
         OID = 0,   
         NAME  = 1,   
         DESCRIPTION = 2, 
         CREATE_TIME = 3,
         VCPU  = 4, 
         TCU = 5,
         MEMORY = 6, 
         VIRT_TYPE = 7,
         ROOT_DEVICE = 8,
         KERNEL_COMMAND = 9,
         BOOTLOADER  = 10,
         RAW_DATA = 11,
         VNC_PASSWORD = 12,
         QUALIFICATION  = 13,
         WATCHDOG = 14,
         APPOINTED_CLUSTER = 15,
         APPOINTED_HOST = 16, 
         UID = 17,
         IS_PUBLIC = 18,
         ENABLE_COREDUMP = 19,
         ENABLE_SERIAL = 20,
         HYPERVISOR    = 21,
         ENABLE_LIVEMIGRATE = 22,
         LIMIT = 23
    };
    
    enum ViewDiskImageColNames 
    {
         VIEW_VMTEMPLATE_ID = 0,
         VIEW_IMAGE   = 1,
         VIEW_DISK  = 2,
         VIEW_TOTAL   = 3,
         VIEW_DISKIMAGE_LIMIT= 4 
    };

/*******************************************************************************
* 3. private
*******************************************************************************/
private:

    VmTemplatePool(SqlDB *pDb):SqlCallbackable(pDb){ };
    static VmTemplatePool  *_instance;
    static int64            _lastOID;
    static pthread_mutex_t  _mutex;
    
    static const char      *_table_vmtemplate;
    static const char      *_view_vmtemplate_disk_image_size;
    
    static const char      *_col_names_vmtemplate;
    static const char      *_col_names_view_vmtemplate_disk_image_size;   
     

    DISALLOW_COPY_AND_ASSIGN(VmTemplatePool); 

};

}
#endif /* IMAGE_MANGER_VMTEMPLATE_POOL_H */

