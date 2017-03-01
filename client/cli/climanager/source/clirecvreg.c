/*********************************************************************
* ��Ȩ���� (C)2008, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ� CliRecvReg.c
* �ļ���ʶ��
* ����ժҪ�����ո�����ע��/ע����Ϣ�����ո�����Agentע����Ϣ��ͬ��
*                           ���ļ���������CLIManager
* ����˵���� ��
* ��ǰ�汾�� V1.0
* ��    �ߣ� �����ߡ���3Gƽ̨
* ������ڣ�
*
* �޸ļ�¼1��
*    �޸����ڣ�2008-7-25 08:45����
*    �� �� �ţ�V1.0
*    �� �� �ˣ�������
*    �޸����ݣ�����
**********************************************************************/

#include "includes.h"
#include "pub_oam_cfg_event.h"
#include "clireg.h"
#include "clireglib.h"
#include "clirecvreg.h"

static void  OamCliScanShm(void);
static BOOLEAN LompCliAppRegInfoIsExist(WORD32 dwNetId);
static void UpdateCliAppRegInfo(WORD32 dwNetId,JID *ptJid);

extern void RegDatToVmm(void);
extern void ReqAllCfgVerInfo(void);
extern void Oam_CfgStopAllLinkExcutingCmd(void);
/*extern VOID OamCfgRemoveAllDat(VOID);*/

/**********************************************************************
* �������ƣ�VOID  OamCliAgingShm
* ����������Manager��ʱɨ���OMP��ע�����ݣ��ѳ�ʱ��
*                          û��ͬ������Ϣ�ϻ���
* ���ʵı���
* �޸ĵı���
* ���������T_OAM_INTERPRET_VAR *ptVar��˽��������
* �����������
* �� �� ֵ��OAM_CLI_SUCCESS(0)���ɹ�
*           OAM_CLI_FAIL(1)��   ʧ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* 2008-7-28             ������
* ---------------------------------------------------------------------
* 2008-7-28             ������      ����
************************************************************************/
void  OamCliAgingShm(T_OAM_INTERPRET_VAR *ptVar)
{
    /* ��ҵ���Ǳ߽ӿ�ȷ�Ϻ������ϻ��� */
    return;
}



/**********************************************************************
* �������ƣ�VOID RecvAgtReg
* ��������������Agentע����Ϣ��ƽ̨/T8000Ӧ�ý���ע��
* ���ʵı���
* �޸ĵı���
* ���������T_AlmAgtVars *ptVar��˽��������
            LPVOID pMsgPara:     ��Ϣ����
* �����������
* �� �� ֵ����
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2008-7-24             ������      ����
************************************************************************/
void RecvAgtReg(T_OAM_INTERPRET_VAR *ptVar, LPVOID pMsgPara, BOOLEAN bIsSameEndian)
{
    WORD32          dwLoop = 0;
    WORD16          wRegDatCnt = 0;
    WORD32          *pdwRegNum = NULL;
    JID             tSenderJid;
    T_CliAppReg     *ptPriData = NULL;
    T_CliAppData *ptRecvAppData,tMsgRecvData;
//    DWORD dwVer = 0;
    WORD16 wLen = 0;
    WORD32          dwRegedCntOfShm = 0;
    
    /* �����ǰ������VMM����汾���򲻴���ע����Ϣ���Ⱥ���ɨ��ע���ʱ�ټ��� */
    if ((g_ptOamIntVar->ucVerInfoReqCnt > 0) || 
        (g_ptOamIntVar->ucVerNotifyRegReqCnt > 0) ||
        ((g_ptOamIntVar->ucVerInfoReqCnt == 0) && (g_ptOamIntVar->wGetCfgVerInfoTimer != INVALID_TIMER_ID)) ||
        ((g_ptOamIntVar->ucVerNotifyRegReqCnt == 0) && (g_ptOamIntVar->wRegOamsNotifyTimer != INVALID_TIMER_ID)))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "######[RecvAgtReg]: now is requesting dat ver...\n");
        return;
    }

    /******************** ��Ϣ���� ��ʼ ********************/
    /* ��Ϣ�ṹĬ�ϳ�ʼ�� */
    memset(&tMsgRecvData, 0xFF, sizeof(tMsgRecvData));

    /* ������ջ��������Ϣ���ݽṹ�����⶯̬�ڴ�����/�ͷ� */
    ptRecvAppData  = &tMsgRecvData;

    /* ������Ϣ�汾����ת������ */
   // XOS_GetMsgAppVer((BYTE *)&dwVer);
    XOS_GetMsgDataLen(PROCTYPE_OAM_CLIMANAGER, (WORD16 *)&wLen);
 //   INVERT_MSG_T_CliAppData((BYTE **)&ptRecvAppData,pMsgPara,dwVer,wLen);
    /******************** ��Ϣ���� ���� ********************/
    
    /*�ֽ���ת��*/
    if (!bIsSameEndian)
    {
        ptRecvAppData->tAppJid.dwJno = XOS_InvertWord32(ptRecvAppData->tAppJid.dwJno);
        ptRecvAppData->tAppJid.dwDevId = XOS_InvertWord32(ptRecvAppData->tAppJid.dwDevId);
        ptRecvAppData->tAppJid.wModule = XOS_InvertWord16(ptRecvAppData->tAppJid.wModule);
        ptRecvAppData->tAppJid.wUnit = XOS_InvertWord16(ptRecvAppData->tAppJid.wUnit);
        ptRecvAppData->dwDatFuncType = XOS_InvertWord32(ptRecvAppData->dwDatFuncType);
    }

    /* ע���������ṹ���ʹ�� */
    memset(&tSenderJid, 0, sizeof(JID));
    XOS_Sender(&tSenderJid);

    /* OMP�ϵ�Ӧ�ý���ע����Ϣ*/
    if (IsSameLogicAddr(&(g_ptOamIntVar->jidSelf), &(ptRecvAppData->tAppJid)))
    {
        /* OMP,ֻ��Ҫɨ��ע���Ϳ�����*/
        OamCliScanShm();
        return;
    }
    /* Ŀǰ��֪Ӧ��ֻ��RPU����ҵ��ע����̣���Ĭ��RPU�� */
    else
    {
        pdwRegNum  = &ptVar->tPlatMpCliAppInfo.dwRegCount;
        ptPriData = &ptVar->tPlatMpCliAppInfo;
    }

    if (!OamRegCntOfShm(&dwRegedCntOfShm))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, 
            "RecvAgtReg: Exec func OamRegCntOfShm failed!\r");
        return;
    }

    /* ���浽������ */
    for(dwLoop=0; dwLoop<*pdwRegNum; dwLoop++)
    {
        if(dwLoop>=OAM_CLI_DAT_SHM_MAX_NUM)
            return;

        /* ֱ����Ϊ�Ѿ�ע����� */
        if(ptRecvAppData->dwDatFuncType == ptPriData->tCliReg[dwLoop].dwDatFuncType)
        {
            /* added by fls on 2009-02-27 (CRDCR00481329 rpu����֮��,���������������ý��벻��) */
            for (wRegDatCnt = 0; wRegDatCnt < g_wRegDatCount; wRegDatCnt++)
            {
                if (g_tDatLink[wRegDatCnt].datIndex.tJid.dwJno == ptRecvAppData->tAppJid.dwJno)
                {
                    memcpy(&g_tDatLink[wRegDatCnt].datIndex.tJid, &ptRecvAppData->tAppJid, sizeof(JID));
                    if (ptRecvAppData->dwDatFuncType == FUNC_SCRIPT_CLIS_BRS)
                        memcpy(&g_ptOamIntVar->jidProtocol, &ptRecvAppData->tAppJid, sizeof(JID));
                        
                    break;
                }
            }
            /* end of add */
            return;
        }
    }/*yinhao test���� ��forѭ���߽��Ƶ���������һ���޷�ע��*/
	
    /* ���ִ����� */
    if(dwLoop != *pdwRegNum)
        return;

    /* �����ڴ��к�ȫ���������е���ע��dat�����ݲ��ܳ������ֵ */
    if ((dwRegedCntOfShm + (*pdwRegNum)) >= OAM_CLI_DAT_SHM_MAX_NUM)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, 
            "RecvAgtReg: Total of registered dats has reached max!\r");
        return;
    }

    /* ��Ҫ�����������ϵ�JID */
    if(OAM_CLI_SUCCESS != OAM_CFGDatRegister(ptRecvAppData->tAppJid, ptRecvAppData->dwDatFuncType))
        return;

    /* �ŵ����һ���ڵ� */
    memcpy(&ptPriData->tCliReg[dwLoop].tAppJid, &ptRecvAppData->tAppJid, sizeof(JID));
    ptPriData->tCliReg[dwLoop].dwDatFuncType = ptRecvAppData->dwDatFuncType;
    ptPriData->tCliReg[dwLoop].ucReadFlag = OAM_CLI_REG_READED;
    (*pdwRegNum)++;

    return;
}

