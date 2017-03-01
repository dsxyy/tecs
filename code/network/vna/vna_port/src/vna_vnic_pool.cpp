/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�vnet_vnic_agent.cpp
* �ļ���ʶ��
* ����ժҪ��CVNetVnicAgent��ʵ���ļ�
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
#include "registered_system.h"
#include "vna_common.h"
#include "dev_monitor.h"
#include "dev_mgr.h"
#include "vna_agent.h"
#include "vnet_vnic.h"
#include "vna_vnic_pool.h"

#include "vna_switch_module.h"
#include "vna_switch_agent.h"
#include "vna_agent.h"
#include "vna_vm_agent.h"
#include "vna_vxlan_agent.h"


namespace zte_tecs
{

/************************************************************
* �������ƣ� CVNAVnicPool
* ���������� CVNAVnicPool���캯��
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1        V1.0       lvech         ����
***************************************************************/
CVNAVnicPool::CVNAVnicPool()
{
    SetMgrInfo("CVNAVnicPool");
    m_bOpenDbgInf = VNET_FALSE;
    m_pMU = NULL;
    m_tCheckVMVSITimerId = INVALID_TIMER_ID;
};

/************************************************************
* �������ƣ� ~CVNAVnicPool
* ���������� CVNAVnicPool��������
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1        V1.0       lvech         ����
***************************************************************/
CVNAVnicPool::~CVNAVnicPool()
{
    m_pMU = NULL;
    m_tCheckVMVSITimerId = INVALID_TIMER_ID;
};

/************************************************************
* �������ƣ� Init
* ���������� ��ʼ������
* ���ʵı� ��
* �޸ĵı� ��
* ��������� 
* ��������� 
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1         V1.0       lvech         ����
***************************************************************/
int32 CVNAVnicPool::Init( MessageUnit  *mu)
{
    if (NULL == mu)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNAVnicPool::Init: mu is NULL.\n");
        return -1;
    }

    m_pMU = mu;
    
    m_mapVMVnicInfo.clear();

    /*������ʱ��ɨ��VM���������ȴ�VM���������ɺ󣬶�������VLAN��Ϣ*/
    m_tCheckVMVSITimerId = mu->CreateTimer();
    if (INVALID_TIMER_ID == m_tCheckVMVSITimerId)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNAVnicPool::Init: Create del vnet timer failed.\n");
        return -1;
    }

    TimeOut  tTmOut;
    tTmOut.duration = TIMER_INTERVAL_VNA_CHECK_VM_VSI_INFO;
    tTmOut.msgid = EV_VNA_CHECK_VM_VSI_INFO;
    tTmOut.type = LOOP_TIMER;
    if (SUCCESS != mu->SetTimer(m_tCheckVMVSITimerId, tTmOut))
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNAVnicPool::Init: Set timer(id:%d) failed.\n", m_tCheckVMVSITimerId);
        return -1;
    }

    return 0;
};

