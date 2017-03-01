

#include "vnet_db_error.h"
#include "vnet_db_proc.h"
#include "vnet_db_mgr.h"
#include "vnet_db_netplane_segmentrange.h"
#include "vnet_db_netplane_segmentpool.h"
    
namespace zte_tecs
{
typedef enum tagNetplaneSegmentPoolPoolProc
{
    EN_NETPLANE_SEGMENTPOOL_PROC_CHECK_ADD = 0,
    EN_NETPLANE_SEGMENTPOOL_PROC_ADD,
    EN_NETPLANE_SEGMENTPOOL_PROC_CHECK_DEL,
    EN_NETPLANE_SEGMENTPOOL_PROC_DEL,
    EN_NETPLANE_SEGMENTPOOL_PROC_QUERY,
    EN_NETPLANE_SEGMENTPOOL_PROC_QUERY_SHARE_NET_SEGMENT,
    EN_NETPLANE_SEGMENTPOOL_PROC_QUERY_VXLAN_PRIV_NET_ISOLATE_NO,    
    EN_NETPLANE_SEGMENTPOOL_PROC_QUERY_SHARE_BY_VNA_NP_SEG,   
    EN_NETPLANE_SEGMENTPOOL_PROC_QUERY_PRIVATE_BY_VNA_NP_SEG,   
    EN_NETPLANE_SEGMENTPOOL_PROC_ALL,
}EN_NETPLANE_SEGMENTPOOL_PROC;

const char *s_astrNetplaneSegmentPoolProc[EN_NETPLANE_SEGMENTPOOL_PROC_ALL] = 
{        
    "pf_net_check_add_netplane_segmentpool",  
    "pf_net_add_netplane_segmentpool",  
    "pf_net_check_del_netplane_segmentpool",
    "pf_net_del_netplane_segmentpool", 
    "pf_net_query_netplane_segmentpool", 
    "pf_net_query_vm_vxlan_share_pg_by_netplane",     
    "pf_net_query_vm_vxlan_private_pg_isolate_no_list_by_netplane",
    "pf_net_query_segmentid_share_by_vna_np_seg", 
    "pf_net_query_segmentid_private_by_vna_np_seg",
};

    
CDBOperateNetplaneSegmentPool::CDBOperateNetplaneSegmentPool(CVNetDbIProcedure * pProc)
{
    m_pIProc = pProc;
}

CDBOperateNetplaneSegmentPool::~CDBOperateNetplaneSegmentPool()
{
    
}

int32 CDBOperateNetplaneSegmentPool::DbProcCallback(int type, CADORecordset *prs, void* nil )
{
    switch(type)
    {
        
        case EN_NETPLANE_SEGMENTPOOL_PROC_QUERY:
        {
            return QueryCallback(prs,nil);
        }
        break;
        case EN_NETPLANE_SEGMENTPOOL_PROC_QUERY_SHARE_BY_VNA_NP_SEG:
        case EN_NETPLANE_SEGMENTPOOL_PROC_QUERY_PRIVATE_BY_VNA_NP_SEG: 
        {
            return QuerySegIdIplistCallback(prs,nil);
        }
        break;

        case EN_NETPLANE_SEGMENTPOOL_PROC_QUERY_SHARE_NET_SEGMENT: // 和private isolateNo是相同的
        case EN_NETPLANE_SEGMENTPOOL_PROC_QUERY_VXLAN_PRIV_NET_ISOLATE_NO:
        {
            return QueryPrivateNetIsolateNoCallback(prs,nil);
        }
        break;
        case EN_NETPLANE_SEGMENTPOOL_PROC_ADD:
        case EN_NETPLANE_SEGMENTPOOL_PROC_DEL:
        {
            return OperateCallback(prs,nil);
        }
        break;
        
        case EN_NETPLANE_SEGMENTPOOL_PROC_CHECK_ADD:
        case EN_NETPLANE_SEGMENTPOOL_PROC_CHECK_DEL:
        {
            return CheckOperateCallback(prs,nil);
        }   
        break;
        default:
        {
            VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneSegmentPool::DbProcCallback type(%d) is invalid.\n",type);
            return VNET_DB_ERROR_PARAM_INVALID;
        }
        break;
    }
    
    return 0;
}
    

int32 CDBOperateNetplaneSegmentPool::Query(const char* uuid,vector<CDbNetplaneSegmentPool> & outVInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneSegmentPool::Query IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }


    // call procedure 
    vector<CADOParameter>  vInParam;
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)uuid);
    vInParam.push_back(inUuid);
    
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,\
        s_astrNetplaneSegmentPoolProc[EN_NETPLANE_SEGMENTPOOL_PROC_QUERY],\
        vInParam, \
        this,\
        EN_NETPLANE_SEGMENTPOOL_PROC_QUERY,\
        static_cast<void*> (&outVInfo));
        
    if( ret != VNET_DB_SUCCESS )
    {
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneSegmentPool::Query(%s) IProc->Handle failed(%d) \n",uuid,ret);
        }
        return ret;
    }
        
    return 0;
}

int32 CDBOperateNetplaneSegmentPool::QueryPrivateNetIsolateNo(const char* NetplaneUUID, vector<int32>& outVInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneSegmentPool::QueryPrivateNetIsolateNo IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }


    // call procedure 
    vector<CADOParameter>  vInParam;
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)NetplaneUUID);
    vInParam.push_back(inUuid);
    
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,\
        s_astrNetplaneSegmentPoolProc[EN_NETPLANE_SEGMENTPOOL_PROC_QUERY_VXLAN_PRIV_NET_ISOLATE_NO],\
        vInParam, \
        this,\
        EN_NETPLANE_SEGMENTPOOL_PROC_QUERY_VXLAN_PRIV_NET_ISOLATE_NO,\
        static_cast<void*> (&outVInfo));
        
    if( ret != VNET_DB_SUCCESS )
    {
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneSegmentPool::QueryPrivateNetIsolateNo(%s) IProc->Handle failed(%d) \n",
                NetplaneUUID,ret);
        }
        return ret;
    }
        
    return 0;
}

int32 CDBOperateNetplaneSegmentPool::QueryShareNetSegment(const char* NetplaneUUID, vector<int32>& outVInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneSegmentPool::QueryShareNetAccessSegment IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }


    // call procedure 
    vector<CADOParameter>  vInParam;
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)NetplaneUUID);
    vInParam.push_back(inUuid);
    
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,\
        s_astrNetplaneSegmentPoolProc[EN_NETPLANE_SEGMENTPOOL_PROC_QUERY_SHARE_NET_SEGMENT],\
        vInParam, \
        this,\
        EN_NETPLANE_SEGMENTPOOL_PROC_QUERY_SHARE_NET_SEGMENT,\
        static_cast<void*> (&outVInfo));
        
    if( ret != VNET_DB_SUCCESS )
    {
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneSegmentPool::QueryShareNetAccessSegment(%s) IProc->Handle failed(%d) \n",
                NetplaneUUID,ret);
        }
        return ret;
    }
        
    return 0;
}

