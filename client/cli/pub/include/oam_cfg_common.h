/*********************************************************************
* 版权所有 (C)2002, 深圳市中兴通讯股份有限公司。
*
* 文件名称：oam_cfg_common.h
* 文件标识：
* 内容摘要：OAM配置模块公用结构定义
* 其它说明：
            
* 当前版本：
* 作    者：马俊辉
* 完成日期：2007.6.5
*
* 修改记录1：// 修改历史记录，包括修改日期、修改者及修改内容
*    修改日期：
*    版 本 号：
*    修 改 人：
*    修改内容：
* 修改记录2：…
**********************************************************************/
#ifndef __OAM_CFG_COMMON_H__
#define __OAM_CFG_COMMON_H__

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

#include "oam_cfg.h"
#include "oamtime.h"
#include "pub_tmp.h"


/* 信号量index定义，OAM2被分配的index范围是35001~40000 */
#define  OAM_SEM_CLI_REG                    (WORD32)35001  /*平台应用注册库互斥用*/
#define  OAM_SEM_CLI_REGEX                (WORD32)35002  /*业务应用注册库互斥用*/
#define  OAM_SEM_CLI_LOG                    (WORD32)35003   /*climanager共享内存日志访问互斥用*/


/* 共享内存 index定义, OAM2被分配的index范围是35001~40000 */
#define  OAM_SHM_CLI_REG                    (WORD32)35001       /* 平台应用注册DAT信息使用的共享内存 */
#define  OAM_SHM_CLI_REGEX                (WORD32)35002       /* 非平台业务注册使用的共享内存 */
#define  OAM_SHM_CLI_LOG                    (WORD32)35003       /* climanager共享内存日志库*/


/* 定义此宏定义表示采用注册方式实现DAT组成 */
/*#define    OAM_CFG_REG*/

#define DM_NORMAL             0
#define DM_TAB                1
#define DM_QUESTION           2
#define DM_BLANKQUESTION      3

#define DEFAULT_BANNER               "\n\r         **************************************************************\n\r                 Welcome to CLI config shell of ZTE Corporation\n\r         **************************************************************\n\r"
#define DEFAULT_PROMPT               "CLI"
#define DEFAULT_ENABLE_PASSWORD      ""
#define DEFAULT_ENABLE_RITHT         5
#define DEFAULT_USERNAME             "admin"
#define DEFAULT_PASSWORD             "admin"
#define DEFAULT_RITHT                1
#define DEFAULT_CONSOLE_IDLE_TIMEOUT 120
#define DEFAULT_CONSOLE_ABS_TIMEOUT  1440
#define DEFAULT_TELNET_IDLE_TIMEOUT  120
#define DEFAULT_TELNET_ABS_TIMEOUT   1440

#define MIN_EXE_VTY_NUMBER           1  /* min vty number of execute */
#define MAX_EXE_VTY_NUMBER           127  /* max vty number of execute */

#define MIN_CLI_VTY_NUMBER        1  /* min vty number */
#define MAX_CLI_VTY_NUMBER        7  /* max vty number */

#define MIN_HTTP_VTY_NUMBER          5  /* min vty number of http */
#define MAX_HTTP_VTY_NUMBER          7  /* max vty number of http */

#define MIN_OMC_VTY_NUMBER           0  /* min vty number of OMC */
#define MAX_OMC_VTY_NUMBER           127 /* max vty number of OMC */


#define MAX_PARAMETER_NUMBER          250
#define PARATOBECONTINUED             255
#define MAX_PARANAME                  30   /**< 参数名称最大长度 */

#define MAX_STRING_LEN                255  /**< 包括\n\0的长度 */

/*MAP*/
#define MAP_TYPE_INT_INNER_ERROR     (WORD16)1
#define MAP_TYPE_EXE_INNER_ERROR     (WORD16)2
#define MAP_TYPE_PROTOCOL_ERROR      (WORD16)314
/*#define MAP_TYPE_MAINTAIN_ERROR      (WORD16)2000*/
#define MAP_TYPE_MAINTAIN_ERROR      (WORD16)4

