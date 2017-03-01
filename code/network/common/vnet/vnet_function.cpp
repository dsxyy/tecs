/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：vnet_function.cpp
* 文件标识：
* 内容摘要：vnet_function的函数定义
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

#include "vnet_function.h"
#include "fcntl.h"

#define CHECK_VNETLIB_HANDLE(function)  \
        if(!vnetlib_handle)   \
        { \
           if(-1 ==LoadVnetlib()) \
           {\
               cout<<"LoadVnetlib failed "<<function<<endl;\
               return -1; \
           }\
        } \
        dlerror();
      

#define CHECK_VNETLIB_ERROR(function) \
    const char *dlsym_error = dlerror();\
    if (dlsym_error) \
    {\
        cout << "Cannot load symbol: "<<function << dlsym_error <<endl;\
        return -1;\
    }\

#define PRINT_CALLING_FUNCTION(function)  \
	if(1 == dug_para) \
	{\
	    cout<<"Calling "<<function<<endl; \
	}


void* vnetlib_handle;
int   dug_para =0;

int VNetRunCmd(const string& cmd, vector<string>& result)
{
    return RunCmd(cmd,result);
}

int RunCmd(const string& cmd, vector<string>& result)
{
    char    s[1024] = {0};
    FILE    *fp = NULL; 
    string  str;
    if(cmd.empty())
    {
        return -1;
    }
   
    if((fp = popen(cmd.c_str(), "r")) == NULL)
    {
        return -1;
    }

    // 避免FD的继承
    fcntl(fileno(fp), F_SETFD, FD_CLOEXEC);
    while(fgets(s, 1024, fp))
    {   
        str = s;       
        boost::trim(str);//boost库函数，处理后str中无回车符
        if(!str.empty())
        {
            result.push_back(str);      
        }
    }
    pclose(fp);
    return 0;
}


int LoadVnetlib()
{
    string cmd("lsmod | grep openvswitch  -c");

    vector<string> vResult;
    vector<string>::iterator it;
    if( 0 != RunCmd(cmd,vResult ))
    {
        cout<<"RunCmd "<<cmd<<" failed"<<endl; 
        return -1;;
    }
    if( vResult.empty())
    {
        return -1;
    }
    it = vResult.begin();

    int num = atoi((*it).c_str());
    if( 0 == num )
    {
        vnetlib_handle = dlopen("/opt/tecs/network/vna/lib/libbr.so", RTLD_LAZY);
    }
    else
    {
        vnetlib_handle = dlopen("/opt/tecs/network/vna/lib/libovs.so", RTLD_LAZY);
    }     
    
   if(!vnetlib_handle)
   {
       cout<<"load lib failed"<<endl;
       return -1;
   }
   return 0;
}

////////////////////////////////////////////////////////////////////////////////
/////////////////网卡相关接口///////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
typedef int (*GetAllNic_t)(vector<string>& nics);
typedef int (*GetAllVirNic_t)(vector<string>& nics);
typedef int (*GetAllVnics_t)(vector<string>& nics);
typedef int (*GetAllUplinks_t)(vector<string>& uplinks);
typedef int (*GetAllUplinksSubnets_t)(const string eth, vector<string>& up_subnets);
typedef int (*DelVnicOfOVS_t)(const string& nic);
typedef int (*IfconfigIsExist_t)(const string& nic);
typedef int (*NicIsInOVS_t)(const string& nic);
typedef int (*SetMtu_t)(const string& nic,const int& mtu);
typedef int (*GetMtu_t)(const char* lpszEth, unsigned int &mtu);
typedef int (*SetNicAdminStatus_t)(const string strNic, int nStatus);
typedef int (*GetNicAdminStatus_t)(const string strNic, int &nStatus);
typedef int (*GetNicStatus_t)(const string nics,int & status);
typedef int (*SetVlan_t)(const string& nic,uint32 dwVlanID);
typedef int (*RemoveVlan_t)(const string& nic,uint32 dwVlanID);

typedef int (*VNetIsPortInBond_t)(const string& nic,string& bond);
typedef int (*VNetIsPortInBridge_t)(const string& nic,string& bridge);

typedef int (*VNetGetPortNameForKVM_t)(const string& portid,string& portname);


////////////////////////////////////////////////////////////////////////////////
/////////////////网桥相关接口///////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
typedef int (*GetAllBridges_t)(vector<string>& bridges);
typedef int (*GetIfsOfBridge_t)(const string& bridge, vector<string>& ifs);
typedef int (*GetAllOfPortIdOfBridge_t)(const string& bridge, map<string, uint32>& of_port_list);
typedef int (*GetOfPortIdOfIf_t)(const string& nic, uint32& of_port_id);
typedef int (*GetIfListOfSameVlan_t)(const uint32& vlan, vector<string>&ifs);
typedef int (*GetBridgeOfIf_t)(const string& nic, string& bridge);
typedef int (*GetRealIfNameFromBridge_t)(string & port, string& rel_name);
typedef int (*GetPhyPortFromBr_t)(const string &strBrName,string& phyport);
typedef int (*AddNetIf_t)(const string& strBr,const string& strNetIf,  uint32 nVlanID);
typedef int (*DelNetIf_t)(const string& strBr,const string& strNetIf,  uint32 nVlanID);
typedef int (*AddVxlanNetIf_t)(const string& strBr,const string& strVxlanNetIf, const string& strKey, const string& strRemoteIP);
typedef int (*DelVxlanNetIf_t)(const string& strBr,const string& strVxlanNetIf);

typedef int (*DelNetIfOVS_t)(const string& strBr,const string& strNetIf);

typedef int (*CreateBridge_t)(const string & strBr);
typedef int (*CreateBridge_withif_t)(const string& strBr,const string& strNetIf,  uint32 nVlanID );
typedef int (*DeleteBridge_t)(const string & strBr);  
typedef int (*DeleteBridge_withif_t)(const string& strBr,const string& strNetIf,  uint32 nVlanID);
typedef int (*SetSdnController_t)(const string& strBr, const string& nProto, const string& strIp, uint32 nPort);
typedef int (*DelSdnController_t)(const string& strBr);
typedef int (*SetExternalid_t)(const string& strBr, const string& strPort, const string& strPortUuid, const string& strMac, const string& strVmUuid);
typedef int (*DelExternalid_t)(const string& strBr, const string& strPort);

