/*------------------------------------------------------------------------------
    2001/12/14
    by Wangwei
    version 01a
    history:
------------------------------------------------------------------------------*/
#include "includes.h"

static BYTE mul_str[]={4,2,9,4,9,6,7,2,9,6,0};

/*lint -e722*/
/*------------------------------------------------------------------------------
    函 数 名:   oam64bit_ConvertLongToArray
    功能描述:   将一个>=0,<2^32长整型数转化为一个字符串
    输    入:   INT32 lNum         需要转化的长整数,>=0,<2^32
                INT *pArray     字符串缓冲区,长为21
    输    出:   缓冲区里放入转化好的字符串，缓冲区的内容被覆盖
                第20位为最低位。最高位前加-1
    返    回:   
    全局变量:   
                
    注    释:
==============================================================================
    修改记录:
    修改日期        版本     修改人      修改原因及内容
==============================================================================
    2000\07\21  1.0             创建
------------------------------------------------------------------------------*/
static void oam64bit_ConvertLongToArray20(UINT32 lNum, INT *pArray)
{
    BYTE cLen;
    cLen = 20;
    if (lNum == 0)
    {
        pArray[20] = 0;
        pArray[19] = -1;
    }
    else
    {
        while (lNum != 0)
        {
            pArray[cLen--] = (BYTE)(lNum % 10);
            lNum = (WORD32)(lNum / 10);
        }
        pArray[cLen] = -1;

    }
}

/*------------------------------------------------------------------------------
    函 数 名:   oam64bit_Add
    功能描述:   64位加法
    输    入:   
                
    输    出:   
                
    返    回:   
    全局变量:   
                
    注    释:
================================================================================
    修改记录:
    修改日期        版本     修改人      修改原因及内容
================================================================================
    2000\07\21  1.0             创建
------------------------------------------------------------------------------*/
static void oam64bit_Add(INT *pArray1, INT *pArray2)
{
    BYTE cBitCount, cTempCount;
    cBitCount = 20;
    for (; pArray2[cBitCount] != -1; cBitCount--)
    {
        if (pArray1[cBitCount] == -1)
        {
            pArray1[cBitCount] = pArray2[cBitCount];
            pArray1[cBitCount - 1] = -1;
        }
        else
        {
            pArray1[cBitCount] += pArray2[cBitCount];
            for (cTempCount = cBitCount; pArray1[cTempCount] >= 10; cTempCount--)
            {
                pArray1[cTempCount] -= 10;
                if (pArray1[cTempCount - 1] >= 0)
                {
                    pArray1[cTempCount - 1] += 1;
                }
                else
                {
                    pArray1[cTempCount - 1] = 1;
                    pArray1[cTempCount - 2] = -1;
                    break;
                }
            }
        }
    }
}



