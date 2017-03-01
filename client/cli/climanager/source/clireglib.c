/*********************************************************************
* ��Ȩ���� (C)2008, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ� CliRegLib.c
* �ļ���ʶ��
* ����ժҪ��CLI����ģ���ṩ��Ӧ�õ�ע��/ע��������
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

#include <unistd.h>
#include <string.h>
#include <assert.h>
#include "clireg.h"
#include "clireglib.h"
#include "pub_tmp.h"
#include "pub_addition.h"
#include "oam_cfg_common.h"

static BOOLEAN GetCliManagerJid(JID *ptJid)
{
    BYTE                   ucBoardStyle;
    if (!ptJid)
    {
        return FALSE;
    }

    /* ����climanager ���̵�JID*/
    memset(ptJid, 0, sizeof(JID));
    /* �����CMM�������壬���챾���JID */
    XOS_GetBoardStyle(&ucBoardStyle);
    if (BOARD_DEPEND != ucBoardStyle)
    {
        XOS_GetSelfJID(ptJid);
        ptJid->dwJno = XOS_ConstructJNO(PROCTYPE_OAM_CLIMANAGER, 1);
    }
    /* ������OMP��JID */
    else
    {
        ptJid->wModule = 1;
        ptJid->dwJno = XOS_ConstructJNO(PROCTYPE_OAM_CLIMANAGER, 1);
        ptJid->wUnit   = INVALID_UNIT;
        ptJid->ucSUnit = INVALID_SUNIT;
        ptJid->ucSubSystem = INVALID_SUBSYS;
        ptJid->ucRouteType = COMM_MASTER_SERVICE;
    }
    return TRUE;
}

/**********************************************************************
* �������ƣ�BYTE  OamCliRegFunc
* ����������ƽ̨Ӧ����OAM����ע����Ҫ�������õ���Ϣ
* ���ʵı���
* �޸ĵı���
* ���������JID tJobID��          ��OAM�������ý�����JID
*           WORD32 dwDatFuncType����JOB��OAM�������ý���ʱ��ʹ�õ���DAT�ű�����
* �����������
* �� �� ֵ��OAM_CLI_SUCCESS(0)���ɹ�
*           OAM_CLI_FAIL(1)��   ʧ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* 2008-7-24             ������
* ---------------------------------------------------------------------
* 2006-4-11 18:57    V1.0    wushg      ����
************************************************************************/
BYTE  OamCliRegFunc(JID tJobID, WORD32 dwDatFuncType)
{
#if 0
    WORD16        wLoop = 0;
    JID           tCliMJid;
    T_CliAppReg   *ptOamCliReg = NULL;
    BOOLEAN       bRet = FALSE;

    /*������ʹ�ú����Oam_UnLockRegTable�ͷ���*/
    ptOamCliReg = Oam_GetRegTableAndLock();
    if (!ptOamCliReg)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,"[OamCliRegFunc] failed to create shm which contains rigister table and set lock on it!\r");
        return OAM_CLI_FAIL;
    }

    bRet = GetCliManagerJid(&tCliMJid);
    if (!bRet)
    {
        Oam_UnLockRegTable(ptOamCliReg);
        return OAM_CLI_FAIL;
    }

    for(wLoop=0; wLoop<OAM_CLI_DAT_SHM_MAX_NUM; wLoop++)
    {
        if(0 == ptOamCliReg->tCliReg[wLoop].dwDatFuncType)
        {
            ptOamCliReg->tCliReg[wLoop].dwDatFuncType = dwDatFuncType;
            ptOamCliReg->tCliReg[wLoop].ucReadFlag = OAM_CLI_REG_NOT_READED;
            ptOamCliReg->tCliReg[wLoop].ucRegType = OAM_CLI_REG_TYPE_3GPLAT;
            memset(&ptOamCliReg->tCliReg[wLoop].tAppJid, 0, sizeof(JID));
            memcpy(&ptOamCliReg->tCliReg[wLoop].tAppJid, &tJobID, sizeof(JID));
            if (tJobID.ucRouteType != COMM_BACK_SERVICE)
                ptOamCliReg->tCliReg[wLoop].tAppJid.ucRouteType = COMM_MASTER_SERVICE;
            ++(ptOamCliReg->dwRegCount);

            /* ֻ������ŷ� */
            if(BOARD_MASTER == XOS_GetBoardMSState())
            {
                XOS_SendAsynMsg(EV_OAMCLI_APP_REG, (BYTE*)(&(ptOamCliReg->tCliReg[wLoop])),
                                sizeof(T_CliAppData), 0, XOS_MSG_MEDIUM, (void*)&tCliMJid);
            }
            Oam_UnLockRegTable(ptOamCliReg);
            return  OAM_CLI_SUCCESS;
        }
        /* ����ע��� */
        else if(ptOamCliReg->tCliReg[wLoop].dwDatFuncType == dwDatFuncType)
        {
            Oam_UnLockRegTable(ptOamCliReg);
            /* ��������ֶ��JOBע��ͬһ��DAT */
            if(ptOamCliReg->tCliReg[wLoop].tAppJid.dwJno != tJobID.dwJno)
            {
                XOS_SysLog(OAM_CFG_LOG_ERROR, 
                    "OamCliRegFunc: A dat is only registered by one job, can not by much job!\r");
                return OAM_CLI_FAIL;
            }
            else
                return OAM_CLI_SUCCESS;   /* �ظ�ע����Ϊ�ɹ� */
        }
    }

    /* �Ѿ��ﵽ���ע����Ŀ */
    XOS_SysLog(OAM_CFG_LOG_ERROR, 
        "OamCliRegFunc: Number of registered dat has reached max!\r");

    Oam_UnLockRegTable(ptOamCliReg);
    return OAM_CLI_FAIL;
