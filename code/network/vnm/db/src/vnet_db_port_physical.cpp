

#include "vnet_db_error.h"
#include "vnet_db_proc.h"
#include "vnet_db_mgr.h"

#include "vnet_db_port_physical.h"
    
namespace zte_tecs
{
typedef enum tagPortPhyProc
{
    EN_PORT_PHY_PROC_QUERYSUMMARY = 0,
    EN_PORT_PHY_PROC_QUERY,      
    EN_PORT_PHY_PROC_QUERY_SRIOVPORT_VSI_NUM,   
    EN_PORT_PHY_PROC_CHECK_SRIOVPORT_LOOP,
    EN_PORT_PHY_PROC_ADD,
    EN_PORT_PHY_PROC_MODIFY,
    EN_PORT_PHY_PROC_DEL,
    EN_PORT_PHY_PROC_ALL,
}EN_PORT_PHY_PROC;

const char *s_astrPortPhyProc[EN_PORT_PHY_PROC_ALL] = 
{        
    "pf_net_query_phyport_summary",
    "pf_net_query_phyport",    
    "pf_net_query_sriovport_vsi_num",  
    "pf_net_check_sriovport_loopback",      
    "pf_net_add_phyport",    
    "pf_net_modify_phyport",  
    "pf_net_del_phyport",  
};

    
CDBOperatePortPhysical::CDBOperatePortPhysical(CVNetDbIProcedure * pProc)
{
    m_pIProc = pProc;
}

CDBOperatePortPhysical::~CDBOperatePortPhysical()
{
    
}

int32 CDBOperatePortPhysical::DbProcCallback(int type, CADORecordset *prs, void* nil )
{
    switch(type)
    {
        case EN_PORT_PHY_PROC_QUERYSUMMARY:
        {
            return QuerySummaryCallback(prs,nil);
        }
        break;
        case EN_PORT_PHY_PROC_QUERY:
        {
            return QueryCallback(prs,nil);
        }
        break;
        case EN_PORT_PHY_PROC_QUERY_SRIOVPORT_VSI_NUM:
        {
            return QueryVsiNumCallback(prs,nil);
        }
        break;
        case EN_PORT_PHY_PROC_CHECK_SRIOVPORT_LOOP:
        {
            return CheckOperateCallback(prs,nil);
        }

        break;
        case EN_PORT_PHY_PROC_ADD:
        case EN_PORT_PHY_PROC_MODIFY:
        case EN_PORT_PHY_PROC_DEL:
        {
            return OperateCallback(prs,nil);
        }
        break;
        default:
        {
            VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortPhysical::DbProcCallback type(%d) is invalid.\n",type);
            return VNET_DB_ERROR_PARAM_INVALID;
        }
        break;
    }
    
    return 0;
}

int32 CDBOperatePortPhysical::QuerySummary(const char* vna_uuid, int32 is_srov,vector<CDbPortSummary> & outVInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortPhysical::QuerySummary IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == vna_uuid)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortPhysical::QuerySummary vna_uuid is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // call procedure 
    vector<CADOParameter>  vInParam;
    CADOParameter inVnaUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)vna_uuid);
    CADOParameter inIsSriov(CADOParameter::paramInt, CADOParameter::paramInput, is_srov);    
    vInParam.push_back(inVnaUuid);
    vInParam.push_back(inIsSriov);    
    
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,\
        s_astrPortPhyProc[EN_PORT_PHY_PROC_QUERYSUMMARY],\
        vInParam, \
        this,\
        EN_PORT_PHY_PROC_QUERYSUMMARY,\
        static_cast<void*> (&outVInfo));
        
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortPhysical::QuerySummary IProc->Handle failed(%d) \n",ret);
        return ret;
    }
        
    return 0;
}

int32 CDBOperatePortPhysical::QuerySummaryCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortPhysical::QuerySummaryCallback prs is NULL.\n");
        //VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }

    vector<CDbPortSummary> *pvOut;
    pvOut = static_cast<vector<CDbPortSummary>*>(nil);

    if( pvOut == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortPhysical::QuerySummaryCallback pvOut is NULL.\n");
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortPhysical::QuerySummaryCallback GetFieldValue failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetPortType(port_type);

    (*pvOut).push_back(info);

    return 0;
}


