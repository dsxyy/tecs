/**************************************************************************
* 版权所有 (C)2002, 深圳市中兴通讯股份有限公司。
*
* 文件名称：CliApp.c
* 文件标识：
* 内容摘要：处理内部消息
* 其它说明：
            
* 当前版本：
* 作    者：马俊辉
* 完成日期：2007.6.5
*
* 修改记录1：// 修改历史记录，包括修改日期、修改者及修改内容
*    修改日期：
*    版 本 号：
*    修 改 人：
*    修改内容：
* 修改记录2：
**************************************************************************/
/**************************************************************************
*                           头文件                                        *
**************************************************************************/
#include "includes.h"
#include "xmlagent.h"
#include "linkstate.h"
#include "oam_execute.h"
#include <string.h> 
#include <math.h>
#include "cliapp.h"
extern BOOL bDebugPrint;
 static void Proc_Cmd_InnerApp(LPVOID ptOamMsg,WORD32 dwMsgId,MSG_COMM_OAM *ptRtnMsg);
static  void Proc_Cmd_MssaveFromDb(LPVOID *ptOamMsg,WORD32 dwMsgId,MSG_COMM_OAM *ptRtnMsg);
static void InnerCmdUser(MSG_COMM_OAM *ptOamMsg,WORD32 dwMsgId,MSG_COMM_OAM *ptRtnMsg);
static void InnerCmdSetUserPwd(MSG_COMM_OAM *ptOamMsg,WORD32 dwMsgId,MSG_COMM_OAM *ptRtnMsg);
static void InnerCmdSetUserRole(MSG_COMM_OAM *ptOamMsg,WORD32 dwMsgId,MSG_COMM_OAM *ptRtnMsg);
static void InnerCmdUnlockUser(MSG_COMM_OAM *ptOamMsg,WORD32 dwMsgId,MSG_COMM_OAM *ptRtnMsg);
static void InnerCmdSetSpecIP(MSG_COMM_OAM *ptOamMsg,WORD32 dwMsgId,MSG_COMM_OAM *ptRtnMsg);
static void InnerCmdExecSaveCmd(MSG_COMM_OAM *ptOamMsg,WORD32 dwMsgId,MSG_COMM_OAM *ptRtnMsg);
static void InnerCmdSetTelnetIdleTimeOut(MSG_COMM_OAM *ptOamMsg,WORD32 dwMsgId,MSG_COMM_OAM *ptRtnMsg);
static void InnerCmdSetPasswordintensity(MSG_COMM_OAM *ptOamMsg,WORD32 dwMsgId,MSG_COMM_OAM *ptRtnMsg);
static void InnerCmdSetSimplePasswordLen(MSG_COMM_OAM *ptOamMsg,WORD32 dwMsgId,MSG_COMM_OAM *ptRtnMsg);

static void InnerCmdShowModeStatck(MSG_COMM_OAM *ptOamMsg,WORD32 dwMsgId,MSG_COMM_OAM *ptRtnMsg);
static void InnerCmdShowUserLog(MSG_COMM_OAM *ptOamMsg,WORD32 dwMsgId,MSG_COMM_OAM *ptRtnMsg);
static void InnerCmdShowSpecifyIP(MSG_COMM_OAM *ptOamMsg,WORD32 dwMsgId,MSG_COMM_OAM *ptRtnMsg);
static void InnerCmdShowTelnetIdleTimeOut(MSG_COMM_OAM *ptOamMsg,WORD32 dwMsgId,MSG_COMM_OAM *ptRtnMsg);
static void InnerCmdShowPasswordintensity(MSG_COMM_OAM *ptOamMsg,WORD32 dwMsgId,MSG_COMM_OAM *ptRtnMsg);
static void InnerCmdShowSimplePasswordLen(MSG_COMM_OAM *ptOamMsg,WORD32 dwMsgId,MSG_COMM_OAM *ptRtnMsg);
static void InnerCmdShowOnlineUsers(MSG_COMM_OAM *ptOamMsg,WORD32 dwMsgId,MSG_COMM_OAM *ptRtnMsg);
static void InnerCmdShowUser(MSG_COMM_OAM *ptOamMsg,WORD32 dwMsgId,MSG_COMM_OAM *ptRtnMsg);
static void InnerCmdSetCliLangType(MSG_COMM_OAM *ptOamMsg,WORD32 dwMsgId,MSG_COMM_OAM *ptRtnMsg);
static void InnerCmdGetCliLangInfo(MSG_COMM_OAM *ptOamMsg,WORD32 dwMsgId,MSG_COMM_OAM *ptRtnMsg);

