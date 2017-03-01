/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：vnet_vnic_agent.cpp
* 文件标识：
* 内容摘要：CVNetVnicAgent类实现文件
* 当前版本：1.0
* 作    者：
* 完成日期：2013年2月22日
*
* 修改记录1：
*     修改日期：2013/2/22
*     版 本 号：V1.0
*     修 改 人：lverchun
*     修改内容：创建
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
* 函数名称： CVNAVnicPool
* 功能描述： CVNAVnicPool构造函数
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1        V1.0       lvech         创建
***************************************************************/
CVNAVnicPool::CVNAVnicPool()
{
    SetMgrInfo("CVNAVnicPool");
    m_bOpenDbgInf = VNET_FALSE;
    m_pMU = NULL;
    m_tCheckVMVSITimerId = INVALID_TIMER_ID;
};

/************************************************************
* 函数名称： ~CVNAVnicPool
* 功能描述： CVNAVnicPool析构函数
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1        V1.0       lvech         创建
***************************************************************/
CVNAVnicPool::~CVNAVnicPool()
{
    m_pMU = NULL;
    m_tCheckVMVSITimerId = INVALID_TIMER_ID;
};

/************************************************************
* 函数名称： Init
* 功能描述： 初始化函数
* 访问的表： 无
* 修改的表： 无
* 输入参数： 
* 输出参数： 
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1         V1.0       lvech         创建
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

    /*启动定时器扫描VM的网卡，等待VM的网口生成后，对其设置VLAN信息*/
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
* 函数名称： AddVMVSIInfo
* 功能描述： 增加VM相关的VSI详细信息
* 访问的表： 无
* 修改的表： 无
* 输入参数： 
* 输出参数： 
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1         V1.0       lvech         创建
***************************************************************/
CVNAVMVnicInfo*  CVNAVnicPool::AddVMVSIInfo(CVMVSIInfo & cVMVSIInfo)
{
    //部署VXLAN VM前的交换准备工作
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
    //不存在
    if (it == m_mapVMVnicInfo.end())
    {
        CVNAVMVnicInfo tmpVNAVnicInfo;

        tmpVNAVnicInfo.SetVMID(cVMVSIInfo.m_nVMID);
        tmpVNAVnicInfo.SetProjectID(cVMVSIInfo.m_nProjectID);
        tmpVNAVnicInfo.SetQueryVSIFlag(VNA_GET_VSI_DETAIL_FLAG_YES);
        tmpVNAVnicInfo.SetQueryVSIStatus(VNA_GET_VSI_DETAIL_STATUS_OK);
        tmpVNAVnicInfo.SetVnicInfo(cVMVSIInfo.m_vecVnicDetail);   //虚拟机网卡配置
        tmpVNAVnicInfo.SetIsWdgEnable(cVMVSIInfo.m_cWatchDogInfo.m_nIsWDGEnable);
        tmpVNAVnicInfo.SetWdgTimeOut(cVMVSIInfo.m_cWatchDogInfo.m_nTimeOut);
            
        pair<map<int64, CVNAVMVnicInfo>::iterator, bool> iter = m_mapVMVnicInfo.insert(pair<int64, CVNAVMVnicInfo> (cVMVSIInfo.m_nVMID, tmpVNAVnicInfo));
        return &(iter.first->second);
    }
    //存在
    else
    {
        it->second.SetProjectID(cVMVSIInfo.m_nProjectID);
        it->second.SetQueryVSIFlag(VNA_GET_VSI_DETAIL_FLAG_YES);
        it->second.SetQueryVSIStatus(VNA_GET_VSI_DETAIL_STATUS_OK);
        it->second.UpdateVnicDetail(cVMVSIInfo.m_vecVnicDetail);        
        it->second.SetIsWdgEnable(cVMVSIInfo.m_cWatchDogInfo.m_nIsWDGEnable);
        it->second.SetWdgTimeOut(cVMVSIInfo.m_cWatchDogInfo.m_nTimeOut);

        //清除部署的任务信息
        //it->second.SetDeployOption(0);
        return &(it->second);
    }

};

