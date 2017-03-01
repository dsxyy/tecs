#ifndef __PUB_ADDITION__
#define __PUB_ADDITION__

#include <arpa/inet.h>   
#include <netinet/tcp.h>
#include <sys/types.h>
#include <fcntl.h>
#include   <netinet/in.h> 
#include   <arpa/inet.h> 
#include   <sys/types.h> 
#include   <sys/socket.h> 
#include "oam_typedef.h"

#define OAM_RETUB(p) \
	if((p) != NULL) \
	{ \
	    free((p)); \
	    (p) = NULL; \
	}
	
#define OAM_CFG_LOG_EMERGENCY       5
#define OAM_CFG_LOG_ALERT           4
#define OAM_CFG_LOG_CRITICAL        3
#define OAM_CFG_LOG_ERROR           2
#define OAM_CFG_LOG_NOTICE          1
#define OAM_CFG_LOG_DEBUG           0

/* ssh log */
#define OAM_SSH_LOG_EMERGENCY       5
#define OAM_SSH_LOG_ERROR           2
#define OAM_SSH_LOG_NOTICE          1
#define OAM_SSH_LOG_DEBUG           0 
/* #define OAM_CliLog(ucLevel, pcErr, ...) XOS_SysLog((ucLevel), (pcErr), ##__VA_ARGS__)        */

#define CONNECTION_FROM_SERIAL         1
#define CONNECTION_FROM_TELNET         2
#define CONNECTION_FROM_HTTP           3
#define CONNECTION_FROM_SSH            4

#define CONNECTION_TYPE_MIN  CONNECTION_FROM_SERIAL
#define CONNECTION_TYPE_MAX  CONNECTION_FROM_SSH

typedef int (*OamConnectCallback)(WORD16 , WORD16 ,  struct sockaddr_in);
typedef int (*OamReceiveCallback)(WORD16 , WORD16 ,LPVOID buf, WORD16 );
typedef int (*OamCloseCallback)(WORD16 , WORD16 );
typedef struct tag_OAM_FUNSET
{
    OamConnectCallback     oamConnect;
    OamReceiveCallback    oamReceive;      
    OamCloseCallback   oamClose;   
}T_OAM_FUNSET;

SWORD32 OAM_FD_Send(WORD16 fd, WORD16 flag, LPVOID buf, WORD16 len);
void OAM_FD_Quit(WORD16 fd, WORD16 flag);
SWORD32 OSS_RegOAMFuncSet(T_OAM_FUNSET* ptOamFuncSet);

/* OAM回调的消息 */
/*OAM注册函数时的返回值*/
#define OAM_REGCALLBACK_ERROR_CONFLICT              2              /*重复注册*/
#define OAM_REGCALLBACK_ERROR_ILLEGAL               1              /*非法的参数，如传递了空的指针*/
#define OAM_REGCALLBACK_SUCCESS	             		0             /*注册成功*/

/********************************************************************************/

/* 从任务发送消息 */
#define OSS_SendFromTask(msg_id, msg, msg_size, sender, receiver)           \
    XOS_SendAsynMsgNJ(msg_id, /*(BYTE *)*/msg, msg_size,                    \
                     XOS_MSG_VER0, XOS_MSG_MEDIUM,  /*版本号和优先级*/      \
                     sender, receiver)

/*业务进程存盘是txt格式还是zdb格式*/
#define SAVE_TYPE_AS_TXT        1
#define SAVE_TYPE_AS_ZDB       0

#if 0
/************************DBSTRAT进程事件号定义结束，最大为0x1BCF***************************/

