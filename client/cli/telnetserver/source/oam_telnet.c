/*********************************************************************
* Version Right (C)2003, ZTE Corporation
*
* File Name      : oss_rdr.c
* Other          : NULL
* Current Version: V1.0
* Author         : Newton
* Finish Date    :
*
* Modify Record 1:
*    Modify Date : Dec. 3rd,2006.
*    Version     : V1.0
*    Author      : Newton
*    Content     : Newton Added for Linux Only
*                  For the purpose of migration from one OS to another OS smoothly, we just use system head files.
**********************************************************************/
#include "includes.h"
#include "netinet/in.h"
#include   <arpa/inet.h>   
#include <netinet/tcp.h>
#include <sys/types.h>
#include <fcntl.h>
#include <time.h>
#include "basesock.h"
#include "telserver.h"
#include "rdtelnet.h"
#include "clivty.h"
#if (defined OS_VXWORKS)
#include "sockLib.h"
#include <sys/socket.h>
#endif

#define TNET_SERVER_PORT                    24                   
#define SSH_SERVER_PORT                      26                   
#define TNET_LENGTH_OF_LISTEN_QUEUE         10              
#define TNET_BUFFER_SIZE                    255
#define TNET_WELCOME_MESSAGE                "Welcome! \n\r"
#define TNET_REJECT_MESSAGE                 "Connection denied! \n\r"
#define TNET_MAX_COUNT                      10
#define SSH_MAX_COUNT						4
#define TASK_NAME_MAX_LEN                   128
#define OAM_REGFUNC_CONNECT                 1
#define OAM_REGFUNC_RECEIVE                 2
#define OAM_REGFUNC_CLOSE                   3

#define TSK_PRIO_PTYS_IN                    62

typedef struct{
    WORDPTR inputtask;
    WORDPTR outputTask;
    WORD32  sockfd;
    WORD32 startOutput;
    WORD32 startInput;
    BOOLEAN loggedIn;
    BOOLEAN busyFlag;
}T_TELNET_SESSION;

typedef struct{
    WORDPTR inputtask;
    WORDPTR outputTask;
    WORD32  sockfd;
    WORD32 startOutput;
    WORD32 startInput;
    BOOLEAN loggedIn;
    BOOLEAN busyFlag;
}T_SSH_SESSION;

int (*pfOamConnect)(WORD16 fd, WORD16 flag, struct sockaddr_in clientAddr);
int (*pfOamReceive)(WORD16 fd, WORD16 flag,LPVOID buf, WORD16 len);
int (*pfOamClose)(WORD16 fd, WORD16 flag);


typedef int (*FUNCPTROamConnect)(WORD16 fd, WORD16 flag, struct sockaddr_in clientAddr);
typedef int (*FUNCPTROamReceive)(WORD16 fd, WORD16 flag,LPVOID buf, WORD16 len);
typedef int (*FUNCPTROamClose)(WORD16 fd, WORD16 flag);
BOOLEAN    telOAMFuncSetRegistered     =FALSE;
BOOLEAN    SSHOAMFuncSetRegistered     =FALSE;
#define TELNET_CONNECTION           2
#define OAM_REGFUNC_CONNECT         1
#define OAM_REGFUNC_RECEIVE         2
#define OAM_REGFUNC_CLOSE           3
#define TELNET_IO_STACK_SIZE  ((int)0x10000)  
#define TELNET_LENGTH_OF_LISTEN_QUEUE 10 
#define TNET_BUFFER_SIZE 255

/*#ifdef PTY_DEMO
#define   oss_telnetdInit       main
#endif*/

static WORD32   tNetSock;
static WORD32   telnetnumOfClients =    0;
static WORD32   tNetInitFlag =          0;
static T_TELNET_SESSION        * g_telnetSession;

static WORD32   SSHSock;
static WORD32   SSHnumOfClients =    0;
static WORD32   SSHInitFlag =          0;
static T_SSH_SESSION        * g_SSHSession;

WORD32 tNetMutexSem;
WORD32 SSHMutexSem;
XOS_TIMER_ID g_tTimer_CheckSendBuf;
static  BOOLEAN SaveDataToBuf(sty *pSty, LPVOID buf, WORD16 len);
	
void R_telnetDaemon(void);
int R_InitTelnetSock(void);
void telNetInTask(T_TELNET_SESSION  *pSlot);
int  telnetTasksCreate(T_TELNET_SESSION *pSlot);
void tNetSessionDisconnect(WORD16 fd,BOOL pSessionDelete);
void tNetTaskDelete(int 	numTasks);

void R_SSHDaemon(void);
int R_InitSSHSock(void);
void SSHInTask(T_SSH_SESSION  *pSlot);
int  SSHTasksCreate(T_SSH_SESSION *pSlot);
void SSHSessionDisconnect(WORD16 fd,BOOL pSessionDelete);
void SSHTaskDelete(int 	numTasks);

