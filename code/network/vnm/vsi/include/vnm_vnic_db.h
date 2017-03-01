/******************************************************************************
* Copyright (c) 2012������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�vnm_vnic_db.h
* �ļ���ʶ��
* ����ժҪ��CVNetVnicDB��Ķ����ļ�
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
#if !defined(VNM_VNIC_DB_INCLUDE_H__)
#define VNM_VNIC_DB_INCLUDE_H__

namespace zte_tecs
{


class CVNetVnicDB
{
public:
    explicit CVNetVnicDB();
    virtual ~CVNetVnicDB();

    int32 Init() ;
    int32 GetVnic(CVNetVnicMem &cNicMem);
    int32 GetVmVnic(int64 nVmID, vector<CVNetVnicMem> &vecNicMem);
	int32 GetVmVnic(int64 nVmID, vector<CVNetVnicDetail> &vecNicDetail);
	int32 GetVmVnicFromDb(int64 nVmID, vector<CVNetVnicDetail> &vecNicDetail);
    int32 InsertVnic(CVNetVnicMem &cNicMem);
    int32 InsertVmVnic(vector<CVNetVnicMem> &vecNicMem);
    int32 UpdateVnic(const CVNetVnicMem &cNicMem);
    int32 UpdateVmVnic( vector<CVNetVnicMem> &vecNicMem);
    int32 DeleteVnic(const string &strVsiUuid);
    int32 DeleteVmVnic(int64 vm_id);
    int32 QueryAllVnic( vector<CDbVsiInfoSummary> & vecAllVnic);

    static CVNetVnicDB* GetInstance()
    {
        if (NULL == _instance)
        {
            _instance = new CVNetVnicDB(); 
        }
        
        return _instance;
    };
    
private:
    static CVNetVnicDB *_instance;
    CVNetDbMgr * m_pVNetDbMgr;
	CDBOperateVsi * m_pOper;

    DISALLOW_COPY_AND_ASSIGN(CVNetVnicDB);

};

}
#endif

