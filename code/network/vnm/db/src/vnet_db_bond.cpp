
#include "vnet_db_error.h"
#include "vnet_db_proc.h"
#include "vnet_db_mgr.h"
#include "vnet_db_bond.h"

    
namespace zte_tecs
{
typedef enum tagBondProc
{
    EN_BOND_PROC_QUERYSUMMARY = 0,
    EN_BOND_PROC_QUERY,  
    EN_BOND_PROC_QUERY_SRIOVPORT_VSI_NUM,
    EN_BOND_PROC_CHECK_ADD,
    EN_BOND_PROC_ADD,
    EN_BOND_PROC_CHECK_MODIFY,
    EN_BOND_PROC_MODIFY,
    EN_BOND_PROC_CHECK_DEL,
    EN_BOND_PROC_DEL,
    
    // bond map 
    EN_BONDMAP_PROC_CHECK_ADD,
    EN_BONDMAP_PROC_ADD,
    EN_BONDMAP_PROC_CHECK_MODIFY,
    EN_BONDMAP_PROC_MODIFY,
    EN_BONDMAP_PROC_CHECK_DEL,
    EN_BONDMAP_PROC_DEL,

    // bond run set state
    EN_BOND_BACKUP_ACTIVE_PROC_SET,
    EN_BOND_BACKUP_ACTIVE_PROC_UNSET,
    EN_BOND_LACP_SUCCESS_PROC_SET,
    EN_BOND_LACP_SUCCESS_PROC_UNSET,

    // bond map query
    EN_BONDMAP_QUERY_BY_VNA,
    EN_BONDMAP_QUERY_BY_BOND,
    EN_BONDMAP_QUERY_BY_PHY, 
    
    EN_BOND_PROC_ALL,
}EN_BOND_PROC;

const char *s_astrBondProc[EN_BOND_PROC_ALL] = 
{        
    "pf_net_query_bond_summary",
    "pf_net_query_bond",
    "pf_net_query_bondsriovport_vsi_num",
    "pf_net_check_add_bond",
    "pf_net_add_bond",   
    "pf_net_check_modify_bond",
    "pf_net_modify_bond",  
    "pf_net_check_del_bond",
    "pf_net_del_bond",  
    
    // bond map
    "pf_net_check_add_bondmap",
    "pf_net_add_bondmap",
    "pf_net_check_modify_bondmap",
    "pf_net_modify_bondmap",
    "pf_net_check_del_bondmap",
    "pf_net_del_bondmap",

    // bond run set state
    "pf_net_set_bondbackup_active",
    "pf_net_unset_bondbackup_active",
    "pf_net_set_bondlacp_success",
    "pf_net_unset_bondlacp_success",

    // bond map query
    "pf_net_query_bondmap_by_vna",
    "pf_net_query_bondmap_by_bond",
    "pf_net_query_bondmap_by_phy",
};

    
CDBOperateBond::CDBOperateBond(CVNetDbIProcedure * pProc)
{
    m_pIProc = pProc;
}

CDBOperateBond::~CDBOperateBond()
{
    
}

int32 CDBOperateBond::DbProcCallback(int type, CADORecordset *prs, void* nil )
{
    switch(type)
    {
        case EN_BOND_PROC_QUERYSUMMARY:
        {
            return QuerySummaryCallback(prs,nil);
        }
        break;
        
        case EN_BONDMAP_QUERY_BY_VNA:
        case EN_BONDMAP_QUERY_BY_BOND:
        case EN_BONDMAP_QUERY_BY_PHY:
        {
            return QueryMapCallback(prs,nil);
        }
        break;
        
        case EN_BOND_PROC_QUERY:
        {
            return QueryCallback(prs,nil);
        }
        break;

        case EN_BOND_PROC_QUERY_SRIOVPORT_VSI_NUM:
        {
            return QueryVsiNumCallback(prs,nil);
        }
        break;
        
        case EN_BOND_PROC_ADD:
        case EN_BOND_PROC_MODIFY:
        case EN_BOND_PROC_DEL:
        case EN_BONDMAP_PROC_ADD:
        case EN_BONDMAP_PROC_MODIFY:
        case EN_BONDMAP_PROC_DEL:
        {
            return OperateCallback(prs,nil);
        }
        break;
        case EN_BOND_PROC_CHECK_ADD:
        case EN_BOND_PROC_CHECK_MODIFY:
        case EN_BOND_PROC_CHECK_DEL:
        case EN_BONDMAP_PROC_CHECK_ADD:
        case EN_BONDMAP_PROC_CHECK_MODIFY:
        case EN_BONDMAP_PROC_CHECK_DEL:        
        {
            return CheckOperateCallback(prs,nil);
        }
        break;
        case EN_BOND_BACKUP_ACTIVE_PROC_SET:
        case EN_BOND_BACKUP_ACTIVE_PROC_UNSET:
        case EN_BOND_LACP_SUCCESS_PROC_SET:
        case EN_BOND_LACP_SUCCESS_PROC_UNSET:
        {
            return OperateCallback(prs,nil);
        }
        break;
        default:
        {
            VNET_LOG(VNET_LOG_ERROR, "CDBOperateBond::DbProcCallback type(%d) is invalid.\n",type);
            return VNET_DB_ERROR_PARAM_INVALID;
        }
        break;
    }
    
    return 0;
}
    
int32 CDBOperateBond::QuerySummary(const char* vna_uuid,vector<CDbBondSummary> & outVInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateBond::QuerySummary IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == vna_uuid )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateBond::QuerySummary vna_uuid or sriov_uuid is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // call procedure 
    vector<CADOParameter>  vInParam;
    CADOParameter inVnaUuid(CADOParameter::paramText, CADOParameter::paramInput,(char *)vna_uuid);
    vInParam.push_back(inVnaUuid);    
    
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,\
        s_astrBondProc[EN_BOND_PROC_QUERYSUMMARY],\
        vInParam, \
        this,\
        EN_BOND_PROC_QUERYSUMMARY,\
        static_cast<void*> (&outVInfo));
        
    if( ret != VNET_DB_SUCCESS)
    {
        // dbg 
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {        
            VNET_LOG(VNET_LOG_ERROR,"CDBOperateBond::QuerySummary(%s) IProc->Handle failed(%d) \n",
            vna_uuid,ret);
        }
        return ret;
    }
        
    return 0;
}