#endif 
    return OAM_CLI_SUCCESS;
}

/**********************************************************************
* �������ƣ�BYTE  OamCliUnRegFunc
* ����������ƽ̨Ӧ����OAM����ע���������ù�����Ϣ
            ����OAM���յ�OSS
* ���ʵı���
* �޸ĵı���
* ���������JID tJobID��          ��OAM�������ý�����JID
* �����������
* �� �� ֵ��OAM_CLI_SUCCESS(0)���ɹ�
*           OAM_CLI_FAIL(1)��   ʧ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* 2008-7-24             ������
* ---------------------------------------------------------------------
* 2006-4-11 18:57    V1.0    wushg      ����
************************************************************************/
BYTE  OamCliUnRegFunc(JID tJobID)
{
#if 0
    WORD16        wLoop = 0;
    JID           tCliMJid;
    T_CliAppReg   *ptOamCliReg = NULL;

    /*������ʹ�ú����Oam_UnLockRegTable�ͷ���*/
    ptOamCliReg = Oam_GetRegTableAndLock();
    if (!ptOamCliReg)
    {
        XOS_SysLog(OAM_CFG_LOG_DEBUG,"failed to create shm which contains rigister table and set lock on it!\r");
        return OAM_CLI_FAIL;
    }

    /* �����ڴ���û�����ݣ�����Ϊ�ɹ� */
    if(0 == ptOamCliReg->dwRegCount)
    {
        Oam_UnLockRegTable(ptOamCliReg);
        return OAM_CLI_SUCCESS;
    }

    for(wLoop=0; wLoop<OAM_CLI_DAT_SHM_MAX_NUM; wLoop++)
    {
        /* �ҵ���ͬ���̺ţ���ɾ���ڵ� */
        if(ptOamCliReg->tCliReg[wLoop].tAppJid.dwJno == tJobID.dwJno)
        {
            /* ��CLIManager��ע����Ϣ */
            /* ֻ������ŷ� */
            if(BOARD_MASTER == XOS_GetBoardMSState())
            {
                BOOLEAN bRet = GetCliManagerJid(&tCliMJid);
                if (!bRet)
                {
                    Oam_UnLockRegTable(ptOamCliReg);
                    return OAM_CLI_FAIL;
                }

                XOS_SendAsynMsg(EV_OAMCLI_APP_UNREG, (BYTE*)(&(ptOamCliReg->tCliReg[wLoop])),
                                sizeof(T_CliAppData), 0, XOS_MSG_MEDIUM, (void*)&tCliMJid);
            }

            memset(&ptOamCliReg->tCliReg[wLoop], 0, sizeof(T_CliAppReg));
            /* �������һ���ڵ� */
            if(1 != ptOamCliReg->dwRegCount - wLoop)
            {
                /* �����һ���ڵ�Ų���˽ڵ� */
                memcpy(&ptOamCliReg->tCliReg[wLoop].tAppJid, &ptOamCliReg->tCliReg[ptOamCliReg->dwRegCount-1].tAppJid, sizeof(JID));
                ptOamCliReg->tCliReg[wLoop].dwDatFuncType = ptOamCliReg->tCliReg[ptOamCliReg->dwRegCount-1].dwDatFuncType;

                /* ������һ���ڵ������ */
                memset(&ptOamCliReg->tCliReg[ptOamCliReg->dwRegCount-1], 0, sizeof(T_CliAppData));

            }

            --(ptOamCliReg->dwRegCount);
            Oam_UnLockRegTable(ptOamCliReg);
            return OAM_CLI_SUCCESS;
        }
    }

    Oam_UnLockRegTable(ptOamCliReg);
    return OAM_CLI_FAIL;
#endif
    return OAM_CLI_SUCCESS;
}

