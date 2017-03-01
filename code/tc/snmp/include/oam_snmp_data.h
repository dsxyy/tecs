#ifndef _OAM_SNMP_DATA_H_
#define _OAM_SNMP_DATA_H_

#include "oam_pub.h"
#include "pub_oam_lm_event.h"
#ifndef WIN32
    #ifndef _PACKED_1_
        #define _PACKED_1_ __attribute__ ((packed)) 
    #endif
#else
    #ifndef _PACKED_1_
#define _PACKED_1_
    #endif
#endif

#ifdef WIN32
    #pragma pack(1)
#endif

#define SNMP_IP_MASK (WORD32)0xFFFFFF00

#define SNMP_DB_FIELDNAME_MAXLEN  32
#define SNMP_MAX_OID_LEN          128

#define PRO_MD5             (BYTE)1
#define PRO_SHA             (BYTE)2

#define usmDES             (BYTE)1
#define usmAES             (BYTE)2

#define SECMODEL_SNMPv1     (BYTE)1
#define SECMODEL_SNMPv2c    (BYTE)2
#define SECMODEL_SNMPusm    (BYTE)3

#define SNMP_DB_SUCCESS            0
#define SNMP_DB_CREATE_FAIL        1  /* 创建数据库失败   */
#define SNMP_DB_NULL_PTR           2  /* 参数空指针 */

typedef struct tagR_com2Sec_tuple {
    CHAR            community[SNMP_DB_FIELDNAME_MAXLEN];    
    DWORD           network;    
    DWORD           mask;    /* 子网掩码位数，1~32 */
    CHAR            secName[SNMP_DB_FIELDNAME_MAXLEN];    
    CHAR            contextName[SNMP_DB_FIELDNAME_MAXLEN];  
}_PACKED_1_ R_com2Sec_tuple, _FAR* LP_R_Com2Sec_tuple;

typedef struct tagR_usmUser {
    CHAR           name[SNMP_DB_FIELDNAME_MAXLEN];    
    CHAR           secName[SNMP_DB_FIELDNAME_MAXLEN];     
    CHAR           contextName[SNMP_DB_FIELDNAME_MAXLEN];
    BYTE           authProtocolType;    
    CHAR           authPassword[SNMP_DB_FIELDNAME_MAXLEN];        
    BYTE           privProtocolTYPE;       
    CHAR           privPassword[SNMP_DB_FIELDNAME_MAXLEN];
}_PACKED_1_ R_usmUser_tuple, _FAR* LP_R_usmUser_tuple;

typedef struct tagR_vacmgroup {        
    DWORD           securityModel;        
    CHAR            securityName[SNMP_DB_FIELDNAME_MAXLEN];        
    CHAR            groupName[SNMP_DB_FIELDNAME_MAXLEN];        
}_PACKED_1_ R_vacmgroup_tuple, _FAR* LP_R_vacmgroup_tuple;

typedef struct tagR_vacmview {        
    CHAR            viewName[SNMP_DB_FIELDNAME_MAXLEN];        
    DWORD           inclexcl;     
    CHAR            viewSubtree[SNMP_MAX_OID_LEN];        
    CHAR            viewMask[SNMP_DB_FIELDNAME_MAXLEN];        
}_PACKED_1_ R_vacmview_tuple, _FAR* LP_R_vacmview_tuple;

typedef struct tagR_vacmaccess {        
    CHAR            groupName[SNMP_DB_FIELDNAME_MAXLEN];        
    char            context[SNMP_DB_FIELDNAME_MAXLEN];        
    DWORD           securityModel;       
    DWORD           securityLevel;        
    DWORD           contextMatch;        
    CHAR            read[SNMP_DB_FIELDNAME_MAXLEN]; 
    CHAR            write[SNMP_DB_FIELDNAME_MAXLEN];
    CHAR            notify[SNMP_DB_FIELDNAME_MAXLEN];
}_PACKED_1_ R_vacmaccess_tuple, _FAR* LP_R_vacmaccess_tuple;

typedef struct tagR_trapcommunity_tuple {
    CHAR            community[SNMP_DB_FIELDNAME_MAXLEN];    
}_PACKED_1_ R_trapcommunity_tuple, _FAR* LP_R_trapcommunity_tuple;

typedef struct tagR_trap2sink_tuple {
    DWORD           network;    
    DWORD           portnumber;    
    DWORD           trapversion;
}_PACKED_1_ R_trap2sink_tuple, _FAR* LP_R_trap2sink_tuple;

typedef struct
{
    CHAR *pszTblName;
    BOOLEAN bCreated;
    BYTE ucPad;
    WORD16 wPad;
    WORD32 dwPad;
}_PACKED_1_ T_SNMP_Table_RegInfo;

void read_snmpdataconf(void);

WORD32 snmp_add_community(char *community,WORD32 network,DWORD mask,
                          char *secName,char *contextName);
WORD32 snmp_add_trap_community(char *trap_community);
WORD32 snmp_modify_trap2sink(BYTE ucIndex,DWORD dwIpAddress,DWORD portnum,DWORD dwTrapVersion);
WORD32 snmp_modify_trapversion(DWORD dwTrapVersion);
WORD32 snmp_delete_community(char *community, WORD32 network, DWORD mask,
                          char *secName,char *contextName);
BOOLEAN SnmpIsAllDbCreated(T_LM_CreateTable_Ack *ptCreateTblAck);
int check_sec_default();
void SnmpLoadDefault();
#ifdef WIN32
    #pragma pack()
#endif

#endif

