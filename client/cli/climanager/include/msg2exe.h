/*********************************************************************
* 版权所有 (C)2002, 深圳市中兴通讯股份有限公司。
*
* 文件名称：Msg2Exe.h
* 文件标识：
* 内容摘要：发送消息到Execute
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
#ifndef __MSG_2_EXE_H__
#define __MSG_2_EXE_H__

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

#define RETURNCODE_INVALID  0

/*#define OAM_CLI_DDM_CMDGRP_SET    (WORD32)0xE7000000
#define OAM_CLI_DDM_CMDGRP_GET    (WORD32)0xE8000000*/
#define OAM_CLI_DDM_CMDGRP_GETTABLE    (WORD32)0xEA000000
#define OAM_CLI_CMM_CMDGRP            (WORD32)0x83000000

/* 对外暴露接口 */
extern void ExecCeaseProtocolCmd(TYPE_LINK_STATE *pLinkState);
extern void ExecCeaseOmmCfgCmd(TYPE_XMLLINKSTATE *pXmlLinkState);
extern void ExecSendCmdAgain(TYPE_LINK_STATE *pLinkState);
extern void DisposeMPCmd(TYPE_LINK_STATE *pLinkState);
extern MSG_COMM_OAM *OamCfgConstructMsgData(TYPE_LINK_STATE *pLinkState, MSG_COMM_OAM *ptMsgBuf);
extern void Oam_Cfg_SendKeepAliveMsgToTelnet(TYPE_LINK_STATE *pLinkState);
extern BOOL CheckMpExecuteMutex(TYPE_LINK_STATE *pLinkState, MSG_COMM_OAM *ptMsg);

#ifdef WIN32
    #pragma pack()
#endif

#endif /* __MSG_2_EXE_H__ */
