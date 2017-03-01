#include <assert.h>
#include "oam_typedef.h"
#include "pub_addition.h"
#include "ssh_session.h"
#include "ssh_pub.h"
#include "ssh_string.h"
#include "ssh_crypt.h"

static void ReadPacketInit(TSSHSession *ptSess);
void HandleMsgKexInit(TSSHSession *ptSess, TSSHPack *ptPack);
int  HandleMsgDHKexInit(TSSHSession *ptSess,TSSHPack *ptPack);
void HandleMsgNewKeys(TSSHSession *ptSess,TSSHPack *ptPack);
void HandleMsgServiceRequest(TSSHSession *ptSess,TSSHPack *ptPack);
void HandleMsgUserAuthRequest(TSSHSession *ptSess,TSSHPack *ptPack);
void HandleMsgChannelOpen(TSSHSession *ptSess,TSSHPack *ptPack);
void HandleMsgChannelData(TSSHSession *ptSess, TSSHPack *ptPack);
void HandleMsgChannelWindowAdjust(TSSHSession *ptSess, TSSHPack *ptPack);
void HandleMsgChannelRequest(TSSHSession *ptSess,TSSHPack *ptPack);

/* 全局变量 */
const struct dropbear_cipher dropbear_nocipher ={NULL, 16, 8}; 
const struct dropbear_hash dropbear_nohash ={NULL, 16, 0}; /* used initially */
const struct dropbear_cipher dropbear_3des = 
	{&des3_desc, 24, 8};
const struct dropbear_hash dropbear_sha1 = 
	{&sha1_desc, 20, 20};
struct _cipher_descriptor* cipher_descriptor[] = 
{
	&des3_desc,
};
struct _hash_descriptor* hash_descriptor[] = 
{
	&sha1_desc,
};
int debug_trace = 0; /* 调试打印日志开关 */
/*lint -e668*/
/*********************************************************************************/
static void SSHServerPowerOn(void)
{
	TSSHSession *ptSess = g_ptSSHSession;
    int i = 0;
    for(;i<MAX_SSH_USER;i++)
    {
    	InitialSess(ptSess+i);
    }
}

static void RecvFdConn(LPVOID pMsgPara)
{
    TSSHSession *ptSess;
    OAM_FD_MSG *ptOamFdMsg = (OAM_FD_MSG *)pMsgPara;
    if(ptOamFdMsg->fd < 1) return ;
    ptSess = GetSessBySockFd(0);
    if(NULL == ptSess)
    {
        CHAR aucTmp[50] = {0};
        strncpy(aucTmp, LOCAL_IDENT"\r\n", sizeof(aucTmp));
	/* 发送服务器端版本V_S */
        OAM_FD_Send(ptSess->sockfd, CONNECTION_FROM_SSH, aucTmp,(int)strlen(aucTmp));
   //     OAM_FD_Send(ptOamFdMsg->fd, CONNECTION_FROM_SSH, LOCAL_IDENT"\r\n",(WORD16)strlen(LOCAL_IDENT"\r\n"));
        SSHSendDisconnect(SSH_DISCONNECT_TOO_MANY_CONNECTIONS,ptOamFdMsg->fd,"Connnection is full!");
        return;
    }
    /*向telnet发送connect消息*/
    XOS_SendAsynMsg(MSG_FD_CONNECT, (BYTE*)ptOamFdMsg, sizeof(OAM_FD_MSG), 0, XOS_MSG_MEDIUM, &jidTelnet);
    ptSess->sockfd = ptOamFdMsg->fd;
    ptSess->wConnTimer = XOS_SetRelativeTimer(PROCTYPE_OAM_SSHSERVER, TIMER_SSH_GET_CONNACK, DURATION_GET_CONNACK, ptSess->sockfd);
}

static void RecvTelConnAck(LPVOID pMsgPara)
{
	  TSSHSession *ptSess;
    OAM_SSH_TEL_MSG *ptOamMsg = (OAM_SSH_TEL_MSG *)pMsgPara;
    ptSess = GetSessBySockFd(ptOamMsg->fd);
    if(NULL == ptSess)
    {
        TRACE(("RecvTelConnAck(): invalid socket fd!"))
        OAM_FD_Quit(ptOamMsg->fd, CONNECTION_FROM_SSH);
        return;
    }
    if (INVALID_TIMER_ID !=  ptSess->wConnTimer)
    {
        XOS_KillTimerByTimerId(ptSess->wConnTimer);
        ptSess->wConnTimer = INVALID_TIMER_ID;
    }

    CHAR aucTmp[50] = {0};
    strncpy(aucTmp, LOCAL_IDENT"\r\n", sizeof(aucTmp));
	/* 发送服务器端版本V_S */
    OAM_FD_Send(ptSess->sockfd, CONNECTION_FROM_SSH, aucTmp,(int)strlen(aucTmp));
    if(ptOamMsg->result == 0)
    {
		SSHSendDisconnect(SSH_DISCONNECT_HOST_NOT_ALLOWED_TO_CONNECT,ptSess->sockfd,(CHAR *)ptOamMsg->buf);
        ptSess->sockfd = 0;
    }
    else if(ptOamMsg->result == 1)
    {
		ptSess->pRcvData = ptSess->aucRcvBuf;
    	ptSess->pRcvDecryptData = ptSess->aucRcvDecryptBuf;
    	ptSess->pSndData = ptSess->aucSndBuf;
    	ptSess->pSndEncryptData = ptSess->aucSndEncryptBuf;
    	ptSess->tNewKey.recv_algo_crypt = &dropbear_nocipher;
    	ptSess->tNewKey.trans_algo_crypt = &dropbear_nocipher;
    	ptSess->tNewKey.recv_algo_mac = &dropbear_nohash;
    	ptSess->tNewKey.trans_algo_mac = &dropbear_nohash;
    	ptSess->dwRcvSeq = -1;
    	ptSess->dwSndSeq = -1;
    	ptSess->pKexBuf = buf_new(4096);
    }
}

