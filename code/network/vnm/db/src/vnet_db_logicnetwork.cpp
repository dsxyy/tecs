

#include "vnet_db_error.h"
#include "vnet_db_proc.h"
#include "vnet_db_mgr.h"
#include "vnet_db_logicnetwork.h"
    
namespace zte_tecs
{
typedef enum tagLogicNetworkProc
{
    EN_LOGICNETWORK_PROC_CHECK_ADD = 0,
    EN_LOGICNETWORK_PROC_ADD,
    EN_LOGICNETWORK_PROC_CHECK_MODIFY,
    EN_LOGICNETWORK_PROC_MODIFY,
    EN_LOGICNETWORK_PROC_CHECK_DEL,
    EN_LOGICNETWORK_PROC_DEL,
    EN_LOGICNETWORK_PROC_QUERY,
    EN_LOGICNETWORK_PROC_QUERY_BY_NAME,
    EN_LOGICNETWORK_PROC_QUERYS_BY_NETPLANE,
    EN_LOGICNETWORK_PROC_QUERYS_BY_PORTGROUP,    
    EN_LOGICNETWORK_PROC_ALL,
}EN_VSWITCH_PROC;

const char *s_astrLogicNetworkProc[EN_LOGICNETWORK_PROC_ALL] = 
{        
    "pf_net_check_add_logicnetwork",  
    "pf_net_add_logicnetwork",    
    "pf_net_check_modify_logicnetwork", 
    "pf_net_modify_logicnetwork", 
    "pf_net_check_del_logicnetwork",  
    "pf_net_del_logicnetwork",  
    
    "pf_net_query_logicnetwork",
    "pf_net_query_logicnetwork_by_name",
    "pf_net_query_logicnetwork_by_netplane",
    "pf_net_query_logicnetwork_by_portgroup",
};

    
CDBOperateLogicNetwork::CDBOperateLogicNetwork(CVNetDbIProcedure * pProc)
{
    m_pIProc = pProc;
}

CDBOperateLogicNetwork::~CDBOperateLogicNetwork()
{
    
}

int32 CDBOperateLogicNetwork::DbProcCallback(int type, CADORecordset *prs, void* nil )
{
    switch(type)
    {
        case EN_LOGICNETWORK_PROC_QUERYS_BY_NETPLANE:
        case EN_LOGICNETWORK_PROC_QUERYS_BY_PORTGROUP:
        {
            return QueryByCallback(prs,nil);
        }
        break;
        case EN_LOGICNETWORK_PROC_QUERY:
        case EN_LOGICNETWORK_PROC_QUERY_BY_NAME:
        {
            return QueryCallback(prs,nil);
        }
        break;
        case EN_LOGICNETWORK_PROC_ADD:
        case EN_LOGICNETWORK_PROC_MODIFY:
        case EN_LOGICNETWORK_PROC_DEL:
        {
            return OperateCallback(prs,nil);
        }
        break;
        
        case EN_LOGICNETWORK_PROC_CHECK_ADD:
        case EN_LOGICNETWORK_PROC_CHECK_MODIFY:
        case EN_LOGICNETWORK_PROC_CHECK_DEL:
        {
            return CheckOperateCallback(prs,nil);
        }   
        break;
        default:
        {
            VNET_LOG(VNET_LOG_ERROR, "CDBOperateLogicNetwork::DbProcCallback type(%d) is invalid.\n",type);
            return VNET_DB_ERROR_PARAM_INVALID;
        }
        break;
    }
    
    return 0;
}
    
int32 CDBOperateLogicNetwork::QueryByNetplane(const char* uuid,vector<CDbLogicNetworkPgNetplane> & outVInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateLogicNetwork::QueryByNetplane IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }


    // call procedure 
    vector<CADOParameter>  vInParam;
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)uuid);
    vInParam.push_back(inUuid);
    
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,\
        s_astrLogicNetworkProc[EN_LOGICNETWORK_PROC_QUERYS_BY_NETPLANE],\
        vInParam, \
        this,\
        EN_LOGICNETWORK_PROC_QUERYS_BY_NETPLANE,\
        static_cast<void*> (&outVInfo));
        
    if( ret != VNET_DB_SUCCESS )
    {
        
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR,"CDBOperateLogicNetwork::QueryByNetplane(%s) IProc->Handle failed(%d) \n",
                uuid,ret);
        }
        return ret;
    }
        
    return 0;
}