/******************** 静态数据同步相关进程的消息的起始宏0x1BD0*****************************/
/*P_SSyncRx进程涉及的消息号*/
#define EV_RECEIVER                 (WORD16)((EV_DBRECEIVER)+0)     /* 7120 = 0x1BD0 */
#define EV_DATAUPDATE_REQ           (WORD16)((EV_RECEIVER)+1)       /* 7121 = 0x1BD1 */
#define EV_COMMBEGIN_REQ            (WORD16)((EV_RECEIVER)+2)       /* 7122 = 0x1BD2 */
#define EV_TABLESBEGIN_REQ          (WORD16)((EV_RECEIVER)+3)       /* 7123 = 0x1BD3 */
#define EV_TABLEBEGIN_REQ           (WORD16)((EV_RECEIVER)+4)       /* 7124 = 0x1BD4 */
#define EV_TUPLE_REQ                (WORD16)((EV_RECEIVER)+5)       /* 7125 = 0x1BD5 */
#define EV_TABLEEND_REQ             (WORD16)((EV_RECEIVER)+6)       /* 7126 = 0x1BD6 */
#define EV_TABLESEND_REQ            (WORD16)((EV_RECEIVER)+7)       /* 7127 = 0x1BD7 */
#define EV_WAIT_REQ                 (WORD16)((EV_RECEIVER)+8)       /* 7128 = 0x1BD8 */
#define EV_COMMCANCEL_REQ           (WORD16)((EV_RECEIVER)+9)       /* 7129 = 0x1BD9 */
#define EV_COMMEND_REQ              (WORD16)((EV_RECEIVER)+10)      /* 7130 = 0x1BDA */
#define EV_FTPSYNC_REQ              (WORD16)((EV_RECEIVER)+11 )     /* 7131 = 0x1BDB */
#define EV_FTPSYNC_FAIL             (WORD16)((EV_RECEIVER)+12 )     /* 7132 = 0x1BDC */
#define EV_FTPSYNC_SUCC             (WORD16)((EV_RECEIVER)+13 )     /* 7133 = 0x1BDD */
/*P_SSyncTx进程涉及的消息号*/
#endif
#define EV_TRANSMITTER              (WORD16)((EV_DBRECEIVER)+15)    /* 7135 = 0x1BDF */
#if 0
#define EV_DATASYNC_REQ             (WORD16)((EV_TRANSMITTER)+1)    /* 7136 = 0x1BE0 */
#define EV_COMMBEGIN_ACK            (WORD16)((EV_TRANSMITTER)+2)    /* 7137 = 0x1BE1 */
#define EV_TABLESBEGIN_ACK          (WORD16)((EV_TRANSMITTER)+3)    /* 7138 = 0x1BE2 */
#define EV_TABLEBEGIN_ACK           (WORD16)((EV_TRANSMITTER)+4)    /* 7139 = 0x1BE3 */
#define EV_TUPLE_ACK                (WORD16)((EV_TRANSMITTER)+5)    /* 7140 = 0x1BE4 */
#define EV_TABLEEND_ACK             (WORD16)((EV_TRANSMITTER)+6)    /* 7141 = 0x1BE5 */
#define EV_TABLESEND_ACK            (WORD16)((EV_TRANSMITTER)+7)    /* 7142 = 0x1BE6 */
#define EV_WAIT                     (WORD16)((EV_TRANSMITTER)+8)    /* 7143 = 0x1BE7 */
#define EV_COMM_END                 (WORD16)((EV_TRANSMITTER)+9)    /* 7144 = 0x1BE8 */
#define EV_COMMEND_ACK              (WORD16)((EV_TRANSMITTER)+10)   /* 7145 = 0x1BE9 */
#define EV_COMM_CANCEL              (WORD16)((EV_TRANSMITTER)+11)   /* 7146 = 0x1BEA */
#define EV_ERROR                    (WORD16)((EV_TRANSMITTER)+12)   /* 7147 = 0x1BEB */
#define EV_WARNING                  (WORD16)((EV_TRANSMITTER)+13)   /* 7148 = 0x1BEC */
#define EV_SAVE                     (WORD16)((EV_TRANSMITTER)+14)   /* 7149 = 0x1BED */
#endif
#define EV_SAVESTATE_QUERY          (WORD16)((EV_TRANSMITTER)+15)   /* 7150 = 0x1BEE用于移动omc向前台查询存盘状态*/
#define EV_OMPTOMPSYNC_STAT         (WORD16)((EV_TRANSMITTER)+16)   /* 7151 = 0x1BEF用于OMP的CM进程把OMP向MP数据分发结果报给后台*/
/**********************************************************************************/

#if 0

#define SCS_MC_PROC 1
#define SCS_VMM_DOWN 1

/*********取自pub_oss.h********/
/* 无效的逻辑地址 */
#define INVALID_MODULE          0xFFFF /* 无效MP编号 */
/*#define INVALID_UNIT            0xFFFF*/ /* 无效单板号 */
/*#define INVALID_SUNIT           0xFF*/   /* 无效单板上的处理器号 */

#define INVALID_MSG_ID           0xFFFF   /* 无效消息号 */
#define INVALID_MSG_DATALEN      0xFFFF   /* 无效消息体长度 */
#define INVALID_MSG_TYPE         0xFF     /* 无效消息类型 */

#define EV_VER_DATA_ACK 1

#define EV_MASTER_TO_SLAVE  5
#define EV_SLAVE_TO_MASTER  6
#define MSG_EXECUTION_TO_PROTOCOL 1
#define EV_VER_DATA_REQ 1
#define MSG_BRSSOCKET_MSG 1

#endif

#define INVALID_SUBSYS          0xFF   /* 无效子系统号 */


/*#define BRS_P_R_PROTOCOL 1*/
#define pZDB_StartupType 1
#define pZDB_IOType 1
#define pZDB_ReceiverType 1
#define pZDB_AccessType 1

#endif
