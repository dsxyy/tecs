/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�dev_mon_reg_func.cpp
* �ļ���ʶ��
* ����ժҪ�������ش�������ʵ���ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�
* ������ڣ�2013��1��16��
*
* �޸ļ�¼1��
*     �޸����ڣ�2013/1/16
*     �� �� �ţ�V1.0
*     �� �� �ˣ�lverchun
*     �޸����ݣ�����
******************************************************************************/
#include <typeinfo>
#include "vna_common.h"
#include "dev_monitor.h"
#include "dev_mgr.h"
#include "vna_vnic_pool.h"
#include "vnet_phyport_class.h"
#include "vnet_trunkport_class.h"
#include "vna_switch_module.h"
//#include "vnetlib_api.h"
#include "vnet_function.h"
#include "dev_mon_reg_func.h"
#include "vnet_phyport_manager.h"
#include "vnet_function.h"
#include "vnet_vnic.h"
#include "vna_switch_module.h"
#include "vna_switch_agent.h"
#include "vna_vnic_pool.h"

namespace zte_tecs
{

extern int32 g_nHypeVisorType;

//�����������豸MTU���Դ�����
TDevMonFuncReg s_MonMTUFuncReg[] = 
{ 
    {TYPE_INFO(CPhyPortClass), CMonitorMTUPhyPort},
    {TYPE_INFO(CTrunkPortClass), CMonitorMTUBondPort},
    {TYPE_INFO(CVnicPortDev), CMonitorMTUVnicPort},
    {TYPE_INFO(CSoftDVSDev), CMonitorMTUSDVS},
    {TYPE_INFO(CEmbDVSDev), CMonitorMTUEDVS},
};

//���������ڵ����ҽ��������Դ�����
TPortMonFuncReg s_MonBridgeFuncReg[] = 
{ 
    {TYPE_INFO(CPhyPortClass), CMonitorBridgePhyPort},
    {TYPE_INFO(CTrunkPortClass), CMonitorBridgeBondPort},
    {TYPE_INFO(CVnicPortDev), CMonitorBridgeVnicPort},
};

//�����������豸Loop���Դ�����
TPortMonFuncReg s_MonLoopFuncReg[] = 
{ 
    {TYPE_INFO(CPhyPortClass), CMonitorLoopPhyPort},
    {TYPE_INFO(CTrunkPortClass), CMonitorLoopBondPort},
};

//�������������ж˿����Դ�����
TDVSMonFuncReg s_MonUpLinkFuncReg[] = 
{ 
    {TYPE_INFO(CSoftDVSDev), CMonitorUpLinkSDVS},
    {TYPE_INFO(CEmbDVSDev), CMonitorUpLinkEDVS},
};

//������VNIC�˿����Դ�����
TVnicMonFuncReg s_MonVnicFuncReg[] = 
{ 
    {TYPE_INFO(CVnicPortDev), CMonitorVnic},
};

TDevMonFuncReg *GetMonMTUFuncReg(int32 &nSize)
{
    nSize = sizeof(s_MonMTUFuncReg)/sizeof(TDevMonFuncReg);
    return s_MonMTUFuncReg;
};

TPortMonFuncReg *GetMonBridgeFuncReg(int32 &nSize)
{
    nSize = sizeof(s_MonBridgeFuncReg)/sizeof(TPortMonFuncReg);
    return s_MonBridgeFuncReg;
};

TPortMonFuncReg *GetMonLoopFuncReg(int32 &nSize)
{
    nSize = sizeof(s_MonLoopFuncReg)/sizeof(TPortMonFuncReg);
    return s_MonLoopFuncReg;
};

TDVSMonFuncReg *GetMonUpLinkFuncReg(int32 &nSize)
{
    nSize = sizeof(s_MonUpLinkFuncReg)/sizeof(TDVSMonFuncReg);
    return s_MonUpLinkFuncReg;
};

TVnicMonFuncReg *GetMonVnicFuncReg(int32 &nSize)
{
    nSize = sizeof(s_MonVnicFuncReg)/sizeof(TVnicMonFuncReg);
    return s_MonVnicFuncReg;
};

void CMonitorMTUPhyPort(CNetDev *dev_entity)
{
    //cout <<"-----------------CMonitorMTUPhyPort" <<dev_entity->GetDevName() <<endl;


    if (0 != VNetSetMtu(dev_entity->GetDevName().c_str(), dev_entity->GetDevMTU())) //����������mtu
    {
        cout << "call set_mtu(" << dev_entity->GetDevName() <<", " << dev_entity->GetDevMTU() << ") failed" <<endl;
    }

};

void CMonitorMTUBondPort(CNetDev *dev_entity)
{
    //cout <<"-----------------CMonitorMTUBondPort" <<dev_entity->GetDevName() <<endl;

    if (0 != VNetSetMtu(dev_entity->GetDevName().c_str(), dev_entity->GetDevMTU())) //����������mtu
    {
        cout << "call set_mtu(" << dev_entity->GetDevName() <<", " << dev_entity->GetDevMTU() << ") failed" <<endl;
    }
};

void CMonitorMTUVnicPort(CNetDev *dev_entity)
{
    //cout <<"-----------------CMonitorMTUVnicPort" <<dev_entity->GetDevName() <<endl;

    if (0 != set_mtu(dev_entity->GetDevName().c_str(), dev_entity->GetDevMTU())) //����������mtu
    {
        cout << "call set_mtu(" << dev_entity->GetDevName() <<", " << dev_entity->GetDevMTU() << ") failed" <<endl;
    }
};

void CMonitorMTUSDVS(CNetDev *dev_entity)
{
    //cout <<"-----------------CMonitorMTUSDVS" <<dev_entity->GetDevName() <<endl;
#if 0
    vector<string> vecPort;
    dev_entity->GetDVSUplinkPort(vecPort);

    int32 nMTU = dev_entity->GetDevMTU();

    vector<string>::iterator itr = vecPort.begin();
    for( ; itr != vecPort.end(); ++itr)
    {
        if(0 != VNetSetMtu(itr->c_str(), nMTU)) //����������mtu
        {
            cout << "call set_mtu(" << *itr <<", " << nMTU << ") failed" <<endl;
        }
    }  
#endif
};

void CMonitorMTUEDVS(CNetDev *dev_entity)
{
    //cout <<"-----------------CMonitorMTUEDVS" <<dev_entity->GetDevName() <<endl;
};

void CMonitorLoopPhyPort(CPortNetDev *dev_entity)
{
    //cout <<"-----------------CMonitorLoopPhyPort" <<dev_entity->GetDevName() <<endl;
};

void CMonitorLoopBondPort(CPortNetDev *dev_entity)
{
    //cout <<"-----------------CMonitorLoopBondPort" <<dev_entity->GetDevName() <<endl;
};

void CMonitorBridgePhyPort(CPortNetDev *dev_entity)
{
    //cout <<"-----------------CMonBridgePhyPort" <<dev_entity->GetDevName() <<endl;
};

void CMonitorBridgeBondPort(CPortNetDev *dev_entity)
{
    //cout <<"-----------------CMonBridgeBondPort" <<dev_entity->GetDevName() <<endl;
};

void CMonitorBridgeVnicPort(CPortNetDev *dev_entity)
{
    //cout <<"-----------------CMonitorBridgeVnicPort" <<dev_entity->GetDevName() <<endl;
};

void CMonitorUpLinkSDVS(CSwitchDev *dev_entity)
{
    //cout <<"-----------------CMonUpLinkSDVS" <<dev_entity->GetDevName() <<endl;

        vector<string> vecPort;
    dev_entity->GetDVSUplinkPort(vecPort);

    int32 nMTU = dev_entity->GetDevMTU();

    vector<string>::iterator itr = vecPort.begin();
    for( ; itr != vecPort.end(); ++itr)
    {
        if(0 != VNetSetMtu(itr->c_str(), nMTU)) //����������mtu
        {
            cout << "call set_mtu(" << *itr <<", " << nMTU << ") failed" <<endl;
        }
    }  
};

void CMonitorUpLinkEDVS(CSwitchDev *dev_entity)
{
    //cout <<"-----------------CMonUpLinkEDVS" <<dev_entity->GetDevName() <<endl;
};


    
//����vepa�������������
void CheckVepaFlowOfPort(CVnicPortDev *dev_entity,int32 op_flag)
{
    int iRet = 0;
    uint32 nNativeVlan = 1;      //���п�native vlan
    uint32 nTagVlan = 0;         //����ڱ��ĳ�����Ҫ���vlan
    vector<string> vecPortList;
    vector<string> vecVlanPortList;

    string strBridgeName = dev_entity->GetBridge();

    //�������EVBģʽ
    CSwitchAgent * pSwitchAgent = CSwitchAgent::GetInstance();
    CSwitchDev * pSwitchDev = pSwitchAgent->GetSwitchDev(strBridgeName);
    if (pSwitchDev == NULL || pSwitchDev->GetDVSEvbMode()!= EN_EVB_MODE_VEPA)
    {
        return;
    }
    
    string strUpPort = pSwitchDev->GetDVSInterface();

    if (VNET_PLUGIN_SUCCESS != VNetGetVlanAccess(strUpPort, nNativeVlan))
    {
        return;
    }    

    if (nNativeVlan == (uint32)dev_entity->GetCVlan())
    {
        nTagVlan = 0;
    }
    else
    {
        nTagVlan = (uint32)dev_entity->GetCVlan();
    }    
    

    //�Ȼ�ȡ�����������ж˿ڼ���openflow port id
    map<string, uint32> mapPortIdList;
    iRet = VNetGetAllOfPortIdOfBridge(strBridgeName, mapPortIdList);
    if (VNET_PLUGIN_SUCCESS != iRet)
    {
        return;
    }
    
    string strSelfVifName = dev_entity->GetVifName();
    string strSelfTapName = dev_entity->GetTapName();
    string strSelfEmuName = dev_entity->GetEmuName();
    
    string strSelfMacAddr;
    int32 nUpPortId = 0;
    
    //��ȡ����������vlan��ͬ�Ķ˿�
    iRet = VNetGetIfListOfSameVlan(dev_entity->GetCVlan(),vecVlanPortList);
    if (VNET_PLUGIN_SUCCESS != iRet)
    {
        return;
    }       

    map<string, TPortInfo> mapPortInfoList;

    //��ñ�������vlan��ͬ�Ķ˿�
    map<string, uint32>::iterator it = mapPortIdList.begin();
    for (it = mapPortIdList.begin(); it != mapPortIdList.end(); )
    {
        //uplik port
        if (strUpPort == it->first)
        {
            nUpPortId = it->second;
            mapPortIdList.erase(it++);
            continue;
        }

        //ɾ��vlan��ͬ�ӿ�
        vector<string>::iterator pos;
        pos = find(vecVlanPortList.begin(),vecVlanPortList.end(), it->first);
        if (pos == vecVlanPortList.end()) 
        {
            mapPortIdList.erase(it++);
            continue;
        }
        
        //���tap��vif���У�����Ҫ�޳���tap��Ӧ��vif�ڣ�ֻ����tap��
        if (it->first.find("tap") != string::npos)
        {
            string strVifName = it->first;
            strVifName.replace(0,3,"vif");
            //����Ƿ��ж�Ӧvif�ӿ�
            map<string, uint32>::iterator vif_id_pos = mapPortIdList.find(strVifName);
            if (vif_id_pos != mapPortIdList.end())
            {
                map<string, TPortInfo>::iterator vif_info_pos = mapPortInfoList.find(strVifName);
                if (vif_info_pos != mapPortInfoList.end())//����
                {
                    vif_info_pos->second.m_strTapPort = it->first;
                    vif_info_pos->second.m_nTapPortOfId = it->second;
                }
                else//��û���
                {
                    TPortInfo tPortInfo;
                    tPortInfo.m_strVifPort = vif_id_pos->first;
                    tPortInfo.m_nVifPortOfId = vif_id_pos->second;
                    tPortInfo.m_strTapPort = it->first;
                    tPortInfo.m_nTapPortOfId = it->second;

                    mapPortInfoList.insert(pair<string, TPortInfo> (vif_id_pos->first, tPortInfo));
                }
            }
            else//û��
            {
                TPortInfo tPortInfo;
                tPortInfo.m_strTapPort = it->first;
                tPortInfo.m_nTapPortOfId = it->second;
                
                mapPortInfoList.insert(pair<string, TPortInfo> (it->first, tPortInfo));
            }
        }
        else //
        {
            map<string, TPortInfo>::iterator vif_info_pos = mapPortInfoList.find(it->first);
            if (vif_info_pos == mapPortInfoList.end())//��û���
            {
                TPortInfo tPortInfo;
                tPortInfo.m_strVifPort = it->first;
                tPortInfo.m_nVifPortOfId = it->second;

                mapPortInfoList.insert(pair<string, TPortInfo> (it->first, tPortInfo));
            }
        
        }

        //���emu��vif���У�����Ҫ�޳���emu��Ӧ��vif�ڣ�ֻ����vif-emu��
        if (it->first.find("emu") != string::npos)
        {
            string strVifName = it->first;
            strVifName = strVifName.substr(0, (strVifName.length() - 4));
            //����Ƿ��ж�Ӧvif�ӿ�
            map<string, uint32>::iterator vif_id_pos = mapPortIdList.find(strVifName);
            if (vif_id_pos != mapPortIdList.end())
            {
                map<string, TPortInfo>::iterator vif_info_pos = mapPortInfoList.find(strVifName);
                if (vif_info_pos != mapPortInfoList.end())//����
                {
                    vif_info_pos->second.m_strEmuPort = it->first;
                    vif_info_pos->second.m_nEmuPortOfId = it->second;
                }
                else//��û���
                {
                    TPortInfo tPortInfo;
                    tPortInfo.m_strVifPort = vif_id_pos->first;
                    tPortInfo.m_nVifPortOfId = vif_id_pos->second;
                    tPortInfo.m_strEmuPort = it->first;
                    tPortInfo.m_nEmuPortOfId = it->second;

                    mapPortInfoList.insert(pair<string, TPortInfo> (vif_id_pos->first, tPortInfo));
                }
            }
            else//û��
            {
                TPortInfo tPortInfo;
                tPortInfo.m_strEmuPort = it->first;
                tPortInfo.m_nEmuPortOfId = it->second;
                
                mapPortInfoList.insert(pair<string, TPortInfo> (it->first, tPortInfo));
            }
        }
        else //
        {
            map<string, TPortInfo>::iterator vif_info_pos = mapPortInfoList.find(it->first);
            if (vif_info_pos == mapPortInfoList.end())//��û���
            {
                TPortInfo tPortInfo;
                tPortInfo.m_strVifPort = it->first;
                tPortInfo.m_nVifPortOfId = it->second;

                mapPortInfoList.insert(pair<string, TPortInfo> (it->first, tPortInfo));
            }
        
        }        

        ++it;
    }


    CVNAVnicPool * pVnicPool = CVNAVnicPool::GetInstance();

    map<string, TPortInfo>::iterator it_info = mapPortInfoList.begin();
    for (; it_info != mapPortInfoList.end(); it_info++)
    {
        //��ѯvif�ӿ�mac��ַ
        pVnicPool->GetMacFromPortName(it_info->first, it_info->second);
    }


    //��ѯ�Լ�vif�ӿ�mac��ַ
    TPortInfo tPortInfo1;
    pVnicPool->GetMacFromPortName(strSelfVifName, tPortInfo1);
    strSelfMacAddr = tPortInfo1.m_strMacAddr;

    //��������
    if (0 == op_flag)
    { 
        //��������
        map<string, TPortInfo>::iterator itPortInfo = mapPortInfoList.begin();
        for (; itPortInfo != mapPortInfoList.end(); itPortInfo++)
        {
            stringstream strVmPortList;
            //������������
            if (0 != itPortInfo->second.m_nVifPortOfId)//vif
            {   
                strVmPortList << itPortInfo->second.m_nVifPortOfId;
                VNetSetEvbUplinkFlow(strBridgeName, itPortInfo->second.m_strMacAddr, itPortInfo->second.m_nVifPortOfId, nUpPortId, nTagVlan);
                
            }

            if (0 != itPortInfo->second.m_nTapPortOfId)//tap
            {
                strVmPortList << ","<< itPortInfo->second.m_nTapPortOfId;
                VNetSetEvbUplinkFlow(strBridgeName, itPortInfo->second.m_strMacAddr, itPortInfo->second.m_nTapPortOfId, nUpPortId, nTagVlan);
            }

            if (0 != itPortInfo->second.m_nEmuPortOfId)//vif-emu
            {
                strVmPortList << ","<< itPortInfo->second.m_nEmuPortOfId;
                VNetSetEvbUplinkFlow(strBridgeName, itPortInfo->second.m_strMacAddr, itPortInfo->second.m_nEmuPortOfId, nUpPortId, nTagVlan);
            }            
	
            //������������,�����������㲥
            stringstream strPortIdList;
            map<string, TPortInfo>::iterator itSet = mapPortInfoList.begin();
            for (; itSet != mapPortInfoList.end(); itSet++)
            {
                if (itSet->first != itPortInfo->first)
                {
                    if (0 != itSet->second.m_nVifPortOfId)//vif
                    {
                        if (strPortIdList.str() != "")
                        {
                            strPortIdList << ",";

                        }

                        strPortIdList  << itSet->second.m_nVifPortOfId;
                    }

                    if (0 != itSet->second.m_nTapPortOfId)//tap
                    {
                        if (strPortIdList.str() != "")
                        {
                            strPortIdList << ",";

                        }

                        strPortIdList  << itSet->second.m_nTapPortOfId;
                    }

                    if (0 != itSet->second.m_nEmuPortOfId)//vif-emu
                    {
                        if (strPortIdList.str() != "")
                        {
                            strPortIdList << ",";

                        }

                        strPortIdList  << itSet->second.m_nEmuPortOfId;
                    }                    
                }
            }
            
            //������������
            VNetSetEvbDownlinkFlow(strBridgeName, strVmPortList.str(), itPortInfo->second.m_strMacAddr,strPortIdList.str(), nUpPortId);
        }

    }
    //ɾ������
    else
    {
        VNetDelEvbUplinkFlow(strBridgeName, strSelfMacAddr);
        VNetDelEvbDownlinkFlow(strBridgeName,strSelfMacAddr);

        //��������
        map<string, TPortInfo>::iterator itPortInfo = mapPortInfoList.begin();
        for (; itPortInfo != mapPortInfoList.end(); itPortInfo++)
        {
            //�޳��Լ�
            if ((strSelfVifName == itPortInfo->first) || (strSelfTapName == itPortInfo->first) || (strSelfEmuName == itPortInfo->first))
            {
                continue;
            }

            stringstream strVmPortList;
            if (0 != itPortInfo->second.m_nVifPortOfId)//vif
            {   
                strVmPortList << itPortInfo->second.m_nVifPortOfId;
            }

            if (0 != itPortInfo->second.m_nTapPortOfId)//tap
            {
                strVmPortList << ","<< itPortInfo->second.m_nTapPortOfId;
            }

            if (0 != itPortInfo->second.m_nEmuPortOfId)//vif-emu
            {
                strVmPortList << ","<< itPortInfo->second.m_nEmuPortOfId;
            }            
            
            //������������,�����������㲥
            stringstream strPortIdList;
            map<string, TPortInfo>::iterator itSet = mapPortInfoList.begin();
            for (; itSet != mapPortInfoList.end(); itSet++)
            {
                //�޳��Լ�
                if ((strSelfVifName == itSet->first) || (strSelfTapName == itSet->first) || (strSelfEmuName == itSet->first))
                {
                    continue;
                }
                
                if (itSet->first != itPortInfo->first)
                {
                    if (0 != itSet->second.m_nVifPortOfId)//vif
                    {
                        if (strPortIdList.str() != "")
                        {
                            strPortIdList << ",";
                        }
                                        
                        strPortIdList  << itSet->second.m_nVifPortOfId;
                    }

                    if (0 != itSet->second.m_nTapPortOfId)//tap
                    {
                        if (strPortIdList.str() != "")
                        {
                            strPortIdList << ",";
                        }
                                        
                        strPortIdList  << itSet->second.m_nTapPortOfId;
                    }

                    if (0 != itSet->second.m_nEmuPortOfId)//vif-emu
                    {
                        if (strPortIdList.str() != "")
                        {
                            strPortIdList << ",";
                        }
                                        
                        strPortIdList  << itSet->second.m_nEmuPortOfId;
                    } 
                    
                }
            }
            
            //��������
            //if (strPortIdList.str() != "")
            //{
                VNetSetEvbDownlinkFlow(strBridgeName, strVmPortList.str(), itPortInfo->second.m_strMacAddr,strPortIdList.str(), nUpPortId);
            //}
            //else
            //{
            //    VNetDelEvbDownlinkFlow(strBridgeName,itPortInfo->second.m_strMacAddr);
            //}
        }
    }
    dev_entity->SetVepaFlowSetFlag(1);
}

int32 CheckBrOfPort(string strPortName, string strBridgeName, int32 nVlan)
{
    string strTmpBr;
    //���˿��Ƿ������ȷ��������
    int iRet = VNetGetBridgeOfIf(strPortName, strTmpBr);
    if (iRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CMonitorVnic: call VNetGetBridgeOfIf(%s) failed.\n", strPortName.c_str());
        return iRet;
    }
    else
    {
        //û������
        if (strTmpBr.empty() )
        {
            VNetAddNetIf(strBridgeName, strPortName, nVlan);
            return 0;
        }
        //���ڴ����������
        else if (strBridgeName != strTmpBr)
        {
            VNetDelNetIf(strTmpBr, strPortName, 0);
            VNetAddNetIf(strBridgeName, strPortName, nVlan);
            return 0;
        }
        //��ȷ
        {
            return 0;
        }
    }

    return 0;
}

int32 CheckVlanOfPort(string strPortName, CVnicPortDev *dev_entity)
{
     uint32 nVlanModeCfg = dev_entity->GetSwitchPortMode();

    //���vif��VLAN�Ƿ���ȷ
    //ovs-vsctl -- get port eth0 vlan_mode
    //ovs-vsctl -- get port eth0 tag
    //ovs-vsctl -- get port eth0 trunks
#if 0
    uint32 nTmpVlanMode;
    if (VNetGetPortVlanMode(strPortName, nTmpVlanMode))
    {
        VNET_LOG(VNET_LOG_INFO, "CheckVlanOfPort: call VNetGetPortVlanMode(%s) failed.\n", strPortName.c_str());
        return -1;
    }
    
    if (nVlanModeCfg != nTmpVlanMode)
    {
        VNetSetPorVlanMode(strPortName, nVlanModeCfg);
    }
#endif

    //access
    if (VNET_PLUGIN_VLAN_MODE_ACCESS == nVlanModeCfg)
    {
        int32 nVlanCfg =  dev_entity->GetCVlan();

        uint32 nVlanValue;
        if (VNetGetVlanAccess(strPortName, nVlanValue))
        {
            VNET_LOG(VNET_LOG_INFO, "CheckVlanOfPort: call VNetGetVlanAccess(%s) failed.\n", strPortName.c_str());
            return -1;
        }
        
        if (nVlanCfg != (int32)nVlanValue)
        {
            if (VNetAccessWork(strPortName, nVlanCfg))
            {
                VNET_LOG(VNET_LOG_INFO, "CheckVlanOfPort: call VNetSetVlanAccess(%s, %d) failed.\n", strPortName.c_str(), nVlanCfg);
                return -1;
            }
        }
    }
    //trunk
    else
    {
        map<uint32, uint32>  mapTrunkVlan;
        dev_entity->GetTrunkVlan(mapTrunkVlan);
        int32 nNativeVlan = dev_entity->GetNativeVlan();

#if 0
        if (VNetGetVlanTrunks(strPortName, vecTrunkVlan))
        {
            VNET_LOG(VNET_LOG_INFO, "CheckVlanOfPort: call VNetGetVlanTrunks(%s) failed.\n", strPortName.c_str());
            return -1;
        }
#endif 
        VNetTrunkWork(strPortName, mapTrunkVlan, nNativeVlan);
    }

    return 0;
};

int32 CheckMtuOfPort(string strPortName, int32 nMTU)
{
    //���Vif��MTU�Ƿ���ȷ
    uint32 nTmpMTU;
    if (0 == VNetGetMtu(strPortName.c_str(), nTmpMTU))
    {
        if ((int32)nTmpMTU != nMTU)
        {
            if (VNetSetMtu(strPortName, nMTU))
            {
                VNET_LOG(VNET_LOG_INFO, "CMonitorVnic: call VNetGetMtu(%s, %d) failed.\n", strPortName.c_str(), nMTU);
                return -1;
            }
        }
    }
    else
    {
        VNET_LOG(VNET_LOG_INFO, "CMonitorVnic: call VNetGetMtu(%s) failed.\n", strPortName.c_str());
        return -1;
    }

    return 0;
};

int32 CheckVlanOfPCI(string strPCIOrder, CVnicPortDev *dev_entity)
{
#if 0
     uint32 nVlanModeCfg = dev_entity->GetSwitchPortMode();

    //���vif��VLAN�Ƿ���ȷ
    //ovs-vsctl -- get port eth0 vlan_mode
    //ovs-vsctl -- get port eth0 tag
    //ovs-vsctl -- get port eth0 trunks
    uint32 nTmpVlanMode;
    if (VNetGetPortVlanMode(strPCIOrder, nTmpVlanMode))
    {
        VNET_LOG(VNET_LOG_INFO, "CheckVlanOfPort: call VNetGetPortVlanMode(%s) failed.\n", strPortName.c_str());
        return -1;
    }
    
    if (nVlanModeCfg != nTmpVlanMode)
    {
        VNetSetPorVlanMode(strPortName, nVlanModeCfg);
    }

    //access
    if (VNET_PLUGIN_VLAN_MODE_ACCESS == nVlanModeCfg)
    {
        int32 nVlanCfg =  dev_entity->GetCVlan();

        uint32 nVlanValue;
        if (VNetGetVlanAccess(strPortName, nVlanValue))
        {
            VNET_LOG(VNET_LOG_INFO, "CheckVlanOfPort: call VNetGetVlanAccess(%s) failed.\n", strPortName.c_str());
            return -1;
        }
        
        if (nVlanCfg != nVlanValue)
        {
            if (VNetSetVFPortVlan(strPortName, nVlanCfg))
            {
                VNET_LOG(VNET_LOG_INFO, "CheckVlanOfPort: call VNetSetVlanAccess(%s, %d) failed.\n", strPortName.c_str(), nVlanCfg);
                return -1;
            }
        }
    }
    //trunk
    else
    {
        map<uint32, uint32>  mapTrunkVlan;
        dev_entity->GetTrunkVlan(mapTrunkVlan);

#if 0
        if (VNetGetVlanTrunks(strPortName, vecTrunkVlan))
        {
            VNET_LOG(VNET_LOG_INFO, "CheckVlanOfPort: call VNetGetVlanTrunks(%s) failed.\n", strPortName.c_str());
            return -1;
        }
#endif 
        VNetSetVlanTrunk(strPortName, mapTrunkVlan);
    }
#endif
    return 0;
};

int32 CheckMacOfPCI(string strPCIOrder, string strMACAdress)
{
#if 0
    CPhyPortManager *pPhyPortManager = CPhyPortManager::GetInstance();
    if (NULL == pPhyPortManager)
    {
        VNET_LOG(VNET_LOG_INFO, "CMonitorVnic: call CPhyPortManager::GetInstance() failed.\n");
        return -1;
    }

    //���Vif��MTU�Ƿ���ȷ
    string tmpMACAdress;
    if (0 == pPhyPortManager->GetVFPortMac(strPCIOrder, tmpMACAdress))
    {
        if (strMACAdress != tmpMACAdress)
        {
            if (0 != pPhyPortManager->SetVFPortMac(strPCIOrder, tmpMACAdress))
            {
                VNET_LOG(VNET_LOG_INFO, "CMonitorVnic: call VNetSetMac(%s, %s) failed.\n", 
                    strPCIOrder.c_str(), tmpMACAdress.c_str());
                return -1;
            }
        }
    }
    else
    {
        VNET_LOG(VNET_LOG_INFO, "CMonitorVnic: call VNetGetMac(%s) failed.\n", strPCIOrder.c_str());
        return -1;
    }
#endif
    return 0;
};



int32 CMonitorEmbedVnic(CVnicPortDev *dev_entity)
{
    int iRet = 0;
    string strMac;
    
    string strPCIOrder = dev_entity->GetPCI();
    if (strPCIOrder.empty())
    {
        return 0;
    }

    iRet = CheckVlanOfPCI(strPCIOrder, dev_entity);
    if (0 != iRet)
    {
        return -1;
    }

    if (0 != dev_entity->GetAssMac().size())
    {
        strMac = dev_entity->GetAssMac();
    }
    else
    {
        strMac = dev_entity->GetMacAddress();
    }
    
    iRet = CheckMacOfPCI(strPCIOrder, strMac);
    if (0 != iRet)
    {
        return -1;
    }

    return 1;
};

int32 MonitorVnic(string strVnicIf, CVnicPortDev *dev_entity)
{   
    //��������Ƿ�Ϊ��    
    if (strVnicIf.empty() || NULL==dev_entity)
    {
        return 0;
    }
    
    //�������
    string strBridge = dev_entity->GetBridge();
    if (!vnet_chk_dev(strBridge))
    {
        VNET_LOG(VNET_LOG_ERROR, "CMonitorVnic: strPortName(%s)'s bridge(%s) is not existed.\n", 
            strVnicIf.c_str(), strBridge.c_str());
        return 0;
    }

    //������ifconfig�����ڶ�OVS����   
    if (!vnet_chk_dev(strVnicIf))
    {        
        if (VNetNicIsInOVS(strVnicIf))
        {
            /*��Ҫ���ovs�е������˿�*/
            if (VNET_PLUGIN_SUCCESS != VNetDelVnicOfOVS(strVnicIf))
            {
                VNET_LOG(VNET_LOG_ERROR, "CMonitorVnic: call VNetDelVnicOfOVS delete conflict <%s> failed.\n", 
                    strVnicIf.c_str());                    
            }
        }
        return 0;
    }
    
    //���vnic��ovs��ϵ�Ƿ�����
    if (VNetNicIsInOVS(strVnicIf))
    {
        //��ȡstrVnicIf���ڵ�OVS�е�Bridge.
        string strBr;
        VNetIsPortInBridge(strVnicIf, strBr);

        //���vnic��ovs��ϵ�Ƿ�����
        if (strBr!=strBridge)        
        {            
            //��ɾ��ԭ����OVS��ϵ
            if (VNET_PLUGIN_SUCCESS !=  VNetDelVnicOfOVS(strVnicIf))
            {
                VNET_LOG(VNET_LOG_ERROR, "CMonitorVnic: call VNetDelVnicOfOVS delete conflict <%s> failed.\n", 
                    strVnicIf.c_str());                    
            }
            //�ٴ����µ�OVS��ϵ
            if (VNET_PLUGIN_SUCCESS != VNetAddNetIfOVS(strBridge, strVnicIf))
            {
                VNET_LOG(VNET_LOG_ERROR, "CMonitorVnic: call VNetDelNetIfOVS delete conflict <%s-%s> failed.\n", 
                    strBridge.c_str(), strVnicIf.c_str());                    
            }
        }
    }
    else
    {
        //�����µ�OVS��ϵ
        if (VNET_PLUGIN_SUCCESS != VNetAddNetIfOVS(strBridge, strVnicIf))
        {
            VNET_LOG(VNET_LOG_ERROR, "CMonitorVnic: call VNetDelNetIfOVS delete conflict <%s-%s> failed.\n", 
                strBridge.c_str(), strVnicIf.c_str());                    
        }
    }
    
    //���VNIC�˿��Ƿ�ifconfig up
    int nAdminStatus;
    if (VNET_PLUGIN_SUCCESS==VNetGetNicAdminStatus(strVnicIf, nAdminStatus))
    {
        if (nAdminStatus==0)
        {
            VNetSetNicAdminStatus(strVnicIf, 1);            
        }        
    }

    //vif VLAN_CHECK
    if (CheckVlanOfPort(strVnicIf, dev_entity))
    {
        VNET_LOG(VNET_LOG_ERROR, "CMonitorVnic: CheckPortAndBr(%s , %s) failed.\n", strVnicIf.c_str(), strBridge.c_str());
    }

    //MTU_CHECK:
    if (CheckMtuOfPort(strVnicIf, dev_entity->GetMTU()))
    {
        VNET_LOG(VNET_LOG_ERROR, "CMonitorVnic: CheckMtuOfPort(%s , %d) failed.\n", strVnicIf.c_str(), dev_entity->GetMTU());
    }

    return 1;
}

int32 CMonitorSoftVnic(CVnicPortDev *dev_entity)
{
    string strVifName = dev_entity->GetVifName();
    string strTapName = dev_entity->GetTapName();    
    string strEmuName = dev_entity->GetEmuName();        
    
    //���domid�Ƿ�֪ͨ����
    if (strVifName.empty() && strTapName.empty() && strEmuName.empty())
    {
        return 0;
    }

    if (HYPE_VISOR_TYPE_XEN == g_nHypeVisorType)
    {
        MonitorVnic(strVifName, dev_entity);
        MonitorVnic(strTapName, dev_entity);
        MonitorVnic(strEmuName, dev_entity);
        //check vepa flows        
        CheckVepaFlowOfPort(dev_entity, 0);
    }
    //KVM
    else
    {
        string strVirName;

        //����vsi id�е�'-',ovs��¼��iface-id����'-'�����ǵ�vsi��û��'-'
        string strVsiIdValue = dev_entity->GetVSIIDValue();
        string strVsiIdValueFormat; //��'-'��vsi id
        if (strVsiIdValue.find("-") == std::string::npos) 
        {
            VNetFormatString(strVsiIdValueFormat, "%s-%s-%s-%s-%s",
                    strVsiIdValue.substr(0,8).c_str(),
                    strVsiIdValue.substr(8,4).c_str(),
                    strVsiIdValue.substr(12,4).c_str(),
                    strVsiIdValue.substr(16,4).c_str(),
                    strVsiIdValue.substr(20,12).c_str());
        }
        else
        {
            strVsiIdValueFormat = strVsiIdValue;
        }
        
        if (VNET_PLUGIN_FALSE == VNetGetPortNameForKVM(strVsiIdValueFormat, strVirName))
        {
            return 0;            
        }
        
        dev_entity->SetVifName(strVirName);
        dev_entity->SetTapName("");
        dev_entity->SetEmuName("");

        MonitorVnic(strVifName, dev_entity);

        //check vepa flows
        //CheckVepaFlowOfPort(dev_entity, 0);
    }

    return 1;
};

void CMonitorVnic(CVnicPortDev *dev_entity)
{
    int iRet = 0;
    if (NULL == dev_entity)
    {
        return;
    }
    
    time_t cur_time = time(0);
    if ((VNA_VNIC_MON_FLAG_YES == dev_entity->GetMonSetFlag())
        && ((cur_time - dev_entity->GetLastMonSetTime()) < DEV_MON_VNIC_NOMAL_INTERVAL))
    {
        return;
    }

    if (dev_entity->GetIsSriov())
    {
        iRet = CMonitorEmbedVnic(dev_entity);
    }
    else
    {
        iRet = CMonitorSoftVnic(dev_entity);
    }

    if (1 == iRet)
    {
        dev_entity->SetMonSetFlag(VNA_VNIC_MON_FLAG_YES);
        dev_entity->SetLastMonSetTime(cur_time);
    }
};

}