int32 CDBOperatePortPhysical::QueryAllSummary(const char* vna_uuid, vector<CDbPortSummary> & outVInfo)
{
    vector<CDbPortSummary> vTmp;

    int32 ret = QuerySummary(vna_uuid, 0, outVInfo );
    if( ret != VNET_DB_SUCCESS)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortPhysical::QueryAllSummary call QuerySummary(%s, 0) failed. ret = %d .\n",
            vna_uuid, ret);
        
        return ret;
    }

    ret = QuerySummary(vna_uuid, 1, vTmp );
    if( ret != VNET_DB_SUCCESS)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortPhysical::QueryAllSummary call QuerySummary(%s, 1) failed. ret = %d .\n",
            vna_uuid, ret);
        
        return ret;
    }

    outVInfo.insert(outVInfo.end(),vTmp.begin(),vTmp.end()); 
    
    return 0;
}
    

int32 CDBOperatePortPhysical::QueryVsiNum( const char* sriovport_uuid, int32 & outVsiNum )
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortPhysical::QueryVsiNum IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == sriovport_uuid )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortPhysical::QueryVsiNum sriovport_uuid is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // call procedure 
    CDbSriovPortVsiNum  Info;
    
    vector<CADOParameter> vInParam;
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)sriovport_uuid);
    vInParam.push_back(inUuid);
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrPortPhyProc[EN_PORT_PHY_PROC_QUERY_SRIOVPORT_VSI_NUM],
        vInParam, 
        this,
        EN_PORT_PHY_PROC_QUERY_SRIOVPORT_VSI_NUM,
        static_cast<void*> (&Info));
    if( ret != VNET_DB_SUCCESS )
    {
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortPhysical::QueryVsiNum(%s) IProc->Handle failed(%d).\n",
                sriovport_uuid,ret);
        }
        return ret;
    }
    
    if( Info.GetRet() != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortPhysical::QueryVsiNum(%s) proc return failed. ret : %d.\n", 
                sriovport_uuid,GET_DB_ERROR_PROC(Info.GetRet()));
        }
        return GET_DB_ERROR_PROC(Info.GetRet());
    }

    // 设置vsi num 值
    outVsiNum = Info.GetVsiNum();
    
    return 0;
}

int32 CDBOperatePortPhysical::CheckSetSriovLoop( const char* sriovport_uuid , int32 bIsloop  )
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortPhysical::CheckSetSriovLoop IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)sriovport_uuid);    
    CADOParameter inIsLoop(CADOParameter::paramInt, CADOParameter::paramInput, bIsloop);    
    
    vInParam.push_back(inUuid); 
    vInParam.push_back(inIsLoop);     

    // ret struct 
    int32 proc_ret = 0; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrPortPhyProc[EN_PORT_PHY_PROC_CHECK_SRIOVPORT_LOOP],
        vInParam, 
        this,
        EN_PORT_PHY_PROC_CHECK_SRIOVPORT_LOOP,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortPhysical::CheckSetSriovLoop(%s,%d) IProc->Handle failed(%d).\n",
            sriovport_uuid,bIsloop, ret);
        return ret;
    }

    //修改内容和原纪录内容相同 
    if( proc_ret == VNET_DB_PROCEDURE_RTN_HAS_SAME_ITEM )
    {
        return VNET_DB_SUCCESS;
    }

    if( proc_ret != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortPhysical::CheckSetSriovLoop(%s,%d) proc return failed. ret : %d.\n",
            sriovport_uuid,bIsloop, GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }
    
    return VNET_DB_SUCCESS;
}


