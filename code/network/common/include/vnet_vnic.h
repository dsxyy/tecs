/******************************************************************************
* Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：vnm_vsi_db.h
* 文件标识：
* 内容摘要：CVNetVSIDB类的定义文件
* 当前版本：1.0
* 作    者：
* 完成日期：2013年2月22日
*
* 修改记录1：
*     修改日期：2013/2/22
*     版 本 号：V1.0
*     修 改 人：lverchun
*     修改内容：创建
******************************************************************************/
#if !defined(VNET_VNIC_INCLUDE_H__)
#define VNET_VNIC_INCLUDE_H__
#include "vnetlib_msg.h"


namespace zte_tecs
{
class CTrunkVlanRange : public Serializable
{   
public:
    SERIALIZE
    {
        SERIALIZE_BEGIN(CTrunkVlanRange);
        WRITE_VALUE(m_nVlanBegin);
        WRITE_VALUE(m_nVlanEnd);
        SERIALIZE_END();
    };
    
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(CTrunkVlanRange);
        READ_VALUE(m_nVlanBegin);
        READ_VALUE(m_nVlanEnd);
        DESERIALIZE_END();
    };
        
public:
    int32 m_nVlanBegin;
    int32 m_nVlanEnd;
};

/*vsi_profile 
供VSI模块操作DB接口*/
class CVNetVSIProfile
{

public:
    CVNetVSIProfile(){m_nVSIIDFormat = 0x5;};

    string GetVsiIDValue() const {return m_strVSIIDValue;};
    void SetVsiIDValue(const string &strVSIIDValue){m_strVSIIDValue = strVSIIDValue;};
    int32 GetVsiIDFormat() const {return m_nVSIIDFormat;};
    void SetVsiIDFormat(int32 &nVSIIDFormat){m_nVSIIDFormat = nVSIIDFormat;};
    string GetPgUuid() const {return m_strVmPGID;};
    void SetPgUuid(const string strPGUuid){m_strVmPGID = strPGUuid;};
    string GetMac() const {return m_strMac;};
    void SetMac(const string strMac){m_strMac = strMac;};
    string GetIP() const {return m_strIp;};
    void SetIP(const string strIp){m_strIp = strIp;};
    string GetMask() const {return m_strMask;};
    void SetMask(const string strMask){m_strMask = strMask;};
    string GetVSwitchUuid() const {return m_strVSwitchUuid;};
    void SetVSwitchUuid(const string strVSwitchUuid){m_strVSwitchUuid = strVSwitchUuid;};
    string GetVfPortUuid() const {return m_strSriovVfPortUuid;};
    void SetVfPortUuid(const string strVfPortUuid){m_strSriovVfPortUuid = strVfPortUuid;};
    string GetQuantumNetworkUuid() const {return m_strQuantumNetworkUuid;};
    void SetQuantumNetworkUuid(const string strQuantumNetworkUuid){m_strQuantumNetworkUuid = strQuantumNetworkUuid;};
    string GetQuantumPortUuid() const {return m_strQuantumPortUuid;};
    void SetQuantumPortUuid(const string strQuantumPortUuid){m_strQuantumPortUuid = strQuantumPortUuid;};

   void Print()
    {
        cout << "CVNetVSIProfile: " << endl;
        cout << "    VSIIDValue              : " << m_strVSIIDValue << endl;
        cout << "    VSIIDFormat             : " << m_nVSIIDFormat << endl;
        cout << "    VmPGID                  : " << m_strVmPGID << endl;
        cout << "    Mac                     : " << m_strMac << endl;
        cout << "    Ip                      : " << m_strIp << endl;
        cout << "    NetMask                 : " << m_strMask << endl;
        cout << "    VSwitchUuid             : " << m_strVSwitchUuid << endl;
        cout << "    SriovVfPortUuid         : " << m_strSriovVfPortUuid << endl;
        cout << "    QuantumNetworkUuid      : " << m_strQuantumNetworkUuid << endl;
        cout << "    QuantumPortUuid         : " << m_strQuantumPortUuid << endl;
    };
   
private:
    string  m_strVSIIDValue;
    int32   m_nVSIIDFormat;
    string  m_strVmPGID;                   //PG UUID,由DB模块负责转换为oid
    string m_strMac;
    string m_strIp;
    string m_strMask;
    string m_strVSwitchUuid;
    string m_strSriovVfPortUuid;
    string m_strQuantumNetworkUuid;
    string m_strQuantumPortUuid;
};


