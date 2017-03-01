
/*********************************************************************
* ��Ȩ���� (C)2002, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ� BASESOCK.
* �ļ���ʶ�� 
* ����ժҪ�� 
* ����˵���� ��
* ��ǰ�汾�� V1.0
* ��    �ߣ� 
* ������ڣ� 
********************************************************************/

/*******************************************************************
 *                      ͷ�ļ�                             *
********************************************************************/
#include "includes.h"

/*#include "sockets.h"*/
#include "rdtelnet.h"
#include "basesock.h"
#include "oam_fd.h"
#include "telserver.h"
#include "clivty.h"

#include "netinet/in.h"
#include   <arpa/inet.h>

/*******************************************************************
*                      ȫ�ֱ���                                        *
********************************************************************/
extern WORD16 g_wWhileLoopCnt;
/*******************************************************************
*                      ��������                                        *
********************************************************************/
extern void oam_sty_puts(sty *cty, CHAR *text);
/**********************************************************************
* �������ƣ�int CreateListenSocket(WORD16 wPort)
* ��������������֡��Socket
* ���������
* ���������
* �� �� ֵ��
            
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
************************************************************************/

int CreateListenSocket(WORD16 wPort)
{
#if 0
    TYPE_FD *tSocket = NULL;

    tSocket = (TYPE_FD *)XOS_GetUB(sizeof(TYPE_FD));
    if (!tSocket)
    {
        return FAIL_MEMORY_ALLOC;
    }
    tSocket->ucFDType    = FD_TYPE_BRSLISTEN;
    tSocket->ucFd        = 0xFF;
    tSocket->wPort          = wPort;
    *tSocket->szIpAddr      = 0;
    tSocket->next           = NULL;

    /* ����Socket */
    tSocket->ucFd = brs_socket(BRS_AF_INET, BRS_SOCK_STREAM, 0);
    if (tSocket->ucFd <= 0)
    {
        OAM_RETUB(tSocket);
        return FAIL_CREATE_SOCKET;
    }

    /* ��Socket�˿� */
    {
        BRS_SOCKETADDR sockAddr;
        memset(&sockAddr, 0, sizeof(sockAddr));

        sockAddr.sin_family = BRS_AF_INET;
        sockAddr.sin_addr.s_addr = BRS_INADDR_ANY;/*brs_htonl(BRS_INADDR_ANY);*/
        sockAddr.sin_port = wPort; /*brs_htons(wPort);*/
        sockAddr.sin_len = sizeof(BRS_SOCKETADDR);

        if (brs_bind(tSocket->ucFd, (brs_sockaddr *)&sockAddr, sizeof(BRS_SOCKETADDR)) != BRS_SOCKET_OK)
        {
            OAM_RETUB(tSocket);
            return FAIL_AYSN_SELECT;
        }
    }

    /* ����֡�� SOCKET_BACK_LOG*/
    if (brs_listen(tSocket->ucFd, MAX_STY_NUMBER) != BRS_SOCKET_OK)
    {
        OAM_RETUB(tSocket);
        return FAIL_AYSN_SELECT;
    }

    /* ����Socket�б� */
    if (NULL == gtSocketList)
    {
        gtSocketList = gtSocketTail = tSocket;
    }
    else
    {
        gtSocketTail->next = tSocket;
        gtSocketTail = tSocket;
    }
#endif
    return SUCCESS;
}

/**********************************************************************
* �������ƣ�void ProcessSocketCommand(LPVOID pMsgPara)
* ����������ִ��Socket����
* ���������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
************************************************************************/
void ProcessSocketCommand(LPVOID pMsgPara)
{
#ifdef USE_BRS
    BRS_SOCKET_IO_MSG *pSockMsg = pMsgPara;
    /*��ȫ�������ȡ�յ���Ϣ��socket�ṹ*/
    TYPE_FD *tSocket = gtSocketList;
    TELNETSVR_VOID_ASSERT(pMsgPara != NULL);

    WHILE (tSocket)
    {
        LOOP_CHECK();
        if (tSocket->ucFd == pSockMsg->s)
        {
            break;
        }
        tSocket = tSocket->next;
    }
    if (!tSocket)
    {
        CHAR strTmp[] = "\nUnknown socket, please try again\n";
        brs_send(pSockMsg->s, strTmp, strlen(strTmp), 0);
        return;
    }

    switch (pSockMsg->type)
    {
    case BRS_SO_ACCEPTED:
        ProcessSocketAccept(tSocket);
        break;
    case BRS_SO_DATA:
        ProcessSocketRecieve(tSocket);
        break;
    case BRS_SO_CLOSE:
        CloseSocket(tSocket);
        break;
    default:
        break;
    }
#endif
}

