/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：vmtemplate.h
* 文件标识：见配置管理计划书
* 内容摘要：模版类的定义文件
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

#ifndef IMAGE_MANGER_VMTEMPLATE_H        
#define IMAGE_MANGER_VMTEMPLATE_H

#include "sky.h"
#include "vmtpl_api.h"
#include "vmcfgbase.h"


namespace zte_tecs
{

/**
@brief 功能描述: 模版管理的实体类\n
@li @b 其它说明: 无
*/
class VmTemplate : public Serializable
{
  
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
 

    VmTemplate()
    {
        _oid       = INVALID_OID;
        _vcpu      = 0;
        _tcu       = 0;
        _memory    = 0;
        _watchdog  = 0;
        _uid       = INVALID_OID;
        _is_public = false;        
        
        _local_images_size = 0;
        _share_images_size = 0;
        _local_disks_size  = 0;
        _share_disks_size  = 0;
        _enable_coredump = false;
        _enable_serial = false;
        _enable_livemigrate = true;
    };

    void Init(const ApiVtAllocateData &allocate_msg, int64 uid)
    {
        _oid               = INVALID_OID;  
        _local_images_size = 0;
        _share_images_size = 0;
        _local_disks_size  = 0;
        _share_disks_size  = 0; 
        
        _uid            = uid; 
        _is_public      = allocate_msg.is_public;
        _name           = allocate_msg.vt_name;
        
        _description    = allocate_msg.cfg_info.base_info.description;
        _create_time.refresh();
        _vcpu           = allocate_msg.cfg_info.base_info.vcpu;
        _tcu            = allocate_msg.cfg_info.base_info.tcu;
        _memory         = allocate_msg.cfg_info.base_info.memory;
 
        _virt_type      = allocate_msg.cfg_info.base_info.virt_type;
        _root_device    = allocate_msg.cfg_info.base_info.root_device;
        _kernel_command = allocate_msg.cfg_info.base_info.kernel_command;
        _bootloader     = allocate_msg.cfg_info.base_info.bootloader;
        _raw_data       = allocate_msg.cfg_info.base_info.rawdata;
        
        _vnc_passwd         = allocate_msg.cfg_info.base_info.vnc_passwd;
        _appointed_cluster  = allocate_msg.cfg_info.base_info.cluster;
        _appointed_host     = allocate_msg.cfg_info.base_info.host;
        _watchdog           = allocate_msg.cfg_info.base_info.vm_wdtime;
        _enable_coredump = allocate_msg.cfg_info.base_info.enable_coredump;
        _enable_serial = allocate_msg.cfg_info.base_info.enable_serial;
        _hypervisor         = allocate_msg.cfg_info.base_info.hypervisor;
        _enable_livemigrate = allocate_msg.cfg_info.base_info.enable_livemigrate;
        
        // image,disk
        VmBaseImage   vm_base_image(INVALID_OID,
                                    allocate_msg.cfg_info.base_info.machine.target,
                                    allocate_msg.cfg_info.base_info.machine.position,
                                    allocate_msg.cfg_info.base_info.machine.id,
                                    allocate_msg.cfg_info.base_info.machine.reserved_backup,
                                    allocate_msg.cfg_info.base_info.machine.type);
        _images.push_back(vm_base_image);

        vector<ApiVmDisk>::const_iterator vec_it_disk; 
        for (vec_it_disk  = allocate_msg.cfg_info.base_info.disks.begin();
             vec_it_disk != allocate_msg.cfg_info.base_info.disks.end();
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
                                        
                _disks.push_back(vm_base_disk);
            }
            else if((INVALID_OID != vec_it_disk->id)
                     &&(DISK_TYPE_DISK != vec_it_disk->type))
            {
                VmBaseImage   vm_base_image(INVALID_OID,
                                    vec_it_disk->target,
                                    vec_it_disk->position,
                                    vec_it_disk->id,
                                    vec_it_disk->reserved_backup,
                                    vec_it_disk->type);
                                    
               _images.push_back(vm_base_image);
            } 
        }

