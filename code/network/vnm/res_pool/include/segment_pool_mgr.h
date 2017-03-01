/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�segment_pool_mgr.h
* �ļ���ʶ��
* ����ժҪ��CSegmentPoolMgr��Ķ����ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�
* ������ڣ�2013��09��05��
*
* �޸ļ�¼1��
*     �޸����ڣ�2013/09/05
*     �� �� �ţ�V1.0
*     �� �� �ˣ�zhao.jin
*     �޸����ݣ�����
*
******************************************************************************/

#if !defined(SEGMENT_POOL_MGR_INCLUDE_H__)
#define SEGMENT_POOL_MGR_INCLUDE_H__

#include "segment_range.h"

namespace zte_tecs
{
    class CSegmentPoolMgr
    {
    public:
        explicit CSegmentPoolMgr();
        virtual ~CSegmentPoolMgr();

    public:
        static CSegmentPoolMgr *GetInstance()
        {
            if(NULL == s_pInstance)
            {
                s_pInstance = new CSegmentPoolMgr();
            }
            
            return s_pInstance;
        };
        
    public:    
        int32 AllocSegmentForVnic(CVNetVmMem & cVNetVmMem);
        int32 FreeSegmentForVnic(CVNetVmMem & cVNetVmMem);
        int32 GetSegmentDetail(int64 nProjID, vector<CVNetVnicDetail> &vecVnicDetail);
        int32 IsExistSegmentRange(const string &cstrNetplaneID);
        int32 IsInSegmentMapRanges(uint32 dwSegmentNum, const string &cstrNetplaneID); 
        int32 IsRangeCrossRanges(uint32 dwSegmentStart, uint32 dwSegmentEnd, const string &cstrNetplaneID);
        
    public:
        int32 Init(MessageUnit *pMu);
        void MessageEntry(const MessageFrame &message);
        
        void SetDbgInfFlg(BOOL bOpen){ m_bOpenDbgInf = bOpen; return ;}
        int32 DbgShowSegmentPool(const string &cstrLogicNetworkID);
        
    protected:
        int32 ProcSegmentRangeMsg(const MessageFrame &message);        
    
    private:
        int32 AddSegmentRange(const CSegmentRangeMsg &cMsg, string &strRetInfo);
        int32 DelSegmentRange(const CSegmentRangeMsg &cMsg, string &strRetInfo);
        int32 GetSegmentRange(const string &cstrNetplaneID, CSegmentRange &cSegmentRange);
        int32 IsConflictWithShareNetSegment(const CSegmentRangeMsg &cMsg, int32 &nConflictSegment);
        int32 GetIsolateNo(const CSegmentRangeMsg &cMsg, vector<int32> &vecIsolateNo);
        
    private:

    private:
        static CSegmentPoolMgr *s_pInstance;

        MessageUnit *m_pMU;

        map<string, CSegmentPoolMgr *> m_mapSegmentPool;        
        BOOL m_bOpenDbgInf;
        DISALLOW_COPY_AND_ASSIGN(CSegmentPoolMgr);
    };
}// namespace zte_tecs

#endif

