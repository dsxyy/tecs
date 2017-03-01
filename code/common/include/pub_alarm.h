/*********************************************************************
* 版权所有 (C)2008, 深圳市中兴通讯股份有限公司。
*
* 文件名称： PUB_ALARM.H
* 文件标识：
* 内容摘要：
* 其它说明： 无
* 当前版本： V1.0
* 作    者： 赵剑——3G平台
* 完成日期：
**********************************************************************/
/** @cond OAM_ALARM_INT
  @file PUB_ALARM.H
  @page oam_alarm 平台告警码定义接口规范
  @author  OAM——3G平台

本文档整理的是告警管理的接口、告警数据。\n
特殊说明：\n
1) 平台告警码由32bit的字段定义，分为4个字节，由下图所示。\n
<img src="alarmcode.jpg"> \n												

其中：\n
第1个、第2个字节合起来表示具体的告警码码值；\n
第3个字节的次高比特位（蓝色部分）0表示告警，1表示预警（预留）；\n
第3个字节的最高比特位（红色部分）为0表示物理类型告警，1表示逻辑类型告警；\n
第4个字节平台定义的告警码中必须为0，该字节供事业部使用，为了规范事业部定义，我们在此作说明：\n
第4个字节的最低比特位（黄色部分）为1表示事业部的告警；\n
第4个直接的次低比特位（绿色部分）为1表示事业部的预警（预留）；\n

2)本文仅约定了各个子系统的告警码范围，告警模块及告警码的定义由告警源自己定义，告警码的名称需要遵从以下规则\n
1.告警模块定义为：ALM_A_B \n
  其中：A为子系统的缩写，B为各个模块的缩写，如ALM_SCS_SMM表示SMM模块\n
  #define ALM_SIG_MTP3                            (BYTE)(ALM_SIG_BEGIN + 4)\n
  #define ALM_SCS_MCM                             (BYTE)(ALM_SCS_BEGIN + 0)\n
2.告警码定义：ALARM_A_B_C\n
  其中：A为子系统的缩写，B为各个模块的缩写，C为该告警含义的缩写，如ALARM_SCS_SMM_MPCOMMOFF表示“单板和归属模块之间的控制面通讯异常”的告警\n
  通知码定义：INFORM_A_B_C\n
  其中：A为子系统的缩写，B为各个模块的缩写，C为该告警含义的缩写，如INFORM_OSS_TIMER_LACK表示“定时器缺失”的通知\n
3.告警码定义时需要注意逻辑告警、物理告警;告警、通知的定义规则，具体规则参考告警码的说明，以下为范例\n
  逻辑告警\n
  #define ALM_SIG_MTP3_START         (WORD32)(ALARMCODE_PLAT + (ALM_SIG_MTP3<<8))\n
  #define ALARM_SIG_MTP3_UNACCESS    (WORD32)(ALM_SIG_MTP3_START + 0 + ALARMCODE_COMPARE_LOGICAL)\n
  物理告警\n
  #define ALM_SCS_MCM_START          (WORD32)(ALARMCODE_PLAT + (ALM_SCS_MCM<<8))\n
  #define ALARM_SCS_MCM_GELINKERROR  (WORD32)(ALM_SCS_MCM_START + 1) \n
  告警通知\n
  #define INFORM_PP_START            (WORD32)(ALARMCODE_PLAT + (ALM_PP<<8) + 64)\n
  #define INFORM_PP_CONNECTION_ERROR (WORD32)(INFORM_PP_START + 0) \n
4.相关的告警、通知定义必须增加注释，用于生成CHM和oam扫描\n
5.对于上报告警的单板非故障单板的告警，如板间心跳丢失，所有的C51告警等都需要定义为逻辑告警\n

3)附加信息结构由各个告警源自己确定，需要按照CHM的格式编写，附加信息中关键字的说明如下：\n
1.文件名固定规范，如“子系统名_alarm.h”,目录在各子系统的pub目录，（该文档中的任何修改都涉及到接口，需要有修改记录）\n
2.告警的附加信息oam不再统一定义联合体，而是由各个告警源在上述头文件中自己定义附加信息，需要自行保证各告警的附加信息的长度不要超过ALARM_ADDINFO_UNION_MAX。\n
3.告警或者通知只要有附加信息，都需要对应一个附加信息结构\n
4.告警附加信息结构命名规则如下：\n
  T_ALM_A_B_C:关键字结构名称\n
  其中：A为子系统的缩写，B为各个模块的缩写，C为特定告警附加信息的缩写，\n
  如T_ALM_SIG_MTP3_LINK表示信令MTP3链路的附加信息结构名称\n
5.需要比较的关键字部分需要放在附加信息的最前面，如果涉及到系统对象，则系统对象一定为关键字，且在关键字结构的最前面\n
  example:（此处省略了CHM的格式说明，正式编码时需要补充）\n
    typedef struct \n
    {\n
        WORD16    wLinkNo;--------关键字,一定在结构的最前面;系统对象,一定在关键字的最前面\n
        WORD16    wWhy;\n
        WORD16    wOfficeId;\n
        WORD16    wLinkSetId;\n
        BYTE      ucStatus;\n
        BYTE      ucSlc;\n
    }T_ALM_SIG_MTP3_LINK;\n
6.如果告警的附加信息只有非关键字结构，上报告警时关键字长度必须填写为0\n
7.告警通知中不涉及关键字长度，可以不关心关键字，但因为有些告警和通知会使用到同一结构体，文档只能保证告警的说明\n
8.附加信息结构的定义必须遵循平台编码规则，需要严格遵守CHM文档注释风格，用于生成CHM和oam扫描\n
9.对于上报告警的单板非故障单板的告警，如板间心跳丢失，所有的C51告警等其附加信息中必须将告警单板的地址作为关键字\n
  与业务相关的告警必须将故障单板的逻辑地址作为关键字\n
  与单板物理器件相关的告警必须将故障单板的物理地址作为关键字\n
10.告警源在主备倒换后的新主板上需要重新检测逻辑告警并上报，检测有告警则上报，无告警则不处理。

4)CHM文档中告警码中文名仅作为快速查找告警使用，并不为编码依据，具体中文名请参考EXCEL文档

5)业务的告警定义也需要遵循上述规则，业务使用平台提供扫描工具生成xml脚本文件，该文件在scs定义单独功能版本类型

6)历史告警文件格式、目录路径、文件名称遵从以下规则\n
1.历史告警文件格式：\n
    文件头＋  告警1＋恢复1＋  告警2＋恢复2＋  告警3＋恢复3......\n
       | \n
    4个字节 \n
文件头说明如下：\n
版本号：       1个BYTE      例如：0x02 表示版本号为2\n
字节序：       1个BYTE      0x00表示字节序为小尾，0x01表示字节序为大尾\n
历史告警条数： 2个BYTE      告警＋恢复为1条\n
2.历史告警目录路径：\n
历史告警目录路径：      /IDE0/oam/alarm/\n
历史告警目录挂载路径：  /IDE0/oam/mate/\n
3.历史告警文件名称：\n
historyalarm.log        当前历史告警文件\n
historyalarmbak.log     历史告警bak文件\n

7)V4上报告警和通知的物理位置、逻辑地址信息，建议网管均加以显示，以便区分不同的逻辑实体\n
注：对于有些告警和通知的逻辑地址是全0或全ff的情况，网管不需要显示\n

@endcond
*/
/**   @cond
*/
#ifndef _ALARM_PUB_H_
#define _ALARM_PUB_H_
/***********************************************************
 *                    其它条件编译选项                     *
***********************************************************/
/***********************************************************
 *                   标准、非标准头文件                    *
***********************************************************/
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/utilities.h>

