#include <cstdlib>
#include <string>
#include <iostream>
#include <xmlrpc-c/girerr.hpp>
#include <xmlrpc-c/base.hpp>
#include <xmlrpc-c/client_simple.hpp>
#include "includes.h"
#include "oam_execute.h"
#include "xmlrpc_preprocess.h"

using namespace std;
using namespace xmlrpc_c;

#define XMLRPC_INT_NULL         (int)-1
#define XMLRPC_STR_NULL         ""

#define XMLRPCAPP_QUERY_COUNT       3

/*全局统一的返回码map*/
#define MAPTYPE_RETURNCODE        400

typedef map<string, xmlrpc_c::value> MAP_STRUCT;

static T_MethodConfig g_atMethodConfig[MAX_METHOD_NUM];
string serverUrl;
int g_iSyslogLevel = 0;

WORD32 tCliMsgBufMutexSem = 0;
typedef struct
{
    BOOLEAN bUsing;
    CHAR       aucSendBuf[MAX_OAMMSG_LEN];
    CHAR       aucRecvBuf[MAX_APPMSG_LEN];
}_PACKED_1_  T_CLI_MSG_BUF;

static T_CLI_MSG_BUF g_tCliMsgBufPool[MAX_CLI_VTY_NUMBER] = {{0}};
static T_CLI_MSG_BUF * GetCliMsgBuf();
static void RetCliMsgBuf(T_CLI_MSG_BUF *);

static BOOLEAN ConvertCliMsgToXmlRpcParam(MSG_COMM_OAM *pCliMsg, TYPE_LINK_STATE *pLinkState, xmlrpc_c::paramList &paramlist);
static BOOLEAN ConvertXmlRpcParamToCliMsg(TYPE_LINK_STATE *pLinkState, xmlrpc_c::value, MSG_COMM_OAM *pCliMsg);
static void AddSimpleTypeToCliMsg(xmlrpc_c::value xmlrpcValue, MSG_COMM_OAM *pCliMsg);
static void AddStructParaToMsg(xmlrpc_c::value xmlrpcValue, MSG_COMM_OAM *pCliMsg);

static void ExecCmd(TYPE_LINK_STATE *pLinkState);
static int GetMethodIdx(MSG_COMM_OAM *pCliMsg);

static BOOLEAN LoadMethodInfo(FILE *fp);
static BOOLEAN LoadCliSysConfig(FILE *fp);
static long FindSectionPos(FILE *fp, CHAR *pcSectionName);

BOOLEAN GetParaByNo(MSG_COMM_OAM *pCliMsg, int iParaNo, OPR_DATA *pData, int index = 0);
void AddDataToParamList(OPR_DATA *pData, TYPE_CMD_PARA *pCurParaInfo, xmlrpc_c::paramList &paramlist);

static void AddCommonParam(TYPE_LINK_STATE *pLinkState, MSG_COMM_OAM *pCliMsg, xmlrpc_c::paramList &paramlist);
typedef struct t_QueryIndex
{
     INT64 iStartIndex;
     INT64 iQueryCount;
}T_QueryIndex;
static void GetCommonOutParam(MSG_COMM_OAM *pCliMsg, xmlrpc_c::value result);

/*xmlrpc 支持的数据类型
enum xmlrpc_c::value::type_t {
        TYPE_INT        = 0,
        TYPE_BOOLEAN    = 1,
        TYPE_DOUBLE     = 2,
        TYPE_DATETIME   = 3,
        TYPE_STRING     = 4,
        TYPE_BYTESTRING = 5,
        TYPE_ARRAY      = 6,
        TYPE_STRUCT     = 7,
        TYPE_C_PTR      = 8,
        TYPE_NIL        = 9,
        TYPE_I8         = 10,
        TYPE_DEAD       = 0xDEAD
    };
*/
void DisposeXmlRpcCmd(TYPE_LINK_STATE *pLinkState)
{
    CHAR strJobName[20] = {0};
    XOS_TASK_T tTaskId;

    /*重置Execute端定时器*/
    Oam_KillExeTimer(pLinkState);
    /*611002503700 不设置cli'定时器*/
    //Oam_SetExeTimer(pLinkState);
    Oam_LinkSetCurRunState(pLinkState, pec_MsgHandle_EXECUTE);
	
    /*单独起线程运行命令*/
    sprintf(strJobName, "clijob_%d", pLinkState->ucLinkID);
    tTaskId = XOS_StartTask(strJobName, TSK_PRIO_PTYS_CLI, 4096*50, 0, (VOID (*)(void*))&ExecCmd, (WORDPTR) pLinkState);
    if (INVALID_SYS_TASKID == tTaskId)	
    {
       XOS_SysLog(OAM_CFG_LOG_EMERGENCY, "Start command execute task failed ! \n");
    }
	
    return; 
}

