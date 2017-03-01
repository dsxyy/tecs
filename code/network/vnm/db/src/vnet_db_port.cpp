

#include "vnet_db_port.h"
#include "vnet_db_error.h"
#include "vnet_db_proc.h"
#include "vnet_db_mgr.h"

namespace zte_tecs
{
typedef enum tagPortProc
{
    EN_PORT_PROC_QUERYSUMMARY_BY_VNA_PORTNAME = 0, 
    EN_PORT_PROC_QUERYSUMMARY,
    EN_PORT_PROC_QUERY,    
    EN_PORT_PROC_CHECK_PORTNAME_IS_CONFLICT, 
#if 0    
    EN_PORT_PROC_ADD,
    EN_PORT_PROC_MODIFY,
    EN_PORT_PROC_DEL,
#endif 
    EN_PORT_PROC_ALL,
}EN_PORT_PROC;

const char *s_astrPortProc[EN_PORT_PROC_ALL] = 
{        
    "pf_net_query_port_summary_by_vna_portname",
    "pf_net_query_port_summary",
    "pf_net_query_port",
    "pf_net_check_portname_is_conflict",
#if 0
    "pf_net_add_port",    
    "pf_net_modify_port",  
    "pf_net_del_port",  
#endif 
};
    
CDBOperatePort::CDBOperatePort(CVNetDbIProcedure * pProc)
{
    m_pIProc = pProc;
}

CDBOperatePort::~CDBOperatePort()
{
    
}

int32 CDBOperatePort::DbProcCallback(int type, CADORecordset *prs, void* nil )
{
    switch(type)
    {
        case EN_PORT_PROC_QUERYSUMMARY:
        {
            return QuerySummaryCallback(prs,nil);
        }
        break;
        case EN_PORT_PROC_QUERY:
        {
            return QueryCallback(prs,nil);
        }
        break;
        case EN_PORT_PROC_QUERYSUMMARY_BY_VNA_PORTNAME:
        {
            return QuerySummaryByVnaPortNameCallback(prs,nil);
        }
        break;
        case EN_PORT_PROC_CHECK_PORTNAME_IS_CONFLICT:
        {
            return CheckOperateCallback(prs,nil);
        }
        break;
        
#if 0         
        case EN_PORT_PROC_ADD:
        case EN_PORT_PROC_MODIFY:
        case EN_PORT_PROC_DEL:
        {
            return OperateCallback(prs,nil);
        }
        break;
#endif         
        default:
        {
            VNET_LOG(VNET_LOG_ERROR, "CDBOperatePort::DbProcCallback type(%d) is invalid.\n",type);
            return VNET_DB_ERROR_PARAM_INVALID;
        }
        break;
    }
    
    return 0;
}


int32 CDBOperatePort::CheckOperateCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePort::CheckOperateCallback param is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    
    int32 *proc_ret = NULL;     
    proc_ret = static_cast<int32 *>(nil);

    if( proc_ret == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePort::CheckOperateCallback proc_ret is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 1);    

    int32 nRet = 0;
    if( false == prs->GetFieldValue(0, nRet))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePort::CheckOperateCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret) = nRet;

    return 0;
}
    
int32 CDBOperatePort::QuerySummary(const char* vna_uuid,vector<CDbPortSummary> & outVInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePort::QuerySummary IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == vna_uuid)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePort::QuerySummary vna_uuid is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // call procedure 
    vector<CADOParameter>  vInParam;
    CADOParameter inVnaUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)vna_uuid);
    vInParam.push_back(inVnaUuid);
    
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,\
        s_astrPortProc[EN_PORT_PROC_QUERYSUMMARY],\
        vInParam, \
        this,\
        EN_PORT_PROC_QUERYSUMMARY,\
        static_cast<void*> (&outVInfo));
        
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePort::QuerySummary IProc->Handle failed(%d) \n",ret);
        return ret;
    }
        
    return 0;
}

