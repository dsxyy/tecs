
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

#if !defined(_VNET_DB_REG_MODULE_INCLUDE_H_)
#define _VNET_DB_REG_MODULE_INCLUDE_H_

#include "vnet_db_in.h"
#include "vnet_db_interface.h"
#include "vnet_db_proc.h"

namespace zte_tecs
{
class CDbVnaRegModuleSummary
{
public:
    CDbVnaRegModuleSummary(){m_strVnaUuid = m_strModuleUuid = m_strModuleName = "";};
    virtual ~CDbVnaRegModuleSummary() {};

    string & GetVnaUuid(){return m_strVnaUuid;};
    string & GetModuleUuid(){return m_strModuleUuid;};    
    string & GetModuleName(){return m_strModuleName;};   
    void SetVnaUuid(const char* uuid) {m_strVnaUuid = uuid;};
    void SetModuleUuid(const char* uuid) {m_strModuleUuid = uuid;};    
    void SetModuleName(const char* name) {m_strModuleName = name;};    

    void DbgShow()
    {
        cout << "|" <<setw(32) << m_strVnaUuid << "|" << setw(32) << m_strModuleUuid <<
                    "|" << setw(32) << m_strModuleName << endl;
    }
private:    
    string m_strVnaUuid;
    string m_strModuleUuid;    
    string m_strModuleName;
};


class CDbVnaRegistModule
{
public:
    CDbVnaRegistModule()
    {
        m_strVnaUuid = m_strModuleName = m_strModuleUuid = m_strClusterName = "";
        m_nRole = m_nIsOnline = m_nIsHasHcInfo= 0;
    };
    virtual ~CDbVnaRegistModule() {};

    string & GetVnaUuid(){return m_strVnaUuid;};
    string & GetModuleName(){return m_strModuleName;};   
    string & GetModuleUuid(){return m_strModuleUuid;};       
    int32  GetRole(){return m_nRole;};
    int32  GetIsOnline(){return m_nIsOnline;};
    int32  GetIsHasHcInfo(){return m_nIsHasHcInfo;};  
    string & GetClusterName(){return m_strClusterName;};  
    
    void SetVnaUuid(const char* uuid) {m_strVnaUuid = uuid;};
    void SetModuleName(const char* name) {m_strModuleName = name;}; 
    void SetModuleUuid(const char* uuid) {m_strModuleUuid = uuid;};
    void SetRole(int32 role) {m_nRole = role;};
    void SetIsOnline(int32 online) {m_nIsOnline = online;};    
    void SetIsHasHcInfo(int32 isHas) {m_nIsHasHcInfo = isHas;};     
    void SetClusterName(const char* name) {m_strClusterName = name;};
    
    void DbgShow()
    {
        cout << "  vna_uuid      : " << m_strVnaUuid << endl;
        cout << "  module_name   : " << m_strModuleName << endl;
        cout << "  module_uuid   : " << m_strModuleUuid << endl;
        cout << "  role          : " << m_nRole << endl;       
        cout << "  IsOnline      : " << m_nIsOnline << endl; 
        cout << "  is_has_HC_info: " << m_nIsHasHcInfo << endl;              
        cout << "  cluster_name  : " << m_strClusterName << endl;  
    }
private: 
    string m_strVnaUuid;
    string m_strModuleName;
    string m_strModuleUuid;   
    int32 m_nRole; 
    int32 m_nIsOnline;

    int32 m_nIsHasHcInfo;
    string m_strClusterName; // if m_nIsHasHcInfo = 1, valid.
};


class CVNetDbIProcedure;
class CDBOperateVnaRegModule : public  CVNetDbIProcedureCallback
{
public:
    explicit CDBOperateVnaRegModule(CVNetDbIProcedure * pb);
    virtual ~CDBOperateVnaRegModule();

    int32 QueryRegModuleSummary(vector<CDbVnaRegModuleSummary> & outVInfo);  
    int32 QueryRegModule(CDbVnaRegistModule & Info);
    int32 AddRegModule(CDbVnaRegistModule & info);
    int32 ModifyRegModule(CDbVnaRegistModule & info);
    int32 DelRegModule(const char* reg_module_uuid);

    int32 DbProcCallback(int32 type, CADORecordset *, void* );

    void DbgShow();    
private:
    int32 QueryRegModuleSummaryCallback(CADORecordset * prs,void * nil);
    int32 QueryRegModuleCallback(CADORecordset * prs,void * nil);
    int32 OperateRegModuleCallback(CADORecordset * prs,void * nil);
    
private:
    DISALLOW_COPY_AND_ASSIGN(CDBOperateVnaRegModule);

    CVNetDbIProcedure * m_pIProc;
};
    
} // namespace zte_tecs

#endif // _VNET_DB_REG_MODULE_INCLUDE_H_ 

