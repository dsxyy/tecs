/**********************************************************************
 * Copyright(C)2002, ZTE Corporation.
 *
 * File Name:    kcs_com.h
 * File Mark:    
 * Description: 
 *               commom definations of kernel driver and user program.
  *Others:
 * Version:     0.1
 * Author:      zhangqiang<zhang.qiang36@zte.com.cn>
 * Date:        2011/05/25
 *

 *************************************************************************/


/******************************************************************
    Definition of user-defined data type
*******************************************************************/
#ifndef KCS_COM_H
#define KCS_COM_H

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned char        BYTE;
typedef unsigned short       WORD16;
typedef signed short         SWORD16;
typedef unsigned int         WORD32;
typedef signed int           SWORD32;
typedef char                 CHAR;
typedef void                 VOID;
typedef unsigned long long   WORD64;
typedef unsigned long int        WORDPTR;


#ifndef _PACKED_1_
#define _PACKED_1_    __attribute__ ((packed))
#endif


#define BSP_OK 		0
#define BSP_ERROR 	1
#define BSP_IOCTL_ERROR  2

/*new format warrning max payload size*/
#define MAX_PAYLOAD_SIZE    18

/*ϵͳ����ʹ�õ�CMD��*/
#define BSP_IOCMD_BRDCTRL_BASE              	             (0x00)
#define BSP_IOCMD_BRDCTRL_SEND_SEL                      (BSP_IOCMD_BRDCTRL_BASE+1)
#define BSP_IOCMD_BRDCTRL_SET_WATCHDOG                  (BSP_IOCMD_BRDCTRL_BASE+3)
#define BSP_IOCMD_BRDCTRL_FEED_WATCHDOG                 (BSP_IOCMD_BRDCTRL_BASE+4)
#define BSP_IOCMD_BRDCTRL_SEND_NEW_ALRAM                (BSP_IOCMD_BRDCTRL_BASE+5) 
#define BSP_IOCMD_BRDCTRL_GET_BOARD_ID                  (BSP_IOCMD_BRDCTRL_BASE+6)
#define BSP_IOCMD_BRDCTRL_GET_LOCATION                  (BSP_IOCMD_BRDCTRL_BASE+7)
#define BSP_IOCMD_BRDCTRL_GET_ENUM                      (BSP_IOCMD_BRDCTRL_BASE+8)




#define WATCHDOS_TYPE_LPC                                   (0x01)
#define WATCHDOG_TYPE_GPIO                                (0x02)


#define BOARDID_MagicNum                       (0x12)
#define BOARDID_CmdType_LOW_HARDID             (0x02)
#define BOARDID_CmdType_LOW_SOFTID             (0x01)
#define BOARDID_CmdType_HIGH                   (0x0)
#define BOARDID_CmdVer                         1
#define BOARDID_Front                          0x00
#define BOARDID_Back                           0x08

#define BOARD_ENUM_OPEN                         0
#define BOARD_ENUM_CLOSE                        1

#ifndef BSP_ERRCODE_BOARDCTRL
#define BSP_ERRCODE_BOARDCTRL (0x1 << 8)
#endif

/** SBC��������� */
#define  SBC_RETURN_ERROR_BASE                     (BSP_ERRCODE_BOARDCTRL )

/** SBC��ָ������� */
#define SBC_E_NULL_POINTER                         (SBC_RETURN_ERROR_BASE + 1)
#define SBC_E_BOARD_IPMC_MESSAGE_ERROR             (SBC_RETURN_ERROR_BASE + 2)
#define SBC_E_INPUT_POINTER                        (SBC_RETURN_ERROR_BASE + 3)
#define SBC_E_KMALLOC_FAILED                       (SBC_RETURN_ERROR_BASE + 4)
#define SBC_E_BRDCTRL_NOT_SUPPORT                  (SBC_RETURN_ERROR_BASE + 5)
#define SBC_E_GET_LPC_PORT_ADDR                    (SBC_RETURN_ERROR_BASE + 6)
#define SBC_E_SEND_MESSAGES_ERROR            (SBC_RETURN_ERROR_BASE + 7)
#define SBC_E_COPY_TO_USR_ERROR                (SBC_RETURN_ERROR_BASE + 8)





/*�����û���Ϣ�ṹ�壬����IPMI�淶����*/

typedef struct
{
    BYTE       ucSensorType;
    BYTE       ucSensorNum;
    BYTE       ucEventDir;
    BYTE       ucEventData1;
    BYTE       ucEventData2;
    BYTE       ucEventData3;

}_PACKED_1_ T_BSP_ALARM_BASIC_INFO;



/*����IPMI�������϶���������ʽ��װ�Ľṹ��*/
typedef struct T_BSP_NEW_REAL_DATA
{
     BYTE ucInfoType;       /*�¼����ͣ��澯��֪ͨ���ָ�*/
     BYTE ucCpuID;            /*Cpu id*/
     BYTE ucvalidDatalenth;    /*���ڶԱȵĹؼ�����,IPMI������б�׼�����ֶ�*/
     BYTE aucDataBuff[25];     /*���������25��BYTE�����ڴ�Ÿ����¼�����Ч����*/

}T_BSP_NEW_REAL_DATA;




typedef struct T_REAL_DATA_WITH_LENTH
{
    BYTE ucRealDatalenth;     /*T_BSP_NEW_REAL_DATA ���������ݻ��峤�ȣ���������Ч����*/
    BYTE ucWarning_code;     /*e_alarm_msg_type �е���������ʶ��������*/
    BYTE ucMessageType;           /*������sel�����µĸ�ʽ*/	
    T_BSP_NEW_REAL_DATA tNewRealData;

}T_REAL_DATA_WITH_LENTH;



