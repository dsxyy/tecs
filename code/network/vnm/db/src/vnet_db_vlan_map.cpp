

#include "vnet_db_error.h"
#include "vnet_db_proc.h"
#include "vnet_db_mgr.h"
#include "vnet_db_vlan_map.h"
    
namespace zte_tecs
{
typedef enum tagVlanMapProc
{
    EN_VLANMAP_PROC_CHECK_ADD = 0,
    EN_VLANMAP_PROC_ADD,
    EN_VLANMAP_PROC_CHECK_DEL,   
    EN_VLANMAP_PROC_DEL,
    EN_VLANMAP_PROC_QUERY_BY_PROJECT,
    EN_VLANMAP_PROC_QUERY_BY_PG,
    EN_VLANMAP_PROC_QUERY_BY_PROJECT_AND_PG,
    EN_VLANMAP_PROC_QUERY_BY_PROJECT_AND_ISOLATE,    
    EN_VLANMAP_PROC_QUERY_BY_NETPLANE,
    EN_VLANMAP_PROC_QUERY_BY_NP_AND_PROJECT_AND_ISOLATE,
    EN_VLANMAP_PROC_ALL,
}EN_VLANMAP_PROC;

const char *s_astrVlanMapProc[EN_VLANMAP_PROC_ALL] = 
{        
    "pf_net_check_add_vlanmap",
    "pf_net_add_vlanmap",
    "pf_net_check_del_vlanmap",
    "pf_net_del_vlanmap",  
    "pf_net_query_vlanmap_by_projectid",   
    "pf_net_query_vlanmap_by_portgroup",  
    "pf_net_query_vlanmap_by_projectid_and_pg",  
    "pf_net_query_vlanmap_by_projectid_and_isolate",
    "pf_net_query_vlanmap_by_netplane",
    "pf_net_query_vlanmap_by_netplane_and_project_and_isolate",
};

    
CDBOperateVlanMap::CDBOperateVlanMap(CVNetDbIProcedure * pProc)
{
    m_pIProc = pProc;
}

CDBOperateVlanMap::~CDBOperateVlanMap()
{
    
}

int32 CDBOperateVlanMap::DbProcCallback(int type, CADORecordset *prs, void* nil )
{
    switch(type)
    {
        case EN_VLANMAP_PROC_QUERY_BY_PROJECT:
        case EN_VLANMAP_PROC_QUERY_BY_PG:
        case EN_VLANMAP_PROC_QUERY_BY_PROJECT_AND_PG:
        case EN_VLANMAP_PROC_QUERY_BY_PROJECT_AND_ISOLATE:
        case EN_VLANMAP_PROC_QUERY_BY_NETPLANE:
        case EN_VLANMAP_PROC_QUERY_BY_NP_AND_PROJECT_AND_ISOLATE:
        {
            return QueryCallback(prs,nil);
        }
        break;
        case EN_VLANMAP_PROC_ADD:
        case EN_VLANMAP_PROC_DEL:
        {
            return OperateCallback(prs,nil);
        }
        break;
        
        case EN_VLANMAP_PROC_CHECK_ADD:
        case EN_VLANMAP_PROC_CHECK_DEL:
        {
            return CheckOperateCallback(prs,nil);
        }   
        break;
        default:
        {
            VNET_LOG(VNET_LOG_ERROR, "CDBOperateVlanMap::DbProcCallback type(%d) is invalid.\n",type);
            return VNET_DB_ERROR_PARAM_INVALID;
        }
        break;
    }
    
    return 0;
}
    
int32 CDBOperateVlanMap::QueryByProjectId(int64 project_id, vector<CDbVlanMap> & outVInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVlanMap::QueryByProjectId IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    vector<CADOParameter>  vInParam;
    CADOParameter inUuid(CADOParameter::paramInt64, CADOParameter::paramInput, project_id);    
    
    vInParam.push_back(inUuid);
    
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,\
        s_astrVlanMapProc[EN_VLANMAP_PROC_QUERY_BY_PROJECT],\
        vInParam, \
        this,\
        EN_VLANMAP_PROC_QUERY_BY_PROJECT,\
        static_cast<void*> (&outVInfo));
        
