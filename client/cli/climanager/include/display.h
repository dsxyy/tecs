/*********************************************************************
* ��Ȩ���� (C)2002, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ�Display.h
* �ļ���ʶ��
* ����ժҪ�����Խ�����غ���
* ����˵����
            
* ��ǰ�汾��
* ��    �ߣ����� ��ֲ����
* ������ڣ�2007.6.5
*
* �޸ļ�¼1��// �޸���ʷ��¼�������޸����ڡ��޸��߼��޸�����
*    �޸����ڣ�
*    �� �� �ţ�
*    �� �� �ˣ�
*    �޸����ݣ�
* �޸ļ�¼2��
**********************************************************************/
#ifndef __DISPLAY_H__
#define __DISPLAY_H__

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

#include "oam_cfg.h"
#include "pub_oam_cfg_event.h"

/* �ɹ� */
#define DISPLAY_SUCCESS               0
                                      
/* ���Բ��ַ���ֵ�Ķ��� */            
#define ERROR_INDIS                   2
#define INVALID_OPRMODE               3
#define SUCCESS_END                   9
#define SUCCESS_ENDWITHSWITCHSTART    10  /* ����%hs */
#define SUCCESS_ENDWITHSWITCHCASE     11  /* ����%hc */
#define SUCCESS_ENDWITHSWITCHEND      12  /* ����%he */
#define SUCCESS_ENDWITHFORSTART       13  /* ����%rs */
#define SUCCESS_ENDWITHFOREND         14  /* ����%re */
#define SUCCESS_ENDWITHSWITCH         15  /* ����%r */
#define SUCCESS_ENDWITHFOR            16  /* ����%h */
#define SUCCESS_ENDWITHTABLE          17  /* ����%t */
#define SUCCESS_CONTINUETONEXTPACKET  20  /* ����δ�������������һ��ָ�� */

#define MAX_PARA_NUMBER               2001 /* ����ܹ���ʾ�Ĳ������� */
#define DISPLAY_BUFFER_SIZE          MAX_OUTPUT_RESULT /* 1025 * 50 *//* 50k size */
#define MAX_DISPLAY_STRLEN            1025      /* 1k size */
#define MAX_DISPLAY_WIDTH             80

#define MAX_STACK_DEPTH               10
#define TEXT_MARK                     34

/* ������ʾ����¼ */
/* ���Բ���error code���䶨�� */
#define UNKNOW_ERROR_IN_DISPLAY       1   /*unknow error in display*/
#define INSUFFICIENT_MEMORY           2   /*insufficient memory*/
#define BUFFER_OVERFLOW               3   /*buffer overflow*/
#define INVALID_TYPE                  4   /*invalid type*/
#define INVALID_FIELD_NUMBERS         5   /*invalid field_numbers*/
#define INVALID_FOOTSCRIPT            6   /*invalid footscript*/
#define INVALID_STRING                7   /*invalid string*/
#define WRONG_RETURN_VALUE            8   /*wrong return value*/
#define FOOTSCRIPT_DATA_MISMATCH      9   /*footScript data mismatch*/
#define INVALID_POINTER               10  /*invalid pointer*/
#define INVALID_STATE                 11  /*invalid state*/
#define INVALID_DATALEN               12  /*invalid datalen*/
#define DATA_LENGTH_ERROR             15  /*data length error*/
#define CMD_ROUTER_BGP						0x0c01000b
#define MODE_PROTOCOL	(BYTE)2

/* ������� */
typedef struct
{
    CHAR sErrorString[50];
    BYTE bErrorLevel;
}_PACKED_1_ ERROR_INFO;

/* DBG_ERR_LOG���ڼ�������������ʱ�Ĵ��� */
/*#define DBG_ERR_LOG(ErrorCode)\
    if (sErrorTable[ErrorCode].bErrorLevel > 0)\
    {\
        OSS_Printf(PRINT_OAM_PLATCFGAGT, PRN_NORMAL, PRN_LEVEL_HIGHEST,sErrorTable[ErrorCode].sErrorString);\
        OSS_Printf(PRINT_OAM_PLATCFGAGT, PRN_NORMAL, PRN_LEVEL_HIGHEST,"\nin %s, %d\n",__FILE__ , __LINE__);\
    }*/
