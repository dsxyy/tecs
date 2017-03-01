/*********************************************************************
* ��Ȩ���� (C)2002, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ�Msg2Tel.h
* �ļ���ʶ��
* ����ժҪ��������Ϣ��Telnet
* ����˵����
            
* ��ǰ�汾��
* ��    �ߣ�����
* ������ڣ�2007.6.5
*
* �޸ļ�¼1��// �޸���ʷ��¼�������޸����ڡ��޸��߼��޸�����
*    �޸����ڣ�
*    �� �� �ţ�
*    �� �� �ˣ�
*    �޸����ݣ�
* �޸ļ�¼2����
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

/* ���Ⱪ¶�ӿ� */
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