/************************************************************
* �������ƣ� AddVMVSIInfo
* ���������� ����VM��ص�VSI��ϸ��Ϣ
* ���ʵı� ��
* �޸ĵı� ��
* ��������� 
* ��������� 
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1         V1.0       lvech         ����
***************************************************************/
CVNAVMVnicInfo*  CVNAVnicPool::AddVMVSIInfo(CVMVSIInfo & cVMVSIInfo)
{
    //����VXLAN VMǰ�Ľ���׼������
    CVxlanAgent *pVxlanAgent = CVxlanAgent::GetInstance();
    if (!pVxlanAgent)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNAVnicPool::AddVMVSIInfo: CVxlanAgent::GetInstance() failed.\n");
    }
    else
    {    
        pVxlanAgent->PrepareDeployVxlanVm(cVMVSIInfo);    
    }

    map<int64, CVNAVMVnicInfo>::iterator it;
    it = m_mapVMVnicInfo.find(cVMVSIInfo.m_nVMID);
    //������
    if (it == m_mapVMVnicInfo.end())
    {
        CVNAVMVnicInfo tmpVNAVnicInfo;

        tmpVNAVnicInfo.SetVMID(cVMVSIInfo.m_nVMID);
        tmpVNAVnicInfo.SetProjectID(cVMVSIInfo.m_nProjectID);
        tmpVNAVnicInfo.SetQueryVSIFlag(VNA_GET_VSI_DETAIL_FLAG_YES);
        tmpVNAVnicInfo.SetQueryVSIStatus(VNA_GET_VSI_DETAIL_STATUS_OK);
        tmpVNAVnicInfo.SetVnicInfo(cVMVSIInfo.m_vecVnicDetail);   //�������������
        tmpVNAVnicInfo.SetIsWdgEnable(cVMVSIInfo.m_cWatchDogInfo.m_nIsWDGEnable);
        tmpVNAVnicInfo.SetWdgTimeOut(cVMVSIInfo.m_cWatchDogInfo.m_nTimeOut);
            
        pair<map<int64, CVNAVMVnicInfo>::iterator, bool> iter = m_mapVMVnicInfo.insert(pair<int64, CVNAVMVnicInfo> (cVMVSIInfo.m_nVMID, tmpVNAVnicInfo));
        return &(iter.first->second);
    }
    //����
    else
    {
        it->second.SetProjectID(cVMVSIInfo.m_nProjectID);
        it->second.SetQueryVSIFlag(VNA_GET_VSI_DETAIL_FLAG_YES);
        it->second.SetQueryVSIStatus(VNA_GET_VSI_DETAIL_STATUS_OK);
        it->second.UpdateVnicDetail(cVMVSIInfo.m_vecVnicDetail);        
        it->second.SetIsWdgEnable(cVMVSIInfo.m_cWatchDogInfo.m_nIsWDGEnable);
        it->second.SetWdgTimeOut(cVMVSIInfo.m_cWatchDogInfo.m_nTimeOut);

        //��������������Ϣ
        //it->second.SetDeployOption(0);
        return &(it->second);
    }

};

/************************************************************
* �������ƣ� DelVMVSIInfo
* ���������� ɾ��VM��ص�VSI��Ϣ
* ���ʵı� ��
* �޸ĵı� ��
* ��������� 
* ��������� 
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1         V1.0       lvech         ����
***************************************************************/
void CVNAVnicPool::DelVMVSIInfo(const int64 nVMID)
{
    map<int64, CVNAVMVnicInfo >::iterator iter;
    iter = m_mapVMVnicInfo.find(nVMID);
    if ( iter != m_mapVMVnicInfo.end() )
    {
        m_mapVMVnicInfo.erase(iter);
    }
};

/************************************************************
* �������ƣ� GetVMVSIInfo
* ���������� ��ȡVM��ص�VSI��Ϣ
* ���ʵı� ��
* �޸ĵı� ��
* ��������� 
* ��������� 
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1         V1.0       lvech         ����
***************************************************************/
CVNAVMVnicInfo* CVNAVnicPool::GetVMVSIInfo(const int64 nVMID)
{
    map<int64, CVNAVMVnicInfo>::iterator it = m_mapVMVnicInfo.find(nVMID);
    //������
    if (it == m_mapVMVnicInfo.end())
    {
        return NULL;
    }
    else
    {
        return &(it->second);
    }
};

