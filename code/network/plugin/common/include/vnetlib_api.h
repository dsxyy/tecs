/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�vnet_xen_plug.h
* �ļ���ʶ��
* ����ժҪ��vnet_xen_plug��Ķ����ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�
* ������ڣ�2012��12��16��
*
* �޸ļ�¼1��
*     �޸����ڣ�2012/12/16
*     �� �� �ţ�V1.0
*     �� �� �ˣ�chenzhiwei
*     �޸����ݣ�����
******************************************************************************/
#ifndef VNETLIB_API_H
#define VNETLIB_API_H

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <assert.h>       
#include <errno.h>       
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/syscall.h> 
#include <sys/time.h>
#include "net/if.h"
#include <fcntl.h>
#include <boost/algorithm/string.hpp>


using namespace std;

typedef unsigned int  uint32;
typedef int           int32;
typedef long long     int64;


#define VNET_PLUGIN_ERROR (-1)
#define VNET_PLUGIN_SUCCESS (0)
#define VNET_PLUGIN_FALSE (0)
#define VNET_PLUGIN_TRUE (1)
#define VNET_PLUGIN_MAX_VLAN_ID (4094)

#define VNET_PLUGIN_VLAN_MODE_ACCESS (1)
#define VNET_PLUGIN_VLAN_MODE_TRUNK  (0)
#define VNET_PLUGIN_VLAN_MODE_NONE   (2)



//////////////////////////////////////////////////////////////////////////////// 
////////////////ioctl������ܺ��ں������ͻ(�ں�˽������Ϊ0x89f0-0x89ff)/////
////////////////////////////////////////////////////////////////////////////////
#define SIOCDEVPRIINFO   0x89FE  
#define PRIVATE_SETLOOP  0x01      /* �����������ڻ���ģʽ */
#define PRIVATE_GETLOOP  0x02      /* ���ڲ�ѯ���ڻ���ģʽ */
#define PRIVATE_SETVF    0x03      /* ������������VF data(=1����; =0�ر�) */
#define PRIVATE_GETVF    0x04      /* ���ڲ�ѯ����VF data(=1����; =0�ر�) */

////////////////////////////////////////////////////////////////////////////////
//////PRIVATE_SETVF  PRIVATE_GETVF ����Data Value///////////////////////////////
//////SIOCDEVPRIINFO�����Ӧ��˽�нṹ//////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#define PRIVATE_VF_VALUE_CLOSE 0x0
#define PRIVATE_VF_VALUE_OPEN  0x01
struct vnet_lib_private_value{
    unsigned int cmd;
    unsigned int data;
};

////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////�˿�״̬/////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#define  PORT_LOOPBACK_NO      (int)0        // ���Ի�ģʽ
#define  PORT_LOOPBACK_YES     (int)1        // �Ի�ģʽ

//////////////////////////////////////////////////////////////////////////
//                           VNET�ű�����
//////////////////////////////////////////////////////////////////////////
#define VNET_BRIDGE_SCRIPTS        "/opt/tecs/network/vna/scripts/vnet_dobridge.sh"
#define VNET_BRIDGE_SCRIPTS_OVS    "/opt/tecs/network/vna/scripts/vnet_dobridge_ovs.sh"
#define VNET_BONDS_SCRIPTS         "/opt/tecs/network/vna/scripts/do_bond.sh"

#define VNET_CREATE_BRIDGE    "create_bridge"
#define VNET_DELETE_BRIDGE    "delete_bridge"
#define VNET_GET_ALLBRIDGE    "get_all_bridge"
#define VNET_CREATE_BRIDGE_NOIF "create_bridge_noif"
#define VNET_DELETE_BRIDGE_NOIF "delete_bridge_noif"
#define VNET_ADD_IF           "add_ifnet"
#define VNET_ADD_IF_OVS       "add_ifnet_ovs"

#define VNET_DEL_IF           "del_ifnet"
#define VNET_DEL_IF_OVS       "del_ifnet_ovs"

