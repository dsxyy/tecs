

#include "vnet_db_error.h"
#include "vnet_db_proc.h"
#include "vnet_db_mgr.h"
#include "vnet_db_port_vtep.h"
    
namespace zte_tecs
{
typedef enum tagPortVtepProc
{
    EN_PORT_VTEP_PROC_QUERYSUMMARY = 0,
    EN_PORT_VTEP_PROC_QUERY,     
    EN_PORT_VTEP_PROC_ADD,
    EN_PORT_VTEP_PROC_MODIFY,
    EN_PORT_VTEP_PROC_DEL,
    EN_PORT_VTEP_PROC_ALL,
}EN_PORT_VTEP_PROC;

const char *s_astrPortVtepProc[EN_PORT_VTEP_PROC_ALL] = 
{        
    "pf_net_query_vtepport_summary",
    "pf_net_query_vtepport",
    "pf_net_add_vtepport",    
    "pf_net_modify_vtepport",  
    "pf_net_del_vtepport",  
};

    
CDBOperatePortVtep::CDBOperatePortVtep(CVNetDbIProcedure * pProc)
{
    m_pIProc = pProc;
}

CDBOperatePortVtep::~CDBOperatePortVtep()
{
    
}

int32 CDBOperatePortVtep::DbProcCallback(int type, CADORecordset *prs, void* nil )
{
    switch(type)
    {
        case EN_PORT_VTEP_PROC_QUERYSUMMARY:
        {
            return QuerySummaryCallback(prs,nil);
        }
        break;
        case EN_PORT_VTEP_PROC_QUERY:
        {
            return QueryCallback(prs,nil);
        }
        break;
        case EN_PORT_VTEP_PROC_ADD:
        case EN_PORT_VTEP_PROC_MODIFY:
        case EN_PORT_VTEP_PROC_DEL:
        {
            return OperateCallback(prs,nil);
        }
        break;
        default:
        {
            VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortVtep::DbProcCallback type(%d) is invalid.\n",type);
            return VNET_DB_ERROR_PARAM_INVALID;
        }
        break;
    }
    
    return 0;
}
    
int32 CDBOperatePortVtep::QuerySummary(const char* vna_uuid,vector<CDbPortSummary> & outVInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortVtep::QuerySummary IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == vna_uuid )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortVtep::QuerySummary vna_uuid or sriov_uuid is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // call procedure 
    vector<CADOParameter>  vInParam;
    CADOParameter inVnaUuid(CADOParameter::paramText, CADOParameter::paramInput,(char *)vna_uuid);
    vInParam.push_back(inVnaUuid);    
    
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,\
        s_astrPortVtepProc[EN_PORT_VTEP_PROC_QUERYSUMMARY],\
        vInParam, \
        this,\
        EN_PORT_VTEP_PROC_QUERYSUMMARY,\
        static_cast<void*> (&outVInfo));
        
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortVtep::QuerySummary IProc->Handle failed(%d) \n",ret);
        return ret;
    }
        
    return 0;
}

int32 CDBOperatePortVtep::QuerySummaryCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortVtep::QueryPortSummaryCallback prs is NULL.\n");
        //VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }

    vector<CDbPortSummary> *pvOut;
    pvOut = static_cast<vector<CDbPortSummary>*>(nil);

    if( pvOut == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortVtep::QueryPortSummaryCallback pvOut is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    CDbPortSummary info;

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 4);
    info.SetVnaUuid(prs->GetFieldValue(0));
    info.SetUuid(prs->GetFieldValue(1));
    info.SetName(prs->GetFieldValue(2));    
    int32 port_type = 0;
    if( false == prs->GetFieldValue(3,port_type) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortVtep::QueryPortSummaryCallback GetFieldValue failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetPortType(port_type);

    (*pvOut).push_back(info);

    return 0;
}

int32 CDBOperatePortVtep::Query(CDbPortVtep & Info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortVtep::Query IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    vector<CADOParameter> vInParam;
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)Info.GetUuid().c_str());
    vInParam.push_back(inUuid);
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrPortVtepProc[EN_PORT_VTEP_PROC_QUERY],
        vInParam, 
        this,
        EN_PORT_VTEP_PROC_QUERY,
        static_cast<void*> (&Info));
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            Info.DbgShow();
        }
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortVtep::Query IProc->Handle failed(%d).\n",ret);
        }
        return ret;
    }
    
    return 0;
}