////////////////////////////////////////////////////////////////////////////////
/////////////////bonding相关接口////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
typedef int (*IsSupportBond_t)(int &isSupport);
typedef int (*GetAllBondings_t)(vector<string>& bondings);
typedef int (*GetBondPara_t)(const string &bond,const string &para,string& value);
typedef int (*AddBond_t)(const string &bond, const string &mode, vector<string> &ifs);
typedef int (*DelBond_t)(const string &bond);
typedef int (*SetBondPara_t)(const string &bond, const string &para,const string &value);
typedef int (*AddBondSlave_t)(const string &bond, const string &slave);
typedef int (*DelBondSlave_t)(const string &bond, const string &slave);
typedef int (*GetBondSlaves_t)(const string &bond, vector<string> &vecslaves);
typedef int (*RenameDVS_t)(const string &oldbr,const string &newbr,const string &bond_name);

typedef int (*LacpInfo_t)(const string &bond, int &state,string &aggregate_id, string &partner_mac,
                   vector<string>  &suc_nics,vector<string> &fail_nics);
typedef int (*BackupInfo_t)(const string& bond, string &active, vector<string> &others);

typedef int (*GetPhyPortFromBrOVS_t)(const string &strBrName,string &phyport);
typedef int (*SetBrideForwordLLDP_t)(const string &strBrName);

////////////////////////////////////////////////////////////////////////////////
/////////////////ovs bonding相关接口////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
typedef int (*GetAllBondingsOVS_t)(vector<string>& bondings);
typedef int (*GetBondModeOVS_t)(const string &bond,string& value);
typedef int (*AddBondOVS_t)(const string &bridge,const string &bond, const string &mode, vector<string> &ifs);
typedef int (*DelBondOVS_t)(const string &bond);
typedef int (*GetBondSlavesOVS_t)(const string &bond, vector<string> &vecslaves);

typedef int (*IsPortInBondOVS_t)(const string& nic,string& bond);

typedef int (*AddNetIfOVS_t)(const string & strBr, const string & strNetIf);

////////////////////////////////////////////////////////////////////////////////
/////////////////SRIOV相关接口//////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
typedef int (*GetAllSriovPort_t)(vector<string>& sriovs);
typedef int (*GetSriovPci_t)(const string &nicname, map<string,string>&vif_pci);
typedef int (*GetSriovNum_t)(const string &nicname, int &num);

typedef int (*SetVFPortMac_t)(const string &port_name, const int &vfnum, const string &mac);
typedef int (*SetVFPortVlan_t)(const string &port_name, const int &vfnum, const int &vlan);

typedef int (*GetVFPortMac_t)(const string &port_name, const int &vfnum,  string &mac);
typedef int (*GetVFPortVlan_t)(const string &port_name, const int &vfnum,  int &vlan);

////////////////////////////////////////////////////////////////////////////////
/////////////////LOOPBACK口相关接口//////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
typedef int (*IsLoopDevice_t)(const string &port_name);
typedef int (*GetPortLoopbackMode_t)(const string &port_name, int &lpflag);
typedef int (*SetPorLoopback_t)(const string &port_name, const uint32 &lpflag);
typedef int (*CreateLoopBackPort_t)(const string &port_name);
typedef int (*DeleteLoopBackPort_t)(const string &port_name);
typedef int (*GetAllLoopBackPort_t)(vector<string>& loops);

////////////////////////////////////////////////////////////////////////////////
/////////////////KERNEL口相关接口//////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
typedef int (*CreateKernelPort_t)(const string &port_name, const string &switch_name, const string &uplinkport, const string &ip, const string &mask, const uint32 is_dhcp,const uint32 vlan, const uint32 mtu);
typedef int (*ModifyKernelPort_t)(const string &port_name, const string &switch_name, const string &uplinkport, const string &ip, const string &mask);
typedef int (*DeleteKernelPort_t)(const string &switch_name, const string &port_name);
typedef int (*GetAllKernelPort_t)(vector<string>& kernels);
typedef int (*GetKernelPortInfo_t)(const string &port_name, string &ip, string &mask);

////////////////////////////////////////////////////////////////////////////////
/////////////////trunk/accsee相关接口//////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
typedef int (*SetPorVlanMode_t)(const string &port_name, const uint32 &vlanflag);
typedef int (*RemovePorVlanMode_t)(const string &port_name, const uint32 &vlanflag);
typedef int (*GetPortVlanMode_t)(const string &port_name,  uint32 &vlanflag);

typedef int (*SetVlanAccess_t)(const string &port_name, const uint32 &accessvlan);
typedef int (*GetVlanAccess_t)(const string &port_name,  uint32 &accessvlan); 

typedef int (*VNetSetVlanNativeForTrunk_t)
            (const string &port_name, const uint32 &accessvlan);

typedef int (*SetVlanTrunk_t)(const string &port_name, map<uint32,uint32>trunks);
typedef int (*GetVlanTrunks_t)(const string &port_name,  vector<uint32>vlan);

typedef int (*VNetTrunkWork_t)(const string &port_name, map<uint32,uint32>trunks,
                                const uint32 &nativevlan);
typedef int (*VNetAccessWork_t)(const string &port_name, const uint32 &access_vlan);

typedef int (*VNetSetEvbUplinkFlow_t)(const string bridge, const string vmMac, const uint32 vmPort,const uint32 UplinkPort, const uint32 TagVlan);

typedef int (*VNetDelEvbUplinkFlow_t)(const string bridge, const string vmMac);

typedef int (*VNetSetEvbDownlinkFlow_t)(const string bridge, const string vmPort,const string vmMac, const string otherPorts,const uint32  UplinkPort);

typedef int (*VNetDelEvbDownlinkFlow_t)(const string bridge, const string vmMac);

typedef int (*VNetGetVxlanSupport_t)(bool &bVxlanStatus);

////////////////////////////////////////////////////////////////////////////////
/////////////////网卡相关接口///////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
int VNetGetAllNic(vector<string>& nics)
{
    string function = "VNetGetAllNic";
    CHECK_VNETLIB_HANDLE(function);
    
    GetAllNic_t GetAllNic_fun = (GetAllNic_t)dlsym(vnetlib_handle, "GetAllNic");
    CHECK_VNETLIB_ERROR(function);

    PRINT_CALLING_FUNCTION(function);
    return GetAllNic_fun(nics);
}

int VNetGetAllVirNic(vector<string>& nics)
{
    string function = "VNetGetAllVirNic";
    CHECK_VNETLIB_HANDLE(function);
    
    GetAllNic_t GetAllVirNic_fun = (GetAllNic_t)dlsym(vnetlib_handle, "GetAllVirNic");
    CHECK_VNETLIB_ERROR(function);

    PRINT_CALLING_FUNCTION(function);
    return GetAllVirNic_fun(nics);
}

