#ifndef TECS_ERROR_CODE_H
#define TECS_ERROR_CODE_H
#include "errcode.h"

#define TECS_ERRCODE_START              SKY_ERROR_END+1
//Ä£Ó´ë£¬TECS_ERRCODE_STARTÄ»++
#define ERR_VNET_BEGIN                  (TECS_ERRCODE_START)
#define ERR_HOST_MANAGER_BEGIN          (TECS_ERRCODE_START + 1000) // 2001 = 1000 + 1 + 1000
#define ERR_POWER_AGENT_BEGIN           (TECS_ERRCODE_START + 1200) // 2201 = 1000 + 1 + 1200
#define ERR_CLUSTER_MANAGER_BEGIN       (TECS_ERRCODE_START + 2000)
#define ERROR_VERSION_BEGIN             (TECS_ERRCODE_START + 3000 )
#define ERR_IMAGE_MANAGER_BEGIN         (TECS_ERRCODE_START + 4000) //image   [5001, 5999)
#define ERR_PROJECT_BEGIN               (TECS_ERRCODE_START + 5000) //project   [6001, 6999)
#define ERR_VMTEMPLATE_BEGIN            (TECS_ERRCODE_START + 6000) //vmtemplate   [7001, 7999)
#define ERR_VMCFG_BEGIN                 (TECS_ERRCODE_START + 7000) //vmcfg   [8001, 8999)
#define ERR_FILE_STORAGE_BEGIN          (TECS_ERRCODE_START + 8000) //file   [9001, 9999)
#define ERR_VMHANDLER_BEGIN             (TECS_ERRCODE_START + 9000) //vmhandler   [10001, 10999)
#define ERR_HOST_STORAGE_BEGIN          (TECS_ERRCODE_START + 10000) //   [11001, 11999)
#define ERR_AFFINITY_REGION_BEGIN       (TECS_ERRCODE_START + 12000) //affinity_region [12001, 12999)

