/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：mac_pool_mgr.h
* 文件标识：
* 内容摘要：CMACPoolMgr类的定义文件
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

#if !defined(MAC_POOL_MGR_INCLUDE_H__)
#define MAC_POOL_MGR_INCLUDE_H__

namespace zte_tecs
{
    class CMACPoolMgr
    {
    public:
        explicit CMACPoolMgr();
        virtual ~CMACPoolMgr();

    public:
        static CMACPoolMgr *GetInstance()
        {
            if(NULL == s_pInstance)
            {
                s_pInstance = new CMACPoolMgr();
            }
            
            return s_pInstance;
        };
        
    public:
        int32 AllocMACForVnic(CVNetVmMem & cVNetVmMem);
        int32 FreeMACForVnic(CVNetVmMem & cVNetVmMem);
        
    public:
        int32 Init(MessageUnit *pMu);
        void MessageEntry(const MessageFrame &message);
        
        void SetDbgInfFlg(BOOL bOpen){ m_bOpenDbgInf = bOpen; return ;}

        void DbgShowData(const string &cstrNetplaneUUID);
        
    protected:
        int32 ProcMACRangeMsg(const MessageFrame &message);

    private:
        int32 AddMACRange(const CMACRangeMsg &cMsg, string &strRetInfo);
        int32 DelMACRange(const CMACRangeMsg &cMsg, string &strRetInfo);        
        int32 GetMACRange(const string &cstrNetplaneID, CMACRange &cOUI1MACRange, CMACRange &cOUI2MACRange);

    private:
        static CMACPoolMgr *s_pInstance;

        MessageUnit *m_pMU;

        BOOL m_bOpenDbgInf;
        
        DISALLOW_COPY_AND_ASSIGN(CMACPoolMgr);
    };
}// namespace zte_tecs

#endif

