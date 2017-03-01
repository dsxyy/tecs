/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：netplane.h
* 文件标识：
* 内容摘要：CNetplane类的定义文件
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

#if !defined(NET_PLANE_INCLUDE_H__)
#define NET_PLANE_INCLUDE_H__

namespace zte_tecs
{
    class CNetplane
    {
    public:
        explicit CNetplane(){m_dwMTU = VNET_COMMON_MTU;}
        virtual ~CNetplane();

    public:
        string GetName(void) const;
        string GetUUID(void) const;
        string GetDescription(void) const;
        uint32 GetMTU(void) const;
        int32  GetId(void) const;

        int32 SetName(const string &strName);
        int32 SetUUID(const string &strUUID);
        int32 SetDescription(const string &strDescription);
        int32 SetMTU(uint32 dwMTU);
        int32 SetId(int32 dwId);
        
    protected:        
        
    private:        

    private:
        string m_strName;
        string m_strUUID;
        string m_strDescription;
        uint32 m_dwMTU;
        int32  m_nId;
    };
}// namespace zte_tecs

#endif

