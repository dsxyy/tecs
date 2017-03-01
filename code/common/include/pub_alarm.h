/*********************************************************************
* ��Ȩ���� (C)2008, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ� PUB_ALARM.H
* �ļ���ʶ��
* ����ժҪ��
* ����˵���� ��
* ��ǰ�汾�� V1.0
* ��    �ߣ� �Խ�����3Gƽ̨
* ������ڣ�
**********************************************************************/
/** @cond OAM_ALARM_INT
  @file PUB_ALARM.H
  @page oam_alarm ƽ̨�澯�붨��ӿڹ淶
  @author  OAM����3Gƽ̨

���ĵ�������Ǹ澯����Ľӿڡ��澯���ݡ�\n
����˵����\n
1) ƽ̨�澯����32bit���ֶζ��壬��Ϊ4���ֽڣ�����ͼ��ʾ��\n
<img src="alarmcode.jpg"> \n												

���У�\n
��1������2���ֽں�������ʾ����ĸ澯����ֵ��\n
��3���ֽڵĴθ߱���λ����ɫ���֣�0��ʾ�澯��1��ʾԤ����Ԥ������\n
��3���ֽڵ���߱���λ����ɫ���֣�Ϊ0��ʾ�������͸澯��1��ʾ�߼����͸澯��\n
��4���ֽ�ƽ̨����ĸ澯���б���Ϊ0�����ֽڹ���ҵ��ʹ�ã�Ϊ�˹淶��ҵ�����壬�����ڴ���˵����\n
��4���ֽڵ���ͱ���λ����ɫ���֣�Ϊ1��ʾ��ҵ���ĸ澯��\n
��4��ֱ�ӵĴεͱ���λ����ɫ���֣�Ϊ1��ʾ��ҵ����Ԥ����Ԥ������\n

2)���Ľ�Լ���˸�����ϵͳ�ĸ澯�뷶Χ���澯ģ�鼰�澯��Ķ����ɸ澯Դ�Լ����壬�澯���������Ҫ������¹���\n
1.�澯ģ�鶨��Ϊ��ALM_A_B \n
  ���У�AΪ��ϵͳ����д��BΪ����ģ�����д����ALM_SCS_SMM��ʾSMMģ��\n
  #define ALM_SIG_MTP3                            (BYTE)(ALM_SIG_BEGIN + 4)\n
  #define ALM_SCS_MCM                             (BYTE)(ALM_SCS_BEGIN + 0)\n
2.�澯�붨�壺ALARM_A_B_C\n
  ���У�AΪ��ϵͳ����д��BΪ����ģ�����д��CΪ�ø澯�������д����ALARM_SCS_SMM_MPCOMMOFF��ʾ������͹���ģ��֮��Ŀ�����ͨѶ�쳣���ĸ澯\n
  ֪ͨ�붨�壺INFORM_A_B_C\n
  ���У�AΪ��ϵͳ����д��BΪ����ģ�����д��CΪ�ø澯�������д����INFORM_OSS_TIMER_LACK��ʾ����ʱ��ȱʧ����֪ͨ\n
3.�澯�붨��ʱ��Ҫע���߼��澯������澯;�澯��֪ͨ�Ķ�����򣬾������ο��澯���˵��������Ϊ����\n
  �߼��澯\n
  #define ALM_SIG_MTP3_START         (WORD32)(ALARMCODE_PLAT + (ALM_SIG_MTP3<<8))\n
  #define ALARM_SIG_MTP3_UNACCESS    (WORD32)(ALM_SIG_MTP3_START + 0 + ALARMCODE_COMPARE_LOGICAL)\n
  ����澯\n
  #define ALM_SCS_MCM_START          (WORD32)(ALARMCODE_PLAT + (ALM_SCS_MCM<<8))\n
  #define ALARM_SCS_MCM_GELINKERROR  (WORD32)(ALM_SCS_MCM_START + 1) \n
  �澯֪ͨ\n
  #define INFORM_PP_START            (WORD32)(ALARMCODE_PLAT + (ALM_PP<<8) + 64)\n
  #define INFORM_PP_CONNECTION_ERROR (WORD32)(INFORM_PP_START + 0) \n
4.��صĸ澯��֪ͨ�����������ע�ͣ���������CHM��oamɨ��\n
5.�����ϱ��澯�ĵ���ǹ��ϵ���ĸ澯������������ʧ�����е�C51�澯�ȶ���Ҫ����Ϊ�߼��澯\n

3)������Ϣ�ṹ�ɸ����澯Դ�Լ�ȷ������Ҫ����CHM�ĸ�ʽ��д��������Ϣ�йؼ��ֵ�˵�����£�\n
1.�ļ����̶��淶���硰��ϵͳ��_alarm.h��,Ŀ¼�ڸ���ϵͳ��pubĿ¼�������ĵ��е��κ��޸Ķ��漰���ӿڣ���Ҫ���޸ļ�¼��\n
2.�澯�ĸ�����Ϣoam����ͳһ���������壬�����ɸ����澯Դ������ͷ�ļ����Լ����帽����Ϣ����Ҫ���б�֤���澯�ĸ�����Ϣ�ĳ��Ȳ�Ҫ����ALARM_ADDINFO_UNION_MAX��\n
3.�澯����ֻ֪ͨҪ�и�����Ϣ������Ҫ��Ӧһ��������Ϣ�ṹ\n
4.�澯������Ϣ�ṹ�����������£�\n
  T_ALM_A_B_C:�ؼ��ֽṹ����\n
  ���У�AΪ��ϵͳ����д��BΪ����ģ�����д��CΪ�ض��澯������Ϣ����д��\n
  ��T_ALM_SIG_MTP3_LINK��ʾ����MTP3��·�ĸ�����Ϣ�ṹ����\n
5.��Ҫ�ȽϵĹؼ��ֲ�����Ҫ���ڸ�����Ϣ����ǰ�棬����漰��ϵͳ������ϵͳ����һ��Ϊ�ؼ��֣����ڹؼ��ֽṹ����ǰ��\n
  example:���˴�ʡ����CHM�ĸ�ʽ˵������ʽ����ʱ��Ҫ���䣩\n
    typedef struct \n
    {\n
        WORD16    wLinkNo;--------�ؼ���,һ���ڽṹ����ǰ��;ϵͳ����,һ���ڹؼ��ֵ���ǰ��\n
        WORD16    wWhy;\n
        WORD16    wOfficeId;\n
        WORD16    wLinkSetId;\n
        BYTE      ucStatus;\n
        BYTE      ucSlc;\n
    }T_ALM_SIG_MTP3_LINK;\n
6.����澯�ĸ�����Ϣֻ�зǹؼ��ֽṹ���ϱ��澯ʱ�ؼ��ֳ��ȱ�����дΪ0\n
7.�澯֪ͨ�в��漰�ؼ��ֳ��ȣ����Բ����Ĺؼ��֣�����Ϊ��Щ�澯��֪ͨ��ʹ�õ�ͬһ�ṹ�壬�ĵ�ֻ�ܱ�֤�澯��˵��\n
8.������Ϣ�ṹ�Ķ��������ѭƽ̨���������Ҫ�ϸ�����CHM�ĵ�ע�ͷ����������CHM��oamɨ��\n
9.�����ϱ��澯�ĵ���ǹ��ϵ���ĸ澯������������ʧ�����е�C51�澯���丽����Ϣ�б��뽫�澯����ĵ�ַ��Ϊ�ؼ���\n
  ��ҵ����صĸ澯���뽫���ϵ�����߼���ַ��Ϊ�ؼ���\n
  �뵥������������صĸ澯���뽫���ϵ���������ַ��Ϊ�ؼ���\n
10.�澯Դ�����������������������Ҫ���¼���߼��澯���ϱ�������и澯���ϱ����޸澯�򲻴���

4)CHM�ĵ��и澯������������Ϊ���ٲ��Ҹ澯ʹ�ã�����Ϊ�������ݣ�������������ο�EXCEL�ĵ�

5)ҵ��ĸ澯����Ҳ��Ҫ��ѭ��������ҵ��ʹ��ƽ̨�ṩɨ�蹤������xml�ű��ļ������ļ���scs���嵥�����ܰ汾����

6)��ʷ�澯�ļ���ʽ��Ŀ¼·�����ļ�����������¹���\n
1.��ʷ�澯�ļ���ʽ��\n
    �ļ�ͷ��  �澯1���ָ�1��  �澯2���ָ�2��  �澯3���ָ�3......\n
       | \n
    4���ֽ� \n
�ļ�ͷ˵�����£�\n
�汾�ţ�       1��BYTE      ���磺0x02 ��ʾ�汾��Ϊ2\n
�ֽ���       1��BYTE      0x00��ʾ�ֽ���ΪСβ��0x01��ʾ�ֽ���Ϊ��β\n
��ʷ�澯������ 2��BYTE      �澯���ָ�Ϊ1��\n
2.��ʷ�澯Ŀ¼·����\n
��ʷ�澯Ŀ¼·����      /IDE0/oam/alarm/\n
��ʷ�澯Ŀ¼����·����  /IDE0/oam/mate/\n
3.��ʷ�澯�ļ����ƣ�\n
historyalarm.log        ��ǰ��ʷ�澯�ļ�\n
historyalarmbak.log     ��ʷ�澯bak�ļ�\n

7)V4�ϱ��澯��֪ͨ������λ�á��߼���ַ��Ϣ���������ܾ�������ʾ���Ա����ֲ�ͬ���߼�ʵ��\n
ע��������Щ�澯��֪ͨ���߼���ַ��ȫ0��ȫff����������ܲ���Ҫ��ʾ\n

@endcond
*/
/**   @cond
*/
#ifndef _ALARM_PUB_H_
#define _ALARM_PUB_H_
/***********************************************************
 *                    ������������ѡ��                     *
***********************************************************/
/***********************************************************
 *                   ��׼���Ǳ�׼ͷ�ļ�                    *
***********************************************************/
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/utilities.h>

