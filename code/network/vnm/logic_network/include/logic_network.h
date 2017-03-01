/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�logic_network.h
* �ļ���ʶ��
* ����ժҪ��CLogicNetwork��Ķ����ļ�
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

