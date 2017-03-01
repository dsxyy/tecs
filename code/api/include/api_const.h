/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：api_const.h
* 文件标识：
* 内容摘要：API接口对外可见的常量、枚举定义. 严禁在该文件定义xml-rpc的数据结构，
*           本文件TECS内TC、CC、HC均可见，一旦定义将导致CC、HC需链接xml-rpc库！
* 当前版本：1.0
* 作    者：陈文文
* 完成日期：2011年10月19日
*
* 修改记录1：
*     修改日期：2011/10/19
*     版 本 号：V1.0
*     修 改 人：陈文文
*     修改内容：创建
******************************************************************************/
#ifndef TECS_API_CONST_H
#define TECS_API_CONST_H
/****************************************************************************/
/**   系统运行时监控模块                      **/
/****************************************************************************/
#define RUNTIME_STATE_NORMAL        (int(0))
#define RUNTIME_STATE_NOTICE        (int(1))
#define RUNTIME_STATE_WARNNING      (int(2))
#define RUNTIME_STATE_ERROR         (int(3))

/****************************************************************************/
/**   虚拟机配置相关               **/
/****************************************************************************/

#define VMCONFIG_DISK            "DISK"
#define VMCONFIG_IMAGE_DISK      "IMAGE_DISK"
#define VMCONFIG_NETWORK         "NETWORK"

//虚拟化类型
#define VIRT_TYPE_HVM     "hvm"
#define VIRT_TYPE_PVM     "pvm"

//VMM类型
#define HYPERVISOR_XEN     "xen"
#define HYPERVISOR_KVM     "kvm"
#define HYPERVISOR_ANY     "any"

//亲和域级别类型
#define AFFINITY_REGION_SHELF     "shelf"
#define AFFINITY_REGION_RACK      "rack"
#define AFFINITY_REGION_BOARD     "board"


// 虚拟网卡的VLAN 号的有效范围 [0,4094]
#define VM_NIC_VLAN_MIN   (int(0))
#define VM_NIC_VLAN_MAX   (int(4094))

// 网卡类型选择
#define NETFRONT          "netfront"
#define E1000             "e1000"
#define RTL8139           "rtl8139"
#define NE2K_PCI          "ne2k_pci"
#define PCNET             "pcnet"

/************ 虚拟机操作 虚拟机创建是单独的一条命令，不在此列出  *************/

//部署
#define VM_ACTION_DEPLOY   "deploy" 
//撤销
#define VM_ACTION_CANCEL   "cancel" 

//删除
#define VM_ACTION_DELETE   "delete" 

//启动
#define VM_ACTION_START    "start"  
//停止
#define VM_ACTION_STOP     "stop"  

//重启
#define VM_ACTION_REBOOT   "reboot" 

//暂停
#define VM_ACTION_PAUSE    "pause"  
//恢复
#define VM_ACTION_RESUME   "resume"  

//强制重启
#define VM_ACTION_RESET   "reset"

//强制关机
#define VM_ACTION_DESTROY   "destroy"

//配置数据同步
#define VM_ACTION_SYNC      "sync"

// 保存虚拟机
#define VM_ACTION_UPLOAD_IMG      "upload_img"

// 取消保存虚拟机，当保存动作正在进行的时候，该命令有效
#define VM_ACTION_CANCEL_UPLOAD_IMG    "cancel_upload_img"

// 插入usb
#define VM_ACTION_PLUG_IN_USB    "plug_in_usb"

// 拔出USB
#define VM_ACTION_PLUG_OUT_USB    "plug_out_usb"

// 刷新USB
#define VM_ACTION_OUT_IN_USB    "out_in_usb"

// 插入pdisk
#define VM_ACTION_PLUG_IN_PDISK    "plug_in_pdisk"

// 拔出pdisk
#define VM_ACTION_PLUG_OUT_PDISK    "plug_out_pdisk"

// 一键恢复
#define VM_ACTION_RECOVER_IMAGE    "recover_image"

