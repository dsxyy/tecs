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

#if !defined(_VNET_DB_PORT_SRIOVVF_INCLUDE_H_)
#define _VNET_DB_PORT_SRIOVVF_INCLUDE_H_

#include "vnet_db_in.h"
#include "vnet_db_interface.h"
#include "vnet_db_proc.h"

#include "vnet_db_port.h"

namespace zte_tecs
{

// port db


class CDbPortSriovVf : public  CDbPort
{
public: 
    CDbPortSriovVf() 
    {
        m_strPhySriovUuid = m_strPci = "";
        m_nVlanNum = 0;
     };
    virtual ~CDbPortSriovVf() {};
    
    string & GetPhySriovUuid(){return m_strPhySriovUuid;};
    string & GetPci(){return m_strPci;};      
    int32 GetVlanNum(){return m_nVlanNum;};            

    void SetPhySriovUuid(const char* value) {m_strPhySriovUuid = value;};   
    void SetPci(const char* value) {m_strPci = value;};   
    void SetVlanNum(int32 value) {m_nVlanNum = value;};   

    void DbgShow()
    {
        DbgPortShow();
        
        cout << "PhySriovUuid : " << m_strPhySriovUuid << endl;  
        cout << "Pci          : " << m_strPci << endl;  
        cout << "VlanNum      : " << m_nVlanNum << endl;   
    }
private:    

    string m_strPhySriovUuid;
    string m_strPci;
    int32 m_nVlanNum;
};


class CDbPortFreeSriovVf
{
public: 
    CDbPortFreeSriovVf() 
    {
        m_strPhySriovUuid = m_strPci = m_strSriovVfUuid= "";
        m_nVlanNum = 0;
     };
    virtual ~CDbPortFreeSriovVf() {};
    
    string & GetPhySriovUuid(){return m_strPhySriovUuid;};
    string & GetSriovVfUuid(){return m_strSriovVfUuid;};
    string & GetPci(){return m_strPci;};      
    int32 GetVlanNum(){return m_nVlanNum;};            

    void SetPhySriovUuid(const char* value) {m_strPhySriovUuid = value;};  
    void SetSriovVfUuid(const char* value) {m_strSriovVfUuid = value;}; 
    void SetPci(const char* value) {m_strPci = value;};   
    void SetVlanNum(int32 value) {m_nVlanNum = value;};   

    void DbgShow()
    {        
        cout << "PhySriovUuid : " << m_strPhySriovUuid << endl;  
        cout << "SriovVfUuid : " << m_strSriovVfUuid << endl;  
        cout << "Pci          : " << m_strPci << endl;  
        cout << "VlanNum      : " << m_nVlanNum << endl;   
    }
private:    

    string m_strPhySriovUuid;
    string m_strSriovVfUuid;
    string m_strPci;
    int32 m_nVlanNum;
};

class CVNetDbIProcedure;
class CDBOperatePortSriovVf : public  CVNetDbIProcedureCallback, public CDBOperatePortBase
{
public:
    explicit CDBOperatePortSriovVf(CVNetDbIProcedure * pb);
    virtual ~CDBOperatePortSriovVf();

    int32 QuerySummary(const char* vna_uuid,const char* sriov_uuid, vector<CDbPortSummary> & outVInfo);  
    int32 Query(CDbPortSriovVf & Info);
    int32 Add(CDbPortSriovVf & info);
    int32 Modify(CDbPortSriovVf & info);
    int32 Del(const char* port_uuid);

    //ͨ������ڣ���ȡpciûʹ�õ�SRIOV VF��Ϣ 
    int32 QueryFree(const char* phy_uuid, vector<CDbPortFreeSriovVf> &outInfo); 

    int32 DbProcCallback(int32 type, CADORecordset *, void* );

    void DbgShow();    
private:
    int32 QuerySummaryCallback(CADORecordset * prs,void * nil);
    int32 QueryFreeCallback(CADORecordset * prs,void * nil);
    int32 QueryCallback(CADORecordset * prs,void * nil);
    int32 OperateCallback(CADORecordset * prs,void * nil);
    
private:
    DISALLOW_COPY_AND_ASSIGN(CDBOperatePortSriovVf);

    CVNetDbIProcedure * m_pIProc;
};
    
} // namespace zte_tecs

#endif // _VNET_DB_PORT_SRIOVVF_INCLUDE_H_ 


