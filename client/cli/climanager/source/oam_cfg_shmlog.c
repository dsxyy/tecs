/**************************************************************************
* ��Ȩ���� (C)2002, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ�oam_cfg_shmlog.c
* �ļ���ʶ��
* ����ժҪ�������ڴ�������־ģ��
* ����˵����
            
* ��ǰ�汾��
* ��    ��       �����
* ������ڣ�20090604
*
* �޸ļ�¼1 ��// �޸���ʷ��¼�������޸����ڡ��޸��߼��޸�����
*    �޸����ڣ�
*    �� �� ��    ��
*    �� �� ��    ��
*    �޸����� :
* �޸ļ�¼2��
**************************************************************************/
/**************************************************************************
*                           ͷ�ļ�                                        *
**************************************************************************/
#include "includes.h"
#include "oam_cfg_common.h"
#include "oam_cfg_shmlog.h"

/**************************************************************************
*                          ����                                           *
**************************************************************************/
/**************************************************************************
*                          ��                                             *
**************************************************************************/
/**************************************************************************
*                          ��������                                       *
**************************************************************************/
/**************************************************************************
*                          ȫ�ֱ���                                       *
**************************************************************************/

/**************************************************************************
*                          ���ر���                                       *
**************************************************************************/
/*��Ϣӳ���ṹ*/
typedef struct tag_T_MsgMap
{
    WORD32 dwMsgId;
    CHAR *pcMsgName;
}_PACKED_1_ T_MsgMap;

