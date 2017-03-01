/*********************************************************************
* ��Ȩ���� (C)2010, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ� usp_common_typedef.h
* �ļ���ʶ�� �����ù���ƻ���
* ����ժҪ�� USP��Ŀͨ���������Ͷ���
* ����˵���� ��
* ��ǰ�汾�� 0.1
* ��    �ߣ� tao.linjun@zte.com.cn
* ������ڣ� 2010-11-2
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
typedef signed   long int     SWORDPTR;  /* ��ָ���С��ͬ��WORD����, 32λ����64λ */
typedef unsigned long int     WORDPTR;   /* ��ָ���С��ͬ��WORD����, 32λ����64λ */

typedef unsigned char         BOOLEAN;   /*Ӧtecs��Ҫ���޸ģ���Tulip/Tecs��Ŀ����һ��      ������  2012.11.19 */

typedef double                  FP64;       /* With prefix of d */

#endif

#endif

