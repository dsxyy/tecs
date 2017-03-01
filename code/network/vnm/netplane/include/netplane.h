/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�netplane.h
* �ļ���ʶ��
* ����ժҪ��CNetplane��Ķ����ļ�
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