static void ExecCmd(TYPE_LINK_STATE *pLinkState)
{
    MSG_COMM_OAM *ptSendMsg;
    T_CLI_MSG_BUF *ptCliMsgBuf = GetCliMsgBuf();
    if (ptCliMsgBuf == NULL)
    {
        char *strErr = "ExecCmd: call GetCliMsgBuf failed, can not execute command\n";
        //ptSendMsg = (MSG_COMM_OAM *)g_ptOamIntVar->aucSendBuf;/*都用这个缓存需要考虑并发问题*/
        XOS_SysLog(OAM_CFG_LOG_EMERGENCY, strErr);
        pLinkState->wAppMsgReturnCode = ERR_AND_HAVEPARA;
        SendStringToTelnet(pLinkState, strErr);   
		
        Oam_LinkSetCurRunState(pLinkState, pec_MsgHandle_TELNET);
        Oam_KillExeTimer(pLinkState);
    
        return;
    }
    
    ptSendMsg = (MSG_COMM_OAM *)ptCliMsgBuf->aucSendBuf;/*都用这个缓存需要考虑并发问题*/
    
    ptSendMsg = OamCfgConstructMsgData(pLinkState, ptSendMsg);
    if (!ptSendMsg)
    {            
        SendErrStringToClient(pLinkState, map_szDisposeCmdFailed);
        RetCliMsgBuf(ptCliMsgBuf);
        return;
    }	
    ptSendMsg->CmdRestartFlag = pLinkState->CmdRestartFlagForXmlRpc;
    PrintMSGCOMMOAMStruct(ptSendMsg,"MSG_CLIMANAGER_TO_APP");
	
    try
    {  
        /*处理入参*/
        xmlrpc_c::paramList paramList;

         /*插入公共参数*/
        AddCommonParam(pLinkState, ptSendMsg, paramList);
	
        if (!ParamInPreProcess(ptSendMsg, paramList))
        {
            //如果没有注册钩子函数，进行默认处理
            BOOLEAN bRet = ConvertCliMsgToXmlRpcParam(ptSendMsg, pLinkState,paramList);
            if (!bRet)
            {
                /*返回参数转换错误提示*/  
                SendErrStringToClient(pLinkState, "xmlrpc error: convert param to paramlist failed!");
                Oam_LinkSetCurRunState(pLinkState, pec_MsgHandle_TELNET);
                Oam_KillExeTimer(pLinkState);
                RetCliMsgBuf(ptCliMsgBuf);
                return;
            }
        }
		
        /*远程调用*/
        xmlrpc_c::clientSimple myClient;
        xmlrpc_c::value result;
        int  iMethosIdx = GetMethodIdx(ptSendMsg);
        if (iMethosIdx < 0)
        {
            CHAR aucErr[1024] = {0};
            snprintf(aucErr, sizeof(aucErr), "Error:no methodname specifed to this command :0x%x", ptSendMsg->CmdID);
            pLinkState->wAppMsgReturnCode = ERR_AND_HAVEPARA;
            SendStringToTelnet(pLinkState, aucErr);   
		
            Oam_LinkSetCurRunState(pLinkState, pec_MsgHandle_TELNET);
            Oam_KillExeTimer(pLinkState);
        }
		
        myClient.call(serverUrl, g_atMethodConfig[iMethosIdx].aucMethodName, paramList, &result);

        /*处理返回值*/
        //MSG_COMM_OAM *pCliRetMsg = (MSG_COMM_OAM *)g_ptOamIntVar->aucRecvBuf;
        MSG_COMM_OAM *pCliRetMsg = (MSG_COMM_OAM *)ptCliMsgBuf->aucRecvBuf;
        memcpy(pCliRetMsg, (BYTE *)ptSendMsg, sizeof(MSG_COMM_OAM));
		
        GetCommonOutParam(pCliRetMsg, result);
	
	 /*处理其余的返回参数，如果没有注册钩子函数，调用默认处理函数*/
        if (!ParamOutPreProcess(ptSendMsg->CmdID, result, pCliRetMsg))
        {
            ConvertXmlRpcParamToCliMsg(pLinkState, result, pCliRetMsg);     
        }
		
        PrintMSGCOMMOAMStruct(pCliRetMsg,"MSG_XMLRPC_TO_CLIMANAGER"); 
		
        /*发送命令返回消息*/
        XOS_SendAsynMsg(EV_OAM_CFG_CFG, (BYTE *)pCliRetMsg, pCliRetMsg->DataLen, XOS_MSG_VER0, XOS_MSG_MEDIUM, &g_ptOamIntVar->jidSelf);
        RetCliMsgBuf(ptCliMsgBuf);	
    } 
    catch (exception const& e) 
    {
        CHAR aucErr[1024] = {0};
        snprintf(aucErr, sizeof(aucErr), "Error:%s",e.what());
        cerr << "Client threw error: " << aucErr << endl;
        pLinkState->wAppMsgReturnCode = ERR_AND_HAVEPARA;
        SendStringToTelnet(pLinkState, aucErr);   
		
        Oam_LinkSetCurRunState(pLinkState, pec_MsgHandle_TELNET);
        Oam_KillExeTimer(pLinkState);
    } 
    catch (...) 
    {
        Oam_LinkSetCurRunState(pLinkState, pec_MsgHandle_TELNET);
        Oam_KillExeTimer(pLinkState);
        cerr << "Client threw unexpected error." << endl;
    }
    RetCliMsgBuf(ptCliMsgBuf);
}