int32 CDBOperateBond::QuerySummaryCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateBond::QuerySummaryCallback prs is NULL.\n");
        //VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }

    vector<CDbBondSummary> *pvOut;
    pvOut = static_cast<vector<CDbBondSummary>*>(nil);

    if( pvOut == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateBond::QuerySummaryCallback pvOut is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    CDbBondSummary info;

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 4);
    info.SetVnaUuid(prs->GetFieldValue(0));   
    info.SetBondUuid(prs->GetFieldValue(1));
    info.SetBondName(prs->GetFieldValue(2)); 
    int32 value = 0;
    if( false == prs->GetFieldValue(3,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateBond::QuerySummaryCallback GetFieldValue failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetBondMode(value);

    (*pvOut).push_back(info);

    return 0;
}

int32 CDBOperateBond::QueryVsiNum( const char* bond_uuid, int32 & outVsiNum )
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateBond::QueryVsiNum IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == bond_uuid )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateBond::QueryVsiNum sriovport_uuid is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // call procedure 
    CDbBondSriovVsiNum  Info;
    
    vector<CADOParameter> vInParam;
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)bond_uuid);
    vInParam.push_back(inUuid);
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrBondProc[EN_BOND_PROC_QUERY_SRIOVPORT_VSI_NUM],
        vInParam, 
        this,
        EN_BOND_PROC_QUERY_SRIOVPORT_VSI_NUM,
        static_cast<void*> (&Info));
    if( ret != VNET_DB_SUCCESS )
    {
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR,"CDBOperateBond::QueryVsiNum(%s) IProc->Handle failed(%d).\n",
                bond_uuid,ret);
        }
        return ret;
    }
    
    if( Info.GetRet() != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR, "CDBOperateBond::QueryVsiNum(%s) proc return failed. ret : %d.\n", 
                bond_uuid,GET_DB_ERROR_PROC(Info.GetRet()));
        }
        return GET_DB_ERROR_PROC(Info.GetRet());
    }

    // 设置vsi num 值
    outVsiNum = Info.GetVsiNum();
    
    return 0;
}


int32 CDBOperateBond::QueryVsiNumCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateBond::QueryVsiNumCallback param is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    
    CDbBondSriovVsiNum *proc_ret = NULL;     
    proc_ret = static_cast<CDbBondSriovVsiNum *>(nil);

    if( proc_ret == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateBond::QueryVsiNumCallback proc_ret is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 2);    

    int32 value = 0;
    if( false == prs->GetFieldValue(0, value))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateBond::QueryVsiNumCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetRet(value);

    if( false == prs->GetFieldValue(1, value))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateBond::QueryVsiNumCallback GetFieldValue(1) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetVsiNum(value);

    return 0;
}


int32 CDBOperateBond::Query(CDbBond & Info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateBond::Query IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    vector<CADOParameter> vInParam;
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)Info.GetUuid().c_str());
    vInParam.push_back(inUuid);
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrBondProc[EN_BOND_PROC_QUERY],
        vInParam, 
        this,
        EN_BOND_PROC_QUERY,
        static_cast<void*> (&Info));
    if( ret != VNET_DB_SUCCESS )
    {
        // dbg 
        if(DbgGetDBPrint())
        {
            Info.DbgShow();
        }
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR,"CDBOperateBond::Query IProc->Handle failed(%d).\n",(ret));
        }
        
        return (ret);
    }
    
    return 0;
}

int32 CDBOperateBond::QueryCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateBond::QueryCallback prs is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    if( nil == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateBond::QueryCallback nil is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }
    
    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == (DB_PORT_TABLE_FIELD_NUM +6));    

    // 1 获取port 信息
    int32 proc_ret = 0;
    proc_ret = QueryPortBaseCallback(prs,nil);
    if( 0 != proc_ret )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateBond::QueryCallback QueryPortBaseCallback proc failed(%d).\n", (proc_ret));
        return (proc_ret);
    }

    // 2 获取bond信息
    CDbBond *pOutPort;
    pOutPort = static_cast<CDbBond *>(nil);
    if( pOutPort == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateBond::QueryCallback pOutPort is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // port 14        
    int32 values = 0;
    if( false == prs->GetFieldValue(DB_PORT_TABLE_FIELD_NUM,values) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateBond::QueryCallback GetFieldValue 1 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetBondMode(values);      

    (*pOutPort).SetPolicy( prs->GetFieldValue(DB_PORT_TABLE_FIELD_NUM+1));
    
    if( false == prs->GetFieldValue(DB_PORT_TABLE_FIELD_NUM+2,values) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateBond::QueryCallback GetFieldValue 2 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetIsHasLacpInfo(values);  

    if( false == prs->GetFieldValue(DB_PORT_TABLE_FIELD_NUM+3,values) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateBond::QueryCallback GetFieldValue 3 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetLacpState(values);  
    
    (*pOutPort).SetLacpAggrId( prs->GetFieldValue(DB_PORT_TABLE_FIELD_NUM+4));
    (*pOutPort).SetLacpPartnerMac( prs->GetFieldValue(DB_PORT_TABLE_FIELD_NUM+5));

    return 0;
}

int32 CDBOperateBond::CheckAdd(CDbBond & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateBond::CheckAdd IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // uuid generate
    info.SetUuid(GenerateUUID().c_str());
    
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inVnaUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetVnaUuid().c_str());    
    CADOParameter inPortType(CADOParameter::paramInt, CADOParameter::paramInput, EN_DB_PORT_TYPE_BOND);    
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

    // bond 
    CADOParameter inBondMode(CADOParameter::paramInt, CADOParameter::paramInput, info.GetBondMode());
    CADOParameter inPolicy(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetPolicy().c_str());
        
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
  
    vInParam.push_back(inBondMode); 
    vInParam.push_back(inPolicy); 
        
    // ret struct 
    //CDbProcOperateResult proc_ret; 
    int32 proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrBondProc[EN_BOND_PROC_CHECK_ADD],
        vInParam, 
        this,
        EN_BOND_PROC_CHECK_ADD,
        static_cast<void*> (&proc_ret));
    
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateBond::CheckAdd IProc->Handle failed(%d).\n", ret);
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateBond::CheckAdd proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }

    return VNET_DB_SUCCESS;
}

