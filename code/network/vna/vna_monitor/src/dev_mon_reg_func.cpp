/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：dev_mon_reg_func.cpp
* 文件标识：
* 内容摘要：监控相关处理函数的实现文件
* 当前版本：1.0
* 作    者：
* 完成日期：2013年1月16日
*
* 修改记录1：
*     修改日期：2013/1/16
*     版 本 号：V1.0
*     修 改 人：lverchun
*     修改内容：创建
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

//定义监控网络设备MTU属性处理函数
TDevMonFuncReg s_MonMTUFuncReg[] = 
{ 
    {TYPE_INFO(CPhyPortClass), CMonitorMTUPhyPort},
    {TYPE_INFO(CTrunkPortClass), CMonitorMTUBondPort},
    {TYPE_INFO(CVnicPortDev), CMonitorMTUVnicPort},
    {TYPE_INFO(CSoftDVSDev), CMonitorMTUSDVS},
    {TYPE_INFO(CEmbDVSDev), CMonitorMTUEDVS},
};

//定义监控网口的所挂接网桥属性处理函数
TPortMonFuncReg s_MonBridgeFuncReg[] = 
{ 
    {TYPE_INFO(CPhyPortClass), CMonitorBridgePhyPort},
    {TYPE_INFO(CTrunkPortClass), CMonitorBridgeBondPort},
    {TYPE_INFO(CVnicPortDev), CMonitorBridgeVnicPort},
};

//定义监控网络设备Loop属性处理函数
TPortMonFuncReg s_MonLoopFuncReg[] = 
{ 
    {TYPE_INFO(CPhyPortClass), CMonitorLoopPhyPort},
    {TYPE_INFO(CTrunkPortClass), CMonitorLoopBondPort},
};

//定义监控网桥上行端口属性处理函数
TDVSMonFuncReg s_MonUpLinkFuncReg[] = 
{ 
    {TYPE_INFO(CSoftDVSDev), CMonitorUpLinkSDVS},
    {TYPE_INFO(CEmbDVSDev), CMonitorUpLinkEDVS},
};

//定义监控VNIC端口属性处理函数
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


    if (0 != VNetSetMtu(dev_entity->GetDevName().c_str(), dev_entity->GetDevMTU())) //先设置网口mtu
    {
        cout << "call set_mtu(" << dev_entity->GetDevName() <<", " << dev_entity->GetDevMTU() << ") failed" <<endl;
    }

};

void CMonitorMTUBondPort(CNetDev *dev_entity)
{
    //cout <<"-----------------CMonitorMTUBondPort" <<dev_entity->GetDevName() <<endl;

    if (0 != VNetSetMtu(dev_entity->GetDevName().c_str(), dev_entity->GetDevMTU())) //先设置网口mtu
    {
        cout << "call set_mtu(" << dev_entity->GetDevName() <<", " << dev_entity->GetDevMTU() << ") failed" <<endl;
    }
};

void CMonitorMTUVnicPort(CNetDev *dev_entity)
{
    //cout <<"-----------------CMonitorMTUVnicPort" <<dev_entity->GetDevName() <<endl;

    if (0 != set_mtu(dev_entity->GetDevName().c_str(), dev_entity->GetDevMTU())) //先设置网口mtu
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
        if(0 != VNetSetMtu(itr->c_str(), nMTU)) //先设置网口mtu
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
        if(0 != VNetSetMtu(itr->c_str(), nMTU)) //先设置网口mtu
        {
            cout << "call set_mtu(" << *itr <<", " << nMTU << ") failed" <<endl;
        }
    }  
};

