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

#if !defined(_VNET_DB_BOND_INCLUDE_H_)
#define _VNET_DB_BOND_INCLUDE_H_

#include "vnet_db_in.h"
#include "vnet_db_interface.h"
#include "vnet_db_proc.h"

#include "vnet_db_port.h"

namespace zte_tecs
{

// port db


class CDbBond : public  CDbPort
{
public: 
    CDbBond() 
    {
        m_nBondMode = m_nIsHasLacpInfo=m_nLacpState =0;
        m_strPolicy = m_strLacpAggrId = m_strLacpPartnerMac = "";
     };
    virtual ~CDbBond() {};
    
    int32 GetBondMode(){return m_nBondMode;};     
    string & GetPolicy(){return m_strPolicy;};
    int32 GetIsHasLacpInfo(){return m_nIsHasLacpInfo;};  
    int32 GetLacpState(){return m_nLacpState;}; 
    string & GetLacpAggrId(){return m_strLacpAggrId;};
    string & GetLacpPartnerMac(){return m_strLacpPartnerMac;};
    
    void SetBondMode(int32 value) {m_nBondMode = value;};   
    void SetPolicy(const char* uuid) {m_strPolicy = uuid;};
    void SetIsHasLacpInfo(int32 value) {m_nIsHasLacpInfo = value;};   
    void SetLacpState(int32 value) {m_nLacpState = value;};   
    void SetLacpAggrId (const char* uuid) {m_strLacpAggrId = uuid;};
    void SetLacpPartnerMac(const char* uuid) {m_strLacpPartnerMac = uuid;};
    

    void DbgShow()
    {
        DbgPortShow();        
        cout << "BondMode     : " << m_nBondMode << endl;   
        cout << "Policy       : " << m_strPolicy << endl;   
        cout << "IsHasLacpInfo: " << m_nIsHasLacpInfo << endl;   
        cout << "LacpState    : " << m_nLacpState << endl;   
        cout << "LacpAggrId   : " << m_strLacpAggrId << endl;   
        cout << "LacpPartnerMa: " << m_strLacpPartnerMac << endl; 
        
    }
private:    

    int32 m_nBondMode;
    string m_strPolicy;

    int32 m_nIsHasLacpInfo;
    int32 m_nLacpState;
    string m_strLacpAggrId;
    string m_strLacpPartnerMac;
};


class CDbBondSummary 
{
public: 
    CDbBondSummary() 
    {
        m_nBondMode = 0;
        m_strBondUuid = m_strBondName = m_strVnaUuid = "";
     };
    virtual ~CDbBondSummary() {};
      
    string & GetVnaUuid(){return m_strVnaUuid;};
    string & GetBondUuid(){return m_strBondUuid;};
    string & GetBondName(){return m_strBondName;};
    int32 GetBondMode(){return m_nBondMode;};   
    
    void SetVnaUuid(const char* value) {m_strVnaUuid = value;};   
    void SetBondUuid (const char* uuid) {m_strBondUuid = uuid;};
    void SetBondName(const char* value) {m_strBondName = value;};   
    void SetBondMode(int32 value) {m_nBondMode = value;};   
    

    void DbgShow()
    {
        cout << "VnaUuid     : " << m_strVnaUuid << endl;   
        cout << "BondUuid    : " << m_strBondUuid << endl;   
        cout << "BondName    : " << m_strBondName << endl;   
        cout << "BondMode    : " << m_nBondMode << endl;   
        
    }
private:    

    string m_strVnaUuid;
    string m_strBondUuid;
    string m_strBondName;    
    int32 m_nBondMode;
};


class CDBBondMap
{
public: 
    CDBBondMap() 
    {
        m_nBondMode = m_nLacpIsSuc = m_nBackupIsActive = m_nIsCfg = m_nIsSelect = 0;
        m_strBondUuid = m_strBondName = m_strVnaUuid = m_strPhyUuid = m_strPhyName = "";
     };
    virtual ~CDBBondMap() {};
      
    string & GetVnaUuid(){return m_strVnaUuid;};
    string & GetBondUuid(){return m_strBondUuid;};
    string & GetBondName(){return m_strBondName;};
    string & GetPhyUuid(){return m_strPhyUuid;};
    string & GetPhyName(){return m_strPhyName;};
    int32 GetBondMode(){return m_nBondMode;};   
    int32 GetLacpIsSuc(){return m_nLacpIsSuc;};
    int32 GetBackupIsActive(){return m_nBackupIsActive;};
    int32 GetIsCfg(){return m_nIsCfg;};
    int32 GetIsSelect(){return m_nIsSelect;};
    
    void SetVnaUuid(const char* value) {m_strVnaUuid = value;};   
    void SetBondUuid (const char* uuid) {m_strBondUuid = uuid;};
    void SetBondName(const char* value) {m_strBondName = value;};  
    void SetPhyUuid (const char* uuid) {m_strPhyUuid = uuid;};
    void SetPhyName(const char* value) {m_strPhyName = value;}; 
    void SetBondMode(int32 value) {m_nBondMode = value;};   
    void SetLacpIsSuc(int32 value) {m_nLacpIsSuc = value;}; 
    void SetBackupIsActive(int32 value) {m_nBackupIsActive = value;}; 
    void SetIsCfg(int32 value) {m_nIsCfg = value;}; 
    void SetIsSelect(int32 value) {m_nIsSelect = value;}; 

