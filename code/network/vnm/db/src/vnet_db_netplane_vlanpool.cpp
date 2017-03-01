

#include "vnet_db_error.h"
#include "vnet_db_proc.h"
#include "vnet_db_mgr.h"
#include "vnet_db_netplane_vlanrange.h"
#include "vnet_db_netplane_vlanpool.h"
    
namespace zte_tecs
{
typedef enum tagNetplaneVlanPoolPoolProc
{
    EN_NETPLANE_VLANPOOL_PROC_CHECK_ADD = 0,
    EN_NETPLANE_VLANPOOL_PROC_ADD,
    EN_NETPLANE_VLANPOOL_PROC_CHECK_DEL,
    EN_NETPLANE_VLANPOOL_PROC_DEL,
    EN_NETPLANE_VLANPOOL_PROC_QUERY,
    EN_NETPLANE_VLANPOOL_PROC_QUERY_SHARE_NET_ACCESS_VLAN,
    EN_NETPLANE_VLANPOOL_PROC_QUERY_SHARE_NET_TRUNK_VLAN,
    EN_NETPLANE_VLANPOOL_PROC_QUERY_PRIV_NET_ISOLATE_NO,
    EN_NETPLANE_VLANPOOL_PROC_ALL,
}EN_NETPLANE_VLANPOOL_PROC;

const char *s_astrNetplaneVlanPoolProc[EN_NETPLANE_VLANPOOL_PROC_ALL] = 
{        
    "pf_net_check_add_netplane_vlanpool",  
    "pf_net_add_netplane_vlanpool",  
    "pf_net_check_del_netplane_vlanpool",
    "pf_net_del_netplane_vlanpool", 
    "pf_net_query_netplane_vlanpool", 
    "pf_net_query_vm_share_pg_access_list_by_netplane", 
    "pf_net_query_vm_share_pg_Trunk_list_by_netplane",
    "pf_net_query_vm_private_pg_isolate_no_list_by_netplane",
};

    
CDBOperateNetplaneVlanPool::CDBOperateNetplaneVlanPool(CVNetDbIProcedure * pProc)
{
    m_pIProc = pProc;
}

CDBOperateNetplaneVlanPool::~CDBOperateNetplaneVlanPool()
{
    
}

int32 CDBOperateNetplaneVlanPool::DbProcCallback(int type, CADORecordset *prs, void* nil )
{
    switch(type)
    {
        
        case EN_NETPLANE_VLANPOOL_PROC_QUERY:
        {
            return QueryCallback(prs,nil);
        }
        break;
        case EN_NETPLANE_VLANPOOL_PROC_QUERY_SHARE_NET_ACCESS_VLAN:
        {
            return QueryShareNetAccessVlanCallback(prs,nil);
        }
        break;
        case EN_NETPLANE_VLANPOOL_PROC_QUERY_SHARE_NET_TRUNK_VLAN:
        {
            return QueryShareNetTrunkVlanCallback(prs,nil);
        }
        break; 
        case EN_NETPLANE_VLANPOOL_PROC_QUERY_PRIV_NET_ISOLATE_NO:
        {
            return QueryPrivateNetIsolateNoCallback(prs,nil);
        }
        break;
        case EN_NETPLANE_VLANPOOL_PROC_ADD:
        case EN_NETPLANE_VLANPOOL_PROC_DEL:
        {
            return OperateCallback(prs,nil);
        }
        break;
        
        case EN_NETPLANE_VLANPOOL_PROC_CHECK_ADD:
        case EN_NETPLANE_VLANPOOL_PROC_CHECK_DEL:
        {
            return CheckOperateCallback(prs,nil);
        }   
        break;
        default:
        {
            VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneVlanPool::DbProcCallback type(%d) is invalid.\n",type);
            return VNET_DB_ERROR_PARAM_INVALID;
        }
        break;
    }
    
    return 0;
}
    

int32 CDBOperateNetplaneVlanPool::Query(const char* uuid,vector<CDbNetplaneVlanPool> & outVInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneVlanPool::Query IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }


    // call procedure 
    vector<CADOParameter>  vInParam;
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)uuid);
    vInParam.push_back(inUuid);
    
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,\
        s_astrNetplaneVlanPoolProc[EN_NETPLANE_VLANPOOL_PROC_QUERY],\
        vInParam, \
        this,\
        EN_NETPLANE_VLANPOOL_PROC_QUERY,\
        static_cast<void*> (&outVInfo));
        
    if( ret != VNET_DB_SUCCESS )
    {
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneVlanPool::Query(%s) IProc->Handle failed(%d) \n",uuid,ret);
        }
        return ret;
    }
        
    return 0;
}