int32 CDBOperateNetplaneSegmentPool::QueryCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneSegmentPool::QueryCallback prs is NULL.\n");
        //VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }

    vector<CDbNetplaneSegmentPool> *pvOut;
    pvOut = static_cast<vector<CDbNetplaneSegmentPool>*>(nil);

    if( pvOut == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneSegmentPool::QueryCallback pvOut is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    CDbNetplaneSegmentPool info;

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 2);
       
    info.SetUuid(prs->GetFieldValue(0));

    int32 value = 0;
    if( false == prs->GetFieldValue(1,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneSegmentPool::QueryCallback GetFieldValue failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetSegmentNum(value);
    
    (*pvOut).push_back(info);

    return 0;
}

int32 CDBOperateNetplaneSegmentPool::QueryPrivateNetIsolateNoCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneSegmentPool::QueryPrivateNetIsolateNoCallback prs is NULL.\n");
        //VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }

    vector<int32> *pvOut;
    pvOut = static_cast<vector<int32>*>(nil);

    if( pvOut == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneSegmentPool::QueryPrivateNetIsolateNoCallback pvOut is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 1);
    int32 value = 0;
    if( false == prs->GetFieldValue(0,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneSegmentPool::QueryPrivateNetIsolateNoCallback GetFieldValue failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }    
    (*pvOut).push_back(value);

    return 0;
}

#if 0
int32 CDBOperateNetplaneSegmentPool::QueryShareNetAccessSegmentCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneSegmentPool::QueryShareNetAccessSegmentCallback prs is NULL.\n");
        //VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }

    vector<int32> *pvOut;
    pvOut = static_cast<vector<int32>*>(nil);

    if( pvOut == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneSegmentPool::QueryShareNetAccessSegmentCallback pvOut is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 1);
    int32 value = 0;
    if( false == prs->GetFieldValue(0,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneSegmentPool::QueryShareNetAccessSegmentCallback GetFieldValue failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }    
    (*pvOut).push_back(value);

    return 0;
}
int32 CDBOperateNetplaneSegmentPool::QueryShareNetTrunkSegmentCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneSegmentPool::QueryShareNetTrunkSegmentCallback prs is NULL.\n");
        //VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }

    vector<CDbNetplaneTrunkSegment> *pvOut;
    pvOut = static_cast<vector<CDbNetplaneTrunkSegment>*>(nil);

    if( pvOut == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneSegmentPool::QueryShareNetTrunkSegmentCallback pvOut is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 3);
    int32 value = 0;
    CDbNetplaneTrunkSegment cTrunkSegment;
    if( false == prs->GetFieldValue(0,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneSegmentPool::QueryShareNetTrunkSegmentCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }  
    cTrunkSegment.SetSegmentBegin(value);
    if( false == prs->GetFieldValue(1,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneSegmentPool::QueryShareNetTrunkSegmentCallback GetFieldValue(1) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    } 
    cTrunkSegment.SetSegmentEnd(value);
    if( false == prs->GetFieldValue(2,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneSegmentPool::QueryShareNetTrunkSegmentCallback GetFieldValue(2) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    cTrunkSegment.SetNativeSegment(value);
    (*pvOut).push_back(cTrunkSegment);

    return 0;
}
#endif


int32 CDBOperateNetplaneSegmentPool::QuerySegIdIplistByVnaNpSeg(const char* vna_uuid, const char* np_uuid, int32 SegNum, vector<CDbVnaNpSegmentId> & outVInfo)
{
    int32 ret = 0;
    vector<CDbVnaNpSegmentId> vTmp; 

    ret = QuerySegIdIplistShareByVnaNpSeg(vna_uuid, np_uuid,SegNum, outVInfo );
    if( ret != 0 )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneSegmentPool::QuerySegmentIdByVnaNp QuerySegmentIdShareByVnaNp failed.\n");
        return ret;
    }

    ret = QuerySegIdIplistPrivateByVnaNpSeg(vna_uuid, np_uuid,SegNum, vTmp );
    if( ret != 0 )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneSegmentPool::QuerySegmentIdByVnaNp QuerySegmentIdShareByVnaNp failed.\n");
        return ret;
    }

    outVInfo.insert(outVInfo.end(),vTmp.begin(),vTmp.end()); 
    
    return 0;
}

int32 CDBOperateNetplaneSegmentPool::QuerySegIdIplistShareByVnaNpSeg(const char* vna_uuid, const char* np_uuid,int32 SegNum,vector<CDbVnaNpSegmentId> & outVInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneSegmentPool::QuerySegmentIdShareByVnaNp IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == vna_uuid || NULL == np_uuid )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneSegmentPool::QuerySegmentIdShareByVnaNp param is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // call procedure 
    vector<CADOParameter>  vInParam;
    CADOParameter inVnaUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)vna_uuid);
    CADOParameter inNpUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)np_uuid);    
    CADOParameter inSeg(CADOParameter::paramInt, CADOParameter::paramInput, SegNum);    
    vInParam.push_back(inVnaUuid);
    vInParam.push_back(inNpUuid);
    vInParam.push_back(inSeg);    
    
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,\
        s_astrNetplaneSegmentPoolProc[EN_NETPLANE_SEGMENTPOOL_PROC_QUERY_SHARE_BY_VNA_NP_SEG],\
        vInParam, \
        this,\
        EN_NETPLANE_SEGMENTPOOL_PROC_QUERY_SHARE_BY_VNA_NP_SEG,\
        static_cast<void*> (&outVInfo));
        
    if( ret != VNET_DB_SUCCESS )
    {
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneSegmentPool::QuerySegmentIdShareByVnaNp(%s,%s,%d) IProc->Handle failed(%d) \n",vna_uuid,np_uuid,SegNum,ret);
        }
        return ret;
    }
        
    return 0;
}