static void RecvConnExpTimer(LPVOID pMsgPara)
{
	TSSHSession *ptSess;
    SOCKET fd = (SOCKET)XOS_GetConstParaOfCurTimer(pMsgPara); 
    ptSess = GetSessBySockFd(fd);
    if(NULL == ptSess)
    {
        OAM_FD_Quit(fd, CONNECTION_FROM_SSH);
        return;
    }
	/* 发送服务器端版本V_S */
    CHAR aucTmp[50] = {0};
    strncpy(aucTmp, LOCAL_IDENT"\r\n", sizeof(aucTmp));
    /* 发送服务器端版本V_S */
    OAM_FD_Send(ptSess->sockfd, CONNECTION_FROM_SSH, aucTmp,(int)strlen(aucTmp));
  //  OAM_FD_Send(ptSess->sockfd, CONNECTION_FROM_SSH, LOCAL_IDENT"\r\n",(int)strlen(LOCAL_IDENT"\r\n"));
    SSHSendDisconnect(SSH_DISCONNECT_HOST_NOT_ALLOWED_TO_CONNECT,ptSess->sockfd,"CLI is busy!");
    OAM_FD_Quit(ptSess->sockfd, CONNECTION_FROM_SSH);
    InitialSess(ptSess);
}

static void RecvIdleTimer(LPVOID pMsgPara)
{
    TSSHSession *ptSess;
    OAM_FD_MSG tOamFdMsg = {0};
    SOCKET fd = (SOCKET)XOS_GetConstParaOfCurTimer(pMsgPara); 
    ptSess = GetSessBySockFd(fd);
    if(NULL == ptSess)
    {
        OAM_FD_Quit(fd, CONNECTION_FROM_SSH);
        return;
    }

	SSHSendDisconnectEncrypt(SSH_DISCONNECT_HOST_NOT_ALLOWED_TO_CONNECT,ptSess,"client login timeout!");

    tOamFdMsg.fd = fd;
    tOamFdMsg.flag = CONNECTION_FROM_SSH;
    tOamFdMsg.len = 0;
    /*向telnet发送close消息*/

    XOS_SendAsynMsg(MSG_FD_CLOSE, (BYTE*)(&tOamFdMsg),
                    sizeof(OAM_FD_MSG), 0, XOS_MSG_MEDIUM, &jidTelnet);

    OAM_FD_Quit(ptSess->sockfd, CONNECTION_FROM_SSH);
    InitialSess(ptSess);
}

static void VersionExchange(TSSHSession *ptSess)
{
    CHAR *p;
    WORD16 wVersionLen;
    CHAR strssh[5] = {0};
    /* 处理客户端版本V_C */
    p = (CHAR *)ptSess->aucRcvBuf;
    memcpy(strssh,p,4);
    if(strcmp(strssh,"SSH-")!=0)
        return;
    /* 检测是否收到\x0D\x0A,可能只有一个 \x0A */
    while(p && *p)
    {
        if(*p!='\x0A')
        {
            p++; /* 遍历到第一个'\n'*/
            continue;
        }
            /* p+=2; (CR and LF excluded)*/
            if(*(p-1) == '\x0D')
        {
                p = p-1;
        }
            wVersionLen = (WORD16)((WORDPTR)p - (WORDPTR)ptSess->aucRcvBuf);
        wVersionLen = (wVersionLen>MAX_VERSION_LENGTH)?MAX_VERSION_LENGTH:wVersionLen;
            memcpy(ptSess->clientVersion,ptSess->aucRcvBuf,wVersionLen);
            memset(ptSess->aucRcvBuf,0,SSH_MAX_PACKAGE_LENGTH);
            ptSess->pRcvData = ptSess->aucRcvBuf;
            ptSess->verHasExchange = 1;
        return;
        }
}
/* Checks the mac in hashbuf, for the data in readbuf. */
static int checkMac(TSSHSession *ptSess, BYTE* macBuf, WORD32 dwPackLen)
{
    BYTE macsize;
    hmac_state hmac;
    BYTE tempbuf[SHA1_HASH_SIZE];
    WORD32 hashsize;
    
    macsize = ptSess->tNewKey.recv_algo_mac->hashsize;

    if (macsize == 0)
    {
        return SSH_OK;
    }

    /* calculate the mac */
    if (t_hmac_init(&hmac,
                    0,
                    ptSess->tNewKey.recvmackey,
                    ptSess->tNewKey.recv_algo_mac->keysize)
            != SSH_OK)
    {
        printf("HMAC error");
    }

    /* sequence number */
    STORE32H(ptSess->dwRcvSeq, tempbuf);
    if (hmac_process(&hmac, tempbuf, 4) != SSH_OK)
    {
        printf("HMAC error");
    }
    
    /*  */
    if (hmac_process(&hmac, (BYTE *)ptSess->aucRcvDecryptBuf, dwPackLen+sizeof(WORD32)) != SSH_OK)
    {
        printf("HMAC error");
    }

    hashsize = sizeof(tempbuf);
    if (hmac_done(&hmac, tempbuf, &hashsize) != SSH_OK) {
        printf("HMAC error");
    }

    /* compare the hash */
    if (memcmp(tempbuf, macBuf, macsize) != 0)
    {
        return SSH_ERR;
    }
    else
    {
        return SSH_OK;
    }
}