/************************************************************
* �������ƣ� AddVMVnicInfo
* ���������� ����VM��ص�vnic��Ϣ,��ʱ��VSI��ϸ��Ϣ
* ���ʵı� ��
* �޸ĵı� ��
* ��������� 
* ��������� 
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1         V1.0       lvech         ����
***************************************************************/
CVNAVMVnicInfo* CVNAVnicPool::AddVMVnicInfo(CVNetStartNetworkMsg &cMsg)
{
    map<int64, CVNAVMVnicInfo>::iterator it = m_mapVMVnicInfo.find(cMsg.m_vm_id);
    //������
    if (it == m_mapVMVnicInfo.end())
    {
        CVNAVMVnicInfo cVMVnicInfo;
        cVMVnicInfo.SetVMID(cMsg.m_vm_id); 
        cVMVnicInfo.SetDomainID(-1);
        cVMVnicInfo.SetDeployStatus(VNA_VM_DEPLOY_STATUS_NETWORK_STARTING);
        cVMVnicInfo.SetActionID(cMsg.action_id);
        cVMVnicInfo.SetDeployOption(cMsg.m_option);
        cVMVnicInfo.SetDeployMID(cMsg.m_mid);
        cVMVnicInfo.SetQueryVSIFlag(VNA_GET_VSI_DETAIL_FLAG_NO);
        cVMVnicInfo.SetIsWdgEnable(cMsg.m_WatchDogInfo.m_nIsWDGEnable);
        cVMVnicInfo.SetWdgTimeOut(cMsg.m_WatchDogInfo.m_nTimeOut);
        cVMVnicInfo.SetWdgOperStatus(VNA_VM_WDG_OP_STATUS_INIT);
        cVMVnicInfo.SetVnicInfo(cMsg.m_VmNicCfgList);

        pair<map<int64, CVNAVMVnicInfo>::iterator, bool> iter = m_mapVMVnicInfo.insert(pair<int64, CVNAVMVnicInfo> (cVMVnicInfo.GetVMID(), cVMVnicInfo));     
        return &(iter.first->second);
    }
    //����
    else
    {
        //��������,�����Ѳ���ɹ��������κδ���
        if (VNA_VM_DEPLOY_STATUS_NETWORK_STARTING == it->second.GetDeployStatus())
        {
            
        }
        else if (VNA_VM_DEPLOY_STATUS_NETWORK_RUNNING == it->second.GetDeployStatus())
        {
            // vm �����prepare -> migrate EVB������Э��
            if( cMsg.m_option != it->second.GetDeployOption())
            /* if( (cMsg.m_option == VNET_VM_MIGRATE) && \
                it->second.GetDeployOption() == VNET_VM_PREPARE ) */
            {
                 it->second.SetDeployStatus(VNA_VM_DEPLOY_STATUS_NETWORK_STARTING);
                 it->second.SetQueryVSIEvbLastTime(0);
                 it->second.SetQueryVSIEvbTimes(0);
                 it->second.SetQueryVSIEvbStatus(VNA_VM_VSI_EVB_STATUS_INIT);
                 
                 it->second.SetActionID(cMsg.action_id);                                     //����ѡ��
                 it->second.SetDeployMID(cMsg.m_mid);
                 it->second.SetDeployOption(cMsg.m_option); 
            }
            
            it->second.SetDeployOption(cMsg.m_option);                                  //����ѡ��
        }
        //��ͬVM�µĲ�������
        else
        {
            it->second.SetDeployStatus(VNA_VM_DEPLOY_STATUS_NETWORK_STARTING);
            it->second.SetActionID(cMsg.action_id);                               //�����ۣ��������NETLIB�ⶼ������
            it->second.SetDeployOption(cMsg.m_option);                                  //����ѡ��
            it->second.SetDeployMID(cMsg.m_mid);

            if (VNA_GET_VSI_DETAIL_STATUS_FAIL == it->second.GetQueryVSIStatus())
            {
                it->second.SetQueryVSIStatus(VNA_GET_VSI_DETAIL_STATUS_INIT);
                it->second.SetQueryVSITimes(0);
            }
            
            it->second.SetWdgOperStatus(VNA_VM_WDG_OP_STATUS_INIT);

            it->second.SetIsWdgEnable(cMsg.m_WatchDogInfo.m_nIsWDGEnable);
            it->second.SetWdgTimeOut(cMsg.m_WatchDogInfo.m_nTimeOut);
            it->second.SetWdgOperStatus(VNA_VM_WDG_OP_STATUS_INIT);
            it->second.SetWdgOperTimes(0);

            it->second.SetQueryVSIEvbTimes(0);
            it->second.SetQueryVSIEvbStatus(VNA_VM_VSI_EVB_STATUS_INIT);
            it->second.SetQueryVSIEvbLastTime(0);
        }

        return &(it->second);
    }
};