int32 CDBOperateBond::Add(CDbBond & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateBond::Add IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // uuid generate
    info.SetUuid(GenerateUUID().c_str());
    
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inVnaUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetVnaUuid().c_str());    
    CADOParameter inPortType(CADOParameter::paramInt, CADOParameter::paramInput, EN_DB_PORT_TYPE_BOND);    
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

    // bond 
    CADOParameter inBondMode(CADOParameter::paramInt, CADOParameter::paramInput, info.GetBondMode());
    CADOParameter inPolicy(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetPolicy().c_str());
    CADOParameter inIsHasLacp(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsHasLacpInfo());
    CADOParameter inLacpState(CADOParameter::paramInt, CADOParameter::paramInput, info.GetLacpState());
    CADOParameter inLacpAggrId(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetLacpAggrId().c_str());
    CADOParameter inLacpPartMac(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetLacpPartnerMac().c_str());
    
    
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
  
    vInParam.push_back(inBondMode); 
    vInParam.push_back(inPolicy); 
    vInParam.push_back(inIsHasLacp); 
    vInParam.push_back(inLacpState);
    vInParam.push_back(inLacpAggrId); 
    vInParam.push_back(inLacpPartMac); 
      
    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrBondProc[EN_BOND_PROC_ADD],
        vInParam, 
        this,
        EN_BOND_PROC_ADD,
        static_cast<void*> (&proc_ret));
    
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateBond::Add IProc->Handle failed(%d).\n", ret);
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateBond::Add proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }

    return VNET_DB_SUCCESS;
}

int32 CDBOperateBond::CheckModify(CDbBond & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateBond::CheckModify IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( EN_DB_PORT_TYPE_BOND != info.GetPortType() )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateBond::CheckModify type(%d) is invalid.\n",info.GetPortType());
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

    // bond 
    CADOParameter inBondMode(CADOParameter::paramInt, CADOParameter::paramInput, info.GetBondMode());
    CADOParameter inPolicy(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetPolicy().c_str());
        
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
  
    vInParam.push_back(inBondMode); 
    vInParam.push_back(inPolicy);  
 
    // ret struct 
    // CDbProcOperateResult proc_ret; 
    int32 proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrBondProc[EN_BOND_PROC_CHECK_MODIFY],
        vInParam, 
        this,
        EN_BOND_PROC_CHECK_MODIFY,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateBond::CheckModify IProc->Handle failed(%d).\n", ret);
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateBond::CheckModify proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }
    
    return VNET_DB_SUCCESS;
}

int32 CDBOperateBond::Modify(CDbBond & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateBond::Modify IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( EN_DB_PORT_TYPE_BOND != info.GetPortType() )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateBond::Modify type(%d) is invalid.\n",info.GetPortType());
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

    // bond 
    CADOParameter inBondMode(CADOParameter::paramInt, CADOParameter::paramInput, info.GetBondMode());
    CADOParameter inPolicy(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetPolicy().c_str());
    CADOParameter inIsHasLacp(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsHasLacpInfo());
    CADOParameter inLacpState(CADOParameter::paramInt, CADOParameter::paramInput, info.GetLacpState());
    CADOParameter inLacpAggrId(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetLacpAggrId().c_str());
    CADOParameter inLacpPartMac(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetLacpPartnerMac().c_str());
    
    
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
  
    vInParam.push_back(inBondMode); 
    vInParam.push_back(inPolicy); 
    vInParam.push_back(inIsHasLacp); 
    vInParam.push_back(inLacpState);
    vInParam.push_back(inLacpAggrId); 
    vInParam.push_back(inLacpPartMac); 
 

    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrBondProc[EN_BOND_PROC_MODIFY],
        vInParam, 
        this,
        EN_BOND_PROC_MODIFY,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateBond::Modify IProc->Handle failed(%d).\n", ret);
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateBond::Modify proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }
    
    return VNET_DB_SUCCESS;
}

int32 CDBOperateBond::CheckDel(const char* Port_uuid)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateBond::CheckDel IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == Port_uuid )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateBond::CheckDel Port_uuid is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;    
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)Port_uuid);    
    
    vInParam.push_back(inUuid);

    // ret struct 
    // CDbProcOperateResult proc_ret; 
    int32 proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrBondProc[EN_BOND_PROC_CHECK_DEL],
        vInParam, 
        this,
        EN_BOND_PROC_CHECK_DEL,
        static_cast<void*> (&proc_ret));
        
    if( ret != VNET_DB_SUCCESS )
    {        
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateBond::CheckDel(%s) IProc->Handle failed(%d).\n",Port_uuid, ret);
        return ret;
    }
    
    // 重复添加 
    if( proc_ret == VNET_DB_PROCEDURE_RTN_HAS_SAME_ITEM )
    {
        return VNET_DB_SUCCESS;
    }

    if( proc_ret  != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateBond::CheckDel(%s) proc return failed. ret : %d.\n",Port_uuid, GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }
    
    return 0;
}

int32 CDBOperateBond::Del(const char* Port_uuid)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateBond::Del IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == Port_uuid )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateBond::Del Port_uuid is NULL.\n");
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
        s_astrBondProc[EN_BOND_PROC_DEL],
        vInParam, 
        this,
        EN_BOND_PROC_DEL,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateBond::Del(%s) IProc->Handle failed. ret : %d.\n",Port_uuid, ret);
        return ret;
    }
    
    if( proc_ret.GetRet() != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateBond::Del(%s) proc return failed. ret : %d.\n",Port_uuid, GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }
    
    return 0;
}

int32 CDBOperateBond::OperateCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateBond::OperateCallback param is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    
    CDbProcOperateResult *proc_ret = NULL;     
    proc_ret = static_cast<CDbProcOperateResult *>(nil);

    if( proc_ret == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateBond::OperateCallback proc_ret is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 3);    

    int32 nRet = 0;
    if( false == prs->GetFieldValue(0, nRet))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateBond::OperateCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetRet(nRet);

    int64 oid = 0;
    if( false == prs->GetFieldValue(1, oid))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateBond::OperateCallback GetFieldValue(1) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetOid(oid);
    
    (*proc_ret).SetUuid(prs->GetFieldValue(2)); 

    return 0;
}


int32 CDBOperateBond::CheckOperateCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateBond::CheckOperateCallback param is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    
    int32 *proc_ret = NULL;     
    proc_ret = static_cast<int32 *>(nil);

    if( proc_ret == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateBond::CheckOperateCallback proc_ret is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 1);    

    int32 nRet = 0;
    if( false == prs->GetFieldValue(0, nRet))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateBond::CheckOperateCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret) = nRet;

    return 0;
}