int32 CDBOperateLogicNetwork::QueryByPortGroup(const char* uuid,vector<CDbLogicNetworkPgNetplane> & outVInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateLogicNetwork::QueryByPortGroup IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    vector<CADOParameter>  vInParam;
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)uuid);
    vInParam.push_back(inUuid);
    
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,\
        s_astrLogicNetworkProc[EN_LOGICNETWORK_PROC_QUERYS_BY_PORTGROUP],\
        vInParam, \
        this,\
        EN_LOGICNETWORK_PROC_QUERYS_BY_PORTGROUP,\
        static_cast<void*> (&outVInfo));
        
    if( ret != VNET_DB_SUCCESS )
    {
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR,"CDBOperateLogicNetwork::QueryByPortGroup(%s) IProc->Handle failed(%d) \n",
                uuid, ret);
        }
        return ret;
    }
        
    return 0;
}

int32 CDBOperateLogicNetwork::QueryByCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateLogicNetwork::QueryPortSummaryCallback prs is NULL.\n");
        //VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }

    vector<CDbLogicNetworkPgNetplane> *pvOut;
    pvOut = static_cast<vector<CDbLogicNetworkPgNetplane>*>(nil);

    if( pvOut == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateLogicNetwork::QueryPortSummaryCallback pvOut is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    CDbLogicNetworkPgNetplane info;

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 9);
       
    int32 value = 0;
    
    info.SetNetplaneUuid(prs->GetFieldValue(0));
    info.SetNetplaneName(prs->GetFieldValue(1));
    if( false == prs->GetFieldValue(2,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateLogicNetwork::QueryPortSummaryCallback GetFieldValue 2 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetNetplaneMtu(value);

    info.SetPgUuid(prs->GetFieldValue(3));
    info.SetPgName(prs->GetFieldValue(4));
    if( false == prs->GetFieldValue(5,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateLogicNetwork::QueryPortSummaryCallback GetFieldValue 5 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetPgMtu(value);

    info.SetLogicnetworkUuid(prs->GetFieldValue(6));
    info.SetLogicnetworkName(prs->GetFieldValue(7));

    if( false == prs->GetFieldValue(8,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateLogicNetwork::QueryPortSummaryCallback GetFieldValue 5 failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetLogicnetworkIpMode(value);

    (*pvOut).push_back(info);

    return 0;
}

int32 CDBOperateLogicNetwork::QueryByName(CDbLogicNetwork & Info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateLogicNetwork::QueryByName IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    vector<CADOParameter> vInParam;
    CADOParameter inName(CADOParameter::paramText, CADOParameter::paramInput, (char *)Info.GetName().c_str());
    vInParam.push_back(inName);
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrLogicNetworkProc[EN_LOGICNETWORK_PROC_QUERY_BY_NAME],
        vInParam, 
        this,
        EN_LOGICNETWORK_PROC_QUERY_BY_NAME,
        static_cast<void*> (&Info));
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            Info.DbgShow();
        }
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {        
            VNET_LOG(VNET_LOG_ERROR,"CDBOperateLogicNetwork::QueryByName IProc->Handle failed(%d).\n",ret);
        }
        return ret;
    }
    
    return 0;
}


int32 CDBOperateLogicNetwork::Query(CDbLogicNetwork & Info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateLogicNetwork::Query IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    vector<CADOParameter> vInParam;
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)Info.GetUuid().c_str());
    vInParam.push_back(inUuid);
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrLogicNetworkProc[EN_LOGICNETWORK_PROC_QUERY],
        vInParam, 
        this,
        EN_LOGICNETWORK_PROC_QUERY,
        static_cast<void*> (&Info));
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            Info.DbgShow();
        }
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {        
            VNET_LOG(VNET_LOG_ERROR,"CDBOperateLogicNetwork::Query IProc->Handle failed(%d).\n",ret);
        }
        return ret;
    }
    
    return 0;
}

