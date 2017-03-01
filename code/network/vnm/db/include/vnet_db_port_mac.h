
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

#if !defined(_VNET_DB_PORT_MAC_INCLUDE_H_)
#define _VNET_DB_PORT_MAC_INCLUDE_H_

#include "vnet_db_in.h"
#include "vnet_db_interface.h"
#include "vnet_db_proc.h"

namespace zte_tecs
{

class CDbPortMac
{
public: 
    CDbPortMac() 
    {
        m_strVnaUuid = m_strPortUuid = m_strPortName = m_strMac = ""; 
        m_nPortType = 0;
    };
    virtual ~CDbPortMac() {};

    string & GetVnaUuid(){return m_strVnaUuid;};
    string & GetPortUuid(){return m_strPortUuid;};   
    string & GetPortName(){return m_strPortName;};   
    int32   GetPortType(){return m_nPortType;};       
    string & GetMac(){return m_strMac;};    
    
    void SetVnaUuid(const char* value) {m_strVnaUuid = value;};
    void SetPortUuid(const char* value) {m_strPortUuid = value;};   
    void SetPortName(const char* value) {m_strPortName = value;};  
    void SetPortType(int32 value) {m_nPortType = value;};  
    void SetMac(const char* value) {m_strMac = value;}; 
    
    void DbgShow()
    {
        cout << "uuid     : " << m_strVnaUuid << endl;
        cout << "PortUuid : " << m_strPortUuid << endl;       
        cout << "PortName : " << m_strPortName << endl; 
        cout << "PortType : " << m_nPortType << endl;         
        cout << "Mac      : " << m_strMac << endl;     
    }
private:    
    string m_strVnaUuid;
    string m_strPortUuid;
    string m_strPortName;
    int32  m_nPortType;    
    string m_strMac;    
};

class CVNetDbIProcedure;
class CDBOperatePortMac : public  CVNetDbIProcedureCallback
{
public:
    explicit CDBOperatePortMac(CVNetDbIProcedure * pb);
    virtual ~CDBOperatePortMac();

    int32 QueryPortMac(const char* port_uuid, vector<CDbPortMac> & outVInfo);  
    int32 QueryPortMacByVna(const char* vna_uuid, vector<CDbPortMac> & outVInfo); 
    int32 Add(const char* port_uuid,const char* mac);
    int32 Del(const char* port_uuid,const char* mac);
    int32 DelByPort(const char* port_uuid);

    int32 DbProcCallback(int32 type, CADORecordset *, void* );

    void DbgShow();    
private:
    int32 QueryCallback(CADORecordset * prs,void * nil);
    int32 OperateCallback(CADORecordset * prs,void * nil);
    
private:
    DISALLOW_COPY_AND_ASSIGN(CDBOperatePortMac);

    CVNetDbIProcedure * m_pIProc;
};
    
} // namespace zte_tecs

#endif // _VNET_DB_PORT_MAC_INCLUDE_H_ 