/*******************************************************************************
 host manager
*******************************************************************************/
//#define ERR_HOST_TIMEOUT                (ERR_HOST_MANAGER_BEGIN + 0)    // "OK, but not confimed by the host."
//#define ERR_HOST_FORCED_REGISTER        (ERR_HOST_MANAGER_BEGIN + 1)    // "OK, host is forced registered and reboot."
#define ERR_HOST_FORCED_DISABLE         (ERR_HOST_MANAGER_BEGIN + 2)    // "OK, host is forced disabled, please waite vms to be deleted."
#define ERR_HOST_OK                     (SUCCESS)    // "OK."
#define ERR_HANDLE_OK                   (ERR_HOST_MANAGER_BEGIN + 4)    // "OK." 2001 + 4 = 2005
#define ERR_TRUNK_OK                    (ERR_HOST_MANAGER_BEGIN + 5)    // "OK."
#define ERR_CLUSTER_CFG_OK              (ERR_HOST_MANAGER_BEGIN + 7)    // "OK."
#define ERR_TCU_OK                      (ERR_HOST_MANAGER_BEGIN + 8)    // "OK."
#define ERR_CMM_OK                      (ERR_HOST_MANAGER_BEGIN + 9)    // "OK."
#define ERR_HOST_NOT_EXIST              (ERR_HOST_MANAGER_BEGIN + 10)   // "Failed, host is not exist."
//#define ERR_HOST_NOT_FORBIDDEN          (ERR_HOST_MANAGER_BEGIN + 11)   // "Failed, host is not forbidden."
//#define ERR_HOST_NOT_REGISTERED         (ERR_HOST_MANAGER_BEGIN + 12)   // "Failed, host is not registered."
#define ERR_HOST_VMS_RUNNING            (ERR_HOST_MANAGER_BEGIN + 13)   // "Failed, there are some VMs running on the host."
#define ERR_HANDLE_ERROR                (ERR_HOST_MANAGER_BEGIN + 14)   // "Failed, command is error."
#define ERR_CLUSTER_TIMEOUT             (ERR_HOST_MANAGER_BEGIN + 15)   // "Failed, CC is not answered."
#define ERR_HANDLE_EMPTY                (ERR_HOST_MANAGER_BEGIN + 16)   // "Failed, command is empty."
#define ERR_HANDLE_FAILED               (ERR_HOST_MANAGER_BEGIN + 17)   // "Failed, command execute failed."
//#define ERR_PORT_ADD_FAILED             (ERR_HOST_MANAGER_BEGIN + 20)   // "Error, create a new port config failed."
#define ERR_CLUSTER_NOT_REGISTER        (ERR_HOST_MANAGER_BEGIN + 28)   // "Error, cluster have not registered into the tecs center."
#define ERR_CLUSTER_CFG_NAME_INVALID    (ERR_HOST_MANAGER_BEGIN + 29)   // "Error, cluster config name is illegal."
#define ERR_CLUSTER_CFG_SET_FAILD       (ERR_HOST_MANAGER_BEGIN + 30)   // "Failed, set cluster config failed."
#define ERR_CLUSTER_CFG_DEL_FAILD       (ERR_HOST_MANAGER_BEGIN + 31)   // "Failed, delete cluster config failed."
#define ERR_CPUINFO_ID_NOT_EXIST        (ERR_HOST_MANAGER_BEGIN + 32)   // "Error, cpu_info_id is not exist."
#define ERR_TCU_CFG_INVALID             (ERR_HOST_MANAGER_BEGIN + 33)   // "Error, cpu_info and cpu_info_id are both valid."
#define ERR_TCU_SET_FAILD               (ERR_HOST_MANAGER_BEGIN + 34)   // "Failed, set tcu num config failed."
#define ERR_TCU_DEL_FAILD               (ERR_HOST_MANAGER_BEGIN + 35)   // "Failed, delete tcu config failed."
#define ERR_TCU_QUERY_FAILD             (ERR_HOST_MANAGER_BEGIN + 36)   // "Failed, query tcu config failed."
#define ERR_HM_AUTHORIZE_FAILED         (ERR_HOST_MANAGER_BEGIN + 37)   // "Error, failed to authorize usr's operation."
#define ERR_HM_PARA_INVALID             (ERR_HOST_MANAGER_BEGIN + 38)   // "Error, invalide parameter."
#define ERR_TCU_OPERATE_FAILD           (ERR_HOST_MANAGER_BEGIN + 43)   // "Failed, cpu is used by vm."
#define ERR_HOST_NOT_SUPPORT            (ERR_HOST_MANAGER_BEGIN + 44)   // "Failed, the host is not support eht operation"
#define ERR_HOST_THREAD_POOL_FAIL       (ERR_HOST_MANAGER_BEGIN + 45)   // "Failed, host_manager create thread pool fail"
#define ERR_HOST_DB_FAIL                (ERR_HOST_MANAGER_BEGIN + 46)   // "Failed, host_manager update config failed"
#define ERR_HOST_CTRL_HANDLE_FAIL       (ERR_HOST_MANAGER_BEGIN + 47)   // "Failed, host_manager extern ctrl failed"
#define ERR_CLUSTER_CFG_PARA_ERR        (ERR_HOST_MANAGER_BEGIN + 48)   // "Failed, set cluster config para error."
#define ERR_CMM_ADD_FAIL                (ERR_HOST_MANAGER_BEGIN + 67)   // "Failed, cluster agent add cmm fail."
#define ERR_CMM_SET_FAIL                (ERR_HOST_MANAGER_BEGIN + 68)   // "Failed, cluster agent set cmm fail."
#define ERR_CMM_DEL_FAIL                (ERR_HOST_MANAGER_BEGIN + 69)   // "Failed, cluster agent delete cmm fail."
#define ERR_CMM_QUERY_FAIL              (ERR_HOST_MANAGER_BEGIN + 70)   // "Failed, cluster agent query cmm fail."
#define ERR_TCU_VMS_RUNNING             (ERR_HOST_MANAGER_BEGIN + 71)   // "Failed, set tcu num config failed because there are some VMs running."

#define ERR_OPERATE_NOT_SUPPORT         (ERR_HOST_MANAGER_BEGIN + 72)   // "Failed, operation not support."
#define ERR_CMM_IP_NULL                 (ERR_HOST_MANAGER_BEGIN + 73)   // "Failed, IP of cmm not configured."
#define ERR_CMM_CFG_INCORRECT           (ERR_HOST_MANAGER_BEGIN + 74)   // "Failed, cmm configured incorrect."

/*******************************************************************************
 power agent
*******************************************************************************/
#define ERR_POWER_AGENT_SUCCESS               (SUCCESS)                     // "SUCCESS."
#define ERR_HOST_NOT_SUPPORT_RESET            (ERR_POWER_AGENT_BEGIN + 1)   // "Failed, host can not reset at this time."

