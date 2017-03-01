/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：image_pool.h
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
*    修改内容：image_pool改为非缓存模式
*******************************************************************************/
#ifndef IMAGE_MANGER_IMAGE_POOL_H        
#define IMAGE_MANGER_IMAGE_POOL_H
#include "image.h"
#include "pool_sql.h"
#include "image_manager_with_api_method.h"

namespace zte_tecs
{

/**
@brief 功能描述: 映像资源池的实体类\n
@li @b 其它说明: 无
*/
class ImagePool:public SqlCallbackable
{
    // 友元函数
    friend ostream& operator<<(ostream &os, const Image &im);
/*******************************************************************************
* 1. public section
*******************************************************************************/
public:
    int64 get_last_image_id()
    {
	return _last_image_id;
    };
	
    void set_last_image_id(int64 iid)
    {
	_last_image_id = iid;
    };

    static ImagePool* GetInstance()
    {
        return instance;
    };

    static ImagePool* CreateInstance(SqlDB *db)
    {
        if (NULL == instance)
        {
            instance = new ImagePool(db);
        }

        return instance;
    };

    ~ImagePool()
    {};

    STATUS Init();

    const string& get_default_type()
    {
        return _default_type;
    };
	
    int Allocate (Image& image,string &error_str);
    int64 GetImageIdByLocation(const string& location);
    int64 GetImageIdByWhere(const string& where);
    int GetVmsByLocation(const string& location);
    int64 GetImageCountByUid(int64 uid);
    int64 GetImageCountByWhere(const string &where);
    int GetImageFileWithoutMd5(vector<string>& files);
    int GetVmTemplateCount(int64 iid);
    int GetVmTemplateCount(int64 iid, bool is_public);
    STATUS GetImageByWhere(const string& where,Image &image);
    STATUS GetImageByImageId(int64 iid,Image &image);
    STATUS GetImageByLocation(const string& location,Image &image);
    int GetImageStoreOptionCallback(void *nil, SqlColumn *columns);
    STATUS GetImageStoreOption(const string& cluster_name,ImageStoreOption& option);
    STATUS GetShareDiskInfo(const string& cluster_name, int64 &share_disk_size);
    STATUS CheckImageUsedbyOther(int64 image_id);
    STATUS Update(const Image & image);
    STATUS Insert(const Image & image);
    STATUS Delete(int64 image_id);
	int Show(vector<ImageInfo> &vec_img_info, const string& where);
    STATUS CheckImageAuth(int64 image_id, int64 uid, Image &out_image);

private:
    ImagePool (SqlDB *db):SqlCallbackable(db)
    {
        _default_type = IMAGE_TYPE_MACHINE;
    }

    static ImagePool* instance;
    string  _default_type;
    int64  _last_image_id;
    Mutex _image_pool_mutex;

    static const char * image_table;
    static const char * image_all_info;
    static const char * image_extended_db_names;
    static const char * _view_share_disk_info;
    static const char * _view_image_other_user;

    enum ColNames
    {         
        IMAGE_ID         = 0,    /* Image id, allocat by file storage module */
        IMAGE_NAME       = 1,    /* Image name                  */
        IMAGE_OWNER      = 2,    /* Image owner id              */
        IMAGE_TYPE       = 3,    /* Image type， see detail in api_const.h      */
        IMAGE_BUS        = 4,    /* Image bus，= ide,scsi,xvd    */
        IMAGE_SIZE       = 5,    /* The size of image file      */
        IS_PUBLIC        = 6,    /* Public scope (YES OR NO)    */
        ENABLED          = 7,    /* 0)INIT  1)USED   2)DISABLE  */
        LOCATION         = 8,   /* 映像文件存放位置 */
        FILEURL          = 9,   /* 映像文件URL */
        DISK_FORMAT      = 10,   /* raw,qcow2,vhd,vmdk ... */
        CONTAINER_FORMAT = 11,   /* AMI,OVF */
        OS_TYPE          = 12,   /* 映像中预装操作系统类型，windows还是linux?   */
        OS_VERSION       = 13,   /* 映像中预装的操作系统版本号 */ 
        OS_ARCH          = 14,   /* Image arch   */
        REGISTERTIME     = 15,    /* Time of registration        */
        DESCRIPTION      = 16,   /* description arch   */
        RUNNING_VMS      = 17,   /* Number of VMs using the img */
        REF_COUNT_BY_PRIVATE_VT      = 18,   /* Number of VMs using the img */
        REF_COUNT_BY_PUBLIC_VT      = 19,   /* Number of VMs using the img */
        DISK_SIZE        = 20,   /* image size */
        LIMIT            = 21

    };
	
    int64 GenerateImageId(int64 count);
    int ShowCallback(void * arg, SqlColumn * columns);
    int GetImageFileWithoutMd5Callback(void *nil, SqlColumn *columns);
    int GetImageCallback(void *nil, SqlColumn *columns);

    DISALLOW_COPY_AND_ASSIGN(ImagePool); 
};


}
#endif /* IMAGE_MANGER_IMAGE_POOL_H */

