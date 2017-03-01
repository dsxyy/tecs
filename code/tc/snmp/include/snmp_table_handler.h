/*********************************************************************
* 版权所有 (C)2011, 深圳市中兴通讯股份有限公司。
*
* 文件名称： snmp_table_handler.h
* 文件标识：
* 内容摘要：
* 其它说明： SNMP 批量设置/获取优化功能
* 当前版本： V1.0
* 作    者： 刘华振——CSP 平台
* 完成日期：
*
* 修改记录1：
*    修改日期：2011年xx月xx日
*    版 本 号：V1.0
*    修 改 人：刘华振
*    修改内容：创建
**********************************************************************/
#ifndef _SNMP_TABLE_HANDLER_H
#define _SNMP_TABLE_HANDLER_H

#include <net-snmp/net-snmp-config.h>
#include "types.h"
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include "snmpd.h"

void SnmpProtocolTableRegister(T_Oid_Description    *pOidDesc);
#endif

