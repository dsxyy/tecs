/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：vna_info.cpp
* 文件标识：
* 内容摘要：CVNAInfo类实现文件
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
#include "vna_info.h"

namespace zte_tecs
{
    
    CVNAInfo::CVNAInfo()
    {
        m_ucState = EN_STATE_INIT;
        m_dwDelayUpdateTime = 0;
        m_dwAgeingTime = 0;
    }
    
    CVNAInfo::~CVNAInfo()
    {
        m_ucState = EN_STATE_INIT;
        m_dwDelayUpdateTime = 0;
        m_dwAgeingTime = 0;      
    }

    string CVNAInfo::GetApp(void) const
    {
        return m_strApp;
    }

    string CVNAInfo::GetUUID(void) const
    {
        return m_strUUID;
    }

    string CVNAInfo::GetVNMApp(void) const
    {
        return m_strVNMApp;
    }

    uint8 CVNAInfo::GetState(void) const
    {
        return m_ucState;
    }
    
    string CVNAInfo::GetHost(void) const
    {
        return m_strHost;
    }

    string CVNAInfo::GetOSName(void) const
    {
        return m_strOSName;
    }

    uint32 CVNAInfo::GetDelayUpdateTime(void) const
    {
        return m_dwDelayUpdateTime;
    }

    uint32 CVNAInfo::GetAgeingTime(void)const
    {
        return m_dwAgeingTime;
    }
    
    vector<CVNARegModInfo>& CVNAInfo::GetRegistedModInfo(void)
    {
        return m_vecRegModInfo;
    }
    
    CVNARegModInfo *CVNAInfo::GetRegModInfo(const string &cstrModApp, uint8 ucRole)
    {
        vector<CVNARegModInfo>::iterator itr;
        for(itr = m_vecRegModInfo.begin(); itr != m_vecRegModInfo.end(); ++itr)
        {
            if(cstrModApp == itr->GetRegModApp() && ucRole == itr->GetRegModRole())
            {
                return &(*itr);                
            }
        }
        
        return NULL;
    }

    int32 CVNAInfo::SetApp(const string &cstrApp)
    {
        m_strApp = cstrApp;
        return 0;
    }

    int32 CVNAInfo::SetUUID(const string &cstrUUID)
    {
        m_strUUID = cstrUUID;
        return 0;
    }

    int32 CVNAInfo::SetVNMApp(const string &cstrVNMApp)
    {
        m_strVNMApp = cstrVNMApp;
        return 0;
    }
    
    int32 CVNAInfo::SetHost(const string &cstrHost)
    {
        m_strHost = cstrHost;
        return 0;
    }

    int32 CVNAInfo::SetOSName(const string &cstrOSName)
    {
        m_strOSName = cstrOSName;
        return 0;
    }
    
    int32 CVNAInfo::SetState(uint8 ucState)
    {
        m_ucState = ucState;
        if((EN_STATE_ONLINE != m_ucState) && (0 != m_dwDelayUpdateTime))
        {
            m_dwDelayUpdateTime = 0;
        }
        
        return 0;
    }

    int32 CVNAInfo::AddRegModInfo(const CVNARegModInfo &cRegModInfo)
    {
        CVNARegModInfo *p = GetRegModInfo(cRegModInfo.GetRegModApp(), cRegModInfo.GetRegModRole());
        if(p)
        {
            VNET_LOG(VNET_LOG_WARN, "CVNAInfo::AddRegModInfo: %s mod is existed in reglist.", 
                cRegModInfo.GetRegModApp().c_str());
            return -1;
        }
#if 0
        p = new CVNARegModInfo(); 
        if(NULL == p)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNAInfo::AddRegModInfo: failed to allocate memory!\n");
            return -1;
        } 
        p->SetUUID(cRegModInfo.GetUUID());
        p->SetRegModRole(cRegModInfo.GetRegModRole());
        p->SetRegModState(cRegModInfo.GetRegModState());
        p->SetRegModApp(cRegModInfo.GetRegModApp());
        p->SetExtInfo(cRegModInfo.GetExtInfo());
#endif
        m_vecRegModInfo.push_back(cRegModInfo);
        return 0;
    }

    int32 CVNAInfo::Copy(CVNAInfo &cVNA)
    {
        //free vector container
        m_strUUID   = cVNA.GetUUID();
        m_strApp    = cVNA.GetApp();
        m_ucState   = cVNA.GetState();
        m_strHost   = cVNA.GetHost();
        m_strVNMApp = cVNA.GetVNMApp();
        m_strOSName = cVNA.GetOSName();
        m_vecRegModInfo.assign(cVNA.GetRegistedModInfo().begin(), cVNA.GetRegistedModInfo().end());
        
        return 0;
    }
    
    int32 CVNAInfo::DelRegModInfo(const string &cstrModApp)
    {
        vector<CVNARegModInfo>::iterator itr = m_vecRegModInfo.begin();
        for(; itr != m_vecRegModInfo.end(); )
        {
            if(cstrModApp == itr->GetRegModApp())
            {
                itr = m_vecRegModInfo.erase(itr);
                break;
            }
            else
            {
                ++itr;
            }
        }
        
        return 0;
    }

    int32 CVNAInfo::SetDelayUpdateTime(uint32 dwTime)
    {
        if(EN_STATE_ONLINE != m_ucState)
        {
            return -1;
        }
        
        m_dwDelayUpdateTime = dwTime;
        return 0;
    }    

    int32 CVNAInfo::SetAgeingTime(uint32 dwTime)
    {
        m_dwAgeingTime = dwTime;
        return 0;
    }
    
}// end namespace zte_tecs