int32 CDBOperateNetplaneSegmentPool::QuerySegIdIplistPrivateByVnaNpSeg(const char* vna_uuid, const char* np_uuid,int32 SegNum,vector<CDbVnaNpSegmentId> & outVInfo)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneSegmentPool::QuerySegmentIdPrivateByVnaNp IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    if( NULL == vna_uuid || NULL == np_uuid )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneSegmentPool::QuerySegmentIdPrivateByVnaNp param is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // call procedure 
    vector<CADOParameter>  vInParam;
    CADOParameter inVnaUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)vna_uuid);
    CADOParameter inNpUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)np_uuid); 
    CADOParameter inSeg(CADOParameter::paramInt, CADOParameter::paramInput, SegNum);        
    vInParam.push_back(inVnaUuid);
    vInParam.push_back(inNpUuid);
    vInParam.push_back(inSeg);        
    
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_CURSOR,\
        s_astrNetplaneSegmentPoolProc[EN_NETPLANE_SEGMENTPOOL_PROC_QUERY_PRIVATE_BY_VNA_NP_SEG],\
        vInParam, \
        this,\
        EN_NETPLANE_SEGMENTPOOL_PROC_QUERY_PRIVATE_BY_VNA_NP_SEG,\
        static_cast<void*> (&outVInfo));
        
    if( ret != VNET_DB_SUCCESS )
    {
        if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
        {
            VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneSegmentPool::QuerySegmentIdPrivateByVnaNp(%s,%s,%d) IProc->Handle failed(%d) \n",vna_uuid,np_uuid,SegNum,ret);
        }
        return ret;
    }

    return 0;
}

int32 CDBOperateNetplaneSegmentPool::QuerySegIdIplistCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneSegmentPool::QueryCallback prs is NULL.\n");
        //VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }

    vector<CDbVnaNpSegmentId> *pvOut;
    pvOut = static_cast<vector<CDbVnaNpSegmentId>*>(nil);

    if( pvOut == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneSegmentPool::QueryCallback pvOut is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    CDbVnaNpSegmentId info;

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 5);
       
    info.SetVnaUuid(prs->GetFieldValue(0));
    info.SetNpUuid(prs->GetFieldValue(1));

    int32 value = 0;
    if( false == prs->GetFieldValue(2,value) )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneSegmentPool::QueryCallback GetFieldValue failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    info.SetSegmentNum(value);

    info.SetIp(prs->GetFieldValue(3));
    info.SetMask(prs->GetFieldValue(4));
    
    (*pvOut).push_back(info);

    return 0;
}

