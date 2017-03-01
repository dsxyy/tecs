

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

#if !defined(_VNET_DB_SCHEDULE_INCLUDE_H_)
#define _VNET_DB_SCHEDULE_INCLUDE_H_

#include "vnet_db_in.h"
#include "vnet_db_interface.h"
#include "vnet_db_proc.h"
#include "vnet_db_vsi.h"

namespace zte_tecs
{
class CDbSchedule
{
public: 
    CDbSchedule() 
    {
        m_strNetplaneUuid = m_strClusterName = ""; 
        m_nIsSriov =  m_nIsLoop = m_nIsSdn = m_nIsVxlan = m_nNicNum = m_nIsBondNic = m_nMtu = m_nIsHasWatchdog = 0;

        m_strVnaUuid= m_strModuleName = m_strSwitchUuid = m_strPortUuid = "";
        m_nSwitchSriovFreeNum = m_nSwitchFreeNum = m_nUplinkPgMtu = 0;
        m_strBondPhyPortUuid = "";
        m_nPortType = m_nBondTotalPhyPortNum = m_nBondValidPhyPortNum = m_nBondPhyPortFreeNum = 0;
    };
    virtual ~CDbSchedule() {};

    string & GetNetplaneUuid(){return m_strNetplaneUuid;};
    int32  GetIsSriov(){return m_nIsSriov;};   
    int32  GetIsLoop(){return m_nIsLoop;}; 
    int32  GetNicNum(){return m_nNicNum;};       
    int32  GetIsBondNic(){return m_nIsBondNic;};    
    int32  GetMtu(){return m_nMtu;};    
    int32  GetIsHasWatchdog(){return m_nIsHasWatchdog;};
    int32  GetIsSdn(){return m_nIsSdn;};
    int32  GetIsVxlan(){return m_nIsVxlan;};        
    string & GetClusterName(){return m_strClusterName;};
    string  GetVnaUuid()const{return m_strVnaUuid;}; 
    string & GetModuleName(){return m_strModuleName;}; 
    string & GetSwitchUuid(){return m_strSwitchUuid;}; 
    int32  GetSwitchSriovFreeNum(){return m_nSwitchSriovFreeNum;};
    int32  GetSwitchFreeNum(){return m_nSwitchFreeNum;};
    int32  GetUplinkPgMtu(){return m_nUplinkPgMtu;};
    string  GetPortUuid()const{return m_strPortUuid;};

    int32  GetPortType(){return m_nPortType;};
    int32  GetBondTotalPhyPortNum(){return m_nBondTotalPhyPortNum;};
    int32  GetBondValidPhyPortNum(){return m_nBondValidPhyPortNum;};
    string &  GetBondPhyPortUuid(){return m_strBondPhyPortUuid;};
    int32  GetBondPhyPortFreeNum(){return m_nBondPhyPortFreeNum;};

        
    void SetNetplaneUuid(const char* value) {m_strNetplaneUuid = value;};
    void SetIsSriov(int32 value) {m_nIsSriov = value;};   
    void SetIsLoop(int32 value) {m_nIsLoop = value;};
    void SetNicNum (int32 value) {m_nNicNum = value;};  
    void SetIsBondNic (int32 value) {m_nIsBondNic = value;};      
    void SetMtu(int32 value) {m_nMtu = value;}; 
    void SetIsHasWatchdog(int32 value) {m_nIsHasWatchdog = value;};   
    void SetIsSdn(int32 value) {m_nIsSdn = value;}; 
    void SetIsVxlan(int32 value) {m_nIsVxlan = value;};   
    void SetClusterName(const char* value) {m_strClusterName = value;}; 
    void SetVnaUuid(const char* value) {m_strVnaUuid = value;}; 
    void SetModuleName(const char* value) {m_strModuleName = value;}; 
    void SetSwitchUuid(const char* value) {m_strSwitchUuid = value;}; 
    void SetSwitchSriovFreeNum(int32 value) {m_nSwitchSriovFreeNum = value;}; 
    void SetSwitchFreeNum(int32 value) {m_nSwitchFreeNum = value;}; 
    void SetUplinkPgMtu(int32 value) {m_nUplinkPgMtu = value;}; 
    void SetPortUuid(const char* value) {m_strPortUuid = value;}; 

    void SetPortType(int32 value) {m_nPortType = value;}; 
    void SetBondTotalPhyPortNum(int32 value) {m_nBondTotalPhyPortNum = value;}; 
    void SetBondValidPhyPortNum(int32 value) {m_nBondValidPhyPortNum = value;}; 
    void SetBondPhyPortUuid(const char* value) {m_strBondPhyPortUuid = value;}; 
    void SetBondPhyPortFreeNum(int32 value) {m_nBondPhyPortFreeNum = value;};     
    