int VNetGetAllVnics(vector<string>& nics)
{
    string function = "VNetGetAllVnics";
    CHECK_VNETLIB_HANDLE(function);
    
    GetAllVnics_t GetAllVnics_fun = (GetAllVnics_t)dlsym(vnetlib_handle, "GetAllVnics");
    CHECK_VNETLIB_ERROR(function);

    PRINT_CALLING_FUNCTION(function);
    return GetAllVnics_fun(nics);
}

int VNetGetAllUplinks(vector<string>& uplinks)
{
    string function = "VNetGetAllUplinks";
    CHECK_VNETLIB_HANDLE(function);
    
    GetAllUplinks_t GetAllUplinks_fun = (GetAllUplinks_t)dlsym(vnetlib_handle, "GetAllUplinks");
    CHECK_VNETLIB_ERROR(function);

    PRINT_CALLING_FUNCTION(function);
    return GetAllUplinks_fun(uplinks);
}

int VNetGetAllUplinksSubnets(const string eth, vector<string>& up_subnets)
{
    string function = "VNetGetAllUplinksSubnets";
    CHECK_VNETLIB_HANDLE(function);
    
    GetAllUplinksSubnets_t GetAllUplinksSubnets_fun = (GetAllUplinksSubnets_t)dlsym(vnetlib_handle, "GetAllUplinksSubnets");
    CHECK_VNETLIB_ERROR(function);

    PRINT_CALLING_FUNCTION(function);
    return GetAllUplinksSubnets_fun(eth, up_subnets);
}


int VNetDelVnicOfOVS(const string& nic)
{
    string function ="VNetDelVnicOfOVS";
    CHECK_VNETLIB_HANDLE(function);
    
    DelVnicOfOVS_t DelVnicOfOVS_fun = (DelVnicOfOVS_t)dlsym(vnetlib_handle, "DelVnicOfOVS");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return DelVnicOfOVS_fun(nic);
}

int VNetIfconfigIsExist(const string &nic)
{
    string function ="VNetIfconfigIsExist";
    CHECK_VNETLIB_HANDLE(function);
    
    IfconfigIsExist_t IfconfigIsExist_fun = (IfconfigIsExist_t)dlsym(vnetlib_handle, "IfconfigIsExist");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return IfconfigIsExist_fun(nic);
}

int VNetNicIsInOVS(const string &nic)
{
    string function ="VNetNicIsInOVS";
    CHECK_VNETLIB_HANDLE(function);
    
    NicIsInOVS_t NicIsInOVS_fun = (NicIsInOVS_t)dlsym(vnetlib_handle, "NicIsInOVS");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return NicIsInOVS_fun(nic);
}

int VNetSetMtu(const string& nic,const int& mtu)
{
    string function ="VNetSetMtu";
    CHECK_VNETLIB_HANDLE(function);
    
    SetMtu_t SetMtu_fun = (SetMtu_t)dlsym(vnetlib_handle, "SetMtu");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return SetMtu_fun(nic,mtu);
    
}
int VNetGetMtu(const char* lpszEth, unsigned int &mtu)
{
    string function ="VNetGetMtu";
    CHECK_VNETLIB_HANDLE(function);
    
    GetMtu_t GetMtu_fun = (GetMtu_t)dlsym(vnetlib_handle, "GetMtu");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return GetMtu_fun(lpszEth,mtu);
    
}

int VNetSetNicAdminStatus(const string strNic, int nStatus)
{
    string function ="VNetSetNicAdminStatus";
    CHECK_VNETLIB_HANDLE(function);
    
    SetNicAdminStatus_t SetNicAdminStatus_fun = (SetNicAdminStatus_t)dlsym(vnetlib_handle, "SetNicAdminStatus");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return SetNicAdminStatus_fun(strNic,nStatus);
}

int VNetGetNicAdminStatus(const string strNic, int &nStatus)
{
    string function ="VNetGetNicAdminStatus";
    CHECK_VNETLIB_HANDLE(function);
    
    GetNicAdminStatus_t GetNicAdminStatus_fun = (GetNicAdminStatus_t)dlsym(vnetlib_handle, "GetNicAdminStatus");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return GetNicAdminStatus_fun(strNic,nStatus);
}

int VNetGetNicStatus(const string nics,int & status)
{
    string function ="VNetGetNicStatus";
    CHECK_VNETLIB_HANDLE(function);
    
    GetNicStatus_t GetNicStatus_fun = (GetNicStatus_t)dlsym(vnetlib_handle, "GetNicStatus");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return GetNicStatus_fun(nics,status);
    
}

int VNetSetVlan(const string& nic,uint32 dwVlanID)
{
    string function ="VNetSetVlan";
    CHECK_VNETLIB_HANDLE(function);
    
    SetVlan_t SetVlan_fun = (SetVlan_t)dlsym(vnetlib_handle, "SetVlan");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return SetVlan_fun(nic,dwVlanID);
    
}

int VNetRemoveVlan(const string& nic,uint32 dwVlanID)
{
    string function ="VNetRemoveVlan";
    CHECK_VNETLIB_HANDLE(function);
    
    RemoveVlan_t RemoveVlan_fun = (RemoveVlan_t)dlsym(vnetlib_handle, "RemoveVlan");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return RemoveVlan_fun(nic,dwVlanID);
    
}

int VNetIsPortInBond(const string& nic,string& bond)
{
    string function ="VNetIsPortInBond";
    CHECK_VNETLIB_HANDLE(function);
    
    VNetIsPortInBond_t VNetIsPortInBond_fun = (VNetIsPortInBond_t)dlsym(vnetlib_handle, "VNetIsPortInBond");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return VNetIsPortInBond_fun(nic,bond);
    
}

int VNetIsPortInBridge(const string& nic,string& bridge)
{
    string function ="VNetIsPortInBridge";
    CHECK_VNETLIB_HANDLE(function);
    
    VNetIsPortInBridge_t VNetIsPortInBridge_fun = (VNetIsPortInBridge_t)dlsym(vnetlib_handle, "VNetIsPortInBridge");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return VNetIsPortInBridge_fun(nic,bridge);
}


int VNetGetPortNameForKVM(const string& portid,string& portname)
{
    string function ="VNetGetPortNameForKVM";
    CHECK_VNETLIB_HANDLE(function);
    
    VNetGetPortNameForKVM_t VNetGetPortNameForKVM_fun = (VNetGetPortNameForKVM_t)dlsym(vnetlib_handle, "GetPortNameForKVM");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return VNetGetPortNameForKVM_fun(portid,portname);
}


