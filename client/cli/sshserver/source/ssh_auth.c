
#include "ssh_pub.h"
#include "ssh_session.h"
#include "ssh_string.h"
#include "oam_cfg_common.h"

#define AUTH_METHOD_NONE	"none"
#define AUTH_METHOD_NONE_LEN 4
#define AUTH_METHOD_PUBKEY "publickey"
#define AUTH_METHOD_PUBKEY_LEN 9
#define AUTH_METHOD_PASSWORD "password"
#define AUTH_METHOD_PASSWORD_LEN 8

void HandleMsgServiceRequest(TSSHSession *ptSess,TSSHPack *ptPack)
{
    CHAR * serviceName;
    WORD32 serviceNameLen;
    
    serviceNameLen = *(WORD32 *)(ptPack->pucPayload+sizeof(BYTE));
    serviceNameLen = ntohl(serviceNameLen);
    serviceName = (CHAR *)ptPack->pucPayload+sizeof(BYTE)+sizeof(WORD32);

	if (serviceNameLen == SSH_SERVICE_USERAUTH_LEN && strncmp(SSH_SERVICE_USERAUTH, serviceName, serviceNameLen) == 0) 
	{
		BYTE *pStr;
        WORD32 dwPayloadLen;
		pStr = ptSess->aucSndBuf+sizeof(WORD32)+sizeof(BYTE);
        pStr = ssh_byte_to_buffer(SSH_MSG_SERVICE_ACCEPT,pStr);
        pStr = (BYTE*)ssh_string_to_buffer(SSH_SERVICE_USERAUTH,(CHAR *)pStr);
		dwPayloadLen = (WORDPTR)pStr-(WORDPTR)(ptSess->aucSndBuf)-sizeof(WORD32)-sizeof(BYTE);
        FillPkgHeadLen(dwPayloadLen,ptSess->aucSndBuf);
        SSHSendPkg(ptSess);
	}
    return;
}

void HandleMsgUserAuthRequest(TSSHSession *ptSess,TSSHPack *ptPack)
{
	Tssh_string *ptStr;
	CHAR *username,*servicename, *methodname;
    WORD32 dwUserNameLen,dwServiceNameLen,dwMethodNameLen;
	CHAR *p;
	p = (CHAR *)ptPack->pucPayload+sizeof(BYTE);

    p = ssh_string_from_buffer(&ptStr, p);
    username = ptStr->data;
    dwUserNameLen = ntohl(ptStr->length);

    p = ssh_string_from_buffer(&ptStr, p);
    servicename = ptStr->data;
    dwServiceNameLen = ntohl(ptStr->length);

    p = ssh_string_from_buffer(&ptStr, p);
    methodname = ptStr->data;
    dwMethodNameLen = ntohl(ptStr->length);
    
    /* only handle 'ssh-connection' currently */
	if (dwServiceNameLen != SSH_SERVICE_CONNECTION_LEN
		&& (strncmp(servicename, SSH_SERVICE_CONNECTION,
		SSH_SERVICE_CONNECTION_LEN) != 0))
	{
	    printf("not support service: %s!\n",servicename);
		return;	
		/* TODO - disconnect here */
	}

	/* user wants to know what methods are supported */
	if (dwMethodNameLen == AUTH_METHOD_NONE_LEN && 
        strncmp(methodname, AUTH_METHOD_NONE, AUTH_METHOD_NONE_LEN) == 0) 
	{
		BYTE *pStr;
        WORD32 dwPayloadLen;
		pStr = ptSess->aucSndBuf+sizeof(WORD32)+sizeof(BYTE);
        pStr = ssh_byte_to_buffer(SSH_MSG_USERAUTH_FAILURE,pStr);
        pStr = (BYTE *)ssh_string_to_buffer(AUTH_METHOD_PASSWORD,(CHAR *)pStr);
        pStr = ssh_byte_to_buffer(TRUE,pStr);
		dwPayloadLen = (WORDPTR)pStr-(WORDPTR)(ptSess->aucSndBuf)-sizeof(WORD32)-sizeof(BYTE);
        FillPkgHeadLen(dwPayloadLen,ptSess->aucSndBuf);
        SSHSendPkg(ptSess);
 		return;
    }
    if (dwMethodNameLen == AUTH_METHOD_PASSWORD_LEN &&
        strncmp(methodname, AUTH_METHOD_PASSWORD, AUTH_METHOD_PASSWORD_LEN) == 0) 
	{
        CHAR *password;
        WORD32 dwPassWordLen;
        OAM_FD_MSG tFdMsg={0};
        

        p += 1; /*boolean FALSE*/
        p = ssh_string_from_buffer(&ptStr, p);
        password = ptStr->data;
        dwPassWordLen = ntohl(ptStr->length);

		tFdMsg.fd = ptSess->sockfd;
        tFdMsg.flag = CONNECTION_FROM_SSH;
        memcpy(tFdMsg.buf,username,dwUserNameLen);
        tFdMsg.len = dwUserNameLen;
        memcpy(tFdMsg.buf+tFdMsg.len,"?",1);
        tFdMsg.len += 1;
        memcpy(tFdMsg.buf+tFdMsg.len,password,dwPassWordLen);
        tFdMsg.len += dwPassWordLen;
        
        /*·¢ËÍµ½telnetServer*/
        XOS_SendAsynMsg(OAM_MSG_SSH_AUTH, (BYTE*)&tFdMsg,
                    sizeof(OAM_FD_MSG), 0, XOS_MSG_MEDIUM, &jidTelnet);

        
        #if 0
        OSS_SendAsynMsg(OAM_MSG_SSH_AUTH, (BYTE*)&tFdMsg, sizeof(OAM_FD_MSG),COMM_RELIABLE, &jidTelnet);
        pStr = ptSess->aucSndBuf+sizeof(WORD32)+sizeof(BYTE);
        pStr = ssh_byte_to_buffer(SSH_MSG_USERAUTH_SUCCESS,pStr);
		dwPayloadLen = sizeof(BYTE);
        FillPkgHeadLen(dwPayloadLen,ptSess->aucSndBuf);
        SSHSendPkg(ptSess);
        #endif
		return;
	}
	return;
}


