/*********************************************************************
* ��Ȩ���� (C)2002, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ�oam_cfg.h
* �ļ���ʶ��
* ����ժҪ��OAM����ģ�����ӿڽṹ�嶨��
* ����˵����
            
* ��ǰ�汾��
* ��    �ߣ����
* ������ڣ�2008.9.18
*
* �޸ļ�¼1��// �޸���ʷ��¼�������޸����ڡ��޸��߼��޸�����
*    �޸����ڣ�
*    �� �� �ţ�
*    �� �� �ˣ�
*    �޸����ݣ�
* �޸ļ�¼2����
**********************************************************************/
/**
  @file OAM_CFG.H
  @brief CLI�ӿڵ����͡�����ֵ�Լ��������ýӿ���صĺ궨��
  @page OAM_CFG OAM��ϵͳ������ģ�飩�ӿڹ淶
  @author  OAM����3Gƽ̨

���ĵ������������ģ��Ľӿ����ݡ�\n

*/
#ifndef __OAM_CFG_H__
#define __OAM_CFG_H__

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

/***********************************************************
 *                    ������������ѡ��                     *
***********************************************************/

/***********************************************************
 *                   ��׼���Ǳ�׼ͷ�ļ�                    *
***********************************************************/
#include "oam_typedef.h"

/***********************************************************
 *                        ��������                         *
***********************************************************/

#define MAX_TABLE_FIELD               50   /**< table���Ͳ���������ֶ���Ŀ����дtable���Ͳ�����ʱ��Ҫע�� */
#define MAX_TABLE_ITEM                100  /**< table���Ͳ�����һ�������Է���ô���¼�� */
#define OAM_CFG_MIN_PARANO            0    /**< OPR_DATA �ṹ��ParaNo��Сֵ */
#define OAM_CFG_MAX_PARANO            254  /**< OPR_DATA �ṹ��ParaNo���ֵ */
#define MAX_STORE_LEN                 128  /**< max backup data length from protocol */

/***********************************************************
 *                       ȫ�ֺ�                            *
***********************************************************/
/*---------------------------------------------------
                     ��BRS��txt�������Ľ��
--------------------------------------------------*/
#define  OAM_TXT_SUCCESS            0  /**< �ɹ� */
#define  OAM_TXT_READ_ERROR         1  /**< ��txt�ļ����� */
#define  OAM_TXT_NO_DAT             2  /**< �޵�ǰDAT�ļ� */
#define  OAM_TXT_CRC_ERROR          3  /**< txt�ļ�CRCУ����� */
#define  OAM_TXT_PARSE_ERROR        4  /**< ������������� */
#define  OAM_TXT_INTERPRET_SLAVE    5  /**< ���ͳ����ڱ�״̬ */
#define  OAM_TXT_TIMEOUT            6  /**< ���ͳ�ʱ */
#define  OAM_TXT_CFGAGENT_UNKNOWN   7  /**< ���ô���δ֪����*/

