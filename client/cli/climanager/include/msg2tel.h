/*********************************************************************
* 版权所有 (C)2002, 深圳市中兴通讯股份有限公司。
*
* 文件名称：Msg2Tel.h
* 文件标识：
* 内容摘要：发送消息到Telnet
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
#ifndef __MSG_2_TEL_H__
#define __MSG_2_TEL_H__

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
extern void ChangePrompt(TYPE_LINK_STATE *pLinkState, BYTE bModeId);
extern void OAM_ChangeMode(TYPE_LINK_STATE *pLinkState, BYTE bModeId);

extern void SendLogoutToTelnet(TYPE_LINK_STATE *pLinkState, CHAR *sOutputResult);
extern void SendTelMsgErrToTelnet(TYPE_LINK_STATE *pLinkState, INT32 iErrCode);
extern void SendExeMsgErrToTelnet(TYPE_LINK_STATE *pLinkState, INT32 iErrCode);
extern void SendUserNameErrToTelnet(TYPE_LINK_STATE *pLinkState, CHAR *sOutputResult);
extern void SendPasswordErrToTelnet(TYPE_LINK_STATE *pLinkState, CHAR *sOutputResult);   
extern void SendSysInitInfoToTelnet(TYPE_LINK_STATE *pLinkState);
extern void SendStartupToTelnet(TYPE_LINK_STATE *pLinkState);
extern void SendUserNamePromptToTelnet(TYPE_LINK_STATE *pLinkState, CHAR *sOutputResult);
extern void SendPasswordPromptToTelnet(TYPE_LINK_STATE *pLinkState, CHAR *sOutputResult);
extern void SendNormalPromptToTelnet(TYPE_LINK_STATE *pLinkState, CHAR *sOutputResult);
extern void SendTextPromptToTelnet(TYPE_LINK_STATE *pLinkState);
extern void SendCeaseToTelnet(TYPE_LINK_STATE *pLinkState);
extern void SendAskInfoToTelnet(TYPE_LINK_STATE *pLinkState, INT32 iPos);
extern void SendStringToTelnet(TYPE_LINK_STATE *pLinkState, CHAR *sOutputResult);
extern void SendZDBSaveAckToClient(TYPE_LINK_STATE *pLinkState,WORD32 dwMsgId,CHAR *pStrOutResult,BOOLEAN isNeedTxtSave);
extern void SendTxtSaveAckToClient(TYPE_LINK_STATE *pLinkState,CHAR *pStrOutResult,BOOLEAN bOR);
extern void SendErrStringToClient(TYPE_LINK_STATE *pLinkState,CHAR *pStrOutResult);
extern void SendPacketToTelnet(TYPE_LINK_STATE *pLinkState, BOOLEAN bSendResultOnly);

extern void DisposeInnerCmd(TYPE_LINK_STATE *pLinkState);
extern void DisposeInterpretPosCmd(TYPE_LINK_STATE *pLinkState);
extern void DisposeInnerCmdForXmlAgent(MSG_COMM_OAM *ptOamMsg);
extern void Oam_CfgCloseAllLink(CHAR *pcReason);
extern void Oam_InnerCmdLogout(TYPE_LINK_STATE *pLinkState);

void Oam_SendOutputToTelnet(TYPE_LINK_STATE *pLinkState, WORD16 *pwPacketSize, BYTE *pPacket);
void Oam_ConstructTerminateMsg(WORD16 *pwPacketSize, BYTE *pPacket);
void Oam_ConstructChgPromptMsg(TYPE_LINK_STATE *pLinkState, WORD16 *pwPacketSize, BYTE *pPacket);
void Oam_ConstructChgInputModeMsg(TYPE_LINK_STATE *pLinkState, WORD16 *pwPacketSize, BYTE *pPacket);
void Oam_ConstructPasswordMsg(TYPE_LINK_STATE *pLinkState, WORD16 *pwPacketSize, BYTE *pPacket);
void Oam_ConstructShowCmdLineMsg(TYPE_LINK_STATE *pLinkState, WORD16 *pwPacketSize, BYTE *pPacket, BYTE *pbSendCmdLine);
void Oam_ConstructNeedExtrInfoMsg(WORD16 *pwPacketSize, BYTE *pPacket);

extern  BOOL CheckRuleOfPwd(CHAR *pPassWord);
extern void AddParaToMsg(MSG_COMM_OAM *pMsg, BYTE ucType, BYTE *pData, WORD16 wLen);
extern void AddMapParaToMsg(MSG_COMM_OAM *pMsg, WORD16 wType, WORD16 wValue);
//extern VOID AddMapParaToMsg(MSG_COMM_OAM *ptRtnMsg,WORD16 wMapType, WORD16 wMapValue);
extern void AddXmlRpcMapParaToCliMsg(MSG_COMM_OAM *pMsg, WORD32 dwMap);
extern void ConstructShowModeAckMsgParam(BYTE bModeStackTop,BYTE *paucModeId,MSG_COMM_OAM *ptRtnMsg);
#ifdef WIN32
    #pragma pack()
#endif

#endif /* __MSG_2_TEL_H__ */
