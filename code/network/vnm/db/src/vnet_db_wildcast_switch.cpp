

#include "vnet_db_error.h"
#include "vnet_db_proc.h"
#include "vnet_db_mgr.h"
#include "vnet_db_wildcast_switch.h"
    
namespace zte_tecs
{
typedef enum tagWildcastSwitchProc
{
    EN_WILDCAST_SWITCH_PROC_QUERY = 0,
    EN_WILDCAST_SWITCH_PROC_QUERY_SUMMARY,   
    EN_WILDCAST_SWITCH_PROC_QUERY_BY_PORTNAME,  
    EN_WILDCAST_SWITCH_PROC_CHECK_ADD,
    EN_WILDCAST_SWITCH_PROC_ADD,
    EN_WILDCAST_SWITCH_PROC_CHECK_MODIFY,
    EN_WILDCAST_SWITCH_PROC_MODIFY,
    EN_WILDCAST_SWITCH_PROC_CHECK_DEL,
    EN_WILDCAST_SWITCH_PROC_DEL,

    EN_WILDCAST_SWITCH_PROC_BONDMAP_QUERY_SUMMARY,
    EN_WILDCAST_SWITCH_PROC_BONDMAP_QUERY_BY_PHYPORTNAME,    
    EN_WILDCAST_SWITCH_PROC_BONDMAP_QUERY,   
    EN_WILDCAST_SWITCH_PROC_BONDMAP_CHECK_ADD,
    EN_WILDCAST_SWITCH_PROC_BONDMAP_ADD,
    EN_WILDCAST_SWITCH_PROC_BONDMAP_CHECK_DEL,
    EN_WILDCAST_SWITCH_PROC_BONDMAP_DEL,
    
    EN_WILDCAST_SWITCH_PROC_ALL,
}EN_WILDCAST_SWITCH_PROC;

const char *s_astrWildcastSwitchProc[EN_WILDCAST_SWITCH_PROC_ALL] = 
{        
    "pf_net_query_wildcast_switch",
    "pf_net_query_wildcast_switch_summary",
    "pf_net_query_wildcast_switch_by_portname",
    "pf_net_check_add_wildcast_switch",  
    "pf_net_add_wildcast_switch",    
    "pf_net_check_modify_wildcast_switch", 
    "pf_net_modify_wildcast_switch", 
    "pf_net_check_del_wildcast_switch",  
    "pf_net_del_wildcast_switch",  

    "pf_net_query_wildcast_switch_bondmap_summary",
    "pf_net_query_wildcast_switch_bondmap_by_phyportname",    
    "pf_net_query_wildcast_switch_bondmap",
    "pf_net_check_add_wildcast_switch_bondmap",
    "pf_net_add_wildcast_switch_bondmap",
    "pf_net_check_del_wildcast_switch_bondmap",
    "pf_net_del_wildcast_switch_bondmap",
};

    
CDBOperateWildcastSwitch::CDBOperateWildcastSwitch(CVNetDbIProcedure * pProc)
{
    m_pIProc = pProc;
}

CDBOperateWildcastSwitch::~CDBOperateWildcastSwitch()
{
    
}

int32 CDBOperateWildcastSwitch::DbProcCallback(int type, CADORecordset *prs, void* nil )
{
    switch(type)
    {
        case EN_WILDCAST_SWITCH_PROC_QUERY_SUMMARY:
        case EN_WILDCAST_SWITCH_PROC_QUERY_BY_PORTNAME:
        {
            return QuerySummaryCallback(prs,nil);
        }
        break;
        
        case EN_WILDCAST_SWITCH_PROC_QUERY:
        {
            return QueryCallback(prs,nil);
        }
        break;
        
        case EN_WILDCAST_SWITCH_PROC_BONDMAP_QUERY_SUMMARY:
        case EN_WILDCAST_SWITCH_PROC_BONDMAP_QUERY:
        case EN_WILDCAST_SWITCH_PROC_BONDMAP_QUERY_BY_PHYPORTNAME:
        {
            return QueryBondMapCallback(prs,nil);
        }
        break;
         
        case EN_WILDCAST_SWITCH_PROC_ADD:
        case EN_WILDCAST_SWITCH_PROC_MODIFY:
        case EN_WILDCAST_SWITCH_PROC_DEL:
        case EN_WILDCAST_SWITCH_PROC_BONDMAP_ADD:
        case EN_WILDCAST_SWITCH_PROC_BONDMAP_DEL:
        {
            return OperateCallback(prs,nil);
        }
        break;
        
        case EN_WILDCAST_SWITCH_PROC_CHECK_ADD:
        case EN_WILDCAST_SWITCH_PROC_CHECK_MODIFY:
        case EN_WILDCAST_SWITCH_PROC_CHECK_DEL:
        case EN_WILDCAST_SWITCH_PROC_BONDMAP_CHECK_ADD:
        case EN_WILDCAST_SWITCH_PROC_BONDMAP_CHECK_DEL:
        {
            return CheckOperateCallback(prs,nil);
        }   
        break;
        default:
        {
            VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastSwitch::DbProcCallback type(%d) is invalid.\n",type);
            return VNET_DB_ERROR_PARAM_INVALID;
        }
        break;
    }
    
    return 0;
}
    
int32 CDBOperateWildcastSwitch::QuerySummary(vector<CDbWildcastSwitch> & outVInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastSwitch::QuerySummary IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    vector<CADOParameter> vInParam;

    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,
        s_astrWildcastSwitchProc[EN_WILDCAST_SWITCH_PROC_QUERY_SUMMARY],
        vInParam, 
        this,
        EN_WILDCAST_SWITCH_PROC_QUERY_SUMMARY,
        static_cast<void*> (&outVInfo));
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastSwitch::QuerySummary IProc->Handle failed(%d).\n",ret);
        return ret;
    }
    
    return 0;
}