int32 CDBOperateLogicNetwork::QueryCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateLogicNetwork::QueryCallback prs is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    if( nil == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateLogicNetwork::QueryCallback nil is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    CDbLogicNetwork *pOutPort;
    pOutPort = static_cast<CDbLogicNetwork *>(nil);

    if( pOutPort == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateSwitchBase::QueryCallback pOutPort is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    
    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == (6));    

    int32 proc_ret = 0;
    if( false == prs->GetFieldValue(0,proc_ret ))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateLogicNetwork::QueryCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }    

    if( 0 != proc_ret )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateLogicNetwork::QueryCallback proc failed(%d).\n", GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }

    (*pOutPort).SetUuid( prs->GetFieldValue(1));
    (*pOutPort).SetName( prs->GetFieldValue(2));
    (*pOutPort).SetPgUuid( prs->GetFieldValue(3));
    (*pOutPort).SetDes( prs->GetFieldValue(4));

    int32 value = 0;
    if( false == prs->GetFieldValue(5,value ))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateLogicNetwork::QueryCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    } 
    (*pOutPort).SetIpMode( value);
    
    return 0;
}

int32 CDBOperateLogicNetwork::CheckAdd(CDbLogicNetwork & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateLogicNetwork::CheckAdd IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    
    if (EN_DB_IP_MODE_STATIC != info.GetIpMode() && 
        EN_DB_IP_MODE_DYNAMIC != info.GetIpMode() && 
        EN_DB_IP_MODE_NONE != info.GetIpMode() )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateLogicNetwork::CheckAdd ip mode(%d) invalid.\n",info.GetIpMode());
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // uuid generate
    info.SetUuid(GenerateUUID().c_str());
    
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inName(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetName().c_str());
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUuid().c_str());    
    CADOParameter inPgUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetPgUuid().c_str());    
    CADOParameter inDes(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetDes().c_str());    
    CADOParameter inIpMode(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIpMode());    
    
    vInParam.push_back(inName);
    vInParam.push_back(inUuid);     
    vInParam.push_back(inPgUuid);    
    vInParam.push_back(inDes);    
    vInParam.push_back(inIpMode);    
  
    // ret struct 
    int32 proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrLogicNetworkProc[EN_LOGICNETWORK_PROC_CHECK_ADD],
        vInParam, 
        this,
        EN_LOGICNETWORK_PROC_CHECK_ADD,
        static_cast<void*> (&proc_ret));
    
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateLogicNetwork::CheckAdd IProc->Handle failed(%d).\n", ret);
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateLogicNetwork::CheckAdd proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }

    return VNET_DB_SUCCESS;
}

int32 CDBOperateLogicNetwork::Add(CDbLogicNetwork & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateLogicNetwork::Add IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }
    
    if (EN_DB_IP_MODE_STATIC != info.GetIpMode() && 
        EN_DB_IP_MODE_DYNAMIC != info.GetIpMode() && 
        EN_DB_IP_MODE_NONE != info.GetIpMode() )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateLogicNetwork::Add ip mode(%d) invalid.\n",info.GetIpMode());
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // uuid generate
    info.SetUuid(GenerateUUID().c_str());
    
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inName(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetName().c_str());
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUuid().c_str());    
    CADOParameter inPgUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetPgUuid().c_str());    
    CADOParameter inDes(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetDes().c_str());    
    CADOParameter inIpMode(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIpMode());    
    
    vInParam.push_back(inName);
    vInParam.push_back(inUuid);     
    vInParam.push_back(inPgUuid);    
    vInParam.push_back(inDes);    
    vInParam.push_back(inIpMode);
  
    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrLogicNetworkProc[EN_LOGICNETWORK_PROC_ADD],
        vInParam, 
        this,
        EN_LOGICNETWORK_PROC_ADD,
        static_cast<void*> (&proc_ret));
    
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateLogicNetwork::Add IProc->Handle failed(%d).\n", ret);
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateLogicNetwork::Add proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }

    return VNET_DB_SUCCESS;
}