int32 CDBOperatePort::QuerySummaryCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePort::QuerySummaryCallback prs is NULL.\n");
        //VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }

    vector<CDbPortSummary> *pvOut;
    pvOut = static_cast<vector<CDbPortSummary>*>(nil);

    if( pvOut == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePort::QuerySummaryCallback pvOut is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    CDbPortSummary info;

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 4);
    info.SetVnaUuid(prs->GetFieldValue(0));
    info.SetName(prs->GetFieldValue(1));    
    info.SetUuid(prs->GetFieldValue(2));
    int32 port_type = 0;
    if( false == prs->GetFieldValue(3,port_type) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePort::QuerySummaryCallback GetFieldValue failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetPortType(port_type);

    (*pvOut).push_back(info);

    return 0;
}

int32 CDBOperatePort::Query(CDbPort & Info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePort::Query IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    vector<CADOParameter> vInParam;
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)Info.GetUuid().c_str());
    vInParam.push_back(inUuid);
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrPortProc[EN_PORT_PROC_QUERY],
        vInParam, 
        this,
        EN_PORT_PROC_QUERY,
        static_cast<void*> (&Info));
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            Info.DbgPortShow();
        }
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR,"CDBOperatePort::Query IProc->Handle failed(%d).\n",ret);
        }
        return ret;
    }
    
    return 0;
}

