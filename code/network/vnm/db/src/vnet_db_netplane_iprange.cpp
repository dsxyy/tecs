

#include "vnet_db_error.h"
#include "vnet_db_proc.h"
#include "vnet_db_mgr.h"
#include "vnet_db_netplane_iprange.h"
    
namespace zte_tecs
{
typedef enum tagNetplaneIpRangeProc
{
    EN_NETPLANE_IPRANGE_PROC_CHECK_ADD = 0,
    EN_NETPLANE_IPRANGE_PROC_ADD,
    EN_NETPLANE_IPRANGE_PROC_DEL,
    EN_NETPLANE_IPRANGE_PROC_FORCE_DEL,
    EN_NETPLANE_IPRANGE_PROC_QUERY,  
    EN_NETPLANE_IPRANGE_PROC_QUERY_ISUSING,
    EN_NETPLANE_IPRANGE_PROC_QUERY_ALLOC,
    EN_NETPLANE_IPRANGE_PROC_ALL,
}EN_NETPLANE_IPRANGE_PROC;

const char *s_astrNetplaneIpRangeProc[EN_NETPLANE_IPRANGE_PROC_ALL] = 
{        
    "pf_net_check_add_netplane_iprange",  
    "pf_net_add_netplane_iprange",    
    "pf_net_del_netplane_iprange", 
    "pf_net_force_del_netplane_iprange", 
    "pf_net_query_netplane_iprange", 
    "pf_net_query_netplane_iprange_is_using",
    "pf_net_query_netplane_iprange_alloc",
};
    
CDBOperateNetplaneIpRange::CDBOperateNetplaneIpRange(CVNetDbIProcedure * pProc)
{
    m_pIProc = pProc;
}

CDBOperateNetplaneIpRange::~CDBOperateNetplaneIpRange()
{
    
}

int32 CDBOperateNetplaneIpRange::DbProcCallback(int type, CADORecordset *prs, void* nil )
{
    switch(type)
    {
        
        case EN_NETPLANE_IPRANGE_PROC_QUERY:
        {
            return QueryCallback(prs,nil);
        }
        break;
        case EN_NETPLANE_IPRANGE_PROC_QUERY_ISUSING:
        {
            return QueryIsUsingCallback(prs,nil);
        }
        case EN_NETPLANE_IPRANGE_PROC_QUERY_ALLOC:
        {
            return QueryAllocNumCallback(prs,nil);
        }        
        break;
        case EN_NETPLANE_IPRANGE_PROC_ADD:
        case EN_NETPLANE_IPRANGE_PROC_DEL:
        case EN_NETPLANE_IPRANGE_PROC_FORCE_DEL:
        {
            return OperateCallback(prs,nil);
        }
        break;
        
        case EN_NETPLANE_IPRANGE_PROC_CHECK_ADD:
        {
            return CheckOperateCallback(prs,nil);
        }   
        break;
        default:
        {
            VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneIpRange::DbProcCallback type(%d) is invalid.\n",type);
            return VNET_DB_ERROR_PARAM_INVALID;
        }
        break;
    }
    
    return 0;
}


int32 CDBOperateNetplaneIpRange::QueryIsUsing(const char* npUuid, int64 begin, int64 end, int32 &isUsing)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneIpRange::QueryIsUsing IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( npUuid == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneIpRange::QueryIsUsing uuid is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // call procedure 
    vector<CADOParameter>  vInParam;
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)npUuid);
    CADOParameter inBegin(CADOParameter::paramInt64, CADOParameter::paramInput, begin);
    CADOParameter inEnd(CADOParameter::paramInt64, CADOParameter::paramInput, end);
    vInParam.push_back(inUuid);
    vInParam.push_back(inBegin);
    vInParam.push_back(inEnd);

    CDbNetplaneIpRangeIsUsing info; 
    
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,\
        s_astrNetplaneIpRangeProc[EN_NETPLANE_IPRANGE_PROC_QUERY_ISUSING],\
        vInParam, \
        this,\
        EN_NETPLANE_IPRANGE_PROC_QUERY_ISUSING,\
        static_cast<void*> (&info));
        
    if( ret != VNET_DB_SUCCESS )
    {
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneIpRange::QueryIsUsing(%s,%lld,%lld) IProc->Handle failed(%d) \n",
                npUuid,begin,end,ret);
        }
        return ret;
    }

    if( info.GetRet() != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneIpRange::QueryIsUsing(%s,%lld,%lld) proc return failed. ret : %d.\n", 
            npUuid,begin,end,GET_DB_ERROR_PROC(info.GetRet()));
        return GET_DB_ERROR_PROC(info.GetRet());
    }

    // 设置vsi num 值
    if( info.GetIsUsing() > 0 )
    {
        isUsing = 1;
    }
    else
    {
        isUsing = 0;
    }
        
    return 0;
}

