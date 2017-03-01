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

#if !defined(_VNET_DB_WILDCAST_TASK_SWITCH_INCLUDE_H_)
#define _VNET_DB_WILDCAST_TASK_SWITCH_INCLUDE_H_

#include "vnet_db_in.h"
#include "vnet_db_interface.h"
#include "vnet_db_proc.h"

namespace zte_tecs
{


// vswitch db
class CDbWildcastTaskSwitch
{
public: 
    CDbWildcastTaskSwitch() 
    {
        m_nState = m_nSendNum = 0;
        m_strUuid = m_strVnaUuid = m_strWcSwitchUuid = "";
    };
    virtual ~CDbWildcastTaskSwitch() {};
      
    string& GetUuid(){return m_strUuid;}; 
    string& GetVnaUuid(){return m_strVnaUuid;};
    string& GetSwitchUuid(){return m_strWcSwitchUuid;}; 
    int32 GetState(){return m_nState;};  
    int32 GetSendNum(){return m_nSendNum;};  
      
    void SetUuid(const char* value) {m_strUuid = value;};
    void SetVnaUuid(const char* value) {m_strVnaUuid = value;};
    void SetSwitchUuid(const char* value) {m_strWcSwitchUuid = value;};
    void SetState(int32 value) {m_nState = value;};   
    void SetSendNum(int32 value) {m_nSendNum = value;};   

    void DbgShow()
    {
        cout << "Uuid        : " << m_strUuid << endl; 
        cout << "VnaUuid     : " << m_strVnaUuid << endl; 
        cout << "SwitchUuid  : " << m_strWcSwitchUuid << endl; 
        cout << "State       : " << m_nState << endl; 
        cout << "SendNum     : " << m_nSendNum << endl; 
    }
private:    

    string m_strUuid;
    string m_strVnaUuid;
    string m_strWcSwitchUuid;    
    int32 m_nState;
    int32 m_nSendNum;    
};

class CDbWildcastTaskSwitchDetail
{
public: 
    CDbWildcastTaskSwitchDetail() 
    {
        m_nTaskState = m_nTaskSendNum = m_nSwitchType = m_nMtuMax = m_nNicMaxNum = m_nPortType =m_nBondMode = m_nVnaIsOnline = 0;
        m_strTaskUuid = m_strSwitchUuid = m_strSwitchName = m_strPgUuid = m_strPortName = m_strVnaUuid = "";
    };
    virtual ~CDbWildcastTaskSwitchDetail() {};
      
    string& GetUuid(){return m_strTaskUuid;}; 
    int32 GetTaskState(){return m_nTaskState;};  
    int32 GetTaskSendNum(){return m_nTaskSendNum;};  
    string& GetSwitchUuid(){return m_strSwitchUuid;};
    string& GetSwitchName(){return m_strSwitchName;}; 
    int32 GetSwitchType(){return m_nSwitchType;};  
    string& GetPgUuid(){return m_strPgUuid;}; 
    int32 GetMtuMax(){return m_nMtuMax;};  
    int32 GetNicMaxNum(){return m_nNicMaxNum;};  
    string& GetPortName(){return m_strPortName;};  
    int32 GetPortType(){return m_nPortType;};  
    int32 GetBondMode(){return m_nBondMode;};  
    string& GetVnaUuid(){return m_strVnaUuid;};  
    int32 GetVnaIsOnline(){return m_nVnaIsOnline;};  
      
    void SetUuid(const char* value) {m_strTaskUuid = value;};
    void SetTaskState(int32 value) {m_nTaskState = value;};
    void SetTaskSendNum(int32 value) {m_nTaskSendNum = value;};
    void SetSwitchUuid(const char* value) {m_strSwitchUuid = value;};   
    void SetSwitchName(const char* value) {m_strSwitchName = value;};  
    void SetSwitchType(int32 value) {m_nSwitchType = value;};   
    void SetPgUuid(const char* value) {m_strPgUuid = value;};  
    void SetMtuMax(int32 value) {m_nMtuMax = value;};  
    void SetNicMaxNum(int32 value) {m_nNicMaxNum = value;};
    void SetPortName(const char* value) {m_strPortName = value;};  
    void SetPortType(int32 value) {m_nPortType = value;};  
    void SetBondMode(int32 value) {m_nBondMode = value;};
    void SetVnaUuid(const char* value) {m_strVnaUuid = value;};
    void SetVnaIsOnline (int32 value) {m_nVnaIsOnline = value;};
    
    void DbgShow()
    {
        cout << "Uuid       : " << m_strTaskUuid << endl; 
        cout << "TaskState  : " << m_nTaskState << endl; 
        cout << "TaskSendNum: " << m_nTaskSendNum << endl; 
        cout << "SwitchUuid : " << m_strSwitchUuid << endl; 
        cout << "SwitchName : " << m_strSwitchName << endl; 
        cout << "SwitchType : " << m_nSwitchType << endl; 
        cout << "PgUuid     : " << m_strPgUuid << endl; 
        cout << "MtuMax     : " << m_nMtuMax << endl; 
        cout << "NicMaxNum  : " << m_nNicMaxNum << endl; 
        cout << "PortName   : " << m_strPortName << endl; 
        cout << "PortType   : " << m_nPortType << endl; 
        cout << "BondMode   : " << m_nBondMode << endl; 
        cout << "VnaUuid    : " << m_strVnaUuid << endl; 
        cout << "VnaIsOnline: " << m_nVnaIsOnline << endl; 
    }
private:    

    string m_strTaskUuid;
    int32  m_nTaskState;
    int32  m_nTaskSendNum;    
    string m_strSwitchUuid;
    string m_strSwitchName;
    int32  m_nSwitchType;
    string m_strPgUuid;    
    int32 m_nMtuMax;
    int32 m_nNicMaxNum; 
    string m_strPortName;
    int32 m_nPortType; 
    int32 m_nBondMode; 
    string m_strVnaUuid;
    int32 m_nVnaIsOnline;

};

class CVNetDbIProcedure;
class CDBOperateWildcastTaskSwitch : public  CVNetDbIProcedureCallback
{
public:
    explicit CDBOperateWildcastTaskSwitch(CVNetDbIProcedure * pb);
    virtual ~CDBOperateWildcastTaskSwitch();

    int32 QuerySummaryByVna(const char* uuid, vector<CDbWildcastTaskSwitchDetail> & outVInfo);
    int32 QuerySummary( vector<CDbWildcastTaskSwitchDetail> & outVInfo);  
    int32 Query(CDbWildcastTaskSwitchDetail & Info);
    
    int32 CheckAdd(CDbWildcastTaskSwitch & info);
    int32 Add(CDbWildcastTaskSwitch & info);
    int32 CheckModify(CDbWildcastTaskSwitch & info);
    int32 Modify(CDbWildcastTaskSwitch & info);
    int32 CheckDel(const char* uuid);
    int32 Del(const char* uuid);

    int32 DbProcCallback(int32 type, CADORecordset *, void* );

    void DbgShow();    
private:
    int32 QuerySummaryCallback(CADORecordset * prs,void * nil);
    int32 QueryCallback(CADORecordset * prs,void * nil);
    int32 OperateCallback(CADORecordset * prs,void * nil);
    int32 CheckOperateCallback(CADORecordset * prs,void * nil);    
    
private:
    DISALLOW_COPY_AND_ASSIGN(CDBOperateWildcastTaskSwitch);

    CVNetDbIProcedure * m_pIProc;
};
    
} // namespace zte_tecs

#endif // _VNET_DB_WILDCAST_TASK_SWITCH_INCLUDE_H_ 