int32 CDBOperateNetplaneVlanPool::QueryPrivateNetIsolateNo(const char* NetplaneUUID, vector<int32>& outVInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneVlanPool::QueryPrivateNetIsolateNo IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }


    // call procedure 
    vector<CADOParameter>  vInParam;
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)NetplaneUUID);
    vInParam.push_back(inUuid);
    
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,\
        s_astrNetplaneVlanPoolProc[EN_NETPLANE_VLANPOOL_PROC_QUERY_PRIV_NET_ISOLATE_NO],\
        vInParam, \
        this,\
        EN_NETPLANE_VLANPOOL_PROC_QUERY_PRIV_NET_ISOLATE_NO,\
        static_cast<void*> (&outVInfo));
        
    if( ret != VNET_DB_SUCCESS )
    {
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneVlanPool::QueryPrivateNetIsolateNo(%s) IProc->Handle failed(%d) \n",
                NetplaneUUID,ret);
        }
        return ret;
    }
        
    return 0;
}

int32 CDBOperateNetplaneVlanPool::QueryShareNetAccessVlan(const char* NetplaneUUID, vector<int32>& outVInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneVlanPool::QueryShareNetAccessVlan IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }


    // call procedure 
    vector<CADOParameter>  vInParam;
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)NetplaneUUID);
    vInParam.push_back(inUuid);
    
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,\
        s_astrNetplaneVlanPoolProc[EN_NETPLANE_VLANPOOL_PROC_QUERY_SHARE_NET_ACCESS_VLAN],\
        vInParam, \
        this,\
        EN_NETPLANE_VLANPOOL_PROC_QUERY_SHARE_NET_ACCESS_VLAN,\
        static_cast<void*> (&outVInfo));
        
    if( ret != VNET_DB_SUCCESS )
    {
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneVlanPool::QueryShareNetAccessVlan(%s) IProc->Handle failed(%d) \n",
                NetplaneUUID,ret);
        }
        return ret;
    }
        
    return 0;
}

int32 CDBOperateNetplaneVlanPool::QueryShareNetTrunkVlan(const char* NetplaneUUID, 
                                                         vector<CDbNetplaneTrunkVlan>& outVInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneVlanPool::QueryShareNetTrunkVlan IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }


    // call procedure 
    vector<CADOParameter>  vInParam;
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)NetplaneUUID);
    vInParam.push_back(inUuid);
    
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,\
        s_astrNetplaneVlanPoolProc[EN_NETPLANE_VLANPOOL_PROC_QUERY_SHARE_NET_TRUNK_VLAN],\
        vInParam, \
        this,\
        EN_NETPLANE_VLANPOOL_PROC_QUERY_SHARE_NET_TRUNK_VLAN,\
        static_cast<void*> (&outVInfo));
        
    if( ret != VNET_DB_SUCCESS )
    {
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneVlanPool::QueryShareNetTrunkVlan(%s) IProc->Handle failed(%d) \n",
                NetplaneUUID,ret);
        }
        return ret;
    }
        
    return 0;
}