void RecvAgtRegSpecial(T_OAM_INTERPRET_VAR *ptVar, LPVOID pMsgPara, BOOLEAN bIsSameEndian)
{
    T_CliAppData *ptRecvAppData,tMsgRecvData;
//    DWORD dwVer = 0;
    WORD16 wLen = 0;
    WORD32       *pdwRegNum = NULL;
    T_CliAppReg  *ptPriData = NULL;
    WORD32       dwLoop = 0;
    WORD16       wRegDatCnt = 0;
    WORD32          dwRegedCntOfShm = 0;
    
    XOS_SysLog(OAM_CFG_LOG_NOTICE, "[RecvAgtRegSpecial] Recv msg: EV_OAMCLI_SPECIAL_REG......");

    /******************** ��Ϣ���� ��ʼ ********************/
    /* ��Ϣ�ṹĬ�ϳ�ʼ�� */
    memset(&tMsgRecvData, 0xFF, sizeof(tMsgRecvData));

    /* ������ջ��������Ϣ���ݽṹ�����⶯̬�ڴ�����/�ͷ� */
    ptRecvAppData  = &tMsgRecvData;

    /* ������Ϣ�汾����ת������ */
  //  XOS_GetMsgAppVer((BYTE *)&dwVer);
    XOS_GetMsgDataLen(PROCTYPE_OAM_CLIMANAGER, (WORD16 *)&wLen);
    //INVERT_MSG_T_CliAppData((BYTE **)&ptRecvAppData,pMsgPara,dwVer,wLen);
    /******************** ��Ϣ���� ���� ********************/

    /*�ֽ���ת��*/
    if (!bIsSameEndian)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "[RecvAgtRegSpecial] Need to invert......");
        ptRecvAppData->tAppJid.dwJno = XOS_InvertWord32(ptRecvAppData->tAppJid.dwJno);
        ptRecvAppData->tAppJid.dwDevId = XOS_InvertWord32(ptRecvAppData->tAppJid.dwDevId);
        ptRecvAppData->tAppJid.wModule = XOS_InvertWord16(ptRecvAppData->tAppJid.wModule);
        ptRecvAppData->tAppJid.wUnit = XOS_InvertWord16(ptRecvAppData->tAppJid.wUnit);
        ptRecvAppData->dwDatFuncType = XOS_InvertWord32(ptRecvAppData->dwDatFuncType);
    }

    /* ����Ƿ�OMPע�ᣬ����Ҫ���뵽ȫ��������������OMPע�ᣬ���Ѿ����뵽�����ڴ��� */
    if (!IsSameLogicAddr(&(g_ptOamIntVar->jidSelf), &(ptRecvAppData->tAppJid)))
    {
        pdwRegNum  = &ptVar->tPlatMpCliAppInfo.dwRegCount;
        ptPriData = &ptVar->tPlatMpCliAppInfo;

        if (!OamRegCntOfShm(&dwRegedCntOfShm))
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR, 
                "RecvAgtRegSpecial: Exec func OamRegCntOfShm failed!\r");
            return;
        }

        /* ���浽������ */
        for(dwLoop=0; dwLoop<*pdwRegNum; dwLoop++)
        {
            if(dwLoop>=OAM_CLI_DAT_SHM_MAX_NUM)
                return;

            /* ֱ����Ϊ�Ѿ�ע����� */
            if(ptRecvAppData->dwDatFuncType == ptPriData->tCliReg[dwLoop].dwDatFuncType)
            {
                for (wRegDatCnt = 0; wRegDatCnt < g_wRegDatCount; wRegDatCnt++)
                {
                    if (g_tDatLink[wRegDatCnt].datIndex.tJid.dwJno == ptRecvAppData->tAppJid.dwJno)
                    {
                        memcpy(&g_tDatLink[wRegDatCnt].datIndex.tJid, &ptRecvAppData->tAppJid, sizeof(JID));
                        break;
                    }
                }
                return;
            }
        }

        /* ���ִ����� */
        if(dwLoop != *pdwRegNum)
            return;

        /* �����ڴ��к�ȫ���������е���ע��dat�����ݲ��ܳ������ֵ */
        if ((dwRegedCntOfShm + (*pdwRegNum)) >= OAM_CLI_DAT_SHM_MAX_NUM)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR, 
                "RecvAgtRegSpecial: Total of registered dats has reached max!\r");
            return;
        }

        /* �ŵ����һ���ڵ� */
        memcpy(&ptPriData->tCliReg[dwLoop].tAppJid, &ptRecvAppData->tAppJid, sizeof(JID));
        ptPriData->tCliReg[dwLoop].dwDatFuncType = ptRecvAppData->dwDatFuncType;
        ptPriData->tCliReg[dwLoop].ucReadFlag = OAM_CLI_REG_NOT_READED;
        ptPriData->tCliReg[dwLoop].ucRegType = OAM_CLI_REG_TYPE_SPECIAL;
        (*pdwRegNum)++;
        
    }
#if 0
    /* ֹͣ��������ִ�е�����Ͽ��������� */
    Oam_CfgStopAllLinkExcutingCmd();

    /* �������ע���dat�ڴ� */
    OAM_CFGRemoveAllDat();
    /*����ǵ�Ƭ����������汾 ��Ϣ����vmm���»�ȡ*/
    if (!Oam_CfgIfCurVersionIsPC())
    {
        g_ptOamIntVar->wDatSum = 0;
    }
    
    RegDatToVmm();
    ReqAllCfgVerInfo();
