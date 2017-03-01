/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：vmcfg_operate.cpp
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
#include "vmtemplate_operate.h"
#include "authrequest.h"
#include "authmanager.h"
#include "image_db.h"

namespace zte_tecs
{

VmTemplateOperate *VmTemplateOperate::_op_inst= NULL;
VmTemplatePool    *VmTemplateOperate::_vtpool = NULL;

/******************************************************************************/

int VmTemplateOperate::DealAllocate(const ApiVtAllocateData  &req,
                                           int64            uid,
                                           ostringstream   &err_oss)
{
    string           error_str;
    VmTemplate       vt;

    /* 授权 */
    if (Authorize(uid, NULL, AuthRequest::CREATE) == false)
    {
        err_oss<<"Authorize Fail";
        return  ERR_AUTHORIZE_FAILED;
    }

    /* 查重 */
    STATUS tRet = _vtpool->GetVmTemplate(vt,req.vt_name);
    if(SUCCESS == tRet)
    {
        err_oss<<"Name conflict";
        return ERR_VMTEMPLATE_NAME_CONFLICT;
    }

    /* 校验映像文件 */
    vector<VmBaseImage>  vec_img;
    ParseVmBaseImage(req.cfg_info.base_info, vec_img);
    tRet = CheckImageFiles(vec_img,
                           uid,
                           req.cfg_info.base_info.virt_type,
                           req.cfg_info.base_info.hypervisor,
                           req.is_public,
                           true,
                           err_oss);
    if (ERR_IMAGE_NOT_PUBLIC == tRet)
    {
        return ERR_VMTEMPLATE_IMAGE_NOT_PUBLIC;
    }
    if (SUCCESS != tRet)
    {
        return ERR_VMTEMPLATE_ACQUIRE_IMAGE_FAILED;
    }

    /*  创建一个新的vt对象  */
    vt.Init(req, uid);
    int rc = _vtpool->Allocate(vt);
    if (rc != SUCCESS)
    {
        err_oss <<"Create VmTemplate instance failed, "<< endl;
        return ERR_OBJECT_ALLOCATE_FAILED;
    }

    err_oss <<"Success ";

    return  SUCCESS ;

}

/******************************************************************************/
STATUS  VmTemplateOperate::DealDelete(int64 uid,
                                         const string   &vt_name,
                                         ostringstream  &err_oss)
{
    VmTemplate      vt;

    /* 搜索vt pool中是否已有该vt? */
    int rc = _vtpool->GetVmTemplate(vt,vt_name);
    if (SUCCESS!= rc)
    {
        err_oss <<"VmTemplate ("<<vt_name<<") is not exist !" << endl;
        return ERR_OBJECT_NOT_EXIST;
    }

    /* 授权 */
    if (Authorize(uid, &vt, AuthRequest::DELETE) == false)
    {
        err_oss<<"Authorize Fail";
        return  ERR_AUTHORIZE_FAILED ;
    }

    rc = _vtpool->Drop(&vt);
    if (0 != rc)
    {
        err_oss <<"Delete VmTemplate ("<<vt_name<<") failed !" << endl;
        return  ERR_OBJECT_DROP_FAILED;
    }

    err_oss <<"Delete VmTemplate ("<<vt_name<<") success !" << endl;

    return SUCCESS;

}

/******************************************************************************/
STATUS VmTemplateOperate::DealModify(const ApiVtModifyData  &req,
                                         int64          uid,
                                         ostringstream &err_oss)
{
    /* 获取vt对象 */
    VmTemplate vt;
    int rc = _vtpool->GetVmTemplate(vt, req.vt_name);
    if (SUCCESS!= rc)
    {
        err_oss<<"VmTemplate (" <<req.vt_name << ") not exist!";
        return ERR_OBJECT_NOT_EXIST;
    }

    /* 授权 */
    if (Authorize(uid, &vt, AuthRequest::MANAGE) == false)
    {
        err_oss<<"Authorize Fail";
        return ERR_AUTHORIZE_FAILED;
    }

    /* 校验映像文件 */
    vector<VmBaseImage>  vec_img;
    ParseVmBaseImage(req.cfg_info.base_info, vec_img);
    STATUS tRet = CheckImageFiles(vec_img,
                           uid,
                           req.cfg_info.base_info.virt_type,
                           req.cfg_info.base_info.hypervisor,
                           req.is_public,
                           true,
                           err_oss);
    if (ERR_IMAGE_NOT_PUBLIC == tRet)
    {
        return ERR_VMTEMPLATE_IMAGE_NOT_PUBLIC;
    }
    if (SUCCESS != tRet)
    {
        return ERR_VMTEMPLATE_ACQUIRE_IMAGE_FAILED;
    }

    // b. 更新虚拟机配置
    vt.Modify(req);
    rc =_vtpool->Update(vt);
    if (rc < 0)
    {
        err_oss <<"Update VmTemplate ("<< req.vt_name <<") failed !";
        return ERR_OBJECT_UPDATE_FAILED;
    }

    err_oss<< "Success";
    return SUCCESS;

}

/******************************************************************************/
STATUS VmTemplateOperate::SetDisks(vector<VmBaseDisk>  &vec_disks,
                          const string   &vt_name,
                          int64          uid,
                          ostringstream &err_oss)
{
    VmTemplate vt;
    int rc = _vtpool->GetVmTemplate(vt,vt_name);
    if (SUCCESS!= rc)
    {
        err_oss<<"Get VmTemplate ("<< vt_name << ") failed! "<< endl ;
        return ERR_OBJECT_NOT_EXIST;
    }

    /* 授权 */
    if (Authorize(uid, &vt, AuthRequest::MANAGE) == false)
    {
        err_oss<<"Authorize Fail";
        return ERR_AUTHORIZE_FAILED;
    }

    vt.set_disks(vec_disks);
    /* 更新数据库 */
    rc = _vtpool->Update(vt);
    if (0 != rc)
    {
        err_oss<<"Error trying to set VmTemplate ("<<vt_name<< ") disk." <<endl;
        return ERR_OBJECT_UPDATE_FAILED;
    }

    return SUCCESS;
}

/******************************************************************************/
STATUS VmTemplateOperate::SetImageDisks(vector<VmBaseImage>  &vec_imgs,
                            const string   &vt_name,
                          int64          uid,
                          ostringstream &err_oss)
{
    VmTemplate vt;
    int rc = _vtpool->GetVmTemplate(vt,vt_name);
    if (SUCCESS!= rc)
    {
        err_oss<<"Get VmTemplate ("<< vt_name << ") failed! "<< endl ;
        return ERR_OBJECT_NOT_EXIST;
    }

    /* 授权 */
    if (Authorize(uid, &vt, AuthRequest::MANAGE) == false)
    {
        err_oss<<"Authorize Fail";
        return ERR_AUTHORIZE_FAILED;
    }

    //校验映像文件
    STATUS tRet = CheckImageFiles(vec_imgs,
                                  uid,
                                  vt.get_virt_type(),
                                  vt.get_hypervisor(),
                                  vt.get_public(),
                                  true,
                                  err_oss);
    if (SUCCESS != tRet)
    {
        return ERR_VMCFG_ACQUIRE_IMAGE_FAILED;
    }

    vt.set_images(vec_imgs);
    /* 更新数据库 */
    rc = _vtpool->Update(vt);
    if (0 != rc)
    {
        err_oss<<"Error trying to set VmTemplate ("<<vt_name<< ") imagedisk." <<endl;
        return ERR_OBJECT_UPDATE_FAILED;
    }

    return SUCCESS;
}
/******************************************************************************/
STATUS VmTemplateOperate::SetNics(vector<VmBaseNic>  &vec_nics,
                          const string   &vt_name,
                          int64          uid,
                          ostringstream &err_oss)
{
    VmTemplate vt;
    int rc = _vtpool->GetVmTemplate(vt,vt_name);
    if (SUCCESS!= rc)
    {
        err_oss<<"Get VmTemplate ("<< vt_name << ") failed! "<< endl ;
        return ERR_OBJECT_NOT_EXIST;
    }

    /* 授权 */
    if (Authorize(uid, &vt, AuthRequest::MANAGE) == false)
    {
        err_oss<<"Authorize Fail";
        return ERR_AUTHORIZE_FAILED;
    }

    vt.set_nics(vec_nics);

    /* 更新数据库 */
    rc = _vtpool->Update(vt);
    if (0 != rc)
    {
        err_oss<<"Error trying to set  VmTemplate ("<<vt_name<< ")  nic." <<endl;
        return ERR_OBJECT_UPDATE_FAILED;
    }

    return SUCCESS;

}

/******************************************************************************/
STATUS VmTemplateOperate::DealPublish(int64           uid,
                           const string   &vt_name,
                           bool            publish_flag,
                           ostringstream  &err_oss)
{
    STATUS      tRet = ERROR;
    vector<VmBaseImage> vec_image;

    VmTemplate  vt;

    int rc = _vtpool->GetVmTemplate(vt, vt_name);
    if (SUCCESS!= rc)
    {
        err_oss<<"VmTemplate (" << vt_name << ") not exist!";
        return ERR_OBJECT_NOT_EXIST;
    }

    /* 授权 */
    if (Authorize(uid, &vt, AuthRequest::MANAGE) == false)
    {
        err_oss<<"Authorize Fail";
        return ERR_AUTHORIZE_FAILED;
    }

    bool is_public = vt.get_public();
    if( is_public == publish_flag)
    {
        goto publish_success;
    }

    if ((true == publish_flag)) //发布模板需要校验映像文件
    {
        vt.get_images(vec_image);
        //校验映像文件
        tRet = CheckImageFiles(vec_image,
                               uid,
                               vt.get_virt_type(),
                               vt.get_hypervisor(),
                               publish_flag,
                               false,
                               err_oss);
        if (ERR_IMAGE_NOT_PUBLIC == tRet)
        {
            return ERR_VMTEMPLATE_IMAGE_NOT_PUBLIC;
        }
        if (SUCCESS != tRet)
        {
            return ERR_VMTEMPLATE_ACQUIRE_IMAGE_FAILED;
        }
    }
    vt.set_public(publish_flag);
    _vtpool->Update(vt);

publish_success:
    if (true == publish_flag)
    {
        err_oss<<"Success to Public ";
    }
    else
    {
        err_oss<<"Success to Private ";
    }
    err_oss<<"VmTemplate ("<<vt_name<< ")." <<endl;

    return SUCCESS;

}

/******************************************************************************/
STATUS VmTemplateOperate::DealQuery(const ApiVtQuery  &req,
                                        int64               uid,
                                        vector<ApiVtInfo>  &vec_vt,
                                        int64              &max_count,
                                        int64              &next_index,
                                        ostringstream      &err_oss)
{
    ostringstream      where_string;
    ostringstream      where_string_count;
    int64              out_num = 0;

    bool  is_admin =  Authorize(uid, NULL, AuthRequest::INFO);

    ApiVtQueryType    query_type = static_cast<ApiVtQueryType>(req.type);
    switch (query_type)
    {
        /* 该用户创建的模板文件  */
    case VT_USER_CREATE:
    {
        where_string << " UID = " << uid <<\
                        " order by oid limit " <<  req.count<<\
                		" offset "<< req.start_index;
        where_string_count << " UID = " << uid;
        break;
    }

    /* 该用户可见的模板 */
    case VT_USER_VISIBLE:
    {
        where_string << " UID = " << uid << " OR ( is_public=1 and UID IS NOT NULL ) "<<\
                       " order by oid limit " << req.count <<\
                	   " offset "<< req.start_index;
        where_string_count << " UID = " << uid << " OR ( is_public = 1 and UID IS NOT NULL )";
        break;
    }

    /* 指定查询 */
    case VT_APPOINTED:
    {
        where_string << " NAME = '" << req.vt_name <<"'"<< " AND UID IS NOT NULL";
        where_string_count << " NAME = '" << req.vt_name <<"'" << " AND UID IS NOT NULL" ;
        break;
    }

    default:
    {
        err_oss<<"Incorrect query type: "<< req.type <<endl;
        return ERR_VMTEMPLATE_INVALID_QUERY_TYPE ;
        break;
    }
    }

    /* 取满足条件的记录总数 */
    vector<int64>   tids;
    int rc = _vtpool->GetTids(tids, where_string_count.str());
    if ((SUCCESS != rc)
         &&(ERROR_OBJECT_NOT_EXIST != rc))
    {
        err_oss<<"Query failed !"<<endl;
        return ERROR_DB_SELECT_FAIL;
    }

    max_count = tids.size();

    /* 下面取最大值和下包数据 */
    vector<ApiVtInfo>  info;
    rc = _vtpool->Show(info, where_string.str());
    if (SQLDB_OK != rc)
    {
        err_oss<<"Query failed !"<<endl;
        return ERROR_DB_SELECT_FAIL;
    }

    /* 授权 */
    if (VT_APPOINTED == req.type)
    {
        if ((info.size() == 1)
             &&(!is_admin) // 非管理员
             &&(info.at(0).is_public != true))    // 未发布
        {
            VmTemplate  vt;
            _vtpool->GetVmTemplate(vt, req.vt_name);

            if(uid != vt.get_uid()) // 非所有者
            {
               err_oss<<"Authorize Fail";
               return ERR_AUTHORIZE_FAILED;
            }
        }
    }

    if (info.size() >= (uint64)(req.count))
    {
        next_index = req.start_index + req.count;
        out_num    = req.count;
    }
    else
    {
        next_index = -1;
        out_num = info.size();
    }

    for (int index = 0; index < out_num; index++)
    {
        if (!is_admin) //该字段对普通租户不可见
        {
            vector<ApiVtInfo>::iterator iter = info.begin() + index;
            iter->cfg_info.base_info.cluster = "";
            iter->cfg_info.base_info.host = "";
        }
        vec_vt.push_back(info.at(index));
    }

    return SUCCESS;

}


/******************************************************************************/
void VmTemplateOperate::ParseVmBaseImage(const ApiVmBase &vm_base, vector<VmBaseImage> &vec_img)
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
STATUS VmTemplateOperate::CheckImageFiles(const vector<VmBaseImage> &vec_img,
                                                int64    uid,
                                                const string &virt_type,
                                                const string &hypervisor_type,
                                                bool     is_vt_public,
                                                bool     is_images_change,
                                                ostringstream   &err_oss)
{
    STATUS         tRet = ERROR;
    vector<VmBaseImage>::const_iterator iter;
    ImageMetadata  img;
    ostringstream  oss;

    for (iter = vec_img.begin();
         iter!=  vec_img.end();
         iter ++)
    {
        /* 1. 是否有权限使用映像 */
          tRet = GetImageMetadata(iter->_image_id, uid, img);
           if (SUCCESS != tRet)
           {
               if (ERROR_OBJECT_NOT_EXIST == tRet)
               {
                   err_oss<<"Image ( "<< iter->_image_id <<" ) not exist";
               }
               else if (ERR_IMAGE_IS_DISABLE == tRet)
               {
                   err_oss<<"Image ( "<< iter->_image_id <<" ) is disable";
               }
               else if (ERR_IMAGE_NOT_PUBLIC == tRet)
               {
                   err_oss<<"Image ( "<< iter->_image_id <<" ) is not public";
               }
               return ERROR;
           }

           /* 2. 模板能否被公开 */
           if((uid == img._uid ) //映像所有者
               && (false == img._is_public) //映像私有
               && (true  == is_vt_public))//模板共有
           {
               err_oss<<"Vmtemplate cannot public,"
                       " for Image( "<< iter->_image_id <<" ) not public";
               return ERR_IMAGE_NOT_PUBLIC;
           }

           /* 3. 磁盘类型: 只有image信息修改了才校验 */
           if (is_images_change)//?
           {
               if (iter->_cfg_type != img._type)
               {
                   err_oss << "Image ("<< iter->_image_id <<
           		   	    ") type error! original type "<< img._type <<
           		   	    " used as "<< iter->_cfg_type;
                   return ERROR;
               }
           }

            /* 4. 总线类型校验 */
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
                   err_oss << "Disk name ("<< iter->_target
                       << ") conflict with Image ("<< iter->_image_id
           		   	   << ") "<<img._bus << " bus type !";

                   return ERROR;
               }
           }
           else if(VIRT_TYPE_PVM == virt_type)
           {
               if(0 != iter->_target.compare(0,3,"xvd"))//xvd
               {
                   /*err_oss << "Disk name ("<< iter->_target
                       << ") error, should be \"xvd*\" .";

                   return ERROR;*/
               }
           }

            /* 4. 镜像格式校验 */
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
bool VmTemplateOperate::Authorize(int64 opt_uid, VmTemplate *buf, int oprate)
{
    /* 获取授权 */
    AuthRequest ar(opt_uid);
    int64 oid;
    int64 self_uid;

    if (buf == NULL)
    {
        oid = 0;
        self_uid = 0;
    }
    else
    {
        oid = buf->get_tid();
        self_uid = buf->get_uid();
        if (INVALID_UID == self_uid)
        {
            _vtpool->Drop(buf);
            return false;
        }
    }

    ar.AddAuthorize(AuthRequest::VMTEMPLATE,          //授权对象类型,用user做例子
                     oid,                             //对象的ID,oid
                     (AuthRequest::Operation)oprate,  //操作类型
                     self_uid,                        //对象拥有者的用户ID
                     false);                          //此对象是否为公有


    if (-1 == UserPool::Authorize(ar))
    {   /* 授权失败 */

        return false;
    }

    return true;

}

/******************************************************************************/


/******************************************************************************/


}