class CVNetVSIProfileDetail: public Serializable
{
public:    
    string  m_strVSIIDValue;
    int32   m_nVSIIDFormat;
    string  m_strMgrID;
    string  m_strVSITypeID;
    int32  m_nVSITypeVersion;
    
    string  m_strPGUuid;
    int32  m_nPGType;
    string  m_strAcl;
    string  m_strQos;
    string  m_strAllowedPriorities;
    string  m_strReceiveBandwidthLimit;
    string  m_strReceiveBandwidthReserve;
    int32   m_nDefaultVlanID;
    int32   m_nPromiscuous;
    int32   m_nMacVlanFilterEnable;
    string  m_strAllowedTransmitMacs;
    string  m_strAllowedTransmitMacvlans;
    string  m_strPolicyBlockOverride;
    string  m_strPolicyVlanOverride;
    int32   m_nAllowMacChange;
    int32   m_nMTU;
    int32   m_nSwitchportMode;
    int32   m_nAccessVlan;
    int32   m_nAccessCvlan;
    vector<CTrunkVlanRange> m_vecTrunkVlan;
    int32   m_nNativeVlan;
    string   m_nIPAddress;
    string   m_nNetmask;
    string   m_nGateway;
    string  m_strMacAddress;
    string m_strVSwitchUuid;
    string m_strSriovVfPortUuid;
    string  m_strSwitchName;
    string  m_strPciOrder;
    string  m_strPortName;
    string  m_strNetplaneNickName;
    int32   m_strSegmentId;
    int32   m_nIsolateType;
    string  m_strQuantumNetWorkUuid;
    string  m_strQuantumPortUuid;

public:
    CVNetVSIProfileDetail()
    {
        m_strVSIIDValue = STR_VSIID_INIT_VALUE;
        m_nVSIIDFormat = 0x5 ;
        m_strMgrID = STR_MGRID_INIT_VALUE;
        m_strVSITypeID  = STR_VSITYPEID_INIT_VALUE;
        m_nVSITypeVersion = 0x1;
        m_nDefaultVlanID = 1;
        m_nPromiscuous = 0;
        m_nMacVlanFilterEnable = 0;
        m_nAllowMacChange = 0;
        m_nMTU = 1500;
        m_nSwitchportMode = 0;
        m_nAccessVlan = 1;
        m_nAccessCvlan = 1;
        m_nNativeVlan = 1;
        m_strNetplaneNickName = "";
        m_strSegmentId = 0;
        m_nIsolateType = ISOLATE_VLAN;
        m_strQuantumNetWorkUuid = "";
        m_strQuantumPortUuid = "";
    };

