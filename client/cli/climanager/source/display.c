/************************************************************************* 
    
    version 12a
    2002/5/10


    history:
    12a: 1. 加入了根据ID显示字符串类型
    11d: 1. 加入了日志功能
            2. 引用oamros.h中的参数类型定义

    11c:    1. 因扩充表结构已经能够处理部分表的显示，
               在原有的表模式中去掉部分表的显示功能

    11b:    1. 大函数拆分

    11a:    1. 在表中也可以使用判断，增加了表显示模式

    10a:    1. 去掉了pDisplayBufferCounter,该用pDispBuff指针指向缓冲区的下一个位置
            2. 去掉了一个显示short类型的数的bug
            3. 所有的文本都用TEXT_MARK引起来
            4. 增加了一些错误判定，统一告警机制
            5. 增加了大写的脚本字符，用于显示网络字节序的数字,A U

    09g:    1. 改变了预打印的机制
            2. 映射表打印时，一个2级指针使用的错误
            3. 某些显示格式的修改
            4. 表头名也放入sbuffer中
            5. 统一了两种表显示的格式

    09f:    1. 程序函数化

    09e:    1. 修改了一些bug

    09d:    1. 完善了多包显示支持
            2. 向多进程靠拢

    09c:    1. 改变了映射表的定义后，修改了参数显示函数
            2. 将映射表的参数项改为4字节的

    09b:    1. 由头一个包的最后一个ParaNo与下一个包的第一个paraNo完成多包显示的脚本选择
            2. 改正了脚本中参数获得的几个相同的bug
            3. 加入0值为分支判断的有效值  (程序未作改动，经考察有此功能)
            4. 修改了位置参数省略时的一个bug

    09a:    1. 加入循环和分支判断的嵌套
            2. 修改了相应的脚本符号
            3. 循环中不支持break，continue，goto语句

    08_a:   1. 对照06e加入脚本循环
            2. 在%r后必须加入一个冒号，以免引起歧义
            3. 加入了时间日期的显示 
            4. 加入了64位无符号数的显示

    07_a:   1. 增加了新的表参数的格式，当类型为字符串时，len将无效，长度由'\0'符控制
            2. 当表参数中前一列为右对齐，后一列为左对齐时，中间加空格
            3. 表参数中字符串显示方式的改进

    06_h:   1. 改进了越界处理。保证只要有越界，就能给出断言错误
            2. 去掉了对pDisplayBufferCounter的一个无效计数

    06_g:   1. 修改显示缓冲的相关变量

    06e:    1. 显示统一调用intpParaDisplay函数
            2. 考虑了表参数中 0*n 的情况

    06d:    1. 表显示中加入了横向显示"%th"表示

    06c:    1. 跳过了06a版
            2. 加入脚本复用的功能
               整个回显现实部分分为3层:
               第一层:脚本复用层
               第二层:脚本多分支处理层
               第三层:简单脚本处理层
            3. 在等参数制定符中采用了缺省规则

    06b:    1. 由05d版改成，没有脚本复用功能
            2. 主要考虑与测试后修改的display.c相统一

    05c:    1. 测试版，将显示结果输出到文件中
            2. 改变了一下测试脚本

    05b:    1. 测试版，修改了测试输入的脚本
            2. 修改了bug

    05a:    1. 正式支持多脚本，及支持类似switch命令

    04b:    1. 在字符串显示前加入了判断,避免系统崩溃
            2. 去掉了格式显示时的问题
            3. 去掉了一个if语句中的=，该为==
            4. 当表的一行显示超过80列时，左对齐时不在末尾加空格

    04a:    1. 去掉了一些令人混淆的代码和命名
            2. 部分代码的优化
            3. 全中文注释


    03f:    本版专门用于强壮性测试
            1. 加入了一些断言

    03e:    1. 在表头名中支持%%号

    03d:    1. 修改接口数据结构完全遵从msg.h
            2. 改正了长度为2，4的整形或浮点参数的回显问题
    
    03c:    1. 修改了表显示代码

    03b:    1. 加入了部分表的显示功能

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
    ST_NORMAL,                      /* 正常状态; 输出文字字符   */
    ST_PERCENT                     /* 刚读入了 '%'             */
/*    ST_TABLE,                       刚读入了表说明符       未引用到pclint报错，故删除  */
/*    ST_VALUE                        刚读入了参数位置说明符 未引用到pclint报错，故删除 */
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

/*下面数组下标要和oam_cfg.h中定义的类型一致*/
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
    0,                              /*DATA_TYPE_CONST, 回显中无定义*/
    0,                              /*DATA_TYPE_IFPORT, 回显中无定义*/
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

