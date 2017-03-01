/**************************************************************************
* ��Ȩ���� (C)2002, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ�CliVty.c
* �ļ���ʶ��
* ����ժҪ�������ڲ���Ϣ
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
**************************************************************************/
/**************************************************************************
*                           ͷ�ļ�                                        *
**************************************************************************/
#include <stdio.h>
#include "telserver.h"
#include "basesock.h"
#include "rdtelnet.h"
#include "oam_fd.h"
#include "clivty.h"
#include "pub_addition.h"
#include "netinet/in.h"
#include   <arpa/inet.h>
/*******************************************************************
*                      ��������                                        *
********************************************************************/
extern void oam_sty_puts(sty *cty, CHAR *text);
/**************************************************************************
*                          ȫ�ֱ���                                       *
**************************************************************************/
static CHAR result_data_buffer[MAX_REMOTE_PARSE_CONN][MAX_RESULT_DATA_LEN];

TYPE_FD *gtSocketList , *gtSocketTail;
sty gtSty[MAX_STY_NUMBER];
sty gtExtraSty[MAX_STY_NUMBER];
JID g_tCfgAgentJid={0};
JID g_tSSHServerJid={0};

extern WORD16 g_wWhileLoopCnt;
/**************************************************************************
*                          ȫ�ֺ���                                     *
**************************************************************************/
static int CloseSocket(TYPE_FD *tSocket);
/**************************************************************************
* �������ƣ�Oam_CliVtyInit
* ������������ʼ��Vtyȫ�ֱ���
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
void Oam_CliVtyInit()
{
    gtSocketList = NULL;
    gtSocketTail = NULL;
    /*��ʼ��gtSty*/
    memset(&gtSty, 0, sizeof(gtSty));
    memset(&gtExtraSty, 0, sizeof(gtExtraSty));
    memset(&g_tCfgAgentJid, 0, sizeof(g_tCfgAgentJid));
    XOS_GetSelfJID(&g_tCfgAgentJid);
    g_tCfgAgentJid.dwJno = XOS_ConstructJNO(PROCTYPE_OAM_CLIMANAGER, 1);    

    memset(&g_tSSHServerJid, 0, sizeof(g_tSSHServerJid));
    XOS_GetSelfJID(&g_tSSHServerJid);
    g_tSSHServerJid.dwJno = XOS_ConstructJNO(PROCTYPE_OAM_SSHSERVER, 1);
#if 0
    /* ע����RPU�������ָ���·����Ϣ */
    T_LogicalAddr     tLogicalAddr;	
    memset(&tLogicalAddr, 0, sizeof(tLogicalAddr));
    tLogicalAddr.wModule	  = 2;
    tLogicalAddr.wUnit 	  = INVALID_UNIT;
    tLogicalAddr.ucSubSystem = INVALID_SUBSYS;
    tLogicalAddr.ucSUnit	  = INVALID_SUNIT;
    /*~*/

    SCS_RegCommLinkLogicAddress(&tLogicalAddr);