typedef struct T_BSP_NEW_SOCKET_DATA
{
     BYTE ucVerson;          /*��sel�����¶���ĸ�ʽ*/
     BYTE ucInfoType;       /*�¼����ͣ��澯��֪ͨ���ָ�*/
     BYTE ucEventType;    /*���������¼������̵�*/ 
     BYTE ucRealDatalenth;      /*��������Ч���ȣ������T_BSP_NEW_REAL_DATA�е�ucvalidDatalenth��һ��*/
     T_BSP_NEW_REAL_DATA  tNewRealData;     /*��Ÿ����¼��ĸ澯���ݣ���ͷ��*/

}T_BSP_NEW_SOCKET_DATA;


/*����SEL��Ϣ�ṹ�壬����IPMI�淶����*/
typedef struct
{
    WORD16     usRecordId;    	
    BYTE       ucRecordType;
    WORD32     dwTimestamp;
    WORD16     usGenID;
    BYTE       ucEvMRev;
   T_BSP_ALARM_BASIC_INFO tBspAlarmBasicInfo;
}_PACKED_1_ T_BSP_ALARM_SEL_INFO;



typedef struct
{
    BYTE         ucFlag;
    BYTE         ucSeqNumber;
    WORD32   dwWarrningNum;
    BYTE         ucOffset;
    BYTE         aucData[MAX_PAYLOAD_SIZE];
}_PACKED_1_ T_BSP_ALARM_NEW_REQ;


typedef struct
{
    BYTE         ucCompletion;
    BYTE         ucFlag;
    BYTE         ucSeqNumber;	
    WORD32   dwWarrningNum;
    BYTE         ucOffset;
}_PACKED_1_ T_BSP_ALARM_NEW_REP;




/*heartbeat ������ʹ�õ����ݽṹ*/
/*IPMC ����*/

typedef struct 
{
    WORD16     wHeartFreqData;             
    BYTE          ucResetFlag;
    BYTE          ucCpuDspId;
    BYTE          ucWatchDogType;
} _PACKED_1_ T_BSP_HEARTBEAT_SET_REQ;



/*��Ӧ*/
typedef struct 
{
    BYTE   ucNetFnCmd;         /*kcs netfn cmd*/
    BYTE   ucCmd;              /*kcs responsed cmd*/
    BYTE   ucMsgOk;           /* kcs ��Ϣ��Ч�ֶ� */
    BYTE   ucRepData;              /* ��Σ�0������ʧ:1�������ָ� */
} _PACKED_1_ T_BSP_HEARTBEAT_SET_REP;




/*ι���ṹ��*/
typedef struct 
{
    //WORD16 wCPUID; //using new feed dog cmd, modified by zhangqiang 2011-12-15
     BYTE ucCPUID;
} _PACKED_1_ T_BSP_HEARTBEAT_GET_REQ;



typedef struct 
{
    BYTE   ucNetFnCmd;         /*kcs netfn cmd*/
    BYTE   ucCmd;              /*kcs responsed cmd*/
    BYTE   ucMsgOk;            /* kcs ��Ϣ��Ч�ֶ� */
} _PACKED_1_ T_BSP_HEARTBEAT_GET_REP;


/*��ѯENUM״̬*/
typedef struct
{       
    WORD16 wVersion;	  
}_PACKED_1_ T_ENUM_QUERY_REQ;


typedef struct
{                       
	BYTE ucCompleteCode;	
	BYTE ucBoardReques;	
	BYTE ucBoardStatus;	
	BYTE ucBoardPoweroffReason;
}_PACKED_1_ T_ENUM_QUERY_REP;



typedef struct T_BSP_BRDCTRL_PARA
{
    WORD32      dwCmd;                   /**< IOCRTL �� */
    WORD32      dwReturn;                /**< ����ֵ */
    WORD32      dwLen;                   /**< pParam ���������� */
    VOID        *pParam;	         /**< ������ */
	WORD32      dwRealDataLen;        /**<��������������Ч���ݳ���*/
    WORD32      dwRsvLen;                /**< ���� */
    VOID        *pRsvBuf;	         /**< ���� */
    
}T_BSP_BRDCTRL_PARA; 



typedef struct 
{
    BYTE   ucReqOperation;             
    BYTE   ucPowerStatus;   
    BYTE   ucPowerOffCause;
    WORD16 wVersion;
}T_BSP_BRDCTRL_HEARTBEAT_INFO;


/*�����ӻ�ȡ����board id ����*/
typedef struct
{
   BYTE ucMagicNum;
   BYTE aucCmdType[2];
   BYTE ucCmdVer;
   BYTE ucID;
   BYTE aucReserve[20];
}_PACKED_1_ T_CARD_INFO_REQ;


typedef struct
{
   BYTE ucCompleteCode;
   BYTE ucMagicNum;
   BYTE aucCmdType[2];
   BYTE ucCmdVer;
   BYTE aucBoardID[2];
   BYTE ucPCBID;
   BYTE ucBOMID;
   BYTE ucEPLDID;
   BYTE aucReserve[14];
   BYTE ucReadstaus;           
 /*ucReadtus, bit0=1��ʾBORAD ID ��PCB ID��BOM ID��ȡ�ɹ���
bit1=1��ʾEPLD�汾�Ŷ�ȡ�ɹ���
bit2=1��ʾ�ӿ���λ��=0��ʾ�ӿ�����λ��
��ѯĸ��ʱ����λ�����塣
����λĬ��Ϊ0��*/
}_PACKED_1_ T_CARD_INFO_REP;


#ifdef __cplusplus
}
#endif


#endif
