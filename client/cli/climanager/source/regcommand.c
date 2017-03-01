/*********************************************************************
* 版权所有 (C)2008, 深圳市中兴通讯股份有限公司。
*
* 文件名称： 
* 文件标识： 
* 内容摘要： 
* 其它说明： 
* 当前版本： V1.0
* 作    者：
* 完成日期： 
**********************************************************************/

/***********************************************************
 *                      头文件                             *
***********************************************************/
#include "includes.h"
/***********************************************************
 *                     全局变量                            *
***********************************************************/
/*
    树节点信息的描述

    树节点信息使用二叉树链表表示
    链表使用数组下标表示链
    空闲链表使用wNextNodePos字段链接，空闲链表头为g_wFreeNode
    记录树节点信息的数组元素，使用wNextNodePos字段表示下一个兄弟节点
    使用wNextSonPos字段表示第一个子节点

    由于树节点从模式节点索引，因此不需要链表头。

    树节点的第一个元素不使用
*/
WORD16 g_wFreeNode = 0;
T_TreeNode *gtTreeNode = NULL;

/***********************************************************
 *                     本地 变量                           *
***********************************************************/
/***********************************************************
 *                     函数申明                            *
***********************************************************/
static void _DeleteChildNodes(WORD16 * nextptr, T_OAM_Cfg_Dat* cfgDat, int calllevel);
/***********************************************************
 *                     宏定义                              *
***********************************************************/

/************************************************************************
* 函数名称：
* 功能描述： 初始化树节点数组
* 输入参数： 
* 返 回 值： 
* 其它说明： 
* 修改日期      版本号  修改人      修改内容
************************************************************************/
void OAM_CFGInitTreeNode(void)
{
    WORD32 i = 0;
	
    if (g_ptOamIntVar->dwTreeNodeSum < 1)
        return;

    /* 初始化数据区 */
    memset(gtTreeNode, 0, sizeof(T_TreeNode) * g_ptOamIntVar->dwTreeNodeSum);

    /* 构造空闲链表，注意从下标1开始 */
    for (i=1;i < g_ptOamIntVar->dwTreeNodeSum; i++)
    {
        gtTreeNode[i].treenode.wNextNodePos = i+1;
    }
    gtTreeNode[g_ptOamIntVar->dwTreeNodeSum-1].treenode.wNextNodePos = 0;

    /* 初始化空闲表头 */
    g_wFreeNode = 1;
}

void OAM_CFGInitTreeNodeForRestore(WORD32 dwTreeNodeSum)
{
    WORD32 i = 0;
	
    if (dwTreeNodeSum < 1)
        return;
	
    /* 初始化数据区 */
    memset(gtTreeNode, 0, sizeof(T_TreeNode) * g_ptOamIntVar->dwTreeNodeSum);

    /* 构造空闲链表，注意从下标1开始 */
    for (i=1;i < dwTreeNodeSum; i++)
    {
        gtTreeNode[i].treenode.wNextNodePos = i+1;
    }
    gtTreeNode[dwTreeNodeSum-1].treenode.wNextNodePos = 0;

    /* 初始化空闲表头 */
    g_wFreeNode = 1;

    return;
}