int32 CDBOperateNetplaneSegmentPool::CheckAdd(CDbNetplaneSegmentPool & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneSegmentPool::CheckAdd IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }
    
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUuid().c_str());    
    CADOParameter inSegmentNum(CADOParameter::paramInt, CADOParameter::paramInput, info.GetSegmentNum());
    
    vInParam.push_back(inUuid);   
    vInParam.push_back(inSegmentNum);  
  
    // ret struct 
    int32 proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrNetplaneSegmentPoolProc[EN_NETPLANE_SEGMENTPOOL_PROC_CHECK_ADD],
        vInParam, 
        this,
        EN_NETPLANE_SEGMENTPOOL_PROC_CHECK_ADD,
        static_cast<void*> (&proc_ret));
    
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneSegmentPool::CheckAdd IProc->Handle failed(%d).\n", ret);
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneSegmentPool::CheckAdd proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }

    return VNET_DB_SUCCESS;
}

int32 CDBOperateNetplaneSegmentPool::Add(CDbNetplaneSegmentPool & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneSegmentPool::Add IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }
    
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUuid().c_str());    
    CADOParameter inSegmentNum(CADOParameter::paramInt, CADOParameter::paramInput, info.GetSegmentNum());
    
    vInParam.push_back(inUuid);   
    vInParam.push_back(inSegmentNum); 
  
    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrNetplaneSegmentPoolProc[EN_NETPLANE_SEGMENTPOOL_PROC_ADD],
        vInParam, 
        this,
        EN_NETPLANE_SEGMENTPOOL_PROC_ADD,
        static_cast<void*> (&proc_ret));
    
    if( ret != VNET_DB_SUCCESS )
    {        
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneSegmentPool::Add IProc->Handle failed(%d).\n", ret);
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneSegmentPool::Add proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }

    return VNET_DB_SUCCESS;
}

int32 CDBOperateNetplaneSegmentPool::CheckDel(CDbNetplaneSegmentPool & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneSegmentPool::CheckDel IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }
    
    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;   
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUuid().c_str());    
    CADOParameter inSegmentNum(CADOParameter::paramInt, CADOParameter::paramInput, info.GetSegmentNum());
    
    vInParam.push_back(inUuid);   
    vInParam.push_back(inSegmentNum);  
  
    // ret struct 
    int32 proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrNetplaneSegmentPoolProc[EN_NETPLANE_SEGMENTPOOL_PROC_CHECK_DEL],
        vInParam, 
        this,
        EN_NETPLANE_SEGMENTPOOL_PROC_CHECK_DEL,
        static_cast<void*> (&proc_ret));
    
    if( ret != VNET_DB_SUCCESS )
    {        
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneSegmentPool::CheckDel IProc->Handle failed(%d).\n", ret);
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
        VNET_LOG(VNET_LOG_ERROR,"CDBOperateNetplaneSegmentPool::CheckDel proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret));
        return GET_DB_ERROR_PROC(proc_ret);
    }

    return VNET_DB_SUCCESS;
}

int32 CDBOperateNetplaneSegmentPool::Del(CDbNetplaneSegmentPool & info)
{
    if( NULL == m_pIProc )
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneSegmentPool::Del IProc is NULL.\n");
        return VNET_DB_ERROR_IPROCEDURE_IS_NULL;
    }

    // call procedure 
    // inParam
    vector<CADOParameter> vInParam;    
    CADOParameter inUuid(CADOParameter::paramText, CADOParameter::paramInput, (char *)info.GetUuid().c_str());    
    CADOParameter inSegmentNum(CADOParameter::paramInt, CADOParameter::paramInput, info.GetSegmentNum());
    
    vInParam.push_back(inUuid);   
    vInParam.push_back(inSegmentNum); 

    // ret struct 
    CDbProcOperateResult proc_ret; 
    int32 ret = m_pIProc->Handle(DB_PROC_RTN_RECORD,
        s_astrNetplaneSegmentPoolProc[EN_NETPLANE_SEGMENTPOOL_PROC_DEL],
        vInParam, 
        this,
        EN_NETPLANE_SEGMENTPOOL_PROC_DEL,
        static_cast<void*> (&proc_ret));
    if( ret != VNET_DB_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneSegmentPool::Del IProc->Handle failed. ret : %d.\n", ret);
        return ret;
    }
    
    if( proc_ret.GetRet() != VNET_DB_PROCEDURE_RTN_SUCCESS )
    {
        if(DbgGetDBPrint())
        {
            info.DbgShow();
        }
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneSegmentPool::Del proc return failed. ret : %d.\n", GET_DB_ERROR_PROC(proc_ret.GetRet()));
        return GET_DB_ERROR_PROC(proc_ret.GetRet());
    }
    
    return 0;
}