int32 CDBOperatePort::QueryCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePort::QueryCallback prs is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
        
    CDbPort *pOutPort;
    pOutPort = static_cast<CDbPort *>(nil);

    if( pOutPort == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePort::QueryCallback pOutPort is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 14);    

    int32 proc_ret = 0;
    if( false == prs->GetFieldValue(0,proc_ret ))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePort::QueryCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }    

    if( 0 != proc_ret )
    {
        if( DbgGetDBPrint() || !(VNET_DB_PROCEDURE_RTN_ITEM_NO_EXIST == proc_ret) )
        {
            VNET_LOG(VNET_LOG_ERROR, "CDBOperatePort::QueryCallback proc failed(%d).\n", GET_DB_ERROR_PROC(proc_ret));
        }
        return GET_DB_ERROR_PROC(proc_ret);
    }

    (*pOutPort).SetVnaUuid( prs->GetFieldValue(1));
    
    int32 port_type = 0;
    if( false == prs->GetFieldValue(2,port_type) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePort::QueryCallback GetFieldValue 2 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetPortType(port_type);
    
    (*pOutPort).SetName( prs->GetFieldValue(3));
    (*pOutPort).SetUuid( prs->GetFieldValue(4));
    
    int32 state = 0;
    if( false == prs->GetFieldValue(5,state) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePort::QueryCallback GetFieldValue 5 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetState(state);

    int32 broadcast = 0;
    if( false == prs->GetFieldValue(6,broadcast) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePort::QueryCallback GetFieldValue 6 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetIsBroadcast(broadcast);

    int32 running = 0;
    if( false == prs->GetFieldValue(7,running) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePort::QueryCallback GetFieldValue 7 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetIsRunning(running);

    int32 multicast = 0;
    if( false == prs->GetFieldValue(8,multicast) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePort::QueryCallback GetFieldValue 8 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetIsMulticast(multicast);

    int32 promis = 0;
    if( false == prs->GetFieldValue(9,promis) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePort::QueryCallback GetFieldValue 9 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetPromiscuous(promis);

    int32 mtu = 0;
    if( false == prs->GetFieldValue(10,mtu) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePort::QueryCallback GetFieldValue 10 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetMtu(mtu);

    int32 master = 0;
    if( false == prs->GetFieldValue(11,master) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePort::QueryCallback GetFieldValue 11 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetIsMaster(master);

    int32 admin = 0;
    if( false == prs->GetFieldValue(12,admin) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePort::QueryCallback GetFieldValue 12 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetAdminState(admin);

    int32 online = 0;
    if( false == prs->GetFieldValue(13,online) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePort::QueryCallback GetFieldValue 13 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetIsOnline(online);

    return 0;
}

int32 CDBOperatePort::QuerySummaryByVnaPortName(const char* vna_uuid, const char* name,CDbPortSummary & outInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePort::QuerySummaryByVnaPortName IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    vector<CADOParameter> vInParam;
    CADOParameter inVnaUuid(CADOParameter::paramText, CADOParameter::paramInput, (char*)vna_uuid);
    CADOParameter inName(CADOParameter::paramText, CADOParameter::paramInput, (char*)name);    
    vInParam.push_back(inVnaUuid);
    vInParam.push_back(inName);
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrPortProc[EN_PORT_PROC_QUERYSUMMARY_BY_VNA_PORTNAME],
        vInParam, 
        this,
        EN_PORT_PROC_QUERYSUMMARY_BY_VNA_PORTNAME,
        static_cast<void*> (&outInfo));
    if( ret != VNET_DB_SUCCESS )
    {
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR,"CDBOperatePort::QuerySummaryByVnaPortName(%s,%s) IProc->Handle failed(%d).\n",
                vna_uuid,name,ret);
        }
        return ret;
    }

    return 0;
}


int32 CDBOperatePort::QuerySummaryByVnaPortNameCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePort::QuerySummaryByVnaPortNameCallback prs is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
        
    CDbPortSummary *pOutPort;
    pOutPort = static_cast<CDbPortSummary *>(nil);

    if( pOutPort == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePort::QuerySummaryByVnaPortNameCallback pOutPort is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 5);    

    int32 proc_ret = 0;
    if( false == prs->GetFieldValue(0,proc_ret ))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePort::QuerySummaryByVnaPortNameCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }    

    if( 0 != proc_ret )
    {
        if( DbgGetDBPrint() || !(VNET_DB_PROCEDURE_RTN_ITEM_NO_EXIST == proc_ret) )
        {  
            VNET_LOG(VNET_LOG_ERROR, "CDBOperatePort::QuerySummaryByVnaPortNameCallback proc failed(%d).\n", GET_DB_ERROR_PROC(proc_ret));
        }
        return GET_DB_ERROR_PROC(proc_ret);
    }

    (*pOutPort).SetVnaUuid( prs->GetFieldValue(1));
    
    int32 port_type = 0;
    if( false == prs->GetFieldValue(2,port_type) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePort::QuerySummaryByVnaPortNameCallback GetFieldValue 2 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetPortType(port_type);
    
    (*pOutPort).SetName( prs->GetFieldValue(3));
    (*pOutPort).SetUuid( prs->GetFieldValue(4));
    
    return 0;
}

int32 CDBOperatePort::CheckPortNameIsConflict(const char* vna_uuid, const char* portName, int32 & nResult )
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePort::CheckPortNameIsConflict IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    vector<CADOParameter> vInParam;
    CADOParameter inVnaUuid(CADOParameter::paramText, CADOParameter::paramInput, (char*)vna_uuid);
    CADOParameter inName(CADOParameter::paramText, CADOParameter::paramInput, (char*)portName);    
    vInParam.push_back(inVnaUuid);
    vInParam.push_back(inName);
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrPortProc[EN_PORT_PROC_CHECK_PORTNAME_IS_CONFLICT],
        vInParam, 
        this,
        EN_PORT_PROC_CHECK_PORTNAME_IS_CONFLICT,
        static_cast<void*> (&nResult));
    if( ret != VNET_DB_SUCCESS )
    {        
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePort::CheckPortNameIsConflict(%s,%s) IProc->Handle failed(%d).\n",
            vna_uuid,portName, ret);
        return ret;
    }
    return 0;
}


#if 0
int32 CDBOperatePort::Add(CDbPort & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePort::AddPort IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // uuid generate
    info.SetUuid(GenerateUUID().c_str());
    
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inVnaUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetVnaUuid().c_str());    
    CADOParameter inPortType(CADOParameter::paramInt, CADOParameter::paramInput, info.GetPortType());    
    CADOParameter inName(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetName().c_str());
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUuid().c_str());    
    
    CADOParameter inState(CADOParameter::paramInt, CADOParameter::paramInput, info.GetState());    
    CADOParameter inBroadcast(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsBroadcast());    
    CADOParameter inRunning(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsRunning());    
    CADOParameter inMult(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsMulticast());    
    CADOParameter inPromi(CADOParameter::paramInt, CADOParameter::paramInput, info.GetPromiscuous());    
    CADOParameter inMtu(CADOParameter::paramInt, CADOParameter::paramInput, info.GetMtu());        
    CADOParameter inMaster(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsMaster()); 
    CADOParameter inAdminState(CADOParameter::paramInt, CADOParameter::paramInput, info.GetAdminState()); 
    CADOParameter inOnline(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsOnline()); 
    
    vInParam.push_back(inVnaUuid);
    vInParam.push_back(inPortType);
    vInParam.push_back(inName);
    vInParam.push_back(inUuid);    
    vInParam.push_back(inState);    
    vInParam.push_back(inBroadcast);    
    vInParam.push_back(inRunning);    
    vInParam.push_back(inMult);    
    vInParam.push_back(inPromi);    
    vInParam.push_back(inMtu);    
    vInParam.push_back(inMaster);    
    vInParam.push_back(inAdminState);    
    vInParam.push_back(inOnline);       
    
    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrPortProc[EN_PORT_PROC_ADD],
        vInParam, 
        this,
        EN_PORT_PROC_ADD,
        static_cast<void*> (&proc_ret));
    
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePort::AddPort IProc->Handle failed(%d).\n", ret);
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePort::AddPort proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }

    return VNET_DB_SUCCESS;
}


