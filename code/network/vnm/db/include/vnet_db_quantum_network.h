

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

#if !defined(_VNET_DB_QUANTUM_NETWORK_INCLUDE_H_)
#define _VNET_DB_QUANTUM_NETWORK_INCLUDE_H_

#include "vnet_db_in.h"
#include "vnet_db_interface.h"
#include "vnet_db_proc.h"

namespace zte_tecs
{


// �ͱ���Լ��һ�� 
#if 0
typedef enum tagDbIpTypeValues
{
    EN_DB_IP_MODE_STATIC = 0,
    EN_DB_IP_MODE_DYNAMIC, 
    EN_DB_IP_MODE_NONE,
}EN_DB_IP_TYPE_VALUES;
#endif 
// logicnetwork db
class CDbQuantumNetwork
{
public: 
    CDbQuantumNetwork() 
    {
        m_strUuid = m_strLogicNetworkUuid = "";
        m_oProjectId = 0;
    };
    virtual ~CDbQuantumNetwork() {};
     
    string & GetUuid(){return m_strUuid;}; 
    string & GetLnUuid(){return m_strLogicNetworkUuid;};
    int64  GetProjectId(){return m_oProjectId;};
     
    void SetUuid(const char* value) {m_strUuid = value;};
    void SetLnUuid(const char* value) {m_strLogicNetworkUuid = value;};
    void SetProjectId(int64 value) {m_oProjectId = value;};    

    void DbgShow()
    {       
        cout << "Uuid  : " << m_strUuid << endl;   
        cout << "LogicNetworkUuid: " << m_strLogicNetworkUuid << endl; 
        cout << "ProjectId: " << m_oProjectId << endl;     
    }
private:    

    string m_strUuid;
    string m_strLogicNetworkUuid;
    int64  m_oProjectId;
};


class CVNetDbIProcedure;
class CDBOperateQuantumNetwork : public  CVNetDbIProcedureCallback
{
public:
    explicit CDBOperateQuantumNetwork(CVNetDbIProcedure * pb);
    virtual ~CDBOperateQuantumNetwork();
    
    int32 QueryAll(vector<CDbQuantumNetwork> & outVInfo); 
    int32 QueryByLn(const char* uuid, vector<CDbQuantumNetwork> & outVInfo);
    
#if 0    
    int32 QueryByPortGroup(const char* uuid, vector<CDbQuantumNetworkPgNetplane> & outVInfo);
#endif    
    int32 Query(CDbQuantumNetwork & Info);
    int32 QueryByLnProjectid(CDbQuantumNetwork & Info);
    int32 Add(CDbQuantumNetwork & info);
    int32 Del(const char* uuid);

    int32 CheckAdd(CDbQuantumNetwork & info);
    int32 CheckDel(const char* uuid);

    int32 DbProcCallback(int32 type, CADORecordset *, void* );

    void DbgShow();   
private:
    int32 QueryAllCallback(CADORecordset * prs,void * nil);   
    int32 QueryCallback(CADORecordset * prs,void * nil);
    int32 OperateCallback(CADORecordset * prs,void * nil);
    int32 CheckOperateCallback(CADORecordset * prs,void * nil);
    
private:
    DISALLOW_COPY_AND_ASSIGN(CDBOperateQuantumNetwork);

    CVNetDbIProcedure * m_pIProc;
};
    
} // namespace zte_tecs

#endif // _VNET_DB_LOGICNETWORK_INCLUDE_H_ 



