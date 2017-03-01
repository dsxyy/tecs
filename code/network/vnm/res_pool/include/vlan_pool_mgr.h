/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�vlan_pool_mgr.h
* �ļ���ʶ��
* ����ժҪ��CVlanPoolMgr��Ķ����ļ�
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

#if !defined(VLAN_POOL_MGR_INCLUDE_H__)
#define VLAN_POOL_MGR_INCLUDE_H__

namespace zte_tecs
{
    class CVlanPoolMgr
    {
    public:
        explicit CVlanPoolMgr();
        virtual ~CVlanPoolMgr();

    public:
        static CVlanPoolMgr *GetInstance()
        {
            if(NULL == s_pInstance)
            {
                s_pInstance = new CVlanPoolMgr();
            }
            
            return s_pInstance;
        };
        
    public:    
        int32 AllocVlanForVnic(CVNetVmMem & cVNetVmMem);
        int32 FreeVlanForVnic(CVNetVmMem & cVNetVmMem);
        int32 GetVlanDetail(int64 nProjID, vector<CVNetVnicDetail> &vecVnicDetail);
        int32 IsExistVlanRange(const string &cstrNetplaneID);
        int32 IsInVlanMapRanges(uint32 dwVlanNum, const string &cstrNetplaneID); 
        int32 IsRangeCrossRanges(uint32 dwVlanStart, uint32 dwVlanEnd, const string &cstrNetplaneID);
        
    public:
        int32 Init(MessageUnit *pMu);
        void MessageEntry(const MessageFrame &message);
        
        void SetDbgInfFlg(BOOL bOpen){ m_bOpenDbgInf = bOpen; return ;}
        int32 DbgShowVlanPool(const string &cstrLogicNetworkID);
        
    protected:
        int32 ProcVlanRangeMsg(const MessageFrame &message);        
    
    private:
        int32 AddVlanRange(const CVlanRangeMsg &cMsg, string &strRetInfo);
        int32 DelVlanRange(const CVlanRangeMsg &cMsg, string &strRetInfo);
        int32 GetVlanRange(const string &cstrNetplaneID, CVlanRange &cVlanRange);
        int32 IsConflictWithShareNetAccessVlan(const CVlanRangeMsg &cMsg, int32 &nConflictVlan);
        int32 IsConflictWithShareNetTrunkVlan(const CVlanRangeMsg &cMsg, 
                                              int32 &nConflictVlanStart, int32 &nConflictVlanEnd);
        int32 GetIsolateNo(const CVlanRangeMsg &cMsg, vector<int32> &vecIsolateNo);
        
    private:

    private:
        static CVlanPoolMgr *s_pInstance;

        MessageUnit *m_pMU;

        map<string, CVlanPoolMgr *> m_mapVlanPool;        
        BOOL m_bOpenDbgInf;
        DISALLOW_COPY_AND_ASSIGN(CVlanPoolMgr);
    };
}// namespace zte_tecs

#endif

