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

#if !defined(_VNET_DB_NETPLANE_INCLUDE_H_)
#define _VNET_DB_NETPLANE_INCLUDE_H_

#include "vnet_db_in.h"
#include "vnet_db_interface.h"
#include "vnet_db_proc.h"


namespace zte_tecs
{

#define MTU_DEFAULT_VALUE (1500)

// Netplane db
class CDbNetplane  //:public CDBObjectBase
{
public: 
    CDbNetplane() {m_strUuid = m_strName = m_strDesc = ""; m_nMtu = MTU_DEFAULT_VALUE; m_nKeyId = 0;};
    virtual ~CDbNetplane() {};
    
    string & GetUuid(){return m_strUuid;};
    string & GetName(){return m_strName;};   
    string & GetDesc(){return m_strDesc;};   
    int32 GetMtu(){return m_nMtu;};
    int32 GetId(){return m_nKeyId;};       
    void SetUuid(const char* uuid) {m_strUuid = uuid;};
    void SetName(const char* name) {m_strName = name;};
    void SetDesc(const char* desc) {m_strDesc = desc;};    
    void SetMtu(int32 mtu) {m_nMtu = mtu;}; 
    void SetId(int32 id) {m_nKeyId = id;};         

    void DbgShow()
    {
        //cout << "ret  : " << m_nRet << endl;
        cout << "uuid : " << m_strUuid << endl;
        cout << "name : " << m_strName << endl;       
        cout << "desc : " << m_strDesc << endl;
        cout << "mtu  : " << m_nMtu << endl;
        cout << "KeyId: " << m_nKeyId << endl;
    }
private:    
    //int32  m_nRet; 
    string m_strUuid;
    string m_strName;
    string m_strDesc;
    int32  m_nMtu;
    int32  m_nKeyId;
};

class CVNetDbIProcedure;
//class CADORecordset;
class CDBOperateNetplane : public  CVNetDbIProcedureCallback
{
public:
    explicit CDBOperateNetplane(CVNetDbIProcedure * pb);
    virtual ~CDBOperateNetplane();

    int32 QueryNetplaneSummary(vector<CDBObjectBase> & outVInfo);  
    int32 QueryNetplane(CDbNetplane & Info);
    int32 AddNetplane(CDbNetplane & info);
    int32 ModifyNetplane(CDbNetplane & info);
    int32 DelNetplane(const char* netplane_uuid);

    int32 DbProcCallback(int32 type, CADORecordset *, void* );

    void DbgShow();    
private:
    int32 QueryNetplaneSummaryCallback(CADORecordset * prs,void * nil);
    int32 QueryNetplaneCallback(CADORecordset * prs,void * nil);
    int32 OperateNetplaneCallback(CADORecordset * prs,void * nil);
    
private:
    DISALLOW_COPY_AND_ASSIGN(CDBOperateNetplane);

    CVNetDbIProcedure * m_pIProc;
};

} // namespace zte_tecs

#endif // _VNET_DB_NETPLANE_INCLUDE_H_ 