static void ReadPacket(TSSHSession *ptSess)
{
    BYTE *p;
    BYTE blockSize;
    BYTE block[MAXBLOCKSIZE] = {0};
    BYTE blockDecrypt[MAXBLOCKSIZE] = {0};
    WORD32 decryptLen ;

    TSSHPack tPack = {0};
    tPack.dwPackLen = *(WORD32*)ptSess->aucRcvDecryptBuf;
    NTOHL(tPack.dwPackLen);
    
    blockSize = ptSess->tNewKey.recv_algo_crypt->blocksize;
    decryptLen = blockSize;
    p = ptSess->aucRcvBuf+blockSize;

    /* 接受一个新的消息,消息序列需要递增 */
    ptSess->dwRcvSeq++;
    if (ptSess->tNewKey.recv_algo_crypt->cipherdesc != NULL)
    {
        /* 解析包(从second block开始，不包括MAC) */
        while(decryptLen < (tPack.dwPackLen+sizeof(WORD32)))
        {
            memcpy(block,p,blockSize);
            if (cbc_decrypt((BYTE*)block, (BYTE*)blockDecrypt,
                            &ptSess->tNewKey.recv_symmetric_struct) != SSH_OK)
            {
                printf("############error decrypting first package!!!\n");
    return;
            }
            memcpy(ptSess->pRcvDecryptData,blockDecrypt,blockSize);
            ptSess->pRcvDecryptData += blockSize;
            decryptLen += blockSize;
            p += blockSize;
        }

        /* 检查 MAC */
        if(checkMac(ptSess,p,tPack.dwPackLen)!=SSH_OK)
            return;
    }
    else
    {
        memcpy(ptSess->pRcvDecryptData,p,ptSess->pRcvData-p);
        ptSess->pRcvDecryptData += (ptSess->pRcvData-p);
    }
    return;
}
static void RecvFdData(LPVOID pMsgPara)
{
    TSSHSession *ptSess;
    OAM_FD_MSG *ptOamFdMsg = (OAM_FD_MSG *)pMsgPara;
    if(ptOamFdMsg->buf == NULL || ptOamFdMsg->len == 0) return;
    if((ptSess=GetSessBySockFd(ptOamFdMsg->fd))== NULL) return;
    if(ptOamFdMsg->len <= (SSH_MAX_PACKAGE_LENGTH-(ptSess->pRcvData-ptSess->aucRcvBuf)))
    	memcpy(ptSess->pRcvData,ptOamFdMsg->buf,ptOamFdMsg->len);
    else
    {
		memset(ptSess->aucRcvBuf,0,SSH_MAX_PACKAGE_LENGTH);
        ptSess->pRcvData = ptSess->aucRcvBuf;
        memset(ptSess->aucRcvDecryptBuf,0,SSH_MAX_PACKAGE_LENGTH);
        ptSess->pRcvDecryptData = ptSess->aucRcvDecryptBuf;
        return ;
    }
    ptSess->pRcvData += ptOamFdMsg->len;
    ptSess->wIdleTimer = XOS_SetRelativeTimer(PROCTYPE_OAM_SSHSERVER, TIMER_SSH_IDLE_CONNACK, DURATION_IDLE_CONNACK, ptSess->sockfd);
    /* 版本交换 */
    if(!ptSess->verHasExchange)
    {
    	VersionExchange(ptSess);
        return ;
    }
    ReadPacketInit(ptSess);
    return ;
}

static void RecvFdClose(LPVOID pMsgPara)
{
    TSSHSession *ptSess;
    OAM_FD_MSG *ptOamFdMsg = (OAM_FD_MSG *)pMsgPara;

    if(ptOamFdMsg->fd < 1)
    {
        return;
    }
    ptSess = GetSessBySockFd(ptOamFdMsg->fd);
    if(NULL == ptSess)
    {
        return;
    }

    /*向telnet发送close消息*/
    XOS_SendAsynMsg(MSG_FD_CLOSE, (BYTE*)ptOamFdMsg,
                    sizeof(OAM_FD_MSG), 0, XOS_MSG_MEDIUM, &jidTelnet);
    /*向客户端发送close消息*/
    OAM_FD_Quit(ptOamFdMsg->fd, CONNECTION_FROM_SSH);
    
    if(ptSess->pKexBuf!=NULL)
    {
		if(ptSess->pKexBuf->data!=NULL)
        {
            if(XOS_RetUB(ptSess->pKexBuf->data))
            {
                ptSess->pKexBuf->data =NULL;
            }
        }
        if(XOS_RetUB((BYTE *)(ptSess->pKexBuf)))
        {
            ptSess->pKexBuf =NULL;
        }
    }
    if(ptSess->ptChannel!=NULL)
    {
        if(XOS_RetUB((BYTE *)(ptSess->ptChannel)))
        {
            ptSess->ptChannel =NULL;
        }
    }
    memset(ptSess,0,sizeof(TSSHSession));
    ptSess->pRcvData = ptSess->aucRcvBuf;
    ptSess->pRcvDecryptData = ptSess->aucRcvDecryptBuf;
    ptSess->pSndData = ptSess->aucSndBuf;
    ptSess->pSndEncryptData = ptSess->aucSndEncryptBuf;
}