/************************************************************************
* 函数名称：
* 功能描述： 增加树节点
* 输入参数： 
            父节点下标,     parentnode
            数节点信息,     treenode
            DAT数据区,      datptr
* 返 回 值： 插入的树节点下标
* 其它说明： 
* 修改日期      版本号  修改人      修改内容
************************************************************************/
WORD16 OAM_CFGAppendChildNode(WORD16 parentnode, TYPE_TREE_NODE * treenode, T_OAM_Cfg_Dat* cfgDat)
{
    if ((g_wFreeNode == 0) || (parentnode == 0))
    {
        return 0;
    }
    else
    {
        /* 取得一个节点 */
        WORD16 node = g_wFreeNode;
        g_wFreeNode = gtTreeNode[g_wFreeNode].treenode.wNextNodePos;

        {
            /* 搜索 */
            WORD16 childnode = gtTreeNode[parentnode].treenode.wNextSonPos;
            WORD16 prionode = 0;

            while (childnode != 0)
            {
                /* 找到第一个名称大于treenode的节点 */
                int cmpresult = StrCmpNoCase(gtTreeNode[childnode].treenode.sName, treenode->sName);
                if (cmpresult == 0)
                {
                    /*如果重名的是中间节点，替换成命令节点的命令属性*/
                    if ((INVALID_CMDINDEX == gtTreeNode[childnode].treenode.wCmdAttrPos) &&
                         (INVALID_CMDINDEX != treenode->wCmdAttrPos))
                    {
                        gtTreeNode[childnode].treenode.wCmdAttrPos = treenode->wCmdAttrPos;
                        gtTreeNode[childnode].cfgDat = cfgDat;
                    }
                    /* 有重复项，不插入。恢复空闲链表 */
                    gtTreeNode[node].treenode.wNextNodePos = g_wFreeNode;
                    g_wFreeNode = node;
                    return childnode;
                }
                else if (cmpresult > 0)
                {
                    /* 插入到prionode */
                    break;
                }
                
                prionode = childnode;
                childnode = gtTreeNode[childnode].treenode.wNextNodePos;
            }

            /* 插入链表 */
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
* 函数名称：
* 功能描述： 增加树节点，兄弟节点方式
* 输入参数： 
            兄弟点下标,     siblingnode
            数节点信息,     treenode
            DAT数据区,      datptr
* 返 回 值： 插入的树节点下标
* 其它说明： 
            用于加速操作，因此必须保证节点必须插入在sibling的后面
            当合并树的时候，第一次插入节点使用OAM_CFGAppendChildNode函数，函数返回插入节点的索引
            之后以此节点为基础，增加到此节点的后面（链表已经预先排序）
* 修改日期      版本号  修改人      修改内容
************************************************************************/
WORD16 AppendSiblingNode(WORD16 siblingnode, TYPE_TREE_NODE * treenode, T_OAM_Cfg_Dat* cfgDat)
{
    if ((g_wFreeNode == 0) || (siblingnode == 0))
    {
        return 0;
    }
#if 0
    /* 节点必须插入在sibling的后面 */
    if (strncmp(gtTreeNode[siblingnode].treenode.sName, treenode->sName, MAX_NODENAME) >= 0)
    {
        return 0;
    }
    else
#endif
    {
        /* 取得一个节点 */
        WORD16 node = g_wFreeNode;
        g_wFreeNode = gtTreeNode[g_wFreeNode].treenode.wNextNodePos;

        {
            /* 搜索 */
            WORD16 childnode = gtTreeNode[siblingnode].treenode.wNextNodePos;
            WORD16 prionode = siblingnode;

            while (childnode != 0)
            {
                /* 找到第一个名称大于treenode的节点 */
                int cmpresult = strncmp(gtTreeNode[childnode].treenode.sName, treenode->sName, MAX_NODENAME);
                if (cmpresult == 0)
                {
                    /* 有重复项，不插入。恢复空闲链表 */
                    gtTreeNode[node].treenode.wNextNodePos = g_wFreeNode;
                    g_wFreeNode = node;

                    /* 如果插入的节点不是命令，可以忽略 */
                    if (treenode->wCmdAttrPos == 0)
                    {
                        /* 返回该节点的下标 */
                        return childnode;
                    }
                    /* 如果插入的节点是命令，需要判断命令重复 */
                    else if (gtTreeNode[childnode].treenode.wCmdAttrPos == 0)
                    {
                        /* 覆盖节点信息 */
                        gtTreeNode[childnode].treenode = *treenode;
                        gtTreeNode[childnode].cfgDat   = cfgDat;

                        /* 返回该节点的下标 */
                        return childnode;
                    }
                    /* 重复的命令，忽略新插入的节点 */
                    else
                    {
                        /* 返回错误 */
                        return childnode;
                    }
                }
                else if (cmpresult > 0)
                {
                    /* 插入到prionode */
                    break;
                }
                
                prionode = childnode;
                childnode = gtTreeNode[childnode].treenode.wNextNodePos;
            }

            /* 插入链表 */
            {
                gtTreeNode[node].treenode = *treenode;
                gtTreeNode[node].treenode.wNextNodePos = childnode;
                gtTreeNode[node].treenode.wNextSonPos = 0;
                gtTreeNode[node].cfgDat = cfgDat;

                /* prionode不会是0 */
                gtTreeNode[prionode].treenode.wNextNodePos = node;
            }
        }

        return node;
    }
}

/************************************************************************
* 函数名称：
* 功能描述： 删除指定树节点下，datptr为特定值的子节点
* 输入参数： 
            兄弟点下标,     siblingnode
            数节点信息,     treenode
            DAT数据区,      datptr
* 返 回 值： 插入的树节点下标
* 其它说明： 
* 修改日期      版本号  修改人      修改内容
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
    /* 遍历所有模式 */
    for (i=1;i<=g_dwModeCount;i++)
    {
        WORD16 * nextptr = &gtMode[i].wTreeRootPos;
        XOS_SysLog(OAM_CFG_LOG_NOTICE,"mode: %d\n", gtMode[i].bModeID);
        _DeleteChildNodes(nextptr, cfgDat, 0);
    }
}
void _DeleteChildNodes(WORD16 * nextptr, T_OAM_Cfg_Dat* cfgDat, int calllevel)
{
    /* 限制递归层次 */
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
        /* 是否有子节点 */
        if (gtTreeNode[*nextptr].treenode.wNextSonPos != 0)
        {
            PrintBlank(calllevel);
            XOS_SysLog(OAM_CFG_LOG_NOTICE,"sub tree node: %d,%s\n", *nextptr, gtTreeNode[*nextptr].treenode.sName);
            _DeleteChildNodes(&gtTreeNode[*nextptr].treenode.wNextSonPos, cfgDat, calllevel+1);
        }

        /* 指针不匹配，是命令节点 */
        if ((gtTreeNode[*nextptr].treenode.wCmdAttrPos != 0) && (gtTreeNode[*nextptr].cfgDat != cfgDat))
        {
            PrintBlank(calllevel);
            /* 移动指针 */
            XOS_SysLog(OAM_CFG_LOG_NOTICE,"skip: %d,%s\n", *nextptr, gtTreeNode[*nextptr].treenode.sName);
            nextptr = &gtTreeNode[*nextptr].treenode.wNextNodePos;
        }
        /* 还有子节点 */
        else if (gtTreeNode[*nextptr].treenode.wNextSonPos != 0)
        {
            PrintBlank(calllevel);
            XOS_SysLog(OAM_CFG_LOG_NOTICE,"delete cmd attr: %d,%s\n", *nextptr, gtTreeNode[*nextptr].treenode.sName);
            /* 指针匹配时，修改为中间节点 */
            if (gtTreeNode[*nextptr].cfgDat == cfgDat)
            {
                gtTreeNode[*nextptr].cfgDat = NULL;
                gtTreeNode[*nextptr].treenode.wCmdAttrPos = 0;
            }
            /* 移动指针 */
            nextptr = &gtTreeNode[*nextptr].treenode.wNextNodePos;
        }
        /* 没有子节点，且指针匹配 */
        /* 没有子节点，且指针不匹配。如果不是命令则删除该节点 */
        else if ((gtTreeNode[*nextptr].cfgDat == cfgDat) || (gtTreeNode[*nextptr].treenode.wCmdAttrPos == 0))
        {
            /* 删除后没有子节点，直接删除 */
            WORD16 deletenode = *nextptr;
            PrintBlank(calllevel);
            XOS_SysLog(OAM_CFG_LOG_NOTICE,"delete tree node: %d,%s\n", *nextptr, gtTreeNode[*nextptr].treenode.sName);

            /* 修改指针内容 */
            *nextptr = gtTreeNode[*nextptr].treenode.wNextNodePos;

            /* 删除节点 */
            gtTreeNode[deletenode].treenode.wNextNodePos = g_wFreeNode;
            gtTreeNode[deletenode].treenode.wNextSonPos = 0xFFFF;
            g_wFreeNode = deletenode;
        }
    }
}
