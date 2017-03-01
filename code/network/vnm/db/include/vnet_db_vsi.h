/******************************************************************************
* Copyright (c) 2013，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：vnet_db_netplane.h
* 文件标识：
* 内容摘要：CVNetDb类的定义文件
* 当前版本：1.0
* 作    者：
* 完成日期：2013年1月28日
*
* 修改记录1：
*     修改日期：2013/1/28
*     版 本 号：V1.0
*     修 改 人：gong.xiefeng
*     修改内容：创建
*
******************************************************************************/

#if !defined(_VNET_DB_VSI_INCLUDE_H_)
#define _VNET_DB_VSI_INCLUDE_H_

#include "vnet_db_in.h"
#include "vnet_db_interface.h"
#include "vnet_db_proc.h"

namespace zte_tecs
{

// vnic 
// schedule 也需要此信息 
class CDbVnic
{
public: 
    CDbVnic() 
    {
        m_oVmId   = 0;
        m_nNicIndex = m_nIsSriov = m_nIsLoop  = 0;
        
        m_strLogicNetworkUuid = m_strPortType ="";

        m_strAssIP = m_strAssMask = m_strAssGateway = m_strAssMac = "";
    };
    virtual ~CDbVnic() {};

    int64 GetVmId(){return m_oVmId;};  
    int32 GetNicIndex(){return m_nNicIndex;};  
    int32 GetIsSriov(){return m_nIsSriov;};  
    int32 GetIsLoop(){return m_nIsLoop;};  
    string & GetLogicNetworkUuid(){return m_strLogicNetworkUuid;};
    string & GetAssIP(){return m_strAssIP;};
    string & GetAssMask(){return m_strAssMask;};
    string & GetAssGateway(){return m_strAssGateway;};
    string & GetAssMac() {return m_strAssMac;};
    string & GetPortType(){return m_strPortType;}; 

    void SetVmId (const int64 value) {m_oVmId = value;};   
    void SetNicIndex(const int32 value) {m_nNicIndex = value;};
    void SetIsSriov(const int32 value) {m_nIsSriov = value;};   
    void SetIsLoop(const int32 value) {m_nIsLoop = value;};  
    void SetLogicNetworkUuid(const char* value) {m_strLogicNetworkUuid = value;};
    void SetAssIP(const char* value){m_strAssIP = value;};
    void SetAssMask(const char* value){m_strAssMask = value;};
    void SetAssGateway(const char* value){m_strAssGateway = value;};
    void SetAssMac(const char* value){m_strAssMac = value;};
    void SetPortType(const char* value) {m_strPortType = value;};  

    void DbgShow()
    {       
        cout << "VmId            : " << m_oVmId << endl;   
        
        cout << "NicIndex        : " << m_nNicIndex << endl; 
        cout << "IsSriov         : " << m_nIsSriov << endl; 
        cout << "IsLoop          : " << m_nIsLoop << endl; 
        cout << "LogicNetworkUuid: " << m_strLogicNetworkUuid << endl;
        cout << "AssginIP        : " << m_strAssIP << endl; 
        cout << "AssginMask      : " << m_strAssMask << endl; 
        cout << "AssginGateway   : " << m_strAssGateway << endl; 
        cout << "AssginMac       : " << m_strAssMac << endl;
        cout << "PortType        : " << m_strPortType << endl; 
    }
private:    
    int64 m_oVmId;
    int32 m_nNicIndex;
    int32 m_nIsSriov;
    int32 m_nIsLoop;
    string m_strLogicNetworkUuid;
    string m_strAssIP;
    string m_strAssMask;
    string m_strAssGateway;
    string m_strAssMac;
    string m_strPortType;
};

// vsi db
class CDbVsiInfo
{
public: 
    CDbVsiInfo() 
    {
    };
    virtual ~CDbVsiInfo() {};
        