////////////////////////////////////////////////////////////////////////////////
/////////////////网桥相关接口///////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
int VNetGetAllBridges(vector<string>& bridges)
{
    string function ="VNetGetAllBridges";
    CHECK_VNETLIB_HANDLE(function);
    
    GetAllBridges_t  GetAllBridges_fun = (GetAllBridges_t)dlsym(vnetlib_handle, "GetAllBridges");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return GetAllBridges_fun(bridges);
}

int VNetGetIfsOfBridge(const string& bridge, vector<string>& ifs)
{
    string function ="VNetGetIfsOfBridge";
    CHECK_VNETLIB_HANDLE(function);
    
    GetIfsOfBridge_t GetIfsOfBridge_fun = (GetIfsOfBridge_t)dlsym(vnetlib_handle, "GetIfsOfBridge");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return GetIfsOfBridge_fun(bridge,ifs);
}

int VNetGetAllOfPortIdOfBridge(const string& bridge, map<string, uint32>& of_port_list)
{
    string function ="VNetGetAllOfPortIdOfBridge";
    CHECK_VNETLIB_HANDLE(function);
    
    GetAllOfPortIdOfBridge_t _fun = (GetAllOfPortIdOfBridge_t)dlsym(vnetlib_handle, "GetAllOfPortIdOfBridge");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return _fun(bridge,of_port_list);
}

int VNetGetOfPortIdOfIf(const string& nic, uint32& of_port_id)
{
    string function ="VNetGetOfPortIdOfIf";
    CHECK_VNETLIB_HANDLE(function);
    
    GetOfPortIdOfIf_t _fun = (GetOfPortIdOfIf_t)dlsym(vnetlib_handle, "GetOfPortIdOfIf");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return _fun(nic,of_port_id);
}

int VNetGetIfListOfSameVlan(const uint32& vlan, vector<string>&ifs)
{
    string function ="VNetGetIfListOfSameVlan";
    CHECK_VNETLIB_HANDLE(function);
    
    GetIfListOfSameVlan_t _fun = (GetIfListOfSameVlan_t)dlsym(vnetlib_handle, "GetIfListOfSameVlan");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return _fun(vlan,ifs);
}

int VNetGetBridgeOfIf(const string& nic, string& bridge)
{
    string function ="VNetGetBridgeOfIf";
    CHECK_VNETLIB_HANDLE(function);
    
    GetBridgeOfIf_t GetBridgeOfIf_fun = (GetBridgeOfIf_t)dlsym(vnetlib_handle, "GetBridgeOfIf");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return GetBridgeOfIf_fun(nic,bridge);
}

int VNetGetRealIfNameFromBridge(string & port, string& rel_name)
{
    string function ="VNetGetRealIfNameFromBridge";
    CHECK_VNETLIB_HANDLE(function);
    
    GetRealIfNameFromBridge_t GetRealIfNameFromBridge_fun = (GetRealIfNameFromBridge_t)dlsym(vnetlib_handle, "GetRealIfNameFromBridge");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return GetRealIfNameFromBridge_fun(port,rel_name);
}

int VNetGetPhyPortFromBr(const string &strBrName,string& phyport)
{
    string function ="VNetGetPhyPortFromBr";
    CHECK_VNETLIB_HANDLE(function);
    
    GetPhyPortFromBr_t GetPhyPortFromBr_fun = (GetPhyPortFromBr_t)dlsym(vnetlib_handle, "GetPhyPortFromBr");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return GetPhyPortFromBr_fun(strBrName,phyport);
}

int VNetAddNetIf(const string& strBr,const string& strNetIf,  uint32 nVlanID)
{
    string function ="VNetAddNetIf";
    CHECK_VNETLIB_HANDLE(function);
    
    AddNetIf_t AddNetIf_fun = (AddNetIf_t)dlsym(vnetlib_handle, "AddNetIf");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return AddNetIf_fun(strBr,strNetIf,nVlanID);
}

int VNetDelNetIf(const string& strBr,const string& strNetIf,  uint32 nVlanID)
{
    string function ="VNetDelNetIf";
    CHECK_VNETLIB_HANDLE(function);
    
    DelNetIf_t DelNetIf_fun = (DelNetIf_t)dlsym(vnetlib_handle, "DelNetIf");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return DelNetIf_fun(strBr,strNetIf,nVlanID);
}

int VNetAddVxlanNetIf(const string& strBr,const string& strVxlanNetIf, const string& strKey, const string& strRemoteIP)
{
    string function ="VNetAddVxlanNetIf";
    CHECK_VNETLIB_HANDLE(function);
    
    AddVxlanNetIf_t AddVxlanNetIf_fun = (AddVxlanNetIf_t)dlsym(vnetlib_handle, "AddVxlanNetIf");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return AddVxlanNetIf_fun(strBr,strVxlanNetIf,strKey,strRemoteIP);
}

int VNetDelVxlanNetIf(const string& strBr,const string& strVxlanNetIf)
{
    string function ="VNetDelVxlanNetIf";
    CHECK_VNETLIB_HANDLE(function);
    
    DelVxlanNetIf_t DelVxlanNetIf_fun = (DelVxlanNetIf_t)dlsym(vnetlib_handle, "DelVxlanNetIf");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return DelVxlanNetIf_fun(strBr,strVxlanNetIf);
}

int VNetDelNetIfOVS(const string& strBr,const string& strNetIf)
{
    string function ="VNetDelNetIfOVS";
    CHECK_VNETLIB_HANDLE(function);
    
    DelNetIfOVS_t DelNetIfOVS_fun = (DelNetIfOVS_t)dlsym(vnetlib_handle, "DelNetIfOVS");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return DelNetIfOVS_fun(strBr,strNetIf);
}

int VNetCreateBridge(const string & strBr)
{
    string function ="VNetCreateBridge";
    CHECK_VNETLIB_HANDLE(function);
    
    CreateBridge_t CreateBridge_fun = (CreateBridge_t)dlsym(vnetlib_handle, "CreateBridge");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return CreateBridge_fun(strBr);
}

int VNetCreateBridge_withif(const string& strBr,const string& strNetIf,  uint32 nVlanID )
{
    string function ="VNetCreateBridge_withif";
    CHECK_VNETLIB_HANDLE(function);
    
    CreateBridge_withif_t CreateBridge_withif_fun = (CreateBridge_withif_t)dlsym(vnetlib_handle, "CreateBridge_withif");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return CreateBridge_withif_fun(strBr,strNetIf,nVlanID);
}

