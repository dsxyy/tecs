

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

#if !defined(_VNET_DB_NETPLANE_MACRANGE_INCLUDE_H_)
#define _VNET_DB_NETPLANE_MACRANGE_INCLUDE_H_

#include "vnet_db_in.h"
#include "vnet_db_interface.h"
#include "vnet_db_proc.h"

namespace zte_tecs
{


// logicnetwork db
class CDbNetplaneMacRange
{
public: 
    CDbNetplaneMacRange() 
    {
        m_strUuid = m_strBegin = m_strEnd = "";
        m_oBegin = m_oEnd = 0;
     };
    virtual ~CDbNetplaneMacRange() {};
     
    string & GetUuid(){return m_strUuid;}; 
    int64  GetBeginNum(){return m_oBegin;};
    int64  GetEndNum(){return m_oEnd;};

    string & GetBeginStr(){return m_strBegin;}; 
    string & GetEndStr(){return m_strEnd;}; 
     
    void SetUuid(const char* value) {m_strUuid = value;};
    void SetBeginNum(int64 value) {m_oBegin = value;};
    void SetEndNum (int64 value) {m_oEnd = value;};
    
    void SetBeginStr(const char* value) {m_strBegin = value;};
    void SetEndStr(const char* value) {m_strEnd = value;};

    void DbgShow()
    {       
        cout << "Uuid    : " << m_strUuid << endl; 
        cout << "oBegin  : " << m_oBegin << endl; 
        cout << "oEnd    : " << m_oEnd << endl;   
        cout << "strBegin: " << m_strBegin << endl; 
        cout << "strEnd  : " << m_strEnd << endl; 
    }
private:    
    string m_strUuid;
    int64  m_oBegin;
    int64  m_oEnd;  
    string m_strBegin;
    string m_strEnd;
};


class CVNetDbIProcedure;
class CDBOperateNetplaneMacRange : public  CVNetDbIProcedureCallback
{
public:
    explicit CDBOperateNetplaneMacRange(CVNetDbIProcedure * pb);
    virtual ~CDBOperateNetplaneMacRange();

    int32 QueryAllocNum(const char* uuid, int64 begin, int64 end, int32 &outTotalNum);
    int32 Query(const char* uuid, vector<CDbNetplaneMacRange> & outVInfo);  
    int32 Add(CDbNetplaneMacRange & info);
    int32 Del(CDbNetplaneMacRange & info);

    int32 CheckAdd(CDbNetplaneMacRange & info);

    int32 DbProcCallback(int32 type, CADORecordset *, void* );

    void DbgShow();    
private:
    int32 QueryAllocNumCallback(CADORecordset * prs,void * nil);
    int32 QueryCallback(CADORecordset * prs,void * nil); 
    int32 OperateCallback(CADORecordset * prs,void * nil);
    int32 CheckOperateCallback(CADORecordset * prs,void * nil);
    
private:
    DISALLOW_COPY_AND_ASSIGN(CDBOperateNetplaneMacRange);

    CVNetDbIProcedure * m_pIProc;
};
    
} // namespace zte_tecs

#endif // _VNET_DB_NETPLANE_MACRANGE_INCLUDE_H_ 