/*把回显数据转成tlv，目前是根据回显脚本格式化符号来转
如果xmlrpc_c可以自定义类型，增加map、ip地址等类型，可以根据返回数据本身获取类型信息，
就不需要根据回显脚本的格式化符号转了*/
static BOOLEAN ConvertXmlRpcParamToCliMsg(TYPE_LINK_STATE *pLinkState, xmlrpc_c::value result, MSG_COMM_OAM *pCliMsg)
{
    xmlrpc_c::value_array const arrayValue(result);
    vector<xmlrpc_c::value> rs = arrayValue.vectorValueValue();

    WORD16 iIdx;
    //第0个回显参数是返回码，下面从1开始
    for (iIdx = 1; iIdx < rs.size(); iIdx++)
    {
        if (rs[iIdx].type() == value::TYPE_STRUCT)
        {
            AddStructParaToMsg(rs[iIdx], pCliMsg);
        }
        else if (rs[iIdx].type() == value::TYPE_ARRAY)
        {
            ConvertXmlRpcParamToCliMsg(pLinkState, rs[iIdx], pCliMsg);
        }
        else
        {
            AddSimpleTypeToCliMsg(rs[iIdx], pCliMsg);
        }
    }

    return TRUE;
}

static void AddStructParaToMsg(xmlrpc_c::value xmlrpcValue, MSG_COMM_OAM *pCliMsg)
{
    MAP_STRUCT vStruct = xmlrpc_c::value_struct(xmlrpcValue);
    MAP_STRUCT::const_iterator it;

    for (it = vStruct.begin(); it != vStruct.end(); it++)
    {
        //不考虑嵌套结构体的情况，结构体里面都是简单类型
        AddSimpleTypeToCliMsg(it->second, pCliMsg);
    }
}

static void AddSimpleTypeToCliMsg(xmlrpc_c::value xmlrpcValue, MSG_COMM_OAM *pCliMsg)
{
    int iTmp;
    string strTmp;

    switch(xmlrpcValue.type())
    {
        case value::TYPE_BOOLEAN:
            iTmp = xmlrpc_c::value_boolean(xmlrpcValue);
            AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&iTmp, sizeof(int));
            break;
        case value::TYPE_INT:
            iTmp = xmlrpc_c::value_int(xmlrpcValue);
            AddParaToMsg(pCliMsg, DATA_TYPE_INT, (BYTE *)&iTmp, sizeof(int));
            break;
        case value::TYPE_I8:
            WORD64 qdwTmp;
            qdwTmp = xmlrpc_c::value_i8(xmlrpcValue);
            AddParaToMsg(pCliMsg, DATA_TYPE_WORD64, (BYTE *)&qdwTmp, sizeof(WORD64));
            break;
        case value::TYPE_BYTESTRING:
            break;
        case value::TYPE_STRING:
            strTmp = xmlrpc_c::value_string(xmlrpcValue);
            AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)strTmp.c_str(), strTmp.size());
            break;
        case value::TYPE_DATETIME:
            iTmp = xmlrpc_c::value_datetime(xmlrpcValue);
            AddParaToMsg(pCliMsg, DATA_TYPE_STRING, (BYTE*)&iTmp, sizeof(int));
            break;
            break;
        case value::TYPE_STRUCT:
        case value::TYPE_DOUBLE:
        case value::TYPE_ARRAY:
        case value::TYPE_C_PTR:
        case value::TYPE_NIL:
            break;
        default:
            XOS_SysLog(OAM_CFG_LOG_ERROR, "ConvertXmlRpcParamToCliMsg: unknown type %u \n",xmlrpcValue.type());
            break;
    }

}