/*---------------------------------------------------
                     ������������
--------------------------------------------------*/
#define DATA_TYPE_BYTE              (BYTE)1     /**< BYTE */
#define DATA_TYPE_WORD              (BYTE)2     /**< WORD16 */
#define DATA_TYPE_DWORD             (BYTE)3     /**< DWORD */
#define DATA_TYPE_INT               (BYTE)4     /**< INT */
#define DATA_TYPE_CHAR              (BYTE)5     /**< CHAR */
#define DATA_TYPE_STRING            (BYTE)6     /**< STRING */
#define DATA_TYPE_TEXT              (BYTE)7     /**< text : many a string  */
#define DATA_TYPE_TABLE             (BYTE)8     /**< represent it is a table varible */
#define DATA_TYPE_IPADDR            (BYTE)9     /**< ip address */
#define DATA_TYPE_DATE              (BYTE)10    /**< DATE = the number of the days from 1900/1/1 */
#define DATA_TYPE_TIME              (BYTE)11    /**< TIME = the number of the seconds from 2000/1/1/00:00:00*/
#define DATA_TYPE_MAP               (BYTE)12    /**< Mapping Variable */
#define DATA_TYPE_LIST              (BYTE)13    /**< represent it is a special Table varible */
#define DATA_TYPE_CONST             (BYTE)14    /**< represent constant variable */
#define DATA_TYPE_IFPORT            (BYTE)15    /**< represent interface port type */
#define DATA_TYPE_HEX               (BYTE)16    /**< 1234.2fcd.e34d */
#define DATA_TYPE_MASK              (BYTE)17    /**< IP MASK */
#define DATA_TYPE_IMASK             (BYTE)18    /**< IP INVERSE MASK */
#define DATA_TYPE_MACADDR           (BYTE)19    /**< Mac Address */
#define DATA_TYPE_FLOAT             (BYTE)20    /**< Float */
#define DATA_TYPE_BOARDNAME         (BYTE)21    /**< BOARDNAME */
#define DATA_TYPE_IPV6PREFIX        (BYTE)22    /**< IPV6 PREFIX */
#define DATA_TYPE_IPV6ADDR          (BYTE)23    /**< IPV6 ADDR */
#define DATA_TYPE_VPN_ASN           (BYTE)24    /**< ASN:nn */
#define DATA_TYPE_VPN_IP            (BYTE)25    /**< IP:nn */
#define DATA_TYPE_PHYADDRESS        (BYTE)26    /**< T_PhysAddress: rack-shelf-slot-cpu*/
#define DATA_TYPE_LOGICADDRESS    (BYTE)27      /**< T_LogicalAddr: system-subsystem-module-unit-sunit-index*/
#define DATA_TYPE_WORD64            (BYTE)28    /**< WORD64 */
#define DATA_TYPE_SWORD             (BYTE)29    /**< SWORD */
#define DATA_TYPE_LOGIC_ADDR_M      (BYTE)30    /**< LogicalAddrM */
#define DATA_TYPE_LOGIC_ADDR_U      (BYTE)31    /**< LogicalAddrU */
#define DATA_TYPE_IPVXADDR          (BYTE)32    /**< IPVXADDR */
#define DATA_TYPE_T_TIME            (BYTE)33    /**< T_TIME */
#define DATA_TYPE_SYSCLOCK          (BYTE)34    /**< SYSCLOCK */
#define DATA_TYPE_VMMDATETIME        (BYTE)35   /**< vmm����İ汾����ʱ������*/
#define DATA_TYPE_VMMDATE            (BYTE)36   /**< vmm����İ汾������������*/

#define DATA_TYPE_PARA              (BYTE)254    /**< Add for [] and whole by wangmh */

/*---------------------------------------------------
                     �������ݳ���
--------------------------------------------------*/
#define LEN_IPADDR                  4   /**< ipaddr�洢���� */
#define LEN_MACADDR                 6   /**< Mac address�洢���� */
#define LEN_IPV6ADDR                16  /**< ipv6��ַ�洢���� */
#define VPN_NAME_LEN                32  /**< VPN�洢���� */

/*---------------------------------------------------
              ���ģʽ ����Ϣͷ����OutputMode��
--------------------------------------------------*/
#define OUTPUT_MODE_NORMAL          0    /**< ���ģʽ:��ͨ */
#define OUTPUT_MODE_ROLL            1    /**< ���ģʽ:�ع� */
#define OUTPUT_MODE_REQUEST         2    /**< ���ģʽ:���� */
#define OUTPUT_MODE_WAIT            3    /**< ���ģʽ:�ȴ� */

/*---------------------------------------------------
        ��Ϣ�ṹ��ReturnCode����ĳɹ��������Ϣ
--------------------------------------------------*/
#define OAM_CFG_RETURNCODE               0                      /**< ��������ʼֵ */
#define ERR_NO_NEED_THE_CMD             (OAM_CFG_RETURNCODE+0)  /**< ���������Ҫ */
#define SUCC_AND_NOPARA                 (OAM_CFG_RETURNCODE+1)  /**< ����ִ�гɹ����޻��� */
#define SUCC_AND_HAVEPARA               (OAM_CFG_RETURNCODE+2)  /**< ����ִ�гɹ����л��� */
#define SUCC_CMD_NOT_FINISHED           (OAM_CFG_RETURNCODE+3)  /**< ������δִ����� */
#define SUCC_SND_PING_NOT_FINISHED      (OAM_CFG_RETURNCODE+32) /**< PING������δ���� */