#endif    
    return;
}

/**********************************************************************
* �������ƣ�VOID  OamCliScanShm
* ����������Manager��ʱɨ��OMP�Ĺ����ڴ棬�ж��Ƿ���DAT
*                          ��FUNTYPE��Ϊ0����flagΪ0ʱ�����°�JID����������
* ���ʵı���
* �޸ĵı���
* ���������T_OAM_INTERPRET_VAR *ptVar��˽��������
* �����������
* �� �� ֵ��OAM_CLI_SUCCESS(0)���ɹ�
*           OAM_CLI_FAIL(1)��   ʧ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* 2008-7-28             ������
* ---------------------------------------------------------------------
* 2008-7-28             ������      ����
************************************************************************/
void  OamCliScanShm()
{
#if 0
    WORD16        wLoop = 0;
    T_CliAppReg   *ptOamCliReg = NULL;

    XOS_SysLog(OAM_CFG_LOG_NOTICE, "######[OamCliScanShm]: begin to scan register table in shm...");

    /*������ʹ�ú����Oam_UnLockRegTable�ͷ���*/
    ptOamCliReg = Oam_GetRegTableAndLock();
    if (!ptOamCliReg)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "######[OamCliScanShm]: failed to create shm which contains rigister table and set lock on it!\r");
        return;
    }

    for(wLoop=0; wLoop<OAM_CLI_DAT_SHM_MAX_NUM; wLoop++)
    {
        if(0 == ptOamCliReg->tCliReg[wLoop].dwDatFuncType)
            break;

        /* ��ȡ��ʶΪ0����Ϊ����ڵ�û��ע��� */
        if(OAM_CLI_REG_NOT_READED == ptOamCliReg->tCliReg[wLoop].ucReadFlag)
        {
            if(OAM_CLI_SUCCESS != OAM_CFGDatRegister(ptOamCliReg->tCliReg[wLoop].tAppJid, ptOamCliReg->tCliReg[wLoop].dwDatFuncType))
            	{
            	    continue;
            	}

            ptOamCliReg->tCliReg[wLoop].ucReadFlag = OAM_CLI_REG_READED;
        }
    }

    /* �����ж��������֮���ٿ�����û��SBCdatû�м��سɹ������������Ҫ��VMM���� */
    for(wLoop = 0; wLoop < OAM_CLI_DAT_SHM_MAX_NUM; wLoop++)
    {
        if(0 == ptOamCliReg->tCliReg[wLoop].dwDatFuncType)
            break;
        
        if((OAM_CLI_REG_NOT_READED == ptOamCliReg->tCliReg[wLoop].ucReadFlag) &&
            (OAM_CLI_REG_TYPE_SPECIAL == ptOamCliReg->tCliReg[wLoop].ucRegType))
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR, "######[OamCliScanShm]: Has special dat need to request to VMM...\r");
            
            /* ֹͣ��������ִ�е�����Ͽ��������� */
            Oam_CfgStopAllLinkExcutingCmd();

            /* �������ע���dat�ڴ� */
            OAM_CFGRemoveAllDat();
            /*����ǵ�Ƭ����������汾 ��Ϣ����vmm���»�ȡ*/
            if (!Oam_CfgIfCurVersionIsPC())
            {
                g_ptOamIntVar->wDatSum = 0;
            }
            
            RegDatToVmm();
            ReqAllCfgVerInfo();
            break;
        }
    }

    if (!Oam_UnLockRegTable(ptOamCliReg))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "######[OamCliScanShm]: failed to unlock shm which contains register table ");
    }
#endif	
    return;
}

/**********************************************************************
* �������ƣ�VOID  Oam_CfgScanRegTable
* �������������ע������Ƿ���û��ע��ģ�
                                 ����У�����ע��
* ���ʵı���
* �޸ĵı���
* �����������
* �����������
 * �� �� ֵ�� ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2009-3-5           ���      ����
************************************************************************/
void Oam_CfgScanRegTable()
{
    T_CliAppReg *ptOtherMpRegInfo = NULL;  
    WORD16 wLoop = 0;

    if (NULL == g_ptOamIntVar)
    {
        return;
    }

     /*��鹲���ڴ��б�ģ���ע���*/
    OamCliScanShm();

    /*���������������ģ���ע���*/
    ptOtherMpRegInfo = &(g_ptOamIntVar->tPlatMpCliAppInfo);
    for(wLoop=0; wLoop < ptOtherMpRegInfo->dwRegCount; wLoop++)
    {
        if(wLoop>=OAM_CLI_DAT_SHM_MAX_NUM)
        {
            break;
        }

        if (OAM_CLI_REG_NOT_READED == ptOtherMpRegInfo->tCliReg[wLoop].ucReadFlag)
        {
            if(OAM_CLI_SUCCESS != OAM_CFGDatRegister(ptOtherMpRegInfo->tCliReg[wLoop].tAppJid, ptOtherMpRegInfo->tCliReg[wLoop].dwDatFuncType))
            {
                XOS_SysLog(OAM_CFG_LOG_ERROR, "[Oam_CfgScanRegTable] Job register fail, jid = %d, functype = %d", ptOtherMpRegInfo->tCliReg[wLoop].tAppJid, ptOtherMpRegInfo->tCliReg[wLoop].dwDatFuncType);
                continue;
            }
        
            ptOtherMpRegInfo->tCliReg[wLoop].ucReadFlag = OAM_CLI_REG_READED;
        }
    }

    /* �����ж��������֮���ٿ�����û��SBCdatû�м��سɹ������������Ҫ��VMM���� */
    for(wLoop = 0; wLoop < ptOtherMpRegInfo->dwRegCount; wLoop++)
    {
        if(wLoop >= OAM_CLI_DAT_SHM_MAX_NUM)
        {
            break;
        }

        if ((OAM_CLI_REG_NOT_READED == ptOtherMpRegInfo->tCliReg[wLoop].ucReadFlag) &&
            (OAM_CLI_REG_TYPE_SPECIAL == ptOtherMpRegInfo->tCliReg[wLoop].ucRegType))
        {
            /* ֹͣ��������ִ�е�����Ͽ��������� */
            Oam_CfgStopAllLinkExcutingCmd();

            /* �������ע���dat�ڴ� */
            OAM_CFGRemoveAllDat();
            /*����ǵ�Ƭ����������汾 ��Ϣ����vmm���»�ȡ*/
            if (!Oam_CfgIfCurVersionIsPC())
            {
                g_ptOamIntVar->wDatSum = 0;
            }
            
            RegDatToVmm();
            ReqAllCfgVerInfo();
            break;
        }
            
    }
}