    SERIALIZE
    {
        SERIALIZE_BEGIN(CVNetVSIProfileDetail);
            WRITE_STRING(m_strVSIIDValue);
            WRITE_DIGIT(m_nVSIIDFormat);
            WRITE_STRING(m_strMgrID);
            WRITE_STRING(m_strVSITypeID);
            WRITE_DIGIT(m_nVSITypeVersion);
            WRITE_STRING(m_strPGUuid);
            WRITE_STRING(m_strAcl);
            WRITE_STRING(m_strQos);
            WRITE_STRING(m_strAllowedPriorities);
            WRITE_STRING(m_strReceiveBandwidthLimit);
            WRITE_STRING(m_strReceiveBandwidthReserve);
            WRITE_DIGIT(m_nDefaultVlanID);
            WRITE_DIGIT(m_nPromiscuous);
            WRITE_DIGIT(m_nMacVlanFilterEnable);
            WRITE_STRING(m_strAllowedTransmitMacs);
            WRITE_STRING(m_strAllowedTransmitMacvlans);
            WRITE_STRING(m_strPolicyBlockOverride);
            WRITE_STRING(m_strPolicyVlanOverride);
            WRITE_DIGIT(m_nAllowMacChange);
            WRITE_DIGIT(m_nMTU);
            WRITE_DIGIT(m_nSwitchportMode);
            WRITE_DIGIT(m_nAccessVlan);
            WRITE_DIGIT(m_nAccessCvlan);
            WRITE_OBJECT_VECTOR(m_vecTrunkVlan);
            WRITE_DIGIT(m_nNativeVlan);
            WRITE_STRING(m_nIPAddress);
            WRITE_STRING(m_nNetmask);
            WRITE_STRING(m_nGateway);
            WRITE_STRING(m_strMacAddress);
            WRITE_STRING(m_strSwitchName);
            WRITE_STRING(m_strPciOrder);
            WRITE_STRING(m_strPortName);
            WRITE_STRING(m_strNetplaneNickName);
            WRITE_STRING(m_strSegmentId);   
            WRITE_DIGIT(m_nIsolateType);
            WRITE_STRING(m_strQuantumNetWorkUuid);
            WRITE_STRING(m_strQuantumPortUuid);
        SERIALIZE_END();
    };
    
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(CVNetVSIProfileDetail);
            READ_STRING(m_strVSIIDValue);
            READ_DIGIT(m_nVSIIDFormat);
            READ_STRING(m_strMgrID);
            READ_STRING(m_strVSITypeID);
            READ_DIGIT(m_nVSITypeVersion);
            READ_STRING(m_strPGUuid);
            READ_STRING(m_strAcl);
            READ_STRING(m_strQos);
            READ_STRING(m_strAllowedPriorities);
            READ_STRING(m_strReceiveBandwidthLimit);
            READ_STRING(m_strReceiveBandwidthReserve);
            READ_DIGIT(m_nDefaultVlanID);
            READ_DIGIT(m_nPromiscuous);
            READ_DIGIT(m_nMacVlanFilterEnable);
            READ_STRING(m_strAllowedTransmitMacs);
            READ_STRING(m_strAllowedTransmitMacvlans);
            READ_STRING(m_strPolicyBlockOverride);
            READ_STRING(m_strPolicyVlanOverride);
            READ_DIGIT(m_nAllowMacChange);
            READ_DIGIT(m_nMTU);
            READ_DIGIT(m_nSwitchportMode);
            READ_DIGIT(m_nAccessVlan);
            READ_DIGIT(m_nAccessCvlan);
            READ_OBJECT_VECTOR(m_vecTrunkVlan);
            READ_DIGIT(m_nNativeVlan);
            READ_STRING(m_nIPAddress);
            READ_STRING(m_nNetmask);
            READ_STRING(m_nGateway);
            READ_STRING(m_strMacAddress);
            READ_STRING(m_strSwitchName);
            READ_STRING(m_strPciOrder);            
            READ_STRING(m_strPortName);
            READ_STRING(m_strNetplaneNickName);
            READ_STRING(m_strSegmentId);     
            READ_DIGIT(m_nIsolateType);
            READ_STRING(m_strQuantumNetWorkUuid);
            READ_STRING(m_strQuantumPortUuid);
        DESERIALIZE_END();
    };

    void Print()
    {
        cout << "VSI Profile Detail: " << endl;
        cout << "    VSIIDValue              : " << m_strVSIIDValue << endl;
        cout << "    VSIIDFormat             : " << m_nVSIIDFormat << endl;
        cout << "    MgrID                   : " << m_strMgrID << endl;
        cout << "    VSITypeID               : " << m_strVSITypeID << endl;
        cout << "    VSITypeVersion          : " << m_nVSITypeVersion << endl;
        cout << "    PGUuid                  : " << m_strPGUuid << endl;
        cout << "    ACL                     : " << m_strAcl << endl;
        cout << "    QOS                     : " << m_strQos << endl;
        cout << "    AllowedPriorities       : " << m_strAllowedPriorities << endl;
        cout << "    ReceiveBandwidthLimit   : " << m_strReceiveBandwidthLimit << endl;
        cout << "    ReceiveBandwidthReserve : " << m_strReceiveBandwidthReserve << endl;
        cout << "    DefaultVlanID           : " << m_nDefaultVlanID << endl;
        cout << "    Promiscuous             : " << m_nPromiscuous << endl;
        cout << "    MacVlanFilterEnable     : " << m_nMacVlanFilterEnable << endl;
        cout << "    AllowedTransmitMacs     : " << m_strAllowedTransmitMacs << endl;
        cout << "    AllowedTransmitMacvlans : " << m_strAllowedTransmitMacvlans << endl;
        cout << "    PolicyBlockOverride     : " << m_strPolicyBlockOverride << endl;
        cout << "    PolicyVlanOverride      : " << m_strPolicyVlanOverride << endl;
        cout << "    AllowMacChange          : " << m_nAllowMacChange << endl;
        cout << "    MTU                     : " << m_nMTU << endl;
        cout << "    SwitchportMode          : " << m_nSwitchportMode << endl;
        cout << "    AccessVlan              : " << m_nAccessVlan << endl;
        cout << "    AccessCvlan             : " << m_nAccessCvlan << endl;
        cout << "    NativeVlan              : " << m_nNativeVlan << endl;
        cout << "    IPAddress               : " << m_nIPAddress << endl;
        cout << "    Netmask                 : " << m_nNetmask << endl;
        cout << "    Gateway                 : " << m_nGateway << endl;
        cout << "    MacAddress              : " << m_strMacAddress << endl;
        cout << "    SwitchName              : " << m_strSwitchName << endl;
        cout << "    PCIOrder                : " << m_strPciOrder<< endl;
        cout << "    PortName                : " << m_strPortName<< endl;
        cout << "    m_strNetplaneNickName   : " << m_strNetplaneNickName<< endl;
        cout << "    m_strSegmentId          : " << m_strSegmentId<< endl;
        cout << "    m_nIsolateType          : " << m_nIsolateType<< endl;
        cout << "    QuantumNetWorkUuid      : " << m_strQuantumNetWorkUuid << endl;
        cout << "    QuantumPortUuid         : " << m_strQuantumPortUuid << endl;
    };
};

