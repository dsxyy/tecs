/**********************************************************
* ��Ȩ���� (C)2008������������ͨѶ�ɷ����޹�˾
* 
* �ļ����ƣ�BladeCrystal.c
* �ļ���ʶ��
* ����ժҪ����Ƭ���徧��״̬��ѯ����
* ����˵����Ŀǰ��֧��SBCJ���壬SBCWû����Ҫ��ѯ�ľ�������������ο�SBCJ��ʵ�֣�������Ӧ��֧��
* ��ǰ�汾��V1.0
* ��    �ߣ�jimei
* ������ڣ�20120328
*---------------------------------------------------------
* �޸ļ�¼�� ���ڣ�    �޸��ˣ�   �޸�ԭ��
*---------------------------------------------------------
*           20120328   jimei      ����
**********************************************************/


/**************************************************************************
*                        ͷ�ļ�                                           *
**************************************************************************/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/io.h>
#include <linux/string.h>


/**************************************************************************
*                        �궨��                                           *
**************************************************************************/
#undef NULL
#define NULL       (void *)(0)

/* ����ֵ�� */
#define BSP_OK                         (0)             /* �����룬�����ɹ� */
#define BSP_ERROR                      ((WORD32)(-1))  /* �����룬����ʧ�� */
#define BSP_NOT_SUPPORT                ((WORD32)(-2))  /* �����룬�ӿڲ�֧�� */

/* ����λ�ú� */
#define BSP_PANEL_BOARD                (0x00)    /* ĸ�� */

/* �������Ƴ��Ⱥ� */
#define CRYSTAL_NAME_LEN               (16)
/* ������������� */
#define CRYSTAL_TABLE_MAX_NUM          (16)      /* ���ĳ������ľ���������ڴ�ֵ����Ҫ����ֵ���� */

/* ����״̬�� */
#define BSP_CRYSTAL_MODE_NORMAL        (0)       /* ״̬���� */
#define BSP_CRYSTAL_MODE_ABNORMAL      (1)       /* ״̬�쳣 */
#define BSP_CRYSTAL_NOT_SUPPORT        (2)       /* ״̬��ѯ��֧�� */

/* ����� */
#define BSP_BOARD_SBCJ                 (1)

/* �Ĵ�����ַ�� */
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
*                        ���ݽṹ                                         *
**************************************************************************/
typedef unsigned char           BYTE;
typedef unsigned short          WORD16;
typedef unsigned int            WORD32;
typedef unsigned long long int  WORD64;
typedef unsigned long int       WORDPTR; 

typedef struct
{   
    /* �ṹԭʼ�������������������ֶ�����������ı����ֶ� */
    WORD32 dwIndex;             /* ���,��1��ʼ */	
    BYTE   ucCrystsalName[CRYSTAL_NAME_LEN];  /* ��������:�ַ������ɵ����������� */	
    WORD32 dwLocation;          /* ����λ��,ĸ�塢ĸ�幦���ӿ�1~8��ĸ��CPU�ӿ�1~4����忨����忨�ӿ�1~4 */ 
    WORD32 dwMode;              /* ����״̬:
                                     BSP_CRYSTAL_MODE_NORMAL    ״̬����
                                     BSP_CRYSTAL_MODE_ABNORMAL  ״̬�쳣
                                     BSP_CRYSTAL_NOT_SUPPORT    ״̬��ѯ��֧�� */

    /*�����ֶκ������ֶ����������ֶξ����ѱ����ֶ�*/
    WORDPTR dwReserve[4];           
    /*End of �����ֶκ������ֶ�����*/	
}T_BSP_CRYSTAL_STATUS;

/**************************************************************************
*                        ȫ�ֱ���                                         *
**************************************************************************/
void __iomem *s_dwWorkEpldVirAddr = NULL;
BYTE g_ucBrdEpldVer = 0;  /* ģ������ʱ��ȡĸ��EPLD�汾�����ڴ�ȫ�ֱ����� */
BYTE g_ucBrdType = 0;     /* ģ������ʱ��ȡ�������ͣ����ڴ�ȫ�ֱ����� */
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
*                        ����ʵ��                                         *
**************************************************************************/