int VNetDeleteBridge(const string & strBr)
{
    string function ="VNetDeleteBridge";
    CHECK_VNETLIB_HANDLE(function);
    
    DeleteBridge_t DeleteBridge_fun = (DeleteBridge_t)dlsym(vnetlib_handle, "DeleteBridge");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return DeleteBridge_fun(strBr);
}

int VNetDeleteBridge_withif(const string& strBr,const string& strNetIf,  uint32 nVlanID )
{
    string function ="VNetDeleteBridge_withif";
    CHECK_VNETLIB_HANDLE(function);
    
    DeleteBridge_withif_t DeleteBridge_withif_fun = (DeleteBridge_withif_t)dlsym(vnetlib_handle, "DeleteBridge_withif");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return DeleteBridge_withif_fun(strBr,strNetIf,nVlanID);
}

int VNetSetSdnController(const string& strBr, const string& strProto, const string& strIp, uint32 nPort)
{
    string function ="VNetSetSdnController";
    CHECK_VNETLIB_HANDLE(function);
    
    SetSdnController_t SetSdnController_fun = (SetSdnController_t)dlsym(vnetlib_handle, "SetSdnController");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return SetSdnController_fun(strBr, strProto, strIp, nPort);
}

int VNetDelSdnController(const string& strBr)
{
    string function ="VNetDelSdnController";
    CHECK_VNETLIB_HANDLE(function);
    
    DelSdnController_t DelSdnController_fun = (DelSdnController_t)dlsym(vnetlib_handle, "DelSdnController");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return DelSdnController_fun(strBr);
}

int VNetSetExternalid(const string& strBr, const string& strPort, const string& strPortUuid, const string& strMac, const string& strVmUuid)
{
    string function ="VNetSetExternalid";
    CHECK_VNETLIB_HANDLE(function);
    
    SetExternalid_t SetExternalid_fun = (SetExternalid_t)dlsym(vnetlib_handle, "SetExternalid");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return SetExternalid_fun(strBr, strPort, strPortUuid, strMac, strVmUuid);
}

int VNetDelExternalid(const string& strBr, const string& strPort)
{
    string function ="VNetDelExternalid";
    CHECK_VNETLIB_HANDLE(function);
    
    DelExternalid_t DelExternalid_fun = (DelExternalid_t)dlsym(vnetlib_handle, "DelExternalid");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return DelExternalid_fun(strBr, strPort);
}

////////////////////////////////////////////////////////////////////////////////
/////////////////bonding相关接口////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
int VNetIsSupportBond(int &isSupport)
{
    string function ="VNetIsSupportBond";
    CHECK_VNETLIB_HANDLE(function);
    
    IsSupportBond_t IsSupportBond_fun = (IsSupportBond_t)dlsym(vnetlib_handle, "IsSupportBond");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return IsSupportBond_fun(isSupport);
}

int VNetGetAllBondings(vector<string>& bondings)
{
    string function ="VNetGetAllBondings";
    CHECK_VNETLIB_HANDLE(function);
    
    GetAllBondings_t GetAllBondings_fun = (GetAllBondings_t)dlsym(vnetlib_handle, "GetAllBondings");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return GetAllBondings_fun(bondings);
}

int VNetLacpInfo(const string &bond, int &state,string &aggregate_id, string &partner_mac,
                   vector<string>  &suc_nics,vector<string> &fail_nics)
{
    string function ="VNetLacpInfo";
    CHECK_VNETLIB_HANDLE(function);
    
    LacpInfo_t GetLacpInfo_fun = (LacpInfo_t)dlsym(vnetlib_handle, "LacpInfo");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return GetLacpInfo_fun(bond, state,aggregate_id, partner_mac,suc_nics,fail_nics);
}

int VNetGetPhyPortFromBrOVS(const string &strBrName,string &phyport)
{
    string function ="VNetGetPhyPortFromBrOVS";
    CHECK_VNETLIB_HANDLE(function);
    
    GetPhyPortFromBrOVS_t GetPhyPortFromBrOVS_fun = (GetPhyPortFromBrOVS_t)dlsym(vnetlib_handle, "GetPhyPortFromBrOVS");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return GetPhyPortFromBrOVS_fun(strBrName,phyport);
}


int VNetSetBrideForwordLLDP(const string &strBrName)
{
    string function ="VNetSetBrideForwordLLDP";
    CHECK_VNETLIB_HANDLE(function);
    
    SetBrideForwordLLDP_t SetBrideForwordLLDP_fun = (SetBrideForwordLLDP_t)dlsym(vnetlib_handle, "SetBrideForwordLLDP");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return SetBrideForwordLLDP_fun(strBrName);
}

int VNetBackupInfo(const string &bond, string &active, vector<string> &others)
{
    string function ="VNetBackupInfo";
    CHECK_VNETLIB_HANDLE(function);
    
    BackupInfo_t BackupInfo_fun = (BackupInfo_t)dlsym(vnetlib_handle, "BackupInfo");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return BackupInfo_fun(bond, active,others);
}


int VNetAddNetIfOVS(const string & strBr, const string & strNetIf)
{
    string function ="VNetAddNetIfOVS";
    CHECK_VNETLIB_HANDLE(function);
    
    AddNetIfOVS_t AddNetIfOVS_fun = (AddNetIfOVS_t)dlsym(vnetlib_handle, "AddNetIfOVS");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return AddNetIfOVS_fun(strBr,strNetIf);
}


int VNetGetBondPara(const string &bond,const string &para,string& value)
{
    string function ="VNetGetBondPara";
    CHECK_VNETLIB_HANDLE(function);
    
    GetBondPara_t GetBondPara_fun = (GetBondPara_t)dlsym(vnetlib_handle, "GetBondPara");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return GetBondPara_fun(bond,para,value);
}

int VNetAddBond(const string &bond, const string &mode, vector<string> &ifs)
{
    string function ="VNetAddBond";
    CHECK_VNETLIB_HANDLE(function);
    
    AddBond_t AddBond_fun = (AddBond_t)dlsym(vnetlib_handle, "AddBond");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return AddBond_fun(bond,mode,ifs);
}

int VNetDelBond(const string &bond)
{
    string function ="VNetDelBond";
    CHECK_VNETLIB_HANDLE(function);
    
    DelBond_t DelBond_fun = (DelBond_t)dlsym(vnetlib_handle, "DelBond");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return DelBond_fun(bond);
}

int VNetSetBondPara(const string &bond, const string &para,const string &value)
{
    string function ="VNetSetBondPara";
    CHECK_VNETLIB_HANDLE(function);
    
    SetBondPara_t SetBondPara_fun = (SetBondPara_t)dlsym(vnetlib_handle, "SetBondPara");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return SetBondPara_fun(bond,para,value);
}