    void DbgShow()
    {
        cout << "NetplaneUuid       : " << m_strNetplaneUuid << endl;
        cout << "IsSriov            : " << m_nIsSriov << endl;       
        cout << "IsLoop             : " << m_nIsLoop << endl;         
        cout << "NicNum             : " << m_nNicNum << endl;  
        cout << "IsBondNic          : " << m_nIsBondNic << endl;          
        cout << "Mtu                : " << m_nMtu << endl;         
        cout << "isHasWatchdog      : " << m_nIsHasWatchdog << endl; 
        cout << "IsSdn              : " << m_nIsSdn << endl; 
        cout << "IsVxlan              : " << m_nIsVxlan << endl;              
        cout << "ClusterName        : " << m_strClusterName << endl;  
        cout << "VnaUuid            : " << m_strVnaUuid << endl; 
        cout << "ModuleName         : " << m_strModuleName << endl; 
        cout << "SwitchUuid         : " << m_strSwitchUuid << endl; 
        cout << "SwitchSriovFreeNum : " << m_nSwitchSriovFreeNum << endl; 
        cout << "SwitchFreeNum      : " << m_nSwitchFreeNum << endl; 
        cout << "UplinkPgMtu        : " << m_nUplinkPgMtu << endl; 
        cout << "PortUuid           : " << m_strPortUuid << endl;         
        cout << "PortType           : " << m_nPortType << endl;  
        cout << "BondTotalPhyPortNum: " << m_nBondTotalPhyPortNum << endl;  
        cout << "BondValidPhyPortNum: " << m_nBondValidPhyPortNum << endl;  
        cout << "BondPhyPortUuid    : " << m_strBondPhyPortUuid << endl;          
        cout << "BondPhyPortFreeNum : " << m_nBondPhyPortFreeNum << endl;             
    }
public:    
    string m_strNetplaneUuid;
    int32  m_nIsSriov;
    int32  m_nIsLoop;
    int32  m_nNicNum;
    int32  m_nIsBondNic;
    int32  m_nMtu;
    int32  m_nIsHasWatchdog;
    int32  m_nIsSdn;
    int32  m_nIsVxlan;
    
    
    // 结果
    string m_strClusterName;  
    string m_strVnaUuid;
    string m_strModuleName;
    string m_strSwitchUuid;
    int32 m_nSwitchSriovFreeNum;  // sdvs 该值为switch 剩余可配置VNIC总数
                                  // edvs if bond 时，该值为bond的剩余可用VF数值
                                  // edvs if physical,该值为physical的剩余可用VF数值 
    int32 m_nSwitchFreeNum;       // 此值和m_nSwitchSriovFreeNum相同
    int32 m_nUplinkPgMtu;
    string m_strPortUuid;  
    
    int32  m_nPortType;
    int32  m_nBondTotalPhyPortNum;
    int32  m_nBondValidPhyPortNum;
    string  m_strBondPhyPortUuid;
    int32  m_nBondPhyPortFreeNum;    
};

struct CompareCDbSchedule
{
    bool operator ()(CDbSchedule &s1,CDbSchedule &s2)
    {
        if ((s1.GetVnaUuid() != s2.GetVnaUuid()))
        {
           return (s1.GetVnaUuid() > s2.GetVnaUuid());
        }
        else
        {
           return (s1.GetPortUuid() > s2.GetPortUuid());
        }
    };///自定义一个仿函数
};

class CDbDeplySummary
{
public: 
    CDbDeplySummary() 
    {
        m_strClusterName = ""; 
        m_nIsSriov = m_nIsLoop = 0;

        m_strVnaUuid= m_strModuleName = m_strSwitchUuid = m_strPortUuid = m_strLgNetworkUuid = "";
        m_oProjectId = 0;
    };
    virtual ~CDbDeplySummary() {};

    int64  GetProjectId(){return m_oProjectId;};
    string & GetLgNetworkUuid(){return m_strLgNetworkUuid;};
    int32  GetIsSriov(){return m_nIsSriov;};   
    int32  GetIsLoop(){return m_nIsLoop;};  
    int32  GetIsSdn(){return m_nIsSdn;}; 
    string & GetClusterName(){return m_strClusterName;};
    string & GetVnaUuid(){return m_strVnaUuid;}; 
    string & GetModuleName(){return m_strModuleName;}; 
    string & GetSwitchUuid(){return m_strSwitchUuid;}; 
    string & GetPortUuid(){return m_strPortUuid;};
    
    void SetProjectId(int64 value) {m_oProjectId = value;};
    void SetLgNetworkUuid (const char* value) {m_strLgNetworkUuid = value;}; 
    void SetIsSriov(int32 value) {m_nIsSriov = value;};   
    void SetIsLoop(int32 value) {m_nIsLoop = value;};
    void SetIsSdn(int32 value) {m_nIsSdn = value;};
    void SetClusterName(const char* value) {m_strClusterName = value;}; 
    void SetVnaUuid(const char* value) {m_strVnaUuid = value;}; 
    void SetModuleName(const char* value) {m_strModuleName = value;}; 
    void SetSwitchUuid(const char* value) {m_strSwitchUuid = value;}; 
    void SetPortUuid(const char* value) {m_strPortUuid = value;}; 
    