/**************************************************************************
* 函数名称：VOID InnerCmdUser
* 功能描述：处理增加或者删除用户命令
* 访问的表：无
* 修改的表：无
* 输入参数：
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
static void InnerCmdUser(MSG_COMM_OAM *ptOamMsg,WORD32 dwMsgId,MSG_COMM_OAM *ptRtnMsg)
{
    MSG_COMM_OAM *ptSendMsg = ptOamMsg;
    OPR_DATA *pOprData = NULL;
    CHAR  sUserNme[MAX_OAM_USER_NAME_LEN];
    CHAR  sPassWord1[MAX_OAM_USER_PWD_LEN];
    CHAR  sPassWord2[MAX_OAM_USER_PWD_LEN];
    BYTE  ucPassWord1Len=0;
    BYTE  ucPassWord2Len=0;        
    BOOL bIsNoCmd = FALSE;
    WORD16 wRoleValue = 0;
    TYPE_USER_INFO tUserInfo;
    BYTE ucUserConfCnt = 0;
    BYTE ucUserViewCnt = 0;
    BYTE    ucUserRight =0;                            /* 用户权限 */
    static TYPE_CMD_LINE         tCmdLine;                /* 命令行信息 */
    static T_CliCmdLogRecord       tCmdLog;          /* 命令行操作日志 */    
    CHAR szPwd1MD5[MAX_OAM_USER_PWD_ENCRYPT_LEN] = {0};
    BYTE ucPasswordIntensity=0;
    BYTE ucNorPasslesslen=0;    
    memset(&tUserInfo, 0, sizeof(TYPE_USER_INFO));    
    memset(&tCmdLine, 0, sizeof(TYPE_CMD_LINE));
    memset(&tCmdLog, 0, sizeof(T_CliCmdLogRecord));    
    pOprData = ptSendMsg->Data;
    bIsNoCmd = ptSendMsg->bIfNo;
    Oam_GetPasswordIntensity(&ucPasswordIntensity);
    Oam_GetNorPasslesslen(&ucNorPasslesslen);       
    if (bIsNoCmd)
    {
        /*para1: username*/
        memset(sUserNme, 0, MAX_OAM_USER_NAME_LEN);
        memcpy(sUserNme,pOprData->Data,pOprData->Len);
        Oam_LinkGetUserRight((LPVOID)ptOamMsg,dwMsgId,&ucUserRight);
         /* 只有管理员用户才有权限执行此命令 */        
        if (USER_ROLE_ADMIN != ucUserRight)
        {
            AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)NotAdmin);    
            ptRtnMsg->ReturnCode = ERR_AND_HAVEPARA;            
            return;
        }
        if (strcmp(sUserNme, DEFAULT_USER_NAME) == 0)
        {
            AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)CannotDelAdmin);
            ptRtnMsg->ReturnCode = ERR_AND_HAVEPARA;                
            return;
        }    
        memset(&tUserInfo, 0, sizeof(TYPE_USER_INFO));        
        if (!FindUserInfoFromLink(sUserNme, &tUserInfo))
        {
            AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)UserNotExists);    
            ptRtnMsg->ReturnCode = ERR_AND_HAVEPARA;                
            return;
        }    
     LogoutLinkStateToTelnet(sUserNme);    
        DeleteUserInfo(sUserNme);
        if (!WriteUserScriptFile())
        {
            AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)CmdExecFailed);
            ptRtnMsg->ReturnCode = ERR_AND_HAVEPARA;                
            return;        
        }
        else
        {
            AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)CmdExecSucc);
        }
    }
    else
    {
        /*para1: username*/
        memset(sUserNme, 0, MAX_OAM_USER_NAME_LEN);
        memcpy(sUserNme,pOprData->Data,pOprData->Len);
        pOprData = GetNextOprData(pOprData);

        /*para2: password1*/
        memset(sPassWord1, 0, MAX_OAM_USER_PWD_LEN);
     ucPassWord1Len = pOprData->Len-1;        
        memcpy(sPassWord1,pOprData->Data,pOprData->Len);
        pOprData = GetNextOprData(pOprData);        
            
        /*para3: password2*/
        memset(sPassWord2, 0, MAX_OAM_USER_PWD_LEN);
        memcpy(sPassWord2,pOprData->Data,pOprData->Len);
        ucPassWord2Len = pOprData->Len-1;            
        pOprData = GetNextOprData(pOprData);

        /*para4: role*/
        BYTE *pParaBuffer = (BYTE *)(pOprData->Data);
        memcpy(&wRoleValue, pParaBuffer, sizeof(WORD16));
		
        /*********命令行中的密码需要加密之后再写入日志文件，这里做一下处理*********/
        Oam_LinkGetCmdLine((LPVOID)ptOamMsg,dwMsgId,&tCmdLine);
        Oam_LinkGetCmdLog((LPVOID)ptOamMsg,dwMsgId,&tCmdLog);            
        memset(tCmdLog.szCmdLine, 0, sizeof(tCmdLog.szCmdLine));
        XOS_snprintf(tCmdLog.szCmdLine, sizeof(tCmdLog.szCmdLine) - 1, "%s %s %s", 
            tCmdLine.sCmdWord[0], sUserNme, tCmdLine.sCmdWord[4]);
        Oam_LinkSetCmdLog((LPVOID)ptOamMsg,dwMsgId,&tCmdLog);        
        /**************************************************************************/		
        /*当为弱密码时，检查密码长度大于设置的密码长度，
        当为强密码时，检查密码长度必须大于8*/
        if(ucPasswordIntensity == INTENSITY_OAM_STRONGPWD)
        {
            if((ucPassWord1Len <MIN_OAM_USER_PWD_LEN)||(ucPassWord2Len <MIN_OAM_USER_PWD_LEN))
           {
                AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)StrongPasslen);
                ptRtnMsg->ReturnCode = ERR_AND_HAVEPARA;                    
                return;                      
           }
        }
        else
        {
            if((ucPassWord1Len < ucNorPasslesslen) ||(ucPassWord2Len < ucNorPasslesslen))
            {
                AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)SimplePasslen);
                ptRtnMsg->ReturnCode = ERR_AND_HAVEPARA;                        
                return;         
            }
        }        

        /* 只有管理员用户才有权限执行此命令 */
        Oam_LinkGetUserRight((LPVOID)ptOamMsg,dwMsgId,&ucUserRight);        
        if (USER_ROLE_ADMIN != ucUserRight)
        {
            AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)NotAdmin);
            ptRtnMsg->ReturnCode = ERR_AND_HAVEPARA;                    
            return;       
        }

        if (strcmp(sUserNme, DEFAULT_USER_NAME) == 0)
        {
            AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)CannotCreateAdmin);
            ptRtnMsg->ReturnCode = ERR_AND_HAVEPARA;                    
            return;      
        }

        memset(&tUserInfo, 0, sizeof(TYPE_USER_INFO));
        if (FindUserInfoFromLink(sUserNme, &tUserInfo))
        {
            AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)UserDoesExists);
            ptRtnMsg->ReturnCode = ERR_AND_HAVEPARA;                
            return;
        }
        /*强密码时，需要核对是否符合强密码规则*/
     if(ucPasswordIntensity ==INTENSITY_OAM_STRONGPWD)
     {
             if (!CheckRuleOfPwd(sPassWord1))
             {
                 AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)PasswordRule);
                 ptRtnMsg->ReturnCode = ERR_AND_HAVEPARA;                     
                 return;
             }
     }
        if (strcmp(sPassWord1, sPassWord2) != 0)
        {
            AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)TwoInputPwdNotSame);
            ptRtnMsg->ReturnCode = ERR_AND_HAVEPARA;                
            return;       
        }

        /* 不能超过相应权限的最大用户数 */
     GetUserConfViewNum(&ucUserConfCnt,&ucUserViewCnt);
        if ((ucUserConfCnt >= MAX_USER_NUM_ROLE_CONFIG) && 
            (wRoleValue == USER_ROLE_CONFIG))
        {
            AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)ConfUserFull);
            ptRtnMsg->ReturnCode = ERR_AND_HAVEPARA;                
            return;       
        }
        
        if ((ucUserViewCnt >= MAX_USER_NUM_ROLE_VIEW) && 
            (wRoleValue == USER_ROLE_VIEW))
        {
            AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)ViewUserFull);
            ptRtnMsg->ReturnCode = ERR_AND_HAVEPARA;                
            return;      
        }

        /* 参数验证通过，开始写入用户信息管理文件 */
        memset(&tUserInfo, 0, sizeof(TYPE_USER_INFO));
        tUserInfo.ucRole = wRoleValue;
        tUserInfo.bLocked = FALSE;
        XOS_snprintf(tUserInfo.szUserName, sizeof(tUserInfo.szUserName), "%s", sUserNme);
        XOS_snprintf(szPwd1MD5, sizeof(szPwd1MD5), "%s", MDString(sPassWord1));     		
        strncpy(tUserInfo.szPassWord, szPwd1MD5, sizeof(tUserInfo.szPassWord));
        
        if(!AddOneUserToLink(&tUserInfo))
        {
            AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)CmdExecFailed);
            ptRtnMsg->ReturnCode = ERR_AND_HAVEPARA;                
            return;     
        }
        else
        {
            if (!WriteUserScriptFile())
            {
                AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)CmdExecFailed);
                ptRtnMsg->ReturnCode = ERR_AND_HAVEPARA;                    
                return;     
            }
            else
            {
                AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)CmdExecSucc);
            }
        }
    }
    return;
}
/**************************************************************************
* 函数名称：VOID InnerCmdSetPasswordintensity
* 功能描述：设置密码强度
* 访问的表：无
* 修改的表：无
* 输入参数：
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
static void InnerCmdSetPasswordintensity(MSG_COMM_OAM *ptOamMsg,WORD32 dwMsgId,MSG_COMM_OAM *ptRtnMsg)
{
    MSG_COMM_OAM *ptSendMsg = (MSG_COMM_OAM *)ptOamMsg;
    OPR_DATA *pOprData = ptSendMsg->Data;
    WORD16 wValue=0;
    BYTE ucValue=0;
    BYTE    ucUserRight =0;     
    
     /* 只有管理员用户才有权限执行此命令 */    
    Oam_LinkGetUserRight((LPVOID)ptOamMsg,dwMsgId,&ucUserRight);     
    if (USER_ROLE_ADMIN != ucUserRight)
    {
        AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)NotAdmin);
        ptRtnMsg->ReturnCode = ERR_AND_HAVEPARA;            
        return ;
    }    
    memcpy(&wValue,pOprData->Data,sizeof(WORD16));
    ucValue=(BYTE)wValue;    
    Oam_SetPasswordIntensity(&ucValue);    
    if (!WriteUserScriptFile())
    {
        AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)CmdExecFailed);
        ptRtnMsg->ReturnCode = ERR_AND_HAVEPARA;            
        return ;        
    }
    else
    {
        AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)CmdExecSucc);
    }
    return;
}
/**************************************************************************
* 函数名称：VOID InnerCmdShowPasswordintensity
* 功能描述：显示密码强度
* 访问的表：无
* 修改的表：无
* 输入参数：
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
static void InnerCmdShowPasswordintensity(MSG_COMM_OAM *ptOamMsg,WORD32 dwMsgId,MSG_COMM_OAM *ptRtnMsg)
{
    BYTE ucPasswordIntensity =0;    
    Oam_GetPasswordIntensity(&ucPasswordIntensity);
    if(ucPasswordIntensity ==INTENSITY_OAM_STRONGPWD)
    {
        AddMapParaToMsg(ptRtnMsg,7, (WORD16)1);
        return ;    
    }
    else
    {
        AddMapParaToMsg(ptRtnMsg,7, (WORD16)0);
        return ;
    }
    return ;
}
/**************************************************************************
* 函数名称：VOID InnerCmdSetSimplePasswordLen
* 功能描述：设置弱密码的最小长度
* 访问的表：无
* 修改的表：无
* 输入参数：
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
static void InnerCmdSetSimplePasswordLen(MSG_COMM_OAM *ptOamMsg,WORD32 dwMsgId,MSG_COMM_OAM *ptRtnMsg)
{
    MSG_COMM_OAM *ptSendMsg = (MSG_COMM_OAM *)ptOamMsg;
    OPR_DATA *pOprData = ptSendMsg->Data;
    BYTE ucSimplePasswordLen=0;
    BYTE    ucUserRight =0;                            /* 用户权限 */    
    
    Oam_LinkGetUserRight((LPVOID)ptOamMsg,dwMsgId,&ucUserRight);         
     /* 只有管理员用户才有权限执行此命令 */        
    if (USER_ROLE_ADMIN != ucUserRight)
    {
        AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)NotAdmin);
        ptRtnMsg->ReturnCode = ERR_AND_HAVEPARA;            
        return ;
    }    
    memcpy(&ucSimplePasswordLen,pOprData->Data,sizeof(BYTE));
    Oam_SetNorPasslesslen(&ucSimplePasswordLen);    
    if (!WriteUserScriptFile())
    {
        AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)CmdExecFailed);
        ptRtnMsg->ReturnCode = ERR_AND_HAVEPARA;            
        return ;        
    }
    else
    {
        AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)CmdExecSucc);
    }
    return;
}
/**************************************************************************
* 函数名称：VOID InnerCmdShowSimplePasswordLen
* 功能描述：显示弱密码的最小程度
* 访问的表：无
* 修改的表：无
* 输入参数：
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
static void InnerCmdShowSimplePasswordLen(MSG_COMM_OAM *ptOamMsg,WORD32 dwMsgId,MSG_COMM_OAM *ptRtnMsg)
{    
    BYTE ucSimplePasswordLen =0;    
    Oam_GetNorPasslesslen(&ucSimplePasswordLen);
    AddParaToMsg (ptRtnMsg,DATA_TYPE_BYTE,(BYTE *)&ucSimplePasswordLen,sizeof(BYTE));        
    return ;    

}
/**************************************************************************
* 函数名称：VOID InnerCmdShowOnlineUsers
* 功能描述：显示在线用户信息
* 访问的表：无
* 修改的表：无
* 输入参数：
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
static void InnerCmdShowOnlineUsers(MSG_COMM_OAM *ptOamMsg,WORD32 dwMsgId,MSG_COMM_OAM *ptRtnMsg)
{    
    CHAR szDisplay[256] = {0};    
    CHAR *pDisplay = szDisplay;
    BYTE i;    
    BYTE iLoop = 0;/*控制脚本的循环次数*/
    BYTE *piLoopCnt = NULL; /*循环返回的字符串数目*/
    CHAR    tnIpAddr[LEN_IP_ADDR_STR] ={0};              /* 保存Telnet终端IP地址 */
    WORD16 wIdleTimeout=0;                    /* Telnet端空闲超时秒数 */
    CHAR    sUserName[MAX_USERNAME_LEN]={0};       /* 用户名 */    
    
    for (i = MIN_CLI_VTY_NUMBER; i <= MAX_CLI_VTY_NUMBER; i++)
    {
        WORD16 wHour,wMinute,wSecond;
        BYTE k = i - MIN_CLI_VTY_NUMBER;        
        if (Oam_LinkGetCurRunStateByIndex(k) == pec_MsgHandle_IDLE)                        
        {
            continue;
        }
        if(iLoop == 0)
        {
             AddParaToMsg(ptRtnMsg, DATA_TYPE_BYTE, (BYTE *)&iLoop, (BYTE)1);    
             piLoopCnt = (BYTE *)ptRtnMsg->Data[ptRtnMsg->Number - 1].Data;
             iLoop++;
        }
           memset(pDisplay,0,sizeof(szDisplay));        
        if (i == ptRtnMsg->LinkChannel)
        {
            XOS_snprintf(pDisplay, sizeof(szDisplay) - 1,"%s * ",pDisplay);    
        }
        else
        {
             XOS_snprintf(pDisplay, sizeof(szDisplay) - 1,"%s   ",pDisplay);           
        }
        if (i == MIN_CLI_VTY_NUMBER)
        {
            XOS_snprintf(pDisplay, sizeof(szDisplay) - 1, "%s%d  con %d",pDisplay,i,i-MIN_CLI_VTY_NUMBER);        
        }
        else
        {
            XOS_snprintf(pDisplay, sizeof(szDisplay) - 1, "%s%d  vty %d",pDisplay,i,i-MIN_CLI_VTY_NUMBER);                
        }
        AddParaToMsg(ptRtnMsg , DATA_TYPE_STRING,(BYTE *)(pDisplay),strlen((CHAR *)pDisplay)+1);    
        memset(pDisplay,0,sizeof(szDisplay));
        Oam_LinkGetUserNameByIndex(k,sUserName);             
        XOS_snprintf(pDisplay, sizeof(szDisplay) - 1, "%s%s",pDisplay,sUserName);        
        AddParaToMsg(ptRtnMsg , DATA_TYPE_STRING,(BYTE *)(pDisplay),strlen((CHAR *)pDisplay)+1);    

        memset(pDisplay,0,sizeof(szDisplay));
        XOS_snprintf(pDisplay, sizeof(szDisplay) - 1, "%s",pDisplay);        
        AddParaToMsg(ptRtnMsg , DATA_TYPE_STRING,(BYTE *)(pDisplay),strlen((CHAR *)pDisplay)+1);    

        memset(pDisplay,0,sizeof(szDisplay));
        Oam_LinkGetIdleTimeoutByIndex(k,&wIdleTimeout);         
        wHour = wIdleTimeout/60;
        wMinute = wIdleTimeout%60;
        wSecond = 0;
        XOS_snprintf(pDisplay, sizeof(szDisplay) - 1, "%s%02u:%02u:%02u",pDisplay,wHour,wMinute,wSecond);           
        AddParaToMsg(ptRtnMsg , DATA_TYPE_STRING,(BYTE *)(pDisplay),strlen((CHAR *)pDisplay)+1);    

        memset(pDisplay,0,sizeof(szDisplay));
        Oam_LinkGetTnIpAddrByIndex(k,tnIpAddr);
        XOS_snprintf(pDisplay, sizeof(szDisplay) - 1, "%s%s",pDisplay,tnIpAddr);           
        AddParaToMsg(ptRtnMsg , DATA_TYPE_STRING,(BYTE *)(pDisplay),strlen((CHAR *)pDisplay)+1);

        *piLoopCnt += 1;
    }
    return ;    
}
/**************************************************************************
* 函数名称：VOID InnerCmdSetUserPwd
* 功能描述：处理修改用户密码请求
* 访问的表：无
* 修改的表：无
* 输入参数：
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
static void InnerCmdSetUserPwd(MSG_COMM_OAM *ptOamMsg,WORD32 dwMsgId,MSG_COMM_OAM *ptRtnMsg)
{
    MSG_COMM_OAM *ptSendMsg = ptOamMsg;
    OPR_DATA *pOprData = NULL;
    CHAR  sLinkUserName[MAX_USERNAME_LEN];       /* 用户名 */    
    CHAR  sUserName[MAX_OAM_USER_NAME_LEN];
    CHAR  sPassWord1[MAX_OAM_USER_PWD_LEN];
    CHAR  sPassWord2[MAX_OAM_USER_PWD_LEN];
    BYTE  ucPassWord1Len=0;
    BYTE  ucPassWord2Len=0;            
    TYPE_USER_INFO tUserInfo;
    BYTE ucPasswordIntensity=0;
    BYTE ucNorPasslesslen=0;        
    CHAR szPwd1MD5[MAX_OAM_USER_PWD_ENCRYPT_LEN] = {0};
    BYTE    ucUserRight =0;                            /* 用户权限 */
    static TYPE_CMD_LINE         tCmdLine;                /* 命令行信息 */
    static T_CliCmdLogRecord       tCmdLog;          /* 命令行操作日志 */    
    memset(&tCmdLine, 0, sizeof(TYPE_CMD_LINE));
    memset(&tCmdLog, 0, sizeof(T_CliCmdLogRecord));    
    memset(sLinkUserName, 0, MAX_OAM_USER_NAME_LEN);    
    pOprData = ptSendMsg->Data;
    Oam_GetPasswordIntensity(&ucPasswordIntensity);
    Oam_GetNorPasslesslen(&ucNorPasslesslen);               
    /*para1: username*/
    memset(sUserName, 0, MAX_OAM_USER_NAME_LEN);
    memcpy(sUserName,pOprData->Data,pOprData->Len);
    pOprData = GetNextOprData(pOprData);

    /*para2: new password1*/
    memset(sPassWord1, 0, MAX_OAM_USER_PWD_LEN);
    memcpy(sPassWord1,pOprData->Data,pOprData->Len);
    ucPassWord1Len = pOprData->Len-1;    
    pOprData = GetNextOprData(pOprData);

    /*para3: new password2*/
    memset(sPassWord2, 0, MAX_OAM_USER_PWD_LEN);
    memcpy(sPassWord2,pOprData->Data,pOprData->Len);
    ucPassWord2Len = pOprData->Len-1;    
	
    /*********命令行中的密码需要加密之后再写入日志文件，这里做一下处理*********/
    Oam_LinkGetCmdLine((LPVOID)ptOamMsg,dwMsgId,&tCmdLine);
    Oam_LinkGetCmdLog((LPVOID)ptOamMsg,dwMsgId,&tCmdLog);        
    memset(tCmdLog.szCmdLine, 0, sizeof(tCmdLog.szCmdLine));
    XOS_snprintf(tCmdLog.szCmdLine, sizeof(tCmdLog.szCmdLine) - 1, "%s %s", 
        tCmdLine.sCmdWord[0], sUserName);
    Oam_LinkSetCmdLog((LPVOID)ptOamMsg,dwMsgId,&tCmdLog);            
    /**************************************************************************/
	
    /*当为弱密码时，检查密码长度大于设置的密码长度，
       当为强密码时，检查密码长度必须大于8*/
    if(ucPasswordIntensity == INTENSITY_OAM_STRONGPWD)
    {
        if((ucPassWord1Len <MIN_OAM_USER_PWD_LEN)||(ucPassWord2Len <MIN_OAM_USER_PWD_LEN))
        {
            AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)StrongPasslen);
            ptRtnMsg->ReturnCode = ERR_AND_HAVEPARA;                
            return;                      
        }
    }
    else
    {
        if((ucPassWord1Len < ucNorPasslesslen) ||(ucPassWord2Len < ucNorPasslesslen))
        {
            AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)SimplePasslen);
            ptRtnMsg->ReturnCode = ERR_AND_HAVEPARA;                
            return;         
        }
    }    


    /* 查找用户是否存在 */
    memset(&tUserInfo, 0, sizeof(TYPE_USER_INFO));
    if (!FindUserInfoFromLink(sUserName, &tUserInfo))
    {
        AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)UserNotExists);
        ptRtnMsg->ReturnCode = ERR_AND_HAVEPARA;            
        return;
    }        

    /* 如果是配置、查看权限的用户，则只能修改自己的密码 */
    Oam_LinkGetUserRight((LPVOID)ptOamMsg,dwMsgId,&ucUserRight);    
    Oam_LinkGetUserName((LPVOID)ptOamMsg,dwMsgId,sLinkUserName);    
    if (((ucUserRight == USER_ROLE_CONFIG) || (ucUserRight == USER_ROLE_VIEW)) && 
        (strcmp(sLinkUserName, sUserName) != 0))
    {
        AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)CannotSetOtherUserPwd);
        ptRtnMsg->ReturnCode = ERR_AND_HAVEPARA;            
        return;    
    }
    /*强密码时，需要核对是否符合强密码规则*/
    if(ucPasswordIntensity ==INTENSITY_OAM_STRONGPWD)
    {
         if (!CheckRuleOfPwd(sPassWord1))
         {
             AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)PasswordRule);
             ptRtnMsg->ReturnCode = ERR_AND_HAVEPARA;                 
             return;
         }
    }
    if (strcmp(sPassWord1, sPassWord2) != 0)
    {
        AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)TwoInputPwdNotSame);
        ptRtnMsg->ReturnCode = ERR_AND_HAVEPARA;            
        return;        
    }
    
    memset(tUserInfo.szPassWord, 0, MAX_OAM_USER_PWD_ENCRYPT_LEN);
    XOS_snprintf(szPwd1MD5, sizeof(szPwd1MD5), "%s", MDString(sPassWord1));	
    strncpy(tUserInfo.szPassWord,szPwd1MD5, sizeof(tUserInfo.szPassWord));
    
    UpdateOneUserToLink(&tUserInfo);
    if (!WriteUserScriptFile())
    {
        AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)CmdExecFailed);
        ptRtnMsg->ReturnCode = ERR_AND_HAVEPARA;            
        return;        
    }
    else
    {
        AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)CmdExecSucc);
    }
    return;
}
/**************************************************************************
* 函数名称：VOID InnerCmdSetUserRole
* 功能描述：处理修改用户权限请求
* 访问的表：无
* 修改的表：无
* 输入参数：
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
static void InnerCmdSetUserRole(MSG_COMM_OAM *ptOamMsg,WORD32 dwMsgId,MSG_COMM_OAM *ptRtnMsg)
{
    MSG_COMM_OAM *ptSendMsg = ptOamMsg;
    OPR_DATA *pOprData = NULL;
    CHAR  sUserName[MAX_OAM_USER_NAME_LEN];
    WORD16 wRoleValue = 0;
    TYPE_USER_INFO tUserInfo;
    BYTE ucConfigRoleCnt = 0;
    BYTE ucViewRoleCnt = 0;
    BYTE    ucUserRight =0;                            /* 用户权限 */    
    pOprData = ptSendMsg->Data;
    
     /* 只有管理员用户才有权限执行此命令 */        
    Oam_LinkGetUserRight((LPVOID)ptOamMsg,dwMsgId,&ucUserRight);         
    if (USER_ROLE_ADMIN != ucUserRight)
    {
        AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)NotAdmin);
        ptRtnMsg->ReturnCode = ERR_AND_HAVEPARA;            
        return;
    }    
    /*para1: username*/
    memset(sUserName, 0, MAX_OAM_USER_NAME_LEN);
    memcpy(sUserName,pOprData->Data,pOprData->Len);
    pOprData = GetNextOprData(pOprData);

    /*para2: role*/
    BYTE *pParaBuffer = (BYTE *)(pOprData->Data);
    memcpy(&wRoleValue, pParaBuffer, sizeof(WORD16));

    /* 查找用户是否存在 */
    memset(&tUserInfo, 0, sizeof(TYPE_USER_INFO));
    if (!FindUserInfoFromLink(sUserName, &tUserInfo))
    {
        AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)UserNotExists);
        ptRtnMsg->ReturnCode = ERR_AND_HAVEPARA;            
        return;
    }    
    if (strcmp(sUserName, DEFAULT_USER_NAME) == 0)
    {
        AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)CannotSetAdminRole);
        ptRtnMsg->ReturnCode = ERR_AND_HAVEPARA;            
        return;      
    }
    /*在线用户不能设置其权限*/
    if(CheckUsernameBlfOnline(sUserName))
    {
        AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)UserOnline);
        ptRtnMsg->ReturnCode = ERR_AND_HAVEPARA;            
        return;       
    }
    if(tUserInfo.ucRole == wRoleValue)
    {
        AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)NeedNotSetRole);
        ptRtnMsg->ReturnCode = ERR_AND_HAVEPARA;            
        return;        
    }

     /* 权限最大个数校验 */
    GetUserConfViewNum(&ucConfigRoleCnt,&ucViewRoleCnt);            
    if ((wRoleValue == USER_ROLE_CONFIG) && (ucConfigRoleCnt >= MAX_USER_NUM_ROLE_CONFIG))
    {
        AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)ConfUserFull);
        ptRtnMsg->ReturnCode = ERR_AND_HAVEPARA;            
        return;     
    }
    
    if ((wRoleValue == USER_ROLE_VIEW) && (ucViewRoleCnt >= MAX_USER_NUM_ROLE_VIEW))
    {
        AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)ViewUserFull);
        ptRtnMsg->ReturnCode = ERR_AND_HAVEPARA;            
        return;      
    }
    tUserInfo.ucRole = wRoleValue;
    UpdateOneUserToLink(&tUserInfo);
    if (!WriteUserScriptFile())
    {
        AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)CmdExecFailed);
        ptRtnMsg->ReturnCode = ERR_AND_HAVEPARA;            
        return;        
    }
    else
    {
        AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)CmdExecSucc);
    }       
    return;
}
/**************************************************************************
* 函数名称：VOID InnerCmdShowUser
* 功能描述：处理显示用户信息
* 访问的表：无
* 修改的表：无
* 输入参数：
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
static void InnerCmdShowUser(MSG_COMM_OAM *ptOamMsg,WORD32 dwMsgId,MSG_COMM_OAM *ptRtnMsg)    
{ 
    WORD16 i = 0;
    CHAR szDisplay[512] = {0};
    CHAR *pDisplay = szDisplay;    
    CHAR aucRole[10]={0};
    CHAR aucLock[5]={0};
    int Number=0;/*显示的序列号*/
    BYTE iLoop = 1;/*控制脚本的循环次数*/
    BYTE *piLoopCnt = NULL; /*循环返回的字符串数目*/    
    TYPE_USER_INFO UserInfo[MAX_USER_NUM_ROLE_ADMIN + MAX_USER_NUM_ROLE_CONFIG + MAX_USER_NUM_ROLE_VIEW] = {{0}};
    if(!Oam_GetUserInfo(UserInfo))
    {
        AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)UserNotExists);
        ptRtnMsg->ReturnCode = ERR_AND_HAVEPARA;            
        return ;   

    }
    AddParaToMsg(ptRtnMsg, DATA_TYPE_BYTE, (BYTE *)&iLoop, (BYTE)1);    
    piLoopCnt = (BYTE *)ptRtnMsg->Data[ptRtnMsg->Number - 1].Data;            
    memset(pDisplay, 0, sizeof(szDisplay));
    strncpy(pDisplay, "No UserName                        Role    Locked\n", sizeof(szDisplay));
    AddParaToMsg (ptRtnMsg,DATA_TYPE_STRING,(BYTE *)pDisplay,strlen((CHAR *)pDisplay)+1);    
    for(i=0;i<sizeof(UserInfo)/sizeof(TYPE_USER_INFO);i++)
    {
        memset(pDisplay, 0, sizeof(szDisplay));
        if (strlen(UserInfo[i].szUserName) == 0)
            continue;
        memset(aucRole,0,sizeof(aucRole));  
        memset(aucLock,0,sizeof(aucLock));          
        /* 权限 */
        if (UserInfo[i].ucRole == USER_ROLE_ADMIN)
        {
            memcpy(aucRole,"ADMIN",sizeof("ADMIN"));
        }
        else if (UserInfo[i].ucRole == USER_ROLE_CONFIG)
        {
            memcpy(aucRole,"CONFIG",sizeof("CONFIG"));       
        }
        else if (UserInfo[i].ucRole == USER_ROLE_VIEW)
        {
            memcpy(aucRole,"GUEST",sizeof("GUEST"));               
        }
        else
        {
            memcpy(aucRole,"UNKNOWN",sizeof("UNKNOWN"));                
        }

        /* 是否被锁定 */
        if (UserInfo[i].bLocked)
        {
            memcpy(aucLock,"YES",sizeof("YES"));              
        }
     else
     {
            memcpy(aucLock,"NO",sizeof("NO"));               
     }   
        XOS_snprintf(pDisplay, sizeof(szDisplay) - 1,
                     "%-3d%-32s%-8s%-3s\n",
                              ++Number,
                              UserInfo[i].szUserName,
                              aucRole,
                              aucLock
                              );
        AddParaToMsg (ptRtnMsg,DATA_TYPE_STRING,(BYTE *)pDisplay,strlen((CHAR *)pDisplay)+1);
        *piLoopCnt += 1;        
    }
    return;
}
/**************************************************************************
* 函数名称：VOID InnerCmdUnlockUser
* 功能描述：解除锁定用户
* 访问的表：无
* 修改的表：无
* 输入参数：
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
static void InnerCmdUnlockUser(MSG_COMM_OAM *ptOamMsg,WORD32 dwMsgId,MSG_COMM_OAM *ptRtnMsg)
{
    MSG_COMM_OAM *ptSendMsg = ptOamMsg;
    OPR_DATA *pOprData = NULL;
    CHAR  sUserName[MAX_OAM_USER_NAME_LEN];
    TYPE_USER_INFO tUserInfo;
    BYTE    ucUserRight =0;                            /* 用户权限 */    
    pOprData = ptSendMsg->Data;
    
    Oam_LinkGetUserRight((LPVOID)ptOamMsg,dwMsgId,&ucUserRight);           
    /* 只有管理员用户才有权限执行此命令 */
    if (USER_ROLE_ADMIN != ucUserRight)
    {
        AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)NotAdmin);
        ptRtnMsg->ReturnCode = ERR_AND_HAVEPARA;            
        return;
    }        
    /*para1: username*/
    memset(sUserName, 0, MAX_OAM_USER_NAME_LEN);
    memcpy(sUserName,pOprData->Data,pOprData->Len);

    /* 查找用户是否存在 */
    memset(&tUserInfo, 0, sizeof(TYPE_USER_INFO));
    if (!FindUserInfoFromLink(sUserName, &tUserInfo))
    {
        AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)UserNotExists);
        ptRtnMsg->ReturnCode = ERR_AND_HAVEPARA;            
        return;
    }        
    if (FALSE == tUserInfo.bLocked)
    {
        AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)UserNotLocked);
        ptRtnMsg->ReturnCode = ERR_AND_HAVEPARA;            
        return;        
    }
    tUserInfo.bLocked = FALSE;
    UpdateOneUserToLink(&tUserInfo);
    if (!WriteUserScriptFile())
    {
        AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)CmdExecFailed);
        ptRtnMsg->ReturnCode = ERR_AND_HAVEPARA;            
        return;        
    }
    else
    {
        AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)CmdExecSucc);
    }   
    return;
}
/**************************************************************************
* 函数名称：VOID InnerCmdSetSpecIP
* 功能描述：设置特定IP信息
* 访问的表：无
* 修改的表：无
* 输入参数：
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
 static void InnerCmdSetSpecIP(MSG_COMM_OAM *ptOamMsg,WORD32 dwMsgId,MSG_COMM_OAM *ptRtnMsg)
{
    CHAR TempStr[LEN_IP_ADDR_STR] = {0};
    BYTE    ucUserRight =0;                            /* 用户权限 */    
    
    Oam_LinkGetUserRight((LPVOID)ptOamMsg,dwMsgId,&ucUserRight);          
    /* 只有管理员用户才有权限执行此命令 */
    if (USER_ROLE_ADMIN != ucUserRight)
    {
        AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)NotAdmin);
        ptRtnMsg->ReturnCode = ERR_AND_HAVEPARA;            
        return;
    }        
    Oam_LinkGetTnIpAddr((MSG_COMM_OAM*)ptOamMsg,dwMsgId,TempStr);
    Oam_SetSpecifyIp(TempStr);
    
    if (!WriteUserScriptFile())
    {
        AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)CmdExecFailed);
        ptRtnMsg->ReturnCode = ERR_AND_HAVEPARA;            
        return;        
    }
    else
    {
        AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)CmdExecSucc);
    }  
    return;
}