/*******************************************************************************
  ÈºÄ´ 
*******************************************************************************/
#define ERR_CM_SUCCESS                  (SUCCESS)                         // "SUCCESS, ÈºÉ¹."
#define ERR_CM_CREATE_FAIL              (ERR_CLUSTER_MANAGER_BEGIN + 1)   // "Failed, ÈºÊ§."
#define ERR_CM_CLUSTER_REGISTERD        (ERR_CLUSTER_MANAGER_BEGIN + 2)   // "Failed, Èº×¢."
#define ERR_CM_CLUSTER_NOT_EXIST        (ERR_CLUSTER_MANAGER_BEGIN + 3)   // "Failed, Èº."
//#define ERR_CM_CLUSTER_NOT_REGISTER     (ERR_CLUSTER_MANAGER_BEGIN + 4)   // "Failed, ÈºÎ´×¢."
#define ERR_CM_CLUSTER_NOT_FREE         (ERR_CLUSTER_MANAGER_BEGIN + 5)   // "Failed, ÈºÇ¿Ðµ."

/*******************************************************************************
 æ±?
*******************************************************************************/
#define ERROR_LOAD_FAIL                            (ERROR_VERSION_BEGIN +1)
#define ERROR_INSTALL_FAIL                       (ERROR_VERSION_BEGIN +2)
#define ERROR_VERSION_INFO_FAIL             (ERROR_VERSION_BEGIN +3)
#define ERROR_UNINSTALL_FAIL                       (ERROR_VERSION_BEGIN +4)
#define ERROR_NOT_INSTALL                       (ERROR_VERSION_BEGIN +5)
#define ERROR_QUERY_FAIL                       (ERROR_VERSION_BEGIN +6)

/*******************************************************************************
  Ó³
*******************************************************************************/
#define ERR_IMAGE_FILE_NOT_EXIST         (ERR_IMAGE_MANAGER_BEGIN + 0) // "Failed, file not exist"
#define ERR_IMAGE_HAVE_REGISTERED        (ERR_IMAGE_MANAGER_BEGIN + 1) // "Failed, image have already registered"
#define ERR_IMAGE_IS_USING               (ERR_IMAGE_MANAGER_BEGIN + 2) // "Failed, image is using" 
#define ERR_IMAGE_INVALID_QUERY_TYPE     (ERR_IMAGE_MANAGER_BEGIN + 3) // "Failed, invalid query type"
#define ERR_IMAGE_IS_DISABLE             (ERR_IMAGE_MANAGER_BEGIN + 4) // "Failed, image is disable"
#define ERR_IMAGE_NOT_PUBLIC             (ERR_IMAGE_MANAGER_BEGIN + 5) // "Failed, image not public"
#define ERR_IMAGE_IS_DEPLOYED            (ERR_IMAGE_MANAGER_BEGIN + 6) // "Failed, image is deployed" 

/*******************************************************************************
   
*******************************************************************************/
#define ERR_RPOJECT_SUCCESS                (SUCCESS)                         // "SUCCESS"
#define ERR_PROJECT_EXIST                  (ERR_PROJECT_BEGIN+1)             // "project have exist"
#define ERR_PROJECT_AUTH_FAIL              (ERR_PROJECT_BEGIN+2)             // "È¨Ê§"
#define ERR_PROJECT_ALLOC_FAIL             (ERR_PROJECT_BEGIN+3)             // "Ê§"
#define ERR_PROJECT_NOT_EXIST              (ERR_PROJECT_BEGIN+4)             // "í®‹í»´"
#define ERR_PROJECT_UPDATA_FAIL            (ERR_PROJECT_BEGIN+5)             // "Â¶Ê§"
#define ERR_PROJECT_QUERY_TYPE_ERR         (ERR_PROJECT_BEGIN+6)             // "Ñ¯Í´"
#define ERR_PROJECT_QUERY_FAIL             (ERR_PROJECT_BEGIN+7)             // "Ñ¯Ý¿Ê§"
#define ERR_PROJECT_DELETE_DB_FAIL         (ERR_PROJECT_BEGIN+8)             // "É¾Ý¿Ê§"
#define ERR_RPOJECT_HAVE_VM                (ERR_PROJECT_BEGIN+9)             // "Ì»"
#define ERR_RPOJECT_TIMEOUT                (ERR_PROJECT_BEGIN+10)             // "?"
#define ERR_RPOJECT_PARA_INVALID           (ERR_PROJECT_BEGIN+11)             // "?"
#define ERR_RPOJECT_INVALID_ACTION_TYPE    (ERR_PROJECT_BEGIN+12)             // "?"


