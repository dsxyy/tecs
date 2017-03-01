/******************************************************************************
* Copyright (c) 2012������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�image_cache.cpp
* �ļ���ʶ��
* ����ժҪ��image�������ݿ�����ģ��ʵ���ļ�
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
#include "db_config.h"
#include "image_cache.h"
#include "log_agent.h"
#include "storage.h"
#include <sys/types.h>

static int image_cache_print_on = 0;
DEFINE_DEBUG_VAR(image_cache_print_on);
#define Debug(level,fmt,arg...) \
        do \
        { \
            if(image_cache_print_on) \
            { \
                printf("[%s:%d]"fmt,__func__,__LINE__, ##arg); \
            } \
            OutPut(level,fmt,##arg);\
        }while(0)

namespace zte_tecs
{
#define MAX_DOWNLOAD_TASKS 5
const static string table_image_cache("image_cache");
const static string col_names_image_cache("image_id,file_url,size,source,target,cached_at,state,cache_id,position,progress,request_id,volume_uuid,target_url");
const static string db_bootstrap_image_cache("CREATE TABLE IF NOT EXISTS image_cache ("
        "image_id numeric(19) PRIMARY KEY,"           //ӳ��id
        "file_url varchar(1024),"                //ӳ���ļ���url��Ϣ�����л�֮����ַ���
        "size numeric(19),"                     //ӳ���ļ��Ĵ�С
        "source varchar(256) UNIQUE,"           //�������Դ�ļ�·����curl�ɶ���ʽ
        "target varchar(256) UNIQUE,"           //������cc�ϵ��ļ�·��
        "cached_at varchar(19),"                //����ʱ��
        "state integer)");                      //����״̬�������ڻ����У������Ѿ�����

class ImageCacheTask;
static map<string,ImageCacheTask*> _cache_tasks;
static Mutex _cache_tasks_mutex;

class ImageCacheTask:public Worktodo
{
public:
    ImageCacheTask(const string& name,const ImageCache& cache,const ImageStoreOption& option):
            Worktodo(name),_cache(cache),_option(option)
    {
        _work_thread_id = INVALID_THREAD_ID;
        _child_pid = -1;
    };

    ~ImageCacheTask()
    {
        //�ӹ���������ɾ��
        MutexLock lock (_cache_tasks_mutex);
        _cache_tasks.erase(getName());
    };
    pid_t get_child_pid(void)
    {
        return _child_pid;
    }
    void Print()
    {
        if (_work_thread_id == INVALID_THREAD_ID)
        {
            cout << "task " << getName() << " is still waiting in queue." << endl;
            return;
        }

        cout << "cache " << _cache._cache_id << " downloading task " << getName() << " id = 0x" << hex << _work_thread_id << endl;
        cout << "source file: " << _cache._source_file << endl;
        cout << "download to: " << _cache._target_file << endl;
        cout << "source size : " << dec << _cache._size << " bytes" << endl;
        
        struct stat64 s;
        if (0 == stat64(_cache._target_file.c_str(),&s))
        {
            cout << "target size : " << dec << s.st_size << " bytes" << endl;
            cout << "finished: "<< setw(2) << (s.st_size*100)/_cache._size << "%" << endl;
        }
        //��Datetime��ȡ����ʱ���ַ���
        cout << "download from local time: " << _begin_at.tostr(false) << endl;
    };

    int do_system(const string &command)
    {
        int status = ERROR;
        pid_t pid = -1;

        if (command.empty())
        {
            return status;
        }

        if ((pid = fork()) < 0)
        {
            return status;
        }
        else if (0 == pid)
        {
            execl("/bin/sh","sh","-c",command.c_str(),(char *)0);
        }
        else
        {
            _child_pid = pid;
            while(waitpid(pid,&status,0) < 0)
            {
                if (EINTR == errno)
                {
                    status = ERROR;
                    break;
                }
            }
            _child_pid = -1;
        }

        return status;
    }
    
    STATUS virtual Execute()
    {
        string    checksum;
        _work_thread_id = pthread_self();
        _begin_at.refresh();

        if (_cache._source_file.empty() || _cache._target_file.empty())
        {
            SkyAssert(false);
            return ERROR_INVALID_ARGUMENT;
        }

        //�����ݿ��н��û����״̬��ʶΪ����������
        ImageCachePool* cache_pool = ImageCachePool::GetInstance();
        SkyAssert(NULL != cache_pool);
        _cache._state = ICS_DOWNLOADING;
        cache_pool->UpdateState(_cache);

        //�ϵ�������ѡ��ʵ�ʲ����е����⣬�ݲ�ʹ��
        string command = "curl " + _cache._source_file + " -o " + _cache._target_file + " --create-dirs -s";
        Debug(SYS_NOTICE,"Task 0x%lx is running command: \"%s\"!\n",_work_thread_id,command.c_str());
        int ret = do_system(command.c_str());
        if (-1 == ret)
        {
            Debug(SYS_ERROR,"system command [%s] failed! errno = %d\n",command.c_str(),errno);
            return ERROR;
        }
        else if (0 != ret)
        {
            Debug(SYS_NOTICE,"system command [%s] returns %d\n",command.c_str(),ret);
        }

        //��黺���������ļ�size��checksum�Ƿ���ȷ�������ȷ���͸������ݿ�
        //ֻ����ͨ�ļ�(S_IFREG)���������壬�����ֱ�����ص����豸�����ǲ��ü���
        //ֻ�е��û�ָ�����ļ���Сʱ(0 < _cache._size)����������
        struct stat64 s;
        ret = stat64(_cache._target_file.c_str(),&s);       
        if (0 != ret || ((s.st_mode & S_IFREG) &&  0 < _cache._size && s.st_size != _cache._size))
        {
            if(ret != 0)
            {
                Debug(SYS_WARNING,"stat64 cache file %s failed! errno = %d\n",
                    _cache._target_file.c_str(),errno);
            }
            else
            {
                //����û��ṩ��һ���Ϸ���sizeֵ���򽫻�����ɵ��ļ���С��֮�Ƚ���֤
                Debug(SYS_WARNING,"cached image(%s) size = %lld, not equal to expected size = %lld!\n",
                    _cache._target_file.c_str(),s.st_size,_cache._size);
            }
            _cache._state = ICS_ERROR;
            cache_pool->UpdateState(_cache);
            //todo: ����ʧ������ô��? �澯�����Զ���������?
            return ERROR;
        }
        if((IMAGE_SOURCE_TYPE_FILE == _option._img_srctype) && ! _cache._url.checksum.empty())
        {
            OutPut(SYS_DEBUG,"call sys_md5 calc %s checksum,should %s\n",_cache._target_file.c_str(),_cache._url.checksum.c_str());
            if(SUCCESS == sys_md5(_cache._target_file,checksum))
            {
                /*2.1.1.1:md5����ɹ�,�ж�md5�Ƿ�һ��*/
                if (checksum != _cache._url.checksum)
                {
                    OutPut(SYS_ERROR,"%s checksum mismatch,should:%s,but:%s\n",getName().c_str(),_cache._url.checksum.c_str(),checksum.c_str());
                    return ERROR;
                }
            }
            else
            {
                /*2.1.1.2:md5����ʧ��*/
                OutPut(SYS_ERROR,"sys_md5 failed\n");
                return ERROR;
            }
        }
        
        //������ɣ��������ݿ⣬��ʶ�û����Ѿ�������
        Debug(SYS_NOTICE,"cached image(%s) size = %lld is ok!\n",
              _cache._target_file.c_str(),s.st_size);
        Datetime dt;
        dt.refresh();
        _cache._cached_at = dt.serialize();
        _cache._state = ICS_READY;
        cache_pool->UpdateState(_cache);
        return SUCCESS;
    }

