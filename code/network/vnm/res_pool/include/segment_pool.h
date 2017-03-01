/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：segment_pool.h
* 文件标识：
* 内容摘要：CSegmentPool类的定义文件
* 当前版本：1.0
* 作    者：
* 完成日期：2013年09月05日
*
* 修改记录1：
*     修改日期：2013/09/05
*     版 本 号：V1.0
*     修 改 人：zhao.jin
*     修改内容：创建
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
        uint32 dwSegmentID; // Segment 值
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

