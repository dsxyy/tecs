/*********************************************************************
* 版权所有 (C)2002, 深圳市中兴通讯股份有限公司。
*
* 文件名称：oam_cfg.h
* 文件标识：
* 内容摘要：OAM配置模块对外接口结构体定义
* 其它说明：
            
* 当前版本：
* 作    者：殷浩
* 完成日期：2008.9.18
*
* 修改记录1：// 修改历史记录，包括修改日期、修改者及修改内容
*    修改日期：
*    版 本 号：
*    修 改 人：
*    修改内容：
* 修改记录2：…
**********************************************************************/
/**
  @file OAM_CFG.H
  @brief CLI接口的类型、返回值以及其他配置接口相关的宏定义
  @page OAM_CFG OAM子系统（配置模块）接口规范
  @author  OAM——3G平台

本文档整理的是配置模块的接口数据。\n

*/
#ifndef __OAM_CFG_H__
#define __OAM_CFG_H__

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

/***********************************************************
 *                    其它条件编译选项                     *
***********************************************************/

/***********************************************************
 *                   标准、非标准头文件                    *
***********************************************************/
#include "oam_typedef.h"

/***********************************************************
 *                        常量定义                         *
***********************************************************/

#define MAX_TABLE_FIELD               50   /**< table类型参数中最多字段数目，填写table类型参数的时候要注意 */
#define MAX_TABLE_ITEM                100  /**< table类型参数中一次最多可以放这么多记录数 */
#define OAM_CFG_MIN_PARANO            0    /**< OPR_DATA 结构中ParaNo最小值 */
#define OAM_CFG_MAX_PARANO            254  /**< OPR_DATA 结构中ParaNo最大值 */
#define MAX_STORE_LEN                 128  /**< max backup data length from protocol */

/***********************************************************
 *                       全局宏                            *
***********************************************************/
/*---------------------------------------------------
                     给BRS回txt加载完后的结果
--------------------------------------------------*/
#define  OAM_TXT_SUCCESS            0  /**< 成功 */
#define  OAM_TXT_READ_ERROR         1  /**< 读txt文件错误 */
#define  OAM_TXT_NO_DAT             2  /**< 无当前DAT文件 */
#define  OAM_TXT_CRC_ERROR          3  /**< txt文件CRC校验错误 */
#define  OAM_TXT_PARSE_ERROR        4  /**< 有命令解析错误 */
#define  OAM_TXT_INTERPRET_SLAVE    5  /**< 解释程序处于备状态 */
#define  OAM_TXT_TIMEOUT            6  /**< 解释超时 */
#define  OAM_TXT_CFGAGENT_UNKNOWN   7  /**< 配置代理未知错误*/

/*---------------------------------------------------
                     参数数据类型
--------------------------------------------------*/
#define DATA_TYPE_BYTE              (BYTE)1     /**< BYTE */
#define DATA_TYPE_WORD              (BYTE)2     /**< WORD16 */
#define DATA_TYPE_DWORD             (BYTE)3     /**< DWORD */
#define DATA_TYPE_INT               (BYTE)4     /**< INT */
#define DATA_TYPE_CHAR              (BYTE)5     /**< CHAR */
#define DATA_TYPE_STRING            (BYTE)6     /**< STRING */
#define DATA_TYPE_TEXT              (BYTE)7     /**< text : many a string  */
#define DATA_TYPE_TABLE             (BYTE)8     /**< represent it is a table varible */
#define DATA_TYPE_IPADDR            (BYTE)9     /**< ip address */
#define DATA_TYPE_DATE              (BYTE)10    /**< DATE = the number of the days from 1900/1/1 */
#define DATA_TYPE_TIME              (BYTE)11    /**< TIME = the number of the seconds from 2000/1/1/00:00:00*/
#define DATA_TYPE_MAP               (BYTE)12    /**< Mapping Variable */
#define DATA_TYPE_LIST              (BYTE)13    /**< represent it is a special Table varible */
#define DATA_TYPE_CONST             (BYTE)14    /**< represent constant variable */
#define DATA_TYPE_IFPORT            (BYTE)15    /**< represent interface port type */
#define DATA_TYPE_HEX               (BYTE)16    /**< 1234.2fcd.e34d */
#define DATA_TYPE_MASK              (BYTE)17    /**< IP MASK */
#define DATA_TYPE_IMASK             (BYTE)18    /**< IP INVERSE MASK */
#define DATA_TYPE_MACADDR           (BYTE)19    /**< Mac Address */
#define DATA_TYPE_FLOAT             (BYTE)20    /**< Float */
#define DATA_TYPE_BOARDNAME         (BYTE)21    /**< BOARDNAME */
#define DATA_TYPE_IPV6PREFIX        (BYTE)22    /**< IPV6 PREFIX */
#define DATA_TYPE_IPV6ADDR          (BYTE)23    /**< IPV6 ADDR */
#define DATA_TYPE_VPN_ASN           (BYTE)24    /**< ASN:nn */
#define DATA_TYPE_VPN_IP            (BYTE)25    /**< IP:nn */
#define DATA_TYPE_PHYADDRESS        (BYTE)26    /**< T_PhysAddress: rack-shelf-slot-cpu*/
#define DATA_TYPE_LOGICADDRESS    (BYTE)27      /**< T_LogicalAddr: system-subsystem-module-unit-sunit-index*/
#define DATA_TYPE_WORD64            (BYTE)28    /**< WORD64 */
#define DATA_TYPE_SWORD             (BYTE)29    /**< SWORD */
#define DATA_TYPE_LOGIC_ADDR_M      (BYTE)30    /**< LogicalAddrM */
#define DATA_TYPE_LOGIC_ADDR_U      (BYTE)31    /**< LogicalAddrU */
#define DATA_TYPE_IPVXADDR          (BYTE)32    /**< IPVXADDR */
#define DATA_TYPE_T_TIME            (BYTE)33    /**< T_TIME */
#define DATA_TYPE_SYSCLOCK          (BYTE)34    /**< SYSCLOCK */
#define DATA_TYPE_VMMDATETIME        (BYTE)35   /**< vmm定义的版本操作时间类型*/
#define DATA_TYPE_VMMDATE            (BYTE)36   /**< vmm定义的版本制作日期类型*/