/************************************************************
* �������ƣ� AddVMVnicInfo
* ���������� ����VM��ص�vnic��Ϣ,��ʱ��VSI��ϸ��Ϣ
* ���ʵı� ��
* �޸ĵı� ��
* ��������� 
* ��������� 
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1         V1.0       lvech         ����
***************************************************************/
void CVNAVnicPool::AddVMVnicInfo(CVNetVMVSIMsg &cMsg)
{
    map<int64, CVNAVMVnicInfo>::iterator it = m_mapVMVnicInfo.find(cMsg.m_vm_id);
    //������
    if (it == m_mapVMVnicInfo.end())
    {
        CVNAVMVnicInfo cVMVnicInfo;

        cVMVnicInfo.SetVMID(cMsg.m_vm_id); 
        cVMVnicInfo.SetDomainID(cMsg.m_domain_id);
        cVMVnicInfo.SetDeployStatus(VNA_VM_DEPLOY_STATUS_NETWORK_STARTING);      //�����ۣ�RUNNINGҲ���ԣ���Ҫlibȷ��ֻ�Ѳ���ɹ���VM��Ϣ������
        cVMVnicInfo.SetQueryVSIFlag(VNA_GET_VSI_DETAIL_FLAG_NO);                 //��ʶ�Ƿ��Ѵ�VNM�ϻ�ȡ��ϸ��Ϣ
        cVMVnicInfo.SetIsWdgEnable(cMsg.m_WatchDogInfo.m_nIsWDGEnable);
        cVMVnicInfo.SetWdgTimeOut(cMsg.m_WatchDogInfo.m_nTimeOut);
        cVMVnicInfo.SetWdgOperStatus(VNA_VM_WDG_OP_STATUS_INIT);
        cVMVnicInfo.SetWdgOperTimes(0);
        cVMVnicInfo.SetVnicInfo(cMsg.m_vnics);
        
        m_mapVMVnicInfo.insert(pair<int64, CVNAVMVnicInfo> (cVMVnicInfo.GetVMID(), cVMVnicInfo));
    }
    else
    {
        it->second.SetDomainID(cMsg.m_domain_id);
    }
};