#endif
    return;
}
/**************************************************************************
* �������ƣ�Oam_CliVtyConnect
* �����������½����������VTY������·��Ϣ��֪ͨ
                              ���ͳ���
* ���ʵı���
* �޸ĵı���
* ���������
*pMsgPara         �����ӵ���·��Ϣ
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ���      ����
**************************************************************************/
void Oam_CliVtyConnect(LPVOID pMsgPara)
{
    OAM_FD_MSG *pMsg = (OAM_FD_MSG*)pMsgPara;
    TYPE_FD *tSocket = NULL;
    sty    *sty = NULL;
    BYTE bTmp = 0;
    struct sockaddr_in clientAddr;

    TELNETSVR_VOID_ASSERT(pMsgPara != NULL);

    tSocket = (TYPE_FD *)XOS_GetUB(sizeof(TYPE_FD));
    if (!tSocket)
    {
        /*OSS_Printf(PRINT_OAM_TELSVR, PRN_NORMAL, PRN_LEVEL_HIGHEST,
                   "ProcessOssConnect: GetUB fail! file=%d; line=%d\n",__FILE__,__LINE__);*/
        return ;
    }

    tSocket->wPort            = 0;

    if(CONNECTION_FROM_SERIAL == pMsg->flag)
    {/*����*/
        /*OSS_Printf(PRINT_OAM_TELSVR, PRN_NORMAL, PRN_LEVEL_HIGHEST,
                   "ProcessOssConnect:Receive From SERIAL CONNECTION. file=%d; line=%d\n",__FILE__,__LINE__);*/
        tSocket->ucFDType = FD_TYPE_SERIAL;
    }
    else if(CONNECTION_FROM_TELNET == pMsg->flag)
    {/*��������*/
        /*OSS_Printf(PRINT_OAM_TELSVR, PRN_NORMAL, PRN_LEVEL_HIGHEST,
                   "ProcessOssConnect:Receive From TELNET CONNECTION. file=%d; line=%d\n",__FILE__,__LINE__);*/
        tSocket->ucFDType = FD_TYPE_OSSTELNET;
    }
    else if(CONNECTION_FROM_SSH == pMsg->flag)
    {/*��������SSH*/
        tSocket->ucFDType = FD_TYPE_OSSSSH;
    }
    else
    {/*��������*/
        /*OSS_Printf(PRINT_OAM_TELSVR, PRN_NORMAL, PRN_LEVEL_HIGHEST,
                   "ProcessOssConnect:Receive From UNKNOW CONNECTION,so Terminate the Connection. file=%d; line=%d\n",__FILE__,__LINE__);*/
        tSocket->ucFDType = FD_TYPE_OSSTELNET;
        OAM_RETUB(tSocket);
        return ;
    }

    tSocket->ucFd               = pMsg->fd;
    memcpy(&clientAddr, pMsg->buf, sizeof(clientAddr));
    tSocket->wPort            = clientAddr.sin_port;
    if (CONNECTION_FROM_SERIAL == pMsg->flag)
        strncpy(tSocket->szIpAddr, "255.255.255.255", sizeof(tSocket->szIpAddr));
    else        
    memcpy(tSocket->szIpAddr, (CHAR *)(inet_ntoa(clientAddr.sin_addr)), LEN_IP_ADDR);
    
    tSocket->next            = NULL;

    bTmp=AddSty(tSocket);
    if (bTmp != ADD_STY_SUCCESS)
    {
        CHAR *strTmp = "\nUnknown error while add sty!\n";
        switch(bTmp)
        {
        case ADD_STY_CONNECTIONFULL:   /*����������*/
            /*OSS_Printf(PRINT_OAM_TELSVR, PRN_NORMAL, PRN_LEVEL_HIGHEST,
                       "ProcessOssConnect:Connection is full!. file=%s; line=%d\n",__FILE__,__LINE__);*/
            strTmp = "\nConnection is full!\n";
            break;
        case ADD_STY_INTERPRETSTARTFAIL:   /*����ʵ������ʧ��*/
            /*OSS_Printf(PRINT_OAM_TELSVR, PRN_NORMAL, PRN_LEVEL_HIGHEST,
                       "ProcessOssConnect:Start Interpret instance fail!. file=%s; line=%d\n",__FILE__,__LINE__);*/
            strTmp = "\nStart Interpret instance fail!\n";
            break;
        case ADD_STY_SLAVEBOARD:   /*��ǰ�����Ǳ���*/
            strTmp = "\nCan not connect to telnet server on slave board!\n";
        default:
            break;
        }

        if (CONNECTION_FROM_SSH == pMsg->flag)
            oam_fd_send_for_ssh(pMsg->fd, strTmp);
        else
            OAM_FD_Send(pMsg->fd, pMsg->flag, strTmp, strlen(strTmp));
        
        if(FD_TYPE_OSSTELNET == tSocket->ucFDType)
        {
            OAM_FD_Quit((WORD16)(tSocket->ucFd), CONNECTION_FROM_TELNET);
        }
        else if (FD_TYPE_OSSSSH == tSocket->ucFDType)
        {
            oam_fd_quit_for_ssh(pMsg->fd);
        }
        
        OAM_RETUB(tSocket);
        return ;
    }

    /* ����Socket�б� */
    if (gtSocketList == NULL)
    {
        gtSocketList = gtSocketTail = tSocket;
    }
    else
    {
        gtSocketTail->next = tSocket;
        gtSocketTail = tSocket;
    }

    sty = FindSty(tSocket);
    if(sty)
    {
        if((FD_TYPE_OSSTELNET == sty->tSocket->ucFDType) ||
           (FD_TYPE_OSSSSH == sty->tSocket->ucFDType) ||
           (FD_TYPE_SERIAL == sty->tSocket->ucFDType))
        {
            /*�����½�������Ϣ*/
            if (FD_TYPE_OSSSSH == sty->tSocket->ucFDType)
                sty->sshconnmsg = TRUE;
            
            make_conn_to_remote_parse(sty);
        }
        else
        {
            /*OAM_FD_Send(pMsg->fd, pMsg->flag,  "SSH-2.0-ZTE_SSH.1.0\r\n", strlen("SSH-2.0-ZTE_SSH.1.0\r\n"));*/

        }
    }

    return ;
}
/**************************************************************************
* �������ƣ�Oam_CliVtyRecive
* �����������յ��û���������ݺ�vty�����ݽ��з���
* ���ʵı���
* �޸ĵı���
* ���������
*pMsgPara         ���ͻ����������Ϣ
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ���      ����
**************************************************************************/
void Oam_CliVtyRecive(LPVOID pMsgPara)
{
    OAM_FD_MSG *pMsg = (OAM_FD_MSG *)pMsgPara;
    TYPE_FD *tSocket = gtSocketList;

    TELNETSVR_VOID_ASSERT(pMsgPara != NULL);

    /*����Ǵ����նˣ��������յ��ĵ�һ���س�*/
    if((CONNECTION_FROM_SERIAL  == pMsg->flag) && (NULL == gtSty[0].tSocket))
    {
        if(pMsg->buf && (('\n' == pMsg->buf[0]) ||('\r' == pMsg->buf[0]) ))
        {
            ProcessOssConnect(pMsgPara);
            return;
        }
    }

    /*��ȫ�������ȡ�յ���Ϣ��socket�ṹ*/
    WHILE (tSocket)
    {
        LOOP_CHECK();
        if (tSocket->ucFd == pMsg->fd)/*?? ��OSS ��BRS����socket�������Ƿ�������?*/
        {
            break;
        }
        tSocket = tSocket->next;
    }
    if (!tSocket)
    {
        return;
    }
    if((FD_TYPE_OSSTELNET == tSocket->ucFDType) ||
        (FD_TYPE_OSSSSH == tSocket->ucFDType) ||
        (FD_TYPE_SERIAL == tSocket->ucFDType))
        ReadTelnet(tSocket, (CHAR *)pMsg->buf, pMsg->len);
    else
        ;
}
/**************************************************************************
* �������ƣ�Oam_CliVtyClose
* �����������յ����ӹر���Ϣ�󣬹ر�vty����
* ���ʵı���
* �޸ĵı���
* ���������
*pMsgPara         �����ӵ���·��Ϣ
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ���      ����
**************************************************************************/
void Oam_CliVtyClose(LPVOID pMsgPara)
{
   OAM_FD_MSG *pMsg = (OAM_FD_MSG*)pMsgPara;
    TYPE_FD *tList = gtSocketList, *tPrev = NULL;
    sty *cty = NULL;
    CHAR aucClient[50] = {0};

    TELNETSVR_VOID_ASSERT(pMsgPara != NULL);

    XOS_SysLog(OAM_CFG_LOG_NOTICE, "611000956402:ProcessOSSClose called\n");
    /*��ȫ�������ȡ�յ���Ϣ��socket�ṹ*/
    while (tList)
    {
        if (tList->ucFd == pMsg->fd)
        {
            break;
        }
        tPrev = tList;
        tList = tList->next;
    }
    if(!tList)
    {/*���û���ҵ�*/
        return ;
    }

    XOS_snprintf(aucClient, sizeof(aucClient), "%s:%u", tList->szIpAddr, tList->wPort);
    XOS_SysLog(OAM_CFG_LOG_NOTICE, "611000956402:ProcessOSSClose:: client is %s\n", aucClient);
	
    cty = FindSty(tList);
    if(cty)
    {
        remote_parse_cookie *rpc = NULL;
        cty->sshauthmsg = FALSE;
        terminate_conn_to_remote_parse(cty);
        oam_sty_puts(cty, OAM_TELSVR_SHELL_CLOSE);

        Oam_CfgClearSendBuf(cty);
		
        /*����������������ö�ʱ���Ĳ�������˷���ɱ��ʱ��ǰ��*/
        CloseTelnetConn(cty);
        cty->tSocket = NULL;
        rpc = cty->link_cookie;
        if(rpc)
        {
            if (INVALID_TIMER_ID != rpc->dwTimerNo)
            {
                XOS_KillTimerByTimerId(rpc->dwTimerNo);
                rpc->dwTimerNo = INVALID_TIMER_ID;
            }
        }
        
        OAM_RETUB(cty->link_cookie);
        XOS_SysLog(OAM_CFG_LOG_NOTICE, "611000956402:ProcessOSSClose success\n");
    }

    XOS_SysLog(OAM_CFG_LOG_NOTICE, "611000956402:ProcessOSSClose end\n");

    /*    OAM_RETUB(tList);            */
    return ;

}
/**************************************************************************
* �������ƣ�Oam_ProcMsgFromInterpret
* ����������������ͷ��ص���Ϣ
* ���ʵı���
* �޸ĵı���
* ���������
*pMsgPara         �����ͷ��ص���Ϣ
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ���      ����
**************************************************************************/
void Oam_ProcMsgFromInterpret(BYTE* pMsgPara)
{
    BYTE    vty_num = 0;
    BYTE    output_mode = 0;
    BYTE    last_pkt_flag = 0;
    WORD16    seq_no = 0;
    WORD16    data_len = 0;
    WORD16    result_len = 0;
    sty     *cty = NULL;
    remote_parse_cookie *rpc = NULL;
    BYTE     msg_type = 0;
    WORD16     msg_len = 0;
    WORD16     one_page_flag = 0;
    TYPE_INTERPRET_MSGHEAD *pMsgHead = (TYPE_INTERPRET_MSGHEAD *)pMsgPara;

    /*�����Ϣ����*/
    {
        WORD16 wRecvLen = 0;
    
        if (XOS_GetMsgDataLen(PROCTYPE_OAM_TELNETSERVER, &wRecvLen) != XOS_SUCCESS)
        {
            /*�����ȡ�յ�����Ϣ����ʧ�ܣ�����false*/
            XOS_SysLog(OAM_CFG_LOG_ERROR, "Oam_ProcMsgFromInterpret:call XOS_GetMsgDataLen failed! ");
            return;
        }
        else
        {
            if (wRecvLen < sizeof(TYPE_INTERPRET_MSGHEAD))
            {
                XOS_SysLog(OAM_CFG_LOG_ERROR,
                    "Oam_ProcMsgFromInterpret: receive message length is %d, less than minimum length %d! ",
                    wRecvLen, sizeof(TYPE_INTERPRET_MSGHEAD));
                return;
            }
        }
    }
    
    vty_num = pMsgHead->bVtyNum;/**(BYTE *)(buf);*/
    if ((vty_num < 1) || (vty_num > MAX_REMOTE_PARSE_CONN))
    {
        return;
    }

    cty = &gtSty[vty_num - 1];
    if (!cty || !cty->tSocket)
    {
        return;
    }

    rpc = cty->link_cookie;
    if (!rpc)
    {
        return;
    }

    seq_no = pMsgHead->wSeqNo;/* *(WORD16 *)(buf + 3);*/

    cty->sshauthrslt = pMsgHead->bSSHAuthRslt;

    /*�����debug��Ϣ����ɱ��ʱ����������Ϣɱ��ʱ��*/
    if((seq_no != 0) && (INVALID_TIMER_ID != rpc->dwTimerNo))
    {
        XOS_KillTimerByTimerId(rpc->dwTimerNo);
        rpc->dwTimerNo = INVALID_TIMER_ID;
    }

    if (cty->sshconnmsg)
    {
        cty->sshconnmsg = FALSE;
        /* ��SSH���ӽ�����͸�SSHServer */
        oam_ssh_conn_ack(cty, pMsgPara);
        return;
    }

    if (seq_no != 0)
    {
        rpc->seq_no++;
    }

    output_mode = pMsgHead->bOutputMode;/* *(BYTE *)(buf + 1);*/
    last_pkt_flag = pMsgHead->bLastPacket; /* *(BYTE *)(buf + 2);*/
    data_len = pMsgHead->wDataLen;/* *(WORD16 *)(buf + 5);*/

    pMsgPara += MSG_HDR_SIZE_IN;
    if (seq_no != 0)    /* not debug info*/
    {
        cty->flags &= ~OAM_STY_FLAG_EXECMODE;
    }

    /* fall into ��Ԫ����� */
    WHILE (data_len >= 3)
    {
        LOOP_CHECK();
        msg_type = *(BYTE *)pMsgPara;
        msg_len = *(WORD16 *)(pMsgPara + 1);

        if (msg_len > (data_len - 3))
        {
            break;
        }
        XOS_SysLog(OAM_CFG_LOG_DEBUG, "Oam_ProcMsgFromInterpret:: msg_type=%u \n", msg_type);

        switch (msg_type)
        {
        case MSGID_NOTPASSWORD:
            /* taggle the sty echo to terminal on */
            oam_sty_input_password(cty, 0);
            break;
        case MSGID_ISPASSWORD:
            /* password, turn off echo to terminal */
            oam_sty_input_password(cty, 1);
            break;
        case MSGID_CHANGEPROMPT:
            /*�ı���ʾ���͸ı�����ģʽ*/
            oam_sty_put_prompt_line(cty, (CHAR *)(pMsgPara + 3), msg_len - 1);
            break;
        case MSGID_CHANGE_INPUTMODE:
            {
                BYTE ucInputFlag = 0;
                memcpy(&ucInputFlag, (CHAR *)(pMsgPara + 3 ), msg_len);
                if(OAM_INPUT_MODE_CHANGE_ASYN == ucInputFlag)
                {
                    cty->flags |= OAM_STY_FLAG_EXEINPUT;
                }
                else
                {
                    cty->flags &= ~OAM_STY_FLAG_EXEINPUT;
                }

            }
            break;
        case MSGID_OUTPUTRESULT:
            /* output from parse, be careful */
            result_len = (MAX_RESULT_DATA_LEN - 1) > msg_len ? msg_len : MAX_RESULT_DATA_LEN - 1;
            memset(result_data_buffer[vty_num - 1], 0, sizeof(result_data_buffer[vty_num - 1]));
            memcpy(result_data_buffer[vty_num - 1], (CHAR *)(pMsgPara + 3), result_len);
            result_data_buffer[vty_num - 1][result_len] = '\0';

            XOS_SysLog(OAM_CFG_LOG_DEBUG, "Oam_ProcMsgFromInterpret:: recv string length=%u \n", result_len);
   
            if (0 == seq_no)    /* this is debug info, put out directly */
            {
                if (OUTPUT_MODE_ROLL == output_mode)
                {
                    oam_sty_roll_progress(cty, result_data_buffer[vty_num - 1]);
                }
                else
                {
                    oam_sty_puts_with_line_process(cty, result_data_buffer[vty_num - 1]);
                }
            }
            else
            {                           
            	if (OUTPUT_MODE_ROLL == output_mode)
                {
                    oam_sty_roll_progress(cty, result_data_buffer[vty_num - 1]);
                }
                else if (OUTPUT_MODE_REQUEST == output_mode)
                { /* use buffered STY IO */
                    one_page_flag = sdp_puts(cty, result_data_buffer[vty_num - 1]);
                    /* record the last_pkt_flag */
                    rpc->last_pkt_flag = last_pkt_flag;

                    if (one_page_flag || (cty->p_disp_head == NULL))  /* within one page */
                    {
                        if (MSG_MORE_PKT == last_pkt_flag)
                        {
                            req_more_data_to_remote_parse(cty);
                        }
                    }

                    cty->flags |= OAM_STY_FLAG_ECHOING;
                }
                else
                { /* no buffered STY IO */
                    oam_sty_puts_with_line_process(cty, result_data_buffer[vty_num - 1]);

                    rpc->seq_no --; /*?? no need to modify seq_no for waiting result*/
                    /* record the last_pkt_flag */
                    rpc->last_pkt_flag = last_pkt_flag;

                    if (MSG_LAST_PKT != rpc->last_pkt_flag)
                    {
                        cty->flags |= OAM_STY_FLAG_EXECMODE;
                        req_more_data_to_remote_parse(cty);
                    }
                }
            }
            break;
#if  0
        case MSGID_WAITRESULT:
            result_len = MAX_RESULT_DATA_LEN - 1> msg_len ? msg_len: MAX_RESULT_DATA_LEN - 1;
            memset(result_data_buffer[vty_num - 1], 0, sizeof(result_data_buffer[vty_num - 1]));
            memcpy(result_data_buffer[vty_num - 1], (CHAR *)(pMsgPara + 3), result_len);
            result_data_buffer[vty_num - 1][result_len] = '\0';

            if (seq_no == 0)    /* this is debug info, put out directly */
            {
                if (output_mode == OUTPUT_MODE_ROLL)
                {
                    oam_sty_roll_progress(cty, result_data_buffer[vty_num - 1]);
                }
                else
                {
                    oam_sty_puts(cty, result_data_buffer[vty_num - 1]);
                }
            }
            else        /* use buffered STY IO */
            {
                WORD16 wStrlen = strlen(result_data_buffer[vty_num - 1]);
                oam_sty_puts(cty,result_data_buffer[vty_num - 1]);
                if(result_data_buffer[vty_num - 1][wStrlen - 1] == '\n')
                {
                    oam_sty_puts(cty,"\r");
                    cty->fill_len = 0 ;
                }
                else
                {
                    cty->fill_len += wStrlen;
                }

                rpc->seq_no --; /*no need to modify seq_no for waiting result*/
                /* record the last_pkt_flag */
                rpc->last_pkt_flag = last_pkt_flag;

                cty->flags |= OAM_STY_FLAG_EXECMODE;
            }
            break;
#endif
        case MSGID_NEEDEXTRINFO:/*�Զ�����Ϣ�Ĵ���*/
            {
                EXTRAINFO data ;
                memset(&data, 0, sizeof(EXTRAINFO));
                data.disp_line = gtExtraSty[vty_num - 1].disp_line;
                data.disp_pos = gtExtraSty[vty_num - 1].disp_pos;
                data.edit_pos = gtExtraSty[vty_num - 1].edit_pos;
                data.line_max = gtExtraSty[vty_num - 1].line_max;
                send_data_to_remote_parse(cty, (CHAR *)&data, sizeof(EXTRAINFO));
                /*cty�༭������0*/
                cty->disp_line = 0;
                cty->disp_pos = 0;
                cty->edit_pos = 0;
                cty->line_max = 0;
            }
            break;
        case MSGID_SHOWCMDLINE:
            oam_sty_put_command_line(cty, (CHAR *)(pMsgPara + 3), msg_len);
        	
            /*611001728952 ���¼�����ʾλ�ã�tab ��ֹ����󳬹��п�*/
            cty->line_max = OAM_STY_LINE_MAX - strlen(cty->prompt_line) - 1;
            while (  (cty->edit_pos > cty->disp_pos) &&
                         ((cty->edit_pos - cty->disp_pos) >= cty->line_max))
            {
                cty->disp_pos += OAM_STY_JUMP_LEN;
            }

            if((!(cty->flags & OAM_STY_FLAG_RAWMODE)) &&
                    (!(cty->flags & OAM_STY_FLAG_EXECMODE)) &&
                    (cty->p_disp_head == NULL))
            {
                oam_sty_put_prompt_cmdline(cty);
            }
            cty->flags &= ~OAM_STY_FLAG_EXECMODE;
            cty->flags |= OAM_STY_FLAG_ECHOING;
            break;
        case MSGID_TERMINATE:
            /*oam_sty_close(cty);*/
            XOS_SysLog(OAM_CFG_LOG_NOTICE, "611000895377:MSGID_TERMINATE received\n");
            oam_sty_puts(cty, OAM_TELSVR_SHELL_CLOSE);
            XOS_SysLog(OAM_CFG_LOG_NOTICE, "611000895377:MSGID_TERMINATE-call oam_sty_puts end\n");
            CloseTelnetConn(cty);
            XOS_SysLog(OAM_CFG_LOG_NOTICE, "611000895377:MSGID_TERMINATE-call CloseTelnetConn end\n");
            return;
        /*611000748643 ��ӱ�����Ϣ*/
        case MSGID_KEEPALIVE:
             if(INVALID_TIMER_ID != rpc->dwTimerNo)
            {
                XOS_KillTimerByTimerId(rpc->dwTimerNo);
                rpc->dwTimerNo = INVALID_TIMER_ID;
            }
            rpc->dwTimerNo = XOS_SetRelativeTimer(PROCTYPE_OAM_TELNETSERVER, TIMER_WAIT_INTERPRET_ACK, MAX_WAIT_ACK_LEN, (WORD32)cty->sty_id);
            break;
        default:
            break;
        }

        pMsgPara += (msg_len + 3);
        data_len -= (msg_len + 3);
    }
    oam_sty_enable_input(cty, 1);
}

