

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

#if !defined(_VNET_DB_NETPLANE_VLANPOOL_INCLUDE_H_)
#define _VNET_DB_NETPLANE_VLANPOOL_INCLUDE_H_

#include "vnet_db_in.h"
#include "vnet_db_interface.h"
#include "vnet_db_proc.h"

namespace zte_tecs
{

// logicnetwork db
class CDbNetplaneVlanPool
{
public: 
    CDbNetplaneVlanPool() 
    {
        m_strUuid =  "";
        m_nVlanNum = 0;
    };
    virtual ~CDbNetplaneVlanPool() {};
     
    string & GetUuid(){return m_strUuid;}; 
    int32  GetVlanNum(){return m_nVlanNum;};

     
    void SetUuid(const char* value) {m_strUuid = value;};
    void SetVlanNum(int32 value) {m_nVlanNum = value;};
    

    void DbgShow()
    {       
        cout << "Uuid    : " << m_strUuid << endl;  
        cout << "vlan_Num: " << m_nVlanNum << endl;   
    }
private:    
    string m_strUuid;
    int32  m_nVlanNum;
};

// for getting netplane share net trunk vlan config;
class CDbNetplaneTrunkVlan
{
public: 
    CDbNetplaneTrunkVlan() 
    {
        m_nVlanBegin  = 0;
        m_nVlanEnd    = 0;
        m_nNativeVlan = 0;
    }
    
    virtual ~CDbNetplaneTrunkVlan() {}
     
    int32 GetVlanBegin()    {return m_nVlanBegin;} 
    int32 GetVlanEnd()      {return m_nVlanEnd;}
    int32 GetNativeVlan()   {return m_nNativeVlan;}
     
    void SetVlanBegin(int32 value)      {m_nVlanBegin = value; return ;}
    void SetVlanEnd(int32 value)        {m_nVlanEnd = value; return ;}
    void SetNativeVlan(int32 value)    {m_nNativeVlan = value; return ;}

    void DbgShow()
    {   
        cout << "Trunk vlan info:{" << m_nVlanBegin 
             << "-" << m_nVlanEnd << "}, native vlan:"
             << m_nNativeVlan << "." << endl;
    }
private:    
    int32  m_nVlanBegin;
    int32  m_nVlanEnd;
    int32  m_nNativeVlan;
};


class CVNetDbIProcedure;
class CDBOperateNetplaneVlanPool : public  CVNetDbIProcedureCallback
{
public:
    explicit CDBOperateNetplaneVlanPool(CVNetDbIProcedure * pb);
    virtual ~CDBOperateNetplaneVlanPool();

    int32 Query(const char* uuid, vector<CDbNetplaneVlanPool> & outVInfo);  
    int32 QueryPrivateNetIsolateNo(const char* NetplaneUUID, vector<int32>& outVInfo);
    int32 QueryShareNetAccessVlan(const char* NetplaneUUID, vector<int32>& outVInfo);    
    int32 QueryShareNetTrunkVlan(const char* NetplaneUUID, vector<CDbNetplaneTrunkVlan>& outVInfo);
    int32 Add(CDbNetplaneVlanPool & info);
    int32 Del(CDbNetplaneVlanPool & info);

    int32 CheckAdd(CDbNetplaneVlanPool & info);
    int32 CheckDel(CDbNetplaneVlanPool & info);

    int32 DbProcCallback(int32 type, CADORecordset *, void* );

    void DbgShow();    
private:
    int32 QueryPrivateNetIsolateNoCallback(CADORecordset * prs,void * nil);
    int32 QueryShareNetAccessVlanCallback(CADORecordset * prs,void * nil);
    int32 QueryShareNetTrunkVlanCallback(CADORecordset * prs,void * nil);
    int32 QueryCallback(CADORecordset * prs,void * nil); 
    int32 OperateCallback(CADORecordset * prs,void * nil);
    int32 CheckOperateCallback(CADORecordset * prs,void * nil);
    
private:
    DISALLOW_COPY_AND_ASSIGN(CDBOperateNetplaneVlanPool);

    CVNetDbIProcedure * m_pIProc;
};
    
} // namespace zte_tecs

#endif // _VNET_DB_NETPLANE_VLANPOOL_INCLUDE_H_ 



