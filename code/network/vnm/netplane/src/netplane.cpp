/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：netplane.cpp
* 文件标识：
* 内容摘要：CNetplane类实现文件
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
#include "netplane.h"

namespace zte_tecs
{
    CNetplane::~CNetplane()
    {
        
    } 

    string CNetplane::GetName(void) const
    {
        return m_strName;
    }
    
    string CNetplane::GetUUID(void) const
    {
        return m_strUUID;
    }
    
    string CNetplane::GetDescription(void) const
    {
        return m_strDescription;
    }
    
    uint32 CNetplane::GetMTU(void) const
    {
        return m_dwMTU;
    }

    int32 CNetplane::GetId(void) const
    {
        return m_nId;
    }
    
    
    int32 CNetplane::SetName(const string &strName)
    {
        m_strName = strName;
        return 0;
    }
    
    int32 CNetplane::SetUUID(const string &strUUID)
    {
        m_strUUID = strUUID;
        return 0;
    }
    
    int32 CNetplane::SetDescription(const string &strDescription)
    {
        m_strDescription = strDescription;
        return 0;
    }
    
    int32 CNetplane::SetMTU(uint32 dwMTU)
    {
        m_dwMTU = dwMTU;
        return 0;
    }

    int32 CNetplane::SetId(int32 dwId)
    {
        m_nId = dwId;
        return 0;
    }
    

}// end namespace zte_tecs