/**************************************************************************
* �������ƣ�Oam_ProcTimerEventFromInterpret
* ����������������ͷ������ݳ�ʱ
* ���ʵı���
* �޸ĵı���
* ���������
*pMsgPara         ����ʱ���ĺ㶨����
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ���      ����
**************************************************************************/
void Oam_ProcTimerEventFromInterpret(WORD32 wTimerPara)
{
    DWORD iLoop = 0;
    remote_parse_cookie *rpc = NULL;

    sty *cty = NULL;
    /*���ݶ�ʱ���������ҳ�ʱ���ն�*/
    for(iLoop = 0; iLoop < MAX_STY_NUMBER; iLoop++)
    {
        if (wTimerPara == gtSty[iLoop].sty_id)
        {
            cty = &gtSty[iLoop];
            break;
        }
    }
    
    if(NULL == cty)
    {
        return;
    }

    /*����ն˶Ͽ��ˣ����ٴ�����ض�ʱ����Ϣ*/
    if(NULL == cty->tSocket)
    {
        return;
    }
      
    rpc = cty->link_cookie;
    if (rpc)
    {
        rpc->dwTimerNo = INVALID_TIMER_ID;
    }
    

    cty->flags &= ~OAM_STY_FLAG_EXEINPUT;
    cty->flags &= ~OAM_STY_FLAG_EXECMODE;
    cty->flags |= OAM_STY_FLAG_ECHOING;

    oam_sty_puts(cty, OAM_TELSVR_PARSE_TIMEOUT);
  
    /*����Ǻ�cli�������ӳ�ʱ���˿�socket���� */
    if (rpc && (rpc->msg_id == MSGID_NEWCONNECT))
    {
         CloseTelnetConn(cty);
    }
}
/**************************************************************************
* �������ƣ�Oam_CloseAllVty
* �����������ر�vty��ȫ�ֱ���
* ���ʵı���
* �޸ĵı���
* ���������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ���      ����
**************************************************************************/
void Oam_CloseAllVty(CHAR *pcReason)
{
    int iLoop = 0;
    for(iLoop = 1; iLoop < MAX_STY_NUMBER; iLoop++)
    {
        if (NULL != gtSty[iLoop].tSocket)
        {
            if (pcReason)
            {
                oam_sty_puts(&gtSty[iLoop], pcReason);
            }
            oam_sty_puts(&gtSty[iLoop], OAM_TELSVR_SHELL_CLOSE);
            CloseTelnetConn(&gtSty[iLoop]);
        }
    }
}
/**************************************************************************
* �������ƣ�Oam_ProcCeaseMsgFromInterpret
* �����������յ����ͷ��ص�������ֹ����
* ���ʵı���
* �޸ĵı���
* ���������
*pMsgPara         �����ͷ��ص���ֹ����ֵ
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ���      ����
**************************************************************************/
void Oam_ProcCeaseMsgFromInterpret(BYTE* pMsgPara)
{
    TYPE_CEASE_MSG *pCeaseMsg = (TYPE_CEASE_MSG *)pMsgPara;
    BYTE    vty_num = 0;
    sty     *cty = NULL;

    vty_num = pCeaseMsg->bVtyNum;/**(BYTE *)(buf);*/
    if ((vty_num < 1) || (vty_num > MAX_REMOTE_PARSE_CONN))
    {
        return;
    }
    
    cty = &gtSty[vty_num - 1];
    if (!cty || !cty->tSocket)
    {
        return;
    }

    cty->bCeaseAttr = pCeaseMsg->bStopAttr;
}
/**************************************************************************
* �������ƣ�Oam_ProcSSHAuth
* ��������������sshserver�������û���֤��Ϣ
* ���ʵı���
* �޸ĵı���
* ���������
*pMsgPara         ��ssh�����socket������Ϣ
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ���      ����
**************************************************************************/
void Oam_ProcSSHAuth(LPVOID pMsgPara)
{
    OAM_FD_MSG *pMsg = (OAM_FD_MSG *)pMsgPara;
    TYPE_FD *tSocket = gtSocketList;

    TELNETSVR_VOID_ASSERT(pMsgPara != NULL);

    /*��ȫ�������ȡ�յ���Ϣ��socket�ṹ*/
    WHILE (tSocket)
    {
        LOOP_CHECK();
        if (tSocket->ucFd == pMsg->fd)/*?? ��OSS ��BRS����socket�������Ƿ�������?*/
        {
            break;
        }
        tSocket = tSocket->next;
    }
    if (!tSocket)
    {
        return;
    }

    ProcessSSHAuthEvent(tSocket, (CHAR *)pMsg->buf, pMsg->len);
    
    return;
}
/**********************************************************************
* �������ƣ�Oam_SendBufData
* �������������ͻ������������
* ���ʵı���
* �޸ĵı���
* �����������
* �����������
* �� �� ֵ����
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2010/8/25    V1.0   ���      ����
************************************************************************/
void Oam_SendBufData(void)
{
    T_StySendBuf *pBuf = NULL;
    int iFor;
	
    for (iFor=0; iFor < MAX_STY_NUMBER; iFor++)
    { 
        T_StySendBuf *pBufTail = gtSty[iFor].ptSendBufTail;
        pBuf = gtSty[iFor].ptSendBufHead;
        /*�������»���ʹ�� */
        gtSty[iFor].ptSendBufHead = NULL;
        gtSty[iFor].ptSendBufTail = NULL;
		
        while (pBuf != NULL)
        {
            SWORD32 swRet;
            T_StySendBuf *pTmpBuf = pBuf;
            pBuf = pBuf->pNext;
            if (gtSty[iFor].tSocket == NULL)
            {
                OAM_RETUB(pTmpBuf->buf);
                OAM_RETUB(pTmpBuf);
                break;
            }
			
            /*���ͣ�������ʧ�ܵĻ��������±��浽����*/
            swRet = OAM_FD_Send(gtSty[iFor].tSocket->ucFd, 0, pTmpBuf->buf, pTmpBuf->len);
            if (swRet == ERROR)
            {
                /*����ʧ�ܻ����±��浽���棬���
                    �������ͷ��Ϊ�գ�ֱ�Ӱ�tail����һ������Ϊ��ǰ���ڷ��͵Ľڵ���һ��
                    ���Ϊ�գ�����ͷ��Ϊ��ǰ���ڷ��͵Ľڵ�*/
                if (gtSty[iFor].ptSendBufHead == NULL)
                {
                    gtSty[iFor].ptSendBufHead = pTmpBuf;
                }
                else
                {
                    gtSty[iFor].ptSendBufTail->pNext = pBuf;
                }
                gtSty[iFor].ptSendBufTail  = pBufTail;

                break;
            }
            else
            {
                OAM_RETUB(pTmpBuf->buf);
                OAM_RETUB(pTmpBuf);
            }
        }
    }
}
/**********************************************************************
* �������ƣ�Oam_CliVtyCommAbort
* ��������������ͨ���쳣�����
* ���ʵı���
* �޸ĵı���
* �����������
* �����������
* �� �� ֵ����
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2010/8/25    V1.0   ���      ����
************************************************************************/
void Oam_CliVtyCommAbort(LPVOID pMsgPara)
{
    TELNETSVR_VOID_ASSERT(pMsgPara != NULL);
    OAM_FD_MSG *pMsg = (OAM_FD_MSG*)pMsgPara;
    CHAR *strTmp = OAM_TELSVR_COMM_LOST;
    OAM_FD_Send(pMsg->fd, pMsg->flag, strTmp, strlen(strTmp));
}

