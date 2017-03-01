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

#if !defined(_VNET_DB_WILDCAST_CREATE_VPORT_INCLUDE_H_)
#define _VNET_DB_WILDCAST_CREATE_VPORT_INCLUDE_H_

#include "vnet_db_in.h"
#include "vnet_db_interface.h"
#include "vnet_db_proc.h"

namespace zte_tecs
{


// vswitch db
class CDbWildcastCreateVPort
{
public: 
    CDbWildcastCreateVPort() 
    {
        m_nIsHasKernelType = m_nKernelType = m_nPortType = m_nIsDhcp=  0;
        m_strUuid = m_strPortName = "";
        m_strKernelPgUuid = m_strSwitchUuid = "";
     };
    virtual ~CDbWildcastCreateVPort() {};
      
    string & GetUuid(){return m_strUuid;}; 
    string& GetPortName(){return m_strPortName;}; 
    int32 GetPortType(){return m_nPortType;};  
    int32 GetIsHasKernelType(){return m_nIsHasKernelType;};
    int32 GetKernelType(){return m_nKernelType;};
    string& GetKernelPgUuid(){return m_strKernelPgUuid;};
    string& GetSwitchUuid(){return m_strSwitchUuid;};
    int32 GetIsDhcp(){return m_nIsDhcp;};    
      
    void SetUuid(const char* value) {m_strUuid = value;};
    void SetPortName(const char* value) {m_strPortName = value;};
    void SetPortType(int32 value) {m_nPortType = value;};  
    void SetIsHasKernelType(int32 value) {m_nIsHasKernelType = value;};   
    void SetKernelType(int32 value) {m_nKernelType = value;};       
    void SetKernelPgUuid(const char* value) {m_strKernelPgUuid = value;}; 
    void SetSwitchUuid(const char* value) {m_strSwitchUuid = value;};  
    void SetIsDhcp(int32 value) {m_nIsDhcp = value;};      

    void DbgShow()
    {
        cout << "Uuid           : " << m_strUuid << endl; 
        cout << "PortName       : " << m_strPortName << endl; 
        cout << "PortType       : " << m_nPortType << endl; 
        cout << "IsHasKernelType: " << m_nIsHasKernelType << endl; 
        cout << "KernelType     : " << m_nKernelType << endl;         
        cout << "knlPgUuid : " << m_strKernelPgUuid << endl;
        cout << "SwitchUuid: " << m_strSwitchUuid << endl;  
        cout << "IsDhcp: " << m_nIsDhcp << endl;          
    }
private:    

    string m_strUuid;
    string m_strPortName;
    int32 m_nPortType;
    int32 m_nIsHasKernelType;    
    int32 m_nKernelType;
    string m_strKernelPgUuid; // kernel tap, kernel_pg_uuid
    string m_strSwitchUuid; // switch name;
    int32 m_nIsDhcp;
};


class CVNetDbIProcedure;
class CDBOperateWildcastCreateVPort : public  CVNetDbIProcedureCallback
{
public:
    explicit CDBOperateWildcastCreateVPort(CVNetDbIProcedure * pb);
    virtual ~CDBOperateWildcastCreateVPort();

    int32 QuerySummary( vector<CDbWildcastCreateVPort> & outVInfo);  
    int32 Query(CDbWildcastCreateVPort & Info);
    int32 CheckAdd(CDbWildcastCreateVPort & info);
    int32 Add(CDbWildcastCreateVPort & info);
    //int32 CheckModify(CDbWildcastCreateVPort & info);
    //int32 Modify(CDbWildcastCreateVPort & info);
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
    DISALLOW_COPY_AND_ASSIGN(CDBOperateWildcastCreateVPort);

    CVNetDbIProcedure * m_pIProc;
};
    
} // namespace zte_tecs

#endif // _VNET_DB_WILDCAST_CREATE_VPORT_INCLUDE_H_ 