BOOLEAN GetParaByNo(MSG_COMM_OAM *pCliMsg, int iParaNo, OPR_DATA *pData, int index)
{
    int iIdx;
    if (!pCliMsg || !pData)
    {
        return FALSE;
    }

    OPR_DATA *pOprData =  (OPR_DATA *)pCliMsg->Data;
    int iIdxTmp = 0;
    for (iIdx = 0; iIdx < pCliMsg->Number; iIdx++)
    {
        if(pOprData->ParaNo == iParaNo)
        {
            if (index == iIdxTmp)
            {
                memcpy(pData, pOprData, sizeof(OPR_DATA) + pOprData->Len - 1);
                return TRUE;
            }
            else
            {
                 iIdxTmp++;
            }
        }
        pOprData = GetNextOprData(pOprData);
    }
	 
    return FALSE;
}

static int GetMethodIdx(MSG_COMM_OAM *pCliMsg)
{
    if (!pCliMsg)
    {
        return -1;
    }
    int iIdx;
    for (iIdx = 0; iIdx < MAX_METHOD_NUM; iIdx++)
    {
        if(g_atMethodConfig[iIdx].dwCmdId == 0)
        {
            break;
        }
        
        if(g_atMethodConfig[iIdx].dwCmdId == pCliMsg->CmdID &&
            g_atMethodConfig[iIdx].bIsNo == pCliMsg->bIfNo)
        {
            return iIdx;
        }
    }

    return -1;
}

/*在配置文件中查找段位置*/
long FindSectionPos(FILE *fp, CHAR *pcSectionName)
{
    CHAR acLine[MAX_LINE_STR_LEN] = {0};
    CHAR *pcTmpLine = NULL;
	
    if (!fp || !pcSectionName)
    {
        return -1;
    }

    fseek(fp, 0, SEEK_SET);
    while (0 == feof(fp))
    {
        memset(acLine, 0, sizeof(acLine));
        if (NULL == fgets(acLine, MAX_LINE_STR_LEN - 1, fp))
        {
            break;
        }
        pcTmpLine = OAM_RTrim(OAM_LTrim(acLine));
  
        /*跳过注释 -- 开头的*/
        if (strlen(pcTmpLine) < 3)
        {
            continue;
        }
        else if (pcTmpLine[0] == '-' && pcTmpLine[1] == '-')
        {
            continue;
        }
        if (Oam_String_Compare_Nocase(pcTmpLine, pcSectionName, strlen(pcSectionName)) == 0)
        {
            break;
        }
    }

    if (0 != feof(fp))
    {
        return -1;
    }

    return ftell(fp);
}

