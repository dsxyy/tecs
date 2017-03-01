/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：vnet_evb_mgr.cpp
* 文件标识：
* 内容摘要：CEvbMgr类实现文件
* 当前版本：1.0
* 作    者：
* 完成日期：2012年5月22日
*
* 修改记录1：
*     修改日期：2012/5/22
*     版 本 号：V1.0
*     修 改 人：Wang.Lule
*     修改内容：创建
******************************************************************************/
#include "vna_common.h"
#include "vnet_vnic.h"
#include "vnet_comm.h"
#include "vnet_event.h"
#include "vm_messages.h"

#include "vnet_portinfo.h"
#include "vnet_portagent.h"
#include "dev_monitor.h"
#include "dev_mgr.h"
#include "vna_switch_module.h"

#include "vna_switch_agent.h"

#include "evb_comm.h"
#include "evb_timer.h"
#include "evb_entry.h"
#include "evb_port_mgr.h"
#include "lldp_in.h"
#include "vdp.h"
#include "evb_dbg_func.h"
#include "vsi_profile.h"
#include "vsi_b_outFilter.h"
#include "evb_tlv.h"
#include "vnet_evb_mgr.h"

//#include "lnxrsock_drv.h"

extern "C" void DbgShowEvbRawSocket(u32 uiPort);

namespace zte_tecs
{
    extern bool enable_evb;
    int32 g_iEvbMgrPrint = 0;
    DEFINE_DEBUG_VAR(g_iEvbMgrPrint);

    CEvbMgr* CEvbMgr::s_pInstance = NULL;
    CEvbMgr::CEvbMgr()
    {
        m_bEvbEnable = VNET_TRUE;/* 默认启用 */
        m_p20msTimer = NULL;
    };

    CEvbMgr::~CEvbMgr()
    {
        delete s_pInstance;
        s_pInstance = NULL;
        
        if(m_bEvbEnable)
        {
            ShutdownEvbModule();
        }
    };


    int32 CEvbMgr::Init()
    {
        // 为 ci 添加的关闭evb功能; 
        // ParseEvbEnable();
        m_bEvbEnable = FALSE;
        #if 0
        if(VNET_FALSE == m_bEvbEnable)
        {
            VNET_LOG(VNET_LOG_INFO,
                "CEvbMgr::Init: EVB is closed.\n");
            return 0;
        }
        #endif
        
        m_p20msTimer = C20msMsgTimer::GetInstance();
        if(NULL == m_p20msTimer)
        {
            VNET_LOG(VNET_LOG_ERROR,
                "CEvbMgr::Init: call C20msMsgTimer::GetInstance() failed.\n");
            return 0;
        }
        if(SUCCESS != m_p20msTimer->Init())
        {
            VNET_LOG(VNET_LOG_ERROR,
                "CEvbMgr::Init: call C20msMsgTimer::init() failed.\n");
            return 0;
        }
        
        /* Start EVB */
        if(0 != StartEvbModule(EVBSYS_TYPE_STATION))
        {
            VNET_LOG(VNET_LOG_ERROR,
                "CEvbMgr::Init: call StartEvbModule(EVBSYS_TYPE_STATION) failed.\n");
            return 0;
        }

        if(0 != m_p20msTimer->StartTimer())
        {
            VNET_LOG(VNET_LOG_ERROR,
                "CEvbMgr::Init: call C20msMsgTimer::StartTimer() failed.\n");
            return 0;
        }

        m_uiISSPortIdx = 0;
        m_bEvbEnable = TRUE;
        return 0;
    }

    void CEvbMgr::ParseEvbEnable()
    {
        m_bEvbEnable = enable_evb;
                    
        return;
    }

    int32 CEvbMgr::ConfigISSPort(const string &strPortName, const uint32 &uiPortNo)
    {
        BYTE aucMac[EVB_PORT_MAC_LEN];

        /* 重复配置检查; */
        if (m_mapISSPort.end() != m_mapISSPort.find(strPortName))
        {
            VNET_LOG(VNET_LOG_WARN,
                     "CEvbMgr::ConfigISSPort: the ISS Port(%s, %u) is configured.\n",
                     strPortName.c_str(), uiPortNo);
            return 0;
        }

        MEMSET(aucMac, 0x0, sizeof(aucMac));
        if (0 == EvbGetNicMac(strPortName.c_str(), aucMac, EVB_PORT_MAC_LEN))
        {
            if(0 == AddEvbIssPort(uiPortNo, strPortName.c_str(), aucMac, 1, EM_EVB_PT_URP))
            {
                m_mapISSPort.insert(make_pair(strPortName, uiPortNo));
            }
            else
            {
                VNET_LOG(VNET_LOG_ERROR,
                         "CEvbMgr::ConfigISSPort: call AddEvbIssPort(%d, %s...)failed.\n",
                         uiPortNo, strPortName.c_str());
                return -1;
            }
        }
        else
        {
            VNET_LOG(VNET_LOG_ERROR,
                     "CEvbMgr::ConfigISSPort: call EvbGetNicMac(%s...)failed.\n",
                     strPortName.c_str());
            return -1;
        }

        return 0;
    }

    int32 CEvbMgr::NoCfgISSPort(const string &strPortName)
    {
        map<string, uint32>::iterator pos = m_mapISSPort.find(strPortName);
        if (m_mapISSPort.end() == pos)
        {
            VNET_LOG(VNET_LOG_ERROR,
                     "CEvbMgr::NoCfgISSPort: the ISS Port(%s) is not configured.\n",
                     strPortName.c_str());
            return 0;
        }

        if(0 != DelEvbIssPort(pos->second))
        {
            VNET_LOG(VNET_LOG_ERROR,
                     "CEvbMgr::NoCfgISSPort: call DelEvbIssPort (%d) failed.\n",
                     pos->second);
            return -1;
        }
        
        m_mapISSPort.erase(pos);
        
        return 0;
    }

    int32 CEvbMgr::ChkVMOpt(BYTE option)
    {
        switch (option)
        {
        case VNET_VM_DEPLOY:
        case VNET_VM_PREPARE:
        case VNET_VM_MIGRATE:
        case VNET_VM_SUSPEND:
        case VNET_VM_CANCEL:
        {
            break;
        }
        default:
        {
            EVB_ASSERT(0);
            return -1;
        }
        }

        return 0;
    }

    int CEvbMgr::ChkVSIParam(vector<CVNetVnicDetail> &vecNic)
    {
        if (vecNic.empty())
        {
            return 0;
        }

        /* check VmNicConfig; */
        vector<CVNetVnicDetail>::iterator it_vNic = vecNic.begin();
        for (; it_vNic != vecNic.end(); ++it_vNic)
        {
            if ((*it_vNic).GetVSIIDValue().size() != VDPTLV_VSIID_LEN*2 )
            {
                EVB_LOG(EVB_LOG_ERROR,"ChkVSIParam: VmNicConfig param _vsiid (%u) is invalid.\n", (*it_vNic).GetVSIIDValue().size());
                return -1;
            }

            if ((*it_vNic).GetMgrID().size() != VDPTLV_VSIMGRID_LEN*2)
            {
                EVB_LOG(EVB_LOG_ERROR,"ChkVSIParam: VmNicConfig param _mgrid (%u) is invalid.\n", (*it_vNic).GetMgrID().size());
                return -1;
            }

            if ((*it_vNic).GetVSITypeID().size() != VDPTLV_VSITYPEID_LEN*2)
            {
                EVB_LOG(EVB_LOG_INFO,"ChkVSIParam: VmNicConfig param _vsitypeid (%u) is invalid.\n", (*it_vNic).GetVSITypeID().size());
                return -1;
            }

            string strMacAddress;
            if (0 != (*it_vNic).GetAssMac().size())
            {
                strMacAddress = (*it_vNic).GetAssMac();
            }
            else
            {
                strMacAddress = (*it_vNic).GetMacAddress();
            }
 
            if ( 0 != vnet_check_macstr(strMacAddress) )
            {
                EVB_LOG(EVB_LOG_INFO,"ChkVSIParam: VmNicConfig _mac(%s) is invalid.\n",strMacAddress.c_str());
                return -1;
            }
        }

        return 0;
    }