int32 CDBOperateWildcastSwitch::QueryByPortName(const char* pcPortName, vector<CDbWildcastSwitch> & outVInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastSwitch::QueryByPortName IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == pcPortName )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastSwitch::QueryByPortName pcPortName is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    vector<CADOParameter> vInParam;
    CADOParameter inPortName(CADOParameter::paramText, CADOParameter::paramInput, (char *)pcPortName);
    vInParam.push_back(inPortName);

    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,
        s_astrWildcastSwitchProc[EN_WILDCAST_SWITCH_PROC_QUERY_BY_PORTNAME],
        vInParam, 
        this,
        EN_WILDCAST_SWITCH_PROC_QUERY_BY_PORTNAME,
        static_cast<void*> (&outVInfo));
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastSwitch::QueryByPortName IProc->Handle failed(%d).\n",ret);
        return ret;
    }
        
    return 0;
}


int32 CDBOperateWildcastSwitch::QuerySummaryCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastSwitch::QuerySummaryCallback prs is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    if( nil == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastSwitch::QuerySummaryCallback nil is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }
    
    // 
    vector<CDbWildcastSwitch> *pOutPort = NULL;
    pOutPort = static_cast<vector<CDbWildcastSwitch> *>(nil);
    if( pOutPort == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastSwitch::QuerySummaryCallback pOutPort is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    CDbWildcastSwitch info;    
    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == (11));    
    
    info.SetUuid( prs->GetFieldValue(0) );    
    info.SetSwitchName( prs->GetFieldValue(1) );
    
    int32 values = 0;
    if( false == prs->GetFieldValue(2,values) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastSwitch::QuerySummaryCallback GetFieldValue 2 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetSwitchType(values);  
    
    if( false == prs->GetFieldValue(3,values) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastSwitch::QuerySummaryCallback GetFieldValue 3 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetState(values); 
    
    if( false == prs->GetFieldValue(4,values) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastSwitch::QuerySummaryCallback GetFieldValue 4 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetEvbMode(values); 
    
    info.SetUplinkPgUuid( prs->GetFieldValue(5) );    

    if( false == prs->GetFieldValue(6,values) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastSwitch::QuerySummaryCallback GetFieldValue 6 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetMtuMax(values);  

    if( false == prs->GetFieldValue(7,values) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastSwitch::QuerySummaryCallback GetFieldValue 7 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetNicMaxNum(values);  
    
    info.SetUplinkPortName( prs->GetFieldValue(8) );
    
    if( false == prs->GetFieldValue(9,values) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastSwitch::QuerySummaryCallback GetFieldValue 9 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetUplinkPortType(values);  

    if( false == prs->GetFieldValue(10,values) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastSwitch::QuerySummaryCallback GetFieldValue 10 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetBondMode(values);  

    (*pOutPort).push_back(info);  

    return 0;
}

int32 CDBOperateWildcastSwitch::Query(CDbWildcastSwitch & Info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastSwitch::Query IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    vector<CADOParameter> vInParam;
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)Info.GetUuid().c_str());
    vInParam.push_back(inUuid);
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrWildcastSwitchProc[EN_WILDCAST_SWITCH_PROC_QUERY],
        vInParam, 
        this,
        EN_WILDCAST_SWITCH_PROC_QUERY,
        static_cast<void*> (&Info));
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            Info.DbgShow();
        }
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastSwitch::Query IProc->Handle failed(%d).\n",ret);
        }
        return ret;
    }
    
    return 0;
}

