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

#if !defined(_VNET_DB_QUANTUM_CFG_INCLUDE_H_)
#define _VNET_DB_QUANTUM_CFG_INCLUDE_H_

#include "vnet_db_in.h"
#include "vnet_db_interface.h"
#include "vnet_db_proc.h"


namespace zte_tecs
{

#define VNET_SDN_CTRL_CFG_PROTOCOL_TYPE_UDP (0)
#define VNET_SDN_CTRL_CFG_PROTOCOL_TYPE_TCP (1)


class CDbSdnCtrlCfg 
{
public: 
    CDbSdnCtrlCfg() {m_strUuid = m_strIp= m_nProtocolType=""; m_nPort = 0;};
    virtual ~CDbSdnCtrlCfg() {};
    
    string & GetUuid(){return m_strUuid;};  
    string &  GetProtocolType(){return m_nProtocolType;};   
    int32 GetPort(){return m_nPort;};
    string & GetIp(){return m_strIp;};  
    
    void SetUuid(const char* uuid) {m_strUuid = uuid;};
    void SetProtocolType(const char* type) {m_nProtocolType = type;};     
    void SetPort(int32 mtu) {m_nPort = mtu;}; 
    void SetIP(const char* valuse) {m_strIp = valuse;};         

    void DbgShow()
    {
        cout << "uuid : " << m_strUuid << endl;     
        cout << "ProtocolType : " << m_nProtocolType << endl;
        cout << "Port  : " << m_nPort << endl;
        cout << "Ip    : " << m_strIp << endl;
    }
private:    
    string m_strUuid;
    string m_nProtocolType;
    int32 m_nPort;    
    string m_strIp;
};

class CDbQuantumRestfulCfg 
{
public: 
    CDbQuantumRestfulCfg() {m_strUuid = m_strIp= "";};
    virtual ~CDbQuantumRestfulCfg() {};
    
    string & GetUuid(){return m_strUuid;};  
    string & GetIp(){return m_strIp;};  
    
    void SetUuid(const char* uuid) {m_strUuid = uuid;};
    void SetIP(const char* valuse) {m_strIp = valuse;};         

    void DbgShow()
    {
        cout << "uuid : " << m_strUuid << endl;
        cout << "Ip    : " << m_strIp << endl;
    }
private:    
    string m_strUuid;
    string m_strIp;
};


class CVNetDbIProcedure;
class CDBOperateSdnCfg : public  CVNetDbIProcedureCallback
{
public:
    explicit CDBOperateSdnCfg(CVNetDbIProcedure * pb);
    virtual ~CDBOperateSdnCfg();

    int32 QuerySdnCtrl(CDbSdnCtrlCfg & Info);  
    int32 AddSdnCtrl(CDbSdnCtrlCfg & info);
    int32 ModifySdnCtrl(CDbSdnCtrlCfg & info); 
    int32 DelSdnCtrl(const char* uuid);  

    int32 QueryQuantum(CDbQuantumRestfulCfg & Info); 
    int32 AddQuantum(CDbQuantumRestfulCfg & info); 
    int32 ModifyQuantum(CDbQuantumRestfulCfg & info);  
    int32 DelQuantum(const char* uuid); 

    int32 DbProcCallback(int32 type, CADORecordset *, void* ); 

    void DbgShow();   
private:
    int32 QuerySdnCtrlAll(vector<CDbSdnCtrlCfg> & outVInfo);
    int32 QuerySdnCtrAllCallback(CADORecordset * prs,void * nil);    
    int32 OperateCallback(CADORecordset * prs,void * nil); 
    int32 QueryQuantumRestfullAll(vector<CDbQuantumRestfulCfg> & outVInfo);
    int32 QueryQuantumAllCallback(CADORecordset * prs,void * nil);

    
private:
    DISALLOW_COPY_AND_ASSIGN(CDBOperateSdnCfg);

    CVNetDbIProcedure * m_pIProc;
};

} // namespace zte_tecs

#endif // _VNET_DB_NETPLANE_INCLUDE_H_ 

