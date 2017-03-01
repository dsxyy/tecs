/*********************************************************************
* 版权所有 (C)2002, 深圳市中兴通讯股份有限公司。
*
* 文件名称：oam_fd.c
* 文件标识：
* 内容摘要：OAM回调函数部分
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
* 修改记录2：…
**********************************************************************/
#include "includes.h"

#include "oam_fd.h"
#include "oam_cfg.h"
#include "oam_cfg_common.h"

#include "netinet/in.h"

#define  MCM_SHELL_PWD_LEN              (BYTE)8

JID jidTelnet;
JID jidSSH;
static JID jidHttp;

/* 重新使能shell密码 */
static CHAR *s_acShellPwd = "oamcfg";

static BYTE s_ucInputCount = 0;                                 /* 保存输入字符数 */
static CHAR s_acInputStr[MCM_SHELL_PWD_LEN] = {0};              /* 保存输入字符串 */

/**************************************************************************
* 函数名称：int OAM_FD_Connect
* 功能描述：连接时调用
* 访问的表：无
* 修改的表：无
* 输入参数：WORD16 fd, WORD16 flag
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
int OAM_FD_Connect(WORD16 fd, WORD16 flag, struct sockaddr_in clientAddr)
{
    OAM_FD_MSG msg;
    assert((flag >= CONNECTION_FROM_SERIAL) && (flag <= CONNECTION_FROM_SSH));

    if ((flag < CONNECTION_FROM_SERIAL) || (flag > CONNECTION_FROM_SSH))
    {
        return OAM_ERROR;
    }

    msg.fd    = fd;
    msg.flag  = flag;
    msg.len   = 0;
    memset(msg.buf,0,sizeof(msg.buf));
    memcpy(msg.buf, &clientAddr, sizeof(clientAddr));

    switch(msg.flag)
    {
    case CONNECTION_FROM_SERIAL:
    case CONNECTION_FROM_TELNET:
        OSS_SendFromTask(MSG_FD_CONNECT, (BYTE *)&msg, sizeof(msg),
                         NULL,&jidTelnet);
        break;
    case CONNECTION_FROM_SSH:
        OSS_SendFromTask(MSG_FD_CONNECT, (BYTE *)&msg, sizeof(msg),
                         NULL,&jidSSH);
        break;
    case CONNECTION_FROM_HTTP:
        OSS_SendFromTask(MSG_FD_CONNECT, (BYTE *)&msg, sizeof(msg),
                         NULL,&jidHttp);
        break;
    default:
        return OAM_ERROR;
    }
    return OAM_OK;
}

/**************************************************************************
* 函数名称：int OAM_FD_Receive
* 功能描述：收到数据时调用
* 访问的表：无
* 修改的表：无
* 输入参数：WORD16 fd, WORD16 flag,LPVOID buf, WORD16 len
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
int OAM_FD_Receive(WORD16 fd, WORD16 flag,LPVOID buf, WORD16 len)
{
    OAM_FD_MSG msg;
    WORD16 wPos = 0;

    assert((flag >= CONNECTION_FROM_SERIAL) && (flag <= CONNECTION_FROM_SSH) && 
               (buf) && (len>0));

    if ((flag < CONNECTION_FROM_SERIAL) || (flag > CONNECTION_FROM_SSH) ||
        (!buf) || (len==0))
    {
        return OAM_ERROR;
    }

    msg.fd      = fd;
    msg.flag    = flag;
    while(wPos<len) 
    {
        if ((len-wPos) < MAX_BUF_LEN)
            msg.len   = len-wPos;
        else
            msg.len   = MAX_BUF_LEN;

        memset(msg.buf,0,sizeof(msg.buf));
       /*lint -e124*/ memcpy(msg.buf,(BYTE *)(buf)+wPos, msg.len);/*lint +e124*/
        wPos += msg.len;
        switch(msg.flag)
        {
        case CONNECTION_FROM_SERIAL:
        case CONNECTION_FROM_TELNET:
            OSS_SendFromTask(MSG_FD_RECEIVE, (BYTE *)&msg, sizeof(msg),
                             NULL,&jidTelnet);
            break;
        case CONNECTION_FROM_SSH:
            OSS_SendFromTask(MSG_FD_RECEIVE, (BYTE *)&msg, sizeof(msg),
                             NULL,&jidSSH);
            break;
        case CONNECTION_FROM_HTTP:
            OSS_SendFromTask(MSG_FD_RECEIVE, (BYTE *)&msg, sizeof(msg),
                             NULL,&jidHttp);
            break;
        default:
            return OAM_ERROR;
        }
    }
    return OAM_OK;
}

