/************************************************************************* 
    
    version 12a
    2002/5/10


    history:
    12a: 1. �����˸���ID��ʾ�ַ�������
    11d: 1. ��������־����
            2. ����oamros.h�еĲ������Ͷ���

    11c:    1. �������ṹ�Ѿ��ܹ������ֱ����ʾ��
               ��ԭ�еı�ģʽ��ȥ�����ֱ����ʾ����

    11b:    1. �������

    11a:    1. �ڱ���Ҳ����ʹ���жϣ������˱���ʾģʽ

    10a:    1. ȥ����pDisplayBufferCounter,����pDispBuffָ��ָ�򻺳�������һ��λ��
            2. ȥ����һ����ʾshort���͵�����bug
            3. ���е��ı�����TEXT_MARK������
            4. ������һЩ�����ж���ͳһ�澯����
            5. �����˴�д�Ľű��ַ���������ʾ�����ֽ��������,A U

    09g:    1. �ı���Ԥ��ӡ�Ļ���
            2. ӳ����ӡʱ��һ��2��ָ��ʹ�õĴ���
            3. ĳЩ��ʾ��ʽ���޸�
            4. ��ͷ��Ҳ����sbuffer��
            5. ͳһ�����ֱ���ʾ�ĸ�ʽ

    09f:    1. ��������

    09e:    1. �޸���һЩbug

    09d:    1. �����˶����ʾ֧��
            2. �����̿�£

    09c:    1. �ı���ӳ���Ķ�����޸��˲�����ʾ����
            2. ��ӳ���Ĳ������Ϊ4�ֽڵ�

    09b:    1. ��ͷһ���������һ��ParaNo����һ�����ĵ�һ��paraNo��ɶ����ʾ�Ľű�ѡ��
            2. �����˽ű��в�����õļ�����ͬ��bug
            3. ����0ֵΪ��֧�жϵ���Чֵ  (����δ���Ķ����������д˹���)
            4. �޸���λ�ò���ʡ��ʱ��һ��bug

    09a:    1. ����ѭ���ͷ�֧�жϵ�Ƕ��
            2. �޸�����Ӧ�Ľű�����
            3. ѭ���в�֧��break��continue��goto���

    08_a:   1. ����06e����ű�ѭ��
            2. ��%r��������һ��ð�ţ�������������
            3. ������ʱ�����ڵ���ʾ 
            4. ������64λ�޷���������ʾ

    07_a:   1. �������µı�����ĸ�ʽ��������Ϊ�ַ���ʱ��len����Ч��������'\0'������
            2. ���������ǰһ��Ϊ�Ҷ��룬��һ��Ϊ�����ʱ���м�ӿո�
            3. ��������ַ�����ʾ��ʽ�ĸĽ�

    06_h:   1. �Ľ���Խ�紦����ֻ֤Ҫ��Խ�磬���ܸ������Դ���
            2. ȥ���˶�pDisplayBufferCounter��һ����Ч����

    06_g:   1. �޸���ʾ�������ر���

    06e:    1. ��ʾͳһ����intpParaDisplay����
            2. �����˱������ 0*n �����

    06d:    1. ����ʾ�м����˺�����ʾ"%th"��ʾ

    06c:    1. ������06a��
            2. ����ű����õĹ���
               ����������ʵ���ַ�Ϊ3��:
               ��һ��:�ű����ò�
               �ڶ���:�ű����֧�����
               ������:�򵥽ű������
            3. �ڵȲ����ƶ����в�����ȱʡ����

    06b:    1. ��05d��ĳɣ�û�нű����ù���
            2. ��Ҫ��������Ժ��޸ĵ�display.c��ͳһ

    05c:    1. ���԰棬����ʾ���������ļ���
            2. �ı���һ�²��Խű�

    05b:    1. ���԰棬�޸��˲�������Ľű�
            2. �޸���bug

    05a:    1. ��ʽ֧�ֶ�ű�����֧������switch����

    04b:    1. ���ַ�����ʾǰ�������ж�,����ϵͳ����
            2. ȥ���˸�ʽ��ʾʱ������
            3. ȥ����һ��if����е�=����Ϊ==
            4. �����һ����ʾ����80��ʱ�������ʱ����ĩβ�ӿո�

    04a:    1. ȥ����һЩ���˻����Ĵ��������
            2. ���ִ�����Ż�
            3. ȫ����ע��


    03f:    ����ר������ǿ׳�Բ���
            1. ������һЩ����

    03e:    1. �ڱ�ͷ����֧��%%��

    03d:    1. �޸Ľӿ����ݽṹ��ȫ���msg.h
            2. �����˳���Ϊ2��4�����λ򸡵�����Ļ�������
    
    03c:    1. �޸��˱���ʾ����

    03b:    1. �����˲��ֱ����ʾ����

    03a:    1.add the table displayfuction. partial table display fuction will not be 
            supported
            2.use a new footscript code. the orignal symbol 'p' will not be used again
            all the symbol will follow the symbol used in printf function

            add %a for IPAddress, %b for envirment table value, %t for table formation
            %a & %b support flags,width and precision

            A format specification, which consists of optional and required fields,
            following the following form:
                %[flags] [width] [.precision] [{h | l | I64 | L}]type
            will be supported

            3.rename the critical files
            4.use a new msg headfile
            5.add alias and constant value display function
            6.add a debug function
            7.add assert

    01b:    fixed a bug in displayFlush()
    
*************************************************************************/
#include "includes.h"
#include "xmlagent.h"

/*lint -e420*/
/*lint -e571*/
/*lint -e716*/
/*lint -e722*/
/*lint -e797*/
/*lint -e662*/
/*lint -e1772*/
/*lint -e1895*/
/*lint -e749*/

/* state definitions */
/*lint -e749*/
enum STATE {
    ST_NORMAL,                      /* ����״̬; ��������ַ�   */
    ST_PERCENT                     /* �ն����� '%'             */
/*    ST_TABLE,                       �ն����˱�˵����       δ���õ�pclint������ɾ��  */
/*    ST_VALUE                        �ն����˲���λ��˵���� δ���õ�pclint������ɾ�� */
};
/*lint +e749*/

static ERROR_INFO sErrorTable[] =
{
    /* 0*/  {"", 1},
    /* 1*/  {"unknow error in display", 1},
    /* 2*/  {"insufficient memory", 1},
    /* 3*/  {"buffer overflow", 1},
    /* 4*/  {"invalid type", 1},
    /* 5*/  {"invalid field_numbers", 1},
    /* 6*/  {"invalid footscript", 1},
    /* 7*/  {"invalid string", 1},
    /* 8*/  {"wrong return value", 1},
    /* 9*/  {"footScript, return data mismatch" , 1},
    /*10*/  {"invalid pointer", 1},
    /*11*/  {"Invalid state", 1},
    /*12*/  {"invalid datalen", 1},
    /*13*/  {"", 1},
    /*14*/  {"", 1},
    /*15*/  {"rerun data length error", 1},
    /*16*/  {"", 1},
    /*17*/  {"", 1},
    /*18*/  {"", 1},
    /*19*/  {"", 1},
    /*20*/  {"", 1},
    /*21*/  {"", 1},
    /*22*/  {"", 1},
    /*23*/  {"", 1},
    /*24*/  {"", 1},
    /*25*/  {"", 1},
    /*26*/  {"", 1},
    /*27*/  {"", 1},
    /*28*/  {"", 1},
    /*29*/  {"", 1},
    /*30*/  {"", 1},
    /*31*/  {"", 1},
    /*32*/  {"", 1},
    /*33*/  {"", 1},
    /*34*/  {"", 1},
    /*35*/  {"", 1},
    /*36*/  {"", 1},
    /*37*/  {"", 1},
    /*38*/  {"", 1},
    /*39*/  {"", 1},
    /*40*/  {"", 1},
    /*41*/  {"", 1},
    /*42*/  {"", 1},
    /*43*/  {"", 1},
    /*44*/  {"", 1},
    /*45*/  {"", 1},
    /*46*/  {"", 1},
    /*47*/  {"", 1},
    /*48*/  {"", 1},
    /*49*/  {"", 1},
    /*50*/  {"", 1}
};

/*���������±�Ҫ��oam_cfg.h�ж��������һ��*/
/* datalen list array */
static WORD32 RETURN_DATALEN[] = 
{
    0,
    1,                              /*DATA_TYPE_BYTE*/
    2,                              /*DATA_TYPE_WORD*/
    4,                              /*DATA_TYPE_DWORD*/
    4,                              /*DATA_TYPE_INT*/
    1,                              /*DATA_TYPE_CHAR*/
    255,                            /*DATA_TYPE_STRING*/
    0,                              /*DATA_TYPE_TEXT*/
    MAX_TABLE_ITEM,                 /*DATA_TYPE_TABLE*/
    4,                              /*DATA_TYPE_IPADDR*/
    4,                              /*DATA_TYPE_DATE*/
    8,                              /*DATA_TYPE_TIME*/
    4,                              /*DATA_TYPE_MAP*/
    MAX_TABLE_ITEM,                 /*DATA_TYPE_LIST*/
    0,                              /*DATA_TYPE_CONST, �������޶���*/
    0,                              /*DATA_TYPE_IFPORT, �������޶���*/
    255,                            /*DATA_TYPE_HEX*/
    4,                              /*DATA_TYPE_MASK*/
    4,                              /*DATA_TYPE_IMASK*/
    6,                              /*DATA_TYPE_MACADDR*/
    4,                              /*DATA_TYPE_FLOAT*/
    4,                              /*DATA_TYPE_BROADTYPE*/
    20,                             /*DATA_TYPE_IPV6PREFIX*/
    16,                             /*DATA_TYPE_IPV6ADDR*/  
    6,                              /*DATA_TYPE_VPN_ASN*/
    6,                               /*DATA_TYPE_VPN_IP*/
    sizeof(T_PhysAddress),                              /*DATA_TYPE_PHYADDRESS*/
    sizeof(T_LogicalAddr),                                /*DATA_TYPE_LOGICADDRESS*/
    8,                              /*DATA_TYPE_WORD64*/
    4,                              /*DATA_TYPE_SWORD*/
    sizeof(T_LogicalAddrM),         /*DATA_TYPE_LOGIC_ADDR_M*/
    sizeof(T_LogicalAddrU),         /*DATA_TYPE_LOGIC_ADDR_U*/
    sizeof(T_IPVXADDR),             /*DATA_TYPE_IPVXADDR*/
    sizeof(T_Time),                 /*DATA_TYPE_T_TIME*/
    sizeof(T_SysSoftClock) ,         /*DATA_TYPE_SYSCLOCK*/
    4,                                         /*DATA_TYPE_VMMDATETIME*/
    4,                                         /*DATA_TYPE_VMMDATE*/
};

/************************* MACRO DEFINITION ***********************/
#define DISPLAY_BUFFER_COUNTER (pGlobal->pDispBuff - pGlobal->pDisplayBuffer)

/*�����������������͵�telnet��ʾ��*/
static void SendToTelnetIfBufIsFull(TYPE_LINK_STATE * pLinkState, WORD32 dwAppendLen)
{
    TYPE_DISP_GLOBAL_PARA *pGlobal = NULL;
    DWORD dwReserverSize = 0;	
    if (!pLinkState)
    {
        return;
    }

    pGlobal = &(pLinkState->tDispGlobalPara);
    /*��������еĵط������޷����㣬
    ����512�ֽ���Ϊ���壬����Խ��*/
    dwReserverSize = (DISPLAY_BUFFER_SIZE > 512) ? 512 : 0;
    if (((pGlobal->pDispBuff - pGlobal->pDisplayBuffer) + dwReserverSize + dwAppendLen) > DISPLAY_BUFFER_SIZE)
    {
        if (pLinkState == &gtLinkState[MAX_CLI_VTY_NUMBER])
        {
//            SendStringToOmm(Xml_GetCurUsedLinkState(), pLinkState->tDispGlobalPara.pDisplayBuffer, TRUE, FALSE);
        }
        else
        {
            pLinkState->bOutputResult = TRUE;
            SendPacketToTelnet(pLinkState, TRUE);
        }
        *(pGlobal->pDisplayBuffer) = MARK_STRINGEND;    /* ��ʼ����ʾ������ */
        memset(pGlobal->pDisplayBuffer, 0, MAX_OUTPUT_RESULT);
        pGlobal->pDispBuff = pGlobal->pDisplayBuffer;
        pLinkState->wSendPos = 0;
    }
}

