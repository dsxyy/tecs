/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：vmtemplate_pool.cpp
* 文件标识：见配置管理计划书
* 内容摘要：vmtemplate类的实现文件
* 当前版本：1.0
* 作    者：Bob
* 完成日期：2011年7月28日
*
* 修改记录1：
*    修改日期：2011/7/28
*    版 本 号：V1.0
*    修 改 人：Bob
*    修改内容：创建
*******************************************************************************/
#include "vmtemplate_pool.h"
#include "log_agent.h"
#include "user_pool.h"

namespace zte_tecs
{
#define ERROR_RETURN  do\
          {\
            Transaction::Cancel();\
            SkyAssert(false);\
            return ERROR; \
          }while(0)

    VmTemplatePool *VmTemplatePool::_instance = NULL;
    int64           VmTemplatePool::_lastOID = INVALID_OID;
    pthread_mutex_t VmTemplatePool::_mutex;


    const char    * VmTemplatePool::_table_vmtemplate  =
            "vmtemplate_pool"; //虚拟机表

    const char    * VmTemplatePool::_view_vmtemplate_disk_image_size =
             "view_vmtemplate_disk_image_size";

    const char   * VmTemplatePool::_col_names_vmtemplate =
            "oid,"
    	    "name,"
    	    "description, "
    	    "create_time,"
    	    "vcpu,"
    	    "tcu,"
    	    "memory,"
    	    "virt_type,"
    	    "root_device,"
    	    "kernel_command,"
    	    "bootloader,"
    	    "raw_data,"
    	    "vnc_password,"
    	    "qualification,"
    	    "watchdog,"
    	    "appointed_cluster,"
    	    "appointed_host,"
    	    "uid,"
    	    "is_public,"
            "enable_coredump,"
            "enable_serial,"
	     "hypervisor,"
             "enable_livemigrate";

