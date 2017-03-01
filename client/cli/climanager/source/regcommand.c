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
    ���ڵ���Ϣ������

    ���ڵ���Ϣʹ�ö����������ʾ
    ����ʹ�������±��ʾ��
    ��������ʹ��wNextNodePos�ֶ����ӣ���������ͷΪg_wFreeNode
    ��¼���ڵ���Ϣ������Ԫ�أ�ʹ��wNextNodePos�ֶα�ʾ��һ���ֵܽڵ�
    ʹ��wNextSonPos�ֶα�ʾ��һ���ӽڵ�

    �������ڵ��ģʽ�ڵ���������˲���Ҫ����ͷ��

    ���ڵ�ĵ�һ��Ԫ�ز�ʹ��
*/
WORD16 g_wFreeNode = 0;
T_TreeNode *gtTreeNode = NULL;

/***********************************************************
 *                     ���� ����                           *
***********************************************************/
/***********************************************************
 *                     ��������                            *
***********************************************************/
static void _DeleteChildNodes(WORD16 * nextptr, T_OAM_Cfg_Dat* cfgDat, int calllevel);
/***********************************************************
 *                     �궨��                              *
***********************************************************/

/************************************************************************
* �������ƣ�
* ���������� ��ʼ�����ڵ�����
* ��������� 
* �� �� ֵ�� 
* ����˵���� 
* �޸�����      �汾��  �޸���      �޸�����
************************************************************************/
void OAM_CFGInitTreeNode(void)
{
    WORD32 i = 0;
	
    if (g_ptOamIntVar->dwTreeNodeSum < 1)
        return;

    /* ��ʼ�������� */
    memset(gtTreeNode, 0, sizeof(T_TreeNode) * g_ptOamIntVar->dwTreeNodeSum);

    /* �����������ע����±�1��ʼ */
    for (i=1;i < g_ptOamIntVar->dwTreeNodeSum; i++)
    {
        gtTreeNode[i].treenode.wNextNodePos = i+1;
    }
    gtTreeNode[g_ptOamIntVar->dwTreeNodeSum-1].treenode.wNextNodePos = 0;

    /* ��ʼ�����б�ͷ */
    g_wFreeNode = 1;
}

void OAM_CFGInitTreeNodeForRestore(WORD32 dwTreeNodeSum)
{
    WORD32 i = 0;
	
    if (dwTreeNodeSum < 1)
        return;
	
    /* ��ʼ�������� */
    memset(gtTreeNode, 0, sizeof(T_TreeNode) * g_ptOamIntVar->dwTreeNodeSum);

    /* �����������ע����±�1��ʼ */
    for (i=1;i < dwTreeNodeSum; i++)
    {
        gtTreeNode[i].treenode.wNextNodePos = i+1;
    }
    gtTreeNode[dwTreeNodeSum-1].treenode.wNextNodePos = 0;

    /* ��ʼ�����б�ͷ */
    g_wFreeNode = 1;

    return;
}


/************************************************************************
* �������ƣ�
* ���������� �������ڵ�
* ��������� 
            ���ڵ��±�,     parentnode
            ���ڵ���Ϣ,     treenode
            DAT������,      datptr
* �� �� ֵ�� ��������ڵ��±�
* ����˵���� 
* �޸�����      �汾��  �޸���      �޸�����
************************************************************************/
WORD16 OAM_CFGAppendChildNode(WORD16 parentnode, TYPE_TREE_NODE * treenode, T_OAM_Cfg_Dat* cfgDat)
{
    if ((g_wFreeNode == 0) || (parentnode == 0))
    {
        return 0;
    }
    else
    {
        /* ȡ��һ���ڵ� */
        WORD16 node = g_wFreeNode;
        g_wFreeNode = gtTreeNode[g_wFreeNode].treenode.wNextNodePos;

        {
            /* ���� */
            WORD16 childnode = gtTreeNode[parentnode].treenode.wNextSonPos;
            WORD16 prionode = 0;

            while (childnode != 0)
            {
                /* �ҵ���һ�����ƴ���treenode�Ľڵ� */
                int cmpresult = StrCmpNoCase(gtTreeNode[childnode].treenode.sName, treenode->sName);
                if (cmpresult == 0)
                {
                    /*������������м�ڵ㣬�滻������ڵ����������*/
                    if ((INVALID_CMDINDEX == gtTreeNode[childnode].treenode.wCmdAttrPos) &&
                         (INVALID_CMDINDEX != treenode->wCmdAttrPos))
                    {
                        gtTreeNode[childnode].treenode.wCmdAttrPos = treenode->wCmdAttrPos;
                        gtTreeNode[childnode].cfgDat = cfgDat;
                    }
                    /* ���ظ�������롣�ָ��������� */
                    gtTreeNode[node].treenode.wNextNodePos = g_wFreeNode;
                    g_wFreeNode = node;
                    return childnode;
                }
                else if (cmpresult > 0)
                {
                    /* ���뵽prionode */
                    break;
                }
                
                prionode = childnode;
                childnode = gtTreeNode[childnode].treenode.wNextNodePos;
            }

            /* �������� */
            {
                gtTreeNode[node].treenode = *treenode;
                gtTreeNode[node].treenode.wNextNodePos = childnode;
                gtTreeNode[node].treenode.wNextSonPos = 0;
                gtTreeNode[node].cfgDat = cfgDat;

                if (prionode != 0)
                {
                    gtTreeNode[prionode].treenode.wNextNodePos = node;
                }
                else
                {
                    gtTreeNode[parentnode].treenode.wNextSonPos = node;
                }
            }
        }

        return node;
    }
}

