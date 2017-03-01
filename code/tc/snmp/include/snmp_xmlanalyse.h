#ifndef _SNMP_XMLANALYSE_H_
#define _SNMP_XMLANALYSE_H_
#include <net-snmp/net-snmp-config.h>
#include "types.h"
#include "snmpd.h"

BOOLEAN SnmpProtocolAnalyseXmlOid(const LPXmlNode ptNode, T_Oid_Description *ptOidDesc);
#endif
