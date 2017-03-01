/*********************************************************************
* 版权所有 (C)2008, 深圳市中兴通讯股份有限公司。
*
* 文件名称： DDM_SNMP.h
* 文件标识：
* 内容摘要：
* 其它说明： 无
* 当前版本： V1.0
* 作    者：
* 完成日期：
**********************************************************************/
#ifndef DDM_SNMP_H_INCLUDE_
#define DDM_SNMP_H_INCLUDE_

/***********************************************************
                   其它条件编译选项                     
***********************************************************/

/***********************************************************
                   标准、非标准头文件                     
***********************************************************/
#define        MAX_STRING        20
#define        MAX_ACL_NUMBER    50


#define        SNMP_SUCCESS        0
#define        SNMP_FAIL               (WORD32)DDM_RETCODE_SNMP_BEGIN+0
#define        SNMP_ERR_UNKNOWN        (WORD32)DDM_RETCODE_SNMP_BEGIN+1
#define        SNMP_RECORD_NULL        (WORD32)DDM_RETCODE_SNMP_BEGIN+2
#define        SNMP_RECORD_FULL        (WORD32)DDM_RETCODE_SNMP_BEGIN+3
#define        SNMP_RECORD_EXIST       (WORD32)DDM_RETCODE_SNMP_BEGIN+4

#define        SNMP_RETCODE_SUCC_AND_NOPARA 16 /* 操作成功无数据返回*/
#define        SNMP_TRAP_ENABLE        0
#define        SNMP_TRAP_DISABLE        1
#define        SNMP_TRAP_VERSION_1        0
#define        SNMP_TRAP_VERSION_2        1

/* SNMP地址控制列表 */
#define        FUNCID_OAM_SNMP_ACL_CONFIG        (WORD32)0x00070000
/* SNMP地址控制列表请求 */
typedef struct
{
    T_IPADDR        dwIPAddr;    /* 地址控制段 */
    CHAR        strCommunity[MAX_STRING];    /* 联合体字符串 */
}_PACKED_1_ T_OAM_SNMP_ACL_CONFIG_Req;

/* SNMP地址控制列表应答 */
typedef struct
{
    T_IPADDR        dwIPAddr;    /* 地址控制段 */
    CHAR        strCommunity[MAX_STRING];    /* 联合体字符串 */
}_PACKED_1_ T_OAM_SNMP_ACL_CONFIG_Ack;

/* SNMP TRAP可用性配置 */
#define        FUNCID_OAM_SNMP_TRAP_ENABLE_CONFIG        (WORD32)0x00070001
/* SNMP TRAP可用性配置请求 */
typedef struct
{
    WORD32        dwTrapEnable;    /* TRAP是否可用 */
}_PACKED_1_ T_OAM_SNMP_TRAP_ENABLE_CONFIG_Req;

/* SNMP TRAP可用性配置应答 */
typedef struct
{
    WORD32        dwTrapEnable;    /* TRAP是否可用 */
}_PACKED_1_ T_OAM_SNMP_TRAP_ENABLE_CONFIG_Ack;

/* SNMP TRAP团体串配置 */
#define        FUNCID_OAM_SNMP_TRAP_COMMUNITY_CONFIG        (WORD32)0x00070002
/* SNMP TRAP团体串配置请求 */
typedef struct
{
    CHAR        strTrapCommunity[MAX_STRING];    /* TRAP联合体字符串 */
}_PACKED_1_ T_OAM_SNMP_TRAP_COMMUNITY_CONFIG_Req;

/* SNMP TRAP团体串配置应答 */
typedef struct
{
    CHAR        strTrapCommunity[MAX_STRING];    /* TRAP联合体字符串 */
}_PACKED_1_ T_OAM_SNMP_TRAP_COMMUNITY_CONFIG_Ack;

/* SNMP TRAP版本配置 */
#define        FUNCID_OAM_SNMP_TRAP_VERSION_CONFIG        (WORD32)0x00070003
/* SNMP TRAP版本配置请求 */
typedef struct
{
    WORD32        dwTrapVersion;    /* TRAP版本 */
}_PACKED_1_ T_OAM_SNMP_TRAP_VERSION_CONFIG_Req;

/* SNMP TRAP版本配置应答 */
typedef struct
{
    WORD32        dwTrapVersion;    /* TRAP版本 */
}_PACKED_1_ T_OAM_SNMP_TRAP_VERSION_CONFIG_Ack;