int32 CDBOperateNetplaneSegmentPool::OperateCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneSegmentPool::OperateCallback param is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    
    CDbProcOperateResult *proc_ret = NULL;     
    proc_ret = static_cast<CDbProcOperateResult *>(nil);

    if( proc_ret == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneSegmentPool::OperateCallback proc_ret is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 3);    

    int32 nRet = 0;
    if( false == prs->GetFieldValue(0, nRet))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneSegmentPool::OperateCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetRet(nRet);

    int64 oid = 0;
    if( false == prs->GetFieldValue(1, oid))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneSegmentPool::OperateCallback GetFieldValue(1) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret).SetOid(oid);
    
    (*proc_ret).SetUuid(prs->GetFieldValue(2)); 

    return 0;
}

int32 CDBOperateNetplaneSegmentPool::CheckOperateCallback(CADORecordset * prs,void * nil)
{
    if( NULL == prs)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneSegmentPool::CheckOperateCallback param is NULL.\n");
        VNET_ASSERT(0);
        return VNET_DB_ERROR_PROC_ADORS_IS_NULL;
    }
    
    int32 *proc_ret = NULL;     
    proc_ret = static_cast<int32 *>(nil);

    if( proc_ret == NULL)
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneSegmentPool::CheckOperateCallback proc_ret is NULL.\n");
        return VNET_DB_ERROR_PARAM_INVALID;
    }

    // 和存贮过程对应  
    VNET_ASSERT(prs->GetFieldCount() == 1);    

    int32 nRet = 0;
    if( false == prs->GetFieldValue(0, nRet))
    {
        VNET_LOG(VNET_LOG_ERROR, "CDBOperateNetplaneSegmentPool::CheckOperateCallback GetFieldValue(0) failed.\n");
        return VNET_DB_ERROR_GET_ADORS_FIELD_FAILED;
    }
    (*proc_ret) = nRet;

    return 0;
}