/*------------------------------------------------------------------------------
    函 数 名:   oam64bit_Mutiple
    功能描述:   64位乘法（结果不能超过64位）
    输    入:   
                
    输    出:   
                
    返    回:   
    全局变量:   
                
    注    释:
================================================================================
    修改记录:
    修改日期        版本     修改人      修改原因及内容
================================================================================
    2000\07\21  1.0             创建
------------------------------------------------------------------------------*/
static void oam64bit_Mutiple(INT *pArray1, INT *pArray2)
{
    /* cTempNum1为累加器，cTempNum2为临时乘法器 */
    INT cTempNum1[21], cTempNum2[21];
    BYTE cBitCount1, cBitCount2, cBitMultiple, cTemp;
    memset(cTempNum1, -1, 20);
    cTempNum1[20] = 0;
    /* 将pArray2中的数一位位的乘pArray1放在cTempNum2中 */
    for (cBitCount2 = 20; cBitCount2 >=1 && pArray2[cBitCount2] >= 0; cBitCount2--)
    {
        if (pArray2[cBitCount2] == 0)
            continue;
        memset(cTempNum2, -1, 20);
        cTempNum2[20] = 0;
        for (cTemp = 20; cTemp > cBitCount2; cTempNum2[cTemp--] = 0);
        for (cBitCount1 = 20; cBitCount1 >=1 && pArray1[cBitCount1] >= 0; cBitCount1--)
        {
            CHAR digital1, digital2;
            BYTE cBitCount3;
            cBitCount3 = cBitCount1 + cBitCount2 - 20;
            cBitMultiple = pArray1[cBitCount1] * pArray2[cBitCount2];
            digital1 = cBitMultiple / 10;
            digital2 = cBitMultiple % 10;
            if (cTempNum2[cBitCount3] == -1)
                cTempNum2[cBitCount3] = cBitMultiple % 10;
            else
                cTempNum2[cBitCount3] += cBitMultiple % 10;
            if (cTempNum2[cBitCount3] >= 10)
            {
                if (cTempNum2[cBitCount3 - 1] == -1)
                    cTempNum2[cBitCount3 - 1] = 1;
                else
                    cTempNum2[cBitCount3 - 1] += 1;
                cTempNum2[cBitCount3] -= 10;
            }
            if (digital1 != 0)
            {
                if (cTempNum2[cBitCount3 - 1] == -1)
                    cTempNum2[cBitCount3 - 1] = digital1;
                else
                    cTempNum2[cBitCount3 - 1] += digital1;
            }
        }   /* end of 一位数乘法 */
        oam64bit_Add(cTempNum1, cTempNum2);
    }
    memcpy(pArray1, cTempNum1, 21);     /* 结果放入被乘数中 */
}

/*------------------------------------------------------------------------------
    函 数 名:   oam64bit_ConvertToString
    功能描述:   将64位的数转化为字符串
    输    入:   
                
                
    输    出:   
    返    回:   
                
    全局变量:   
                
    注    释:   
                
                
================================================================================
    修改记录:
    修改日期        版本     修改人      修改原因及内容
==============================================================================
    2000\07\21  1.0             创建
------------------------------------------------------------------------------*/
static void oam64bit_ConvertToString(INT *pArray, CHAR *pResultString)
{
    INT cCount;
    for (cCount = 0; *pArray < 0 && cCount <= 20; pArray++, cCount++);
    for (; cCount <= 20;*pResultString++ = *pArray++ + '0', cCount++);
    *pResultString = 0;
}

/*------------------------------------------------------------------------------
    函 数 名:   OamCfgintpGetStringFrom64bit
    功能描述:   将一个64位整数化位10进制的字符串
    输    入:   &ParameterDimension 参数空间入口地址
                iOperation          操作类型
                ppDispBuf
    输    出:   全局性的显示缓冲区里放入回显字符串
    返    回:   错误代码
                0: 成功
    全局变量:   
                
    注    释:   分别将高32位与低32位化为10进制放于数组内，
                再用十进制乘法及加法函数运算，以将64bit数
                化为十进制
================================================================================
    修改记录:
    修改日期        版本     修改人      修改原因及内容
==============================================================================
    2000\07\21  1.0             创建
------------------------------------------------------------------------------*/
INT32 OamCfgintpGetStringFrom64bit(UINT32 Low32bit,       /* 低32位 */
                           UINT32 High32bit,        /* 高32位 */
                           CHAR * pResultString         /* 结果 */
                           )
{
    /* 数字排列从左到右，第20位为最低位。无效部分放-1 */
    INT low32[21], high32[21];
    INT const2exp32[21] =
            {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,4,2,9,4,9,6,7,2,9,6};
    memset(low32, -1, 20);
    memset(high32, -1, 20);
    oam64bit_ConvertLongToArray20(Low32bit  , low32);
    oam64bit_ConvertLongToArray20(High32bit , high32);
    if (High32bit != 0)
    {
        oam64bit_Mutiple(high32, const2exp32);
    }
    oam64bit_Add(high32, low32);
    oam64bit_ConvertToString(high32, pResultString);
    return OAM64bit_SUCCESS;
}

