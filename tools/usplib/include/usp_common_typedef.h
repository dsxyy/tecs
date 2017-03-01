/*********************************************************************
* 版权所有 (C)2010, 深圳市中兴通讯股份有限公司。
*
* 文件名称： usp_common_typedef.h
* 文件标识： 见配置管理计划书
* 内容摘要： USP项目通用数据类型定义
* 其它说明： 无
* 当前版本： 0.1
* 作    者： tao.linjun@zte.com.cn
* 完成日期： 2010-11-2
**********************************************************************/
#ifndef __USP_COMM_TYPEDEF_H__
#define __USP_COMM_TYPEDEF_H__

#ifdef LINK_OSS

#include "tulip.h"

#else

typedef int                   INT;
typedef unsigned char         BYTE;
typedef signed   char         SBYTE;
typedef char                  CHAR;
typedef unsigned short        WORD16;
typedef signed   short        SWORD16;
typedef unsigned int          WORD32;
typedef signed   int          SWORD32;
#ifdef WIN32
typedef unsigned __int64      WORD64;
typedef signed   __int64      SWORD64;
#define VOID void
#else
typedef unsigned long long    WORD64;
typedef signed   long long    SWORD64;
typedef void                  VOID;
#endif
typedef signed   long int     SWORDPTR;  /* 与指针大小相同的WORD类型, 32位或者64位 */
typedef unsigned long int     WORDPTR;   /* 与指针大小相同的WORD类型, 32位或者64位 */

typedef unsigned char         BOOLEAN;   /*应tecs的要求修改，与Tulip/Tecs项目保持一致      熊永刚  2012.11.19 */

typedef double                  FP64;       /* With prefix of d */

#endif

#endif