private:
    DISALLOW_COPY_AND_ASSIGN(ImageCacheTask);
    ImageCacheTask():Worktodo("null")
    {};
    THREAD_ID _work_thread_id;
    //���ع��������Ĺ������ǽ�_source�������Դ�ļ�
    //����Ϊ�����ļ�_cache._cached_file
    //���У����size��checksum
    ImageCache _cache;
    ImageStoreOption _option;
    Datetime   _begin_at;
    pid_t      _child_pid;
};

ImageCachePool* ImageCachePool::instance = NULL;

ImageCachePool::ImageCachePool(SqlDB *pDb):SqlCallbackable(pDb)
{
    db = pDb;
}

STATUS ImageCachePool::Init()
{
    int ret = -1;

    //�����̳߳�����ӳ�������أ�Ŀǰʹ����5���̣߳��Ժ��б�Ҫ�Ļ���������
    _cache_tasks_mutex.Init();
    tpool = new ThreadPool(MAX_DOWNLOAD_TASKS);
    if (!tpool)
    {
        Debug(SYS_EMERGENCY,"failed to create ThreadPool!\n");
        return ERROR;
    }
    ret = tpool->CreateThreads("ImageDownloader");
    if (SUCCESS != ret)
    {
        Debug(SYS_EMERGENCY,"failed to start ThreadPool threads! ret = %d\n",ret);
        return ret;
    }
    return ret;
}
pid_t ImageCachePool::GetChildPid(int64 id)
{
    MutexLock lock (_cache_tasks_mutex);
    if (_cache_tasks.empty())
    {
        return -1;
    }
    //ȷ���ǲ����Ѿ���ImageCacheTask��ʵʩ���ػ��Ŷ���
    string taskname = to_string<int64>(id,dec);
    map<string,ImageCacheTask*>::iterator it = _cache_tasks.find(taskname);
    if (it == _cache_tasks.end())
    {
        return -1;
    }
    return it->second->get_child_pid();
}
bool ImageCachePool::IsDownloading(int64 iid)
{
    MutexLock lock (_cache_tasks_mutex);
    if (_cache_tasks.empty())
    {
        return false;
    }
    //ȷ���ǲ����Ѿ���ImageCacheTask��ʵʩ���ػ��Ŷ���
    string taskname = to_string<int64>(iid,dec);
    map<string,ImageCacheTask*>::iterator it = _cache_tasks.find(taskname);
    if (it == _cache_tasks.end())
    {
        return false;
    }
    return true;
}