/*******************************************************************************

*******************************************************************************/
#define ERR_AFFINITY_REGION_SUCCESS         (SUCCESS)                         // "SUCCESS"
#define ERR_AFFINITY_REGION_EXIST           (ERR_AFFINITY_REGION_BEGIN+1)
#define ERR_AFFINITY_REGION_AUTH_FAIL       (ERR_AFFINITY_REGION_BEGIN+2)
#define ERR_AFFINITY_REGION_ALLOC_FAIL      (ERR_AFFINITY_REGION_BEGIN+3)
#define ERR_AFFINITY_REGION_NOT_EXIST       (ERR_AFFINITY_REGION_BEGIN+4)
#define ERR_AFFINITY_REGION_UPDATA_FAIL     (ERR_AFFINITY_REGION_BEGIN+5)
#define ERR_AFFINITY_REGION_QUERY_TYPE_ERR  (ERR_AFFINITY_REGION_BEGIN+6)
#define ERR_AFFINITY_REGION_QUERY_FAIL      (ERR_AFFINITY_REGION_BEGIN+7)
#define ERR_AFFINITY_REGION_DELETE_DB_FAIL  (ERR_AFFINITY_REGION_BEGIN+8)
#define ERR_AFFINITY_REGION_HAVE_VM         (ERR_AFFINITY_REGION_BEGIN+9)
#define ERR_AFFINITY_REGION_HAVE_REPELVM    (ERR_AFFINITY_REGION_BEGIN+10)
#define ERR_AFFINITY_REGION_TIMEOUT         (ERR_AFFINITY_REGION_BEGIN+11)
#define ERR_AFFINITY_REGION_PARA_INVALID    (ERR_AFFINITY_REGION_BEGIN+12)


/*******************************************************************************
   Ä£
*******************************************************************************/
#define ERR_VMTEMPLATE_INVALID_QUERY_TYPE    (ERR_VMTEMPLATE_BEGIN + 0)// "Failed, invalid query type"
#define ERR_VMTEMPLATE_ACQUIRE_IMAGE_FAILED  (ERR_VMTEMPLATE_BEGIN + 1)//"Failed to acquire image"
#define ERR_VMTEMPLATE_NAME_CONFLICT         (ERR_VMTEMPLATE_BEGIN + 2)//"Failed, vmtemplate name conflict"
#define ERR_VMTEMPLATE_NOT_PUBLIC            (ERR_VMTEMPLATE_BEGIN + 3) // "Failed, vmtemplate not public"
#define ERR_VMTEMPLATE_IMAGE_NOT_PUBLIC      (ERR_VMTEMPLATE_BEGIN + 4) // "Failed, vmtemplate not public because image is not public"

/*******************************************************************************
   
*******************************************************************************/
#define ERR_VMCFG_ACQUIRE_IMAGE_FAILED       (ERR_VMCFG_BEGIN + 0)//"Failed to acquire image"
#define ERR_VMCFG_INVALID_PROJECT            (ERR_VMCFG_BEGIN + 1)//"Failed, invalide project"
#define ERR_VMCFG_ACQUIRE_VMTEMPLATE_FAILED  (ERR_VMCFG_BEGIN + 2)//"Failed to acquire vmtemplate"
#define ERR_VMCFG_STATE_FORBIDDEN            (ERR_VMCFG_BEGIN + 3)//"Failed, the operation is forbidden in current state"
#define ERR_VMCFG_NO_SUITABLE_CLUSTER        (ERR_VMCFG_BEGIN + 4)//"Failed, no cluster is suitable to the vm"
#define ERR_VMCFG_ACTION_FAILED_IN_CC        (ERR_VMCFG_BEGIN + 5)//"Failed to action vm in cluster"
#define ERR_VMCFG_ACTION_FAILED_IN_HC        (ERR_VMCFG_BEGIN + 6)//"Failed to action vm in host"
#define ERR_VMCFG_INVALID_QUERY_TYPE         (ERR_VMCFG_BEGIN + 7)// "Failed, invalid query type"
#define ERR_VMCFG_INVALID_ACTION_TYPE        (ERR_VMCFG_BEGIN + 8)// "Failed, invalid action type"
#define ERR_VMCFG_MODIFY_FORBIDDEN           (ERR_VMCFG_BEGIN + 9)//"Failed, modify is forbidden for VM has deployed"
#define ERR_VMCFG_ALLOCATE_MAC               (ERR_VMCFG_BEGIN + 10)//"Failed to allocate mac address"
#define ERR_VMCFG_RELEASE_MAC                (ERR_VMCFG_BEGIN + 11)//"Failed to release mac address"
#define ERR_VM_NOT_EXIST                     (ERR_VMCFG_BEGIN + 12)//
#define ERR_VMCFG_EXIST_IN_DELETED_TAB       (ERR_VMCFG_BEGIN + 13)//"Failed, VM exist in VM deleted tab"
#define ERR_VMCFG_INSERT_DELETED_TAB         (ERR_VMCFG_BEGIN + 14)//"Failed to insert to VM deleted tab"
#define ERR_VMCFG_DEPLOY_IMAGE_FAILED        (ERR_VMCFG_BEGIN + 15)//"Failed to deploy the images of the VM"
#define ERR_VMCFG_IMAGE_DEPLOYING            (ERR_VMCFG_BEGIN + 16)//"Failed, the images of the VM are deploying"