int32 CDBOperateWildcastSwitch::QueryCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastSwitch::QueryCallback prs is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    if( nil == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastSwitch::QueryCallback nil is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }
    
    // 
    CDbWildcastSwitch *pOutPort;
    pOutPort = static_cast<CDbWildcastSwitch *>(nil);
    if( pOutPort == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastSwitch::QueryCallback pOutPort is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }
    
    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == (12));    

    int32 proc_ret = 0;
    if( false == prs->GetFieldValue(0,proc_ret ))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastSwitch::QueryCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }    

    if( 0 != proc_ret )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastSwitch::QueryCallback proc failed(%d).\n", GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }
    
    (*pOutPort).SetUuid( prs->GetFieldValue(1) );    
    (*pOutPort).SetSwitchName( prs->GetFieldValue(2) );
    
    int32 values = 0;
    if( false == prs->GetFieldValue(3,values) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastSwitch::QueryCallback GetFieldValue 3 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetSwitchType(values);  

    if( false == prs->GetFieldValue(4,values) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastSwitch::QueryCallback GetFieldValue 4 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetState(values);

    if( false == prs->GetFieldValue(5,values) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastSwitch::QueryCallback GetFieldValue 5 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetEvbMode(values);
    
    (*pOutPort).SetUplinkPgUuid( prs->GetFieldValue(6) );    

    if( false == prs->GetFieldValue(7,values) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastSwitch::QueryCallback GetFieldValue 7 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetMtuMax(values);  

    if( false == prs->GetFieldValue(8,values) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastSwitch::QueryCallback GetFieldValue 8 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetNicMaxNum(values);  
    
    (*pOutPort).SetUplinkPortName( prs->GetFieldValue(9) );
    
    if( false == prs->GetFieldValue(10,values) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastSwitch::QueryCallback GetFieldValue 10 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetUplinkPortType(values);  

    if( false == prs->GetFieldValue(11,values) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastSwitch::QueryCallback GetFieldValue 11 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetBondMode(values);  

    return 0;
}

int32 CDBOperateWildcastSwitch::CheckAdd(CDbWildcastSwitch & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastSwitch::CheckAdd IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // uuid generate
    info.SetUuid(GenerateUUID().c_str());
    
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUuid().c_str());
    CADOParameter inSwitchName(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetSwitchName().c_str());    
    CADOParameter inSwitchType(CADOParameter::paramInt, CADOParameter::paramInput, info.GetSwitchType());    
    CADOParameter inState(CADOParameter::paramInt, CADOParameter::paramInput, info.GetState());    
    CADOParameter inEvbMode(CADOParameter::paramInt, CADOParameter::paramInput, info.GetEvbMode());    
    
    CADOParameter inPgUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUplinkPgUuid().c_str());
    CADOParameter inMtuMax(CADOParameter::paramText, CADOParameter::paramInput, info.GetMtuMax());   
    CADOParameter inNicMaxNum(CADOParameter::paramText, CADOParameter::paramInput, info.GetNicMaxNum());   
    CADOParameter inUplinkPortName(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUplinkPortName().c_str());    
    CADOParameter inUplinkPortType(CADOParameter::paramInt, CADOParameter::paramInput, info.GetUplinkPortType());    
    CADOParameter inBondMode(CADOParameter::paramInt, CADOParameter::paramInput, info.GetBondMode()); 
    
    vInParam.push_back(inUuid);
    vInParam.push_back(inSwitchName);    
    vInParam.push_back(inSwitchType);   
    vInParam.push_back(inState); 
    vInParam.push_back(inEvbMode); 
    vInParam.push_back(inPgUuid);         
    vInParam.push_back(inMtuMax);     
    vInParam.push_back(inNicMaxNum);    
    vInParam.push_back(inUplinkPortName);    
    vInParam.push_back(inUplinkPortType); 
    vInParam.push_back(inBondMode); 
  
    // ret struct 
    int32 proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrWildcastSwitchProc[EN_WILDCAST_SWITCH_PROC_CHECK_ADD],
        vInParam, 
        this,
        EN_WILDCAST_SWITCH_PROC_CHECK_ADD,
        static_cast<void*> (&proc_ret));
    
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastSwitch::CheckAdd IProc->Handle failed(%d).\n", ret);
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastSwitch::CheckAdd proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }

    return VNET_DB_SUCCESS;
}

int32 CDBOperateWildcastSwitch::Add(CDbWildcastSwitch & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastSwitch::Add IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // uuid generate
    info.SetUuid(GenerateUUID().c_str());
    
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUuid().c_str());
    CADOParameter inSwitchName(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetSwitchName().c_str());    
    CADOParameter inSwitchType(CADOParameter::paramInt, CADOParameter::paramInput, info.GetSwitchType());    
    CADOParameter inState(CADOParameter::paramInt, CADOParameter::paramInput, info.GetState());    
    CADOParameter inEvbMode(CADOParameter::paramInt, CADOParameter::paramInput, info.GetEvbMode());    
    
    CADOParameter inPgUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUplinkPgUuid().c_str());
    CADOParameter inMtuMax(CADOParameter::paramText, CADOParameter::paramInput, info.GetMtuMax());   
    CADOParameter inNicMaxNum(CADOParameter::paramText, CADOParameter::paramInput, info.GetNicMaxNum());   
    CADOParameter inUplinkPortName(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUplinkPortName().c_str());    
    CADOParameter inUplinkPortType(CADOParameter::paramInt, CADOParameter::paramInput, info.GetUplinkPortType());    
    CADOParameter inBondMode(CADOParameter::paramInt, CADOParameter::paramInput, info.GetBondMode()); 
    
    vInParam.push_back(inUuid);
    vInParam.push_back(inSwitchName);    
    vInParam.push_back(inSwitchType);   
    vInParam.push_back(inState); 
    vInParam.push_back(inEvbMode); 
    vInParam.push_back(inPgUuid);         
    vInParam.push_back(inMtuMax);     
    vInParam.push_back(inNicMaxNum);    
    vInParam.push_back(inUplinkPortName);    
    vInParam.push_back(inUplinkPortType); 
    vInParam.push_back(inBondMode); 
  
    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrWildcastSwitchProc[EN_WILDCAST_SWITCH_PROC_ADD],
        vInParam, 
        this,
        EN_WILDCAST_SWITCH_PROC_ADD,
        static_cast<void*> (&proc_ret));
    
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastSwitch::Add IProc->Handle failed(%d).\n", ret);
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastSwitch::Add proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }

    info.SetUuid(proc_ret.GetUuid().c_str());

    return VNET_DB_SUCCESS;
}