/* get for counter or switch case value */
static WORD16 GetForCounterSwitchCase(OPR_DATA *pOprData)
{
    if(!pOprData)
        return 0;

    switch(pOprData->Type)
    {
    case DATA_TYPE_BYTE:
        return (*(pOprData->Data));
    case DATA_TYPE_WORD:
        {
            WORD16 value;
            memcpy(&value, (BYTE *)pOprData->Data, sizeof(WORD16));
            return value;
        }
    case DATA_TYPE_DWORD:
    case DATA_TYPE_INT:
        {
            WORD32 value;
            memcpy(&value, (BYTE *)pOprData->Data, sizeof(WORD32));
            return (WORD16)value;
        }
    case DATA_TYPE_FLOAT:
        {
            WORD32 value;
            memcpy(&value, (BYTE *)pOprData->Data, sizeof(WORD32));
            return (WORD16)value;
        }
    case DATA_TYPE_MAP:
        {
            WORD16 wMapValue;
            memcpy(&wMapValue, (BYTE *)pOprData->Data + sizeof(WORD16), sizeof(WORD16));
            return wMapValue;
        }
    default:
        break;
    }
    
    return 0;
}

static WORD16 GetTableSwitchCase(TYPE_DISP_GLOBAL_PARA_ITM *pGlobalPara, TABLE_DATA *pTableData, WORD32 ulParameterLocation)
{
    switch(pTableData->Item[ulParameterLocation - 1].Type)
    {
    case DATA_TYPE_BYTE:
        return (*pGlobalPara->pParameterStartAddress[ulParameterLocation]);
    case DATA_TYPE_WORD:
        {
            WORD16 value;
            memcpy(&value, pGlobalPara->pParameterStartAddress[ulParameterLocation], sizeof(WORD16));
            return value;
        }
    case DATA_TYPE_DWORD:
    case DATA_TYPE_INT:
        {
            WORD32 value;
            memcpy(&value, pGlobalPara->pParameterStartAddress[ulParameterLocation], sizeof(WORD32));
            return (WORD16)value;
        }
    case DATA_TYPE_FLOAT:
        {
            WORD32 value;
            memcpy(&value, pGlobalPara->pParameterStartAddress[ulParameterLocation], sizeof(WORD32));
            return (WORD16)value;
        }
    default:
        break;
    }
    
    return 0;
}

/*------------------------------------------------------------------------------
    �� �� ��:   intpGetTextString
    ��������:   ��ȡ��˫�������������ַ���
    ��    ��:
    ��    ��:   ��ȡ���ַ����ĳ���
    ��    ��:
    ���Դ���:

    ȫ�ֱ���:
    ע    ��:

==============================================================================
    �޸ļ�¼:
    �޸�����        �汾     �޸���      �޸�ԭ������
==============================================================================
    2000\12\29  1.0             ����
------------------------------------------------------------------------------*/
static void intpGetTextString(CHAR **SouString, CHAR *destiString)
{
    CHAR *sourceString;
    sourceString = *SouString;
    /* �����������ַ� */
    while (*sourceString != TEXT_MARK && *sourceString != MARK_STRINGEND)
        sourceString++;
    sourceString++;  /* ����TEXT_MARK �� MARK_STRINGEND */

    while (*sourceString != TEXT_MARK && *sourceString != MARK_STRINGEND)
        *destiString++ = *sourceString++;
    *destiString++ = MARK_STRINGEND;
    *SouString  = sourceString + 1;  /* ����TEXT_MARK �� MARK_STRINGEND */
}

/*------------------------------------------------------------------------------
    �� �� ��:   intpGetFormatString
    ��������:   ��ȡ�ַ����еĴ�ӡ��ʽ˵����
                ˵����Ϊ��abcdiouxefgnpy!msAU
    ��    ��:
    ��    ��:
    ��    ��:
    ���Դ���:

    ȫ�ֱ���:
    ע    ��:   �޴��󷵻أ����ֹ���ʱ������ֵ���ܲ���ȷ
                ָ��ԭ�ִ��Ķ���ָ�뽫���ƶ�

==============================================================================
    �޸ļ�¼:
    �޸�����        �汾     �޸���      �޸�ԭ������
==============================================================================
    2000\10\16  1.0             ����
------------------------------------------------------------------------------*/
static void intpGetFormatString(CHAR **SouString, CHAR **DetString)
{
    CHAR *sourceString, *destiString;
    sourceString = *SouString;
    destiString = *DetString;

    while (*sourceString != 'a' &&  /*DATA_TYPE_IPADDR*/
           *sourceString != 'b' &&      /*DATA_TYPE_MAP*/
           *sourceString != 'c' &&
           *sourceString != 'd' &&
           *sourceString != 'e' &&
           *sourceString != 'f' &&
           *sourceString != 'g' &&
           *sourceString != 'i' &&
           *sourceString != 'j' &&
           *sourceString != 'm' &&  /*DATA_TYPE_TIME*/
           *sourceString != 'n' &&
           *sourceString != 'o' &&
           *sourceString != 'p' &&
           *sourceString != 's' &&
           *sourceString != 'u' &&
           *sourceString != 'x' &&
           *sourceString != 'y' &&  /*DATA_TYPE_DATE */
           *sourceString != 'z' &&  /*DATA_TYPE_PHYADDRESS/DATA_TYPE_LOGICADDRESS*/
           *sourceString != '!' &&
           *sourceString != 'w' &&  /*DATA_TYPE_WORD64*/
           *sourceString != 'A' &&  /*DATA_TYPE_IPV6ADDR/DATA_TYPE_IPV6PREFIX*/
           *sourceString != 'B' &&  /*DATA_TYPE_LOGIC_ADDR_M*/
           *sourceString != 'C' &&  /*DATA_TYPE_LOGIC_ADDR_U*/
           *sourceString != 'D' &&  /*DATA_TYPE_IPVXADDR*/
           *sourceString != 'E' &&  /*DATA_TYPE_T_TIME*/
           *sourceString != 'F' &&  /*DATA_TYPE_SYSCLOCK*/
           *sourceString != 'G' &&  /*DATA_TYPE_VMMDATETIME*/
           *sourceString != 'H' &&  /*DATA_TYPE_VMMDATE*/
           *sourceString != 'U' &&
           *sourceString != 'Y' /*DATA_TYPE_DATE DATA_TYPE_TIME*/)
        {
             if (*sourceString == MARK_STRINGEND)
            {
                break;
            }
            *destiString++ = *sourceString++;
        }
    if (*sourceString != MARK_STRINGEND)
        *destiString++ = *sourceString++;
    *destiString++ = MARK_STRINGEND;
    *SouString  = sourceString;
    *DetString = destiString;
}

/*------------------------------------------------------------------------------
    �� �� ��:   intpGetScriptNumber
    ��������:   ��ȡ�ַ����е�ʮ�����޷��ų������������������ַ��˳�
    ��    ��:   
    ��    ��:   
    ��    ��:   
    ���Դ���:   
                
    ȫ�ֱ���:   
    ע    ��:   
                
==============================================================================
    �޸ļ�¼:
    �޸�����        �汾     �޸���      �޸�ԭ������
==============================================================================
    2000\10\16  1.0             ����
------------------------------------------------------------------------------*/
static WORD32 intpGetScriptNumber(CHAR **ppFootScript)
{
    CHAR * pFootScript;
    WORD32 lResultNumber;
    pFootScript = *ppFootScript;
    lResultNumber = 0;
    while (*pFootScript >= '0' && *pFootScript <= '9')
    {
        lResultNumber = lResultNumber * 10 + *pFootScript - '0';
        pFootScript++;
    }
    *ppFootScript = pFootScript;    /* move the pointer to the right place */
    return lResultNumber;
}

/*------------------------------------------------------------------------------
    �� �� ��:   intpGetStringByWidth
    ��������:   ԭ�ַ�����������ȡ����뷽ʽ����Ŀ���ַ���
    ��    ��:   CHAR *OrgString             Դ�ַ���
                CHAR *DetString             Ŀ���ַ���
                BYTE    Width       Ŀ���ַ����ĳ���
                INT     Style                  ���뷽ʽ��0,�Ҷ��룻-1,�����
    ��    ��:   ��
    ��    ��:   ��
                
    ȫ�ֱ���:   
    ע    ��:   Դ�ַ������ᱻ�ƻ�
                ���ᵼ��Խ��
==============================================================================
    �޸ļ�¼:
    �޸�����        �汾     �޸���      �޸�ԭ������
==============================================================================
    2000\10\16  1.0             ����
------------------------------------------------------------------------------*/
static void intpGetStringByWidth(CHAR  *OrgString,     /* ԭʼ�ַ��� */
                                 CHAR  *DetString,     /* Ŀ���ַ��� */
                                 WORD32 dwBufSize,/* Ŀ���ַ��� ����*/
                                 WORD32 Width,          /* Ŀ���ַ�����ʾ�ĳ��� */
                                 INT   Style           /* ���뷽ʽ */
                                 )
{
    static CHAR cTempString[MAX_DISPLAY_STRLEN];
    WORD32   ulOrgLenth;
    SWORD16 sCmp;
    
    ulOrgLenth = strlen(OrgString);
    sCmp = (WORD16)((WORD16)Width - (WORD16)ulOrgLenth);
    if (sCmp == 0)     /* ֱ�ӽ�ԭʼ�ַ���������Ŀ���ַ��� */
    {
        strncpy(DetString, OrgString, dwBufSize);
    }
    else if (sCmp > 0) /* ��Ҫ���ַ���ǰ�򴮺����ո� */
    {
        /* ��һ�����Ⱥ��ʵĿո��ַ��� */
        memset(cTempString, MARK_BLANK, (size_t)sCmp);
        cTempString[sCmp] = MARK_STRINGEND;

        if (Style == -1)
        {   /* ����룬�ں�ӿո� */
            strncpy(DetString, OrgString, dwBufSize);
            strncat(DetString, cTempString, dwBufSize);
        }
        else
        {   /* �Ҷ��룬���ַ���ǰ����ո� */
            strncpy(DetString, cTempString, dwBufSize);
            strncat(DetString, OrgString, dwBufSize);
        }

    }
    else               /* ��Ҫ�ض��ַ� */
    {
        if (Style == 0)
        {
            OrgString -= sCmp;
        }
        memcpy(DetString, OrgString, (size_t)Width);
        DetString[Width] = MARK_STRINGEND;
    }
}

