/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：vnet_manager.cpp
* 文件标识：
* 内容摘要：CVNetManager类实现文件
* 当前版本：1.0
* 作    者：
* 完成日期：2011年8月16日
*
* 修改记录1：
*     修改日期：2011/8/16
*     版 本 号：V1.0
*     修 改 人：Wang.Lule
*     修改内容：创建
******************************************************************************/

#include "vnet_comm.h"
#include "vm_messages.h"
#include "db_config.h"

#include "vnm_pub.h"
#include "msg_proc_reg.h"
#include "vnm_vxlan_mgr.h"

#include "vnet_manager.h"

namespace zte_tecs
{
    CVNetManager* CVNetManager::s_pInstance = NULL;

    CVNetManager::CVNetManager()
    {
        m_nStateMachines = VNET_STATE_MACHINE_INIT;
        m_bOpenDbgInf = VNET_FALSE; 
        m_pMU = NULL;
        m_pVNMCtrlInst = NULL;
        m_pVNAMgrInst = NULL;
        m_pVlanMgrInst = NULL;
        m_pSegmentMgrInst = NULL;
        m_pMACMgrInst = NULL;
        m_pIPv4MgrInst = NULL;
        m_pNetplaneMgrInst = NULL;
        m_pVNetVnicDB = NULL;
        m_pVNetVmDB = NULL;
        m_pVNetVMMgrInst = NULL;
        m_pVNetVMMgrExtInst = NULL;
        m_pVNetVsiMgrInst = NULL;
        m_pSwitchMgrInst   = NULL;
        m_pPGMgrInst  = NULL;
        m_pPortMgrInst = NULL;
        m_pWildcastMgrInst = NULL;
        m_pVmMigrateMgrInst = NULL;
        m_pVxlanMgr = NULL;
        
        m_pVNetVNMQueryInst = NULL;
        m_pVNetVNAReportInst = NULL;
    };

    CVNetManager::~CVNetManager()
    {
        delete m_pMU;
        m_pMU = NULL;
        m_pVNAMgrInst = NULL;
        m_pVlanMgrInst = NULL;
        m_pSegmentMgrInst = NULL;
        m_nStateMachines = VNET_STATE_MACHINE_INIT;
        m_bOpenDbgInf = VNET_FALSE;
        m_pMACMgrInst = NULL;
        m_pIPv4MgrInst = NULL;
        m_pVNMCtrlInst = NULL;
        m_pVNetVnicDB = NULL;
        m_pVNetVmDB = NULL;
        m_pVNetVMMgrInst = NULL;
        m_pVNetVMMgrExtInst = NULL;
        m_pVNetVsiMgrInst = NULL;
        m_pSwitchMgrInst   = NULL;
        m_pPGMgrInst  = NULL;
        m_pPortMgrInst = NULL;
        m_pWildcastMgrInst = NULL;
        m_pVmMigrateMgrInst = NULL;   
        m_pVxlanMgr = NULL;
        m_pVNetVNMQueryInst = NULL;
        m_pVNetVNAReportInst = NULL;
    };

    STATUS CVNetManager::StartMu(const string& name)
    {
        // 消息单元的创建和初始化
        m_pMU = new MessageUnit(name);
        if (!m_pMU)
        {
            VNET_LOG(VNET_LOG_ERROR, "Create mu %s failed!\n",name.c_str());
            return ERROR_NO_MEMORY;
        }

        STATUS ret = m_pMU->SetHandler(this);
        if (SUCCESS != ret)
        {
            VNET_LOG(VNET_LOG_ERROR, "SetHandler mu %s failed! ret = %d\n",name.c_str(),ret);
            return ret;
        }

        ret = m_pMU->Run();
        if (SUCCESS != ret)
        {
            VNET_LOG(VNET_LOG_ERROR, "Run mu %s failed! ret = %d\n",name.c_str(),ret);
            return ret;
        }

        ret = m_pMU->Register();
        if (SUCCESS != ret)
        {
            VNET_LOG(VNET_LOG_ERROR, "Register mu %s failed! ret = %d\n",name.c_str(),ret);
            return ret;
        }

        m_pMU->set_state(S_Startup);
        m_nStateMachines = VNET_STATE_MACHINE_START;
        
        // 给自己发送上电消息，促使消息单元状态切换到工作态
        MessageFrame message(SkyInt32(0), EV_VNET_POWER_ON);
        ret = m_pMU->Receive(message);
        if (SUCCESS != ret)
        {
            SkyAssert(0);
            return ret;
        }
        m_pMU->Print();
        return SUCCESS;
    }