        //nic
        vector<ApiVmNic>::const_iterator vec_it_nic; 
        for (vec_it_nic  = allocate_msg.cfg_info.base_info.nics.begin();
             vec_it_nic != allocate_msg.cfg_info.base_info.nics.end();
             vec_it_nic++)
        {           
            VmBaseNic   vm_base_nic(INVALID_OID,
                            vec_it_nic->nic_index,
                            vec_it_nic->sriov,
                            vec_it_nic->loop,                            
                            vec_it_nic->logic_network_id,
                            vec_it_nic->model,
                            "");
 
            _nics.push_back(vm_base_nic);
        }

        // context
        map<string, string>::const_iterator  it_context;
        for (it_context  = allocate_msg.cfg_info.base_info.context.begin();
             it_context != allocate_msg.cfg_info.base_info.context.end();
             it_context++)
        {           
            _context._id = INVALID_OID;
 
            _context._str_map.insert(make_pair(it_context->first, it_context->second));
        }

        // qualifications
        vector<ApiVmExpression>::const_iterator vec_it_exp; 
        for (vec_it_exp  = allocate_msg.cfg_info.base_info.qualifications.begin();
             vec_it_exp != allocate_msg.cfg_info.base_info.qualifications.end();
             vec_it_exp++)
        {
            Expression  exp(vec_it_exp->key,
                            vec_it_exp->op,
                            vec_it_exp->value);
            
            _qualifications._qua.push_back(exp);
        }

        //_devices            = allocate_msg.base_info.devices;
        _devices.assign(allocate_msg.cfg_info.base_info.devices.begin(), allocate_msg.cfg_info.base_info.devices.end());

        return ;
    };

    ~VmTemplate(){};

    /**
     * Get the VmTemplate unique identifier IID, that matches the OID of the object
     *    @return IID VmTemplate identifier
     */
    int64 get_tid() const
    {
        return _oid;  //   chenww0728
    }; 

    /**
     * Gets the uid of the owner of the VmTemplate
     * @return uid
     **/
    int64 get_uid() const
    {
        return _uid;
    }

    const string& get_name() const
    {
        return _name;
    };

    /**
     *  Returns true if the vm is public
     *     @return true if the vm is public
     */
    bool get_public()
    {
        return _is_public;
    };

    string get_virt_type()const
    {
        return _virt_type;
    };

    string get_hypervisor() const
    {
        return _hypervisor;
    };
    
    void get_images(vector<VmBaseImage>  &images)
    {
        images = _images;
    };
    
    void get_disks(vector<VmBaseDisk>  &disks)
    {
        disks = _disks;
    };
    
    void get_nics(vector<VmBaseNic>  &nics)
    {
        nics = _nics;
    };

    int64 get_total_size() const
    {
        return _local_images_size + _share_images_size + _local_disks_size +_share_disks_size;  
    }
    
    void set_name(const string &name)    
    {
        _name = name;
    };

    /**
     *  Publish or unpublish an image
     *    @param pub true to publish the image
     *    @return 0 on success
     */
    void set_public(bool to_public)    
    {
        if (true == to_public)
        {
            _is_public = true;
        }
        else
        {
            _is_public = false;
        }
    };

    void  set_images(const vector<VmBaseImage>  &images)
    {
        _images.clear();
        _images = images;
    };
     
    void set_disks(const vector<VmBaseDisk>  &disks)
    {
        _disks.clear();
        _disks = disks;
    };

    void set_nics(const vector<VmBaseNic>  &nics)
    {
        _nics.clear();
        _nics = nics;
    };

