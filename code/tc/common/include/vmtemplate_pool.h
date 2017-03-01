/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�vmtemplate_pool.h
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��vmtemplate_pool��Ķ����ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�Bob
* ������ڣ�2011��7��26��
*
* �޸ļ�¼1��
*    �޸����ڣ�2011/7/26
*    �� �� �ţ�V1.0
*    �� �� �ˣ�Bob
*    �޸����ݣ�����
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
@brief ��������: ģ����Դ�ص�ʵ����\n
@li @b ����˵��: ��
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

