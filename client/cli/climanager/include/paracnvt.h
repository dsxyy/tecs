/*********************************************************************
* 版权所有 (C)2002, 深圳市中兴通讯股份有限公司。
*
* 文件名称：ParaCnvt.h
* 文件标识：
* 内容摘要：参数转换
* 其它说明：
            
* 当前版本：
* 作    者：马俊辉 移植整理
* 完成日期：2007.6.5
*
* 修改记录1：// 修改历史记录，包括修改日期、修改者及修改内容
*    修改日期：
*    版 本 号：
*    修 改 人：
*    修改内容：
* 修改记录2：…
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
#define PARA_SUCCESS                0 /* 匹配成功，没有错误 */
#define PARA_INIT                   -1/* 初始值 */
                                    
/* convert error */                 
#define PARAERR_CONVERT             -101  /* 参数类型转化失败 */
#define PARAERR_OVERFLOW            -102  /* 参数值越界   */
#define PARAERR_NULLSTRING          -103  /* 要转换的字符串是NULL */
                                    
#define PARAERR_BOARDTYPE           -104  /* 不存在的board类型 */
#define PARAERR_VIRGULE             -105  /* 缺少'/' */
#define PARAERR_ENDMARK             -106  /* 缺少'\0' */
#define PARAERR_IFNOEXIST           -107  /* 不存在的接口号 */
#define PARAERR_NOFIND              -108  /* 在指定字符串中无指定的字符*/
#define PARAERR_CONSTANT            -109  /* constant类型匹配失败 */
#define PARAERR_MACADDR             -110  /* error mac address */
#define PARAERR_MACLEN              -111  /* error mac address */
                                    
/* vital error and cease */         
#define PARAERR_VITALERR            -201
#define PARAERR_SQUARE_SYMPARA      -202  /* []的sym成功，而para失败  []致命错误  */
#define PARAERR_WHOLEPARA           -203  /* WHOLE的Para/NOSYM非第一个匹配失败*/
#define PARAERR_REPEATNULL          -204    /* REPEAT中参数个数为零 */
                                    
/* failure and contineu */          
#define PARAERR_DETERMINER          -301  /* 输入有Determiner，提前结束匹配*/
#define PARAERR_STRUCTTYPE          -302  /* 没有定义的结构类型 */
#define PARAERR_PRAMMARTYPE         -303  /* 没有定义的语法类型 */
#define PARAERR_MISUSESTRUCT        -304  /* 结构体类型使用错误 */
#define PARAERR_MISUSEPRAMMAR       -305  /* 语法类型使用错误 */
#define PARAERR_DATATYPE            -306  /* 没有定义的参数类型 */
#define PARAERR_ANGLE               -307  /* {}匹配失败，非选择项 */
#define PARAERR_SQUARE_SYM          -308  /* []的Symbol匹配失败   可看做略过[]*/
#define PARAERR_SQUARE_PARA         -309  /* []的Parameter匹配失败  可看做略过[]*/
#define PARAERR_WHOLESYMBOL         -311  /* WHOLE的Symbol匹配失败*/
#define PARAERR_NODEFAULT           -313  /* 非缺省参数 */
#define PARAERR_MAXIMUM             -314  /* 超过参数最大个数 */
#define PARAERR_NOCMDMATCH          -315  /* 没有定义的NoCmd匹配规定*/

#define MAX_LONG_LONG_VALUE              (WORD64)(-1) /*支持WORD64类型*/
#define MAX_LONG_LONG_LENGTH             20 /*支持WORD64类型*/
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

/* 对外暴露接口 */
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

 
