/*********************************************************************
* ��Ȩ���� (C)2002, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ�OamScript.h
* �ļ���ʶ��
* ����ժҪ��OAM�ű��ṹ��
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
* �޸ļ�¼2��
**********************************************************************/
#ifndef __OAM_SCRIPT_H__
#define __OAM_SCRIPT_H__

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

#include "oam_cfg_common.h"

/* ����ű����� */
#define MARK_BLANK        ' '
#define MARK_STRINGEND    '\0'
#define MARK_VIRGULE      '/'
#define MARK_ZERO         '0'
#define MARK_NINE         '9'
#define MARK_POINT        '.'
#define MARK_HYPHEN       '-'
#define MARK_COLON        ':'
#define MARK_RETURN       '\n'
#define MARK_POS          '^'
#define MARK_TYPE_STRING  '"'           /*�ַ����������пո�ʱ���ַ���Ҫ������*/
#define MARK_STRING_TRANS '\\'   /*�ַ��������е�ת���*/
#define MARK_CHAR_QUESTION '?'      /*�����ʺ�Ҳ������ģ���Ҫת��*/

#define STR_CMD_NO        "no"

#define READ_CMD_SCRIPT_ERR   "\n\r%The DAT file which contains CLI map type haven`t registed, can not use OAM config shell now!\n"

#define PATH_INVALID      0   /* ��Чֵ */
#define PATH_COMMAND      10  /* ������ */
#define PATH_PARAMETER    20  /* ���� */
#define PATH_TITLE        30  /* MAP, {}����{[][][]}��̧ͷ */

#define INVALID_MODENO    0
#define INVALID_NODE      0
#define INVALID_CMDINDEX  0
#define INVALID_PARAINDEX 0
#define INVALID_SHOWINDEX 0
#define INVALID_EXEID     0

/* bPrammarType == 1, 2 */
#define PRAMMAR_BRACKET_SQUARE  1 /* [] */
#define PRAMMAR_BRACKET_ANGLE   2 /* {} */
#define PRAMMAR_WHOLE_SYMBOL    3 /* WHOLE: ���������ɲ�����һ������ */
#define PRAMMAR_WHOLE_NOSYM     4 /* WHOLE: ��SYMBOL */
#define PRAMMAR_RECYCLE         5
#define PRAMMAR_MORE_ANGLE      6   /* {[][][]} */

#define FILE_FORMAT         0xFFFFFFFF
#define FILE_VERSION        0x00020000 /*0x00020000*/
#define FILE_VERSION_OLD    0x00010000

#define MAX_NODENAME        30 
#define MAX_NODECOMMENT     100 /*50*/ 
#define MAX_PARASYMBOL      MAX_PARANAME
#define MAX_PARACOMMENT     100 /*50 */

#define MAX_MODEHELP        80    /* ����\n\0�ĳ��� */

#define ATTR_FLAG_HASNO     0x01
#define ATTR_FLAG_CANCEASE  0x02
#define ATTR_FLAG_NEEDACK   0x04
#define ATTR_FLAG_ONMENU    0x08
#define ATTR_FLAG_CANCEASE_NEEDACK    0x10

/*��ֹ����*/
#define CEASE_ATTR_CANCEASE_NEEDACK    0
#define CEASE_ATTR_CANCEASE_NOACK      1
#define CEASE_ATTR_CANNOTCEASE         2

#define CMD_ATTR_HASNO(_P_)    (((_P_)->bAttrFlags & ATTR_FLAG_HASNO) != 0)
#define CMD_ATTR_CANCEASE(_P_) (((_P_)->bAttrFlags & ATTR_FLAG_CANCEASE) != 0)
#define CMD_ATTR_NEEDACK(_P_)  (((_P_)->bAttrFlags & ATTR_FLAG_NEEDACK) !=0)
#define CMD_ATTR_ONMENU(_P_)   (((_P_)->bAttrFlags & ATTR_FLAG_ONMENU) !=0)
#define CMD_ATTR_CANCEASE_NEEDACK(_P_) (((_P_)->bAttrFlags & ATTR_FLAG_CANCEASE_NEEDACK) != 0)

#define OAM_CLI_LANGTYPE_OLDFMT     "unknown"
#define OAM_CLI_LANGTYPE_ENGLISH    "english"
#define OAM_CLI_LANGTYPE_CHINESE    "chinese"
/*#define OAM_CLI_LANGTYPE_LEN            12*/

