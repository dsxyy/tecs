/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�vnet_ovs_plug.h
* �ļ���ʶ��
* ����ժҪ��vnet_ovs_plug��Ķ����ļ�
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
#ifndef __VNET_OVS_PUGIN_H
#define __VNET_OVS_PUGIN_H

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

#define VNET_PLUGIN_ERROR (-1)
#define VNET_PLUGIN_SUCCESS (0)
#define VNET_PLUGIN_FALSE (0)
#define VNET_PLUGIN_TRUE (1)
#define VNET_PLUGIN_MAX_VLAN_ID (4094)



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
#define VNET_BRIDGE_SCRIPTS        "/usr/local/bin/tecs/network-scripts/vnet_dobridge.sh"
#define VNET_BRIDGE_SCRIPTS_OVS    "/usr/local/bin/tecs/network-scripts/vnet_dobridge_ovs.sh"
#define VNET_BONDS_SCRIPTS         "/usr/local/bin/tecs/network-scripts/do_bond.sh"

#define VNET_CREATE_BRIDGE    "create_bridge"
#define VNET_DELETE_BRIDGE    "delete_bridge"
#define VNET_ADD_IF           "add_ifnet"
#define VNET_DEL_IF           "del_ifnet"
#define VNET_ADD_VLAN         "add_vlan"
#define VNET_DEL_VLAN         "del_Vlan"
#define VNET_ADD_BOND         "add_bond"
#define VNET_DEL_BOND         "del_bond"
#define VNET_SET_BOND         "set_bond"

////////////////////////////////////////////////////////////////////////////////
////////////////linux�������ַ�����ȹ�����ؽӿ�///////////////////////////////
////////////////////////////////////////////////////////////////////////////////
int vnet_chk_dir(const string &strDir);
int vnet_chk_dev(const string &strDevName);
int vnet_system(const string &strCmd);
string int2string(const int& intvar);
int removeendl(string &s);
int put_string_to_vec(string text,vector<string>& vec);
int RunCmd(const string& cmd, vector<string>& result);
int SendIOCtrl(const char* ifname, struct vnet_lib_private_value *pval);

////////////////////////////////////////////////////////////////////////////////
/////////////////������ؽӿ�///////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
int GetAllNic(vector<string>& nics);
int GetAllVirNic(vector<string>& nics);
int GetAllVnics(vector<string>& nics);
int GetAllUplinks(vector<string>& uplinks);
int GetAllUplinksSubnets(const string eth, vector<string>& up_subnets);
int DelVnicOfOVS(const string& nic);
int IfconfigIsExist(const string& nic);
int NicIsInOVS(const string& nic);
int SetMtu(const string& nic,const int& mtu);
int GetMtu(const char* lpszEth, unsigned int &mtu);
int GetNicAdminStatus(const string strNic, int &nStatus);
int SetNicAdminStatus(const string strNic, int nStatus);
int GetNicStatus(const string nics,int & status);
int SetVlan(const string& nic,uint32 dwVlanID);
int RemoveVlan(const string& nic,uint32 dwVlanID);

////////////////////////////////////////////////////////////////////////////////
/////////////////������ؽӿ�///////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
int GetAllBridges(vector<string>& bridges);
int GetIfsOfBridge(const string& bridge, vector<string>& ifs);
int GetAllOfPortIdOfBridge(const string& bridge, map<string, uint32>& of_port_list);
int GetOfPortIdOfIf(const string& nic, uint32& of_port_id);
int GetIfListOfSameVlan(const uint32& vlan, vector<string>&ifs);
int GetBridgeOfIf(const string& nic, string& bridge);
int GetPhyPortFromBr(const string &strBrName,string&phyport);
int AddNetIf(const string& strBr,const string& strNetIf,  uint32 nVlanID = 0);
int DelNetIf(const string& strBr,const string& strNetIf,  uint32 nVlanID = 0);
int AddVxlanNetIf(const string& strBr,const string& strVxlanNetIf, const string& strKey, const string& strRemoteIP);
int DelVxlanNetIf(const string& strBr,const string& strVxlanNetIf);
int CreateBridge(const string & strBr);
int CreateBridge(const string& strBr,const string& strNetIf,  uint32 nVlanID = 0);
int DeleteBridge(const string & strBr);  
int DeleteBridge(const string& strBr,const string& strNetIf,  uint32 nVlanID = 0);
int SetSdnController(const string& strBr, const string& strProto, const string& strIp, uint32 nPort);
int DelSdnController(const string& strBr);
int SetExternalid(const string& strBr, const string& strPort, const string& strPortUuid, const string& strMac, const string& strVmUuid);
int DelExternalid(const string& strBr, const string& strPort);

////////////////////////////////////////////////////////////////////////////////
/////////////////bonding��ؽӿ�////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
int IsSupportBond(int &isSupport);
int GetAllBondings(vector<string>& bondings);
int GetBondPara(const string &bond,const string &para,string& value);
int AddBond(const string &bond, const string &mode, vector<string> &ifs);
int DelBond(const string &bond);
int SetBondPara(const string &bond, const string &para,const string &value);

////////////////////////////////////////////////////////////////////////////////
/////////////////SRIOV��ؽӿ�//////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
int GetAllSriovPort(vector<string>& sriovs);
int GetSriovPci(const string &nicname, map<string,string>&vif_pci);
int GetSriovNum(const string &nicname, int &num);

////////////////////////////////////////////////////////////////////////////////
/////////////////LOOPBACK����ؽӿ�//////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
int IsLoopDevice(const string &port_name);
int GetPortLoopbackMode(const string &port_name, int &lpflag);
int SetPorLoopback(const string &port_name, const uint32 &lpflag);


int SetEvbUplinkFlow(const string bridge, const string vmMac, const uint32 vmPort,const uint32 UplinkPort, const uint32 TagVlan);
int DelEvbUplinkFlow(const string bridge, const string vmMac);
int SetEvbDownlinkFlow(const string bridge, const string vmPort,const string vmMac, const string otherPorts,const uint32  UplinkPort);
int DelEvbDownlinkFlow(const string bridge, const string vmMac);

int GetVxlanSupport(bool &bVxlanStatus);


#endif