/************************************************************
* �������ƣ� CheckVMVSIInfo
* ���������� ����VM��ص�VSI��Ϣ
* ���ʵı� ��
* �޸ĵı� ��
* ��������� 
* ��������� 
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1         V1.0       lvech         ����
***************************************************************/
void CVNAVnicPool::CheckVMVSIInfo()
{
    int32 iRet = 0;
    string strVnmName;

    if (ERROR == GetRegisteredSystem(strVnmName))
    {
        cout << "GetRegisteredSystem() failed"<<endl;
        return;
    }
    
    CVNetAgent *m_pVNetAgent = CVNetAgent::GetInstance();
    if (NULL == m_pVNetAgent)
    {
        cout << "CVNetAgent::GetInstance() failed"<<endl;
        return;
    }

    CVNetVmAgent * pVmMgr = CVNetVmAgent::GetInstance();
    if (NULL == pVmMgr)
    {
        cout << "CVNetVmAgent::GetInstance() failed"<<endl;
        return;
    }
    
    TModuleInfo cWdgModule;
    int nWdgIsOK = 1;
    iRet = m_pVNetAgent->GetModuleInfo(VNA_MODULE_ROLE_WATCHDOG, cWdgModule);
    if (0 != iRet)
    {
        nWdgIsOK = 0;
    }


    TModuleInfo cHcModule;
    int nHcIsOK = 1;
    iRet = m_pVNetAgent->GetModuleInfo(VNA_MODULE_ROLE_HC, cHcModule);
    if (0 != iRet) 
    {
        nHcIsOK = 0;
    }

    iRet = 0;
    time_t cur_time = time(0);

    map<int64, CVNAVMVnicInfo >::iterator iter = m_mapVMVnicInfo.begin();
    for ( ; iter != m_mapVMVnicInfo.end(); ++iter)
    {
        //
        if ( (VNA_VM_DEPLOY_STATUS_NETWORK_STARTING != iter->second.GetDeployStatus())
            && (VNA_VM_DEPLOY_STATUS_NETWORK_STOPING != iter->second.GetDeployStatus()))
        {
            continue;
        }

        //����Ƿ���ʧ��,�����Ǵ�����ʱ��������ȷʧ��
        if (VNA_VM_GET_VSI_STATUS_FAIL(&(iter->second)))
        {
            iter->second.SetQueryVSIStatus(VNA_GET_VSI_DETAIL_STATUS_FAIL);
            iRet = ERROR_VNA_VNIC_GET_VSI_FROM_VNM_FAIL;
        }
        else if (VNA_VM_WDG_STATUS_FAIL(&(iter->second)))
        {
            iter->second.SetWdgOperStatus(VNA_VM_WDG_OP_STATUS_FAIL);
            iRet = ERROR_VNA_VNIC_WDG_OPER_FAIL;            
        }
        else if (VNA_VM_VSI_EVB_STATUS_FAIL(&(iter->second)))
        {
            iter->second.SetQueryVSIEvbStatus(VNA_VM_VSI_EVB_STATUS_FAILED);
            iRet = ERROR_VNA_VNIC_VSI_EVB_STATUS_FAIL;            
        }

        if (0 != iRet)
        {
            if (VNA_VM_DEPLOY_STATUS_NETWORK_STARTING == iter->second.GetDeployStatus())
            {
                iter->second.SetDeployStatus(VNA_VM_DEPLOY_STATUS_NETWORK_START_FAIL);
            }
            else if (VNA_VM_DEPLOY_STATUS_NETWORK_STOPING == iter->second.GetDeployStatus())
            {
                iter->second.SetDeployStatus(VNA_VM_DEPLOY_STATUS_NETWORK_STOP_FAIL);
            }
        }
        else
        {
            //����Ƿ���Ҫ��VNM����VSI��ϸ��Ϣ
            if (VNA_VM_GET_VSI_STATUS_NOT_OK(&(iter->second)))
            {
                iter->second.SetQueryVSILastTime(cur_time);
                iter->second.SetQueryVSITimes(iter->second.GetQueryVSITimes() + 1);
           
                CVMVSIInfoReqMsg cVSIReqMsg;
                cVSIReqMsg.m_nVMID = iter->second.GetVMID();
                iter->second.GetVnicCfgInfo(cVSIReqMsg.m_vecVnicCfg);
                
                MID receiver;
                receiver._application = strVnmName;
                receiver._process = PROC_VNM;
                receiver._unit = MU_VNM;
                MessageOption option(receiver, EV_VNA_REQUEST_VMVSI, 0, 0);
                m_pMU->Send(cVSIReqMsg, option);
            }

            //����Ƿ���Ҫ��WDG������Ϣ
            if (VNA_VM_WDG_STATUS_NOT_OK(&(iter->second)))
            {
                iter->second.SetWdgOperTimes(iter->second.GetWdgOperTimes() + 1);
           
                if (nWdgIsOK)
                {
                    CVNetWdgMsg cWdgMsg;
                    cWdgMsg.action_id = iter->second.GetActionID();
                    cWdgMsg.m_qwVmId = iter->first;
                    cWdgMsg.m_nTimerOut = iter->second.GetWdgTimeOut();
                    if (VNA_VM_DEPLOY_STATUS_NETWORK_STARTING == iter->second.GetDeployStatus())
                    {
                        cWdgMsg.m_nOperator = EN_START_WDG;
                    }
                    else
                    {
                        cWdgMsg.m_nOperator = EN_STOP_WDG;
                    }
                    
                    MessageOption option(cWdgModule.GetModuleLibMid(), EV_VNETLIB_WDG_OPER, 0, 0);
                    m_pMU->Send(cWdgMsg, option);
                }
            }
           
            // ����Ƿ���Ҫ���»�ȡEVB TLV��Ϣ   
            pVmMgr->CheckEvbStatus(&(iter->second));            
        }
        
        if ((0 != iRet) && (nHcIsOK))
        {
            //��HC��Ӧ����Ϣ

            //����VM���粿��״̬
            if (VNA_VM_DEPLOY_STATUS_NETWORK_STARTING == iter->second.GetDeployStatus())
            {
                // ����Ϊʧ��
                iter->second.SetDeployStatus(VNA_VM_DEPLOY_STATUS_NETWORK_START_FAIL);
                        
                //��HC����startnetworkӦ����Ϣ
                CVNetStartNetworkMsgAck cAck;
                cAck.m_mid = iter->second.GetDeployMID();
                cAck.m_vmid =  iter->first;
                cAck.action_id = iter->second.GetActionID();
                cAck.state = iRet;

                MessageOption option(cHcModule.GetModuleLibMid(), EV_VNETLIB_STARTNETWORK_ACK,0,0);
                m_pMU->Send(cAck,option);
            }
            else if (VNA_VM_DEPLOY_STATUS_NETWORK_STOPING == iter->second.GetDeployStatus())
            {
                // ����Ϊʧ��
                iter->second.SetDeployStatus(VNA_VM_DEPLOY_STATUS_NETWORK_STOP_FAIL);
                
                //��HC����stoptnetworkӦ����Ϣ
                CVNetStopNetworkMsgAck cAck;
                cAck.m_mid = iter->second.GetDeployMID();
                cAck.m_vmid =  iter->first;
                cAck.action_id = iter->second.GetActionID();
                cAck.state = iRet;

                MessageOption option(cHcModule.GetModuleLibMid(), EV_VNETLIB_STOPNETWORK_ACK,0,0);
                m_pMU->Send(cAck,option);
            }
            else
            {
                return;
            }
        }

        iRet = 0;
        
    }
};