/* file head */
typedef struct tagTYPE_FILE_HEAD
{
    DWORD    dFormatDef;       /* FormatDef == 0xFFFFFFFF,����Ƿ� */
    DWORD    dVersion;         /* ����ű��ļ��汾   */
    DWORD    dData;            /* ����ű��ļ��������� */
    DWORD    dTime;            /* ����ű��ļ��������� */
    DWORD    dPhyBoardType;    /* ����ű��ļ���������� */
    DWORD    dLogicBoardType;  /* ����ű��ļ��߼������� */
    /*DWORD    dPad[6];       */   /* ����ű��ļ�ͷԤ����չ */
    DWORD    dwCurLangDatSize; /*��ǰ�������͵�DAT�ļ���С*/
    BYTE        aucLanguageVer[OAM_CLI_LANGTYPE_LEN]; /* ���������ַ���,���ģ�Chinese;Ӣ�ģ�English */
    DWORD    adwPad2[2];       /* �����ֶ�2 */
    DWORD    dModeBegin;       /* ģʽ����ƫ���� */
    DWORD    dCmdTreeBegin;    /* ����������ƫ���� */
    DWORD    dCmdAttrBegin;    /* �������Բ���ƫ���� */
    DWORD    dCmdParaBegin;    /* �������Բ���ƫ���� */
    DWORD    dFormatBegin;     /* ��ʾ��ʽ����ƫ���� */
    DWORD    dMapTypeBegin;    /* ת�������Ͳ���ƫ���� */
    DWORD    dMapItemBegin;    /* ת�������ƫ���� */
    DWORD    dMapStrBegin;     /* ת��ֵ�ַ���ƫ���� */
}_PACKED_1_ TYPE_FILE_HEAD;

/* mode */
typedef struct tagTYPE_MODE
{
    BYTE     bModeID;          /* ģʽID     */
    WORD16     wModeClass;       /* ��ģʽ�ļ���   */
    WORD16     wTreeRootPos;     /* ��������λ��   */
    BYTE     bHasNo;           /* ��ģʽ�����Ƿ���NO���� */
    BYTE     bReserved;        /* �����ֽ�     */
    CHAR     sOddPrompt[MAX_PROMPT_LEN];   /* ���ӵ���ʾ��   */
    CHAR     sModeHelp[MAX_MODEHELP];  /* ��ģʽ�İ�����Ϣ */
}_PACKED_1_ TYPE_MODE;

/* tree node*/
typedef struct tagTYPE_TREE_NODE
{
    WORD16     wNextNodePos;         /* ��һ���ڵ�λ�� */
    WORD16     wNextSonPos;          /* ��һ��ڵ�λ�� */
    WORD16     wCmdAttrPos;          /* ������������   */
    CHAR     sName[MAX_NODENAME];       /* �ڵ�����     */
    CHAR     sComment[MAX_NODECOMMENT]; /* �ڵ�ע��     */
}_PACKED_1_ TYPE_TREE_NODE;

/* command attribution */
typedef struct tagTYPE_CMD_ATTR
{
    DWORD dExeID;           /* ����ִ��ID   */
    BYTE  bExeModeID;       /* ����ִ��ģʽ   */
    BYTE  bNextModeID;      /* ��һ��ִ��ģʽID */
    BYTE  bNextInputModeID; /* ��һ������ģʽID */
    BYTE  bExePos;          /* ����ִ��λ��   */
    BYTE  bAttrFlags;       /* �Ƿ��з�����:0x01, �Ƿ����ֹ:0x02, �Ƿ���Ҫ������Ϣ:0x04, �Ƿ���Է���˵�:0x08 */
    WORD16  wAllParaCount;    /* ��������(CMD + NO)*/
    DWORD dwParaPos;        /* �����б��λ�� */
    WORD16  wCmdParaCount;    /* �����������   */
    WORD16  wNoParaCount;     /* NO����������� */
    WORD16  wToShowLen;       /* ��ʾ��Ϣ��ʽ���� */
    DWORD dwToShowPos;      /* ��ʾ��Ϣ��λ�� */
}_PACKED_1_ TYPE_CMD_ATTR;

/* command parameter */
typedef struct tagTYPE_PARA
{
    BYTE    bParaType;    /* ��������   */
    WORD16    wLen;         /* ��������   */
    union
    {
        WORD64   qdwMinVal; /* WORD64����ֵ������ */
        float    fMinVal; /* float����ֵ������ */
    }Min;
    union 
    {
        WORD64   qdwMaxVal; /* WORD64����ֵ������ */
        float    fMaxVal; /* float����ֵ������ */
    }Max;
    CHAR    sName[MAX_PARANAME];       /* �������� */
    CHAR    sComment[MAX_PARACOMMENT]; /* �ڵ�ע�� */
}_PACKED_1_ TYPE_PARA;

typedef struct tagTYPE_PARA_OLD
{
    BYTE    bParaType;    /* ��������   */
    WORD16    wLen;         /* ��������   */
    union
    {
        WORD32   dwMinVal; /* WORD64����ֵ������ */
        float    fMinVal; /* float����ֵ������ */
    }Min;
    union 
    {
        WORD32   dwMaxVal; /* WORD64����ֵ������ */
        float    fMaxVal; /* float����ֵ������ */
    }Max;
    CHAR    sName[MAX_PARANAME];       /* �������� */
    CHAR    sComment[MAX_PARACOMMENT]; /* �ڵ�ע�� */
}_PACKED_1_ TYPE_PARA_OLD;