int32 CDBOperateLogicNetwork::CheckModify(CDbLogicNetwork & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateLogicNetwork::CheckModify IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if (EN_DB_IP_MODE_STATIC != info.GetIpMode() && 
        EN_DB_IP_MODE_DYNAMIC != info.GetIpMode()&& 
        EN_DB_IP_MODE_NONE != info.GetIpMode()  )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateLogicNetwork::CheckModify ip mode(%d) invalid.\n",info.GetIpMode());
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inName(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetName().c_str());
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUuid().c_str());    
    CADOParameter inPgUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetPgUuid().c_str());    
    CADOParameter inDes(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetDes().c_str());    
    CADOParameter inIpMode(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIpMode());    
    
    vInParam.push_back(inName);
    vInParam.push_back(inUuid);     
    vInParam.push_back(inPgUuid);    
    vInParam.push_back(inDes);    
    vInParam.push_back(inIpMode); 

    // ret struct 
    int32 proc_ret = 0; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrLogicNetworkProc[EN_LOGICNETWORK_PROC_CHECK_MODIFY],
        vInParam, 
        this,
        EN_LOGICNETWORK_PROC_CHECK_MODIFY,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateLogicNetwork::CheckModify IProc->Handle failed(%d).\n", ret);
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateLogicNetwork::CheckModify proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }
    
    return VNET_DB_SUCCESS;
}

int32 CDBOperateLogicNetwork::Modify(CDbLogicNetwork & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateLogicNetwork::Modify IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if (EN_DB_IP_MODE_STATIC != info.GetIpMode() && 
        EN_DB_IP_MODE_DYNAMIC != info.GetIpMode()&& 
        EN_DB_IP_MODE_NONE != info.GetIpMode()  )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateLogicNetwork::Modify ip mode(%d) invalid.\n",info.GetIpMode());
        return VNET_DB_ERROR_PARAM_INVALID;
    }
    
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inName(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetName().c_str());
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUuid().c_str());    
    CADOParameter inPgUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetPgUuid().c_str());    
    CADOParameter inDes(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetDes().c_str());    
    CADOParameter inIpMode(CADOParameter::paramInt, CADOParameter::paramInput, info.GetIpMode());    
    
    vInParam.push_back(inName);
    vInParam.push_back(inUuid);     
    vInParam.push_back(inPgUuid);    
    vInParam.push_back(inDes);    
    vInParam.push_back(inIpMode); 


    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrLogicNetworkProc[EN_LOGICNETWORK_PROC_MODIFY],
        vInParam, 
        this,
        EN_LOGICNETWORK_PROC_MODIFY,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateLogicNetwork::Modify IProc->Handle failed(%d).\n", ret);
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateLogicNetwork::Modify proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }
    
    return VNET_DB_SUCCESS;
}


int32 CDBOperateLogicNetwork::CheckDel(const char* uuid)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateLogicNetwork::CheckDel IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == uuid )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateLogicNetwork::CheckDel uuid is NULL.\n");
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
        s_astrLogicNetworkProc[EN_LOGICNETWORK_PROC_CHECK_DEL],
        vInParam, 
        this,
        EN_LOGICNETWORK_PROC_CHECK_DEL,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateLogicNetwork::CheckDel(%s) IProc->Handle failed. ret : %d.\n",
            uuid,ret);
        return ret;
    }
    
    if( proc_ret != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateLogicNetwork::CheckDel(%s) proc return failed. ret : %d.\n", 
            uuid,GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }
    
    return 0;
}


int32 CDBOperateLogicNetwork::Del(const char* uuid)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateLogicNetwork::Del IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == uuid )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateLogicNetwork::Del uuid is NULL.\n");
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
        s_astrLogicNetworkProc[EN_LOGICNETWORK_PROC_DEL],
        vInParam, 
        this,
        EN_LOGICNETWORK_PROC_DEL,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateLogicNetwork::Del(%s) IProc->Handle failed. ret : %d.\n",
            uuid,ret);
        return ret;
    }
    
    if( proc_ret.GetRet() != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateLogicNetwork::Del(%s) proc return failed. ret : %d.\n",
            uuid,GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }
    
    return 0;
}

