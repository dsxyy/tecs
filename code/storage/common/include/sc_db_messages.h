/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：sc_db_messages.h
* 文件标识：见配置管理计划书
* 内容摘要：sc数据库实体类头文件
* 当前版本：1.0
* 作    者：颜伟
* 完成日期：2012年9月27日
*
* 修改记录1：
*     修改日期：2012/9/27
*     版 本 号：V1.0
*     修 改 人：颜伟
*     修改内容：创建
*******************************************************************************/

#ifndef SC_DB_MESSAGES_H
#define SC_DB_MESSAGES_H

#include "sky.h"
#include "tecs_pub.h"

namespace zte_tecs 
{

/**
@brief 功能描述: StorageAdaptorAddress实体类\n
@li @b 其它说明: 无
*/
class StorageAdaptorAddress : public Serializable
{
public:  
    StorageAdaptorAddress()   
    {
    };

    StorageAdaptorAddress(const string& application, 
                       const string& ip_address)   
    {
        _application = application;
        _ip_address = ip_address;
    };

    SERIALIZE
    {
        SERIALIZE_BEGIN(StorageAdaptorAddress);
        WRITE_VALUE(_application);
        WRITE_VALUE(_ip_address);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(StorageAdaptorAddress);
        READ_VALUE(_application);
        READ_VALUE(_ip_address);      
        DESERIALIZE_END();
    };

    string   _application;
    string   _ip_address;
};

/**
@brief 功能描述: StorageAdaptor实体类\n
@li @b 其它说明: 无
*/
class StorageAdaptor : public Serializable
{
public:  
    StorageAdaptor()   
    {
        _sid        = INVALID_SID;
        _is_online  = false;
        _enabled    = false;
    };

    SERIALIZE
    {
        SERIALIZE_BEGIN(StorageAdaptor);
        WRITE_VALUE(_sid);
        WRITE_VALUE(_name);
        WRITE_VALUE(_application);
        WRITE_VALUE(_ctrl_address);
        WRITE_VALUE(_type);
        WRITE_VALUE(_is_online);
        WRITE_VALUE(_enabled);
        WRITE_VALUE(_description);
        WRITE_OBJECT(_register_time);
        WRITE_OBJECT(_refresh_time);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(StorageAdaptor);
        READ_VALUE(_sid);
        READ_VALUE(_name);
        READ_VALUE(_application);
        READ_VALUE(_ctrl_address);
        READ_VALUE(_type);
        READ_VALUE(_is_online);
        READ_VALUE(_enabled);
        READ_VALUE(_description);
        READ_OBJECT(_register_time); 
        READ_OBJECT(_refresh_time); 
        DESERIALIZE_END();
    };

    int64    _sid;
    string   _name;
    string   _application;
    string   _ctrl_address;
    string   _type;
    bool     _is_online;
    bool     _enabled;
    string   _description;
    Datetime _register_time;
    Datetime _refresh_time;
};

/**
@brief 功能描述: StorageAddress实体类\n
@li @b 其它说明: 无
*/
class StorageAddress : public Serializable
{
public:  
    StorageAddress()   
    {
        _sid = INVALID_SID;
    };

    StorageAddress(int64 sid, const string& media_address)   
    {
        _sid           = sid;
        _media_address = media_address;
    };

    SERIALIZE
    {
        SERIALIZE_BEGIN(StorageAddress);
        WRITE_VALUE(_sid);
        WRITE_VALUE(_media_address);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(StorageAddress);
        READ_VALUE(_sid);
        READ_VALUE(_media_address);      
        DESERIALIZE_END();
    };

    int64    _sid;
    string   _media_address;
};

/**
@brief 功能描述: StorageVg实体类\n
@li @b 其它说明: 无
*/
class StorageVg : public Serializable
{
public:  
    StorageVg()   
    {
        _sid  = INVALID_SID;
        _size = 0;
    };

    StorageVg(int64 sid, const string& name, int64 size)   
    {
        _sid  = sid;
        _name = name;
        _size = size;
    };

    SERIALIZE
    {
        SERIALIZE_BEGIN(StorageVg);
        WRITE_VALUE(_sid);
        WRITE_VALUE(_name);
        WRITE_VALUE(_size);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(StorageVg);
        READ_VALUE(_sid);
        READ_VALUE(_name); 
        READ_VALUE(_size);
        DESERIALIZE_END();
    };

    int64    _sid;
    string   _name;
    int64    _size;
};

/**
@brief 功能描述: StorageTarget实体类\n
@li @b 其它说明: 无
*/
class StorageTarget : public Serializable
{
public:  
    StorageTarget()   
    {
        _sid = INVALID_SID;
        _target_id = INVALID_OID;
    };