/* 执行模式编号 */
#define EXEC_INTERPRETER             (BYTE)0     /* 解释执行操作 */
#define EXEC_DATABASE                (BYTE)1     /* 数据库操作 */
#define EXEC_PROTOCOL                (BYTE)2     /* 与协议相关的操作 */
#define EXEC_DEFEND                  (BYTE)9     /* Defend命令 */

/*MSG_COMM_OAM结构体中CMDID字段类型定义，omc私有接口用*/
#define UNKNOWN_CMD_TYPE		0	/*未知命令类别*/
#define CONFIG_CMD_TYPE		  1	/*配置命令类别*/
#define SHOW_CMD_TYPE		    2	/*显示命令类别*/
#define TOOLS_CMD_TYPE		  3	/*工具命令类别*/

/*命令组编号*/
#define OAM_CFG_CMDGRP_DEFEND        (BYTE)0x82

/* Interpret Command, cmdgroup:0x00*/
/*#define CMD_TELNET_SHOW_TERMINAL            (DWORD)0x06000004*/
#define CMD_TELNET_LOGOUT                   (DWORD)0x80040003
#define CMD_TELNET_EXIT                     CMD_TELNET_LOGOUT
#define CMD_TELNET_QUIT                     CMD_TELNET_LOGOUT

#define CMD_INTERPT_ENABLE                  (DWORD)0x80040001
#define CMD_INTERPT_DISABLE                 (DWORD)0x80040002
#define CMD_INTERPT_END                     (DWORD)0x80040004
#define CMD_INTERPT_EXIT                    (DWORD)0x80040005
/*#define CMD_INTERPT_CONFIG_TERMINAL         (DWORD)0x00040004*/
/*#define CMD_INTERPT_LOGIN                   (DWORD)0x00040007*/

#define CMD_LDB_SET_CLI_LANGTYPE     (DWORD)0x8001000B/*设置cli语言类型命令*/
#define CMD_LDB_GET_CLI_LANGINFO     (DWORD)0x8000000B/*查询cli语言类型命令*/

#define CMD_TELNET_WHO                      (DWORD)0x81000001
#define CMD_LDB_SET_BANNER                  (DWORD)0x81010002
#define CMD_LDB_SET_HOSTNAME                (DWORD)0x81010003
#define CMD_LDB_SET_CONSOLE_ABS_TIMEOUT     (DWORD)0x81010004
/*#define CMD_LDB_SET_ENABLE_PASSWORD         (DWORD)0x01010004*/
#define CMD_LDB_SET_CONSOLE_IDLE_TIMEOUT    (DWORD)0x81010005
#define CMD_LDB_SET_TELNET_ABS_TIMEOUT      (DWORD)0x81010006
#define CMD_LDB_SET_TELNET_IDLE_TIMEOUT     (DWORD)0x81010007
#define CMD_LDB_GET_USERNAME                (DWORD)0x81000008
#define CMD_LDB_SET_USERNAME                (DWORD)0x81010009
#define CMD_LDB_GET_MODESTATCK            (DWORD)0x8100000a
#define CMD_TELNET_SHOW_TERMINAL            (DWORD)0x8100000B

#define CMD_DEF_SET_SAVE                    (DWORD)0x82020002    /* 存盘命令 */

#define CMD_DEF_SET_USER                    (DWORD)0x82010004    /* 新增、删除用户 */
#define CMD_DEF_SET_PASSWORD                (DWORD)0x82010005    /* 设置用户密码 */
#define CMD_DEF_SET_ROLE                    (DWORD)0x82010006    /* 设置用户权限 */
#define CMD_DEF_SET_UNLOCKUSER              (DWORD)0x82010007    /* 解锁定用户 */
#define CMD_DEF_SET_SPECIFYIP               (DWORD)0x82010008    /* 设置指定IP */
#define CMD_DEF_GET_SHOWUSER                (DWORD)0x82000009    /* 查看用户信息 */
#define CMD_DEF_GET_SHOWUSERLOG             (DWORD)0x8200000A    /* 查看用户登录登出日志 */
#define CMD_DEF_GET_SHOWSPECIFYIP           (DWORD)0x8200000B    /* 查看指定IP */

