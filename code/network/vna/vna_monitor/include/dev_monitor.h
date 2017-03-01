/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：dev_monitor.h
* 文件标识：
* 内容摘要：dev监控相关类的定义文件
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
#ifndef DEV_MONITOR_INCLUDE_H__
#define DEV_MONITOR_INCLUDE_H__

namespace zte_tecs
{
class CNetDev;
//class CDVSNetDev;
class CPortNetDev;
class CNetDevMgr;
class CSwitchDev;
class CVnicPortDev;

/**
@brief 功能描述：网络设备监控基类，网络设备通用的监控类都可以从该类继承
@li @b 其它说明：无
*/

class CNetDevVisitor
{
public:
    CNetDevVisitor(){SetVisitorInfo("CNetDevVisitor");};
    virtual ~CNetDevVisitor(){};

    virtual void VisitDev(CNetDev *dev_entity){    cout << "enter CNetDevVisitor::VisitDev" <<endl;};
    void SetVisitorInfo(string strVisitorInfo){m_strVisitorInfo = strVisitorInfo;};
    string GetVisitorInfo(){return m_strVisitorInfo;};

private:
    string m_strVisitorInfo;
};

/**
@brief 功能描述：网络设备MTU监控类，用于监控网络设备的MTU属性
@li @b 其它说明：无
*/
class CDevMonMTU : public CNetDevVisitor
{
public:
    CDevMonMTU(){SetVisitorInfo("CDevMonMTU");};
    virtual ~CDevMonMTU(){};

    virtual void VisitDev(CNetDev *dev_entity);
};

/**
@brief 功能描述：网口设备监控基类，网口设备通用的监控类都可以从该类继承
@li @b 其它说明：无
*/
class CPortVisitor
{
public:
    CPortVisitor(){SetVisitorInfo("CPortVisitor");};
    virtual ~CPortVisitor(){};

    virtual void VisitPort(CPortNetDev *dev_entity){    cout << "enter CPortVisitor::VisitPort" <<endl;};
    void SetVisitorInfo(string strVisitorInfo){m_strVisitorInfo = strVisitorInfo;};
    string GetVisitorInfo(){return m_strVisitorInfo;};

private:
    string m_strVisitorInfo;
};

/**
@brief 功能描述：网络设备LOOP监控类，用于监控网络设备的环回属性
@li @b 其它说明：无
*/
class CPortMonLoop: public CPortVisitor
{
public:
    CPortMonLoop(){SetVisitorInfo("CPortMonLoop");};
    virtual ~CPortMonLoop(){};

    virtual void VisitPort(CPortNetDev *dev_entity);
};

/**
@brief 功能描述：虚拟网口设备网桥属性监控类，用于监控虚拟网口和所属的网桥关系是否正常
@li @b 其它说明：无
*/
class CPortMonBridge:public CPortVisitor
{
public:
    CPortMonBridge(){SetVisitorInfo("CPortMonBridge");};
    virtual ~CPortMonBridge(){};

    virtual void VisitPort(CPortNetDev *dev_entity);    
};


/**
@brief 功能描述：网口设备监控基类，网口设备通用的监控类都可以从该类继承
@li @b 其它说明：无
*/
class CVnicVisitor
{
public:
    CVnicVisitor(){SetVisitorInfo("CVnicVisitor");};
    virtual ~CVnicVisitor(){};

    virtual void VisitPort(CVnicPortDev *dev_entity);
    void SetVisitorInfo(string strVisitorInfo){m_strVisitorInfo = strVisitorInfo;};
    string GetVisitorInfo(){return m_strVisitorInfo;};

private:
    string m_strVisitorInfo;
};

/**
@brief 功能描述：网桥设备监控基类，网桥设备通用的监控类都可以从该类继承
@li @b 其它说明：无
*/
class CBridgeVisitor
{
public:
    CBridgeVisitor(){SetVisitorInfo("CBridgeVisitor");};
    virtual ~CBridgeVisitor(){};

    virtual void VisitBridge(CSwitchDev *dev_entity){    cout << "enter CBridgeVisitor::VisitBridge" <<endl;};
    void SetVisitorInfo(string strVisitorInfo){m_strVisitorInfo = strVisitorInfo;};
    string GetVisitorInfo(){return m_strVisitorInfo;};

private:
    string m_strVisitorInfo;
};

/**
@brief 功能描述：网桥设备上行端口监控类，用于监控网桥设备的上行端口是否正常
@li @b 其它说明：无
*/
class CBridgeMonUplink:public CBridgeVisitor
{
public:
    CBridgeMonUplink(){SetVisitorInfo("CBridgeMonUplink");};
    virtual ~CBridgeMonUplink(){};

    virtual void VisitBridge(CSwitchDev *dev_entity);    
};

}
#endif // #ifndef DEV_MONITOR_INCLUDE_H__


