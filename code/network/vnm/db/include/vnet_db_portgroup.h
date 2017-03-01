

/******************************************************************************
* Copyright (c) 2013，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：vnet_db_netplane.h
* 文件标识：
* 内容摘要：CVNetDb类的定义文件
* 当前版本：1.0
* 作    者：
* 完成日期：2013年1月28日
*
* 修改记录1：
*     修改日期：2013/1/28
*     版 本 号：V1.0
*     修 改 人：gong.xiefeng
*     修改内容：创建
*
******************************************************************************/

#if !defined(_VNET_DB_PORTGROUP_INCLUDE_H_)
#define _VNET_DB_PORTGROUP_INCLUDE_H_

#include "vnet_db_in.h"
#include "vnet_db_interface.h"
#include "vnet_db_proc.h"


namespace zte_tecs
{

typedef enum tagDbPortGroupTypeValues
{
    EN_DB_PORTGROUP_TYPE_UPLINK = 0,
    EN_DB_PORTGROUP_TYPE_KERNEL_MGR = 10,     
    EN_DB_PORTGROUP_TYPE_KERNEL_STORAGE = 11,
    EN_DB_PORTGROUP_TYPE_VM_SHARE = 20,
    EN_DB_PORTGROUP_TYPE_VM_PRIVATE = 21,
}EN_DB_PORTGROUP_TYPE_VALUES;

typedef enum tagDbSwitchPortModeTypeValues
{
    EN_DB_SWITCHPORTMODE_TYPE_TRUNK = 0,
    EN_DB_SWITCHPORTMODE_TYPE_ACCESS = 1, 
}EN_DB_SWITCHPORTMODE_TYPE_VALUES;


typedef enum tagDbVmPgIsolateTypeValues
{
    EN_DB_VMPG_ISOLATE_TYPE_VLAN = 0,
    EN_DB_VMPG_ISOLATE_TYPE_VXLAN = 1, 
}EN_DB_SVMPG_ISOLATE_TYPE_VALUES;

/*  = 0 valid. 
   != 0 invalid.
*/
int32 CheckPortGroupType(int32 type);
int32 CheckSwitchPortModeType(int32 type);

// port db
class CDbPortGroup
{
public: 
    CDbPortGroup() 
    {
        m_strName = m_strUuid = m_strAcl = m_strQos = m_strAllowPriorities = m_strRcvBandwidthLmt = m_strRcvBandwidthRsv = "";
        m_strAllowTrmtMacs = m_strAllowTrmtMacVlans =m_strPlyBlkOver = m_strPlyVlanOver =  m_strMgrId = m_strTypeId = m_strNetplaneUuid = "";  
        m_nPgType = m_nDftVlanId = m_nPromiscuous = m_nMacVlanFltEnable = m_nVersion =  0;
        m_nAllowMacChg = m_nSwithportMode = m_nSwithportMode = m_nIsCfgNetplane = 0;
        m_nMtu = m_nTrunkNatvieVlanNum = m_nAccessVlanNum = m_nAccessIsolateNo = 0;
        m_strDesc = "";
        m_nIsSdn =0;
        m_nVxlanIsolateNo = 0;
        
        m_nSegmentIdNum = 0;
        m_nIsolateType = 0;
     };
    virtual ~CDbPortGroup() {};
    
    string & GetName() { return m_strName;};   
    int32 GetPgType() {return m_nPgType;};
    string & GetUuid() {return m_strUuid;};
    string & GetAcl() {return m_strAcl;};
    string & GetQos() {return m_strQos;};
    string & GetAllowPriorities() {return m_strAllowPriorities;};    
    string & GetRcvBandwidthLmt() {return m_strRcvBandwidthLmt;};    
    string & GetRcvBandwidthRsv() {return m_strRcvBandwidthRsv;};    
    int32 GetDftVlanId() {return m_nDftVlanId;};    
    int32 GetPromiscuous() {return m_nPromiscuous;};    
    int32 GetMacVlanFltEnable() {return m_nMacVlanFltEnable;};    
    string & GetAllowTrmtMacs() {return m_strAllowTrmtMacs;};     
    string & GetAllowTrmtMacVlans() {return m_strAllowTrmtMacVlans;};    
    string & GetPlyBlkOver() {return m_strPlyBlkOver;};    
    string & GetPlyVlanOver() {return m_strPlyVlanOver;};    
    int32 GetVersion() {return m_nVersion;};    
    string & GetMgrId() {return m_strMgrId;};    
    string & GetTypeId() {return m_strTypeId;};    
    int32 GetAllowMacChg() {return m_nAllowMacChg;};   
    int32 GetSwithportMode() {return m_nSwithportMode;};    
    int32 GetIsCfgNetplane() {return m_nIsCfgNetplane;};    
    string & GetNetplaneUuid() {return m_strNetplaneUuid;};    
    int32 GetMtu() {return m_nMtu;};    
    int32 GetTrunkNatvieVlanNum() {return m_nTrunkNatvieVlanNum;};    
    int32 GetAccessVlanNum() {return m_nAccessVlanNum;};    
    int32 GetAccessIsolateNo() {return m_nAccessIsolateNo;};   
    string & GetDesc() {return m_strDesc;};

