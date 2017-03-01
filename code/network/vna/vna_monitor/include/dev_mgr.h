/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：dev_mgr.h
* 文件标识：
* 内容摘要：dev相关类的定义文件
* 当前版本：1.0
* 作    者：
* 完成日期：2013年1月16日
*
* 修改记录1：
*     修改日期：2013/1/16
*     版 本 号：V1.0
*     修 改 人：lverchun
*     修改内容：创建
******************************************************************************/
#ifndef DEV_MANAGER_INCLUDE_H__
#define DEV_MANAGER_INCLUDE_H__

namespace zte_tecs
{


class CNetDev
{
public:
    CNetDev()
    {
        m_nDevType         = 0;             
        m_nMTU             = VNET_COMMON_MTU;   
        m_nIPAddress       = 0;           
        m_nFlag            = 0;                
        m_nRcvPackStatics  = 0;     
        m_nSendPackStatics = 0;    
    };
    virtual ~CNetDev(){};

    virtual void Enable(){};
    virtual void Disable(){};
    virtual int GetDevMTU(){return m_nMTU;};
    virtual void SetDevMTU(int nMTU)
    {
        if (m_nMTU != nMTU)
        {
            if(0 == set_mtu(m_strDevName.c_str(), nMTU)) //先设置网口mtu
            {
                m_nMTU = nMTU;
            }
        }
    };
    virtual string GetMacAdd(){return m_strMacAddress;};
    virtual void SetMacAdd(string strMacAddress){m_strMacAddress = strMacAddress;};
    virtual string GetDevName(){return m_strDevName;};
    virtual void SetDevName(string strDevName){m_strDevName = strDevName;};

    virtual void Accept(CNetDevVisitor *visitor){visitor->VisitDev(this);};

private:
    string m_strDevName;         //设备名称
    int m_nDevType;                 //标识是ethernet或者是ATM等
    int m_nMTU;                         //设备MTU值
    int32 m_nIPAddress;           //设备IP地址
    string m_strMacAddress;                 //设备MAC地址
    int32 m_nFlag;                     //标识广播、组播、混杂等标记位
    int64 m_nRcvPackStatics;    //收报计数
    int64 m_nSendPackStatics;  //发报计数
};


class CPortNetDev:public CNetDev
{
public:
    CPortNetDev(){};
    virtual ~CPortNetDev(){};

    virtual void Accept(CNetDevVisitor *visitor){visitor->VisitDev(this);};
    virtual void Accept(CPortVisitor *visitor){visitor->VisitPort(this);};
    
private:

};


class CPhyPortNetDev:public CPortNetDev
{
public:
    CPhyPortNetDev(){};
    virtual ~CPhyPortNetDev(){};
    
    virtual void Accept(CNetDevVisitor *visitor){visitor->VisitDev(this);};
    virtual void Accept(CPortVisitor *visitor){visitor->VisitPort(this);};

private:
    vector<string> portcfg_list;   //用于保存VNM下发的配置信息
    vector<string> portreport_list;   //用于保存收集的实际端口信息

};


class CBondPortNetDev:public CPortNetDev
{
public:
    CBondPortNetDev(){};
    virtual ~CBondPortNetDev(){};
    
    virtual void Accept(CNetDevVisitor *visitor){visitor->VisitDev(this);};
    virtual void Accept(CPortVisitor *visitor){visitor->VisitPort(this);};

};


class CSRPortNetDev:public CPortNetDev
{
public:
    CSRPortNetDev(){};
    virtual ~CSRPortNetDev(){};
    
    virtual void Accept(CNetDevVisitor *visitor){visitor->VisitDev(this);};
    virtual void Accept(CPortVisitor *visitor){visitor->VisitPort(this);};

};


class CVNicNetDev:public CPortNetDev
{
public:
    CVNicNetDev(){};
    virtual ~CVNicNetDev(){};

private:
     string m_strVSIID;
     //CLogicNet  m_cLogicNet;
     string m_strBridgeName;
     string m_strPCIOrder;
};


class CVSVNicNetDev:public CVNicNetDev
{
public:
    CVSVNicNetDev(){};
    virtual ~CVSVNicNetDev(){};

    
    virtual void Accept(CNetDevVisitor *visitor){visitor->VisitDev(this);};
    virtual void Accept(CPortVisitor *visitor){visitor->VisitPort(this);};

};

class CESVNicNetDev:public CVNicNetDev
{
public:
    CESVNicNetDev(){};
    virtual ~CESVNicNetDev(){};
    void Accept(CNetDevVisitor &visitor);

};

class CSwitchNetDev:public CNetDev
{
public:
    CSwitchNetDev(){};
    virtual ~CSwitchNetDev(){};

