

#include "vnet_db_error.h"
#include "vnet_db_proc.h"
#include "vnet_db_mgr.h"
#include "vnet_db_wildcast_create_vport.h"
    
namespace zte_tecs
{
typedef enum tagWildcastCreateVPortProc
{
    EN_WILDCAST_CREATE_VPORT_PROC_QUERYSUMMARY = 0,
    EN_WILDCAST_CREATE_VPORT_PROC_QUERY,   
    EN_WILDCAST_CREATE_VPORT_PROC_CHECK_ADD,
    EN_WILDCAST_CREATE_VPORT_PROC_ADD,
    EN_WILDCAST_CREATE_VPORT_PROC_CHECK_DEL,
    EN_WILDCAST_CREATE_VPORT_PROC_DEL,
    EN_WILDCAST_CREATE_VPORT_PROC_ALL,
}EN_WILDCAST_CREATE_VPORT_PROC;

const char *s_astrWildcastCreateVPortProc[EN_WILDCAST_CREATE_VPORT_PROC_ALL] = 
{        
    "pf_net_wildcast_query_create_vport_summary",
    "pf_net_wildcast_query_create_vport",
    "pf_net_check_add_wildcast_create_vport",  
    "pf_net_add_wildcast_create_vport",    
    "pf_net_check_del_wildcast_create_vport",  
    "pf_net_del_wildcast_create_vport",  
};

    
CDBOperateWildcastCreateVPort::CDBOperateWildcastCreateVPort(CVNetDbIProcedure * pProc)
{
    m_pIProc = pProc;
}

CDBOperateWildcastCreateVPort::~CDBOperateWildcastCreateVPort()
{
    
}

int32 CDBOperateWildcastCreateVPort::DbProcCallback(int type, CADORecordset *prs, void* nil )
{
    switch(type)
    {
        case EN_WILDCAST_CREATE_VPORT_PROC_QUERYSUMMARY:
        {
            return QuerySummaryCallback(prs,nil);
        }
        break;
        case EN_WILDCAST_CREATE_VPORT_PROC_QUERY:
        {
            return QueryCallback(prs,nil);
        }
        break;
        case EN_WILDCAST_CREATE_VPORT_PROC_ADD:
        //case EN_WILDCAST_CREATE_VPORT_PROC_MODIFY:
        case EN_WILDCAST_CREATE_VPORT_PROC_DEL:
        {
            return OperateCallback(prs,nil);
        }
        break;
        
        case EN_WILDCAST_CREATE_VPORT_PROC_CHECK_ADD:
        //case EN_WILDCAST_CREATE_VPORT_PROC_CHECK_MODIFY:
        case EN_WILDCAST_CREATE_VPORT_PROC_CHECK_DEL:
        {
            return CheckOperateCallback(prs,nil);
        }   
        break;
        default:
        {
            VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastCreateVPort::DbProcCallback type(%d) is invalid.\n",type);
            return VNET_DB_ERROR_PARAM_INVALID;
        }
        break;
    }
    
    return 0;
}
    
int32 CDBOperateWildcastCreateVPort::QuerySummary(vector<CDbWildcastCreateVPort> & outVInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastCreateVPort::QuerySummary IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }


    // call procedure 
    vector<CADOParameter>  vInParam;
    
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,\
        s_astrWildcastCreateVPortProc[EN_WILDCAST_CREATE_VPORT_PROC_QUERYSUMMARY],\
        vInParam, \
        this,\
        EN_WILDCAST_CREATE_VPORT_PROC_QUERYSUMMARY,\
        static_cast<void*> (&outVInfo));
        
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastCreateVPort::QuerySummary IProc->Handle failed(%d) \n",ret);
        return ret;
    }
        
    return 0;
}