#define DATA_TYPE_PARA              (BYTE)254    /**< Add for [] and whole by wangmh */

/*---------------------------------------------------
                     参数数据长度
--------------------------------------------------*/
#define LEN_IPADDR                  4   /**< ipaddr存储长度 */
#define LEN_MACADDR                 6   /**< Mac address存储长度 */
#define LEN_IPV6ADDR                16  /**< ipv6地址存储长度 */
#define VPN_NAME_LEN                32  /**< VPN存储长度 */

/*---------------------------------------------------
              输出模式 ，消息头里面OutputMode用
--------------------------------------------------*/
#define OUTPUT_MODE_NORMAL          0    /**< 输出模式:普通 */
#define OUTPUT_MODE_ROLL            1    /**< 输出模式:回滚 */
#define OUTPUT_MODE_REQUEST         2    /**< 输出模式:请求 */
#define OUTPUT_MODE_WAIT            3    /**< 输出模式:等待 */

/*---------------------------------------------------
        消息结构中ReturnCode代表的成功或出错信息
--------------------------------------------------*/
#define OAM_CFG_RETURNCODE               0                      /**< 返回码起始值 */
#define ERR_NO_NEED_THE_CMD             (OAM_CFG_RETURNCODE+0)  /**< 此命令不再需要 */
#define SUCC_AND_NOPARA                 (OAM_CFG_RETURNCODE+1)  /**< 命令执行成功，无回显 */
#define SUCC_AND_HAVEPARA               (OAM_CFG_RETURNCODE+2)  /**< 命令执行成功，有回显 */
#define SUCC_CMD_NOT_FINISHED           (OAM_CFG_RETURNCODE+3)  /**< 命令尚未执行完毕 */
#define SUCC_SND_PING_NOT_FINISHED      (OAM_CFG_RETURNCODE+32) /**< PING命令尚未结束 */

#define SUCC_CMD_DISPLAY_AND_KILLTIMER  (OAM_CFG_RETURNCODE+4)  /**< 命令尚未执行完毕, 关闭解释定时器, 需要回显 */
#define SUCC_CMD_DISPLAY_NO_KILLTIMER   (OAM_CFG_RETURNCODE+5)  /**< 命令尚未执行完毕, 不关闭解释定时器, 需要回显 */
#define SUCC_TERMINATE_INTERPT_TIMER    (OAM_CFG_RETURNCODE+6)  /**< 命令正在执行, 关闭解释定时器， 无需回显 */

#define ERR_AND_HAVEPARA                (OAM_CFG_RETURNCODE+7)  /**< 命令执行失败，有回显 */
#define ERR_MSG_FROM_INTPR              (OAM_CFG_RETURNCODE+8)  /**< 来自解释消息结构错误 */

#define  CLI_RET_KEEP_ALIVE_NOPARA (OAM_CFG_RETURNCODE+9)  /**应用保活消息,无回显*/

#define SUCC_WAIT_AND_HAVEPARA          (OAM_CFG_RETURNCODE+32) /**< 命令尚未执行完毕, 回显并等待 */  

