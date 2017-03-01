/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�vnet_mid.h
* �ļ���ʶ��
* ����ժҪ��VNET ���̺���Ϣ��Ԫ����
* ��ǰ�汾��1.0
* ��    �ߣ�
* ������ڣ�2013��1��18��
*
* �޸ļ�¼1��
*     �޸����ڣ�2013/1/18
*     �� �� �ţ�V1.0
*     �� �� �ˣ�Wang.Lule
*     �޸����ݣ�����
*
******************************************************************************/
#if !defined(COMMON_VNET_MID_INCLUDE_H__)
#define COMMON_VNET_MID_INCLUDE_H__
namespace zte_tecs
{

/* VNM ���̼���Ϣ��Ԫ */
#define PROC_VNM                            "proc_vnm"
#define MU_VNM                              "vnm"
#define MU_SWITCH_MANAGER                   "switch_manager"
#define MU_VNM_SCHEDULE                     "vnm_schedule"
#define MU_VNM_QUERY                        "vnm_query"       // vnm �ṩ���ⲿxmlrpc�ӿ���Ϣ
//#define MU_VNM_MGR                          "vnm_mgr"         // vnm �Ͻ���vna�ϱ���Ϣ

#define MU_VNM_VNA_REPORT                   "vnm_vna_report"  // vnm �Ͻ���vna�ϱ���Ϣ

/* VNA ���̼���Ϣ��Ԫ */
#define PROC_VNA                            "proc_vna"
#define MU_VNA_CONTROLLER                   "vna_controller"
#define MU_VNA_AGENT                        "vna_agent"

#define MU_VNA_HEARTBEAT                    "vna_heartbeat"
#define MU_VNET_WDG                         "wdg"
#define PROC_WDG                            "proc_wdg"

#define MU_VNET_PORAGENT                    "portagent"

/* �鲥�� */
#define MUTIGROUP_VNA_REG                   "mg_vna_reg"        // ����VNM�����룬��������������Ϣ
#define MUTIGROUP_VNA_REG_SYSTEM            "_vna_reg_system"   
#define MUTIGROUP_VNA_OF_VNM                "mg_vna_of_vnm"     // VNM�����й�����VNA������
#define MUTIGROUP_VNM_MS                    "mg_vnm_ms"         // VNM����������ͨ��APPȺ

}
using namespace zte_tecs;
#endif // ifnedf COMMON_MID_H

