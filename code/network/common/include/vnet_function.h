/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：vnet_function.h
* 文件标识：
* 内容摘要：vnet_libnet.h类的定义文件
* 当前版本：1.0
* 作    者：
* 完成日期：2012年12月16日
*
* 修改记录1：
*     修改日期：2012/1/8
*     版 本 号：V1.0
*     修 改 人：chenzhiwei
*     修改内容：创建
******************************************************************************/
#ifndef __VNET_FUNCTION_H
#define __VNET_FUNCTION_H

#include <stdio.h>
#include <string>
#include <string.h>
#include <vector>
#include <map>
#include <iostream>
#include <dlfcn.h>
#include <dirent.h>
#include <boost/algorithm/string.hpp>

using namespace boost;
using namespace std;

#ifndef uint32
//#define uint32 int
typedef unsigned int uint32;
#endif

#ifndef VNET_PLUGIN_VLAN_MODE_ACCESS
#define VNET_PLUGIN_VLAN_MODE_ACCESS (1)
#endif 

#ifndef VNET_PLUGIN_VLAN_MODE_TRUNK
#define VNET_PLUGIN_VLAN_MODE_TRUNK  (0)
#endif

#ifndef VNET_PLUGIN_VLAN_MODE_NONE
#define VNET_PLUGIN_VLAN_MODE_NONE   (2)
#endif

#ifndef VNET_PLUGIN_ERROR
#define VNET_PLUGIN_ERROR (-1)
#endif

#ifndef VNET_PLUGIN_SUCCESS
#define VNET_PLUGIN_SUCCESS (0)
#endif

#ifndef VNET_PLUGIN_FALSE
#define VNET_PLUGIN_FALSE (0)
#endif

#ifndef VNET_PLUGIN_TRUE
#define VNET_PLUGIN_TRUE (1)
#endif

#ifndef VNET_PLUGIN_MAX_VLAN_ID
#define VNET_PLUGIN_MAX_VLAN_ID (4094)
#endif

int RunCmd(const string& cmd, vector<string>& result);
int VNetRunCmd(const string& cmd, vector<string>& result);
////////////////////////////////////////////////////////////////////////////////
/////////////////网卡相关接口///////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
int VNetGetAllNic(vector<string>& nics);
int VNetGetAllVirNic(vector<string>& nics);
int VNetGetAllVnics(vector<string>& nics);
int VNetGetAllUplinks(vector<string>& uplinks);
int VNetGetAllUplinksSubnets(const string eth, vector<string>& up_subnets);
int VNetDelVnicOfOVS(const string& nic);
int VNetIfconfigIsExist(const string &nic);
int VNetNicIsInOVS(const string &nic);
int VNetSetMtu(const string& nic,const int& mtu);
int VNetGetMtu(const char* lpszEth, unsigned int &mtu);
int VNetSetNicAdminStatus(const string strNic, int nStatus);
int VNetGetNicAdminStatus(const string strNic, int &nStatus);
int VNetGetNicStatus(const string nics,int & status);
int VNetSetVlan(const string& nic,uint32 dwVlanID);
int VNetRemoveVlan(const string& nic,uint32 dwVlanID);
int VNetIsPortInBond(const string& nic,string& bond);
int VNetIsPortInBridge(const string& nic,string& bridge);
int VNetGetPortNameForKVM(const string& portid ,string& portname);


////////////////////////////////////////////////////////////////////////////////
/////////////////网桥相关接口///////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
int VNetGetAllBridges(vector<string>& bridges);
int VNetGetIfsOfBridge(const string& bridge, vector<string>& ifs);
int VNetGetAllOfPortIdOfBridge(const string& bridge, map<string, uint32>& of_port_list);
int VNetGetOfPortIdOfIf(const string& nic, uint32& of_port_id);
int VNetGetIfListOfSameVlan(const uint32& vlan, vector<string>&ifs);
int VNetGetBridgeOfIf(const string& nic, string& bridge);
int VNetGetRealIfNameFromBridge(string & port, string& rel_name);
int VNetGetPhyPortFromBr(const string &strBrName,string &phyport);
int VNetAddNetIf(const string& strBr,const string& strNetIf,  uint32 nVlanID);
int VNetDelNetIf(const string& strBr,const string& strNetIf,  uint32 nVlanID);
int VNetAddVxlanNetIf(const string& strBr,const string& strVxlanNetIf, const string& strKey, const string& strRemoteIP);
int VNetDelVxlanNetIf(const string& strBr,const string& strVxlanNetIf);
int VNetDelNetIfOVS(const string& strBr,const string& strNetIf);
int VNetCreateBridge(const string & strBr);
int VNetCreateBridge_withif(const string& strBr,const string& strNetIf,  uint32 nVlanID );
int VNetDeleteBridge(const string & strBr);  
int VNetDeleteBridge_withif(const string& strBr,const string& strNetIf,  uint32 nVlanID );
int VNetSetSdnController(const string& strBr, const string& strProto, const string& strIp, uint32 nPort);
int VNetDelSdnController(const string& strBr);
int VNetSetExternalid(const string& strBr, const string& strPort, const string& strPortUuid, const string& strMac, const string& strVmUuid);
int VNetDelExternalid(const string& strBr, const string& strPort);


