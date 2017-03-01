/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：vna_switch_module.h
* 文件标识： 
* 内容摘要：交换相关操作 
* 当前版本：1.0
* 作    者：钟春山 
* 完成日期： 
*******************************************************************************/
#ifndef VNA_SWITCH_MODULE_H
#define VNA_SWITCH_MODULE_H

namespace zte_tecs
{

#define VNET_SRIOV_RANDOM_MAC  "02:10:10:10:10:10"

class CSwitchDev:public CSwitchNetDev
{
public:
    CSwitchDev();
    virtual ~CSwitchDev();

public:
    virtual int32 CreateSwitch(CSwitchCfgMsgToVNA &cSwitchCfg)=0;
    virtual int32 DeleteSwitch(CSwitchCfgMsgToVNA &cSwitchCfg)=0;
    virtual int32 DeleteSwitchPort(CSwitchCfgMsgToVNA &cSwitchCfg, vector<string> &vecNewData)= 0;
    virtual int32  SetSwitchAttr(CSwitchCfgMsgToVNA &cSwitchCfg)=0;
    virtual int32  IsSwitchValid(CVNetVnicDetail &cVnicDetail, BOOL bFlag) = 0;
    virtual void   Accept(CNetDevVisitor *visitor){visitor->VisitDev(this);};
    virtual void   Accept(CBridgeVisitor *visitor){visitor->VisitBridge(this);};
    virtual int32  SetSwitchSdnController(CSwitchCfgMsgToVNA &cSwitchCfg)=0;
    virtual int32  DelSwitchSdnController(CSwitchCfgMsgToVNA &cSwitchCfg)=0;
public:    
    int32 CreateBonding(CSwitchCfgMsgToVNA &cSwitchCfg);
    int32 CheckPortIsValid(CSwitchCfgMsgToVNA &cSwitchCfg);
    int32 AddBridgeNetIf(const string &strBridge, const string &strBrIf);
    int32 CreateBridge(const string &strBridge, const string &strMixBridge);
    int32 DeleteBridge(const string &strBridge);
    int32 ProcEvbPort(const string &strBridge, BOOL bFlag);
    int32 IsVtepPortConsistent(CSwitchCfgMsgToVNA &cSwitchCfg);

private:    
    int32 IsVectorEqua(vector<string> new_vec, vector<string> old_vec);
    int32 IsPortJoinBridge(const string &strBr, const string &strPort);
    int32 IsPortJoinBond(const string &strPort);
    int32 IsDvsConsistent(const string &strBridge, const string &strBond, 
            const string &strBondMode, const vector<string> &vecBondMem);
    
public:       
    void SetSwitchUuid(const string &strUuid)
    {
        m_strUuid = strUuid;
    }    
    void SetSwitchType(int32 nType)
    {
        m_nType = nType;
    }
    void SetSwitchState(bool bState)
    {
        m_bState = bState;
    }
    void SetDVSEvbMode(int32 nMode)
    {
        m_nEvbMode = nMode;
    }
    void SetDVSMaxVnic(int32 nVnicNums)
    {
        m_nMaxVnics = nVnicNums;
    }
    void SetDVSUplinkPort(vector<string> &vecPort)
    {
        if(!m_vecPort.empty())
        {
            m_vecPort.clear();
        }
        
        vector<string>::iterator itr = vecPort.begin();
        for( ; itr != vecPort.end(); ++itr)
        {
            m_vecPort.push_back(*itr);
        }
    }  
    //增加一个端口
    void SetDVSUplinkPort(string &port)
    {
        vector<string>::iterator itr = m_vecPort.begin();   
        for( ; itr != m_vecPort.end(); ++itr)
        {
            if(0 == port.compare(*itr))
            {
                return;
            }
        }
        m_vecPort.push_back(port);
    }
    void SetDVSBondName(const string &strBondName)
    {
        m_strBondName = strBondName;
    }
    void SetDVSBondMode(const int32 nBondMode)
    {
        m_nBondMode = nBondMode;
    }
    void SetDVSInterface(const string &strSwIf)
    {
        m_strSwInf = strSwIf;
    }
    
    string& GetSwitchUuid()
    {
        return m_strUuid;
    }
    int32 GetSwitchType()
    {
        return m_nType;
    }
   