#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/agent_trap.h>
#include <net-snmp/agent/snmp_agent.h>
#include <net-snmp/agent/agent_callbacks.h>

#include <net-snmp/agent/mib_module_config.h>
#include "sky.h"
/* 定义VxWorks中的Packed(1)*/
#ifndef _PACKED_1_
#ifndef  VOS_WINNT
#define _PACKED_1_  __attribute__((packed))
#else
#define _PACKED_1_
#endif
#endif

#if defined(_ARCHITECTURE_)
    #pragma noalign     /* noalign for ic386 */
#elif defined(__BORLANDC__)
    #pragma option -a- /* noalign for Borland C */
#elif defined(_MSC_VER)
    #pragma pack(1)    /* noalign for Microsoft C */
#elif defined(__WATCOMC__)
    #pragma pack(1)    /* noalign for Watcom C */
#elif defined(__DIAB)
    #pragma pack(1)    /* noalign for psosystem C */
#endif

/***********************************************************
 *                        常量定义                         *
***********************************************************/
#define     OAM_IPV4_ADDR_LEN                   4
#define     OAM_IPV6_ADDR_LEN                   16
#define     OAM_MAC_LENGTH                      6
#define     ALARM_ADDINFO_UNION_MAX             256

/* 告警码的产品标识，为dwAlarmCode的最高BYTE */
#define     ALARM_PLAT                          (BYTE)0
#define     ALARMCODE_PLAT                      (WORD32)(ALARM_PLAT<<24)
#define     ALARMCODE_COMPARE_LOGICAL           (WORD32)0x800000
#define     ALARMCODE_COMPARE_PREALM            (WORD32)0x2000000  /* 预警 */

/* 告警码与告警模块掩码&之后，>>OFFSET，可以得到告警模块的值
   目前告警模块仅使用第3个BYTE，以后可能会扩展到第2个BYTE
   考虑到扩展性，在定义告警模块时应使用WORD16 */