/*------------------------------------------------------------------------------
    �� �� ��:   intpParaDisplay
    ��������:   �����򵥲�������ʾ
    ��    ��:   CHAR * StrBuff                  ��ʾ�����Ż�����
                INT32 lBuffLen                  �������ĳ���
                CHAR * StrFormat                ��ʾ��ʽ˵��
                BYTE * pParaBuffer              ��������ڵ�ַ
                WORD32 ulDataLen         �������ֽڳ���
                BYTE bType
    ��    ��:   ��ʾ���������������ַ���
    ��    ��:   ������ַ�����
                �������� -1 �ַ�������
                           -2 �������ֽڳ�����ű�˵��������
    ���Դ���:   ��ʾ�������ĳ��ȴ���
                
    ȫ�ֱ���:   
    ע    ��:
==============================================================================
    �޸ļ�¼:
    �޸�����        �汾     �޸���      �޸�ԭ������
==============================================================================
    2000\00\00  1.0             ����
------------------------------------------------------------------------------*/
static WORD32 intpParaDisplay(TYPE_LINK_STATE *pLinkState,
                             CHAR *StrBuff,
                             INT32 lBuffLen,
                             CHAR *StrFormat,
                             BYTE *pParaBuffer,
                             WORD32 ulDataLen
                             )
{
    static CHAR TempValueStr[MAX_DISPLAY_WIDTH], TempFormat[MAX_DISPLAY_WIDTH];
    INT32  lFormatLen, lTempLen, lDisplayLen = 0;
    CHAR cFormatChar, *pSValue;
    CHAR StrNoZero[80] = {0};

    memset(TempValueStr, 0, sizeof(TempValueStr));

    lFormatLen  = strlen(StrFormat);
    cFormatChar = *(StrFormat + lFormatLen - 1);
    StrBuff[0]  = MARK_STRINGEND;
    if (lBuffLen < MAX_DISPLAY_WIDTH && cFormatChar != 's')
    {
        DBG_ERR_LOG(BUFFER_OVERFLOW);
        return DISPLAY_SUCCESS;
    }
    switch (cFormatChar)
    {
    case 'c':
    case 'd':
    case 'i':
    case 'o':
    case 'u':
    case 'x':
    case 'n':
    case 'p':
        /* ��Ϊ�����������sprintf���� */
        /*������ʾ���˸��������ﰴ���Ƿ�%d�ֱ���yinhao20081204*/
        if (ulDataLen      == 1)
        {
            if (('d' == cFormatChar) || ('i' == cFormatChar))
            {                
                SBYTE value;
                memcpy(&value, pParaBuffer, (size_t)1);
                
                lDisplayLen = XOS_snprintf(StrBuff,lBuffLen,StrFormat,value);
            }
            else
            {
                BYTE value;
                memcpy(&value, pParaBuffer, (size_t)1);

                lDisplayLen = XOS_snprintf(StrBuff,lBuffLen,StrFormat,value);
            }
        }
        else if (ulDataLen == 2)
        {
            if (('d' == cFormatChar) || ('i' == cFormatChar))
            {
                SWORD16 value;
                memcpy(&value, pParaBuffer, (size_t)2);
            
                lDisplayLen = XOS_snprintf(StrBuff,lBuffLen,StrFormat,value);            
            }
            else
            {
                WORD16 value;
                memcpy(&value, pParaBuffer, (size_t)2);
            
                lDisplayLen = XOS_snprintf(StrBuff,lBuffLen,StrFormat,value);
            }
        }
        else if (ulDataLen == 4)
        {
            if(('d' == cFormatChar) || ('i' == cFormatChar))
            {
                SWORD32 value;
                memcpy(&value, pParaBuffer, (size_t)4);
            
                lDisplayLen = XOS_snprintf(StrBuff,lBuffLen,StrFormat,value);
            }
            else
            {
                WORD32 value;
                memcpy(&value, pParaBuffer, (size_t)4);
            
                lDisplayLen = XOS_snprintf(StrBuff,lBuffLen,StrFormat,value);
            }
        }
        else if (ulDataLen == 8)
        {/*64λ�з�������*/
            WORD32 v1,v2;
            WORD16 wOrderTest = 0x1234;
	     CHAR sResult[40] = {0};
		 
            if (0x34 == *(BYTE*)&wOrderTest)
            {
                memcpy(&v1, pParaBuffer, (size_t)4);
                memcpy(&v2, pParaBuffer+4, (size_t)4);
            }
            else
            {
                /*��β*/
                memcpy(&v2, pParaBuffer, (size_t)4);
                memcpy(&v1, pParaBuffer+4, (size_t)4);	
            }

            if ((v2 & 0x80000000) == 0x0)
            {
                int2Str_Of64bit(v2, v1, TempValueStr);
                getWord64Str(TempValueStr, StrNoZero);  
                XOS_snprintf(sResult, sizeof(sResult), "%s", StrNoZero);
            }
            else
            {
                v2 = ~v2;
                if (v1 == 0x0)
                {
                    v2 += 1;
                }
                v1 = ~v1 + 1;
        
                int2Str_Of64bit(v2, v1, TempValueStr);
                getWord64Str(TempValueStr, StrNoZero);  
                XOS_snprintf(sResult, sizeof(sResult), "-%s", StrNoZero);
            }

            *(StrFormat + lFormatLen -1) = 's'; /* change the format specification to  %...s */
            lDisplayLen = XOS_snprintf(StrBuff, lBuffLen, StrFormat, sResult);
            *(StrFormat + lFormatLen -1) = 'i';
        }
        else if (ulDataLen == 20)
        {
            int ii=0;
            PREFIX_IPV6 *ipv6prefix = (PREFIX_IPV6 *)pParaBuffer;
            for(ii=0;ii<8;ii++)
            {
                lDisplayLen += XOS_snprintf(StrBuff+lDisplayLen,lBuffLen - lDisplayLen,StrFormat,XOS_InvertWord16(ipv6prefix->prefix.__u6_addr.__u6_addr16[ii]));
                if (ii==7)      
                    lDisplayLen += XOS_snprintf(StrBuff+lDisplayLen, lBuffLen - lDisplayLen,"%c",'/');
                else
                    lDisplayLen += XOS_snprintf(StrBuff+lDisplayLen,lBuffLen - lDisplayLen,"%c",':');
            }
            lDisplayLen += XOS_snprintf(StrBuff+lDisplayLen,lBuffLen - lDisplayLen,"%u",ipv6prefix->prefixlen);
        }
        else if (ulDataLen == 16)
        {
            int ii=0;
            IN6_ADDR *ipv6addr = (IN6_ADDR *)pParaBuffer;
            for(ii=0;ii<8;ii++)
            {
                lDisplayLen += XOS_snprintf(StrBuff+lDisplayLen,lBuffLen - lDisplayLen,StrFormat,XOS_InvertWord16(ipv6addr->__u6_addr.__u6_addr16[ii]));
                if (ii == 7)
                    lDisplayLen += XOS_snprintf(StrBuff+lDisplayLen,lBuffLen - lDisplayLen,"%c",' ');
                else
                    lDisplayLen += XOS_snprintf(StrBuff+lDisplayLen,lBuffLen - lDisplayLen,"%c",':');
            }
        }
        else
        {
            DBG_ERR_LOG(DATA_LENGTH_ERROR);
            return 0;
        }
        break;
    /*Ӧ�÷��ص��ǽ�����Ϣ*/
    case 'j':
        {         
            WORD16 wValue;
			
	     if (ulDataLen == 1)
     	     {
                BYTE ucTmp;
                memcpy(&ucTmp, pParaBuffer, (size_t)1);
                wValue = ucTmp;
            }
            else
     	     {
                memcpy(&wValue, pParaBuffer, (size_t)2);				
            }
		 			
            *(StrFormat + lFormatLen -1) = 'u'; /* change the format specification to  %...u */
            lDisplayLen = XOS_snprintf(StrBuff,lBuffLen,StrFormat, wValue);
            *(StrFormat + lFormatLen -1) = 'j';

            /* ��������Ϣ���͵�telnet��ʾ */
            pLinkState->bOutputMode = OUTPUT_MODE_ROLL;
            pLinkState->wAppMsgReturnCode = SUCC_WAIT_AND_HAVEPARA;
         //   strncat(pLinkState->sOutputResult, StrBuff, MAX_OUTPUT_RESULT);

            /*���������100������һ�����з�*/
            if (wValue == 100)
            {
                strncat(pLinkState->sOutputResult, "\n\r", MAX_STRING_LEN);
            }
	
            SendStringToTelnet(pLinkState, pLinkState->sOutputResult);
            pLinkState->bOutputMode = OUTPUT_MODE_NORMAL;
         /*   SendToTelnetIfBufIsFull(pLinkState, 1);*/

            return lDisplayLen;
        }
        break;
    case 'U':
        /* ��Ϊ�����������sprintf���� */
        if (ulDataLen == 1)
        {
            BYTE value;
            memcpy(&value, pParaBuffer, (size_t)1);
            
            lDisplayLen = XOS_snprintf(StrBuff,lBuffLen,StrFormat,value);
        }
        else if (ulDataLen == 2)
        {
            WORD16 value;
            
            /*CRDCM00313101 ��XOS_snprintf�滻strcpy��strcat����*/
            XOS_snprintf(TempFormat,
                         sizeof(TempFormat) -1,
                         "%s",
                         StrFormat);

            TempFormat[lFormatLen - 1] = 'h';
            TempFormat[lFormatLen] = (CHAR)(cFormatChar + 32);
            TempFormat[lFormatLen + 1] = MARK_STRINGEND;
            TempValueStr[1] = *pParaBuffer++;
            TempValueStr[0] = *pParaBuffer;
            memcpy(&value, TempValueStr, (size_t)2);
            lDisplayLen = XOS_snprintf(StrBuff,lBuffLen,TempFormat,value);
        }
        else if (ulDataLen == 4)
        {
            INT32 value;
            
            TempValueStr[3] = *pParaBuffer++;
            TempValueStr[2] = *pParaBuffer++;
            TempValueStr[1] = *pParaBuffer++;
            TempValueStr[0] = *pParaBuffer;
            memcpy(&value, TempValueStr, (size_t)4);
            StrFormat[lFormatLen - 1] = (CHAR)(cFormatChar + 32);
            lDisplayLen = XOS_snprintf(StrBuff,lBuffLen,StrFormat,value);
            StrFormat[lFormatLen - 1] = cFormatChar;
        }
        else
        {
            DBG_ERR_LOG(15);
            return 0;
        }
        break;
    case 'e':
    case 'g':
    case 'f':
        DISPLAY_ASSERT(ulDataLen == 4);
        {
            float value;
            memcpy(&value, pParaBuffer, (size_t)4);
            lDisplayLen = XOS_snprintf(StrBuff,lBuffLen,StrFormat,value);
            /*ȥ��С��������õ�'0'*/
            DeleteNouseZero(StrBuff);
            lDisplayLen = strlen(StrBuff);
        }
        break;

    case 's':
        /* �ַ������ȵĺϷ����ж� */
        /* ���ڱ������ԭ�򣬲���ֱ����LenthTemp == ulDataLen�ж���ȷ�� */
        lTempLen = strlen((CHAR *)pParaBuffer);
        if (lTempLen > lBuffLen
               || lTempLen < 0 
          /*     || (WORD32)lTempLen > (ulDataLen - 1)*/)
        {
            DBG_ERR_LOG(BUFFER_OVERFLOW);
            return DISPLAY_SUCCESS;
        }
        if (lTempLen > 0)
        {
            lDisplayLen = XOS_snprintf(StrBuff,lBuffLen,StrFormat,pParaBuffer);
        }
        break;
    case 'z':
        /*�����ַ���߼���ַ*/
        if (ulDataLen == sizeof(T_PhysAddress))
        {
            T_PhysAddress *ptAddr = (T_PhysAddress *)pParaBuffer;
            XOS_snprintf(TempValueStr, sizeof(TempValueStr),"%d-%d-%d-%d", 
                                                      ptAddr->ucRackId, ptAddr->ucShelfId, ptAddr->ucSlotId, ptAddr->ucCpuId);
        }
        else if (ulDataLen == sizeof(T_LogicalAddr))
        {
            T_LogicalAddr *ptAddr = (T_LogicalAddr *)pParaBuffer;
            XOS_snprintf(TempValueStr, sizeof(TempValueStr), "%d-%d-%d-%d-%d-%d", 
                ptAddr->wSystem,ptAddr->ucSubSystem,ptAddr->wModule,ptAddr->wUnit,ptAddr->ucSUnit,ptAddr->wIndex);
        }
        else
        {
            DBG_ERR_LOG(DATA_LENGTH_ERROR);
            return 0;
        }
        
        *(StrFormat + lFormatLen -1) = 's'; 
        lDisplayLen  = XOS_snprintf(StrBuff,lBuffLen,StrFormat, TempValueStr);
        *(StrFormat + lFormatLen -1) = 'z';
        break;
    case 'a':
        /* edit by wangmh  for byteorder handler 2003-04-08
        --keep the order
        */
   
        /*611000763178 ip��ַ�Ĵ������ֽ�������*/
        #if 0
        pParaBuffer += 3; 
        XOS_snprintf(TempValueStr, "%d.", *(BYTE *)(pParaBuffer--));
        XOS_snprintf(TempValueStr, "%s%d.", TempValueStr, \
                                            *(BYTE *)(pParaBuffer--));
        XOS_snprintf(TempValueStr, "%s%d.", TempValueStr, \
                                            *(BYTE *)(pParaBuffer--));
        XOS_snprintf(TempValueStr, "%s%d", TempValueStr,  \
                                            *(BYTE *)(pParaBuffer--));
	  #endif
	  {
	      WORD32 dwIpTmp = *((WORD32*)pParaBuffer);
	      /*�յ���ip��ַ�������������*/
	      XOS_snprintf(TempValueStr, sizeof(TempValueStr),"%u.%u.%u.%u",
	                          (dwIpTmp & 0xFF000000) >> 24,
	                          (dwIpTmp & 0x00FF0000) >> 16,
	                          (dwIpTmp & 0x0000FF00) >> 8,
	                          (dwIpTmp & 0x000000FF) 
	                    
	      );
         }

        /* pParameter has been moved to the right position */

        *(StrFormat + lFormatLen -1) = 's'; /* change the format specification to
                                            %...s */
        lDisplayLen = XOS_snprintf(StrBuff,lBuffLen,StrFormat, TempValueStr);
        *(StrFormat + lFormatLen -1) = 'a';
        break;
    case 'A':
        /* A ��ΪIPV6 ��ַ��ʾ modified by yinhao
        pParaBuffer += 3; 
       */
        
        XOS_snprintf(TempValueStr, sizeof(TempValueStr),"%X:", XOS_InvertWord16(*(WORD16 *)(pParaBuffer)));
        XOS_snprintf(TempValueStr, sizeof(TempValueStr),"%s%X:",TempValueStr, XOS_InvertWord16(*(WORD16 *)(pParaBuffer+2)));
        XOS_snprintf(TempValueStr, sizeof(TempValueStr),"%s%X:",TempValueStr, XOS_InvertWord16(*(WORD16 *)(pParaBuffer+4)));
        XOS_snprintf(TempValueStr, sizeof(TempValueStr),"%s%X:",TempValueStr, XOS_InvertWord16(*(WORD16 *)(pParaBuffer+6)));
        XOS_snprintf(TempValueStr, sizeof(TempValueStr),"%s%X:",TempValueStr, XOS_InvertWord16(*(WORD16 *)(pParaBuffer+8)));
        XOS_snprintf(TempValueStr, sizeof(TempValueStr),"%s%X:",TempValueStr, XOS_InvertWord16(*(WORD16 *)(pParaBuffer+10)));
        XOS_snprintf(TempValueStr, sizeof(TempValueStr),"%s%X:",TempValueStr, XOS_InvertWord16(*(WORD16 *)(pParaBuffer+12)));
        XOS_snprintf(TempValueStr, sizeof(TempValueStr),"%s%X",TempValueStr, XOS_InvertWord16(*(WORD16 *)(pParaBuffer+14)));

        /* edit by wangmh  for byteorder handler 2003-04-08
        */
        *(StrFormat + lFormatLen -1) = 's'; /* change the format specification to
                                            %...s */
        lDisplayLen = XOS_snprintf(StrBuff,lBuffLen,StrFormat, TempValueStr);
        *(StrFormat + lFormatLen -1) = 'A';
        break;
    case 'b':
        /* constant parameter display */
        {
            WORD16 wType, wValue;
            memcpy(&wType, pParaBuffer, sizeof(WORD16));
            memcpy(&wValue, (BYTE *)pParaBuffer + sizeof(WORD16), sizeof(WORD16));
            pSValue = OamCfgConvertWordToString(pLinkState->tMatchResult.cfgDat,wType, wValue);
        }
        DISPLAY_ASSERT(pSValue != NULL);
        *(StrFormat + lFormatLen -1) = 's'; /* change the format specification to
                                            %...s */
        lDisplayLen = XOS_snprintf(StrBuff,lBuffLen,StrFormat, pSValue);
        *(StrFormat + lFormatLen -1) = 'b';
        break;
    case 'Y':       /* ����ʱ�����ʾ */
        {
            WORD32 dwDateTime;
            memcpy(&dwDateTime, pParaBuffer, sizeof(WORD32));
            DateTimeToStringAmericanStyle(&dwDateTime, TempValueStr, sizeof(TempValueStr));
            lDisplayLen = XOS_snprintf(StrBuff,lBuffLen,"%s", TempValueStr);
        }
        break;
    case 'y':       /* ���ڵ���ʾ */
        {
            WORD32 dwDate;
            memcpy(&dwDate, pParaBuffer, sizeof(WORD32));
            DateToStringAmericanStyle(&dwDate, TempValueStr, sizeof(TempValueStr));
            lDisplayLen = XOS_snprintf(StrBuff,lBuffLen,"%s", TempValueStr);
        }
        break;
    case 'm':       /* ʱ�����ʾ */
        {
            WORD32 dwTime;
            memcpy(&dwTime, pParaBuffer, sizeof(WORD32));
            OamCfgTimeToString(&dwTime, TempValueStr,sizeof(TempValueStr));
            lDisplayLen = XOS_snprintf(StrBuff,lBuffLen,"%s", TempValueStr);
        }
        break;
    case 'w':       /* 64λ�޷�����������ʾ */
        {
            WORD32 v1,v2;
           /* memcpy(&v1, pParaBuffer, (size_t)4);
            memcpy(&v2, pParaBuffer+4, (size_t)4);
            */
            WORD16 wOrderTest = 0x1234;
            if (0x34 == *(BYTE*)&wOrderTest)
            {
            memcpy(&v1, pParaBuffer, (size_t)4);
            memcpy(&v2, pParaBuffer+4, (size_t)4);
        }
            else
            {
                /*��β*/
                memcpy(&v2, pParaBuffer, (size_t)4);
                memcpy(&v1, pParaBuffer+4, (size_t)4);	
            }
			
            /*OamCfgintpGetStringFrom64bit(v1, v2, TempValueStr);*/
            int2Str_Of64bit(v2, v1, TempValueStr);
            getWord64Str(TempValueStr, StrNoZero);
        }
        *(StrFormat + lFormatLen -1) = 's'; /* change the format specification to
                                            %...s */
        lDisplayLen = XOS_snprintf(StrBuff, lBuffLen, StrFormat, StrNoZero);
        *(StrFormat + lFormatLen -1) = 'w';
        break;
    case 'B':      /*LogicalAddrM*/
        {
            WORD16 wModule;
            memcpy(&wModule, pParaBuffer, sizeof(WORD16));
            lDisplayLen = XOS_snprintf(StrBuff,lBuffLen,StrFormat,wModule);
            *(StrFormat + lFormatLen -1) = 'B';
        }
        break;
    case 'C':      /*LogicalAddrU*/
        {
            T_LogicalAddrU *pLogicAddrU = (T_LogicalAddrU *)pParaBuffer;
            BYTE ucSubSystem = pLogicAddrU->ucSubSystem;
            WORD16 wUnit = pLogicAddrU->wUnit;
            WORD16 wSUnit = pLogicAddrU->ucSUnit;

            XOS_snprintf(TempValueStr, sizeof(TempValueStr),"%d-%d-%d", ucSubSystem, wUnit, wSUnit);
            pParaBuffer += 4;
        }
        *(StrFormat + lFormatLen -1) = 's'; /* change the format specification to
                                            %...s */
        lDisplayLen = XOS_snprintf(StrBuff,lBuffLen,StrFormat, TempValueStr);
        *(StrFormat + lFormatLen -1) = 'C';
        break;
    case 'D':     /*T_IPVXADDR*/
        {
            T_IPVXADDR *pIpVxAddr = (T_IPVXADDR *)pParaBuffer;
            XOS_snprintf(TempValueStr, sizeof(TempValueStr),"%d-%d-", pIpVxAddr->ucIpType, pIpVxAddr->wVpnId);
            pParaBuffer += 4; 
            if (pIpVxAddr->ucIpType == IPADDR_TYPE_IPV4)
            {
                /*pParaBuffer += 3; */
                /*1.2.3.4 ���ڴ�����Ӹߵ����� 4 3 2 1 */
                
                XOS_snprintf(TempValueStr, sizeof(TempValueStr),"%s%d.", TempValueStr, *(BYTE *)(pParaBuffer++));
                XOS_snprintf(TempValueStr, sizeof(TempValueStr),"%s%d.", TempValueStr, \
                                                    *(BYTE *)(pParaBuffer++));
                XOS_snprintf(TempValueStr, sizeof(TempValueStr),"%s%d.", TempValueStr, \
                                                    *(BYTE *)(pParaBuffer++));
                XOS_snprintf(TempValueStr, sizeof(TempValueStr),"%s%d", TempValueStr,  \
                                                    *(BYTE *)(pParaBuffer++));
                
                /* pParameter has been moved to the right position */

                *(StrFormat + lFormatLen -1) = 's'; /* change the format specification to
                                            %...s */
                lDisplayLen = XOS_snprintf(StrBuff, lBuffLen, StrFormat, TempValueStr);
                *(StrFormat + lFormatLen -1) = 'D';
            }
            else if (pIpVxAddr->ucIpType == IPADDR_TYPE_IPV6)
            {
                XOS_snprintf(TempValueStr, sizeof(TempValueStr),"%s%X:",TempValueStr, XOS_InvertWord16(*(WORD16 *)(pParaBuffer)));
                XOS_snprintf(TempValueStr, sizeof(TempValueStr),"%s%X:",TempValueStr, XOS_InvertWord16(*(WORD16 *)(pParaBuffer+2)));
                XOS_snprintf(TempValueStr, sizeof(TempValueStr),"%s%X:",TempValueStr, XOS_InvertWord16(*(WORD16 *)(pParaBuffer+4)));
                XOS_snprintf(TempValueStr, sizeof(TempValueStr),"%s%X:",TempValueStr, XOS_InvertWord16(*(WORD16 *)(pParaBuffer+6)));
                XOS_snprintf(TempValueStr, sizeof(TempValueStr),"%s%X:",TempValueStr, XOS_InvertWord16(*(WORD16 *)(pParaBuffer+8)));
                XOS_snprintf(TempValueStr, sizeof(TempValueStr),"%s%X:",TempValueStr, XOS_InvertWord16(*(WORD16 *)(pParaBuffer+10)));
                XOS_snprintf(TempValueStr, sizeof(TempValueStr),"%s%X:",TempValueStr, XOS_InvertWord16(*(WORD16 *)(pParaBuffer+12)));
                XOS_snprintf(TempValueStr, sizeof(TempValueStr),"%s%X",TempValueStr, XOS_InvertWord16(*(WORD16 *)(pParaBuffer+14)));

                *(StrFormat + lFormatLen -1) = 's'; /* change the format specification to
                                                    %...s */
                lDisplayLen = XOS_snprintf(StrBuff,lBuffLen,StrFormat, TempValueStr);
                *(StrFormat + lFormatLen -1) = 'D';
            }
            else
            {
                DBG_ERR_LOG(INVALID_TYPE)
            }
        }
        break;
    case 'E':   /*T_Time*/
        {
            T_Time *pTime = (T_Time *)pParaBuffer;
            WORD32 dwDateTime = pTime->dwSecond;

            DateTimeToStringAmericanStyle(&dwDateTime, TempValueStr, sizeof(TempValueStr));
            XOS_snprintf(TempValueStr, sizeof(TempValueStr),"%s:%d", TempValueStr, pTime->wMilliSec);
        }
        *(StrFormat + lFormatLen -1) = 's';
        lDisplayLen = XOS_snprintf(StrBuff,lBuffLen,StrFormat, TempValueStr);
        *(StrFormat + lFormatLen -1) = 'E';
        break;
    case 'F':   /*T_SysSoftClock*/
        {
            T_SysSoftClock *pSysClock = (T_SysSoftClock *)pParaBuffer;

            XOS_snprintf(TempValueStr, sizeof(TempValueStr),"%d-%d-%d %d:%d:%d:%d", pSysClock->wSysYear, pSysClock->ucSysMon, pSysClock->ucSysDay,
                                                              pSysClock->ucSysHour, pSysClock->ucSysMin, pSysClock->ucSysSec,pSysClock->wMilliSec);
        }
        *(StrFormat + lFormatLen -1) = 's';
        lDisplayLen = XOS_snprintf(StrBuff,lBuffLen,StrFormat, TempValueStr);
        *(StrFormat + lFormatLen -1) = 'F';
        break;
    case 'G':   /*DATA_TYPE_VMMDATETIME*/
        {
            /*
               �汾����ʱ�䣺
                  |31-20|19-16|15-11|10-6|5-0|
                       ��     ��   ��    ʱ   ��
                ���磺0x7DA7B2DD Ϊ2010��7��22��11ʱ29��
           */
            WORD32 wDateTime;
            memcpy(&wDateTime, pParaBuffer, sizeof(wDateTime));
            WORD16 wYear  = (wDateTime & 0xFFF00000) >> 20;
            WORD16 wMonth = (wDateTime & 0x000F0000) >> 16;
            WORD16 wDay   = (wDateTime & 0x0000F800) >> 11;
            WORD16 wHour  = (wDateTime & 0x000007C0) >> 6;
            WORD16 wMinute= (wDateTime & 0x0000003F);
            XOS_snprintf(TempValueStr, sizeof(TempValueStr),"%04u-%02u-%02u %02u:%02u", wYear, wMonth, wDay, wHour,wMinute);
        }
        *(StrFormat + lFormatLen -1) = 's';
        lDisplayLen = XOS_snprintf(StrBuff,lBuffLen,StrFormat, TempValueStr);
        *(StrFormat + lFormatLen -1) = 'G';
        break;
    case 'H':   /*DATA_TYPE_VMMDATE*/
        {
            /*
                �汾�������ڣ�
               0x AABB CC DD
                   ��   �� ��
               ���磺0x07DA040D  Ϊ2010��4��13��
           */
            WORD32 wDateTime;
            memcpy(&wDateTime, pParaBuffer, sizeof(wDateTime));
            WORD16 wYear  = (wDateTime & 0xFFFF0000) >> 16;
            WORD16 wMonth = (wDateTime & 0x0000FF00) >> 8;
            WORD16 wDay   = (wDateTime & 0x000000FF);
            XOS_snprintf(TempValueStr, sizeof(TempValueStr),"%04u-%02u-%02u", wYear, wMonth, wDay);
        }
        *(StrFormat + lFormatLen -1) = 's';
        lDisplayLen = XOS_snprintf(StrBuff,lBuffLen,StrFormat, TempValueStr);
        *(StrFormat + lFormatLen -1) = 'H';
        break;
    case '!':
        XOS_snprintf(TempValueStr, sizeof(TempValueStr),"%02x-%02x-%02x-%02x-%02x-%02x", 
                pParaBuffer[0], pParaBuffer[1], pParaBuffer[2], 
                pParaBuffer[3], pParaBuffer[4], pParaBuffer[5]);
        pParaBuffer += 6;

        *(StrFormat + lFormatLen -1) = 's'; /* change the format specification to
                                            %...s */
        lDisplayLen = XOS_snprintf(StrBuff,lBuffLen,StrFormat, TempValueStr);
        *(StrFormat + lFormatLen -1) = '!';
        break;
    default:
        DBG_ERR_LOG(UNKNOW_ERROR_IN_DISPLAY)
        return DISPLAY_SUCCESS;
    }
    return lDisplayLen;
}

