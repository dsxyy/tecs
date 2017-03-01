


#include "vnet_db_error.h"
#include "vnet_db_proc.h"
#include "vnet_db_mgr.h"
#include "vnet_db_portgroup.h"
    
namespace zte_tecs
{

int32 CheckPortGroupType(int32 type)
{
    int32 ret = 0;
    switch(type)
    {

        case EN_DB_PORTGROUP_TYPE_UPLINK:
        case EN_DB_PORTGROUP_TYPE_KERNEL_MGR:
        case EN_DB_PORTGROUP_TYPE_KERNEL_STORAGE:
        case EN_DB_PORTGROUP_TYPE_VM_SHARE:
        case EN_DB_PORTGROUP_TYPE_VM_PRIVATE:
        {
        }
        break;
        default: 
        {
             ret = -1;
        }
        break;
    }
    return ret;
}

int32 CheckSwitchPortModeType(int32 type)
{
    int32 ret = 0;
    switch(type)
    {

        case EN_DB_SWITCHPORTMODE_TYPE_TRUNK:
        case EN_DB_SWITCHPORTMODE_TYPE_ACCESS:
        {
        }
        break;
        default: 
        {
             ret = -1;
        }
        break;
    }
    return ret;    
}


typedef enum tagPortGroupProc
{
    EN_PORTGROUP_PROC_QUERYSUMMARY = 0,
    EN_PORTGROUP_PROC_QUERY,     
    EN_PORTGROUP_PROC_ADD,
    EN_PORTGROUP_PROC_MODIFY,
    EN_PORTGROUP_PROC_DEL,
    EN_PORTGROUP_PROC_ISUSING,
    EN_PORTGROUP_PROC_ALL,
}EN_PORT_KERNEL_PROC;

const char *s_astrPortGrouplProc[EN_PORTGROUP_PROC_ALL] = 
{        
    "pf_net_query_portgroup_summary",
    "pf_net_query_portgroup",
    "pf_net_add_portgroup",    
    "pf_net_modify_portgroup",  
    "pf_net_del_portgroup",  
    "pf_net_check_del_portgroup", 
};

    
CDBOperatePortGroup::CDBOperatePortGroup(CVNetDbIProcedure * pProc)
{
    m_pIProc = pProc;
}

CDBOperatePortGroup::~CDBOperatePortGroup()
{
    
}

int32 CDBOperatePortGroup::DbProcCallback(int type, CADORecordset *prs, void* nil )
{
    switch(type)
    {
        case EN_PORTGROUP_PROC_QUERYSUMMARY:
        {
            return QuerySummaryCallback(prs,nil);
        }
        break;
        case EN_PORTGROUP_PROC_QUERY:
        {
            return QueryCallback(prs,nil);
        }
        break;
        case EN_PORTGROUP_PROC_ADD:
        case EN_PORTGROUP_PROC_MODIFY:
        case EN_PORTGROUP_PROC_DEL:
        {
            return OperateCallback(prs,nil);
        }
        break;
        case EN_PORTGROUP_PROC_ISUSING:
        {
            return CheckOperateCallback(prs,nil);
        }
        break;
        default:
        {
            VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortGroup::DbProcCallback type(%d) is invalid.\n",type);
            return VNET_DB_ERROR_PARAM_INVALID;
        }
        break;
    }
    
    return 0;
}
    
int32 CDBOperatePortGroup::QuerySummary(vector<CDbPortGroupSummary> & outVInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortGroup::QuerySummary IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    vector<CADOParameter>  vInParam;
    
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,\
        s_astrPortGrouplProc[EN_PORTGROUP_PROC_QUERYSUMMARY],\
        vInParam, \
        this,\
        EN_PORTGROUP_PROC_QUERYSUMMARY,\
        static_cast<void*> (&outVInfo));
        
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortGroup::QuerySummary IProc->Handle failed(%d) \n",ret);
        return ret;
    }
        
    return 0;
}