int32 CDBOperateNetplaneVlanPool::QueryCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneVlanPool::QueryCallback prs is NULL.\n");
        //VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }

    vector<CDbNetplaneVlanPool> *pvOut;
    pvOut = static_cast<vector<CDbNetplaneVlanPool>*>(nil);

    if( pvOut == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneVlanPool::QueryCallback pvOut is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    CDbNetplaneVlanPool info;

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 2);
       
    info.SetUuid(prs->GetFieldValue(0));

    int32 value = 0;
    if( false == prs->GetFieldValue(1,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneVlanPool::QueryCallback GetFieldValue failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetVlanNum(value);
    
    (*pvOut).push_back(info);

    return 0;
}

int32 CDBOperateNetplaneVlanPool::QueryPrivateNetIsolateNoCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneVlanPool::QueryPrivateNetIsolateNoCallback prs is NULL.\n");
        //VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }

    vector<int32> *pvOut;
    pvOut = static_cast<vector<int32>*>(nil);

    if( pvOut == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneVlanPool::QueryPrivateNetIsolateNoCallback pvOut is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 1);
    int32 value = 0;
    if( false == prs->GetFieldValue(0,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneVlanPool::QueryPrivateNetIsolateNoCallback GetFieldValue failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }    
    (*pvOut).push_back(value);

    return 0;
}

int32 CDBOperateNetplaneVlanPool::QueryShareNetAccessVlanCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneVlanPool::QueryShareNetAccessVlanCallback prs is NULL.\n");
        //VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }

    vector<int32> *pvOut;
    pvOut = static_cast<vector<int32>*>(nil);

    if( pvOut == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneVlanPool::QueryShareNetAccessVlanCallback pvOut is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 1);
    int32 value = 0;
    if( false == prs->GetFieldValue(0,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneVlanPool::QueryShareNetAccessVlanCallback GetFieldValue failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }    
    (*pvOut).push_back(value);

    return 0;
}

int32 CDBOperateNetplaneVlanPool::QueryShareNetTrunkVlanCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneVlanPool::QueryShareNetTrunkVlanCallback prs is NULL.\n");
        //VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }

    vector<CDbNetplaneTrunkVlan> *pvOut;
    pvOut = static_cast<vector<CDbNetplaneTrunkVlan>*>(nil);

    if( pvOut == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneVlanPool::QueryShareNetTrunkVlanCallback pvOut is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 3);
    int32 value = 0;
    CDbNetplaneTrunkVlan cTrunkVlan;
    if( false == prs->GetFieldValue(0,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneVlanPool::QueryShareNetTrunkVlanCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }  
    cTrunkVlan.SetVlanBegin(value);
    if( false == prs->GetFieldValue(1,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneVlanPool::QueryShareNetTrunkVlanCallback GetFieldValue(1) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    } 
    cTrunkVlan.SetVlanEnd(value);
    if( false == prs->GetFieldValue(2,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneVlanPool::QueryShareNetTrunkVlanCallback GetFieldValue(2) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    cTrunkVlan.SetNativeVlan(value);
    (*pvOut).push_back(cTrunkVlan);

    return 0;
}


int32 CDBOperateNetplaneVlanPool::CheckAdd(CDbNetplaneVlanPool & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneVlanPool::CheckAdd IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }
    
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUuid().c_str());    
    CADOParameter inVlanNum(CADOParameter::paramInt, CADOParameter::paramInput, info.GetVlanNum());
    
    vInParam.push_back(inUuid);   
    vInParam.push_back(inVlanNum);  
  
    // ret struct 
    int32 proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrNetplaneVlanPoolProc[EN_NETPLANE_VLANPOOL_PROC_CHECK_ADD],
        vInParam, 
        this,
        EN_NETPLANE_VLANPOOL_PROC_CHECK_ADD,
        static_cast<void*> (&proc_ret));
    
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneVlanPool::CheckAdd IProc->Handle failed(%d).\n", ret);
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneVlanPool::CheckAdd proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }

    return VNET_DB_SUCCESS;
}

int32 CDBOperateNetplaneVlanPool::Add(CDbNetplaneVlanPool & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneVlanPool::Add IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }
    
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUuid().c_str());    
    CADOParameter inVlanNum(CADOParameter::paramInt, CADOParameter::paramInput, info.GetVlanNum());
    
    vInParam.push_back(inUuid);   
    vInParam.push_back(inVlanNum); 
  
    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrNetplaneVlanPoolProc[EN_NETPLANE_VLANPOOL_PROC_ADD],
        vInParam, 
        this,
        EN_NETPLANE_VLANPOOL_PROC_ADD,
        static_cast<void*> (&proc_ret));
    
    if( ret != VNET_DB_SUCCESS )
    {        
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneVlanPool::Add IProc->Handle failed(%d).\n", ret);
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneVlanPool::Add proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }

    return VNET_DB_SUCCESS;
}