/**********************************************************************
* Function          : int  oss_rdrinit()
* Desciption        : This procedure is made to redirect input and output to our control file discriptor
* Require Table     : NULL
* Modify  Table     : NULL
* Input Parameter   : NULL
* Output Parameter  : NULL
* Return Value      : Success 0; Fail -1
* Others            :
* Modify Date   Version    Author      Content
* ---------------------------------------------------------------------
* Dec. 7th,2006   V1.0     Newton       Create
************************************************************************/
int  oss_telnetdInit(void)
{

    int count;

    if (tNetInitFlag)
        return 0;

    if (SSHInitFlag)
        return 0;

    if (FALSE == XOS_CreateSemaphore(
                0,
                0,
                MUTEX_STYLE,
                &tNetMutexSem)
       )
    {
        return (ERROR);
    }
    if (FALSE == XOS_CreateSemaphore(
                0,
                0,
                MUTEX_STYLE,
                &SSHMutexSem)
       )
    {
        return (ERROR);
    }
    R_InitTelnetSock();
    R_InitSSHSock();
    g_telnetSession=(T_TELNET_SESSION*)XOS_GetUB(TNET_MAX_COUNT*sizeof(T_TELNET_SESSION));
    g_SSHSession=(T_SSH_SESSION*)XOS_GetUB(SSH_MAX_COUNT*sizeof(T_SSH_SESSION));

    if (g_telnetSession == NULL || g_SSHSession == NULL)
    {
    #ifdef VOS_VXWORKS
        semDelete ((SEM_ID)&tNetMutexSem);
        semDelete ((SEM_ID)&SSHMutexSem);
    #endif
        /*R_Printf("fail to get telnet session ub");*/
        return (ERROR);
    }
    for (count = 0; count <TNET_MAX_COUNT ; count++)
    {
        g_telnetSession[count].sockfd=-1;
        g_telnetSession[count].loggedIn=FALSE;
        g_telnetSession[count].busyFlag=FALSE;
        if(telnetTasksCreate( &g_telnetSession[count])==ERROR)
        {
            tNetTaskDelete(count);
            return ERROR;
        }

    }
    for (count = 0; count <SSH_MAX_COUNT ; count++)
    {
        g_SSHSession[count].sockfd=-1;
        g_SSHSession[count].loggedIn=FALSE;
        g_SSHSession[count].busyFlag=FALSE;
        if(SSHTasksCreate( &g_SSHSession[count])==ERROR)
        {
            SSHTaskDelete(count);
            return ERROR;
        }

    }
    /* TCP需要专门的监听任务 */
    XOS_StartTask("R_telnetDaemon", TSK_PRIO_PTYS_IN, TELNET_IO_STACK_SIZE, 0, (VOID (*)(void*))R_telnetDaemon, 0);
    XOS_StartTask("R_SSHDaemon", TSK_PRIO_PTYS_IN, TELNET_IO_STACK_SIZE, 0, (VOID (*)(void*))R_SSHDaemon, 0);

    return 0;
}

int telnetTasksCreate(T_TELNET_SESSION *pSlot)
{
    CHAR sessionTaskName[TASK_NAME_MAX_LEN];
    CHAR sessionInTaskName[TASK_NAME_MAX_LEN];
    XOS_snprintf (sessionTaskName, sizeof(sessionTaskName),"_%lx", (unsigned long int)pSlot);
    XOS_snprintf (sessionInTaskName, sizeof(sessionInTaskName),"tTelnetIn%s", sessionTaskName);
    if (XOS_CreateSemaphore(0, 0, SYNC_SEM_STYLE, &pSlot->startInput) == FALSE)
    {
        /*R_DbgPrintf(PRINT_MODULE_PRINTF, PRN_LEVEL_HIGH ,"R_InitTelnetSock: Failed to creeate syn sem!\n");*/
        return ERROR;
    }
    pSlot->inputtask =XOS_StartTask(sessionInTaskName, TSK_PRIO_PTYS_IN, TELNET_IO_STACK_SIZE, 0, (VOID (*)(void*))telNetInTask, (WORDPTR)pSlot);
    return 0;
}

int SSHTasksCreate(T_SSH_SESSION *pSlot)
{
    CHAR sessionTaskName[TASK_NAME_MAX_LEN];
    CHAR sessionInTaskName[TASK_NAME_MAX_LEN];
    XOS_snprintf (sessionTaskName, sizeof(sessionTaskName),"_%lx", (unsigned long int)pSlot);
    XOS_snprintf (sessionInTaskName, sizeof(sessionInTaskName),"tSSHIn%s", sessionTaskName);
    if (XOS_CreateSemaphore(0, 0, SYNC_SEM_STYLE, &pSlot->startInput) == FALSE)
    {
        /*R_DbgPrintf(PRINT_MODULE_PRINTF, PRN_LEVEL_HIGH ,"R_InitTelnetSock: Failed to creeate syn sem!\n");*/
        return ERROR;
    }
    pSlot->inputtask =XOS_StartTask(sessionInTaskName, TSK_PRIO_PTYS_IN, TELNET_IO_STACK_SIZE, 0, (VOID (*)(void*))SSHInTask,  (WORDPTR)pSlot);
    return 0;
}


void tNetTaskDelete(int 	numTasks)
{
    int 	count;


    for (count = 0; count < numTasks; count++)
    {
        tNetSessionDisconnect(g_telnetSession[count].sockfd, TRUE);
    }
    OAM_RETUB(g_telnetSession);
#ifdef VOS_VXWORKS
    semDelete ((SEM_ID)&tNetMutexSem);
#endif
    return;
}

void SSHTaskDelete(int 	numTasks)
{
    int 	count;
    for (count = 0; count < 1; count++)
    {
        SSHSessionDisconnect(g_SSHSession[count].sockfd, TRUE);
    }
    OAM_RETUB(g_SSHSession);
#ifdef VOS_VXWORKS
    semDelete ((SEM_ID)&tNetMutexSem);
#endif
    return;
}