/**********************************************************************
* �������ƣ�int CloseSocket(TYPE_FD *tSocket)
* �������������ݶ˿ڹر�����
* ���������
* ���������
* �� �� ֵ��
            
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
************************************************************************/
int CloseSocket(TYPE_FD *tSocket)
{
    TYPE_FD *tList = gtSocketList, *tPrev = NULL;
    DWORD nResult = 0;
    sty *cty = NULL;
    CHAR aucClient[50] = {0};

    TELNETSVR_0_ASSERT(tSocket != NULL);

    XOS_SysLog(OAM_CFG_LOG_NOTICE, "611000895377:CloseSocket called\n");
	
    XOS_snprintf(aucClient, sizeof(aucClient), "%s:%u", tSocket->szIpAddr, tSocket->wPort);
    XOS_SysLog(OAM_CFG_LOG_NOTICE, "611000956402:CloseSocket:: client is %s\n", aucClient);
    WHILE (tList)
    {
        LOOP_CHECK();
        if(tSocket == tList)
        {
            break;
        }
        tPrev = tList;
        tList = tList->next;
    }
    if(!tList)
    {/*���û���ҵ�*/
        XOS_SysLog(OAM_CFG_LOG_NOTICE, "611000956402:CloseSocket:: tSocket not exist in gtSocketList\n");
        return FAIL_FIND_SOCKET;
    }

    cty = FindSty(tList);
    
#ifdef USE_BRS
    if(FD_TYPE_BRSTELNET == tSocket->ucFDType)
    {
        nResult = brs_close(tList->ucFd);
    }
    else
#endif
    if(FD_TYPE_OSSTELNET == tSocket->ucFDType)
    {
        OAM_FD_Quit((WORD16)(tSocket->ucFd), CONNECTION_FROM_TELNET);
    }
    else if(FD_TYPE_SERIAL== tSocket->ucFDType)
    {
        OAM_FD_Quit((WORD16)(tSocket->ucFd), CONNECTION_FROM_SERIAL);
    }
    else if(FD_TYPE_OSSSSH== tSocket->ucFDType)
    {
        send_close_ssh(cty);
    }

    XOS_SysLog(OAM_CFG_LOG_NOTICE, "611000895377:CloseSocket-OAM_FD_Quit end\n");
	
    /* ɾ���ڵ� */
    if ((tList == gtSocketList) && (tList == gtSocketTail))
    {
        gtSocketList = gtSocketTail = NULL;
    }
    else if (tList == gtSocketList)
    {
        gtSocketList = tList->next;
    }
    else if (tList == gtSocketTail)
    {
        tPrev->next = NULL;
        gtSocketTail = tPrev;
    }
    else
    {
        tPrev->next = tList->next;
    }

    if(cty)
    {
        remote_parse_cookie *rpc = NULL;
        XOS_SysLog(OAM_CFG_LOG_NOTICE, "611000895377:terminate_conn_to_remote_parse begin\n");
        terminate_conn_to_remote_parse(cty);
        XOS_SysLog(OAM_CFG_LOG_NOTICE, "611000895377:terminate_conn_to_remote_parse end\n");
        oam_sty_close(cty);
        XOS_SysLog(OAM_CFG_LOG_NOTICE, "611000895377:oam_sty_close end\n");
        cty->tSocket = NULL;
        rpc = cty->link_cookie;
        if(rpc)
        {
            if (INVALID_TIMER_ID != rpc->dwTimerNo)
            {
                XOS_KillTimerByTimerId(rpc->dwTimerNo);
                rpc->dwTimerNo = INVALID_TIMER_ID;
            }
            OAM_RETUB(cty->link_cookie);
        }
        Oam_CfgClearSendBuf(cty);
    }

    OAM_RETUB(tList);
    XOS_SysLog(OAM_CFG_LOG_NOTICE, "611000895377:CloseSocket end\n");
	
    return nResult;
}
/**********************************************************************
* �������ƣ�int CloseTelnetConn(sty *cty)
* �����������ر�ĳtelnet�ն�
* ���������
* ���������
* �� �� ֵ��
            
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
************************************************************************/
int CloseTelnetConn(sty *cty)
{
    TELNETSVR_0_ASSERT(cty != NULL);
    XOS_SysLog(OAM_CFG_LOG_NOTICE, "\n\rCloseTelnetConn:The telnet client has disconnected!\n");

    return CloseSocket(cty->tSocket);
}
BYTE AddSty(TYPE_FD *tSocket)
{
    BOOL bFindUsableSty = FALSE;
    BYTE iLoop = 0;
    remote_parse_cookie *rpc = NULL;

    if (BOARD_SLAVE ==  XOS_GetBoardMSState())
    {
        return ADD_STY_SLAVEBOARD;
    }

    /* 0-����; 1��2��3-Telnet; 4��5��6-SSH */
    if(FD_TYPE_SERIAL == tSocket->ucFDType)
    {/*���ڹ̶�ռ��0������Ѿ�ռ�þͲ���������*/
        bFindUsableSty = (NULL == gtSty[0].tSocket);
    }
    else if (FD_TYPE_OSSTELNET == tSocket->ucFDType)
    {
        for(iLoop = 1; iLoop < 4; iLoop++)
        {
            if (NULL == gtSty[iLoop].tSocket)
            {
                bFindUsableSty = TRUE;
                break;
            }
        }
    }
    else if (FD_TYPE_OSSSSH == tSocket->ucFDType)
    {
        for(iLoop = 4; iLoop < MAX_STY_NUMBER; iLoop++)
        {
            if (NULL == gtSty[iLoop].tSocket)
            {
                bFindUsableSty = TRUE;
                break;
            }
        }
    }
    else
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "[AddSty] Unknown FDType!\n");
    }
    
    if (!bFindUsableSty)
    {
        return ADD_STY_CONNECTIONFULL;
    }

    gtSty[iLoop].tSocket        = tSocket;
    gtSty[iLoop].disp_line      = 1;
    gtSty[iLoop].fill_len         = 0;
    gtSty[iLoop].p_disp_head    = NULL;
    gtSty[iLoop].flags               = (OAM_STY_FLAG_REMOTE_PARSE |OAM_STY_FLAG_ECHOING);
    gtSty[iLoop].bCeaseAttr     = 0;

    Oam_CfgClearSendBuf(&gtSty[iLoop]);

    /*Ϊ����������һ��ר�õĽ��ͳ���ʵ��*/
    {
        /*WORD32 dwPno = 0;
        XOS_STATUS tState = OSS_CreateProcess(
                                        0,
                                        0,
                                        PROCTYPE_OAM_CLIMANAGER,
                                        &(dwPno)
                                     );

        if (tState != XOS_SUCCESS)
        {
            gtSty[iLoop].tSocket = NULL;
            return ADD_STY_INTERPRETSTARTFAIL;                 
        }*/
        /*
        XOS_GetSelfJID(&gtSty[iLoop].interpJid);
        gtSty[iLoop].interpJid.wModule = 2;
        gtSty[iLoop].interpJid.dwPno = OSS_ConstructPNO(PROCTYPE_OAM_CLIMANAGER, 1);
        */
        memcpy(&gtSty[iLoop].interpJid, &g_tCfgAgentJid, sizeof(g_tCfgAgentJid));
        memcpy(&gtSty[iLoop].sshsvrJid, &g_tSSHServerJid, sizeof(g_tSSHServerJid));
    }

    rpc = (remote_parse_cookie *)XOS_GetUB(sizeof(remote_parse_cookie));
    if (!rpc)
    {
        gtSty[iLoop].tSocket = NULL;
        return ADD_STY_OTHERERROR;
    }
    memset(rpc, 0, sizeof(remote_parse_cookie));
    rpc->vty_num = iLoop + 1;   /* notice: +1 is right */
    rpc->seq_no = 1;        /* make seq_no start from 1 */
    rpc->dwTimerNo = INVALID_TIMER_ID;
    gtSty[iLoop].link_cookie = rpc;
    gtSty[iLoop].sty_id = rpc->vty_num;

    memcpy(&gtExtraSty[iLoop], &gtSty[iLoop], sizeof(sty));
    return ADD_STY_SUCCESS;
}

