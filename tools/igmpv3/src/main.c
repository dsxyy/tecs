/*********************************************************************
* ��Ȩ���� (C)2012, ����������ͨѶ�ɷ����޹�˾��
*
* �ļ����ƣ� igmpv3_report.h
* �ļ���ʶ�� 
* ����˵���� 
* ��ǰ�汾�� V1.0
* ��    �ߣ� liuxuefeng
* ������ڣ� 
*
* �޸ļ�¼1��
*    �޸����ڣ�2012��03��28��
*    �� �� �ţ�V1.0
*    �� �� �ˣ�liuxuefeng
*    �޸����ݣ�����
* �޸ļ�¼2��
*    �޸����ڣ�2012��03��30��
*    �� �� �ţ�V1.1
*    �� �� �ˣ�liuxuefeng
*    �޸����ݣ�����Ƕ��TECS;�����ػ�����ģʽ

**********************************************************************/
#include "igmpv3_report.h"
#include <dirent.h>
#include <fcntl.h>
#include <ctype.h>


#define TECS_RUNNING_DIR "/var/run/tecs"
#define SYS_RUNNING_DIR  "/var/run/"  /* ��ʱ�������Ƕ���ϵͳ�ػ����� */

static bool create_pid_file(const char *pcExeName, pid_t pid);
static const char *filename_without_path(const char *path);
static bool write_pidfile(const char *pidfile,pid_t *oldpid);
static void RemoveOldPidfile(const char *pcPrefix);

/*
����:�Ƚ������ַ����������ִ�Сд
����: s1��s2 -- ����Ƚϵ��ַ�����
             n --����Ƚϵ��ַ�����
����ֵ: 0 -- �����ַ������
                  ���� 0 -- s1 > s2
                  С�� 0 -- s1 < s2
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

    //ʹ��Ĭ�ϵ�pidfileĿ¼: /var/run/tecs/
    snprintf(piddir, sizeof(piddir),"%s",SYS_RUNNING_DIR);

    //���Ŀ¼�ַ�����β�Ƿ�Ϊ/�����û�оͲ���һ��
    piddir[sizeof(piddir)-1] = '\0';
    piddir[sizeof(piddir)-2] = '\0';
    if(*(piddir+strlen(piddir)-1) != '/' )
    {
        *(piddir+strlen(piddir)) = '/';
    }

    //���Ŀ¼�������ڣ��ʹ���һ��
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

    //����pidfile�ļ���: ��ִ���ļ����� + pid + ".pid"��׺
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
    
    /* �ҵ�б�ܳ��ֵ����λ�ã�����Ҳ����򷵻�NULL */
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
    pid�ļ������ݸ�ʽ���б�׼�涨�ģ��μ�: http://www.pathname.com/fhs/2.2/fhs-5.13.html
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
    //���ܹر��ļ�����������ʧЧ��!!!
    return true;
}

/*ɾ���ɵ�pid�ļ�*/
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
* �������ƣ� main
* ���������� 
* ���ʵı� ��
* �޸ĵı� ��
* ���������
* ��������� ��
* �� �� ֵ�� 
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2012/03/28      V1.0       ��ѩ��       ����
************************************************************************************/
int main (int argc, char **argv)
{
    pid_t forkpid;
    BOOLEAN bIsDaemon = TRUE;
    char ch;
    CHAR ucMac[ETH_HW_ADDR_LEN]={0};
    OSS_STATUS    tRet = OSS_ERR_UNKNOWN;
    
    /* ��ʼ��ΪԤ�������� */
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

    /* ��һ����εĴ��� */
    if(NULL != argv[1])
    {
        if((IFNAMSIZ-1) >= strlen(argv[1]))
        {
            if(strncmp(argv[1],"-f", IFNAMSIZ) != 0)
            {
                /* ��һ�������������"-f"ѡ������Ϊ�Ǵ���������� */
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

    /* ������ͨ����ȡ������mac��ַ������������Ƿ�������ȷ������ȷ��ʾ��ӡ */
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

    //�������˳�
    if(0 < forkpid)
    {        
        IGMP_Print("main:Will run in daemon mode! \n");
        exit(0);
    }

    //��ֹ��ͬapplication+process�Ľ����ظ�����--IGMPV3�ɲ�����
   
    //����Ǻ�̨���У���Ҫ�رձ�׼io
    if (TRUE == bIsDaemon)
    {
        pid_t sid;
        //����pid�ļ�
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