#define VNET_SET_SDN_CONTROLLER  "set_controller"
#define VNET_DEL_SDN_CONTROLLER  "del_controller"
#define VNET_SET_EXTERNAL_ID     "set_external_id"
#define VNET_DEL_EXTERNAL_ID     "del_external_id"

#define VNET_SET_PORT_MODE    "set_port_mode"
#define VNET_DEL_PORT_MODE    "del_port_mode"
#define VNET_ADD_VLAN         "add_vlan"
#define VNET_DEL_VLAN         "del_Vlan"

#define VNET_SET_VLAN_NATIVE_FOR_TRUNK   "set_native_vlan"

#define VNET_ADD_BOND         "add_bond"
#define VNET_ADD_BOND_SLAVE   "add_bond_slave"
#define VNET_DEL_BOND_SLAVE   "del_bond_slave" 
#define VNET_DEL_BOND         "del_bond"
#define VNET_SET_BOND         "set_bond"
//#define VNET_ADD_KERNAL       "add_kernal"
#define VNET_ADD_LOOP         "add_loop"
#define VNET_DEL_LOOP         "del_loop"

#define VNET_ADD_KERNEL       "add_kernel"
#define VNET_MOD_KERNEL       "mod_kernel"
#define VNET_DEL_KERNEL       "del_kernel"
#define VNET_GET_KERNEL       "get_kernel"

#define VNET_IS_BOND_SLAVE    "is_bond_slave"
#define VNET_IS_BOND_SLAVE_OVS    "is_bond_slave_ovs"

#define VNET_IS_IN_BRIDGE     "is_in_bridge"
#define VNET_ID_TO_PORTNAME   "id_to_portname"

#define VNET_ADD_BOND_OVS     "add_bond_ovs"
#define VNET_RENAME_DVS       "rename_dvs"
#define VNET_BACKUP_INFO      "backup_info"
#define VNET_LACP_INFO        "lacp_info"


////////////////////////////////////////////////////////////////////////////////
////////////////linux�������ַ�����ȹ�����ؽӿ�///////////////////////////////
////////////////////////////////////////////////////////////////////////////////
extern "C" int vnet_chk_dir(const string &strDir);
extern "C" int vnet_chk_dev(const string &strDevName);
extern "C" int vnet_system(const string &strCmd);
extern "C" string int2string(const int& intvar);
extern "C" int  string2int(const string& strvar);
extern "C" void replacedstr(string &a,string b);
extern "C" int removeendl(string &s);
extern "C" int put_string_to_vec(string text,vector<string>& vec);
extern "C" int RunCmd(const string& cmd, vector<string>& result);
extern "C" int RunCmdRetString(const string& cmd, string& result);
extern "C" int SendIOCtrl(const char* ifname, struct vnet_lib_private_value *pval);

////////////////////////////////////////////////////////////////////////////////
/////////////////������ؽӿ�///////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
extern "C" int GetAllNic(vector<string>& nics);
extern "C" int GetAllVirNic(vector<string>& nics);
extern "C" int GetAllVnics(vector<string>& nics);
extern "C" int GetAllUplinks(vector<string>& uplinks);
extern "C" int GetAllUplinksSubnets(const string eth, vector<string>& up_subnets);
extern "C" int DelVnicOfOVS(const string& nic);
extern "C" int IfconfigIsExist(const string& nic);
extern "C" int NicIsInOVS(const string& nic);
extern "C" int SetMtu(const string& nic,const int& mtu);
extern "C" int GetMtu(const char* lpszEth, unsigned int &mtu);
extern "C" int GetNicAdminStatus(const string strNic, int &nStatus);
extern "C" int SetNicAdminStatus(const string strNic, int nStatus);
extern "C" int GetNicStatus(const string nics,int & status);
extern "C" int SetVlan(const string& nic,uint32 dwVlanID);
extern "C" int RemoveVlan(const string& nic,uint32 dwVlanID);
extern "C" int VNetIsPortInBond(const string& nic,string& bond);
extern "C" int VNetIsPortInBridge(const string& nic,string& bridge);
extern "C" int GetPortNameForKVM(const string& portid,string& port_name);


