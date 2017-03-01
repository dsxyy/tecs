/*
   DESCRIPTION
   This library implements the server side of the File Transfer Protocol (FTP),
   which provides remote access to the file systems available on a target.
   The protocol is defined in RFC 959. This implementation supports all commands
   required by that specification, as well as several additional commands.

   USER intERFACE
   During system startup, the ftpdInit() routine creates a control connection
   at the predefined FTP server port which is monitored by the primary FTP
   task. Each FTP session established is handled by a secondary server task
   created as necessary. The server accepts the following commands:

   .TS
   tab(|);
   l1 l.
   HELP | - List supported commands.
   USER | - Verify user name.
   PASS | - Verify password for the user.
   QUIT | - Quit the session.
   LIST | - List out contents of a directory.
   NLST | - List directory contents using a concise format.
   RETR | - Retrieve a file.
   STOR | - Store a file.
   CWD | - Change working directory.
   CDUP | - goto up level directory
   TYPE | - Change the data representation type.
   PORT | - Change the port number.
   PWD | - Get the name of current working directory.
   STRU | - Change file structure settings.
   MODE | - Change file transfer mode.
   ALLO | - Reserver sufficient storage.
   ACCT | - Identify the user's account.
   PASV | - Make the server listen on a port for data connection.
   NOOP | - Do nothing.
   DELE | - Delete a file
   SIZE | - show a file size

   .TE

   The ftpdDelete() routine will disable the FTP server until restarted.
   It reclaims all system resources used by the server tasks and cleanly
   terminates all active sessions.

   To use this feature, include the following component:
   INCLUDE_FTP_SERVER

   intERNAL
   The ftpdInit() routine spawns the primary server task ('ftpdTask') to handle
   multiple FTP sessions. That task creates a separate task ('ftpdWorkTask') for
   each active control connection.

   The diagram below defines the structure chart of ftpdLib.
   .CS

   ftpdDelete                                   ftpdInit
   |  \                                   |
   |   \                                          |
   |    \                                       ftpdTask
   |     \                                      /    |  \____________
   |      \                            /     |               \
   |    |                 ftpdSessionAdd ftpdWorkTask ftpdSessionDelete
   |    |             ______________________/     |  \
   |    |            /    /     |                 |   \
   ftpdSlotDelete | ftpdDirListGet /  ftpdDataStreamReceive |   ftpdDataStreamSend
   |    |               /       |       \         |   /      /
   \    |    __________/        |        \        |  /      /
   \    |   /                   |         ftpdDataConnGet  /
   \    |   |                   |          |   ___________/
   \    |   |                   |          |  /
   ftpdSockFree                 ftpdDataCmdSend
   .CE

INCLUDE FILES: ftpdLib.h

SEE ALSO:
ftpLib, netDrv,
    .I "RFC-959 File Transfer Protocol"
    )
    }
*/
#include <sys/poll.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <pwd.h>
#include <boost/algorithm/string.hpp>
#include "tecs_pub.h"
#include "tecs_config.h"
#include "log_agent.h"
#include "authmanager.h"
#include "ftpd.h"
#include "image.h"
#include "image_pool.h"
#include "image_store.h"
#include "nfs.h"
#include "license_common.h"
#include "api_pub.h"


using namespace zte_tecs;
typedef void (*TaskEntry)(void *);
#define FILE_CREAT_MODE         0666
#define FTP_TASK_OPTIONS        0
const char *runtimeVersion      = "Image Service";

#define FTPD_BINARY_TYPE        0x1
#define FTPD_ASCII_TYPE         0x2
#define FTPD_EBCDIC_TYPE        0x4
#define FTPD_LOCAL_BYTE_TYPE    0x8

/* Transfer mode */
#define FTPD_STREAM_MODE        0x10
#define FTPD_BLOCK_MODE         0x20
#define FTPD_COMPRESSED_MODE    0x40

/* File structure */
#define FTPD_NO_RECORD_STRU     0x100
#define FTPD_RECORD_STRU        0x200
#define FTPD_PAGE_STRU          0x400

/* Session Status */
#define FTPD_USER_OK            0x1000
#define FTPD_PASSIVE            0x2000

/* Macros to obtain correct parts of the status code */
#define FTPD_REPRESENTATION(slot)       ( (slot)->status        & 0xff)
#define FTPD_TRANS_MODE(slot)           (((slot)->status >> 8)  & 0xff)
#define FTPD_FILE_STRUCTURE(slot)       (((slot)->status >> 16) & 0xff)
#define FTPD_STATUS(slot)               (((slot)->status >> 24) & 0xff)

/* Well known port definitions -- someday we'll have getservbyname */
static string s_ftp_server_ip("*");
static int s_ftp_data_port = 20;
static int s_ftp_daemon_port = 21;

/* Free socket indicative */
/* Arbitrary limits for the size of the FTPD work task name */

#define FTPD_WORK_TASK_NAME_LEN 40

/* Arbitrary limits hinted by Unix FTPD in waing for a new data connection */
/* Macro to get the byte out of an int */

#define FTPD_UC(ch)             (((int) (ch)) & 0xff)

/* Bit set in FTP reply code to indicate multi-line reply.
 * Used internally by ftpdCmdSend() where codes are less than
 * 1024 but are 32-bit integers.  [Admittedly a hack, see
 * ftpdCmdSend().]
 */

#define FTPD_MULTI_LINE         0x10000000
#define CLOSE_DELAY_TIME        3                   /*  �ӳٹر�ʱ�� */
#define MAX_FTP_USERNAME_LEN   ((BYTE)32)  /*�û�������*/
#define MAX_FTP_PWD_LEN        ((BYTE)254)  /*�û����볤��*/
#define SEND_STR(sd,str)        send(sd,str,strlen(str),0)
#define FTP_MAX_CLIENTS         200                 /* ���֧��ͬʱ���ӵ�ftp�ͻ����� */
#define FTP_MIN_CLIENTS           5                 /* ���֧��ͬʱ���ӵ�ftp�ͻ����� */
#define FTP_DATA_BUF_SIZE       (12*1024)           /* �շ�ftp����ʱ��������С */
#define FTP_TSKSTACK_SIZE       (8*1024)            /* ftp���������ջ��С */
#define FTP_WORKTSKSTACK_SIZE   (12*1024+FTP_TSKSTACK_SIZE+FTP_DATA_BUF_SIZE)           /* ftp���������ջ��С */
#define FTPD_WINDOW_SIZE        10240

/*
 * The FTP server keeps track of active client sessions in a linked list
 * of the following FTPD_SESSION_DATA data structures. That structure
 * contains all the variables which must be maintained separately
 * for each client so that the code shared by every secondary
 * task will function correctly.
 */

#define FTPD_DATA_TCPCB_NUM             5
#ifndef BUFSIZE
#define BUFSIZE 4096
#endif

#define DEF_READ_TIMEO                  (120)   /*  �����ݳ�ʱʱ��Ϊ2����   */
#define DEF_CMD_READ_TIMEO              (300)   /*  �û�5���Ӳ��Ϊ��ʱ   */
#define DEF_ACCEPT_TIMEOUT              (120)   /*  �ȴ��Է����ӳ�ʱʱ��Ϊ2���� */
#define ERROR_NORESP                     -2     /*  ���󣬲���δ���ش�����  */

#define INET_ADDR_LEN   32
#define SPD_CTRL_TIME   100

typedef int SOCK_STATUS;                     /**< socket�������״̬ */
#define SOCKET_ERROR ((SOCK_STATUS)-1)       /**< �׽��ִ��� */
#define SOCKET_SUCCESS 0                     /**< �׽��ֲ����ɹ� */

static bool MutexInit(pthread_mutex_t *pMutex);
static bool MutexLock(pthread_mutex_t *pMutex);
static bool MutexUnLock(pthread_mutex_t *pMutex);
static void MutexClose(pthread_mutex_t *pMutex);

/*
 *  lizl    ��linux�ں��и��ƹ���
 */

/*
 * Simple doubly linked list implementation.
 *
 * Some of the internal functions ("__xxx") are useful when
 * manipulating whole lists rather than single entries, as
 * sometimes we already know the next/prev entries and we can
 * generate better code by using them directly rather than
 * using the generic single-entry routines.
 */

struct oss_list_head {
        struct oss_list_head *next, *prev;
};

#define XOS_LIST_HEAD_INIT(name) { &(name), &(name) }
#define XOS_LIST_HEAD(name)    \
 struct oss_list_head name = XOS_LIST_HEAD_INIT(name)

static __inline__ void XOS_INIT_LIST_HEAD(struct oss_list_head *list)
{
        list->next = list;
        list->prev = list;
}

/*
 * Insert a new entry between two known consecutive entries.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static __inline__ void __oss_list_add(struct oss_list_head * newi,
        struct oss_list_head * prev,
        struct oss_list_head * next)
{
        next->prev = newi;
        newi->next = next;
        newi->prev = prev;
        prev->next = newi;
}

/**
 * list_add - add a new entry
 * @new: new entry to be added
 * @head: list head to add it after
 *
 * Insert a new entry after the specified head.
 * This is good for implementing stacks.
 */
static __inline__ void oss_list_add(struct oss_list_head *newi, struct oss_list_head *head)
{
        __oss_list_add(newi, head, head->next);
}

/**
 * list_add_tail - add a new entry
 * @new: new entry to be added
 * @head: list head to add it before
 *
 * Insert a new entry before the specified head.
 * This is useful for implementing queues.
 */
static __inline__ void oss_list_add_tail(struct oss_list_head *newi, struct oss_list_head *head)
{
        __oss_list_add(newi, head->prev, head);
}

/*
 * Delete a list entry by making the prev/next entries
 * point to each other.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static __inline__ void __oss_list_del(struct oss_list_head * prev,
                                  struct oss_list_head * next)
{
        next->prev = prev;
        prev->next = next;
}

/**
 * list_del - deletes entry from list.
 * @entry: the element to delete from the list.
 * Note: list_empty on entry does not return true after this, the entry is in an undefined state.
 */
static __inline__ void oss_list_del(struct oss_list_head *entry)
{
        __oss_list_del(entry->prev, entry->next);
        entry->next = entry->prev = 0;
}

/**
 * list_del_init - deletes entry from list and reinitialize it.
 * @entry: the element to delete from the list.
 */
static __inline__ void oss_list_del_init(struct oss_list_head *entry)
{
        __oss_list_del(entry->prev, entry->next);
        XOS_INIT_LIST_HEAD(entry);
}

/**
 * list_empty - tests whether a list is empty
 * @head: the list to test.
 */
static __inline__ int oss_list_empty(struct oss_list_head *head)
{
        return head->next == head;
}

/**
 * list_splice - join two lists
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 */
static __inline__ void oss_list_splice(struct oss_list_head *list, struct oss_list_head *head)
{
        struct oss_list_head *first = list->next;

        if (first != list) {
                struct oss_list_head *last = list->prev;
                struct oss_list_head *at = head->next;

                first->prev = head;
                head->next = first;

                last->next = at;
                at->prev = last;
        }
}

/**
 * list_entry - get the struct for this entry
 * @ptr:        the &struct oss_list_head pointer.
 * @type:       the type of the struct this is embedded in.
 * @member:     the name of the list_struct within the struct.
 */
#define oss_list_entry(ptr, type, member) \
        ((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member)))

/**
 * list_for_each        -       iterate over a list
 * @pos:        the &struct oss_list_head to use as a loop counter.
 * @head:       the head for your list.
 */
#define oss_list_for_each(pos, head) \
        for (pos = (head)->next; pos != (head); \
                pos = pos->next)

/**
 * list_for_each_safe   -       iterate over a list safe against removal of list entry
 * @pos:        the &struct oss_list_head to use as a loop counter.
 * @n:          another &struct oss_list_head to use as temporary storage
 * @head:       the head for your list.
 */
#define oss_list_for_each_safe(pos, n, head) \
        for (pos = (head)->next, n = pos->next; pos != (head); \
                pos = n, n = pos->next)

/**
 * list_for_each_entry  -       iterate over list of given type
 * @pos:        the type * to use as a loop cursor.
 * @head:       the head for your list.
 * @member:     the name of the list_struct within the struct.
 */
#define oss_list_for_each_entry(pos, head, member)                          \
        for (pos = oss_list_entry((head)->next, typeof(*pos), member);      \
             &pos->member != (head);        \
             pos = oss_list_entry(pos->member.next, typeof(*pos), member))

/**
 * list_for_each_entry_reverse - iterate backwards over list of given type.
 * @pos:        the type * to use as a loop cursor.
 * @head:       the head for your list.
 * @member:     the name of the list_struct within the struct.
 */
#define oss_list_for_each_entry_reverse(pos, head, member)                  \
        for (pos = oss_list_entry((head)->prev, typeof(*pos), member);      \
             &pos->member != (head);        \
             pos = oss_list_entry(pos->member.prev, typeof(*pos), member))

/**

 * list_for_each_entry_safe - iterate over list of given type safe against removal of list entry
 * @pos:        the type * to use as a loop cursor.
 * @n:          another type * to use as temporary storage
 * @head:       the head for your list.
 * @member:     the name of the list_struct within the struct.
 */
#define oss_list_for_each_entry_safe(pos, n, head, member)                  \
        for (pos = oss_list_entry((head)->next, typeof(*pos), member),      \
                n = oss_list_entry(pos->member.next, typeof(*pos), member); \
             &pos->member != (head);                                    \
             pos = n, n = oss_list_entry(n->member.next, typeof(*n), member))

