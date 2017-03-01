/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�storage.h
* �ļ���ʶ��
* ����ժҪ��storageģ��ͷ�ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�yanwei
* ������ڣ�2011��7��25��
*
* �޸ļ�¼1��
*     �޸����ڣ�2011/7/25
*     �� �� �ţ�V1.0
*     �� �� �ˣ�yanwei
*     �޸����ݣ�����
*******************************************************************************/

#ifndef STORAGE_H
#define STORAGE_H

#include <vector>
#include "sky.h"
#include "volume.h"
#include "mid.h"
#include "registered_system.h"


using namespace zte_tecs;

/****************************************************************************************
*                                 �ӿں�ֵ                                              *
****************************************************************************************/
/*����״̬*/
#define DISKSTATE_GOOD                     0   /* ����״̬��� */
#define DISKSTATE_BROKEN                   1   /* ����״̬���� */
/****************************************************************************************
*                                 ��ֵ                                                  *
****************************************************************************************/   
/*�ļ�����*/
#define FILETYPE_REG  ((int)0) /*��ͨ�ļ�*/
#define FILETYPE_DIR  ((int)1) /*Ŀ¼*/
#define FILETYPE_LNK  ((int)2) /*������*/
#define FILETYPE_CHR  ((int)3) /*�ַ��豸*/
#define FILETYPE_BLK  ((int)4) /*���豸*/
#define FILETYPE_FIFO ((int)5) /*�ܵ�*/
#define FILETYPE_SOCK ((int)6) /*�׽���*/
#define FILETYPE_UNKNOWN ((int)0xFFFFFFFF) /*δ֪����*/

/*������� */
#define STORAGE_ALIGN_SIZE    (4*1024*1024)     /*4M����*/
#define STORAGE_SIZE_ALIGNED(origi_size)    \
    (((origi_size - 1) / STORAGE_ALIGN_SIZE + 1) * STORAGE_ALIGN_SIZE)     /*���������С*/
#if 0
/*Դӳ������*/
#define INVALID_STORAGE_SOURCE_TYPE ((int)-1)  /*�Ƿ�*/
#define STORAGE_SOURCE_TYPE_FILE    ((int)0)   /*�ļ�*/
#define STORAGE_SOURCE_TYPE_BLOCK   ((int)1)   /*��*/
#endif
#define STORAGE_ITEM_FAILED     ((int)-1)
#define STORAGE_ITEM_NOT_READY  ((int)0)
#define STORAGE_ITEM_READY      ((int)1)
#define STORAGE_ITEM_TEMP_READY ((int)2)
#define STORAGE_ITEM_BASE_READY ((int)3)
#define STORAGE_ITEM_SS_READY   ((int)4)
/*�洢λ��*/
#define STORAGE_POSITION_LOCAL      ((int)1)   /*����*/
#define STORAGE_POSITION_SHARE      ((int)2)   /*����*/

#define STORAGE_ACCESS_ISCSI        "iscsi"
/****************************************************************************************
*                                 ����                                                  *
****************************************************************************************/
const int STORAGE_MAX_CMDRESLEN = 2048; /*��������󳤶�*/
const string STORAGE_SOURCE_TYPE_FILE  = "file"; /*�洢Դ����Ϊ�ļ�*/
const string STORAGE_SOURCE_TYPE_BLOCK = "block";/*�洢Դ����Ϊ���豸*/
const string STORAGE_MKFS_SHELL = "/var/run/mkfsdevice.sh";
/****************************************************************************************
*                                 ���ݽṹ����                                          *
****************************************************************************************/
typedef struct tagDiskStateInfo
{
    string _name;
    int32  _state;          //0: ����״̬��ã�1: ����״̬����
}DiskStateInfo;
/*��ȡ�����С*/
typedef struct tagTVGSpaceTotalInfo
{
    int     iVGCountNum;        //VG����(Ŀǰʹ��1��)
    int64   uiTotalCapacity;    //�߼��������ܿռ�(��λB)
    int64   uiAllocCapacity;    //�߼��������Ѿ�ʹ�ÿռ�(��λB)
    int64   uiFreeCapacity;     //�߼�������ʣ����ÿռ�(��λB)
}TVGSpaceTotalInfo;
/****************************************************************************************
*                                 ����ӿ�����                                          *
****************************************************************************************/
//int HCStorageInit(void);
//int32 HCGetPhysicalDiskState(vector<DiskStateInfo> &state_info);
//int32 HCGetVGSpaceTotalInfo(TVGSpaceTotalInfo &resTVGSpaceTotalInfo);
/****************************************************************************************
*                                 �ڲ��ӿ�����                                          *
****************************************************************************************/
int storage_command(const string &cmd,string &res);
int get_filetype(const string& name,int &type);
bool check_fileexist(const string& name);
bool is_regular(const string& name);
bool is_directory(const string& name);
bool check_mount(const string &name);
int get_mount(const string &fsname,string &mountdir);
bool findsubstr(const string &str,const string &sub);
bool runfind(const string &cmd,const string &find,string &res);
bool runfind(const string &cmd,const string &find);
bool check_filesystem(const string &fsname, const string &fsprefix);
int make_filesystem(const string &fsname, const string &fstype = "ext4");
void gen_mkfsscript(void);
int sys_md5(const string &filename,string &md5);
MID GetSC_MID();
int GetTailBytes(const string &name, char *buf, size_t count);
int SetTailBytes(const string &name, const char *buf, size_t count);
STATUS LoadShareDisk(int64 vid, 
                         const string &target, 
                         const string &access_option, 
                         const string &fstype, 
                         string &device);
STATUS RefreshLoadShareDisk(const string &target, 
                         const string &access_option);
STATUS UnLoadShareDisk(int64 vid, 
                            const string &target, 
                            const string &access_option);
STATUS CreateShareDisk(MessageUnit *mu, const string& request_id, const string& base_id, int64 size);
STATUS AuthShareDisk(MessageUnit *mu, const auth_info& auth);
STATUS DeAuthShareDisk(MessageUnit *mu, const auth_info& auth);
STATUS DeleteShareDisk(MessageUnit *mu, const string& request_id);
STATUS CheckImgFormatAndSize(const string& img, string& format, int64& size);
void   ReclaimDevice();

#endif