    int32 GetIsolateType() {return m_nIsolateType;}; 
    int32 GetSegmentIdNum() {return m_nSegmentIdNum;}; 
    int32 GetVxlanIsolateNo() {return m_nVxlanIsolateNo;}; 
    int32 GetIsSdn() {return m_nIsSdn;};     
    
    void SetName(const char*  value) {m_strName = value;};   
    void SetPgType(int32 value) {m_nPgType = value;};
    void SetUuid(const char*  value) {m_strUuid = value;};
    void SetAcl(const char* value) {m_strAcl = value;};
    void SetQos(const char*  value) {m_strQos = value;};
    void SetAllowPriorities(const char*  value) {m_strAllowPriorities = value;};    
    void SetRcvBandwidthLmt(const char*  value) {m_strRcvBandwidthLmt = value;};    
    void SetRcvBandwidthRsv(const char*  value) {m_strRcvBandwidthRsv = value;};    
    void SetDftVlanId(int32 value) {m_nDftVlanId = value;};    
    void SetPromiscuous(int32 value) {m_nPromiscuous = value;};    
    void SetMacVlanFltEnable(int32 value) {m_nMacVlanFltEnable = value;};    
    void SetAllowTrmtMacs(const char*  value) {m_strAllowTrmtMacs = value;};     
    void SetAllowTrmtMacVlans(const char*  value) {m_strAllowTrmtMacVlans = value;};    
    void SetPlyBlkOver(const char*  value) {m_strPlyBlkOver = value;};    
    void SetPlyVlanOver(const char*  value) {m_strPlyVlanOver = value;};    
    void SetVersion(int32 value) {m_nVersion = value;};    
    void SetMgrId(const char*  value) {m_strMgrId = value;};    
    void SetTypeId(const char*  value) {m_strTypeId = value;};    
    void SetAllowMacChg(int32 value) {m_nAllowMacChg = value;};   
    void SetSwithportMode(int32 value) {m_nSwithportMode = value;};    
    void SetIsCfgNetplane(int32 value) {m_nIsCfgNetplane = value;};    
    void SetNetplaneUuid(const char*  value) {m_strNetplaneUuid = value;};    
    void SetMtu(int32 value) {m_nMtu = value;};    
    void SetTrunkNatvieVlanNum(int32 value) {m_nTrunkNatvieVlanNum = value;};    
    void SetAccessVlanNum(int32 value) {m_nAccessVlanNum = value;};    
    void SetAccessIsolateNo(int32 value) {m_nAccessIsolateNo = value;};   
    void SetDesc(const char* value) {m_strDesc = value;};  
    
    void SetIsolateType(int32 value) {m_nIsolateType = value;};  
    void SetSegmentIdNum(int32 value) {m_nSegmentIdNum = value;};     
    void SetVxlanIsolateNo(int32 value) {m_nVxlanIsolateNo = value;};   
    void SetIsSdn(int32 value) {m_nIsSdn = value;};   
    
    void DbgShow()
    {      
        cout << "Name               : " << m_strName << endl;   
        cout << "PgType             : " << m_nPgType << endl; 
        cout << "Uuid               : " << m_strUuid << endl; 
        cout << "Acl                : " << m_strAcl << endl; 
        cout << "Qos                : " << m_strQos << endl; 
        cout << "AllowPriorities    : " << m_strAllowPriorities << endl;  
        cout << "RcvBandwidthLmt    : " << m_strRcvBandwidthLmt << endl; 
        cout << "RcvBandwidthRsv    : " << m_strRcvBandwidthRsv << endl; 
        cout << "DftVlanId          : " << m_nDftVlanId << endl; 
        cout << "Promiscuous        : " << m_nPromiscuous << endl; 
        cout << "MacVlanFltEnable   : " << m_nMacVlanFltEnable << endl; 
        cout << "AllowTrmtMacs      : " << m_strAllowTrmtMacs << endl; 
        cout << "AllowTrmtMacVlans  : " << m_strAllowTrmtMacVlans << endl; 
        cout << "PlyBlkOver         : " << m_strPlyBlkOver << endl; 
        cout << "PlyVlanOver        : " << m_strPlyVlanOver << endl; 
        cout << "Version            : " << m_nVersion << endl; 
        cout << "MgrId              : " << m_strMgrId << endl; 
        cout << "TypeId             : " << m_strTypeId << endl; 
        cout << "AllowMacChg        : " << m_nAllowMacChg << endl; 
        cout << "SwithportMode      : " << m_nSwithportMode << endl; 
        cout << "IsCfgNetplane      : " << m_nIsCfgNetplane << endl; 
        cout << "NetplaneUuid       : " << m_strNetplaneUuid << endl; 
        cout << "Mtu                : " << m_nMtu << endl; 
        cout << "TrunkNatvieVlanNum : " << m_nTrunkNatvieVlanNum << endl; 
        cout << "AccessVlanNum      : " << m_nAccessVlanNum << endl; 
        cout << "AccessIsolateNo    : " << m_nAccessIsolateNo << endl; 
        cout << "Description        : " << m_strDesc << endl;         
        cout << "IsolateType        : " << m_nIsolateType << endl;              
        cout << "SegmentIdNum       : " << m_nSegmentIdNum << endl;        
        cout << "VxlanIsolateNo     : " << m_nVxlanIsolateNo << endl;        
        cout << "IsSdn              : " << m_nIsSdn << endl;                
    }
private:    