/**********************************************************************
* �������ƣ�BYTE  OamCliRegFuncEx
* ������������ƽ̨��Ӧ��/ҵ�������OAM����ע����Ҫ�������õ���Ϣ
* ���ʵı���
* �޸ĵı���
* ���������JID tJobID��          ��OAM�������ý�����JID
*                            WORD32 dwNetId��    LOMP���ڵ���Ԫ��ʾ
* �����������
* �� �� ֵ��OAM_CLI_SUCCESS(0)���ɹ�
*           OAM_CLI_FAIL(1)��   ʧ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* 2008-7-24             ������
* ---------------------------------------------------------------------
* 2006-4-11 18:57    V1.0    wushg      ����
************************************************************************/
BYTE  OamCliRegFuncEx(JID tJobID, WORD32 dwNetId)
{
    WORD16        wLoop = 0;
    JID           tCliMJid;
    T_CliAppAgtRegEx *ptOamCliReg = NULL;
    WORD16        wMaxRegRecord = 0;
    BOOLEAN       bRet  = FALSE;
    BYTE        ucRetValue = OAM_CLI_SUCCESS;

    /*��ȡclimanager��jid*/
    bRet = GetCliManagerJid(&tCliMJid);
    if (!bRet && (BOARD_MASTER == XOS_GetBoardMSState()))
    {
        return OAM_CLI_FAIL;
    }

    /*OMP��������������֧�ֵ�ҵ��ע��������ͬ*/
    if (tJobID.wModule == tCliMJid.wModule)
    {
        wMaxRegRecord = OAM_CLI_APP_MGT_SHM_MAX_NUM;
    }
    else
    {
        wMaxRegRecord = OAM_CLI_APP_AGT_SHM_MAX_NUM;
    }
    
    /*������ʹ�ú����Oam_UnLockRegTable�ͷ���*/
    ptOamCliReg = Oam_GetRegExTableAndLock();
    if (!ptOamCliReg)
    {
        XOS_SysLog(OAM_CFG_LOG_DEBUG,"failed to create shm which contains rigister table and set lock on it!\r");
        return OAM_CLI_FAIL;
    }

    for(wLoop=0; wLoop<wMaxRegRecord; wLoop++)
    {    
        /* ����ע��� */
        if(ptOamCliReg->tCliReg[wLoop].dwNetId == dwNetId)
        {
            /* ��������ֶ��JOBע��ͬһ����Ԫ���� */
            if(ptOamCliReg->tCliReg[wLoop].tAppJid.dwJno != tJobID.dwJno)
            {
                ucRetValue = OAM_CLI_FAIL;
            }
            else
            {
                ucRetValue = OAM_CLI_SUCCESS; /* �ظ�ע����Ϊ�ɹ� */
            }

            break;
        }
        /* �ҵ���һ����Ԫ��������Ϊ0����Ϊ����ڵ�û��ע��� */
        else if(0 == ptOamCliReg->tCliReg[wLoop].dwNetId)
        {

            ptOamCliReg->tCliReg[wLoop].dwNetId = dwNetId;
            memcpy(&ptOamCliReg->tCliReg[wLoop].tAppJid, &tJobID, sizeof(JID));
            if (tJobID.ucRouteType != COMM_BACK_SERVICE)
                ptOamCliReg->tCliReg[wLoop].tAppJid.ucRouteType = COMM_MASTER_SERVICE;
            ++(ptOamCliReg->dwRegCount);

            /* ��CLIManager����Ϣ */
            /* ֻ������ŷ� */
            if(BOARD_MASTER == XOS_GetBoardMSState())
            {
                XOS_SendAsynMsg(EV_OAMCLI_APP_REGEX, (BYTE*)(&(ptOamCliReg->tCliReg[wLoop])),
                                sizeof(ptOamCliReg->tCliReg[wLoop]), 0, XOS_MSG_MEDIUM,(void*)&tCliMJid);
            }
            
            ucRetValue = OAM_CLI_SUCCESS; 
            break;
        }
    }

    Oam_UnLockRegExTable(ptOamCliReg);
    return ucRetValue;

}