/*vsi_profile 
供VSI模块获取完整VSI相关信息，下发给VNA使用*/

class CVNetVnicDetail: public Serializable
{
public:
    int32 GetNicIndex() const {return m_cVnicConfig.m_nNicIndex;};  
    int32 GetIsSriov() const {return m_cVnicConfig.m_nIsSriov;};  
    int32 GetIsLoop() const {return m_cVnicConfig.m_nIsLoop;};  
    string  GetLogicNetworkUuid() const {return m_cVnicConfig.m_strLogicNetworkID;}; 
    string  GetPortType() const {return m_cVnicConfig.m_strAdapterModel;};
    string  GetAssIp() const {return m_cVnicConfig.m_strIP;};
    string  GetAssMask() const {return m_cVnicConfig.m_strNetmask;};
    string  GetAssGateway() const {return m_cVnicConfig.m_strGateway;};
    string  GetAssMac() const {return m_cVnicConfig.m_strMac;};
    string GetVSIIDValue() const {return m_cVSIDetail.m_strVSIIDValue;}; 
    int32 GetVSIIDFormat() const {return m_cVSIDetail.m_nVSIIDFormat;}; 
    string GetMgrID() const {return m_cVSIDetail.m_strMgrID;}; 
    string GetVSITypeID() const {return m_cVSIDetail.m_strVSITypeID;}; 
    int32 GetVSITypeVersion() const {return m_cVSIDetail.m_nVSITypeVersion;};   
    string GetPGUuid() const {return m_cVSIDetail.m_strPGUuid;}; 
    int32 GetPGType()const {return m_cVSIDetail.m_nPGType;};
    string GetAcl() const {return m_cVSIDetail.m_strAcl;}; 
    string GetQos() const {return m_cVSIDetail.m_strQos;}; 
    string GetAllowedPriorities() const {return m_cVSIDetail.m_strAllowedPriorities;};  
    string GetReceiveBandwidthLimit() const {return m_cVSIDetail.m_strReceiveBandwidthLimit;}; 
    string GetReceiveBandwidthReserve() const {return m_cVSIDetail.m_strReceiveBandwidthReserve;}; 
    int32 GetDefaultVlanID() const {return m_cVSIDetail.m_nDefaultVlanID;}; 
    int32 GetPromiscuous() const {return m_cVSIDetail.m_nPromiscuous;}; 
    int32 GetMacVlanFilterEnable() const {return m_cVSIDetail.m_nMacVlanFilterEnable;};  
    string GetAllowedTransmitMacs() const {return m_cVSIDetail.m_strAllowedTransmitMacs;}; 
    string GetAllowedTransmitMacvlans() const {return m_cVSIDetail.m_strAllowedTransmitMacvlans;}; 
    string GetPolicyBlockOverride() const {return m_cVSIDetail.m_strPolicyBlockOverride;}; 
    string GetPolicyVlanOverride() const {return m_cVSIDetail.m_strPolicyVlanOverride;};  
    int32 GetAllowMacChange() const {return m_cVSIDetail.m_nAllowMacChange;};
    int32 GetMTU() const {return m_cVSIDetail.m_nMTU;}; 
    int32 GetSwitchportMode() const {return m_cVSIDetail.m_nSwitchportMode;};
    int32 GetAccessVlan() const {return m_cVSIDetail.m_nAccessVlan;};
    int32 GetAccessCvlan() const {return m_cVSIDetail.m_nAccessCvlan;}; 
    void GetTrunkVlan(vector<CTrunkVlanRange> &vecVlanRange) const {vecVlanRange= m_cVSIDetail.m_vecTrunkVlan;}; 
    int32 GetNativeVlan() const {return m_cVSIDetail.m_nNativeVlan;};
    string GetIPAddress() const {return m_cVSIDetail.m_nIPAddress;}; 
    string GetNetmask() const {return m_cVSIDetail.m_nNetmask;}; 
    string GetGateway() const {return m_cVSIDetail.m_nGateway;}; 
    string GetMacAddress() const {return m_cVSIDetail.m_strMacAddress;}; 
    string GetSwitchUuid() const { return m_cVSIDetail.m_strVSwitchUuid;}; 
    string GetSwitchName() const {return m_cVSIDetail.m_strSwitchName;}; 
    string GetVportUuid() const { return m_cVSIDetail.m_strSriovVfPortUuid;};
    string GetPciOrder() const {return m_cVSIDetail.m_strPciOrder;};
    string GetPortName() const {return m_cVSIDetail.m_strPortName;};
    void GetVnicConfig(CVNetVnicConfig &cVnicConfig) const {cVnicConfig = m_cVnicConfig;};
    void GetVsiDetail(CVNetVSIProfileDetail &cVsiDetail) const {cVsiDetail = m_cVSIDetail;};
    string GetNetplaneNickName() const {return m_cVSIDetail.m_strNetplaneNickName;};
    int32 GetSegmentId() const {return m_cVSIDetail.m_strSegmentId;};    
    int32 GetIsolateType() const {return m_cVSIDetail.m_nIsolateType;}; 
    string GetQuantumNetworkUuid() const {return m_cVSIDetail.m_strQuantumNetWorkUuid;};
    string GetQuantumPortUuid() const {return m_cVSIDetail.m_strQuantumPortUuid;};