    uint32 CEvbMgr::GetISSPortNo(const string &c_strPortName)
    {

        if (c_strPortName.empty())
        {
            VNET_LOG(VNET_LOG_ERROR, "CEvbMgr::GetISSPortNo: net plane is NULL.\n");
            VNET_ASSERT(0);
            return EVB_INVALID_PORT_NO;
        }
        
        if (m_mapISSPort.end() == m_mapISSPort.find(c_strPortName))
        {
            /* 没有找到对应的port no */
            VNET_LOG(VNET_LOG_INFO, "CEvbMgr::GetISSPortNo: Param strISSPortName(%s) is invalid .\n",
                     c_strPortName.c_str());
            return EVB_INVALID_PORT_NO;
        }

        return m_mapISSPort[c_strPortName];
    }

    int32 CEvbMgr::IsNotNeedEVB(vector<CVNetVnicDetail> &vecNic,vector<CVNetVnicDetail> &vecEvbNic)
    {
        if (vecNic.empty())
        {
            VNET_ASSERT(0);
            return -1;
        }
        CSwitchAgent *pSwitch = CSwitchAgent::GetInstance();
        if(NULL == pSwitch)
        {
            VNET_LOG(VNET_LOG_ERROR, "CEvbMgr::IsNotNeedEVB: can't get the CSwitchAgent instance.\n"); 
            return -1;
        }

        int32 bNotNeedEvb = true;

        vector<CVNetVnicDetail>::iterator itr = vecNic.begin();
        for (itr = vecNic.begin(); itr != vecNic.end(); ++itr)
        {
            if (ISOLATE_VXLAN == itr->GetIsolateType())
            {
                continue;
            }
            // 此处需要判断交换是否是EVB 模式
            int32 nMode = 0;
            pSwitch->GetEVBMode(itr->GetSwitchName(), nMode);
            if(EN_EVB_MODE_VEB == nMode || EN_EVB_MODE_VEPA == nMode)
            {
                vecEvbNic.push_back((*itr));
                bNotNeedEvb = false;
            }
        }
        
        return bNotNeedEvb;
    }

    /* 存在多网口，但都在同一个URP下 */
    int32 CEvbMgr::BuildVNicURPMap(vector<CVNetVnicDetail> &vecNic,
                                   map<CVNetVnicDetail*, uint32> &mapVNicURP)
    {
        if (vecNic.empty() || !mapVNicURP.empty())
        {
            VNET_ASSERT(0);
            return -1;
        }

        CSwitchAgent *pSwitch = CSwitchAgent::GetInstance();
        if(NULL == pSwitch)
        {
            VNET_LOG(VNET_LOG_ERROR, "CEvbMgr::BuildVNicURPMap: can't get the CSwitchAgent instance.\n"); 
            return -1;
        }

        int32 bEvbSwitch = 0;
        vector<CVNetVnicDetail>::iterator itr = vecNic.begin();
        for (itr = vecNic.begin(); itr != vecNic.end(); ++itr)
        {
            if (ISOLATE_VXLAN == itr->GetIsolateType())
            {
                continue;
            }            

            // 此处需要判断交换是否是EVB 模式
            int32 nMode = 0;
            pSwitch->GetEVBMode(itr->GetSwitchName(), nMode);
            if(EN_EVB_MODE_VEB != nMode && EN_EVB_MODE_VEPA != nMode)
            {
                continue ;
            }
            
            bEvbSwitch = 1;
            uint32 uiISSPortNo = GetISSPortNo(itr->GetPortName());
            if (EVB_INVALID_PORT_NO == uiISSPortNo)
            {
                /* 没有找到对应的ISS Port; */
                VNET_LOG(VNET_LOG_INFO,
                         "CEvbMgr::BuildVNicURPMap: call GetISSPortNo failed.\n");
                return -1;
            }

            /* iss port --> urp port; */
            int32 bFindEvbPort = 0;
            struct tagEvbPort *pEVBPort = NULL;
            for (pEVBPort = Get1stEvbPort(); pEVBPort; pEVBPort = GetNextEvbPort(pEVBPort))
            {
                if (EM_EVB_PT_URP != pEVBPort->ucEvbPortType)
                {
                    continue ;
                }

                if (pEVBPort->pIssPort)
                {
                    if (pEVBPort->pIssPort->uiPortNo == uiISSPortNo )
                    {
                        bFindEvbPort = 1;
                        mapVNicURP.insert(make_pair( &(*itr),pEVBPort->uiPortNo));
                        break;
                    }
                }
            }/* end for(pPort...;  */

            if (1 != bFindEvbPort )
            {
                /* 此处后续Multi-Channel支持后需要重新考虑，因为有UAP口;  */
                VNET_LOG(VNET_LOG_ERROR,
                         "CEvbMgr::BuildVNicURPMap (issPort: %u) failed.\n",
                         uiISSPortNo);
                return -1;
            }
        }/* end for(itr...;  */

        if(0 == bEvbSwitch)
        {
            VNET_LOG(VNET_LOG_INFO,
                     "CEvbMgr::BuildVNicURPMap vnic is not connected with qbg switch.\n");
            return -1;
        }

        return 0;
    }

    int32 CEvbMgr::GetAllURP(map<CVNetVnicDetail*, uint32> &mapVNicURP, vector<uint32> &vecURPNo)
    {
        /* 从map中提取, 避免重复构造urp检查; 
           当存在多个nic，在同一个urp上，必须如此实现 
        */
        map<CVNetVnicDetail*, uint32>::iterator itrMap;
        vector<uint32>::iterator itrVec;

        if (mapVNicURP.empty() || (!vecURPNo.empty()))
        {
            EVB_ASSERT(0);
            return -1;
        }

        for (itrMap = mapVNicURP.begin(); itrMap != mapVNicURP.end(); ++itrMap)
        {
            vecURPNo.push_back(itrMap->second);
        }

        if (vecURPNo.empty())
        {
            VNET_LOG(VNET_LOG_ERROR, "CEvbMgr::GetAllURP: get urp no failed...\n");
            EVB_ASSERT(0);
            return -1;
        }

        return 0;
    }

    int32 CEvbMgr::GetVMEVBTlvStat(map<CVNetVnicDetail*,uint32> &mapVNicURP, map<CVNetVnicDetail*,int32> &mVnicEvb)
    {
        map<CVNetVnicDetail*,uint32>::iterator itrMapVNicURP;
        vector<uint32> vecAllURPNo; /* 从map中提取, 避免重复构造urp检查; */
        vector<uint32>::iterator itrVec;

        if (mapVNicURP.empty())
        {
            EVB_ASSERT(0);
            return -1;
        }

        /* 检查所有urp口的evb tlv协商结果; 仅仅查询，不会等待协商结果 */
        itrMapVNicURP = mapVNicURP.begin();
        for(; itrMapVNicURP != mapVNicURP.end(); ++itrMapVNicURP)
        {
            if (EVB_TLV_STATUS_SUCC == GetEcpVdp_evbtlvStatus( (*itrMapVNicURP).second))
            {
                mVnicEvb.insert(make_pair( (*itrMapVNicURP).first,TRUE));
            }
            else
            {
                mVnicEvb.insert(make_pair((*itrMapVNicURP).first,FALSE));
            }
        }

        return 0;
    }

