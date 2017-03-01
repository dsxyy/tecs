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

#if !defined(_VNET_DB_VLAN_MAP_INCLUDE_H_)
#define _VNET_DB_VLAN_MAP_INCLUDE_H_

#include "vnet_db_in.h"
#include "vnet_db_interface.h"
#include "vnet_db_proc.h"
#include "vnet_db_switch.h"

namespace zte_tecs
{

// vswitch db
class CDbVlanMap
{
public: 
    CDbVlanMap() 
    {
        m_strNetplaneUuid = m_strNetplaneName = "";
        m_strPgUuid = m_strPgName = "";
        m_nVlanNum = m_nIsolateNo = 0;
        m_oProjectId = 0;
    };
    virtual ~CDbVlanMap() {};
      
    string & GetNetplaneUuid(){return m_strNetplaneUuid;}; 
    string & GetNetplaneName(){return m_strNetplaneName;}; 
    string & GetPgUuid(){return m_strPgUuid;}; 
    string & GetPgName(){return m_strPgName;}; 
    int64 GetProjectId(){return m_oProjectId;}; 
    int32 GetVlanNum(){return m_nVlanNum;};  
    int32 GetIsolateNo(){return m_nIsolateNo;}; 

    void SetNetplaneUuid(const char* value) {m_strNetplaneUuid = value;};
    void SetNetplaneName(const char* value) {m_strNetplaneName = value;};
    void SetPgUuid(const char* value) {m_strPgUuid = value;};
    void SetPgName(const char* value) {m_strPgName = value;};

    void SetProjectId(int64 value) {m_oProjectId = value;};
    void SetVlanNum(int32 value) {m_nVlanNum = value;};
    void SetIsolateNo(int32 value) {m_nIsolateNo = value;};

    void DbgShow()
    {       
        cout << "NetplaneUuid: " << m_strNetplaneUuid << endl;   
        cout << "NetplaneName: " << m_strNetplaneName << endl; 

        cout << "PgUuid      : " << m_strPgUuid << endl; 
        cout << "PgName      : " << m_strPgName << endl;         
   
        cout << "ProjectId   : " << m_oProjectId << endl; 
        cout << "VlanNum     : " << m_nVlanNum << endl;
        cout << "IsolateNo   : " << m_nIsolateNo << endl;
    }
private:    
    string m_strNetplaneUuid;
    string m_strNetplaneName;
    string m_strPgUuid;
    string m_strPgName;
    int64 m_oProjectId;  
    int32 m_nVlanNum;
    int32 m_nIsolateNo;
};


class CVNetDbIProcedure;
class CDBOperateVlanMap : public  CVNetDbIProcedureCallback 
{
public:
    explicit CDBOperateVlanMap(CVNetDbIProcedure * pb);
    virtual ~CDBOperateVlanMap();
 
    int32 QueryByProjectId(int64 project_id, vector<CDbVlanMap> & outVInfo);  
    int32 QueryByPortGroup(const char* pg_uuid, vector<CDbVlanMap> & outVInfo);    
    int32 QueryByProjectIdPG(int64 project_id,const char* pg_uuid, vector<CDbVlanMap> & outVInfo);    
    int32 QueryByProjectIdPGIsolateNo(int64 project_id,const char* pg_uuid, int32 isolateNo, int32& outVlanNum);    
    int32 QueryByNetplane(const char* uuid, vector<CDbVlanMap> & outVInfo);    
    int32 QueryByProjectIdIsolateNo(int64 project_id,int32 isolateNo, vector<CDbVlanMap> & outVInfo); 
    int32 QueryByNpProjectIdIsolateNo(const char* np_uuid, int64 project_id,int32 isolateNo, vector<CDbVlanMap> & outVInfo);
    
    int32 CheckAdd(int64 projectId,const char* pg_uuid, int32 vlan_num,int32 IsolateNo);
    int32 Add(int64 projectId,const char* pg_uuid, int32 vlan_num,int32 IsolateNo);
    int32 CheckDel(int64 projectId,const char* pg_uuid, int32 vlan_num,int32 IsolateNo);
    int32 Del(int64 projectId,const char* pg_uuid, int32 vlan_num,int32 IsolateNo);

    int32 DbProcCallback(int32 type, CADORecordset *, void* );

    void DbgShow();    
private:
    int32 QueryCallback(CADORecordset * prs,void * nil);
    
    int32 OperateCallback(CADORecordset * prs,void * nil);
    int32 CheckOperateCallback(CADORecordset * prs,void * nil);    
    
private:
    DISALLOW_COPY_AND_ASSIGN(CDBOperateVlanMap);

    CVNetDbIProcedure * m_pIProc;
};
    
} // namespace zte_tecs

#endif // _VNET_DB_VLAN_MAP_INCLUDE_H_ 


