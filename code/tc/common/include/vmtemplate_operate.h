/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：vmtemplate_operate.h
* 文件标识：
* 内容摘要：VmTemplateOperate类的定义文件
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
#ifndef VMTEMPLATE_OPERATE_H        
#define VMTEMPLATE_OPERATE_H
#include "sky.h"
#include "mid.h"
#include "event.h"
#include "vmtemplate_pool.h"


namespace zte_tecs
{

class VmTemplateOperate
{
public:

    static VmTemplateOperate *GetInstance( )
    {
        SkyAssert(NULL != _op_inst);
        SkyAssert(NULL != _vtpool);
        
        return _op_inst;
    };

    static VmTemplateOperate *CreateInstance()
    {        
        if(NULL == _op_inst)
        {
            _op_inst = new VmTemplateOperate( );
            _vtpool  = VmTemplatePool::GetInstance();
            SkyAssert(NULL != _vtpool);
        }        
        
        return _op_inst;
    };

    ~VmTemplateOperate(){};  

    STATUS DealAllocate(const ApiVtAllocateData  &req, 
                                           int64          uid, 
                                           ostringstream &err_oss);

    STATUS DealDelete(int64 uid, 
                      const string   &vt_name,
                      ostringstream  &err_oss);  

    STATUS DealModify(const ApiVtModifyData  &req,
                          int64 uid, 
                          ostringstream &err_oss);
                          
    STATUS SetDisks(vector<VmBaseDisk>  &vec_disks,
                          const string   &vt_name,
                          int64          uid,  
                          ostringstream &err_oss);

    STATUS SetImageDisks(vector<VmBaseImage>  &vec_imgs,
                            const string   &vt_name,
                          int64          uid,  
                          ostringstream &err_oss);
                          
    STATUS SetNics(vector<VmBaseNic>  &vec_nics,
                          const string   &vt_name,
                          int64          uid,  
                          ostringstream &err_oss);                     

    STATUS DealPublish(int64           uid,
                           const string   &vt_name, 
                           bool            enable_flag,
                           ostringstream  &err_oss);

    STATUS DealQuery(const ApiVtQuery  &req, 
                        int64               uid,
                        vector<ApiVtInfo>  &vec_vt,
                        int64              &max_count,
                        int64              &next_index,
                        ostringstream      &err_oss); 
                        
    void ParseVmBaseImage(const ApiVmBase &vm_base, 
                                  vector<VmBaseImage> &vec_img);
                                       
    bool Authorize(int64 opt_uid, VmTemplate *buf, int oprate);

    STATUS CheckImageFiles(const vector<VmBaseImage> &vec_img, 
                                 int64       uid,
                                 const string &virt_type,
                                 const string &hypervisor_type,
                                 bool     is_vt_public, 
                                 bool     is_images_change,
                                 ostringstream &err_oss);
                                 
private:
    static  VmTemplateOperate * _op_inst;
    static  VmTemplatePool    * _vtpool;

     VmTemplateOperate(){};

    DISALLOW_COPY_AND_ASSIGN(VmTemplateOperate); 
};



}

#endif /* VMTEMPLATE_OPERATE_H */