    int32 CEvbMgr::FormatVSIID(const string & s, BYTE *vsiid)
    {
        int tmp[VDPTLV_VSIID_LEN]={0};
        if ( NULL == vsiid || (s.size() != VDPTLV_VSIID_LEN*2))
        {
            return -1;
        }

        sscanf(s.c_str(),VSIFORMAT_SCANF,&tmp[0], &tmp[1],\
               &tmp[2], &tmp[3],&tmp[4],&tmp[5],&tmp[6],&tmp[7],&tmp[8],\
               &tmp[9], &tmp[10],&tmp[11],&tmp[12],&tmp[13],&tmp[14],&tmp[15]);

        for (int i = 0; i<16; i++)
        {
            vsiid[i] = (BYTE)tmp[i];
        }
        return 0;
    }

    int32 CEvbMgr::FormatMgrID(const string & s, BYTE *mgrid)
    {
        int tmp[VDPTLV_VSIMGRID_LEN]={0};
        if ( NULL == mgrid || (s.size() != VDPTLV_VSIMGRID_LEN*2))
        {
            return -1;
        }

        sscanf(s.c_str(),MGRIDFORMAT_SCANF,&tmp[0], &tmp[1],\
               &tmp[2], &tmp[3],&tmp[4],&tmp[5],&tmp[6],&tmp[7],&tmp[8],\
               &tmp[9], &tmp[10],&tmp[11],&tmp[12],&tmp[13],&tmp[14],&tmp[15]);

        for (int i = 0; i<16; i++)
        {
            mgrid[i] = (BYTE)tmp[i];
        }
        return 0;
    }

    int32 CEvbMgr::FormatVSITypeID(const string & s, BYTE *typid)
    {
        int tmp[VDPTLV_VSITYPEID_LEN]={0};
        if ( NULL == typid || (s.size() != VDPTLV_VSITYPEID_LEN*2))
        {
            return -1;
        }

        sscanf(s.c_str(),VSITYPEIDFORMAT_SCANF,&tmp[0], &tmp[1],&tmp[2]);

        for (int i = 0; i<16; i++)
        {
            typid[i] = (BYTE)tmp[i];
        }
        return 0;
    }

    /*
       此函数内部将不会等待 协商结果 
       函数可重入 
    */
    int32 CEvbMgr::ExecVDPCmd4VSI(uint32 uiURPNo, CVNetVnicDetail*pNic, BYTE option)
    {
        BYTE vsiid[VDPTLV_VSIID_LEN] = {0};
        BYTE mgrid[VDPTLV_VSIMGRID_LEN] = {0};
        BYTE vsitypeid[VDPTLV_VSITYPEID_LEN]= {0};
        BYTE groupid[EVB_VDP_GROUPID_LEN]={0};

        if ( NULL == pNic)
        {
            VNET_LOG(VNET_LOG_ERROR, "CEvbMgr::EvbInstallVsi: Param is NULL. \n");
            return -1;
        }

        FormatVSIID(pNic->GetVSIIDValue(),vsiid);
        FormatMgrID(pNic->GetMgrID(),mgrid);
        FormatVSITypeID(pNic->GetVSITypeID(),vsitypeid);

        struct tagVDP_FLT_GROUPMACVID  fltGMV;
        MEMSET(&fltGMV,0, sizeof(fltGMV));

        /* mac地址格式需转换; */
        string strMacAddress;
        if (0 != pNic->GetAssMac().size())
        {
            strMacAddress = pNic->GetAssMac();
        }
        else
        {
            strMacAddress = pNic->GetMacAddress();
        }
            
        if ( 0 != vnet_mac_str_to_byte(strMacAddress,fltGMV.mac) )
        {
            EVB_LOG(EVB_LOG_ERROR,"CEvbMgr::EvbInstallVsi vnet_mac_str_to_byte(%s) failed.\n",strMacAddress.c_str());
            return -1;
        }

        if ( 0 != vsiProfile_addvsivlanmac(pNic->GetVSIIDFormat(),vsiid,mgrid,vsitypeid,pNic->GetVSITypeVersion(),groupid,pNic->GetAccessCvlan(),fltGMV.mac) )
        {
            EVB_LOG(EVB_LOG_ERROR,"CEvbMgr::EvbInstallVsi vsiProfile_addvsivlanmac failed.\n");
            return -1;
        }

        /*cmdpool*/
        if ( 0 != vsiCmd_pool_add(uiURPNo,pNic->GetVSIIDFormat(),vsiid,option) )
        {
            EVB_LOG(EVB_LOG_ERROR,"CEvbMgr::EvbInstallVsi vsiCmd_pool_add failed.\n");
            return -1;
        }
#if 0
        if ( (option <= VDPTLV_TYPE_ASSOCIATE) && (option >= VDPTLV_TYPE_PRE_ASSOCIATE ))
        {
            int waitTimes = 0;
            int ret = 0;
            while (1)
            {
                waitTimes++;
                /* 30s */
                if (waitTimes >= 30)
                {
                    break;
                }
                EvbDelay(1000);

                ret = vdp_lookup_vsiState(uiURPNo,pNic->GetVSIIDFormat(),vsiid);
                if (VSISTATE_FAILED == ret )
                {
                    EVB_LOG(EVB_LOG_ERROR,"CEvbMgr::EvbInstallVsi vdp_lookup_vsiState return failed.\n");
                    return -1;
                }

                if (VSISTATE_SUCCESS==ret)
                {
                    return 0;
                }
            }
        }
#endif 
        return 0;
    }

    int32 CEvbMgr::ChkURPVDPStat(map<CVNetVnicDetail*, uint32> &mapURPVNic)
    {
        BYTE vsi[VDPTLV_VSIID_LEN] = {0};
        struct tagEvbIssPort * issPort = NULL;
        vector<uint32> vecAllURPNo; /* 从map中提取, 避免重复构造urp检查; */
        vector<uint32>::iterator itrVec;

        if (mapURPVNic.empty())
        {
            EVB_ASSERT(0);
            return -1;
        }

        if (0 != GetAllURP(mapURPVNic, vecAllURPNo))
        {
            VNET_LOG(VNET_LOG_ERROR, "CEvbMgr::ChkURPVDPStat: call GetAllURP failed.\n");
            return -1;
        }

        for (itrVec = vecAllURPNo.begin(); itrVec != vecAllURPNo.end(); ++itrVec)
        {
            issPort = GetIssPortFromEvbPort(*itrVec);
            if ( NULL == issPort)
            {
                VNET_LOG(VNET_LOG_ERROR, "CEvbMgr::ChkURPVDPStat: call GetIssPortFromEvbPort(%u) failed.\n",
                         *itrVec);
                return -1;
            }

            memset(vsi,0, VDPTLV_VSIID_LEN);

            vsi[10]=issPort->aucMac[0];
            vsi[11]=issPort->aucMac[1];
            vsi[12]=issPort->aucMac[2];
            vsi[13]=issPort->aucMac[3];
            vsi[14]=issPort->aucMac[4];
            vsi[15]=issPort->aucMac[5];

            // 0x5 == uuid format,后续用宏替代;
            if (VSISTATE_SUCCESS != vdp_lookup_vsiState((*itrVec), 0x5, vsi))
            {
                VNET_LOG(VNET_LOG_ERROR, "CEvbMgr::ChkURPVDPStat: call vdp_lookup_vsiState(%u, %x:%x:%x:%x:%x:%x) failed.\n",\
                         (*itrVec),vsi[10],vsi[11],vsi[12],vsi[13],vsi[14],vsi[15]);
                return -1;
            }
        }

        return 0;
    }

