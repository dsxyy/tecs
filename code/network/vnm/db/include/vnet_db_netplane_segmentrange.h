

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

#if !defined(_VNET_DB_NETPLANE_SEGMENTRANGE_INCLUDE_H_)
#define _VNET_DB_NETPLANE_SEGMENTRANGE_INCLUDE_H_

#include "vnet_db_in.h"
#include "vnet_db_interface.h"
#include "vnet_db_proc.h"

namespace zte_tecs
{

class CDbNetplaneSegmentRange
{
public: 
    CDbNetplaneSegmentRange() 
    {
        m_strUuid = "";
        m_nBegin = m_nEnd = 0;
     };
    virtual ~CDbNetplaneSegmentRange() {};
     
    string & GetUuid(){return m_strUuid;}; 
    int32  GetBeginNum(){return m_nBegin;};
    int32  GetEndNum(){return m_nEnd;};

    void SetUuid(const char* value) {m_strUuid = value;};
    void SetBeginNum(int32 value) {m_nBegin = value;};
    void SetEndNum (int32 value) {m_nEnd = value;};
    
    void DbgShow()
    {       
        cout << "Uuid    : " << m_strUuid << endl; 
        cout << "Begin  : " << m_nBegin << endl; 
        cout << "End    : " << m_nEnd << endl;   
    }
private:    
    string m_strUuid; // netplane uuid
    int32  m_nBegin;
    int32  m_nEnd;
};


class CVNetDbIProcedure;
class CDBOperateNetplaneSegmentRange : public  CVNetDbIProcedureCallback
{
public:
    explicit CDBOperateNetplaneSegmentRange(CVNetDbIProcedure * pb);  
    virtual ~CDBOperateNetplaneSegmentRange();  

    int32 QueryAllocNum(const char* uuid, int32 begin, int32 end, int32 &outTotalNum);  
    int32 Query(const char* uuid, vector<CDbNetplaneSegmentRange> & outVInfo);    
    int32 Add(CDbNetplaneSegmentRange & info);  
    int32 Del(CDbNetplaneSegmentRange & info);  

    int32 CheckAdd(CDbNetplaneSegmentRange & info);  

    //查询VM share vlans
    int32 QueryVmShareSegments(const char* np_uuid, string & outVlans);  

    int32 DbProcCallback(int32 type, CADORecordset *, void* );  

    void DbgShow();      
private:
    int32 QueryAllocNumCallback(CADORecordset * prs,void * nil);  
    int32 QueryVmShareSegmentsCallback(CADORecordset * prs,void * nil);
    int32 QueryCallback(CADORecordset * prs,void * nil);   
    int32 OperateCallback(CADORecordset * prs,void * nil);  
    int32 CheckOperateCallback(CADORecordset * prs,void * nil);  
    
private:
    DISALLOW_COPY_AND_ASSIGN(CDBOperateNetplaneSegmentRange);

    CVNetDbIProcedure * m_pIProc;
};
    
} // namespace zte_tecs

#endif // _VNET_DB_NETPLANE_VLANRANGE_INCLUDE_H_ 