int32 CDBOperateWildcastCreateVPort::QuerySummaryCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastCreateVPort::QuerySummaryCallback prs is NULL.\n");
        //VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }

    vector<CDbWildcastCreateVPort> *pvOut;
    pvOut = static_cast<vector<CDbWildcastCreateVPort>*>(nil);

    if( pvOut == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastCreateVPort::QuerySummaryCallback pvOut is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    CDbWildcastCreateVPort info;

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 8);
    
    info.SetUuid(prs->GetFieldValue(0));
    info.SetPortName(prs->GetFieldValue(1));   
    int32 value = 0;
    if( false == prs->GetFieldValue(2,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastCreateVPort::QuerySummaryCallback GetFieldValue 2 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetPortType(value);

    if( false == prs->GetFieldValue(3,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastCreateVPort::QuerySummaryCallback GetFieldValue 3 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetIsHasKernelType(value);


    if( false == prs->GetFieldValue(4,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastCreateVPort::QuerySummaryCallback GetFieldValue 4 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetKernelType(value);

    info.SetKernelPgUuid(prs->GetFieldValue(5));

    info.SetSwitchUuid(prs->GetFieldValue(6));

    if( false == prs->GetFieldValue(7,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastCreateVPort::QuerySummaryCallback GetFieldValue 7 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetIsDhcp(value);

    (*pvOut).push_back(info);

    return 0;
}

int32 CDBOperateWildcastCreateVPort::Query(CDbWildcastCreateVPort & Info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastCreateVPort::Query IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    vector<CADOParameter> vInParam;
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)Info.GetUuid().c_str());
    vInParam.push_back(inUuid);
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrWildcastCreateVPortProc[EN_WILDCAST_CREATE_VPORT_PROC_QUERY],
        vInParam, 
        this,
        EN_WILDCAST_CREATE_VPORT_PROC_QUERY,
        static_cast<void*> (&Info));
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            Info.DbgShow();
        }
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastCreateVPort::Query IProc->Handle failed(%d).\n",ret);
        }
        return ret;
    }
    
    return 0;
}

int32 CDBOperateWildcastCreateVPort::QueryCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastCreateVPort::QueryCallback prs is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    if( nil == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastCreateVPort::QueryCallback nil is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }
    
    // 2 获取kernel 信息
    CDbWildcastCreateVPort *pOutPort = NULL;
    pOutPort = static_cast<CDbWildcastCreateVPort *>(nil);
    if( pOutPort == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastCreateVPort::QueryCallback pOutPort is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }
    
    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 9);    

    int32 proc_ret = 0;
    if( false == prs->GetFieldValue(0,proc_ret ))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastCreateVPort::QueryCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }    

    if( 0 != proc_ret )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastCreateVPort::QueryCallback proc failed(%d).\n", GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }

    (*pOutPort).SetUuid( prs->GetFieldValue(1));
    (*pOutPort).SetPortName( prs->GetFieldValue(2));

    
    int32 value = 0;
    if( false == prs->GetFieldValue(3,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastCreateVPort::QueryCallback GetFieldValue 3 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetPortType(value);

    if( false == prs->GetFieldValue(4,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastCreateVPort::QueryCallback GetFieldValue 4 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetIsHasKernelType(value);

    if( false == prs->GetFieldValue(5,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastCreateVPort::QueryCallback GetFieldValue 5 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetKernelType(value);
    

    (*pOutPort).SetKernelPgUuid(prs->GetFieldValue(6));

    (*pOutPort).SetSwitchUuid(prs->GetFieldValue(7));

    if( false == prs->GetFieldValue(8,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastCreateVPort::QueryCallback GetFieldValue 8 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetIsDhcp(value);
    
    return 0;
}

int32 CDBOperateWildcastCreateVPort::CheckAdd(CDbWildcastCreateVPort & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastCreateVPort::CheckAdd IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // uuid generate
    info.SetUuid(GenerateUUID().c_str());
    
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUuid().c_str());    
    CADOParameter inName(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetPortName().c_str());
    CADOParameter inPortType(CADOParameter::paramInt, CADOParameter::paramInput, info.GetPortType()); 
    CADOParameter inIsHasKernelType(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsHasKernelType()); 
    CADOParameter inKernelType(CADOParameter::paramInt, CADOParameter::paramInput, info.GetKernelType());     
    CADOParameter inKernelPgUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetKernelPgUuid().c_str());     
    CADOParameter inSwitchUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetSwitchUuid().c_str());     
    CADOParameter inIsDhcp(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsDhcp()); 
    
    vInParam.push_back(inUuid);
    vInParam.push_back(inName);
    vInParam.push_back(inPortType);
    vInParam.push_back(inIsHasKernelType);
    vInParam.push_back(inKernelType);    
    vInParam.push_back(inKernelPgUuid);
    vInParam.push_back(inSwitchUuid);    
    vInParam.push_back(inIsDhcp);      
  
    // ret struct 
    int32 proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrWildcastCreateVPortProc[EN_WILDCAST_CREATE_VPORT_PROC_CHECK_ADD],
        vInParam, 
        this,
        EN_WILDCAST_CREATE_VPORT_PROC_CHECK_ADD,
        static_cast<void*> (&proc_ret));
    
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastCreateVPort::CheckAdd IProc->Handle failed(%d).\n", ret);
        return ret;
    }
    
    // 重复添加 
    if( proc_ret == VNET_DB_PROCEDURE_RTN_HAS_SAME_ITEM )
    {
        return VNET_DB_SUCCESS;
    }

    if( proc_ret  != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastCreateVPort::CheckAdd proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }

    return VNET_DB_SUCCESS;
}

int32 CDBOperateWildcastCreateVPort::Add(CDbWildcastCreateVPort & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastCreateVPort::Add IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }
   
    // uuid generate
    info.SetUuid(GenerateUUID().c_str());
    
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;     
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUuid().c_str());    
    CADOParameter inName(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetPortName().c_str());
    CADOParameter inPortType(CADOParameter::paramInt, CADOParameter::paramInput, info.GetPortType()); 
    CADOParameter inIsHasKernelType(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsHasKernelType()); 
    CADOParameter inKernelType(CADOParameter::paramInt, CADOParameter::paramInput, info.GetKernelType());     
    CADOParameter inKernelPgUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetKernelPgUuid().c_str());     
    CADOParameter inSwitchUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetSwitchUuid().c_str());     
    CADOParameter inIsDhcp(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsDhcp()); 
    
    vInParam.push_back(inUuid);
    vInParam.push_back(inName);
    vInParam.push_back(inPortType);
    vInParam.push_back(inIsHasKernelType);
    vInParam.push_back(inKernelType);  
    vInParam.push_back(inKernelPgUuid);
    vInParam.push_back(inSwitchUuid);  
    vInParam.push_back(inIsDhcp); 
  
    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrWildcastCreateVPortProc[EN_WILDCAST_CREATE_VPORT_PROC_ADD],
        vInParam, 
        this,
        EN_WILDCAST_CREATE_VPORT_PROC_ADD,
        static_cast<void*> (&proc_ret));
    
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastCreateVPort::Add IProc->Handle failed(%d).\n", ret);
        return ret;
    }
    
    // 重复添加 
    if( proc_ret.GetRet() == VNET_DB_PROCEDURE_RTN_HAS_SAME_ITEM )
    {
        info.SetUuid(proc_ret.GetUuid().c_str());
        return VNET_DB_SUCCESS;
    }

    if( proc_ret.GetRet() != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastCreateVPort::Add proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }

    info.SetUuid(proc_ret.GetUuid().c_str());

    return VNET_DB_SUCCESS;
}

