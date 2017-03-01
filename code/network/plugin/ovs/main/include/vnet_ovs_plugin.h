/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：vnet_ovs_plug.h
* 文件标识：
* 内容摘要：vnet_ovs_plug类的定义文件
* 当前版本：1.0
* 作    者：
* 完成日期：2012年12月16日
*
* 修改记录1：
*     修改日期：2012/12/16
*     版 本 号：V1.0
*     修 改 人：chenzhiwei
*     修改内容：创建
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
////////////////ioctl命令，不能和内核命令冲突(内核私有命令为0x89f0-0x89ff)/////
////////////////////////////////////////////////////////////////////////////////
#define SIOCDEVPRIINFO   0x89FE  
#define PRIVATE_SETLOOP  0x01      /* 用于设置网口环回模式 */
#define PRIVATE_GETLOOP  0x02      /* 用于查询网口环回模式 */
#define PRIVATE_SETVF    0x03      /* 用于设置网口VF data(=1开启; =0关闭) */
#define PRIVATE_GETVF    0x04      /* 用于查询网口VF data(=1开启; =0关闭) */

////////////////////////////////////////////////////////////////////////////////
//////PRIVATE_SETVF  PRIVATE_GETVF 命令Data Value///////////////////////////////
//////SIOCDEVPRIINFO命令对应的私有结构//////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#define PRIVATE_VF_VALUE_CLOSE 0x0
#define PRIVATE_VF_VALUE_OPEN  0x01
struct vnet_lib_private_value{
    unsigned int cmd;
    unsigned int data;
};

////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////端口状态/////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#define  PORT_LOOPBACK_NO      (int)0        // 非自环模式
#define  PORT_LOOPBACK_YES     (int)1        // 自环模式

//////////////////////////////////////////////////////////////////////////
//                           VNET脚本参数
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
////////////////linux操作，字符处理等公共相关接口///////////////////////////////
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
/////////////////网卡相关接口///////////////////////////////////////////////////
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
/////////////////网桥相关接口///////////////////////////////////////////////////
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
/////////////////bonding相关接口////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
int IsSupportBond(int &isSupport);
int GetAllBondings(vector<string>& bondings);
int GetBondPara(const string &bond,const string &para,string& value);
int AddBond(const string &bond, const string &mode, vector<string> &ifs);
int DelBond(const string &bond);
int SetBondPara(const string &bond, const string &para,const string &value);

////////////////////////////////////////////////////////////////////////////////
/////////////////SRIOV相关接口//////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
int GetAllSriovPort(vector<string>& sriovs);
int GetSriovPci(const string &nicname, map<string,string>&vif_pci);
int GetSriovNum(const string &nicname, int &num);

////////////////////////////////////////////////////////////////////////////////
/////////////////LOOPBACK口相关接口//////////////////////////////////////////////////
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




