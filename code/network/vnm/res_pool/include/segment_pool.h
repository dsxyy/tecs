/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�segment_pool.h
* �ļ���ʶ��
* ����ժҪ��CSegmentPool��Ķ����ļ�
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

#if !defined(SEGMENT_POOL_INCLUDE_H__)
#define SEGMENT_POOL_INCLUDE_H__

namespace zte_tecs
{
    typedef struct tagSegmentMap
    {
        int64 nProjID;
        string strPortGroupID;
        uint32 dwSegmentID; // Segment ֵ
    }TSegmentMap;
    
    class CSegmentPool
    {
    public:
        explicit CSegmentPool();
        virtual ~CSegmentPool();

    public:
        int32 Init(const string &cstrNetplaneID);
        int32 Init(CSegmentRange *pSegmentRange, const string &cstrNetplaneID);
        int32 AllocSegmentID(const string &cstrNetplaneID, 
                          int64 nProjID, 
                          const string &cstrPortGroupID, 
                          int32 nIsolationNo, 
                          uint32 &dwAllocSegmentID);
        int32 FreeSegmentID(int64 nProjID, 
                         const string &cstrPortGroupID, 
                         int32 nIsolationNo, 
                         uint32 &dwFreeSegmentID);
        int32 AddSegmentRange(void);
        int32 DelSegmentRange(void);
        
    protected:        
        
    private:        

    private:
        vector<uint8> m_vecSegmentStatus; /* vector.size() == 16000000; */
        CSegmentRange *m_pSegmentRange;
        string m_strNetplaneID;
        DISALLOW_COPY_AND_ASSIGN(CSegmentPool);
    };
}// namespace zte_tecs

#endif