    StorageTarget(int64 sid, 
                     const string& name,
                     int64 target_id)   
    {
        _sid  = sid;
        _name = name;
        _target_id = target_id;
    };

    SERIALIZE
    {
        SERIALIZE_BEGIN(StorageTarget);
        WRITE_VALUE(_sid);
        WRITE_VALUE(_name);
        WRITE_VALUE(_target_id);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(StorageTarget);
        READ_VALUE(_sid);
        READ_VALUE(_name); 
        READ_VALUE(_target_id);
        DESERIALIZE_END();
    };

    int64    _sid;
    string   _name;
    int64    _target_id;
};

/**
@brief 功能描述: StorageAuthorize实体类\n
@li @b 其它说明: 无
*/
class StorageAuthorize : public Serializable
{
public:  
    StorageAuthorize()   
    {
        _sid = INVALID_SID;
    };

    StorageAuthorize(int64 sid, 
                     const string& target_name, 
                     const string& ip_address,
                     const string& iscsi_initiator,
                     const string& auth_uuid)   
    {
        _sid  = sid;
        _target_name     = target_name;
        _ip_address      = ip_address;
        _iscsi_initiator = iscsi_initiator;
        _auth_uuid       = auth_uuid;
    };

    SERIALIZE
    {
        SERIALIZE_BEGIN(StorageAuthorize);
        WRITE_VALUE(_sid);
        WRITE_VALUE(_target_name);
        WRITE_VALUE(_ip_address);
        WRITE_VALUE(_iscsi_initiator);
        WRITE_VALUE(_auth_uuid);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(StorageAuthorize);
        READ_VALUE(_sid);
        READ_VALUE(_target_name); 
        READ_VALUE(_ip_address);
        READ_VALUE(_iscsi_initiator);
        READ_VALUE(_auth_uuid);
        DESERIALIZE_END();
    };

    int64    _sid;
    string   _target_name;
    string   _ip_address;
    string   _iscsi_initiator;
    string   _auth_uuid;
};


/**
@brief 功能描述: StorageCluster实体类\n
@li @b 其它说明: 无
*/
class StorageCluster : public Serializable
{
public:  
    StorageCluster()   
    {
        _sid = INVALID_SID;
    };

    StorageCluster(int64 sid, const string& cluster_name)   
    {
        _sid = sid;
        _cluster_name = cluster_name;
    };

    SERIALIZE
    {
        SERIALIZE_BEGIN(StorageCluster);
        WRITE_VALUE(_sid);
        WRITE_VALUE(_cluster_name);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(StorageCluster);
        READ_VALUE(_sid);
        READ_VALUE(_cluster_name);      
        DESERIALIZE_END();
    };

    int64    _sid;
    string   _cluster_name;
};

/**
@brief 功能描述: StorageVolume实体类\n
@li @b 其它说明: 无
*/
class StorageVolume : public Serializable
{
public:  
    StorageVolume()   
    {
        _sid  = INVALID_SID;
        _size = 0;
        _volume_id  = INVALID_OID;

    };

    StorageVolume(int64 sid, 
                       const string& vg_name,
                       const string& name,
                       int64 size,
                       int64 volume_id,
                       const string& volume_uuid,
                       const string& request_id)   
    {
        _sid         = sid;
        _vg_name     = vg_name;
        _name        = name;
        _size        = size;
        _volume_id   = volume_id;
        _volume_uuid = volume_uuid;
        _request_id  = request_id;
    };

    SERIALIZE
    {
        SERIALIZE_BEGIN(StorageVolume);
        WRITE_VALUE(_sid);
        WRITE_VALUE(_vg_name);
        WRITE_VALUE(_name);
        WRITE_VALUE(_size);
        WRITE_VALUE(_volume_id);
        WRITE_VALUE(_volume_uuid);
        WRITE_VALUE(_request_id);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(StorageVolume);
        READ_VALUE(_sid);     
        READ_VALUE(_vg_name);  
        READ_VALUE(_name);
        READ_VALUE(_size);     
        READ_VALUE(_volume_id);  
        READ_VALUE(_volume_uuid); 
        READ_VALUE(_request_id); 
        DESERIALIZE_END();
    };

    int64    _sid;
    string   _vg_name;
    string   _name;
    int64    _size;
    int64    _volume_id;
    string   _volume_uuid;
    string   _request_id;
};


/**
@brief 功能描述: StorageLun实体类\n
@li @b 其它说明: 无
*/
class StorageLun : public Serializable
{
public:  
    StorageLun()   
    {
        _sid    = INVALID_SID;
        _lun_id = INVALID_LUNID;
    };