    const char * VmTemplatePool::_col_names_view_vmtemplate_disk_image_size =
            "vmtemplate_id,"
            "total_image_size,"
            "total_disk_size,"
            "total_size";

STATUS VmTemplatePool::Init()
{
    int64    tmp_oid = 0;
    string   where =" 1=1 ";
    string   column = " MAX(oid) ";
    ostringstream   sql_delete;
    int             rc;

    // 1.获取最大_oid
    SqlCallbackable sqlcb(_db);
    rc = sqlcb.SelectColumn( _table_vmtemplate, column, where, tmp_oid);
    if((SQLDB_OK != rc) &&(SQLDB_RESULT_EMPTY != rc))
    {
            OutPut(SYS_DEBUG, "*** Select record from vmtemplate_pool failed ***\n");
            return ERROR_DB_SELECT_FAIL;
    }

    // 2. 删除uid为NULL的 模板
    sql_delete<< "DELETE FROM " << _table_vmtemplate<< " WHERE uid is NULL" ;

    rc = _db->Execute(sql_delete);
    if ( rc != 0 )
    {
        OutPut(SYS_ALERT, "[%s] Execute failed!", sql_delete.str().c_str());
    }
    _lastOID = tmp_oid;

    // 3. 初始化互斥锁
    pthread_mutex_init(&_mutex,0);

    return SUCCESS;

}

/******************************************************************************/
int VmTemplatePool::Allocate (VmTemplate  &vt)
{
    ostringstream   oss;
    int             rc;

    Lock();
    vt._oid = ++_lastOID;
    UnLock();

    Transaction::Begin();

    // 1. vmtemplate_pool表
    SqlCommand sql(_db, _table_vmtemplate, SqlCommand::WITH_TRANSACTION);
    sql.Add("oid",              vt._oid);
    sql.Add("name",             vt._name);
    sql.Add("description",      vt._description);
    sql.Add("create_time",      vt._create_time);
    sql.Add("vcpu",             vt._vcpu);
    sql.Add("tcu",              vt._tcu);
    sql.Add("memory",           vt._memory);
    sql.Add("virt_type",        vt._virt_type);
    sql.Add("root_device",      vt._root_device);
    sql.Add("kernel_command",   vt._kernel_command);
    sql.Add("bootloader",       vt._bootloader);
    sql.Add("raw_data",         vt._raw_data);
    sql.Add("vnc_password",     vt._vnc_passwd);
    sql.Add("qualification",    vt._qualifications);
    sql.Add("watchdog",         vt._watchdog);
    sql.Add("appointed_cluster",vt._appointed_cluster);
    sql.Add("appointed_host",   vt._appointed_host);

    sql.Add("is_public",        vt._is_public);
    sql.Add("uid",              vt._uid);
    sql.Add("enable_coredump",   vt._enable_coredump);
    sql.Add("enable_serial",   vt._enable_serial);
    sql.Add("hypervisor",      vt._hypervisor);
    sql.Add("enable_livemigrate",  vt._enable_livemigrate);

    rc = sql.Insert();
    if(0 != rc)
    {
        ERROR_RETURN;
    }

    // 2. vmtemplate_image 表
    VmBaseImagePool *p_image = VmBaseImagePool::GetInstance(_db);
    if (NULL == p_image)
    {
        ERROR_RETURN;
    }

    rc = p_image->InsertVmBaseImages(vt._images, vt._oid, false);
    if(0 != rc)
    {
        ERROR_RETURN;
    }

    // 3. vmtemplate_disk 表
    VmBaseDiskPool *p_disk = VmBaseDiskPool::GetInstance(_db);
    if (NULL == p_disk)
    {
        ERROR_RETURN;
    }

    rc = p_disk->InsertVmBaseDisks(vt._disks, vt._oid, false);
    if(0 != rc)
    {
        ERROR_RETURN;
    }

    // 4. vmtemplate_nic 表
    VmBaseNicPool *p_nic = VmBaseNicPool::GetInstance(_db);
    if (NULL == p_nic)
    {
        ERROR_RETURN;
    }

    rc = p_nic->InsertVmBaseNics(vt._nics, vt._oid, false);
    if(0 != rc)
    {
        ERROR_RETURN;
    }

    // 6. vmtemplate_context 表
    VmBaseContextPool *p_context = VmBaseContextPool::GetInstance(_db);
    if (NULL == p_context)
    {
        ERROR_RETURN;
    }

    rc = p_context->InsertVmBaseContext(vt._context, vt._oid, false);
    if(0 != rc)
    {
       ERROR_RETURN;
    }

    // 7. vmcfg_device 表
    VmBaseDevicePool *p_device_pool = VmBaseDevicePool::GetInstance(_db);
    if (NULL == p_device_pool)
    {
        ERROR_RETURN;
    }

    rc = p_device_pool->InsertVmDevices(vt._devices, vt._oid, false);
    if(0 != rc)
    {
        ERROR_RETURN;
    }

    return Transaction::Commit();

};

/******************************************************************************/
int VmTemplatePool::Drop(VmTemplate  *vt)
{
    int rc;
    ostringstream   sql_delete;
    sql_delete<< "DELETE FROM " << _table_vmtemplate<< " WHERE oid = "<<vt->_oid;
    rc = _db->Execute(sql_delete);
    return rc;
};


/******************************************************************************/
int VmTemplatePool::Update(VmTemplate  &vt)
{
    int             rc;

    Transaction::Begin();

    // 1. vmtemplate_pool表
    SqlCommand sql(_db, _table_vmtemplate, SqlCommand::WITH_TRANSACTION);
    sql.Add("oid",              vt._oid);
    sql.Add("name",             vt._name);
    sql.Add("description",      vt._description);
    sql.Add("create_time",      vt._create_time);
    sql.Add("vcpu",             vt._vcpu);
    sql.Add("tcu",              vt._tcu);
    sql.Add("memory",           vt._memory);
    sql.Add("virt_type",        vt._virt_type);
    sql.Add("root_device",      vt._root_device);
    sql.Add("kernel_command",   vt._kernel_command);
    sql.Add("bootloader",       vt._bootloader);
    sql.Add("raw_data",         vt._raw_data);
    sql.Add("vnc_password",     vt._vnc_passwd);
    sql.Add("qualification",    vt._qualifications);
    sql.Add("watchdog",         vt._watchdog);
    sql.Add("appointed_cluster",vt._appointed_cluster);
    sql.Add("appointed_host",   vt._appointed_host);

    sql.Add("is_public",        vt._is_public);

    sql.Add("uid",  vt._uid);
    sql.Add("enable_coredump",   vt._enable_coredump);
    sql.Add("enable_serial",vt._enable_serial);
    sql.Add("hypervisor",      vt._hypervisor);
    sql.Add("enable_livemigrate",  vt._enable_livemigrate);

    rc = sql.Update("WHERE oid = " + to_string(vt._oid,dec));
    if(0 != rc)
    {
        SkyAssert(false);
        return rc;
    }

    // 2. vmtemplate_image 表
    VmBaseImagePool *p_image = VmBaseImagePool::GetInstance(_db);
    if (NULL == p_image)
    {
        Transaction::Cancel();
        SkyAssert(false);
        return rc;
    }

    rc = p_image->SetVmBaseImages(vt._images, vt._oid, false);
    if(0 != rc)
    {
        Transaction::Cancel();
        SkyAssert(false);
        return rc;
    }

    // 3. vmtemplate_disk 表
    VmBaseDiskPool *p_disk = VmBaseDiskPool::GetInstance(_db);
    if (NULL == p_disk)
    {
        Transaction::Cancel();
        SkyAssert(false);
        return rc;
    }

    rc = p_disk->SetVmBaseDisks(vt._disks, vt._oid, false);
    if(0 != rc)
    {
        Transaction::Cancel();
        SkyAssert(false);
        return rc;
    }

    // 4. vmtemplate_nic 表
    VmBaseNicPool *p_nic = VmBaseNicPool::GetInstance(_db);
    if (NULL == p_nic)
    {
        Transaction::Cancel();
        SkyAssert(false);
        return rc;
    }

    rc = p_nic->SetVmBaseNics(vt._nics, vt._oid, false);
    if(0 != rc)
    {
        Transaction::Cancel();
        SkyAssert(false);
        return rc;
    }

    // 5. vmtemplate_context 表
    VmBaseContextPool *p_context = VmBaseContextPool::GetInstance(_db);
    if (NULL == p_context)
    {
        Transaction::Cancel();
        SkyAssert(false);
        return rc;
    }

    rc = p_context->SetVmBaseContext(vt._context, vt._oid, false);
    if(0 != rc)
    {
        Transaction::Cancel();
        SkyAssert(false);
        return rc;
    }

    // 6. vmcfg_device 表
    VmBaseDevicePool *p_device_pool = VmBaseDevicePool::GetInstance(_db);
    if (NULL == p_device_pool)
    {
        Transaction::Cancel();
        SkyAssert(false);
        return rc;
    }

    rc = p_device_pool->SetVmDevices(vt._devices, vt._oid, false);
    if(0 != rc)
    {
        Transaction::Cancel();
        SkyAssert(false);
        return rc;
    }

    return Transaction::Commit();
};

/******************************************************************************/
STATUS VmTemplatePool::GetVmTemplate(VmTemplate &vt, int64 oid)
{
    ostringstream   oss;
    int             rc;
    int64           tmp_oid;

    // 1. 查 vmtemplate_pool
    SetCallback(static_cast<Callbackable::Callback>(&VmTemplatePool::SelectCallback),static_cast<void *>(&vt));

    oss << "SELECT " << _col_names_vmtemplate
        << " FROM " << _table_vmtemplate
        << " WHERE oid = " << oid;

    tmp_oid = oid;
    vt._oid = INVALID_OID;

    rc = _db->Execute(oss, this);

    UnsetCallback();
    if ( (SQLDB_OK != rc) && (SQLDB_RESULT_EMPTY != rc)  )
    {
        //数据库系统出错
        SkyAssert(false);
        return rc;
    }

    if ((vt._oid != tmp_oid ) || (SQLDB_RESULT_EMPTY == rc) )
    {
        //数据库系统没问题，但是没找到记录
        return -1;
    }

    // 2. 查 vmtemplate_image 表
    VmBaseImagePool *p_image = VmBaseImagePool::GetInstance(_db);
    if (NULL == p_image)
    {
        SkyAssert(false);
        return -1;
    }

    rc = p_image->GetVmBaseImages(vt._images, oid, false);
    if ( (SQLDB_OK!= rc)
          && (SQLDB_RESULT_EMPTY != rc) )
    {
        SkyAssert(false);
        return rc;
    }
       // _view_vmtemplate_image
    rc = p_image->GetImagesSize(oid, false,VM_STORAGE_TO_LOCAL,vt._local_images_size);
    if ( (SQLDB_OK!= rc)
          && (SQLDB_RESULT_EMPTY != rc) )
    {
        SkyAssert(false);
        return rc;
    }

    rc = p_image->GetImagesSize(oid, false,VM_STORAGE_TO_SHARE,vt._share_images_size);
    if ( (SQLDB_OK!= rc)
          && (SQLDB_RESULT_EMPTY != rc) )
    {
        SkyAssert(false);
        return rc;
    }

    // 3. 查 vmtemplate_disk 表
    VmBaseDiskPool  *p_disk = VmBaseDiskPool::GetInstance(_db);
    if (NULL == p_disk)
    {
        SkyAssert(false);
        return -1;
    }

    rc = p_disk->GetVmBaseDisks(vt._disks, oid, false);
    if ( (SQLDB_OK!= rc)
          && (SQLDB_RESULT_EMPTY != rc) )
    {
        SkyAssert(false);
        return rc;
    }
       // _view_vmtemplate_image
    rc = p_disk->GetDisksSize(oid, true,VM_STORAGE_TO_LOCAL, vt._local_disks_size);
    if ( (SQLDB_OK!= rc)
          && (SQLDB_RESULT_EMPTY != rc) )
    {
        SkyAssert(false);
        return rc;
    }

    rc = p_disk->GetDisksSize(oid, true,VM_STORAGE_TO_SHARE, vt._share_disks_size);
    if ( (SQLDB_OK!= rc)
          && (SQLDB_RESULT_EMPTY != rc) )
    {
        SkyAssert(false);
        return rc;
    }

    // 4. vmtemplate_nic 表
    VmBaseNicPool *p_nic = VmBaseNicPool::GetInstance(_db);
    if (NULL == p_disk)
    {
        SkyAssert(false);
        return -1;
    }

    rc = p_nic->GetVmBaseNics(vt._nics, oid, false);
    if ( (SQLDB_OK!= rc)
          && (SQLDB_RESULT_EMPTY != rc) )
    {
        SkyAssert(false);
        return rc;
    }


    // 5. 查 vmtemplate_context 表
    VmBaseContextPool  *p_context = VmBaseContextPool::GetInstance(_db);
    if (NULL == p_context)
    {
        SkyAssert(false);
        return -1;
    }

    rc = p_context->GetVmBaseContext(vt._context,oid, false);
    if ( (SQLDB_OK!= rc)
          && (SQLDB_RESULT_EMPTY != rc) )
    {
        SkyAssert(false);
        return rc;
    }

    // 6. vmcfg_device 表
    VmBaseDevicePool *p_device_pool = VmBaseDevicePool::GetInstance(_db);
    if (NULL == p_device_pool)
    {
        SkyAssert(false);
        return -1;
    }

    rc = p_device_pool->GetVmDevices(vt._devices, oid, false);
    if ( (SQLDB_OK!= rc)
          && (SQLDB_RESULT_EMPTY != rc) )
    {
        SkyAssert(false);
        return rc;
    }

    return 0;
}


/******************************************************************************/
STATUS  VmTemplatePool::GetVmTemplate(VmTemplate &vt,const string &name)
{
    ostringstream   oss;
    vector<int64> tids;

    oss << "name = '"<<name <<"'";
    GetTids(tids, oss.str());

    if (tids.size() == 0)
    {
        return  SQLDB_RESULT_EMPTY;
    }

    return GetVmTemplate(vt, tids[0]);
}




/******************************************************************************/
int VmTemplatePool::SelectCallback(void * nil, SqlColumn * columns)
{

    VmTemplate *vt;
    if (NULL == columns ||
        LIMIT != columns->get_column_num())
    {
        return -1;
    }

    vt = static_cast<VmTemplate *>(nil);

    columns->GetValue(OID,  vt->_oid);
    columns->GetValue(NAME, vt->_name);
    columns->GetValue(DESCRIPTION, vt->_description);
    columns->GetValue(CREATE_TIME, vt->_create_time);
    columns->GetValue(VCPU,    vt->_vcpu);
    columns->GetValue(TCU,     vt->_tcu);
    columns->GetValue(MEMORY,  vt->_memory);

    columns->GetValue(VIRT_TYPE,     vt->_virt_type);
    columns->GetValue(ROOT_DEVICE,    vt->_root_device);
    columns->GetValue(KERNEL_COMMAND, vt->_kernel_command);
    columns->GetValue(BOOTLOADER,     vt->_bootloader);
    columns->GetValue(RAW_DATA,       vt->_raw_data);
    columns->GetValue(VNC_PASSWORD,   vt->_vnc_passwd);
    columns->GetValue(QUALIFICATION,  vt->_qualifications);
    columns->GetValue(WATCHDOG,       vt->_watchdog);
    columns->GetValue(APPOINTED_CLUSTER, vt->_appointed_cluster);
    columns->GetValue(APPOINTED_HOST,    vt->_appointed_host);

    int32  tmp_int = 0;
    columns->GetValue(IS_PUBLIC,        tmp_int);
    vt->_is_public = static_cast<bool>(tmp_int);

    columns->GetValue(UID,  vt->_uid);
    columns->GetValue(ENABLE_COREDUMP,    vt->_enable_coredump);
    columns->GetValue(ENABLE_SERIAL, vt->_enable_serial);
    columns->GetValue(HYPERVISOR,     vt->_hypervisor);
    columns->GetValue(ENABLE_LIVEMIGRATE,     vt->_enable_livemigrate);

    return 0;
}




/******************************************************************************/
int VmTemplatePool::GetTids(vector<int64> &tids, const string& where)
{
    ostringstream   sql;
    int             rc;

    SetCallback(static_cast<Callbackable::Callback>(&VmTemplatePool::GetTidsCallback),static_cast<void *>(&tids));

    sql  << "SELECT oid FROM " <<  _table_vmtemplate << " WHERE " << where;

    rc = _db->Execute(sql, this);

    UnsetCallback();

    if(SQLDB_RESULT_EMPTY == rc)
    {
        return ERROR_OBJECT_NOT_EXIST;
    }

    if(rc != SQLDB_OK)
    {
        return ERROR_DB_SELECT_FAIL;
    }

    return SUCCESS;

}

/******************************************************************************/
int VmTemplatePool:: GetTidsCallback(void * tids, SqlColumn *pColumns)
{
    vector<int64> *  tids_vector;

    tids_vector = static_cast<vector<int64> *>(tids);

    if ( pColumns->get_column_num() == 0 )
    {
        return -1;
    }

    tids_vector->push_back(pColumns->GetValueInt64(0));

    return 0;
}

/******************************************************************************/
int VmTemplatePool::Show(vector<ApiVtInfo> &vec_vt_info, const string& where)
{

    int                     rc;
    vector<int64>           tids;
    vector<int64>::iterator it;
    VmTemplate             vmtemplate;
    STATUS                  tRet = ERROR;

    rc = GetTids(tids, where);

    if ((SUCCESS != rc)
         &&(ERROR_OBJECT_NOT_EXIST != rc))
    {
        return ERROR;
    }

    for (it = tids.begin();  it != tids.end(); it++)
    {
        tRet = GetVmTemplate(vmtemplate,*it);
        if (SUCCESS!= tRet)
        {
           return ERROR;
        }
        ApiVtInfo          info;
        ToApiInfo(vmtemplate,info);

        vec_vt_info.push_back(info);
    }

    return SUCCESS;
}

/******************************************************************************/
STATUS VmTemplatePool::ToApiInfo(VmTemplate  &vmtemp, ApiVtInfo &info)
{
    ostringstream   oss;
    int             rc = -1;

    info.vt_id            = vmtemp._oid;
    info.vt_name          = vmtemp._name;
    info.uid              = vmtemp._uid;
    info.create_time      = vmtemp._create_time.serialize();
    info.is_public        = vmtemp._is_public;

    UserPool *pup = UserPool::GetInstance();
    if(pup != NULL)
    {
        pup->GetUserNameByID(vmtemp._uid, info.user_name);
    }

    info.cfg_info.base_info.description = vmtemp._description;

    info.cfg_info.base_info.vcpu   = vmtemp._vcpu;
    info.cfg_info.base_info.tcu    = vmtemp._tcu;
    info.cfg_info.base_info.memory = vmtemp._memory;

   // info.base_info.disk_size      = vmtemp.get_total_size();
    info.cfg_info.base_info.virt_type      = vmtemp._virt_type;
    info.cfg_info.base_info.root_device    = vmtemp._root_device;
    info.cfg_info.base_info.kernel_command = vmtemp._kernel_command;
    info.cfg_info.base_info.bootloader     = vmtemp._bootloader;
    info.cfg_info.base_info.rawdata        = vmtemp._raw_data;

    info.cfg_info.base_info.vnc_passwd     = vmtemp._vnc_passwd;
    info.cfg_info.base_info.enable_coredump = vmtemp._enable_coredump;
    info.cfg_info.base_info.vm_wdtime      = vmtemp._watchdog;
    info.cfg_info.base_info.cluster        = vmtemp._appointed_cluster;
    info.cfg_info.base_info.host           = vmtemp._appointed_host;
    info.cfg_info.base_info.enable_serial  = vmtemp._enable_serial;
    info.cfg_info.base_info.hypervisor    = vmtemp._hypervisor;
    info.cfg_info.base_info.enable_livemigrate  = vmtemp._enable_livemigrate;
    info.cfg_info.base_info.context        = vmtemp._context._str_map;

    // 1. 部署策略
    vector<Expression>::const_iterator vec_it_exp;
    for (vec_it_exp = vmtemp._qualifications._qua.begin();
         vec_it_exp != vmtemp._qualifications._qua.end();
         ++vec_it_exp)
    {
        ApiVmExpression   ap_exp(vec_it_exp->_key,
                                 vec_it_exp->_op,
                                 vec_it_exp->_value);

        info.cfg_info.base_info.qualifications.push_back(ap_exp);
     }

    // 2. disk
    vector<VmBaseDisk>::const_iterator  it_disk;
    for (it_disk = vmtemp._disks.begin();
         it_disk !=vmtemp._disks.end();
         it_disk++ )
    {
       ApiVmDisk api_disk(INVALID_OID,
                               it_disk->_position,
                               it_disk->_size,
                               it_disk->_bus,
                               DISK_TYPE_DISK,
                               it_disk->_target,
                               it_disk->_fstype);
        info.cfg_info.base_info.disks.push_back(api_disk);
    }

    // 3. 查 image 视图
    VmBaseImagePool  *p_image = VmBaseImagePool::GetInstance(_db);

    vector<VmBaseImageInfo>            image_info;
    vector<VmBaseImageInfo>::iterator  it_image;
    rc = p_image->GetImageInfoByView(image_info, vmtemp._oid, false);
    if ( (SQLDB_OK!= rc)
          && (SQLDB_RESULT_EMPTY != rc) )
    {
        SkyAssert(false);
        return rc;
    }

    for (it_image = image_info.begin();
         it_image != image_info.end();
         it_image++)
    {
        if (DISK_TYPE_MACHINE == it_image->_type)
        {
           info.cfg_info.base_info.machine.id       = it_image->_image_id;
           info.cfg_info.base_info.machine.position = it_image->_position;
           info.cfg_info.base_info.machine.size     = it_image->_size;
           info.cfg_info.base_info.machine.type     = it_image->_type;
           info.cfg_info.base_info.machine.bus      = it_image->_bus;
           info.cfg_info.base_info.machine.target   = it_image->_target;
           info.cfg_info.base_info.machine.reserved_backup = it_image->_reserved_backup;
        }
        else
        {
            ApiVmDisk api_disk;
            api_disk.id       = it_image->_image_id;
            api_disk.size     = it_image->_size;
            api_disk.type     = it_image->_type;
            api_disk.bus      = it_image->_bus;
            api_disk.target   = it_image->_target;
            api_disk.position = it_image->_position;
            api_disk.fstype   = "";
            api_disk.reserved_backup = it_image->_reserved_backup;
            info.cfg_info.base_info.disks.push_back(api_disk);
        }
    }


    // 4. 查 nic 视图
    VmBaseNicPool      *p_nic = VmBaseNicPool::GetInstance(_db);
    vector<VmBaseNic>  vmbase_nics;
    vector<VmBaseNic>::iterator  iter;
    rc = p_nic->GetVmBaseNics(vmbase_nics, vmtemp._oid, false);

    if((SQLDB_OK!= rc)
        &&(SQLDB_RESULT_EMPTY != rc))
    {
        SkyAssert(false);
        return rc;
    }

    for(iter = vmbase_nics.begin();
        iter!= vmbase_nics.end();
        iter++)
    {
       ApiVmNic  api_nic(iter->_nic_index,
                         iter->_sriov,
                         iter->_loop,
                         iter->_logic_network_id,
                         iter->_model,
                         iter->_vsi_profile_id);

       info.cfg_info.base_info.nics.push_back(api_nic);
    }

    //info._base_info.devices = vmtemp._devices;
    vector<VmDeviceConfig>::const_iterator it_dev;
    for (it_dev = vmtemp._devices.begin();
         it_dev != vmtemp._devices.end();
         ++it_dev)
    {
         ApiVmDevice api_dev;
         api_dev.type = it_dev->_type;
         api_dev.count = it_dev->_count;
         info.cfg_info.base_info.devices.push_back(api_dev);
    }

    return SUCCESS;
}

/******************************************************************************/

}