typedef struct tagTYPE_PRAMMAR
{
    BYTE     bPrammarType;   /* �﷨����   */
    BYTE     bItemCount;     /* Ԫ�ظ���   */
    BYTE     bMoreFlags;     /* PRAMMAR_MORE_ANGLE�﷨ѡ���־ */
    CHAR     sSymbol[MAX_PARASYMBOL];     /* ȱʡ��ʶ�� */
    CHAR     sComment[MAX_PARACOMMENT]; /* ע����Ϣ   */
}_PACKED_1_ TYPE_PRAMMAR;

#define STRUCTTYPE_PARA      1      /* para */
#define STRUCTTYPE_PRAMMAR   2      /* prammar */

typedef struct tagTYPE_CMD_PARA
{
    BYTE     bStructType;    /* �ṹ����   */
    BYTE     bIsNoCmdPara;   /* NO������� */
    WORD16     ParaNo;         /* �������   */
    union
    {
        TYPE_PRAMMAR  tPrammar;
        TYPE_PARA     tPara;
    }uContent;
}_PACKED_1_ TYPE_CMD_PARA;

typedef struct tagTYPE_CMD_PARA_OLD
{
    BYTE     bStructType;    /* �ṹ����   */
    BYTE     bIsNoCmdPara;   /* NO������� */
    WORD16     ParaNo;         /* �������   */
    union
    {
        TYPE_PRAMMAR  tPrammar;
        TYPE_PARA_OLD tPara;
    }uContent;
}_PACKED_1_ TYPE_CMD_PARA_OLD;

/* map */
/* type */
typedef struct tagTYPE_MAP_TYPE
{
    WORD16     wType;          /* ��������   */
    WORD16     wCount;         /* ƥ������� */
    DWORD    dwItemPos;      /* ƥ�����һ���λ�� */
}_PACKED_1_ TYPE_MAP_TYPE;

/* item */
typedef struct tagTYPE_MAP_ITEM
{ 
    WORD16     wValue;        /* ������ֵ   */
    DWORD    dwKeyPos;      /* �ַ���λ�� */
    DWORD    dwCommentPos;  /* �ַ���λ�� */
}_PACKED_1_ TYPE_MAP_ITEM;


/* ���ýű����� */
#define CFG_FILE_FORMAT         0x5A5A5A5A
#define CFG_FILE_VERSION        0x00010000

#define CFG_FILE_COMMENT_MARK               "#"
#define CFG_FILE_EQUAL_MARK                    " = "
#define CFG_FILE_COMMON_MARK                 "COMMON"
#define CFG_FILE_BANNER_NAME                   "BANNER"
#define CFG_FILE_PROMPT_NAME                   "PROMPT"
#define CFG_FILE_ENABLEPWD_NAME            "ENABLE_PASSWORD"
#define CFG_FILE_CONSOLE_ABS_TIMEOUT    "CONSOLE_ABS_TIMEOUT"
#define CFG_FILE_CONSOLE_IDLE_TIMEOUT   "CONSOLE_IDLE_TIMEOUT"
#define CFG_FILE_TELNET_ABS_TIMEOUT       "TELNET_ABS_TIMEOUT"
#define CFG_FILE_TELNET_IDLE_TIMEOUT     "TELNET_IDLE_TIMEOUT"

#define CFG_FILE_USER_MARK                       "USER"
#define CFG_FILE_USERNAME_NAME              "USERNAME"
#define CFG_FILE_PASSWORD_NAME             "PASSWORD"
#define CFG_FILE_PRIVILEGE_NAME              "PRIVILEGE"

/* file head */
typedef struct tagTYPE_CFG_FILE_HEAD
{
    DWORD    dFormatDef;     /* FormatDef == 0x5A5A5A5A,����Ƿ� */
    DWORD    dVersion;       /* ���ýű��ļ��汾   */
    DWORD    dCommonBegin;   /* ��������ƫ����   */
    DWORD    dUserBegin;     /* �û�����ƫ����   */
}_PACKED_1_ TYPE_CFG_FILE_HEAD;

typedef struct tagCommon_Info
{
    CHAR sBanner[MAX_BANNER_LEN];
    CHAR sPrompt[MAX_PROMPT_LEN];
    CHAR sEnablePassword[MAX_PASSWORD_LEN];
    WORD16 wConsoleAbsTimeout; 
    WORD16 wConsoleIdleTimeout; 
    WORD16 wTelnetAbsTimeout; 
    WORD16 wTelnetIdleTimeout; 
}_PACKED_1_ COMMON_INFO;

typedef struct tagUser_Info
{
    CHAR    sUsername[MAX_USERNAME_LEN];
    CHAR    sPassword[MAX_PASSWORD_LEN];
    BYTE    ucRight;
}_PACKED_1_ USER_INFO;

typedef struct tagUser_Node
{
    USER_INFO tUserInfo;
    struct tagUser_Node *pNext;
}_PACKED_1_ USER_NODE;

#ifdef WIN32
    #pragma pack()
#endif

#endif /* __OAM_SCRIPT_H__ */