int R_InitTelnetSock(void)
{
    int bReuseaddr = 1;
    struct sockaddr_in serverAddr;
    int flags;
    
    if ((tNetSock = socket(AF_INET,SOCK_STREAM,0)) < 0)
    {
        /*R_DbgPrintf(PRINT_MODULE_PRINTF, PRN_LEVEL_HIGH ,"create socket error!\n");*/
        return -1;
    }

    /* 允许重用本地地址和端口 */
    setsockopt(tNetSock,SOL_SOCKET,SO_REUSEADDR,(CHAR*)&bReuseaddr,sizeof(bReuseaddr));

    bzero((CHAR *)&serverAddr,sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(TNET_SERVER_PORT);
    serverAddr.sin_addr.s_addr = htons(INADDR_ANY); /* 是否需要在所有地址上打开侦听？ */
    /*
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    */
    /*
    sockaddr_in和sockaddr是并列的结构，指向sockaddr_in的结构体的指针也可以指向
    sockadd的结构体，并代替它。也就是说，你可以使用sockaddr_in建立你所需要的信息,
    在最后用进行类型转换就可以了
    */
    if (bind(tNetSock,(struct sockaddr*)&serverAddr,sizeof(serverAddr))<0)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "ERROR::bind to port [%u] failed!Cli exit, please check whether cli is already runing\n",TNET_SERVER_PORT);
        /*R_DbgPrintf(PRINT_MODULE_PRINTF, PRN_LEVEL_HIGH ,"R_InitTelnetSock：bind to port %d failure!\n",TNET_SERVER_PORT);*/
        close(tNetSock);

        exit(-1);
//        return -1;
    }

    if (listen(tNetSock,TELNET_LENGTH_OF_LISTEN_QUEUE) < 0)
    {
        /*R_DbgPrintf(PRINT_MODULE_PRINTF, PRN_LEVEL_HIGH ,"R_InitTelnetSock: Failed to listen on the socket!\n");*/
        close(tNetSock);
        return -1;
    }

    flags = fcntl(tNetSock, F_GETFL, 0);
    if (flags < 0)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "R_InitTelnetSock: Getfl failed!\n\r");
    }
    flags |= O_NONBLOCK;
    
    /*设置该连接为非阻塞模式*/
    if (fcntl(tNetSock, F_SETFL, flags) < 0)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "R_InitTelnetSock: Setfl failed!\n\r");
    }
    
    /*611001013168 【ETCA】DDM查询RPU上路由信息时，有时会出现回显信息不全*/
    g_tTimer_CheckSendBuf = INVALID_TIMER_ID;
	
    /*sem_init(&tNetActiveSockSem,0,0); */

    /*XOS_StartTask("telNetInTask", TSK_PRIO_PTYS_IN, TELNET_IO_STACK_SIZE, 0, (void *)telNetInTask, 1);*/
    tNetInitFlag = 1;
    return 1;
}

int R_InitSSHSock(void)
{
    int bReuseaddr = 1;
    struct sockaddr_in serverAddr;
    int flags;
    
    if ((SSHSock = socket(AF_INET,SOCK_STREAM,0)) < 0)
    {
        /*R_DbgPrintf(PRINT_MODULE_PRINTF, PRN_LEVEL_HIGH ,"create socket error!\n");*/
        return -1;
    }

    /* 允许重用本地地址和端口 */
    setsockopt(SSHSock,SOL_SOCKET,SO_REUSEADDR,(CHAR*)&bReuseaddr,sizeof(bReuseaddr));

    bzero((CHAR *)&serverAddr,sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SSH_SERVER_PORT);
    serverAddr.sin_addr.s_addr = htons(INADDR_ANY); /* 是否需要在所有地址上打开侦听？ */
    /*
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    */
    /*
    sockaddr_in和sockaddr是并列的结构，指向sockaddr_in的结构体的指针也可以指向
    sockadd的结构体，并代替它。也就是说，你可以使用sockaddr_in建立你所需要的信息,
    在最后用进行类型转换就可以了
    */
    if (bind(SSHSock,(struct sockaddr*)&serverAddr,sizeof(serverAddr))<0)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "ERROR::bind to port [%u] failed! Cli exit, please check whether cli is already runing\n",SSH_SERVER_PORT);
        /*R_DbgPrintf(PRINT_MODULE_PRINTF, PRN_LEVEL_HIGH ,"R_InitTelnetSock：bind to port %d failure!\n",TNET_SERVER_PORT);*/
        close(SSHSock);

        exit(-1);
    }

    if (listen(SSHSock,TELNET_LENGTH_OF_LISTEN_QUEUE) < 0)
    {
        /*R_DbgPrintf(PRINT_MODULE_PRINTF, PRN_LEVEL_HIGH ,"R_InitTelnetSock: Failed to listen on the socket!\n");*/
        close(SSHSock);
        return -1;
    }

    flags = fcntl(SSHSock, F_GETFL, 0);
    if (flags < 0)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "R_InitSSHSock: Getfl failed!\n\r");
    }
    flags |= O_NONBLOCK;
    
    /*设置该连接为非阻塞模式*/
    if (fcntl(SSHSock, F_SETFL, flags) < 0)
    {
        XOS_SysLog(OAM_CFG_LOG_ERROR, "R_InitSSHSock: Setfl failed!\n\r");
    }

    /*sem_init(&tNetActiveSockSem,0,0); */

    /*XOS_StartTask("telNetInTask", TSK_PRIO_PTYS_IN, TELNET_IO_STACK_SIZE, 0, (void *)telNetInTask, 1);*/
    SSHInitFlag = 1;
    return 1;
}

T_TELNET_SESSION  * tNetSessionAdd(void)
{
    int   count;
    T_TELNET_SESSION  *pSlot;
    XOS_SemaphoreP((WORD32)tNetMutexSem, (WORD32)WAIT_FOREVER);
    if(telnetnumOfClients==TNET_MAX_COUNT)
    {
        XOS_SemaphoreV(tNetMutexSem);
        /*R_Printf("execude the max ");*/
        return (NULL);
    }
    /* Find an idle pair of input/output tasks, if needed. */
    for (count = 0; count < TNET_MAX_COUNT; count++)
    {
        if (!g_telnetSession[count].busyFlag)
        {
            break;
        }
    }
    if ( (count == TNET_MAX_COUNT))
    {
        XOS_SemaphoreV(tNetMutexSem);
        return (NULL);
    }
    pSlot = g_telnetSession+count;
    pSlot->busyFlag = TRUE;
    pSlot->loggedIn = FALSE;
    telnetnumOfClients++;
    XOS_SemaphoreV(tNetMutexSem);
    return (pSlot);

}

