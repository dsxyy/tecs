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

#if !defined(_VNET_DB_PORT_INCLUDE_H_)
#define _VNET_DB_PORT_INCLUDE_H_

#include "vnet_db_in.h"
#include "vnet_db_interface.h"
#include "vnet_db_proc.h"


namespace zte_tecs
{

// 和表中约束一致 
typedef enum tagDbPortTypeValues
{
    EN_DB_PORT_TYPE_PHYSICAL = 0,
    EN_DB_PORT_TYPE_BOND,     
    EN_DB_PORT_TYPE_SRIOVVF,
    EN_DB_PORT_TYPE_KERNEL,
    EN_DB_PORT_TYPE_UPLINKLOOP,
    EN_DB_PORT_TYPE_VSWITCH,
    EN_DB_PORT_TYPE_VTEP,
}EN_DB_PORT_TYPE_VALUES;

typedef enum tagDbKernelPgTypeValues
{
    EN_DB_KERNEL_PG_TYPE_MGR = 10,
    EN_DB_KERNEL_PG_TYPE_STORAGE, 
}EN_DB_KERNEL_PG_TYPE_VALUES;

typedef enum tagDbKernelTypeValues
{
    EN_DB_KERNEL_TYPE_MGR = 10,
    EN_DB_KERNEL_TYPE_STORAGE, 
}EN_DB_KERNEL_TYPE_VALUES;


#define DB_PORT_TABLE_FIELD_NUM (16)

// port db values
class CDbPortSummary
{
public:
    CDbPortSummary(){m_strVnaUuid = m_strUuid = m_strName = ""; m_nPortType = 0;};
    virtual ~CDbPortSummary() {};

    string & GetVnaUuid(){return m_strVnaUuid;};
    string & GetUuid(){return m_strUuid;};
    string & GetName(){return m_strName;};  
    int32  GetPortType(){return m_nPortType;}; 
    void SetVnaUuid(const char* uuid) {m_strVnaUuid = uuid;};
    void SetUuid(const char* uuid) {m_strUuid = uuid;};
    void SetName(const char* name) {m_strName = name;};    
    void SetPortType(int32 type) {m_nPortType = type;};

    void DbgShow()
    {
        cout << "|" <<setw(32) << m_strVnaUuid << 
                    "|" <<setw(32) << m_strUuid << 
                    "|" << setw(32) << m_strName << 
                    "|" << setw(5) << m_nPortType <<endl;
    }
private:    
    string m_strVnaUuid;
    string m_strUuid;
    string m_strName;
    int32  m_nPortType;
};

class CDbPort  
{
public: 
    CDbPort() 
    {
        m_strUuid = m_strName = m_strVnaUuid = "";
        m_nPortType = m_nState = m_nIsBroadcast = m_nIsRunning = m_nIsMulticast = m_nPromiscuous = m_nMtu = 0;
        m_nIsMaster = m_nAdminState = m_nIsOnline = m_nIsPortCfg = m_nIsConsistency = 0; 
     };
    virtual ~CDbPort() {};
    
    string & GetVnaUuid(){return m_strVnaUuid;};
    int32 GetPortType() {return m_nPortType;}; 
    string & GetName(){return m_strName;};     
    string & GetUuid(){return m_strUuid;};
    int32 GetState(){return m_nState;};
    int32 GetIsBroadcast(){return m_nIsBroadcast;};
    int32 GetIsRunning(){return m_nIsRunning;};
    int32 GetIsMulticast(){return m_nIsMulticast;};
    int32 GetPromiscuous(){return m_nPromiscuous;};
    int32 GetMtu(){return m_nMtu;};
    int32 GetIsMaster(){return m_nIsMaster;};
    int32 GetAdminState(){return m_nAdminState;};    
    int32 GetIsOnline(){return m_nIsOnline;};    
    int32 GetIsPortCfg(){return m_nIsPortCfg;};
    int32 GetIsConsistency(){return m_nIsConsistency;};

