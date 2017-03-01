

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

#if !defined(_VNET_DB_LOGICNETWORK_INCLUDE_H_)
#define _VNET_DB_LOGICNETWORK_INCLUDE_H_

#include "vnet_db_in.h"
#include "vnet_db_interface.h"
#include "vnet_db_proc.h"

namespace zte_tecs
{


// 和表中约束一致 
typedef enum tagDbIpTypeValues
{
    EN_DB_IP_MODE_STATIC = 0,
    EN_DB_IP_MODE_DYNAMIC, 
    EN_DB_IP_MODE_NONE,
}EN_DB_IP_TYPE_VALUES;

// logicnetwork db
class CDbLogicNetwork
{
public: 
    CDbLogicNetwork() 
    {
        m_strUuid = m_strName = m_strPgUuid = m_strDes = "";
        m_nIpMode = 0;
     };
    virtual ~CDbLogicNetwork() {};
     
    string & GetUuid(){return m_strUuid;}; 
    string & GetName(){return m_strName;};
    string & GetPgUuid(){return m_strPgUuid;};
    string & GetDes(){return m_strDes;};
    int32  GetIpMode(){return m_nIpMode;};
     
    void SetUuid(const char* value) {m_strUuid = value;};
    void SetName(const char* value) {m_strName = value;};
    void SetPgUuid(const char* value) {m_strPgUuid = value;};
    void SetDes(const char* value) {m_strDes = value;};
    void SetIpMode(int32 value) {m_nIpMode = value;};    

    void DbgShow()
    {       
        cout << "Uuid  : " << m_strUuid << endl;   
        cout << "Name  : " << m_strName << endl; 
        cout << "PgUuid: " << m_strPgUuid << endl; 
        cout << "Des   : " << m_strDes << endl;     
        cout << "IpMode: " << m_nIpMode << endl;     
    }
private:    

    string m_strUuid;
    string m_strName;
    string m_strPgUuid;
    string m_strDes;
    int32  m_nIpMode;
};

class CDbLogicNetworkPgNetplane
{
public: 
    CDbLogicNetworkPgNetplane() 
    {
        m_strNetplaneUuid = m_strNetplaneName = "";
        m_strPgUuid = m_strPgName = "";
        m_strLogicnetworkUuid = m_strLogicnetworkName = "";

        m_nNetplaneMtu = m_nPgMtu = m_nLogicnetworkIpMode =0;
    };
    virtual ~CDbLogicNetworkPgNetplane() {};
     
    string & GetNetplaneUuid(){return m_strNetplaneUuid;}; 
    string & GetNetplaneName(){return m_strNetplaneName;};
    int32  GetNetplaneMtu(){return m_nNetplaneMtu;};

    string & GetPgUuid(){return m_strPgUuid;}; 
    string & GetPgName(){return m_strPgName;};
    int32  GetPgMtu(){return m_nPgMtu;};

    string & GetLogicnetworkUuid(){return m_strLogicnetworkUuid;}; 
    string & GetLogicnetworkName(){return m_strLogicnetworkName;};
    int32  GetLogicnetworkIpMode(){return m_nLogicnetworkIpMode;};
    
     
    void SetNetplaneUuid(const char* value) {m_strNetplaneUuid = value;};
    void SetNetplaneName(const char* value) {m_strNetplaneName = value;};
    void SetNetplaneMtu(int32 value) {m_nNetplaneMtu = value;};

    void SetPgUuid(const char* value) {m_strPgUuid = value;};
    void SetPgName(const char* value) {m_strPgName = value;};
    void SetPgMtu(int32 value) {m_nPgMtu = value;};

    void SetLogicnetworkUuid(const char* value) {m_strLogicnetworkUuid = value;};
    void SetLogicnetworkName(const char* value) {m_strLogicnetworkName = value;};
    void SetLogicnetworkIpMode(int32 value) {m_nLogicnetworkIpMode = value;};

    void DbgShow()
    {       
        cout << "NetplaneUuid    : " << m_strNetplaneUuid << endl;   
        cout << "NetplaneName    : " << m_strNetplaneName << endl; 
        cout << "NetplaneMtu     : " << m_nNetplaneMtu << endl; 

        cout << "PortgroupUuid   : " << m_strPgUuid << endl;   
        cout << "PortgroupName   : " << m_strPgName << endl; 
        cout << "PortgroupMtu    : " << m_nPgMtu << endl; 

        cout << "LogicnetworkUuid: " << m_strLogicnetworkUuid << endl;   
        cout << "LogicnetworkName: " << m_strLogicnetworkName << endl; 
        cout << "LogicnetworkIpMode: " << m_nLogicnetworkIpMode << endl; 
    }
private:    

    string m_strNetplaneUuid;
    string m_strNetplaneName;
    int32  m_nNetplaneMtu;

    string m_strPgUuid;
    string m_strPgName;
    int32  m_nPgMtu;

    string m_strLogicnetworkUuid;
    string m_strLogicnetworkName;
    int32 m_nLogicnetworkIpMode;
};

class CVNetDbIProcedure;
class CDBOperateLogicNetwork : public  CVNetDbIProcedureCallback
{
public:
    explicit CDBOperateLogicNetwork(CVNetDbIProcedure * pb);
    virtual ~CDBOperateLogicNetwork();

    int32 QueryByNetplane(const char* uuid, vector<CDbLogicNetworkPgNetplane> & outVInfo);  
    int32 QueryByPortGroup(const char* uuid, vector<CDbLogicNetworkPgNetplane> & outVInfo);

    int32 QueryByName(CDbLogicNetwork & Info);
    
    int32 Query(CDbLogicNetwork & Info);
    int32 Add(CDbLogicNetwork & info);
    int32 Modify(CDbLogicNetwork & info);
    int32 Del(const char* uuid);

    int32 CheckAdd(CDbLogicNetwork & info);
    int32 CheckModify(CDbLogicNetwork & info);
    int32 CheckDel(const char* uuid);

    int32 DbProcCallback(int32 type, CADORecordset *, void* );

    void DbgShow();    
private:
    int32 QueryByCallback(CADORecordset * prs,void * nil);    
    int32 QueryCallback(CADORecordset * prs,void * nil);
    int32 OperateCallback(CADORecordset * prs,void * nil);
    int32 CheckOperateCallback(CADORecordset * prs,void * nil);
    
private:
    DISALLOW_COPY_AND_ASSIGN(CDBOperateLogicNetwork);

    CVNetDbIProcedure * m_pIProc;
};
    
} // namespace zte_tecs

#endif // _VNET_DB_LOGICNETWORK_INCLUDE_H_ 



