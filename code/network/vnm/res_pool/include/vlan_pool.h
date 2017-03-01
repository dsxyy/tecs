/******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�vlan_pool.h
* �ļ���ʶ��
* ����ժҪ��CVlanPool��Ķ����ļ�
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

#if !defined(VLAN_POOL_INCLUDE_H__)
#define VLAN_POOL_INCLUDE_H__

namespace zte_tecs
{
    typedef struct tagVlanMap
    {
        int64 nProjID;
        string strPortGroupID;
        uint32 dwVlanNum; // vlan ֵ
    }TVlanMap;
    
    class CVlanPool
    {
    public:
        explicit CVlanPool();
        virtual ~CVlanPool();

    public:
        int32 Init(const string &cstrNetplaneID);
        int32 Init(CVlanRange *pVlanRange, const string &cstrNetplaneID);
        int32 AllocVlanID(const string &cstrNetplaneID, 
                          int64 nProjID, 
                          const string &cstrPortGroupID, 
                          int32 nIsolationNo, 
                          uint32 &dwAllocVlanID);
        int32 FreeVlanID(int64 nProjID, 
                         const string &cstrPortGroupID, 
                         int32 nIsolationNo, 
                         uint32 &dwFreeVlanID);
        int32 AddVlanRange(void);
        int32 DelVlanRange(void);
        
    protected:        
        
    private:        

    private:
        vector<uint8> m_vecVlanStatus; /* vector.size() == 4096; */
        CVlanRange *m_pVlanRange;
        string m_strNetplaneID;
        DISALLOW_COPY_AND_ASSIGN(CVlanPool);
    };
}// namespace zte_tecs

#endif