int32 CDBOperateNetplaneVlanPool::CheckDel(CDbNetplaneVlanPool & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneVlanPool::CheckDel IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }
    
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUuid().c_str());    
    CADOParameter inVlanNum(CADOParameter::paramInt, CADOParameter::paramInput, info.GetVlanNum());
    
    vInParam.push_back(inUuid);   
    vInParam.push_back(inVlanNum);  
  
    // ret struct 
    int32 proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrNetplaneVlanPoolProc[EN_NETPLANE_VLANPOOL_PROC_CHECK_DEL],
        vInParam, 
        this,
        EN_NETPLANE_VLANPOOL_PROC_CHECK_DEL,
        static_cast<void*> (&proc_ret));
    
    if( ret != VNET_DB_SUCCESS )
    {        
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneVlanPool::CheckDel IProc->Handle failed(%d).\n", ret);
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneVlanPool::CheckDel proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }

    return VNET_DB_SUCCESS;
}

int32 CDBOperateNetplaneVlanPool::Del(CDbNetplaneVlanPool & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneVlanPool::Del IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;    
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUuid().c_str());    
    CADOParameter inVlanNum(CADOParameter::paramInt, CADOParameter::paramInput, info.GetVlanNum());
    
    vInParam.push_back(inUuid);   
    vInParam.push_back(inVlanNum); 

    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrNetplaneVlanPoolProc[EN_NETPLANE_VLANPOOL_PROC_DEL],
        vInParam, 
        this,
        EN_NETPLANE_VLANPOOL_PROC_DEL,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneVlanPool::Del IProc->Handle failed. ret : %d.\n", ret);
        return ret;
    }
    
    if( proc_ret.GetRet() != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneVlanPool::Del proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }
    
    return 0;
}

int32 CDBOperateNetplaneVlanPool::OperateCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneVlanPool::OperateCallback param is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    
    CDbProcOperateResult *proc_ret = NULL;     
    proc_ret = static_cast<CDbProcOperateResult *>(nil);

    if( proc_ret == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneVlanPool::OperateCallback proc_ret is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 3);    

    int32 nRet = 0;
    if( false == prs->GetFieldValue(0, nRet))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneVlanPool::OperateCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetRet(nRet);

    int64 oid = 0;
    if( false == prs->GetFieldValue(1, oid))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneVlanPool::OperateCallback GetFieldValue(1) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetOid(oid);
    
    (*proc_ret).SetUuid(prs->GetFieldValue(2)); 

    return 0;
}

int32 CDBOperateNetplaneVlanPool::CheckOperateCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneVlanPool::CheckOperateCallback param is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    
    int32 *proc_ret = NULL;     
    proc_ret = static_cast<int32 *>(nil);

    if( proc_ret == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneVlanPool::CheckOperateCallback proc_ret is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 1);    

    int32 nRet = 0;
    if( false == prs->GetFieldValue(0, nRet))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneVlanPool::CheckOperateCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret) = nRet;

    return 0;
}