/*��Ϣӳ�����������id������*/
T_MsgMap atMsgMap[] =
{
    {EV_OAM_BEGIN,                                     "EV_OAM_BEGIN"},                            
    {EV_OAMCFGBEGIN,                                   "EV_OAMCFGBEGIN"},                          
    {MSG_FD_CONNECT,                                   "MSG_FD_CONNECT"},                          
    {MSG_FD_RECEIVE,                                   "MSG_FD_RECEIVE"},                          
    {MSG_FD_CLOSE,                                     "MSG_FD_CLOSE"},                            
    {OAM_MSG_TELNET_TO_INTERPRET    ,                     "OAM_MSG_TELNET_TO_INTERPRET  "},           
    {OAM_MSG_INTERPRET_TO_TELNET    ,                     "OAM_MSG_INTERPRET_TO_TELNET  "},           
    {OAM_MSG_INTERPRET_TO_EXECUTION,                   "OAM_MSG_INTERPRET_TO_EXECUTION"},          
    {OAM_MSG_EXECUTION_TO_INTERPRET,                   "OAM_MSG_EXECUTION_TO_INTERPRET"},          
    {OAM_MSG_RESTORE_PREPARE_OK,                       "OAM_MSG_RESTORE_PREPARE_OK"},              
    {OAM_MSG_RESTORE_CONTINUE,                         "OAM_MSG_RESTORE_CONTINUE"},                
    {OAM_MSG_SAVE_BEGIN,                               "OAM_MSG_SAVE_BEGIN"},                      
    {EV_TEST_1,                                        "EV_TEST_1"},                               
    {ITEM_TEST_COMM_TIME_SEND,                         "ITEM_TEST_COMM_TIME_SEND"},                
    {OAM_MSG_MS_CFGFILE_SYN,                           "OAM_MSG_MS_CFGFILE_SYN"},                  
    {OAM_MSG_MS_CHANGE_OVER,                           "OAM_MSG_MS_CHANGE_OVER"},                  
    {OAM_MSG_EXECUTION_TO_PROTOCOL,                    "OAM_MSG_EXECUTION_TO_PROTOCOL"},           
    {OAM_MSG_PROTOCOL_TO_EXECUTION,                    "OAM_MSG_PROTOCOL_TO_EXECUTION"},           
    {OAM_MSG_PRO_PING_TO_EXECUTION,                    "OAM_MSG_PRO_PING_TO_EXECUTION"},           
    {OAM_MSG_PRO_TRACE_TO_EXECUTION,                   "OAM_MSG_PRO_TRACE_TO_EXECUTION"},          
    {EV_OAMCLI_APP_REG,                                "EV_OAMCLI_APP_REG"},                       
    {EV_OAMCLI_APP_UNREG,                              "EV_OAMCLI_APP_UNREG"},                     
    {EV_OAMCLI_APP_REGEX,                              "EV_OAMCLI_APP_REGEX"},                     
    {EV_OAMCLI_APP_REG_A2M,                            "EV_OAMCLI_APP_REG_A2M"},                   
    {EV_OAMCLI_APP_REGEX_A2M,                          "EV_OAMCLI_APP_REGEX_A2M"},                 
    {MSG_OFFLINE_CFG_TO_INTERPRET,                     "MSG_OFFLINE_CFG_TO_INTERPRET"},            
    {OAM_CFG_FINISH_ACK,                               "OAM_CFG_FINISH_ACK"},                      
    {EV_OAMCFGEND,                                     "EV_OAMCFGEND"},                            
    {EV_OAM_CFG_CFG,                                   "EV_OAM_CFG_CFG"},                          
    {EV_TXTCFG_REQ,                                    "EV_TXTCFG_REQ"},                           
    {EV_TXTCFG_ACK,                                    "EV_TXTCFG_ACK"},                           
    {EV_XML_OMMCFG_REQ,                                "EV_XML_OMMCFG_REQ"},                       
    {EV_XML_OMMCFG_ACK,                                "EV_XML_OMMCFG_ACK"},                       
    {EV_XML_REGISTER_REQ,                              "EV_XML_REGISTER_REQ"},                     
/*    {EV_XML_REGISTER_ACK,                              "EV_XML_REGISTER_ACK"},                     
    {EV_XML_UNREGISTER_REQ,                            "EV_XML_UNREGISTER_REQ"},                   
    {EV_XML_UNREGISTER_ACK,                            "EV_XML_UNREGISTER_ACK"},                   
    {EV_XML_HANDSHAKE_REQ,                             "EV_XML_HANDSHAKE_REQ"},                    
    {EV_XML_HANDSHAKE_ACK,                             "EV_XML_HANDSHAKE_ACK"},                    
    {OAM_MSG_XMLAGNET_TO_INTERPRET,                    "OAM_MSG_XMLAGNET_TO_INTERPRET"},       
    {EV_SAVETABLE,                                     "EV_SAVETABLE"},                            
    {EV_SAVEOK,                                        "EV_SAVEOK"},                               
    {EV_SAVEFAIL,                                      "EV_SAVEFAIL"},                                                          
    {EV_OAMS_VERINFO_REQ,                              "EV_OAMS_VERINFO_REQ"},                     
    {EV_OAMS_VERINFO_ACK,                              "EV_OAMS_VERINFO_ACK"},                     
    {EV_OAMS_VER_UPDATE_NOTIFY,                        "EV_OAMS_VER_UPDATE_NOTIFY"},                               
    {EV_VER_OAMS_NOTIFYREG_REQ,                        "EV_VER_OAMS_NOTIFYREG_REQ"},               
    {EV_VER_OAMS_NOTIFYREG_ACK,                        "EV_VER_OAMS_NOTIFYREG_ACK"},                                                                        
        {EV_TIMER_1          ,    "EV_TIMER_1"},           
        {EV_TIMER_2          ,    "EV_TIMER_2"},           
        {EV_TIMER_3          ,    "EV_TIMER_3"},           
        {EV_TIMER_4          ,    "EV_TIMER_4"},           
        {EV_TIMER_5          ,    "EV_TIMER_5"},           
        {EV_TIMER_6          ,    "EV_TIMER_6"},           
        {EV_TIMER_7          ,    "EV_TIMER_7"},           
        {EV_TIMER_8          ,    "EV_TIMER_8"},           
        {EV_TIMER_9          ,    "EV_TIMER_9"},           
        {EV_TIMER_10         ,    "EV_TIMER_10 "},         
        {EV_TIMER_11         ,    "EV_TIMER_11 "},         
        {EV_TIMER_12         ,    "EV_TIMER_12 "},         
        {EV_TIMER_13         ,    "EV_TIMER_13 "},         
        {EV_TIMER_14         ,    "EV_TIMER_14 "},         
        {EV_TIMER_15         ,    "EV_TIMER_15 "},         
        {EV_TIMER_16         ,    "EV_TIMER_16 "},         
        {EV_TIMER_17         ,    "EV_TIMER_17 "},         
        {EV_TIMER_18         ,    "EV_TIMER_18 "},         
        {EV_TIMER_19         ,    "EV_TIMER_19 "},         
        {EV_TIMER_20         ,    "EV_TIMER_20 "},         
        {EV_TIMER_21         ,    "EV_TIMER_21 "},         
        {EV_TIMER_22         ,    "EV_TIMER_22 "},         
        {EV_TIMER_23         ,    "EV_TIMER_23 "},         
        {EV_TIMER_24         ,    "EV_TIMER_24 "},         
        {EV_TIMER_25         ,    "EV_TIMER_25 "},         
        {EV_TIMER_26         ,    "EV_TIMER_26 "},         
        {EV_TIMER_27         ,    "EV_TIMER_27 "},         
        {EV_TIMER_28         ,    "EV_TIMER_28 "},         
        {EV_TIMER_29         ,    "EV_TIMER_29 "},         
        {EV_TIMER_30         ,    "EV_TIMER_30 "},         
        {EV_TIMER_31         ,    "EV_TIMER_31 "},         
        {EV_TIMER_32         ,    "EV_TIMER_32"},          
        {EV_MASTER_POWER_ON  ,    "EV_MASTER_POWER_ON "},  
        {EV_SLAVE_POWER_ON   ,    "EV_SLAVE_POWER_ON"},    
        {EV_PRE_CHANGEOVER   ,    "EV_PRE_CHANGEOVER "},    
        {EV_SLAVE_TO_MASTER  ,    "EV_SLAVE_TO_MASTER "},   
        {EV_MASTER_TO_SLAVE  ,    "EV_MASTER_TO_SLAVE "},   
        {EV_OAMCLI_SPECIAL_REG  ,    "EV_OAMCLI_SPECIAL_REG "},*/

};