    int64 GetVmId(){return m_vnic.GetVmId();};  
    int32 GetNicIndex(){return m_vnic.GetNicIndex();};   
    int32 GetIsSriov(){return m_vnic.GetIsSriov();};  
    int32 GetIsLoop(){return m_vnic.GetIsLoop();};  
    string & GetLogicNetworkUuid(){return m_vnic.GetLogicNetworkUuid();};
    string & GetAssIP(){return m_vnic.GetAssIP();};
    string & GetAssMask(){return m_vnic.GetAssMask();};
    string & GetAssGateway(){return m_vnic.GetAssGateway();};
    string & GetAssMac(){return m_vnic.GetAssMac();};
    string & GetPortType(){return m_vnic.GetPortType();}; 
    
    string & GetVsiIdValue(){return m_vsi.GetVsiIdValue();}; 
    int32 GetVsiIdFormat(){return m_vsi.GetVsiIdFormat();}; 
    string & GetVmPgUuid(){return m_vsi.GetVmPgUuid();}; 
    string & GetMac(){return m_vsi.GetMac();}; 
    int32    GetIsHasIp(){return m_vsi.GetIsHasIp();}; 
    string & GetIp(){return m_vsi.GetIp();}; 
    string & GetMask(){return m_vsi.GetMask();}; 
    int32    GetIsHasVSwitch(){return m_vsi.GetIsHasVSwitch();};     
    string & GetVSwitchUuid(){return m_vsi.GetVSwitchUuid();};    
    int32    GetIsHasSriovVf(){return m_vsi.GetIsHasSriovVf();};    
    string & GetSriovVfPortUuid(){return m_vsi.GetSriovVfPortUuid();};  
    string & GetQuantumNetworkUuid(){return m_vsi.GetQuantumNetworkUuid();}; 
    string & GetQuantumPortUuid(){return m_vsi.GetQuantumPortUuid();}; 
        
    
    void SetVmId (const int64 value) { m_vnic.SetVmId(value);};   
    void SetNicIndex(const int32 value){ m_vnic.SetNicIndex(value);};
    void SetIsSriov(const int32 value) {m_vnic.SetIsSriov( value);};   
    void SetIsLoop(const int32 value) {m_vnic.SetIsLoop(value);};  
    void SetLogicNetworkUuid(const char* value) {m_vnic.SetLogicNetworkUuid(value);};
    void SetAssIP(const char* value) {m_vnic.SetAssIP(value);};
    void SetAssMask(const char* value) {m_vnic.SetAssMask(value);};
    void SetAssGateway(const char* value) {m_vnic.SetAssGateway(value);};
    void SetAssMac(const char* value) {m_vnic.SetAssMac(value);};
    void SetPortType(const char* value) {m_vnic.SetPortType(value);};  
    
    void SetVsiIdValue(const char* value) {m_vsi.SetVsiIdValue(value);};  
    void SetVsiIdFormat(int32 value) {m_vsi.SetVsiIdFormat(value);};   
    void SetVmPgUuid(const char* value) {m_vsi.SetVmPgUuid(value);};  
    void SetMac(const char* value) {m_vsi.SetMac(value);};  
    void SetIsHasIp(const int32 value) {m_vsi.SetIsHasIp(value);};  
    void SetIp(const char* value) {m_vsi.SetIp(value);};
    void SetMask(const char* value) {m_vsi.SetMask(value);};  
    void SetIsHasVSwitch(const int32 value) {m_vsi.SetIsHasVSwitch(value);}; 
    void SetVSwitchUuid(const char* value) {m_vsi.SetVSwitchUuid(value);};   
    void SetIsHasSriovVf(const int32 value) {m_vsi.SetIsHasSriovVf(value);};  
    void SetSriovVfPortUuid(const char* value) {m_vsi.SetSriovVfPortUuid(value);};      
    void SetQuantumNetworkUuid(const char* value) {m_vsi.SetQuantumNetworkUuid(value);}; 
    void SetQuantumPortUuid(const char* value) {m_vsi.SetQuantumPortUuid(value);};     

