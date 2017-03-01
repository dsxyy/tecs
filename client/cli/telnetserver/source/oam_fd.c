/*********************************************************************
* ��Ȩ���� (C)2002, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ�oam_fd.c
* �ļ���ʶ��
* ����ժҪ��OAM�ص���������
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
#include "includes.h"

#include "oam_fd.h"
#include "oam_cfg.h"
#include "oam_cfg_common.h"

#include "netinet/in.h"

#define  MCM_SHELL_PWD_LEN              (BYTE)8

JID jidTelnet;
JID jidSSH;
static JID jidHttp;

/* ����ʹ��shell���� */
static CHAR *s_acShellPwd = "oamcfg";

static BYTE s_ucInputCount = 0;                                 /* ���������ַ��� */
static CHAR s_acInputStr[MCM_SHELL_PWD_LEN] = {0};              /* ���������ַ��� */

/**************************************************************************
* �������ƣ�int OAM_FD_Connect
* ��������������ʱ����
* ���ʵı���
* �޸ĵı���
* ���������WORD16 fd, WORD16 flag
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
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
* �������ƣ�int OAM_FD_Receive
* �����������յ�����ʱ����
* ���ʵı���
* �޸ĵı���
* ���������WORD16 fd, WORD16 flag,LPVOID buf, WORD16 len
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
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
* �������ƣ�int OAM_FD_Close
* �������������ӶϿ�ʱ����
* ���ʵı���
* �޸ĵı���
* ���������WORD16 fd, WORD16 flag
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
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
* �������ƣ�WORD32 OamRegCallback
* ������������OSSע��ص�����
* ���ʵı���
* �޸ĵı���
* ���������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
**************************************************************************/
WORD32 OamRegCallback(void)
{
    T_OAM_FUNSET tFuns;
    WORD32 dwResult; 

    /* ����Telnet��JID */
    XOS_GetSelfJID(&jidTelnet);
    jidTelnet.dwJno = XOS_ConstructJNO(PROCTYPE_OAM_TELNETSERVER, 1);
    
    /* ����SSH��JID */
    XOS_GetSelfJID(&jidSSH);
    jidSSH.dwJno = XOS_ConstructJNO(PROCTYPE_OAM_SSHSERVER, 1);    

    /* ����Http��JID */
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
    /* ע��ÿ�ζ�Ӧ�÷������ʹ��״̬���ȱ��浱ǰ��Shell״̬ */
    BOOLEAN  bNeedEnableShell = bEnableUshell;

    /* ��ǰ����ushell�£�oam�ӹ��� */
    if (!bEnableUshell)
    {
        /* ����ctrl+d�����ֹ������ص�ushell�� */
        if (0x4 == cInput)
        {
            bNeedEnableShell = TRUE;
        }
        else
        {
            iRet = OAM_FD_Receive(65535, CONNECTION_FROM_SERIAL, buf, 1);
        }
    }
    /* ��ǰ��ushell�£���Ҫ����oamcfg����oam�ӹ� */
    else
    {
        /* ��ǰuShellΪ��ֹ���״̬������oamcfg��ʹ����� */
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

            /* ���㾲̬ȫ�ֱ��� */
            s_ucInputCount = 0;
            memset(s_acInputStr, 0, MCM_SHELL_PWD_LEN);
        }
    }

    if (TRUE == bNeedEnableShell)
    {
        /* ʹ��Shell��� */
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


