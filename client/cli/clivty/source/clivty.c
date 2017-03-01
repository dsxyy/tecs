/**************************************************************************
* 版权所有 (C)2002, 深圳市中兴通讯股份有限公司。
*
* 文件名称：CliVty.c
* 文件标识：
* 内容摘要：处理内部消息
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
* 修改记录2：
**************************************************************************/
/**************************************************************************
*                           头文件                                        *
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
*                      函数申明                                        *
********************************************************************/
extern void oam_sty_puts(sty *cty, CHAR *text);
/**************************************************************************
*                          全局变量                                       *
**************************************************************************/
static CHAR result_data_buffer[MAX_REMOTE_PARSE_CONN][MAX_RESULT_DATA_LEN];

TYPE_FD *gtSocketList , *gtSocketTail;
sty gtSty[MAX_STY_NUMBER];
sty gtExtraSty[MAX_STY_NUMBER];
JID g_tCfgAgentJid={0};
JID g_tSSHServerJid={0};

extern WORD16 g_wWhileLoopCnt;
/**************************************************************************
*                          全局函数                                     *
**************************************************************************/
static int CloseSocket(TYPE_FD *tSocket);
/**************************************************************************
* 函数名称：Oam_CliVtyInit
* 功能描述：初始化Vty全局变量
* 访问的表：无
* 修改的表：无
* 输入参数：
*
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    殷浩      创建
**************************************************************************/
void Oam_CliVtyInit()
{
    gtSocketList = NULL;
    gtSocketTail = NULL;
    /*初始化gtSty*/
    memset(&gtSty, 0, sizeof(gtSty));
    memset(&gtExtraSty, 0, sizeof(gtExtraSty));
    memset(&g_tCfgAgentJid, 0, sizeof(g_tCfgAgentJid));
    XOS_GetSelfJID(&g_tCfgAgentJid);
    g_tCfgAgentJid.dwJno = XOS_ConstructJNO(PROCTYPE_OAM_CLIMANAGER, 1);    

    memset(&g_tSSHServerJid, 0, sizeof(g_tSSHServerJid));
    XOS_GetSelfJID(&g_tSSHServerJid);
    g_tSSHServerJid.dwJno = XOS_ConstructJNO(PROCTYPE_OAM_SSHSERVER, 1);
#if 0
    /* 注册与RPU断链、恢复链路的消息 */
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
* 函数名称：Oam_CliVtyConnect
* 功能描述：新建连接请求后，VTY保存链路信息并通知
                              解释程序
* 访问的表：无
* 修改的表：无
* 输入参数：
*pMsgPara         ：连接的链路信息
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    殷浩      创建
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
    {/*串口*/
        /*OSS_Printf(PRINT_OAM_TELSVR, PRN_NORMAL, PRN_LEVEL_HIGHEST,
                   "ProcessOssConnect:Receive From SERIAL CONNECTION. file=%d; line=%d\n",__FILE__,__LINE__);*/
        tSocket->ucFDType = FD_TYPE_SERIAL;
    }
    else if(CONNECTION_FROM_TELNET == pMsg->flag)
    {/*管理网口*/
        /*OSS_Printf(PRINT_OAM_TELSVR, PRN_NORMAL, PRN_LEVEL_HIGHEST,
                   "ProcessOssConnect:Receive From TELNET CONNECTION. file=%d; line=%d\n",__FILE__,__LINE__);*/
        tSocket->ucFDType = FD_TYPE_OSSTELNET;
    }
    else if(CONNECTION_FROM_SSH == pMsg->flag)
    {/*管理网口SSH*/
        tSocket->ucFDType = FD_TYPE_OSSSSH;
    }
    else
    {/*管理网口*/
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
        case ADD_STY_CONNECTIONFULL:   /*连接数已满*/
            /*OSS_Printf(PRINT_OAM_TELSVR, PRN_NORMAL, PRN_LEVEL_HIGHEST,
                       "ProcessOssConnect:Connection is full!. file=%s; line=%d\n",__FILE__,__LINE__);*/
            strTmp = "\nConnection is full!\n";
            break;
        case ADD_STY_INTERPRETSTARTFAIL:   /*解释实例启动失败*/
            /*OSS_Printf(PRINT_OAM_TELSVR, PRN_NORMAL, PRN_LEVEL_HIGHEST,
                       "ProcessOssConnect:Start Interpret instance fail!. file=%s; line=%d\n",__FILE__,__LINE__);*/
            strTmp = "\nStart Interpret instance fail!\n";
            break;
        case ADD_STY_SLAVEBOARD:   /*当前单板是备板*/
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

    /* 加入Socket列表 */
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
            /*发送新建连接消息*/
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
* 函数名称：Oam_CliVtyRecive
* 功能描述：收到用户输入的数据后，vty对数据进行分析
* 访问的表：无
* 修改的表：无
* 输入参数：
*pMsgPara         ：客户端输入的信息
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    殷浩      创建
**************************************************************************/
void Oam_CliVtyRecive(LPVOID pMsgPara)
{
    OAM_FD_MSG *pMsg = (OAM_FD_MSG *)pMsgPara;
    TYPE_FD *tSocket = gtSocketList;

    TELNETSVR_VOID_ASSERT(pMsgPara != NULL);

    /*如果是串口终端，并且是收到的第一个回车*/
    if((CONNECTION_FROM_SERIAL  == pMsg->flag) && (NULL == gtSty[0].tSocket))
    {
        if(pMsg->buf && (('\n' == pMsg->buf[0]) ||('\r' == pMsg->buf[0]) ))
        {
            ProcessOssConnect(pMsgPara);
            return;
        }
    }

    /*从全局链表获取收到消息的socket结构*/
    WHILE (tSocket)
    {
        LOOP_CHECK();
        if (tSocket->ucFd == pMsg->fd)/*?? 从OSS 和BRS来的socket描述符是否可能相等?*/
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
* 函数名称：Oam_CliVtyClose
* 功能描述：收到连接关闭消息后，关闭vty连接
* 访问的表：无
* 修改的表：无
* 输入参数：
*pMsgPara         ：连接的链路信息
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    殷浩      创建
**************************************************************************/
void Oam_CliVtyClose(LPVOID pMsgPara)
{
   OAM_FD_MSG *pMsg = (OAM_FD_MSG*)pMsgPara;
    TYPE_FD *tList = gtSocketList, *tPrev = NULL;
    sty *cty = NULL;
    CHAR aucClient[50] = {0};

    TELNETSVR_VOID_ASSERT(pMsgPara != NULL);

    XOS_SysLog(OAM_CFG_LOG_NOTICE, "611000956402:ProcessOSSClose called\n");
    /*从全局链表获取收到消息的socket结构*/
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
    {/*如果没有找到*/
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
		
        /*下面这段里面有设置定时器的操作，因此放在杀定时器前面*/
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
* 函数名称：Oam_ProcMsgFromInterpret
* 功能描述：处理解释返回的消息
* 访问的表：无
* 修改的表：无
* 输入参数：
*pMsgPara         ：解释返回的消息
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    殷浩      创建
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

    /*检查消息长度*/
    {
        WORD16 wRecvLen = 0;
    
        if (XOS_GetMsgDataLen(PROCTYPE_OAM_TELNETSERVER, &wRecvLen) != XOS_SUCCESS)
        {
            /*如果获取收到的消息长度失败，返回false*/
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

    /*如果是debug消息，不杀定时器；其他消息杀定时器*/
    if((seq_no != 0) && (INVALID_TIMER_ID != rpc->dwTimerNo))
    {
        XOS_KillTimerByTimerId(rpc->dwTimerNo);
        rpc->dwTimerNo = INVALID_TIMER_ID;
    }

    if (cty->sshconnmsg)
    {
        cty->sshconnmsg = FALSE;
        /* 将SSH连接结果发送给SSHServer */
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

    /* fall into 三元组解析 */
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
            /*改变提示符和改变输入模式*/
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
        case MSGID_NEEDEXTRINFO:/*对额外信息的处理*/
            {
                EXTRAINFO data ;
                memset(&data, 0, sizeof(EXTRAINFO));
                data.disp_line = gtExtraSty[vty_num - 1].disp_line;
                data.disp_pos = gtExtraSty[vty_num - 1].disp_pos;
                data.edit_pos = gtExtraSty[vty_num - 1].edit_pos;
                data.line_max = gtExtraSty[vty_num - 1].line_max;
                send_data_to_remote_parse(cty, (CHAR *)&data, sizeof(EXTRAINFO));
                /*cty编辑参数清0*/
                cty->disp_line = 0;
                cty->disp_pos = 0;
                cty->edit_pos = 0;
                cty->line_max = 0;
            }
            break;
        case MSGID_SHOWCMDLINE:
            oam_sty_put_command_line(cty, (CHAR *)(pMsgPara + 3), msg_len);
        	
            /*611001728952 重新计算显示位置，tab 防止补齐后超过行宽*/
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
        /*611000748643 添加保活消息*/
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
* 函数名称：Oam_ProcTimerEventFromInterpret
* 功能描述：处理解释返回数据超时
* 访问的表：无
* 修改的表：无
* 输入参数：
*pMsgPara         ：定时器的恒定参数
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    殷浩      创建
**************************************************************************/
void Oam_ProcTimerEventFromInterpret(WORD32 wTimerPara)
{
    DWORD iLoop = 0;
    remote_parse_cookie *rpc = NULL;

    sty *cty = NULL;
    /*根据定时器参数查找超时的终端*/
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

    /*如果终端断开了，不再处理相关定时器消息*/
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
  
    /*如果是和cli建立连接超时，端口socket连接 */
    if (rpc && (rpc->msg_id == MSGID_NEWCONNECT))
    {
         CloseTelnetConn(cty);
    }
}
/**************************************************************************
* 函数名称：Oam_CloseAllVty
* 功能描述：关闭vty中全局变量
* 访问的表：无
* 修改的表：无
* 输入参数：
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    殷浩      创建
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
* 函数名称：Oam_ProcCeaseMsgFromInterpret
* 功能描述：收到解释返回的命令终止属性
* 访问的表：无
* 修改的表：无
* 输入参数：
*pMsgPara         ：解释返回的终止属性值
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    殷浩      创建
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
* 函数名称：Oam_ProcSSHAuth
* 功能描述：处理sshserver发来的用户认证消息
* 访问的表：无
* 修改的表：无
* 输入参数：
*pMsgPara         ：ssh本身的socket连接信息
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    殷浩      创建
**************************************************************************/
void Oam_ProcSSHAuth(LPVOID pMsgPara)
{
    OAM_FD_MSG *pMsg = (OAM_FD_MSG *)pMsgPara;
    TYPE_FD *tSocket = gtSocketList;

    TELNETSVR_VOID_ASSERT(pMsgPara != NULL);

    /*从全局链表获取收到消息的socket结构*/
    WHILE (tSocket)
    {
        LOOP_CHECK();
        if (tSocket->ucFd == pMsg->fd)/*?? 从OSS 和BRS来的socket描述符是否可能相等?*/
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
* 函数名称：Oam_SendBufData
* 功能描述：发送缓存里面的数据
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2010/8/25    V1.0   殷浩      创建
************************************************************************/
void Oam_SendBufData(void)
{
    T_StySendBuf *pBuf = NULL;
    int iFor;
	
    for (iFor=0; iFor < MAX_STY_NUMBER; iFor++)
    { 
        T_StySendBuf *pBufTail = gtSty[iFor].ptSendBufTail;
        pBuf = gtSty[iFor].ptSendBufHead;
        /*链表重新回收使用 */
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
			
            /*发送，发送又失败的话，会重新保存到缓存*/
            swRet = OAM_FD_Send(gtSty[iFor].tSocket->ucFd, 0, pTmpBuf->buf, pTmpBuf->len);
            if (swRet == ERROR)
            {
                /*发送失败会重新保存到缓存，因此
                    如果缓存头不为空，直接把tail的下一个设置为当前正在发送的节点下一个
                    如果为空，缓存头设为当前正在发送的节点*/
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
* 函数名称：Oam_CliVtyCommAbort
* 功能描述：处理通信异常的情况
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2010/8/25    V1.0   殷浩      创建
************************************************************************/
void Oam_CliVtyCommAbort(LPVOID pMsgPara)
{
    TELNETSVR_VOID_ASSERT(pMsgPara != NULL);
    OAM_FD_MSG *pMsg = (OAM_FD_MSG*)pMsgPara;
    CHAR *strTmp = OAM_TELSVR_COMM_LOST;
    OAM_FD_Send(pMsg->fd, pMsg->flag, strTmp, strlen(strTmp));
}

/**********************************************************************
* 函数名称：int CloseSocket(TYPE_FD *tSocket)
* 功能描述：根据端口关闭连接
* 输入参数：
* 输出参数：
* 返 回 值：
            
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
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
    {/*如果没有找到*/
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
	
    /* 删除节点 */
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
* 函数名称：int CloseTelnetConn(sty *cty)
* 功能描述：关闭某telnet终端
* 输入参数：
* 输出参数：
* 返 回 值：
            
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
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

    /* 0-串口; 1、2、3-Telnet; 4、5、6-SSH */
    if(FD_TYPE_SERIAL == tSocket->ucFDType)
    {/*串口固定占用0，如果已经占用就不能再连接*/
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

    /*为新连接启动一个专用的解释程序实例*/
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
    {/*如果没有找到*/
        return FAIL_FIND_SOCKET;
    }

    cty = FindSty(tList);
    
     /* 删除节点 */
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

/*显示缓存里面的数据*/
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
@brief  打印clivty上连接的客户端信息
@verbatim
\n无
@endverbatim

@li @b 返回值列表：
@verbatim
无
@endverbatim

@li @b 适用范围：
\n适用于OMP/CMM2单板，在oammanager下运行

@li @b 风险提示：
\n无
 
@li @b 其它说明：
\n1、会显示连接客户端的ip地址和端口号
2、flags字段是vty连接的标志位，cli定位问题用
@li @b 输出示例：
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
@li @b 修改记录：
-#  ： 
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


