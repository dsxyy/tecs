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

#if !defined(_VNET_DB_WILDCAST_TASK_CREATE_VPORT_INCLUDE_H_)
#define _VNET_DB_WILDCAST_TASK_CREATE_VPORT_INCLUDE_H_

#include "vnet_db_in.h"
#include "vnet_db_interface.h"
#include "vnet_db_proc.h"

namespace zte_tecs
{


// vswitch db
class CDbWildcastTaskCreateVPort
{
public: 
    CDbWildcastTaskCreateVPort() 
    {
        m_nState = m_nSendNum = m_nVnaIsOnline= 0;
        m_strUuid = m_strVnaUuid = m_strWcVPortUuid = m_strPortName = "";
        m_nPortType = m_nIsHasKernelType = m_nKernelType = m_nSwitchType= 0;
        m_strKernelPgUuid = m_strSwitchUuid = "";
        m_nIsDhcp = 0;
    };
    virtual ~CDbWildcastTaskCreateVPort() {};
      
    string& GetUuid(){return m_strUuid;}; 
    string& GetVnaUuid(){return m_strVnaUuid;};
    int32 GetVnaIsOnline(){return m_nVnaIsOnline;};  
    string& GetCreateVPortUuid(){return m_strWcVPortUuid;}; 
    int32 GetState(){return m_nState;};  
    int32 GetSendNum(){return m_nSendNum;};  
    
    string& GetPortName(){return m_strPortName;};  
    int32 GetPortType(){return m_nPortType;};  
    int32 GetIsHasKernelType(){return m_nIsHasKernelType;};  
    int32 GetKernelType(){return m_nKernelType;};  
    string& GetKernelPgUuid(){return m_strKernelPgUuid;};
    int32 GetSwitchType(){return m_nSwitchType;};  
    string& GetSwitchUuid(){return m_strSwitchUuid;};        
    int32 GetIsDhcp(){return m_nIsDhcp;};    
      
    void SetUuid(const char* value) {m_strUuid = value;};
    void SetVnaUuid(const char* value) {m_strVnaUuid = value;};
    void SetVnaIsOnline(int32 value) {m_nVnaIsOnline = value;};  
    void SetState(int32 value) {m_nState = value;};   
    void SetSendNum(int32 value) {m_nSendNum = value;};  

    void SetCreateVPortUuid(const char* value) {m_strWcVPortUuid = value;};
    
    void SetPortName(const char*  value) {m_strPortName = value;}; 
    void SetPortType(int32 value) {m_nPortType = value;}; 
    void SetIsHasKernelType(int32 value) {m_nIsHasKernelType = value;}; 
    void SetKernelType(int32 value) {m_nKernelType = value;}; 
    void SetKernelPgUuid(const char*  value) {m_strKernelPgUuid = value;}; 
    void SetSwitchType(int32 value) {m_nSwitchType = value;};     
    void SetSwitchUuid(const char*  value) {m_strSwitchUuid = value;};     
    void SetIsDhcp(int32  value) {m_nIsDhcp = value;};    

    void DbgShow()
    {
        cout << "Uuid            : " << m_strUuid << endl;  
        cout << "State           : " << m_nState << endl; 
        cout << "SendNum         : " << m_nSendNum << endl; 
        cout << "VnaUuid         : " << m_strVnaUuid << endl; 
        cout << "VnaIsOnline     : " << m_nVnaIsOnline << endl;
        cout << "CreateVPortUuid : " << m_strWcVPortUuid << endl;
        cout << "PortName        : " << m_strPortName << endl;
        cout << "PortType        : " << m_nPortType << endl;
        cout << "IsHasKernelType : " << m_nIsHasKernelType << endl;
        cout << "KernelType      : " << m_nKernelType << endl;
        cout << "KernelPgUuid    : " << m_strKernelPgUuid << endl;
        cout << "SwitchType      : " << m_nSwitchType << endl;        
        cout << "switchUuid      : " << m_strSwitchUuid << endl;        
        cout << "IsDhcp          : " << m_nIsDhcp << endl;         
    }
private:    

    string m_strUuid;
    string m_strVnaUuid;
    int32  m_nVnaIsOnline;
    string m_strWcVPortUuid;    
    int32 m_nState;
    int32 m_nSendNum;    

    string m_strPortName;
    int32 m_nPortType;
    int32 m_nIsHasKernelType;    
    int32 m_nKernelType;
    string m_strKernelPgUuid;
    int32 m_nSwitchType;
    string m_strSwitchUuid;
    int32 m_nIsDhcp;
};



class CVNetDbIProcedure;
class CDBOperateWildcastTaskCreateVPort : public  CVNetDbIProcedureCallback
{
public:
    explicit CDBOperateWildcastTaskCreateVPort(CVNetDbIProcedure * pb);
    virtual ~CDBOperateWildcastTaskCreateVPort();

    int32 QuerySummaryByVna(const char* uuid, vector<CDbWildcastTaskCreateVPort> & outVInfo);
    int32 QuerySummary( vector<CDbWildcastTaskCreateVPort> & outVInfo);  
    int32 Query(CDbWildcastTaskCreateVPort & Info);
    
    int32 CheckAdd(CDbWildcastTaskCreateVPort & info);
    int32 Add(CDbWildcastTaskCreateVPort & info);
    int32 CheckModify(CDbWildcastTaskCreateVPort & info);
    int32 Modify(CDbWildcastTaskCreateVPort & info);
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
    DISALLOW_COPY_AND_ASSIGN(CDBOperateWildcastTaskCreateVPort);

    CVNetDbIProcedure * m_pIProc;
};
    
} // namespace zte_tecs

#endif // _VNET_DB_WILDCAST_TASK_CREATE_VPORT_INCLUDE_H_ 