/**********************************************************************
* �������ƣ�VOID RecvAgtUnreg
* ��������������Agentע����Ϣ��ƽ̨/T8000Ӧ�ý���ע��
* ���ʵı���
* �޸ĵı���
* ���������T_AlmAgtVars *ptVar��˽��������
            LPVOID pMsgPara:     ��Ϣ����
* �����������
* �� �� ֵ����
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2008-7-24             ������      ����
************************************************************************/
void RecvAgtUnreg(T_OAM_INTERPRET_VAR *ptVar, LPVOID pMsgPara, BOOLEAN bIsSameEndian)
{
    WORD32          dwLoop = 0;
    WORD32          *pdwRegNum = NULL;
    /*T_CliAppData    *ptRecvAppData = NULL;*/
    JID             tSenderJid;
    T_CliAppReg     *ptPriData = NULL;

    T_CliAppData *ptRecvAppData,tMsgRecvData;
//    DWORD dwVer = 0;
    WORD16 wLen = 0;
    
    /******************** ��Ϣ���� ��ʼ ********************/
    /* ��Ϣ�ṹĬ�ϳ�ʼ�� */
    memset(&tMsgRecvData, 0xFF, sizeof(tMsgRecvData));

    /* ������ջ��������Ϣ���ݽṹ�����⶯̬�ڴ�����/�ͷ� */
    ptRecvAppData  = &tMsgRecvData;

    /* ������Ϣ�汾����ת������ */
  //  XOS_GetMsgAppVer((BYTE *)&dwVer);
    XOS_GetMsgDataLen(PROCTYPE_OAM_CLIMANAGER, (WORD16 *)&wLen);
 //   INVERT_MSG_T_CliAppData((BYTE **)&ptRecvAppData,pMsgPara,dwVer,wLen);
    /******************** ��Ϣ���� ���� ********************/
#if 0
    if(XOS_SUCCESS != XOS_GetCurMsgData((BYTE**)&ptRecvAppData))
        return;
#endif
    /*�ֽ���ת��*/
    if (!bIsSameEndian)
    {
        ptRecvAppData->tAppJid.dwJno = XOS_InvertWord32(ptRecvAppData->tAppJid.dwJno);
        ptRecvAppData->tAppJid.dwDevId = XOS_InvertWord32(ptRecvAppData->tAppJid.dwDevId);
        ptRecvAppData->tAppJid.wModule = XOS_InvertWord16(ptRecvAppData->tAppJid.wModule);
        ptRecvAppData->tAppJid.wUnit = XOS_InvertWord16(ptRecvAppData->tAppJid.wUnit);
        ptRecvAppData->dwDatFuncType = XOS_InvertWord32(ptRecvAppData->dwDatFuncType);
    }
    
    /* ע���������ṹ���ʹ�� */
    memset(&tSenderJid, 0, sizeof(JID));
    /* OMP�ϵ�Ӧ�ý���ע����Ϣ����������Agentͬ�� */
    if (IsSameLogicAddr(&(g_ptOamIntVar->jidSelf), &(ptRecvAppData->tAppJid)))
    {
        if (OAM_CLI_SUCCESS == OAM_CFGDatUnregister(ptRecvAppData->tAppJid))
        {
            XOS_SysLog(OAM_CFG_LOG_NOTICE, "RecvAgtUnreg success, dat functype = %d\n", ptRecvAppData->dwDatFuncType);
        }
        else
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR, "RecvAgtUnreg fail, dat functype = %d\n", ptRecvAppData->dwDatFuncType);
        }
        return;
    }
    /* Ŀǰ��֪Ӧ��ֻ��RPU����ҵ��ע����̣���Ĭ��RPU�� */
    else
    {
        pdwRegNum  = &ptVar->tPlatMpCliAppInfo.dwRegCount;
        ptPriData = &ptVar->tPlatMpCliAppInfo;
    }

    /* ��Ҫɾ������������ */
    for(dwLoop=0; dwLoop<*pdwRegNum; dwLoop++)
    {
        if(dwLoop>=OAM_CLI_DAT_SHM_MAX_NUM)
            return;

        /* ֱ����Ϊ�Ѿ�ע����� */
        if(ptRecvAppData->dwDatFuncType == ptPriData->tCliReg[dwLoop].dwDatFuncType)
        {
            /* ��Ҫ�����������ϵ�JID */
            if (OAM_CLI_SUCCESS == OAM_CFGDatUnregister(ptRecvAppData->tAppJid))
            {
                XOS_SysLog(OAM_CFG_LOG_NOTICE,"RecvAgtUnreg success, dat functype = %d\n", ptRecvAppData->dwDatFuncType);
            }
            else
            {
                XOS_SysLog(OAM_CFG_LOG_ERROR, "RecvAgtUnreg fail, dat functype = %d\n", ptRecvAppData->dwDatFuncType);
            }
			
            memset(&ptPriData->tCliReg[dwLoop], 0, sizeof(T_CliAppData));
            (*pdwRegNum)--;

            return;
        }
    }

    return;
}

/**********************************************************************
* �������ƣ�VOID RecvAgtRegEx
* ��������������Agentע����Ϣ����ƽ̨ҵ�����ע��
* ���ʵı���
* �޸ĵı���
* ���������T_AlmAgtVars *ptVar��˽��������
            LPVOID pMsgPara:     ��Ϣ����
* �����������
* �� �� ֵ����
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2008-7-24             ������      ����
************************************************************************/
void RecvAgtRegEx(T_OAM_INTERPRET_VAR *ptVar, LPVOID pMsgPara, BOOLEAN bIsSameEndian)
{
    WORD32          dwIndex = 0;
    /*T_CliAppDataEx  *ptRecvAppData = NULL;*/

    T_CliAppDataEx *ptRecvAppData,tMsgRecvData;
//    DWORD dwVer = 0;
    WORD16 wLen = 0;
    
    /******************** ��Ϣ���� ��ʼ ********************/
    /* ��Ϣ�ṹĬ�ϳ�ʼ�� */
    memset(&tMsgRecvData, 0xFF, sizeof(tMsgRecvData));

    /* ������ջ��������Ϣ���ݽṹ�����⶯̬�ڴ�����/�ͷ� */
    ptRecvAppData  = &tMsgRecvData;

    /* ������Ϣ�汾����ת������ */
  //  XOS_GetMsgAppVer((BYTE *)&dwVer);
    XOS_GetMsgDataLen(PROCTYPE_OAM_CLIMANAGER, (WORD16 *)&wLen);
  //  INVERT_MSG_T_CliAppDataEx((BYTE **)&ptRecvAppData,pMsgPara,dwVer,wLen);
    /******************** ��Ϣ���� ���� ********************/
#if 0
    if(XOS_SUCCESS != XOS_GetCurMsgData((BYTE**)&ptRecvAppData))
        return;
#endif
    /*�ֽ���ת��*/
    if (!bIsSameEndian)
    {
        ptRecvAppData->tAppJid.dwJno = XOS_InvertWord32(ptRecvAppData->tAppJid.dwJno);
        ptRecvAppData->tAppJid.dwDevId = XOS_InvertWord32(ptRecvAppData->tAppJid.dwDevId);
        ptRecvAppData->tAppJid.wModule = XOS_InvertWord16(ptRecvAppData->tAppJid.wModule);
        ptRecvAppData->tAppJid.wUnit = XOS_InvertWord16(ptRecvAppData->tAppJid.wUnit);
        ptRecvAppData->dwNetId = XOS_InvertWord32(ptRecvAppData->dwNetId);
    }
    
    /* OMP�ϵ�Ӧ�ý���ע����Ϣ*/
    if (IsSameLogicAddr(&(g_ptOamIntVar->jidSelf), &(ptRecvAppData->tAppJid)))
    {
        /* OMP,ʲôҲ����Ҫ��*/
        return;
    }

    /*�����Ԫid�Ѿ���ע���������*/
    if (LompCliAppRegInfoIsExist(ptRecvAppData->dwNetId))
    {
    	  UpdateCliAppRegInfo(ptRecvAppData->dwNetId,&(ptRecvAppData->tAppJid));
        return;
    }
    
    /* ���浽������ */
    /* �ŵ����һ���ڵ� */
    dwIndex = ptVar->tLompCliAppRegInfo.dwRegCount;
    if (dwIndex >= OAM_CLI_APP_SHM_NUM)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "RecvAgtRegEx: register failed, ptVar->tLompCliAppRegInfo.dwRegCount too big(%d)\n", dwIndex);
    }
    else
    {
        memcpy(&(ptVar->tLompCliAppRegInfo.tCliReg[dwIndex].tAppJid), &(ptRecvAppData->tAppJid), sizeof(JID));
        ptVar->tLompCliAppRegInfo.tCliReg[dwIndex].dwNetId = ptRecvAppData->dwNetId;
        ptVar->tLompCliAppRegInfo.dwRegCount++;
    }

    return;
}