/**************************************************************************
*                          ȫ�ֺ���ԭ��                                   *
**************************************************************************/


/**************************************************************************
*                          �ֲ�����ԭ��                                   *
**************************************************************************/
static BOOLEAN UnLockLogShm(T_OamCfgLogShm *ptCliRunLog );
static T_OamCfgLogShm* GetLogShmAndLock(void);
static CHAR *GetMsgNameById(WORD32 dwMsgId);
/**************************************************************************
*                         ��������                                               *
**************************************************************************/

/**************************************************************************
* �������ƣ�Oam_CfgWriteShmLog
* ���������������ڴ�д��climanager������־
* ���ʵı���
* �޸ĵı���
* ���������wState -- job ״̬
                                 dwMsgId -- climanager�յ�����Ϣ
* ���������
* �� �� ֵ����
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2009/06/04   V1.0    ���      ����
**************************************************************************/
void Oam_CfgWriteShmLog(WORD16 wState, WORD32 dwMsgId)
{
    T_OamCfgLogShm *ptCliRunLog = NULL;

    ptCliRunLog = GetLogShmAndLock();
    if (!ptCliRunLog)
    {
        XOS_SysLog(OAM_CFG_LOG_DEBUG, "Oam_CfgWriteShmLog: failed to write climanager run log");
        return ;
    }

    /*��־ѭ��ʹ�ã�����±���ڵ���������´�0��ʼ��*/
    if (ptCliRunLog->dwCurIdx >= OAM_MAX_SHM_LOG_NUM)
    {
        ptCliRunLog->dwCurIdx = 0;
    }

    /*��¼������Ϣ*/
    ptCliRunLog->atLogs[ptCliRunLog->dwCurIdx].ucMSState   = XOS_GetBoardMSState();
    ptCliRunLog->atLogs[ptCliRunLog->dwCurIdx].wJobState  = wState;
    ptCliRunLog->atLogs[ptCliRunLog->dwCurIdx].dwMsgId     = dwMsgId;

    ptCliRunLog->dwCurIdx++;
    
    UnLockLogShm(ptCliRunLog);
}