    void DbgShow()
    {
        cout << "CDbVsiInfo : " << endl;   
        m_vnic.DbgShow();
        m_vsi.DbgShow();        
        cout << endl;   
    }
private:    
    // 下面两个类是CDbVsiInfo的私有成员类，不对外暴露
    
    
    // vsi   
    class CDbVsi
    {
    public: 
        CDbVsi() 
        {
            m_nIsHasIp = m_nIsHasVSwitch = m_nIsHasSriovVf = 0;
            m_nVsiIdFormat = 5;
            m_strVsiIdValue =  "";
            m_strVmPgUuid = m_strMac = m_strIp = m_strMask = m_strVSwitchUuid = m_strSriovVfPortUuid = "";
            m_strQuantumPortUuid = m_strQuantumNetworkUuid = "";
         };
        virtual ~CDbVsi() {};
        
         
        string & GetVsiIdValue(){return m_strVsiIdValue;}; 
        int32 GetVsiIdFormat(){return m_nVsiIdFormat;}; 
        string & GetVmPgUuid(){return m_strVmPgUuid;}; 
        string & GetMac(){return m_strMac;}; 
        int32    GetIsHasIp(){return m_nIsHasIp;}; 
        string & GetIp(){return m_strIp;}; 
        string & GetMask(){return m_strMask;}; 
        int32    GetIsHasVSwitch(){return m_nIsHasVSwitch;};     
        string & GetVSwitchUuid(){return m_strVSwitchUuid;};    
        int32    GetIsHasSriovVf(){return m_nIsHasSriovVf;};    
        string & GetSriovVfPortUuid(){return m_strSriovVfPortUuid;};  
        string & GetQuantumNetworkUuid(){return m_strQuantumNetworkUuid;};  
        string & GetQuantumPortUuid(){return m_strQuantumPortUuid;};
            
        
        void SetVsiIdValue(const char* value) {m_strVsiIdValue = value;};  
        void SetVsiIdFormat(const int32 value) {m_nVsiIdFormat = value;};   
        void SetVmPgUuid(const char* value) {m_strVmPgUuid = value;};  
        void SetMac(const char* value) {m_strMac = value;};  
        void SetIsHasIp(const int32 value) {m_nIsHasIp = value;};  
        void SetIp(const char* value) {m_strIp = value;};
        void SetMask(const char* value) {m_strMask = value;};  
        void SetIsHasVSwitch(const int32 value) {m_nIsHasVSwitch = value;}; 
        void SetVSwitchUuid(const char* value) {m_strVSwitchUuid = value;};   
        void SetIsHasSriovVf(const int32 value) {m_nIsHasSriovVf = value;};  
        void SetSriovVfPortUuid(const char* value) {m_strSriovVfPortUuid = value;};      
        void SetQuantumNetworkUuid(const char* value) {m_strQuantumNetworkUuid = value;};  
        void SetQuantumPortUuid(const char* value) {m_strQuantumPortUuid = value;}; 
    
        void DbgShow()
        {       
            cout << "VsiIdValue      : " << m_strVsiIdValue << endl; 
            cout << "VsiIdFormat     : " << m_nVsiIdFormat << endl;         
            cout << "VmPgUuid        : " << m_strVmPgUuid << endl; 
            cout << "Mac             : " << m_strMac << endl; 
            cout << "IsHasIp         : " << m_nIsHasIp << endl; 
            cout << "Ip              : " << m_strIp << endl; 
            cout << "Mask            : " << m_strMask << endl; 
            cout << "IsHasVSwitch    : " << m_nIsHasVSwitch << endl; 
            cout << "VSwitchUuid     : " << m_strVSwitchUuid << endl; 
            cout << "IsHasSriovVf    : " << m_nIsHasSriovVf << endl; 
            cout << "SriovVfPortUuid : " << m_strSriovVfPortUuid << endl;   
            cout << "QuantumNetworkUuid : " << m_strQuantumNetworkUuid << endl;  
            cout << "QuantumPortUuid : " << m_strQuantumPortUuid << endl;                  
            
        }
    private:    
        string m_strVsiIdValue;
        int32  m_nVsiIdFormat;
        string m_strVmPgUuid;
        string m_strMac;
        int32  m_nIsHasIp;
        string m_strIp;
        string m_strMask;
        int32  m_nIsHasVSwitch;
        string m_strVSwitchUuid;
        int32  m_nIsHasSriovVf;
        string m_strSriovVfPortUuid;
        string m_strQuantumNetworkUuid;
        string m_strQuantumPortUuid;
    
    };

