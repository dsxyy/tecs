/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�image_pool.h
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��image��Ķ����ļ�
* ��ǰ�汾��2.0
* ��    �ߣ�lichun
* ������ڣ�2012��10��19��
*
* �޸ļ�¼1��
*    �޸����ڣ�2012/10/19
*    �� �� �ţ�V2.0
*    �� �� �ˣ�lichun
*    �޸����ݣ�image_pool��Ϊ�ǻ���ģʽ
*******************************************************************************/
#ifndef IMAGE_MANGER_IMAGE_POOL_H        
#define IMAGE_MANGER_IMAGE_POOL_H
#include "image.h"
#include "pool_sql.h"
#include "image_manager_with_api_method.h"

namespace zte_tecs
{

/**
@brief ��������: ӳ����Դ�ص�ʵ����\n
@li @b ����˵��: ��
*/
class ImagePool:public SqlCallbackable
{
    // ��Ԫ����
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
        IMAGE_TYPE       = 3,    /* Image type�� see detail in api_const.h      */
        IMAGE_BUS        = 4,    /* Image bus��= ide,scsi,xvd    */
        IMAGE_SIZE       = 5,    /* The size of image file      */
        IS_PUBLIC        = 6,    /* Public scope (YES OR NO)    */
        ENABLED          = 7,    /* 0)INIT  1)USED   2)DISABLE  */
        LOCATION         = 8,   /* ӳ���ļ����λ�� */
        FILEURL          = 9,   /* ӳ���ļ�URL */
        DISK_FORMAT      = 10,   /* raw,qcow2,vhd,vmdk ... */
        CONTAINER_FORMAT = 11,   /* AMI,OVF */
        OS_TYPE          = 12,   /* ӳ����Ԥװ����ϵͳ���ͣ�windows����linux?   */
        OS_VERSION       = 13,   /* ӳ����Ԥװ�Ĳ���ϵͳ�汾�� */ 
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

