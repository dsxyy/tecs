/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：vnet_trunkport_manager.cpp
* 文件标识：
* 内容摘要：trunk端口类实现文件
* 当前版本：V1.0
* 作    者：liuhx
* 完成日期：2013年2月19日
*******************************************************************************/
#include "vnet_comm.h"
#include "vna_common.h"
#include "dev_monitor.h"
#include "dev_mgr.h"
#include "vnet_function.h"
#include "vnet_trunkport_manager.h"

namespace zte_tecs
{

//实例
CTrunkPortManager *CTrunkPortManager::m_instance = NULL;

CTrunkPortManager::~CTrunkPortManager()
{
    m_instance = NULL;
    map<string,CTrunkPortClass*>::iterator it = m_mapTrunks.begin();

    for (;it != m_mapTrunks.end();)
    {
        CTrunkPortClass *port = static_cast<CTrunkPortClass *>(it->second);
        if (NULL != port)
        {
            delete port;
        }

        m_mapTrunks.erase(it++);
    }
}

/****************************Lookup Function **************************************************/
//获取trunkport子函数
CTrunkPortClass *CTrunkPortManager::GetTrunk(const string &strTrunkName)
{
    if (strTrunkName.empty())
    {
        return NULL;
    }

    map<string, CTrunkPortClass*>::iterator it =m_mapTrunks.find(strTrunkName);

    if (it == m_mapTrunks.end())
    {
        return NULL;
    }

    return static_cast<CTrunkPortClass *>(it->second);
}

//添加trunkport子函数
CTrunkPortClass *CTrunkPortManager::AddTrunk(const string &strTrunkName)
{
    if (strTrunkName.empty())
    {
        return NULL;
    }

    //返回已经存在的CTrunkPortClass
    CTrunkPortClass *trunk;
    if (NULL != (trunk = GetTrunk(strTrunkName)))
    {
        return trunk;
    }

    //生成新的CTrunkPortClass
    if (NULL != (trunk = new CTrunkPortClass()))
    {
        m_mapTrunks.insert(make_pair(strTrunkName, trunk));
    }

    return trunk;
}

//删除trunkport子函数
int32 CTrunkPortManager::DelTrunk(const string &strTrunkName)
{
    if (m_mapTrunks.find(strTrunkName) == m_mapTrunks.end())
    {
        return 0;
    }

    CTrunkPortClass *trunk = GetTrunk(strTrunkName);
    if (NULL != trunk)
    {
        delete trunk;
    }

    m_mapTrunks.erase(strTrunkName);

    return 0;
}

//扫描所有trunk口
void CTrunkPortManager::LookupAllTrunk()
{
    vector<string> bondings;
    //VNetGetAllBondings(bondings);
    VNetGetAllBondingsOVS(bondings);

    if (bondings.empty())
    {
        map<string,CTrunkPortClass*>::iterator itMap = m_mapTrunks.begin();
        for (;itMap != m_mapTrunks.end();)
        {
            CTrunkPortClass *port = static_cast<CTrunkPortClass *>(itMap->second);
            if (NULL != port)
            {
                delete port;
            }

            m_mapTrunks.erase(itMap++);
        }
        return;
    }

    vector<string>::iterator it_trunk = bondings.begin();
    for (; it_trunk!=bondings.end(); it_trunk++)
    {
        //以trunk名称和trunkclass形成对应关系
        AddTrunk(*it_trunk);
    }

    map<string,CTrunkPortClass*>::iterator it = m_mapTrunks.begin();
    for (;it != m_mapTrunks.end();)
    {
        vector<string>::iterator pos;
        pos = find(bondings.begin(), bondings.end(), (it->first));
        if (pos != bondings.end())
        {
            it++;
            continue;
        }

        CTrunkPortClass *port = static_cast<CTrunkPortClass *>(it->second);
        if (NULL != port)
        {
            delete port;
        }

        m_mapTrunks.erase((it++)->first);
    }
}

/****************************GetTrunkInfo Function **************************************************/
int32 CTrunkPortManager::GetTrunkInfo(vector<CBondReport> &vTrunkInfo)
{
    //先扫描trunk口
    LookupAllTrunk();

    if (m_mapTrunks.empty()) return 0;


    map<string, CTrunkPortClass*>::const_iterator iter = m_mapTrunks.begin();
    for (; iter!= m_mapTrunks.end(); ++iter)
    {
        CBondReport trunk;
        CBondInfo info;

        CTrunkPortClass *ptemp = GetTrunk(iter->first);
        if (ptemp == NULL)
        {
            continue;
        }

        if (!(ptemp->GetTrunkInfo(iter->first, info)))
        {
            trunk._bond_name = info.name;
            trunk._bond_report_mode = info.mode;

            //获取bond口的ip/mask
            //VNetGetKernelPortInfo(trunk._bond_name, trunk._basic_info._ip, trunk._basic_info._mask);

            //获取bond口的状态
            /*int sockfd;
            sockfd = socket(AF_INET, SOCK_DGRAM, 0);
            if (sockfd >= 0)
            {
                struct ifreq ifr;
                bzero(ifr.ifr_name, sizeof(ifr.ifr_name));
                strcpy(ifr.ifr_name, trunk._bond_name.c_str());
                if (0 == ioctl(sockfd, SIOCGIFFLAGS, &ifr))
                {
                    if (ifr.ifr_flags & IFF_UP)
                    {
                        trunk._basic_info._state = 1;
                    }
                    else
                    {
                        trunk._basic_info._state = 0;
                    }
                }

                close(sockfd);				
            }*/

            //分模式处理
            // lacp 协商成功的情况下，bond state 设置为1，失败设置为0
            // backup 存在active nic的情况下，bond state 设置为1，没有设置为0 
            switch (info.mode)
            {
            case EN_BOND_MODE_BACKUP:
            {
                trunk._bond_backup._active_nic = info.backup.active_nic;
                if( 0 != STRCMP(trunk._bond_backup._active_nic.c_str(), ""))
                {
                    trunk._basic_info._state = TRUE;
                }
                else
                {
                    trunk._basic_info._state = FALSE;
                }

                trunk._bond_backup._other_nic.clear();
                vector<string>::iterator it_nics = info.backup.other_nic.begin();
                for (; it_nics != info.backup.other_nic.end(); ++it_nics)
                {
                    trunk._bond_backup._other_nic.push_back(*it_nics);
                }

                vTrunkInfo.push_back(trunk);
                break;
            }
            case EN_BOND_MODE_802:
            {
                trunk._bond_lacp._suc_nics.clear();
                trunk._bond_lacp._fail_nics.clear();

                trunk._bond_lacp._state = info.lacp.status;
                trunk._bond_lacp._aggregate_id = info.lacp.aggregator_id;
                trunk._bond_lacp._partner_mac = info.lacp.partner_mac;

                trunk._basic_info._state = FALSE;

                vector<string>::iterator it_nics = info.lacp.nic_suc.begin();
                for (; it_nics != info.lacp.nic_suc.end(); ++it_nics)
                {
                    trunk._bond_lacp._suc_nics.push_back(*it_nics);
                    trunk._basic_info._state = TRUE;
                }

                it_nics = info.lacp.nic_fail.begin();
                for (; it_nics != info.lacp.nic_fail.end(); ++it_nics)
                {
                    trunk._bond_lacp._fail_nics.push_back(*it_nics);
                }

                vTrunkInfo.push_back(trunk);
                break;
            }
            default:
            {
                break;
            }
            }
        }
    }

    return 0;
}

/****************************Interface Function **************************************************/
//提供trunk添加操作
int32 CTrunkPortManager::CreateBond(const string &strBondName,const string &strMode, vector<string> &ifs)
{
    if (strBondName.empty())
    {
        return -1;
    }

    //先判断trunk是否存在，存在则删除TrunkPortClass，以防成员口变化
    DelTrunk(strBondName);

    //默认802.3ad模式
    VNetAddBond(strBondName, strMode, ifs);

    //添加到map中
    AddTrunk(strBondName);

    return 0;
}

//提供trunk删除操作
int32 CTrunkPortManager::DelBond(const string &strBondName)
{
    if (strBondName.empty())
    {
        return -1;
    }

    if (m_mapTrunks.find(strBondName) == m_mapTrunks.end())
    {
        return -1;
    }

    VNetDelBond(strBondName);

    DelTrunk(strBondName);

    return 0;
}

//提供修改trunk模式
int32 CTrunkPortManager::SetBondMode(const string &strBondName, int32 nBondMode)
{
    if (strBondName.empty())
    {
        return -1;
    }

    if (m_mapTrunks.find(strBondName) == m_mapTrunks.end())
    {
        return -1;
    }

    stringstream stream;
    string type;
    stream<<nBondMode;
    type=stream.str();
    if (SUCCESS != VNetSetBondPara(strBondName,"mode",type))
    {
        return -1;
    }

    return 0;
}

//获取bond参数信息?
int32 CTrunkPortManager::GetBondPara(const string strBondName,string& strMode,string& strMiimon,
                                     string& strLacprate, string& strMiistatus, vector<string>& vSlaves)
{
    if (SUCCESS == get_bond_info(strBondName, strMode, strMiimon, strLacprate, strMiistatus, vSlaves))
    {
        return 0;
    }

    return -1;
}

//查询bond是否存在，存在返回0，并提供bond成员口
int32 CTrunkPortManager::GetBondMember(const string &strBondName, int32 nBondMode, vector<string> &ifs)
{
    int32 ret;
    vector<string> bondings;
    CBondInfo info;

    if (strBondName.empty())
    {
        return -1;
    }

    ret = VNetGetAllBondingsOVS(bondings);
    if (ret != SUCCESS)
    {
        return -1;
    }

    if (find(bondings.begin(), bondings.end(), strBondName) != bondings.end())
    {
        if (SUCCESS != get_bond_info(strBondName, info))
        {
            return -1;
        }

        //分模式处理
        switch (info.mode)
        {
        case EN_BOND_MODE_BACKUP:
        {
            ifs.push_back(info.backup.active_nic);
            nBondMode = EN_BOND_MODE_BACKUP;
            vector<string>::iterator it_nics = info.backup.other_nic.begin();
            for (; it_nics != info.backup.other_nic.end(); ++it_nics)
            {
                ifs.push_back(*it_nics);
            }

            return 0;
        }
        case EN_BOND_MODE_802:
        {
            nBondMode = EN_BOND_MODE_802;
            vector<string>::iterator suc_nics = info.lacp.nic_suc.begin();
            for (; suc_nics != info.lacp.nic_suc.end(); ++suc_nics)
            {
                ifs.push_back(*suc_nics);
            }

            vector<string>::iterator fail_nics = info.lacp.nic_fail.begin();
            for (; fail_nics != info.lacp.nic_fail.end(); ++fail_nics)
            {
                ifs.push_back(*fail_nics);
            }

            return 0;
        }
        default:
        {
            break;
        }
        }

        return -1;
    }

    return -1;
}

#if 0
int32 CTrunkPortManager::GetPortMapInfo(map<string, int32> &mapPortInfo)
{
    string strName;
    int32  nStatus;

    map<string, CTrunkPortClass*>::iterator iter = m_mapTrunks.begin();
    for (; iter!= m_mapTrunks.end(); ++iter)
    {
        CTrunkPortClass *ptemp = GetTrunk(iter->first);
        if (NULL != ptemp)
        {
            strName = ptemp->GetTrunkName();
            //状态如何填写呢??????????????????????
            nStatus = 1;
            mapPortInfo.insert(make_pair(strName, nStatus));
        }
    }

    return 0;
}
#endif

int32 CTrunkPortManager::GetPortMapInfo(map<string, int32> &mapPortInfo)
{
    string strName;
    int32  nStatus;

    vector<string> bondings;
    //VNetGetAllBondings(bondings);
    VNetGetAllBondingsOVS(bondings);

    vector<string>::iterator iter = bondings.begin();
    for (; iter!= bondings.end(); ++iter)
    {
        strName = (*iter);
        nStatus = 0;
        mapPortInfo.insert(make_pair(strName, nStatus));
    }

    return 0;
}

void CTrunkPortManager::DoMonitor(CNetDevVisitor *dev_visit)
{
    map<string, CTrunkPortClass*>::iterator port_dev = m_mapTrunks.begin();

    for (; port_dev!=m_mapTrunks.end(); port_dev++)
    {
        port_dev->second->Accept(dev_visit);
    }
};

void CTrunkPortManager::DoMonitor(CPortVisitor *dev_visit)
{
    map<string, CTrunkPortClass*>::iterator port_dev = m_mapTrunks.begin();

    for (; port_dev!=m_mapTrunks.end(); port_dev++)
    {
        port_dev->second->Accept(dev_visit);
    }
};

/****************************Debug Function **************************************************/
void CTrunkPortManager::DbgShow()
{
    CTrunkPortClass *trunk;
    map<string, CTrunkPortClass*>::iterator it;
    for (it = m_mapTrunks.begin(); it != m_mapTrunks.end(); ++it)
    {
        trunk = it->second;
        if (NULL != trunk)
        {
            trunk->DbgShow();
        }
    }
}

void TestTrunkMap()
{
    CMessageVNAPortInfoReport PortInfo;
    CTrunkPortManager *trunk = CTrunkPortManager::GetInstance();
    if (NULL == trunk)
    {
        cout << "TrunkPortManager is not created." << endl;
        return;
    }
    trunk->GetTrunkInfo(PortInfo._bond_info);
    trunk->DbgShow();
}

DEFINE_DEBUG_FUNC(TestTrunkMap);

void VnetDbgShowTrunk()
{
    CTrunkPortManager *trunk = CTrunkPortManager::GetInstance();
    if (NULL == trunk)
    {
        cout << "CTrunkPortManager is not created." << endl;
        return;
    }
    trunk->DbgShow();
}

DEFINE_DEBUG_FUNC(VnetDbgShowTrunk);
}

