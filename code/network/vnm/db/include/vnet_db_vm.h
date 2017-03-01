
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

#if !defined(_VNET_DB_VM_H_)
#define _VNET_DB_VM_H_

#include "vnet_db_in.h"
#include "vnet_db_interface.h"
#include "vnet_db_proc.h"

namespace zte_tecs
{

class CDbVm
{
public: 
    CDbVm() 
    {
        m_oVmid = m_oProjectid = 0;
        m_strClusterName = m_strHostName = ""; 
        m_nIsEnableWdg = m_nWdgTimeout= m_nIsHasCluster = m_nIsHasHost =0;
    };
    virtual ~CDbVm() {};

    int64 & GetVmId(){return m_oVmid;};
    int64 & GetProjectId(){return m_oProjectid;};
    int32 GetIsEnableWdg(){return m_nIsEnableWdg;};   
    int32 GetWdgTimeout(){return m_nWdgTimeout;};   
    int32 GetIsHasCluster(){return m_nIsHasCluster;};       
    string & GetClusterName(){return m_strClusterName;};    
    int32  GetIsHasHost() {return m_nIsHasHost;};
    string &  GetHostName() {return m_strHostName;};
    
    void SetVmId(int64 value) {m_oVmid = value;};
    void SetProjectId(int64 value) {m_oProjectid = value;};
    void SetIsEnableWdg(int32 value) {m_nIsEnableWdg = value;};   
    void SetWdgTimeout(int32 value) {m_nWdgTimeout = value;};  
    void SetIsHasCluster(int32 value) {m_nIsHasCluster = value;};  
    void SetClusterName(const char* value) {m_strClusterName = value;}; 
    void SetIsHasHost(int32 value) {m_nIsHasHost = value;};
    void SetHostName(const char* value) {m_strHostName = value;};
    
    void DbgShow()
    {
        cout << "Vmid          : " << m_oVmid << endl;
        cout << "ProjectId     : " << m_oProjectid << endl;
        cout << "IsEnableWdg   : " << m_nIsEnableWdg << endl;       
        cout << "WdgTimeout    : " << m_nWdgTimeout << endl; 
        cout << "IsHasCluster  : " << m_nIsHasCluster << endl;         
        cout << "ClusterName   : " << m_strClusterName << endl;
        cout << "IsHasHost     : " << m_nIsHasHost << endl;      
        cout << "HostName      : " << m_strHostName << endl; 
    }
private:    
    int64  m_oVmid;
    int64  m_oProjectid;
    int32  m_nIsEnableWdg;
    int32  m_nWdgTimeout;
    int32  m_nIsHasCluster;
    string m_strClusterName;
    int32  m_nIsHasHost;
    string m_strHostName;
};

class CVNetDbIProcedure;
class CDBOperateVm : public  CVNetDbIProcedureCallback
{
public:
    explicit CDBOperateVm(CVNetDbIProcedure * pb);
    virtual ~CDBOperateVm();

    int32 QuerySummary(vector<CDbVm> & outVInfo);  
    int32 Query(CDbVm & Info);
    int32 Add(CDbVm & info);
    int32 Modify(CDbVm & info);
    int32 Del(int64 vm_id);

    int32 DbProcCallback(int32 type, CADORecordset *, void* );

    void DbgShow();    
private:
    int32 QuerySummaryCallback(CADORecordset * prs,void * nil);
    int32 QueryCallback(CADORecordset * prs,void * nil);
    int32 OperateCallback(CADORecordset * prs,void * nil);
    
private:
    DISALLOW_COPY_AND_ASSIGN(CDBOperateVm);

    CVNetDbIProcedure * m_pIProc;
};
    
} // namespace zte_tecs

#endif // _VNET_DB_VM_H_ 