    bool GetSwitchState()
    {
        return m_bState;
    }
    string& GetDVSBondName()
    {
        return m_strBondName;
    }
    int32 GetDVSBondMode()
    {
        return m_nBondMode;
    }
    int32 GetDVSEvbMode()
    {
        return m_nEvbMode;
    }
    int32 GetDVSMaxVnic()
    {
        return m_nMaxVnics;
    }    
    void GetDVSUplinkPort(vector<string> &vecPort)
    {
        vector<string>::iterator itr = m_vecPort.begin();
        for( ; itr != m_vecPort.end(); ++itr)
        {
            vecPort.push_back(*itr);
        }        
    }
    string& GetDVSInterface()
    {
        return m_strSwInf;
    }
    string& GetDVSVtepPort()
    {
        return m_strVtepPort;
    }
    void SetDVSVtepPort(const string &strVtep)
    {
        m_strVtepPort = strVtep;
    }
    
    string& GetDVSVtepPortIP()
    {
        return m_strVtepIP;
    }
    void SetDVSVtepPortIP(const string &strVtepIP)
    {
        m_strVtepIP = strVtepIP;
    }  
    
    string& GetDVSVtepPortMask()
    {
        return m_strVtepMask;
    }
    void SetDVSVtepPortMask(const string &strVtepMask)
    {
        m_strVtepMask = strVtepMask;
    }
    
    int32 GetDVSNativeVlan()
    {
        return m_nNativeVlan;
    }
    void SetDVSNativeVlan(const int32 nNativeVlan)
    {
        m_nNativeVlan = nNativeVlan;
    }
    
protected:
    string m_strUuid;
    int32  m_nType;
    BOOL   m_bState;
    string m_strBondName;
    int32  m_nBondMode;
    int32  m_nEvbMode;
    int32  m_nMaxVnics;
    vector<string> m_vecPort;
    string m_strSwInf;
    string m_strVtepPort;
    string m_strVtepIP;
    string m_strVtepMask;
    int32  m_nNativeVlan;
    
private:
    DISALLOW_COPY_AND_ASSIGN(CSwitchDev);
};

class CSoftDVSDev : public CSwitchDev
{

public:
    CSoftDVSDev(){};
    virtual ~CSoftDVSDev(){};
    int32 CreateSwitch(CSwitchCfgMsgToVNA &cSwitchCfg);
    int32  DeleteSwitch(CSwitchCfgMsgToVNA &cSwitchCfg);
    int32  DeleteSwitchPort(CSwitchCfgMsgToVNA &cSwitchCfg, vector<string> &vecNewData);
    int32  SetSwitchAttr(CSwitchCfgMsgToVNA &cSwitchCfg);
    int32  IsSwitchValid(CVNetVnicDetail &cVnicDetail, BOOL bFlag);
    void   GetSoftDVS(vector<CSwitchReportInfo> &vecDVSReport);
    int32  SetSwitchSdnController(CSwitchCfgMsgToVNA &cSwitchCfg);
    int32  DelSwitchSdnController(CSwitchCfgMsgToVNA &cSwitchCfg);
    void   DbgSdnController(const string& strBr, const int32 nOper);
    virtual void Accept(CNetDevVisitor *visitor){visitor->VisitDev(this);};
    virtual void Accept(CBridgeVisitor *visitor){visitor->VisitBridge(this);};
      
private:
    DISALLOW_COPY_AND_ASSIGN(CSoftDVSDev);
};

class CEmbDVSDev : public CSwitchDev
{

public:
    CEmbDVSDev(){};
    virtual ~CEmbDVSDev(){};
    int32 CreateSwitch(CSwitchCfgMsgToVNA &cSwitchCfg);
    int32  DeleteSwitch(CSwitchCfgMsgToVNA &cSwitchCfg);
    int32  DeleteSwitchPort(CSwitchCfgMsgToVNA &cSwitchCfg, vector<string> &vecNewData);
    int32  SetSwitchAttr(CSwitchCfgMsgToVNA &cSwitchCfg);
    int32  IsSwitchValid(CVNetVnicDetail &cVnicDetail, BOOL bFlag);
    int32  SetSwitchSdnController(CSwitchCfgMsgToVNA &cSwitchCfg);
    int32  DelSwitchSdnController(CSwitchCfgMsgToVNA &cSwitchCfg);
    virtual void Accept(CNetDevVisitor *visitor){visitor->VisitDev(this);};
    virtual void Accept(CBridgeVisitor *visitor){visitor->VisitBridge(this);};

private:
    DISALLOW_COPY_AND_ASSIGN(CEmbDVSDev);
};

}
#endif