    void SetNicIndex(int32 value){ m_cVnicConfig.m_nNicIndex = value;};
    void SetIsSriov(int32 value) {m_cVnicConfig.m_nIsSriov = value;};   
    void SetIsLoop(int32 value) {m_cVnicConfig.m_nIsLoop = value;};  
    void SetLogicNetworkUuid(string value) {m_cVnicConfig.m_strLogicNetworkID = value;};  
    void SetPortType(string value) {m_cVnicConfig.m_strAdapterModel = value;};
    void SetAssIp(string value) {m_cVnicConfig.m_strIP = value;};
    void SetAssMask(string value) {m_cVnicConfig.m_strNetmask = value;};
    void SetAssGateway(string value) {m_cVnicConfig.m_strGateway = value;};
    void SetAssMac(string value) {m_cVnicConfig.m_strMac = value;};
    void SetVSIIDValue(string value){ m_cVnicConfig.m_strVSIProfileID = value;m_cVSIDetail.m_strVSIIDValue = value;}; 
    void SetVSIIDFormat(int32 value){ m_cVSIDetail.m_nVSIIDFormat = value;}; 
    void SetMgrID(string value){ m_cVSIDetail.m_strMgrID = value;}; 
    void SetVSITypeID(string value){ m_cVSIDetail.m_strVSITypeID = value;}; 
    void SetVSITypeVersion(int32 value){ m_cVSIDetail.m_nVSITypeVersion = value;};   
    void SetPGUuid(string value){ m_cVSIDetail.m_strPGUuid = value;}; 
    void SetPGType(int32 value){ m_cVSIDetail.m_nPGType = value;};
    void SetAcl(string value){ m_cVSIDetail.m_strAcl = value;}; 
    void SetQos(string value){ m_cVSIDetail.m_strQos = value;}; 
    void SetAllowedPriorities(string value){ m_cVSIDetail.m_strAllowedPriorities = value;};  
    void SetReceiveBandwidthLimit(string value){ m_cVSIDetail.m_strReceiveBandwidthLimit = value;}; 
    void SetReceiveBandwidthReserve(string value){ m_cVSIDetail.m_strReceiveBandwidthReserve = value;}; 
    void SetDefaultVlanID(int32 value){m_cVSIDetail.m_nDefaultVlanID = value;}; 
    void SetPromiscuous(int32 value){ m_cVSIDetail.m_nPromiscuous = value;}; 
    void SetMacVlanFilterEnable(int32 value){ m_cVSIDetail.m_nMacVlanFilterEnable = value;};  
    void SetAllowedTransmitMacs(string value){ m_cVSIDetail.m_strAllowedTransmitMacs = value;}; 
    void SetAllowedTransmitMacvlans(string value){ m_cVSIDetail.m_strAllowedTransmitMacvlans = value;}; 
    void SetPolicyBlockOverride(string value){ m_cVSIDetail.m_strPolicyBlockOverride = value;}; 
    void SetPolicyVlanOverride(string value){ m_cVSIDetail.m_strPolicyVlanOverride = value;};  
    void SetAllowMacChange(int32 value){ m_cVSIDetail.m_nAllowMacChange = value;};
    void SetMTU(int32 value){ m_cVSIDetail.m_nMTU = value;}; 
    void SetSwitchportMode(int32 value){ m_cVSIDetail.m_nSwitchportMode = value;};
    void SetAccessVlan(int32 value){ m_cVSIDetail.m_nAccessVlan = value;};
    void SetAccessCvlan(int32 value){ m_cVSIDetail.m_nAccessCvlan = value;}; 
    void SetTrunkVlan(vector<CTrunkVlanRange> &vecVlanRange)  { m_cVSIDetail.m_vecTrunkVlan = vecVlanRange;}; 
    void SetNativeVlan(int32 value){ m_cVSIDetail.m_nNativeVlan = value;};
    void SetIPAddress(string  value){ m_cVSIDetail.m_nIPAddress = value;}; 
    void SetNetmask(string value){ m_cVSIDetail.m_nNetmask = value;}; 
    void SetGateway(string  value){ m_cVSIDetail.m_nGateway = value;}; 
    void SetMacAddress(string value){ m_cVSIDetail.m_strMacAddress = value;}; 
    void SetSwitchUuid(string value){ m_cVSIDetail.m_strVSwitchUuid = value;}; 
    void SetSwitchName(string value){ m_cVSIDetail.m_strSwitchName = value;}; 
    void SetVportUuid(string value){ m_cVSIDetail.m_strSriovVfPortUuid = value;};
    void SetPciOrder(string value){ m_cVSIDetail.m_strPciOrder = value;};
    void SetPortName(string value){ m_cVSIDetail.m_strPortName = value;};
    void SetVnicConfig(CVNetVnicConfig &value){ m_cVnicConfig = value;};
    void SetVsiDetail(CVNetVSIProfileDetail &value) {m_cVSIDetail = value;};
    void SetNetplaneNickName(string value){ m_cVSIDetail.m_strNetplaneNickName = value;};
    void SetSegmentId(int32 value){ m_cVSIDetail.m_strSegmentId = value;};  
    void SetIsolateType(int32 value){ m_cVSIDetail.m_nIsolateType = value;};  
    void SetQuantumNetworkUuid(string value) {m_cVSIDetail.m_strQuantumNetWorkUuid = value;};
    void SetQuantumPortUuid(string value) {m_cVSIDetail.m_strQuantumPortUuid = value;};

private:
    CVNetVnicConfig m_cVnicConfig;
    CVNetVSIProfileDetail m_cVSIDetail;

public:                        
    SERIALIZE
    {
        SERIALIZE_BEGIN(CVNetVSIProfileDetail);
            WRITE_OBJECT(m_cVnicConfig);
            WRITE_OBJECT(m_cVSIDetail);
        SERIALIZE_END();
    };
    
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(CVNetVSIProfileDetail);
            READ_OBJECT(m_cVnicConfig);
            READ_OBJECT(m_cVSIDetail);
        DESERIALIZE_END();
    };

    void Print()
    {
        m_cVnicConfig.Print();
        m_cVSIDetail.Print();
    };
};


