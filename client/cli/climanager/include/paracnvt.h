/*********************************************************************
* ��Ȩ���� (C)2002, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ�ParaCnvt.h
* �ļ���ʶ��
* ����ժҪ������ת��
* ����˵����
            
* ��ǰ�汾��
* ��    �ߣ����� ��ֲ����
* ������ڣ�2007.6.5
*
* �޸ļ�¼1��// �޸���ʷ��¼�������޸����ڡ��޸��߼��޸�����
*    �޸����ڣ�
*    �� �� �ţ�
*    �� �� �ˣ�
*    �޸����ݣ�
* �޸ļ�¼2����
**********************************************************************/
#ifndef __PARA_CNVT_H__
#define __PARA_CNVT_H__

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

/* sucess and continue */
#define PARA_SUCCESS                0 /* ƥ��ɹ���û�д��� */
#define PARA_INIT                   -1/* ��ʼֵ */
                                    
/* convert error */                 
#define PARAERR_CONVERT             -101  /* ��������ת��ʧ�� */
#define PARAERR_OVERFLOW            -102  /* ����ֵԽ��   */
#define PARAERR_NULLSTRING          -103  /* Ҫת�����ַ�����NULL */
                                    
#define PARAERR_BOARDTYPE           -104  /* �����ڵ�board���� */
#define PARAERR_VIRGULE             -105  /* ȱ��'/' */
#define PARAERR_ENDMARK             -106  /* ȱ��'\0' */
#define PARAERR_IFNOEXIST           -107  /* �����ڵĽӿں� */
#define PARAERR_NOFIND              -108  /* ��ָ���ַ�������ָ�����ַ�*/
#define PARAERR_CONSTANT            -109  /* constant����ƥ��ʧ�� */
#define PARAERR_MACADDR             -110  /* error mac address */
#define PARAERR_MACLEN              -111  /* error mac address */
                                    
/* vital error and cease */         
#define PARAERR_VITALERR            -201
#define PARAERR_SQUARE_SYMPARA      -202  /* []��sym�ɹ�����paraʧ��  []��������  */
#define PARAERR_WHOLEPARA           -203  /* WHOLE��Para/NOSYM�ǵ�һ��ƥ��ʧ��*/
#define PARAERR_REPEATNULL          -204    /* REPEAT�в�������Ϊ�� */
                                    
/* failure and contineu */          
#define PARAERR_DETERMINER          -301  /* ������Determiner����ǰ����ƥ��*/
#define PARAERR_STRUCTTYPE          -302  /* û�ж���Ľṹ���� */
#define PARAERR_PRAMMARTYPE         -303  /* û�ж�����﷨���� */
#define PARAERR_MISUSESTRUCT        -304  /* �ṹ������ʹ�ô��� */
#define PARAERR_MISUSEPRAMMAR       -305  /* �﷨����ʹ�ô��� */
#define PARAERR_DATATYPE            -306  /* û�ж���Ĳ������� */
#define PARAERR_ANGLE               -307  /* {}ƥ��ʧ�ܣ���ѡ���� */
#define PARAERR_SQUARE_SYM          -308  /* []��Symbolƥ��ʧ��   �ɿ����Թ�[]*/
#define PARAERR_SQUARE_PARA         -309  /* []��Parameterƥ��ʧ��  �ɿ����Թ�[]*/
#define PARAERR_WHOLESYMBOL         -311  /* WHOLE��Symbolƥ��ʧ��*/
#define PARAERR_NODEFAULT           -313  /* ��ȱʡ���� */
#define PARAERR_MAXIMUM             -314  /* �������������� */
#define PARAERR_NOCMDMATCH          -315  /* û�ж����NoCmdƥ��涨*/

#define MAX_LONG_LONG_VALUE              (WORD64)(-1) /*֧��WORD64����*/
#define MAX_LONG_LONG_LENGTH             20 /*֧��WORD64����*/
#define MAX_BOARDNAME               8

#define IPADDR_TYPE_IPV4               4
#define IPADDR_TYPE_IPV6               6

typedef struct
{
    union 
    {
        BYTE   __u6_addr8[16];
        WORD16   __u6_addr16[8];
        WORD32  __u6_addr32[4];
    } __u6_addr;            /* 128-bit IP6 address */
}_PACKED_1_ IN6_ADDR;

typedef struct
{
    BYTE     family;
    BYTE     prefixlen;
    BYTE     pad1;
    BYTE     pad2;
    IN6_ADDR prefix;
}_PACKED_1_ PREFIX_IPV6;

typedef struct 
{
    WORD16     asn;
    WORD32    nn;
}_PACKED_1_ VPN_ASN;

typedef struct 
{
    WORD32    ipaddr;
    WORD16     nn;
}_PACKED_1_ VPN_IP;

/* ���Ⱪ¶�ӿ� */
WORD32 FindMapTypePos(T_OAM_Cfg_Dat * , WORD16 wType);
WORD32 FindMapItemPos(T_OAM_Cfg_Dat * , WORD32 dwTypePos, WORD16 wValue);
int OamCfgFindCharInString(CHAR *pString, CHAR cChar, int *pPos);
CHAR *OamCfgConvertWordToString(T_OAM_Cfg_Dat * ,WORD16 wType, WORD16 wValue);
int ConvertStringToWord(TYPE_PARA *pPara, CHAR *pValue, WORD16 *pWValue, WORD16 *pMatchLen, T_OAM_Cfg_Dat * cfgDat);
int MatchTwoString(CHAR *pInStr, CHAR *pPatternStr, WORD16 *pMatchLen); 
int StrCmpNoCase(CHAR *pInStr, CHAR *pPatternStr);
void DeleteNouseZero(CHAR *str);
int ConvertStrToCmdPara(CHAR *pString, TYPE_LINK_STATE *pLinkState, T_OAM_Cfg_Dat * cfgDat);
CHAR* GetFormatByCmdID(WORD32 dwCmdId, T_OAM_Cfg_Dat * cfgDat);
CHAR* GetFormatByCmdAttrPos(WORD16 wCmdAttrPos, T_OAM_Cfg_Dat * cfgDat);
int ConvertStrToCmdParaByParaType(BYTE ucLinkID,CHAR *pString,T_OAM_Cfg_Dat * pcfgDat,TYPE_PARA* pParaInfo,WORD16 wParaNo,CMDPARA *pCmdPara,BYTE bDeterminer);

#ifdef WIN32
    #pragma pack()
#endif

#endif /* __PARA_CNVT_H__ */

 