#define CMD_DEF_SET_PASSWORDINTENSITY           (DWORD)0x82010009    /* 设置密码强度*/
#define CMD_DEF_SET_SIMPLEPASSLENGTH           (DWORD)0x8201000A    /* 设置弱密码最低长度 */

#define CMD_DEF_GET_PASSWORDINTENSITY           (DWORD)0x8200000C    /* 查看密码强度*/
#define CMD_DEF_GET_SIMPLEPASSLENGTH           (DWORD)0x8200000D    /* 查看弱密码最低长度 */

#define CMD_INSTALL_ACTIVE                 (DWORD)0xB4010005
#define CMD_INSTALL_ACTIVE_APTD       (DWORD)0xB401000B

/*efine CMD_DEF_SET_MSSYNC                  (DWORD)0x1901000c
#define CMD_DEF_PRO_DATASYNC                (DWORD)0x1905000b
#define CMD_DEF_SET_DELBUREAU               (DWORD)0x1901000d
#define CMD_DEF_SET_DELUIDHDLC              (DWORD)0x1901000e
#define CMD_DEF_SET_BUREAUBAKDIR            (DWORD)0x1901000f
#define CMD_DEF_GET_BUREAUBAKDIR            (DWORD)0x19000010
#define CMD_DEF_SET_LOADBUBAKDATA           (DWORD)0x19010011
#define CMD_DEF_SET_GETCURBUONBAK           (DWORD)0x19010012
#define CMD_DEF_GET_SAVESTATE               (DWORD)0x19000013*/

#define CMD_IP_TERMINAL_CMD                 (DWORD)0x04010042

/*需要对MSG_COMM_OAM接口进行赋值的命令*/
#define CMD_LPTL_PING                                 (DWORD)0x0405000C
#define CMD_LPTL_TRACE                                (DWORD)0x0405000E
#define CMD_LPTL_IPV6_PING                            (DWORD)0x32053FFC	
#define CMD_IS_PING_OR_TRACE(MSGID)  (((MSGID)==CMD_LPTL_PING) || \
                                     ((MSGID)==CMD_LPTL_TRACE)  || \
                                     ((MSGID)==CMD_LPTL_IPV6_PING))

#define CMD_LPTL_DEBUG_INFO                           (DWORD)0x1801000b
#define CMD_LPTL_SET_DEBUG_PACKET                     (DWORD)0x18010027
#define CMD_LPTL_SET_DEBUG_FSM                        (DWORD)0x18010026
#define CMD_LPTL_SET_TEMINAL_DISPLAY                  (DWORD)0x0401001d

#if 0
#define CMD_LPTL_SET_INTERFACE                        (DWORD)0x07010026
#define CMD_LPTL_SET_LOOPBACK                         (DWORD)0x0701002c
#define CMD_LPTL_SET_TUNNEL                           (DWORD)0x0701002b
#define CMD_LPTL_SET_IPROUTE                          (DWORD)0x0701000b
#define CMD_LPTL_SET_NAT                              (DWORD)0x07010036
#define CMD_LPTL_SET_NATPT                            (DWORD)0x3c01000b
#define CMD_LPTL_SET_ACL                              (DWORD)0x0701030B
#define CMD_LPTL_SET_ACL6    	                      (DWORD)0x0701031a
#define CMD_LPTL_SET_TERMINALDSP                      (DWORD)0x07010027

