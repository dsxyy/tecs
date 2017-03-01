/*********************************************************************
* ��Ȩ���� (C)2002, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ�oam_execute.h
* �ļ���ʶ��
* ����ժҪ��OAMִ�н���
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
#ifndef __OAM_EXECUTE_H__
#define __OAM_EXECUTE_H__

/***********************************************************
 *                     ��������                            *
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

/*switch�滻��,case�Ӿ��������
�滻ǰ:
switch(pRevOprData->ParaNo)
{
case 0:
    printf("0");
    reak;
case 1:
    rintf("1");
    break;
case 2:
case 3:
case 4:
    rintf("2/3/4");
    break;
default:
    printf("unknown");
    break;
}
�滻��:
SWITCH(pRevOprData->ParaNo)
{
CASE(1)
    printf("0");
    BREAK
CASE(1)
    printf("1");
    BREAK
CASE(3)
    printf("2/3/4");
    BREAK
DEFAULT
    printf("unknown");
    BREAK
}
END
*/

/* �滻ԭ����switch�Ӿ� */
#define SWITCH(x) \
{\
WORD16 __MY_INPARA__=(x); \
WORD16 __MY_PARANO__=0; \
WORD16 __MY_OFFSET__=0; \
do \
{ \
/* �滻ԭ����case�Ӿ�, ��ʾ����n��case�Ӿ�����ͬ�Ĵ���(n>0) */
#define CASE(n) \
for(__MY_OFFSET__=0;__MY_OFFSET__<(n);__MY_OFFSET__++) \
{ \
    if (__MY_INPARA__==(__MY_PARANO__+__MY_OFFSET__)) \
		break; \
} \
if (__MY_OFFSET__>=(n)) \
    __MY_PARANO__+=(n); \
else \
{ 
/* �滻ԭ����default�Ӿ� */
#define DEFAULT {
/* �滻ԭ����break�Ӿ� */
#define BREAK continue;}
/* ���ӽ�����(��������) */
#define END }while(1==2);}

#define S_Init                    (WORD16)0
#define S_Work                    (WORD16)1

/* ��ʱ�� */
#define TIMER_READ_CMD_SCRIPT     (TIMER_NO_1)
#define EV_TIMER_READ_CMD_SCRIPT  (EV_TIMER_1)
#define DURATION_READ_CMD_SCRIPT  (WORD16)(30*1000)

#define PRO_CMD_BASE              (WORD16)10
#define INIT_PACKET_NO            (WORD16)1

#define ERR_EXE_INNER_CFG_SUCC    (WORD16)1
#define ERR_EXE_INNER_CFG_FAIL    (WORD16)2
#define ERR_EXE_INNER_REC_TOOMANY (WORD16)3

#define ERR_DBS_MASTER_SAVE_FAIL  (WORD16)200
#define ERR_DBS_SLAVE_SAVE_FAIL   (WORD16)201
#define ERR_DBS_MP_IS_BUSY        (WORD16)101
#define ERR_DBS_UNKNOWN_EVEN      (WORD16)255

#define SAVEINFO_LEN_MAX          150  /*����ʱЯ������Ϣ*/

#if (_CPU_BYTE_ORDER == _LITTLE_ENDIAN)
    typedef struct tagCMDID
    {
        WORD16    OID;   /* objectID */
        BYTE    OPM;   /* ����ģʽ */
        BYTE    GRP;   /* ������ */
    }_PACKED_1_ CMDID;
#else
    typedef struct tagCMDID
    {
        BYTE    GRP;   /* ������ */
        BYTE    OPM;   /* ����ģʽ */
        WORD16    OID;   /* objectID */
    }_PACKED_1_ CMDID;
#endif

typedef union tagT_CMDID
{
  DWORD   dwID;
  CMDID   tUid;
}_PACKED_1_ T_CMDID;

typedef struct tagT_PCMDMatch
{
  BYTE    ucBID;
  BYTE    ucPID;
}_PACKED_1_ T_PCMDMatch;

typedef struct tagT_OMCToBTSAttachInfo 
{ 
    JID      SourceJID;
}_PACKED_1_ T_OMCToBTSAttachInfo;

typedef	T_OMCToBTSAttachInfo T_PRODB_REQ;

#if 0
#define DB_NAME_LEN    (WORD16)32
#define DB_INSTANCE_NUM_MAX    (WORD16)64

typedef struct tagT_SaveDBItem
{
    BOOL         ucNeedSaveAllFlg; /* �����ݿ��Ƿ�ȫ������̱�ǣ�1����ʾȫ������̣�0����ʾ���������*/
    BYTE         ucPad[3];       /*����ֽ�*/
    CHAR         dbName[DB_NAME_LEN];
}_PACKED_1_ T_SaveDB_Item;

/*CTRCR00113922 add by yinhao �����������̹���*/

typedef struct tagT_DBSMssaveReq
{
    BYTE     ucNeedSaveAllFlg; /* ��Ҫȫ������̱��*/
    JID      BackMgtJid;       /* ��̨��JID*/
}_PACKED_1_ T_DbsBack_SaveReq;

typedef struct tagT_DBSMssaveReq
{
    JID                   BackMgtJid;         /* ��̨��JID*/ 
    BOOL                   isAllDbSave; /*�Ƿ�CPU�����пⶼ���̣�1����CPU���������ݿⶼ����;0����ʾucDatabaseNum��tDatabaseSave��������ݿ����*/
    BYTE                   ucPad;       /*����ֽ�*/
    WORD16               wDbNum;       /*���δ��̵����ݿ�ʵ����Ŀ,Ŀǰһ��CPU�����64�����ݿ�*/
    T_SaveDB_Item     tDatabaseSave[DB_INSTANCE_NUM_MAX];       /*���δ��̵����ݿ�ʵ����Ŀ,Ŀǰһ��CPU�����64�����ݿ�*/
}_PACKED_1_ T_DbsBack_SaveReq;
#endif

#define DM_GETBULOADINFO     	(WORD16) 5230	/*��ѯǰ̨���ݿⱸ�ݾֵĵ�ǰ�������Ϣ.�ݶ�Ϊ5230���Ժ���ܱ仯*/

typedef struct
{
    WORD16    wTransId;          /* �����ʶ�� GES�������� */
    BYTE    ucNeedSaveAllFlg;  /* ��Ҫȫ������̱��*/
    JID     BackMgtJid;        /* ��̨��JID*/
}_PACKED_1_ TDbsBack_SaveReq;

#define BUBAKLOAD_FAIL_TAB_NUM    (WORD16)5  /*���ڼ���ʧ�ܱ�ͳ�Ƶ����鶨�壬cFailTabName��ʹ�ã�Ŀǰֻ�ṩǰ5��ʧ�ܵı���*/    
#define BUBAKLOAD_DBNAMELEN       (WORD16)13 /* ������󳤶�12 */
typedef struct
{
    BYTE     ucResult;    /* ���ؽ���� 0���سɹ�����������ʧ�� */
    BYTE     ucReLoad;    /* ������ʧ��ʱ������Դ�Ƿ��������������أ�  1 ǰ̨æ���������¼��أ�2����  */
    WORD16     wFailTabNum; /*����ʧ�ܵı����*/                        
    CHAR     strFailTabName[BUBAKLOAD_FAIL_TAB_NUM][BUBAKLOAD_DBNAMELEN];/*����ʧ�ܵı���*/
}_PACKED_1_ FtpSyncFailReason, *LPFtpSyncFailReason;

typedef struct
{
    WORD16     wRetCODE;      /* ������ */
    BYTE     ucBureauno;    /* ����ֺ� */
    BYTE     ucPad;         /* ����ֶ� */
    WORD16     wDbVersion;    /* ��汾�� */
    WORD16     wDbVersionRef; /* С�汾�� */
}_PACKED_1_ GETBULOADINFO_ACK, *LPGETBULOADINFO_ACK;

typedef struct
{
    WORD16    wTransId;         /* �����ʶ�� */
    BYTE    ucMorS;           /* 1:Master, 2:Slave */
}_PACKED_1_ TSAVESTATE_REQ;

typedef struct
{
    BYTE     ucBureauno;     /* Ԥ�������ľֺ� */
    BOOL     bMasterToSlave; /* �Ƿ񴥷�����ͬ��*/
    BOOL     bSave;          /* �Ƿ񴥷����̲���*/
    BOOL     bOmpToMp;       /* ��Ԫ�����ݷַ���־λ */
    DWORD    dwReserved[2];  /* Ԥ��2��DWORD�������䣬��0 */
}_PACKED_1_ TDbsFtpLoadBuInfo,  *LPTDbsFtpLoadBuInfo;

typedef struct
{
    BYTE     ucMsgType;  /* ��Ϣ����,��Ϊ34����ʾ�첽��Ϣ */
    BYTE     aucPad[3];  /* ����ֶ� */
}_PACKED_1_ GETBULOADINFO_REQ, *LPGETBULOADINFO_REQ;

typedef struct
{
    WORD16    wTransId;                      /*�����ʶ��*/
    CHAR    chSaveInfo[SAVEINFO_LEN_MAX];  /*���Ӵ�����Ϣ*/
    WORD16    wSaveStat;                     /*Ŀǰ�Ĵ���״̬*/
    DWORD   dwDevId;                       /*�豸ID������������ţ��ֺ���Ϣ����8λΪ�ֺţ��θ�16λΪ����*/
    WORD16    wModule;                       /*ģ���*/
    WORD16    wTblTotal;                     /*Ҫ���̵��ܱ���*/
    WORD16    wTblMDone;                     /*��Ŀ¼���Ѵ�ĵı���*/
    WORD16    wTblSDone;                     /*��Ŀ¼���Ѵ�ĵı���*/
}_PACKED_1_ TSAVESTATE_ACK;

/*ִ����ص�ͷ�ļ�*/
 /*void LoopReadCmdScript();*/
 /*extern void RecvFromInterpret(LPVOID pMsgPara, JID);*/
extern void ParseBrsTxtSaveData(TYPE_XMLLINKSTATE *pLinkState,LPVOID pMsgPara);
extern void ConstructBrsMapPara(MSG_COMM_OAM *ptMsgBuf);
extern void PrintMSGCOMMOAMStruct(MSG_COMM_OAM *ptMsg, CHAR *pDesc);
extern void Oam_RecvFromApp(TYPE_LINK_STATE *pLinkState,LPVOID pMsgPara);
extern void RecvFromDBS(WORD16 dwMsgId, LPVOID pMsgPara, CHAR *errStr, WORD32 dwBufSize, BOOLEAN bIsSameEndian);
extern void SendPacketToExecute(TYPE_LINK_STATE *pLinkState, MSG_COMM_OAM *ptMsg, JID tJid);
extern void Oam_ExecSaveCmd(TYPE_LINK_STATE *pLinkState);

extern BOOL GetCmdLogFileLines(void);
extern BOOL WriteLogOfCmdLine(T_CliCmdLogRecord *pCmdLog, BOOL bCmdExecRslt);

extern void Oam_SetCmdPlanTimer(TYPE_LINK_STATE *pLinkState);
extern void Oam_KillCmdPlanTimer(TYPE_LINK_STATE *pLinkState);
extern void Oam_RecvPlanInfoFromApp(TYPE_LINK_STATE *pLinkState, LPVOID pMsgPara, BOOLEAN bIsSameEndian);
extern void Oam_DealPlanTimer(TYPE_LINK_STATE *pLinkState);
extern void Oam_InnerSaveCmdLompErr(LPVOID ptOamMsg,WORD32 dwMsgId, CHAR *pStrOutResult);
extern void Oam_InnerKillCmdPlanTimer(LPVOID ptOamMsg,WORD32 dwMsgId);
extern void Oam_InnerSetCmdPlanTimer(LPVOID ptOamMsg,WORD32 dwMsgId);

extern void SetMapOprData(MSG_COMM_OAM *ptMsg, OPR_DATA *pOprData, WORD16 v1, WORD16 v2);
extern DWORD GetMsgStructSize(MSG_COMM_OAM *ptMsg, OPR_DATA *pOprData);

extern void Oam_RecvFromCliApp(WORD32 dwMsgId,LPVOID pMsgPara);
extern void Oam_ProRecMsgFromCliApp(TYPE_LINK_STATE *pLinkState,LPVOID pMsgPara);
extern void Oam_ProRecMsgToOmm(LPVOID pMsgPara);
extern BOOLEAN IfModuleIsLomp(WORD16 wModule);
extern INT Oam_String_Compare_Nocase(const CHAR *s1, const CHAR *s2,INT n);
extern void CfgMsgTransByteOder(MSG_COMM_OAM *ptr);

extern BOOL IsSaveCmd(DWORD CmdID);

/*�������ID*/
extern BYTE CheckGetOrSet(DWORD CmdID);
#ifdef WIN32
    #pragma pack()
#endif

#endif /*__OAM_EXECUTE_H__*/

