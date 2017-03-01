/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：host_storage.h
* 文件标识：
* 内容摘要：物理机storage模块头文件
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

#ifndef HOST_STORAGE_H
#define HOST_STORAGE_H

#include <mntent.h>
#include <vector>
#include "sky.h"
#include "image_url.h"
#include "vm_messages.h"


using namespace zte_tecs;

/****************************************************************************************
*                                    宏值                                               *
****************************************************************************************/
#define MAX_IMGDOWNLOAD_THREADS ((int)10) /*映像下载线程池最大线程数*/
#define MAX_IMGUPLOAD_THREADS   ((int)3)
#define STORAGE_VM_STATE_DEPLOY ((int)0)  /*获取存储过程中 对应的虚拟机状态置为部署*/
#define STORAGE_VM_STATE_CANCEL ((int)1)  /*获取存储过程中 对应的虚拟机状态被置为撤销*/
#define STORAGE_VM_STATE_FAILED ((int)2)  /*对应的虚拟机获取存储过程失败*/
#define STORAGE_VM_STATE_READY  ((int)3)  /*对应的虚拟机获取ok*/
#define UPLOAD_STATE_INIT       ((int)0)  /*上传初始化状态*/
#define UPLOAD_STATE_UPLOAD     ((int)1)  /*上传状态*/
#define UPLOAD_STATE_CANCEL     ((int)2)  /*上传取消状态*/
#define UPLOAD_STATE_FAILED     ((int)3)  /*上传失败状态*/
#define UPLOAD_STATE_FINISH     ((int)4)  /*上传完成状态*/
#define UPLOAD_STATE_UNKNOWN    ((int)5)  /*上传位置状态*/
#define SAVE_FREE_DELAY_TIME    ((uint32)5000) /*安全释放延时时间*/
/****************************************************************************************
*                                    常量                                               *
****************************************************************************************/
const string STORAGE_INSTANCES_DIR = "/var/lib/tecs/instances/";
const string STORAGE_HC_MOUNT_DIR  = "/mnt/tecs_hc";
const string IMG2BLOCK_SIGNATURE   = "tecsimage";
/****************************************************************************************
*                                  数据结构                                             *
****************************************************************************************/
#define DIE_ON_ERROR AI_CANONNAME
    
#ifndef offsetof
#define offsetof(type, field)	((long) &((type *)0)->field)
#endif   /* offsetof */
    
#ifndef BUFSIZ
# define BUFSIZ 4096
#endif
    
typedef union 
{
    struct sockaddr sa;
    struct sockaddr_in sin;
} sock_union;

typedef struct len_and_sockaddr {
    socklen_t len;
    sock_union u;
} len_and_sockaddr;

enum {
    LSA_LEN_SIZE = offsetof(len_and_sockaddr,u),
    LSA_SIZEOF_SA = sizeof(sock_union)
}; 

typedef struct tagFtpClintPara
{
    const char *user;
    const char *password;
    struct len_and_sockaddr *lsa;
    FILE *control_stream;
    int verbose_flag;
    int do_continue;
    char buf[BUFSIZ];
}FtpClintPara;