#if 0
int32 CDBOperateWildcastCreateVPort::CheckModify(CDbWildcastCreateVPort & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastCreateVPort::ModifyPort IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUuid().c_str());    
    CADOParameter inName(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetPortName().c_str());
    CADOParameter inPortType(CADOParameter::paramInt, CADOParameter::paramInput, info.GetPortType()); 
    CADOParameter inIsHasKernelType(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsHasKernelType()); 
    CADOParameter inKernelType(CADOParameter::paramInt, CADOParameter::paramInput, info.GetKernelType());     
    
    vInParam.push_back(inUuid);
    vInParam.push_back(inName);
    vInParam.push_back(inPortType);
    vInParam.push_back(inIsHasKernelType);
    vInParam.push_back(inKernelType); 

    // ret struct 
    int32 proc_ret = 0; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrWildcastCreateVPortProc[EN_WILDCAST_CREATE_VPORT_PROC_CHECK_MODIFY],
        vInParam, 
        this,
        EN_WILDCAST_CREATE_VPORT_PROC_CHECK_MODIFY,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastCreateVPort::ModifyPort IProc->Handle failed(%d).\n", ret);
        return ret;
    }

    //修改内容和原纪录内容相同 
    if( proc_ret == VNET_DB_PROCEDURE_RTN_HAS_SAME_ITEM )
    {
        return VNET_DB_SUCCESS;
    }

    if( proc_ret != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastCreateVPort::ModifyPort proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }
    
    return VNET_DB_SUCCESS;
}

