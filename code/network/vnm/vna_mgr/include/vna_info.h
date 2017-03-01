/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：vna_info.h
* 文件标识：
* 内容摘要：CVNAInfo类的定义文件
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
   
        uint32 m_dwDelayUpdateTime; // 心跳检测时间;

        uint32 m_dwAgeingTime;      // 注册老化时间;
        
        DISALLOW_COPY_AND_ASSIGN(CVNAInfo);
    };

}// namespace zte_tecs

#endif

