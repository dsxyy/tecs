/******************************************************************************
* Copyright  = c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：vmcfgbase.h
* 文件标识：见配置管理计划书
* 内容摘要：vmcfg 与 vmtemplate公共的部分
* 当前版本：1.0
* 作    者：陈文文
* 完成日期：2012年6月25日
*
* 修改记录1：
*     修改日期：2012/6/25
*     版 本 号：V1.0
*     修 改 人：陈文文
*     修改内容：创建
******************************************************************************/
#ifndef TC_VMCFGBASE_H
#define TC_VMCFGBASE_H
#include "sky.h"
#include "tecs_pub.h"
#include "image_url.h"
#include "pool_sql.h"
#include "vm_messages.h"

namespace zte_tecs 
{

//定义该类是为了方便进行序列化
class Qualification: public Serializable
{
    /*******************************************************************************
    * 1. public section
    *******************************************************************************/
public:
    Qualification()
    {
        _qua.clear();
    };
    
    ~Qualification() {};
    
    SERIALIZE
    {
        SERIALIZE_BEGIN(Qualification);        
        WRITE_OBJECT_ARRAY(_qua);
        SERIALIZE_END();
    };
    
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(Qualification);
        READ_OBJECT_ARRAY(_qua);
        DESERIALIZE_END();
    };

    vector<Expression>     _qua; 

};


class VmBaseImage : public Serializable
{
public:
    VmBaseImage()
    {
      _id       = INVALID_OID;
      _image_id = INVALID_OID;
      _position = 0;
      _reserved_backup = 0;
    };
    
    VmBaseImage(int64  id, 
                       const string& target, 
                       int32 position, 
                       int64 image_id,
                       const string& cfg_type)   
    {
        _id     = id;
        _target = target;
        _position = position;
        _image_id = image_id;
        _cfg_type = cfg_type;
    };

    VmBaseImage(int64  id, 
                       const string& target, 
                       int32 position, 
                       int64 image_id,
                       int32 reserved_backup,
                       const string& cfg_type)   
    {
        _id     = id;
        _target = target;
        _position = position;
        _image_id = image_id;
        _reserved_backup = reserved_backup;
        _cfg_type = cfg_type;
    };

    SERIALIZE
    {
        SERIALIZE_BEGIN(VmBaseImage);
        WRITE_VALUE(_id);
        WRITE_VALUE(_target);
        WRITE_VALUE(_position);
        WRITE_VALUE(_image_id);
        WRITE_VALUE(_reserved_backup);
        WRITE_VALUE(_cfg_type);        
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(VmBaseImage);
        READ_VALUE(_id);
        READ_VALUE(_target);
        READ_VALUE(_position);
        READ_VALUE(_image_id);
        READ_VALUE(_reserved_backup);
        READ_VALUE(_cfg_type);        
        DESERIALIZE_END();
    };


    int64    _id; //虚拟机ID或模板ID
    string   _target;
    int32    _position ;
    int64    _image_id;
    int32    _reserved_backup;
    string   _cfg_type;
};

// 供web页面查询用
class VmBaseImageInfo
{
public:
    VmBaseImageInfo()
    {
       _id       = INVALID_OID;
       _image_id = INVALID_OID;
       _position = 0;
       _size     = 0;
    };
    int64   _id;  //虚拟机 OR 模板 ID
    int64   _image_id ;
    int     _position;
    int64   _size;
    string  _type;
    string  _bus;
    string  _target;
    FileUrl _url;
    FileUrl _share_url;
    string  _os_type;
    int64   _disk_size;
    string  _disk_format;
    int32   _reserved_backup;
    string  _os_arch;
};

class VmBaseImagePool: public Callbackable
{
public:
    static VmBaseImagePool* GetInstance(SqlDB *db = NULL)
    {
        if(NULL == _instance)
        {
            SkyAssert(NULL != db);
            _instance = new VmBaseImagePool(db); 
        }
        
        return _instance;
    };
    
    int SelectCallback(void *nil, SqlColumn * columns);
    int GetVmBaseImages(vector<VmBaseImage>& vmbase_images, int64 id, bool is_vm);    
    int SetVmBaseImages(vector<VmBaseImage>& vmbase_images, int64 id, bool is_vm);
    int DropVmBaseImages(int64 id, bool is_vm);
    int DropVmBaseImages(const VmBaseImage &image);
    int InsertVmBaseImages(const vector<VmBaseImage>& vmbase_images, int64 id,bool is_vm);
    