STATUS ImageCachePool::CacheImage(const ImageCache& cache,const ImageStoreOption& option)
{
    //�����������������ȼ��빤������
    string taskname = to_string<int64>(cache._cache_id,dec);
    ImageCacheTask* task = new ImageCacheTask(taskname,cache,option);
    if (!task)
    {
        return ERROR_NO_MEMORY;
    }
    _cache_tasks.insert(make_pair(taskname,task));

    //Ȼ�󽫹������뵽�̳߳ع�������
    Debug(SYS_NOTICE,"cache %lld(%s) downloading work is started!\n",cache._cache_id,cache._source_file.c_str());
    tpool->AssignWork(task);
    return SUCCESS;
}

STATUS ImageCachePool::Insert(const ImageCache& cache)
{
    SqlCommand sql(db,table_image_cache);
    sql.Add("image_id",cache._image_id);
    sql.Add("file_url",cache._url);
    sql.Add("size",cache._size);
    sql.Add("source",cache._source_file);
    sql.Add("target",cache._target_file);
    sql.Add("cached_at",cache._cached_at);
    sql.Add("state",cache._state);
    sql.Add("cache_id", cache._cache_id);
    sql.Add("position", cache._position);
    sql.Add("progress", cache._progress);
    sql.Add("request_id", cache._request_id);
    sql.Add("volume_uuid", cache._volume_uuid);
    sql.Add("target_url",cache._target_url);
    
    return sql.Insert();
}

STATUS ImageCachePool::Delete(int64 id)
{
    int ret = -1;
    ostringstream oss;
    oss << "DELETE FROM " << table_image_cache << " WHERE cache_id = " << id;
    ret = db->Execute(oss);
    if (ret != 0)
    {
        cerr << oss.str() << endl;
        SkyAssert(false);
        return ret;
    }
    return SUCCESS;
}

STATUS ImageCachePool::Delete(const string& file)
{
    //�����ļ�����ɾ�����棬��δʵ��
    SkyAssert(false);
    return ERROR_NOT_SUPPORT;
}

STATUS ImageCachePool::Update(const ImageCache& cache)
{
    SqlCommand sql(db,table_image_cache);
    sql.Add("image_id",cache._image_id);
    sql.Add("file_url",cache._url);
    sql.Add("size",cache._size);
    sql.Add("source",cache._source_file);
    sql.Add("target",cache._target_file);
    sql.Add("cached_at",cache._cached_at);
    sql.Add("state",cache._state);
    sql.Add("cache_id", cache._cache_id);
    sql.Add("position", cache._position);
    sql.Add("progress", cache._progress);
    sql.Add("request_id", cache._request_id);
    sql.Add("volume_uuid", cache._volume_uuid);
    sql.Add("target_url", cache._target_url);
    
    return sql.Update("WHERE cache_id = " + to_string<int64>(cache._cache_id,dec));
}

