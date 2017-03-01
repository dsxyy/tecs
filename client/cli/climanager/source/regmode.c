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
    模式记录的描述
    使用长度为100的数组记录模式节点。其中，数组的第一个元素节点不使用。
    因此，可以记录99个模式信息。
    g_dwModeCount记录有效的模式数量。
    所有有效模式信息记录在数组的前端。例如，g_dwModeCount为10的时候，
        gtMode[0]为无效值(永远无效)
        gtMode[1]-gtMode[10]为有效模式信息
        gtMode[11]-gtMode[99]为未使用元素
    由于数组的第一个元素不使用，最后一个有效模式的下标为g_dwModeCount

    删除模式时，为了保证删除后有效模式仍然在数组的前端，
    使用最后一个有效模式信息(gtMode[g_dwModeCount])，覆盖要删除的模式即可。
    例如，按照上面的例子，删除下标为6的模式，则使用gtMode[10]覆盖gtMode[6]，
        然后将g_dwModeCount减1。
*/

DWORD g_dwModeCount = 0;
TYPE_MODE *gtMode = NULL;

/***********************************************************
 *                     本地 变量                           *
***********************************************************/
/***********************************************************
 *                     函数申明                            *
***********************************************************/

/***********************************************************
 *                     宏定义                              *
***********************************************************/



/************************************************************************
* 函数名称：
* 功能描述： 增加模式节点
* 输入参数： 模式信息指针
* 返 回 值： 树节点下标，0表示操作失败
* 其它说明： 
* 修改日期      版本号  修改人      修改内容
************************************************************************/
WORD16 OAM_CFGAppendMode(TYPE_MODE * mode, BOOLEAN bRestore)
{
    if (!bRestore)
    {
        WORD32 dwMaxModeCnt = OAM_CFGGetModeCntInAllDat();
        /* CRDCM00025960 OMP主备多次倒换触发OAM_Interpret异常，并且打印函数调用链失败 */
        /* 检查全局数组是否还有剩下的空间 */
        if (((g_dwModeCount + 1) >= dwMaxModeCnt) || (g_wFreeNode == 0))
        {
            XOS_SysLog(OAM_CFG_LOG_ERROR, "g_dwModeCount[%d] is big than max[%d]\n", g_dwModeCount, dwMaxModeCnt);
            return 0;
        }
    }

    {
        DWORD index;
        index = OAM_CFGSearchMode(mode->bModeID);

        /* 检索不到模式节点，表示是一个新的模式，需要插入该节点 */
        if (index == 0)
        {
            /* 插入模式节点 */
            /* 注意第一个节点不使用（因此先使g_dwModeCount增加1） */
            g_dwModeCount++;
            gtMode[g_dwModeCount] = *mode;
            gtMode[g_dwModeCount].wTreeRootPos = 0;
            index = g_dwModeCount;
        }

        if (gtMode[index].wTreeRootPos == 0)
        {
            /* 插入该模式对应的树节点 */
            {
                /* 取得一个节点 */
                WORD16 node = g_wFreeNode;
                g_wFreeNode = gtTreeNode[g_wFreeNode].treenode.wNextNodePos;
                memset(&gtTreeNode[node], 0, sizeof(gtTreeNode[node]));

                /* 模式对应的树上的节点，其中的名称和注释信息，界面上观察不到，所以不处理 */

                /* 增加根索引 */
                gtMode[index].wTreeRootPos = node;

                /* 模式对应的树上的节点，之间的“兄弟”链(wNextNodePos)，没有实际意义，因此不处理 */
#if 0
                /* 插入树节点，插入到第一个mode对应的节点之前 */
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
* 函数名称：
* 功能描述： 删除模式节点
* 输入参数： 模式ID
* 返 回 值： 无
* 其它说明： 
* 修改日期      版本号  修改人      修改内容
************************************************************************/
void OAM_CFGDeleteMode(BYTE modeID)
{
    /* 检索模式 */
    int index = OAM_CFGSearchMode(modeID);
    if ((index != 0) && (gtMode[index].wTreeRootPos != 0))
    {
        /* 如果模式下还有命令时，就不能删除该模式 */
        if (gtTreeNode[gtMode[index].wTreeRootPos].treenode.wNextSonPos == 0)
        {
            /* 删除模式的方式：使用最后一个有效模式数据替换该模式数据 */
            /* 注意第一个节点不使用 */
            gtMode[index] = gtMode[g_dwModeCount];
            memset(&gtMode[g_dwModeCount+1], 0, sizeof(gtMode[g_dwModeCount+1]));
            g_dwModeCount--;
        }
        else
        {
            /* 还有命令，不能删除 */
            return;
        }
    } 
}

/************************************************************************
* 函数名称：
* 功能描述： 检索模式节点
* 输入参数： 模式ID
* 返 回 值： 模式信息在gtMode数组中的下标，未查找到则返回0
* 其它说明： 
* 修改日期      版本号  修改人      修改内容
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

