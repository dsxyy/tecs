#include "common.h"
#include "clock.h"

#ifndef MIN
#define	MIN(a,b) (((a)<(b))?(a):(b))
#endif

#ifndef MAX
#define	MAX(a,b) (((a)>(b))?(a):(b))
#endif

#define TELNET_CLIENT_COLS      80
#define TELNET_CLIENT_ROWS      100
#define TELNETD_BUSY_ALERT      "\nTelnet server is busy!\n"
#define TELNETD_BYE_ALERT       "\nThanks for use!\n"
#define MAX_COMMAND_LEN         15

/* telnetЭ��ѡ�� */
static const char iacs_to_send[] =
    {
        IAC, DONT, TELOPT_ECHO,   /* �ͻ��˲����� */
        IAC, DO, TELOPT_NAWS,     /* ���ڴ�СЭ�� */
        IAC, DO, TELOPT_LFLOW,    /* Զ�������� */
        IAC, WILL, TELOPT_ECHO,   /* ����˽��л��� */
        IAC, WILL, TELOPT_SGA     /* suppress go ahead */
    };

static const char iacs_naws[] =
    {
        IAC, SB, TELOPT_NAWS,
        (byte) (TELNET_CLIENT_COLS >> 8), (byte) (TELNET_CLIENT_COLS & 0xff),
        (byte) (TELNET_CLIENT_ROWS >> 8), (byte) (TELNET_CLIENT_ROWS & 0xff),
        IAC,SE
    };

static int SafeRead(int fd, void *buf, size_t count)
{
    int n = -1;

    do
    {
        n = read(fd, buf, count);
    }
    while (n < 0 && errno == EINTR);

    return n;
}

static int SafeWrite(int fd, const void *buf, size_t count)
{
    int n = -1;

    do
    {
        n = write(fd, buf, count);
    }
    while (n < 0 && errno == EINTR);

    return n;
}

extern "C" void *TelnetdTaskEntry(void *arg)
{
    TelnetServer *p = static_cast<TelnetServer *>(arg);
    p->Serve();
    return NULL;
}

TelnetServer::TelnetServer():stdout_file("/dev/null")
{
    busy = false;
    server_thread_id = INVALID_THREAD_ID;

    ptyfd_master = -1;
    ptyfd_slave = -1;
    memset(ptyname,0,sizeof(ptyname));

    serverfd = -1;
    clientfd = -1;
    ctlwritefd = -1;
    ctlreadfd = -1;
    rdidx1 = 0;
    wridx1 = 0;
    size1 = 0;
    rdidx2 = 0;
    wridx2 = 0;
    size2 = 0;
    memset(buf1,0,sizeof(buf1));
    memset(buf2,0,sizeof(buf2));
}

TelnetServer::~TelnetServer()
{
    if(server_thread_id != INVALID_THREAD_ID)
        KillThread(server_thread_id);

    if(0 < serverfd)
        close(serverfd);

    if(0 < clientfd)
        close(clientfd);
    
    if(0 < ptyfd_master)
        close(ptyfd_master);

    if(0 < ctlreadfd)
        close(ctlreadfd);

    if(0 < ctlwritefd)
        close(ctlwritefd);
}

STATUS TelnetServer::SetBanner(const string& str)
{
    banner = str;
    return SUCCESS;
}
    