////////////////////////////////////////////////////////////////////////////////
/////////////////������ؽӿ�///////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
extern "C" int GetAllBridges(vector<string>& bridges);
extern "C" int GetIfsOfBridge(const string& bridge, vector<string>& ifs);
extern "C" int GetAllOfPortIdOfBridge(const string& bridge, map<string, uint32>& of_port_list);
extern "C" int GetOfPortIdOfIf(const string& nic, uint32& of_port_id);
extern "C" int GetIfListOfSameVlan(const uint32& vlan, vector<string>&ifs);
extern "C" int GetBridgeOfIf(const string& nic, string& bridge);
extern "C" int GetRealIfNameFromBridge(string & port, string& rel_name);
extern "C" int GetPhyPortFromBr(const string &strBrName,string &phyport);
extern "C" int AddNetIf(const string& strBr,const string& strNetIf,  uint32 nVlanID = 0);
extern "C" int DelNetIf(const string& strBr,const string& strNetIf,  uint32 nVlanID = 0);
extern "C" int AddVxlanNetIf(const string& strBr,const string& strVxlanNetIf, const string& strKey, const string& strRemoteIP);
extern "C" int DelVxlanNetIf(const string& strBr,const string& strVxlanNetIf);
extern "C" int DelNetIfOVS(const string& strBr,const string& strNetIf);

extern "C" int CreateBridge(const string & strBr);
extern "C" int CreateBridge_withif(const string& strBr,const string& strNetIf,  uint32 nVlanID = 0);
extern "C" int DeleteBridge(const string & strBr);  
extern "C" int DeleteBridge_withif(const string& strBr,const string& strNetIf,  uint32 nVlanID = 0);
extern "C" int SetSdnController(const string& strBr, const string& strProto, const string& strIp, uint32 nPort);
extern "C" int DelSdnController(const string& strBr);
extern "C" int SetExternalid(const string& strBr, const string& strPort, const string& strPortUuid, const string& strMac, const string& strVmUuid);
extern "C" int DelExternalid(const string& strBr, const string& strPort);

////////////////////////////////////////////////////////////////////////////////
/////////////////bonding��ؽӿ�////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
extern "C" int IsSupportBond(int &isSupport);
extern "C" int GetAllBondings(vector<string>& bondings);
extern "C" int GetBondPara(const string &bond,const string &para,string& value);
extern "C" int AddBond(const string &bond, const string &mode, vector<string> &ifs);
extern "C" int DelBond(const string &bond);
extern "C" int SetBondPara(const string &bond, const string &para,const string &value);
extern "C" int AddBondSlave(const string &bond, const string &slave);
extern "C" int DelBondSlave(const string &bond, const string &slave);
extern "C" int GetBondSlave(const string &bond, vector<string> &vecslave);
 
////////////////////////////////////////////////////////////////////////////////
/////////////////ovs bonding��ؽӿ�////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
extern "C" int GetAllBondingsOVS(vector<string>& bondings);
extern "C" int GetBondModeOVS(const string &bond,string& mode);
extern "C" int GetBondSlavesOVS(const string &bond, vector<string> &vecslaves);
extern "C" int AddBondOVS(const string &bridge,const string &bond, const string &mode, vector<string> &ifs);
extern "C" int DelBondOVS(const string &bond);
extern "C" int RenameDVS(const string &oldbr,const string &newbr,const string &bond_name);
extern "C" int IsPortInBondOVS(const string& nic,string& bond);
extern "C" int BackupInfo(const string& bond, string &active, vector<string> &others);
extern "C" int LacpInfo(const string &bond, int &state,string &aggregate_id, string &partner_mac,
                   vector<string>  &suc_nics,vector<string> &fail_nics);
