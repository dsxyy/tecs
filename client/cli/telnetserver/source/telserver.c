/**************************************************************************
* ��Ȩ���� (C)2007, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ� telnetserver.c
* �ļ���ʶ��
* ����ժҪ��
* ����˵���� ��
* ��ǰ�汾�� V1.0
* ��    �ߣ� 3Gƽ̨
* ������ڣ�
*
* �޸ļ�¼1��
*    �޸����ڣ�2007��6��26��
*    �� �� �ţ�V1.0
*    �� �� �ˣ�
*    �޸����ݣ�����
**************************************************************************/

/**************************************************************************
*                           ͷ�ļ�                                        *
**************************************************************************/

#include "pub_addition.h"

#include "oam_cfg_common.h"
#include "telserver.h"
#include "basesock.h"
#include "rdtelnet.h"
#include "oam_fd.h"
#include "clivty.h"
/*#include "sockets.h"*/
/**************************************************************************
*                          ����                                           *
**************************************************************************/
/**************************************************************************
*                          ��                                             *
**************************************************************************/
/**************************************************************************
*                          ��������                                       *
**************************************************************************/
/**************************************************************************
*                          ȫ�ֱ���                                       *
**************************************************************************/

/**************************************************************************
*                          ���ر���                                       *
**************************************************************************/

/**************************************************************************
*                          �ֲ�����ԭ��                                   *
**************************************************************************/
static void OamTelSvrEntryCheckPtr(BYTE *InputPtr);
static WORD32 Initialize(void);
/**************************************************************************
*                          ȫ�ֺ���ʵ��                                   *
**************************************************************************/
extern int  oss_telnetdInit(void);
/**************************************************************************
*                          ����ں���                                     *
**************************************************************************/
void Oam_TelnetServer(WORD16 wState, WORD32 dwMsgId, LPVOID pMsgPara, LPVOID pVar, BOOLEAN byteOrder/*~*/)
{
    OamTelSvrEntryCheckPtr((BYTE*)pMsgPara);
/*
    XOS_SysLog(OAM_CFG_LOG_DEBUG, 
                       "[Oam_TelnetServer]=======s/m = %d telnetserver wstate=%d msgid=%d==========\n", 
                        XOS_GetBoardMSState(),wState,dwMsgId);
*/
    switch (wState)
    {
    case S_Init:
        switch(dwMsgId)
        {
        /*case EV_STARTUP:*/
        case EV_SLAVE_POWER_ON:
        case EV_MASTER_POWER_ON:
            /*��ʼ��һЩȫ�ֱ���*/
            if(Initialize() != OAM_OK)
            {
                return;
            }
            #if 0
            /* ����Telnet֡��*/
            if (CreateListenSocket(SOCKET_PORT_TELNET) != SUCCESS)
            {
                return;
            }
            #endif
            XOS_SetNextState(PROCTYPE_OAM_TELNETSERVER, S_Work);
            break;
        default:
            break;
        }
        break;
    case S_Work:
        switch(dwMsgId)
        {
        #if 0
        case MSG_BRSSOCKET_MSG:/*ҵ������socket��Ϣ*/
            ProcessSocketCommand(pMsgPara);
            break;
	#endif
        case MSG_FD_CONNECT:/*���������½���������*/
            /*OSS_Printf(PRINT_OAM_TELSVR, PRN_NORMAL, PRN_LEVEL_HIGHEST,
                       "OamTelnetServer: Receive MSG_FD_CONNECT ;CurrentState=%d; line=%d\n",wState,__LINE__);*/
            ProcessOssConnect(pMsgPara);
            break;
        case MSG_FD_RECEIVE:/*�������ڡ����ڷ�������*/
            ProcessOSSRecive(pMsgPara);
            break;
        case MSG_FD_CLOSE:/*�������ڶϿ���������*/
            ProcessOSSClose(pMsgPara);
            break;
        case OAM_MSG_INTERPRET_TO_TELNET:/*���ͳ��򷵻�����*/
            Oam_ProcMsgFromInterpret((BYTE *)pMsgPara);
            break;
        case EV_TIMER_TELSVR:/*���ͷ��ص���Ϣ��ʱ*/
            Oam_ProcTimerEventFromInterpret(XOS_GetConstParaOfCurTimer(pMsgPara));
            break;

            /* ������RPU����·�ϵ� */
            /* ������RPU��·�� */
        case EV_COMM_ABORT:
            /*case OAM_OMP_SLAVE_TO_MASTER:*/
            /*�ر�����socket*/
            Oam_CloseAllVty("\n\r%Communication with RPU losted! \n");
            XOS_SetNextState(PROCTYPE_OAM_TELNETSERVER, S_CommAbort);
            break;

            /* ������RPU��·�ָ� */
        case EV_COMM_OK:
            break;

        case EV_MASTER_TO_SLAVE:
        case EV_SLAVE_TO_MASTER:
            Oam_CloseAllVty("\n\r%The board was changed over! \n");
            break;
        case OAM_MSG_TO_TELSERVER_OF_CEASE_ATTR:
            Oam_ProcCeaseMsgFromInterpret((BYTE *)pMsgPara);
            break;
        case OAM_MSG_SSH_AUTH: /*  SSH�������û���¼��֤ */
            Oam_ProcSSHAuth(pMsgPara);
            break;
        /*���ͻ����ط���ʱ��*/
        case EV_TIMER_CHECK_SENDBUF:
            Oam_SendBufData();
            break;
        default:
            break;
        }
        break;
    case S_CommAbort:
        switch(dwMsgId)
        {/* ������RPU��·�ָ� */
        case EV_COMM_OK:
            XOS_SetNextState(PROCTYPE_OAM_TELNETSERVER, S_Work);
            break;
        case MSG_FD_CONNECT:/*���������½���������*/
        case MSG_FD_RECEIVE:/*�������ڡ����ڷ�������*/
            Oam_CliVtyCommAbort(pMsgPara);
            break;
        /*~*/
#if 0
        case MSG_BRSSOCKET_MSG:/*ҵ������socket��Ϣ*/
            {
                BRS_SOCKET_IO_MSG *pSockMsg = pMsgPara;
                CHAR *strTmp = OAM_TELSVR_COMM_LOST;
                brs_send(pSockMsg->s, strTmp, strlen(strTmp), 0);
            }
            break;
#endif
        default:
            break;
        }
        break;
    default:
        break;
    }
    XOS_SetDefaultNextState();
}