int32 CDBOperateWildcastSwitch::CheckModify(CDbWildcastSwitch & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastSwitch::CheckModify IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUuid().c_str());
    CADOParameter inSwitchName(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetSwitchName().c_str());    
    CADOParameter inSwitchType(CADOParameter::paramInt, CADOParameter::paramInput, info.GetSwitchType());    
    CADOParameter inState(CADOParameter::paramInt, CADOParameter::paramInput, info.GetState());    
    CADOParameter inEvbMode(CADOParameter::paramInt, CADOParameter::paramInput, info.GetEvbMode());    
    
    CADOParameter inPgUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUplinkPgUuid().c_str());
    CADOParameter inMtuMax(CADOParameter::paramText, CADOParameter::paramInput, info.GetMtuMax());   
    CADOParameter inNicMaxNum(CADOParameter::paramText, CADOParameter::paramInput, info.GetNicMaxNum());   
    CADOParameter inUplinkPortName(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUplinkPortName().c_str());    
    CADOParameter inUplinkPortType(CADOParameter::paramInt, CADOParameter::paramInput, info.GetUplinkPortType());    
    CADOParameter inBondMode(CADOParameter::paramInt, CADOParameter::paramInput, info.GetBondMode()); 
    
    vInParam.push_back(inUuid);
    vInParam.push_back(inSwitchName);    
    vInParam.push_back(inSwitchType);   
    vInParam.push_back(inState); 
    vInParam.push_back(inEvbMode); 
    vInParam.push_back(inPgUuid);         
    vInParam.push_back(inMtuMax);     
    vInParam.push_back(inNicMaxNum);    
    vInParam.push_back(inUplinkPortName);    
    vInParam.push_back(inUplinkPortType); 
    vInParam.push_back(inBondMode); 

    // ret struct 
    int32 proc_ret = 0; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrWildcastSwitchProc[EN_WILDCAST_SWITCH_PROC_CHECK_MODIFY],
        vInParam, 
        this,
        EN_WILDCAST_SWITCH_PROC_CHECK_MODIFY,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastSwitch::CheckModify IProc->Handle failed(%d).\n", ret);
        return ret;
    }

    //修改内容和原纪录内容相同 
    if( proc_ret == VNET_DB_PROCEDURE_RTN_HAS_SAME_ITEM )
    {
        return VNET_DB_SUCCESS;
    }

    if( proc_ret != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastSwitch::CheckModify proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }
    
    return VNET_DB_SUCCESS;
}

int32 CDBOperateWildcastSwitch::Modify(CDbWildcastSwitch & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastSwitch::Modify IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUuid().c_str());
    CADOParameter inSwitchName(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetSwitchName().c_str());    
    CADOParameter inSwitchType(CADOParameter::paramInt, CADOParameter::paramInput, info.GetSwitchType());    
    CADOParameter inState(CADOParameter::paramInt, CADOParameter::paramInput, info.GetState());    
    CADOParameter inEvbMode(CADOParameter::paramInt, CADOParameter::paramInput, info.GetEvbMode());    
    
    CADOParameter inPgUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUplinkPgUuid().c_str());
    CADOParameter inMtuMax(CADOParameter::paramText, CADOParameter::paramInput, info.GetMtuMax());   
    CADOParameter inNicMaxNum(CADOParameter::paramText, CADOParameter::paramInput, info.GetNicMaxNum());   
    CADOParameter inUplinkPortName(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUplinkPortName().c_str());    
    CADOParameter inUplinkPortType(CADOParameter::paramInt, CADOParameter::paramInput, info.GetUplinkPortType());    
    CADOParameter inBondMode(CADOParameter::paramInt, CADOParameter::paramInput, info.GetBondMode()); 
    
    vInParam.push_back(inUuid);
    vInParam.push_back(inSwitchName);    
    vInParam.push_back(inSwitchType);   
    vInParam.push_back(inState); 
    vInParam.push_back(inEvbMode); 
    vInParam.push_back(inPgUuid);         
    vInParam.push_back(inMtuMax);     
    vInParam.push_back(inNicMaxNum);    
    vInParam.push_back(inUplinkPortName);    
    vInParam.push_back(inUplinkPortType); 
    vInParam.push_back(inBondMode); 

    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrWildcastSwitchProc[EN_WILDCAST_SWITCH_PROC_MODIFY],
        vInParam, 
        this,
        EN_WILDCAST_SWITCH_PROC_MODIFY,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastSwitch::Modify IProc->Handle failed(%d).\n", ret);
        return ret;
    }

    //修改内容和原纪录内容相同 
    if( proc_ret.GetRet() == VNET_DB_PROCEDURE_RTN_HAS_SAME_ITEM )
    {
        return VNET_DB_SUCCESS;
    }

    if( proc_ret.GetRet() != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastSwitch::Modify proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }
    
    return VNET_DB_SUCCESS;
}


int32 CDBOperateWildcastSwitch::CheckDel(const char* uuid)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastSwitch::CheckDel IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == uuid )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastSwitch::CheckDel switch_uuid is NULL.\n");
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
        s_astrWildcastSwitchProc[EN_WILDCAST_SWITCH_PROC_CHECK_DEL],
        vInParam, 
        this,
        EN_WILDCAST_SWITCH_PROC_CHECK_DEL,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastSwitch::CheckDel(%s) IProc->Handle failed. ret : %d.\n",
            uuid,ret);
        return ret;
    }
    
    if( proc_ret != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastSwitch::CheckDel(%s) proc return failed. ret : %d.\n",
            uuid,GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }
    
    return 0;
}


int32 CDBOperateWildcastSwitch::Del(const char* uuid)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastSwitch::Del IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == uuid )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastSwitch::Del uuid is NULL.\n");
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
        s_astrWildcastSwitchProc[EN_WILDCAST_SWITCH_PROC_DEL],
        vInParam, 
        this,
        EN_WILDCAST_SWITCH_PROC_DEL,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastSwitch::Del(%s) IProc->Handle failed. ret : %d.\n",
            uuid,ret);
        return ret;
    }
    
    if( proc_ret.GetRet() != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastSwitch::Del(%s) proc return failed. ret : %d.\n",
            uuid,GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }
    
    return 0;
}

int32 CDBOperateWildcastSwitch::OperateCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastSwitch::OperateCallback param is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    
    CDbProcOperateResult *proc_ret = NULL;     
    proc_ret = static_cast<CDbProcOperateResult *>(nil);

    if( proc_ret == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastSwitch::OperateCallback proc_ret is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 3);    

    int32 nRet = 0;
    if( false == prs->GetFieldValue(0, nRet))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastSwitch::OperateCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetRet(nRet);

    int64 oid = 0;
    if( false == prs->GetFieldValue(1, oid))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastSwitch::OperateCallback GetFieldValue(1) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetOid(oid);
    
    (*proc_ret).SetUuid(prs->GetFieldValue(2)); 

    return 0;
}