int32 CDBOperateNetplaneIpRange::QueryAllocNum(const char* uuid, int64 begin, int64 end, int32 &outTotalNum)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneIpRange::QueryAllocNum IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( uuid == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneIpRange::QueryAllocNum uuid is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    if( begin > end )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneIpRange::QueryAllocNum begin > end.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // call procedure 
    vector<CADOParameter>  vInParam;
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)uuid);
    CADOParameter inBegin(CADOParameter::paramInt64, CADOParameter::paramInput, begin);
    CADOParameter inEnd(CADOParameter::paramInt64, CADOParameter::paramInput, end);
    vInParam.push_back(inUuid);
    vInParam.push_back(inBegin);
    vInParam.push_back(inEnd);    

    CDbResourceAllocNum Info;

    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,\
        s_astrNetplaneIpRangeProc[EN_NETPLANE_IPRANGE_PROC_QUERY_ALLOC],\
        vInParam, \
        this,\
        EN_NETPLANE_IPRANGE_PROC_QUERY_ALLOC,\
        static_cast<void*> (&Info));

    if( ret != VNET_DB_SUCCESS )
    {
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneIpRange::QueryAllocNum(%s,%lld,%lld) IProc->Handle failed(%d) \n",
                uuid,begin,end,ret);
        }
        return ret;
    }

    if( Info.GetRet() != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneIpRange::QueryAllocNum(%s,%lld,%lld) proc return failed. ret : %d.\n",
                uuid,begin,end,GET_DB_ERROR_PROC(Info.GetRet()));
        }
        return GET_DB_ERROR_PROC(Info.GetRet());
    }

    // 设置 num 值
    outTotalNum = Info.GetNum();
        
    return 0;
}


int32 CDBOperateNetplaneIpRange::Query(const char* uuid,vector<CDbNetplaneIpRange> & outVInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneIpRange::Query IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( uuid == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneIpRange::Query uuid is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // call procedure 
    vector<CADOParameter>  vInParam;
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)uuid);
    vInParam.push_back(inUuid);
    
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,\
        s_astrNetplaneIpRangeProc[EN_NETPLANE_IPRANGE_PROC_QUERY],\
        vInParam, \
        this,\
        EN_NETPLANE_IPRANGE_PROC_QUERY,\
        static_cast<void*> (&outVInfo));
        
    if( ret != VNET_DB_SUCCESS )
    {
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneIpRange::Query(%s) IProc->Handle failed(%d) \n",
                uuid,ret);
        }
        return ret;
    }
        
    return 0;
}

int32 CDBOperateNetplaneIpRange::QueryCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneIpRange::QueryCallback prs is NULL.\n");
        //VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }

    vector<CDbNetplaneIpRange> *pvOut;
    pvOut = static_cast<vector<CDbNetplaneIpRange>*>(nil);

    if( pvOut == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneIpRange::QueryCallback pvOut is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    CDbNetplaneIpRange info;

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 7);
           
    info.SetUuid(prs->GetFieldValue(0));

    int64 v64 = 0;
    if( false == prs->GetFieldValue(1,v64) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneIpRange::QueryCallback GetFieldValue failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetBeginNum(v64);
    
    if( false == prs->GetFieldValue(2,v64) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneIpRange::QueryCallback GetFieldValue failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetEndNum(v64);

    if( false == prs->GetFieldValue(3,v64) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneIpRange::QueryCallback GetFieldValue failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetMaskNum(v64);

    info.SetBeginStr(prs->GetFieldValue(4));
    info.SetEndStr(prs->GetFieldValue(5));
    info.SetMaskStr(prs->GetFieldValue(6));
    
    (*pvOut).push_back(info);

    return 0;
}


int32 CDBOperateNetplaneIpRange::CheckAdd(CDbNetplaneIpRange & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneIpRange::CheckAdd IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }
    
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUuid().c_str());    
    CADOParameter inBeginNum(CADOParameter::paramInt64, CADOParameter::paramInput, info.GetBeginNum());
    CADOParameter inEndNum(CADOParameter::paramInt64, CADOParameter::paramInput, info.GetEndNum());
    CADOParameter inMaskNum(CADOParameter::paramInt64, CADOParameter::paramInput, info.GetMaskNum());
    
    CADOParameter inBeginStr(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetBeginStr().c_str());    
    CADOParameter inEndStr(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetEndStr().c_str());    
    CADOParameter inMaskStr(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetMaskStr().c_str());    
    
    vInParam.push_back(inUuid);    
    vInParam.push_back(inBeginNum);    
    vInParam.push_back(inEndNum);   
    vInParam.push_back(inMaskNum);    
    vInParam.push_back(inBeginStr);    
    vInParam.push_back(inEndStr);    
    vInParam.push_back(inMaskStr);  
  
    // ret struct 
    int32 proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrNetplaneIpRangeProc[EN_NETPLANE_IPRANGE_PROC_CHECK_ADD],
        vInParam, 
        this,
        EN_NETPLANE_IPRANGE_PROC_CHECK_ADD,
        static_cast<void*> (&proc_ret));
    
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneIpRange::CheckAdd IProc->Handle failed(%d).\n", ret);
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneIpRange::CheckAdd proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }

    return VNET_DB_SUCCESS;
}

