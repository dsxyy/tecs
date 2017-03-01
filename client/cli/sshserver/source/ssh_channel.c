#include "ssh_session.h"
#include "ssh_string.h"

static int sessionpty(struct ChanSess * chansess,char *msg_body)
{
    char *p;
    Tssh_string *ptStr;
	char *termVar; /*TERM environment variable value */
    WORD32 dwTermVarLen;

    p = ssh_string_from_buffer(&ptStr, msg_body);
    termVar = ptStr->data;
    dwTermVarLen = ntohl(ptStr->length);
    dwTermVarLen = dwTermVarLen>64?64:dwTermVarLen;
    /*lint -e670*/
    memcpy(chansess->term,termVar,dwTermVarLen);
    /*lint +e670*/
    memcpy(&chansess->termc,p,sizeof(WORD32));
    chansess->termc = ntohl(chansess->termc);
    p += sizeof(WORD32);

    memcpy(&chansess->termr,p,sizeof(WORD32));
    chansess->termr = ntohl(chansess->termr);
    p += sizeof(WORD32);

    memcpy(&chansess->termw,p,sizeof(WORD32));
    chansess->termw = ntohl(chansess->termw);
    p += sizeof(WORD32);

    memcpy(&chansess->termh,p,sizeof(WORD32));
    chansess->termh = ntohl(chansess->termh);
    p += sizeof(WORD32);

    return 0;
	
}

/* returns 0 on sucesss, 1 otherwise */
static int sessioncommand(struct Channel *channel, TSSHSession *ptSess)
{
    BYTE *pData;
    WORD32 dwPayloadLen;
    pData = ssh_byte_to_buffer(SSH_MSG_CHANNEL_DATA, ptSess->aucSndBuf+sizeof(WORD32)+sizeof(BYTE));
    pData = ssh_word32_to_buffer(ptSess->ptChannel->remotechan, pData); 
        
    pData = (BYTE *)ssh_string_mem_to_buffer((void *)ptSess->aucPromptBuf,strlen((char *)ptSess->aucPromptBuf),(char *)pData);
	dwPayloadLen = (WORDPTR)pData -(WORDPTR)ptSess->aucSndBuf-sizeof(WORD32)-sizeof(BYTE); 
	FillPkgHeadLen(dwPayloadLen, ptSess->aucSndBuf);
    SSHSendPkg(ptSess);

    memset(ptSess->aucPromptBuf,0,255);
	return 0;
}

static int sessionwinchange(struct ChanSess *chansess)
{
	/*
	chansess->termc = buf_getint(ses[ssh_context->sty_id].payload);
	chansess->termr = buf_getint(ses[ssh_context->sty_id].payload);
	chansess->termw = buf_getint(ses[ssh_context->sty_id].payload);
	chansess->termh = buf_getint(ses[ssh_context->sty_id].payload);
	*/
	/* 2003-12-4 	
	pty_change_window_size(chansess->master, chansess->termr, chansess->termc,
	chansess->termw, chansess->termh);
	*/
	return 0;
}

static TsshChannel* newchannel(TSSHSession *ptSess,unsigned int remotechan, unsigned char type, 
								unsigned int transwindow, unsigned int transmaxpacket) 
{
	
	TsshChannel * newchan;
	newchan = (TsshChannel *)XOS_GetUB(sizeof(TsshChannel));
    memset(newchan ,0,sizeof(TsshChannel));
    newchan->type = type;
	newchan->index = 1;
	
	newchan->remotechan = remotechan;
	newchan->recvdonelen = 0;
	newchan->transwindow = transwindow;
	newchan->transmaxpacket = transmaxpacket;
	
	newchan->recvwindow = RECV_MAXWINDOW;
	newchan->recvmaxpacket = RECV_MAXPACKET;

    newchan->typedata.cmd = newchan->typedata.writebuf;
	
	return newchan;
}