T_SSH_SESSION  * SSHSessionAdd(void)
{
    int   count;
    T_SSH_SESSION  *pSlot;
    XOS_SemaphoreP((WORD32)SSHMutexSem, (WORD32)WAIT_FOREVER);
    if(SSHnumOfClients==SSH_MAX_COUNT)
    {
        XOS_SemaphoreV(SSHMutexSem);
        /*R_Printf("execude the max ");*/
        return (NULL);
    }
    /* Find an idle pair of input/output tasks, if needed. */
    for (count = 0; count < SSH_MAX_COUNT; count++)
    {
        if (!g_SSHSession[count].busyFlag)
        {
            break;
        }
    }
    if ( (count == SSH_MAX_COUNT))
    {
        XOS_SemaphoreV(SSHMutexSem);
        return (NULL);
    }
    pSlot = g_SSHSession+count;
    pSlot->busyFlag = TRUE;
    pSlot->loggedIn = FALSE;
    SSHnumOfClients++;
    XOS_SemaphoreV(SSHMutexSem);
    return (pSlot);

}
void tNetSessionDisconnect(WORD16 fd,BOOL pSessionDelete)
{
    CHAR sessionTaskName[TASK_NAME_MAX_LEN];
    CHAR sessionInTaskName[TASK_NAME_MAX_LEN];
    T_TELNET_SESSION *pSlot;
    int loop;

    XOS_SysLog(OAM_CFG_LOG_NOTICE, "611000956833:Enter into tNetSessionDisconnect...\n");
    
    for(loop=0;loop<TNET_MAX_COUNT;loop++)
    {
        if(fd==g_telnetSession[loop].sockfd)
        {
            break;
        }
    }
    if(loop<TNET_MAX_COUNT)
    {
        pSlot=g_telnetSession+loop;
    }
    else
    {
        return ;
    }

    XOS_snprintf (sessionTaskName, sizeof(sessionTaskName),"_%lx", (WORDPTR)pSlot);
    XOS_SysLog(OAM_CFG_LOG_NOTICE, "611000956835:sessionTaskName = [%s]\n", sessionTaskName);
    
    XOS_snprintf (sessionInTaskName, sizeof(sessionInTaskName),"tTelnetIn%s", sessionTaskName);
    XOS_SysLog(OAM_CFG_LOG_NOTICE, "611000956835:sessionInTaskName = [%s]\n", sessionInTaskName);
    
    XOS_SemaphoreP((WORD32)tNetMutexSem, (WORD32)WAIT_FOREVER);
    XOS_SysLog(OAM_CFG_LOG_NOTICE, "611000956833:Call XOS_SemaphoreP end...\n");
    
    close (pSlot->sockfd);
    XOS_SysLog(OAM_CFG_LOG_NOTICE, "611000956833:Call close end...\n");
    
    if(!pSessionDelete)/*restart the task */
    {
        XOS_SysLog(OAM_CFG_LOG_NOTICE, "611000956833:pSessionDelete is FALSE...\n");
        if (pSlot->inputtask)
        {        
            XOS_SysLog(OAM_CFG_LOG_NOTICE, "611001089268:pSlot->inputtask = [%ld]\n", pSlot->inputtask);
            XOS_DeleteTask(pSlot->inputtask);/*杀掉Telnet进程*/
            XOS_SysLog(OAM_CFG_LOG_NOTICE, "611000956833:Call XOS_DeleteTask end...\n");
        }
        XOS_SysLog(OAM_CFG_LOG_NOTICE, "611000956833:if (pSlot->inputtask) end...\n");
        
        #ifdef VOS_VXWORKS
        if (pSlot->startInput)
            XOS_SemaphoreP((WORD32)pSlot->startInput, (WORD32)NO_WAIT);
        #endif 
        if (pSlot->inputtask)
        {
            /*启动Telnet任务*/
            XOS_SysLog(OAM_CFG_LOG_NOTICE, "611000956833:Call XOS_StartTask start. sessionInTaskName = [%s].\n", sessionInTaskName);
            pSlot->inputtask =XOS_StartTask(sessionInTaskName, TSK_PRIO_PTYS_IN, TELNET_IO_STACK_SIZE, 0, (VOID (*)(void*))telNetInTask, (WORDPTR)pSlot);
            XOS_SysLog(OAM_CFG_LOG_NOTICE, "611000956833:Call XOS_StartTask end. ret = %d.\n", pSlot->inputtask);
        }
        XOS_SysLog(OAM_CFG_LOG_NOTICE, "611000956833:if (pSlot->inputtask) end...\n");
        
        pSlot->sockfd        = -1;
        pSlot->loggedIn      = FALSE;
        pSlot->busyFlag      = FALSE;
        --telnetnumOfClients;
    }
    else/*delete the task */
    {
        XOS_SysLog(OAM_CFG_LOG_NOTICE, "611000956833:pSessionDelete is TRUE...\n");
        
        /* Remove the i/o tasks */
        if (pSlot->inputtask)
        {
        #ifdef VOS_VXWORKS
            taskDelete (pSlot->inputtask);
        #elif defined  VOS_LINUX
            td(sessionInTaskName);
        #endif
        }
        /* Delete the semaphores */
        if (pSlot->startInput)
        {
        #ifdef VOS_VXWORKS
            semDelete ((SEM_ID)&pSlot->startInput);
        #endif
        }
        pSlot->sockfd        = -1;
        pSlot->loggedIn      = FALSE;
        pSlot->busyFlag      = FALSE;

    }
    XOS_SysLog(OAM_CFG_LOG_NOTICE, "611000956833:Call XOS_SemaphoreV start...\n");
    XOS_SemaphoreV(tNetMutexSem);
    XOS_SysLog(OAM_CFG_LOG_NOTICE, "611000956833:Call XOS_SemaphoreV end...\n");
}

