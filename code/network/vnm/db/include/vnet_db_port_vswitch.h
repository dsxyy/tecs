/******************************************************************************
* Copyright (c) 2013，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：vnet_db_netplane.h
* 文件标识：
* 内容摘要：CVNetDb类的定义文件
* 当前版本：1.0
* 作    者：
* 完成日期：2013年1月28日
*
* 修改记录1：
*     修改日期：2013/1/28
*     版 本 号：V1.0
*     修 改 人：gong.xiefeng
*     修改内容：创建
*
******************************************************************************/

#if !defined(_VNET_DB_PORT_VSWITCH_INCLUDE_H_)
#define _VNET_DB_PORT_VSWITCH_INCLUDE_H_

#include "vnet_db_in.h"
#include "vnet_db_interface.h"
#include "vnet_db_proc.h"

#include "vnet_db_port.h"

namespace zte_tecs
{

// port db

class CVNetDbIProcedure;
class CDBOperatePortVSwitch : public  CVNetDbIProcedureCallback, public CDBOperatePortBase
{
public:
    explicit CDBOperatePortVSwitch(CVNetDbIProcedure * pb);
    virtual ~CDBOperatePortVSwitch();

    int32 QuerySummary(const char* vna_uuid, vector<CDbPortSummary> & outVInfo);  
    int32 Query(CDbPort & Info);
    int32 Add(CDbPort & info);
    int32 Modify(CDbPort & info);
    int32 Del(const char* port_uuid);

    int32 DbProcCallback(int32 type, CADORecordset *, void* );

    void DbgShow();    
private:
    int32 QuerySummaryCallback(CADORecordset * prs,void * nil);
    int32 QueryCallback(CADORecordset * prs,void * nil);
    int32 OperateCallback(CADORecordset * prs,void * nil);
    
private:
    DISALLOW_COPY_AND_ASSIGN(CDBOperatePortVSwitch);

    CVNetDbIProcedure * m_pIProc;
};
    
} // namespace zte_tecs

#endif // _VNET_DB_PORT_VSWITCH_INCLUDE_H_ 