    if( ret != VNET_DB_SUCCESS )
    {   
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR,"CDBOperateVlanMap::QueryByProjectId(%lld) IProc->Handle failed(%d) \n",
                project_id,ret);
        }
        return ret;
    }
        
    return 0;
}

int32 CDBOperateVlanMap::QueryByPortGroup(const char* uuid, vector<CDbVlanMap> & outVInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVlanMap::QueryByPortGroup IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

     if( NULL == uuid )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVlanMap::QueryByPortGroup vna_uuid is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // call procedure 
    vector<CADOParameter>  vInParam;
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)uuid);    
    
    vInParam.push_back(inUuid);
    
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,\
        s_astrVlanMapProc[EN_VLANMAP_PROC_QUERY_BY_PG],\
        vInParam, \
        this,\
        EN_VLANMAP_PROC_QUERY_BY_PG,\
        static_cast<void*> (&outVInfo));
        
    if( ret != VNET_DB_SUCCESS )
    {
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR,"CDBOperateVlanMap::QueryByPortGroup(%s) IProc->Handle failed(%d) \n",
                uuid,ret);
        }
        return ret;
    }
        
    return 0;
}

int32 CDBOperateVlanMap::QueryByProjectIdPG(int64 project_id,const char* pg_uuid, vector<CDbVlanMap> & outVInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVlanMap::QueryByProjectIdPG IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == pg_uuid )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVlanMap::QueryByProjectIdPG uuid is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // call procedure 
    vector<CADOParameter>  vInParam;
    CADOParameter inProjectId(CADOParameter::paramInt64, CADOParameter::paramInput, project_id);   
    CADOParameter inPgUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)pg_uuid);    
    
    vInParam.push_back(inProjectId);
    vInParam.push_back(inPgUuid);
    
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,\
        s_astrVlanMapProc[EN_VLANMAP_PROC_QUERY_BY_PROJECT_AND_PG],\
        vInParam, \
        this,\
        EN_VLANMAP_PROC_QUERY_BY_PROJECT_AND_PG,\
        static_cast<void*> (&outVInfo));
        
    if( ret != VNET_DB_SUCCESS )
    {
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR,"CDBOperateVlanMap::QueryByProjectIdPG(%lld,%s) IProc->Handle failed(%d) \n",
                project_id, pg_uuid,ret);
        }
        return ret;
    }
        
    return 0;
}
int32 CDBOperateVlanMap::QueryByProjectIdIsolateNo(int64 project_id,int32 isolateNo, vector<CDbVlanMap> & outVInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVlanMap::QueryByProjectIdIsolateNo IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    vector<CADOParameter>  vInParam;
    CADOParameter inProjectId(CADOParameter::paramInt64, CADOParameter::paramInput, project_id);   
    CADOParameter inIsolate(CADOParameter::paramInt, CADOParameter::paramInput, isolateNo);    
    
    vInParam.push_back(inProjectId);
    vInParam.push_back(inIsolate);
    
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,\
        s_astrVlanMapProc[EN_VLANMAP_PROC_QUERY_BY_PROJECT_AND_ISOLATE],\
        vInParam, \
        this,\
        EN_VLANMAP_PROC_QUERY_BY_PROJECT_AND_ISOLATE,\
        static_cast<void*> (&outVInfo));
        
    if( ret != VNET_DB_SUCCESS )
    {
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR,"CDBOperateVlanMap::QueryByProjectIdIsolateNo(%lld,%d) IProc->Handle failed(%d) \n",
                project_id,isolateNo,ret);
        }
        return ret;
    }
        
    return 0;
}
int32 CDBOperateVlanMap::QueryByNpProjectIdIsolateNo(const char* np_uuid, int64 project_id,int32 isolateNo, vector<CDbVlanMap> & outVInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVlanMap::QueryByProjectIdIsolateNo IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    vector<CADOParameter>  vInParam;
    CADOParameter inNpUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)np_uuid); 
    CADOParameter inProjectId(CADOParameter::paramInt64, CADOParameter::paramInput, project_id);   
    CADOParameter inIsolate(CADOParameter::paramInt, CADOParameter::paramInput, isolateNo);    

    vInParam.push_back(inNpUuid);
    vInParam.push_back(inProjectId);
    vInParam.push_back(inIsolate);
    
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,\
        s_astrVlanMapProc[EN_VLANMAP_PROC_QUERY_BY_NP_AND_PROJECT_AND_ISOLATE],\
        vInParam, \
        this,\
        EN_VLANMAP_PROC_QUERY_BY_NP_AND_PROJECT_AND_ISOLATE,\
        static_cast<void*> (&outVInfo));
        
    if( ret != VNET_DB_SUCCESS )
    {
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR,"CDBOperateVlanMap::QueryByNpProjectIdIsolateNo(%s,%lld,%d) IProc->Handle failed(%d) \n",
                np_uuid,project_id,isolateNo, ret);
        }
        return ret;
    }
        
    return 0;
}    

