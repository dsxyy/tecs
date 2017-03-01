
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

#if !defined(_VNET_DB_VM_MIGRATE_H_)
#define _VNET_DB_VM_MIGRATE_H_

#include "vnet_db_in.h"
#include "vnet_db_interface.h"
#include "vnet_db_proc.h"

namespace zte_tecs
{

class CDbVmMigrate
{
public: 
    CDbVmMigrate() 
    {
        m_oVmid = 0;
        m_strSrcClusterName = m_strSrcHostName = m_strDstClusterName = m_strDstHostName = ""; 
        m_nState = 0;
    };
    virtual ~CDbVmMigrate() {};

    int64 & GetVmId(){return m_oVmid;};
    string & GetSrcClusterName(){return m_strSrcClusterName;};   
    string & GetSrcHostName(){return m_strSrcHostName;};   
    string & GetDstClusterName(){return m_strDstClusterName;};       
    string & GetDstHostName(){return m_strDstHostName;};    
    int32  GetState() {return m_nState;};
    
    void SetVmId(int64 value) {m_oVmid = value;};
    void SetSrcClusterName(const char* value) {m_strSrcClusterName = value;};   
    void SetSrcHostName(const char* value) {m_strSrcHostName = value;};  
    void SetDstClusterName(const char* value) {m_strDstClusterName = value;};  
    void SetDstHostName(const char* value) {m_strDstHostName = value;}; 
    void SetState(int32 value) {m_nState = value;};
    
    void DbgShow()
    {
        cout << "Vmid          : " << m_oVmid << endl;
        cout << "SrcClusterName: " << m_strSrcClusterName << endl;       
        cout << "SrcHostName   : " << m_strSrcHostName << endl; 
        cout << "DstClusterName: " << m_strDstClusterName << endl;         
        cout << "DstHostName   : " << m_strDstHostName << endl;
        cout << "State         : " << m_nState << endl;        
    }
private:    
    int64  m_oVmid;
    string m_strSrcClusterName;
    string m_strSrcHostName;
    string m_strDstClusterName;
    string m_strDstHostName;
    int32 m_nState;
};

class CVNetDbIProcedure;
class CDBOperateVmMigrate : public  CVNetDbIProcedureCallback
{
public:
    explicit CDBOperateVmMigrate(CVNetDbIProcedure * pb);
    virtual ~CDBOperateVmMigrate();

    int32 QuerySummary(vector<CDbVmMigrate> & outVInfo);  
    int32 Query(CDbVmMigrate & Info);
    int32 Add(CDbVmMigrate & info);
    int32 Modify(CDbVmMigrate & info);
    int32 Del(int64 vm_id);

    int32 DbProcCallback(int32 type, CADORecordset *, void* );

    void DbgShow();    
private:
    int32 QuerySummaryCallback(CADORecordset * prs,void * nil);
    int32 QueryCallback(CADORecordset * prs,void * nil);
    int32 OperateCallback(CADORecordset * prs,void * nil);
    
private:
    DISALLOW_COPY_AND_ASSIGN(CDBOperateVmMigrate);

    CVNetDbIProcedure * m_pIProc;
};
    
} // namespace zte_tecs

#endif // _VNET_DB_VM_MIGRATE_H_ 