/*------------------------------------------------------------------------------
    �� �� ��:   intpSendToBuff_Normal_TableMode

    ��������:   tableģʽ�������ű�������ӳ������ڱ��У�����ȥ���˱��������ʾ
                û�о�̬�ı��ĸ�ʽ����
    ��    ��:   pGlobal
    ��    ��:   ȫ���Ե���ʾ���������������ַ���
    ��    ��:   �������
    ȫ�ֱ���:   
    ע    ��:
==============================================================================
    �޸ļ�¼:
    �޸�����        �汾     �޸���      �޸�ԭ������
==============================================================================
    2001\1\18  1.0             ����
------------------------------------------------------------------------------*/
static INT32 intpSendToBuff_Normal_TableMode(TYPE_LINK_STATE *pLinkState,TYPE_DISP_GLOBAL_PARA_ITM *pGlobal)
                                             /* ������ڵ�ַ���Ѿ���Ϊȫ�� */
{
    CHAR *pParaBuffer;  /* ָ�������ָ�� */
    CHAR *pFootScript;  /* ָ��ű���ǰ��λ��ָ�� */
    enum  STATE state;  /* ��ǰ״̬ */

    WORD32 ulDataLen;
    BYTE bType;

    /* ��ʱ�ַ�������ʱֻ���ڸ�ʽ˵�����Ĵ����� */
    static CHAR TempSpecifierString[MAX_DISPLAY_WIDTH];
    CHAR *pTempSpecifierString;

    WORD32 ulParameterLocation; /* ����λ�� */

    pFootScript = pGlobal->pFootScript;

    /* start automaton */
    state = ST_NORMAL;
    while (*pFootScript != MARK_STRINGEND)
    {
        switch (state)
        {
        case ST_NORMAL:
            if (*pFootScript != '%')
            {
                SendToTelnetIfBufIsFull(pLinkState, 1);
                *pGlobal->pDispBuff++ = *pFootScript++;
            }
            else 
            {
                state = ST_PERCENT;
                pFootScript++;
            }
            break;
        case ST_PERCENT:
            switch (*pFootScript)
            {
            case 'h': /* %h */
                pFootScript++;
                pGlobal->pFootScript = pFootScript;
                return SUCCESS_ENDWITHSWITCH;  /* ����%h���� */
            case 'r': /* %r */
                pFootScript++;
                pGlobal->pFootScript = pFootScript;
                return SUCCESS_ENDWITHFOR;     /* ����%r���� */
            case 't': /* %t */
                pFootScript++;
                pGlobal->pFootScript = pFootScript;
                return SUCCESS_ENDWITHTABLE;   /* ����%t���� */
            case '%': /* %% */
                SendToTelnetIfBufIsFull(pLinkState, 1);
                *pGlobal->pDispBuff++ = *pFootScript++;
                state = ST_NORMAL;
                break;
            case 'v':
                /* ��ȡ��λ�� */
                pFootScript++;
                ulParameterLocation = intpGetScriptNumber(&pFootScript);
                if (ulParameterLocation == 0)
                    ulParameterLocation = pGlobal->giParameterLocation++;
                else
                    pGlobal->giParameterLocation = ulParameterLocation + 1;

                /* λ�úϷ� */
                if (ulParameterLocation > pGlobal->TableData.ItemNumber)
                {
                    DBG_ERR_LOG(FOOTSCRIPT_DATA_MISMATCH);
                    return DISPLAY_SUCCESS;
                }

                pParaBuffer = pGlobal->pParameterStartAddress[ulParameterLocation];
                pFootScript++;      

                /* �������� */
                bType = pGlobal->TableData.Item[ulParameterLocation - 1].Type;
                /* ��ȡ��ʽ˵����������ʾ */
                /* look for the specification, copy the whole format specification 
                   to TempSpecifierString */
                state = ST_NORMAL;
                pTempSpecifierString = &TempSpecifierString[0];
                *pTempSpecifierString++ = '%';
                intpGetFormatString(&pFootScript, &pTempSpecifierString);

                ulDataLen = pGlobal->TableData.Item[ulParameterLocation - 1].Len;
                SendToTelnetIfBufIsFull(pLinkState, 1);
                pGlobal->pDispBuff += intpParaDisplay(pLinkState,
                                                      pGlobal->pDispBuff,
                                                      (INT32)(DISPLAY_BUFFER_SIZE - DISPLAY_BUFFER_COUNTER),
                                                      TempSpecifierString,
                                                      (BYTE *)pParaBuffer,
                                                      (WORD32)ulDataLen);
                pParaBuffer += ulDataLen;
                SendToTelnetIfBufIsFull(pLinkState, 1);
                break; /* end of case 'v' */
            default:
                DBG_ERR_LOG(INVALID_FOOTSCRIPT);
                return ERROR_INDIS;
            }
            break;
        default:
            DBG_ERR_LOG(INVALID_STATE);
            return ERROR_INDIS;
        }  /* end of switch (state) */
                        
    }  /* end of  while (*pFootScript != MARK_STRINGEND) */
    pGlobal->pFootScript = pFootScript;
    return SUCCESS_END;
}


