/*********************************************************************
* 版权所有 (C)2002, 深圳市中兴通讯股份有限公司。
*
* 文件名称：clireglib.h
* 文件标识：
* 内容摘要：OAM配置模块CLI进程注册对外接口结构体定义
* 其它说明：
            
* 当前版本：
* 作    者：殷浩
* 完成日期：2008.9.18
*
* 修改记录1：// 修改历史记录，包括修改日期、修改者及修改内容
*    修改日期：2010年9月2日
*    版 本 号：V1.0
*    修 改 人：傅龙锁
*    修改内容：增加提供给SBC等业务注册dat脚本的函数OamCliRegFuncSpecial
* 修改记录2：…
**********************************************************************/
/**
  @file clireglib.h
  @brief CLI注册接口函数定义
  @page CLIREGLIB OAM子系统（CLI进程注册）接口规范
  @author  OAM——3G平台

本文档整理的是CLI进程注册的接口数据。\n

*/
#ifndef _OAM_CLI_REG_LIB_H_
#define _OAM_CLI_REG_LIB_H_

/***********************************************************
 *                   标准、非标准头文件                    *
***********************************************************/
#include "oam_cfg.h"

/***********************************************************
 *                       全局宏                            *
***********************************************************/
/*---------------------------------------------------
                     注册函数返回值定义
--------------------------------------------------*/
#define  OAM_CLI_SUCCESS  0 /*成功*/
#define  OAM_CLI_FAIL     1 /*失败*/

/**
  @brief 平台应用向OAM进行注册需要进行配置的信息
  @param tJobID 与OAM进行配置交互的JID
  @param dwDatFuncType 此JOB与OAM进行配置交互时，使用到的DAT脚本类型  
  @return  OAM_CLI_SUCCESS(0)：成功    OAM_CLI_FAIL(1)：   失败
  <HR>
  @b 修改记录：
*/
BYTE  OamCliRegFunc(JID tJobID, WORD32 dwDatFuncType);

/**
  @brief 平台应用向OAM进行注销进行配置过的信息或者OAM接收到OSS
  @param tJobID  与OAM进行配置交互的JID  
  @return  OAM_CLI_SUCCESS(0)：成功    OAM_CLI_FAIL(1)：   失败
  <HR>
  @b 修改记录：
*/
BYTE  OamCliUnRegFunc(JID tJobID);

/**
  @brief 非平台的应用/业务进程向OAM进行注册需要进行配置的信息
  @param tJobID  与OAM进行配置交互的JID  
  @param dwNetId  LOMP所在的网元标识
  @return  OAM_CLI_SUCCESS(0)：成功    OAM_CLI_FAIL(1)：   失败
  <HR>
  @b 修改记录：
*/
BYTE  OamCliRegFuncEx(JID tJobID, WORD32 dwNetId);

/**
  @brief SBC等业务进程向OAM进行注册需要进行配置的信息
  @param tJobID  与OAM进行配置交互的JID  
  @param dwDatFuncType 此JOB与OAM进行配置交互时，使用到的DAT脚本类型
  @return  OAM_CLI_SUCCESS(0)：成功    OAM_CLI_FAIL(1)：   失败
  <HR>
  @b 修改记录：
*/
BYTE  OamCliRegFuncSpecial(JID tJobID, WORD32 dwDatFuncType);

#endif

