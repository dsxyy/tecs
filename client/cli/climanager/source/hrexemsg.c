/**************************************************************************
* ��Ȩ���� (C)2002, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ�HrExeMsg.c
* �ļ���ʶ��
* ����ժҪ����������Execute����Ϣ
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
#include "saveprtclcfg.h"
#include "convertencoding.h"

/*lint -e420*/
/*lint -e669*/
/*lint -e416*/

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
/**************************************************************************
*                          �ֲ�����ԭ��                                   *
**************************************************************************/
/**************************************************************************
*                          �ֲ�����ʵ��                                   *
**************************************************************************/

/**************************************************************************
* �������ƣ�int XmlProcess_Return_Data
* ��������������������(ר����XML����)
* ���ʵı���
* �޸ĵı���
* ���������TYPE_LINK_STATE *pLinkState,MSG_COMM_OAM * ptMsg
* ���������
* �� �� ֵ��
* ����˵������
* �޸�����      �汾��  �޸���      �޸�����
* ------------------------------------------------------------------------
* 2009/03/02    V1.0    ������      ����
**************************************************************************/
int XmlProcess_Return_Data(TYPE_XMLLINKSTATE *pXmLinkState,MSG_COMM_OAM * ptMsg)
{
    TYPE_LINK_STATE *pLinkStateTmp = NULL;
    CHAR *ptrOutputResultBuf = NULL;/*���Դ�������*/
    INT iBufLen = 0;                            /*���Դ���������С*/
    CHAR *ptrUTF8Buf = NULL;            /*UTF-8��ʽ�ַ���������*/
    CHAR *pFormatStr = NULL;
    int iRtnValue = DISPLAY_SUCCESS;

    /* ��ȡ���Ը�ʽ�� */
    pFormatStr = GetFormatByCmdAttrPos(pXmLinkState->tCurCMDInfo.wCmdAttrPos, pXmLinkState->tCurCMDInfo.cfgDat);    
    if (!pFormatStr)
    {
        return ERROR_INDIS;
    }
    pLinkStateTmp = &gtLinkState[MAX_CLI_VTY_NUMBER];
    Oam_InitOneLinkForTxtRestore(pLinkStateTmp);
    /* ���ݻ��Ը�ʽ�����ɻ����ַ��� */
    pLinkStateTmp->tMatchResult.cfgDat = pXmLinkState->tCurCMDInfo.cfgDat;
    iRtnValue = OamCfgintpSendToBuffer(pLinkStateTmp, ptMsg, pFormatStr,&pLinkStateTmp->tDispGlobalPara);
    /*����ȡ�Ļ��Դ�ת��ΪUTF-8��ʽ*/	
    ptrOutputResultBuf = pXmLinkState->tDispGlobalPara.pDisplayBuffer;
    if ('\0' == ptrOutputResultBuf[0])
    {
        return iRtnValue;
    }
    iBufLen = strlen(ptrOutputResultBuf);
    iBufLen *= 2;
    ptrUTF8Buf = (CHAR *)XOS_GetUB(iBufLen);
    if (NULL == ptrUTF8Buf)
    {
        return ERROR_INDIS;
    }
    memset(ptrUTF8Buf,0,iBufLen);
    if (!Gb2312ToUtf8(ptrOutputResultBuf, iBufLen,ptrUTF8Buf))
    {
        OAM_RETUB(ptrUTF8Buf);
	 return ERROR_INDIS;
    }
    memset(ptrOutputResultBuf,0,MAX_OUTPUT_RESULT);
    strncpy(ptrOutputResultBuf,ptrUTF8Buf,MAX_OUTPUT_RESULT);    
    /*�ͷŻ�����*/	
    OAM_RETUB(ptrUTF8Buf);    
    return iRtnValue;
}

/**************************************************************************
*                          ȫ�ֺ���ʵ��                                   *
**************************************************************************/

/*lint +e420*/
/*lint +e669*/
/*lint +e416*/