static void RecvTelAuthAck(LPVOID pMsgPara)
{
    TSSHSession *ptSess;
    CHAR *pStr;
    WORD32 dwPayloadLen;
    OAM_SSH_TEL_MSG *ptOamMsg = (OAM_SSH_TEL_MSG *)pMsgPara;
    ptSess = GetSessBySockFd(ptOamMsg->fd);
    if(NULL == ptSess)
    {
        OAM_FD_Quit(ptOamMsg->fd, CONNECTION_FROM_SSH);
        return;
    }
    
    if(ptOamMsg->result == 1)
    {
    	/*save prompt string*/
    	memcpy(ptSess->aucPromptBuf,SSH_BANNER,strlen(SSH_BANNER));
    	memcpy(ptSess->aucPromptBuf+strlen(SSH_BANNER),ptOamMsg->buf,ptOamMsg->len);
        pStr = (CHAR *)ptSess->aucSndBuf+sizeof(WORD32)+sizeof(BYTE);
    	pStr = (CHAR *)ssh_byte_to_buffer(SSH_MSG_USERAUTH_SUCCESS,(BYTE *)pStr);
		dwPayloadLen = sizeof(BYTE);
    	FillPkgHeadLen(dwPayloadLen,ptSess->aucSndBuf);
    	SSHSendPkg(ptSess);
    }
    else if(ptOamMsg->result == 0)
    {
        CHAR msg[512] = {0};
        memcpy(msg,ptOamMsg->buf,ptOamMsg->len);
    	/*send banner msg */
    	SSHSendBannerMsg(msg,ptSess);
	    pStr = (CHAR *)ptSess->aucSndBuf+sizeof(WORD32)+sizeof(BYTE);
    	pStr = (CHAR *)ssh_byte_to_buffer(SSH_MSG_USERAUTH_FAILURE,(BYTE *)pStr);
    	pStr = ssh_string_to_buffer("password",(CHAR *)pStr);
    	pStr = (CHAR *)ssh_byte_to_buffer(FALSE,(BYTE *)pStr);
		dwPayloadLen = (WORDPTR)pStr-(WORDPTR)(ptSess->aucSndBuf)-sizeof(WORD32)-sizeof(BYTE);
    	FillPkgHeadLen(dwPayloadLen,ptSess->aucSndBuf);
    	SSHSendPkg(ptSess);
    }
}

static void RecvTelMsg(LPVOID pMsgPara)
{
    CHAR *pStr;
    TSSHSession *ptSess;
    WORD32 dwPayloadLen;
    OAM_FD_MSG_EX *ptOamFdMsg = (OAM_FD_MSG_EX *)pMsgPara;
    /*
    TRACE(("TEL->SSH:fd=%d;flag=%d;len=%d;data=%s",ptOamFdMsg->fd,
               ptOamFdMsg->flag,ptOamFdMsg->len,ptOamFdMsg->buf))
               */
    ptSess = GetSessBySockFd(ptOamFdMsg->fd);
    if((NULL == ptSess)||(NULL == ptSess->ptChannel))
    {
        return;
    }
    
    pStr = (CHAR *)ssh_byte_to_buffer(SSH_MSG_CHANNEL_DATA, ptSess->aucSndBuf+sizeof(WORD32)+sizeof(BYTE));
    pStr = (CHAR *)ssh_word32_to_buffer(ptSess->ptChannel->remotechan, (BYTE *)pStr); 
    pStr = ssh_string_mem_to_buffer(ptOamFdMsg->buf,ptOamFdMsg->len,pStr);
	dwPayloadLen = (WORDPTR)pStr -(WORDPTR)ptSess->aucSndBuf-sizeof(WORD32)-sizeof(BYTE); 
	FillPkgHeadLen(dwPayloadLen, ptSess->aucSndBuf);
    SSHSendPkg(ptSess);
    usleep(100);

    ptSess->ptChannel->recvdonelen += dwPayloadLen;
    if(ptSess->ptChannel->recvdonelen>RECV_MINWINDOW)
    {
		BYTE *pData;
        pData = ssh_byte_to_buffer(SSH_MSG_CHANNEL_WINDOW_ADJUST, ptSess->aucSndBuf+sizeof(WORD32)+sizeof(BYTE));
        pData = ssh_word32_to_buffer(ptSess->ptChannel->remotechan, pData); /*recipient channel*/
        pData = ssh_word32_to_buffer(ptSess->ptChannel->recvdonelen, pData); /*bytes to add*/
		dwPayloadLen = (WORDPTR)pData -(WORDPTR)ptSess->aucSndBuf-sizeof(WORD32)-sizeof(BYTE); 
		FillPkgHeadLen(dwPayloadLen, ptSess->aucSndBuf);
        SSHSendPkg(ptSess);
        ptSess->ptChannel->recvdonelen = 0;
        /*
        printf("#############send SSH_MSG_CHANNEL_WINDOW_ADJUST\n");
        printf("#############transwindow =%d\n",ptSess->ptChannel->transwindow);
        */
    }
    
}

