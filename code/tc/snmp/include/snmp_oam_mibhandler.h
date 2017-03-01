#ifndef _SNMP_OAM_MIBHANDLER_H
#define _SNMP_OAM_MIBHANDLER_H
#include <net-snmp/net-snmp-config.h>
#include "types.h"
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include "snmpd.h"
void SnmpProtocolUnRegister(T_Module_Snmp_Description *pModuleDescription);
void SnmpProtocolRegister(T_Module_Snmp_Description *pModuleDescription);
T_Oid_Property *SnmpProtocolGetPropertyOffSet(oid *name,WORD name_len, 
T_Oid_Description *ptOidDesc, BYTE *ptEntry, BYTE **ptDataBuf);
int SnmpProtocolSetValue(netsnmp_variable_list *requestvb, T_Oid_Property *pOidProp, BYTE *pDataBuf);
#endif
