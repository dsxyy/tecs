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
    //ʹ���̵߳�tid�͵�ǰ�ı���ʱ���������������
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
        //������������Ŀ¼�����Ѿ������ˣ���Ҫ���³��Դ���һ��
        tv.tv_usec ++;
        ss  << dec << tid << "-" 
            << dec << dt.tm_year + 1900
            << dec << setfill('0') << setw(2) << dt.tm_mon + 1  //�⺯�����ص��·��Ǵ�0��ʼ�ģ�����Ҫ��1
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
    //��ȡĳ��·��������mount�ŵ��ļ�Դ·��
    ifstream fin("/proc/mounts");  
    string line;  
    string temp;
    string _source,_target;
    //ͬ����·������ʱ��β������б�ܣ���ʱû�У�����ͳһһ�£���ɾ����
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
    //��ȡĳ���ļ�Դ��ǰmount����һ��λ���ϣ�ע��һ��Դ����ͬʱmount�ڶ��Ŀ��·���ϣ�����ֻ���ص�һ��
    ifstream fin("/proc/mounts");  
    string line;  
    string temp;

    string _source,_target;
    //ͬ����·������ʱ��β������б�ܣ���ʱû�У�����ͳһһ�£���ɾ����
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
    //ȷ��nfs��������
    int ret = system("/sbin/service nfs start");

    if(-1 == ret)
    {
        //system����ʧ��ʱ����-1
        Debug(SYS_ERROR,"system command 'service nfs start' execute failed! errno = %d\n",errno);
        return ERROR;
    }
    else if(0 != ret)
    {
        //���򷵻ص���ʵ��ִ��������˳���
        Debug(SYS_NOTICE,"system command 'service nfs start' return = %d\n",ret);
    }
    return SUCCESS;
}

STATUS ExportNfs(const string& source,const string& client, const string& option)
{  
    //exportfs -i *:/home/share ����nfs����
    //exportfs -u *:/home/share ����nfs����  
    //���������������nfs���������ŵ�ǰ���²���ʵ������
    int ret = -1;
    if(source.empty() || client.empty())
    {
        return ERROR_INVALID_ARGUMENT;
    }

    //nfs����·����û�д���
    string command;
    string nfsshare;
    string nfsexport;
    if(access(source.c_str(), F_OK) != 0)
    {
        //��������mount��targetĿ¼
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
            //���Ŀ¼�Ѿ�����
            struct stat64 stat;
            lstat64(target.c_str(),&stat);
            if(!S_ISLNK(stat.st_mode))
            {
                //���Ǹ�������
                Debug(SYS_WARNING,"target %s is not a link!\n",target.c_str());
                return ERROR;
            }

            char buf[256] = {'\0'};
            readlink(target.c_str(),buf,sizeof(buf));
            regular_path(buf);
            if(service.shared_dir == string(buf))
            {
                //�Ѿ����ӵ���ָ��·��
                Debug(SYS_DEBUG,"target %s is already linked to source %s!\n",target.c_str(),buf);
                return SUCCESS;
            }
            else
            {
                //û�����ӵ�ָ��·��
                Debug(SYS_WARNING,"target %s is linked to source %s!\n",target.c_str(),buf);
                return ERROR;
            }
        }
        else
        {
            //���Ŀ¼�������ڣ�ֱ�ӽ���������
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
        //���һ�¿����ǲ����Ѿ�mount���ˣ�������򷵻�mount�ĵ�ַ
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
                //����Ѿ�mount�ڷ�ָ����Ŀ¼����ʵ��mount��·������
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

        //Դ�Ѿ�mount��Ŀ�ģ�ֱ�ӷ��سɹ�
        if(dstsame && style != TO_TEMP_DIR)
        {
            Debug(SYS_DEBUG,"source %s is already mounted to target %s !\n",
                  _source.c_str(),_target.c_str());
            return SUCCESS;
        }
        
        //Զ��nfs��û���ڱ���mount����ִ��mount����
        string command;
        bool newdir = false;
        if(access(target.c_str(), F_OK) != 0)
        {
            //��������mount��targetĿ¼
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
        
        //��Զ��Ŀ¼mount����
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
    //���Ŀ¼�Ѿ�����
    struct stat64 stat;
    lstat64(_target.c_str(),&stat);
    if(S_ISLNK(stat.st_mode))
    {
        //�Ǹ������ӣ�ɾ������
        Debug(SYS_DEBUG,"target %s is a link, removed!\n",_target.c_str());
        command = "rm -rf " + _target;
    }
    else
    {
        //�������ӣ�����Ƿ���Ҫumount
        string _source;
        GetMountSource(target,_source);
        if(_source.empty())
        {
            //��ǰ��Ŀ¼�ϲ�δmount�κζ���������umount��ֱ�ӷ��سɹ�
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