void Oam_SSHServer(WORD16 wState, WORD32 dwMsgId, LPVOID pMsgPara, LPVOID pVar, BOOLEAN byteOrder)
    {
    assert(pMsgPara != NULL);
    if(pMsgPara == NULL)
    {
        TRACE(("pMsgPara is NULL"))
        XOS_SetDefaultNextState();
    }
    g_ptSSHSession = (TSSHSession *)pVar;

    switch(wState)
    {
    case S_Init:
        switch(dwMsgId)
    {
        case EV_SLAVE_POWER_ON:
        case EV_MASTER_POWER_ON:
            SSHServerPowerOn();
            XOS_SetNextState(PROCTYPE_OAM_SSHSERVER, S_Work);
            break;
        default:
            break;
}
        break;
    case S_Work:
        switch(dwMsgId)
            {
            /* 收到连接建立消息 */
        case MSG_FD_CONNECT:
            TRACE(("rcv connect"))
            RecvFdConn(pMsgPara);
            break;
            /* 收到telnet连接应答 */
        case OAM_MSG_SSH_CONN_ACK:
            TRACE(("rcv connect ack"))
            RecvTelConnAck(pMsgPara);
            break;
            /* 与telnet连接超时 */
        case EV_TIMER_GET_CONNACK:
            TRACE(("rcv connect timeout"))
            RecvConnExpTimer(pMsgPara);
            break;
            /* 客户端空闲连接超时 */
        case EV_TIMER_IDLE_CONNACK:
            RecvIdleTimer(pMsgPara);
            break;
            /* 接受数据消息 */
        case MSG_FD_RECEIVE:
            RecvFdData(pMsgPara);
            break;
            /* 收到关闭socket消息 */
        case MSG_FD_CLOSE:
            TRACE(("rcv close"))
            RecvFdClose(pMsgPara);
            break;
        case OAM_MSG_SSH_AUTH_ACK:
            RecvTelAuthAck(pMsgPara);
            break;
        case OAM_MSG_TELNET_TO_SSH:
            RecvTelMsg(pMsgPara);
            break;
        case EV_MASTER_TO_SLAVE:
        case EV_SLAVE_TO_MASTER:
        	  break;
        
        default:
            TRACE(("Unknown Msg: %d!\n",dwMsgId))
            break;
    }
        XOS_SetDefaultNextState();
        break;
    default:
        break;
    }
    XOS_SetDefaultNextState();
}

static void HandleMsgDisconnect(TSSHSession *ptSess,TSSHPack *ptPack)
{
    OAM_FD_MSG tOamFdMsg = {0};
    WORD32 dwReasonCode;
    Tssh_string *ptStr;
    WORD32 dwDesLen;
    CHAR *des;
    BYTE *p = ptPack->pucPayload+sizeof(BYTE);
    /*
      byte      SSH_MSG_DISCONNECT
      uint32    reason code
      string    description in ISO-10646 UTF-8 encoding [RFC3629]
      string    language tag [RFC3066]
    */
    memcpy(&dwReasonCode,p,sizeof(WORD32));
    dwReasonCode = ntohl(dwReasonCode);
    p += sizeof(WORD32);

    p = (BYTE *)ssh_string_from_buffer(&ptStr, (CHAR *)p);
    des = ptStr->data;
	dwDesLen = ntohl(ptStr->length);

    TRACE(("Rcv Disconnect:ReasonCode is %d;Description is %s\n",dwReasonCode,des))

    tOamFdMsg.fd = ptSess->sockfd;
    tOamFdMsg.flag = CONNECTION_FROM_SSH;
	/*向telnet发送close消息*/

    XOS_SendAsynMsg(MSG_FD_CLOSE, (BYTE*)(&tOamFdMsg),
                    sizeof(OAM_FD_MSG), 0, XOS_MSG_MEDIUM, &jidTelnet);
    if(ptSess->pKexBuf!=NULL)
    {
		if(ptSess->pKexBuf->data!=NULL)
        {
            if(XOS_RetUB(ptSess->pKexBuf->data))
            {
                ptSess->pKexBuf->data = NULL;
            }
        }
        if(XOS_RetUB((BYTE *)(ptSess->pKexBuf)))
        {
            ptSess->pKexBuf=NULL;
        }
    }
    if(ptSess->ptChannel!=NULL)
    {
        if(XOS_RetUB((BYTE *)(ptSess->ptChannel)))
        {
            ptSess->ptChannel = NULL;
        }
    }
    memset(ptSess,0,sizeof(TSSHSession));
    ptSess->pRcvData = ptSess->aucRcvBuf;
    ptSess->pRcvDecryptData = ptSess->aucRcvDecryptBuf;
    ptSess->pSndData = ptSess->aucSndBuf;
    ptSess->pSndEncryptData = ptSess->aucSndEncryptBuf;
}