////////////////////////////////////////////////////////////////////////////////
/////////////////bonding相关接口////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
int VNetIsSupportBond(int &isSupport);
int VNetGetAllBondings(vector<string>& bondings);
int VNetGetBondPara(const string &bond,const string &para,string& value);
int VNetAddBond(const string &bond, const string &mode, vector<string> &ifs);
int VNetDelBond(const string &bond);
int VNetSetBondPara(const string &bond, const string &para,const string &value);
int VNetAddBondSlave(const string &bond, const string &slave);
int VNetDelBondSlave(const string &bond, const string &slave);
int VNetGetBondSlaves(const string &bond, vector<string> &vecslaves);

////////////////////////////////////////////////////////////////////////////////
/////////////////ovs bonding相关接口////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
int VNetGetAllBondingsOVS(vector<string>& bondings);
int VNetGetBondModeOVS(const string &bond,string& mode);
int VNetGetBondSlavesOVS(const string &bond, vector<string> &vecslaves);
int VNetAddBondOVS(const string &bridge,const string &bond, const string &mode, vector<string> &ifs);
int VNetDelBondOVS(const string &bond);
int VNetRenameDVS(const string &oldbr,const string &newbr,const string &bond_name);

int VNetIsPortInBondOVS(const string& nic,string& bond);
int VNetBackupInfo(const string &bond, string &active, vector<string> &others);
int VNetLacpInfo(const string &bond, int &state,string &aggregate_id, string &partner_mac,
                   vector<string>  &suc_nics,vector<string> &fail_nics);

int VNetGetPhyPortFromBrOVS(const string &strBrName,string &phyport);
int VNetSetBrideForwordLLDP(const string &strBrName);

int VNetAddNetIfOVS(const string & strBr, const string & strNetIf);

////////////////////////////////////////////////////////////////////////////////
/////////////////SRIOV相关接口//////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
int VNetGetAllSriovPort(vector<string>& sriovs);
int VNetGetSriovPci(const string &nicname, map<string,string>&vif_pci);
int VNetGetSriovNum(const string &nicname, int &num);
int VNetSetVFPortMac(const string &port_name, const int &vfnum, const string &mac);
int VNetSetVFPortVlan(const string &port_name, const int &vfnum, const int &vlan);
int VNetGetVFPortMac(const string &port_name, const int &vfnum,  string &mac);
int VNetGetVFPortVlan(const string &port_name, const int &vfnum,  int &vlan);

////////////////////////////////////////////////////////////////////////////////
/////////////////LOOPBACK口相关接口//////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
int VNetIsLoopDevice(const string &port_name);
int VNetGetPortLoopbackMode(const string &port_name, int &lpflag);
int VNetSetPorLoopback(const string &port_name, const uint32 &lpflag);
int VNetCreateLoopBackPort(const string &port_name);
int VNetDeleteLoopBackPort(const string &port_name);
int VNetGetAllLoopBackPort(vector<string>& loops);

////////////////////////////////////////////////////////////////////////////////
/////////////////KERNEL口相关接口//////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
int VNetCreateKernelPort(const string &port_name, const string &switch_name, const string &uplinkport, const string &ip, const string &mask, const uint32 is_dhcp,const uint32 vlan, const uint32 mtu);
int VNetModifyKernelPort(const string &port_name, const string &switch_name, const string &uplinkport, const string &ip, const string &mask);
int VNetDeleteKernelPort(const string &switch_name, const string &port_name);
int VNetGetAllKernelPort(vector<string>& kernels);
int VNetGetKernelPortInfo(const string &port_name, string &ip, string &mask);

////////////////////////////////////////////////////////////////////////////////
/////////////////trunk/accsee相关接口//////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
int VNetSetPorVlanMode(const string &port_name, const uint32 &vlanflag);
int VNetRemovePorVlanMode(const string &port_name, const uint32 &vlanflag);
int VNetGetPortVlanMode(const string &port_name,  uint32 &vlanflag);

int VNetSetVlanAccess(const string &port_name, const uint32 &accessvlan);
int VNetGetVlanAccess(const string &port_name,  uint32 &accessvlan); 

int VNetSetVlanNativeForTrunk(const string &port_name, const uint32 &nativevlan);

int VNetSetVlanTrunk(const string &port_name, map<uint32,uint32>trunks);
int VNetGetVlanTrunks(const string &port_name,  vector<uint32>vlan);

int VNetTrunkWork(const string &port_name, map<uint32,uint32>trunks, const uint32 &nativevlan);
int VNetAccessWork(const string &port_name, const uint32 &access_vlan);

int VNetSetEvbUplinkFlow(const string bridge, const string vmMac, const uint32 vmPort,const uint32 UplinkPort, const uint32 TagVlan);
int VNetDelEvbUplinkFlow(const string bridge, const string vmMac);
int VNetSetEvbDownlinkFlow(const string bridge, const string vmPort,const string vmMac, const string otherPorts,const uint32  UplinkPort);
int VNetDelEvbDownlinkFlow(const string bridge, const string vmMac);
bool VNetIsVxlanSupport(void);


#endif



