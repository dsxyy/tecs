/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：image_pool.cpp
* 文件标识：见配置管理计划书
* 内容摘要：Image类的实现文件
* 当前版本：2.0 
* 作    者：lichun
* 完成日期：2012年10月19日
*
* 修改记录1：
*    修改日期：2012/10/19
*    版 本 号：V2.0
*    修 改 人：lichun
*    修改内容：将image_pool改成非缓存模式
*******************************************************************************/
#include "image_pool.h"
#include "image.h"
#include "log_agent.h"
#include "tecs_errcode.h"

static int ip_print = 1;  
DEFINE_DEBUG_VAR(ip_print);
#define Dbg_Prn(fmt,arg...) \
        do \
        { \
            if(ip_print) \
            { \
                printf("[%s:%d]"fmt,__func__,__LINE__, ##arg); \
            } \
        }while(0)

namespace zte_tecs
{
ImagePool *ImagePool::instance = NULL;

const char * ImagePool::image_table = "image_pool";
const char * ImagePool::image_all_info = "image_all_info";
const char * ImagePool::image_extended_db_names = "image_id, "
                                                  "name,"
                                                  "uid,"
                                                  "type,"
                                                  "bus,"
                                                  "size,"
                                                  "is_public, "
                                                  "enabled,"
                                                  "location, "
                                                  "fileurl, "
                                                  "disk_format, "
                                                  "container_format, "
                                                  "os_type,"
                                                  "os_version,"
                                                  "os_arch,"
                                                  "register_time,"
                                                  "description,"
                                                  "running_vms,"
                                                  "ref_count_by_private_vt,"
                                                  "ref_count_by_public_vt,"
                                                  "disk_size";
const char* ImagePool::_view_share_disk_info = "storage_cluster_max_free_size_view";
const char* ImagePool::_view_image_other_user = "view_image_other_user";

STATUS ImagePool::Init()
{
    int64 imageId = 0;
    /*初始化锁*/
    _image_pool_mutex.Init();
    
    /*获取image_id最大值，后续allocate函数会用到*/
    int ret = SelectColumn(image_table,"MAX(image_id)","",imageId);

    if(ret != SUCCESS)
    {
        return ERROR;
    }

    _last_image_id = imageId;

    return SUCCESS;
}

/******************************************************************************/
int64 ImagePool::GetImageCountByUid(int64 uid)
{
    ostringstream where;

    where<<"uid = "<<uid;

    return GetImageCountByWhere(where.str());
}

int64 ImagePool::GetImageCountByWhere(const string &where)
{
    int64 count = -1;

    int ret = SelectColumn(image_table,"count(*)",where,count);
     
    if(ret != 0)
    {
        count = -1;
    }

    return count;
}


/******************************************************************************/
int ImagePool::GetImageFileWithoutMd5Callback(void *nil, SqlColumn *columns)
{
    int64 iid;
    string location;
    FileUrl url;
    vector<string>* pfiles;
    if(!nil)
    {
        return 0;
    }
    
    pfiles = static_cast<vector<string> *>(nil);
    if ( NULL == columns || 3 != columns->get_column_num())
    {
        return -1;
    }

    columns->GetValue(0,iid);
    columns->GetValue(1,url);
    columns->GetValue(2,location);

    if(url.checksum.empty() && !location.empty())
    {
        OutPut(SYS_DEBUG,"Image %lld has no md5sum, file location:%s\n",iid,location.c_str());
        pfiles->push_back(location);
    }

    return 0;
}

/******************************************************************************/
int ImagePool::GetImageFileWithoutMd5(vector<string>& files)
{
    ostringstream sql;
    //注意，这里的列名来自extended_db_names
    sql << "SELECT image_id,fileurl,location FROM " 
        << image_table;
    OutPut(SYS_DEBUG,"GetImageFileWithoutMd5: %s\n",sql.str().c_str());

    SetCallback(static_cast<Callbackable::Callback>(&ImagePool::GetImageFileWithoutMd5Callback),(void *)&files);
    int ret = _db->Execute(sql, this);
    UnsetCallback();

    return ret;
}

int ImagePool::GetImageCallback(void *nil, SqlColumn *columns)
{    
    if (NULL == columns ||  LIMIT != columns->get_column_num())
    {
        return -1;
    }

    if(!nil)
    {
        return 0;
    }

    Image * image;
    image = static_cast<Image *>(nil);

    columns->GetValue(IMAGE_ID, image->_image_id);
    columns->GetValue(IMAGE_NAME, image->_name);
    columns->GetValue(IMAGE_OWNER, image->_owner);
    columns->GetValue(IMAGE_TYPE, image->_type); 
    columns->GetValue(IMAGE_BUS, image->_bus);
    columns->GetValue(IMAGE_SIZE, image->_size);   
    columns->GetValue(IS_PUBLIC, image->_is_public); 
    columns->GetValue(ENABLED, image->_is_enabled);   
    columns->GetValue(LOCATION, image->_location);
    columns->GetValue(FILEURL, image->_file_url);   
    columns->GetValue(DISK_FORMAT, image->_disk_format);
    columns->GetValue(CONTAINER_FORMAT, image->_container_format);
    columns->GetValue(OS_TYPE, image->_os_type);
    columns->GetValue(OS_VERSION, image->_os_version);
    columns->GetValue(OS_ARCH, image->_os_arch);
    columns->GetValue(REGISTERTIME, image->_register_time);
    columns->GetValue(DESCRIPTION, image->_description);
    columns->GetValue(RUNNING_VMS, image->_running_vms);
    columns->GetValue(REF_COUNT_BY_PRIVATE_VT, image->_ref_count_by_private_vt);
    columns->GetValue(REF_COUNT_BY_PUBLIC_VT, image->_ref_count_by_public_vt);    
    columns->GetValue(DISK_SIZE, image->_disk_size);
    
    return 0;
}


/******************************************************************************/
int64 ImagePool::GetImageIdByLocation(const string& location)
{
    vector<int64> iids;
    ostringstream where;

    where<<" location = '" << location
         << "' ORDER BY image_id ASC LIMIT 1000 ";
    int ret = SelectColumn(image_table,"image_id",where.str(),iids);

    if(SQLDB_RESULT_EMPTY == ret)
    {   /*未找到相应记录*/
        return 0;
    }
    else if(SQLDB_OK != ret)
    {
        return INVALID_OID;
    }
    else
    {
        return iids[0];
    }

};

/******************************************************************************/
int64 ImagePool::GetImageIdByWhere(const string& where)
{
    vector<int64> iids;

    int ret = SelectColumn(image_table, "image_id", where, iids);

    if(0 != ret)
    {
        return INVALID_OID;
    }
    else if(iids.empty())
    {   /*表示找不到*/
        return 0;
    }
    else
    {
        return iids[0];
    }
}
/******************************************************************************/
int ImagePool::GetVmsByLocation(const string& location)
{
    int64 iid = GetImageIdByLocation(location);

    if(INVALID_OID == iid)
    {
        OutPut(SYS_NOTICE,"file %s not register as image!\n",location.c_str());
        return -1;
    }
    else if(0 == iid)
    {  /*表示未找到*/
        return 0;
    }

    ostringstream where;
    int count = 0;

    where << "image_id = " << iid; 
    int ret = SelectColumn(image_all_info,"running_vms",where.str(),count);

    if(ret != 0)
    {
        OutPut(SYS_ERROR,"file %s get error!\n",location.c_str());
        return -1;
    }

    return count;
}

/******************************************************************************/
int ImagePool::GetVmTemplateCount(int64 iid)
{
    //获取一下有多少个虚拟机模板在使用映像
    //无论模板是否公开
    int count1 = -1;
    int count2 = -1;

    ostringstream where;

    where << "image_id = " << iid; 
    int ret = SelectColumn(image_all_info,"ref_count_by_public_vt",where.str(),count1);

    if(0 != ret)
    {
        return -1;
    }

    ret = SelectColumn(image_all_info,"ref_count_by_private_vt",where.str(),count2);

    if(0 != ret)
    {
        return -1;
    }

    return count1 + count2;
}

int ImagePool::GetVmTemplateCount(int64 iid, bool is_public)
{
    //获取一下有多少个虚拟机模板在使用映像
    //可以指定模板是否公开
    int count = -1;
    ostringstream where;
    int ret;

    where << "image_id = " << iid; 

    if(is_public)
    {
        ret = SelectColumn(image_all_info,"ref_count_by_public_vt",where.str(),count);
    }
    else
    {
        ret = SelectColumn(image_all_info,"ref_count_by_private_vt",where.str(),count);
    }

    if(0 != ret)
    {
        return -1;
    }    

    return count;
}

/******************************************************************************/
STATUS ImagePool::GetImageByWhere(const string& where,Image &image)
{
    ostringstream sql;

    SetCallback(static_cast<Callbackable::Callback>(&ImagePool::GetImageCallback),(void *)&image);
    sql<< "SELECT "<< image_extended_db_names << " FROM "
        << image_all_info
        <<" WHERE "<< where;
    int rc = _db->Execute(sql, this);
    UnsetCallback(); 

    if (SQLDB_RESULT_EMPTY == rc)
    {
        return ERROR_OBJECT_NOT_EXIST;
    }
    else if(SQLDB_OK != rc)
    {
        return ERROR;
    }

    return SUCCESS;

}

/******************************************************************************/
STATUS ImagePool::GetImageByImageId(int64 iid,Image &image)
{
    ostringstream sql;

    SetCallback(static_cast<Callbackable::Callback>(&ImagePool::GetImageCallback),(void *)&image);
    sql<< "SELECT "<< image_extended_db_names << " FROM "
        << image_all_info
        <<" WHERE image_id = "<< iid;
    int rc = _db->Execute(sql, this);
    UnsetCallback(); 

    if (SQLDB_RESULT_EMPTY == rc)
    {
        return ERROR_OBJECT_NOT_EXIST;
    }
    else if(SQLDB_OK != rc)
    {
        return ERROR;
    }

    return SUCCESS;

}
/******************************************************************************/
STATUS ImagePool::GetImageByLocation(const string& location,Image &image)
{
    ostringstream sql;

    SetCallback(static_cast<Callbackable::Callback>(&ImagePool::GetImageCallback),(void *)&image);
    sql<< "SELECT "<< image_extended_db_names << " FROM "
        << image_all_info
        <<" WHERE location = '"<< location <<"'";
    int rc = _db->Execute(sql, this);
    UnsetCallback(); 

    if (SQLDB_RESULT_EMPTY == rc)
    {
        cerr<<"No image record"<<endl;
        return ERROR_OBJECT_NOT_EXIST;
    }
    else if(SQLDB_OK != rc)
    {
        cerr<<"get image failed"<<endl;  
        return ERROR;
    }

    return SUCCESS;

}
/******************************************************************************/
int ImagePool::GetImageStoreOptionCallback(void *nil, SqlColumn *columns)
{    
    if (NULL == columns ||  4 != columns->get_column_num())
    {
        return -1;
    }

    if(!nil)
    {
        return 0;
    }

    ImageStoreOption *pOption;
    pOption = static_cast<ImageStoreOption *>(nil);

    columns->GetValue(0, pOption->_img_usage);
    columns->GetValue(1, pOption->_img_srctype);
    columns->GetValue(2, pOption->_img_spcexp);
    columns->GetValue(3, pOption->_share_img_usage);

    return 0;
}
/******************************************************************************/
STATUS ImagePool::GetImageStoreOption(const string& cluster_name,ImageStoreOption& option)
{
    ostringstream sql;

    SetCallback(static_cast<Callbackable::Callback>(&ImagePool::GetImageStoreOptionCallback),(void *)&option);
    sql<< "SELECT img_usage,img_srctype,img_spcexp,share_img_usage FROM cluster_pool"
        <<" WHERE name = '"<< cluster_name + "'";
    int rc = _db->Execute(sql, this);
    UnsetCallback(); 

    if (SQLDB_RESULT_EMPTY == rc)
    {
        return ERROR_OBJECT_NOT_EXIST;
    }
    else if(SQLDB_OK != rc)
    { 
        return ERROR;
    }

    return SUCCESS;
}

/******************************************************************************/
STATUS ImagePool::GetShareDiskInfo(const string& cluster_name, int64 &share_disk_size)
{

    string  column  = " max_free_size";
    string  where   = " cluster_name = '" + cluster_name + "'";
    
    SqlCallbackable sqlcb(_db);
    int rc = sqlcb.SelectColumn(_view_share_disk_info,
                            column,
                            where,
                            share_disk_size);

    if (SQLDB_RESULT_EMPTY == rc)
    {
        cerr<<"No cluster_name record"<<endl;
        return ERROR;
    }
    else if(SQLDB_OK != rc)
    {
        cerr<<"get storage_cluster_max_free_size_view failed"<<endl;  
        return ERROR;
    }

    return SUCCESS;
}

/******************************************************************************/
/* 这个接口给VM使用，主要校验创建虚拟机的时候，使用的映像是否有效 */
STATUS ImagePool::CheckImageAuth(int64 image_id, int64 uid, Image &out_image)
{
    Image temp;

    int ret = GetImageByImageId(image_id,temp);

    if (ERROR == ret)
    {
        return ERROR_OBJECT_NOT_EXIST;
    }

    /* 映像处于disable状态，不能创建虚拟机 */
    if (temp._is_enabled == false)
    {
        return ERR_IMAGE_IS_DISABLE;
    }

    /* 如果映像为公共的，则可以被使用 */
    /* 如果映像为自己创建的，则可以使用 */
    if (temp._is_public || uid == temp._owner)
    {
        out_image._image_id = temp._image_id;
        out_image._owner     = temp._owner;
        out_image._name      = temp._name;
        out_image._type      = temp._type;
        out_image._os_arch   = temp._os_arch;
        out_image._is_public = temp._is_public;
        out_image._is_enabled = temp._is_enabled;
        out_image._register_time = temp._register_time;
        out_image._size = temp._size;
        out_image._running_vms = temp._running_vms;
        out_image.set_file_url(temp._file_url);
        out_image._bus = temp._bus;
        out_image._os_type= temp._os_type;
        out_image._disk_size= temp._disk_size;
        out_image._disk_format= temp._disk_format;
        return SUCCESS;
    }

    return ERR_IMAGE_NOT_PUBLIC;
}

/******************************************************************************/
/* 检查镜像是否仅被上传者引用 */
STATUS ImagePool::CheckImageUsedbyOther(int64 image_id)
{
    vector<int64> iids;
    string column = " image_id";
    ostringstream where;
    where << "image_id = " << image_id;

    SqlCallbackable sqlcb(_db);
    int rc = sqlcb.SelectColumn(_view_image_other_user,
                            column,
                            where.str(),
                            iids);

    if (SQLDB_RESULT_EMPTY == rc)
    {
        cerr<<"Image not used by others"<<endl;
        return SUCCESS;
    }
    else if(SQLDB_OK != rc)
    {
        cerr<<"CheckImageUsedByOwner failed"<<endl;  
        return ERR_EXIST;
    }

    return ERR_EXIST;
}

/******************************************************************************/
STATUS ImagePool::Update(const Image & image)
{
    SqlCommand sql(_db,image_table);

    sql.Add("image_id", image._image_id);
    sql.Add("name", image._name);
    sql.Add("description", image._description);
    sql.Add("register_time", image._register_time);
    sql.Add("uid", image._owner);
    sql.Add("type", image._type); 
    sql.Add("bus", image._bus);
    sql.Add("size", image._size);   
    sql.Add("is_public", image._is_public); 
    sql.Add("enabled", image._is_enabled);   
    sql.Add("location", image._location);
    sql.Add("fileurl", image._file_url);   
    sql.Add("disk_format", image._disk_format);
    sql.Add("container_format", image._container_format);
    sql.Add("os_type", image._os_type);
    sql.Add("os_version", image._os_version);
    sql.Add("os_arch", image._os_arch);
    sql.Add("disk_size", image._disk_size);

    return sql.Update("WHERE image_id = " + to_string<int64>(image._image_id,dec));
};

STATUS ImagePool::Insert(const Image & image)
{
    SqlCommand sql(_db,image_table);

    sql.Add("image_id", image._image_id);
    sql.Add("name", image._name);
    sql.Add("description", image._description);
    sql.Add("register_time", image._register_time);
    sql.Add("uid", image._owner);
    sql.Add("type", image._type); 
    sql.Add("bus", image._bus);
    sql.Add("size", image._size);   
    sql.Add("is_public", image._is_public); 
    sql.Add("enabled", image._is_enabled);   
    sql.Add("location", image._location);
    sql.Add("fileurl", image._file_url);   
    sql.Add("disk_format", image._disk_format);
    sql.Add("container_format", image._container_format);
    sql.Add("os_type", image._os_type);
    sql.Add("os_version", image._os_version);
    sql.Add("os_arch", image._os_arch);
    sql.Add("disk_size", image._disk_size);

    return sql.Insert();
};


/******************************************************************************/
STATUS ImagePool::Delete(int64 image_id)
{
    int ret = -1;
    ostringstream oss;

    oss << "DELETE FROM " << image_table << " WHERE image_id = " << image_id;
    ret = _db->Execute(oss);
    if (ret != 0)
    {
        cerr << oss.str() << endl;
        SkyAssert(false);
        return ret;
    }
    return SUCCESS;
};

/******************************************************************************/
int ImagePool::Show(vector<ImageInfo> &vec_img_info, const string &where)
{
    int             rc;
    ostringstream   cmd;

    SetCallback(static_cast<Callbackable::Callback>(&ImagePool::ShowCallback),
               static_cast<void *>(&vec_img_info));

    cmd << "SELECT "<< image_extended_db_names << " FROM "
        << image_all_info;

    if (!where.empty())
    {
        cmd << " WHERE " << where;
    }

    rc = _db->Execute(cmd, this);

    UnsetCallback();

    if ((SQLDB_OK != rc)
         &&(SQLDB_RESULT_EMPTY != rc))
    {
        return -1;
    }

    return 0;
}

/******************************************************************************/
int ImagePool::Allocate (Image& image,string &error_str)
{
    int       ret = -1;

    _image_pool_mutex.Lock(); 
    /*在现有的image_id基础上增加1*/
    image._image_id = ++_last_image_id;
    _image_pool_mutex.Unlock();

    ret = Insert(image);
    if(0 != ret)
    {
        error_str = "*** Insert Image failed ***";       
        return -1;
    }

    return ret;    
}

/******************************************************************************/
int ImagePool::ShowCallback(void * arg, SqlColumn * columns)
{
    vector<ImageInfo> *ptImageRecord;
    ptImageRecord = static_cast<vector<ImageInfo> *>(arg);
    ImageInfo  tInfo;
    
    if (NULL == columns ||  LIMIT != columns->get_column_num())
    {
        return -1;
    }

    columns->GetValue(IMAGE_ID, tInfo._image_id);
    columns->GetValue(IMAGE_OWNER, tInfo._owner);
    columns->GetValue(IMAGE_NAME, tInfo._name);

    string location;
    columns->GetValue(LOCATION, location);
    tInfo._file   =  location.substr(location.find_last_of('/')+1);

    FileUrl url;
    columns->GetValue(FILEURL, url);
    tInfo._md5sum = url.checksum;
    
    columns->GetValue(IMAGE_TYPE, tInfo._type);

    int   tmp_int =0;    
    columns->GetValue(IS_PUBLIC, tmp_int);
    tInfo._is_public = tmp_int;
    
    columns->GetValue(REGISTERTIME, tInfo._register_time);
    columns->GetValue(IMAGE_SIZE, tInfo._size);
    
    columns->GetValue(ENABLED, tmp_int);
    tInfo._is_enabled = tmp_int;
    
    columns->GetValue(RUNNING_VMS, tInfo._running_vms);
    columns->GetValue(OS_TYPE, tInfo._os_type);
    columns->GetValue(OS_VERSION, tInfo._os_version);
    columns->GetValue(OS_ARCH, tInfo._os_arch);
    columns->GetValue(IMAGE_BUS, tInfo._bus);
    columns->GetValue(DESCRIPTION, tInfo._description);

    columns->GetValue(REF_COUNT_BY_PRIVATE_VT, tInfo._ref_count_by_private_vt);
    columns->GetValue(REF_COUNT_BY_PUBLIC_VT, tInfo._ref_count_by_public_vt);
    

    columns->GetValue(DISK_SIZE, tInfo._disk_size);
    columns->GetValue(DISK_FORMAT, tInfo._disk_format);
    columns->GetValue(CONTAINER_FORMAT, tInfo._container_format);

    ptImageRecord->push_back(tInfo);
    
    return 0;
}

}

    