/*****************************************************************************/
/**
@brief ��ʾClimanager����յ�����Ϣ�͵�ʱ��״̬��Ϣ

@li @b ����б�
@verbatim
wCnt :  WORD16���ͣ�ȡֵ��Χ1~1000�������趨��ʾ��Ϣ��Ŀ
@endverbatim

@li @b ����ֵ�б�
@verbatim
��
@endverbatim

@li @b ���÷�Χ��
\n������OMP/CMM2���壬��oammanager������

@li @b ������ʾ��
\n��
 
@li @b ����˵����
\n1����ʾ���ǰ����ʾ���Ǻ��յ�����Ϣ������������֮ǰ�յ�����Ϣ
2����������Ϣ���û�м�ʱά���õ��Ժ���������ʾ��Ϣ������ʾ��Ϣ���

@li @b ���ʾ����
\n
==========CLIMANAGER LOG BEGIN==========
CLI-LOG-0005: MSState = 1, JobState = 1, Msg = EV_TIMER_4
CLI-LOG-0004: MSState = 1, JobState = 1, Msg = EV_TIMER_4
CLI-LOG-0003: MSState = 1, JobState = 1, Msg = EV_TIMER_4
CLI-LOG-0002: MSState = 1, JobState = 1, Msg = EV_TIMER_4
CLI-LOG-0001: MSState = 1, JobState = 1, Msg = EV_TIMER_4
==========CLIMANAGER LOG END============
@li @b �޸ļ�¼��
-#  �� 
*/
/*****************************************************************************/
void OAM_DbgShowCliLog(WORD16 wCnt)
{
    T_OamCfgLogShm *ptCliRunLog = NULL;
    DWORD dwLoop = 0;
    DWORD dwIndex = 0;
    WORD16 wReqCnt = OAM_MAX_SHM_LOG_NUM;

    ptCliRunLog = GetLogShmAndLock();
    if (!ptCliRunLog)
    {
        XOS_SysLog(OAM_CFG_LOG_DEBUG, "Oam_CfgWriteShmLog: failed to write climanager run log");
        return ;
    }

    /*��������������������������ʾ�������
    û������Ҳ��ʾ�������*/
    if ((wCnt == 0) || (wCnt > OAM_MAX_SHM_LOG_NUM))
    {
        wReqCnt = OAM_MAX_SHM_LOG_NUM;
    }
    else
    {
        wReqCnt = wCnt;
    }
    /*
       ptCliRunLog->dwCurIdx��ʾ��ǰ����д���λ�ã�
       ��˷���λ�ô�ptCliRunLog->dwCurIdxǰһ����ʼ
    */
    if (ptCliRunLog->dwCurIdx == 0) 
    {
        dwIndex = OAM_MAX_SHM_LOG_NUM - 1;
    }
    else
    {
        dwIndex = ptCliRunLog->dwCurIdx - 1;
    }
    
    printf("                          [CLIMANAGER LOG]\n");
    printf("Index,  MSState,  JobState,                         MsgId\n");
    printf("=========================================================\n");
    for (dwLoop = 0; dwLoop < wReqCnt; dwLoop++)
    {
        CHAR *pcMsgName = GetMsgNameById(ptCliRunLog->atLogs[dwIndex].dwMsgId);

        if ((ptCliRunLog->atLogs[dwIndex].wJobState == 0) &&
             (ptCliRunLog->atLogs[dwIndex].dwMsgId == 0))
        {
            break;
        }
		
        printf("%5d,%9d,%10d,%30s\n",
                  dwLoop+1,
                  ptCliRunLog->atLogs[dwIndex].ucMSState,
                  ptCliRunLog->atLogs[dwIndex].wJobState,
                  pcMsgName);

        dwIndex = (dwIndex == 0) ? (OAM_MAX_SHM_LOG_NUM - 1) : (dwIndex - 1);
    }
    printf("=========================================================\n");
    
    UnLockLogShm(ptCliRunLog);
}