int32 CDBOperateWildcastSwitch::CheckOperateCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastSwitch::CheckOperateCallback param is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    
    int32 *proc_ret = NULL;     
    proc_ret = static_cast<int32 *>(nil);

    if( proc_ret == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastSwitch::CheckOperateCallback proc_ret is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 1);    

    int32 nRet = 0;
    if( false == prs->GetFieldValue(0, nRet))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastSwitch::CheckOperateCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret) = nRet;

    return 0;
}

void CDBOperateWildcastSwitch::DbgShow()
{
#if 0
    cout << "--------------------------Port Info------------------------" << endl;
    // query summary
    vector<CDbSwitchSummary> vInfo;
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
    vector<CDbSwitchSummary>::iterator it = vInfo.begin();
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
void db_query_wc_switch(const char* uuid)
{
    if( NULL == uuid )
    {
        cout << "uuid is NULL" << endl;
        return;
    }
    
    CDBOperateWildcastSwitch * pOper = GetDbIWildcastSwitch();
    if( NULL == pOper )
    {
        cout << "CDBOperateWildcastSwitch is NULL" << endl;
        return ;
    }

    CDbWildcastSwitch info;
    info.SetUuid(uuid);
    int ret = pOper->Query(info);
    if( ret != 0 )
    {
        cout << "Query failed." << ret << endl;
        return;
    }
    cout << "Query success." << endl;
    info.DbgShow();
}
DEFINE_DEBUG_FUNC(db_query_wc_switch);

void db_query_wc_switch_summary()
{
    
    CDBOperateWildcastSwitch * pOper = GetDbIWildcastSwitch();
    if( NULL == pOper )
    {
        cout << "CDBOperateWildcastSwitch is NULL" << endl;
        return ;
    }

    vector<CDbWildcastSwitch> vInfo;
    int ret = pOper->QuerySummary(vInfo);
    if( ret != 0 )
    {
        cout << "Query failed." << ret << endl;
        return;
    }
    cout << "Query success." << endl;
    vector<CDbWildcastSwitch>::iterator it = vInfo.begin();
    for(; it != vInfo.end(); ++it)
    {
        cout << "-----------------" << endl;
        (*it).DbgShow();
    }
}
DEFINE_DEBUG_FUNC(db_query_wc_switch_summary);


void db_query_wc_switch_by_portname(const char* pcPortName)
{
    
    CDBOperateWildcastSwitch * pOper = GetDbIWildcastSwitch();
    if( NULL == pOper || NULL == pcPortName)
    {
        cout << "CDBOperateWildcastSwitch is NULL" << endl;
        return ;
    }

    vector<CDbWildcastSwitch> vInfo;
    int ret = pOper->QueryByPortName(pcPortName,vInfo);
    if( ret != 0 )
    {
        cout << "Query failed." << ret << endl;
        return;
    }
    cout << "Query success." << endl;
    vector<CDbWildcastSwitch>::iterator it = vInfo.begin();
    for(; it != vInfo.end(); ++it)
    {
        cout << "-----------------" << endl;
        (*it).DbgShow();
    }
}
DEFINE_DEBUG_FUNC(db_query_wc_switch_by_portname);

void db_check_add_wc_switch( char* switch_name, int32 switch_type, char* pg_uuid,
         int32 nic_max, char* uplink_port_name, int32 uplink_port_type, int32 bond_mode)
{
    if(  NULL == switch_name || NULL == pg_uuid || NULL == uplink_port_name )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateWildcastSwitch * pOper = GetDbIWildcastSwitch();
    if( NULL == pOper )
    {
        cout << "CDBOperateWildcastSwitch is NULL" << endl;
        return ;
    }

    CDbWildcastSwitch info;
    //info.SetUuid(vna_uuid);
    info.SetSwitchName(switch_name);
    info.SetSwitchType(switch_type);
    info.SetUplinkPgUuid(pg_uuid);
    info.SetNicMaxNum(nic_max);
    info.SetUplinkPortName(uplink_port_name);    
    info.SetUplinkPortType(uplink_port_type);
    info.SetBondMode(bond_mode);

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
DEFINE_DEBUG_FUNC(db_check_add_wc_switch);


void db_add_wc_switch( char* switch_name, int32 switch_type, char* pg_uuid,
         int32 nic_max, char* uplink_port_name, int32 uplink_port_type, int32 bond_mode)
{
    if(  NULL == switch_name || NULL == pg_uuid || NULL == uplink_port_name )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateWildcastSwitch * pOper = GetDbIWildcastSwitch();
    if( NULL == pOper )
    {
        cout << "CDBOperateWildcastSwitch is NULL" << endl;
        return ;
    }

    CDbWildcastSwitch info;
    //info.SetUuid(vna_uuid);
    info.SetSwitchName(switch_name);
    info.SetSwitchType(switch_type);
    info.SetUplinkPgUuid(pg_uuid);
    info.SetNicMaxNum(nic_max);
    info.SetUplinkPortName(uplink_port_name);    
    info.SetUplinkPortType(uplink_port_type);
    info.SetBondMode(bond_mode);

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
DEFINE_DEBUG_FUNC(db_add_wc_switch);


void db_check_mod_wc_switch(char* uuid, char* switch_name, int32 switch_type, char* pg_uuid,
         int32 nic_max, char* uplink_port_name, int32 uplink_port_type, int32 bond_mode)
{
    if( NULL == uuid || NULL == switch_name || NULL == pg_uuid || NULL == uplink_port_name )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateWildcastSwitch * pOper = GetDbIWildcastSwitch();
    if( NULL == pOper )
    {
        cout << "CDBOperateWildcastSwitch is NULL" << endl;
        return ;
    }

    CDbWildcastSwitch info;
    info.SetUuid(uuid);
    info.SetSwitchName(switch_name);
    info.SetSwitchType(switch_type);
    info.SetUplinkPgUuid(pg_uuid);
    info.SetNicMaxNum(nic_max);
    info.SetUplinkPortName(uplink_port_name);    
    info.SetUplinkPortType(uplink_port_type);
    info.SetBondMode(bond_mode);

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
DEFINE_DEBUG_FUNC(db_check_mod_wc_switch);


void db_mod_wc_switch(char* uuid, char* switch_name, int32 switch_type, char* pg_uuid,
         int32 nic_max, char* uplink_port_name, int32 uplink_port_type, int32 bond_mode)
{
    if( NULL == uuid || NULL == switch_name || NULL == pg_uuid || NULL == uplink_port_name )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateWildcastSwitch * pOper = GetDbIWildcastSwitch();
    if( NULL == pOper )
    {
        cout << "CDBOperateWildcastSwitch is NULL" << endl;
        return ;
    }

    CDbWildcastSwitch info;
    info.SetUuid(uuid);
    info.SetSwitchName(switch_name);
    info.SetSwitchType(switch_type);
    info.SetUplinkPgUuid(pg_uuid);
    info.SetNicMaxNum(nic_max);
    info.SetUplinkPortName(uplink_port_name);    
    info.SetUplinkPortType(uplink_port_type);
    info.SetBondMode(bond_mode);

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
DEFINE_DEBUG_FUNC(db_mod_wc_switch);


void db_check_del_wc_switch(char * uuid)
{
    if(uuid == NULL )
    {
        cout << "Param is NULL" << endl;
        return;
    }

    CDBOperateWildcastSwitch * pOper = GetDbIWildcastSwitch();
    if( NULL == pOper )
    {
        cout << "CDBOperateWildcastSwitch is NULL" << endl;
        return ;
    }

    int ret = pOper->CheckDel(uuid);
    if( ret != 0 )
    {
        cout << "CheckDel failed." << ret << endl;
        return;
    }
    cout << "CheckDel success." << endl;
}
DEFINE_DEBUG_FUNC(db_check_del_wc_switch);

void db_del_wc_switch(char * uuid)
{
    if(uuid == NULL )
    {
        cout << "Param is NULL" << endl;
        return;
    }

    CDBOperateWildcastSwitch * pOper = GetDbIWildcastSwitch();
    if( NULL == pOper )
    {
        cout << "CDBOperateWildcastSwitch is NULL" << endl;
        return ;
    }

    int ret = pOper->Del(uuid);
    if( ret != 0 )
    {
        cout << "Del failed." << ret << endl;
        return;
    }
    cout << "Del success." << endl;
}
DEFINE_DEBUG_FUNC(db_del_wc_switch);


int32 CDBOperateWildcastSwitch::QueryBondMapSummary( vector<CDbWildcastSwitchBondMap> &outVInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastSwitch::QueryBondMapSummary IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    vector<CADOParameter> vInParam;
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,
        s_astrWildcastSwitchProc[EN_WILDCAST_SWITCH_PROC_BONDMAP_QUERY_SUMMARY],
        vInParam, 
        this,
        EN_WILDCAST_SWITCH_PROC_BONDMAP_QUERY_SUMMARY,
        static_cast<void*> (&outVInfo));
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastSwitch::QueryBondMapSummary IProc->Handle failed(%d).\n",ret);
        return ret;
    }
    
    return 0;
}

int32 CDBOperateWildcastSwitch::QueryBondMapByPhyPortName(const char* pcPortName, vector<CDbWildcastSwitchBondMap> & outVInfo)
{
    if( NULL == m_pIProc)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastSwitch::QueryBondMapByPhyPortName IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == pcPortName)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastSwitch::QueryBondMapByPhyPortName pcPortName is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    vector<CADOParameter> vInParam;
    CADOParameter inPortName(CADOParameter::paramText, CADOParameter::paramInput, (char *)pcPortName);
    vInParam.push_back(inPortName);
    
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,
        s_astrWildcastSwitchProc[EN_WILDCAST_SWITCH_PROC_BONDMAP_QUERY_BY_PHYPORTNAME],
        vInParam, 
        this,
        EN_WILDCAST_SWITCH_PROC_BONDMAP_QUERY_BY_PHYPORTNAME,
        static_cast<void*> (&outVInfo));
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastSwitch::QueryBondMapByPhyPortName IProc->Handle failed(%d).\n",ret);
        return ret;
    }
    
    return 0;
}