int32 CDBOperateVlanMap::QueryByProjectIdPGIsolateNo(int64 project_id,const char* pg_uuid, int32 isolateNo, int32& outVlanNum)   
{
    int32 ret = 0;
    int32 bFind = FALSE;
    vector<CDbVlanMap> vInfo;
    ret = QueryByProjectIdPG(project_id,pg_uuid,vInfo);
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVlanMap::QueryByProjectIdPGIsolateNo failed(%d) \n",ret);
        return ret;
    }

    vector<CDbVlanMap>::iterator it = vInfo.begin();
    for(; it != vInfo.end(); ++it)
    {
        if (  (*it).GetIsolateNo() == isolateNo )
        {
            outVlanNum = (*it).GetVlanNum();
            bFind = TRUE;
            break;
        }
    }

    if( FALSE == bFind )
    {
        return VNET_DB_ERROR_ITEM_NO_EXIST;
    }
    
    return 0;
}

int32 CDBOperateVlanMap::QueryByNetplane(const char* uuid, vector<CDbVlanMap> & outVInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVlanMap::QueryByNetplane IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == uuid )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVlanMap::QueryByNetplane uuid is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // call procedure 
    vector<CADOParameter>  vInParam; 
    CADOParameter inPgUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)uuid);    
    
    vInParam.push_back(inPgUuid);
    
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,\
        s_astrVlanMapProc[EN_VLANMAP_PROC_QUERY_BY_NETPLANE],\
        vInParam, \
        this,\
        EN_VLANMAP_PROC_QUERY_BY_NETPLANE,\
        static_cast<void*> (&outVInfo));
        
    if( ret != VNET_DB_SUCCESS )
    {
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR,"CDBOperateVlanMap::QueryByNetplane(%s) IProc->Handle failed(%d) \n",
                uuid,ret);
        }
        return ret;
    }
        
    return 0;
}