int32 CDBOperatePortVtep::QueryCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortVtep::QueryCallback prs is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    if( nil == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortVtep::QueryCallback nil is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }
    
    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == (DB_PORT_TABLE_FIELD_NUM +5));    

    // 1 获取port 信息
    int32 proc_ret = 0;
    proc_ret = QueryPortBaseCallback(prs,nil);
    if( 0 != proc_ret )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortVtep::QueryCallback QueryPortBaseCallback proc failed(%d).\n", (proc_ret));
        return (proc_ret);
    }

    // 2 获取vtep 信息
    CDbPortVtep *pOutPort;
    pOutPort = static_cast<CDbPortVtep *>(nil);
    if( pOutPort == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortVtep::QueryCallback pOutPort is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // port 14    
    (*pOutPort).SetVswitchUuid( prs->GetFieldValue(DB_PORT_TABLE_FIELD_NUM));
    (*pOutPort).SetIp( prs->GetFieldValue(DB_PORT_TABLE_FIELD_NUM + 1));
    (*pOutPort).SetMask( prs->GetFieldValue(DB_PORT_TABLE_FIELD_NUM + 2));

    
    int64 value64 = 0;
    if( false == prs->GetFieldValue(DB_PORT_TABLE_FIELD_NUM + 3,value64) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortVtep::QueryCallback GetFieldValue failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetIpNum(value64);

    if( false == prs->GetFieldValue(DB_PORT_TABLE_FIELD_NUM + 4,value64) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortVtep::QueryCallback GetFieldValue failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetMaskNum(value64);
        
    return 0;
}

int32 CDBOperatePortVtep::Add(CDbPortVtep & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortVtep::Add IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }
   
    // uuid generate
    info.SetUuid(GenerateUUID().c_str());
    
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inVnaUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetVnaUuid().c_str());    
    CADOParameter inPortType(CADOParameter::paramInt, CADOParameter::paramInput, EN_DB_PORT_TYPE_VTEP);    
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
    CADOParameter inIsPortCfg(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsPortCfg()); 
    CADOParameter inIsConsistency(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsConsistency()); 

    // vtep port 
    CADOParameter inVsUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetVswitchUuid().c_str());
    CADOParameter inIp(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetIp().c_str());
    CADOParameter inMask(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetMask().c_str());
    CADOParameter inIpNum(CADOParameter::paramInt64, CADOParameter::paramInput, info.GetIpNum());    
    CADOParameter inMaskNum(CADOParameter::paramInt64, CADOParameter::paramInput, info.GetMaskNum());
    
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
    vInParam.push_back(inIsPortCfg); 
    vInParam.push_back(inIsConsistency);    
 
    vInParam.push_back(inVsUuid); 
    vInParam.push_back(inIpNum); 
    vInParam.push_back(inMaskNum);
    vInParam.push_back(inIp); 
    vInParam.push_back(inMask);    
  
    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrPortVtepProc[EN_PORT_VTEP_PROC_ADD],
        vInParam, 
        this,
        EN_PORT_VTEP_PROC_ADD,
        static_cast<void*> (&proc_ret));
    
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortVtep::Add IProc->Handle failed(%d).\n", ret);
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortVtep::Add proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }

    return VNET_DB_SUCCESS;
}


int32 CDBOperatePortVtep::Modify(CDbPortVtep & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortVtep::Modify IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( EN_DB_PORT_TYPE_VTEP != info.GetPortType() )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortVtep::Modify type(%d) is invalid.\n",info.GetPortType());
        return VNET_DB_ERROR_PARAM_INVALID;
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
    CADOParameter inIsPortCfg(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsPortCfg()); 
    CADOParameter inIsConsistency(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsConsistency()); 


    // vtep port 
    CADOParameter inVsUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetVswitchUuid().c_str());
    CADOParameter inIp(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetIp().c_str());
    CADOParameter inMask(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetMask().c_str());
    CADOParameter inIpNum(CADOParameter::paramInt64, CADOParameter::paramInput, info.GetIpNum());    
    CADOParameter inMaskNum(CADOParameter::paramInt64, CADOParameter::paramInput, info.GetMaskNum());
    
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
    vInParam.push_back(inIsPortCfg); 
    vInParam.push_back(inIsConsistency);    
 
    vInParam.push_back(inVsUuid);  
    vInParam.push_back(inIpNum); 
    vInParam.push_back(inMaskNum);
    vInParam.push_back(inIp); 
    vInParam.push_back(inMask);       

    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrPortVtepProc[EN_PORT_VTEP_PROC_MODIFY],
        vInParam, 
        this,
        EN_PORT_VTEP_PROC_MODIFY,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortVtep::Modify IProc->Handle failed(%d).\n", ret);
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortVtep::Modify proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }
    
    return VNET_DB_SUCCESS;
}

