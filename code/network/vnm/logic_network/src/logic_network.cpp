/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�logic_network.cpp
* �ļ���ʶ��
* ����ժҪ��CLogicNetwork��ʵ���ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�
* ������ڣ�2013��1��18��
*
* �޸ļ�¼1��
*     �޸����ڣ�2013/1/18
*     �� �� �ţ�V1.0
*     �� �� �ˣ�Wang.Lule
*     �޸����ݣ�����
******************************************************************************/

#include "vnet_comm.h"
#include "ipv4_addr.h"
#include "ipv4_addr_range.h"
#include "logic_network.h"

namespace zte_tecs
{
    CLogicNetwork::CLogicNetwork(const string &cstrPortGroupUUID)
    {
        m_strPortGoupUUID = cstrPortGroupUUID;
    }
    
    CLogicNetwork::~CLogicNetwork()
    {
        
    } 

    string CLogicNetwork::GetName(void) const
    {
        return m_strName;
    }
    
    string CLogicNetwork::GetUUID(void) const
    {
        return m_strUUID;
    }

    string CLogicNetwork::GetPortGroupUUID() const
    {
        return m_strPortGoupUUID;
    }
    
    string CLogicNetwork::GetDescription(void) const
    {
        return m_strDescription;
    }
    
    CIPv4AddrRange &CLogicNetwork::GetIPv4AddrRange(void)
    {
        return m_cIPv4Range;
    }
    
    int32 CLogicNetwork::GetIPBootProtoMode(void) const
    {
        return m_nIPBootProtoMode;
    }
    
    int32 CLogicNetwork::SetName(const string &strName)
    {
        m_strName = strName;
        return 0;
    }
    
    int32 CLogicNetwork::SetUUID(const string &strUUID)
    {
        m_strUUID = strUUID;
        return 0;
    }

    int32 CLogicNetwork::SetPortGroupUUID(const string &strPortGroupUUID)
    {
        m_strPortGoupUUID = strPortGroupUUID;
        return 0;
    }
    
    int32 CLogicNetwork::SetDescription(const string &strDescription)
    {
        m_strDescription = strDescription;
        return 0;
    }
    
    int32 CLogicNetwork::SetIPBootProtoMode(int32 nIPBootProtoMode)
    {
        m_nIPBootProtoMode = nIPBootProtoMode;
        return 0;
    }

}// end namespace zte_tecs