int32 CDBOperateWildcastCreateVPort::Modify(CDbWildcastCreateVPort & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastCreateVPort::Modify IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUuid().c_str());    
    CADOParameter inName(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetPortName().c_str());
    CADOParameter inPortType(CADOParameter::paramInt, CADOParameter::paramInput, info.GetPortType()); 
    CADOParameter inIsHasKernelType(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsHasKernelType()); 
    CADOParameter inKernelType(CADOParameter::paramInt, CADOParameter::paramInput, info.GetKernelType());     
    
    vInParam.push_back(inUuid);
    vInParam.push_back(inName);
    vInParam.push_back(inPortType);
    vInParam.push_back(inIsHasKernelType);
    vInParam.push_back(inKernelType); 

    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrWildcastCreateVPortProc[EN_WILDCAST_CREATE_VPORT_PROC_MODIFY],
        vInParam, 
        this,
        EN_WILDCAST_CREATE_VPORT_PROC_MODIFY,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastCreateVPort::Modify IProc->Handle failed(%d).\n", ret);
        return ret;
    }

    //修改内容和原纪录内容相同 
    if( proc_ret.GetRet() == VNET_DB_PROCEDURE_RTN_HAS_SAME_ITEM )
    {
        return VNET_DB_SUCCESS;
    }

    if( proc_ret.GetRet() != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastCreateVPort::Modify proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }
    
    return VNET_DB_SUCCESS;
}
#endif 

int32 CDBOperateWildcastCreateVPort::CheckDel(const char* uuid)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastCreateVPort::CheckDel IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == uuid )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastCreateVPort::CheckDel uuid is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;    
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)uuid);    
    
    vInParam.push_back(inUuid);

    // ret struct 
    int32 proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrWildcastCreateVPortProc[EN_WILDCAST_CREATE_VPORT_PROC_CHECK_DEL],
        vInParam, 
        this,
        EN_WILDCAST_CREATE_VPORT_PROC_CHECK_DEL,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastCreateVPort::CheckDel(%s) IProc->Handle failed. ret : %d.\n",
            uuid,ret);
        return ret;
    }
    
    if( proc_ret != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastCreateVPort::CheckDel(%s) proc return failed. ret : %d.\n", 
            uuid,GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }
    
    return 0;
}


int32 CDBOperateWildcastCreateVPort::Del(const char* uuid)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastCreateVPort::Del IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == uuid )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastCreateVPort::Del uuid is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;    
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)uuid);    
    
    vInParam.push_back(inUuid);

    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrWildcastCreateVPortProc[EN_WILDCAST_CREATE_VPORT_PROC_DEL],
        vInParam, 
        this,
        EN_WILDCAST_CREATE_VPORT_PROC_DEL,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastCreateVPort::Del(%s) IProc->Handle failed. ret : %d.\n",
            uuid,ret);
        return ret;
    }
    
    if( proc_ret.GetRet() != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastCreateVPort::Del(%s) proc return failed. ret : %d.\n",
            uuid,GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }
    
    return 0;
}

int32 CDBOperateWildcastCreateVPort::OperateCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastCreateVPort::OperateCallback param is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    
    CDbProcOperateResult *proc_ret = NULL;     
    proc_ret = static_cast<CDbProcOperateResult *>(nil);

    if( proc_ret == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastCreateVPort::OperateCallback proc_ret is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 3);    

    int32 nRet = 0;
    if( false == prs->GetFieldValue(0, nRet))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastCreateVPort::OperateCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetRet(nRet);

    int64 oid = 0;
    if( false == prs->GetFieldValue(1, oid))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastCreateVPort::OperateCallback GetFieldValue(1) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetOid(oid);
    
    (*proc_ret).SetUuid(prs->GetFieldValue(2)); 

    return 0;
}

int32 CDBOperateWildcastCreateVPort::CheckOperateCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastCreateVPort::CheckOperateCallback param is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    
    int32 *proc_ret = NULL;     
    proc_ret = static_cast<int32 *>(nil);

    if( proc_ret == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastCreateVPort::CheckOperateCallback proc_ret is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 1);    

    int32 nRet = 0;
    if( false == prs->GetFieldValue(0, nRet))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastCreateVPort::CheckOperateCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret) = nRet;

    return 0;
}

