/*********************************************************************
* ��Ȩ���� (C)2002, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ�Msg2Exe.h
* �ļ���ʶ��
* ����ժҪ��������Ϣ��Execute
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
#ifndef __MSG_2_EXE_H__
#define __MSG_2_EXE_H__

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

#define RETURNCODE_INVALID  0

/*#define OAM_CLI_DDM_CMDGRP_SET    (WORD32)0xE7000000
#define OAM_CLI_DDM_CMDGRP_GET    (WORD32)0xE8000000*/
#define OAM_CLI_DDM_CMDGRP_GETTABLE    (WORD32)0xEA000000
#define OAM_CLI_CMM_CMDGRP            (WORD32)0x83000000

/* ���Ⱪ¶�ӿ� */
extern void ExecCeaseProtocolCmd(TYPE_LINK_STATE *pLinkState);
extern void ExecCeaseOmmCfgCmd(TYPE_XMLLINKSTATE *pXmlLinkState);
extern void ExecSendCmdAgain(TYPE_LINK_STATE *pLinkState);
extern void DisposeMPCmd(TYPE_LINK_STATE *pLinkState);
extern MSG_COMM_OAM *OamCfgConstructMsgData(TYPE_LINK_STATE *pLinkState, MSG_COMM_OAM *ptMsgBuf);
extern void Oam_Cfg_SendKeepAliveMsgToTelnet(TYPE_LINK_STATE *pLinkState);
extern BOOL CheckMpExecuteMutex(TYPE_LINK_STATE *pLinkState, MSG_COMM_OAM *ptMsg);

#ifdef WIN32
    #pragma pack()
#endif

#endif /* __MSG_2_EXE_H__ */
