/*********************************************************************
* ��Ȩ���� (C)2002, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ� PUBTYPEDEF.H
* �ļ���ʶ��
* ����˵���� ͷ�ļ��͹��õ���������
* ��ǰ�汾�� V1.0
* ��    �ߣ� ��Ԫ�졪��3Gƽ̨
* ������ڣ�
*
* �޸ļ�¼1��
*    �޸����ڣ�2002��10��23��
*    �� �� �ţ�V1.0
*    �� �� �ˣ���Ԫ��
*    �޸����ݣ�����
* �޸ļ�¼2��
*    �޸����ڣ�2002��12��28��
*    �� �� �ţ�V1.0
*    �� �� �ˣ���Ԫ��
*    �޸����ݣ�������ϵͳ��ص�ͷ�ļ���Pub_TypeDef.h�ļ�������
               OSS_SubSys.h
**********************************************************************/
#ifndef _PUB_TYPEDEF_H_
#define _PUB_TYPEDEF_H_

/***********************************************************
                   ������������ѡ��
***********************************************************/
/***********************************************************
 *                        ��������                         *
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

#include <mqueue.h>
/* ********************************************************
                        ������������                        
********************************************************* */
typedef  unsigned char      BYTE;
typedef  signed char         SBYTE;
typedef  unsigned char      BOOLEAN;
typedef  char               CHAR;
typedef  void               *LPVOID;
typedef  void               VOID;
typedef  unsigned short     WORD16;
typedef  unsigned short     WORD;
typedef  signed short       SWORD16;
typedef  signed int         SWORD32;  
typedef  signed int        INT32;  
typedef  signed long long INT64;
typedef  unsigned int       WORD32; 
typedef  unsigned int       UINT32; 
typedef  unsigned long long WORD64;
typedef  char               BOOL;
typedef  unsigned int       DWORD;
typedef unsigned char    T_PAD;
typedef   int                    INT;
typedef  unsigned int       XOS_TIMER_ID;
typedef unsigned long int   WORDPTR; 
/* ֧�ָ����ӡ���� */
typedef double      DOUBLE;
typedef float       FLOAT; 
typedef long double LDOUBLE;

typedef int                 SOCKET;
typedef WORD32             XOS_TASK_T;  /**< ֧�Ŷ����TaskID�������� */
//typedef fd_set              SOCKET_SET;

#define OK		0
#define ERROR		(-1)
//typedef  int    XOS_FD;


typedef struct tagTIME
{
    WORD32  dwSecond;       /**< �� */
    WORD16  wMilliSec;      /**< ���� */
    BYTE    ucPad[2];
}T_Time; 

typedef struct tagJID
{
    WORD32  dwJno;          /**< JOB��, ��16λΪJOB ����, ��16λΪJOB ʵ���� */
    WORD32  dwDevId;        /**< �豸ID, �������ֵľֺ���Ϣ, ��8λΪ�ֺţ���֧�ִ���������Ϣ*/
    WORD16  wModule;        /**< MP��� */
    WORD16  wUnit;          /**< ����� */
    BYTE    ucSUnit;        /**< �����ϵĴ������� */
    BYTE    ucSubSystem;    /**< ��ϵͳ�� */
    BYTE    ucRouteType;    /**< ·�����ͣ�����ҵ��/������Ϣ�����Ұ�λ������ͨ��*/
    BYTE    ucExtendFlag; 	/**< ��չ��־ */
}JID;


typedef struct
{
    WORD           wReason;   /**<������̽�����μ�: ::_DB_E_SAVE_BOTHPATH, ::_DB_E_SAVE_MAINPATH, ::_DB_E_SAVE_BACKUP*/
    WORD           wSlaveRes; /**< ������̽�� */
    WORD32         dwReserve;  /**< Ԥ���ֶΣ����ֽڶ���*/
    CHAR           dbName[100]; /**< ���ݿ����� */
}_PACKED_1_  _db_t_save_db_result_item, *lp_db_t_save_db_result_item;