    int GetImageInfoByView(vector<VmBaseImageInfo>& image_info, int64 id, bool is_vm);
    int GetImageInfoByViewCallback(void *nil, SqlColumn * columns);
    int GetImagesSize(int64 id, bool is_vm, int position ,int64 &total_size);
   // int GetTotalImageSizeCallback(void *nil, SqlColumn * columns);
   
private:
    enum ColNames 
    {
        ID       = 0,
        TARGET   = 1,
        POSITION = 2,
        IMAGE_ID = 3,
        RESERVED_BACKUP = 4,
        LIMIT    = 5
    };

    enum ViewVmBaseImageColNames
    {
        VIEW_ID             = 0,   
        VIEW_TARGET         = 1,   
        VIEW_IMAGE_ID       = 2, 
        VIEW_POSITION       = 3,
        VIEW_RESERVED_BACKUP = 4,
        VIEW_TYPE           = 5,
        VIEW_SIZE           = 6, 
        VIEW_BUS            = 7,
        VIEW_FILEURL        = 8, 
        VIEW_LOACATION      = 9,
        VIEW_IMAGE_OWNER    = 0,
        VIEW_OS_TYPE        = 11,
        VIEW_DISK_SIZE      = 12,
        VIEW_DISK_FORMAT    = 13,
        VIEW_OS_ARCH        = 14,
        VIEW_LIMIT          = 15
    };
    
    VmBaseImagePool(SqlDB *pDb)
    {
        db = pDb;
    };
    
    DISALLOW_COPY_AND_ASSIGN(VmBaseImagePool);
    static VmBaseImagePool *_instance;
    SqlDB             * db;
    static const char * _table_vmcfg_image;
    static const char * _table_vmtemplate_image;
    
    static const char * _view_vmcfg_image;
    static const char * _view_vmtemplate_image;
    
    static const char * _col_names_vmcfg_image;    
    static const char * _col_names_vmtemplate_image; 
    
    static const char * _col_names_view_vmcfg_image;
    static const char * _col_names_view_vmtemplate_image;
 //   static const char * _col_names_view_vmcfg_image_size;
 //   static const char * _col_names_view_vmtemplate_image_size;
    
};


class VmBaseDisk : public Serializable
{
public:
    VmBaseDisk()
    {
      _id       = INVALID_OID;
      _position = 0;
      _size     = 0;      
    };
    
    VmBaseDisk(int64           id, 
                       const string& target, 
                       int32         position,
                       const string& bus,
                       int64         size,
                       const string& fstype)
    {
        _id       = id;
        _target   = target;
        _position = position;
        _bus      = bus;
        _size     = size;
        _fstype   = fstype;
    };

    SERIALIZE
    {
        SERIALIZE_BEGIN(VmBaseDisk);
        WRITE_VALUE(_id);
        WRITE_VALUE(_target);
        WRITE_VALUE(_position);
        WRITE_VALUE(_bus);
        WRITE_VALUE(_size);        
        WRITE_VALUE(_fstype);
        WRITE_VALUE(_volume_uuid);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(VmBaseDisk);
        READ_VALUE(_id);
        READ_VALUE(_target);
        READ_VALUE(_position);
        READ_VALUE(_bus);
        READ_VALUE(_size);        
        READ_VALUE(_fstype);
        READ_VALUE(_volume_uuid);
        DESERIALIZE_END();
    };

    int64    _id; //虚拟机ID或模板ID
    string   _target;
    int32    _position ;
    string   _bus;
    int64    _size;
    string   _fstype;
    string   _volume_uuid;
};

class VmBaseDiskPool: public Callbackable
{
public:
    static VmBaseDiskPool* GetInstance(SqlDB *db = NULL)
    {
        if(NULL == _instance)
        {
            SkyAssert(NULL != db);
            _instance = new VmBaseDiskPool(db); 
        }
        
        return _instance;
    };
    
    int SelectCallback(void *nil, SqlColumn * columns);
    int GetVmBaseDisks(vector<VmBaseDisk>& vmbase_disks, int64 id, bool is_vm);
    int SetVmBaseDisks(vector<VmBaseDisk>& vmbase_disks, int64 id, bool is_vm);
    int DropVmBaseDisks(int64 id, bool is_vm);
    int InsertVmBaseDisks(const vector<VmBaseDisk>& vmbase_disks, int64 id,bool is_vm);
    int GetDisksSize(int64 id, bool is_vm, int position, int64 &total_size);
   
private:
    enum ColNames 
    {
        ID       = 0,
        TARGET   = 1,
        POSITION = 2,
        BUS      = 3,
        SIZE     = 4,
        FSTYPE   = 5,
        VOLUUID  = 6,
        LIMIT    = 7
    };
    
    VmBaseDiskPool(SqlDB *pDb)
    {
        db = pDb;
    };
    