class VmInfoOfDLC
{
public:
    VmInfoOfDLC()
    {
        _state = STORAGE_VM_STATE_DEPLOY;
        _vid = INVALID_VID;
        _pid = -1;
    }
    VmInfoOfDLC(int state, int64 vid, pid_t pid, const VmDiskConfig &disk, const ImageStoreOption &opt)
    :_state(state),_vid(vid),_pid(pid),_disk(disk),_opt(opt)
    {
    }
    int _state;        /*虚拟机在镜像处理过程中的状态*/
    int64 _vid;        /*虚拟机id*/
    pid_t _pid;        /*映像直接使用模式下记录的下载进程pid*/
    VmDiskConfig _disk;
    ImageStoreOption _opt;
};
class DownLoadCtl
{
public:
    DownLoadCtl()
    {
        _iid = INVALID_FILEID;
        _vid = INVALID_VID;
        _pid = -1;
        _refcount = 0;
        _finish = false;
    }
    DownLoadCtl(int64 iid, int64 vid, pid_t pid, int refcount, bool finish)
    :_iid(iid),_vid(vid),_pid(pid),_refcount(refcount),_finish(finish)
    {
    }
    int64 _iid;                   /*映像id*/
    int64 _vid;                   /*负责下载该镜像的虚拟机id*/
    pid_t _pid;                   /*下载映像进程pid*/
    int _refcount;                /*等待该映像下载的虚拟机数*/
    bool _finish;                 /*映像是否下载完成 true-完成 false-未完成*/
    vector<VmInfoOfDLC> _vminfo;  /*等待该映像下载的虚拟机队列*/
};
class UploadCtl
{
public:
    UploadCtl()
    {
        _vid    = INVALID_VID;
        _finish = 0;
        _state  = UPLOAD_STATE_INIT;
    }
    UploadCtl(int64 vid, int64 finish, int state, const VmSaveReq &req, const VmDiskConfig &disk, const ImageStoreOption &opt)
    :_vid(vid),_finish(finish),_state(state),_req(req),_disk(disk),_opt(opt)
    {
    }
    int64 _vid;
    int64 _finish;
    int   _state;
    VmSaveReq _req;
    VmDiskConfig _disk;
    ImageStoreOption _opt;
};
class StorageOperation
{
public:
    static StorageOperation *GetInstance()
    {
        if (NULL == _instance)
        {
            _instance = new StorageOperation();
        }
        return _instance;
    }
    int get_diskstate(vector<DiskStateInfo> &state_info);
    int get_vginfo(TVGSpaceTotalInfo &vginfo);
    int scan_storage(int64 vid,vector<VmDiskConfig> &disks,const ImageStoreOption &opt);
    void fail_storage(int64 iid,int64 vid,bool isalone);
    void cancel_storage(int64 iid,int64 vid);
    int get_storage(int64 vid,VmDiskConfig &disk,const ImageStoreOption &opt);
    int release_storage(int64 vid);
    int release_storage(int64 vid,const VmDiskConfig &disk,const ImageStoreOption &opt);
    void query_storage(int64 vid,VmDiskConfig &disk,const ImageStoreOption &opt,int64 &size,int &state);
    void print_download(void);
    void print_upload(void);
    int start_upload_img(int64 vid,const VmSaveReq &req,const VmDiskConfig disk,const ImageStoreOption opt);
    int stop_upload_img(int64 vid,const VmDiskConfig disk);
    int cancel_upload_img(int64 vid,const VmDiskConfig disk);
    int query_upload_img(int64 vid,const VmDiskConfig disk,int64 &finish,int &state);
    int recover_image(int64 vid, VmDiskConfig &disk);
    int CreateImgBackup(VmImageBackupReq &req);
    int DeleteImgBackup(VmImageBackupReq &req);
    int init(void);
protected:
    StorageOperation();
    virtual ~StorageOperation(); 
private: 
    friend class ImgDownLoadTask;
    friend class ImgUpLoadTask;
    
    void lock_download(void)
    {
        _download_mutex.Lock();
    }
    void unlock_download(void)
    {
        _download_mutex.Unlock();
    }
    void lock_upload(void)
    {
        _upload_mutex.Lock();
    }
    void unlock_upload(void)
    {
        _upload_mutex.Unlock();
    }