/* SNMP TRAP地址1配置 */
#define        FUNCID_OAM_SNMP_TRAP_ADDRESS1_CONFIG        (WORD32)0x00070004
/* SNMP TRAP地址1配置请求 */
typedef struct
{
    T_IPADDR        dwTrapAddress;    /* TRAP发送目的地址 */
    WORD32        dwTrapPort;    /* TRAP发送目的地址端口 */
}_PACKED_1_ T_OAM_SNMP_TRAP_ADDRESS1_CONFIG_Req;

/* SNMP TRAP地址1配置应答 */
typedef struct
{
    T_IPADDR        dwTrapAddress;    /* TRAP发送目的地址 */
    WORD32        dwTrapPort;    /* TRAP发送目的地址端口 */
}_PACKED_1_ T_OAM_SNMP_TRAP_ADDRESS1_CONFIG_Ack;

/* SNMP TRAP地址2配置 */
#define        FUNCID_OAM_SNMP_TRAP_ADDRESS2_CONFIG        (WORD32)0x00070005
/* SNMP TRAP地址2配置请求 */
typedef struct
{
    T_IPADDR        dwTrapAddress;    /* TRAP发送目的地址 */
    WORD32        dwTrapPort;    /* TRAP发送目的地址端口 */
}_PACKED_1_ T_OAM_SNMP_TRAP_ADDRESS2_CONFIG_Req;

/* SNMP TRAP地址2配置应答 */
typedef struct
{
    T_IPADDR        dwTrapAddress;    /* TRAP发送目的地址 */
    WORD32        dwTrapPort;    /* TRAP发送目的地址端口 */
}_PACKED_1_ T_OAM_SNMP_TRAP_ADDRESS2_CONFIG_Ack;

/* SNMP TRAP地址3配置 */
#define        FUNCID_OAM_SNMP_TRAP_ADDRESS3_CONFIG        (WORD32)0x00070006
/* SNMP TRAP地址3配置请求 */
typedef struct
{
    T_IPADDR        dwTrapAddress;    /* TRAP发送目的地址 */
    WORD32        dwTrapPort;    /* TRAP发送目的地址端口 */
}_PACKED_1_ T_OAM_SNMP_TRAP_ADDRESS3_CONFIG_Req;

/* SNMP TRAP地址3配置应答 */
typedef struct
{
    T_IPADDR        dwTrapAddress;    /* TRAP发送目的地址 */
    WORD32        dwTrapPort;    /* TRAP发送目的地址端口 */
}_PACKED_1_ T_OAM_SNMP_TRAP_ADDRESS3_CONFIG_Ack;

/* SNMP TRAP地址4配置 */
#define        FUNCID_OAM_SNMP_TRAP_ADDRESS4_CONFIG        (WORD32)0x00070007
/* SNMP TRAP地址4配置请求 */
typedef struct
{
    T_IPADDR        dwTrapAddress;    /* TRAP发送目的地址 */
    WORD32        dwTrapPort;    /* TRAP发送目的地址端口 */
}_PACKED_1_ T_OAM_SNMP_TRAP_ADDRESS4_CONFIG_Req;

/* SNMP TRAP地址4配置应答 */
typedef struct
{
    T_IPADDR        dwTrapAddress;    /* TRAP发送目的地址 */
    WORD32        dwTrapPort;    /* TRAP发送目的地址端口 */
}_PACKED_1_ T_OAM_SNMP_TRAP_ADDRESS4_CONFIG_Ack;

/* SNMP TRAP地址5配置 */
#define        FUNCID_OAM_SNMP_TRAP_ADDRESS5_CONFIG        (WORD32)0x00070008
/* SNMP TRAP地址5配置请求 */
typedef struct
{
    T_IPADDR        dwTrapAddress;    /* TRAP发送目的地址 */
    WORD32        dwTrapPort;    /* TRAP发送目的地址端口 */
}_PACKED_1_ T_OAM_SNMP_TRAP_ADDRESS5_CONFIG_Req;

/* SNMP TRAP地址5配置应答 */
typedef struct
{
    T_IPADDR        dwTrapAddress;    /* TRAP发送目的地址 */
    WORD32        dwTrapPort;    /* TRAP发送目的地址端口 */
}_PACKED_1_ T_OAM_SNMP_TRAP_ADDRESS5_CONFIG_Ack;


#endif

