/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�dev_monitor.h
* �ļ���ʶ��
* ����ժҪ��dev��������Ķ����ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�
* ������ڣ�2013��1��16��
*
* �޸ļ�¼1��
*     �޸����ڣ�2013/1/16
*     �� �� �ţ�V1.0
*     �� �� �ˣ�lverchun
*     �޸����ݣ�����
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
@brief ���������������豸��ػ��࣬�����豸ͨ�õļ���඼���ԴӸ���̳�
@li @b ����˵������
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
@brief ���������������豸MTU����࣬���ڼ�������豸��MTU����
@li @b ����˵������
*/
class CDevMonMTU : public CNetDevVisitor
{
public:
    CDevMonMTU(){SetVisitorInfo("CDevMonMTU");};
    virtual ~CDevMonMTU(){};

    virtual void VisitDev(CNetDev *dev_entity);
};

/**
@brief ���������������豸��ػ��࣬�����豸ͨ�õļ���඼���ԴӸ���̳�
@li @b ����˵������
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
@brief ���������������豸LOOP����࣬���ڼ�������豸�Ļ�������
@li @b ����˵������
*/
class CPortMonLoop: public CPortVisitor
{
public:
    CPortMonLoop(){SetVisitorInfo("CPortMonLoop");};
    virtual ~CPortMonLoop(){};

    virtual void VisitPort(CPortNetDev *dev_entity);
};

/**
@brief �������������������豸�������Լ���࣬���ڼ���������ں����������Ź�ϵ�Ƿ�����
@li @b ����˵������
*/
class CPortMonBridge:public CPortVisitor
{
public:
    CPortMonBridge(){SetVisitorInfo("CPortMonBridge");};
    virtual ~CPortMonBridge(){};

    virtual void VisitPort(CPortNetDev *dev_entity);    
};


/**
@brief ���������������豸��ػ��࣬�����豸ͨ�õļ���඼���ԴӸ���̳�
@li @b ����˵������
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
@brief ���������������豸��ػ��࣬�����豸ͨ�õļ���඼���ԴӸ���̳�
@li @b ����˵������
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
@brief ���������������豸���ж˿ڼ���࣬���ڼ�������豸�����ж˿��Ƿ�����
@li @b ����˵������
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


