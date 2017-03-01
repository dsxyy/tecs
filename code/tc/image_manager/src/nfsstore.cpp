#include "tecs_config.h"
#include "log_agent.h"
#include "nfs.h"
#include "image_store.h"
#include <boost/algorithm/string.hpp>
using namespace boost;

static int nfsstore_print_on = 0;
DEFINE_DEBUG_VAR(nfsstore_print_on);
#define Debug(level,fmt,arg...) \
        do \
        { \
            if(nfsstore_print_on) \
            { \
                printf("[%s:%d]"fmt,__func__,__LINE__, ##arg); \
            } \
            OutPut(level,fmt,##arg);\
        }while(0)

namespace zte_tecs
{
STATUS NfsStore::Init()
{
    STATUS ret;
    
    //ȥ��ip��ַǰ��Ŀո�
    trim(_server_ip);
    if(_server_ip.empty())
    {
        //����û�û��ָ�������nfs�����ַ����ʹ�ù���ƽ��ip
        TecsConfig *config = TecsConfig::Get();
        ret = GetIpAddress(config->get_management_interface(),_server_ip);
        if (SUCCESS != ret)
        {
            OutPut(SYS_ALERT, "Get ip address of mgt interface failed! ret = %d\n",ret);
            SkyAssert(false);
            return ERROR;
        }    
        OutPut(SYS_NOTICE, "nfs store will use mgt interface ip %s.\n",_server_ip.c_str());
    }
    else if(_server_ip == "localhost"  || 
        _server_ip == "*" || 
        0 == _server_ip.compare(0,3,"127"))
    {
        //��ֹ��loopback�ӿ�������nfs���� ��TC����ip����hc û������ !
        Debug(SYS_ERROR,"illegal nfs server address on loopback interface: %s!\n",_server_ip.c_str());
        return ERROR_INVALID_ARGUMENT;
    }
    
    
    //ȥ��·������б�ܣ��Ա����ͳһ����
    trim(_root_dir);
    regular_path(_root_dir);
    if(_root_dir.empty())
    {
        SkyAssert(false);
        return ERROR;
    }
   
    Debug(SYS_NOTICE,"starting nfs service on %s with directory %s ...\n",
        _server_ip.c_str(),_root_dir.c_str());

    //����nfs����·��
    return ExportNfs(_root_dir,"*","");
}

STATUS NfsStore::GetStorageDir(string& dir)
{
    dir = _root_dir;
    return SUCCESS;
}

STATUS NfsStore::GetUrlByPath(const string& path,FileUrl& url)
{
    if(0 != path.compare(0,_root_dir.length(),_root_dir))
    {
        OutPut(SYS_WARNING,"GetUrlByPath: path %s not in nfs root %s\n",path.c_str(),_root_dir.c_str());
        return ERROR_INVALID_ARGUMENT;
    }
    
    url.path = _server_ip + ":" + path;
    url.access_type = "nfs";

    NfsAccessOption option;
    option.server_ip = _server_ip;
    option.shared_dir = _root_dir;
    //��nfs store������ѡ�����л�֮�󱣴浽�ļ�url��
    url.access_option = option.serialize();
    return SUCCESS;
}

void DbgTestMountNfs()
{
    NfsAccessOption service;
    //service._ip = "10.43.178.199";
    service.server_ip = "127.0.0.1";
    service.shared_dir = "/tmp/mnt";

    string target("/dev/shm/testmount");
    MountNfs(service,target);
}
DEFINE_DEBUG_FUNC(DbgTestMountNfs);

void DbgTestGetUrl(const char* file)
{
    FileUrl url;
    NfsStore *pNfsStore = new NfsStore("127.0.0.1","/home/tecs");
    if(SUCCESS != pNfsStore->GetUrlByPath(file,url))
    {
        cerr << "failed to get url for file " << file << endl;
        return; 
    }
    cout << "url:" << endl << url.serialize() << endl;
    delete pNfsStore;
}
DEFINE_DEBUG_FUNC(DbgTestGetUrl);
}


