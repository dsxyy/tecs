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
    �� �� ��:   oam64bit_ConvertLongToArray
    ��������:   ��һ��>=0,<2^32��������ת��Ϊһ���ַ���
    ��    ��:   INT32 lNum         ��Ҫת���ĳ�����,>=0,<2^32
                INT *pArray     �ַ���������,��Ϊ21
    ��    ��:   �����������ת���õ��ַ����������������ݱ�����
                ��20λΪ���λ�����λǰ��-1
    ��    ��:   
    ȫ�ֱ���:   
                
    ע    ��:
==============================================================================
    �޸ļ�¼:
    �޸�����        �汾     �޸���      �޸�ԭ������
==============================================================================
    2000\07\21  1.0             ����
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
    �� �� ��:   oam64bit_Add
    ��������:   64λ�ӷ�
    ��    ��:   
                
    ��    ��:   
                
    ��    ��:   
    ȫ�ֱ���:   
                
    ע    ��:
================================================================================
    �޸ļ�¼:
    �޸�����        �汾     �޸���      �޸�ԭ������
================================================================================
    2000\07\21  1.0             ����
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
    �� �� ��:   oam64bit_Mutiple
    ��������:   64λ�˷���������ܳ���64λ��
    ��    ��:   
                
    ��    ��:   
                
    ��    ��:   
    ȫ�ֱ���:   
                
    ע    ��:
================================================================================
    �޸ļ�¼:
    �޸�����        �汾     �޸���      �޸�ԭ������
================================================================================
    2000\07\21  1.0             ����
------------------------------------------------------------------------------*/
static void oam64bit_Mutiple(INT *pArray1, INT *pArray2)
{
    /* cTempNum1Ϊ�ۼ�����cTempNum2Ϊ��ʱ�˷��� */
    INT cTempNum1[21], cTempNum2[21];
    BYTE cBitCount1, cBitCount2, cBitMultiple, cTemp;
    memset(cTempNum1, -1, 20);
    cTempNum1[20] = 0;
    /* ��pArray2�е���һλλ�ĳ�pArray1����cTempNum2�� */
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
        }   /* end of һλ���˷� */
        oam64bit_Add(cTempNum1, cTempNum2);
    }
    memcpy(pArray1, cTempNum1, 21);     /* ������뱻������ */
}

/*------------------------------------------------------------------------------
    �� �� ��:   oam64bit_ConvertToString
    ��������:   ��64λ����ת��Ϊ�ַ���
    ��    ��:   
                
                
    ��    ��:   
    ��    ��:   
                
    ȫ�ֱ���:   
                
    ע    ��:   
                
                
================================================================================
    �޸ļ�¼:
    �޸�����        �汾     �޸���      �޸�ԭ������
==============================================================================
    2000\07\21  1.0             ����
------------------------------------------------------------------------------*/
static void oam64bit_ConvertToString(INT *pArray, CHAR *pResultString)
{
    INT cCount;
    for (cCount = 0; *pArray < 0 && cCount <= 20; pArray++, cCount++);
    for (; cCount <= 20;*pResultString++ = *pArray++ + '0', cCount++);
    *pResultString = 0;
}

/*------------------------------------------------------------------------------
    �� �� ��:   OamCfgintpGetStringFrom64bit
    ��������:   ��һ��64λ������λ10���Ƶ��ַ���
    ��    ��:   &ParameterDimension �����ռ���ڵ�ַ
                iOperation          ��������
                ppDispBuf
    ��    ��:   ȫ���Ե���ʾ���������������ַ���
    ��    ��:   �������
                0: �ɹ�
    ȫ�ֱ���:   
                
    ע    ��:   �ֱ𽫸�32λ���32λ��Ϊ10���Ʒ��������ڣ�
                ����ʮ���Ƴ˷����ӷ��������㣬�Խ�64bit��
                ��Ϊʮ����
================================================================================
    �޸ļ�¼:
    �޸�����        �汾     �޸���      �޸�ԭ������
==============================================================================
    2000\07\21  1.0             ����
------------------------------------------------------------------------------*/
INT32 OamCfgintpGetStringFrom64bit(UINT32 Low32bit,       /* ��32λ */
                           UINT32 High32bit,        /* ��32λ */
                           CHAR * pResultString         /* ��� */
                           )
{
    /* �������д����ң���20λΪ���λ����Ч���ַ�-1 */
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
    �� �� ��:   int2Str_Of64bit
    ��������:   ��һ��64λ����ת��10���Ƶ��ַ���
    ��    ��:   dwHigh     64λ�����еĸ�32λ
                dwLow      64λ�����еĵ�32λ
    ��    ��:   pResult    ����Ϊ20���ַ���ʮ���Ʊ�ʾ���ַ���
    ��    ��:   �������
                0: �ɹ�
    ȫ�ֱ���:   
                
    ע    ��:   �ֱ𽫸�32λ���32λ��Ϊ10���Ʒ��������ڣ�
                ����ʮ���Ƴ˷����ӷ��������㣬�Խ�64bit��
                ��Ϊʮ����
================================================================================
    �޸ļ�¼:
    �޸�����        �汾     �޸���      �޸�ԭ������
==============================================================================
    2009\12\02      1.0      ������       ����
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
    �� �� ��:   getWord64Str
    ��������:   ��һ��10���Ƶ��ַ����еĶ����0ȥ��
    ��    ��:   pInput     ʮ���Ƶı�ʾ64λ���ַ���
    ��    ��:   pResult    ȥ�������0֮����ַ���
    ��    ��:   0: ʧ��
                1: �ɹ�
    ȫ�ֱ���:   
                
    ע    ��:   
================================================================================
    �޸ļ�¼:
    �޸�����        �汾     �޸���      �޸�ԭ������
==============================================================================
    2009\12\02      1.0      ������       ����
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


 