int VNetAddBondSlave(const string &bond, const string &slave)
{
    string function ="VNetAddBondSlave";
    CHECK_VNETLIB_HANDLE(function);
    
    AddBondSlave_t AddBondSlave_fun = (AddBondSlave_t)dlsym(vnetlib_handle, "AddBondSlave");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return AddBondSlave_fun(bond,slave);
}

int VNetDelBondSlave(const string &bond, const string &slave)
{
    string function ="VNetDelBondSlave";
    CHECK_VNETLIB_HANDLE(function);
    
    DelBondSlave_t DelBondSlave_fun = (DelBondSlave_t)dlsym(vnetlib_handle, "DelBondSlave");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return DelBondSlave_fun(bond,slave);
}

int VNetGetBondSlaves(const string &bond, vector<string> &vecslaves)
{
    string function ="VNetGetBondSlaves";
    CHECK_VNETLIB_HANDLE(function);
    
    GetBondSlaves_t GetBondSlaves_fun = (GetBondSlaves_t)dlsym(vnetlib_handle, "GetBondSlave");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return GetBondSlaves_fun(bond,vecslaves);
}

////////////////////////////////////////////////////////////////////////////////
/////////////////ovs bonding相关接口////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
int VNetGetAllBondingsOVS(vector<string>& bondings)
{
    string function ="VNetGetAllBondingsOVS";
    CHECK_VNETLIB_HANDLE(function);
    
    GetAllBondingsOVS_t GetAllBondingsOVS_fun = (GetAllBondingsOVS_t)dlsym(vnetlib_handle, "GetAllBondingsOVS");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return GetAllBondingsOVS_fun(bondings);
}

int VNetGetBondModeOVS(const string &bond,string& mode)
{
    string function ="VNetGetBondModeOVS";
    CHECK_VNETLIB_HANDLE(function);

    GetBondModeOVS_t GetBondModeOVS_fun = (GetBondModeOVS_t)dlsym(vnetlib_handle, "GetBondModeOVS");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return GetBondModeOVS_fun(bond,mode);
}

int VNetAddBondOVS(const string &bridge,const string &bond, const string &mode, vector<string> &ifs)
{
    string function ="VNetAddBondOVS";
    CHECK_VNETLIB_HANDLE(function);

    AddBondOVS_t AddBondOVS_fun = (AddBondOVS_t)dlsym(vnetlib_handle, "AddBondOVS");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return AddBondOVS_fun(bridge,bond,mode,ifs);
}

int VNetDelBondOVS(const string &bond)
{
    string function ="VNetDelBondOVS";
    CHECK_VNETLIB_HANDLE(function);

    DelBondOVS_t DelBondOVS_fun = (DelBondOVS_t)dlsym(vnetlib_handle, "DelBondOVS");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return DelBondOVS_fun(bond);
}

int VNetGetBondSlavesOVS(const string &bond, vector<string> &vecslaves)
{
    string function ="VNetGetBondSlaves";
    CHECK_VNETLIB_HANDLE(function);

    GetBondSlaves_t GetBondSlaves_fun = (GetBondSlaves_t)dlsym(vnetlib_handle, "GetBondSlavesOVS");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return GetBondSlaves_fun(bond,vecslaves);
}    

int VNetRenameDVS(const string &oldbr,const string &newbr,const string &bond_name)
{
    string function ="VNetRenameDVS";
    CHECK_VNETLIB_HANDLE(function);

    RenameDVS_t RenameDVS_fun = (RenameDVS_t)dlsym(vnetlib_handle, "RenameDVS");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return RenameDVS_fun(oldbr,newbr,bond_name);
}

int VNetIsPortInBondOVS(const string& nic,string& bond)
{
    string function ="VNetIsPortInBondOVS";
    CHECK_VNETLIB_HANDLE(function);

    IsPortInBondOVS_t IsPortInBondOVS_fun = (IsPortInBondOVS_t)dlsym(vnetlib_handle, "IsPortInBondOVS");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return IsPortInBondOVS_fun(nic,bond);
}

////////////////////////////////////////////////////////////////////////////////
/////////////////SRIOV相关接口//////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
int VNetGetAllSriovPort(vector<string>& sriovs)
{
    string function ="VNetGetAllSriovPort";
    CHECK_VNETLIB_HANDLE(function);
    
    GetAllSriovPort_t GetAllSriovPort_fun = (GetAllSriovPort_t)dlsym(vnetlib_handle, "GetAllSriovPort");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return GetAllSriovPort_fun(sriovs);
}


int VNetGetSriovPci(const string &nicname, map<string,string>&vif_pci)
{
    string function ="VNetGetSriovPci";
    CHECK_VNETLIB_HANDLE(function);
    
    GetSriovPci_t GetSriovPci_fun = (GetSriovPci_t)dlsym(vnetlib_handle, "GetSriovPci");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return GetSriovPci_fun(nicname,vif_pci);
}

int VNetGetSriovNum(const string &nicname, int &num)
{
    string function ="VNetGetSriovNum";
    CHECK_VNETLIB_HANDLE(function);
    
    GetSriovNum_t GetSriovNum_fun = (GetSriovNum_t)dlsym(vnetlib_handle, "GetSriovNum");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return GetSriovNum_fun(nicname,num);
}    

int VNetSetVFPortMac(const string &port_name, const int &vfnum, const string &mac)
{
    string function ="VNetSetVFPortMac";
    CHECK_VNETLIB_HANDLE(function);
    
    SetVFPortMac_t SetVFPortMac_fun = (SetVFPortMac_t)dlsym(vnetlib_handle, "SetVFPortMac");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return SetVFPortMac_fun(port_name,vfnum,mac);
}

int VNetSetVFPortVlan(const string &port_name, const int &vfnum, const int &vlan)
{
    string function ="VNetSetVFPortVlan";
    CHECK_VNETLIB_HANDLE(function);
    
    SetVFPortVlan_t SetVFPortVlan_fun = (SetVFPortVlan_t)dlsym(vnetlib_handle, "SetVFPortVlan");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return SetVFPortVlan_fun(port_name,vfnum,vlan);
}


int VNetGetVFPortMac(const string &port_name, const int &vfnum,  string &mac)
{
    string function ="VNetGetVFPortMac";
    CHECK_VNETLIB_HANDLE(function);
    
    GetVFPortMac_t GetVFPortMac_fun = (GetVFPortMac_t)dlsym(vnetlib_handle, "GetVFPortMac");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return GetVFPortMac_fun(port_name,vfnum,mac);
}

