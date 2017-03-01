/**********************************************************
* 版权所有 (C)2008，深圳市中兴通讯股份有限公司
* 
* 文件名称：BladeCrystal.c
* 文件标识：
* 内容摘要：刀片单板晶振状态查询代码
* 其他说明：目前仅支持SBCJ单板，SBCW没有需要查询的晶振，其他单板请参考SBCJ的实现，增加相应分支。
* 当前版本：V1.0
* 作    者：jimei
* 完成日期：20120328
*---------------------------------------------------------
* 修改记录： 日期：    修改人：   修改原因：
*---------------------------------------------------------
*           20120328   jimei      创建
**********************************************************/


/**************************************************************************
*                        头文件                                           *
**************************************************************************/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/io.h>
#include <linux/string.h>


/**************************************************************************
*                        宏定义                                           *
**************************************************************************/
#undef NULL
#define NULL       (void *)(0)

/* 返回值宏 */
#define BSP_OK                         (0)             /* 返回码，操作成功 */
#define BSP_ERROR                      ((WORD32)(-1))  /* 返回码，操作失败 */
#define BSP_NOT_SUPPORT                ((WORD32)(-2))  /* 返回码，接口不支持 */

/* 晶振位置宏 */
#define BSP_PANEL_BOARD                (0x00)    /* 母板 */

/* 晶振名称长度宏 */
#define CRYSTAL_NAME_LEN               (16)
/* 晶振表最大个数宏 */
#define CRYSTAL_TABLE_MAX_NUM          (16)      /* 如果某个单板的晶振个数大于此值，需要将此值扩大 */

/* 晶振状态宏 */
#define BSP_CRYSTAL_MODE_NORMAL        (0)       /* 状态正常 */
#define BSP_CRYSTAL_MODE_ABNORMAL      (1)       /* 状态异常 */
#define BSP_CRYSTAL_NOT_SUPPORT        (2)       /* 状态查询不支持 */

/* 单板宏 */
#define BSP_BOARD_SBCJ                 (1)

/* 寄存器地址宏 */
#define SBCJ_WORKEPLD_PHYADDR          (0xfe000000)
#define SBCJ_WORKEPLD_SIZE             (0x200)

#define BSP_EPLD_VER_ADDR              (0x0)

#define SBCJ_EPLD_25M_CLOCK_REG        (0x151) /* the offset of 25M clock status */
#define SBCJ_EPLD_25M_CLOCK_VALUE      (0x0)   /* the normal value of  25M clock */
#define SBCJ_EPLD_25M_CLOCK_MASK       (0x100) /* the mask value of  25M clock  */

#define SBCJ_EPLD_32K_CLOCK_REG        (0x164) /* the offset of 32K clock status */
#define SBCJ_EPLD_32K_CLOCK_VALUE      (0x0)   /* the normal value of 32K clock  */
#define SBCJ_EPLD_32K_CLOCK_MASK       (0x1)   /* the mask value of  32K clock  */


/**************************************************************************
*                        数据结构                                         *
**************************************************************************/
typedef unsigned char           BYTE;
typedef unsigned short          WORD16;
typedef unsigned int            WORD32;
typedef unsigned long long int  WORD64;
typedef unsigned long int       WORDPTR; 

typedef struct
{   
    /* 结构原始定义区域，如需增加新字段请消费下面的保留字段 */
    WORD32 dwIndex;             /* 编号,从1开始 */	
    BYTE   ucCrystsalName[CRYSTAL_NAME_LEN];  /* 晶振名称:字符串，由单板自行命名 */	
    WORD32 dwLocation;          /* 晶振位置,母板、母板功能子卡1~8、母板CPU子卡1~4、后插卡、后插卡子卡1~4 */ 
    WORD32 dwMode;              /* 晶振状态:
                                     BSP_CRYSTAL_MODE_NORMAL    状态正常
                                     BSP_CRYSTAL_MODE_ABNORMAL  状态异常
                                     BSP_CRYSTAL_NOT_SUPPORT    状态查询不支持 */

    /*保留字段和新增字段区域，新增字段均消费保留字段*/
    WORDPTR dwReserve[4];           
    /*End of 保留字段和新增字段区域*/	
}T_BSP_CRYSTAL_STATUS;