void SSHSessionDisconnect(WORD16 fd,BOOL pSessionDelete)
{
    CHAR sessionTaskName[TASK_NAME_MAX_LEN];
    CHAR sessionInTaskName[TASK_NAME_MAX_LEN];
    T_SSH_SESSION *pSlot;
    int loop;
    for(loop=0;loop<SSH_MAX_COUNT;loop++)
    {
        if(fd==g_SSHSession[loop].sockfd)
        {
            break;
        }
    }
    if(loop<SSH_MAX_COUNT)
    {
        pSlot=g_SSHSession+loop;
    }
    else
    {
        return ;
    }

    XOS_snprintf (sessionTaskName, sizeof(sessionTaskName),"_%lx", (WORDPTR)pSlot);
    XOS_snprintf (sessionInTaskName, sizeof(sessionInTaskName),"tSSHIn%s", sessionTaskName);
    XOS_SemaphoreP((WORD32)SSHMutexSem, (WORD32)WAIT_FOREVER);
    close (pSlot->sockfd);
    if(!pSessionDelete)/*restart the task */
    {
        if (pSlot->inputtask)
        {
            XOS_DeleteTask(pSlot->inputtask);/*杀掉SSH进程*/
        }
        #ifdef VOS_VXWORKS
        if (pSlot->startInput)
            XOS_SemaphoreP((WORD32)pSlot->startInput, (WORD32)NO_WAIT);
        #endif 
        if (pSlot->inputtask)
        {
            /*启动SSH任务*/
            pSlot->inputtask =XOS_StartTask(sessionInTaskName, TSK_PRIO_PTYS_IN, TELNET_IO_STACK_SIZE, 0, (VOID (*)(void*))SSHInTask, (WORDPTR)pSlot);
        }
        pSlot->sockfd        = -1;
        pSlot->loggedIn      = FALSE;
        pSlot->busyFlag      = FALSE;
        --SSHnumOfClients;
    }
    else/*delete the task */
    {
        /* Remove the i/o tasks */
        if (pSlot->inputtask)
        {
        #ifdef VOS_VXWORKS
            taskDelete (pSlot->inputtask);
        #elif defined  VOS_LINUX
            td(sessionInTaskName);
        #endif
        }
        /* Delete the semaphores */
        if (pSlot->startInput)
        {
        #ifdef VOS_VXWORKS
            semDelete ((SEM_ID)&pSlot->startInput);
        #endif
        }
        pSlot->sockfd        = -1;
        pSlot->loggedIn      = FALSE;
        pSlot->busyFlag      = FALSE;

    }
    XOS_SemaphoreV(SSHMutexSem);
}

void R_telnetDaemon(void)
{
    struct sockaddr_in clientAddr;
    int newSock;
    
    int on = 1;
/*    int keepIdle = 6; 
    int keepInterval = 5; 
    int keepCount = 3; */

    CHAR buf[TNET_BUFFER_SIZE] = {'\0'};
    unsigned int length = sizeof(clientAddr);
    T_TELNET_SESSION  *pSlot;
    XOS_SysLog(OAM_CFG_LOG_NOTICE, "telnet  daemon starting ...\n\r");
    while(1)
    {
        newSock = accept(tNetSock,(struct sockaddr*)&clientAddr,&length);
        if (newSock < 0)
        {
            /*XOS_SysLog(OAM_CFG_LOG_NOTICE, "UserShellDaemon: Failed to accept a new connection!\n\r");*/
            XOS_Delay(3000);
            continue;
        }
        /*611000064496 tcp 探测时间和次数设置*/
        /*setsockopt(newSock, IPPROTO_TCP, TCP_KEEPIDLE, (void *)&keepIdle, sizeof(keepIdle)); */
        /*setsockopt(newSock, IPPROTO_TCP,TCP_KEEPINTVL, (void *)&keepInterval, sizeof(keepInterval)); */
        /*setsockopt(newSock, IPPROTO_TCP, TCP_KEEPCNT, (void *)&keepCount, sizeof(keepCount)); */
      
        /*CQ:CCRDCR00428086*/
        /*开启SO_KEEPALIVE选项*/
        setsockopt(newSock,SOL_SOCKET,SO_KEEPALIVE,(CHAR*)&on,sizeof(on));
              
        /* 打开一个TCP SESSION */
        pSlot = tNetSessionAdd ();
        if (pSlot == NULL)
        {
            /* Prevent denial of service attack by waiting 5 seconds */
            XOS_SysLog(OAM_CFG_LOG_NOTICE, "\r\nSorry, session limit reached.\r\n");
            XOS_Delay(5000);
            close (newSock);
            continue;
        }
        pSlot->sockfd = newSock;
        /*插入认证过程*/
        pSlot->loggedIn = TRUE;
        strncpy(buf,TNET_WELCOME_MESSAGE, sizeof(buf));
        send(newSock,buf,TNET_BUFFER_SIZE,0);
        /* write(newSock,buf,TNET_BUFFER_SIZE);*/

     
        XOS_SysLog(OAM_CFG_LOG_NOTICE, "\n\rA remote user shell connection from client,IP:%s,Port:%u\n\r", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port) );

        if(telOAMFuncSetRegistered)
        {
       printf("pfOamConnect ...... \n");
            pfOamConnect(newSock,TELNET_CONNECTION,clientAddr);
        }
        /* oss_prn(g_globalout,"R_UserShellDaemon: give activeSockSem...\n"); */
        /*sem_post(&tNetActiveSockSem);*/
        XOS_SemaphoreV((WORD32)pSlot->startInput);


    }
}

