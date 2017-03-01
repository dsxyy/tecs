/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：tecs_guard.c
* 文件标识：见配置管理计划书
* 内容摘要：tecs守护进程的实现文件
* 当前版本：1.0

* 作    者：licanwei
* 完成日期：2011年8月11日
*
* 修改记录1：
*    修改日期：2011年8月11日
*    版 本 号：V1.0
*    修 改 人：licanwei
*    修改内容：创建
* 修改记录2：
*    修改日期：2012年5月22日
*    版 本 号：V2.0
*    修 改 人：张文剑
*    修改内容：修改guard框架，改为调用脚本来实施监控
* 修改记录3：
*    修改日期：2012年9月27日
*    版 本 号：V3.0
*    修 改 人：颜伟
*    修改内容：增加物理机喂狗功能
*******************************************************************************/
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <time.h>
#include <string>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <getopt.h>
#include<iostream>
#include<sstream>
#include <sys/ioctl.h>
#include <sched.h>
#include <pthread.h>
#include <syslog.h>
#include <stdarg.h>

#include "kcs_com.h"

using namespace std;

typedef void *(*ThreadEntry)(void*);   /* 任务入口原型 */

static bool debug = false;
#define Debug(fmt,arg...) \
        do \
        { \
            if(debug) \
            { \
                printf("[%s:%d]"fmt,__func__,__LINE__, ##arg); \
            } \
        }while(0)

#define NIC_UNKNOWN_EXP          10    /* 网口未知状态最长周期10次 */     
pid_t pidfile_pid(const char *pidfile)
{
	int fd;
	char pidstr[20];
	pid_t pid;
	unsigned int ret;

	fd = open(pidfile, O_NONBLOCK | O_RDONLY, 0600);
	if (fd == -1) {
		return 0;
	}
    memset(pidstr,0,sizeof(pidstr));
	if (read(fd, pidstr, sizeof(pidstr)-1) <= 0) {
		goto noproc;
	}

	ret = atoi(pidstr);

	if (ret == 0) {
		/* Obviously we had some garbage in the pidfile... */
		printf("Could not parse contents of pidfile %s\n",pidfile);
		goto noproc;
	}
	
	pid = (pid_t)ret;
	if (!(kill(pid,0) == 0 || errno != ESRCH)) {
		goto noproc;
	}

	close(fd);
	return (pid_t)ret;

 noproc:
	close(fd);
	unlink(pidfile);
	return 0;
}       
static bool write_pidfile(const char *pidfile,pid_t *oldpid)
{
    char buf[32];

    *oldpid = pidfile_pid(pidfile);
    if (*oldpid != 0)
    {
        printf("file %s exists and process id %d is running.\n",pidfile, (int)*oldpid);
        return false;
    }

    int fd = open(pidfile, O_NONBLOCK |O_RDWR | O_CREAT | O_EXCL, 0600);
    if (fd == -1)
    {
        cerr << "failed to open pid file! errno = " << errno << endl;
        return false;
    }

    /* We exit silently if daemon already running. */
    if (lockf(fd, F_TLOCK, 0) == -1)
    {
        cerr << "failed to lock pid file! errno = " << errno << endl;
        if (oldpid && read(fd,buf,sizeof(buf)))
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
        cerr << "failed to write pid file! errno = " << errno << endl;
        close(fd);
        return false;
    }

    fcntl(fd, F_SETFD, FD_CLOEXEC);
    //不能关闭文件，否则锁就失效了!!!
    return true;
}

static void show_options ()
{
    cout << "Options:\n";
    cout << "    -h  --help         Show this help\n"
            "    -d  --debug        Run in debug mode\n"
            "    -f  --foregroud    Run in foregroud\n"
            "    -s  --script       The script used for guard\n"
            "    -i  --interval     Guard interval, the unit is second\n"
            "    -p  --pidfile      The pidfile of daemon\n"
            "    -l  --log          The log file\n"
         << endl;
}

static bool from_string(int& t,const string& s,ios_base& (*f)(std::ios_base&))
{
    stringstream ss;
    ss << f << s;
    return !(ss >> t).fail();
}
static void *feed_dog(void *arg) 
{
    int ret = -1;
    int doghandle = -1;
    T_BSP_BRDCTRL_PARA  tUserBoardCtrl;
    T_BSP_HEARTBEAT_GET_REQ tBspBrdctrlGetHearbeat;
    int *p = static_cast<int *>(arg);
    int interval = *p;
    
    memset(&tBspBrdctrlGetHearbeat,0,sizeof(T_BSP_HEARTBEAT_GET_REQ));
    tBspBrdctrlGetHearbeat.ucCPUID = 1;
    tUserBoardCtrl.pParam     = (unsigned char *)&tBspBrdctrlGetHearbeat;
    tUserBoardCtrl.dwLen   = sizeof(T_BSP_HEARTBEAT_GET_REQ); 
    
    doghandle = open("/dev/kcsdrv", O_RDWR);
    if (doghandle < 0)
    {
        cerr << " Cann't open file /dev/kcsdrv!\n" << errno << endl;
        return NULL;
    }      
    
    while (1)
    {
        ret = ioctl(doghandle, BSP_IOCMD_BRDCTRL_FEED_WATCHDOG, &(tUserBoardCtrl));
        sleep(interval);
    }
    close(doghandle);

    return NULL;
}
static void guard(const string& script,const string& log)
{
    Debug("guard! script = %s, log = %s\n",script.c_str(),log.c_str());
    system(script.c_str());
}
static void KillSelf(void)
{
    system("reboot");
    sleep(5);
    exit(-1);
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

static void CheckNicStatus(void)
{
    string cmd="/etc/init.d/nicchk probe";
    string service="/etc/init.d/nicchk";
    int ret;
    static int nic_unkonwn_count = 0;
    
    #define NORMAL  0
    #define ERROR   1
    #define UNKONWN 2

    if(access(service.c_str(), X_OK) != 0)
    {
        Debug("access service script failed! ");
        return;
    }
    ret = system(cmd.c_str());
    ret = ret/256;
    if (UNKONWN != ret)
    {
        nic_unkonwn_count = 0;
        if(ERROR == ret)
        {
            Debug("nic status error,exit. \n");
            Log(LOG_EMERG,"service nicchk probe get ERROR state,system will reboot.");
            KillSelf();
        }
    }
    else
    {
        Debug("nic status unknown. \n");
        nic_unkonwn_count++;
        if( NIC_UNKNOWN_EXP < nic_unkonwn_count)
        {
            Debug("nic status unknown last for more than %d times,exit. \n",NIC_UNKNOWN_EXP);
            Log(LOG_EMERG,"service nicchk probe get UNKONWN state for more than %d times,system will reboot.",NIC_UNKNOWN_EXP);
            KillSelf();            
        }

    }    
}

pthread_t StartRealTimeThread(const char *name,int priority,int stacksize,ThreadEntry entry,void *arg)
{
    pthread_t thread_id;
    pthread_attr_t attr;
    
    pthread_attr_init(&attr);
    if ( pthread_attr_setinheritsched(&attr,PTHREAD_EXPLICIT_SCHED) != 0 )
    {
        printf("StartRealTimeThread: set attr failed! errno: %d.\n",errno);
        return 0;
    }
    //线程设置为detached模式
    pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED);

    //使用实时优先级
    struct sched_param scheparam;
    pthread_attr_setschedpolicy(&attr,SCHED_FIFO);
    pthread_attr_getschedparam(&attr,&scheparam);
    scheparam.__sched_priority  = priority;
    pthread_attr_setschedparam(&attr,&scheparam);

    if(stacksize > 0)
    {
        //自定义堆栈大小
        int mem_page_size = getpagesize(); /* 页大小 */
        stacksize = ((stacksize + mem_page_size - 1 )/mem_page_size)*mem_page_size;
        pthread_attr_setstacksize(&attr,stacksize);
    }

    if(0 != pthread_create(&thread_id,&attr,(ThreadEntry)entry,(void *)arg))
    {
        printf("StartRealTimeThread call pthread_create failed, errno = %d!\n",errno);
        return 0;
    }
    return thread_id;
}
int main(int argc,char **argv)
{
    bool daemon = true;
    int interval = 5; //默认每5秒钟监控一次
    string script("/opt/tecs/guard/guard.sh");
    string log("/var/log/tecs/guard.log");
    string pidfile("/var/run/tecs-guard.pid");
        
    // Long options
    const struct option long_opt[] =
        {
            { "debug",       0,  NULL,   'd'},
            { "foreground",  0,  NULL,   'f'},
            { "pidfile",     1,  NULL,   'p'},
            { "script",      1,  NULL,   's'},
            { "log",         1,  NULL,   'l'},
            { "interval",    1,  NULL,   'i'},
            { "help",        0,  NULL,   'h'},
            { NULL,          0,  NULL,   0}
        };

    int c;
    while ((c = getopt_long (argc, argv, "dfp:s:l:i:h", long_opt, NULL)) != -1)
    switch (c)
    {
        case 'd':
            debug = true;
            break;
            
        case 'f':
            daemon = false;
            break;
            
        case 'p':
            pidfile = optarg;
            break;
            
        case 's':
            script = optarg;
            break;

        case 'l':
            log = optarg;
            break;
            
        case 'i':
            if(true != from_string(interval,optarg,dec))
            {
                cerr << "failed to get interval: " << optarg << endl;
                exit(-1);
            }
            break;
            
        case 'h':
            show_options();
            return 0;
    }

    Debug("\n");
    Debug("script = %s\n",script.c_str());
    Debug("pidfile = %s\n",pidfile.c_str());
    Debug("log = %s\n",log.c_str());
    Debug("interval = %d\n",interval);
    
    if(access(script.c_str(), X_OK) != 0)
    {
        cerr << "access script failed! errno = " << errno << endl;
        exit(-1);
    }

    pid_t forkpid;
    if (daemon)
    {
        forkpid = fork();
    }
    else
    {
        forkpid = 0; //Do not fork
    }

    if (0 > forkpid)
    {
        cerr << "Error: Unable to fork! errno = " << errno << endl;
        exit(-1);
    }

    //父进程退出
    if (0 < forkpid)
    {
        exit(0);
    }

    if (daemon)
    {
        //创建pid文件
        pid_t oldpid;
        write_pidfile(pidfile.c_str(),&oldpid);
    }
    
    //如果是后台运行，需要关闭标准io
    if (daemon)
    {
        pid_t sid = setsid();
        if (sid == -1)
        {
            cerr << "Error: Unable to setsid.\n";
            exit(-1);
        }

        int fd = open("/dev/null", O_RDWR);
        if (fd < 0)
        {
            cerr << "failed to open /dev/null! errno = " << errno << endl;
            return false;
        }

        dup2(fd,0);
        dup2(fd,1);
        dup2(fd,2);
        close(fd);

        fcntl(0,F_SETFD,0); // Keep them open across exec funcs
        fcntl(1,F_SETFD,0);
        fcntl(2,F_SETFD,0);
    }

    Debug("guard is now starting ...\n");
    
    int pri = sched_get_priority_max(SCHED_FIFO);
    int *arg = new int(interval);
    if (0 == StartRealTimeThread("feed_dog", pri, 0, (ThreadEntry)feed_dog, (void *)arg))
    {
        cerr << "Error: Unable to StartRealTimeThread feed_dog! errno = " << errno << endl;
        exit(-1);
    }

    while (1)
    {
        guard(script,log);
        CheckNicStatus();
        sleep(interval);
    }
}


