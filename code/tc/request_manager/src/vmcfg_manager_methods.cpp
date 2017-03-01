/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：vmcfg_manager_methods.cpp
* 文件标识：
* 内容摘要：VmCfg模块XML-RPC注册方法类的实现文件
* 当前版本：1.0
* 作    者：Bob
* 完成日期：2011年9月5日
*
* 修改记录1：
*    修改日期：2011/9/5
*    版 本 号：V1.0
*    修 改 人：Bob
*    修改内容：创建
*******************************************************************************/
#include "vmcfg_manager_methods.h"
#include "mid.h"
#include "event.h"
#include "sky.h"
#include "vm_messages.h"
#include "authrequest.h"
#include "usergroup.h"
#include "db_config.h"
#include "log_agent.h"
#include "vmcfg_operate.h"
#include "license_common.h"
#include "image_db.h"
#include "image_url.h"
#include "vmtemplate_pool.h"

static int vcm_print = 0;
DEFINE_DEBUG_VAR(vcm_print);
#define Debug(fmt,arg...) \
        do \
        { \
            if(vcm_print) \
            { \
                OutPut(SYS_DEBUG,"[%s:%d]"fmt,__func__,__LINE__, ##arg);\
            } \
        }while(0)

namespace zte_tecs
{


#define EV_QUERY_TIMEOUT          EV_TIMER_1
#define EV_VM_SYN_QUERY_TIMEOUT   EV_TIMER_2


//const int  k_vmcfg_deploy_wait_time = (VM_DEPLOY_DURATION_TC + 2 )* 1000; // 2小时 + 6S
const int  k_vmcfg_deploy_wait_time = (VM_OTHER_OP_DURATION_TC + 2)*1000; // 10S + 6S 调小，提高用户体验
const int  k_vmcfg_cancel_wait_time = (VM_CANCEL_DURATION_TC + 2)*1000; // 3 分钟  + 6S
const int  k_vmcfg_action_wait_time = (VM_OTHER_OP_DURATION_TC + 2)*1000; // 10S + 6S
int   k_vmcfg_query_wait_time  = (VM_QUERY_DURATION_TC*10 + 70 +2)*100; // 10.2 S
const int  k_vmcfg_usb_query_wait_time  = (VM_QUERY_DURATION_TC*20 + 2)*100; //6.2 S


static STATUS VmCfgInit(VmCfgOperate   *op_inst,
                           ostringstream  &err_oss)
{
    op_inst = VmCfgOperate::GetInstance();
    if (NULL == op_inst)
    {
        err_oss<< "Get op_inst instance failed.";
        return ERROR;
    }

    STATUS ret = Transaction::Enable(GetDB());
    if (SUCCESS != ret)
    {
        err_oss << "Enable transaction failed.";
        return ERROR;
    }

    return SUCCESS;
}

#define VC_INIT(op_inst, err_oss)\
        do \
        {\
            if (SUCCESS != VmCfgInit(op_inst,err_oss )) \
            { \
                xRET2(xINT(RpcError(ERROR)),xSTR(err_oss.str())); \
                return ;\
            }\
        }while (0)\


/******************************************************************************/

/******************************************************************************/
void VmCfgAllocateMethod::MethodEntry(const paramList& paramList,value* const retval)
{
    ostringstream    oss;
    string           error_str;
    VmCfgOperate *op_inst = NULL;

    /* 1. 获取参数 */
    int64   uid = get_userid();

    ApiVmCfgAllocateData  allocate_data;

    allocate_data.from_rpc(paramList.getStruct(1));
    bool  success = allocate_data.Validate(error_str);
    if (!success)//非法参数
    {
        xRET2(xINT(RpcError(ERROR_INVALID_ARGUMENT)),xSTR(error_str));
        return ;
    }

    if (ValidateLicense(LICOBJ_CREATED_VMS, allocate_data.vm_num) != 0)
    {
        xRET2(xINT(TECS_ERR_LICENSE_INVALID),xSTR(error_str));
        return ;
    }

    /* 只有管理员才能设置cluster和host */
    if (!allocate_data.cfg_info.base_info.cluster.empty() || !allocate_data.cfg_info.base_info.host.empty())
    {
        User *p = NULL;
        p    = _upool->Get(uid,false);
        if (NULL == p)
        {
            oss << "Error, only administrators can appoint cluster and host!";
            Debug("%s\n",oss.str().c_str());
            xRET2(xINT(RpcError(ERR_AUTHORIZE_FAILED)),xSTR(oss.str()));
            return;
        }

        int role = p->get_role();
        if (UserGroup::CLOUD_ADMINISTRATOR != role)
        {
            oss << "Error, only administrators can appoint cluster and host!";
            Debug("%s\n",oss.str().c_str());
            xRET2(xINT(RpcError(ERR_AUTHORIZE_FAILED)),xSTR(oss.str()));
            return;
        }
    }

    /* todo: 只有管理员才能设置disk position */
    Debug("\n\n++++%s\n",allocate_data.toString().c_str());

    vector<int64>  vids;

    VC_INIT(op_inst, oss);
    int ret_code = op_inst->DealDirectCreate(allocate_data, uid, vids,oss);

    Debug("%s : ret_code(%d), result: %s\n",
          _method_name.c_str(),
          ret_code,
          oss.str().c_str());

    if (ret_code != SUCCESS)
    {
        xRET2(xINT(RpcError(ret_code)),xSTR(oss.str()));
        return;
    }
    else
    {
        //xRET2(xINT(RpcError(ret_code)),xI8(vid));

        vector<value>                      array_vids;
        vector<int64>::const_iterator   it;
        for (it = vids.begin(); it != vids.end();++it)
        {
            array_vids.push_back(xI8(*it));
        }
        xRET2(xINT(RpcError(ret_code)),xARRAY(array_vids));

        return;
    }

    return;
}
/******************************************************************************/
void VmCfgAllocateByVtMethod::MethodEntry(const paramList& paramList,value* const retval)
{
    ApiVmCfgAllocateByVt  allocate_info;
    ostringstream         oss;
    string                error_str;
    VmTemplate            vt;
    ApiVtInfo             vt_info;
    VmCfgOperate         *op_inst = NULL;
    VmTemplatePool       *vtpool = NULL;

    int64   uid = get_userid();

    allocate_info.from_rpc(paramList.getStruct(1));
    bool  success = allocate_info.Validate(error_str);
    if (!success)//非法参数
    {
        xRET2(xINT(RpcError(ERROR_INVALID_ARGUMENT)),xSTR(error_str));
        return ;
    }

    if (ValidateLicense(LICOBJ_CREATED_VMS, allocate_info.vm_num) != 0)
    {
        xRET2(xINT(TECS_ERR_LICENSE_INVALID),xSTR(error_str));
        return ;
    }

    Debug("\n\n++++%s\n",allocate_info.toString().c_str());

    /*获取模版信息*/
    vtpool  = VmTemplatePool::GetInstance();
    if(NULL == vtpool)
    {
        oss << "Error, get vmtemplate fail!";
        Debug("%s\n",oss.str().c_str());
        xRET2(xINT(RpcError(ERROR)),xSTR(oss.str()));
        return ;
    }

    STATUS tRet = vtpool->GetVmTemplate(vt,allocate_info.vt_name);
    if(SUCCESS != tRet)
    {
        oss<<"vmtemplate ("<< allocate_info.vt_name <<") not exist.";
        Debug("%s\n",oss.str().c_str());
        xRET2(xINT(RpcError(ERR_OBJECT_NOT_EXIST)),xSTR(oss.str()));
    }

    if( SUCCESS!=vtpool->ToApiInfo(vt,vt_info))
    {
        oss<<"get vmtemplate ("<< allocate_info.vt_name <<") fail.";
        Debug("%s\n",oss.str().c_str());
        xRET2(xINT(RpcError(ERROR)),xSTR(oss.str()));
    }

    /*非管理员不指定部署*/
    User *p = NULL;
    p    = _upool->Get(uid,false);
    if (NULL == p)
    {
        oss << "Error, only administrators can appoint cluster and host!";
        Debug("%s\n",oss.str().c_str());
        xRET2(xINT(RpcError(ERR_AUTHORIZE_FAILED)),xSTR(oss.str()));
        return;
    }

    int role = p->get_role();
    if (UserGroup::CLOUD_ADMINISTRATOR != role)
    {
        vt_info.cfg_info.base_info.cluster = "";
        vt_info.cfg_info.base_info.host= "";

        if(uid != vt.get_uid())
        {
            oss << "Error, only administrators can use others' template!";
            xRET2(xINT(RpcError(ERR_AUTHORIZE_FAILED)),xSTR(oss.str()));
            return;
        }
    }

    vector<int64>  vids;
    ApiVmCfgAllocateData  allocate_data;

    allocate_data.cfg_info.base_info = vt_info.cfg_info.base_info;
    allocate_data.cfg_info.vm_name = vt_info.vt_name;
    allocate_data.cfg_info.project_name = allocate_info.project_name;
    allocate_data.vm_num = allocate_info.vm_num;

    VC_INIT(op_inst, oss);
    int ret_code = op_inst->DealDirectCreate(allocate_data, uid, vids,oss);

    Debug("%s : ret_code(%d), result: %s\n",
          _method_name.c_str(),
          ret_code,
          oss.str().c_str());

    if (ret_code != SUCCESS)
    {
        xRET2(xINT(RpcError(ret_code)),xSTR(oss.str()));
        return;
    }
    else
    {
        //xRET2(xINT(RpcError(ret_code)),xI8(vid));

        vector<value>                      array_vids;
        vector<int64>::const_iterator   it;
        for (it = vids.begin(); it != vids.end();++it)
        {
            array_vids.push_back(xI8(*it));
        }
        xRET2(xINT(RpcError(ret_code)),xARRAY(array_vids));

        return;
    }

}

/******************************************************************************/
void VmCfgCloneMethod::MethodEntry(const paramList& paramList,value* const retval)
{
    ostringstream    oss;
    VmCfgOperate *op_inst = NULL;
    int          clone_num = 1;
    int64        clone_vid = -1;
    STATUS       tRet     = ERROR;
    int          ret_code = ERROR;
    QuerySession   query_session;
    VmCfg       vmcfg;
    int          role = 0;
    bool         is_admin = false;
    /* 1. 获取参数 */
    int64   uid = get_userid();
    int64   vid = paramList.getI8(1);

    VmCfgPool  *vcpool = VmCfgPool::GetInstance();
    if(NULL == vcpool)
    {
        oss<<"Get vm pool instance failed.";
        Debug("%s\n",oss.str().c_str());
        xRET2(xINT(ERROR),xSTR(oss.str()));
        return ;
    }
    tRet = vcpool->GetVmById(vmcfg, vid);
    if (SUCCESS!= tRet)
    {
        oss<<"VM ("<< vid <<") not exist.";
        Debug("%s\n",oss.str().c_str());
        xRET2(xINT(ERR_OBJECT_NOT_EXIST),xSTR(oss.str()));
        return ;
    }

    if(uid != vmcfg.get_uid())
    {
        oss<<"Current user can not clone other user's vm.";
        Debug("%s\n",oss.str().c_str());
        xRET2(xINT(ERR_AUTHORIZE_FAILED),xSTR(oss.str()));
        return ;
    }

    if (ValidateLicense(LICOBJ_CREATED_VMS, clone_num) != 0)
    {
        oss<<"license is invalid.";
        Debug("%s\n",oss.str().c_str());
        xRET2(xINT(TECS_ERR_LICENSE_INVALID),xSTR(oss.str()));
        return ;
    }

    role = _upool->Get(uid,false)->get_role();
    is_admin = (UserGroup::CLOUD_ADMINISTRATOR == role)?true: false;

    ApiVmCfgQuery  api_req;
    api_req.start_index = 0;
    api_req.count       = 1;
    api_req.type        = RPC_QUERY_SELF_APPOINTED;
    api_req.appointed_vid  = vid;


    VC_INIT(op_inst, oss);
    tRet = op_inst->DealQuery(api_req,
                               uid,
                               is_admin,
                               query_session,
                               oss);
    /* 3. 查询失败，直接返回  */
    if (SUCCESS != tRet)
    {
        ret_code = tRet;
        xRET2(xINT(RpcError(ret_code)),xSTR(oss.str()));
        Debug("\n%s: tRet = %d,  err_str = %s\n",
               _method_name.c_str(),
               ret_code,
               oss.str().c_str());
        return;
    }

    if ( 0 != query_session.wait_cc_msgs)
    {
        MessageUnit    temp_mu(TempUnitName("tecs_vmcfg_clone"));
        temp_mu.Register();
        query_session.agent_mid = temp_mu.GetSelfMID();

        SendQueryMsg(query_session,temp_mu);
        oss.str("");
        tRet = WaitQueryAckMsg(query_session, temp_mu, oss);
        if (SUCCESS != tRet)
        {
            ret_code = tRet;
            xRET2(xINT(RpcError(ret_code)),xSTR(oss.str()));
            Debug("\n%s: tRet = %d,  err_str = %s\n",_method_name.c_str(),ret_code,oss.str().c_str());
            return;
        }
    }

    if(query_session._vc_info.size()!=1)
    {
        oss<<"query base vm fail.";
        xRET2(xINT(RpcError(ERROR)),xSTR(oss.str()));
        Debug("\nuery base vm fail, ret vm size = %d\n",query_session._vc_info.size());
        return;
    }

    ret_code = op_inst->DealClone(query_session._vc_info.front(),uid,clone_vid,oss);

    Debug("%s : ret_code(%d), result: %s, new vid :%d\n",
          _method_name.c_str(),
          ret_code,
          oss.str().c_str(),clone_vid);
    if (ret_code != SUCCESS)
    {
        xRET2(xINT(RpcError(ret_code)),xSTR(oss.str()));
        return;
    }
    else
    {
        xRET2(xINT(RpcError(ret_code)),xI8(clone_vid));
        return;
    }
}

/******************************************************************************/
void VmStaticInfoQueryMethod::MethodEntry(const paramList& paramList,value* const retval)
{
    ostringstream  err_oss;
    string         error_str;
    STATUS         tRet     = ERROR;
    int            ret_code = ERROR;
    VmCfgOperate *op_inst = NULL;
    int          role = 0;
    bool         is_admin = false;
    int64 uid    =  get_userid();
    QuerySession   query_session;
    MessageUnit    temp_mu(TempUnitName("tecs_vmcfg_query"));

    /* 1. 获取参数 */
    ApiVmCfgQuery  api_req;
    api_req.start_index = xI8(paramList.getI8(1));
    api_req.count       = xI8(paramList.getI8(2));
    api_req.type        = paramList.getString(3);
    api_req.appointed_vid  = xI8(paramList.getI8(4));

    bool  success = api_req.Validate(error_str);
    if (!success)//非法参数
    {
        err_oss << error_str ;
        ret_code = TECS_ERR_PARA_INVALID;
        goto QUERY_ERROR ;
    }

    role = _upool->Get(uid,false)->get_role();
    is_admin = (UserGroup::CLOUD_ADMINISTRATOR == role)?true: false;

    /* 2. 查询VM */
    VC_INIT(op_inst, err_oss);
    tRet = op_inst->DealQuery(api_req,
                               uid,
                               is_admin,
                               query_session,
                               err_oss);
    /* 3. 查询失败，直接返回  */
    if (SUCCESS != tRet)
    {
        ret_code = tRet;
        goto QUERY_ERROR ;
    }
    else
    {
        goto QUERY_SUCCESS;
    }

QUERY_ERROR:
    xRET2(xINT(RpcError(ret_code)),xSTR(err_oss.str()));
    Debug("\n%s: tRet = %d,  err_str = %s\n",
               _method_name.c_str(),
               ret_code,
               err_oss.str().c_str());
    return;

QUERY_SUCCESS:
    vector<value> array_vms;
    vector<ApiVmStaticInfo>::const_iterator    iter_in;
    for (iter_in = query_session._vc_info.begin();
            iter_in != query_session._vc_info.end();
            ++iter_in)
    {
        array_vms.push_back(iter_in->to_rpc());
        Debug("\n %s:\n%s\n\n", _method_name.c_str(),iter_in->toString().c_str());
    }

    xRET4(xINT(RpcError(SUCCESS)),
          xI8(query_session.next_index),
          xI8(query_session.max_count),
          xARRAY(array_vms));
    Debug("\n%s: tRet = 0,  max_count = %d, next_index = %d\n",
               _method_name.c_str(),
               query_session.max_count,
               query_session.next_index);
    return;
}


/******************************************************************************/
void VmRunningInfoQueryMethod::MethodEntry(const paramList& paramList,value* const retval)
{
    ostringstream  err_oss;
    string         error_str;
    STATUS         tRet     = ERROR;
    int            ret_code = ERROR;
    VmCfgOperate *op_inst = NULL;
    int          role = 0;
    bool         is_admin = false;
    int64 uid    =  get_userid();
    QuerySession   query_session;
    MessageUnit    temp_mu(TempUnitName("tecs_vmcfg_query"));

    /* 1. 获取参数 */
    ApiVmCfgQuery  api_req;
    api_req.start_index = xI8(paramList.getI8(1));
    api_req.count       = xI8(paramList.getI8(2));
    api_req.type        = paramList.getString(3);
    api_req.appointed_vid  = xI8(paramList.getI8(4));

    bool  success = api_req.Validate(error_str);
    if (!success)//非法参数
    {
        err_oss << error_str ;
        ret_code = TECS_ERR_PARA_INVALID;
        goto QUERY_ERROR ;
    }

    role = _upool->Get(uid,false)->get_role();
    is_admin = (UserGroup::CLOUD_ADMINISTRATOR == role)?true: false;

    /* 2. 查询VM */
    VC_INIT(op_inst, err_oss);
    tRet = op_inst->DealQuery(api_req,
                               uid,
                               is_admin,
                               query_session,
                               err_oss);
    /* 3. 查询失败，直接返回  */
    if (SUCCESS != tRet)
    {
        ret_code = tRet;
        goto QUERY_ERROR ;
    }



    /* 4. 无需向CC查状态? */
    if ( 0 == query_session.wait_cc_msgs)
    {
        goto QUERY_SUCCESS;
    }

    /* 5. 注册临时消息单元 */
    temp_mu.Register();
    query_session.agent_mid = temp_mu.GetSelfMID();

    /* 6. 发消息给 CC*/
    SendQueryMsg(query_session,temp_mu);

    /* 7. 等等所有CC的应答  */
    err_oss.str("");
    tRet = WaitQueryAckMsg(query_session, temp_mu, err_oss);
    if (SUCCESS != tRet)
    {
        ret_code = tRet;
        goto QUERY_ERROR;
    }
    else
    {
        goto QUERY_SUCCESS;
    }

    return;

QUERY_ERROR:
    xRET2(xINT(RpcError(ret_code)),xSTR(err_oss.str()));
    Debug("\n%s: tRet = %d,  err_str = %s\n",
               _method_name.c_str(),
               ret_code,
               err_oss.str().c_str());
    return;

QUERY_SUCCESS:

    vector<value> array_vms;
    vector<ApiVmRunningInfo>::const_iterator    iter_in;
    for (iter_in = query_session._vc_runinfo.begin();
            iter_in != query_session._vc_runinfo.end();
            ++iter_in)
    {
        
        array_vms.push_back(iter_in->to_rpc());
        
        Debug("\n %s:\n%s\n\n", _method_name.c_str(),iter_in->toString().c_str());
    }

    xRET4(xINT(RpcError(SUCCESS)),
          xI8(query_session.next_index),
          xI8(query_session.max_count),
          xARRAY(array_vms));
    Debug("\n%s: tRet = 0,  max_count = %d, next_index = %d\n",
               _method_name.c_str(),
               query_session.max_count,
               query_session.next_index);
    return;
}


/******************************************************************************/
void  VmCfgQueryMethod::SendQueryMsg(const QuerySession &query_session,
                                                       MessageUnit &mu)
{
    string  stamp = mu.GetSelfMID();

    //下发给cc的消息
    MID             cc_mid(PROC_CC, MU_VM_MANAGER);
    MessageOption   cc_option(cc_mid, EV_VM_QUERY, 0,0);
    map<string, ClusterVmRunningInfo>::const_iterator  it_cluster;
    vector<VmRunningInfo>::const_iterator              it_vm;
    for (it_cluster = query_session._cluster_vcs.begin();
         it_cluster != query_session._cluster_vcs.end();
            it_cluster++ )
    {
        vector<int64>         vids;
        ClusterVmRunningInfo  cluster_vm_info(it_cluster->second);
        string cluster_name = it_cluster->first;

        SkyAssert(!cluster_name.empty());
        vids.clear();
        for (it_vm = cluster_vm_info._vm_running_info.begin();
                it_vm != cluster_vm_info._vm_running_info.end();
                it_vm++)
        {
            vids.push_back(it_vm->_vid);
        }

        VmQueryReq      req_msg(stamp, vids);

        cc_mid._application = cluster_name;
        cc_option.setReceiver(cc_mid);

        mu.Send(req_msg, cc_option);
    }

     //下发给vnm的消息
    MID             vnm_mid(MU_VNET_LIBNET);
    MessageOption   vnm_option(vnm_mid, EV_VNETLIB_QUERY_VNICS , 0,0);
    vector<int64>::const_iterator              it_vid;
    for (it_vid  = query_session._need_query_network_vids.begin();
         it_vid != query_session._need_query_network_vids.end();
         it_vid++ )
    {
        CQueryVmNicInfoMsg req(*it_vid);
        mu.Send(req, vnm_option);
    }

    return ;

}

/******************************************************************************/
STATUS VmCfgQueryMethod::WaitQueryAckMsg(QuerySession &query_session,
        MessageUnit &temp_mu,
        ostringstream &err_oss)
{
    STATUS tRet = ERROR;

    /* 1. 设置一个终极等待超时的定时器 */
    TIMER_ID timer_id = temp_mu.CreateTimer();
    if (INVALID_TIMER_ID == timer_id)
    {
        err_oss<< "Create timer failed";
        return ERROR;
    }

    TimeOut timeout(EV_QUERY_TIMEOUT,k_vmcfg_query_wait_time);
    tRet = temp_mu.SetTimer(timer_id,timeout);
    if (SUCCESS != tRet)
    {
        err_oss<< "Set timer failed";
        return ERROR;
    }

    /* 2. 等待所有CC的回应 */
    MessageFrame ack;

    while (1)
    {
        if ((0 >= query_session.wait_cc_msgs)
             &&(0 >= query_session.wait_vnm_msgs))
        {
            //VNM和所有CC均已反馈，直接返回
            temp_mu.KillTimer(timer_id);
            return SUCCESS;
        }

        //因为已设终极定时器，这里可以一直等下去
        tRet = temp_mu.Wait(&ack, WAIT_FOREVER);
        if (SUCCESS != tRet)
        {
            OutPut(SYS_DEBUG,"mu(%s) WaitClusterAckMsg wait failed\n",
                    temp_mu.GetSelfMID().c_str());
            err_oss<<"Failed to wait CC or VNM ack\n";
            temp_mu.KillTimer(timer_id);
            return ERROR_TIME_OUT;
        }

        if (ack.option.id() == EV_QUERY_TIMEOUT)
        {
            OutPut(SYS_DEBUG,"mu(%s) WaitClusterAckMsg: rcv time out msg\n",
                    temp_mu.GetSelfMID().c_str() );
            err_oss<<"Query VM state failed, time out\n";
            temp_mu.KillTimer(timer_id);
            return ERROR_TIME_OUT;
        }
        else if(EV_VM_QUERY_ACK == ack.option.id()) // 状态的
        {
            VmQueryAck  cc_ack;
            cc_ack.deserialize(ack.data);

            MID  sender= ack.option.sender();
            string cc_name = sender._application;
            Debug(" rcv vm state ack form cluster(%s):\n%s\n",
                   cc_name.c_str(),
                   cc_ack.serialize().c_str());
            DealStateQueryAck(cc_ack,query_session,temp_mu);
        }
        else if (EV_VNETLIB_QUERY_VNICS_ACK == ack.option.id())// 网卡的
        {
            CQueryVmNicInfoMsgAck  vnm_ack;
            vnm_ack.deserialize(ack.data);

            Debug(" rcv vm nics ack: \n %s\n",
                  vnm_ack.serialize().c_str());
            DealVmNicQueryAck(vnm_ack,query_session,temp_mu);
        }
        else
        {
            //收到的不是我们真正要等的消息
            OutPut(SYS_DEBUG,"WaitQueryAckMsg: Unknown message! id = %d\n",\
                   ack.option.id());
            continue;
        }

    }

    temp_mu.KillTimer(timer_id);
    return SUCCESS;
}

/******************************************************************************/
void VmCfgQueryMethod::DealStateQueryAck(VmQueryAck  &ack,
                                                 QuerySession &query_session,
                                                 MessageUnit &mu)
{
    map<string, ClusterVmRunningInfo>::iterator  it_cluster;
    vector<VmRunningInfo>::const_iterator   it_ack_vm;

    string  agent_mid    = ack._stamp;
    MID     cc_mid       = mu.CurrentMessageFrame()->option.sender();
    string  cluster_name = cc_mid._application;

    /* 查找会话 */

    if (query_session.agent_mid != ack._stamp)//会话不存在
    {
        OutPut(SYS_INFORMATIONAL,"*** VM Query ack error: session(%s) not exist ***\n"
               "*** msg from cluster(%s), _result(%d), _error(%s) ***\n",
               agent_mid.c_str(),
               cluster_name.c_str(),
               ack._result,
               ack._error.c_str());
        return ;
    }

    /* 查找集群 */
    it_cluster = query_session._cluster_vcs.find(cluster_name);
    if (it_cluster == query_session._cluster_vcs.end())//集群不存在
    {
        OutPut(SYS_INFORMATIONAL, "*** VM state query ack error: cluster(%s) not exist ***\n"
               "*** session(%s), msg info: _result(%d), _error(%s) ***\n",
               cluster_name.c_str(),
               agent_mid.c_str(),
               ack._result,
               ack._error.c_str());
        return ;
    }

    if (it_cluster->second._is_ack)// 集群已应答
    {
        OutPut(SYS_INFORMATIONAL, "*** VM state query ack error: cluster(%s) has acked ***\n"
               "*** session(%s), msg info : _result(%d), _error(%s) ***\n",
               cluster_name.c_str(),
               agent_mid.c_str(),
               ack._result,
               ack._error.c_str() );
        return ;
    }

    it_cluster->second._is_ack = true;
    query_session.wait_cc_msgs --;

    //该CC上查询不成功，VM状态直接填默认值
    if ((SUCCESS != ack._result)
        &&(0 != query_session.wait_cc_msgs))
    {
        return ;
    }

    VmCfgPool  *vcpool = VmCfgPool::GetInstance();
    if(NULL == vcpool)
    {
        return ;
    }

    /* 查找VM */
    for (it_ack_vm  = ack._running_info.begin();
            it_ack_vm != ack._running_info.end();
            it_ack_vm ++ )
    {
        vector<ApiVmRunningInfo>::iterator       it_vm;
        for (it_vm  = query_session._vc_runinfo.begin();
             it_vm != query_session._vc_runinfo.end();
                it_vm++ )
        {
            if (it_vm->vid == it_ack_vm->_vid)
            {
                it_vm->computationinfo.state    = it_ack_vm->_state; //更新vm状态
                it_vm->computationinfo.vnc_ip   = it_ack_vm->_vnc_out_ip;
                it_vm->computationinfo.vnc_port = it_ack_vm->_vnc_port;
                it_vm->computationinfo.serial_port = it_ack_vm->_serial_port;
                it_vm->computationinfo.cpu_rate = it_ack_vm->_cpu_rate;
                it_vm->computationinfo.memory_rate= it_ack_vm->_memory_rate;
                it_vm->netinfo.use_rate = it_ack_vm->_nic_rate;

                if (VM_CONFIGURATION == it_ack_vm->_state)//该虚拟机不在该CC内
                {
                     break;
                }
                else
                {
                    if (query_session.is_admin)//仅对管理员显示虚拟机的位置信息
                    {
                        it_vm->computationinfo.deployed_cluster = cluster_name;
                        it_vm->computationinfo.deployed_hc = it_ack_vm->_hc_name;
                    }
                }
                break;
            }
        }

    }

    return ;

}

/******************************************************************************/
void VmCfgQueryMethod::DealVmNicQueryAck(CQueryVmNicInfoMsgAck  &ack,
                                                  QuerySession &query_session,
                                                  MessageUnit &mu)
 {
    vector<int64>::iterator  it_vid;
    /* 查找虚拟机 */
    for( it_vid  = query_session._need_query_network_vids.begin();
         it_vid != query_session._need_query_network_vids.end();
         it_vid++)
                    {
        if(*it_vid == ack.m_vm_id)
                        {
           break;
        }
    }

    if (it_vid == query_session._need_query_network_vids.end())// 虚拟机 不存在
                            {
        OutPut(SYS_INFORMATIONAL, "*** VM nics query ack error: vid(%d) not exist ***\n"
               "*** msg info: state(%d), detail(%s) ***\n",
               *it_vid,
               ack.state,
               ack.detail.c_str());
        return ;
                            }

    query_session.wait_vnm_msgs --;

    //该虚拟机网卡查询不成功
    if ((SUCCESS != ack.state)
        &&(0 != query_session.wait_cc_msgs))
    {
        return ;
                        }

    vector<ApiVmRunningInfo>::iterator  it_vm;
    vector<CNicInfo>::iterator      it_vnet_nic;
    vector<ApiVmNic>::iterator      it_nic;;
    for (it_vm  = query_session._vc_runinfo.begin();
         it_vm != query_session._vc_runinfo.end();
         it_vm++ )
    {
        //找到虚拟机
        if (it_vm->vid == *it_vid)
        {
            for(it_vnet_nic = ack.m_infos.begin();
                it_vnet_nic!= ack.m_infos.end();
                it_vnet_nic++)
            {
                for(it_nic = it_vm->netinfo.nic_info.begin();
                    it_nic != it_vm->netinfo.nic_info.end();
                    it_nic++)
                {
                     // 找到网卡
                     if(it_nic->nic_index == it_vnet_nic->m_nNicIndex)
                     {
                         it_nic->vsi_profile_id = it_vnet_nic->m_strVSIProfileID;
                         it_nic->pci_order      = it_vnet_nic->m_strPCIOrder;
                         it_nic->bridge_name    = it_vnet_nic->m_strBridgeName;
                         it_nic->ip             = it_vnet_nic->m_strIP;
                         it_nic->netmask        = it_vnet_nic->m_strNetmask;
                         it_nic->gateway        = it_vnet_nic->m_strGateway;
                         it_nic->mac            = it_vnet_nic->m_strMac;
                         it_nic->c_vlan         = it_vnet_nic->m_nCvlan;
                    }
                }
            }

        }

    }

    return ;
}

/******************************************************************************/
void VmCfgGetIdByNameMethod::MethodEntry(const paramList& paramList,value* const retval)
{
    string           err_str;
    VmCfgOperate *op_inst = NULL;

    ApiGetVidByName  api_req;

    /* 1. 获取参数 */
    api_req.query_scope = xI8(paramList.getI8(1));
    api_req.vm_name = xSTR(paramList.getString(2));
    api_req.vm_user = xSTR(paramList.getString(3));

    // api_req.from_rpc(paramList.getStruct(1));
    bool  success = api_req.Validate(err_str);
    if (!success)//非法参数
    {
        xRET2(xINT(RpcError(ERROR_INVALID_ARGUMENT)),xSTR(err_str));
        return ;
    }

    int64 uid      =  get_userid();
    int   role     =  _upool->Get(uid,false)->get_role();
    bool  is_admin =  (role== UserGroup::CLOUD_ADMINISTRATOR)?true:false;
    vector<int64>    vids;
    ostringstream    err_oss;

    VC_INIT(op_inst, err_oss);
    STATUS tRet = op_inst->QueryByName(api_req,uid, is_admin,vids, err_oss);
    if (SUCCESS != tRet)
    {
        xRET2(xINT(RpcError(tRet)),xSTR(err_oss.str()));
        return;
    }
    else
    {
        vector<value> array_vids;
        vector<int64>::iterator it;
        for (it = vids.begin(); it != vids.end(); it++)
        {
            array_vids.push_back(xI8(*it));
        }
        xRET2(xINT(RpcError(SUCCESS)),xARRAY(array_vids));
    }

    return;
}

/******************************************************************************/
void VmCfgActionMethod::MethodEntry(const paramList& paramList,value* const retval)
{
    /* 1. 获取参数 */
    int64  uid         = get_userid();
    string session     = paramList.getString(0);
    int64  vid         = paramList.getI8(1);
    string action_name = paramList.getString(2);

    ostringstream    err_oss;
    VmOperation      action_type;
    VmCfgOperate    *op_inst = NULL;
    string           workflow_id;
    string           workflow_engine;

    /* 2. 检查权限 */
    VC_INIT(op_inst, err_oss);
    STATUS tRet = op_inst->CheckAction(uid, vid,action_name,action_type, err_oss);
    if (SUCCESS != tRet)
    {
        xRET2(xINT(RpcError(tRet)),xSTR(err_oss.str()));
        return ;
    }
    err_oss.str("");
    if (DEPLOY == action_type)
    {
        tRet = DealOpDeploy(vid, workflow_id, err_oss,workflow_engine);
    }
    else if (CANCEL == action_type)
    {
        tRet = DealOpCancel(vid, workflow_id, err_oss,workflow_engine);
    }
    else if (UPLOAD_IMG == action_type)
    {
        tRet = DealOpUpLoadImg(vid, session, workflow_id, err_oss,workflow_engine);
    }
    else if (DELETE == action_type)
    {
        tRet = op_inst->DealDelete(vid, uid, err_oss);
    }
    else if (SYNC == action_type)
    {
        tRet = DealOpSync(vid, workflow_id, err_oss);
    }
    else
    {
        tRet = DealOpNormal(vid, action_type, workflow_id, err_oss,workflow_engine);
    }

    if(SUCCESS == tRet)
    {
        Debug("\n %s, tRet = %d, workflow_id = %s, target = %s\n",
              _method_name.c_str(),
              tRet, workflow_id.c_str(),workflow_engine.c_str());
        xRET3(xINT(RpcError(tRet)),xSTR(workflow_id),xSTR(workflow_engine));
    }
    else
    {
        Debug("\n %s, tRet = %d, err_str= %s\n",
              _method_name.c_str(),
              tRet, err_oss.str().c_str());
        xRET2(xINT(RpcError(tRet)),xSTR(err_oss.str()));
    }

    return;
}
/******************************************************************************/
STATUS VmCfgActionMethod::DealOpDeploy(int64         vid, string& workflow_id,
                                        ostringstream   &err_oss,
                                        string &workflow_engine)
{
    VmCfg       vmcfg;

    VmCfgPool  *vcpool = VmCfgPool::GetInstance();
    if(NULL == vcpool)
    {
        err_oss<<"Get Pool instance failed.";
        return ERROR;
    }
    STATUS tRet = vcpool->GetVmById(vmcfg, vid);
    if (SUCCESS!= tRet)
    {
        err_oss<<"VM ("<< vid <<") not exist.";
        return ERR_OBJECT_NOT_EXIST;
    }

    tRet = vcpool->CheckImageIdByVid(vid);
    if (SUCCESS != tRet)
    {
        err_oss<<"Image file of VM ("<< vid <<") not exist.";
        return ERR_IMAGE_FILE_NOT_EXIST;
    }

    MessageUnit    temp_mu(TempUnitName("tecs_vmcfg_deploy"));
    temp_mu.Register();

    VmOperationReq  req_msg(vid,(int)DEPLOY, temp_mu.GetSelfMID());

    MID            dest(PROC_TC, MU_VMCFG_MANAGER);
    MessageOption  option(dest, EV_VM_OP, 0, 0);

    int32   wait_time = 0;
    wait_time = k_vmcfg_action_wait_time;

    temp_mu.Send(req_msg, option);

    /*  等待TC回应 */
    MessageFrame  msg;
    if (SUCCESS == temp_mu.Wait(&msg, wait_time))
    {
        /*  收到应答，针对之前消息的应答 ?  */
        Debug("\n*** %s : rcv vm deploy ack msg:\n  %s \n ***\n",
                _method_name.c_str(),msg.data.c_str());
        VmOperationAck  tc_ack;
        tc_ack.deserialize(msg.data);

        if (tc_ack.state == SUCCESS || tc_ack.state == ERROR_ACTION_RUNNING) //成功
        {
            err_oss<<"Success to create task to Deploy VM ("<< vid <<")";
            workflow_engine = tc_ack._workflow_engine;
            workflow_id = tc_ack._workflow_id;
            return SUCCESS;
        }
        else  //失败
        {
            err_oss << tc_ack.detail;
            OutPut(SYS_NOTICE," Rcv create %s workflow of VM(%d) failed: %s \n",
                   Enum2Str::VmOpStr(DEPLOY),
                   vid,
                   tc_ack.detail.c_str());
            return tc_ack.state;
        }
    }
    else
    {
        /*  超时 or 其他错误 ? */
        Debug("%s : time out\n",_method_name.c_str());
        err_oss <<_method_name <<" time out";
        return ERROR_TIME_OUT;
    }
    return SUCCESS;
}

/******************************************************************************/
STATUS VmCfgActionMethod::DealOpCancel(int64         vid, string& workflow_id,
                                            ostringstream   &err_oss,
                                            string  &workflow_engine)
{
    VmCfg       vmcfg;

    VmCfgPool  *vcpool = VmCfgPool::GetInstance();
    if(NULL == vcpool)
    {
        err_oss<<"Get Pool instance failed.";
        return ERROR;
    }
    STATUS tRet = vcpool->GetVmById(vmcfg, vid);
    if (SUCCESS!= tRet)
    {
        err_oss<<"VM ("<< vid <<") not exist.";
        return ERR_OBJECT_NOT_EXIST;
    }

    MessageUnit    temp_mu(TempUnitName("tecs_vmcfg_cancel"));
    temp_mu.Register();

    VmOperationReq  req_msg(vid,(int)CANCEL, temp_mu.GetSelfMID());

    MID            dest(PROC_TC, MU_VMCFG_MANAGER);
    MessageOption  option(dest, EV_VM_OP, 0, 0);

    temp_mu.Send(req_msg, option);

    int32   wait_time = 0;
    wait_time = k_vmcfg_cancel_wait_time;

    /*  等待TC回应 */
    MessageFrame  msg;
    if (SUCCESS == temp_mu.Wait(&msg, wait_time))
    {
        /*  收到应答，针对之前消息的应答 ?  */
        Debug("\n*** %s : rcv vm cancel ack msg:\n  %s \n ***\n",
                _method_name.c_str(),msg.data.c_str());
        VmOperationAck  tc_ack;
        tc_ack.deserialize(msg.data);

        if ((SUCCESS == tc_ack.state )
            ||(ERROR_ACTION_RUNNING == tc_ack.state) ) // 成功
        {
            err_oss<<"Success to create task to Cancel VM ("<<tc_ack._vid <<")";
            workflow_engine = tc_ack._workflow_engine;
            workflow_id = tc_ack._workflow_id;
            return SUCCESS;
        }
        else  //失败
        {
            err_oss << tc_ack.detail;
            OutPut(SYS_NOTICE," Rcv create %s workflow of VM(%d) failed: %s \n",
                   Enum2Str::VmOpStr(CANCEL),
                   vid,
                   tc_ack.detail.c_str());
            return tc_ack.state;
        }
    }
    else
    {
        /*  超时 or 其他错误 ? */
        Debug("%s : time out\n",_method_name.c_str());
        err_oss <<_method_name <<" time out";
        return ERROR_TIME_OUT;
    }
    return SUCCESS;
}


/******************************************************************************/
STATUS VmCfgActionMethod::DealOpSync(int64         vid, string& workflow_id,
                                     ostringstream   &err_oss)
{
    VmCfg  vmcfg;
    string   cluster_name;

    VmCfgOperate *op_inst = NULL;
    STATUS tRet = VmCfgInit(op_inst,err_oss );
    if (SUCCESS != tRet)
    {
       return tRet;
    }

    tRet = op_inst->GetVmcfg(vid,  vmcfg, cluster_name, err_oss);
    if (SUCCESS != tRet)
    {
        return tRet;
    }

    return SendSync2Cluster(vmcfg, cluster_name, workflow_id, err_oss);
}

/******************************************************************************/
STATUS VmCfgActionMethod::DealOpUpLoadImg(int64         vid,
                                                  const string  &session,
                                                  string& workflow_id,
                                                  ostringstream &err_oss,
                                                  string &workflow_engine)
{
    VmCfg  vmcfg;
    string   cluster_name;

    VmCfgOperate *op_inst = NULL;
    STATUS tRet = VmCfgInit(op_inst,err_oss );
    if (SUCCESS != tRet)
    {
       return tRet;
    }

    tRet = op_inst->GetVmcfg(vid,  vmcfg, cluster_name, err_oss);
    if (SUCCESS != tRet)
    {
        return tRet;
    }

    // 1. 获取地址
    UploadUrl  img_url;
    tRet = GetUpLoadUrl(img_url, err_oss);
    if (SUCCESS!= tRet)
    {
        return tRet;
    }

    // 对ack消息中的空用户名和空密码进行处理
    // 当使用空用户名的时候，该参数由RPC接口填写
    if (img_url.user.empty())
    {
        img_url.user = MAGIC_USER;
    }
    if (img_url.passwd.empty())
    {
        img_url.passwd = session;
    }

    // 2. 下发消息给CC
    err_oss.str("");
    tRet = SendUpLoadImg2Cluster(vid, img_url,cluster_name, err_oss,workflow_id,workflow_engine);
    if (SUCCESS!= tRet)
    {
        return tRet;
    }

    return SUCCESS;

}



/******************************************************************************/
STATUS VmCfgActionMethod::DealOpNormal(int64  vid,
                                 VmOperation   op, string& workflow_id,
                                 ostringstream &err_oss,
                                 string &workflow_engine)
{
    VmCfg  vmcfg;
    string   cluster_name;

    VmCfgOperate *op_inst = NULL;
    STATUS tRet = VmCfgInit(op_inst,err_oss );
    if (SUCCESS != tRet)
    {
       return tRet;
    }

    tRet = op_inst->GetVmcfg(vid,  vmcfg, cluster_name, err_oss);
    if (SUCCESS != tRet)
    {
        return tRet;
    }

    /* 2. 创建临时消息单元 */
    MessageUnit    temp_mu(TempUnitName("tecs_vmcfg_action"));
    temp_mu.Register();

    /* 3. 下发消息给CC */
    VmOperationReq  req_msg(vid,(int)op, temp_mu.GetSelfMID(), vmcfg._user_name);

    MID             req_mid(cluster_name, PROC_CC, MU_VM_MANAGER);
    MessageOption   req_option(req_mid, EV_VM_OP, 0,0);

    temp_mu.Send(req_msg, req_option);

    int32   wait_time = 0;
    if (op == CANCEL)
    {
        wait_time = k_vmcfg_cancel_wait_time;
    }
    else
    {
        wait_time = k_vmcfg_action_wait_time;
    }

    /* 4. 等待CC回应 */
    MessageFrame  msg;
    if (SUCCESS == temp_mu.Wait(&msg, wait_time))
    {
        /*  收到应答，针对之前消息的应答 ?  */
        Debug("\n*** %s : rcv vm op ack msg:\n  %s \n ***\n",
                _method_name.c_str(),msg.data.c_str());
        VmOperationAck  cc_ack;
        cc_ack.deserialize(msg.data);
        workflow_engine = cc_ack._workflow_engine;
        STATUS  tRet = DealClusterAck(cc_ack,cluster_name, workflow_id, err_oss);
        return tRet;
    }
    else
    {
        /*  超时 or 其他错误 ? */
        Debug("%s : time out\n",_method_name.c_str());
        err_oss <<_method_name <<" time out";
        return ERROR_TIME_OUT;
    }
}


/******************************************************************************/
STATUS VmCfgActionMethod::SendUpLoadImg2Cluster(int64 vid,
                                                          const UploadUrl &img_url,
                                                          const string    &cluster,
                                                          ostringstream   &err_oss,
                                                          string &workflow_id,
                                                          string &workflow_engine)
{
    /* 1. 创建临时消息单元 */
    MessageUnit temp_mu(TempUnitName("tecs_vmcfg_uploadimg"));
    temp_mu.Register();

    // 发送消息给CC
    VmSaveReq     save_req;

    save_req._url = img_url;
    // 获取虚拟机要保存的映像名称
    // 使用的格式vm+id+copy+uuid,eg: vm_1_copy_uuid.img
    save_req._name = "vm_" + to_string<int64>(vid,dec) + "_copy_" \
                     + GenerateUUID()+".img";
    // 填充虚拟机操作结构的信息
    save_req._vid       = vid;
    save_req._operation = UPLOAD_IMG;
    save_req._stamp     = temp_mu.GetSelfMID();

    // 发送消息给CC
    MID            req_mid(cluster, PROC_CC, MU_VM_MANAGER);
    MessageOption  req_option(req_mid, EV_VM_OP, 0,0);
    temp_mu.Send(save_req, req_option);

    // 等CC的应答
    MessageFrame  msg;
    if (SUCCESS != temp_mu.Wait(&msg, k_vmcfg_query_wait_time))
    {
        /*  超时 or 其他错误 ? */
        Debug("%s : wait CC ack time out\n",_method_name.c_str());
        err_oss <<_method_name <<": wait CC ack time out";
        return ERROR_TIME_OUT;
    }

    // 以下注释为svn 32809节点修改，直接改为调用DealClusterAck接口处理结果
    VmOperationAck optack;
    optack.deserialize(msg.data);
    //workflow_id = optack._workflow_id;
    workflow_engine = optack._workflow_engine;
    Debug("%s : VmOperationAck: %s\n",_method_name.c_str(), msg.data.c_str());
    err_oss << optack.detail;
    //if (optack.state == SUCCESS || optack.state == ERROR_ACTION_RUNNING)
    //{
    //    return SUCCESS;
    //}

    //return optack.state;
    return DealClusterAck(optack,cluster, workflow_id, err_oss);
}

/******************************************************************************/
STATUS VmCfgActionMethod::SendSync2Cluster(VmCfg    &vmcfg,
                                                    const string  &cluster_name,
                                                    string& workflow_id,
                                                    ostringstream &err_oss)
{
    /* 1. 构造消息 */
    err_oss.str("");
    VMDeployInfo  req_msg;
    VmCfgOperate *op_inst = NULL;
    STATUS tRet = VmCfgInit(op_inst,err_oss );
    if (SUCCESS != tRet)
    {
       return tRet;
    }
    tRet = op_inst->PrepareSynMsg(vmcfg,
                                      "",
                                      req_msg,
                                      err_oss);
    if (SUCCESS != tRet)
    {
        return tRet;
    }

    /* 获取coredump_url并填入到deployinfo */
    if (vmcfg.is_enable_coredump())
    {
        /* 发送消息给imagemage 获取URL路径 */
        UploadUrl  coredump_url;
        tRet = GetCoredumpUrl(coredump_url, err_oss);
        if (SUCCESS!= tRet)
        {
            return tRet;
        }
        req_msg._config._core_dump_url = coredump_url;
    }

    /* 2. 创建临时消息单元 */
    MessageUnit    temp_mu(TempUnitName("tecs_vmcfg_syn"));
    temp_mu.Register();

    req_msg._stamp = temp_mu.GetSelfMID();
    MID            dest_mid(cluster_name, PROC_CC, MU_VM_MANAGER);
    MessageOption  req_option(dest_mid, EV_VMCFG_SYNC_REQ, 0, 0);

    Debug("*** Send VM (%d) SYN msg to cluster (%s) success!***\n",
             req_msg._config._vid, cluster_name.c_str());

    Debug("*** the msg content :\n %s \n", req_msg.serialize().c_str());


    temp_mu.Send(req_msg, req_option);

    /* 3. 等待CC回应 */
    MessageFrame  msg;
    if (SUCCESS == temp_mu.Wait(&msg, k_vmcfg_action_wait_time))
    {
        /*  收到应答，针对之前消息的应答 ?  */
        Debug("\n*** %s : rcv vm syn ack msg:\n  %s \n ***\n",
                _method_name.c_str(),msg.data.c_str());
        VmOperationAck  cc_ack;
        cc_ack.deserialize(msg.data);

        STATUS  tRet = DealClusterAck(cc_ack,cluster_name, workflow_id, err_oss);
        return tRet;
    }
    else
    {
        /*  超时 or 其他错误 ? */
        Debug("%s : time out\n",_method_name.c_str());
        err_oss <<_method_name <<" time out";
        return ERROR_TIME_OUT;
    }
}

/******************************************************************************/
STATUS VmCfgActionMethod::GetUpLoadUrl(
                                 UploadUrl  &img_url,
                                 ostringstream &err_oss)
{
    /* 1. 创建临时消息单元 */
    MessageUnit temp_mu(TempUnitName("tecs_vmcfg_get_imgurl"));
    temp_mu.Register();

    // 向img模块发送文件上传请求
    MessageOption option(MID(PROC_IMAGE_MANAGER, MU_IMAGE_MANAGER),
                         EV_IMAGE_GET_URL_REQ,0,0);
    temp_mu.Send("", option);

    // 等img模块的应答
    MessageFrame message;
    if (SUCCESS != temp_mu.Wait(&message, k_vmcfg_query_wait_time))
    {
        /*  超时 or 其他错误 ? */
        OutPut(SYS_NOTICE,"In method %s, Get ImgUrl time out\n",_method_name.c_str());
        err_oss <<_method_name <<" Get ImgUrl time out";
        return ERROR_TIME_OUT;
    }

    // 获取img的地址
    ImageUploadAck ack;
    ack.deserialize(message.data);
    if (SUCCESS != ack._result)
    {
        /*  超时 or 其他错误 ? */
        OutPut(SYS_NOTICE,"%s : Get ImgUrl fail\n",_method_name.c_str());
        err_oss <<_method_name <<" Get ImgUrl fail";
        return ERROR;
    }

    img_url = ack._url;

    return SUCCESS;
}

/******************************************************************************/
STATUS VmCfgActionMethod::GetCoredumpUrl(
                                 UploadUrl  &core_dump_url,
                                 ostringstream &err_oss)
{
    /* 1. 创建临时消息单元 */
    MessageUnit temp_mu(TempUnitName("tecs_vmcfg_get_imgurl"));
    temp_mu.Register();

    // 向file模块发送文件上传请求
    MessageOption option(MID(PROC_FILE_MANAGER, MU_FILE_MANAGER),
                         EV_FILE_GET_URL_REQ,0,0);
    temp_mu.Send("", option);

    // 等file模块的应答
    MessageFrame message;
    if (SUCCESS != temp_mu.Wait(&message, k_vmcfg_query_wait_time))
    {
        /*  超时 or 其他错误 ? */
        OutPut(SYS_NOTICE,"In method %s, Get FileUrl time out\n",_method_name.c_str());
        err_oss <<_method_name <<" Get FileUrl time out";
        return ERROR_TIME_OUT;
    }

    // 获取img的地址
    UploadUrl ack;
    ack.deserialize(message.data);
    core_dump_url = ack;

    return SUCCESS;
}

 /******************************************************************************/
STATUS VmCfgActionMethod::DealClusterAck(VmOperationAck  &ack,
                                              const string    &cc_name,
                                              string& workflow_id,
                                              ostringstream   &err_oss)
{
    VmCfg  vmcfg;

    VmCfgPool  *vcpool = VmCfgPool::GetInstance();
	if(NULL == vcpool)
	{
	    err_oss<<"Get VmCfgPool instance failed.";
	    return ERROR;
	}

    switch (static_cast<VmOperation>(ack._operation))
    {
    // 不会受到DEPLOY、CANCEL操作了
    case CREATE:
    case START:
    case STOP:
    case REBOOT:
    case PAUSE:
    case RESUME:
    case RESET:
    case DESTROY:
    case CANCEL_UPLOAD_IMG:
    case RECOVER_IMAGE:
    case FREEZE:
    case UNFREEZE:
	case UPLOAD_IMG:
    {
        if (ack.state == SUCCESS || ack.state == ERROR_ACTION_RUNNING) //成功
        {
            OutPut(SYS_NOTICE," Rcv cluster ack of %s of VM(%d) success: %s \n",
                   Enum2Str::VmOpStr(ack._operation),
                   vmcfg.get_vid(),
                   ack.detail.c_str());
            workflow_id = ack._workflow_id;
            return SUCCESS;
        }
        else  //失败
        {
            err_oss << ack.detail;
            OutPut(SYS_NOTICE," Rcv cluster ack of %s of VM(%d) failed: %s \n",
                   Enum2Str::VmOpStr(ack._operation),
                   vmcfg.get_vid(),
                   ack.detail.c_str());
            return ack.state;
        }
        break;
    }
    case SYNC:
    {
        err_oss <<ack.detail;
        return ack.state;
        break;
    }
    default:
    {
        OutPut(SYS_DEBUG," Rcv unsupport vm operation %s ack in timerout msg.\n",
               Enum2Str::VmOpStr(ack._operation));
        break;
    }
    }

    return SUCCESS;
}

/******************************************************************************/
void VmCfgSetMethod::MethodEntry(const paramList& paramList,
                                      value* const retval)
{
    string         error_str;
    ostringstream  err_oss;
    VmCfgOperate  *op_inst = NULL;

    /* 1. 获取参数 */
    int64   uid        = get_userid();

    ApiVmCfgModifyData  modify_data;
    modify_data.from_rpc(paramList.getStruct(1));

    bool  success = modify_data.Validate(error_str);
    if (!success)//非法参数
    {
        xRET2(xINT(TECS_ERR_PARA_INVALID),xSTR(error_str));
        return ;
    }

    /* 只有管理员才能设置cluster和host */
    if (!modify_data.cfg_info.base_info.cluster.empty() || !modify_data.cfg_info.base_info.host.empty())
    {
        if (_upool->Get(uid,false)->get_role() != UserGroup::CLOUD_ADMINISTRATOR)
        {
            err_oss << "Error, only administrators can modify cluster and host!";
            Debug("%s\n",err_oss.str().c_str());
            xRET2(xINT(TECS_ERR_AUTHORIZE_FAILED),xSTR(err_oss.str()));
            return;
        }
    }

    VC_INIT(op_inst, err_oss);
    STATUS tRet = op_inst->DealModify(modify_data, uid, err_oss);

    Debug("\n %s, tRet = %d, err_str= %s\n",
          _method_name.c_str(),
          tRet, err_oss.str().c_str());
    xRET2(xINT(RpcError(tRet)),xSTR(err_oss.str()));

    return;
}

/******************************************************************************/
void VmCfgModifyMethod::MethodEntry(const paramList& paramList,
                                      value* const retval)
{
    ostringstream  err_oss;

    ApiVmCfgBatchModify  modify_data;
    modify_data.from_rpc(paramList.getStruct(1));

    int64   uid = get_userid();

    MessageUnit    temp_mu(TempUnitName("tecs_vmcfg_modify"));
    temp_mu.Register();

    VmCfgBatchModify  req_msg(modify_data, uid);

    MID            dest(PROC_TC, MU_VMCFG_MANAGER);
    MessageOption  option(dest, EV_VMCFG_MODIFY_REQ, 0, 0);

    temp_mu.Send(req_msg, option);

    /*  等待TC回应 */
    MessageFrame  msg;
    if (SUCCESS == temp_mu.Wait(&msg, k_vmcfg_action_wait_time))
    {
        /*  收到应答，针对之前消息的应答 ?  */
        Debug("\n*** %s : rcv vm modify ack msg:\n  %s \n ***\n",
                _method_name.c_str(),msg.data.c_str());
        VmOperationAck  tc_ack;
        tc_ack.deserialize(msg.data);

        if (tc_ack.state == SUCCESS || tc_ack.state == ERROR_ACTION_RUNNING) //成功
        {
            err_oss<<"Success to create task to modify VM ("<< tc_ack._vid <<")";
            xRET2(xINT(RpcError(SUCCESS)),xSTR(tc_ack._workflow_id));
        }
        else  //失败
        {
            err_oss << tc_ack.detail;
            OutPut(SYS_NOTICE," Rcv create modify workflow of VM(%d) failed: %s \n",
                   tc_ack._vid,
                   tc_ack.detail.c_str());
            xRET2(xINT(RpcError(tc_ack.state)),xSTR(err_oss.str()));
        }
    }
    else
    {
        /*  超时 or 其他错误 ? */
        Debug("%s : time out\n",_method_name.c_str());
        err_oss <<_method_name <<" time out";
        xRET2(xINT(RpcError(ERROR_TIME_OUT)),xSTR(err_oss.str()));
        return;
    }

    return;
}

/******************************************************************************/
void VmCfgSetDiskMethod::MethodEntry(
    const paramList& paramList,
    value* const retval)
{
    vector<value>       array_disk;
    ostringstream       err_oss;
    string              err_str;
    VmCfgOperate *op_inst = NULL;

    /* 1. 获取参数 */
    int64  uid  = get_userid();
    int64  vid  = paramList.getI8(1);
    array_disk  = value_array(paramList.getArray(2)).vectorValueValue();

    vector<VmBaseDisk> vec_disk;
    vector<value>::iterator iter;
    for (iter = array_disk.begin();
            iter != array_disk.end();
            ++iter)
    {
        ApiVmDisk     api_disk;
        api_disk.from_rpc(value_struct(*iter));
        if (false == api_disk.Validate(err_str))
        {
            xRET2(xINT(TECS_ERR_PARA_INVALID),xSTR(err_str));
            return;
        }

        VmBaseDisk vm_base_disk(INVALID_OID,
                                api_disk.target,
                                api_disk.position,
                                api_disk.bus,
                                api_disk.size,
                                api_disk.fstype);
        vec_disk.push_back(vm_base_disk);
    }

    VC_INIT(op_inst, err_oss);
    STATUS tRet = op_inst->SetDisks(vec_disk,
                                  vid,
                                     uid,
                                     err_oss);

    xRET2(xINT(RpcError(tRet)),xSTR(err_oss.str()));
    return;
}

/******************************************************************************/
void VmCfgSetImageDiskMethod :: MethodEntry(
    const paramList& paramList,
    value* const retval)
{
    vector<value>  array_image;
    ostringstream  err_oss;
    string         err_str;
    VmCfgOperate  *op_inst = NULL;

    /* 1. 获取参数 */
    int64  uid       = get_userid();
    int64  vid  = paramList.getI8(1);
    array_image = value_array(paramList.getArray(2)).vectorValueValue();

    vector<VmBaseImage> vec_image;
    vector<value>::iterator iter;
    for (iter = array_image.begin();
            iter != array_image.end();
            ++iter)
    {
        ApiVmDisk     api_disk;

        api_disk.from_rpc(value_struct(*iter));
        if (false == api_disk.Validate(err_str))
        {
            xRET2(xINT(TECS_ERR_PARA_INVALID),xSTR(err_str));
            return;
        }
        VmBaseImage   vm_base_image(INVALID_OID,
                                    api_disk.target,
                                    api_disk.position,
                                    api_disk.id,
                                    api_disk.reserved_backup,
                                    api_disk.type);

        vec_image.push_back(vm_base_image);
    }

    VC_INIT(op_inst, err_oss);
    STATUS tRet = op_inst->SetImageDisks(vec_image,
                                vid,
                                          uid,
                                          err_oss);

    xRET2(xINT(RpcError(tRet)),xSTR(err_oss.str()));
    return;
}

/******************************************************************************/
void VmCfgSetNicMethod :: MethodEntry(
    const paramList& paramList,
    value* const retval)
{
    vector<value>       array_nic;
    ostringstream       err_oss;
    string              error_str;
    VmCfgOperate *op_inst = NULL;

    /* 1. 获取参数 */
    int64  uid  = get_userid();
    int64  vid  = paramList.getI8(1);
    array_nic   = value_array(paramList.getArray(2)).vectorValueValue();

    vector<value>::iterator iter;
    vector<VmBaseNic>       vec_nic;
    int          num = 1;

    for (iter = array_nic.begin();
            iter != array_nic.end();
            ++iter, ++num)
    {
        ApiVmNic    api_nic;
        api_nic.from_rpc(value_struct(*iter));

        bool success = api_nic.Validate(error_str);
        if (!success)//非法参数
        {
            err_oss <<"The "<<num <<"th nic invalid,"<<error_str;
            xRET2(xINT(RpcError(ERROR_INVALID_ARGUMENT)),xSTR(err_oss.str()));
            return ;
        }

        VmBaseNic   vm_base_nic(INVALID_OID,
                                api_nic.nic_index,
                                api_nic.sriov,
                                api_nic.loop,
                                api_nic.logic_network_id,
                                api_nic.model,
                                "");

        vec_nic.push_back(vm_base_nic);

    }

    VC_INIT(op_inst, err_oss);
    STATUS tRet = op_inst->SetNics(vec_nic,
                                vid,
                                    uid,
                                    err_oss);

    xRET2(xINT(RpcError(tRet)),xSTR(err_oss.str()));
    return;
}


/******************************************************************************/
void VmCfgRelationMethod::MethodEntry(const paramList& paramList,
                                      value* const retval)
{
    ostringstream  err_oss;
    VmCfgOperate  *op_inst = NULL;

    /* 1. 获取参数 */
    int64   uid = get_userid();

    ApiVmCfgRelationData  api_data;
    api_data.from_rpc(paramList.getStruct(1));

    VC_INIT(op_inst, err_oss);
    string wf_id;
    STATUS tRet = op_inst->SetVmRelation( api_data,uid, err_oss, wf_id);

    if (tRet == ERROR_ACTION_RUNNING)
    {
        xRET2(xINT(RpcError(SUCCESS)),xSTR(wf_id));
    }
    else
    {
        xRET2(xINT(RpcError(tRet)),xSTR(err_oss.str()));
    }

    return;

}

/******************************************************************************/
void VmCfgRelationQueryMethod::MethodEntry(const paramList& paramList,
        value* const retval)
{
    ostringstream    err_oss;
    string           error_str;

    /* 1. 获取参数 */
    int64   uid = get_userid();
    int64   vid = paramList.getI8(1);

    vector<ApiVmCfgMap> vec_vm;
    VmCfgOperate *op_inst = NULL;

    VC_INIT(op_inst, err_oss);
    STATUS  tRet = op_inst->DealQueryVmRelation(vid, uid, vec_vm, err_oss);

    Debug("\n%s : tRet = %d, err_oss = %s\n",
          _method_name.c_str(),
          tRet,
          err_oss.str().c_str());

    if (SUCCESS != tRet)
    {
        xRET2(xINT(RpcError(tRet)),xSTR(err_oss.str()));
        return;
    }
    else
    {
        vector<value>                      array_vms;
        vector<ApiVmCfgMap>::const_iterator   iter_in;
        for (iter_in  = vec_vm.begin();
                iter_in != vec_vm.end();
                ++iter_in)
        {
            array_vms.push_back(iter_in->to_rpc());

            Debug("\n ++\n%s \n ++\n",
                  iter_in->toString().c_str());
        }
        xRET2(xINT(RpcError(tRet)),xARRAY(array_vms));
    }

    return ;

}

/******************************************************************************/
void VmCfgAffinityRegionAddMethod::MethodEntry(const paramList& paramList,
                                      value* const retval)
{
    ostringstream  err_oss;
    VmCfgOperate  *op_inst = NULL;

    /* 1. 获取参数 */
    int64   uid = get_userid();

    ApiAffinityRegionMemberAction  api_data;
    api_data.from_rpc(paramList.getStruct(1));

    VC_INIT(op_inst, err_oss);
    STATUS tRet = op_inst->AddVmAffinityRegion( api_data,uid, err_oss);

    xRET2(xINT(RpcError(tRet)),xSTR(err_oss.str()));

    return;

}

/******************************************************************************/
void VmCfgAffinityRegionDelMethod::MethodEntry(const paramList& paramList,
                                      value* const retval)
{
    ostringstream  err_oss;
    VmCfgOperate  *op_inst = NULL;

    /* 1. 获取参数 */
    int64   uid = get_userid();

    ApiAffinityRegionMemberAction  api_data;
    api_data.from_rpc(paramList.getStruct(1));

    VC_INIT(op_inst, err_oss);
    STATUS tRet = op_inst->DelVmAffinityRegion( api_data,uid, err_oss);

    xRET2(xINT(RpcError(tRet)),xSTR(err_oss.str()));

    return;

}
/******************************************************************************/
void VmCfgMigrateMethod::MethodEntry(const paramList& paramList,
                                          value* const retval)
{
    ostringstream     err_oss;
    VmOperation       action_type;
    string            cluster_name;
    string            mu_name;

    VmCfgOperate *op_inst = NULL;
    VC_INIT(op_inst, err_oss);

    /* 1. 获取参数 */
    ApiVmCfgMigrate  lm_data;
    lm_data.from_rpc(paramList.getStruct(1));
    Debug("%s : rcv migrate msg for vm:%lld to host(%lld), forced = %d, bLive = %d!\n",
          _method_name.c_str(), lm_data.vid, lm_data.hid, lm_data.bForced,  lm_data.bLive);

    /* 2. 仅管理员有权限 */
    int64  uid = get_userid();
    STATUS tRet = op_inst->CheckOpMigrate(uid, lm_data.vid, lm_data.bLive, cluster_name, err_oss);
    if (SUCCESS != tRet)
    {
        xRET2(xINT(RpcError(tRet)),xSTR(err_oss.str()));
        return ;
    }

    if(lm_data.bLive)
    {
       action_type = LIVE_MIGRATE;
       mu_name     = "vm_live_migrate";
    }
    else
    {
       action_type = COLD_MIGRATE;
       mu_name     = "vm_cold_migrate";
    }

    /* 4. 发消息给 CC  */
    MessageUnit temp_mu(TempUnitName(mu_name));
    temp_mu.Register();
    {
        MID           req_mid(cluster_name, PROC_CC, MU_VM_MANAGER );
        MessageOption req_option(req_mid, EV_VM_OP, 0, 0 );

        VmMigrateReq req_msg(lm_data.vid,
                             lm_data.hid,
                             lm_data.bForced,
                             action_type,
                             temp_mu.GetSelfMID());

        temp_mu.Send(req_msg, req_option);
    }

    /* 5. 等待CC回应 */
    MessageFrame      message;
    if (SUCCESS == temp_mu.Wait(&message,k_vmcfg_action_wait_time))
    {
        /*  收到应答，针对之前消息的应答 ?  */
        Debug("%s : rcv ack msg success!\n", _method_name.c_str());
        VmOperationAck  ack_message;
        ack_message.deserialize(message.data);

        //ack_message.Print();
        Debug("%s \n", ack_message.serialize().c_str());

        /* 正在做和已成功都直接返回成功 */
        if ((ack_message.state == SUCCESS)
           || (ack_message.state == ERROR_ACTION_RUNNING)) //成功
        {
            xRET2(xINT(RpcError(SUCCESS)),xSTR(ack_message.detail));
        }
        else
        {
            xRET2(xINT(RpcError(ack_message.state)),xSTR(ack_message.detail));
        }
        return;
    }
    else
    {
        /*  超时 or 其他错误 ? */
        Debug("%s : time out\n",_method_name.c_str());
        err_oss <<_method_name <<" time out";
        xRET2(xINT(RpcError(ERROR_TIME_OUT)),xSTR(err_oss.str()));
        return;
    }

    return;
}


/******************************************************************************/
void VmCfgUsbDeviceQueryMethod::MethodEntry(const paramList& paramList,
        value* const retval)
{
    ostringstream    err_oss;
    string           error_str;
    VmUsbListAck  usb_info_ack_from_cc;

    /* 1. 获取参数 */
    int64   vid = paramList.getI8(1);
    int64   uid = get_userid();

    // 检测虚拟机是否已经部署，没有部署，不继续
    VmCfgOperate *op_inst = NULL;
    STATUS tRet = VmCfgInit(op_inst,err_oss );
    if (SUCCESS != tRet)
    {
       xRET2(xINT(RpcError(tRet)),xSTR(err_oss.str()));
       return;
    }

    string cluster_name;
    VmCfg cfg;
    tRet = op_inst->GetVmcfg(vid,  cfg, cluster_name, err_oss);
    if (SUCCESS != tRet)
    {
        xRET2(xINT(RpcError(tRet)),xSTR(err_oss.str()));
        return ;
    }

    // 检测权限
    tRet = op_inst->CheckOpQuery(cfg, uid, err_oss);
    if (SUCCESS != tRet)
    {
        xRET2(xINT(RpcError(tRet)),xSTR(err_oss.str()));
        return ;
    }

    // 发送消息给CC
    MessageUnit    temp_mu(TempUnitName("usb_device_query"));
    temp_mu.Register();
    MID            dest_mid(cluster_name, PROC_CC, MU_VM_MANAGER);
    MessageOption  req_option(dest_mid, EV_VMCFG_USB_INFO_QUERY, 0, 0);

    // 只发送一个消息ID
    ostringstream vid_oss;
    vid_oss<< vid ;
    string query_vid = vid_oss.str();
    temp_mu.Send(query_vid, req_option);

    MessageFrame  msg;
    if (SUCCESS == temp_mu.Wait(&msg, k_vmcfg_usb_query_wait_time))
    {
        usb_info_ack_from_cc.deserialize(msg.data);
        if (usb_info_ack_from_cc._ret != SUCCESS)
        {
            xRET2(xINT(RpcError(usb_info_ack_from_cc._ret)),xSTR("CC return Error"));
            return;
        }
    }
    else
    {
        /*  超时 or 其他错误 ? */
        Debug("%s : time out\n",_method_name.c_str());
        err_oss <<_method_name <<" time out";
        xRET2(xINT(RpcError(ERROR_TIME_OUT)),xSTR(err_oss.str()));
        return;
    }

    vector<ApiVmUsbInfo>  vec_usb;
    vector<ApiVmUsbInfo>::iterator  it;
    vector<value>               array_usb_info;
    usb_info_ack_from_cc.GetApi(vec_usb);
    for (it = vec_usb.begin(); it != vec_usb.end(); it++)
    {
        array_usb_info.push_back(it->to_rpc());
    }
    xRET2(xINT(RpcError(SUCCESS)),xARRAY(array_usb_info));
}


/******************************************************************************/
void VmCfgUsbDeviceOpMethod::MethodEntry(const paramList& paramList,
        value* const retval)
{
    ostringstream    err_oss;
    /* 1. 获取参数 */
    ApiVmUsbOp usb_op;
    int64  uid         = get_userid();
    usb_op.from_rpc(paramList.getStruct(1));

    string           error_str;
    if (!usb_op.validate(error_str))
    {
        xRET2(xINT(RpcError(ERROR_INVALID_ARGUMENT)),xSTR(error_str));
        return;
    }

    /* 2. 检查权限 */
    VmOperation      action_type;
    VmCfgOperate    *op_inst = NULL;
    VC_INIT(op_inst, err_oss);
    STATUS tRet = op_inst->CheckAction(uid, usb_op.op_vid, usb_op.cmd, action_type, err_oss);
    if (SUCCESS != tRet)
    {
        xRET2(xINT(RpcError(tRet)),xSTR(err_oss.str()));
        return ;
    }

    // 发送消息给CC
    string cluster_name;
    tRet = op_inst->GetVmcfg(usb_op.op_vid,  cluster_name, err_oss);
    if (SUCCESS != tRet)
    {
        xRET2(xINT(RpcError(tRet)),xSTR(err_oss.str()));
        return ;
    }
    MessageUnit    temp_mu(TempUnitName("usb_device_op"));
    temp_mu.Register();
    MID            dest_mid(cluster_name, PROC_CC, MU_VM_MANAGER);
    MessageOption  req_option(dest_mid, EV_VM_OP, 0, 0);

    VmUsbOp op_msg(usb_op.op_vid, action_type, usb_op.vendor_id,
                                 usb_op.product_id, usb_op.name, temp_mu.GetSelfMID());
    temp_mu.Send(op_msg, req_option);

    MessageFrame  msg;
    if (SUCCESS == temp_mu.Wait(&msg, k_vmcfg_query_wait_time))
    {
        Debug("%s : rcv ack msg success!\n", _method_name.c_str());
        VmOperationAck  cc_ack;
        cc_ack.deserialize(msg.data);
        xRET2(xINT(RpcError(cc_ack.state)),xSTR(cc_ack.detail));
        return;
    }
    else
    {
        /*  超时 or 其他错误 ? */
        Debug("%s : time out\n",_method_name.c_str());
        err_oss <<_method_name <<" time out";
        xRET2(xINT(RpcError(ERROR_TIME_OUT)),xSTR(err_oss.str()));
    }

}

/******************************************************************************/
void VmCfgAttachPortableDiskMethod::MethodEntry(const paramList& paramList,
        value* const retval)
{
    ostringstream    err_oss;
    /* 1. 获取参数 */
    VmPortableDiskOp para;
    int64  uid = get_userid();
    para._request_id = paramList.getString(1);
    para._vid = paramList.getI8(2);
    para._target = paramList.getString(3);
    para._bus = paramList.getString(4);

    /* 3. 检查参数 */
    if (!StringCheck::CheckNormalName(para._request_id, 1, STR_LEN_64))
    {
       err_oss << "Error, request_id invalid";
       xRET2(xINT(TECS_ERR_PARA_INVALID),xSTR(err_oss.str()));
    }
    if (!StringCheck::CheckNormalName(para._target, 1, STR_LEN_64))
    {
       err_oss << "Error, target invalid";
       xRET2(xINT(TECS_ERR_PARA_INVALID),xSTR(err_oss.str()));
    }
    if (!StringCheck::CheckNormalName(para._bus, 1, STR_LEN_64))
    {
       err_oss << "Error, bus invalid";
       xRET2(xINT(TECS_ERR_PARA_INVALID),xSTR(err_oss.str()));
    }

    string           error_str;

    /* 2. 检查权限 */
    VmOperation     action_type;
    VmCfgOperate    *op_inst = NULL;
    VC_INIT(op_inst, err_oss);
    STATUS tRet = op_inst->CheckAction(uid, para._vid, VM_ACTION_PLUG_IN_PDISK, action_type, err_oss);
    if (SUCCESS != tRet)
    {
        xRET2(xINT(RpcError(tRet)),xSTR(err_oss.str()));
        return ;
    }

    // 发送消息给CC
    string cluster_name;
    VmCfg  cfg;
    tRet = op_inst->GetVmcfg(para._vid, cfg, cluster_name, err_oss);
    if (SUCCESS != tRet)
    {
        xRET2(xINT(RpcError(tRet)),xSTR(err_oss.str()));
        return ;
    }
    //获取vmcfg portdisk

    MessageUnit    temp_mu(TempUnitName("portable_disk_op"));
    temp_mu.Register();
    MID            dest_mid(cluster_name, PROC_CC, MU_VM_MANAGER);
    MessageOption  req_option(dest_mid, EV_VM_OP, 0, 0);

    VmPortableDiskOp op_msg(para._vid, action_type, para._target, para._bus, para._request_id, temp_mu.GetSelfMID(),cfg._user_name);
    temp_mu.Send(op_msg, req_option);

    MessageFrame  msg;
    if (SUCCESS == temp_mu.Wait(&msg, k_vmcfg_query_wait_time))
    {
        Debug("%s : rcv ack msg success!\n", _method_name.c_str());
        VmOperationAck  cc_ack;
        cc_ack.deserialize(msg.data);

        if(cc_ack.state == SUCCESS || cc_ack.state == ERROR_ACTION_RUNNING)
        {
            Debug("\n %s, success to attach disk, workflow_id = %s\n",_method_name.c_str(),cc_ack._workflow_id.c_str());
            xRET3(xINT(RpcError(SUCCESS)),xSTR(cc_ack._workflow_id),xSTR(cc_ack._workflow_engine));
        }
        else
        {
            Debug("\n %s, tRet = %d, err_str= %s\n",
              _method_name.c_str(),tRet, err_oss.str().c_str());
            xRET2(xINT(RpcError(cc_ack.state)),xSTR(cc_ack.detail));
        }
        return;
    }
    else
    {
        /*  超时 or 其他错误 ? */
        Debug("%s : time out\n",_method_name.c_str());
        err_oss <<_method_name <<" time out";
        xRET2(xINT(RpcError(ERROR_TIME_OUT)),xSTR(err_oss.str()));
    }

}

/******************************************************************************/
void VmCfgDetachPortableDiskMethod::MethodEntry(const paramList& paramList,
        value* const retval)
{
    ostringstream    err_oss;
    /* 1. 获取参数 */
    VmPortableDiskOp para;
    int64  uid = get_userid();
    para._vid = paramList.getI8(1);
    para._request_id = paramList.getString(2);

    string           error_str;

    /* 2. 检查权限 */
    VmOperation     action_type;
    VmCfgOperate    *op_inst = NULL;
    VC_INIT(op_inst, err_oss);
    STATUS tRet = op_inst->CheckAction(uid, para._vid, VM_ACTION_PLUG_OUT_PDISK, action_type, err_oss);
    if (SUCCESS != tRet)
    {
        xRET2(xINT(RpcError(tRet)),xSTR(err_oss.str()));
        return ;
    }

    // 发送消息给CC
    string cluster_name;
    VmCfg  cfg;
    tRet = op_inst->GetVmcfg(para._vid, cfg, cluster_name, err_oss);
    if (SUCCESS != tRet)
    {
        xRET2(xINT(RpcError(tRet)),xSTR(err_oss.str()));
        return ;
    }
    //获取vmcfg portdisk

    MessageUnit    temp_mu(TempUnitName("portable_disk_op"));
    temp_mu.Register();
    MID            dest_mid(cluster_name, PROC_CC, MU_VM_MANAGER);
    MessageOption  req_option(dest_mid, EV_VM_OP, 0, 0);

    VmPortableDiskOp op_msg(para._vid, action_type, "", "", para._request_id, temp_mu.GetSelfMID(),cfg._user_name);
    temp_mu.Send(op_msg, req_option);

    MessageFrame  msg;
    if (SUCCESS == temp_mu.Wait(&msg, k_vmcfg_query_wait_time))
    {
        Debug("%s : rcv ack msg success!\n", _method_name.c_str());
        VmOperationAck  cc_ack;
        cc_ack.deserialize(msg.data);

        if(cc_ack.state == SUCCESS || cc_ack.state == ERROR_ACTION_RUNNING)
        {
            Debug("\n %s, success to dettach disk, workflow_id = %s\n",
              _method_name.c_str(),cc_ack._workflow_id.c_str());
            xRET3(xINT(RpcError(SUCCESS)),xSTR(cc_ack._workflow_id),xSTR(cc_ack._workflow_engine));
        }
        else
        {
            Debug("\n %s, tRet = %d, err_str= %s\n",
              _method_name.c_str(),tRet, err_oss.str().c_str());
            xRET2(xINT(RpcError(cc_ack.state)),xSTR(cc_ack.detail));
        }
        return;
    }
    else
    {
        /*  超时 or 其他错误 ? */
        Debug("%s : time out\n",_method_name.c_str());
        err_oss <<_method_name <<" time out";
        xRET2(xINT(RpcError(ERROR_TIME_OUT)),xSTR(err_oss.str()));
    }

}

/******************************************************************************/
void VmCfgImageBackupModifyMethod::MethodEntry(const paramList& paramList,
        value* const retval)
{
    ostringstream    err_oss;
    /* 1. 获取参数 */
    VmImageBackupOp para;
    int64  uid = get_userid();
    para._vid = paramList.getI8(1);
    para._request_id = paramList.getString(2);
    para._description = paramList.getString(3);

    string           error_str;

    /* 2. 检查权限 */
    VmOperation     action_type;
    VmCfgOperate    *op_inst = NULL;
    VC_INIT(op_inst, err_oss);
    STATUS tRet = op_inst->CheckAction(uid, para._vid, VM_ACTION_MODIFY_IMAGE_BACKUP, action_type, err_oss);
    if (SUCCESS != tRet)
    {
        xRET2(xINT(RpcError(tRet)),xSTR(err_oss.str()));
        return ;
    }

    // 发送消息给CC
    string cluster_name;
    tRet = op_inst->GetVmcfg(para._vid,  cluster_name, err_oss);
    if (SUCCESS != tRet)
    {
        xRET2(xINT(RpcError(tRet)),xSTR(err_oss.str()));
        return ;
    }

    MessageUnit    temp_mu(TempUnitName("modify_image_backup_op"));
    temp_mu.Register();
    MID            dest_mid(cluster_name, PROC_CC, MU_VM_MANAGER);
    MessageOption  req_option(dest_mid, EV_VM_OP, 0, 0);

    VmImageBackupOp op_msg(para._vid, action_type, temp_mu.GetSelfMID(), para._request_id, "", para._description);
    temp_mu.Send(op_msg, req_option);

    MessageFrame  msg;
    if (SUCCESS == temp_mu.Wait(&msg, k_vmcfg_query_wait_time))
    {
        Debug("%s : rcv ack msg success!\n", _method_name.c_str());
        VmOperationAck  cc_ack;
        cc_ack.deserialize(msg.data);
        xRET2(xINT(RpcError(cc_ack.state)),xSTR(cc_ack.detail));
        return;
    }
    else
    {
        /*  超时 or 其他错误 ? */
        Debug("%s : time out\n",_method_name.c_str());
        err_oss <<_method_name <<" time out";
        xRET2(xINT(RpcError(ERROR_TIME_OUT)),xSTR(err_oss.str()));
    }

}

/******************************************************************************/
void VmCfgCreateImageBackupMethod::MethodEntry(const paramList& paramList,
        value* const retval)
{
    ostringstream    err_oss;
    /* 1. 获取参数 */
    VmImageBackupOp para;
    int64  uid = get_userid();
    para._vid = paramList.getI8(1);
    para._target = paramList.getString(2);
    para._description= paramList.getString(3);

    /* 3. 检查参数 */
    if (!StringCheck::CheckNormalName(para._target, 1, STR_LEN_64))
    {
       err_oss << "Error, target invalid";
       xRET2(xINT(TECS_ERR_PARA_INVALID),xSTR(err_oss.str()));
    }


    string           error_str;

    /* 2. 检查权限 */
    VmOperation     action_type;
    VmCfgOperate    *op_inst = NULL;
    VC_INIT(op_inst, err_oss);
    STATUS tRet = op_inst->CheckAction(uid, para._vid, VM_ACTION_IMAGE_BACKUP_CREATE, action_type, err_oss);
    if (SUCCESS != tRet)
    {
        xRET2(xINT(RpcError(tRet)),xSTR(err_oss.str()));
        return ;
    }

    // 发送消息给CC
    string cluster_name;
    tRet = op_inst->GetVmcfg(para._vid,  cluster_name, err_oss);
    if (SUCCESS != tRet)
    {
        xRET2(xINT(RpcError(tRet)),xSTR(err_oss.str()));
        return ;
    }
    //获取vmcfg portdisk

    MessageUnit    temp_mu(TempUnitName("image_backup_op"));
    temp_mu.Register();
    MID            dest_mid(cluster_name, PROC_CC, MU_VM_MANAGER);
    MessageOption  req_option(dest_mid, EV_VM_OP, 0, 0);

    VmImageBackupOp op_msg(para._vid, action_type, "", "",para._target,para._description);
    temp_mu.Send(op_msg, req_option);

    MessageFrame  msg;
    if (SUCCESS == temp_mu.Wait(&msg, k_vmcfg_query_wait_time))
    {
        Debug("%s : rcv ack msg success!\n", _method_name.c_str());
        VmOperationAck  cc_ack;
        cc_ack.deserialize(msg.data);
        if(cc_ack.state == SUCCESS || cc_ack.state == ERROR_ACTION_RUNNING)
        {
            Debug("\n %s, success to create image backup, workflow_id = %s\n",
              _method_name.c_str(),cc_ack._workflow_id.c_str());
            xRET3(xINT(RpcError(SUCCESS)),xSTR(cc_ack._workflow_id),xSTR(cc_ack._workflow_engine));
        }
        else
        {
            Debug("\n %s, tRet = %d, err_str= %s\n",
              _method_name.c_str(),tRet, err_oss.str().c_str());
            xRET2(xINT(RpcError(cc_ack.state)),xSTR(cc_ack.detail));
        }

        return;
    }
    else
    {
        /*  超时 or 其他错误 ? */
        Debug("%s : time out\n",_method_name.c_str());
        err_oss <<_method_name <<" time out";
        xRET2(xINT(RpcError(ERROR_TIME_OUT)),xSTR(err_oss.str()));
    }

}

/******************************************************************************/
void VmCfgDeleteImageBackupMethod::MethodEntry(const paramList& paramList,
        value* const retval)
{
    ostringstream    err_oss;
    /* 1. 获取参数 */
    VmImageBackupOp para;
    int64  uid = get_userid();
    para._vid = paramList.getI8(1);
    para._request_id = paramList.getString(2);

    string           error_str;

    /* 2. 检查权限 */
    VmOperation     action_type;
    VmCfgOperate    *op_inst = NULL;
    VC_INIT(op_inst, err_oss);
    STATUS tRet = op_inst->CheckAction(uid, para._vid, VM_ACTION_IMAGE_BACKUP_DELETE, action_type, err_oss);
    if (SUCCESS != tRet)
    {
        xRET2(xINT(RpcError(tRet)),xSTR(err_oss.str()));
        return ;
    }

    // 发送消息给CC
    string cluster_name;
    tRet = op_inst->GetVmcfg(para._vid, cluster_name, err_oss);
    if (SUCCESS != tRet)
    {
        xRET2(xINT(RpcError(tRet)),xSTR(err_oss.str()));
        return ;
    }

    MessageUnit    temp_mu(TempUnitName("image_backup_op"));
    temp_mu.Register();
    MID            dest_mid(cluster_name, PROC_CC, MU_VM_MANAGER);
    MessageOption  req_option(dest_mid, EV_VM_OP, 0, 0);

    VmImageBackupOp op_msg(para._vid, action_type, "", para._request_id,"","" );
    temp_mu.Send(op_msg, req_option);

    MessageFrame  msg;
    if (SUCCESS == temp_mu.Wait(&msg, k_vmcfg_query_wait_time))
    {
        Debug("%s : rcv ack msg success!\n", _method_name.c_str());
        VmOperationAck  cc_ack;
        cc_ack.deserialize(msg.data);
        if(cc_ack.state == SUCCESS || cc_ack.state == ERROR_ACTION_RUNNING)
        {
            Debug("\n %s, success to create image backup, workflow_id = %s\n",
              _method_name.c_str(),cc_ack._workflow_id.c_str());
            xRET3(xINT(RpcError(SUCCESS)),xSTR(cc_ack._workflow_id),xSTR(cc_ack._workflow_engine));
        }
        else
        {
            Debug("\n %s, tRet = %d, err_str= %s\n",
              _method_name.c_str(),tRet, err_oss.str().c_str());
            xRET2(xINT(RpcError(cc_ack.state)),xSTR(cc_ack.detail));
        }
        return;
    }
    else
    {
        /*  超时 or 其他错误 ? */
        Debug("%s : time out\n",_method_name.c_str());
        err_oss <<_method_name <<" time out";
        xRET2(xINT(RpcError(ERROR_TIME_OUT)),xSTR(err_oss.str()));
    }

}

/******************************************************************************/
void VmCfgRestoreImageBackupMethod::MethodEntry(const paramList& paramList,
        value* const retval)
{
    ostringstream    err_oss;
    string           error_str;
    STATUS           tRet;

    /* 1. 获取参数 */
    VmImageBackupOp para;
    int64  uid = get_userid();
    para._vid = paramList.getI8(1);
    para._request_id = paramList.getString(2);
    para._target = paramList.getString(3);

    /* 2. 检查权限 */
    VmOperation     action_type;
    VmCfgOperate    *op_inst = NULL;
    VC_INIT(op_inst, err_oss);
    tRet = op_inst->CheckAction(uid, para._vid, VM_ACTION_IMAGE_BACKUP_RESTORE, action_type, err_oss);
    if (SUCCESS != tRet)
    {
        xRET2(xINT(RpcError(tRet)),xSTR(err_oss.str()));
        return ;
    }

    // 发送消息给CC
    string cluster_name;
    tRet = op_inst->GetVmcfg(para._vid, cluster_name, err_oss);
    if (SUCCESS != tRet)
    {
        xRET2(xINT(RpcError(tRet)),xSTR(err_oss.str()));
        return ;
    }

    MessageUnit    temp_mu(TempUnitName("restore_vm_image"));
    temp_mu.Register();
    MID            dest_mid(cluster_name, PROC_CC, MU_VM_MANAGER);
    MessageOption  req_option(dest_mid, EV_VM_OP, 0, 0);

    VmImageBackupOp op_msg(para._vid, RESTORE_IMAGE_BACKUP, temp_mu.GetSelfMID(), para._request_id, para._target,"");
    temp_mu.Send(op_msg, req_option);

    MessageFrame  msg;
    if (SUCCESS == temp_mu.Wait(&msg, k_vmcfg_action_wait_time))
    {
        Debug("%s : rcv ack msg success!\n", _method_name.c_str());
        VmOperationAck  cc_ack;
        cc_ack.deserialize(msg.data);
        if(cc_ack.state == SUCCESS || cc_ack.state == ERROR_ACTION_RUNNING)
        {
            Debug("\n %s, success to create image backup, workflow_id = %s\n",
              _method_name.c_str(),cc_ack._workflow_id.c_str());
            xRET3(xINT(RpcError(SUCCESS)),xSTR(cc_ack._workflow_id),xSTR(cc_ack._workflow_engine));
        }
        else
        {
            Debug("\n %s, tRet = %d, err_str= %s\n",
              _method_name.c_str(),tRet, err_oss.str().c_str());
            xRET2(xINT(RpcError(cc_ack.state)),xSTR(cc_ack.detail));
        }

        return;
    }
    else
    {
        /*  超时 or 其他错误 ? */
        Debug("%s : time out\n",_method_name.c_str());
        err_oss <<_method_name <<" time out";
        xRET2(xINT(RpcError(ERROR_TIME_OUT)),xSTR(err_oss.str()));
    }

}

void DbgSetVmQryWaitTime(int wt)
{
    if (wt < 3 || wt >60 )
    {
        cout << "Invalid para!,time must is in(3s,60s)" <<endl;
        return;
    }

    k_vmcfg_query_wait_time = wt*1000;
    return;
}
DEFINE_DEBUG_FUNC(DbgSetVmQryWaitTime);

void DbgAttachDisk(int64 vid,const char* cluster, const char* request_id, const char* target, const char* bus)
{
    if (INVALID_IID == vid)
    {
        cout << "Invalid vid " << vid <<endl;
        return;
    }

    if (NULL == request_id || NULL == target || NULL == bus)
    {
        cout << "Invalid para!" <<endl;
        return;
    }

    MessageUnit    temp_mu(TempUnitName("portable_disk_op_dbg"));
    temp_mu.Register();
    MID  dest_mid(cluster, PROC_CC, MU_VM_MANAGER);
    MessageOption  req_option(dest_mid, EV_VM_OP, 0, 0);

    VmPortableDiskOp op_msg(vid, PLUG_IN_PDISK, target, bus, request_id, temp_mu.GetSelfMID(),"admin");
    STATUS ret = temp_mu.Send(op_msg, req_option);
    if (SUCCESS != ret)
    {
        cerr << "Fail to send attach request! ret = " << ret << endl;
    }
    else
    {
        cout << "Attach request send success" << endl;
    }

    MessageFrame message;
    if(SUCCESS == temp_mu.Wait(&message,3000))
    {
        VmOperationAck  ack;
        ack.deserialize(message.data);
        cout << " VmOperationAck: "
             << " vid: " << ack._vid
             << " op:" << ack._operation
             << " state:" << ack._vm_state << endl;
    }
    else
    {
        cout<<"Wait ack timeout"<<endl;
    }
}
DEFINE_DEBUG_FUNC(DbgAttachDisk);

void DbgDetachDisk(int64 vid,const char* cluster, const char* request_id)
{
    if (INVALID_IID == vid)
    {
        cout << "Invalid vid " << vid <<endl;
        return;
    }

    if (NULL == request_id)
    {
        cout << "Invalid para!" <<endl;
        return;
    }

    MessageUnit    temp_mu(TempUnitName("portable_disk_op_dbg"));
    temp_mu.Register();
    MID  dest_mid(cluster, PROC_CC, MU_VM_MANAGER);
    MessageOption  req_option(dest_mid, EV_VM_OP, 0, 0);

    VmPortableDiskOp op_msg(vid, PLUG_OUT_PDISK, "", "", request_id, temp_mu.GetSelfMID(),"admin");
    STATUS ret = temp_mu.Send(op_msg, req_option);
    if (SUCCESS != ret)
    {
        cerr << "Fail to send detach request! ret = " << ret << endl;
    }
    else
    {
        cout << "Detach request send success" << endl;
    }

    MessageFrame message;
    if(SUCCESS == temp_mu.Wait(&message,3000))
    {
        VmOperationAck  ack;
        ack.deserialize(message.data);
        cout << " VmOperationAck: "
             << " vid: " << ack._vid
             << " op:" << ack._operation
             << " state:" << ack._vm_state << endl;
    }
    else
    {
        cout<<"Wait ack timeout"<<endl;
    }
}
DEFINE_DEBUG_FUNC(DbgDetachDisk);

}

