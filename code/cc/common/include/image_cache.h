/******************************************************************************
* Copyright (c) 2012������ͨѶ�ɷ����޹�˾��All rights reserved.
* 
* �ļ����ƣ�image_cache.h
* �ļ���ʶ��
* ����ժҪ��image�������ݿ�����ģ��ͷ�ļ�
* ��ǰ�汾��1.0
* ��    �ߣ����Ľ�
* ������ڣ�2012��3��12��
*
* �޸ļ�¼1��
*     �޸����ڣ�2012/3/12
*     �� �� �ţ�V1.0
*     �� �� �ˣ����Ľ�
*     �޸����ݣ�����
******************************************************************************/
#ifndef TECS_IMAGE_CACHE_H
#define TECS_IMAGE_CACHE_H
#include "sky.h"
#include "vm_messages.h"
#include "pool_sql.h"
#include "image_url.h"

namespace zte_tecs
{

#define MAX_SNAPSHOT_PER_CACHE 100    //һ��cache��һ����Ⱥ����������

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
//ӳ�������Ļ���
class ImageAgentImpl
{
public:
    virtual ~ImageAgentImpl() {};
    
    virtual STATUS Init() = 0;
    
    //����tc�ṩ��ӳ����Ϣ�ṩӳ����url��hcʹ��
    virtual STATUS GetMappedUrl(ImageCache& cache,string& error) = 0;
    
    //����һ��ӳ��cc����֮ǰҪ����׼��������϶���Ҫ���ر�����
    virtual STATUS CachePrepare(ImageCache& cache) = 0;
    
    //����һ��ӳ��cc����֮��Ҫִ�е��������������϶���Ҫ���ر�����
    virtual STATUS CacheRelease(ImageCache& cache) = 0;

    //�ϵ�ʱ������л����Ƿ���ã����������������Ǳ����!!!
    STATUS VerifyCache();
    
    //���ĳ��cache�Ƿ����ready�ˣ��������ļ��Ĵ�С��checksum�Ƿ���ȷ
    //���ǻ��浽��ͨ�ļ��ͻ��浽���豸�ϵļ�鷽ʽ�ǲ�һ���ģ��������Ҫ���ر��麯��
    //virtual bool CheckCache(const ImageCache& cache);

    //ɾ���Ѿ����ص�cache�ļ����������ͨ�ļ���ɾ����ʽ�������ʵ��
    //���ǻ��浽��ͨ�ļ��ͻ��浽���豸�ϵ�ɾ����ʽ�ǲ�һ���ģ��������Ҫ���ر��麯��
    //virtual STATUS DeleteCache(const ImageCache& cache);
    string _cache_dir;
    ImageStoreOption _option;
};

//HttpdIAImpl�����ò���
class HttpdIAConfig
{
public:
    string www_base_url;    //��hc��¶��http���ط���ip��ַ
    string www_root_dir;    //web�������ĵ����ظ�·��
    string www_proxy_url;
    int16  www_proxy_port;
    bool auto_cache;
};

//��hc�ṩhttp���ط�ʽ��ӳ�����ʵ����
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
    HttpdIAImpl(); //��ֹ����ָ����������
    HttpdIAConfig _config;
};
//NfsIAImpl�����ò���
class NfsIAConfig
{
public:
    string server_ip;       //��hc��¶��nfs����ip��ַ
    string root_dir;        //���ڻ����·��
    bool auto_cache;        //�Ƿ��Զ�����cc���õ���ӳ��
};

//��hc�ṩnfs���ط�ʽ��ӳ�����ʵ����
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
    NfsIAImpl(); //��ֹ����ָ����������
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




