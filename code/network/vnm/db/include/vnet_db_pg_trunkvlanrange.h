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

#if !defined(_VNET_DB_PORTGROUP_TRUNKVLANRANGE_INCLUDE_H_)
#define _VNET_DB_PORTGROUP_TRUNKVLANRANGE_INCLUDE_H_

#include "vnet_db_in.h"
#include "vnet_db_interface.h"
#include "vnet_db_proc.h"

#include "vnet_db_portgroup.h"

namespace zte_tecs
{

// port db


class CDbPgTrunkVlanRange 
{
public: 
    CDbPgTrunkVlanRange() 
    {
        m_nVlanBegin = m_nVlanEnd = 0;
        m_strPgUuid  = "";
     };
    virtual ~CDbPgTrunkVlanRange() {};

    string & GetPgUuid(){return m_strPgUuid;}; 
    int32 GetVlanBegin(){return m_nVlanBegin;};              
    int32 GetVlanEnd(){return m_nVlanEnd;};   
    
    void SetPgUuid(const char*  value) {m_strPgUuid = value;};   
    void SetVlanBegin(int32 value) {m_nVlanBegin = value;};   
    void SetVlanEnd(int32 value) {m_nVlanEnd = value;};       

    void DbgShow()
    {  
        cout << "PgUuid    : " << m_strPgUuid << endl;   
        cout << "VlanBegin : " << m_nVlanBegin << endl;   
        cout << "VlanEnd   : " << m_nVlanEnd << endl;         
    }
private:    
    string m_strPgUuid;
    int32 m_nVlanBegin;
    int32 m_nVlanEnd;    
};



class CVNetDbIProcedure;
class CDBOperatePgTrunkVlanRange : public  CVNetDbIProcedureCallback
{
public:
    explicit CDBOperatePgTrunkVlanRange(CVNetDbIProcedure * pb);
    virtual ~CDBOperatePgTrunkVlanRange();

    int32 Query(const char* pg_uuid, vector<CDbPgTrunkVlanRange> & outVInfo);  
    //int32 Query(CDbPgTrunkVlanRange & Info);
    int32 Add(CDbPgTrunkVlanRange & info);
    int32 Modify(CDbPgTrunkVlanRange & info, int32 old_vlan_begin, int32 old_vlan_end);
    int32 Del(CDbPgTrunkVlanRange & info );

    int32 DbProcCallback(int32 type, CADORecordset *, void* );

    void DbgShow();    
private:
    int32 QuerySummaryCallback(CADORecordset * prs,void * nil);
    int32 QueryCallback(CADORecordset * prs,void * nil);
    int32 OperateCallback(CADORecordset * prs,void * nil);
    
private:
    DISALLOW_COPY_AND_ASSIGN(CDBOperatePgTrunkVlanRange);

    CVNetDbIProcedure * m_pIProc;
};
    
} // namespace zte_tecs

#endif // _VNET_DB_PORTGROUP_TRUNKVLANRANGE_INCLUDE_H_ 


