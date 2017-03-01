/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：port_group.h
* 文件标识： 
* 内容摘要：端口组定义类
* 当前版本：1.0
* 作    者： 
* 完成日期： 
*******************************************************************************/
#ifndef PORT_GROUP_H
#define PORT_GROUP_H



namespace zte_tecs
{
#if 0
class CPortGroup
{
public:
    explicit CPortGroup(){};
    virtual ~CPortGroup(){};

    string & GetName() { return m_strName;};   
    int32    GetPgType() {return m_nPGType;};
    string & GetUuid() {return m_strUUID;};
    string & GetAcl() {return m_strACL;};
    string & GetQos() {return m_strQOS;};
    string & GetAllowPriorities() {return m_strPrior;};    
    string & GetRcvBandwidthLmt() {return m_strRcvBWLimit;};    
    string & GetRcvBandwidthRsv() {return m_strRcvBWRsv;};    
    int32 GetDftVlanId() {return m_nDefaultVlan;};    
    int32 GetPromiscuous() {return m_nPromisc;};    
    int32 GetMacVlanFltEnable() {return m_nMacVlanFilter;};    
    string & GetAllowTrmtMacs() {return m_strTxMacs;};     
    string & GetAllowTrmtMacVlans() {return m_strTxMacvlans;};    
    string & GetPlyBlkOver() {return m_strPolicyBlkOver;};    
    string & GetPlyVlanOver() {return m_strPolicyVlanOver;};    
    int32 GetVersion() {return m_nVersion;};    
    string & GetMgrId() {return m_strMgrId;};    
    string & GetTypeId() {return m_strVSIType;};    
    int32 GetAllowMacChg() {return m_nMacChange;};   
    int32 GetSwithportMode() {return m_nSwitchPortMode;};    
    int32 GetIsCfgNetplane() {return m_nIsCfgNetPlane;};    
    string & GetNetplaneUuid() {return m_strNetPlaneUUID;};    
    int32 GetMtu() {return m_nMTU;};    
    int32 GetTrunkNatvieVlanNum() {return m_nTrunkNativeVlan;};    
    int32 GetAccessVlanNum() {return m_nAccessVlanNum;};    
    int32 GetAccessIsolateNo() {return m_nAccessIsolateNum;};   
    vector<CTrunkVlanRange> & GetTrunkVlanRange(){return m_vecTrunkVlanRange;}
    
    void SetName(const string  &value) {m_strName = value;};   
    void SetPgType(int32 value) {m_nPGType = value;};
    void SetUuid(const string  &value) {m_strUUID = value;};
    void SetAcl(const string  &value) {m_strACL = value;};
    void SetQos(const string  &value) {m_strQOS = value;};
    void SetAllowPriorities(const string  &value) {m_strPrior = value;};    
    void SetRcvBandwidthLmt(const string  &value) {m_strRcvBWLimit = value;};    
    void SetRcvBandwidthRsv(const string  &value) {m_strRcvBWRsv = value;};    
    void SetDftVlanId(int32 value) {m_nDefaultVlan = value;};    
    void SetPromiscuous(int32 value) {m_nPromisc = value;};    
    void SetMacVlanFltEnable(int32 value) {m_nMacVlanFilter = value;};    
    void SetAllowTrmtMacs(const string  &value) {m_strTxMacs = value;};     
    void SetAllowTrmtMacVlans(const string  &value) {m_strTxMacvlans = value;};    
    void SetPlyBlkOver(const string  &value) {m_strPolicyBlkOver = value;};    
    void SetPlyVlanOver(const string  &value) {m_strPolicyVlanOver = value;};    
    void SetVersion(int32 value) {m_nVersion = value;};    
    void SetMgrId(const string  &value) {m_strMgrId = value;};    
    void SetTypeId(const string  &value) {m_strVSIType = value;};    
    void SetAllowMacChg(int32 value) {m_nMacChange = value;};   
    void SetSwithportMode(int32 value) {m_nSwitchPortMode = value;};    
    void SetIsCfgNetplane(int32 value) {m_nIsCfgNetPlane = value;};    
    void SetNetplaneUuid(const string  &value) {m_strNetPlaneUUID = value;};    
    void SetMtu(int32 value) {m_nMTU = value;};    
    void SetTrunkNatvieVlanNum(int32 value) {m_nTrunkNativeVlan = value;};    
    void SetAccessVlanNum(int32 value) {m_nAccessVlanNum = value;};    
    void SetAccessIsolateNo(int32 value) {m_nAccessIsolateNum = value;}; 
    void SetTrunkVlanRange(CTrunkVlanRange &cVlanRange)
    {
        m_vecTrunkVlanRange.push_back(cVlanRange);
    };
private:
    string m_strName;
    string m_strUUID;
    string m_strdescription;
    int32  m_nPGType;
    string m_strACL;
    string m_strQOS;;
    string m_strPrior;
    string m_strRcvBWLimit;
    string m_strRcvBWRsv;
    int32  m_nDefaultVlan;
    int32  m_nPromisc;
    int32  m_nMacVlanFilter;
    string m_strTxMacs;
    string m_strTxMacvlans;
    string m_strPolicyBlkOver;
    string m_strPolicyVlanOver;
    int32  m_nVersion;
    string m_strMgrId;
    string m_strVSIType;
    int32  m_nMacChange;
    int32  m_nSwitchPortMode;   // 0--access, 1--trunk
    int32  m_nIsCfgNetPlane;
    string m_strNetPlaneUUID;
    int32  m_nMTU;
    int32  m_nAccessVlanNum;
    int32  m_nAccessIsolateNum; 
    int32  m_nTrunkNativeVlan;

    vector<CTrunkVlanRange> m_vecTrunkVlanRange;
};
#endif
}//namespace

#endif