BOOLEAN LoadCliSysConfig(FILE *fp)
{
    CHAR acLine[MAX_LINE_STR_LEN] = {0};
    CHAR *pcTmpLine = NULL;
    CHAR *pcUrlName = "server-url=";
    CHAR *pcSyslogLevel="syslog-level=";
	
    if (!fp)
    {
        return FALSE;
    }

    long lServerInfoPos = FindSectionPos(fp, "[sysconfig]");
    if (lServerInfoPos < 0)
    {
        return FALSE;
    }
	
    fseek(fp, lServerInfoPos, SEEK_SET);
    while (0 == feof(fp))
    {
        memset(acLine, 0, sizeof(acLine));
        if (NULL == fgets(acLine, MAX_LINE_STR_LEN - 1, fp))
        {
            break;
        }
        pcTmpLine = OAM_RTrim(OAM_LTrim(acLine));
        if (*pcTmpLine == '[')
        {//读取到下一段
            break;
        }
		
        /*跳过注释 -- 开头的*/
        if (strlen(pcTmpLine) < 3)
        {
            continue;
        }
        else if (pcTmpLine[0] == '-' && pcTmpLine[1] == '-')
        {
            continue;
        }

        if (Oam_String_Compare_Nocase(pcTmpLine, pcUrlName, strlen(pcUrlName)) == 0)
        {
            serverUrl = string(pcTmpLine + strlen(pcUrlName));
            XOS_SysLog(OAM_CFG_LOG_DEBUG, "LoadCliSysConfig: server url=%s\n", serverUrl.c_str());
        }
        else if (Oam_String_Compare_Nocase(pcTmpLine, pcSyslogLevel, strlen(pcSyslogLevel)) == 0)
        {
            CHAR *pcTmpLevel=pcTmpLine + strlen(pcSyslogLevel);
            g_iSyslogLevel = atoi(pcTmpLevel);
            if (g_iSyslogLevel < 0 ||  g_iSyslogLevel > 9)
            {
                g_iSyslogLevel = 5;
                XOS_SysLog(OAM_CFG_LOG_DEBUG, "LoadCliSysConfig: get g_iSyslogLevel failed\n");
            }
            XOS_SysLog(OAM_CFG_LOG_DEBUG, "LoadCliSysConfig: g_iSyslogLevel=%d\n", g_iSyslogLevel);
        }
    }

    if (serverUrl.length() == 0)
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

BOOLEAN LoadMethodInfo(FILE *fp)
{
    CHAR acLine[MAX_LINE_STR_LEN] = {0};
    CHAR *pcTmpLine = NULL;
    CHAR *pcNO = "no";
	
    if (!fp)
    {
        return FALSE;
    }

    long lMethodInfoPos = FindSectionPos(fp, "[method]");
    if (lMethodInfoPos < 0)
    {
        return FALSE;
    }
	
    fseek(fp, lMethodInfoPos, SEEK_SET);
    WORD16 wIdx = 0;
    CHAR acCmdId[20] = {0};
    CHAR *pcPos = NULL;
    memset(g_atMethodConfig, 0, sizeof(g_atMethodConfig));  
    /*
    methodconfig.ini 文件内每行结构:
    cmdid,methodname
    比如:
    0x82010058,sample.add
    */
    while (0 == feof(fp))
    {
        char *pcMethodNamePos;
        if (wIdx >= MAX_METHOD_NUM)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR, "LoadMethodInfo: too many methods  in %s!\n", OAM_CFG_METHOD_CONFIG_FILE);
            break;
        }
		
        memset(acLine, 0, sizeof(acLine));
        if (NULL == fgets(acLine, MAX_LINE_STR_LEN - 1, fp))
        {
            break;
        }
        pcTmpLine = OAM_RTrim(OAM_LTrim(acLine));
  
        /*跳过注释 -- 开头的*/
        if (strlen(pcTmpLine) < 3)
        {
            continue;
        }
        else if (pcTmpLine[0] == '-' && pcTmpLine[1] == '-')
        {
            continue;
        }

        /*读取命令id*/
        pcPos = strchr(pcTmpLine, '=');
        if (!pcPos)
        {
            continue;
        }
        memset(acCmdId, 0, sizeof(acCmdId));
        memcpy(acCmdId, pcTmpLine, pcPos - pcTmpLine);
        char *pcNoPos = strstr(acCmdId, pcNO);
        char *pcCmdIdBegin;
        if ( pcNoPos == NULL)
        {
            g_atMethodConfig[wIdx].bIsNo = FALSE;
            pcCmdIdBegin = acCmdId;
        }
        else
        {
            g_atMethodConfig[wIdx].bIsNo = TRUE;
            pcCmdIdBegin = pcNoPos + strlen(pcNO);
        }
		
        g_atMethodConfig[wIdx].dwCmdId = strtol(OAM_RTrim(OAM_LTrim(pcCmdIdBegin)), NULL, 0);
        
        if ((WORD32)(pcPos - pcTmpLine + 1) >= strlen(pcTmpLine))
        {
            continue;
        }

        /*读取methodname*/
        pcPos++;
        pcMethodNamePos = pcPos;
		
        strcpy(g_atMethodConfig[wIdx].aucMethodName, pcMethodNamePos);

        XOS_SysLog(OAM_CFG_LOG_DEBUG, "LoadMethodInfo: [%d]: cmdid=0x%x, is_no_cmd[%u], methodname=%s\n", 
                             wIdx, g_atMethodConfig[wIdx].dwCmdId, g_atMethodConfig[wIdx].bIsNo, g_atMethodConfig[wIdx].aucMethodName);
        wIdx++;
    }

    return TRUE;
}

BOOLEAN LoadClientConfigInfo()
{
    FILE *fp = NULL;

    CHAR  acOamDatCfgFile[FILE_PATH_LEN] = {'\0'};
    sprintf(acOamDatCfgFile, "%s/%s", g_aucScriptPath, OAM_CFG_METHOD_CONFIG_FILE);	
    fp = fopen(acOamDatCfgFile, "r");
    if (!fp)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "LoadClientConfigInfo: open %s error!\n", OAM_CFG_METHOD_CONFIG_FILE);
        return FALSE;
    }

    if (LoadCliSysConfig(fp) == FALSE)
    {
        fclose(fp);
        XOS_SysLog(OAM_CFG_LOG_ERROR, "LoadClientConfigInfo: Load server url error!\n");
        return FALSE;
    }

    if (LoadMethodInfo(fp) == FALSE)
    {
        fclose(fp);
        XOS_SysLog(OAM_CFG_LOG_ERROR, "LoadClientConfigInfo: Load method configure error!\n");
        return FALSE;
    }
	
    fclose(fp);
    
    return TRUE;

}