void CDBOperateWildcastCreateVPort::DbgShow()
{
#if 0
    cout << "--------------------------Port Info------------------------" << endl;
    // query summary
    vector<CDbWildcastCreateVPort> vInfo;
    int ret = 0;
    ret = QuerySummary(vInfo);
    if( ret != 0 )
    {
        cout << "QuerySummary failed. ret : " << ret << endl;
        return;
    }

    cout << "QuerySummary success " << endl;
    cout << "|" <<setw(32) << "uuid" << 
                    "|" << setw(32) << "name" << endl;
    vector<CDbWildcastCreateVPort>::iterator it = vInfo.begin();
    for(; it != vInfo.end(); ++it)
    {
        (*it).DbgShow();
    }
    
    // query 
    cout << "-----------------------------------" << endl;
    it = vInfo.begin();
    for(; it != vInfo.end(); ++it)
    {
        CDbPort info;
        info.SetUuid((*it).GetUuid().c_str());
        ret = Query(info);
        if( ret != 0 )
        {
            cout << "Query failed. uuid : " << info.GetUuid() << " ret : " << ret << endl;
            continue;
        }

        info.DbgShow();
        cout << "-----------------" << endl;        
    }
#endif     
}

/* test code ------------------------------------------------------------

*/

void db_query_wc_create_vport_summary()
{    
    CDBOperateWildcastCreateVPort * pOper = GetDbIWildcastCreateVPort();
    if( NULL == pOper )
    {
        cout << "CDBOperateWildcastCreateVPort is NULL" << endl;
        return ;
    }

    vector<CDbWildcastCreateVPort> outVInfo;
    int ret = pOper->QuerySummary(outVInfo);
    if( ret != 0 )
    {
        cout << "QuerySummary failed." << ret << endl;
        return;
    }
    cout << "QuerySummary success." << endl;
    vector<CDbWildcastCreateVPort>::iterator it = outVInfo.begin();
    for(; it != outVInfo.end(); ++it)
    {
        cout << "------------" << endl;
        (*it).DbgShow();
    }    
}
DEFINE_DEBUG_FUNC(db_query_wc_create_vport_summary);

void db_query_wc_create_vport(char * uuid)
{
    if( NULL == uuid)
    {
        cout << "Param uuid is NULL" << endl;
        return;
    }
    CDBOperateWildcastCreateVPort * pOper = GetDbIWildcastCreateVPort();
    if( NULL == pOper )
    {
        cout << "CDBOperateWildcastCreateVPort is NULL" << endl;
        return ;
    }

    CDbWildcastCreateVPort info;
    info.SetUuid(uuid);  
    int ret = pOper->Query(info);
    if( ret != 0 )
    {
        cout << "CDBOperateWildcastCreateVPort query failed." << ret << endl;
        return;
    }
    cout << "CDBOperateWildcastCreateVPort query success." << endl;
    info.DbgShow();
    
}
DEFINE_DEBUG_FUNC(db_query_wc_create_vport);


void db_check_add_wc_create_vport(char* uuid,char* name, int32 port_type,
       int32 is_has_kernel, int32 kernel_type, char * pg_uuid, char* switch_uuid )
{
    if( NULL == uuid || NULL == name || NULL == pg_uuid || NULL == switch_uuid )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateWildcastCreateVPort * pOper = GetDbIWildcastCreateVPort();
    if( NULL == pOper )
    {
        cout << "CDBOperateWildcastCreateVPort is NULL" << endl;
        return ;
    }

    CDbWildcastCreateVPort info;
    info.SetPortName(name);
    info.SetUuid(uuid);
    info.SetPortType(port_type);
    info.SetIsHasKernelType(is_has_kernel);
    info.SetKernelType(kernel_type);    
    info.SetKernelPgUuid(pg_uuid); 
    info.SetSwitchUuid(switch_uuid); 

    //info.DbgShow();
    int ret = pOper->CheckAdd(info);
    if( ret != 0 )
    {
        cout << "CheckAdd failed." << ret << endl;
        return;
    }
    cout << "CheckAdd success." << endl;
    info.DbgShow();
}
DEFINE_DEBUG_FUNC(db_check_add_wc_create_vport);