/**************************************************************************
* 函数名称：VOID InnerCmdSetTelnetIdleTimeOut
* 功能描述：设置Telnet的空闲时间
* 访问的表：无
* 修改的表：无
* 输入参数：
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
 static void InnerCmdSetTelnetIdleTimeOut(MSG_COMM_OAM *ptOamMsg,WORD32 dwMsgId,MSG_COMM_OAM *ptRtnMsg)
{
    /*静态局部变量*/
    static COMMON_INFO   tCommonInfo;
    COMMON_INFO   *ptCommonInfo=&tCommonInfo;        
    OPR_DATA *pRevOprData = (OPR_DATA *)ptOamMsg->Data;
    WORD16 i = 0;
    memset(ptCommonInfo,0,sizeof(COMMON_INFO));
    
    Oam_GetCommonInfo(ptCommonInfo);    
    if(!ptOamMsg->bIfNo)
    {
        for (i = 0; i < ptOamMsg->Number; i++)
        {
            SWITCH(pRevOprData->ParaNo)
            {
                CASE(1)
                memcpy(&ptCommonInfo->wTelnetIdleTimeout, pRevOprData->Data, sizeof(WORD16));
                BREAK
                DEFAULT
                BREAK
            }
            END
            pRevOprData = GetNextOprData(pRevOprData);
        }
    }
    else
    {
        ptCommonInfo->wTelnetIdleTimeout = DEFAULT_TELNET_IDLE_TIMEOUT;
    }
    Oam_SetCommonInfo(ptCommonInfo);    
    if (SaveOamCfgInfo() == WRITEERR_SUCCESS)
    {
        AddMapParaToMsg(ptRtnMsg,MAP_TYPE_EXE_INNER_ERROR, (WORD16)ERR_EXE_INNER_CFG_SUCC);
    }
    else
    {
        AddMapParaToMsg(ptRtnMsg,MAP_TYPE_EXE_INNER_ERROR, (WORD16)ERR_EXE_INNER_CFG_FAIL);    
        ptRtnMsg->ReturnCode = ERR_AND_HAVEPARA;            
    }
    return;

}