int32 CDBOperatePortGroup::QuerySummaryCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortGroup::QueryPortSummaryCallback prs is NULL.\n");
        //VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }

    vector<CDbPortGroupSummary> *pvOut;
    pvOut = static_cast<vector<CDbPortGroupSummary>*>(nil);

    if( pvOut == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortGroup::QueryPortSummaryCallback pvOut is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    CDbPortGroupSummary info;

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 4);
    info.SetUuid(prs->GetFieldValue(0));
    info.SetName(prs->GetFieldValue(1)); 
    
    int32 value = 0;
    if( false == prs->GetFieldValue(2,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortGroup::QueryPortSummaryCallback GetFieldValue failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetPgType(value);

    if( false == prs->GetFieldValue(3,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortGroup::QueryPortSummaryCallback GetFieldValue failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetSwithportMode(value);

    (*pvOut).push_back(info);

    return 0;
}

int32 CDBOperatePortGroup::Query(CDbPortGroup & Info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortGroup::Query IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    vector<CADOParameter> vInParam;
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)Info.GetUuid().c_str());
    vInParam.push_back(inUuid);
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrPortGrouplProc[EN_PORTGROUP_PROC_QUERY],
        vInParam, 
        this,
        EN_PORTGROUP_PROC_QUERY,
        static_cast<void*> (&Info));
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            Info.DbgShow();
        }
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortGroup::Query IProc->Handle failed(%d).\n",ret);
        }
        return ret;
    }
    
    return 0;
}