#define ERR_VMCFG_LACK_COMPUTE               (ERR_VMCFG_BEGIN + 17)//"Failed, no enough compute resource"
#define ERR_VMCFG_LACK_SHARE_STORAGE         (ERR_VMCFG_BEGIN + 18)//"Failed, no enough share storage resource"
#define ERR_VMCFG_LACK_NETWORK               (ERR_VMCFG_BEGIN + 19)//"Failed, no enough network resource"
#define ERR_VMCFG_GET_COREDUMP_URL           (ERR_VMCFG_BEGIN + 20)//"Failed to get VM coredump url"
#define ERR_VMCFG_PREPARE_DEPLOY             (ERR_VMCFG_BEGIN + 21)//"Failed to generate deploy msg"
#define ERR_VMCFG_ALLOCATE_NETWORK_TC        (ERR_VMCFG_BEGIN + 22)//"Failed, allocate network resource failed in TC"


/* Ä¼Ñ¯ */
#define ERR_FILE_SUCCESS                (SUCCESS)                         // "SUCCESS, Ä¼É¹."
#define ERR_FILE_USER_NAME_NOT_EXIST    (ERR_FILE_STORAGE_BEGIN + 1)      // "Ã»"
#define ERR_FILE_QUERY_FAILED           (ERR_FILE_STORAGE_BEGIN + 2)      // "Ñ¯Ê§"
#define ERR_FILE_AUTH_FAIL              (ERR_FILE_STORAGE_BEGIN + 3)      // "È¨Ê§"
#define ERR_FILE_QUERY_TYPE_ERR         (ERR_FILE_STORAGE_BEGIN + 4)      // "Ð§Ñ¯"