#define DBG_ERR_LOG(ErrorCode)\
    if (sErrorTable[ErrorCode].bErrorLevel > 0)\
    {\
        XOS_SysLog(OAM_CFG_LOG_ERROR, "%s\n", sErrorTable[ErrorCode].sErrorString); \
        XOS_SysLog(OAM_CFG_LOG_ERROR, "\nin %s, %d\n",__FILE__ , __LINE__); \
    }
    
/* ASSERT���ڼ�Ȿ���ݳ����������ʱ���ܳ��ֵĴ��� */
/*#define DISPLAY_ASSERT(test)    \
if (!(test))\
{\
    OSS_Printf(PRINT_OAM_PLATCFGAGT, PRN_NORMAL, PRN_LEVEL_HIGHEST,"\n!!Assertion failed: file %s,line %d!!\n", __FILE__ , __LINE__);\
    return ERROR_INDIS;\
}*/
#define DISPLAY_ASSERT(test)    \
if (!(test))\
{\
    return ERROR_INDIS;\
}

typedef struct 
{
    CHAR    *pDisplayBuffer;    /* a pointer to the display buffer */
    CHAR    *pDispBuff;         /* �������е�ǰ��λ�� */
    CHAR    *pFootScript;       /* ʹ�õĽű�ָ�� */
    
    /* ��������ʼλ�ñ� */
    CHAR    *pParameterStartAddress[MAX_PARA_NUMBER];
    /* ����λ�õ�Ĭ��ֵ */
    int     giParameterLocation;
    DWORD   dwNumber;
    
    /* ���浱ǰ�������һ��paraNoֵ */
    WORD16    ParaNo;
    
    /* �����˳�ʱ�ű���λ��,������LastPackʱ��Ӧ����ָ����� */
    CHAR    *spFootScript;
    
    struct 
    {
        WORD16    wForCounter;        /* ѭ���Ĵ��� */
        CHAR    *pFootScript;       /* ѭ��ʱ��ָ��ѭ���ű���ʼ��λ�� */
    }_PACKED_1_ sForStack[MAX_STACK_DEPTH];    /* for���Ŀ���ջ */
    BYTE     bForStackTop;          /* for���Ŀ���ջ�� */
    
    struct
    {
        WORD16    wParaValue;
    }_PACKED_1_ sSwitchStack[MAX_STACK_DEPTH]; /* switch���Ŀ���ջ */
    BYTE    bSwitchStackTop;        /* switch���Ŀ���ջ�� */
    
}_PACKED_1_ TYPE_DISP_GLOBAL_PARA;

typedef struct 
{
    CHAR    *pDisplayBuffer;        /* a pointer to the display buffer */
    CHAR    *pDispBuff;             /* �������е�ǰ��λ�� */
    CHAR    *pFootScript;           /* ʹ�õĽű�ָ�� */
    
    /* ��������ʼλ�ñ� */
    CHAR    *pParameterStartAddress[MAX_PARA_NUMBER];
    
    /* ����λ�õ�Ĭ��ֵ */
    int     giParameterLocation;
    
    struct
    {
        WORD16    wParaValue;
    }_PACKED_1_ sSwitchStack[MAX_STACK_DEPTH]; /* switch���Ŀ���ջ */
    BYTE    bSwitchStackTop;        /* switch���Ŀ���ջ�� */
    
    BYTE    bTableType;             /* ������ͣ�1 table 2 list */
    TABLE_DATA TableData;
}_PACKED_1_ TYPE_DISP_GLOBAL_PARA_ITM;


/* ���Ⱪ¶�ӿ� */
struct tag_TYPE_LINK_STATE;
INT32 OamCfgintpSendToBuffer(struct tag_TYPE_LINK_STATE *pLinkState,
                            MSG_COMM_OAM *ptMsg,           /* �����ռ���ڵ�ַ */
                            CHAR *pFootScriptEx,           /* �ű������λ�� */
                            TYPE_DISP_GLOBAL_PARA *pGlobal /* ��������ȫ�ֱ�����ָ�� */
                            );

#ifdef WIN32
    #pragma pack()
#endif

#endif /* __DISPLAY_H__ */
