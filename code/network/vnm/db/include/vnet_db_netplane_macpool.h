

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

#if !defined(_VNET_DB_NETPLANE_MACPOOL_INCLUDE_H_)
#define _VNET_DB_NETPLANE_MACPOOL_INCLUDE_H_

#include "vnet_db_in.h"
#include "vnet_db_interface.h"
#include "vnet_db_proc.h"

namespace zte_tecs
{

// logicnetwork db
class CDbNetplaneMacPool
{
public: 
    CDbNetplaneMacPool() 
    {
        m_strUuid = m_strMac = "";
        m_oMac = 0;
     };
    virtual ~CDbNetplaneMacPool() {};
     
    string & GetUuid(){return m_strUuid;}; 
    int64  GetMacNum(){return m_oMac;};

    string & GetMacStr(){return m_strMac;}; 
     
    void SetUuid(const char* value) {m_strUuid = value;};
    void SetMacNum(int64 value) {m_oMac = value;};
    
    void SetMacStr(const char* value) {m_strMac = value;};;

    void DbgShow()
    {       
        cout << "Uuid    : " << m_strUuid << endl;  
        cout << "oMac  : " << m_oMac << endl;    
        cout << "strMac: " << m_strMac << endl;  
    }
private:    
    string m_strUuid;
    int64  m_oMac;
    string m_strMac;
};


class CVNetDbIProcedure;
class CDBOperateNetplaneMacPool : public  CVNetDbIProcedureCallback
{
public:
    explicit CDBOperateNetplaneMacPool(CVNetDbIProcedure * pb);
    virtual ~CDBOperateNetplaneMacPool();

    int32 Query(const char* uuid, vector<CDbNetplaneMacPool> & outVInfo);  
    int32 QueryByNpRange(const char* np_uuid, int64 mac_begin, int64 mac_end, vector<CDbNetplaneMacPool> & outVInfo); 
    int32 Add(CDbNetplaneMacPool & info);
    int32 Del(CDbNetplaneMacPool & info);
    
    int32 CheckAdd(CDbNetplaneMacPool & info);
    int32 CheckDel(CDbNetplaneMacPool & info);

    int32 DbProcCallback(int32 type, CADORecordset *, void* );

    void DbgShow();    
private:
    int32 QueryCallback(CADORecordset * prs,void * nil); 
    int32 OperateCallback(CADORecordset * prs,void * nil);
    int32 CheckOperateCallback(CADORecordset * prs,void * nil);
    
private:
    DISALLOW_COPY_AND_ASSIGN(CDBOperateNetplaneMacPool);

    CVNetDbIProcedure * m_pIProc;
};
    
} // namespace zte_tecs

#endif // _VNET_DB_NETPLANE_MACPOOL_INCLUDE_H_ 