    int32 CEvbMgr::GetEvbIssPortNo(const string &strPortName)
    {
        if(strPortName.empty())
        {
            VNET_ASSERT(0);
            return -1;
        }
        
        map<string, uint32>::iterator pos = m_mapISSPort.find(strPortName);
        if (m_mapISSPort.end() != pos)
        {
            return (int32)(pos->second);
        }
        
        return -1;
    }


    int32 CEvbMgr::SetEvbIssPort(const string &strPortName)
    {
        if(strPortName.empty())
        {
            VNET_ASSERT(0);
            return -1;
        }
        
        if(VNET_FALSE == m_bEvbEnable)
        {
            VNET_LOG(VNET_LOG_WARN,
                "CEvbMgr::SetEvbIssPort: EVB is not enable.\n");
            return -1;
        }
        
        uint32 uiPortNo = m_uiISSPortIdx+1;
        if(0 != ConfigISSPort(strPortName, uiPortNo))
        {    
            VNET_LOG(VNET_LOG_ERROR,
                "CEvbMgr::SetEvbIssPort: call ConfigISSPort(%s, %d) failed.\n", 
                strPortName.c_str(), uiPortNo);
            return -1;
        }
        m_uiISSPortIdx = (m_uiISSPortIdx++)%4096;

        VNET_LOG(VNET_LOG_INFO, "CEvbMgr::SetEvbIssPort: Call ConfigISSPort successfully, ISS PORT NUM[%u].\n",
                 uiPortNo);
        return 0;
    }

    
    int32 CEvbMgr::UnsetEvbIssPort(const string &strPortName)
    {
        if(strPortName.empty())
        {
            VNET_ASSERT(0);
            return -1;
        }
        
        if(VNET_FALSE == m_bEvbEnable)
        {
            VNET_LOG(VNET_LOG_WARN,
                "CEvbMgr::UnsetEvbIssPort: EVB is not enable.\n");
            return -1;
        }
        
        if(0 != NoCfgISSPort(strPortName))
        {    
            VNET_LOG(VNET_LOG_ERROR,
                "CEvbMgr::UnsetEvbIssPort: call NoCfgISSPort(%s) failed.\n", 
                strPortName.c_str());
            return -1;
        }

        VNET_LOG(VNET_LOG_INFO, "CEvbMgr::UnsetEvbIssPort: Call NoCfgISSPort successfully, ISS PORT[%s].\n",
                 strPortName.c_str());
        return 0;
    }

    int32 CEvbMgr::SetEvbMode(const string &strPortName, int32 nEVBMode)
    {
        if(strPortName.empty())
        {
            VNET_ASSERT(0);
            return -1;
        }
        
        if(VNET_FALSE == m_bEvbEnable)
        {
            VNET_LOG(VNET_LOG_WARN,
                "CEvbMgr::SetEvbMode: EVB is not enable.\n");
            return -1;
        }

        if(EN_EVB_MODE_VEB != nEVBMode && 
           EN_EVB_MODE_VEPA != nEVBMode && 
           EN_EVB_MODE_NORMAL != nEVBMode)
        {
            return -1;
        }
        
        map<string, uint32>::iterator pos = m_mapISSPort.find(strPortName);
        if (m_mapISSPort.end() == pos)
        {
            VNET_LOG(VNET_LOG_ERROR,
                     "CEvbMgr::SetEvbMode: the ISS Port(%s) is not configured.\n",
                     strPortName.c_str());
            return -1;
        }
        
        uint8 ucMode = 0;
        if(EN_EVB_MODE_VEPA == nEVBMode)
        {
            ucMode = EN_VEPA_MODE;
        }
        else
        {
            ucMode = EN_VEB_MODE;
        }  

        if(0 != ChgEvbIssPortMode(pos->second, ucMode))
        {
            VNET_LOG(VNET_LOG_ERROR,
                "CEvbMgr::SetEvbMode: call ChgEvbIssPortMode(%d, %d) failed.\n", 
                pos->second, ucMode);
        }

        VNET_LOG(VNET_LOG_INFO, "CEvbMgr::SetEvbMode: Call ChgEvbIssPortMode successfully, ISS PORT No[%d].\n",
                 pos->second);
        return 0;
    }