int ImageCachePool::SelectCallback(void *nil, SqlColumn *columns)
{
    if (!nil || !columns || LIMIT != columns->get_column_num())
    {
        return -1;
    }

    ImageCache cache;

    columns->GetValue(IMAGEID, cache._image_id);
    columns->GetValue(FILEURL, cache._url);
    columns->GetValue(SIZE, cache._size);
    columns->GetValue(SOURCE_FILE, cache._source_file);
    columns->GetValue(TARGET_FILE, cache._target_file);
    columns->GetValue(CACHED_AT, cache._cached_at);
    columns->GetValue(STATE, cache._state);
    columns->GetValue(CACHE_ID, cache._cache_id);
    columns->GetValue(POSITION, cache._position);
    columns->GetValue(PROGRESS, cache._progress);
    columns->GetValue(REQUEST_ID, cache._request_id);
    columns->GetValue(VOLUME_UUID, cache._volume_uuid);
    columns->GetValue(TARGET_URL, cache._target_url);
    
    vector<ImageCache>* ics = static_cast<vector<ImageCache> *>(nil);
    ics->push_back(cache);
    return 0;
}
int ImageCachePool::UpdateProgress(const ImageCache& cache)
{
    SqlCommand sql(db,table_image_cache);

    sql.Add("progress", cache._progress);
    
    return sql.Update("WHERE cache_id = " + to_string<int64>(cache._cache_id,dec));
}

int ImageCachePool::UpdateState(const ImageCache& cache)
{
    SqlCommand sql(db,table_image_cache);

    sql.Add("source",cache._source_file);
    sql.Add("cached_at",cache._cached_at);
    sql.Add("state",cache._state);
    
    return sql.Update("WHERE cache_id = " + to_string<int64>(cache._cache_id,dec));
}

int ImageCachePool::UpdateStateAndProgress(const ImageCache& cache)
{
    SqlCommand sql(db,table_image_cache);
    
    sql.Add("state",cache._state);
    sql.Add("progress", cache._progress);
    
    return sql.Update("WHERE cache_id = " + to_string<int64>(cache._cache_id,dec));
}

int ImageCachePool::UpdateTarget(const ImageCache& cache)
{
    SqlCommand sql(db,table_image_cache);
    
    sql.Add("target",cache._target_file);
    
    return sql.Update("WHERE cache_id = " + to_string<int64>(cache._cache_id,dec));
}

int ImageCachePool::UpdateVolumeUuid(const ImageCache& cache)
{
    SqlCommand sql(db,table_image_cache);
    
    sql.Add("volume_uuid",cache._volume_uuid);
    
    return sql.Update("WHERE cache_id = " + to_string<int64>(cache._cache_id,dec));
}

int ImageCachePool::UpdateTargetUrl(const ImageCache& cache)
{
    SqlCommand sql(db,table_image_cache);
    
    sql.Add("target_url",cache._target_url);
    
    return sql.Update("WHERE cache_id = " + to_string<int64>(cache._cache_id,dec));
}

int ImageCachePool::GetCacheById(ImageCache& cache)
{
    vector<ImageCache> ics;
    ostringstream sql;
    SetCallback(static_cast<Callbackable::Callback>(&ImageCachePool::SelectCallback),(void *)&ics);
    sql << "SELECT " << col_names_image_cache << " FROM " << table_image_cache
        << " WHERE cache_id = " << cache._cache_id;
    int ret = db->Execute(sql, this);
    UnsetCallback();
    if (SQLDB_RESULT_EMPTY == ret)
    {
        return ERROR_OBJECT_NOT_EXIST;
    }
    else if (SQLDB_ERROR == ret)
    {
        return ERROR;
    }
    cache = ics[0];
    return SUCCESS;
}
int ImageCachePool::GetCacheByImageId(vector<ImageCache>& ic,int64 image_id)
{
    ostringstream sql;
    SetCallback(static_cast<Callbackable::Callback>(&ImageCachePool::SelectCallback),(void *)&ic);
    sql << "SELECT " << col_names_image_cache << " FROM " << table_image_cache
        << " WHERE image_id = " << image_id;
    int ret = db->Execute(sql, this);
    UnsetCallback();
    return ret;
}
int ImageCachePool::GetCacheByState(vector<ImageCache>& ics,ImageCacheState state,int limit)
{
    ostringstream sql;
    SetCallback(static_cast<Callbackable::Callback>(&ImageCachePool::SelectCallback),(void *)&ics);
    sql << "SELECT " << col_names_image_cache << " FROM " << table_image_cache
        << " WHERE state = " << state
        << " LIMIT " << limit;
    int ret = db->Execute(sql, this);
    UnsetCallback();
    return ret;
}

