/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�vlan_range.h
* �ļ���ʶ��
* ����ժҪ��CVlanRange��Ķ����ļ�
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

