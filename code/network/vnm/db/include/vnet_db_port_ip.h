
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

#if !defined(_VNET_DB_PORT_IP_INCLUDE_H_)
#define _VNET_DB_PORT_IP_INCLUDE_H_

#include "vnet_db_in.h"
#include "vnet_db_interface.h"
#include "vnet_db_proc.h"

namespace zte_tecs
{

class CDbPortIp
{
public: 
    CDbPortIp() 
    {
        m_strVnaUuid = m_strPortUuid = m_strPortName = m_strPortIpName = m_strIp=  m_strMask = ""; 
        m_nPortType = m_nIsCfg = m_nIsOnline = 0;
    };
    virtual ~CDbPortIp() {};

    string & GetVnaUuid(){return m_strVnaUuid;};
    string & GetPortUuid(){return m_strPortUuid;};   
    string & GetPortName(){return m_strPortName;};   
    int32   GetPortType(){return m_nPortType;};       
    string & GetPortIpName(){return m_strPortIpName;};    
    string & GetIp(){return m_strIp;};  
    string & GetMask(){return m_strMask;};  
    int32  GetIsCfg(){return m_nIsCfg;};  
    int32  GetIsOnline(){return m_nIsOnline;};  
    
    
    void SetVnaUuid(const char* value) {m_strVnaUuid = value;};
    void SetPortUuid(const char* value) {m_strPortUuid = value;};   
    void SetPortName(const char* value) {m_strPortName = value;};  
    void SetPortType(int32 value) {m_nPortType = value;};  
    void SetPortIpName(const char* value) {m_strPortIpName = value;}; 
    void SetIp(const char* value) {m_strIp = value;}; 
    void SetMask(const char* value) {m_strMask = value;}; 
    void SetIsCfg(int32 value) {m_nIsCfg = value;}; 
    void SetIsOnline(int32 value) {m_nIsOnline = value;}; 
    
    void DbgShow()
    {
        cout << "uuid      : " << m_strVnaUuid << endl;
        cout << "PortUuid  : " << m_strPortUuid << endl;       
        cout << "PortName  : " << m_strPortName << endl; 
        cout << "PortType  : " << m_nPortType << endl;         
        cout << "PortIpName: " << m_strPortIpName << endl;     
        cout << "Ip        : " << m_strIp << endl;    
        cout << "Mask      : " << m_strMask << endl;    
        cout << "IsCfg     : " << m_nIsCfg << endl;    
        cout << "IsOnline  : " << m_nIsOnline << endl; 
    }
private:    
    string m_strVnaUuid;
    string m_strPortUuid;
    string m_strPortName;
    int32  m_nPortType;    
    string m_strPortIpName;  
    string m_strIp;
    string m_strMask;
    int32  m_nIsCfg;
    int32  m_nIsOnline;
};

class CVNetDbIProcedure;
class CDBOperatePortIp : public  CVNetDbIProcedureCallback
{
public:
    explicit CDBOperatePortIp(CVNetDbIProcedure * pb);
    virtual ~CDBOperatePortIp();

    int32 QueryPortIp(const char* port_uuid, vector<CDbPortIp> & outVInfo);  
    int32 QueryPortIpByVna(const char* vna_uuid, vector<CDbPortIp> & outVInfo); 

    int32 Add(const char* port_uuid,const char* port_ip_name,const char* ip, const char* mask, int32 iscfg, int32 isOnline);
    int32 Modify(const char* port_uuid,const char* port_ip_name,const char* ip, const char* mask, int32 iscfg, int32 isOnline);
    int32 Del(const char* port_uuid,const char* port_ip_name);
    int32 DelByPort(const char* port_uuid);

    int32 DbProcCallback(int32 type, CADORecordset *, void* );

    void DbgShow();    
private:
    int32 QueryCallback(CADORecordset * prs,void * nil);
    int32 OperateCallback(CADORecordset * prs,void * nil);
    
private:
    DISALLOW_COPY_AND_ASSIGN(CDBOperatePortIp);

    CVNetDbIProcedure * m_pIProc;
};
    
} // namespace zte_tecs

#endif // _VNET_DB_PORT_IP_INCLUDE_H_ 