int32 CDBOperatePort::Modify(CDbPort & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePort::ModifyPort IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inVnaUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetVnaUuid().c_str());    
    CADOParameter inPortType(CADOParameter::paramInt, CADOParameter::paramInput, info.GetPortType());    
    CADOParameter inName(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetName().c_str());
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUuid().c_str());    
    
    CADOParameter inState(CADOParameter::paramInt, CADOParameter::paramInput, info.GetState());    
    CADOParameter inBroadcast(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsBroadcast());    
    CADOParameter inRunning(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsRunning());    
    CADOParameter inMult(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsMulticast());    
    CADOParameter inPromi(CADOParameter::paramInt, CADOParameter::paramInput, info.GetPromiscuous());    
    CADOParameter inMtu(CADOParameter::paramInt, CADOParameter::paramInput, info.GetMtu());        
    CADOParameter inMaster(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsMaster()); 
    CADOParameter inAdminState(CADOParameter::paramInt, CADOParameter::paramInput, info.GetAdminState()); 
    CADOParameter inOnline(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsOnline()); 
    
    vInParam.push_back(inVnaUuid);
    vInParam.push_back(inPortType);
    vInParam.push_back(inName);
    vInParam.push_back(inUuid);    
    vInParam.push_back(inState);    
    vInParam.push_back(inBroadcast);    
    vInParam.push_back(inRunning);    
    vInParam.push_back(inMult);    
    vInParam.push_back(inPromi);    
    vInParam.push_back(inMtu);    
    vInParam.push_back(inMaster);    
    vInParam.push_back(inAdminState);    
    vInParam.push_back(inOnline);       

    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrPortProc[EN_PORT_PROC_MODIFY],
        vInParam, 
        this,
        EN_PORT_PROC_MODIFY,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePort::ModifyPort IProc->Handle failed(%d).\n", ret);
        return ret;
    }

    //修改内容和原纪录内容相同 
    if( proc_ret.GetRet() == VNET_DB_PROCEDURE_RTN_HAS_SAME_ITEM )
    {
        return VNET_DB_SUCCESS;
    }

    if( proc_ret.GetRet() != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePort::ModifyPort proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }
    
    return VNET_DB_SUCCESS;
}

int32 CDBOperatePort::Del(const char* Port_uuid)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePort::DelPort IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == Port_uuid )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePort::DelPort Port_uuid is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;    
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)Port_uuid);    
    
    vInParam.push_back(inUuid);

    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrPortProc[EN_PORT_PROC_DEL],
        vInParam, 
        this,
        EN_PORT_PROC_DEL,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePort::DelPort IProc->Handle failed. ret : %d.\n", ret);
        return ret;
    }
    
    if( proc_ret.GetRet() != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePort::DelPort proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }
    
    return 0;
}