/**************************************************************************
* �������ƣ�GetLogShmAndLock
* ������������ȡ��־�ڴ��ַ
* ���ʵı���
* �޸ĵı���
* �����������
* �����������
* �� �� ֵ��   NULL -- ��ȡ��ַʧ��
                                ��NULL --��־�ڴ��ַ
* ����˵������Ҫ��UnLockLogShmһ��ʹ��
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2009/06/04   V1.0    ���      ����
**************************************************************************/
T_OamCfgLogShm* GetLogShmAndLock(void)
{
 //   DWORD     dwRet = 0;
 //   WORD32 dwSemid = 0;
    static T_OamCfgLogShm *ptCliRunLog = NULL;
#if 0
    /* �������̻����ź���������JOB��Ϣ���� */
    dwSemid = XOS_CreateProcessSem(OAM_SEM_CLI_LOG, 1, MUTEX_STYLE);
    if(dwSemid == XOS_INVALID_VOSSEM)
    {
        XOS_SysLog(OAM_CFG_LOG_DEBUG, "Oam_GetShmLogAndLock: failed to CreateNameSem , return %u", dwSemid);
        return NULL;
    }

    if (!ptCliRunLog)
    {
        /* ����������־��Ϣ�Ĺ����ڴ� */
        dwRet = XOS_CreateShmByIndex(OAM_SHM_CLI_LOG,
                              sizeof(T_OamCfgLogShm),
                              XOS_RDWR | XOS_CREAT,
                              (BYTE**)&ptCliRunLog);

        if ((XOS_SUCCESS != dwRet) && (XOS_VOS_SHM_EXIST != dwRet))
        {
            XOS_SysLog(OAM_CFG_LOG_DEBUG, "Oam_GetShmLogAndLock: failed to c CreateShm , return %d",dwRet);
            return NULL;
        }

        /*����ǵ�һ�δ�������ǰ�����ڣ���ʼ��Ϊȫ0*/
        if (XOS_SUCCESS == dwRet)
        {
            memset(ptCliRunLog, 0, sizeof(T_OamCfgLogShm));
        }
    }

    ptCliRunLog->dwSemid = dwSemid;
    if (!XOS_ProcessSemP(ptCliRunLog->dwSemid, WAIT_FOREVER))
    {
        XOS_SysLog(OAM_CFG_LOG_DEBUG, "Oam_GetShmLogAndLock: failed to XOS_ProcessSemP ");
        return NULL;
    }
#endif
    return ptCliRunLog;
}

/**************************************************************************
* �������ƣ�VOID UnLockLogShm(VOID)
* ����������ȥ���Թ����ڴ�����־��ַ�ķ�������
* ���ʵı���
* �޸ĵı���
* ���������T_OamCfgLogShm *ptCliRunLog
* ���������
* �� �� ֵ��    ��
* ����˵������Ҫ��GetLogShmAndLockһ��ʹ��
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2009/06/04   V1.0    ���      ����
**************************************************************************/
BOOLEAN UnLockLogShm(T_OamCfgLogShm *ptCliRunLog )
{
    assert(ptCliRunLog != NULL);
    if (!ptCliRunLog)
    {
        return FALSE;
    }

    return XOS_ProcessSemV(ptCliRunLog->dwSemid);
}

/**************************************************************************
* �������ƣ�GetMsgNameById
* ����������������Ϣid��ȡ���ƣ�
           ӳ���û�еĻ���������Ϣid�ַ���
* ���ʵı���
* �޸ĵı���
* ���������dwMsgId : ��Ϣid
* �����������
* �� �� ֵ��    ��Ϣ����
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2009/06/04   V1.0    ���      ����
**************************************************************************/
CHAR *GetMsgNameById(WORD32 dwMsgId)
{
    WORD16 wIdx = 0;
    WORD16 wRecordNum = 0;
    static CHAR aucDefaultRet[15];

    wRecordNum = sizeof(atMsgMap) / sizeof(T_MsgMap);

    for (wIdx = 0; wIdx < wRecordNum; wIdx++)
    {
        if (atMsgMap[wIdx].dwMsgId == dwMsgId)
        {
            break;
        }
    }

    if (wIdx < wRecordNum)
    {
        return atMsgMap[wIdx].pcMsgName;
    }

    memset(aucDefaultRet, 0, sizeof(aucDefaultRet));
    XOS_snprintf(aucDefaultRet, sizeof(aucDefaultRet),"%d", dwMsgId);

    return aucDefaultRet;
}