static BOOLEAN ConvertCliMsgToXmlRpcParam(MSG_COMM_OAM *pCliMsg, TYPE_LINK_STATE *pLinkState, xmlrpc_c::paramList &paramlist)
{
    T_OAM_Cfg_Dat *pDat = pLinkState->tMatchResult.cfgDat;
    TYPE_CMD_ATTR *pCmdAttr = &(pDat->tCmdAttr[pLinkState->wCmdAttrPos]);
    int iParamNum;
    TYPE_CMD_PARA *pCmdPara;

    if (pCliMsg->bIfNo)
    {
        iParamNum = pCmdAttr->wNoParaCount;
        pCmdPara = &(pDat->tCmdPara[pCmdAttr->dwParaPos + pCmdAttr->wCmdParaCount]);
    }
    else
    {
        iParamNum = pCmdAttr->wCmdParaCount;
        pCmdPara = &(pDat->tCmdPara[pCmdAttr->dwParaPos]);
    }

    int iParaIdx = 0;
    int iParaNo = 0;
    for (iParaIdx = 0; iParaIdx < iParamNum; iParaIdx++)
    {		
       TYPE_CMD_PARA *pCurPara = &(pCmdPara[iParaIdx]);
	
        /*判断语法类型节点是否占参数编号*/
        if(pCurPara->bStructType == STRUCTTYPE_PRAMMAR)
        {
            if(pCmdAttr->bExeModeID == EXEC_PROTOCOL)/*EXEC_PROTOCOL*/
            {
                if (!(pCurPara->uContent.tPrammar.bPrammarType == PRAMMAR_BRACKET_SQUARE &&
                       pCurPara->uContent.tPrammar.bItemCount == 0))
                {
                    continue;
                }
            }
            else	
            {
                if (!( (  pCurPara->uContent.tPrammar.bPrammarType == PRAMMAR_BRACKET_SQUARE &&
                             (*(pCurPara->uContent.tPrammar.sSymbol)) != MARK_STRINGEND
                          ) || 
                          pCurPara->uContent.tPrammar.bPrammarType == PRAMMAR_WHOLE_SYMBOL
                        )
                    )
                {
                    continue;
                }
            }
        }
       		 
        OPR_DATA *ptData = (OPR_DATA *)g_aucParaDataBuf;
        BOOLEAN bRet = GetParaByNo(pCliMsg, iParaNo, ptData);
        if (!bRet)
        {
            /*如果参数没有输入，填空值*/
            AddDataToParamList(NULL, pCurPara, paramlist);
        }
        else
        {
            AddDataToParamList(ptData, pCurPara, paramlist);
        }
		
        iParaNo++;
    }

    return TRUE;
}

void AddDataToParamList(OPR_DATA *pData, TYPE_CMD_PARA *pCurParaInfo, xmlrpc_c::paramList &paramlist)
{
    BYTE ucParaDataType;

    /*钩子函数中会传入空的pCurParaInfo，但pData不为空*/
    if (pCurParaInfo == NULL)
    {
        if (pData == NULL)
        {
            return;
        }
        ucParaDataType = pData->Type;
    }
    else
    {
        if (pCurParaInfo->bStructType == STRUCTTYPE_PRAMMAR)
        {
            ucParaDataType = DATA_TYPE_CONST;
        }
        else 
        {
            ucParaDataType = pCurParaInfo->uContent.tPara.bParaType;
        }
    }
	
    switch (ucParaDataType)
    {
    case DATA_TYPE_BYTE:
    case DATA_TYPE_WORD:
    case DATA_TYPE_DWORD:
    case DATA_TYPE_INT:
    case DATA_TYPE_SWORD:
    case DATA_TYPE_CONST:
    case DATA_TYPE_MAP:
    case DATA_TYPE_IPADDR:
    case DATA_TYPE_PARA:
        if (pData == NULL)
        {
            paramlist.add(xmlrpc_c::value_int(XMLRPC_INT_NULL));
        }
        else
        {
            int i = 0;
            int iLen = OAM_MIN(sizeof(int), pData->Len);
            memcpy(&i, pData->Data, iLen);
            paramlist.add(xmlrpc_c::value_int(i));
        }
        break;
    case DATA_TYPE_WORD64:
        if (pData == NULL)
        {
            paramlist.add(xmlrpc_c::value_i8(XMLRPC_INT_NULL));
        }
        else
        {
            WORD64 i = 0;
            int iLen = OAM_MIN(sizeof(WORD64), pData->Len);
            memcpy(&i, pData->Data, iLen);
            paramlist.add(xmlrpc_c::value_i8(i));
        }
        break;
    case DATA_TYPE_CHAR:
    case DATA_TYPE_STRING:
        if (pData == NULL)
        {
            paramlist.add(xmlrpc_c::value_string(XMLRPC_STR_NULL));
        }
        else
        {
            paramlist.add(xmlrpc_c::value_string((char*)pData->Data));
        }
        break;
    case DATA_TYPE_TIME:
        if (pData == NULL)
        {
            paramlist.add(xmlrpc_c::value_datetime(0));
        }
        else
        {
            int i = 0;
            int iLen = OAM_MIN(sizeof(int), pData->Len);
            memcpy(&i, pData->Data, iLen);
            paramlist.add(xmlrpc_c::value_datetime(i));
        }
        break;
    default:
        break;
    }
}
 