    /* startnetwork evb 流程
           检查evb tlv 协商结果
           1 如果 检查到URP evb tlv 协商存在部分失败
              将vsi profile and cmd pool 删除，返回失败
           2 如果 检查到 URP evb tlv 协商全部成功
             首先确保 vsi profile and cmd pool 中存在此信息
             2.1.1 检查vsi profile state 
                2.1.1.1 如果全部ok --- 和 option 一致
                     返回成功
                2.1.1.1 如果部分ok 
                     返回RUNNING 等待                 
    */
    int32 CEvbMgr::StartVPortProc(const int64 &nVMID, map<CVNetVnicDetail*, uint32> &mapVNicTblURP,BYTE option)
    {
        int32 ret = 0;
        
        // 如果vnic为空，直接返回成功
        if( mapVNicTblURP.empty())
        {
            return START_VPORT_DISCOVERY_SUCCESS;
        }
        
        int32 bVsiRunning = FALSE;
        BYTE vsiid[VDPTLV_VSIID_LEN] = {0};            
        BYTE evb_oper = VDPTLV_TYPE_ASSOCIATE;        
        int32 bExistEvbSuc = FALSE;
        int32 bExistEvbFailed = FALSE;

        map<CVNetVnicDetail*, int32>::iterator itEvb;
        CVNetVnicDetail* pVNicDetail = NULL;
        map<CVNetVnicDetail*, uint32>::iterator itVNicUrp;

        // 判断evb tlv 状态 
        map<CVNetVnicDetail*,int32> mVnicEvb;
        map<CVNetVnicDetail*,int32>::iterator itVnicEvb;
        if (0 != GetVMEVBTlvStat(mapVNicTblURP,mVnicEvb))
        {
            EVB_LOG(EVB_LOG_ERROR,"CEvbMgr::StartVPortProc: call GetVMEVBTlvStat failed.\n");
            goto STARTPROC_FAILED;
        }

        itVnicEvb= mVnicEvb.begin();
        for(; itVnicEvb != mVnicEvb.end(); ++itVnicEvb)
        {
            if( (*itVnicEvb).second == FALSE )
            {
                EVB_LOG(EVB_LOG_ERROR,"CEvbMgr::StartVPortProc: EVB TLV negoti failed.\n");
                goto STARTPROC_FAILED;
            }
        }

        // check dom0 vsi state
        if ( 0 != ChkURPVDPStat(mapVNicTblURP))
        {
            VNET_LOG(VNET_LOG_ERROR, "CEvbMgr::StartVPortDiscovery: CheckDom0VsiState failed.\n");
            goto STARTPROC_FAILED;
        }

        // option --> evb oper
        switch (option)
        {
            case VNET_VM_DEPLOY:
            {
                evb_oper = VDPTLV_TYPE_ASSOCIATE;
            }
            break;
            case VNET_VM_PREPARE:
            {
                evb_oper = VDPTLV_TYPE_PRE_ASSOCIATE_WITH_RR;
            }
            break;
            case VNET_VM_MIGRATE:
            {
                evb_oper = VDPTLV_TYPE_ASSOCIATE;
            }
            break;
            default:
            {
                VNET_LOG(VNET_LOG_ERROR, "CEvbMgr::StartVPortDiscovery: param option(%u) invalid.\n",option);
                return START_VPORT_DISCOVERY_FAILED;
            }
        }

        
        // 是否URP EVB TLV 是否都成功
        itEvb = mVnicEvb.begin();
        for(; itEvb != mVnicEvb.end(); ++itEvb)
        {
            if((*itEvb).second == FALSE )
            {
                bExistEvbFailed = TRUE;
            }
            if((*itEvb).second == TRUE )
            {
                bExistEvbSuc = TRUE;
            }
        }

        // 1 如果 检查到URP evb tlv 协商存在部分失败
        itVNicUrp = mapVNicTblURP.begin();
        if( bExistEvbFailed == TRUE)
        {
            goto STARTPROC_FAILED;
        }
        else
        {
            // 2 如果 检查到 URP evb tlv 协商全部成功
            // 首先确保vsi profile and cmd pool 中存在此信息
            
            itVNicUrp = mapVNicTblURP.begin();
            for(; itVNicUrp != mapVNicTblURP.end(); ++itVNicUrp)
            {                        
                ret = ExecVDPCmd4VSI((*itVNicUrp).second, (*itVNicUrp).first, evb_oper);
                if( ret != 0)
                {
                    VNET_LOG(VNET_LOG_ERROR, "CEvbMgr::StartVPortProc: ExecVDPCmd4VSI failed. \n");                                   
                    goto STARTPROC_FAILED;
                }                 
            }

            
            // 检查vdp vsi 状态信息
            // 只要存在一个失败,则返回失败，其他都需要清除
            // 只要检查到一个处于RUNNING, 则返回RUNNING 
            itVNicUrp = mapVNicTblURP.begin();
            for(; itVNicUrp != mapVNicTblURP.end(); ++itVNicUrp)
            { 
                pVNicDetail = (*itVNicUrp).first;

                FormatVSIID(pVNicDetail->GetVSIIDValue(),vsiid);
                ret = vdp_lookup_vsiState((*itVNicUrp).second,pVNicDetail->GetVSIIDFormat(),vsiid);
                if (VSISTATE_FAILED == ret )
                {
                    EVB_LOG(EVB_LOG_ERROR,"CEvbMgr::StartVPortProc vdp_lookup_vsiState return failed.\n");
                    goto STARTPROC_FAILED;
                }
                
                if (VSISTATE_RUNNING==ret)
                {
                    bVsiRunning = TRUE;
                }
            }

            if(bVsiRunning)
            {
                return START_VPORT_DISCOVERY_RUNNING;
            }
            
            return START_VPORT_DISCOVERY_SUCCESS;            
        }

        return 0;
STARTPROC_FAILED:

        // 释放资源，统一放在stopnetwork来完成 
        return START_VPORT_DISCOVERY_FAILED;
    }

    int32 CEvbMgr::StartVPortProcNoNeedEvbTlv(const int64 &nVMID, map<CVNetVnicDetail*, uint32> &mapVNicTblURP,BYTE option)
    {
        int32 ret = 0;
        
        // 如果vnic为空，直接返回成功
        if( mapVNicTblURP.empty())
        {
            return START_VPORT_DISCOVERY_SUCCESS;
        }
                 
        BYTE evb_oper = VDPTLV_TYPE_ASSOCIATE;  
        map<CVNetVnicDetail*, uint32>::iterator itVNicUrp;

        // 判断evb tlv 状态 

        // option --> evb oper
        switch (option)
        {
            case VNET_VM_DEPLOY:
            {
                evb_oper = VDPTLV_TYPE_ASSOCIATE;
            }
            break;
            case VNET_VM_PREPARE:
            {
                evb_oper = VDPTLV_TYPE_PRE_ASSOCIATE_WITH_RR;
            }
            break;
            case VNET_VM_MIGRATE:
            {
                evb_oper = VDPTLV_TYPE_ASSOCIATE;
            }
            break;
            default:
            {
                VNET_LOG(VNET_LOG_ERROR, "CEvbMgr::StartVPortProcNoNeedEvbTlv: param option(%u) invalid.\n",option);
                return START_VPORT_DISCOVERY_FAILED;
            }
        }

        
        // 是否URP EVB TLV 是否都成功

        // 1 如果 检查到URP evb tlv 协商存在部分失败
        
        // 首先确保vsi profile and cmd pool 中存在此信息
            
        itVNicUrp = mapVNicTblURP.begin();
        for(; itVNicUrp != mapVNicTblURP.end(); ++itVNicUrp)
        {                        
            ret = ExecVDPCmd4VSI((*itVNicUrp).second, (*itVNicUrp).first, evb_oper);
            if( ret != 0)
            {
                VNET_LOG(VNET_LOG_ERROR, "CEvbMgr::StartVPortProcNoNeedEvbTlv: ExecVDPCmd4VSI failed. \n"); 
            }                 
        }
            
        return START_VPORT_DISCOVERY_SUCCESS;
    }

    int32 CEvbMgr::DeleteVsiProfileAndCmdPool(uint32 uiURPNo, CVNetVnicDetail*pNic)
    {
        BYTE vsiid[VDPTLV_VSIID_LEN] = {0};
        BYTE mgrid[VDPTLV_VSIMGRID_LEN] = {0};
        BYTE vsitypeid[VDPTLV_VSITYPEID_LEN]= {0};
        /* BYTE groupid[EVB_VDP_GROUPID_LEN]={0}; */

        if ( NULL == pNic)
        {
            VNET_LOG(VNET_LOG_ERROR, "CEvbMgr::EvbInstallVsi: Param is NULL. \n");
            return -1;
        }

        FormatVSIID(pNic->GetVSIIDValue(),vsiid);
        FormatMgrID(pNic->GetMgrID(),mgrid);
        FormatVSITypeID(pNic->GetVSITypeID(),vsitypeid);

        struct tagVDP_FLT_GROUPMACVID  fltGMV;
        MEMSET(&fltGMV,0, sizeof(fltGMV));

        /* mac地址格式需转换; */
        string strMacAddress;
        if (0 != pNic->GetAssMac().size())
        {
            strMacAddress = pNic->GetAssMac();
        }
        else
        {
            strMacAddress = pNic->GetMacAddress();
        }
        
        if ( 0 != vnet_mac_str_to_byte(strMacAddress,fltGMV.mac) )
        {
            EVB_LOG(EVB_LOG_ERROR,"CEvbMgr::EvbInstallVsi vnet_mac_str_to_byte(%s) failed.\n",strMacAddress.c_str());
            return -1;
        }
     
        // 删除 vsi and cmd pool 
        if( 0 != vdp_delete_vsi_and_cmdPool(uiURPNo,pNic->GetVSIIDFormat(),vsiid) )
        {
            EVB_LOG(EVB_LOG_ERROR,"CEvbMgr::EvbInstallVsi vdp_delete_vsi_and_cmdPool failed.\n");
            return -1;
        }

        return 0;
    }