#define CMD_LPTL_SET_INTERFACE_PORT_CHANNEL           (DWORD)0x0701002d
#define CMD_LPTL_SET_INTERFACE_IPADDRESS              (DWORD)0x0701000c
#define CMD_LPTL_SET_INTERFACE_IPMAC                  (DWORD)0x0701004d
#define CMD_LPTL_SET_INTERFACE_IPMTU                  (DWORD)0x07010011
#define CMD_LPTL_SET_VRFCFG                           (DWORD)0x3701000b
#define CMD_LPTL_SET_INTERFACE_SUBINTERFACE           (DWORD)0x0701020f

#define CMD_LPTL_SET_MONITOR_SOURCE                   (DWORD)0x4401000c
#define CMD_LPTL_SET_MONITOR_DESTINATION              (DWORD)0x4401000d

#define CMD_LPTL_SET_VLAN_IGMP_SNOOPING_STATIC        (DWORD)0x0701006d
#define CMD_LPTL_SET_VLAN_IGMP_SNOOPING_MROUTER       (DWORD)0x0701006e

#define CMD_LPTL_GET_IP_IGMP_SNOOPING_STATISTIC       (DWORD)0x07000070
#define CMD_LPTL_GET_IP_IGMP_SNOOPING_STATISTIC_CLEAR (DWORD)0x07000071
#define CMD_LPTL_GET_INTERFACE_SWITCHPORT             (DWORD)0x4300000c

#define CMD_LPTL_GET_IPIF_BRIEF                       (DWORD)0x07000025
#define CMD_LPTL_GET_RUNNING_CONFIG                   (DWORD)0x07000028

#define CMD_LPTL_SET_ROUTE_IPE1                       (DWORD)0x1f010015
#define CMD_LPTL_SET_TETUNNEL                         (DWORD)0x4101000F
#define CMD_LPTL_SET_ROUTE_OSPF                       (DWORD)0x0d01000b

#define CMD_LPTL_SET_ROUTER_RIP                       (DWORD)0x0e01000b
#define CMD_LPTL_SET_ADDRESSFAMILY_IPV4               (DWORD)0x0e01002c
#define CMD_LPTL_SET_ROUTER_ISIS                      (DWORD)0x2401000B

#define CMD_LPTL_SET_POLICE_MAP             (DWORD)0X0701032C
#define CMD_LPTL_SET_RMPCFG                 (DWORD)0x4301000b
/*added by fls on 2008/05/13 (CRDCR00177258 【派生软件单】V05.03.50_TDRNC_P002 iBSC RNC和TDRNC网元V05.03.50起改为在线配置网元)*/
#define CMD_LPTL_SET_MP_INTERFACE        (DWORD)0x1101001b

/*IPSEC相关命令段起始*/
#define CMD_IPSEC_START										(DWORD)0x0701050b
#define CMD_IPSEC_END										(DWORD)0x07010521
	


/* added by fls on 20070915 (CTRCR00257393 MNIC_2debug版本上电SCH6挂起)*/
#define SUCC_AND_WAR_CODE   (RETURNCODE+15000)
#define SUCC_AND_WAR_2350_NOT_SUPPORT_4K_AND_1200_NOT_SUPPORT_VLAN  (SUCC_AND_WAR_CODE + 1)   /*15001*/
#define SUCC_AND_WAR_1200_NOT_SUPPORT_VLAN                          (SUCC_AND_WAR_CODE + 2)   /*15002*/
#define SUCC_AND_WAR_1200_NOT_SUPPORT_4K                            (SUCC_AND_WAR_CODE + 3)   /*15003*/
/* end of add*/
#endif

#define MAX_USERNAME_LEN            32  /*包括\0*/
#define MAX_PASSWORD_LEN            32  /*包括\0*/
                              
#define MAX_BANNER_LEN              256  
#define MAX_PROMPT_LEN              32

#define MAX_OMC_MSG_SIZE             (16*1024) /*发送消息的最大长度*/
#define MAX_OAM_MSG_LEN              1400       /*与后台网管最大消息的交互长度*/