class CVNetVnicMem
{
public:
    CVNetVnicMem()
    {
        m_nVmID = 0;
        m_nNicIndex = 0;
        m_nIsSriov = 0;
        m_nIsLoop = 0;
        m_strIP = m_strMask = m_strGateway = m_strMac = "";
    };
    
    int64 GetVmId () const { return m_nVmID;};   
    int32 GetNicIndex() const {return m_nNicIndex;};  
    int32 GetIsSriov() const {return m_nIsSriov;};  
    int32 GetIsLoop() const {return m_nIsLoop;};  
    string  GetLogicNetworkUuid() const {return m_strLogicNetworkID;}; 
    string GetAssIP() const {return m_strIP;};
    string GetAssMask() const {return m_strMask;};
    string GetAssGateway() const {return m_strGateway;};
    string GetAssMac() const {return m_strMac;};
    string GetPortType() const {return m_strAdapterModel;}; 
    
    string  GetVsiIdValue() const {return m_cVSIProfile.GetVsiIDValue();}; 
    int32  GetVsiIdFormat() const {return m_cVSIProfile.GetVsiIDFormat();}; 
    string  GetVmPgUuid() const {return m_cVSIProfile.GetPgUuid();}; 
    string  GetMac() const {return m_cVSIProfile.GetMac();}; 
    string  GetIp() const {return m_cVSIProfile.GetIP();}; 
    string  GetMask() const {return m_cVSIProfile.GetMask();}; 
    string  GetVSwitchUuid() const {return m_cVSIProfile.GetVSwitchUuid();};    
    string  GetSriovVfPortUuid() const {return m_cVSIProfile.GetVfPortUuid();};  
    string  GetQuantumNetworkUuid() const {return m_cVSIProfile.GetQuantumNetworkUuid();};
    string  GetQuantumPortUuid() const {return m_cVSIProfile.GetQuantumPortUuid();};