int ImageCachePool::GetCacheNotReady(vector<ImageCache>& ics,int limit)
{
    ostringstream sql;
    SetCallback(static_cast<Callbackable::Callback>(&ImageCachePool::SelectCallback),(void *)&ics);
    sql << "SELECT " << col_names_image_cache << " FROM " << table_image_cache
        << " WHERE state != " << ICS_READY
        << " LIMIT " << limit;
    int ret = db->Execute(sql, this);
    UnsetCallback();
    return ret;
}

int ImageCachePool::GetAllCaches(vector<ImageCache>& ics,int limit)
{
    ostringstream sql;
    SetCallback(static_cast<Callbackable::Callback>(&ImageCachePool::SelectCallback),(void *)&ics);
    sql << "SELECT " << col_names_image_cache << " FROM " << table_image_cache
        << " ORDER BY cache_id ASC LIMIT " << limit;
    int ret = db->Execute(sql, this);
    UnsetCallback();
    return ret;
}

int64 ImageCachePool::GetTotalCachedSize()
{
    int64 val = 0;
    //��ȡ������ʱ������Ҫдcallback����
    int ret = SelectColumn(table_image_cache,"SUM(size)","",val);
    if(SQLDB_ERROR == ret)
    {
        return -1;
    }
    else if(SQLDB_RESULT_EMPTY == ret)
    {
        return 0;
    }
    return val;
}