/**************************************************************************
* 函数名称：VOID InnerCmdShowTelnetIdleTimeOut
* 功能描述：查看Telnet的空闲时间
* 访问的表：无
* 修改的表：无
* 输入参数：
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
static void InnerCmdShowTelnetIdleTimeOut(MSG_COMM_OAM *ptOamMsg,WORD32 dwMsgId,MSG_COMM_OAM *ptRtnMsg)
{
    CHAR TempStr[MAX_STRING_LEN] = {0};
    WORD16 wDay,wHour,wMinute,wSecond;
    /*静态局部变量*/    
    static COMMON_INFO   tCommonInfo;
    COMMON_INFO   *ptCommonInfo=&tCommonInfo;        
    memset(ptCommonInfo,0,sizeof(COMMON_INFO));
    Oam_GetCommonInfo(ptCommonInfo);
        
    if (ptOamMsg->LinkChannel == MIN_CLI_VTY_NUMBER)
    {
        wHour = ptCommonInfo->wConsoleIdleTimeout/60;
        wMinute = ptCommonInfo->wConsoleIdleTimeout%60;
        wSecond = 0;
        wDay = ptCommonInfo->wConsoleAbsTimeout/1440;
        wHour = (ptCommonInfo->wConsoleAbsTimeout%1440)/60;
        wMinute = (ptCommonInfo->wConsoleAbsTimeout%1440)%60;
        XOS_snprintf(TempStr, MAX_STRING_LEN, " Console idle-timeout is: %02u:%02u:%02u\n Console absolute-timeout is: %ud%02uh%02um\n",
                  wHour,wMinute,wSecond,wDay,wHour,wMinute);                    
    }
    else
    {
        wHour = ptCommonInfo->wTelnetIdleTimeout/60;
        wMinute = ptCommonInfo->wTelnetIdleTimeout%60;    
        wSecond = 0;
        XOS_snprintf(TempStr, MAX_STRING_LEN, " Idle-timeout is: %02u:%02u:%02u\n",wHour,wMinute,wSecond);
    }
    AddParaToMsg(ptRtnMsg , DATA_TYPE_STRING,(BYTE *)(TempStr),strlen((CHAR *)TempStr)+1);      
    Oam_SetCommonInfo(ptCommonInfo);        
    return;
}
/**************************************************************************
* 函数名称：VOID InnerCmdShowUserLog
* 功能描述：显示用户日志信息
* 访问的表：无
* 修改的表：无
* 输入参数：
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
static void InnerCmdShowUserLog(MSG_COMM_OAM *ptOamMsg,WORD32 dwMsgId,MSG_COMM_OAM *ptRtnMsg)
{
    MSG_COMM_OAM *ptSendMsg = ptOamMsg;
    XOS_FD fd;
    XOS_STATUS dwRet = XOS_SUCCESS;
    TYPE_USER_LOG_HEAD tFileHead;
    SWORD32 sdwByteCount;
    TYPE_USER_LOG tUserLog;
    CHAR szDisplay[256] = {0};
    SWORD32 sdwFileLen = 0;       
    CHAR *pDisplay = szDisplay;
    CHAR  aucConnType[10]={0};
    CHAR  aucLoginRslt[10]={0};
    CHAR  aucQuitReason[10]={0};
    CHAR  aucLoginTime[20]={0};
    CHAR  aucLogoutTime[20]={0};
    CHAR  acOamDir[FILE_PATH_LEN] = {'\0'}; 
    CHAR  acMateDir[FILE_PATH_LEN] = {'\0'}; 
    CHAR  acUserFile[MAX_ABS_PATH_LEN] = {'\0'};    
    DWORD dwOffset = 0;
    BYTE bFlag = 0;/*控制标记*/    
    BYTE  i=0;
    BYTE ucMaxNote=(MAX_OAMMSG_LEN-sizeof(MSG_COMM_OAM))/256;/*一包的最大记录个数*/
    BYTE iLoop = 0;/*控制脚本的循环次数*/
    BYTE *piLoopCnt = NULL; /*循环返回的字符串数目*/    
    ptRtnMsg->OutputMode  = OUTPUT_MODE_REQUEST;    
    if (!OamGetPath(acOamDir, acMateDir))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "[InnerCmdShowUserLog]OamGetPath fail!\n");
        return;
    }
    XOS_snprintf(acUserFile, MAX_ABS_PATH_LEN, "%s%s", acOamDir, OAM_USER_LOG_FILE);
    /* 打开原文件 */
    dwRet = XOS_OpenFile(acUserFile, XOS_RDONLY, &fd);
    if (dwRet != XOS_SUCCESS)
    {
        bFlag = 0;
        AddParaToMsg(ptRtnMsg , DATA_TYPE_BYTE,&bFlag, sizeof(BYTE));   
     AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)OpenFileFail);    
        ptRtnMsg->ReturnCode = ERR_AND_HAVEPARA;         
        return;
    }
    dwRet = XOS_GetFileLength(acUserFile,&sdwFileLen);
    /*获取文件长度失败*/
    if (dwRet != XOS_SUCCESS)
    {
        bFlag = 0;
        AddParaToMsg(ptRtnMsg , DATA_TYPE_BYTE,&bFlag, sizeof(BYTE));   
     AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)GetFileLengthFail);    
        ptRtnMsg->ReturnCode = ERR_AND_HAVEPARA;         
        return ;
    }    
    /*文件长度为0*/
    if(0 == sdwFileLen)
    {
        bFlag = 0;
        AddParaToMsg(ptRtnMsg , DATA_TYPE_BYTE,&bFlag, sizeof(BYTE));   
     AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)FileLengthNotRigth);      
        ptRtnMsg->ReturnCode = ERR_AND_HAVEPARA;         
        return ;
    }    
    /* 读取文件头 */
    dwRet = XOS_ReadFile(fd, (CHAR *)&tFileHead,sizeof(TYPE_USER_LOG_HEAD), &sdwByteCount);    
    if (dwRet != XOS_SUCCESS)
    {
        bFlag = 0;
        AddParaToMsg(ptRtnMsg , DATA_TYPE_BYTE,&bFlag, sizeof(BYTE));   
     AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)ReadFileFail);    
        ptRtnMsg->ReturnCode = ERR_AND_HAVEPARA;         
        XOS_CloseFile(fd);
        return;
    }
    if (sdwByteCount != sizeof(TYPE_USER_LOG_HEAD))
    {
        bFlag = 0;
        AddParaToMsg(ptRtnMsg , DATA_TYPE_BYTE,&bFlag, sizeof(BYTE));   
     AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)ReadFileFail);    
        ptRtnMsg->ReturnCode = ERR_AND_HAVEPARA;         
        XOS_CloseFile(fd);
        return;
    }
    if (tFileHead.dwFormatDef != USER_LOG_FILE_FORMAT)
    {
        bFlag = 0;
        AddParaToMsg(ptRtnMsg , DATA_TYPE_BYTE,&bFlag, sizeof(BYTE));   
     AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)ReadFileFail);    
        ptRtnMsg->ReturnCode = ERR_AND_HAVEPARA;         
        XOS_CloseFile(fd);
        return;
    }
    if (tFileHead.dwVersion != USER_LOG_FILE_VERSION)
    {
        bFlag = 0;
        AddParaToMsg(ptRtnMsg , DATA_TYPE_BYTE,&bFlag, sizeof(BYTE));   
     AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)ReadFileFail);     
        ptRtnMsg->ReturnCode = ERR_AND_HAVEPARA;         
        XOS_CloseFile(fd);
        return;
    }
    if (!(sizeof(TYPE_USER_LOG_HEAD)<= tFileHead.dwLogBegin))
    {
        bFlag = 0;
        AddParaToMsg(ptRtnMsg , DATA_TYPE_BYTE,&bFlag, sizeof(BYTE));   
     AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)ReadFileFail);    
        ptRtnMsg->ReturnCode = ERR_AND_HAVEPARA;         
        XOS_CloseFile(fd);
        return;
    }    
    /*第一个包*/
    if (ptSendMsg->CmdRestartFlag)
    {        
        memset(ptSendMsg->LastData, 0, sizeof(ptSendMsg->LastData));        
    }
    memcpy(&dwOffset,ptSendMsg->LastData,sizeof(dwOffset));
    if(dwOffset ==0)
    {
        dwOffset =  tFileHead.dwLogBegin;
    }
    /*文件偏移地址超出文件长度*/
    if (dwOffset >= (DWORD)sdwFileLen)
    {
        bFlag = 0;
        AddParaToMsg(ptRtnMsg , DATA_TYPE_BYTE,&bFlag, sizeof(BYTE));   
     AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)ReadFileFail);    
        ptRtnMsg->ReturnCode = ERR_AND_HAVEPARA;         
        return ;
    }   
    /* 从原文件中读取内容，写入临时文件 */
    dwRet = XOS_SeekFile(fd, XOS_SEEK_SET,dwOffset);
    if (dwRet != XOS_SUCCESS)
    {
        bFlag = 0;
        AddParaToMsg(ptRtnMsg , DATA_TYPE_BYTE,&bFlag, sizeof(BYTE));   
     AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)ReadFileFail);    
        ptRtnMsg->ReturnCode = ERR_AND_HAVEPARA;         
        XOS_CloseFile(fd);
        return;
    }

    for(i =0;i<ucMaxNote;i++)
    {
        /* 从原文件中读取每条记录 */
        memset(&tUserLog, 0, sizeof(TYPE_USER_LOG));
        memset(pDisplay, 0, sizeof(szDisplay));
     memset(aucConnType,0,sizeof(aucConnType));
     memset(aucLoginRslt,0,sizeof(aucLoginRslt));
     memset(aucQuitReason,0,sizeof(aucQuitReason));
     memset(aucLoginTime,0,sizeof(aucConnType));
     memset(aucLogoutTime,0,sizeof(aucConnType));        
        dwRet = XOS_ReadFile(fd, (CHAR *)&tUserLog, sizeof(TYPE_USER_LOG), &sdwByteCount);
        if (dwRet != XOS_SUCCESS)
        {
            bFlag = 0;
            AddParaToMsg(ptRtnMsg , DATA_TYPE_BYTE,&bFlag, sizeof(BYTE));   
             AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)ReadFileFail);    
            ptRtnMsg->ReturnCode = ERR_AND_HAVEPARA;                 
            XOS_CloseFile(fd);
            return;
        }
    
        if (sdwByteCount != sizeof(TYPE_USER_LOG))
        {
            bFlag = 0;
            AddParaToMsg(ptRtnMsg , DATA_TYPE_BYTE,&bFlag, sizeof(BYTE));   
             AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)ReadFileFail);
            ptRtnMsg->ReturnCode = ERR_AND_HAVEPARA;                 
            XOS_CloseFile(fd);
            return;
        }
        if (dwOffset == tFileHead.dwLogBegin)
        {
            strncpy(pDisplay, "No     ConnType UserName                        IP              LoginTime        LoginResult LogoutTime       LogoutReason\n", sizeof(szDisplay));
        }
        else
        {
          /*  strncpy(pDisplay, 0, sizeof(szDisplay));*/
        }
        
        if (OAM_CONN_TYPE_SERIAL == tUserLog.ucConnType)
        {
            memcpy(aucConnType,"SERIAL",sizeof("SERIAL"));    
        }
        else if (OAM_CONN_TYPE_TELNET == tUserLog.ucConnType)

        {
            memcpy(aucConnType,"TELNET",sizeof("TELNET"));   
        }
        else if (OAM_CONN_TYPE_SSH == tUserLog.ucConnType)
        {
            memcpy(aucConnType,"SSH",sizeof("SSH"));    
        }
        else
        {
            memcpy(aucConnType,"UNKNOWN",sizeof("UNKNOWN"));   
        }       

        if (USER_LOGIN_SUCCESS == tUserLog.ucLoginRslt)
        {
            memcpy(aucLoginRslt,"SUCCESS",sizeof("SUCCESS"));    
        }
        else
        {
            memcpy(aucLoginRslt,"FAILED",sizeof("FAILED"));     
        }       
        
        if (USER_LOGOUT_COMMON == tUserLog.ucQuitReason)
        {
            memcpy(aucQuitReason,"COMMON",sizeof("COMMON"));       
        }
        else if (USER_LOGOUT_TIMEOUT == tUserLog.ucQuitReason)
        {
            memcpy(aucQuitReason,"TIMEOUT",sizeof("TIMEOUT"));    
        }
        else if (USER_LOGOUT_ERROR == tUserLog.ucQuitReason)
        {
            memcpy(aucQuitReason,"EXCEPTION",sizeof("EXCEPTION"));   
        }
        else
        {
            memcpy(aucQuitReason,"NONE",sizeof("NONE"));    
        }  

     XOS_snprintf(aucLoginTime, sizeof(aucLoginTime) - 1,"%d-%d-%d %d:%d", 
                       tUserLog.tSysClockLogin.wSysYear,
                    tUserLog.tSysClockLogin.ucSysMon,
                    tUserLog.tSysClockLogin.ucSysDay,
                    tUserLog.tSysClockLogin.ucSysHour,
                    tUserLog.tSysClockLogin.ucSysMin);   

     XOS_snprintf(aucLogoutTime, sizeof(aucLogoutTime) - 1,"%d-%d-%d %d:%d", 
                       tUserLog.tSysClockLogout.wSysYear,
                    tUserLog.tSysClockLogout.ucSysMon,
                    tUserLog.tSysClockLogout.ucSysDay,
                    tUserLog.tSysClockLogout.ucSysHour,
                    tUserLog.tSysClockLogout.ucSysMin);        
    
    if(i == 0)
    {
        /*控制字符*/
        bFlag = 1;
           AddParaToMsg(ptRtnMsg, DATA_TYPE_BYTE,&bFlag, sizeof(BYTE));  
           AddParaToMsg(ptRtnMsg, DATA_TYPE_BYTE, (BYTE *)&iLoop, (BYTE)1);    
        piLoopCnt = (BYTE *)ptRtnMsg->Data[ptRtnMsg->Number - 1].Data;               
    }
    XOS_snprintf(pDisplay, sizeof(szDisplay) - 1,"%s%-7d%-9s%-32s%-16s%-17s%-12s%-17s%-12s\n", 
                  pDisplay,
                  tUserLog.wNo,
               aucConnType,
               tUserLog.sUserName,
               tUserLog.tnIpAddr,
                        aucLoginTime,
               aucLoginRslt,
                        aucLogoutTime,
               aucQuitReason);   
    AddParaToMsg(ptRtnMsg , DATA_TYPE_STRING,(BYTE *)(pDisplay),strlen((CHAR *)pDisplay)+1);    
    dwOffset = dwOffset + sizeof(TYPE_USER_LOG);    
    *piLoopCnt += 1;
    if(dwOffset  >= (DWORD)sdwFileLen)
    {
        break;
    }    
    }    
    if(dwOffset < (DWORD)sdwFileLen)
    {
        /* ptRtnMsg->OutputMode  = OUTPUT_MODE_REQUEST;*/
        ptRtnMsg->ReturnCode      = SUCC_CMD_NOT_FINISHED;
     ptRtnMsg->CmdRestartFlag =FALSE;    
        /*保存本次文件信息*/
        memcpy(ptRtnMsg->LastData, &dwOffset, sizeof(dwOffset));
    } 
    XOS_CloseFile(fd);    
    return;    
}
/**************************************************************************
* 函数名称：VOID InnerCmdShowSpecifyIP
* 功能描述：显示特定IP信息
* 访问的表：无
* 修改的表：无
* 输入参数：
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
static void InnerCmdShowSpecifyIP(MSG_COMM_OAM *ptOamMsg,WORD32 dwMsgId,MSG_COMM_OAM *ptRtnMsg)
{
    CHAR szDisplayStr[LEN_IP_ADDR_STR] = {0};    
    Oam_GetSpecifyIP(szDisplayStr);
    AddParaToMsg (ptRtnMsg,DATA_TYPE_STRING,(BYTE *)szDisplayStr,strlen((CHAR *)szDisplayStr)+1);        
    return;
}
/**************************************************************************
* 函数名称：VOID InnerCmdShowModeStatck
* 功能描述：查询模式栈
* 访问的表：无
* 修改的表：无
* 输入参数：MSG_COMM_OAM *ptOamMsg:OAM配置请求
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2009/01/07    V1.0    齐龙兆      创建
**************************************************************************/
static void InnerCmdShowModeStatck(MSG_COMM_OAM *ptOamMsg,WORD32 dwMsgId,MSG_COMM_OAM *ptRtnMsg)
{
    MSG_COMM_OAM *ptSendMsg = ptOamMsg;
    WORD16 wLinkStateIndex = 0;
    DWORD dwLinkType = 0;
    BYTE bModeStackTop = 0;
    BYTE aucModeID[MAX_MODESTACK] = {0};
    
    /*参数检查*/
    if ((NULL == ptOamMsg)||(NULL == ptRtnMsg))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return;
    }
    ptRtnMsg->ReturnCode = SUCC_AND_NOPARA;
    
    dwLinkType = ((ptSendMsg->LinkChannel)&0X00000200)>>9; /*BIT9 1-表示XML应答0-CLI应答*/
    /*链路类型-Telnet*/
    if (LINKSTATE_TYPE_CLI == dwLinkType)
    {
        Oam_LinkGetModeStackTop((LPVOID)ptOamMsg,dwMsgId,&bModeStackTop); 
        Oam_LinkGetModeID((LPVOID)ptOamMsg,dwMsgId,aucModeID);        
    }
    else if(LINKSTATE_TYPE_XML == dwLinkType)/*链路-XML*/
    {
        wLinkStateIndex = (WORD16)(ptSendMsg->LinkChannel&0X000001FF);
        XML_LinkGetModeStackTop(wLinkStateIndex,&bModeStackTop);    
        XML_LinkGetModeID(wLinkStateIndex,aucModeID);                
    }
    else
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d dwLinkType Error!%d ....................",__func__,__LINE__,dwLinkType);
        return;
    }
    /*构建参数*/
    ConstructShowModeAckMsgParam(bModeStackTop,aucModeID,ptRtnMsg);
    return;
}

