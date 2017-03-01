/*********************************************************************
* ��Ȩ���� (C)2002, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ�telnetserver.h
* �ļ���ʶ��
* ����ժҪ��OAM telnetserver������ڣ���Ϣ����������
* ����˵����
            
* ��ǰ�汾��
* ��    �ߣ�
* ������ڣ�2007.6.26
*
* �޸ļ�¼1��// �޸���ʷ��¼�������޸����ڡ��޸��߼��޸�����
*    �޸����ڣ�
*    �� �� �ţ�
*    �� �� �ˣ�
*    �޸����ݣ�
* �޸ļ�¼2����
**********************************************************************/
#ifndef __OAM_TELNETSERVER_H__
#define __OAM_TELNETSERVER_H__

#include <assert.h>
#include "oam_cfg_common.h"
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

#define S_Init                    (WORD16)0
#define S_Work                    (WORD16)1
#define S_CommAbort               (WORD16)2

#define OAM_TELSVR_COMM_LOST      "\n\rCommunication with RPU losted, can not connect now.\n\r"
#define OAM_TELSVR_SHELL_CLOSE    "\n\rOam shell closed!\n\r"
#define OAM_TELSVR_PARSE_TIMEOUT  "\n\rRemote parse timeout,please check whether Interpret instance is working and then retry. \n\r"

#define MSG_HDR_SIZE_IN            sizeof(TYPE_INTERPRET_MSGHEAD)
#define MAX_REMOTE_PARSE_CONN    MAX_STY_NUMBER
#define MAX_RESULT_DATA_LEN         MAX_OAMMSG_LEN /*??��������ַ�������󳤶�*/
#define MAX_RESULT_TO_SSH_LEN       MAX_OAMMSG_LEN + 128  /*���͸�ssh����󳤶ȣ���4000����������128*/

#define TIMER1                     (TIMER_NO_1)
#define EV_TIMER_TELSVR            (EV_TIMER_1)
#define TIMER_WAIT_INTERPRET_ACK   TIMER1
#define MAX_WAIT_ACK_LEN           1000*60*60
#define MIN_WAIT_ACK_LEN           1000*5

/* ���ͻ��������� */
#define TIMER_CHECK_SENDBUF             (TIMER_NO_2)
#define EV_TIMER_CHECK_SENDBUF          (EV_TIMER_2)
#define DURATION_CHECK_SENDBUF          (SWORD32)(100)

/*telnetЭ��Э����*/
#define OAM_CTL_IAC                 (unsigned char)255
#define OAM_CTL_DONT                (unsigned char)254
#define OAM_CTL_DO                  (unsigned char)253
#define OAM_CTL_WONT                (unsigned char)252
#define OAM_CTL_WILL                (unsigned char)251
#define OAM_OPT_ECHO                (unsigned char)1 /* Echo over TELNET connection */
#define OAM_OPT_SUPPRESS_GO_AHEAD   (unsigned char)3  /* Suppress Go Ahead controls */

#define MAX_WHILE_ROUNDS          5000 /*�������while��ѭ��*/
#define WHILE g_wWhileLoopCnt = 0;\
              while
#define LOOP_CHECK()   {\
                           if (g_wWhileLoopCnt++ > MAX_WHILE_ROUNDS)\
                           {\
                               break;\
                           }\
                        }

/*#define TELNETSVR_VOID_ASSERT(CONDITION)  \
    XOS_ASSERT((CONDITION)); \
    if (!((CONDITION)))\
    {\
        OSS_Printf(PRINT_OAM_TELSVR, PRN_NORMAL, PRN_LEVEL_HIGHEST,\
                   "OamTelnetServer: %s; line=%d\n","\
                   (CONDITION)\
                   ",__LINE__);\
        return;\
    }*/
#define TELNETSVR_VOID_ASSERT(CONDITION)  \
    assert((CONDITION)); \
    if (!((CONDITION)))\
    {\
        return;\
    }

/*#define TELNETSVR_0_ASSERT(CONDITION)  \
    XOS_ASSERT((CONDITION)); \
    if (!((CONDITION)))\
    {\
        OSS_Printf(PRINT_OAM_TELSVR, PRN_NORMAL, PRN_LEVEL_HIGHEST,\
                   "OamTelnetServer: %s; line=%d\n","\
                   (CONDITION)\
                   ",__LINE__);\
        return 0;\
    }*/
#define TELNETSVR_0_ASSERT(CONDITION)  \
    assert((CONDITION)); \
    if (!((CONDITION)))\
    {\
        return 0;\
    }
	
/* define the structures */

/* �������ṹ�� */
/*
typedef struct tagT_OAM_TELNETSERVER_VAR
{
    JID    jidSelf;  
}_PACKED_1_ T_OAM_TELNETSERVER_VAR;
*/

/*����OSS ��Ϣ�ĺ���*/
void ProcessOssConnect(LPVOID pMsgPara);
void ProcessOSSRecive (LPVOID pMsgPara);
void ProcessOSSClose(LPVOID pMsgPara);

#ifdef WIN32
    #pragma pack()
#endif


#endif /*__OAM_TELNETSERVER_H__*/