/* MsgID: 1 - 100 Telnet->Interpretation */
#define MSGID_NEWCONNECT        10
#define MSGID_TERMINATE         20
#define MSGID_SENDSTRING        50
#define MSGID_ASKTELACK         60
#define MSGID_REQUESTDATA       130
#define MSGID_CEASEDATA         140
/*--------------------------------XML代理进程使用宏-----------------------------------------*/
#define MAX_SIZE_LINKSTATE           256

/* MsgID: 101 - 200 Interpretation0->Telnet */
#define MSGID_NOTPASSWORD       110
#define MSGID_ISPASSWORD        111

#define MSGID_OUTPUTRESULT      120
#define MSGID_CHANGEPROMPT      122
#define MSGID_SHOWCMDLINE       124
#define MSGID_KEEPALIVE         125       /*611000748643 添加保活消息*/

#define MSGID_DEBUGMESSAGE      126
/*#define MSGID_WAITRESULT        128*/
#define MSGID_NEEDEXTRINFO      129

#define MSGID_CHANGE_INPUTMODE 130

/*****************************************************************************************/
#define MAX_USERNAME_LEN_LOG            32  /*包括\0*/
#define LEN_IP_ADDR_STR             16
#define MAX_CMDLINE_LEN_LOG             512
#define MAX_LOG_FILE_READ_BUF       512
#define OAM_MAX_CMD_LOG_LINES    10000

#define OAM_CMDLINE_LOG_FILENAME                    "CliCmdLog.txt"
#define OAM_CMDLINE_LOG_FILENAME_BAK                "CliCmdLogBak.txt"

#define OAM_USER_INFO_FILE           "UserInfo.dat"
#define OAM_USER_INFO_FILE_TEMP      "UserInfo.dat.tmp"

#define OAM_USER_LOG_FILE          "UserLog.dat"
#define OAM_USER_LOG_FILE_TEMP     "UserLog.dat.tmp"

#define OAM_CFG_SCRIPT_FILE        "zx3gplatcfg.dat"
#define OAM_CFG_SCRIPT_TEMP        "zx3gplatcfg.tmp"

#define MAX_USER_LOG_FILE_SIZE     40*1024    /* 登录登出日志文件不能超过40K */

#define USER_LOG_FILE_FORMAT         0x6F6F6F6F
#define USER_LOG_FILE_VERSION        0x00001000

#define MAX_OAM_USER_NAME_LEN          32    /* 登录用户名最大长度 */
#define MIN_OAM_USER_PWD_LEN           8     /* 密码最小长度 */
#define INTENSITY_OAM_SIMPLEPWD           0     /* 弱密码强度 */
#define INTENSITY_OAM_STRONGPWD           1     /* 强密码强度 */
#define MAX_OAM_USER_PWD_LEN           32    /* 密码最大长度 */
#define ERR_OAM_USER_PWD_LEN           0    /*错误的密码长度 */
#define MIN_OAM_USER_SIMPLEPWD_DEFLEN           8     /*弱 密码默认最小长度 */

#define SHA1_RESULTLEN  (160/8)
#define MAX_OAM_USER_PWD_ENCRYPT_LEN   (SHA1_RESULTLEN*2 + 1)/*33*/    /* 登录用户密码(MD5加密过)最大长度，最后一位为结束符 */
#define MAX_SESSION_LEN          161

#define USER_FILE_FORMAT         0x8F8F8F8F
#define USER_FILE_VERSION        0x00010000

#define USER_ROLE_ADMIN     1  /* 用户权限-管理员 */
#define USER_ROLE_CONFIG    2  /* 用户权限-配置用户 */
#define USER_ROLE_VIEW      3  /* 用户权限-查看用户 */

#define MAX_USER_NUM_ROLE_ADMIN    1  /* 用户-管理员最大个数 */
#define MAX_USER_NUM_ROLE_CONFIG   3  /* 用户-配置用户最大个数 */
#define MAX_USER_NUM_ROLE_VIEW     5  /* 用户-查看用户最大个数 */