int32 CDBOperateNetplaneIpRange::Add(CDbNetplaneIpRange & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneIpRange::Add IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }
    
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUuid().c_str());    
    CADOParameter inBeginNum(CADOParameter::paramInt64, CADOParameter::paramInput, info.GetBeginNum());
    CADOParameter inEndNum(CADOParameter::paramInt64, CADOParameter::paramInput, info.GetEndNum());
    CADOParameter inMaskNum(CADOParameter::paramInt64, CADOParameter::paramInput, info.GetMaskNum());
    
    CADOParameter inBeginStr(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetBeginStr().c_str());    
    CADOParameter inEndStr(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetEndStr().c_str());    
    CADOParameter inMaskStr(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetMaskStr().c_str());    
    
    vInParam.push_back(inUuid);    
    vInParam.push_back(inBeginNum);    
    vInParam.push_back(inEndNum);   
    vInParam.push_back(inMaskNum);    
    vInParam.push_back(inBeginStr);    
    vInParam.push_back(inEndStr);    
    vInParam.push_back(inMaskStr);  
  
    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrNetplaneIpRangeProc[EN_NETPLANE_IPRANGE_PROC_ADD],
        vInParam, 
        this,
        EN_NETPLANE_IPRANGE_PROC_ADD,
        static_cast<void*> (&proc_ret));
    
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneIpRange::Add IProc->Handle failed(%d).\n", ret);
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneIpRange::Add proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }

    return VNET_DB_SUCCESS;
}


int32 CDBOperateNetplaneIpRange::Del(CDbNetplaneIpRange & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneIpRange::Del IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;    
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUuid().c_str());    
    CADOParameter inBeginNum(CADOParameter::paramInt64, CADOParameter::paramInput, info.GetBeginNum());
    CADOParameter inEndNum(CADOParameter::paramInt64, CADOParameter::paramInput, info.GetEndNum());
    CADOParameter inMaskNum(CADOParameter::paramInt64, CADOParameter::paramInput, info.GetMaskNum());
    
    CADOParameter inBeginStr(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetBeginStr().c_str());    
    CADOParameter inEndStr(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetEndStr().c_str());    
    CADOParameter inMaskStr(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetMaskStr().c_str());    
    
    vInParam.push_back(inUuid);   
    vInParam.push_back(inBeginNum);    
    vInParam.push_back(inEndNum);   
    vInParam.push_back(inMaskNum);    
    vInParam.push_back(inBeginStr);    
    vInParam.push_back(inEndStr);    
    vInParam.push_back(inMaskStr);  

    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrNetplaneIpRangeProc[EN_NETPLANE_IPRANGE_PROC_DEL],
        vInParam, 
        this,
        EN_NETPLANE_IPRANGE_PROC_DEL,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneIpRange::Del IProc->Handle failed. ret : %d.\n", ret);
        return ret;
    }
    
    if( proc_ret.GetRet() != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneIpRange::Del proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }
    
    return 0;
}

int32 CDBOperateNetplaneIpRange::ForceDel(CDbNetplaneIpRange & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneIpRange::ForceDel IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;    
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUuid().c_str());    
    CADOParameter inBeginNum(CADOParameter::paramInt64, CADOParameter::paramInput, info.GetBeginNum());
    CADOParameter inEndNum(CADOParameter::paramInt64, CADOParameter::paramInput, info.GetEndNum());
    CADOParameter inMaskNum(CADOParameter::paramInt64, CADOParameter::paramInput, info.GetMaskNum());
    
    CADOParameter inBeginStr(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetBeginStr().c_str());    
    CADOParameter inEndStr(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetEndStr().c_str());    
    CADOParameter inMaskStr(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetMaskStr().c_str());    
    
    vInParam.push_back(inUuid);   
    vInParam.push_back(inBeginNum);    
    vInParam.push_back(inEndNum);   
    vInParam.push_back(inMaskNum);    
    vInParam.push_back(inBeginStr);    
    vInParam.push_back(inEndStr);    
    vInParam.push_back(inMaskStr);  

    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrNetplaneIpRangeProc[EN_NETPLANE_IPRANGE_PROC_FORCE_DEL],
        vInParam, 
        this,
        EN_NETPLANE_IPRANGE_PROC_FORCE_DEL,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneIpRange::ForceDel IProc->Handle failed. ret : %d.\n", ret);
        return ret;
    }
    
    if( proc_ret.GetRet() != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneIpRange::ForceDel proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }
    
    return 0;
}