int32 CDBOperatePortVtep::Del(const char* Port_uuid)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortVtep::Del IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == Port_uuid )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortVtep::Del Port_uuid is NULL.\n");
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
        s_astrPortVtepProc[EN_PORT_VTEP_PROC_DEL],
        vInParam, 
        this,
        EN_PORT_VTEP_PROC_DEL,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortVtep::Del(%s) IProc->Handle failed. ret : %d.\n",Port_uuid, ret);
        return ret;
    }
    
    if( proc_ret.GetRet() != VNET_DB_PROCEDURE_RTN_SUCCESS 
        && proc_ret.GetRet() != VNET_DB_PROCEDURE_RTN_ITEM_NO_EXIST )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortVtep::Del(%s) proc return failed. ret : %d.\n", Port_uuid,GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }
    
    return 0;
}

int32 CDBOperatePortVtep::OperateCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortVtep::OperateCallback param is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    
    CDbProcOperateResult *proc_ret = NULL;     
    proc_ret = static_cast<CDbProcOperateResult *>(nil);

    if( proc_ret == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortVtep::OperateCallback proc_ret is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 3);    

    int32 nRet = 0;
    if( false == prs->GetFieldValue(0, nRet))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortVtep::OperateCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetRet(nRet);

    int64 oid = 0;
    if( false == prs->GetFieldValue(1, oid))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortVtep::OperateCallback GetFieldValue(1) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetOid(oid);
    
    (*proc_ret).SetUuid(prs->GetFieldValue(2)); 

    return 0;
}

void CDBOperatePortVtep::DbgShow()
{
}

/* test code ------------------------------------------------------------

*/
void db_query_vtepportsummary(const char* vna_uuid)
{
    if( NULL == vna_uuid )
    {
        cout << "vna uuid is NULL" << endl;
        return;
    }
    
    CDBOperatePortVtep * pOper = GetDbIPortVtep();
    if( NULL == pOper )
    {
        cout << "CDBOperatePortVtep is NULL" << endl;
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
DEFINE_DEBUG_FUNC(db_query_vtepportsummary);

void db_query_vtepport(char * uuid)
{
    if( NULL == uuid)
    {
        cout << "Param uuid is NULL" << endl;
        return;
    }
    CDBOperatePortVtep * pOper = GetDbIPortVtep();
    if( NULL == pOper )
    {
        cout << "CDBOperatePortVtep is NULL" << endl;
        return ;
    }

    CDbPortVtep info;
    info.SetUuid(uuid);  
    int ret = pOper->Query(info);
    if( ret != 0 )
    {
        cout << "CDBOperatePortVtep query failed." << ret << endl;
        return;
    }
    cout << "CDBOperatePortVtep query success." << endl;
    info.DbgShow();
    
}
DEFINE_DEBUG_FUNC(db_query_vtepport);

void db_add_vtepport(char* vna_uuid,int32 port_type, char* name, int32 is_online,
        char* switch_uuid, int64 ip,int64 mask)
{
    if( NULL == vna_uuid || NULL == name )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperatePortVtep * pOper = GetDbIPortVtep();
    if( NULL == pOper )
    {
        cout << "CDBOperatePortVtep is NULL" << endl;
        return ;
    }

    CDbPortVtep info;
    info.SetVnaUuid(vna_uuid);
    info.SetPortType(port_type);
    info.SetName(name);
    info.SetIsOnline(is_online);

    info.SetVswitchUuid(switch_uuid);
    info.SetIpNum(ip);
    info.SetMaskNum(mask);
    
    info.DbgShow();
    int ret = pOper->Add(info);
    if( ret != 0 )
    {
        cout << "Add failed." << ret << endl;
        return;
    }
    cout << "Add success." << endl;
    info.DbgShow();
}
DEFINE_DEBUG_FUNC(db_add_vtepport);

void db_mod_vtepport(char* port_uuid, char* vna_uuid,char* name, int32 is_online,
        char* switch_uuid, int64 ip,int64 mask)
{
    if( NULL == vna_uuid || NULL == name )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperatePortVtep * pOper = GetDbIPortVtep();
    if( NULL == pOper )
    {
        cout << "CDBOperatePortVtep is NULL" << endl;
        return ;
    }

    CDbPortVtep info;
    info.SetUuid(port_uuid);
    info.SetPortType(EN_DB_PORT_TYPE_VTEP);
    info.SetVnaUuid(vna_uuid);
    info.SetName(name);
    info.SetIsOnline(is_online);
    
    info.SetVswitchUuid(switch_uuid);
    info.SetIpNum(ip);
    info.SetMaskNum(mask);
        
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
DEFINE_DEBUG_FUNC(db_mod_vtepport);

void db_del_vtepport(char * uuid)
{
    if(uuid == NULL )
    {
        cout << "Param is NULL" << endl;
        return;
    }

    CDBOperatePortVtep * pOper = GetDbIPortVtep();
    if( NULL == pOper )
    {
        cout << "CDBOperatePortVtep is NULL" << endl;
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
DEFINE_DEBUG_FUNC(db_del_vtepport);


}