void CDBOperateBond::DbgShow()
{
#if 0
    cout << "--------------------------Port Info------------------------" << endl;
    // query summary
    vector<CDbBondSummary> vInfo;
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
    vector<CDbBondSummary>::iterator it = vInfo.begin();
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
void db_query_bondsummary(const char* vna_uuid)
{
    if( NULL == vna_uuid )
    {
        cout << "vna uuid is NULL" << endl;
        return;
    }
    
    CDBOperateBond * pOper = GetDbIBond();
    if( NULL == pOper )
    {
        cout << "CDBOperateBond is NULL" << endl;
        return ;
    }

    vector<CDbBondSummary> outVInfo;
    int ret = pOper->QuerySummary(vna_uuid,outVInfo);
    if( ret != 0 )
    {
        cout << "QuerySummary failed." << ret << endl;
        return;
    }
    cout << "QuerySummary success." << endl;
    vector<CDbBondSummary>::iterator it = outVInfo.begin();
    for(; it != outVInfo.end(); ++it)
    {
        cout << "---------" << endl;
        (*it).DbgShow();
    }
    
}
DEFINE_DEBUG_FUNC(db_query_bondsummary);


void db_query_bond(char * uuid)
{
    if( NULL == uuid)
    {
        cout << "Param uuid is NULL" << endl;
        return;
    }
    CDBOperateBond * pOper = GetDbIBond();
    if( NULL == pOper )
    {
        cout << "CDBOperateBond is NULL" << endl;
        return ;
    }

    CDbBond info;
    info.SetUuid(uuid);  
    int ret = pOper->Query(info);
    if( ret != 0 )
    {
        cout << "CDBOperateBond query failed." << ret << endl;
        return;
    }
    cout << "CDBOperateBond query success." << endl;
    info.DbgShow();
    
}
DEFINE_DEBUG_FUNC(db_query_bond);


void db_query_bondvsi(char * uuid)
{
    if( NULL == uuid)
    {
        cout << "Param uuid is NULL" << endl;
        return;
    }
    CDBOperateBond * pOper = GetDbIBond();
    if( NULL == pOper )
    {
        cout << "CDBOperateBond is NULL" << endl;
        return ;
    }

    int32 vsiNum = 0; 
    int ret = pOper->QueryVsiNum(uuid,vsiNum);
    if( ret != 0 )
    {
        cout << "CDBOperateBond QueryVsiNum failed." << ret << endl;
        return;
    }
    cout << "CDBOperateBond QueryVsiNum success. vsiNum : " << vsiNum << endl;    
}
DEFINE_DEBUG_FUNC(db_query_bondvsi);

void db_check_add_bond(char* vna_uuid, char* name, int32 is_online,
        int32 is_cfg,int32 bond_mode)
{
    if( NULL == vna_uuid || NULL == name )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateBond * pOper = GetDbIBond();
    if( NULL == pOper )
    {
        cout << "CDBOperateBond is NULL" << endl;
        return ;
    }

    CDbBond info;
    info.SetVnaUuid(vna_uuid);
    info.SetPortType(EN_DB_PORT_TYPE_BOND);
    info.SetName(name);
    info.SetIsOnline(is_online);

    info.SetIsPortCfg(is_cfg);

    info.SetBondMode(bond_mode);
    
    //info.DbgShow();
    int ret = pOper->CheckAdd(info);
    if( ret != 0 )
    {
        cout << "Add failed." << ret << endl;
        return;
    }
    cout << "Add success." << endl;
    info.DbgShow();
}
DEFINE_DEBUG_FUNC(db_check_add_bond);

void db_add_bond(char* vna_uuid, char* name, int32 is_online,
        int32 is_cfg,int32 bond_mode)
{
    if( NULL == vna_uuid || NULL == name )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateBond * pOper = GetDbIBond();
    if( NULL == pOper )
    {
        cout << "CDBOperateBond is NULL" << endl;
        return ;
    }

    CDbBond info;
    info.SetVnaUuid(vna_uuid);
    info.SetPortType(EN_DB_PORT_TYPE_BOND);
    info.SetName(name);
    info.SetIsOnline(is_online);

    info.SetIsPortCfg(is_cfg);

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
DEFINE_DEBUG_FUNC(db_add_bond);


void db_check_mod_bond(char* port_uuid, char* vna_uuid, char* name, int32 is_online,
        int32 is_cfg,int32 bond_mode)
{
    if( NULL == vna_uuid || NULL == name )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateBond * pOper = GetDbIBond();
    if( NULL == pOper )
    {
        cout << "CDBOperateBond is NULL" << endl;
        return ;
    }

    CDbBond info;
    info.SetUuid(port_uuid);
    info.SetPortType(EN_DB_PORT_TYPE_BOND);
    info.SetVnaUuid(vna_uuid);
    info.SetName(name);
    info.SetIsOnline(is_online);

    info.SetIsPortCfg(is_cfg);

    info.SetBondMode(bond_mode);
        
    //info.DbgShow();
    int ret = pOper->CheckModify(info);
    if( ret != 0 )
    {
        cout << "Modify failed." << ret << endl;
        return;
    }
    cout << "Modify success." << endl;
    info.DbgShow();
}
DEFINE_DEBUG_FUNC(db_check_mod_bond);



void db_mod_bond(char* port_uuid, char* vna_uuid, char* name, int32 is_online,
        int32 is_cfg,int32 bond_mode)
{
    if( NULL == vna_uuid || NULL == name )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateBond * pOper = GetDbIBond();
    if( NULL == pOper )
    {
        cout << "CDBOperateBond is NULL" << endl;
        return ;
    }

    CDbBond info;
    info.SetUuid(port_uuid);
    info.SetPortType(EN_DB_PORT_TYPE_BOND);
    info.SetVnaUuid(vna_uuid);
    info.SetName(name);
    info.SetIsOnline(is_online);

    info.SetIsPortCfg(is_cfg);

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
DEFINE_DEBUG_FUNC(db_mod_bond);

void db_check_del_bond(char * uuid)
{
    if(uuid == NULL )
    {
        cout << "Param is NULL" << endl;
        return;
    }

    CDBOperateBond * pOper = GetDbIBond();
    if( NULL == pOper )
    {
        cout << "CDBOperateBond is NULL" << endl;
        return ;
    }

    int ret = pOper->CheckDel(uuid);
    if( ret != 0 )
    {
        cout << "Del failed." << ret << endl;
        return;
    }
    cout << "Del success." << endl;
}
DEFINE_DEBUG_FUNC(db_check_del_bond);


void db_del_bond(char * uuid)
{
    if(uuid == NULL )
    {
        cout << "Param is NULL" << endl;
        return;
    }

    CDBOperateBond * pOper = GetDbIBond();
    if( NULL == pOper )
    {
        cout << "CDBOperateBond is NULL" << endl;
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
DEFINE_DEBUG_FUNC(db_del_bond);


// bond map 部分
int32 CDBOperateBond::CheckAddMap(CDBBondMap & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateBond::AddPort IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }
    
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inVnaUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetVnaUuid().c_str());    
    CADOParameter inBondUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetBondUuid().c_str());    
    CADOParameter inPhyUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetPhyUuid().c_str());    
    CADOParameter inIsCfg(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsCfg());
    CADOParameter inIsSelect(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsSelect()); 
    
    vInParam.push_back(inVnaUuid);
    vInParam.push_back(inBondUuid);
    vInParam.push_back(inPhyUuid);    
    vInParam.push_back(inIsCfg);    
    vInParam.push_back(inIsSelect);  
        
    // ret struct 
    //CDbProcOperateResult proc_ret; 
    int32 proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrBondProc[EN_BONDMAP_PROC_CHECK_ADD],
        vInParam, 
        this,
        EN_BONDMAP_PROC_CHECK_ADD,
        static_cast<void*> (&proc_ret));
    
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateBond::CheckAddMap IProc->Handle failed(%d).\n", ret);
        return ret;
    }
    
    // 重复添加 
    if( proc_ret == VNET_DB_PROCEDURE_RTN_HAS_SAME_ITEM )
    {
        return VNET_DB_SUCCESS;
    }

    if( proc_ret  != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateBond::CheckAddMap proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }

    return VNET_DB_SUCCESS;
}

