

#include "vnet_db_switch.h"
#include "vnet_db_error.h"
#include "vnet_db_proc.h"

namespace zte_tecs
{
int32 CDBOperateSwitchBase::QuerySwitchBaseCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateSwitchBase::QuerySwitchBaseCallback prs is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
        
    CDbSwitch *pOutPort;
    pOutPort = static_cast<CDbSwitch *>(nil);

    if( pOutPort == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateSwitchBase::QuerySwitchBaseCallback pOutPort is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    //VNET_ASSERT(prs->GetFieldCount() == 14);    

    int32 proc_ret = 0;
    if( false == prs->GetFieldValue(0,proc_ret ))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateSwitchBase::QuerySwitchBaseCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }    

    if( 0 != proc_ret )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateSwitchBase::QuerySwitchBaseCallback proc failed(%d).\n", GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }

    (*pOutPort).SetName( prs->GetFieldValue(1));
    (*pOutPort).SetUuid( prs->GetFieldValue(2));
    
    int32 vlaue = 0;
    if( false == prs->GetFieldValue(3,vlaue) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateSwitchBase::QuerySwitchBaseCallback GetFieldValue 3 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetSwitchType(vlaue);

    if( false == prs->GetFieldValue(4,vlaue) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateSwitchBase::QuerySwitchBaseCallback GetFieldValue 4 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetState(vlaue);

    if( false == prs->GetFieldValue(5,vlaue) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateSwitchBase::QuerySwitchBaseCallback GetFieldValue 5 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetMaxMtu(vlaue);
    
    return 0;
}
}