#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/agent_trap.h>
#include <net-snmp/agent/snmp_agent.h>
#include <net-snmp/agent/agent_callbacks.h>

#include <net-snmp/agent/mib_module_config.h>
#include "sky.h"
/* ����VxWorks�е�Packed(1)*/
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
 *                        ��������                         *
***********************************************************/
#define     OAM_IPV4_ADDR_LEN                   4
#define     OAM_IPV6_ADDR_LEN                   16
#define     OAM_MAC_LENGTH                      6
#define     ALARM_ADDINFO_UNION_MAX             256

/* �澯��Ĳ�Ʒ��ʶ��ΪdwAlarmCode�����BYTE */
#define     ALARM_PLAT                          (BYTE)0
#define     ALARMCODE_PLAT                      (WORD32)(ALARM_PLAT<<24)
#define     ALARMCODE_COMPARE_LOGICAL           (WORD32)0x800000
#define     ALARMCODE_COMPARE_PREALM            (WORD32)0x2000000  /* Ԥ�� */

/* �澯����澯ģ������&֮��>>OFFSET�����Եõ��澯ģ���ֵ
   Ŀǰ�澯ģ���ʹ�õ�3��BYTE���Ժ���ܻ���չ����2��BYTE
   ���ǵ���չ�ԣ��ڶ���澯ģ��ʱӦʹ��WORD16 */
