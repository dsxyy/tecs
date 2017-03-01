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

#include "vmcfgbase.h"
#include "log_agent.h"

namespace zte_tecs{

      
const char * VmBaseImagePool::_table_vmcfg_image = 
        "vmcfg_image";  //虚拟机 映像表

const char * VmBaseImagePool::_table_vmtemplate_image = 
        "vmtemplate_image";  //模板 映像表

const char * VmBaseImagePool::_view_vmcfg_image =
        "view_vmcfg_image" ;
        
const char * VmBaseImagePool::_view_vmtemplate_image =
        "view_vmtemplate_image" ;

        
const char * VmBaseImagePool::_col_names_vmcfg_image = 
        "vid,"
        "target,"
        "position,"
        "image_id," 
        "reserved_backup";
       
const char * VmBaseImagePool::_col_names_vmtemplate_image = 
        "vmtemplate_id,"
        "target,"
        "position,"
        "image_id," 
        "reserved_backup";

const char * VmBaseImagePool::_col_names_view_vmcfg_image =
        "vid,"
        "target,"
        "image_id,"
        "position,"
        "reserved_backup,"
        "type,"
        "size,"
        "bus,"
        "fileurl,"
        "location,"
        "image_owner,"
        "os_type,"
        "disk_size,"
        "disk_format,"
        "os_arch" ;

const char * VmBaseImagePool::_col_names_view_vmtemplate_image =
        "vmtemplate_id,"
        "target,"
        "image_id,"
        "position,"
        "reserved_backup,"
        "type,"
        "size,"
        "bus,"
        "fileurl,"
        "location,"
        "image_owner,"
        "os_type,"
        "disk_size,"
        "disk_format,"
        "os_arch" ;

VmBaseImagePool *VmBaseImagePool::_instance = NULL;

/******************************************************************************/
int VmBaseImagePool::SelectCallback(void *nil, SqlColumn * columns)
{
    if ( NULL == columns || LIMIT != columns->get_column_num())
    {
        return -1;
    }
    
    VmBaseImage  vmbase_image;
    columns->GetValue(VmBaseImagePool::ID,      vmbase_image._id);
    columns->GetValue(VmBaseImagePool::TARGET,   vmbase_image._target);
    columns->GetValue(VmBaseImagePool::POSITION, vmbase_image._position);
    columns->GetValue(VmBaseImagePool::IMAGE_ID, vmbase_image._image_id);
    columns->GetValue(VmBaseImagePool::RESERVED_BACKUP, vmbase_image._reserved_backup);
    
    vector<VmBaseImage>* p_vmbase_images = static_cast<vector<VmBaseImage> *>(nil);
    p_vmbase_images->push_back(vmbase_image);
    return 0;
}

/******************************************************************************/
int VmBaseImagePool::GetVmBaseImages(vector<VmBaseImage>& vmbase_images, 
                                                 int64 id, 
                                                 bool is_vm)
{
    ostringstream   oss;
    int             rc;

    if (is_vm)
    {
       oss << "SELECT " << _col_names_vmcfg_image << 
             " FROM " << _table_vmcfg_image <<
             " WHERE vid = " << id;
    }
    else
    {
       oss << "SELECT " << _col_names_vmtemplate_image << 
             " FROM " << _table_vmtemplate_image <<
             " WHERE vmtemplate_id = " << id;
    }

    SetCallback(static_cast<Callbackable::Callback>(&VmBaseImagePool::SelectCallback),&vmbase_images);
    rc = db->Execute(oss, this);
    UnsetCallback();
    if ( 0 != rc && SQLDB_RESULT_EMPTY != rc) 
    {
        SkyAssert(false);
    }
    return rc;
}

/******************************************************************************/
int VmBaseImagePool::SetVmBaseImages(vector<VmBaseImage>& vmbase_images, 
                                                    int64 id, 
                                                    bool is_vm)
{
    int rc = -1;
    
    SqlCommand sql(db, SqlCommand::WITH_TRANSACTION);
    if (is_vm)
    {
        sql.SetTable(_table_vmcfg_image);
        rc = sql.Delete(" WHERE vid = " + to_string(id,dec));
    }
    else
    {
        sql.SetTable(_table_vmtemplate_image);
        rc = sql.Delete(" WHERE vmtemplate_id = " + to_string(id,dec));
    }    
    
    if ( (0 != rc) 
         &&(SQLDB_RESULT_EMPTY != rc) )
    {
        SkyAssert(false);
        return rc;
    }

    vector<VmBaseImage>::const_iterator  it;
    for(it = vmbase_images.begin(); it != vmbase_images.end(); it++)
    {
        sql.Clear();
        if (is_vm)
        {
            sql.Add("vid", id);
        }
        else
        {
            sql.Add("vmtemplate_id", id);
        }
        
        sql.Add("target",   it->_target);
        sql.Add("position", it->_position);
        sql.Add("image_id", it->_image_id);
        sql.Add("reserved_backup", it->_reserved_backup);
        
        rc = sql.Insert();
        if(rc != 0)
        {
            SkyAssert(false);
            break;
        }
    }
    return rc;
}

/******************************************************************************/
int VmBaseImagePool::DropVmBaseImages(int64 id, bool is_vm)
{
    ostringstream oss;
    int rc;
    if (is_vm)
    {
       oss << "DELETE FROM " << _table_vmcfg_image << " WHERE vid = " << id ;
    }
    else
    {
      oss << "DELETE FROM " << _table_vmtemplate_image << " WHERE vmtemplate_id = " << id ;
    }
    
    if(Transaction::On())
    {
        rc = Transaction::Append(oss.str());
    }
    else
    {
        rc = db->Execute(oss);
    }
    return rc;
    
}

int VmBaseImagePool::DropVmBaseImages(const VmBaseImage &image)
{
    ostringstream oss;

    oss << "DELETE FROM " << _table_vmcfg_image << " WHERE vid = " << image._id
                                                << " and target = '" << image._target << "'";
    
    int rc;
    if(Transaction::On())
    {
        rc = Transaction::Append(oss.str());
    }
    else
    {
        rc = db->Execute(oss);
    }
    return rc;
}

/******************************************************************************/
int VmBaseImagePool::InsertVmBaseImages(const vector<VmBaseImage>& vmbase_images,
                                                 int64  id,
                                                 bool   is_vm)
{
    int rc = 0;

    SqlCommand sql(db, SqlCommand::WITH_TRANSACTION);
    if (is_vm)
    {
       sql.SetTable(VmBaseImagePool::_table_vmcfg_image);
    }
    else
    {
       sql.SetTable(VmBaseImagePool::_table_vmtemplate_image);
    }   
    
    vector<VmBaseImage>::const_iterator it;
    for(it = vmbase_images.begin(); it != vmbase_images.end(); it++)
    {
        sql.Clear();
        if (is_vm)
        {
          sql.Add("vid", id);
        }
        else
        {
          sql.Add("vmtemplate_id", id);
        }

        sql.Add("target",it->_target);
        sql.Add("position",it->_position);
        if (INVALID_IID != it->_image_id)
        {
            sql.Add("image_id",it->_image_id);
        }
        sql.Add("reserved_backup", it->_reserved_backup);
        
        rc = sql.Insert();
        if(rc != 0)
        {
            SkyAssert(false);
            break;
        }
    }
    return rc;
}

/******************************************************************************/
int VmBaseImagePool::GetImageInfoByViewCallback(void *nil, SqlColumn * columns)
{
    VmBaseImageInfo          info;    
    vector<VmBaseImageInfo> *p_info;
    
    p_info = static_cast<vector<VmBaseImageInfo> *>(nil);
	
    if ( NULL == columns || 
         VmBaseImagePool::VIEW_LIMIT != columns->get_column_num())
    {
        return -1;
    }
    
    columns->GetValue(VmBaseImagePool::VIEW_ID,       info._id);
    columns->GetValue(VmBaseImagePool::VIEW_TARGET,   info._target);
    columns->GetValue(VmBaseImagePool::VIEW_POSITION, info._position);
    columns->GetValue(VmBaseImagePool::VIEW_IMAGE_ID, info._image_id);    
    columns->GetValue(VmBaseImagePool::VIEW_RESERVED_BACKUP, info._reserved_backup);  
    columns->GetValue(VmBaseImagePool::VIEW_SIZE,     info._size);
    columns->GetValue(VmBaseImagePool::VIEW_TYPE,     info._type);
    columns->GetValue(VmBaseImagePool::VIEW_BUS,      info._bus);    
    columns->GetValue(VmBaseImagePool::VIEW_FILEURL,  info._url);
    columns->GetValue(VmBaseImagePool::VIEW_OS_TYPE,  info._os_type);
    columns->GetValue(VmBaseImagePool::VIEW_DISK_SIZE,  info._disk_size);
    columns->GetValue(VmBaseImagePool::VIEW_DISK_FORMAT,  info._disk_format);
    columns->GetValue(VmBaseImagePool::VIEW_OS_ARCH,  info._os_arch);

    p_info->push_back(info);
    
    return SUCCESS;
    
}

/******************************************************************************/
int VmBaseImagePool::GetImageInfoByView(vector<VmBaseImageInfo>& image_infos, 
                                                 int64 id, 
                                                 bool is_vm)

{
    ostringstream   oss;
    int             rc;

    if (is_vm)
    {
       oss << "SELECT " << _col_names_view_vmcfg_image << 
             " FROM " <<  _view_vmcfg_image <<
             " WHERE vid = " << id;
    }
    else
    {
       oss << "SELECT " << _col_names_view_vmtemplate_image << 
             " FROM " << _view_vmtemplate_image <<
             " WHERE vmtemplate_id = " << id;
    }

    SetCallback(static_cast<Callbackable::Callback>(&VmBaseImagePool::GetImageInfoByViewCallback),
                &image_infos);
                
    rc = db->Execute(oss, this);
    UnsetCallback();
    if ( 0 != rc && SQLDB_RESULT_EMPTY != rc) 
    {
        SkyAssert(false);
    }
    return rc;
}

/******************************************************************************/

int VmBaseImagePool::GetImagesSize(int64  id, 
                                        bool   is_vm, 
                                        int    position,
                                        int64 &total_size)
{
    int             rc;
    ostringstream  where;    
    string         column = " sum(size) ";
    
    SqlCallbackable sql(db);
    total_size = 0;
    
    if (is_vm)
    {
        where << " vid = " << id << " and position = " << position;
        rc = sql.SelectColumn(_view_vmcfg_image,
                              column,
                              where.str(), 
                              total_size);
    }
    else
    {
        where << " vmtemplate_id = " << id << " and position = " << position; 
        rc = sql.SelectColumn(_view_vmtemplate_image,
                              column,
                              where.str(), 
                              total_size);
    }

    return rc;

}

/******************************************************************************/

const char * VmBaseDiskPool::_table_vmcfg_disk = 
        "vmcfg_disk";  //虚拟机 映像表

const char * VmBaseDiskPool::_table_vmtemplate_disk = 
        "vmtemplate_disk";  //模板 映像表
        
const char * VmBaseDiskPool::_col_names_vmcfg_disk = 
        "vid,"
        "target,"
        "position,"
	    "bus," 
	    "size,"
	    "fstype,"
	    "volume_uuid";
       
const char * VmBaseDiskPool::_col_names_vmtemplate_disk = 
        "vmtemplate_id,"
        "target,"
        "position,"
	    "bus," 
	    "size,"
	    "fstype";

	    
VmBaseDiskPool *VmBaseDiskPool::_instance = NULL;

/******************************************************************************/
int VmBaseDiskPool::SelectCallback(void *nil, SqlColumn * columns)
{
    if ( NULL == columns )
    {
        return -1;
    }
    
    VmBaseDisk  vmbase_disk;
    columns->GetValue(VmBaseDiskPool::ID,       vmbase_disk._id);
    columns->GetValue(VmBaseDiskPool::TARGET,   vmbase_disk._target);
    columns->GetValue(VmBaseDiskPool::POSITION, vmbase_disk._position);
    columns->GetValue(VmBaseDiskPool::BUS,    vmbase_disk._bus);
    columns->GetValue(VmBaseDiskPool::SIZE,   vmbase_disk._size);
    columns->GetValue(VmBaseDiskPool::FSTYPE, vmbase_disk._fstype);
	if(LIMIT==columns->get_column_num())
	{
    	columns->GetValue(VmBaseDiskPool::VOLUUID, vmbase_disk._volume_uuid);
	}
	
    vector<VmBaseDisk>* p_vmbase_disks = static_cast<vector<VmBaseDisk> *>(nil);
    p_vmbase_disks->push_back(vmbase_disk);
    return 0;
}

/******************************************************************************/
int VmBaseDiskPool::GetVmBaseDisks(vector<VmBaseDisk>& vmbase_disks, 
                                                 int64 id, 
                                                 bool is_vm)
{
    ostringstream   oss;
    int             rc;

    if (is_vm)
    {
       oss << "SELECT " << _col_names_vmcfg_disk << 
             " FROM " << _table_vmcfg_disk <<
             " WHERE vid = " << id;
    }
    else
    {
       oss << "SELECT " << _col_names_vmtemplate_disk << 
             " FROM " << _table_vmtemplate_disk <<
             " WHERE vmtemplate_id = " << id;
    }

    SetCallback(static_cast<Callbackable::Callback>(&VmBaseDiskPool::SelectCallback),&vmbase_disks);
    rc = db->Execute(oss, this);
    UnsetCallback();
    if ( 0 != rc && SQLDB_RESULT_EMPTY != rc) 
    {
        SkyAssert(false);
    }
    return rc;
}

/******************************************************************************/
int VmBaseDiskPool::SetVmBaseDisks(vector<VmBaseDisk>& vmbase_disks, 
                                                    int64 id, 
                                                    bool is_vm)
{
    int rc = -1;
    
    SqlCommand sql(db, SqlCommand::WITH_TRANSACTION);
    if (is_vm)
    {
        sql.SetTable(_table_vmcfg_disk);
        rc = sql.Delete(" WHERE vid = " + to_string(id,dec));
    }
    else
    {
        sql.SetTable(_table_vmtemplate_disk);
        rc = sql.Delete(" WHERE vmtemplate_id = " + to_string(id,dec));
    }    
    
    if ( (0 != rc) 
         &&(SQLDB_RESULT_EMPTY != rc) )
    {
        SkyAssert(false);
        return rc;
    }

    vector<VmBaseDisk>::const_iterator  it;
    for(it = vmbase_disks.begin(); it != vmbase_disks.end(); it++)
    {
        sql.Clear();
        if (is_vm)
        {
            sql.Add("vid", id);
        }
        else
        {
            sql.Add("vmtemplate_id", id);
        }
        
        sql.Add("target",   it->_target);
        sql.Add("position", it->_position);
        sql.Add("bus",  it->_bus);
        sql.Add("size", it->_size);
        sql.Add("fstype", it->_fstype);
		if (is_vm)
			sql.Add("volume_uuid", it->_volume_uuid);
        
        rc = sql.Insert();
        if(rc != 0)
        {
            SkyAssert(false);
            break;
        }
    }
    return rc;
}

/******************************************************************************/
int VmBaseDiskPool::DropVmBaseDisks(int64 id, bool is_vm)
{
    ostringstream oss;
    int rc;
    if (is_vm)
    {
       oss << "DELETE FROM " << _table_vmcfg_disk << " WHERE vid = " << id ;
    }
    else
    {
      oss << "DELETE FROM " << _table_vmtemplate_disk << " WHERE vmtemplate_id = " << id ;
    }
    
    if(Transaction::On())
    {
        rc = Transaction::Append(oss.str());
    }
    else
    {
        rc = db->Execute(oss);
    }
    return rc;
    
}


/******************************************************************************/
int VmBaseDiskPool::InsertVmBaseDisks(const vector<VmBaseDisk>& vmbase_disks,
                                                 int64  id,
                                                 bool   is_vm)
{
    int rc = 0;

    SqlCommand sql(db, SqlCommand::WITH_TRANSACTION);
    if (is_vm)
    {
       sql.SetTable(VmBaseDiskPool::_table_vmcfg_disk);
    }
    else
    {
       sql.SetTable(VmBaseDiskPool::_table_vmtemplate_disk);
    }   
    
    vector<VmBaseDisk>::const_iterator it;
    for(it = vmbase_disks.begin(); it != vmbase_disks.end(); it++)
    {
        sql.Clear();
        if (is_vm)
        {
          sql.Add("vid", id);
        }
        else
        {
          sql.Add("vmtemplate_id", id);
        }

        sql.Add("target",   it->_target);
        sql.Add("position", it->_position);
        sql.Add("bus",  it->_bus);
        sql.Add("size", it->_size);
        sql.Add("fstype", it->_fstype);
        if (is_vm)
			sql.Add("volume_uuid", it->_volume_uuid);
        rc = sql.Insert();
        if(rc != 0)
        {
            SkyAssert(false);
            break;
        }
    }
    return rc;
}

/******************************************************************************/
int VmBaseDiskPool::GetDisksSize(int64  id, 
                                        bool   is_vm, 
                                        int    position,
                                        int64 &total_size)
{
    int             rc;
    ostringstream  where;    
    string         column = " sum(size) ";
    
    SqlCallbackable sql(db);
    total_size = 0;
    
    if (is_vm)
    {
        where << " vid = " << id << " and position = " << position;
        rc = sql.SelectColumn(_table_vmcfg_disk,
                              column,
                              where.str(), 
                              total_size);
    }
    else
    {
        where << " vmtemplate_id = " << id << " and position = " << position; 
        rc = sql.SelectColumn(_table_vmtemplate_disk,
                              column,
                              where.str(), 
                              total_size);
    }

    return rc;

}

/******************************************************************************/

const char * VmBaseNicPool::_table_vmcfg_nic = 
        "vmcfg_nic";  //虚拟机 映像表

const char * VmBaseNicPool::_table_vmtemplate_nic = 
        "vmtemplate_nic";  //模板 映像表

const char * VmBaseNicPool::_col_names_vmcfg_nic = 
        "vid,"
        "nic_index,"
        "sriov,"
        "loop,"
	"logic_network_id,"
	"model," 
	"vsi_profile_id,"
        "ip_address,"
        "netmask,"
        "gateway,"
        "mac";
       
const char * VmBaseNicPool::_col_names_vmtemplate_nic = 
        "vmtemplate_id,"
        "nic_index,"
        "sriov,"
        "loop,"
	    "logic_network_id,"
	    "model" ;
        // "vsi_profile_id"; 模板里没有该字段
 	    
VmBaseNicPool *VmBaseNicPool::_instance = NULL;

/******************************************************************************/
int VmBaseNicPool::VmSelectCallback(void *nil, SqlColumn * columns)
{
    if ( NULL == columns || VM_LIMIT != columns->get_column_num())
    {
        return -1;
    }

    VmBaseNic  vmbase_nic;
    columns->GetValue(VmBaseNicPool::ID,              vmbase_nic._id);
    columns->GetValue(VmBaseNicPool::NIC_INDEX,       vmbase_nic._nic_index);    
    columns->GetValue(VmBaseNicPool::SRIOV,           vmbase_nic._sriov ); 
    columns->GetValue(VmBaseNicPool::LOOP,            vmbase_nic._loop); 
    columns->GetValue(VmBaseNicPool::LOGIC_NETWORK_ID,vmbase_nic._logic_network_id);
    columns->GetValue(VmBaseNicPool::MODEL,           vmbase_nic._model);    
    columns->GetValue(VmBaseNicPool::VSI_PROFILE_ID,  vmbase_nic._vsi_profile_id); 
    columns->GetValue(VmBaseNicPool::IPADDRESS,  vmbase_nic._ip); 
    columns->GetValue(VmBaseNicPool::NETMASK,  vmbase_nic._netmask); 
    columns->GetValue(VmBaseNicPool::GATEWAY,  vmbase_nic._gateway); 
    columns->GetValue(VmBaseNicPool::MAC,  vmbase_nic._mac); 	    
    vector<VmBaseNic>* p_vmbase_nics = static_cast<vector<VmBaseNic> *>(nil);
    p_vmbase_nics->push_back(vmbase_nic); 
    return 0;
}

/******************************************************************************/
int VmBaseNicPool::VtSelectCallback(void *nil, SqlColumn * columns)
{
    if ( NULL == columns || VMTEMPLATE_LIMIT != columns->get_column_num())
    {
        return -1;
    }

    VmBaseNic  vmbase_nic;
    columns->GetValue(VmBaseNicPool::ID,            vmbase_nic._id);
    columns->GetValue(VmBaseNicPool::NIC_INDEX,     vmbase_nic._nic_index);    
    columns->GetValue(VmBaseNicPool::SRIOV,           vmbase_nic._sriov ); 
    columns->GetValue(VmBaseNicPool::LOOP,            vmbase_nic._loop); 
    columns->GetValue(VmBaseNicPool::LOGIC_NETWORK_ID,vmbase_nic._logic_network_id);
    columns->GetValue(VmBaseNicPool::MODEL,         vmbase_nic._model);
    
    vector<VmBaseNic>* p_vmbase_nics = static_cast<vector<VmBaseNic> *>(nil);
    p_vmbase_nics->push_back(vmbase_nic);
    return 0;
}


/******************************************************************************/
int VmBaseNicPool::GetVmBaseNics(vector<VmBaseNic>& vmbase_nics, 
                                                 int64 id, 
                                                 bool is_vm)
{
    ostringstream   oss;
    int             rc;

    if (is_vm)
    {
       oss << "SELECT " << _col_names_vmcfg_nic << 
             " FROM " << _table_vmcfg_nic <<
             " WHERE vid = " << id;
       
        SetCallback(static_cast<Callbackable::Callback>(&VmBaseNicPool::VmSelectCallback),&vmbase_nics);
    }
    else
    {
       oss << "SELECT " << _col_names_vmtemplate_nic << 
             " FROM " << _table_vmtemplate_nic <<
             " WHERE vmtemplate_id = " << id;
        
        SetCallback(static_cast<Callbackable::Callback>(&VmBaseNicPool::VtSelectCallback),&vmbase_nics);     
    }

    rc = db->Execute(oss, this);
    UnsetCallback();
    if ( 0 != rc && SQLDB_RESULT_EMPTY != rc) 
    {
        SkyAssert(false);
    }
    return rc;
}

/******************************************************************************/
int VmBaseNicPool::SetVmBaseNics(vector<VmBaseNic>& vmbase_nics, 
                                                    int64 id, 
                                                    bool is_vm)
{
    int rc = -1;
    
    SqlCommand sql(db, SqlCommand::WITH_TRANSACTION);
    if (is_vm)
    {
        sql.SetTable(_table_vmcfg_nic);
        rc = sql.Delete(" WHERE vid = " + to_string(id,dec));
    }
    else
    {
        sql.SetTable(_table_vmtemplate_nic);
        rc = sql.Delete(" WHERE vmtemplate_id = " + to_string(id,dec));
    }    
    
    if ( (0 != rc) 
         &&(SQLDB_RESULT_EMPTY != rc) )
    {
        SkyAssert(false);
        return rc;
    }

    vector<VmBaseNic>::const_iterator  it;
    for(it = vmbase_nics.begin(); it != vmbase_nics.end(); it++)
    {
        sql.Clear();
        if (is_vm)
        {
            sql.Add("vid", id);
            sql.Add("nic_index",        it->_nic_index);
            sql.Add("sriov",            it->_sriov);
            sql.Add("loop",             it->_loop);        
            sql.Add("logic_network_id", it->_logic_network_id);
            sql.Add("model",            it->_model);
            sql.Add("vsi_profile_id",   it->_vsi_profile_id);
            sql.Add("ip_address",   it->_ip);
            sql.Add("netmask",   it->_netmask);
            sql.Add("gateway",   it->_gateway);
            sql.Add("mac",   it->_mac);
        }
        else
        {
            sql.Add("vmtemplate_id", id);
        sql.Add("nic_index",     it->_nic_index);
            sql.Add("sriov",            it->_sriov);
            sql.Add("loop",             it->_loop);        
            sql.Add("logic_network_id", it->_logic_network_id);
        sql.Add("model",         it->_model);
        }
         
        rc = sql.Insert();
        if(rc != 0)
        {
            SkyAssert(false);
            break;
        }
    }
    return rc;
}

/******************************************************************************/
int VmBaseNicPool::DropVmBaseNics(int64 id, bool is_vm)
{
    ostringstream oss;
    int rc;
    if (is_vm)
    {
       oss << "DELETE FROM " << _table_vmcfg_nic << " WHERE vid = " << id ;
    }
    else
    {
      oss << "DELETE FROM " << _table_vmtemplate_nic << " WHERE vmtemplate_id = " << id ;
    }
    
    if(Transaction::On())
    {
        rc = Transaction::Append(oss.str());
    }
    else
    {
        rc = db->Execute(oss);
    }
    return rc;
    
}


/******************************************************************************/
int VmBaseNicPool::InsertVmBaseNics(const vector<VmBaseNic>& vmbase_nics,
                                           int64  id,
                                           bool   is_vm)
{
    int rc = 0;

    SqlCommand sql(db, SqlCommand::WITH_TRANSACTION);
    if (is_vm)
    {
       sql.SetTable(VmBaseNicPool::_table_vmcfg_nic);
    }
    else
    {
       sql.SetTable(VmBaseNicPool::_table_vmtemplate_nic);
    }   
    
    vector<VmBaseNic>::const_iterator it;
    for(it = vmbase_nics.begin(); it != vmbase_nics.end(); it++)
    {
        sql.Clear();
        if (is_vm)
        {
          sql.Add("vid", id);
          sql.Add("nic_index",        it->_nic_index);
          sql.Add("sriov",            it->_sriov);        
          sql.Add("loop",             it->_loop);        
          sql.Add("logic_network_id", it->_logic_network_id);
          sql.Add("model",            it->_model);
          sql.Add("vsi_profile_id",   it->_vsi_profile_id);
          sql.Add("ip_address",   it->_ip);
          sql.Add("netmask",   it->_netmask);
          sql.Add("gateway",   it->_gateway);
          sql.Add("mac",   it->_mac);
        }
        else
        {
          sql.Add("vmtemplate_id", id);
          sql.Add("nic_index",     it->_nic_index);
          sql.Add("sriov",            it->_sriov);        
          sql.Add("loop",             it->_loop);        
          sql.Add("logic_network_id", it->_logic_network_id);
          sql.Add("model",         it->_model);
        }
        
        rc = sql.Insert();
        if(rc != 0)
        {
            SkyAssert(false);
            break;
        }
    }
    return rc;
}

int VmBaseNicPool::GetVmCfgNetworkCallback(void *nil, SqlColumn * columns)
{
    if ( NULL == columns || VM_LIMIT != columns->get_column_num())
    {
        return -1;
    }

    VmNicConfig  vm_nic;
    columns->GetValue(VmBaseNicPool::NIC_INDEX,       vm_nic._nic_index);    
    columns->GetValue(VmBaseNicPool::SRIOV,           vm_nic._sriov ); 
    columns->GetValue(VmBaseNicPool::LOOP,            vm_nic._loop); 
    columns->GetValue(VmBaseNicPool::LOGIC_NETWORK_ID,vm_nic._logic_network_id);
    columns->GetValue(VmBaseNicPool::MODEL,           vm_nic._model);    
    columns->GetValue(VmBaseNicPool::VSI_PROFILE_ID,  vm_nic._vsi_profile_id); 
    columns->GetValue(VmBaseNicPool::IPADDRESS,  vm_nic._ip); 
    columns->GetValue(VmBaseNicPool::NETMASK,  vm_nic._netmask); 
    columns->GetValue(VmBaseNicPool::GATEWAY,  vm_nic._gateway); 
    columns->GetValue(VmBaseNicPool::MAC,  vm_nic._mac); 
    vector<VmNicConfig>* p_vm_nics = static_cast<vector<VmNicConfig> *>(nil);
    p_vm_nics->push_back(vm_nic); 
    return 0;
}

int VmBaseNicPool::GetVmCfgNetwork(vector<VmNicConfig>& nic_info, int64 vid)
{
    ostringstream   oss;
    int             rc;

       oss << "SELECT " << _col_names_vmcfg_nic << 
             " FROM " << _table_vmcfg_nic <<
             " WHERE vid = " << vid;
   
    SetCallback(static_cast<Callbackable::Callback>(&VmBaseNicPool::GetVmCfgNetworkCallback),&nic_info);
    rc = db->Execute(oss, this);
    UnsetCallback();
    if ( 0 != rc && SQLDB_RESULT_EMPTY != rc) 
    {
        SkyAssert(false);
    }
    return rc;
}




const char * VmBaseContextPool::_table_vmcfg_context = 
        "vmcfg_context";  //虚拟机 上下文

const char * VmBaseContextPool::_table_vmtemplate_context = 
        "vmtemplate_context";  //模板 上下文
        
const char * VmBaseContextPool::_col_names_vmcfg_context = 
        "vid,"
        "file_name,"
        "file_content";
       
const char * VmBaseContextPool::_col_names_vmtemplate_context = 
        "vmtemplate_id,"
        "file_name,"
        "file_content";
       
	    
VmBaseContextPool *VmBaseContextPool::_instance = NULL;


/******************************************************************************/
int VmBaseContextPool::SelectCallback(void *nil, SqlColumn * columns)
{
    if ( NULL == columns || LIMIT != columns->get_column_num())
    {
        return -1;
    }

    string          file_name;
    string          file_content;
    
    VmBaseContext  *p_vmbase_contexts = static_cast<VmBaseContext *>(nil);
    
    columns->GetValue(VmBaseContextPool::ID,            p_vmbase_contexts->_id);
    columns->GetValue(VmBaseContextPool::FILE_NAME,     file_name);
    columns->GetValue(VmBaseContextPool::FILE_CONTENT,  file_content);

    p_vmbase_contexts->_str_map.insert(make_pair(file_name, file_content)); 
    
    return 0;
}

/******************************************************************************/
int VmBaseContextPool::GetVmBaseContext(VmBaseContext &vmbase_contexts, 
                                                 int64 id, 
                                                 bool is_vm)
{
    ostringstream   oss;
    int             rc;

    if (is_vm)
    {
       oss << "SELECT " << _col_names_vmcfg_context << 
             " FROM " << _table_vmcfg_context <<
             " WHERE vid = " << id;
    }
    else
    {
       oss << "SELECT " << _col_names_vmtemplate_context << 
             " FROM " << _table_vmtemplate_context <<
             " WHERE vmtemplate_id = " << id;
    }

    SetCallback(static_cast<Callbackable::Callback>(&VmBaseContextPool::SelectCallback),&vmbase_contexts);
    rc = db->Execute(oss, this);
    UnsetCallback();
    if ( 0 != rc && SQLDB_RESULT_EMPTY != rc) 
    {
        SkyAssert(false);
    }
    return rc;
}

/******************************************************************************/
int VmBaseContextPool::SetVmBaseContext(VmBaseContext  &vmbase_contexts, 
                                                    int64 id, 
                                                    bool is_vm)
{
    int rc = -1;
    
    SqlCommand sql(db, SqlCommand::WITH_TRANSACTION);
    if (is_vm)
    {
        sql.SetTable(_table_vmcfg_context);
        rc = sql.Delete(" WHERE vid = " + to_string(id,dec));
    }
    else
    {
        sql.SetTable(_table_vmtemplate_context);
        rc = sql.Delete(" WHERE vmtemplate_id = " + to_string(id,dec));
    }    
    
    if ( (0 != rc) 
         &&(SQLDB_RESULT_EMPTY != rc) )
    {
        SkyAssert(false);
        return rc;
    }

    map<string, string>::iterator  map_it;
    for (map_it  = vmbase_contexts._str_map.begin(); 
         map_it != vmbase_contexts._str_map.end();
         map_it++ )
    {
        sql.Clear();
        if (is_vm)
        {
            sql.Add("vid", id);
        }
        else
        {
            sql.Add("vmtemplate_id", id);
        }
    
        sql.Add("file_name",     map_it->first);
        sql.Add("file_content",  map_it->second);

        rc = sql.Insert();
        if(rc != 0)
        {
            SkyAssert(false);
            break;
        }
    }
    
    return rc;
}

/******************************************************************************/
int VmBaseContextPool::DropVmBaseContext(int64 id, bool is_vm)
{
    int rc =1;
    ostringstream oss;
    if (is_vm)
    {
       oss << "DELETE FROM " << _table_vmcfg_context << " WHERE vid = " << id ;
    }
    else
    {
      oss << "DELETE FROM " << _table_vmtemplate_context << " WHERE vmtemplate_id = " << id ;
    }
    
    if(Transaction::On())
    {
        rc = Transaction::Append(oss.str());
    }
    else
    {
        rc = db->Execute(oss);
    }
    return rc;
    
}


/******************************************************************************/
int VmBaseContextPool::InsertVmBaseContext(VmBaseContext  &vmbase_contexts,
                                                   int64  id,
                                                   bool   is_vm)
{
    int rc = 0;

    SqlCommand    sql(db, SqlCommand::WITH_TRANSACTION);
    if (is_vm)
    {
       sql.SetTable(VmBaseContextPool::_table_vmcfg_context);
    }
    else
    {
       sql.SetTable(VmBaseContextPool::_table_vmtemplate_context);
    }   
    
    map<string, string>::iterator  map_it;
    for (map_it  = vmbase_contexts._str_map.begin(); 
         map_it != vmbase_contexts._str_map.end();
         map_it++ )
    {
        sql.Clear();
        if (is_vm)
        {
            sql.Add("vid", id);
        }
        else
        {
            sql.Add("vmtemplate_id", id);
        }
    
        sql.Add("file_name",     map_it->first);
        sql.Add("file_content",  map_it->second);

        rc = sql.Insert();
        if(rc != 0)
        {
            SkyAssert(false);
            break;
        }
    }
    
    return rc;
}


/******************************************************************************/
const char * VmBaseDevicePool::_cfg_table_name = 
        "vmcfg_device";  //虚拟机 上下文

const char * VmBaseDevicePool::_template_table_name = 
        "vmtemplate_device";  //模板 上下文
        
const char * VmBaseDevicePool::_col_names = 
        "vid,"
        "type,"
        "count";
       
const char * VmBaseDevicePool::_template_col_names = 
        "vmtemplate_id,"
        "type,"
        "count";

VmBaseDevicePool *VmBaseDevicePool::_instance = NULL;

/******************************************************************************/
int VmBaseDevicePool::SelectCallback(void *nil, SqlColumn * columns)
{
    if ( NULL == columns || LIMIT != columns->get_column_num())
    {
        return -1;
    }
    
    VmDeviceConfig device;
    //columns->GetValue(VmBaseDevicePool::ID,      device._id);
    columns->GetValue(VmBaseDevicePool::TYPE,    (int&)device._type);
    columns->GetValue(VmBaseDevicePool::COUNT,   (int&)device._count);

    vector<VmDeviceConfig>  *p  = static_cast<vector<VmDeviceConfig> *>(nil);
    p->push_back(device);
    
    return 0;
}

/******************************************************************************/
int VmBaseDevicePool::GetVmDevices(vector<VmDeviceConfig> &devices, 
                                                 int64 id, 
                                                 bool is_vm)
{
    ostringstream   oss;
    int             rc;

    if (is_vm)
    {
       oss << "SELECT " << _col_names << 
             " FROM " << _cfg_table_name <<
             " WHERE vid = " << id;
    }
    else
    {
       oss << "SELECT " << _template_col_names << 
             " FROM " << _template_table_name <<
             " WHERE vmtemplate_id = " << id;
    }

    SetCallback(static_cast<Callbackable::Callback>(&VmBaseDevicePool::SelectCallback),&devices);
    rc = db->Execute(oss, this);
    UnsetCallback();
    if ( 0 != rc && SQLDB_RESULT_EMPTY != rc) 
    {
        SkyAssert(false);
    }
    return rc;
}

/******************************************************************************/
int VmBaseDevicePool::SetVmDevices(const vector<VmDeviceConfig> &devices, int64 id, bool is_vm)
{
    int rc = DropVmDevices(id, is_vm);
    
    if ( (0 != rc) 
         &&(SQLDB_RESULT_EMPTY != rc) )
    {
        SkyAssert(false);
        return rc;
    }

    rc = InsertVmDevices(devices, id, is_vm);
    return rc;
}

/******************************************************************************/
int VmBaseDevicePool::DropVmDevices(int64 id, bool is_vm)
{
    int rc =1;
    ostringstream oss;
    if (is_vm)
    {
        oss << "DELETE FROM " << _cfg_table_name << " WHERE vid = " << id ;
    }
    else
    {
        oss << "DELETE FROM " << _template_table_name << " WHERE vmtemplate_id = " << id ;
    }
    
    if(Transaction::On())
    {
        rc = Transaction::Append(oss.str());
    }
    else
    {
        rc = db->Execute(oss);
    }
    return rc;
    
}


/******************************************************************************/
int VmBaseDevicePool::InsertVmDevices(const vector<VmDeviceConfig> &devices, int64 id, bool is_vm)
{
    int rc = 0;

    SqlCommand    sql(db, SqlCommand::WITH_TRANSACTION);
    if (is_vm)
    {
       sql.SetTable(VmBaseDevicePool::_cfg_table_name);
    }
    else
    {
       sql.SetTable(VmBaseDevicePool::_template_table_name);
    }   
    
    vector<VmDeviceConfig>::const_iterator  it;
    for (it  = devices.begin(); 
         it != devices.end();
         it++ )
    {
        sql.Clear();
        if (is_vm)
        {
            sql.Add("vid", id);
        }
        else
        {
            sql.Add("vmtemplate_id", id);
        }
    
        sql.Add("type",     it->_type);
        sql.Add("count",    it->_count);

        rc = sql.Insert();
        if(rc != 0)
        {
            SkyAssert(false);
            break;
        }
    }
    
    return rc;
}

}
	


