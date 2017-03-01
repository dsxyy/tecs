/*********************************************************************
* 版权所有 (C)2008, 深圳市中兴通讯股份有限公司。
*
* 文件名称： snmp_data_func.h
* 文件标识：
* 内容摘要：
* 其它说明： 无
* 当前版本： V1.0
* 作    者： 安志奇——3G平台
* 完成日期：
*
* 修改记录1：
*    修改日期：2008年xx月xx日
*    版 本 号：V1.0
*    修 改 人：
*    修改内容：创建
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
 *                    其它条件编译选项                     *
***********************************************************/
/***********************************************************
 *                   标准、非标准头文件                    *
***********************************************************/
/***********************************************************
 *                        常量定义                         *
***********************************************************/
#define MAX_SESSION_LEN 161
/***********************************************************
 *                       全局宏                            *
***********************************************************/


/***********************************************************
 *                     全局数据类型                        *
 ***********************************************************/
int SnmpGetXmlRpcValue(char *aucMethodName,vector<xmlrpc_c::value> &array_struct,int *next_index,int *array_num);
int SnmpSetXmlRpcValue(char *aucMethodName,xmlrpc_c::paramList &paramList,int *retcode);
int SnmpDeleteXmlRpcValue(char *aucMethodName,xmlrpc_c::paramList &paramList,int *retcode);
bool SnmpLoginAuth(const string &user);

    

 /***********************************************************
 *                     全局函数原型                        *
***********************************************************/
#ifdef WIN32
    #pragma pack()
#endif

#endif