    StorageLun(int64 sid, 
                  const string& target_name, 
                  int64 lun_id,
                  const string& lun_uuid,
                  const string& volume_uuid)   
    {
        _sid         = sid;
        _target_name = target_name;
        _lun_id      = lun_id;
        _lun_uuid    = lun_uuid;
        _volume_uuid = volume_uuid;
    };

    SERIALIZE
    {
        SERIALIZE_BEGIN(StorageLun);
        WRITE_VALUE(_sid);
        WRITE_VALUE(_target_name);
        WRITE_VALUE(_lun_id);
        WRITE_VALUE(_lun_uuid);
        WRITE_VALUE(_volume_uuid);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(StorageLun);
        READ_VALUE(_sid);
        READ_VALUE(_target_name);     
        READ_VALUE(_lun_id);  
        READ_VALUE(_lun_uuid);     
        READ_VALUE(_volume_uuid);  
        DESERIALIZE_END();
    };

    int64    _sid;
    string   _target_name;
    int64    _lun_id;
    string   _lun_uuid;
    string   _volume_uuid;
};

/**
@brief 功能描述: StorageAuthorizeLun实体类\n
@li @b 其它说明: 无
*/
class StorageAuthorizeLun : public Serializable
{
public:  
    StorageAuthorizeLun()   
    {
    };

    StorageAuthorizeLun(const string& auth_uuid, const string& lun_uuid)   
    {
        _auth_uuid = auth_uuid;
        _lun_uuid = lun_uuid;
    };

    SERIALIZE
    {
        SERIALIZE_BEGIN(StorageAuthorizeLun);
        WRITE_VALUE(_auth_uuid);
        WRITE_VALUE(_lun_uuid);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(StorageAuthorizeLun);
        READ_VALUE(_auth_uuid);     
        READ_VALUE(_lun_uuid);  
        DESERIALIZE_END();
    };

    string   _auth_uuid;
    string   _lun_uuid;
};

/**
@brief 功能描述: StorageUserVolume实体类\n
@li @b 其它说明: 无
*/
class StorageUserVolume : public Serializable
{
public:  
    StorageUserVolume()   
    {
        _uid = INVALID_UID;
    };

    StorageUserVolume(int64 uid,
                             const string& request_id, 
                             const string& usage,
                             const string& description):   
    _uid(uid), _request_id(request_id), _usage(usage), _description(description)
    {
    };

    SERIALIZE
    {
        SERIALIZE_BEGIN(StorageUserVolume);
        WRITE_VALUE(_uid);
        WRITE_VALUE(_request_id);
        WRITE_VALUE(_usage);
        WRITE_VALUE(_description);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(StorageUserVolume);
        READ_VALUE(_uid);     
        READ_VALUE(_request_id);  
        READ_VALUE(_usage);     
        READ_VALUE(_description);  
        DESERIALIZE_END();
    };

    int64    _uid;
    string   _request_id;
    string   _usage;
    string   _description;
};

/**
@brief 功能描述: StorageSnapshot实体类\n
@li @b 其它说明: 无
*/
class StorageSnapshot : public Serializable
{
public:  
    StorageSnapshot()   
    {
        
    };

    StorageSnapshot(const string& base_id,const string& snap_id):   
    _base_uuid(base_id), _snap_uuid(snap_id)
    {
    };

    SERIALIZE
    {
        SERIALIZE_BEGIN(StorageSnapshot);
        WRITE_VALUE(_base_uuid);
        WRITE_VALUE(_snap_uuid);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(StorageSnapshot);
        READ_VALUE(_base_uuid);     
        READ_VALUE(_snap_uuid);        
        DESERIALIZE_END();
    };
    
    string   _base_uuid;
    string   _snap_uuid;    
};


/**
@brief 功能描述: StorageVolumeWithLun实体类\n
@li @b 其它说明: 无
*/
class StorageVolumeWithLun : public Serializable
{
public:  
    StorageVolumeWithLun()   
    {
        _sid = INVALID_SID;
        _lun_id = INVALID_LUNID;
    };

    StorageVolumeWithLun(int64 sid, 
                       const string& vg_name,
                       const string& volume_name,
                       const string& volume_uuid,
                       const string& target_name,
                       int64 lun_id,
                       const string& lun_uuid)   
    {
        _sid         = sid;
        _vg_name     = vg_name;
        _volume_name = volume_name;
        _volume_uuid = volume_uuid;
        _target_name = target_name;
        _lun_id      = lun_id;
        _lun_uuid    = lun_uuid;
    };