#define     ALARM_MASK_MODULE                   (WORD32)0xFF00
#define     ALARM_OFFSET_MODULE                 (BYTE)8

/* �澯��ĵ�2��BYTE�еĵ�1�����ض���Ϊ�澯λ��ƥ�䷽ʽ��
   �Ƚ������ַ(0)���Ƚ��߼���ַ(1)��
   �澯����澯�ȽϷ�ʽ������&֮��>>OFFSET�����Եõ� */
#define     ALARM_MASK_COMPARE                  (WORD32)0x800000
#define     ALARM_MASK_PLAT                     (WORD32)0x1000000
#define     PREALM_MASK_COMPARE_PLAT            (WORD32)0x400000    /* ƽ̨Ԥ�� */
#define     PREALM_MASK_COMPARE_NOPLAT          (WORD32)0x2000000   /* ��ҵ��Ԥ�� */
#define     ALARM_OFFSET_PREALARM_PLAT          (BYTE)22   /* ƽ̨Ԥ��ƫ�� */
#define     ALARM_OFFSET_COMPARE                (BYTE)23   /* �߼��澯ƫ�� */
#define     ALARM_OFFSET_PLAT                   (BYTE)24   /* ��ҵ���澯ƫ�� */
#define     ALARM_OFFSET_PREALARM_NOPLAT        (BYTE)25   /* ��ҵ��Ԥ��ƫ�� */
#define     ALARM_COMPARE_PHYSICAL              (BYTE)0
#define     ALARM_COMPARE_LOGICAL               (BYTE)1
#define     ALARM_COMPARE_PREALARM              (BYTE)2    /* Ϊ��Ԥ�������������ӣ�ƽ̨ר�� */