/******************************************************************************
* �������ƣ�WORD32 k_GetCrystalStatus(T_BSP_CRYSTAL_STATUS *ptCrystalStatus)
* ��������������״̬��ѯ 
* ���������ptCrystalStatus ->dwIndex   ������,��1��ʼ
* ���������ptCrystalStatus ->dwMode    ����״̬
* �� �� ֵ��BSP_OK       ��ѯ�ɹ�
*           BSP_ERROR    ��ѯʧ��
* ����˵������
* �޸�����    �汾��   �޸���      �޸�����
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
                /* RTCʱ�Ӽ��,EPLD 112֮���ṩ��� */ 
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
#if 0   /* ���Խ׶ο��Դ򿪴˴�ӡ */
    printk("[CRYSTAL] Index:%d, RegVal:0x%x\n",dwIndex,ucRegValue);
#endif		      
    return BSP_OK;
}

/******************************************************************************
* �������ƣ�k_crystal_read_proc
* ������������ѯ����״̬��д��/proc/zte/crystal
* ���������page : ����д�����ݵĻ�����
*           off  : �ļ�ָ���ƫ��
*           count: ����д�������ַ���
*           data : �������ڲ�ʹ��
* ���������eof  : �ļ���������
*           start: д�ڴ�ҳ������,�÷�����,����Ƿ���С������(������һҳ4KB),��ֵΪNULL
* �� �� ֵ���������ֽ���
* ����˵������
* �޸�����    �汾��   �޸���      �޸�����
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

	/* ��ѯ����״̬ */
    for (dwIndex = 1; dwIndex <= s_ucBrdCrystalTableSize; dwIndex++)
    {
        k_GetCrystalStatus(&s_dwBrdCrystalTable[dwIndex-1]);
    }

	/* ���ļ���д�뾧��״̬ */
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

#if 0   /* ����������һҳʱ */
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
* �������ƣ�static int __init k_InitCrystalModule(void)
* ����������ģ���ʼ��
* �����������
* �����������
* �� �� ֵ����
* ����˵������
* �޸�����    �汾��   �޸���      �޸�����
* -----------------------------------------------
* 20120328    V1.0     jimei       creat
******************************************************************************/
static int __init k_InitCrystalModule(void)
{
    char x86_model_id[64];
    unsigned int *v;
    /*WORD32 dwIndex  = 0;*/

    /* ��ȡ�������ͣ��浽g_ucBrdType�� */
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

    /* �����ַӳ�� */
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
            /* ��ȡEPLD�汾�ţ��浽g_ucBrdEpldVer�� */
            g_ucBrdEpldVer = (BYTE)(*(volatile BYTE*)((WORDPTR)s_dwWorkEpldVirAddr + BSP_EPLD_VER_ADDR));
            g_ucBrdEpldVer = (0xf) & (g_ucBrdEpldVer);
        }
#if 0   /* ���Խ׶ο��Դ򿪴˴�ӡ */		
        printk("[CRYSTAL] EpldVirAddr:0x%lx\n",(WORDPTR)s_dwWorkEpldVirAddr);
        printk("[CRYSTAL] EpldVersion:0x%x\n",g_ucBrdEpldVer);
#endif
    }

#if 0   /* ���Խ׶ο��Դ򿪴˴�ӡ */
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
	
    /* ��/proc�´���zte�ļ��� */
    proc_zte_dir = proc_mkdir("zte", NULL);
    if (!proc_zte_dir)
    {
        printk("[CRYSTAL] proc_mkdir zte failed!\n");
        return BSP_ERROR;
    }

    /* ��/proc/zte�´���crystal�ļ� */
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
* �������ƣ�static void __exit k_ExitCrystalModule(void)
* ����������ģ��ע��
* �����������
* �����������
* �� �� ֵ����
* ����˵������
* �޸�����    �汾��   �޸���      �޸�����
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