static void InnerCmdExecSaveCmd(MSG_COMM_OAM *ptOamMsg,WORD32 dwMsgId,MSG_COMM_OAM *ptRtnMsg)
{
    MSG_COMM_OAM *ptSendMsg = ptOamMsg;
    WORD16 wModule = 0;
    WORD16 wValue = 0;
    WORD16 wPara1 = 0;
    WORD16 wPara2 = 0;
    JID                  tAppJid;
    _db_t_back_save_req  tProDB;
    int i = 0;
    OPR_DATA *pOprData = NULL;
    BOOL bIsOmp = FALSE;
    BOOL bIsLomp = FALSE;
    JID     jidSelf;
    
    if (ptSendMsg == NULL)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,"ptSendMsg is NULL!!! \n");
        return;
    }

    pOprData = (OPR_DATA *)ptSendMsg->Data;
    if (NULL == pOprData)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,"pOprData is NULL!!! \n");
        return;
    }
    
    /*获取本job的jid信息*/
    Oam_InnerCmdGetPtVarJidinfo(&jidSelf);
    for (i = 0; i < ptOamMsg->Number; i++)
    {
        SWITCH(pOprData->ParaNo)
        {
            CASE(1)
            memcpy(&wPara1, pOprData->Data, sizeof(WORD16));
            BREAK
            CASE(1)
            memcpy(&wPara2, pOprData->Data, sizeof(WORD16));
            BREAK
            DEFAULT
            BREAK
        }
        END
        pOprData = GetNextOprData(pOprData);
    }

    /*判断是否是OMP*/
    if (ptOamMsg->CmdID == CMD_RPU_SAVE)
    {
        wValue = wPara1;
        bIsOmp = TRUE;
    }
    else
    {
        wModule = (wPara1 == 0) ? jidSelf.wModule : wPara1;
        wValue = wPara2;            
        if (jidSelf.wModule == wModule)
        {
            bIsOmp = TRUE;
        }

        /*判断是否是LOMP*/
        if (!bIsOmp)
        {
            bIsLomp = IfModuleIsLomp(wModule);
        }
    }

    if ((!bIsOmp) && (!bIsLomp))
    {
        /*调用解析端的处理函数，将错误信息返回，
            并将执行状态设置为telnet端*/
        Oam_InnerSaveCmdLompErr((LPVOID)ptOamMsg,dwMsgId,Hdr_szErrInvalidModule);      
        return;
    }

    /*先存ZDB，等到DBS返回结果之后再判断是否需要存TXT*/
    
    /*构造DBS的JID*/
    memset(&tAppJid, 0, sizeof(JID));
    if (ptSendMsg->CmdID == CMD_RPU_SAVE)
    {
        tAppJid.wModule = 2;    
    }
    else
    {
        tAppJid.wModule = wModule;    
    }
    tAppJid.dwJno = XOS_ConstructJNO(pNZDB_IO,1);
    tAppJid.wUnit   = 65535;
    tAppJid.ucSUnit = 255;
    tAppJid.ucSubSystem = 255;
    if (0 == wValue)
    {
        tAppJid.ucRouteType = COMM_MASTER_SERVICE;    

    }
    else
    {
        tAppJid.ucRouteType = COMM_SLAVE_SERVICE;    
    }   
    memset(&tProDB, 0, sizeof(_db_t_back_save_req));
    
    if (ptSendMsg->CmdID == CMD_RPU_SAVE)
    {
        tProDB.isAllDbSave = FALSE;
        tProDB.wDbNum = 1;
        XOS_snprintf(tProDB.tDatabaseSave[0].dbName,
                             sizeof(tProDB.tDatabaseSave[0].dbName) -1,
                             "%s",
                             "IPCONF_DB");
    }
    else
    {
        tProDB.isAllDbSave = TRUE;
    }
    tProDB.isReport = TRUE;

    XOS_GetSelfJID(&jidSelf);
    memcpy(&tProDB.BackMgtJid, &jidSelf,sizeof(JID));

    /* 命令发送，设置进度信息上报等待定时器 */
    Oam_InnerKillCmdPlanTimer((LPVOID)ptOamMsg,dwMsgId);
    Oam_InnerSetCmdPlanTimer((LPVOID)ptOamMsg,dwMsgId);    
    XOS_SendAsynMsg(EV_SAVETABLE, (BYTE*)&tProDB, (WORD16)sizeof(tProDB),
                      0, 0,(void*)&tAppJid);
    /*设置返回数据*/
    ptRtnMsg->ReturnCode = SUCC_AND_NOPARA;    
    return;    

}
/**************************************************************************
* 函数名称：VOID Oam_InnerApp
* 功能描述：内部函数处理
* 访问的表：无
* 修改的表：无
* 输入参数：
* 输出参数：
* 返 回 值：
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    马俊辉      创建
**************************************************************************/
void Oam_InnerApp(LPVOID ptOamMsg,WORD32 dwMsgId,MSG_COMM_OAM *ptRtnMsg)
{
    /*参数检查*/
    if ((NULL == ptOamMsg)||(NULL == ptRtnMsg))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return;
    }
    
    /*ptRtnMsg返回结构体的初始化*/
    memset(ptRtnMsg,0,MAX_OAMMSG_LEN);    
    if((dwMsgId !=EV_SAVEOK) && (dwMsgId !=EV_SAVEFAIL))
    {
        memcpy(ptRtnMsg,ptOamMsg,sizeof(MSG_COMM_OAM));
    }
    ptRtnMsg->Number = 0;
    ptRtnMsg->DataLen  = sizeof(MSG_COMM_OAM);
    ptRtnMsg->OutputMode = OUTPUT_MODE_NORMAL;
    ptRtnMsg->ReturnCode = SUCC_AND_HAVEPARA;
    ptRtnMsg->ucOamTag = OAMCFG_TAG_NORMAL;    
    
    /*用来区分是配置消息还是db返回的消息*/
    switch(dwMsgId)
    {
    case EV_OAM_CFG_CFG:
    case OAM_MSG_XMLAGNET_TO_INTERPRET:                
        Proc_Cmd_InnerApp((MSG_COMM_OAM*)ptOamMsg,dwMsgId,ptRtnMsg);
        break;
    case EV_SAVEOK:         
    case EV_SAVEFAIL:
        Proc_Cmd_MssaveFromDb(&ptOamMsg,dwMsgId,ptRtnMsg);
        break;
    default:
        break;
    }
    return;
}