int32 CDBOperatePort::OperateCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePort::OperateCallback param is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    
    CDbProcOperateResult *proc_ret = NULL;     
    proc_ret = static_cast<CDbProcOperateResult *>(nil);

    if( proc_ret == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePort::OperateCallback proc_ret is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 3);    

    int32 nRet = 0;
    if( false == prs->GetFieldValue(0, nRet))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePort::OperateCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetRet(nRet);

    int64 oid = 0;
    if( false == prs->GetFieldValue(1, oid))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePort::OperateCallback GetFieldValue(1) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetOid(oid);
    
    (*proc_ret).SetUuid(prs->GetFieldValue(2)); 

    return 0;
}
#endif 

void CDBOperatePort::DbgShow()
{
#if 0
    cout << "--------------------------Port Info------------------------" << endl;
    // query summary
    vector<CDbPortSummary> vInfo;
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
    vector<CDbPortSummary>::iterator it = vInfo.begin();
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

int32 CDBOperatePortBase::QueryPortBaseCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortBase::QueryPortBaseCallback prs is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
        
    CDbPort *pOutPort;
    pOutPort = static_cast<CDbPort *>(nil);

    if( pOutPort == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortBase::QueryPortBaseCallback pOutPort is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    //VNET_ASSERT(prs->GetFieldCount() == 14);    

    int32 proc_ret = 0;
    if( false == prs->GetFieldValue(0,proc_ret ))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortBase::QueryPortBaseCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }    

    if( 0 != proc_ret )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortBase::QueryPortBaseCallback proc failed(%d).\n", GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }

    (*pOutPort).SetVnaUuid( prs->GetFieldValue(1));
    
    int32 port_type = 0;
    if( false == prs->GetFieldValue(2,port_type) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortBase::QueryPortBaseCallback GetFieldValue failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetPortType(port_type);
    
    (*pOutPort).SetName( prs->GetFieldValue(3));
    (*pOutPort).SetUuid( prs->GetFieldValue(4));
    
    int32 state = 0;
    if( false == prs->GetFieldValue(5,state) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortBase::QueryPortBaseCallback GetFieldValue failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetState(state);

    int32 broadcast = 0;
    if( false == prs->GetFieldValue(6,broadcast) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortBase::QueryPortBaseCallback GetFieldValue failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetIsBroadcast(broadcast);

    int32 running = 0;
    if( false == prs->GetFieldValue(7,running) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortBase::QueryPortBaseCallback GetFieldValue failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetIsRunning(running);

    int32 multicast = 0;
    if( false == prs->GetFieldValue(8,multicast) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortBase::QueryPortBaseCallback GetFieldValue failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetIsMulticast(multicast);

    int32 promis = 0;
    if( false == prs->GetFieldValue(9,promis) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortBase::QueryPortBaseCallback GetFieldValue failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetPromiscuous(promis);

    int32 mtu = 0;
    if( false == prs->GetFieldValue(10,mtu) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortBase::QueryPortBaseCallback GetFieldValue failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetMtu(mtu);

    int32 master = 0;
    if( false == prs->GetFieldValue(11,master) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortBase::QueryPortBaseCallback GetFieldValue failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetIsMaster(master);

    int32 admin = 0;
    if( false == prs->GetFieldValue(12,admin) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortBase::QueryPortBaseCallback GetFieldValue failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetAdminState(admin);

    int32 online = 0;
    if( false == prs->GetFieldValue(13,online) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortBase::QueryPortBaseCallback GetFieldValue failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetIsOnline(online);
    
    int32 value = 0;
    if( false == prs->GetFieldValue(14,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortBase::QueryPortBaseCallback GetFieldValue failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetIsPortCfg(value);

    if( false == prs->GetFieldValue(15,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortBase::QueryPortBaseCallback GetFieldValue failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetIsConsistency(value);
    return 0;
}

int32 CDBOperatePortBase::QueryUuid(const char* vna_uuid, const char* name, int32 port_type, string & port_uuid)
{
    CDBOperatePort * pOper = GetDbIPort();
    if( NULL == pOper )
    {
        return VNET_DB_ERROR_GET_DBI_FAILED;
    }
    CDbPortSummary Info;
    
    int ret= pOper->QuerySummaryByVnaPortName(vna_uuid,name,Info);

    if ( ret != VNET_DB_SUCCESS )
    {
        return ret;
    }

    if(Info.GetPortType() == port_type)
    {

        port_uuid = Info.GetUuid();
        return 0;
    }
    else
    {
        return VNET_DB_ERROR_ITEM_NO_EXIST;
    }    
}

int32 CDBOperatePortBase::CheckPortNameIsConflict(const char* vna_uuid, const char* portName,int32 &nResult )
{
    CDBOperatePort * pOper = GetDbIPort();
    if( NULL == pOper )
    {
        return VNET_DB_ERROR_GET_DBI_FAILED;
    }

    int ret= pOper->CheckPortNameIsConflict(vna_uuid,portName,nResult);

    if ( ret != VNET_DB_SUCCESS )
    {
        return ret;
    }
    return VNET_DB_SUCCESS;
}

#if 0
int32 CDBOperatePortBase::QueryPortBaseSummaryCallBack(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortBase::QueryPortBaseSummaryCallBack prs is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
        
    CDbPortBaseSummary *pOutPort;
    pOutPort = static_cast<CDbPortBaseSummary *>(nil);

    if( pOutPort == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortBase::QueryPortBaseSummaryCallBack pOutPort is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    //VNET_ASSERT(prs->GetFieldCount() == 14);    

    int32 proc_ret = 0;
    if( false == prs->GetFieldValue(0,proc_ret ))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortBase::QueryPortBaseSummaryCallBack GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }    

    if( 0 != proc_ret )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortBase::QueryPortBaseSummaryCallBack proc failed(%d).\n", GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }

    (*pOutPort).SetVnaUuid( prs->GetFieldValue(1));
    
    int32 port_type = 0;
    if( false == prs->GetFieldValue(2,port_type) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortBase::QueryPortBaseSummaryCallBack GetFieldValue failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetPortType(port_type);
    
    (*pOutPort).SetName( prs->GetFieldValue(3));
    (*pOutPort).SetUuid( prs->GetFieldValue(4));
    
    return 0;
}

#endif 

/* test code ------------------------------------------------------------

*/
void db_query_portsummary(const char* vna_uuid)
{
    if( NULL == vna_uuid)
    {
        cout << "vna uuid is NULL" << endl;
        return;
    }
    
    CDBOperatePort * pOper = GetDbIPort();
    if( NULL == pOper )
    {
        cout << "CDBOperateVna is NULL" << endl;
        return ;
    }

    vector<CDbPortSummary> outVInfo;
    int ret = pOper->QuerySummary(vna_uuid,outVInfo);
    if( ret != 0 )
    {
        cout << "QuerySummary failed." << ret << endl;
        return;
    }
    cout << "QuerySummary success." << endl;
    vector<CDbPortSummary>::iterator it = outVInfo.begin();
    for(; it != outVInfo.end(); ++it)
    {
        (*it).DbgShow();
    }
    
}
DEFINE_DEBUG_FUNC(db_query_portsummary);

void db_query_portsummary_by_vnaportname(const char* vna_uuid, const char* name)
{
    if( NULL == vna_uuid)
    {
        cout << "vna uuid is NULL" << endl;
        return;
    }
    
    CDBOperatePort * pOper = GetDbIPort();
    if( NULL == pOper )
    {
        cout << "CDBOperateVna is NULL" << endl;
        return ;
    }

    CDbPortSummary outInfo;
    int ret = pOper->QuerySummaryByVnaPortName(vna_uuid,name,outInfo);
    if( ret != 0 )
    {
        cout << "QuerySummary failed." << ret << endl;
        return;
    }
    cout << "QuerySummary success." << endl;
    outInfo.DbgShow();
    
}
DEFINE_DEBUG_FUNC(db_query_portsummary_by_vnaportname);


void db_query_port(char * uuid)
{
    if( NULL == uuid)
    {
        cout << "Param uuid is NULL" << endl;
        return;
    }
    CDBOperatePort * pOper = GetDbIPort();
    if( NULL == pOper )
    {
        cout << "CDBOperateVna is NULL" << endl;
        return ;
    }


    CDbPort info;
    info.SetUuid(uuid);
    int ret = pOper->Query(info);
    if( ret != 0 )
    {
        cout << "CDBOperatePort query failed." << ret << endl;
        return;
    }
    cout << "CDBOperatePort query success." << endl;
    info.DbgPortShow();
    
}
DEFINE_DEBUG_FUNC(db_query_port);


void db_check_portname_is_conflict(char * vnauuid, char* portName)
{
    if( NULL == vnauuid || NULL == portName)
    {
        cout << "Param uuid is NULL" << endl;
        return;
    }
    CDBOperatePort * pOper = GetDbIPort();
    if( NULL == pOper )
    {
        cout << "CDBOperateVna is NULL" << endl;
        return ;
    }

    
    int32 nResult = 0;
    int ret = pOper->CheckPortNameIsConflict(vnauuid,portName,nResult);
    if( ret != 0 )
    {
        cout << "CheckPortNameIsConflict query failed." << ret << endl;
        return;
    }
    cout << "CheckPortNameIsConflict query success." << nResult << endl;
}
DEFINE_DEBUG_FUNC(db_check_portname_is_conflict);

#if 0
void db_add_port(char* vna_uuid,int32 port_type, char* name, int32 state,int32 Broadcast,
        int32 Running,int32 Multicast,int32 Promiscuous,int32 Mtu,int32 Master,int32 AdminState,int32 is_online)
{
    if( NULL == vna_uuid || NULL == name )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperatePort * pOper = GetDbIPort();
    if( NULL == pOper )
    {
        cout << "CDBOperatePort is NULL" << endl;
        return ;
    }

    CDbPort info;
    info.SetVnaUuid(vna_uuid);
    info.SetPortType(port_type);
    info.SetName(name);
    info.SetState(state);
    info.SetIsBroadcast(Broadcast);
    info.SetIsRunning(Running);
    info.SetIsMulticast(Multicast);
    info.SetPromiscuous(Promiscuous);
    
    Mtu = 1500;
    Master  = AdminState = is_online = 1;    
    //cout << "mtu: " << Mtu <<  endl;
    info.SetMtu(Mtu);    
    info.SetIsMaster(Master);    
    info.SetAdminState(AdminState);
    info.SetIsOnline(is_online);

    //info.DbgShow();
    int ret = pOper->Add(info);
    if( ret != 0 )
    {
        cout << "Add failed." << ret << endl;
        return;
    }
    cout << "Add success." << endl;
    info.DbgPortShow();
}
DEFINE_DEBUG_FUNC(db_add_port);

void db_mod_port(char * port_uuid, char* vna_uuid,int32 port_type, char* name, int32 state,int32 Broadcast,
        int32 Running,int32 Multicast,int32 Promiscuous,int32 Mtu,int32 Master,int32 AdminState,int32 is_online)
{
    if( NULL == port_uuid || NULL == vna_uuid || NULL == name )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperatePort * pOper = GetDbIPort();
    if( NULL == pOper )
    {
        cout << "CDBOperatePort is NULL" << endl;
        return ;
    }

    CDbPort info;
    info.SetUuid(port_uuid);
    info.SetVnaUuid(vna_uuid);
    info.SetPortType(port_type);
    info.SetName(name);
    info.SetState(state);
    info.SetIsBroadcast(Broadcast);
    info.SetIsRunning(Running);
    
    Mtu = 1500;
    Promiscuous = Multicast = Master  = AdminState = is_online = 1;    
    
    info.SetIsMulticast(Multicast);
    info.SetPromiscuous(Promiscuous);
    info.SetMtu(Mtu);    
    info.SetIsMaster(Master);    
    info.SetAdminState(AdminState);
    info.SetIsOnline(is_online);
    
    info.DbgPortShow();
    int ret = pOper->Modify(info);
    if( ret != 0 )
    {
        cout << "Modify failed." << ret << endl;
        return;
    }
    cout << "Modify success." << endl;
    info.DbgPortShow();
}
DEFINE_DEBUG_FUNC(db_mod_port);

void db_del_port(char * uuid)
{
    if(uuid == NULL )
    {
        cout << "Param is NULL" << endl;
        return;
    }

    CDBOperatePort * pOper = GetDbIPort();
    if( NULL == pOper )
    {
        cout << "CDBOperatePort is NULL" << endl;
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
DEFINE_DEBUG_FUNC(db_del_port);

#endif 

}