/**************************************************************************
* �������ƣ�T_CliAppReg* GetRegTableAndLock(VOID)
* ������������ȡ��ƽ̨�����ڴ���ע����ַ������
* ���ʵı���
* �޸ĵı���
* ���������
* ���������
* �� �� ֵ��    NULL -����ʧ��
                              ��NULL -��ƽ̨�����ڴ��ַ
* ����˵������Ҫ��Oam_UnLockRegExTable һ��ʹ��
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2009/03/17    V1.0    ������      ����
**************************************************************************/
T_CliAppAgtRegEx* Oam_GetRegExTableAndLock(void)
{
 //   DWORD     dwRet = 0;
  //  WORD32 dwSemid = 0;
    /*CRDCM00034213 ��ֹ�����ڴ汻�ظ�ӳ��yinhao@20090324*/
    static T_CliAppAgtRegEx *ptOamCliReg = NULL;

#if 0
    /* �������̻����ź���������JOB��Ϣ���� */
    dwSemid = XOS_CreateProcessSem(OAM_SEM_CLI_REGEX, 1, MUTEX_STYLE);
    if(dwSemid == XOS_INVALID_VOSSEM)
    {
        XOS_SysLog(OAM_CFG_LOG_DEBUG, "Oam_GetRegExTableAndLock: failed to CreateNameSem , return %u", dwSemid);
        return NULL;
    }

    /*CRDCM00034213 ��ֹ�����ڴ汻�ظ�ӳ��yinhao@20090324*/
    if (!ptOamCliReg)
    {
        /* ��������JOB��Ϣ�Ĺ����ڴ� */
        dwRet = XOS_CreateShmByIndex(OAM_SHM_CLI_REGEX,
                              sizeof(*ptOamCliReg),
                              XOS_RDWR | XOS_CREAT,
                              (BYTE**)&ptOamCliReg);

        if ((XOS_SUCCESS != dwRet) && (XOS_VOS_SHM_EXIST != dwRet))
        {
            XOS_SysLog(OAM_CFG_LOG_DEBUG, "Oam_GetRegExTableAndLock: failed to c CreateShm , return %d",dwRet);
            return NULL;
        }    

        /*
        ��鹲���ڴ��Ƿ���Ҫ����:
        �������������µģ��͸���
        */
        #ifdef OS_VXWORKS
        {
            /*vxworks ���Ը��ݷ���ֵ�ж��Ƿ�Ҫ��ʼ��*/
            if (XOS_SUCCESS == dwRet)	
            {
                XOS_SysLog(OAM_CFG_LOG_DEBUG,
                       "clear shm which contains register table");

                memset(ptOamCliReg, 0, sizeof(T_CliAppAgtRegEx));
            }
            else
            {
                XOS_SysLog(OAM_CFG_LOG_DEBUG, "GetRegTableAndLock: shm does not need clear");
            }
        }
        #else
        {
            pid_t platpid = getppid();
            if (platpid != ptOamCliReg->apppid)
            {
                XOS_SysLog(OAM_CFG_LOG_DEBUG,
                           "clear shm which contains register table, oldapppid=%u,newapppid=%u",
                           ptOamCliReg->apppid,
                           platpid);
                memset(ptOamCliReg, 0, sizeof(T_CliAppAgtRegEx));
                ptOamCliReg->apppid = platpid;
            }
            else
            {
                XOS_SysLog(OAM_CFG_LOG_DEBUG, "Oam_GetRegExTableAndLock: shm does not need clear");
            }
        }
        #endif
    }
    ptOamCliReg->dwSemid = dwSemid;
    if (!XOS_ProcessSemP(ptOamCliReg->dwSemid, WAIT_FOREVER))
    {
        XOS_SysLog(OAM_CFG_LOG_DEBUG, "Oam_GetRegExTableAndLock: failed to XOS_ProcessSemP ");
        return NULL;
    }
#endif
    return ptOamCliReg;
}