#define OAM_MSG_XMLAGNET_TO_INTERPRET       (WORD32)(EV_OAMCFGBEGIN+37)  /**< XML适配器向解释发送配置请求*/


/*---------------------------------------------------
                 命令id中 GET/SET类型
--------------------------------------------------*/
#define IS_GET                      0  /**< 命令操作模式:GET */
#define IS_SET                      1  /**< 命令操作模式:SET */
#define IS_SAVE                     2  /**< 命令操作模式:SAVE */
#define IS_DEBUG                    3  /**< 命令操作模式:DEBUG */
#define IS_PROCESS                  4  /**< 命令操作模式:PROCESS */

#define OAM_CLI_DDM_CMDGRP_SET    (WORD32)0xE7000000  /**< DDM命令组:SET */
#define OAM_CLI_DDM_CMDGRP_GET    (WORD32)0xE8000000  /**< DDM命令组:GET */

#define CMD_RPU_SAVE               (DWORD)0x82020003  /* RPU存盘命令 */

/***********************************************************
 *                     全局数据类型                        *
***********************************************************/

/**
  @struct TABLE_ITEM
  @brief 参数表条目数据 
  <HR>
  @b 修改记录：
  
*/
typedef struct
{
	  /**
    @li @b 参数   Type
    @li @b 类型   BYTE
    @li @b 解释   Each Table Column Type
    */
    BYTE        Type;           
    /**
    @li @b 参数   Len
    @li @b 类型   BYTE
    @li @b 解释   Each Table Column Len
    */
    BYTE        Len;  
    /**
    @li @b 参数   ucPad1
    @li @b 类型   BYTE
    @li @b 解释   填充数据
    */          
    BYTE        ucPad1;
    /**
    @li @b 参数   ucPad1
    @li @b 类型   BYTE
    @li @b 解释   填充数据
    */ 
    BYTE        ucPad2;
}TABLE_ITEM, * PTABLE_ITEM;

/**
  @struct   TABLE_DATA
  @brief    参数表数据
  <HR>
  @b 修改记录：
*/
typedef struct 
{
	  /**
    @li @b 参数   Item
    @li @b 类型   TABLE_ITEM
    @li @b 解释   Table Fields Array Struct
    */ 
    TABLE_ITEM  Item[MAX_TABLE_FIELD];
    /**
    @li @b 参数   ItemNumber
    @li @b 类型   BYTE
    @li @b 解释   Table Fields Number
    */
    BYTE        ItemNumber; 
    /**
    @li @b 参数   ucPad1
    @li @b 类型   BYTE
    @li @b 解释   填充数据
    */            
    BYTE        ucPad1;
    /**
    @li @b 参数   ucPad1
    @li @b 类型   BYTE
    @li @b 解释   填充数据
    */
    BYTE        ucPad2;   
    /**
    @li @b 参数   Data
    @li @b 类型   BYTE[]
    @li @b 解释   Table Contents
    */  
    BYTE        Data[1];                
}TABLE_DATA, * PTABLE_DATA;

#if 0
/**
  @struct   OPR_DATA
  @brief    参数结构体
  <HR>
  @b 修改记录：
*/
typedef struct 
{
    /* WORD16        ParaNo;         Prarmeter Code Number */
    /**
    @li @b 参数   ParaNo
    @li @b 类型   BYTE
    @li @b 解释   Prarmeter Code Number
    */ 
    BYTE        ParaNo;         
  /*    CHAR        sName[MAX_PARANAME];      参数名称 */
    /**
    @li @b 参数   Type
    @li @b 类型   BYTE
    @li @b 解释   Prarmeter Type
    */ 
    BYTE        Type;           /**<@b Prarmeter Type */
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

/**
  @struct   MSG_COMM_OAM
  @brief    OAM配置通用消息体
  @li @b    消息功能   OAM配置信息请求/应答
  @li @b    消息方向:  OAM->3GPLAT或3GPLAT->OAM
*/
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
    BYTE        Number;         
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
#endif

/*job 入口函数原型*/
void Oam_Interpret(WORD16 wState, WORD32 dwMsgId, LPVOID pMsgPara, LPVOID pVar, BOOLEAN bIsSameEndian/*~*/);
void Oam_TelnetServer(WORD16 wState, WORD32 dwMsgId, LPVOID pMsgPara, LPVOID pVar, BOOLEAN byteOrder/*~*/);
void Oam_SSHServer(WORD16 wState, WORD32 dwMsgId, LPVOID pMsgPara, LPVOID pVar, BOOLEAN byteOrder);

extern T_CliJobInfo * GetJobInfoByJno(WORD16 wJno);
#ifdef WIN32
    #pragma pack()
#endif
   
#endif /*__OAM_CFG_H__*/


