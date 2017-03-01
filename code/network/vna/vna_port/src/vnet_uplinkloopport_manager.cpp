/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�vnet_phyport_manager.cpp
* �ļ���ʶ��
* ����ժҪ������˿���ʵ���ļ�
* ��ǰ�汾��V1.0
* ��    �ߣ�liuhx
* ������ڣ�2013��2��17��
*******************************************************************************/
#include "vnet_comm.h"
#include "vnet_function.h"
#include "vnet_uplinkloopport_manager.h"

using namespace std;
using namespace _GLIBCXX_STD;

namespace zte_tecs
{

//��ȡʵ��
CUplinkLoopPortManager *CUplinkLoopPortManager::m_instance = NULL;
CUplinkLoopPortManager *CUplinkLoopPortManager::GetInstance()
{
    if (m_instance == NULL)
    {
        m_instance = new CUplinkLoopPortManager();
    }
    return m_instance;
}

CUplinkLoopPortManager::~CUplinkLoopPortManager()
{
    m_instance = NULL;

    map<string,CUplinkLoopPortClass*>::iterator it = m_mapPorts.begin();
    for (;it != m_mapPorts.end();)
    {
        CUplinkLoopPortClass *port = static_cast<CUplinkLoopPortClass *>(it->second);
        if (NULL != port)
        {
            delete port;
        }
        m_mapPorts.erase(it++);
    }
}

//ʵ�ʵ���PortClass��ȡ�˿ڸ���������Ϣ
void CUplinkLoopPortManager::GetPortWorkPara()
{
    int fd = 0;
    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        //VNET_ASSERT(0);
        VNET_LOG(VNET_LOG_ERROR, "CUplinkLoopPortManager: [%d] GetPortWorkPara failed.\n", fd);
    }
    else
    {
        map<string, CUplinkLoopPortClass*>::iterator iter;
        for (iter = m_mapPorts.begin(); iter != m_mapPorts.end(); ++iter)
        {
            CUplinkLoopPortClass *ptemp;
            ptemp = iter->second;
            if (NULL == ptemp)
            {
                continue;
            }
            //ʵ��ִ��PortClass��ȡ�˿ڸ���������Ϣ
            ptemp->GetPortWorkPara(fd);
        }
        close(fd);
    }
}

//��ȡ�˿���Ϣ
int32 CUplinkLoopPortManager::GetUplinkLoopPortInfo(vector <CUplinkLoopPortReport> &portInfo)
{
    //Ŀǰ���úͲ�ѯû���ǻ���
    //MutexLock lock(_mutex);

    //�����
    portInfo.clear();

    //��ɨ�����е�����˿ں�
    LookupUplinkLoopPort();

    if (m_mapPorts.empty()) return 0;

    map<string, CUplinkLoopPortClass*>::const_iterator it = m_mapPorts.begin();
    for (; it!= m_mapPorts.end(); ++it)
    {
        CUplinkLoopPortClass *ptemp = it->second;
        if (ptemp == NULL)
        {
            continue;
        }

        CUplinkLoopPortReport portinfo;
        //���һ�ȡ��Щ����ֵ�������
        portinfo._basic_info._name    = ptemp->GetNicName();
        //portinfo._basic_info._is_master = ptemp->GetPortMaster();
        //portinfo._basic_info._state = ptemp->GetPortStatus();
        //portinfo._basic_info._mtu = ptemp->GetPortMtu();
        //portinfo._basic_info._port_type = ptemp->GetPortType();
        //portinfo._basic_info._promiscuous = ptemp->GetPortPromiscuous();

        portInfo.push_back(portinfo);
    }
    return 0;
}

