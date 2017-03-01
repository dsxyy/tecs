/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�vnm_vnic_db.cpp
* �ļ���ʶ��
* ����ժҪ��CVNetVmMigrateMgr��ʵ���ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�
* ������ڣ�2013��2��22��
*
* �޸ļ�¼1��
*     �޸����ڣ�2013/2/22
*     �� �� �ţ�V1.0
*     �� �� �ˣ�lverchun
*     �޸����ݣ�����
******************************************************************************/
#include "vnet_comm.h"
#include "vnet_db_mgr.h"
#include "vnet_vnic.h"
#include "vnet_db_vsi.h"
#include "vna_reg_mod.h"
#include "vnet_msg.h"
#include "vna_info.h"
#include "ipv4_addr.h"
#include "ipv4_addr_range.h"
#include "ipv4_pool_mgr.h"
#include "logic_network.h"
#include "logic_network_mgr.h"
#include "vnet_portmgr.h"
#include "vna_mgr.h"
#include "vm_migrate_mgr.h"
#include "vnet_db_vsi.h"
#include "vnet_db_vm_migrate.h"
#include "vnet_db_vm.h"
#include "vnet_error.h"

namespace zte_tecs
{
CVNetVmMigrateMgr* CVNetVmMigrateMgr::_instance = NULL;

CVNetVmMigrateMgr::CVNetVmMigrateMgr()
{
    m_pVNetDbMgr = NULL;
};

CVNetVmMigrateMgr::~CVNetVmMigrateMgr()
{
    m_pVNetDbMgr = NULL;
};

/************************************************************
* �������ƣ� Init
* ���������� ��ʼ������
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2012/5         V1.0       lvech         ����
***************************************************************/
int32 CVNetVmMigrateMgr::Init()
{
    m_pVNetDbMgr = GetDbIVmMigrate();
    if( NULL == m_pVNetDbMgr)
    {
        cout << "CVNetVmMigrateMgr::Init() GetDbIVmMigrate is NULL" << endl;
        return -1;
    }
    
    VNET_LOG(VNET_LOG_INFO, "CVNetVmMigrateMgr::Init: Call  successfully.\n");

    return 0;
} ;


/************************************************************
* �������ƣ� MigrateBegin
* ���������� ����vm migrate bgin
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2012/5         V1.0       lvech         ����
***************************************************************/
int32 CVNetVmMigrateMgr::MigrateBegin(CVNetVmMigrateBeginMsg &msg)
{
    // 0 �������
    if( (0 == msg.m_src_cc_application.compare("")) || \
        (0 == msg.m_src_hc_application.compare("")) || \
        (0 == msg.m_dst_cc_application.compare("")) || \
        (0 == msg.m_dst_hc_application.compare(""))  )
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetVmMigrateMgr::MigrateBegin: param is NULL.\n",\
              msg.m_vm_id);
        return ERROR_VNM_VM_MIGRATE_PARAM_INVALID;    
    }    

    VNET_LOG(VNET_LOG_INFO, "vnet recv MigrateBegin msg(%lld, %s,%s ---> %s,%s )\n",\
              msg.m_vm_id, msg.m_src_cc_application.c_str(),msg.m_src_hc_application.c_str(),\
              msg.m_dst_cc_application.c_str(),msg.m_dst_hc_application.c_str());

    // 1 �Ƿ�����Ѵ��ڸ������
    int32 bDbExist = false;
    CDbVmMigrate info;
    info.SetVmId(msg.m_vm_id);
    int32 nRet = m_pVNetDbMgr->Query(info);
    if( nRet == VNET_DB_SUCCESS)
    {            
        bDbExist = true;              

        if(info.GetState() == VNET_VM_MIGRATE_INIT || 
           info.GetState() == VNET_VM_MIGRATE_BEGIN )
        {
            // �Ƚ����Ƿ�һ�� 
            if( (0 == msg.m_src_cc_application.compare(info.GetSrcClusterName())) &&
                (0 == msg.m_src_hc_application.compare(info.GetSrcHostName())) &&
                (0 == msg.m_dst_cc_application.compare(info.GetDstClusterName())) &&
                (0 == msg.m_dst_hc_application.compare(info.GetDstHostName()))  )
            {
                VNET_LOG(VNET_LOG_ERROR, "CVNetVmMigrateMgr::MigrateBegin: vm_id(%lld) has exist.\n",
                   msg.m_vm_id);
                return 0;                
            }
            else
            {
                VNET_LOG(VNET_LOG_ERROR, "CVNetVmMigrateMgr::MigrateBegin: vm_id(%lld) is conflict.\n",
                  msg.m_vm_id);
                return ERROR_VNM_VM_MIGRATE_IS_CONFLICT;
            }
        }
                
        // return ERROR_VNM_VM_MIGRATE_HAS_EXISTED;        
    }
    