int32 CDBOperateVlanMap::QueryCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVlanMap::QueryCallback prs is NULL.\n");
        //VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }

    vector<CDbVlanMap> *pvOut;
    pvOut = static_cast<vector<CDbVlanMap>*>(nil);

    if( pvOut == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVlanMap::QueryCallback pvOut is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    CDbVlanMap info;

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 7);
    info.SetNetplaneUuid(prs->GetFieldValue(0));
    info.SetNetplaneName(prs->GetFieldValue(1));   
    info.SetPgUuid(prs->GetFieldValue(2));
    info.SetPgName(prs->GetFieldValue(3));
    
    int64 v64 = 0;
    if( false == prs->GetFieldValue(4,v64) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVlanMap::QueryCallback GetFieldValue failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetProjectId(v64);
    
    int32 value = 0;
    if( false == prs->GetFieldValue(5,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVlanMap::QueryCallback GetFieldValue failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetVlanNum(value);

    if( false == prs->GetFieldValue(6,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVlanMap::QueryCallback GetFieldValue failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetIsolateNo(value);

    (*pvOut).push_back(info);

    return 0;
}


int32 CDBOperateVlanMap::CheckAdd(int64 projectId,const char* pg_uuid, int32 vlan_num,int32 IsolateNo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVlanMap::CheckAdd IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == pg_uuid )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVlanMap::CheckAdd pg_uuid is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }
        
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inProjectId(CADOParameter::paramInt64, CADOParameter::paramInput, projectId);    
    CADOParameter inPgUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)pg_uuid);    
    CADOParameter inVlanNum(CADOParameter::paramInt, CADOParameter::paramInput, vlan_num); 
    CADOParameter inIsolateNo(CADOParameter::paramInt, CADOParameter::paramInput, IsolateNo); 

    
    vInParam.push_back(inProjectId);
    vInParam.push_back(inPgUuid);
    vInParam.push_back(inVlanNum);  
    vInParam.push_back(inIsolateNo); 
  
    // ret struct 
    int32 proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrVlanMapProc[EN_VLANMAP_PROC_CHECK_ADD],
        vInParam, 
        this,
        EN_VLANMAP_PROC_CHECK_ADD,
        static_cast<void*> (&proc_ret));
    
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVlanMap::CheckAdd(%lld,%s,%d,%d) IProc->Handle failed(%d).\n", 
            projectId,pg_uuid,vlan_num,IsolateNo,ret);
        return ret;
    }
    
    // 重复添加 
    if( proc_ret == VNET_DB_PROCEDURE_RTN_HAS_SAME_ITEM )
    {
        return VNET_DB_SUCCESS;
    }

    if( proc_ret  != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVlanMap::CheckAdd(%lld,%s,%d,%d) proc return failed. ret : %d.\n", 
            projectId,pg_uuid,vlan_num,IsolateNo,GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }

    return VNET_DB_SUCCESS;
}

int32 CDBOperateVlanMap::Add(int64 projectId,const char* pg_uuid, int32 vlan_num,int32 IsolateNo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVlanMap::Add IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == pg_uuid )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVlanMap::Add pg_uuid is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }
        
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inProjectId(CADOParameter::paramInt64, CADOParameter::paramInput, projectId);    
    CADOParameter inPgUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)pg_uuid);    
    CADOParameter inVlanNum(CADOParameter::paramInt, CADOParameter::paramInput, vlan_num); 
    CADOParameter inIsolateNo(CADOParameter::paramInt, CADOParameter::paramInput, IsolateNo);
    
    vInParam.push_back(inProjectId);
    vInParam.push_back(inPgUuid);
    vInParam.push_back(inVlanNum); 
    vInParam.push_back(inIsolateNo); 
  
    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrVlanMapProc[EN_VLANMAP_PROC_ADD],
        vInParam, 
        this,
        EN_VLANMAP_PROC_ADD,
        static_cast<void*> (&proc_ret));
    
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVlanMap::Add(%lld,%s,%d,%d) IProc->Handle failed(%d).\n",
            projectId,pg_uuid,vlan_num,IsolateNo,ret);
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateVlanMap::Add(%lld,%s,%d,%d) proc return failed. ret : %d.\n",
            projectId,pg_uuid,vlan_num,IsolateNo,GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }

    return VNET_DB_SUCCESS;
}

int32 CDBOperateVlanMap::CheckDel(int64 projectId,const char* pg_uuid, int32 vlan_num,int32 IsolateNo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVlanMap::CheckDel IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == pg_uuid )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVlanMap::CheckDel pg_uuid is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }
        
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inProjectId(CADOParameter::paramInt64, CADOParameter::paramInput, projectId);    
    CADOParameter inPgUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)pg_uuid);    
    CADOParameter inVlanNum(CADOParameter::paramInt, CADOParameter::paramInput, vlan_num); 
    CADOParameter inIsolateNo(CADOParameter::paramInt, CADOParameter::paramInput, IsolateNo);
    
    vInParam.push_back(inProjectId);
    vInParam.push_back(inPgUuid);
    vInParam.push_back(inVlanNum);    
    vInParam.push_back(inIsolateNo); 

    // ret struct 
    int32 proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrVlanMapProc[EN_VLANMAP_PROC_CHECK_DEL],
        vInParam, 
        this,
        EN_VLANMAP_PROC_CHECK_DEL,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVlanMap::CheckDel(%lld,%s,%d,%d) IProc->Handle failed. ret : %d.\n", 
            projectId,pg_uuid,vlan_num,IsolateNo,ret);
        return ret;
    }
    
    if( proc_ret != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVlanMap::CheckDel(%lld,%s,%d,%d) proc return failed. ret : %d.\n", 
            projectId,pg_uuid,vlan_num,IsolateNo,GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }
    
    return 0;
}


