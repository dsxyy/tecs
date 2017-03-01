/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：storage.h
* 文件标识：
* 内容摘要：storage模块头文件
* 当前版本：1.0
* 作    者：yanwei
* 完成日期：2011年7月25日
*
* 修改记录1：
*     修改日期：2011/7/25
*     版 本 号：V1.0
*     修 改 人：yanwei
*     修改内容：创建
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
*                                 接口宏值                                              *
****************************************************************************************/
/*磁盘状态*/
#define DISKSTATE_GOOD                     0   /* 磁盘状态完好 */
#define DISKSTATE_BROKEN                   1   /* 磁盘状态故障 */
/****************************************************************************************
*                                 宏值                                                  *
****************************************************************************************/   
/*文件属性*/
#define FILETYPE_REG  ((int)0) /*普通文件*/
#define FILETYPE_DIR  ((int)1) /*目录*/
#define FILETYPE_LNK  ((int)2) /*软链接*/
#define FILETYPE_CHR  ((int)3) /*字符设备*/
#define FILETYPE_BLK  ((int)4) /*块设备*/
#define FILETYPE_FIFO ((int)5) /*管道*/
#define FILETYPE_SOCK ((int)6) /*套接字*/
#define FILETYPE_UNKNOWN ((int)0xFFFFFFFF) /*未知类型*/

/*计算对齐 */
#define STORAGE_ALIGN_SIZE    (4*1024*1024)     /*4M对齐*/
#define STORAGE_SIZE_ALIGNED(origi_size)    \
    (((origi_size - 1) / STORAGE_ALIGN_SIZE + 1) * STORAGE_ALIGN_SIZE)     /*计算对齐后大小*/
#if 0
/*源映像类型*/
#define INVALID_STORAGE_SOURCE_TYPE ((int)-1)  /*非法*/
#define STORAGE_SOURCE_TYPE_FILE    ((int)0)   /*文件*/
#define STORAGE_SOURCE_TYPE_BLOCK   ((int)1)   /*块*/
#endif
#define STORAGE_ITEM_FAILED     ((int)-1)
#define STORAGE_ITEM_NOT_READY  ((int)0)
#define STORAGE_ITEM_READY      ((int)1)
#define STORAGE_ITEM_TEMP_READY ((int)2)
#define STORAGE_ITEM_BASE_READY ((int)3)
#define STORAGE_ITEM_SS_READY   ((int)4)
/*存储位置*/
#define STORAGE_POSITION_LOCAL      ((int)1)   /*本地*/
#define STORAGE_POSITION_SHARE      ((int)2)   /*共享*/

#define STORAGE_ACCESS_ISCSI        "iscsi"
/****************************************************************************************
*                                 常量                                                  *
****************************************************************************************/
const int STORAGE_MAX_CMDRESLEN = 2048; /*命令结果最大长度*/
const string STORAGE_SOURCE_TYPE_FILE  = "file"; /*存储源类型为文件*/
const string STORAGE_SOURCE_TYPE_BLOCK = "block";/*存储源类型为块设备*/
const string STORAGE_MKFS_SHELL = "/var/run/mkfsdevice.sh";
/****************************************************************************************
*                                 数据结构声明                                          *
****************************************************************************************/
typedef struct tagDiskStateInfo
{
    string _name;
    int32  _state;          //0: 磁盘状态完好，1: 磁盘状态故障
}DiskStateInfo;
/*获取卷组大小*/
typedef struct tagTVGSpaceTotalInfo
{
    int     iVGCountNum;        //VG个数(目前使用1个)
    int64   uiTotalCapacity;    //逻辑卷组中总空间(单位B)
    int64   uiAllocCapacity;    //逻辑卷组中已经使用空间(单位B)
    int64   uiFreeCapacity;     //逻辑卷组中剩余可用空间(单位B)
}TVGSpaceTotalInfo;
/****************************************************************************************
*                                 对外接口声明                                          *
****************************************************************************************/
//int HCStorageInit(void);
//int32 HCGetPhysicalDiskState(vector<DiskStateInfo> &state_info);
//int32 HCGetVGSpaceTotalInfo(TVGSpaceTotalInfo &resTVGSpaceTotalInfo);
/****************************************************************************************
*                                 内部接口申明                                          *
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

