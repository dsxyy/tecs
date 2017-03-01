/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�affinity_region_operate.cpp
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��
* ��ǰ�汾��1.0
* ��    �ߣ�
* ������ڣ�2013��8��26��
*
* �޸ļ�¼1��
*    �޸����ڣ�2013/8/26
*    �� �� �ţ�V1.0
*    �� �� �ˣ�Bob
*    �޸����ݣ�����
*******************************************************************************/
#include "affinity_region_operate.h"
#include "authrequest.h"
#include "authmanager.h"
#include "affinity_region_pool.h"
#include "license_common.h"
#include "vmcfg_pool.h"

namespace zte_tecs
{

AffinityRegionOperate *AffinityRegionOperate::_op_inst= NULL;
AffinityRegionPool    *AffinityRegionOperate::_ppool = NULL;

/******************************************************************************/
STATUS AffinityRegionOperate::DealModify(const ApiAffinityRegionModifyData  &req,
                                         int64          uid,
                                         ostringstream &err_oss)
{
    VmCfgPool     *vcpool = NULL;
    AffinityRegion  ar;
    vector<int64>  vids;
    vector<VmRepel> repels;
    STATUS         tRet =  ERROR;


    tRet = _ppool->GetAffinityRegion(req.id,ar);
    if( SUCCESS != tRet)
    {
        err_oss<<"Error: user does not has affinity region " << req.id <<"!";
        return ERR_AFFINITY_REGION_NOT_EXIST;
    }

    /* ��Ȩ */
    if (Authorize(uid, &ar, AuthRequest::MANAGE) == false)
    {
        err_oss<<"Authorize Fail !";
        return ERR_AFFINITY_REGION_AUTH_FAIL;
    }



    if(uid != ar.get_uid())
    {
        err_oss<<"can not modify other users' affinity region";
        return ERR_AFFINITY_REGION_AUTH_FAIL;
    }

    if((ar._level != req.level)&&(req.level == AFFINITY_REGION_BOARD))
    {
        vcpool = VmCfgPool::GetInstance();
        if(vcpool == NULL)
        {
            err_oss<<"Can not get vm info of affinity region ("<< req.id << ")";
            return ERROR;
        }

        repels.clear();
        tRet = vcpool->GetVmRepelWithAR(req.id, repels);
        if ((SUCCESS != tRet)
             &&(ERROR_OBJECT_NOT_EXIST != tRet))
        {
            err_oss<<"Can not get repel vm info of affinity region ("<< req.id << ")";
            return ERROR;
        }

        if(repels.size()>0)
        {
            err_oss<<"There are "<< repels.size()<<
            " repels vms in affinity region ("<< req.id << "), level can not be board" ;
            return ERR_AFFINITY_REGION_HAVE_REPELVM;
        }

    }

    ar._name = req.name;
    ar._description = req.new_des;
    ar._level = req.level;
    ar._is_forced= req.is_forced;

    /* �������ݿ� */
    int rc = _ppool->Update(ar);
    if ( rc < 0 )
    {
        err_oss<<"Error trying to modify affinity region "<<req.id<< " ." <<endl;
        return ERR_AFFINITY_REGION_UPDATA_FAIL;
    }

    return SUCCESS;

}

/******************************************************************************/
STATUS AffinityRegionOperate::DealDelete(const ApiAffinityRegionDelete &req,
                                      int64  uid,
                                      ostringstream &err_oss)
{
    AffinityRegion  ar;
    vector<int64>  vids;
    VmCfgPool     *vcpool = NULL;
    STATUS         tRet =  ERROR;
    int rc = -1;

    tRet = _ppool->GetAffinityRegion(req.affinity_region_id,ar);
    if( SUCCESS != tRet)
    {
        err_oss<<"Error: user does not has affinity region ("<< req.affinity_region_id << ")";
        return ERR_AFFINITY_REGION_NOT_EXIST;
    }

    /* ��Ȩ */
    if (Authorize(uid, &ar, AuthRequest::DELETE) == false)
    {
        err_oss<<"Authorize Fail !";
        return ERR_AFFINITY_REGION_AUTH_FAIL;
    }


    if(uid != ar.get_uid())
    {
        err_oss<<"can not delete other users' affinity region";
        return ERR_AFFINITY_REGION_AUTH_FAIL;
    }

    vcpool = VmCfgPool::GetInstance();
    if(vcpool == NULL)
    {
        err_oss<<"Can not get vm info of affinity region ("<< req.affinity_region_id << ")";
        return ERROR;
    }

    tRet = vcpool->GetVidsInAR(vids, req.affinity_region_id);
    if ((SUCCESS != tRet)
         &&(ERROR_OBJECT_NOT_EXIST != tRet))
    {
        err_oss<<"Can not get vm info of affinity region ("<< req.affinity_region_id << ")";
        return ERR_AFFINITY_REGION_QUERY_FAIL;
    }

    if(0 != vids.size())
    {
       err_oss<<"There are "<< vids.size()<<
            " vm in affinity region ("<< req.affinity_region_id << ")" ;
       return ERR_AFFINITY_REGION_HAVE_VM;
    }

    rc = _ppool->Drop(ar);
    if (SQLDB_OK != rc)
    {
        err_oss<<"Delete affinity region ("<< req.affinity_region_id << ") from DB failed!" ;
        return ERR_AFFINITY_REGION_DELETE_DB_FAIL;
    }

    err_oss<<"Delete affinity region ("<< req.affinity_region_id << ") success!" ;
    return  SUCCESS;

}

/******************************************************************************/
STATUS AffinityRegionOperate::DealQuery(const ApiAffinityRegionQuery  &req,
                                    int64                    uid,
                                    int64                    appointed_uid,
                                    vector<ApiAffinityRegionInfo>  &vec_ar,
                                    int64                   &max_count,
                                    int64                   &next_index,
                                    ostringstream           &err_oss)
{
    ostringstream  where_string;
    ostringstream  where_string_count;
    int rc = -1;
    vector<ApiAffinityRegionInfo> info;
    QUERY_TYPE    type;

    STATUS tRet = Str2QueryType(req.type, type, err_oss);
    if(SUCCESS != tRet)
    {
        return  tRet;
    }

    switch (type )
    {
        case QUERY_ALL_SELF:
        {

            where_string << " uid = " << uid <<\
                          " order by oid limit " <<req.count <<\
                          " offset " << req.start_index ;
            where_string_count << " uid = " << uid;

            break;
        }

        case QUERY_ALL:
        {
            /* ��Ȩ,����д������ֻ�й���Ա����Ȩ�޲���ͨ�� */
            if (Authorize(uid, NULL,  AuthRequest::INFO) == false)
            {
                err_oss<<"Authorize Fail : " <<
                       "only adminstrator can query all user's project";
                return  ERR_AFFINITY_REGION_AUTH_FAIL;
            }

            where_string << " 1=1 " <<\
                          " order by oid limit " <<req.count <<\
                          " offset " << req.start_index ;
            where_string_count << " 1=1 ";
            break;
        }

        case QUERY_ALL_APPOINTED:
        {
            /* ��Ȩ,����д������ֻ�й���Ա����Ȩ�޲���ͨ�� */
            if (Authorize(uid, NULL, AuthRequest::INFO) == false)
            {
                err_oss<<"Authorize Fail : " <<
                       "only adminstrator can query other user's affinity region";
                return  ERR_AFFINITY_REGION_AUTH_FAIL;
            }
            where_string << "uid = " << appointed_uid <<\
                          " order by oid limit " << req.count <<\
                          " offset " << req.start_index ;
            where_string_count << " uid = " << appointed_uid ;
            break;
        }

        case QUERY_SELF_APPOINTED:
        {
            where_string << " oid = " << req.appointed_id<<
                        " and uid = " << uid;
            where_string_count << " oid = " << req.appointed_id<<
                        " and uid = " << uid;
            AffinityRegion  ar;
            tRet = _ppool->GetAffinityRegion(req.appointed_id,ar);
            if( SUCCESS != tRet)
            {
                err_oss<<"Error: user does not has affinity region ("<< req.appointed_id << ")";
                return ERR_AFFINITY_REGION_NOT_EXIST;
            }

            /* ��Ȩ */
            if (Authorize(uid, &ar, AuthRequest::INFO) == false)
            {
                err_oss<<"Authorize Fail !";
                return  ERR_AFFINITY_REGION_AUTH_FAIL;
            }

            break;
        }

        default:
        {
            err_oss<<"Incorrect query type: "<< req.type <<endl;
            return ERR_AFFINITY_REGION_QUERY_TYPE_ERR;
            break;
        }
    }

    max_count = _ppool->Count(where_string_count.str());
    if(-1 == max_count)
    {
        err_oss<<"Query failed !" ;
        return ERR_AFFINITY_REGION_QUERY_FAIL;
    }

    rc = _ppool->Show(info, where_string.str());

    if (rc != 0)
    {
        err_oss<<"Query failed !" ;
        return ERR_AFFINITY_REGION_QUERY_FAIL;
    }

    int64  out_num = 0;

    //��βҳ
    if (info.size() >= (uint64)(req.count ))
    {
        next_index = req.count + req.start_index;
        out_num = req.count;
    }
    else  //βҳ
    {
        next_index = -1;
        out_num = info.size();
    }


    /* �Ѳ�ѯ�Ľ���ŵ���Ϣ�� */
    for (int64 i = 0; i < out_num; i++)
    {
         vec_ar.push_back(info.at(i));
    }

    return SUCCESS;

}

/******************************************************************************/
bool AffinityRegionOperate::Authorize(int64 opt_uid, AffinityRegion *buf, int oprate)
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
        oid      = buf->get_oid();
        self_uid = buf->get_uid();
        if (INVALID_UID == self_uid)
        {
           AffinityRegionOperate::_ppool->Drop(*buf);
           return false;
        }
    }
    ar.AddAuthorize(
                    AuthRequest::AFFINITY_REGION,           //��Ȩ��������,��user������
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
STATUS AffinityRegionOperate::Str2QueryType(const string   &query_name,
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
    else if (RPC_QUERY_ALL_APPOINTED== query_name)
    {
        type = QUERY_ALL_APPOINTED;
    }
    else
    {
        err_oss <<"Unsupport query type : "<< query_name << " \n";
        return ERR_AFFINITY_REGION_QUERY_TYPE_ERR;
    }

    return SUCCESS;
}


/******************************************************************************/


/******************************************************************************/


}
