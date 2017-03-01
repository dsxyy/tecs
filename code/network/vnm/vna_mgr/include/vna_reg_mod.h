/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：vna_reg_mod.h
* 文件标识：
* 内容摘要：CVNARegModInfo类的定义文件
* 当前版本：1.0
* 作    者：
* 完成日期：2012年1月18日
*
* 修改记录1：
*     修改日期：2013/1/18
*     版 本 号：V1.0
*     修 改 人：Wang.Lule
*     修改内容：创建
*
******************************************************************************/

#if !defined(VNA_REG_MOD_INCLUDE_H__)
#define VNA_REG_MOD_INCLUDE_H__

namespace zte_tecs
{
    class CVNARegModInfo
    {
    public:
        explicit CVNARegModInfo() {m_ucRegModState = EN_STATE_OFFLINE;}
        virtual ~CVNARegModInfo() {m_ucRegModState = EN_STATE_OFFLINE;}
        
    public:    
        uint8 GetRegModRole(void) const;   
        string GetUUID(void) const; 
        uint8 GetRegModState(void) const;
        string GetRegModApp(void) const;
        string GetExtInfo(void) const;

        int32 SetRegModRole(uint8 ucRole);
        int32 SetRegModState(uint8 ucState);        
        int32 SetRegModApp(const string &cstrName);
        int32 SetExtInfo(const string &cstrExtInfo);
        int32 SetUUID(const string &cstrUUID);
        
    private:
        uint8 m_ucModRole;      // Role ID.        
        uint8 m_ucRegModState;
        string m_strModApp;     // Module application.
        string m_strExtInfo;    // Ext info,exam: hc's cc application.
        string m_strUUID;       // UUID        
    };  
}// namespace zte_tecs

#endif