    int32 CEvbMgr::StartVPortDiscovery(const int64 &nVMID, vector<CVNetVnicDetail> &vecNic,BYTE option, int32 noNeedEvbTlv)
    {
        map<CVNetVnicDetail*, uint32> mapVNicTblURP;    // URP No.<-> VNic table;
        map<CVNetVnicDetail*, uint32>::iterator itrMap;
        int32 nRet = START_VPORT_DISCOVERY_SUCCESS;

        if(VNET_FALSE == m_bEvbEnable)
        {
            VNET_LOG(VNET_LOG_INFO,
                "CEvbMgr::StartVPortDiscovery: EVB is closed.\n");
            return START_VPORT_DISCOVERY_SUCCESS;
        }
         
        if (vecNic.empty())
        {
            EVB_LOG(EVB_LOG_ERROR,"CEvbMgr::StartVPortDiscovery: TRUE == vecNic.empty().\n");
            // EVB_ASSERT(0);
            return START_VPORT_DISCOVERY_SUCCESS;
        }

        // 获取vm 需要evb的nics，后面只需对需要evb的nic进行协商
        vector<CVNetVnicDetail> vecEvbNic;
        if(IsNotNeedEVB(vecNic,vecEvbNic) || vecEvbNic.empty())
        {
            EVB_LOG(EVB_LOG_INFO,"CEvbMgr::StartVPortDiscovery: The VM don't need EVB.\n");
            return START_VPORT_DISCOVERY_SUCCESS;
        }

        // Check param
        if (0 != ChkVMOpt(option))
        {
            EVB_LOG(EVB_LOG_ERROR,"CEvbMgr::StartVPortDiscovery: ChkVMOpt option(%u) is invalid.\n", option);
            return START_VPORT_DISCOVERY_INVALID;
        }

        if (0 != ChkVSIParam(vecEvbNic))
        {
            EVB_LOG(EVB_LOG_ERROR,"CEvbMgr::StartVPortDiscovery: call ChkVSIParam failed.\n");
            return START_VPORT_DISCOVERY_INVALID;
        }

        if (0 != BuildVNicURPMap(vecEvbNic, mapVNicTblURP))
        {
            EVB_LOG(EVB_LOG_ERROR,"CEvbMgr::StartVPortDiscovery: call GetVSIURPMapTbl failed.\n");
            return START_VPORT_DISCOVERY_INVALID;
        }

        if (g_iEvbMgrPrint)
        {
            cout << "evb StartVPortDiscovery " << nVMID << ", option:" << option << endl;
            DbgShowVmNicCfg(vecEvbNic, mapVNicTblURP, option);
        }

        // 考虑EVB PORT 刚添加，虚拟机马上部署过来，此时将不判断EVB TLV dom0 vsi状态
        // 只要存在一个evb port端口刚添加，则整个虚拟机都不需要查看EVB TLV DOM0状态
        int32 nIsExistNewEvbP = CheckIsExistNewEvbPort(mapVNicTblURP);
        if( nIsExistNewEvbP )
        {            
            EVB_LOG(EVB_LOG_INFO,"CEvbMgr::StartVPortDiscovery:(%ll) has new evb port.\n",nVMID); 
        }

        // noNeedEvbTlv = TRUE, 说明无需判断EVB TLV，Dom0 Vsi 状态，直接
        if( noNeedEvbTlv == TRUE || nIsExistNewEvbP == TRUE)
        {
            nRet = StartVPortProcNoNeedEvbTlv(nVMID,mapVNicTblURP,option);
        }
        else
        {
            // Check URP EVB TLV ability
            nRet = StartVPortProc(nVMID,mapVNicTblURP,option);
        }
                
        return nRet;
    }

    int32 CEvbMgr::CheckIsExistNewEvbPort(map<CVNetVnicDetail*, uint32> &mapURPVNic)
    {
        vector<uint32> vecAllURPNo; /* 从map中提取, 避免重复构造urp检查; */
        vector<uint32>::iterator itrVec;

        time_t tNow = time(0);
        
        struct tagEvbPort* evbport = NULL;

        if (mapURPVNic.empty())
        {
            return FALSE;
        }

        if (0 != GetAllURP(mapURPVNic, vecAllURPNo))
        {
            VNET_LOG(VNET_LOG_ERROR, "CEvbMgr::CheckIsExistNewEvbPort: call GetAllURP failed.\n");
            return FALSE;
        }

        for (itrVec = vecAllURPNo.begin(); itrVec != vecAllURPNo.end(); ++itrVec)
        {
            // 1 check 
            evbport = GetEvbPort(*itrVec);
            if( NULL == evbport )
            {
                EVB_LOG(EVB_LOG_ERROR, "CEvbMgr:CheckIsExistNewEvbPort GetEvbPort(port:%u)port is invalid.\n",(*itrVec));
                return FALSE;
            }

            if( (tNow - evbport->tCreateTimeStamp) <= NEW_EVBPORT_DELAY_TIME)
            {
                return TRUE;
            }
        }

        return FALSE;
    }

    /* stopnetwork evb 流程
        确保将VSI 删除即可        
    */
    int32 CEvbMgr::StopVPortProc(const int64 &nVMID, map<CVNetVnicDetail*, uint32> &mapVNicTblURP,BYTE option)
    {
        // 如果vnic为空，直接返回成功
        if( mapVNicTblURP.empty())
        {
            return START_VPORT_DISCOVERY_SUCCESS;
        }

        // 需要将 vsi profile and cmd pool删除 
        map<CVNetVnicDetail*, uint32>::iterator itVNicUrp = mapVNicTblURP.begin();
        for(; itVNicUrp != mapVNicTblURP.end(); ++itVNicUrp)
        {
            // DELETE vsi_profile cmd_pool 
            if( 0 != DeleteVsiProfileAndCmdPool((*itVNicUrp).second,(*itVNicUrp).first) )
            {
                VNET_LOG(VNET_LOG_INFO, "CEvbMgr::StopVPortProc: DeleteVsiProfileAndCmdPool failed. \n");                                   
            }
        }
        return START_VPORT_DISCOVERY_SUCCESS;
    }

    int32 CEvbMgr::StopVPortDiscovery(const int64 &nVMID, vector<CVNetVnicDetail> &vecNic,BYTE option)
    {
        map<CVNetVnicDetail*, uint32> mapVNicTblURP;    // URP No.<-> VNic table;
        map<CVNetVnicDetail*, uint32>::iterator itrMap;
        int32 nRet = STOP_VPORT_DISCOVERY_SUCCESS;

        if(VNET_FALSE == m_bEvbEnable)
        {
            VNET_LOG(VNET_LOG_INFO,
                "CEvbMgr::StopVPortDiscovery: EVB is closed.\n");
            return STOP_VPORT_DISCOVERY_SUCCESS;
        }

        if (vecNic.empty())
        {
            EVB_LOG(EVB_LOG_ERROR,"CEvbMgr::StopVPortDiscovery: TRUE == vecNic.empty().\n");
            return STOP_VPORT_DISCOVERY_SUCCESS;
        }

        // 获取vm 需要evb的nics，后面只需对需要evb的nic进行协商
        vector<CVNetVnicDetail> vecEvbNic;
        if(IsNotNeedEVB(vecNic,vecEvbNic) || vecEvbNic.empty())
        {
            EVB_LOG(EVB_LOG_INFO,"CEvbMgr::StopVPortDiscovery: The VM don't need EVB.\n");
            return STOP_VPORT_DISCOVERY_SUCCESS;
        }

        // Check param
        if (0 != ChkVMOpt(option))
        {
            EVB_LOG(EVB_LOG_ERROR,"CEvbMgr::StopVPortDiscovery: ChkVMOpt option(%u) is invalid.\n", option);
            return STOP_VPORT_DISCOVERY_INVALID;
        }

        if (0 != ChkVSIParam(vecEvbNic))
        {
            EVB_LOG(EVB_LOG_ERROR,"CEvbMgr::StopVPortDiscovery: call ChkVSIParam failed.\n");
            return STOP_VPORT_DISCOVERY_INVALID;
        }

        if (0 != BuildVNicURPMap(vecEvbNic, mapVNicTblURP))
        {
            EVB_LOG(EVB_LOG_ERROR,"CEvbMgr::StopVPortDiscovery: call GetVSIURPMapTbl failed.\n");
            return STOP_VPORT_DISCOVERY_INVALID;
        }

        if (g_iEvbMgrPrint)
        {
            cout << "evb StopVPortDiscovery " << nVMID << ", option:" << option << endl;
            DbgShowVmNicCfg(vecEvbNic, mapVNicTblURP, option);
        }

        // Check URP EVB ability
        nRet = StopVPortProc(nVMID,mapVNicTblURP,option);

        return STOP_VPORT_DISCOVERY_SUCCESS;
    }


