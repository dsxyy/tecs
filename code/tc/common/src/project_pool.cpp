/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：project_pool.cpp
* 文件标识： 
* 内容摘要：Project资源池类的实现文件
* 当前版本：1.0 
* 作    者：Bob
* 完成日期：2011年7月28日
*
* 修改记录1：
*    修改日期：2011/7/28
*    版 本 号：V1.0
*    修 改 人：Bob
*    修改内容：创建
*******************************************************************************/
#include "project_pool.h"
#include "pub.h"
#include "log_agent.h"
#include "api_const.h"


namespace zte_tecs
{
          
int64        ProjectPool::_lastOID  = INVALID_OID;
ProjectPool *ProjectPool::_instance = NULL;
pthread_mutex_t    ProjectPool::_mutex;

const char * ProjectPool::_table_project = "project_pool";
const char * ProjectPool::_col_names_project = 
                                              "oid, "
                                              "name, "
                                              "description, "
                                              "create_time, "
                                              "uid";
                                              

const char * ProjectPool::_view_project = "view_project";
const char * ProjectPool::_col_names_view_project = 
                                              "oid, "
                                              "name, "
                                              "description, "
                                              "user_name ";

const char * ProjectPool::_view_vmcfg_with_project = "view_vmcfg_with_project";
const char * ProjectPool::_col_names_view_vmcfg_with_project = 
                                              "oid, "
                                              "name ";

const char * ProjectPool::_view_static_data_with_project = "view_project_statistics_static_sum";
const char * ProjectPool::_col_names_static_data_with_project = 
                                              "project_id, "
                                              "project_name, "
                                              "vm_sum,"
                                              "tcu_sum,"
                                              "memory_sum,"
                                              "nic_number_sum,"
                                              "storage_share_sum,"
                                              "storage_local_sum";;
  
const char * ProjectPool::_view_cluster_with_project = "view_project_with_cluster";
 
STATUS ProjectPool::Init(const bool& isReadOnly)
{
    if(false == isReadOnly)
    {
        int64    tmp_oid = 0;
        string   where   = " 1=1 ";
        string   column  = " MAX(oid) ";
        int             rc;
    
        // 1.获取最大_oid
        SqlCallbackable sqlcb(_db);
        rc = sqlcb.SelectColumn( _table_project, column, where, tmp_oid);
        if((SQLDB_OK != rc) &&(SQLDB_RESULT_EMPTY != rc))
        { 
                OutPut(SYS_DEBUG, "*** Select record from project_pool failed ***\n");
                return ERROR_DB_SELECT_FAIL;
        }
        
        _lastOID = tmp_oid;
    
        // 初始化互斥锁
        pthread_mutex_init(&_mutex,0);
    }
    
    return SUCCESS;
}

/******************************************************************************/
int ProjectPool::Allocate(Project & project)
{
    int rc = -1;
    
    Lock();
    _lastOID++;
    project._oid = _lastOID;
    UnLock();
    
    SqlCommand sql(_db,_table_project);
    sql.Add("oid",project._oid);
    sql.Add("name",project._name);
    sql.Add("description",project._description);
    sql.Add("create_time",project._create_time);
    sql.Add("uid",project._uid);
    
    rc = sql.Insert();
    if (0 != rc)
    {
        SkyAssert(false);
        return ERROR_DB_INSERT_FAIL;
    }

    return SUCCESS;
}

/******************************************************************************/
STATUS ProjectPool::GetProject(int64 oid, Project & project)
{
    ostringstream   oss;
    int             rc = -1;
    int64           tmp_oid = INVALID_OID;

    SetCallback(static_cast<Callbackable::Callback>(&ProjectPool::SelectCallback),static_cast<void *>(&project));

    oss << "SELECT " << _col_names_project << " FROM " << _table_project << " WHERE oid = " << oid;

    tmp_oid = oid;
    project._oid  = INVALID_OID;

    rc = _db->Execute(oss, this);
    UnsetCallback();
    
    if (SQLDB_RESULT_EMPTY == rc)
    {
        //数据库为空        
        return  ERROR;
    }

    if (SQLDB_OK != rc)
    {
        //数据库系统出错
        SkyAssert(false);
        return ERROR;
    }
    
    if (project._oid != tmp_oid  )
    {
        //数据库系统没问题，但是没找到记录
        return ERROR;
    }
    
    return SUCCESS; 
};


/******************************************************************************/
STATUS ProjectPool::GetProject(int64 uid, const string &name, Project & project)
{
    ostringstream   oss;
    int             rc;

    SetCallback(static_cast<Callbackable::Callback>(&ProjectPool::SelectCallback),static_cast<void *>(&project));

    oss << "SELECT " << _col_names_project << " FROM " << _table_project << " WHERE uid =  "<< uid << " AND name= '"<<name << "'" ;

    rc = _db->Execute(oss, this);
    UnsetCallback();
    
    if (SQLDB_RESULT_EMPTY == rc)
    {
       return  SQLDB_RESULT_EMPTY;
    }
    
    if ( (SQLDB_OK != rc) && (SQLDB_RESULT_EMPTY != rc)  )
    {
        //数据库系统出错
        SkyAssert(false);
        return rc;
    }
    
    return SUCCESS;  
} 

/******************************************************************************/
int ProjectPool::SelectCallback(void * nil, SqlColumn * columns)
{
    if (NULL == columns ||          
        LIMIT != columns->get_column_num())    
    {        
        return -1;   
    }  
    
    Project* project = static_cast<Project*>(nil);
    
    columns->GetValue(OID, project->_oid);
    columns->GetValue(NAME, project->_name);
    columns->GetValue(DESCRIPTION, project->_description);
    columns->GetValue(CREATE_TIME, project->_create_time);
    columns->GetValue(UID, project->_uid);
       
    return 0;
}

/******************************************************************************/
int64  ProjectPool::GetProjectIdByName(int64 uid, const string &name)
{
    ostringstream   oss;
    int             rc;
    int64 temp_oid = INVALID_OID;
    
    oss << " uid = " << uid <<" AND name= '"<<name << "'" ;

    rc = SelectColumn(_table_project, "oid", oss.str(),temp_oid);
    if(0!=rc)
    {
        return INVALID_OID;
    }
    else
    {
        return temp_oid;
    }
    
} 

/******************************************************************************/
int ProjectPool::Update(const Project  &prj)
{
    SqlCommand sql(_db,_table_project);
    sql.Add("oid",prj._oid);
    sql.Add("name",prj._name);
    sql.Add("description",prj._description);
    sql.Add("create_time",prj._create_time);
    sql.Add("uid",prj._uid);

    return sql.Update(" WHERE oid = " +  to_string<int64>(prj._oid,dec));
};

/******************************************************************************/
int ProjectPool::Drop(Project  &prj)
{
    SqlCommand sql(_db, _table_project);

    return sql.Delete(" WHERE oid = " + to_string(prj._oid, dec));

};


/******************************************************************************/
int ProjectPool::Show(vector<ApiProjectInfo> &vec_pro_info, const string &where)
{
    int             rc;
    ostringstream   cmd;

   SetCallback(static_cast<Callbackable::Callback>(&ProjectPool::ShowCallback),
               static_cast<void *>(&vec_pro_info));
         
   cmd << "SELECT "<< _col_names_view_project << " FROM "
        << _view_project;

    if (!where.empty())
    {
        cmd << " WHERE " << where;
    }

    rc = _db->Execute(cmd, this);

    UnsetCallback();

    if ((SQLDB_OK != rc)
         &&(SQLDB_RESULT_EMPTY != rc))
    {
        return -1;
    }

    return 0;
}

/******************************************************************************/
int ProjectPool::ShowCallback(void * arg, SqlColumn * columns)
{
    vector<ApiProjectInfo> *ptProRecord;

    ptProRecord = static_cast<vector<ApiProjectInfo> *>(arg);

    ApiProjectInfo  tInfo;
	
    if ( NULL == columns || 
         VIEW_LIMIT != columns->get_column_num())
    {
        return -1;
    }

    columns->GetValue(VIEW_OID, tInfo.id);   
    columns->GetValue(VIEW_NAME, tInfo.name );
    columns->GetValue(VIEW_DESCRIPTION, tInfo.des);
    columns->GetValue(VIEW_USER_NAME, tInfo.uname);

    ptProRecord->push_back(tInfo);

    return 0;
    
}


/******************************************************************************/  
int ProjectPool::ShowProjectVm(vector<ApiProjectVmInfo> &vec_vm_info, 
                                     const string& where)
{
    int             rc;
    ostringstream   cmd;

   SetCallback(static_cast<Callbackable::Callback>(&ProjectPool::ShowCallback),
               static_cast<void *>(&vec_vm_info));
         
   cmd << " SELECT " << _col_names_view_vmcfg_with_project \
       << " FROM "   << _view_vmcfg_with_project;

    if (!where.empty())
    {
        cmd << " WHERE " << where;
    }

    rc = _db->Execute(cmd, this);

    UnsetCallback();

    if ((SQLDB_OK != rc)
         &&(SQLDB_RESULT_EMPTY != rc))
    {
        return -1;
    }

    return 0;
}


/******************************************************************************/
int ProjectPool::ShowProjectVmCallback(void * arg, SqlColumn * columns)
{
    vector<ApiProjectVmInfo> *ptProRecord;

    ptProRecord = static_cast<vector<ApiProjectVmInfo> *>(arg);

    ApiProjectVmInfo  tInfo;
	
    if ((NULL == columns) || 
        (VIEW_VM_LIMIT != columns->get_column_num()))
    {
        return -1;
    }

    columns->GetValue(VIEW_VM_OID,  tInfo.vm_id);   
    columns->GetValue(VIEW_VM_NAME, tInfo.vm_name);

    ptProRecord->push_back(tInfo);

    return 0;
    
}


/******************************************************************************/  
int64 ProjectPool::Count(const string &where)
{
    ostringstream   sql;
    int64 count = 0;
    int rc;
    
    rc = SelectColumn(_table_project,"COUNT(*)",where,count);   

    if ((SQLDB_OK != rc )
        &&(SQLDB_RESULT_EMPTY!= rc))
    {
        return -1;
    }

    if(SQLDB_RESULT_EMPTY == rc)
    {
       return 0;
    }

    return count;
}
/******************************************************************************/
int ProjectPool::GetVidsByWhere(const string& where, vector<int64>& vids)
{
    int rc = SelectColumn(_view_vmcfg_with_project,"oid",where,vids);
    if ((SQLDB_OK != rc) && (SQLDB_RESULT_EMPTY != rc))
    {
        return ERROR_DB_SELECT_FAIL;
    }
    return SUCCESS;
}

/******************************************************************************/  
int ProjectPool::ShowProjectStatisticsStaticData(vector<ApiProjectStatisticsStaticData> &vec_static_data, 
                                                 const string& where)
{
    int             rc;
    ostringstream   cmd;

    SetCallback(static_cast<Callbackable::Callback>(&ProjectPool::ShowProjectStatisticsStaticDataCallback),
               static_cast<void *>(&vec_static_data));
         
   cmd << " SELECT " << _col_names_static_data_with_project \
       << " FROM "   << _view_static_data_with_project;

    if (!where.empty())
    {
        cmd << " WHERE " << where;
    }

    rc = _db->Execute(cmd, this);

    UnsetCallback();

    if ((SQLDB_OK != rc)
         &&(SQLDB_RESULT_EMPTY != rc))
    {
        return -1;
    }

    return 0;
}



/******************************************************************************/
int ProjectPool::ShowProjectStatisticsStaticDataCallback(void * arg, SqlColumn * columns)
{
    vector<ApiProjectStatisticsStaticData> *ptProRecord;

    ptProRecord = static_cast<vector<ApiProjectStatisticsStaticData> *>(arg);

    ApiProjectStatisticsStaticData  tInfo;

    if ((NULL == columns) || 
        (VIEW_SD_LIMIT != columns->get_column_num()))
    {
        return -1;
    }

    columns->GetValue(VIEW_SD_OID,  tInfo.id);   
    columns->GetValue(VIEW_SD_NAME, tInfo.name);
    columns->GetValue(VIEW_SD_VM_SUM,  tInfo.vm_sum);   
    columns->GetValue(VIEW_SD_TCU_SUM, tInfo.tcu_sum);
    columns->GetValue(VIEW_SD_MEM_SUM,  tInfo.memory_sum);   
    columns->GetValue(VIEW_SD_NIC_SUM, tInfo.nic_sum);
    columns->GetValue(VIEW_SD_STROAGE_SHARE, tInfo.storage_share);
    columns->GetValue(VIEW_SD_STROAGE_LOCAL, tInfo.storage_local);
    ptProRecord->push_back(tInfo);

    return 0;
    
}

/******************************************************************************/
int ProjectPool::GetProjectCluster(string& cluster,const string& where)
{
    int rc = SelectColumn(_view_cluster_with_project,"cluster_name",where,cluster);
    if ((SQLDB_OK != rc) && (SQLDB_RESULT_EMPTY != rc))
    {
        return ERROR_DB_SELECT_FAIL;
    }
    return SUCCESS;
}
}

	