void db_add_wc_create_vport(char* uuid,char* name, int32 port_type,
       int32 is_dhcp, int32 kernel_type , char * pg_uuid, char* switch_uuid)
{
    if( NULL == uuid || NULL == name  )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateWildcastCreateVPort * pOper = GetDbIWildcastCreateVPort();
    if( NULL == pOper )
    {
        cout << "CDBOperateWildcastCreateVPort is NULL" << endl;
        return ;
    }

    CDbWildcastCreateVPort info;
    info.SetPortName(name);
    info.SetUuid(uuid);
    info.SetPortType(port_type);
    if( port_type == 3 )
        info.SetIsHasKernelType(true );
    else
        info.SetIsHasKernelType(false);
    info.SetKernelType(kernel_type);    
    info.SetKernelPgUuid(pg_uuid); 
    info.SetSwitchUuid(switch_uuid);    
    info.SetIsDhcp(is_dhcp);        


    //info.DbgShow();
    int ret = pOper->Add(info);
    if( ret != 0 )
    {
        cout << "Add failed." << ret << endl;
        return;
    }
    cout << "Add success." << endl;
    info.DbgShow();
}
DEFINE_DEBUG_FUNC(db_add_wc_create_vport);

#if 0
void db_check_mod_wc_create_vport(char* uuid,char* name, int32 port_type,
       int32 is_has_kernel, int32 kernel_type )
{
    if( NULL == uuid || NULL == name  )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateWildcastCreateVPort * pOper = GetDbIWildcastCreateVPort();
    if( NULL == pOper )
    {
        cout << "CDBOperateWildcastCreateVPort is NULL" << endl;
        return ;
    }

    CDbWildcastCreateVPort info;
    info.SetPortName(name);
    info.SetUuid(uuid);
    info.SetPortType(port_type);
    info.SetIsHasKernelType(is_has_kernel);
    info.SetKernelType(kernel_type);

    //info.DbgShow();
    int ret = pOper->CheckModify(info);
    if( ret != 0 )
    {
        cout << "CheckModify failed." << ret << endl;
        return;
    }
    cout << "CheckModify success." << endl;
    info.DbgShow();
}
DEFINE_DEBUG_FUNC(db_check_mod_wc_create_vport);


void db_mod_wc_create_vport(char* uuid,char* name, int32 port_type,
       int32 is_has_kernel, int32 kernel_type )
{
    if( NULL == uuid || NULL == name  )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateWildcastCreateVPort * pOper = GetDbIWildcastCreateVPort();
    if( NULL == pOper )
    {
        cout << "CDBOperateWildcastCreateVPort is NULL" << endl;
        return ;
    }

    CDbWildcastCreateVPort info;
    info.SetPortName(name);
    info.SetUuid(uuid);
    info.SetPortType(port_type);
    info.SetIsHasKernelType(is_has_kernel);
    info.SetKernelType(kernel_type);

    //info.DbgShow();
    int ret = pOper->Modify(info);
    if( ret != 0 )
    {
        cout << "Modify failed." << ret << endl;
        return;
    }
    cout << "Modify success." << endl;
    info.DbgShow();
}
DEFINE_DEBUG_FUNC(db_mod_wc_create_vport);
#endif 

void db_check_del_wc_create_vport(char* uuid)
{
    if( NULL == uuid )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateWildcastCreateVPort * pOper = GetDbIWildcastCreateVPort();
    if( NULL == pOper )
    {
        cout << "CDBOperateWildcastCreateVPort is NULL" << endl;
        return ;
    }
    //info.DbgShow();
    int ret = pOper->CheckDel(uuid);
    if( ret != 0 )
    {
        cout << "CheckDel failed." << ret << endl;
        return;
    }
    cout << "CheckDel success." << endl;
}
DEFINE_DEBUG_FUNC(db_check_del_wc_create_vport);

void db_del_wc_create_vport(char* uuid)
{
    if( NULL == uuid )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateWildcastCreateVPort * pOper = GetDbIWildcastCreateVPort();
    if( NULL == pOper )
    {
        cout << "CDBOperateWildcastCreateVPort is NULL" << endl;
        return ;
    }
    //info.DbgShow();
    int ret = pOper->Del(uuid);
    if( ret != 0 )
    {
        cout << "Del failed." << ret << endl;
        return;
    }
    cout << "Del success." << endl;
}
DEFINE_DEBUG_FUNC(db_del_wc_create_vport);

}