/*------------------------------------------------------------------------------
    �� �� ��:   intpSendToBuffer_TableMode
    ��������:   tableģʽ�½ű�������ӳ����ڴ�ģʽ��ֻ��Ҫ�����֧�жϵ����
                ��֧��ѭ����
                ֱ��ʹ������ģʽ�µĳ���Σ�Ϊ���ڵ�ÿ������������ڵ�ַ��
    ��    ��:   
    ��    ��:   ȫ���Ե���ʾ���������������ַ���
    ��    ��:   �������
    ȫ�ֱ���:   
    ע    ��:
==============================================================================
    �޸ļ�¼:
    �޸�����        �汾     �޸���      �޸�ԭ������
==============================================================================
    2001\1\18  1.0             ����
------------------------------------------------------------------------------*/
static INT32 intpSendToBuffer_TableMode(TYPE_LINK_STATE *pLinkState,
                                        TYPE_DISP_GLOBAL_PARA  *pGlobal, /* ��������ȫ�ֱ�����ָ�� */
                                        WORD32 iParaLocat                 /* �������λ�� */
                                        )
{
    static TYPE_DISP_GLOBAL_PARA_ITM global_InTableMode;
    CHAR *pFootScript;
    CHAR *pReservedScript;
    CHAR *pParaBufferTemp;
    BYTE ucTableField, ucTableItem;
    WORD32 iTemp;
    WORD16 wSwitchCounter;                    /* ��switch��Ƕ�׽��м��� */
    POPR_DATA pTempData;
    PTABLE_DATA pTempTableData;

    global_InTableMode.giParameterLocation = 1;
    global_InTableMode.pDispBuff = pGlobal->pDispBuff;
    global_InTableMode.pDisplayBuffer = pGlobal->pDisplayBuffer;
    global_InTableMode.bSwitchStackTop = 0;
    pFootScript = pGlobal->pFootScript;
    pReservedScript = pFootScript;

    /* ȡ�øñ�Ĳ��� */
    pTempData = (POPR_DATA)pGlobal->pParameterStartAddress[iParaLocat];
    ucTableItem = pTempData->Len;
    pTempTableData = (PTABLE_DATA)&(pTempData->Data[0]);
    ucTableField = pTempTableData->ItemNumber;
    /* ��ȡ�в������ͼ����� */
    memcpy(&global_InTableMode.TableData,
            pTempTableData, sizeof(TABLE_DATA));
    pParaBufferTemp = (CHAR *)(&pTempTableData->Data[0]);
    for (;ucTableItem > 0; ucTableItem--)
    {
        /* ȡ��һ����ÿ����������ڵ�ַ */
        for (iTemp = 1; iTemp <= ucTableField; iTemp++)
        {
            global_InTableMode.pParameterStartAddress[iTemp] = pParaBufferTemp;

            /* ���������������һ��������λ�� */
            if (pTempTableData->Item[iTemp - 1].Type == DATA_TYPE_STRING 
                && pTempData->Type == DATA_TYPE_LIST)
            {
                pParaBufferTemp += strlen (pParaBufferTemp) + 1;
            }
            else
            {
                pParaBufferTemp += pTempTableData->Item[iTemp - 1].Len;
            }
        }
        /* *pFootScriptָ����ڽű�����ʼλ�� */
        /* ����ѭ������Ϊ'\0'���˳� */
        for (;;)
        {
            WORD16 wParaValue;
            WORD32 ulParameterLocation;

            if (*pFootScript == MARK_STRINGEND)
                goto ErrorInDis;
            /* ���ýű����֧���������ú�������'\0'��"%r"�򷵻� */
            global_InTableMode.pFootScript = pFootScript;
            switch (intpSendToBuff_Normal_TableMode(pLinkState,&global_InTableMode))
            {
                /* ����˵��������'/0',�������˳� */
            case SUCCESS_END:
                /* if need to do some free work ? */
                goto ErrorInDis;
                break;
            case SUCCESS_ENDWITHTABLE:
                pFootScript = global_InTableMode.pFootScript;
                goto BreakFromWhile;
                break;
            case SUCCESS_ENDWITHSWITCH:
                /* %h�Ѿ����� */
                pFootScript = global_InTableMode.pFootScript;
                switch (*pFootScript++)
                {
                case 's':
                    /* �õ������� */
                    /* ����ò�����ֵ */
                    ulParameterLocation = intpGetScriptNumber(&pFootScript);
                    if (ulParameterLocation == 0)
                        ulParameterLocation = global_InTableMode.giParameterLocation++;
                    else
                        global_InTableMode.giParameterLocation = ulParameterLocation + 1;

                    DISPLAY_ASSERT(ulParameterLocation <= ucTableField);
                    global_InTableMode.sSwitchStack[global_InTableMode.bSwitchStackTop++].wParaValue = 
                        GetTableSwitchCase(&global_InTableMode, pTempTableData, ulParameterLocation);

                    continue;
                    break;
                case 'c':
                    /* �ж�ֵ�Ƿ���ȣ���֧�����Ƿ���� */
                    wParaValue = (WORD16)intpGetScriptNumber(&pFootScript);
                    DISPLAY_ASSERT(*pFootScript == ':');
                    pFootScript++;

                    if (global_InTableMode.sSwitchStack[global_InTableMode.bSwitchStackTop - 1].wParaValue == wParaValue)
                        continue;       /* whileѭ������ */
                    /* ���ȣ����ҵ���һ��%h,��Խ���м�ķ�֧�ж� */
                    wSwitchCounter = 1;
                    do
                    {
                        DISPLAY_ASSERT(*pFootScript != 0);
                        while (MARK_STRINGEND !=  *pFootScript )
                        {
                            if (*pFootScript == '%')
                            {
                                if (*(pFootScript + 1) == 'h')
                                {
                                    break;
                                }
                            }
                            pFootScript++;
                        }
                        switch (*(pFootScript + 2))
                        {
                        case 's':
                            wSwitchCounter++;
                            pFootScript += 3;
                            break;
                        case 'e':
                            wSwitchCounter--;
                            pFootScript += 3;
                            if (wSwitchCounter == 0)
                            {
                                global_InTableMode.bSwitchStackTop --;
                                goto next1;
                            }
                            break;
                        default:
                            /* %h�Ѿ��ҵ��ˣ��������� */
                            if (wSwitchCounter == 1)
                                goto next1;
                            pFootScript += 3;
                            break;
                        }
                    }
                    while (wSwitchCounter != 0);
    next1:          continue;
                    break;
                case 'e':
                    /* ��ջ */
                    DISPLAY_ASSERT(global_InTableMode.bSwitchStackTop > 0);
                    global_InTableMode.bSwitchStackTop--;
                    break;
                default:
                    DBG_ERR_LOG(INVALID_FOOTSCRIPT);
                    goto ErrorInDis;
                    break;
                }   /* end of %h�ķ�֧ */
                break;
            case ERROR_INDIS:
                goto ErrorInDis;
                break;
            default:
                DBG_ERR_LOG(WRONG_RETURN_VALUE);
                goto ErrorInDis;
            }       /* end of ���غ��� */
        }
    BreakFromWhile:
        DISPLAY_ASSERT(global_InTableMode.bSwitchStackTop == 0);
        if (ucTableItem == 1)
            break;
        pFootScript = pReservedScript;
    }
    pGlobal->pFootScript = pFootScript;
    pGlobal->pDispBuff = global_InTableMode.pDispBuff;
    return DISPLAY_SUCCESS;
ErrorInDis:
    return ERROR_INDIS;
}

