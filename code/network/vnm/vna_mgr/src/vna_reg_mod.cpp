/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�vna_reg_mod.cpp
* �ļ���ʶ��
* ����ժҪ��CVNARegModInfo��ʵ���ļ�
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
#include "vna_reg_mod.h"

namespace zte_tecs
{
    uint8 CVNARegModInfo::GetRegModRole(void) const
    {
        return m_ucModRole;
    }
    
    uint8 CVNARegModInfo::GetRegModState(void) const
    {
        return m_ucRegModState;
    }

    string CVNARegModInfo::GetRegModApp(void) const
    {
        return m_strModApp;
    }

    string CVNARegModInfo::GetUUID(void) const
    {
        return m_strUUID;
    }
    
    string CVNARegModInfo::GetExtInfo(void) const
    {
        return m_strExtInfo;
    }
    
    int32 CVNARegModInfo::SetRegModRole(uint8 ucRole)
    {
        m_ucModRole = ucRole;
        return 0;
    }

    
    int32 CVNARegModInfo::SetRegModState(uint8 ucState)
    {
        m_ucRegModState = ucState;
        return 0;
    }

    int32 CVNARegModInfo::SetRegModApp(const string &cstrName)
    {
        m_strModApp = cstrName;
        return 0;
    }
    
    int32 CVNARegModInfo::SetExtInfo(const string &cstrExtInfo)
    {
        m_strExtInfo = cstrExtInfo;
        return 0;
    }  
    
    int32 CVNARegModInfo::SetUUID(const string &cstrUUID)
    {
        m_strUUID = cstrUUID;
        return 0;
    }
}// end namespace zte_tecs