    STATUS CVNetManager::Receive(const MessageFrame& message)
    {
        return m_pMU->Receive(message);
    };

    void CVNetManager::JoinMcGroup(const string& strMG)
    {
        // 加入一个组播组
        STATUS nRet = m_pMU->JoinMcGroup(strMG);
        if (SUCCESS == nRet)
        {
            VNET_LOG(VNET_LOG_INFO, "CVNetManager::JoinGroup:  the vnm joined %s MC Group.\n", strMG.c_str());
        }
        else
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNetManager::JoinGroup: the vnm joined %s MC Group failed, nRet = %d.\n",
                     strMG.c_str(), nRet);
        }
        return ;
    }

    void CVNetManager::ExitMcGroup(const string& strMG)
    {
        // 退出一个组播组
        STATUS nRet = m_pMU->ExitMcGroup(strMG);
        if (SUCCESS == nRet)
        {
            VNET_LOG(VNET_LOG_INFO, "CVNetManager::JoinGroup:  the vnm exited %s MC Group.\n", strMG.c_str());
        }
        else
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNetManager::JoinGroup: the vnm exited %s MC Group failed, nRet = %d.\n",
                     strMG.c_str(), nRet);
        }
    }


    int32 CVNetManager::InitVNetManager()
    {        
        m_strVNMApp = ApplicationName();
        if (m_strVNMApp.empty())
        {
            VNET_LOG(VNET_LOG_ERROR,  "CVNetManager::InitVNetManager:  empty == ApplicationName.\n");
            return -1;
        }
        
        // VNM Ctrl;
        m_pVNMCtrlInst = CVNMCtrl::GetInstance();
        if (NULL == m_pVNMCtrlInst)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNetManager::InitVNetManager: NULL == m_pVNMCtrlInst.\n");
            return -1;
        }
        if (0 != m_pVNMCtrlInst->Init(m_pMU, m_strVNMApp))
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNetManager::InitVNetManager: CVNAManager init failed.\n");
            return -1;
        }

        // VNA MANAGER;
        m_pVNAMgrInst = CVNAManager::GetInstance();
        if (NULL == m_pVNAMgrInst)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNetManager::InitVNetManager: NULL == m_pVNAMgrInst.\n");
            return -1;
        }
        if (0 != m_pVNAMgrInst->Init(m_pMU, m_strVNMApp))
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNetManager::InitVNetManager: CVNAManager init failed.\n");
            return -1;
        }

        // Resource pool; 
        // MAC pool;
        m_pMACMgrInst = CMACPoolMgr::GetInstance();
        if (NULL == m_pMACMgrInst)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNetManager::InitVNetManager: NULL == m_pMACMgrInst.\n");
            return -1;
        }
        if (0 != m_pMACMgrInst->Init(m_pMU))
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNetManager::InitVNetManager: MAC manager init failed.\n");
            return -1;
        }
        // VLAN pool;
        m_pVlanMgrInst = CVlanPoolMgr::GetInstance();
        if (NULL == m_pVlanMgrInst)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNetManager::InitVNetManager: NULL == m_pVlanMgrInst.\n");
            return -1;
        }
        if (0 != m_pVlanMgrInst->Init(m_pMU))
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNetManager::InitVNetManager: Vlan manager init failed.\n");
            return -1;
        }
        // SEGMENT pool;
        m_pSegmentMgrInst = CSegmentPoolMgr::GetInstance();
        if (NULL == m_pSegmentMgrInst)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNetManager::InitVNetManager: NULL == m_pSegmentMgrInst.\n");
            return -1;
        }
        if (0 != m_pSegmentMgrInst->Init(m_pMU))
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNetManager::InitVNetManager: Segment manager init failed.\n");
            return -1;
        }        
        // IP pool;
        m_pIPv4MgrInst = CIPv4PoolMgr::GetInstance();
        if (NULL == m_pIPv4MgrInst)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNetManager::InitVNetManager: NULL == m_pIPv4MgrInst.\n");
            return -1;
        }
        if (0 != m_pIPv4MgrInst->Init(m_pMU))
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNetManager::InitVNetManager: IPv4 manager init failed.\n");
            return -1;
        }

        // netplane;
        m_pNetplaneMgrInst = CNetplaneMgr::GetInstance();
        if (NULL == m_pNetplaneMgrInst)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNetManager::InitVNetManager: NULL == m_pNetplaneMgrInst.\n");
            return -1;
        }
        if (0 != m_pNetplaneMgrInst->Init(m_pMU))
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNetManager::InitVNetManager: netplane manager init failed.\n");
            return -1;
        } 

        // logic network;
        m_pLogicNetworkMgrInst = CLogicNetworkMgr::GetInstance();
        if (NULL == m_pLogicNetworkMgrInst)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNetManager::InitVNetManager: NULL == m_pLogicNetworkMgrInst.\n");
            return -1;
        }
        if (0 != m_pLogicNetworkMgrInst->Init(m_pMU))
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNetManager::InitVNetManager: logic network manager init failed.\n");
            return -1;
        } 

        // vnic;
        m_pVNetVmDB = CVNetVmDB::GetInstance();
        if (NULL == m_pVNetVmDB)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNetManager::InitVNetManager: NULL == m_pVNetVmDB.\n");
            return -1;
        }
        if (0 != m_pVNetVmDB->Init())
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNetManager::InitVNetManager: CVNetVmDB init failed.\n");
            return -1;
        }

        // vnic;
        m_pVNetVnicDB = CVNetVnicDB::GetInstance();
        if (NULL == m_pVNetVnicDB)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNetManager::InitVNetManager: NULL == m_pVNetVnicMgrInst.\n");
            return -1;
        }
        if (0 != m_pVNetVnicDB->Init())
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNetManager::InitVNetManager: CVNetVnicDB init failed.\n");
            return -1;
        }

        // SCHEDULE;
        m_pVNMSchedule = CVNMSchedule::GetInstance();
        if (NULL == m_pVNMSchedule)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNetManager::InitVNetManager: NULL == m_pVNMSchedule.\n");
            return -1;
        }
        if (0 != m_pVNMSchedule->Init())
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNetManager::InitVNetManager: CVNMSchedule init failed.\n");
            return -1;
        } 

        // CVNetVmMgr;
        m_pVNetVMMgrInst = CVNetVmMgr::GetInstance();
        if (NULL == m_pVNetVMMgrInst)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNetManager::InitVNetManager: NULL == m_pVNetVMMgrInst.\n");
            return -1;
        }
        if (0 != m_pVNetVMMgrInst->Init(m_pMU))
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNetManager::InitVNetManager: CVNetVmMgr init failed.\n");
            return -1;
        }   

        // CVNetVmMgrExt;
        m_pVNetVMMgrExtInst = CVNetVmMgrExt::GetInstance();
        if (NULL == m_pVNetVMMgrExtInst)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNetManager::InitVNetManager: NULL == m_pVNetVnicMgrInst.\n");
            return -1;
        }
        if (0 != m_pVNetVMMgrExtInst->Init(m_pMU))
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNetManager::InitVNetManager: CVNetVmMgrExt init failed.\n");
            return -1;
        } 
        
        // vnm query
        m_pVNetVNMQueryInst = CVNMQuery::GetInstance();
        if (NULL == m_pVNetVNMQueryInst)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNetManager::InitVNetManager: NULL == m_pVNetVNMQueryInst.\n");
            return -1;
        }
        if (0 != m_pVNetVNMQueryInst->Init())
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNetManager::InitVNetManager: m_pVNetVNMQueryInst init failed.\n");
            return -1;
        } 
        // vnm vna report
        m_pVNetVNAReportInst = CVNAReportMgr::GetInstance();
        if (NULL == m_pVNetVNAReportInst)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNetManager::InitVNetManager: NULL == m_pVNetVNAReportInst.\n");
            return -1;
        }
        if (0 != m_pVNetVNAReportInst->Init())
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNetManager::InitVNetManager: m_pVNetVNAReportInst init failed.\n");
            return -1;
        } 
        
        //vsi
        m_pVNetVsiMgrInst = CVSIProfileMgr::GetInstance();
        if (NULL == m_pVNetVsiMgrInst)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNetManager::InitVNetManager: NULL == m_pVNetVsiMgrInst.\n");
            return -1;
        }
        if (0 != m_pVNetVsiMgrInst->Init())
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNetManager::InitVNetManager: vsi manager init failed.\n");
            return -1;
        }     
        
        // switch;
        m_pSwitchMgrInst = CSwitchMgr::GetInstance();
        if (NULL == m_pSwitchMgrInst)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNetManager::InitVNetManager: NULL == m_pSwitchMgrInst.\n");
            return -1;
        }
        if (0 != m_pSwitchMgrInst->Init(m_pMU))
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNetManager::InitVNetManager: switch manager init failed.\n");
            return -1;
        }          
        
        // port group;
        m_pPGMgrInst = CPortGroupMgr::GetInstance();
        if (NULL == m_pPGMgrInst)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNetManager::InitVNetManager: NULL == m_pPGMgrInst.\n");
            return -1;
        }
        if (0 != m_pPGMgrInst->Init(m_pMU))
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNetManager::InitVNetManager: portgroup manager init failed.\n");
            return -1;
        }         

        // portmgr;
        m_pPortMgrInst= CPortMgr::GetInstance();
        if (NULL == m_pPortMgrInst)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNetManager::InitVNetManager: NULL == m_pPortMgr.\n");
            return -1;
        }
        if (0 != m_pPortMgrInst->Init(m_pMU))
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNetManager::InitVNetManager: port manager init failed.\n");
            return -1;
        }   

        // wildcastmgr;
        m_pWildcastMgrInst= CWildcastMgr::GetInstance();
        if (NULL == m_pWildcastMgrInst)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNetManager::InitVNetManager: NULL == m_pWildcastMgrInst.\n");
            return -1;
        }
        if (0 != m_pWildcastMgrInst->Init(m_pMU))
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNetManager::InitVNetManager: m_pWildcastMgrInst manager init failed.\n");
            return -1;
        }   

        // vm migrate ;
        m_pVmMigrateMgrInst= CVNetVmMigrateMgr::GetInstance();
        if (NULL == m_pVmMigrateMgrInst)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNetManager::InitVNetManager: NULL == m_pVmMigrateMgrInst.\n");
            return -1;
        }
        if (0 != m_pVmMigrateMgrInst->Init())
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNetManager::InitVNetManager: m_pVmMigrateMgrInst manager init failed.\n");
            return -1;
        }

        // vxlan mgr ;
        m_pVxlanMgr = CVxlanMgr::GetInstance();
        if (NULL == m_pVxlanMgr)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNetManager::InitVNetManager: NULL == m_pVxlanMgr.\n");
            return -1;
        }
        if (0 != m_pVxlanMgr->Init(m_pMU))
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNetManager::InitVNetManager: m_pVxlanMgr manager init failed.\n");
            return -1;
        }        
    
        return 0;
    }

    int32 CVNetManager::PowerOn(void)
    {
#if 0    
        if (SUCCESS != Transaction::Enable(GetDB()))
        {
            VNET_LOG(VNET_LOG_ERROR,  "CVNetManager enable transaction fail!\n");
            SkyExit(-1, "CVNetManager::PowerOn: call Transaction::Enable(GetDB()) failed.\n");
            return -1;
        }
#endif 
        // 完成初始化
        if (0 != InitVNetManager())
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNetManager::PowerOn InitVNetManager failed.\n");
            SkyAssert(false);
            SkyExit(-1,"InitVNetManager failed!\n");
        }
        
        m_nStateMachines = VNET_STATE_MACHINE_WORK;
        m_pMU->set_state(S_Work);
        VNET_LOG(VNET_LOG_INFO, "VNetManager: %s entering VNET_STATE_MACHINE_WORK.\n", 
            m_pMU->name().c_str());   
            
        // 加入组播组
        JoinMcGroup(MUTIGROUP_VNA_REG);
        JoinMcGroup(MUTIGROUP_VNM_MS);
        
        return 0;
    }
    
    void CVNetManager::MessageEntry(const MessageFrame& message)
    {
        if (VNET_TRUE == m_bOpenDbgInf)
        {
            cout << "StateMachines: " << m_pMU->get_state() <<" msg id: " << message.option.id() << endl;
        }

        switch (m_pMU->get_state())
        {
        case S_Startup:
            {
                switch (message.option.id())
                {
                case EV_VNET_POWER_ON:
                    {
                        PowerOn();
                    }
                break;
                
                default:
                    break;
                }
            } // end VNET_STATE_MACHINE_START
            break;        
        case S_Work:
            {
                // 处于WORK状态 各模块都已经上电初始化成功
                int32 nNum = 0;
                TMsgProcReg *pMsgProc = GetMsgProcRegInfo(nNum);

                for(int i =0; i<nNum; i++)
                {           
                    if(pMsgProc[i].msg_id == message.option.id())
                    {
                        // VNA_CTRL模块;
                        if(0 == pMsgProc[i].mod_name.compare(VNM_CTRL_MODULE))
                        {
                            m_pVNMCtrlInst->MessageEntry(message);
                            return;
                        }
                        // VNA_MGR模块;
                        if(0 == pMsgProc[i].mod_name.compare(VNA_MGR_MODULE))
                        {
                            m_pVNAMgrInst->MessageEntry(message);
                            return;
                        }
                        // VLAN资源池模块;
                        if(0 == pMsgProc[i].mod_name.compare(VNET_VLAN_RES_MODULE))
                        {
                            m_pVlanMgrInst->MessageEntry(message);
                            return;
                        }
                        // SEGMENT资源池模块;
                        if(0 == pMsgProc[i].mod_name.compare(VNET_SEGMENT_RES_MODULE))
                        {
                            m_pSegmentMgrInst->MessageEntry(message);
                            return;
                        }
                        
                        // MAC资源池模块;
                        if(0 == pMsgProc[i].mod_name.compare(VNET_MAC_RES_MODULE))
                        {
                            m_pMACMgrInst->MessageEntry(message);
                            return;
                        }
                        // IP资源池模块;
                        if(0 == pMsgProc[i].mod_name.compare(VNET_IP_RES_MODULE))
                        {
                            m_pIPv4MgrInst->MessageEntry(message);
                            return;
                        }
                        // Netplane模块;
                        if(0 == pMsgProc[i].mod_name.compare(VNM_NETPLANE_MODULE))
                        {
                            m_pNetplaneMgrInst->MessageEntry(message);
                            return;
                        }
                        // Logic network模块;
                        if(0 == pMsgProc[i].mod_name.compare(VNM_LOGIC_NETWORK_MODULE))
                        {
                            m_pLogicNetworkMgrInst->MessageEntry(message);
                            return;
                        }  
                        // CVNetVmMgr模块;
                        if(0 == pMsgProc[i].mod_name.compare(VNM_VM_MGR_MODULE))
                        {
                            m_pVNetVMMgrInst->MessageEntry(message);
                            return;
                        }                        
                        // CVNetVmMgrExt模块;
                        if(0 == pMsgProc[i].mod_name.compare(VNM_VM_MGR_EXT_MODULE))
                        {
                            m_pVNetVMMgrExtInst->MessageEntry(message);
                            return;
                        }                        
                        // Switch模块;
                        if(0 == pMsgProc[i].mod_name.compare(VNM_SWITCH_MODULE))
                        {
                            m_pSwitchMgrInst->MessageEntry(message);
                            return;
                        }                         
                        // portgroup模块;
                        if(0 == pMsgProc[i].mod_name.compare(VNM_PORTGROUP_MODULE))
                        {
                            m_pPGMgrInst->MessageEntry(message); 
                            return;
                        }                         
                        // port模块;
                        if(0 == pMsgProc[i].mod_name.compare(VNM_PORT_MODULE))
                        {
                            m_pPortMgrInst->MessageEntry(message); 
                            return;
                        } 
                        // wildcast模块;
                        if(0 == pMsgProc[i].mod_name.compare(VNM_WILDCAST_MODULE))
                        {
                            m_pWildcastMgrInst->MessageEntry(message); 
                            return;
                        } 

                        // vxlanmgr模块;
                        if(0 == pMsgProc[i].mod_name.compare(VNM_VXLAN_MODULE))
                        {
                            m_pVxlanMgr->MessageEntry(message); 
                            return;
                        } 
                        
                    }
                }

                VNET_LOG(VNET_LOG_INFO, "VNetManager: unknown message(ID=%d).\n", message.option.id());

            } // end POWERON
            break;
        default:
            break;
        }// end switch(mu->get_state())
    }  

}// end namespace zte_tecs

