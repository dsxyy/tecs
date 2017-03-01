/******************************************************************************
* Copyright (c) 2012������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�image_common.h
* �ļ���ʶ��
* ����ժҪ��image ͨ����Ϣͷ�ļ�
* ��ǰ�汾��1.0
* ��    �ߣ���ΰ
* ������ڣ�2013��3��12��
*
* �޸ļ�¼1��
*     �޸����ڣ�2013/3/12
*     �� �� �ţ�V1.0
*     �� �� �ˣ���ΰ
*     �޸����ݣ�����
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
    @brief ��������: ӳ��������չ�ռ����
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: 20
    @li @b ����˵��: һ�������Ϊ����ӳ��ռ��ӳ���С�ı�,��_img_srctype=0�������Ч
    */
    int _img_spcexp;
    /**
    @brief ��������: ָ��Դӳ���ļ�����
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: 0
    @li @b ����˵��: ��ʱ֧���������� 0-file �� 1-block
    */
    string  _img_srctype;
    /**
    @brief ��������: ӳ��ʹ�÷���
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: snapshot
    @li @b ����˵��: ��ʱ֧���������� snapshot �� direct
    */
    string _img_usage;  
    
    /**
    @brief ��������:���� ӳ��ʹ�÷���
    @li @b �Ƿ��û�����: ��
    @li @b �Ƿ������: ��
    @li @b Ĭ��ֵ���⻧�ɲ��������£�: snapshot
    @li @b ����˵��: ��ʱ֧���������� snapshot �� direct
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


