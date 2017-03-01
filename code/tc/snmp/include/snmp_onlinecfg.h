#ifndef _SNMP_ONLINECFG_H
#define _SNMP_ONLINECFG_H
#include <net-snmp/net-snmp-config.h>
#include "types.h"
#include "snmpd.h"
int SetSNMPTrapCommunity (char *TrapCommunity, size_t Community_len);
int CheckCommunityLength(char *community);
int CheckTrapVersion(int trapversion);
void SetSnmpCommunity(char *community);
void SetTrapAddress(char *cptr);
int DeleteTrapAddress(int address,int port);
#endif