#define DEFAULT_USER_NAME             "admin"   /* 默认用户名 */
#define DEFAULT_USER_PASSWORD         "admin"   /* 默认用户密码 */

#define OAM_CONN_TYPE_SERIAL       1    /* 串口连接 */
#define OAM_CONN_TYPE_TELNET       2    /* TELNET连接 */
#define OAM_CONN_TYPE_SSH          3    /* SSH连接 */

#define USER_LOGIN_SUCCESS         1
#define USER_LOGIN_FAIL            0

#define USER_LOGOUT_NONE           0
#define USER_LOGOUT_COMMON         1
#define USER_LOGOUT_TIMEOUT        2
#define USER_LOGOUT_ERROR          3

#define OAM_CLI_REQUESTSUBSCR_TIME  (WORD32)0xffffffff

/*---------------------------------------------------
                 OAM内部使用的消息定义
--------------------------------------------------*/
#define EV_OAM_BEGIN 1000

#define EV_OAMCFGBEGIN   (WORD32)(EV_OAM_BEGIN)
/* FD事件 */
#define MSG_FD_CONNECT                          (WORD32)(EV_OAMCFGBEGIN)
#define MSG_FD_RECEIVE                          (WORD32)(EV_OAMCFGBEGIN+1)
#define MSG_FD_CLOSE                            (WORD32)(EV_OAMCFGBEGIN+2)

/* Telnet <--> Interpret */
#define OAM_MSG_TELNET_TO_INTERPRET	            (WORD32)(EV_OAMCFGBEGIN+3)
#define OAM_MSG_INTERPRET_TO_TELNET	            (WORD32)(EV_OAMCFGBEGIN+4)

/* Execute <--> Interpret */
#define OAM_MSG_INTERPRET_TO_EXECUTION          (WORD32)(EV_OAMCFGBEGIN+5)
#define OAM_MSG_EXECUTION_TO_INTERPRET          (WORD32)(EV_OAMCFGBEGIN+6)

/* Restore */
#define OAM_MSG_RESTORE_PREPARE_OK              (WORD32)(EV_OAMCFGBEGIN+7)
#define OAM_MSG_RESTORE_CONTINUE                (WORD32)(EV_OAMCFGBEGIN+8)

/* Save */
#define OAM_MSG_SAVE_BEGIN                      (WORD32)(EV_OAMCFGBEGIN+9)
/*#define OAM_MSG_SLAVE_SAVE                      (WORD32)(EV_OAMCFGBEGIN+10)*/
/*andy*/
#define EV_TEST_1                               (WORD32)(EV_OAMCFGBEGIN+88)
#define ITEM_TEST_COMM_TIME_SEND                (WORD32)(EV_OAMCFGBEGIN+89)
/*andy*/

/* Syn */
#define OAM_MSG_MS_CFGFILE_SYN                  (WORD32)(EV_OAMCFGBEGIN+11)
#define OAM_MSG_MS_CHANGE_OVER                  (WORD32)(EV_OAMCFGBEGIN+12)

/* To Others JOB */
#define OAM_MSG_EXECUTION_TO_PROTOCOL           (WORD32)(EV_OAMCFGBEGIN+13)
#define OAM_MSG_PROTOCOL_TO_EXECUTION           (WORD32)(EV_OAMCFGBEGIN+14)
#define OAM_MSG_PRO_PING_TO_EXECUTION           (WORD32)(EV_OAMCFGBEGIN+15)
#define OAM_MSG_PRO_TRACE_TO_EXECUTION          (WORD32)(EV_OAMCFGBEGIN+16)

#define EV_TXTCFG_ACK        (WORD32)(EV_OAMCFGBEGIN+24)

