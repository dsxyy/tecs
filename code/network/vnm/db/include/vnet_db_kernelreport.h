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

#if !defined(_VNET_DB_KERNEL_REPORT_INCLUDE_H_)
#define _VNET_DB_KERNEL_REPORT_INCLUDE_H_

#include "vnet_db_in.h"
#include "vnet_db_interface.h"
#include "vnet_db_proc.h"

namespace zte_tecs
{

class CDbKernelReport
{
public: 
    CDbKernelReport() 
    {
        m_nIsOnline = m_strUplinkPortType = 0;
        m_strVnaUuid = m_strSwitchName = m_strIp = m_strMask = "";
        m_strKernelPortName = m_strUplinkPortName = "";
        m_nIsDhcp = 0;
        m_strUplinkPortBondMode = 1;
     };
    virtual ~CDbKernelReport() {};

    string & GetVnaUuid(){return m_strVnaUuid;}; 
    string & GetUuid(){return m_strUuid;};
    string & GetSwitchName(){return m_strSwitchName;}; 
    string & GetIp(){return m_strIp;}; 
    string & GetMask(){return m_strMask;};
    string & GetKernelPortName(){return m_strKernelPortName;}; 
    string & GetUplinkPortName(){return m_strUplinkPortName;}; 
    int32 GetUplinkPortType(){return m_strUplinkPortType;}; 
    int32 GetBondMode(){return m_strUplinkPortBondMode;}; 
    int32 GetIsOnline(){return m_nIsOnline;}; 
    int32 GetIsDhcp(){return m_nIsDhcp;};    

    void SetVnaUuid(const char* value) {m_strVnaUuid = value;}; 
    void SetUuid(const char* value) {m_strUuid = value;}; 
    void SetSwitchName(const char* value) {m_strSwitchName = value;}; 
    void SetIp(const char* value) {m_strIp = value;}; 
    void SetMask(const char* value) {m_strMask = value;}; 
    void SetKernelPortName(const char* value) {m_strKernelPortName = value;}; 
    void SetUplinkPortName(const char* value) {m_strUplinkPortName = value;};     
    void SetUplinkPortType(int32 value) {m_strUplinkPortType = value;};   
    void SetBondMode(int32 value) {m_strUplinkPortBondMode = value;}; 
    void SetIsOnline(int32 value) {m_nIsOnline = value;};   
    void SetIsDhcp(int32 value) {m_nIsDhcp = value;};     

    void DbgShow()
    {
        cout << "VnaUuid       : " << m_strVnaUuid << endl;
        cout << "Uuid          : " << m_strUuid << endl;
        cout << "SwitchName    : " << m_strSwitchName << endl;
        cout << "Ip            : " << m_strIp << endl;    
        cout << "Mask          : " << m_strMask << endl;
        cout << "KernelPortName: " << m_strKernelPortName << endl;
        cout << "UplinkPortName: " << m_strUplinkPortName << endl;
        cout << "UplinkPortType: " << m_strUplinkPortType << endl;  
        cout << "BondMode      : " << m_strUplinkPortBondMode << endl;  
        cout << "IsOnline      : " << m_nIsOnline << endl;          
        cout << "IsDhcp      : " << m_nIsDhcp << endl;          
    }
private:
    string m_strVnaUuid;
    string m_strUuid;    
    string m_strSwitchName;
    string m_strIp;
    string m_strMask;
    string m_strKernelPortName;
    string m_strUplinkPortName;
    int32 m_strUplinkPortType;
    int32 m_strUplinkPortBondMode;
    int32 m_nIsOnline;
    int32 m_nIsDhcp;
};


class CDbKernelReportBondMap
{
public: 
    CDbKernelReportBondMap() 
    {
        m_strVnaUuid = m_strSwitchName = "";
        m_strUplinkPortPhyName = m_strUplinkPortName = "";

        m_strUplinkPortBondMode = 1;
     };
    virtual ~CDbKernelReportBondMap() {};

    string & GetVnaUuid(){return m_strVnaUuid;}; 
    string & GetUuid(){return m_strUuid;};
    string & GetSwitchName(){return m_strSwitchName;}; 
    string & GetUplinkPortName(){return m_strUplinkPortName;}; 
    int32 GetBondMode(){return m_strUplinkPortBondMode;}; 
    string& GetBondPhyName(){return m_strUplinkPortPhyName;}; 

    void SetVnaUuid(const char* value) {m_strVnaUuid = value;}; 
    void SetUuid(const char* value) {m_strUuid = value;}; 
    void SetSwitchName(const char* value) {m_strSwitchName = value;}; 
    void SetUplinkPortName(const char* value) {m_strUplinkPortName = value;}; 
    void SetBondMode(int32 value) {m_strUplinkPortBondMode = value;}; 
    void SetBondPhyName(const char* value) {m_strUplinkPortPhyName = value;};   

    void DbgShow()
    {
        cout << "VnaUuid       : " << m_strVnaUuid << endl;
        cout << "Uuid          : " << m_strUuid << endl;
        cout << "SwitchName    : " << m_strSwitchName << endl;
        cout << "UplinkPortName: " << m_strUplinkPortName << endl;
        cout << "BondMode      : " << m_strUplinkPortBondMode << endl;  
        cout << "BondPhyName   : " << m_strUplinkPortPhyName << endl;          
    }
private:
    string m_strVnaUuid;
    string m_strUuid;    
    string m_strSwitchName;
    string m_strUplinkPortName;
    int32 m_strUplinkPortBondMode;
    string m_strUplinkPortPhyName;
};

class CVNetDbIProcedure;
class CDBOperateKernelReport : public  CVNetDbIProcedureCallback
{
public:
    explicit CDBOperateKernelReport(CVNetDbIProcedure * pb);
    virtual ~CDBOperateKernelReport();

    // 通过uuid来查询
    int32 Query(CDbKernelReport & Info);
    int32 QueryByVna(CDbKernelReport & Info);
    
    int32 CheckAdd(CDbKernelReport & info);
    int32 Add(CDbKernelReport & info);
    int32 CheckModify(CDbKernelReport & info);
    int32 Modify(CDbKernelReport & info);
    int32 CheckDel(const char* switch_uuid);
    int32 Del(const char* switch_uuid);

    // 查询kernelreport bond map
    int32 QueryBondMap(const char* uuid, vector<CDbKernelReportBondMap> &outVInfo);
    int32 QueryBondMapByVna(const char* uuid, vector<CDbKernelReportBondMap> &outVInfo);

    int32 CheckAddBondMap(const char* uuid, const char* phy_name);
    int32 AddBondMap(const char* uuid, const char* phy_name); 
    int32 CheckDelBondMap(const char* uuid, const char* phy_name);
    int32 DelBondMap(const char* uuid, const char* phy_name);       

    int32 DbProcCallback(int32 type, CADORecordset *, void* );

    void DbgShow();    
private:
    int32 QueryCallback(CADORecordset * prs,void * nil);
    int32 QueryBondMapCallback(CADORecordset * prs,void * nil);
    int32 OperateCallback(CADORecordset * prs,void * nil);
    int32 CheckOperateCallback(CADORecordset * prs,void * nil);  

private:
    DISALLOW_COPY_AND_ASSIGN(CDBOperateKernelReport);

    CVNetDbIProcedure * m_pIProc;
};
    
} // namespace zte_tecs

#endif // _VNET_DB_KERNEL_REPORT_INCLUDE_H_ 