    SERIALIZE
    {
        SERIALIZE_BEGIN(StorageVolumeWithLun);
        WRITE_VALUE(_sid);
        WRITE_VALUE(_vg_name);
        WRITE_VALUE(_volume_name);
        WRITE_VALUE(_volume_uuid);
        WRITE_VALUE(_target_name);
        WRITE_VALUE(_lun_id);
        WRITE_VALUE(_lun_uuid);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(StorageVolumeWithLun);
        READ_VALUE(_sid);     
        READ_VALUE(_vg_name);  
        READ_VALUE(_volume_name);
        READ_VALUE(_volume_uuid);  
        READ_VALUE(_target_name);     
        READ_VALUE(_lun_id); 
        READ_VALUE(_lun_uuid); 
        DESERIALIZE_END();
    };

    int64    _sid;
    string   _vg_name;
    string   _volume_name;
    string   _volume_uuid;
    string   _target_name;
    int64    _lun_id;
    string   _lun_uuid;
};

/**
@brief 功能描述: StorageAdaptorWithVgAndCluster实体类\n
@li @b 其它说明: 无
*/
class StorageAdaptorWithVgAndCluster : public Serializable
{
public:  
    StorageAdaptorWithVgAndCluster()   
    {
        _sid = INVALID_SID;
        _sa_total_free_size = 0;
        _max_vg_free_size = 0;
    };

    SERIALIZE
    {
        SERIALIZE_BEGIN(StorageAdaptorWithVgAndCluster);
        WRITE_VALUE(_sid);
        WRITE_VALUE(_cluster_name);
        WRITE_VALUE(_application);
        WRITE_VALUE(_unit);
        WRITE_VALUE(_type);
        WRITE_VALUE(_vg_name);
        WRITE_VALUE(_sa_total_free_size);
        WRITE_VALUE(_max_vg_free_size);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(StorageAdaptorWithVgAndCluster);
        READ_VALUE(_sid);
        READ_VALUE(_cluster_name);     
        READ_VALUE(_application);  
        READ_VALUE(_unit);
        READ_VALUE(_type);
        READ_VALUE(_vg_name);
        READ_VALUE(_sa_total_free_size);  
        READ_VALUE(_max_vg_free_size);
        DESERIALIZE_END();
    };

    int64     _sid;
    string    _cluster_name;
    string    _application;
    string    _unit;
    string    _type;
    string    _vg_name;
    int64     _sa_total_free_size;
    int64     _max_vg_free_size;
};

/**
@brief 功能描述: StorageAdaptorWithMultivgAndCluster实体类\n
@li @b 其它说明: 无
*/
class StorageAdaptorWithMultivgAndCluster : public Serializable
{
public:  
    StorageAdaptorWithMultivgAndCluster()   
    {
        _sid = INVALID_SID;
        _sa_total_free_size = 0;
        _vg_free_size = 0;
    };

    SERIALIZE
    {
        SERIALIZE_BEGIN(StorageAdaptorWithMultivgAndCluster);
        WRITE_VALUE(_sid);
        WRITE_VALUE(_cluster_name);
        WRITE_VALUE(_application);
        WRITE_VALUE(_unit);
        WRITE_VALUE(_type);
        WRITE_VALUE(_vg_name);
        WRITE_VALUE(_sa_total_free_size);
        WRITE_VALUE(_vg_free_size);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(StorageAdaptorWithMultivgAndCluster);
        READ_VALUE(_sid);
        READ_VALUE(_cluster_name);     
        READ_VALUE(_application);  
        READ_VALUE(_unit);
        READ_VALUE(_type);
        READ_VALUE(_vg_name);
        READ_VALUE(_sa_total_free_size);  
        READ_VALUE(_vg_free_size);
        DESERIALIZE_END();
    };

    int64     _sid;
    string    _cluster_name;
    string    _application;
    string    _unit;
    string    _type;
    string    _vg_name;
    int64     _sa_total_free_size;
    int64     _vg_free_size;
};


/**
@brief 功能描述: StorageVolumeWithLunAndAuthAndAdaptor实体类\n
@li @b 其它说明: 无
*/
class StorageVolumeWithLunAndAuthAndAdaptor : public Serializable
{
public:  
    StorageVolumeWithLunAndAuthAndAdaptor()   
    {
        _lun_id = INVALID_LUNID;
    };

