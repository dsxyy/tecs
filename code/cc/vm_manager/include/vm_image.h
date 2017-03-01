/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�vm_image.h
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ����������̡�ӳ��������Ϣ����ģ��ͷ�ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�ҦԶ
* ������ڣ�2012��4��9��
*
* �޸ļ�¼1��
*     �޸����ڣ�2012/04/09
*     �� �� �ţ�V1.0
*     �� �� �ˣ�ҦԶ
*     �޸����ݣ�����
*******************************************************************************/
#ifndef TECS_CC_VM_IMAGE_H
#define TECS_CC_VM_IMAGE_H
#include "sky.h"
#include "pool_object_sql.h"
#include "vm_messages.h"
#include "pool_sql.h"

namespace zte_tecs
{

#define IMG_BACKUP_ACTIVE  1
#define IMG_BACKUP_BACKED  0 

class VMImgBackup: public Serializable
{
    /*******************************************************************************
    * 1. public section
    *******************************************************************************/
public:
    VMImgBackup()   
    {
     
        
    };
    ~VMImgBackup()    {};
    
    SERIALIZE
    {
        SERIALIZE_BEGIN(VMImgBackup);
        WRITE_VALUE(_request_id);
        WRITE_VALUE(_vid);
        WRITE_VALUE(_target);
        WRITE_VALUE(_state);
        WRITE_VALUE(_parent_request_id);   
        WRITE_OBJECT(_create_time);
        WRITE_VALUE(_description);   
        SERIALIZE_END();
    };

    DESERIALIZE
    {
        DESERIALIZE_BEGIN(VMImgBackup);
        READ_VALUE(_request_id);
        READ_STRING(_vid);
        READ_DIGIT(_target);
        READ_DIGIT(_state);
        READ_DIGIT(_parent_request_id);   
        READ_OBJECT(_create_time);
        READ_DIGIT(_description);        
        DESERIALIZE_END();
    };    
    string _request_id;    
    int64 _vid;   
    string _target;
    int   _state;
    string _parent_request_id;    
    Datetime _create_time;
    string  _description;

};   

class Host_using_Image
{
public:
    int64 _hid;
    int64 _image_id;
};

inline bool operator==(const Host_using_Image &lhi, const Host_using_Image &rhi)
{
    return ((lhi._hid == rhi._hid) && (lhi._image_id == rhi._image_id));
}

class VMImagePool : public  SqlCallbackable
{
public:
    static VMImagePool* GetInstance()
    {
        return instance;
    };

    static VMImagePool* CreateInstance(SqlDB *db)
    {
        if(NULL == instance)
        {
            instance = new VMImagePool(db);
        }

        return instance;
    };

    ~VMImagePool(){};

    int InsertVMImageConfig(int64 vid, const VMConfig &vmcfg);
    int UpdateVMImageConfig(int64 vid, const VMConfig &vmcfg);    
    int GetHostUsingImage(vector<Host_using_Image> &vec);
    int GetDisk(int64 vid, vector<VmDiskConfig> &disks);
    int GetDisk(const string& request_id, VmDiskConfig& disk);
    int SelectShareDisk(void *nil, SqlColumn *columns);
    int SelectImageCallback(void *nil, SqlColumn *columns);
    int UpdateDisk(int64 vid, const VmDiskConfig &disk);
    int UpdateImage(int64 vid, const VmDiskConfig &disk);
    int GetImageRsvNumByVid(int64 vid,int &num);
    int DeleteDisk(int64 vid, const VmDiskConfig &disk);
    int InsertDisk(int64 vid, const VmDiskConfig &disk);
    bool IsDiskExist(const string& request_id);
    bool IsDiskExist(const string& request_id,int vid);
    int GetDiskTarget(const string& request_id,int vid,string &target);
    int UpdateImageBackup(const string &request_id, const string &description);
    int GetImageBackupState(const string & request_id, const string & target,int vid, int & state);
    int UpdateImageBackup(const string & request_id, const string & target,int64 vid,int state);
    int SetImageBackupState(const string & target,int64 vid);
    int UpdateImageBackup(const string& request_id,int state);
    int GetActiveImageBackup(string& request_id,const string & target,int64 vid);
    int InsertImageBackup(VMImgBackup &imgbackup);
    int DeleteImageBackup(const string& request_id);
    int GetSonImageBackup(const string& request_id, vector<string>& son_request_id);
    int GetImageBackupTarget(const string& request_id,string & target);
    int DeleteImageBackupByVid(int64 vid,string & target);
    bool IsImageBackupExist(const string& request_id);
    bool IsBaseImageBackup(string& request_id,const string & target,int64 vid);
    int GetImageBackupNumByVid(int64 vid,int &num);
    bool IsDiskExist(int64 vid, const string& target);
    bool IsImageExist(int64 vid, const string& target);
    int DeleteVmImage(int64 vid, const VmDiskConfig &disk);
    int InsertVmImage(int64 vid, const VmDiskConfig &disk);

private:
    enum ColNames
    {
        VID = 0,
        TARGET,
        SIZE,
        POSITION,
        REQUEST_ID,
        ACCESS_URL,
        IS_NEED_RELEASE,
        LIMIT = 7,
    };
    
    VMImagePool(SqlDB *pDb):SqlCallbackable(pDb)
    {
        db = pDb;
    }
    
    vector<VmDiskConfig> FilterImager(const VMConfig &vmcfg);
    int32 GetHostUsingImageCallback(void *nil, SqlColumn *columns);
    DISALLOW_COPY_AND_ASSIGN(VMImagePool);
    static VMImagePool *instance;

    static const char *table_image;//����
    static const char *table_image_cancel;//����
    static const char *col_names_image;//��ѯ����
    static const char *col_names_image_cancel;//��ѯ����

    static const char *table_disk;//����
    static const char *table_disk_cancel;//����
    static const char *col_names_disk;//��ѯ����
    static const char *col_names_disk_cancel;//��ѯ����

    static const char *table_image_backup;//����
    static const char *col_names_image_backup;

    static const char *db_bootstrap_image;//�������SQL
    static const char *db_bootstrap_image_cancel;
    static const char *db_bootstrap_vm_image_pool_with_hid;

    SqlDB *db;
};

}
#endif