void HandleMsgChannelOpen(TSSHSession *ptSess,TSSHPack *ptPack)
{
    WORD32 dwSendChan,dwTransWindow,dwTransMaxPacket;
    BYTE ucTypeVal;
    Tssh_string *ptStr;
    BYTE *p = ptPack->pucPayload +sizeof(BYTE);
    char *channelType;
    WORD32 dwChannelTypeLen;
	/* get channel type */
    p = (BYTE *)ssh_string_from_buffer(&ptStr, (char *)p);
    channelType = ptStr->data;
	dwChannelTypeLen = ntohl(ptStr->length);
    
	/* get send channel */
	memcpy(&dwSendChan,p,sizeof(WORD32));
	dwSendChan = ntohl(dwSendChan);
	p += sizeof(WORD32);
    
	/* get initial window size */
	memcpy(&dwTransWindow,p,sizeof(WORD32));
	dwTransWindow = ntohl(dwTransWindow);
	p += sizeof(WORD32);
    
	/* get maximum packet size */
	memcpy(&dwTransMaxPacket,p,sizeof(WORD32));
	dwTransMaxPacket = ntohl(dwTransMaxPacket);
	p += sizeof(WORD32);

	if (strncmp(channelType, "session",dwChannelTypeLen) == 0)
	{
		ucTypeVal = CHANNEL_ID_SESSION;
	}
	else
	{
	    printf("not support channelType:%s!\n",channelType);
		/* send failure */
		return;
	}
    if(ptSess->ptChannel == NULL)
    {
	ptSess->ptChannel = newchannel(ptSess,dwSendChan, ucTypeVal, dwTransWindow, dwTransMaxPacket);
	/*send confirmation*/
	{
		BYTE *pData;
        WORD32 dwPayloadLen;
        pData = ssh_byte_to_buffer(SSH_MSG_CHANNEL_OPEN_CONFIRMATION, ptSess->aucSndBuf+sizeof(WORD32)+sizeof(BYTE));
        pData = ssh_word32_to_buffer(dwSendChan, pData); /*recipient channel*/
        pData = ssh_word32_to_buffer(ptSess->ptChannel->index, pData); /*sender channel*/
		pData = ssh_word32_to_buffer(ptSess->ptChannel->recvwindow, pData); /*initial window size*/
		pData = ssh_word32_to_buffer(ptSess->ptChannel->recvmaxpacket, pData); /*maximum packet size*/
		dwPayloadLen = (WORDPTR)pData -(WORDPTR)ptSess->aucSndBuf-sizeof(WORD32)-sizeof(BYTE); 
		FillPkgHeadLen(dwPayloadLen, ptSess->aucSndBuf);
        SSHSendPkg(ptSess);
	}
	#if 0
    /*send window adjust*/
    {
    	BYTE *pData;
        WORD32 dwPayloadLen;
        pData = ssh_byte_to_buffer(SSH_MSG_CHANNEL_WINDOW_ADJUST, ptSess->aucSndBuf+sizeof(WORD32)+sizeof(BYTE));
        pData = ssh_word32_to_buffer(dwSendChan, pData); /*recipient channel*/
        pData = ssh_word32_to_buffer(0xFFFFFF, pData); /*bytes to add*/
		dwPayloadLen = (WORDPTR)pData -(WORDPTR)ptSess->aucSndBuf-sizeof(WORD32)-sizeof(BYTE); 
		FillPkgHeadLen(dwPayloadLen, ptSess->aucSndBuf);
        SSHSendPkg(ptSess);
    }
    #endif
}
    else
    {
        /*
		byte      SSH_MSG_CHANNEL_OPEN_FAILURE
      	uint32    recipient channel
      	uint32    reason code
      	string    description in ISO-10646 UTF-8 encoding [RFC3629]
      	string    language tag [RFC3066]
        */
		/*send confirmation*/
		{
			BYTE *pData;
        	WORD32 dwPayloadLen;
        	pData = ssh_byte_to_buffer(SSH_MSG_CHANNEL_OPEN_FAILURE, ptSess->aucSndBuf+sizeof(WORD32)+sizeof(BYTE));
        	pData = ssh_word32_to_buffer(dwSendChan, pData); /*recipient channel*/
        	pData = ssh_word32_to_buffer(1, pData); /*reason code*/
            pData = (BYTE *)ssh_string_to_buffer("Does not support multi-channel!",(char*)pData);
            pData = (BYTE *)ssh_string_to_buffer("en",(char*)pData);
			dwPayloadLen = (WORDPTR)pData -(WORDPTR)ptSess->aucSndBuf-sizeof(WORD32)-sizeof(BYTE); 
			FillPkgHeadLen(dwPayloadLen, ptSess->aucSndBuf);
        	SSHSendPkg(ptSess);
		}
    }
}