/*------------------------------------------------------------------------------
    �� �� ��:   intpSendToBuff_Normal
    ��������:   �����ű�������ӳ���
    ��    ��:   pGlobal
    ��    ��:   ȫ���Ե���ʾ���������������ַ���
    ��    ��:   �������
    ȫ�ֱ���:   pParameterStartAddress     ������ڱ�
                giParameterLocation        ��ǰĬ�ϵĲ���λ��
                pDisplayBuffer             ��ʾ��������ͷָ��
    ע    ��:
==============================================================================
    �޸ļ�¼:
    �޸�����        �汾     �޸���      �޸�ԭ������
==============================================================================
    2000\07\21  1.0             ����
------------------------------------------------------------------------------*/
static INT32 intpSendToBuff_Normal(TYPE_LINK_STATE *pLinkState,TYPE_DISP_GLOBAL_PARA *pGlobal)
{
    typedef struct
    {
        BYTE Field;
        BYTE Item;
    }D_FIELD_ITEM;
    typedef struct
    {
        CHAR *FieldName;
        CHAR FieldFormation[10];
    }D_TABLE_FIELD;

    enum  STATE state;        /* ��ǰ״̬ */
    D_FIELD_ITEM sTable;      /* ��ű����������ֵ */
    static D_TABLE_FIELD sTableField[MAX_TABLE_FIELD + 1];  /* ��¼��������� */
    BYTE  ucTableFieldCounter;      /* ��ĸ��� */

    WORD32 ulParameterLocation;      /* ������λ�� */
    CHAR *pFootScript;              /* ָ��ű���ָ�� */
    CHAR *pParaBuffer;              /* ָ�������ָ�� */

    /* ��ʱ���� */
    INT32  iTemp, jTemp, lTemp;
    static CHAR TempSpecifierString[MAX_DISPLAY_WIDTH];
    CHAR *pTempSpecifierString;

    POPR_DATA pOprData;

    pFootScript = pGlobal->pFootScript;
    state = ST_NORMAL;                      /* start automaton */
    while (*pFootScript != MARK_STRINGEND)
    {
        switch (state)
        {
        case ST_NORMAL:
            if (*pFootScript != '%')
            {
                SendToTelnetIfBufIsFull(pLinkState, 1);
                *pGlobal->pDispBuff++ = *pFootScript++;
            }
            else 
            {
                state = ST_PERCENT;
                pFootScript++;
            }
            break;
        case ST_PERCENT:  /* %״̬ */
            switch (*pFootScript)
            {
            case 'h':  /* ����%h */
                pFootScript++;
                pGlobal->pFootScript = pFootScript;
                return SUCCESS_ENDWITHSWITCH;
                break;
            case 'r':  /* ����%r */
                pFootScript++;
                pGlobal->pFootScript = pFootScript;
                return SUCCESS_ENDWITHFOR;
                break;
            case 't':  /* ����%t */
                pFootScript++;
                pGlobal->pFootScript = pFootScript;
                return SUCCESS_ENDWITHTABLE;
                break;
            case '%':  /* ����%% */
                state = ST_NORMAL;  /* ���״̬�� */
                SendToTelnetIfBufIsFull(pLinkState, 1);
                *pGlobal->pDispBuff++ = *pFootScript++;
                break;
            case '-':
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                {
                    CHAR TempString[MAX_DISPLAY_WIDTH];
                    INT cStyle;
                    WORD32 ulWidth;
                    if (*pFootScript == '-')
                    {
                        cStyle = -1;
                        pFootScript++;
                    }
                    else
                        cStyle = 0;
                    ulWidth = intpGetScriptNumber(&pFootScript);
                    DISPLAY_ASSERT(ulWidth < MAX_DISPLAY_WIDTH);
                    DISPLAY_ASSERT(*pFootScript++ == 't');
                    intpGetTextString(&pFootScript, TempString);
                    SendToTelnetIfBufIsFull(pLinkState, ulWidth);
                    intpGetStringByWidth(TempString,
                                         pGlobal->pDispBuff,
                                         (WORD32)(MAX_OUTPUT_RESULT - DISPLAY_BUFFER_COUNTER),
                                         (WORD32)ulWidth,
                                         cStyle);
                    pGlobal->pDispBuff += ulWidth;
                }
                state = ST_NORMAL;
                break;
            case 'v':
                /* ��ȡ��λ�� */
                pFootScript++;
                ulParameterLocation = intpGetScriptNumber(&pFootScript);
                if (ulParameterLocation == 0)
                    ulParameterLocation = pGlobal->giParameterLocation++;
                else
                    pGlobal->giParameterLocation = ulParameterLocation + 1;

                /* λ�úϷ� */
                if (ulParameterLocation > pGlobal->dwNumber)
                {
                    DBG_ERR_LOG(FOOTSCRIPT_DATA_MISMATCH);
                    return DISPLAY_SUCCESS;
                }

                pParaBuffer = pGlobal->pParameterStartAddress[ulParameterLocation];
                pGlobal->ParaNo = ((POPR_DATA)pParaBuffer)->ParaNo;
                if (pGlobal->ParaNo == PARATOBECONTINUED)
                {
                    /* ֻ�ڱ�Ļ�����ʹ�� */
                    /* �ű�����������%v�� */
                    for (pGlobal->spFootScript = pFootScript - 1;
                        *(pGlobal->spFootScript) != '%';
                            pGlobal->spFootScript--)
                    {
                    };
                }
                pFootScript++;      

                if (*pFootScript == 't')
                {
                    /* ���ֱ����ʾ������ı�������ͬ�� */
                    PTABLE_DATA pTableData;
                    static CHAR *sDisplayArray[MAX_TABLE_ITEM + 1][MAX_TABLE_FIELD];
                    static CHAR sBuffer[DISPLAY_BUFFER_SIZE + 1000];/* �����1000��ű�ͷ�� */
                    static INT32 iDisplayWidth[MAX_TABLE_FIELD + 1];
                    CHAR *pBuffer;

                    state = ST_NORMAL;
                    pFootScript++;
                    DISPLAY_ASSERT(*pFootScript != '%' || *(pFootScript+1) != 't'); 
                                                         /* ���費�� "%t%t" */
                                                         /* ��ͷ���ͷ�Ļ��Ը�ʽ����ָ��*/
                    pOprData = (POPR_DATA)pParaBuffer;
                    pTableData = (PTABLE_DATA)&(pOprData->Data[0]);
                    pBuffer = (CHAR *)sBuffer;

                    /* ��ȡ�����������ֵ */
                    /* �˴������ƶ��岻̫��ͬ,msg.h �е�item�൱�ڱ������е�field */
                    sTable.Item = pOprData->Len;
                    sTable.Field = pTableData->ItemNumber;
                    DISPLAY_ASSERT(sTable.Item <= MAX_TABLE_ITEM);
                    DISPLAY_ASSERT(sTable.Field <= MAX_TABLE_FIELD);

                    /* ��ȡ��ͷ��������ʾ��ʽ */
                    ucTableFieldCounter = 0;

                    while (*pFootScript != '%' || *(pFootScript+1) != 't')
                    {
                        CHAR * pTemp;
                        ucTableFieldCounter++;

                        /* 1. ��ȡ��ͷ�� */
                        sTableField[ucTableFieldCounter].FieldName = pBuffer;

                        while (*pFootScript != '%')
                        {
                            *pBuffer++ = *pFootScript++;
                            if (*pFootScript == '%' && *(pFootScript + 1) == '%')
                            {
                                *pBuffer++ = '%';
                                *pBuffer++ = '%';
                                pFootScript += 2;
                            }
                        }
                        *pBuffer++ = MARK_STRINGEND;  /* ��������� */

                        /* 2. ��ȡ�и�ʽָ�� */
                        if (*(pFootScript + 1) == 't')
                        {
                            sTableField[ucTableFieldCounter].FieldFormation[0] = MARK_STRINGEND;
                            break;
                        }
                        pTemp = &sTableField[ucTableFieldCounter].FieldFormation[0];
                        intpGetFormatString(&pFootScript, &pTemp);
                    }

                    sTable.Field = ucTableFieldCounter; /* �Խű��е�����Ϊ׼ */
                    /* ��pFootScript�Ƶ���һ��λ�� */
                    pFootScript++;
                    pFootScript++;

                    /* �ж��Ǻ��Ի������� */
                    if (*pFootScript != 'h')
                    {
                        /* �ű�ָ���Ƶ���һ��λ�� */
                        pFootScript++;
                        pFootScript++;
                        /* Ԥ��ʾ��������������С�д�ӡ��� */
                        /* ����һ��Ԥ��ʾ���� */

                        /* Ԥ��ʾ��ͷ�� */
                        if (sTableField[1].FieldName[0] != 0)
                            for (jTemp = 1; jTemp <= sTable.Field; jTemp++)
                            {
                                sDisplayArray[0][jTemp] = sTableField[jTemp].FieldName;
                            }

                        /* Ԥ��ʾ���еı�Ԫ�أ�������ʾ������ */
                        pParaBuffer = (CHAR *)&(pTableData->Data[0]);

                        for (iTemp = 1; iTemp <= sTable.Item; iTemp++)
                            for (jTemp = 1; jTemp <= sTable.Field; jTemp++)
                            {
                                /* ���ͼ�� */
                                if (pTableData->Item[jTemp - 1].Type > DATA_TYPE_SYSCLOCK)
                                {
                                    DBG_ERR_LOG(INVALID_TYPE);
                                    return DISPLAY_SUCCESS;
                                }
                                /* ��ǰ�Ƿ�Ϊ�ַ������� */
                                if (pTableData->Item[jTemp - 1].Type != DATA_TYPE_STRING)
                                {
                                    sDisplayArray[iTemp][jTemp] =  pBuffer;
                                    pBuffer += intpParaDisplay(pLinkState,
                                                pBuffer,
                                                (INT32)MAX_DISPLAY_WIDTH,
                                                &sTableField[jTemp].FieldFormation[0],
                                                (BYTE *)pParaBuffer,
                                                (WORD32)pTableData->Item[jTemp - 1].Len
                                                ) + 1;
                                }
                                else
                                {
                                    sDisplayArray[iTemp][jTemp] = pParaBuffer;
                                }
                                /* �ж��Ƿ�ΪLIST���͵ı��ҵ�ǰ�Ƿ�Ϊ�ַ�������
                                   ��Ӧ���޸Ĳ���ָ�� */
                                if (pOprData->Type == DATA_TYPE_LIST &&
                                    pTableData->Item[jTemp - 1].Type == DATA_TYPE_STRING)
                                    pParaBuffer += strlen(pParaBuffer) + 1;
                                else
                                    pParaBuffer += pTableData->Item[jTemp - 1].Len;
                            }

                        /* ��ȡ��ʽ˵������ָ��������ʾ��ȣ������������iDisplayWidth */
                        for (jTemp = 1; jTemp <= sTable.Field; jTemp++)
                        {
                            CHAR * pTempFormation;
                            pTempFormation = sTableField[jTemp].FieldFormation;

                            /* ��ȡ%��С����֮��Ŀ��ָ�����������ܵĶ���˵���� */
                            DISPLAY_ASSERT(*pTempFormation == '%');
                            pTempFormation++;
                            if (*pTempFormation == '-')
                                pTempFormation++;
                            iDisplayWidth[jTemp] = intpGetScriptNumber(&pTempFormation);
                        }
                        /* ����������С��ȡ���ʽ���Ѿ�˵�����򲻿��� */
                        /* �ַ���Ҫ�������� */
                        for (jTemp = 1; jTemp <= sTable.Field; jTemp++)
                            if (iDisplayWidth[jTemp] == 0)
                                for (iTemp = 0; iTemp <= sTable.Item; iTemp++)
                                {
                                    if ((iTemp == 0 && sTableField[1].FieldName[0] == 0)
                                        || sDisplayArray[iTemp][jTemp] == NULL)
                                        continue;
                                    if (iDisplayWidth[jTemp] < 
                                        (lTemp = strlen(sDisplayArray[iTemp][jTemp])))
                                        iDisplayWidth[jTemp] = lTemp;
                                }

                        /* �����뷽ʽ�����ʵ���ʾ�����ʾԤ��ʾ����sDisplayArray�е����� */
                        if (sTableField[1].FieldName[0] != 0) 
                            iTemp = 0;
                        else
                            iTemp = 1;
                        for (; iTemp <= sTable.Item; iTemp++)
                        {
                            INT cNowStyle;
                            for (jTemp = 1; jTemp <= sTable.Field; jTemp++)
                            {
                                if (sTableField[jTemp].FieldFormation[1] == '-')  /* ��ǰ�Ķ����ʽ */
                                    cNowStyle = -1;
                                else
                                    cNowStyle = 0;

                                if (cNowStyle == -1)
                                {   /* ����� */
                                    /* ��Ϊ���һ��
                                       �������ʱ�����Ǽӿո�*/
                                    if (jTemp != sTable.Field)
                                    {
                                        SendToTelnetIfBufIsFull(pLinkState, iDisplayWidth[jTemp]);
                                        intpGetStringByWidth(sDisplayArray[iTemp][jTemp],
                                                         pGlobal->pDispBuff,
                                                         (WORD32)(MAX_OUTPUT_RESULT - DISPLAY_BUFFER_COUNTER),
                                                         (WORD32)iDisplayWidth[jTemp],
                                                         cNowStyle);
                                        pGlobal->pDispBuff += iDisplayWidth[jTemp];
                                    }
                                    else
                                    {
                                        strncpy(pGlobal->pDispBuff, 
                                                            sDisplayArray[iTemp][jTemp],
                                                            (WORD32)(MAX_OUTPUT_RESULT - DISPLAY_BUFFER_COUNTER));
                                        pGlobal->pDispBuff += strlen(sDisplayArray[iTemp][jTemp]);
                                        SendToTelnetIfBufIsFull(pLinkState, 1);
                                    }
                                }
                                else
                                {       /* �Ҷ��� */
                                    intpGetStringByWidth(sDisplayArray[iTemp][jTemp],
                                                     pGlobal->pDispBuff,
                                                     (WORD32)(MAX_OUTPUT_RESULT - DISPLAY_BUFFER_COUNTER),
                                                     (WORD32)iDisplayWidth[jTemp],
                                                     cNowStyle);
                                    pGlobal->pDispBuff += iDisplayWidth[jTemp];
                                    SendToTelnetIfBufIsFull(pLinkState, 1);
                                }
                                *pGlobal->pDispBuff++ = MARK_BLANK;
                                *pGlobal->pDispBuff = MARK_STRINGEND;
                            }  /* end of for jTemp */
                            /* ȥ������ʾ��һ���ո� ��һ���س� */
                            *(pGlobal->pDispBuff - 1) = '\n';
                            SendToTelnetIfBufIsFull(pLinkState, 1);
                        }
                        /* ȥ������ʾ��һ��\n */
                        *(--pGlobal->pDispBuff) = MARK_STRINGEND;
                    }
                    else
                    {
                        /* �������ˮƽ��ʽ��ӡ */
                        pParaBuffer = (CHAR *)&(pTableData->Data[0]);
                        SendToTelnetIfBufIsFull(pLinkState, 1);
                        for (iTemp = 1; iTemp <= sTable.Item; iTemp++)
                        {
                            for (jTemp = 1; jTemp <= sTable.Field; jTemp++)
                            {
                                strncpy(pGlobal->pDispBuff,
                                                    sTableField[jTemp].FieldName,
                                                    (WORD32)(MAX_OUTPUT_RESULT - DISPLAY_BUFFER_COUNTER)); /* ��ʾ��ͷ�� */
                                pGlobal->pDispBuff += strlen(sTableField[jTemp].FieldName);
                                SendToTelnetIfBufIsFull(pLinkState, 1);
                                if (sTableField[jTemp].FieldFormation[0] != MARK_STRINGEND)
                                {
                                    pGlobal->pDispBuff +=
                                        intpParaDisplay(pLinkState,
                                                pGlobal->pDispBuff,
                                                (INT32)(DISPLAY_BUFFER_SIZE - DISPLAY_BUFFER_COUNTER),
                                                sTableField[jTemp].FieldFormation, 
                                                (BYTE *)pParaBuffer,
                                                (WORD32)pTableData->Item[jTemp - 1].Len);
                                    /* pParaBuffer�Ƶ���ȷλ�� */
                                    /* �ж��Ƿ�ΪLIST���͵ı��ҵ�ǰ�Ƿ�Ϊ�ַ�������
                                       ��Ӧ���޸Ĳ���ָ�� */
                                    if (pOprData->Type == DATA_TYPE_LIST &&
                                        pTableData->Item[jTemp - 1].Type == DATA_TYPE_STRING)
                                        pParaBuffer += strlen(pParaBuffer) + 1;
                                    else
                                        pParaBuffer += pTableData->Item[jTemp - 1].Len;
                                }
                            }
                        /* ��һ���س� */
                        *pGlobal->pDispBuff++ = '\n';
                        *pGlobal->pDispBuff = MARK_STRINGEND;
                        SendToTelnetIfBufIsFull(pLinkState, 1);
                        }
                        /* ȥ������ʾ��һ��\n */
                        *(--pGlobal->pDispBuff) = MARK_STRINGEND;
                        pFootScript++;

                    }  /* TABLE1�͵ı�ĺ�����ʾ����*/
                }      /* �������ʾ�������        */
                else   /* �˴���������ʽ˵�����﷨���,ֱ�ӽ���sprintf */
                {
                    WORD32 ulDataLen;
                    BYTE bType;

                    pOprData = (POPR_DATA)pParaBuffer;
                    /* ��ȡParaNo */
                    pGlobal->ParaNo = pOprData->ParaNo;

                    /* �������� */
                    bType = (BYTE)pOprData->Type;
                    /*
                    change by wangmh for DATA_TYPE_MACADDR type display
                    if (bType > DATA_TYPE_IMASK)*/
                    if (bType > DATA_TYPE_PARA)/*DATA_TYPE_MACADDR*/
                    {
                        DBG_ERR_LOG(INVALID_TYPE);
                        return ERROR_INDIS;
                    }
                    /* ��ȡ��ʽ˵����������ʾ */
                    /* look for the specification, copy the whole format specification
                       to TempSpecifierString */
                    state = ST_NORMAL;

                    pTempSpecifierString = &TempSpecifierString[0];
                    *pTempSpecifierString++ = '%';
                    intpGetFormatString(&pFootScript, &pTempSpecifierString);
                    ulDataLen = (BYTE)pOprData->Len;
                    if (RETURN_DATALEN[bType] < ulDataLen)
                    {
                        DBG_ERR_LOG(INVALID_DATALEN);
                        return ERROR_INDIS;
                    }
                    SendToTelnetIfBufIsFull(pLinkState, 1);
                    pParaBuffer = (CHAR *)&(pOprData->Data[0]);
                    pGlobal->pDispBuff += 
                                    intpParaDisplay(pLinkState,
                                                pGlobal->pDispBuff,
                                                (INT32)(DISPLAY_BUFFER_SIZE - DISPLAY_BUFFER_COUNTER),
                                                TempSpecifierString,
                                                (BYTE *)pParaBuffer,
                                                (WORD32)ulDataLen);
                    pParaBuffer += ulDataLen;
                    SendToTelnetIfBufIsFull(pLinkState, 1);
                } /* end of else  */ 
                /* omit by wangmh 2003-07-11 for not over 255 parameter a packet
                if (pGlobal->ParaNo == PARATOBECONTINUED)
                {
                    // �ڱ��У���Ҫ��һ���س� 
                    *pGlobal->pDispBuff++ = '\n';
                    *pGlobal->pDispBuff = MARK_STRINGEND;
                    return SUCCESS_CONTINUETONEXTPACKET;
                }
                */
                break; /* end of case 'v' */
            default:
                DBG_ERR_LOG(INVALID_FOOTSCRIPT);
                return ERROR_INDIS;
                break;
            }
            break;
        default:
            DBG_ERR_LOG(INVALID_STATE);
            return ERROR_INDIS;
        }  /* end of  switch (state) */
                        
    }  /* end of  while (*pFootScript != MARK_STRINGEND) */
    pGlobal->pFootScript = pFootScript;
    return SUCCESS_END;
}