void CMonitorUpLinkEDVS(CSwitchDev *dev_entity)
{
    //cout <<"-----------------CMonUpLinkEDVS" <<dev_entity->GetDevName() <<endl;
};


    
//设置vepa网桥上相关流表
void CheckVepaFlowOfPort(CVnicPortDev *dev_entity,int32 op_flag)
{
    int iRet = 0;
    uint32 nNativeVlan = 1;      //上行口native vlan
    uint32 nTagVlan = 0;         //虚拟口报文出网桥要打的vlan
    vector<string> vecPortList;
    vector<string> vecVlanPortList;

    string strBridgeName = dev_entity->GetBridge();

    //检查网桥EVB模式
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
    

    //先获取本网桥上所有端口及其openflow port id
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
    
    //获取本机上所有vlan相同的端口
    iRet = VNetGetIfListOfSameVlan(dev_entity->GetCVlan(),vecVlanPortList);
    if (VNET_PLUGIN_SUCCESS != iRet)
    {
        return;
    }       

    map<string, TPortInfo> mapPortInfoList;

    //获得本网桥上vlan相同的端口
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

        //删除vlan不同接口
        vector<string>::iterator pos;
        pos = find(vecVlanPortList.begin(),vecVlanPortList.end(), it->first);
        if (pos == vecVlanPortList.end()) 
        {
            mapPortIdList.erase(it++);
            continue;
        }
        
        //如果tap和vif都有，则需要剔除掉tap对应的vif口，只处理tap口
        if (it->first.find("tap") != string::npos)
        {
            string strVifName = it->first;
            strVifName.replace(0,3,"vif");
            //检查是否有对应vif接口
            map<string, uint32>::iterator vif_id_pos = mapPortIdList.find(strVifName);
            if (vif_id_pos != mapPortIdList.end())
            {
                map<string, TPortInfo>::iterator vif_info_pos = mapPortInfoList.find(strVifName);
                if (vif_info_pos != mapPortInfoList.end())//存在
                {
                    vif_info_pos->second.m_strTapPort = it->first;
                    vif_info_pos->second.m_nTapPortOfId = it->second;
                }
                else//还没添加
                {
                    TPortInfo tPortInfo;
                    tPortInfo.m_strVifPort = vif_id_pos->first;
                    tPortInfo.m_nVifPortOfId = vif_id_pos->second;
                    tPortInfo.m_strTapPort = it->first;
                    tPortInfo.m_nTapPortOfId = it->second;

                    mapPortInfoList.insert(pair<string, TPortInfo> (vif_id_pos->first, tPortInfo));
                }
            }
            else//没有
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
            if (vif_info_pos == mapPortInfoList.end())//还没添加
            {
                TPortInfo tPortInfo;
                tPortInfo.m_strVifPort = it->first;
                tPortInfo.m_nVifPortOfId = it->second;

                mapPortInfoList.insert(pair<string, TPortInfo> (it->first, tPortInfo));
            }
        
        }

        //如果emu和vif都有，则需要剔除掉emu对应的vif口，只处理vif-emu口
        if (it->first.find("emu") != string::npos)
        {
            string strVifName = it->first;
            strVifName = strVifName.substr(0, (strVifName.length() - 4));
            //检查是否有对应vif接口
            map<string, uint32>::iterator vif_id_pos = mapPortIdList.find(strVifName);
            if (vif_id_pos != mapPortIdList.end())
            {
                map<string, TPortInfo>::iterator vif_info_pos = mapPortInfoList.find(strVifName);
                if (vif_info_pos != mapPortInfoList.end())//存在
                {
                    vif_info_pos->second.m_strEmuPort = it->first;
                    vif_info_pos->second.m_nEmuPortOfId = it->second;
                }
                else//还没添加
                {
                    TPortInfo tPortInfo;
                    tPortInfo.m_strVifPort = vif_id_pos->first;
                    tPortInfo.m_nVifPortOfId = vif_id_pos->second;
                    tPortInfo.m_strEmuPort = it->first;
                    tPortInfo.m_nEmuPortOfId = it->second;

                    mapPortInfoList.insert(pair<string, TPortInfo> (vif_id_pos->first, tPortInfo));
                }
            }
            else//没有
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
            if (vif_info_pos == mapPortInfoList.end())//还没添加
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
        //查询vif接口mac地址
        pVnicPool->GetMacFromPortName(it_info->first, it_info->second);
    }


    //查询自己vif接口mac地址
    TPortInfo tPortInfo1;
    pVnicPool->GetMacFromPortName(strSelfVifName, tPortInfo1);
    strSelfMacAddr = tPortInfo1.m_strMacAddr;

    //增加流表
    if (0 == op_flag)
    { 
        //更新流表
        map<string, TPortInfo>::iterator itPortInfo = mapPortInfoList.begin();
        for (; itPortInfo != mapPortInfoList.end(); itPortInfo++)
        {
            stringstream strVmPortList;
            //设置上行流表
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
	
            //设置下行流表,包括单播及广播
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
            
            //增加下行流表
            VNetSetEvbDownlinkFlow(strBridgeName, strVmPortList.str(), itPortInfo->second.m_strMacAddr,strPortIdList.str(), nUpPortId);
        }

    }
    //删除流表
    else
    {
        VNetDelEvbUplinkFlow(strBridgeName, strSelfMacAddr);
        VNetDelEvbDownlinkFlow(strBridgeName,strSelfMacAddr);

        //更新流表
        map<string, TPortInfo>::iterator itPortInfo = mapPortInfoList.begin();
        for (; itPortInfo != mapPortInfoList.end(); itPortInfo++)
        {
            //剔除自己
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
            
            //设置下行流表,包括单播及广播
            stringstream strPortIdList;
            map<string, TPortInfo>::iterator itSet = mapPortInfoList.begin();
            for (; itSet != mapPortInfoList.end(); itSet++)
            {
                //剔除自己
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
            
            //更新流表
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
    //检查端口是否挂在正确的网桥上
    int iRet = VNetGetBridgeOfIf(strPortName, strTmpBr);
    if (iRet)
    {
        VNET_LOG(VNET_LOG_ERROR, "CMonitorVnic: call VNetGetBridgeOfIf(%s) failed.\n", strPortName.c_str());
        return iRet;
    }
    else
    {
        //没挂网桥
        if (strTmpBr.empty() )
        {
            VNetAddNetIf(strBridgeName, strPortName, nVlan);
            return 0;
        }
        //挂在错误的网桥上
        else if (strBridgeName != strTmpBr)
        {
            VNetDelNetIf(strTmpBr, strPortName, 0);
            VNetAddNetIf(strBridgeName, strPortName, nVlan);
            return 0;
        }
        //正确
        {
            return 0;
        }
    }

    return 0;
}

int32 CheckVlanOfPort(string strPortName, CVnicPortDev *dev_entity)
{
     uint32 nVlanModeCfg = dev_entity->GetSwitchPortMode();

    //监控vif的VLAN是否正确
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
    //监控Vif的MTU是否正确
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

    //监控vif的VLAN是否正确
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

    //监控Vif的MTU是否正确
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
    //检查网口是否为空    
    if (strVnicIf.empty() || NULL==dev_entity)
    {
        return 0;
    }
    
    //监控网桥
    string strBridge = dev_entity->GetBridge();
    if (!vnet_chk_dev(strBridge))
    {
        VNET_LOG(VNET_LOG_ERROR, "CMonitorVnic: strPortName(%s)'s bridge(%s) is not existed.\n", 
            strVnicIf.c_str(), strBridge.c_str());
        return 0;
    }

    //若网口ifconfig不存在而OVS中有   
    if (!vnet_chk_dev(strVnicIf))
    {        
        if (VNetNicIsInOVS(strVnicIf))
        {
            /*需要清除ovs中的垃圾端口*/
            if (VNET_PLUGIN_SUCCESS != VNetDelVnicOfOVS(strVnicIf))
            {
                VNET_LOG(VNET_LOG_ERROR, "CMonitorVnic: call VNetDelVnicOfOVS delete conflict <%s> failed.\n", 
                    strVnicIf.c_str());                    
            }
        }
        return 0;
    }
    
    //监控vnic与ovs关系是否正常
    if (VNetNicIsInOVS(strVnicIf))
    {
        //获取strVnicIf所在的OVS中的Bridge.
        string strBr;
        VNetIsPortInBridge(strVnicIf, strBr);

        //监控vnic与ovs关系是否正常
        if (strBr!=strBridge)        
        {            
            //先删除原来的OVS关系
            if (VNET_PLUGIN_SUCCESS !=  VNetDelVnicOfOVS(strVnicIf))
            {
                VNET_LOG(VNET_LOG_ERROR, "CMonitorVnic: call VNetDelVnicOfOVS delete conflict <%s> failed.\n", 
                    strVnicIf.c_str());                    
            }
            //再创建新的OVS关系
            if (VNET_PLUGIN_SUCCESS != VNetAddNetIfOVS(strBridge, strVnicIf))
            {
                VNET_LOG(VNET_LOG_ERROR, "CMonitorVnic: call VNetDelNetIfOVS delete conflict <%s-%s> failed.\n", 
                    strBridge.c_str(), strVnicIf.c_str());                    
            }
        }
    }
    else
    {
        //创建新的OVS关系
        if (VNET_PLUGIN_SUCCESS != VNetAddNetIfOVS(strBridge, strVnicIf))
        {
            VNET_LOG(VNET_LOG_ERROR, "CMonitorVnic: call VNetDelNetIfOVS delete conflict <%s-%s> failed.\n", 
                strBridge.c_str(), strVnicIf.c_str());                    
        }
    }
    
    //监控VNIC端口是否ifconfig up
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
    
    //检查domid是否通知过来
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

        //处理vsi id中的'-',ovs记录中iface-id中有'-'，我们的vsi中没有'-'
        string strVsiIdValue = dev_entity->GetVSIIDValue();
        string strVsiIdValueFormat; //带'-'的vsi id
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
