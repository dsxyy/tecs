/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：vnet_trunkport_class.cpp
* 文件标识：
* 内容摘要：TRUNK端口类实现文件
* 当前版本：V1.0
* 作    者：liuhx
* 完成日期：2013年2月20日
*******************************************************************************/
#include "vnet_comm.h"
#include "vna_common.h"
#include "dev_monitor.h"
#include "dev_mgr.h"
#include "vnet_function.h"
#include "vnet_trunkport_class.h"

namespace zte_tecs
{

//获取本trunk口的信息
int32 CTrunkPortClass::GetTrunkInfo(const string strTrunkName, CBondInfo &cInfo)
{
    /*if (SUCCESS != get_bond_info(strTrunkName, cInfo))
    {
        return -1;
    }*/

    string strMode = "";
    int32 nStatus = 0;

    if (VNET_PLUGIN_SUCCESS != VNetGetBondModeOVS(strTrunkName, strMode))
    {
        return -1;
    }

    //分模式处理
    if (strMode == "1"/*EN_BOND_MODE_BACKUP*/)
    {
        if (VNET_PLUGIN_SUCCESS != VNetBackupInfo(strTrunkName, cInfo.backup.active_nic, cInfo.backup.other_nic))
        {
            VNET_LOG(VNET_LOG_ERROR, "CTrunkPortClass:[%s] GetTrunkInfo VNetBackupInfo failed.\n", strTrunkName.c_str());
            return -1;
        }

        //下面开始获取信息，成员和出参都返回
        m_strName = strTrunkName;
        cInfo.name = strTrunkName;
        m_nMode = EN_BOND_MODE_BACKUP;
        cInfo.mode = EN_BOND_MODE_BACKUP;


        m_tBackup.active_nic = cInfo.backup.active_nic;
        m_tBackup.other_nic.clear();

        vector<string>::iterator it_nics = cInfo.backup.other_nic.begin();
        for (; it_nics != cInfo.backup.other_nic.end(); ++it_nics)
        {
            m_tBackup.other_nic.push_back(*it_nics);
        }
    }
    else if (strMode == "4"/*EN_BOND_MODE_802*/)
    {
        m_tLacp.nic_suc.clear();
        m_tLacp.nic_fail.clear();

        if (VNET_PLUGIN_SUCCESS != VNetLacpInfo(strTrunkName, nStatus, cInfo.lacp.aggregator_id, cInfo.lacp.partner_mac, \
                                                cInfo.lacp.nic_suc, \
                                                cInfo.lacp.nic_fail))
        {
            VNET_LOG(VNET_LOG_ERROR, "CTrunkPortClass:[%s] GetTrunkInfo VNetLacpInfo failed.\n", strTrunkName.c_str());
            return -1;
        }

        //下面开始获取信息，成员和出参都返回
        m_strName = strTrunkName;
        cInfo.name = strTrunkName;
        m_nMode = EN_BOND_MODE_802;
        cInfo.mode = EN_BOND_MODE_802;
        m_tLacp.status = nStatus;
        cInfo.lacp.status = nStatus;

        m_tLacp.aggregator_id = cInfo.lacp.aggregator_id;
        m_tLacp.partner_mac = cInfo.lacp.partner_mac;

        if (0 != nStatus)
        {
            vector<string>::iterator suc_nics = cInfo.lacp.nic_suc.begin();
            for (; suc_nics != cInfo.lacp.nic_suc.end(); ++suc_nics)
            {
                m_tLacp.nic_suc.push_back(*suc_nics);
            }

            vector<string>::iterator fail_nics = cInfo.lacp.nic_fail.begin();
            for (; fail_nics != cInfo.lacp.nic_fail.end(); ++fail_nics)
            {
                m_tLacp.nic_fail.push_back(*fail_nics);
            }
        }
        else
        {
            cInfo.lacp.nic_suc.clear();
            cInfo.lacp.nic_fail.clear();
            //协商失败，lacp重新获取slave
            if (0 != VNetGetBondSlavesOVS(strTrunkName, cInfo.lacp.nic_fail))
            {
                return -1;
            }
            vector<string>::iterator slave_nics = cInfo.lacp.nic_fail.begin();
            for (; slave_nics != cInfo.lacp.nic_fail.end(); ++slave_nics)
            {
                m_tLacp.nic_fail.push_back(*slave_nics);
            }
        }
    }
    else
    {
        //0 不确定
        VNET_LOG(VNET_LOG_ERROR, "CTrunkPortClass:[%s] GetTrunkInfo VNetGetBondModeOVS mode is 0 \n", strTrunkName.c_str());
        return -1;
    }

    return 0;
}

/******************************************************************************/
void CTrunkPortClass::DbgShow()
{
    cout << "----------- TrunkPort Info ----------------" << endl;
    cout << "trunk_name = " << m_strName << endl;
    cout << "trunk_mode = " << m_nMode << endl;

    if (m_nMode == EN_BOND_MODE_BACKUP)
    {
        cout << "active_nic = " << m_tBackup.active_nic<< endl;
        cout << "----------Backup other nic---------------" << endl;
        vector<string>::iterator it;
        for (it = m_tBackup.other_nic.begin(); it != m_tBackup.other_nic.end(); ++it)
        {
            cout << *it<< endl;
        }
    }
    else if (m_nMode == EN_BOND_MODE_802)
    {
        cout << "802 status = " << m_tLacp.status<< endl;
        cout << "802 agg = " << m_tLacp.aggregator_id<< endl;
        cout << "802 patmac = " << m_tLacp.partner_mac<< endl;

        cout << "----------802 nic suc--------------" << endl;
        vector<string>::iterator it_a;
        for (it_a = m_tLacp.nic_suc.begin(); it_a!= m_tLacp.nic_suc.end(); ++it_a)
        {
            cout << *it_a<< endl;
        }

        cout << "----------802 nic fail--------------" << endl;
        vector<string>::iterator it_b;
        for (it_b = m_tLacp.nic_fail.begin(); it_b != m_tLacp.nic_fail.end(); ++it_b)
        {
            cout << *it_b<< endl;
        }
    }
}
} // namespace zte_tecs