int32 CDBOperateWildcastSwitch::QueryBondMap(const char* uuid, vector<CDbWildcastSwitchBondMap> &outVInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastSwitch::QueryBondMap IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }
    
    if( NULL == uuid )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastSwitch::QueryBondMap uuid is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }
    
    // call procedure 
    vector<CADOParameter> vInParam;
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)uuid);
    vInParam.push_back(inUuid);
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,
        s_astrWildcastSwitchProc[EN_WILDCAST_SWITCH_PROC_BONDMAP_QUERY],
        vInParam, 
        this,
        EN_WILDCAST_SWITCH_PROC_BONDMAP_QUERY,
        static_cast<void*> (&outVInfo));
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastSwitch::QueryBondMap IProc->Handle failed(%d).\n",ret);
        return ret;
    }
    
    return 0;
}

int32 CDBOperateWildcastSwitch::QueryBondMapCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastSwitch::QueryBondMapCallback prs is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    if( nil == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastSwitch::QueryBondMapCallback nil is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }
    
    // 
    vector<CDbWildcastSwitchBondMap>  *pOutPort;
    pOutPort = static_cast<vector<CDbWildcastSwitchBondMap>  *>(nil);
    
    if( pOutPort == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastSwitch::QueryBondMapCallback pOutPort is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    CDbWildcastSwitchBondMap info;
    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == (7));    

    info.SetUuid( prs->GetFieldValue(0) );    
    info.SetSwitchName( prs->GetFieldValue(1) ); 
    
    //       
    int32 values = 0;
    if( false == prs->GetFieldValue(2,values) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastSwitch::QueryBondMapCallback GetFieldValue failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetSwitchType(values);  

    info.SetPgUuid( prs->GetFieldValue(3) ); 
    info.SetUplinkPortName( prs->GetFieldValue(4) ); 

    if( false == prs->GetFieldValue(5,values) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastSwitch::QueryBondMapCallback GetFieldValue failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetBondMode(values);  

    info.SetBondPhyName( prs->GetFieldValue(6) ); 
    
    (*pOutPort).push_back(info);  

    return 0;
}

int32 CDBOperateWildcastSwitch::CheckAddBondMap(const char* uuid, const char* phy_name)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastSwitch::CheckAddBondMap IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == uuid || NULL == phy_name)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastSwitch::CheckAddBondMap uuid or phy_name is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }
    
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)uuid);
    CADOParameter inPhyName(CADOParameter::paramText, CADOParameter::paramInput, (char *)phy_name);    
   
    vInParam.push_back(inUuid);
    vInParam.push_back(inPhyName);
  
    // ret struct 
    int32 proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrWildcastSwitchProc[EN_WILDCAST_SWITCH_PROC_BONDMAP_CHECK_ADD],
        vInParam, 
        this,
        EN_WILDCAST_SWITCH_PROC_BONDMAP_CHECK_ADD,
        static_cast<void*> (&proc_ret));
    
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastSwitch::CheckAddBondMap IProc->Handle failed(%d).\n", ret);
        return ret;
    }
    
    // 重复添加 
    if( proc_ret == VNET_DB_PROCEDURE_RTN_HAS_SAME_ITEM )
    {
        return VNET_DB_SUCCESS;
    }

    if( proc_ret  != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastSwitch::CheckAddBondMap proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }

    return VNET_DB_SUCCESS;
}