int VNetGetVFPortVlan(const string &port_name, const int &vfnum,  int &vlan)
{
    string function ="VNetGetVFPortVlan";
    CHECK_VNETLIB_HANDLE(function);
    
    GetVFPortVlan_t GetVFPortVlan_fun = (GetVFPortVlan_t)dlsym(vnetlib_handle, "GetVFPortVlan");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return GetVFPortVlan_fun(port_name,vfnum,vlan);
}

////////////////////////////////////////////////////////////////////////////////
/////////////////LOOPBACK口相关接口//////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
int VNetIsLoopDevice(const string &port_name)
{
    string function ="VNetIsLoopDevice";
    CHECK_VNETLIB_HANDLE(function);
    
    IsLoopDevice_t IsLoopDevice_fun = (IsLoopDevice_t)dlsym(vnetlib_handle, "IsLoopDevice");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return IsLoopDevice_fun(port_name);
}

int VNetGetPortLoopbackMode(const string &port_name, int &lpflag)
{
    string function ="VNetGetPortLoopbackMode";
    CHECK_VNETLIB_HANDLE(function);
    
    GetPortLoopbackMode_t GetPortLoopbackMode_fun = (GetPortLoopbackMode_t)dlsym(vnetlib_handle, "GetPortLoopbackMode");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return GetPortLoopbackMode_fun(port_name,lpflag);
}


int VNetSetPorLoopback(const string &port_name, const uint32 &lpflag)
{
    string function ="VNetSetPorLoopback";
    CHECK_VNETLIB_HANDLE(function);
    
    SetPorLoopback_t SetPorLoopback_fun = (SetPorLoopback_t)dlsym(vnetlib_handle, "SetPorLoopback");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return SetPorLoopback_fun(port_name,lpflag);
}

int VNetCreateLoopBackPort(const string &port_name)
{
    string function ="VNetCreateLoopBackPort";
    CHECK_VNETLIB_HANDLE(function);
    
    CreateLoopBackPort_t CreateLoopBackPort_fun = (CreateLoopBackPort_t)dlsym(vnetlib_handle, "CreateLoopBackPort");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return CreateLoopBackPort_fun(port_name);
}

int VNetDeleteLoopBackPort(const string &port_name)
{
    string function ="VNetDeleteLoopBackPort";
    CHECK_VNETLIB_HANDLE(function);
    
    DeleteLoopBackPort_t DeleteLoopBackPort_fun = (DeleteLoopBackPort_t)dlsym(vnetlib_handle, "DeleteLoopBackPort");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return DeleteLoopBackPort_fun(port_name);
}

int VNetGetAllLoopBackPort(vector<string>& loops)
{
    string function ="VNetGetAllLoopBackPort";
    CHECK_VNETLIB_HANDLE(function);
    
    GetAllLoopBackPort_t GetAllLoopBackPort_fun = (GetAllLoopBackPort_t)dlsym(vnetlib_handle, "GetAllLoopPort");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return GetAllLoopBackPort_fun(loops);
}

////////////////////////////////////////////////////////////////////////////////
/////////////////KERNEL口相关接口//////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
int VNetCreateKernelPort(const string &port_name, const string &switch_name, const string &uplinkport, const string &ip, const string &mask, const uint32 is_dhcp,const uint32 vlan, const uint32 mtu)
{
    string function ="VNetCreateKernelPort";
    CHECK_VNETLIB_HANDLE(function);
    
    CreateKernelPort_t CreateKernelPort_fun = (CreateKernelPort_t)dlsym(vnetlib_handle, "CreateKernelPort");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return CreateKernelPort_fun(port_name, switch_name, uplinkport, ip, mask, is_dhcp,vlan, mtu);
}

int VNetModifyKernelPort(const string &port_name, const string &switch_name, const string &uplinkport, const string &ip, const string &mask)
{
    string function ="VNetModifyKernelPort";
    CHECK_VNETLIB_HANDLE(function);
    
    ModifyKernelPort_t ModifyKernelPort_fun = (ModifyKernelPort_t)dlsym(vnetlib_handle, "ModifyKernelPort");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return ModifyKernelPort_fun(port_name, switch_name, uplinkport, ip, mask);
}

int VNetDeleteKernelPort(const string &switch_name, const string &port_name)
{
    string function ="VNetDeleteKernelPort";
    CHECK_VNETLIB_HANDLE(function);
    
    DeleteKernelPort_t DeleteKernelPort_fun = (DeleteKernelPort_t)dlsym(vnetlib_handle, "DeleteKernelPort");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return DeleteKernelPort_fun(switch_name, port_name);
}

int VNetGetAllKernelPort(vector<string>& kernels)
{
    string function ="VNetGetAllKernelPort";
    CHECK_VNETLIB_HANDLE(function);
    
    GetAllKernelPort_t GetAllKernelPort_fun = (GetAllKernelPort_t)dlsym(vnetlib_handle, "GetAllKernelPort");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return GetAllKernelPort_fun(kernels);
}

int VNetGetKernelPortInfo(const string &port_name, string &ip, string &mask)
{
    string function ="VNetGetKernelPortInfo";
    CHECK_VNETLIB_HANDLE(function);
    
    GetKernelPortInfo_t GetKernelPortInfo_fun = (GetKernelPortInfo_t)dlsym(vnetlib_handle, "GetKernelPortInfo");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return GetKernelPortInfo_fun(port_name, ip, mask);
}


////////////////////////////////////////////////////////////////////////////////
/////////////////trunk/accsee相关接口//////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
int VNetSetPorVlanMode(const string &port_name, const uint32 &vlanflag)
{
    string function ="VNetSetPorVlanMode";
    CHECK_VNETLIB_HANDLE(function);
    
    SetPorVlanMode_t SetPorVlanMode_fun = (SetPorVlanMode_t)dlsym(vnetlib_handle, "SetPorVlanMode");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return SetPorVlanMode_fun(port_name,vlanflag);
}

int VNetRemovePorVlanMode(const string &port_name, const uint32 &vlanflag)
{
    string function ="VNetRemovePorVlanMode";
    CHECK_VNETLIB_HANDLE(function);
    
    RemovePorVlanMode_t RemovePorVlanMode_fun = (RemovePorVlanMode_t)dlsym(vnetlib_handle, "RemovePorVlanMode");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return RemovePorVlanMode_fun(port_name,vlanflag);
}

int VNetGetPortVlanMode(const string &port_name,  uint32 &vlanflag)
{
    string function ="VNetGetPortVlanMode";
    CHECK_VNETLIB_HANDLE(function);
    
    GetPortVlanMode_t GetPortVlanMode_fun = (GetPortVlanMode_t)dlsym(vnetlib_handle, "GetPortVlanMode");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return GetPortVlanMode_fun(port_name,vlanflag);
}

