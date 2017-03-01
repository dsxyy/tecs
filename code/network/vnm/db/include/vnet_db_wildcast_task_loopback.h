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

#if !defined(_VNET_DB_WILDCAST_TASK_LOOPBACK_INCLUDE_H_)
#define _VNET_DB_WILDCAST_TASK_LOOPBACK_INCLUDE_H_

#include "vnet_db_in.h"
#include "vnet_db_interface.h"
#include "vnet_db_proc.h"

namespace zte_tecs
{


// vswitch db
class CDbWildcastTaskLoopback
{
public: 
    CDbWildcastTaskLoopback() 
    {
        m_nState = m_nSendNum = m_nVnaIsOnline= 0;
        m_strUuid = m_strVnaUuid = m_strLoopbackUuid = m_strPortName = "";
        m_nIsLoop = 0;
    };
    virtual ~CDbWildcastTaskLoopback() {};
      
    string& GetUuid(){return m_strUuid;}; 
    string& GetVnaUuid(){return m_strVnaUuid;};
    int32 GetVnaIsOnline(){return m_nVnaIsOnline;};  
    string& GetLoopbackUuid(){return m_strLoopbackUuid;}; 
    int32 GetState(){return m_nState;};  
    int32 GetSendNum(){return m_nSendNum;};  
    
    string& GetPortName(){return m_strPortName;};  
    int32 GetIsLoop(){return m_nIsLoop;};  
          
    void SetUuid(const char* value) {m_strUuid = value;};
    void SetVnaUuid(const char* value) {m_strVnaUuid = value;};
    void SetVnaIsOnline(int32 value) {m_nVnaIsOnline = value;};  
    void SetState(int32 value) {m_nState = value;};   
    void SetSendNum(int32 value) {m_nSendNum = value;};  

    void SetLoopbackUuid(const char* value) {m_strLoopbackUuid = value;};
    
    void SetPortName(const char*  value) {m_strPortName = value;}; 
    void SetIsLoop(int32 value) {m_nIsLoop = value;}; 

    void DbgShow()
    {
        cout << "Uuid         : " << m_strUuid << endl;  
        cout << "State        : " << m_nState << endl; 
        cout << "SendNum      : " << m_nSendNum << endl; 
        cout << "VnaUuid      : " << m_strVnaUuid << endl; 
        cout << "VnaIsOnline  : " << m_nVnaIsOnline << endl;
        cout << "LoopBackUuid : " << m_strLoopbackUuid << endl;
        cout << "PortName     : " << m_strPortName << endl;
        cout << "IsLoop       : " << m_nIsLoop << endl;
    }
private:    

    string m_strUuid;
    string m_strVnaUuid;
    int32  m_nVnaIsOnline;
    string m_strLoopbackUuid;    
    int32 m_nState;
    int32 m_nSendNum;    

    string m_strPortName;
    int32 m_nIsLoop;    
};



class CVNetDbIProcedure;
class CDBOperateWildcastTaskLoopback : public  CVNetDbIProcedureCallback
{
public:
    explicit CDBOperateWildcastTaskLoopback(CVNetDbIProcedure * pb);
    virtual ~CDBOperateWildcastTaskLoopback();

    int32 QuerySummaryByVna(const char* uuid, vector<CDbWildcastTaskLoopback> & outVInfo);
    int32 QuerySummary( vector<CDbWildcastTaskLoopback> & outVInfo);  
    int32 Query(CDbWildcastTaskLoopback & Info);
    
    int32 CheckAdd(CDbWildcastTaskLoopback & info);
    int32 Add(CDbWildcastTaskLoopback & info);
    int32 CheckModify(CDbWildcastTaskLoopback & info);
    int32 Modify(CDbWildcastTaskLoopback & info);
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
    DISALLOW_COPY_AND_ASSIGN(CDBOperateWildcastTaskLoopback);

    CVNetDbIProcedure * m_pIProc;
};
    
} // namespace zte_tecs

#endif // _VNET_DB_WILDCAST_TASK_CREATE_VPORT_INCLUDE_H_ 