/**************************************************************************
* �������ƣ�VOID Oam_UnLockRegExTable(T_CliAppAgtRegEx *ptOamCliReg )
* ����������ȥ���Է�ƽ̨�����ڴ���ע����ַ������
* ���ʵı���
* �޸ĵı���
* ���������ptOamCliReg:ע����ַ
* �����������
* �� �� ֵ��    ��
* ����˵������Ҫ��Oam_GetRegExTableAndLock(VOID)һ��ʹ��
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2009/03/17    V1.0    ������      ����
**************************************************************************/
BOOLEAN Oam_UnLockRegExTable(T_CliAppAgtRegEx *ptOamCliReg )
{
    assert(ptOamCliReg != NULL);
    if (!ptOamCliReg)
    {
        return FALSE;
    }

    return XOS_ProcessSemV(ptOamCliReg->dwSemid);
}

/**************************************************************************
* �������ƣ�T_CliAppReg* GetRegTableAndLock(VOID)
* ������������ȡ�����ڴ���ע����ַ������
* ���ʵı���
* �޸ĵı���
* ���������
* ���������
* �� �� ֵ��    NULL -����ʧ��
                              ��NULL -�����ڴ��ַ
* ����˵������Ҫ��Oam_UnLockRegTable һ��ʹ��
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2008/10/20    V1.0    ���      ����
**************************************************************************/
T_CliAppReg* Oam_GetRegTableAndLock(void)
{
 //   DWORD     dwRet = 0;
//    WORD32 dwSemid = 0;
    /*CRDCM00034213 ��ֹ�����ڴ汻�ظ�ӳ��yinhao@20090324*/
    static T_CliAppReg   *ptOamCliReg = NULL;
#if 0
    /* �������̻����ź���������JOB��Ϣ���� */
    dwSemid = XOS_CreateProcessSem(OAM_SEM_CLI_REG, 1, MUTEX_STYLE);
    if(dwSemid == XOS_INVALID_VOSSEM)
    {
        XOS_SysLog(OAM_CFG_LOG_DEBUG, "GetRegTableAndLock: failed to CreateNameSem , return %u", dwSemid);
        return NULL;
    }

    /*CRDCM00034213 ��ֹ�����ڴ汻�ظ�ӳ��yinhao@20090324*/
    if (!ptOamCliReg)
    {
        /* ��������JOB��Ϣ�Ĺ����ڴ� */
        dwRet = XOS_CreateShmByIndex(OAM_SHM_CLI_REG,
                              sizeof(T_CliAppReg),
                              XOS_RDWR | XOS_CREAT,
                              (BYTE**)&ptOamCliReg);

        if ((XOS_SUCCESS != dwRet) && (XOS_VOS_SHM_EXIST != dwRet))
        {
            XOS_SysLog(OAM_CFG_LOG_DEBUG, "GetRegTableAndLock: failed to c CreateShm , return %d",dwRet);
            return NULL;
        }

        /*
        ��鹲���ڴ��Ƿ���Ҫ����:
        �������������µģ��͸���
        */
        #ifdef OS_VXWORKS
        {
            /*vxworks ���Ը��ݷ���ֵ�ж��Ƿ�Ҫ��ʼ��*/
            if (XOS_SUCCESS == dwRet)	
            {
                XOS_SysLog(OAM_CFG_LOG_DEBUG,
                       "clear shm which contains register table");

                memset(ptOamCliReg, 0, sizeof(T_CliAppReg));
            }
            else
            {
                XOS_SysLog(OAM_CFG_LOG_DEBUG, "GetRegTableAndLock: shm does not need clear");
            }
        }
        #else
        {
            pid_t platpid = getppid(); 
            if (platpid != ptOamCliReg->platpid)
            {
                XOS_SysLog(OAM_CFG_LOG_DEBUG,
                           "clear shm which contains register table, oldplatpid=%u,newplatpid=%u",
                           ptOamCliReg->platpid,
                           platpid);

                memset(ptOamCliReg, 0, sizeof(T_CliAppReg));
                ptOamCliReg->platpid = platpid;
            }
            else
            {
                XOS_SysLog(OAM_CFG_LOG_DEBUG, "GetRegTableAndLock: shm does not need clear");
            }
        }
        #endif
   
    }


    ptOamCliReg->dwSemid = dwSemid;
    if (!XOS_ProcessSemP(ptOamCliReg->dwSemid, WAIT_FOREVER))
    {
        XOS_SysLog(OAM_CFG_LOG_DEBUG, "GetRegTableAndLock: failed to XOS_ProcessSemP ");
        return NULL;
    }
#endif
    return ptOamCliReg;
}

