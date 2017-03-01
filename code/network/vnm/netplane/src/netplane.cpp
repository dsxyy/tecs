/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�netplane.cpp
* �ļ���ʶ��
* ����ժҪ��CNetplane��ʵ���ļ�
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