typedef struct
{
    WORD                         wDbNum;/**< ���δ��̵����ݿ�ʵ����Ŀ*/
    WORD                         wTranId;/**< ��ˮ��,ֻ�����򱸷���Ĵ��̹�����ʹ�� */
    WORD                        timeResult;  /**<���δ��̵Ľ�����μ�::_DB_TIME_SUCCESS,::_DB_TIME_ERROR,::_DB_TIME_BUSY*/
    WORD                            wReserve;  /**< Ԥ���ֶΣ����ֽڶ���*/
    _db_t_save_db_result_item    tDatabaseSaveResult[100];/**< �����ݿ�ľ�����̽�����μ�: ::_db_t_save_db_result_item */
}_PACKED_1_ _db_t_saveresult,*  lp_db_t_saveresult;
/***********************************************************
 *                       ȫ�ֺ�                            *
***********************************************************/
#define TRUE   1
#define FALSE 0

typedef unsigned int T_PORT;
#define IPADDR   DWORD

#ifndef IPV4ADDR_LEN
#define IPV4ADDR_LEN      4
#endif

#ifndef IPV6ADDR_LEN
#define IPV6ADDR_LEN      16
#endif

#ifndef IPADDR_LEN_MAX
#define IPADDR_LEN_MAX         (IPV6ADDR_LEN)
#endif

#define IPADDR_IPNONE               (BYTE)0
#define IPADDR_IPV4                    (BYTE)4
#define IPADDR_IPV6                    (BYTE)6

#undef	OAM_MIN
#define OAM_MIN(a, b)  (((a) < (b)) ? (a) : (b))

/* MAC��ַ���ȶ��� */
#define INTERNAL_HARD_ADDRESS_LENTH   6

#define MAX_OAMMSG_LEN               (1024 * 16)  /**< packet size */
#define MAX_APPMSG_LEN                (1024 * 16)  /*��Ӧ��֮�����Ϣ��󳤶�*/

/*���ؽṹ���忪ʼ*/
#define  MAX_JOB_NUM                     5
#define JOB_NAME_SIZE                   32

typedef VOID (* JCBEntry)(
                         WORD16,    /* JOB's state */
                         WORD32,    /* Current message's ID */
                         void *,    /* Current message body */
                         void *,    /* JOB's private data */
                         BOOLEAN    /*
                                     * message body's endian is the
                                     * same as my endian or not
                                              */
                               );

typedef struct tagCliJobCreatReg
{
    WORD16    wIndex;
    CHAR        acName[JOB_NAME_SIZE];    /**< JOB���ƣ�XOS_JOB_NAME_SIZE��ֵ����Ϊ32*/
    JCBEntry    pEntry;     /**< JOB���*/
    WORD32      dwStackSize;   /**<JOB��ջ��С,��С����Ϊ4096�ı���*/
    WORD32      dwDataRegionSize;  /**<JOB˽����������С,��С����Ϊ4096�ı���*/
} T_CliJobCreatReg;

typedef struct tagCliJobInfo
{
    WORD32      dwJno;
    WORD16      wState;
    CHAR          acMqueueName[JOB_NAME_SIZE + 10];
    mqd_t          mq_Recv;    /*�������ݵ���Ϣ���У�������job������Ϣ��*/
    WORD32      dwCurMsgLen;
    WORD32      dwStackSize;
    JCBEntry      pEntry;
    XOS_TASK_T dwTaskId;
    LPVOID        pMsgRecvBuff;
    LPVOID        pDataRegion;
 } T_CliJobInfo;

typedef struct tagCliMsg
{
    WORD32 dwMsgId;
    WORD16 wDataLen;
    BYTE      aucMsg[MAX_OAMMSG_LEN];
}T_CliMsg;
/*���ؽṹ�������*/
/***********************************************************
 *                     ȫ�ֺ���ԭ��                        *
***********************************************************/


#endif /* end of _PUB_TYPEDEF_H_ */

