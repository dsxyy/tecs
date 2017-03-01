/**************************************************************************
* ��Ȩ���� (C)2002, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ�CliApp.c
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
* �޸ļ�¼2��
**************************************************************************/
/**************************************************************************
*                           ͷ�ļ�                                        *
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
* �������ƣ�VOID InnerCmdUser
* �����������������ӻ���ɾ���û�����
* ���ʵı���
* �޸ĵı���
* ���������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
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
    BYTE    ucUserRight =0;                            /* �û�Ȩ�� */
    static TYPE_CMD_LINE         tCmdLine;                /* ��������Ϣ */
    static T_CliCmdLogRecord       tCmdLog;          /* �����в�����־ */    
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
         /* ֻ�й���Ա�û�����Ȩ��ִ�д����� */        
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
		
        /*********�������е�������Ҫ����֮����д����־�ļ���������һ�´���*********/
        Oam_LinkGetCmdLine((LPVOID)ptOamMsg,dwMsgId,&tCmdLine);
        Oam_LinkGetCmdLog((LPVOID)ptOamMsg,dwMsgId,&tCmdLog);            
        memset(tCmdLog.szCmdLine, 0, sizeof(tCmdLog.szCmdLine));
        XOS_snprintf(tCmdLog.szCmdLine, sizeof(tCmdLog.szCmdLine) - 1, "%s %s %s", 
            tCmdLine.sCmdWord[0], sUserNme, tCmdLine.sCmdWord[4]);
        Oam_LinkSetCmdLog((LPVOID)ptOamMsg,dwMsgId,&tCmdLog);        
        /**************************************************************************/		
        /*��Ϊ������ʱ��������볤�ȴ������õ����볤�ȣ�
        ��Ϊǿ����ʱ��������볤�ȱ������8*/
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

        /* ֻ�й���Ա�û�����Ȩ��ִ�д����� */
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
        /*ǿ����ʱ����Ҫ�˶��Ƿ����ǿ�������*/
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

        /* ���ܳ�����ӦȨ�޵�����û��� */
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

        /* ������֤ͨ������ʼд���û���Ϣ�����ļ� */
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
* �������ƣ�VOID InnerCmdSetPasswordintensity
* ������������������ǿ��
* ���ʵı���
* �޸ĵı���
* ���������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
**************************************************************************/
static void InnerCmdSetPasswordintensity(MSG_COMM_OAM *ptOamMsg,WORD32 dwMsgId,MSG_COMM_OAM *ptRtnMsg)
{
    MSG_COMM_OAM *ptSendMsg = (MSG_COMM_OAM *)ptOamMsg;
    OPR_DATA *pOprData = ptSendMsg->Data;
    WORD16 wValue=0;
    BYTE ucValue=0;
    BYTE    ucUserRight =0;     
    
     /* ֻ�й���Ա�û�����Ȩ��ִ�д����� */    
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
* �������ƣ�VOID InnerCmdShowPasswordintensity
* ������������ʾ����ǿ��
* ���ʵı���
* �޸ĵı���
* ���������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
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
* �������ƣ�VOID InnerCmdSetSimplePasswordLen
* �����������������������С����
* ���ʵı���
* �޸ĵı���
* ���������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
**************************************************************************/
static void InnerCmdSetSimplePasswordLen(MSG_COMM_OAM *ptOamMsg,WORD32 dwMsgId,MSG_COMM_OAM *ptRtnMsg)
{
    MSG_COMM_OAM *ptSendMsg = (MSG_COMM_OAM *)ptOamMsg;
    OPR_DATA *pOprData = ptSendMsg->Data;
    BYTE ucSimplePasswordLen=0;
    BYTE    ucUserRight =0;                            /* �û�Ȩ�� */    
    
    Oam_LinkGetUserRight((LPVOID)ptOamMsg,dwMsgId,&ucUserRight);         
     /* ֻ�й���Ա�û�����Ȩ��ִ�д����� */        
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
* �������ƣ�VOID InnerCmdShowSimplePasswordLen
* ������������ʾ���������С�̶�
* ���ʵı���
* �޸ĵı���
* ���������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
**************************************************************************/
static void InnerCmdShowSimplePasswordLen(MSG_COMM_OAM *ptOamMsg,WORD32 dwMsgId,MSG_COMM_OAM *ptRtnMsg)
{    
    BYTE ucSimplePasswordLen =0;    
    Oam_GetNorPasslesslen(&ucSimplePasswordLen);
    AddParaToMsg (ptRtnMsg,DATA_TYPE_BYTE,(BYTE *)&ucSimplePasswordLen,sizeof(BYTE));        
    return ;    

}
/**************************************************************************
* �������ƣ�VOID InnerCmdShowOnlineUsers
* ������������ʾ�����û���Ϣ
* ���ʵı���
* �޸ĵı���
* ���������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
**************************************************************************/
static void InnerCmdShowOnlineUsers(MSG_COMM_OAM *ptOamMsg,WORD32 dwMsgId,MSG_COMM_OAM *ptRtnMsg)
{    
    CHAR szDisplay[256] = {0};    
    CHAR *pDisplay = szDisplay;
    BYTE i;    
    BYTE iLoop = 0;/*���ƽű���ѭ������*/
    BYTE *piLoopCnt = NULL; /*ѭ�����ص��ַ�����Ŀ*/
    CHAR    tnIpAddr[LEN_IP_ADDR_STR] ={0};              /* ����Telnet�ն�IP��ַ */
    WORD16 wIdleTimeout=0;                    /* Telnet�˿��г�ʱ���� */
    CHAR    sUserName[MAX_USERNAME_LEN]={0};       /* �û��� */    
    
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
* �������ƣ�VOID InnerCmdSetUserPwd
* ���������������޸��û���������
* ���ʵı���
* �޸ĵı���
* ���������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
**************************************************************************/
static void InnerCmdSetUserPwd(MSG_COMM_OAM *ptOamMsg,WORD32 dwMsgId,MSG_COMM_OAM *ptRtnMsg)
{
    MSG_COMM_OAM *ptSendMsg = ptOamMsg;
    OPR_DATA *pOprData = NULL;
    CHAR  sLinkUserName[MAX_USERNAME_LEN];       /* �û��� */    
    CHAR  sUserName[MAX_OAM_USER_NAME_LEN];
    CHAR  sPassWord1[MAX_OAM_USER_PWD_LEN];
    CHAR  sPassWord2[MAX_OAM_USER_PWD_LEN];
    BYTE  ucPassWord1Len=0;
    BYTE  ucPassWord2Len=0;            
    TYPE_USER_INFO tUserInfo;
    BYTE ucPasswordIntensity=0;
    BYTE ucNorPasslesslen=0;        
    CHAR szPwd1MD5[MAX_OAM_USER_PWD_ENCRYPT_LEN] = {0};
    BYTE    ucUserRight =0;                            /* �û�Ȩ�� */
    static TYPE_CMD_LINE         tCmdLine;                /* ��������Ϣ */
    static T_CliCmdLogRecord       tCmdLog;          /* �����в�����־ */    
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
	
    /*********�������е�������Ҫ����֮����д����־�ļ���������һ�´���*********/
    Oam_LinkGetCmdLine((LPVOID)ptOamMsg,dwMsgId,&tCmdLine);
    Oam_LinkGetCmdLog((LPVOID)ptOamMsg,dwMsgId,&tCmdLog);        
    memset(tCmdLog.szCmdLine, 0, sizeof(tCmdLog.szCmdLine));
    XOS_snprintf(tCmdLog.szCmdLine, sizeof(tCmdLog.szCmdLine) - 1, "%s %s", 
        tCmdLine.sCmdWord[0], sUserName);
    Oam_LinkSetCmdLog((LPVOID)ptOamMsg,dwMsgId,&tCmdLog);            
    /**************************************************************************/
	
    /*��Ϊ������ʱ��������볤�ȴ������õ����볤�ȣ�
       ��Ϊǿ����ʱ��������볤�ȱ������8*/
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


    /* �����û��Ƿ���� */
    memset(&tUserInfo, 0, sizeof(TYPE_USER_INFO));
    if (!FindUserInfoFromLink(sUserName, &tUserInfo))
    {
        AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)UserNotExists);
        ptRtnMsg->ReturnCode = ERR_AND_HAVEPARA;            
        return;
    }        

    /* ��������á��鿴Ȩ�޵��û�����ֻ���޸��Լ������� */
    Oam_LinkGetUserRight((LPVOID)ptOamMsg,dwMsgId,&ucUserRight);    
    Oam_LinkGetUserName((LPVOID)ptOamMsg,dwMsgId,sLinkUserName);    
    if (((ucUserRight == USER_ROLE_CONFIG) || (ucUserRight == USER_ROLE_VIEW)) && 
        (strcmp(sLinkUserName, sUserName) != 0))
    {
        AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)CannotSetOtherUserPwd);
        ptRtnMsg->ReturnCode = ERR_AND_HAVEPARA;            
        return;    
    }
    /*ǿ����ʱ����Ҫ�˶��Ƿ����ǿ�������*/
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
* �������ƣ�VOID InnerCmdSetUserRole
* ���������������޸��û�Ȩ������
* ���ʵı���
* �޸ĵı���
* ���������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
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
    BYTE    ucUserRight =0;                            /* �û�Ȩ�� */    
    pOprData = ptSendMsg->Data;
    
     /* ֻ�й���Ա�û�����Ȩ��ִ�д����� */        
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

    /* �����û��Ƿ���� */
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
    /*�����û�����������Ȩ��*/
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

     /* Ȩ��������У�� */
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
* �������ƣ�VOID InnerCmdShowUser
* ����������������ʾ�û���Ϣ
* ���ʵı���
* �޸ĵı���
* ���������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
**************************************************************************/
static void InnerCmdShowUser(MSG_COMM_OAM *ptOamMsg,WORD32 dwMsgId,MSG_COMM_OAM *ptRtnMsg)    
{ 
    WORD16 i = 0;
    CHAR szDisplay[512] = {0};
    CHAR *pDisplay = szDisplay;    
    CHAR aucRole[10]={0};
    CHAR aucLock[5]={0};
    int Number=0;/*��ʾ�����к�*/
    BYTE iLoop = 1;/*���ƽű���ѭ������*/
    BYTE *piLoopCnt = NULL; /*ѭ�����ص��ַ�����Ŀ*/    
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
        /* Ȩ�� */
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

        /* �Ƿ����� */
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
* �������ƣ�VOID InnerCmdUnlockUser
* ������������������û�
* ���ʵı���
* �޸ĵı���
* ���������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
**************************************************************************/
static void InnerCmdUnlockUser(MSG_COMM_OAM *ptOamMsg,WORD32 dwMsgId,MSG_COMM_OAM *ptRtnMsg)
{
    MSG_COMM_OAM *ptSendMsg = ptOamMsg;
    OPR_DATA *pOprData = NULL;
    CHAR  sUserName[MAX_OAM_USER_NAME_LEN];
    TYPE_USER_INFO tUserInfo;
    BYTE    ucUserRight =0;                            /* �û�Ȩ�� */    
    pOprData = ptSendMsg->Data;
    
    Oam_LinkGetUserRight((LPVOID)ptOamMsg,dwMsgId,&ucUserRight);           
    /* ֻ�й���Ա�û�����Ȩ��ִ�д����� */
    if (USER_ROLE_ADMIN != ucUserRight)
    {
        AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)NotAdmin);
        ptRtnMsg->ReturnCode = ERR_AND_HAVEPARA;            
        return;
    }        
    /*para1: username*/
    memset(sUserName, 0, MAX_OAM_USER_NAME_LEN);
    memcpy(sUserName,pOprData->Data,pOprData->Len);

    /* �����û��Ƿ���� */
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
* �������ƣ�VOID InnerCmdSetSpecIP
* ���������������ض�IP��Ϣ
* ���ʵı���
* �޸ĵı���
* ���������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
**************************************************************************/
 static void InnerCmdSetSpecIP(MSG_COMM_OAM *ptOamMsg,WORD32 dwMsgId,MSG_COMM_OAM *ptRtnMsg)
{
    CHAR TempStr[LEN_IP_ADDR_STR] = {0};
    BYTE    ucUserRight =0;                            /* �û�Ȩ�� */    
    
    Oam_LinkGetUserRight((LPVOID)ptOamMsg,dwMsgId,&ucUserRight);          
    /* ֻ�й���Ա�û�����Ȩ��ִ�д����� */
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
* �������ƣ�VOID InnerCmdSetTelnetIdleTimeOut
* ��������������Telnet�Ŀ���ʱ��
* ���ʵı���
* �޸ĵı���
* ���������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
**************************************************************************/
 static void InnerCmdSetTelnetIdleTimeOut(MSG_COMM_OAM *ptOamMsg,WORD32 dwMsgId,MSG_COMM_OAM *ptRtnMsg)
{
    /*��̬�ֲ�����*/
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
* �������ƣ�VOID InnerCmdShowTelnetIdleTimeOut
* �����������鿴Telnet�Ŀ���ʱ��
* ���ʵı���
* �޸ĵı���
* ���������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
**************************************************************************/
static void InnerCmdShowTelnetIdleTimeOut(MSG_COMM_OAM *ptOamMsg,WORD32 dwMsgId,MSG_COMM_OAM *ptRtnMsg)
{
    CHAR TempStr[MAX_STRING_LEN] = {0};
    WORD16 wDay,wHour,wMinute,wSecond;
    /*��̬�ֲ�����*/    
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
* �������ƣ�VOID InnerCmdShowUserLog
* ������������ʾ�û���־��Ϣ
* ���ʵı���
* �޸ĵı���
* ���������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
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
    BYTE bFlag = 0;/*���Ʊ��*/    
    BYTE  i=0;
    BYTE ucMaxNote=(MAX_OAMMSG_LEN-sizeof(MSG_COMM_OAM))/256;/*һ��������¼����*/
    BYTE iLoop = 0;/*���ƽű���ѭ������*/
    BYTE *piLoopCnt = NULL; /*ѭ�����ص��ַ�����Ŀ*/    
    ptRtnMsg->OutputMode  = OUTPUT_MODE_REQUEST;    
    if (!OamGetPath(acOamDir, acMateDir))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "[InnerCmdShowUserLog]OamGetPath fail!\n");
        return;
    }
    XOS_snprintf(acUserFile, MAX_ABS_PATH_LEN, "%s%s", acOamDir, OAM_USER_LOG_FILE);
    /* ��ԭ�ļ� */
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
    /*��ȡ�ļ�����ʧ��*/
    if (dwRet != XOS_SUCCESS)
    {
        bFlag = 0;
        AddParaToMsg(ptRtnMsg , DATA_TYPE_BYTE,&bFlag, sizeof(BYTE));   
     AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)GetFileLengthFail);    
        ptRtnMsg->ReturnCode = ERR_AND_HAVEPARA;         
        return ;
    }    
    /*�ļ�����Ϊ0*/
    if(0 == sdwFileLen)
    {
        bFlag = 0;
        AddParaToMsg(ptRtnMsg , DATA_TYPE_BYTE,&bFlag, sizeof(BYTE));   
     AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)FileLengthNotRigth);      
        ptRtnMsg->ReturnCode = ERR_AND_HAVEPARA;         
        return ;
    }    
    /* ��ȡ�ļ�ͷ */
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
    /*��һ����*/
    if (ptSendMsg->CmdRestartFlag)
    {        
        memset(ptSendMsg->LastData, 0, sizeof(ptSendMsg->LastData));        
    }
    memcpy(&dwOffset,ptSendMsg->LastData,sizeof(dwOffset));
    if(dwOffset ==0)
    {
        dwOffset =  tFileHead.dwLogBegin;
    }
    /*�ļ�ƫ�Ƶ�ַ�����ļ�����*/
    if (dwOffset >= (DWORD)sdwFileLen)
    {
        bFlag = 0;
        AddParaToMsg(ptRtnMsg , DATA_TYPE_BYTE,&bFlag, sizeof(BYTE));   
     AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)ReadFileFail);    
        ptRtnMsg->ReturnCode = ERR_AND_HAVEPARA;         
        return ;
    }   
    /* ��ԭ�ļ��ж�ȡ���ݣ�д����ʱ�ļ� */
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
        /* ��ԭ�ļ��ж�ȡÿ����¼ */
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
        /*�����ַ�*/
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
        /*���汾���ļ���Ϣ*/
        memcpy(ptRtnMsg->LastData, &dwOffset, sizeof(dwOffset));
    } 
    XOS_CloseFile(fd);    
    return;    
}
/**************************************************************************
* �������ƣ�VOID InnerCmdShowSpecifyIP
* ������������ʾ�ض�IP��Ϣ
* ���ʵı���
* �޸ĵı���
* ���������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
**************************************************************************/
static void InnerCmdShowSpecifyIP(MSG_COMM_OAM *ptOamMsg,WORD32 dwMsgId,MSG_COMM_OAM *ptRtnMsg)
{
    CHAR szDisplayStr[LEN_IP_ADDR_STR] = {0};    
    Oam_GetSpecifyIP(szDisplayStr);
    AddParaToMsg (ptRtnMsg,DATA_TYPE_STRING,(BYTE *)szDisplayStr,strlen((CHAR *)szDisplayStr)+1);        
    return;
}
/**************************************************************************
* �������ƣ�VOID InnerCmdShowModeStatck
* ������������ѯģʽջ
* ���ʵı���
* �޸ĵı���
* ���������MSG_COMM_OAM *ptOamMsg:OAM��������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2009/01/07    V1.0    ������      ����
**************************************************************************/
static void InnerCmdShowModeStatck(MSG_COMM_OAM *ptOamMsg,WORD32 dwMsgId,MSG_COMM_OAM *ptRtnMsg)
{
    MSG_COMM_OAM *ptSendMsg = ptOamMsg;
    WORD16 wLinkStateIndex = 0;
    DWORD dwLinkType = 0;
    BYTE bModeStackTop = 0;
    BYTE aucModeID[MAX_MODESTACK] = {0};
    
    /*�������*/
    if ((NULL == ptOamMsg)||(NULL == ptRtnMsg))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return;
    }
    ptRtnMsg->ReturnCode = SUCC_AND_NOPARA;
    
    dwLinkType = ((ptSendMsg->LinkChannel)&0X00000200)>>9; /*BIT9 1-��ʾXMLӦ��0-CLIӦ��*/
    /*��·����-Telnet*/
    if (LINKSTATE_TYPE_CLI == dwLinkType)
    {
        Oam_LinkGetModeStackTop((LPVOID)ptOamMsg,dwMsgId,&bModeStackTop); 
        Oam_LinkGetModeID((LPVOID)ptOamMsg,dwMsgId,aucModeID);        
    }
    else if(LINKSTATE_TYPE_XML == dwLinkType)/*��·-XML*/
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
    /*��������*/
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
    
    /*��ȡ��job��jid��Ϣ*/
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

    /*�ж��Ƿ���OMP*/
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

        /*�ж��Ƿ���LOMP*/
        if (!bIsOmp)
        {
            bIsLomp = IfModuleIsLomp(wModule);
        }
    }

    if ((!bIsOmp) && (!bIsLomp))
    {
        /*���ý����˵Ĵ���������������Ϣ���أ�
            ����ִ��״̬����Ϊtelnet��*/
        Oam_InnerSaveCmdLompErr((LPVOID)ptOamMsg,dwMsgId,Hdr_szErrInvalidModule);      
        return;
    }

    /*�ȴ�ZDB���ȵ�DBS���ؽ��֮�����ж��Ƿ���Ҫ��TXT*/
    
    /*����DBS��JID*/
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

    /* ����ͣ����ý�����Ϣ�ϱ��ȴ���ʱ�� */
    Oam_InnerKillCmdPlanTimer((LPVOID)ptOamMsg,dwMsgId);
    Oam_InnerSetCmdPlanTimer((LPVOID)ptOamMsg,dwMsgId);    
    XOS_SendAsynMsg(EV_SAVETABLE, (BYTE*)&tProDB, (WORD16)sizeof(tProDB),
                      0, 0,(void*)&tAppJid);
    /*���÷�������*/
    ptRtnMsg->ReturnCode = SUCC_AND_NOPARA;    
    return;    

}
/**************************************************************************
* �������ƣ�VOID Oam_InnerApp
* �����������ڲ���������
* ���ʵı���
* �޸ĵı���
* ���������
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2007/06/05    V1.0    ����      ����
**************************************************************************/
void Oam_InnerApp(LPVOID ptOamMsg,WORD32 dwMsgId,MSG_COMM_OAM *ptRtnMsg)
{
    /*�������*/
    if ((NULL == ptOamMsg)||(NULL == ptRtnMsg))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return;
    }
    
    /*ptRtnMsg���ؽṹ��ĳ�ʼ��*/
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
    
    /*����������������Ϣ����db���ص���Ϣ*/
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
    BYTE bFlag = 0;/*���Ʊ��*/
    _db_t_saveresult *pSaveResult  = NULL;
    WORD16 wDbNum = 0;
    
    /*�������*/
    if ((NULL == ptOamMsg)||(NULL == ptRtnMsg))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return;
    }
    
    pSaveResult = (_db_t_saveresult  *)ptOamMsg;
    /*��ʼ��ptRthMsg*/
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
            /** @brief ����Ŀ¼���̶�ʧ��*/
            if (pSaveResult->tDatabaseSaveResult[wDbNum].wReason == _DB_E_SAVE_BOTHPATH)
            {
                AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)SaveZDBErrSaveBothPath);               
            }
            /** @brief ��Ŀ¼����ʧ��*/
            else if (pSaveResult->tDatabaseSaveResult[wDbNum].wReason == _DB_E_SAVE_MAINPATH)
            {
                AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)SaveZDBErrMainPath);                           
            }
            /** @brief��Ŀ¼����ʧ��*/
            else if (pSaveResult->tDatabaseSaveResult[wDbNum].wReason == _DB_E_SAVE_BACKUP)
            {
                AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)SaveZDBErrBackup);           
            }
            /** @briefû�б���Ҫ����*/
            else if (pSaveResult->tDatabaseSaveResult[wDbNum].wReason == _DB_NOT_TABLE_SAVE)
            {
                AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)SaveZDBErrNoTblSave);             
            }
            /** @brief��ȡ�ļ����ȴ���*/
            else if (pSaveResult->tDatabaseSaveResult[wDbNum].wReason == _DB_READ_FILE_ERROR)
            {
                AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)SaveZDBErrReadFileErr);             
            }
            /** @brief����Ҫ����*/
            else if (pSaveResult->tDatabaseSaveResult[wDbNum].wReason == _DB_TABLE_NOT_NEED_SAVE)
            {
                AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)SaveZDBErrTblNotNeedSave);             
            }
            /** @brief �����ڽ��б������� */
            else if (pSaveResult->tDatabaseSaveResult[wDbNum].wReason == _DB_E_LOCAL_SAVING)
            {
                AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)SaveZDBErrLocalSaving);            
            }
            /** @brief �����ڽ����������� */
            else if (pSaveResult->tDatabaseSaveResult[wDbNum].wReason == _DB_E_MS_SAVING)
            {
                AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)SaveZDBErrMSSaving);             
            }
            /** @brief �����ڽ����ϵ���� */
            else if (pSaveResult->tDatabaseSaveResult[wDbNum].wReason == _DB_E_POWERON_LOADING)
            {
                AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)SaveZDBErrPowerOnLoading);            
            }
            /** @brief �����ڽ���RX FTP���� */
            else if (pSaveResult->tDatabaseSaveResult[wDbNum].wReason == _DB_E_FTP_LOADING)
            {
                AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)SaveZDBErrFtpLoading);             
            }
            /** @brief �����ڽ���APP FTP���� */
            else if (pSaveResult->tDatabaseSaveResult[wDbNum].wReason == _DB_E_APPFTP_LOADING)
            {
                AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)SaveZDBErrAppFtpLoading);             
            }
            /** @brief IO����æ */
            else if (pSaveResult->tDatabaseSaveResult[wDbNum].wReason == _DB_E_IOBUSY)
            {
                AddMapParaToMsg(ptRtnMsg,MAP_TYPE_INT_INNER_ERROR, (WORD16)SaveZDBErrIOBusy);            
            }
            /** @brief �Ƿ���� */
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
    
    /*��ӡ�շ���Ϣ*/
    if (bDebugPrint)
    {
        PrintMSGCOMMOAMStruct((MSG_COMM_OAM *)ptOamMsg,"MSG_CLIMANAGER_TO_INNERAPP");
    }
    
    switch(dwExeID)
    {
    case CMD_DEF_SET_USER:           /* ������ɾ���û� */
        InnerCmdUser(ptRevMsg,dwMsgId,ptRtnMsg);
        break;    
    case CMD_DEF_SET_PASSWORD:       /* �����û����� */
        InnerCmdSetUserPwd((MSG_COMM_OAM*)ptOamMsg,dwMsgId,ptRtnMsg);
        break;        
    case CMD_DEF_SET_ROLE:           /* �����û�Ȩ�� */
        InnerCmdSetUserRole((MSG_COMM_OAM*)ptOamMsg,dwMsgId,ptRtnMsg);
        break;        
    case CMD_DEF_SET_UNLOCKUSER:     /* �������û� */
        InnerCmdUnlockUser((MSG_COMM_OAM*)ptOamMsg,dwMsgId,ptRtnMsg);
        break;
    case CMD_DEF_SET_SPECIFYIP:      /* ����ָ��IP */
        InnerCmdSetSpecIP((MSG_COMM_OAM*)ptOamMsg,dwMsgId,ptRtnMsg);
        break;
    case CMD_DEF_SET_SAVE:    /* ���̲��� */
    case CMD_RPU_SAVE:
        InnerCmdExecSaveCmd((MSG_COMM_OAM*)ptOamMsg,dwMsgId,ptRtnMsg);
        break;    
    case CMD_LDB_SET_TELNET_IDLE_TIMEOUT:  /*����idletime��ʱ��*/
        InnerCmdSetTelnetIdleTimeOut((MSG_COMM_OAM*)ptOamMsg,dwMsgId,ptRtnMsg);
        break;    
    case CMD_DEF_SET_PASSWORDINTENSITY:  /* ��������ǿ��*/
        InnerCmdSetPasswordintensity((MSG_COMM_OAM*)ptOamMsg,dwMsgId,ptRtnMsg);
        break;
    case CMD_DEF_SET_SIMPLEPASSLENGTH:  /* ������������ͳ��� */
        InnerCmdSetSimplePasswordLen((MSG_COMM_OAM*)ptOamMsg,dwMsgId,ptRtnMsg);
        break;        
    case CMD_LDB_GET_MODESTATCK:/*��ѯģʽջ*/
        InnerCmdShowModeStatck((MSG_COMM_OAM*)ptOamMsg,dwMsgId,ptRtnMsg);
        break;
    case CMD_DEF_GET_SHOWUSER:       /* �鿴�û���Ϣ */
        InnerCmdShowUser((MSG_COMM_OAM*)ptOamMsg,dwMsgId,ptRtnMsg);
        break;    
    case CMD_DEF_GET_SHOWUSERLOG:    /* �鿴�û���¼�ǳ���־ */
        InnerCmdShowUserLog((MSG_COMM_OAM*)ptOamMsg,dwMsgId,ptRtnMsg);
        break;
    case CMD_DEF_GET_SHOWSPECIFYIP:  /* �鿴ָ��IP */
        InnerCmdShowSpecifyIP((MSG_COMM_OAM*)ptOamMsg,dwMsgId,ptRtnMsg);        
        break;
    case CMD_TELNET_SHOW_TERMINAL: /*��ʾidletime��ʱ��*/
        InnerCmdShowTelnetIdleTimeOut((MSG_COMM_OAM*)ptOamMsg,dwMsgId,ptRtnMsg);    
        break;    
    case CMD_DEF_GET_PASSWORDINTENSITY:  /* �鿴����ǿ��*/
        InnerCmdShowPasswordintensity((MSG_COMM_OAM*)ptOamMsg,dwMsgId,ptRtnMsg);
        break;
    case CMD_DEF_GET_SIMPLEPASSLENGTH:  /* �鿴��������ͳ��� */
        InnerCmdShowSimplePasswordLen((MSG_COMM_OAM*)ptOamMsg,dwMsgId,ptRtnMsg);
        break;    
    case CMD_TELNET_WHO:           /*�鿴�����û�*/
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
                bValidateNow = FALSE; /*const ���� */
                BREAK
            DEFAULT
                BREAK
        }
        END
        pRevOprData = GetNextOprData(pRevOprData);
    }

    /*����mapitemֵ���Ҷ�Ӧ�����ַ���*/
    Oam_InnerCmdGetPtVarJidinfo(&tJidSelf);    
    /*ע��ʱ��jid����·�������޸Ĺ�*/
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

    /*���ں��滹Ҫ�л�����ģʽ�����ﲻ�ø��Լ�����Ϣ�ķ�ʽ�������*/
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

    /*�������ִ�гɹ�����Ҫ�л��ű���������*/
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
//    WORD16 wDatSum=0;                                       /*�汾�����������dat�ļ���*/    
 //   CHAR aucLangType[OAM_CLI_LANGTYPE_LEN]={0};
 //   CHAR aucVerFileName[FILE_NAME_LEN]={0};   /**< �汾���ļ����������СΪ80�������ַ���������*/
 //   CHAR aucVerFilePath[FILE_PATH_LEN]={0};   /**< �汾�ļ����·���������СΪ80�������ַ���������*/    
    
    /*�����ַ�������*/
    pRtnOprData = (OPR_DATA *)ptRtnMsg->Data;
    pRtnOprData->ParaNo = ptRtnMsg->Number;
    pRtnOprData->Type = DATA_TYPE_BYTE;
    pRtnOprData->Len = 1;
    pStrCnt = (BYTE *)pRtnOprData->Data;
    *pStrCnt = 0;
    
    /*��ǰϵͳ���õĽű���������*/    
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

/*611002546549 ��TECS���޸����Ժ󣬲�ѯ���ԣ���ӡ��������Ҫ�Ż�*/
#if 0
    /*ѭ����������Ѽ��ص�dat�ļ�����������Ϣ*/
    Oam_GetDatSum(&wDatSum);    
    for(i = 0; i < wDatSum; i++)
    {
        CHAR aucTmpPath[255] = {0};
        Oam_GetLangType(i,aucLangType);        
        if (strlen(aucLangType) == 0)
        {/*û�м��صĲ���ʾ*/
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



