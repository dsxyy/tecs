
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

#if !defined(_VNET_DB_VNA_INCLUDE_H_)
#define _VNET_DB_VNA_INCLUDE_H_

#include "vnet_db_in.h"
#include "vnet_db_interface.h"
#include "vnet_db_proc.h"

namespace zte_tecs
{
class CDbVnaSummary
{
public:
    CDbVnaSummary(){m_strUuid = m_strVnaApp = "";};
    virtual ~CDbVnaSummary() {};

    string & GetUuid(){return m_strUuid;};
    string & GetApp(){return m_strVnaApp;};   
    void SetUuid(const char* uuid) {m_strUuid = uuid;};
    void SetApp(const char* name) {m_strVnaApp = name;};    

    void DbgShow()
    {
        cout << "|" <<setw(32) << m_strUuid << 
                    "|" << setw(32) << m_strVnaApp << endl;
    }
private:    
    string m_strUuid;
    string m_strVnaApp;
};



class CDbVna
{
public: 
    CDbVna() 
    {
        m_strUuid = m_strVnaApp = m_strVnmApp = m_strHostName = m_strOsName = ""; 
        m_nIsOnline = false;
    };
    virtual ~CDbVna() {};

    string & GetUuid(){return m_strUuid;};
    string & GetVnaApp(){return m_strVnaApp;};   
    string & GetVnmApp(){return m_strVnmApp;};   
    string & GetHostName(){return m_strHostName;};       
    string & GetOsName(){return m_strOsName;};    
    int32  GetIsOnline() {return m_nIsOnline;};
    
    void SetUuid(const char* uuid) {m_strUuid = uuid;};
    void SetVnaApp(const char* name) {m_strVnaApp = name;};   
    void SetVnmApp(const char* name) {m_strVnmApp = name;};  
    void SetHostName(const char* name) {m_strHostName = name;};  
    void SetOsName(const char* name) {m_strOsName = name;}; 
    void SetIsOnline(int32 isCfg) {m_nIsOnline = isCfg;};
    
    void DbgShow()
    {
        cout << "uuid        : " << m_strUuid << endl;
        cout << "vna_app     : " << m_strVnaApp << endl;       
        cout << "vnm_app     : " << m_strVnmApp << endl; 
        cout << "host_name   : " << m_strHostName << endl;         
        cout << "os_name     : " << m_strOsName << endl;
        cout << "is_online   : " << m_nIsOnline << endl;        
    }
private:    
    string m_strUuid;
    string m_strVnaApp;
    string m_strVnmApp;
    string m_strHostName;    
    string m_strOsName;    
    int32 m_nIsOnline;
};

class CVNetDbIProcedure;
class CDBOperateVna : public  CVNetDbIProcedureCallback
{
public:
    explicit CDBOperateVna(CVNetDbIProcedure * pb);
    virtual ~CDBOperateVna();

    int32 QueryVnaSummary(vector<CDbVnaSummary> & outVInfo);  
    int32 QueryVna(CDbVna & Info);
    int32 AddVna(CDbVna & info);
    int32 ModifyVna(CDbVna & info);
    int32 DelVna(const char* vna_uuid);

    int32 DbProcCallback(int32 type, CADORecordset *, void* );

    void DbgShow();    
private:
    int32 QueryVnaSummaryCallback(CADORecordset * prs,void * nil);
    int32 QueryVnaCallback(CADORecordset * prs,void * nil);
    int32 OperateVnaCallback(CADORecordset * prs,void * nil);
    
private:
    DISALLOW_COPY_AND_ASSIGN(CDBOperateVna);

    CVNetDbIProcedure * m_pIProc;
};
    
} // namespace zte_tecs

#endif // _VNET_DB_VNA_INCLUDE_H_ 