int32 CDBOperateNetplaneIpRange::OperateCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneIpRange::OperateCallback param is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    
    CDbProcOperateResult *proc_ret = NULL;     
    proc_ret = static_cast<CDbProcOperateResult *>(nil);

    if( proc_ret == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneIpRange::OperateCallback proc_ret is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 3);    

    int32 nRet = 0;
    if( false == prs->GetFieldValue(0, nRet))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneIpRange::OperateCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetRet(nRet);

    int64 oid = 0;
    if( false == prs->GetFieldValue(1, oid))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneIpRange::OperateCallback GetFieldValue(1) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetOid(oid);
    
    (*proc_ret).SetUuid(prs->GetFieldValue(2)); 

    return 0;
}

int32 CDBOperateNetplaneIpRange::CheckOperateCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneIpRange::CheckOperateCallback param is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    
    int32 *proc_ret = NULL;     
    proc_ret = static_cast<int32 *>(nil);

    if( proc_ret == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneIpRange::CheckOperateCallback proc_ret is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 1);    

    int32 nRet = 0;
    if( false == prs->GetFieldValue(0, nRet))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneIpRange::CheckOperateCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret) = nRet;

    return 0;
}

int32 CDBOperateNetplaneIpRange::QueryIsUsingCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneIpRange::QueryIsUsingCallback param is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    
    CDbNetplaneIpRangeIsUsing *proc_ret = NULL;     
    proc_ret = static_cast<CDbNetplaneIpRangeIsUsing *>(nil);

    if( proc_ret == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneIpRange::QueryIsUsingCallback proc_ret is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 2);    

    int32 value = 0;
    if( false == prs->GetFieldValue(0, value))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneIpRange::QueryIsUsingCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetRet(value);

    if( false == prs->GetFieldValue(1, value))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneIpRange::QueryIsUsingCallback GetFieldValue(1) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetIsUsing(value);

    return 0;
}

int32 CDBOperateNetplaneIpRange::QueryAllocNumCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneIpRange::QueryAllocNumCallback param is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    
    CDbResourceAllocNum *proc_ret = NULL;     
    proc_ret = static_cast<CDbResourceAllocNum *>(nil);

    if( proc_ret == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneIpRange::QueryAllocNumCallback proc_ret is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 2);    

    int32 value = 0;
    if( false == prs->GetFieldValue(0, value))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneIpRange::QueryAllocNumCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetRet(value);

    if( false == prs->GetFieldValue(1, value))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneIpRange::QueryAllocNumCallback GetFieldValue(1) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetNum(value);

    return 0;
}


