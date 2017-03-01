#include <pthread.h>
#include <unistd.h>
#include <getopt.h>
#include <mqueue.h>
#include <sys/resource.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

#include "includes.h"
#include "pub_tmp.h"
#include "cli_timer.h"
#include "oam_execute.h"
#include "xmlrpc_preprocess.h"

#define TECS_RUNNING_DIR "/var/run/tecs"
extern WORD32 tCliMsgBufMutexSem;

T_CliJobCreatReg g_atJobRegTbl[] = 
{ 
     /*   wIndex                                      acName                      PEntry                         StckSize      DataSize*/
    {PROCTYPE_OAM_CLIMANAGER,         "OAM_Climanager",      &Oam_Interpret,       4096*20,    4096*25},
    {PROCTYPE_OAM_TELNETSERVER,      "OAM_TelServer",        &Oam_TelnetServer,  4096*20 ,   4096*20},
    {PROCTYPE_OAM_SSHSERVER,           "OAM_SSHServer",       &Oam_SSHServer,      4096*20 ,   4096*20},
  
};

T_CliJobInfo  g_atJobSet[MAX_JOB_NUM];
WORD32 g_dwNumOfAppJob = sizeof(g_atJobRegTbl) / sizeof (T_CliJobCreatReg);
T_CliJobInfo * GetJobInfoByJno(WORD16 wJno);

BOOLEAN Initialize(void );
void JobPowerOn(T_CliJobInfo *pJob);
void Run(T_CliJobInfo *pJob);
static BOOLEAN Msgqueuecreate(char *name);
static void ExitCli(int signo);
static bool create_pid_file(const char *pcExeName, pid_t pid);
static const char *filename_without_path(const char *path);
static bool write_pidfile(const char *pidfile,pid_t *oldpid);
static void RemoveOldPidfile(const char *pcPrefix);

/*全局信号量相关变量*/
pthread_mutex_t            s_tMutexLock; /* g_atMutex的互斥锁 */
WORD32   g_dwMaxSem=100;
T_Obj_MutexType *g_atMutex;
WORD32               g_dwSemCount=0;
void SemInitial();
int CliMain();

int main(int argc, char **argv)
{
    bool  foreground = false;
    char  opt;    
    pid_t           pid,sid;
    int iRet;

    /*读取命令参数*/
    while((opt = getopt(argc,argv,"p:fh")) != -1)        
    {
        switch(opt)        
        {            
        case 'p':    
            if (optarg != NULL)
            {
                strncpy(g_aucScriptPath, optarg, sizeof(g_aucScriptPath));
            }           
            continue;            
        case 'f':     
            foreground = true;
            continue;   
        case 'h':     
            printf("Usage:  cli -p [scriptpath] -hf\n"
                     "            -h print help\n"
                     "            -f force to run ground\n");                
            exit(0);                         
            break;          
        default:          
            break;    
        }
    }

    if (foreground == true)    
    {        
        pid = 0; //Do not fork    
    }
    else
    {
        pid = fork();        
    }

    switch (pid)
    {
    case -1: // Error           
        XOS_SysLog(OAM_CFG_LOG_EMERGENCY, "Error: Unable to fork.\n");        
        exit(-1);                    
    case 0: // Child process                     
        if (foreground == false)            
        {
            //创建pid文件
            if(false == create_pid_file(filename_without_path(argv[0]), getppid()))
            {
                exit(-1);
            }
    
            sid = setsid();
            if (sid == -1) 
            {
                XOS_SysLog(OAM_CFG_LOG_EMERGENCY, "Error: Unable to setsid.\n");        
                exit(-1);                    
            }
            int fd = open("/dev/null", O_RDWR);
            dup2(fd,0);
            dup2(fd,1);
            dup2(fd,2);
            close(fd);

            fcntl(0,F_SETFD,0); // Keep them open across exec funcs
            fcntl(1,F_SETFD,0);
            fcntl(2,F_SETFD,0);
		
        }                                    
        iRet = CliMain(); 
        if (iRet < 0)
        {
            exit(-1);
        }
        break;
    default: // Parent process  
        break; 
    }    	

    return 0;
}