/************************************************************
* 函数名称： DelVMVSIInfo
* 功能描述： 删除VM相关的VSI信息
* 访问的表： 无
* 修改的表： 无
* 输入参数： 
* 输出参数： 
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1         V1.0       lvech         创建
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
* 函数名称： GetVMVSIInfo
* 功能描述： 获取VM相关的VSI信息
* 访问的表： 无
* 修改的表： 无
* 输入参数： 
* 输出参数： 
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1         V1.0       lvech         创建
***************************************************************/
CVNAVMVnicInfo* CVNAVnicPool::GetVMVSIInfo(const int64 nVMID)
{
    map<int64, CVNAVMVnicInfo>::iterator it = m_mapVMVnicInfo.find(nVMID);
    //不存在
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
* 函数名称： AddVMVnicInfo
* 功能描述： 增加VM相关的vnic信息,此时无VSI详细信息
* 访问的表： 无
* 修改的表： 无
* 输入参数： 
* 输出参数： 
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1         V1.0       lvech         创建
***************************************************************/
CVNAVMVnicInfo* CVNAVnicPool::AddVMVnicInfo(CVNetStartNetworkMsg &cMsg)
{
    map<int64, CVNAVMVnicInfo>::iterator it = m_mapVMVnicInfo.find(cMsg.m_vm_id);
    //不存在
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
    //存在
    else
    {
        //正在启动,或者已部署成功，则不做任何处理
        if (VNA_VM_DEPLOY_STATUS_NETWORK_STARTING == it->second.GetDeployStatus())
        {
            
        }
        else if (VNA_VM_DEPLOY_STATUS_NETWORK_RUNNING == it->second.GetDeployStatus())
        {
            // vm 如果从prepare -> migrate EVB需重新协商
            if( cMsg.m_option != it->second.GetDeployOption())
            /* if( (cMsg.m_option == VNET_VM_MIGRATE) && \
                it->second.GetDeployOption() == VNET_VM_PREPARE ) */
            {
                 it->second.SetDeployStatus(VNA_VM_DEPLOY_STATUS_NETWORK_STARTING);
                 it->second.SetQueryVSIEvbLastTime(0);
                 it->second.SetQueryVSIEvbTimes(0);
                 it->second.SetQueryVSIEvbStatus(VNA_VM_VSI_EVB_STATUS_INIT);
                 
                 it->second.SetActionID(cMsg.action_id);                                     //部署选项
                 it->second.SetDeployMID(cMsg.m_mid);
                 it->second.SetDeployOption(cMsg.m_option); 
            }
            
            it->second.SetDeployOption(cMsg.m_option);                                  //部署选项
        }
        //相同VM新的部署流程
        else
        {
            it->second.SetDeployStatus(VNA_VM_DEPLOY_STATUS_NETWORK_STARTING);
            it->second.SetActionID(cMsg.action_id);                               //待讨论，计算或者NETLIB库都可以填
            it->second.SetDeployOption(cMsg.m_option);                                  //部署选项
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
* 函数名称： AddVMVnicInfo
* 功能描述： 增加VM相关的vnic信息,此时无VSI详细信息
* 访问的表： 无
* 修改的表： 无
* 输入参数： 
* 输出参数： 
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1         V1.0       lvech         创建
***************************************************************/
void CVNAVnicPool::AddVMVnicInfo(CVNetVMVSIMsg &cMsg)
{
    map<int64, CVNAVMVnicInfo>::iterator it = m_mapVMVnicInfo.find(cMsg.m_vm_id);
    //不存在
    if (it == m_mapVMVnicInfo.end())
    {
        CVNAVMVnicInfo cVMVnicInfo;

        cVMVnicInfo.SetVMID(cMsg.m_vm_id); 
        cVMVnicInfo.SetDomainID(cMsg.m_domain_id);
        cVMVnicInfo.SetDeployStatus(VNA_VM_DEPLOY_STATUS_NETWORK_STARTING);      //待讨论，RUNNING也可以，需要lib确保只把部署成功的VM信息发过来
        cVMVnicInfo.SetQueryVSIFlag(VNA_GET_VSI_DETAIL_FLAG_NO);                 //标识是否已从VNM上获取详细信息
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
* 函数名称： CheckVMVSIInfo
* 功能描述： 增加VM相关的VSI信息
* 访问的表： 无
* 修改的表： 无
* 输入参数： 
* 输出参数： 
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1         V1.0       lvech         创建
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

        //检查是否已失败,可能是次数超时，不是明确失败
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
            //检查是否需要向VNM请求VSI详细信息
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

            //检查是否需要向WDG发送消息
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
           
            // 检查是否需要重新获取EVB TLV信息   
            pVmMgr->CheckEvbStatus(&(iter->second));            
        }
        
        if ((0 != iRet) && (nHcIsOK))
        {
            //向HC回应答消息

            //重置VM网络部署状态
            if (VNA_VM_DEPLOY_STATUS_NETWORK_STARTING == iter->second.GetDeployStatus())
            {
                // 设置为失败
                iter->second.SetDeployStatus(VNA_VM_DEPLOY_STATUS_NETWORK_START_FAIL);
                        
                //向HC发送startnetwork应答消息
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
                // 设置为失败
                iter->second.SetDeployStatus(VNA_VM_DEPLOY_STATUS_NETWORK_STOP_FAIL);
                
                //向HC发送stoptnetwork应答消息
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
* 函数名称： SetScheduleFlag
* 功能描述： 更新VM相关的DomID信息
* 访问的表： 无
* 修改的表： 无
* 输入参数： 
* 输出参数： 
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1         V1.0       lvech         创建
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
* 函数名称： UpdateDomID
* 功能描述： 更新VM相关的DomID信息
* 访问的表： 无
* 修改的表： 无
* 输入参数： 
* 输出参数： 
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1         V1.0       lvech         创建
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
* 函数名称： DbgShowData
* 功能描述： 调试信息打印
* 访问的表： 无
* 修改的表： 无
* 输入参数： 
* 输出参数： 
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1         V1.0       lvech         创建
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
* 函数名称： SetDbgFlag
* 功能描述： 调试信息开关
* 访问的表： 无
* 修改的表： 无
* 输入参数： 
* 输出参数： 
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2013/1         V1.0       lvech         创建
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
