/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�vna_reg_mod.h
* �ļ���ʶ��
* ����ժҪ��CVNARegModInfo��Ķ����ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�
* ������ڣ�2012��1��18��
*
* �޸ļ�¼1��
*     �޸����ڣ�2013/1/18
*     �� �� �ţ�V1.0
*     �� �� �ˣ�Wang.Lule
*     �޸����ݣ�����
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