/***********************************************************
 *                       ȫ�ֺ�                            *
***********************************************************/
/*---------------------------------------------------
         �澯�����壬Ŀǰʹ��AlarmCode��
         �ĵ�3��BYTE��0~255��
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
*                                                          TECS ģ��澯��                                                          *
*                                                                                                                                                        *
*                                                                                                                                                        *
*                                                                                                                                                        *
*                                                                                                                                                        *
****************************************************************************************/
#define ALM_TECS_FILESTORAGE_START                      (WORD32)(ALARMCODE_PLAT + (ALM_TECS_BEGIN<<8) + ALM_TECS_FILESTORAGE*16)
#define ALM_TECS_FILESTORAGE_NFS_START_FAILED           (WORD32) (ALM_TECS_FILESTORAGE_START+0)    /*�澯��NFS ����ʧ��*/
#define ALM_TECS_FILESTORAGE_TCDB_FAILED                (WORD32) (ALM_TECS_FILESTORAGE_START+1)    /*�澯��TCFileStorageDB����ʧ��*/
#define ALM_TECS_FILESTORAGE_CCDB_FAILED                (WORD32) (ALM_TECS_FILESTORAGE_START+2)    /*�澯��CCFileStorageDB����ʧ��*/
#define ALM_TECS_FILESTORAGE_INSERT_DB_FAILED           (WORD32) (ALM_TECS_FILESTORAGE_START+3)    /*�澯֪ͨ���������ݿ��¼ʧ��*/
#define ALM_TECS_FILESTORAGE_DOWNLOAD_FILE_FAILED       (WORD32) (ALM_TECS_FILESTORAGE_START+4)    /*�澯֪ͨ�������ļ�ʧ��*/
#define ALM_TECS_FILESTORAGE_DELETE_FILE_FAILED         (WORD32) (ALM_TECS_FILESTORAGE_START+5)    /*�澯֪ͨ�������ļ�ʧ��*/
#define ALM_TECS_FILESTORAGE_NFS_MOUNT_FAILED           (WORD32) (ALM_TECS_FILESTORAGE_START+6)    /*�澯֪ͨ��NFS Mount����ʧ��*/
#define ALM_TECS_FILESTORAGE_IMAGE_SPACE_HIGH           (WORD32) (ALM_TECS_FILESTORAGE_START+7)    /*�澯֪ͨ������������洢�ռ�ʹ���ʸ�*/

/* ��Ⱥ����澯��Ķ��� */
#define ALM_TECS_CLUSTER_MANAGER_START                (WORD32)(ALARMCODE_PLAT + (ALM_TECS_BEGIN<<8) + ALM_TECS_CLUSTER_MANAGER*16)
#define ALM_TECS_CLUSTER_OFF_LINE                     (WORD32) (ALM_TECS_CLUSTER_MANAGER_START+0)   /* ��Ⱥ���߸澯 */
#define ALM_TECS_CLUSTER_DUPLICATE_NAME               (WORD32) (ALM_TECS_CLUSTER_MANAGER_START+1)   /* ��Ⱥ�����澯 */
#define ALM_TECS_CLUSTER_INVALID_IP                   (WORD32) (ALM_TECS_CLUSTER_MANAGER_START+2)   /* ��ȺIP�Ƿ��澯 */