    string m_strName;
    int32 m_nPgType;
    string m_strUuid;
    string m_strAcl;
    string m_strQos;
    string m_strAllowPriorities;
    string m_strRcvBandwidthLmt;
    string m_strRcvBandwidthRsv;
    int32 m_nDftVlanId;
    int32 m_nPromiscuous;
    int32 m_nMacVlanFltEnable;
    string m_strAllowTrmtMacs;
    string m_strAllowTrmtMacVlans;
    string m_strPlyBlkOver;
    string m_strPlyVlanOver;
    int32 m_nVersion;
    string m_strMgrId;
    string m_strTypeId;
    int32  m_nAllowMacChg;
    int32  m_nSwithportMode;

    int32  m_nIsCfgNetplane;
    string m_strNetplaneUuid;
    int32  m_nMtu;

    int32  m_nTrunkNatvieVlanNum;
    int32  m_nAccessVlanNum;
    int32  m_nAccessIsolateNo;

    string m_strDesc;

    int32 m_nIsolateType; // vm pg, uplink pg 才有效
    int32  m_nSegmentIdNum;
    int32  m_nVxlanIsolateNo;
    int32  m_nIsSdn;
};


class CDbPortGroupSummary
{
public: 
    CDbPortGroupSummary() 
    {
        m_strName = m_strUuid = "";
        m_nPgType = m_nSwithportMode  = 0;
     };
    virtual ~CDbPortGroupSummary() {};
    
    string & GetName() { return m_strName;};   
    int32 GetPgType() {return m_nPgType;};
    string & GetUuid() {return m_strUuid;};
    int32 GetSwithportMode() {return m_nSwithportMode;};    
    
    void SetName(const char*  value) {m_strName = value;};   
    void SetPgType(int32 value) {m_nPgType = value;};
    void SetUuid(const char*  value) {m_strUuid = value;};
    void SetSwithportMode(int32 value) {m_nSwithportMode = value;};
    
    void DbgShow()
    {      
        cout << "Name          : " << m_strName << endl;   
        cout << "PgType        : " << m_nPgType << endl; 
        cout << "Uuid          : " << m_strUuid << endl; 
        cout << "SwithportMode : " << m_nSwithportMode << endl;         
    }
private:
    string m_strName;
    int32 m_nPgType;
    string m_strUuid;
    int32  m_nSwithportMode;
};

class CVNetDbIProcedure;
class CDBOperatePortGroup : public  CVNetDbIProcedureCallback
{
public:
    explicit CDBOperatePortGroup(CVNetDbIProcedure * pb);
    virtual ~CDBOperatePortGroup();

    int32 QuerySummary(vector<CDbPortGroupSummary> & outVInfo);  
    int32 Query(CDbPortGroup & Info);
    int32 Add(CDbPortGroup & info);
    int32 Modify(CDbPortGroup & info);
    int32 Del(const char* port_uuid);

    int32 IsUsing(const char* port_uuid);

    int32 DbProcCallback(int32 type, CADORecordset *, void* );

    void DbgShow();    
private:
    int32 QuerySummaryCallback(CADORecordset * prs,void * nil);
    int32 QueryCallback(CADORecordset * prs,void * nil);
    int32 OperateCallback(CADORecordset * prs,void * nil);
    int32 CheckOperateCallback(CADORecordset * prs,void * nil);  
    
private:
    DISALLOW_COPY_AND_ASSIGN(CDBOperatePortGroup);

    CVNetDbIProcedure * m_pIProc;
};
    
} // namespace zte_tecs

#endif // _VNET_DB_PORTGROUP_INCLUDE_H_ 