int CliMain()
{
    WORD16 wIdx;
    WORD16 wMaxJobNum = (MAX_JOB_NUM > g_dwNumOfAppJob) ? g_dwNumOfAppJob : MAX_JOB_NUM;

    /*job注册信息初始化*/
    BOOLEAN bRet = Initialize();
    if (!bRet)
    {
        XOS_SysLog(OAM_CFG_LOG_EMERGENCY, "System initialize failed!\n");
        return -1;
    }

    /*从配置文件读取method配置信息*/
    if (!LoadClientConfigInfo())
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "Failed to load client configure info!");
        return -1;
    }

    /*运行所有job*/
    for (wIdx = 0; wIdx < wMaxJobNum; wIdx++)
    {
        JobPowerOn( &g_atJobSet[wIdx]);
    }

    LoadPreprocessFuncs();

    while(1)
    {
        sleep(1000 * 3000);
    }
}

BOOLEAN Initialize(void )
{
    WORD16 wIdx;
    WORD16 wMaxJobNum = (MAX_JOB_NUM > g_dwNumOfAppJob) ? g_dwNumOfAppJob : MAX_JOB_NUM;
	
    /*初始化注册job*/
    memset(g_atJobSet, 0, sizeof(g_atJobSet));
    for (wIdx = 0; wIdx < wMaxJobNum; wIdx++)
    {
        g_atJobSet[wIdx].dwJno = g_atJobRegTbl[wIdx].wIndex;
        g_atJobSet[wIdx].wState  = 0;
        g_atJobSet[wIdx].dwStackSize = g_atJobRegTbl[wIdx].dwStackSize;
        g_atJobSet[wIdx].pEntry = g_atJobRegTbl[wIdx].pEntry;
        g_atJobSet[wIdx].pDataRegion = (LPVOID)XOS_GetUB(g_atJobRegTbl[wIdx].dwDataRegionSize);
	 if (g_atJobSet[wIdx].pDataRegion == NULL)
	 {
            XOS_SysLog(OAM_CFG_LOG_EMERGENCY, "Initialize: allocate dataregion to %s failed!\n", g_atJobRegTbl[wIdx].acName);
            return FALSE;
	 }
	 memset(g_atJobSet[wIdx].pDataRegion, 0, g_atJobRegTbl[wIdx].dwDataRegionSize);

        memset(g_atJobSet[wIdx].acMqueueName, 0, sizeof(g_atJobSet[wIdx].acMqueueName));
	sprintf(g_atJobSet[wIdx].acMqueueName, "/mqname_%s_%u",  g_atJobRegTbl[wIdx].acName, getpid());
        //sprintf(g_atJobSet[wIdx].acMqueueName, "/mqname_%s",  g_atJobRegTbl[wIdx].acName);
    }

    SemInitial();
    TimerInitial();

    if (FALSE == XOS_CreateSemaphore(
                0,
                0,
                MUTEX_STYLE,
                &tCliMsgBufMutexSem)
       )
    {
        printf("ERROR:: create tCliMsgBufMutexSem mutex semaphore failed \n");
        return FALSE;
    }
	
    /*进程退出处理函数*/
    signal(SIGINT, ExitCli);
	
    return TRUE;
}

void JobPowerOn(T_CliJobInfo *pJob)
{
    pJob->dwTaskId = XOS_StartTask(pJob->acMqueueName, TSK_PRIO_PTYS_CLI, pJob->dwStackSize, 0, (VOID (*)(void*))&Run, (WORDPTR) pJob);
    if (INVALID_SYS_TASKID == pJob->dwTaskId)	
    {
       XOS_SysLog(OAM_CFG_LOG_EMERGENCY, "Start task failed ! \n");
    }
}

