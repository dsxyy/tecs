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

#if !defined(_VNET_DB_VIRTUAL_SWITCH_MAP_INCLUDE_H_)
#define _VNET_DB_VIRTUAL_SWITCH_MAP_INCLUDE_H_

#include "vnet_db_in.h"
#include "vnet_db_interface.h"
#include "vnet_db_proc.h"
#include "vnet_db_switch.h"

namespace zte_tecs
{

// vswitch db
class CDbVSwitchMap
{
public: 
    CDbVSwitchMap() 
    {
        m_strVSwitchUuid = m_strVSwitchName = "";
        m_strVnaUuid = m_strVnaApp = m_strVnmApp = m_strHostName = "";
        m_strPortUuid = m_strPortName = "";
        m_nSwitchType = m_nPortType = 0;
        m_nSwitchMapIsConsistency = m_nSwitchMapIsCfg = m_nSwitchMapIsSelected  =0 ;
    };
    virtual ~CDbVSwitchMap() {};
      
    string & GetVSwitchUuid(){return m_strVSwitchUuid;}; 
    string & GetVSwitchName(){return m_strVSwitchName;}; 
    int32 GetSwitchType(){return m_nSwitchType;}; 
    string & GetVnaUuid(){return m_strVnaUuid;}; 
    string & GetVnaApp(){return m_strVnaApp;}; 
    string & GetVnmApp(){return m_strVnmApp;}; 
    string & GetHostName(){return m_strHostName;}; 
    string & GetPortUuid(){return m_strPortUuid;}; 
    string & GetPortName(){return m_strPortName;}; 
    int32 GetPortType(){return m_nPortType;};  
    int32 GetSwitchMapIsCfg(){return m_nSwitchMapIsCfg;};  
    int32 GetSwitchMapIsSelected(){return m_nSwitchMapIsSelected;};  
    int32 GetSwitchMapIsConsistency(){return m_nSwitchMapIsConsistency;};  

    void SetVSwitchUuid(const char* value) {m_strVSwitchUuid = value;};
    void SetVSwitchName(const char* value) {m_strVSwitchName = value;};
    void SetSwitchType(int32 value) {m_nSwitchType = value;};   
    void SetVnaUuid(const char* value) {m_strVnaUuid = value;};
    void SetVnaApp(const char* value) {m_strVnaApp = value;};
    void SetVnmApp(const char* value) {m_strVnmApp = value;};
    void SetHostName(const char* value) {m_strHostName = value;};

    void SetPortUuid (const char* value) {m_strPortUuid = value;};
    void SetPortName(const char* value) {m_strPortName = value;};
    void SetPortType(int32 value) {m_nPortType = value;};

    void SetSwitchMapIsCfg(int32 value) {m_nSwitchMapIsCfg = value;};
    void SetSwitchMapIsSelected (int32 value) {m_nSwitchMapIsSelected = value;};
    void SetSwitchMapIsConsistency(int32 value) {m_nSwitchMapIsConsistency = value;};

    void DbgShow()
    {       
        cout << "VSwitchUuid : " << m_strVSwitchUuid << endl;   
        cout << "VSwitchName : " << m_strVSwitchName << endl; 
        cout << "SwitchType  : " << m_nSwitchType << endl; 

        cout << "VnaUuid     : " << m_strVnaUuid << endl;   
        cout << "VnaApp      : " << m_strVnaApp << endl; 
        cout << "VnmApp      : " << m_strVnmApp << endl; 
        cout << "HostName    : " << m_strHostName << endl;         

        cout << "PortUuid    : " << m_strPortUuid << endl;   
        cout << "PortName    : " << m_strPortName << endl; 
        cout << "PortType    : " << m_nPortType << endl;

        cout << "IsCfg       : " << m_nSwitchMapIsCfg << endl;
        cout << "IsSelected  : " << m_nSwitchMapIsSelected << endl;
        cout << "IsConsistency: " << m_nSwitchMapIsConsistency << endl;
    }
private:    
    string m_strVSwitchUuid;
    string m_strVSwitchName;    
    int32 m_nSwitchType;

    string m_strVnaUuid;
    string m_strVnaApp;    
    string m_strVnmApp;
    string m_strHostName; 
    
    string m_strPortUuid;
    string m_strPortName;    
    int32 m_nPortType;

    int32 m_nSwitchMapIsCfg;
    int32 m_nSwitchMapIsSelected;
    int32 m_nSwitchMapIsConsistency;
};


class CVNetDbIProcedure;
class CDBOperateVSwitchMap : public  CVNetDbIProcedureCallback 
{
public:
    explicit CDBOperateVSwitchMap(CVNetDbIProcedure * pb);
    virtual ~CDBOperateVSwitchMap();
 
    int32 QueryBySwitch(const char* switch_uuid, vector<CDbVSwitchMap> & outVInfo);  
    int32 QueryByVna(const char* vna_uuid, vector<CDbVSwitchMap> & outVInfo);    
    int32 QueryBySwitchVna(const char* switch_uuid,const char* vna_uuid, vector<CDbVSwitchMap> & outVInfo);    

    // ͨ��port ��ѯ��switch  
    int32 QueryByPort(const char* port_uuid, vector<CDbVSwitchMap> & outVInfo);
    
    int32 CheckAdd(const char* switch_uuid, const char* port_uuid);
    int32 Add(const char* switch_uuid, const char* port_uuid, int32 is_cfg, int32 is_selected, int32 is_consistency);
    int32 CheckModify(const char* switch_uuid, const char* port_uuid);
    int32 Modify(const char* switch_uuid, const char* port_uuid, int32 is_cfg, int32 is_selected, int32 is_consistency);

    int32 CheckDel(const char* switch_uuid, const char* port_uuid);
    int32 Del(const char* switch_uuid, const char* port_uuid);

    int32 DbProcCallback(int32 type, CADORecordset *, void* );

    void DbgShow();    
private:
    int32 QueryCallback(CADORecordset * prs,void * nil);
    
    int32 OperateCallback(CADORecordset * prs,void * nil);
    int32 CheckOperateCallback(CADORecordset * prs,void * nil);    
    
private:
    DISALLOW_COPY_AND_ASSIGN(CDBOperateVSwitchMap);

    CVNetDbIProcedure * m_pIProc;
};
    
} // namespace zte_tecs

#endif // _VNET_DB_VIRTUAL_SWITCH_MAP_INCLUDE_H_ 