    void SetVmId (const int64 value) { m_nVmID = value;};   
    void SetNicIndex(const int32 value){ m_nNicIndex = value;};
    void SetIsSriov(const int32 value) {m_nIsSriov = value;};   
    void SetIsLoop(const int32 value) {m_nIsLoop = value;};  
    void SetLogicNetworkUuid(const string value) {m_strLogicNetworkID = value;};
    void SetAssIP(const string value) {m_strIP = value;};  
    void SetAssMask(const string value) {m_strMask = value;};  
    void SetAssGateway(const string value) {m_strGateway = value;}; 
    void SetAssMac(const string value) {m_strMac = value;};
    void SetPortType(const string value) {m_strAdapterModel = value;};  
    
    void SetVsiIdValue(const string value) {m_cVSIProfile.SetVsiIDValue(value);};  
    void SetVsiIdFormat(int32 value) {m_cVSIProfile.SetVsiIDFormat(value);};   
    void SetVmPgUuid(const string value) {m_cVSIProfile.SetPgUuid(value);};  
    void SetMac(const string value) {m_cVSIProfile.SetMac(value);};  
    void SetIp(const string value) {m_cVSIProfile.SetIP(value);};
    void SetMask(const string value) {m_cVSIProfile.SetMask(value);};  
    void SetVSwitchUuid(const string value) {m_cVSIProfile.SetVSwitchUuid(value);};   
    void SetSriovVfPortUuid(const string value) {m_cVSIProfile.SetVfPortUuid(value);};    
    void SetQuantumNetworkUuid(const string value) {m_cVSIProfile.SetQuantumNetworkUuid(value);};
    void SetQuantumPortUuid(const string value) {m_cVSIProfile.SetQuantumPortUuid(value);};

