/*********************************************************************
* ��Ȩ���� (C)2008, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ� 
* �ļ���ʶ�� 
* ����ժҪ�� 
* ����˵���� 
* ��ǰ�汾�� V1.0
* ��    �ߣ�
* ������ڣ� 
**********************************************************************/

/***********************************************************
 *                      ͷ�ļ�                             *
***********************************************************/

#include "includes.h"

/***********************************************************
 *                     ȫ�ֱ���                            *
***********************************************************/
/*
    ģʽ��¼������
    ʹ�ó���Ϊ100�������¼ģʽ�ڵ㡣���У�����ĵ�һ��Ԫ�ؽڵ㲻ʹ�á�
    ��ˣ����Լ�¼99��ģʽ��Ϣ��
    g_dwModeCount��¼��Ч��ģʽ������
    ������Чģʽ��Ϣ��¼�������ǰ�ˡ����磬g_dwModeCountΪ10��ʱ��
        gtMode[0]Ϊ��Чֵ(��Զ��Ч)
        gtMode[1]-gtMode[10]Ϊ��Чģʽ��Ϣ
        gtMode[11]-gtMode[99]Ϊδʹ��Ԫ��
    ��������ĵ�һ��Ԫ�ز�ʹ�ã����һ����Чģʽ���±�Ϊg_dwModeCount

    ɾ��ģʽʱ��Ϊ�˱�֤ɾ������Чģʽ��Ȼ�������ǰ�ˣ�
    ʹ�����һ����Чģʽ��Ϣ(gtMode[g_dwModeCount])������Ҫɾ����ģʽ���ɡ�
    ���磬������������ӣ�ɾ���±�Ϊ6��ģʽ����ʹ��gtMode[10]����gtMode[6]��
        Ȼ��g_dwModeCount��1��
*/

DWORD g_dwModeCount = 0;
TYPE_MODE *gtMode = NULL;

/***********************************************************
 *                     ���� ����                           *
***********************************************************/
/***********************************************************
 *                     ��������                            *
***********************************************************/

/***********************************************************
 *                     �궨��                              *
***********************************************************/



/************************************************************************
* �������ƣ�
* ���������� ����ģʽ�ڵ�
* ��������� ģʽ��Ϣָ��
* �� �� ֵ�� ���ڵ��±꣬0��ʾ����ʧ��
* ����˵���� 
* �޸�����      �汾��  �޸���      �޸�����
************************************************************************/
WORD16 OAM_CFGAppendMode(TYPE_MODE * mode, BOOLEAN bRestore)
{
    if (!bRestore)
    {
        WORD32 dwMaxModeCnt = OAM_CFGGetModeCntInAllDat();
        /* CRDCM00025960 OMP������ε�������OAM_Interpret�쳣�����Ҵ�ӡ����������ʧ�� */
        /* ���ȫ�������Ƿ���ʣ�µĿռ� */
        if (((g_dwModeCount + 1) >= dwMaxModeCnt) || (g_wFreeNode == 0))
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR, "g_dwModeCount[%d] is big than max[%d]\n", g_dwModeCount, dwMaxModeCnt);
            return 0;
        }
    }

    {
        DWORD index;
        index = OAM_CFGSearchMode(mode->bModeID);

        /* ��������ģʽ�ڵ㣬��ʾ��һ���µ�ģʽ����Ҫ����ýڵ� */
        if (index == 0)
        {
            /* ����ģʽ�ڵ� */
            /* ע���һ���ڵ㲻ʹ�ã������ʹg_dwModeCount����1�� */
            g_dwModeCount++;
            gtMode[g_dwModeCount] = *mode;
            gtMode[g_dwModeCount].wTreeRootPos = 0;
            index = g_dwModeCount;
        }

        if (gtMode[index].wTreeRootPos == 0)
        {
            /* �����ģʽ��Ӧ�����ڵ� */
            {
                /* ȡ��һ���ڵ� */
                WORD16 node = g_wFreeNode;
                g_wFreeNode = gtTreeNode[g_wFreeNode].treenode.wNextNodePos;
                memset(&gtTreeNode[node], 0, sizeof(gtTreeNode[node]));

                /* ģʽ��Ӧ�����ϵĽڵ㣬���е����ƺ�ע����Ϣ�������Ϲ۲첻�������Բ����� */

                /* ���Ӹ����� */
                gtMode[index].wTreeRootPos = node;

                /* ģʽ��Ӧ�����ϵĽڵ㣬֮��ġ��ֵܡ���(wNextNodePos)��û��ʵ�����壬��˲����� */
#if 0
                /* �������ڵ㣬���뵽��һ��mode��Ӧ�Ľڵ�֮ǰ */
                if (g_dwModeCount != 0)
                {
                    g_wFreeNode = gtTreeNode[node].treenode.wNextNodePos = gtMode[1].wTreeRootPos;
                }
#endif
                return node;
            }
        }
        else
        {
            return gtMode[index].wTreeRootPos;
        }
    }
}

/************************************************************************
* �������ƣ�
* ���������� ɾ��ģʽ�ڵ�
* ��������� ģʽID
* �� �� ֵ�� ��
* ����˵���� 
* �޸�����      �汾��  �޸���      �޸�����
************************************************************************/
void OAM_CFGDeleteMode(BYTE modeID)
{
    /* ����ģʽ */
    int index = OAM_CFGSearchMode(modeID);
    if ((index != 0) && (gtMode[index].wTreeRootPos != 0))
    {
        /* ���ģʽ�»�������ʱ���Ͳ���ɾ����ģʽ */
        if (gtTreeNode[gtMode[index].wTreeRootPos].treenode.wNextSonPos == 0)
        {
            /* ɾ��ģʽ�ķ�ʽ��ʹ�����һ����Чģʽ�����滻��ģʽ���� */
            /* ע���һ���ڵ㲻ʹ�� */
            gtMode[index] = gtMode[g_dwModeCount];
            memset(&gtMode[g_dwModeCount+1], 0, sizeof(gtMode[g_dwModeCount+1]));
            g_dwModeCount--;
        }
        else
        {
            /* �����������ɾ�� */
            return;
        }
    } 
}

/************************************************************************
* �������ƣ�
* ���������� ����ģʽ�ڵ�
* ��������� ģʽID
* �� �� ֵ�� ģʽ��Ϣ��gtMode�����е��±꣬δ���ҵ��򷵻�0
* ����˵���� 
* �޸�����      �汾��  �޸���      �޸�����
************************************************************************/
DWORD OAM_CFGSearchMode(BYTE modeID)
{
    DWORD index;
    for (index=1;index<=g_dwModeCount;index++)
    {
        if (gtMode[index].bModeID == modeID)
            break;
    }

    if (index<=g_dwModeCount)
        return index;
    else
        return 0;
}

void OAM_CFGPrintgtMode(void)
{
    DWORD index;
    for (index=1;index<=g_dwModeCount;index++)
    {
        printf("%d:\n",index);
        printf("bModeID=%d:\n",gtMode[index].bModeID);
        printf("wModeClass=%d:\n",gtMode[index].wModeClass);
        printf("wTreeRootPos=%d:\n",gtMode[index].wTreeRootPos);
        printf("bHasNo=%d:\n",gtMode[index].bHasNo);
        printf("sOddPrompt=%s:\n",gtMode[index].sOddPrompt);
        printf("sModeHelp=%s:\n",gtMode[index].sModeHelp);
    }    
}