void Run(T_CliJobInfo *pJob)
{
    mqd_t mqd;
    int iMsgLen;	
    struct mq_attr attr;
    BOOLEAN bRet;

    if (pJob == NULL)
    {
        XOS_SysLog(OAM_CFG_LOG_EMERGENCY, "Job info is NULL, job run failed \n");
        return;
    }

    /*创建消息队列*/
    bRet = Msgqueuecreate(pJob->acMqueueName);
    if (!bRet)
    {
        return;
    }

    /*打开job的消息接收队列，供其他job发送消息用*/
    pJob->mq_Recv = mq_open(pJob->acMqueueName, O_WRONLY);
    if (pJob->mq_Recv == -1)
    {
        XOS_SysLog(OAM_CFG_LOG_EMERGENCY, "open msg queue for receive failed \n");
        return;
    }
   
    mqd = mq_open(pJob->acMqueueName, O_RDONLY);    
    if (mqd == -1)
    {
        XOS_SysLog(OAM_CFG_LOG_EMERGENCY, "message queue open failed \n");
        return;
    }
	
    mq_getattr(mqd, &attr);
    XOS_SysLog(OAM_CFG_LOG_EMERGENCY, "msg_queue info :max #msgs = %ld, max #bytes/msg = %ld, #currently on queue = %ld \n",
	        attr.mq_maxmsg, attr.mq_msgsize, attr.mq_curmsgs);
	        
    pJob->pMsgRecvBuff = XOS_GetUB(attr.mq_msgsize);
    if (pJob->pMsgRecvBuff == NULL)
    {
        XOS_SysLog(OAM_CFG_LOG_EMERGENCY, "message receive receive buffer allocate failed!\n");
        return;
    }

    /*上电消息*/
    pJob->pEntry(pJob->wState, EV_MASTER_POWER_ON, pJob->pMsgRecvBuff, pJob->pDataRegion, TRUE);
	
    /*Job 的消息环回*/
    while(1)
    {
        T_CliMsg *ptMsg = (T_CliMsg *)pJob->pMsgRecvBuff;
        memset(ptMsg, 0, attr.mq_msgsize);
        iMsgLen = mq_receive(mqd, (char *)ptMsg, attr.mq_msgsize, NULL);
        if (iMsgLen < 0)
        {
            XOS_SysLog(OAM_CFG_LOG_EMERGENCY, "ERROR:: mq_receive failed!\n");
            continue;
        }

        pJob->dwCurMsgLen = ptMsg->wDataLen;
        pJob->pEntry(pJob->wState, ptMsg->dwMsgId, ptMsg->aucMsg, pJob->pDataRegion, TRUE);
    }
}

static BOOLEAN Msgqueuecreate(char *name)
{
    int  flags;
    mqd_t mqd;
    struct mq_attr attr;

    struct rlimit  rlim_new;
    rlim_new.rlim_cur = rlim_new.rlim_max = RLIM_INFINITY;
    if (setrlimit(RLIMIT_MSGQUEUE , &rlim_new)!=0)  
    {
        XOS_SysLog(OAM_CFG_LOG_EMERGENCY, "call setrlimit failed \n");
    }
	
    flags = O_RDWR|O_CREAT;
    attr.mq_maxmsg = 20;
    attr.mq_msgsize =  MAX_OAMMSG_LEN;
    
    mqd = mq_open(name, flags, 777,  &attr);
    	
    if (mqd == -1)
    {
        XOS_SysLog(OAM_CFG_LOG_EMERGENCY, "message queue create failed \n");
		
        switch (errno)
        {
        case EACCES : 
            XOS_SysLog(OAM_CFG_LOG_EMERGENCY, "The queue exists, but the caller does not have permission to open it in the specified mode.");
            break; 
        case EEXIST : 
            XOS_SysLog(OAM_CFG_LOG_EMERGENCY, " Both O_CREAT and O_EXCL were specified in oflag, but a queue with this name already exists.");
            break; 
        case EINVAL : 
            XOS_SysLog(OAM_CFG_LOG_EMERGENCY, " O_CREAT was specified in oflag, and attr was not NULL, but attr->mq_maxmsg or attr->mq_msqsize was invalid."
           "Both of these fields must be greater than zero.  In a process that  is  unprivileged  (does  not  have  the"
           "CAP_SYS_RESOURCE  capability),  attr->mq_maxmsg  must  be  less  than  or  equal  to the msg_max limit, and"
           "attr->mq_msgsize must be less than or equal to the msgsize_max limit.  In addition, even  in  a  privileged"
           "process,  attr->mq_maxmsg  cannot exceed the HARD_MAX limit.  (See mq_overview(7) for details of these lim-"
           "its.)");
            break; 
        case EMFILE : 
            XOS_SysLog(OAM_CFG_LOG_EMERGENCY, " The process already has the maximum number of files and message queues open.");
            break; 
        case ENAMETOOLONG : 
            XOS_SysLog(OAM_CFG_LOG_EMERGENCY, "       name was too long.");
            break; 
        case ENFILE : 
            XOS_SysLog(OAM_CFG_LOG_EMERGENCY, " The system limit on the total number of open files and message queues has been reached.");
            break; 
        case ENOENT : 
            XOS_SysLog(OAM_CFG_LOG_EMERGENCY, " The O_CREAT flag was not specified in oflag, and no queue with this name exists.");
            break; 
        case ENOMEM : 
            XOS_SysLog(OAM_CFG_LOG_EMERGENCY, " Insufficient memory.");
            break; 
        case ENOSPC : 
            XOS_SysLog(OAM_CFG_LOG_EMERGENCY, " Insufficient space for the creation of a new message queue.  This probably occurred because the  queues_max limit was encountered; see mq_overview(7).");
            break;
        default:
            XOS_SysLog(OAM_CFG_LOG_EMERGENCY, "unknown err [%d]\n",errno);
            break;
        }
        XOS_SysLog(OAM_CFG_LOG_EMERGENCY, "\n");
        return FALSE;
    }
	
    mq_close(mqd);
    return TRUE;
}