    if( !VNET_DB_IS_ITEM_NO_EXIST(nRet))
    {    
        if( nRet != VNET_DB_SUCCESS )
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNetVmMigrateMgr::MigrateBegin: m_pVNetDbMgr->Query(%lld) failed. ret=%d.\n",
               msg.m_vm_id,nRet);
            return ERROR_VNM_VM_MIGRATE_GET_DB_FAIL;
        }   
    }
    
    // 2 insert/modify vm 
    info.SetVmId(msg.m_vm_id);
    info.SetSrcClusterName(msg.m_src_cc_application.c_str());
    info.SetSrcHostName(msg.m_src_hc_application.c_str());
    info.SetDstClusterName(msg.m_dst_cc_application.c_str());
    info.SetDstHostName(msg.m_dst_hc_application.c_str());
    info.SetState(VNET_VM_MIGRATE_BEGIN);

    if( false == bDbExist  )
    {
        nRet = m_pVNetDbMgr->Add(info);
        if( nRet != VNET_DB_SUCCESS )
        {
            if( m_dbg )
            {
                info.DbgShow();
            }
            VNET_LOG(VNET_LOG_ERROR, "CVNetVmMigrateMgr::MigrateBegin: m_pVNetDbMgr->Add(%lld) failed. ret=%d.\n",
                   msg.m_vm_id,nRet);
            return ERROR_VNM_VM_MIGRATE_SET_DB_FAIL;
        }
    }
    else
    {
        nRet = m_pVNetDbMgr->Modify(info);
        if( nRet != VNET_DB_SUCCESS )
        {
            if( m_dbg )
            {
                info.DbgShow();
            }
            VNET_LOG(VNET_LOG_ERROR, "CVNetVmMigrateMgr::MigrateBegin: m_pVNetDbMgr->Add(%lld) failed. ret=%d.\n",
                   msg.m_vm_id,nRet);
            return ERROR_VNM_VM_MIGRATE_SET_DB_FAIL;
        }
    }
    
    VNET_LOG(VNET_LOG_INFO, "vnet handle MigrateBegin msg(%lld, %s,%s ---> %s,%s) success.\n",\
              msg.m_vm_id, msg.m_src_cc_application.c_str(),msg.m_src_hc_application.c_str(),\
              msg.m_dst_cc_application.c_str(),msg.m_dst_hc_application.c_str());
              
    return 0;
}


/************************************************************
* �������ƣ� MigrateEnd
* ���������� ����vm migrate End
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2012/5         V1.0       lvech         ����
***************************************************************/
int32 CVNetVmMigrateMgr::MigrateEnd(CVNetVmMigrateEndMsg &msg)
{
    // 1 �Ƿ�����Ѵ��ڸ������
    CDbVmMigrate info;
    info.SetVmId(msg.m_vm_id);
    int32 nRet = m_pVNetDbMgr->Query(info);
    if( nRet != VNET_DB_SUCCESS)
    {    
        VNET_LOG(VNET_LOG_ERROR, "CVNetVmMigrateMgr::MigrateEnd: m_pVNetDbMgr->Query(%lld) failed. ret=%d.\n",
               msg.m_vm_id,nRet);
        return ERROR_VNM_VM_MIGRATE_GET_DB_FAIL;        
    }

    VNET_LOG(VNET_LOG_INFO, "vnet recv MigrateEnd msg(%lld, %s,%s ---> %s,%s, result:%d)\n",\
              msg.m_vm_id, msg.m_src_cc_application.c_str(),msg.m_src_hc_application.c_str(),\
              msg.m_dst_cc_application.c_str(),msg.m_dst_hc_application.c_str(),\
              msg.m_result);
    
    // �Ƚ����Ƿ�һ�� 
    if( (0 != msg.m_src_cc_application.compare(info.GetSrcClusterName())) || \
        (0 != msg.m_src_hc_application.compare(info.GetSrcHostName())) || \
        (0 != msg.m_dst_cc_application.compare(info.GetDstClusterName())) || \
        (0 != msg.m_dst_hc_application.compare(info.GetDstHostName()))  )
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetVmMigrateMgr::MigrateEnd: input param is not same as DB data.\n",
              msg.m_vm_id);
        return ERROR_VNM_VM_MIGRATE_INPUTPARAM_DB_NOT_SAME;    
    }

    if(info.GetState() == VNET_VM_MIGRATE_END )
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetVmMigrateMgr::MigrateEnd: vm_id(%lld) has existed, vm migrate success.\n",
              msg.m_vm_id);
        return 0;
    }
    
    // 2update vnic  
    //  update vm migrate
    
    // ����vnic ���¼ 
    if( 0 == msg.m_result )
    {
        nRet = UpdateVnic(msg);
        if( nRet != 0 )
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNetVmMigrateMgr::MigrateEnd: UpdateVnic() failed.\n");
            return nRet;
        }
    }

    info.SetState(VNET_VM_MIGRATE_END);
    nRet = m_pVNetDbMgr->Modify(info);
    if( nRet != VNET_DB_SUCCESS )
    {
        if( m_dbg )
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR, "CVNetVmMigrateMgr::MigrateEnd: m_pVNetDbMgr->Add(%lld) failed. ret=%d.\n",
               msg.m_vm_id,nRet);
        return ERROR_VNM_VM_MIGRATE_SET_DB_FAIL;
    }

    VNET_LOG(VNET_LOG_INFO, "vnet handle MigrateEnd msg(%lld, %s,%s ---> %s,%s, result:%d) success.\n",\
              msg.m_vm_id, msg.m_src_cc_application.c_str(),msg.m_src_hc_application.c_str(),\
              msg.m_dst_cc_application.c_str(),msg.m_dst_hc_application.c_str(),\
              msg.m_result);
    
    return 0;
}