    void CEvbMgr::DbgShowISSPort(void)
    {
        // Show EVB Enable
        cout<< "--------------------------------------------------------------------------" << endl;
        
        cout << "m_bEvbEnable value: " << m_bEvbEnable <<"." << endl;
        if(VNET_FALSE == m_bEvbEnable)
        {
            cout << "EVB is closed." <<endl;
        }
        else
        {
            cout << "EVB is opened." <<endl;
            // Show HC_GetAllPhyPort的iss port
            cout << "EVB ISS port from HC_GetAllPhyPort()" << endl;
            map<string, uint32>::iterator itrMap = m_mapISSPort.begin();
            for (; itrMap != m_mapISSPort.end(); ++itrMap)
            {
                cout << "Name: " << itrMap->first << " "
                << "No: " << itrMap->second << endl;
            }
        }    
        cout<< "--------------------------------------------------------------------------" << endl;

        return ;
    }

    void CEvbMgr::DbgShowVmNicCfg(vector<CVNetVnicDetail> &vecNic,
                                  map<CVNetVnicDetail*,uint32 > &mapURPVNic,
                                  BYTE option)
    {
        cout << "VM option : " << (uint32)option << endl;
        vector<CVNetVnicDetail>::iterator it_vNic =  vecNic.begin();
        cout << "-----vector<VmNicConfig>-----------" << endl;
        for (; it_vNic != vecNic.end(); ++it_vNic)
        {
            cout << "vsiidformat : " <<(*it_vNic).GetVSIIDFormat() << endl;
            cout << "vsiid       : " << (*it_vNic).GetVSIIDValue() << endl;
        }

        cout << "-----map<string, uint32> mapVSIURP----" << endl;
        map<CVNetVnicDetail*,uint32 >::iterator itrMap = mapURPVNic.begin();
        for (; itrMap != mapURPVNic.end(); ++itrMap)
        {
            CVNetVnicDetail *pVNic = itrMap->first;
            cout << "URP No: " << itrMap->second << ". ";
            if (pVNic)
            {
                cout << "vssid : " << pVNic->GetVSIIDValue() << endl;
            }
            else
            {
                cout << "vssid : error" << endl;
            }
        }

    }

    C20msMsgTimer *C20msMsgTimer::s_pInstance = NULL;
    C20msMsgTimer::C20msMsgTimer()
    {
        m_mu = NULL;
    };

    C20msMsgTimer::~C20msMsgTimer()
    {
        if (INVALID_TIMER_ID != m_t20msTimerID)
        {
            m_mu->KillTimer(m_t20msTimerID);
            m_t20msTimerID = INVALID_TIMER_ID;
        }

        delete s_pInstance;
        s_pInstance = NULL;
        delete m_mu;
        m_mu = NULL;
    }
    STATUS C20msMsgTimer::StartMu(const string& name)
    {
        if(m_mu)
        {
            SkyAssert(false);
            return ERROR_DUPLICATED_OP;
        }
        
        //创建一个MessageQueue实例
        m_mu = new MessageQueue(name);
        if (!m_mu)
        {
            VNET_LOG(VNET_LOG_ERROR, "Create mu %s failed!\n",name.c_str());
            return ERROR_NO_MEMORY;
        }
        
        //将MyHandler注册为该MessageQueue的消息处理器
        STATUS ret = m_mu->SetHandler(this);
        if (SUCCESS != ret)
        {
            VNET_LOG(VNET_LOG_ERROR, "SetHandler mu %s failed! ret = %d\n",name.c_str(),ret);
            return ret;
        }
        
        //运行MessageQueue
        ret = m_mu->Run();
        if (SUCCESS != ret)
        {
            VNET_LOG(VNET_LOG_ERROR, "Run mu %s failed! ret = %d\n",name.c_str(),ret);
            return ret;
        }
        
        ret = m_mu->Register();
        if (SUCCESS != ret)
        {
            VNET_LOG(VNET_LOG_ERROR, "Register mu %s failed! ret = %d\n",name.c_str(),ret);
            return ret;
        }
        
        //给自己发送上电消息，促使消息单元状态切换到工作态
        MessageFrame message(SkyInt32(0), EV_POWER_ON);
        ret = m_mu->Receive(message);
        if (SUCCESS != ret)
        {
            SkyAssert(0);
            return ret;
        }
        m_mu->Print();
        
        return SUCCESS;
    }

    int32 C20msMsgTimer::InitTimer(void)
    {
        m_t20msTimerID = m_mu->CreateTimer();
        if (INVALID_TIMER_ID == m_t20msTimerID)
        {
            VNET_LOG(VNET_LOG_ERROR, "C20msMsgTimer::InitTimer: call CreateTimer failed.\n");
            return -1;
        }

        return 0;
    }

    int32 C20msMsgTimer::StartTimer(void)
    {
        if (INVALID_TIMER_ID == m_t20msTimerID)
        {
            VNET_LOG(VNET_LOG_ERROR, "C20msMsgTimer::StartTimer: INVALID_TIMER_ID == m_t20msTimerID.\n");
            return -1;
        }
        
        TimeOut timeout;
        timeout.type = LOOP_TIMER;
        timeout.duration = 20;
        timeout.msgid = EV_VNET_20MS_TIMER;
        timeout.arg = 0;
        
        if (SUCCESS != m_mu->SetTimer(m_t20msTimerID, timeout))
        {
            VNET_LOG(VNET_LOG_ERROR, "C20msMsgTimer::StartTimer: SetTimer(m_t20msTimerID) failed.\n");
            return -1;
        }

        return 0;
    }

    int32 C20msMsgTimer::StopTimer(void)
    {
        if (INVALID_TIMER_ID == m_t20msTimerID)
        {
            VNET_LOG(VNET_LOG_ERROR, "C20msMsgTimer::StopTimer: INVALID_TIMER_ID == m_t20msTimerID.\n");
            return -1;
        }    
      
        if (SUCCESS != m_mu->StopTimer(m_t20msTimerID))
        {
            VNET_LOG(VNET_LOG_ERROR, "C20msMsgTimer::StopTimer: StopTimer(m_t20msTimerID) failed.\n");
            return -1;
        }
        
        return 0;
    }
        
