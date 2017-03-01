/******************************************************************************
* Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：vnm_vm_db.h
* 文件标识：
* 内容摘要：CVNetVmDB类的定义文件
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

