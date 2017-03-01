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

#if !defined(_VNET_DB_WILDCAST_LOOPBACK_PORT_INCLUDE_H_)
#define _VNET_DB_WILDCAST_LOOPBACK_PORT_INCLUDE_H_

#include "vnet_db_in.h"
#include "vnet_db_interface.h"
#include "vnet_db_proc.h"

namespace zte_tecs
{


// vswitch db
class CDbWildcastLoopbackPort
{
public: 
    CDbWildcastLoopbackPort() 
    {
        m_nIsLoop = 0;
        m_strUuid = m_strPortName = "";
     };
    virtual ~CDbWildcastLoopbackPort() {};
      
    string & GetUuid(){return m_strUuid;}; 
    string& GetPortName(){return m_strPortName;}; 
    int32 GetIsLoop(){return m_nIsLoop;};  
      
    void SetUuid(const char* value) {m_strUuid = value;};
    void SetPortName(const char* value) {m_strPortName = value;};
    void SetIsLoop (int32 value) {m_nIsLoop = value;};   

    void DbgShow()
    {
        cout << "Uuid      : " << m_strUuid << endl; 
        cout << "PortName  : " << m_strPortName << endl; 
        cout << "IsLoop    : " << m_nIsLoop << endl; 
    }
private:    

    string m_strUuid;
    string m_strPortName;
    int32 m_nIsLoop;
};


class CVNetDbIProcedure;
class CDBOperateWildcastLoopbackPort : public  CVNetDbIProcedureCallback
{
public:
    explicit CDBOperateWildcastLoopbackPort(CVNetDbIProcedure * pb);
    virtual ~CDBOperateWildcastLoopbackPort();

    int32 QuerySummary( vector<CDbWildcastLoopbackPort> & outVInfo);  
    int32 Query(CDbWildcastLoopbackPort & Info);
    int32 CheckAdd(CDbWildcastLoopbackPort & info);
    int32 Add(CDbWildcastLoopbackPort & info);
    int32 CheckModify(CDbWildcastLoopbackPort & info);
    int32 Modify(CDbWildcastLoopbackPort & info);
    int32 CheckDel(const char* uuid);
    int32 Del(const char* uuid);

    int32 DbProcCallback(int32 type, CADORecordset *, void* );

    void DbgShow();    
private:
    int32 QuerySummaryCallback(CADORecordset * prs,void * nil);
    int32 QueryCallback(CADORecordset * prs,void * nil);
    int32 OperateCallback(CADORecordset * prs,void * nil);
    int32 CheckOperateCallback(CADORecordset * prs,void * nil);    
    
private:
    DISALLOW_COPY_AND_ASSIGN(CDBOperateWildcastLoopbackPort);

    CVNetDbIProcedure * m_pIProc;
};
    
} // namespace zte_tecs

#endif // _VNET_DB_WILDCAST_LOOPBACK_PORT_INCLUDE_H_ 


