/*********************************************************************
* 版权所有 (C)2008, 中兴通讯股份有限公司。
*
* 文件名称： pub_oam_cfg_event.h
* 文件标识：
* 其它说明： 本文件定义了V4平台的OAM子系统配置通道部分事件号划分
* 当前版本： V1.0
* 作    者： 傅龙锁
* 完成日期：
*
* 修改记录1：
*    修改日期：2009年10月13日
*    版 本 号：V1.0
*    修 改 人：傅龙锁
*    修改内容：创建
* 修改记录2：
*    修改日期：2011年1月17日
*    版 本 号：V1.0
*    修 改 人：傅龙锁
*    修改内容：由于宏定义OAM_CLI_DAT_SHM_MAX_NUM的值发生了改动(由20改为150)，；
*              受此影响，需要同时修改EV_OAMCLI_APP_REG_A2M_VER的值，由0改为1，
*              为兼容考虑，因为结构体T_CliAppReg用到了宏值
**********************************************************************/

/***********************************************************
 *                    其它条件编译选项                     *
***********************************************************/
#ifndef    _PUB_OAM_CFG_EVENT_H_
#define    _PUB_OAM_CFG_EVENT_H_
/**
@file   PUB_OAM_CFG_EVENT.H
@brief  事件号定义，网元内唯一
*/
/***********************************************************
 *                   标准、非标准头文件                    *
***********************************************************/
#ifdef __cplusplus
extern "C" {
#endif

#include "oam_typedef.h"
/***********************************************************
 *                        常量定义                         *
***********************************************************/
/** @brief 单板上平台应用使用的共享内存块  */
#define  OAM_CLI_DAT_SHM_MAX_NUM                     150
/** @brief 单板上非平台使用的共享内存块，每块单板只支持注册一个业务进程  */
#define  OAM_CLI_APP_AGT_SHM_MAX_NUM                 1
/** @brief OMP上非平台使用的共享内存块  */
#define  OAM_CLI_APP_MGT_SHM_MAX_NUM                 50
/** @brief 共享内存块数量  */
#define  OAM_CLI_APP_SHM_NUM    (OAM_CLI_APP_MGT_SHM_MAX_NUM > OAM_CLI_APP_AGT_SHM_MAX_NUM ? OAM_CLI_APP_MGT_SHM_MAX_NUM : OAM_CLI_APP_AGT_SHM_MAX_NUM)
/** @brief 最大长度，注意:该长度宏值不得改变(兼容性考虑) */
#define MAX_STORE_LEN                 128
#define EV_SCS_BEGIN                        (WORD32)60001
/***********************************************************
 *                       全局宏                            *
***********************************************************/

/**
@defgroup EV_OAMCFGBEGIN OAM配置消息起始编号
@brief 消息ID：    EV_OAMCFGBEGIN
@brief 用途：      OAM配置消息起始编号，OAM内部消息
@li @b 作者：      傅龙锁
@li @b 结构体：    无
@li @b 转换函数:   无
@li @b 其它说明：  无
@li @b 修改记录：  无
@verbatim
version=0 创建新消息
@endverbatim
@{*/

/** @brief 消息ID  */

/** @brief 消息版本号  */

/** @brief 消息体数据结构 */

/** @brief 无转换函数 */

/** @} */

/**
@defgroup EV_OAMCLI_APP_REG 平台应用注册DAT时使用
@brief 消息ID：    EV_OAMCLI_APP_REG
@brief 用途：      平台应用注册DAT时使用
@li @b 作者：      傅龙锁
@li @b 结构体：    T_CliAppData
@li @b 转换函数:   无
@li @b 其它说明：  无
@li @b 修改记录：  无
@verbatim
version=0 创建新消息
@endverbatim
@{*/


/** @brief 平台应用的CLI详细注册信息*/
typedef struct
{
    /**
    @li @b 参数   tAppJid
    @li @b 类型   JID
    @li @b 解释   应用注册的JID
    */ 
    JID           tAppJid;
    /**
    @li @b 参数   dwDatFuncType
    @li @b 类型   WORD32
    @li @b 解释   应用注册的DAT的功能版本类型
    */ 
    WORD32        dwDatFuncType;
    /**
    @li @b 参数   ucReadFlag
    @li @b 类型   BYTE
    @li @b 解释   Manager是否读取的标记
    */ 
    BYTE          ucReadFlag;
    /**
    @li @b 参数   ucCmpFlag
    @li @b 类型   BYTE
    @li @b 解释   是否已经比较过的标记
    */ 
    BYTE          ucCmpFlag;
    /**
    @li @b 参数   ucSaveType
    @li @b 类型   BYTE
    @li @b 解释   存盘类型1-TXT存盘；0-ZDB存盘
    */ 
    BYTE          ucSaveType;
    /**
    @li @b 参数   ucRegType
    @li @b 类型   BYTE
    @li @b 解释   注册类型 1-平台注册; 2-SBC等业务注册
    */ 
    BYTE          ucRegType;
} T_CliAppData;

/**
@defgroup EV_OAMCLI_APP_UNREG 平台应用注销DAT时使用
@brief 消息ID：    EV_OAMCLI_APP_UNREG
@brief 用途：      平台应用注销DAT时使用
@li @b 作者：      傅龙锁
@li @b 结构体：    与 EV_OAMCLI_APP_REG 公用同一个结构体，即T_CliAppData
@li @b 转换函数:   无
@li @b 其它说明：  无
@li @b 修改记录：  无
@verbatim
version=0 创建新消息
@endverbatim
@{*/

/** @brief 消息体数据结构 */


/**
@defgroup EV_OAMCLI_APP_REGEX 非平台应用注册需要配置进程时使用
@brief 消息ID：    EV_OAMCLI_APP_REGEX
@brief 用途：      非平台应用注册需要配置进程时使用
@li @b 作者：      傅龙锁
@li @b 结构体：    T_CliAppDataEx
@li @b 转换函数:   无
@li @b 其它说明：  无
@li @b 修改记录：  无
@verbatim
version=0 创建新消息
@endverbatim
@{*/

/** @brief 业务应用的CLI详细注册信息 */
typedef struct
{
    /**
    @li @b 参数   tAppJid
    @li @b 类型   JID
    @li @b 解释   应用注册的JID
    */ 
    JID         tAppJid;
    /**
    @li @b 参数   dwNetId
    @li @b 类型   WORD32
    @li @b 解释   应用注册的网元标识
    */ 
    WORD32      dwNetId;
    /**
    @li @b 参数   ucAgingTime
    @li @b 类型   BYTE
    @li @b 解释   此注册信息老化时间次数，如果超过3次同步时间，则认为老化
    */ 
    BYTE        ucAgingTime;
    /**
    @li @b 参数   ucPad
    @li @b 类型   BYTE[]
    @li @b 解释   填充字节
    */ 
    BYTE        ucPad[3];
} T_CliAppDataEx;

/**
@defgroup EV_OAMCLI_APP_REG_A2M CliAgent给Manager定时发送的平台应用注册信息
@brief 消息ID：    EV_OAMCLI_APP_REG_A2M
@brief 用途：      CliAgent给Manager定时发送的平台应用注册信息
@li @b 作者：      傅龙锁
@li @b 结构体：    T_CliAppReg
@li @b 转换函数:   无
@li @b 其它说明：  无
@li @b 修改记录：  2011-1-17 由于宏定义OAM_CLI_DAT_SHM_MAX_NUM的值被修改，考虑到兼容，需要修改消息版本号
@verbatim
version=0 创建新消息
@endverbatim
@{*/


/** @brief 平台应用的CLI注册表结构 */
typedef struct
{
    /**
    @li @b 参数   dwSemid
    @li @b 类型   WORD32
    @li @b 解释   信号量
    */ 
    WORD32          dwSemid;
    /**
    @li @b 参数   platpid
    @li @b 类型   pid_t
    @li @b 解释   管理进程pid，用来判断是否要清共享内存
    */ 
    pid_t           platpid;
    /**
    @li @b 参数   dwRegCount
    @li @b 类型   WORD32
    @li @b 解释   当前注册个数
    */ 
    WORD32          dwRegCount;
    /**
    @li @b 参数   tCliReg
    @li @b 类型   T_CliAppData[]
    @li @b 解释   注册数据
    */ 
    T_CliAppData    tCliReg[OAM_CLI_DAT_SHM_MAX_NUM];
} T_CliAppReg;

/**
@defgroup EV_OAMCLI_APP_REGEX_A2M CliAgent给Manager定时发送的非平台应用注册信息
@brief 消息ID：    EV_OAMCLI_APP_REGEX_A2M
@brief 用途：      CliAgent给Manager定时发送的非平台应用注册信息
@li @b 作者：      傅龙锁
@li @b 结构体：    T_CliAppAgtRegEx
@li @b 转换函数:   无
@li @b 其它说明：  无
@li @b 修改记录：  无
@verbatim
version=0 创建新消息
@endverbatim
@{*/


/** @brief 业务应用的CLI注册表结构 */
typedef struct
{
    /**
    @li @b 参数   dwSemid
    @li @b 类型   WORD32
    @li @b 解释   信号量
    */ 
    WORD32          dwSemid;
    /**
    @li @b 参数   apppid
    @li @b 类型   pid_t
    @li @b 解释   管理进程pid，用来判断是否要清共享内存
    */ 
    pid_t           apppid;
    /**
    @li @b 参数   dwRegCount
    @li @b 类型   WORD32
    @li @b 解释   当前注册个数
    */ 
    WORD32          dwRegCount;
    /**
    @li @b 参数   tCliReg
    @li @b 类型   T_CliAppDataEx[]
    @li @b 解释   注册数据
    */ 
    T_CliAppDataEx  tCliReg[OAM_CLI_APP_SHM_NUM];
} T_CliAppAgtRegEx;

/**
@defgroup EV_OAM_CFG_CFG CliManager 和应用直接配置消息
@brief 消息ID：    EV_OAM_CFG_CFG
@brief 用途：      CliManager 和应用直接配置消息
@li @b 作者：      傅龙锁
@li @b 结构体：    MSG_COMM_OAM
@li @b 转换函数:   无
@li @b 其它说明：  无
@li @b 修改记录：  无
@verbatim
version=0 创建新消息
@endverbatim
@{*/


/** @brief 参数TLV数据结构，用于MSG_COMM_OAM结构中 */
typedef struct 
{
    /**
    @li @b 参数   ParaNo
    @li @b 类型   BYTE
    @li @b 解释   Prarmeter Code Number
    */ 
    WORD16        ParaNo;         
    /**
    @li @b 参数   Type
    @li @b 类型   BYTE
    @li @b 解释   Prarmeter Type
    */ 
    BYTE        Type;
    /**
    @li @b 参数   Len
    @li @b 类型   BYTE
    @li @b 解释   Prarmeter Length If Type equal to DATA_TYPE_TABLE, it represent the number of the records
    */ 
    BYTE        Len;  
    /**
    @li @b 参数   Data
    @li @b 类型   BYTE[]
    @li @b 解释   Prarmeter Value If Type equal to DATA_TYPE_TABLE, it represent the struct "TABLE_DATA"
    */           
    BYTE        Data[1];        
}_PACKED_1_ OPR_DATA, * POPR_DATA;

/** @brief CLI通道和应用间的接口数据结构 */
typedef struct 
{
	  /**
    @li @b 参数   SeqNo
    @li @b 类型   DWORD
    @li @b 解释   Define the seqence number to distinct the command
    */
    DWORD       SeqNo;          
    /**
    @li @b 参数   DataLen
    @li @b 类型   DWORD
    @li @b 解释   Post Message Total Length
    */
    DWORD       DataLen;        
    /**
    @li @b 参数   CmdID
    @li @b 类型   DWORD
    @li @b 解释   Command ID
    */
    DWORD       CmdID;          
    /**
    @li @b 参数   LinkChannel
    @li @b 类型   DWORD
    @li @b 解释   Define which vty do it connect
    */
    DWORD        LinkChannel;    
    /**
    @li @b 参数   ReturnCode
    @li @b 类型   WORD16
    @li @b 解释   Success Or Fail Type
    */
    WORD16      ReturnCode;     
    /**
    @li @b 参数   LastData
    @li @b 类型   BYTE[]
    @li @b 解释   The Last Data From Protocol
    */
    BYTE        LastData[MAX_STORE_LEN];   
    /**
    @li @b 参数   BUF
    @li @b 类型   BYTE[]
    @li @b 解释   The Buffer
    */
    BYTE        BUF[MAX_STORE_LEN];
    /**
    @li @b 参数   bIfNo
    @li @b 类型   BYTE
    @li @b 解释   TRUE for no-command and FALSE for normal-command
    */
    BYTE        bIfNo;          
    /**
    @li @b 参数   CmdRestartFlag
    @li @b 类型   BYTE
    @li @b 解释   The Flag of Restart a Command
    */
    BYTE        CmdRestartFlag; 
    /**
    @li @b 参数   OutputMode
    @li @b 类型   BYTE
    @li @b 解释   Display mode "normal","roll","request"
    */
    BYTE        OutputMode;     
    /**
    @li @b 参数   Number
    @li @b 类型   BYTE
    @li @b 解释   Parameter Sum Number
    */
    WORD16        Number;         
    /**
    @li @b 参数   ucOamTag
    @li @b 类型   BYTE
    @li @b 解释   OAM特殊处理的标志，定义见interpret.h，应用透传
    */  
    BYTE        ucOamTag;
    /**
    @li @b 参数   aucPad
    @li @b 类型   BYTE[]
    @li @b 解释   填充数据
    */
    BYTE        aucPad[1];
    /**
    @li @b 参数   Data
    @li @b 类型   OPR_DATA[]
    @li @b 解释   Parameter Buffer
    */
    OPR_DATA    Data[1];        
}_PACKED_1_ MSG_COMM_OAM, * PMSG_COMM_OAM;

/** @brief 这个结构体是TLV格式的，本身就是兼容概念的，不需要转换函数 */

/** @} */

/**
@defgroup EV_TXTCFG_REQ txt存盘数据恢复的请求消息
@brief 消息ID：    EV_TXTCFG_REQ
@brief 用途：      txt存盘数据恢复的请求消息
@li @b 作者：      傅龙锁
@li @b 结构体：    无
@li @b 转换函数:   无
@li @b 其它说明：  无
@li @b 修改记录：  无
@verbatim
version=0 创建新消息
@endverbatim
@{*/

/** @brief 消息体数据结构 */

/** @brief 无转换函数 */

/** @} */

/**
@defgroup EV_TXTCFG_ACK txt存盘数据恢复的恢复消息
@brief 消息ID：    EV_TXTCFG_ACK
@brief 用途：      txt存盘数据恢复的恢复消息
@li @b 作者：      傅龙锁
@li @b 结构体：    无
@li @b 转换函数:   无
@li @b 其它说明：  无
@li @b 修改记录：  无
@verbatim
version=0 创建新消息
@endverbatim
@{*/
/** @brief 消息体数据结构 */

/** @brief 无转换函数 */

/** @} */


/**
@defgroup EV_OAM_CEASE_TO_APP 发送到应用的中止消息
@brief 消息ID：    EV_OAM_CEASE_TO_APP
@brief 用途：      发送到应用的中止消息
@li @b 作者：      傅龙锁
@li @b 结构体：    与 EV_OAM_CFG_CFG 消息是一样的
@li @b 转换函数:   与 EV_OAM_CFG_CFG 消息是一样的
@li @b 其它说明：  无
@li @b 修改记录：  无
@verbatim
version=0 创建新消息
@endverbatim
@{*/

/** @brief 消息体数据结构 */

/** @brief 无转换函数 */

/** @} */

/**
@defgroup EV_OAM_CFG_PLAN 应用上报命令执行进度的消息
@brief 消息ID：    EV_OAM_CFG_PLAN
@brief 用途：      应用上报命令执行进度的消息
@li @b 作者：      傅龙锁
@li @b 结构体：    T_Cmd_Plan_Cfg
@li @b 转换函数:   无
@li @b 其它说明：  无
@li @b 修改记录：  无
@verbatim
version=0 创建新消息
@endverbatim
@{*/


/** @brief 平台应用进度上报消息体数据结构 */
typedef struct
{
    /**
    @li @b 参数   wPlanNo
    @li @b 类型   WORD16
    @li @b 解释   进度上报消息序列号
    */  
    WORD16      wPlanNo;
    /**
    @li @b 参数   ucPlanValue
    @li @b 类型   BYTE
    @li @b 解释   进度值 0-100
    */  
    BYTE        ucPlanValue;
    /**
    @li @b 参数   ucPad
    @li @b 类型   BYTE[]
    @li @b 解释   填充字节
    */  
    BYTE        ucPad[2];
    /**
    @li @b 参数   dwCmdID
    @li @b 类型   DWORD
    @li @b 解释   进度上报的命令ID
    */  
    DWORD       dwCmdID;
    /**
    @li @b 参数   dwLinkChannel
    @li @b 类型   DWORD
    @li @b 解释   当前链路号，执行命令时，会在通用结构体MSG_COMM_OAM中传给应用
    */  
    DWORD       dwLinkChannel;
} T_Cmd_Plan_Cfg;


/**
@defgroup EV_OAMCLI_SPECIAL_REG SBC等业务注册的消息
@brief 消息ID：    EV_OAMCLI_SPECIAL_REG
@brief 用途：      SBC等业务注册的消息
@li @b 作者：      傅龙锁
@li @b 结构体：    与平台应用一样，T_CliAppData
@li @b 转换函数:   无
@li @b 其它说明：  无
@li @b 修改记录：  无
@verbatim
version=0 创建新消息
@endverbatim
@{*/

/** @brief 消息体数据结构 */

/** @brief 平台应用注册DAT消息结构体兼容转换函数 */

/** @} */

/**
@defgroup EV_OAMCFGEND OAM配置终止编号
@brief 消息ID：    EV_OAMCFGEND
@brief 用途：      OAM配置终止编号
@li @b 作者：      傅龙锁
@li @b 结构体：    无
@li @b 转换函数:   无
@li @b 其它说明：  无
@li @b 修改记录：  无
@verbatim
version=0 创建新消息
@endverbatim
@{*/

/** @brief 消息ID  */
#define EV_OAMCFGEND                            (WORD32)(EV_OAM_BEGIN + 1699)

/** @brief 消息版本号  */

/** @brief 消息体数据结构 */

/** @brief 无转换函数 */

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* end of _PUB_OAM_CFG_EVENT_H_ */

