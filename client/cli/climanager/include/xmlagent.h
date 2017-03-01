/*********************************************************************
* ��Ȩ���� (C)2006, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ� xmlagent.h
* �ļ���ʶ��
* ����ժҪ��
* ����˵���� ��
* ��ǰ�汾�� 
* ��    �ߣ� �����ס���3Gƽ̨
*��������: 2008/9/10
**********************************************************************/

/***********************************************************
 *                    ������������ѡ��                     *
***********************************************************/
#ifndef _XMLAGENT_H_
#define _XMLAGENT_H_


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

#define MAX_NODETEXT_BUF_SIZE (WORD16)255
#define MAX_NODEATTR_BUF_SIZE (WORD16)255
#define MAX_SIZE_OPJID     (WORD16)65535
#define MAX_COUNT_TRM  256
#define MAX_SIZE_HANDSHANK_COUNT (WORD16)3
typedef struct tagT_TYPE_OMMRegister_REQ
{
    WORD16 wComponentId;/*�����������*/
    WORD16 wTRMNum;/*��ע���ն˱�Ÿ���*/
    WORD16 awTRMId[MAX_COUNT_TRM];/*��ƽ̨OAMע���TRM����*/
}OMMRegisterReq,OMMUnRegisterReq;/*�����ն�ע���ע������*/

typedef struct tagT_TYPE_OMMRegister_ACK
{
    WORD16 wComponentId;/*�����������*/
    WORD16 wTRMNum;/*��ע���ն˱�Ÿ���*/
    WORD16 awTRMId[MAX_COUNT_TRM];/*��ƽ̨OAMע���TRM����*/
    WORD16 awOR[MAX_COUNT_TRM];/*�������*/
}OMMRegisterAck,OMMUnRegisterAck;

typedef struct tagT_TYPE_OMMHandShank_REQ
{
    WORD16 wComponentId;/*�����������*/
}OMMHandShankReq;/*���ܷ�������ƽ̨OAM��������*/

typedef struct tatT_TYPE_OMMHandShank_ACK
{
    WORD16 wComponentId;/*�����������*/
    WORD16 wTRMNum;/*�����ն˸���*/    
    WORD16 aucTRMId[MAX_COUNT_TRM];/*��ǰOMC����TRM����*/    
}OMMHandShankAck;/*���ܷ�������ƽ̨OAM����Ӧ��*/

extern void ReceiveFromOmm(LPVOID ptrMsgBuf);/*OMM������Ϣ��OAM*/
extern void SendErrStringToOmm(TYPE_XMLLINKSTATE *ptLinkState,CHAR *ptrOutputResult);/*�����ܷ��ʹ���XML�ַ���*/
extern void SendStringToOmm(TYPE_XMLLINKSTATE *ptLinkState,CHAR *sOutputResult,BOOLEAN bOR,BOOLEAN bEof);/*�����ܷ���XML�ַ���*/
extern void RecvFromProtocolToOMM(LPVOID ptrMsgBuf);/*OAM������Ϣ��OMM*/
extern void RecvFromOP(LPVOID pMsgPara);/*����ҵ�񷵻���Ϣ*/
extern void ReceiveRegisterMsgFromOmm(LPVOID ptrMsgBuf);/*���ܶ�XMLע������*/
extern void ReceiveUnRegisterMsgFromOmm(LPVOID ptrMsgBuf);/*���ܶ�XMLע������*/
extern void ReceiveHandShakeMsgFromOmm(LPVOID ptrMsgBuf);/*���ܶ�XML��������*/
extern void ReceiveLinkCfgMsgFromOmm(LPVOID ptrMsgBuf);/*�������ܶ���������*/
extern void ReceiveCeaseCmdMsgFromOmm(LPVOID ptrMsgBuf);/*��������������ֹ����*/

extern void XML_SetCmdPlanTimer(TYPE_XMLLINKSTATE *ptLinkState);
extern void XML_KillCmdPlanTimer(TYPE_XMLLINKSTATE *ptLinkState);
extern void XML_RecvPlanInfoFromApp(LPVOID pMsgPara, BOOLEAN bIsSameEndian);
extern void XML_DealPlanTimer(TYPE_XMLLINKSTATE *ptLinkState);
extern void SendPLATAckToOMM(MSG_COMM_OAM *ptOamMsgBuf);

#ifdef WIN32
    #pragma pack()
#endif

#endif /* __PARA_CNVT_H__ */


