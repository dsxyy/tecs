/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：logic_network.h
* 文件标识：
* 内容摘要：CLogicNetwork类的定义文件
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

#if !defined(LOGIC_NETWORK_INCLUDE_H__)
#define LOGIC_NETWORK_INCLUDE_H__

namespace zte_tecs
{
    class CLogicNetwork
    {
    public:
        explicit CLogicNetwork(const string &cstrPortGroupUUID);
        virtual ~CLogicNetwork();

    public:
        string GetPortGroupUUID() const;
        string GetName(void) const;
        string GetUUID(void) const;
        string GetDescription(void) const;
        int32 GetIPBootProtoMode(void) const;
        CIPv4AddrRange &GetIPv4AddrRange(void);

        int32 SetName(const string &strName);
        int32 SetUUID(const string &strUUID);
        int32 SetPortGroupUUID(const string &strPortGroupUUID);
        int32 SetDescription(const string &strDescription);
        int32 SetIPBootProtoMode(int32 nIPBootProtoMode);
        
    protected:        
        
    private:        

    private:
        string m_strName;
        string m_strUUID;
        string m_strPortGoupUUID;
        string m_strDescription;
        int32  m_nIPBootProtoMode; // static or dynamic
        CIPv4AddrRange m_cIPv4Range;
    };
}// namespace zte_tecs

#endif