/*如果输出缓存满，发送到telnet显示先*/
static void SendToTelnetIfBufIsFull(TYPE_LINK_STATE * pLinkState, WORD32 dwAppendLen)
{
    TYPE_DISP_GLOBAL_PARA *pGlobal = NULL;
    DWORD dwReserverSize = 0;	
    if (!pLinkState)
    {
        return;
    }

    pGlobal = &(pLinkState->tDispGlobalPara);
    /*输出回显有的地方长度无法估算，
    给出512字节作为缓冲，避免越界*/
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
        *(pGlobal->pDisplayBuffer) = MARK_STRINGEND;    /* 初始化显示缓冲区 */
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
    函 数 名:   intpGetTextString
    功能描述:   获取由双引号扩起来的字符串
    输    入:
    输    出:   获取的字符串的长度
    返    回:
    断言错误:

    全局变量:
    注    释:

==============================================================================
    修改记录:
    修改日期        版本     修改人      修改原因及内容
==============================================================================
    2000\12\29  1.0             创建
------------------------------------------------------------------------------*/
static void intpGetTextString(CHAR **SouString, CHAR *destiString)
{
    CHAR *sourceString;
    sourceString = *SouString;
    /* 跳过非引号字符 */
    while (*sourceString != TEXT_MARK && *sourceString != MARK_STRINGEND)
        sourceString++;
    sourceString++;  /* 跳过TEXT_MARK 或 MARK_STRINGEND */

    while (*sourceString != TEXT_MARK && *sourceString != MARK_STRINGEND)
        *destiString++ = *sourceString++;
    *destiString++ = MARK_STRINGEND;
    *SouString  = sourceString + 1;  /* 跳过TEXT_MARK 或 MARK_STRINGEND */
}

/*------------------------------------------------------------------------------
    函 数 名:   intpGetFormatString
    功能描述:   获取字符串中的打印格式说明符
                说明符为：abcdiouxefgnpy!msAU
    输    入:
    输    出:
    返    回:
    断言错误:

    全局变量:
    注    释:   无错误返回，数字过大时，返回值可能不正确
                指向原字串的二级指针将会移动

==============================================================================
    修改记录:
    修改日期        版本     修改人      修改原因及内容
==============================================================================
    2000\10\16  1.0             创建
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
    函 数 名:   intpGetScriptNumber
    功能描述:   获取字符串中的十进制无符号长整数，遇到非数字字符退出
    输    入:   
    输    出:   
    返    回:   
    断言错误:   
                
    全局变量:   
    注    释:   
                
==============================================================================
    修改记录:
    修改日期        版本     修改人      修改原因及内容
==============================================================================
    2000\10\16  1.0             创建
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
    函 数 名:   intpGetStringByWidth
    功能描述:   原字符串按给定宽度、对齐方式生成目的字符串
    输    入:   CHAR *OrgString             源字符串
                CHAR *DetString             目标字符串
                BYTE    Width       目标字符串的长度
                INT     Style                  对齐方式：0,右对齐；-1,左对齐
    输    出:   无
    返    回:   无
                
    全局变量:   
    注    释:   源字符串不会被破坏
                不会导致越界
==============================================================================
    修改记录:
    修改日期        版本     修改人      修改原因及内容
==============================================================================
    2000\10\16  1.0             创建
------------------------------------------------------------------------------*/
static void intpGetStringByWidth(CHAR  *OrgString,     /* 原始字符串 */
                                 CHAR  *DetString,     /* 目标字符串 */
                                 WORD32 dwBufSize,/* 目标字符串 长度*/
                                 WORD32 Width,          /* 目标字符串显示的长度 */
                                 INT   Style           /* 对齐方式 */
                                 )
{
    static CHAR cTempString[MAX_DISPLAY_STRLEN];
    WORD32   ulOrgLenth;
    SWORD16 sCmp;
    
    ulOrgLenth = strlen(OrgString);
    sCmp = (WORD16)((WORD16)Width - (WORD16)ulOrgLenth);
    if (sCmp == 0)     /* 直接将原始字符串拷贝到目标字符串 */
    {
        strncpy(DetString, OrgString, dwBufSize);
    }
    else if (sCmp > 0) /* 需要在字符串前或串后加入空格 */
    {
        /* 做一个长度合适的空格字符串 */
        memset(cTempString, MARK_BLANK, (size_t)sCmp);
        cTempString[sCmp] = MARK_STRINGEND;

        if (Style == -1)
        {   /* 左对齐，在后加空格 */
            strncpy(DetString, OrgString, dwBufSize);
            strncat(DetString, cTempString, dwBufSize);
        }
        else
        {   /* 右对齐，在字符串前加入空格 */
            strncpy(DetString, cTempString, dwBufSize);
            strncat(DetString, OrgString, dwBufSize);
        }

    }
    else               /* 需要截断字符 */
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
    函 数 名:   intpParaDisplay
    功能描述:   单个简单参数的显示
    输    入:   CHAR * StrBuff                  显示结果存放缓冲区
                INT32 lBuffLen                  缓冲区的长度
                CHAR * StrFormat                显示格式说明
                BYTE * pParaBuffer              参量的入口地址
                WORD32 ulDataLen         参量的字节长度
                BYTE bType
    输    出:   显示缓冲区里放入回显字符串
    返    回:   输出的字符个数
                或错误代码 -1 字符串过长
                           -2 参量的字节长度与脚本说明符不符
    断言错误:   显示缓冲区的长度错误
                
    全局变量:   
    注    释:
==============================================================================
    修改记录:
    修改日期        版本     修改人      修改原因及内容
==============================================================================
    2000\00\00  1.0             创建
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
        /* 分为各种情况调用sprintf函数 */
        /*由于显示不了负数，这里按照是否%d分别处理，yinhao20081204*/
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
        {/*64位有符号整数*/
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
                /*大尾*/
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
    /*应用返回的是进度信息*/
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

            /* 将进度信息发送到telnet显示 */
            pLinkState->bOutputMode = OUTPUT_MODE_ROLL;
            pLinkState->wAppMsgReturnCode = SUCC_WAIT_AND_HAVEPARA;
         //   strncat(pLinkState->sOutputResult, StrBuff, MAX_OUTPUT_RESULT);

            /*如果进度是100，补充一个换行符*/
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
        /* 分为各种情况调用sprintf函数 */
        if (ulDataLen == 1)
        {
            BYTE value;
            memcpy(&value, pParaBuffer, (size_t)1);
            
            lDisplayLen = XOS_snprintf(StrBuff,lBuffLen,StrFormat,value);
        }
        else if (ulDataLen == 2)
        {
            WORD16 value;
            
            /*CRDCM00313101 用XOS_snprintf替换strcpy、strcat函数*/
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
            /*去掉小数点后无用的'0'*/
            DeleteNouseZero(StrBuff);
            lDisplayLen = strlen(StrBuff);
        }
        break;

    case 's':
        /* 字符串长度的合法性判断 */
        /* 由于表参数的原因，不能直接用LenthTemp == ulDataLen判断正确性 */
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
        /*物理地址和逻辑地址*/
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
   
        /*611000763178 ip地址的处理有字节序问题*/
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
	      /*收到的ip地址按照主机序解析*/
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
        /* A 改为IPV6 地址显示 modified by yinhao
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
    case 'Y':       /* 日期时间的显示 */
        {
            WORD32 dwDateTime;
            memcpy(&dwDateTime, pParaBuffer, sizeof(WORD32));
            DateTimeToStringAmericanStyle(&dwDateTime, TempValueStr, sizeof(TempValueStr));
            lDisplayLen = XOS_snprintf(StrBuff,lBuffLen,"%s", TempValueStr);
        }
        break;
    case 'y':       /* 日期的显示 */
        {
            WORD32 dwDate;
            memcpy(&dwDate, pParaBuffer, sizeof(WORD32));
            DateToStringAmericanStyle(&dwDate, TempValueStr, sizeof(TempValueStr));
            lDisplayLen = XOS_snprintf(StrBuff,lBuffLen,"%s", TempValueStr);
        }
        break;
    case 'm':       /* 时间的显示 */
        {
            WORD32 dwTime;
            memcpy(&dwTime, pParaBuffer, sizeof(WORD32));
            OamCfgTimeToString(&dwTime, TempValueStr,sizeof(TempValueStr));
            lDisplayLen = XOS_snprintf(StrBuff,lBuffLen,"%s", TempValueStr);
        }
        break;
    case 'w':       /* 64位无符号整数的显示 */
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
                /*大尾*/
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
                /*1.2.3.4 在内存里面从高到低是 4 3 2 1 */
                
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
               版本操作时间：
                  |31-20|19-16|15-11|10-6|5-0|
                       年     月   日    时   分
                例如：0x7DA7B2DD 为2010年7月22日11时29分
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
                版本制作日期：
               0x AABB CC DD
                   年   月 日
               例如：0x07DA040D  为2010年4月13日
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
    函 数 名:   intpSendToBuff_Normal_TableMode

    功能描述:   table模式下正常脚本的输出子程序，因在表中，所以去掉了表参数的显示
                没有静态文本的格式控制
    输    入:   pGlobal
    输    出:   全局性的显示缓冲区里放入回显字符串
    返    回:   错误代码
    全局变量:   
    注    释:
==============================================================================
    修改记录:
    修改日期        版本     修改人      修改原因及内容
==============================================================================
    2001\1\18  1.0             创建
------------------------------------------------------------------------------*/
static INT32 intpSendToBuff_Normal_TableMode(TYPE_LINK_STATE *pLinkState,TYPE_DISP_GLOBAL_PARA_ITM *pGlobal)
                                             /* 参数入口地址表已经改为全局 */
{
    CHAR *pParaBuffer;  /* 指向参数的指针 */
    CHAR *pFootScript;  /* 指向脚本当前的位置指针 */
    enum  STATE state;  /* 当前状态 */

    WORD32 ulDataLen;
    BYTE bType;

    /* 临时字符串，暂时只用在格式说明符的处理中 */
    static CHAR TempSpecifierString[MAX_DISPLAY_WIDTH];
    CHAR *pTempSpecifierString;

    WORD32 ulParameterLocation; /* 参数位置 */

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
                return SUCCESS_ENDWITHSWITCH;  /* 遇到%h返回 */
            case 'r': /* %r */
                pFootScript++;
                pGlobal->pFootScript = pFootScript;
                return SUCCESS_ENDWITHFOR;     /* 遇到%r返回 */
            case 't': /* %t */
                pFootScript++;
                pGlobal->pFootScript = pFootScript;
                return SUCCESS_ENDWITHTABLE;   /* 遇到%t返回 */
            case '%': /* %% */
                SendToTelnetIfBufIsFull(pLinkState, 1);
                *pGlobal->pDispBuff++ = *pFootScript++;
                state = ST_NORMAL;
                break;
            case 'v':
                /* 获取该位置 */
                pFootScript++;
                ulParameterLocation = intpGetScriptNumber(&pFootScript);
                if (ulParameterLocation == 0)
                    ulParameterLocation = pGlobal->giParameterLocation++;
                else
                    pGlobal->giParameterLocation = ulParameterLocation + 1;

                /* 位置合法 */
                if (ulParameterLocation > pGlobal->TableData.ItemNumber)
                {
                    DBG_ERR_LOG(FOOTSCRIPT_DATA_MISMATCH);
                    return DISPLAY_SUCCESS;
                }

                pParaBuffer = pGlobal->pParameterStartAddress[ulParameterLocation];
                pFootScript++;      

                /* 参数类型 */
                bType = pGlobal->TableData.Item[ulParameterLocation - 1].Type;
                /* 获取格式说明符，并显示 */
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
    函 数 名:   intpSendToBuffer_TableMode
    功能描述:   table模式下脚本的输出子程序，在此模式下只需要处理分支判断的情况
                不支持循环。
                直接使用正常模式下的程序段，为表内的每个参数建立入口地址表
    输    入:   
    输    出:   全局性的显示缓冲区里放入回显字符串
    返    回:   错误代码
    全局变量:   
    注    释:
==============================================================================
    修改记录:
    修改日期        版本     修改人      修改原因及内容
==============================================================================
    2001\1\18  1.0             创建
------------------------------------------------------------------------------*/
static INT32 intpSendToBuffer_TableMode(TYPE_LINK_STATE *pLinkState,
                                        TYPE_DISP_GLOBAL_PARA  *pGlobal, /* 保存所有全局变量的指针 */
                                        WORD32 iParaLocat                 /* 表参数的位置 */
                                        )
{
    static TYPE_DISP_GLOBAL_PARA_ITM global_InTableMode;
    CHAR *pFootScript;
    CHAR *pReservedScript;
    CHAR *pParaBufferTemp;
    BYTE ucTableField, ucTableItem;
    WORD32 iTemp;
    WORD16 wSwitchCounter;                    /* 对switch的嵌套进行计数 */
    POPR_DATA pTempData;
    PTABLE_DATA pTempTableData;

    global_InTableMode.giParameterLocation = 1;
    global_InTableMode.pDispBuff = pGlobal->pDispBuff;
    global_InTableMode.pDisplayBuffer = pGlobal->pDisplayBuffer;
    global_InTableMode.bSwitchStackTop = 0;
    pFootScript = pGlobal->pFootScript;
    pReservedScript = pFootScript;

    /* 取得该表的参数 */
    pTempData = (POPR_DATA)pGlobal->pParameterStartAddress[iParaLocat];
    ucTableItem = pTempData->Len;
    pTempTableData = (PTABLE_DATA)&(pTempData->Data[0]);
    ucTableField = pTempTableData->ItemNumber;
    /* 获取列参数类型及长度 */
    memcpy(&global_InTableMode.TableData,
            pTempTableData, sizeof(TABLE_DATA));
    pParaBufferTemp = (CHAR *)(&pTempTableData->Data[0]);
    for (;ucTableItem > 0; ucTableItem--)
    {
        /* 取得一行中每个参数的入口地址 */
        for (iTemp = 1; iTemp <= ucTableField; iTemp++)
        {
            global_InTableMode.pParameterStartAddress[iTemp] = pParaBufferTemp;

            /* 分三种情况计算下一个参数的位置 */
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
        /* *pFootScript指向表内脚本的起始位置 */
        /* 进入循环，若为'\0'则退出 */
        for (;;)
        {
            WORD16 wParaValue;
            WORD32 ulParameterLocation;

            if (*pFootScript == MARK_STRINGEND)
                goto ErrorInDis;
            /* 调用脚本多分支处理函数，该函数遇到'\0'或"%r"则返回 */
            global_InTableMode.pFootScript = pFootScript;
            switch (intpSendToBuff_Normal_TableMode(pLinkState,&global_InTableMode))
            {
                /* 返回说明是遇到'/0',则正常退出 */
            case SUCCESS_END:
                /* if need to do some free work ? */
                goto ErrorInDis;
                break;
            case SUCCESS_ENDWITHTABLE:
                pFootScript = global_InTableMode.pFootScript;
                goto BreakFromWhile;
                break;
            case SUCCESS_ENDWITHSWITCH:
                /* %h已经跳过 */
                pFootScript = global_InTableMode.pFootScript;
                switch (*pFootScript++)
                {
                case 's':
                    /* 得到参数号 */
                    /* 保存该参数的值 */
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
                    /* 判断值是否相等，分支条件是否成立 */
                    wParaValue = (WORD16)intpGetScriptNumber(&pFootScript);
                    DISPLAY_ASSERT(*pFootScript == ':');
                    pFootScript++;

                    if (global_InTableMode.sSwitchStack[global_InTableMode.bSwitchStackTop - 1].wParaValue == wParaValue)
                        continue;       /* while循环继续 */
                    /* 不等，则找到下一个%h,并越过中间的分支判断 */
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
                            /* %h已经找到了，可以跳出 */
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
                    /* 退栈 */
                    DISPLAY_ASSERT(global_InTableMode.bSwitchStackTop > 0);
                    global_InTableMode.bSwitchStackTop--;
                    break;
                default:
                    DBG_ERR_LOG(INVALID_FOOTSCRIPT);
                    goto ErrorInDis;
                    break;
                }   /* end of %h的分支 */
                break;
            case ERROR_INDIS:
                goto ErrorInDis;
                break;
            default:
                DBG_ERR_LOG(WRONG_RETURN_VALUE);
                goto ErrorInDis;
            }       /* end of 返回号码 */
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
    函 数 名:   intpSendToBuff_Normal
    功能描述:   正常脚本的输出子程序
    输    入:   pGlobal
    输    出:   全局性的显示缓冲区里放入回显字符串
    返    回:   错误代码
    全局变量:   pParameterStartAddress     参数入口表
                giParameterLocation        当前默认的参数位置
                pDisplayBuffer             显示缓冲区的头指针
    注    释:
==============================================================================
    修改记录:
    修改日期        版本     修改人      修改原因及内容
==============================================================================
    2000\07\21  1.0             创建
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

    enum  STATE state;        /* 当前状态 */
    D_FIELD_ITEM sTable;      /* 存放表参数的行列值 */
    static D_TABLE_FIELD sTableField[MAX_TABLE_FIELD + 1];  /* 记录表的域属性 */
    BYTE  ucTableFieldCounter;      /* 域的个数 */

    WORD32 ulParameterLocation;      /* 参数的位置 */
    CHAR *pFootScript;              /* 指向脚本的指针 */
    CHAR *pParaBuffer;              /* 指向参数的指针 */

    /* 临时变量 */
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
        case ST_PERCENT:  /* %状态 */
            switch (*pFootScript)
            {
            case 'h':  /* 遇到%h */
                pFootScript++;
                pGlobal->pFootScript = pFootScript;
                return SUCCESS_ENDWITHSWITCH;
                break;
            case 'r':  /* 遇到%r */
                pFootScript++;
                pGlobal->pFootScript = pFootScript;
                return SUCCESS_ENDWITHFOR;
                break;
            case 't':  /* 遇到%t */
                pFootScript++;
                pGlobal->pFootScript = pFootScript;
                return SUCCESS_ENDWITHTABLE;
                break;
            case '%':  /* 遇到%% */
                state = ST_NORMAL;  /* 清空状态机 */
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
                /* 获取该位置 */
                pFootScript++;
                ulParameterLocation = intpGetScriptNumber(&pFootScript);
                if (ulParameterLocation == 0)
                    ulParameterLocation = pGlobal->giParameterLocation++;
                else
                    pGlobal->giParameterLocation = ulParameterLocation + 1;

                /* 位置合法 */
                if (ulParameterLocation > pGlobal->dwNumber)
                {
                    DBG_ERR_LOG(FOOTSCRIPT_DATA_MISMATCH);
                    return DISPLAY_SUCCESS;
                }

                pParaBuffer = pGlobal->pParameterStartAddress[ulParameterLocation];
                pGlobal->ParaNo = ((POPR_DATA)pParaBuffer)->ParaNo;
                if (pGlobal->ParaNo == PARATOBECONTINUED)
                {
                    /* 只在表的回显中使用 */
                    /* 脚本逆向搜索到%v处 */
                    for (pGlobal->spFootScript = pFootScript - 1;
                        *(pGlobal->spFootScript) != '%';
                            pGlobal->spFootScript--)
                    {
                    };
                }
                pFootScript++;      

                if (*pFootScript == 't')
                {
                    /* 两种表的显示，所需的变量是相同的 */
                    PTABLE_DATA pTableData;
                    static CHAR *sDisplayArray[MAX_TABLE_ITEM + 1][MAX_TABLE_FIELD];
                    static CHAR sBuffer[DISPLAY_BUFFER_SIZE + 1000];/* 多余的1000存放表头名 */
                    static INT32 iDisplayWidth[MAX_TABLE_FIELD + 1];
                    CHAR *pBuffer;

                    state = ST_NORMAL;
                    pFootScript++;
                    DISPLAY_ASSERT(*pFootScript != '%' || *(pFootScript+1) != 't'); 
                                                         /* 假设不是 "%t%t" */
                                                         /* 表头或表头的回显格式必须指定*/
                    pOprData = (POPR_DATA)pParaBuffer;
                    pTableData = (PTABLE_DATA)&(pOprData->Data[0]);
                    pBuffer = (CHAR *)sBuffer;

                    /* 获取表参数的行列值 */
                    /* 此处的名称定义不太相同,msg.h 中的item相当于本程序中的field */
                    sTable.Item = pOprData->Len;
                    sTable.Field = pTableData->ItemNumber;
                    DISPLAY_ASSERT(sTable.Item <= MAX_TABLE_ITEM);
                    DISPLAY_ASSERT(sTable.Field <= MAX_TABLE_FIELD);

                    /* 获取表头名及列显示格式 */
                    ucTableFieldCounter = 0;

                    while (*pFootScript != '%' || *(pFootScript+1) != 't')
                    {
                        CHAR * pTemp;
                        ucTableFieldCounter++;

                        /* 1. 获取表头名 */
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
                        *pBuffer++ = MARK_STRINGEND;  /* 加入结束符 */

                        /* 2. 获取列格式指定 */
                        if (*(pFootScript + 1) == 't')
                        {
                            sTableField[ucTableFieldCounter].FieldFormation[0] = MARK_STRINGEND;
                            break;
                        }
                        pTemp = &sTableField[ucTableFieldCounter].FieldFormation[0];
                        intpGetFormatString(&pFootScript, &pTemp);
                    }

                    sTable.Field = ucTableFieldCounter; /* 以脚本中的内容为准 */
                    /* 将pFootScript移到下一个位置 */
                    pFootScript++;
                    pFootScript++;

                    /* 判断是横显还是竖显 */
                    if (*pFootScript != 'h')
                    {
                        /* 脚本指针移到下一个位置 */
                        pFootScript++;
                        pFootScript++;
                        /* 预显示，以求出所需的最小列打印宽度 */
                        /* 定义一个预显示矩阵 */

                        /* 预显示表头名 */
                        if (sTableField[1].FieldName[0] != 0)
                            for (jTemp = 1; jTemp <= sTable.Field; jTemp++)
                            {
                                sDisplayArray[0][jTemp] = sTableField[jTemp].FieldName;
                            }

                        /* 预显示所有的表元素，无须显示则跳过 */
                        pParaBuffer = (CHAR *)&(pTableData->Data[0]);

                        for (iTemp = 1; iTemp <= sTable.Item; iTemp++)
                            for (jTemp = 1; jTemp <= sTable.Field; jTemp++)
                            {
                                /* 类型检测 */
                                if (pTableData->Item[jTemp - 1].Type > DATA_TYPE_SYSCLOCK)
                                {
                                    DBG_ERR_LOG(INVALID_TYPE);
                                    return DISPLAY_SUCCESS;
                                }
                                /* 当前是否为字符串参数 */
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
                                /* 判断是否为LIST类型的表，且当前是否为字符串参数
                                   相应的修改参数指针 */
                                if (pOprData->Type == DATA_TYPE_LIST &&
                                    pTableData->Item[jTemp - 1].Type == DATA_TYPE_STRING)
                                    pParaBuffer += strlen(pParaBuffer) + 1;
                                else
                                    pParaBuffer += pTableData->Item[jTemp - 1].Len;
                            }

                        /* 获取格式说明符中指定的列显示宽度，并存放于数组iDisplayWidth */
                        for (jTemp = 1; jTemp <= sTable.Field; jTemp++)
                        {
                            CHAR * pTempFormation;
                            pTempFormation = sTableField[jTemp].FieldFormation;

                            /* 获取%到小数点之间的宽度指定，跳过可能的对齐说明符 */
                            DISPLAY_ASSERT(*pTempFormation == '%');
                            pTempFormation++;
                            if (*pTempFormation == '-')
                                pTempFormation++;
                            iDisplayWidth[jTemp] = intpGetScriptNumber(&pTempFormation);
                        }
                        /* 求出所需的最小宽度。格式中已经说明的则不考虑 */
                        /* 字符串要单独处理 */
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

                        /* 按对齐方式，合适的显示宽度显示预显示矩阵sDisplayArray中的内容 */
                        if (sTableField[1].FieldName[0] != 0) 
                            iTemp = 0;
                        else
                            iTemp = 1;
                        for (; iTemp <= sTable.Item; iTemp++)
                        {
                            INT cNowStyle;
                            for (jTemp = 1; jTemp <= sTable.Field; jTemp++)
                            {
                                if (sTableField[jTemp].FieldFormation[1] == '-')  /* 当前的对齐格式 */
                                    cNowStyle = -1;
                                else
                                    cNowStyle = 0;

                                if (cNowStyle == -1)
                                {   /* 左对齐 */
                                    /* 若为最后一列
                                       则左对齐时不考虑加空格*/
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
                                {       /* 右对齐 */
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
                            /* 去掉多显示的一个空格 加一个回车 */
                            *(pGlobal->pDispBuff - 1) = '\n';
                            SendToTelnetIfBufIsFull(pLinkState, 1);
                        }
                        /* 去掉多显示的一个\n */
                        *(--pGlobal->pDispBuff) = MARK_STRINGEND;
                    }
                    else
                    {
                        /* 表参数以水平方式打印 */
                        pParaBuffer = (CHAR *)&(pTableData->Data[0]);
                        SendToTelnetIfBufIsFull(pLinkState, 1);
                        for (iTemp = 1; iTemp <= sTable.Item; iTemp++)
                        {
                            for (jTemp = 1; jTemp <= sTable.Field; jTemp++)
                            {
                                strncpy(pGlobal->pDispBuff,
                                                    sTableField[jTemp].FieldName,
                                                    (WORD32)(MAX_OUTPUT_RESULT - DISPLAY_BUFFER_COUNTER)); /* 显示表头名 */
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
                                    /* pParaBuffer移到正确位置 */
                                    /* 判断是否为LIST类型的表，且当前是否为字符串参数
                                       相应的修改参数指针 */
                                    if (pOprData->Type == DATA_TYPE_LIST &&
                                        pTableData->Item[jTemp - 1].Type == DATA_TYPE_STRING)
                                        pParaBuffer += strlen(pParaBuffer) + 1;
                                    else
                                        pParaBuffer += pTableData->Item[jTemp - 1].Len;
                                }
                            }
                        /* 补一个回车 */
                        *pGlobal->pDispBuff++ = '\n';
                        *pGlobal->pDispBuff = MARK_STRINGEND;
                        SendToTelnetIfBufIsFull(pLinkState, 1);
                        }
                        /* 去掉多显示的一个\n */
                        *(--pGlobal->pDispBuff) = MARK_STRINGEND;
                        pFootScript++;

                    }  /* TABLE1型的表的横向显示结束*/
                }      /* 表参数显示处理结束        */
                else   /* 此处将不做格式说明符语法检查,直接交给sprintf */
                {
                    WORD32 ulDataLen;
                    BYTE bType;

                    pOprData = (POPR_DATA)pParaBuffer;
                    /* 获取ParaNo */
                    pGlobal->ParaNo = pOprData->ParaNo;

                    /* 参数类型 */
                    bType = (BYTE)pOprData->Type;
                    /*
                    change by wangmh for DATA_TYPE_MACADDR type display
                    if (bType > DATA_TYPE_IMASK)*/
                    if (bType > DATA_TYPE_PARA)/*DATA_TYPE_MACADDR*/
                    {
                        DBG_ERR_LOG(INVALID_TYPE);
                        return ERROR_INDIS;
                    }
                    /* 获取格式说明符，并显示 */
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
                    // 在表中，需要补一个回车 
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
    函 数 名:   OamCfgintpSendToBuffer
    功能描述:   脚本的输出子程序，外部调用的入口
    输    入:   MSG_COMM_OAM *ptMsg   参数空间入口地址
                CHAR * pFootScript    脚本的入口位置
                TYPE_DISP_GLOBAL_PARA *pGlobal 保存所有全局变量的指针
    输    出:   全局性的显示缓冲区里放入回显字符串
    返    回:   错误代码
    全局变量:   pParameterStartAddress     参数入口表
                giParameterLocation        当前默认的参数位置
                pDisplayBuffer             显示缓冲区的头指针
    注    释:
==============================================================================
    修改记录:
    修改日期        版本     修改人      修改原因及内容
==============================================================================
------------------------------------------------------------------------------*/
INT32 OamCfgintpSendToBuffer(TYPE_LINK_STATE *pLinkState,
                             MSG_COMM_OAM *ptMsg,           /* 参数空间入口地址 */
                             CHAR * pFootScriptEx,          /* 脚本的入口位置 */
                             TYPE_DISP_GLOBAL_PARA *pGlobal /* 保存所有全局变量的指针 */
                             )
{

    /* 获取每个参数的起始地址 每一个表参数认为是一个参数 */
    CHAR *pParaBuffer;
    POPR_DATA pTempData;
    CHAR *pParaBufferTemp;
    WORD16 wForCounter;                       /* 对for的嵌套进行计数 */
    WORD16 wSwitchCounter;                    /* 对switch的嵌套进行计数 */
    WORD16 wParaValue;
    WORD32 ulParameterLocation;
    CHAR *pFootScript = NULL;
   
    pFootScript = pFootScriptEx;
    pGlobal->giParameterLocation = 1;       /* 初始化参数位置 */
    pGlobal->bForStackTop = 0;              /* 初始化For栈 */
    pGlobal->bSwitchStackTop = 0;           /* 初始化Switch栈 */
    pGlobal->spFootScript = NULL;           /* 初始化脚本位置 */

    pGlobal->dwNumber = ptMsg->Number;
    *(pGlobal->pDisplayBuffer) = MARK_STRINGEND;    /* 初始化显示缓冲区 */
    pGlobal->pDispBuff = pGlobal->pDisplayBuffer;   /* 显示缓冲区当前位置 */
    pParaBuffer = (CHAR *)&(ptMsg->Data[0]);        /* 获取第一个参数的起始地址 */
    pParaBufferTemp = pParaBuffer;

    /* 将每一个参数的入口地址放入入口地址表中 */
    {
        WORD32 iTemp;
        PTABLE_DATA pTempTableData;
        WORD16 wLenOfData = 0; /* 数据部分的长度 */

        for (iTemp = 1; iTemp <= pGlobal->dwNumber; iTemp++)
        {
            /* 将每一个参数的入口地址放入入口地址表中 */
            pGlobal->pParameterStartAddress[iTemp] = pParaBufferTemp;

            pTempData = (POPR_DATA)pParaBufferTemp;

            /*下面检查是对CRDCM00206044 做的预案，这个单子怀疑是pTempData->Type访问时候出错*/
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
            /* 611000337784 应用返回的table类型记录数为0且OPR_DATA的data部分为0的时候，
            OamCfgintpSendToBuffer里面访问会越界 */
            wLenOfData = (WORD16)((BYTE *)pTempData - (BYTE *)ptMsg);
            wLenOfData = ptMsg->DataLen - wLenOfData;
            if (wLenOfData < sizeof(OPR_DATA))
            {
                DBG_ERR_LOG(INVALID_DATALEN);
                return DISPLAY_SUCCESS;
            }

            /* 分三种情况计算下一个参数的位置 */
            if (pTempData->Type == DATA_TYPE_TABLE)
            {
                WORD32 jTemp;
                INT32 iTempCounter = 0;

                pTempTableData = (PTABLE_DATA)&(pTempData->Data[0]);
                /* 611000337784 应用返回的table类型记录数为0且OPR_DATA的data部分为0的时候，
                OamCfgintpSendToBuffer里面访问会越界 */
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
                /* !!!当结构或编译选项改变时，此处一定要检验计算的正确性!!! */
                /* 此外，0*0的矩阵的长度计算用以上公式不正确 */

            }
            else if (pTempData->Type == DATA_TYPE_LIST)
            {
                CHAR * pTemp;
                INT32 kTemp, lTemp;

                pTempTableData = (PTABLE_DATA)&(pTempData->Data[0]);
                /* 611000337784 应用返回的table类型记录数为0且OPR_DATA的data部分为0的时候，
                OamCfgintpSendToBuffer里面访问会越界 */
                wLenOfData = (WORD16)((BYTE *)pTempTableData - (BYTE *)ptMsg);
                wLenOfData = ptMsg->DataLen - wLenOfData;
                if (wLenOfData < sizeof(TABLE_DATA))
                {
                    DBG_ERR_LOG(INVALID_DATALEN);
                    return DISPLAY_SUCCESS;
                }
                
                /* 跳过表头，指向数据 */
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
                                此处可以加入错误判断 
                                while (*pTemp++ != 0);    //一直跳过'\0'结尾符为止 
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
                
            /* 检验长度是否正确 */
            if ((WORD32)((CHAR *)pParaBufferTemp - (CHAR *)ptMsg) > ptMsg->DataLen)
            {
                SendStringToTelnet(pLinkState, map_szAppMsgDatalengthErr);
                DBG_ERR_LOG(INVALID_DATALEN);
                return DISPLAY_SUCCESS;
            }
        }
        /* 检验长度是否正确 */
        {
            if ((WORD32)((CHAR *)pParaBufferTemp - (CHAR *)ptMsg) > ptMsg->DataLen)
                DBG_ERR_LOG(INVALID_DATALEN);
        }
    }

    /*如果returncode大于ERR_AND_HAVEPARA且返回参数为map类型，修改回显脚本，
        目前只有brs这么用*/
/*    if(ptMsg->ExecModeID == MODE_PROTOCOL)*/
    {
            if((ptMsg->Data[0].Type == DATA_TYPE_MAP)&&
                (ptMsg->ReturnCode == ERR_AND_HAVEPARA ||ptMsg->ReturnCode == ERR_MSG_FROM_INTPR) &&
                (ptMsg->Number == 1))		    
            {
                /*XML专用链路*/
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

    /* * pFootScript已经指向脚本的起始位置 */
    /* 进入循环，若为'\0'则退出 */
    while (*pFootScript != MARK_STRINGEND)
    {
        /* 调用脚本多分支处理函数，该函数遇到'\0'或"%r"则返回 */
        pGlobal->pFootScript = pFootScript;
        switch (intpSendToBuff_Normal(pLinkState,pGlobal))
        {
        /* 返回说明是遇到'/0',则正常退出 */
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
            /* ParaNo 为255，记录下当前的脚本位置 */
            pParaBuffer = pGlobal->pParameterStartAddress[ulParameterLocation];
            pGlobal->ParaNo = ((POPR_DATA)pParaBuffer)->ParaNo;
            if (pGlobal->ParaNo == PARATOBECONTINUED)
            {
                /* 只在表的回显中使用 */
                /* 脚本逆向搜索到%v处 */
                for (pGlobal->spFootScript = pFootScript - 1;
                    *(pGlobal->spFootScript) != '%';
                        pGlobal->spFootScript--)
                {
                };
            }
            /* 在脚本中该冒号不能省略, 冒号是一个有效分割符 */
            DISPLAY_ASSERT(*pFootScript == ':'); 
            pFootScript++;
            /* 调用表模式下的回显函数 */
            pGlobal->pFootScript = pFootScript;
            intpSendToBuffer_TableMode(pLinkState, pGlobal, ulParameterLocation);
            pFootScript = pGlobal->pFootScript;
            if (pGlobal->ParaNo == PARATOBECONTINUED)
                goto BreakFromWhile;
            break;
        case SUCCESS_ENDWITHFOR:  /* 是%r，进行循环处理 */
            pFootScript = pGlobal->pFootScript;
            switch (*pFootScript++)
            {
            case 'e':  /* 是%re */
                DISPLAY_ASSERT(pGlobal->bForStackTop > 0); 
                if (pGlobal->sForStack[pGlobal->bForStackTop - 1].wForCounter <= 1)
                {
                    pGlobal->bForStackTop--;
                    break;
                }
                pGlobal->sForStack[pGlobal->bForStackTop - 1].wForCounter--;
                pFootScript = pGlobal->sForStack[pGlobal->bForStackTop - 1].pFootScript;
                break;
            case 's':  /* 是%rs */
                /* 得到参数号 */
                ulParameterLocation = intpGetScriptNumber(&pFootScript);
                if (ulParameterLocation == 0)
                    ulParameterLocation = pGlobal->giParameterLocation++;
                else
                    pGlobal->giParameterLocation = ulParameterLocation + 1;

                DISPLAY_ASSERT(ulParameterLocation <= pGlobal->dwNumber);
                /* 在脚本中该冒号不能省略，以免引起歧义 */
                DISPLAY_ASSERT(*pFootScript == ':'); 
                pFootScript++;
                /* 栈中已放入循环所需的次数 */
                pTempData = (POPR_DATA)pGlobal->pParameterStartAddress[ulParameterLocation];
                wParaValue = GetForCounterSwitchCase(pTempData);
                if (wParaValue <= 0)  /* 循环的次数小于1,跳过循环体 */
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
                                    pFootScript += 3;    /* 脚本跳到%rs后 */
                                    wForCounter++;
                                    continue;
                                    break;
                                case 'e':
                                    pFootScript += 3;    /* 脚本跳到%re后 */
                                    wForCounter--;
                                    if (wForCounter == 0)
                                        goto next2;      /* 与当前的%rs所对应的%re已经跳过 */
                                    continue;
                                default:
                                    DBG_ERR_LOG(INVALID_FOOTSCRIPT);
                                    break;
                                }
                            }
                        }
                        pFootScript++;
                    }
                    /* %h尚未找到，脚本结束，返回错误代码 */
                    DBG_ERR_LOG(INVALID_FOOTSCRIPT);
                    goto BreakFromWhile;
                }
                else
                {
                    pGlobal->sForStack[pGlobal->bForStackTop++].wForCounter = wParaValue;
                    pGlobal->sForStack[pGlobal->bForStackTop - 1].pFootScript = pFootScript;
                }
next2:          continue;  /* while循环继续 */
                break;
            default:
                DBG_ERR_LOG(INVALID_FOOTSCRIPT);
                goto BreakFromWhile;
                break;
            }
            break;
        case SUCCESS_ENDWITHSWITCH:
            /* %h已经跳过 */
            pFootScript = pGlobal->pFootScript;
            switch (*pFootScript++)
            {
            case 's':
                /* 得到参数号 */
                /* 保存该参数的值 */
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
                /* 判断值是否相等, 分支条件是否成立 */
                wParaValue = (WORD16)intpGetScriptNumber(&pFootScript);
                DISPLAY_ASSERT(*pFootScript == ':');
                pFootScript++;

                /* 相等, while循环继续 */
                if (pGlobal->sSwitchStack[pGlobal->bSwitchStackTop - 1].wParaValue == wParaValue)
                    continue; 

                /* 不等, 则找到下一个%h, 并越过中间的分支判断 */
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
                        /* %h已经找到了，可以跳出 */
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
                /* 退栈 */
                DISPLAY_ASSERT(pGlobal->bSwitchStackTop > 0);
                pGlobal->bSwitchStackTop--;
                break;
            default:
                DBG_ERR_LOG(INVALID_FOOTSCRIPT);
                goto BreakFromWhile;
                break;
            }   /* end of %h的分支 */
            break;
        case SUCCESS_CONTINUETONEXTPACKET:
            goto BreakFromWhile;
            break;
        case ERROR_INDIS:
            goto BreakFromWhile;
            break;
        default: /* 目前只有 return DISPLAY_SUCCESS; 会到这里*/
            DBG_ERR_LOG(WRONG_RETURN_VALUE);
            goto BreakFromWhile;
        }  /* end of 返回号码 */
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