    void DbgShow()
    {
        cout << "VnaUuid       : " << m_strVnaUuid << endl;   
        cout << "BondUuid      : " << m_strBondUuid << endl;   
        cout << "BondName      : " << m_strBondName << endl;   
        cout << "PhyUuid       : " << m_strPhyUuid << endl;   
        cout << "PhyName       : " << m_strPhyName << endl; 
        cout << "BondMode      : " << m_nBondMode << endl;   
        cout << "LacpIsSuc     : " << m_nLacpIsSuc << endl; 
        cout << "BackupIsActive: " << m_nBackupIsActive << endl; 
        cout << "IsCfg         : " << m_nIsCfg << endl; 
        cout << "IsSelect      : " << m_nIsSelect << endl;         
    }
private:    

    string m_strVnaUuid;
    string m_strBondUuid;
    string m_strBondName;  
    string m_strPhyUuid;
    string m_strPhyName;
    int32 m_nBondMode;  
    int32 m_nLacpIsSuc;
    int32 m_nBackupIsActive;
    int32 m_nIsCfg;   // bond_map table ->iscfg;
    int32 m_nIsSelect;// bond_map table ->isSelect;
};


// bond sriov port 下部署的虚拟网口数目
class CDbBondSriovVsiNum
{
public: 
    CDbBondSriovVsiNum() 
    {
        m_nRet = m_nVsiNum = 0;
     };
    virtual ~CDbBondSriovVsiNum() {};
    
    int32 GetRet(){return m_nRet;}; 
    int32 GetVsiNum(){return m_nVsiNum;}; 

    void SetRet(int32 value) {m_nRet = value;};   
    void SetVsiNum(int32 value) {m_nVsiNum = value;};   
private:
    int32 m_nRet;
    int32 m_nVsiNum;

};


class CVNetDbIProcedure;
class CDBOperateBond : public  CVNetDbIProcedureCallback, public CDBOperatePortBase
{
public:
    explicit CDBOperateBond(CVNetDbIProcedure * pb);
    virtual ~CDBOperateBond();

    // bond port 操作
    int32 QuerySummary(const char* vna_uuid, vector<CDbBondSummary> & outVInfo);  
    int32 Query(CDbBond & Info);
    
    int32 CheckAdd(CDbBond & info);
    int32 Add(CDbBond & info);
    int32 CheckModify(CDbBond & info);
    int32 Modify(CDbBond & info);
    int32 CheckDel(const char* port_uuid);
    int32 Del(const char* port_uuid);

    //查询BOND SRIOV网口下 部署的虚拟机网卡数目
    int32 QueryVsiNum( const char* bond_uuid, int32 & outVsiNum ); 

    // bond map 操作
    // 以下接口CDBBondMap中仅需设置 vna_uuid, bond_uuid, phy_uuid, is_cfg,is_select 字段信息
    int32 CheckAddMap(CDBBondMap & info);
    int32 AddMap(CDBBondMap & info);
    int32 CheckModifyMap(CDBBondMap & info);
    int32 ModifyMap(CDBBondMap & info);    
    int32 CheckDelMap(const char* vna_uuid, const char * bond_uuid, const char* phy_uuid);
    int32 DelMap(const char* vna_uuid, const char * bond_uuid, const char* phy_uuid);    

    // 以下几个查询接口，能查询到成员口状态信息m_nLacpIsSuc m_nBackupIsActive
    int32 QueryMapByVna(const char* vna_uuid, vector<CDBBondMap> & outVInfo);
    int32 QueryMapByBond(const char* bond_uuid, vector<CDBBondMap> & outVInfo);    
    int32 QueryMapByPhy(const char* phy_uuid, vector<CDBBondMap> & outVInfo);        

    // 设置bond 运行结果信息
    int32 SetBackupActiveNic(const char* bond_uuid, const char* phy_uuid);
    int32 UnsetBackupActiveNic(const char* bond_uuid, const char* phy_uuid); 
    int32 SetLacpSuccessNic(const char* bond_uuid, const char* phy_uuid);
    int32 UnsetLacpSuccessNic(const char* bond_uuid, const char* phy_uuid);    

    int32 DbProcCallback(int32 type, CADORecordset *, void* );

    void DbgShow();    
private:
    int32 QueryVsiNumCallback(CADORecordset * prs,void * nil);
    int32 QuerySummaryCallback(CADORecordset * prs,void * nil);
    int32 QueryCallback(CADORecordset * prs,void * nil);
    int32 OperateCallback(CADORecordset * prs,void * nil);
    int32 CheckOperateCallback(CADORecordset * prs,void * nil); 

    // 查询map 
    int32 QueryMapCallback(CADORecordset * prs,void * nil);
    
private:
    DISALLOW_COPY_AND_ASSIGN(CDBOperateBond);

    CVNetDbIProcedure * m_pIProc;
};
    
} // namespace zte_tecs

#endif // _VNET_DB_BOND_INCLUDE_H_ 