// 备份image
#define VM_ACTION_IMAGE_BACKUP_CREATE  "image_backup_create"
// 虚拟机镜像备份修改
#define VM_ACTION_MODIFY_IMAGE_BACKUP    "modify_image_backup"

// 虚拟机镜像备份恢复
#define VM_ACTION_IMAGE_BACKUP_RESTORE    "image_backup_restore"

// 虚拟机备份image删除
#define VM_ACTION_IMAGE_BACKUP_DELETE  "image_backup_delete"

// 热迁移
#define VM_ACTION_LIVE_MIGRATE    "live_migrate"

// 冷迁移
#define VM_ACTION_COLD_MIGRATE    "cold_migrate"

// 冻结
#define VM_ACTION_FREEZE    "freeze"

// 解冻
#define VM_ACTION_UNFREEZE    "unfreeze"

/****************************************************************************/
/**   映像相关              **/
/****************************************************************************/
#define IMAGE_TYPE_KERNEL     "kernel"
#define IMAGE_TYPE_RAMDISK    "ramdisk"
#define IMAGE_TYPE_MACHINE    "machine"
#define IMAGE_TYPE_CDROM      "cdrom"
#define IMAGE_TYPE_DATABLOCK  "datablock"

#define IMAGE_ARCH_I386       "i386"
#define IMAGE_ARCH_X86_64     "x86_64"

#define IMAGE_FORMAT_VHD       "vhd"
#define IMAGE_FORMAT_RAW       "raw"
#define IMAGE_FORMAT_QCOW2     "qcow2"

/****************************************************************************/
/**   磁盘相关                    **/
/****************************************************************************/
#define DISK_TYPE_MACHINE    IMAGE_TYPE_MACHINE
#define DISK_TYPE_CDROM      IMAGE_TYPE_CDROM
#define DISK_TYPE_DATABLOCK  IMAGE_TYPE_DATABLOCK
#define DISK_TYPE_DISK       "disk"

#define DISK_BUS_IDE         "ide"
#define DISK_BUS_SCSI        "scsi"
#define DISK_BUS_XVD         "xvd"

#define DISK_POSITION_ANY    (int(0))
#define DISK_POSITION_LOCAL  (int(1))
#define DISK_POSITION_SHARE  (int(2))

#define DISK_MAX_SIZE        ((long long)500000000) //单位GB


/****************************************************************************/
/**   工程相关                    **/
/****************************************************************************/
#define PROJECT_NAME_DEFAULAT  "default"
//部署
#define PJ_ACTION_DEPLOY     "project_deploy"  
//撤销
#define PJ_ACTION_CANCEL     "project_cancel"  
//重启
#define PJ_ACTION_REBOOT     "project_reboot" 
//强制重启
#define PJ_ACTION_RESET      "project_reset" 


/****************************************************************************/
/**   虚拟机配置相关                    **/
/****************************************************************************/
#define CFG_CONTEXT_MAX_SIZE       (100*1024)
#define CFG_SRIOV_NIC_MAX_NUM      16
#define CFG_NONSRIOV_NIC_MAX_NUM   8
#define CFG_DISK_MAX_NUM           16

/****************************************************************************/
/**   RPC查询类型                    **/
/****************************************************************************/
#define RPC_QUERY_ALL_SELF            "query_all_self"
#define RPC_QUERY_ALL_APPOINTED       "query_all_appointed"
#define RPC_QUERY_ALL                 "query_all"
#define RPC_QUERY_SELF_APPOINTED      "query_self_appointed"

enum QUERY_TYPE
{
    QUERY_ALL_SELF             = 1,
    QUERY_ALL_APPOINTED        = 2,
    QUERY_ALL                  = 3,
    QUERY_SELF_APPOINTED       = 4
};

/****************************************************************************/
/**   其他模块在下面添加              **/
/****************************************************************************/
typedef enum
{
    SDevice_PDH=1
}Device_Type;



#endif