void HandleMsgChannelRequest(TSSHSession *ptSess,TSSHPack *ptPack)
{
	WORD32 dwRecipientChan;
	TsshChanSess *chansess;
	Tssh_string *ptStr;
    char *requestType;
    WORD32 dwRequestTypeLen;
    BYTE ucWantReply;
    BYTE *p = ptPack->pucPayload+sizeof(BYTE);
    
	memcpy(&dwRecipientChan,p,sizeof(WORD32));
    dwRecipientChan = ntohl(dwRecipientChan);
    p += sizeof(WORD32);

    p = (BYTE *)ssh_string_from_buffer(&ptStr, (char *)p);
    requestType = ptStr->data;
	dwRequestTypeLen = ntohl(ptStr->length);

    memcpy(&ucWantReply,p,sizeof(BYTE));
	p += sizeof(BYTE);

	chansess = &(ptSess->ptChannel->typedata);
	if (strncmp(requestType, "pty-req",dwRequestTypeLen) == 0) 
	{
		sessionpty(chansess,(char *)p);
	}
	else if (strncmp(requestType, "shell",dwRequestTypeLen) == 0)
	{
		sessioncommand(ptSess->ptChannel,ptSess);
	}
	else if (strncmp(requestType, "window-change",dwRequestTypeLen) == 0) 
	{
		sessionwinchange(chansess);
	}
	
	if(ucWantReply)
	{
		/* send success */
  		BYTE *pData;
        WORD32 dwPayloadLen;
        pData = ssh_byte_to_buffer(SSH_MSG_CHANNEL_SUCCESS, ptSess->aucSndBuf+sizeof(WORD32)+sizeof(BYTE));
        pData = ssh_word32_to_buffer(ptSess->ptChannel->remotechan, pData); 
		dwPayloadLen = (WORDPTR)pData -(WORDPTR)ptSess->aucSndBuf-sizeof(WORD32)-sizeof(BYTE); 
		FillPkgHeadLen(dwPayloadLen, ptSess->aucSndBuf);
        SSHSendPkg(ptSess);
	}
}

