/*********************************************************************
* ��Ȩ���� (C)2008, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ� snmp_data_func.h
* �ļ���ʶ��
* ����ժҪ��
* ����˵���� ��
* ��ǰ�汾�� V1.0
* ��    �ߣ� ��־�桪��3Gƽ̨
* ������ڣ�
*
* �޸ļ�¼1��
*    �޸����ڣ�2008��xx��xx��
*    �� �� �ţ�V1.0
*    �� �� �ˣ�
*    �޸����ݣ�����
**********************************************************************/
#ifndef _SNMP_DATA_FUNC_H_
#define _SNMP_DATA_FUNC_H_

#include <cstdlib>
#include <string>
#include <iostream>
#include <xmlrpc-c/girerr.hpp>
#include <xmlrpc-c/base.hpp>
#include <xmlrpc-c/client_simple.hpp>
#include "sky.h"

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
 *                    ������������ѡ��                     *
***********************************************************/
/***********************************************************
 *                   ��׼���Ǳ�׼ͷ�ļ�                    *
***********************************************************/
/***********************************************************
 *                        ��������                         *
***********************************************************/
#define MAX_SESSION_LEN 161
/***********************************************************
 *                       ȫ�ֺ�                            *
***********************************************************/


/***********************************************************
 *                     ȫ����������                        *
 ***********************************************************/
int SnmpGetXmlRpcValue(char *aucMethodName,vector<xmlrpc_c::value> &array_struct,int *next_index,int *array_num);
int SnmpSetXmlRpcValue(char *aucMethodName,xmlrpc_c::paramList &paramList,int *retcode);
int SnmpDeleteXmlRpcValue(char *aucMethodName,xmlrpc_c::paramList &paramList,int *retcode);
bool SnmpLoginAuth(const string &user);

    

 /***********************************************************
 *                     ȫ�ֺ���ԭ��                        *
***********************************************************/
#ifdef WIN32
    #pragma pack()
#endif

#endif