/**********************************************************************
* �������ƣ�VOID Oam_CfgRecvRegA2M
* ��������������Agent��Manager��ͬ����Ϣ��ƽ̨/T8000ҵ�����ע�����Ϣ
*                           Ŀǰֻ��RPU(RPU��OMP����һʱ)����OMPͬ��
* ���ʵı���
* �޸ĵı���
* ���������T_AlmAgtVars *ptVar��˽��������
            LPVOID pMsgPara:     ��Ϣ����
* �����������
* �� �� ֵ����
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2008-7-24             ������      ����
************************************************************************/
void Oam_CfgRecvRegA2M(T_OAM_INTERPRET_VAR *ptVar, LPVOID pMsgPara, BOOLEAN bIsSameEndian)
{
   /*����pcLint�澯����sword32�޸�Ϊword32 ,SWORD32        dwLoop = 0;*/
    WORD32         dwRet = 0;
    WORD32        dwLoop = 0;
    WORD16         wRegDatCnt = 0;
    WORD32         dwRegLoop = 0;
    JID tSenderJid;
    T_CliAppReg *ptRecvAppData,tMsgRecvData;
  //  DWORD dwVer = 0;
    WORD16 wLen = 0;
    WORD32       *pdwRegNum = NULL;
    T_CliAppReg  *ptPriData = NULL;
    BOOLEAN bHasSpecial = FALSE;
    WORD32         dwRegedCntOfShm = 0;
    
    /******************** ��Ϣ���� ��ʼ ********************/
    /* ��Ϣ�ṹĬ�ϳ�ʼ�� */
    memset(&tMsgRecvData, 0xFF, sizeof(tMsgRecvData));

    /* ������ջ��������Ϣ���ݽṹ�����⶯̬�ڴ�����/�ͷ� */
    ptRecvAppData  = &tMsgRecvData;

    /* ������Ϣ�汾����ת������ */
  //  XOS_GetMsgAppVer((BYTE *)&dwVer);
    XOS_GetMsgDataLen(PROCTYPE_OAM_CLIMANAGER, (WORD16 *)&wLen);
  //  INVERT_MSG_T_CliAppReg((BYTE **)&ptRecvAppData,pMsgPara,dwVer,wLen);
    /******************** ��Ϣ���� ���� ********************/

    /*�ֽ���ת��*/
    if (!bIsSameEndian)
    {
        ptRecvAppData->dwSemid = XOS_InvertWord32(ptRecvAppData->dwSemid);
        ptRecvAppData->dwRegCount = XOS_InvertWord32(ptRecvAppData->dwRegCount);
        for(dwRegLoop=0; dwRegLoop<ptRecvAppData->dwRegCount; dwRegLoop++)
        {
            ptRecvAppData->tCliReg[dwRegLoop].tAppJid.dwJno = XOS_InvertWord32(ptRecvAppData->tCliReg[dwRegLoop].tAppJid.dwJno);
            ptRecvAppData->tCliReg[dwRegLoop].tAppJid.dwDevId = XOS_InvertWord32(ptRecvAppData->tCliReg[dwRegLoop].tAppJid.dwDevId);
            ptRecvAppData->tCliReg[dwRegLoop].tAppJid.wModule = XOS_InvertWord16(ptRecvAppData->tCliReg[dwRegLoop].tAppJid.wModule);
            ptRecvAppData->tCliReg[dwRegLoop].tAppJid.wUnit = XOS_InvertWord16(ptRecvAppData->tCliReg[dwRegLoop].tAppJid.wUnit);
            ptRecvAppData->tCliReg[dwRegLoop].dwDatFuncType = XOS_InvertWord32(ptRecvAppData->tCliReg[dwRegLoop].dwDatFuncType);
        }
    }
    
    /*��ȡ�����ߵ�jid*/
    memset(&tSenderJid, 0, sizeof(tSenderJid));
    dwRet = XOS_Sender(&tSenderJid);
    /*��ȡ���Ͷ�JIDʧ��*/
    if (XOS_SUCCESS != dwRet)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d XOS_Sender Error,%d! ....................",__func__,__LINE__,dwRet);
        return;
    }
    if (NULL == g_ptOamIntVar)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return;
    }
    /* �����OMP�ϵ�ͬ����Ϣ������ */
    if (IsSameLogicAddr(&(g_ptOamIntVar->jidSelf), &tSenderJid))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "Oam_CfgRecvRegA2M: receive register table from omp cliagent, ignored");
        return;
    }

    pdwRegNum  = &ptVar->tPlatMpCliAppInfo.dwRegCount;

    /*++++++++++++++++++++++++++++++++++++�Ƚ���Ч��ע�����ڳ��ռ�++++++++++++++++++++++++++++++++++++++++++++++++*/
    /* ��ͬ����Ϣ���ͷ���ͬģ��ŵıȽϱ����� */
    for(dwLoop = 0; dwLoop < *pdwRegNum; dwLoop++)
    {
        if(dwLoop >= OAM_CLI_DAT_SHM_MAX_NUM)
            break;

        if (IsSameLogicAddr(&(ptVar->tPlatMpCliAppInfo.tCliReg[dwLoop].tAppJid), &tSenderJid))
        {
            ptVar->tPlatMpCliAppInfo.tCliReg[dwLoop].ucCmpFlag = 0;
        }
    }

    /* ��ͬ����������ͬ�������͵Ĵ��ϱ�� */
    for(dwRegLoop = 0; dwRegLoop < ptRecvAppData->dwRegCount; dwRegLoop++)
    {
        if(dwRegLoop >= OAM_CLI_DAT_SHM_MAX_NUM)
            break;

        for(dwLoop = 0; dwLoop < *pdwRegNum; dwLoop++)
        {
            if(dwLoop >= OAM_CLI_DAT_SHM_MAX_NUM)
                break;

            if (ptRecvAppData->tCliReg[dwRegLoop].dwDatFuncType == ptVar->tPlatMpCliAppInfo.tCliReg[dwLoop].dwDatFuncType)
                ptVar->tPlatMpCliAppInfo.tCliReg[dwLoop].ucCmpFlag = 1;
        }
        
    }

    /* ע����Ч��ע�� */
    for(dwLoop = (*pdwRegNum) - 1; dwLoop >= 0; dwLoop--)
    {
        if (dwLoop >= OAM_CLI_DAT_SHM_MAX_NUM)
        {
            break;
        }
        
        /* ��ʾû�д��ϱ�ǩ��˵����ע����ϢӦ����Ч�� */
        if(0 == ptVar->tPlatMpCliAppInfo.tCliReg[dwLoop].ucCmpFlag)
        {
            /*ע����ʧЧ��job*/
            if (OAM_CLI_SUCCESS == OAM_CFGDatUnregister(ptVar->tPlatMpCliAppInfo.tCliReg[dwLoop].tAppJid))
            {
                XOS_SysLog(OAM_CFG_LOG_ERROR, "Oam_CfgRecvRegA2M: unreginster success, dat functype = %d\n", ptVar->tPlatMpCliAppInfo.tCliReg[dwLoop].dwDatFuncType);
            }
            else
            {
                XOS_SysLog(OAM_CFG_LOG_ERROR, "Oam_CfgRecvRegA2M: unreginster failed, dat functype = %d\n", ptVar->tPlatMpCliAppInfo.tCliReg[dwLoop].dwDatFuncType);
                continue;
            }
        
            /* �����һ���ڵ�Ų���˽ڵ� */
            if(dwLoop != ptVar->tPlatMpCliAppInfo.dwRegCount-1)
            {
                memcpy(&ptVar->tPlatMpCliAppInfo.tCliReg[dwLoop].tAppJid,
                       &ptVar->tPlatMpCliAppInfo.tCliReg[ptVar->tPlatMpCliAppInfo.dwRegCount-1].tAppJid, sizeof(JID));
                ptVar->tPlatMpCliAppInfo.tCliReg[dwLoop].dwDatFuncType =
                    ptVar->tPlatMpCliAppInfo.tCliReg[ptVar->tPlatMpCliAppInfo.dwRegCount-1].dwDatFuncType;
            }
            /* ������һ���ڵ������ */
            memset(&ptVar->tPlatMpCliAppInfo.tCliReg[ptVar->tPlatMpCliAppInfo.dwRegCount-1], 0, sizeof(T_CliAppData));
            ptVar->tPlatMpCliAppInfo.dwRegCount--;
        }
    }
    
    /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

    pdwRegNum  = &ptVar->tPlatMpCliAppInfo.dwRegCount;
    /* �����ڴ��к�ȫ���������е���ע��dat�����ݲ��ܳ������ֵ */
    if (!OamRegCntOfShm(&dwRegedCntOfShm))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, 
            "Oam_CfgRecvRegA2M: Exec func OamRegCntOfShm failed!\r");
        return;
    }

    /* ���ͬ����������SBC��ҵ��dat��Ϣ����������û��ע����ģ���Ҫ��VMM����汾 */
    for(dwRegLoop = 0; dwRegLoop < ptRecvAppData->dwRegCount; dwRegLoop++)
    {
        if(dwRegLoop >= OAM_CLI_DAT_SHM_MAX_NUM)
            break;

        if (ptRecvAppData->tCliReg[dwRegLoop].ucRegType == OAM_CLI_REG_TYPE_SPECIAL)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR, "Oam_CfgRecvRegA2M: Has special dat in register table...");
            
            pdwRegNum  = &ptVar->tPlatMpCliAppInfo.dwRegCount;
            ptPriData = &ptVar->tPlatMpCliAppInfo;

            for(dwLoop = 0; dwLoop < *pdwRegNum; dwLoop++)
            {
                if(dwLoop > OAM_CLI_DAT_SHM_MAX_NUM)
                    break;
                
                if(ptRecvAppData->tCliReg[dwRegLoop].dwDatFuncType == ptPriData->tCliReg[dwLoop].dwDatFuncType)
                {
                    XOS_SysLog(OAM_CFG_LOG_ERROR, "Oam_CfgRecvRegA2M: The special dat in register table is exists...");
                    break;
                }
            }

            /* ����������û���ҵ�����Ҫ��ӽ��� */
            if(dwLoop == *pdwRegNum)
            {
                /* �����ڴ��к�ȫ���������е���ע��dat�����ݲ��ܳ������ֵ */
                if ((dwRegedCntOfShm + (*pdwRegNum)) >= OAM_CLI_DAT_SHM_MAX_NUM)
                {
                    XOS_SysLog(OAM_CFG_LOG_ERROR, 
                        "Oam_CfgRecvRegA2M: Total of registered dats has reached max!\r");
                    break;
                }

                XOS_SysLog(OAM_CFG_LOG_ERROR, "Oam_CfgRecvRegA2M: Add special dat to gloable data...");
                memcpy(&ptPriData->tCliReg[dwLoop].tAppJid, &(ptRecvAppData->tCliReg[dwRegLoop].tAppJid), sizeof(JID));
                ptPriData->tCliReg[dwLoop].dwDatFuncType = ptRecvAppData->tCliReg[dwRegLoop].dwDatFuncType;
                ptPriData->tCliReg[dwLoop].ucReadFlag = OAM_CLI_REG_NOT_READED;
                ptPriData->tCliReg[dwLoop].ucRegType = OAM_CLI_REG_TYPE_SPECIAL;
                (*pdwRegNum)++;
                bHasSpecial = TRUE;
            }
        }
    }

    if (bHasSpecial)
    {
        /* ֹͣ��������ִ�е�����Ͽ��������� */
        Oam_CfgStopAllLinkExcutingCmd();

        /* �������ע���dat�ڴ� */
        OAM_CFGRemoveAllDat();
        /*����ǵ�Ƭ����������汾 ��Ϣ����vmm���»�ȡ*/
        if (!Oam_CfgIfCurVersionIsPC())
        {
            g_ptOamIntVar->wDatSum = 0;
        }
        
        RegDatToVmm();
        ReqAllCfgVerInfo();

        return;
    }

    for(dwRegLoop = 0; dwRegLoop < ptRecvAppData->dwRegCount; dwRegLoop++)
    {
        if(dwRegLoop >= OAM_CLI_DAT_SHM_MAX_NUM)
            break;

        for(dwLoop = 0; dwLoop < ptVar->tPlatMpCliAppInfo.dwRegCount; dwLoop++)
        {
            if(dwLoop >= OAM_CLI_DAT_SHM_MAX_NUM)
                break;

            /* ֱ����Ϊ�Ѿ�ע����� */
            if(ptRecvAppData->tCliReg[dwRegLoop].dwDatFuncType == ptVar->tPlatMpCliAppInfo.tCliReg[dwLoop].dwDatFuncType)
            {
                /* added by fls on 2009-02-27 (CRDCR00481329 rpu����֮��,���������������ý��벻��) */
                for (wRegDatCnt = 0; wRegDatCnt < g_wRegDatCount; wRegDatCnt++)
                {
                    if (g_tDatLink[wRegDatCnt].datIndex.tJid.dwJno == ptRecvAppData->tCliReg[dwRegLoop].tAppJid.dwJno)
                    {
                        memcpy(&g_tDatLink[wRegDatCnt].datIndex.tJid, &ptRecvAppData->tCliReg[dwRegLoop].tAppJid, sizeof(JID));
                        if (ptRecvAppData->tCliReg[dwRegLoop].dwDatFuncType == FUNC_SCRIPT_CLIS_BRS)
                            memcpy(&g_ptOamIntVar->jidProtocol, &ptRecvAppData->tCliReg[dwRegLoop].tAppJid, sizeof(JID));
                        break;
                    }
                }
                /* end of add */
                ptVar->tPlatMpCliAppInfo.tCliReg[dwLoop].ucCmpFlag = 1;
                break;
            }
        }

        /* δ�Ƚϳ���ͬ�Ĺ������ͣ���Ҫ����ע��*/
        if(dwLoop == ptVar->tPlatMpCliAppInfo.dwRegCount)
        {
            /* �����ڴ��к�ȫ���������е���ע��dat�����ݲ��ܳ������ֵ */
            if ((dwRegedCntOfShm + ptVar->tPlatMpCliAppInfo.dwRegCount) >= OAM_CLI_DAT_SHM_MAX_NUM)
            {
                XOS_SysLog(OAM_CFG_LOG_ERROR, 
                    "Oam_CfgRecvRegA2M: Total of registered dats has reached max!\r");
                break;
            }
                
            /*ע��������job*/
            if(OAM_CLI_SUCCESS != OAM_CFGDatRegister(ptRecvAppData->tCliReg[dwRegLoop].tAppJid, ptRecvAppData->tCliReg[dwRegLoop].dwDatFuncType))
            {
                XOS_SysLog(OAM_CFG_LOG_ERROR, "Oam_CfgRecvRegA2M: register failed, dat functype = %d\n", ptRecvAppData->tCliReg[dwRegLoop].dwDatFuncType);
                continue;
            }
            else
            {
                ptRecvAppData->tCliReg[dwRegLoop].ucReadFlag = OAM_CLI_REG_READED;
                XOS_SysLog(OAM_CFG_LOG_ERROR, "Oam_CfgRecvRegA2M: register success, dat functype = %d\n", ptRecvAppData->tCliReg[dwRegLoop].dwDatFuncType);
            }
            
            ptVar->tPlatMpCliAppInfo.tCliReg[dwLoop].ucCmpFlag = 1;
            memcpy(&(ptVar->tPlatMpCliAppInfo.tCliReg[dwLoop].tAppJid), &(ptRecvAppData->tCliReg[dwRegLoop].tAppJid), sizeof(JID));
            ptVar->tPlatMpCliAppInfo.tCliReg[dwLoop].dwDatFuncType = ptRecvAppData->tCliReg[dwRegLoop].dwDatFuncType;
            ptVar->tPlatMpCliAppInfo.dwRegCount++;
        }
    }

    return;
}