int32 CDBOperateBond::AddMap(CDBBondMap & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateBond::AddMap IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }
    
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inVnaUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetVnaUuid().c_str());    
    CADOParameter inBondUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetBondUuid().c_str());    
    CADOParameter inPhyUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetPhyUuid().c_str());    
    CADOParameter inIsCfg(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsCfg());
    CADOParameter inIsSelect(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsSelect()); 
    
    vInParam.push_back(inVnaUuid);
    vInParam.push_back(inBondUuid);
    vInParam.push_back(inPhyUuid);    
    vInParam.push_back(inIsCfg);    
    vInParam.push_back(inIsSelect);  
      
    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrBondProc[EN_BONDMAP_PROC_ADD],
        vInParam, 
        this,
        EN_BONDMAP_PROC_ADD,
        static_cast<void*> (&proc_ret));
    
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateBond::AddMap IProc->Handle failed(%d).\n", ret);
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateBond::AddMap proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }

    return VNET_DB_SUCCESS;
}


int32 CDBOperateBond::CheckModifyMap(CDBBondMap & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateBond::CheckModifyMap IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;  
    CADOParameter inVnaUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetVnaUuid().c_str());    
    CADOParameter inBondUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetBondUuid().c_str());    
    CADOParameter inPhyUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetPhyUuid().c_str());    
    CADOParameter inIsCfg(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsCfg());
    CADOParameter inIsSelect(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsSelect()); 
    
    vInParam.push_back(inVnaUuid);
    vInParam.push_back(inBondUuid);
    vInParam.push_back(inPhyUuid);    
    vInParam.push_back(inIsCfg);    
    vInParam.push_back(inIsSelect);  
 
    // ret struct 
    // CDbProcOperateResult proc_ret; 
    int32 proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrBondProc[EN_BONDMAP_PROC_CHECK_MODIFY],
        vInParam, 
        this,
        EN_BONDMAP_PROC_CHECK_MODIFY,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateBond::CheckModifyMap IProc->Handle failed(%d).\n", ret);
        return ret;
    }

    // 重复添加 
    if( proc_ret == VNET_DB_PROCEDURE_RTN_HAS_SAME_ITEM )
    {
        return VNET_DB_SUCCESS;
    }

    if( proc_ret  != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateBond::CheckModifyMap proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }
    
    return VNET_DB_SUCCESS;
}

int32 CDBOperateBond::ModifyMap(CDBBondMap & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateBond::ModifyMap IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inVnaUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetVnaUuid().c_str());    
    CADOParameter inBondUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetBondUuid().c_str());    
    CADOParameter inPhyUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetPhyUuid().c_str());    
    CADOParameter inIsCfg(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsCfg());
    CADOParameter inIsSelect(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsSelect()); 
    
    vInParam.push_back(inVnaUuid);
    vInParam.push_back(inBondUuid);
    vInParam.push_back(inPhyUuid);    
    vInParam.push_back(inIsCfg);    
    vInParam.push_back(inIsSelect);  
 

    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrBondProc[EN_BONDMAP_PROC_MODIFY],
        vInParam, 
        this,
        EN_BONDMAP_PROC_MODIFY,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateBond::ModifyMap IProc->Handle failed(%d).\n", ret);
        return ret;
    }

    //修改内容和原纪录内容相同 
    if( proc_ret.GetRet() == VNET_DB_PROCEDURE_RTN_HAS_SAME_ITEM )
    {
        return VNET_DB_SUCCESS;
    }

    if( proc_ret.GetRet() != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateBond::ModifyMap proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }
    
    return VNET_DB_SUCCESS;
}

int32 CDBOperateBond::CheckDelMap(const char* vna_uuid,const char* bond_uuid, const char* phy_uuid)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateBond::CheckDelMap IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == vna_uuid || NULL == bond_uuid || NULL == phy_uuid)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateBond::CheckDelMap Port_uuid is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;    
    CADOParameter inVnaUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)vna_uuid);    
    CADOParameter inBondUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)bond_uuid);    
    CADOParameter inPhyUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)phy_uuid);    
    
    vInParam.push_back(inVnaUuid);
    vInParam.push_back(inBondUuid);
    vInParam.push_back(inPhyUuid);    

    // ret struct 
    // CDbProcOperateResult proc_ret; 
    int32 proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrBondProc[EN_BONDMAP_PROC_CHECK_DEL],
        vInParam, 
        this,
        EN_BONDMAP_PROC_CHECK_DEL,
        static_cast<void*> (&proc_ret));
        
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateBond::CheckDelMap IProc->Handle failed(%d).\n", ret);
        return ret;
    }
    
    // 重复添加 
    if( proc_ret == VNET_DB_PROCEDURE_RTN_HAS_SAME_ITEM )
    {
        return VNET_DB_SUCCESS;
    }

    if( proc_ret  != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateBond::CheckDelMap proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }
    
    return 0;
}

