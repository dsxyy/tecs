/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：vmcfg_pool.h
* 文件标识：见配置管理计划书
* 内容摘要：vmcfg类的定义文件
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

#ifndef VMCFG_MANGER_VMCFG_POOL_H        
#define VMCFG_MANGER_VMCFG_POOL_H

#include "vmcfg.h"
#include "sky.h"
#include "vm_messages.h"
#include "affinity_region.h"


namespace zte_tecs
{

/**
@brief 功能描述: 映像资源池的实体类\n
@li @b 其它说明: 无
*/
class VmCfgPool : public SqlCallbackable
{

/*******************************************************************************
* 1. public section
*******************************************************************************/
public:

   static VmCfgPool *GetInstance()
    {
        SkyAssert(NULL != _instance);
        return _instance;
    };
    
    static VmCfgPool *CreateInstance(SqlDB *database)
    {
        if(NULL == _instance)
        {
            _instance = new VmCfgPool(database);
        }
        
        return _instance;
    };

    static void Lock()
    {
        pthread_mutex_lock(&_mutex);
    };

    static void UnLock()
    {
        pthread_mutex_unlock(&_mutex);
    };

    STATUS Init(const bool& isReadOnly);  
    
    STATUS Allocate(VmCfg   &vmcfg);

    STATUS Drop(const VmCfg   &vmcfg);
                   
    STATUS Update(VmCfg   &vmcfg);    

    STATUS GetVids(vector<int64>  &vids, const string& where);

    int GetVidCallback(void * oids, SqlColumn *pColumns);
    
    STATUS GetVmById(VmCfg  &vmcfg, int64 vid); 
    
    STATUS CheckImageIdByVid(int64 vid); 

    int Show(vector<ApiVmStaticInfo> &vec_info, vector<ApiVmRunningInfo> &vec_runinfo,const string& where);
    
    // 需操作数据库的 虚拟机相关的接口放在这里

    STATUS ToApiInfo(const VmCfg &vmcfg, ApiVmStaticInfo &info);

    STATUS ToVMDeployInfo( VmCfg &vmcfg,VMDeployInfo &deploy_info);
 
    STATUS IsVmExistInDelTab(int64 vid);
    
    STATUS Insert2Repel(int64 vid1, int64 vid2);
    
    STATUS Insert2RepelTransAction(int64 vid1, int64 vid2);
    
    STATUS DropRepel(int64 vid1, int64 vid2);
    
    STATUS SearchRepelTab(vector<ApiVmCfgMap> &vms, const string &where);
    
    int GetVmRepelsCallback(void *nil, SqlColumn * columns);

    int GetVmAffregCallback(void *nil, SqlColumn * columns);

    STATUS GetVmRepels(const vector<int64>& vids, vector<VmRepel>& repels);
    
    STATUS GetVmAffregs(const vector<int64>& vids, vector<VmAffReg>& affregs);

    STATUS SetVmRepels(const vector<VmRepel>& repels);

    STATUS GetTotalSize(int64 vid, int position, int64 &size);

    int    GetVidsInFullInfo(vector<int64>& vids,
                             const string& where);
                                  
    int64  GetVmCfgCount(const string& where);
    
    int64  GetVmCfgOwner(int64 vid);
    
    STATUS GetRunningCluster(int64 vid,  string& cluster);
    int64 GetTotalSize(int64 vid, int position);
    bool IsVmDeploying(int64 vid);
    bool IsVmCanceling(int64 vid);
    bool IsVmModifying(int64 vid);

    STATUS GetARByVid(int64 vid, AffinityRegion & ar);
    STATUS GetVidsInAR(vector<int64> &vids, int64 arid);
    STATUS GetClusterWithARByVid(string &cc, int64 vid);
    STATUS GetVmRepelWithAR(int64 arid, vector<VmRepel>& repels);
    int AddVmInAR(int64 vid, int64 ar_id);
    int DeleteVmFromAR(int64 vid, int64 ar_id);
    int GetRepelWithARCallback(void *nil, SqlColumn * columns);
    int GetARCallback(void *nil, SqlColumn * columns);
    int Execute(ostringstream& cmd);
/*******************************************************************************
* 2. protected section
*******************************************************************************/
protected:

