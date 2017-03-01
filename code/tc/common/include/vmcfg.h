/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：vmcfg.h
* 文件标识：见配置管理计划书
* 内容摘要：vmcfg类的定义文件
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

#ifndef VMCFG_MANGER_VMCFG_H
#define VMCFG_MANGER_VMCFG_H

#include "sky.h"
#include "vmcfgbase.h"
#include "vmcfg_api.h"
#include "vnet_libnet.h"
#include "affinity_region.h"

namespace zte_tecs
{

class VmRepel : public Serializable
{
public:
    VmRepel()
    {
        _vid1 = INVALID_OID;
        _vid2 = INVALID_OID;
    };

    ~VmRepel(){};

    SERIALIZE
    {
        SERIALIZE_BEGIN(VmRepel);
        WRITE_VALUE(_vid1);
        WRITE_VALUE(_vid2);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(VmRepel);
        READ_VALUE(_vid1);
        READ_VALUE(_vid2);
        DESERIALIZE_END();
    };

    int64 _vid1;
    int64 _vid2;
};

class VmAffReg : public Serializable
{
public:
    VmAffReg()
    { 
        _arid = INVALID_OID;     
    };

    ~VmAffReg(){};

    SERIALIZE
    {
        SERIALIZE_BEGIN(VmAffReg);
        WRITE_VALUE(_arid);
        WRITE_OBJECT(aff_region);
        WRITE_DIGIT_VECTOR(_vid);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(VmAffReg);
        READ_VALUE(_arid);
        READ_OBJECT(aff_region);
        READ_DIGIT_VECTOR(_vid);
        DESERIALIZE_END();
    };

    int64 _arid;
    AffinityRegion aff_region;
    vector<int64> _vid;
};
/**
@brief 功能描述: 虚拟机配置管理的实体类\n
@li @b 其它说明: 无
*/
class VmCfg : public Serializable
{


/*******************************************************************************
* 1. public section
*******************************************************************************/
public:

  /***************************************************************************
     *  Constructor
  ***************************************************************************/
    VmCfg()
    {
        _oid           = INVALID_OID;
        _vcpu         = 0;
        _tcu          = 0;
        _memory       = 0;
        _watchdog     = 0;
        _uid          = INVALID_OID;
        _project_id   = INVALID_OID;

        _config_version   = 0;
        _local_disk_size  = 0;
        _share_disk_size  = 0;
        _enable_coredump = false;
        _enable_serial  = false;
        _enable_livemigrate = true;

    };

    VmCfg(const ApiVmCfgAllocateData &allocate_msg, int64 uid)
    {
        _oid              = INVALID_OID;
        _project_id       = INVALID_OID;
        _config_version   = 0;
        _local_disk_size  = 0;
        _share_disk_size  = 0;
        _uid              = uid;

        _name           = allocate_msg.cfg_info.vm_name;
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
        _enable_livemigrate= allocate_msg.cfg_info.base_info.enable_livemigrate;

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
                            "",
                            vec_it_nic->ip,
                            vec_it_nic->netmask,
                            vec_it_nic->gateway,
                            vec_it_nic->mac);
 
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
    };


    ~VmCfg(){};
    /**
     * Get the VmCfg unique identifier IID, that matches the OID of the object
     *    @return IID VmCfg identifier
     */
    int get_vid() const
    {
        return _oid;  //
    };

    /**
     * Gets the uid of the owner of the VmCfg
     * @return uid
     **/
    int64  get_uid() const
    {
       return _uid;
    };

    string get_project_name() const
    {
        return _project_name;
    };

    int64 get_project_id() const
    {
       return _project_id;
    };

    string get_virt_type() const
    {
        return _virt_type;
    };

    string get_hypervisor() const
    {
        return _hypervisor;
    };

    int  get_vcpu()
    {
        return _vcpu;
    };

    int get_tcu()
    {
        return _tcu;
    }

    int64  get_memory()
    {
        return _memory;
    };

    string get_cluster_name( ) const
    {
       return _deployed_cluster;
    };

    string get_appointed_cluster() const
    {
        return _appointed_cluster;
    };

    string get_appointed_host() const
    {
        return _appointed_host;
    };

    void get_images(vector<VmBaseImage>  &images)
    {
        images = _images;
    };

    int32 share_image_num() const
    {
        int32 num=0;
        vector<VmBaseImage>::const_iterator  it_img;
        for(it_img  = _images.begin();
            it_img != _images.end();
            it_img++)
        {
            if(VM_STORAGE_TO_SHARE ==  it_img->_position )
            {
               num++;
            }
        }
        return num;
    }

    void get_disks(vector<VmBaseDisk>  &disks)
    {
        disks = _disks;
    };

    bool is_enable_wdg() const
    {
        if(_watchdog > 0)
        {
            return true;
        }

        return false;
    };

    bool is_need_network()
    {
        if ((0 != _nics.size())
            ||(0 < _watchdog))
        {
           return true;
        }

        return false;
    }

    int  get_wdg_period()const
    {
        return _watchdog;
    };

    bool is_enable_coredump()
    {
        return _enable_coredump;
    };

    bool enable_serial()
    {
        return _enable_serial;
    };

    bool enable_livemigrate()
    {
        return _enable_livemigrate;
    };
    void get_nics(vector<VmBaseNic>  &nics)
    {
        nics = _nics;
    };

