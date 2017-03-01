/*******************************************************************************
* Copyright (c) 2012������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�misc.cpp
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ������ļ��������ڶ���һЩ�޷����������ŵ����������ͨ�ú���
* ��ǰ�汾��1.0
* ��    �ߣ����Ľ�
* ������ڣ�2012��5��17��
*
* �޸ļ�¼1��
*     �޸����ڣ�2012/05/17
*     �� �� �ţ�V1.0
*     �� �� �ˣ����Ľ�
*     �޸����ݣ�����
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
        //ȥ��β�����з���
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

    //�����ļ���ȡ��
    int fd = open(keyfile.c_str(), O_RDWR | O_CREAT | O_APPEND);
    if (fd == -1)
    {
        cerr << "failed to open ssh key file! errno = " << errno << endl;
        return ERROR_RESOURCE_UNAVAILABLE;
    }

    //�ȴ���Ľ�����ʹ���ļ�����
    uint32 count = 0;
    while (lockf(fd,F_TEST,0L)==-1) 
    {   
        //waiting�����һֱ�ȴ����������˳�������һֱ�ò�������
        count++;
        if (count > 10)
        {        
            close(fd);    
            return ERROR_DEVICE_BUSY;
        }        
        Delay(100);
    }
    //��ȡ�����������ʧ�ܣ���ر��ļ��˳�
    if (lockf(fd,F_TLOCK,0L) == -1)
    {
        cerr << "failed to lock key file! errno = " << errno << endl;
        close(fd);
        return ERROR_RESOURCE_UNAVAILABLE;
    }

    //���ļ��������
    ifstream ifile (keyfile.c_str());
    if (! ifile.is_open())
    { 
        //ʧ�����˳����ر��ļ��ͷ���
        cerr << "Error opening in file!errno =" << errno << endl;
        lockf(fd,F_ULOCK,0L);
        close(fd);
        return ERROR_NOT_READY; 
    }
    //һ������ȡkey,�ж��Ƿ��Ѿ�д���
    string key;
    char buf[1024];    
    while (!ifile.eof()) 
    {
        ifile.getline(buf,1024);
        key = string(buf);
        if (pubkey == key)
        {
            //����һ���ģ�����Ҫ�ٴ����ɹ��˳����ر��ļ��ͷ���
            ifile.close();
            lockf(fd,F_ULOCK,0L);
            close(fd);
            return SUCCESS;
        }
    }
    ifile.close();    

    //����������д��keyֵ
    ofstream ofile;
    ofile.open(keyfile.c_str(),ios::app|ios::out); 
    if (!ofile.is_open())
    { 
        //ʧ�����˳����ر��ļ��ͷ���
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
    
    //ɾ��key
    string cmd = "sed -i 's:" + pubkey + "\\>::' " + authorized_keys;
    int ret = system(cmd.c_str());
    if(0 != ret)
    {
        SkyAssert(false);
        return ERROR;
    }
    
    //��ɾ������
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


//ִ��linux����
//���룺cmd-linux����
//�����result-����ִ�к����������з���vector<string>�У�
//      ȥ��ÿ�е���λ�ո�ͻس�����
//����ֵ:0-�ɹ�������-ʧ��
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
        boost::trim(str);//boost�⺯���������str���޻س���
        if(!str.empty())
        {
            result.push_back(str);      
        }
    }

    pclose(fp);
    
    return SUCCESS;
}

//ִ��linux����
//���룺cmd-linux����
//�����result-����ִ�к�������ȥ��ÿ�е���β�ո񣬱����س�
//����ֵ:0-�ɹ�������-ʧ��
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

//ִ��linux�������������ִ�е����
//���룺cmd-linux����
//����ֵ:systemϵͳ�����ֵ��0-�ɹ�������-ʧ��
//�������˵����ο�system()����˵��
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

modeȡֵ:   R_OK - �ɶ�
            W_OK - ��д
            X_OK - ��ִ��
            F_OK - �ļ�����
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

modeȡֵ:   S_IRWXU - ��������Ȩ����д��ִ��Ȩ�ޣ��൱��00700
            S_IRWXG - ͬ���û���Ȩ����д��ִ��Ȩ�ޣ��൱��00070
            S_IRWXO - �����û���Ȩ����д��ִ��Ȩ�ޣ��൱��00007

����ֵ: 0��ʾ�ɹ���-1��ʾʧ��
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

//ls --ͨ������Ŀ¼�µ��ļ����ﵽlsĿ��
DIR* OpenDir(const string& dirname)
{
    return opendir(dirname.c_str());
}
DIRENT* ReadDir(DIR *d)
{
    DIRENT *file = NULL;

    while (((file=readdir(d)) != NULL))
    {
        //��ֹ����������ѭ��
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