#define     ALARM_MASK_MODULE                   (WORD32)0xFF00
#define     ALARM_OFFSET_MODULE                 (BYTE)8

/* 告警码的第2个BYTE中的第1个比特定义为告警位置匹配方式：
   比较物理地址(0)、比较逻辑地址(1)；
   告警码与告警比较方式的掩码&之后，>>OFFSET，可以得到 */
#define     ALARM_MASK_COMPARE                  (WORD32)0x800000
#define     ALARM_MASK_PLAT                     (WORD32)0x1000000
#define     PREALM_MASK_COMPARE_PLAT            (WORD32)0x400000    /* 平台预警 */
#define     PREALM_MASK_COMPARE_NOPLAT          (WORD32)0x2000000   /* 事业部预警 */
#define     ALARM_OFFSET_PREALARM_PLAT          (BYTE)22   /* 平台预警偏移 */
#define     ALARM_OFFSET_COMPARE                (BYTE)23   /* 逻辑告警偏移 */
#define     ALARM_OFFSET_PLAT                   (BYTE)24   /* 事业部告警偏移 */
#define     ALARM_OFFSET_PREALARM_NOPLAT        (BYTE)25   /* 事业部预警偏移 */
#define     ALARM_COMPARE_PHYSICAL              (BYTE)0
#define     ALARM_COMPARE_LOGICAL               (BYTE)1
#define     ALARM_COMPARE_PREALARM              (BYTE)2    /* 为了预警操作链表所加，平台专用 */

/***********************************************************
 *                       全局宏                            *
***********************************************************/
/*---------------------------------------------------
         告警对象定义，目前使用AlarmCode中
         的第3个BYTE，0~255。
--------------------------------------------------*/

/****** TECS ******/
#define ALM_TECS_BEGIN                          (BYTE)180
#define ALM_TECS_FILESTORAGE                    (BYTE)0
#define ALM_TECS_CLUSTER_MANAGER                (BYTE)1 
#define ALM_TECS_HOST_MANAGER                   (BYTE)2
#define ALM_TECS_VNET_MANAGER                   (BYTE)3
#define ALM_TECS_VNET_CONTROLLER                (BYTE)4
#define ALM_TECS_SC_CONTROLLER                  (BYTE)5
#define ALM_TECS_VM_MANAGER                     (BYTE)6


#define ALM_TECS2_BEGIN                          (BYTE)181
#define ALM_TECS3_BEGIN                          (BYTE)182
#define ALM_TECS4_BEGIN                          (BYTE)183
#define ALM_TECS5_BEGIN                          (BYTE)184

#define TECS_ALARM_SYS_COMPUTING                 "computing"
#define TECS_ALARM_SYS_STORAGE                   "storage"
#define TECS_ALARM_SYS_NETWORK                   "network"           
#define TECS_ALARM_SYS_IMAGE                     "image"
/****************************************************************************************
*                                                                                                                                                        *
*                                                                                                                                                        *
*                                                                                                                                                        *
*                                                          TECS 模块告警码                                                          *
*                                                                                                                                                        *
*                                                                                                                                                        *
*                                                                                                                                                        *
*                                                                                                                                                        *
****************************************************************************************/
#define ALM_TECS_FILESTORAGE_START                      (WORD32)(ALARMCODE_PLAT + (ALM_TECS_BEGIN<<8) + ALM_TECS_FILESTORAGE*16)
#define ALM_TECS_FILESTORAGE_NFS_START_FAILED           (WORD32) (ALM_TECS_FILESTORAGE_START+0)    /*告警，NFS 启动失败*/
#define ALM_TECS_FILESTORAGE_TCDB_FAILED                (WORD32) (ALM_TECS_FILESTORAGE_START+1)    /*告警，TCFileStorageDB创建失败*/
#define ALM_TECS_FILESTORAGE_CCDB_FAILED                (WORD32) (ALM_TECS_FILESTORAGE_START+2)    /*告警，CCFileStorageDB创建失败*/
#define ALM_TECS_FILESTORAGE_INSERT_DB_FAILED           (WORD32) (ALM_TECS_FILESTORAGE_START+3)    /*告警通知，插入数据库记录失败*/
#define ALM_TECS_FILESTORAGE_DOWNLOAD_FILE_FAILED       (WORD32) (ALM_TECS_FILESTORAGE_START+4)    /*告警通知，下载文件失败*/
#define ALM_TECS_FILESTORAGE_DELETE_FILE_FAILED         (WORD32) (ALM_TECS_FILESTORAGE_START+5)    /*告警通知，下载文件失败*/
#define ALM_TECS_FILESTORAGE_NFS_MOUNT_FAILED           (WORD32) (ALM_TECS_FILESTORAGE_START+6)    /*告警通知，NFS Mount操作失败*/
#define ALM_TECS_FILESTORAGE_IMAGE_SPACE_HIGH           (WORD32) (ALM_TECS_FILESTORAGE_START+7)    /*告警通知，镜像服务器存储空间使用率高*/

