/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�api_const.h
* �ļ���ʶ��
* ����ժҪ��API�ӿڶ���ɼ��ĳ�����ö�ٶ���. �Ͻ��ڸ��ļ�����xml-rpc�����ݽṹ��
*           ���ļ�TECS��TC��CC��HC���ɼ���һ�����彫����CC��HC������xml-rpc�⣡
* ��ǰ�汾��1.0
* ��    �ߣ�������
* ������ڣ�2011��10��19��
*
* �޸ļ�¼1��
*     �޸����ڣ�2011/10/19
*     �� �� �ţ�V1.0
*     �� �� �ˣ�������
*     �޸����ݣ�����
******************************************************************************/
#ifndef TECS_API_CONST_H
#define TECS_API_CONST_H
/****************************************************************************/
/**   ϵͳ����ʱ���ģ��                      **/
/****************************************************************************/
#define RUNTIME_STATE_NORMAL        (int(0))
#define RUNTIME_STATE_NOTICE        (int(1))
#define RUNTIME_STATE_WARNNING      (int(2))
#define RUNTIME_STATE_ERROR         (int(3))

/****************************************************************************/
/**   ������������               **/
/****************************************************************************/

#define VMCONFIG_DISK            "DISK"
#define VMCONFIG_IMAGE_DISK      "IMAGE_DISK"
#define VMCONFIG_NETWORK         "NETWORK"

//���⻯����
#define VIRT_TYPE_HVM     "hvm"
#define VIRT_TYPE_PVM     "pvm"

//VMM����
#define HYPERVISOR_XEN     "xen"
#define HYPERVISOR_KVM     "kvm"
#define HYPERVISOR_ANY     "any"

//�׺��򼶱�����
#define AFFINITY_REGION_SHELF     "shelf"
#define AFFINITY_REGION_RACK      "rack"
#define AFFINITY_REGION_BOARD     "board"


// ����������VLAN �ŵ���Ч��Χ [0,4094]
#define VM_NIC_VLAN_MIN   (int(0))
#define VM_NIC_VLAN_MAX   (int(4094))

// ��������ѡ��
#define NETFRONT          "netfront"
#define E1000             "e1000"
#define RTL8139           "rtl8139"
#define NE2K_PCI          "ne2k_pci"
#define PCNET             "pcnet"

/************ ��������� ����������ǵ�����һ��������ڴ��г�  *************/

//����
#define VM_ACTION_DEPLOY   "deploy" 
//����
#define VM_ACTION_CANCEL   "cancel" 

//ɾ��
#define VM_ACTION_DELETE   "delete" 

//����
#define VM_ACTION_START    "start"  
//ֹͣ
#define VM_ACTION_STOP     "stop"  

//����
#define VM_ACTION_REBOOT   "reboot" 

//��ͣ
#define VM_ACTION_PAUSE    "pause"  
//�ָ�
#define VM_ACTION_RESUME   "resume"  

//ǿ������
#define VM_ACTION_RESET   "reset"

//ǿ�ƹػ�
#define VM_ACTION_DESTROY   "destroy"

//��������ͬ��
#define VM_ACTION_SYNC      "sync"

// ���������
#define VM_ACTION_UPLOAD_IMG      "upload_img"

// ȡ������������������涯�����ڽ��е�ʱ�򣬸�������Ч
#define VM_ACTION_CANCEL_UPLOAD_IMG    "cancel_upload_img"

// ����usb
#define VM_ACTION_PLUG_IN_USB    "plug_in_usb"

// �γ�USB
#define VM_ACTION_PLUG_OUT_USB    "plug_out_usb"

// ˢ��USB
#define VM_ACTION_OUT_IN_USB    "out_in_usb"

// ����pdisk
#define VM_ACTION_PLUG_IN_PDISK    "plug_in_pdisk"

// �γ�pdisk
#define VM_ACTION_PLUG_OUT_PDISK    "plug_out_pdisk"

// һ���ָ�
#define VM_ACTION_RECOVER_IMAGE    "recover_image"

// ����image
#define VM_ACTION_IMAGE_BACKUP_CREATE  "image_backup_create"
// ��������񱸷��޸�
#define VM_ACTION_MODIFY_IMAGE_BACKUP    "modify_image_backup"

// ��������񱸷ݻָ�
#define VM_ACTION_IMAGE_BACKUP_RESTORE    "image_backup_restore"

// ���������imageɾ��
#define VM_ACTION_IMAGE_BACKUP_DELETE  "image_backup_delete"

// ��Ǩ��
#define VM_ACTION_LIVE_MIGRATE    "live_migrate"

// ��Ǩ��
#define VM_ACTION_COLD_MIGRATE    "cold_migrate"

// ����
#define VM_ACTION_FREEZE    "freeze"

// �ⶳ
#define VM_ACTION_UNFREEZE    "unfreeze"

/****************************************************************************/
/**   ӳ�����              **/
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
/**   �������                    **/
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

#define DISK_MAX_SIZE        ((long long)500000000) //��λGB


/****************************************************************************/
/**   �������                    **/
/****************************************************************************/
#define PROJECT_NAME_DEFAULAT  "default"
//����
#define PJ_ACTION_DEPLOY     "project_deploy"  
//����
#define PJ_ACTION_CANCEL     "project_cancel"  
//����
#define PJ_ACTION_REBOOT     "project_reboot" 
//ǿ������
#define PJ_ACTION_RESET      "project_reset" 


/****************************************************************************/
/**   ������������                    **/
/****************************************************************************/
#define CFG_CONTEXT_MAX_SIZE       (100*1024)
#define CFG_SRIOV_NIC_MAX_NUM      16
#define CFG_NONSRIOV_NIC_MAX_NUM   8
#define CFG_DISK_MAX_NUM           16

/****************************************************************************/
/**   RPC��ѯ����                    **/
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
/**   ����ģ�����������              **/
/****************************************************************************/
typedef enum
{
    SDevice_PDH=1
}Device_Type;



#endif