#define MSG_OFFLINE_CFG_TO_INTERPRET        (WORD32)(EV_OAMCFGBEGIN+38) /*BRS离线数据转在线数据请求*/
#define OAM_CFG_FINISH_ACK                  (WORD32)(EV_OAMCFGBEGIN+39) /*BRS离线数据转在线数据操作结束通知*/

#define OAM_MSG_TO_TELSERVER_OF_CEASE_ATTR  (WORD32)(EV_OAM_BEGIN + 42)

/* SSH */
#define OAM_MSG_SSH_CONN_ACK                    (WORD32)(EV_OAMCFGBEGIN+77)
#define OAM_MSG_SSH_AUTH_ACK                    (WORD32)(EV_OAMCFGBEGIN+78)
#define OAM_MSG_TELNET_TO_SSH                   (WORD32)(EV_OAMCFGBEGIN+79)
#define OAM_MSG_SSH_AUTH                        (WORD32)(EV_OAMCFGBEGIN+80)

#define OAM_CLI_LANGTYPE_LEN            12
/*linkstate.ucSynOrAsynInput用*/
typedef enum
{
    OAM_INPUT_MODE_CHANGE_SYN = 0,
    OAM_INPUT_MODE_CHANGE_ASYN
}TELSVR_INPUT_MODE;

typedef struct
{
    BYTE    bMsgID;         /* 消息ID */
    BYTE    bVtyNum;        /* TELNET虚拟终端号 */
    BYTE    bLastPacket;    /* 大容量消息是否结束 */
    BYTE    bDeterminer;    /* 字符串限制符 */
    WORD16    wSeqNo;         /* 和RFS交互的消息序列号 */
    WORD16    wDataLen;       /* 数据长度 */
    BOOL    bSSHAuth;       /* 是否为SSH用户登录消息 */
    BOOL    bSSHAuthRslt;   /* SSH用户登录结果 */
    CHAR    pData[1];       /* 消息数据 */
} _PACKED_1_ TYPE_TELNET_MSGHEAD, * PTYPE_TELNET_MSGHEAD;

typedef struct
{
    BYTE    bVtyNum;        /* TELNET虚拟终端号 */
    BYTE    bOutputMode;    /* 输出模式 */
    BYTE    bLastPacket;    /* 大容量消息是否结束 */
    BYTE    bPad;
    WORD16    wSeqNo;         /* 和RFS交互的消息序列号 */
    BOOL    bSSHConnRslt;    /* SSH连接结果 */
    BYTE    ucPad;          
    BOOL    bSSHAuth;       /* 是否为SSH用户登录消息 */
    BOOL    bSSHAuthRslt;   /* SSH用户登录结果 */
    WORD16    wDataLen;       /* 数据长度 */
} _PACKED_1_ TYPE_INTERPRET_MSGHEAD, * PTYPE_INTERPRET_MSGHEAD;

typedef struct 
{
    WORD16    edit_pos;       /*-命令行编辑的位置-*/
    WORD16    line_max;       /*-允许显示的列位数-*/
    WORD16    disp_line;      /*-终端显示的行数-*/
    WORD16    disp_pos;       /*-显示的起始位置-*/
}EXTRAINFO, * PEXTRAINFO;

typedef struct
{
    BYTE    bVtyNum;        /* TELNET虚拟终端号 */
    BYTE    bStopAttr;      /* 中止属性 */
} _PACKED_1_ TYPE_CEASE_MSG, * PTYPE_CEASE_MSG;

/* 记录命令行日志结构 */
typedef struct tag_T_CliCmdLogRecord
{
    BYTE           ucConnType;                        /* 接入类型:1-串口 2-TELNET 3-SSH */
    CHAR           szIpAddr[LEN_IP_ADDR_STR];    /* 保存Telnet终端IP地址 */
    CHAR           szUserName[MAX_USERNAME_LEN_LOG]; /* 用户名 */
    T_SysSoftClock tSysTime;                     /* 当前系统时间 */
    CHAR           szCmdLine[MAX_CMDLINE_LEN_LOG];   /* 当前命令行 */
    BOOL           bExecResult;                  /* 该命令执行结果 */
}_PACKED_1_ T_CliCmdLogRecord;

