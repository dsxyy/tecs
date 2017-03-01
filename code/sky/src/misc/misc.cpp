/*******************************************************************************
* Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：misc.cpp
* 文件标识：见配置管理计划书
* 内容摘要：这个文件里面用于定义一些无法清晰归类存放的零零碎碎的通用函数
* 当前版本：1.0
* 作    者：张文剑
* 完成日期：2012年5月17日
*
* 修改记录1：
*     修改日期：2012/05/17
*     版 本 号：V1.0
*     修 改 人：张文剑
*     修改内容：创建
*******************************************************************************/
#include <pwd.h>
#include <uuid/uuid.h>
#include "misc.h"
#include "exception.h"
#include "shell.h"
#include "uuid.h"
#include <boost/algorithm/string.hpp>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h> 
#include <sys/mount.h> 
using namespace boost;

extern void Delay(uint32 miliseconds);
STATUS SshGetPubkey(string& pubkey)
{
    struct passwd *pw = getpwuid(getuid());
    const char *homedir = pw->pw_dir;
    if(!homedir)
    {
        return ERROR;
    }
    string keyfile(homedir);
    keyfile.append("/.ssh/id_dsa.pub");
    //string keyfile("/root/.ssh/id_dsa.pub");

    if (0 != access(keyfile.c_str(), R_OK))
    {
        return ERROR_FILE_NOT_EXIST;
    }

    ifstream fin(keyfile.c_str());
    ostringstream oss;
    char c;
    while ((c=fin.get()) != EOF)
    {
        oss << c;
    }

    pubkey = oss.str();
    if (pubkey.length()>0 && pubkey[pubkey.length()-1] == '\n')
    {
        //去掉尾部换行符号
        pubkey.erase(pubkey.length()-1, 1);
    }
    return SUCCESS;
}

STATUS SshTrust(const string& pubkey)
{
    struct passwd *pw = getpwuid(getuid());
    const char *homedir = pw->pw_dir;
    if(!homedir)
    {
        return ERROR;
    }
    string keyfile(homedir);
    keyfile.append("/.ssh/authorized_keys");

    //代开文件获取锁
    int fd = open(keyfile.c_str(), O_RDWR | O_CREAT | O_APPEND);
    if (fd == -1)
    {
        cerr << "failed to open ssh key file! errno = " << errno << endl;
        return ERROR_RESOURCE_UNAVAILABLE;
    }

    //等待别的进程在使用文件结束
    uint32 count = 0;
    while (lockf(fd,F_TEST,0L)==-1) 
    {   
        //waiting。如果一直等待不到，则退出，避免一直得不到处理
        count++;
        if (count > 10)
        {        
            close(fd);    
            return ERROR_DEVICE_BUSY;
        }        
        Delay(100);
    }
    //获取锁，如果还是失败，则关闭文件退出
    if (lockf(fd,F_TLOCK,0L) == -1)
    {
        cerr << "failed to lock key file! errno = " << errno << endl;
        close(fd);
        return ERROR_RESOURCE_UNAVAILABLE;
    }

    //打开文件的输出流
    ifstream ifile (keyfile.c_str());
    if (! ifile.is_open())
    { 
        //失败则退出，关闭文件释放锁
        cerr << "Error opening in file!errno =" << errno << endl;
        lockf(fd,F_ULOCK,0L);
        close(fd);
        return ERROR_NOT_READY; 
    }
    //一条条读取key,判断是否已经写入过
    string key;
    char buf[1024];    
    while (!ifile.eof()) 
    {
        ifile.getline(buf,1024);
        key = string(buf);
        if (pubkey == key)
        {
            //发现一样的，不需要再处理，成功退出，关闭文件释放锁
            ifile.close();
            lockf(fd,F_ULOCK,0L);
            close(fd);
            return SUCCESS;
        }
    }
    ifile.close();    

    //打开输入流，写入key值
    ofstream ofile;
    ofile.open(keyfile.c_str(),ios::app|ios::out); 
    if (!ofile.is_open())
    { 
        //失败则退出，关闭文件释放锁
        cerr << "Error opening out file!errno =" << errno << endl;
        lockf(fd,F_ULOCK,0L);
        close(fd);
        return ERROR_NOT_READY; 
    }
    ofile << pubkey << endl;
    ofile.close();

    lockf(fd,F_ULOCK,0L);
    close(fd);
    return SUCCESS;
}