/* 集群管理告警码的定义 */
#define ALM_TECS_CLUSTER_MANAGER_START                (WORD32)(ALARMCODE_PLAT + (ALM_TECS_BEGIN<<8) + ALM_TECS_CLUSTER_MANAGER*16)
#define ALM_TECS_CLUSTER_OFF_LINE                     (WORD32) (ALM_TECS_CLUSTER_MANAGER_START+0)   /* 集群离线告警 */
#define ALM_TECS_CLUSTER_DUPLICATE_NAME               (WORD32) (ALM_TECS_CLUSTER_MANAGER_START+1)   /* 集群重名告警 */
#define ALM_TECS_CLUSTER_INVALID_IP                   (WORD32) (ALM_TECS_CLUSTER_MANAGER_START+2)   /* 集群IP非法告警 */

// 物理机管理模块告警码的定义
#define ALM_TECS_HOST_MANAGER_START     (WORD32)(ALARMCODE_PLAT + (ALM_TECS_BEGIN<<8) + ALM_TECS_HOST_MANAGER*16)
#define ALM_TECS_HOST_OFF_LINE          (WORD32)(ALM_TECS_HOST_MANAGER_START+0)
#define ALM_TECS_HOST_MEMORY_ERR        (WORD32)(ALM_TECS_HOST_MANAGER_START+2)
#define ALM_TECS_HOST_HARDDISK_ERR      (WORD32)(ALM_TECS_HOST_MANAGER_START+3)
#define ALM_TECS_HOST_OSCILLATOR_ERR    (WORD32)(ALM_TECS_HOST_MANAGER_START+4)
#define ALM_TECS_PORT_SPEED_ERR         (WORD32)(ALM_TECS_HOST_MANAGER_START+5)
#define ALM_TECS_PORT_DUPLEX_ERR        (WORD32)(ALM_TECS_HOST_MANAGER_START+6)
#define ALM_TECS_ETHERNET_PORT_DOWN     (WORD32)(ALM_TECS_HOST_MANAGER_START+7)
#define ALM_TECS_TRUNK_PORT_DOWN        (WORD32)(ALM_TECS_HOST_MANAGER_START+8)
#define ALM_TECS_PORT_LINKED_NO         (WORD32)(ALM_TECS_HOST_MANAGER_START+9) // add by 薛明忠
#define ALM_TECS_FREE_TCU_ERR           (WORD32)(ALM_TECS_HOST_MANAGER_START+10) 
#define ALM_TECS_FREE_DISK_ERR          (WORD32)(ALM_TECS_HOST_MANAGER_START+11) 
#define ALM_TECS_FREE_MEMORY_ERR        (WORD32)(ALM_TECS_HOST_MANAGER_START+12) 
#define ALM_TECS_FREE_CPU_ERR           (WORD32)(ALM_TECS_HOST_MANAGER_START+13)
#define ALM_TECS_HOST_HARDDISK_WARN     (WORD32)(ALM_TECS_HOST_MANAGER_START+14)
#define ALM_TECS_ROOTDEVICE_USAGE_ERR   (WORD32)(ALM_TECS_HOST_MANAGER_START+15)
#define ALM_TECS_HOST_IPADDRESS_EMPTY   (WORD32)(ALM_TECS_HOST_MANAGER_START+16)


// VNET管理模块告警码的定义
#define ALM_TECS_VNET_MANAGER_START     (WORD32)(ALARMCODE_PLAT + (ALM_TECS_BEGIN<<8) + ALM_TECS_VNET_MANAGER*16)
#define ALM_TECS_VNET_START_DHCPSRV_FAILED          (WORD32)(ALM_TECS_VNET_MANAGER_START+0)
#define ALM_TECS_VNET_DHCP_NO_FREE_IP           (WORD32)(ALM_TECS_VNET_MANAGER_START+1)

// VNET控制模块告警码的定义
#define ALM_TECS_VNET_CONTROLLER_START  (WORD32)(ALARMCODE_PLAT + (ALM_TECS_BEGIN<<8) + ALM_TECS_VNET_CONTROLLER*16)
#define ALM_TECS_VNET_NETWORK_START     (WORD32)(ALM_TECS_VNET_CONTROLLER_START+0)

// SC控制模块告警码的定义
#define ALM_TECS_STORAGE_MANAGER_START  (WORD32)(ALARMCODE_PLAT + (ALM_TECS_BEGIN<<8) + ALM_TECS_SC_CONTROLLER*16)
#define ALM_TECS_SA_OFF_LINE            (WORD32)(ALM_TECS_STORAGE_MANAGER_START+0)

