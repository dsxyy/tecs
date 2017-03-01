/*********************************************************************
* 版权所有 (C)2006, 深圳市中兴通讯股份有限公司。
*
* 文件名称： convertencoding.h
* 文件标识：
* 内容摘要：
* 其它说明： 无
* 当前版本： 
* 作    者： 齐龙兆——3G平台
*创建日期: 2008/9/10
**********************************************************************/

/***********************************************************
 *                    其它条件编译选项                     *
***********************************************************/
#ifndef _CONVERTENCODING_H_
#define _CONVERTENCODING_H_


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

extern BOOLEAN  Gb2312ToUtf8(const CHAR *ptrGBBuf,WORD16 wDataLen,CHAR *ptrUTF8Buf);/*GB2312转换为UTF-8*/

#ifdef WIN32
    #pragma pack()
#endif

#endif /* __PARA_CNVT_H__ */