static void oam_sshhandlepacket(TSSHSession *ptSess)
{
    BYTE msgType;
    TSSHPack tPack = {0};
    tPack.dwPackLen = *(WORD32*)ptSess->aucRcvDecryptBuf;
    NTOHL(tPack.dwPackLen);
    tPack.ucPadLen = *(BYTE*)(ptSess->aucRcvDecryptBuf+sizeof(WORD32));
    tPack.pucPayload = ptSess->aucRcvDecryptBuf+sizeof(WORD32)+sizeof(BYTE);
    tPack.pucPadding = tPack.pucPayload + (tPack.dwPackLen-tPack.ucPadLen-1);/* size of payload */
    tPack.pucMAC = tPack.pucPadding + tPack.ucPadLen;
    
    msgType = *(BYTE *)tPack.pucPayload;
    switch(msgType)
    {
    case SSH_MSG_IGNORE:
        TRACE(("Recv SSH_MSG_IGNORE"))
        break;
    case SSH_MSG_DEBUG:
        TRACE(("Recv SSH_MSG_DEBUG"))
        break;
    case SSH_MSG_UNIMPLEMENTED:
        TRACE(("Recv SSH_MSG_UNIMPLEMENTED"))
        break;
    case SSH_MSG_KEXINIT:
        TRACE(("Recv SSH_MSG_KEXINIT"))
        HandleMsgKexInit(ptSess,&tPack);
        break;
    case SSH_MSG_KEXDH_INIT:
        TRACE(("Recv SSH_MSG_KEXDH_INIT"))
        HandleMsgDHKexInit(ptSess,&tPack);
        break;
    case SSH_MSG_NEWKEYS:
        TRACE(("Recv SSH_MSG_NEWKEYS"))
        HandleMsgNewKeys(ptSess,&tPack);
        break;
    case SSH_MSG_SERVICE_REQUEST:
        TRACE(("Recv SSH_MSG_SERVICE_REQUEST"))
        HandleMsgServiceRequest(ptSess,&tPack);
        break;
    case SSH_MSG_USERAUTH_REQUEST:
        TRACE(("Recv SSH_MSG_USERAUTH_REQUEST"))
        HandleMsgUserAuthRequest(ptSess,&tPack);
        break;
    case SSH_MSG_CHANNEL_OPEN:
        TRACE(("Recv SSH_MSG_CHANNEL_OPEN"))
        HandleMsgChannelOpen(ptSess,&tPack);
        break;
    case SSH_MSG_CHANNEL_DATA:
        HandleMsgChannelData(ptSess,&tPack);
        break;
    case SSH_MSG_CHANNEL_WINDOW_ADJUST:
        TRACE(("Recv SSH_MSG_CHANNEL_WINDOW_ADJUST"))
        HandleMsgChannelWindowAdjust(ptSess,&tPack); 
        break;
    case SSH_MSG_CHANNEL_REQUEST:
        TRACE(("Recv SSH_MSG_CHANNEL_REQUEST"))
        HandleMsgChannelRequest(ptSess,&tPack);
        break;
    case SSH_MSG_DISCONNECT:
        TRACE(("Recv SSH_MSG_DISCONNECT"))
        HandleMsgDisconnect(ptSess,&tPack);
        break;
    default:
        TRACE(("Recv Unknown Msg:%d",msgType))
        break;
    }
}

