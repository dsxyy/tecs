/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：project_operate.cpp
* 文件标识：见配置管理计划书
* 内容摘要：
* 当前版本：1.0 
* 作    者：Bob
* 完成日期：2012年11月15日
*
* 修改记录1：
*    修改日期：2012/11/15
*    版 本 号：V1.0
*    修 改 人：Bob
*    修改内容：创建
*******************************************************************************/
#include "project_operate.h"
#include "authrequest.h"
#include "authmanager.h"
#include "project_pool.h"
#include "license_common.h"
#include "vmcfg_pool.h"

namespace zte_tecs
{

ProjectOperate *ProjectOperate::_op_inst= NULL;
ProjectPool    *ProjectOperate::_ppool = NULL;

/******************************************************************************/
STATUS ProjectOperate::DealModify(const ApiProjectModifyData  &req, 
                                      int64          uid, 
                                      ostringstream &err_oss)
{

    /* 搜索project pool中是否已有该project? */
    Project  project;
    STATUS   tRet = _ppool->GetProject(uid, req.name,project);
    if( SUCCESS != tRet)
    {
        err_oss<<"Error: user ("<<uid << 
                 ") does not has the project of name (" << req.name <<") !";
        return ERR_PROJECT_NOT_EXIST;
    }

    /* 授权 */
    if (Authorize(uid, &project, AuthRequest::MANAGE) == false)
    {
        err_oss<<"Authorize Fail";
        return ERR_PROJECT_AUTH_FAIL;
    }
    
    project.set_description(req.new_des);

    /* 更新数据库 */
    int rc = _ppool->Update(project);
    if ( rc < 0 )
    {
        err_oss<<"Error trying to modify project ("<<req.name<< ") ." <<endl;
        return ERR_PROJECT_UPDATA_FAIL;
    }

    return SUCCESS;

}
                      

/******************************************************************************/
STATUS ProjectOperate::DealDelete(int64  op_uid, 
                                      int64  owner, 
                                      const string &project_name, 
                                      ostringstream &err_oss)
{
    Project        project;
    string         where;
    vector<int64>  vids;
    VmCfgPool     *vcpool = NULL;
    STATUS         tRet =  ERROR;
    int rc = -1;
    
    /* 搜索project pool中是否已有该project? */
    tRet = _ppool->GetProject(owner, project_name, project);
    if( SUCCESS != tRet)
    {
        err_oss<<"Error: user ("<< owner << ") does not has the project of name (" <<
              project_name <<") !";

        return ERR_PROJECT_NOT_EXIST;
    }

    /* 授权 */
    if (Authorize(op_uid, &project, AuthRequest::DELETE) == false)
    {
        err_oss<<"Authorize Fail";
        return ERR_PROJECT_AUTH_FAIL;
    }

    vcpool = VmCfgPool::GetInstance();
    where = " project_id = " + to_string(project._oid, dec);
    
    tRet = vcpool->GetVids(vids, where);
    if ((SUCCESS != tRet)
         &&(ERROR_OBJECT_NOT_EXIST != tRet))
    {
        err_oss<<"Can not get vm info of Project ( "<< project_name <<" ).";
        return ERROR;
    }

    if(0 != vids.size())
    {
       err_oss<<"There are "<< vids.size()<<
            " vm in project ("<< project_name << ")" ;
       return ERR_RPOJECT_HAVE_VM;
    }

    rc = _ppool->Drop(project);
    if (SQLDB_OK != rc)
    {
        err_oss<<"Delete project ("<< project_name << ") from DB failed!" ;
        return ERR_PROJECT_DELETE_DB_FAIL;
    }

    err_oss<<"Delete project ("<< project_name << ") success!" ;
    return  SUCCESS;

}

/******************************************************************************/
STATUS ProjectOperate::DealQuery(const ApiProjectQuery  &req, 
                                     int64                    uid,
                                     int64                    appointed_uid,
                                     vector<ApiProjectInfo>  &vec_prj,
                                     int64                   &max_count,
                                     int64                   &next_index,
                                     ostringstream           &err_oss)
{
    ostringstream  where_string;
    ostringstream  where_string_count;
    int rc = -1;

    vector<ApiProjectInfo> info;

    switch (static_cast <ApiProjectQueryType>(req.type) )
    {
        case PROJECT_USER_CREATE: 
        {
            where_string << " UID = " << uid <<\
                          " order by oid limit " <<req.count <<\
                          " offset " << req.start_index ;
            where_string_count << " UID = " << uid;  
            break;
        }
    
        case PROJECT_USER_ALL:
        {
            /* 授权,这种写法，就只有管理员才有权限才能通过 */
            if (Authorize(uid, NULL,  AuthRequest::INFO) == false)
            {
                err_oss<<"Authorize Fail : " << 
                       "only adminstrator can query all user's project";
                return  ERR_PROJECT_AUTH_FAIL;
            }
    
            where_string << " 1=1 " <<\
                          " order by oid limit " <<req.count <<\
                          " offset " << req.start_index ;
            where_string_count << " 1=1 ";   
            break;
        }
        
        case PROJECT_USER_APPOINTED:
        {
            /* 授权,这种写法，就只有管理员才有权限才能通过 */
            if (Authorize(uid, NULL,  AuthRequest::INFO) == false)
            {
                err_oss<<"Authorize Fail : " << 
                       "only adminstrator can query other user's project";
                return  ERR_PROJECT_AUTH_FAIL;
            }
            where_string << "UID = " << appointed_uid <<\
                          " order by oid limit " << req.count <<\
                          " offset " << req.start_index ;
            where_string_count << " UID = " << appointed_uid ;
            break;
        }
        
        case PROJECT_APPOINTED:
        {
            where_string << " name = " <<"'"<< req.appointed_project <<"'"
                         << " and uid = " << uid;
            where_string_count << " name = " <<"'"<< req.appointed_project <<"'"
                         << " and uid = " << uid;             
            
            Project  project;
            rc=_ppool->GetProject(uid, req.appointed_project, project);
            if( SUCCESS != rc)
            {
                err_oss << "PROJECT ("<<req.appointed_project << ") not exist!";
                return  ERR_PROJECT_NOT_EXIST;
            }
            /* 授权 */
            if (Authorize(uid, &project, AuthRequest::INFO) == false)
            {
                err_oss<<"Authorize Fail";
                return  ERR_PROJECT_AUTH_FAIL;
            }
            break;
        }
        default:
        { 
            err_oss<<"Incorrect query type: "<< req.type <<endl;
            return ERR_PROJECT_QUERY_TYPE_ERR;
            break;
        }
    }

    max_count = _ppool->Count(where_string_count.str());
    if(-1 == max_count)
    {
        err_oss<<"Query failed !" ;
        return ERR_PROJECT_QUERY_FAIL;
    }
    
    // 调用project_pool的Show方法
    rc = _ppool->Show(info, where_string.str());

    if (rc != 0)
    {
        err_oss<<"Query failed !" ;
        return ERR_PROJECT_QUERY_FAIL;
    }
  
    int64  out_num = 0;
    /* 对下包起始位置进行设置 */
    if (PROJECT_APPOINTED == static_cast <ApiProjectQueryType>(req.type))
    {
        next_index = -1;
        out_num = info.size();
    }
    else
    {    //非尾页
        if (info.size() >= (uint64)(req.count ))
        {
            next_index = req.count + req.start_index;
            out_num = req.count;
        }
        else  //尾页
        {
            next_index = -1;
            out_num = info.size();
        }
    }

    /* 把查询的结果放到消息中 */
    for (int64 i = 0; i < out_num; i++)
    {
         vec_prj.push_back(info.at(i));
    }    
   
    return SUCCESS;

}

/******************************************************************************/
STATUS ProjectOperate::DealProjectVmQuery(const ApiProjectVmQuery  &req, 
                                                 int64                      op_uid,
                                                 int64                      appointed_uid,
                                                 vector<ApiProjectVmInfo>  &vec_prj,
                                                 int64                     &max_count,
                                                 int64                     &next_index,
                                                 ostringstream             &err_oss)
{
    ostringstream  where_string;
    ostringstream  where_string_count;
    int rc = -1;

    vector<ApiProjectVmInfo> info;

    //查别人名下的VM
    if (!req.appointed_user.empty())
    {
        /* 授权 */
       if (Authorize(op_uid, NULL, AuthRequest::INFO) == false)
       {
          err_oss<<"Authorize Fail : " << 
                       "only adminstrator can query all user's VM";
          return  ERR_PROJECT_AUTH_FAIL;
       }

        where_string << " uid = " << appointed_uid \
                     << " and project_name = " <<"'"<<req.appointed_project<<"'" \
                     << " order by oid limit " << req.count \
                     << " offset " << req.start_index ;

        where_string_count << " uid = " << appointed_uid \
                     << " and project_name = " <<"'"<<req.appointed_project<<"'";
    }
    else //查自己名下的VM
    { 
       where_string << " uid = " << op_uid \
                     << " and project_name = " <<"'"<<req.appointed_project<<"'"\
                     << " order by oid limit " << req.count \
                     << " offset " << req.start_index ;

       where_string_count << " uid = " << op_uid \
                     << " and project_name = " <<"'"<<req.appointed_project<<"'";
    }
    
    rc = _ppool->ShowProjectVm(info, where_string_count.str());
    if(0 != rc)
    {
        err_oss<<"Query failed !" ;
        return ERR_PROJECT_QUERY_FAIL;
    }
    max_count = info.size();
    if( 0 == max_count)
    {
        next_index = -1;
        return SUCCESS;
    }

    info.clear();
    
    // 调用project_pool的Show方法
    rc = _ppool->ShowProjectVm(info, where_string.str());
    if (0 != rc)
    {
        err_oss<<"Query failed !" ;
        return ERR_PROJECT_QUERY_FAIL;
    }

  
    int64  out_num = 0;
    //非尾页
    if (info.size() >= (uint64)(req.count ))
    {
        next_index = req.count + req.start_index;
        out_num    = req.count;
    }
    else  //尾页
    {
        next_index = -1;
        out_num    = info.size();
    }
    
    /* 把查询的结果放到消息中 */
    for (int64 i = 0; i < out_num; i++)
    {
         vec_prj.push_back(info.at(i));
    }    

    return  SUCCESS;

}
                        
/******************************************************************************/
bool ProjectOperate::Authorize(int64 opt_uid, Project *buf, int oprate)
{   
    /* 获取授权 */
    AuthRequest ar(opt_uid);
    int64       oid = INVALID_OID;
    int64       self_uid = INVALID_OID;

    if (buf == NULL)
    {
        oid = 0;
        self_uid = 0;
    }
    else
    {
        oid      = buf->get_oid();
        self_uid = buf->get_uid();
        if (INVALID_UID == self_uid)
        {
           ProjectOperate::_ppool->Drop(*buf); 
           return false;
        }
    }    
    ar.AddAuthorize(
                    AuthRequest::PROJECT,            //授权对象类型,用user做例子
                    oid,                             //对象的ID,oid
                    (AuthRequest::Operation)oprate,  //操作类型
                    self_uid,                        //对象拥有者的用户ID
                    false                            //此对象是否为公有
                    );
    
    if (-1 == UserPool::GetInstance()->Authorize(ar))
    {   /* 授权失败 */
        
        return false;
    }

    return true;

}
STATUS ProjectOperate::Str2ProjOp(const string   &action_name,
                          ProjectOperation    &type,
                          ostringstream  &err_oss)
{
    if (PJ_ACTION_DEPLOY == action_name)
    {
        type = PROJECT_DEPLOY;
    }
    else if (PJ_ACTION_CANCEL == action_name)
    {
        type = PROJECT_CENCEL;
    }
    else if (PJ_ACTION_REBOOT == action_name)
    {
        type = PROJECT_REBOOT;
    } 
    else if (PJ_ACTION_RESET == action_name)
    {
        type = PROJECT_RESET;
    } 
    else
    {
        err_oss <<"Unsupport action : "<< action_name << " \n";
        return ERR_RPOJECT_INVALID_ACTION_TYPE;
    }

    return SUCCESS;

}
/******************************************************************************/


/******************************************************************************/
STATUS ProjectOperate::DealStatisticsStaticData(const ApiProjectQuery  &req, 
                                                int64                    uid,
                                                int64                    appointed_uid,
                                                vector<ApiProjectStatisticsStaticData>  &vec_prj,
                                                int64                   &max_count,
                                                int64                   &next_index,
                                                ostringstream           &err_oss)
{
    ostringstream  where_string;
    ostringstream  where_string_count;
    int rc = -1;

//    vector<ApiProjectStatisticsStaticData> info;

    switch (static_cast <ApiProjectQueryType>(req.type) )
    {
        case PROJECT_USER_CREATE: 
        {
            where_string << " user_id = " << uid <<\
                          " order by project_id limit " <<req.count <<\
                          " offset " << req.start_index ;
            where_string_count << " user_id = " << uid;  
            break;
        }
    
        case PROJECT_USER_ALL:
        {
            /* 授权,这种写法，就只有管理员才有权限才能通过 */
            if (Authorize(uid, NULL,  AuthRequest::INFO) == false)
            {
                err_oss<<"Authorize Fail : " << 
                       "only adminstrator can query all user's project";
                return  ERR_PROJECT_AUTH_FAIL;
            }
    
            where_string << " 1=1 " <<\
                          " order by project_id limit " <<req.count <<\
                          " offset " << req.start_index ;
            where_string_count << " 1=1 ";   
            break;
        }
        
        case PROJECT_USER_APPOINTED:
        {
            /* 授权,这种写法，就只有管理员才有权限才能通过 */
            if (Authorize(uid, NULL,  AuthRequest::INFO) == false)
            {
                err_oss<<"Authorize Fail : " << 
                       "only adminstrator can query other user's project";
                return  ERR_PROJECT_AUTH_FAIL;
            }
            where_string << " user_id = " << appointed_uid <<\
                          " order by project_id limit " << req.count <<\
                          " offset " << req.start_index ;
            where_string_count << " user_id = " << appointed_uid ;
            break;
        }
        
        case PROJECT_APPOINTED:
        {
            where_string << " project_name = " <<"'"<< req.appointed_project <<"'"
                         << " and user_id = " << uid;
            where_string_count << " project_name = " <<"'"<< req.appointed_project <<"'"
                         << " and user_id = " << uid;             
            
            Project  project;
            rc=_ppool->GetProject(uid, req.appointed_project, project);
            if( SUCCESS != rc)
            {
                err_oss << "PROJECT ("<<req.appointed_project << ") not exist!";
                return  ERR_PROJECT_NOT_EXIST;
            }
            /* 授权 */
            if (Authorize(uid, &project, AuthRequest::INFO) == false)
            {
                err_oss<<"Authorize Fail";
                return  ERR_PROJECT_AUTH_FAIL;
            }
            break;
        }
        default:
        { 
            err_oss<<"Incorrect query type: "<< req.type <<endl;
            return ERR_PROJECT_QUERY_TYPE_ERR;
            break;
        }
    }

    rc = _ppool->ShowProjectStatisticsStaticData(vec_prj, where_string_count.str());
    if(0 != rc)
    {
        err_oss<<"Query failed !" ;
        return ERR_PROJECT_QUERY_FAIL;
    }
    max_count = vec_prj.size();
    if( 0 == max_count)
    {
        next_index = -1;
        return SUCCESS;
    }

    vec_prj.clear();
    
    // 调用project_pool的Show方法
    rc = _ppool->ShowProjectStatisticsStaticData(vec_prj, where_string.str());

    if (rc != 0)
    {
        err_oss<<"Query failed !" ;
        return ERR_PROJECT_QUERY_FAIL;
    }
  
    int64  out_num = 0;
    /* 对下包起始位置进行设置 */
    if (PROJECT_APPOINTED == static_cast <ApiProjectQueryType>(req.type))
    {
        next_index = -1;
        out_num = vec_prj.size();
    }
    else
    {    //非尾页
        if (vec_prj.size() >= (uint64)(req.count ))
        {
            next_index = req.count + req.start_index;
            out_num = req.count;
        }
        else  //尾页
        {
            next_index = -1;
            out_num = vec_prj.size();
        }
    }

   
    return SUCCESS;

}

/******************************************************************************/

STATUS ProjectOperate::DealProjectClusterQuery(ApiProjectDataQuery &req,
                                                   int64    &uid,                                                  
                                                   string  &cluster_name,
                                                   int64 &max_count,
                                                   int64 &next_index,
                                                   ostringstream &err_oss)
{

    ostringstream  where_string;
    int rc = -1;

    where_string << " project_name = " <<"'"<< req.appointed_project ;    
            
    Project  project;
    rc=_ppool->GetProject(uid, req.appointed_project, project);
    if( SUCCESS != rc)
    {
        err_oss << "PROJECT ("<<req.appointed_project << ") not exist!";
        return  ERR_PROJECT_NOT_EXIST;
    }
    /* 授权 */
    if (Authorize(uid, &project, AuthRequest::INFO) == false)
    {
        err_oss<<"Authorize Fail";
        return  ERR_PROJECT_AUTH_FAIL;
    }

    rc = _ppool->GetProjectCluster(cluster_name, where_string.str());
    if(0 != rc)
    {
        err_oss<<"Query failed !" ;
        return ERR_PROJECT_QUERY_FAIL;
    }
    max_count = 1;
    next_index = -1;

   
    return SUCCESS;

}


/******************************************************************************/


}
