/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：image_pool.h
* 文件标识：见配置管理计划书
* 内容摘要：image类的定义文件
* 当前版本：1.0
* 作    者：Bob
* 完成日期：2011年7月26日
*
* 修改记录1：
*    修改日期：2011/7/26
*    版 本 号：V1.0
*    修 改 人：Bob
*    修改内容：创建
*******************************************************************************/

#ifndef PROJECT_MANGER_PROJECT_POOL_H        
#define PROJECT_MANGER_PROJECT_POOL_H

#include "project.h"
#include "pool_sql.h"
#include "project_api.h"


namespace zte_tecs
{

/**
@brief 功能描述: 映像资源池的实体类\n
@li @b 其它说明: 无
*/
class ProjectPool : public SqlCallbackable
{

/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    static ProjectPool* GetInstance()
    {
        SkyAssert(NULL != _instance);
        return _instance;
    };

    static ProjectPool* CreateInstance(SqlDB *db)
    {
        if(NULL == _instance)
        {
            _instance = new ProjectPool(db); 
        }
        
        return _instance;
    };
    
    static void Lock()
    {
        pthread_mutex_lock(&_mutex);
    };

    static void UnLock()
    {
        pthread_mutex_unlock(&_mutex);
    };
    
    STATUS Init(const bool& isReadOnly);

     ~ProjectPool(){};

    /**
     *  Function to allocate a new Project object
     *    @param uid the user id of the image's owner
     *    @param stemplate template associated with the image
     *    @param oid the id assigned to the Project
     *    @return the oid assigned to the object,
     *                  -1 in case of failure
     *                  -2 in case of template parse failure
     */

    STATUS   Allocate (Project &project);
    /**
     *  Function to get a Project from the pool, if the object is not in memory
     *  it is loaded from the DB
     *    @param oid Project unique id
     *    @param lock locks the Project mutex
     *    @return a pointer to the Project, 0 if the Project could not be loaded
     */

    STATUS GetProject(int64 oid, Project & project);
        
    int SelectCallback(void * nil, SqlColumn * columns);

    /* 通过工程名和用户名查询工程id */
    int64 GetProjectIdByName(int64 uid, const string &name);

 
    /**
    *  function to get an Project from the pool using the name and uid
    *  @param name of the project
    *  @param uid of the project
    *  @return SUCCUSS if the project exist;ERROR if not
    */
    STATUS GetProject(int64 uid, const string &name, Project & project);

    /** Update a particular Project
    *  @param image pointer to Project
    *  @return 0 on success
    */
    int Update(const Project &prj);

    /** Drops an image from the DB, the image mutex MUST BE locked
    *  @param image pointer to Project
    *  @return 0 on success
    */
    int Drop(Project &pro);

    int Show(vector<ApiProjectInfo> &vec_pro_info, const string& where);
    
    int ShowProjectVm(vector<ApiProjectVmInfo> &vec_vm_info, const string& where);

    int64 Count(const string &where);

    int GetVidsByWhere(const string& where, vector<int64>& vids);

    int ShowProjectStatisticsStaticData(vector<ApiProjectStatisticsStaticData> &vec_static_data,const string& where);
    int GetProjectCluster(string& cluster,const string& where);

/*******************************************************************************
* 2. protected section
*******************************************************************************/
protected:
    enum ColNames
    {
        OID              = 0,    /* Project identifier (IID)      */
        NAME             = 1,    /* Project name                  */
        DESCRIPTION      = 2,    /* Time of create                   */
        CREATE_TIME      = 3,
        UID              = 4,    /* Project owner id              */
        LIMIT            = 5
    };

    enum ViewColNames
    {
        VIEW_OID              = 0,    /* Project identifier (IID)      */
        VIEW_NAME             = 1,    /* Project name                  */
        VIEW_DESCRIPTION      = 2,    /* Time of create                */
        VIEW_USER_NAME        = 3,
        VIEW_LIMIT            = 4
    };

    enum ViewProjectVmColNames
    {
        VIEW_VM_OID        = 0,    /* VID       */
        VIEW_VM_NAME       = 1,    /* VM name   */
        VIEW_VM_LIMIT      = 2
    };

    enum ViewProjectStaticData
    {
        VIEW_SD_OID        = 0,    /* Project identifier (IID)       */
        VIEW_SD_NAME       = 1,    /* Project name    */    
        VIEW_SD_VM_SUM     = 2,    /* VM sum    */
        VIEW_SD_TCU_SUM    = 3,    /* tcu sum    */
        VIEW_SD_MEM_SUM    = 4,    /* memory sum    */
        VIEW_SD_NIC_SUM    = 5,     /* nic sum    */
        VIEW_SD_STROAGE_SHARE   = 6,    /* storage share    */
        VIEW_SD_STROAGE_LOCAL   = 7,    /* storage local    */
        VIEW_SD_LIMIT      = 8    
    };	
    int ShowCallback(void * arg, SqlColumn * columns);

    int ShowProjectVmCallback(void * arg, SqlColumn * columns);

    int ShowProjectStatisticsStaticDataCallback(void * arg, SqlColumn * columns);
/*******************************************************************************
* 3. private section
*******************************************************************************/
private:
    static ProjectPool *_instance;
    static int64            _lastOID;
    static pthread_mutex_t  _mutex;
    
    static const char *     _table_project;
    static const char *     _col_names_project;
    static const char *     _view_project;
    static const char *     _col_names_view_project;

    static const char *     _view_vmcfg_with_project;
    static const char *     _col_names_view_vmcfg_with_project;

    static const char *     _view_static_data_with_project;
    static const char *     _col_names_static_data_with_project;

    static const char *    _view_cluster_with_project;
    ProjectPool(SqlDB *pDb):SqlCallbackable(pDb){ };

    DISALLOW_COPY_AND_ASSIGN(ProjectPool); 

};

}
#endif /* PROJECT_MANGER_PROJECT_POOL_H */