/************************************************************
* �������ƣ� SetScheduleFlag
* ���������� ����VM��ص�DomID��Ϣ
* ���ʵı� ��
* �޸ĵı� ��
* ��������� 
* ��������� 
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1         V1.0       lvech         ����
***************************************************************/
int32 CVNAVnicPool::SetWdgStatusInfo(CVNetWdgAckMsg &cMsg)
{
    map<int64, CVNAVMVnicInfo >::iterator iter = m_mapVMVnicInfo.begin();
    for ( ; iter != m_mapVMVnicInfo.end(); ++iter)
    {
        if (cMsg.m_qwVmId == iter->first)
        {
            if (iter->second.GetActionID() == cMsg.action_id)
            {
                if (cMsg.state == 0)
                {
                    if (!VNA_VM_WDG_STATUS_FAIL(&(iter->second)))
                    {
                        iter->second.SetWdgOperStatus(VNA_VM_WDG_OP_STATUS_OK);
                        iter->second.SetWdgVnicInfo(cMsg.m_cVnic);
                    }
                }
                else
                {
                    iter->second.SetWdgOperStatus(VNA_VM_WDG_OP_STATUS_FAIL);
                }
                iter->second.SetWdgOperTimes(0);

                return 0;
            }
            else
            {
                cout <<"SetWdgStatusInfo failed, strActionID invalid" <<endl;
                return -1;
            }
            break;
        }
    }

    return 0;
}

/************************************************************
* �������ƣ� UpdateDomID
* ���������� ����VM��ص�DomID��Ϣ
* ���ʵı� ��
* �޸ĵı� ��
* ��������� 
* ��������� 
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1         V1.0       lvech         ����
***************************************************************/
void CVNAVnicPool::UpdateDomID(int64 nVmID, int nDomID)
{
    map<int64, CVNAVMVnicInfo >::iterator iter = m_mapVMVnicInfo.begin();
    for ( ; iter != m_mapVMVnicInfo.end(); ++iter)
    {
        if (nVmID == iter->first)
        {
            iter->second.SetDomainID(nDomID);
            iter->second.SetVnicDevName();
            break;
        }
    }
}

void CVNAVnicPool::DoMonitor(CNetDevVisitor *dev_visit)
{
    map<int64, CVNAVMVnicInfo >::iterator iter = m_mapVMVnicInfo.begin();
    for ( ; iter != m_mapVMVnicInfo.end(); ++iter)
    {
        iter->second.Accept(dev_visit);
    }
};