void HandleMsgChannelData(TSSHSession *ptSess, TSSHPack *ptPack)
{
	WORD32 dwRecipientChan;
    Tssh_string *ptStr;
    char *data;
    WORD32 dwDataLen;
    OAM_FD_MSG tOamMsg={0};
    BYTE *p = ptPack->pucPayload+sizeof(BYTE);
	memcpy(&dwRecipientChan,p,sizeof(WORD32));
    dwRecipientChan = ntohl(dwRecipientChan);
    p += sizeof(WORD32);

	p = (BYTE *)ssh_string_from_buffer(&ptStr, (char *)p);
    data = ptStr->data;
	dwDataLen = ntohl(ptStr->length);

    tOamMsg.fd = ptSess->sockfd;
    tOamMsg.flag = CONNECTION_FROM_SSH;
    tOamMsg.len = dwDataLen;
    if(dwDataLen > MAX_BUF_LEN)
    {
        dwDataLen = MAX_BUF_LEN;
    }
    memcpy(tOamMsg.buf,data,dwDataLen);

    XOS_SendAsynMsg(MSG_FD_RECEIVE, (BYTE*)(&tOamMsg),
                    sizeof(OAM_FD_MSG), 0, XOS_MSG_MEDIUM, &jidTelnet);

    #if 0
    memcpy(ptSess->ptChannel->typedata.cmd,data,dwDataLen);
    ptSess->ptChannel->typedata.cmd += dwDataLen;
    
	if((*data == '\r'))
    {
        *(ptSess->ptChannel->typedata.cmd) = '\n';
        ptSess->ptChannel->typedata.cmd++;
		BYTE *pData;
        WORD32 dwPayloadLen;
        pData = ssh_byte_to_buffer(SSH_MSG_CHANNEL_DATA, ptSess->aucSndBuf+sizeof(WORD32)+sizeof(BYTE));
        pData = ssh_word32_to_buffer(ptSess->ptChannel->remotechan, pData); 
        
        pData = ssh_string_mem_to_buffer(ptSess->ptChannel->typedata.writebuf,
            (WORDPTR)ptSess->ptChannel->typedata.cmd- (WORDPTR)ptSess->ptChannel->typedata.writebuf,pData);
		dwPayloadLen = (WORDPTR)pData -(WORDPTR)ptSess->aucSndBuf-sizeof(WORD32)-sizeof(BYTE); 
		FillPkgHeadLen(dwPayloadLen, ptSess->aucSndBuf);
        SSHSendPkg(ptSess);
        memset(ptSess->ptChannel->typedata.writebuf,0,1024);
    }
    else
    {
        /*
    	BYTE *pData;
        WORD32 dwPayloadLen;
        pData = ssh_byte_to_buffer(SSH_MSG_CHANNEL_DATA, ptSess->aucSndBuf+sizeof(WORD32)+sizeof(BYTE));
        pData = ssh_word32_to_buffer(ptSess->ptChannel->remotechan, pData); 
        pData = ssh_string_mem_to_buffer(data,dwDataLen,pData);
        
		dwPayloadLen = (WORDPTR)pData -(WORDPTR)ptSess->aucSndBuf+sizeof(WORD32)+sizeof(BYTE); 
		FillPkgHeadLen(dwPayloadLen, ptSess->aucSndBuf);
        SSHSendPkg(ptSess);
        */
    }
    #endif
}

void HandleMsgChannelWindowAdjust(TSSHSession *ptSess,TSSHPack *ptPack)
{
   	BYTE *pData;
    WORD32 dwPayloadLen;
    WORD32 dwSendChan;
    WORD32 dwAddbytes;
    BYTE *p = ptPack->pucPayload +sizeof(BYTE);
   
		/* get send channel */
		memcpy(&dwSendChan,p,sizeof(WORD32));
		dwSendChan = ntohl(dwSendChan);
		p += sizeof(WORD32);
		
		/* get bytes */
		memcpy(&dwAddbytes,p,sizeof(WORD32));
		dwAddbytes = ntohl(dwAddbytes);
		p += sizeof(WORD32);
		/*
    printf("##########dwSendChan=%d,remotechan=%d,leftbytes=%d\n",dwSendChan,ptSess->ptChannel->remotechan,dwAddbytes);
    */
	  pData = ssh_byte_to_buffer(SSH_MSG_CHANNEL_WINDOW_ADJUST, ptSess->aucSndBuf+sizeof(WORD32)+sizeof(BYTE));
    pData = ssh_word32_to_buffer(ptSess->ptChannel->remotechan, pData); /*recipient channel*/
    pData = ssh_word32_to_buffer(RECV_MAXWINDOW, pData); /*bytes to add*/
		dwPayloadLen = (WORDPTR)pData -(WORDPTR)ptSess->aucSndBuf-sizeof(WORD32)-sizeof(BYTE); 
		FillPkgHeadLen(dwPayloadLen, ptSess->aucSndBuf);
   /*  SSHSendPkg(ptSess); */
}

