/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：vnet_phyport_manager.cpp
* 文件标识：
* 内容摘要：物理端口类实现文件
* 当前版本：V1.0
* 作    者：liuhx
* 完成日期：2013年2月17日
*******************************************************************************/
#include "vnet_comm.h"
#include "vnet_function.h"
#include "vnet_uplinkloopport_manager.h"

using namespace std;
using namespace _GLIBCXX_STD;

namespace zte_tecs
{

//获取实例
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

//实际调用PortClass获取端口各个参数信息
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
            //实际执行PortClass获取端口各个参数信息
            ptemp->GetPortWorkPara(fd);
        }
        close(fd);
    }
}

//获取端口信息
int32 CUplinkLoopPortManager::GetUplinkLoopPortInfo(vector <CUplinkLoopPortReport> &portInfo)
{
    //目前设置和查询没考虑互斥
    //MutexLock lock(_mutex);

    //先清除
    portInfo.clear();

    //先扫描所有的物理端口号
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
        //暂且获取这些基础值方便调试
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

//扫描所有uplink loop port
void CUplinkLoopPortManager::LookupUplinkLoopPort()
{
    vector<string> nics;
#if 0
    //先找桥再找nics
    vector<string> bridges;
    get_bridges(bridges);
    vector<string> ifs;
    vector<string>::iterator it_bridge=bridges.begin();
    for (; it_bridge!= bridges.end(); ++it_bridge )
    {
        //判断桥的特征
        if (((*it_bridge).compare("loop")) > 0)
        {
            get_ifs_of_bridge(*it_bridge,ifs);
            vector<string>::iterator it_ifs=ifs.begin();
            for (; it_ifs!= ifs.end(); ++it_ifs )
            {
                //这样it_ifs需要编码，在同一个br下是否会有名字冲突?
                nics.push_back(*it_ifs);

                if (m_mapPorts.find(*it_ifs) != m_mapPorts.end())
                {
                    continue;
                }

                CUplinkLoopPortClass *port = NULL;
                if (NULL != (port = new CUplinkLoopPortClass()))
                {
                    //插入新的物理端口
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
        //判断端口的特征
        if (((*it).find("loop", 0)) == 0)
        {
            if (m_mapPorts.find(*it) != m_mapPorts.end())
            {
                continue;
            }

            CUplinkLoopPortClass *port = NULL;
            if (NULL != (port = new CUplinkLoopPortClass()))
            {
                //插入新的物理端口
                port->SetNicName(*it);
                m_mapPorts.insert(make_pair(*it, port));
            }
        }
    }

    //删除不存在uplink loop port
    map<string,CUplinkLoopPortClass*>::iterator it_a = m_mapPorts.begin();
    for (; it_a != m_mapPorts.end();)
    {
        //上次扫描到的端口和新查到物理端口比较，查找是否存在
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

    //扫描后执行获取端口参数信息
    //GetPortWorkPara();
}


//调试和测试代码
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

