/**************************************************************************
* 版权所有 (C)2007, 深圳市中兴通讯股份有限公司。
*
* 文件名称： telnetserver.c
* 文件标识：
* 内容摘要：
* 其它说明： 无
* 当前版本： V1.0
* 作    者： 3G平台
* 完成日期：
*
* 修改记录1：
*    修改日期：2007年6月26日
*    版 本 号：V1.0
*    修 改 人：
*    修改内容：创建
**************************************************************************/

/**************************************************************************
*                           头文件                                        *
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
*                          常量                                           *
**************************************************************************/
/**************************************************************************
*                          宏                                             *
**************************************************************************/
/**************************************************************************
*                          数据类型                                       *
**************************************************************************/
/**************************************************************************
*                          全局变量                                       *
**************************************************************************/

/**************************************************************************
*                          本地变量                                       *
**************************************************************************/

/**************************************************************************
*                          局部函数原型                                   *
**************************************************************************/
static void OamTelSvrEntryCheckPtr(BYTE *InputPtr);
static WORD32 Initialize(void);
/**************************************************************************
*                          全局函数实现                                   *
**************************************************************************/
extern int  oss_telnetdInit(void);
/**************************************************************************
*                          主入口函数                                     *
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
            /*初始化一些全局变量*/
            if(Initialize() != OAM_OK)
            {
                return;
            }
            #if 0
            /* 启动Telnet帧听*/
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
        case MSG_BRSSOCKET_MSG:/*业务网口socket消息*/
            ProcessSocketCommand(pMsgPara);
            break;
	#endif
        case MSG_FD_CONNECT:/*管理网口新建连接请求*/
            /*OSS_Printf(PRINT_OAM_TELSVR, PRN_NORMAL, PRN_LEVEL_HIGHEST,
                       "OamTelnetServer: Receive MSG_FD_CONNECT ;CurrentState=%d; line=%d\n",wState,__LINE__);*/
            ProcessOssConnect(pMsgPara);
            break;
        case MSG_FD_RECEIVE:/*管理网口、串口发送数据*/
            ProcessOSSRecive(pMsgPara);
            break;
        case MSG_FD_CLOSE:/*管理网口断开连接请求*/
            ProcessOSSClose(pMsgPara);
            break;
        case OAM_MSG_INTERPRET_TO_TELNET:/*解释程序返回数据*/
            Oam_ProcMsgFromInterpret((BYTE *)pMsgPara);
            break;
        case EV_TIMER_TELSVR:/*解释返回的消息超时*/
            Oam_ProcTimerEventFromInterpret(XOS_GetConstParaOfCurTimer(pMsgPara));
            break;

            /* 考虑与RPU邋链路断等 */
            /* 单板与RPU链路断 */
        case EV_COMM_ABORT:
            /*case OAM_OMP_SLAVE_TO_MASTER:*/
            /*关闭所有socket*/
            Oam_CloseAllVty("\n\r%Communication with RPU losted! \n");
            XOS_SetNextState(PROCTYPE_OAM_TELNETSERVER, S_CommAbort);
            break;

            /* 单板与RPU链路恢复 */
        case EV_COMM_OK:
            break;

        case EV_MASTER_TO_SLAVE:
        case EV_SLAVE_TO_MASTER:
            Oam_CloseAllVty("\n\r%The board was changed over! \n");
            break;
        case OAM_MSG_TO_TELSERVER_OF_CEASE_ATTR:
            Oam_ProcCeaseMsgFromInterpret((BYTE *)pMsgPara);
            break;
        case OAM_MSG_SSH_AUTH: /*  SSH发来的用户登录认证 */
            Oam_ProcSSHAuth(pMsgPara);
            break;
        /*发送缓存重发定时器*/
        case EV_TIMER_CHECK_SENDBUF:
            Oam_SendBufData();
            break;
        default:
            break;
        }
        break;
    case S_CommAbort:
        switch(dwMsgId)
        {/* 单板与RPU链路恢复 */
        case EV_COMM_OK:
            XOS_SetNextState(PROCTYPE_OAM_TELNETSERVER, S_Work);
            break;
        case MSG_FD_CONNECT:/*管理网口新建连接请求*/
        case MSG_FD_RECEIVE:/*管理网口、串口发送数据*/
            Oam_CliVtyCommAbort(pMsgPara);
            break;
        /*~*/
#if 0
        case MSG_BRSSOCKET_MSG:/*业务网口socket消息*/
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
*                     局部函数实现                                        *
**************************************************************************/
/**************************************************************************
* 函数名称：VOID OamTelSvrEntryCheckPtr
* 功能描述：检查函数指针型入参
* 访问的表：无
* 修改的表：无
* 输入参数：
    BYTE *InputPtr
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/6/12    V1.0                 创建
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

        /* 如果入参为空，直接退出 */
        XOS_SetDefaultNextState();
    }
}

/**************************************************************************
* 函数名称：Initialize
* 功能描述：初始化
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
WORD32 Initialize(void)
{
    /*初始化VTY的相关全局变量*/
    Oam_CliVtyInit();
    XOS_SysLog(OAM_CFG_LOG_NOTICE, "Begin to initial socket ...");
    /*初始化socket*/
    oss_telnetdInit();

  //  OamRegConsoleCallback();

    /*注册回调函数*/
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
		
    /*通知VTY,存在新建连接请求*/
    Oam_CliVtyConnect( pMsgPara);
	
    /*主动发送协商请求*/
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
      收到用户信息，先有vty进行分析，
      将分析后的结果发送到具体的server；由server发送给客户端
    */
    Oam_CliVtyRecive(pMsgPara);
                }

void ProcessOSSClose(LPVOID pMsgPara)
            {
    TELNETSVR_VOID_ASSERT(pMsgPara != NULL);
    /*通知vty，关闭vty的相关连接*/
    Oam_CliVtyClose(pMsgPara);	
    return ;
}