int VNetSetVlanAccess(const string &port_name, const uint32 &accessvlan)
{
    string function ="VNetSetVlanAccess";
    CHECK_VNETLIB_HANDLE(function);
    
    SetVlanAccess_t SetVlanAccess_fun = (SetVlanAccess_t)dlsym(vnetlib_handle, "SetVlanAccess");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return SetVlanAccess_fun(port_name,accessvlan);
}

int VNetGetVlanAccess(const string &port_name,  uint32 &accessvlan)
{
    string function ="VNetGetVlanAccess";
    CHECK_VNETLIB_HANDLE(function);
    
    GetVlanAccess_t GetVlanAccess_fun = (GetVlanAccess_t)dlsym(vnetlib_handle, "GetVlanAccess");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return GetVlanAccess_fun(port_name,accessvlan);
}


int VNetSetVlanNativeForTrunk(const string &port_name, const uint32 &nativevlan)
{
    string function ="VNetSetVlanNativeForTrunk";
    CHECK_VNETLIB_HANDLE(function);
    
    VNetSetVlanNativeForTrunk_t VNetSetVlanNativeForTrunk_fun = 
              (VNetSetVlanNativeForTrunk_t)dlsym(vnetlib_handle, "SetVlanNativeForTrunk");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return VNetSetVlanNativeForTrunk_fun(port_name,nativevlan);
}

    
int VNetSetVlanTrunk(const string &port_name, map<uint32,uint32>trunks)
{
    string function ="VNetSetVlanTrunk";
    CHECK_VNETLIB_HANDLE(function);
    
    SetVlanTrunk_t SetVlanTrunk_fun = (SetVlanTrunk_t)dlsym(vnetlib_handle, "SetVlanTrunk");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return SetVlanTrunk_fun(port_name,trunks);
}

int VNetGetVlanTrunks(const string &port_name,  vector<uint32>vlan)
{
    string function ="VNetGetVlanTrunks";
    CHECK_VNETLIB_HANDLE(function);
    
    GetVlanTrunks_t GetVlanTrunks_fun = (GetVlanTrunks_t)dlsym(vnetlib_handle, "GetVlanTrunks");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return GetVlanTrunks_fun(port_name,vlan);
}

int VNetTrunkWork(const string &port_name, map<uint32,uint32>trunks, const uint32 &nativevlan)
{
    string function ="VNetTrunkWork";
    CHECK_VNETLIB_HANDLE(function);

    VNetTrunkWork_t VNetTrunkWork_fun = (VNetTrunkWork_t)dlsym(vnetlib_handle, "TrunkWork");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return VNetTrunkWork_fun(port_name,trunks,nativevlan);
}


int VNetAccessWork(const string &port_name, const uint32 &access_vlan)
{
    string function ="VNetAccessWork";
    CHECK_VNETLIB_HANDLE(function);

    VNetAccessWork_t VNetAccessWork_fun = (VNetAccessWork_t)dlsym(vnetlib_handle, "AccessWork");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return VNetAccessWork_fun(port_name,access_vlan);
}

int VNetSetEvbUplinkFlow(const string bridge, const string vmMac, const uint32 vmPort,const uint32 UplinkPort, const uint32 TagVlan)
{
    string function ="VNetSetEvbUplinkFlow";
    CHECK_VNETLIB_HANDLE(function);

    VNetSetEvbUplinkFlow_t _fun = (VNetSetEvbUplinkFlow_t)dlsym(vnetlib_handle, "SetEvbUplinkFlow");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return _fun(bridge, vmMac, vmPort,UplinkPort, TagVlan);
}

int VNetDelEvbUplinkFlow(const string bridge, const string vmMac)
{
    string function ="VNetDelEvbUplinkFlow";
    CHECK_VNETLIB_HANDLE(function);

    VNetDelEvbUplinkFlow_t _fun = (VNetDelEvbUplinkFlow_t)dlsym(vnetlib_handle, "DelEvbUplinkFlow");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return _fun(bridge, vmMac);
}

int VNetSetEvbDownlinkFlow(const string bridge, const string vmPort,const string vmMac, const string otherPorts,const uint32  UplinkPort)
{
    string function ="VNetSetEvbDownlinkFlow";
    CHECK_VNETLIB_HANDLE(function);

    VNetSetEvbDownlinkFlow_t _fun = (VNetSetEvbDownlinkFlow_t)dlsym(vnetlib_handle, "SetEvbDownlinkFlow");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return _fun(bridge, vmPort,vmMac, otherPorts, UplinkPort);
}

int VNetDelEvbDownlinkFlow(const string bridge, const string vmMac)
{
    string function ="VNetDelEvbDownlinkFlow";
    CHECK_VNETLIB_HANDLE(function);

    VNetDelEvbDownlinkFlow_t _fun = (VNetDelEvbDownlinkFlow_t)dlsym(vnetlib_handle, "DelEvbDownlinkFlow");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);
    return _fun(bridge, vmMac);
}

bool VNetIsVxlanSupport(void)
{
    string function ="VNetIsVxlanSupport";
    CHECK_VNETLIB_HANDLE(function);

    VNetGetVxlanSupport_t GetVxlanSupport_fun = (VNetGetVxlanSupport_t)dlsym(vnetlib_handle, "GetVxlanSupport");
    CHECK_VNETLIB_ERROR(function);
    PRINT_CALLING_FUNCTION(function);

    bool bVxlanStatus = false;
    GetVxlanSupport_fun(bVxlanStatus);
    return bVxlanStatus;
}


#if 0
int main()
{
    vector <string> allnics;
    int ret ;
    ret = VNetGetAllNic(allnics);
    if (ret != 0)
    {
        assert(0);
        return 1;
    }
    for(vector <string>::iterator itor = allnics.begin();itor !=allnics.end();itor++)
    {
        cout<<"all phyic nics is : "<<*itor<<endl;
    }

    allnics.clear();
    VNetGetAllVirNic(allnics);
    if (ret != 0)
    {
        assert(0);
        return 1;
    }
    for(vector <string>::iterator itor = allnics.begin();itor !=allnics.end();itor++)
    {
        cout<<"all virtual nics is : "<<*itor<<endl;
    }

    
    ret = VNetSetMtu("eth1",2400);
    if (ret != 0)
    {
        assert(0);
        return 1;
    }
    cout <<"success set eth1 mtu 2400";
    
     return 0;

    
}

#endif