T_CliJobInfo * GetJobInfoByJno(WORD16 wJno)
{
    WORD16 wIdx;
    WORD16 wMaxJobNum = (MAX_JOB_NUM > g_dwNumOfAppJob) ? g_dwNumOfAppJob : MAX_JOB_NUM;

    for (wIdx = 0; wIdx < wMaxJobNum; wIdx++)
    {
        if (g_atJobSet[wIdx].dwJno == wJno)
        {
            break;
        }
    }

    if (wIdx <wMaxJobNum )
    {
        return &g_atJobSet[wIdx];
    }
    else
    {
        return NULL;
    }
}

void SemInitial()
{
    WORD16 dwLoop;

    g_atMutex=(T_Obj_MutexType *)XOS_GetUB(sizeof(T_Obj_MutexType)*g_dwMaxSem);
    /*全局信号量初始化*/
    memset(g_atMutex,0,sizeof(T_Obj_MutexType)*g_dwMaxSem);
    for (dwLoop = 0; dwLoop < g_dwMaxSem; dwLoop++)
    {
        g_atMutex[dwLoop].ucUsed = VOS_SEM_CTL_NO_USED;
    }
	
}

void ExitCli(int signo)
{
    int wIdx;
    WORD16 wMaxJobNum = (MAX_JOB_NUM > g_dwNumOfAppJob) ? g_dwNumOfAppJob : MAX_JOB_NUM;
	
    switch(signo)
    {
    case SIGINT:
        /*释放内存*/
        for (wIdx = 0; wIdx < wMaxJobNum; wIdx++)
        {
            XOS_RetUB((BYTE *)g_atJobSet[wIdx].pDataRegion);	
            XOS_RetUB((BYTE *)g_atJobSet[wIdx].pMsgRecvBuff);

            mq_close(g_atJobSet[wIdx].mq_Recv);
            mqd_t tret = mq_unlink(g_atJobSet[wIdx].acMqueueName);
            XOS_SysLog(OAM_CFG_LOG_DEBUG, "unlink mq [%s] ret: %u \n", g_atJobSet[wIdx].acMqueueName,tret);
        }
        XOS_RetUB((BYTE *)g_atMutex);
        printf("*****************CliExit*****************\n");
        exit(0);
    default:
        printf("catch a signal: %d\n", signo);
         break;
    }
}

