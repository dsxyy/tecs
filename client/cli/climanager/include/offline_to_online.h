/*********************************************************************
* ��Ȩ���� (C)2002, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ�offline_to_online.h
* �ļ���ʶ��
* ����ժҪ������brs����ת���ߵ�������������(V05.03.80.2_3GPF_TDRNC_003)
* ����˵����
            
* ��ǰ�汾��
* ��    ��       �����
* ������ڣ�20090116
*
* �޸ļ�¼1��// �޸���ʷ��¼�������޸����ڡ��޸��߼��޸�����
*    �޸����ڣ�
*    �� �� �ţ�
*    �� �� �ˣ�
*    �޸����ݣ�
* �޸ļ�¼2����
**********************************************************************/
#ifndef __OFFLINE_TO_ONLINE_H__
#define __OFFLINE_TO_ONLINE_H__

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

/*����ʧ��ʱ���brs�ص�����id*/
#define CMD_OAM_OFFLINE_EXEC_ERROR       0x04000017
#define CMD_ID_OFFLINE_PARSEFAIL          CMD_OAM_OFFLINE_EXEC_ERROR

#define OAM_MAX_OFFLINE_CMD_LEN          1024
typedef struct tagMSG_OFFLINECFG_CMD
{
    BYTE        CmdBuffer[OAM_MAX_OFFLINE_CMD_LEN];
    BYTE        LastData[MAX_STORE_LEN]; 
}T_OfflineCfgCmd,*PMSG_OFFLINECFG_CMD;

/*����ת����ר�õ�linkstate*/
extern TYPE_LINK_STATE g_tLinkStateForOfflineToOnline;

extern void Oam_CfgHandleMSGOffline2Online(TYPE_LINK_STATE *pLinkState, LPVOID pMsgPara);
extern void Oam_CfgHandleMSGOffline2OnlineFinish(void);
extern BOOLEAN Oam_CfgCurLinkIsOffline2Online(TYPE_LINK_STATE *pLinkState);
extern void Oam_CfgGetBrsPidForOffline2Online(JID *ptPid);
extern void Oam_CfgSetLastDataForOffline2Online(MSG_COMM_OAM *ptMsg);
extern BOOLEAN Oam_CfgCurCmdIsOffline2Online(MSG_COMM_OAM *ptMsg);
extern void Oam_CfgGetNextCmdFromBrs(void);

#ifdef WIN32
    #pragma pack()
#endif

#endif /* __OFFLINE_TO_ONLINE_H__*/
 