    int sys_rm(const string &file);
    int sys_rmdir(const string &dir);
    int sys_mkdir(const string &dir);
    int sys_umount(const string &filename,const string &option="");
    int sys_mount(const string &fsname,const string &mountpoint,const string &fstype="ext4");
    bool file_isexist(const string &file);
    int inc_upload(int64 vid,const VmSaveReq &req,const VmDiskConfig disk,const ImageStoreOption opt);
    int dec_upload(int64 vid,const VmDiskConfig disk);
    int stop_upload(int64 vid,const VmDiskConfig disk);
    int update_upload(int64 vid,const VmDiskConfig disk,int64 finish);
    int update_upload(int64 vid,const VmDiskConfig disk,int state);
    int check_upload(int64 vid,const VmDiskConfig disk,UploadCtl &ulc);
    bool iscancel_upload(int64 vid,const VmDiskConfig disk);
    int inc_download(int64 iid, int64 vid, bool &first,const VmDiskConfig &disk,const ImageStoreOption &opt);
    int dec_download(int64 iid, int64 vid);
    int update_download(int64 iid,int64 vid,pid_t pid,bool finish);
    int update_download(int64 iid,const VmInfoOfDLC &vminfo);
    int check_download(int64 iid,VmInfoOfDLC &vminfo);
    int release_download(int64 vid,const VmDiskConfig &disk,const ImageStoreOption &opt);      
    void stop_download(int64 vid,const VmDiskConfig &disk,const ImageStoreOption &opt);
    int clear_download(int64 vid,const VmDiskConfig &disk,const ImageStoreOption &opt);
    bool iscancel_download(int64 iid,int64 vid);
    void kill_download(void);
    void kill_download(int64 vid,const VmDiskConfig &disk,const ImageStoreOption &opt);
    void kill_checksum(int64 vid,const VmDiskConfig &disk,const ImageStoreOption &opt);
    void kill_checksum(void);
    int free_loop(int64 vid);
    int free_tap(int64 vid);
    int free_imglv(const string &imglv);
    int free_imglv(int64 vid,const VmDiskConfig &disk);
    int recover_imglv(int64 vid,const VmDiskConfig &disk);
    int check_lv_exist(int64 iid, int basefree);
    void clear_disk(VmDiskConfig &disk);
    bool is_usesnapshot(const VmDiskConfig &disk, const ImageStoreOption &opt);
    bool is_imageformatraw(const string& img);
    int check_imgvalid(const string &name,const VmDiskConfig &disk,bool need = true);
    int img_source_file(int64 vid,const VmDiskConfig &disk,const string &target,string &imgname);
    int fill_disk(const string &lv,int64 &vid,VmDiskConfig &disk,const ImageStoreOption &opt);
    void find_disk(const string &tag,vector<VmDiskConfig> &disks,const ImageStoreOption &opt);
    int rename_freelv(int64 iid);   
    int rename_baselv(void);
    int create_lv(const string &lv,int64 size,const string &base,int64 id,string &res);
    int alloc_lv(int64 vid,VmDiskConfig& disk,const ImageStoreOption& opt);
    void calc_lvsize(int64 size,int exp,int64 &lvsize);
    bool check_lvsize(const string &lv,int64 size,int factor);
    bool check_lvvalid(string lv,int64 size,int exp);   
    int load_share(int64 vid,VmDiskConfig &disk);
    int unload_share(int64 vid,const VmDiskConfig &disk);
    int clear_tmpresource(int64 vid,const VmDiskConfig &disk,const ImageStoreOption &opt);
    int clear_nfsmount(void);
    int clear_nfsmount(int64 id);
    void make_distname(int64 vid,const VmDiskConfig &disk,const string &prefix,string &name);
    void make_distdirname(int64 vid,const VmDiskConfig &disk,const string &prefix,string &name);
    int make_basename(int64 iid,string & baseimgdir,string &baseimg);
    int make_imgname(int64 vid,const VmDiskConfig &disk,string & imgdir,string &img);
    void make_imglvname(int64 vid,const VmDiskConfig &disk,bool fullname,string &imglv);
    int assign_work(int64 vid,const VmDiskConfig& disk,const ImageStoreOption& opt);
    int cancel_work(int64 vid,const VmDiskConfig &disk);
    bool find_work(int64 vid,const VmDiskConfig &disk);
    int assign_upload_work(int64 vid,const VmSaveReq &req,const VmDiskConfig& disk,const ImageStoreOption& opt);
    int cancel_upload_work(int64 vid,const VmDiskConfig &disk);

