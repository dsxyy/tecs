/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�vna_info.h
* �ļ���ʶ��
* ����ժҪ��CVNAInfo��Ķ����ļ�
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

#if !defined(VNA_INFO_INCLUDE_H__)
#define VNA_INFO_INCLUDE_H__

namespace zte_tecs
{
    class CVNAInfo
    {
    public:
        explicit CVNAInfo();
        virtual ~CVNAInfo();

    public:
        string GetApp(void) const;
        string GetUUID(void) const;
        string GetVNMApp(void)const;
        string GetHost(void) const;
        string GetOSName(void) const;
        uint8 GetState(void) const;
        uint32 GetDelayUpdateTime(void) const; 
        uint32 GetAgeingTime(void) const;
        vector<CVNARegModInfo>& GetRegistedModInfo(void);
        CVNARegModInfo *GetRegModInfo(const string &cstrModApp, uint8 ucRole);

        int32 SetApp(const string &cstrApp);
        int32 SetUUID(const string &cstrUUID);
        int32 SetVNMApp(const string &cstrVNMApp);
        int32 SetState(uint8 ucState);
        int32 SetHost(const string &cstrHost);
        int32 SetOSName(const string &cstrOSName);
        int32 SetDelayUpdateTime(uint32 dwTime);
        int32 SetAgeingTime(uint32 dwTime);
        int32 AddRegModInfo(const CVNARegModInfo &cRegModInfo);
        int32 DelRegModInfo(const string &cstrModApp);

        int32 Copy(CVNAInfo &cVNA);
    
    private:
        string m_strUUID;
        string m_strApp;
        uint8  m_ucState;
        string m_strHost;
        string m_strVNMApp;
        string m_strOSName;
        vector<CVNARegModInfo> m_vecRegModInfo;
   
        uint32 m_dwDelayUpdateTime; // �������ʱ��;

        uint32 m_dwAgeingTime;      // ע���ϻ�ʱ��;
        
        DISALLOW_COPY_AND_ASSIGN(CVNAInfo);
    };

}// namespace zte_tecs

#endif