void CVNAVnicPool::DoMonitor(CPortVisitor *dev_visit)
{
    map<int64, CVNAVMVnicInfo >::iterator iter = m_mapVMVnicInfo.begin();
    for ( ; iter != m_mapVMVnicInfo.end(); ++iter)
    {
        iter->second.Accept(dev_visit);
    }
};

void CVNAVnicPool::DoMonitor(CVnicVisitor *dev_visit)
{
    map<int64, CVNAVMVnicInfo >::iterator iter = m_mapVMVnicInfo.begin();
    for ( ; iter != m_mapVMVnicInfo.end(); ++iter)
    {
        iter->second.Accept(dev_visit);
    }
};


void CVNAVnicPool::GetMacFromPortName(string port_name, TPortInfo &port_info)
{
    map<int64, CVNAVMVnicInfo >::iterator iter = m_mapVMVnicInfo.begin();
    for ( ; iter != m_mapVMVnicInfo.end(); ++iter)
    {
        vector<CVnicPortDev> vecVnicInfo;
        iter->second.GetVnicInfo(vecVnicInfo);
        
        vector<CVnicPortDev>::iterator iterVnicPort = vecVnicInfo.begin();
        for ( ; iterVnicPort != vecVnicInfo.end(); ++iterVnicPort)
        {
            if((iterVnicPort->GetVifName() == port_name)
                || (iterVnicPort->GetTapName() == port_name))
            {
                if (0 != iterVnicPort->GetAssMac().size())
                {
                    port_info.m_strMacAddr = iterVnicPort->GetAssMac();
                }
                else
                {
                    port_info.m_strMacAddr = iterVnicPort->GetMacAddress();
                }
                return;
            }
        }
    }
   
};

/************************************************************
* �������ƣ� DbgShowData
* ���������� ������Ϣ��ӡ
* ���ʵı� ��
* �޸ĵı� ��
* ��������� 
* ��������� 
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1         V1.0       lvech         ����
***************************************************************/
void CVNAVnicPool::DbgShowData(int64 vm_id)
{    cout << endl;    
    cout << "++++++++++++++++++++++VNA Vnic Information BEGIN ++++++++++++++++++++++++" << endl;

    map<int64, CVNAVMVnicInfo >::iterator iter = m_mapVMVnicInfo.begin();
    for ( ; iter != m_mapVMVnicInfo.end(); ++iter)
    {
        if ((vm_id != 0) && (iter->first != vm_id))
        {
            continue;
        }
        
        iter->second.Print();
        cout <<"<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << endl;
    }

    cout << "++++++++++++++++++++++VNA Vnic Information END ++++++++++++++++++++++++" << endl;
};

/************************************************************
* �������ƣ� SetDbgFlag
* ���������� ������Ϣ����
* ���ʵı� ��
* �޸ĵı� ��
* ��������� 
* ��������� 
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2013/1         V1.0       lvech         ����
***************************************************************/
void CVNAVnicPool::SetDbgFlag(int bDbgFlag)
{
    m_bOpenDbgInf = bDbgFlag;
};

int VNET_DBG_SHOW_VNA_VNIC_INFO(int64 vm_id)
{
    if (vm_id < 0)
    {
        cout << "Invalid VM ID!"<< endl;
        return 0;
    }
    
    CVNAVnicPool *pVNetHandle = CVNAVnicPool::GetInstance();
    if (pVNetHandle)
    {
        pVNetHandle->DbgShowData(vm_id);
        cout << endl;
    }
    return 0;
}
DEFINE_DEBUG_FUNC(VNET_DBG_SHOW_VNA_VNIC_INFO);

int VNET_DBG_CLEAR_ALL_VNA_VNIC(void)
{
    CVNAVnicPool *pVNetHandle = CVNAVnicPool::GetInstance();
    if (pVNetHandle)
    {
        pVNetHandle->ClearAll();
    }
    return 0;
}
DEFINE_DEBUG_FUNC(VNET_DBG_CLEAR_ALL_VNA_VNIC);

}
