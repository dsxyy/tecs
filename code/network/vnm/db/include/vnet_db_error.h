

/******************************************************************************
* Copyright (c) 2013������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�vnet_db_error.h
* �ļ���ʶ��
* ����ժҪ��CVNetDbMgr��Ķ����ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�
* ������ڣ�2013��1��28��
*
* �޸ļ�¼1��
*     �޸����ڣ�2013/1/28
*     �� �� �ţ�V1.0
*     �� �� �ˣ�gong.xiefeng
*     �޸����ݣ�����
*
******************************************************************************/

#if !defined(_VNET_DB_ERROR_INCLUDE_H_)
#define _VNET_DB_ERROR_INCLUDE_H_

namespace zte_tecs
{

// VNET���ݿ�ģ����󷵻���
#define  VNET_DB_SUCCESS                     (0)
#define  VNET_DB_ERROR_START                 (0)
#define  VNET_DB_ERROR_NO_INIT               (VNET_DB_ERROR_START + 1)
#define  VNET_DB_ERROR_OPEN_DB_FAILED        (VNET_DB_ERROR_START + 2)
#define  VNET_DB_ERROR_EXECUTE_FAILED        (VNET_DB_ERROR_START + 3)
#define  VNET_DB_ERROR_ALLOC_MEM_FAILED      (VNET_DB_ERROR_START + 4)
#define  VNET_DB_ERROR_IPROCEDURE_IS_NULL    (VNET_DB_ERROR_START + 5)
#define  VNET_DB_ERROR_PROCCALLBACK_IS_NULL  (VNET_DB_ERROR_START + 6)
#define  VNET_DB_ERROR_PROCTYPE_IS_INVALID   (VNET_DB_ERROR_START + 7)
#define  VNET_DB_ERROR_PROC_ADORS_IS_NULL    (VNET_DB_ERROR_START + 8)
#define  VNET_DB_ERROR_GET_ADORS_FIELD_FAILED (VNET_DB_ERROR_START + 9)
#define  VNET_DB_ERROR_PARAM_INVALID         (VNET_DB_ERROR_START + 10)
#define  VNET_DB_ERROR_GET_DBI_FAILED        (VNET_DB_ERROR_START + 11)
#define  VNET_DB_ERROR_ITEM_NO_EXIST         (VNET_DB_ERROR_START + 12)
#define  VNET_DB_ERROR_NO_DB_HANDLE_RESOURCE (VNET_DB_ERROR_START + 13)

// ����������̴����� = �������̴�����+VNET_DB_ERROR_PROC_START
#define  VNET_DB_ERROR_PROC_START            (VNET_DB_ERROR_START + 100)
#define  GET_DB_ERROR_PROC(PROC_RTN)         (VNET_DB_ERROR_PROC_START + (PROC_RTN))

/* �������̴��󷵻��� 
   �ʹ��������з���ֵ����һ�� 
*/
#define VNET_DB_PROCEDURE_RTN_SUCCESS                     (0)
#define VNET_DB_PROCEDURE_RTN_OPERATOR_FAILED             (1)
#define VNET_DB_PROCEDURE_RTN_HAS_SAME_ITEM               (2)
#define VNET_DB_PROCEDURE_RTN_ITEM_CONFLICT               (3)
#define VNET_DB_PROCEDURE_RTN_ITEM_NO_EXIST               (4)
#define VNET_DB_PROCEDURE_RTN_NETPLANE_MTU_LESS_PG        (5)
#define VNET_DB_PROCEDURE_RTN_RESOURCE_USING              (6)
#define VNET_DB_PROCEDURE_RTN_PARAM_INVALID               (7)
#define VNET_DB_PROCEDURE_RTN_SWITCH_MTU_LESS_PG          (8)
#define VNET_DB_PROCEDURE_RTN_SWITCH_UPLINKPORT_INVALID   (9)
#define VNET_DB_PROCEDURE_RTN_PORT_HAS_JOIN_OTHER_SWITCH  (10)
#define VNET_DB_PROCEDURE_RTN_SWITCH_HAS_DEPLOY_VSI       (11)
#define VNET_DB_PROCEDURE_RTN_GET_SWITCH_VSI_NUM_FAILED   (12)
#define VNET_DB_PROCEDURE_RTN_PG_MUST_CFG_NETPLANE        (13)
#define VNET_DB_PROCEDURE_RTN_MUST_CFG_VALID_RANGE        (14)
#define VNET_DB_PROCEDURE_RTN_NO_MAC_RESOURCE             (15)
#define VNET_DB_PROCEDURE_RTN_NO_IP_RESOURCE              (16)
#define VNET_DB_PROCEDURE_RTN_NO_VLAN_RESOURCE            (17)
#define VNET_DB_PROCEDURE_RTN_UPLINKPGMTU_LESS_KERNELPG   (18)
#define VNET_DB_PROCEDURE_RTN_WILDCAST_TASK_IS_USING      (19)
#define VNET_DB_PROCEDURE_RTN_VNA_IS_ONLINE               (20)
#define VNET_DB_PROCEDURE_RTN_SDN_CFG_INVALID             (21)

// �ۺ����ݿ�ģ�������ʹ������̴�����
#define VNET_DB_IS_ITEM_NO_EXIST(DBERROR) ( ((DBERROR) == (VNET_DB_ERROR_ITEM_NO_EXIST)) || ((DBERROR) == (GET_DB_ERROR_PROC(VNET_DB_PROCEDURE_RTN_ITEM_NO_EXIST))) )
#define VNET_DB_IS_RESOURCE_USING(DBERROR) ((DBERROR) == (GET_DB_ERROR_PROC(VNET_DB_PROCEDURE_RTN_RESOURCE_USING)))
} // namespace zte_tecs

#endif 