/**********************************************************************
* �������ƣ�int ProcessSocketAccept(TYPE_FD *tListenSocket)
* �������������յ����ӵĴ�����
* ���������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
************************************************************************/
int ProcessSocketAccept(TYPE_FD *tListenSocket)
{
#if 0
    TYPE_FD *tSocket = NULL;
    BRS_SOCKETADDR sockAddr;
    int nAddrLen = 0;
    sty    *sty = NULL;
    CHAR strAddressTmp[25] = {0};

    TELNETSVR_0_ASSERT(tListenSocket != NULL);

    tSocket = (TYPE_FD *)XOS_GetUB(sizeof(TYPE_FD));
    if (!tSocket)
    {
        return FAIL_MEMORY_ALLOC;
    }
    switch (tListenSocket->wPort)
    {
    case SOCKET_PORT_TELNET:
        if (AddSty(tSocket)  != ADD_STY_SUCCESS)
        {
            OAM_RETUB(tSocket);
            return FAIL_TELNET_EXIST;
        }
        tSocket->ucFDType = FD_TYPE_BRSTELNET;
        break;
    default:
        tSocket->ucFDType = FD_TYPE_UNKNOW;
        break;
    }
    tSocket->ucFd        = OAM_INVALID_SOCKET;
    tSocket->wPort          = 0;
    *tSocket->szIpAddr      = 0;
    tSocket->next           = NULL;

    nAddrLen = sizeof(sockAddr);
    memset(&sockAddr, 0, sizeof(sockAddr));
    sockAddr.sin_family = BRS_AF_INET;

    tSocket->ucFd = brs_accept(tListenSocket->ucFd, (brs_sockaddr *)&sockAddr, &nAddrLen);
    if (tSocket->ucFd == BRS_INVALID_SOCKET)
    {
        OAM_RETUB(tSocket);
        return FAIL_ACCEPT_SOKCET;
    }

#ifndef VOS_WINNT
    tSocket->wPort = ntohs(sockAddr.sin_port);
#else
    tSocket->wPort = XOS_InvertWord16(sockAddr.sin_port);
#endif
    brs_inet_ntoa(sockAddr.sin_addr.s_addr, strAddressTmp);
    XOS_strncpy(tSocket->szIpAddr, strAddressTmp,sizeof(tSocket->szIpAddr));

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

    switch(tSocket->ucFDType)
    {
    case FD_TYPE_BRSTELNET:
        sty = FindSty(tSocket);
        if(sty)
        {
            make_conn_to_remote_parse(sty);
        }
        break;
    default:
        break;
    }
#endif
    return SUCCESS;
}

/**********************************************************************
* �������ƣ�void ProcessSocketRecieve(TYPE_FD *tSocket)
* �����������յ�����ʱ�Ĵ�����
* ���������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
************************************************************************/
void ProcessSocketRecieve(TYPE_FD *tSocket)
{
#ifdef USE_BRS
    CHAR buffer[RECV_BUFFER_LEN] = {0};
    WORD16 nRecvLen = 0;

    TELNETSVR_VOID_ASSERT(tSocket != NULL);

    switch(tSocket->ucFDType)
    {
    case FD_TYPE_BRSTELNET:
        nRecvLen = brs_recv(tSocket->ucFd, buffer, RECV_BUFFER_LEN, 0);
        ReadTelnet(tSocket, buffer, nRecvLen);
        break;
    default:
        break;
    }
#endif
}