int ImageCachePool::GetTotalCachedNum()
{
    int val = -1;
    //��ȡ������ʱ������Ҫдcallback����
    int ret = SelectColumn(table_image_cache,"COUNT(*)","",val);
    if(SQLDB_ERROR == ret)
    {
        return -1;
    }
    return val;
}
/******************************************************************************/
int CheckCacheDownloading(const ImageCache &cache)
{
    ImageCachePool *pInst = ImageCachePool::GetInstance();
    if (NULL == pInst)
    {
        OutPut(SYS_ERROR,"CheckCacheDownloading GetInstance Failed\n");
        return ERROR;
    }
    if (pInst->IsDownloading(cache._cache_id))
    {
        return SUCCESS;
    }
    return ERROR_OBJECT_NOT_EXIST;
}
/******************************************************************************/
int AllocateCache(const ImageCache &cache)
{
    int ret = ERROR;
    ImageCachePool *pInst = ImageCachePool::GetInstance();
    if (NULL == pInst)
    {
        OutPut(SYS_ERROR,"AllocateCache GetInstance Failed\n");
        return ret;
    }
    ret = pInst->Insert(cache);
    if (SUCCESS != ret)
    {
        OutPut(SYS_ERROR,"AllocateCache Insert Failed\n");
    }
    return ret;
}
/******************************************************************************/
int DeleteCacheFromDb(const ImageCache &cache)
{
    int ret = ERROR;
    ImageCachePool *pInst = ImageCachePool::GetInstance();
    if (NULL == pInst)
    {
        OutPut(SYS_ERROR,"DeleteCacheFromDb GetInstance Failed\n");
        return ret;
    }
    ret = pInst->Delete(cache._cache_id);
    if (SUCCESS != ret)
    {
        OutPut(SYS_ERROR,"DeleteCacheFromDb Insert Failed\n");
    }
    return ret;
}
/******************************************************************************/
int UpdateCache(const ImageCache &cache)
{
    int ret = ERROR;
    ImageCachePool *pInst = ImageCachePool::GetInstance();
    if (NULL == pInst)
    {
        OutPut(SYS_ERROR,"UpdateCache GetInstance Failed\n");
        return ret;
    }
    ret = pInst->Update(cache);
    if (SUCCESS != ret)
    {
        OutPut(SYS_ERROR,"UpdateCache Insert Failed\n");
    }
    return ret;
}
/******************************************************************************/
int UpdateCacheState(const ImageCache &cache)
{
    int ret = ERROR;
    ImageCachePool *pInst = ImageCachePool::GetInstance();
    if (NULL == pInst)
    {
        OutPut(SYS_ERROR,"UpdateCacheState GetInstance Failed\n");
        return ret;
    }
    ret = pInst->UpdateState(cache);
    if (SUCCESS != ret)
    {
        OutPut(SYS_ERROR,"UpdateCacheState Insert Failed\n");
    }
    return ret;
}
/******************************************************************************/
int UpdateCacheTarget(const ImageCache &cache)
{
    int ret = ERROR;
    ImageCachePool *pInst = ImageCachePool::GetInstance();
    if (NULL == pInst)
    {
        OutPut(SYS_ERROR,"UpdateCacheTarget GetInstance Failed\n");
        return ret;
    }
    ret = pInst->UpdateTarget(cache);
    if (SUCCESS != ret)
    {
        OutPut(SYS_ERROR,"UpdateCacheTarget Insert Failed\n");
    }
    return ret;
}
/******************************************************************************/
int UpdateCacheProgress(const ImageCache &cache)
{
    int ret = ERROR;
    ImageCachePool *pInst = ImageCachePool::GetInstance();
    if (NULL == pInst)
    {
        OutPut(SYS_ERROR,"UpdateCacheProgress GetInstance Failed\n");
        return ret;
    }
    ret = pInst->UpdateProgress(cache);
    if (SUCCESS != ret)
    {
        OutPut(SYS_ERROR,"UpdateCacheProgress Insert Failed\n");
    }
    return ret;
}
/******************************************************************************/
int UpdateCacheStateAndProgress(const ImageCache &cache)
{
    int ret = ERROR;
    ImageCachePool *pInst = ImageCachePool::GetInstance();
    if (NULL == pInst)
    {
        OutPut(SYS_ERROR,"UpdateCacheStateAndProgress GetInstance Failed\n");
        return ret;
    }
    ret = pInst->UpdateStateAndProgress(cache);
    if (SUCCESS != ret)
    {
        OutPut(SYS_ERROR,"UpdateCacheStateAndProgress Insert Failed\n");
    }
    return ret;
}
/******************************************************************************/
int UpdateCacheVolumeUuid(const ImageCache &cache)
{
    int ret = ERROR;
    ImageCachePool *pInst = ImageCachePool::GetInstance();
    if (NULL == pInst)
    {
        OutPut(SYS_ERROR,"UpdateCacheVolumeUuid GetInstance Failed\n");
        return ret;
    }
    ret = pInst->UpdateVolumeUuid(cache);
    if (SUCCESS != ret)
    {
        OutPut(SYS_ERROR,"UpdateCacheVolumeUuid Insert Failed\n");
    }
    return ret;
}
/******************************************************************************/
int UpdateCacheTargetUrl(const ImageCache &cache)
{
    int ret = ERROR;
    ImageCachePool *pInst = ImageCachePool::GetInstance();
    if (NULL == pInst)
    {
        OutPut(SYS_ERROR,"UpdateCacheTargetUrl GetInstance Failed\n");
        return ret;
    }
    ret = pInst->UpdateTargetUrl(cache);
    if (SUCCESS != ret)
    {
        OutPut(SYS_ERROR,"UpdateCacheTargetUrl Insert Failed\n");
    }
    return ret;
}
/******************************************************************************/
int GetCacheByCacheId(int64 id, ImageCache &cache)
{
    int ret = ERROR;
    ImageCachePool *pInst = ImageCachePool::GetInstance();
    if (NULL == pInst)
    {
        OutPut(SYS_ERROR,"GetCacheByCacheId GetInstance Failed\n");
        return ret;
    }

    cache._cache_id = id;
    ret = pInst->GetCacheById(cache);
    if (ERROR_OBJECT_NOT_EXIST == ret)
    {
        OutPut(SYS_DEBUG,"GetCacheByCacheId GetCacheById Not Exist\n");
    }
    else if (SUCCESS != ret)
    {
        OutPut(SYS_ERROR,"GetCacheByCacheId GetCacheById Failed\n");
    }
    
    return ret;
}
/******************************************************************************/
int SelectCacheByImageId(int64 id, vector<ImageCache> &vec_ic)
{
    int ret = ERROR;
    ImageCachePool *pInst = ImageCachePool::GetInstance();
    if (NULL == pInst)
    {
        OutPut(SYS_ERROR,"SelectCacheByImageId GetInstance Failed\n");
        return ret;
    }
    
    ret = pInst->GetCacheByImageId(vec_ic, id);
    if (SUCCESS != ret)
    {
        OutPut(SYS_ERROR,"SelectCacheByImageId Select Failed\n");
    }

    if (0 == vec_ic.size())
    {
        return ERROR_OBJECT_NOT_EXIST;
    }
    return SUCCESS;
}
/******************************************************************************/
int64 GetTotalCachedSizeFromDb(void)
{
    ImageCachePool *pInst = ImageCachePool::GetInstance();
    if (NULL == pInst)
    {
        OutPut(SYS_ERROR,"GetTotalCachedSizeFromDb GetInstance Failed\n");
        return -1;
    }

    return pInst->GetTotalCachedSize();
}
/******************************************************************************/
int GetTotalCachedNumFromDb(void)
{
    ImageCachePool *pInst = ImageCachePool::GetInstance();
    if (NULL == pInst)
    {
        OutPut(SYS_ERROR,"GetTotalCachedNumFromDb GetInstance Failed\n");
        return -1;
    }

    return pInst->GetTotalCachedNum();
}
/******************************************************************************/
pid_t GetCacheChildPid(int64 cache_id)
{
    ImageCachePool *pInst = ImageCachePool::GetInstance();
    if (NULL == pInst)
    {
        OutPut(SYS_ERROR,"GetTotalCachedNumFromDb GetInstance Failed\n");
        return -1;
    }
    return pInst->GetChildPid(cache_id);
}
/******************************************************************************/
STATUS DoCacheImage(const ImageCache& cache,const ImageStoreOption& option)
{
    ImageCachePool *pInst = ImageCachePool::GetInstance();
    if (NULL == pInst)
    {
        OutPut(SYS_ERROR,"DoCacheImage GetInstance Failed\n");
        return -1;
    }
    return pInst->CacheImage(cache,option);
}
/******************************************************************************/
void DbgShowCacheWorks()
{
    MutexLock lock (_cache_tasks_mutex);
    if (_cache_tasks.empty())
    {
        cout << "No download tasks!" << endl;
        return;
    }
    map<string,ImageCacheTask*>::iterator it;
    for (it = _cache_tasks.begin(); it != _cache_tasks.end(); it++)
    {
        cout << "------------------------------------------------------" << endl;
        ImageCacheTask* task = it->second;
        task->Print();
    }
}
DEFINE_DEBUG_CMD(downloading,DbgShowCacheWorks);

