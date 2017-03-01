/******************************************************************************
* Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：image_cache.h
* 文件标识：
* 内容摘要：image缓存数据库表管理模块头文件
* 当前版本：1.0
* 作    者：张文剑
* 完成日期：2012年3月12日
*
* 修改记录1：
*     修改日期：2012/3/12
*     版 本 号：V1.0
*     修 改 人：张文剑
*     修改内容：创建
******************************************************************************/
#ifndef TECS_IMAGE_CACHE_H
#define TECS_IMAGE_CACHE_H
#include "sky.h"
#include "vm_messages.h"
#include "pool_sql.h"
#include "image_url.h"

namespace zte_tecs
{

#define MAX_SNAPSHOT_PER_CACHE 100    //一个cache在一个集群内最大快照数

enum ImageCacheState
{
    ICS_NOSTATE = 0,
    ICS_DOWNLOADING = 1,
    ICS_READY = 2,
    ICS_ERROR = 3
};

inline const char* ImageCacheStateStr(int state)
{
#define ENUM2STR(value) case(value): return #value;
    switch((ImageCacheState)state)
    {
        ENUM2STR(ICS_NOSTATE);
        ENUM2STR(ICS_DOWNLOADING);
        ENUM2STR(ICS_READY);
        ENUM2STR(ICS_ERROR);
        default:return "unknown";
    }
};

enum ImageCacheCheckPoint
{
    ICCP_INIT = 0,
    ICCP_DEPLOY_START = 1,
    ICCP_DEPLOY_CREATE = 2,
    ICCP_DEPLOY_AUTH = 3,
    ICCP_DEPLOY_LOAD = 4,
    ICCP_DEPLOY_MOUNT = 5,
    ICCP_DEPLOY_DOWNLOAD = 6,
    ICCP_DEPLOY_SIGNATURE = 7,
    ICCP_DEPLOY_UMOUNT = 8,
    ICCP_DEPLOY_UNLOAD = 9,
    ICCP_DEPLOY_FINISH = 10,
    ICCP_CANCEL_START = 11,
    ICCP_CANCEL_DOWNLOAD = 12,
    ICCP_CANCEL_SIGNATURE = 13,
    ICCP_CANCEL_UMOUNT = 14,
    ICCP_CANCEL_UNLOAD = 15,
    ICCP_CANCEL_DEAUTH = 16,
    ICCP_CANCEL_DELETE = 17,
    ICCP_CANCEL_FINISH = 18,
    ICCP_UNKNOWN = 19
};

inline const char* ImageCacheCheckPointStr(int check_point)
{
#define ENUM2STR(value) case(value): return #value;
    switch((ImageCacheCheckPoint)check_point)
    {
        ENUM2STR(ICCP_INIT);
        ENUM2STR(ICCP_DEPLOY_START);
        ENUM2STR(ICCP_DEPLOY_CREATE);
        ENUM2STR(ICCP_DEPLOY_AUTH);
        ENUM2STR(ICCP_DEPLOY_LOAD);
        ENUM2STR(ICCP_DEPLOY_MOUNT);
        ENUM2STR(ICCP_DEPLOY_DOWNLOAD);
        ENUM2STR(ICCP_DEPLOY_SIGNATURE);
        ENUM2STR(ICCP_DEPLOY_UMOUNT);
        ENUM2STR(ICCP_DEPLOY_UNLOAD);
        ENUM2STR(ICCP_DEPLOY_FINISH);
        ENUM2STR(ICCP_CANCEL_START);
        ENUM2STR(ICCP_CANCEL_DOWNLOAD);
        ENUM2STR(ICCP_CANCEL_SIGNATURE);
        ENUM2STR(ICCP_CANCEL_UMOUNT);
        ENUM2STR(ICCP_CANCEL_UNLOAD);
        ENUM2STR(ICCP_CANCEL_DEAUTH);    
        ENUM2STR(ICCP_CANCEL_DELETE);
        ENUM2STR(ICCP_CANCEL_FINISH);
        ENUM2STR(ICCP_UNKNOWN);    
        default:return "ICCP_UNKNOWN";
    }
};


enum ImageCacheFields
{
    IMAGEID = 0,
    FILEURL = 1,
    SIZE = 2,
    SOURCE_FILE = 3,
    TARGET_FILE = 4,
    CACHED_AT = 5,
    STATE = 6,
    CACHE_ID = 7,
    POSITION = 8,
    PROGRESS = 9,
    REQUEST_ID = 10,
    VOLUME_UUID = 11,
    TARGET_URL = 12,
    LIMIT = 13
};

class ImageCache
{
public:
    ImageCache()
    {
        _cache_id = INVALID_OID;
        _image_id = INVALID_OID;
        _size = 0;
        _state = ICS_NOSTATE;
        _position = 0;
        _progress = 0;
    }