    virtual void Accept(CNetDevVisitor *visitor){visitor->VisitDev(this);};
    virtual void Accept(CBridgeVisitor *visitor){};

private:

};

class CPhysicalNetDev:public CSwitchNetDev
{
public:
    CPhysicalNetDev(){};
    virtual ~CPhysicalNetDev(){};

    virtual void Accept(CNetDevVisitor *visitor){};
    virtual void Accept(CBridgeVisitor *visitor){};

private:

};

#if 0
class CDVSNetDev:public CSwitchNetDev
{
public:
    CDVSNetDev(){};
    virtual ~CDVSNetDev(){};

    virtual void Accept(CNetDevVisitor *visitor){visitor->VisitDev(this);};
    virtual void Accept(CBridgeVisitor *visitor){visitor->VisitBridge(this);};
};


class CSDVSNetDev:public CDVSNetDev
{
public:
    CSDVSNetDev(){};
    virtual ~CSDVSNetDev(){};
    
    virtual void Accept(CNetDevVisitor *visitor){visitor->VisitDev(this);};
    virtual void Accept(CBridgeVisitor *visitor){visitor->VisitBridge(this);};
};

class CEDVSNetDev:public CDVSNetDev
{
public:
    CEDVSNetDev(){};
    virtual ~CEDVSNetDev(){};
    
    virtual void Accept(CNetDevVisitor *visitor){visitor->VisitDev(this);};
    virtual void Accept(CBridgeVisitor *visitor){visitor->VisitBridge(this);};

};
#endif

class CNetDevMgr
{
public:
    CNetDevMgr(){};
    virtual ~CNetDevMgr(){};

    virtual void DoMonitor(CNetDevVisitor *dev_visit);
    virtual void DoMonitor(CPortVisitor *dev_visit);
    virtual void DoMonitor(CBridgeVisitor *dev_visit);
    virtual void DoMonitor(CVnicVisitor *dev_visit);
    void SetMgrInfo(string strMgrInfo){m_strMgrInfo = strMgrInfo;};
    string GetMgrInfo(){return m_strMgrInfo;};
    
private:
    string m_strMgrInfo;
};


class CPortManager:public CNetDevMgr
{
public:
    CPortManager(){SetMgrInfo("CPortManager");};
    virtual ~CPortManager(){};

    static CPortManager *GetInstance()
    {
        if (NULL == s_pInstance)
        {
            s_pInstance = new CPortManager();
        }

        return s_pInstance;
    };
    
    void GetReportInfo();
    void DoMonitor(CNetDevVisitor *dev_visit);
    void DoMonitor(CPortVisitor *dev_visit);
    
    void AddPort(CPortNetDev *pNetDev){    m_mapPortDev.insert(pair<string, CPortNetDev*> (pNetDev->GetDevName(), pNetDev));};

private:
    static CPortManager *s_pInstance;

    map<string, CPortNetDev*> m_mapPortDev;
    DISALLOW_COPY_AND_ASSIGN(CPortManager);
};

#if 0
class CBridgeManager:public CNetDevMgr
{
public:
    CBridgeManager(){};
    virtual ~CBridgeManager(){};

    static CBridgeManager *GetInstance()
    {
        if (NULL == s_pInstance)
        {
            s_pInstance = new CBridgeManager();
        }

        return s_pInstance;
    };
    
    void GetReportInfo();
    void DoMonitor(CNetDevVisitor *dev_visit);
    void DoMonitor(CBridgeVisitor *dev_visit);
    void AddBridge(CDVSNetDev *pNetDev){ m_mapBridgeDev.insert(pair<string, CDVSNetDev*> (pNetDev->GetDevName(), pNetDev));};

private:
    static CBridgeManager *s_pInstance;
    map<string, CDVSNetDev*> m_mapBridgeDev;
    DISALLOW_COPY_AND_ASSIGN(CBridgeManager);
};
#endif
}// namespace zte_tecs

#endif

