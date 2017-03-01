/*********************************************************************
* ��Ȩ���� (C)2008, ����ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ� pub_oam_cfg_event.h
* �ļ���ʶ��
* ����˵���� ���ļ�������V4ƽ̨��OAM��ϵͳ����ͨ�������¼��Ż���
* ��ǰ�汾�� V1.0
* ��    �ߣ� ������
* ������ڣ�
*
* �޸ļ�¼1��
*    �޸����ڣ�2009��10��13��
*    �� �� �ţ�V1.0
*    �� �� �ˣ�������
*    �޸����ݣ�����
* �޸ļ�¼2��
*    �޸����ڣ�2011��1��17��
*    �� �� �ţ�V1.0
*    �� �� �ˣ�������
*    �޸����ݣ����ں궨��OAM_CLI_DAT_SHM_MAX_NUM��ֵ�����˸Ķ�(��20��Ϊ150)����
*              �ܴ�Ӱ�죬��Ҫͬʱ�޸�EV_OAMCLI_APP_REG_A2M_VER��ֵ����0��Ϊ1��
*              Ϊ���ݿ��ǣ���Ϊ�ṹ��T_CliAppReg�õ��˺�ֵ
**********************************************************************/

/***********************************************************
 *                    ������������ѡ��                     *
***********************************************************/
#ifndef    _PUB_OAM_CFG_EVENT_H_
#define    _PUB_OAM_CFG_EVENT_H_
/**
@file   PUB_OAM_CFG_EVENT.H
@brief  �¼��Ŷ��壬��Ԫ��Ψһ
*/
/***********************************************************
 *                   ��׼���Ǳ�׼ͷ�ļ�                    *
***********************************************************/
#ifdef __cplusplus
extern "C" {
#endif

#include "oam_typedef.h"
/***********************************************************
 *                        ��������                         *
***********************************************************/
/** @brief ������ƽ̨Ӧ��ʹ�õĹ����ڴ��  */
#define  OAM_CLI_DAT_SHM_MAX_NUM                     150
/** @brief �����Ϸ�ƽ̨ʹ�õĹ����ڴ�飬ÿ�鵥��ֻ֧��ע��һ��ҵ�����  */
#define  OAM_CLI_APP_AGT_SHM_MAX_NUM                 1
/** @brief OMP�Ϸ�ƽ̨ʹ�õĹ����ڴ��  */
#define  OAM_CLI_APP_MGT_SHM_MAX_NUM                 50
/** @brief �����ڴ������  */
#define  OAM_CLI_APP_SHM_NUM    (OAM_CLI_APP_MGT_SHM_MAX_NUM > OAM_CLI_APP_AGT_SHM_MAX_NUM ? OAM_CLI_APP_MGT_SHM_MAX_NUM : OAM_CLI_APP_AGT_SHM_MAX_NUM)
/** @brief ��󳤶ȣ�ע��:�ó��Ⱥ�ֵ���øı�(�����Կ���) */
#define MAX_STORE_LEN                 128
#define EV_SCS_BEGIN                        (WORD32)60001
/***********************************************************
 *                       ȫ�ֺ�                            *
***********************************************************/

/**
@defgroup EV_OAMCFGBEGIN OAM������Ϣ��ʼ���
@brief ��ϢID��    EV_OAMCFGBEGIN
@brief ��;��      OAM������Ϣ��ʼ��ţ�OAM�ڲ���Ϣ
@li @b ���ߣ�      ������
@li @b �ṹ�壺    ��
@li @b ת������:   ��
@li @b ����˵����  ��
@li @b �޸ļ�¼��  ��
@verbatim
version=0 ��������Ϣ
@endverbatim
@{*/

/** @brief ��ϢID  */

/** @brief ��Ϣ�汾��  */

/** @brief ��Ϣ�����ݽṹ */

/** @brief ��ת������ */

/** @} */

/**
@defgroup EV_OAMCLI_APP_REG ƽ̨Ӧ��ע��DATʱʹ��
@brief ��ϢID��    EV_OAMCLI_APP_REG
@brief ��;��      ƽ̨Ӧ��ע��DATʱʹ��
@li @b ���ߣ�      ������
@li @b �ṹ�壺    T_CliAppData
@li @b ת������:   ��
@li @b ����˵����  ��
@li @b �޸ļ�¼��  ��
@verbatim
version=0 ��������Ϣ
@endverbatim
@{*/


/** @brief ƽ̨Ӧ�õ�CLI��ϸע����Ϣ*/
typedef struct
{
    /**
    @li @b ����   tAppJid
    @li @b ����   JID
    @li @b ����   Ӧ��ע���JID
    */ 
    JID           tAppJid;
    /**
    @li @b ����   dwDatFuncType
    @li @b ����   WORD32
    @li @b ����   Ӧ��ע���DAT�Ĺ��ܰ汾����
    */ 
    WORD32        dwDatFuncType;
    /**
    @li @b ����   ucReadFlag
    @li @b ����   BYTE
    @li @b ����   Manager�Ƿ��ȡ�ı��
    */ 
    BYTE          ucReadFlag;
    /**
    @li @b ����   ucCmpFlag
    @li @b ����   BYTE
    @li @b ����   �Ƿ��Ѿ��ȽϹ��ı��
    */ 
    BYTE          ucCmpFlag;
    /**
    @li @b ����   ucSaveType
    @li @b ����   BYTE
    @li @b ����   ��������1-TXT���̣�0-ZDB����
    */ 
    BYTE          ucSaveType;
    /**
    @li @b ����   ucRegType
    @li @b ����   BYTE
    @li @b ����   ע������ 1-ƽ̨ע��; 2-SBC��ҵ��ע��
    */ 
    BYTE          ucRegType;
} T_CliAppData;

/**
@defgroup EV_OAMCLI_APP_UNREG ƽ̨Ӧ��ע��DATʱʹ��
@brief ��ϢID��    EV_OAMCLI_APP_UNREG
@brief ��;��      ƽ̨Ӧ��ע��DATʱʹ��
@li @b ���ߣ�      ������
@li @b �ṹ�壺    �� EV_OAMCLI_APP_REG ����ͬһ���ṹ�壬��T_CliAppData
@li @b ת������:   ��
@li @b ����˵����  ��
@li @b �޸ļ�¼��  ��
@verbatim
version=0 ��������Ϣ
@endverbatim
@{*/

/** @brief ��Ϣ�����ݽṹ */


/**
@defgroup EV_OAMCLI_APP_REGEX ��ƽ̨Ӧ��ע����Ҫ���ý���ʱʹ��
@brief ��ϢID��    EV_OAMCLI_APP_REGEX
@brief ��;��      ��ƽ̨Ӧ��ע����Ҫ���ý���ʱʹ��
@li @b ���ߣ�      ������
@li @b �ṹ�壺    T_CliAppDataEx
@li @b ת������:   ��
@li @b ����˵����  ��
@li @b �޸ļ�¼��  ��
@verbatim
version=0 ��������Ϣ
@endverbatim
@{*/

/** @brief ҵ��Ӧ�õ�CLI��ϸע����Ϣ */
typedef struct
{
    /**
    @li @b ����   tAppJid
    @li @b ����   JID
    @li @b ����   Ӧ��ע���JID
    */ 
    JID         tAppJid;
    /**
    @li @b ����   dwNetId
    @li @b ����   WORD32
    @li @b ����   Ӧ��ע�����Ԫ��ʶ
    */ 
    WORD32      dwNetId;
    /**
    @li @b ����   ucAgingTime
    @li @b ����   BYTE
    @li @b ����   ��ע����Ϣ�ϻ�ʱ��������������3��ͬ��ʱ�䣬����Ϊ�ϻ�
    */ 
    BYTE        ucAgingTime;
    /**
    @li @b ����   ucPad
    @li @b ����   BYTE[]
    @li @b ����   ����ֽ�
    */ 
    BYTE        ucPad[3];
} T_CliAppDataEx;

/**
@defgroup EV_OAMCLI_APP_REG_A2M CliAgent��Manager��ʱ���͵�ƽ̨Ӧ��ע����Ϣ
@brief ��ϢID��    EV_OAMCLI_APP_REG_A2M
@brief ��;��      CliAgent��Manager��ʱ���͵�ƽ̨Ӧ��ע����Ϣ
@li @b ���ߣ�      ������
@li @b �ṹ�壺    T_CliAppReg
@li @b ת������:   ��
@li @b ����˵����  ��
@li @b �޸ļ�¼��  2011-1-17 ���ں궨��OAM_CLI_DAT_SHM_MAX_NUM��ֵ���޸ģ����ǵ����ݣ���Ҫ�޸���Ϣ�汾��
@verbatim
version=0 ��������Ϣ
@endverbatim
@{*/


/** @brief ƽ̨Ӧ�õ�CLIע���ṹ */
typedef struct
{
    /**
    @li @b ����   dwSemid
    @li @b ����   WORD32
    @li @b ����   �ź���
    */ 
    WORD32          dwSemid;
    /**
    @li @b ����   platpid
    @li @b ����   pid_t
    @li @b ����   �������pid�������ж��Ƿ�Ҫ�干���ڴ�
    */ 
    pid_t           platpid;
    /**
    @li @b ����   dwRegCount
    @li @b ����   WORD32
    @li @b ����   ��ǰע�����
    */ 
    WORD32          dwRegCount;
    /**
    @li @b ����   tCliReg
    @li @b ����   T_CliAppData[]
    @li @b ����   ע������
    */ 
    T_CliAppData    tCliReg[OAM_CLI_DAT_SHM_MAX_NUM];
} T_CliAppReg;

/**
@defgroup EV_OAMCLI_APP_REGEX_A2M CliAgent��Manager��ʱ���͵ķ�ƽ̨Ӧ��ע����Ϣ
@brief ��ϢID��    EV_OAMCLI_APP_REGEX_A2M
@brief ��;��      CliAgent��Manager��ʱ���͵ķ�ƽ̨Ӧ��ע����Ϣ
@li @b ���ߣ�      ������
@li @b �ṹ�壺    T_CliAppAgtRegEx
@li @b ת������:   ��
@li @b ����˵����  ��
@li @b �޸ļ�¼��  ��
@verbatim
version=0 ��������Ϣ
@endverbatim
@{*/


/** @brief ҵ��Ӧ�õ�CLIע���ṹ */
typedef struct
{
    /**
    @li @b ����   dwSemid
    @li @b ����   WORD32
    @li @b ����   �ź���
    */ 
    WORD32          dwSemid;
    /**
    @li @b ����   apppid
    @li @b ����   pid_t
    @li @b ����   �������pid�������ж��Ƿ�Ҫ�干���ڴ�
    */ 
    pid_t           apppid;
    /**
    @li @b ����   dwRegCount
    @li @b ����   WORD32
    @li @b ����   ��ǰע�����
    */ 
    WORD32          dwRegCount;
    /**
    @li @b ����   tCliReg
    @li @b ����   T_CliAppDataEx[]
    @li @b ����   ע������
    */ 
    T_CliAppDataEx  tCliReg[OAM_CLI_APP_SHM_NUM];
} T_CliAppAgtRegEx;

/**
@defgroup EV_OAM_CFG_CFG CliManager ��Ӧ��ֱ��������Ϣ
@brief ��ϢID��    EV_OAM_CFG_CFG
@brief ��;��      CliManager ��Ӧ��ֱ��������Ϣ
@li @b ���ߣ�      ������
@li @b �ṹ�壺    MSG_COMM_OAM
@li @b ת������:   ��
@li @b ����˵����  ��
@li @b �޸ļ�¼��  ��
@verbatim
version=0 ��������Ϣ
@endverbatim
@{*/


/** @brief ����TLV���ݽṹ������MSG_COMM_OAM�ṹ�� */
typedef struct 
{
    /**
    @li @b ����   ParaNo
    @li @b ����   BYTE
    @li @b ����   Prarmeter Code Number
    */ 
    WORD16        ParaNo;         
    /**
    @li @b ����   Type
    @li @b ����   BYTE
    @li @b ����   Prarmeter Type
    */ 
    BYTE        Type;
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

/** @brief CLIͨ����Ӧ�ü�Ľӿ����ݽṹ */
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
    WORD16        Number;         
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

/** @brief ����ṹ����TLV��ʽ�ģ�������Ǽ��ݸ���ģ�����Ҫת������ */

/** @} */

/**
@defgroup EV_TXTCFG_REQ txt�������ݻָ���������Ϣ
@brief ��ϢID��    EV_TXTCFG_REQ
@brief ��;��      txt�������ݻָ���������Ϣ
@li @b ���ߣ�      ������
@li @b �ṹ�壺    ��
@li @b ת������:   ��
@li @b ����˵����  ��
@li @b �޸ļ�¼��  ��
@verbatim
version=0 ��������Ϣ
@endverbatim
@{*/

/** @brief ��Ϣ�����ݽṹ */

/** @brief ��ת������ */

/** @} */

/**
@defgroup EV_TXTCFG_ACK txt�������ݻָ��Ļָ���Ϣ
@brief ��ϢID��    EV_TXTCFG_ACK
@brief ��;��      txt�������ݻָ��Ļָ���Ϣ
@li @b ���ߣ�      ������
@li @b �ṹ�壺    ��
@li @b ת������:   ��
@li @b ����˵����  ��
@li @b �޸ļ�¼��  ��
@verbatim
version=0 ��������Ϣ
@endverbatim
@{*/
/** @brief ��Ϣ�����ݽṹ */

/** @brief ��ת������ */

/** @} */


/**
@defgroup EV_OAM_CEASE_TO_APP ���͵�Ӧ�õ���ֹ��Ϣ
@brief ��ϢID��    EV_OAM_CEASE_TO_APP
@brief ��;��      ���͵�Ӧ�õ���ֹ��Ϣ
@li @b ���ߣ�      ������
@li @b �ṹ�壺    �� EV_OAM_CFG_CFG ��Ϣ��һ����
@li @b ת������:   �� EV_OAM_CFG_CFG ��Ϣ��һ����
@li @b ����˵����  ��
@li @b �޸ļ�¼��  ��
@verbatim
version=0 ��������Ϣ
@endverbatim
@{*/

/** @brief ��Ϣ�����ݽṹ */

/** @brief ��ת������ */

/** @} */

/**
@defgroup EV_OAM_CFG_PLAN Ӧ���ϱ�����ִ�н��ȵ���Ϣ
@brief ��ϢID��    EV_OAM_CFG_PLAN
@brief ��;��      Ӧ���ϱ�����ִ�н��ȵ���Ϣ
@li @b ���ߣ�      ������
@li @b �ṹ�壺    T_Cmd_Plan_Cfg
@li @b ת������:   ��
@li @b ����˵����  ��
@li @b �޸ļ�¼��  ��
@verbatim
version=0 ��������Ϣ
@endverbatim
@{*/


/** @brief ƽ̨Ӧ�ý����ϱ���Ϣ�����ݽṹ */
typedef struct
{
    /**
    @li @b ����   wPlanNo
    @li @b ����   WORD16
    @li @b ����   �����ϱ���Ϣ���к�
    */  
    WORD16      wPlanNo;
    /**
    @li @b ����   ucPlanValue
    @li @b ����   BYTE
    @li @b ����   ����ֵ 0-100
    */  
    BYTE        ucPlanValue;
    /**
    @li @b ����   ucPad
    @li @b ����   BYTE[]
    @li @b ����   ����ֽ�
    */  
    BYTE        ucPad[2];
    /**
    @li @b ����   dwCmdID
    @li @b ����   DWORD
    @li @b ����   �����ϱ�������ID
    */  
    DWORD       dwCmdID;
    /**
    @li @b ����   dwLinkChannel
    @li @b ����   DWORD
    @li @b ����   ��ǰ��·�ţ�ִ������ʱ������ͨ�ýṹ��MSG_COMM_OAM�д���Ӧ��
    */  
    DWORD       dwLinkChannel;
} T_Cmd_Plan_Cfg;


/**
@defgroup EV_OAMCLI_SPECIAL_REG SBC��ҵ��ע�����Ϣ
@brief ��ϢID��    EV_OAMCLI_SPECIAL_REG
@brief ��;��      SBC��ҵ��ע�����Ϣ
@li @b ���ߣ�      ������
@li @b �ṹ�壺    ��ƽ̨Ӧ��һ����T_CliAppData
@li @b ת������:   ��
@li @b ����˵����  ��
@li @b �޸ļ�¼��  ��
@verbatim
version=0 ��������Ϣ
@endverbatim
@{*/

/** @brief ��Ϣ�����ݽṹ */

/** @brief ƽ̨Ӧ��ע��DAT��Ϣ�ṹ�����ת������ */

/** @} */

/**
@defgroup EV_OAMCFGEND OAM������ֹ���
@brief ��ϢID��    EV_OAMCFGEND
@brief ��;��      OAM������ֹ���
@li @b ���ߣ�      ������
@li @b �ṹ�壺    ��
@li @b ת������:   ��
@li @b ����˵����  ��
@li @b �޸ļ�¼��  ��
@verbatim
version=0 ��������Ϣ
@endverbatim
@{*/

/** @brief ��ϢID  */
#define EV_OAMCFGEND                            (WORD32)(EV_OAM_BEGIN + 1699)

/** @brief ��Ϣ�汾��  */

/** @brief ��Ϣ�����ݽṹ */

/** @brief ��ת������ */

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* end of _PUB_OAM_CFG_EVENT_H_ */

