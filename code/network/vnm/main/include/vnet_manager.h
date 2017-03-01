/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�vnet_manager.h
* �ļ���ʶ��
* ����ժҪ��CVNetManager��Ķ����ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�
* ������ڣ�2011��8��16��
*
* �޸ļ�¼1��
*     �޸����ڣ�2011/8/16
*     �� �� �ţ�V1.0
*     �� �� �ˣ�Wang.Lule
*     �޸����ݣ�����

*     �޸����ڣ�2012/12/12
*     �� �� �ţ�V2.0
*     �� �� �ˣ�Wang.Lule
*     �޸����ݣ�����
******************************************************************************/

#if !defined(VNET_MANAGER_INCLUDE_H__)
#define VNET_MANAGER_INCLUDE_H__

#include "vnm_vxlan_mgr.h"
#include "vnm_query.h"
#include "vna_report_mgr.h"

namespace zte_tecs
{
    class CVNetManager : public MessageHandler
    {    
    private:
        explicit CVNetManager();
    public:
        virtual ~CVNetManager();
    public:    
        static CVNetManager *GetInstance()
        {
            if(NULL == s_pInstance)
            {
                s_pInstance = new CVNetManager();
            }
            return s_pInstance;
        };

        STATUS Init()
        {        
            STATUS ret = SUCCESS;
            ret = StartMu(MU_VNM);
            if( ret != SUCCESS )
            {               
                VNET_LOG(VNET_LOG_ERROR, "CVNetManager::Init StartMu failed.\n");
                return ret;
            }
            
            return SUCCESS;
        }

        void SetDbgInfFlg(BOOL bOpen){ m_bOpenDbgInf = bOpen; return ;}
        int32 DbgShowData(void);
        
    protected:
        int32 PowerOn(void);
        STATUS Receive(const MessageFrame& message);
        void MessageEntry(const MessageFrame& message);
        int32 InitVNetManager(void);
        void JoinMcGroup(const string& strMG);
        void ExitMcGroup(const string& strMG);
        STATUS StartMu(const string& name);

    private:    
        static CVNetManager * s_pInstance;
        MessageUnit *m_pMU;
        int32 m_nStateMachines;
        BOOL m_bOpenDbgInf;
        string m_strVNMApp;

        CVNMCtrl            *m_pVNMCtrlInst;
        CVNAManager         *m_pVNAMgrInst;
        CVlanPoolMgr        *m_pVlanMgrInst;
        CSegmentPoolMgr     *m_pSegmentMgrInst;        
        CMACPoolMgr         *m_pMACMgrInst;
        CIPv4PoolMgr        *m_pIPv4MgrInst;
        CNetplaneMgr        *m_pNetplaneMgrInst;
        CLogicNetworkMgr    *m_pLogicNetworkMgrInst;
        CVNetVnicDB         *m_pVNetVnicDB;
        CVNetVmDB           *m_pVNetVmDB;
        CVNMSchedule        *m_pVNMSchedule;
        CVNetVmMgr          *m_pVNetVMMgrInst;
        CVNetVmMgrExt       *m_pVNetVMMgrExtInst;
        CVSIProfileMgr      *m_pVNetVsiMgrInst;
        CSwitchMgr          *m_pSwitchMgrInst;
        CPortGroupMgr       *m_pPGMgrInst;
        CPortMgr            *m_pPortMgrInst;
        CWildcastMgr        *m_pWildcastMgrInst;
        CVNetVmMigrateMgr   *m_pVmMigrateMgrInst;
        CVxlanMgr           *m_pVxlanMgr;
        
        CVNMQuery           *m_pVNetVNMQueryInst;
        CVNAReportMgr       *m_pVNetVNAReportInst;
        

        DISALLOW_COPY_AND_ASSIGN(CVNetManager);
    };
    
}// namespace zte_tecs

#endif

