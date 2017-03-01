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

#if !defined(_VNET_DB_PORT_PHYSICAL_INCLUDE_H_)
#define _VNET_DB_PORT_PHYSICAL_INCLUDE_H_

#include "vnet_db_in.h"
#include "vnet_db_interface.h"
#include "vnet_db_proc.h"

#include "vnet_db_port.h"

namespace zte_tecs
{

// port db


class CDbPortPhysical : public  CDbPort
{
public: 
    CDbPortPhysical() 
    {
        m_strSupportedPorts = m_strSupportedLinkModes = m_strAdvLinkModes = m_strAdvPauseFromeUse = "";
        m_strSpeed = m_strDuplex = m_strPort = m_strPhyad = m_strTransceiver = m_strAutoNeg = "";
        m_strSupportWakeOn = m_strWakeOn = m_strCntMsgLevel = "";
        m_nIsSupportAutoNeg = m_nIsAdvAutoNeg = m_nIsLinked = 0;

        m_nIsRxCheckSum= m_nIsTxCheckSum = m_nIsScatterGather = m_nIsTcpSegOffload = 0;
        m_nIsUdpFragOffload = m_nIsGenSegOffload = m_nIsGenRecOffload = m_nIsLargeRecOffload = 0;

        m_nIsSriov = m_nIsloopRep = m_nIsloopCfg = 0;
     };
    virtual ~CDbPortPhysical() {};
    
    string & GetSupportedPorts(){return m_strSupportedPorts;};
    string & GetSupportedLinkModes() {return m_strSupportedLinkModes;};     
    int32 GetIsSupportAutoNeg(){return m_nIsSupportAutoNeg;};
    string & GetAdvLinkModes(){return m_strAdvLinkModes;};     
    string & GetAdvPauseFromeUse(){return m_strAdvPauseFromeUse;};   
    int32 GetIsAdvAutoNeg(){return m_nIsAdvAutoNeg;};  
    string & GetSpeed(){return m_strSpeed;};     
    string & GetDuplex(){return m_strDuplex;};
    string & GetPort(){return m_strPort;};
    string & GetPhyad(){return m_strPhyad;};
    string & GetTransceiver(){return m_strTransceiver;};    
    string & GetAutoNeg(){return m_strAutoNeg;};    
    string & GetSupportWakeOn(){return m_strSupportWakeOn;};
    string & GetWakeOn(){return m_strWakeOn;};
    string & GetCntMsgLevel(){return m_strCntMsgLevel;}; 
    int32 GetIsLinked(){return m_nIsLinked;};

    int32 GetIsRxCheckSum(){return m_nIsRxCheckSum;};
    int32 GetIsTxCheckSum(){return m_nIsTxCheckSum;};
    int32 GetIsScatterGather(){return m_nIsScatterGather;};
    int32 GetIsTcpSegOffload(){return m_nIsTcpSegOffload;};
    int32 GetIsUdpFragOffload(){return m_nIsUdpFragOffload;};
    int32 GetIsGenSegOffload(){return m_nIsGenSegOffload;};
    int32 GetIsGenRecOffload(){return m_nIsGenRecOffload;};
    int32 GetIsLargeRecOffload(){return m_nIsLargeRecOffload;};
      
    int32 GetIsSriov(){return m_nIsSriov;};
    int32 GetIsloopRep(){return m_nIsloopRep;};
    int32 GetIsloopCfg(){return m_nIsloopCfg;};            

    void SetSupportedPorts(const char* value) {m_strSupportedPorts = value;};
    void SetSupportedLinkModes(const char* value) {m_strSupportedLinkModes = value;};     
    void SetIsSupportAutoNeg(int32 value) {m_nIsSupportAutoNeg = value;};   
    void SetAdvLinkModes(const char* value) {m_strAdvLinkModes = value;};
    void SetAdvPauseFromeUse(const char* value) {m_strAdvPauseFromeUse = value;};     
    void SetIsAdvAutoNeg(int32 value) {m_nIsAdvAutoNeg = value;};   
    void SetSpeed(const char* value) {m_strSpeed = value;};
    void SetDuplex(const char* value) {m_strDuplex = value;}; 
    void SetPort(const char* value) {m_strPort = value;};
    void SetPhyad(const char* value) {m_strPhyad = value;}; 
    void SetTransceiver(const char* value) {m_strTransceiver = value;};
    void SetAutoNeg(const char* value) {m_strAutoNeg = value;}; 
    void SetSupportWakeOn(const char* value) {m_strSupportWakeOn = value;};
    void SetWakeOn(const char* value) {m_strWakeOn = value;}; 
    void SetCntMsgLevel(const char* value) {m_strCntMsgLevel = value;};  
    void SetIsLinked(int32 value) {m_nIsLinked = value;};   
    
    void SetIsRxCheckSum(int32 value) {m_nIsRxCheckSum = value;};   
    void SetIsTxCheckSum(int32 value) {m_nIsTxCheckSum = value;};   
    void SetIsScatterGather(int32 value) {m_nIsScatterGather = value;};   
    void SetIsTcpSegOffload(int32 value) {m_nIsTcpSegOffload = value;};   
    void SetIsUdpFragOffload(int32 value) {m_nIsUdpFragOffload = value;};   
    void SetIsGenSegOffload(int32 value) {m_nIsGenSegOffload = value;};   
    void SetIsGenRecOffload(int32 value) {m_nIsGenRecOffload = value;};  
    void SetIsLargeRecOffload(int32 value) {m_nIsLargeRecOffload = value;};   

    void SetIsSriov(int32 value) {m_nIsSriov = value;};   
    void SetIsloopRep(int32 value) {m_nIsloopRep = value;};   
    void SetIsloopCfg(int32 value) {m_nIsloopCfg = value;};   