/* vm instance  */
#define VMOP_SUCC                       (SUCCESS)
#define VMOP_RUNNING                    (ERROR_ACTION_RUNNING)         // Ú²
#define VMOP_TIME_OUT                  (ERROR_TIME_OUT)
#define CREATE_ERROR_EXISTINSTANCE      (ERR_VMHANDLER_BEGIN + 1)
#define CREATE_ERROR_NEWINSTANCE        (ERR_VMHANDLER_BEGIN + 2)
#define CREATE_ERROR_PTHREADCREATE      (ERR_VMHANDLER_BEGIN + 3)
#define CREATE_ERROR_MKDIR              (ERR_VMHANDLER_BEGIN + 4)
#define CREATE_ERROR_STARTNETWORK       (ERR_VMHANDLER_BEGIN + 5)
#define CREATE_ERROR_STARTSTORAGE       (ERR_VMHANDLER_BEGIN + 6)
#define CREATE_ERROR_CREATECONTEXT      (ERR_VMHANDLER_BEGIN + 7)
#define CREATE_ERROR_CREATEISO          (ERR_VMHANDLER_BEGIN + 8)
#define CREATE_ERROR_CREATEXML          (ERR_VMHANDLER_BEGIN + 9)
#define CREATE_ERROR_CREATECP           (ERR_VMHANDLER_BEGIN + 10)
#define CREATE_ERROR_CREATEDOMAIN       (ERR_VMHANDLER_BEGIN + 11)
#define VMOP_ERROR_NOTEXIST             (ERR_VMHANDLER_BEGIN + 12)
#define VMOP_ERROR_NETWORK              (ERR_VMHANDLER_BEGIN + 13)  //Îªcancel
#define VMOP_ERROR_STORAGE              (ERR_VMHANDLER_BEGIN + 14)  //Îªcancel
#define VMOP_ERROR_DIR                  (ERR_VMHANDLER_BEGIN + 15)  //Îªcancel
#define VMOP_ERROR_UNKNOWN              (ERR_VMHANDLER_BEGIN + 16)
#define VMOP_ERROR_REBOOT_FAST          (ERR_VMHANDLER_BEGIN + 17)  // 1·ÖÖÓÄÚ½ûÖ¹¶à´ÎÖØÆô
#define VMOP_ERROR_CANCEL_ABORT         (ERR_VMHANDLER_BEGIN + 18)  // ·ÅÆú³·ÏúÐéÄâ»ú
#define VMOP_VM_DO_FAIL                 (ERR_VMHANDLER_BEGIN + 19)  // Çý¶¯½Ó¿Úµ÷ÓÃÊ§°Ü
#define VMOP_VM_AGT_BUSY                (ERR_VMHANDLER_BEGIN + 20)  // ÐéÄâ»ú´úÀíÕýÃ¦
#define VMOP_ERROR_WATCHDOG             (ERR_VMHANDLER_BEGIN + 21)  // Å¹Ê§
#define VMOP_ERROR_STATE                (ERR_VMHANDLER_BEGIN + 22)   // ×´Ì¬        
#define VMOMP_UPLOAD_ERROR              (ERR_VMHANDLER_BEGIN + 23)   // Ï´Ê§  
#define VMOP_USB_NOTEXIST               (ERR_VMHANDLER_BEGIN + 24)   // USBè±?
#define VMOP_RECOVER_ERROR              (ERR_VMHANDLER_BEGIN + 25)   // Ò»¼ü»Ö¸´Ê§°Ü
#define VMOP_VM_DO_TIMEOUT              (ERR_VMHANDLER_BEGIN + 26)   // ÃüÁîÖ´ÐÐ³¬Ê±
#define VMOP_ERROR_NETWORK_START        (ERR_VMHANDLER_BEGIN + 27)  // Æô¶¯ÍøÂç
#define VMOP_ERROR_NETWORK_STOP         (ERR_VMHANDLER_BEGIN + 28)  // ÊÍ·ÅÍøÂç
#define VMOP_ERROR_NETWORK_NOTIFY       (ERR_VMHANDLER_BEGIN + 29)  // Í¨ÖªÍøÂçdomId±ä»¯
#define VMOP_ERROR_WATCHDOG_START       (ERR_VMHANDLER_BEGIN + 30)  // ¿ªÆô¿´ÃÅ¹·
#define VMOP_ERROR_WATCHDOG_STOP        (ERR_VMHANDLER_BEGIN + 31)  // ¹Ø±Õ¿´ÃÅ¹·
#define VMOP_ERROR_MOUNT_COREDUMP_URL   (ERR_VMHANDLER_BEGIN + 32)  // ¹ÒÔØcoredumpÂ·¾¶Ê§°Ü
#define VMOP_ERROR_UMOUNT_COREDUMP_URL  (ERR_VMHANDLER_BEGIN + 33)  // È¡Ïû¹ÒÔØcoredumpÂ·¾¶Ê§°Ü
#define VMOP_ERROR_LOAD_SHARE_DISK      (ERR_VMHANDLER_BEGIN + 34)  //  ¼ÓÔØ¹²ÏíÅÌ
#define VMOP_ERROR_UNLOAD_SHARE_DISK    (ERR_VMHANDLER_BEGIN + 35)  //  Ð¶ÔØ¹²ÏíÅÌ
#define VMOP_ERROR_STORAGE_START        (ERR_VMHANDLER_BEGIN + 36)  //  ×¼±¸´æ´¢
#define VMOP_ERROR_STORAGE_RELEASE      (ERR_VMHANDLER_BEGIN + 37)  //  ÊÍ·Å´æ´¢

/*host storage err code*/
#define ERR_HS_DEQUEUE_OK               (ERR_HOST_STORAGE_BEGIN + 1)
#define ERR_SC_TIMEOUT                  (ERR_HOST_STORAGE_BEGIN + 2)
#define ERR_SC_AUTHORIZE_FAILED         (ERR_HOST_MANAGER_BEGIN + 3)   // "Error, failed to authorize usr's operation."
#define ERR_SC_PARA_INVALID             (ERR_HOST_MANAGER_BEGIN + 4)  
#endif //TECS_ERROR_CODE_H

