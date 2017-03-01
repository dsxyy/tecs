

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

#if !defined(_VNET_DB_LOGICNETWORK_IPPOOL_INCLUDE_H_)
#define _VNET_DB_LOGICNETWORK_IPPOOL_INCLUDE_H_

#include "vnet_db_in.h"
#include "vnet_db_interface.h"
#include "vnet_db_proc.h"

namespace zte_tecs
{

// logicnetwork db
class CDbLogicNetworkIpPool
{
public: 
    CDbLogicNetworkIpPool() 
    {
        m_strUuid = m_strIp = m_strMask = "";
        m_oIp = m_oMask = 0;
     };
    virtual ~CDbLogicNetworkIpPool() {};
     
    string & GetUuid(){return m_strUuid;}; 
    int64  GetIpNum(){return m_oIp;};
    int64  GetMaskNum(){return m_oMask;};

    string & GetIpStr(){return m_strIp;}; 
    string & GetMaskStr(){return m_strMask;}; 
     
    void SetUuid(const char* value) {m_strUuid = value;};
    void SetIpNum(int64 value) {m_oIp = value;};
    void SetMaskNum(int64 value) {m_oMask = value;};
    
    void SetIpStr(const char* value) {m_strIp = value;};
    void SetMaskStr(const char* value) {m_strMask = value;};

    void DbgShow()
    {       
        cout << "Uuid    : " << m_strUuid << endl;  
        cout << "oIp  : " << m_oIp << endl; 
        cout << "oMask   : " << m_oMask << endl;   
        cout << "strIp: " << m_strIp << endl; 
        cout << "strMask : " << m_strMask << endl;  
    }
private:    
    string m_strUuid;
    int64  m_oIp;
    int64  m_oMask;  
    string m_strIp;
    string m_strMask;
};


class CVNetDbIProcedure;
class CDBOperateLogicNetworkIpPool : public  CVNetDbIProcedureCallback
{
public:
    explicit CDBOperateLogicNetworkIpPool(CVNetDbIProcedure * pb);
    virtual ~CDBOperateLogicNetworkIpPool();

    int32 Query(const char* uuid, vector<CDbLogicNetworkIpPool> & outVInfo);  
    int32 QueryByLnRange(const char* uuid, int64 begin, int64 end, vector<CDbLogicNetworkIpPool> & outVInfo); 
    int32 Add(CDbLogicNetworkIpPool & info);
    int32 Del(CDbLogicNetworkIpPool & info);

    int32 CheckAdd(CDbLogicNetworkIpPool & info);
    int32 CheckDel(CDbLogicNetworkIpPool & info);    

    int32 DbProcCallback(int32 type, CADORecordset *, void* );

    void DbgShow();    
private:
    int32 QueryCallback(CADORecordset * prs,void * nil); 
    int32 OperateCallback(CADORecordset * prs,void * nil);
    int32 CheckOperateCallback(CADORecordset * prs,void * nil);
    
private:
    DISALLOW_COPY_AND_ASSIGN(CDBOperateLogicNetworkIpPool);

    CVNetDbIProcedure * m_pIProc;
};
    
} // namespace zte_tecs

#endif // _VNET_DB_LOGICNETWORK_IPPOOL_INCLUDE_H_ 



