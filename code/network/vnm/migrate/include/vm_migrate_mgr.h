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