/**********************************************************************
* �������ƣ�VOID RecvRegExA2M
* ��������������Agent��Manager��ͬ����Ϣ����ƽ̨ҵ�����ע�����Ϣ
* ���ʵı���
* �޸ĵı���
* ���������T_AlmAgtVars *ptVar��˽��������
            LPVOID pMsgPara:     ��Ϣ����
* �����������
* �� �� ֵ����
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2008-7-24             ������      ����
************************************************************************/
void RecvRegExA2M(T_OAM_INTERPRET_VAR *ptVar, LPVOID pMsgPara, BOOLEAN bIsSameEndian)
{
    WORD32          dwIndex = 0;
    WORD32          dwRegLoop = 0;
    /*T_CliAppAgtRegEx   *ptRecvAppData = NULL;*/
    JID                   tSenderJid;

    T_CliAppAgtRegEx *ptRecvAppData,tMsgRecvData;
//    DWORD dwVer = 0;
    WORD16 wLen = 0;
    
    /******************** ��Ϣ���� ��ʼ ********************/
    /* ��Ϣ�ṹĬ�ϳ�ʼ�� */
    memset(&tMsgRecvData, 0xFF, sizeof(tMsgRecvData));

    /* ������ջ��������Ϣ���ݽṹ�����⶯̬�ڴ�����/�ͷ� */
    ptRecvAppData  = &tMsgRecvData;

    /* ������Ϣ�汾����ת������ */
 //   XOS_GetMsgAppVer((BYTE *)&dwVer);
    XOS_GetMsgDataLen(PROCTYPE_OAM_CLIMANAGER, (WORD16 *)&wLen);
//    INVERT_MSG_T_CliAppAgtRegEx((BYTE **)&ptRecvAppData,pMsgPara,dwVer,wLen);
    /******************** ��Ϣ���� ���� ********************/
#if 0
    if(XOS_SUCCESS != XOS_GetCurMsgData((BYTE**)&ptRecvAppData))
        return;
#endif
    /*�ֽ���ת��*/
    if (!bIsSameEndian)
    {
        ptRecvAppData->dwSemid = XOS_InvertWord32(ptRecvAppData->dwSemid);
        ptRecvAppData->dwRegCount = XOS_InvertWord32(ptRecvAppData->dwRegCount);
        for(dwRegLoop=0; dwRegLoop<ptRecvAppData->dwRegCount; dwRegLoop++)
        {
            ptRecvAppData->tCliReg[dwRegLoop].tAppJid.dwJno = XOS_InvertWord32(ptRecvAppData->tCliReg[dwRegLoop].tAppJid.dwJno);
            ptRecvAppData->tCliReg[dwRegLoop].tAppJid.dwDevId = XOS_InvertWord32(ptRecvAppData->tCliReg[dwRegLoop].tAppJid.dwDevId);
            ptRecvAppData->tCliReg[dwRegLoop].tAppJid.wModule = XOS_InvertWord16(ptRecvAppData->tCliReg[dwRegLoop].tAppJid.wModule);
            ptRecvAppData->tCliReg[dwRegLoop].tAppJid.wUnit = XOS_InvertWord16(ptRecvAppData->tCliReg[dwRegLoop].tAppJid.wUnit);
            ptRecvAppData->tCliReg[dwRegLoop].dwNetId = XOS_InvertWord32(ptRecvAppData->tCliReg[dwRegLoop].dwNetId);
        }
    }

    /*��ȡ�����ߵ�jid*/
    memset(&tSenderJid, 0, sizeof(tSenderJid));
    XOS_Sender(&tSenderJid);

    /*OMP�ϲ���Ҫͬ��*/
    if (IsSameLogicAddr(&(g_ptOamIntVar->jidSelf), &tSenderJid))
    {
        XOS_SysLog(OAM_CFG_LOG_NOTICE, "RecvRegExA2M: receive register table(ex) from omp cliagent, ignored");
        return;
    }

    /* �Ƚϡ��ұ��浽������ */
    for(dwRegLoop=0; dwRegLoop<ptRecvAppData->dwRegCount; dwRegLoop++)
    {
        if(dwRegLoop >= OAM_CLI_APP_SHM_NUM)
            break;

        /*����Ѿ�ע����ˣ�����ע����Ϣ*/
        if (LompCliAppRegInfoIsExist(ptRecvAppData->tCliReg[dwRegLoop].dwNetId))
        {
            UpdateCliAppRegInfo(ptRecvAppData->tCliReg[dwRegLoop].dwNetId,&(ptRecvAppData->tCliReg[dwRegLoop].tAppJid));
        }
        else
        {
            /* δע�������Ҫд��˽�������� */
            dwIndex = ptVar->tLompCliAppRegInfo.dwRegCount;
            memcpy(&(ptVar->tLompCliAppRegInfo.tCliReg[dwIndex].tAppJid), &(ptRecvAppData->tCliReg[dwRegLoop].tAppJid), sizeof(JID));
            ptVar->tLompCliAppRegInfo.tCliReg[dwIndex].dwNetId = ptRecvAppData->tCliReg[dwRegLoop].dwNetId;
            ptVar->tLompCliAppRegInfo.dwRegCount++;
        }
    }

    return;
}

