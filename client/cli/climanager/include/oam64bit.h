/*********************************************************************
* 版权所有 (C)2002, 深圳市中兴通讯股份有限公司。
*
* 文件名称：Oam64Bit.h
* 文件标识：
* 内容摘要：
* 其它说明：
            
* 当前版本：
* 作    者：马俊辉 移植整理
* 完成日期：2007.6.5
*
* 修改记录1：// 修改历史记录，包括修改日期、修改者及修改内容
*    修改日期：
*    版 本 号：
*    修 改 人：
*    修改内容：
* 修改记录2：…
**********************************************************************/
#ifndef __OAM_64BIT_H__
#define __OAM_64BIT_H__

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

#define OAM64bit_SUCCESS        0

/* 对外暴露接口 */
extern INT32 OamCfgintpGetStringFrom64bit(UINT32 Low32bit,        /* 低32位 */
                                    UINT32 High32bit,       /* 高32位 */
                                    CHAR * pResultString    /* 结果 */
                                    );

extern BYTE int2Str_Of64bit(WORD32 dwHigh, WORD32 dwLow, CHAR *pResult );  
extern BYTE getWord64Str(CHAR *pInput, CHAR *pResult );  

#ifdef WIN32
    #pragma pack()
#endif

#endif  /* __OAM_64BIT_H__ */

