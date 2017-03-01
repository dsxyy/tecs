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
#if !defined(VNM_VM_MIGRATE_INCLUDE_H__)
#define VNM_VM_MIGRATE_INCLUDE_H__

#include "vnetlib_msg.h"

namespace zte_tecs
{


class CVNetVmMigrateMgr
{
public:
    explicit CVNetVmMigrateMgr();
    virtual ~CVNetVmMigrateMgr();

    int32 Init() ;
    
    int32 MigrateBegin(CVNetVmMigrateBeginMsg &msg);
    int32 MigrateEnd(CVNetVmMigrateEndMsg &msg);
    void SetDbgPrint(int32 i) {m_dbg = i;};

    static CVNetVmMigrateMgr* GetInstance()
    {
        if (NULL == _instance)
        {
            _instance = new CVNetVmMigrateMgr(); 
        }
        
        return _instance;
    };
private:
    int32 UpdateVnic(CVNetVmMigrateEndMsg &msg);    
    
private:
    static CVNetVmMigrateMgr *_instance;
    CDBOperateVmMigrate * m_pVNetDbMgr;
    int32   m_dbg;

    DISALLOW_COPY_AND_ASSIGN(CVNetVmMigrateMgr);

};

}
#endif