int32 CDBOperatePortGroup::QueryCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortGroup::QueryCallback prs is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    if( nil == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortGroup::QueryCallback nil is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    CDbPortGroup *pOutPort;
    pOutPort = static_cast<CDbPortGroup *>(nil);
    if( pOutPort == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortGroup::QueryCallback pOutPort is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 32);    

    int32 proc_ret = 0;
    if( false == prs->GetFieldValue(0,proc_ret ))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortBase::QueryCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }    

    if( 0 != proc_ret )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortBase::QueryCallback proc failed(%d).\n", GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }

    (*pOutPort).SetName( prs->GetFieldValue(1));  
    
    int32 values = 0;
    if( false == prs->GetFieldValue(2,values) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortGroup::QueryCallback GetFieldValue 2 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetPgType(values);  
    
    (*pOutPort).SetUuid( prs->GetFieldValue(3));
    (*pOutPort).SetAcl( prs->GetFieldValue(4));
    (*pOutPort).SetQos( prs->GetFieldValue(5));
    (*pOutPort).SetAllowPriorities( prs->GetFieldValue(6));    
    (*pOutPort).SetRcvBandwidthLmt( prs->GetFieldValue(7));    
    (*pOutPort).SetRcvBandwidthRsv( prs->GetFieldValue(8)); 

    if( false == prs->GetFieldValue(9,values) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortGroup::QueryCallback GetFieldValue 9 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetDftVlanId(values);  

    if( false == prs->GetFieldValue(10,values) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortGroup::QueryCallback GetFieldValue 10 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetPromiscuous(values);  

    if( false == prs->GetFieldValue(11,values) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortGroup::QueryCallback GetFieldValue 11 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetMacVlanFltEnable(values);  
        
    (*pOutPort).SetAllowTrmtMacs( prs->GetFieldValue(12));    
    (*pOutPort).SetAllowTrmtMacVlans( prs->GetFieldValue(13));    
    (*pOutPort).SetPlyBlkOver( prs->GetFieldValue(14));    
    (*pOutPort).SetPlyVlanOver( prs->GetFieldValue(15));   

    if( false == prs->GetFieldValue(16,values) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortGroup::QueryCallback GetFieldValue 16 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetVersion(values); 

    
    (*pOutPort).SetMgrId( prs->GetFieldValue(17));    
    (*pOutPort).SetTypeId( prs->GetFieldValue(18));  
    
    if( false == prs->GetFieldValue(19,values) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortGroup::QueryCallback GetFieldValue 19 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetAllowMacChg(values); 
    
    if( false == prs->GetFieldValue(20,values) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortGroup::QueryCallback GetFieldValue 20 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetSwithportMode(values); 

    if( false == prs->GetFieldValue(21,values) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortGroup::QueryCallback GetFieldValue 21 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetIsCfgNetplane(values); 
    
    (*pOutPort).SetNetplaneUuid( prs->GetFieldValue(22));  

    if( false == prs->GetFieldValue(23,values) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortGroup::QueryCallback GetFieldValue 23 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetMtu(values); 

    if( false == prs->GetFieldValue(24,values) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortGroup::QueryCallback GetFieldValue 24 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetTrunkNatvieVlanNum(values); 

    if( false == prs->GetFieldValue(25,values) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortGroup::QueryCallback GetFieldValue 25 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetAccessVlanNum(values); 

    if( false == prs->GetFieldValue(26,values) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortGroup::QueryCallback GetFieldValue 26 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetAccessIsolateNo(values); 

    (*pOutPort).SetDesc( prs->GetFieldValue(27)); 

    if( false == prs->GetFieldValue(28,values) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortGroup::QueryCallback GetFieldValue 30 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetIsolateType(values); 

    if( false == prs->GetFieldValue(29,values) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortGroup::QueryCallback GetFieldValue 31 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetSegmentIdNum(values); 

    if( false == prs->GetFieldValue(30,values) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortGroup::QueryCallback GetFieldValue 32 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetVxlanIsolateNo(values); 

    if( false == prs->GetFieldValue(31,values) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortGroup::QueryCallback GetFieldValue 32 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetIsSdn(values); 

    return 0;
}

int32 CDBOperatePortGroup::Add(CDbPortGroup & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortGroup::Add IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( 0 != CheckPortGroupType( info.GetPgType() ))
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortGroup::Add pg type(%d) is invalid.\n",info.GetPgType() );
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    if( 0 != CheckSwitchPortModeType( info.GetSwithportMode() ))
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortGroup::Add switchport mode(%d) is invalid.\n",info.GetSwithportMode() );
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // uuid generate
    info.SetUuid(GenerateUUID().c_str());
    
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   

    CADOParameter inName(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetName().c_str());
    CADOParameter inPgType(CADOParameter::paramInt, CADOParameter::paramInput, info.GetPgType());    
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUuid().c_str());    
    CADOParameter inAcl(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetAcl().c_str());    
    CADOParameter inQos(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetQos().c_str());    
    CADOParameter inAllowPriorities(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetAllowPriorities().c_str());    
    CADOParameter inRcvBandwidthLmt(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetRcvBandwidthLmt().c_str());    
    CADOParameter inRcvBandwidthRsv(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetRcvBandwidthRsv().c_str());    
    CADOParameter inDftVlanId(CADOParameter::paramInt, CADOParameter::paramInput, info.GetDftVlanId());    
    CADOParameter inPromiscuous(CADOParameter::paramInt, CADOParameter::paramInput, info.GetPromiscuous());    
    CADOParameter inMacVlanFltEnable(CADOParameter::paramInt, CADOParameter::paramInput, info.GetMacVlanFltEnable());    
    CADOParameter inAllowTrmtMacs(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetAllowTrmtMacs().c_str());    
    CADOParameter inAllowTrmtMacVlans(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetAllowTrmtMacVlans().c_str());    
    CADOParameter inPlyBlkOver(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetPlyBlkOver().c_str());    
    CADOParameter inPlyVlanOver(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetPlyVlanOver().c_str());    
    CADOParameter inVersion(CADOParameter::paramInt, CADOParameter::paramInput, info.GetVersion());    
    CADOParameter inMgrId(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetMgrId().c_str());    
    CADOParameter inTypeId(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetTypeId().c_str());    
    CADOParameter inAllowMacChg(CADOParameter::paramInt, CADOParameter::paramInput, info.GetAllowMacChg());    
    CADOParameter inSwithportMode(CADOParameter::paramInt, CADOParameter::paramInput, info.GetSwithportMode());    
    CADOParameter inIsCfgNetplane(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsCfgNetplane());    
    CADOParameter inNetplaneUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetNetplaneUuid().c_str());    
    CADOParameter inMtu(CADOParameter::paramInt, CADOParameter::paramInput, info.GetMtu());    
    CADOParameter inTrunkNatvieVlanNum(CADOParameter::paramInt, CADOParameter::paramInput, info.GetTrunkNatvieVlanNum());    
    CADOParameter inAccessVlanNum(CADOParameter::paramInt, CADOParameter::paramInput, info.GetAccessVlanNum());    
    CADOParameter inAccessIsolateNo(CADOParameter::paramInt, CADOParameter::paramInput, info.GetAccessIsolateNo());        
    CADOParameter inDesc(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetDesc().c_str());
    CADOParameter inIsolateType(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsolateType());
    CADOParameter inSegmentId(CADOParameter::paramInt, CADOParameter::paramInput, info.GetSegmentIdNum());
    CADOParameter inVxlanIsolate(CADOParameter::paramInt, CADOParameter::paramInput, info.GetVxlanIsolateNo());
    CADOParameter inSdn(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsSdn());
    
    vInParam.push_back(inName);
    vInParam.push_back(inPgType);
    vInParam.push_back(inUuid);
    vInParam.push_back(inAcl);
    vInParam.push_back(inQos);
    vInParam.push_back(inAllowPriorities);
    vInParam.push_back(inRcvBandwidthLmt);
    vInParam.push_back(inRcvBandwidthRsv);
    vInParam.push_back(inDftVlanId);
    vInParam.push_back(inPromiscuous);
    vInParam.push_back(inMacVlanFltEnable);
    vInParam.push_back(inAllowTrmtMacs);
    vInParam.push_back(inAllowTrmtMacVlans);
    vInParam.push_back(inPlyBlkOver);
    vInParam.push_back(inPlyVlanOver);
    vInParam.push_back(inVersion);
    vInParam.push_back(inMgrId);
    vInParam.push_back(inTypeId);
    vInParam.push_back(inAllowMacChg);
    vInParam.push_back(inSwithportMode);
    vInParam.push_back(inIsCfgNetplane);
    vInParam.push_back(inNetplaneUuid);
    vInParam.push_back(inMtu);
    vInParam.push_back(inTrunkNatvieVlanNum);
    vInParam.push_back(inAccessVlanNum);
    vInParam.push_back(inAccessIsolateNo);
    vInParam.push_back(inDesc);    
    vInParam.push_back(inIsolateType);    
    vInParam.push_back(inSegmentId); 
    vInParam.push_back(inVxlanIsolate);
    vInParam.push_back(inSdn);
  
    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrPortGrouplProc[EN_PORTGROUP_PROC_ADD],
        vInParam, 
        this,
        EN_PORTGROUP_PROC_ADD,
        static_cast<void*> (&proc_ret));
    
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortGroup::Add IProc->Handle failed(%d).\n", ret);
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortGroup::Add proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }

    return VNET_DB_SUCCESS;
}


int32 CDBOperatePortGroup::Modify(CDbPortGroup & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortGroup::Modify IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( 0 != CheckPortGroupType( info.GetPgType() ))
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortGroup::Modify pg type(%d) is invalid.\n",info.GetPgType() );
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    if( 0 != CheckSwitchPortModeType( info.GetSwithportMode() ))
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortGroup::Modify switchport mode(%d) is invalid.\n",info.GetSwithportMode() );
        return VNET_DB_ERROR_PARAM_INVALID;
    }


    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    
    CADOParameter inName(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetName().c_str());
    CADOParameter inPgType(CADOParameter::paramInt, CADOParameter::paramInput, info.GetPgType());    
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUuid().c_str());    
    CADOParameter inAcl(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetAcl().c_str());    
    CADOParameter inQos(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetQos().c_str());    
    CADOParameter inAllowPriorities(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetAllowPriorities().c_str());    
    CADOParameter inRcvBandwidthLmt(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetRcvBandwidthLmt().c_str());    
    CADOParameter inRcvBandwidthRsv(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetRcvBandwidthRsv().c_str());    
    CADOParameter inDftVlanId(CADOParameter::paramInt, CADOParameter::paramInput, info.GetDftVlanId());    
    CADOParameter inPromiscuous(CADOParameter::paramInt, CADOParameter::paramInput, info.GetPromiscuous());    
    CADOParameter inMacVlanFltEnable(CADOParameter::paramInt, CADOParameter::paramInput, info.GetMacVlanFltEnable());    
    CADOParameter inAllowTrmtMacs(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetAllowTrmtMacs().c_str());    
    CADOParameter inAllowTrmtMacVlans(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetAllowTrmtMacVlans().c_str());    
    CADOParameter inPlyBlkOver(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetPlyBlkOver().c_str());    
    CADOParameter inPlyVlanOver(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetPlyVlanOver().c_str());    
    CADOParameter inVersion(CADOParameter::paramInt, CADOParameter::paramInput, info.GetVersion());    
    CADOParameter inMgrId(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetMgrId().c_str());    
    CADOParameter inTypeId(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetTypeId().c_str());    
    CADOParameter inAllowMacChg(CADOParameter::paramInt, CADOParameter::paramInput, info.GetAllowMacChg());    
    CADOParameter inSwithportMode(CADOParameter::paramInt, CADOParameter::paramInput, info.GetSwithportMode());    
    CADOParameter inIsCfgNetplane(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsCfgNetplane());    
    CADOParameter inNetplaneUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetNetplaneUuid().c_str());    
    CADOParameter inMtu(CADOParameter::paramInt, CADOParameter::paramInput, info.GetMtu());    
    CADOParameter inTrunkNatvieVlanNum(CADOParameter::paramInt, CADOParameter::paramInput, info.GetTrunkNatvieVlanNum());    
    CADOParameter inAccessVlanNum(CADOParameter::paramInt, CADOParameter::paramInput, info.GetAccessVlanNum());    
    CADOParameter inAccessIsolateNo(CADOParameter::paramInt, CADOParameter::paramInput, info.GetAccessIsolateNo());        
    CADOParameter inDesc(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetDesc().c_str());
    CADOParameter inIsolateType(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsolateType());
    CADOParameter inSegmentId(CADOParameter::paramInt, CADOParameter::paramInput, info.GetSegmentIdNum());
    CADOParameter inVxlanIsolate(CADOParameter::paramInt, CADOParameter::paramInput, info.GetVxlanIsolateNo());
    CADOParameter inSdn(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsSdn());
    
    vInParam.push_back(inName);
    vInParam.push_back(inPgType);
    vInParam.push_back(inUuid);
    vInParam.push_back(inAcl);
    vInParam.push_back(inQos);
    vInParam.push_back(inAllowPriorities);
    vInParam.push_back(inRcvBandwidthLmt);
    vInParam.push_back(inRcvBandwidthRsv);
    vInParam.push_back(inDftVlanId);
    vInParam.push_back(inPromiscuous);
    vInParam.push_back(inMacVlanFltEnable);
    vInParam.push_back(inAllowTrmtMacs);
    vInParam.push_back(inAllowTrmtMacVlans);
    vInParam.push_back(inPlyBlkOver);
    vInParam.push_back(inPlyVlanOver);
    vInParam.push_back(inVersion);
    vInParam.push_back(inMgrId);
    vInParam.push_back(inTypeId);
    vInParam.push_back(inAllowMacChg);
    vInParam.push_back(inSwithportMode);
    vInParam.push_back(inIsCfgNetplane);
    vInParam.push_back(inNetplaneUuid);
    vInParam.push_back(inMtu);
    vInParam.push_back(inTrunkNatvieVlanNum);
    vInParam.push_back(inAccessVlanNum);
    vInParam.push_back(inAccessIsolateNo);
    vInParam.push_back(inDesc);
    vInParam.push_back(inIsolateType);    
    vInParam.push_back(inSegmentId);
    vInParam.push_back(inVxlanIsolate);
    vInParam.push_back(inSdn);

    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrPortGrouplProc[EN_PORTGROUP_PROC_MODIFY],
        vInParam, 
        this,
        EN_PORTGROUP_PROC_MODIFY,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortGroup::Modify IProc->Handle failed(%d).\n", ret);
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortGroup::Modify proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }
    
    return VNET_DB_SUCCESS;
}

int32 CDBOperatePortGroup::Del(const char* Port_uuid)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortGroup::Del IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == Port_uuid )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortGroup::Del Port_uuid is NULL.\n");
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
        s_astrPortGrouplProc[EN_PORTGROUP_PROC_DEL],
        vInParam, 
        this,
        EN_PORTGROUP_PROC_DEL,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortGroup::Del(%s) IProc->Handle failed. ret : %d.\n",
            Port_uuid,ret);
        return ret;
    }
    
    if( proc_ret.GetRet() != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortGroup::Del(%s) proc return failed. ret : %d.\n", 
            Port_uuid,GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }
    
    return 0;
}

int32 CDBOperatePortGroup::IsUsing(const char* switch_uuid)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortGroup::IsUsing IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == switch_uuid )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortGroup::IsUsing switch_uuid is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;    
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)switch_uuid);    
    
    vInParam.push_back(inUuid);

    // ret struct 
    int32 proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrPortGrouplProc[EN_PORTGROUP_PROC_ISUSING],
        vInParam, 
        this,
        EN_PORTGROUP_PROC_ISUSING,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortGroup::IsUsing(%s) IProc->Handle failed. ret : %d.\n",switch_uuid, ret);
        return ret;
    }
    
    if( proc_ret != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortGroup::IsUsing(%s) proc return failed. ret : %d.\n",switch_uuid, GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }
    
    return 0;
}


