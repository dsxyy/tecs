#include "tecs_config.h"
#include "log_agent.h"
#include "image_store.h"
#include <sys/vfs.h> 

static int store_print_on = 0;
DEFINE_DEBUG_VAR(store_print_on);
#define Debug(level,fmt,arg...) \
        do \
        { \
            if(store_print_on) \
            { \
                printf("[%s:%d]"fmt,__func__,__LINE__, ##arg); \
            } \
            OutPut(level,fmt,##arg);\
        }while(0)

//使用nfsstore时支持的配置选项
#define DEFAULT_NFS_ROOT "/var/lib/tecs/image_repo"
static string nfs_server_ip;
static string nfs_server_dir(DEFAULT_NFS_ROOT);
//使用httpdstore时支持的配置选项
#define DEFAULT_WWW_ROOT "/var/www/html/tecs/image_repo"
static string httpd_base_url;
static string www_root_dir(DEFAULT_WWW_ROOT);

static string image_store_type(IMAGE_ACCESS_NFS); //默认使用nfs store

namespace zte_tecs
{
ImageStore* ImageStore::instance = NULL;

//获取一种store的实现类
static ImageStoreImpl* GetStoreImpl(const string& store_type)
{
    ImageStoreImpl* impl = NULL;
    if(store_type == IMAGE_ACCESS_NFS)
    {
        impl = new NfsStore(nfs_server_ip,nfs_server_dir);
    }
    else if(store_type == IMAGE_ACCESS_HTTP)
    {
        impl = new HttpdStore();
    }
    else
    {
        SkyAssert(false);
        SkyExit(-1,"invalid store type!");
    }
    return impl;
}

ImageStore::ImageStore() 
{
    _store_type = image_store_type;
    impl = GetStoreImpl(_store_type);
    if(!impl)
    {
        SkyAssert(false);
    }
};

ImageStore::ImageStore(const string& store_type):_store_type(store_type)
{
    impl = GetStoreImpl(store_type);
    if(!impl)
    {
        SkyAssert(false);
    }
};

STATUS NfsStore::GetSpace(int64 &freespace, int64 &totalspace)
{
    int64         temp;
    STATUS        ret;    
    struct statfs tStatfs;

    ret = statfs(DEFAULT_NFS_ROOT, &tStatfs);
    if (0 != ret)
    {
        cout << "GetSpace statfs() ERROR! errno=" << errno << endl;
        return ERROR;
    }

    temp = tStatfs.f_bavail % 1024;
    temp = (temp * tStatfs.f_bsize) / 1024;
    freespace = (tStatfs.f_bavail / 1024 ) * tStatfs.f_bsize + temp;

    temp = tStatfs.f_blocks % 1024;
    temp = (temp * tStatfs.f_bsize) / 1024;
    totalspace = (tStatfs.f_blocks / 1024 ) * tStatfs.f_bsize + temp;

    return SUCCESS;
};

STATUS EnableImageStoreOptions()
{
    TecsConfig *config = TecsConfig::Get();
    config->EnableCustom("image_store_type", image_store_type,"Image manager uses which store type for image? default = nfs.");
    config->EnableCustom("nfs_server_url", nfs_server_ip,"Image manager's nfs server url for clusters and hosts to download image.");
    config->EnableCustom("nfs_server_dir", nfs_server_dir,"Image manager's nfs root directory to store user's vm images, which is also ftpd root directory.");
    //config->EnableCustom("httpd_base_url", httpd_base_url,"Http base url for image manager to generate a http link.");
    //config->EnableCustom("www_root_dir", www_root_dir,"Image manager's http server root directory.");
    return SUCCESS;
}

STATUS ImageStoreInit()
{
    ImageStore* store = ImageStore::CreateInstance(image_store_type);
    if(!store)
    {
        SkyAssert(false);
        return ERROR_NOT_READY;
    }
    return store->Init();
}
}

