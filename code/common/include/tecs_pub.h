#ifndef TECS_COMMON_PUB_H
#define TECS_COMMON_PUB_H
#include "pub.h"
namespace zte_tecs
{
typedef int64                     OBJECT_ID; 
typedef OBJECT_ID                 FILE_ID; 
typedef OBJECT_ID                 IMAGE_ID; 
#define INVALID_OID         (-1)
#define INVALID_FILEID      INVALID_OID
#define INVALID_VID          INVALID_OID
#define INVALID_UID          INVALID_OID
#define INVALID_HID          INVALID_OID
#define INVALID_SID          INVALID_OID
#define INVALID_LUNID        INVALID_OID
#define INVALID_IID          INVALID_OID
#define INVALID_MAC         ((int32)-1)
#define INVALID_BOARD_TYPE  ((int32)-1)

// 定义当WEB或者HC上的FTP客户端使用特殊用户名进行登录的用户名称
#define MAGIC_USER      "#$%Tecs_Web_Ftp_Unique_User%$#"

inline float BytesToKB(int64 bytes)
{
    return bytes/((float)1024);
}

inline int64 KBToBytes(int64 KB)
{
    return KB*1024;
}

inline float BytesToMB(int64 bytes)
{
    return bytes/((float)1024*1024);
}

inline int64 MBToBytes(int MB)
{
    return ((int64)MB)*1024*1024;
}

inline float BytesToGB(int64 bytes)
{
    return bytes/((float)1024*1024*1024);
}

inline int64 GBToBytes(int GB)
{
    return ((int64)GB)*1024*1024*1024;
}

inline float BytesToTB(int64 bytes)
{
    return bytes/((float)1024*1024*1024*1024);
}

inline int64 TBToBytes(int TB)
{
    return ((int64)TB)*1024*1024*1024*1024;
}

inline string ZteMacInt2Str(int32 mac)
{
    int32 mac2,mac1,mac0;
    stringstream ss;
    
    mac2 = (mac>>16) & 0x000000FF;
    mac1 = (mac>>8) & 0x000000FF;
    mac0 = (mac) & 0x000000FF;
    
    ss << "00:19:C6:" 
        << hex << setfill('0') << setw(2) << mac2 << ":"
        << hex << setfill('0') << setw(2) << mac1 << ":"
        << hex << setfill('0') << setw(2) << mac0;

    return ss.str();
}

inline int32 ZteMacStr2Int(const string &mac)
{
    uint32 buf[6] = {0};
    BYTE  ucMac2, ucMac1, ucMac0;
    int32 tmp = 0;
    
    if (sscanf(mac.c_str(),"%x:%x:%x:%x:%x:%x",buf,buf+1,buf+2,buf+3,buf+4,buf+5) != 6)
    {
        return INVALID_MAC;
    }

    ucMac2 = (BYTE)buf[3];
    ucMac1 = (BYTE)buf[4];
    ucMac0 = (BYTE)buf[5];

    tmp = (int32) ((ucMac2<<16) | (ucMac1<<8) | (ucMac0));

    return tmp;
}

/*******************************************************************************
  工作流相关的宏定义
  注意: 1. 请使用全小写的英文合法单词，不要使用空格
        2. 请保证接口稳定性, 这些名称会被web界面识别和使用， 
*******************************************************************************/


/*******************************************************************************
   TC 上虚拟机相关的
*******************************************************************************/
#define VM_WF_VIEW                  "vmm_workflow"              //从工作流拉出视图的名字
#define VM_WF_CATEGORY              "vm"                        //计算子系统相关工作流
#define VM_WF_DEPLOY_TO_CLUSTER     "deploy_to_cluster"          //  虚拟机部署正向工作流
#define VM_WF_CANCEL_FROM_CLUSTER   "cancel_from_cluster"        //  撤销虚拟机部署工作流
#define VM_WF_BATCH_MODIFY_VMCFG    "batch_modify_vmcfg"         //  批量修改虚拟机参数
#define VM_WF_MODIFY_INDIVIDUAL_VMCFG \
                                    "modify_individual_vmcfg"

#define VM_ACT_CATEGORY             "vm"          //  计算子系统相关动作
#define VM_ACT_FILTER_COMPUTE       "fileter_by_compute"
#define VM_ACT_FILTER_STORAGE       "fileter_by_strorage"
#define VM_ACT_FILTER_NETWORK       "fileter_by_network"
#define VM_ACT_SELECT_CLUSTER       "select_cluster"        //  挑选cluster ACTION

#define VM_ACT_DEPLOY_IMAGE         "deploy_image"         //  部署镜像 ACTION
#define VM_ACT_GET_COREDUMP_URL     "get_coredump_url"        //  获取coredump_url ACTION
#define VM_ACT_DEPLOY_IMAGE         "deploy_image"
#define VM_ACT_ALLOCATE_NETWORK     "allocate_network"
#define VM_ACT_RELEASE_NETWORK      "release_network"
#define VM_ACT_GENERATE_DEPLOY      "generate_deploy_request"        //  构造部署消息ACTION
#define VM_ACT_SEND_DEPLOY          "send_deploy_request"        //  部署vm到CC ACTION
#define VM_ACT_SEND_CANCEL          "send_cancel_request"        //  撤销CC上的虚拟机ACTION
#define VM_ACT_MODIFY_VMCFG_INDIVIDUAL_ITEM \
                                    "modify_vmcfg_individual_item"

/*******************************************************************************
   TC 上工程相关的
*******************************************************************************/
#define PROJECT_CATEGORY              "project"                    
#define SAVE_AS_IMAGE                 "save_as_image"   
#define CREATE_BY_IMAGE               "create_by_image"   
#define PROJECT_WF_OPERATE            "operate_project"
#define PROJECT_ACT_DEPLOY            "deploy_project"   
#define PROJECT_ACT_CANCEL            "cancel_project"   
#define PROJECT_ACT_REBOOT            "reboot_project" 
#define PROJECT_ACT_RESET             "reset_project"


/*******************************************************************************
   CC 上虚拟机相关的Action
*******************************************************************************/
#define VM_WF_RESCH_CATEGORY            "vm_resch"
#define VM_ACT_RESCH_CATEGORY            "vm_resch"

#define VM_WF_DEPLOY                    "deploy_to_host"
#define VM_WF_CANCEL                    "cancel_from_host"                    
#define VM_WF_CREATE                    "create"
#define VM_WF_START                     "start"
#define VM_WF_STOP                      "stop"
#define VM_WF_DESTROY                   "destroy"
#define VM_WF_REBOOT                    "reboot"
#define VM_WF_RESET                     "reset"
#define VM_WF_PAUSE                     "pause"                     
#define VM_WF_RESUME                    "resume"                    
#define VM_WF_MIGRATE                   "live_migrate"                    
#define VM_WF_COLD_MIGRATE              "cold_migrate"                    
#define VM_WF_UPLOAD_IMG                "upload_img"                    
#define VM_WF_CANCEL_UPLOAD_IMG         "cancel_upload_img"                    
#define VM_WF_PLUG_IN_USB               "plug_in_usb"
#define VM_WF_PLUG_OUT_USB              "plug_out_usb"
#define VM_WF_OUT_IN_USB                "out_in_usb"  
#define VM_WF_PLUG_IN_PDISK             "plug_in_pdisk"  
#define VM_WF_PLUG_OUT_PDISK            "plug_out_pdisk"  
#define VM_WF_RECOVER_IMAGE             "recover_image"  
#define VM_WF_RESTORE                   "vm_restore"
#define VM_WF_IMAGE_BACKUP_CREATE       "image_backup_create"  
#define VM_WF_IMAGE_BACKUP_DELETE       "image_backup_delete"  
#define VM_WF_HA_MIGRATE                "vm_ha_migrate"  
#define VM_WF_SCH_RUN_RESCH             "vm_sch_run_resch"  
#define VM_WF_FREEZE                    "freeze"  
#define VM_WF_UNFREEZE                  "unfreeze"  
#define VM_WF_MODIFY_VMCFG              "modify_vmcfg"  

#define VM_ACT_SELECT_HC                "select_hc"
#define VM_ACT_SELECT_HC_BY_NIC         "select_hc_by_nic"
#define VM_ACT_PREPARE_NIC              "prepare_nic"
#define VM_ACT_RELEASE_NIC              "release_nic"
#define VM_ACT_BEGIN_M_NIC              "begin_migrate_nic"
#define VM_ACT_END_M_NIC                "end_migrate_nic"
#define VM_ACT_UNSELECT_HC              "unselect_hc"
#define VM_ACT_CREATE                   "create"
#define VM_ACT_REMOVE                   "remove"
#define VM_ACT_START                    "start"
#define VM_ACT_STOP                     "stop"
#define VM_ACT_DESTROY                  "destroy"
#define VM_ACT_REBOOT                   "reboot"
#define VM_ACT_RESET                    "reset"
#define VM_ACT_PAUSE                    "pause"
#define VM_ACT_RESUME                   "resume"
#define VM_ACT_PRAPARE_M                "prepare_migrate"
#define VM_ACT_MIGRATE                  "migrate"
#define VM_ACT_COLD_MIGRATE             "cold_migrate"                    
#define VM_ACT_POST_M                   "post_migrate"
#define VM_ACT_UPLOAD_IMG               "upload_img"
#define VM_ACT_CANCEL_UPLOAD_IMG        "cancel_upload_img"
#define VM_ACT_PLUG_IN_USB              "plug_in_usb"
#define VM_ACT_PLUG_OUT_USB             "plug_out_usb"
#define VM_ACT_OUT_IN_USB               "out_in_usb"
#define VM_ACT_PREPARE_IMAGE            "prepare_image_url"
#define VM_ACT_RELEASE_IMAGE            "release_image_url"
#define VM_ACT_CREATE_BLOCK             "create_share_block"
#define VM_ACT_DELETE_BLOCK             "delete_share_block"
#define VM_ACT_AUTH_BLOCK               "auth_share_block"
#define VM_ACT_DEAUTH_BLOCK             "deauth_share_block"
#define VM_ACT_GENERATE_CREATE_REQUEST  "generate_create_request"
#define VM_ACT_RESUME                   "resume"                    
#define VM_ACT_PLUG_IN_PDISK            "plug_in_pdisk"
#define VM_ACT_PLUG_OUT_PDISK           "plug_out_pdisk"  
#define VM_ACT_RECOVER_IMAGE            "recover_image"  
#define VM_ACT_BACKUP_IMAGE_CREATE      "backup_image_create"
#define VM_ACT_BACKUP_IMAGE_DELETE      "backup_image_delete"  
#define VM_ACT_DESTROY_VMS              "destroy_vms"  
#define VM_ACT_REBOOT_HC                "reboot_hc"  
#define VM_ACT_FREEZE                   "freeze"  
#define VM_ACT_UNFREEZE                 "unfreeze"  
#define VM_ACT_MODIFY_NEXT              "modify_next"  
#define VM_ACT_MODIFY_VMCFG             "modify_vmcfg"  
#define VM_ACT_UNBIND_VM_FROM_HOST      "unbind_vm_from_host"  
#define VM_ACT_ANSWER_UPSTREAM          "answer_upstream"  
#define VM_ACT_CANCEL                   "remove"



/*******************************************************************************
   SC上相关的Action
*******************************************************************************/
//#define STORAGE_WF_CATEGORY        "storage"      存储子系统工作流
//#define STORAGE_WF_AUTH                "auth"            存储认证操作工作流
//#define NETWORK_WF_CATEGORY       "network"     网络子系统相关工作流
//#define IMAGE_WF_CATEGORY             "image"

#define IMAGE_CATEGORY         "image"
#define IM_DEPLOY_IMAGE        "im_deploy_image"
#define IM_CANCEL_IMAGE        "im_cancel_image"
#define IA_DEPLOY_IMAGE        "ia_deploy_image"
#define IA_CANCEL_IMAGE        "ia_cancel_image"
#define DOWNLOAD_IMAGE         "download_image"
#define CANCEL_DOWNLOAD_IMAGE  "cancel_download_image"
#define PREPARE_IMAGE_URL      "prepare_image_url"
#define RELEASE_IMAGE_URL      "release_image_url"

#define STORAGE_CATEGORY       "storage"
#define CREATE_BLOCK           "create_block"
#define DELETE_BLOCK           "delete_block"
#define AUTH_BLOCK             "auth_block"
#define DEAUTH_BLOCK           "deauth_block"
#define CREATE_SNAPSHOT_BLOCK  "create_snapshot_block"
#define DELETE_SNAPSHOT_BLOCK  "delete_snapshot_block"

#define CREATE_VOLUME          "create_volume"
#define DELETE_VOLUME          "delete_volume"
#define CREATE_SNAPSHOT_VOLUME "create_snapshot_volume"
#define DELETE_SNAPSHOT_VOLUME "delete_snapshot_volume"

#define AUTH_VOLUME            "auth_volume"
#define CREATE_TARGET          "create_target"
#define ADD_HOST_TO_TARGET     "add_host_to_target"
#define ADD_VOLUME_TO_TARGET   "add_volume_to_target"
#define DEAUTH_VOLUME          "deauth_volume"
#define DELETE_TARGET          "delete_target"
#define DELETE_HOST_FROM_TARGET   "delete_host_from_target"
#define DELETE_VOLUME_FROM_TARGET "delete_volume_from_target"

}
#endif