    CDbVnic m_vnic;
    CDbVsi  m_vsi;
};

class CDbVsiInfoSummary 
{

public: 
    CDbVsiInfoSummary() 
    {
        m_oVmId = m_oProjectId = 0;
        m_nNicIndex = 0;
        m_nVsiIdFormat  = 5;
        m_strVsiIdValue = "";
    };
    virtual ~CDbVsiInfoSummary() {};
        
    int64 GetVmId() const {return m_oVmId;};  
    int32 GetNicIndex() const{return m_nNicIndex;};  
    int64 GetProjectId() const{return m_oProjectId;}; 
    string & GetVsiIdValue(){return m_strVsiIdValue;};
    int32 GetVsiIdFormat() const{return m_nVsiIdFormat;};

    
    void SetVmId (const int64 value) {m_oVmId = value;};   
    void SetNicIndex(const int32 value) {m_nNicIndex = value;};
    void SetProjectId(const int64 value) {m_oProjectId = value;};
    void SetVsiIdValue(const char* value) {m_strVsiIdValue = value;};  
    void SetVsiIdFormat(const int32 value) {m_nVsiIdFormat = value;}; 

    void DbgShow()
    {       
        cout << "VmId            : " << m_oVmId << endl;   
        cout << "NicIndex        : " << m_nNicIndex << endl; 
        cout << "ProjectId       : " << m_oProjectId << endl;
        cout << "VsiIdValue      : " << m_strVsiIdValue << endl; 
        cout << "VsiIdFormat     : " << m_nVsiIdFormat << endl;  
    }
private:    
    int64 m_oVmId;
    int32 m_nNicIndex;
    int64 m_oProjectId;
    string m_strVsiIdValue;
    int32  m_nVsiIdFormat;    
};

class CVNetDbIProcedure;
class CDBOperateVsi : public  CVNetDbIProcedureCallback 
{
public:
    explicit CDBOperateVsi(CVNetDbIProcedure * pb);
    virtual ~CDBOperateVsi();

    //通过vsi_id来查询 
    int32 QueryByVsi(CDbVsiInfo & Info);
    int32 QueryByVmVnic(CDbVsiInfo & Info);

    //通过VM 来查询 
    int32 QuerySummaryByVm(int64 vmid, vector<CDbVsiInfoSummary> & outVInfo);
    int32 QuerySummary(vector<CDbVsiInfoSummary> & outVInfo);

    // 添加接口
    int32 CheckAdd(CDbVsiInfo & info);
    int32 Add(CDbVsiInfo & info);
    int32 CheckModify(CDbVsiInfo & info);
    int32 Modify(CDbVsiInfo & info);
    int32 CheckDel(const char* vsiId);
    int32 Del(const char* vsiId);
    int32 DelByVmId(int64 vm_id);   

#if 0 // 后面添加 
    int32 DelByVmVnic(int64 vm_id, int32 index);  
#endif 
    int32 DbProcCallback(int32 type, CADORecordset *, void* );

    void DbgShow();    
private:
    int32 QuerySummaryCallback(CADORecordset * prs,void * nil);
    int32 QueryCallback(CADORecordset * prs,void * nil);
    int32 OperateCallback(CADORecordset * prs,void * nil);
    int32 CheckOperateCallback(CADORecordset * prs,void * nil);    
    
private:
    DISALLOW_COPY_AND_ASSIGN(CDBOperateVsi);

    CVNetDbIProcedure * m_pIProc;
};
    
} // namespace zte_tecs

#endif // _VNET_DB_VSI_INCLUDE_H_ 


