/*******************************************************************************
* Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：image.h
* 文件标识：见配置管理计划书
* 内容摘要：image类的定义文件
* 当前版本：2.0
* 作    者：lichun
* 完成日期：2012年10月19日
*
* 修改记录1：
*    修改日期：2012/10/19
*    版 本 号：V2.0
*    修 改 人：lichun
*    修改内容：将image单独作为资源类，数据库相关操作放入image_pool类中
*******************************************************************************/
#ifndef IMAGE_MANGER_IMAGE_H        
#define IMAGE_MANGER_IMAGE_H
#include "sky.h"
#include "image_api.h"
#include "api_const.h"
#include "tecs_pub.h"
#include "image_store.h"

namespace zte_tecs
{
class Image:public Serializable 
{
    friend class ImagePool;

public:
    Image(int64 uid ,int64 image_id = INVALID_OID, const string &reg_time= "" )
    {
        _owner = uid;
        _image_id = image_id;    
        _is_public = false;  
        _is_enabled = false;
        _running_vms = 0;
        _ref_count_by_private_vt = 0;
        _ref_count_by_public_vt = 0;
        _size = 0;    
    }
	
    Image()
    {
        _owner = INVALID_UID;
        _image_id = INVALID_OID;    
        _is_public = false;  
        _is_enabled = false;
        _running_vms = 0;
        _ref_count_by_private_vt = 0;
        _ref_count_by_public_vt = 0;
        _size = 0;    	
    }
	
    ~Image(){ };   

    void Print()
    {
        cout<<"\n image_id:"<<_image_id<< endl;
        cout<<"owner:"<<_owner<< endl;
        cout<<"name:"<<_name<<endl;
        cout<<"type:"<<_type<<endl;
        cout<<"bus:"<<_bus<<endl;
        cout<<"os_arch:"<<_os_arch<<endl;
        cout<<"os_type:"<<_os_type<<endl;
        cout<<"register_time:"<<_register_time<<endl;
        cout<<"size:"<<_size<<endl;
        cout<<"location:"<<_location<<endl;
        cout<<"running_vms:"<<_running_vms<<endl;
        cout<<"description:"<<_description<<endl;
    }

    int64 get_iid() const
    {
        return _image_id;
    }; 
  
    int64 get_uid() const
    {
        return _owner;
    };

    const string& get_name() const
    {
        return _name;
    };

    const string& get_type() const
    {
        return _type;
    };
    
    const string& get_location() const
    {
        return _location;
    };
    
    void get_file_url(FileUrl& file_url) const
    {
        file_url = _file_url;
    };
    
    const string& get_arch() const
    {
        return _os_arch;
    };

    const string& get_bus() const
    {
        return _bus;
    };

    int64 get_size() const
    {
        return _size;
    };

    bool get_public() const
    {
        return _is_public;
    };	

    int get_running_vms() const    
    {
        return _running_vms;
    };

    int get_ref_count_by_private_vt() const    
    {
        return _ref_count_by_private_vt;
    };

    int get_ref_count_by_public_vt() const    
    {
        return _ref_count_by_public_vt;
    };

    int get_ref_count() const    
    {
        return _ref_count_by_public_vt + _ref_count_by_private_vt;
    };
    
    void set_name(const string &name)
    {
         _name = name;
    };

    void set_uid(int64 uid)
    {
         _owner = uid;
    };

    void set_register_time(const string &register_time)
    {
        _register_time = register_time;
    };
	
    int set_type(const string &type)
    {
        if (!ImageParaCheck::Type(type))
        {
            return -1;
        }
        _type = type;
        return 0;
    };

    int set_location(const string& location)
    {
        _location = location;
        return 0;
    };
     
    int set_file_url(const FileUrl &file_url)
    {
        _file_url = file_url;
        return 0;
    };
     
    int set_file_checksum(const string &checksum)
    {
        _file_url.checksum = checksum;
        return 0;
    };
     
    int set_arch(const string &arch)
    {
        if( !ImageParaCheck::Arch(arch))
        {
            return -1;
        }
        _os_arch = arch;
        return 0;
    };

    int set_size(int64 size)
    {
        _size = size;
        return 0;
    };
     
    int set_bus(const string &bus)
    {
        _bus = bus;
        return 0;
    };
     
    int set_description(const string &description)
    {
        _description = description;
        return 0;
    };
        