    void C20msMsgTimer::MessageEntry(const MessageFrame& message)
    {

        switch (m_mu->get_state())
        {
            case S_Startup:
            {
                switch (message.option.id())
                {
                    case EV_POWER_ON:
                    {
                        if(0 != InitTimer())
                        {
                            SkyAssert(false);
                            SkyExit(-1,"C20msMsgTimer:: call InitTimer failed!\n");
                        }
                        m_mu->set_state(S_Work);
                        OutPut(SYS_NOTICE, "%s power on!\n",m_mu->name().c_str());
                        VNET_LOG(VNET_LOG_INFO,
                                 "C20msMsgTimer::MessageEntry: C20msMsgTimer: entering WORK!!!\n");
                    }
                    break;
                    
                    default:
                    {
                        break;
                    }
                }
                break;
            }

            case S_Work:
            {                    
                switch (message.option.id())
                {
                    case EV_VNET_20MS_TIMER:
                    {                        
                        Evb20msTimerLoop();
                        break;
                    }
                }

                break;
            }

            default:
            {
                break;
            }
        } // end switch (m_mu
    }

    void evb_setissport(const char* portName, int32 setFlag)
    {
        /* 6、创建evb port */
        CEvbMgr *pEvbMgr = CEvbMgr::GetInstance();
        if(NULL == pEvbMgr)
        {
            VNET_LOG(VNET_LOG_ERROR, "CSoftDVSDev::CreateSwitch: CEvbMgr::GetInstance() is NULL.\n");
            return ;
        }

        string sPortName = portName;
        if( setFlag )
        {        
            if(0 != pEvbMgr->SetEvbIssPort(sPortName))
            {
                VNET_LOG(VNET_LOG_ERROR, "evb_setissport: SetEvbIssPort<%s> failed.\n", sPortName.c_str());
                return ;
            }
            cout << "success"<<endl;
        }
        else
        {
            if(0 != pEvbMgr->UnsetEvbIssPort(sPortName))
            {
                VNET_LOG(VNET_LOG_ERROR, "evb_setissport: UnSetEvbIssPort<%s> failed.\n", sPortName.c_str());
                return;
            }
            cout << "success"<<endl;

        }
    }
    DEFINE_DEBUG_FUNC(evb_setissport);

    void evb_setportevbmod(const char* portName, int32 evbMode)
    {
        CEvbMgr *pEvbMgr = CEvbMgr::GetInstance();
        if(NULL == pEvbMgr)
        {
            VNET_LOG(VNET_LOG_ERROR, "CSoftDVSDev::DoSwitchModify: CEvbMgr::GetInstance() is NULL.\n");
            return;
        }
        string sPortName = portName;
        if(0 != pEvbMgr->SetEvbMode(sPortName, evbMode))
        {
            VNET_LOG(VNET_LOG_ERROR, "CSoftDVSDev::DoSwitchModify: SetEvbMode<%s, %d> failed.\n", sPortName.c_str(), evbMode);
            return;
        }
        cout << "success"<<endl;
    }  
    DEFINE_DEBUG_FUNC(evb_setportevbmod);

    extern "C"   void evbtlv_dbgshowregister(void);
    extern  "C"  void evbtlv_dbgshowport(u32 local_port);
    extern  "C"  void evbtlv_dbgshowbridge(u32 local_port);
    extern  "C"  void evbtlv_dbgshowstation(u32 local_port);
    DEFINE_DEBUG_FUNC(evbtlv_dbgshowregister);
    DEFINE_DEBUG_FUNC(evbtlv_dbgshowport);
    DEFINE_DEBUG_FUNC(evbtlv_dbgshowbridge);
    DEFINE_DEBUG_FUNC(evbtlv_dbgshowstation);
    DEFINE_DEBUG_FUNC(evbtlv_setprint);


    DEFINE_DEBUG_FUNC(DbgAccTimerTestStart);
    DEFINE_DEBUG_FUNC(DbgAccTimerTestStop);

    DEFINE_DEBUG_FUNC(DbgShowCdcpConf);
    DEFINE_DEBUG_FUNC(DbgOpenSndCdcpPktPrint);
    DEFINE_DEBUG_FUNC(DbgCloseSndCdcpPktPrint);
    DEFINE_DEBUG_FUNC(DbgOpenRcvCdcpPktPrint);
    DEFINE_DEBUG_FUNC(DbgCloseRcvCdcpPktPrint);

    DEFINE_DEBUG_FUNC(DbgSetStationCdcpData);
    DEFINE_DEBUG_FUNC(DbgSetBridgeCdcpData);

    DEFINE_DEBUG_FUNC(DbgShowEvbSysState);
    DEFINE_DEBUG_FUNC(DbgSetMsgPrint);

    DEFINE_DEBUG_FUNC(test_set_station);
    DEFINE_DEBUG_FUNC(test_set_bridge);

    DEFINE_DEBUG_FUNC(lldp_show_lossless);
    DEFINE_DEBUG_FUNC(lldp_show_info);
    DEFINE_DEBUG_FUNC(lldp_show_port);
    DEFINE_DEBUG_FUNC(lldp_set_print);
    

    DEFINE_DEBUG_FUNC(DbgShowEvbPortInfo);
    DEFINE_DEBUG_FUNC(DbgShowEvbPortPktStt);
    DEFINE_DEBUG_FUNC(DbgMemShow);
    DEFINE_DEBUG_FUNC(DbgShowEvbTimer);
    DEFINE_DEBUG_FUNC(ShowEcpInfo);
    DEFINE_DEBUG_FUNC(ShowEcpTxListInfo);
    DEFINE_DEBUG_FUNC(ShowEcpRxListInfo);
    //DEFINE_DEBUG_FUNC(t_ecp_init);
    //DEFINE_DEBUG_FUNC(t_ecp_tx_sm);
    //DEFINE_DEBUG_FUNC(t_ecp_list_tx_insert);
    DEFINE_DEBUG_FUNC(ecp_print);
    DEFINE_DEBUG_FUNC(vdp_print);
    DEFINE_DEBUG_FUNC(ShowVdpInfo);
    //DEFINE_DEBUG_FUNC(t_vdp_init);
    //DEFINE_DEBUG_FUNC(t_cfg_SB);
    DEFINE_DEBUG_FUNC(evbcfg_show);
    //DEFINE_DEBUG_FUNC(t_vsiProfile_init);
    //DEFINE_DEBUG_FUNC(t_vsiProfile_AddDelflt);
    DEFINE_DEBUG_FUNC(DbgShowVsiProfile);
    DEFINE_DEBUG_FUNC(DbgShowVsiOutFlt);
    //DEFINE_DEBUG_FUNC(t_byteorder);
    //DEFINE_DEBUG_FUNC(t_newCmd);
    DEFINE_DEBUG_FUNC(DbgShowCmdPool);
    DEFINE_DEBUG_FUNC(DbgEcpPacket);
    DEFINE_DEBUG_FUNC(DbgEcpEvent);
    DEFINE_DEBUG_FUNC(DbgVdpPacket);
    DEFINE_DEBUG_FUNC(DbgVdpEvent);
    DEFINE_DEBUG_FUNC(DbgVdpCmdPool);
    DEFINE_DEBUG_FUNC(DbgVdpProfile);
	
    DEFINE_DEBUG_FUNC(vdp_GetEvbTlvNegResult);
/*
    void t_evbrsock(unsigned int port)
    {
        DbgShowEvbRawSocket(port);
    }
    DEFINE_DEBUG_FUNC(t_evbrsock);
*/  
    DEFINE_DEBUG_FUNC(DbgShowEvbRawSocket);

    DEFINE_DEBUG_FUNC(test_del_vsi);
}


