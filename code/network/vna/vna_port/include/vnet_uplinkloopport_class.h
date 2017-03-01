/*******************************************************************************
 * Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
 *
 * �ļ����ƣ�vnet_uplinkloopport_class.h
 * �ļ���ʶ��
 * ����ժҪ��VNA ���л��ض˿���Ϣ�ռ�
 * ��ǰ�汾��V1.0
 * ��    �ߣ�liuhx
 * ������ڣ�2013��3��7��
 *******************************************************************************/

#ifndef  VNET_UPLINKLOOPPORT_CLASS_H
#define  VNET_UPLINKLOOPPORT_CLASS_H

#include "vnet_portinfo.h"

// ���ÿ�������궨��
#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName&); \
void operator=(const TypeName&)
#endif

using namespace std;
namespace zte_tecs
{
class CUplinkLoopPortClass
{
public:
    CUplinkLoopPortClass()
    {
        m_nPortType = 0;
        m_strName = "";
        m_nState = 0;
        m_nIsBroadcast = 0;
        m_nIsRunning = 0;
        m_nIsMulticast = 0;
        m_nPromiscuous = 0;
        m_nMtu = 0;
        m_nIsMaster = 0;
        m_nAdminState = 0;
        m_nIsOnline = 0;
    }

    virtual ~CUplinkLoopPortClass();

    void GetPortWorkPara(int fd);

    void SetNicName(const string &strNicName)
    {
        m_strName = strNicName;
    }

    string GetNicName()
    {
        return m_strName;
    }

    int32 GetPortMaster()
    {
        return m_nIsMaster;
    }

    int32 GetPortStatus()
    {
        return m_nState;
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

    void DbgShow();

protected:

private:

    int32               m_nPortType;
    string              m_strName;
    int32               m_nState;
    int32               m_nIsBroadcast;
    int32               m_nIsRunning;
    int32               m_nIsMulticast;
    int32               m_nPromiscuous;
    int32               m_nMtu;
    int32               m_nIsMaster;
    int32               m_nAdminState;
    int32               m_nIsOnline;

    DISALLOW_COPY_AND_ASSIGN(CUplinkLoopPortClass);
};
} // namespace zte_tecs

#endif

