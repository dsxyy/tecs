/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：vlan_pool.h
* 文件标识：
* 内容摘要：CVlanPool类的定义文件
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

#if !defined(VLAN_POOL_INCLUDE_H__)
#define VLAN_POOL_INCLUDE_H__

namespace zte_tecs
{
    typedef struct tagVlanMap
    {
        int64 nProjID;
        string strPortGroupID;
        uint32 dwVlanNum; // vlan 值
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