/**************************************************************************
* �������ƣ�VOID UnLockRegTable(VOID)
* ����������ȥ���Թ����ڴ���ע����ַ������
* ���ʵı���
* �޸ĵı���
* ���������
* ���������
* �� �� ֵ��    ��
* ����˵������Ҫ��Oam_GetRegTableAndLock(VOID)һ��ʹ��
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2008/10/20    V1.0    ���      ����
**************************************************************************/
BOOLEAN Oam_UnLockRegTable(T_CliAppReg *ptOamCliReg )
{
    assert(ptOamCliReg != NULL);
    if (!ptOamCliReg)
    {
        return FALSE;
    }

    return XOS_ProcessSemV(ptOamCliReg->dwSemid);
}

#if 0

/*
������������ע���ʹ�ã�managerҲ��
Ϊ�˼�С���С���������ﶨ�壬��clireg.h������
*/

static CHAR *OAM_RTrim(CHAR *pStr)
{
    CHAR *pRet = pStr;
    WORD16 wLen = 0;
    SWORD16 i = 0;
    if (NULL == pStr)/*����Ŀմ���ֱ�ӷ���*/
    {
        return NULL;
    }
    wLen = strlen(pStr);
    for(i = wLen - 1; i >= 0; i--)
    {
        if((*(pStr + i) == ' ') ||
                (*(pStr + i) == '\t') ||
                (*(pStr + i) == '\r') ||
                (*(pStr + i) == '\n'))
        {
            *(pRet + i) = '\0';
        }
        else
        {
            break;
        }
    }

    return pStr;
}

