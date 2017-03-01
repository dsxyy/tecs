/******************************************************************************
* Copyright (c) 2012������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�vnm_vm_db.h
* �ļ���ʶ��
* ����ժҪ��CVNetVmDB��Ķ����ļ�
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
#if !defined(VNM_VM_DB_INCLUDE_H__)
#define VNM_VM_DB_INCLUDE_H__

namespace zte_tecs
{


class CVNetVmDB
{
public:
    explicit CVNetVmDB();
    virtual ~CVNetVmDB();

    int32 Init() ;
    int32 GetVm(CDbVm &cDbVm);
    int32 GetVm(CVNetVmMem &cVmMem);
    int32 GetVm(CVMVSIInfo &cVMVSIInfo);
    int32 InsertVm(CDbVm &cDbVm);
    int32 InsertVm(CVNetVmMem &cVnetVmMem);
    int32 UpdateVm(CDbVm &cDbVm);
    int32 UpdateVm(CVNetVmMem &cVnetVmMem);
    int32 DeleteVm(const int64 &nVmID);
    int32 QueryAllVm( vector<CDbVm> & vecAllVm);

    static CVNetVmDB* GetInstance()
    {
        if (NULL == _instance)
        {
            _instance = new CVNetVmDB(); 
        }
        
        return _instance;
    };
    
private:
    static CVNetVmDB *_instance;
    CVNetDbMgr * m_pVNetDbMgr;
    CDBOperateVm * m_pDbOper;

    DISALLOW_COPY_AND_ASSIGN(CVNetVmDB);

};

}
#endif