    ImageCache(int64 cache_id)
    {
        _cache_id = cache_id;
        _image_id = INVALID_FILEID;
        _size = 0;
        _state = ICS_NOSTATE;
        _position = 0;
        _progress = 0;
    }
    
    ImageCache(int64 cache_id, const VmDiskConfig& image)
    {
        _cache_id = cache_id;
        _image_id = image._id;
        _size = image._size;
        _url = image._url;
        _state = ICS_NOSTATE;
        _progress = 0;
        _position = image._position;
    }

    friend ostream & operator << (ostream& os, const ImageCache& cache)
    {
        os << "id: " << cache._cache_id << endl;
        os.setf(ios::fixed); 
        if(cache._size > 1024 * 1024)
        {
            os << setprecision(0) << 
                "size: " << BytesToMB(cache._size) <<" M" << endl;
        }
        else
        {
            os << "size: " << cache._size <<" bytes" << endl;
        }
        os.unsetf(ios::fixed); 
         os << "source: " << cache._source_file << endl;
        os << "target: " << cache._target_file << endl;
        os << "cached: " << cache._cached_at << endl;       
        os << "state: " << ImageCacheStateStr(cache._state) << endl;
        return os;
    };
    
    int64    _image_id;
    FileUrl  _url;
    int64    _size;
    string   _source_file;
    string   _target_file;
    string   _cached_at;
    int      _state;
    int64    _cache_id;
    int      _position;
    int      _progress;
    string   _request_id;
    string   _volume_uuid;
    FileUrl  _target_url;
};

class ImageCachePool: public SqlCallbackable
{
public:
    static ImageCachePool* GetInstance()
    {
        return instance;
    };

    static ImageCachePool* CreateInstance(SqlDB *db)
    {
        if(NULL == instance)
        {
            instance = new ImageCachePool(db); 
        }
        
        return instance;
    };
    
    virtual ~ImageCachePool() {};
    STATUS Init();
    STATUS CacheImage(const ImageCache& cache,const ImageStoreOption& option);
    int GetCacheById(ImageCache& cache);
    int GetCacheByImageId(vector<ImageCache>& ic,int64 image_id);
    int GetCacheByState(vector<ImageCache>& ics,ImageCacheState state,int limit);
    int GetCacheNotReady(vector<ImageCache>& ics,int limit);
    int GetAllCaches(vector<ImageCache>& ics,int limit);
    STATUS Insert(const ImageCache& cache);
    STATUS Update(const ImageCache& cache);
    STATUS Delete(int64 id);
    STATUS Delete(const string& file);
    int64 GetTotalCachedSize();
    int GetTotalCachedNum();
    bool IsDownloading(int64 iid);
    pid_t GetChildPid(int64 id);
    int UpdateProgress(const ImageCache& cache);
    int UpdateState(const ImageCache& cache);
    int UpdateStateAndProgress(const ImageCache& cache);
    int UpdateTarget(const ImageCache& cache);
    int UpdateVolumeUuid(const ImageCache& cache);
    int UpdateTargetUrl(const ImageCache& cache);

private:    
    int SelectCallback(void *nil, SqlColumn *columns);
    ImageCachePool(SqlDB *pDb);
    DISALLOW_COPY_AND_ASSIGN(ImageCachePool);
    static ImageCachePool *instance;
    SqlDB *db;
    ThreadPool *tpool;
};
//映像代理类的基类
class ImageAgentImpl
{
public:
    virtual ~ImageAgentImpl() {};
    