int32 CDBOperateVlanMap::Del(int64 projectId,const char* pg_uuid, int32 vlan_num,int32 IsolateNo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVlanMap::Del IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == pg_uuid )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVlanMap::Del pg_uuid is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }
        
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inProjectId(CADOParameter::paramInt64, CADOParameter::paramInput, projectId);    
    CADOParameter inPgUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)pg_uuid);    
    CADOParameter inVlanNum(CADOParameter::paramInt, CADOParameter::paramInput, vlan_num); 
    CADOParameter inIsolateNo(CADOParameter::paramInt, CADOParameter::paramInput, IsolateNo);
    
    vInParam.push_back(inProjectId);
    vInParam.push_back(inPgUuid);
    vInParam.push_back(inVlanNum);   
    vInParam.push_back(inIsolateNo); 

    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrVlanMapProc[EN_VLANMAP_PROC_DEL],
        vInParam, 
        this,
        EN_VLANMAP_PROC_DEL,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVlanMap::Del(%lld,%s,%d,%d) IProc->Handle failed. ret : %d.\n", 
            projectId,pg_uuid,vlan_num,IsolateNo,ret);
        return ret;
    }
    
    if( proc_ret.GetRet() != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVlanMap::Del(%lld,%s,%d,%d) proc return failed. ret : %d.\n", 
            projectId,pg_uuid,vlan_num,IsolateNo,GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }
    
    return 0;
}

int32 CDBOperateVlanMap::OperateCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVlanMap::OperateCallback param is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    
    CDbProcOperateResult *proc_ret = NULL;     
    proc_ret = static_cast<CDbProcOperateResult *>(nil);

    if( proc_ret == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVlanMap::OperateCallback proc_ret is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 3);    

    int32 nRet = 0;
    if( false == prs->GetFieldValue(0, nRet))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVlanMap::OperateCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetRet(nRet);

    int64 oid = 0;
    if( false == prs->GetFieldValue(1, oid))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVlanMap::OperateCallback GetFieldValue(1) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetOid(oid);
    
    (*proc_ret).SetUuid(prs->GetFieldValue(2)); 

    return 0;
}

int32 CDBOperateVlanMap::CheckOperateCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVlanMap::CheckOperateCallback param is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    
    int32 *proc_ret = NULL;     
    proc_ret = static_cast<int32 *>(nil);

    if( proc_ret == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVlanMap::CheckOperateCallback proc_ret is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 1);    

    int32 nRet = 0;
    if( false == prs->GetFieldValue(0, nRet))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateVlanMap::CheckOperateCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret) = nRet;

    return 0;
}


