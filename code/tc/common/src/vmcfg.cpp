/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：VmCfg.cpp
* 文件标识：见配置管理计划书
* 内容摘要：VmCfg类的实现文件
* 当前版本：1.0
* 作    者：Bob
* 完成日期：2011年7月28日
*
* 修改记录1：
*    修改日期：2011/7/28
*    版 本 号：V1.0
*    修 改 人：Bob
*    修改内容：创建
*******************************************************************************/

#include "vmcfg.h"
#include "sky.h"

namespace zte_tecs{


//检查虚拟机配置是否修改
bool VmCfg::VmCfgModified(const ApiVmCfgModifyData &modify_msg)
{
    bool match = false;

    if (_vcpu != modify_msg.cfg_info.base_info.vcpu ||
        _tcu != modify_msg.cfg_info.base_info.tcu ||
        _memory != modify_msg.cfg_info.base_info.memory)
    {
        return true;
    }

    //校验 machine
    vector<VmBaseImage>::const_iterator image;
    for (image = _images.begin(); image != _images.end(); image++)
    {
        if ((image->_image_id == modify_msg.cfg_info.base_info.machine.id)
            &&(image->_target == modify_msg.cfg_info.base_info.machine.target))
        {
            match = true;
            //_cfg_type无需再比较， VmCfgOperate::CheckImageFiles中已校验
            if (image->_position != modify_msg.cfg_info.base_info.machine.position)
            {
                return true;
            }
        }
    }
    if (!match)
    {
        return true;
    }

    //校验其他disk 和 image
    match = false;
    vector<ApiVmDisk>::const_iterator vec_it_disk;
    vector<VmBaseDisk>::const_iterator disk;
    for (vec_it_disk  = modify_msg.cfg_info.base_info.disks.begin();
             vec_it_disk != modify_msg.cfg_info.base_info.disks.end();
             vec_it_disk++)
    {
        if (DISK_TYPE_DISK == vec_it_disk->type)
        {
            for (disk = _disks.begin(); disk != _disks.end(); disk++)
            {
                if (vec_it_disk->target == disk->_target)
                {
                    match = true;
                    if (disk->_position != vec_it_disk->position ||
                        disk->_bus != vec_it_disk->bus ||
                        disk->_size != vec_it_disk->size ||
                        disk->_fstype != vec_it_disk->fstype)
                    {
                        return true;
                    }
                }
            }
        }
        else
        {
            for (image = _images.begin(); image != _images.end(); image++)
            {
                if (vec_it_disk->target == image->_target)
                {
                    match = true;
                    if (image->_position != vec_it_disk->position ||
                        image->_image_id != vec_it_disk->id )//||
                        //image->_cfg_type != vec_it_disk->type)
                    {
                        return true;
                    }
                }
            }
        }
        if (!match)
        {
            return true;
        }
    }

    return false;
}

/******************************************************************************/
void VmCfg::Modify(const ApiVmCfgModifyData &modify_msg, int64 uid)
{
    //虚拟机配置确实修改了，版本号才加1
    if(true == VmCfgModified(modify_msg))
    {
        _config_version++;
    }

    _uid     = uid;
    _name    = modify_msg.cfg_info.vm_name;
    _description = modify_msg.cfg_info.base_info.description;
    _vcpu    = modify_msg.cfg_info.base_info.vcpu;
    _tcu     = modify_msg.cfg_info.base_info.tcu;
    _memory  = modify_msg.cfg_info.base_info.memory;

    // image, disk
    _images.clear();
    VmBaseImage   vm_base_image(INVALID_OID,
                                modify_msg.cfg_info.base_info.machine.target,
                                modify_msg.cfg_info.base_info.machine.position,
                                modify_msg.cfg_info.base_info.machine.id,
                                modify_msg.cfg_info.base_info.machine.reserved_backup,
                                modify_msg.cfg_info.base_info.machine.type);
    _images.push_back(vm_base_image);

    _disks.clear();
    vector<ApiVmDisk>::const_iterator vec_it_disk;
    for (vec_it_disk  = modify_msg.cfg_info.base_info.disks.begin();
             vec_it_disk != modify_msg.cfg_info.base_info.disks.end();
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


    // context
    _context._str_map.clear();
    map<string, string>::const_iterator  it_context;
    for (it_context  = modify_msg.cfg_info.base_info.context.begin();
             it_context != modify_msg.cfg_info.base_info.context.end();
             it_context++)
    {
            _context._id = INVALID_OID;
            _context._str_map.insert(make_pair(it_context->first, it_context->second));
    }

    // 调度条件
    _qualifications._qua.clear();
    vector<ApiVmExpression>::const_iterator vec_it_exp;
    for (vec_it_exp = modify_msg.cfg_info.base_info.qualifications.begin();
             vec_it_exp !=  modify_msg.cfg_info.base_info.qualifications.end();
             vec_it_exp++)
    {
            Expression  exp(vec_it_exp->key,
                            vec_it_exp->op,
                            vec_it_exp->value);

            _qualifications._qua.push_back(exp);
    }

    _nics.clear();
    vector<ApiVmNic>::const_iterator  it_nic_new;
    for(it_nic_new = modify_msg.cfg_info.base_info.nics.begin();
        it_nic_new != modify_msg.cfg_info.base_info.nics.end();
        it_nic_new++)
    {
        // 1. nic
        VmBaseNic   vm_base_nic(INVALID_OID,
                            it_nic_new->nic_index,
                            it_nic_new->sriov,
                            it_nic_new->loop,
                            it_nic_new->logic_network_id,
                            it_nic_new->model,
                            it_nic_new->vsi_profile_id,
                            it_nic_new->ip,
                            it_nic_new->netmask,
                            it_nic_new->gateway,
                            it_nic_new->mac);
 
        _nics.push_back(vm_base_nic);      
        }

    _virt_type           = modify_msg.cfg_info.base_info.virt_type;
    _root_device         = modify_msg.cfg_info.base_info.root_device;
    _kernel_command      = modify_msg.cfg_info.base_info.kernel_command;
    _bootloader          = modify_msg.cfg_info.base_info.bootloader;
    _raw_data            = modify_msg.cfg_info.base_info.rawdata;
    _appointed_cluster   = modify_msg.cfg_info.base_info.cluster;
    _appointed_host      = modify_msg.cfg_info.base_info.host;

    _watchdog            = modify_msg.cfg_info.base_info.vm_wdtime;
    _vnc_passwd          = modify_msg.cfg_info.base_info.vnc_passwd;
    _enable_serial       = modify_msg.cfg_info.base_info.enable_serial;
    _hypervisor          = modify_msg.cfg_info.base_info.hypervisor;

    _enable_coredump  = modify_msg.cfg_info.base_info.enable_coredump;

    _enable_livemigrate  = modify_msg.cfg_info.base_info.enable_livemigrate;

    _devices.assign(modify_msg.cfg_info.base_info.devices.begin(), modify_msg.cfg_info.base_info.devices.end());

}


/******************************************************************************/

}



