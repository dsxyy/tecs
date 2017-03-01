#ifndef TECS_NFS_H
#define TECS_NFS_H
#include "sky.h"
#include <boost/algorithm/string.hpp>
using namespace boost;

enum NfsMountStyle
{
    //这个enum主要是为MountNfs所使用，通知其采用实际所需的方式mount nfs服务
    TO_TEMP_DIR,    //将nfs共享mount到一个随机产生的临时路径上
    TO_UNIQUE_DIR,  //将nfs共享mount到唯一的路径上，即如果该nfs服务已经被mount过了，返回正在mount的路径
    TO_FORCE_DIR    //将nfs强制mount到指定的路径上，即便该nfs服务已经mount过了，也要再次mount一遍到新的路径
};

//NFS访问选项，download客户端在有了文件uri之后，需要利用此选项来下载
class NfsAccessOption: public Serializable
{
public:
    string server_ip;
    string shared_dir;
    NfsAccessOption() {};
    NfsAccessOption(const string& ip,const string& dir):server_ip(ip),shared_dir(dir)
    {};

    bool valid() const
    {
        return (!server_ip.empty() && !shared_dir.empty());
    }
    
    SERIALIZE
    {
        SERIALIZE_BEGIN(NfsAccessOption);
        WRITE_VALUE(server_ip);
        WRITE_VALUE(shared_dir);
        SERIALIZE_END();
    }
    
    DESERIALIZE
    {
        DESERIALIZE_BEGIN(NfsAccessOption);
        READ_VALUE(server_ip);
        READ_VALUE(shared_dir);
        DESERIALIZE_END();
    }
};

inline void regular_path(string& path)
{
    trim(path);
    if(path.size() == 1)
        return;
    
    if(*(path.rbegin()) == '/')
        path.erase(path.end()-1);
}

inline void regular_path(char* path)
{
    int len = strlen(path);
    if(len == 1)
        return;
        
    if(path[len-1] == '/')
        path[len-1] = '\0';
}

STATUS EnsureNfsRunning();
STATUS GetMountSource(const string& target,string& source);
STATUS GetMountTarget(const string& source,string& target);
STATUS ExportNfs(const string& source,const string& client, const string& option);
STATUS MountNfs(const NfsAccessOption& service,string& target,NfsMountStyle style = TO_UNIQUE_DIR);
STATUS MountNfs(const string& ip,const string& source,string& target,NfsMountStyle style = TO_UNIQUE_DIR);
STATUS UnMountNfs(const string& target);
#endif

