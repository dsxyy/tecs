
#include "vnet_db_netplane.h"
#include "vnet_db_error.h"
#include "vnet_db_proc.h"
#include "vnet_db_mgr.h"

namespace zte_tecs
{
typedef enum tagNetplaneProc
{
    EN_NETPLANE_PROC_QUERYSUMMARY = 0,
    EN_NETPLANE_PROC_QUERY,     
    EN_NETPLANE_PROC_ADD,
    EN_NETPLANE_PROC_MODIFY,
    EN_NETPLANE_PROC_DEL,
    EN_NETPLANE_PROC_ALL,
}EN_NETPLANE_PROC;

const char *s_astrNetplaneProc[EN_NETPLANE_PROC_ALL] = 
{        
    "pf_net_query_netplane_summary",
    "pf_net_query_netplane",
    "pf_net_add_netplane",    
    "pf_net_modify_netplane",  
    "pf_net_del_netplane",  
};

    
CDBOperateNetplane::CDBOperateNetplane(CVNetDbIProcedure * pProc)
{
    m_pIProc = pProc;
}

CDBOperateNetplane::~CDBOperateNetplane()
{
    
}

int32 CDBOperateNetplane::DbProcCallback(int type, CADORecordset *prs, void* nil )
{
    switch(type)
    {
        case EN_NETPLANE_PROC_QUERYSUMMARY:
        {
            return QueryNetplaneSummaryCallback(prs,nil);
        }
        break;
        case EN_NETPLANE_PROC_QUERY:
        {
            return QueryNetplaneCallback(prs,nil);
        }
        break;
        case EN_NETPLANE_PROC_ADD:
        case EN_NETPLANE_PROC_MODIFY:
        case EN_NETPLANE_PROC_DEL:
        {
            return OperateNetplaneCallback(prs,nil);
        }
        break;
        default:
        {
            VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplane::DbProcCallback type(%d) is invalid.\n",type);
            return VNET_DB_ERROR_PARAM_INVALID;
        }
        break;
    }
    
    return 0;
}
    
int32 CDBOperateNetplane::QueryNetplaneSummary(vector<CDBObjectBase> & outVInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplane::QueryNetplaneSummary IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    vector<CADOParameter>  vInParam;
    
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,\
        s_astrNetplaneProc[EN_NETPLANE_PROC_QUERYSUMMARY],\
        vInParam, \
        this,\
        EN_NETPLANE_PROC_QUERYSUMMARY,\
        static_cast<void*> (&outVInfo));
        
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplane::QueryNetplaneSummary IProc->Handle failed(%d) \n",ret);
        return ret;
    }
        
    return 0;
}

int32 CDBOperateNetplane::QueryNetplaneSummaryCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplane::QueryNetplaneSummaryCallback prs is NULL.\n");
        //VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }

    vector<CDBObjectBase> *pvOut;
    pvOut = static_cast<vector<CDBObjectBase>*>(nil);

    if( pvOut == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplane::QueryNetplaneSummaryCallback pvOut is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    CDBObjectBase info;

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 2);
    info.SetName(prs->GetFieldValue(0));
    info.SetUuid(prs->GetFieldValue(1));

    (*pvOut).push_back(info);

    return 0;
}

int32 CDBOperateNetplane::QueryNetplane(CDbNetplane & Info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplane::QueryNetplane IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    vector<CADOParameter> vInParam;
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)Info.GetUuid().c_str());
    vInParam.push_back(inUuid);
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrNetplaneProc[EN_NETPLANE_PROC_QUERY],
        vInParam, 
        this,
        EN_NETPLANE_PROC_QUERY,
        static_cast<void*> (&Info));
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            Info.DbgShow();
        }
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplane::QueryNetplane IProc->Handle failed(%d).\n",ret);
        }
        return ret;
    }
    
    return 0;
}

int32 CDBOperateNetplane::QueryNetplaneCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplane::QueryNetplaneCallback prs is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
        
    CDbNetplane *pOutNetplane;
    pOutNetplane = static_cast<CDbNetplane *>(nil);

    if( pOutNetplane == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplane::QueryNetplaneCallback pOutNetplane is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 6);    

    int32 proc_ret = 0;
    if( false == prs->GetFieldValue(0,proc_ret ))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplane::QueryNetplaneCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }    

    if( 0 != proc_ret )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplane::QueryNetplaneCallback proc failed(%d).\n", GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }

    (*pOutNetplane).SetUuid( prs->GetFieldValue(1));
    (*pOutNetplane).SetName( prs->GetFieldValue(2));
    (*pOutNetplane).SetDesc( prs->GetFieldValue(3));  
    int32 mtu = 0;
    if( false == prs->GetFieldValue(4,mtu) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplane::QueryNetplaneCallback GetFieldValue failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutNetplane).SetMtu(mtu);

    int32 key = 0;
    if( false == prs->GetFieldValue(5,key) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplane::QueryNetplaneCallback GetFieldValue failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*pOutNetplane).SetId(key);
    

    return 0;
}