STATUS TelnetServer::Init(bool local,uint16 port,const char* stdout)
{
    if (ptyfd_master > 0)
    {
        //��ֹ�ظ���ʼ��
        return ERROR_DUPLICATED_OP;
    }

    /* Ϊ�µĿͻ��˽���һ��α�ն� */
    if (0 > openpty(&ptyfd_master,&ptyfd_slave,ptyname,NULL,NULL))
    {
        printf("Create pty failed! errno: %d\n",errno);
        return ERROR;
    }

    //printf("Telnetd pty create, master: %d, slave: %d\n",masterfd,slavefd);

    /* �ӽ��̲��̳б�fd */
    fcntl(ptyfd_master, F_SETFD, FD_CLOEXEC);
    fcntl(ptyfd_slave, F_SETFD, FD_CLOEXEC);
    /* ����ptyΪ������ģʽ */
    fcntl(ptyfd_master, F_SETFL, fcntl(ptyfd_master,F_GETFL) | O_NONBLOCK);

    //���������������ܵ�
    int ctl_pipe[2];
    //if (0 > pipe2(ctl_pipe, O_NONBLOCK|O_CLOEXEC))
    if (0 > pipe(ctl_pipe))
    {
        close(ptyfd_master);
        return ERROR;
    }

    ctlreadfd = ctl_pipe[0];
    ctlwritefd = ctl_pipe[1];
    /* �ӽ��̲��̳б�fd */
    fcntl(ctlreadfd, F_SETFD, FD_CLOEXEC);
    fcntl(ctlwritefd, F_SETFD, FD_CLOEXEC);
    
    fcntl(ctlreadfd, F_SETFL, fcntl(ctlreadfd,F_GETFL) | O_NONBLOCK | FD_CLOEXEC);

    //����telnetd����socket
    if ((serverfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
    {
        printf("create telnetd socket failed! errno: %d\n",errno);
        close(ptyfd_master);
        close(ctlreadfd);
        close(ctlwritefd);
        return ERROR;
    }

    fcntl(serverfd, F_SETFD, FD_CLOEXEC);

    /* �������ñ��ص�ַ�Ͷ˿� */
    int reuseaddr = 1;
    setsockopt(serverfd,SOL_SOCKET,SO_REUSEADDR,(const char*)&reuseaddr,sizeof(int));

    bzero(&server_addr,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    /* ���ݴ����־ȷ���Ƿ�ֻ��127��ַ */
    if (true == local)
    {
        server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    }
    else
    {
        server_addr.sin_addr.s_addr = htons(INADDR_ANY);
    }

    //��serverfd
    if (bind(serverfd,(struct sockaddr*)&server_addr,sizeof(server_addr)) < 0)
    {
        printf("bind telnetd socket failed! errno: %d\n",errno);
        close(ptyfd_master);
        close(ctlreadfd);
        close(ctlwritefd);
        close(serverfd);
        return ERROR;
    }
    
    if(0 == port)
    {
        struct sockaddr_in addr;  
        socklen_t len = sizeof(struct sockaddr_in);
        if(0 == getsockname(serverfd,(struct sockaddr*)&addr,&len))
            port = ntohs(addr.sin_port);
    }
    
    if(stdout)
        stdout_file.assign(stdout);
    
    return SUCCESS;
}

int TelnetServer::GetListenPort()
{
    if(serverfd < 0)
        return 0;

    struct sockaddr_in addr;  
    socklen_t len = sizeof(struct sockaddr_in);
    if(0 == getsockname(serverfd,(struct sockaddr*)&addr,&len))
        return ntohs(addr.sin_port);
    else
        return 0;
}

STATUS TelnetServer::Control(const char* command)
{
    if (!command)
    {
        return ERROR_INVALID_ARGUMENT;
    }

    int len = strlen(command);
    if (len > MAX_COMMAND_LEN)
    {
        return ERROR_INVALID_ARGUMENT;
    }

    //ͨ���ܵ����Ϳ�������
    if (len != write(ctlwritefd,command,len))
    {
        return ERROR_IO_FAILED;
    }

    return SUCCESS;
}

STATUS TelnetServer::OpenStdout(bool flag)
{
    if(flag)
    {
        //����׼����ſ���telnet�ն���
        dup2(ptyfd_slave,1);
        dup2(ptyfd_slave,2);
    }
    else
    {
        int fd = -1;
        
        //����׼�����ض���
        if(stdout_file.empty())
            fd = open("/dev/null", O_RDWR);
        else
            fd = open(stdout_file.c_str(), O_RDWR | O_CREAT);
        
        dup2(fd,1);
        dup2(fd,2);
        close(fd);
    }
    return SUCCESS;
}

STATUS TelnetServer::AcceptClient(int fd)
{
    if (fd < 0)
    {
        return ERROR_INVALID_ARGUMENT;
    }

    if (busy == true)
    {
        // ����æʱ�������¿ͻ���
        write(fd,TELNETD_BUSY_ALERT,strlen(TELNETD_BUSY_ALERT));
        close(fd);
        return ERROR;
    }

    //����ͻ���socket fd
    clientfd = fd;

    //����socket tcp����
    int on = 1;
    setsockopt(clientfd, SOL_SOCKET, SO_KEEPALIVE, (char *)&on, sizeof (on));
    //���÷��ͻ�������С
    int sendsize = 64 * 1024;
    setsockopt(clientfd, SOL_SOCKET, SO_SNDBUF,(char *)&sendsize,sizeof(sendsize));
    //����Ϊ������
    fcntl(clientfd, F_SETFL, fcntl(clientfd,F_GETFL) | O_NONBLOCK);

    /* �ڴ����͵Ļ�����Ԥ��׼����ѡ��Э���������� */
    memcpy(buf2, iacs_to_send, sizeof(iacs_to_send));
    rdidx2 = sizeof(iacs_to_send);
    size2 = sizeof(iacs_to_send);

    //�ſ���׼�����pty
    OpenStdout(true);
    //�������æ״̬
    busy = true;
    cout << banner << endl;
    return SUCCESS;
}

void TelnetServer::RefuseClient()
{
    if (clientfd > 0)
    {
        write(clientfd,TELNETD_BYE_ALERT,strlen(TELNETD_BYE_ALERT));
        shutdown(clientfd,SHUT_RDWR);
        close(clientfd);
    }

    clientfd = -1;
    rdidx1 = 0;
    wridx1 = 0;
    size1 = 0;
    rdidx2 = 0;
    wridx2 = 0;
    size2 = 0;
    memset(buf1,0,sizeof(buf1));
    memset(buf2,0,sizeof(buf2));
    
    //�رձ�׼���
    OpenStdout(false);
    //�������½������״̬
    busy = false;
}

char* TelnetServer::RemoveIAC(uint32 *num)
{
    /* ������Ҫд��pty���ַ����� */
    uint32 num_totty;

    /* ��socket����������δд��pty�Ļ���λ������ */
    unsigned char *ptr0 = (unsigned char*)(buf1 + wridx1);
    unsigned char *ptr = ptr0;
    unsigned char *totty = ptr;

    /* ������Ļ����β */
    unsigned char *end = ptr + MIN(BUFSIZE - wridx1, size1);

    /* ��������������ַ� */
    while (ptr < end)
    {
        /* ��telnet�������� */
        if (*ptr != IAC)
        {
            /* ��д��pty���ַ�����++ */
            char c = *ptr;
            *totty++ = c;
            ptr++;

            /* ��"\r\n"��"\r\0"�滻��"\r" */
            if (c == '\r' && ptr < end && (*ptr == '\n' || *ptr == '\0'))
                ptr++;
        }
        else
        {
            if ((ptr+2) >= end)
            {
                /* ��������IAC���� */
                break;
            }
            else if (ptr[1] == SB && ptr[2] == TELOPT_NAWS)
            {
                /*
                * ���ڴ�Сѡ���Լ���ѡ��Э�̿�����������:
                * IAC -> SB -> TELOPT_NAWS -> 4-byte -> IAC -> SE
                */
                struct winsize ws;
                if ((ptr+8) >= end)
                {
                    break;	/* incomplete, can't process */
                }
                ws.ws_col = (ptr[3] << 8) | ptr[4];
                ws.ws_row = (ptr[5] << 8) | ptr[6];

                //printf("Telnet window size: row: %d, col: %d\n",ws.ws_row,ws.ws_col);
                ioctl(ptyfd_master, TIOCSWINSZ, (char *)&ws);
                ptr += 9;
            }
            else
            {
                /* �Թ�������ѡ��Э�� */
                ptr += 3;
            }
        }
    }

    num_totty = totty - ptr0;
    *num = num_totty;

    /* the difference between ptr and totty is number of iacs
       we removed from the stream. Adjust buf1 accordingly. */
    if ((ptr - totty) == 0) /* 99.999% of cases */
        return (char*)ptr0;

    wridx1 += ptr - totty;
    size1 -= ptr - totty;
    /* ��������IAC������ɾ����,�ڳ���ptr - totty�ֽڵĿռ�,
       ��ʣ�����Ҫд��pty���ַ���Ų��������,�ڳ��Ŀռ���ǰ�� */
    return (char*)memmove(ptr - num_totty, ptr0, num_totty);
}

STATUS TelnetServer::Serve()
{
    int maxfd = 0;
    fd_set rdfdset, wrfdset;
    int32 count;
    struct sockaddr_in client_addr;

    if (listen(serverfd, 1) < 0)
    {
        printf("listen telnetd socket failed! errno: %d\n",errno);
        return ERROR;
    }

    //��telentα�ն˻�ȡ��׼����
    dup2(ptyfd_slave,0);
    
    //��û���û����ӣ��ȹرձ�׼���
    OpenStdout(false);
    
    /*
       This is how the buffers are used. The arrows indicate the movement
       of data.
       +-------+     wridx1++     +------+     rdidx1++     +----------+
       |       | <--------------  | buf1 | <--------------  |          |
       |       |     size1--      +------+     size1++      |          |
       |  pty  |                                            |  socket  |
       |       |     rdidx2++     +------+     wridx2++     |          |
       |       |  --------------> | buf2 |  --------------> |          |
       +-------+     size2++      +------+     size2--      +----------+

       size1: "how many bytes are buffered for pty between rdidx1 and wridx1?"
       size2: "how many bytes are buffered for socket between rdidx2 and wridx2?"

       Each session has got two buffers. Buffers are circular. If sizeN == 0,
       buffer is empty. If sizeN == BUFSIZE, buffer is full. In both these cases
       rdidxN == wridxN.
    */
    
    while (1)
    {
        FD_ZERO(&rdfdset);
        FD_ZERO(&wrfdset);

        //�������������
        FD_SET(ctlreadfd, &rdfdset);
        maxfd = MAX(maxfd,ctlreadfd);

       //����¿ͻ�����������
        FD_SET(serverfd, &rdfdset);
        maxfd = MAX(maxfd,serverfd);

        if (busy == true)
        {
            /* ������busy״̬ʱ������Ƿ���Ҫ������ά�� */
            /* sock->pty�Ľ��ջ���������ʱ,��Ҫ���pty�Ƿ��д */
            if (size1 > 0)
            {
                FD_SET(ptyfd_master, &wrfdset);
                maxfd = MAX(maxfd,ptyfd_master);
            }

            /* pty->sock�ķ��ͻ����пռ�ʱ,��Ҫ���pty�Ƿ�ɶ� */
            if (size2 < BUFSIZE)
            {
                FD_SET(ptyfd_master, &rdfdset);
                maxfd = MAX(maxfd,ptyfd_master);
            }

            /* sock->pty�Ľ��ջ����пռ�,��Ҫ���socket�Ƿ�ɶ� */
            if (size1 < BUFSIZE)
            {
                FD_SET(clientfd, &rdfdset);
                maxfd = MAX(maxfd,clientfd);
            }

            /* pty->sock�ķ��ͻ���������,��Ҫ���socket�Ƿ��д */
            if (size2 > 0)
            {
                FD_SET(clientfd, &wrfdset);
                maxfd = MAX(maxfd,clientfd);
            }
        }

        //��ʼ���
        count = select(maxfd + 1, &rdfdset, &wrfdset, NULL, NULL);
        if (count < 0)
        {
            Delay(100);
            continue;
        }

        //��������������
        if (FD_ISSET(ctlreadfd, &rdfdset))
        {
            char command[MAX_COMMAND_LEN + 1] = {'\0'};
            read(ctlreadfd,command,sizeof(command));
            if (strncmp(command,"quit",MAX_COMMAND_LEN) == 0)
            {
                RefuseClient();
                continue;
            }
        }

        //�����¿ͻ�����
        if (FD_ISSET(serverfd, &rdfdset))
        {
            socklen_t socklen = sizeof(struct sockaddr);
            // acceptһ���µ�telnet����
            AcceptClient(accept(serverfd, (struct sockaddr *)&client_addr, &socklen));
            continue;
        }

        /* ����Ự��˫�򻺳�������ͨ�� */
//write_pty:
        /* ���buffer1��������pty��д,��buffer1�е�����д��pty */
        if (size1 && FD_ISSET(ptyfd_master, &wrfdset))
        {
            uint32 num = 0;
            char  *ptr = NULL;
            /* �Ƴ�buf1�����ݵ�iac�ַ�. */
            ptr = RemoveIAC(&num);
            /* ���Ự�յ���socket����д��Ự��pty */
            count = SafeWrite(ptyfd_master, ptr, num);
            if (count < 0)
            {
                if (errno == EAGAIN)
                {
                    goto write_socket;
                }

                RefuseClient();
                continue;
            }

            /* buf1��size���� */
            size1 -= count;
            /* buf1��write index���� */
            wridx1 += count;

            /* ���write index����buf��С��������Ϊ0 */
            if (wridx1 >= BUFSIZE)
            {
                wridx1 = 0;
            }
        }

write_socket:
        /* ���socket��д����buf2�е����ݴ�socket�Ϸ��� */
        if (size2 && FD_ISSET(clientfd, &wrfdset))
        {
            count = MIN(BUFSIZE - wridx2, size2);
            count = SafeWrite(clientfd, buf2 + wridx2, count);
            if (count < 0)
            {
                if (errno == EAGAIN)
                    goto read_socket;

                RefuseClient();
                continue;
            }

            size2 -= count;
            wridx2 += count;
            if (wridx2 >= BUFSIZE)
            {
                wridx2 = 0;
            }
        }

read_socket:
        //���buf1�е����ݶ��Ѿ�д��pty������дλ������
        if (size1 == 0)
        {
            rdidx1 = 0;
            wridx1 = 0;
        }

        //���buf2�е����ݶ��Ѿ�д��socket������дλ������
        if (size2 == 0)
        {
            rdidx2 = 0;
            wridx2 = 0;
        }

        /* ���buffer1�пռ���socket�ɶ�����socket�е����ݶ���buf1�� */
        if (size1 < BUFSIZE && FD_ISSET(clientfd, &rdfdset))
        {
            count = MIN(BUFSIZE - rdidx1, BUFSIZE - size1);
            count = SafeRead(clientfd, buf1 + rdidx1, count);
            if (count <= 0)
            {
                if (count < 0 && errno == EAGAIN)
                    goto read_pty;

                RefuseClient();
                continue;
            }

            if (!buf1[rdidx1 + count - 1])
            {
                --count;
            }
            size1 += count;
            rdidx1 += count;
            if (rdidx1 >= BUFSIZE)
            {
                rdidx1 = 0;
            }
        }

read_pty:
        /* ���pty�ɶ�����pty�е����ݶ���buf2�� */
        if (size2 < BUFSIZE && FD_ISSET(ptyfd_master, &rdfdset))
        {
            count = MIN(BUFSIZE - rdidx2, BUFSIZE - size2);
            count = SafeRead(ptyfd_master, buf2 + rdidx2, count);
            if (count <= 0)
            {
                RefuseClient();
                continue;
            }

            size2 += count;
            rdidx2 += count;

            if (rdidx2 >= BUFSIZE)
            {
                rdidx2 = 0;
            }
        }
        continue;
    }

    return ERROR;
}

STATUS TelnetServer::Start(int priority)
{
    if (serverfd <= 0)
    {
        return ERROR_NOT_READY;
    }

    if(INVALID_THREAD_ID != server_thread_id)
    {
        return ERROR_DUPLICATED_OP;
    }
    //����һ���߳�������serverfd
    server_thread_id = StartThreadEx("TelnetServer",1,0,TelnetdTaskEntry,(void *)this);
    if(INVALID_THREAD_ID == server_thread_id)
        return ERROR;
    else
        return SUCCESS;
}

static TelnetServer *server = NULL;
STATUS Telnetd(bool local,uint16& port,const char* stdout)
{
    STATUS ret;
    if (server)
    {
        return ERROR_DUPLICATED_OP;
    }

    server = new TelnetServer();
    if (!server)
    {
        return ERROR_NO_MEMORY;
    }
    
    ret = server->Init(local,port,stdout);
    if(SUCCESS != ret)
    {
        return ret;
    }
    port = server->GetListenPort();
    return server->Start();
}

STATUS SetTelnetdBanner(const string& banner)
{
    if (!server)
    {
        return ERROR_NOT_READY;
    }
    return server->SetBanner(banner);
}

static void ClientQuit()
{
    if (server)
    {
        server->Control("quit");
    }
    else
    {
        printf("'quit' command only works for telnet client!\n");
    }
}

DEFINE_DEBUG_CMD(quit,ClientQuit);