static void Proc_Cmd_MssaveFromDb(LPVOID *ptOamMsg,WORD32 dwMsgId,MSG_COMM_OAM *ptRtnMsg)
{    
    BYTE bFlag = 0;/*控制标记*/
    _db_t_saveresult *pSaveResult  = NULL;
    WORD16 wDbNum = 0;
    
    /*参数检查*/
    if ((NULL == ptOamMsg)||(NULL == ptRtnMsg))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return;
    }
    
    pSaveResult = (_db_t_saveresult  *)ptOamMsg;
    /*初始化ptRthMsg*/
    Oam_InnerGetRtnMsgInfo((LPVOID)ptOamMsg,dwMsgId,ptRtnMsg);
    ptRtnMsg->DataLen  = sizeof(MSG_COMM_OAM);
    ptRtnMsg->CmdRestartFlag =TRUE;    
    switch(dwMsgId)
    {
    case  EV_SAVEOK:    
        bFlag = 0;
        AddParaToMsg(ptRtnMsg , DATA_TYPE_BYTE,&bFlag, sizeof(BYTE));        
        AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)SaveZDBOK);    
        break;        
    case  EV_SAVEFAIL:
        bFlag = 1;
        AddParaToMsg(ptRtnMsg , DATA_TYPE_BYTE,&bFlag, sizeof(BYTE));        
        AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)SaveZDBFail);    
        AddParaToMsg(ptRtnMsg , DATA_TYPE_WORD,(BYTE *)(&pSaveResult->wDbNum), sizeof(WORD));
        ptRtnMsg->ReturnCode = ERR_AND_HAVEPARA;        
                
        for (wDbNum = 0; wDbNum < pSaveResult->wDbNum; wDbNum++)
        {    
            AddParaToMsg(ptRtnMsg , DATA_TYPE_STRING,(BYTE *)(pSaveResult->tDatabaseSaveResult[wDbNum].dbName),strlen(pSaveResult->tDatabaseSaveResult[wDbNum].dbName)+1);           
#if 0
            /** @brief 主备目录存盘都失败*/
            if (pSaveResult->tDatabaseSaveResult[wDbNum].wReason == _DB_E_SAVE_BOTHPATH)
            {
                AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)SaveZDBErrSaveBothPath);               
            }
            /** @brief 主目录存盘失败*/
            else if (pSaveResult->tDatabaseSaveResult[wDbNum].wReason == _DB_E_SAVE_MAINPATH)
            {
                AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)SaveZDBErrMainPath);                           
            }
            /** @brief备目录存盘失败*/
            else if (pSaveResult->tDatabaseSaveResult[wDbNum].wReason == _DB_E_SAVE_BACKUP)
            {
                AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)SaveZDBErrBackup);           
            }
            /** @brief没有表需要存盘*/
            else if (pSaveResult->tDatabaseSaveResult[wDbNum].wReason == _DB_NOT_TABLE_SAVE)
            {
                AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)SaveZDBErrNoTblSave);             
            }
            /** @brief读取文件长度错误*/
            else if (pSaveResult->tDatabaseSaveResult[wDbNum].wReason == _DB_READ_FILE_ERROR)
            {
                AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)SaveZDBErrReadFileErr);             
            }
            /** @brief表不需要存盘*/
            else if (pSaveResult->tDatabaseSaveResult[wDbNum].wReason == _DB_TABLE_NOT_NEED_SAVE)
            {
                AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)SaveZDBErrTblNotNeedSave);             
            }
            /** @brief 库正在进行本机存盘 */
            else if (pSaveResult->tDatabaseSaveResult[wDbNum].wReason == _DB_E_LOCAL_SAVING)
            {
                AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)SaveZDBErrLocalSaving);            
            }
            /** @brief 库正在进行主备存盘 */
            else if (pSaveResult->tDatabaseSaveResult[wDbNum].wReason == _DB_E_MS_SAVING)
            {
                AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)SaveZDBErrMSSaving);             
            }
            /** @brief 库正在进行上电加载 */
            else if (pSaveResult->tDatabaseSaveResult[wDbNum].wReason == _DB_E_POWERON_LOADING)
            {
                AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)SaveZDBErrPowerOnLoading);            
            }
            /** @brief 库正在进行RX FTP加载 */
            else if (pSaveResult->tDatabaseSaveResult[wDbNum].wReason == _DB_E_FTP_LOADING)
            {
                AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)SaveZDBErrFtpLoading);             
            }
            /** @brief 库正在进行APP FTP加载 */
            else if (pSaveResult->tDatabaseSaveResult[wDbNum].wReason == _DB_E_APPFTP_LOADING)
            {
                AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)SaveZDBErrAppFtpLoading);             
            }
            /** @brief IO正在忙 */
            else if (pSaveResult->tDatabaseSaveResult[wDbNum].wReason == _DB_E_IOBUSY)
            {
                AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)SaveZDBErrIOBusy);            
            }
            /** @brief 非法句柄 */
            else if (pSaveResult->tDatabaseSaveResult[wDbNum].wReason == _DB_E_INVALIDHANDLE)
            {
                AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)SaveZDBErrInvalidHandle);            
            }
            else
            {
                AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)SaveZDBErrOther);            
            }      