void CDBOperateNetplaneVlanPool::DbgShow()
{
#if 0
    cout << "--------------------------Port Info------------------------" << endl;
    // query summary
    vector<CDbNetplaneVlanPool> vInfo;
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
    vector<CDbNetplaneVlanPool>::iterator it = vInfo.begin();
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

void db_query_np_vlanpool(const char* uuid)
{
    if( NULL == uuid )
    {
        cout << "uuid is NULL" << endl;
        return;
    }
    
    CDBOperateNetplaneVlanPool * pOper = GetDbINetplaneVlanPool();
    if( NULL == pOper )
    {
        cout << "CDBOperateNetplaneVlanPool is NULL" << endl;
        return ;
    }

    vector<CDbNetplaneVlanPool> outVInfo;
    int ret = pOper->Query(uuid,outVInfo);
    if( ret != 0 )
    {
        cout << "Query failed." << ret << endl;
        return;
    }
    cout << "Query success." << endl;
    vector<CDbNetplaneVlanPool>::iterator it = outVInfo.begin();
    for(; it != outVInfo.end(); ++it)
    {
        cout << "------------" << endl;
        (*it).DbgShow();
    }    
}
DEFINE_DEBUG_FUNC(db_query_np_vlanpool);

void db_query_np_priv_net_isolate(const char* uuid)
{
    if( NULL == uuid )
    {
        cout << "uuid is NULL" << endl;
        return ;
    }
    
    CDBOperateNetplaneVlanPool * pOper = GetDbINetplaneVlanPool();
    if( NULL == pOper )
    {
        cout << "CDBOperateNetplaneVlanPool is NULL" << endl;
        return ;
    }

    vector<int32> outVInfo;
    int ret = pOper->QueryPrivateNetIsolateNo(uuid,outVInfo);
    if( ret != 0 )
    {
        cout << "Query failed." << ret << endl;
        return ;
    }
    cout << "Query success." << endl;
    vector<int32>::iterator it = outVInfo.begin();
    cout << "------isolate_no_list------" << endl;
    for(; it != outVInfo.end(); ++it)
    {
        cout << *it <<" ";
    }    
    cout << endl;
    return ;    
}
DEFINE_DEBUG_FUNC(db_query_np_priv_net_isolate);


void db_query_np_share_net_vlan(const char* uuid)
{
    if( NULL == uuid )
    {
        cout << "uuid is NULL" << endl;
        return ;
    }
    
    CDBOperateNetplaneVlanPool * pOper = GetDbINetplaneVlanPool();
    if( NULL == pOper )
    {
        cout << "CDBOperateNetplaneVlanPool is NULL" << endl;
        return ;
    }

    vector<int32> outVInfo;
    int ret = pOper->QueryShareNetAccessVlan(uuid,outVInfo);
    if( ret != 0 )
    {
        cout << "Query failed." << ret << endl;
        return ;
    }
    cout << "Query success." << endl;
    vector<int32>::iterator it = outVInfo.begin();
    cout << "------vlan_list------" << endl;
    for(; it != outVInfo.end(); ++it)
    {
        cout << *it <<" ";
    }    
    cout << endl;
    return ;    
}
DEFINE_DEBUG_FUNC(db_query_np_share_net_vlan);

void db_query_np_share_net_trunk_vlan(const char* uuid)
{
    if( NULL == uuid )
    {
        cout << "uuid is NULL" << endl;
        return ;
    }
    
    CDBOperateNetplaneVlanPool * pOper = GetDbINetplaneVlanPool();
    if( NULL == pOper )
    {
        cout << "CDBOperateNetplaneVlanPool is NULL" << endl;
        return ;
    }

    vector<CDbNetplaneTrunkVlan> outVInfo;
    int ret = pOper->QueryShareNetTrunkVlan(uuid,outVInfo);
    if( ret != 0 )
    {
        cout << "Query failed." << ret << endl;
        return ;
    }
    cout << "Query success." << endl;
    vector<CDbNetplaneTrunkVlan>::iterator it = outVInfo.begin();
    cout << "------Trunk vlan list------" << endl;
    for(; it != outVInfo.end(); ++it)
    {
        it->DbgShow();
    }    
    cout << endl;
    return ;    
}
DEFINE_DEBUG_FUNC(db_query_np_share_net_trunk_vlan);


void db_check_add_np_vlanpool(char* uuid, int32 vlan)
{
    if( NULL == uuid  )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateNetplaneVlanPool * pOper = GetDbINetplaneVlanPool();
    if( NULL == pOper )
    {
        cout << "CDBOperateNetplaneVlanPool is NULL" << endl;
        return ;
    }

    CDbNetplaneVlanPool info;
    info.SetUuid(uuid);
    //info.SetType(type);
    info.SetVlanNum(vlan);
    

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
DEFINE_DEBUG_FUNC(db_check_add_np_vlanpool);

void db_add_np_vlanpool(char* uuid, int32 vlan )
{
    if( NULL == uuid  )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateNetplaneVlanPool * pOper = GetDbINetplaneVlanPool();
    if( NULL == pOper )
    {
        cout << "CDBOperateNetplaneVlanPool is NULL" << endl;
        return ;
    }

    CDbNetplaneVlanPool info;
    info.SetUuid(uuid);
    //info.SetType(type);
    info.SetVlanNum(vlan);

    
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
DEFINE_DEBUG_FUNC(db_add_np_vlanpool);

void db_del_np_vlanpool(char * uuid, int32 vlan  )
{
    if( NULL == uuid  )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateNetplaneVlanPool * pOper = GetDbINetplaneVlanPool();
    if( NULL == pOper )
    {
        cout << "CDBOperateNetplaneVlanPool is NULL" << endl;
        return ;
    }

    CDbNetplaneVlanPool info;
    info.SetUuid(uuid);
    //info.SetType(type);
    info.SetVlanNum(vlan);
   

    //info.DbgShow();
    int ret = pOper->Del(info);
    if( ret != 0 )
    {
        cout << "Del failed." << ret << endl;
        return;
    }
    cout << "Del success." << endl;
    info.DbgShow();
}
DEFINE_DEBUG_FUNC(db_del_np_vlanpool);


}