    void Modify(const ApiVtModifyData &modify_msg);
    SERIALIZE
    {
         SERIALIZE_BEGIN(VmTemplate);
         WRITE_VALUE(_oid);
         WRITE_VALUE(_name);
         WRITE_VALUE(_description);
         WRITE_OBJECT(_create_time);
         WRITE_VALUE(_vcpu);
         WRITE_VALUE(_tcu);
         WRITE_VALUE(_memory);
         
         WRITE_OBJECT_ARRAY(_images);
         WRITE_OBJECT_ARRAY(_disks);        
         WRITE_OBJECT_ARRAY(_nics);
         WRITE_OBJECT(_context);
         WRITE_OBJECT(_qualifications);
         
         WRITE_VALUE(_virt_type);
         WRITE_VALUE(_root_device);
         WRITE_VALUE(_kernel_command);
         WRITE_VALUE(_bootloader);
         WRITE_VALUE(_raw_data);  
         WRITE_VALUE(_vnc_passwd);
         WRITE_VALUE(_appointed_cluster);
         WRITE_VALUE(_appointed_host);
         WRITE_VALUE(_watchdog);
         WRITE_VALUE(_enable_coredump);
         WRITE_VALUE(_enable_serial);
         WRITE_VALUE(_uid);
         WRITE_VALUE(_is_public);
         WRITE_OBJECT_ARRAY(_devices);
         WRITE_VALUE(_hypervisor);
         WRITE_VALUE(_enable_livemigrate);
         
         SERIALIZE_END();
      };
     
      DESERIALIZE
      {
         DESERIALIZE_BEGIN(VmTemplate);
         READ_VALUE(_oid);
         READ_VALUE(_name);
         READ_VALUE(_description);
         READ_OBJECT(_create_time);
         READ_VALUE(_vcpu);
         READ_VALUE(_tcu);
         READ_VALUE(_memory);
         READ_OBJECT_ARRAY(_images);
         READ_OBJECT_ARRAY(_disks);        
         READ_OBJECT_ARRAY(_nics);
         
         READ_OBJECT(_context);
         READ_OBJECT(_qualifications);
         READ_VALUE(_virt_type);
         READ_VALUE(_root_device);
         READ_VALUE(_kernel_command);
         READ_VALUE(_bootloader);
         READ_VALUE(_raw_data);
         READ_VALUE(_vnc_passwd);
         READ_VALUE(_appointed_cluster);
         READ_VALUE(_appointed_host);
         READ_VALUE(_watchdog);
         READ_VALUE(_enable_coredump);
         READ_VALUE(_enable_serial);
         READ_VALUE(_uid);
         READ_VALUE(_is_public); 
         READ_OBJECT_ARRAY(_devices);
         READ_VALUE(_hypervisor);
         READ_VALUE(_enable_livemigrate);
         DESERIALIZE_END();     
    };

    
    int64                  _oid;
    string                 _name;
    string                 _description;
    Datetime               _create_time;
    int                    _vcpu;  
    int                    _tcu;
    long long              _memory;

    vector<VmBaseImage>    _images;  
    vector<VmBaseDisk>     _disks; 
    vector<VmBaseNic>      _nics;
    VmBaseContext          _context;     
    Qualification          _qualifications; 
    
    string                 _virt_type;
    string                 _root_device;
    string                 _kernel_command;
    string                 _bootloader;
    string                 _raw_data;

    string                 _vnc_passwd;    
    string                 _appointed_cluster;
    string                 _appointed_host;   
    int                    _watchdog;
    bool                 _enable_coredump;
    bool                 _enable_serial;
    int64                  _uid;
    bool                   _is_public; 
    string                 _hypervisor;
    bool                 _enable_livemigrate;

    vector<VmDeviceConfig> _devices;   //虚拟机特殊设备列表

    //以下字段不存入数据库
    int64                  _local_images_size;
    int64                  _share_images_size;
    int64                  _local_disks_size;
    int64                  _share_disks_size;

   DISALLOW_COPY_AND_ASSIGN(VmTemplate); 

};

}
#endif /* IMAGE_MANGER_VMTEMPLATE_H */