sty *FindStyByFd(FD fd)
{
    DWORD nLoop = 0;

    for(nLoop = 0; nLoop < MAX_STY_NUMBER; nLoop++)
    {
        if ((gtSty[nLoop].tSocket != NULL) && (gtSty[nLoop].tSocket->ucFd == fd))
        {
            return &gtSty[nLoop];
        }
    }
    return NULL;
}

sty *FindSty(TYPE_FD *tSocket)
{
    DWORD nLoop = 0;

    TELNETSVR_0_ASSERT(tSocket != NULL);

    for(nLoop = 0; nLoop < MAX_STY_NUMBER; nLoop++)
    {
        if (gtSty[nLoop].tSocket == tSocket)
        {
            return &gtSty[nLoop];
        }
    }
    return NULL;
}

sty *FindExtraSty(TYPE_FD *tSocket)
{
    DWORD nLoop = 0;

    TELNETSVR_0_ASSERT(tSocket != NULL);

    for(nLoop = 0; nLoop < MAX_STY_NUMBER; nLoop++)
    {
        if (gtExtraSty[nLoop].tSocket == tSocket)
        {
            return &gtExtraSty[nLoop];
        }
    }
    return NULL;
}

int oam_close_socket_for_ssh(TYPE_FD *tSocket)
{
    TYPE_FD *tList = gtSocketList, *tPrev = NULL;
    DWORD nResult = 0;
    sty *cty = NULL;

    TELNETSVR_0_ASSERT(tSocket != NULL);

    WHILE (tList)
    {
        LOOP_CHECK();
        if(tSocket == tList)
        {
            break;
        }
        tPrev = tList;
        tList = tList->next;
    }
    if(!tList)
    {/*���û���ҵ�*/
        return FAIL_FIND_SOCKET;
    }

    cty = FindSty(tList);
    
     /* ɾ���ڵ� */
    if ((tList == gtSocketList) && (tList == gtSocketTail))
    {
        gtSocketList = gtSocketTail = NULL;
    }
    else if (tList == gtSocketList)
    {
        gtSocketList = tList->next;
    }
    else if (tList == gtSocketTail)
    {
        tPrev->next = NULL;
        gtSocketTail = tPrev;
    }
    else
    {
        tPrev->next = tList->next;
    }

    if(cty)
    {
        remote_parse_cookie *rpc = NULL;
        oam_sty_close(cty);
        cty->tSocket = NULL;
        rpc = cty->link_cookie;
        if(rpc)
        {
            if (INVALID_TIMER_ID != rpc->dwTimerNo)
            {
                XOS_KillTimerByTimerId(rpc->dwTimerNo);
                rpc->dwTimerNo = INVALID_TIMER_ID;
            }
            OAM_RETUB(cty->link_cookie);
        }
    }

    OAM_RETUB(tList);
    return nResult;
}

