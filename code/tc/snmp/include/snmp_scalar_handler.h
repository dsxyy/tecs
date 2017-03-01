/*********************************************************************
* ��Ȩ���� (C)2011, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ� snmp_scalar_handler.h
* �ļ���ʶ��
* ����ժҪ��
* ����˵���� SNMP ��������/��ȡ�Ż�����
* ��ǰ�汾�� V1.0
* ��    �ߣ� �����񡪡�CSP ƽ̨
* ������ڣ�
*
* �޸ļ�¼1��
*    �޸����ڣ�2011��xx��xx��
*    �� �� �ţ�V1.0
*    �� �� �ˣ�������
*    �޸����ݣ�����
**********************************************************************/
#ifndef _SNMP_SCALAR_HALDLER_H
#define _SNMP_SCALAR_HALDLER_H

#include <net-snmp/net-snmp-config.h>
#include "types.h"
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include "snmpd.h"

#define SNMP_CACHE_ERROR     (int)-1
#define SNMP_CACHE_SUCCESS (int)0

void SnmpProtocolScalarRegister(T_Oid_Description    *pOidDesc);
#endif