STATUS SshUntrust(const string &pubkey)
{
    struct passwd *pw = getpwuid(getuid());
    const char *homedir = pw->pw_dir;
    if(!homedir)
    {
        return ERROR;
    }
    string authorized_keys(homedir);
    authorized_keys.append("/.ssh/authorized_keys");
    //string authorized_keys("/root/.ssh/authorized_keys");

    if (0 != access(authorized_keys.c_str(), W_OK))
    {
        return ERROR_FILE_NOT_EXIST;
    }
    
    //删除key
    string cmd = "sed -i 's:" + pubkey + "\\>::' " + authorized_keys;
    int ret = system(cmd.c_str());
    if(0 != ret)
    {
        SkyAssert(false);
        return ERROR;
    }
    
    //再删除空行
    cmd = "sed -i /^$/d " + authorized_keys;
    ret = system(cmd.c_str());
    if(0 != ret)
    {
        SkyAssert(false);
        return ERROR;
    }
    return 0;
}

void DbgSshGetPubkey()
{
    struct passwd *pw = getpwuid(getuid());
    const char *homedir = pw->pw_dir;
    if(!homedir)
    {
        cerr << "failed to get home dir!" << endl;
        return;
    }

    cout << "home dir = " << string(homedir) << endl;
    
    string pubkey;
    STATUS ret = SshGetPubkey(pubkey);
    if(SUCCESS != ret)
    {
        cerr << "SshGetPubkey failed! ret = " << ret << "!" << endl;
        return;
    }
    cout << "ssh public key:" << endl << pubkey << endl;
}
DEFINE_DEBUG_FUNC(DbgSshGetPubkey);


//执行linux命令
//输入：cmd-linux命令
//输出：result-命令执行后的输出，按行放入vector<string>中，
//      去掉每行的首位空格和回车符号
//返回值:0-成功，其他-失败
STATUS RunCmd(const string& cmd, vector<string>& result)
{
    char    s[1024] = {0};
    FILE    *fp = NULL; 
    string  str;
    if(cmd.empty())
    {
        return ERROR_INVALID_ARGUMENT;
    }
   
    if((fp = popen(cmd.c_str(), "r")) == NULL)
    {
        return ERROR;
    }

    while(fgets(s, 1024, fp))
    {   
        str = s;       
        boost::trim(str);//boost库函数，处理后str中无回车符
        if(!str.empty())
        {
            result.push_back(str);      
        }
    }

    pclose(fp);
    
    return SUCCESS;
}

//执行linux命令
//输入：cmd-linux命令
//输出：result-命令执行后的输出，去掉每行的首尾空格，保留回车
//返回值:0-成功，其他-失败
int RunCmd(const string& cmd, string& result)
{
    vector<string> v_result;
    vector<string>::iterator it;
    int32 ret;
    
    ret = RunCmd(cmd,v_result);
    if(SUCCESS == ret)
    {
        it = v_result.begin();
        while(it != v_result.end())
        {
            result += (*it + "\n");
            ++it;
        } 
    }
    else
    {
        result="";
    }
    
    return ret;
}

//执行linux命令，不关心命令执行的输出
//输入：cmd-linux命令
//返回值:system系统命令返回值，0-成功，其他-失败
//错误代码说明请参考system()命令说明
STATUS RunCmd(const string& cmd)
{
    int32 ret;

    if(cmd.empty())
    {
        return ERROR_INVALID_ARGUMENT;
    }

    ret = system(cmd.c_str());

    return ret;
}

FILE *Popen(const char *cmdstring, const char *type,pid_t &pid)  
{  
    int     pfd[2];  
    FILE    *fp;  
    #define SHELL   "/bin/sh" 
    /* only allow "r" or "w" */  
    if ((type[0] != 'r' && type[0] != 'w') || type[1] != 0) {  
        errno = EINVAL;     /* required by POSIX.2 */  
        return(NULL);  
    }
  
    if (pipe(pfd) < 0)  
        return(NULL);   /* errno set by pipe() */  
  
    if ( (pid = fork()) < 0)  
        return(NULL);   /* errno set by fork() */  
    else if (pid == 0) {                            /* child */  
        if (*type == 'r') {  
            close(pfd[0]);  
            if (pfd[1] != STDOUT_FILENO) {  
                dup2(pfd[1], STDOUT_FILENO);  
                close(pfd[1]);  
            }  
        } else {  
            close(pfd[1]);  
            if (pfd[0] != STDIN_FILENO) {  
                dup2(pfd[0], STDIN_FILENO);  
                close(pfd[0]);  
            }  
        }  
        execl(SHELL, "sh", "-c", cmdstring, (char *) 0);  
        _exit(127);  
    }  
                                /* parent */  
    if (*type == 'r') {  
        close(pfd[1]);  
        if ( (fp = fdopen(pfd[0], type)) == NULL)  
            return(NULL);  
    } else {  
        close(pfd[0]);  
        if ( (fp = fdopen(pfd[1], type)) == NULL)  
            return(NULL);  
    }  
    return(fp);  
}  