    virtual STATUS Init() = 0;
    
    //根据tc提供的映像信息提供映射后的url给hc使用
    virtual STATUS GetMappedUrl(ImageCache& cache,string& error) = 0;
    
    //缓存一个映像到cc本地之前要做的准备，子类肯定需要重载本函数
    virtual STATUS CachePrepare(ImageCache& cache) = 0;
    
    //缓存一个映像到cc本地之后要执行的清理操作，子类肯定需要重载本函数
    virtual STATUS CacheRelease(ImageCache& cache) = 0;

    //上电时检查所有缓存是否可用，在主备倒换场合是必需的!!!
    STATUS VerifyCache();
    
    //检查某个cache是否真的ready了，即缓存文件的大小和checksum是否正确
    //但是缓存到普通文件和缓存到块设备上的检查方式是不一样的，子类可能要重载本虚函数
    //virtual bool CheckCache(const ImageCache& cache);

    //删除已经下载的cache文件，大多数普通文件的删除方式基类可以实现
    //但是缓存到普通文件和缓存到块设备上的删除方式是不一样的，子类可能要重载本虚函数
    //virtual STATUS DeleteCache(const ImageCache& cache);
    string _cache_dir;
    ImageStoreOption _option;
};

//HttpdIAImpl的配置参数
class HttpdIAConfig
{
public:
    string www_base_url;    //对hc暴露的http下载服务ip地址
    string www_root_dir;    //web服务器文档下载根路径
    string www_proxy_url;
    int16  www_proxy_port;
    bool auto_cache;
};

//向hc提供http下载方式的映像代理实现类
class HttpdIAImpl:public ImageAgentImpl
{
public:
    HttpdIAImpl(const HttpdIAConfig& config);
    ~HttpdIAImpl() {};
    STATUS Init();
    STATUS GetMappedUrl(ImageCache& cache,string& error);
    STATUS CachePrepare(ImageCache& cache);
    STATUS CacheRelease(ImageCache& cache);
private:
    HttpdIAImpl(); //禁止不带指定参数构造
    HttpdIAConfig _config;
};
//NfsIAImpl的配置参数
class NfsIAConfig
{
public:
    string server_ip;       //对hc暴露的nfs服务ip地址
    string root_dir;        //用于缓存的路径
    bool auto_cache;        //是否自动缓存cc所用到的映像
};

//向hc提供nfs下载方式的映像代理实现类
class NfsIAImpl:public ImageAgentImpl
{
public:
    NfsIAImpl(const NfsIAConfig& config);
    ~NfsIAImpl() {};
    STATUS Init();
    STATUS GetMappedUrl(ImageCache& cache,string& error);
    STATUS CachePrepare(ImageCache& cache);
    STATUS CacheRelease(ImageCache& cache);
private:
    STATUS GetLocalPath(ImageCache& url,string& localdir,string& localfile);
    NfsIAImpl(); //禁止不带指定参数构造
    NfsIAConfig _config;
};


int CheckCacheDownloading(const ImageCache &cache);
int AllocateCache(const ImageCache &cache);
int DeleteCacheFromDb(const ImageCache &cache);
int UpdateCache(const ImageCache &cache);
int UpdateCacheState(const ImageCache &cache);
int UpdateCacheTarget(const ImageCache &cache);
int UpdateCacheProgress(const ImageCache &cache);
int UpdateCacheStateAndProgress(const ImageCache &cache);
int UpdateCacheVolumeUuid(const ImageCache &cache);
int UpdateCacheTargetUrl(const ImageCache &cache);
int GetCacheByCacheId(int64 id, ImageCache &cache);
int SelectCacheByImageId(int64 id, vector<ImageCache> &vec_ic);
int64 GetTotalCachedSizeFromDb(void);
int GetTotalCachedNumFromDb(void);
pid_t GetCacheChildPid(int64 cache_id);
STATUS DoCacheImage(const ImageCache& cache,const ImageStoreOption& option);




}
#endif




