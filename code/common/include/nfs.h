#ifndef TECS_NFS_H
#define TECS_NFS_H
#include "sky.h"
#include <boost/algorithm/string.hpp>
using namespace boost;

enum NfsMountStyle
{
    //���enum��Ҫ��ΪMountNfs��ʹ�ã�֪ͨ�����ʵ������ķ�ʽmount nfs����
    TO_TEMP_DIR,    //��nfs����mount��һ�������������ʱ·����
    TO_UNIQUE_DIR,  //��nfs����mount��Ψһ��·���ϣ��������nfs�����Ѿ���mount���ˣ���������mount��·��
    TO_FORCE_DIR    //��nfsǿ��mount��ָ����·���ϣ������nfs�����Ѿ�mount���ˣ�ҲҪ�ٴ�mountһ�鵽�µ�·��
};

//NFS����ѡ�download�ͻ����������ļ�uri֮����Ҫ���ô�ѡ��������
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