int32 CDBOperateLogicNetwork::OperateCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateLogicNetwork::OperateCallback param is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    
    CDbProcOperateResult *proc_ret = NULL;     
    proc_ret = static_cast<CDbProcOperateResult *>(nil);

    if( proc_ret == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateLogicNetwork::OperateCallback proc_ret is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 3);    

    int32 nRet = 0;
    if( false == prs->GetFieldValue(0, nRet))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateLogicNetwork::OperateCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetRet(nRet);

    int64 oid = 0;
    if( false == prs->GetFieldValue(1, oid))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateLogicNetwork::OperateCallback GetFieldValue(1) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetOid(oid);
    
    (*proc_ret).SetUuid(prs->GetFieldValue(2)); 

    return 0;
}

int32 CDBOperateLogicNetwork::CheckOperateCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateLogicNetwork::CheckOperateCallback param is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    
    int32 *proc_ret = NULL;     
    proc_ret = static_cast<int32 *>(nil);

    if( proc_ret == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateLogicNetwork::CheckOperateCallback proc_ret is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 1);    

    int32 nRet = 0;
    if( false == prs->GetFieldValue(0, nRet))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateLogicNetwork::CanOperateCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret) = nRet;

    return 0;
}


void CDBOperateLogicNetwork::DbgShow()
{
#if 0
    cout << "--------------------------Port Info------------------------" << endl;
    // query summary
    vector<CDbLogicNetworkPgNetplane> vInfo;
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
    vector<CDbLogicNetworkPgNetplane>::iterator it = vInfo.begin();
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

void db_query_ln_by_np(const char* uuid)
{
    if( NULL == uuid )
    {
        cout << "uuid is NULL" << endl;
        return;
    }
    
    CDBOperateLogicNetwork * pOper = GetDbILogicNetwork();
    if( NULL == pOper )
    {
        cout << "CDBOperateLogicNetwork is NULL" << endl;
        return ;
    }

    vector<CDbLogicNetworkPgNetplane> outVInfo;
    int ret = pOper->QueryByNetplane(uuid,outVInfo);
    if( ret != 0 )
    {
        cout << "Query failed." << ret << endl;
        return;
    }
    cout << "Query success." << endl;
    vector<CDbLogicNetworkPgNetplane>::iterator it = outVInfo.begin();
    for(; it != outVInfo.end(); ++it)
    {
        cout << "------------" << endl;
        (*it).DbgShow();
    }    
}
DEFINE_DEBUG_FUNC(db_query_ln_by_np);

void db_query_ln_by_pg(const char* uuid)
{    
    if( NULL == uuid )
    {
        cout << "uuid is NULL" << endl;
        return;
    }
    
    CDBOperateLogicNetwork * pOper = GetDbILogicNetwork();
    if( NULL == pOper )
    {
        cout << "CDBOperateLogicNetwork is NULL" << endl;
        return ;
    }

    vector<CDbLogicNetworkPgNetplane> outVInfo;
    int ret = pOper->QueryByPortGroup(uuid,outVInfo);
    if( ret != 0 )
    {
        cout << "Query failed." << ret << endl;
        return;
    }
    cout << "Query success." << endl;
    vector<CDbLogicNetworkPgNetplane>::iterator it = outVInfo.begin();
    for(; it != outVInfo.end(); ++it)
    {
        cout << "------------" << endl;
        (*it).DbgShow();
    }    
}
DEFINE_DEBUG_FUNC(db_query_ln_by_pg);

void db_query_logicnetwork(char * uuid)
{
    if( NULL == uuid)
    {
        cout << "Param uuid is NULL" << endl;
        return;
    }
    CDBOperateLogicNetwork * pOper = GetDbILogicNetwork();
    if( NULL == pOper )
    {
        cout << "CDBOperateLogicNetwork is NULL" << endl;
        return ;
    }

    CDbLogicNetwork info;
    info.SetUuid(uuid);  
    int ret = pOper->Query(info);
    if( ret != 0 )
    {
        cout << "CDBOperateLogicNetwork query failed." << ret << endl;
        return;
    }
    cout << "CDBOperateLogicNetwork query success." << endl;
    info.DbgShow();
    
}
DEFINE_DEBUG_FUNC(db_query_logicnetwork);


void db_check_add_ln(char* name, char* uuid,char* pg_uuid,char* des ,int32 ipmode)
{
    if( NULL == uuid || NULL == name || NULL == pg_uuid || NULL == des )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateLogicNetwork * pOper = GetDbILogicNetwork();
    if( NULL == pOper )
    {
        cout << "CDBOperateLogicNetwork is NULL" << endl;
        return ;
    }

    CDbLogicNetwork info;
    info.SetName(name);
    info.SetUuid(uuid);
    info.SetPgUuid(pg_uuid);
    info.SetDes(des);
    info.SetIpMode(ipmode);

    //info.DbgShow();
    int ret = pOper->CheckAdd(info);
    if( ret != 0 )
    {
        cout << "can Add failed." << ret << endl;
        return;
    }
    cout << "can Add success." << endl;
    info.DbgShow();
}
DEFINE_DEBUG_FUNC(db_check_add_ln);

void db_add_ln(char* name, char* uuid,char* pg_uuid,char* des,int32 ipmode )
{
    if( NULL == uuid || NULL == name || NULL == pg_uuid || NULL == des )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateLogicNetwork * pOper = GetDbILogicNetwork();
    if( NULL == pOper )
    {
        cout << "CDBOperateLogicNetwork is NULL" << endl;
        return ;
    }

    CDbLogicNetwork info;
    info.SetName(name);
    info.SetUuid(uuid);
    info.SetPgUuid(pg_uuid);
    info.SetDes(des);
    info.SetIpMode(ipmode);

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
DEFINE_DEBUG_FUNC(db_add_ln);

void db_check_mod_ln(char* name, char* uuid,char* pg_uuid,char* des,int32 ipmode )
{
    if( NULL == uuid || NULL == name || NULL == pg_uuid )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateLogicNetwork * pOper = GetDbILogicNetwork();
    if( NULL == pOper )
    {
        cout << "CDBOperateLogicNetwork is NULL" << endl;
        return ;
    }

    CDbLogicNetwork info;
    info.SetName(name);
    info.SetUuid(uuid);
    info.SetPgUuid(pg_uuid);
    info.SetDes(des);
    info.SetIpMode(ipmode);
        
    //info.DbgShow();
    int ret = pOper->CheckModify(info);
    if( ret != 0 )
    {
        cout << "check Modify failed." << ret << endl;
        return;
    }
    cout << "check Modify success." << endl;
    info.DbgShow();
}
DEFINE_DEBUG_FUNC(db_check_mod_ln);

void db_mod_ln(char* name, char* uuid,char* pg_uuid,char* des,int32 ipmode )
{
    if( NULL == uuid || NULL == name || NULL == pg_uuid )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateLogicNetwork * pOper = GetDbILogicNetwork();
    if( NULL == pOper )
    {
        cout << "CDBOperateLogicNetwork is NULL" << endl;
        return ;
    }

    CDbLogicNetwork info;
    info.SetName(name);
    info.SetUuid(uuid);
    info.SetPgUuid(pg_uuid);
    info.SetDes(des);
    info.SetIpMode(ipmode);
        
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
DEFINE_DEBUG_FUNC(db_mod_ln);

void db_check_del_ln(char * uuid)
{
    if(uuid == NULL )
    {
        cout << "Param is NULL" << endl;
        return;
    }

    CDBOperateLogicNetwork * pOper = GetDbILogicNetwork();
    if( NULL == pOper )
    {
        cout << "CDBOperateLogicNetwork is NULL" << endl;
        return ;
    }

    int ret = pOper->CheckDel(uuid);
    if( ret != 0 )
    {
        cout << "check Del failed." << ret << endl;
        return;
    }
    cout << "check Del success." << endl;
}
DEFINE_DEBUG_FUNC(db_check_del_ln);

void db_del_ln(char * uuid)
{
    if(uuid == NULL )
    {
        cout << "Param is NULL" << endl;
        return;
    }

    CDBOperateLogicNetwork * pOper = GetDbILogicNetwork();
    if( NULL == pOper )
    {
        cout << "CDBOperateLogicNetwork is NULL" << endl;
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
DEFINE_DEBUG_FUNC(db_del_ln);


}




