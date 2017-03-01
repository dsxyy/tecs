/*********************************************************************
* 版权所有 (C)2009, 中兴通讯股份有限公司。
*
* 文件名称： private_snmp_interface_event.h
* 文件标识： 
* 其它说明： 本文件定义了V4平台oam子系统snmp模块的事件号划分
* 当前版本： V1.0
* 作    者： 曹亮
* 完成日期： 
*
* 修改记录1：
*    修改日期：2010年4月9日
*    版 本 号：V1.0
*    修 改 人：曹亮
*    修改内容：创建
**********************************************************************/

/***********************************************************
 *                    其它条件编译选项                     *
***********************************************************/
#ifndef    _PRIVATE_SNMP_INTERFACE_EVENTDEF_H_
#define    _PRIVATE_SNMP_INTERFACE_EVENTDEF_H_
/**
@file   PRIVATE_SNMP_INTERFACE_EVENT.H
@brief  事件号定义，网元内唯一
*/

#ifndef _PACKED_1_
 #ifndef VOS_WINNT
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
 *                   标准、非标准头文件                    *
***********************************************************/
#include "oam_eventdef.h"
#include "pub_oam_event.h"
#include "oam_ddm_pub.h"
#include "pub_oam_event.h"
#include "pub_oam_ddm_event.h" 
#include "pub_oam_snmp_event.h"
/* #include "pub_ipmi_event.h" */
/**
@defgroup EV_SNMP_DDM_FUNC_REQ SNMP协议处理模块向SNMP接口模块发送分发请求消息
@brief 消息ID：    EV_SNMP_DDM_FUNC_REQ
@brief 用途：      SNMP协议处理模块向SNMP接口模块发送分发请求消息
@li @b 作者：      曹亮
@li @b 结构体：    T_SNMP_DDM_Req
@li @b 转换函数:   INVERT_MSG_T_SNMP_DDM_Req
@li @b 其它说明：  无
@li @b 修改记录：  无
@verbatim
version=0 创建新消息
@endverbatim
@{*/
/** @brief SNMP协议处理模块向SNMP接口模块发送分发请求消息ID  */
#define EV_SNMP_DDM_FUNC_REQ              (WORD32)(EV_SNMPBEGIN +  1)
/** @brief SNMP协议处理模块向SNMP接口模块发送分发请求消息版本号 */
#define EV_SNMP_DDM_FUNC_REQ_VER          (WORD32)0

/**
  @struct	T_SNMP_DDM_Req
  @brief	SNMP协议处理模块向SNMP接口模块发送分发请求消息

  @li @b	消息功能       SNMP协议处理模块向SNMP接口模块发送分发请求消息
  @li @b	消息号         EV_SNMP_DDM_FUNC_REQ
  @li @b	消息方向       snmp protocol->snmp interface
*/
typedef struct
{
    T_DDM_Req tDdmReq;
    JID       jobID;  /*需要发送到的JID*/
    BYTE      ucPad[4];
}_PACKED_1_ T_SNMP_DDM_Req;
/** @brief 消息结构体兼容转换函数 */
MSG_INVERT_COMMON(T_SNMP_DDM_Req, EV_SNMP_DDM_FUNC_REQ_VER)
/** @} */

/**
@defgroup EV_SNMPCLI_FUNC_MGT SNMP协议处理模块向SNMP接口模块发送分发IPMI请求消息
@brief 消息ID：    EV_SNMPCLI_FUNC_MGT
@brief 用途：      SNMP协议处理模块向SNMP接口模块发送分发IPMI请求消息
@li @b 作者：      曹亮
@li @b 结构体：    T_SNMP_IPMI_Req
@li @b 转换函数:   INVERT_MSG_T_SNMP_IPMI_Req
@li @b 其它说明：  无
@li @b 修改记录：  无
@verbatim
version=0 创建新消息
@endverbatim
@{*/
/** @brief SNMP协议处理模块向SNMP接口模块发送分发IPMI请求消息ID  */
#define EV_SNMPCLI_FUNC_MGT              (WORD32)(EV_SNMPBEGIN +  2)
/** @brief SNMP协议处理模块向SNMP接口模块发送分发IPMI请求消息版本号 */
#define EV_SNMPCLI_FUNC_MGT_VER          (WORD32)0

/**
  @struct	T_SNMP_IPMI_Req
  @brief	SNMP协议处理模块向SNMP接口模块发送分发IPMI请求消息

  @li @b	消息功能       SNMP协议处理模块向SNMP接口模块发送分发IPMI请求消息
  @li @b	消息号         EV_SNMPCLI_FUNC_MGT
  @li @b	消息方向       snmp protocol->snmp interface
*/
typedef struct
{
	JID       jobID;  /*需要发送到的JID*/
    BYTE      ucPad[4];
    T_TLV_SNMP_REQUEST tIpmiReq;
}_PACKED_1_ T_SNMP_IPMI_Req;
/** @brief 消息结构体兼容转换函数 */
MSG_INVERT_COMMON(T_SNMP_IPMI_Req, EV_SNMPCLI_FUNC_MGT_VER)
/** @} */

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
#endif /* end of _PRIVATE_OAM_DDM_EVENTDEF_H_ */
/**  @endcond */