    const string& get_description() const
    {
        return _description;
    };

    int set_public(bool flag)    
    {
        _is_public = flag;
        return 0;
    };

    int set_enable(bool flag)
    {
        _is_enabled = flag;
        return 0;
    };

    bool get_enable() const
    {
        return _is_enabled;
    };

    int set_os_type(const string &os_type)
    {
        _os_type = os_type;
        return 0;
    };

    const string& get_os_type() const
    {
        return _os_type;
    };

    int set_disk_size(int64 disk_size)
    {
        _disk_size = disk_size;
        return 0;
    };

    int64 get_disk_size() const
    {
        return _disk_size;
    };

    int set_disk_format(const string &disk_format)
    {
        _disk_format = disk_format;
        return 0;
    };

    const string& get_disk_format() const
    {
        return _disk_format;
    };

    int set_container_format(const string &container_format)
    {
        _container_format = container_format;
        return 0;
    };

    const string& get_container_format() const
    {
        return _container_format;
    };

    SERIALIZE
    {
        SERIALIZE_BEGIN(Image);		
        WRITE_VALUE(_image_id);
        WRITE_VALUE(_owner);
        WRITE_VALUE(_name);
        WRITE_VALUE(_type);
        WRITE_VALUE(_bus);        
	WRITE_VALUE(_os_arch);        
        WRITE_VALUE(_os_type);        
        WRITE_VALUE(_os_version);
        WRITE_VALUE(_is_public);
        WRITE_VALUE(_is_enabled);
        WRITE_VALUE(_register_time);
        WRITE_VALUE(_size);        
	WRITE_VALUE(_location);        
        WRITE_OBJECT(_file_url);      
        WRITE_VALUE(_disk_format);
        WRITE_VALUE(_container_format);        
	WRITE_VALUE(_running_vms);        
        WRITE_VALUE(_description);      
        WRITE_VALUE(_ref_count_by_private_vt);     
        WRITE_VALUE(_ref_count_by_public_vt);    
        WRITE_VALUE(_disk_size);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(Image);	
        READ_VALUE(_image_id);
        READ_VALUE(_owner);
        READ_VALUE(_name);
        READ_VALUE(_type);
        READ_VALUE(_bus);        
	READ_VALUE(_os_arch);        
        READ_VALUE(_os_type);        
        READ_VALUE(_os_version);
        READ_VALUE(_is_public);
        READ_VALUE(_is_enabled);
        READ_VALUE(_register_time);
        READ_VALUE(_size);        
	READ_VALUE(_location);        
        READ_OBJECT(_file_url);      
        READ_VALUE(_disk_format);
        READ_VALUE(_container_format);        
	READ_VALUE(_running_vms);        
        READ_VALUE(_description);      
        READ_VALUE(_ref_count_by_private_vt);     
        READ_VALUE(_ref_count_by_public_vt);     
        READ_VALUE(_disk_size);
        DESERIALIZE_END();
    };

private:
    int64             _image_id;  //映像文件ID
    int64             _owner;
    string            _name;	
    string            _type;
    string            _bus;
    string            _os_arch;
    string            _os_type;
    string            _os_version;
    bool              _is_public;
    bool              _is_enabled;
    string            _register_time;
    int64             _size;
    string            _location; //文件存放位置，可以从存放位置反查image信息
    FileUrl           _file_url; //文件URL，供CC、HC取用时作为位置信息
    string            _disk_format; 
    //The disk format of a virtual machine image is the format of the underlying disk image. 
    //Virtual appliance vendors have different formats for laying out the information contained 
    //in a virtual machine disk image. (value = raw,vhd,qcow2,vmdk ...)
    string            _container_format;
    //The container format refers to whether the virtual machine image is in a file format 
    //that also contains metadata about the actual virtual machine.
    //There are two main types of container formats: OVF and Amazon’s AMI. 
    //In addition, a virtual machine image may have no container format at all – 
    //basically, it’s just a blob of unstructured data...
    int               _running_vms;
    string            _description;
    int               _ref_count_by_private_vt;
    int               _ref_count_by_public_vt;
    int64             _disk_size;  //镜像虚拟大小

    //DISALLOW_COPY_AND_ASSIGN(Image); 
};

}
#endif /* IMAGE_MANGER_IMAGE_H */