#endif
        }
        break;
    default:
    {
        bFlag = 1;
        AddParaToMsg(ptRtnMsg , DATA_TYPE_BYTE,&bFlag, sizeof(BYTE));    
        break;        
    }
    }
    return;
}

static void Proc_Cmd_InnerApp(LPVOID ptOamMsg,WORD32 dwMsgId,MSG_COMM_OAM *ptRtnMsg)
{
    PMSG_COMM_OAM ptRevMsg = NULL;
    ptRevMsg = (PMSG_COMM_OAM)ptOamMsg;
    DWORD dwExeID = ptRevMsg->CmdID;
    
    /*打印收发消息*/
    if (bDebugPrint)
    {
        PrintMSGCOMMOAMStruct((MSG_COMM_OAM *)ptOamMsg,"MSG_CLIMANAGER_TO_INNERAPP");
    }
    
    switch(dwExeID)
    {
    case CMD_DEF_SET_USER:           /* 新增、删除用户 */
        InnerCmdUser(ptRevMsg,dwMsgId,ptRtnMsg);
        break;    
    case CMD_DEF_SET_PASSWORD:       /* 设置用户密码 */
        InnerCmdSetUserPwd((MSG_COMM_OAM*)ptOamMsg,dwMsgId,ptRtnMsg);
        break;        
    case CMD_DEF_SET_ROLE:           /* 设置用户权限 */
        InnerCmdSetUserRole((MSG_COMM_OAM*)ptOamMsg,dwMsgId,ptRtnMsg);
        break;        
    case CMD_DEF_SET_UNLOCKUSER:     /* 解锁定用户 */
        InnerCmdUnlockUser((MSG_COMM_OAM*)ptOamMsg,dwMsgId,ptRtnMsg);
        break;
    case CMD_DEF_SET_SPECIFYIP:      /* 设置指定IP */
        InnerCmdSetSpecIP((MSG_COMM_OAM*)ptOamMsg,dwMsgId,ptRtnMsg);
        break;
    case CMD_DEF_SET_SAVE:    /* 存盘操作 */
    case CMD_RPU_SAVE:
        InnerCmdExecSaveCmd((MSG_COMM_OAM*)ptOamMsg,dwMsgId,ptRtnMsg);
        break;    
    case CMD_LDB_SET_TELNET_IDLE_TIMEOUT:  /*设置idletime的时间*/
        InnerCmdSetTelnetIdleTimeOut((MSG_COMM_OAM*)ptOamMsg,dwMsgId,ptRtnMsg);
        break;    
    case CMD_DEF_SET_PASSWORDINTENSITY:  /* 设置密码强度*/
        InnerCmdSetPasswordintensity((MSG_COMM_OAM*)ptOamMsg,dwMsgId,ptRtnMsg);
        break;
    case CMD_DEF_SET_SIMPLEPASSLENGTH:  /* 设置弱密码最低长度 */
        InnerCmdSetSimplePasswordLen((MSG_COMM_OAM*)ptOamMsg,dwMsgId,ptRtnMsg);
        break;        
    case CMD_LDB_GET_MODESTATCK:/*查询模式栈*/
        InnerCmdShowModeStatck((MSG_COMM_OAM*)ptOamMsg,dwMsgId,ptRtnMsg);
        break;
    case CMD_DEF_GET_SHOWUSER:       /* 查看用户信息 */
        InnerCmdShowUser((MSG_COMM_OAM*)ptOamMsg,dwMsgId,ptRtnMsg);
        break;    
    case CMD_DEF_GET_SHOWUSERLOG:    /* 查看用户登录登出日志 */
        InnerCmdShowUserLog((MSG_COMM_OAM*)ptOamMsg,dwMsgId,ptRtnMsg);
        break;
    case CMD_DEF_GET_SHOWSPECIFYIP:  /* 查看指定IP */
        InnerCmdShowSpecifyIP((MSG_COMM_OAM*)ptOamMsg,dwMsgId,ptRtnMsg);        
        break;
    case CMD_TELNET_SHOW_TERMINAL: /*显示idletime的时间*/
        InnerCmdShowTelnetIdleTimeOut((MSG_COMM_OAM*)ptOamMsg,dwMsgId,ptRtnMsg);    
        break;    
    case CMD_DEF_GET_PASSWORDINTENSITY:  /* 查看密码强度*/
        InnerCmdShowPasswordintensity((MSG_COMM_OAM*)ptOamMsg,dwMsgId,ptRtnMsg);
        break;
    case CMD_DEF_GET_SIMPLEPASSLENGTH:  /* 查看弱密码最低长度 */
        InnerCmdShowSimplePasswordLen((MSG_COMM_OAM*)ptOamMsg,dwMsgId,ptRtnMsg);
        break;    
    case CMD_TELNET_WHO:           /*查看在线用户*/
        InnerCmdShowOnlineUsers((MSG_COMM_OAM*)ptOamMsg,dwMsgId,ptRtnMsg);
        break;        
    case CMD_LDB_SET_CLI_LANGTYPE:
        InnerCmdSetCliLangType((MSG_COMM_OAM*)ptOamMsg,dwMsgId,ptRtnMsg);
        break;
    case CMD_LDB_GET_CLI_LANGINFO:
        InnerCmdGetCliLangInfo((MSG_COMM_OAM*)ptOamMsg,dwMsgId,ptRtnMsg);
        break;        
    default:
        break;
    }
    return;
}