void DbgNoCache(int64 iid)
{
    ImageCachePool* cache_pool = ImageCachePool::GetInstance();
    SkyAssert(NULL != cache_pool);
    if(true == cache_pool->IsDownloading(iid))
    {
        cerr << "image " << iid << " is downloading, can not be deleted!" << endl;
        return;
    }
    
    ImageCache tmp(iid);
    STATUS ret = cache_pool->GetCacheById(tmp);
    if(ret == ERROR)
    {
        cerr << "GetImageCacheById failed!" << endl;
    }
    else if(ret == ERROR_OBJECT_NOT_EXIST)
    {
        cerr << "image " << iid << " is not cached!" << endl;
        return;
    }
    
    unlink(tmp._target_file.c_str());
    ret = cache_pool->Delete(iid);
    if(ret == SQLDB_ERROR)
    {
        cerr << "DeleteImageCache failed!" << endl;
    }
    else
    {
        cout << "Image " << iid << " is deleted from cache!" << endl;
    }
    return;
}
DEFINE_DEBUG_FUNC(DbgNoCache);

void DbgShowCaches()
{
    ImageCachePool* cache_pool = ImageCachePool::GetInstance();
    SkyAssert(NULL != cache_pool);
    vector<ImageCache> ics;
    int ret = cache_pool->GetAllCaches(ics,1000);
    if(SQLDB_ERROR == ret)
    {
        cout << "failed to get image cache info!" << endl;
        return;
    }
    else if(SQLDB_RESULT_EMPTY == ret)
    {
        cout << "No image cache record!" << endl;
        return;
    }
    
    vector<ImageCache>::iterator it;
    for (it = ics.begin(); it != ics.end(); it++)
    {
        cout << "------------------------------------------------------"
             << endl << *it;
    }
    return;
}
DEFINE_DEBUG_CMD(caches,DbgShowCaches);
}