void R_SSHDaemon(void)
{
    struct sockaddr_in clientAddr;
    int newSock;
    
    int on = 1;
    int keepIdle = 6; 
    int keepInterval = 5; 
    int keepCount = 3; 
    
    /*CHAR buf[TNET_BUFFER_SIZE] = {'\0'};*/
    unsigned int  length = sizeof(clientAddr);
    T_SSH_SESSION  *pSlot;
    /*R_Printf("telnet  daemon starting ...\n ");*/
    while(1)
    {
        newSock = accept(SSHSock,(struct sockaddr*)&clientAddr,&length);
        if (newSock < 0)
        {
            /*R_DbgPrintf(PRINT_MODULE_PRINTF, PRN_LEVEL_HIGH ,"UserShellDaemon: Failed to accept a new connection!\n");*/
            XOS_Delay(3000);
            continue;
        }
        
        /*611000064496 tcp 探测时间和次数设置*/
        setsockopt(newSock, SOL_TCP, TCP_KEEPIDLE, (void *)&keepIdle, sizeof(keepIdle)); 
        setsockopt(newSock, SOL_TCP,TCP_KEEPINTVL, (void *)&keepInterval, sizeof(keepInterval)); 
        setsockopt(newSock, SOL_TCP, TCP_KEEPCNT, (void *)&keepCount, sizeof(keepCount)); 
      
        /*CQ:CCRDCR00428086*/
        /*开启SO_KEEPALIVE选项*/
        setsockopt(newSock,SOL_SOCKET,SO_KEEPALIVE,(CHAR*)&on,sizeof(on));
        
        /* 打开一个TCP SESSION */
        pSlot = SSHSessionAdd ();
        if (pSlot == NULL)
        {
            /*R_Printf( "\r\nSorry, session limit reached.\r\n");*/
            /* Prevent denial of service attack by waiting 5 seconds */
            XOS_Delay(5000);
            close (newSock);
            continue;
        }
        pSlot->sockfd = newSock;
        /*插入认证过程*/
        pSlot->loggedIn = TRUE;
        /* del by wl
        strcpy(buf,TNET_WELCOME_MESSAGE);
        send(newSock,buf,TNET_BUFFER_SIZE,0);
        */
        /* write(newSock,buf,TNET_BUFFER_SIZE);*/

        /*R_Printf("\nA remote user shell connection from client,IP:%s,Port:%d\n",
                inet_ntoa(clientAddr.sin_addr),
                ntohs(clientAddr.sin_port)
                );*/

        if(SSHOAMFuncSetRegistered)
        {

            pfOamConnect(newSock,CONNECTION_FROM_SSH,clientAddr);
        }
        /* oss_prn(g_globalout,"R_UserShellDaemon: give activeSockSem...\n"); */
        /*sem_post(&tNetActiveSockSem);*/
        XOS_SemaphoreV((WORD32)pSlot->startInput);


    }
}

void telNetInTask(T_TELNET_SESSION  *pSlot)
{
    CHAR buf[512] = {'\0'};
    int ret = 0;
    int sockfd;
#if 1 
    /*611001834936 改为非阻塞方式读取数据*/
    while (1)
    {
        int iRecved = 0;
        /* 等待活动的远程shell连接进来 */
        /*sem_wait(&tNetActiveSockSem);*/
        XOS_SemaphoreP(pSlot->startInput, (WORD32)WAIT_FOREVER);
        sockfd=pSlot->sockfd;
       
        while(1)
        { 
            static fd_set fds; 
            int maxfdp;
            struct timeval timeout={10,0}; 

            FD_ZERO(&fds); 
            FD_SET(sockfd,&fds); 
            maxfdp=sockfd+1;
        
            iRecved = 0;
            switch(select(maxfdp,&fds,NULL,NULL,&timeout))
            { 
            case -1: 
               // exit(-1);
                break;
            case 0:
                break;
            default: 
                if(FD_ISSET(sockfd,&fds)) 
                { 
                    iRecved = 1;
                    ret = recv(sockfd,buf,sizeof(buf),MSG_DONTWAIT);
                    /*call oam function*/
                    if(telOAMFuncSetRegistered && (ret > 0))
                    {
                        pfOamReceive(sockfd,TELNET_CONNECTION,buf, ret);
                    }
                    memset(buf,'\0',sizeof(buf));
                }
                break;
           }/*end switch*/

           if((strcmp("exit",buf) == 0) || 
             (strcmp("quit",buf) == 0) || 
             ((iRecved == 1) && (ret <= 0))/*soket 已断开*/
             )
           {
              memset(buf,'\0',sizeof(buf));
              break;
           }
         } /*end while*/

         /*R_Printf("The telnet client has disconnected!\n");*/
         /*fj temp add for test */
         if(telOAMFuncSetRegistered && (sockfd>0))
         {
             pfOamClose(sockfd,TELNET_CONNECTION);
         }
        
         /* 结束一个TCP SESSION */
         if(pSlot->sockfd )
         {
             close(pSlot->sockfd );
             pSlot->sockfd = -1;
             pSlot->loggedIn = FALSE;
             pSlot->busyFlag=FALSE;
             telnetnumOfClients --;
         }
    }

#else   

    while(1)
    {
        /* 等待活动的远程shell连接进来 */
        /*sem_wait(&tNetActiveSockSem);*/
        XOS_SemaphoreP(pSlot->startInput, (WORD32)WAIT_FOREVER);
        sockfd=pSlot->sockfd;
        /*while ((ret = read (sockfd, buf, sizeof (buf))) > 0)*/
        while ((ret = recv(sockfd, buf, sizeof (buf), 0)) > 0)
        {


            if((strcmp("exit",buf) == 0) || (strcmp("quit",buf) == 0))
            {
                memset(buf,'\0',sizeof(buf));
                break;
            }

            /* n = write(s_pm,buf,n);*/
            /*call oam function*/
            if(telOAMFuncSetRegistered)
            {
                pfOamReceive(sockfd,TELNET_CONNECTION,buf, ret);
            }
            memset(buf,'\0',sizeof(buf));
        };


        /*R_Printf("The telnet client has disconnected!\n");*/
        /*fj temp add for test */
        if(telOAMFuncSetRegistered && (sockfd>0))
        {
            pfOamClose(sockfd,TELNET_CONNECTION);
        }

        /* 结束一个TCP SESSION */
        if(pSlot->sockfd )
        {
            close(pSlot->sockfd );
            pSlot->sockfd = -1;
            pSlot->loggedIn = FALSE;
            pSlot->busyFlag=FALSE;
            telnetnumOfClients --;
        }
    }
#endif
}