/************************************************************************
* �������ƣ�
* ���������� �������ڵ㣬�ֵܽڵ㷽ʽ
* ��������� 
            �ֵܵ��±�,     siblingnode
            ���ڵ���Ϣ,     treenode
            DAT������,      datptr
* �� �� ֵ�� ��������ڵ��±�
* ����˵���� 
            ���ڼ��ٲ�������˱��뱣֤�ڵ���������sibling�ĺ���
            ���ϲ�����ʱ�򣬵�һ�β���ڵ�ʹ��OAM_CFGAppendChildNode�������������ز���ڵ������
            ֮���Դ˽ڵ�Ϊ���������ӵ��˽ڵ�ĺ��棨�����Ѿ�Ԥ������
* �޸�����      �汾��  �޸���      �޸�����
************************************************************************/
WORD16 AppendSiblingNode(WORD16 siblingnode, TYPE_TREE_NODE * treenode, T_OAM_Cfg_Dat* cfgDat)
{
    if ((g_wFreeNode == 0) || (siblingnode == 0))
    {
        return 0;
    }
#if 0
    /* �ڵ���������sibling�ĺ��� */
    if (strncmp(gtTreeNode[siblingnode].treenode.sName, treenode->sName, MAX_NODENAME) >= 0)
    {
        return 0;
    }
    else
#endif
    {
        /* ȡ��һ���ڵ� */
        WORD16 node = g_wFreeNode;
        g_wFreeNode = gtTreeNode[g_wFreeNode].treenode.wNextNodePos;

        {
            /* ���� */
            WORD16 childnode = gtTreeNode[siblingnode].treenode.wNextNodePos;
            WORD16 prionode = siblingnode;

            while (childnode != 0)
            {
                /* �ҵ���һ�����ƴ���treenode�Ľڵ� */
                int cmpresult = strncmp(gtTreeNode[childnode].treenode.sName, treenode->sName, MAX_NODENAME);
                if (cmpresult == 0)
                {
                    /* ���ظ�������롣�ָ��������� */
                    gtTreeNode[node].treenode.wNextNodePos = g_wFreeNode;
                    g_wFreeNode = node;

                    /* �������Ľڵ㲻��������Ժ��� */
                    if (treenode->wCmdAttrPos == 0)
                    {
                        /* ���ظýڵ���±� */
                        return childnode;
                    }
                    /* �������Ľڵ��������Ҫ�ж������ظ� */
                    else if (gtTreeNode[childnode].treenode.wCmdAttrPos == 0)
                    {
                        /* ���ǽڵ���Ϣ */
                        gtTreeNode[childnode].treenode = *treenode;
                        gtTreeNode[childnode].cfgDat   = cfgDat;

                        /* ���ظýڵ���±� */
                        return childnode;
                    }
                    /* �ظ�����������²���Ľڵ� */
                    else
                    {
                        /* ���ش��� */
                        return childnode;
                    }
                }
                else if (cmpresult > 0)
                {
                    /* ���뵽prionode */
                    break;
                }
                
                prionode = childnode;
                childnode = gtTreeNode[childnode].treenode.wNextNodePos;
            }

            /* �������� */
            {
                gtTreeNode[node].treenode = *treenode;
                gtTreeNode[node].treenode.wNextNodePos = childnode;
                gtTreeNode[node].treenode.wNextSonPos = 0;
                gtTreeNode[node].cfgDat = cfgDat;

                /* prionode������0 */
                gtTreeNode[prionode].treenode.wNextNodePos = node;
            }
        }

        return node;
    }
}

