/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�mac_pool_mgr.h
* �ļ���ʶ��
* ����ժҪ��CMACPoolMgr��Ķ����ļ�
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