void SSHInTask(T_SSH_SESSION  *pSlot)
{
    CHAR buf[512] = {'\0'};
    int ret = 0;
    int sockfd;
	
#if 1 
    /*611001834936 改为非阻塞方式读取数据*/
    while (1)
    {
        int iRecved = 0;
        /* 等待活动的远程shell连接进来 */
        /*sem_wait(&tNetActiveSockSem);*/
        XOS_SemaphoreP(pSlot->startInput, (WORD32)WAIT_FOREVER);
        sockfd=pSlot->sockfd;
        
        while(1)
        { 
            static fd_set fds; 
            int maxfdp;
            struct timeval timeout={10,0}; 

            FD_ZERO(&fds); 
            FD_SET(sockfd,&fds); 
            maxfdp=sockfd+1;
        
            iRecved = 0;
            switch(select(maxfdp,&fds,NULL,NULL,&timeout))
            { 
            case -1: 
                //exit(-1);
                break;
            case 0:
                break;
            default: 
                if(FD_ISSET(sockfd,&fds)) 
                { 
                    iRecved = 1;
                    ret = recv(sockfd,buf,sizeof(buf),MSG_DONTWAIT);
                    /*call oam function*/
                    if(SSHOAMFuncSetRegistered && (ret > 0))
                    {
                        pfOamReceive(sockfd,CONNECTION_FROM_SSH,buf, ret);
                    }
                    memset(buf,'\0',sizeof(buf));
                }
                break;
           }/*end switch*/

           if((strcmp("exit",buf) == 0) || 
             (strcmp("quit",buf) == 0) || 
             ((iRecved == 1) && (ret <= 0))/*soket 已断开*/
             )
           {
              memset(buf,'\0',sizeof(buf));
              break;
           }
         } /*end while*/

         /*R_Printf("The telnet client has disconnected!\n");*/
         /*fj temp add for test */
         if(SSHOAMFuncSetRegistered && (sockfd>0))
         {
             pfOamClose(sockfd,CONNECTION_FROM_SSH);
         }
        
         /* 结束一个TCP SESSION */
         if(pSlot->sockfd )
         {
             close(pSlot->sockfd );
             pSlot->sockfd = -1;
             pSlot->loggedIn = FALSE;
             pSlot->busyFlag=FALSE;
             SSHnumOfClients --;
         }
    }

#else   

    while(1)
    {
        /* 等待活动的远程shell连接进来 */
        /*sem_wait(&tNetActiveSockSem);*/
        XOS_SemaphoreP(pSlot->startInput, (WORD32)WAIT_FOREVER);
        sockfd=pSlot->sockfd;
        /*while ((ret = read (sockfd, buf, sizeof (buf))) > 0)*/
        while ((ret = recv(sockfd, buf, sizeof (buf), 0)) > 0)
        {


            if((strcmp("exit",buf) == 0) || (strcmp("quit",buf) == 0))
            {
                memset(buf,'\0',sizeof(buf));
                break;
            }

            /* n = write(s_pm,buf,n);*/
            /*call oam function*/
            if(SSHOAMFuncSetRegistered)
            {
                pfOamReceive(sockfd,CONNECTION_FROM_SSH,buf, ret);
            }
            memset(buf,'\0',sizeof(buf));
        };


        /*R_Printf("The telnet client has disconnected!\n");*/
        /*fj temp add for test */
        if(SSHOAMFuncSetRegistered&& (sockfd>0))
        {

            pfOamClose(sockfd,CONNECTION_FROM_SSH);
        }

        /* 结束一个TCP SESSION */
        if(pSlot->sockfd )
        {
            close(pSlot->sockfd );
        pSlot->sockfd = -1;
        pSlot->loggedIn = FALSE;
        pSlot->busyFlag=FALSE;
        SSHnumOfClients --;
        }
    }
#endif
}

SWORD32 OSS_RegOAMFuncSet(T_OAM_FUNSET *ptOamFuncSet)
{
    pfOamConnect=ptOamFuncSet->oamConnect;
    pfOamReceive=ptOamFuncSet->oamReceive;
    pfOamClose=ptOamFuncSet->oamClose;

    if(pfOamConnect && pfOamReceive && pfOamClose)
    {
        telOAMFuncSetRegistered = TRUE;
        SSHOAMFuncSetRegistered = TRUE;
    }
    else
    {
        telOAMFuncSetRegistered = FALSE;
        SSHOAMFuncSetRegistered = FALSE;
        return OAM_REGCALLBACK_ERROR_ILLEGAL;
    }
    return OAM_REGCALLBACK_SUCCESS;
}