/*------------------------------------------------------------------------------
    函 数 名:   int2Str_Of64bit
    功能描述:   将一个64位整数转换10进制的字符串
    输    入:   dwHigh     64位整数中的高32位
                dwLow      64位整数中的低32位
    输    出:   pResult    长度为20个字符的十进制表示的字符串
    返    回:   错误代码
                0: 成功
    全局变量:   
                
    注    释:   分别将高32位与低32位化为10进制放于数组内，
                再用十进制乘法及加法函数运算，以将64bit数
                化为十进制
================================================================================
    修改记录:
    修改日期        版本     修改人      修改原因及内容
==============================================================================
    2009\12\02      1.0      傅龙锁       创建
------------------------------------------------------------------------------*/
BYTE int2Str_Of64bit(WORD32 dwHigh, WORD32 dwLow, CHAR *pResult)
{
    CHAR tmp_str[11];
    BYTE i, j, k, inc, tmp;
    //high * 2^32

    memset(pResult, 0, 21);
    XOS_snprintf(tmp_str, sizeof(tmp_str),"%010u", dwHigh);
    for(i = 0; i < 10; i++)
        tmp_str[i] -= '0';

    for(i = 10; i > 0; i--)
    {
        for(j = 10; j > 0; j--)
        {
            k = i + j - 1;
            inc = 0;
            tmp = tmp_str[i - 1] * mul_str[j - 1] + pResult[k];
            inc = tmp / 10;
            pResult[k] = tmp % 10;   
            while(inc != 0)
            {
                k--;
                tmp = pResult[k] + inc;
                pResult[k] = tmp % 10;
                inc = tmp / 10;
            } 
        } 
    }
  
    // result + low
    XOS_snprintf(tmp_str, sizeof(tmp_str),"%010u", dwLow);
    for(i = 0; i < 10; i++)
        tmp_str[i] -= '0';  

    inc = 0;
    for(i = 10; i > 0; i--)
    {
        tmp = pResult[i + 9] + tmp_str[i - 1] + inc; 
        inc = tmp / 10;
        pResult[i + 9] = tmp % 10;   
    }

    k = 10;
    while(inc != 0)
    {
        k--;
        tmp  = pResult[k] + inc;
        inc = tmp / 10;
        pResult[k] = tmp % 10;
    } 
  
    //remove addtional 0s
    for(i = 0;i < 19; i++)
        if(pResult != 0)
            break;

    for(j = i; j < 20; j++)
    {
        pResult[j - i] = pResult[j] + '0';
    }
    pResult[20 - i] = 0;
    return (20 - i);
    
}

/*------------------------------------------------------------------------------
    函 数 名:   getWord64Str
    功能描述:   将一个10进制的字符串中的多余的0去掉
    输    入:   pInput     十进制的表示64位的字符串
    输    出:   pResult    去掉多余的0之后的字符串
    返    回:   0: 失败
                1: 成功
    全局变量:   
                
    注    释:   
================================================================================
    修改记录:
    修改日期        版本     修改人      修改原因及内容
==============================================================================
    2009\12\02      1.0      傅龙锁       创建
------------------------------------------------------------------------------*/
BYTE getWord64Str(CHAR *pInput, CHAR *pResult)
{
    CHAR tmpStr[80];
    CHAR *pTmpStr = NULL;
    int i = 0;
    BYTE bFlg = 0;
    int nInputLen = 0;

    if (pInput == NULL || pResult == NULL)
        return 0;

    memset(tmpStr, 0, 80);
    pTmpStr = tmpStr;
    nInputLen = strlen(pInput);
    
    for (i = 0; i< nInputLen; i++)
    {
        if (pInput[i] != '0' && bFlg == 0)
            bFlg = 1;
        
        if (bFlg == 1)
        {
            *pTmpStr = pInput[i];
            pTmpStr++;
        }    
    }
    
    if (bFlg == 0)
        tmpStr[0] = '0';
     
    memcpy(pResult, tmpStr, strlen(tmpStr));

    return 1;
}


/*lint +e722*/


 