int32 CDBOperateWildcastSwitch::AddBondMap(const char* uuid, const char* phy_name)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastSwitch::AddBondMap IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == uuid || NULL == phy_name)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastSwitch::AddBondMap uuid or phy_name is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }
        
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)uuid);
    CADOParameter inPhyName(CADOParameter::paramText, CADOParameter::paramInput, (char *)phy_name);    
   
    vInParam.push_back(inUuid);
    vInParam.push_back(inPhyName);
  
    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrWildcastSwitchProc[EN_WILDCAST_SWITCH_PROC_BONDMAP_ADD],
        vInParam, 
        this,
        EN_WILDCAST_SWITCH_PROC_BONDMAP_ADD,
        static_cast<void*> (&proc_ret));
    
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastSwitch::AddBondMap IProc->Handle failed(%d).\n", ret);
        return ret;
    }
    
    // 重复添加 
    if( proc_ret.GetRet() == VNET_DB_PROCEDURE_RTN_HAS_SAME_ITEM )
    {
        //info.SetUuid(proc_ret.GetUuid().c_str());
        return VNET_DB_SUCCESS;
    }

    if( proc_ret.GetRet() != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateWildcastSwitch::AddBondMap proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }

    return VNET_DB_SUCCESS;
}


int32 CDBOperateWildcastSwitch::CheckDelBondMap(const char* uuid, const char* phy_name)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastSwitch::CheckDelBondMap IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == uuid || NULL == phy_name)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastSwitch::CheckDelBondMap uuid or phy_name is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }
        
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)uuid);
    CADOParameter inPhyName(CADOParameter::paramText, CADOParameter::paramInput, (char *)phy_name);    
   
    vInParam.push_back(inUuid);
    vInParam.push_back(inPhyName);

    // ret struct 
    int32 proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrWildcastSwitchProc[EN_WILDCAST_SWITCH_PROC_BONDMAP_CHECK_DEL],
        vInParam, 
        this,
        EN_WILDCAST_SWITCH_PROC_BONDMAP_CHECK_DEL,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastSwitch::CheckDelBondMap IProc->Handle failed. ret : %d.\n", ret);
        return ret;
    }
    
    if( proc_ret != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastSwitch::CheckDelBondMap proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }
    
    return 0;
}


int32 CDBOperateWildcastSwitch::DelBondMap(const char* uuid, const char* phy_name)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastSwitch::DelBondMap IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == uuid || NULL == phy_name)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastSwitch::DelBondMap uuid or phy_name is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }
        
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)uuid);
    CADOParameter inPhyName(CADOParameter::paramText, CADOParameter::paramInput, (char *)phy_name);    
   
    vInParam.push_back(inUuid);
    vInParam.push_back(inPhyName);

    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrWildcastSwitchProc[EN_WILDCAST_SWITCH_PROC_BONDMAP_DEL],
        vInParam, 
        this,
        EN_WILDCAST_SWITCH_PROC_BONDMAP_DEL,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastSwitch::DelBondMap IProc->Handle failed. ret : %d.\n", ret);
        return ret;
    }
    
    if( proc_ret.GetRet() != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateWildcastSwitch::DelBondMap proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }
    
    return 0;
}