/*
cli登录认证
*/
bool CliLoginAuth(TYPE_LINK_STATE *pLinkState)
{
    /*构造临时的show user cli消息*/
    MSG_COMM_OAM tTmpCliMsg;
    tTmpCliMsg.CmdID = 0x97000003;
    tTmpCliMsg.bIfNo = 0;
	
    try
    {          
        xmlrpc_c::paramList paramList;
        /*首先插入认证字段*/
        string strAuth = string(pLinkState->sUserName)+ ":" + string(pLinkState->sPassword);
        paramList.add(xmlrpc_c::value_string(strAuth));
        paramList.add(xmlrpc_c::value_i8(0));
        paramList.add(xmlrpc_c::value_i8(XMLRPCAPP_QUERY_COUNT));
        paramList.add(xmlrpc_c::value_string(string(pLinkState->sUserName)));
		
        /*远程调用*/
        xmlrpc_c::clientSimple myClient;
        xmlrpc_c::value result;
        int  iMethosIdx = GetMethodIdx(&tTmpCliMsg);
        if (iMethosIdx < 0)
        {
            CHAR aucErr[1024] = {0};
            snprintf(aucErr, sizeof(aucErr), "Error:no methodname specifed to this command :0x%x", tTmpCliMsg.CmdID);
            XOS_SysLog(OAM_CFG_LOG_ERROR, "CliLoginAuth: %s!\n", aucErr);
            return false;
        }
		
        myClient.call(serverUrl, g_atMethodConfig[iMethosIdx].aucMethodName, paramList, &result);
	
        /*读取返回码*/
        xmlrpc_c::value_array const arrayValue(result);
        vector<xmlrpc_c::value> rs = arrayValue.vectorValueValue();
        int iTmp = xmlrpc_c::value_int(rs[0]);

        XOS_SysLog(OAM_CFG_LOG_ERROR, "CliLoginAuth: call show user return:%u!\n", iTmp);
        if (iTmp == 20)
        {
            memset(pLinkState->sSession, 0, sizeof(pLinkState->sSession));
            string strSession = xmlrpc_c::value_string(rs[2]);
            strncpy(pLinkState->sSession, strSession.c_str(), sizeof(pLinkState->sSession));

            XOS_SysLog(OAM_CFG_LOG_ERROR, "CliLoginAuth : call show user return session:%s!\n", pLinkState->sSession);
            
            xmlrpc_c::clientSimple AuthClient;
            xmlrpc_c::paramList paramListTmp;            
            xmlrpc_c::value result2;
            
            /*首先插入认证字段*/
            paramListTmp.add(xmlrpc_c::value_string(pLinkState->sSession));
            paramListTmp.add(xmlrpc_c::value_i8(0));
            paramListTmp.add(xmlrpc_c::value_i8(XMLRPCAPP_QUERY_COUNT));
            paramListTmp.add(xmlrpc_c::value_string(string(pLinkState->sUserName)));
            
            AuthClient.call(serverUrl, g_atMethodConfig[iMethosIdx].aucMethodName, paramListTmp, &result2);
            xmlrpc_c::value_array const arrayValue2(result2);
            vector<xmlrpc_c::value> rs2 = arrayValue2.vectorValueValue();
            int iTmp2 = xmlrpc_c::value_int(rs2[0]);
            
            XOS_SysLog(OAM_CFG_LOG_ERROR, "CliLoginAuth 2: call show user return:%u!\n", iTmp2);

            if (iTmp2 == 0)            
                return true;
            else
                return false;
        }
        else
        {
            return false;
        }
    } 
    catch (exception const& e) 
    {
        CHAR aucErr[1024] = {0};
        snprintf(aucErr, sizeof(aucErr), "Error:%s",e.what());
        cerr << "Client threw error: " << aucErr << endl;

	 return false;
    } 
    catch (...) 
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "CliLoginAuth: call show user throws exception!\n");
        return false;
    }	
}
 