/**********************************************************************
* �������ƣ�UpdateCliAppRegInfo
* ��������������ҵ��ע�������Ϣ
* ���ʵı���
* �޸ĵı���
* ���������WORD32 dwNetId : ��Ҫ������Ԫid
* �����������
* �� �� ֵ��   ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2009-8-11          ������    ����
************************************************************************/
static void UpdateCliAppRegInfo(WORD32 dwNetId,JID *ptJid)
{
    WORD32 dwLoop = 0;
    BOOLEAN bRet = FALSE;
    
    /*�������*/
    if (NULL == ptJid)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
                       "....................FUNC:%s LINE:%d NULL PTR! ....................",__func__,__LINE__);
        return;
    }

    if (g_ptOamIntVar)
    {        
        /*�����������ע����Ϣ*/
        for(dwLoop=0; dwLoop< g_ptOamIntVar->tLompCliAppRegInfo.dwRegCount; dwLoop++)
        {
            if(dwLoop >= OAM_CLI_APP_SHM_NUM)
            {
                bRet = FALSE;
                break;
            }

            /* �Ѿ�ע����� */
            if(dwNetId == g_ptOamIntVar->tLompCliAppRegInfo.tCliReg[dwLoop].dwNetId)
            {
                memcpy(&(g_ptOamIntVar->tLompCliAppRegInfo.tCliReg[dwLoop].tAppJid), ptJid, sizeof(JID));
                bRet = TRUE;
                break;
            }
        }
    }


    /*���������ע�����û�У���鹲���ڴ�*/
    if(!bRet)
    {
        T_CliAppAgtRegEx *ptOamCliReg = NULL;
        /*������ʹ�ú����Oam_UnLockRegTable�ͷ���*/
        ptOamCliReg = Oam_GetRegExTableAndLock();
        if (ptOamCliReg)
        {
            for(dwLoop = 0; dwLoop < ptOamCliReg->dwRegCount; dwLoop++)
            {
                if(dwLoop >= OAM_CLI_APP_SHM_NUM)
                {
                    bRet = FALSE;
                    break;
                }

                if (dwNetId == ptOamCliReg->tCliReg[dwLoop].dwNetId)
                {
                    memcpy(&(ptOamCliReg->tCliReg[dwLoop].tAppJid), ptJid, sizeof(JID));
                    bRet = TRUE;
                    break;
                }
            }

            Oam_UnLockRegExTable(ptOamCliReg);
        }
    }

    return;
}