   void Print()
    {
        cout << "CVNetVnicMem: " << endl;
        cout << "    VmID                : " << m_nVmID << endl;
        cout << "    NicIndex            : " << m_nNicIndex << endl;
        cout << "    IsSriov             : " << m_nIsSriov << endl;
        cout << "    IsLoop              : " << m_nIsLoop << endl;
        cout << "    IP                  : " << m_strIP << endl;
        cout << "    Netmask             : " << m_strMask << endl;
        cout << "    Gateway             : " << m_strGateway << endl;
        cout << "    MAC                 : " << m_strMac << endl;
        cout << "    LogicNetworkI       : " << m_strLogicNetworkID << endl;
        cout << "    AdapterModel        : " << m_strAdapterModel << endl;

        m_cVSIProfile.Print();
    };
   
private:
    int64   m_nVmID;             //由计算填写，虚拟机id
    int32   m_nNicIndex;                      //NicIndex,输入参数
    int32   m_nIsSriov;                        //IsSriov,输入参数
    int32   m_nIsLoop;                         //IsLoop,输入参数
    string  m_strLogicNetworkID;        //LogicNetwork UUID,输入参数
    string  m_strIP;                    //用户指定IP
    string  m_strMask;                  //用户指定Netmask
    string  m_strGateway;               //用户指定Gateway
    string  m_strMac;                   //用户指定MAC
    string  m_strAdapterModel;           //网卡型号,输入参数
    
    CVNetVSIProfile  m_cVSIProfile;                //VSIProfile UUID,输出参数
};

class CVNetVmMem
{
public:
    CVNetVmMem()
    {
        m_nVmID = 0;
        m_nProjectId = -1;
        m_nIsHasWatchdog = 0;
        m_nWatchdogTimeout = 0;
    };
    
    int64 GetVmId () const { return m_nVmID;};   
    int64 GetProjectId() const{return m_nProjectId;};
    int32  GetIsHasWatchdog()const{return m_nIsHasWatchdog;};        
    int32  GetWatchdogTimeout()const{return m_nWatchdogTimeout;};        
    string  GetClusterName() const {return m_strClusterApp;};
    string  GetHostName() const {return m_strHostApp;};    
    void  GetVecVnicMem(vector<CVNetVnicMem> &vecVnicMem){vecVnicMem = m_vecVnicMem;};    
       
    void SetVmId (const int64 value) { m_nVmID = value;};   
    void SetProjectId(const int64 value) {m_nProjectId = value;};
    void SetIsHasWatchdog(int32 value) {m_nIsHasWatchdog = value;};     
    void SetWatchdogTimeout(int32 value) {m_nWatchdogTimeout = value;};     
    void SetClusterName(const string value) {m_strClusterApp = value;}; 
    void SetHostName(const string value) {m_strHostApp = value;}; 
    void SetVecVnicMem(vector<CVNetVnicMem> &vecVnicMem){m_vecVnicMem = vecVnicMem;};    
    
   void Print()
    {
        cout << "CVNetVmMem: " << endl;
        cout << "    VmID                : " << m_nVmID << endl;
        cout << "    ProjectId           : " << m_nProjectId << endl;
        cout << "    IsHasWatchdog       : " << m_nIsHasWatchdog << endl;
        cout << "    WatchdogTimeout     : " << m_nWatchdogTimeout << endl;
        cout << "    ClusterName         : " << m_strClusterApp << endl;
        cout << "    HostName            : " << m_strHostApp << endl;

	    vector<CVNetVnicMem>::iterator  itrVnic = m_vecVnicMem.begin();
	    for ( ; itrVnic != m_vecVnicMem.end(); ++itrVnic)
	    {
	        itrVnic->Print();
	    }
	};
   
private:
    int64  m_nVmID;             //由计算填写，虚拟机id
    int64  m_nProjectId;        //由计算填写, 工程id
    int32  m_nIsHasWatchdog;
    int32  m_nWatchdogTimeout;
    string m_strClusterApp;
    string m_strHostApp;
    
	vector<CVNetVnicMem> m_vecVnicMem;
};

}
#endif