//ɨ������uplink loop port
void CUplinkLoopPortManager::LookupUplinkLoopPort()
{
    vector<string> nics;
#if 0
    //����������nics
    vector<string> bridges;
    get_bridges(bridges);
    vector<string> ifs;
    vector<string>::iterator it_bridge=bridges.begin();
    for (; it_bridge!= bridges.end(); ++it_bridge )
    {
        //�ж��ŵ�����
        if (((*it_bridge).compare("loop")) > 0)
        {
            get_ifs_of_bridge(*it_bridge,ifs);
            vector<string>::iterator it_ifs=ifs.begin();
            for (; it_ifs!= ifs.end(); ++it_ifs )
            {
                //����it_ifs��Ҫ���룬��ͬһ��br���Ƿ�������ֳ�ͻ?
                nics.push_back(*it_ifs);

                if (m_mapPorts.find(*it_ifs) != m_mapPorts.end())
                {
                    continue;
                }

                CUplinkLoopPortClass *port = NULL;
                if (NULL != (port = new CUplinkLoopPortClass()))
                {
                    //�����µ�����˿�
                    m_mapPorts.insert(make_pair(*it_ifs, port));
                }
            }
        }
    }
#endif

    VNetGetAllLoopBackPort(nics);

    if (nics.empty())
    {
        map<string,CUplinkLoopPortClass*>::iterator itMap = m_mapPorts.begin();
        for (;itMap != m_mapPorts.end();)
        {
            CUplinkLoopPortClass *port = static_cast<CUplinkLoopPortClass *>(itMap->second);
            if (NULL != port)
            {
                delete port;
            }
            m_mapPorts.erase(itMap++);
        }

        return;
    }

    vector<string>::iterator it = nics.begin();

    for (; it!= nics.end(); ++it )
    {
        //�ж϶˿ڵ�����
        if (((*it).find("loop", 0)) == 0)
        {
            if (m_mapPorts.find(*it) != m_mapPorts.end())
            {
                continue;
            }

            CUplinkLoopPortClass *port = NULL;
            if (NULL != (port = new CUplinkLoopPortClass()))
            {
                //�����µ�����˿�
                port->SetNicName(*it);
                m_mapPorts.insert(make_pair(*it, port));
            }
        }
    }

    //ɾ��������uplink loop port
    map<string,CUplinkLoopPortClass*>::iterator it_a = m_mapPorts.begin();
    for (; it_a != m_mapPorts.end();)
    {
        //�ϴ�ɨ�赽�Ķ˿ں��²鵽����˿ڱȽϣ������Ƿ����
        vector<string>::iterator pos;
        pos = find(nics.begin(), nics.end(), (it_a->first));
        if (pos != nics.end())
        {
            it_a++;
            continue;
        }

        CUplinkLoopPortClass *port = static_cast<CUplinkLoopPortClass *>(it_a->second);
        if (NULL != port)
        {
            delete port;
        }

        m_mapPorts.erase((it_a++)->first);
    }

    //ɨ���ִ�л�ȡ�˿ڲ�����Ϣ
    //GetPortWorkPara();
}


//���ԺͲ��Դ���
/******************************************************************************/
void CUplinkLoopPortManager::DbgShow()
{
    //MutexLock lock(_mutex);
    CUplinkLoopPortClass *port;
    map<string, CUplinkLoopPortClass*>::iterator it_port;
    for (it_port = m_mapPorts.begin(); it_port != m_mapPorts.end(); ++it_port)
    {
        port = it_port->second;
        if (NULL != port)
        {
            port->DbgShow();
        }
    }
}

//test code
void TestUplinkLoopPortInfo()
{
    CMessageVNAPortInfoReport PortInfo;
    CUplinkLoopPortManager *pHandle = CUplinkLoopPortManager::GetInstance();
    if (pHandle)
    {
        pHandle->GetUplinkLoopPortInfo(PortInfo._uplink_info);
        pHandle->DbgShow();
    }
    return;
}
DEFINE_DEBUG_FUNC(TestUplinkLoopPortInfo);

void VnetDbgShowUpLoopPortInfo()
{
    CUplinkLoopPortManager *pHandle = CUplinkLoopPortManager::GetInstance();
    if (pHandle)
    {
        pHandle->DbgShow();
    }
    return;
}
DEFINE_DEBUG_FUNC(VnetDbgShowUpLoopPortInfo);
} // namespace zte_tecs

