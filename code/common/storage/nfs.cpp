#include "nfs.h"
#include "log_agent.h"
#include <sys/mount.h>
using namespace zte_tecs;
static int nfs_print_on = 0;
DEFINE_DEBUG_VAR(nfs_print_on);
#define Debug(level,fmt,arg...) \
        do \
        { \
            if(nfs_print_on) \
            { \
                printf("[%s:%d]"fmt,__func__,__LINE__, ##arg); \
            } \
            OutPut(level,fmt,##arg);\
        }while(0)
#define TEMP_MOUNT_DIR "/dev/shm/tecs/"

static string randomdir(const string& prefix)
{
    //使用线程的tid和当前的本地时间生成随机的名称
    pid_t tid = (pid_t)syscall(__NR_gettid);
    struct timeval tv;
    gettimeofday(&tv,NULL);
    time_t now = tv.tv_sec;
    struct tm dt;
    localtime_r(&now, &dt);
    string dir(prefix);
    stringstream ss;
    do
    {
        //如果随机产生的目录名称已经存在了，需要重新尝试创建一个
        tv.tv_usec ++;
        ss  << dec << tid << "-" 
            << dec << dt.tm_year + 1900
            << dec << setfill('0') << setw(2) << dt.tm_mon + 1  //库函数返回的月份是从0开始的，所以要加1
            << dec << setfill('0') << setw(2) << dt.tm_mday
            << dec << setfill('0') << setw(2) << dt.tm_hour
            << dec << setfill('0') << setw(2) << dt.tm_min
            << dec << setfill('0') << setw(2) << dt.tm_sec
            << "-" << dec << tv.tv_usec;
    }
    while(0 == access((dir + ss.str()).c_str(),F_OK));
    
    return (dir + ss.str());
}

STATUS GetMountSource(const string& target,string& source)
{
    //获取某个路径上正在mount着的文件源路径
    ifstream fin("/proc/mounts");  
    string line;  
    string temp;
    string _source,_target;
    //同样是路径，有时候尾部带个斜杠，有时没有，这里统一一下，都删除掉
    string regular_target = target;
    regular_path(regular_target);
    
    while(getline(fin,line))
    {    
        _source = line.substr(0,line.find_first_of(' '));
        temp = line.substr(line.find_first_of(' ') + 1);
        _target = temp.substr(0,temp.find_first_of(' '));
        regular_path(_source);
        regular_path(_target);
        if(_target == regular_target)
        {
            source = _source;
            break;
        }
    }
    return SUCCESS;
}

STATUS GetMountTarget(const string& source,string& target)
{
    //获取某个文件源当前mount在哪一个位置上，注意一个源可以同时mount在多个目的路径上，这里只返回第一个
    ifstream fin("/proc/mounts");  
    string line;  
    string temp;

    string _source,_target;
    //同样是路径，有时候尾部带个斜杠，有时没有，这里统一一下，都删除掉
    string regular_source = source;
    regular_path(regular_source);
    while(getline(fin,line))
    {    
        _source = line.substr(0,line.find_first_of(' '));
        temp = line.substr(line.find_first_of(' ') + 1);
        _target = temp.substr(0,temp.find_first_of(' '));
        regular_path(_source);
        regular_path(_target);
        
        if(_source == regular_source)
        {
            target = _target;
            break;
        }
    }
    return SUCCESS;
}

STATUS EnsureNfsRunning()
{
    //确保nfs服务启动
    int ret = system("/sbin/service nfs start");

    if(-1 == ret)
    {
        //system本身失败时返回-1
        Debug(SYS_ERROR,"system command 'service nfs start' execute failed! errno = %d\n",errno);
        return ERROR;
    }
    else if(0 != ret)
    {
        //否则返回的是实际执行命令的退出码
        Debug(SYS_NOTICE,"system command 'service nfs start' return = %d\n",ret);
    }
    return SUCCESS;
}

STATUS ExportNfs(const string& source,const string& client, const string& option)
{  
    //exportfs -i *:/home/share 创建nfs服务
    //exportfs -u *:/home/share 撤销nfs服务  
    //这两个命令必须在nfs服务运行着的前提下才有实际意义
    int ret = -1;
    if(source.empty() || client.empty())
    {
        return ERROR_INVALID_ARGUMENT;
    }

    //nfs服务路径还没有创建
    string command;
    string nfsshare;
    string nfsexport;
    if(access(source.c_str(), F_OK) != 0)
    {
        //创建用于mount的target目录
        command = "mkdir -p " + source;
        ret = system(command.c_str());  
        if(0 != ret)
        {
            Debug(SYS_WARNING,"command %s failed! errno = %d\n",
                command.c_str(),errno);
            return ERROR;
        }
    }
    nfsshare  = source ;
    nfsexport.assign(nfsshare);
    nfsexport.append(" ");
    nfsexport.append(client);
    if(!option.empty())
    {
        nfsexport.append(option);
    }
    else
    {
        nfsexport.append("(ro,no_root_squash)");
    }

    command = "[ `grep -c \""+nfsshare+"\" /etc/exports` -eq 0 ] && echo \""+nfsexport+"\" >> /etc/exports";   
    system(command.c_str());

    EnsureNfsRunning();
    command = "exportfs -ar";
    ret = system(command.c_str());
    if(0 != ret)
    {
        Debug(SYS_ERROR,"failed to execute command: %s, errno = %d\n",command.c_str(),errno);
        return ERROR;
    }
    return SUCCESS;
}

STATUS MountNfs(const string& ip,const string& source,string& target,NfsMountStyle style)
{
    string tmpStr = source;
    regular_path(tmpStr);
    NfsAccessOption service(ip,tmpStr);
    return MountNfs(service,target,style);
}

STATUS MountNfs(const NfsAccessOption& service,string& target,NfsMountStyle style)
{
    int ret = -1;
    if((service.valid() == false) || (target.empty()))
    {
        return ERROR_INVALID_ARGUMENT;
    }

    string remote_source = service.server_ip + ":" + service.shared_dir;
    bool is_local = IsLocalUrl(service.server_ip);
    if(is_local)
    {
        if(access(target.c_str(), F_OK) == 0)
        {
            //如果目录已经存在
            struct stat64 stat;
            lstat64(target.c_str(),&stat);
            if(!S_ISLNK(stat.st_mode))
            {
                //不是个软链接
                Debug(SYS_WARNING,"target %s is not a link!\n",target.c_str());
                return ERROR;
            }

            char buf[256] = {'\0'};
            readlink(target.c_str(),buf,sizeof(buf));
            regular_path(buf);
            if(service.shared_dir == string(buf))
            {
                //已经链接到了指定路径
                Debug(SYS_DEBUG,"target %s is already linked to source %s!\n",target.c_str(),buf);
                return SUCCESS;
            }
            else
            {
                //没有链接到指定路径
                Debug(SYS_WARNING,"target %s is linked to source %s!\n",target.c_str(),buf);
                return ERROR;
            }
        }
        else
        {
            //如果目录还不存在，直接建立软链接
            ret = symlink(service.shared_dir.c_str(),target.c_str());
            if(ret != 0)
            {
                Debug(SYS_WARNING,"symlink %s to %s failed! errno = %d\n",
                    target.c_str(),service.shared_dir.c_str(),errno);
                return ERROR;
            }
            else
            {
                Debug(SYS_DEBUG,"target %s is now linked to source %s!\n",
                    target.c_str(),service.shared_dir.c_str());
                return SUCCESS;
            }
        }
    }
    else
    {
        //检查一下看看是不是已经mount过了，如果是则返回mount的地址
        bool srcsame = false;
        bool dstsame = false;
        ifstream fin("/proc/mounts");  
        string line;  
        string _source,_target,temp;
        string remote_source = service.server_ip + ":" + service.shared_dir;
        while(getline(fin,line))
        {    
            _source = line.substr(0,line.find_first_of(' '));        
            regular_path(_source);
            if(_source == remote_source)
            {
                srcsame = true;
                temp = line.substr(line.find_first_of(' ') + 1);
                _target = temp.substr(0,temp.find_first_of(' ')); 
                regular_path(_target);
                regular_path(target);
                if(_target == target)
                {
                    dstsame = true;                
                    break;
                }
            }
        }
        if(style == TO_UNIQUE_DIR)
        {    
            if(srcsame)
            {
                //如果已经mount在非指定的目录，将实际mount的路径返回
                target = _target;
                Debug(SYS_DEBUG,"source %s is already mounted to target %s !\n",
                      _source.c_str(),_target.c_str());
                return SUCCESS;
            }
        }

        if(style == TO_TEMP_DIR)
        {
            target = randomdir(TEMP_MOUNT_DIR);
        }

        //源已经mount到目的，直接返回成功
        if(dstsame && style != TO_TEMP_DIR)
        {
            Debug(SYS_DEBUG,"source %s is already mounted to target %s !\n",
                  _source.c_str(),_target.c_str());
            return SUCCESS;
        }
        
        //远程nfs还没有在本地mount过，执行mount命令
        string command;
        bool newdir = false;
        if(access(target.c_str(), F_OK) != 0)
        {
            //创建用于mount的target目录
            command = "mkdir -p " + target;
            ret = system(command.c_str());  
            if(0 != ret)
            {
                Debug(SYS_WARNING,"command %s failed! errno = %d\n",
                    command.c_str(),errno);
                return ERROR;
            }
            newdir = true;
        }
        
        //将远程目录mount过来
        command = "mount " + remote_source + " " + target;
        ret = system(command.c_str());  
        if(0 != ret)
        {
            if(newdir)
            {
                rmdir(target.c_str());
            }
            Debug(SYS_WARNING,"command %s failed! errno = %d\n",
                    command.c_str(),errno);
            return ERROR;
        }

        Debug(SYS_DEBUG,"source %s is now mounted to target %s !\n",
            remote_source.c_str(),target.c_str());
        return SUCCESS;
    }
}

STATUS UnMountNfs(const string& target)
{
    string _target(target);
    if(_target.empty())
    {
        return ERROR_INVALID_ARGUMENT;
    }
    
    if(access(_target.c_str(), F_OK) != 0)
    {
        return ERROR_FILE_NOT_EXIST;
    }
    
    regular_path(_target);
    string command;
    //如果目录已经存在
    struct stat64 stat;
    lstat64(_target.c_str(),&stat);
    if(S_ISLNK(stat.st_mode))
    {
        //是个软链接，删除即可
        Debug(SYS_DEBUG,"target %s is a link, removed!\n",_target.c_str());
        command = "rm -rf " + _target;
    }
    else
    {
        //非软链接，检查是否需要umount
        string _source;
        GetMountSource(target,_source);
        if(_source.empty())
        {
            //当前该目录上并未mount任何东西，无须umount，直接返回成功
            Debug(SYS_DEBUG,"target %s has nothing to be umounted!\n",_target.c_str());
            return SUCCESS;
        }
        
        Debug(SYS_DEBUG,"target %s will be umounted(source: %s)!\n",_target.c_str(),_source.c_str());
        command = "umount -f " + _target + " 2>/dev/null";
    }
    
    int ret = system(command.c_str());
    if(0 != ret)
    {
        Debug(SYS_ERROR,"command %s failed! errno = %d\n",command.c_str(),errno);
        return ERROR;
    }
    return SUCCESS;
}