void InnerCmdSetCliLangType(MSG_COMM_OAM *ptOamMsg,WORD32 dwMsgId,MSG_COMM_OAM *ptRtnMsg)
{        
    OPR_DATA *pRevOprData = (OPR_DATA *)ptOamMsg->Data;
    OPR_DATA *pRtnOprData = NULL;
    WORD16 i = 0;
    BOOL bSucceed = FALSE;
    WORD16 wLangType = 0;
    CHAR *pcLangType = NULL;
    BOOL bValidateNow = TRUE;
   /* T_Cli_LangType tLangType;*/
    INT iLinkStateType;
//    T_DAT_LINK *pCfgDat;
    JID tJidSelf;
    
    ptRtnMsg->Number = 0;
    pRtnOprData = (OPR_DATA *)ptRtnMsg->Data;
    
    for (i = 0; i < ptOamMsg->Number; i++)
    {
        SWITCH(pRevOprData->ParaNo)
        {
            CASE(1)
                memcpy(&wLangType, pRevOprData->Data, pRevOprData->Len);
                BREAK
            CASE(1)
                bValidateNow = FALSE; /*const 类型 */
                BREAK
            DEFAULT
                BREAK
        }
        END
        pRevOprData = GetNextOprData(pRevOprData);
    }

    /*根据mapitem值查找对应语言字符串*/
    Oam_InnerCmdGetPtVarJidinfo(&tJidSelf);    
    /*注册时候jid里面路由类型修改过*/
#if 0
    tJidSelf.ucRouteType = COMM_MASTER_SERVICE;
    pCfgDat = Oam_CliSearchDat(tJidSelf);
    if (pCfgDat == NULL)
    {
        ptRtnMsg->ReturnCode = ERR_AND_HAVEPARA;
        pRtnOprData->ParaNo = ptRtnMsg->Number;
        ptRtnMsg->Number++;
        pRtnOprData->Type = DATA_TYPE_STRING;
        XOS_snprintf((CHAR *)pRtnOprData->Data, MAX_STRING_LEN,
               "Failed, get language map define failed! can not change language\n");
       pRtnOprData->Len = strlen((CHAR *)pRtnOprData->Data) + 1;
       bSucceed = FALSE;
    }
    else
#endif
    {
       // pcLangType = OamCfgConvertWordToString(&(pCfgDat->datIndex), MAPTYPE_CLI_LANGTYPE, wLangType);
        if (wLangType == 1)
        {
            pcLangType = "Chinese";
        }
        else
        {
            pcLangType = "English";
        }
		
        if (Oam_String_Compare_Nocase(pcLangType, "MapErr", strlen("MapErr")) == 0)
        {
            bSucceed = FALSE;
        }
        else
        {
            bSucceed = OAM_CliSetLanguageType(pcLangType);
        }

        if (bSucceed)
        {
            if (bValidateNow)
            {
                 ptRtnMsg->ReturnCode = SUCC_AND_HAVEPARA;
                 pRtnOprData->ParaNo = ptRtnMsg->Number;
                 pRtnOprData->Type = DATA_TYPE_STRING;
                 ptRtnMsg->Number++;
                 XOS_snprintf((CHAR *)pRtnOprData->Data, MAX_STRING_LEN,
            "Success, and the CLI script files will be changed to %s language when this command finish.\nYou can see the change result with 'show script-language' command.\n", pcLangType);
                pRtnOprData->Len = strlen((CHAR *)pRtnOprData->Data) + 1;
            }
            else
            {
                 ptRtnMsg->ReturnCode = SUCC_AND_HAVEPARA;
                 pRtnOprData->ParaNo = ptRtnMsg->Number;
                 ptRtnMsg->Number++;
                 pRtnOprData->Type = DATA_TYPE_STRING;
                 XOS_snprintf((CHAR *)pRtnOprData->Data, MAX_STRING_LEN,
            "Success, and the CLI script files will be changed to %s language when system load script files next time.\n", pcLangType);
                pRtnOprData->Len = strlen((CHAR *)pRtnOprData->Data) + 1;
            }
        }
        else
        {
            ptRtnMsg->ReturnCode = ERR_AND_HAVEPARA;
            pRtnOprData->ParaNo = ptRtnMsg->Number;
            ptRtnMsg->Number++;
            pRtnOprData->Type = DATA_TYPE_STRING;
            XOS_snprintf((CHAR *)pRtnOprData->Data, MAX_STRING_LEN,
               "Failed, can not change to language type %s\n", pcLangType);
           pRtnOprData->Len = strlen((CHAR *)pRtnOprData->Data) + 1;
        }
    }
    
    ptRtnMsg->DataLen = GetMsgStructSize(ptRtnMsg, pRtnOprData);

    /*由于后面还要切换配置模式，这里不用给自己发消息的方式处理回显*/
    iLinkStateType = OamCfgGetLinkStateType(ptRtnMsg);
    if (LINKSTATE_TYPE_XML == iLinkStateType)
    {
//        RecvFromProtocolToOMM(ptRtnMsg);
    }
    else
    {
        BOOLEAN bCmdExecRslt;
        if (ptRtnMsg->ReturnCode == SUCC_AND_NOPARA ||
            ptRtnMsg->ReturnCode == SUCC_AND_HAVEPARA ||
            ptRtnMsg->ReturnCode == SUCC_CMD_NOT_FINISHED)
        {
            bCmdExecRslt = TRUE;
        }
        else
        {
            bCmdExecRslt = FALSE;
        }
    }

    /*如果命令执行成功，需要切换脚本语言类型*/
    if (bSucceed && bValidateNow)
    {
        XOS_SendAsynMsg(EV_OAM_CLI_SCRIPT_LANGUAGE_CHANGED, (BYTE*)ptRtnMsg, (WORD16)ptRtnMsg->DataLen,
                        0, 0,&tJidSelf);
    }
    
    return;
}

void InnerCmdGetCliLangInfo(MSG_COMM_OAM *ptOamMsg,WORD32 dwMsgId,MSG_COMM_OAM *ptRtnMsg)
{
    OPR_DATA *pRtnOprData = NULL;
//    BYTE i;
    BYTE *pStrCnt;
    T_Cli_LangType tLangType;
    JID tJidSelf;
//    WORD16 wDatSum=0;                                       /*版本管理表中所有dat文件数*/    
 //   CHAR aucLangType[OAM_CLI_LANGTYPE_LEN]={0};
 //   CHAR aucVerFileName[FILE_NAME_LEN]={0};   /**< 版本的文件名，数组大小为80，保活字符串结束符*/
 //   CHAR aucVerFilePath[FILE_PATH_LEN]={0};   /**< 版本文件存放路径，数组大小为80，保活字符串结束符*/    
    
    /*返回字符串数量*/
    pRtnOprData = (OPR_DATA *)ptRtnMsg->Data;
    pRtnOprData->ParaNo = ptRtnMsg->Number;
    pRtnOprData->Type = DATA_TYPE_BYTE;
    pRtnOprData->Len = 1;
    pStrCnt = (BYTE *)pRtnOprData->Data;
    *pStrCnt = 0;
    
    /*当前系统配置的脚本语言类型*/    
    pRtnOprData = (OPR_DATA *)(pRtnOprData->Data + pRtnOprData->Len);
    ptRtnMsg->Number++;
    pRtnOprData->ParaNo = ptRtnMsg->Number;
    pRtnOprData->Type = DATA_TYPE_STRING;
    
    if (!OAM_CliGetLanguageType(&tLangType))
    {         
        XOS_snprintf((CHAR *)pRtnOprData->Data, MAX_STRING_LEN,"Get script languagetype from database failed!\n" );
        pRtnOprData->Len = strlen((CHAR *)pRtnOprData->Data) + 1;
        ptRtnMsg->DataLen = (BYTE *)(pRtnOprData->Data + pRtnOprData->Len) - (BYTE *)ptRtnMsg;

        *pStrCnt = ptRtnMsg->Number;
        ptRtnMsg->Number++;
        ptRtnMsg->ReturnCode = ERR_AND_HAVEPARA;
        Oam_InnerCmdGetPtVarJidinfo(&tJidSelf);            

        XOS_SendAsynMsg(EV_OAM_CFG_CFG, (BYTE*)ptRtnMsg, (WORD16)ptRtnMsg->DataLen,
                        0, 0,&tJidSelf);
        /*Oam_CmdReturnMsgHandler(pLinkState, ptRtnMsg);*/
        return;
    }
    
    XOS_snprintf((CHAR *)pRtnOprData->Data, MAX_STRING_LEN,"System CLI script language type: %s\n", tLangType.aucLangType);
    pRtnOprData->Len = strlen((CHAR *)pRtnOprData->Data) + 1;

/*611002546549 【TECS】修改语言后，查询语言，打印的内容需要优化*/
#if 0
    /*循环添加所有已加载的dat文件语言类型信息*/
    Oam_GetDatSum(&wDatSum);    
    for(i = 0; i < wDatSum; i++)
    {
        CHAR aucTmpPath[255] = {0};
        Oam_GetLangType(i,aucLangType);        
        if (strlen(aucLangType) == 0)
        {/*没有加载的不显示*/
            continue;
        }
        Oam_GetVerFilePath(i,aucVerFilePath);
     XOS_snprintf(aucTmpPath, sizeof(aucTmpPath), "%s",aucVerFilePath);
     if (strlen(aucTmpPath) > 0)
        {
            if (aucTmpPath[strlen(aucTmpPath) - 1] == '/')
            {
                aucTmpPath[strlen(aucTmpPath) - 1] = '\0';
            }
        }
        pRtnOprData = (OPR_DATA *)(pRtnOprData->Data + pRtnOprData->Len);
        ptRtnMsg->Number++;
        pRtnOprData->ParaNo = ptRtnMsg->Number;
        pRtnOprData->Type = DATA_TYPE_STRING;
        Oam_GetVerFileName(i,aucVerFileName);        
        XOS_snprintf((CHAR *)pRtnOprData->Data, MAX_STRING_LEN,
            "file: %s/%s , language: %s\n",
                 aucTmpPath,
                 aucVerFileName,
                 aucLangType);
        pRtnOprData->Len = strlen((CHAR *)pRtnOprData->Data) + 1;
    }
#endif

    *pStrCnt = ptRtnMsg->Number;
    ptRtnMsg->Number++;
    ptRtnMsg->ReturnCode = SUCC_AND_HAVEPARA;
    ptRtnMsg->OutputMode = OUTPUT_MODE_NORMAL;
    ptRtnMsg->DataLen = (BYTE *)(pRtnOprData->Data + pRtnOprData->Len) - (BYTE *)ptRtnMsg;

    return;
}