#define ALM_TECS_VM_MANAGER_START       (WORD32)(ALARMCODE_PLAT + (ALM_TECS_BEGIN<<8) + ALM_TECS_VM_MANAGER*16) //46176
#define ALM_TECS_VM_OFF_LINE            (WORD32)(ALM_TECS_VM_MANAGER_START + 0)
#define ALM_TECS_VM_START_FAILED        (WORD32)(ALM_TECS_VM_MANAGER_START + 1)
#define ALM_TECS_VM_STATE_CONFLICT      (WORD32)(ALM_TECS_VM_MANAGER_START + 2)


/**   @endcond
*/
/**  
    @struct T_AddInfoUnion
    @brief  告警/通知附加信息
    <HR>
    @b 修改记录：
*/
typedef union 
{
    BYTE        aucMaxAddInfo[ALARM_ADDINFO_UNION_MAX];      /* 联合体的最大长度 */
}_PACKED_1_ T_AddInfoUnion;



// 预定义宏，用于从告警附加信息转换为snmp需要的list
#define ALARM_MAP(code, classname)  \
    case code: \
    { \
        classname a; \
        a.deserialize(alarmmsg.aucMaxAddInfo); \
        a.AddToVarList(pOid, alarmmsg.dwAlarmCode); \
        break; \
    }
#define TECS_ALARM_PREFIX 0
// 待snmp功能支持后，此段代码需要删除
#if 0
typedef void netsnmp_variable_list;
typedef long oid;

#define ASN_OCTET_STR	    ((u_char)0x04)
#define ASN_INTEGER	    ((u_char)0x02)
#define ASN_APPLICATION     ((u_char)0x40)
#define ASN_UNSIGNED    (ASN_APPLICATION | 2)   /* RFC 1902 - same as GAUGE */
#define ASN_INTEGER64        (ASN_APPLICATION | 10)
#define ASN_UNSIGNED64       (ASN_APPLICATION | 11)
#define ASN_FLOAT	    (ASN_APPLICATION | 8)
#define ASN_DOUBLE	    (ASN_APPLICATION | 9)
#define ASN_BOOLEAN	    ((u_char)0x01)
#define ASN_TIMETICKS   (ASN_APPLICATION | 3)
#define ASN_IPADDRESS   (ASN_APPLICATION | 0)
extern void snmp_varlist_add_variable(netsnmp_variable_list ** varlist,
                          const oid * name,
                          size_t name_length,
                          u_char type, const u_char * value, size_t len)
                          __attribute__ ((weak));
#endif

#define MAX_OID 20
#define MAX_STRING_LEN 100

typedef struct _oid_struct
{
    oid mem_oid[MAX_OID];
    int oid_len;
    int oid_type;
    u_char value[MAX_STRING_LEN];
    int value_len;
}oid_struct;

#define TO_ALARM \
    void AddToVarList(oid_struct *pOid, unsigned code)

#define TO_ALARM_BEGIN() \
    oid member_oid[] = {1, 3, 6, 1, 4, 1, 3902, 6051, 19, 1, 2, 1, code, 0}; \
    int i = 0

#define TO_ALARM_END()
               
#define TO_ALARM_STRING(member) \
    member_oid[sizeof(member_oid)/sizeof(oid) - 1] = ++i; \
    memcpy(pOid->mem_oid,member_oid,sizeof(member_oid)); \
    pOid->oid_len = sizeof(member_oid)/sizeof(oid); \
    pOid->oid_type = ASN_OCTET_STR; \
    memcpy(pOid->value,(u_char*)member.c_str(),member.size()); \
    pOid->value_len = member.size()
    
#define TO_ALARM_INT32(member) \
    member_oid[sizeof(member_oid)/sizeof(oid) - 1] = ++i; \
    memcpy(pOid->mem_oid,member_oid,sizeof(member_oid)); \
    pOid->oid_len = sizeof(member_oid)/sizeof(oid); \
    pOid->oid_type = ASN_INTEGER; \
    memcpy(pOid->value,(u_char*)&member,sizeof(member)); \
    pOid->value_len = sizeof(member)

#define TO_ALARM_UINT32(member) \
    member_oid[sizeof(member_oid)/sizeof(oid) - 1] = ++i; \
    memcpy(pOid->mem_oid,member_oid,sizeof(member_oid)); \
    pOid->oid_len = sizeof(member_oid)/sizeof(oid); \
    pOid->oid_type = ASN_UNSIGNED; \
    memcpy(pOid->value,(u_char*)&member,sizeof(member));\
    pOid->value_len = sizeof(member)
    