    void get_vnet_nics(vector<CVNetVnicConfig>  &vnet_nics)
    {
        vector<VmBaseNic>::const_iterator  iter;
        for(iter = _nics.begin(); iter != _nics.end(); iter++)
        {
            CVNetVnicConfig  vnet_nic(iter->_nic_index,
                                      iter->_sriov,
                                      iter->_loop,
                                      iter->_logic_network_id,
                                      iter->_model,
                                      iter->_ip,
                                      iter->_netmask,
                                      iter->_gateway,
                                      iter->_mac);
            vnet_nics.push_back(vnet_nic);           
        }       
    };
    int64 get_local_disk_size( ) const
    {
        return _local_disk_size;
    };

    int64 get_share_disk_size( ) const
    {
        return _share_disk_size;
    };

    int64 get_total_size( ) const
    {
       return _local_disk_size + _share_disk_size;
    };

    int get_config_version() const
    {
        return _config_version;
    }

    void set_cluster_name(const string &cluster)
    {
         _deployed_cluster  = cluster;
    };

    void set_name(const string &name)
    {
         _name = name;
    }

    void set_project_id(int64 project_id)
    {
         _project_id = project_id;
    }

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
       vector<VmBaseNic>      tmp_nic;
       vector<VmBaseNic>::const_iterator  it_nic_new;

       for(it_nic_new  = nics.begin();
           it_nic_new != nics.end();
           it_nic_new++)
       {
           // 1. nic
           tmp_nic.push_back(*it_nic_new);
        }

       _nics.clear();
       _nics    = tmp_nic;

    };

    void clear_vsi()
    {
        vector<VmBaseNic>::iterator      ite_nic;
        for(ite_nic  = _nics.begin();
           ite_nic != _nics.end();
           ite_nic++)
       {
           ite_nic->_vsi_profile_id = "";
       }
    }
    void update_nics(const vector<CVNetVnicConfig>  &vnet_nics)
    {
       vector<VmBaseNic>::iterator      ite_nic;
       vector<CVNetVnicConfig>::const_iterator  it_vnet_nic;

       for(ite_nic  = _nics.begin();
           ite_nic != _nics.end();
           ite_nic++)
       {
           for(it_vnet_nic  = vnet_nics.begin();
               it_vnet_nic != vnet_nics.begin();
               it_vnet_nic++)
           {
              if (ite_nic->_nic_index == it_vnet_nic->m_nNicIndex)
              {
                  ite_nic->_vsi_profile_id = it_vnet_nic->m_strVSIProfileID;
              }
           }
        }

    };

    void set_config_version(int config_version)
    {
        _config_version = config_version;
    }

    void clear_appointed_cluster()
    {
         _appointed_cluster.clear();
    }

    void clear_appointed_host()
    {
         _appointed_host.clear();
    }

    void clear_deployed_cluster()
    {
         _deployed_cluster.clear();
    }

    //检查虚拟机配置是否修改
    bool VmCfgModified(const ApiVmCfgModifyData &modify_msg);

    void Modify(const ApiVmCfgModifyData &modify_msg, int64 uid);

    bool isDeployed(void)
    {
        if (_deployed_cluster.empty())
        {
            return false;
        }
        else
        {
            return true;
        }
    }

    SERIALIZE
    {
        SERIALIZE_BEGIN(VmCfg);
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
        WRITE_VALUE(_hypervisor);
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
        WRITE_VALUE(_project_id);
        WRITE_VALUE(_deployed_cluster);
        WRITE_VALUE(_config_version);

        WRITE_VALUE(_project_name);
        WRITE_VALUE(_uid);
        WRITE_VALUE(_user_name);
        WRITE_VALUE(_local_disk_size);
        WRITE_VALUE(_share_disk_size);
        WRITE_OBJECT_ARRAY(_devices);
        WRITE_VALUE(_enable_livemigrate);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(VmCfg);
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
        READ_VALUE(_hypervisor);
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
        READ_VALUE(_project_id);
        READ_VALUE(_deployed_cluster);
        READ_VALUE(_config_version);

        READ_VALUE(_project_name);
        READ_VALUE(_uid);
        READ_VALUE(_user_name);
        READ_VALUE(_local_disk_size);
        READ_VALUE(_share_disk_size);
        READ_OBJECT_ARRAY(_devices);
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

    int64                  _project_id;
    string                 _deployed_cluster;
    int32                  _config_version;
    bool                   _enable_serial;
    string                 _hypervisor;

    vector<VmDeviceConfig> _devices;   //虚拟机特殊设备列表

    bool                   _enable_livemigrate;   //是否允许热迁移

    //以下字段不存入数据库
    string                 _project_name;
    int64                  _uid;
    string                 _user_name;
    int64                  _local_disk_size;
    int64                  _share_disk_size;

/*******************************************************************************
* 2. protected section
*******************************************************************************/



/*******************************************************************************
* 3. private section
*******************************************************************************/



   //DISALLOW_COPY_AND_ASSIGN(VmCfg);

};

class VmAllocateReq: public Serializable
{
public:
    int       vm_num;
    VmCfg     base_info;

    SERIALIZE
    {
        SERIALIZE_BEGIN(VmAllocateReq);
        WRITE_VALUE(vm_num);
        WRITE_OBJECT(base_info);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(VmAllocateReq);
        READ_VALUE(vm_num);
        READ_OBJECT(base_info);
        DESERIALIZE_END();
    };
};

class VmAllocateAck: public Serializable
{
public:
    SERIALIZE
    {
        SERIALIZE_BEGIN(VmAllocateAck);
        WRITE_VALUE(state);
        WRITE_VALUE(vids);
        WRITE_VALUE(detail);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(VmAllocateAck);
        READ_VALUE(state);
        READ_VALUE(vids);
        READ_VALUE(detail);
        DESERIALIZE_END();
    };
    int state;
    vector<int64>  vids;
    string detail;
};
}
#endif /* IMAGE_MANGER_IMAGE_H */