    SERIALIZE
    {
        SERIALIZE_BEGIN(StorageVolumeWithLunAndAuthAndAdaptor);
        WRITE_VALUE(_vol_uuid);
        WRITE_VALUE(_target_name);
        WRITE_VALUE(_lun_id);
        WRITE_VALUE(_ip_address);
        WRITE_VALUE(_iscsi_initiator);
        WRITE_VALUE(_application);
        WRITE_VALUE(_unit);
        WRITE_VALUE(_type);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(StorageVolumeWithLunAndAuthAndAdaptor);
        READ_VALUE(_vol_uuid);     
        READ_VALUE(_target_name);  
        READ_VALUE(_lun_id);
        READ_VALUE(_ip_address);     
        READ_VALUE(_iscsi_initiator);  
        READ_VALUE(_application);
        READ_VALUE(_unit);
        READ_VALUE(_type);
        DESERIALIZE_END();
    };

    string    _vol_uuid;
    string    _target_name;
    int64     _lun_id;
    string    _ip_address;
    string    _iscsi_initiator;
    string    _application;
    string    _unit;
    string    _type;
};

/**
@brief 功能描述: StorageVolumeWithLunAndAddress实体类\n
@li @b 其它说明: 无
*/
class StorageVolumeWithLunAndAddress : public Serializable
{
public:  
    StorageVolumeWithLunAndAddress()   
    {
        _lun_id = INVALID_LUNID;
    };

    SERIALIZE
    {
        SERIALIZE_BEGIN(StorageVolumeWithLunAndAddress);
        WRITE_VALUE(_vol_uuid);
        WRITE_VALUE(_lun_id);
        WRITE_VALUE(_media_address);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(StorageVolumeWithLunAndAddress);
        READ_VALUE(_vol_uuid); 
        READ_VALUE(_lun_id);     
        READ_VALUE(_media_address);  
        DESERIALIZE_END();
    };

    string    _vol_uuid;
    int64     _lun_id;
    string    _media_address;
};

/**
@brief 功能描述: StorageVolumeWithAdaptor实体类\n
@li @b 其它说明: 无
*/
class StorageVolumeWithAdaptor : public Serializable
{
public:  
    StorageVolumeWithAdaptor()   
    {
        _sid = INVALID_SID;
    };

    SERIALIZE
    {
        SERIALIZE_BEGIN(StorageVolumeWithAdaptor);
        WRITE_VALUE(_vol_uuid);
        WRITE_VALUE(_sid);
        WRITE_VALUE(_vg_name);
        WRITE_VALUE(_vol_name);
        WRITE_VALUE(_application);
        WRITE_VALUE(_unit);
        WRITE_VALUE(_type);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(StorageVolumeWithAdaptor);
        READ_VALUE(_vol_uuid); 
        READ_VALUE(_sid); 
        READ_VALUE(_vg_name);     
        READ_VALUE(_vol_name); 
        READ_VALUE(_application);  
        READ_VALUE(_unit);  
        READ_VALUE(_type);
        DESERIALIZE_END();
    };

    string    _vol_uuid;
    int64     _sid;
    string    _vg_name;
    string    _vol_name;
    string    _application;
    string    _unit;
    string    _type;
};

/**
@brief 功能描述: StorageAuthorizeWithTargetId实体类\n
@li @b 其它说明: 无
*/
class StorageAuthorizeWithTargetId : public Serializable
{
public:  
    StorageAuthorizeWithTargetId()   
    {
        _target_id = INVALID_OID;
        _sid = INVALID_SID;
    };

    StorageAuthorizeWithTargetId(int64 target_id,
                     int64 sid, 
                     const string& target_name, 
                     const string& ip_address,
                     const string& iscsi_initiator,
                     const string& auth_uuid)   
    {
        _target_id = target_id;
        _sid = sid;
        _target_name = target_name;
        _ip_address = ip_address;
        _iscsi_initiator = iscsi_initiator;
        _auth_uuid = auth_uuid;
    };

    SERIALIZE
    {
        SERIALIZE_BEGIN(StorageAuthorizeWithTargetId);
        WRITE_VALUE(_target_id);
        WRITE_VALUE(_sid);
        WRITE_VALUE(_target_name);
        WRITE_VALUE(_ip_address);
        WRITE_VALUE(_iscsi_initiator);
        WRITE_VALUE(_auth_uuid);
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(StorageAuthorizeWithTargetId);
        READ_VALUE(_target_id);
        READ_VALUE(_sid);
        READ_VALUE(_target_name); 
        READ_VALUE(_ip_address);
        READ_VALUE(_iscsi_initiator);
        READ_VALUE(_auth_uuid);
        DESERIALIZE_END();
    };

    int64    _target_id;
    int64    _sid;
    string   _target_name;
    string   _ip_address;
    string   _iscsi_initiator;
    string   _auth_uuid;
};

}
#endif