int32 CDBOperatePortPhysical::QueryVsiNumCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortPhysical::QueryVsiNumCallback param is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    
    CDbSriovPortVsiNum *proc_ret = NULL;     
    proc_ret = static_cast<CDbSriovPortVsiNum *>(nil);

    if( proc_ret == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortPhysical::QueryVsiNumCallback proc_ret is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 2);    

    int32 value = 0;
    if( false == prs->GetFieldValue(0, value))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortPhysical::QueryVsiNumCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetRet(value);

    if( false == prs->GetFieldValue(1, value))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortPhysical::QueryVsiNumCallback GetFieldValue(1) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetVsiNum(value);

    return 0;
}

int32 CDBOperatePortPhysical::Query(CDbPortPhysical & Info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortPhysical::Query IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    vector<CADOParameter> vInParam;
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)Info.GetUuid().c_str());
    vInParam.push_back(inUuid);
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrPortPhyProc[EN_PORT_PHY_PROC_QUERY],
        vInParam, 
        this,
        EN_PORT_PHY_PROC_QUERY,
        static_cast<void*> (&Info));
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            Info.DbgShow();
        }
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortPhysical::Query IProc->Handle failed(%d).\n",ret);
        }
        return ret;
    }
    
    return 0;
}

int32 CDBOperatePortPhysical::QueryCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortPhysical::QueryCallback prs is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    if( nil == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortPhysical::QueryCallback nil is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }
    
    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == (DB_PORT_TABLE_FIELD_NUM + 27));    
 
    // 1 获取port 信息
    int32 proc_ret = 0;
    proc_ret = QueryPortBaseCallback(prs,nil);
    if( 0 != proc_ret )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortPhysical::QueryCallback QueryPortBaseCallback proc failed(%d).\n", (proc_ret));
        return (proc_ret);
    }

    // 2 获取physical信息
    CDbPortPhysical *pOutPort;
    pOutPort = static_cast<CDbPortPhysical *>(nil);
    if( pOutPort == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortPhysical::QueryCallback pOutPort is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // port 16
    (*pOutPort).SetSupportedPorts( prs->GetFieldValue(DB_PORT_TABLE_FIELD_NUM + 0));
    (*pOutPort).SetSupportedLinkModes( prs->GetFieldValue(DB_PORT_TABLE_FIELD_NUM + 1));
        
    int32 IsSupportAutoNeg = 0;
    if( false == prs->GetFieldValue(DB_PORT_TABLE_FIELD_NUM + 2,IsSupportAutoNeg) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortPhysical::QueryCallback GetFieldValue 2 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetIsSupportAutoNeg(IsSupportAutoNeg);
    
    (*pOutPort).SetAdvLinkModes( prs->GetFieldValue(DB_PORT_TABLE_FIELD_NUM + 3));
    (*pOutPort).SetAdvPauseFromeUse( prs->GetFieldValue(DB_PORT_TABLE_FIELD_NUM + 4));

    int32 IsAdvAutoNeg = 0;
    if( false == prs->GetFieldValue(DB_PORT_TABLE_FIELD_NUM + 5,IsAdvAutoNeg) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortPhysical::QueryCallback GetFieldValue 5 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetIsAdvAutoNeg(IsAdvAutoNeg);

    (*pOutPort).SetSpeed( prs->GetFieldValue(DB_PORT_TABLE_FIELD_NUM + 6));
    (*pOutPort).SetDuplex( prs->GetFieldValue(DB_PORT_TABLE_FIELD_NUM + 7));
    (*pOutPort).SetPort( prs->GetFieldValue(DB_PORT_TABLE_FIELD_NUM + 8));
    (*pOutPort).SetPhyad( prs->GetFieldValue(DB_PORT_TABLE_FIELD_NUM + 9));
    (*pOutPort).SetTransceiver( prs->GetFieldValue(DB_PORT_TABLE_FIELD_NUM + 10));
    (*pOutPort).SetAutoNeg( prs->GetFieldValue(DB_PORT_TABLE_FIELD_NUM + 11));
    (*pOutPort).SetSupportWakeOn( prs->GetFieldValue(DB_PORT_TABLE_FIELD_NUM + 12));
    (*pOutPort).SetWakeOn( prs->GetFieldValue(DB_PORT_TABLE_FIELD_NUM + 13));
    (*pOutPort).SetCntMsgLevel( prs->GetFieldValue(DB_PORT_TABLE_FIELD_NUM + 14));  
    
    int32 IsLinked = 0;
    if( false == prs->GetFieldValue(DB_PORT_TABLE_FIELD_NUM + 15,IsLinked) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortPhysical::QueryCallback GetFieldValue 15 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetIsLinked(IsLinked);

    // physical port extend 
    int32 value = 0;
    if( false == prs->GetFieldValue(DB_PORT_TABLE_FIELD_NUM + 16,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortPhysical::QueryCallback GetFieldValue 16 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetIsRxCheckSum(value);
    
    if( false == prs->GetFieldValue(DB_PORT_TABLE_FIELD_NUM + 17,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortPhysical::QueryCallback GetFieldValue 17 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetIsTxCheckSum(value);
    
    if( false == prs->GetFieldValue(DB_PORT_TABLE_FIELD_NUM + 18,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortPhysical::QueryCallback GetFieldValue 18 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetIsScatterGather(value);
    
    if( false == prs->GetFieldValue(DB_PORT_TABLE_FIELD_NUM + 19,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortPhysical::QueryCallback GetFieldValue 19 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetIsTcpSegOffload(value);

    if( false == prs->GetFieldValue(DB_PORT_TABLE_FIELD_NUM + 20,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortPhysical::QueryCallback GetFieldValue 20 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetIsUdpFragOffload(value);
    
    if( false == prs->GetFieldValue(DB_PORT_TABLE_FIELD_NUM + 21,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortPhysical::QueryCallback GetFieldValue 21 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetIsGenSegOffload(value);

    if( false == prs->GetFieldValue(DB_PORT_TABLE_FIELD_NUM + 22,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortPhysical::QueryCallback GetFieldValue 22 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetIsGenRecOffload(value);

    if( false == prs->GetFieldValue(DB_PORT_TABLE_FIELD_NUM + 23,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortPhysical::QueryCallback GetFieldValue 23 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetIsLargeRecOffload(value);

    // sriov port
    if( false == prs->GetFieldValue(DB_PORT_TABLE_FIELD_NUM + 24,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortPhysical::QueryCallback GetFieldValue 24 ailed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetIsSriov(value);

    if( false == prs->GetFieldValue(DB_PORT_TABLE_FIELD_NUM + 25,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortPhysical::QueryCallback GetFieldValue 25 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetIsloopRep(value);

    if( false == prs->GetFieldValue(DB_PORT_TABLE_FIELD_NUM + 26,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortPhysical::QueryCallback GetFieldValue 26 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutPort).SetIsloopCfg(value);
    

    return 0;
}

int32 CDBOperatePortPhysical::Add(CDbPortPhysical & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortPhysical::Add IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // uuid generate
    info.SetUuid(GenerateUUID().c_str());
    
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inVnaUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetVnaUuid().c_str());    
    CADOParameter inPortType(CADOParameter::paramInt, CADOParameter::paramInput, EN_DB_PORT_TYPE_PHYSICAL);    
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

    // physical port 
    CADOParameter inSupportedPorts(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetSupportedPorts().c_str());
    CADOParameter inSupportedLinkModes(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetSupportedLinkModes().c_str());
    CADOParameter inIsSupportAutoNeg(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsSupportAutoNeg());
    CADOParameter inAdvLinkModes(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetAdvLinkModes().c_str());
    CADOParameter inAdvPauseFromeUse(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetAdvPauseFromeUse().c_str());
    CADOParameter inIsAdvAutoNeg(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsAdvAutoNeg());
    CADOParameter inSpeed(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetSpeed().c_str());
    CADOParameter inDuplex(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetDuplex().c_str());
    CADOParameter inPort(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetPort().c_str());
    CADOParameter inPhyad(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetPhyad().c_str());
    CADOParameter inTransceiver(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetTransceiver().c_str());
    CADOParameter inAutoNeg(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetAutoNeg().c_str());
    CADOParameter inSupportWakeOn(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetSupportWakeOn().c_str());
    CADOParameter inWakeOn(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetWakeOn().c_str());
    CADOParameter inCntMsgLevel(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetCntMsgLevel().c_str());
    CADOParameter inIsLinked(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsLinked());
    
    CADOParameter inIsRxCheckSum(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsRxCheckSum());
    CADOParameter inIsTxCheckSum(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsTxCheckSum());
    CADOParameter inIsScatterGather(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsScatterGather());
    CADOParameter inIsTcpSegOffload(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsTcpSegOffload());
    CADOParameter inIsUdpFragOffload(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsUdpFragOffload());
    CADOParameter inIsGenSegOffload(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsGenSegOffload());
    CADOParameter inIsGenRecOffload(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsGenRecOffload());
    CADOParameter inIsLargeRecOffload(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsLargeRecOffload());
    
    CADOParameter inIsSriov(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsSriov());    
    CADOParameter inIsloopRep(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsloopRep());    
    CADOParameter inIsloopCfg(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsloopCfg());        
    
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
 
    vInParam.push_back(inSupportedPorts); 
    vInParam.push_back(inSupportedLinkModes); 
    vInParam.push_back(inIsSupportAutoNeg); 
    vInParam.push_back(inAdvLinkModes); 
    vInParam.push_back(inAdvPauseFromeUse); 
    vInParam.push_back(inIsAdvAutoNeg); 
    vInParam.push_back(inSpeed); 
    vInParam.push_back(inDuplex); 
    vInParam.push_back(inPort); 
    vInParam.push_back(inPhyad); 
    vInParam.push_back(inTransceiver); 
    vInParam.push_back(inAutoNeg); 
    vInParam.push_back(inSupportWakeOn); 
    vInParam.push_back(inWakeOn); 
    vInParam.push_back(inCntMsgLevel); 
    vInParam.push_back(inIsLinked); 

    vInParam.push_back(inIsRxCheckSum); 
    vInParam.push_back(inIsTxCheckSum); 
    vInParam.push_back(inIsScatterGather); 
    vInParam.push_back(inIsTcpSegOffload); 
    vInParam.push_back(inIsUdpFragOffload); 
    vInParam.push_back(inIsGenSegOffload); 
    vInParam.push_back(inIsGenRecOffload); 
    vInParam.push_back(inIsLargeRecOffload); 

    vInParam.push_back(inIsSriov); 
    vInParam.push_back(inIsloopRep); 
    vInParam.push_back(inIsloopCfg); 
  
    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrPortPhyProc[EN_PORT_PHY_PROC_ADD],
        vInParam, 
        this,
        EN_PORT_PHY_PROC_ADD,
        static_cast<void*> (&proc_ret));
    
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortPhysical::Add IProc->Handle failed(%d).\n", ret);
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortPhysical::Add proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }

    return VNET_DB_SUCCESS;
}


int32 CDBOperatePortPhysical::Modify(CDbPortPhysical & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortPhysical::Modify IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( EN_DB_PORT_TYPE_PHYSICAL != info.GetPortType() )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortPhysical::Modify type(%d) is invalid.\n",info.GetPortType());
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

    // physical port 
    CADOParameter inSupportedPorts(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetSupportedPorts().c_str());
    CADOParameter inSupportedLinkModes(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetSupportedLinkModes().c_str());
    CADOParameter inIsSupportAutoNeg(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsSupportAutoNeg());
    CADOParameter inAdvLinkModes(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetAdvLinkModes().c_str());
    CADOParameter inAdvPauseFromeUse(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetAdvPauseFromeUse().c_str());
    CADOParameter inIsAdvAutoNeg(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsAdvAutoNeg());
    CADOParameter inSpeed(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetSpeed().c_str());
    CADOParameter inDuplex(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetDuplex().c_str());
    CADOParameter inPort(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetPort().c_str());
    CADOParameter inPhyad(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetPhyad().c_str());
    CADOParameter inTransceiver(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetTransceiver().c_str());
    CADOParameter inAutoNeg(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetAutoNeg().c_str());
    CADOParameter inSupportWakeOn(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetSupportWakeOn().c_str());
    CADOParameter inWakeOn(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetWakeOn().c_str());
    CADOParameter inCntMsgLevel(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetCntMsgLevel().c_str());
    CADOParameter inIsLinked(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsLinked());
    
    CADOParameter inIsRxCheckSum(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsRxCheckSum());
    CADOParameter inIsTxCheckSum(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsTxCheckSum());
    CADOParameter inIsScatterGather(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsScatterGather());
    CADOParameter inIsTcpSegOffload(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsTcpSegOffload());
    CADOParameter inIsUdpFragOffload(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsUdpFragOffload());
    CADOParameter inIsGenSegOffload(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsGenSegOffload());
    CADOParameter inIsGenRecOffload(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsGenRecOffload());
    CADOParameter inIsLargeRecOffload(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsLargeRecOffload());
    
    CADOParameter inIsSriov(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsSriov());    
    CADOParameter inIsloopRep(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsloopRep());    
    CADOParameter inIsloopCfg(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIsloopCfg());        
    
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
 
    vInParam.push_back(inSupportedPorts); 
    vInParam.push_back(inSupportedLinkModes); 
    vInParam.push_back(inIsSupportAutoNeg); 
    vInParam.push_back(inAdvLinkModes); 
    vInParam.push_back(inAdvPauseFromeUse); 
    vInParam.push_back(inIsAdvAutoNeg); 
    vInParam.push_back(inSpeed); 
    vInParam.push_back(inDuplex); 
    vInParam.push_back(inPort); 
    vInParam.push_back(inPhyad); 
    vInParam.push_back(inTransceiver); 
    vInParam.push_back(inAutoNeg); 
    vInParam.push_back(inSupportWakeOn); 
    vInParam.push_back(inWakeOn); 
    vInParam.push_back(inCntMsgLevel); 
    vInParam.push_back(inIsLinked); 

    vInParam.push_back(inIsRxCheckSum); 
    vInParam.push_back(inIsTxCheckSum); 
    vInParam.push_back(inIsScatterGather); 
    vInParam.push_back(inIsTcpSegOffload); 
    vInParam.push_back(inIsUdpFragOffload); 
    vInParam.push_back(inIsGenSegOffload); 
    vInParam.push_back(inIsGenRecOffload); 
    vInParam.push_back(inIsLargeRecOffload); 

    vInParam.push_back(inIsSriov); 
    vInParam.push_back(inIsloopRep); 
    vInParam.push_back(inIsloopCfg); 

    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrPortPhyProc[EN_PORT_PHY_PROC_MODIFY],
        vInParam, 
        this,
        EN_PORT_PHY_PROC_MODIFY,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortPhysical::Modify IProc->Handle failed(%d).\n", ret);
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperatePortPhysical::Modify proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }
    
    return VNET_DB_SUCCESS;
}

int32 CDBOperatePortPhysical::Del(const char* Port_uuid)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortPhysical::Del IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == Port_uuid )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortPhysical::Del Port_uuid is NULL.\n");
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
        s_astrPortPhyProc[EN_PORT_PHY_PROC_DEL],
        vInParam, 
        this,
        EN_PORT_PHY_PROC_DEL,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortPhysical::Del(%s) IProc->Handle failed. ret : %d.\n",
            Port_uuid,ret);
        return ret;
    }
    
    if( proc_ret.GetRet() != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortPhysical::Del(%s) proc return failed. ret : %d.\n", 
            Port_uuid,GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }
    
    return 0;
}

int32 CDBOperatePortPhysical::OperateCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortPhysical::OperateCallback param is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    
    CDbProcOperateResult *proc_ret = NULL;     
    proc_ret = static_cast<CDbProcOperateResult *>(nil);

    if( proc_ret == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortPhysical::OperateCallback proc_ret is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 3);    

    int32 nRet = 0;
    if( false == prs->GetFieldValue(0, nRet))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortPhysical::OperateCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetRet(nRet);

    int64 oid = 0;
    if( false == prs->GetFieldValue(1, oid))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortPhysical::OperateCallback GetFieldValue(1) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetOid(oid);
    
    (*proc_ret).SetUuid(prs->GetFieldValue(2)); 

    return 0;
}


int32 CDBOperatePortPhysical::CheckOperateCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortPhysical::CheckOperateCallback param is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    
    int32 *proc_ret = NULL;     
    proc_ret = static_cast<int32 *>(nil);

    if( proc_ret == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortPhysical::CheckOperateCallback proc_ret is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 1);    

    int32 nRet = 0;
    if( false == prs->GetFieldValue(0, nRet))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperatePortPhysical::CheckOperateCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret) = nRet;

    return 0;
}

void CDBOperatePortPhysical::DbgShow()
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

/* test code ------------------------------------------------------------

*/

void db_query_phyportallsummary(const char* vna_uuid)
{
    if( NULL == vna_uuid)
    {
        cout << "vna uuid is NULL" << endl;
        return;
    }
    
    CDBOperatePortPhysical * pOper = GetDbIPortPhy();
    if( NULL == pOper )
    {
        cout << "CDBOperatePortPhysical is NULL" << endl;
        return ;
    }

    vector<CDbPortSummary> outVInfo;
    int ret = pOper->QueryAllSummary(vna_uuid,outVInfo);
    if( ret != 0 )
    {
        cout << "QueryAllSummary failed." << ret << endl;
        return;
    }
    cout << "QueryAllSummary success." << endl;
    vector<CDbPortSummary>::iterator it = outVInfo.begin();
    for(; it != outVInfo.end(); ++it)
    {
        (*it).DbgShow();
    }    
}
DEFINE_DEBUG_FUNC(db_query_phyportallsummary);

void db_query_phyportsummary(const char* vna_uuid, int32 is_sriov)
{
    if( NULL == vna_uuid)
    {
        cout << "vna uuid is NULL" << endl;
        return;
    }
    
    CDBOperatePortPhysical * pOper = GetDbIPortPhy();
    if( NULL == pOper )
    {
        cout << "CDBOperatePortPhysical is NULL" << endl;
        return ;
    }

    vector<CDbPortSummary> outVInfo;
    int ret = pOper->QuerySummary(vna_uuid,is_sriov,outVInfo);
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
DEFINE_DEBUG_FUNC(db_query_phyportsummary);

void db_query_phyportuuid(const char* vna_uuid, const char* name)
{
    if( NULL == vna_uuid || NULL == name)
    {
        cout << "vna uuid is NULL" << endl;
        return;
    }
    
    CDBOperatePortPhysical * pOper = GetDbIPortPhy();
    if( NULL == pOper )
    {
        cout << "CDBOperatePortPhysical is NULL" << endl;
        return ;
    }

    string uuid("");
    int ret = pOper->QueryUuid(vna_uuid,name, EN_DB_PORT_TYPE_PHYSICAL, uuid);
    if( ret != 0 )
    {
        cout << "QueryUuid failed." << ret << endl;
        return;
    }
    cout << "QuerySummary success." << endl;
    cout << "uuid: " << uuid << endl;
    
}
DEFINE_DEBUG_FUNC(db_query_phyportuuid);


void db_check_set_sriovport_loop(const char* uuid)
{
    if( NULL == uuid)
    {
        cout << "uuid is NULL" << endl;
        return;
    }
    
    CDBOperatePortPhysical * pOper = GetDbIPortPhy();
    if( NULL == pOper )
    {
        cout << "CDBOperatePortPhysical is NULL" << endl;
        return ;
    }

    int ret = pOper->CheckSetSriovLoop(uuid);
    if( ret != 0 )
    {
        cout << "CheckSetSriovLoop failed." << ret << endl;
        return;
    }
    cout << "CheckSetSriovLoop success." << endl;    
}
DEFINE_DEBUG_FUNC(db_check_set_sriovport_loop);

void db_query_phyport(char * uuid)
{
    if( NULL == uuid)
    {
        cout << "Param uuid is NULL" << endl;
        return;
    }
    CDBOperatePortPhysical * pOper = GetDbIPortPhy();
    if( NULL == pOper )
    {
        cout << "CDBOperatePortPhysical is NULL" << endl;
        return ;
    }

    CDbPortPhysical info;
    info.SetUuid(uuid);  
    int ret = pOper->Query(info);
    if( ret != 0 )
    {
        cout << "CDBOperatePortPhysical query failed." << ret << endl;
        return;
    }
    cout << "CDBOperatePortPhysical query success." << endl;
    info.DbgShow();
    
}
DEFINE_DEBUG_FUNC(db_query_phyport);


void db_query_sriovport_vsi_num(const char* sriov_uuid)
{
    if( NULL == sriov_uuid)
    {
        cout << "vna uuid is NULL" << endl;
        return;
    }
    
    CDBOperatePortPhysical * pOper = GetDbIPortPhy();
    if( NULL == pOper )
    {
        cout << "CDBOperatePortPhysical is NULL" << endl;
        return ;
    }

    int32 vsinum = 0;    
    int ret = pOper->QueryVsiNum(sriov_uuid,vsinum);
    if( ret != 0 )
    {
        cout << "QueryVsiNum failed." << ret << endl;
        return;
    }
    cout << "QueryVsiNum success." << endl;
    cout << "uuid: " << sriov_uuid << ", vsinum : " << vsinum << endl;
    
}
DEFINE_DEBUG_FUNC(db_query_sriovport_vsi_num);

void db_add_phyport(char* vna_uuid,int32 port_type, char* name, int32 is_online,
        char * spt_ports, int32 is_linked,
        int32 is_sriov, /*int32 is_loop_cfg,*/ int mtu)
{
    if( NULL == vna_uuid || NULL == name )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperatePortPhysical * pOper = GetDbIPortPhy();
    if( NULL == pOper )
    {
        cout << "CDBOperatePortPhysical is NULL" << endl;
        return ;
    }

    CDbPortPhysical info;
    info.SetVnaUuid(vna_uuid);
    info.SetPortType(port_type);
    info.SetName(name);
    info.SetIsOnline(is_online);

    info.SetSupportedPorts(spt_ports);
    info.SetIsLinked(is_linked);

    info.SetIsSriov(is_sriov);
    //info.SetIsloopCfg(is_loop_cfg);
    info.SetMtu(mtu);
        
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
DEFINE_DEBUG_FUNC(db_add_phyport);

void db_mod_phyport(char* port_uuid, char* vna_uuid, char* name, int32 is_online,
        char * spt_ports, int32 is_linked,
        int32 is_sriov, int32 is_loop_cfg)
{
    if( NULL == port_uuid || NULL == vna_uuid || NULL == name )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperatePortPhysical * pOper = GetDbIPortPhy();
    if( NULL == pOper )
    {
        cout << "CDBOperatePortPhysical is NULL" << endl;
        return ;
    }

    CDbPortPhysical info;
    info.SetUuid(port_uuid);
    info.SetPortType(EN_DB_PORT_TYPE_PHYSICAL);
    info.SetVnaUuid(vna_uuid);
    info.SetName(name);
    info.SetIsOnline(is_online);

    info.SetSupportedPorts(spt_ports);
    info.SetIsLinked(is_linked);

    info.SetIsSriov(is_sriov);
    info.SetIsloopCfg(is_loop_cfg);
        
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
DEFINE_DEBUG_FUNC(db_mod_phyport);

void db_del_phyport(char * uuid)
{
    if(uuid == NULL )
    {
        cout << "Param is NULL" << endl;
        return;
    }

    CDBOperatePortPhysical * pOper = GetDbIPortPhy();
    if( NULL == pOper )
    {
        cout << "CDBOperatePortPhysical is NULL" << endl;
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
DEFINE_DEBUG_FUNC(db_del_phyport);


}

