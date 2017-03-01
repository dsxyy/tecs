#include "ssh_session.h"
#include "ssh_des.h"



TSSHSession *g_ptSSHSession;

TSSHSession * GetSessBySockFd(SOCKET fd)
{
	TSSHSession *ptSess = g_ptSSHSession;
	int i=0;
    for(;i<MAX_SSH_USER;i++)
    {
		if((ptSess+i)->sockfd == fd)
        {
			return (ptSess+i);
        }
    }
    return NULL;
}

void InitialSess(TSSHSession *ptSession)
{
	memset(ptSession,0,sizeof(TSSHSession));

    ptSession->pRcvData = ptSession->aucRcvBuf;
    ptSession->pRcvDecryptData = ptSession->aucRcvDecryptBuf;
    ptSession->pSndData = ptSession->aucSndBuf;
	ptSession->pSndEncryptData = ptSession->aucSndEncryptBuf;
    ptSession->wConnTimer = INVALID_TIMER_ID;
    ptSession->wIdleTimer = INVALID_TIMER_ID;
}