SWORD32 OAM_FD_Send(WORD16 fd, WORD16 flag, LPVOID buf, WORD16 len)
{
    int sdwCharCount =1;

    if (flag == CONNECTION_FROM_SERIAL)
    {
//        XOS_ConsolePrint((CHAR *)buf);
        sdwCharCount = len;
        return sdwCharCount;
    }
    else
    {
        /*sdwCharCount = write(fd,buf,len);*/
        /*611001541200 12版本OAMMGR进程报段错误异常*/
        static fd_set fdset; 
        struct timeval timeout={1,0}; /*select等待1秒*/
        int maxfdp;
        int iRet;

        /*如果显示缓存不为空，数据先写入显示缓存*/
        sty *pSty = FindStyByFd(fd);
        if (pSty && pSty->tSocket)
        {
            if (pSty->ptSendBufTail != NULL)
            {
                /*写入缓存，启动发送查询定时器*/
                BOOLEAN bSaveRet = SaveDataToBuf(pSty, buf, len);
                if (!bSaveRet)
                {
                    XOS_SysLog(OAM_CFG_LOG_ERROR, "OAM_FD_Send: save data to send buffer failed! Can not display all data received!\n");
                }
    		
                return ERROR;
            }
        }

        FD_ZERO(&fdset); /*每次循环都要清空集合，否则不能检测描述符变化 */
        FD_SET(fd, &fdset); /*添加描述符 */
        maxfdp = fd + 1; /*描述符最大值加1 */

        iRet = select(maxfdp, NULL, &fdset, NULL, &timeout);

        if((iRet > 0) && FD_ISSET(fd, &fdset)) /*测试文件是否可写 */
        {
            sdwCharCount = send(fd,buf,len,MSG_DONTWAIT);
            if  (ERROR ==sdwCharCount)
                return ERROR;
            else
                return sdwCharCount;
        }
        else
        {
            /*写入缓存，启动发送查询定时器*/
            BOOLEAN bSaveRet = SaveDataToBuf(pSty, buf, len);
            if (!bSaveRet)
            {
                XOS_SysLog(OAM_CFG_LOG_ERROR, "OAM_FD_Send: save data to send buffer failed! Can not display all data received!\n");
            }
		
            return ERROR;
        }
    }
}


/**********************************************************************
* 函数名称：R_OAM_FD_Quit
* 功能描述：退出OAM Shell，恢复OSS Shell
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2007/7/17    V1.0    张文剑      创建
************************************************************************/
void OAM_FD_Quit(WORD16 fd, WORD16 flag)
{
    XOS_SysLog(OAM_CFG_LOG_NOTICE, "611000895377:OAM_FD_Quit called\n");
    flag=flag;
    if(CONNECTION_FROM_TELNET == flag)
    {
        tNetSessionDisconnect(fd,FALSE);
    }
    else if(CONNECTION_FROM_SSH == flag)
    {
        SSHSessionDisconnect(fd,FALSE);
    }
    XOS_SysLog(OAM_CFG_LOG_NOTICE, "611000895377:OAM_FD_Quit end\n");
}


BOOL getOAMCallbackRegStatus(void)
{
    return telOAMFuncSetRegistered;
}


/**********************************************************************
* 函数名称：SaveDataToBuf
* 功能描述：保存未能马上输出的数据到缓存
* 访问的表：无
* 修改的表：无
* 输入参数：pSty -输出用的sty
                              LPVOID buf -要输出的数据地址
                              WORD16 len -要输出数据长度
* 输出参数：true - 保存成功
                              false - 保存失败
* 返 回 值：无
* 其它说明：无
* 修改日期      版本号  修改人      修改内容
* ---------------------------------------------------------------------
* 2010/8/25    V1.0   殷浩      创建
************************************************************************/
static BOOLEAN SaveDataToBuf(sty *pSty, LPVOID buf, WORD16 len)
{
    T_StySendBuf *pBuf = NULL;

    if (!buf || !pSty)
    {
        return FALSE;
    }
	
    pBuf = (T_StySendBuf *)XOS_GetUB(sizeof(T_StySendBuf));
    if (!pBuf)
    {
        return FALSE;
    }

    memset(pBuf, 0, sizeof(T_StySendBuf));
    pBuf->buf = XOS_GetUB(len);
    if (!pBuf->buf)
    {
        XOS_RetUB((BYTE *)pBuf);
        return FALSE;
    }
	
    memcpy(pBuf->buf, buf, len);
    pBuf->len = len;

    if (pSty->ptSendBufHead == NULL)
    {
        pSty->ptSendBufHead = pBuf;
        pSty->ptSendBufTail = pSty->ptSendBufHead;
    }
    else
    {
        if (pSty->ptSendBufTail == NULL)
        {
            pSty->ptSendBufTail = pSty->ptSendBufHead;
        }

        pSty->ptSendBufTail->pNext = pBuf;
        pSty->ptSendBufTail = pBuf;
    }
	
    if (INVALID_TIMER_ID !=  g_tTimer_CheckSendBuf)
    {
        XOS_KillTimerByTimerId( g_tTimer_CheckSendBuf);
        g_tTimer_CheckSendBuf = INVALID_TIMER_ID;
    }
    g_tTimer_CheckSendBuf = XOS_SetRelativeTimer(PROCTYPE_OAM_TELNETSERVER, TIMER_CHECK_SENDBUF, DURATION_CHECK_SENDBUF, 1);
 
    return TRUE;
}