// ���������ģ��澯��Ķ���
#define ALM_TECS_HOST_MANAGER_START     (WORD32)(ALARMCODE_PLAT + (ALM_TECS_BEGIN<<8) + ALM_TECS_HOST_MANAGER*16)
#define ALM_TECS_HOST_OFF_LINE          (WORD32)(ALM_TECS_HOST_MANAGER_START+0)
#define ALM_TECS_HOST_MEMORY_ERR        (WORD32)(ALM_TECS_HOST_MANAGER_START+2)
#define ALM_TECS_HOST_HARDDISK_ERR      (WORD32)(ALM_TECS_HOST_MANAGER_START+3)
#define ALM_TECS_HOST_OSCILLATOR_ERR    (WORD32)(ALM_TECS_HOST_MANAGER_START+4)
#define ALM_TECS_PORT_SPEED_ERR         (WORD32)(ALM_TECS_HOST_MANAGER_START+5)
#define ALM_TECS_PORT_DUPLEX_ERR        (WORD32)(ALM_TECS_HOST_MANAGER_START+6)
#define ALM_TECS_ETHERNET_PORT_DOWN     (WORD32)(ALM_TECS_HOST_MANAGER_START+7)
#define ALM_TECS_TRUNK_PORT_DOWN        (WORD32)(ALM_TECS_HOST_MANAGER_START+8)
#define ALM_TECS_PORT_LINKED_NO         (WORD32)(ALM_TECS_HOST_MANAGER_START+9) // add by Ѧ����
#define ALM_TECS_FREE_TCU_ERR           (WORD32)(ALM_TECS_HOST_MANAGER_START+10) 
#define ALM_TECS_FREE_DISK_ERR          (WORD32)(ALM_TECS_HOST_MANAGER_START+11) 
#define ALM_TECS_FREE_MEMORY_ERR        (WORD32)(ALM_TECS_HOST_MANAGER_START+12) 
#define ALM_TECS_FREE_CPU_ERR           (WORD32)(ALM_TECS_HOST_MANAGER_START+13)
#define ALM_TECS_HOST_HARDDISK_WARN     (WORD32)(ALM_TECS_HOST_MANAGER_START+14)
#define ALM_TECS_ROOTDEVICE_USAGE_ERR   (WORD32)(ALM_TECS_HOST_MANAGER_START+15)
#define ALM_TECS_HOST_IPADDRESS_EMPTY   (WORD32)(ALM_TECS_HOST_MANAGER_START+16)


// VNET����ģ��澯��Ķ���
#define ALM_TECS_VNET_MANAGER_START     (WORD32)(ALARMCODE_PLAT + (ALM_TECS_BEGIN<<8) + ALM_TECS_VNET_MANAGER*16)
#define ALM_TECS_VNET_START_DHCPSRV_FAILED          (WORD32)(ALM_TECS_VNET_MANAGER_START+0)
#define ALM_TECS_VNET_DHCP_NO_FREE_IP           (WORD32)(ALM_TECS_VNET_MANAGER_START+1)

// VNET����ģ��澯��Ķ���
#define ALM_TECS_VNET_CONTROLLER_START  (WORD32)(ALARMCODE_PLAT + (ALM_TECS_BEGIN<<8) + ALM_TECS_VNET_CONTROLLER*16)
#define ALM_TECS_VNET_NETWORK_START     (WORD32)(ALM_TECS_VNET_CONTROLLER_START+0)

// SC����ģ��澯��Ķ���
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
    @brief  �澯/֪ͨ������Ϣ
    <HR>
    @b �޸ļ�¼��
*/
typedef union 
{
    BYTE        aucMaxAddInfo[ALARM_ADDINFO_UNION_MAX];      /* ���������󳤶� */
}_PACKED_1_ T_AddInfoUnion;



// Ԥ����꣬���ڴӸ澯������Ϣת��Ϊsnmp��Ҫ��list
#define ALARM_MAP(code, classname)  \
    case code: \
    { \
        classname a; \
        a.deserialize(alarmmsg.aucMaxAddInfo); \
        a.AddToVarList(pOid, alarmmsg.dwAlarmCode); \
        break; \
    }
#define TECS_ALARM_PREFIX 0
// ��snmp����֧�ֺ󣬴˶δ�����Ҫɾ��
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

// ���澯���Ӧ�ĸ�����Ϣ�ඨ������
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

// ���ø澯���븽����Ϣ��Ķ�Ӧ��ϵ
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
