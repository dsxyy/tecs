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

#if !defined(_VNET_DB_WILDCAST_SWITCH_INCLUDE_H_)
#define _VNET_DB_WILDCAST_SWITCH_INCLUDE_H_

#include "vnet_db_in.h"
#include "vnet_db_interface.h"
#include "vnet_db_proc.h"

namespace zte_tecs
{

class CDbWildcastSwitch
{
public: 
    CDbWildcastSwitch() 
    {
        m_strUplinkPortType = 0;
        m_nState = m_nEvbMode = 0;
        m_strUuid = m_strSwitchName = m_strUplinkPgUuid = m_strUplinkPortName = "";

        m_nNicMaxNum = 0;
        m_nMtuMax = 1500;
        m_nSwitchType = 0;
        m_strUplinkPortBondMode = 0;
     };
    virtual ~CDbWildcastSwitch() {};

    string & GetUuid(){return m_strUuid;};
    string & GetSwitchName(){return m_strSwitchName;}; 
    int32  GetSwitchType(){return m_nSwitchType;};
    int32  GetState(){return m_nState;};
    int32  GetEvbMode(){return m_nEvbMode;};    
    string & GetUplinkPgUuid(){return m_strUplinkPgUuid;}; 
    int32  GetMtuMax(){return m_nMtuMax;};
    int32  GetNicMaxNum(){return m_nNicMaxNum;}; 
    string & GetUplinkPortName(){return m_strUplinkPortName;}; 
    int32 GetUplinkPortType(){return m_strUplinkPortType;}; 
    int32 GetBondMode(){return m_strUplinkPortBondMode;}; 
 
    void SetUuid(const char* value) {m_strUuid = value;}; 
    void SetSwitchName(const char* value) {m_strSwitchName = value;}; 
    void SetSwitchType(int32 value) {m_nSwitchType = value;};     
    void SetState(int32 value) {m_nState = value;};
    void SetEvbMode(int32 value) {m_nEvbMode = value;};    
    void SetUplinkPgUuid(const char* value) {m_strUplinkPgUuid = value;}; 
    void SetMtuMax(int32 value) {m_nMtuMax = value;}; 
    void SetNicMaxNum(int32 value) {m_nNicMaxNum = value;}; 
    void SetUplinkPortName(const char* value) {m_strUplinkPortName = value;};     
    void SetUplinkPortType(int32 value) {m_strUplinkPortType = value;};   
    void SetBondMode(int32 value) {m_strUplinkPortBondMode = value;}; 

    void DbgShow()
    {
        cout << "Uuid          : " << m_strUuid << endl;
        cout << "SwitchName    : " << m_strSwitchName << endl;
        cout << "SwitchType    : " << m_nSwitchType << endl;
        cout << "State         : " << m_nState << endl;
        cout << "EvbMode       : " << m_nEvbMode << endl;        
        cout << "UplinkPgUuid  : " << m_strUplinkPgUuid << endl;    
        cout << "MtuMax        : " << m_nMtuMax << endl;
        cout << "NicMaxNum     : " << m_nNicMaxNum << endl;
        cout << "UplinkPortName: " << m_strUplinkPortName << endl;
        cout << "UplinkPortType: " << m_strUplinkPortType << endl;  
        cout << "BondMode      : " << m_strUplinkPortBondMode << endl; 
    }
private:
    string m_strUuid;    
    string m_strSwitchName;
    int32  m_nSwitchType;
    int32  m_nState;
    int32  m_nEvbMode;
    string m_strUplinkPgUuid;
    int32  m_nMtuMax;
    int32  m_nNicMaxNum;
    string m_strUplinkPortName;
    int32 m_strUplinkPortType;
    int32 m_strUplinkPortBondMode;
};


class CDbWildcastSwitchBondMap
{
public: 
    CDbWildcastSwitchBondMap() 
    {
        m_strUuid = m_strSwitchName = m_strPgUuid = m_strUplinkPortName = m_strUplinkPortPhyName = "";
        m_nSwitchType = 0;
        m_strUplinkPortBondMode = 1;
     };
    virtual ~CDbWildcastSwitchBondMap() {};

