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

/* telnet协商选项 */
static const char iacs_to_send[] =
    {
        IAC, DONT, TELOPT_ECHO,   /* 客户端不回显 */
        IAC, DO, TELOPT_NAWS,     /* 窗口大小协商 */
        IAC, DO, TELOPT_LFLOW,    /* 远程流控制 */
        IAC, WILL, TELOPT_ECHO,   /* 服务端进行回显 */
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
        //禁止重复初始化
        return ERROR_DUPLICATED_OP;
    }

    /* 为新的客户端建立一个伪终端 */
    if (0 > openpty(&ptyfd_master,&ptyfd_slave,ptyname,NULL,NULL))
    {
        printf("Create pty failed! errno: %d\n",errno);
        return ERROR;
    }

    //printf("Telnetd pty create, master: %d, slave: %d\n",masterfd,slavefd);

    /* 子进程不继承本fd */
    fcntl(ptyfd_master, F_SETFD, FD_CLOEXEC);
    fcntl(ptyfd_slave, F_SETFD, FD_CLOEXEC);
    /* 设置pty为非阻塞模式 */
    fcntl(ptyfd_master, F_SETFL, fcntl(ptyfd_master,F_GETFL) | O_NONBLOCK);

    //创建服务控制命令管道
    int ctl_pipe[2];
    //if (0 > pipe2(ctl_pipe, O_NONBLOCK|O_CLOEXEC))
    if (0 > pipe(ctl_pipe))
    {
        close(ptyfd_master);
        return ERROR;
    }

    ctlreadfd = ctl_pipe[0];
    ctlwritefd = ctl_pipe[1];
    /* 子进程不继承本fd */
    fcntl(ctlreadfd, F_SETFD, FD_CLOEXEC);
    fcntl(ctlwritefd, F_SETFD, FD_CLOEXEC);
    
    fcntl(ctlreadfd, F_SETFL, fcntl(ctlreadfd,F_GETFL) | O_NONBLOCK | FD_CLOEXEC);

    //创建telnetd监听socket
    if ((serverfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
    {
        printf("create telnetd socket failed! errno: %d\n",errno);
        close(ptyfd_master);
        close(ctlreadfd);
        close(ctlwritefd);
        return ERROR;
    }

    fcntl(serverfd, F_SETFD, FD_CLOEXEC);

    /* 允许重用本地地址和端口 */
    int reuseaddr = 1;
    setsockopt(serverfd,SOL_SOCKET,SO_REUSEADDR,(const char*)&reuseaddr,sizeof(int));

    bzero(&server_addr,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    /* 根据传入标志确定是否只绑定127地址 */
    if (true == local)
    {
        server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    }
    else
    {
        server_addr.sin_addr.s_addr = htons(INADDR_ANY);
    }

    //绑定serverfd
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

    //通过管道发送控制命令
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
        //将标准输出放开到telnet终端上
        dup2(ptyfd_slave,1);
        dup2(ptyfd_slave,2);
    }
    else
    {
        int fd = -1;
        
        //将标准输入重定向
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
        // 服务忙时不接受新客户端
        write(fd,TELNETD_BUSY_ALERT,strlen(TELNETD_BUSY_ALERT));
        close(fd);
        return ERROR;
    }

    //保存客户端socket fd
    clientfd = fd;

    //设置socket tcp保活
    int on = 1;
    setsockopt(clientfd, SOL_SOCKET, SO_KEEPALIVE, (char *)&on, sizeof (on));
    //设置发送缓冲区大小
    int sendsize = 64 * 1024;
    setsockopt(clientfd, SOL_SOCKET, SO_SNDBUF,(char *)&sendsize,sizeof(sendsize));
    //设置为非阻塞
    fcntl(clientfd, F_SETFL, fcntl(clientfd,F_GETFL) | O_NONBLOCK);

    /* 在待发送的缓冲区预先准备好选项协商命令序列 */
    memcpy(buf2, iacs_to_send, sizeof(iacs_to_send));
    rdidx2 = sizeof(iacs_to_send);
    size2 = sizeof(iacs_to_send);

    //放开标准输出到pty
    OpenStdout(true);
    //服务进入忙状态
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
    
    //关闭标准输出
    OpenStdout(false);
    //服务重新进入空闲状态
    busy = false;
}

char* TelnetServer::RemoveIAC(uint32 *num)
{
    /* 最终需要写入pty的字符个数 */
    uint32 num_totty;

    /* 从socket读进来但尚未写入pty的缓冲位置索引 */
    unsigned char *ptr0 = (unsigned char*)(buf1 + wridx1);
    unsigned char *ptr = ptr0;
    unsigned char *totty = ptr;

    /* 待处理的缓冲结尾 */
    unsigned char *end = ptr + MIN(BUFSIZE - wridx1, size1);

    /* 逐个处理缓冲区的字符 */
    while (ptr < end)
    {
        /* 非telnet控制命令 */
        if (*ptr != IAC)
        {
            /* 待写入pty的字符数量++ */
            char c = *ptr;
            *totty++ = c;
            ptr++;

            /* 将"\r\n"和"\r\0"替换成"\r" */
            if (c == '\r' && ptr < end && (*ptr == '\n' || *ptr == '\0'))
                ptr++;
        }
        else
        {
            if ((ptr+2) >= end)
            {
                /* 不完整的IAC命令 */
                break;
            }
            else if (ptr[1] == SB && ptr[2] == TELOPT_NAWS)
            {
                /*
                * 窗口大小选项以及子选项协商控制命令序列:
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
                /* 略过其它的选项协商 */
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
    /* 缓冲区的IAC被处理并删除后,腾出了ptr - totty字节的空间,
       将剩余的需要写入pty的字符串挪动到后面,腾出的空间在前面 */
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

    //到telent伪终端获取标准输入
    dup2(ptyfd_slave,0);
    
    //还没有用户连接，先关闭标准输出
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

        //检查服务控制命令
        FD_SET(ctlreadfd, &rdfdset);
        maxfd = MAX(maxfd,ctlreadfd);

       //检查新客户端连接请求
        FD_SET(serverfd, &rdfdset);
        maxfd = MAX(maxfd,serverfd);

        if (busy == true)
        {
            /* 服务处于busy状态时，检查是否需要缓冲区维护 */
            /* sock->pty的接收缓冲有数据时,需要检查pty是否可写 */
            if (size1 > 0)
            {
                FD_SET(ptyfd_master, &wrfdset);
                maxfd = MAX(maxfd,ptyfd_master);
            }

            /* pty->sock的发送缓冲有空间时,需要检查pty是否可读 */
            if (size2 < BUFSIZE)
            {
                FD_SET(ptyfd_master, &rdfdset);
                maxfd = MAX(maxfd,ptyfd_master);
            }

            /* sock->pty的接收缓冲有空间,需要检查socket是否可读 */
            if (size1 < BUFSIZE)
            {
                FD_SET(clientfd, &rdfdset);
                maxfd = MAX(maxfd,clientfd);
            }

            /* pty->sock的发送缓冲有数据,需要检查socket是否可写 */
            if (size2 > 0)
            {
                FD_SET(clientfd, &wrfdset);
                maxfd = MAX(maxfd,clientfd);
            }
        }

        //开始检查
        count = select(maxfd + 1, &rdfdset, &wrfdset, NULL, NULL);
        if (count < 0)
        {
            Delay(100);
            continue;
        }

        //处理服务控制命令
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

        //处理新客户连接
        if (FD_ISSET(serverfd, &rdfdset))
        {
            socklen_t socklen = sizeof(struct sockaddr);
            // accept一个新的telnet连接
            AcceptClient(accept(serverfd, (struct sockaddr *)&client_addr, &socklen));
            continue;
        }

        /* 处理会话的双向缓冲区数据通道 */
//write_pty:
        /* 如果buffer1有数据且pty可写,将buffer1中的数据写入pty */
        if (size1 && FD_ISSET(ptyfd_master, &wrfdset))
        {
            uint32 num = 0;
            char  *ptr = NULL;
            /* 移除buf1中数据的iac字符. */
            ptr = RemoveIAC(&num);
            /* 将会话收到的socket数据写入会话的pty */
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

            /* buf1的size减少 */
            size1 -= count;
            /* buf1的write index增加 */
            wridx1 += count;

            /* 如果write index超过buf大小，将其置为0 */
            if (wridx1 >= BUFSIZE)
            {
                wridx1 = 0;
            }
        }

write_socket:
        /* 如果socket可写，将buf2中的数据从socket上发送 */
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
        //如果buf1中的内容都已经写入pty，将读写位置清零
        if (size1 == 0)
        {
            rdidx1 = 0;
            wridx1 = 0;
        }

        //如果buf2中的内容都已经写入socket，将读写位置清零
        if (size2 == 0)
        {
            rdidx2 = 0;
            wridx2 = 0;
        }

        /* 如果buffer1有空间且socket可读，将socket中的数据读到buf1中 */
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
        /* 如果pty可读，将pty中的数据读到buf2中 */
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
    //启动一个线程来侦听serverfd
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


