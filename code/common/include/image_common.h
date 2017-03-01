/******************************************************************************
* Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：image_common.h
* 文件标识：
* 内容摘要：image 通用信息头文件
* 当前版本：1.0
* 作    者：颜伟
* 完成日期：2013年3月12日
*
* 修改记录1：
*     修改日期：2013/3/12
*     版 本 号：V1.0
*     修 改 人：颜伟
*     修改内容：创建
******************************************************************************/
#ifndef TECS_IMAGE_COMMON_H
#define TECS_IMAGE_COMMON_H
#include "sky.h"
#include "storage.h"
#include "tecs_pub.h"

namespace zte_tecs
{
#define IMAGE_ACCESS_NFS        "nfs"
#define IMAGE_ACCESS_HTTP       "http"
#define IMAGE_ACCESS_ISCSI      "iscsi"
#define IMAGE_ACCESS_FTP        "ftp"

#define IMAGE_DEFAULT_FSTYPE    "ext4"

#define IMAGE_SOURCE_TYPE_FILE   "file"
#define IMAGE_SOURCE_TYPE_BLOCK  "block"

#define IMAGE_USE_SNAPSHOT       "snapshot"
#define IMAGE_USE_NOSNAPSHOT     "no-snapshot"

#define IMAGE_POSITION_LOCAL STORAGE_POSITION_LOCAL
#define IMAGE_POSITION_SHARE STORAGE_POSITION_SHARE

#define IMAGE_CONTAINER_TPI      "tpi"
#define IMAGE_TPI_SUFFIX         ".tpi"
#define IMAGE_TPI_LST_SUFFIX     ".lst"
#define IMAGE_TPI_CFG_SUFFIX     ".cfg"
#define IMAGE_TPI_IMG_SUFFIX     ".img"
#define IMAGE_NAME_DUP_PREFIX    "dup_"

#define MAX_IMAGE_NAME_LENGTH    256

class ImageStoreOption: public Serializable
{
public:
    /**
    @brief 参数描述: 映像所需扩展空间比例
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 20
    @li @b 其它说明: 一般情况下为基础映像空间和映像大小的比,当_img_srctype=0情况下有效
    */
    int _img_spcexp;
    /**
    @brief 参数描述: 指定源映像文件类型
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: 0
    @li @b 其它说明: 暂时支持两种类型 0-file 和 1-block
    */
    string  _img_srctype;
    /**
    @brief 参数描述: 映像使用方法
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: snapshot
    @li @b 其它说明: 暂时支持两种类型 snapshot 和 direct
    */
    string _img_usage;  
    
    /**
    @brief 参数描述:共享 映像使用方法
    @li @b 是否用户必填: 否
    @li @b 是否可配多个: 否
    @li @b 默认值（租户可不填的情况下）: snapshot
    @li @b 其它说明: 暂时支持两种类型 snapshot 和 direct
    */
    string _share_img_usage;  

    
    ImageStoreOption()
    {
        _img_usage  = IMAGE_USE_SNAPSHOT;
        _img_spcexp = 20;
        _img_srctype = IMAGE_SOURCE_TYPE_FILE;
        _share_img_usage = IMAGE_USE_SNAPSHOT;
    };

    ImageStoreOption(const string& imgsrctype, int imgspcexp, const string& imgusage, const string& shareimgusage)
    { 
        _img_usage   = imgusage; 
        _img_srctype = imgsrctype;
        _img_spcexp  = imgspcexp;
        _share_img_usage = shareimgusage;
    };
    
    bool IsValid() const
    {
        return ((0 <= _img_spcexp) 
               && (IMAGE_SOURCE_TYPE_FILE == _img_srctype || IMAGE_SOURCE_TYPE_BLOCK == _img_srctype)
               && (IMAGE_USE_SNAPSHOT == _img_usage || IMAGE_USE_NOSNAPSHOT == _img_usage)
               && (IMAGE_USE_SNAPSHOT == _share_img_usage || IMAGE_USE_NOSNAPSHOT == _share_img_usage));
    };
    
    SERIALIZE
    {
        SERIALIZE_BEGIN(ImageStoreOption);   
        WRITE_DIGIT(_img_spcexp);  
        WRITE_STRING(_img_srctype);
        WRITE_STRING(_img_usage);
        WRITE_STRING(_share_img_usage);
        SERIALIZE_END(); 
    }
    
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(ImageStoreOption);  
        READ_DIGIT(_img_spcexp);  
        READ_STRING(_img_srctype); 
        READ_STRING(_img_usage);
        READ_STRING(_share_img_usage);
        DESERIALIZE_END();
    }
};

}
#endif