#define TO_ALARM_INT64(member) \
    member_oid[sizeof(member_oid)/sizeof(oid) - 1] = ++i; \
    memcpy(pOid->mem_oid,member_oid,sizeof(member_oid)); \
    pOid->oid_len = sizeof(member_oid)/sizeof(oid); \
    pOid->oid_type = ASN_INTEGER64; \
    memcpy(pOid->value,(u_char*)&member,sizeof(member));\
    pOid->value_len = sizeof(member)

#define TO_ALARM_UINT64(member) \
    member_oid[sizeof(member_oid)/sizeof(oid) - 1] = ++i; \
    memcpy(pOid->mem_oid,member_oid,sizeof(member_oid)); \
    pOid->oid_len = sizeof(member_oid)/sizeof(oid); \
    pOid->oid_type = ASN_UNSIGNED64; \
    memcpy(pOid->value,(u_char*)&member,sizeof(member));\
    pOid->value_len = sizeof(member)

#define TO_ALARM_FLOAT(member) \
    member_oid[sizeof(member_oid)/sizeof(oid) - 1] = ++i; \
    memcpy(pOid->mem_oid,member_oid,sizeof(member_oid)); \
    pOid->oid_len = sizeof(member_oid)/sizeof(oid); \
    pOid->oid_type = ASN_FLOAT; \
    memcpy(pOid->value,(u_char*)&member,sizeof(member));\
    pOid->value_len = sizeof(member)

#define TO_ALARM_DOUBLE(member) \
    member_oid[sizeof(member_oid)/sizeof(oid) - 1] = ++i; \
    memcpy(pOid->mem_oid,member_oid,sizeof(member_oid)); \
    pOid->oid_len = sizeof(member_oid)/sizeof(oid); \
    pOid->oid_type = ASN_DOUBLE; \
    memcpy(pOid->value,(u_char*)&member,sizeof(member));\
    pOid->value_len = sizeof(member)


#define TO_ALARM_BOOL(member) \
    member_oid[sizeof(member_oid)/sizeof(oid) - 1] = ++i; \
    memcpy(pOid->mem_oid,member_oid,sizeof(member_oid)); \
    pOid->oid_len = sizeof(member_oid)/sizeof(oid); \
    pOid->oid_type = ASN_BOOLEAN; \
    memcpy(pOid->value,(u_char*)&member,sizeof(member));\
    pOid->value_len = sizeof(member)

#define TO_ALARM_IPADDRESS(member) \
    member_oid[sizeof(member_oid)/sizeof(oid) - 1] = ++i; \
    memcpy(pOid->mem_oid,member_oid,sizeof(member_oid)); \
    pOid->oid_len = sizeof(member_oid)/sizeof(oid); \
    pOid->oid_type = ASN_IPADDRESS; \
    memcpy(pOid->value,(u_char*)&member,sizeof(member));\
    pOid->value_len = sizeof(member)

#define TO_ALARM_TIME(member) \
    member_oid[sizeof(member_oid)/sizeof(oid) - 1] = ++i; \
    memcpy(pOid->mem_oid,member_oid,sizeof(member_oid)); \
    pOid->oid_len = sizeof(member_oid)/sizeof(oid); \
    pOid->oid_type = ASN_TIMETICKS; \
    memcpy(pOid->value,(u_char*)&member,sizeof(member));\
    pOid->value_len = sizeof(member)


class Alarmable
{
public:
    virtual ~Alarmable() {}
    virtual void AddToVarList(oid_struct *pOid, unsigned code)
        = 0;
};

// 各告警码对应的附加信息类定义如下
class AlarmPortLinkedNo: public Serializable, Alarmable
{
public:
    AlarmPortLinkedNo() {}
    ~AlarmPortLinkedNo() {}
    AlarmPortLinkedNo(const string &port_name) { _port_name = port_name; }

    string _port_name;

    SERIALIZE
    {
        SERIALIZE_BEGIN(AlarmPortDown);
        WRITE_VALUE(_port_name);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(AlarmPortDown);
        READ_VALUE(_port_name);
        DESERIALIZE_END();
    };

    TO_ALARM
    {
        TO_ALARM_BEGIN();
        TO_ALARM_STRING(_port_name);
        TO_ALARM_END();
    };
};
class AlarmPortDown: public Serializable, Alarmable
{
public:
    AlarmPortDown() {}
    ~AlarmPortDown() {}    
    AlarmPortDown(const string &port_name) { _port_name = port_name; }
    
    string _port_name;

    SERIALIZE
    {
        SERIALIZE_BEGIN(AlarmPortDown);
        WRITE_VALUE(_port_name);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(AlarmPortDown);
        READ_VALUE(_port_name);
        DESERIALIZE_END();
    };

    TO_ALARM
    {
        TO_ALARM_BEGIN();
        TO_ALARM_STRING(_port_name);
        TO_ALARM_END();
    };
};

