/******************************************************************************
* Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：image_agent.cpp
* 文件标识：
* 内容摘要：image代理进程的实现文件
* 当前版本：1.0
* 作    者：张文剑
* 完成日期：2012年3月12日
*
* 修改记录1：
*     修改日期：2012/3/12
*     版 本 号：V1.0
*     修 改 人：张文剑
*     修改内容：创建
* 修改记录2：
*     修改日期：2013/3/12
*     版 本 号：V2.0
*     修 改 人：颜伟
*     修改内容：整改工作流and 架构
******************************************************************************/
#include "tecs_config.h"
#include "image_agent.h"
#include "image_base.h"
#include "image_download.h"
#include "log_agent.h"
#include "mid.h"
#include "event.h"
#include "volume.h"
#include <sys/mount.h>

namespace zte_tecs
{
//本机媒体面地址
static string media_address;

STATUS EnableImageAgentOptions()
{
    TecsConfig *config = TecsConfig::Get();
    config->EnableCustom("media_address", media_address, "Local media ip address.");
    return SUCCESS;
}

ImageAgent* ImageAgent::instance = NULL;

/**********************************************************************
* 函数名称：ImageProxy::MessageEntry
* 功能描述：ImageProxy消息单元主处理循环
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期     版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2012/03/12   V1.0    张文剑      创建
************************************************************************/
void ImageAgent::MessageEntry(const MessageFrame& message)
{
    ostringstream error;
    switch(m->get_state())
    {
        case S_Startup:
        {
            switch(message.option.id())
            {
                case EV_POWER_ON:
                {
                    if (SUCCESS != Transaction::Enable(GetDB()))
                    {
                        OutPut(SYS_ERROR,"ImageAgent enable transaction fail!");
                        SkyExit(-1, "ImageAgent::MessageEntry: call Transaction::Enable(GetDB()) failed.");
                    }
                    OutPut(SYS_DEBUG,"ImageAgent power on!\n");
                    m->set_state(S_Work);
                    OutPut(SYS_NOTICE, "%s power on!\n",m->name().c_str());
                    break;
                }
                default:
                    break;
            }
            break;
        }

        case S_Work:
        {
            switch(message.option.id())
            {
                #if 0
                case EV_VM_IMAGE_URL_PREPARE_REQ:
                {
                    ImageUrlPrepareReq prepare_req;
                    prepare_req.deserialize(message.data);
                    ImageUrlPrepareAck prepare_ack;
                    prepare_ack.action_id = prepare_req.action_id;
                    prepare_ack._request_id = prepare_req._request_id;
                    prepare_ack.state = SUCCESS;
                    prepare_ack.progress = 100;
                    prepare_ack._url = prepare_req._url;
                    MessageOption prepare_ack_option(message.option.sender(), EV_VM_IMAGE_URL_PREPARE_ACK, 0, 0);
                    m->Send(prepare_ack, prepare_ack_option);
                    break;
                }
                case EV_VM_IMAGE_URL_RELEASE_REQ:
                {
                    ImageUrlReleaseReq release_req;
                    release_req.deserialize(message.data);
                    ImageUrlReleaseAck release_ack;
                    release_ack.action_id = release_req.action_id;
                    release_ack._request_id = release_req._request_id;
                    release_ack.state = SUCCESS;
                    release_ack.progress = 100;
                    MessageOption release_ack_option(message.option.sender(), EV_VM_IMAGE_URL_RELEASE_ACK, 0, 0);
                    m->Send(release_ack, release_ack_option);

                    break;
                }
                #endif
                case EV_IMAGE_URL_PREPARE_REQ:
                {
                    HandleImageUrlPrepareReq(message);
                    break;
                }
                case EV_IMAGE_DEPLOY_REQ:
                {
                    HandleDeployReq(message);
                    break;
                }

                case EV_IMAGE_CANCEL_REQ:
                {
                    HandleCancelReq(message);
                    break;
                }

                case EV_STORAGE_CREATE_BLOCK_ACK:
                {
                    HandleCreateBlockAck(message);
                    break;
                }

                case EV_STORAGE_DELETE_BLOCK_ACK:
                {
                    HandleDeleteBlockAck(message);
                    break;
                }

                case EV_STORAGE_AUTH_BLOCK_ACK:
                {
                    HandleAuthBlockAck(message);
                    break;
                }

                case EV_STORAGE_DEAUTH_BLOCK_ACK:
                {
                    HandleDeAuthBlockAck(message);
                    break;
                }

                case EV_IMAGE_DOWNLOAD_ACK:
                {
                    HandleImageDownloadAck(message);
                    break;
                }

                case EV_IMAGE_CANCEL_DOWNLOAD_ACK:
                {
                    HandleCancelImageDownloadAck(message);
                    break;
                }

                default:
                    break;
            }
        }
    }
}

/**********************************************************************
* 函数名称：ImageProxy::Init
* 功能描述：
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期     版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2012/03/12   V1.0    张文剑      创建
************************************************************************/
STATUS ImageAgent::Init()
{
    STATUS ret = ERROR;
    ret = Start(MU_IMAGE_AGENT);
    if(SUCCESS != ret)
    {
        return ret;
    }

    //给自己发一个上电通知消息
    MessageOption option(MU_IMAGE_AGENT,EV_POWER_ON,0,0);
    MessageFrame frame(SkyInt(0),option);
    return m->Receive(frame);
}
/******************************************************************************/
void ImageAgent::CalcBlockSize(int64 origin_size, const ImageStoreOption& option, int64& size)
{
    if (IMAGE_SOURCE_TYPE_FILE == option._img_srctype)
    {
        size = (origin_size*(option._img_spcexp+100))/100;
    }
    else
    {
        size = origin_size + 4*1024*1024;
    }
}
/******************************************************************************/
STATUS ImageAgent::CheckTableAndWorkflow(const string &base_uuid,
                                                   bool &table,
                                                   bool &forward,
                                                   bool &rollback,
                                                   bool &reverse,
                                                   int &progress,
                                                   string &workflow_id,
                                                   string &rollback_id,
                                                   ostringstream &oss)
{
    int rc = ERROR;
    vector<ImageBase> vec_tables;

    rc = GetImageBaseFromTable(base_uuid, vec_tables);
    if (SUCCESS != rc && ERROR_OBJECT_NOT_EXIST != rc)
    {
        oss<<"Get image base ("<< base_uuid <<") from table failed! "<< endl ;
        return rc;
    }

    table = (0 == vec_tables.size())?false:true;

    ostringstream where;
    where << "label_string_1 = '" << base_uuid + "'";

    vector<string> workflows;
    //找正向
    rc = FindWorkflow(workflows, IMAGE_CATEGORY, IA_DEPLOY_IMAGE, where.str());
    if (SUCCESS != rc)
    {
        oss<<"Find work flow ("<< IMAGE_CATEGORY <<","<< IA_DEPLOY_IMAGE << ") failed! "<< endl ;
        return rc;
    }
    if (0 != workflows.size())
    {
        forward = true;
        workflow_id = workflows[0];
        progress = GetWorkflowProgress(workflow_id);
        oss<<"Work flow "<< workflow_id <<"("<< IMAGE_CATEGORY <<","<< IA_DEPLOY_IMAGE << ") exist,progress = " << progress <<endl;
        rc = GetRollbackWorkflow(workflow_id, rollback_id);
        if (SUCCESS != rc)
        {
            oss<<"Get rollback work flow of "<< workflow_id << ") failed! "<< endl ;
            return rc;
        }
        //有正向 不需要再去找回滚和反向了
        return SUCCESS;
    }

    //找回滚和反向
    rc = FindWorkflow(workflows, IMAGE_CATEGORY, IA_CANCEL_IMAGE, where.str());
    if (SUCCESS != rc)
    {
        oss<<"Find work flow ("<< IMAGE_CATEGORY <<","<< IA_CANCEL_IMAGE << ") failed! "<< endl ;
        return rc;
    }
    if (0 != workflows.size())
    {
        if (IsRollbackWorkflow(workflows[0]))
        {
            rollback = true;
            rollback_id = workflows[0];
        }
        else
        {
            reverse = true;
            workflow_id = workflows[0];
            progress = GetWorkflowProgress(workflows[0]);
        }
        oss<<"Work flow "<< workflows[0] <<"("<< IMAGE_CATEGORY <<","<< IA_CANCEL_IMAGE << ") exist,progress = " << progress <<endl;
    }

    return SUCCESS;
}
/******************************************************************************/
STATUS ImageAgent::CreateDeployWorkflow(const MID &sender, const DeployBaseImageReq &req, ostringstream &oss, string & workflow_id)
{
    STATUS ret = ERROR;

    Action create_action(IMAGE_CATEGORY,CREATE_BLOCK,EV_STORAGE_CREATE_BLOCK_REQ,MID(m->name()),GetSC_MID());
    CreateBlockReq create_req;
    CalcBlockSize(req._disk_size,req._option,create_req._volsize);
    create_req._cluster_name = req._cluster_name;
    create_req._request_id = req._base_uuid;
    create_req.action_id = create_action.get_id();
    create_req._uid = INVALID_UID;
    create_action.message_req = create_req.serialize();

    string ip = media_address;
    if (ip.empty())
    {
        TecsConfig *config = TecsConfig::Get();
        ret = GetIpByConnectBroker(config->get_sky_conf().messaging.connects[0].broker_url,ip);
        if (SUCCESS != ret)
        {
            oss<<"Create workflow failed for GetIpByConnectBroker! id = "<<req._base_uuid<<endl;
            return ret;
        }
    }

    string iscsi;
    if (!GetIscsiName(iscsi))
    {
        oss<<"Create workflow failed for  GetIscsiName! id = "<<req._base_uuid<<endl;
        return ret;
    }

    Action auth_action(IMAGE_CATEGORY,AUTH_BLOCK,EV_STORAGE_AUTH_BLOCK_REQ,MID(m->name()),GetSC_MID());
    AuthBlockReq auth_req;
    auth_req.action_id = auth_action.get_id();
    auth_req._request_id = req._base_uuid;
    auth_req._iscsiname = iscsi;
    auth_req._userip = ip;
    auth_req._vid= -1;
    auth_req._dist_id = req._base_id;
    auth_req._cluster_name = req._cluster_name;
    auth_req._usage = to_string<int64>(req._base_id,dec)+":"+req._cluster_name;
    
    auth_action.message_req = auth_req.serialize();

    Action download_action(IMAGE_CATEGORY,DOWNLOAD_IMAGE,EV_IMAGE_DOWNLOAD_REQ,MID(m->name()),MID(req._cluster_name,PROC_IMAGE_AGENT,MU_IMAGE_DOWNLOAD_AGENT));
    FileUrl target_url;
    ImageDownloadReq download_req(download_action.get_id(),
                                  req._base_uuid,
                                  req._image_id,
                                  req._size,
                                  req._url,
                                  target_url,
                                  req._option);
    download_action.message_req = download_req.serialize();

    Action create_rollback_action(IMAGE_CATEGORY,DELETE_BLOCK,EV_STORAGE_DELETE_BLOCK_REQ,MID(m->name()),GetSC_MID());
    DeleteBlockReq delete_req;
    delete_req.action_id = create_rollback_action.get_id();
    delete_req._request_id = req._base_uuid;
    delete_req._uid = INVALID_UID;
    create_rollback_action.message_req = delete_req.serialize();

    Action auth_rollback_action(IMAGE_CATEGORY,DEAUTH_BLOCK,EV_STORAGE_DEAUTH_BLOCK_REQ,MID(m->name()),GetSC_MID());
    DeAuthBlockReq deauth_req;
    deauth_req.action_id = auth_rollback_action.get_id();
    deauth_req._request_id = req._base_uuid;
    deauth_req._iscsiname = iscsi;
    deauth_req._userip = ip;
    deauth_req._vid= -1;
    deauth_req._dist_id = req._base_id;
    deauth_req._cluster_name = req._cluster_name;
    auth_rollback_action.message_req = deauth_req.serialize();

    Action download_rollback_action(IMAGE_CATEGORY,CANCEL_DOWNLOAD_IMAGE,EV_IMAGE_CANCEL_DOWNLOAD_REQ,MID(m->name()),MID(req._cluster_name,PROC_IMAGE_AGENT,MU_IMAGE_DOWNLOAD_AGENT));
    ImageDownloadReq cancel_download_req(download_rollback_action.get_id(),
                                  req._base_uuid,
                                  req._image_id,
                                  req._size,
                                  req._url,
                                  target_url,
                                  req._option);
    download_rollback_action.message_req = cancel_download_req.serialize();

    create_action.rollback_action_id = create_rollback_action.get_id();
    auth_action.rollback_action_id = auth_rollback_action.get_id();
    download_action.rollback_action_id = download_rollback_action.get_id();

    create_action.success_feedback = false;
    create_action.timeout_feedback = false;
    auth_action.timeout_feedback = false;
    download_action.timeout_feedback = false;
    create_rollback_action.timeout_feedback = false;
    auth_rollback_action.success_feedback = false;
    auth_rollback_action.timeout_feedback = false;
    download_rollback_action.success_feedback = false;
    download_rollback_action.timeout_feedback = false;

    download_action.progress_weight = 10;

    CreateBlockLabels create_labels;
    create_labels._request_id = req._base_uuid;
    create_labels._size = req._disk_size;
    create_labels._cluster_name = req._cluster_name;
    create_action.labels = create_labels;
    create_rollback_action.labels = create_labels;

    AuthBlockLabels auth_labels;
    auth_labels._request_id = req._base_uuid;
    auth_labels._cluster_name = req._cluster_name;
    auth_labels._vid = -1;
    auth_labels._ip = ip;
    auth_labels._iscsi = iscsi;
    auth_labels._dist_id = req._base_id;
    auth_action.labels = auth_labels;
    auth_rollback_action.labels = auth_labels;

    DownloadImageLabels download_labels;
    download_labels._request_id = req._base_uuid;
    download_labels._source_url = req._url.serialize();
    download_action.labels = download_labels;
    download_rollback_action.labels = download_labels;

    Workflow w_rollback(IMAGE_CATEGORY,IA_CANCEL_IMAGE);
    w_rollback.Add(&download_rollback_action);
    w_rollback.Add(&download_rollback_action,&auth_rollback_action);
    w_rollback.Add(&auth_rollback_action,&create_rollback_action);

    Workflow w(IMAGE_CATEGORY,IA_DEPLOY_IMAGE);
    w.Add(&create_action);
    w.Add(&create_action,&auth_action);
    w.Add(&auth_action,&download_action);

    w.upstream_action_id = req.action_id;
    w.upstream_sender = sender;
    w_rollback.upstream_action_id = req.action_id;
    w_rollback.upstream_sender = sender;

    ImageBaseLabels deploy_labels;
    deploy_labels._base_uuid = req._base_uuid;
    deploy_labels._cluster_name = req._cluster_name;
    deploy_labels._image_id = req._image_id;
    deploy_labels._size = create_req._volsize;
    w.labels = deploy_labels;
    w_rollback.labels = deploy_labels;

    workflow_id = w.get_id();

    ret = CreateWorkflow(w,w_rollback);
    if(SUCCESS != ret)
    {
        oss<<"Create Deploy Image Workflow Failed!" << endl ;
    }

    return ret;

}
/******************************************************************************/
STATUS ImageAgent::CreateCancelWorkflow(const MID &sender, const CancelBaseImageReq &req,ostringstream &oss, string & workflow_id)
{
    STATUS ret = ERROR;

    Action delete_action(IMAGE_CATEGORY,DELETE_BLOCK,EV_STORAGE_DELETE_BLOCK_REQ,MID(m->name()),GetSC_MID());
    DeleteBlockReq delete_req;
    delete_req.action_id = delete_action.get_id();
    delete_req._request_id = req._base_uuid;
    delete_req._uid= INVALID_UID;
    delete_action.message_req = delete_req.serialize();

    string ip = media_address;
    if (ip.empty())
    {
        TecsConfig *config = TecsConfig::Get();
        ret = GetIpByConnectBroker(config->get_sky_conf().messaging.connects[0].broker_url,ip);
        if (SUCCESS != ret)
        {
            oss<<"Create workflow failed for GetIpByConnectBroker! id = "<<req._base_uuid<<endl;
            return ret;
        }
    }

    string iscsi;
    if (!GetIscsiName(iscsi))
    {
        oss<<"Create workflow failed for  GetIscsiName! id = "<<req._base_uuid<<endl;
        return ret;
    }
    Action deauth_action(IMAGE_CATEGORY,DEAUTH_BLOCK,EV_STORAGE_DEAUTH_BLOCK_REQ,MID(m->name()),GetSC_MID());
    DeAuthBlockReq deauth_req;
    deauth_req.action_id = deauth_action.get_id();
    deauth_req._request_id = req._base_uuid;
    deauth_req._iscsiname = iscsi;
    deauth_req._userip = ip;
    deauth_req._vid= -1;
    deauth_req._dist_id = req._base_id;
    deauth_req._cluster_name = req._cluster_name;
    deauth_action.message_req = deauth_req.serialize();

    Action cancel_download_action(IMAGE_CATEGORY,CANCEL_DOWNLOAD_IMAGE,EV_IMAGE_CANCEL_DOWNLOAD_REQ,MID(m->name()),MID(req._cluster_name,PROC_IMAGE_AGENT,MU_IMAGE_DOWNLOAD_AGENT));
    ImageBase var;
    GetImageBaseByBaseUuid(req._base_uuid, var);
    FileUrl target_url;
    target_url.deserialize(var._access_url);
    ImageDownloadReq cancel_download_req(cancel_download_action.get_id(),
                              req._base_uuid,
                              req._image_id,
                              req._size,
                              req._url,
                              target_url,
                              req._option);
    cancel_download_action.message_req = cancel_download_req.serialize();

    delete_action.timeout_feedback = false;
    deauth_action.success_feedback = false;
    deauth_action.timeout_feedback = false;
    cancel_download_action.success_feedback = false;
    cancel_download_action.timeout_feedback = false;

    CreateBlockLabels delete_labels;
    delete_labels._request_id = req._base_uuid;
    delete_labels._size = req._disk_size;
    delete_labels._cluster_name = req._cluster_name;
    delete_action.labels = delete_labels;

    AuthBlockLabels deauth_labels;
    deauth_labels._request_id = req._base_uuid;
    deauth_labels._cluster_name = req._cluster_name;
    deauth_labels._vid = -1;
    deauth_labels._ip = ip;
    deauth_labels._iscsi = iscsi;
    deauth_labels._dist_id = req._base_id;
    deauth_action.labels = deauth_labels;

    DownloadImageLabels cancel_download_labels;
    cancel_download_labels._request_id = req._base_uuid;
    cancel_download_labels._source_url = req._url.serialize();
    cancel_download_labels._target_url = var._access_url;
    cancel_download_action.labels = cancel_download_labels;

    Workflow w(IMAGE_CATEGORY,IA_CANCEL_IMAGE);
    w.Add(&cancel_download_action);
    w.Add(&cancel_download_action,&deauth_action);
    w.Add(&deauth_action,&delete_action);

    w.upstream_action_id = req.action_id;
    w.upstream_sender = sender;

    ImageBaseLabels cancel_labels;
    cancel_labels._base_uuid = req._base_uuid;
    cancel_labels._cluster_name = req._cluster_name;
    cancel_labels._image_id = req._image_id;
    CalcBlockSize(req._size, req._option, cancel_labels._size);
    w.labels = cancel_labels;

    workflow_id = w.get_id();

    ret = CreateWorkflow(w);
    if(SUCCESS != ret)
    {
        oss<<"Create Cancel Image Workflow Failed!" << endl ;
    }

    return ret;
}
/******************************************************************************/
STATUS ImageAgent::HandleDeployReq(const MessageFrame &message)
{
    DeployBaseImageReq req;
    if(!req.deserialize(message.data))
    {
        OutPut(SYS_ERROR,"Handle Deploy Request Deserialize Failed\n");
        return ERROR;
    }

    if (req._base_uuid.empty() || req._cluster_name.empty() || req._url.access_option.empty()
        || INVALID_IID == req._image_id || 0 == req._size)
    {
        OutPut(SYS_ERROR,"Handle Deploy Request Argument Invalid\n");
        return ERROR;
    }

    STATUS rc = ERROR;
    ostringstream oss;

    MID sender = message.option.sender();
    MessageOption ack_option(sender, EV_IMAGE_DEPLOY_ACK, 0, 0);
    DeployBaseImageAck ack_msg(req.action_id, req._base_uuid, req._cluster_name);

    bool table = false,forward = false,rollback = false,reverse = false;
    int progress = 0;
    string workflow_id,rollback_id;;

    rc = CheckTableAndWorkflow(req._base_uuid,
                               table,
                               forward,
                               rollback,
                               reverse,
                               progress,
                               workflow_id,
                               rollback_id,
                               oss);
    if (SUCCESS != rc)
    {//查询
        ack_msg.state = rc;
    }
    else
    {

        if (forward)
        {//有正向
            ack_msg.state = ERROR_ACTION_RUNNING;
            ack_msg.progress = progress;
            oss<<"Deploying Image Exist"<<endl ;
        }
        else if (rollback)
        {//有回滚
            ack_msg.state = ERROR_ACTION_RUNNING;
            ack_msg.progress = progress;
            oss<<"Rollbacking Image Exist"<<endl ;
        }
        else if (reverse)
        {//有反向
            ack_msg.state = ERROR_ACTION_CANCELLED;
            ack_msg.progress = 0;
            oss<<"Canceling Image Exist"<<endl ;
        }
        else if (table)
        {//有表
            ack_msg.state = SUCCESS;
            ack_msg.progress = 100;
            oss<<"Deployed Image Exist"<<endl ;

        }
        else if (!table)
        {//无表
            if (IsUuidDeleted(req._base_uuid))
            {//uuid 已经入回收站了
                ack_msg.state = ERROR_DUPLICATED_OP;
                ack_msg.progress = 0;
                oss<<"BaseUuid " << req._base_uuid << " Is Discarded"<<endl ;
            }
            else
            {
                rc = CreateDeployWorkflow(sender, req, oss, ack_msg._workflow_id);
                if (SUCCESS != rc)
                {
                    ack_msg.state = rc;
                    oss<<"Create Deploy Workflow Failed,ret:"<<rc<<endl ;
                }
                else
                {
                    ack_msg.state = ERROR_ACTION_RUNNING;
                    ack_msg.progress = 0;
                    oss<<"Ready To Deploy Image"<<endl ;
                }
            }
        }
    }

    ack_msg.detail = oss.str();
    if (SUCCESS == rc)
    {
        OutPut(SYS_DEBUG,"Deploy Base(%s) To %s Success,state:%d,detail:%s\n",
                         req._base_uuid.c_str(),
                         req._cluster_name.c_str(),
                         ack_msg.state,
                         ack_msg.detail.c_str());
    }
    else
    {
        OutPut(SYS_ERROR,"Deploy Base(%s) To %s Failed,state:%d,detail:%s\n",
                         req._base_uuid.c_str(),
                         req._cluster_name.c_str(),
                         ack_msg.state,
                         ack_msg.detail.c_str());
    }
    return m->Send(ack_msg, ack_option);
}
/******************************************************************************/
STATUS ImageAgent::HandleCancelReq(const MessageFrame &message)
{
    CancelBaseImageReq req;
    if(!req.deserialize(message.data))
    {
        OutPut(SYS_ERROR,"Handle Cancel Request Deserialize Failed\n");
        return ERROR;
    }

    if (req._base_uuid.empty() || req._cluster_name.empty() || req._url.access_option.empty()
        || INVALID_IID == req._image_id || 0 == req._size)
    {
        OutPut(SYS_ERROR,"Handle Cancel Request Argument Invalid\n");
        return ERROR;
    }

    int rc = ERROR;
    ostringstream oss;

    MID sender = message.option.sender();
    MessageOption ack_option(sender, EV_IMAGE_CANCEL_ACK, 0, 0);
    CancelBaseImageAck ack_msg(req.action_id, req._base_uuid, req._cluster_name);

    bool table = false,forward = false,rollback = false,reverse = false;
    int progress = 0;
    string workflow_id,rollback_id;;

    rc = CheckTableAndWorkflow(req._base_uuid,
                               table,
                               forward,
                               rollback,
                               reverse,
                               progress,
                               workflow_id,
                               rollback_id,
                               oss);
    if (SUCCESS != rc)
    {//查询
        ack_msg.state = rc;
    }
    else
    {
        if (forward)
        {//有正向
            rc = RollbackWorkflow(workflow_id);
            if (SUCCESS != rc)
            {
                ack_msg.state = rc;
                oss<<"Rollback Image Deploy Failed,ret:"<< rc <<endl ;
            }
            else
            {
                oss<<"Ready To Rollback Image Deploy"<<endl ;
                rc = UnsetRollbackAttr(rollback_id);
                if (SUCCESS != rc)
                {
                    ack_msg.state = rc;
                    oss<<"Unset Rollback Attr Failed,ret:"<< rc <<endl ;
                }
                else
                {
                    oss<<"Unset Rollback Attr Success"<<endl ;
                    ack_msg.state = ERROR_ACTION_RUNNING;
                    ack_msg.progress = progress;
                    oss<<"Canceling Image"<<endl ;
                }
            }
        }
        else if (rollback)
        {//有回滚
            rc = UnsetRollbackAttr(rollback_id, message.option.sender(), req.action_id);
            if (SUCCESS != rc)
            {
                ack_msg.state = rc;
                oss<<"Unset Rollback Attr Failed,ret:"<< rc <<endl ;
            }
            else
            {
                oss<<"Unset Rollback Attr Success"<<endl ;
                ack_msg.state = ERROR_ACTION_RUNNING;
                ack_msg.progress = progress;
                oss<<"Canceling Image"<<endl ;
            }
        }
        else if (reverse)
        {//有反向
            ack_msg.state = ERROR_ACTION_RUNNING;
            ack_msg.progress = progress;
            oss<<"Canceling Image"<<endl ;
        }
        else if (!table)
        {//无表
            ack_msg.state = SUCCESS;
            ack_msg.progress = 100;
            oss<<"Cancel Image Success For Canceled"<<endl ;
        }
        else if (table)
        {//有表
            rc = CreateCancelWorkflow(sender, req, oss, ack_msg._workflow_id);
            if (SUCCESS != rc)
            {
                ack_msg.state = rc;
                oss<<"Create Cancel Workflow Failed,ret:"<< rc <<endl ;
            }
            else
            {
                oss<<"Create Cancel Workflow Success"<<endl ;
                ack_msg.state = ERROR_ACTION_RUNNING;
                ack_msg.progress = progress;
                oss<<"Canceling Image"<<endl ;
            }
        }
    }

    ack_msg.detail = oss.str();
    if (SUCCESS == rc)
    {
        OutPut(SYS_DEBUG,"Cancel Base(%s) To %s Success,state:%d,detail:%s\n",
                         req._base_uuid.c_str(),
                         req._cluster_name.c_str(),
                         ack_msg.state,
                         ack_msg.detail.c_str());
    }
    else
    {
        OutPut(SYS_ERROR,"Cancel Base(%s) To %s Failed,state:%d,detail:%s\n",
                         req._base_uuid.c_str(),
                         req._cluster_name.c_str(),
                         ack_msg.state,
                         ack_msg.detail.c_str());
    }
    return m->Send(ack_msg, ack_option);
}
/******************************************************************************/
STATUS ImageAgent::HandleCreateBlockAck(const MessageFrame &message)
{
    OutPut(SYS_DEBUG,"CreateBlockAck:%s\n",message.data.c_str());
    CreateBlockAck ack;
    ack.deserialize(message.data);

    if(true == IsActionFinished(ack.action_id))
    {
        return SUCCESS;
    }

    STATUS rc = ERROR;
    Action action;
    rc = GetActionById(ack.action_id,action);
    if(SUCCESS != rc)
    {
        return rc;
    }

    OutPut(SYS_DEBUG,"ImageAgent receives action %s(%s) feedback!\n",
        ack.action_id.c_str(),
        action.name.c_str());

    switch(ack.state)
    {
        case SUCCESS:
            //不应该收到这个feedback
            OutPut(SYS_NOTICE,"action %s is success, but not need!\n",ack.action_id.c_str());
            rc = SUCCESS;
            break;
        case ERROR_ACTION_RUNNING:
            OutPut(SYS_DEBUG,"action %s is already running, ok!\n",ack.action_id.c_str());
            rc = SUCCESS;
            break;
        default:
            OutPut(SYS_ERROR,"action %s failed!\n",ack.action_id.c_str());
            rc = RollbackWorkflow(ack.action_id);
            break;
    }

    return rc;
}
/******************************************************************************/
STATUS ImageAgent::HandleDeleteBlockAck(const MessageFrame &message)
{
    OutPut(SYS_DEBUG,"DeleteBlockAck:%s\n",message.data.c_str());
    DeleteBlockAck ack;
    ack.deserialize(message.data);
    stringstream strStream;

    if(true == IsActionFinished(ack.action_id))
    {
        return SUCCESS;
    }

    STATUS rc = ERROR;
    Action action;
    rc = GetActionById(ack.action_id,action);
    if(SUCCESS != rc)
    {
        return rc;
    }

    OutPut(SYS_DEBUG,"ImageAgent receives action %s(%s) feedback!\n",
        ack.action_id.c_str(),
        action.name.c_str());

    switch(ack.state)
    {
        case SUCCESS:
        {
            ImageBaseLabels labels;
            rc = GetWorkflowLabels(ack.action_id,labels);
            if(SUCCESS != rc)
            {
                SkyAssert(false);
                break;
            }

            string upstream_action_id;
            rc = GetUpstreamActionId(ack.action_id,upstream_action_id);
            if(SUCCESS != rc)
            {
                SkyAssert(false);
                break;
            }

            MID sender;
            rc = GetUpstreamSender(ack.action_id,sender);
            if(SUCCESS != rc)
            {
                SkyAssert(false);
                break;
            }

            if (IsRollbackWorkflow(ack.action_id))
            {//正向回滚成功 finishaction 并返回部署失败
                rc = FinishAction(ack.action_id);
                if (SUCCESS != rc)
                {
                    SkyAssert(false);
                    break;
                }
                //发部署失败应答
                DeployBaseImageAck deploy_ack(upstream_action_id,labels._base_uuid,labels._cluster_name);
                deploy_ack.state = ERROR;
                strStream <<"deploy img "<<labels._image_id<<" fail!";
                deploy_ack.detail = strStream.str();
                MessageOption deploy_option(sender, EV_IMAGE_DEPLOY_ACK, 0, 0);
                rc = m->Send(deploy_ack, deploy_option);
            }
            else
            {//反向成功
                rc = Transaction::Begin();
                if(SUCCESS != rc)
                {
                    SkyAssert(false);
                    break;
                }
                rc = DeleteImageBase(labels._base_uuid);
                if(SUCCESS != rc)
                {
                    SkyAssert(false);
                    Transaction::Cancel();
                    break;
                }
                rc = FinishAction(ack.action_id);
                if(SUCCESS != rc)
                {
                    SkyAssert(false);
                    Transaction::Cancel();
                    break;
                }
                rc = DeleteUuid(labels._base_uuid);
                if(SUCCESS != rc)
                {
                    SkyAssert(false);
                    Transaction::Cancel();
                    break;
                }
                rc = Transaction::Commit();
                if(SUCCESS != rc)
                {
                    SkyAssert(false);
                    Transaction::Cancel();
                    break;
                }
                //发撤销成功应答
                CancelBaseImageAck cancel_ack(upstream_action_id,labels._base_uuid,labels._cluster_name);
                cancel_ack.state = SUCCESS;
                cancel_ack.progress = 100;
                MessageOption cancel_option(sender, EV_IMAGE_CANCEL_ACK, 0, 0);
                rc = m->Send(cancel_ack, cancel_option);
            }
            break;
        }
        case ERROR_ACTION_RUNNING:
        {
            OutPut(SYS_DEBUG,"action %s is already running, ok!\n",ack.action_id.c_str());
            rc = SUCCESS;
            break;
        }
        default:
        {
            OutPut(SYS_ERROR,"action %s failed!\n",ack.action_id.c_str());
            rc = RestartAction(ack.action_id);
            break;
        }
    }
    return rc;
}
/******************************************************************************/
STATUS ImageAgent::HandleAuthBlockAck(const MessageFrame &message)
{
    OutPut(SYS_DEBUG,"AuthBlockAck:%s\n",message.data.c_str());
    AuthBlockAck ack;
    ack.deserialize(message.data);

    if(true == IsActionFinished(ack.action_id))
    {
        return SUCCESS;
    }

    STATUS rc = ERROR;
    Action action;
    rc = GetActionById(ack.action_id,action);
    if(SUCCESS != rc)
    {
        return rc;
    }

    OutPut(SYS_DEBUG,"ImageAgent receives action %s(%s) feedback!\n",
        ack.action_id.c_str(),
        action.name.c_str());

    switch(ack.state)
    {
        case SUCCESS:
        {
            //更新imagedownload action和imagecanceldownload的req
            vector<string> actions;
            rc = GetActionsByName(ack.action_id,DOWNLOAD_IMAGE,actions);
            if(SUCCESS != rc)
            {
                SkyAssert(false);
                break;
            }
            string message_req;
            rc = GetActionMessageReq(actions[0],DOWNLOAD_IMAGE,message_req);
            if(SUCCESS != rc)
            {
                SkyAssert(false);
                break;
            }
            string rollback_action_id;
            rc = GetRollbackAction(actions[0], rollback_action_id);
            if(SUCCESS != rc)
            {
                SkyAssert(false);
                break;
            }

            string cancel_message_req;
            rc = GetActionMessageReq(rollback_action_id,CANCEL_DOWNLOAD_IMAGE,cancel_message_req);
            if(SUCCESS != rc)
            {
                SkyAssert(false);
                break;
            }

            iSCSIAccessOption option;

            AuthBlockReq req;
            req.deserialize(action.message_req);            
            option._type = ack._type;            
            option._dataip_array = ack._dataip_vec;
            LunAddr  addr(ack._target_name,ack._lunid);                            
            option._type = ack._type;            
            option._dataip_array = ack._dataip_vec;
            option._auth_tbl.AddAuth(req._iscsiname, addr); 
            ImageDownloadReq download_req;
            download_req.deserialize(message_req);
            download_req._target_url.access_type = IMAGE_ACCESS_ISCSI;
            download_req._target_url.access_option = option.serialize();
            ImageDownloadReq cancel_download_req;
            cancel_download_req.deserialize(cancel_message_req);
            cancel_download_req._target_url.access_type = IMAGE_ACCESS_ISCSI;
            cancel_download_req._target_url.access_option = option.serialize();

            DownloadImageLabels labels;
            rc = GetActionLabels(actions[0],labels);
            if(SUCCESS != rc)
            {
                SkyAssert(false);
                break;
            }
            labels._target_url = download_req._target_url.serialize();

            DownloadImageLabels cancel_labels;
            rc = GetActionLabels(rollback_action_id,cancel_labels);
            if(SUCCESS != rc)
            {
                SkyAssert(false);
                break;
            }
            cancel_labels._target_url = cancel_download_req._target_url.serialize();

            rc = Transaction::Begin();
            if(SUCCESS != rc)
            {
                SkyAssert(false);
                break;
            }
            rc = UpdateActionReq(actions[0], download_req.serialize());
            if(SUCCESS != rc)
            {
                SkyAssert(false);
                Transaction::Cancel();
                break;
            }
            rc = SetActionLabels(actions[0],labels);
            if(SUCCESS != rc)
            {
                SkyAssert(false);
                Transaction::Cancel();
                break;
            }
            rc = UpdateActionReq(rollback_action_id, cancel_download_req.serialize());
            if(SUCCESS != rc)
            {
                SkyAssert(false);
                Transaction::Cancel();
                break;
            }
            rc = SetActionLabels(rollback_action_id,cancel_labels);
            if(SUCCESS != rc)
            {
                SkyAssert(false);
                Transaction::Cancel();
                break;
            }
            rc = FinishAction(ack.action_id);
            if(SUCCESS != rc)
            {
                SkyAssert(false);
                Transaction::Cancel();
                break;
            }
            rc = Transaction::Commit();
            if(SUCCESS != rc)
            {
                SkyAssert(false);
                Transaction::Cancel();
                break;
            }
            break;
        }
        case ERROR_ACTION_RUNNING:
        {
            OutPut(SYS_DEBUG,"action %s is already running, ok!\n",ack.action_id.c_str());
            rc = SUCCESS;
            break;
        }
        default:
        {
            OutPut(SYS_ERROR,"action %s failed!\n",ack.action_id.c_str());
            rc = RollbackWorkflow(ack.action_id);
            break;
        }
    }

    return rc;
}
/******************************************************************************/
STATUS ImageAgent::HandleDeAuthBlockAck(const MessageFrame &message)
{
    OutPut(SYS_DEBUG,"DeAuthBlockAck:%s\n",message.data.c_str());
    DeAuthBlockAck ack;
    ack.deserialize(message.data);

    if(true == IsActionFinished(ack.action_id))
    {
        return SUCCESS;
    }

    STATUS rc = ERROR;
    Action action;
    rc = GetActionById(ack.action_id,action);
    if(SUCCESS != rc)
    {
        return rc;
    }

    OutPut(SYS_DEBUG,"ImageAgent receives action %s(%s) feedback!\n",
        ack.action_id.c_str(),
        action.name.c_str());

    switch(ack.state)
    {
        case SUCCESS:
        {
            //不应该收到这个feedback
            OutPut(SYS_NOTICE,"action %s is success, but not need!\n",ack.action_id.c_str());
            rc = SUCCESS;
            break;
        }
        case ERROR_ACTION_RUNNING:
        {
            OutPut(SYS_DEBUG,"action %s is already running, ok!\n",ack.action_id.c_str());
            rc = SUCCESS;
            break;
        }
        default:
        {
            OutPut(SYS_ERROR,"action %s failed!\n",ack.action_id.c_str());
            rc = RestartAction(ack.action_id);
            break;
        }
    }

    return rc;
}
/******************************************************************************/
STATUS ImageAgent::HandleImageDownloadAck(const MessageFrame &message)
{
    OutPut(SYS_DEBUG,"ImageDownloadAck:%s\n",message.data.c_str());
    ImageDownloadAck ack;
    ack.deserialize(message.data);

    if(true == IsActionFinished(ack.action_id))
    {
        return SUCCESS;
    }

    STATUS rc = ERROR;
    Action action;
    rc = GetActionById(ack.action_id,action);
    if(SUCCESS != rc)
    {
        return rc;
    }

    OutPut(SYS_DEBUG,"ImageAgent receives action %s(%s) feedback!\n",
        ack.action_id.c_str(),
        action.name.c_str());

    switch(ack.state)
    {
        case SUCCESS:
        {
            ImageBaseLabels labels;
            rc = GetWorkflowLabels(ack.action_id,labels);
            if(SUCCESS != rc)
            {
                SkyAssert(false);
                break;
            }
            DownloadImageLabels download_labels;
            rc = GetActionLabels(ack.action_id, download_labels);
            if(SUCCESS != rc)
            {
                SkyAssert(false);
                break;
            }
            string upstream_action_id;
            rc = GetUpstreamActionId(ack.action_id,upstream_action_id);
            if(SUCCESS != rc)
            {
                SkyAssert(false);
                break;
            }
            MID sender;
            rc = GetUpstreamSender(ack.action_id,sender);
            if(SUCCESS != rc)
            {
                SkyAssert(false);
                break;
            }
            rc = Transaction::Begin();
            if(SUCCESS != rc)
            {
                SkyAssert(false);
                break;
            }
            rc = AllocateImageBase(labels._base_uuid, labels._image_id, labels._size, download_labels._target_url);
            if(SUCCESS != rc)
            {
                SkyAssert(false);
                Transaction::Cancel();
                break;
            }
            rc = FinishAction(ack.action_id);
            if(SUCCESS != rc)
            {
                SkyAssert(false);
                Transaction::Cancel();
                break;
            }
            rc = Transaction::Commit();
            if(SUCCESS != rc)
            {
                SkyAssert(false);
                Transaction::Cancel();
                break;
            }

            DeployBaseImageAck up_ack(upstream_action_id, labels._base_uuid, labels._cluster_name);
            MessageOption up_ack_option(sender, EV_IMAGE_DEPLOY_ACK, 0, 0);
            up_ack.state = SUCCESS;
            up_ack.progress = 100;
            up_ack.detail = "OK";
            break;
        }
        case ERROR_ACTION_RUNNING:
        {
            OutPut(SYS_DEBUG,"action %s is already running, ok!\n",ack.action_id.c_str());
            rc = SUCCESS;
            break;
        }
        default:
        {
            OutPut(SYS_ERROR,"action %s failed!\n",ack.action_id.c_str());
            rc = RollbackWorkflow(ack.action_id);
            break;
        }
    }

    return rc;
}
/******************************************************************************/
STATUS ImageAgent::HandleCancelImageDownloadAck(const MessageFrame &message)
{
    OutPut(SYS_DEBUG,"CancelImageDownloadAck:%s\n",message.data.c_str());
    ImageDownloadAck ack;
    ack.deserialize(message.data);

    if(true == IsActionFinished(ack.action_id))
    {
        return SUCCESS;
    }

    STATUS rc = ERROR;
    Action action;
    rc = GetActionById(ack.action_id,action);
    if(SUCCESS != rc)
    {
        return rc;
    }

    OutPut(SYS_DEBUG,"ImageAgent receives action %s(%s) feedback!\n",
        ack.action_id.c_str(),
        action.name.c_str());

    switch(ack.state)
    {
        case SUCCESS:
        {
            ImageBaseLabels labels;
            rc = GetWorkflowLabels(ack.action_id,labels);
            if(SUCCESS != rc)
            {
                SkyAssert(false);
                break;
            }
            string upstream_action_id;
            rc = GetUpstreamActionId(ack.action_id,upstream_action_id);
            if(SUCCESS != rc)
            {
                SkyAssert(false);
                break;
            }
            MID sender;
            rc = GetUpstreamSender(ack.action_id,sender);
            if(SUCCESS != rc)
            {
                SkyAssert(false);
                break;
            }

            if (IsRollbackWorkflow(ack.action_id))
            {//正向回滚成功 finishaction 并返回部署失败
                rc = FinishAction(ack.action_id);
                if (SUCCESS != rc)
                {
                    SkyAssert(false);
                    break;
                }
                //发部署失败应答
                DeployBaseImageAck deploy_ack(upstream_action_id, labels._base_uuid, labels._cluster_name);
                deploy_ack.state = ERROR;
                MessageOption deploy_option(sender, EV_IMAGE_DEPLOY_ACK, 0, 0);
                rc = m->Send(deploy_ack, deploy_option);
            }
            else
            {//反向成功
                rc = Transaction::Begin();
                if(SUCCESS != rc)
                {
                    SkyAssert(false);
                    break;
                }
                rc = DeleteImageBase(labels._base_uuid);
                if(SUCCESS != rc)
                {
                    SkyAssert(false);
                    Transaction::Cancel();
                    break;
                }
                rc = FinishAction(ack.action_id);
                if(SUCCESS != rc)
                {
                    SkyAssert(false);
                    Transaction::Cancel();
                    break;
                }
                rc = DeleteUuid(labels._base_uuid);
                if(SUCCESS != rc)
                {
                    SkyAssert(false);
                    Transaction::Cancel();
                    break;
                }
                rc = Transaction::Commit();
                if(SUCCESS != rc)
                {
                    SkyAssert(false);
                    Transaction::Cancel();
                    break;
                }
                //发撤销成功应答
                CancelBaseImageAck cancel_ack(upstream_action_id, labels._base_uuid, labels._cluster_name);
                MessageOption cancel_option(sender, EV_IMAGE_CANCEL_ACK, 0, 0);
                cancel_ack.state = SUCCESS;
                cancel_ack.progress = 100;
                rc = m->Send(cancel_ack, cancel_option);
            }
            break;
        }
        case ERROR_ACTION_RUNNING:
        {
            OutPut(SYS_DEBUG,"action %s is already running, ok!\n",ack.action_id.c_str());
            rc = SUCCESS;
            break;
        }
        default:
        {
            OutPut(SYS_ERROR,"action %s failed!\n",ack.action_id.c_str());
            rc = RestartAction(ack.action_id);
            break;
        }
    }

    return rc;
}
/******************************************************************************/
STATUS ImageAgent::HandleImageUrlPrepareReq(const MessageFrame &message)
{
    ImageUrlPrepareReq req;
    if(!req.deserialize(message.data))
    {
        OutPut(SYS_ERROR,"Handle Image Url Prepare Request Deserialize Failed\n");
        return ERROR;
    }
    STATUS ret = SUCCESS;
    ImageUrlPrepareAck ack(req.action_id, req._request_id, req._url);
    switch (req._position)
    {
        case IMAGE_POSITION_LOCAL:
        {
            ack.state = SUCCESS;
            ack.progress = 100;
            break;
        }
        case IMAGE_POSITION_SHARE:
        {
            if (IMAGE_USE_SNAPSHOT == req._option._share_img_usage)
            {
                ret = GetImageBaseByImageid(req._image_id,ack._base_uuid_vec,ack._size);
                if (SUCCESS == ret)
                {
                    ack.state = SUCCESS;
                    ack.progress = 100;
                }
                else
                {
                    if (ERROR_OBJECT_NOT_EXIST == ret)
                    {
                        ack.detail = "Prepare image url failed for base not exist!";
                    }
                    else
                    {
                        ack.detail = "Prepare image url failed for db failed!";
                    }
                    ack.state = ERROR;
                    ack.progress = 0;
                }
            }
            else
            {
                ack.state = SUCCESS;
                ack.progress = 100;
            }
            break;
        }
        default:
        {
            ack.detail = "Prepare image url failed for unknown position("+to_string<int>(req._position,dec)+")";
            ack.state = ERROR;
            ack.progress = 0;
            break;
        }
    }
    
    MessageOption ack_option(message.option.sender(), EV_IMAGE_URL_PREPARE_ACK, 0, 0);
    return m->Send(ack, ack_option);
}

}