static void ReadPacketInit(TSSHSession *ptSess)
{
    BYTE blockSize;
    BYTE blockHeader[MAXBLOCKSIZE] = {0};
    BYTE blockDecryptHeader[MAXBLOCKSIZE] = {0};
    BYTE tempBuf[512] = {0};
    WORD32 fullPackLen;
    WORD32 leftLen;
    if((ptSess->tNewKey.recv_algo_crypt==NULL)||(ptSess->tNewKey.recv_algo_mac==NULL))
        return;
    blockSize = ptSess->tNewKey.recv_algo_crypt->blocksize;
    WORD32 dwPackSize = *(WORD32 *)ptSess->aucRcvDecryptBuf;
    NTOHL(dwPackSize);
    /* 解析first block */
    if(dwPackSize == 0)
    {
        memcpy(blockHeader,ptSess->aucRcvBuf,blockSize);
        /* 判断是否是加密包，如果是，需要解密 */
        if (ptSess->tNewKey.recv_algo_crypt->cipherdesc != NULL)
        {
            if (cbc_decrypt((BYTE*)blockHeader, (BYTE*)blockDecryptHeader,
                            &ptSess->tNewKey.recv_symmetric_struct) != SSH_OK)
            {
                printf("############error decrypting first package!!!\n");
                return;
            }
        }
        else
            memcpy(blockDecryptHeader,blockHeader,blockSize);

        memcpy(ptSess->aucRcvDecryptBuf,blockDecryptHeader,blockSize);
        ptSess->pRcvDecryptData = ptSess->aucRcvDecryptBuf+blockSize;
        dwPackSize = *(WORD32 *)ptSess->aucRcvDecryptBuf;
        NTOHL(dwPackSize);
    }
        fullPackLen = sizeof(WORD32)+dwPackSize+ptSess->tNewKey.recv_algo_mac->hashsize;
    /*包错误，断开连接*/
    if(fullPackLen > SSH_MAX_PACKAGE_LENGTH)
    {
        SSHSendDisconnect(SSH_DISCONNECT_BY_APPLICATION,ptSess->sockfd,"connection is lost by sshserver!");
        OAM_FD_Quit(ptSess->sockfd, CONNECTION_FROM_SSH);
        InitialSess(ptSess);
        return;
    }
    /* 包还未接收完整 */ /*lint -e574*/
    if((ptSess->pRcvData - ptSess->aucRcvBuf)<fullPackLen) return; /*lint -e574*/
    else
    	leftLen = (ptSess->pRcvData - ptSess->aucRcvBuf)-fullPackLen;
        /* 包已完全接收 */
        ReadPacket(ptSess);
        /* 包已经解密，解析包的结构 */
        oam_sshhandlepacket(ptSess);

        /* 对于linux下的shell客户端，channel-request消息里的 pty-req和session是
        一起发送过来的，需要特殊处理下 */
		
        if(leftLen > 0)
        {
			memcpy(tempBuf,ptSess->aucRcvBuf+fullPackLen,leftLen);
            /* 清空发送接受缓存 */
    		memset(ptSess->aucRcvBuf,0,SSH_MAX_PACKAGE_LENGTH);
    		ptSess->pRcvData = ptSess->aucRcvBuf;
    		memset(ptSess->aucRcvDecryptBuf,0,SSH_MAX_PACKAGE_LENGTH);
    		ptSess->pRcvDecryptData = ptSess->aucRcvDecryptBuf;
    		memset(ptSess->aucSndBuf,0,SSH_MAX_PACKAGE_LENGTH);
    		ptSess->pSndData = ptSess->aucSndBuf;
    		memset(ptSess->aucSndEncryptBuf,0,SSH_MAX_PACKAGE_LENGTH);
            memcpy(ptSess->aucRcvBuf,tempBuf,leftLen);
            ptSess->pRcvData = ptSess->aucRcvBuf+leftLen;
            ReadPacketInit(ptSess);
            
        }
        else
        {
        /* 清空发送接受缓存 */
    	memset(ptSess->aucRcvBuf,0,SSH_MAX_PACKAGE_LENGTH);
    	ptSess->pRcvData = ptSess->aucRcvBuf;
    	memset(ptSess->aucRcvDecryptBuf,0,SSH_MAX_PACKAGE_LENGTH);
    	ptSess->pRcvDecryptData = ptSess->aucRcvDecryptBuf;
    	memset(ptSess->aucSndBuf,0,SSH_MAX_PACKAGE_LENGTH);
    	ptSess->pSndData = ptSess->aucSndBuf;
    	memset(ptSess->aucSndEncryptBuf,0,SSH_MAX_PACKAGE_LENGTH);
        ptSess->pSndEncryptData = ptSess->aucSndEncryptBuf;
    }

      return;
}




BYTE PaddingLength(DWORD payload_size)
{    /* sizeof(DWORD) + sizeof(byte) + payload_size + padding_length 可以被8整除 */
    DWORD x = (payload_size + sizeof(DWORD) + sizeof(BYTE)) & 0x07;
    return 16 - (BYTE)(x);
}
/* 填充packetLen,paddingLen字段 */
void FillPkgHeadLen(WORD32 dwPayloadLen,BYTE *pSndBuf)
{
	BYTE ucPadLen;
    WORD32 dwPkgLen;
    BYTE *p = pSndBuf;
    ucPadLen = PaddingLength(dwPayloadLen);
    dwPkgLen = dwPayloadLen+ucPadLen+sizeof(BYTE);

	p = ssh_word32_to_buffer(dwPkgLen, p);
    p = ssh_byte_to_buffer(ucPadLen, p);
}


extern void EncryptPacket(TSSHSession *ptSess);

void SSHSendPkg(TSSHSession *ptSess)
{
	WORD32 dwPkgLen;
    dwPkgLen = ntohl(*(WORD32 *)ptSess->aucSndBuf);
    ptSess->dwSndSeq++;
    if(ptSess->tNewKey.recv_algo_crypt->cipherdesc != NULL)
	{
		EncryptPacket(ptSess);
	}
	else
	{
		memcpy(ptSess->aucSndEncryptBuf,ptSess->aucSndBuf,sizeof(WORD32)+dwPkgLen);
	}
    OAM_FD_Send(ptSess->sockfd,CONNECTION_FROM_SSH,ptSess->aucSndEncryptBuf,
                sizeof(WORD32)+dwPkgLen+ptSess->tNewKey.trans_algo_mac->hashsize);

    /* 清空发送缓存 */
    memset(ptSess->aucSndBuf,0,SSH_MAX_PACKAGE_LENGTH);
    ptSess->pSndData = ptSess->aucSndBuf;
    memset(ptSess->aucSndEncryptBuf,0,SSH_MAX_PACKAGE_LENGTH);
    ptSess->pSndEncryptData = ptSess->aucSndEncryptBuf;
}

