
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

#if !defined(_VNET_DB_PORT_VTEP_INCLUDE_H_)
#define _VNET_DB_PORT_VTEP_INCLUDE_H_

#include "vnet_db_in.h"
#include "vnet_db_interface.h"
#include "vnet_db_proc.h"

#include "vnet_db_port.h"

namespace zte_tecs
{

// port db
class CDbPortVtep : public  CDbPort
{
public: 
    CDbPortVtep() 
    {
        m_strVswitchUuid = m_strIp =  "";
    };
    virtual ~CDbPortVtep() {};
    
    string & GetVswitchUuid(){return m_strVswitchUuid;};
    string & GetIp(){return m_strIp;};
    string & GetMask(){return m_strMask;};
    int64  GetIpNum(){return m_oIP;};
    int64  GetMaskNum(){return m_oMask;};
     
    void SetVswitchUuid(const char* value) {m_strVswitchUuid = value;};
    void SetIp(const char* value) {m_strIp = value;};
    void SetMask(const char* value) {m_strMask = value;};  
    void SetIpNum(int64 value) {m_oIP = value;};
    void SetMaskNum(int64 value) {m_oMask = value;};
    
    void DbgShow()
    {
        DbgPortShow();        
        cout << "VswitchUuid  : " << m_strVswitchUuid << endl; 
        cout << "Ip           : " << m_strIp << endl;  
        cout << "Mask         : " << m_strMask << endl;  
        cout << "IpNum        : " << m_oIP << endl;  
        cout << "IpMask       : " << m_oMask << endl;  
    }
private:    

    string m_strVswitchUuid;
    string m_strIp;
    string m_strMask;
    int64  m_oIP;
    int64  m_oMask;
};


class CVNetDbIProcedure;
class CDBOperatePortVtep : public  CVNetDbIProcedureCallback, public CDBOperatePortBase
{
public:
    explicit CDBOperatePortVtep(CVNetDbIProcedure * pb);
    virtual ~CDBOperatePortVtep();

    int32 QuerySummary(const char* vna_uuid, vector<CDbPortSummary> & outVInfo);
    int32 Query(CDbPortVtep & Info);
    int32 Add(CDbPortVtep & info);
    int32 Modify(CDbPortVtep & info); 
    int32 Del(const char* port_uuid);

    int32 DbProcCallback(int32 type, CADORecordset *, void* );

    void DbgShow();   
private:
    int32 QuerySummaryCallback(CADORecordset * prs,void * nil);
    int32 QueryCallback(CADORecordset * prs,void * nil);
    int32 OperateCallback(CADORecordset * prs,void * nil);
    
private:
    DISALLOW_COPY_AND_ASSIGN(CDBOperatePortVtep);

    CVNetDbIProcedure * m_pIProc;
};
    
} // namespace zte_tecs

#endif // _VNET_DB_KERNEL_INCLUDE_H_ 


