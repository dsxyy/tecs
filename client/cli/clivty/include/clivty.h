/*********************************************************************
* 版权所有 (C)2002, 深圳市中兴通讯股份有限公司。
*
* 文件名称：CliVty.h
* 文件标识：
* 内容摘要：处理内部消息
* 其它说明：
            
* 当前版本：
* 作    者：马俊辉
* 完成日期：2007.6.5
*
* 修改记录1：// 修改历史记录，包括修改日期、修改者及修改内容
*    修改日期：
*    版 本 号：
*    修 改 人：
*    修改内容：
* 修改记录2：…
**********************************************************************/
#ifndef __OAM_CLIVTY_H__
#define __OAM_CLIVTY_H__
#include "oam_cfg_common.h"
/***********************************************************
 *                     常量定义                            *
***********************************************************/
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

extern void Oam_CliVtyInit();
extern void Oam_CliVtyConnect(LPVOID pMsgPara);
extern void Oam_CliVtyRecive(LPVOID pMsgPara);
extern void Oam_CliVtyClose(LPVOID pMsgPara);
extern void Oam_ProcMsgFromInterpret(BYTE* pMsgPara);
extern void Oam_ProcTimerEventFromInterpret(WORD32 wTimerPara);
extern void Oam_CloseAllVty(CHAR *pcReason);
extern void Oam_ProcCeaseMsgFromInterpret(BYTE* pMsgPara);
extern void Oam_ProcSSHAuth(LPVOID pMsgPara);
extern void Oam_SendBufData(void);
extern void Oam_CliVtyCommAbort(LPVOID pMsgPara);
extern int  CloseTelnetConn(sty *cty);
extern BYTE AddSty(TYPE_FD *tSocket);
extern sty *FindSty(TYPE_FD *tSocket);
extern sty *FindStyByFd(FD fd);
extern sty *FindExtraSty(TYPE_FD *tSocket);
extern int oam_close_socket_for_ssh(TYPE_FD *tSocket);
#ifdef WIN32
    #pragma pack()
#endif

#endif /* __CLIVTY_H__ */
