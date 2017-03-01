/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�VmTemplate.cpp
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��VmTemplate���ʵ���ļ�
* ��ǰ�汾��1.0 
* ��    �ߣ�Bob
* ������ڣ�2011��7��28��
*
* �޸ļ�¼1��
*    �޸����ڣ�2011/7/28
*    �� �� �ţ�V1.0
*    �� �� �ˣ�Bob
*    �޸����ݣ�����
*******************************************************************************/

#include "vmtemplate.h"
#include "sky.h"

namespace zte_tecs{

/******************************************************************************/
void VmTemplate::Modify(const ApiVtModifyData &modify_msg )
{
    _name      = modify_msg.vt_name;
    _is_public = modify_msg.is_public;
    
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
        
    // ��������
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
    
    vector<VmBaseNic>      tmp_nic;   
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
                            "");
 
        tmp_nic.push_back(vm_base_nic);
    }  
    
    _nics.clear();   
    _nics = tmp_nic;     
       
   _virt_type           = modify_msg.cfg_info.base_info.virt_type;
   _root_device         = modify_msg.cfg_info.base_info.root_device;
   _kernel_command      = modify_msg.cfg_info.base_info.kernel_command;
   _bootloader          = modify_msg.cfg_info.base_info.bootloader;
   _raw_data            = modify_msg.cfg_info.base_info.rawdata;
   _appointed_cluster   = modify_msg.cfg_info.base_info.cluster;
   _appointed_host      = modify_msg.cfg_info.base_info.host;
       
   _watchdog            = modify_msg.cfg_info.base_info.vm_wdtime;
   _vnc_passwd          = modify_msg.cfg_info.base_info.vnc_passwd;
   _enable_coredump = modify_msg.cfg_info.base_info.enable_coredump;
   _enable_serial       = modify_msg.cfg_info.base_info.enable_serial;
   _hypervisor          = modify_msg.cfg_info.base_info.hypervisor;
   _enable_livemigrate  = modify_msg.cfg_info.base_info.enable_livemigrate;

    _devices.assign(modify_msg.cfg_info.base_info.devices.begin(), modify_msg.cfg_info.base_info.devices.end());
}


/******************************************************************************/

}
	