    void DbgShow()
    {
        cout << "ProjectId         : " << m_oProjectId << endl;
        cout << "LgNetworkUuid     : " << m_strLgNetworkUuid << endl;
        cout << "IsSriov           : " << m_nIsSriov << endl;       
        cout << "IsLoop            : " << m_nIsLoop << endl; 
        cout << "IsSdn             : " << m_nIsSdn << endl;
        cout << "ClusterName       : " << m_strClusterName << endl;  
        cout << "VnaUuid           : " << m_strVnaUuid << endl; 
        cout << "ModuleName        : " << m_strModuleName << endl; 
        cout << "SwitchUuid        : " << m_strSwitchUuid << endl; 
        cout << "PortUuid          : " << m_strPortUuid << endl;         
    }
private:    
    int64 m_oProjectId;
    string m_strLgNetworkUuid;
    int32  m_nIsSriov;
    int32  m_nIsLoop;
    int32  m_nIsSdn;
    
    // 结果
    string m_strClusterName;  
    string m_strVnaUuid;
    string m_strModuleName;
    string m_strSwitchUuid;
    string m_strPortUuid;  
};

class CDbScheduleWdg
{
public: 
    CDbScheduleWdg() 
    {
        m_strClusterName = ""; 
        m_strVnaUuid= m_strModuleName  = "";
    };
    virtual ~CDbScheduleWdg() {};
 
    string & GetClusterName(){return m_strClusterName;};
    string  GetVnaUuid()const{return m_strVnaUuid;}; 
    string & GetModuleName(){return m_strModuleName;}; 

            
    void SetClusterName(const char* value) {m_strClusterName = value;}; 
    void SetVnaUuid(const char* value) {m_strVnaUuid = value;}; 
    void SetModuleName(const char* value) {m_strModuleName = value;};  
    CDbScheduleWdg & operator=(const CDbScheduleWdg &d)
    {
        m_strClusterName = d.m_strClusterName;  
        m_strVnaUuid = d.m_strVnaUuid;
        m_strModuleName = d.m_strModuleName;
        return *this;
    };
    
    void DbgShow()
    {            
        cout << "ClusterName        : " << m_strClusterName << endl;  
        cout << "VnaUuid            : " << m_strVnaUuid << endl; 
        cout << "ModuleName         : " << m_strModuleName << endl;     
    }
public:    
    
    string m_strVnaUuid;
    string m_strModuleName;    
    string m_strClusterName;  
    
};

class CVNetDbIProcedure;
class CDBOperateSchedule : public  CVNetDbIProcedureCallback 
{
public:
    explicit CDBOperateSchedule(CVNetDbIProcedure * pb);
    virtual ~CDBOperateSchedule();

    //check resource 
    int32 CheckMacResource(const char* netplane_uuid, int32 nic_num);
    int32 CheckIpResource(const char* lgnetwork_uuid, int32 nic_num);

    //通过netplane mtu is_sriov, is_loop, nic_num, mtu 来调度 
    //input (m_strNetplaneUuid,m_nIsSriov,m_nIsLoop,m_nNicNum,m_nMtu 必填) 
    int32 Schedule(CDbSchedule & inInfo, vector<CDbSchedule> &outInfo);
    int32 ScheduleByCluster(CDbSchedule & inInfo, vector<CDbSchedule> &outInfo); 
    int32 ScheduleByVna(CDbSchedule & inInfo, vector<CDbSchedule> &outInfo); 
    
    //调度看门狗VM
    int32 ScheduleWdg(vector<CDbScheduleWdg> &outInfo);
    int32 ScheduleWdgByCluster(const string& inClusterName,vector<CDbScheduleWdg> &outInfo);    
    int32 ScheduleWdgByVna(const string& inVnaUuid,vector<CDbScheduleWdg> &outInfo);    
    
    
    //查询已部署虚拟机vna switch port(通过project_id, lgnetwork, is_loop, is_sriov)    
    int32 QueryDeployByProjectLn(CDbDeplySummary & inInfo, vector<CDbDeplySummary> &outInfo);
    //查询已部署虚拟机vna switch port(通过project_id, is_loop, is_sriov)    
    int32 QueryDeployByProject(CDbDeplySummary & inInfo, vector<CDbDeplySummary> &outInfo);

    int32 DbProcCallback(int32 type, CADORecordset *, void* );

    void DbgShow();    
private:
    int32 ScheduleProc(CDbSchedule & inInfo, vector<CDbSchedule> &outInfo);
    int32 ScheduleByClusterProc(CDbSchedule & inInfo, vector<CDbSchedule> &outInfo); 
    int32 ScheduleByVnaProc(CDbSchedule & inInfo, vector<CDbSchedule> &outInfo);
    
    
    int32 ScheduleCallback(CADORecordset * prs,void * nil);
    int32 ScheduleWdgCallback(CADORecordset * prs,void * nil);
    int32 ScheduleQueryDeployCallback(CADORecordset * prs,void * nil);
    int32 CheckOperateCallback(CADORecordset * prs,void * nil);  
private:
    DISALLOW_COPY_AND_ASSIGN(CDBOperateSchedule);

    CVNetDbIProcedure * m_pIProc;
};
    
    
} // namespace zte_tecs

#endif // _VNET_DB_SCHEDULE_DEPLOY_INCLUDE_H_ 





