
/******************************************************************************
* Copyright (c) 2013，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：vnet_db_netplane.h
* 文件标识：
* 内容摘要：CVNetDb类的定义文件
* 当前版本：1.0
* 作    者：
* 完成日期：2013年1月28日
*
* 修改记录1：
*     修改日期：2013/1/28
*     版 本 号：V1.0
*     修 改 人：gong.xiefeng
*     修改内容：创建
*
******************************************************************************/
#include "vnet_db_in.h"
#include "vnet_db_proc.h"
#include <pthread.h>

extern CADODatabase *GetAdoDB();
extern void FreeAdoDB();
namespace zte_tecs
{

extern int32 DbgGetDBPrint();
extern CADODatabase *g_pDb;

CADODatabase * GetVnmDb()
{
    CADODatabase * pAdo = GetAdoDB();    
//    VNET_ASSERT(pAdo != NULL);
    if( pAdo == NULL)
    {
        cout << "Vnm no db resouce." << endl;
    }
    return pAdo;
}

void FreeVnmDb()
{
    // FreeAdoDB();
}

int32 CVNetDbProcedurePostgresql::Handle(DB_PROC_RTN_TYPES type,const char * fun_name,
        vector<CADOParameter> & inParam,CVNetDbIProcedureCallback*  cb, int32 callback_type,void *cb_arg)
{
    //cout << "CVNetDbProcedurePostgresql::Handle" << endl;
    switch(type)
    {
        case DB_PROC_RTN_RECORD:
        {
            return HandleRecord(fun_name,inParam,cb,callback_type,cb_arg);
        }
        break;
        case DB_PROC_RTN_CURSOR:
        {
            return HandleCursor(fun_name,inParam,cb,callback_type,cb_arg);
        }
        break;
        default: 
        {
            return VNET_DB_ERROR_PROCTYPE_IS_INVALID;
        }
        break;
    }
    return 0;
}

int32 CVNetDbProcedurePostgresql::HandleRecord(const char * fun_name,
        vector<CADOParameter> & inParam, CVNetDbIProcedureCallback* cb, int32 callback_type,void *cb_arg)
{ 
    int32 ret = VNET_DB_SUCCESS;     
    int ret_ado = SQLDB_OK;
    CADORecordset* prs = NULL;

    if( NULL == GetVnmDb() )
    {
        VNET_LOG(VNET_LOG_ERROR,"CVNetDbProcedurePostgresql::HandleRecord no db handle resource.\n");
        return VNET_DB_ERROR_NO_DB_HANDLE_RESOURCE;
    }
    
    CADOCommand adoCmd(GetVnmDb(), fun_name);     
   
    vector<CADOParameter>::iterator it = inParam.begin();
    for(; it != inParam.end(); ++it)
    {
        adoCmd.AddParameter( &(*it));
    }

    if( NULL == cb)
    {
        VNET_LOG(VNET_LOG_ERROR,"CVNetDbProcedurePostgresql::HandleRecord proc callback is NULL.\n");
        ret= VNET_DB_ERROR_PROCCALLBACK_IS_NULL;
        goto free_db_resouce;
    }
    
    prs = new CADORecordset(GetVnmDb());        
    if( NULL == prs )
    {
        VNET_LOG(VNET_LOG_ERROR,"CVNetDbProcedurePostgresql::HandleRecord alloc memory failed.\n");
        ret = VNET_DB_ERROR_ALLOC_MEM_FAILED;
        goto free_db_resouce;
    }

    ret_ado = prs->Execute(&adoCmd);
    if( SQLDB_OK == ret_ado)    
    {   
        if( cb != NULL )
        {
            ret = cb->DbProcCallback(callback_type,prs,cb_arg);
            if( ret != VNET_DB_SUCCESS )
            {
                if( DbgGetDBPrint() || !VNET_DB_IS_ITEM_NO_EXIST(ret) )
                {
                    VNET_LOG(VNET_LOG_ERROR,"CVNetDbProcedurePostgresql::HandleRecord %s cb->DbProcCallback failed(%d).\n",fun_name,ret);
                }
                goto free_resource;
            }
        }
    }    
    else
    {
        ret = VNET_DB_ERROR_EXECUTE_FAILED;
        VNET_LOG(VNET_LOG_ERROR, "CVNetDbProcedurePostgresql::HandleRecord %s prs->Execute failed.\n",fun_name);
    }
    
free_resource:    
    delete prs;   
free_db_resouce:    
    FreeVnmDb();
    return ret;
}

int32 CVNetDbProcedurePostgresql::HandleCursor(const char * proc_name,
         vector<CADOParameter> & inParam, CVNetDbIProcedureCallback* cb, int32 callback_type,void *cb_arg)
{ 
    int32 ret = VNET_DB_SUCCESS;     
    int ret_ado = SQLDB_OK;
    CADORecordset* prs = NULL;
    
    if( NULL == GetVnmDb() )
    {
        VNET_LOG(VNET_LOG_ERROR,"CVNetDbProcedurePostgresql::HandleCursor no db handle resource.\n");
        return VNET_DB_ERROR_NO_DB_HANDLE_RESOURCE;
    }
    
    CADOCommand adoCmd(GetVnmDb(), proc_name, CADOCommand::typeCmdStoredProcCursor);    
    
    vector<CADOParameter>::iterator it = inParam.begin();
    for(; it != inParam.end(); ++it)
    {
        adoCmd.AddParameter( &(*it));
    }

    if( NULL == cb)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetDbProcedurePostgresql::HandleCursor proc callback is NULL.\n");
        ret= VNET_DB_ERROR_PROCCALLBACK_IS_NULL;
        goto free_db_resouce;
    } 

