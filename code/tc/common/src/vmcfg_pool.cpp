/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：vmcfg_pool.cpp
* 文件标识：见配置管理计划书
* 内容摘要：VmCfg类的实现文件
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
#include "vmcfg_pool.h"
#include "log_agent.h"
#include "vmcfgbase.h"
#include "tecs_config.h"
#include "vm_messages.h"
#include "workflow.h"
#include "affinity_region_pool.h"
#include "vmcfg_manager.h"

static int vp_print = 1;
DEFINE_DEBUG_VAR(vp_print);
#define Dbg_Prn(fmt,arg...) \
        do \
        { \
            if(vp_print) \
            { \
                printf("[%s:%d]"fmt,__func__,__LINE__, ##arg); \
            } \
        }while(0)

#define ERROR_RETURN  do\
          {\
            Transaction::Cancel();\
            SkyAssert(false);\
            return ERROR; \
          }while(0)

namespace zte_tecs
{


VmCfgPool *VmCfgPool::_instance = NULL;

int64      VmCfgPool::_lastOID = INVALID_OID;

pthread_mutex_t    VmCfgPool::_mutex ;

const char * VmCfgPool::_table_vmcfg  =
        "vmcfg_pool"; //虚拟机表

const char * VmCfgPool::_table_vmcfg_deleted =
        "deleted_vmcfg_pool"; //已删除的虚拟机列表


const char * VmCfgPool::_table_vmcfg_repel =
        "vmcfg_repel" ;

const char * VmCfgPool::_table_vmcfg_affinity_region =
        "affinity_region_vmcfg";

const char * VmCfgPool::_view_vmcfg_repel =
        "view_vmcfg_repel" ;

const char * VmCfgPool::_view_vmcfg_with_project =
        "view_vmcfg_with_project";

const char * VmCfgPool::_view_project_with_cluster =
         "view_project_with_cluster";

const char * VmCfgPool::_view_vmcfg_disk_image_size =
         "view_vmcfg_disk_image_size";

const char * VmCfgPool::_view_vmcfg_with_affinity_region =
         "view_vmcfg_with_affinity_region";

const char * VmCfgPool::_view_repel_with_affinity_region =
         "view_repel_with_affinity_region";

const char * VmCfgPool::_col_names_repel_with_affinity_region =
         "affinity_region_id, "
         "vid1, "
         "vid2, "
         "level";
const char * VmCfgPool::_col_names_affinity_region =
         "affinity_region_id, "
         "vid";

const char *VmCfgPool:: _col_names_vmcfg_with_affinity_region =
         "vid, "
         "affinity_region_id, "
         "name, "
         "description, "
         "create_time, "
         "uid, "
         "level, "
         "is_forced";


const char * VmCfgPool::_col_names_vmcfg =
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
	    "appointed_cluster,"
	    "appointed_host,"
	    "project_id,"
	    "deployed_cluster,"
	    "config_version,"
	    "enable_coredump"
	    "enable_serial";
const char * VmCfgPool::_col_names_vmcfg_repel =
        "vid1,"
        "vid2";

const char * VmCfgPool::_col_names_view_vmcfg_repel =
        "vid1,"
        "uid1,"
        "vid2,"
        "uid2";

const char * VmCfgPool::_col_names_view_vmcfg_with_project =
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
	    "project_id,"
	    "deployed_cluster,"
	    "config_version,"
        "project_name,"
        "uid,"
        "user_name,"
        "enable_coredump,"
        "enable_serial,"
        "hypervisor,"
        "enable_livemigrate";

const char * VmCfgPool::_col_names_view_project_with_cluster =
        "project_id,"
        "project_name,"
        "cluster_id,"
        "cluster_name";

const char * VmCfgPool::_col_names_view_vmcfg_disk_image_size =
        "vid,"
        "total_image_size,"
        "total_disk_size,"
        "total_size";


const char* VmCfgPool::_view_vmcfg_full_info = "vmcfg_full_info";

vector<string>   VmCfgPool::_columns_full_info;

string           VmCfgPool::_columns_full_info_joined_string;

/******************************************************************************/
/* VmCfg :: Constructor/Destructor                                   */
/******************************************************************************/

STATUS VmCfgPool::Init(const bool& isReadOnly)
{
    if(false == isReadOnly)
    {
        int             rc = -1;
        int64           vmcfg_max_vid = 0;
        string  column  = " MAX(oid) ";
        string   where  = " 1=1 ";
        int _delete_max_vid = 0;
    
        // 1. 读取存活的vm表
        SqlCallbackable sql(_db);
        rc = sql.SelectColumn(_table_vmcfg,
                                    column,
                                    where,
                                    vmcfg_max_vid);
    
        if((SQLDB_OK != rc)
            &&(SQLDB_RESULT_EMPTY != rc))
        {
            OutPut(SYS_DEBUG, "*** Select record from vmcfg_pool failed ***\n");
            SkyAssert(false);
            return ERROR_DB_SELECT_FAIL;
        }
    
        // 2. 读取删除的vm表
        SqlCallbackable sql_delete(_db);
        rc = sql_delete.SelectColumn(_table_vmcfg_deleted,
                                    column,
                                    where,
                                    _delete_max_vid);
    
        if((SQLDB_OK != rc)
            &&(SQLDB_RESULT_EMPTY != rc))
        {
            OutPut(SYS_DEBUG, "*** Select record from vmcfg_delete_pool failed ***\n");
            SkyAssert(false);
            return ERROR_DB_SELECT_FAIL;
        }
    
        _lastOID = vmcfg_max_vid > _delete_max_vid? vmcfg_max_vid:_delete_max_vid;
    
        pthread_mutex_init(&_mutex,0);
    }

    if(!_columns_full_info.empty())
    {
        SkyAssert(false);
        return ERROR_DUPLICATED_OP;
    }

    _columns_full_info.push_back("vid");
    _columns_full_info.push_back("vm_name");
    _columns_full_info.push_back("uid");
    _columns_full_info.push_back("user_name");
    _columns_full_info.push_back("project_id");
    _columns_full_info.push_back("project_name");
    _columns_full_info.push_back("vcpu");
    _columns_full_info.push_back("tcu");
    _columns_full_info.push_back("memory");
    _columns_full_info.push_back("total_size");
    _columns_full_info.push_back("deployed_cluster");
    _columns_full_info.push_back("vm_create_time");
    _columns_full_info_joined_string = JoinStrings(_columns_full_info, ",");

    return SUCCESS;
}

/******************************************************************************/
STATUS VmCfgPool::Allocate( VmCfg   &vmcfg)
{
   // 参照 原版本中 VmCfg::Insert 的实现
    ostringstream   oss;
    int             rc = -1;
    Transaction::Begin();

    // 1. vmcfg_pool表
    SqlCommand sql(_db, _table_vmcfg, SqlCommand::WITH_TRANSACTION);

    Lock();
    vmcfg._oid = ++_lastOID;
    UnLock();

    sql.Add("oid",              vmcfg._oid);
    sql.Add("name",             vmcfg._name);
    sql.Add("description",      vmcfg._description);
    sql.Add("create_time",      vmcfg._create_time);
    sql.Add("vcpu",             vmcfg._vcpu);
    sql.Add("tcu",              vmcfg._tcu);
    sql.Add("memory",           vmcfg._memory);
    sql.Add("virt_type",        vmcfg._virt_type);
    sql.Add("root_device",      vmcfg._root_device);
    sql.Add("kernel_command",   vmcfg._kernel_command);
    sql.Add("bootloader",       vmcfg._bootloader);
    sql.Add("raw_data",         vmcfg._raw_data);
    sql.Add("vnc_password",     vmcfg._vnc_passwd);
    sql.Add("qualification",    vmcfg._qualifications);
    sql.Add("watchdog",         vmcfg._watchdog);

    if (!vmcfg._appointed_cluster.empty())
    {
        sql.Add("appointed_cluster",vmcfg._appointed_cluster);
    }
    sql.Add("appointed_host",   vmcfg._appointed_host);
    sql.Add("project_id",       vmcfg._project_id);
    sql.Add("config_version",   vmcfg._config_version);

    if (!vmcfg._deployed_cluster.empty())
    {
        sql.Add("deployed_cluster", vmcfg._deployed_cluster);
    }
    sql.Add("enable_coredump",    vmcfg._enable_coredump);
    sql.Add("enable_serial", vmcfg._enable_serial);
    sql.Add("hypervisor",       vmcfg._hypervisor);
    sql.Add("enable_livemigrate", vmcfg._enable_livemigrate);

    rc = sql.Insert();
    if(SQLDB_OK!= rc)
    {
        ERROR_RETURN;
    }

    // 2. vmcfg_image 表
    VmBaseImagePool *p_image = VmBaseImagePool::GetInstance(_db);
    if (NULL == p_image)
    {
        ERROR_RETURN;
    }

    rc = p_image->InsertVmBaseImages(vmcfg._images, vmcfg._oid, true);
    if(SQLDB_OK != rc)
    {
        ERROR_RETURN;
    }

    // 3. vmcfg_disk 表
    VmBaseDiskPool *p_disk = VmBaseDiskPool::GetInstance(_db);
    if (NULL == p_disk)
    {
        ERROR_RETURN;
    }

    rc = p_disk->InsertVmBaseDisks(vmcfg._disks,vmcfg._oid, true);
    if(SQLDB_OK != rc)
    {
        ERROR_RETURN;
    }

    // 4. vmcfg_nic 表
    VmBaseNicPool *p_nic = VmBaseNicPool::GetInstance(_db);
    if (NULL == p_nic)
    {
        ERROR_RETURN;
    }

    rc = p_nic->InsertVmBaseNics(vmcfg._nics, vmcfg._oid, true);
    if(SQLDB_OK != rc)
    {
        ERROR_RETURN;
    }


    // 7. vmcfg_context 表
    VmBaseContextPool *p_context = VmBaseContextPool::GetInstance(_db);
    if (NULL == p_context)
    {
        ERROR_RETURN;
    }

    rc = p_context->InsertVmBaseContext(vmcfg._context,vmcfg._oid, true);
    if(SQLDB_OK != rc)
    {
        ERROR_RETURN;
    }

    // 8. vmcfg_device 表
    VmBaseDevicePool *p_device_pool = VmBaseDevicePool::GetInstance(_db);
    if (NULL == p_device_pool)
    {
        ERROR_RETURN;
    }

    rc = p_device_pool->InsertVmDevices(vmcfg._devices, vmcfg._oid, true);
    if(0 != rc)
    {
        ERROR_RETURN;
    }
    return Transaction::Commit();

}

/******************************************************************************/
STATUS VmCfgPool::Drop(const VmCfg   &vmcfg)
{

    int           rc = -1;
    // 1. 查看是否已在删除表中

    rc = IsVmExistInDelTab(vmcfg._oid);

    if(SQLDB_OK == rc)
    {
        return ERR_VMCFG_EXIST_IN_DELETED_TAB;
    }
    else if(SQLDB_RESULT_EMPTY != rc)
    {
        return ERROR_DB_SELECT_FAIL;
    }

    Transaction::Begin();

    // 2. 插入虚拟机删除表
    SqlCommand sql_del(_db, _table_vmcfg_deleted, SqlCommand::WITH_TRANSACTION);
    sql_del.Add("oid",              vmcfg._oid);
    sql_del.Add("name",             vmcfg._name);
    sql_del.Add("description",      vmcfg._description);
    sql_del.Add("create_time",      vmcfg._create_time);
    sql_del.Add("vcpu",             vmcfg._vcpu);
    sql_del.Add("tcu",             vmcfg._tcu);
    sql_del.Add("memory",           vmcfg._memory);
    sql_del.Add("virt_type",        vmcfg._virt_type);
    sql_del.Add("root_device",      vmcfg._root_device);
    sql_del.Add("kernel_command",   vmcfg._kernel_command);
    sql_del.Add("bootloader",       vmcfg._bootloader);
    sql_del.Add("raw_data",         vmcfg._raw_data);
    sql_del.Add("vnc_password",     vmcfg._vnc_passwd);
    sql_del.Add("qualification",    vmcfg._qualifications);
    sql_del.Add("watchdog",        vmcfg._watchdog);
    sql_del.Add("appointed_cluster",vmcfg._appointed_cluster);
    sql_del.Add("appointed_host",   vmcfg._appointed_host);
    sql_del.Add("project_id",       vmcfg._project_id);
    sql_del.Add("config_version",   vmcfg._config_version);
    sql_del.Add("deployed_cluster", vmcfg._deployed_cluster);
    sql_del.Add("enable_coredump",    vmcfg._enable_coredump);
    sql_del.Add("enable_serial", vmcfg._enable_serial);

    rc = sql_del.Insert();
    if(SQLDB_OK != rc)
    {
        Transaction::Cancel();
        SkyAssert(false);
        return rc;
    }

    // 3. 从虚拟机存活表中删除
    SqlCommand sql(_db, _table_vmcfg, SqlCommand::WITH_TRANSACTION);

    sql.Delete(" WHERE oid =" +  to_string(vmcfg._oid,dec));

    rc = Transaction::Commit();

    return rc;
}

/******************************************************************************/
STATUS VmCfgPool::Update(VmCfg   &vmcfg)
{
  // 参照 原版本中 VmCfg::Update 的实现
    int             rc;
    //SqlDB * database;
    Transaction::Begin();

    // 1. vmcfg_pool表
    SqlCommand sql(_db, _table_vmcfg, SqlCommand::WITH_TRANSACTION);
    sql.Add("oid",              vmcfg._oid);
    sql.Add("name",             vmcfg._name);
    sql.Add("description",      vmcfg._description);
    sql.Add("create_time",      vmcfg._create_time);
    sql.Add("vcpu",             vmcfg._vcpu);
    sql.Add("tcu",              vmcfg._tcu);
    sql.Add("memory",           vmcfg._memory);
    sql.Add("virt_type",        vmcfg._virt_type);
    sql.Add("root_device",      vmcfg._root_device);
    sql.Add("kernel_command",   vmcfg._kernel_command);
    sql.Add("bootloader",       vmcfg._bootloader);
    sql.Add("raw_data",         vmcfg._raw_data);
    sql.Add("vnc_password",     vmcfg._vnc_passwd);
    sql.Add("qualification",    vmcfg._qualifications);
    sql.Add("watchdog",         vmcfg._watchdog);
    if (vmcfg._appointed_cluster.empty())//插入 NULL
    {
        sql.Add("appointed_cluster");
    }
    else
    {
        sql.Add("appointed_cluster",vmcfg._appointed_cluster);
    }
    sql.Add("appointed_host",   vmcfg._appointed_host);
    sql.Add("project_id",       vmcfg._project_id);
    sql.Add("config_version",   vmcfg._config_version);

    if (vmcfg._deployed_cluster.empty())
    {
        sql.Add("deployed_cluster" );
    }
    else
    {
        sql.Add("deployed_cluster", vmcfg._deployed_cluster);
    }
    sql.Add("enable_coredump",    vmcfg._enable_coredump);
    sql.Add("enable_serial", vmcfg._enable_serial);
    sql.Add("hypervisor",       vmcfg._hypervisor);
    sql.Add("enable_livemigrate", vmcfg._enable_livemigrate);

    rc = sql.Update("WHERE oid = " + to_string(vmcfg._oid,dec));
    if(0 != rc)
    {
        //SkyAssert(false);
        return rc;
    }

    // 2. vmcfg_image 表
    VmBaseImagePool *p_image = VmBaseImagePool::GetInstance(_db);
    if (NULL == p_image)
    {
        Transaction::Cancel();
        SkyAssert(false);
        return rc;
    }

    rc = p_image->SetVmBaseImages(vmcfg._images, vmcfg._oid, true);
    if(0 != rc)
    {
        Transaction::Cancel();
        SkyAssert(false);
        return rc;
    }

    // 3. vmcfg_disk 表
    VmBaseDiskPool *p_disk = VmBaseDiskPool::GetInstance(_db);
    if (NULL == p_disk)
    {
        Transaction::Cancel();
        SkyAssert(false);
        return rc;
    }

    rc = p_disk->SetVmBaseDisks(vmcfg._disks, vmcfg._oid, true);
    if(0 != rc)
    {
        Transaction::Cancel();
        SkyAssert(false);
        return rc;
    }

    // 4. vmcfg_nic 表
    VmBaseNicPool *p_nic = VmBaseNicPool::GetInstance(_db);
    if (NULL == p_nic)
    {
        Transaction::Cancel();
        SkyAssert(false);
        return rc;
    }

    rc = p_nic->SetVmBaseNics(vmcfg._nics, vmcfg._oid, true);
    if(0 != rc)
    {
        Transaction::Cancel();
        SkyAssert(false);
        return rc;
    }


    // 7. vmcfg_context 表
    VmBaseContextPool *p_context = VmBaseContextPool::GetInstance(_db);
    if (NULL == p_context)
    {
        Transaction::Cancel();
        SkyAssert(false);
        return rc;
    }

    rc = p_context->SetVmBaseContext(vmcfg._context, vmcfg._oid, true);
    if(0 != rc)
    {
        Transaction::Cancel();
        SkyAssert(false);
        return rc;
    }

    // 8. vmcfg_device 表
    VmBaseDevicePool *p_device_pool = VmBaseDevicePool::GetInstance(_db);
    if (NULL == p_device_pool)
    {
        Transaction::Cancel();
        SkyAssert(false);
        return rc;
    }

    rc = p_device_pool->SetVmDevices(vmcfg._devices, vmcfg._oid, true);
    if(0 != rc)
    {
        Transaction::Cancel();
        SkyAssert(false);
        return rc;
    }

    return Transaction::Commit();
}


/******************************************************************************/
STATUS VmCfgPool::GetVids(vector<int64>  &vids, const string& where)
{
    ostringstream   sql;
    int             rc;

    SetCallback(static_cast<Callbackable::Callback>(&VmCfgPool::GetVidCallback),static_cast<void *>(&vids));

    sql  << "SELECT oid FROM " <<  _view_vmcfg_with_project << " WHERE " << where;

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
STATUS VmCfgPool::GetVidsInAR(vector<int64> &vids, int64 arid)
{
    ostringstream   sql;
    int             rc;

    SetCallback(static_cast<Callbackable::Callback>(&VmCfgPool::GetVidCallback),static_cast<void *>(&vids));

    sql  << "SELECT vid FROM " <<  _view_vmcfg_with_affinity_region << " WHERE affinity_region_id = " << arid;

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
int VmCfgPool::GetARCallback(void *nil, SqlColumn * columns)
{
    if (NULL == columns ||
        VIEW_VMAR_LIMIT != columns->get_column_num())
    {
        return -1;
    }

    AffinityRegion* affinity_region = static_cast<AffinityRegion*>(nil);

    columns->GetValue(VIEW_AR_ID, affinity_region->_oid);
    columns->GetValue(VIEW_AR_NAME, affinity_region->_name);
    columns->GetValue(VIEW_AR_DES, affinity_region->_description);
    columns->GetValue(VIEW_AR_TIME, affinity_region->_create_time);
    columns->GetValue(VIEW_AR_UID, affinity_region->_uid);
    columns->GetValue(VIEW_AR_LEVEL, affinity_region->_level);
    columns->GetValue(VIEW_AR_FORCED, affinity_region->_is_forced);

    return 0;
}
/******************************************************************************/
STATUS VmCfgPool::GetARByVid(int64 vid, AffinityRegion & ar)
{
    ostringstream   sql;
    int             rc;

    SetCallback(static_cast<Callbackable::Callback>(&VmCfgPool::GetARCallback),static_cast<void *>(&ar));

    sql  << "SELECT "<<_col_names_vmcfg_with_affinity_region
         <<" FROM " <<  _view_vmcfg_with_affinity_region
         << " WHERE vid = " + to_string(vid, dec);

    rc = _db->Execute(sql, this);

    UnsetCallback();

    if(SQLDB_RESULT_EMPTY == rc)
    {
        return ERROR_OBJECT_NOT_EXIST;
    }

    if(rc != SQLDB_OK)
    {
        OutPut(SYS_ERROR, "Query affinity region from view_vmcfg_with_affinity_region failed !\n");
        return ERROR_DB_SELECT_FAIL;
    }

    return SUCCESS;

}
/******************************************************************************/
int VmCfgPool::GetRepelWithARCallback(void *nil, SqlColumn * columns)
{
    if ( (NULL == columns)
        ||(VIEW_REPELAR_LIMIT != columns->get_column_num()) )
    {
        return -1;
    }

    VmRepel  repel;
    vector<VmRepel> *ptRecord = static_cast<vector<VmRepel> *>(nil);

    columns->GetValue(VIEW_VID1, repel._vid1);
    columns->GetValue(VIEW_VID2, repel._vid2);

    ptRecord->push_back(repel);
    return 0;
}

/******************************************************************************/
STATUS VmCfgPool::GetVmRepelWithAR(int64 arid, vector<VmRepel>& repels)
{
    ostringstream   sql;
    int             rc;

    SetCallback(static_cast<Callbackable::Callback>(&VmCfgPool::GetRepelWithARCallback),
                static_cast<void *>(&repels));

    sql << "SELECT "<<_col_names_repel_with_affinity_region
        <<" FROM " <<  _view_repel_with_affinity_region
        << " WHERE affinity_region_id = " + to_string(arid, dec);

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
STATUS VmCfgPool::GetClusterWithARByVid(string &cc, int64 vid)
{
    STATUS tRet;
    vector<int64> vids;
    AffinityRegion ar;

    tRet = GetARByVid(vid,ar);
    if(SUCCESS != tRet)
    {
        return tRet;
    }

    vids.clear();
    tRet = GetVidsInAR(vids, ar.get_oid());
    if ((SUCCESS != tRet)
         &&(ERROR_OBJECT_NOT_EXIST != tRet))
    {
        OutPut(SYS_DEBUG, "GetClusterWithARByVid get vid fail, ret = %d!\n",tRet);
        return ERROR;
    }

    vector<int64>::const_iterator it;
    for (it = vids.begin();it!= vids.end();it++)
    {
        VmCfg  vmcfg;
        tRet = GetVmById(vmcfg, *it);
        if (SUCCESS != tRet)
        {
            OutPut(SYS_DEBUG, "GetClusterWithARByVid get VM failed, ret = %d!\n",tRet);
            return ERROR;
        }

        if(!vmcfg.get_cluster_name().empty())
        {
            cc = vmcfg.get_cluster_name();
            OutPut(SYS_DEBUG, "GetClusterWithARByVid get CC %s!\n",cc.c_str());
            return SUCCESS;
        }

        /*检查虚拟机是否正在部署,是则取要部署的cluster返回*/
        ostringstream where;
        where << "label_int64_1 = " << *it ;
        vector<string> workflows;
        tRet = FindWorkflow(workflows, VM_WF_CATEGORY, VM_WF_DEPLOY_TO_CLUSTER, where.str(), false);
        if(SUCCESS == tRet && !workflows.empty())
        {
            SelectClusterLabels ac_labels_cluster;
            tRet = GetActionLabels(workflows[0], VM_ACT_SELECT_CLUSTER, ac_labels_cluster);
            if((SUCCESS != tRet)&&(ERROR_OBJECT_NOT_EXIST != tRet))
            {
                OutPut(SYS_DEBUG, "s% GetActionLabels failed!\n", workflows[0].c_str());
                return ERROR;
            }
            else if(!ac_labels_cluster._des_cluster.empty())
            {
                cc = ac_labels_cluster._des_cluster;
                OutPut(SYS_DEBUG, "GetClusterWithARByVid get CC %s!\n",cc.c_str());
                return SUCCESS;
            }
        }

    }
    OutPut(SYS_DEBUG, "vm not in affinity_region or other vms(%d) in this ar have not deployed \n",vids.size());
    return ERROR;

}
/******************************************************************************/
int VmCfgPool::AddVmInAR(int64 vid, int64 ar_id)
{
    int rc = SQLDB_OK;
    int64 tmp_vid = 0;
    if ((INVALID_OID  == vid)
         ||(INVALID_OID ==  ar_id))
    {
       return -1;
    }

    ostringstream oss;
    oss << " affinity_region_id = " << ar_id << " AND vid = " << vid ;
    int ret = SelectColumn(_table_vmcfg_affinity_region, "vid", oss.str(), tmp_vid);
    if (SQLDB_RESULT_EMPTY == ret)
    {
        SqlCommand sql(_db,
                       VmCfgPool::_table_vmcfg_affinity_region,
                       SqlCommand::WITH_TRANSACTION);

        sql.Add("affinity_region_id", ar_id);
        sql.Add("vid", vid);
        /*已支持事务*/
        int rc = sql.Insert();
        if(rc != SQLDB_OK)
        {
            SkyAssert(false);
        }
    }
    else if(ret == SQLDB_OK)
    {
        OutPut(SYS_DEBUG, "Vmcfg already exist in affinity_region! where %s, ret = %d!\n", oss.str().c_str(), ret);
    }
    else
    {
        SkyAssert(false);
    }

    return rc;

};
/******************************************************************************/
int VmCfgPool::DeleteVmFromAR(int64 vid, int64 ar_id)
{

    ostringstream   sql;
    int             rc;

    if ((INVALID_OID == vid)
         ||(INVALID_OID == ar_id) )
    {
       SkyAssert(false);
       return -1;
    }

    sql << "DELETE FROM " << _table_vmcfg_affinity_region <<
         " WHERE affinity_region_id = " << ar_id << " AND vid = " << vid;
    if(Transaction::On())
    {
        rc = Transaction::Append(sql.str());
    }
    else
    {
        rc = _db->Execute(sql);
    }
    return rc;
}

/******************************************************************************/
int VmCfgPool:: GetVidCallback(void * oids, SqlColumn *pColumns)
{
    vector<int64> *  oids_vector;

    oids_vector = static_cast<vector<int64> *>(oids);

    if ( pColumns->get_column_num() == 0 )
    {
        return -1;
    }

    oids_vector->push_back(pColumns->GetValueInt64(0));

    return 0;
}
/******************************************************************************/
STATUS VmCfgPool::GetVmById(VmCfg  &vmcfg, int64 vid)
{
    ostringstream   oss;
    int             rc;
    int64           tmp_oid;
    vector<VmNicConfig>               vm_nics;
    vector<VmNicConfig>::iterator     it_vm_nics;
    //SqlDB  *database;

    // 1. 查 _view_vmcfg_with_project
    SetCallback(static_cast<Callbackable::Callback>(&VmCfgPool::GetCallback), static_cast<void *>(&vmcfg));

    oss << "SELECT " << _col_names_view_vmcfg_with_project <<
           " FROM " << _view_vmcfg_with_project <<
           " WHERE oid = " << vid;

   // cout << oss.str();//wh_debug

    tmp_oid = vid;
    vmcfg._oid    = INVALID_OID;
    vmcfg._uid    = INVALID_UID;

    rc = _db->Execute(oss, this);

    UnsetCallback();
    if ( (SQLDB_OK != rc) && (SQLDB_RESULT_EMPTY != rc)  )
    {
        //数据库系统出错
        SkyAssert(false);
        return rc;
    }

    if ((vmcfg._oid != tmp_oid ) || (SQLDB_RESULT_EMPTY == rc) )
    {
        //数据库系统没问题，但是没找到记录
        return -1;
    }

    // 2. 查 vmcfg_image 表
    VmBaseImagePool *p_image = VmBaseImagePool::GetInstance(_db);
    if (NULL == p_image)
    {
        SkyAssert(false);
        return -1;
    }

    rc = p_image->GetVmBaseImages(vmcfg._images, vmcfg._oid, true);
    if ( (SQLDB_OK!= rc)
          && (SQLDB_RESULT_EMPTY != rc) )
    {
        SkyAssert(false);
        return rc;
    }

    // 3. 查 vmcfg_disk 表
    VmBaseDiskPool  *p_disk = VmBaseDiskPool::GetInstance(_db);
    if (NULL == p_disk)
    {
        SkyAssert(false);
        return -1;
    }

    rc = p_disk->GetVmBaseDisks(vmcfg._disks, vmcfg._oid, true);
    if ( (SQLDB_OK!= rc)
          && (SQLDB_RESULT_EMPTY != rc) )
    {
        SkyAssert(false);
        return rc;
    }

    // 4. 查 nic 信息
    VmBaseNicPool  *p_nic = VmBaseNicPool::GetInstance(_db);
    if (NULL == p_nic)
    {
        SkyAssert(false);
        return -1;
    }

    rc = p_nic->GetVmBaseNics(vmcfg._nics, vmcfg._oid,true);
    if ( (SQLDB_OK!= rc)
          && (SQLDB_RESULT_EMPTY != rc) )
    {
        SkyAssert(false);
        return rc;
    }


    // 7. 查 vmcfg_context 表
    VmBaseContextPool  *p_context = VmBaseContextPool::GetInstance(_db);
    if (NULL == p_context)
    {
        SkyAssert(false);
        return -1;
    }

    rc = p_context->GetVmBaseContext(vmcfg._context,vmcfg._oid, true);
    if ( (SQLDB_OK!= rc)
          && (SQLDB_RESULT_EMPTY != rc) )
    {
        SkyAssert(false);
        return rc;
    }

    // 8. 获取本地磁盘大小
    rc = GetTotalSize(vid, VM_STORAGE_TO_LOCAL, vmcfg._local_disk_size);
    if(SUCCESS != rc)
    {
        SkyAssert(false);
        return rc;
    }

    // 9. 获取本地磁盘大小
    rc = GetTotalSize(vid, VM_STORAGE_TO_SHARE, vmcfg._share_disk_size);
    if(SUCCESS != rc)
    {
        SkyAssert(false);
        return rc;
    }

    //  vmcfg_device 表
    VmBaseDevicePool *p_device_pool = VmBaseDevicePool::GetInstance(_db);
    if (NULL == p_device_pool)
    {
        SkyAssert(false);
        return -1;
    }

    rc = p_device_pool->GetVmDevices(vmcfg._devices, vmcfg._oid, true);
    if ( (SQLDB_OK!= rc)
          && (SQLDB_RESULT_EMPTY != rc) )
    {
        SkyAssert(false);
        return rc;
    }

    return SUCCESS;

}


/******************************************************************************/
int VmCfgPool::GetCallback(void *nil, SqlColumn * columns)
{
    VmCfg * vmcfg ;

    if (NULL == columns ||
    LIMIT != columns->get_column_num())
    {
        return -1;
    }

    vmcfg = static_cast<VmCfg *>(nil);

    columns->GetValue(OID,  vmcfg->_oid);
    columns->GetValue(NAME, vmcfg->_name);
    columns->GetValue(DESCRIPTION, vmcfg->_description);
    columns->GetValue(CREATE_TIME, vmcfg->_create_time);
    columns->GetValue(VCPU,    vmcfg->_vcpu);
    columns->GetValue(TCU,     vmcfg->_tcu);
    columns->GetValue(MEMORY,  vmcfg->_memory);

    columns->GetValue(VIRT_TYPE,      vmcfg->_virt_type);
    columns->GetValue(ROOT_DEVICE,    vmcfg->_root_device);
    columns->GetValue(KERNEL_COMMAND, vmcfg->_kernel_command);
    columns->GetValue(BOOTLOADER,     vmcfg->_bootloader);
    columns->GetValue(RAW_DATA,       vmcfg->_raw_data);
    columns->GetValue(VNC_PASSWORD,   vmcfg->_vnc_passwd);
    columns->GetValue(QUALIFICATION,  vmcfg->_qualifications);
    columns->GetValue(WATCHDOG,       vmcfg->_watchdog);
    columns->GetValue(APPOINTED_CLUSTER, vmcfg->_appointed_cluster);
    columns->GetValue(APPOINTED_HOST,   vmcfg->_appointed_host);
    columns->GetValue(PROJECT_ID,        vmcfg->_project_id);

    columns->GetValue(DEPLOYED_CLUSTER, vmcfg->_deployed_cluster);
    columns->GetValue(CONFIG_VERSION, vmcfg->_config_version);
    columns->GetValue(PROJECT_NAME,     vmcfg->_project_name);
    columns->GetValue(UID,              vmcfg->_uid);
    columns->GetValue(USER_NAME,        vmcfg->_user_name);
    columns->GetValue(ENABLE_COREDUMP,        vmcfg->_enable_coredump);
    columns->GetValue(ENABLE_SERIAL,     vmcfg->_enable_serial);
    columns->GetValue(HYPERVISOR,       vmcfg->_hypervisor);
    columns->GetValue(ENABLE_LIVEMIGRATE, vmcfg->_enable_livemigrate);
    return 0;
}

/******************************************************************************/
/* 检查vid对应的image是否存在，以及image_id是否都不为空 */
STATUS VmCfgPool::CheckImageIdByVid(int64 vid)
{
    int            rc;
    int            count = 0; //count统计某个vid有多少个image
    int            count_null = 0; //count_null统计vid所有为null的image个数
    ostringstream  where;
    string         column = " image_id ";

    SqlCallbackable sql(_db);

    where << " vid = " << vid;
    rc = sql.SelectColumn("vmcfg_image",
                          "count(*)",
                          where.str(),
                          count);
    if(rc != 0 || count == 0)
    {
        return ERROR;
    }

    where << " and " << column <<" is null";
    rc = sql.SelectColumn("vmcfg_image",
                          "count(*)",
                          where.str(),
                          count_null);

    if(count > 0 && count_null == 0)
    {
        return SUCCESS;
    }
    else
    {
        return ERROR;
    }
}

/******************************************************************************/
int VmCfgPool::Show(vector<ApiVmStaticInfo> &vec_info, vector<ApiVmRunningInfo> &vec_runinfo, const string& where)
{
    STATUS           tRet = ERROR;
    vector<int64>    vids;

    tRet = GetVids(vids, where);
    if ((SUCCESS != tRet)
        &&(ERROR_OBJECT_NOT_EXIST != tRet))
    {
        return ERROR;
    }

    vector<int64>::iterator it;
    for (it = vids.begin();  it != vids.end(); it++)
    {
        VmCfg           vmcfg ;
        ApiVmStaticInfo    info;
        tRet = GetVmById(vmcfg, *it);
        if (SUCCESS != tRet)
        {
           return SUCCESS;
        }

        ToApiInfo(vmcfg, info);
        vec_info.push_back(info);

        ApiVmRunningInfo runinfo;
        runinfo.vid = vmcfg._oid;
        runinfo.computationinfo.state= VM_CONFIGURATION;
        runinfo.computationinfo.deployed_cluster= vmcfg._deployed_cluster;
        runinfo.storageinfo.disk_size = vmcfg.get_total_size();
        runinfo.netinfo.nic_info= info.cfg_info.base_info.nics;
        vec_runinfo.push_back(runinfo);
    }

    return SUCCESS;
}


/******************************************************************************/
STATUS VmCfgPool::ToApiInfo(const VmCfg &vmcfg, ApiVmStaticInfo &info)
{
    ostringstream   oss;
    int             rc = -1;

    info.vid          = vmcfg._oid;
    info.cfg_info.vm_name      = vmcfg._name;
    info.project_id   = vmcfg.get_project_id();;
    info.cfg_info.project_name  = vmcfg.get_project_name();
    info.uid          = vmcfg._uid;
    info.user_name      = vmcfg._user_name;

    info.config_version  = vmcfg.get_config_version();
    info.create_time      = vmcfg._create_time.serialize();

    info.cfg_info.base_info.description = vmcfg._description;
    info.cfg_info.base_info.vcpu  = vmcfg._vcpu;
    info.cfg_info.base_info.tcu   = vmcfg._tcu;
    info.cfg_info.base_info.memory= vmcfg._memory;

    info.cfg_info.base_info.root_device    = vmcfg._root_device;
    info.cfg_info.base_info.kernel_command = vmcfg._kernel_command;
    info.cfg_info.base_info.bootloader     = vmcfg._bootloader;
    info.cfg_info.base_info.virt_type      = vmcfg._virt_type;
    info.cfg_info.base_info.hypervisor    = vmcfg._hypervisor;
    info.cfg_info.base_info.rawdata        = vmcfg._raw_data;
    info.cfg_info.base_info.context        = vmcfg._context._str_map;

    info.cfg_info.base_info.cluster        = vmcfg._appointed_cluster;
    info.cfg_info.base_info.host           = vmcfg._appointed_host;
    info.cfg_info.base_info.vm_wdtime      = vmcfg._watchdog;
    info.cfg_info.base_info.vnc_passwd     = vmcfg._vnc_passwd;
    info.cfg_info.base_info.enable_coredump = vmcfg._enable_coredump;
    info.cfg_info.base_info.enable_serial = vmcfg._enable_serial;
    info.cfg_info.base_info.enable_livemigrate= vmcfg._enable_livemigrate;


    //部署策略
    vector<Expression>::const_iterator   vec_it_exp;
    for (vec_it_exp = vmcfg._qualifications._qua.begin();
         vec_it_exp != vmcfg._qualifications._qua.end();
         ++vec_it_exp)
    {
        ApiVmExpression   ap_exp(vec_it_exp->_key,
                                 vec_it_exp->_op,
                                 vec_it_exp->_value);

        info.cfg_info.base_info.qualifications.push_back(ap_exp);
    }

    //disk
    vector<VmBaseDisk>::const_iterator  it_disk;
    for (it_disk = vmcfg._disks.begin();
         it_disk !=vmcfg._disks.end();
         it_disk++ )
    {
        ApiVmDisk   api_disk;

        api_disk.id       = INVALID_OID;
        api_disk.size     = it_disk->_size;
        api_disk.type     = DISK_TYPE_DISK;
        api_disk.bus      = it_disk->_bus;
        api_disk.target   = it_disk->_target;
        api_disk.position = it_disk->_position;
        api_disk.fstype   = it_disk->_fstype;
        info.cfg_info.base_info.disks.push_back(api_disk);
    }

    // 3. 查 image 视图
    VmBaseImagePool  *p_image = VmBaseImagePool::GetInstance(_db);

    vector<VmBaseImageInfo>            image_info;
    vector<VmBaseImageInfo>::iterator  it_image;
    rc = p_image->GetImageInfoByView(image_info, vmcfg._oid, true);
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


    // 4. 查 网卡信息
    VmBaseNicPool  *p_nic = VmBaseNicPool::GetInstance(_db);

    vector<VmBaseNic>  nic_info;
    vector<VmBaseNic>::const_iterator vec_it_nic;

    rc = p_nic->GetVmBaseNics(nic_info, vmcfg._oid,true);
    if ( (SQLDB_OK!= rc)
          && (SQLDB_RESULT_EMPTY != rc) )
    {
        SkyAssert(false);
        return rc;
    }

    for (vec_it_nic = nic_info.begin();
         vec_it_nic != nic_info.end();
         ++vec_it_nic)
    {
         ApiVmNic api_nic(vec_it_nic->_nic_index,
                          vec_it_nic->_sriov,
                          vec_it_nic->_loop,
                          vec_it_nic->_logic_network_id,
                          vec_it_nic->_model,
                          vec_it_nic->_vsi_profile_id,
                          vec_it_nic->_ip,
                          vec_it_nic->_netmask,
                          vec_it_nic->_gateway,
                          vec_it_nic->_mac);  

         info.cfg_info.base_info.nics.push_back(api_nic);
    }

    vector<VmDeviceConfig>::const_iterator it_dev;
    for (it_dev = vmcfg._devices.begin();
         it_dev != vmcfg._devices.end();
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
STATUS VmCfgPool::ToVMDeployInfo(VmCfg &vmcfg,VMDeployInfo &deploy_info)
{
    int       rc               = -1;

    deploy_info._config._name   = vmcfg._name;
    deploy_info._project_id     = vmcfg.get_project_id();
    deploy_info._uid            = vmcfg.get_uid();
    deploy_info._host           = vmcfg._appointed_host;

    deploy_info._config._vid    = vmcfg._oid ;
    deploy_info._config._vcpu   = vmcfg._vcpu ;
    deploy_info._config._tcu    = vmcfg._tcu ;
    deploy_info._config._memory = vmcfg._memory;

    deploy_info._config._kernel._id   = INVALID_OID;
    deploy_info._config._kernel._size = 0;

    deploy_info._config._initrd._id   = INVALID_OID;
    deploy_info._config._initrd._size = 0;


    deploy_info._config._virt_type      = vmcfg._virt_type ;
    deploy_info._config._root_device    = vmcfg._root_device ;
    deploy_info._config._kernel_command = vmcfg._kernel_command;
    deploy_info._config._bootloader     = vmcfg._bootloader;
    deploy_info._config._rawdata        = vmcfg._raw_data ;
    deploy_info._config._vnc_passwd     = vmcfg._vnc_passwd ;
    deploy_info._config._vm_wdtime      = vmcfg._watchdog;
    deploy_info._config._config_version = vmcfg._config_version;

    deploy_info._config._local_disk_size = vmcfg.get_local_disk_size();
    deploy_info._config._share_disk_size = vmcfg.get_share_disk_size();

    deploy_info._config._context  = vmcfg._context._str_map;
    deploy_info._config._enable_serial = vmcfg._enable_serial;
    deploy_info._config._enable_livemigrate = vmcfg._enable_livemigrate;
    deploy_info._config._hypervisor    = vmcfg._hypervisor;

    deploy_info._requirement._qualifications = vmcfg._qualifications._qua;

    VmBaseDiskPool *p_disk = VmBaseDiskPool::GetInstance(_db);

    vector<VmBaseDisk>::iterator  it_disk;
    for (it_disk = vmcfg._disks.begin();
         it_disk !=vmcfg._disks.end();
         it_disk++ )
    {
        VmDiskConfig   disk(INVALID_OID,
                            it_disk->_position,
                            it_disk->_size,
                            DISK_TYPE_DISK,
                            it_disk->_bus,
                            it_disk->_target,
                            "",
                            "",
                            it_disk->_fstype);
        deploy_info._config._disks.push_back(disk);

    }

    /*更新数据库*/
    if (NULL == p_disk)
    {
        SkyAssert(false);
        return rc;
    }
    rc = p_disk->SetVmBaseDisks(vmcfg._disks, vmcfg._oid, true);

    // 3. 查 image 视图
    VmBaseImagePool  *p_image = VmBaseImagePool::GetInstance(_db);
    vector<VmBaseImageInfo>            image_info;
    rc = p_image->GetImageInfoByView(image_info, vmcfg._oid, true);
    if ( (SQLDB_OK!= rc)
          && (SQLDB_RESULT_EMPTY != rc) )
    {
        SkyAssert(false);
        return rc;
    }

    vector<VmBaseImageInfo>::iterator  it_image;

    for (it_image = image_info.begin();
         it_image != image_info.end();
         it_image++)
    {
        if (DISK_TYPE_MACHINE == it_image->_type)
        {
           deploy_info._config._machine._id       = it_image->_image_id;
           deploy_info._config._machine._position = it_image->_position;
           deploy_info._config._machine._size     = it_image->_size;
           deploy_info._config._machine._type     = it_image->_type;
           deploy_info._config._machine._bus      = it_image->_bus;
           deploy_info._config._machine._target   = it_image->_target;
           deploy_info._config._machine._url      = it_image->_url;
           deploy_info._config._machine._disk_format = it_image->_disk_format;
           deploy_info._config._machine._disk_size = it_image->_disk_size;
           deploy_info._config._machine._reserved_backup = it_image->_reserved_backup;
           deploy_info._config._machine._os_arch   = it_image->_os_arch;
        }
        else
        {
           VmDiskConfig   disk(it_image->_image_id,
                            it_image->_position,
                            it_image->_size,
                            it_image->_type,
                            it_image->_bus,
                            it_image->_target,
                            it_image->_url,
                            it_image->_share_url,
                            it_image->_disk_format,
                            it_image->_disk_size);

            deploy_info._config._disks.push_back(disk);
        }
    }

    // 4. 查 nic 信息
    VmBaseNicPool  *p_nic = VmBaseNicPool::GetInstance(_db);

    rc = p_nic->GetVmCfgNetwork(deploy_info._config._nics, vmcfg._oid );
    if ( (SQLDB_OK!= rc)
          && (SQLDB_RESULT_EMPTY != rc) )
    {
        SkyAssert(false);
        return rc;
    }

    deploy_info._config._devices = vmcfg._devices;

    return SUCCESS;
}


/******************************************************************************/
int VmCfgPool::IsVmExistInDelTab(int64 vid)
{
    int64   tmp_oid  = -1;
    string  column   = " oid ";
    string  where    = " oid = " + to_string<int64>(vid, dec);

    int ret = SelectColumn(VmCfgPool::_table_vmcfg_deleted,
                            column,
                            where,
                            tmp_oid);


    return ret;
};

/******************************************************************************/
int VmCfgPool::Insert2Repel(int64 vid1, int64 vid2)
{
    int rc = SQLDB_OK;

    if ((INVALID_OID  == vid1)
         ||(INVALID_OID ==  vid2))
    {
       return -1;
    }

    ostringstream oss;
    int vid = 0;
    oss << " vid1 = " << vid1 << " AND vid2 = " << vid2 ;
    int ret = SelectColumn(_table_vmcfg_repel, "vid1", oss.str(), vid);
    if (SQLDB_RESULT_EMPTY == ret)
    {
        SqlCommand sql(_db,
                       VmCfgPool::_table_vmcfg_repel,
                       SqlCommand::WITHOUT_TRANSACTION);

        sql.Add("vid1", vid1);
        sql.Add("vid2", vid2);

        int rc = sql.Insert();
        if(rc != SQLDB_OK)
        {
            SkyAssert(false);
        }
    }
    else if(ret == SQLDB_OK)
    {
        OutPut(SYS_DEBUG, "Vmcfg repel already exist! where %s, ret = %d!\n", oss.str().c_str(), ret);
    }
    else
    {
        SkyAssert(false);
    }

    return rc;

};

/******************************************************************************/
STATUS VmCfgPool::Insert2RepelTransAction(int64 vid1, int64 vid2)
{
    if ((INVALID_OID  == vid1)
         ||(INVALID_OID ==  vid2))
    {
       return ERROR;
    }

    ostringstream oss;
    int64 vid = 0;
    oss << " vid1 = " << vid1 << " AND vid2 = " << vid2 ;
    int ret = SelectColumn(_table_vmcfg_repel, "vid1", oss.str(), vid);
    if (SQLDB_RESULT_EMPTY == ret)
    {
        SqlCommand sql(_db,
                       VmCfgPool::_table_vmcfg_repel,
                       SqlCommand::WITH_TRANSACTION);

        sql.Add("vid1", vid1);
        sql.Add("vid2", vid2);

        int rc = sql.Insert();
        if(rc != SQLDB_OK)
        {
            SkyAssert(false);
            return ERROR;
        }
    }
    else if(ret == SQLDB_OK)
    {
        OutPut(SYS_DEBUG, "Vmcfg repel already exist! where %s, ret = %d!\n", oss.str().c_str(), ret);
    }
    else
    {
        SkyAssert(false);
        return ERROR;
    }

    return SUCCESS;

};
/******************************************************************************/
int VmCfgPool::DropRepel(int64 vid1, int64 vid2)
{
    ostringstream   sql;
    int             rc;

    if ((INVALID_OID == vid1)
         ||(INVALID_OID == vid2) )
    {
       SkyAssert(false);
       return -1;
    }

    sql << "DELETE FROM " << _table_vmcfg_repel <<
         " WHERE vid1 =" << vid1 << " and vid2 = " << vid2;

    if(Transaction::On())
    {
        rc = Transaction::Append(sql.str());
    }
    else
    {
        rc = _db->Execute(sql);
    }
    return rc;

};

/******************************************************************************/
int  VmCfgPool::SearchRepelTab(vector<ApiVmCfgMap> &vms, const string &where)
{
    ostringstream sql;

    SetCallback(static_cast<Callbackable::Callback>(&VmCfgPool::SearchRepelTabCallback),
                static_cast<void *>(&vms));

    sql << "SELECT " << _col_names_view_vmcfg_repel
        << " FROM " << _view_vmcfg_repel ;

    if (!where.empty())
    {
        sql << " WHERE " << where;
    }

    int ret = _db->Execute(sql, this);
    UnsetCallback();

    if ((SQLDB_OK != ret)
         &&(SQLDB_RESULT_EMPTY != ret))
    {
        return -1;
    }

    return 0;
}

/******************************************************************************/
int VmCfgPool::SearchRepelTabCallback(void *nil, SqlColumn * columns)
{
   if ( (NULL == columns)
        ||(VIEW_REPELTAB_LIMIT != columns->get_column_num()) )
    {
        return -1;
    }

    ApiVmCfgMap  vm_map;
    vector<ApiVmCfgMap> * ptRecord = static_cast<vector<ApiVmCfgMap> *>(nil);
    int64  vid1 = INVALID_OID;
    int64  vid2 = INVALID_OID;

    columns->GetValue(VID1, vid1);
    columns->GetValue(VID2, vid2);
    if (vid2 <= vid1)
    {
        vm_map.src = vid2;
        vm_map.des = vid1;
    }
    else
    {
        vm_map.src = vid1;
        vm_map.des = vid2;
    }

    ptRecord->push_back(vm_map);
    return 0;
}
/******************************************************************************/
int VmCfgPool::GetVmRepelsCallback(void *nil, SqlColumn * columns)
{
   if ( (NULL == columns)
        ||(2 != columns->get_column_num()) )
    {
        return -1;
    }

    VmRepel  repel;
    vector<VmRepel> *ptRecord = static_cast<vector<VmRepel> *>(nil);

    columns->GetValue(0, repel._vid1);
    columns->GetValue(1, repel._vid2);

    ptRecord->push_back(repel);
    return 0;
}
/******************************************************************************/
int VmCfgPool::GetVmAffregCallback(void *nil, SqlColumn * columns)
{
   if ( (NULL == columns)
        ||(2 != columns->get_column_num()) )
    {
        return -1;
    }

    bool isfind = false;
    int64 arid,vid;
    VmAffReg affreg;
    vector<VmAffReg> *ptRecord = static_cast<vector<VmAffReg> *>(nil);
    vector<VmAffReg>::iterator it_affreg;

    columns->GetValue(0, arid);
    columns->GetValue(1, vid);

    for (it_affreg = ptRecord->begin(); it_affreg != ptRecord->end(); it_affreg++)
    {
        if(it_affreg->_arid == arid)
        {
            it_affreg->_vid.push_back(vid);
            isfind = true;
            break;
        }        
    }

    if(false == isfind)
    {
        affreg._arid = arid;
        affreg._vid.push_back(vid);
        ptRecord->push_back(affreg);
    }

    return 0;
}
/******************************************************************************/
STATUS VmCfgPool::GetVmRepels(const vector<int64>& vids, vector<VmRepel>& repels)
{
    if (0 == vids.size())
    {
        return SUCCESS;
    }
    vector<int64>::const_iterator it = vids.begin();
    string set = "(" + to_string<int64>(*it,dec);
    it++;
    while (it != vids.end())
    {
        set += ",";
        set += to_string<int64>(*it,dec);
        it++;
    }
    set += ")";

    ostringstream sql;

    SetCallback(static_cast<Callbackable::Callback>(&VmCfgPool::GetVmRepelsCallback),
                static_cast<void *>(&repels));

    sql << "SELECT " << _col_names_vmcfg_repel
        << " FROM " << _table_vmcfg_repel
        << " WHERE vid1 in " << set
        << " AND vid2 in " << set;

    int ret = _db->Execute(sql, this);
    UnsetCallback();

    if ((SQLDB_OK != ret) && (SQLDB_RESULT_EMPTY != ret))
    {
        return ERROR_DB_SELECT_FAIL;
    }

    return SUCCESS;
}
/******************************************************************************/
STATUS VmCfgPool::GetVmAffregs(const vector<int64>& vids, vector<VmAffReg>& affregs)
{
    if (0 == vids.size())
    {
        return SUCCESS;
    }
    vector<int64>::const_iterator it = vids.begin();
    string set = "(" + to_string<int64>(*it,dec);
    it++;
    while (it != vids.end())
    {
        set += ",";
        set += to_string<int64>(*it,dec);
        it++;
    }
    set += ")";

    ostringstream sql;

    SetCallback(static_cast<Callbackable::Callback>(&VmCfgPool::GetVmAffregCallback),
                static_cast<void *>(&affregs));

    sql << "SELECT " << _col_names_affinity_region
        << " FROM " << _table_vmcfg_affinity_region
        << " WHERE vid in " << set;

    int ret = _db->Execute(sql, this);
    UnsetCallback();

    if (0 != affregs.size())
    {
        vector<VmAffReg>::iterator affreg_it;
        AffinityRegionPool *_ppool = AffinityRegionPool::GetInstance();

        for (affreg_it = affregs.begin(); affreg_it != affregs.end(); affreg_it++)
        {    
            _ppool->GetAffinityRegion(affreg_it->_arid, affreg_it->aff_region);
        }
    }
    

    if ((SQLDB_OK != ret) && (SQLDB_RESULT_EMPTY != ret))
    {
        return ERROR_DB_SELECT_FAIL;
    }

    return SUCCESS;
}
/******************************************************************************/
STATUS VmCfgPool::SetVmRepels(const vector<VmRepel>& repels)
{
    STATUS ret = SUCCESS;
    if (0 == repels.size())
    {
        return ret;
    }

    int64   vid_small = 0;
    int64   vid_big   = 0;

    vector<VmRepel>::const_iterator it;
    for (it = repels.begin(); it != repels.end(); it++)
    {

        if(it->_vid1 < it->_vid2)
        {
            vid_small = it->_vid1;
            vid_big   = it->_vid2;
        }
        else
        {
            vid_small = it->_vid2 ;
            vid_big   = it->_vid1 ;
        }
        ret = Insert2RepelTransAction(vid_small,vid_big);
        if (SUCCESS != ret)
        {
            return ret;
        }
    }
    return SUCCESS;
}
/******************************************************************************/
STATUS VmCfgPool::GetTotalSize(int64 vid, int position, int64 &size)
{
    int   rc = -1;
    int64 total_image = 0;
    int64 total_disk = 0;
    ostringstream where;

    where << " vid = " << vid << " and position = " << position;

    rc = SelectColumn("view_vmcfg_image","sum(size)", where.str(),total_image);
    if ((SQLDB_OK!= rc)
         && (SQLDB_RESULT_EMPTY != rc) )
    {
        SkyAssert(false);
        return ERROR;
    }

    rc = SelectColumn("vmcfg_disk","sum(size)", where.str(),total_disk);
    if ((SQLDB_OK!= rc)
         && (SQLDB_RESULT_EMPTY != rc) )
    {
        SkyAssert(false);
        return ERROR;
    }

    size = total_image + total_disk;
    return SUCCESS;
}
/******************************************************************************/
int64 VmCfgPool::GetVmCfgCount(const string& where)
{
    int64   count  = -1;
    int     ret    = -1;
    string  column = " COUNT(*) ";

    ret = SelectColumn(VmCfgPool::_view_vmcfg_full_info,
                            column,
                            where,
                            count);

    return count;
}

/******************************************************************************/
int64 VmCfgPool::GetVmCfgOwner(int64 vid)
{
    int64   uid    = -1;
    int     rc     = -1;
    string  column = " uid ";
    string  where  = " vid = " + to_string<int64>(vid, dec);

    rc = SelectColumn(VmCfgPool::_view_vmcfg_full_info,
                            column,
                            where,
                            uid);
    return uid;
}

/******************************************************************************/
int VmCfgPool::GetVidsInFullInfo(vector<int64>& vids,const string& where)
{
    int     ret       = -1;
    string  column    = " vid ";

    ret = SelectColumn(VmCfgPool::_view_vmcfg_full_info,
                            column,
                            where,
                            vids);

    if(SQLDB_RESULT_EMPTY == ret)
    {
        return ERROR_OBJECT_NOT_EXIST;
    }

    if(ret != SQLDB_OK)
    {
        return ERROR_DB_SELECT_FAIL;
    }

    return SUCCESS;
}

/******************************************************************************/
STATUS VmCfgPool::GetRunningCluster(int64 vid,string& cluster)
{
    int     ret    = -1;
    string  column = " deployed_cluster ";
    string  where  = " vid = " + to_string<int64>(vid, dec);

    ret = SelectColumn(VmCfgPool::_view_vmcfg_full_info,
                            column,
                            where,
                            cluster);

    if(SQLDB_RESULT_EMPTY == ret)
    {
        return ERROR_OBJECT_NOT_EXIST;
    }

    if(ret != SQLDB_OK)
    {
        return ERROR_DB_SELECT_FAIL;
    }

    return SUCCESS;
}

int64 VmCfgPool::GetTotalSize(int64 vid, int position)
{
    ostringstream where;
    where << "vid = " << vid << " and position = " << position;

    int64 total_image = 0;
    SelectColumn("view_vmcfg_image","sum(size)", where.str(),total_image);

    int64 total_disk = 0;
    SelectColumn("vmcfg_disk","sum(size)", where.str(),total_disk);

    return total_image + total_disk;
}

bool IsVmDoing(int64 vid, const string &wf_name)
{
    ostringstream where;
    where << "label_int64_1 = " << vid;
    vector<string> workflows;

    STATUS  ret = FindWorkflow(workflows, VM_WF_CATEGORY, wf_name, where.str(), false);
    if(SUCCESS == ret && !workflows.empty())
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool VmCfgPool::IsVmDeploying(int64 vid)
{
    return IsVmDoing(vid, VM_WF_DEPLOY_TO_CLUSTER);
}

bool VmCfgPool::IsVmCanceling(int64 vid)
{
    return IsVmDoing(vid, VM_WF_CANCEL_FROM_CLUSTER);
}

bool VmCfgPool::IsVmModifying(int64 vid)
{
    return IsVmDoing(vid, VM_WF_MODIFY_INDIVIDUAL_VMCFG);
}

int VmCfgPool::Execute(ostringstream& cmd)
{
    return _db->Execute(cmd, this);
}

}




