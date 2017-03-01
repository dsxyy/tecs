/*******************************************************************************
 * Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
 *
 * 文件名称：vnet_phyport_class.h
 * 文件标识：
 * 内容摘要：VNA 物理端口信息收集
 * 当前版本：V1.0
 * 作    者：liuhx
 * 完成日期：2013年2月5日
 *******************************************************************************/

#ifndef  VNET_PHYPORT_CLASS_H
#define  VNET_PHYPORT_CLASS_H

#include "vnet_portinfo.h"

// 禁用拷贝构造宏定义
#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName&); \
void operator=(const TypeName&)
#endif

using namespace std;
namespace zte_tecs
{
class CPhyPortClass:public CPortNetDev
{
public:
    CPhyPortClass()
    {
        m_strNicName = "";
        m_nPortType = 0;
        m_nState = 0;
        m_nIsBroadcast = 0;
        m_nIsRunning = 0;
        m_nMulticast = 0;
        m_nPromiscuous = 0;
        m_nMtu = 0;
        m_nIsMaster = 0;
        m_nAdminState = 0;
        m_strSupportedPorts = "";
        m_strSupportedLinkModes = "";
        m_nIsSupportAutoNegotiation = 0;
        m_strAdvertisedLinkModes = "";
        m_strAdvertisedPauseFrameUse = "";
        m_nIsAdvertisedAutoNegotiation = 0;
        m_strSpeed = "";
        m_strDuplex = "";
        m_strPort = "";
        m_strPhyad = "";
        m_strTransceiver = "";
        m_strAutoNegotiate = "";
        m_strSupportsWakeOn = "";
        m_strWakeOn = "";
        m_strCurrentMsgLevel = "";
        m_nIsLinked = 0;
        m_nIsRxChecksumming = 0;
        m_nIsTxChecksumming = 0;
        m_nIsScatterGather = 0;
        m_nIsTcpSegmentationOffload = 0;
        m_nIsUdpFragmentationOffload = 0;
        m_nGenericSegmentationOffload = 0;
        m_nIsGenericReceiveOffload = 0;
        m_nIsLargeReceiveOffload = 0;
        m_nIsSriov = 0;
        m_nIsLoopReport = 0;
        m_nTotalVfNum = 0;
        m_nFreeVfNum = 0;
    }

    virtual ~CPhyPortClass();

    void GetPortWorkPara(int32 fd);

    string GetNicName()
    {
        return m_strNicName;
    }

    string GetPortNegotiate()
    {
        return m_strAutoNegotiate;
    }

    string GetPortDuplex()
    {
        return m_strDuplex;
    }

    int32 GetPortMaster()
    {
        return m_nIsMaster;
    }

    int32 GetPortStatus()
    {
        return m_nState;
    }

    int32 GetPortLinked()
    {
        return m_nIsLinked;
    }

    string GetPortSpeed()
    {
        return m_strSpeed;
    }

    int32 GetLoopbackMode()
    {
        return m_nIsLoopReport;
    }

    int32 GetPortSriovNum()
    {
        return m_nTotalVfNum;
    }

    int32 GetPortMtu()
    {
        return m_nMtu;
    }

    int32 GetPortPromiscuous()
    {
        return m_nPromiscuous;
    }

    int32 GetPortType()
    {
        return m_nPortType;
    }

    int32 GetIsSriov()
    {
        return m_nIsSriov;
    }	

    void SetPortSriovNum(int32 dwPortSriovNum)
    {
        m_nTotalVfNum = dwPortSriovNum;
    }

    void SetNicName(const string &strNicName)
    {
        m_strNicName = strNicName;
    }

    int32 GetSriovVfPci(vector <CSriovPortReport> &vSriovInfo);

    int32 IsPciExit(const string &strPci, string &strVf);	

    static string strOfSpeed(int32 speed);
    static string strOfDuplex(int32 duplex);
    static string strOfMaster(int32 master);
    static string strOfStatus(int32 status);
    static string strOfLinked(int32 linked);
    static string strOfNegotiate(int32 negotiate);
    static string strOfConnector(int32 connector);
    static string strOfPromisc(int32 promisc);
    static string strOfUsed(int32 used);
    static int32 speedFromSys(__u16 sys);
    static int32 duplexFromSys(__u8 sys);
    static int32 autonegFromSys(__u8 sys);

    virtual void Accept(CNetDevVisitor *visitor){visitor->VisitDev(this);};
    virtual void Accept(CPortVisitor *visitor){visitor->VisitPort(this);};

    void DbgShow();

protected:

private:

    /* 基础属性*/
    string m_strNicName;
    int32  m_nPortType;
    int32  m_nState;

    int32  m_nIsBroadcast;
    int32  m_nIsRunning;
    int32  m_nMulticast;
    int32  m_nPromiscuous;
    int32  m_nMtu;
    int32  m_nIsMaster;
    int32  m_nAdminState;

    /* 暂未实现*/
    string m_strSupportedPorts;
    string m_strSupportedLinkModes;
    int32  m_nIsSupportAutoNegotiation;
    string m_strAdvertisedLinkModes;
    string m_strAdvertisedPauseFrameUse;
    int32  m_nIsAdvertisedAutoNegotiation;

    /* 基础属性*/
    string m_strSpeed;
    string m_strDuplex;

    /* 暂未实现*/
    string m_strPort;
    string m_strPhyad;
    string m_strTransceiver;
    string m_strAutoNegotiate;
    string m_strSupportsWakeOn;
    string m_strWakeOn;
    string m_strCurrentMsgLevel;

    /* 基础属性*/
    int32  m_nIsLinked;

    /* 扩展属性未实现*/
    int32 m_nIsRxChecksumming;
    int32 m_nIsTxChecksumming;
    int32 m_nIsScatterGather;
    int32 m_nIsTcpSegmentationOffload;
    int32 m_nIsUdpFragmentationOffload;
    int32 m_nGenericSegmentationOffload;
    int32 m_nIsGenericReceiveOffload;
    int32 m_nIsLargeReceiveOffload;

    /* SRIOV */
    int32 m_nIsSriov;
    int32 m_nIsLoopReport;
    int32 m_nTotalVfNum;
    int32 m_nFreeVfNum;

    map<string, string> m_mapVirtfnPci;

    DISALLOW_COPY_AND_ASSIGN(CPhyPortClass);
};
} // namespace zte_tecs

#endif

