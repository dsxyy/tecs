/******************************************************************************
* Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：vnet_vsi_mgr.h
* 文件标识：
* 内容摘要：CVSIProfileMgr类的定义文件
* 当前版本：1.0
* 作    者：
* 完成日期：2013年2月22日
*
* 修改记录1：
*     修改日期：2013/2/22
*     版 本 号：V1.0
*     修 改 人：lverchun
*     修改内容：创建
******************************************************************************/
#if !defined(VNM_VSI_MGR_INCLUDE_H__)
#define VNM_VSI_MGR_INCLUDE_H__

#include "segment_pool_mgr.h"

namespace zte_tecs
{

class CVSIProfileMgr
{
public:
    explicit CVSIProfileMgr();
    virtual ~CVSIProfileMgr();
    
    int32 Init();
    int GetVSIResForVnic(vector<CVNetVnicMem> &vecNicMem);
    int AllocVSIResForVnic(CVNetVmMem &cVnetVmMem);
    int FreeVSIResForVnic(CVNetVmMem &cVnetVmMem);
    int32 GetVSIDetailInfo(int64 nVmID, CVMVSIInfo &cVMVSIInfo);
    int32 GetVSIDetailInfo(string strVNAUUID,int64 nVmID, CVMVSIInfo &cVMVSIInfo);
    void SetVSIDetailFromVnicMem(vector<CVNetVnicMem> &vecNicMem, vector<CVNetVnicDetail> &vecVnicDetail);
    void DbgShowData(void);

    static CVSIProfileMgr* GetInstance()
    {
        if (NULL == _instance)
        {
            _instance = new CVSIProfileMgr(); 
        }
        
        return _instance;
    };
    
private:    
    static CVSIProfileMgr *_instance;
    CMACPoolMgr * m_pMACPoolMgr;
    CVlanPoolMgr * m_pVlanPoolMgr;
    CSegmentPoolMgr * m_pSegmentPoolMgr;
    CIPv4PoolMgr * m_pIPv4PoolMgr;
    CPortGroupMgr * m_pPortGroupMgr;
    CSwitchMgr * m_pSwitchMgr;
    
    DISALLOW_COPY_AND_ASSIGN(CVSIProfileMgr);
};

}
#endif

