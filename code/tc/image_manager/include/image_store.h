#ifndef TECS_IMAGE_STORE_H
#define TECS_IMAGE_STORE_H
#include "sky.h"
#include "image_url.h"

namespace zte_tecs
{
///////////////////////////////////////////////////////////////////
class ImageStoreImpl
{
public:  
    virtual STATUS Init() = 0;
    virtual STATUS GetStorageDir(string& dir) = 0;
    virtual STATUS GetUrlByPath(const string& path,FileUrl& url) = 0;
    virtual STATUS GetUploadIp(string &ip) = 0;
    virtual STATUS AddFile(const string& file,FileUrl& url) = 0;
    virtual STATUS DeleteFile(const FileUrl& url) = 0;
    virtual STATUS GetSpace(int64 &freespace, int64 &totalspace) = 0;
    virtual ~ImageStoreImpl() {};
};

////////////////////////////////////////////////////////////////////
class ImageStore
{
public:   
    static ImageStore* CreateInstance(const string& store_type)
    {
        if(NULL == instance)
        {
            try
            {
                instance = new ImageStore(store_type); 
            }
            catch(...)
            {
                printf("exception happens when constructing ImageStore!\n");
                instance = NULL;
            }
        }

        return instance;
    }

    static ImageStore* GetInstance()
    {
        return instance;
    }
    
    ~ImageStore() 
    {
        instance = NULL;
        delete impl;
        impl = NULL;
    }

    const string& GetType()
    {
        return _store_type;
    }
    
    STATUS Init()
    {
        return impl->Init();
    }
    
    STATUS GetStorageDir(string& dir)
    {
        return impl->GetStorageDir(dir);
    }
    
    STATUS GetUrlByPath(const string& path,FileUrl& url)
    {
        return impl->GetUrlByPath(path,url); 
    }

    STATUS GetUploadIp(string &ip)
    {
        return impl->GetUploadIp(ip); 
    }
    
    STATUS AddFile(const string& file,FileUrl& url)
    {
        return impl->AddFile(file,url);
    }
    
    STATUS DeleteFile(const FileUrl& url)
    {
        return impl->DeleteFile(url);
    }
    
    STATUS GetSpace(int64 &freespace, int64 &totalspace)
    {
        return impl->GetSpace(freespace, totalspace); 
    }
    
private:
    ImageStore();
    ImageStore(const string& store_type);
    DISALLOW_COPY_AND_ASSIGN(ImageStore);
    static ImageStore *instance;
    ImageStoreImpl *impl;
    string _store_type;
};

class HttpdStore: public ImageStoreImpl
{
public:
    HttpdStore();
    ~HttpdStore();
    
    static STATUS EnableOptions();
    STATUS Init();
    STATUS GetStorageDir(string& dir);
    STATUS GetUrlByPath(const string& path,FileUrl& url);
    STATUS GetUploadIp(string &ip)
    {
        ip = "";
        return SUCCESS;
    }
    
    STATUS AddFile(const string& file,FileUrl& url)
    {
        //不支持
        return ERROR_NOT_SUPPORT;
    }
    
    STATUS DeleteFile(const FileUrl& url)
    {
        //不支持
        return ERROR_NOT_SUPPORT;
    }

    STATUS GetSpace(int64 &freespace, int64 &totalspace)
    {
        //不支持
        return ERROR_NOT_SUPPORT;
    }

private:
    DISALLOW_COPY_AND_ASSIGN(HttpdStore);
};

class NfsStore: public ImageStoreImpl
{
public:
    NfsStore(const string& server_ip,const string& root_dir):
    _server_ip(server_ip),_root_dir(root_dir)
    {};
    ~NfsStore() 
    {};
    
    static STATUS EnableOptions();
    STATUS Init();
    STATUS GetStorageDir(string& dir);
    STATUS GetUrlByPath(const string& path,FileUrl& url);
    STATUS GetUploadIp(string &ip)
    {
        ip = _server_ip;
        return SUCCESS;
    }

    STATUS AddFile(const string& file,FileUrl& url)
    {
        //不支持
        return ERROR_NOT_SUPPORT;
    }
    
    STATUS DeleteFile(const FileUrl& url)
    {
        //不支持
        return ERROR_NOT_SUPPORT;
    }

    STATUS GetSpace(int64 &freespace, int64 &totalspace);

private:
    NfsStore() {};
    DISALLOW_COPY_AND_ASSIGN(NfsStore);
    string _server_ip;
    string _root_dir;
};

STATUS EnableImageStoreOptions();
STATUS ImageStoreInit();
}
#endif