int  Pclose(FILE *fp,const pid_t &pid)  
{  
    int     stat;
    if (fclose(fp) == EOF)  
        return(-1);  
  
    while (waitpid(pid, &stat, 0) < 0)  
        if (errno != EINTR)  
            return(-1); /* error other than EINTR from waitpid() */  
  
    return(stat);   /* return child's termination status */  
}

string GenerateUUID(void)
{
#if 0
    uuid_t uuid;
    char   acUuid[64]={0};
    string tmp_str;
    uuid_create(&uuid);        
    sprintf(acUuid,UUIDFORMAT_PRINTF,uuid.time_low, 
            uuid.time_mid,uuid.time_hi_and_version, 
            uuid.clock_seq_hi_and_reserved, uuid.clock_seq_low,
            uuid.node[0],uuid.node[1],uuid.node[2], 
            uuid.node[3],uuid.node[4],uuid.node[5]);
    tmp_str = acUuid;
    return tmp_str;    
#else
    uuid_t        buf;
    char        str[100] = {'\0'};
    uuid_generate(buf);
    uuid_unparse(buf, str);
    return string(str);
#endif    
}

void Log(int priority,const string& log)
{
    syslog(priority,"%s",log.c_str());
}

void Log(int priority, const char* format,...)
{
    va_list vlist;
    va_start(vlist, format);
    vsyslog(priority, format, vlist);
    va_end(vlist);
}

//#define PROT_READ	0x1		/* page can be read */
//#define PROT_WRITE	0x2		/* page can be written */
//#define PROT_EXEC	0x4		/* page can be executed */
//#define PROT_SEM	0x8		/* page may be used for atomic ops */
//#define PROT_NONE	0x0		/* page can not be accessed */

void MmuModPage(BYTE *pucAddr, uint32 dwSize,int iReadProt)
{
    //int iReadProt = PROT_READ;
    int iProtRet = 0;
    uint32 dwProtLen = (dwSize + 0xfff) & 0xfffff000;

    iProtRet = mprotect((void *) pucAddr, (size_t) dwProtLen, iReadProt);
    if ( 0 != iProtRet )
    {
	    printf("MmuModPage: modify page attribute readonly failed!, ErrorNo = %d!\n",errno);//errnoGet()
    }

    return;    
}

/*

mode取值:   R_OK - 可读
            W_OK - 可写
            X_OK - 可执行
            F_OK - 文件存在
*/
//access()
bool Access(const string& pathname, int mode)
{
    if(!access(pathname.c_str(), mode))
    {
        return true;
    }        
        
    return false;
}

bool IsFileExist(const string& pathname)
{
    if(Access(pathname, F_OK))
    {
        return true;
    }

    return false;        
}

/*

mode取值:   S_IRWXU - 所有者授权读、写、执行权限，相当于00700
            S_IRWXG - 同组用户授权读、写、执行权限，相当于00070
            S_IRWXO - 其他用户授权读、写、执行权限，相当于00007

返回值: 0表示成功，-1表示失败
*/
//mkdir
int CreateDir(string& dirname, int mode)
{
    if(!mode)
        mode=S_IRWXU | S_IRWXG | S_IRWXO;

    int stat = 0;
    stat = mkdir(dirname.c_str(), mode);

    if(stat)
    {
        cerr << "Error creating dir:"<< dirname.c_str()<<" errno =" << errno  << endl;
        
        if(errno == EEXIST)
            return 0;
    }

    return stat;
}

//ls --通过遍历目录下的文件，达到ls目标
DIR* OpenDir(const string& dirname)
{
    return opendir(dirname.c_str());
}
DIRENT* ReadDir(DIR *d)
{
    DIRENT *file = NULL;

    while (((file=readdir(d)) != NULL))
    {
        //防止遍历出现死循环
        if(strncmp(file->d_name, ".", 1) == 0)
            continue;
        
        if(strncmp(file->d_name, "..", 2) == 0)
            continue;

        return file;
    }
    
    return NULL;
}
int CloseDir(DIR *d)
{
    int stat = 0;

    stat = closedir(d);

    if(stat)
        cerr<<"Error close dir failed! errno ="<<errno<<endl;

    return stat;
}

DEFINE_DEBUG_FUNC(MmuModPage);

void DbgRunCmd(const char* command)
{
    if(!command)
    {
        cerr << "no command!" << endl;
        return;
    }
    
    string result;
    int ret = RunCmd(command,result);
    cout << "ret = " << ret << endl;
    cout << "result: " << endl;
    cout << result << endl;
}
DEFINE_DEBUG_FUNC(DbgRunCmd);

void DbgLog(const char* log)
{
    if(!log)
    {
        cerr << "no log!" << endl;
        return;
    }
    
    Log(LOG_CRIT,"%s",log);
    Log(LOG_CRIT,"[%s:%d] %s",__func__,__LINE__,log);
}
DEFINE_DEBUG_FUNC(DbgLog);



