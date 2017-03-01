/*********************************************************************
* ��Ȩ���� (C)2009, ����ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ� private_snmp_interface_event.h
* �ļ���ʶ�� 
* ����˵���� ���ļ�������V4ƽ̨oam��ϵͳsnmpģ����¼��Ż���
* ��ǰ�汾�� V1.0
* ��    �ߣ� ����
* ������ڣ� 
*
* �޸ļ�¼1��
*    �޸����ڣ�2010��4��9��
*    �� �� �ţ�V1.0
*    �� �� �ˣ�����
*    �޸����ݣ�����
**********************************************************************/

/***********************************************************
 *                    ������������ѡ��                     *
***********************************************************/
#ifndef    _PRIVATE_SNMP_INTERFACE_EVENTDEF_H_
#define    _PRIVATE_SNMP_INTERFACE_EVENTDEF_H_
/**
@file   PRIVATE_SNMP_INTERFACE_EVENT.H
@brief  �¼��Ŷ��壬��Ԫ��Ψһ
*/

#ifndef _PACKED_1_
 #ifndef VOS_WINNT
  #define _PACKED_1_  __attribute__((packed))
 #else
  #define _PACKED_1_
 #endif
#endif

#if defined(_ARCHITECTURE_)
    #pragma noalign     /* noalign for ic386 */
#elif defined(__BORLANDC__)
    #pragma option -a- /* noalign for Borland C */
#elif defined(_MSC_VER)
    #pragma pack(1)    /* noalign for Microsoft C */
#elif defined(__WATCOMC__)
    #pragma pack(1)    /* noalign for Watcom C */
#elif defined(__DIAB)
    #pragma pack(1)    /* noalign for psosystem C */
#endif

/***********************************************************
 *                   ��׼���Ǳ�׼ͷ�ļ�                    *
***********************************************************/
#include "oam_eventdef.h"
#include "pub_oam_event.h"
#include "oam_ddm_pub.h"
#include "pub_oam_event.h"
#include "pub_oam_ddm_event.h" 
#include "pub_oam_snmp_event.h"
/* #include "pub_ipmi_event.h" */
/**
@defgroup EV_SNMP_DDM_FUNC_REQ SNMPЭ�鴦��ģ����SNMP�ӿ�ģ�鷢�ͷַ�������Ϣ
@brief ��ϢID��    EV_SNMP_DDM_FUNC_REQ
@brief ��;��      SNMPЭ�鴦��ģ����SNMP�ӿ�ģ�鷢�ͷַ�������Ϣ
@li @b ���ߣ�      ����
@li @b �ṹ�壺    T_SNMP_DDM_Req
@li @b ת������:   INVERT_MSG_T_SNMP_DDM_Req
@li @b ����˵����  ��
@li @b �޸ļ�¼��  ��
@verbatim
version=0 ��������Ϣ
@endverbatim
@{*/
/** @brief SNMPЭ�鴦��ģ����SNMP�ӿ�ģ�鷢�ͷַ�������ϢID  */
#define EV_SNMP_DDM_FUNC_REQ              (WORD32)(EV_SNMPBEGIN +  1)
/** @brief SNMPЭ�鴦��ģ����SNMP�ӿ�ģ�鷢�ͷַ�������Ϣ�汾�� */
#define EV_SNMP_DDM_FUNC_REQ_VER          (WORD32)0

/**
  @struct	T_SNMP_DDM_Req
  @brief	SNMPЭ�鴦��ģ����SNMP�ӿ�ģ�鷢�ͷַ�������Ϣ

  @li @b	��Ϣ����       SNMPЭ�鴦��ģ����SNMP�ӿ�ģ�鷢�ͷַ�������Ϣ
  @li @b	��Ϣ��         EV_SNMP_DDM_FUNC_REQ
  @li @b	��Ϣ����       snmp protocol->snmp interface
*/
typedef struct
{
    T_DDM_Req tDdmReq;
    JID       jobID;  /*��Ҫ���͵���JID*/
    BYTE      ucPad[4];
}_PACKED_1_ T_SNMP_DDM_Req;
/** @brief ��Ϣ�ṹ�����ת������ */
MSG_INVERT_COMMON(T_SNMP_DDM_Req, EV_SNMP_DDM_FUNC_REQ_VER)
/** @} */

/**
@defgroup EV_SNMPCLI_FUNC_MGT SNMPЭ�鴦��ģ����SNMP�ӿ�ģ�鷢�ͷַ�IPMI������Ϣ
@brief ��ϢID��    EV_SNMPCLI_FUNC_MGT
@brief ��;��      SNMPЭ�鴦��ģ����SNMP�ӿ�ģ�鷢�ͷַ�IPMI������Ϣ
@li @b ���ߣ�      ����
@li @b �ṹ�壺    T_SNMP_IPMI_Req
@li @b ת������:   INVERT_MSG_T_SNMP_IPMI_Req
@li @b ����˵����  ��
@li @b �޸ļ�¼��  ��
@verbatim
version=0 ��������Ϣ
@endverbatim
@{*/
/** @brief SNMPЭ�鴦��ģ����SNMP�ӿ�ģ�鷢�ͷַ�IPMI������ϢID  */
#define EV_SNMPCLI_FUNC_MGT              (WORD32)(EV_SNMPBEGIN +  2)
/** @brief SNMPЭ�鴦��ģ����SNMP�ӿ�ģ�鷢�ͷַ�IPMI������Ϣ�汾�� */
#define EV_SNMPCLI_FUNC_MGT_VER          (WORD32)0

/**
  @struct	T_SNMP_IPMI_Req
  @brief	SNMPЭ�鴦��ģ����SNMP�ӿ�ģ�鷢�ͷַ�IPMI������Ϣ

  @li @b	��Ϣ����       SNMPЭ�鴦��ģ����SNMP�ӿ�ģ�鷢�ͷַ�IPMI������Ϣ
  @li @b	��Ϣ��         EV_SNMPCLI_FUNC_MGT
  @li @b	��Ϣ����       snmp protocol->snmp interface
*/
typedef struct
{
	JID       jobID;  /*��Ҫ���͵���JID*/
    BYTE      ucPad[4];
    T_TLV_SNMP_REQUEST tIpmiReq;
}_PACKED_1_ T_SNMP_IPMI_Req;
/** @brief ��Ϣ�ṹ�����ת������ */
MSG_INVERT_COMMON(T_SNMP_IPMI_Req, EV_SNMPCLI_FUNC_MGT_VER)
/** @} */

#if defined(_ARCHITECTURE_)
    #pragma align      /* align for ic386 */
#elif defined(__BORLANDC__)
    #pragma option -a  /* align for Borland C */
#elif defined(_MSC_VER)
    #pragma pack()     /* align for Microsoft C */
#elif defined(__WATCOMC__)
    #pragma pack()     /* align for Watcom C */
#elif defined(__DIAB)
    #pragma pack()     /* align for psosystem C */
#endif
#endif /* end of _PRIVATE_OAM_DDM_EVENTDEF_H_ */
/**  @endcond */