/*------------------------------------------------------------------------------
    �� �� ��:   OamCfgintpSendToBuffer
    ��������:   �ű�������ӳ����ⲿ���õ����
    ��    ��:   MSG_COMM_OAM *ptMsg   �����ռ���ڵ�ַ
                CHAR * pFootScript    �ű������λ��
                TYPE_DISP_GLOBAL_PARA *pGlobal ��������ȫ�ֱ�����ָ��
    ��    ��:   ȫ���Ե���ʾ���������������ַ���
    ��    ��:   �������
    ȫ�ֱ���:   pParameterStartAddress     ������ڱ�
                giParameterLocation        ��ǰĬ�ϵĲ���λ��
                pDisplayBuffer             ��ʾ��������ͷָ��
    ע    ��:
==============================================================================
    �޸ļ�¼:
    �޸�����        �汾     �޸���      �޸�ԭ������
==============================================================================
------------------------------------------------------------------------------*/
INT32 OamCfgintpSendToBuffer(TYPE_LINK_STATE *pLinkState,
                             MSG_COMM_OAM *ptMsg,           /* �����ռ���ڵ�ַ */
                             CHAR * pFootScriptEx,          /* �ű������λ�� */
                             TYPE_DISP_GLOBAL_PARA *pGlobal /* ��������ȫ�ֱ�����ָ�� */
                             )
{

    /* ��ȡÿ����������ʼ��ַ ÿһ���������Ϊ��һ������ */
    CHAR *pParaBuffer;
    POPR_DATA pTempData;
    CHAR *pParaBufferTemp;
    WORD16 wForCounter;                       /* ��for��Ƕ�׽��м��� */
    WORD16 wSwitchCounter;                    /* ��switch��Ƕ�׽��м��� */
    WORD16 wParaValue;
    WORD32 ulParameterLocation;
    CHAR *pFootScript = NULL;
   
    pFootScript = pFootScriptEx;
    pGlobal->giParameterLocation = 1;       /* ��ʼ������λ�� */
    pGlobal->bForStackTop = 0;              /* ��ʼ��Forջ */
    pGlobal->bSwitchStackTop = 0;           /* ��ʼ��Switchջ */
    pGlobal->spFootScript = NULL;           /* ��ʼ���ű�λ�� */

    pGlobal->dwNumber = ptMsg->Number;
    *(pGlobal->pDisplayBuffer) = MARK_STRINGEND;    /* ��ʼ����ʾ������ */
    pGlobal->pDispBuff = pGlobal->pDisplayBuffer;   /* ��ʾ��������ǰλ�� */
    pParaBuffer = (CHAR *)&(ptMsg->Data[0]);        /* ��ȡ��һ����������ʼ��ַ */
    pParaBufferTemp = pParaBuffer;

    /* ��ÿһ����������ڵ�ַ������ڵ�ַ���� */
    {
        WORD32 iTemp;
        PTABLE_DATA pTempTableData;
        WORD16 wLenOfData = 0; /* ���ݲ��ֵĳ��� */

        for (iTemp = 1; iTemp <= pGlobal->dwNumber; iTemp++)
        {
            /* ��ÿһ����������ڵ�ַ������ڵ�ַ���� */
            pGlobal->pParameterStartAddress[iTemp] = pParaBufferTemp;

            pTempData = (POPR_DATA)pParaBufferTemp;

            /*�������Ƕ�CRDCM00206044 ����Ԥ����������ӻ�����pTempData->Type����ʱ�����*/
            if (!pTempData)
            {
                /*printf("!!!yu-an-CRDCM00206044: pTempData is NULL!\n");*/
                return DISPLAY_SUCCESS;
            }
            else if (!pTempData->Type)
            {
                /*printf("!!!yu-an-CRDCM00206044: pTempData->Type is NULL!\n");*/
                return DISPLAY_SUCCESS;
            }
            /* 611000337784 Ӧ�÷��ص�table���ͼ�¼��Ϊ0��OPR_DATA��data����Ϊ0��ʱ��
            OamCfgintpSendToBuffer������ʻ�Խ�� */
            wLenOfData = (WORD16)((BYTE *)pTempData - (BYTE *)ptMsg);
            wLenOfData = ptMsg->DataLen - wLenOfData;
            if (wLenOfData < sizeof(OPR_DATA))
            {
                DBG_ERR_LOG(INVALID_DATALEN);
                return DISPLAY_SUCCESS;
            }

            /* ���������������һ��������λ�� */
            if (pTempData->Type == DATA_TYPE_TABLE)
            {
                WORD32 jTemp;
                INT32 iTempCounter = 0;

                pTempTableData = (PTABLE_DATA)&(pTempData->Data[0]);
                /* 611000337784 Ӧ�÷��ص�table���ͼ�¼��Ϊ0��OPR_DATA��data����Ϊ0��ʱ��
                OamCfgintpSendToBuffer������ʻ�Խ�� */
                wLenOfData = (WORD16)((BYTE *)pTempTableData - (BYTE *)ptMsg);
                wLenOfData = ptMsg->DataLen - wLenOfData;
                if (wLenOfData < sizeof(TABLE_DATA))
                {
                    DBG_ERR_LOG(INVALID_DATALEN);
                    return DISPLAY_SUCCESS;
                }

                for (jTemp = 1; jTemp <= pTempTableData->ItemNumber; jTemp++)
                    iTempCounter += pTempTableData->Item[jTemp - 1].Len;
                if (pTempData->Len != 0)
                    pParaBufferTemp += sizeof(TABLE_DATA) + sizeof(OPR_DATA)
                                     - sizeof(BYTE) - sizeof(BYTE)
                                     + iTempCounter * pTempData->Len;
                else 
                    pParaBufferTemp += sizeof(TABLE_DATA) + sizeof(OPR_DATA) 
                                       - sizeof(BYTE);
                /* !!!���ṹ�����ѡ��ı�ʱ���˴�һ��Ҫ����������ȷ��!!! */
                /* ���⣬0*0�ľ���ĳ��ȼ��������Ϲ�ʽ����ȷ */

            }
            else if (pTempData->Type == DATA_TYPE_LIST)
            {
                CHAR * pTemp;
                INT32 kTemp, lTemp;

                pTempTableData = (PTABLE_DATA)&(pTempData->Data[0]);
                /* 611000337784 Ӧ�÷��ص�table���ͼ�¼��Ϊ0��OPR_DATA��data����Ϊ0��ʱ��
                OamCfgintpSendToBuffer������ʻ�Խ�� */
                wLenOfData = (WORD16)((BYTE *)pTempTableData - (BYTE *)ptMsg);
                wLenOfData = ptMsg->DataLen - wLenOfData;
                if (wLenOfData < sizeof(TABLE_DATA))
                {
                    DBG_ERR_LOG(INVALID_DATALEN);
                    return DISPLAY_SUCCESS;
                }
                
                /* ������ͷ��ָ������ */
                pTemp = (CHAR *)&(pTempTableData->Data[0]);
                if (pTempData->Len != 0)
                {
                    for (kTemp = 1; kTemp <= pTempData->Len; kTemp++)
                        for (lTemp = 1; lTemp <= pTempTableData->ItemNumber; lTemp++)
                        {
                            if (pTempTableData->Item[lTemp -1].Type == DATA_TYPE_STRING)
                            {
                                /*edit start by wangmh for string handler 2003-05-16*/
                              /*  pTemp += pTempTableData->Item[lTemp -1].Len; */
                                /*edit end by wangmh for string handler 2003-05-16*/
                                /* Keep the older 
                                �˴����Լ�������ж� 
                                while (*pTemp++ != 0);    //һֱ����'\0'��β��Ϊֹ 
                                */
                                while (*pTemp++ != 0)
                                {
                                	if ((WORD32)((CHAR *)pTemp - (CHAR *)ptMsg) > ptMsg->DataLen)
                                	{
                                		break;	
                                	}
                                } 
                                
                            }
                            else
                            {
                                pTemp += pTempTableData->Item[lTemp -1].Len;
                            }
                        }
                    pParaBufferTemp = pTemp;
                 }
                 else
                    pParaBufferTemp = pTemp + 1;
            }
            else
                pParaBufferTemp = (CHAR *)GetNextOprData(pTempData);
                
            /* ���鳤���Ƿ���ȷ */
            if ((WORD32)((CHAR *)pParaBufferTemp - (CHAR *)ptMsg) > ptMsg->DataLen)
            {
                SendStringToTelnet(pLinkState, map_szAppMsgDatalengthErr);
                DBG_ERR_LOG(INVALID_DATALEN);
                return DISPLAY_SUCCESS;
            }
        }
        /* ���鳤���Ƿ���ȷ */
        {
            if ((WORD32)((CHAR *)pParaBufferTemp - (CHAR *)ptMsg) > ptMsg->DataLen)
                DBG_ERR_LOG(INVALID_DATALEN);
        }
    }

    /*���returncode����ERR_AND_HAVEPARA�ҷ��ز���Ϊmap���ͣ��޸Ļ��Խű���
        Ŀǰֻ��brs��ô��*/
/*    if(ptMsg->ExecModeID == MODE_PROTOCOL)*/
    {
            if((ptMsg->Data[0].Type == DATA_TYPE_MAP)&&
                (ptMsg->ReturnCode == ERR_AND_HAVEPARA ||ptMsg->ReturnCode == ERR_MSG_FROM_INTPR) &&
                (ptMsg->Number == 1))		    
            {
                /*XMLר����·*/
                if(CheckIsXMLAdapterLinkState(pLinkState))
                {
                    pFootScript = "%v%b";
                }
                else
                {
                    pFootScript = "%v%b\r\n";
                }
            }
    }

    /* * pFootScript�Ѿ�ָ��ű�����ʼλ�� */
    /* ����ѭ������Ϊ'\0'���˳� */
    while (*pFootScript != MARK_STRINGEND)
    {
        /* ���ýű����֧���������ú�������'\0'��"%r"�򷵻� */
        pGlobal->pFootScript = pFootScript;
        switch (intpSendToBuff_Normal(pLinkState,pGlobal))
        {
        /* ����˵��������'/0',�������˳� */
        case SUCCESS_END:
            /* if need to do some free work ? */
            goto BreakFromWhile;
            break;
        case SUCCESS_ENDWITHTABLE:
            pFootScript = pGlobal->pFootScript;
            ulParameterLocation = intpGetScriptNumber(&pFootScript);
            if (ulParameterLocation == 0)
                ulParameterLocation = pGlobal->giParameterLocation++;
            else
                pGlobal->giParameterLocation = ulParameterLocation + 1;

            DISPLAY_ASSERT(ulParameterLocation <= pGlobal->dwNumber);
            /* ParaNo Ϊ255����¼�µ�ǰ�Ľű�λ�� */
            pParaBuffer = pGlobal->pParameterStartAddress[ulParameterLocation];
            pGlobal->ParaNo = ((POPR_DATA)pParaBuffer)->ParaNo;
            if (pGlobal->ParaNo == PARATOBECONTINUED)
            {
                /* ֻ�ڱ�Ļ�����ʹ�� */
                /* �ű�����������%v�� */
                for (pGlobal->spFootScript = pFootScript - 1;
                    *(pGlobal->spFootScript) != '%';
                        pGlobal->spFootScript--)
                {
                };
            }
            /* �ڽű��и�ð�Ų���ʡ��, ð����һ����Ч�ָ�� */
            DISPLAY_ASSERT(*pFootScript == ':'); 
            pFootScript++;
            /* ���ñ�ģʽ�µĻ��Ժ��� */
            pGlobal->pFootScript = pFootScript;
            intpSendToBuffer_TableMode(pLinkState, pGlobal, ulParameterLocation);
            pFootScript = pGlobal->pFootScript;
            if (pGlobal->ParaNo == PARATOBECONTINUED)
                goto BreakFromWhile;
            break;
        case SUCCESS_ENDWITHFOR:  /* ��%r������ѭ������ */
            pFootScript = pGlobal->pFootScript;
            switch (*pFootScript++)
            {
            case 'e':  /* ��%re */
                DISPLAY_ASSERT(pGlobal->bForStackTop > 0); 
                if (pGlobal->sForStack[pGlobal->bForStackTop - 1].wForCounter <= 1)
                {
                    pGlobal->bForStackTop--;
                    break;
                }
                pGlobal->sForStack[pGlobal->bForStackTop - 1].wForCounter--;
                pFootScript = pGlobal->sForStack[pGlobal->bForStackTop - 1].pFootScript;
                break;
            case 's':  /* ��%rs */
                /* �õ������� */
                ulParameterLocation = intpGetScriptNumber(&pFootScript);
                if (ulParameterLocation == 0)
                    ulParameterLocation = pGlobal->giParameterLocation++;
                else
                    pGlobal->giParameterLocation = ulParameterLocation + 1;

                DISPLAY_ASSERT(ulParameterLocation <= pGlobal->dwNumber);
                /* �ڽű��и�ð�Ų���ʡ�ԣ������������� */
                DISPLAY_ASSERT(*pFootScript == ':'); 
                pFootScript++;
                /* ջ���ѷ���ѭ������Ĵ��� */
                pTempData = (POPR_DATA)pGlobal->pParameterStartAddress[ulParameterLocation];
                wParaValue = GetForCounterSwitchCase(pTempData);
                if (wParaValue <= 0)  /* ѭ���Ĵ���С��1,����ѭ���� */
                {
                    wForCounter = 1;
                    while(*pFootScript != 0)
                    {
                        if (*pFootScript == '%')
                        {
                            if (*(pFootScript + 1) == 'r')
                            {
                                switch (*(pFootScript + 2))
                                {
                                case 's':
                                    pFootScript += 3;    /* �ű�����%rs�� */
                                    wForCounter++;
                                    continue;
                                    break;
                                case 'e':
                                    pFootScript += 3;    /* �ű�����%re�� */
                                    wForCounter--;
                                    if (wForCounter == 0)
                                        goto next2;      /* �뵱ǰ��%rs����Ӧ��%re�Ѿ����� */
                                    continue;
                                default:
                                    DBG_ERR_LOG(INVALID_FOOTSCRIPT);
                                    break;
                                }
                            }
                        }
                        pFootScript++;
                    }
                    /* %h��δ�ҵ����ű����������ش������ */
                    DBG_ERR_LOG(INVALID_FOOTSCRIPT);
                    goto BreakFromWhile;
                }
                else
                {
                    pGlobal->sForStack[pGlobal->bForStackTop++].wForCounter = wParaValue;
                    pGlobal->sForStack[pGlobal->bForStackTop - 1].pFootScript = pFootScript;
                }
next2:          continue;  /* whileѭ������ */
                break;
            default:
                DBG_ERR_LOG(INVALID_FOOTSCRIPT);
                goto BreakFromWhile;
                break;
            }
            break;
        case SUCCESS_ENDWITHSWITCH:
            /* %h�Ѿ����� */
            pFootScript = pGlobal->pFootScript;
            switch (*pFootScript++)
            {
            case 's':
                /* �õ������� */
                /* ����ò�����ֵ */
                ulParameterLocation = intpGetScriptNumber(&pFootScript);                
                if (ulParameterLocation == 0)
                    ulParameterLocation = pGlobal->giParameterLocation++;
                else
                    pGlobal->giParameterLocation = ulParameterLocation + 1;

                DISPLAY_ASSERT(ulParameterLocation <= pGlobal->dwNumber);
                pTempData = (POPR_DATA)pGlobal->pParameterStartAddress[ulParameterLocation];
                pGlobal->sSwitchStack[pGlobal->bSwitchStackTop++].wParaValue = GetForCounterSwitchCase(pTempData);

                continue;
                break;
            case 'c':
                /* �ж�ֵ�Ƿ����, ��֧�����Ƿ���� */
                wParaValue = (WORD16)intpGetScriptNumber(&pFootScript);
                DISPLAY_ASSERT(*pFootScript == ':');
                pFootScript++;

                /* ���, whileѭ������ */
                if (pGlobal->sSwitchStack[pGlobal->bSwitchStackTop - 1].wParaValue == wParaValue)
                    continue; 

                /* ����, ���ҵ���һ��%h, ��Խ���м�ķ�֧�ж� */
                wSwitchCounter = 1;
                do
                {
                    DISPLAY_ASSERT(*pFootScript != 0);
                    while (MARK_STRINGEND != *pFootScript)
                    {
                        if (*pFootScript == '%')
                        {
                            if (*(pFootScript + 1) == 'h')
                            {
                                break;
                            }
                        }
                        pFootScript++;
                    }
                    switch (*(pFootScript + 2))
                    {
                    case 's':
                        wSwitchCounter++;
                        pFootScript += 3;
                        break;
                    case 'e':
                        wSwitchCounter--;
                        pFootScript += 3;
                        if (wSwitchCounter == 0)
                        {
                            pGlobal->bSwitchStackTop --;
                            goto next1;
                        }
                        break;
                    default:
                        /* %h�Ѿ��ҵ��ˣ��������� */
                        if (wSwitchCounter == 1)
                            goto next1;
                        pFootScript += 3;
                        break;
                    }
                }
                while (wSwitchCounter != 0);
next1:          continue;
                break;
            case 'e':
                /* ��ջ */
                DISPLAY_ASSERT(pGlobal->bSwitchStackTop > 0);
                pGlobal->bSwitchStackTop--;
                break;
            default:
                DBG_ERR_LOG(INVALID_FOOTSCRIPT);
                goto BreakFromWhile;
                break;
            }   /* end of %h�ķ�֧ */
            break;
        case SUCCESS_CONTINUETONEXTPACKET:
            goto BreakFromWhile;
            break;
        case ERROR_INDIS:
            goto BreakFromWhile;
            break;
        default: /* Ŀǰֻ�� return DISPLAY_SUCCESS; �ᵽ����*/
            DBG_ERR_LOG(WRONG_RETURN_VALUE);
            goto BreakFromWhile;
        }  /* end of ���غ��� */
    }
BreakFromWhile:
    *pGlobal->pDispBuff = MARK_STRINGEND;
    /*    assert((pGlobal->bForStackTop == 0) && (pGlobal->bSwitchStackTop == 0));*/
    DISPLAY_ASSERT(pGlobal->bForStackTop == 0);
    DISPLAY_ASSERT(pGlobal->bSwitchStackTop == 0);

    return DISPLAY_SUCCESS;
}

/*lint +e420*/
/*lint +e571*/
/*lint +e716*/
/*lint +e722*/
/*lint +e797*/
/*lint +e662*/
/*lint +e1772*/
/*lint +e1895*/
/*lint +e749*/