static CHAR *OAM_LTrim(CHAR *pStr )
{
    CHAR *pRet = pStr;
    WORD16 wLen = 0;
    SWORD16 i = 0;
    if (NULL == pStr)/*����Ŀմ���ֱ�ӷ���*/
    {
        return NULL;
    }
    wLen = strlen(pStr);
    for(i = 0; i < wLen; i++)
    {
        if((*(pStr + i) == ' ') ||
                (*(pStr + i) == '\t') ||
                (*(pStr + i) == '\r') ||
                (*(pStr + i) == '\n'))
        {
            pRet++;
        }
        else
        {
            break;
        }
    }

    return pRet;
}

static int StrCmpNoCase(CHAR * pInStr, CHAR * pPatternStr)
{
    CHAR * pInChar = pInStr;
    CHAR * pPatternChar = pPatternStr;
    CHAR cLowerIn, cLowerPattern;
    int nLenInStr = strlen(pInStr);
    int nLenPatternStr = strlen(pPatternStr);
    int i = 1;

    while (i <= nLenInStr && i <= nLenPatternStr)
    {
        cLowerIn = ((*pInChar) >= 'A' && (*pInChar) <= 'Z') ? ((*pInChar) + 32) : (*pInChar);
        cLowerPattern = ((*pPatternChar) >= 'A' && (*pPatternChar) <= 'Z') ? ((*pPatternChar) + 32) : (*pPatternChar);
        if (cLowerIn == cLowerPattern)
        {
            pInChar++;
            pPatternChar++;
        }
        else
            return (cLowerIn < cLowerPattern) ? - 1 : 1;
        i++;
    }

    if (nLenInStr == nLenPatternStr)
        return 0;
    else
        return (nLenInStr < nLenPatternStr) ? - 1 : 1;
}


/*�ݲ�֧�ָ������Ʒ��ʲ���*/
/**********************************************************************
* �������ƣ� Oam_GetParaByName
* �����������������ƴ�MSG_COMM_OAM�ṹ��Ϣ�����ȡ������Ϣ
* ���ʵı���
* �޸ĵı���
* ���������MSG_COMM_OAM *pMsg : ��OAM�������ý�������Ϣ
*                            CHAR *pucParaName : �������ƣ������ִ�Сд
* �����������
* �� �� ֵ��OPR_DATA�ṹָ�룺�ɹ�
*                         NULL��   ʧ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* 2008-8-21           ���
* ---------------------------------------------------------------------
* 2008-8-21 18:57    V1.0   ���    ����
************************************************************************/
OPR_DATA *Oam_GetParaByName(MSG_COMM_OAM *pMsg, CHAR *pucParaName)
{
    INT i = 0;
    CHAR *pucName1 = NULL, *pucName2 = NULL;
    OPR_DATA *pRtnOprData = NULL;

    if (!pMsg || !pucParaName)
    {
        return NULL;
    }

    pRtnOprData = (OPR_DATA *)pMsg->Data;

    for (i = 0; i<pMsg->Number; i++)
    {
        pucName1 = OAM_RTrim(OAM_LTrim(pucParaName));
        pucName2 = OAM_RTrim(OAM_LTrim(pRtnOprData->sName));
        if (0 == StrCmpNoCase(pucName1, pucName2))
        {
            break;
        }

        pRtnOprData = (OPR_DATA *)((BYTE *)pRtnOprData->Data + pRtnOprData->Len);
        if (!pRtnOprData)
        {
            return NULL;
        }
    }

    if ( i == pMsg->Number)
    {
        return NULL;
    }
    else
    {
        return pRtnOprData;
    }

}
#endif 

