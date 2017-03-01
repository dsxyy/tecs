/*********************************************************************
* 版权所有 (C)2002, 深圳市中兴通讯股份有限公司。
*
* 文件名称：HrTelMsg.h
* 文件标识：
* 内容摘要：接收来自Telnet的消息
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
#ifndef __HR_TEL_MSG_H__
#define __HR_TEL_MSG_H__

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

/* 对外暴露接口 */
extern void RecvFromTelnet(TYPE_LINK_STATE *pLinkState,LPVOID pMsgPara);
extern BOOLEAN Oam_CfgGlobalPointersIsOk(void);
extern void Oam_CfgStopExcutingCmd(TYPE_LINK_STATE *pLinkState, CHAR *pstrReason);
extern void Omm_CfgStopExcutingCmd(TYPE_XMLLINKSTATE *pLinkState);

#ifdef WIN32
    #pragma pack()
#endif

#endif /* __HR_TEL_MSG_H__ */