    void DbgShow()
    {
        DbgPortShow();
        cout << "SupportedPorts    : " << m_strSupportedPorts << endl;
        cout << "SupportedLinkModes: " << m_strSupportedLinkModes << endl;
        cout << "IsSupportAutoNeg  : " << m_nIsSupportAutoNeg << endl;       
        cout << "AdvLinkModes      : " << m_strAdvLinkModes << endl;
        cout << "AdvPauseFromeUse  : " << m_strAdvPauseFromeUse << endl;
        cout << "IsAdvAutoNeg      : " << m_nIsAdvAutoNeg << endl;
        cout << "Speed             : " << m_strSpeed << endl;
        cout << "Duplex            : " << m_strDuplex << endl;
        cout << "Port              : " << m_strPort << endl;       
        cout << "Phyad             : " << m_strPhyad << endl;
        cout << "Transceiver       : " << m_strTransceiver << endl;
        cout << "AutoNeg           : " << m_strAutoNeg << endl;        
        cout << "SupportWakeOn     : " << m_strSupportWakeOn << endl; 
        cout << "WakeOn            : " << m_strWakeOn << endl;         
        cout << "CntMsgLevel       : " << m_strCntMsgLevel << endl;  
        cout << "IsLinked          : " << m_nIsLinked << endl;   

        cout << "IsRxCheckSum     : " << m_nIsRxCheckSum << endl;  
        cout << "IsTxCheckSum     : " << m_nIsTxCheckSum << endl;  
        cout << "IsScatterGather  : " << m_nIsScatterGather << endl;  
        cout << "IsTcpSegOffload  : " << m_nIsTcpSegOffload << endl;  
        cout << "IsUdpFragOffload : " << m_nIsUdpFragOffload << endl;  
        cout << "IsGenSegOffload  : " << m_nIsGenSegOffload << endl;  
        cout << "IsGenRecOffload  : " << m_nIsGenRecOffload << endl;  
        cout << "IsLargeRecOffload: " << m_nIsLargeRecOffload << endl;  
        
        cout << "IsSriov          : " << m_nIsSriov << endl;  
        cout << "IsloopRep        : " << m_nIsloopRep << endl;  
        cout << "IsloopCfg        : " << m_nIsloopCfg << endl;   
    }
private:    
    string m_strSupportedPorts;
    string m_strSupportedLinkModes;
    int32 m_nIsSupportAutoNeg;
    string m_strAdvLinkModes;
    string m_strAdvPauseFromeUse;
    int32 m_nIsAdvAutoNeg;
    string m_strSpeed;
    string m_strDuplex;
    string m_strPort;
    string m_strPhyad;
    string m_strTransceiver;
    string m_strAutoNeg;
    string m_strSupportWakeOn;
    string m_strWakeOn;
    string m_strCntMsgLevel;
    int32 m_nIsLinked;

    int32 m_nIsRxCheckSum;
    int32 m_nIsTxCheckSum;
    int32 m_nIsScatterGather;
    int32 m_nIsTcpSegOffload;
    int32 m_nIsUdpFragOffload;
    int32 m_nIsGenSegOffload;
    int32 m_nIsGenRecOffload;
    int32 m_nIsLargeRecOffload;
    
    int32 m_nIsSriov;
    int32 m_nIsloopRep;
    int32 m_nIsloopCfg;    
};

// sriov port 下部署的虚拟网口数目
class CDbSriovPortVsiNum
{
public: 
    CDbSriovPortVsiNum() 
    {
        m_nRet = m_nVsiNum = 0;
     };
    virtual ~CDbSriovPortVsiNum() {};
    
    int32 GetRet(){return m_nRet;}; 
    int32 GetVsiNum(){return m_nVsiNum;}; 

    void SetRet(int32 value) {m_nRet = value;};   
    void SetVsiNum(int32 value) {m_nVsiNum = value;};   
private:
    int32 m_nRet;
    int32 m_nVsiNum;

};

class CVNetDbIProcedure;
class CDBOperatePortPhysical : public  CVNetDbIProcedureCallback, public CDBOperatePortBase
{
public:
    explicit CDBOperatePortPhysical(CVNetDbIProcedure * pb);
    virtual ~CDBOperatePortPhysical();

    int32 QueryAllSummary(const char* vna_uuid, vector<CDbPortSummary> & outVInfo);
    int32 QuerySummary(const char* vna_uuid,int32 is_sriov, vector<CDbPortSummary> & outVInfo);  
    int32 Query(CDbPortPhysical & Info);
    int32 Add(CDbPortPhysical & info);
    int32 Modify(CDbPortPhysical & info);
    int32 Del(const char* port_uuid);

    //查询SRIOV网口下 部署的虚拟机网卡数目
    int32 QueryVsiNum( const char* sriovport_uuid, int32 & outVsiNum ); 
    
    int32 CheckSetSriovLoop( const char* sriovport_uuid, int32 bIsloop = TRUE ); 
    
    int32 DbProcCallback(int32 type, CADORecordset *, void* );

    void DbgShow();    
private:
    int32 QueryVsiNumCallback(CADORecordset * prs,void * nil);
    int32 QuerySummaryCallback(CADORecordset * prs,void * nil);
    int32 QueryCallback(CADORecordset * prs,void * nil);
    int32 OperateCallback(CADORecordset * prs,void * nil);
    
    int32 CheckOperateCallback(CADORecordset * prs,void * nil);    
    
private:
    DISALLOW_COPY_AND_ASSIGN(CDBOperatePortPhysical);

    CVNetDbIProcedure * m_pIProc;
};
    
} // namespace zte_tecs

#endif // _VNET_DB_PORT_PHYSICAL_INCLUDE_H_ 


