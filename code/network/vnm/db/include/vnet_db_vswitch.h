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

#if !defined(_VNET_DB_VIRTUAL_SWITCH_INCLUDE_H_)
#define _VNET_DB_VIRTUAL_SWITCH_INCLUDE_H_

#include "vnet_db_in.h"
#include "vnet_db_interface.h"
#include "vnet_db_proc.h"
#include "vnet_db_switch.h"

namespace zte_tecs
{

class CDbSwitchVsiKernelNum
{
public: 
    CDbSwitchVsiKernelNum() 
    {
        m_nRet = m_nVsiNum = 0;
     };
    virtual ~CDbSwitchVsiKernelNum() {};
    
    int32 GetRet(){return m_nRet;}; 
    int32 GetVsiNum(){return m_nVsiNum;}; 

    void SetRet(int32 value) {m_nRet = value;};   
    void SetVsiNum(int32 value) {m_nVsiNum = value;};   
private:
    int32 m_nRet;
    int32 m_nVsiNum;

};

// vswitch db
class CDbVSwitch : public  CDbSwitch
{
public: 
    CDbVSwitch() 
    {
        m_nEvbMode = m_nIsActive = m_nNicMaxNumCfg  = 0;
        m_strPgUuid = m_strSdnCfgUuid = "";
     };
    virtual ~CDbVSwitch() {};
    
    int32 GetEvbMode(){return m_nEvbMode;};  
    string & GetPgUuid(){return m_strPgUuid;}; 
    int32 GetNicMaxNumCfg(){return m_nNicMaxNumCfg;}; 
    int32 GetIsActive(){return m_nIsActive;};
    string & GetSdnCfgUuid(){return m_strSdnCfgUuid;};
    
    void SetEvbMode(int32 value) {m_nEvbMode = value;};   
    void SetPgUuid(const char* value) {m_strPgUuid = value;};
    void SetNicMaxNumCfg(int32 value) {m_nNicMaxNumCfg = value;};
    void SetIsActive(int32 value) {m_nIsActive = value;};   
    void SetSdnCfgUuid(const char* value) {m_strSdnCfgUuid = value;};

    void DbgShow()
    {
        DbgSwitchShow();        
        cout << "EvbMode     : " << m_nEvbMode << endl;   
        cout << "PgUuid      : " << m_strPgUuid << endl; 
        cout << "NicMaxNumCfg: " << m_nNicMaxNumCfg << endl; 
        cout << "IsActive    : " << m_nIsActive << endl; 
        cout << "SdnCfgUuid  : " << m_strSdnCfgUuid << endl;         
    }
private:    

    int32 m_nEvbMode;
    string m_strPgUuid;
    int32 m_nNicMaxNumCfg;
    int32 m_nIsActive;
    string m_strSdnCfgUuid;
};


class CVNetDbIProcedure;
class CDBOperateVSwitch : public  CVNetDbIProcedureCallback, public CDBOperateSwitchBase
{
public:
    explicit CDBOperateVSwitch(CVNetDbIProcedure * pb);
    virtual ~CDBOperateVSwitch();

    int32 QueryVsiNum( const char* switch_uuid, int32 & outVsiNum );  
    int32 QueryVsiNumBySwitchVna( const char* switch_uuid, const char* vna_uuid, int32 & outVsiNum ); 

    int32 QueryKernelNum( const char* switch_uuid, int32 & outVsiNum );  
    int32 QueryKernelNumBySwitchVna( const char* switch_uuid, const char* vna_uuid, int32 & outVsiNum ); 
    
    int32 QuerySummary( vector<CDbSwitchSummary> & outVInfo);
    int32 SdnQuerySummary(vector<CDbSwitchSummary> & outVInfo);
    int32 Query(CDbVSwitch & Info);
    int32 CheckAdd(CDbVSwitch & info);
    int32 Add(CDbVSwitch & info);
    int32 CheckModify(CDbVSwitch & info);
    int32 Modify(CDbVSwitch & info);
    int32 CheckDel(const char* switch_uuid);
    int32 Del(const char* switch_uuid);

    int32 DbProcCallback(int32 type, CADORecordset *, void* );

    void DbgShow();    
private:
    int32 QueryVsiKernelNumCallback(CADORecordset * prs,void * nil);
    int32 QuerySummaryCallback(CADORecordset * prs,void * nil);
    int32 QuerySdnSummaryCallback(CADORecordset * prs,void *nil);
    int32 QueryCallback(CADORecordset * prs,void * nil);
    int32 OperateCallback(CADORecordset * prs,void * nil);
    int32 CheckOperateCallback(CADORecordset * prs,void * nil);    
    
private:
    DISALLOW_COPY_AND_ASSIGN(CDBOperateVSwitch);

    CVNetDbIProcedure * m_pIProc;
};
    
} // namespace zte_tecs

#endif // _VNET_DB_VIRTUAL_SWITCH_INCLUDE_H_ 


