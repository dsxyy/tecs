/*********************************************************************
* 版权所有 (C)2002, 深圳市中兴通讯股份有限公司。
*
* 文件名称：HrTmrMsg.h
* 文件标识：
* 内容摘要：处理来自定时器的超时消息
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
#ifndef __HR_TMR_MSG_H__
#define __HR_TMR_MSG_H__

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
extern void RecvFromTimer(TYPE_LINK_STATE *pLinkState,WORD16 wMsgId);
extern void RecvFromXmlAdapterTimer(TYPE_XMLLINKSTATE *pLinkState,WORD16 wMsgId);

#ifdef WIN32
    #pragma pack()
#endif

#endif /* __HR_TMR_MSG_H__ */