void CDBOperateNetplaneSegmentPool::DbgShow()
{
#if 0
    cout << "--------------------------Port Info------------------------" << endl;
    // query summary
    vector<CDbNetplaneSegmentPool> vInfo;
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
    vector<CDbNetplaneSegmentPool>::iterator it = vInfo.begin();
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

void db_query_np_segmentpool(const char* uuid)
{
    if( NULL == uuid )
    {
        cout << "uuid is NULL" << endl;
        return;
    }
    
    CDBOperateNetplaneSegmentPool * pOper = GetDbINetplaneSegmentPool();
    if( NULL == pOper )
    {
        cout << "CDBOperateNetplaneSegmentPool is NULL" << endl;
        return ;
    }

    vector<CDbNetplaneSegmentPool> outVInfo;
    int ret = pOper->Query(uuid,outVInfo);
    if( ret != 0 )
    {
        cout << "Query failed." << ret << endl;
        return;
    }
    cout << "Query success." << endl;
    vector<CDbNetplaneSegmentPool>::iterator it = outVInfo.begin();
    for(; it != outVInfo.end(); ++it)
    {
        cout << "------------" << endl;
        (*it).DbgShow();
    }    
}
DEFINE_DEBUG_FUNC(db_query_np_segmentpool);

void db_query_np_priv_net_segment_isolate(const char* uuid)
{
    if( NULL == uuid )
    {
        cout << "uuid is NULL" << endl;
        return ;
    }
    
    CDBOperateNetplaneSegmentPool * pOper = GetDbINetplaneSegmentPool();
    if( NULL == pOper )
    {
        cout << "CDBOperateNetplaneSegmentPool is NULL" << endl;
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
DEFINE_DEBUG_FUNC(db_query_np_priv_net_segment_isolate);


void db_query_np_share_net_segment(const char* uuid)
{
    if( NULL == uuid )
    {
        cout << "uuid is NULL" << endl;
        return ;
    }
    
    CDBOperateNetplaneSegmentPool * pOper = GetDbINetplaneSegmentPool();
    if( NULL == pOper )
    {
        cout << "CDBOperateNetplaneSegmentPool is NULL" << endl;
        return ;
    }

    vector<int32> outVInfo;
    int ret = pOper->QueryShareNetSegment(uuid,outVInfo);
    if( ret != 0 )
    {
        cout << "Query failed." << ret << endl;
        return ;
    }
    cout << "Query success." << endl;
    vector<int32>::iterator it = outVInfo.begin();
    cout << "------Segment_list------" << endl;
    for(; it != outVInfo.end(); ++it)
    {
        cout << *it <<" ";
    }    
    cout << endl;
    return ;    
}
DEFINE_DEBUG_FUNC(db_query_np_share_net_segment);




void db_check_add_np_segmentpool(char* uuid, int32 segment)
{
    if( NULL == uuid  )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateNetplaneSegmentPool * pOper = GetDbINetplaneSegmentPool();
    if( NULL == pOper )
    {
        cout << "CDBOperateNetplaneSegmentPool is NULL" << endl;
        return ;
    }

    CDbNetplaneSegmentPool info;
    info.SetUuid(uuid);
    //info.SetType(type);
    info.SetSegmentNum(segment);
    

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
DEFINE_DEBUG_FUNC(db_check_add_np_segmentpool);

void db_add_np_segmentpool(char* uuid, int32 segment )
{
    if( NULL == uuid  )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateNetplaneSegmentPool * pOper = GetDbINetplaneSegmentPool();
    if( NULL == pOper )
    {
        cout << "CDBOperateNetplaneSegmentPool is NULL" << endl;
        return ;
    }

    CDbNetplaneSegmentPool info;
    info.SetUuid(uuid);
    //info.SetType(type);
    info.SetSegmentNum(segment);

    
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
DEFINE_DEBUG_FUNC(db_add_np_segmentpool);

void db_del_np_segmentpool(char * uuid, int32 segment  )
{
    if( NULL == uuid  )
    {
        cout << "Param is NULL" << endl;
        return;
    }
    CDBOperateNetplaneSegmentPool * pOper = GetDbINetplaneSegmentPool();
    if( NULL == pOper )
    {
        cout << "CDBOperateNetplaneSegmentPool is NULL" << endl;
        return ;
    }

    CDbNetplaneSegmentPool info;
    info.SetUuid(uuid);
    //info.SetType(type);
    info.SetSegmentNum(segment);
   

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
DEFINE_DEBUG_FUNC(db_del_np_segmentpool);

void db_query_seg_by_vna_np_seg(const char* vna_uuid,const char* np_uuid, int32 segNum)
{
    if( NULL == vna_uuid || NULL == np_uuid )
    {
        cout << "uuid is NULL" << endl;
        return;
    }
    
    CDBOperateNetplaneSegmentPool * pOper = GetDbINetplaneSegmentPool();
    if( NULL == pOper )
    {
        cout << "CDBOperateNetplaneSegmentPool is NULL" << endl;
        return ;
    }

    vector<CDbVnaNpSegmentId> outVInfo;
    int ret = pOper->QuerySegIdIplistByVnaNpSeg(vna_uuid,np_uuid,segNum,outVInfo);
    if( ret != 0 )
    {
        cout << "Query failed." << ret << endl;
        return;
    }
    cout << "Query success." << endl;
    vector<CDbVnaNpSegmentId>::iterator it = outVInfo.begin();
    for(; it != outVInfo.end(); ++it)
    {
        cout << "------------" << endl;
        (*it).DbgShow();
    }    
}
DEFINE_DEBUG_FUNC(db_query_seg_by_vna_np_seg);


}