/*
添加公共入参
*/
static void AddCommonParam(TYPE_LINK_STATE *pLinkState, MSG_COMM_OAM *pCliMsg, xmlrpc_c::paramList &paramlist)
{
    /*插入认证字段*/
//    string strAuth = string(pLinkState->sUserName)+ ":" + string(pLinkState->sPasswordOrg);
 //   string strAuth = string(pLinkState->sUserName)+ ":" + string(pLinkState->sPassword);

    paramlist.add(xmlrpc_c::value_string(pLinkState->sSession));

    /*如果是查询命令，插入索引字段*/
    if (CheckGetOrSet(pCliMsg->CmdID) == IS_GET)
    {
        if (pCliMsg->CmdRestartFlag == TRUE)
        {
            memset(pCliMsg->LastData, 0, sizeof(pCliMsg->LastData));
        }
        
        T_QueryIndex *pQueryIndex = (T_QueryIndex *)pCliMsg->LastData;
        pQueryIndex->iQueryCount = XMLRPCAPP_QUERY_COUNT;
		
        paramlist.add(xmlrpc_c::value_i8(pQueryIndex->iStartIndex));
        paramlist.add(xmlrpc_c::value_i8(pQueryIndex->iQueryCount));
    }
}

/*
获取公共出参
*/
static void GetCommonOutParam(MSG_COMM_OAM *pCliRetMsg, xmlrpc_c::value result)
{
    /*1、设置消息返回码*/
    pCliRetMsg->Number = 0;
    pCliRetMsg->DataLen = 0;
    pCliRetMsg->ReturnCode = SUCC_AND_HAVEPARA;//应用根据执行结果自行赋值，成功或失败
	
    /*2、读取返回码，放在map 400 里面*/
    xmlrpc_c::value_array const arrayValue(result);
    vector<xmlrpc_c::value> rs = arrayValue.vectorValueValue();
    int iReturncode = xmlrpc_c::value_int(rs[0]);
    AddMapParaToMsg(pCliRetMsg, MAPTYPE_RETURNCODE, (WORD16)iReturncode);

    /*3、如果是查询命令并且返回成功，获取查询索引，用于多包请求*/
    if (CheckGetOrSet(pCliRetMsg->CmdID) == IS_GET)
    {
        if (iReturncode == 0)
        {
            T_QueryIndex *pQueryIndex = (T_QueryIndex *)pCliRetMsg->LastData;
            INT64 iQryIdx = xmlrpc_c::value_i8(rs[1]);
            pQueryIndex->iStartIndex = iQryIdx;
            pQueryIndex->iQueryCount = XMLRPCAPP_QUERY_COUNT;

            /*如果应用返回索引是 -1 ，表示还有数据没有返回*/ 
            if (iQryIdx >= 0)
            {
                pCliRetMsg->ReturnCode = SUCC_CMD_NOT_FINISHED;
            }
        }
    }
}

static T_CLI_MSG_BUF * GetCliMsgBuf()
{
    int i;
    T_CLI_MSG_BUF *pRet = NULL;
		
    XOS_SemaphoreP((WORD32)tCliMsgBufMutexSem, (WORD32)WAIT_FOREVER);
	
    for (i = 0; i < MAX_VTY_NUMBER; i++)
    {
        if (g_tCliMsgBufPool[i].bUsing == FALSE)
        {
            pRet = &g_tCliMsgBufPool[i];
            pRet->bUsing = TRUE;
            break;
        }
    }
	
    XOS_SemaphoreV(tCliMsgBufMutexSem);

    return pRet;
}

static void RetCliMsgBuf(T_CLI_MSG_BUF *pBuf)
{
    if (pBuf == NULL)
    {
        return;
    }

    XOS_SemaphoreP((WORD32)tCliMsgBufMutexSem, (WORD32)WAIT_FOREVER);
    memset(pBuf, 0, sizeof(T_CLI_MSG_BUF));
    XOS_SemaphoreV(tCliMsgBufMutexSem);
}

 