    prs = new CADORecordset(GetVnmDb());      
    if( NULL == prs )
    {
        VNET_LOG(VNET_LOG_ERROR,"CVNetDbProcedurePostgresql::HandleCursor alloc memory failed.\n");
        ret = VNET_DB_ERROR_ALLOC_MEM_FAILED;
        goto free_db_resouce;
    }

    ret_ado = prs->Execute(&adoCmd);
    if(SQLDB_OK == ret_ado )    
    {                                    
        while ( !prs->IsEOF() )        
        {        
            if( cb != NULL )
            {
                ret = cb->DbProcCallback(callback_type,prs,cb_arg);//cb(prs4,cb_arg);
                if( ret != VNET_DB_SUCCESS )
                {
                    VNET_LOG(VNET_LOG_ERROR,"CVNetDbProcedurePostgresql::HandleCursor %s cb->DbProcCallback failed(%d).\n",proc_name,ret);
                    goto free_resource;
                }
            }
            prs->MoveNext();           
        }    
    }    
    else if (SQLDB_RESULT_EMPTY == ret_ado)
    {
        // 也是ok的，没有合适的游标打开 
    }
    else    
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetDbProcedurePostgresql::HandleCursor %s prs->Execute failed.\n",proc_name);
        ret = VNET_DB_ERROR_EXECUTE_FAILED;
    }
    
free_resource:      
    delete prs;  
free_db_resouce:      
    FreeVnmDb();
    
    return ret;
}

CVNetDbProcedureFactory * CVNetDbProcedureFactory::s_instance = NULL;
CVNetDbProcedureFactory::CVNetDbProcedureFactory()
{
    m_pDbIProc = NULL;
}

CVNetDbProcedureFactory::~CVNetDbProcedureFactory()
{
    if( m_pDbIProc )
    {
        delete m_pDbIProc;
    }
}

int32 CVNetDbProcedureFactory::Init()
{
    if ( NULL == m_pDbIProc)
    {
        m_pDbIProc = new CVNetDbProcedurePostgresql();
        if( m_pDbIProc == NULL)
        {
            VNET_LOG(VNET_LOG_ERROR, "CVNetDbProcedureFactory::Init alloc memory failed.\n");
            return VNET_DB_ERROR_ALLOC_MEM_FAILED;
        }
    }
    return VNET_DB_SUCCESS;
}

} // namespace zte_tecs