    void SetVnaUuid(const char* uuid) {m_strVnaUuid = uuid;};
    void SetPortType(int32 type) {m_nPortType = type;};  
    void SetName(const char* name) {m_strName = name;};
    void SetUuid(const char* uuid) {m_strUuid = uuid;};    
    void SetState(int32 state) {m_nState = state;};         
    void SetIsBroadcast(int32 broadcast) {m_nIsBroadcast = broadcast;};    
    void SetIsRunning(int32 running) {m_nIsRunning = running;};    
    void SetIsMulticast(int32 mult) {m_nIsMulticast = mult;};    
    void SetPromiscuous(int32 prom) {m_nPromiscuous = prom;};      
    void SetMtu(int32 mtu) {m_nMtu = mtu;};        
    void SetIsMaster(int32 master) {m_nIsMaster = master;};    
    void SetAdminState(int32 state) {m_nAdminState = state;};       
    void SetIsOnline(int32 isonline) {m_nIsOnline = isonline;}; 
    void SetIsPortCfg(int32 value) {m_nIsPortCfg = value;}; 
    void SetIsConsistency(int32 value) {m_nIsConsistency = value;};     

    void DbgPortShow()
    {
        cout << "vna uuid    : " << m_strVnaUuid << endl;
        cout << "uuid        : " << m_strUuid << endl;
        cout << "name        : " << m_strName << endl;       
        cout << "port type   : " << m_nPortType << endl;
        cout << "state       : " << m_nState << endl;
        cout << "broadcast   : " << m_nIsBroadcast << endl;
        cout << "running     : " << m_nIsRunning << endl;
        cout << "multicast   : " << m_nIsMulticast << endl;
        cout << "promis      : " << m_nPromiscuous << endl;       
        cout << "mtu         : " << m_nMtu << endl;
        cout << "master      : " << m_nIsMaster << endl;
        cout << "adminstat   : " << m_nAdminState << endl;        
        cout << "online      : " << m_nIsOnline << endl; 
        cout << "port cfg    : " << m_nIsPortCfg << endl; 
        cout << "consistency : " << m_nIsConsistency << endl;         
    }
private:    
    string m_strVnaUuid;
    int32 m_nPortType;    
    string m_strName;
    string m_strUuid;
    int32 m_nState;
    int32 m_nIsBroadcast;
    int32 m_nIsRunning;
    int32 m_nIsMulticast;
    int32 m_nPromiscuous;
    int32 m_nMtu;
    int32 m_nIsMaster;
    int32 m_nAdminState;
    int32 m_nIsOnline;
    int32 m_nIsPortCfg;
    int32 m_nIsConsistency;    
    
};

class CDBOperatePortBase 
{
public:
    CDBOperatePortBase() {};
    virtual ~CDBOperatePortBase() {};

    int32 QueryPortBaseCallback(CADORecordset * prs,void * nil);
    int32 QueryUuid(const char* vna_uuid, const char* name, int32 port_type, string & port_uuid);
    int32 CheckPortNameIsConflict(const char* vna_uuid, const char* portName,int32 & nResult  );
};

class CVNetDbIProcedure;
class CDBOperatePort : public  CVNetDbIProcedureCallback
{
public:
    explicit CDBOperatePort(CVNetDbIProcedure * pb);
    virtual ~CDBOperatePort();

    int32 QuerySummary(const char* vna_uuid, vector<CDbPortSummary> & outVInfo);  
    int32 Query(CDbPort & Info);
    int32 QuerySummaryByVnaPortName(const char* vna_uuid, const char* name, CDbPortSummary & outVInfo);
    int32 CheckPortNameIsConflict(const char* vna_uuid, const char* portName,int32 & nResult  );
#if 0    
    int32 Add(CDbPort & info);
    int32 Modify(CDbPort & info);
    int32 Del(const char* port_uuid);
#endif 
    int32 DbProcCallback(int32 type, CADORecordset *, void* );

    void DbgShow();    
private:
    int32 QuerySummaryByVnaPortNameCallback(CADORecordset * prs,void * nil);
    int32 QuerySummaryCallback(CADORecordset * prs,void * nil);
    int32 QueryCallback(CADORecordset * prs,void * nil);
    int32 CheckOperateCallback(CADORecordset * prs,void * nil);
//    int32 OperateCallback(CADORecordset * prs,void * nil);
    
private:
    DISALLOW_COPY_AND_ASSIGN(CDBOperatePort);

    CVNetDbIProcedure * m_pIProc;
};
    
} // namespace zte_tecs

#endif // _VNET_DB_PORT_INCLUDE_H_ 