int32 CDBOperateBond::DelMap(const char* vna_uuid,const char* bond_uuid, const char* phy_uuid)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateBond::DelMap IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

   if( NULL == vna_uuid || NULL == bond_uuid || NULL == phy_uuid)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateBond::DelMap Port_uuid is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;    
    CADOParameter inVnaUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)vna_uuid);    
    CADOParameter inBondUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)bond_uuid);    
    CADOParameter inPhyUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)phy_uuid);    
    
    vInParam.push_back(inVnaUuid);
    vInParam.push_back(inBondUuid);
    vInParam.push_back(inPhyUuid);    

    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrBondProc[EN_BONDMAP_PROC_DEL],
        vInParam, 
        this,
        EN_BONDMAP_PROC_DEL,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateBond::DelMap IProc->Handle failed. ret : %d.\n", ret);
        return ret;
    }
    
    if( proc_ret.GetRet() != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateBond::DelMap proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }
    
    return 0;
}

int32 CDBOperateBond::SetBackupActiveNic(const char* bond_uuid, const char* phy_uuid)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateBond::SetBackupActiveNic IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

   if(  NULL == bond_uuid || NULL == phy_uuid)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateBond::SetBackupActiveNic Port_uuid is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;        
    CADOParameter inBondUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)bond_uuid);    
    CADOParameter inPhyUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)phy_uuid);    
    
    vInParam.push_back(inBondUuid);
    vInParam.push_back(inPhyUuid);    

    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrBondProc[EN_BOND_BACKUP_ACTIVE_PROC_SET],
        vInParam, 
        this,
        EN_BOND_BACKUP_ACTIVE_PROC_SET,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateBond::SetBackupActiveNic IProc->Handle failed. ret : %d.\n", ret);
        return ret;
    }
    
    if( proc_ret.GetRet() != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateBond::SetBackupActiveNic proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }
    
    return 0;
}

int32 CDBOperateBond::UnsetBackupActiveNic(const char* bond_uuid, const char* phy_uuid)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateBond::UnsetBackupActiveNic IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

   if(  NULL == bond_uuid || NULL == phy_uuid)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateBond::UnsetBackupActiveNic Port_uuid is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;        
    CADOParameter inBondUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)bond_uuid);    
    CADOParameter inPhyUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)phy_uuid);    
    
    vInParam.push_back(inBondUuid);
    vInParam.push_back(inPhyUuid);    

    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrBondProc[EN_BOND_BACKUP_ACTIVE_PROC_UNSET],
        vInParam, 
        this,
        EN_BOND_BACKUP_ACTIVE_PROC_UNSET,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateBond::UnsetBackupActiveNic IProc->Handle failed. ret : %d.\n", ret);
        return ret;
    }
    
    if( proc_ret.GetRet() != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateBond::UnsetBackupActiveNic proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }
    
    return 0;
}

int32 CDBOperateBond::SetLacpSuccessNic(const char* bond_uuid, const char* phy_uuid)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateBond::SetLacpSuccessNic IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

   if(  NULL == bond_uuid || NULL == phy_uuid)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateBond::SetLacpSuccessNic Port_uuid is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;        
    CADOParameter inBondUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)bond_uuid);    
    CADOParameter inPhyUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)phy_uuid);    
    
    vInParam.push_back(inBondUuid);
    vInParam.push_back(inPhyUuid);    

    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrBondProc[EN_BOND_LACP_SUCCESS_PROC_SET],
        vInParam, 
        this,
        EN_BOND_LACP_SUCCESS_PROC_SET,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateBond::SetLacpSuccessNic IProc->Handle failed. ret : %d.\n", ret);
        return ret;
    }
    
    if( proc_ret.GetRet() != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateBond::SetLacpSuccessNic proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }
    
    return 0;
}

int32 CDBOperateBond::UnsetLacpSuccessNic(const char* bond_uuid, const char* phy_uuid)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateBond::UnsetLacpSuccessNic IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

   if(  NULL == bond_uuid || NULL == phy_uuid)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateBond::UnsetLacpSuccessNic Port_uuid is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;        
    CADOParameter inBondUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)bond_uuid);    
    CADOParameter inPhyUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)phy_uuid);    
    
    vInParam.push_back(inBondUuid);
    vInParam.push_back(inPhyUuid);    

    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrBondProc[EN_BOND_LACP_SUCCESS_PROC_UNSET],
        vInParam, 
        this,
        EN_BOND_LACP_SUCCESS_PROC_UNSET,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateBond::UnsetLacpSuccessNic IProc->Handle failed. ret : %d.\n", ret);
        return ret;
    }
    
    if( proc_ret.GetRet() != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateBond::SetLacpSuccessNic proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }
    
    return 0;
}

// bondmap query
int32 CDBOperateBond::QueryMapByVna(const char* vna_uuid,vector<CDBBondMap> & outVInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateBond::QueryMapByVna IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == vna_uuid )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateBond::QueryMapByVna vna_uuid or sriov_uuid is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // call procedure 
    vector<CADOParameter>  vInParam;
    CADOParameter inVnaUuid(CADOParameter::paramText, CADOParameter::paramInput,(char *)vna_uuid);
    vInParam.push_back(inVnaUuid);    
    
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,\
        s_astrBondProc[EN_BONDMAP_QUERY_BY_VNA],\
        vInParam, \
        this,\
        EN_BONDMAP_QUERY_BY_VNA,\
        static_cast<void*> (&outVInfo));
        
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateBond::QueryMapByVna IProc->Handle failed(%d) \n",ret);
        return ret;
    }
        
    return 0;
}