    DISALLOW_COPY_AND_ASSIGN(VmBaseDiskPool);
    static VmBaseDiskPool *_instance;
    SqlDB              *db;
    static const char *  _table_vmcfg_disk;
    static const char *  _table_vmtemplate_disk;
    
    static const char *  _col_names_vmcfg_disk;    
    static const char * _col_names_vmtemplate_disk; 
};



class VmBaseNic : public Serializable
{
public:
    VmBaseNic()
    {
      _id        = INVALID_OID;
      _nic_index = 0;
      _sriov     = 0;  
      _loop      = 0;
    }; 

    VmBaseNic(int64          id, 
              int32          nic_index, 
              int32          sriov,
              int32          loop,
              const string&  logic_network_id,
              const string&  model,
              const string&  vsi_profile_id,
              const string&  ip,
              const string&  netmask,
              const string&  gateway,
              const string&  mac)
    {
        _id               = id;
        _nic_index        = nic_index;
        _sriov            = sriov;        
        _loop             = loop;
        _logic_network_id = logic_network_id;
        _model            = model;
        _vsi_profile_id   = vsi_profile_id;
        _ip               = ip;
        _netmask          = netmask;
        _gateway          = gateway;
        _mac              = mac;
    };   
    VmBaseNic(int64          id, 
              int32          nic_index, 
              int32          sriov,
              int32          loop,
              const string&  logic_network_id,
              const string&  model,
              const string&  vsi_profile_id,
              const string&  ip,
              const string&  netmask,
              const string&  gateway)
    {
        _id               = id;
        _nic_index        = nic_index;
        _sriov            = sriov;        
        _loop             = loop;
        _logic_network_id = logic_network_id;
        _model            = model;
        _vsi_profile_id   = vsi_profile_id;
        _ip               = ip;
        _netmask          = netmask;
        _gateway          = gateway;
    };

    VmBaseNic(int64          id, 
              int32          nic_index, 
              int32          sriov,
              int32          loop,
              const string&  logic_network_id,
              const string&  model,
              const string&  vsi_profile_id)
    {
        _id               = id;
        _nic_index        = nic_index;
        _sriov            = sriov;        
        _loop             = loop;
        _logic_network_id = logic_network_id;
        _model            = model;
        _vsi_profile_id   = vsi_profile_id;
    };
    SERIALIZE
    {
        SERIALIZE_BEGIN(VmBaseNic);
        WRITE_VALUE(_id);
        WRITE_VALUE(_nic_index);
        WRITE_VALUE(_sriov);        
        WRITE_VALUE(_loop);
        WRITE_VALUE(_logic_network_id);
        WRITE_VALUE(_model);
        WRITE_VALUE(_vsi_profile_id);
        WRITE_VALUE(_ip);
        WRITE_VALUE(_netmask);
        WRITE_VALUE(_gateway);
        WRITE_VALUE(_mac);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(VmBaseNic);
        READ_VALUE(_id);
        READ_VALUE(_nic_index);
        READ_VALUE(_sriov);        
        READ_VALUE(_loop);
        READ_VALUE(_logic_network_id);
        READ_VALUE(_model);
        READ_VALUE(_vsi_profile_id);
        READ_VALUE(_ip);
        READ_VALUE(_netmask);
        READ_VALUE(_gateway);   
        READ_VALUE(_mac);                           
        DESERIALIZE_END();
    };

    int64     _id; //虚拟机ID或模板ID
    int32     _nic_index;
    int32     _sriov;
    int32     _loop; 
    string    _logic_network_id;
    string    _model;
    string    _vsi_profile_id;    //VSIProfile UUID,输出参数
    string    _ip; //网卡指定IP
    string    _netmask; // 网卡指定掩码
    string    _gateway; 
    string    _mac; 
    
};

class VmBaseNicPool: public Callbackable
{
public:
    static VmBaseNicPool* GetInstance(SqlDB *db = NULL)
    {
        if(NULL == _instance)
        {
            SkyAssert(NULL != db);
            _instance = new VmBaseNicPool(db); 
        }
        
        return _instance;
    };
    