static int ftpd_print_on = 0;
DEFINE_DEBUG_VAR(ftpd_print_on);
#define Debug(level,fmt,arg...) \
        do \
        { \
            if(ftpd_print_on) \
            { \
                printf("[%s:%d]"fmt,__func__,__LINE__, ##arg); \
            } \
            OutPut(level,fmt,##arg);\
        }while(0)

#define MAC_ADDR_LEN            6       /*  MAC��ַ����,����DHCP������     */
#define OSSSVR_FILEPATH_SIZE        256


/**
 * @����: FTP����������֤�ص�����
 *
 * @����:   uname   [in]    �û���
 *          pass    [in]    ����
 *          strHome [out]   �û��ĸ�Ŀ¼
 *          iHomeStrSize    [in]    �û���Ŀ¼�������Ĵ�С
 *          pbWritable      [out]   �û��ǲ�����дȨ�ޣ�дȨ��Ҳ��ɾ��Ȩ��
 *          pbOverWrite     [out]   �û��ǲ��ǿ��Ը���д
 *          pdwFlowLimit    [out]   �û������ش�С,��λΪbytes/ms,0��ʾ������
 *
 * @����:   true:   ��֤�ɹ�����ʱ�����[out]���Բ�������ȷ����
 *          false:  ��֤ʧ�ܣ���ʱ�����[out]���Բ���ֵ������
 */
typedef int (*FTPD_AUTH_FUNCPTR)(
        const char* uname,const char* pass,
        char *strHome,int iHomeStrSize,
        int *pbWritable, int *pbOverWrite,SWORD16 *pdwFlowLimit);

typedef struct
{
    struct oss_list_head    node;           /* for link-listing */
    int                 status;         /* see various status bits above */
    int                 byteCount;      /* bytes transferred */
    int                 cmdSock;        /* command socket */
    STATUS              cmdSockError;   /* Set to ERROR on write error */
    int                 dataSock;       /* data socket */
    struct sockaddr_in  peerAddr;       /* address of control connection */
    struct sockaddr_in  dataAddr;       /* address of data connection */
    char                buf[BUFSIZE];   /* multi-purpose buffer per session */
    char                rootDirName[OSSSVR_FILEPATH_SIZE];  /* �û���Ŀ¼       */
    char                curDirName [OSSSVR_FILEPATH_SIZE];  /* active directory */
    /*char               user [MAX_LOGIN_NAME_LEN+1];  current user */
    char                username [MAX_FTP_USERNAME_LEN+1]; /* current user */
    char                password[MAX_FTP_PWD_LEN+1];   /*���û������룬�û���֤�����*/
    int64               uid;
    int                 curDataSockType;
    int                 bWritable;          /*  �ǲ��ǿ�д,����д�Ͳ���ɾ��  */
    int                 bOverWritable;      /*  �ǲ��ǿɸ���  */
    SWORD16             iStreamThrashold;   /*����������ֵ kb/s���û���֤�����*/
}FTPD_SESSION_DATA;

/*
 *  ftp������Ϣ
 */
typedef struct tagTCmdInfo
{
    const char  *strToken;                   /*  ����ؼ���  */
    int     (*pFunc)(FTPD_SESSION_DATA *pSlot,const char *strParam);   /* ������� */
    WORD16      wTokenLen;                   /*  �ؼ��ֳ���  */
    WORD16      wNeedAuth;                   /*  ��Ȩָ���־,
                                                  *   0: ����Ȩָ�����֤Ҳ����ִ��
                                                  *   1: ��Ȩָ��,��Ҫ��֤�ſ���ִ��
                                                  */
}T_CmdInfo;


/* ʵ�ֵ�ftp���� */
static int Do_User(FTPD_SESSION_DATA   *pSlot,const char *strParam);
static int Do_Pass(FTPD_SESSION_DATA   *pSlot,const char *strParam);
static int Do_Quit(FTPD_SESSION_DATA   *pSlot,const char *strParam);
static int Do_Help(FTPD_SESSION_DATA   *pSlot,const char *strParam);
static int Do_Noop(FTPD_SESSION_DATA   *pSlot,const char *strParam);
static int Do_Cwd(FTPD_SESSION_DATA   *pSlot,const char *strParam);
static int Do_Cdup(FTPD_SESSION_DATA   *pSlot,const char *strParam);
static int Do_Pwd(FTPD_SESSION_DATA   *pSlot,const char *strParam);
static int Do_List(FTPD_SESSION_DATA   *pSlot,const char *strParam);
static int Do_NLst(FTPD_SESSION_DATA   *pSlot,const char *strParam);
static int Do_Size(FTPD_SESSION_DATA   *pSlot,const char *strParam);
static int Do_RetR(FTPD_SESSION_DATA   *pSlot,const char *strParam);
static int Do_StoR(FTPD_SESSION_DATA   *pSlot,const char *strParam);
static int Do_Dele(FTPD_SESSION_DATA   *pSlot,const char *strParam);
static int Do_Port(FTPD_SESSION_DATA   *pSlot,const char *strParam);
static int Do_Pasv(FTPD_SESSION_DATA   *pSlot,const char *strParam);
static int Do_Type(FTPD_SESSION_DATA   *pSlot,const char *strParam);
static int Do_Mode(FTPD_SESSION_DATA   *pSlot,const char *strParam);
static int Do_StrU(FTPD_SESSION_DATA   *pSlot,const char *strParam);
static int Do_Allo(FTPD_SESSION_DATA   *pSlot,const char *strParam);


/*
 *  FTP������г�����֧�ֵ�ftp����
 */
static T_CmdInfo   s_atFtpCmds[] =
    {
        /*ftp������     ������        ����������      �ǲ�����Ȩָ��  */
        {"USER",        Do_User,        4,              0},
        {"PASS",        Do_Pass,        4,              0},
        {"QUIT",        Do_Quit,        4,              0},
        {"HELP",        Do_Help,        4,              0},
        {"NOOP",        Do_Noop,        4,              1},
        {"CWD",         Do_Cwd,         3,              1},
        {"CDUP",        Do_Cdup,        4,              1},
        {"PWD",         Do_Pwd,         3,              1},
        {"LIST",        Do_List,        4,              1},
        {"NLST",        Do_NLst,        4,              1},
        {"SIZE",        Do_Size,        4,              1},
        {"RETR",        Do_RetR,        4,              1},
        {"STOR",        Do_StoR,        4,              1},
        {"DELE",        Do_Dele,        4,              1},
        {"PORT",        Do_Port,        4,              1},
        {"PASV",        Do_Pasv,        4,              1},
        {"TYPE",        Do_Type,        4,              1},
        {"MODE",        Do_Mode,        4,              1},
        {"STRU",        Do_StrU,        4,              1},
        {"ALLO",        Do_Allo,        4,              1},
        {"ACCT",        Do_Allo,        4,              1},
        {NULL,          NULL,           0,              0}
    };



/* Various s_astrMessages to be told to the clients */
/*3GCHG00037052,�޸�����ASCII���͵�Ӧ����Ϣ��yindesheng*/
static const char *s_astrMessages [] =
    {
        "Can't open passive connection",
        "Parameter not accepted",
        "Data connection error",
        "Directory non existent or syntax error",
        "Local resource failure: %s",
        "Tecs (%s) FTP server ready",
        "Password required",
        "User logged in",
        "Bye...see you later",
        "USER and PASS required",
        "No files found or invalid directory or permission problem",
        "Transfer complete",
        "File \"%s\" not found or permission problem",
        "Cannot create file \"%s\" or permission problem",
        "Changed directory to \"%s\"",
        "Type set to I, binary mode",
        "Don't support ASCII mode,please select BINARY mode",
        "Port set okay",
        "Current directory is \"%s\"",
        "File structure set to NO RECORD",
        "Stream mode okay",
        "Allocate and Account not required",
        "Entering Passive Mode (%d,%d,%d,%d,%d,%d)",
        "NOOP -- did nothing as requested...hah!",
        "Command not recognized",
        "Error in input file",
        "Unimplemented TYPE %d",
        "You could at least say goodbye.",
        "The following commands are recognized:",
        "End of command list.",
        "File deleted successfully.",
        "Login failed, please input \"user\" command to try again",
        "Modify file failed, \"%s\" Permission deny",
        "Can't cover image with the same name.",
    };

/* Indexes to the s_astrMessages [] array */
#define MSG_PASSIVE_ERROR       0
#define MSG_PARAM_BAD           1
#define MSG_DATA_CONN_ERROR     2
#define MSG_DIR_NOT_PRESENT     3
#define MSG_LOCAL_RESOURCE_FAIL 4
#define MSG_SERVER_READY        5
#define MSG_PASSWORD_REQUIRED   6
#define MSG_USER_LOGGED_IN      7
#define MSG_SEE_YOU_LATER       8
#define MSG_USER_PASS_REQ       9
#define MSG_DIR_ERROR           10
#define MSG_TRANS_COMPLETE      11
#define MSG_FILE_ERROR          12
#define MSG_CREATE_ERROR        13
#define MSG_CHANGED_DIR         14
#define MSG_TYPE_BINARY         15
#define MSG_TYPE_ASCII          16
#define MSG_PORT_SET            17
#define MSG_CUR_DIR             18
#define MSG_FILE_STRU           19
#define MSG_STREAM_MODE         20
#define MSG_ALLOC_ACCOUNT       21
#define MSG_PASSIVE_MODE        22
#define MSG_NOOP_OKAY           23
#define MSG_BAD_COMMAND         24
#define MSG_INPUT_FILE_ERROR    25
#define MSG_TYPE_ERROR          26
#define MSG_NO_GOOD_BYE         27
#define MSG_COMMAND_LIST_BEGIN  28
#define MSG_COMMAND_LIST_END    29
#define MSG_DELE_OKAY           30
#define MSG_USER_LOGIN_FAILED   31
#define MSG_PERMISSION_DENY     32
#define MSG_COVER_DENY          33

static const char *s_strFtpdCmdList =
    "HELP   USER    PASS    QUIT    LIST    NLST\n\
    RETR    STOR    CWD     TYPE    PORT    PWD\n\
    STRU    MODE    ALLO    ACCT    PASV    NOOP\n\
    DELE    SIZE\n";


static volatile int s_bFtpsActive = false;/* Server started? */
static volatile int s_iFtpsCurClients;             /*  �Ѿ����е�ftp����������Ŀ */
static int s_bFtpsShutdownFlag;    /*  �˳�״̬����        */
static int s_hftpdServSd = SOCKET_ERROR;         /*  �����˿� */
static struct oss_list_head s_atFtpSessions;    /*  ftp�Ự�б� */
static pthread_mutex_t s_hFtpSessMutex;     /*  ������,������s_atFtpSessions�Ĳ��� */
//static int s_iFtpWorkTskPrio = 20;    /*  �����������ȼ�  */
static int s_iFtpMaxClients = 5;
static string s_strDefRootPath;
static ThreadPool *s_md5SumTaskPool = NULL;
static FTPD_SESSION_DATA *ftpdSessionAdd (void);
static void ftpdSessionDelete (FTPD_SESSION_DATA *);
static void ftpdWorkTask (FTPD_SESSION_DATA *);
static STATUS ftpdCmdSend (FTPD_SESSION_DATA *, int, int, const char *,
                           SWORDPTR, SWORDPTR, SWORDPTR, SWORDPTR, SWORDPTR, SWORDPTR);
static STATUS ftpdDataConnGet (FTPD_SESSION_DATA *);
static void ftpdDataStreamSend (FTPD_SESSION_DATA *, FILE *, char * pFileName);
static STATUS ftpdDataStreamReceive (FTPD_SESSION_DATA *, FILE *outStream);
static void ftpdSockFree (int *);
static STATUS ftpdDirListGet ( FTPD_SESSION_DATA   *,int, const char *,int);
static void unImplementedType (FTPD_SESSION_DATA *pSlot);
static void dataError (FTPD_SESSION_DATA *pSlot,int need_resp);
static void fileError (FTPD_SESSION_DATA *pSlot);
static void transferOkay (FTPD_SESSION_DATA *pSlot,const char* prompt = NULL);
static void ftpdDataSockFree (FTPD_SESSION_DATA *,int *);
static STATUS FtpLogin(FTPD_SESSION_DATA *pSlot);
static int transferByCtrl(FTPD_SESSION_DATA *pSlot,int inFd,char* pInFileName, int outFd,char *pBuf,int bufSize);
extern void Md5Encrypt(const char *in, char *out);
void addDefaultFtpUserInfo(void);
static void MkPath(const char *strParent,const char *strSub,char *strOut,unsigned int iOutSize);
static int ChDir(char *strCurDir,const char *strDirName,int iCurDirSize);
static int ConnWithTimeout(int Sd,const struct sockaddr *Addr,struct timeval *tv);
static STATUS RegisterImage(int64 uid,const string& file,ostringstream& result);
static STATUS DeregisterImage(const string& file);
bool CheckListPermission(FTPD_SESSION_DATA *pSlot,const string& list_dir);

multimap< int64,string >           ftpFileRegister;
pthread_mutex_t                    ftpFileRegisterMutex;     /*  ������,������ftpFileRegister�Ĳ��� */

bool HasSameFileInRegister(int64 uid,const string &filename);
bool InsertFile2Register(int64 uid,const string &filename);
bool DelFileFromRegister(int64 uid,const string &filename);

map< int,bool >                    ftpDataPortFixedMap;
pthread_mutex_t                    ftpDataPortFixedMutex;

static int s_ftp_data_port_begin = 9001;
static int s_ftp_data_port_end = 9010;

void FtpDataPortFixedInit();
int FtpDataPortFixedGet();
bool FtpDataPortFixedFree(int port);

class Md5sumTask:public Worktodo
{
public:
    Md5sumTask(const string& name,const string& file):
            Worktodo(name),_file(file)
    {
        _work_thread_id = INVALID_THREAD_ID;
    };

    ~Md5sumTask()
    {
    };

    STATUS virtual Execute()
    {
        _work_thread_id = pthread_self();
        _begin_at.refresh();

        if (_file.empty())
        {
            SkyAssert(false);
            return ERROR_INVALID_ARGUMENT;
        }

        ImagePool *ipool = ImagePool::GetInstance();
        int64 iid = ipool->GetImageIdByLocation(_file);
        if((iid == INVALID_OID)||(0 == iid))
        {
            return ERROR_OBJECT_NOT_EXIST;
        }

        string checksum;
        Debug(SYS_DEBUG,"calculating md5sum of file %s ...\n",_file.c_str());
        #if 0
        STATUS ret = GetFileMd5sum(_file,checksum);
        #endif
        Delay(5000);
        STATUS ret = sys_md5(_file,checksum);
        Debug(SYS_DEBUG,"md5sum of file %s is: %s\n",_file.c_str(),checksum.c_str());


        Image image;
        int result = ipool->GetImageByImageId(iid,image);

        if(ERROR == result)
        {
            Debug(SYS_WARNING,"Failed to get image %lld\n",iid);
            return ERROR_OBJECT_NOT_EXIST;
        }

        image.set_file_checksum(checksum);
        ipool->Update(image);

        return ret;
    }

private:
    DISALLOW_COPY_AND_ASSIGN(Md5sumTask);
    Md5sumTask():Worktodo("null")
    {};
    THREAD_ID _work_thread_id;
    Datetime   _begin_at;
    string     _file;
};

static STATUS CalcMd5sum(const char* strFilePath)
{
    //Debug(SYS_DEBUG,"%s\n",oss.str().c_str());
    if (!s_md5SumTaskPool || !strFilePath)
    {
        SkyAssert(false);
        return ERROR_INVALID_ARGUMENT;
    }

    //��ʼ����md5
    Md5sumTask* task = new Md5sumTask("md5sum",strFilePath);
    if (!task)
    {
        return ERROR_NO_MEMORY;
    }

    //���������뵽�̳߳ع�������
    Debug(SYS_NOTICE,"file (%s) md5sum task is started!\n",strFilePath);
    return s_md5SumTaskPool->AssignWork(task);
}

static bool MutexInit(pthread_mutex_t *pMutex)
{
    int ret = -1;
    pthread_mutexattr_t attr;

    SkyAssert(pMutex);
    if (pMutex)
    {
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_RECURSIVE);
        ret = pthread_mutex_init(pMutex,&attr);
        pthread_mutexattr_destroy(&attr);
    }
    return ret == 0? true: false;
}

static bool MutexLock(pthread_mutex_t *pMutex)
{
    if (pMutex)
    {
        if (0 != pthread_mutex_lock(pMutex))
        {
            Debug(SYS_WARNING,"MutexLock: pthread_mutex_lock failed !\n");
            return false;
        }
        return true;
    }
    return false;
}

static bool MutexUnLock(pthread_mutex_t *pMutex)
{
    if (pMutex)
    {
        pthread_mutex_unlock(pMutex);
        return true;
    }
    return false;
}

static void MutexClose(pthread_mutex_t *pMutex)
{
    if (pMutex)
    {
        pthread_mutex_destroy(pMutex);
    }
}

/*
 *  mtu���Դ�ӡ����
 */
static int s_bMTUPrn        = 0;
#define MTU_PRN(fmt,arg...) do{ if(s_bMTUPrn)   printf( fmt,##arg); }while(0)

static ssize_t ReadTO(int fd, void *buf, size_t count,int timeout)
{
    struct pollfd  fds[1];
    fds[0].fd = fd;
    fds[0].events = POLLIN | POLLPRI;
    timeout = timeout * 1000;
    if ( poll(fds,1,timeout) >0)
    {
        return read(fds[0].fd,buf,count);
    }
    else
    {
        return -2;
    }
}

static int AcceptTO(int sd,struct sockaddr *addr, socklen_t *addrLen,int timeout )
{
    struct pollfd fds[1];
    fds[0].fd = sd;
    fds[0].events = POLLIN | POLLPRI;
    timeout = timeout * 1000;
    if (poll(fds,1,timeout) > 0)
    {
        return accept(fds[0].fd,addr,addrLen);
    }
    else
    {
        return -2;
    }
}

/*******************************************************************************
 *
 * ftpdTask - FTP server daemon task
 *
 * This routine monitors the FTP control port for incoming requests from clients
 * and processes each request by spawning a secondary server task after
 * establishing the control connection. If the maximum number of connections is
 * reached, it returns the appropriate error to the requesting client. The
 * routine is the entry point for the primary FTP server task and should only
 * be called internally.
 *
 * RETURNS: N/A
 *
 * ERRNO: N/A
 *
 * intERNAL:
 * The server task is deleted by the server shutdown routine. Adding a newly
 * created client session to the list of active clients is performed atomically
 * with respect to the shutdown routine. However, accepting control connections
 * is not a critical section, since closing the initial socket used in the
 * listen() call also closes any later connections which are still open.
 *
 * NOMANUAL
 */

static void ftpdTask (void)
{
    int         newSock;
    FTPD_SESSION_DATA *pSlot;
    int         on = 1;
    socklen_t      addrLen;
    struct sockaddr_in addr;
    char        a_ip_addr [INET_ADDR_LEN];  /* ascii ip address of client */

    char        ftpdWorkTaskName [FTPD_WORK_TASK_NAME_LEN];

    /*��ʼ��ftp �ļ���ע�ụ�����*/
    MutexInit(&ftpFileRegisterMutex);
    MutexInit(&ftpDataPortFixedMutex);
    FtpDataPortFixedInit();
        /* The following loop halts if this task is deleted. */
    for (;;)
    {
        /* Wait for a new incoming connection. */

        addrLen = sizeof (struct sockaddr);

        Debug(SYS_DEBUG,"waiting for a new client connection...\n",0,0,0,0);

        newSock = accept (s_hftpdServSd, (struct sockaddr *) &addr, &addrLen);
        if (newSock < 0)
        {
            /*yindesheng CTRCR00010656*/
            Debug(SYS_ERROR,"cannot accept a new connection��errno is 0x%x\n",errno);
            if ( SOCKET_ERROR == s_hftpdServSd )
            {
                break;
            }
            continue;
        }

        /*
         * Register a new FTP client session. This process is a critical
         * section with the server shutdown routine. If an error occurs,
         * the reply must be sent over the control connection to the peer
         * before the semaphore is released. Otherwise, this task could
         * be deleted and no response would be sent, possibly causing
         * the new client to hang indefinitely.
         */

        MutexLock(&s_hFtpSessMutex );
        setsockopt (newSock, SOL_SOCKET, SO_KEEPALIVE, (char *) &on,sizeof (on));
        inet_ntop(AF_INET,&addr.sin_addr,a_ip_addr,sizeof(a_ip_addr)-1);
        a_ip_addr[sizeof(a_ip_addr)-1]  = 0;
        Debug(SYS_DEBUG,"accepted a new client connection from %s\n",
                      (WORDPTR) a_ip_addr, 0, 0, 0);

        /* Create a new session entry for this connection, if possible. */
        pSlot = ftpdSessionAdd ();
        if ( NULL == pSlot )    /* Maximum number of connections reached. */
        {
            /* Send transient failure report to client. */
            ftpdCmdSend (pSlot, newSock, 421,
                         "Session limit reached, closing control connection",
                         0, 0, 0, 0, 0, 0);
            Debug(SYS_ERROR,"cannot get a new connection slot\n");
            close (newSock);
            MutexUnLock(&s_hFtpSessMutex);
            continue;
        }

        pSlot->cmdSock  = newSock;

        /* Save the control address and assign the default data address. */
        bcopy ( (char *)&addr, (char *)&pSlot->peerAddr,sizeof (struct sockaddr_in));
        bcopy ( (char *)&addr, (char *)&pSlot->dataAddr,sizeof (struct sockaddr_in));
        pSlot->dataAddr.sin_port = htons (s_ftp_data_port);

        /* Create a task name. */
        sprintf (ftpdWorkTaskName, "tFtpdServ%d", s_iFtpsCurClients);

        /* Spawn a secondary task to process FTP requests for this session. */
        Debug(SYS_DEBUG,"creating a new server task %s...\n",(SWORDPTR) ftpdWorkTaskName, 0, 0, 0);
        if(INVALID_THREAD_ID == StartThread(ftpdWorkTaskName,(ThreadEntry)ftpdWorkTask,(void *)pSlot))
        {

            /* Send transient failure report to client. */
            ftpdCmdSend (pSlot, newSock, 421,
                         "Service not available, closing control connection",
                         0, 0, 0, 0, 0, 0);
            ftpdSessionDelete (pSlot);
            Debug(SYS_ERROR,"cannot create a new work task\n");
            MutexUnLock(&s_hFtpSessMutex);
            continue;
        }

        Debug(SYS_DEBUG,"done.\n",0,0,0,0);
        /* Session added - end of critical section with shutdown routine. */
        MutexUnLock(&s_hFtpSessMutex);
    }

    /*�ͷ�ftpFileRegisterMutex*/
    MutexClose(&ftpDataPortFixedMutex);
    MutexClose(&ftpFileRegisterMutex);
    /* Fatal error - update state of server. */
    s_bFtpsActive = false;
    return;
}

/*******************************************************************************
 *
 * ftpdSessionAdd - add a new entry to the ftpd session slot list
 *
 * Each of the incoming FTP sessions is associated with an entry in the
 * FTP server's session list which records session-specific context for each
 * control connection established by the FTP clients. This routine creates and
 * initializes a new entry in the session list, unless the needed memory is not
 * available or the upper limit for simultaneous connections is reached.
 *
 * RETURNS: A pointer to the session list entry, or NULL of none available.
 *
 * ERRNO: N/A
 *
 * NOMANUAL
 *
 * intERNAL
 * This routine executes within a critical section of the primary FTP server
 * task, so mutual exclusion is already present when adding entries to the
 * client list and updating the shared variables indicating the current number
 * of connected clients.
 */

static FTPD_SESSION_DATA *ftpdSessionAdd (void)
{
    FTPD_SESSION_DATA   *pSlot;

    if ( s_iFtpMaxClients == s_iFtpsCurClients )
        return (NULL);

    /* get memory for the new session entry */

    pSlot = (FTPD_SESSION_DATA *) malloc(sizeof (FTPD_SESSION_DATA));
    if (pSlot == NULL)
    {
        return (NULL);
    }
    bzero ((char *)pSlot, sizeof(FTPD_SESSION_DATA));

    /* initialize key fields in the newly acquired slot */

    pSlot->dataSock     = SOCKET_ERROR;
    pSlot->cmdSock      = SOCKET_ERROR;
    pSlot->cmdSockError = OK;
    /*3GCHG00037052,����ASCII���ͣ�yindesheng*/
    pSlot->status       = FTPD_STREAM_MODE | FTPD_BINARY_TYPE | FTPD_NO_RECORD_STRU;
    pSlot->byteCount    = 0;

    /*yindesheng �û���֤���*/
    pSlot->iStreamThrashold = 0;

    /* assign the default directory for this guy */
    strncpy(pSlot->rootDirName,s_strDefRootPath.c_str(),sizeof(pSlot->rootDirName)-1);
    strncpy(pSlot->curDirName,s_strDefRootPath.c_str(),sizeof(pSlot->curDirName)-1);

    /* Add new entry to the list of active sessions.*/
    /*  �����Ѿ����ϲ���������ﲻ����Ҫ��          */
    oss_list_add(&pSlot->node,&s_atFtpSessions);

    s_iFtpsCurClients++;

    return (pSlot);
}

/*******************************************************************************
 *
 * ftpdSessionDelete - remove an entry from the FTP session list
 *
 * This routine removes the session-specific context from the session list
 * after the client exits or a fatal error occurs.
 *
 * RETURNS: N/A
 *
 * ERRNO: N/A
 *
 * NOMANUAL
 *
 * intERNAL
 * Unless an error occurs, this routine is only called in response to a
 * pending server shutdown, indicated by the shutdown flag. Even if the
 * shutdown flag is not detected and this routine is called because of an
 * error, the appropriate signal will still be sent to any pending shutdown
 * routine. The shutdown routine will only return after any active client
 * sessions have exited.
 */

static void ftpdSessionDelete
(
    FTPD_SESSION_DATA *pSlot       /* pointer to the slot to be deleted */
)
{
    if (pSlot == NULL)                  /* null slot? don't do anything */
        return;

    /*
     * The deletion of a session entry must be an atomic operation to support
     * an upper limit on the number of simultaneous connections allowed.
     * This mutual exclusion also prevents a race condition with the server
     * shutdown routine. The last client session will always send an exit
     * signal to the shutdown routine, whether or not the shutdown flag was
     * detected during normal processing.
     */

    MutexLock(&s_hFtpSessMutex);

    -- s_iFtpsCurClients;

    oss_list_del(&pSlot->node);             /* ��������ɾ���ڵ� */

    ftpdSockFree (&pSlot->cmdSock);     /* release data and command sockets */
    ftpdSockFree (&pSlot->dataSock);

    free((char *)pSlot);

    /* Send required signal if all sessions are closed. */

    MutexUnLock(&s_hFtpSessMutex);

    return;
}


/**********************************************************************
 * �������ƣ�ReadAFTPCmd
 * �������������׽����ж����Ի��Ż��з��ṹһ��ftp����,��ȥ����β�Ŀո�,�������׵��ʱ�ɴ�д
 * ���ʵı���
 * �޸ĵı���
 * ���������
 *           sd��        ftp�������׽���
 *           RecvBuf:    ������յ�������
 *           MaxSize:    RecvBuf������С
 *
 * ���������pNext      ��һ�п�ʼ��ַ
 * �� �� ֵ��
 *           1          ���ɹ�
 *           0          ��ʧ��,�׽��ַ�������
 *           -1         ��ʧ�ܣ�������ַ�������
 *           -2         ����ʱ
 * ����˵����
 * �޸�����      �汾��  �޸���      �޸�����
 * ---------------------------------------------------------------------
 * 2007/08/27    V1.0    ������      ����
 ************************************************************************/
static int ReadAFTPCmd(int sd,char *strRecvBuf,int iMaxSize,char** pstrNext)
{
    char    *pcBuf              = NULL;
    int     iRecvSize           = 0;
    int     iReadSize;

    /*  ����ϴ�û�з����꣬�Ǽ��������ϴ�����
     *  һ����˵ftpֻ�н��յ���Ӧ���ٷ���һ��������������������ֵĿ����Բ���
     */
    if ( NULL != *pstrNext )
    {
        iRecvSize               = strlen(*pstrNext);
        memmove(strRecvBuf,*pstrNext,iRecvSize);
        strRecvBuf[iRecvSize]   = 0;
        *pstrNext               = NULL;
    }

    /*
     *  ÿ��ftp�����\r\n����,ftp����ĸ�ʽ����:
     *  Cmd[ param]\r\n
     *  ����Ͳ�������һ���ո��
     */
    while ( iRecvSize < (iMaxSize - 1) )
    {
        /*
         *  ����Ѿ��ҵ�һ������
         */
        pcBuf = strstr(strRecvBuf,"\r\n");
        if ( NULL != pcBuf )
        {
            break;
        }

        /*
         *  ���û�У���������
         */
        iReadSize = ReadTO (sd, strRecvBuf + iRecvSize, iMaxSize - iRecvSize-1,DEF_CMD_READ_TIMEO );

        if ( iReadSize  <= 0)
        {
            /*
             * The primary server task will close the control connection
             * when a halt is in progress, causing an error on the socket.
             * In this case, ignore the error and exit the command loop
             * to send a termination message to the connected client.
             */
            /*  ����������ڹر�,�ѽ��յ������ݷ��ظ��ϲ�
            */
            if ( s_bFtpsShutdownFlag )
            {
                *strRecvBuf  = 0;
                return 1;
            }

            if ( iReadSize == -2 )
                return -2;
            else
                return 0;
        }

        iRecvSize += iReadSize;
        strRecvBuf[iRecvSize]   = 0;
    }

    if ( pcBuf == NULL )
    {
        return -1;
    }

    /*  iRecvSize���浱ǰ�г��� */
    iRecvSize = pcBuf - strRecvBuf;

    /*  strRecvBuf�������\r\n, ����\r\n���滹������,��*pstrNextָ����һ������
    */
    if ( 0 != pcBuf[2] )
    {
        *pstrNext   = pcBuf +2;
    }

    /*  ��\r\n���\0 */
    *pcBuf-- = 0;

    /*  ȥ����β�ո� */
    for ( ;iRecvSize > 0 && isspace(*pcBuf); iRecvSize --, pcBuf--)
    {}

    pcBuf[1] = 0;


    /* �ٰѵ�һ������(������)ת���ɴ�д */
    for (pcBuf = strRecvBuf; (*pcBuf != ' ') && (*pcBuf != 0); pcBuf++)
    {
        *pcBuf = toupper (*pcBuf);
    }

    return 1;
}

/*******************************************************************************
 *
 * ftpdWorkTask - main protocol processor for the FTP service
 *
 * This function handles all the FTP protocol requests by parsing
 * the request string and performing appropriate actions and returning
 * the result strings.  The main body of this function is a large
 * FOREVER loop which reads in FTP request commands from the client
 * located on the other side of the connection.  If the result of
 * parsing the request indicates a valid command, ftpdWorkTask() will
 * call appropriate functions to handle the request and return the
 * result of the request.  The parsing of the requests are done via
 * a list of strncmp routines for simplicity.
 *
 * RETURNS: N/A
 *
 * ERRNO: N/A
 *
 * NOMANUAL
 *
 * intERNAL
 * To handle multiple simultaneous connections, this routine and all secondary
 * routines which process client commands must be re-entrant. If the server's
 * halt routine is started, the shutdown flag is set, causing this routine to
 * exit after completing any operation already in progress.
 */

static void ftpdWorkTask
(
    FTPD_SESSION_DATA   *pSlot  /* pointer to the active slot to be handled */
)
{
    T_CmdInfo *ptCmd;          /* ָ��Ҫִ�е����� */
    int sock;           /* command socket descriptor */
    char strRecvBuf[BUFSIZE];
    char *pBuf;              /* pointer to session specific buffer */
    char *pstrNext   = NULL; /* pointer to next line in buffer */
    sock = pSlot->cmdSock;
    if ( true == s_bFtpsShutdownFlag )
    {
        /* Server halt in progress - send abort message to client. */
        ftpdCmdSend (pSlot, sock, 421,
                     "Service not available, closing control connection",
                     0, 0, 0, 0, 0, 0);
        ftpdSessionDelete (pSlot);
        return ;
    }

    /* tell the client we're ready to rock'n'roll */
    /* ��ӡ��ӭ�� */

    if ( ERROR == ftpdCmdSend (pSlot, sock, 220, s_astrMessages [MSG_SERVER_READY],
                               (SWORDPTR)runtimeVersion, 0, 0, 0, 0, 0) )

    {
        ftpdSessionDelete (pSlot);
        return ;
    }

    /*record harddisk*/
    /* BSP_AccessFileRec(HARDDISK_IDE_PREFIX);*/

    memset(strRecvBuf,0,sizeof(strRecvBuf));

    /* ��ʼ������������������ */
    for (;;)
    {
        int     ret = 0;
        /* Check error in writting to the control socket */

        usleep (1000);          /* time share among same priority tasks */

        if ( ERROR == pSlot->cmdSockError )
        {
            ftpdSessionDelete (pSlot);
            return ;
        }

        /*
         * Stop processing client requests if a server halt is in progress.
         * These tests of the shutdown flag are not protected with the
         * mutual exclusion semaphore to prevent unnecessary synchronization
         * between client sessions. Because the secondary tasks execute at
         * a lower priority than the primary task, the worst case delay
         * before ending this session after shutdown has started would only
         * allow a single additional command to be performed.
         */
        if ( s_bFtpsShutdownFlag )
        {
            break;
        }

        /* get a request command */
        ret     = ReadAFTPCmd(sock,strRecvBuf,sizeof(strRecvBuf),&pstrNext);
        if ( ret <= 0 )
        {
            switch ( ret )
            {
            case 0:
                ftpdCmdSend (pSlot, sock, 221, s_astrMessages [MSG_NO_GOOD_BYE],
                             0, 0, 0, 0, 0, 0);
                break;

            case -1:
                ftpdCmdSend (pSlot, sock, 500, "Invalid command string,length > 1023",
                             0, 0, 0, 0, 0, 0);
                break;

            case -2:
                ftpdCmdSend (pSlot, sock, 500, "wait command timeout",
                             0, 0, 0, 0, 0, 0);
                break;

            default:
                ftpdCmdSend (pSlot, sock, 500, "read command error",
                             0, 0, 0, 0, 0, 0);
                break;
            }
            break;
        }

        /*  Reset Buffer Pointer before we use it */
        pBuf = &strRecvBuf [0];
        Debug(SYS_DEBUG,"read command [%s] from user %s\n", (SWORDPTR)pBuf,pSlot->username);
        /*
         * Send an abort message to the client if a server
         * shutdown was started while reading the next command.
         */
        if (s_bFtpsShutdownFlag)
        {
            ftpdCmdSend (pSlot, sock, 421,
                         "Service not available, closing control connection",
                         0, 0, 0, 0, 0, 0);
            break;
        }

        /*  ����������Ҵ�����    */
        ptCmd    = s_atFtpCmds;
        for ( ; NULL != ptCmd->strToken; ptCmd ++ )
        {
            if ( 0 == strncmp(ptCmd->strToken,pBuf,ptCmd->wTokenLen) )
            {
                break;
            }
        }

        if ( NULL == ptCmd->strToken )
        {
            /*  û���ҵ�������    */
            if ( ERROR == ftpdCmdSend (pSlot, sock, 500, s_astrMessages [MSG_BAD_COMMAND],
                                       0, 0, 0, 0, 0, 0) )
            {
                break;
            }
        }
        else
        {
            /*  �ҵ�������,�ж��ǲ�����Ȩ������ҵ�ǰ�Ѿ���½ */
            if ( (0 != ptCmd->wNeedAuth )
                    && (0 == (pSlot->status & FTPD_USER_OK)) )
            {
                /*  �ҵ�������,���Ǵ˺�����Ҫ��֤�����û�û�е�½, ��ܾ�ִ�� */
                if ( ERROR == ftpdCmdSend (pSlot, sock, 530, s_astrMessages [MSG_USER_PASS_REQ],
                                           0, 0, 0, 0, 0, 0) )
                {
                    break;
                }
            }
            else
            {
                /* ��������в�������pBufָ���������ʼ����
                 * ���û�в���, ��ָ��\0
                 */
                pBuf    += ptCmd->wTokenLen;
                if ( 0 != *pBuf )
                {
                    pBuf ++;
                }

                /* ִ������, �������ִ�г���(�������),��ֹͣ */
                if ( false == ptCmd->pFunc(pSlot,pBuf) )
                {
                    break;
                }
            }

        }
    }

    /*
     * Processing halted due to pending server shutdown.
     * Remove all resources and exit.
     */

    ftpdSessionDelete (pSlot);
    return ;
}

/*******************************************************************************
 *
 * ftpdDataConnGet - get a fresh data connection socket for FTP data transfer
 *
 * FTP uses upto two connections per session (as described above) at any
 * time.  The command connection (cmdSock) is maintained throughout the
 * FTP session to pass the request command strings and replies between
 * the client and the server.  For commands that require bulk data transfer
 * such as contents of a file or a list of files in a directory, FTP
 * sets up dynamic data connections separate from the command connection.
 * This function, ftpdDataConnGet, is responsible for creating
 * such connections.
 *
 * Setting up the data connection is performed in two ways.  If the dataSock
 * is already initialized and we're in passive mode (as indicated by the
 * FTPD_PASSIVE bit of the status field in the FTPD_SESSION_SLOT) we need to
 * wait for our client to make a connection to us -- so we just do an accept
 * on this already initialized dataSock.  If the dataSock is already
 * initialized and we're not in passive mode, we just use the already
 * existing connection.  Otherwise, we need to initialize a new socket and
 * make a connection to the the port where client is accepting new
 * connections.  This port number is in general set by "PORT" command (see
 * ftpdWorkTask()).
 */

static STATUS ftpdDataConnGet
(
    FTPD_SESSION_DATA   *pSlot          /* pointer to the work slot */
)
{
    int             newSock;    /* new connection socket */
    socklen_t          addrLen;        /* to be used with accept */
    struct sockaddr_in addr;           /* to be used with accept */
    int                 on = 1;         /* to be used to turn things on */
    int         ftpdWindowSize     = FTPD_WINDOW_SIZE;
    struct timeval      timev;

    /* command socket is invalid, return immediately */

    if (pSlot->cmdSock == SOCKET_ERROR)
        return (ERROR_NORESP);

    pSlot->curDataSockType = false;

    pSlot->byteCount = 0;

    if (pSlot->dataSock != SOCKET_ERROR)
    {
        /* data socket is already initialized */

        /* are we being passive? (should we wait for client to connect
         * to us rather than connecting to the client?)
         */

        if (pSlot->status & FTPD_PASSIVE)
        {
            /* we're being passive.  wait for our client to connect to us. */

            addrLen = sizeof (struct sockaddr);

            if ((newSock = AcceptTO (pSlot->dataSock, (struct sockaddr *) &addr,
                                     &addrLen,DEF_ACCEPT_TIMEOUT)) < 0)
            {
                ftpdCmdSend (pSlot, pSlot->cmdSock,
                             425, "Can't open data connection",
                             0, 0, 0, 0, 0, 0);
                ftpdSockFree (&pSlot->dataSock);
                /* we can't be passive no more */
                pSlot->status &= ~FTPD_PASSIVE;
                return (ERROR);
            }

            /*
             * Enable the keep alive option to prevent misbehaving clients
             * from locking the server.
             */
            if (setsockopt (newSock, SOL_SOCKET, SO_KEEPALIVE, (char *) &on,
                            sizeof (on)) != 0)
            {
                ftpdSockFree (&pSlot->dataSock);
                return (ERROR_NORESP);
            }

            on  = 1;
            if (setsockopt(newSock, SOL_TCP, TCP_NODELAY,
                           (char *)&on, sizeof (on)))
                Debug(SYS_WARNING,"Couldn't set IO no delay option\n");

            /* set the window size  */

            //if (setsockopt(newSock, SOL_SOCKET, SO_SNDBUF,
            //               (char *)&ftpdWindowSize, sizeof (ftpdWindowSize)))
            //    Debug(SYS_WARNING,"Couldn't set the Send Window to 10k\n");

            //if (setsockopt(newSock, SOL_SOCKET, SO_RCVBUF,
            //                (char *)&ftpdWindowSize, sizeof (ftpdWindowSize)))
            //    Debug(SYS_WARNING,"Couldn't set the Send Window to 10k\n");

            /* replace the dataSock with our new connection */

            (void) close (pSlot->dataSock);
            pSlot->dataSock = newSock;

            /* N.B.: we stay passive */

            if ( ERROR == ftpdCmdSend (pSlot, pSlot->cmdSock,
                                       150, "Opening %s mode data connection",
                                       pSlot->status & FTPD_ASCII_TYPE ? (SWORDPTR) "ASCII"
                                       : (SWORDPTR) "BINARY", 0, 0, 0, 0, 0) )
            {
                (void) close (pSlot->dataSock);
                return (ERROR);
            }

            return (OK);
        }
        else
        {
            /* reuse the old connection -- it's still useful */

            if ( ERROR == ftpdCmdSend (pSlot, pSlot->cmdSock,
                                       125, "Using existing data connection",
                                       0, 0, 0, 0, 0, 0) )
            {
                ftpdSockFree (&pSlot->dataSock);
                return (ERROR);
            }
            return (OK);
        }
    }
    else
    {
        /* Determine address for local end of connection. */

        addrLen = sizeof (struct sockaddr);

        if (getsockname (pSlot->cmdSock, (struct sockaddr *) &addr, &addrLen)
                < 0)
        {
            return (ERROR_NORESP);
        }

        /* Replace control port with default data port. */

        addr.sin_port = htons (s_ftp_data_port);

        /* open a new data socket */

        if ((pSlot->dataSock = socket (AF_INET, SOCK_STREAM, 0)) < 0)
        {
            return (ERROR_NORESP);
        }

        if (setsockopt (pSlot->dataSock, SOL_SOCKET,
                        SO_REUSEADDR, (char *) &on, sizeof (on)) < 0 ||
                bind (pSlot->dataSock, (struct sockaddr *) &addr,
                      sizeof (addr)) < 0)
        {
            ftpdSockFree (&pSlot->dataSock);
            return (ERROR_NORESP);
        }


        /* Set socket address to PORT command values or default. */

        bcopy ( (char *)&pSlot->dataAddr, (char *)&addr,
                sizeof (struct sockaddr_in));

        /*
         * 3GCHG00026948: Do not try connect again, so that client can
         * launch a new ftp session with different port.
         */
        /* try until we get a connection to the client's port */

        timev.tv_sec  = 10;
        timev.tv_usec = 0;    /* 2s */

        while ( ConnWithTimeout (pSlot->dataSock,
                                 (struct sockaddr *) &addr, &timev) < 0)
            /*
               while (connect (pSlot->dataSock,
               (struct sockaddr *) &addr, sizeof (addr)) < 0)
               */
        {

            /* timeout -- we give up */
            ftpdCmdSend (pSlot, pSlot->cmdSock,
                         425, "Can't build data connection",
                         0, 0, 0, 0, 0, 0);
            ftpdSockFree (&pSlot->dataSock);
            return (ERROR);
        }

        /*
         * Enable the keep alive option to prevent misbehaving clients
         * from locking the secondary task during file transfers.
         */

        if (setsockopt (pSlot->dataSock, SOL_SOCKET, SO_KEEPALIVE,
                        (char *) &on, sizeof (on)) != 0)
        {
            ftpdSockFree (&pSlot->dataSock);
            return (ERROR_NORESP);
        }

        /* set the window size  */

        if (setsockopt(pSlot->dataSock, SOL_SOCKET, SO_SNDBUF,
                       (char *)&ftpdWindowSize, sizeof (ftpdWindowSize)))
            Debug(SYS_WARNING,"Couldn't set the Send Window to 10k\n");

        if (setsockopt(pSlot->dataSock, SOL_SOCKET, SO_RCVBUF,
                       (char *)&ftpdWindowSize, sizeof (ftpdWindowSize)))
            Debug(SYS_WARNING,"Couldn't set the Send Window to 10k\n");

        if ( ERROR == ftpdCmdSend (pSlot, pSlot->cmdSock,
                                   150, "Opening %s mode data connection",
                                   pSlot->status & FTPD_ASCII_TYPE ? (SWORDPTR) "ASCII" :
                                   (SWORDPTR) "BINARY", 0, 0, 0, 0, 0) )
        {
            ftpdSockFree (&pSlot->dataSock);
            return (ERROR);
        }
    }

    return (OK);
}

/*******************************************************************************
 *
 * ftpdDataStreamSend - send FTP data over data connection
 *
 * When our FTP client does a "RETR" (send me a file) and we find an existing
 * file, ftpdWorkTask() will call us to perform the actual shipment of the
 * file in question over the data connection.
 *
 * We do the initialization of the new data connection ourselves here
 * and make sure that everything is fine and dandy before shipping the
 * contents of the file.  Special attention is given to the type of
 * the file representation -- ASCII or BINARY.  If it's binary, we
 * don't perform the prepending of "\r" character in front of each
 * "\n" character.  Otherwise, we have to do this for the ASCII files.
 *
 * SEE ALSO:
 * ftpdDataStreamReceive  which is symmetric to this function.
 */

static void ftpdDataStreamSend
(
    FTPD_SESSION_DATA   *pSlot,         /* pointer to our session slot */
    FILE                *inStream      , /* pointer to the input file stream */
    char *  pFileName
)
{
    char   *pBuf;                  /* pointer to the session buffer */
    int     netFd;                      /* output socket */
    int     fileFd;                     /* input file descriptor */
    char    ch;                     /* character holder */
    int     cnt;                        /* number of chars read/written */
    FILE        *outStream;             /* buffered output socket stream */
    /*int   retval = 0;*/
    char    acBuffer[FTP_DATA_BUF_SIZE];
    int     ret;

    /* get a fresh connection or reuse the old one */

    if ( (ret = ftpdDataConnGet (pSlot)) != OK )
    {
        Debug(SYS_ERROR,"get data connect fail\n");
        dataError (pSlot,ret == ERROR_NORESP );
        return;
    }

    Debug(SYS_DEBUG,"get data connect succ\n",0,0,0,0);

    /* set type of dataSock is true */
    pSlot->curDataSockType = true;

    /*pBuf = &pSlot->buf [0];*/
    pBuf = acBuffer;

    if (pSlot->status & FTPD_ASCII_TYPE)
    {
        /* ASCII representation */

        /* get a buffered I/O stream for this output data socket */

        if ((outStream = fdopen (pSlot->dataSock, "w")) == NULL)
        {
            Debug(SYS_ERROR,"retr,open data fd err\n");
            dataError (pSlot,1);
            return;
        }

        /* write out the contents of the file and do the '\r' prepending */

        while ((ch = getc (inStream)) != (char) EOF)
        {
            pSlot->byteCount++;

            /* if '\n' is encountered, we prepend a '\r' */

            if (ch == '\n')
            {
                if (ferror (outStream))
                {
                    dataError (pSlot,1);
                    fclose (outStream);
                    return;
                }

                if (putc ('\r', outStream) == EOF)
                {
                    dataError (pSlot,1);
                    fclose (outStream);
                    return;
                }
            }

            if (putc (ch, outStream) == EOF)
            {
                dataError (pSlot,1);
                fclose (outStream);
                return;
            }

            /* Abort the file transfer if a shutdown is in progress. */

            if (ch == '\n' && s_bFtpsShutdownFlag)
            {
                dataError (pSlot,1);
                fclose (outStream);
                return;
            }
        }

        /* flush it out */

        (void) fflush (outStream);

        if (ferror (inStream))
        {
            /* error in reading the file */

            fileError (pSlot);
            fclose (outStream);
            return;
        }

        if (ferror (outStream))
        {
            /* error in sending the file */

            dataError (pSlot,1);
            fclose (outStream);
            return;
        }

        fclose (outStream);

        /* everything is okay */
        transferOkay (pSlot);
    }
    else if (pSlot->status & FTPD_BINARY_TYPE)
    {
        /* BINARY representation */

        netFd = pSlot->dataSock;

        /* get a raw descriptor for this input file */

        fileFd = fileno (inStream);

        /* unbuffered block I/O between file and network */
#if 0
        while ((cnt = read (fileFd, pBuf, sizeof(acBuffer))) > 0 &&
                (retval = write (netFd, pBuf, cnt)) == cnt)
        {
            pSlot->byteCount += cnt;

            if (s_bFtpsShutdownFlag)
            {
                /* Abort the file transfer if a shutdown is in progress. */

                cnt = 1;
                break;
            }
        }
#endif
        /*ͨ��������������������*/
        Debug(SYS_DEBUG,"ftpdDataStreamSend:trans data begin \n");
        cnt = transferByCtrl(pSlot,fileFd,pFileName,netFd,pBuf,sizeof(acBuffer));
        Debug(SYS_DEBUG,"ftpdDataStreamSend:trans pFileName = %s   finish ret:%d\n",pFileName,cnt);
        if (cnt != 0)
        {
            Debug(SYS_ERROR,"ftpdDataStreamSend:trans pFileName = %s  finish ret:%d\n",pFileName,cnt);
        }

        /* cnt should be zero if the transfer ended normally */

        if (cnt != 0)
        {
            if (cnt < 0)
            {
                MTU_PRN("@@ ftpsvr send file error @@\n");
                fileError (pSlot);
                return;
            }

            /*PRN_MSG(PRN_LEVEL_ERROR,"read %d bytes, wrote %d bytes\n", cnt, retval;*/

            dataError (pSlot,1);
            return;
        }

        transferOkay (pSlot);
    }
    else
    {
        Debug(SYS_ERROR,"RETR data  in unexpect\n");
        unImplementedType (pSlot);      /* invalide representation type */
    }
}

/*******************************************************************************
 *
 * ftpdDataStreamReceive - receive FTP data over data connection
 *
 * When our FTP client requests "STOR" command and we were able to
 * create a file requested, ftpdWorkTask() will call ftpdDataStreamReceive
 * to actually carry out the request -- receiving the contents of the
 * named file and storing it in the new file created.
 *
 * We do the initialization of the new data connection ourselves here
 * and make sure that everything is fine and dandy before receiving the
 * contents of the file.  Special attention is given to the type of
 * the file representation -- ASCII or BINARY.  If it's binary, we
 * don't perform the handling of '\r' character in front of each
 * '\n' character.  Otherwise, we have to do this for the ASCII files.
 *
 * SEE ALSO:
 * ftpdDataStreamSend which is symmetric to this function.
 */

static STATUS ftpdDataStreamReceive(FTPD_SESSION_DATA *pSlot,FILE *outStream)
{
    char        *pBuf;          /* pointer to the session buffer */
    char        ch;             /* character holder */
    FILE        *inStream;      /* buffered input file stream for data socket */
    int     fileFd;             /* output file descriptor */
    int     netFd;              /* network file descriptor */
    int        dontPutc;       /* flag to prevent bogus chars */
    int     cnt;                /* number of chars read/written */
    int     ret;

    /* get a fresh data connection or reuse the old one */
    if ((ret = ftpdDataConnGet (pSlot) ) != OK)
    {
        dataError (pSlot,ret == ERROR_NORESP);
        return ERROR;
    }
    pBuf = &pSlot->buf [0];
    if (pSlot->status & FTPD_ASCII_TYPE)
    {
        /* ASCII representation */
        /* get a buffer I/O stream for the input data socket connection */
        if ((inStream = fdopen (pSlot->dataSock, "r")) == NULL)
        {
            dataError (pSlot,1);
            return ERROR;
        }

        /* read in the contents of the file while doing the '\r' handling */

        while ((ch = getc (inStream)) != (char) EOF)
        {
            dontPutc = false;
            pSlot->byteCount++;

            /* a rather strange handling of sequences of '\r' chars */
            while (ch == '\r')
            {
                if (ferror (outStream))
                {
                    dataError (pSlot,1);
                    fclose (inStream);
                    return ERROR;
                }

                /* replace bogus chars between '\r' and '\n' chars with '\r' */
                if ((ch = getc (inStream)) != '\n')
                {
                    (void) putc ('\r', outStream);
                    if (ch == '\0' || ch == (char) EOF)
                    {
                        dontPutc = true;
                        break;
                    }
                }
            }

            if (dontPutc == false)
                (void) putc (ch, outStream);

            /* Abort file transfer if a shutdown is in progress. */
            if (ch == '\n' && s_bFtpsShutdownFlag)
            {
                dataError (pSlot,1);
                fclose (inStream);
                return ERROR;
            }
        }

        /* flush out to the file */
        (void) fflush (outStream);
        if (ferror (inStream))
        {
            /* network input error */
            dataError (pSlot,1);
            fclose (inStream);
            return ERROR;
        }

        if (ferror (outStream))
        {
            /* file output error */
            fileError (pSlot);
            fclose (inStream);
            return ERROR;
        }

        /* we've succeeded! */
        fclose (inStream);
        transferOkay (pSlot);
    }
    else if (pSlot->status & FTPD_BINARY_TYPE)
    {
        /* BINARY representation */
        /* get a raw descriptor for output file stream */
        fileFd = fileno (outStream);
        netFd  = pSlot->dataSock;
        /* perform non-buffered block I/O between network and file */
#if 0
        while ((cnt = read (netFd, pBuf, BUFSIZE)) > 0)
        {
            if (write (fileFd, pBuf, cnt) != cnt)
            {
                fileError (pSlot);
                return;
            }

            pSlot->byteCount += cnt;

            /* Abort the file transfer if a shutdown is in progress. */

            if (s_bFtpsShutdownFlag)
            {
                cnt = -1;
                break;
            }
        }

        if (cnt < 0)
        {
            dataError (pSlot);
            return;
        }
#endif
        /*���������޸ģ���ʼ*/
        cnt = transferByCtrl(pSlot,netFd,NULL,fileFd,pBuf,BUFSIZE);
        if (cnt != 0)
        {
            if (cnt < 0)
            {
                MTU_PRN("@@ftpsvr Rcv file error !!! @@\n");
                fileError (pSlot);
                return ERROR;
            }

            /*PRN_MSG(PRN_LEVEL_DEBUG,"read %d bytes, wrote %d bytes\n", cnt, retval,0,0);*/
            dataError (pSlot,1);
            return ERROR;
        }

        /*�������ƣ�����*/
        /* we've done it */
        transferOkay (pSlot);
    }
    else
    {
        unImplementedType (pSlot);      /* invalid representation type */
        return ERROR;
    }
    return SUCCESS;
}

/*******************************************************************************
 *
 * ftpdSockFree - release a socket
 *
 * This function is used to examine whether or not the socket pointed
 * by pSock is active and release it if it is.
 */

static void ftpdSockFree
(
    int *pSock
)
{
    if (*pSock != SOCKET_ERROR)
    {
        int off = 0;

        /*
         *  �ر�socketʱҪ����socket��FIN_WAIT_2״̬������ʹ�����沽��ر�:
         *      1.  �ر�KEEP_ALIVEѡ��
         *      2.  ����SO_LINGER,
         *      3.  ����shutdown, ��Զ˷���FIN��
         *      4.  ����close,����رյȴ���
         *  ����vxworks��linux��vxworks����SO_LINGER={1,0},��closeǰsleepһ��ʱ�䣬
         *  ����Ϊ����ԭ���Ĵ������
         *  ����linux,ֱ��ָ��SO_LINGERΪ{1,timeout},��������closseʱ�ͻ��Զ��ȴ�
         */

        struct linger so_linger =
            {
                1, CLOSE_DELAY_TIME
            };



        setsockopt (*pSock, SOL_SOCKET, SO_LINGER, (void *)&so_linger, sizeof (so_linger));
        setsockopt (*pSock, SOL_SOCKET, SO_KEEPALIVE, (char *)&off, sizeof (off));

        /*
         * Now that we have said we do not want to keep the PCBs around for 60
         * seconds (which is what happens is SO_LINGER is enabled in vxWorks), we
         * need to call shutdown to gracefully terminate the connection without
         * loosing any data.
         */

        shutdown (*pSock, 2);

        Debug(SYS_DEBUG,"ftpdLib: (%d) Closing sock %d\n", __LINE__, *pSock, 0, 0);

        (void) close (*pSock);
        *pSock = SOCKET_ERROR;
    }
}

/**********************************************************************
 * �������ƣ�DecodeFileMode
 * ����������
 * ���ʵı���
 * �޸ĵı���
 * ���������
 *           mode    �ļ���ģʽ
 * ���������
 * �� �� ֵ���ļ��������ַ�
 * ����˵������
 * �޸�����      �汾��  �޸���      �޸�����
 * ---------------------------------------------------------------------
 *  2007.08.27  lizl    ��betaftpd���븴��
 ************************************************************************/
static char DecodeFileMode(mode_t mode)
{
    /*
     * S_IFLINK seems to be broken? or perhaps I just have missed
     * something (S_IFLINK is always set for all *files* on my
     * glibc 2.0.111 system)
     */

    /* ordered for speed */
    if (mode & S_IFREG)  return '-';
    if (mode & S_IFDIR)  return 'd';
    if (mode & S_IFLNK)  return 'l';
    if (mode & S_IFBLK)  return 'b';
    if (mode & S_IFCHR)  return 'c';
    if (mode & S_IFSOCK) return 's';
    if (mode & S_IFIFO)  return 'f';

    /* hmmm, i'm a unix newbie -- what is the sticky bit? */
    return '-';
}

/*
 *  �����ļ���״̬��дftp������Ϣ
 *
 *  2007.08.27  lizl    ��betaftpd�����޸�
 */

/**********************************************************************
 * �������ƣ�FileInfo2ListResult
 * ������������һ���ļ���״̬����list���ؽ��
 * ���ʵı���
 * �޸ĵı���
 * ���������
 *           outbufsize  outbuf�Ĵ�С
 *           year        ��������
 *           pstatus     �ļ�״̬
 *           fname       �ļ���
 * ���������
 *           outbuf      ���������������������
 *
 * �� �� ֵ����outbuf��д��ĳ���
 *
 * ����˵����
 *
 * �޸�����      �汾��  �޸���      �޸�����
 * ---------------------------------------------------------------------
 *  2007.08.27  lizl    ��betaftpd���븴��
 ************************************************************************/
static int FileInfo2ListResult(char *strOut,int iOutSize,
                               int iYear,
                               const struct stat *ptStatus,const char *strFName )
{

    struct tm       when;

    int             nLink           = 0;

    char            strDate[32];
    char            strUsr[32]      = "user";
    char            strGrp[32]      = "group";

    static const char months[][4] =
        { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
          "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
        };

    /*  ����linux,����id���û���Ϣ��ת�� */
#if 0
    //���ھ�̬����ʱ����warning, ��ת������ȥ��
    if ( 0 == getpwuid_r(ptStatus->st_uid,&tPwd,strPwdBuf,256,&pPwd) )
    {
        strncpy(strUsr, pPwd->pw_name,sizeof(strUsr));
        strUsr[31]      = 0;
    }
#endif
    sprintf(strUsr, "%u", ptStatus->st_uid);

#if 0
    if ( 0 == getgrgid_r(ptStatus->st_gid,&tGrp,strPwdBuf,256,&pGrp) )
    {
        strncpy(strGrp, pGrp->gr_name,sizeof(strGrp));
        strGrp[31]      = 0;
    }
#endif
    sprintf(strGrp, "%u", ptStatus->st_gid);

    /*  ���ڳ�ģʽ������ļ���ʱ����ʾ�뵱ǰ������
     *  ����ļ��޸�ʱ��Ϊ��ǰ�꣬����ʾ�¡��ա�ʱ��Ϣ
     *  ��Ȼ��ʾ�ꡢ�¡�����Ϣ
     */
    localtime_r(&(ptStatus->st_mtime),&when);
    if (when.tm_year == iYear)
    {
        sprintf(strDate, "%3s %2u %02u:%02u",
                months[when.tm_mon], when.tm_mday, when.tm_hour, when.tm_min);
    }
    else
    {
        sprintf(strDate, "%3s %2u %5u",
                months[when.tm_mon], when.tm_mday, when.tm_year + 1900);
    }

    nLink   = ptStatus->st_nlink;

    return snprintf(strOut, iOutSize,
                    "%c%c%c%c%c%c%c%c%c%c %3u %-8s %-8s %8lu %12s %s\r\n",
                    DecodeFileMode(ptStatus->st_mode),
                    (ptStatus->st_mode & S_IRUSR) ? 'r' : '-',
                    (ptStatus->st_mode & S_IWUSR) ? 'w' : '-',
                    (ptStatus->st_mode & S_IXUSR) ? ((ptStatus->st_mode & S_ISUID) ? 's' : 'x') : '-',
                            (ptStatus->st_mode & S_IRGRP) ? 'r' : '-',
                            (ptStatus->st_mode & S_IWGRP) ? 'w' : '-',
                            (ptStatus->st_mode & S_IXGRP) ? ((ptStatus->st_mode & S_ISGID) ? 's' : 'x') : '-',
                            (ptStatus->st_mode & S_IROTH) ? 'r' : '-',
                            (ptStatus->st_mode & S_IWOTH) ? 'w' : '-',
                            (ptStatus->st_mode & S_IXOTH) ? 'x' : '-',
                            nLink, strUsr, strGrp, ptStatus->st_size,
                            strDate, strFName);
}

/*******************************************************************************
 *
 * ftpdDirListGet - list files in a directory for FTP client
 *
 * This function performs the client's request to list out all
 * the files in a given directory.  The VxWorks implementation of
 * stat() does not work on RT-11 filesystem drivers, it is simply not supported.
 *
 * This command is similar to UNIX ls.  It lists the contents of a directory
 * in one of two formats.  If <doLong> is false, only the names of the files
 * (or subdirectories) in the specified directory are displayed.  If <doLong>
 * is true, then the file name, size, date, and time are displayed.  If
 * doing a long listing, any entries that describe subdirectories will also
 * be flagged with a "DIR" comment.
 *
 * The <dirName> parameter specifies the directory to be listed.  If
 * <dirName> is omitted or NULL, the current working directory will be
 * listed.
 *
 * Empty directory entries and MS-DOS volume label entries are not
 * reported.
 *
 * intERNAL
 * Borrowed from ls() in usrLib.c.
 *
 * RETURNS:  OK or ERROR.
 *
 * SEE ALSO: ls(), stat()
 *
 *  20070827    lizl    �޸�ʹ�����ϱ�׼ftp����ķ���
 */
static STATUS ftpdDirListGet
(
    FTPD_SESSION_DATA   *pSlot, /* Session Info */
    int         sd,             /* socket descriptor to write on */
    const char *dirName,       /* name of the directory to be listed */
    int    Long            /*  �ǲ���Ҫ�����  */
)
{
    STATUS              iRet;       /* return status */

    DIR                 *pDir;      /* ptr to directory descriptor */
    struct dirent       *pDirEnt;   /* ptr to dirent */

    int                 iSize;      /*  outcache���ݴ�С   */

    char                strFileName [OSSSVR_FILEPATH_SIZE];    /*  �ļ���ȫ·���� */
    struct stat         tStatus;    /*  �ļ���Ϣ    */

    time_t              iNow;       /*  ��ǰʱ��, ��λΪ��  */
    struct tm           tDay;       /*  ��ǰ����,��Ϊ ��ǰ���-1900 */

    /* If no directory name specified, use "." */

    if (dirName == NULL)
        dirName = ".";

    /* Open dir */

    if ((pDir = opendir (dirName)) == NULL)
    {
        snprintf(pSlot->buf,sizeof(pSlot->buf),"Can't open \"%s\".\r\n", dirName);
        SEND_STR(sd,pSlot->buf);
        return (ERROR);
    }


    /* List files */

    iRet = OK;

    if ( true == Long )
    {
        /*  ���ڳ�ģʽ������ļ���ʱ����ʾ�뵱ǰ������
         *  ����ļ��޸�ʱ��Ϊ��ǰ�꣬����ʾ�¡��ա�ʱ��Ϣ
         *  ��Ȼ��ʾ�ꡢ�¡�����Ϣ
         */
        time(&iNow);
        localtime_r(&iNow,&tDay);
    }

    while ( (OK == iRet) && (NULL != (pDirEnt = readdir(pDir))) )
    {
        if ( true == Long )
        {
            MkPath (dirName, pDirEnt->d_name, strFileName,sizeof(strFileName));

            iRet = stat(strFileName, &tStatus);

            if ( OK != iRet )
            {
                break;
            }

            iSize    = FileInfo2ListResult(pSlot->buf,sizeof(pSlot->buf),
                                           tDay.tm_year,&tStatus,pDirEnt->d_name);
        }
        else
        {
            iSize    = snprintf(pSlot->buf,sizeof(pSlot->buf),"%s\r\n",pDirEnt->d_name);
        }

        if ( ERROR == send(sd,pSlot->buf,iSize,0) )
        {
            iRet = ERROR;
        }
    }

    closedir (pDir);

    return (iRet);
}


/*******************************************************************************
 *
 * unImplementedType - send FTP invalid representation type error reply
 */

static void unImplementedType
(
    FTPD_SESSION_DATA   *pSlot
)
{
    ftpdCmdSend (pSlot, pSlot->cmdSock, 550, s_astrMessages [MSG_TYPE_ERROR],
                 FTPD_REPRESENTATION (pSlot), 0, 0, 0, 0, 0);
    ftpdSockFree (&pSlot->dataSock);
}

/*******************************************************************************
 *
 * dataError - send FTP data connection error reply
 *
 * Send the final error message about connection error and delete the session.
 */

static void dataError
(
    FTPD_SESSION_DATA   *pSlot,int need_resp
)
{
    if ( need_resp )
        ftpdCmdSend (pSlot, pSlot->cmdSock, 426, s_astrMessages [MSG_DATA_CONN_ERROR],
                     0, 0, 0, 0, 0, 0);
    ftpdSockFree (&pSlot->dataSock);
}

/*******************************************************************************
 *
 * fileError - send FTP file I/O error reply
 *
 * Send the final error message about file error and delete the session.
 */

static void fileError
(
    FTPD_SESSION_DATA  *pSlot
)
{
    ftpdCmdSend (pSlot, pSlot->cmdSock, 551, s_astrMessages [MSG_INPUT_FILE_ERROR],
                 0, 0, 0, 0, 0, 0);
    ftpdSockFree (&pSlot->dataSock);
}

/*******************************************************************************
 *
 * transferOkay - send FTP file transfer completion reply
 */

static void transferOkay(FTPD_SESSION_DATA *pSlot, const char* prompt)
{
    if (pSlot->curDataSockType == true)
    {
        ftpdDataSockFree(pSlot,&pSlot->dataSock);
        pSlot->curDataSockType = false;
    }
    else
    {
        ftpdSockFree (&pSlot->dataSock);
    }

    if(prompt)
    {
        ftpdCmdSend (pSlot, pSlot->cmdSock, 226, prompt,
                     0, 0, 0, 0, 0, 0);
    }
    else
    {
    ftpdCmdSend (pSlot, pSlot->cmdSock, 226, s_astrMessages [MSG_TRANS_COMPLETE],
                 0, 0, 0, 0, 0, 0);
}
}

/*******************************************************************************
 *
 * ftpdCmdSend - send a FTP command reply
 *
 * In response to a request, we send a reply containing completion
 * status, error status, and other information over a command connection.
 */

static STATUS ftpdCmdSend
(
    FTPD_SESSION_DATA  *pSlot,         /* pointer to the session slot */
    int            controlSock,    /* control socket for reply */
    int                 code,           /* FTP status code */
    const char         *format,        /* printf style format string */
    SWORDPTR                 arg1,
    SWORDPTR                 arg2,
    SWORDPTR                 arg3,
    SWORDPTR                 arg4,
    SWORDPTR                 arg5,
    SWORDPTR                 arg6
)
{
    int                 buflen;
    char                buf [BUFSIZE];          /* local buffer */
    char                *pBuf = &buf [0];       /* pointer to buffer */
    int                lineContinue =
        (code & FTPD_MULTI_LINE) == FTPD_MULTI_LINE;

    /*
     * If this routine is called before a session is established, the
     * pointer to session-specific data is NULL. Otherwise, exit with
     * an error if an earlier attempt to send a control message failed.
     */

    if ( (pSlot != NULL) && (pSlot->cmdSockError == ERROR))
        return (ERROR);

    code &= ~FTPD_MULTI_LINE;   /* strip multi-line bit from reply code */

    /* embed the code first */

    (void) sprintf (pBuf, "%d%c", code, lineContinue ? '-' : ' ');
    pBuf += strlen (pBuf);

    (void) sprintf (pBuf, format, arg1, arg2, arg3, arg4, arg5, arg6);
    pBuf += strlen (pBuf);

    /* telnet style command terminator */

    (void) sprintf (pBuf, "\r\n");

    /* send it over to our client */

    buflen = strlen (buf);

    if ( write (controlSock, buf, buflen) != buflen )
    {
        if (pSlot != NULL)
            pSlot->cmdSockError = ERROR;
        Debug(SYS_ERROR,"sent %s Failed on write\n", (SWORDPTR)buf);
        return (ERROR);    /* Write Error */
    }

    Debug(SYS_DEBUG,"sent %s\n", (SWORDPTR)buf);
    return (OK);                        /* Command Sent Successfully */
}


/*******************************************************************************
 *
 * ftpdDataSockFree - release a socket
 *
 * This function is used to examine whether or not the socket pointed
 * by pSock is active and release it if it is.
 */

static void ftpdDataSockFree
(
    FTPD_SESSION_DATA   *pSlot,
    int *pSock
)
{
    int off = 0;
    struct linger so_linger =
        {
            1, CLOSE_DELAY_TIME
        };

    setsockopt (*pSock, SOL_SOCKET, SO_KEEPALIVE, (char *)&off, sizeof (off));
    setsockopt (*pSock, SOL_SOCKET, SO_LINGER, (void *)&so_linger, sizeof (so_linger));
    /* TODO,lizl,20070903,�ȴ�����ΰȷ�� */
    /*
     * Now that we have said we do not want to keep the PCBs around for 60
     * seconds (which is what happens is SO_LINGER is enabled in vxWorks), we
     * need to call shutdown to gracefully terminate the connection without
     * loosing any data.
     */

    shutdown (*pSock, 2);

    /* close the socket */
    close (*pSock);
    *pSock = SOCKET_ERROR;
}

/**********************************************************************
 * �������ƣ�static STATUS transferByCtrl(FTPD_SESSION_DATA *pSlot,int inFd,int outFd,char *pBuf,int bufSize)
 * ���������������������´�������
 * ���ʵı���
 * �޸ĵı���
 * ���������
 FTPD_SESSION_DATA    *pSlot  :   ��ǰ�ĻỰ����
 int                  inFd  ��   Դ�ļ�fd
 int                  outFd :    Ŀ���ļ�fd
 char                 *pBuf   :    �û������ļ��Ļ���
 int                  bufSize :    ����Ļ����С

 * �����������
 * �� �� ֵ��
 ���һ�ζ����ֽ��������ݴ˷���ֵ���жϴ����Ƿ�ɹ���
 * ����˵������
 * �޸�����        �汾��     �޸���          �޸�����
 * -----------------------------------------------
 *
 ************************************************************************/
static int transferByCtrl(FTPD_SESSION_DATA *pSlot,int inFd,char* pInFileName, int outFd,char *pBuf,int bufSize)
{
    int cnt = 0;
    int retval = 0;
    int iErrNo;
    WORD32  dwSuccBytes = 0;
    char *pTowrite =NULL;
    socklen_t optlen= sizeof(int);
    int ret=0;
    int val=0;

        while ((cnt = ReadTO (inFd, pBuf, bufSize,DEF_READ_TIMEO)) > 0 )
        {
            retval = write (outFd, pBuf, cnt);
            pSlot->byteCount += retval;
            dwSuccBytes += retval;
            pTowrite = pBuf;
            while ( retval >0 && retval <cnt)
            {
                pTowrite = pTowrite + retval;
                Debug(SYS_ERROR,"transferByCtrl: Write More Than One Time\n");
                cnt = cnt - retval;
                retval = write (outFd, pTowrite, cnt);
                pSlot->byteCount += retval;
                dwSuccBytes += retval;
            }
            if (retval < 0)
            {
                Debug(SYS_ERROR,"transferByCtrl: The return value of write is %d, errno = %d\n",retval,errno);
                break;
            }

            if (s_bFtpsShutdownFlag)
            {
                /* Abort the file transfer if a shutdown is in progress. */
                // cnt = 111;
                cnt = -1;
                break;
            }
        }/*end while */

    if (cnt > 0)
    {
        iErrNo = errno;

        ret = getsockopt(pSlot->dataSock,SOL_SOCKET,SO_ERROR,&val, &optlen);
        if (0== ret)
        {
            MTU_PRN("@@ Socket SO_ERROR:%d @@\n",  val);
        }

        ret = getsockopt(pSlot->dataSock,SOL_SOCKET,SO_SNDBUF,&val, &optlen);
        if (0== ret)
        {
            MTU_PRN("@@ Socket Send Buf Size:%d @@\n",  val);
        }

        ret = getsockopt(pSlot->dataSock,SOL_SOCKET,SO_RCVBUF,&val, &optlen);
        if (0== ret)
        {
            MTU_PRN("@@ Socket Rcv Buf Size:%d @@\n",  val);
        }

        ret = getsockopt(pSlot->dataSock,IPPROTO_TCP,TCP_MAXSEG,&val, &optlen);
        if (0== ret)
        {
            MTU_PRN("@@ TCP_MAXSEG:%d @@\n",  val);
        }
        if ( NULL == pInFileName)
        {
            MTU_PRN("@@ Succ Transfer: %d bytes @@\n",  dwSuccBytes);
        }
        else
        {
            MTU_PRN("@@ Infile: %s,  Succ Transfer: %d bytes @@\n", pInFileName, dwSuccBytes);
        }
        MTU_PRN("@@ read %d bytes, wrote %d bytes,errno is :%d @@ \n", cnt, retval,iErrNo);
    }
    return (cnt);
}

/**********************************************************************
 * �������ƣ�static char *PathSimplify(char *strFilename)
 * ������������·���ı�ʾ����·���е�..�ȷ���ת��һ��
 * ���ʵı���
 * �޸ĵı���
 * ���������
 *           strFilename     Ҫת����·��
 * ���������strFilename     ת�����·��
 *
 * �� �� ֵ��strFilename
 *
 *
 * ����˵������
 *
 * �޸�����      �汾��    �޸���      �޸�����
 * ---------------------------------------------------------------------
 * 2007/8/30    V1.0    ������      ����
 ************************************************************************/
static char *PathSimplify(char *strFilename)
{
    /* lizl 20070824, ��wzdftpd-0.8.2�����и��ƹ���
     * \brief remove // /./ and /../ from filename
     *
     * Return filename with any useless component removed: double /
     * /./ or /../
     * Modifications does not check that filename is valid.
     * WARNING: this function does NOT check anything on filename, it just
     * operates on the raw string (i.e it is the responsability of the caller
     * eo check that there is no path injection in string
     * (eg: "c:/../d:/pathname" )
     * This function modify filename !
     */

    int iPos, iPos2;

    if (!strFilename) return strFilename;

    iPos = iPos2 = 0;

    while ( '\0' != strFilename[iPos] )
    {
        switch ( strFilename[iPos] )
        {
        case '/':
            if ( '/' == strFilename[iPos+1] )
            {}
            else if (( 0 == strncmp(strFilename + iPos, "/./", 3) ) ||
                     ( 0 == strcmp(strFilename + iPos, "/.") ))
            {
                iPos++;
            }
            else if (( 0 == strncmp(strFilename + iPos, "/../", 4) ) ||
                     ( 0 == strcmp(strFilename + iPos, "/..") ))
            {
                if (iPos2 > 1)
                {
                    iPos2--;
                }
                while ((iPos2 > 0) && (strFilename[iPos2] != '/'))
                {
                    iPos2--;
                }

                iPos += 2; /* /.. */
                if (strFilename[iPos2] != '/') /* ex: toto/../dir */
                {
                    iPos++;
                }
            }
            else
            {
                strFilename[iPos2] = '/';
                iPos2++;
            }
            break;

        default:
            strFilename[iPos2] = strFilename[iPos];
            iPos2++;
        }
        iPos++;
    }

    if (iPos2 == 0)
    {
        strFilename[iPos2]  = '/';
        iPos2++;
    }
    strFilename[iPos2]      = '\0';

    return strFilename;
}

/**********************************************************************
 * �������ƣ�DbgPrintFtpRegister
 * ������������ӡftp �ļ�ע������ڵ���
 * ���ʵı���
 * �޸ĵı���
 * ���������
 * ���������
 * �� �� ֵ��
 * ����˵����
 * �޸�����      �汾��  �޸���      �޸�����
 * ---------------------------------------------------------------------
 * 2012/08/23            V1.0          �              ����
 ************************************************************************/
void DbgPrintFtpFileRegister(void)
{
    multimap< int64, string > ::iterator iter;

    if(ftpFileRegister.empty())
    {
        Debug(SYS_NOTICE,"ftpFileRegister is empty\n");
        return;
    }

    for( iter = ftpFileRegister.begin(); iter != ftpFileRegister.end(); iter++ )
    {
        Debug(SYS_NOTICE,"uid: %u, filename: %s\n", iter->first,iter->second.c_str());
    }

    return;
}

DEFINE_DEBUG_FUNC(DbgPrintFtpFileRegister);

/**********************************************************************
 * �������ƣ�DbgClearFtpRegister
 * ��������������ļ�ע����ָ���������ֹ����
 * ���ʵı���
 * �޸ĵı���
 * ���������
 * ���������
 * �� �� ֵ��
 * ����˵����
 * �޸�����      �汾��  �޸���      �޸�����
 * ---------------------------------------------------------------------
 * 2012/08/23            V1.0          �              ����
 ************************************************************************/
void DbgClearFtpFileRegister(void)
{
    if(ftpFileRegister.empty())
    {
        Debug(SYS_NOTICE,"ftpFileRegister is empty\n");
        return;
    }

    ftpFileRegister.clear();
    return;
}

DEFINE_DEBUG_FUNC(DbgClearFtpFileRegister);


/**********************************************************************
 * �������ƣ�hasSameFile
 * �����������ж��Ƿ�����ͬ���ļ��Ѿ�ע����
 * ���ʵı���
 * �޸ĵı���
 * ���������int64 uid             �û�uid
 string filename    �ļ���
 * ���������
 * �� �� ֵ��   bool �Ƿ�Ϊ��
 * ����˵����
 * �޸�����      �汾��  �޸���      �޸�����
 * ---------------------------------------------------------------------
 * 2012/08/23            V1.0          �              ����
 ************************************************************************/
bool HasSameFileInRegister(int64 uid,const string &filename)
{
    if((0 == uid)||(filename.empty()))
    {
        Debug(SYS_NOTICE,"The parame is error\n");
        return false;
    }

    /*��סftpFileRegisterMutex����ֹ�����߳����ڲ�����Ӱ������Ĳ��Ҳ���*/
    MutexLock(&ftpFileRegisterMutex);
    multimap< int64, string > ::iterator iter =  ftpFileRegister.find(uid);

    if(ftpFileRegister.end() == iter)
    {
        Debug(SYS_NOTICE,"The file has not be exist\n");
        MutexUnLock(&ftpFileRegisterMutex);
        return false;
    }

    multimap< int64, string > ::iterator iter1 = ftpFileRegister.lower_bound(uid);
    multimap< int64, string > ::iterator iter2 = ftpFileRegister.upper_bound(uid);

    for( iter = iter1; iter != iter2; iter++ )
    {
        if(iter->second == filename)
        {
            Debug(SYS_NOTICE,"The file exists\n");
            MutexUnLock(&ftpFileRegisterMutex);
            return true;
        }
    }

    Debug(SYS_NOTICE,"The file has not be exist\n");
    MutexUnLock(&ftpFileRegisterMutex);
    return false;
}

/**********************************************************************
 * �������ƣ�InsertFile2Register
 * ��������������ע���
 * ���ʵı���
 * �޸ĵı���
 * ���������int64 uid             �û�uid
 string filename    �ļ���
 * ���������
 * �� �� ֵ��   bool �Ƿ�Ϊ��
 * ����˵����
 * �޸�����      �汾��  �޸���      �޸�����
 * ---------------------------------------------------------------------
 * 2012/08/23            V1.0          �              ����
 ************************************************************************/
bool InsertFile2Register(int64 uid,const string &filename)
{
    if((0 == uid)||(filename.empty()))
    {
        Debug(SYS_NOTICE,"The parame is error\n");
        return false;
    }

    MutexLock(&ftpFileRegisterMutex);

    if(HasSameFileInRegister(uid,filename))
    {
        Debug(SYS_NOTICE,"The file exists\n");
        MutexUnLock(&ftpFileRegisterMutex);
        return false;
    }

    ftpFileRegister.insert(pair<int64,string>(uid,filename) );
    DbgPrintFtpFileRegister();
    MutexUnLock(&ftpFileRegisterMutex);
    return true;
}

/**********************************************************************
 * �������ƣ�InsertFile2Register
 * �����������˳�ע���
 * ���ʵı���
 * �޸ĵı���
 * ���������int64 uid             �û�uid
 string filename    �ļ���
 * ���������
 * �� �� ֵ��   bool �Ƿ�Ϊ��
 * ����˵����
 * �޸�����      �汾��  �޸���      �޸�����
 * ---------------------------------------------------------------------
 * 2012/08/23            V1.0          �              ����
 ************************************************************************/
bool DelFileFromRegister(int64 uid,const string &filename)
{
    if((0 == uid)||(filename.empty()))
    {
        Debug(SYS_NOTICE,"The parame is error\n");
        return false;
    }

    MutexLock(&ftpFileRegisterMutex);

    if(!HasSameFileInRegister(uid,filename))
    {
        Debug(SYS_NOTICE,"The file not exists\n");
        MutexUnLock(&ftpFileRegisterMutex);
        return false;
    }

    multimap< int64, string > ::iterator iter;
    multimap< int64, string > ::iterator iter1 = ftpFileRegister.lower_bound(uid);
    multimap< int64, string > ::iterator iter2 = ftpFileRegister.upper_bound(uid);

    for( iter = iter1; iter != iter2; iter++ )
    {
        if(iter->second == filename)
        {
            ftpFileRegister.erase(iter);
            Debug(SYS_NOTICE,"Del filename succ\n");
            MutexUnLock(&ftpFileRegisterMutex);
            return true;
        }
    }

    MutexUnLock(&ftpFileRegisterMutex);
    Debug(SYS_NOTICE,"Del filename failed\n");
    return false;

}

/**********************************************************************
 * �������ƣ�CheckFileModPerm
 * ��������������ļ����޸�Ȩ��
 * ���ʵı���
 * �޸ĵı���
 * ���������
 *          pSlot       ��ǰ�Ự��Ϣ
 *          iPathLen    strPath�ĳ���
 *          strFileName �ļ���·��
 *          bWrite      ��д�ļ�����ɾ���ļ�
 * ���������
 *          strPath     �ļ��ľ���·��
 *
 * �� �� ֵ��FALSE       �����Ѿ�ʧЧ,��������Ӧ���˳�
 *           true        ��������
 *
 * ����˵������
 *
 * �޸�����      �汾��    �޸���      �޸�����
 * ---------------------------------------------------------------------
 * 2007/8/30    V1.0    ������      ����
 ************************************************************************/
static int CheckFileModPerm(char *strPath,int iPathLen,
                            FTPD_SESSION_DATA *pSlot,const char *strFileName,int bWrite)
{
    struct  stat        status;
    bool exist = false;
    strPath[0]  = 0;
    if ( !pSlot->bWritable )
    {   /*  û��дȨ�ޣ�ֱ�ӷ���FALSE  */
        return false;
    }

    if ( '/' != *strFileName )
    {
        MkPath(pSlot->curDirName,strFileName,strPath,iPathLen);
    }
    else
    {
        strncpy(strPath,strFileName,iPathLen-1);
    }

    strPath[iPathLen-1]     = 0;
    if(stat(strPath,&status) == 0)
    {
        exist = true;
    }

    if ( bWrite )
    {
        if ( exist )
        {
            /*  ���ɸ���ͬ�����񣬲����ļ��Ѵ���,����FALSE */
            Debug(SYS_WARNING,"file bOverWritable is false@CheckFileModPerm!\n");
            return 2;
        }
    }

    PathSimplify(strPath);

    if ( memcmp(strPath,pSlot->rootDirName,strlen(pSlot->rootDirName)) )
    {
        /*  ����������û�Ŀ¼�£��ǲ�����д    */
        return false;
    }

    if(exist)
    {
        //������������ʹ�ø�ӳ����������
        regular_path(strPath);
        ImagePool* ipool = ImagePool::GetInstance();
        if(!ipool)
        {
            return false;
        }

        int vms = ipool->GetVmsByLocation(strPath);
        if(vms < 0)
        {
            Debug(SYS_WARNING,"get vms of file %s failed!\n",strPath);
            return false;
        }
        else if(vms > 0)
        {
            Debug(SYS_WARNING,"file %s is used by %d vms, cannot be rewrited!\n",strPath,vms);
            return false;
        }
        else
        {
            //���û�������ʹ�ø�ӳ�󣬸���֮ǰҪ�ȴ����ݿ���ɾ��
            Debug(SYS_NOTICE,"file %s is not used,will be rewrited!\n",strPath,vms);
            if(SUCCESS != DeregisterImage(strPath))
            {
                Debug(SYS_WARNING,"file %s deregister failed!\n",strPath);
                return false;
            }
        }
    }

    Debug(SYS_WARNING,"file %s not exist@CheckFileModPerm!\n",strPath);

    return true;
}

/**********************************************************************
 * �������ƣ�static void MkPath(const char *strParent,const char *strSub,char *strOut,int iOutSize)
 * �������������ݸ�Ŀ¼������Ŀ¼������·���ַ�����
 * ���ʵı���
 * �޸ĵı���
 * ���������
 *           strParent  ��Ŀ¼
 *           strSub     ��Ŀ¼
 *           iOutSize   strOut��С
 * ���������strOut     Ŀ¼��·��
 *
 * �� �� ֵ��
 *
 * ����˵������
 *
 * �޸�����      �汾��    �޸���      �޸�����
 * ---------------------------------------------------------------------
 * 2007/8/30    V1.0    ������      ����
 ************************************************************************/
static void MkPath(const char *strParent,const char *strSub,char *strOut,unsigned int iOutSize)
{
    if ( NULL == strOut )
        return ;

    *strOut             = 0;
    strOut[iOutSize-1]  = 0;

    if ( NULL == strParent || 0 == *strParent )
    {
        strParent  = "/";
    }

    if ( NULL == strSub || 0 == *strSub )
    {
        if (strlen(strParent) > (iOutSize -1 ))
        {
            Debug(SYS_ERROR,"The Path Name is too long,will be cut off and the opertion will be fail!\n strParent = %s\n",
                    (SWORDPTR)(strParent),0,0,0);
        }
        strncpy(strOut,strParent,iOutSize-1);
        return ;
    }

    if ( '/' == *strSub )
    {
        if (strlen(strSub) > (iOutSize - 1))
        {
            Debug(SYS_ERROR,"The Path Name is too long,will be cut off and the opertion will be fail!\n strParent = %s, strSub = %s\n",
                    (SWORDPTR)(strParent),(SWORDPTR)(strSub),0,0);
        }
        strncpy(strOut,strSub,iOutSize-1);
    }
    else
    {
        int iLen = strlen(strParent);
        memcpy(strOut,strParent,iLen);


        if ( '/' != strParent[iLen-1] )
        {

            if (iLen+strlen(strSub) > (iOutSize - 2))
            {
                Debug(SYS_ERROR,"The Path Name is too long,will be cut off and the opertion will be fail!\n strParent = %s, strSub = %s\n",
                        (SWORDPTR)(strParent),(SWORDPTR)(strSub),0,0);
            }

            strOut[iLen++]   = '/';

        }
        else
        {
            if (iLen+strlen(strSub) > (iOutSize - 1))
            {
                Debug(SYS_ERROR,"The Path Name is too long,will be cut off and the opertion will be fail!\n strParent = %s, strSub = %s\n",
                        (SWORDPTR)(strParent),(SWORDPTR)(strSub),0,0);
            }

        }

        strncpy(strOut+iLen,strSub,iOutSize - iLen - 1);
    }

}

/**********************************************************************
 * �������ƣ�static int ChDir(char *strCurDir,const char *strNewdir,int iCurDirSize )
 * �����������л���ǰĿ¼����Ŀ¼������ɹ�����Ŀ¼������strCurDirȻ�󷵻�
 * ���ʵı���
 * �޸ĵı���
 * ���������
 *           strCurDir  ��ǰĿ¼
 *           strNewdir  Ŀ��Ŀ¼
 *           iCurDirSize    strCurDir�Ĵ�С
 * ���������strCurDir  �µĵ�ǰĿ¼
 *
 * �� �� ֵ��FALSE      ����,û���л��ɹ�
 *           true       �ɹ�
 *
 * ����˵������
 *
 * �޸�����      �汾��    �޸���      �޸�����
 * ---------------------------------------------------------------------
 * 2007/8/30    V1.0    ������      ����
 ************************************************************************/
static int ChDir(char *strCurDir,const char *strNewdir,int iCurDirSize )
{
    if (  NULL == strNewdir || 0 == *strNewdir )
        return false;


    struct stat     tStatus;
    char            strNewName [OSSSVR_FILEPATH_SIZE];
    strNewName[OSSSVR_FILEPATH_SIZE-1]  = 0;

    if ( '/' == *strNewdir )
    {   /*  ʹ�þ���·��    */
        (void) strncpy (strNewName, strNewdir,OSSSVR_FILEPATH_SIZE-1);/* use the whole thing */
    }
    else
    {   /*  ʹ�����·��    */
        (void) MkPath (strCurDir, strNewdir, strNewName,sizeof(strNewName));
    }

    PathSimplify(strNewName);

    /*  if directory not exist,return directly  */
    if (  0 !=  stat(strNewName,&tStatus)
            || (0 == S_ISDIR(tStatus.st_mode)) )
    {
        return false;
    }

    strncpy(strCurDir,strNewName,iCurDirSize-1);
    strCurDir[iCurDirSize-1]    = 0;


    return true;
}


/**********************************************************************
 * �������ƣ�static int ConnWithTimeout(int sd,const struct sockaddr *tAddr,struct timeval *tv)
 * �������������ӵ������������ҿ������ó�ʱʱ��
 * ���ʵı���
 * �޸ĵı���
 * ���������
 *           sd         ���ӵ�socket
 *           tAddr      Ŀ���ַ
 *           tv         ��ʱ��Ϣ
 * �����������
 *
 * �� �� ֵ��-1         ����,û��������
 *           0          �ɹ�
 *
 * ����˵������
 *
 * �޸�����      �汾��    �޸���      �޸�����
   2010-12-30              qiurh       ʹ��poll����select
 * ---------------------------------------------------------------------
 * 2007/8/30    V1.0    ������      ����
 ************************************************************************/
static int ConnWithTimeout(int sd,
                           const struct sockaddr *tAddr,struct timeval *tv)
{
    long                lArg;
    struct pollfd fds[1];
    int                 timeout;
    int                 iRet;

    int                 iValOpt;
    socklen_t           iLon;

    /* Set non-blocking */
    lArg         =  fcntl(sd, F_GETFL, NULL);
    lArg         |= O_NONBLOCK;
    fcntl(sd, F_SETFL, lArg);

    iRet         = connect(sd, tAddr, sizeof(struct sockaddr_in));

    if ( -1 == iRet )
    {   /*  û�����ӳɹ�����Ҫ�ǲ����ж����ڽ�������    */
        if ( EINPROGRESS == errno )
        {   /*  ����������ӣ���ʹ��poll�ȴ�  */
            timeout = tv->tv_sec*1000+tv->tv_usec/1000;
            fds[0].fd = sd;
            fds[0].events = POLLOUT|POLLWRBAND;
            /*����������ӣ���ʹ��poll �ȴ� */
            if (poll(fds,1,timeout) >0)
            {
                /*�ж�poll �����*/
                iLon = sizeof(int);
                getsockopt(fds[0].fd, SOL_SOCKET, SO_ERROR, (void*)(&iValOpt), &iLon);
                if (0 == iValOpt )
                {
                    iRet = 0;
                }
            }

        }
    }

    lArg = fcntl(sd, F_GETFL, NULL);
    lArg &= (~O_NONBLOCK);
    fcntl(sd, F_SETFL, lArg);

    return iRet;
}


/*========================================================================
 *
 *      FTP ����ʵ�ֺ���
 *      ÿ���������������:
 *          pSlot:      ftp�Ự��Ϣ
 *          strParam:      ftp����Ĳ���
 *
 *      ÿ�������ķ���ֵ��������:
 *          true:       �����ӹ�������
 *          false:      �����ӹ����쳣��Ӧ�ùر�
 *
 *======================================================================*/


/**********************************************************************
 * �������ƣ�Do_User
 * ��������������Ftp��USER����, ���û�����¼���Ự��
 * ���ʵı���
 * �޸ĵı���
 * ���������
 *           pSlot       ��ǰ�Ự��Ϣ
 *           strParam       �û���
 * �����������
 *
 * �� �� ֵ��FALSE       �����Ѿ�ʧЧ,��������Ӧ���˳�
 *           true        ��������
 *
 * ����˵������
 *
 * �޸�����      �汾��    �޸���      �޸�����
 * ---------------------------------------------------------------------
 * 2007/8/30    V1.0    ������      ����
 ************************************************************************/
static int Do_User(FTPD_SESSION_DATA   *pSlot,const char *strParam)
{
    /* check user name */

    /* Actually copy the user name into a buffer and save it */
    /* till the password comes in. Name is located one space */
    /* character after USER string */
    if ( 0 == *strParam )
    {
        pSlot->username[0] = '\0';          /* NOP user for null user */
    }
    else
    {
        strncpy((char *)pSlot->username, strParam, MAX_FTP_USERNAME_LEN);
    }

    pSlot->status &= ~FTPD_USER_OK;

    if ( ERROR == ftpdCmdSend (pSlot, pSlot->cmdSock, 331, s_astrMessages [MSG_PASSWORD_REQUIRED],
                               0, 0, 0, 0, 0, 0) )
    {
        return false;
    }
    else
    {
        return true;
    }
}


/**********************************************************************
 * �������ƣ�Do_Pass
 * ��������������Ftp��PASS����, �����û���֤�������֤�ɹ����»Ự��״̬
 * ���ʵı���
 * �޸ĵı���
 * ���������
 *           pSlot       ��ǰ�Ự��Ϣ
 *strParam       ����
 * �����������
 *
 * �� �� ֵ��FALSE       �����Ѿ�ʧЧ,��������Ӧ���˳�
 *           true        ��������
 *
 * ����˵������
 *
 * �޸�����      �汾��    �޸���      �޸�����
 * ---------------------------------------------------------------------
 * 2007/8/30    V1.0    ������      ����
 ************************************************************************/
static int Do_Pass(FTPD_SESSION_DATA   *pSlot,const char *strParam)
{
    /* check user passwd */

    /* Actually check it against earlier supplied user name */

    /*yindesheng ���*/
    if ( 0 == *strParam )
    {
        pSlot->password[0] = '\0';          /* NOP user for null user */
    }
    else
    {
        strncpy((char *)pSlot->password, strParam, MAX_FTP_PWD_LEN);
    }

    if ( FtpLogin(pSlot) != OK )
    {
        if ( ERROR == ftpdCmdSend (pSlot, pSlot->cmdSock,
                                   530, s_astrMessages [MSG_USER_LOGIN_FAILED],
                                   0, 0, 0, 0, 0, 0) )
        {
            return false;
        }

        pSlot->status &= ~FTPD_USER_OK;

        return true;
    }

    pSlot->status |= FTPD_USER_OK;
    if ( ERROR == ftpdCmdSend (pSlot, pSlot->cmdSock, 230, s_astrMessages [MSG_USER_LOGGED_IN],
                               0, 0, 0, 0, 0, 0) )
    {
        return false;
    }

    return true;
}

/**********************************************************************
 * �������ƣ�Do_Quit
 * ��������������Ftp��QUIT����, �����˳���Ϣ
 * ���ʵı���
 * �޸ĵı���
 * ���������
 *           pSlot       ��ǰ�Ự��Ϣ
 *strParam       ������
 * �����������
 *
 * �� �� ֵ��FALSE       �����Ѿ�ʧЧ,��������Ӧ���˳�
 *
 * ����˵������
 *
 * �޸�����      �汾��    �޸���      �޸�����
 * ---------------------------------------------------------------------
 * 2007/8/30    V1.0    ������      ����
 ************************************************************************/
static int Do_Quit(FTPD_SESSION_DATA   *pSlot,const char *strParam)
{
    /* sayonara */
    ftpdCmdSend (pSlot,pSlot->cmdSock, 221, s_astrMessages [MSG_SEE_YOU_LATER],
                 0, 0, 0, 0, 0, 0);
    return false;
}

/**********************************************************************
 * �������ƣ�Do_Help
 * ��������������Ftp��HELP����, ���ذ�����Ϣ
 * ���ʵı���
 * �޸ĵı���
 * ���������
 *           pSlot       ��ǰ�Ự��Ϣ
 *          strParam       ������
 * �����������
 *
 * �� �� ֵ��FALSE       �����Ѿ�ʧЧ,��������Ӧ���˳�
 *           true        ��������
 *
 * ����˵����
 *
 * �޸�����      �汾��    �޸���      �޸�����
 * ---------------------------------------------------------------------
 * 2007/8/30    V1.0    ������      ����
 ************************************************************************/
static int Do_Help(FTPD_SESSION_DATA   *pSlot,const char *strParam)
{
    /* send list of supported commands with multiple line response */

    if ( ERROR == ftpdCmdSend (pSlot, pSlot->cmdSock, FTPD_MULTI_LINE | 214,
                               s_astrMessages [MSG_COMMAND_LIST_BEGIN], 0, 0, 0, 0, 0, 0)
       )
    {
        return false;
    }

    if (write (pSlot->cmdSock, s_strFtpdCmdList,
               strlen (s_strFtpdCmdList) ) <= 0)
    {
        return false;
    }

    /* this signifies the end of the multiple line response */

    if ( ERROR == ftpdCmdSend (pSlot, pSlot->cmdSock, 214, s_astrMessages [MSG_COMMAND_LIST_END],
                               0, 0, 0, 0, 0, 0) )

    {
        return false;
    }

    return true;
}




/**********************************************************************
 * �������ƣ�Do_FtpList
 * ��������������Ftp��LIST��NLST����, ����ָ��Ŀ¼������
 * ���ʵı���
 * �޸ĵı���
 * ���������
 *           pSlot       ��ǰ�Ự��Ϣ
 *strParam       Ҫ��ʾ��Ŀ¼
 *           Long        �ǲ����Գ���ʽ������̸�ʽֻ��ʾ�ļ���������ʽ�����������
 * �����������
 *
 * �� �� ֵ��FALSE       �����Ѿ�ʧЧ,��������Ӧ���˳�
 *           true        ��������
 *
 * ����˵������Do_List��Do_NLst��������
 *
 * �޸�����      �汾��    �޸���      �޸�����
 * ---------------------------------------------------------------------
 * 2007/8/30    V1.0    ������      ����
 ************************************************************************/
static int Do_FtpList(FTPD_SESSION_DATA   *pSlot,const char *strParam,int Long)
{
    STATUS              iRetVal;
    const char          *strDirName;    /* directory name place holder */
    char                strPath [OSSSVR_FILEPATH_SIZE];
    struct sockaddr_in  addr;
    socklen_t           addrLen;
    int                 port;

    addrLen = sizeof (struct sockaddr);
    getsockname (pSlot->dataSock, (struct sockaddr *)&addr, &addrLen);
    port = ntohs(addr.sin_port);

    /* client wants to list out the contents of a directory */

    /* if no directory specified or "." specified as a directory
     * we use the currently active directory name
     */

    switch ( *strParam )
    {
    case 0:         /*  û��ָ��ʱ��ʾ��ǰĿ¼ */
    case '.':
    case '-':       /*  ���������-,�Ǻ����� */
        strDirName = &pSlot->curDirName [0];
        break;

    case '/':       /*  Ŀ¼�Ծ���·����ʼ����ֱ����ʾ��Ŀ¼ */
        strDirName = strParam;
        break;

    default:        /* Ŀ¼�������·��������Ŀ¼�� */
        MkPath(pSlot->curDirName,strParam,strPath,sizeof(strPath));
        strDirName = strPath;
        break;
    }

    Debug(SYS_DEBUG,"LIST %s\n", (SWORDPTR)strDirName,0,0,0);
    if(false == CheckListPermission(pSlot,strDirName))
    {
        Debug(SYS_DEBUG,"no permission!\n");
        /*����ģʽ�������Ƿ�ɹ����˴���Ҫ����˴����ݶ˿�*/
        if(pSlot->status & FTPD_PASSIVE)
        {
            Debug(SYS_DEBUG,"the data port is %d\n", port);
            FtpDataPortFixedFree(port);
        }
        return false;
    }

    /* get a new data socket connection for the transmission of
     * the directory listing data
     */
    switch (ftpdDataConnGet (pSlot) )
    {
    case ERROR_NORESP:
        /*����ģʽ�������Ƿ�ɹ����˴���Ҫ����˴����ݶ˿�*/
        if(pSlot->status & FTPD_PASSIVE)
        {
            Debug(SYS_DEBUG,"the data port is %d\n", port);
            FtpDataPortFixedFree(port);
        }
        return true;
        break;

    case ERROR:
    {
        if ( ERROR == ftpdCmdSend (pSlot, pSlot->cmdSock,
                                   426, s_astrMessages [MSG_DATA_CONN_ERROR],
                                   0, 0, 0, 0, 0, 0) )
        {
            /*����ģʽ�������Ƿ�ɹ����˴���Ҫ����˴����ݶ˿�*/
            if(pSlot->status & FTPD_PASSIVE)
            {
                Debug(SYS_DEBUG,"the data port is %d\n", port);
                FtpDataPortFixedFree(port);
            }
            return false;
        }
        else
        {
            /*����ģʽ�������Ƿ�ɹ����˴���Ҫ����˴����ݶ˿�*/
            if(pSlot->status & FTPD_PASSIVE)
            {
                Debug(SYS_DEBUG,"the data port is %d\n", port);
                FtpDataPortFixedFree(port);
            }
            return true;
        }
    }
    break;

    default:
        break;
    }

    /* print out the directory contents over the data connection */
    /*record harddisk*/
#if 0
    BSP_AccessFileRec(strDirName);
#endif
    iRetVal = ftpdDirListGet (pSlot,pSlot->dataSock, strDirName,Long);


    if ( ERROR == iRetVal )
    {
        if ( ERROR == ftpdCmdSend (pSlot, pSlot->cmdSock, 550, s_astrMessages [MSG_DIR_ERROR],
                                   0, 0, 0, 0, 0, 0) )
        {
            /*����ģʽ�������Ƿ�ɹ����˴���Ҫ����˴����ݶ˿�*/
            if(pSlot->status & FTPD_PASSIVE)
            {
                Debug(SYS_DEBUG,"the data port is %d\n", port);
                FtpDataPortFixedFree(port);
            }
            return false;
        }
    }
    else
    {
        if ( ERROR == ftpdCmdSend (pSlot, pSlot->cmdSock,
                                   226, s_astrMessages [MSG_TRANS_COMPLETE],
                                   0, 0, 0, 0, 0, 0) )
        {
            /*����ģʽ�������Ƿ�ɹ����˴���Ҫ����˴����ݶ˿�*/
            if(pSlot->status & FTPD_PASSIVE)
            {
                Debug(SYS_DEBUG,"the data port is %d\n", port);
                FtpDataPortFixedFree(port);
            }
            return false;
        }
    }

    /*����ģʽ�������Ƿ�ɹ����˴���Ҫ����˴����ݶ˿�*/
    if(pSlot->status & FTPD_PASSIVE)
    {
        Debug(SYS_DEBUG,"the data port is %d\n", port);
        FtpDataPortFixedFree(port);
    }

    /* free up the data socket */
    ftpdDataSockFree(pSlot,&pSlot->dataSock);
    return true;
}

/**********************************************************************
 * �������ƣ�Do_List
 * ��������������Ftp��LIST����, ����ָ��Ŀ¼������
 * ���ʵı���
 * �޸ĵı���
 * ���������
 *           pSlot       ��ǰ�Ự��Ϣ
 *strParam       Ҫ��ʾ��Ŀ¼
 * �����������
 *
 * �� �� ֵ��FALSE       �����Ѿ�ʧЧ,��������Ӧ���˳�
 *           true        ��������
 *
 * ����˵������
 *
 * �޸�����      �汾��    �޸���      �޸�����
 * ---------------------------------------------------------------------
 * 2007/8/30    V1.0    ������      ����
 ************************************************************************/
static int Do_List(FTPD_SESSION_DATA   *pSlot,const char *strParam)
{
    return Do_FtpList(pSlot,strParam,true);
}

/**********************************************************************
 * �������ƣ�Do_NLst
 * ��������������FtpNLST����, ����ָ��Ŀ¼������
 * ���ʵı���
 * �޸ĵı���
 * ���������
 *           pSlot       ��ǰ�Ự��Ϣ
 *strParam       Ҫ��ʾ��Ŀ¼
 * �����������
 *
 * �� �� ֵ��FALSE       �����Ѿ�ʧЧ,��������Ӧ���˳�
 *           true        ��������
 *
 * ����˵������
 *
 * �޸�����      �汾��    �޸���      �޸�����
 * ---------------------------------------------------------------------
 * 2007/8/30    V1.0    ������      ����
 ************************************************************************/
static int Do_NLst(FTPD_SESSION_DATA   *pSlot,const char *strParam)
{
    return Do_FtpList(pSlot,strParam,false);
}

static int Do_Size(FTPD_SESSION_DATA   *pSlot,const char *strParam)
{
    const char          *strDirName = "";    /* directory name place holder */
    char                strPath [OSSSVR_FILEPATH_SIZE];
    struct  stat        tStatus;
    int                 iRet    = -1;

    /* client wants to list out the contents of a directory */

    /* if no directory specified or "." specified as a directory
     * we use the currently active directory name
     */

    switch ( *strParam )
    {
    case 0:         /*  û��ָ��ʱ��ʾ��ǰĿ¼ */
    case '.':
    case '-':       /*  ���������-,�Ǻ����� */
        break;

    case '/':       /*  Ŀ¼�Ծ���·����ʼ����ֱ����ʾ��Ŀ¼ */
        strDirName = strParam;
        break;

    default:        /* Ŀ¼�������·��������Ŀ¼�� */
        MkPath(pSlot->curDirName,strParam,strPath,sizeof(strPath));
        strDirName = strPath;
        break;
    }

    if ( strDirName && *strDirName )
    {
        iRet        = stat(strDirName,&tStatus);
        Debug(SYS_DEBUG,"SIZE %s ret:%d sz:%d\n", (SWORDPTR)strDirName,iRet,tStatus.st_size,0);
    }

    if ( iRet == 0 )
    {
        sprintf(strPath,"%lu",tStatus.st_size);
        if ( ERROR == ftpdCmdSend (pSlot, pSlot->cmdSock,
                                   213, strPath,
                                   0, 0, 0, 0, 0, 0) )
        {
            return false;
        }

    }
    else
    {
        if ( ERROR == ftpdCmdSend (pSlot, pSlot->cmdSock,
                                   550, s_astrMessages [MSG_FILE_ERROR],
                                   (SWORDPTR)(strParam), 0, 0, 0, 0, 0) )
        {
            return false;
        }
    }

    return true;
}


/**********************************************************************
 * �������ƣ�Do_RetR
 * ��������������RETR����, ���ͻ����ṩ�����ļ�����
 * ���ʵı���
 * �޸ĵı���
 * ���������
 *           pSlot       ��ǰ�Ự��Ϣ
 *strParam       Ҫ���ص��ļ�
 * �����������
 *
 * �� �� ֵ��FALSE       �����Ѿ�ʧЧ,��������Ӧ���˳�
 *           true        ��������
 *
 * ����˵������
 *
 * �޸�����      �汾��    �޸���      �޸�����
 * ---------------------------------------------------------------------
 * 2007/8/30    V1.0    ������      ����
 ************************************************************************/
static int Do_RetR(FTPD_SESSION_DATA   *pSlot,const char *strParam)
{
    int             bRet;

    FILE                *fp;
    const char          *pFileName  = strParam;     /* directory name place holder */
    char                strFilePath [OSSSVR_FILEPATH_SIZE] = {'\0'};
    struct sockaddr_in  addr;
    socklen_t           addrLen;
    int                 port;

    addrLen = sizeof (struct sockaddr);
    getsockname (pSlot->dataSock, (struct sockaddr *)&addr, &addrLen);
    port = ntohs(addr.sin_port);

    /* retrieve a file */
    /* open the file to be sent to the client */

    if ( '/' != *strParam )
    {
        MkPath(pSlot->curDirName,strParam,strFilePath,sizeof(strFilePath));
        pFileName = strFilePath;
    }

    Debug(SYS_DEBUG,"RETR %s\n", (SWORDPTR)pFileName,0,0,0);
    /*record harddisk*/
#if 0
    BSP_AccessFileRec(pFileName);
#endif

    bRet      = true;
    if ( NULL == (fp = fopen (pFileName, "r")) )
    {
        if ( ERROR == ftpdCmdSend (pSlot, pSlot->cmdSock, 550, s_astrMessages [MSG_FILE_ERROR],
                                   (SWORDPTR)(strParam), 0, 0, 0, 0, 0) )
        {
            bRet = false;
        }
    }
    else
    {
        /* ship it away */
        ftpdDataStreamSend (pSlot, fp, (char *)pFileName);
        (void) fclose (fp);
    }

    /*����ģʽ�������Ƿ�ɹ����˴���Ҫ����˴����ݶ˿�*/
    if(pSlot->status & FTPD_PASSIVE)
    {
         Debug(SYS_DEBUG,"the data port is %d\n", port);
         FtpDataPortFixedFree(port);
    }

    return bRet;
}


/**********************************************************************
 * �������ƣ�Do_StoR
 * ��������������STOR����, �ṩ�ļ��ϴ�����
 * ���ʵı���
 * �޸ĵı���
 * ���������
 *           pSlot       ��ǰ�Ự��Ϣ
 *strParam       �ļ���Ŀ��·��
 * �����������
 *
 * �� �� ֵ��FALSE       �����Ѿ�ʧЧ,��������Ӧ���˳�
 *           true        ��������
 *
 * ����˵������
 *
 * �޸�����      �汾��    �޸���      �޸�����
 * ---------------------------------------------------------------------
 * 2007/8/30    V1.0    ������      ����
 ************************************************************************/
static int Do_StoR(FTPD_SESSION_DATA   *pSlot,const char *strParam)
{
    int bRet = -1;
    int iRet1,iRet2 = -1;
    FILE *fp = NULL;
    char strFilePath [OSSSVR_FILEPATH_SIZE] = {'\0'};
    char ftpFileName[OSSSVR_FILEPATH_SIZE] = {'\0'};
    struct sockaddr_in addr;
    socklen_t          addrLen;

    /*�洢�£���һ������*/
    strncpy(ftpFileName,strParam,OSSSVR_FILEPATH_SIZE - 1);
    addrLen = sizeof (struct sockaddr);
    getsockname (pSlot->dataSock, (struct sockaddr *)&addr, &addrLen);
    int port = ntohs(addr.sin_port);

    if ( !InsertFile2Register(pSlot->uid,ftpFileName))
    {
        if ( ERROR == ftpdCmdSend (
                    pSlot, pSlot->cmdSock, 553, s_astrMessages[MSG_PERMISSION_DENY],
                    (SWORDPTR)(&strParam[0]), 0, 0, 0, 0, 0) )
        {
            /*����ģʽ�������Ƿ�ɹ����˴���Ҫ����˴����ݶ˿�*/
            if(pSlot->status & FTPD_PASSIVE)
            {
                Debug(SYS_DEBUG,"the data port is %d\n", port);
                FtpDataPortFixedFree(port);
            }

            DelFileFromRegister(pSlot->uid,ftpFileName);
            return false;
        }

        /*����ģʽ�������Ƿ�ɹ����˴���Ҫ����˴����ݶ˿�*/
        if(pSlot->status & FTPD_PASSIVE)
        {
            Debug(SYS_DEBUG,"the data port is %d\n", port);
            FtpDataPortFixedFree(port);
        }

        DelFileFromRegister(pSlot->uid,ftpFileName);
        return true;
    }

    iRet1 = CheckFileModPerm(strFilePath,sizeof(strFilePath),pSlot,strParam,true);
    if ( true != iRet1)
    {
        if(false == iRet1)
        {
            iRet2 = ftpdCmdSend (
                    pSlot, pSlot->cmdSock, 553, s_astrMessages[MSG_PERMISSION_DENY],
                    (SWORDPTR)(&strParam[0]), 0, 0, 0, 0, 0);
        }
        else
        {
             iRet2 = ftpdCmdSend (
                    pSlot, pSlot->cmdSock, 553, s_astrMessages[MSG_COVER_DENY],
                    (SWORDPTR)(&strParam[0]), 0, 0, 0, 0, 0);
        }
        if ( ERROR ==  iRet2)
        {
            /*����֮���˳�ʱ��Ҫ��ʱ����*/
            DelFileFromRegister(pSlot->uid,ftpFileName);

            /*����ģʽ�������Ƿ�ɹ����˴���Ҫ����˴����ݶ˿�*/
            if(pSlot->status & FTPD_PASSIVE)
            {
                Debug(SYS_DEBUG,"the data port is %d\n", port);
                FtpDataPortFixedFree(port);
            }

            return false;
        }
        DelFileFromRegister(pSlot->uid,ftpFileName);

        /*����ģʽ�������Ƿ�ɹ����˴���Ҫ����˴����ݶ˿�*/
        if(pSlot->status & FTPD_PASSIVE)
        {
            Debug(SYS_DEBUG,"the data port is %d\n", port);
            FtpDataPortFixedFree(port);
        }

        return true;
    }

    Debug(SYS_DEBUG,"STOR %s\n", (SWORDPTR)strFilePath,0,0,0);

    bRet  = true;
    if ((fp = fopen (strFilePath, "w")) == NULL)
    {
        if ( ERROR == ftpdCmdSend (pSlot, pSlot->cmdSock, 553, s_astrMessages[MSG_CREATE_ERROR],
                                   (SWORDPTR)(&strParam[0]), 0, 0, 0, 0, 0) )
        {
            bRet  = false;
        }

        /*����ģʽ�������Ƿ�ɹ����˴���Ҫ����˴����ݶ˿�*/
        if(pSlot->status & FTPD_PASSIVE)
        {
            Debug(SYS_DEBUG,"the data port is %d\n", port);
            FtpDataPortFixedFree(port);
        }

        DelFileFromRegister(pSlot->uid,ftpFileName);
        return bRet;
    }


    /* receive the file */
    STATUS result = ftpdDataStreamReceive (pSlot, fp);

    /*����ģʽ�������Ƿ�ɹ����˴���Ҫ����˴����ݶ˿�*/
    if(pSlot->status & FTPD_PASSIVE)
    {
        Debug(SYS_DEBUG,"the data port is %d\n", port);
        FtpDataPortFixedFree(port);
    }

    (void) fclose (fp);

    if(result != SUCCESS)
    {
        DelFileFromRegister(pSlot->uid,ftpFileName);
        remove(strFilePath);
        return bRet;
    }

    ostringstream oss;
    if(SUCCESS != RegisterImage(pSlot->uid,strFilePath,oss))
    {
        remove(strFilePath);
        //Debug(SYS_ERROR,"%s\n",oss.str().c_str());
    }
    else
    {
        CalcMd5sum(strFilePath);
    }

    DelFileFromRegister(pSlot->uid,ftpFileName);

    return bRet;
}

/**********************************************************************
 * �������ƣ�Do_Dele
 * ��������������DELE����, ɾ��һ���ļ�
 * ���ʵı���
 * �޸ĵı���
 * ���������
 *           pSlot       ��ǰ�Ự��Ϣ
 *strParam       Ҫɾ�����ļ�·��
 * �����������
 *
 * �� �� ֵ��FALSE       �����Ѿ�ʧЧ,��������Ӧ���˳�
 *           true        ��������
 *
 * ����˵������
 *
 * �޸�����      �汾��    �޸���      �޸�����
 * ---------------------------------------------------------------------
 * 2007/8/30    V1.0    ������      ����
 ************************************************************************/
static int Do_Dele(FTPD_SESSION_DATA   *pSlot,const char *strParam)
{
    char                strFilePath [OSSSVR_FILEPATH_SIZE];

    if ( false == CheckFileModPerm(
                strFilePath,sizeof(strFilePath),pSlot,strParam,false) )
    {
        if ( ERROR == ftpdCmdSend (
                    pSlot, pSlot->cmdSock, 553, s_astrMessages[MSG_PERMISSION_DENY],
                    (SWORDPTR)(&strParam[0]), 0, 0, 0, 0, 0) )
        {
            return false;
        }
        return true;
    }

    Debug(SYS_DEBUG,"DELE %s\n", (SWORDPTR)strFilePath,0,0,0);
    /*record harddisk*/
#if 0
    BSP_AccessFileRec(pFileName);
#endif
    if ( OK != remove (strFilePath) )
        {
            if ( ERROR == ftpdCmdSend (pSlot, pSlot->cmdSock, 550, s_astrMessages [MSG_FILE_ERROR],
                                       (SWORDPTR) strFilePath, 0, 0, 0, 0, 0) )
            {
                return false;
            }
        }
    else
    {
        if ( ERROR == ftpdCmdSend (pSlot, pSlot->cmdSock, 250, s_astrMessages [MSG_DELE_OKAY],
                                   0, 0, 0, 0, 0, 0) )
        {
            return false;
        }
    }

    DeregisterImage(strFilePath);
    return true;
}



/**********************************************************************
 * �������ƣ�Do_Cwd
 * ��������������CWD����, �л���ָ��Ŀ¼
 * ���ʵı���
 * �޸ĵı���
 * ���������
 *           pSlot       ��ǰ�Ự��Ϣ
 *strParam       ��Ŀ¼
 * �����������
 *
 * �� �� ֵ��FALSE       �����Ѿ�ʧЧ,��������Ӧ���˳�
 *           true        ��������
 *
 * ����˵������
 *
 * �޸�����      �汾��    �޸���      �޸�����
 * ---------------------------------------------------------------------
 * 2007/8/30    V1.0    ������      ����
 * 2009/06/30            ������     611000013611cd������Ŀ¼��
 *                               ����˻�Ӧ���ε��¿ͻ��˻����쳣
 ************************************************************************/
static  int Do_Cwd(FTPD_SESSION_DATA   *pSlot,const char *strParam)
{
    /* change directory */
    /* there is no default device for the specified directory */

    if ( false == ChDir(pSlot->curDirName,strParam,sizeof(pSlot->curDirName)) )
    {
        if ( ERROR == ftpdCmdSend (pSlot, pSlot->cmdSock,
                                   501, s_astrMessages [MSG_DIR_NOT_PRESENT],
                                   0, 0, 0, 0, 0, 0) )
        {
            return false;
        }
        else
        {
            return true;
        }
    }

    /* notify successful chdir */

    if ( ERROR == ftpdCmdSend (pSlot, pSlot->cmdSock, 250, s_astrMessages [MSG_CHANGED_DIR],
                               (SWORDPTR)pSlot->curDirName, 0, 0, 0, 0, 0) )
    {
        return false;
    }

    return true;
}

/**********************************************************************
 * �������ƣ�Do_Cdup
 * ��������������CDUP����, �л����ϲ�Ŀ¼
 * ���ʵı���
 * �޸ĵı���
 * ���������
 *           pSlot       ��ǰ�Ự��Ϣ
 *strParam       ��
 * �����������
 *
 * �� �� ֵ��FALSE       �����Ѿ�ʧЧ,��������Ӧ���˳�
 *           true        ��������
 *
 * ����˵������
 *
 * �޸�����      �汾��    �޸���      �޸�����
 * ---------------------------------------------------------------------
 * 2007/8/30    V1.0    ������      ����
 ************************************************************************/
static  int Do_Cdup(FTPD_SESSION_DATA   *pSlot,const char *strParam)
{
    return Do_Cwd(pSlot,"..");
}

/**********************************************************************
 * �������ƣ�Do_Pwd
 * ��������������PWD����, ���ص�ǰĿ¼
 * ���ʵı���
 * �޸ĵı���
 * ���������
 *           pSlot       ��ǰ�Ự��Ϣ
 *strParam       ��
 * �����������
 *
 * �� �� ֵ��FALSE       �����Ѿ�ʧЧ,��������Ӧ���˳�
 *           true        ��������
 *
 * ����˵������
 *
 * �޸�����      �汾��    �޸���      �޸�����
 * ---------------------------------------------------------------------
 * 2007/8/30    V1.0    ������      ����
 ************************************************************************/
static int Do_Pwd(FTPD_SESSION_DATA   *pSlot,const char *strParam)
{
    /* get current working directory */
    if ( ERROR == ftpdCmdSend (pSlot, pSlot->cmdSock, 257, s_astrMessages [MSG_CUR_DIR],
                               (SWORDPTR)pSlot->curDirName, 0, 0, 0, 0, 0) )
    {
        return false;
    }
    else
    {
        return true;
    }
}


/**********************************************************************
 * �������ƣ�Do_Type
 * ��������������TYPE����, �л��ļ����͵�ģʽ(�ı�/������),Ŀǰֻ֧�ֶ�����ģʽ
 * ���ʵı���
 * �޸ĵı���
 * ���������
 *           pSlot       ��ǰ�Ự��Ϣ
 *strParam       ��ģʽ
 * �����������
 *
 * �� �� ֵ��FALSE       �����Ѿ�ʧЧ,��������Ӧ���˳�
 *           true        ��������
 *
 * ����˵������
 *
 * �޸�����      �汾��    �޸���      �޸�����
 * ---------------------------------------------------------------------
 * 2007/8/30    V1.0    ������      ����
 ************************************************************************/
static  int Do_Type(FTPD_SESSION_DATA   *pSlot,const char *strParam)
{
    /* we only support BINARY and ASCII representation types */
    switch ( *strParam )
    {
    case 'I':
    case 'i':
    case 'L':
    case 'l':
        pSlot->status |= FTPD_BINARY_TYPE;
        pSlot->status &= ~FTPD_ASCII_TYPE;
        if ( ERROR == ftpdCmdSend (pSlot, pSlot->cmdSock, 200, s_astrMessages [MSG_TYPE_BINARY],
                                   0, 0, 0, 0, 0, 0) )
        {
            return false;
        }
        break;

        /*
         * ��֧���ı�ģʽ
         case 'A':
         case 'a':
         pSlot->status |= FTPD_ASCII_TYPE;
         pSlot->status &= ~FTPD_BINARY_TYPE;
         if( ERROR == ftpdCmdSend (pSlot, pSlot->cmdSock, 200, s_astrMessages [MSG_TYPE_ASCII],
         0, 0, 0, 0, 0, 0) )
         return false;
         break;
         */

    default:
        if ( ERROR == ftpdCmdSend (pSlot, pSlot->cmdSock, 504, s_astrMessages [MSG_PARAM_BAD],
                                   0, 0, 0, 0, 0, 0) )
        {
            return false;
        }
        break;
    }

    return true;
}

/**********************************************************************
 * �������ƣ�Do_StrU
 * ��������������STRU����, �����ļ��ṹ,ֻ֧������ʽ���ļ����͡�
 * ���ʵı���
 * �޸ĵı���
 * ���������
 *           pSlot       ��ǰ�Ự��Ϣ
 *strParam       �²���
 * �����������
 *
 * �� �� ֵ��FALSE       �����Ѿ�ʧЧ,��������Ӧ���˳�
 *           true        ��������
 *
 * ����˵������
 *
 * �޸�����      �汾��    �޸���      �޸�����
 * ---------------------------------------------------------------------
 * 2007/8/30    V1.0    ������      ����
 ************************************************************************/
static int Do_StrU(FTPD_SESSION_DATA   *pSlot,const char *strParam)
{
    /* specify the file structure */

    /* we only support normal byte stream oriented files;
     * we don't support IBM-ish record block oriented files
     */

    if ( ('F' == *strParam) || ('f' == *strParam) )
    {
        if ( ERROR == ftpdCmdSend (pSlot, pSlot->cmdSock, 200, s_astrMessages [MSG_FILE_STRU],
                                   0, 0, 0, 0, 0, 0) )
        {
            return false;
        }
    }
    else
    {
        if ( ERROR == ftpdCmdSend (pSlot, pSlot->cmdSock, 504, s_astrMessages [MSG_PARAM_BAD],
                                   0, 0, 0, 0, 0, 0) )
        {
            return false;
        }
    }

    return true;
}

/**********************************************************************
 * �������ƣ�Do_Mode
 * ��������������Mode����, ���ô���ģʽ,ֻ֧������ʽ
 * ���ʵı���
 * �޸ĵı���
 * ���������
 *           pSlot       ��ǰ�Ự��Ϣ
 *strParam       �²���
 * �����������
 *
 * �� �� ֵ��FALSE       �����Ѿ�ʧЧ,��������Ӧ���˳�
 *           true        ��������
 *
 * ����˵������
 *
 * �޸�����      �汾��    �޸���      �޸�����
 * ---------------------------------------------------------------------
 * 2007/8/30    V1.0    ������      ����
 ************************************************************************/
static int Do_Mode(FTPD_SESSION_DATA   *pSlot,const char *strParam)
{
    /* specify transfer mode */

    /* we only support stream mode -- no block or compressed mode */

    if ( ('S' == *strParam) || ( 's' == *strParam) )
    {
        if ( ERROR == ftpdCmdSend (pSlot, pSlot->cmdSock, 200, s_astrMessages [MSG_STREAM_MODE],
                                   0, 0, 0, 0, 0, 0) )
        {
            return false;
        }
    }
    else
    {
        if ( ERROR == ftpdCmdSend (pSlot, pSlot->cmdSock, 504, s_astrMessages [MSG_PARAM_BAD],
                                   0, 0, 0, 0, 0, 0) )
        {
            return false;
        }
    }

    return true;
}


/**********************************************************************
 * �������ƣ�Do_Port
 * ��������������PORT����, ��������ģʽ(�������ӶԷ�)
 * ���ʵı���
 * �޸ĵı���
 * ���������
 *           pSlot       ��ǰ�Ự��Ϣ
 *strParam       �ͻ��˵ļ�����Ϣ
 * �����������
 *
 * �� �� ֵ��FALSE       �����Ѿ�ʧЧ,��������Ӧ���˳�
 *           true        ��������
 *
 * ����˵������
 *
 * �޸�����      �汾��    �޸���      �޸�����
 * ---------------------------------------------------------------------
 * 2007/8/30    V1.0    ������      ����
 ************************************************************************/
static  int Do_Port(FTPD_SESSION_DATA   *pSlot,const char *strParam)
{
    int                 iPortNum [6];   /* used for "%d,%d,%d,%d,%d,%d" */
    WORD32              dwValue;

    /* client specifies the port to be used in setting up
     * active data connections later on (see ftpdDataConnGet ()).
     * format:  first four decimal digits separated by commas
     * indicate the internet address; the last two decimal
     * digits separated by a comma represents hi and low
     * bytes of a port number.
     */

    (void) sscanf (strParam, "%d,%d,%d,%d,%d,%d",
                   &iPortNum [0], &iPortNum [1], &iPortNum [2],
                   &iPortNum [3], &iPortNum [4], &iPortNum [5]);

    pSlot->dataAddr.sin_port = iPortNum [4] * 256 + iPortNum [5];

    /* convert port number to network byte order */

    pSlot->dataAddr.sin_port = htons (pSlot->dataAddr.sin_port);

    /* Set remote host to given dwValue. */

    dwValue = (iPortNum [0] << 24) | (iPortNum [1] << 16) |
              (iPortNum [2] << 8) | iPortNum [3];
    pSlot->dataAddr.sin_addr.s_addr = htonl (dwValue);

    if ( ERROR == ftpdCmdSend (pSlot, pSlot->cmdSock, 200, s_astrMessages [MSG_PORT_SET],
                               0, 0, 0, 0, 0, 0) )
    {
        return false;
    }
    else
    {
        return true;
    }
}

/**********************************************************************
 * �������ƣ�Do_Pasv
 * ��������������PASV����, ���뱻��ģʽ,�����ظ��Է����˼�����IP��˿ں�
 * ���ʵı���
 * �޸ĵı���
 * ���������
 *           pSlot       ��ǰ�Ự��Ϣ
 *strParam       ��
 * �����������
 *
 * �� �� ֵ��FALSE       �����Ѿ�ʧЧ,��������Ӧ���˳�
 *           true        ��������
 *
 * ����˵������
 *
 * �޸�����      �汾��    �޸���      �޸�����
 * ---------------------------------------------------------------------
 * 2007/8/30    V1.0    ������      ����
 ************************************************************************/
static int Do_Pasv(FTPD_SESSION_DATA   *pSlot,const char *strParam)
{
    /* client wants to connect to us instead of waiting
     * for us to make a connection to its data connection
     * socket
     */
    WORD32      dwAddr;
    WORD16      wPort;
    socklen_t      dwAddrLen     = sizeof (struct sockaddr_in);
    int         datafixedport = 0;

    ftpdSockFree (&pSlot->dataSock);

    /* we need to open a socket and start listening on it
     * to accommodate his request.
     */

    if ((pSlot->dataSock = socket (AF_INET, SOCK_STREAM, 0)) < 0)
    {
        /*  �������socketʧ�ܣ���ֱ�ӷ��� */
        if ( ERROR == ftpdCmdSend (pSlot, pSlot->cmdSock, 425, s_astrMessages [MSG_PASSIVE_ERROR],
                                   0, 0, 0, 0, 0, 0) )
        {
            return false;
        }
        else
        {
            return true;
        }
    }


    /*  �������socket�ɹ����ǿ�ʼ��һ�����ض˿ڣ�Ȼ��Ѱ󶨵���Ϣ�����Է� */

    if (getsockname (pSlot->cmdSock,(struct sockaddr *) &pSlot->dataAddr,&dwAddrLen) < 0)
    {
        ftpdSockFree (&pSlot->dataSock);

        /* Couldn't find address for local end of connection. */
        if ( ERROR == ftpdCmdSend (pSlot, pSlot->cmdSock,
                                   425, s_astrMessages [MSG_PASSIVE_ERROR],
                                   0, 0, 0, 0, 0, 0) )
        {
            return false;
        }
        else
        {
            return true;
        }
    }

    /*
     * Find an ephemeral port for the expected connection
     * and initialize connection queue.
     */

    dwAddrLen                   = sizeof (struct sockaddr_in);

    datafixedport               = FtpDataPortFixedGet();

    if(-1 == datafixedport)
    {
        return false;
    }

    Debug(SYS_DEBUG,"datafixedport:%d \n", datafixedport);
    pSlot->dataAddr.sin_port    = htons(datafixedport);

    Debug(SYS_DEBUG,"pSlot->dataAddr.sin_port:%d \n", pSlot->dataAddr.sin_port);

    if (bind (pSlot->dataSock, (struct sockaddr *)&pSlot->dataAddr,
              sizeof (struct sockaddr_in)) < 0 ||
            getsockname (pSlot->dataSock,
                         (struct sockaddr *) &pSlot->dataAddr,
                         &dwAddrLen) < 0 ||
            listen (pSlot->dataSock, 1) < 0)
    {
        ftpdSockFree (&pSlot->dataSock);

        if ( ERROR == ftpdCmdSend (pSlot, pSlot->cmdSock,
                                   425, s_astrMessages [MSG_PASSIVE_ERROR],
                                   0, 0, 0, 0, 0, 0) )
        {
            FtpDataPortFixedFree(datafixedport);
            return false;
        }
        else
        {
            FtpDataPortFixedFree(datafixedport);
            return true;
        }
    }

    /* we're passive, let us keep that in mind */

    pSlot->status |= FTPD_PASSIVE;

    dwAddr          = htonl(pSlot->dataAddr.sin_addr.s_addr);
    wPort           = htons(pSlot->dataAddr.sin_port);

    /* tell the client to which port to connect */

    if ( ERROR == ftpdCmdSend (pSlot, pSlot->cmdSock,
                               227, s_astrMessages [MSG_PASSIVE_MODE],
                               ( dwAddr & 0xff000000) >> 24,
                               ( dwAddr & 0x00ff0000) >> 16,
                               ( dwAddr & 0x0000ff00) >>  8,
                               ( dwAddr & 0x000000ff),
                               ( wPort & 0xff00) >> 8,
                               ( wPort & 0x00ff) ) )
    {
        FtpDataPortFixedFree(datafixedport);
        return false;
    }
    else
    {
        return true;
    }
}



/**********************************************************************
 * �������ƣ�Do_Allo
 * ��������������Allo����, ����һ�����ڵ�����
 * ���ʵı���
 * �޸ĵı���
 * ���������
 *           pSlot       ��ǰ�Ự��Ϣ
 *strParam       �²���
 * �����������
 *
 * �� �� ֵ��FALSE       �����Ѿ�ʧЧ,��������Ӧ���˳�
 *           true        ��������
 *
 * ����˵������
 *
 * �޸�����      �汾��    �޸���      �޸�����
 * ---------------------------------------------------------------------
 * 2007/8/30    V1.0    ������      ����
 ************************************************************************/
static int Do_Allo(FTPD_SESSION_DATA   *pSlot,const char *strParam)
{
    /* allocate and account commands are not need */

    if ( ERROR == ftpdCmdSend (pSlot, pSlot->cmdSock, 202, s_astrMessages [MSG_ALLOC_ACCOUNT],
                               0, 0, 0, 0, 0, 0) )
    {
        return false;
    }
    else
    {
        return true;
    }
}

/**********************************************************************
 * �������ƣ�Do_Noop
 * ��������������Noop����, �ղ���
 * ���ʵı���
 * �޸ĵı���
 * ���������
 *           pSlot       ��ǰ�Ự��Ϣ
 *strParam       ��
 * �����������
 *
 * �� �� ֵ��FALSE       �����Ѿ�ʧЧ,��������Ӧ���˳�
 *           true        ��������
 *
 * ����˵������
 *
 * �޸�����      �汾��    �޸���      �޸�����
 * ---------------------------------------------------------------------
 * 2007/8/30    V1.0    ������      ����
 ************************************************************************/
static int Do_Noop(FTPD_SESSION_DATA   *pSlot,const char *strParam)
{
    /* don't do anything */
    if ( ERROR == ftpdCmdSend (pSlot, pSlot->cmdSock, 200, s_astrMessages [MSG_NOOP_OKAY],
                               0, 0, 0, 0, 0, 0) )
    {
        return false;
    }
    else
    {
        return true;
    }
}

/*
 *  �������˳�ʱ���ر�����socket,
 */
static void ExitClean()
{
    FTPD_SESSION_DATA   *pData;
    if ( true == s_bFtpsActive )
    {
        close(s_hftpdServSd);
        s_hftpdServSd    = SOCKET_ERROR;
        MutexLock(&s_hFtpSessMutex);
        oss_list_for_each_entry(pData,&s_atFtpSessions,node)
        {
            ftpdSockFree (&pData->cmdSock);
            ftpdSockFree (&pData->dataSock);
        }
        MutexUnLock(&s_hFtpSessMutex);
    }
    s_bFtpsShutdownFlag     = true;
}


/*******************************************************************************
 *
 * ftpdInit - initialize the FTP server task
 *
 * This routine installs the password verification routine indicated by
 * <pLoginRtn> and establishes a control connection for the primary FTP
 * server task, which it then creates. It is called automatically during
 * system startup if INCLUDE_FTP_SERVER is defined. The primary server task
 * supports simultaneous client sessions, up to the limit specified by the
 * global variable 'ftpsMaxClients'. The default value allows a maximum of
 * four simultaneous connections. The <stackSize> argument specifies the stack
 * size for the primary server task. It is set to the value specified in the
 * 'FTP_TSKSTACK_SIZE' global variable by default.
 *
 * RETURNS:
 * OK if server started, or ERROR otherwise.
 *
 * ERRNO: N/A
 */

/**********************************************************************
 * �������ƣ�StartFtpServer
 * ��������������FTP����
 * ���ʵı���
 * �޸ĵı���
 * ���������
 *          pLoginRtn:      ��֤�ص�������ΪNULLʱ��ʾ����Ҫ��֤
 *          strDefPath:     Ĭ��Ŀ¼
 *          iClientMax:     ��󲢷�������
 *          strBindIP:      ������ַ��NULL��������е�ַ
 *          iSvrTskPrio:    FTP������������ȼ�
 *          iWorkerTskPrio: FTP������������ȼ�
 * ���������
 * �� �� ֵ��
 *          SUCCESS:            �ɹ�
 *          ERR_OSSSVR_PARAM_ERROR: ����������
 *          ERR_OSSSVR_SOCKET:      ����socket����
 *          ERR_OSSSVR_THREAD:      �����̳߳���
 * ����˵����
 * �޸�����      �汾��  �޸���      �޸�����
 * ---------------------------------------------------------------------
 * 2008/9/10     V1.0    ������      ����
 ************************************************************************/
STATUS StartFtpServer(const string& strDefPath,const string& strBindIP)
{
    int bOK = false;
    int iOn = 1;    /*  SO_REUSEADDR ���� */
    struct sockaddr_in tCtrlAddr;
    if ( s_bFtpsActive )
    {
        return SUCCESS;
    }

    /* �������в��� */
    if ( s_iFtpMaxClients > FTP_MAX_CLIENTS )
    {
        s_iFtpMaxClients = FTP_MAX_CLIENTS;
    }
    else
    {
        if ( s_iFtpMaxClients < FTP_MIN_CLIENTS )
            s_iFtpMaxClients = FTP_MIN_CLIENTS;
    }

    if (strDefPath.empty())
        return ERR_OSSSVR_PARAM_ERROR;

    if(strDefPath[0] != '/' )
        return ERR_OSSSVR_PARAM_ERROR;

    s_strDefRootPath = strDefPath;

    s_iFtpsCurClients = 0;

    /* �������� */
    Debug(SYS_INFORMATIONAL,"Starting ftpd on interface %s with directory %s ...\n",
        strBindIP.c_str(),s_strDefRootPath.c_str());
    s_hftpdServSd = socket (AF_INET, SOCK_STREAM, 0);
    if (s_hftpdServSd < 0)
    {
        return ERR_OSSSVR_SOCKET;
    }

    /* Setup control connection for client requests. */

    tCtrlAddr.sin_family = AF_INET;
    tCtrlAddr.sin_port = htons (s_ftp_daemon_port);
    if (strBindIP.empty() || strBindIP == "*")
    {
        tCtrlAddr.sin_addr.s_addr = INADDR_ANY;
    }
    else
    {
        tCtrlAddr.sin_addr.s_addr = inet_addr(strBindIP.c_str());
    }

    bOK = false;
    if (0 != setsockopt (s_hftpdServSd, SOL_SOCKET, SO_REUSEADDR,
                          (char *) &iOn, sizeof (int)))
    {
        Debug(SYS_WARNING,"reuse ftp port fail,ret:%d\n",errno);
    }
    else
    {
        if (0 != bind (s_hftpdServSd, (struct sockaddr *) &tCtrlAddr,
                        sizeof (tCtrlAddr)))
        {
            Debug(SYS_ERROR,"bind ftp port fail,ret:%d\n",errno);
        }
        else
        {
            if (0 != listen (s_hftpdServSd, 5))
            {
                Debug(SYS_ERROR,"listen ftp port fail,ret:%d\n",errno);
            }
            else
            {
                bOK = true;
            }
        }
    }

    if (false == bOK)
    {
        close(s_hftpdServSd);
        s_hftpdServSd = SOCKET_ERROR;
        return ERR_OSSSVR_SOCKET;
    }

    /* Create a FTP server task to receive client requests. */

    /* ��ʼ�� ftp ģ��ʹ�õ����ݽṹ */
    /*  �ȳ�ʼ������������� */
    XOS_INIT_LIST_HEAD (&s_atFtpSessions);

    /*  �ٴ������������ */
    if (MutexInit(&s_hFtpSessMutex))
    {
        /* ������������ */
        if(INVALID_THREAD_ID != StartThread("ftpdTask",(ThreadEntry)ftpdTask,NULL))
        {
            /*  ���񴴽��ɹ����������б�־ΪTRUE */
            s_bFtpsActive = true;
            Debug(SYS_DEBUG,"ftpdTask created!\n",0,0,0,0);
        }

        if (false == s_bFtpsActive)
        {
            MutexClose(&s_hFtpSessMutex);
        }
    }

    /*  �����������һ�����ɹ����ر����� */
    if (false == s_bFtpsActive)
    {
        close(s_hftpdServSd);
        s_hftpdServSd = SOCKET_ERROR;

        return ERR_OSSSVR_THREAD;
    }

    /*  �ڳ���ر�ʱ���ر�ftp���Ӻͼ����˿� */
    atexit(ExitClean);
    return SUCCESS;
}

STATUS EnableFtpdOptions()
{
    TecsConfig *config = TecsConfig::Get();
    //����Config �����ļ���TC ��FTP �����������ݶ˿�
    config->EnableCustom("ftp_server_url",s_ftp_server_ip,"The url of ftp server, default = *.");
    //����Config �����ļ���TC ��FTP ���������ػ��˿�
    config->EnableCustom("ftp_daemon_port",s_ftp_daemon_port,"The daemon port of ftp server, default = 21.");
    //����Config �����ļ���TC ��FTP �����������ݶ˿�
    config->EnableCustom("ftp_data_port",s_ftp_data_port,"The data port of ftp server, default = 20.");
    //����ftp ��������̸߳���
    config->EnableCustom("ftp_max_clients",s_iFtpMaxClients,"The max clients of ftp,   default = 5.");
    //����ftp����ģʽ�����ݶ˿���ֹ��Χ
    config->EnableCustom("ftp_start_port",s_ftp_data_port_begin,"The start of dynamic data port, default = 9001.");
    config->EnableCustom("ftp_end_port",s_ftp_data_port_end,"The end of dynamic data port, default = 9010.");

    return SUCCESS;
}

/* ���ĳ���Ự�Ƿ���Ȩ��listĳ��·�� */
bool CheckListPermission(FTPD_SESSION_DATA *pSlot,const string& list_dir)
{
    string root_dir(pSlot->rootDirName);
    regular_path(root_dir);

    if(0 != list_dir.compare(0,root_dir.length(),root_dir))
    {
        return false;
    }
    return true;
}

/**********************************************************************
 * �������ƣ�static STATUS ftpLogin(BYTE *ftpUsername, BYTE *ftpPwd,
 FTPD_SESSION_DATA *pSlot)
 * �����������û���֤��ں���
 * ���ʵı���
 * �޸ĵı���
 * ���������
 FTPD_SESSION_DATA   *pSlot   :��ǰ�Ự�Ľṹָ��
 * �����������
 * �� �� ֵ��
OK:        �ɹ�
ERROR:  ʧ��
 * ����˵������
 * �޸�����        �汾��     �޸���          �޸�����
 * -----------------------------------------------
 *
 ************************************************************************/
static STATUS FtpLogin(FTPD_SESSION_DATA *pSlot)
{
    string session;
    const string magic_user = MAGIC_USER;

    //�û���Ϊ�գ�ֱ�ӷ�����֤ʧ��
    if(!*(pSlot->username))
    {
        Debug(SYS_NOTICE,"FtpLogin failed! no username!");
        return ERROR;
    }

    if(magic_user == pSlot->username)
    {
        string name, pass;
        string secret = UserPool::ExternGetSecretBySession((char *)pSlot->password);
        Debug(SYS_DEBUG,"user's secret is %s.\n", secret.c_str());
        session = secret;
        User::SplitSecret(session, name, pass);
        strncpy(pSlot->username, name.c_str(), sizeof(pSlot->username));
    }
    else
    {
        //������֤����
        session.assign((char *)pSlot->username);
        session.append(":");
        //session.append((char *)pSlot->password);
        session.append(Sha1Digest((char *)pSlot->username, (char *)pSlot->password));
    }

#if 1
    /*���ȵ���TECS �еļ�Ȩ���������û��������Ȩ*/
    pSlot->uid = UserPool::ExternAuthenticate(session);
    if(-1 == pSlot->uid)
    {
        Debug(SYS_NOTICE,"ftpd call ExternAuthenticate failed! username=%s, password=%s, session=%s\n",
            pSlot->username,pSlot->password,session.c_str());
        pSlot->rootDirName[0] = 0;
        return ERROR;
    }
#else
    //��������ʽ��ݼ�������ʱ��ʩ!!!
    pSlot->uid = 123;
#endif

    //���ø��û���rootĿ¼Ϊs_strDefRootPath + /username
    strncpy(pSlot->rootDirName,s_strDefRootPath.c_str(),sizeof(pSlot->rootDirName)-1);
    strncat(pSlot->rootDirName,"/",sizeof(pSlot->rootDirName) - strlen(pSlot->rootDirName));
    strncat(pSlot->rootDirName,pSlot->username,sizeof(pSlot->rootDirName) - strlen(pSlot->rootDirName));
    //���õ�ǰĿ¼ΪrootĿ¼
    pSlot->rootDirName[sizeof(pSlot->rootDirName) - 1] = 0;
    memcpy(pSlot->curDirName,pSlot->rootDirName,sizeof(pSlot->curDirName));

    //rootĿ¼�����ڵĻ��ʹ���һ��
    if(access(pSlot->rootDirName, 0) != 0 && 0 != mkdir(pSlot->rootDirName,0600))
    {
        Debug(SYS_ERROR,"failed to make home dir %s! errno = \n",pSlot->rootDirName,errno);
        return ERROR;
    }

    //Ϊ�û�����Ŀ¼����Ȩ��
    pSlot->bWritable = true;
    pSlot->bOverWritable = true;
    pSlot->iStreamThrashold = 100;
    Debug(SYS_DEBUG,
            "%s[uid=%lld] login,root:%s write:%d overwrite:%d speed:%d KB/s\n",
            pSlot->username,pSlot->uid,pSlot->rootDirName,
            pSlot->bWritable,pSlot->bOverWritable,pSlot->iStreamThrashold);
    return SUCCESS;
}

STATUS GetFileInfo(const string&file,int64& size,string& checksum)
{
    if(access(file.c_str(), F_OK) != 0)
    {
        Debug(SYS_ERROR,"file %s not exist! errno = %d\n",file.c_str(),errno);
        return ERROR_FILE_NOT_EXIST;
    }

    struct stat64 s;
    if (0 != stat64(file.c_str(),&s))
    {
        Debug(SYS_ERROR,"stat64 file %s failed! errno = %d\n",file.c_str(),errno);
        return ERROR;
    }
    size = s.st_size;
#if 0
    Debug(SYS_DEBUG,"calculating md5sum of file %s ...\n",file.c_str());
    STATUS ret = GetFileMd5sum(file,checksum);
    Debug(SYS_DEBUG,"md5sum of file %s is: %s\n",file.c_str(),checksum.c_str());
    return ret;
#endif
    return SUCCESS;
}

static STATUS RegisterImage(int64 uid,const string& file, ostringstream& result)
{
    string format;
    int64 disk_size = 0;
    int64 size = 0;
    string checksum;
    STATUS ret = GetFileInfo(file,size,checksum);
    if(SUCCESS != ret)
    {
        result << "failed to get file info!" << endl;
        return ret;
    }

    if(ValidateLicense(LICOBJ_IMAGES, 1) != 0)
    {
        result << "license is invalid!" << endl;
        return ERROR;
    }

    //�Ӵ洢��˻�ȡ�ļ��ϴ���Ĵ��·��
    ImageStore* store = ImageStore::GetInstance();
    FileUrl url;
    url.checksum = checksum;
    ret = store->GetUrlByPath(file,url);
    if(SUCCESS != ret)
    {
        return ret;
    }

    Debug(SYS_NOTICE,"ftpd will register url:%s,%s,%s\n",
        url.path.c_str(),
        url.access_type.c_str(),
        url.access_option.c_str());

    Image image(uid,INVALID_OID);
    //image���ֱ��������֣���ĸ���»������
    string image_name(file.substr(file.find_last_of('/')+1));
    for(int i = 0; i < (int)image_name.size(); i++)
    {
        if(!VALID_NORMAL_NAME_CHAR(image_name.at(i)))
        {
            image_name = image_name.substr(0,i);
            break;
        }
    }

    if(image_name.empty())
    {
        image_name = "noname";
    }

    // img��nameֻ��32���ֽڣ�ֻȡǰ32���ֽ�
    if(image_name.length()>MAX_IMAGE_NAME_LENGTH)
    {
        image_name = image_name.substr(0,MAX_IMAGE_NAME_LENGTH-1);
    }

    if(SUCCESS != CheckImgFormatAndSize(file,format,disk_size))
    {
        format = IMAGE_FORMAT_RAW;
        disk_size = size;
    }

    image.set_disk_format(format);
    image.set_disk_size(disk_size);

    image.set_name(image_name);
    image.set_size(size);
    image.set_file_url(url);
    image.set_location(file);
    image.set_public(false);
    image.set_bus(DISK_BUS_IDE);
    image.set_type(IMAGE_TYPE_MACHINE);
    image.set_arch(IMAGE_ARCH_X86_64);
    image.set_enable(true);
    Datetime dt;
    dt.refresh();
    image.set_register_time(dt.serialize());
    ImagePool* ipool = ImagePool::GetInstance();
    if(!ipool)
    {
        result << "ImagePool not ready!" << endl;
        return ERROR_NOT_READY;
    }

    string error;
    ret = ipool->Allocate(image,error);
    if(0 != ret)
    {
        remove(file.c_str());
        Debug(SYS_ERROR,"failed to register file %s, error string: %s\n",
            file.c_str(),error.c_str());
        result << error;
        return ERROR_DB_INSERT_FAIL;
    }

    result << "file " << file << " is registered as image " << image_name
           << ", id = " << image.get_iid()
           << ", size = " << size
           << ", md5sum = " << checksum << endl;
    Debug(SYS_NOTICE,"%s\n",result.str().c_str());
    return SUCCESS;
}

static STATUS DeregisterImage(const string& file)
{
    //�Ӵ洢��˻�ȡ�ļ��ϴ���Ĵ��·��
    ImageStore* store = ImageStore::GetInstance();
    FileUrl url;
    int rc;

    store->GetUrlByPath(file,url);
    Debug(SYS_NOTICE,"ftpd will deregister url:%s,%s,%s\n",
        url.path.c_str(),
        url.access_type.c_str(),
        url.access_option.c_str());

    ImagePool* ipool = ImagePool::GetInstance();
    if(!ipool)
    {
        return ERROR_NOT_READY;
    }

    int64 iid = ipool->GetImageIdByLocation(file);
    if(iid == INVALID_OID)
    {
        Debug(SYS_ERROR,"DeregisterImage: file %s error!\n",url.path.c_str());
        return ERROR;
    }
    else if(0 == iid)
    {
        Debug(SYS_NOTICE,"DeregisterImage: file %s not registered!\n",url.path.c_str());
        return SUCCESS;
    }

    Image image(INVALID_OID);

    int ret = ipool->GetImageByImageId(iid,image);

    if(SUCCESS == ret)
    {
        /*�����������е���������߱�ģ������*/
        if((image.get_running_vms() > 0)||(image.get_ref_count() > 0))
        {
            Debug(SYS_ERROR,"file %s is refence by others!\n",url.path.c_str());
            return ERROR;
        }

        rc = ipool->Delete(image.get_iid());

        /*ִ�����ݿ����ʧ��*/
        if(0 != rc)
        {
            Debug(SYS_ERROR,"file %s delete error!\n",url.path.c_str());
            return ERROR_DB_DELETE_FAIL;
        }
    }

    return SUCCESS;
}


STATUS GetUpLoadUrl(UploadUrl &url)
{
    ImageStore* store = ImageStore::GetInstance();
    if(!store)
    {
        SkyAssert(false);
        return ERROR_NOT_READY;
    }

    url.access_type = IMAGE_ACCESS_FTP;
    store->GetUploadIp(url.ip);
    url.port = s_ftp_daemon_port;
    return SUCCESS;
}

STATUS GetFtpSvrInfo(int& port,string& ip)
{
    /*���ftp����˲�����*/
    if(!s_bFtpsActive)
    {
        return ERROR;
    }

    /*ֱ�ӷ������õ�ip���������Ϊ*����web�����⴦��ʹ��Apache��ͬ��IP*/
    /* modified by zhangh ---611003565876 ��FTPServer��IP��ַ����Ϊ����ȡFTPServer��XML-RPC�ӿ��з��ش�����޸�*/
    ip = s_ftp_server_ip;

    /*�ṩftp������˿�*/
    port = s_ftp_daemon_port;

    return SUCCESS;
}

STATUS FtpdInit()
{
    //�Ӵ洢��˻�ȡ�ļ��ϴ���Ĵ��·��
    ImageStore* store = ImageStore::GetInstance();
    if(!store)
    {
        SkyAssert(false);
        return ERROR_NOT_READY;
    }
    string ftp_root_dir;
    store->GetStorageDir(ftp_root_dir);
    if(ftp_root_dir.empty())
    {
        SkyAssert(false);
        return ERROR;
    }

    //����md�����̳߳�
    s_md5SumTaskPool = new ThreadPool(3);
    if (!s_md5SumTaskPool)
    {
        Debug(SYS_EMERGENCY,"failed to create s_md5SumTaskPool!\n");
        return ERROR_NO_MEMORY;
    }
    STATUS ret = s_md5SumTaskPool->CreateThreads("md5sumTask");
    if (SUCCESS != ret)
    {
        Debug(SYS_EMERGENCY,"failed to start ThreadPool threads! ret = %d\n",ret);
        return ret;
    }

    //��һ�����ݿ����Ƿ���ӳ���ļ�δ����md5������оͼ���һ��
    ImagePool *ipool = ImagePool::GetInstance();
    vector<string> files;
    ret = ipool->GetImageFileWithoutMd5(files);
    if (ret != SQLDB_OK && ret != SQLDB_RESULT_EMPTY)
    {
        Debug(SYS_EMERGENCY,"failed to GetImageFileWithoutMd5! ret = %d\n",ret);
        return ERROR_DB_SELECT_FAIL;
    }

    if(!files.empty())
    {
        Debug(SYS_NOTICE,"%d image files need to calc md5!\n",files.size());
        vector<string>::iterator it;
        for(it = files.begin(); it != files.end(); it++)
        {
            CalcMd5sum(it->c_str());
        }
    }

    //����ftp����
    return StartFtpServer(ftp_root_dir,s_ftp_server_ip);
}

void DbgShowFtpdConfig()
{
    cout << "ftpd daemon port: " << s_ftp_daemon_port << endl;
    cout << "ftpd data port: " << s_ftp_data_port << endl;
    cout << "ftpd root directory: " << s_strDefRootPath << endl;
    cout << "ftpd s_ftp_data_port_begin : " << s_ftp_data_port_begin << endl;
    cout << "ftpd s_ftp_data_port_end : " << s_ftp_data_port_end << endl;
    cout << "ftpd s_iFtpMaxClients : " << s_iFtpMaxClients << endl;
}
DEFINE_DEBUG_FUNC(DbgShowFtpdConfig);

void DbgRegisterNfsImage(int64 uid, const char* file)
{
    ostringstream oss;
    string path(file);
    trim(path);
    if(SUCCESS != RegisterImage(uid,path,oss))
    {
        cerr << "failed to register image!" << endl;
    }
}
DEFINE_DEBUG_FUNC(DbgRegisterNfsImage);

void DbgIsLocalIp(char *ip)
{
    if (NULL == ip)
    {
        cout<<"ip is null"<<endl;
        return;
    }
    string checkip;
    checkip.assign(ip);
    if (true == IsLocalIp(checkip))
    {
        cout<<checkip<<" is local ip"<<endl;
    }
    else
    {
        cout<<checkip<<" is not local ip"<<endl;
    }
}
DEFINE_DEBUG_FUNC(DbgIsLocalIp);

/**********************************************************************
 * �������ƣ�FtpDataFixedPortInit
 * ����������FTP data �̶��˿ڳ�ʼ��
 * ���ʵı���
 * �޸ĵı���
 * ���������
 * ���������
 * �� �� ֵ��
 * ����˵����
 * �޸�����      �汾��  �޸���      �޸�����
 * ---------------------------------------------------------------------
 * 2012/10/12            V1.0          �              ����
 ************************************************************************/
void FtpDataPortFixedInit()
{
    if(!ftpDataPortFixedMap.empty())
    {
        ftpDataPortFixedMap.clear();
    }

    MutexLock(&ftpDataPortFixedMutex);

    for(int index = s_ftp_data_port_begin;index <= s_ftp_data_port_end;index ++)
    {
        ftpDataPortFixedMap.insert(pair<int,bool>(index,false));
    }

    Debug(SYS_NOTICE,"ftp data port fixed success\n");
    MutexUnLock(&ftpDataPortFixedMutex);
    return ;
}

/**********************************************************************
 * �������ƣ�FtpDataFixedPortGet
 * ����������FTP data �̶��˿ڻ�ȡ
 * ���ʵı���
 * �޸ĵı���
 * ���������
 * ���������
 * �� �� ֵ��
 * ����˵����
 * �޸�����      �汾��  �޸���      �޸�����
 * ---------------------------------------------------------------------
 * 2012/10/12            V1.0          �              ����
 ************************************************************************/
int FtpDataPortFixedGet()
{
    if(ftpDataPortFixedMap.empty())
    {
        Debug(SYS_WARNING,"ftpDataPortFixedMap is null\n");
        return -1;
    }

    MutexLock(&ftpDataPortFixedMutex);

    for(multimap<int, bool>::iterator iter = ftpDataPortFixedMap.begin(); iter != ftpDataPortFixedMap.end(); ++iter )
    {
        /*δ��ʹ��*/
        if(!iter->second)
        {
            Debug(SYS_NOTICE,"get a data port %d\n",iter->first);
            iter->second = true;
            MutexUnLock(&ftpDataPortFixedMutex);
            return iter->first;
        }
    }

    MutexUnLock(&ftpDataPortFixedMutex);
    Debug(SYS_NOTICE,"ftpDataPortFixedMap is full\n");
    return -1;
}

/**********************************************************************
 * �������ƣ�FtpDataFixedPortGet
 * ����������FTP data �̶��˿��ͷ�
 * ���ʵı���
 * �޸ĵı���
 * ���������
 * ���������
 * �� �� ֵ��
 * ����˵����
 * �޸�����      �汾��  �޸���      �޸�����
 * ---------------------------------------------------------------------
 * 2012/10/12            V1.0          �              ����
 ************************************************************************/
bool FtpDataPortFixedFree(int port)
{
    if(ftpDataPortFixedMap.empty())
    {
        Debug(SYS_WARNING,"ftpDataPortFixedMap is null\n");
        return false;
    }

    if((port < s_ftp_data_port_begin) || (port > s_ftp_data_port_end))
    {
        Debug(SYS_WARNING,"port is not in the fixed scope\n");
        return false;
    }

    MutexLock(&ftpDataPortFixedMutex);

    for(multimap<int, bool>::iterator iter = ftpDataPortFixedMap.begin(); iter != ftpDataPortFixedMap.end(); ++iter )
    {
        /*δ��ʹ��*/
        if((iter->first == port)&&(iter->second == true))
        {
            iter->second = false;
            Debug(SYS_NOTICE,"free port succ\n");
            MutexUnLock(&ftpDataPortFixedMutex);
            return true;
        }
    }

    MutexUnLock(&ftpDataPortFixedMutex);
    Debug(SYS_NOTICE,"free port succ\n");
    return true;
}



