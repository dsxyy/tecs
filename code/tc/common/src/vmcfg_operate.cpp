/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�vmcfg_operate.cpp
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��
* ��ǰ�汾��1.0
* ��    �ߣ�Bob
* ������ڣ�2012��11��15��
*
* �޸ļ�¼1��
*    �޸����ڣ�2012/11/15
*    �� �� �ţ�V1.0
*    �� �� �ˣ�Bob
*    �޸����ݣ�����
*******************************************************************************/
#include "vmcfg_operate.h"
#include "authrequest.h"
#include "authmanager.h"
#include "project_pool.h"
#include "image_db.h"
#include "license_common.h"
#include "affinity_region_pool.h"

namespace zte_tecs
{

VmCfgOperate *VmCfgOperate::_op_inst= NULL;
VmCfgPool    *VmCfgOperate::_vcpool = NULL;

/******************************************************************************/
STATUS VmCfgOperate::DealDirectCreate(const ApiVmCfgAllocateData  &req,
                                           int64          uid,
                                           vector<int64>  &vids,
                                           ostringstream &oss)
{
    int64          project_id = INVALID_OID;
    STATUS         tRet       = SUCCESS;


    /* ��Ȩ */
    if (Authorize(uid, NULL, AuthRequest::CREATE) == false)
    {
        oss<<"Authorize Fail";
        OutPut(SYS_DEBUG, "Authorize Fail\n");
        return ERR_AUTHORIZE_FAILED;
    }

    // У�鹤����Ϣ
    ProjectPool *ppp = ProjectPool::GetInstance();
    project_id = ppp->GetProjectIdByName(uid, req.cfg_info.project_name);
    if (INVALID_OID == project_id)
    {
        oss<<"Project ( "<<req.cfg_info.project_name <<" ) not exist";
        OutPut(SYS_DEBUG, "Project Fail\n");
        return ERR_VMCFG_INVALID_PROJECT;
    }

    vector<VmBaseImage>  vec_img;
    //У��ӳ���ļ�
    ParseVmBaseImage(req.cfg_info.base_info, vec_img);
    tRet = CheckImageFiles(vec_img,
                           uid,
                           req.cfg_info.base_info.virt_type,
                           req.cfg_info.base_info.hypervisor,
                           false,
                           oss);
    if (SUCCESS != tRet)
    {
        OutPut(SYS_DEBUG, "Image Fail, %s\n", oss.str().c_str());
        return ERR_VMCFG_ACQUIRE_IMAGE_FAILED;
    }

    //������������
    int  vm_num = req.vm_num;
    if (0 == vm_num )
    {
        vm_num =1;
    }

    VmCfg        vmcfg(req, uid);
    vmcfg.set_project_id(project_id);

    VmAllocateReq vmcfgreq;

    MessageUnit    temp_mu(TempUnitName("tecs_vmcfg_setup"));
    temp_mu.Register();

    MID            dest(PROC_TC, MU_VMCFG_MANAGER);
    MessageOption  option(dest, EV_VM_ALLOCATE_REQ, 0, 0);

    int32   wait_time = 0;
    wait_time = 30000;

    vmcfgreq.base_info=vmcfg;
    vmcfgreq.vm_num = vm_num;

    temp_mu.Send(vmcfgreq, option);
    /*  �ȴ�TC��Ӧ */
    MessageFrame  msg;
    if (SUCCESS == temp_mu.Wait(&msg, wait_time))
    {
        /*  �յ�Ӧ�����֮ǰ��Ϣ��Ӧ�� ?  */
        OutPut(SYS_DEBUG,"\nDealDirectCreate : rcv vm allocate ack msg:\n  %s \n\n",
                msg.data.c_str());
        VmAllocateAck  tc_ack;
        tc_ack.deserialize(msg.data);
        if (tc_ack.state == SUCCESS) //�ɹ�
        {
            vids=tc_ack.vids;
            return SUCCESS;
        }
        else  //ʧ��
        {
            oss << tc_ack.detail;
            return tc_ack.state;
        }
    }
    else
    {
        /*  ��ʱ or �������� ? */
        oss << "create vm timeout";
        return ERR_RPOJECT_TIMEOUT;
    }
}
/******************************************************************************/
STATUS VmCfgOperate::DealClone( ApiVmStaticInfo& req,int64 uid, int64 &clone_vid,ostringstream &oss)
{
    ApiVmCfgAllocateData  allocate_data;
    STATUS tRet = -1;

    allocate_data.cfg_info.vm_name = req.cfg_info.vm_name;
    allocate_data.cfg_info.project_name = req.cfg_info.project_name;
    allocate_data.vm_num = 1;

    allocate_data.cfg_info.base_info.description = req.cfg_info.base_info.description;
    allocate_data.cfg_info.base_info.vcpu = req.cfg_info.base_info.vcpu;
    allocate_data.cfg_info.base_info.tcu = req.cfg_info.base_info.tcu;
    allocate_data.cfg_info.base_info.memory = req.cfg_info.base_info.memory;
    allocate_data.cfg_info.base_info.machine = req.cfg_info.base_info.machine;
    allocate_data.cfg_info.base_info.root_device = "";
    allocate_data.cfg_info.base_info.kernel_command = "";
    allocate_data.cfg_info.base_info.bootloader = "";
    allocate_data.cfg_info.base_info.virt_type = req.cfg_info.base_info.virt_type;
    allocate_data.cfg_info.base_info.rawdata = "";
    allocate_data.cfg_info.base_info.context = req.cfg_info.base_info.context;
    allocate_data.cfg_info.base_info.cluster = req.cfg_info.base_info.cluster;
    allocate_data.cfg_info.base_info.host = req.cfg_info.base_info.host;
    allocate_data.cfg_info.base_info.vm_wdtime = req.cfg_info.base_info.vm_wdtime;
    allocate_data.cfg_info.base_info.vnc_passwd = req.cfg_info.base_info.vnc_passwd;
    allocate_data.cfg_info.base_info.enable_coredump = req.cfg_info.base_info.enable_coredump;
    allocate_data.cfg_info.base_info.enable_serial = req.cfg_info.base_info.enable_serial;
    allocate_data.cfg_info.base_info.hypervisor = req.cfg_info.base_info.hypervisor;
    allocate_data.cfg_info.base_info.enable_livemigrate= req.cfg_info.base_info.enable_livemigrate;

    vector<ApiVmDevice>::iterator dev_it;
    ApiVmDevice dev;
    for(dev_it = req.cfg_info.base_info.devices.begin(); dev_it != req.cfg_info.base_info.devices.end(); dev_it++)
    {
        dev.type = dev_it->type;
        dev.count = dev_it->count;
        allocate_data.cfg_info.base_info.devices.push_back(dev);
    }

    vector<ApiVmDisk>::iterator disk_it;
    for (disk_it = req.cfg_info.base_info.disks.begin();disk_it != req.cfg_info.base_info.disks.end();disk_it++)
    {
        ApiVmDisk disk(disk_it->id,
                       disk_it->position,
                       disk_it->size,
                       disk_it->bus,
                       disk_it->type,
                       disk_it->target,
                       disk_it->fstype);

        allocate_data.cfg_info.base_info.disks.push_back(disk);
    }

    vector<ApiVmExpression>::iterator exp_it;
    ApiVmExpression exp;
    for (exp_it = req.cfg_info.base_info.qualifications.begin();exp_it != req.cfg_info.base_info.qualifications.end();exp_it++)
    {
        exp.key = exp_it->key;
        exp.op = exp_it->op;
        exp.value = exp_it->value;
        allocate_data.cfg_info.base_info.qualifications.push_back(exp);
    }

    //������Ϣ
    vector<ApiVmNic>::iterator nic_it;
    ApiVmNic nic;
    for (nic_it = req.cfg_info.base_info.nics.begin();nic_it != req.cfg_info.base_info.nics.end();nic_it++)
    {
        nic.nic_index = nic_it->nic_index;
        nic.sriov = nic_it->sriov;
        nic.loop = nic_it->loop;
        nic.logic_network_id = nic_it->logic_network_id;
        nic.model = nic_it->model;
        allocate_data.cfg_info.base_info.nics.push_back(nic);
    }
    vector<int64> vids;
    tRet = DealDirectCreate(allocate_data, uid, vids,oss);
    clone_vid = vids.front();
    OutPut(SYS_DEBUG, "clone vm ret %d, %s, clone_vid %d",tRet,oss.str().c_str(),clone_vid);
    return tRet;
}
/******************************************************************************/
STATUS VmCfgOperate::DealDelete(int64         vid,
                                       int64         uid,
                                       ostringstream   &err_oss)
{
    VmCfg  vmcfg;

    STATUS tRet = VmCfgOperate::_vcpool->GetVmById(vmcfg, vid);
    if (SUCCESS!= tRet)
    {
        err_oss<<"VM ("<< vid <<") not exist.";
        return ERR_OBJECT_NOT_EXIST;
    }

    tRet = CheckOpDelete(vmcfg, uid,err_oss);
    if (SUCCESS!= tRet)
    {
        return tRet;
    }

    err_oss.str("");
    tRet = VmCfgOperate::_vcpool->Drop(vmcfg);
    if (ERR_VMCFG_EXIST_IN_DELETED_TAB == tRet)
    {
             OutPut(SYS_NOTICE,"vm %lld already exist in deleted_vmcfg_pool!\n",vmcfg._oid);
             err_oss<<"Insert VM ("<<vmcfg._oid<< ") to deleted_vmcfg_pool failed"<<endl;
             SkyAssert(false);
             return ERROR_DB_INSERT_FAIL;
    }
    else if(ERROR_DB_SELECT_FAIL == tRet)
    {
            OutPut(SYS_NOTICE,"query vm %lld in deleted_vmcfg_pool failed!\n",vmcfg._oid);
            err_oss<<"Insert VM ("<<vmcfg._oid<< ") to deleted_vmcfg_pool failed"<<endl;
            SkyAssert(false);
            return ERROR_DB_SELECT_FAIL;
    }
    else if(SQLDB_OK != tRet)
    {
            err_oss <<"Delete VM ("<<vmcfg._oid<<") failed !" << endl;
            SkyAssert(false);
            return ERROR_DB_DELETE_FAIL;
    }

    err_oss <<"Delete VM ("<<vmcfg._oid<<") success !" ;

    return SUCCESS;

}

/******************************************************************************/
STATUS VmCfgOperate::DealModify(const ApiVmCfgModifyData  &req,
                      int64          uid,
                      ostringstream &err_oss)
{
    VmCfg          vmcfg;

    /* ��ȡvc���� */
    STATUS  tRet = VmCfgOperate::_vcpool->GetVmById(vmcfg,req.vid);
    if (SUCCESS!=tRet)
    {
        err_oss<<"Get VM ("<< req.vid <<") failed!";
        return ERR_OBJECT_NOT_EXIST;
    }

    /* ��Ȩ */
    if (Authorize(uid, &vmcfg, AuthRequest::MANAGE) == false)
    {
        err_oss<<"Authorize Fail";
        return ERR_AUTHORIZE_FAILED;
    }
    bool is_deploying = _vcpool->IsVmDeploying(req.vid);
    bool is_canceling = _vcpool->IsVmCanceling(req.vid);

    if(true == is_deploying)
    {
         err_oss<<"VM is deploying";
         return ERR_VMCFG_STATE_FORBIDDEN;
    }

    if(true == is_canceling)
    {
         err_oss<<"VM is canceling";
         return ERR_VMCFG_STATE_FORBIDDEN;
    }

    /* ����׺͹�ϵ,��������׺�������ָ������ */
    AffinityRegion ar;
    tRet = _vcpool->GetARByVid(req.vid,ar);
    if ((tRet != SUCCESS)&&(tRet != ERROR_OBJECT_NOT_EXIST))
    {
        err_oss<<"Query vm in affinity region id failed !\n";
        return ERROR_DB_SELECT_FAIL;
    }
    else if(tRet == SUCCESS)
    {
        if(!req.cfg_info.base_info.cluster.empty() ||!req.cfg_info.base_info.host.empty())
        {
            err_oss<<" VM in affinity region,can not appoint cc !\n";
            return ERR_VMCFG_STATE_FORBIDDEN;
        }
    }

    // У�鹤����Ϣ
    ProjectPool *pool = ProjectPool::GetInstance();
    int64 project_id = pool->GetProjectIdByName(uid, req.cfg_info.project_name);
    if (INVALID_OID == project_id)
    {
        err_oss<<"Project ( "<< req.cfg_info.project_name <<" ) not exist";
        return ERR_VMCFG_INVALID_PROJECT;
    }

    //У��ӳ���ļ�
    vector<VmBaseImage>  vec_img;
    ParseVmBaseImage(req.cfg_info.base_info, vec_img);
    tRet = CheckImageFiles(vec_img,
                           uid,
                           req.cfg_info.base_info.virt_type,
                           req.cfg_info.base_info.hypervisor,
                           false,
                           err_oss);
    if (SUCCESS != tRet)
    {
        return ERR_VMCFG_ACQUIRE_IMAGE_FAILED;
    }

    vmcfg.Modify(req, uid);
    vmcfg.set_project_id(project_id);

    int rc =  VmCfgOperate::_vcpool->Update(vmcfg);
    if (SUCCESS != rc)
    {
        SkyAssert(false);
        err_oss <<"Modify VM ("<<req.vid << "failed !" ;
        return ERROR_DB_INSERT_FAIL;
    }
    err_oss <<"Modify VM ("<<req.vid << "success !" ;
    return SUCCESS;
}

// ���������Ƿ��Ѿ����𲢻�ȡ��������úͼ�Ⱥ��
STATUS VmCfgOperate::GetVmcfg(int64 vid, VmCfg &cfg, string &name, ostringstream &oss)
{
    VmCfgPool  *vcpool = VmCfgPool::GetInstance();
    if(NULL == vcpool)
    {
        oss<<"Get VmCfgPool instance failed.";
        return ERROR;
    }
    STATUS tRet = vcpool->GetVmById(cfg, vid);
    if (SUCCESS!= tRet)
    {
        oss<<"VM ("<< vid <<") not exist.";
        return ERR_OBJECT_NOT_EXIST;
    }
    name =cfg.get_cluster_name();
    if (name.empty())
    {
        oss <<"VM ("<< vid <<")Has not deploy";
        return  ERROR_NOT_READY;
    }
    return SUCCESS;
}

// ���������Ƿ��Ѿ����𲢻�ȡ��������úͼ�Ⱥ��
STATUS VmCfgOperate::GetVmcfg(int64 vid, string &name, ostringstream &oss)
{
    VmCfg cfg;
    return GetVmcfg(vid, cfg, name, oss);
}

/******************************************************************************/
STATUS VmCfgOperate::CheckAction(int64   uid,
                          int64          vid,
                          const string  &action_name,
                          VmOperation   &action_type,
                          ostringstream &err_oss)
{
    /* ��ȡ�������� */
    STATUS tRet = Str2VmOp(action_name, action_type, err_oss);
    if(SUCCESS != tRet)
    {
        return  tRet;
    }

    /* ��ȡvmcfg���� */
    err_oss.str("");
    VmCfg   vmcfg;
    tRet = VmCfgOperate::_vcpool->GetVmById(vmcfg, vid);
    if (SUCCESS != tRet)
    {
       err_oss<<"Get VM ("<< vid << ") failed! "<< endl;
       return ERR_OBJECT_NOT_EXIST;
    }

    switch (action_type)
    {
        /* �÷�֧������: �������߿ɲ������������������޹� */
        case DEPLOY:
        {
            tRet = CheckOpDeploy(vmcfg, uid, err_oss);
            return tRet;
            break;
        }

        /* �÷�֧������: �����ߡ������߾��ɲ������������δ���� */
        case DELETE:
        {
            tRet = CheckOpDelete(vmcfg, uid, err_oss);
            return tRet;
            break;
        }

        /* �÷�֧������: �����ߡ������߾��ɲ�������������Ѳ��� */
        case CANCEL:
        {
            tRet = CheckOpCancel(vmcfg, uid, err_oss);
            return tRet;
            break;
        }

        /* �÷�֧������: ���������ܲ�������������Ѳ��� */
        case START:
        case STOP:
        case REBOOT:
        case PAUSE:
        case RESUME:
        case RESET:
        case DESTROY:
        case SYNC:
        case UPLOAD_IMG:
        case CANCEL_UPLOAD_IMG:
        case PLUG_IN_USB:
        case PLUG_OUT_USB:
        case OUT_IN_USB:
        case PLUG_IN_PDISK:
        case PLUG_OUT_PDISK:
        case RECOVER_IMAGE:
        case MODIFY_IMAGE_BACKUP:
        case CREATE_IMAGE_BACKUP:
        case RESTORE_IMAGE_BACKUP:
        case DELETE_IMAGE_BACKUP:
        case FREEZE:
        case UNFREEZE:
        {
            tRet = CheckOpNormal(vmcfg, uid,err_oss);
            return tRet;
            break;
        }

        default:
        {
            OutPut(SYS_DEBUG," Rcv unsupport vm operation %d (%s) request.\n",
                     action_type, Enum2Str::VmOpStr(action_type));
            err_oss<<"Unsupport VM operation "<<action_type <<
                  "("<<Enum2Str::VmOpStr(action_type)<<")" ;

            return ERR_VMCFG_INVALID_ACTION_TYPE;
            break;
        }
    }

    return SUCCESS;

}

/******************************************************************************/
STATUS VmCfgOperate::CheckOpDeploy(VmCfg         &vmcfg,
                                          int64          op_uid,
                                          ostringstream &err_oss)
{
    /* ��Ȩ, �������߿ɲ��� */
    if (Authorize(op_uid, &vmcfg, AuthRequest::MANAGE) == false)
    {
        err_oss<<"Authorize Fail";
        return ERR_AUTHORIZE_FAILED;
    }
    return SUCCESS;
}

/******************************************************************************/
STATUS VmCfgOperate::CheckOpQuery(VmCfg         &vmcfg,
                                          int64          op_uid,
                                          ostringstream &err_oss)
{
    /* ��Ȩ, �û��Լ�ֻ�ܿ��Լ��ģ�����Ա���Կ�ȫ���� */
    if (Authorize(op_uid, &vmcfg, AuthRequest::INFO) == false)
    {
        err_oss<<"Authorize Fail";
        return ERR_AUTHORIZE_FAILED;
    }
    return SUCCESS;
}

/******************************************************************************/
STATUS VmCfgOperate::CheckOpDelete(VmCfg         &vmcfg,
                                       int64          op_uid,
                                       ostringstream &err_oss)
{
    /* ��Ȩ */
    if (Authorize(op_uid, NULL, AuthRequest::MANAGE) == false)//��Ϊ����Ա
    {
       if (Authorize(op_uid, &vmcfg, AuthRequest::MANAGE) == false)//��Ϊ������
       {
            err_oss<<"Authorize Fail";
            return ERR_AUTHORIZE_FAILED;
       }
    }

    string cluster_name = vmcfg.get_cluster_name();
    if (!cluster_name.empty())
    {
         err_oss<<"Delete VM ("<<vmcfg._oid<< ") failed, VM has deployed ";
         return ERR_VMCFG_STATE_FORBIDDEN;
    }
    bool is_deploying = _vcpool->IsVmDeploying(vmcfg.get_vid());
    bool is_canceling = _vcpool->IsVmCanceling(vmcfg.get_vid());

    if(true == is_deploying)
    {
         err_oss<<"VM is deploying";
         return ERR_VMCFG_STATE_FORBIDDEN;
    }
    else if(true == is_canceling)
    {
         err_oss<<"VM is canceling";
         return ERR_VMCFG_STATE_FORBIDDEN;
    }

    return SUCCESS;
}


/******************************************************************************/
STATUS VmCfgOperate::CheckOpCancel(VmCfg         &vmcfg,
                                       int64          op_uid,
                                       ostringstream &err_oss)
{
    /* ��Ȩ */
    if (Authorize(op_uid, NULL, AuthRequest::MANAGE) == false)//��Ϊ����Ա
    {
        if (Authorize(op_uid, &vmcfg, AuthRequest::MANAGE) == false)//��Ϊ������
        {
            err_oss<<"Authorize Fail";
            return ERR_AUTHORIZE_FAILED;
        }
    }

    string cluster_name = vmcfg.get_cluster_name();
    bool is_deploying = _vcpool->IsVmDeploying(vmcfg.get_vid());
    bool is_canceling = _vcpool->IsVmCanceling(vmcfg.get_vid());
    if ((cluster_name.empty()) && (!is_deploying) )
    {
        if(is_canceling)
        {
            err_oss<<"CANCEL VM ("<<vmcfg._oid<< ") failed, VM is canceling ";
        }
        else
        {
            err_oss<<"CANCEL VM ("<<vmcfg._oid<< ") failed, VM has not deployed and is not deploying ";
        }
        return ERR_VMCFG_STATE_FORBIDDEN;
    }

    return SUCCESS;
}


/******************************************************************************/
STATUS VmCfgOperate::CheckOpNormal(VmCfg         &vmcfg,
                                       int64          op_uid,
                                       ostringstream &err_oss)
{
    /* ��Ȩ �������߿ɲ��� */
    if (Authorize(op_uid, &vmcfg, AuthRequest::MANAGE) == false)
    {
         err_oss << "Authorize Fail";
         return ERR_AUTHORIZE_FAILED;
    }
    string cluster_name = vmcfg.get_cluster_name();
    if (cluster_name.empty())
    {
        err_oss<<"Forbiden to operate VM ("<<vmcfg._oid<<
                        "), for VM has not deploy ";
        return ERR_VMCFG_STATE_FORBIDDEN;
    }

    return SUCCESS;
}

/******************************************************************************/
STATUS VmCfgOperate::CheckOpMigrate(int64   op_uid,
                                           int64          vid,
                                           bool          blive,
                                           string        &cluster_name,
                                           ostringstream &err_oss)
{
    err_oss.str("");

    /* ��Ȩ */
    if (Authorize(op_uid, NULL, AuthRequest::MANAGE) == false)//��Ϊ����Ա
    {
        err_oss<<"Authorize Fail";
        return ERR_AUTHORIZE_FAILED;
    }

    /* ��ȡvmcfg���� */
    VmCfg   vmcfg;
    STATUS tRet = VmCfgOperate::_vcpool->GetVmById(vmcfg, vid);
    if (SUCCESS != tRet)
    {
       err_oss<<"Get VM ("<< vid << ") failed! "<< endl;
       return ERR_OBJECT_NOT_EXIST;
    }

    cluster_name = vmcfg.get_cluster_name();
    if (cluster_name.empty())
    {
        err_oss<<"VM("<< vid<<") has not deploy, can not migrate.";
        return ERR_VMCFG_STATE_FORBIDDEN;
    }

    if(!vmcfg.enable_livemigrate() && blive)
    {
        err_oss<<"VM("<< vid<<") is being forbiddn to live migrate.";
        return ERR_VMCFG_STATE_FORBIDDEN;
    }

    return SUCCESS;
 }

/******************************************************************************/
STATUS VmCfgOperate::PrepareSynMsg(VmCfg  &vmcfg,
                                   const string &session_stamp,
                                   VMDeployInfo &req_msg,
                                   ostringstream &err_oss)
{
    ostringstream  where_string;

    // 1. ��ȡ����������
    where_string << "vid1 = " << vmcfg.get_vid()
                 << " OR vid2 = "<< vmcfg.get_vid() <<endl;

    vector<ApiVmCfgMap> map_vm;
    vector<ApiVmCfgMap>::const_iterator iter;
    int rc = VmCfgOperate::_vcpool->SearchRepelTab(map_vm, where_string.str());
    if (rc != SUCCESS)
    {
        err_oss<<"Query vm relation failed !"<<endl;
        return ERROR;
    }
    for(iter = map_vm.begin(); iter != map_vm.end(); iter ++)
    {
        if (iter->src ==  vmcfg.get_vid())
        {
           req_msg._requirement._repel_vms.push_back(iter->des);
        }
        else
        {
           req_msg._requirement._repel_vms.push_back(iter->src);
        }
    }

    // 2. ����ͬ����Ϣ
    req_msg._stamp = session_stamp;
    rc = VmCfgOperate::_vcpool->ToVMDeployInfo(vmcfg,req_msg);
    if (SUCCESS != rc  )
    {
       OutPut(SYS_DEBUG,"Failed to construct syn msg of VM(%d) \n",vmcfg.get_vid() );

       return ERROR;
    }

    return  SUCCESS;
}

/******************************************************************************/
STATUS VmCfgOperate::SetDisks(vector<VmBaseDisk>  &vec_disks,
                          int64          vid,
                          int64          uid,
                          ostringstream &err_oss)
{
    VmCfg  vm;
    int rc = _vcpool->GetVmById(vm,vid);
    if (SUCCESS!= rc)
    {
        err_oss<<"Get VM ("<< vid << ") failed! "<< endl ;
        return ERR_OBJECT_NOT_EXIST;
    }

    /* ��Ȩ */
    if (Authorize(uid, &vm, AuthRequest::MANAGE) == false)
    {
        err_oss<<"Authorize Fail";
        return ERR_AUTHORIZE_FAILED;
    }

    vm.set_disks(vec_disks);
    /* �������ݿ� */
    rc = _vcpool->Update(vm);
    if (0 != rc)
    {
        err_oss<<"Error trying to set VM ("<<vid<< ") disk." <<endl;
        return ERR_OBJECT_UPDATE_FAILED;
    }

    return SUCCESS;
}

/******************************************************************************/
STATUS VmCfgOperate::SetImageDisks(vector<VmBaseImage>  &vec_imgs,
                            int64          vid,
                          int64          uid,
                          ostringstream &err_oss)
{
    VmCfg  vm;
    int rc = _vcpool->GetVmById(vm,vid);
    if (SUCCESS!= rc)
    {
        err_oss<<"Get VM ("<< vid << ") failed! "<< endl ;
        return ERR_OBJECT_NOT_EXIST;
    }

    /* ��Ȩ */
    if (Authorize(uid, &vm, AuthRequest::MANAGE) == false)
    {
        err_oss<<"Authorize Fail";
        return ERR_AUTHORIZE_FAILED;
    }

    //У��ӳ���ļ�
    STATUS tRet = CheckImageFiles(vec_imgs,
                                  uid,
                                  vm.get_virt_type(),
                                  vm.get_hypervisor(),
                                  false,
                                  err_oss);
    if (SUCCESS != tRet)
    {
        return ERR_VMCFG_ACQUIRE_IMAGE_FAILED;
    }

    vm.set_images(vec_imgs);
    /* �������ݿ� */
    rc = _vcpool->Update(vm);
    if (0 != rc)
    {
        err_oss<<"Error trying to set VM ("<<vid<< ") imagedisk." <<endl;
        return ERR_OBJECT_UPDATE_FAILED;
    }

    return SUCCESS;
}
/******************************************************************************/
STATUS VmCfgOperate::SetNics(vector<VmBaseNic>  &vec_nics,
                          int64          vid,
                          int64          uid,
                          ostringstream &err_oss)
{
    VmCfg  vm;
    int rc = _vcpool->GetVmById(vm,vid);
    if (SUCCESS!= rc)
    {
        err_oss<<"Get VM ("<< vid << ") failed! "<< endl ;
        return ERR_OBJECT_NOT_EXIST;
    }

    /* ��Ȩ */
    if (Authorize(uid, &vm, AuthRequest::MANAGE) == false)
    {
        err_oss<<"Authorize Fail";
        return ERR_AUTHORIZE_FAILED;
    }

    vm.set_nics(vec_nics);

    /* �������ݿ� */
    rc = _vcpool->Update(vm);
    if (0 != rc)
    {
        err_oss<<"Error trying to set  VM ("<<vid<< ")  nic." <<endl;
        return ERR_OBJECT_UPDATE_FAILED;
    }

    return SUCCESS;

}
/******************************************************************************/
STATUS VmCfgOperate::Str2QueryType(const string   &query_name,
                          QUERY_TYPE    &type,
                          ostringstream  &err_oss)
{
    if (RPC_QUERY_SELF_APPOINTED == query_name)
    {
        type = QUERY_SELF_APPOINTED;
    }
    else if (RPC_QUERY_ALL_SELF == query_name)
    {
        type = QUERY_ALL_SELF;
    }
    else if (RPC_QUERY_ALL== query_name)
    {
        type = QUERY_ALL;
    }
    else
    {
        err_oss <<"Unsupport query type : "<< query_name << " \n";
        return ERR_VMCFG_INVALID_QUERY_TYPE;
    }

    return SUCCESS;
}


/******************************************************************************/
STATUS VmCfgOperate::DealQuery(const ApiVmCfgQuery &req,
                                   int64          uid,
                                   bool           is_admin,
                                   QuerySession   &query_session,
                                   ostringstream  &err_oss)
{
    ostringstream      where_string;
    ostringstream      where_string_deployed;
    ostringstream      where_string_count;
    bool               appointed_flag = false;
    STATUS tRet = ERROR;
    QUERY_TYPE    type;

    tRet = Str2QueryType(req.type, type, err_oss);
    if(SUCCESS != tRet)
    {
        return  tRet;
    }

    switch (type)
    {
        /* ���û���������������� */
        case QUERY_ALL_SELF:
        {
            where_string << "UID = " << uid<<\
                             " order by oid limit " <<req.count <<\
                             " OFFSET " <<req.start_index ;
           /* where_string_deployed  << "UID = " << uid<<\
                             " AND deployed_cluster IS NOT NULL" <<\
                             " order by oid limit " <<req.count <<\
                             " OFFSET " <<req.start_index ;       */
            where_string_count<< "UID = " << uid;
            appointed_flag = false;
            break;
        }

        /* ���û��ɼ�������*/
         case QUERY_ALL:
        {
            if (!is_admin)
            {
                err_oss << "Error, only administrators can query appoined all vmcfgs!";
                return ERR_AUTHORIZE_FAILED;
            }
            where_string <<" UID IS NOT NULL "<<\
                             " order by oid limit " <<req.count <<\
                             " OFFSET " <<req.start_index;
           /* where_string_deployed <<" UID IS NOT NULL "<<\
                             " AND deployed_cluster IS NOT NULL" <<\
                             " order by oid limit " <<req.count <<\
                             " OFFSET " <<req.start_index; */
            where_string_count<<" UID IS NOT NULL ";
            appointed_flag = false;
            break;
        }

        /* ĳ��VID����ϸ��Ϣ  */
        case QUERY_SELF_APPOINTED:
        {
            where_string << " OID = " << req.appointed_vid << \
                           " AND UID IS NOT NULL";
         /*   where_string_deployed << " OID = " << appointed_vid << \
                           " AND deployed_cluster IS NOT NULL" <<\
                           " AND UID IS NOT NULL";      */
            where_string_count << " OID = " << req.appointed_vid <<\
                            " AND UID IS NOT NULL";
            /* ��Ȩ */
            VmCfg vmcfg;
            tRet = VmCfgOperate::_vcpool->GetVmById(vmcfg,req.appointed_vid);
            if(SUCCESS != tRet)
            {
                err_oss<<"VM (" << req.appointed_vid << ") not exist.";
                return ERROR_DB_SELECT_FAIL;
            }

            if (Authorize(uid, &vmcfg, AuthRequest::INFO) == false)
            {
                err_oss<<"Authorize Fail";
                return  ERR_AUTHORIZE_FAILED;
            }
            appointed_flag = true;
            break;
        }

        default:
        {
            err_oss<<"Incorrect query type: "<< req.type ;
            return ERR_VMCFG_INVALID_QUERY_TYPE;
            break;
        }
    }

    // 1. ��ǰ���������������
    vector<int64>      vids;
    tRet = VmCfgOperate::_vcpool->GetVids(vids, where_string_count.str());
    if ((SUCCESS != tRet)
         &&(ERROR_OBJECT_NOT_EXIST != tRet))
    {
       err_oss<<"Query failed !"<<endl;
       return ERROR_DB_SELECT_FAIL;
    }

    query_session.max_count = vids.size();
    query_session._cluster_vcs.clear();
    query_session._need_query_network_vids.clear();
    query_session._vc_info.clear();

    // 2. ��ǰҳ�����������ϸ��Ϣ
    int rc = VmCfgOperate::_vcpool->Show(query_session._vc_info, query_session._vc_runinfo, where_string.str());
    if (rc != 0)
    {
       err_oss<<"Query failed !"<<endl;
       return  ERROR_DB_SELECT_FAIL;
    }

    // 3. ��ǰҳ������VNM����������CC��״̬�������
    vector<ApiVmStaticInfo>::iterator   iter;
    vector<ApiVmRunningInfo>::iterator   run_iter;
    for(iter = query_session._vc_info.begin();
        iter!= query_session._vc_info.end();
        iter++)
    {

        for(run_iter = query_session._vc_runinfo.begin();
            run_iter!= query_session._vc_runinfo.end();
            run_iter++)
        {
            if(run_iter->vid == iter->vid)
            {
                string deployed_cluster = run_iter->computationinfo.deployed_cluster;
                if (!deployed_cluster.empty())
                {//�Ѳ���
                    if(!iter->cfg_info.base_info.nics.empty())//������
                    {
                        query_session._need_query_network_vids.push_back(iter->vid);
                    }

                    map<string, ClusterVmRunningInfo>::iterator  it_cluster;
                    vector<VmRunningInfo>::iterator    it_vm;
                    VmRunningInfo                      vm_running_info;

                    vm_running_info._vid = iter->vid;

                     it_cluster = query_session._cluster_vcs.find(deployed_cluster);
                     //CC����,����VM
                     if (it_cluster != query_session._cluster_vcs.end())
                     {
                         it_cluster->second._vm_running_info.push_back(vm_running_info);
                     }
                     else//CCδ����
                     {
                         ClusterVmRunningInfo     cluster_vc;
                         cluster_vc._vm_running_info.push_back(vm_running_info);

                         query_session._cluster_vcs.insert(make_pair(deployed_cluster, cluster_vc));
                     }
                 }
             }
        }
    }

    if (true == appointed_flag)//ָ����ѯ
    {
        query_session.next_index = -1;
        query_session.out_num  = query_session._vc_info.size();
    }
    else
    {   //��βҳ
        if (query_session._vc_info.size() >= (uint64)(req.count))
        {
            query_session.next_index = req.start_index + req.count;
            query_session.out_num = req.count;
        }
        else //βҳ
        {
            query_session.next_index = -1;
            query_session.out_num    = query_session._vc_info.size();
        }
    }

    query_session.uid       = uid;
    /* �Ƿ�Ϊ����Ա */
    query_session.is_admin  = Authorize(uid, NULL, AuthRequest::INFO);
    query_session.wait_cc_msgs = query_session._cluster_vcs.size();
    query_session.wait_vnm_msgs= query_session._need_query_network_vids.size();

    return SUCCESS;
}

/******************************************************************************/
STATUS VmCfgOperate::SetVmRelation(const  ApiVmCfgRelationData  &_api_data,
                                        int64                   uid,
                                        ostringstream           &err_oss, string &wf_id)
{
    int     rc  = -1;
    STATUS  tRet = ERROR;
    int64   src_uid = INVALID_OID;
    int64   des_uid = INVALID_OID;
    string  src_cluster;
    string  des_cluster;
    VmCfg   src_vm;
    VmCfg   des_vm;
    bool src_is_deploying = false;
    bool des_is_deploying = false;

    ApiVmCfgRelationData  api_data(_api_data);
    ApiVmCfgRelationData  deployeds(_api_data);
    deployeds.vec_vm.clear();

    /* У��Ϸ��� */
    if ((ApiVmCfgRelationData::RELATION_ADD   != static_cast <ApiVmCfgRelationData::OperationType>(api_data.type))
         &&(ApiVmCfgRelationData::RELATION_DEL!= static_cast <ApiVmCfgRelationData::OperationType>(api_data.type)) )
    {
        err_oss<<"Invalid action type:"<<api_data.type <<" , 0:add 1:del"<<endl;
        return ERROR_INVALID_ARGUMENT;
    }

    vector<ApiVmCfgMap>::iterator  it;
    for (it = api_data.vec_vm.begin();
         it!= api_data.vec_vm.end();
         it++)
    {
       if ( (INVALID_OID   == it->des)
            ||(INVALID_OID == it->src)
            ||(it->des == it->src))
       {
           err_oss<<"Argument error: VM1: "<<it->src <<", VM2: "<<it->des <<" .";
           return ERROR_INVALID_ARGUMENT;
       }

       tRet  = _vcpool->GetVmById(src_vm,it->src);
       if (SUCCESS!=tRet)
       {
           err_oss<<"VM ("<<it->src <<") not exist!";
           return ERR_OBJECT_NOT_EXIST;
       }

       src_uid     = src_vm.get_uid();
       if (uid != src_uid)
       {
           err_oss<<"Permission deny to access VM ("<< it->src <<")";
           return ERR_AUTHORIZE_FAILED;
       }

       src_is_deploying = _vcpool->IsVmDeploying(src_vm.get_vid());
       if (src_is_deploying)
       {
           err_oss<<"VM ("<< it->src <<") is deploying, can not modify";
           return ERR_VMCFG_MODIFY_FORBIDDEN ;
       }

       tRet = _vcpool->GetVmById(des_vm,it->des);
       if (SUCCESS!=tRet)
       {
           err_oss<<"VM ("<<it->des <<") not exist!";
           return  ERR_OBJECT_NOT_EXIST;
       }

       des_uid     = des_vm.get_uid();
       if (uid != des_uid)
       {
           err_oss<<"Permission deny to access VM ("<<it->des <<")";
           return ERR_AUTHORIZE_FAILED;
       }

       des_is_deploying = _vcpool->IsVmDeploying(des_vm.get_vid());
       if (des_is_deploying)
       {
           err_oss<<"VM ("<< it->des <<") is deploying, can not modify";
           return ERR_VMCFG_MODIFY_FORBIDDEN ;
       }

        /*�������������ͬһ��board�����׺����������û����ϵ*/
        if (ApiVmCfgRelationData::RELATION_ADD == static_cast <ApiVmCfgRelationData::OperationType>(api_data.type))
        {
            AffinityRegion ar1,ar2;

            tRet = _vcpool->GetARByVid(it->src,ar1);
            if((tRet !=ERROR_OBJECT_NOT_EXIST )&&(tRet !=SUCCESS))
            {
                err_oss<< "get affinity region of VM ("<< it->src<<") failed ";
                return ERR_OBJECT_UPDATE_FAILED;
            }
            else if (tRet ==SUCCESS)
            {/*it->src�Ѽ����׺���*/
                tRet = _vcpool->GetARByVid(it->des,ar2);
                if((tRet !=ERROR_OBJECT_NOT_EXIST )&&(tRet !=SUCCESS))
                {
                    err_oss<< "get affinity region of VM ("<< it->des<<") failed ";
                    return ERR_OBJECT_UPDATE_FAILED;
                }
                else if (tRet ==SUCCESS)
                {
                    if((ar1.get_oid() == ar2.get_oid())&&(ar1._level ==AFFINITY_REGION_BOARD))
                    {
                        err_oss<< "VM ("<< it->src<<") and VM( "<<it->des<<") have the same board level affinity region";
                        return ERR_OBJECT_UPDATE_FAILED;
                    }
                }
            }
        }

        src_cluster = src_vm.get_cluster_name();
        if (!src_cluster.empty())
        {
            //err_oss<<"VM ("<< it->src <<") has deployed, can not modify";
            //return ERR_VMCFG_MODIFY_FORBIDDEN ;
            deployeds.vec_vm.push_back(*it);
        
            api_data.vec_vm.erase(it);
            it--;
            continue;
        }
        
        des_cluster = des_vm.get_cluster_name();
        if (!des_cluster.empty())
        {
            //err_oss<<"VM ("<< it->des <<") has deployed, can not modify";
            //return ERR_VMCFG_MODIFY_FORBIDDEN ;
            deployeds.vec_vm.push_back(*it);
        
            api_data.vec_vm.erase(it);
            it--;
            continue;
        }
    }

    Transaction::Begin();

    /* ���ݿ���� */
    for (it = api_data.vec_vm.begin();
         it!= api_data.vec_vm.end();
         it ++)
    {
        int64   vid_small = 0;
        int64   vid_big   = 0;
        if(it->src < it->des)
        {
            vid_small = it->src;
            vid_big   = it->des;
        }
        else
        {
            vid_small = it->des ;
            vid_big   = it->src ;
        }

        if (ApiVmCfgRelationData::RELATION_ADD == static_cast <ApiVmCfgRelationData::OperationType>(api_data.type))
        {
            rc = _vcpool->Insert2Repel(vid_small, vid_big);
        }
        else
        {
            rc = _vcpool->DropRepel(vid_small, vid_big);
        }

        if (rc < 0)
        {
            //SkyAssert(false);
            OutPut(SYS_DEBUG, "Operate Database failed\n");
            err_oss<< "Operate Database failed";
            return ERR_OBJECT_UPDATE_FAILED;
        }
    }

    if (SUCCESS != Transaction::Commit())
    {
        //SkyAssert(false);
        OutPut(SYS_DEBUG, "Database commit failed\n");
        err_oss<< "Database commit failed";
        return ERR_OBJECT_UPDATE_FAILED;
    }

    return SetDeployedVmRelation(deployeds, uid, err_oss, wf_id);
}

STATUS VmCfgOperate::SetDeployedVmRelation(const  ApiVmCfgRelationData  &api_data,
                                        int64                   uid,
                                        ostringstream           &err_oss, string &wf_id)
{
    if (api_data.vec_vm.empty())
    {
        return SUCCESS;
    }

    VmCfgBatchModify req_msg;

    VmCfgModifyIndividualItem modification;
    modification._uid = uid;
    modification._element = VM_CFG_MUTEX;
    modification._operation = api_data.type + 1;

    vector<ApiVmCfgMap>::const_iterator  it;
    for (it = api_data.vec_vm.begin(); it != api_data.vec_vm.end(); it++)
    {
        modification._vid = it->src;
        modification._args["src"] = to_string<int64>(it->src, dec);
        modification._args["des"] = to_string<int64>(it->des, dec);

        req_msg._modifications.push_back(modification);
    }

    MessageUnit    temp_mu(TempUnitName("tecs_vmcfg_modify"));
    temp_mu.Register();

    MID            dest(PROC_TC, MU_VMCFG_MANAGER);
    MessageOption  option(dest, EV_VMCFG_MODIFY_REQ, 0, 0);

    temp_mu.Send(req_msg, option);

    /*  �ȴ�TC��Ӧ */
    MessageFrame  msg;
    const int  k_vmcfg_action_wait_time = (VM_OTHER_OP_DURATION_TC + 2)*1000;
    if (SUCCESS == temp_mu.Wait(&msg, k_vmcfg_action_wait_time))
    {
        VmOperationAck  tc_ack;
        tc_ack.deserialize(msg.data);

        err_oss << tc_ack.detail;
        wf_id = tc_ack._workflow_id;
        return tc_ack.state;
    }
    else
    {
        err_oss <<"modify repel time out";
        return ERROR_TIME_OUT;
    }
}

/******************************************************************************/
STATUS VmCfgOperate::DealQueryVmRelation(int64  vid,
                                                int64                uid,
                                                vector<ApiVmCfgMap> &vec_vm,
                                                ostringstream       &err_oss)
{
    ostringstream    where_string;

    if (0 == vid)
    {
        where_string << "uid1 = " << uid
                     << " OR uid2 = "<<uid ;
    }
    else if (0 < vid)
    {
        /* ��ȡvmcfg���� */
        VmCfg    vmcfg ;
        int rc= _vcpool->GetVmById(vmcfg,vid);
    	if (SUCCESS!= rc)
        {
           err_oss<<"Get VM ("<< vid << ") failed! " ;
           return ERR_OBJECT_NOT_EXIST;
        }

        /* ��Ȩ */
        if (Authorize(uid, &vmcfg, AuthRequest::INFO) == false)
        {
            err_oss<<"Authorize Fail";
            return  ERR_AUTHORIZE_FAILED;
        }
        where_string << "vid1 = " << vid
                     << " OR vid2 = "<< vid ;

    }
    else
    {
        err_oss<<"Unexpected query type:"<< vid <<endl;
        return ERROR_INVALID_ARGUMENT;
    }

    int rc = _vcpool->SearchRepelTab(vec_vm, where_string.str());
    if (rc != SUCCESS)
    {
       err_oss<<"Query failed !"<<endl;
       return ERROR_DB_SELECT_FAIL;
    }

    return SUCCESS;
}

/******************************************************************************/
STATUS VmCfgOperate::AddVmAffinityRegion(const  ApiAffinityRegionMemberAction  &api_data,
                              int64                   uid,
                              ostringstream           &err_oss)
{
    AffinityRegion  ar;
    int     rc  = -1;
    STATUS  tRet = ERROR;
    int64   vm_uid = INVALID_OID;
    string  cluster;
    string  host;
    VmCfg   vm;
    bool is_deploying = false;

    /* У��Ϸ��� */
    /* �Ѳ���ָ�������vm�����ܼ����׺���,�����׺����ܼ��� */
    vector<long long>::const_iterator it;
    for (it = api_data.vids.begin();
         it!= api_data.vids.end();
         it++)
    {
        if ( INVALID_OID == *it)
        {
            err_oss<<"Argument error: VM: "<<*it <<" .";
            return ERROR_INVALID_ARGUMENT;
        }

        tRet  = _vcpool->GetVmById(vm,*it);
        if (SUCCESS!=tRet)
        {
            err_oss<<"VM ("<<*it <<") not exist!";
            return ERR_OBJECT_NOT_EXIST;
        }

        vm_uid     = vm.get_uid();
        if (uid != vm_uid)
        {
            err_oss<<"Permission deny to access VM ("<< *it <<")";
            return ERR_AUTHORIZE_FAILED;
        }

        cluster = vm.get_cluster_name();
        if (!cluster.empty())
        {
            err_oss<<"VM ("<< *it <<") has deployed, can not add affinity region";
            return ERR_VMCFG_MODIFY_FORBIDDEN ;
        }

        cluster = vm.get_appointed_cluster();
        if (!cluster.empty())
        {
            err_oss<<"VM ("<< *it <<") has appoint_deployed, can not add affinity region";
            return ERR_VMCFG_MODIFY_FORBIDDEN ;
        }

        host = vm.get_appointed_host();
        if (!host.empty())
        {
            err_oss<<"VM ("<< *it <<") has appoint_deployed, can not add affinity region";
            return ERR_VMCFG_MODIFY_FORBIDDEN ;
        }

        is_deploying = _vcpool->IsVmDeploying(vm.get_vid());
        if (is_deploying)
        {
            err_oss<<"VM ("<< *it <<") is deploying, can not add affinity region";
            return ERR_VMCFG_MODIFY_FORBIDDEN ;
        }

        /*�ж��Ƿ������׺���*/
        AffinityRegion ar;
        tRet = _vcpool->GetARByVid(vm.get_vid(),ar);
        if ((tRet != SUCCESS)&&(tRet != ERROR_OBJECT_NOT_EXIST))
        {
            err_oss<<"Query vm in affinity region id failed !\n";
            return ERROR_DB_SELECT_FAIL;
        }
        else if(tRet == SUCCESS)
        {/*�Ѽ������������*/
            if(ar.get_oid() != api_data.affinity_region_id)
            {/*���ܼ��벻ͬ����*/

                err_oss<<"VM ("<< *it <<") can not add affinity region again";
                return ERR_VMCFG_MODIFY_FORBIDDEN;
            }
        }

    }

    AffinityRegionPool *_ppool = AffinityRegionPool::GetInstance();
    if(_ppool == NULL)
    {
        OutPut(SYS_ERROR,"Can not get info of affinity region\n") ;
        err_oss<<"Can not get info of affinity region  ";
        return ERROR;
    }
    tRet = _ppool->GetAffinityRegion(api_data.affinity_region_id,ar);
    if( SUCCESS != tRet)
    {
        OutPut(SYS_ERROR,"Can not get info of affinity region %d\n",api_data.affinity_region_id) ;
        err_oss<<"Error: user does not has affinity region ";
        return ERR_AFFINITY_REGION_NOT_EXIST;
    }

    if (uid != ar.get_uid())
    {
        err_oss<<"Permission deny to add VM to other users' affinity region ("<< ar.get_oid()<<")";
        return ERR_AUTHORIZE_FAILED;
    }

    if(ar._level== AFFINITY_REGION_BOARD)
    {/*�弶�׺������л����ϵ�����������*/
        vector<int64>  vids;
        vector<VmRepel> repels;

        vids.clear();
        tRet = _vcpool->GetVidsInAR(vids, api_data.affinity_region_id);
        if ((SUCCESS != tRet)
             &&(ERROR_OBJECT_NOT_EXIST != tRet))
        {
            err_oss<<"Can not get info of affinity region ("<< api_data.affinity_region_id << ")";
            return ERROR_DB_SELECT_FAIL;
        }
        for (it = api_data.vids.begin();it!= api_data.vids.end();it ++)
        {
            vids.push_back(*it);
        }

        repels.clear();
        tRet = _vcpool->GetVmRepels(vids,repels);
        if ((SUCCESS != tRet)
             &&(ERROR_OBJECT_NOT_EXIST != tRet))
        {
            err_oss<<"Can not get repel vm info ";
            return ERROR_DB_SELECT_FAIL;
        }

        if(repels.size()>0)
        {
            err_oss<<"There are "<< repels.size()<<
            " repels vms in affinity region ("<< api_data.affinity_region_id<< ")." ;
            return ERR_AFFINITY_REGION_HAVE_REPELVM;
        }
    }

    Transaction::Begin();

    /* ���ݿ���� */
    for (it = api_data.vids.begin();
         it!= api_data.vids.end();
         it ++)
    {
        rc = _vcpool->AddVmInAR(*it, api_data.affinity_region_id);

        if (rc < 0)
        {
            //SkyAssert(false);
            OutPut(SYS_DEBUG, "Operate Database failed\n");
            err_oss<< "Operate Database failed";
            return TECS_ERR_DB_INSERT_FAILED;
        }
    }

    if (SUCCESS != Transaction::Commit())
    {
        //SkyAssert(false);
        OutPut(SYS_DEBUG, "Database commit failed\n");
        err_oss<< "Database commit failed";
        return TECS_ERR_DB_INSERT_FAILED;
    }

    return SUCCESS;
}
/******************************************************************************/
STATUS VmCfgOperate::DelVmAffinityRegion(const  ApiAffinityRegionMemberAction  &api_data,
                              int64                   uid,
                              ostringstream           &err_oss)
{
    int     rc  = -1;
    STATUS  tRet = ERROR;
    int64   vm_uid = INVALID_OID;
    string  cluster;
    VmCfg   vm;
    bool is_deploying = false;

    /* У��Ϸ��� */
    /*�Ѳ�������ڲ���vm�������˳��׺���*/
    vector<long long>::const_iterator it;
    for (it = api_data.vids.begin();
         it!= api_data.vids.end();
         it++)
    {
        if ( INVALID_OID == *it)
        {
            err_oss<<"Argument error: VM: "<<*it <<" .";
            return ERROR_INVALID_ARGUMENT;
        }

        tRet  = _vcpool->GetVmById(vm,*it);
        if (SUCCESS!=tRet)
        {
            err_oss<<"VM ("<<*it <<") not exist!";
            return ERR_OBJECT_NOT_EXIST;
        }

        vm_uid     = vm.get_uid();
        if (uid != vm_uid)
        {
            err_oss<<"Permission deny to access VM ("<< *it<<")";
            return ERR_AUTHORIZE_FAILED;
        }

        cluster = vm.get_cluster_name();
        if (!cluster.empty())
        {
            err_oss<<"VM ("<< *it <<") has deployed, can not add affinity region";
            return ERR_VMCFG_MODIFY_FORBIDDEN ;
        }

        is_deploying = _vcpool->IsVmDeploying(vm.get_vid());
        if (is_deploying)
        {
            err_oss<<"VM ("<< *it <<") is deploying, can not add affinity region";
            return ERR_VMCFG_MODIFY_FORBIDDEN ;
        }

    }

    Transaction::Begin();

    /* ���ݿ���� */
    for (it = api_data.vids.begin();
         it!= api_data.vids.end();
         it ++)
    {
        rc = _vcpool->DeleteVmFromAR(*it, api_data.affinity_region_id);

        if (rc < 0)
        {
            //SkyAssert(false);
            Transaction::Cancel();
            OutPut(SYS_DEBUG, "Operate Database failed\n");
            err_oss<< "Operate Database failed";
            return ERR_OBJECT_DROP_FAILED;
        }
    }

    if (SUCCESS != Transaction::Commit())
    {
        //SkyAssert(false);
        OutPut(SYS_DEBUG, "Database commit failed\n");
        err_oss<< "Database commit failed";
        return ERR_OBJECT_DROP_FAILED;
    }

    return SUCCESS;
}
/******************************************************************************/
STATUS VmCfgOperate::QueryByName(const ApiGetVidByName   &req,
                                   int64                     uid,
                                   bool                      is_admin,
                                   vector<int64>            &vids,
                                   ostringstream            &err_oss)
{
    ostringstream   where_string;
    STATUS          tRet = ERROR;

    where_string << " vm_name = '" << req.vm_name << "'";

    ApiVmCfgQueryType type =  static_cast<ApiVmCfgQueryType>(req.query_scope);
    switch (type)
    {
        /* ��ѯ��ǰ�û�������� */
        case VMCFG_USER_CREATE:
        {
            where_string << " AND uid = " << uid;
            break;
        }

        /* ��ѯָ���û�������� */
        case VMCFG_USER_APPOINTED:
        {
            //ֻ�й���Ա����Ȩ��ô��
            if (! is_admin)
            {
                err_oss << "Error, only administrators can query appoined user vmcfgs!";
                return ERR_AUTHORIZE_FAILED;
            }

            where_string << " AND user_name = '" << req.vm_user << "'";
            break;
        }

        /* ��ѯ�����û��������������Ϣ */
        case VMCFG_USER_ALL:
        {
            //ֻ�й���Ա����Ȩ��ô��
            if (!is_admin)
            {
                err_oss << "Error, only administrators can query all vm!";
                return  ERR_AUTHORIZE_FAILED;
            }
            break;
        }

        default:
        {
            err_oss<<"Incorrect query type: "<< req.query_scope;
            return ERR_VMCFG_INVALID_QUERY_TYPE;
            break;
        }
    }


    tRet = _vcpool->GetVidsInFullInfo(vids,where_string.str());
    if (SUCCESS != tRet)
    {
        err_oss << "Failed to get vid list! where string = " << where_string.str();
        return  TECS_ERR_DB_FAIL;
    }

    return SUCCESS;
}

/******************************************************************************/
STATUS VmCfgOperate::Str2VmOp(const string   &action_name,
                          VmOperation    &type,
                          ostringstream  &err_oss)
{
    if (VM_ACTION_DEPLOY == action_name)
    {
        if(ValidateLicense(LICOBJ_DEPLOYED_VMS, 1) != 0)
        {
            err_oss <<"license invalid!" ;
            return TECS_ERR_LICENSE_INVALID;
        }
        type = DEPLOY;
    }
    else if (VM_ACTION_CANCEL == action_name)
    {
        type = CANCEL;
    }
    else if (VM_ACTION_DELETE == action_name)
    {
        type = DELETE;
    }
    else if (VM_ACTION_START == action_name)
    {
        type = START;
    }
    else if (VM_ACTION_STOP == action_name)
    {
        type = STOP;
    }
    else if (VM_ACTION_REBOOT == action_name)
    {
        type = REBOOT;
    }
    else if (VM_ACTION_PAUSE == action_name)
    {
        type = PAUSE;
    }
    else if (VM_ACTION_RESUME== action_name)
    {
        type = RESUME;
    }
    else if (VM_ACTION_RESET == action_name)
    {
        type = RESET;
    }
    else if (VM_ACTION_DESTROY == action_name)
    {
        type = DESTROY;
    }
    else if (VM_ACTION_SYNC == action_name)
    {
        type = SYNC;
    }
    else if (VM_ACTION_UPLOAD_IMG == action_name)
    {
            type = UPLOAD_IMG;
    }
    else if (VM_ACTION_CANCEL_UPLOAD_IMG == action_name)
    {
        type = CANCEL_UPLOAD_IMG;
    }
    else if (VM_ACTION_PLUG_IN_USB == action_name)
    {
        type = PLUG_IN_USB;
    }
    else if (VM_ACTION_PLUG_OUT_USB == action_name)
    {
        type = PLUG_OUT_USB;
    }
    else if (VM_ACTION_OUT_IN_USB == action_name)
    {
        type = OUT_IN_USB;
    }
     else if (VM_ACTION_PLUG_IN_PDISK == action_name)
    {
        type = PLUG_IN_PDISK;
    }
    else if (VM_ACTION_PLUG_OUT_PDISK == action_name)
    {
        type = PLUG_OUT_PDISK;
    }
    else if (VM_ACTION_RECOVER_IMAGE == action_name)
    {
        type = RECOVER_IMAGE;
    }
    else if (VM_ACTION_MODIFY_IMAGE_BACKUP == action_name)
    {
        type = MODIFY_IMAGE_BACKUP;
    }
    else if(VM_ACTION_LIVE_MIGRATE == action_name)
    {
        type = LIVE_MIGRATE;
    }
    else if(VM_ACTION_LIVE_MIGRATE == action_name)
    {
        type = COLD_MIGRATE;
    }
    else if (VM_ACTION_IMAGE_BACKUP_RESTORE== action_name)
    {
        type = RESTORE_IMAGE_BACKUP;
    }
    else if (VM_ACTION_IMAGE_BACKUP_CREATE== action_name)
    {
        type = CREATE_IMAGE_BACKUP;
    }
    else if (VM_ACTION_IMAGE_BACKUP_DELETE== action_name)
    {
        type = DELETE_IMAGE_BACKUP;
    }
    else if (VM_ACTION_FREEZE== action_name)
    {
        type = FREEZE;
    }
    else if (VM_ACTION_UNFREEZE== action_name)
    {
        type = UNFREEZE;
    }
    else
    {
        err_oss <<"Unsupport action : "<< action_name << " \n";
        return ERR_VMCFG_INVALID_ACTION_TYPE;
    }

    return SUCCESS;

}

/******************************************************************************/
STATUS VmCfgOperate::CheckImageFiles(const vector<VmBaseImage> &vec_img,
                                 int64         uid,
                                 const string &virt_type,
                                           const string  &hypervisor_type,
                                 bool          is_by_vt,
                                 ostringstream &oss)
{
    STATUS         tRet = ERROR;
    vector<VmBaseImage>::const_iterator iter;
    ImageMetadata  img;

    for (iter = vec_img.begin();
         iter!=  vec_img.end();
         iter ++)
    {
          /* 1. У���Ƿ���Ȩ��ʹ�� */
          tRet = GetImageMetadata(iter->_image_id, uid, img);
           if (SUCCESS != tRet)
           {               if (ERROR_OBJECT_NOT_EXIST == tRet)
               {
                   oss<<"Image ( "<< iter->_image_id <<" ) not exist";
               }
               else if (ERR_IMAGE_IS_DISABLE == tRet)
               {
                   oss<<"Image ( "<< iter->_image_id <<" ) is disable ";
               }
               else if (ERR_IMAGE_NOT_PUBLIC == tRet)
               {
                    oss<<"Image ( "<< iter->_image_id <<" ) is not public";
               }

               return ERROR;
           }

           /* 2. ��������У��
                 ����ģ��ʱ��У����ˣ�
                 ��ͨ��ģ�崴������������ظ�У�� */
           if(!is_by_vt)
           {
               if (iter->_cfg_type != img._type)
               {
                   oss << "Image ("<< iter->_image_id <<
                              ") type error! original type "<< img._type <<
                              " used as "<< iter->_cfg_type;

                   return ERROR;
               }
           }

           /* 3. ��������У�� */
           if (VIRT_TYPE_HVM == virt_type)
           {
               if ((0 == iter->_target.compare(0,2,"hd")) //ide
                    &&(DISK_BUS_IDE == img._bus))
               {
                   ;
               }
               else if((0 == iter->_target.compare(0,2,"sd")) //scsi
                        &&(DISK_BUS_SCSI == img._bus))
               {
                   ;
               }
               else
               {
                   oss << "Disk name ("<< iter->_target
                       << ") conflict with Image ("<< iter->_image_id
                             << ") "<<img._bus << " bus type !";

                   return ERROR;
               }
           }
           else if(VIRT_TYPE_PVM == virt_type)
           {
               if(0 != iter->_target.compare(0,3,"xvd"))//xvd
               {
                   /*oss << "Disk name ("<< iter->_target
                       << ") error, should be \"xvd*\" .";
                   return ERROR;*/
               }
           }

           /* 4. �����ʽУ�� */
           if ((HYPERVISOR_KVM == hypervisor_type)
               &&(IMAGE_FORMAT_VHD == img._disk_format))
           {
              oss<<"Image ( "<< iter->_image_id <<" ) format is "<<IMAGE_FORMAT_VHD
              << " which is not supported by "<<HYPERVISOR_KVM;
              return ERROR;
           }

   }

   return SUCCESS;
}

/******************************************************************************/
void VmCfgOperate::ParseVmBaseImage(const ApiVmBase &vm_base, vector<VmBaseImage> &vec_img)
{
    // image,disk
    VmBaseImage   vm_base_image(INVALID_OID,
                                    vm_base.machine.target,
                                    vm_base.machine.position,
                                    vm_base.machine.id,
                                    vm_base.machine.reserved_backup,
                                    vm_base.machine.type);

    vec_img.push_back(vm_base_image);

    vector<ApiVmDisk>::const_iterator vec_it_disk;
    for (vec_it_disk  = vm_base.disks.begin();
         vec_it_disk != vm_base.disks.end();
         vec_it_disk++)
    {
        if((INVALID_OID != vec_it_disk->id)
           &&(DISK_TYPE_DISK != vec_it_disk->type))
        {
            VmBaseImage   vm_base_image(INVALID_OID,
                                    vec_it_disk->target,
                                    vec_it_disk->position,
                                    vec_it_disk->id,
                                    vec_it_disk->reserved_backup,
                                    vec_it_disk->type);

            vec_img.push_back(vm_base_image);
        }
    }

}

/******************************************************************************/
void VmCfgOperate::ParseVmBaseDisk(const ApiVmBase &vm_base, vector<VmBaseDisk> &vec_disk)
{
    vector<ApiVmDisk>::const_iterator vec_it_disk;
    for (vec_it_disk  = vm_base.disks.begin();
         vec_it_disk != vm_base.disks.end();
         vec_it_disk++)
    {
        if ((INVALID_OID == vec_it_disk->id)
            &&(DISK_TYPE_DISK == vec_it_disk->type))
        {
            VmBaseDisk vm_base_disk(INVALID_OID,
                                        vec_it_disk->target,
                                        vec_it_disk->position,
                                        vec_it_disk->bus,
                                        vec_it_disk->size,
                                        vec_it_disk->fstype);

            vec_disk.push_back(vm_base_disk);
        }
    }
}


/******************************************************************************/
bool VmCfgOperate::Authorize(int64 opt_uid, VmCfg *buf, int oprate)
{
    /* ��ȡ��Ȩ */
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
        oid      = buf->get_vid();
        self_uid = buf->get_uid();
        if (INVALID_UID == self_uid)
        {
           VmCfgOperate::_vcpool->Drop(*buf);
           return false;
        }
    }
    ar.AddAuthorize(
                    AuthRequest::VMCONFIG,           //��Ȩ��������,��user������
                    oid,                             //�����ID,oid
                    (AuthRequest::Operation)oprate,  //��������
                    self_uid,                        //����ӵ���ߵ��û�ID
                    false                            //�˶����Ƿ�Ϊ����
                    );

    if (-1 == UserPool::GetInstance()->Authorize(ar))
    {   /* ��Ȩʧ�� */

        return false;
    }

    return true;

}

/******************************************************************************/


/******************************************************************************/


}