    enum ColNames
    {
        OID            = 0,   
        NAME           = 1,   
        DESCRIPTION    = 2, 
        CREATE_TIME    = 3,
        VCPU           = 4, 
        TCU            = 5,
        MEMORY         = 6, 
        VIRT_TYPE      = 7,
        ROOT_DEVICE    = 8,
        KERNEL_COMMAND = 9,
        BOOTLOADER     = 10,
        RAW_DATA       = 11,
        VNC_PASSWORD   = 12,
        QUALIFICATION  = 13,
        WATCHDOG       = 14,
        APPOINTED_CLUSTER = 15,
        APPOINTED_HOST    = 16, 
        PROJECT_ID        = 17,
        DEPLOYED_CLUSTER  = 18,
        CONFIG_VERSION    = 19,
        PROJECT_NAME      = 20,
        UID               = 21,
        USER_NAME         = 22,  
        ENABLE_COREDUMP   = 23,
        ENABLE_SERIAL     = 24,
        HYPERVISOR        = 25,
        ENABLE_LIVEMIGRATE   = 26,
        LIMIT             = 27
    };

    enum ViewDiskImageColNames 
    {
        VIEW_VID     = 0,
        VIEW_IMAGE   = 1,
        VIEW_DISK    = 2,
        VIEW_TOTAL   = 3,
        VIEW_DISKIMAGE_LIMIT   = 4       
    };

    enum ViewRepelTabColNames 
    {
        VID1     = 0,
        UID1     = 1,
        VID2     = 2,
        UID2     = 3,
        VIEW_REPELTAB_LIMIT   = 4       
    };

    enum ViewRepelWithARColNames
    {
        VIEW_ARID      = 0,
        VIEW_VID1      = 1,
        VIEW_VID2      = 2,
        VIEW_LEVEL     = 3,
        VIEW_REPELAR_LIMIT   = 4
    };

    enum ViewVMWithARColNames
    {
        VIEW_AR_VID        = 0,
        VIEW_AR_ID         = 1,
        VIEW_AR_NAME       = 2,
        VIEW_AR_DES        = 3,
        VIEW_AR_TIME       = 4,
        VIEW_AR_UID        = 5,
        VIEW_AR_LEVEL      = 6,
        VIEW_AR_FORCED     = 7,
        VIEW_VMAR_LIMIT   = 8
    };

    int GetCallback(void *nil, SqlColumn * columns);

    int SelectCallback(void *nil, SqlColumn * columns);

    int SearchRepelTabCallback(void *nil, SqlColumn * columns);
    
      
/*******************************************************************************
* 3. private section
*******************************************************************************/
private:

    VmCfgPool(SqlDB *pDb):SqlCallbackable(pDb){ };
    
    int GetVmCfgFullInfoCallback(void *nil, SqlColumn *columns);
    
    static VmCfgPool   *_instance;
    static int64        _lastOID;

    static pthread_mutex_t  _mutex;

    static const char * _table_vmcfg;
    static const char * _table_vmcfg_deleted;
    static const char * _table_vmcfg_repel;
    static const char * _table_vmcfg_affinity_region;
    static const char * _view_vmcfg_repel;    
    static const char * _view_vmcfg_with_project;
    static const char * _view_vmcfg_with_affinity_region;
    static const char * _view_repel_with_affinity_region;
    static const char * _view_project_with_cluster;
    static const char * _view_vmcfg_disk_image_size;
    
    static const char * _col_names_vmcfg;
    static const char * _col_names_vmcfg_repel;
    static const char * _col_names_repel_with_affinity_region;
    static const char * _col_names_affinity_region;
    static const char * _col_names_vmcfg_with_affinity_region;
    static const char * _col_names_view_vmcfg_repel; 
    static const char * _col_names_view_vmcfg_with_project ;   
    static const char * _col_names_view_project_with_cluster;  
    static const char * _col_names_view_vmcfg_disk_image_size;  

    static const char  *   _view_vmcfg_full_info; 
    static vector<string>  _columns_full_info;
    static string          _columns_full_info_joined_string;

    #define FULL_INFO_COLUMN_ORDER(field) (find(_columns_full_info.begin(),_columns_full_info.end(),field) - _columns_full_info.begin())


    DISALLOW_COPY_AND_ASSIGN(VmCfgPool); 

};


}
#endif /* VMCFG_MANGER_VMCFG_POOL_H */

