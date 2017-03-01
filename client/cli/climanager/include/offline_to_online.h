/*********************************************************************
* 版权所有 (C)2002, 深圳市中兴通讯股份有限公司。
*
* 文件名称：offline_to_online.h
* 文件标识：
* 内容摘要：处理brs离线转在线的配置命令请求(V05.03.80.2_3GPF_TDRNC_003)
* 其它说明：
            
* 当前版本：
* 作    者       ：殷浩
* 完成日期：20090116
*
* 修改记录1：// 修改历史记录，包括修改日期、修改者及修改内容
*    修改日期：
*    版 本 号：
*    修 改 人：
*    修改内容：
* 修改记录2：…
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

/*解析失败时候给brs回的命令id*/
#define CMD_OAM_OFFLINE_EXEC_ERROR       0x04000017
#define CMD_ID_OFFLINE_PARSEFAIL          CMD_OAM_OFFLINE_EXEC_ERROR

#define OAM_MAX_OFFLINE_CMD_LEN          1024
typedef struct tagMSG_OFFLINECFG_CMD
{
    BYTE        CmdBuffer[OAM_MAX_OFFLINE_CMD_LEN];
    BYTE        LastData[MAX_STORE_LEN]; 
}T_OfflineCfgCmd,*PMSG_OFFLINECFG_CMD;

/*离线转在线专用的linkstate*/
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
 