class AlarmPortSpeed: public Serializable, Alarmable
{
public:
    AlarmPortSpeed() {}
    ~AlarmPortSpeed() {}    
    AlarmPortSpeed(const string &port_name, 
                   const int32 config_speed, 
                   const int32 work_speed)
    {
        _port_name = port_name; 
        _config_speed = config_speed; 
        _work_speed = work_speed;
    }
    bool operator == (const AlarmPortSpeed &r)
        {
            return  (_port_name == r._port_name)
                    && (_config_speed == r._config_speed)
                    && (_work_speed == r._work_speed);
        }

    string _port_name;
    int32  _config_speed;
    int32  _work_speed;

    SERIALIZE
    {
        SERIALIZE_BEGIN(AlarmPortSpeed);
        WRITE_VALUE(_port_name);
        WRITE_VALUE(_config_speed);
        WRITE_VALUE(_work_speed);        
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(AlarmPortSpeed);
        READ_VALUE(_port_name);
        READ_VALUE(_config_speed);
        READ_VALUE(_work_speed);
        DESERIALIZE_END();
    };

    TO_ALARM
    {
        TO_ALARM_BEGIN();
        TO_ALARM_STRING(_port_name);
        TO_ALARM_INT32(_config_speed);
        TO_ALARM_INT32(_work_speed);        
        TO_ALARM_END();
    };
};

class AlarmPortDuplex: public Serializable, Alarmable
{
public:
    AlarmPortDuplex() {}
    ~AlarmPortDuplex() {}    
    AlarmPortDuplex(const string &port_name, 
                    const int32 config_duplex, 
                    const int32 work_duplex)
    {
        _port_name = port_name; 
        _config_duplex = config_duplex; 
        _work_duplex = work_duplex;
    }
    bool operator == (const AlarmPortDuplex & r)
    {
        return (_port_name == r._port_name)
                &&(_config_duplex == r._config_duplex)
                &&(_work_duplex == r._work_duplex);
    }

    string _port_name;
    int32  _config_duplex;
    int32  _work_duplex;

    SERIALIZE
    {
        SERIALIZE_BEGIN(AlarmPortDuplex);
        WRITE_VALUE(_port_name);
        WRITE_VALUE(_config_duplex);
        WRITE_VALUE(_work_duplex);        
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(AlarmPortDuplex);
        READ_VALUE(_port_name);
        READ_VALUE(_config_duplex);
        READ_VALUE(_work_duplex);
        DESERIALIZE_END();
    };

    TO_ALARM
    {
        TO_ALARM_BEGIN();
        TO_ALARM_STRING(_port_name);
        TO_ALARM_INT32(_config_duplex);
        TO_ALARM_INT32(_work_duplex);
        TO_ALARM_END();
    };
};

class AlarmTrunkDown: public Serializable, Alarmable
{
public:
    AlarmTrunkDown() {}
    ~AlarmTrunkDown() {}    
    AlarmTrunkDown(const string &trunk_name) { _trunk_name = trunk_name; }
    
    string _trunk_name;

    SERIALIZE
    {
        SERIALIZE_BEGIN(AlarmTrunkDown);
        WRITE_VALUE(_trunk_name);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(AlarmTrunkDown);
        READ_VALUE(_trunk_name);
        DESERIALIZE_END();
    };

    TO_ALARM
    {
        TO_ALARM_BEGIN();
        TO_ALARM_STRING(_trunk_name);
        TO_ALARM_END();
    };
};

class AlarmDiskErr: public Serializable, Alarmable
{
public:
    AlarmDiskErr() {}
    ~AlarmDiskErr() {}    
    AlarmDiskErr(const string &disk_name) { _disk_name = disk_name; }
    
    string _disk_name;

    SERIALIZE
    {
        SERIALIZE_BEGIN(AlarmDiskErr);
        WRITE_VALUE(_disk_name);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(AlarmDiskErr);
        READ_VALUE(_disk_name);
        DESERIALIZE_END();
    };

    TO_ALARM
    {
        TO_ALARM_BEGIN();
        TO_ALARM_STRING(_disk_name);
        TO_ALARM_END();
    };
};

class AlarmRootDeviceUsageErr: public Serializable, Alarmable
{
public:
    AlarmRootDeviceUsageErr() {}
    ~AlarmRootDeviceUsageErr() {}    
    AlarmRootDeviceUsageErr(const string &info) { _info = info; }
    
    string _info;

    SERIALIZE
    {
        SERIALIZE_BEGIN(AlarmRootDeviceUsageErr);
        WRITE_VALUE(_info);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(AlarmRootDeviceUsageErr);
        READ_VALUE(_info);
        DESERIALIZE_END();
    };

    TO_ALARM
    {
        TO_ALARM_BEGIN();
        TO_ALARM_STRING(_info);
        TO_ALARM_END();
    };
};