void SSHSendDisconnect(WORD32 dwReasonCode,SOCKET sockfd,CHAR *pstrDesc)
{
    CHAR tempbuf[512]={0};
    CHAR *pStr;
    WORD32 dwPayloadLen,dwPkgLen;
    BYTE ucPadLen;
    pStr = tempbuf+sizeof(WORD32)+sizeof(BYTE);
    pStr = (CHAR *)ssh_byte_to_buffer(SSH_MSG_DISCONNECT,(BYTE *)pStr);
    pStr = (CHAR *)ssh_word32_to_buffer(dwReasonCode, (BYTE *)pStr);
    pStr = ssh_string_to_buffer(pstrDesc,(CHAR *)pStr);
    pStr = ssh_string_to_buffer("en",(CHAR *)pStr);
	dwPayloadLen = (WORDPTR)pStr-(WORDPTR)(tempbuf)-sizeof(WORD32)-sizeof(BYTE);
    ucPadLen = PaddingLength(dwPayloadLen);
    dwPkgLen = dwPayloadLen+ucPadLen+sizeof(BYTE);
    pStr = (CHAR *)ssh_word32_to_buffer(dwPkgLen, (BYTE *)tempbuf);
    pStr = (CHAR *)ssh_byte_to_buffer(ucPadLen, (BYTE *)pStr);
    
    OAM_FD_Send(sockfd,CONNECTION_FROM_SSH,tempbuf,sizeof(WORD32)+dwPkgLen);
}

void SSHSendDisconnectEncrypt(WORD32 dwReasonCode,TSSHSession *ptSess,CHAR *pstrDesc)
{
    CHAR *pStr;
    WORD32 dwPayloadLen,dwPkgLen;
    BYTE ucPadLen;
    pStr = (CHAR *)ptSess->aucSndBuf+sizeof(WORD32)+sizeof(BYTE);
    pStr = (CHAR *)ssh_byte_to_buffer(SSH_MSG_DISCONNECT,(BYTE *)pStr);
    pStr = (CHAR *)ssh_word32_to_buffer(dwReasonCode, (BYTE *)pStr);
    pStr = ssh_string_to_buffer(pstrDesc,(CHAR *)pStr);
    pStr = ssh_string_to_buffer("en",(CHAR *)pStr);
	dwPayloadLen = (WORDPTR)pStr-(WORDPTR)(ptSess->aucSndBuf)-sizeof(WORD32)-sizeof(BYTE);
    ucPadLen = PaddingLength(dwPayloadLen);
    dwPkgLen = dwPayloadLen+ucPadLen+sizeof(BYTE);
    pStr = (CHAR *)ssh_word32_to_buffer(dwPkgLen, (BYTE *)ptSess->aucSndBuf);
    pStr = (CHAR *)ssh_byte_to_buffer(ucPadLen, (BYTE *)pStr);

    FillPkgHeadLen(dwPayloadLen,ptSess->aucSndBuf);
    SSHSendPkg(ptSess);
    /*OAM_FD_Send(sockfd,CONNECTION_FROM_SSH,tempbuf,sizeof(WORD32)+dwPkgLen);*/
}

void SSHSendDebugMsg(CHAR *msg,TSSHSession *ptSess)
{
    CHAR *pStr;
    WORD32 dwPayloadLen;
    pStr = (CHAR *)ptSess->aucSndBuf+sizeof(WORD32)+sizeof(BYTE);
    pStr = (CHAR *)ssh_byte_to_buffer(SSH_MSG_DEBUG,(BYTE *)pStr);
    pStr = (CHAR *)ssh_word32_to_buffer(1, (BYTE *)pStr);/*boolean alwaysdisplay */
    pStr = ssh_string_to_buffer(msg,(CHAR *)pStr);
    pStr = ssh_string_to_buffer("en",(CHAR *)pStr);
	dwPayloadLen = (WORDPTR)pStr-(WORDPTR)(ptSess->aucSndBuf)-sizeof(WORD32)-sizeof(BYTE);
    FillPkgHeadLen(dwPayloadLen,ptSess->aucSndBuf);
    SSHSendPkg(ptSess);
}

void SSHSendBannerMsg(CHAR *msg,TSSHSession *ptSess)
{
    CHAR *pStr;
    WORD32 dwPayloadLen;
    pStr = (CHAR *)ptSess->aucSndBuf+sizeof(WORD32)+sizeof(BYTE);
    pStr = (CHAR *)ssh_byte_to_buffer(SSH_MSG_USERAUTH_BANNER,(BYTE *)pStr);
    pStr = ssh_string_to_buffer(msg,(CHAR *)pStr);
    pStr = ssh_string_to_buffer("en",(CHAR *)pStr);
	dwPayloadLen = (WORDPTR)pStr-(WORDPTR)(ptSess->aucSndBuf)-sizeof(WORD32)-sizeof(BYTE);
    FillPkgHeadLen(dwPayloadLen,ptSess->aucSndBuf);
    SSHSendPkg(ptSess);
}


/******************  调试函数  Begin ***************************/
void dropbear_trace(const CHAR* format, ...)
{

    va_list param;

    if (!debug_trace) {
        return;
    }

    va_start(param, format);
    fprintf(stderr, "TRACE: ");
    vfprintf(stderr, format, param);
    fprintf(stderr, "\n");
    va_end(param);
}

void OAM_OpenDebugTrace()
{
    debug_trace = 1;
}

void OAM_CloseDebugTrace()
{
    debug_trace = 0;
}

/******************  调试函数 End ***************************/

/*lint +e668*/