#define SUCC_CMD_DISPLAY_AND_KILLTIMER  (OAM_CFG_RETURNCODE+4)  /**< ������δִ�����, �رս��Ͷ�ʱ��, ��Ҫ���� */
#define SUCC_CMD_DISPLAY_NO_KILLTIMER   (OAM_CFG_RETURNCODE+5)  /**< ������δִ�����, ���رս��Ͷ�ʱ��, ��Ҫ���� */
#define SUCC_TERMINATE_INTERPT_TIMER    (OAM_CFG_RETURNCODE+6)  /**< ��������ִ��, �رս��Ͷ�ʱ���� ������� */

#define ERR_AND_HAVEPARA                (OAM_CFG_RETURNCODE+7)  /**< ����ִ��ʧ�ܣ��л��� */
#define ERR_MSG_FROM_INTPR              (OAM_CFG_RETURNCODE+8)  /**< ���Խ�����Ϣ�ṹ���� */

#define  CLI_RET_KEEP_ALIVE_NOPARA (OAM_CFG_RETURNCODE+9)  /**Ӧ�ñ�����Ϣ,�޻���*/

#define SUCC_WAIT_AND_HAVEPARA          (OAM_CFG_RETURNCODE+32) /**< ������δִ�����, ���Բ��ȴ� */  

#define OAM_MSG_XMLAGNET_TO_INTERPRET       (WORD32)(EV_OAMCFGBEGIN+37)  /**< XML����������ͷ�����������*/


/*---------------------------------------------------
                 ����id�� GET/SET����
--------------------------------------------------*/
#define IS_GET                      0  /**< �������ģʽ:GET */
#define IS_SET                      1  /**< �������ģʽ:SET */
#define IS_SAVE                     2  /**< �������ģʽ:SAVE */
#define IS_DEBUG                    3  /**< �������ģʽ:DEBUG */
#define IS_PROCESS                  4  /**< �������ģʽ:PROCESS */

#define OAM_CLI_DDM_CMDGRP_SET    (WORD32)0xE7000000  /**< DDM������:SET */
#define OAM_CLI_DDM_CMDGRP_GET    (WORD32)0xE8000000  /**< DDM������:GET */

#define CMD_RPU_SAVE               (DWORD)0x82020003  /* RPU�������� */

/***********************************************************
 *                     ȫ����������                        *
***********************************************************/

/**
  @struct TABLE_ITEM
  @brief ��������Ŀ���� 
  <HR>
  @b �޸ļ�¼��
  
*/
typedef struct
{
	  /**
    @li @b ����   Type
    @li @b ����   BYTE
    @li @b ����   Each Table Column Type
    */
    BYTE        Type;           
    /**
    @li @b ����   Len
    @li @b ����   BYTE
    @li @b ����   Each Table Column Len
    */
    BYTE        Len;  
    /**
    @li @b ����   ucPad1
    @li @b ����   BYTE
    @li @b ����   �������
    */          
    BYTE        ucPad1;
    /**
    @li @b ����   ucPad1
    @li @b ����   BYTE
    @li @b ����   �������
    */ 
    BYTE        ucPad2;
}TABLE_ITEM, * PTABLE_ITEM;

/**
  @struct   TABLE_DATA
  @brief    ����������
  <HR>
  @b �޸ļ�¼��
*/
typedef struct 
{
	  /**
    @li @b ����   Item
    @li @b ����   TABLE_ITEM
    @li @b ����   Table Fields Array Struct
    */ 
    TABLE_ITEM  Item[MAX_TABLE_FIELD];
    /**
    @li @b ����   ItemNumber
    @li @b ����   BYTE
    @li @b ����   Table Fields Number
    */
    BYTE        ItemNumber; 
    /**
    @li @b ����   ucPad1
    @li @b ����   BYTE
    @li @b ����   �������
    */            
    BYTE        ucPad1;
    /**
    @li @b ����   ucPad1
    @li @b ����   BYTE
    @li @b ����   �������
    */
    BYTE        ucPad2;   
    /**
    @li @b ����   Data
    @li @b ����   BYTE[]
    @li @b ����   Table Contents
    */  
    BYTE        Data[1];                
}TABLE_DATA, * PTABLE_DATA;

