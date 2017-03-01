/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：vlan_range.h
* 文件标识：
* 内容摘要：CVlanRange类的定义文件
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

#if !defined(VLAN_RANGE_INCLUDE_H__)
#define VLAN_RANGE_INCLUDE_H__

namespace zte_tecs
{
    typedef struct tagVlanRangeItem
    {
        uint32 dwVlanIDStart;
        uint32 dwVlanIDEnd;
    }TVlanRangeItem;

    class CVlanRange
    {
    public:
        explicit CVlanRange();
        virtual ~CVlanRange();

    public:  
        int32 AddValidRange(const TVlanRangeItem &tVlanRange);
        int32 AddRange(const TVlanRangeItem &tVlanRange);
        int32 DelRange(const TVlanRangeItem &tVlanRange);  
        int32 RemoveAll();
       
        vector<TVlanRangeItem> &GetVlanRange(void) {return m_vecVlanRange;}       
        
        void  DbgShowVlanRange(void);
        
    protected:
        int32 IsValidVlanRange(const TVlanRangeItem &tVlanRange);
        
    private:
        
    private:
        vector<TVlanRangeItem> m_vecVlanRange;
        DISALLOW_COPY_AND_ASSIGN(CVlanRange);
    };
}// namespace zte_tecs

#endif