void CDBOperateNetplaneIpRange::DbgShow()
{
#if 0
    cout << "--------------------------Port Info------------------------" << endl;
    // query summary
    vector<CDbNetplaneIpRange> vInfo;
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
    vector<CDbNetplaneIpRange>::iterator it = vInfo.begin();
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


void db_query_np_iprange_isusing(const char* uuid,int64 begin, int64 end)
{
    if( NULL == uuid )
    {
        cout << "uuid is NULL" << endl;
        return;
    }
    
    CDBOperateNetplaneIpRange * pOper = GetDbINetplaneIpRange();
    if( NULL == pOper )
    {
        cout << "CDBOperateNetplaneIpRange is NULL" << endl;
        return ;
    }

    int32 isUsing = 0;
    int ret = pOper->QueryIsUsing(uuid,begin,end,isUsing);
    if( ret != 0 )
    {
        cout << "Query failed." << ret << endl;
        return;
    }
    cout << "Query success." << isUsing << endl;
}
DEFINE_DEBUG_FUNC(db_query_np_iprange_isusing);

void db_query_np_iprange(const char* uuid)
{
    if( NULL == uuid )
    {
        cout << "uuid is NULL" << endl;
        return;
    }
    
    CDBOperateNetplaneIpRange * pOper = GetDbINetplaneIpRange();
    if( NULL == pOper )
    {
        cout << "CDBOperateNetplaneIpRange is NULL" << endl;
        return ;
    }

    vector<CDbNetplaneIpRange> outVInfo;
    int ret = pOper->Query(uuid,outVInfo);
    if( ret != 0 )
    {
        cout << "Query failed." << ret << endl;
        return;
    }
    cout << "Query success." << endl;
    vector<CDbNetplaneIpRange>::iterator it = outVInfo.begin();
    for(; it != outVInfo.end(); ++it)
    {
        cout << "------------" << endl;
        (*it).DbgShow();
    }    
}
DEFINE_DEBUG_FUNC(db_query_np_iprange);



void db_check_add_np_iprange(char* uuid, int64 begin, int64 end, int64 mask )
{
    if( NULL == uuid  )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateNetplaneIpRange * pOper = GetDbINetplaneIpRange();
    if( NULL == pOper )
    {
        cout << "CDBOperateNetplaneIpRange is NULL" << endl;
        return ;
    }

    CDbNetplaneIpRange info;
    info.SetUuid(uuid);
    info.SetBeginNum(begin);
    info.SetEndNum(end);
    info.SetMaskNum(mask);

    char buf[32]={0};        
    SPRINTF(buf,"%ld",begin);        
    info.SetBeginStr(buf);
    SPRINTF(buf,"%ld",end);
    info.SetEndStr(buf);
    SPRINTF(buf,"%ld",mask);
    info.SetMaskStr(buf);
    

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
DEFINE_DEBUG_FUNC(db_check_add_np_iprange);

void db_add_np_iprange(char* uuid, int64 begin, int64 end, int64 mask )
{
    if( NULL == uuid  )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateNetplaneIpRange * pOper = GetDbINetplaneIpRange();
    if( NULL == pOper )
    {
        cout << "CDBOperateNetplaneIpRange is NULL" << endl;
        return ;
    }

    CDbNetplaneIpRange info;
    info.SetUuid(uuid);
    //info.SetType(type);
    info.SetBeginNum(begin);
    info.SetEndNum(end);
    info.SetMaskNum(mask);

    char buf[32]={0};        
    SPRINTF(buf,"%ld",begin);        
    info.SetBeginStr(buf);
    SPRINTF(buf,"%ld",end);
    info.SetEndStr(buf);
    SPRINTF(buf,"%ld",mask);
    info.SetMaskStr(buf);

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
DEFINE_DEBUG_FUNC(db_add_np_iprange);

void db_del_np_iprange(char * uuid, int64 begin, int64 end, int64 mask )
{
    if( NULL == uuid  )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateNetplaneIpRange * pOper = GetDbINetplaneIpRange();
    if( NULL == pOper )
    {
        cout << "CDBOperateNetplaneIpRange is NULL" << endl;
        return ;
    }

    CDbNetplaneIpRange info;
    info.SetUuid(uuid);
    //info.SetType(type);
    info.SetBeginNum(begin);
    info.SetEndNum(end);
    info.SetMaskNum(mask);

    char buf[32]={0};        
    SPRINTF(buf,"%ld",begin);        
    info.SetBeginStr(buf);
    SPRINTF(buf,"%ld",end);
    info.SetEndStr(buf);
    SPRINTF(buf,"%ld",mask);
    info.SetMaskStr(buf);
    

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
DEFINE_DEBUG_FUNC(db_del_np_iprange);

void db_force_del_np_iprange(char * uuid, int64 begin, int64 end, int64 mask )
{
    if( NULL == uuid  )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateNetplaneIpRange * pOper = GetDbINetplaneIpRange();
    if( NULL == pOper )
    {
        cout << "CDBOperateNetplaneIpRange is NULL" << endl;
        return ;
    }

    CDbNetplaneIpRange info;
    info.SetUuid(uuid);
    //info.SetType(type);
    info.SetBeginNum(begin);
    info.SetEndNum(end);
    info.SetMaskNum(mask);

    char buf[32]={0};        
    SPRINTF(buf,"%ld",begin);        
    info.SetBeginStr(buf);
    SPRINTF(buf,"%ld",end);
    info.SetEndStr(buf);
    SPRINTF(buf,"%ld",mask);
    info.SetMaskStr(buf);
    

    //info.DbgShow();
    int ret = pOper->ForceDel(info);
    if( ret != 0 )
    {
        cout << "Force del failed." << ret << endl;
        return;
    }
    cout << "Force del success." << endl;
    info.DbgShow();
}
DEFINE_DEBUG_FUNC(db_force_del_np_iprange);

}




