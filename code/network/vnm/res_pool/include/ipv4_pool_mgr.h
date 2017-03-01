/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：ipv4_pool_mgr.h
* 文件标识：
* 内容摘要：CIPv4PoolMgr类的定义文件
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

