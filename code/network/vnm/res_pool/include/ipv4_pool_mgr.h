/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�ipv4_pool_mgr.h
* �ļ���ʶ��
* ����ժҪ��CIPv4PoolMgr��Ķ����ļ�
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

#if !defined(IPV4_POOL_MGR_INCLUDE_H__)
#define IPV4_POOL_MGR_INCLUDE_H__

#include "ipv4_addr_range.h"

namespace zte_tecs
{
    class CIPv4PoolMgr
    {
    public:
        explicit CIPv4PoolMgr();
        virtual ~CIPv4PoolMgr();

    public:
        static CIPv4PoolMgr *GetInstance()
        {
            if(NULL == s_pInstance)
            {
                s_pInstance = new CIPv4PoolMgr();
            }
            
            return s_pInstance;
        };
        
        int32 AllocIPAddrForVnic(CVNetVmMem &cVNetVmMem);
        int32 FreeIPAddrForVnic(CVNetVmMem &cVNetVmMem);

        int32 NetplaneAllocIPAddr(const string strNetplaneID, string &strIP, string &strMask);
        int32 NetplaneFreeIPAddr(const string strNetplaneID, const string strIP, const string strMask);
            
        
    public:
        int32 Init(MessageUnit *pMu);
        void MessageEntry(const MessageFrame &message);
        
        void SetDbgInfFlg(BOOL bOpen){ m_bOpenDbgInf = bOpen; return ;}
        void DbgShowNetplaneIPRange(const string &cstrNetplaneUUID);
        void DbgShowLogiNetIPPool(const string &cstrLogiNetUUID);
        
    protected:
        int32 ProcNetplaneIPv4RangeMsg(const MessageFrame &message);
        int32 ProcLogicNetworkIPv4RangeMsg(const MessageFrame &message);
    
    private:
        int32 AddNetplaneIPv4Range(const CNetplaneIPv4RangeMsg &cMsg, string &strRetInfo);
        int32 DelNetplaneIPv4Range(const CNetplaneIPv4RangeMsg &cMsg, string &strRetInfo);
        
        int32 AddLogicNetworkIPv4Range(const CLogicNetworkIPv4RangeMsg &cMsg, string &strRetInfo);
        int32 DelLogicNetworkIPv4Range(const CLogicNetworkIPv4RangeMsg &cMsg, string &strRetInfo);  
        
        int32 GetLogiNetIPv4Range(const string &cstrLogicNetworkID, CIPv4AddrRange &cIPv4Range);
        int32 GetNetplaneIPv4Range(const string &cstrNetplaneID, CIPv4AddrRange &cIPv4Range);

    private:
        static CIPv4PoolMgr *s_pInstance;

        MessageUnit *m_pMU;

        BOOL m_bOpenDbgInf;
        DISALLOW_COPY_AND_ASSIGN(CIPv4PoolMgr);
    };
}// namespace zte_tecs

#endif