/*��ʾ�������������*/
void Oam_DbgShowSendBuf(int iStyNo)
{
    BYTE aucBuf[512] = {0};

    T_StySendBuf *pBuf = NULL;

    if (iStyNo > (MAX_STY_NUMBER - 1))
    {
        printf("input parameter %d is large than max styno , right range is [0 - %d]\n", iStyNo, MAX_STY_NUMBER - 1);
        return;
    }

    if (!gtSty[iStyNo].ptSendBufHead)
    {
        printf("send buffer is null!\n");
        return;
    }

    pBuf = gtSty[iStyNo].ptSendBufHead;
    while (pBuf != NULL)
    {
        memset(aucBuf, 0, sizeof(aucBuf));
        memcpy(aucBuf, pBuf->buf, OAM_MIN((sizeof(aucBuf) -1), pBuf->len));
        printf("[%s]\n", aucBuf);
        pBuf = pBuf->pNext;
    }
}

/*****************************************************************************/
/**
@brief  ��ӡclivty�����ӵĿͻ�����Ϣ
@verbatim
\n��
@endverbatim

@li @b ����ֵ�б�
@verbatim
��
@endverbatim

@li @b ���÷�Χ��
\n������OMP/CMM2���壬��oammanager������

@li @b ������ʾ��
\n��
 
@li @b ����˵����
\n1������ʾ���ӿͻ��˵�ip��ַ�Ͷ˿ں�
2��flags�ֶ���vty���ӵı�־λ��cli��λ������
@li @b ���ʾ����
\n
STYNO  STATE  CLIENT                   FLAGS
=============================================
0      idle                            0
0      idle                            0
0      idle                            0
5      used   192.168.0.1:60029        36
0      idle                            0
0      idle                            0
=============================================
@li @b �޸ļ�¼��
-#  �� 
*/
/*****************************************************************************/
void OAM_DbgShowCliAllVtyInfo(void)
{	
    int iLoop = 0;
    printf("        [Clivty infomation table]\n");
    printf("STYNO,  STATE,                CLIENT,  FLAGS\n");
    printf("================================================\n");
    for(iLoop = 1; iLoop < MAX_STY_NUMBER; iLoop++)
    {
        WORD16 wVtyNum = 0, wFlags = 0;
        CHAR aucState[20] = {0};
        CHAR aucClient[30] = {'-'};
        if (NULL != gtSty[iLoop].tSocket)
        {
            remote_parse_cookie *rpc = gtSty[iLoop].link_cookie;
            if (rpc)
            {
                wVtyNum = rpc->vty_num;
            }

            strcpy(aucState, "used");
            XOS_snprintf(aucClient, sizeof(aucClient),"%s:%u", gtSty[iLoop].tSocket->szIpAddr, gtSty[iLoop].tSocket->wPort);
            wFlags = gtSty[iLoop].flags;
        }
	 else
        {
            strcpy(aucState, "idle");
	 }

	 printf("%5u,%7s,%21s,%7u\n", wVtyNum, aucState, aucClient, wFlags);
    }
	 printf("================================================\n");
}


