/*********************************************************************
* 版权所有 (C)2002, 深圳市中兴通讯股份有限公司。
*
* 文件名称： PUBTYPEDEF.H
* 文件标识：
* 其它说明： 头文件和共用的数据类型
* 当前版本： V1.0
* 作    者： 周元庆——3G平台
* 完成日期：
*
* 修改记录1：
*    修改日期：2002年10月23日
*    版 本 号：V1.0
*    修 改 人：周元庆
*    修改内容：创建
* 修改记录2：
*    修改日期：2002年12月28日
*    版 本 号：V1.0
*    修 改 人：周元庆
*    修改内容：将操作系统相关的头文件从Pub_TypeDef.h文件中引到
               OSS_SubSys.h
**********************************************************************/
#ifndef _PUB_TYPEDEF_H_
#define _PUB_TYPEDEF_H_

/***********************************************************
                   其它条件编译选项
***********************************************************/
/***********************************************************
 *                        常量定义                         *
***********************************************************/

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

#include <mqueue.h>
/* ********************************************************
                        基本数据类型                        
********************************************************* */
typedef  unsigned char      BYTE;
typedef  signed char         SBYTE;
typedef  unsigned char      BOOLEAN;
typedef  char               CHAR;
typedef  void               *LPVOID;
typedef  void               VOID;
typedef  unsigned short     WORD16;
typedef  unsigned short     WORD;
typedef  signed short       SWORD16;
typedef  signed int         SWORD32;  
typedef  signed int        INT32;  
typedef  signed long long INT64;
typedef  unsigned int       WORD32; 
typedef  unsigned int       UINT32; 
typedef  unsigned long long WORD64;
typedef  char               BOOL;
typedef  unsigned int       DWORD;
typedef unsigned char    T_PAD;
typedef   int                    INT;
typedef  unsigned int       XOS_TIMER_ID;
typedef unsigned long int   WORDPTR; 
/* 支持浮点打印所加 */
typedef double      DOUBLE;
typedef float       FLOAT; 
typedef long double LDOUBLE;

typedef int                 SOCKET;
typedef WORD32             XOS_TASK_T;  /**< 支撑定义的TaskID数据类型 */
//typedef fd_set              SOCKET_SET;

#define OK		0
#define ERROR		(-1)
//typedef  int    XOS_FD;


typedef struct tagTIME
{
    WORD32  dwSecond;       /**< 秒 */
    WORD16  wMilliSec;      /**< 毫秒 */
    BYTE    ucPad[2];
}T_Time; 

typedef struct tagJID
{
    WORD32  dwJno;          /**< JOB号, 高16位为JOB 类型, 低16位为JOB 实例号 */
    WORD32  dwDevId;        /**< 设备ID, 包含本局的局号信息, 高8位为局号，不支持传递其他信息*/
    WORD16  wModule;        /**< MP编号 */
    WORD16  wUnit;          /**< 单板号 */
    BYTE    ucSUnit;        /**< 单板上的处理器号 */
    BYTE    ucSubSystem;    /**< 子系统号 */
    BYTE    ucRouteType;    /**< 路由类型，区别业务/管理消息，左右板位，主备通信*/
    BYTE    ucExtendFlag; 	/**< 扩展标志 */
}JID;


typedef struct
{
    WORD           wReason;   /**<主板存盘结果，参见: ::_DB_E_SAVE_BOTHPATH, ::_DB_E_SAVE_MAINPATH, ::_DB_E_SAVE_BACKUP*/
    WORD           wSlaveRes; /**< 备板存盘结果 */
    WORD32         dwReserve;  /**< 预留字段，八字节对齐*/
    CHAR           dbName[100]; /**< 数据库名称 */
}_PACKED_1_  _db_t_save_db_result_item, *lp_db_t_save_db_result_item;

typedef struct
{
    WORD                         wDbNum;/**< 本次存盘的数据库实例数目*/
    WORD                         wTranId;/**< 流水号,只在主向备发起的存盘过程中使用 */
    WORD                        timeResult;  /**<本次存盘的结果，参见::_DB_TIME_SUCCESS,::_DB_TIME_ERROR,::_DB_TIME_BUSY*/
    WORD                            wReserve;  /**< 预留字段，八字节对齐*/
    _db_t_save_db_result_item    tDatabaseSaveResult[100];/**< 各数据库的具体存盘结果，参见: ::_db_t_save_db_result_item */
}_PACKED_1_ _db_t_saveresult,*  lp_db_t_saveresult;
/***********************************************************
 *                       全局宏                            *
***********************************************************/
#define TRUE   1
#define FALSE 0

typedef unsigned int T_PORT;
#define IPADDR   DWORD

#ifndef IPV4ADDR_LEN
#define IPV4ADDR_LEN      4
#endif

#ifndef IPV6ADDR_LEN
#define IPV6ADDR_LEN      16
#endif

#ifndef IPADDR_LEN_MAX
#define IPADDR_LEN_MAX         (IPV6ADDR_LEN)
#endif

#define IPADDR_IPNONE               (BYTE)0
#define IPADDR_IPV4                    (BYTE)4
#define IPADDR_IPV6                    (BYTE)6

#undef	OAM_MIN
#define OAM_MIN(a, b)  (((a) < (b)) ? (a) : (b))

/* MAC地址长度定义 */
#define INTERNAL_HARD_ADDRESS_LENTH   6

#define MAX_OAMMSG_LEN               (1024 * 16)  /**< packet size */
#define MAX_APPMSG_LEN                (1024 * 16)  /*和应用之间的消息最大长度*/

/*主控结构定义开始*/
#define  MAX_JOB_NUM                     5
#define JOB_NAME_SIZE                   32

typedef VOID (* JCBEntry)(
                         WORD16,    /* JOB's state */
                         WORD32,    /* Current message's ID */
                         void *,    /* Current message body */
                         void *,    /* JOB's private data */
                         BOOLEAN    /*
                                     * message body's endian is the
                                     * same as my endian or not
                                              */
                               );

typedef struct tagCliJobCreatReg
{
    WORD16    wIndex;
    CHAR        acName[JOB_NAME_SIZE];    /**< JOB名称，XOS_JOB_NAME_SIZE宏值定义为32*/
    JCBEntry    pEntry;     /**< JOB入口*/
    WORD32      dwStackSize;   /**<JOB堆栈大小,大小必须为4096的倍数*/
    WORD32      dwDataRegionSize;  /**<JOB私有数据区大小,大小必须为4096的倍数*/
} T_CliJobCreatReg;

typedef struct tagCliJobInfo
{
    WORD32      dwJno;
    WORD16      wState;
    CHAR          acMqueueName[JOB_NAME_SIZE + 10];
    mqd_t          mq_Recv;    /*接受数据的消息队列，供其他job发送消息用*/
    WORD32      dwCurMsgLen;
    WORD32      dwStackSize;
    JCBEntry      pEntry;
    XOS_TASK_T dwTaskId;
    LPVOID        pMsgRecvBuff;
    LPVOID        pDataRegion;
 } T_CliJobInfo;

typedef struct tagCliMsg
{
    WORD32 dwMsgId;
    WORD16 wDataLen;
    BYTE      aucMsg[MAX_OAMMSG_LEN];
}T_CliMsg;
/*主控结构定义结束*/
/***********************************************************
 *                     全局函数原型                        *
***********************************************************/


#endif /* end of _PUB_TYPEDEF_H_ */