int32 CDBOperateBond::QueryMapByBond(const char* bond_uuid,vector<CDBBondMap> & outVInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateBond::QueryMapByBond IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == bond_uuid )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateBond::QueryMapByBond bond_uuid or sriov_uuid is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // call procedure 
    vector<CADOParameter>  vInParam;
    CADOParameter inBondUuid(CADOParameter::paramText, CADOParameter::paramInput,(char *)bond_uuid);
    vInParam.push_back(inBondUuid);    
    
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,\
        s_astrBondProc[EN_BONDMAP_QUERY_BY_BOND],\
        vInParam, \
        this,\
        EN_BONDMAP_QUERY_BY_BOND,\
        static_cast<void*> (&outVInfo));
        
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateBond::QueryMapByBond IProc->Handle failed(%d) \n",ret);
        return ret;
    }
        
    return 0;
}

int32 CDBOperateBond::QueryMapByPhy(const char* phy_uuid,vector<CDBBondMap> & outVInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateBond::QueryMapByBond IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == phy_uuid )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateBond::QueryMapByBond phy_uuid or sriov_uuid is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // call procedure 
    vector<CADOParameter>  vInParam;
    CADOParameter inPhyUuid(CADOParameter::paramText, CADOParameter::paramInput,(char *)phy_uuid);
    vInParam.push_back(inPhyUuid);    
    
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,\
        s_astrBondProc[EN_BONDMAP_QUERY_BY_PHY],\
        vInParam, \
        this,\
        EN_BONDMAP_QUERY_BY_PHY,\
        static_cast<void*> (&outVInfo));
        
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateBond::QueryMapByBond IProc->Handle failed(%d) \n",ret);
        return ret;
    }
        
    return 0;
}

