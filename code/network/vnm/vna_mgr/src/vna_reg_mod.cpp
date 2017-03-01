/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：vna_reg_mod.cpp
* 文件标识：
* 内容摘要：CVNARegModInfo类实现文件
* 当前版本：1.0
* 作    者：
* 完成日期：2013年1月18日
*
* 修改记录1：
*     修改日期：2013/1/18
*     版 本 号：V1.0
*     修 改 人：Wang.Lule
*     修改内容：创建
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