class AlarmVNetStartDhcpFailed: public Serializable, Alarmable
{
public:
    AlarmVNetStartDhcpFailed() {}
    ~AlarmVNetStartDhcpFailed() {}    
    AlarmVNetStartDhcpFailed(const string &info) 
        { _info = info; }
    
    string _info;

    SERIALIZE
    {
        SERIALIZE_BEGIN(AlarmVNetStartDhcpFailed);
        WRITE_VALUE(_info);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(AlarmVNetStartDhcpFailed);
        READ_VALUE(_info);
        DESERIALIZE_END();
    };

    TO_ALARM
    {
        TO_ALARM_BEGIN();
        TO_ALARM_STRING(_info);
        TO_ALARM_END();
    };
};

class AlarmVNetDhcpNoIP: public Serializable, Alarmable
{
public:
    AlarmVNetDhcpNoIP() {}
    ~AlarmVNetDhcpNoIP() {}    
    AlarmVNetDhcpNoIP(const string &info) 
        { _info = info; }
    
    string _info;

    SERIALIZE
    {
        SERIALIZE_BEGIN(AlarmVNetDhcpNoIP);
        WRITE_VALUE(_info);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(AlarmVNetDhcpNoIP);
        READ_VALUE(_info);
        DESERIALIZE_END();
    };

    TO_ALARM
    {
        TO_ALARM_BEGIN();
        TO_ALARM_STRING(_info);
        TO_ALARM_END();
    };
};

class AlarmVNetStartNetwork: public Serializable, Alarmable
{
public:
    AlarmVNetStartNetwork() {}
    ~AlarmVNetStartNetwork() {}    
    AlarmVNetStartNetwork(const string &info) 
        { _info = info; }
    
    string _info;

    SERIALIZE
    {
        SERIALIZE_BEGIN(AlarmVNetStartNetwork);
        WRITE_VALUE(_info);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(AlarmVNetStartNetwork);
        READ_VALUE(_info);
        DESERIALIZE_END();
    };

    TO_ALARM
    {
        TO_ALARM_BEGIN();
        TO_ALARM_STRING(_info);
        TO_ALARM_END();
    };
};

class AlarmVm: public Serializable, Alarmable
{
public:
    AlarmVm() {}
    AlarmVm(int64 vid, int64 hid)
    {
        _vid = vid;
        _hid = hid;
    }
    ~AlarmVm() {}    

    SERIALIZE
    {
        SERIALIZE_BEGIN(AlarmVm);
        WRITE_VALUE(_vid);
        WRITE_VALUE(_hid);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(AlarmVm);
        READ_VALUE(_vid);
        READ_VALUE(_hid);
        DESERIALIZE_END();
    };

    TO_ALARM
    {
        TO_ALARM_BEGIN();
        TO_ALARM_INT64(_vid);
        TO_ALARM_INT64(_hid);
        TO_ALARM_END();
    };

    int64 _vid;
    int64 _hid;
};

// 设置告警码与附加信息类的对应关系
#define All_ALARM_MAP \
    ALARM_MAP(ALM_TECS_HOST_HARDDISK_ERR,   AlarmDiskErr) \
    ALARM_MAP(ALM_TECS_PORT_SPEED_ERR,      AlarmPortSpeed) \
    ALARM_MAP(ALM_TECS_PORT_DUPLEX_ERR,     AlarmPortDuplex) \
    ALARM_MAP(ALM_TECS_ETHERNET_PORT_DOWN,  AlarmPortDown) \
    ALARM_MAP(ALM_TECS_TRUNK_PORT_DOWN,     AlarmTrunkDown)\
    ALARM_MAP(ALM_TECS_VNET_START_DHCPSRV_FAILED,   AlarmVNetStartDhcpFailed)\
    ALARM_MAP(ALM_TECS_VNET_DHCP_NO_FREE_IP,        AlarmVNetDhcpNoIP)\
    ALARM_MAP(ALM_TECS_VNET_NETWORK_START,          AlarmVNetStartNetwork)\
    ALARM_MAP(ALM_TECS_VM_OFF_LINE,                 AlarmVm)\
    ALARM_MAP(ALM_TECS_VM_START_FAILED,             AlarmVm)\
    ALARM_MAP(ALM_TECS_VM_STATE_CONFLICT,           AlarmVm)\


#if defined(_ARCHITECTURE_)
    #pragma align      /* align for ic386 */
#elif defined(__BORLANDC__)
    #pragma option -a  /* align for Borland C */
#elif defined(_MSC_VER)
    #pragma pack()     /* align for Microsoft C */
#elif defined(__WATCOMC__)
    #pragma pack()     /* align for Watcom C */
#elif defined(__DIAB)
    #pragma pack()     /* align for psosystem C */
#endif

#endif
