

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

#if !defined(_VNET_DB_NETPLANE_IPRANGE_INCLUDE_H_)
#define _VNET_DB_NETPLANE_IPRANGE_INCLUDE_H_

#include "vnet_db_in.h"
#include "vnet_db_interface.h"
#include "vnet_db_proc.h"

namespace zte_tecs
{


// logicnetwork db
class CDbNetplaneIpRange
{
public: 
    CDbNetplaneIpRange() 
    {
        m_strUuid = m_strBegin = m_strEnd = m_strMask = "";
        m_oBegin = m_oEnd = m_oMask = 0;
     };
    virtual ~CDbNetplaneIpRange() {};
     
    string & GetUuid(){return m_strUuid;}; 
    int64  GetBeginNum(){return m_oBegin;};
    int64  GetEndNum(){return m_oEnd;};
    int64  GetMaskNum(){return m_oMask;};

    string & GetBeginStr(){return m_strBegin;}; 
    string & GetEndStr(){return m_strEnd;}; 
    string & GetMaskStr(){return m_strMask;}; 
     
    void SetUuid(const char* value) {m_strUuid = value;};
    void SetBeginNum(int64 value) {m_oBegin = value;};
    void SetEndNum (int64 value) {m_oEnd = value;};
    void SetMaskNum(int64 value) {m_oMask = value;};
    
    void SetBeginStr(const char* value) {m_strBegin = value;};
    void SetEndStr(const char* value) {m_strEnd = value;};
    void SetMaskStr(const char* value) {m_strMask = value;};

    void DbgShow()
    {       
        cout << "Uuid    : " << m_strUuid << endl; 
        cout << "oBegin  : " << m_oBegin << endl; 
        cout << "oEnd    : " << m_oEnd << endl;   
        cout << "oMask   : " << m_oMask << endl;   
        cout << "strBegin: " << m_strBegin << endl; 
        cout << "strEnd  : " << m_strEnd << endl; 
        cout << "strMask : " << m_strMask << endl;  
    }
private:    
    string m_strUuid;
    int64  m_oBegin;
    int64  m_oEnd;
    int64  m_oMask;  
    string m_strBegin;
    string m_strEnd;
    string m_strMask;
};

class CDbNetplaneIpRangeIsUsing
{
public:    
    CDbNetplaneIpRangeIsUsing() 
    {
        m_nRet = m_nIsUsing = 0;
     };
    virtual ~CDbNetplaneIpRangeIsUsing() {};
    
    int32 GetRet(){return m_nRet;}; 
    int32 GetIsUsing(){return m_nIsUsing;}; 

    void SetRet(int32 value) {m_nRet = value;};   
    void SetIsUsing(int32 value) {m_nIsUsing = value;};   
private:
    int32 m_nRet;
    int32 m_nIsUsing;
  
};

class CVNetDbIProcedure;
class CDBOperateNetplaneIpRange : public  CVNetDbIProcedureCallback
{
public:
    explicit CDBOperateNetplaneIpRange(CVNetDbIProcedure * pb);
    virtual ~CDBOperateNetplaneIpRange();

    // 
    int32 QueryAllocNum(const char* uuid, int64 begin, int64 end, int32 &outTotalNum);    
    int32 QueryIsUsing(const char* npUuid, int64 begin, int64 end, int32 &isUsing);
    int32 Query(const char* uuid, vector<CDbNetplaneIpRange> & outVInfo);  
    int32 Add(CDbNetplaneIpRange & info);
    int32 Del(CDbNetplaneIpRange & info);
    int32 ForceDel(CDbNetplaneIpRange & info);

    int32 CheckAdd(CDbNetplaneIpRange & info);

    int32 DbProcCallback(int32 type, CADORecordset *, void* );

    void DbgShow();    
private:
    int32 QueryIsUsingCallback(CADORecordset * prs,void * nil);
    int32 QueryAllocNumCallback(CADORecordset * prs,void * nil);
    int32 QueryCallback(CADORecordset * prs,void * nil); 
    int32 OperateCallback(CADORecordset * prs,void * nil);
    int32 CheckOperateCallback(CADORecordset * prs,void * nil);
    
private:
    DISALLOW_COPY_AND_ASSIGN(CDBOperateNetplaneIpRange);

    CVNetDbIProcedure * m_pIProc;
};
    
} // namespace zte_tecs

#endif // _VNET_DB_NETPLANE_IPRANGE_INCLUDE_H_ 



