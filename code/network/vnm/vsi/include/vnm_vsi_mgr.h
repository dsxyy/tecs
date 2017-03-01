/******************************************************************************
* Copyright (c) 2012������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�vnet_vsi_mgr.h
* �ļ���ʶ��
* ����ժҪ��CVSIProfileMgr��Ķ����ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�
* ������ڣ�2013��2��22��
*
* �޸ļ�¼1��
*     �޸����ڣ�2013/2/22
*     �� �� �ţ�V1.0
*     �� �� �ˣ�lverchun
*     �޸����ݣ�����
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