/**************************************************************************
*                        全局变量                                         *
**************************************************************************/
void __iomem *s_dwWorkEpldVirAddr = NULL;
BYTE g_ucBrdEpldVer = 0;  /* 模块启动时获取母板EPLD版本，存于此全局变量中 */
BYTE g_ucBrdType = 0;     /* 模块启动时获取单板类型，存于此全局变量中 */
static struct proc_dir_entry *proc_zte_dir;
static struct proc_dir_entry *proc_crystal;
static T_BSP_CRYSTAL_STATUS s_dwBrdCrystalTable[CRYSTAL_TABLE_MAX_NUM];
static BYTE s_ucBrdCrystalTableSize = 0;

static T_BSP_CRYSTAL_STATUS s_dwSbcjCrystalTable[]=
{
    /* Index   Name          Location            Mode                       Reserve */
    {  1,      "25M",        BSP_PANEL_BOARD,    BSP_CRYSTAL_NOT_SUPPORT,   {0,0,0,0}},
    {  2,      "RTC_32K",    BSP_PANEL_BOARD,    BSP_CRYSTAL_NOT_SUPPORT,   {0,0,0,0}}
};
static BYTE s_ucSbcjCrystalTableSize = sizeof(s_dwSbcjCrystalTable) / sizeof (T_BSP_CRYSTAL_STATUS);


/**************************************************************************
*                        函数实现                                         *
**************************************************************************/

/******************************************************************************
* 函数名称：WORD32 k_GetCrystalStatus(T_BSP_CRYSTAL_STATUS *ptCrystalStatus)
* 功能描述：晶振状态查询 
* 输入参数：ptCrystalStatus ->dwIndex   晶振编号,从1开始
* 输出参数：ptCrystalStatus ->dwMode    晶振状态
* 返 回 值：BSP_OK       查询成功
*           BSP_ERROR    查询失败
* 其它说明：无
* 修改日期    版本号   修改人      修改内容
* -----------------------------------------------
* 20120328    V1.0     jimei       creat
******************************************************************************/
WORD32 k_GetCrystalStatus(T_BSP_CRYSTAL_STATUS *ptCrystalStatus)
{
    WORD32 dwIndex  = 0;
    BYTE ucRegValue = 0;
    
    if (NULL == ptCrystalStatus)
    {
        printk("[CRYSTAL] Input null pointer!\n");
        return BSP_ERROR;
    }

    dwIndex = ptCrystalStatus->dwIndex;
    if ((dwIndex < 1)||(dwIndex > s_ucBrdCrystalTableSize))
    {
        printk("[CRYSTAL] Input Index(%d) error! Must be in [1-%d].\n",dwIndex,s_ucBrdCrystalTableSize);
        return BSP_ERROR;
    }

    if ((!s_dwWorkEpldVirAddr) && (g_ucBrdType != 0))
    {
        printk("[CRYSTAL] workepld phy addr not remap to vir addr or remap failed!\n");
        return BSP_ERROR;
    }

    if ((BYTE)BSP_BOARD_SBCJ == g_ucBrdType)
    {
        switch (dwIndex)
        {
            case 1:    /* 25M */
            {
                ucRegValue = 0;
                ucRegValue = (BYTE)(*(volatile BYTE*)((WORDPTR)s_dwWorkEpldVirAddr + SBCJ_EPLD_25M_CLOCK_REG));
         	    if (SBCJ_EPLD_25M_CLOCK_VALUE == (ucRegValue & SBCJ_EPLD_25M_CLOCK_MASK))
                {
                    ptCrystalStatus->dwMode = BSP_CRYSTAL_MODE_NORMAL;
                }
                else
                {
                    ptCrystalStatus->dwMode = BSP_CRYSTAL_MODE_ABNORMAL;
                }
    
                break;
            }
            case 2:    /* RTC_32K */
            {
                /* RTC时钟检测,EPLD 112之后提供检测 */ 
                if(g_ucBrdEpldVer < 12)
                {
                    ptCrystalStatus->dwMode = BSP_CRYSTAL_NOT_SUPPORT;
                    return BSP_OK;
                }
                
                ucRegValue = 0;
                ucRegValue = (BYTE)(*(volatile BYTE*)((WORDPTR)s_dwWorkEpldVirAddr + SBCJ_EPLD_32K_CLOCK_REG));
                if (SBCJ_EPLD_32K_CLOCK_VALUE == (ucRegValue & SBCJ_EPLD_32K_CLOCK_MASK))
                {
                    ptCrystalStatus->dwMode = BSP_CRYSTAL_MODE_NORMAL;
                }
                else
                {
                    ptCrystalStatus->dwMode = BSP_CRYSTAL_MODE_ABNORMAL;
                }  
    
                break;
            }
            default:
            {
                return BSP_ERROR;
                break;
            }
        }
    }
    else
    {
        printk("[CRYSTAL] BladeCrystal module not support on this board!\n");
        return BSP_NOT_SUPPORT;
   	}
#if 0   /* 调试阶段可以打开此打印 */
    printk("[CRYSTAL] Index:%d, RegVal:0x%x\n",dwIndex,ucRegValue);
#endif		      
    return BSP_OK;
}

