#ifndef BASESOCK_H
#define BASESOCK_H

#ifndef WIN32
    #ifndef _PACKED_1_
        #define _PACKED_1_ __attribute__ ((packed)) 
    #endif
#else
    #ifndef _PACKED_1_
        #define _PACKED_1_
    #endif
#endif

#ifdef WIN32
    #pragma pack(1)
#endif

/*socket �����õ��ĺ�*/
#define SUCCESS                    0
#define FAIL_MEMORY_ALLOC       (-1)
#define FAIL_CREATE_SOCKET      (-2)
#define FAIL_AYSN_SELECT        (-3)
#define FAIL_FIND_SOCKET        (-4)
#define FAIL_ACCEPT_SOKCET      (-5)
#define FAIL_TELNET_EXIST       (-6)

#define SOCKET_PORT_TELNET        23
#define OAM_INVALID_SOCKET       INVALID_SOCKET    /* (SOCKET)(~0)*/

#define LEN_IP_ADDR        16

/*socket�����ڵ��ļ�������*/
#define FD_TYPE_UNKNOW         0
#define FD_TYPE_BRSLISTEN      1
#define FD_TYPE_BRSTELNET     2    /*ҵ������*/
#define FD_TYPE_SERIAL            3    /*����*/
#define FD_TYPE_OSSTELNET     4    /*��������*/
#define FD_TYPE_OSSSSH          5    /*��������SSH*/

#define RECV_BUFFER_LEN        1024 /*ҵ�����ڽ������ݵ����ֵ*/

typedef int FD;
typedef struct tagTYPE_FD
{
    BYTE ucFDType;
    FD ucFd;
    WORD16 wPort;
    char    szIpAddr[LEN_IP_ADDR];
    struct tagTYPE_FD *next;
}_PACKED_1_ TYPE_FD;

/*����BRS ��Ϣ�ĺ���*/
int CreateListenSocket(WORD16 wPort);
void ProcessSocketCommand(LPVOID pMsgPara);
int ProcessSocketAccept(TYPE_FD *tListenSocket);
void ProcessSocketRecieve(TYPE_FD *tSocket);

#ifdef WIN32
    #pragma pack()
#endif

#endif

