/*********************************************************************
* 版权所有 (C)2012, 深圳市中兴通讯股份有限公司。
*
* 文件名称： igmpv3_report.h
* 文件标识： 
* 其它说明： 
* 当前版本： V1.0
* 作    者： liuxuefeng
* 完成日期： 
*
* 修改记录1：
*    修改日期：2012年03月28日
*    版 本 号：V1.0
*    修 改 人：liuxuefeng
*    修改内容：创建
* 修改记录2：
*    修改日期：2012年03月30日
*    版 本 号：V1.1
*    修 改 人：liuxuefeng
*    修改内容：程序嵌入TECS;增加守护进程模式

**********************************************************************/
#include "igmpv3_report.h"
#include <dirent.h>
#include <fcntl.h>
#include <ctype.h>


#define TECS_RUNNING_DIR "/var/run/tecs"
#define SYS_RUNNING_DIR  "/var/run/"  /* 暂时方案考虑对齐系统守护进程 */

static bool create_pid_file(const char *pcExeName, pid_t pid);
static const char *filename_without_path(const char *path);
static bool write_pidfile(const char *pidfile,pid_t *oldpid);
static void RemoveOldPidfile(const char *pcPrefix);

/*
功能:比较两个字符串，不区分大小写
参数: s1和s2 -- 参与比较的字符串、
             n --参与比较的字符数量
返回值: 0 -- 两个字符串相等
                  大于 0 -- s1 > s2
                  小于 0 -- s1 < s2
*/
static INT String_Compare_Nocase(const CHAR *s1, const CHAR *s2,INT n)
{
    INT i;
    if ((NULL == s1) || (NULL == s2))
    {
        return -1;
    }

    for (i = 0; i < n; i++)
    {
        if (toupper(s1[i]) != toupper(s2[i]))
        {
            break;
        }
		
        if (s1[i] == '\0')
        {
            break;
        }
    }
    
    return (i == n) ? 0 : ((INT)s1[i] - (INT)s2[i]);
}
XOS_STATUS XOS_DeleteFile(CHAR *pcFileName)
{
    unlink(pcFileName);
    return XOS_SUCCESS;
}
static bool create_pid_file(const char *pcExeName, pid_t pid)
{
    char piddir[256] = {'\0'};

    //使用默认的pidfile目录: /var/run/tecs/
    snprintf(piddir, sizeof(piddir),"%s",SYS_RUNNING_DIR);

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

    DIR *dir = opendir(SYS_RUNNING_DIR);   

    while((dir_env = readdir(dir)))
    {
        if (strstr(dir_env->d_name, pcPrefix) == NULL)
        {
            continue;
        }
 	
        if ((String_Compare_Nocase(dir_env->d_name, pcPrefix, strlen(pcPrefix))) == 0)
        {
            char pidfile[128] = {'\0'};
            snprintf(pidfile, sizeof(pidfile), "%s/%s", SYS_RUNNING_DIR, dir_env->d_name);
            XOS_DeleteFile(pidfile);
            //XOS_SysLog(OAM_CFG_LOG_DEBUG, "Success to remove old pid file %s", dir_env->d_name);
        }
    }

}
char g_ucIfname[IFNAMSIZ] = {0};
/************************************************************************************
* 函数名称： main
* 功能描述： 
* 访问的表： 无
* 修改的表： 无
* 输入参数：
* 输出参数： 无
* 返 回 值： 
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2012/03/28      V1.0       刘雪峰       创建
************************************************************************************/
int main (int argc, char **argv)
{
    pid_t forkpid;
    BOOLEAN bIsDaemon = TRUE;
    char ch;
    CHAR ucMac[ETH_HW_ADDR_LEN]={0};
    OSS_STATUS    tRet = OSS_ERR_UNKNOWN;
    
    /* 初始化为预设网口名 */
    strncpy(g_ucIfname, ISS_BASE_IF_NAME, IFNAMSIZ);
    while ((ch = getopt (argc, argv, "f")) != EOF) {
    	switch (ch) {
    		case 'f':
    			bIsDaemon = FALSE;
    			break;
    		default:
    			fprintf (stderr, "Usage:\n");
    			fprintf (stderr, "	-f	: Start application in forground.\n");
    			exit(0);
    	}
    }

    /* 第一个入参的处理 */
    if(NULL != argv[1])
    {
        if((IFNAMSIZ-1) >= strlen(argv[1]))
        {
            if(strncmp(argv[1],"-f", IFNAMSIZ) != 0)
            {
                /* 第一个参数如果不是"-f"选项则认为是传入的网口名 */
                strncpy(g_ucIfname,argv[1] , IFNAMSIZ);
                g_ucIfname[IFNAMSIZ - 1] = 0;
            }        

        }
        else
        {
             IGMP_Print("Error: first parameter  need input correct  net interface name,please use ifconfig look over! \n\r\n");
             exit(0);
        }
    }   

    /* 父进程通过获取控制面mac地址，检查网口名是否配置正确，不正确提示打印 */
    tRet = InterFaceGetMac(g_ucIfname,ucMac);
    if (OSS_SUCCESS != tRet)
    {
        IGMP_Print("main:InterFaceGetMac test Failed,tRet 0x%lx \n",tRet);
        IGMP_Print("Error: first parameter  need input correct  net interface name! \r\n");
        exit(0);;
    }
    if (TRUE == bIsDaemon )
    {
        forkpid = fork();
    }
    else
    {
        forkpid = 0; //Do not fork
    }

    if(0 > forkpid)
    {
        IGMP_Print("Error: Unable to fork.\n\r\n"); 
        exit(-1);
    }

    //父进程退出
    if(0 < forkpid)
    {        
        IGMP_Print("main:Will run in daemon mode! \n");
        exit(0);
    }

    //防止相同application+process的进程重复启动--IGMPV3可不考虑
   
    //如果是后台运行，需要关闭标准io
    if (TRUE == bIsDaemon)
    {
        pid_t sid;
        //创建pid文件
        if(FALSE == create_pid_file(filename_without_path(argv[0]), getppid()))
        {
            exit(-1);
        }

        sid = setsid();
        if (sid == -1) 
        {
            IGMP_Print("Error: Unable to setsid.\n");    
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


    if(FALSE == InitIgmpv3())
    {
        IGMP_Print("InitIgmpv3 fail.If in PC,do not care about it!  \n");    
        exit(0);
    }
    
    R_AddToUdpMultiGroup(MG_PLAT_OSS_IGMP_V3, NULL);        
    while(1)
    {
        sleep(10);
        IssSendIgmpReport();
    }
    
    return 0;
}