/******************************************************************************
* 函数名称：k_crystal_read_proc
* 功能描述：查询晶振状态并写入/proc/zte/crystal
* 输入参数：page : 用来写入数据的缓冲区
*           off  : 文件指针的偏移
*           count: 可以写入的最大字符数
*           data : 由驱动内部使用
* 输出参数：eof  : 文件结束参数
*           start: 写在此页的哪里,用法复杂,如果是返回小量数据(不超过一页4KB),赋值为NULL
* 返 回 值：读到的字节数
* 其它说明：无
* 修改日期    版本号   修改人      修改内容
* -----------------------------------------------
* 20120328    V1.0     jimei       creat
******************************************************************************/
static int k_crystal_read_proc (char *page, char **start, off_t off, int count,
              int *eof, void *data)
{
    WORD32 dwIndex = 0;
    WORD32 dwLen   = 0;
    WORD32 dwTotalLen = 0;
    /*off_t  begin = 0;*/

	/* 查询晶振状态 */
    for (dwIndex = 1; dwIndex <= s_ucBrdCrystalTableSize; dwIndex++)
    {
        k_GetCrystalStatus(&s_dwBrdCrystalTable[dwIndex-1]);
    }

	/* 向文件中写入晶振状态 */
    for (dwIndex = 1; dwIndex <= s_ucBrdCrystalTableSize; dwIndex++)
    {
        dwLen = sprintf(page+dwTotalLen, "%2d : %-16s : %-10s\n", 
                        s_dwBrdCrystalTable[dwIndex-1].dwIndex,
                        s_dwBrdCrystalTable[dwIndex-1].ucCrystsalName,
                        (s_dwBrdCrystalTable[dwIndex-1].dwMode==BSP_CRYSTAL_NOT_SUPPORT)? "NOTSUPPORT":(\
                        (s_dwBrdCrystalTable[dwIndex-1].dwMode==BSP_CRYSTAL_MODE_NORMAL)? "OK":"ERROR"));

        dwTotalLen += dwLen;
    }

    *eof = 1;

    return dwTotalLen;

#if 0   /* 数据量超过一页时 */
    if ((dwTotalLen + begin) > (off + count))
    {
        goto done;
    }
    if ((dwTotalLen + begin) < off) 
    {
        begin += dwTotalLen;
        dwTotalLen = 0;
    }

    *eof = 1;

done:
    if (off >= (dwTotalLen + begin))
    {
        return 0;
    }
    *start = page + (off-begin);
    return ((count < (begin+dwTotalLen-off)) ? count : (begin+dwTotalLen-off));
#endif
}

