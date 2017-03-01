/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：vm_image.h
* 文件标识：见配置管理计划书
* 内容摘要：虚拟机磁盘、映像配置信息管理模块
* 当前版本：1.0
* 作    者：姚远
* 完成日期：2012年4月9日
*
* 修改记录1：
*     修改日期：2012/04/09
*     版 本 号：V1.0
*     修 改 人：姚远
*     修改内容：创建
*******************************************************************************/
#include "vm_image.h"
#include "db_config.h"
#include "vstorage_manager.h"

namespace zte_tecs
{
VMImagePool* VMImagePool::instance = NULL;

const char *VMImagePool::table_image = "vm_image";
const char *VMImagePool::table_image_cancel = "vm_image_cancel";
const char *VMImagePool::col_names_image = "vid,target,image_id,occupied_size,position,reserved_backup";
const char *VMImagePool::col_names_image_cancel = "vid,hid,target,image_id,occupied_size,position";

const char *VMImagePool::table_disk= "vm_disk";
const char *VMImagePool::table_disk_cancel = "vm_disk_cancel";
const char *VMImagePool::col_names_disk = "vid,target,size,position,request_id,access_url,is_need_release";
const char *VMImagePool::col_names_disk_cancel = "vid,hid,target,occupied_size,position";

const char *VMImagePool::table_image_backup= "vm_image_backup";
const char *VMImagePool::col_names_image_backup = "request_id,vid,target,state,parent_request_id,create_time,description";

/**********************************************************************
* 函数名称：VMImagePool::InsertVMImageConfig
* 功能描述：将虚拟机的映像配置过滤整理，放置到vector中
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期     版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2012/04/09   V1.1    姚远        创建
************************************************************************/
vector<VmDiskConfig> VMImagePool::FilterImager(const VMConfig &vmcfg)
{
    vector<VmDiskConfig> images(vmcfg._disks);

    if (vmcfg._machine._id != INVALID_FILEID)
    {
        images.push_back(vmcfg._machine);
    }
    else
    {
        images.push_back(vmcfg._kernel);
        images.push_back(vmcfg._initrd);
    }

    return images;
};


/**********************************************************************
* 函数名称：VMImagePool::InsertVMImageConfig
* 功能描述：将新加入集群的虚拟机映像配置信息插入到表中
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期     版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2012/04/09   V1.1    姚远        创建
************************************************************************/
int VMImagePool::InsertVMImageConfig(int64 vid, const VMConfig &vmcfg)
{
    vector<VmDiskConfig> images = FilterImager(vmcfg);
    if(images.empty())
    {
        return 0;
    }

    int ret = -1;
    ostringstream oss;
    vector<VmDiskConfig>::const_iterator it;
    for(it=images.begin();it!=images.end();it++)
    {
        if(it->_id == INVALID_FILEID)
        {
            InsertDisk(vid, *it);
            continue;
        }
        
        int64  image_base_size = 0 ;
        image_base_size = CalcDiskRealSize(*it);
        
        const char *table;
        string table_size;
        if(it->_id != INVALID_FILEID)
        {
            table = table_image;
            table_size = "occupied_size";
        }
        else
        {
            table = table_disk;            
            table_size = "size";
        }
        
        SqlCommand sql(db, table);
        sql.Add("vid", vid);
        sql.Add("target", it->_target);

        if(it->_id != INVALID_FILEID)
            sql.Add("image_id", it->_id);

        sql.Add(table_size, image_base_size);
        sql.Add("position", it->_position);
        sql.Add("reserved_backup", it->_reserved_backup);

        oss.str("");
        oss << sql.InsertSql();
        
        if(Transaction::On())
        {
            ret = Transaction::Append(oss.str());
        }
        else
        {
            ret = db->Execute(oss);
        }

        if(ret != 0)
        {
            cerr << oss.str() << endl;
            SkyAssert(false);
            return ret;
        }
    }

    return ret;
}

int VMImagePool::UpdateVMImageConfig(int64 vid, const VMConfig &vmcfg)
{    
    int ret = -1;
    ostringstream oss;

    /* 第一步先把vm的所有image删除 */
    oss << "DELETE FROM " << table_image << " WHERE vid = " << vid;
    oss << " ;DELETE FROM " << table_disk << " WHERE vid = " << vid;
    if(Transaction::On())
    {
        ret = Transaction::Append(oss.str());
    }
    else
    {
        ret = db->Execute(oss);
    }

    if(ret != 0)
    {
        cerr << oss.str() << endl;
        SkyAssert(false);
        return ret;
    }

    return InsertVMImageConfig(vid, vmcfg);
}

int VMImagePool::DeleteVmImage(int64 vid, const VmDiskConfig &disk)
{
    int ret = -1;
    ostringstream oss;

    oss << "DELETE FROM " << table_image << " WHERE vid = " << vid
                                        << " and target = '" << disk._target << "'";

    if(Transaction::On())
    {
        ret = Transaction::Append(oss.str());
    }
    else
    {
        ret = db->Execute(oss);
    }

    if(ret != 0)
    {
        cerr << oss.str() << endl;
        SkyAssert(false);
    }

    return ret;
}

int VMImagePool::InsertVmImage(int64 vid, const VmDiskConfig &disk)
{
    int ret = -1;
    ostringstream oss;

    SkyAssert(disk._id != INVALID_FILEID);

    SqlCommand sql(db, table_image);
    sql.Add("vid", vid);
    sql.Add("target", disk._target);

    sql.Add("image_id", disk._id);

    sql.Add("occupied_size", CalcDiskRealSize(disk));
    sql.Add("position", disk._position);
    sql.Add("reserved_backup", disk._reserved_backup);

    oss.str("");
    oss << sql.InsertSql();
    
    if(Transaction::On())
    {
        ret = Transaction::Append(oss.str());
    }
    else
    {
        ret = db->Execute(oss);
    }

    if(ret != 0)
    {
        cerr << oss.str() << endl;
        SkyAssert(false);
        return ret;
    }

    return ret;
}

int VMImagePool::GetHostUsingImageCallback(void *nil, SqlColumn *columns)
{
    Host_using_Image h_image;
    vector<Host_using_Image>* vec;
    if(!nil)
    {
        return 0;
    }

    if ((NULL == columns) || (2 != columns->get_column_num()))
    {
        return -1;
    }

    vec = static_cast<vector<Host_using_Image> *>(nil);

    columns->GetValue(0, h_image._hid);
    columns->GetValue(1, h_image._image_id);
    
    vec->push_back(h_image);

    return 0;
}

int VMImagePool::GetHostUsingImage(vector<Host_using_Image> &vec)
{
    ostringstream sql;
    
    SetCallback(static_cast<Callbackable::Callback>(&VMImagePool::GetHostUsingImageCallback),(void *)&vec);
    sql << "SELECT hid, image_id FROM host_using_image ";
    int ret = db->Execute(sql, this);
    UnsetCallback();
    return ret;
}

int VMImagePool::InsertDisk(int64 vid, const VmDiskConfig &disk)
{
    int ret = -1;
    ostringstream oss;

    int64  image_base_size = 0 ;
    image_base_size = CalcDiskRealSize(disk);
    
    SqlCommand sql(db, table_disk);
    sql.Add("vid", vid);
    sql.Add("target", disk._target);

    sql.Add("size", image_base_size);
    sql.Add("position", disk._position);
    sql.Add("request_id", disk._request_id);
    sql.Add("access_url", disk._share_url);
    sql.Add("is_need_release", disk._is_need_release);

    oss.str("");
    oss << sql.InsertSql();
    
    if(Transaction::On())
    {
        ret = Transaction::Append(oss.str());
    }
    else
    {
        ret = db->Execute(oss);
    }

    if(ret != 0)
    {
        cerr << oss.str() << endl;
        SkyAssert(false);
        return ret;
    }

    return ret;
}

int VMImagePool::DeleteDisk(int64 vid, const VmDiskConfig &disk)
{
    int ret = -1;

    SqlCommand sql(db, table_disk, SqlCommand::WITH_TRANSACTION);

    ret = sql.Delete(" WHERE (vid = " + to_string<int64>(vid,dec)
                   +") and (target = '" + disk._target + "')");
    if(ret != 0)
    {
        SkyAssert(false);
        return ret;
    }

    return ret;
}

int VMImagePool::UpdateDisk(int64 vid, const VmDiskConfig &disk)
{
    int ret = -1;
    ostringstream oss;

    int64  image_base_size = 0 ;
    image_base_size = CalcDiskRealSize(disk);
    
    SqlCommand sql(db, table_disk);
    sql.Add("vid", vid);
    sql.Add("target", disk._target);
    sql.Add("size", image_base_size);
    sql.Add("position", disk._position);
    sql.Add("request_id", disk._request_id);
    sql.Add("access_url", disk._share_url);
    sql.Add("is_need_release", disk._is_need_release);

    oss.str("");
    oss << sql.UpdateSql()<<" WHERE vid = " << vid << " and target = '" << disk._target << "'";
    
    if(Transaction::On())
    {
        ret = Transaction::Append(oss.str());
    }
    else
    {
        ret = db->Execute(oss);
    }

    if(ret != 0)
    {
        cerr << oss.str() << endl;
        SkyAssert(false);
        return ret;
    }

    return ret;
}

int VMImagePool::UpdateImage(int64 vid, const VmDiskConfig &disk)
{
    int ret = -1;
    ostringstream oss;

    int64  image_base_size = 0 ;
    image_base_size = CalcDiskRealSize(disk);
    
    SqlCommand sql(db, table_image);
    sql.Add("vid", vid);
    sql.Add("target", disk._target);
    sql.Add("image_id", disk._id);
    sql.Add("occupied_size", image_base_size);
    sql.Add("position", disk._position);
    sql.Add("request_id", disk._request_id);
    sql.Add("access_url", disk._share_url);
    sql.Add("reserved_backup", disk._reserved_backup);

    oss.str("");
    oss << sql.UpdateSql()<<" WHERE vid = " << vid << " and target = '" << disk._target << "'";
    
    if(Transaction::On())
    {
        ret = Transaction::Append(oss.str());
    }
    else
    {
        ret = db->Execute(oss);
    }

    if(ret != 0)
    {
        cerr << oss.str() << endl;
        SkyAssert(false);
        return ret;
    }

    return ret;
}

int VMImagePool::GetImageRsvNumByVid(int64 vid,int &num)
{
    int ret;
    ostringstream where;
    where << "vid = " << vid;    
    SqlCallbackable sqlcb(db); 
    int64   tmp_oid = INVALID_OID; 
    ret=sqlcb.SelectColumn(table_image,"reserved_backup",where.str(),tmp_oid );
    if (SQLDB_RESULT_EMPTY == ret)
    {
        return ERROR_OBJECT_NOT_EXIST;
    }

    if (SQLDB_OK != ret)
    {
        SkyAssert(false);
        return ERROR_DB_SELECT_FAIL;
    }    
    num=tmp_oid;
    return SUCCESS;
}

int VMImagePool::SelectShareDisk(void *nil, SqlColumn *columns)
{
    if ( NULL == columns || LIMIT != columns->get_column_num())
    {
        return -1;
    }

    vector<VmDiskConfig> *p_vector;

    p_vector = static_cast<vector<VmDiskConfig> *>(nil);

    VmDiskConfig disk;
    columns->GetValue(TARGET , disk._target);
    columns->GetValue(SIZE , disk._size);
    columns->GetValue(POSITION , disk._position);
    columns->GetValue(REQUEST_ID , disk._request_id);
    columns->GetValue(ACCESS_URL , disk._share_url);
    columns->GetValue(IS_NEED_RELEASE , disk._is_need_release);

    p_vector->push_back(disk);

    return 0;
}

int VMImagePool::SelectImageCallback(void *nil, SqlColumn *columns)
{
    if ( NULL == columns || 3 != columns->get_column_num())
    {
        return -1;
    }

    vector<VmDiskConfig> *p_vector;

    p_vector = static_cast<vector<VmDiskConfig> *>(nil);

    VmDiskConfig disk;
    
    columns->GetValue(1 , disk._target);
    columns->GetValue(2 , disk._id);
    
    p_vector->push_back(disk);

    return 0;
}

//删除的时候要从VM_DISK里去配置数据
//根据request_id查询
int VMImagePool::GetDisk(int64 vid, vector<VmDiskConfig> &disks)
{
    ostringstream oss;
    int rc;

    SetCallback(static_cast<Callbackable::Callback>(&VMImagePool::SelectShareDisk),
                static_cast<void *> (&disks));

    oss << "SELECT " << col_names_disk << " FROM " << table_disk << " WHERE vid = " << vid;

    rc = db->Execute(oss, this);
    UnsetCallback();

    if ( (SQLDB_OK != rc) && (SQLDB_RESULT_EMPTY != rc)  )
    {
        //数据库系统出错
        SkyAssert(false);
        return ERROR_DB_SELECT_FAIL;
    } 
  
    return SUCCESS;
}
int VMImagePool::GetDisk(const string& request_id, VmDiskConfig& disk)
{
    vector<VmDiskConfig> disks;
    ostringstream oss;
    int rc;

    SetCallback(static_cast<Callbackable::Callback>(&VMImagePool::SelectShareDisk),
                static_cast<void *> (&disks));

    oss << "SELECT " << col_names_disk << " FROM " << table_disk << " WHERE request_id = '" << request_id + "'";

    rc = db->Execute(oss, this);
    UnsetCallback();

    if ( (SQLDB_OK != rc) && (SQLDB_RESULT_EMPTY != rc)  )
    {
        //数据库系统出错
        SkyAssert(false);
        return ERROR_DB_SELECT_FAIL;
    } 

    if (0 != disks.size())
    {
        disk = disks[0];
        return SUCCESS;
    }
    else
    {
        return ERROR_OBJECT_NOT_EXIST;
    }
}
bool VMImagePool::IsDiskExist(const string& request_id)
{
    vector<VmDiskConfig> disks;
    ostringstream oss;
    int rc;

    SetCallback(static_cast<Callbackable::Callback>(&VMImagePool::SelectShareDisk),
                static_cast<void *> (&disks));

    oss << "SELECT " << col_names_disk << " FROM " << table_disk << " WHERE request_id = '" << request_id + "'";

    rc = db->Execute(oss, this);
    UnsetCallback();

    if ( (SQLDB_OK != rc) && (SQLDB_RESULT_EMPTY != rc)  )
    {
        //数据库系统出错
        SkyAssert(false);
        return false;
    } 

    return (0 != disks.size());
}

bool VMImagePool::IsDiskExist(const string& request_id,int vid)
{
    vector<VmDiskConfig> disks;
    ostringstream oss;
    SetCallback(static_cast<Callbackable::Callback>(&VMImagePool::SelectShareDisk),
                static_cast<void *> (&disks));

    oss << "SELECT " << col_names_disk << " FROM " << table_disk << " WHERE request_id = '" << request_id + "' and vid = "<<vid;
    int rc = db->Execute(oss, this);
    UnsetCallback();

    if ( (SQLDB_OK != rc) && (SQLDB_RESULT_EMPTY != rc)  )
    {
        SkyAssert(false);
        return false;
    } 
    return (0 != disks.size());
}

int VMImagePool::GetDiskTarget(const string& request_id,int vid,string &target)
{
    ostringstream where;
    where << "request_id = '" << request_id <<"' and vid ="<<vid;
    STATUS ret = SelectColumn(table_disk,"target",where.str(),target);

    if (SQLDB_RESULT_EMPTY == ret)
    {
        return ERROR_OBJECT_NOT_EXIST;
    }
    else if (SQLDB_OK != ret)
    {
        return ERROR_DB_SELECT_FAIL;
    }
    else
    {
        return SUCCESS;
    }
}

int VMImagePool::GetImageBackupState(const string & request_id, const string & target,int vid, int & state)
{
    int backupstate = -1;
    STATUS ret = ERROR;
    ostringstream where;

    where << "request_id = '" << request_id << "' and target = '" <<target <<"' and vid ="<<vid;
    ret = SelectColumn(table_image_backup,"state",where.str(),backupstate);


    if (SQLDB_RESULT_EMPTY == ret)
    {
        return ERROR_OBJECT_NOT_EXIST;
    }
    else if (SQLDB_OK != ret)
    {
        return ERROR_DB_SELECT_FAIL;
    }
    else
    {
        state = backupstate;
        return SUCCESS;
    }
}

int VMImagePool::SetImageBackupState(const string & target,int64 vid)
{
    int ret = -1;
    ostringstream oss;
    SqlCommand sql(db, table_image_backup);

    sql.Add("state", IMG_BACKUP_BACKED);
    oss.str("");
    oss << sql.UpdateSql()<<" WHERE target ='" << target << "' and vid = " << vid << " and state = " << IMG_BACKUP_ACTIVE<< "";
    if(Transaction::On())
    {
        ret = Transaction::Append(oss.str());
    }
    else
    {
        ret = db->Execute(oss);
    }

    if(ret != 0)
    {
        SkyAssert(false);
        return ret;
    }
    return ret;
}

int VMImagePool::UpdateImageBackup(const string &request_id, const string &description)
{
    int ret = -1;
    ostringstream oss;

    SqlCommand sql(db, table_image_backup);
    sql.Add("description", description);
    
    oss.str("");
    oss << sql.UpdateSql()<<" WHERE request_id = '" << request_id << "'";
    
    if(Transaction::On())
    {
        ret = Transaction::Append(oss.str());
    }
    else
    {
        ret = db->Execute(oss);
    }

    if(ret != 0)
    {
        SkyAssert(false);
        return ret;
    }

    return ret;
}

int VMImagePool::UpdateImageBackup(const string &request_id, const string & target,int64 vid,int state)
{
    int ret = -1;

    if(false == IsImageBackupExist(request_id))
    {
        return ret;
    }

    if(IMG_BACKUP_ACTIVE == state)
    {
        ret = SetImageBackupState(target,vid);
        if(ret != 0)
        {
            SkyAssert(false);
            return ret;
        }
    }

    ret = UpdateImageBackup(request_id,state);
    return ret;
}

int VMImagePool::InsertImageBackup(VMImgBackup &imgbackup)
{
    int ret = -1;
    ostringstream oss;
   
    
    SqlCommand sql(db, table_image_backup);
    sql.Add("request_id", imgbackup._request_id);
    sql.Add("vid", imgbackup._vid);
    sql.Add("target", imgbackup._target);
    sql.Add("state", imgbackup._state);
    sql.Add("parent_request_id", imgbackup._parent_request_id);
    sql.Add("create_time", imgbackup._create_time);
    sql.Add("description", imgbackup._description);

    oss.str("");
    oss << sql.InsertSql();
    
    if(Transaction::On())
    {
        ret = Transaction::Append(oss.str());
    }
    else
    {
        ret = db->Execute(oss);
    }

    if(ret != 0)
    {
        cerr << oss.str() << endl;
        SkyAssert(false);
        return ret;
    }

    return ret;
}

int VMImagePool::DeleteImageBackup(const string& request_id)
{
    int ret = -1;
    ostringstream where, where_p;
    string parent_request_id;
    vector<string> son_request_id;
    bool   isTheLast=FALSE;
    ostringstream oss;

    where << "request_id = '" << request_id << "'";    
    SqlCallbackable sqlcb(db); 
    ret=sqlcb.SelectColumn(table_image_backup,"parent_request_id",where.str(),parent_request_id );
    if (SQLDB_RESULT_EMPTY == ret)
    {
        return ERROR_OBJECT_NOT_EXIST;
    }

    if (SQLDB_OK != ret)
    {
        SkyAssert(false);
        return ERROR_DB_SELECT_FAIL;
    }
    
    where_p << "parent_request_id = '" << request_id << "'";    
    ret=sqlcb.SelectColumn(table_image_backup,"request_id",where_p.str(),son_request_id );
    if (SQLDB_RESULT_EMPTY == ret)
    {
        isTheLast = TRUE;
    }
    else if (SQLDB_OK != ret)
    {
        SkyAssert(false);
        return ERROR_DB_SELECT_FAIL;
    }    

    SqlCommand sql(db, table_image_backup);

    if(!isTheLast)
    {
        vector<string>::const_iterator it = son_request_id.begin();
        string set = "('" + *it;
        it++;
        while (it != son_request_id.end())
        {
            set += "','";
            set += *it;
            it++;
        }
        set += "')";
        
        sql.Add("parent_request_id", parent_request_id);
    
        oss.str("");
        oss << sql.UpdateSql()<<" WHERE request_id in " << set;
    
        if(Transaction::On())
        {
            ret = Transaction::Append(oss.str());
        }
        else
        {
            ret = db->Execute(oss);
        }
    
        if(ret != 0)
        {
            SkyAssert(false);
            return ret;
        }       
    }
	
    ret = sql.Delete(" WHERE request_id = '" + request_id + "'");
    if(ret != 0)
    {
        SkyAssert(false);
        return ret;
    }

    return ret;
}

int VMImagePool::GetSonImageBackup(const string& request_id, vector<string>& son_request_id)
{ 
    int ret = -1;
    ostringstream where;
    
    where << "parent_request_id = '" << request_id <<"'";    
    SqlCallbackable sqlcb(db); 
    ret=sqlcb.SelectColumn(table_image_backup,"request_id",where.str(),son_request_id);
    if (SQLDB_RESULT_EMPTY == ret)
    {
        return ERROR_OBJECT_NOT_EXIST;
    }

    if (SQLDB_OK != ret)
    {
        SkyAssert(false);
        return ERROR_DB_SELECT_FAIL;
    }
    return ret;
}

int VMImagePool::GetImageBackupTarget(const string& request_id,string & target)
{  
    int ret;
    ostringstream where;
    where <<" request_id ='" << request_id << "'";
    SqlCallbackable sqlcb(db); 
    ret=sqlcb.SelectColumn(table_image_backup,"target",where.str(),target );
    if (SQLDB_RESULT_EMPTY == ret)
    {
        return ERROR_OBJECT_NOT_EXIST;
    }

    if (SQLDB_OK != ret)
    {
        return ERROR_DB_SELECT_FAIL;
    }
    
    return SUCCESS;
   
}

int VMImagePool::DeleteImageBackupByVid(int64 vid,string & target)
{
    int rc = -1;
    SqlCommand sql(_db, table_image_backup);

    rc = sql.Delete(" WHERE (vid = " + to_string<int64>(vid,dec) +") and (target = '" + target+ "')");
    if (0 != rc && SQLDB_RESULT_EMPTY != rc)
    {
        return ERROR_DB_DELETE_FAIL;
    }
    return SUCCESS;
}

int VMImagePool::UpdateImageBackup(const string& request_id,int state)
{  
    int ret = -1;
    ostringstream oss;
    
    SqlCommand sql(db, table_image_backup);
    sql.Add("state", state);  

    oss.str("");
    oss << sql.UpdateSql()<<" WHERE request_id = '" << request_id << "' " ;
    
    if(Transaction::On())
    {
        ret = Transaction::Append(oss.str());
    }
    else
    {
        ret = db->Execute(oss);
    }

    if(ret != 0)
    {
        cerr << oss.str() << endl;
        SkyAssert(false);
        return ret;
    }

    return ret;
    
}

int VMImagePool::GetActiveImageBackup(string& request_id,const string & target,int64 vid)
{
    
    int ret;
    ostringstream where;
    where << "state = " << IMG_BACKUP_ACTIVE <<"and target ='" << target << "' and vid = " << vid ;
    SqlCallbackable sqlcb(db); 
    ret=sqlcb.SelectColumn(table_image_backup,"request_id",where.str(),request_id );
    if (SQLDB_RESULT_EMPTY == ret)
    {
        return ERROR_OBJECT_NOT_EXIST;
    }

    if (SQLDB_OK != ret)
    {
        return ERROR_DB_SELECT_FAIL;
    }
    
    return SUCCESS;
   
}

bool VMImagePool::IsImageBackupExist(const string& request_id)
{
    int ret;
    ostringstream where;
    where << "request_id = '" << request_id <<"'";
    SqlCallbackable sqlcb(db); 
    int64 vid;
    ret=sqlcb.SelectColumn(table_image_backup,"vid",where.str(),vid );
    if (SQLDB_RESULT_EMPTY == ret)
    {
        return false;
    }

    if (SQLDB_OK != ret)
    {
        SkyAssert(false);
        return false;
    }    
    return true;
}

bool VMImagePool::IsBaseImageBackup(string& request_id,const string & target,int64 vid)
{
    //1 不存在该项
    if(true == IsImageBackupExist(request_id))
    {
        return false;
    }

    //2 只存在于作为父request_id中
    int ret;
    ostringstream where;
    where << "parent_request_id = '" << request_id <<"' and target ='" << target << "' and vid = " << vid;
    SqlCallbackable sqlcb(db); 
    int64 tmp_vid;
    ret=sqlcb.SelectColumn(table_image_backup,"vid",where.str(),tmp_vid );
    if (SQLDB_OK == ret)
    {
        return true;
    }
   
    return false;
}

int VMImagePool::GetImageBackupNumByVid(int64 vid,int &num)
{
    int ret;
    ostringstream where;
    where << "vid = " << vid;
    string  column  = " COUNT(request_id) ";
    SqlCallbackable sqlcb(db); 
    int64   tmp_oid = INVALID_OID; 
    ret=sqlcb.SelectColumn(table_image_backup,column,where.str(),tmp_oid );
    if (SQLDB_RESULT_EMPTY == ret)
    {
        return ERROR_OBJECT_NOT_EXIST;
    }

    if (SQLDB_OK != ret)
    {
        SkyAssert(false);
        return ERROR_DB_SELECT_FAIL;
    }    
    num=tmp_oid;
    return SUCCESS;
}
bool VMImagePool::IsDiskExist(int64 vid, const string& target)
{
    vector<VmDiskConfig> disks;
    ostringstream oss;
    int rc;

    SetCallback(static_cast<Callbackable::Callback>(&VMImagePool::SelectShareDisk),
                static_cast<void *> (&disks));

    oss << "SELECT " << col_names_disk << " FROM " << table_disk 
        << " WHERE vid = " << vid << " and target = '" << target << "'" ;

    rc = db->Execute(oss, this);
    UnsetCallback();

    if ( (SQLDB_OK != rc) && (SQLDB_RESULT_EMPTY != rc)  )
    {
        //数据库系统出错
        SkyAssert(false);
        return false;
    } 

    return (0 != disks.size());
}

bool VMImagePool::IsImageExist(int64 vid, const string& target)
{
    vector<VmDiskConfig> disks;
    ostringstream oss;
    int rc;

    SetCallback(static_cast<Callbackable::Callback>(&VMImagePool::SelectImageCallback),
                static_cast<void *> (&disks));

    oss << "SELECT vid,target,image_id FROM " << table_image 
        << " WHERE vid = " << vid << " and target = '" << target << "'" ;

    rc = db->Execute(oss, this);
    UnsetCallback();

    if ( (SQLDB_OK != rc) && (SQLDB_RESULT_EMPTY != rc)  )
    {
        //数据库系统出错
        SkyAssert(false);
        return false;
    } 

    return (0 != disks.size());
}

}