/**********************************************************************
* �������ƣ�LompCliAppRegInfoIsExist
* �������������ָ������Ԫ�Ƿ���ҵ��ע������Ѿ�����
* ���ʵı���
* �޸ĵı���
* ���������WORD32 dwNetId : ��Ҫ������Ԫid
* �����������
* �� �� ֵ��    TRUE - �Ѿ�ע�����
                                 FALSE -û��ע���
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2009-5-27          ���    ����
************************************************************************/
BOOLEAN LompCliAppRegInfoIsExist(WORD32 dwNetId)
{
    WORD32 dwLoop = 0;
    BOOLEAN bRet = FALSE;

    if (g_ptOamIntVar)
    {        
        /*�����������ע����Ϣ*/
        for(dwLoop=0; dwLoop< g_ptOamIntVar->tLompCliAppRegInfo.dwRegCount; dwLoop++)
        {
            if(dwLoop >= OAM_CLI_APP_SHM_NUM)
            {
                bRet = FALSE;
                break;
            }

            /* �Ѿ�ע����� */
            if(dwNetId == g_ptOamIntVar->tLompCliAppRegInfo.tCliReg[dwLoop].dwNetId)
            {
                bRet = TRUE;
                break;
            }
        }
    }


    /*���������ע�����û�У���鹲���ڴ�*/
    if(!bRet)
    {
        T_CliAppAgtRegEx *ptOamCliReg = NULL;
        /*������ʹ�ú����Oam_UnLockRegTable�ͷ���*/
        ptOamCliReg = Oam_GetRegExTableAndLock();
        if (ptOamCliReg)
        {
            for(dwLoop = 0; dwLoop < ptOamCliReg->dwRegCount; dwLoop++)
            {
                if(dwLoop >= OAM_CLI_APP_SHM_NUM)
                {
                    bRet = FALSE;
                    break;
                }

                if (dwNetId == ptOamCliReg->tCliReg[dwLoop].dwNetId)
                {
                    bRet = TRUE;
                    break;
                }
            }

            Oam_UnLockRegExTable(ptOamCliReg);
        }
    }

    return bRet;
}

/**********************************************************************
* �������ƣ�OamRegCntOfShm
* ������������ȡ�����ڴ��е�datע�����
* ���ʵı���
* �޸ĵı���
* �����������
* �����������
* �� �� ֵ�� WORD32 - �����ڴ�����ע���dat����
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2010-11-15          ������    ����
************************************************************************/
BOOLEAN OamRegCntOfShm(WORD32 *pdwRegedCnt)
{
#if 0
    T_CliAppReg   *ptOamCliReg = NULL;

    /*������ʹ�ú����Oam_UnLockRegTable�ͷ���*/
    ptOamCliReg = Oam_GetRegTableAndLock();
    if (!ptOamCliReg)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "OamRegCntOfShm: failed to create shm which contains rigister table and set lock on it!\r");
        return FALSE;
    }

    (*pdwRegedCnt) = ptOamCliReg->dwRegCount;
    
    if (!Oam_UnLockRegTable(ptOamCliReg))
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "OamRegCntOfShm: failed to unlock shm which contains register table ");
        return FALSE;
    }
#endif
    return TRUE;
}