int32 CDBOperateNetplane::AddNetplane(CDbNetplane & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplane::AddNetplane IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // uuid generate
    info.SetUuid(GenerateUUID().c_str());
    
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;    
    CADOParameter inNetplane(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetName().c_str());
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUuid().c_str());    
    CADOParameter inDesc(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetDesc().c_str());    
    CADOParameter inMtu(CADOParameter::paramInt, CADOParameter::paramInput, info.GetMtu());  
    CADOParameter inKey(CADOParameter::paramInt, CADOParameter::paramInput, info.GetId());    
    
    
    vInParam.push_back(inNetplane);
    vInParam.push_back(inUuid);
    vInParam.push_back(inDesc);
    vInParam.push_back(inMtu);    
    vInParam.push_back(inKey);   
    
    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrNetplaneProc[EN_NETPLANE_PROC_ADD],
        vInParam, 
        this,
        EN_NETPLANE_PROC_ADD,
        static_cast<void*> (&proc_ret));
    
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplane::AddNetplane IProc->Handle failed(%d).\n", ret);
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplane::AddNetplane proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }

    return VNET_DB_SUCCESS;
}


int32 CDBOperateNetplane::ModifyNetplane(CDbNetplane & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplane::ModifyNetplane IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;    
    CADOParameter inNetplane(CADOParameter::paramText, CADOParameter::paramInput, (char* )info.GetName().c_str());
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char* )info.GetUuid().c_str());    
    CADOParameter inDesc(CADOParameter::paramText, CADOParameter::paramInput, (char* )info.GetDesc().c_str());    
    CADOParameter inMtu(CADOParameter::paramInt, CADOParameter::paramInput, info.GetMtu());    

    vInParam.push_back(inUuid);
    vInParam.push_back(inNetplane);
    vInParam.push_back(inDesc);
    vInParam.push_back(inMtu);    
    
    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrNetplaneProc[EN_NETPLANE_PROC_MODIFY],
        vInParam, 
        this,
        EN_NETPLANE_PROC_MODIFY,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplane::ModifyNetplane IProc->Handle failed(%d).\n", ret);
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplane::ModifyNetplane proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }
    
    return VNET_DB_SUCCESS;
}

int32 CDBOperateNetplane::DelNetplane(const char* netplane_uuid)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplane::DelNetplane IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == netplane_uuid )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplane::DelNetplane netplane_uuid is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;    
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)netplane_uuid);    
    
    vInParam.push_back(inUuid);

    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrNetplaneProc[EN_NETPLANE_PROC_DEL],
        vInParam, 
        this,
        EN_NETPLANE_PROC_DEL,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplane::DelNetplane(%s) IProc->Handle failed. ret : %d.\n",
            netplane_uuid,ret);
        return ret;
    }
    
    if( proc_ret.GetRet() != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplane::DelNetplane(%s) proc return failed. ret : %d.\n",
            netplane_uuid,GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }
    
    return 0;
}

int32 CDBOperateNetplane::OperateNetplaneCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplane::OperateNetplaneCallback param is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    
    CDbProcOperateResult *proc_ret = NULL;     
    proc_ret = static_cast<CDbProcOperateResult *>(nil);

    if( proc_ret == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplane::OperateNetplaneCallback proc_ret is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 3);    

    int32 nRet = 0;
    if( false == prs->GetFieldValue(0, nRet))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplane::OperateNetplaneCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetRet(nRet);

    int64 oid = 0;
    if( false == prs->GetFieldValue(1, oid))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplane::OperateNetplaneCallback GetFieldValue(1) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetOid(oid);
    
    (*proc_ret).SetUuid(prs->GetFieldValue(2)); 

    return 0;
}

void CDBOperateNetplane::DbgShow()
{
    cout << "--------------------------netplane Info------------------------" << endl;
    // query summary
    vector<CDBObjectBase> vInfo;
    int ret = 0;
    ret = QueryNetplaneSummary(vInfo);
    if( ret != 0 )
    {
        cout << "QueryNetplaneSummary failed. ret : " << ret << endl;
        return;
    }

    cout << "QueryNetplaneSummary success " << endl;
    cout << "|" <<setw(32) << "uuid" << 
                    "|" << setw(32) << "name" << endl;
    vector<CDBObjectBase>::iterator it = vInfo.begin();
    for(; it != vInfo.end(); ++it)
    {
        (*it).DbgShow();
    }
    
    // query 
    cout << "-----------------------------------" << endl;
    it = vInfo.begin();
    for(; it != vInfo.end(); ++it)
    {
        CDbNetplane info;
        info.SetUuid((*it).GetUuid().c_str());
        ret = QueryNetplane(info);
        if( ret != 0 )
        {
            cout << "QueryNetplane failed. ret : " << ret << endl;
            continue;
        }

        info.DbgShow();
        cout << "-----------------" << endl;        
    }
}

}