    string & GetUuid(){return m_strUuid;};
    string & GetSwitchName(){return m_strSwitchName;}; 
    int32    GetSwitchType(){return m_nSwitchType;}; 
    string & GetPgUuid(){return m_strPgUuid;}; 
    string & GetUplinkPortName(){return m_strUplinkPortName;}; 
    int32    GetBondMode(){return m_strUplinkPortBondMode;}; 
    string&  GetBondPhyName(){return m_strUplinkPortPhyName;}; 

    void SetUuid(const char* value) {m_strUuid = value;}; 
    void SetSwitchName(const char* value) {m_strSwitchName = value;}; 
    void SetSwitchType(int32 value) {m_nSwitchType = value;}; 
    void SetPgUuid(const char* value) {m_strPgUuid = value;}; 
    void SetUplinkPortName(const char* value) {m_strUplinkPortName = value;}; 
    void SetBondMode(int32 value) {m_strUplinkPortBondMode = value;}; 
    void SetBondPhyName(const char* value) {m_strUplinkPortPhyName = value;};   

    void DbgShow()
    {
        cout << "Uuid          : " << m_strUuid << endl;
        cout << "SwitchName    : " << m_strSwitchName << endl;
        cout << "SwitchType    : " << m_nSwitchType << endl;
        cout << "PgUuid        : " << m_strPgUuid << endl;        
        cout << "UplinkPortName: " << m_strUplinkPortName << endl;
        cout << "BondMode      : " << m_strUplinkPortBondMode << endl;  
        cout << "BondPhyName   : " << m_strUplinkPortPhyName << endl;          
    }
private:
    string m_strUuid;    
    string m_strSwitchName;
    int32  m_nSwitchType;
    string m_strPgUuid;
    string m_strUplinkPortName;
    int32  m_strUplinkPortBondMode;
    string m_strUplinkPortPhyName;
};


class CVNetDbIProcedure;
class CDBOperateWildcastSwitch : public  CVNetDbIProcedureCallback
{
public:
    explicit CDBOperateWildcastSwitch(CVNetDbIProcedure * pb);
    virtual ~CDBOperateWildcastSwitch();

    // 通过uuid来查询
    int32 Query(CDbWildcastSwitch & Info);
    int32 QuerySummary(vector<CDbWildcastSwitch> & outVInfo);
    
    // 通过port_name查询wildcastswitch 信息
    int32 QueryByPortName(const char* pcPortName, vector<CDbWildcastSwitch> & outVInfo);
    
    int32 CheckAdd(CDbWildcastSwitch & info);
    int32 Add(CDbWildcastSwitch & info);
    int32 CheckModify(CDbWildcastSwitch & info);
    int32 Modify(CDbWildcastSwitch & info);
    int32 CheckDel(const char* switch_uuid);
    int32 Del(const char* switch_uuid);

    // 查询wc switch bond map
    int32 QueryBondMap(const char* uuid, vector<CDbWildcastSwitchBondMap> &outVInfo);
    int32 QueryBondMapSummary(vector<CDbWildcastSwitchBondMap> &outVInfo);

    // 通过port_name查询wildcastswitch bond,通过phy port name 
    int32 QueryBondMapByPhyPortName(const char* pcPortName, vector<CDbWildcastSwitchBondMap> & outVInfo);

    int32 CheckAddBondMap(const char* uuid, const char* phy_name);
    int32 AddBondMap(const char* uuid, const char* phy_name); 
    int32 CheckDelBondMap(const char* uuid, const char* phy_name);
    int32 DelBondMap(const char* uuid, const char* phy_name);       

    int32 DbProcCallback(int32 type, CADORecordset *, void* );

    void DbgShow();    
private:
    int32 QueryCallback(CADORecordset * prs,void * nil);
    int32 QuerySummaryCallback(CADORecordset * prs,void * nil);
    int32 QueryBondMapCallback(CADORecordset * prs,void * nil);
    int32 OperateCallback(CADORecordset * prs,void * nil);
    int32 CheckOperateCallback(CADORecordset * prs,void * nil);  

private:
    DISALLOW_COPY_AND_ASSIGN(CDBOperateWildcastSwitch);

    CVNetDbIProcedure * m_pIProc;
};
    
} // namespace zte_tecs

#endif // _VNET_DB_WILDCAST_SWITCH_INCLUDE_H_ 