typedef struct tagTYPE_USER_FILE_HEAD
{
    DWORD    dwFormatDef;                 /* FormatDef == 0x8F8F8F8F,否则非法 */
    DWORD    dwVersion;                   /* 配置脚本文件版本   */
    CHAR     szIPAddr[LEN_IP_ADDR_STR];   /* 指定的IP */
    DWORD    dwUserBegin;                 /* 用户部分偏移量   */
    DWORD    dwUserNum;                   /* 用户数量 */
    DWORD    dwUserInfoLen;               /* 单条用户信息长度，为以后兼容做考虑 */
    BYTE        ucPasswordIntensity;     /*是否为强密码,默认为强密码*/
    BYTE        ucNorPasslesslen;          /*设置弱密码的最小程度，默认长度为8*/
    CHAR    aucCliLanguage[OAM_CLI_LANGTYPE_LEN]; /*cli脚本语言类型*/
    BYTE     ucPad[114];                  /* 扩展字段 */
}_PACKED_1_ TYPE_USER_FILE_HEAD;

typedef struct tagTYPE_USER_INFO
{
    BYTE     ucRole;                                  /* 用户权限: 1-管理员;2-配置用户;3-查看用户 */
    BOOL     bLocked;                                 /* 是否被锁定：TRUE-被锁定;FALSE-未锁定 */
    CHAR     szUserName[MAX_OAM_USER_NAME_LEN];        /* 用户名 */
    CHAR     szPassWord[MAX_OAM_USER_PWD_ENCRYPT_LEN]; /* 加密之后的用户密码 */
}_PACKED_1_ TYPE_USER_INFO;

typedef struct tagTYPE_USER_LOG_HEAD
{
    DWORD    dwFormatDef;     /* FormatDef == 0x6F6F6F6F,否则非法 */
    DWORD    dwVersion;       /* 日志文件版本   */
    DWORD    dwLogBegin;     /* 日志部分偏移量   */
    DWORD    dwLogNum;       /* 日志数量 */
}_PACKED_1_ TYPE_USER_LOG_HEAD;

typedef struct tagTYPE_USER_LOG
{
    WORD16         wNo;                               /* 日志编号 */
    BYTE           ucConnType;                        /* 接入类型:1-串口 2-TELNET 3-SSH */
    CHAR           tnIpAddr[LEN_IP_ADDR_STR];         /* 登录计算机IP地址 */
    CHAR           sUserName[MAX_OAM_USER_NAME_LEN];  /* 登录用户名 */
    T_SysSoftClock tSysClockLogin;                    /* 登录时间 */
    BYTE           ucLoginRslt;                       /* 登录结果:1-登录成功  0-登录失败 */
    T_SysSoftClock tSysClockLogout;                   /* 登出时间 */
    BYTE           ucQuitReason;                      /* 登出原因:1-正常退出  0-异常退出 2-超时退出*/
}_PACKED_1_ TYPE_USER_LOG;

/* OAM JOB TYPE范围：[0X0801, 0X0A00] */
#define JOB_TYPE_OAM_BEGIN              (WORD16)0x1
/* 执行进程类型定义 */
#define PROCTYPE_OAM_CLIMANAGER          (JOB_TYPE_OAM_BEGIN + 0)
#define PROCTYPE_OAM_TELNETSERVER       (JOB_TYPE_OAM_BEGIN + 1)
#define PROCTYPE_OAM_SSHSERVER             (JOB_TYPE_OAM_BEGIN + 2)
#define PROCTYPE_OAM_EXECUTE                 (JOB_TYPE_OAM_BEGIN + 3)


#ifdef WIN32
    #pragma pack()
#endif
   
#endif /*__OAM_CFG_COMMON_H__*/