void CDBOperateVlanMap::DbgShow()
{
#if 0
    cout << "--------------------------Port Info------------------------" << endl;
    // query summary
    vector<CDbVlanMap> vInfo;
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
    vector<CDbVlanMap>::iterator it = vInfo.begin();
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
void db_query_vlanmap_by_project(int64 project_id)
{    
    CDBOperateVlanMap * pOper = GetDbIVlanMap();
    if( NULL == pOper )
    {
        cout << "CDBOperateVlanMap is NULL" << endl;
        return ;
    }

    vector<CDbVlanMap> outVInfo;
    int ret = pOper->QueryByProjectId(project_id,outVInfo);
    if( ret != 0 )
    {
        cout << "QueryByProjectId failed." << ret << endl;
        return;
    }
    cout << "QueryByProjectId success." << endl;
    vector<CDbVlanMap>::iterator it = outVInfo.begin();
    for(; it != outVInfo.end(); ++it)
    {
        cout << "-----------------" << endl;
        (*it).DbgShow();
    }
    
}
DEFINE_DEBUG_FUNC(db_query_vlanmap_by_project);

void db_query_vlanmap_by_pg(const char* uuid)
{
    if( NULL == uuid )
    {
        cout << "vna uuid is NULL" << endl;
        return;
    }
    
    CDBOperateVlanMap * pOper = GetDbIVlanMap();
    if( NULL == pOper )
    {
        cout << "CDBOperateVlanMap is NULL" << endl;
        return ;
    }

    vector<CDbVlanMap> outVInfo;
    int ret = pOper->QueryByPortGroup(uuid,outVInfo);
    if( ret != 0 )
    {
        cout << "QueryByPortGroup failed." << ret << endl;
        return;
    }
    cout << "QueryByPortGroup success." << endl;
    vector<CDbVlanMap>::iterator it = outVInfo.begin();
    for(; it != outVInfo.end(); ++it)
    {
        cout << "-----------------" << endl;
        (*it).DbgShow();
    }    
}
DEFINE_DEBUG_FUNC(db_query_vlanmap_by_pg);

void db_query_vlan_by_projectpg(int64 project_id, const char* uuid)
{
    if( NULL == uuid)
    {
        cout << "vna uuid is NULL" << endl;
        return;
    }
    
    CDBOperateVlanMap * pOper = GetDbIVlanMap();
    if( NULL == pOper )
    {
        cout << "CDBOperateVlanMap is NULL" << endl;
        return ;
    }

    vector<CDbVlanMap> outVInfo;
    int ret = pOper->QueryByProjectIdPG(project_id,uuid,outVInfo);
    if( ret != 0 )
    {
        cout << "QueryByProjectIdPG failed." << ret << endl;
        return;
    }
    cout << "QueryByProjectIdPG success." << endl;
    vector<CDbVlanMap>::iterator it = outVInfo.begin();
    for(; it != outVInfo.end(); ++it)
    {
        cout << "-----------------" << endl;
        (*it).DbgShow();
    }
    
}
DEFINE_DEBUG_FUNC(db_query_vlan_by_projectpg);

void db_query_vlan_by_projectisolate(int64 project_id, int32 isolate)
{    
    CDBOperateVlanMap * pOper = GetDbIVlanMap();
    if( NULL == pOper )
    {
        cout << "CDBOperateVlanMap is NULL" << endl;
        return ;
    }

    vector<CDbVlanMap> outVInfo;
    int ret = pOper->QueryByProjectIdIsolateNo(project_id,isolate,outVInfo);
    if( ret != 0 )
    {
        cout << "QueryByProjectIdIsolateNo failed." << ret << endl;
        return;
    }
    cout << "QueryByProjectIdIsolateNo success." << endl;
    vector<CDbVlanMap>::iterator it = outVInfo.begin();
    for(; it != outVInfo.end(); ++it)
    {
        cout << "-----------------" << endl;
        (*it).DbgShow();
    }    
}
DEFINE_DEBUG_FUNC(db_query_vlan_by_projectisolate);

void db_query_vlan_by_npprojectisolate(char* np_uuid, int64 project_id, int32 isolate)
{    
    CDBOperateVlanMap * pOper = GetDbIVlanMap();
    if( NULL == pOper )
    {
        cout << "CDBOperateVlanMap is NULL" << endl;
        return ;
    }

    vector<CDbVlanMap> outVInfo;
    int ret = pOper->QueryByNpProjectIdIsolateNo(np_uuid, project_id,isolate,outVInfo);
    if( ret != 0 )
    {
        cout << "QueryByNpProjectIdIsolateNo failed." << ret << endl;
        return;
    }
    cout << "QueryByNpProjectIdIsolateNo success." << endl;
    vector<CDbVlanMap>::iterator it = outVInfo.begin();
    for(; it != outVInfo.end(); ++it)
    {
        cout << "-----------------" << endl;
        (*it).DbgShow();
    }    
}
DEFINE_DEBUG_FUNC(db_query_vlan_by_npprojectisolate);


void db_query_vlanmap_by_np(const char* uuid)
{
    if( NULL == uuid )
    {
        cout << "vna uuid is NULL" << endl;
        return;
    }
    
    CDBOperateVlanMap * pOper = GetDbIVlanMap();
    if( NULL == pOper )
    {
        cout << "CDBOperateVlanMap is NULL" << endl;
        return ;
    }

    vector<CDbVlanMap> outVInfo;
    int ret = pOper->QueryByNetplane(uuid,outVInfo);
    if( ret != 0 )
    {
        cout << "QueryByNetplane failed." << ret << endl;
        return;
    }
    cout << "QueryByNetplane success." << endl;
    vector<CDbVlanMap>::iterator it = outVInfo.begin();
    for(; it != outVInfo.end(); ++it)
    {
        cout << "-----------------" << endl;
        (*it).DbgShow();
    }    
}
DEFINE_DEBUG_FUNC(db_query_vlanmap_by_np);


void db_can_add_vlanmap(int64 project_id, const char* uuid, int32 vlan_num, int32 isolate)
{
    if( NULL == uuid)
    {
        cout << "vna uuid is NULL" << endl;
        return;
    }
    CDBOperateVlanMap * pOper = GetDbIVlanMap();
    if( NULL == pOper )
    {
        cout << "CDBOperateVlanMap is NULL" << endl;
        return ;
    }

    
    //info.DbgShow();
    int ret = pOper->CheckAdd(project_id,uuid,vlan_num,isolate);
    if( ret != 0 )
    {
        cout << "Add failed." << ret << endl;
        return;
    }
    cout << "Add success." << endl;
}
DEFINE_DEBUG_FUNC(db_can_add_vlanmap);

void db_add_vlanmap(int64 project_id, const char* uuid, int32 vlan_num,int32 isolate)
{
    if( NULL == uuid)
    {
        cout << "vna uuid is NULL" << endl;
        return;
    }
    CDBOperateVlanMap * pOper = GetDbIVlanMap();
    if( NULL == pOper )
    {
        cout << "CDBOperateVlanMap is NULL" << endl;
        return ;
    }

    
    //info.DbgShow();
    int ret = pOper->Add(project_id,uuid,vlan_num,isolate);
    if( ret != 0 )
    {
        cout << "Add failed." << ret << endl;
        return;
    }
    cout << "Add success." << endl;
}
DEFINE_DEBUG_FUNC(db_add_vlanmap);

void db_can_del_vlanmap(int64 project_id, const char* uuid, int32 vlan_num,int32 isolate)
{
    if( NULL == uuid)
    {
        cout << "port uuid is NULL" << endl;
        return;
    }
    CDBOperateVlanMap * pOper = GetDbIVlanMap();
    if( NULL == pOper )
    {
        cout << "CDBOperateVlanMap is NULL" << endl;
        return ;
    }
    
    //info.DbgShow();
    int ret = pOper->CheckDel(project_id,uuid,vlan_num,isolate);
    if( ret != 0 )
    {
        cout << "del failed." << ret << endl;
        return;
    }
    cout << "del success." << endl;
}
DEFINE_DEBUG_FUNC(db_can_del_vlanmap);

void db_del_vlanmap(int64 project_id, const char* uuid, int32 vlan_num,int32 isolate)
{
    if( NULL == uuid)
    {
        cout << "port uuid is NULL" << endl;
        return;
    }
    CDBOperateVlanMap * pOper = GetDbIVlanMap();
    if( NULL == pOper )
    {
        cout << "CDBOperateVlanMap is NULL" << endl;
        return ;
    }

    
    //info.DbgShow();
    int ret = pOper->Del(project_id,uuid,vlan_num,isolate);
    if( ret != 0 )
    {
        cout << "del failed." << ret << endl;
        return;
    }
    cout << "del success." << endl;
}
DEFINE_DEBUG_FUNC(db_del_vlanmap);


}