    int VmSelectCallback(void *nil, SqlColumn * columns);
    int VtSelectCallback(void *nil, SqlColumn * columns);
    int GetVmBaseNics(vector<VmBaseNic>& vmbase_nics, int64 id, bool is_vm);
    int SetVmBaseNics(vector<VmBaseNic>& vmbase_nics, int64 id, bool is_vm);
    int DropVmBaseNics(int64 id, bool is_vm);
    int InsertVmBaseNics(const vector<VmBaseNic>& vmbase_nics, int64 id,bool is_vm); 
    int GetVmCfgNetwork(vector<VmNicConfig>& nic_info, int64 vid);
    int GetVmCfgNetworkCallback(void *nil, SqlColumn * columns);    
   
private:
    enum ColNames 
    {
        ID          = 0,
        NIC_INDEX   = 1,
        SRIOV       = 2,
        LOOP        = 3,
        LOGIC_NETWORK_ID = 4,
        MODEL            = 5,
        VMTEMPLATE_LIMIT = 6,
        VSI_PROFILE_ID   = 6,
        IPADDRESS        = 7,
        NETMASK          = 8,
        GATEWAY          = 9,
        MAC              = 10,
        VM_LIMIT         = 11
    };

    
    VmBaseNicPool(SqlDB *pDb)
    {
        db = pDb;
    };
    
    DISALLOW_COPY_AND_ASSIGN(VmBaseNicPool);
    static VmBaseNicPool *_instance;
    SqlDB                *db;
    static const char *  _table_vmcfg_nic;
    static const char *  _table_vmtemplate_nic;    
    
    static const char *  _col_names_vmcfg_nic;    
    static const char *  _col_names_vmtemplate_nic; 

};

class VmBaseContext : public Serializable
{
public:
    VmBaseContext()
    {
    }; 
    
    VmBaseContext(int64      id, 
                         map<string, string> &str_map)
    {
        _id       = id;
        _str_map  = str_map;
    };

    SERIALIZE
    {
        SERIALIZE_BEGIN(VmBaseContext);
        WRITE_VALUE(_id);
        WRITE_VALUE(_str_map);       
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(VmBaseContext);
        READ_VALUE(_id);
        READ_VALUE(_str_map);
        DESERIALIZE_END();
    };

    int64                  _id; //虚拟机ID或模板ID
    map<string, string >   _str_map;

};

class VmBaseContextPool: public Callbackable
{
public:
    static VmBaseContextPool* GetInstance(SqlDB *db = NULL)
    {   
        if(NULL == _instance)
        {
            SkyAssert(NULL != db);
            _instance = new VmBaseContextPool(db);  
        }
        
        return _instance;
    };
    
    int SelectCallback(void *nil, SqlColumn * columns);
    int GetVmBaseContext(VmBaseContext  & vmbase_context, int64 id, bool is_vm);
    int SetVmBaseContext(VmBaseContext  & vmbase_context, int64 id, bool is_vm);
    int DropVmBaseContext(int64 id, bool is_vm);
    int InsertVmBaseContext(VmBaseContext  &vmbase_context, int64 id,bool is_vm);

   
private:
    enum ColNames 
    {
        ID           = 0,
        FILE_NAME    = 1,
        FILE_CONTENT = 2,
        LIMIT        = 3
    };
    
    VmBaseContextPool(SqlDB *pDb)
    {
        db = pDb;
    };
    
    DISALLOW_COPY_AND_ASSIGN(VmBaseContextPool);
    static VmBaseContextPool *_instance;
    SqlDB              *db;
    static const char *  _table_vmcfg_context;
    static const char *  _table_vmtemplate_context;
    
    static const char *  _col_names_vmcfg_context;    
    static const char *  _col_names_vmtemplate_context; 
};


class VmBaseDevicePool: public Callbackable
{
public:
    static VmBaseDevicePool* GetInstance(SqlDB *db = NULL)
    {   
        if(NULL == _instance)
        {
            SkyAssert(NULL != db);
            _instance = new VmBaseDevicePool(db);  
        }
        
        return _instance;
    };
    
    int SelectCallback(void *nil, SqlColumn * columns);
    int GetVmDevices(vector<VmDeviceConfig> &devices, int64 id, bool is_vm);
    int SetVmDevices(const vector<VmDeviceConfig> &devices, int64 id, bool is_vm);
    int DropVmDevices(int64 id, bool is_vm);
    int InsertVmDevices(const vector<VmDeviceConfig> &devices, int64 id,bool is_vm);

private:
    enum ColNames 
    {
        ID              = 0,
        TYPE            = 1,
        COUNT           = 2,
        LIMIT           = 3
    };
    
    VmBaseDevicePool(SqlDB *pDb)
    {
        db = pDb;
    };
    
    DISALLOW_COPY_AND_ASSIGN(VmBaseDevicePool);
    static VmBaseDevicePool *_instance;
    SqlDB              *db;
    static const char *  _cfg_table_name;
    static const char *  _template_table_name;
    
    static const char *  _col_names;    
    static const char *  _template_col_names; 
};


}

  /* end namespace zte_tecs */

#endif /* end TC_VMCFGBASE_H */
