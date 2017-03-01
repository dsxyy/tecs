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

#if !defined(_VNET_DB_QUANTUM_PORT_INCLUDE_H_)
#define _VNET_DB_QUANTUM_PORT_INCLUDE_H_

#include "vnet_db_in.h"
#include "vnet_db_interface.h"
#include "vnet_db_proc.h"


namespace zte_tecs
{


// Netplane db
class CDbQuantumPort  //:public CDBObjectBase
{
public: 
    CDbQuantumPort() {m_strUuid = m_strQuantumNetworkUuid = ""; };
    virtual ~CDbQuantumPort() {};
    
    string & GetUuid(){return m_strUuid;};
    string & GetQNUuid(){return m_strQuantumNetworkUuid;};
    void SetUuid(const char* uuid) {m_strUuid = uuid;};
    void SetQNUuid(const char* name) {m_strQuantumNetworkUuid = name;};

    void DbgShow()
    {
        cout << "QuantumPortUuid : " << m_strUuid << endl;
        cout << "QuantumNetworkUuid : " << m_strQuantumNetworkUuid << endl;  
    }
private:    
    //int32  m_nRet; 
    string m_strUuid;
    string m_strQuantumNetworkUuid;
};

class CVNetDbIProcedure;
//class CADORecordset;
class CDBOperateQuantumPort : public  CVNetDbIProcedureCallback
{
public:
    explicit CDBOperateQuantumPort(CVNetDbIProcedure * pb);
    virtual ~CDBOperateQuantumPort();

    int32 QueryAll(vector<CDbQuantumPort> & outVInfo); 
    int32 Query(CDbQuantumPort & Info);
    int32 CheckAdd(CDbQuantumPort & info); 
    int32 Add(CDbQuantumPort & info); 
    int32 CheckDel(const char* netplane_uuid);
    int32 Del(const char* netplane_uuid); 

    int32 DbProcCallback(int32 type, CADORecordset *, void* ); 

    void DbgShow();     
private:
    int32 QueryAllCallback(CADORecordset * prs,void * nil); 
    int32 QueryCallback(CADORecordset * prs,void * nil); 
    int32 OperateCallback(CADORecordset * prs,void * nil);  
    int32 CheckOperateCallback(CADORecordset * prs,void * nil);
    
private:
    DISALLOW_COPY_AND_ASSIGN(CDBOperateQuantumPort);

    CVNetDbIProcedure * m_pIProc;
};

} // namespace zte_tecs

#endif // _VNET_DB_NETPLANE_INCLUDE_H_ 