extern "C" int GetPhyPortFromBrOVS(const string &strBrName,string &phyport);
extern "C" int SetBrideForwordLLDP(const string &strBrName);

extern "C" int AddNetIfOVS(const string & strBr, const string & strNetIf);

////////////////////////////////////////////////////////////////////////////////
/////////////////SRIOV��ؽӿ�//////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
extern "C" int GetAllSriovPort(vector<string>& sriovs);
extern "C" int GetSriovPci(const string &nicname, map<string,string>&vif_pci);
extern "C" int GetSriovNum(const string &nicname, int &num);
extern "C" int SetVFPortMac(const string &port_name, const int &vfnum, const string &mac);
extern "C" int SetVFPortVlan(const string &port_name, const int &vfnum, const int &vlan);
extern "C" int GetVFPortMac(const string &port_name, const int &vfnum,  string &mac);
extern "C" int GetVFPortVlan(const string &port_name, const int &vfnum, int &vlan);

////////////////////////////////////////////////////////////////////////////////
/////////////////LOOPBACK����ؽӿ�//////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
extern "C" int GetAllLoopPort(vector<string>& loops);
extern "C" int IsLoopDevice(const string &port_name);
extern "C" int GetPortLoopbackMode(const string &port_name, int &lpflag);
extern "C" int SetPorLoopback(const string &port_name, const uint32 &lpflag);
extern "C" int CreateLoopBackPort(const string &port_name);
extern "C" int DeleteLoopBackPort(const string &port_name);

////////////////////////////////////////////////////////////////////////////////
/////////////////KERNEL����ؽӿ�//////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
extern "C" int GetAllKernelPort(vector<string>& kernels);
extern "C" int CreateKernelPort(const string &port_name, const string &switch_name, const string &uplinkport, 
                                const string &ip, const string &mask, const uint32 is_dhcp,const uint32 vlan, const uint32 mtu);
extern "C" int ModifyKernelPort(const string &port_name, const string &switch_name, const string &uplinkport, const string &ip, const string &mask);
extern "C" int DeleteKernelPort(const string &switch_name, const string &port_name);
extern "C" int GetKernelPortInfo(const string &port_name, string &ip, string &mask);

////////////////////////////////////////////////////////////////////////////////
/////////////////trunk/accsee��ؽӿ�//////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
extern "C" int SetPorVlanMode(const string &port_name, const uint32 &vlanflag);
extern "C" int RemovePorVlanMode(const string &port_name, const uint32 &vlanflag);
extern "C" int GetPortVlanMode(const string &port_name,  uint32 &vlanflag);

extern "C" int SetVlanAccess(const string &port_name, const uint32 &accessvlan);
extern "C" int GetVlanAccess(const string &port_name,  uint32 &accessvlan); 

extern "C" int SetVlanNativeForTrunk(const string &port_name,  const uint32 &nativevlan);

extern "C" int SetVlanTrunk(const string &port_name, map<uint32,uint32>trunks);
extern "C" int GetVlanTrunks(const string &port_name,  vector<uint32>vlan);

extern "C" int TrunkWork(const string &port_name, map<uint32,uint32>trunks, const uint32 &nativevlan);
extern "C" int AccessWork(const string &port_name, const uint32 &access_vlan);

extern "C" int SetEvbUplinkFlow(const string bridge, const string vmMac, const uint32 vmPort,const uint32 UplinkPort, const uint32 TagVlan);
extern "C" int DelEvbUplinkFlow(const string bridge, const string vmMac);
extern "C" int SetEvbDownlinkFlow(const string bridge, const string vmPort,const string vmMac, const string otherPorts,const uint32  UplinkPort);
extern "C" int DelEvbDownlinkFlow(const string bridge, const string vmMac);

extern "C" int GetVxlanSupport(bool &bVxlanStatus);

#endif