/**********************************************************************
* �������ƣ�BYTE  OamCliRegFuncSpecial
* ����������SBCҵ����OAM����ע����Ҫ�������õ���Ϣ
* ���ʵı���
* �޸ĵı���
* ���������JID tJobID��          ��OAM�������ý�����JID
*           WORD32 dwDatFuncType����JOB��OAM�������ý���ʱ��ʹ�õ���DAT�ű�����
* �����������
* �� �� ֵ��OAM_CLI_SUCCESS(0)���ɹ�
*           OAM_CLI_FAIL(1)��   ʧ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
*
* ---------------------------------------------------------------------
* 2010-8-30 11:16    V1.0    fuls      ����
************************************************************************/
BYTE  OamCliRegFuncSpecial(JID tJobID, WORD32 dwDatFuncType)
{
#if 0
    WORD16        wLoop = 0;
    JID           tCliMJid;
    T_CliAppReg   *ptOamCliReg = NULL;
    BOOLEAN       bRet = FALSE;

    /*������ʹ�ú����Oam_UnLockRegTable�ͷ���*/
    ptOamCliReg = Oam_GetRegTableAndLock();
    if (NULL == ptOamCliReg)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR,
            "######[OamCliRegFuncSpecial]: failed to create shm which contains rigister table and set lock on it!\r");
        return OAM_CLI_FAIL;
    }

    bRet = GetCliManagerJid(&tCliMJid);
    if (!bRet)
    {
        Oam_UnLockRegTable(ptOamCliReg);
        return OAM_CLI_FAIL;
    }

    /* ���빲���ڴ� */
    for(wLoop = 0; wLoop < OAM_CLI_DAT_SHM_MAX_NUM; wLoop++)
    {
        if(0 == ptOamCliReg->tCliReg[wLoop].dwDatFuncType)
        {
            ptOamCliReg->tCliReg[wLoop].dwDatFuncType = dwDatFuncType;
            ptOamCliReg->tCliReg[wLoop].ucReadFlag = OAM_CLI_REG_NOT_READED;
            ptOamCliReg->tCliReg[wLoop].ucRegType = OAM_CLI_REG_TYPE_SPECIAL;
            memset(&ptOamCliReg->tCliReg[wLoop].tAppJid, 0, sizeof(JID));
            memcpy(&ptOamCliReg->tCliReg[wLoop].tAppJid, &tJobID, sizeof(JID));
            if (tJobID.ucRouteType != COMM_BACK_SERVICE)
                ptOamCliReg->tCliReg[wLoop].tAppJid.ucRouteType = COMM_MASTER_SERVICE;
            ++(ptOamCliReg->dwRegCount);

            /* ֻ������ŷ� */
            if(BOARD_MASTER == XOS_GetBoardMSState())
            {
                XOS_SendAsynMsg(EV_OAMCLI_SPECIAL_REG, (BYTE*)(&(ptOamCliReg->tCliReg[wLoop])),
                                sizeof(T_CliAppData), 0, XOS_MSG_MEDIUM, (void*)&tCliMJid);
            }
            Oam_UnLockRegTable(ptOamCliReg);
            return  OAM_CLI_SUCCESS;
        }
        /* ����ע��� */
        else if(ptOamCliReg->tCliReg[wLoop].dwDatFuncType == dwDatFuncType)
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR,
                "######[OamCliRegFuncSpecial]: The Function Type of Dat has exists!\n");

            Oam_UnLockRegTable(ptOamCliReg);
            /* ��������ֶ��JOBע��ͬһ��DAT */
            if(ptOamCliReg->tCliReg[wLoop].tAppJid.dwJno != tJobID.dwJno)
            {
                XOS_SysLog(OAM_CFG_LOG_ERROR,
                "######[OamCliRegFuncSpecial]: This dat can not be registered with a few JOB!\n");
                return OAM_CLI_FAIL;
            }
            else
            {
                return OAM_CLI_SUCCESS;   /* �ظ�ע����Ϊ�ɹ� */
            }
        }
    }

    /* �Ѿ��ﵽ���ע����Ŀ */
    XOS_SysLog(OAM_CFG_LOG_ERROR, 
        "######[OamCliRegFuncSpecial]: Number of registered dat has reached max!\r");

    Oam_UnLockRegTable(ptOamCliReg);
    return OAM_CLI_FAIL;
#endif
    return OAM_CLI_SUCCESS;
}