bool create_pid_file(const char *pcExeName, pid_t pid)
{
    char piddir[256] = {'\0'};

    //使用默认的pidfile目录: /var/run/tecs/
    snprintf(piddir, sizeof(piddir),"%s",TECS_RUNNING_DIR);

    //检查目录字符串结尾是否为/，如果没有就补充一个
    piddir[sizeof(piddir)-1] = '\0';
    piddir[sizeof(piddir)-2] = '\0';
    if(*(piddir+strlen(piddir)-1) != '/' )
    {
        *(piddir+strlen(piddir)) = '/';
    }

    //如果目录还不存在，就创建一下
    if(access(piddir, F_OK) != 0)
    {
        if (0 != mkdir(piddir,0600))
        {
//        cerr << "failed to make pid dir " << piddir << "! errno = " << errno << endl;
            printf("ERROR:: failed to make pid dir %s !\n", piddir);
            return false;
        }
    }
    else
    {
        RemoveOldPidfile(pcExeName);
    }

    //构造pidfile文件名: 可执行文件名称 + pid + ".pid"后缀
    char pidfile[128] = {'\0'};
    snprintf(pidfile, sizeof(pidfile), "%s.%d.pid", pcExeName,pid);
    if(false == write_pidfile(strcat(piddir, pidfile),NULL))
    {
 //       cerr << "write pid file " << pidfile << " failed!" << endl;
        printf("ERROR:: write pid file %s failed!\n", pidfile);
        return false;
    }

    return true;
}

static const char *filename_without_path(const char *path)
{
    if (path == NULL)
    {
        return NULL;
    }
    
    /* 找到斜杠出现的最后位置，如果找不到则返回NULL */
    const char *slash = strrchr(path, '/');

    if (!slash || (slash == path && !slash[1]))
    {
        return (char*)path;
    }
    return slash + 1;
}


static bool write_pidfile(const char *pidfile,pid_t *oldpid)
{
    char buf[32];
    int fd = open(pidfile, O_RDWR | O_CREAT, 0600);
    if (fd == -1)
    {   
        printf("failed to open pid file %s! errno = %d\n", pidfile, errno);
        return false;
    }
    
    /* We exit silently if daemon already running. */
    if (lockf(fd, F_TLOCK, 0) == -1)
    {   
        printf("failed to lock pid file %s! errno = %d\n", pidfile, errno);
        if(oldpid && read(fd,buf,sizeof(buf)))
        {
            *oldpid = atoi(buf);
        }
        close(fd);
        return false;
    }
    /*
    pid文件的内容格式是有标准规定的，参见: http://www.pathname.com/fhs/2.2/fhs-5.13.html
    The internal format of PID files remains unchanged. 
    The file must consist of the process identifier in ASCII-encoded decimal, 
    followed by a newline character. For example, if crond was process number 25, 
    /var/run/crond.pid would contain three characters: two, five, and newline. 
    */
    int len = snprintf(buf, sizeof(buf), "%ld\n", (long)getpid());
    if (write(fd, buf, len) != len)
    {   
        printf("failed to write pid file %s! errno = %d\n", pidfile, errno);
        close(fd);
        return false;
    }
    
   // record_tmp_file(fd,pidfile);
    //不能关闭文件，否则锁就失效了!!!
    return true;
}

/*删除旧的pid文件*/
static void RemoveOldPidfile(const char *pcPrefix)
{
    struct dirent *dir_env;

    DIR *dir = opendir(TECS_RUNNING_DIR);   

    while((dir_env = readdir(dir)))
    {
        if (strstr(dir_env->d_name, pcPrefix) == NULL)
        {
            continue;
        }
 	
        if ((Oam_String_Compare_Nocase(dir_env->d_name, pcPrefix, strlen(pcPrefix))) == 0)
        {
            char pidfile[128] = {'\0'};
            snprintf(pidfile, sizeof(pidfile), "%s/%s", TECS_RUNNING_DIR, dir_env->d_name);
            XOS_DeleteFile(pidfile);
            //XOS_SysLog(OAM_CFG_LOG_DEBUG, "Success to remove old pid file %s", dir_env->d_name);
        }
    }

}