void db_query_wc_switch_bondmap(const char* uuid)
{
    if( NULL == uuid )
    {
        cout << "switch_uuid uuid is NULL" << endl;
        return;
    }
    
    CDBOperateWildcastSwitch * pOper = GetDbIWildcastSwitch();
    if( NULL == pOper )
    {
        cout << "CDBOperateWildcastSwitch is NULL" << endl;
        return ;
    }

    vector<CDbWildcastSwitchBondMap> vInfo;
    int ret = pOper->QueryBondMap(uuid,vInfo);
    if( ret != 0 )
    {
        cout << "Query failed." << ret << endl;
        return;
    }
    cout << "Query success." << endl;
    vector<CDbWildcastSwitchBondMap>::iterator it = vInfo.begin();
    for(; it != vInfo.end(); ++it)
    {        
        cout << "--------------------" << endl;
        (*it).DbgShow();
    }
}
DEFINE_DEBUG_FUNC(db_query_wc_switch_bondmap);

void db_query_wc_switch_bondmap_summary()
{
    
    CDBOperateWildcastSwitch * pOper = GetDbIWildcastSwitch();
    if( NULL == pOper )
    {
        cout << "CDBOperateWildcastSwitch is NULL" << endl;
        return ;
    }

    vector<CDbWildcastSwitchBondMap> vInfo;
    int ret = pOper->QueryBondMapSummary(vInfo);
    if( ret != 0 )
    {
        cout << "Query failed." << ret << endl;
        return;
    }
    cout << "Query success." << endl;
    vector<CDbWildcastSwitchBondMap>::iterator it = vInfo.begin();
    for(; it != vInfo.end(); ++it)
    {        
        cout << "--------------------" << endl;
        (*it).DbgShow();
    }
}
DEFINE_DEBUG_FUNC(db_query_wc_switch_bondmap_summary);


void db_query_wc_switch_bondmap_by_portname(const char* pcPortName)
{
    
    CDBOperateWildcastSwitch * pOper = GetDbIWildcastSwitch();
    if( NULL == pOper || NULL == pcPortName )
    {
        cout << "CDBOperateWildcastSwitch is NULL" << endl;
        return ;
    }

    vector<CDbWildcastSwitchBondMap> vInfo;
    int ret = pOper->QueryBondMapByPhyPortName(pcPortName,vInfo);
    if( ret != 0 )
    {
        cout << "Query failed." << ret << endl;
        return;
    }
    cout << "Query success." << endl;
    vector<CDbWildcastSwitchBondMap>::iterator it = vInfo.begin();
    for(; it != vInfo.end(); ++it)
    {        
        cout << "--------------------" << endl;
        (*it).DbgShow();
    }
}
DEFINE_DEBUG_FUNC(db_query_wc_switch_bondmap_by_portname);


void db_check_add_wc_switch_bondmap(char* uuid, char* phy_name)
{
    if( NULL == uuid || NULL == phy_name )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateWildcastSwitch * pOper = GetDbIWildcastSwitch();
    if( NULL == pOper )
    {
        cout << "CDBOperateWildcastSwitch is NULL" << endl;
        return ;
    }

    //info.DbgShow();
    int ret = pOper->CheckAddBondMap(uuid,phy_name);
    if( ret != 0 )
    {
        cout << "CheckAddBondMap failed." << ret << endl;
        return;
    }
    cout << "CheckAddBondMap success." << endl;
}
DEFINE_DEBUG_FUNC(db_check_add_wc_switch_bondmap);

void db_add_wc_switch_bondmap(char* uuid, char* phy_name)
{
    if( NULL == uuid || NULL == phy_name )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateWildcastSwitch * pOper = GetDbIWildcastSwitch();
    if( NULL == pOper )
    {
        cout << "CDBOperateWildcastSwitch is NULL" << endl;
        return ;
    }

    //info.DbgShow();
    int ret = pOper->AddBondMap(uuid,phy_name);
    if( ret != 0 )
    {
        cout << "AddBondMap failed." << ret << endl;
        return;
    }
    cout << "AddBondMap success." << endl;
}
DEFINE_DEBUG_FUNC(db_add_wc_switch_bondmap);


void db_check_del_wc_switch_bondmap(char* uuid, char* phy_name)
{
    if( NULL == uuid || NULL == phy_name )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateWildcastSwitch * pOper = GetDbIWildcastSwitch();
    if( NULL == pOper )
    {
        cout << "CDBOperateWildcastSwitch is NULL" << endl;
        return ;
    }

    //info.DbgShow();
    int ret = pOper->CheckDelBondMap(uuid,phy_name);
    if( ret != 0 )
    {
        cout << "CheckDelBondMap failed." << ret << endl;
        return;
    }
    cout << "CheckDelBondMap success." << endl;
}
DEFINE_DEBUG_FUNC(db_check_del_wc_switch_bondmap);

void db_del_wc_switch_bondmap(char* uuid, char* phy_name)
{
    if( NULL == uuid || NULL == phy_name )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateWildcastSwitch * pOper = GetDbIWildcastSwitch();
    if( NULL == pOper )
    {
        cout << "CDBOperateWildcastSwitch is NULL" << endl;
        return ;
    }

    //info.DbgShow();
    int ret = pOper->DelBondMap(uuid,phy_name);
    if( ret != 0 )
    {
        cout << "DelBondMap failed." << ret << endl;
        return;
    }
    cout << "DelBondMap success." << endl;
}
DEFINE_DEBUG_FUNC(db_del_wc_switch_bondmap);



}