#if 0
/**
  @struct   OPR_DATA
  @brief    �����ṹ��
  <HR>
  @b �޸ļ�¼��
*/
typedef struct 
{
    /* WORD16        ParaNo;         Prarmeter Code Number */
    /**
    @li @b ����   ParaNo
    @li @b ����   BYTE
    @li @b ����   Prarmeter Code Number
    */ 
    BYTE        ParaNo;         
  /*    CHAR        sName[MAX_PARANAME];      �������� */
    /**
    @li @b ����   Type
    @li @b ����   BYTE
    @li @b ����   Prarmeter Type
    */ 
    BYTE        Type;           /**<@b Prarmeter Type */
    /**
    @li @b ����   Len
    @li @b ����   BYTE
    @li @b ����   Prarmeter Length If Type equal to DATA_TYPE_TABLE, it represent the number of the records
    */ 
    BYTE        Len;  
    /**
    @li @b ����   Data
    @li @b ����   BYTE[]
    @li @b ����   Prarmeter Value If Type equal to DATA_TYPE_TABLE, it represent the struct "TABLE_DATA"
    */           
    BYTE        Data[1];        
}_PACKED_1_ OPR_DATA, * POPR_DATA;

/**
  @struct   MSG_COMM_OAM
  @brief    OAM����ͨ����Ϣ��
  @li @b    ��Ϣ����   OAM������Ϣ����/Ӧ��
  @li @b    ��Ϣ����:  OAM->3GPLAT��3GPLAT->OAM
*/
typedef struct 
{
	  /**
    @li @b ����   SeqNo
    @li @b ����   DWORD
    @li @b ����   Define the seqence number to distinct the command
    */
    DWORD       SeqNo;          
    /**
    @li @b ����   DataLen
    @li @b ����   DWORD
    @li @b ����   Post Message Total Length
    */
    DWORD       DataLen;        
    /**
    @li @b ����   CmdID
    @li @b ����   DWORD
    @li @b ����   Command ID
    */
    DWORD       CmdID;          
    /**
    @li @b ����   LinkChannel
    @li @b ����   DWORD
    @li @b ����   Define which vty do it connect
    */
    DWORD        LinkChannel;    
    /**
    @li @b ����   ReturnCode
    @li @b ����   WORD16
    @li @b ����   Success Or Fail Type
    */
    WORD16      ReturnCode;     
    /**
    @li @b ����   LastData
    @li @b ����   BYTE[]
    @li @b ����   The Last Data From Protocol
    */
    BYTE        LastData[MAX_STORE_LEN];   
    /**
    @li @b ����   BUF
    @li @b ����   BYTE[]
    @li @b ����   The Buffer
    */
    BYTE        BUF[MAX_STORE_LEN];
    /**
    @li @b ����   bIfNo
    @li @b ����   BYTE
    @li @b ����   TRUE for no-command and FALSE for normal-command
    */
    BYTE        bIfNo;          
    /**
    @li @b ����   CmdRestartFlag
    @li @b ����   BYTE
    @li @b ����   The Flag of Restart a Command
    */
    BYTE        CmdRestartFlag; 
    /**
    @li @b ����   OutputMode
    @li @b ����   BYTE
    @li @b ����   Display mode "normal","roll","request"
    */
    BYTE        OutputMode;     
    /**
    @li @b ����   Number
    @li @b ����   BYTE
    @li @b ����   Parameter Sum Number
    */
    BYTE        Number;         
    /**
    @li @b ����   ucOamTag
    @li @b ����   BYTE
    @li @b ����   OAM���⴦��ı�־�������interpret.h��Ӧ��͸��
    */  
    BYTE        ucOamTag;
    /**
    @li @b ����   aucPad
    @li @b ����   BYTE[]
    @li @b ����   �������
    */
    BYTE        aucPad[1];
    /**
    @li @b ����   Data
    @li @b ����   OPR_DATA[]
    @li @b ����   Parameter Buffer
    */
    OPR_DATA    Data[1];        
}_PACKED_1_ MSG_COMM_OAM, * PMSG_COMM_OAM;
#endif

/*job ��ں���ԭ��*/
void Oam_Interpret(WORD16 wState, WORD32 dwMsgId, LPVOID pMsgPara, LPVOID pVar, BOOLEAN bIsSameEndian/*~*/);
void Oam_TelnetServer(WORD16 wState, WORD32 dwMsgId, LPVOID pMsgPara, LPVOID pVar, BOOLEAN byteOrder/*~*/);
void Oam_SSHServer(WORD16 wState, WORD32 dwMsgId, LPVOID pMsgPara, LPVOID pVar, BOOLEAN byteOrder);

extern T_CliJobInfo * GetJobInfoByJno(WORD16 wJno);
#ifdef WIN32
    #pragma pack()
#endif
   
#endif /*__OAM_CFG_H__*/