/******************************************************************************
* 函数名称：static int __init k_InitCrystalModule(void)
* 功能描述：模块初始化
* 输入参数：无
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期    版本号   修改人      修改内容
* -----------------------------------------------
* 20120328    V1.0     jimei       creat
******************************************************************************/
static int __init k_InitCrystalModule(void)
{
    char x86_model_id[64];
    unsigned int *v;
    /*WORD32 dwIndex  = 0;*/

    /* 获取单板类型，存到g_ucBrdType里 */
    memset(x86_model_id,'\0',64);
    v = (unsigned int *) x86_model_id;
    cpuid(0x80000002, &v[0], &v[1], &v[2], &v[3]);
    cpuid(0x80000003, &v[4], &v[5], &v[6], &v[7]);
    cpuid(0x80000004, &v[8], &v[9], &v[10], &v[11]);
    printk("[CRYSTAL] model_id:%s\n",x86_model_id);

    if (NULL != strstr(x86_model_id,"C5528"))
    {
        g_ucBrdType = (BYTE)BSP_BOARD_SBCJ;
    }
    else
    {
        printk("[CRYSTAL] BladeCrystal module not support on this board!\n");
        g_ucBrdType = 0;
   	}

    /* 虚拟地址映射 */
    if ((BYTE)BSP_BOARD_SBCJ == g_ucBrdType)
    {
        s_dwWorkEpldVirAddr = ioremap_nocache(SBCJ_WORKEPLD_PHYADDR,SBCJ_WORKEPLD_SIZE);
        memcpy(s_dwBrdCrystalTable,s_dwSbcjCrystalTable,sizeof(s_dwSbcjCrystalTable));
        s_ucBrdCrystalTableSize = s_ucSbcjCrystalTableSize;
   	}
    else
    {
        g_ucBrdType = 0;
        memset(s_dwBrdCrystalTable,0,sizeof(s_dwBrdCrystalTable));
        s_ucBrdCrystalTableSize = 0;
   	}

    if ((BYTE)BSP_BOARD_SBCJ == g_ucBrdType)
    {
        if (!s_dwWorkEpldVirAddr)
        {
            printk("[CRYSTAL] Fail to remap workepld phy to vir addr!\n");
            return BSP_ERROR;
        }
        else
       	{
            /* 获取EPLD版本号，存到g_ucBrdEpldVer里 */
            g_ucBrdEpldVer = (BYTE)(*(volatile BYTE*)((WORDPTR)s_dwWorkEpldVirAddr + BSP_EPLD_VER_ADDR));
            g_ucBrdEpldVer = (0xf) & (g_ucBrdEpldVer);
        }
#if 0   /* 调试阶段可以打开此打印 */		
        printk("[CRYSTAL] EpldVirAddr:0x%lx\n",(WORDPTR)s_dwWorkEpldVirAddr);
        printk("[CRYSTAL] EpldVersion:0x%x\n",g_ucBrdEpldVer);
#endif
    }

#if 0   /* 调试阶段可以打开此打印 */
    for (dwIndex=0;dwIndex<0x100;dwIndex++)
    {
        if (dwIndex%16 == 0)
        {
            printk("0x%04x:",dwIndex);
       	}
        printk("  0x%02x",(BYTE)(*(volatile BYTE*)((WORDPTR)s_dwWorkEpldVirAddr + dwIndex)));
        if ((dwIndex+1)%16 == 0)
        {
            printk("\n");
       	}
   	}
#endif
	
    /* 在/proc下创建zte文件夹 */
    proc_zte_dir = proc_mkdir("zte", NULL);
    if (!proc_zte_dir)
    {
        printk("[CRYSTAL] proc_mkdir zte failed!\n");
        return BSP_ERROR;
    }

    /* 在/proc/zte下创建crystal文件 */
    proc_crystal = create_proc_entry("crystal",0,proc_zte_dir);
    if (proc_crystal)
    {
        proc_crystal->read_proc = k_crystal_read_proc;
    }
    else
    {
        printk("[CRYSTAL] create_proc_entry crystal failed!\n");
        return BSP_ERROR;
    }

    return BSP_OK;
}

/******************************************************************************
* 函数名称：static void __exit k_ExitCrystalModule(void)
* 功能描述：模块注销
* 输入参数：无
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期    版本号   修改人      修改内容
* -----------------------------------------------
* 20120328    V1.0     jimei       creat
******************************************************************************/
static void __exit k_ExitCrystalModule(void)
{
    if (s_dwWorkEpldVirAddr)
    {
        iounmap((void *) s_dwWorkEpldVirAddr);
    }

    if (proc_crystal)
    {
        remove_proc_entry("crystal", proc_zte_dir);
        remove_proc_entry("zte", NULL);
    }

    return;
}

module_init(k_InitCrystalModule);
module_exit(k_ExitCrystalModule);

MODULE_AUTHOR("jimei");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Blade Crystal Driver");