/**************************************************************************
* 函数名称：int OAM_FD_Close
* 功能描述：连接断开时调用
* 访问的表：无
* 修改的表：无
* 输入参数：WORD16 fd, WORD16 flag
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
int OAM_FD_Close(WORD16 fd, WORD16 flag)
{
    OAM_FD_MSG msg;

    assert((flag >= CONNECTION_TYPE_MIN) && (flag <= CONNECTION_TYPE_MAX));

    if ((flag < CONNECTION_TYPE_MIN) || (flag > CONNECTION_TYPE_MAX))
    {
        return OAM_ERROR;
    }

    msg.fd    = fd;
    msg.flag  = flag;
    msg.len   = 0;
    memset(msg.buf,0,sizeof(msg.buf));

    switch(msg.flag)
    {
    case CONNECTION_FROM_SERIAL:
    case CONNECTION_FROM_TELNET:
        OSS_SendFromTask(MSG_FD_CLOSE, (BYTE *)&msg, sizeof(msg),
                         NULL,&jidTelnet);
        break;
    case CONNECTION_FROM_SSH:
        OSS_SendFromTask(MSG_FD_CLOSE, (BYTE *)&msg, sizeof(msg),
                         NULL,&jidSSH);
        break;
    case CONNECTION_FROM_HTTP:
        OSS_SendFromTask(MSG_FD_CLOSE, (BYTE *)&msg, sizeof(msg),
                         NULL,&jidHttp);
        break;
    default:
        return OAM_ERROR;
    }
    return OAM_OK;
}

/**************************************************************************
* 函数名称：WORD32 OamRegCallback
* 功能描述：向OSS注册回调函数
* 访问的表：无
* 修改的表：无
* 输入参数：
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
WORD32 OamRegCallback(void)
{
    T_OAM_FUNSET tFuns;
    WORD32 dwResult; 

    /* 构造Telnet的JID */
    XOS_GetSelfJID(&jidTelnet);
    jidTelnet.dwJno = XOS_ConstructJNO(PROCTYPE_OAM_TELNETSERVER, 1);
    
    /* 构造SSH的JID */
    XOS_GetSelfJID(&jidSSH);
    jidSSH.dwJno = XOS_ConstructJNO(PROCTYPE_OAM_SSHSERVER, 1);    

    /* 构造Http的JID */
    XOS_GetSelfJID(&jidHttp);
    /*
    jidHttp.dwJno = OSS_ConstructPNO(PROCTYPE_OAM_WEBSERVER, 1);
    */

    tFuns.oamConnect = OAM_FD_Connect;
    tFuns.oamReceive = OAM_FD_Receive;
    tFuns.oamClose   = OAM_FD_Close;

    dwResult = OSS_RegOAMFuncSet(&tFuns);

    if (OAM_REGCALLBACK_SUCCESS == dwResult || OAM_REGCALLBACK_ERROR_CONFLICT == dwResult)
    {
        return OAM_OK;
    }
    else 
    {
        /*lint -e570*/return OAM_ERROR;/*lint +e570*/
    }
}

XOS_STATUS Oam_ShellRecvCallBack(BOOLEAN bEnableUshell, CHAR cInput)
{
    int iRet = 0;
    LPVOID buf = (LPVOID)&cInput;
    /* 注意每次都应该返回输出使能状态，先保存当前的Shell状态 */
    BOOLEAN  bNeedEnableShell = bEnableUshell;

    /* 当前不在ushell下，oam接管着 */
    if (!bEnableUshell)
    {
        /* 输入ctrl+d，则禁止输出，回到ushell下 */
        if (0x4 == cInput)
        {
            bNeedEnableShell = TRUE;
        }
        else
        {
            iRet = OAM_FD_Receive(65535, CONNECTION_FROM_SERIAL, buf, 1);
        }
    }
    /* 当前在ushell下，需要输入oamcfg才能oam接管 */
    else
    {
        /* 当前uShell为禁止输出状态，输入oamcfg，使能输出 */
        if (cInput != '\n')
        {
            if (s_ucInputCount < (MCM_SHELL_PWD_LEN-1))
            {
                s_acInputStr[s_ucInputCount] = cInput;
                s_ucInputCount++;
            }
        }
        else
        {
            s_acInputStr[s_ucInputCount] = '\0';
            if (0 == strcmp(s_acInputStr, s_acShellPwd))
            {
                bNeedEnableShell = FALSE;
            }

            /* 清零静态全局变量 */
            s_ucInputCount = 0;
            memset(s_acInputStr, 0, MCM_SHELL_PWD_LEN);
        }
    }

    if (TRUE == bNeedEnableShell)
    {
        /* 使能Shell输出 */
        return XOS_ERROR_UNKNOWN;
    }

    return XOS_SUCCESS;
}

XOS_STATUS Oam_ShellErrCallBack(BOOLEAN bEnableUshell)
{
    return XOS_SUCCESS;
}

WORD32 OamRegConsoleCallback(void)
{
    T_ShellCallBack     tOam_ShellCallBack;

    memset(&tOam_ShellCallBack, 0, sizeof(T_ShellCallBack));
    tOam_ShellCallBack.pRecvFunc    = Oam_ShellRecvCallBack;
    tOam_ShellCallBack.pErrProcFunc = Oam_ShellErrCallBack;

    if (XOS_SUCCESS != XOS_TakeOverConsole(&tOam_ShellCallBack, TRUE))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "OAM_INIT: XOS_TakeOverConsole Failed!\n");
    }
    else
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "OAM_INIT: XOS_TakeOverConsole Success!\n");
    }

    return XOS_SUCCESS;

}