/**************************************************************************
*                     �ֲ�����ʵ��                                        *
**************************************************************************/
/**************************************************************************
* �������ƣ�VOID OamTelSvrEntryCheckPtr
* ������������麯��ָ�������
* ���ʵı���
* �޸ĵı���
* ���������
    BYTE *InputPtr
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/6/12    V1.0                 ����
**************************************************************************/
void OamTelSvrEntryCheckPtr(BYTE *InputPtr)
{
    assert(InputPtr != NULL);
    if (NULL == InputPtr)
    {
        /*OSS_Printf(PRINT_OAM_TELSVR, PRN_NORMAL, PRN_LEVEL_HIGHEST,
                   "OamTelnetServer: InputPtr == NULL; line=%d\n",__LINE__);
        OSS_ExcOutput(EXCCODE_OAM_TELSVR_NULL_PTR,
                      "OamTelnetServer:OamTelSvrEntryCheckPtr(): NULL ptr; line=%d!",__LINE__);*/

        /* ������Ϊ�գ�ֱ���˳� */
        XOS_SetDefaultNextState();
    }
}

/**************************************************************************
* �������ƣ�Initialize
* ������������ʼ��
* ���ʵı���
* �޸ĵı���
* ���������
*
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ���      ����
**************************************************************************/
WORD32 Initialize(void)
{
    /*��ʼ��VTY�����ȫ�ֱ���*/
    Oam_CliVtyInit();
    XOS_SysLog(OAM_CFG_LOG_NOTICE, "Begin to initial socket ...");
    /*��ʼ��socket*/
    oss_telnetdInit();

  //  OamRegConsoleCallback();

    /*ע��ص�����*/
    return OamRegCallback();
}

void SendNegoEcho(OAM_FD_MSG *pMsg, CHAR cNegoEcho1, CHAR cNegoEcho2)
{
    CHAR aucNegoEcho[3] = {0};
	
    aucNegoEcho[0] = (CHAR)OAM_CTL_IAC;
    aucNegoEcho[1] = cNegoEcho1;
    aucNegoEcho[2] = cNegoEcho2;

    OAM_FD_Send(pMsg->fd, pMsg->flag,  aucNegoEcho, sizeof(aucNegoEcho));
}

void ProcessOssConnect(LPVOID pMsgPara)
    { 
    OAM_FD_MSG *pMsg = (OAM_FD_MSG *)pMsgPara;
    TELNETSVR_VOID_ASSERT(pMsgPara != NULL);
		
    /*֪ͨVTY,�����½���������*/
    Oam_CliVtyConnect( pMsgPara);
	
    /*��������Э������*/
    if((CONNECTION_FROM_TELNET == pMsg->flag))
            {
        SendNegoEcho(pMsg, (CHAR)OAM_CTL_WILL,(CHAR) OAM_OPT_ECHO);
        SendNegoEcho(pMsg, (CHAR)OAM_CTL_DONT, (CHAR)OAM_OPT_ECHO);
        SendNegoEcho(pMsg, (CHAR)OAM_CTL_WILL, (CHAR)OAM_OPT_SUPPRESS_GO_AHEAD);
        SendNegoEcho(pMsg, (CHAR)OAM_CTL_DONT, (CHAR)OAM_OPT_SUPPRESS_GO_AHEAD);
            }
			
    return ;
                }

void ProcessOSSRecive(LPVOID pMsgPara)
                {
   /*
      �յ��û���Ϣ������vty���з�����
      ��������Ľ�����͵������server����server���͸��ͻ���
    */
    Oam_CliVtyRecive(pMsgPara);
                }

void ProcessOSSClose(LPVOID pMsgPara)
            {
    TELNETSVR_VOID_ASSERT(pMsgPara != NULL);
    /*֪ͨvty���ر�vty���������*/
    Oam_CliVtyClose(pMsgPara);	
    return ;
}