int32 CVNetVmMigrateMgr::UpdateVnic(CVNetVmMigrateEndMsg &msg)
{    
    CDBOperateVsi * pVsi = GetDbIVsi();
    if( pVsi == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetVmMigrateMgr::UpdateVnic: GetDbIVsi() is NULL.\n");
        return ERROR_VNM_VM_MIGRATE_GET_DB_FAIL;
    }

    CDBOperateVm * pVm = GetDbIVm();
    if( pVm == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetVmMigrateMgr::UpdateVnic: GetDbIVm() is NULL.\n");
        return ERROR_VNM_VM_MIGRATE_GET_DB_FAIL;
    }

    CDbVm info;
    info.SetVmId(msg.m_vm_id);
    int32 nRet = pVm->Query(info);
    if( nRet != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetVmMigrateMgr::UpdateVnic:  pVm->Query(%lld) failed. ret=%d.\n",
               msg.m_vm_id,nRet);
        return ERROR_VNM_VM_MIGRATE_SET_DB_FAIL;
    }

    info.SetIsHasCluster(true);
    info.SetIsHasHost(true);
    info.SetClusterName(msg.m_dst_cc_application.c_str());
    info.SetHostName(msg.m_dst_hc_application.c_str());
    
    nRet = pVm->Modify(info);
    if( nRet != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetVmMigrateMgr::UpdateVnic:  pVm->Modify(%lld) failed. ret=%d.\n",
               msg.m_vm_id,nRet);
        return ERROR_VNM_VM_MIGRATE_SET_DB_FAIL;
    }
    
    return 0;
}


void VNM_DBG_VM_MIGRATE_BEGIN(int64 vmid, char* src_cluster,char* src_host, char* dst_cluster, char* dst_host)
{
    if( NULL == src_cluster || NULL == src_host  || NULL == dst_cluster  || NULL == dst_host )
    {
        cout << "param invald." << endl;
        return;
    }
    
    CVNetVmMigrateMgr *pVmMigrate = CVNetVmMigrateMgr::GetInstance();
    if( NULL == pVmMigrate)
    {
        cout << " NULL == pVmMigrate" << endl;
        return ;
    }
    
    CVNetVmMigrateBeginMsg msg;
    msg.m_vm_id = vmid;
    msg.m_src_cc_application = src_cluster;
    msg.m_src_hc_application = src_host;
    msg.m_dst_cc_application = dst_cluster;
    msg.m_dst_hc_application = dst_host;
    
    msg.serialize();
    int32 nRet = pVmMigrate->MigrateBegin(msg);
    if( nRet == 0)
    {
        cout << "MigrateBegin success" << endl;
    }
    else
    {
        cout << "MigrateBegin failed" << endl;
    }
    
    return;
}
DEFINE_DEBUG_FUNC(VNM_DBG_VM_MIGRATE_BEGIN);

void VNM_DBG_VM_MIGRATE_END(int64 vmid, char* src_cluster,char* src_host, char* dst_cluster, char* dst_host, int32 result)
{
    if( NULL == src_cluster || NULL == src_host  || NULL == dst_cluster  || NULL == dst_host )
    {
        cout << "param invald." << endl;
        return;
    }
    
    CVNetVmMigrateMgr *pVmMigrate = CVNetVmMigrateMgr::GetInstance();
    if( NULL == pVmMigrate)
    {
        cout << " NULL == pVmMigrate" << endl;
        return ;
    }
    
    CVNetVmMigrateEndMsg msg;
    msg.m_vm_id = vmid;
    msg.m_src_cc_application = src_cluster;
    msg.m_src_hc_application = src_host;
    msg.m_dst_cc_application = dst_cluster;
    msg.m_dst_hc_application = dst_host;
    msg.m_result = result;
    
    msg.serialize();
    int32 nRet = pVmMigrate->MigrateEnd(msg);
    if( nRet == 0)
    {
        cout << "MigrateEnd success" << endl;
    }
    else
    {
        cout << "MigrateEnd failed" << endl;
    }
    
    return;
}
DEFINE_DEBUG_FUNC(VNM_DBG_VM_MIGRATE_END);


}


