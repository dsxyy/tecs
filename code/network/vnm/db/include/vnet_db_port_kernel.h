
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

#if !defined(_VNET_DB_KERNEL_INCLUDE_H_)
#define _VNET_DB_KERNEL_INCLUDE_H_

#include "vnet_db_in.h"
#include "vnet_db_interface.h"
#include "vnet_db_proc.h"

#include "vnet_db_port.h"

namespace zte_tecs
{

// port db
class CDbPortKernel : public  CDbPort
{
public: 
    CDbPortKernel() 
    {
        m_nIsCfg = m_nType = m_nIsCfgPg = m_nIsCfgVswitch = 0;
        m_strVswitchUuid = m_strPgUuid =  "";
        m_nIsDhcp = 0;
     };
    virtual ~CDbPortKernel() {};
    
    int32 GetIsCfgVswitch(){return m_nIsCfgVswitch;};  
    string & GetVswitchUuid(){return m_strVswitchUuid;}; 
    int32 GetIsCfgPg(){return m_nIsCfgPg;};  
    string & GetPgUuid(){return m_strPgUuid;};
    int32 GetType(){return m_nType;};  
    int32 GetIsCfg(){return m_nIsCfg;};
    int32 GetIsDhcp(){return m_nIsDhcp;};
    
    void SetIsCfgVswitch(int32 value) {m_nIsCfgVswitch = value;};   
    void SetVswitchUuid(const char* value) {m_strVswitchUuid = value;};
    void SetIsCfgPg(int32 value) {m_nIsCfgPg = value;};
    void SetPgUuid(const char* value) {m_strPgUuid = value;};
    void SetType(int32 value) {m_nType = value;};
    void SetIsCfg(int32 value) {m_nIsCfg = value;};    
    void SetIsDhcp(int32 value) {m_nIsDhcp = value;};     

    void DbgShow()
    {
        DbgPortShow();        
        cout << "IsCfgVswitch : " << m_nIsCfgVswitch << endl;   
        cout << "VswitchUuid  : " << m_strVswitchUuid << endl; 
        cout << "IsCfgPg      : " << m_nIsCfgPg << endl; 
        cout << "PgUuid       : " << m_strPgUuid << endl; 
        cout << "Type         : " << m_nType << endl; 
        cout << "IsCfg        : " << m_nIsCfg << endl;         
        cout << "IsDhcp       : " << m_nIsDhcp << endl;            
    }
private:    

    int32 m_nIsCfgVswitch;
    string m_strVswitchUuid;
    int32 m_nIsCfgPg;
    string m_strPgUuid;
    int32 m_nType;
    int32 m_nIsCfg;
    int32 m_nIsDhcp;
};



class CVNetDbIProcedure;
class CDBOperatePortKernel : public  CVNetDbIProcedureCallback, public CDBOperatePortBase
{
public:
    explicit CDBOperatePortKernel(CVNetDbIProcedure * pb);
    virtual ~CDBOperatePortKernel();

    int32 QuerySummary(const char* vna_uuid, vector<CDbPortSummary> & outVInfo);  
    int32 Query(CDbPortKernel & Info);
    int32 Add(CDbPortKernel & info);
    int32 Modify(CDbPortKernel & info);
    int32 Del(const char* port_uuid);

    int32 DbProcCallback(int32 type, CADORecordset *, void* );

    void DbgShow();    
private:
    int32 QuerySummaryCallback(CADORecordset * prs,void * nil);
    int32 QueryCallback(CADORecordset * prs,void * nil);
    int32 OperateCallback(CADORecordset * prs,void * nil);
    
private:
    DISALLOW_COPY_AND_ASSIGN(CDBOperatePortKernel);

    CVNetDbIProcedure * m_pIProc;
};
    
} // namespace zte_tecs

#endif // _VNET_DB_KERNEL_INCLUDE_H_ 


