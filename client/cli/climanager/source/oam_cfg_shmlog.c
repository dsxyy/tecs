/**************************************************************************
* 版权所有 (C)2002, 深圳市中兴通讯股份有限公司。
*
* 文件名称：oam_cfg_shmlog.c
* 文件标识：
* 内容摘要：共享内存运行日志模块
* 其它说明：
            
* 当前版本：
* 作    者       ：殷浩
* 完成日期：20090604
*
* 修改记录1 ：// 修改历史记录，包括修改日期、修改者及修改内容
*    修改日期：
*    版 本 号    ：
*    修 改 人    ：
*    修改内容 :
* 修改记录2：
**************************************************************************/
/**************************************************************************
*                           头文件                                        *
**************************************************************************/
#include "includes.h"
#include "oam_cfg_common.h"
#include "oam_cfg_shmlog.h"

/**************************************************************************
*                          常量                                           *
**************************************************************************/
/**************************************************************************
*                          宏                                             *
**************************************************************************/
/**************************************************************************
*                          数据类型                                       *
**************************************************************************/
/**************************************************************************
*                          全局变量                                       *
**************************************************************************/

/**************************************************************************
*                          本地变量                                       *
**************************************************************************/
/*消息映射表结构*/
typedef struct tag_T_MsgMap
{
    WORD32 dwMsgId;
    CHAR *pcMsgName;
}_PACKED_1_ T_MsgMap;

/*消息映射表，用来根据id找名称*/
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
*                          全局函数原型                                   *
**************************************************************************/


/**************************************************************************
*                          局部函数原型                                   *
**************************************************************************/
static BOOLEAN UnLockLogShm(T_OamCfgLogShm *ptCliRunLog );
static T_OamCfgLogShm* GetLogShmAndLock(void);
static CHAR *GetMsgNameById(WORD32 dwMsgId);
/**************************************************************************
*                         函数定义                                               *
**************************************************************************/

/**************************************************************************
* 函数名称：Oam_CfgWriteShmLog
* 功能描述：向共享内存写入climanager运行日志
* 访问的表：无
* 修改的表：无
* 输入参数：wState -- job 状态
                                 dwMsgId -- climanager收到的消息
* 输出参数：
* 返 回 值：无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2009/06/04   V1.0    殷浩      创建
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

    /*日志循环使用，如果下标大于等于最大，重新从0开始用*/
    if (ptCliRunLog->dwCurIdx >= OAM_MAX_SHM_LOG_NUM)
    {
        ptCliRunLog->dwCurIdx = 0;
    }

    /*记录运行信息*/
    ptCliRunLog->atLogs[ptCliRunLog->dwCurIdx].ucMSState   = XOS_GetBoardMSState();
    ptCliRunLog->atLogs[ptCliRunLog->dwCurIdx].wJobState  = wState;
    ptCliRunLog->atLogs[ptCliRunLog->dwCurIdx].dwMsgId     = dwMsgId;

    ptCliRunLog->dwCurIdx++;
    
    UnLockLogShm(ptCliRunLog);
}

/*****************************************************************************/
/**
@brief 显示Climanager最近收到的消息和当时的状态信息

@li @b 入参列表：
@verbatim
wCnt :  WORD16类型，取值范围1~1000；用来设定显示消息数目
@endverbatim

@li @b 返回值列表：
@verbatim
无
@endverbatim

@li @b 适用范围：
\n适用于OMP/CMM2单板，在oammanager下运行

@li @b 风险提示：
\n无
 
@li @b 其它说明：
\n1、显示结果前面显示的是后收到的消息，靠后依次是之前收到的消息
2、新增的消息如果没有及时维护该调试函数，不显示消息名，显示消息编号

@li @b 输出示例：
\n
==========CLIMANAGER LOG BEGIN==========
CLI-LOG-0005: MSState = 1, JobState = 1, Msg = EV_TIMER_4
CLI-LOG-0004: MSState = 1, JobState = 1, Msg = EV_TIMER_4
CLI-LOG-0003: MSState = 1, JobState = 1, Msg = EV_TIMER_4
CLI-LOG-0002: MSState = 1, JobState = 1, Msg = EV_TIMER_4
CLI-LOG-0001: MSState = 1, JobState = 1, Msg = EV_TIMER_4
==========CLIMANAGER LOG END============
@li @b 修改记录：
-#  ： 
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

    /*如果请求条数超过最多数量，显示最多数量
    没有输入也显示最多数量*/
    if ((wCnt == 0) || (wCnt > OAM_MAX_SHM_LOG_NUM))
    {
        wReqCnt = OAM_MAX_SHM_LOG_NUM;
    }
    else
    {
        wReqCnt = wCnt;
    }
    /*
       ptCliRunLog->dwCurIdx表示当前可以写入的位置，
       因此访问位置从ptCliRunLog->dwCurIdx前一个开始
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
* 函数名称：GetLogShmAndLock
* 功能描述：获取日志内存地址
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：无
* 返 回 值：   NULL -- 获取地址失败
                                非NULL --日志内存地址
* 其它说明：需要和UnLockLogShm一起使用
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2009/06/04   V1.0    殷浩      创建
**************************************************************************/
T_OamCfgLogShm* GetLogShmAndLock(void)
{
 //   DWORD     dwRet = 0;
 //   WORD32 dwSemid = 0;
    static T_OamCfgLogShm *ptCliRunLog = NULL;
#if 0
    /* 创建进程互斥信号量，用于JOB信息访问 */
    dwSemid = XOS_CreateProcessSem(OAM_SEM_CLI_LOG, 1, MUTEX_STYLE);
    if(dwSemid == XOS_INVALID_VOSSEM)
    {
        XOS_SysLog(OAM_CFG_LOG_DEBUG, "Oam_GetShmLogAndLock: failed to CreateNameSem , return %u", dwSemid);
        return NULL;
    }

    if (!ptCliRunLog)
    {
        /* 创建保存日志信息的共享内存 */
        dwRet = XOS_CreateShmByIndex(OAM_SHM_CLI_LOG,
                              sizeof(T_OamCfgLogShm),
                              XOS_RDWR | XOS_CREAT,
                              (BYTE**)&ptCliRunLog);

        if ((XOS_SUCCESS != dwRet) && (XOS_VOS_SHM_EXIST != dwRet))
        {
            XOS_SysLog(OAM_CFG_LOG_DEBUG, "Oam_GetShmLogAndLock: failed to c CreateShm , return %d",dwRet);
            return NULL;
        }

        /*如果是第一次创建，以前不存在，初始化为全0*/
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
* 函数名称：VOID UnLockLogShm(VOID)
* 功能描述：去除对共享内存中日志地址的访问锁定
* 访问的表：无
* 修改的表：无
* 输入参数：T_OamCfgLogShm *ptCliRunLog
* 输出参数：
* 返 回 值：    无
* 其它说明：需要和GetLogShmAndLock一起使用
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2009/06/04   V1.0    殷浩      创建
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
* 函数名称：GetMsgNameById
* 功能描述：根据消息id获取名称，
           映射表没有的话，返回消息id字符串
* 访问的表：无
* 修改的表：无
* 输入参数：dwMsgId : 消息id
* 输出参数：无
* 返 回 值：    消息名称
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2009/06/04   V1.0    殷浩      创建
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