int32 CDBOperatePortGroup::CheckOperateCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortGroup::CheckOperateCallback param is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    
    int32 *proc_ret = NULL;     
    proc_ret = static_cast<int32 *>(nil);

    if( proc_ret == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortGroup::CheckOperateCallback proc_ret is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 1);    

    int32 nRet = 0;
    if( false == prs->GetFieldValue(0, nRet))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortGroup::CheckOperateCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret) = nRet;

    return 0;
}

int32 CDBOperatePortGroup::OperateCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortGroup::OperateCallback param is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    
    CDbProcOperateResult *proc_ret = NULL;     
    proc_ret = static_cast<CDbProcOperateResult *>(nil);

    if( proc_ret == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortGroup::OperateCallback proc_ret is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 3);    

    int32 nRet = 0;
    if( false == prs->GetFieldValue(0, nRet))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortGroup::OperateCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetRet(nRet);

    int64 oid = 0;
    if( false == prs->GetFieldValue(1, oid))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortGroup::OperateCallback GetFieldValue(1) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetOid(oid);
    
    (*proc_ret).SetUuid(prs->GetFieldValue(2)); 

    return 0;
}

void CDBOperatePortGroup::DbgShow()
{
#if 0
    cout << "--------------------------Port Info------------------------" << endl;
    // query summary
    vector<CDbPortGroupSummary> vInfo;
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
    vector<CDbPortGroupSummary>::iterator it = vInfo.begin();
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
void db_query_portgroupsummary()
{    
    CDBOperatePortGroup * pOper = GetDbIPortGroup();
    if( NULL == pOper )
    {
        cout << "CDBOperatePortGroup is NULL" << endl;
        return ;
    }

    vector<CDbPortGroupSummary> outVInfo;
    int ret = pOper->QuerySummary(outVInfo);
    if( ret != 0 )
    {
        cout << "QuerySummary failed." << ret << endl;
        return;
    }
    cout << "QuerySummary success." << endl;
    vector<CDbPortGroupSummary>::iterator it = outVInfo.begin();
    for(; it != outVInfo.end(); ++it)
    {
        cout << "-------------" << endl;
        (*it).DbgShow();
    }
    
}
DEFINE_DEBUG_FUNC(db_query_portgroupsummary);

void db_query_portgroup(char * uuid)
{
    if( NULL == uuid)
    {
        cout << "Param uuid is NULL" << endl;
        return;
    }
    CDBOperatePortGroup * pOper = GetDbIPortGroup();
    if( NULL == pOper )
    {
        cout << "CDBOperatePortGroup is NULL" << endl;
        return ;
    }

    CDbPortGroup info;
    info.SetUuid(uuid);  
    int ret = pOper->Query(info);
    if( ret != 0 )
    {
        cout << "CDBOperatePortGroup query failed." << ret << endl;
        return;
    }
    cout << "CDBOperatePortGroup query success." << endl;
    info.DbgShow();
    
}
DEFINE_DEBUG_FUNC(db_query_portgroup);

void db_add_portgroup(char* name,int32 pg_type, int32 switchport_mode, int32 trunk_vlan_num,
        int32 access_vlan_num, int32 access_isolate, char* desc)
{
    if(  NULL == name )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperatePortGroup * pOper = GetDbIPortGroup();
    if( NULL == pOper )
    {
        cout << "CDBOperatePortGroup is NULL" << endl;
        return ;
    }

    CDbPortGroup info;
    info.SetSwithportMode(switchport_mode);
    info.SetPgType(pg_type);
    info.SetName(name);
    info.SetTrunkNatvieVlanNum(trunk_vlan_num);
    info.SetAccessVlanNum(access_vlan_num);
    info.SetAccessIsolateNo(access_isolate);
    info.SetDesc(desc);    
    
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
DEFINE_DEBUG_FUNC(db_add_portgroup);

void db_mod_portgroup(char* uuid, char* name,int32 pg_type, int32 switchport_mode, int32 trunk_vlan_num,
        int32 access_vlan_num, int32 access_isolate,char* desc)
{
    if( NULL == uuid || NULL == name )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperatePortGroup * pOper = GetDbIPortGroup();
    if( NULL == pOper )
    {
        cout << "CDBOperatePortGroup is NULL" << endl;
        return ;
    }

    CDbPortGroup info;
    info.SetUuid(uuid);
    info.SetSwithportMode(switchport_mode);
    info.SetPgType(pg_type);
    info.SetName(name);
    info.SetTrunkNatvieVlanNum(trunk_vlan_num);
    info.SetAccessVlanNum(access_vlan_num);
    info.SetAccessIsolateNo(access_isolate);
    info.SetDesc(desc);
        
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
DEFINE_DEBUG_FUNC(db_mod_portgroup);



void db_del_portgroup(char * uuid)
{
    if(uuid == NULL )
    {
        cout << "Param is NULL" << endl;
        return;
    }

    CDBOperatePortGroup * pOper = GetDbIPortGroup();
    if( NULL == pOper )
    {
        cout << "CDBOperatePortGroup is NULL" << endl;
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
DEFINE_DEBUG_FUNC(db_del_portgroup);


void db_isusing_portgroup(char * uuid)
{
    if(uuid == NULL )
    {
        cout << "Param is NULL" << endl;
        return;
    }

    CDBOperatePortGroup * pOper = GetDbIPortGroup();
    if( NULL == pOper )
    {
        cout << "CDBOperatePortGroup is NULL" << endl;
        return ;
    }

    int ret = pOper->IsUsing(uuid);
    if( ret != 0 )
    {
        if( VNET_DB_IS_RESOURCE_USING(ret))
        {
           cout << "is using." << endl;
           return;
        }
        
        cout << "IsUsing failed." << ret << endl;
        return;
    }
    cout << "no use." << endl;
}
DEFINE_DEBUG_FUNC(db_isusing_portgroup);
}