    //map<int64,  Mutex *> _mutexs;
    //Mutex _map_mutex;
    vector<DownLoadCtl> _download_ctl;
    Mutex _download_mutex;
    vector<UploadCtl> _upload_ctl;
    Mutex _upload_mutex;
    ThreadPool *_thread_pool;
    ThreadPool *_thread_pool_upload;
    static StorageOperation *_instance;
};
class ImgDownLoadTask:public Worktodo
{
public:
    ImgDownLoadTask(const string& name,int64 vid,const VmDiskConfig& disk,const ImageStoreOption& opt):
            Worktodo(name),_vid(vid),_disk(disk),_opt(opt)
    {};
    STATUS virtual Execute()
    {
        STATUS ret = ERROR;
        
        StorageOperation *pInst=StorageOperation::GetInstance();
        if (NULL != pInst)
        {
            _inst = pInst;
            ret = img_to_lv();
        }
        return ret;
    } 
    bool check_imgintegrity(const string &srctype,const string &name);
    int recycle_lv(const string &lv);
    int do_curl(const string &source, const string &target);
    int do_nfsmount(string &mountpoint);
    int download_use_nfs(const string &target);
    int download_use_http(const string &target);
    int download_by_url(const string &target);
    int prepare_target(const string &device,string &target,string &imgdir);
    int download_file(const string &target);
    int create_tmplv(const string &baselv,int64 size,string &res);
    int create_reservedlv(void);
    int img_bingo(int64 vid,int64 iid,const string &lv,const string &name,const ImageStoreOption &opt);
    int create_snapshot(int64 vid,const VmDiskConfig &disk,const ImageStoreOption &opt);
    int create_allsnapshot(void);
    int use_img_direct_local(void);
    int use_img_direct_share(void);
    int use_img_direct(void);
    int use_img_snapshot(void);
    int img_to_lv(void);
private:
    DISALLOW_COPY_AND_ASSIGN(ImgDownLoadTask);
    int64 _vid;             /*虚拟机id*/
    VmDiskConfig _disk;     /*虚拟盘*/
    ImageStoreOption _opt;   /*存储选项*/
    StorageOperation *_inst;
};

class ImgUpLoadTask:public Worktodo
{
public:
    ImgUpLoadTask(const string &name,int64 vid,const VmSaveReq& req,const VmDiskConfig& disk,const ImageStoreOption& opt):
            Worktodo(name),_vid(vid),_req(req),_disk(disk),_opt(opt)
    {};
    STATUS virtual Execute()
    {
        STATUS ret = ERROR;
        
        StorageOperation *pInst=StorageOperation::GetInstance();
        if (NULL != pInst)
        {
            _inst = pInst;
            ret = img_from_lv();
        }
        return ret;
    } 
    void set_nport(struct sockaddr *sa, unsigned port);
    len_and_sockaddr* str2sockaddr(const char *host, int port,int ai_flags);
    len_and_sockaddr* xhost2sockaddr(const char *host, int port);
    int xsocket(int domain, int type, int protocol);
    int xconnect(int s, const struct sockaddr *s_addr, socklen_t addrlen);
    int xconnect_stream(const len_and_sockaddr *lsa);
    int ftpcmd(const char *s1, const char *s2 ,FtpClintPara &para);
    int ftp_login(FtpClintPara &para);
    ssize_t safe_write(int fd, const void *buf, size_t count);
    ssize_t full_write(int fd, const void *buf, size_t len);
    ssize_t safe_read(int fd, void *buf, size_t count);
    int64 bb_full_fd_action(int src_fd, int dst_fd, off_t size, FtpClintPara &para);
    int64 bb_copyfd_eof(int fd1, int fd2, FtpClintPara &para);
    int pump_data_and_QUIT(int from, int to, FtpClintPara &para);
    int xconnect_ftpdata(FtpClintPara &para);
    int ftp_send(const char *server_path, const char *local_path, FtpClintPara &para);
    int ftp_client(const char *host, 
                    const char *server_path, 
                    const char *local_path,
                    const char *user, 
                    const char *password, 
                    int port);
    int upload_use_nfs(const string &target);
    int upload_use_http(const string &target);
    int upload_use_ftp(const string &target);
    int upload_by_url(const string &target);
    int upload_file(void);
    int img_from_lv(void);
private:
    DISALLOW_COPY_AND_ASSIGN(ImgUpLoadTask);
    int64 _vid;             /*虚拟机id*/
    VmSaveReq _req;         /*镜像上传请求*/
    VmDiskConfig _disk;     /*虚拟盘*/
    ImageStoreOption _opt;   /*存储选项*/
    StorageOperation *_inst;
};


/****************************************************************************************
*                                  函数申明                                             *
****************************************************************************************/
#endif

