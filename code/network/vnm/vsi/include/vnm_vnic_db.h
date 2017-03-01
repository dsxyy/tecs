/******************************************************************************
* Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：vnm_vnic_db.h
* 文件标识：
* 内容摘要：CVNetVnicDB类的定义文件
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