int32 CDBOperateBond::QueryMapCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateBond::QueryMapCallback prs is NULL.\n");
        //VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }

    vector<CDBBondMap> *pvOut;
    pvOut = static_cast<vector<CDBBondMap>*>(nil);

    if( pvOut == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateBond::QueryMapCallback pvOut is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    CDBBondMap info;

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 10);
    info.SetVnaUuid(prs->GetFieldValue(0));   
    info.SetBondUuid(prs->GetFieldValue(1));
    info.SetBondName(prs->GetFieldValue(2)); 
    info.SetPhyUuid(prs->GetFieldValue(3));
    info.SetPhyName(prs->GetFieldValue(4));
    
    int32 value = 0;
    if( false == prs->GetFieldValue(5,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateBond::QueryMapCallback GetFieldValue failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetBondMode(value);

    if( false == prs->GetFieldValue(6,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateBond::QueryMapCallback GetFieldValue failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetLacpIsSuc(value);

    if( false == prs->GetFieldValue(7,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateBond::QueryMapCallback GetFieldValue failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetBackupIsActive(value);
    
    if( false == prs->GetFieldValue(8,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateBond::QueryMapCallback GetFieldValue failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetIsCfg(value);

    if( false == prs->GetFieldValue(9,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateBond::QueryMapCallback GetFieldValue failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetIsSelect(value);

    (*pvOut).push_back(info);

    return 0;
}

// -- test code 
void db_query_bondmapbyvna(const char* vna_uuid)
{
    if( NULL == vna_uuid )
    {
        cout << "vna uuid is NULL" << endl;
        return;
    }
    
    CDBOperateBond * pOper = GetDbIBond();
    if( NULL == pOper )
    {
        cout << "CDBOperateBond is NULL" << endl;
        return ;
    }

    vector<CDBBondMap> outVInfo;
    int ret = pOper->QueryMapByVna(vna_uuid,outVInfo);
    if( ret != 0 )
    {
        cout << "QueryMapByVna failed." << ret << endl;
        return;
    }
    cout << "QueryMapByVna success." << endl;
    vector<CDBBondMap>::iterator it = outVInfo.begin();
    for(; it != outVInfo.end(); ++it)
    {
        cout << "------------------------" << endl;
        (*it).DbgShow();
    }
    
}
DEFINE_DEBUG_FUNC(db_query_bondmapbyvna);

void db_query_bondmapbybond(const char* bond_uuid)
{
    if( NULL == bond_uuid )
    {
        cout << "bond uuid is NULL" << endl;
        return;
    }
    
    CDBOperateBond * pOper = GetDbIBond();
    if( NULL == pOper )
    {
        cout << "CDBOperateBond is NULL" << endl;
        return ;
    }

    vector<CDBBondMap> outVInfo;
    int ret = pOper->QueryMapByBond(bond_uuid,outVInfo);
    if( ret != 0 )
    {
        cout << "QueryMapByBond failed." << ret << endl;
        return;
    }
    cout << "QueryMapByBond success." << endl;
    vector<CDBBondMap>::iterator it = outVInfo.begin();
    for(; it != outVInfo.end(); ++it)
    {
        cout << "---------" << endl;
        (*it).DbgShow();
    }
    
}
DEFINE_DEBUG_FUNC(db_query_bondmapbybond);

void db_query_bondmapbyphy(const char* phy_uuid)
{
    if( NULL == phy_uuid )
    {
        cout << "bond uuid is NULL" << endl;
        return;
    }
    
    CDBOperateBond * pOper = GetDbIBond();
    if( NULL == pOper )
    {
        cout << "CDBOperateBond is NULL" << endl;
        return ;
    }

    vector<CDBBondMap> outVInfo;
    int ret = pOper->QueryMapByPhy(phy_uuid,outVInfo);
    if( ret != 0 )
    {
        cout << "QueryMapByPhy failed." << ret << endl;
        return;
    }
    cout << "QueryMapByPhy success." << endl;
    vector<CDBBondMap>::iterator it = outVInfo.begin();
    for(; it != outVInfo.end(); ++it)
    {
        cout << "---------" << endl;
        (*it).DbgShow();
    }
    
}
DEFINE_DEBUG_FUNC(db_query_bondmapbyphy);


// add 
void db_check_add_bondmap(char* vna_uuid, char* bond_uuid,char * phy_uuid,
        int32 is_cfg,int32 is_select )
{
    if( NULL == vna_uuid || NULL == bond_uuid || NULL == phy_uuid )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateBond * pOper = GetDbIBond();
    if( NULL == pOper )
    {
        cout << "CDBOperateBond is NULL" << endl;
        return ;
    }

    CDBBondMap info;
    info.SetVnaUuid(vna_uuid);
    info.SetBondUuid(bond_uuid);
    info.SetPhyUuid(phy_uuid);

    info.SetIsCfg(is_cfg);

    info.SetIsSelect(is_select);
    
    //info.DbgShow();
    int ret = pOper->CheckAddMap(info);
    if( ret != 0 )
    {
        cout << "CheckAddMap failed." << ret << endl;
        return;
    }
    cout << "CheckAddMap success." << endl;
    info.DbgShow();
}
DEFINE_DEBUG_FUNC(db_check_add_bondmap);

void db_add_bondmap(char* vna_uuid, char* bond_uuid,char * phy_uuid,
        int32 is_cfg,int32 is_select )
{
     if( NULL == vna_uuid || NULL == bond_uuid || NULL == phy_uuid )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateBond * pOper = GetDbIBond();
    if( NULL == pOper )
    {
        cout << "CDBOperateBond is NULL" << endl;
        return ;
    }

    CDBBondMap info;
    info.SetVnaUuid(vna_uuid);
    info.SetBondUuid(bond_uuid);
    info.SetPhyUuid(phy_uuid);

    info.SetIsCfg(is_cfg);

    info.SetIsSelect(is_select);
    
    //info.DbgShow();
    int ret = pOper->AddMap(info);
    if( ret != 0 )
    {
        cout << "AddMap failed." << ret << endl;
        return;
    }
    cout << "AddMap success." << endl;
    info.DbgShow();
}
DEFINE_DEBUG_FUNC(db_add_bondmap);


void db_check_mod_bondmap(char* vna_uuid, char* bond_uuid,char * phy_uuid,
        int32 is_cfg,int32 is_select )
{
    if( NULL == vna_uuid || NULL == bond_uuid || NULL == phy_uuid )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateBond * pOper = GetDbIBond();
    if( NULL == pOper )
    {
        cout << "CDBOperateBond is NULL" << endl;
        return ;
    }

    CDBBondMap info;
    info.SetVnaUuid(vna_uuid);
    info.SetBondUuid(bond_uuid);
    info.SetPhyUuid(phy_uuid);

    info.SetIsCfg(is_cfg);

    info.SetIsSelect(is_select);
    
    //info.DbgShow();
    int ret = pOper->CheckModifyMap(info);
    if( ret != 0 )
    {
        cout << "CheckModifyMap failed." << ret << endl;
        return;
    }
    cout << "CheckModifyMap success." << endl;
    info.DbgShow();
}
DEFINE_DEBUG_FUNC(db_check_mod_bondmap);

void db_mod_bondmap(char* vna_uuid, char* bond_uuid,char * phy_uuid,
        int32 is_cfg,int32 is_select )
{
     if( NULL == vna_uuid || NULL == bond_uuid || NULL == phy_uuid )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateBond * pOper = GetDbIBond();
    if( NULL == pOper )
    {
        cout << "CDBOperateBond is NULL" << endl;
        return ;
    }

    CDBBondMap info;
    info.SetVnaUuid(vna_uuid);
    info.SetBondUuid(bond_uuid);
    info.SetPhyUuid(phy_uuid);

    info.SetIsCfg(is_cfg);

    info.SetIsSelect(is_select);
    
    //info.DbgShow();
    int ret = pOper->ModifyMap(info);
    if( ret != 0 )
    {
        cout << "ModifyMap failed." << ret << endl;
        return;
    }
    cout << "ModifyMap success." << endl;
    info.DbgShow();
}
DEFINE_DEBUG_FUNC(db_mod_bondmap);


void db_check_del_bondmap(char* vna_uuid, char* bond_uuid,char * phy_uuid)
{
    if( NULL == vna_uuid || NULL == bond_uuid || NULL == phy_uuid )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateBond * pOper = GetDbIBond();
    if( NULL == pOper )
    {
        cout << "CDBOperateBond is NULL" << endl;
        return ;
    }
    
    //info.DbgShow();
    int ret = pOper->CheckDelMap(vna_uuid,bond_uuid,phy_uuid);
    if( ret != 0 )
    {
        cout << "CheckDelMap failed." << ret << endl;
        return;
    }
    cout << "CheckDelMap success." << endl;
}
DEFINE_DEBUG_FUNC(db_check_del_bondmap);

void db_del_bondmap(char* vna_uuid, char* bond_uuid,char * phy_uuid)
{
    if( NULL == vna_uuid || NULL == bond_uuid || NULL == phy_uuid )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateBond * pOper = GetDbIBond();
    if( NULL == pOper )
    {
        cout << "CDBOperateBond is NULL" << endl;
        return ;
    }
    
    //info.DbgShow();
    int ret = pOper->DelMap(vna_uuid,bond_uuid,phy_uuid);
    if( ret != 0 )
    {
        cout << "CheckDelMap failed." << ret << endl;
        return;
    }
    cout << "CheckDelMap success." << endl;
}
DEFINE_DEBUG_FUNC(db_del_bondmap);


void db_bond_backup_active(char* bond_uuid,char * phy_uuid, int flag)
{
    if( NULL == bond_uuid || NULL == phy_uuid )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateBond * pOper = GetDbIBond();
    if( NULL == pOper )
    {
        cout << "CDBOperateBond is NULL" << endl;
        return ;
    }
    
    //info.DbgShow();
    
    int ret = 0;
    if(flag)
    {
        ret = pOper->SetBackupActiveNic(bond_uuid,phy_uuid);
    }
    else
    {
        ret = pOper->UnsetBackupActiveNic(bond_uuid,phy_uuid);
    }

    cout << flag << endl;
    if( ret != 0 )
    {
        cout << "db_bond_backup_active failed." << ret << endl;
        return;
    }
    cout << "db_bond_backup_active success." << endl;
}
DEFINE_DEBUG_FUNC(db_bond_backup_active);

void db_bond_lacp_success(char* bond_uuid,char * phy_uuid, int flag)
{
    if( NULL == bond_uuid || NULL == phy_uuid )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateBond * pOper = GetDbIBond();
    if( NULL == pOper )
    {
        cout << "CDBOperateBond is NULL" << endl;
        return ;
    }
    
    //info.DbgShow();
    
    int ret = 0;
    if(flag)
    {
        ret = pOper->SetLacpSuccessNic(bond_uuid,phy_uuid);
    }
    else
    {
        ret = pOper->UnsetLacpSuccessNic(bond_uuid,phy_uuid);
    }

    cout << flag << endl;
    if( ret != 0 )
    {
        cout << "db_bond_backup_active failed." << ret << endl;
        return;
    }
    cout << "db_bond_backup_active success." << endl;
}
DEFINE_DEBUG_FUNC(db_bond_lacp_success);


}

