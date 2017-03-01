

/******************************************************************************
* Copyright (c) 2013������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�vnet_db_netplane.h
* �ļ���ʶ��
* ����ժҪ��CVNetDb��Ķ����ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�
* ������ڣ�2013��1��28��
*
* �޸ļ�¼1��
*     �޸����ڣ�2013/1/28
*     �� �� �ţ�V1.0
*     �� �� �ˣ�gong.xiefeng
*     �޸����ݣ�����
*
******************************************************************************/

#if !defined(_VNET_DB_LOGICNETWORK_IPRANGE_INCLUDE_H_)
#define _VNET_DB_LOGICNETWORK_IPRANGE_INCLUDE_H_

#include "vnet_db_in.h"
#include "vnet_db_interface.h"
#include "vnet_db_proc.h"

namespace zte_tecs
{


// logicnetwork db
class CDbLogicNetworkIpRange
{
public: 
    CDbLogicNetworkIpRange() 
    {
        m_strUuid = m_strBegin = m_strEnd = m_strMask = "";
        m_oBegin = m_oEnd = m_oMask = 0;
     };
    virtual ~CDbLogicNetworkIpRange() {};
     
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


class CVNetDbIProcedure;
class CDBOperateLogicNetworkIpRange : public  CVNetDbIProcedureCallback
{
public:
    explicit CDBOperateLogicNetworkIpRange(CVNetDbIProcedure * pb);
    virtual ~CDBOperateLogicNetworkIpRange();

    int32 QueryAllocNum(const char* uuid, int64 begin, int64 end, int32 &outTotalNum);
    int32 Query(const char* uuid, vector<CDbLogicNetworkIpRange> & outVInfo);  
    int32 Add(CDbLogicNetworkIpRange & info);
    int32 Del(CDbLogicNetworkIpRange & info);

    int32 CheckAdd(CDbLogicNetworkIpRange & info);

    int32 DbProcCallback(int32 type, CADORecordset *, void* );

    void DbgShow();    
private:
    int32 QueryAllocNumCallback(CADORecordset * prs,void * nil);
    int32 QueryCallback(CADORecordset * prs,void * nil); 
    int32 OperateCallback(CADORecordset * prs,void * nil);
    int32 CheckOperateCallback(CADORecordset * prs,void * nil);
    
private:
    DISALLOW_COPY_AND_ASSIGN(CDBOperateLogicNetworkIpRange);

    CVNetDbIProcedure * m_pIProc;
};
    
} // namespace zte_tecs

#endif // _VNET_DB_LOGICNETWORK_IPRANGE_INCLUDE_H_ 



