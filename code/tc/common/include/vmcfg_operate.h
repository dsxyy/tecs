/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�vmcfg_operate.h
* �ļ���ʶ��
* ����ժҪ��VmCfgOperate��Ķ����ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�Bob
* ������ڣ�2011��7��26��
*
* �޸ļ�¼1��
*    �޸����ڣ�2011/7/26
*    �� �� �ţ�V1.0
*    �� �� �ˣ�Bob
*    �޸����ݣ�����
*******************************************************************************/
#ifndef VMCFG_OPERATE_H
#define VMCFG_OPERATE_H
#include "sky.h"
#include "mid.h"
#include "event.h"
#include "vmcfg_pool.h"
#include "vm_messages.h"

namespace zte_tecs
{

class QuerySession
{
public:
    string        agent_mid;
    int64         uid;
    bool          is_admin;
    int64         next_index;
    int64         max_count;     //�������������������
    int           wait_cc_msgs;
    int           wait_vnm_msgs;
    int           out_num;        //��ҳ��ѯ�е���������
    vector<ApiVmStaticInfo>            _vc_info;;
    vector<ApiVmRunningInfo>           _vc_runinfo;
    vector<int64>                      _need_query_network_vids;//��������VNM��������Ϣ��VM
    map<string, ClusterVmRunningInfo>  _cluster_vcs;//������Ҫ��CC ��ѯ״̬��VM
} ;


class VmCfgOperate
{
public:

    static VmCfgOperate *GetInstance( )
    {
        SkyAssert(NULL != _op_inst);

        return _op_inst;
    };

    static VmCfgOperate *CreateInstance( )
    {
        if(NULL == _op_inst)
        {
            _op_inst = new VmCfgOperate( );
            _vcpool  = VmCfgPool::GetInstance();
        }

        return _op_inst;
    };

    ~VmCfgOperate(){};

    STATUS DealDirectCreate(const ApiVmCfgAllocateData  &req,
                                 int64          uid,
                                 vector<int64>         &vids,
                                 ostringstream &err_oss);

    STATUS DealClone(ApiVmStaticInfo  &req,int64 uid, int64 &clone_vid,ostringstream &oss);

    STATUS DealModify(const ApiVmCfgModifyData  &req,
                      int64          uid,
                      ostringstream &oss);

    STATUS DealDelete(int64 vid, int64 uid, ostringstream &err_oss);

    STATUS DealQuery(const ApiVmCfgQuery &req,
                        int64                uid,
                        bool                 is_admin,
                        QuerySession        &query_session,
                        ostringstream       &err_oss);

    STATUS SetDisks(vector<VmBaseDisk>  &vec_disks,
                          int64          vid,
                          int64          uid,
                          ostringstream &err_oss);

    STATUS SetImageDisks(vector<VmBaseImage>  &vec_imgs,
                              int64          vid,
                              int64          uid,
                              ostringstream &err_oss);

    STATUS SetNics(vector<VmBaseNic>  &vec_nics,
                           int64          vid,
                          int64          uid,
                          ostringstream &err_oss);

    STATUS SetVmRelation(const  ApiVmCfgRelationData  &api_data,
                              int64                   uid,
                              ostringstream           &err_oss, string &wf_id);
    STATUS SetDeployedVmRelation(const  ApiVmCfgRelationData  &api_data,
                              int64                   uid,
                              ostringstream           &err_oss, string &wf_id);

    STATUS DealQueryVmRelation(int64  vid,
                        int64                uid,
                        vector<ApiVmCfgMap> &vec_vm,
                        ostringstream       &err_oss);

    STATUS AddVmAffinityRegion(const  ApiAffinityRegionMemberAction  &api_data,
                              int64                   uid,
                              ostringstream           &err_oss);

    STATUS DelVmAffinityRegion(const  ApiAffinityRegionMemberAction  &api_data,
                              int64                   uid,
                              ostringstream           &err_oss);



    STATUS CheckAction(int64   uid,
                          int64  vid,
                          const string  &action_name,
                          VmOperation   &action_type,
                          ostringstream &err_oss);

    STATUS CheckOpQuery(VmCfg         &vmcfg,
                                              int64          op_uid,
                                              ostringstream &err_oss);

    STATUS CheckOpDeploy(VmCfg         &vmcfg,
                             int64          op_uid,
                             ostringstream &err_oss);

    STATUS CheckOpDelete(VmCfg         &vmcfg,
                             int64          op_uid,
                             ostringstream &err_oss);

    STATUS CheckOpCancel(VmCfg         &vmcfg,
                             int64          op_uid,
                             ostringstream &err_oss);

    STATUS CheckOpNormal(VmCfg   &vmcfg,
                              int64    op_uid,
                              ostringstream &err_oss);

    STATUS CheckOpMigrate(int64   op_uid,
                                 int64          vid,
                                 bool          blive,
                                 string        &cluster_name,
                                 ostringstream &err_oss);

    STATUS PrepareSynMsg(VmCfg        &vmcfg,
                                   const string &session_stamp,
                                   VMDeployInfo &deploy_msg,
                                   ostringstream &err_oss);


    STATUS QueryByName(const ApiGetVidByName   &req,
                                   int64                     uid,
                                   bool                      is_admin,
                                   vector<int64>            &vids,
                                   ostringstream            &err_oss);

    STATUS Str2VmOp(const string   &action_name,
                          VmOperation    &type,
                          ostringstream  &err_oss);

    STATUS Str2QueryType(const string   &query_name,
                          QUERY_TYPE    &type,
                          ostringstream  &err_oss);

    void ParseVmBaseImage(const ApiVmBase &base, vector<VmBaseImage> &vec_img);

    void ParseVmBaseDisk(const ApiVmBase &vm_base, vector<VmBaseDisk> &vec_disk);

    bool Authorize(int64 opt_uid, VmCfg *buf, int oprate);

    STATUS CheckImageFiles(const vector<VmBaseImage> &vec_img,
                                 int64       uid,
                                 const string &virt_type,
                                 const string &hypervisor_type,
                                 bool        is_by_vt,
                                 ostringstream &err_oss);

    STATUS GetVmcfg(int64 vid, VmCfg &cfg, string &name, ostringstream &oss);
    STATUS GetVmcfg(int64 vid, string &name, ostringstream &oss);


private:
    static  VmCfgOperate * _op_inst;
    static  VmCfgPool    * _vcpool;

    VmCfgOperate() { };

};



}

#endif /* VMCFG_OPERATE_H */
