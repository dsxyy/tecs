/*********************************************************************
* ��Ȩ���� (C)2002, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ�CliVty.h
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
* �޸ļ�¼2����
**********************************************************************/
#ifndef __OAM_CLIVTY_H__
#define __OAM_CLIVTY_H__
#include "oam_cfg_common.h"
/***********************************************************
 *                     ��������                            *
***********************************************************/
#ifndef WIN32
    #ifndef _PACKED_1_
        #define _PACKED_1_ __attribute__ ((packed)) 
    #endif
#else
    #ifndef _PACKED_1_
        #define _PACKED_1_
    #endif
#endif

#ifdef WIN32
    #pragma pack(1)
#endif

extern void Oam_CliVtyInit();
extern void Oam_CliVtyConnect(LPVOID pMsgPara);
extern void Oam_CliVtyRecive(LPVOID pMsgPara);
extern void Oam_CliVtyClose(LPVOID pMsgPara);
extern void Oam_ProcMsgFromInterpret(BYTE* pMsgPara);
extern void Oam_ProcTimerEventFromInterpret(WORD32 wTimerPara);
extern void Oam_CloseAllVty(CHAR *pcReason);
extern void Oam_ProcCeaseMsgFromInterpret(BYTE* pMsgPara);
extern void Oam_ProcSSHAuth(LPVOID pMsgPara);
extern void Oam_SendBufData(void);
extern void Oam_CliVtyCommAbort(LPVOID pMsgPara);
extern int  CloseTelnetConn(sty *cty);
extern BYTE AddSty(TYPE_FD *tSocket);
extern sty *FindSty(TYPE_FD *tSocket);
extern sty *FindStyByFd(FD fd);
extern sty *FindExtraSty(TYPE_FD *tSocket);
extern int oam_close_socket_for_ssh(TYPE_FD *tSocket);
#ifdef WIN32
    #pragma pack()
#endif

#endif /* __CLIVTY_H__ */