/************************************************************************
* �������ƣ�
* ���������� ɾ��ָ�����ڵ��£�datptrΪ�ض�ֵ���ӽڵ�
* ��������� 
            �ֵܵ��±�,     siblingnode
            ���ڵ���Ϣ,     treenode
            DAT������,      datptr
* �� �� ֵ�� ��������ڵ��±�
* ����˵���� 
* �޸�����      �汾��  �޸���      �޸�����
************************************************************************/
static void PrintBlank(int count)
{
    while (count > 0)
    {
        printf("  ");
        count--;
    }
}

void DeleteChildNodes(T_OAM_Cfg_Dat* cfgDat)
{
    WORD32 i;
    /* ��������ģʽ */
    for (i=1;i<=g_dwModeCount;i++)
    {
        WORD16 * nextptr = &gtMode[i].wTreeRootPos;
        XOS_SysLog(OAM_CFG_LOG_NOTICE,"mode: %d\n", gtMode[i].bModeID);
        _DeleteChildNodes(nextptr, cfgDat, 0);
    }
}
void _DeleteChildNodes(WORD16 * nextptr, T_OAM_Cfg_Dat* cfgDat, int calllevel)
{
    /* ���Ƶݹ��� */
    if (calllevel >= 200)
    {
        return;
    }

    if (cfgDat == NULL)
    {
        return;
    }

    while (*nextptr != 0)
    {
        if (gtTreeNode[*nextptr].treenode.wNextSonPos == 0xFFFF)
        {
            return;
        }
        /* �Ƿ����ӽڵ� */
        if (gtTreeNode[*nextptr].treenode.wNextSonPos != 0)
        {
            PrintBlank(calllevel);
            XOS_SysLog(OAM_CFG_LOG_NOTICE,"sub tree node: %d,%s\n", *nextptr, gtTreeNode[*nextptr].treenode.sName);
            _DeleteChildNodes(&gtTreeNode[*nextptr].treenode.wNextSonPos, cfgDat, calllevel+1);
        }

        /* ָ�벻ƥ�䣬������ڵ� */
        if ((gtTreeNode[*nextptr].treenode.wCmdAttrPos != 0) && (gtTreeNode[*nextptr].cfgDat != cfgDat))
        {
            PrintBlank(calllevel);
            /* �ƶ�ָ�� */
            XOS_SysLog(OAM_CFG_LOG_NOTICE,"skip: %d,%s\n", *nextptr, gtTreeNode[*nextptr].treenode.sName);
            nextptr = &gtTreeNode[*nextptr].treenode.wNextNodePos;
        }
        /* �����ӽڵ� */
        else if (gtTreeNode[*nextptr].treenode.wNextSonPos != 0)
        {
            PrintBlank(calllevel);
            XOS_SysLog(OAM_CFG_LOG_NOTICE,"delete cmd attr: %d,%s\n", *nextptr, gtTreeNode[*nextptr].treenode.sName);
            /* ָ��ƥ��ʱ���޸�Ϊ�м�ڵ� */
            if (gtTreeNode[*nextptr].cfgDat == cfgDat)
            {
                gtTreeNode[*nextptr].cfgDat = NULL;
                gtTreeNode[*nextptr].treenode.wCmdAttrPos = 0;
            }
            /* �ƶ�ָ�� */
            nextptr = &gtTreeNode[*nextptr].treenode.wNextNodePos;
        }
        /* û���ӽڵ㣬��ָ��ƥ�� */
        /* û���ӽڵ㣬��ָ�벻ƥ�䡣�������������ɾ���ýڵ� */
        else if ((gtTreeNode[*nextptr].cfgDat == cfgDat) || (gtTreeNode[*nextptr].treenode.wCmdAttrPos == 0))
        {
            /* ɾ����û���ӽڵ㣬ֱ��ɾ�� */
            WORD16 deletenode = *nextptr;
            PrintBlank(calllevel);
            XOS_SysLog(OAM_CFG_LOG_NOTICE,"delete tree node: %d,%s\n", *nextptr, gtTreeNode[*nextptr].treenode.sName);

            /* �޸�ָ������ */
            *nextptr = gtTreeNode[*nextptr].treenode.wNextNodePos;

            /* ɾ���ڵ� */
            gtTreeNode[deletenode].treenode.wNextNodePos = g_wFreeNode;
            gtTreeNode[deletenode].treenode.wNextSonPos = 0xFFFF;
            g_wFreeNode = deletenode;
        }
    }
}
