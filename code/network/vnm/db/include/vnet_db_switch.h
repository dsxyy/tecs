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

#if !defined(_VNET_DB_SWITCH_INCLUDE_H_)
#define _VNET_DB_SWITCH_INCLUDE_H_

#include "vnet_db_in.h"
#include "vnet_db_interface.h"
#include "vnet_db_proc.h"


namespace zte_tecs
{

// �ͱ���Լ��һ�� 
typedef enum tagDbSwitchTypeValues
{
    EN_DB_SWITCH_TYPE_PHYSICAL = 0,
    EN_DB_SWITCH_TYPE_SDVS,     
    EN_DB_SWITCH_TYPE_EDVS,
}EN_DB_SWITCH_TYPE_VALUES;

#define DB_SWITCH_TABLE_FIELD_NUM (6)

class CDbSwitchSummary
{
public: 
    CDbSwitchSummary() 
    {
        m_nSwitchType = 0;
        m_strName = m_strUuid =  "";
    };
    virtual ~CDbSwitchSummary() {};
    
    string & GetName(){return m_strName;}; 
    string & GetUuid(){return m_strUuid;}; 
    int32 GetSwitchType(){return m_nSwitchType;};   

    void SetName(const char* value) {m_strName = value;};
    void SetUuid(const char* value) {m_strUuid = value;};
    void SetSwitchType(int32 value) {m_nSwitchType = value;}; 

    void DbgShow()
    {
        cout << "Name       : " << m_strName << endl;   
        cout << "Uuid       : " << m_strUuid << endl; 
        cout << "SwitchType : " << m_nSwitchType << endl; 
    }
private:    
    string m_strName;
    string m_strUuid;    
    int32 m_nSwitchType;     
};

// switch base 
class CDbSwitch
{
public: 
    CDbSwitch() 
    {
        m_nState = m_nSwitchType = 0;
        m_nMaxMtu  = 0;
        m_strName = m_strUuid =  "";
    };
    virtual ~CDbSwitch() {};

    string & GetName(){return m_strName;}; 
    string & GetUuid(){return m_strUuid;}; 
    int32 GetSwitchType(){return m_nSwitchType;};   
    int32 GetState(){return m_nState;};   
    int32 GetMaxMtu(){return m_nMaxMtu;};   

    void SetName(const char* value) {m_strName = value;};
    void SetUuid(const char* value) {m_strUuid = value;};
    void SetSwitchType(int32 value) {m_nSwitchType = value;};  
    void SetState(int32 value) {m_nState = value;};   
    void SetMaxMtu(int32 value) {m_nMaxMtu = value;};   

    void DbgSwitchShow()
    {    
        cout << "Name       : " << m_strName << endl;   
        cout << "Uuid       : " << m_strUuid << endl; 
        cout << "SwitchType : " << m_nSwitchType << endl; 
        cout << "State      : " << m_nState << endl; 
        cout << "MaxMtu     : " << m_nMaxMtu << endl;    
    }
private:    
    string m_strName;
    string m_strUuid;    
    int32 m_nSwitchType; 
    int32 m_nState;
    int32 m_nMaxMtu;

};

class CDBOperateSwitchBase 
{
public:
    CDBOperateSwitchBase() {};
    virtual ~CDBOperateSwitchBase() {};

    int32 QuerySwitchBaseCallback(CADORecordset * prs,void * nil);
};

} // namespace zte_tecs

#endif // _VNET_DB_SWITCH_INCLUDE_H_ 


