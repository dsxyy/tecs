/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：uds.cpp
* 文件标识：见配置管理计划书
* 内容摘要：uds通信封装实现文件
* 当前版本：1.0
* 作    者：张文剑
* 完成日期：2011年8月16日
*
* 修改记录1：
*     修改日期：2011/07/01
*     版 本 号：V1.0
*     修 改 人：张文剑
*     修改内容：创建
*******************************************************************************/
#include "uds.h"

#define MAX_UDSMSG_SIZE (32*1024)

extern "C" void *udsWorkLoopTaskEntry(void *arg)
{
    char rx_buf[MAX_UDSMSG_SIZE];
    UdsHeader *ph = NULL;
    UdsMessenger *pum = NULL;
    if (arg == 0)
    {
        printf("udsWorkLoopTaskEntry no arg!\n");
        return 0;
    }
    pum = static_cast<UdsMessenger *>(arg);

    //printf("%s is waiting for message ...\n",pum->GetSelfPath());
    while (1)
    {
        memset(rx_buf,0,sizeof(rx_buf));
        if (SUCCESS != pum->Wait(rx_buf,sizeof(rx_buf),WAIT_FOREVER))
        {
            continue;
        }

        ph = (UdsHeader *)rx_buf;
        int message_mode = pum->GetMessageMode();
        if(message_mode == UdsMessenger::MESSAGE_BINARY_MODE)
        {
            pum->MessageEntry(ph->msgid,(void *)(ph+1),ph->bodylen, ph->bodyver);
        }
        else if(message_mode == UdsMessenger::MESSAGE_STRING_MODE)
        {
            //如果是字符串消息，就按照c++方式接收、执行回调
            string data((char *)((void *)(ph+1)));
            //从二进制消息头提取信息构造UdsMessageOption
            UdsMessageOption option;
            option.sender.assign(ph->sender);
            option.receiver.assign(pum->GetSelfPath());
            option.msgid = ph->msgid;
            option.msgver = ph->bodyver;
            pum->MessageEntry(data,option);
        }
        else
        {
            SkyAssert(false);
        }
    }
    return 0;
}

void UdsMessenger::MakeFullPath(const char *strParent,const char *strSub,char *strOut,int iOutSize)
{
    if ( NULL == strOut )
        return;

    *strOut = 0;
    strOut[iOutSize-1]  = 0;

    if ( NULL == strParent || 0 == *strParent )
    {
        strParent  = "/";
    }

    if ( NULL == strSub || 0 == *strSub )
    {
        strncpy(strOut,strParent,iOutSize-1);
        return;
    }

    if ( '/' == *strSub )
    {
        strncpy(strOut,strSub,iOutSize-1);
    }
    else
    {
        int iLen = strlen(strParent);
        memcpy(strOut,strParent,iLen);

        if ( '/' != strParent[iLen-1] )
        {
            strOut[iLen++]   = '/';
        }

        strncpy(strOut+iLen,strSub,iOutSize - iLen - 1);
    }
}

STATUS UdsMessenger::Init(const char *file)
{
    int ret = -1;
    struct sockaddr_un addr;
    if (0 < _sockfd)
    {
        return ERROR_DUPLICATED_OP;
    }

    if(access(file, F_OK) != 0)
    {
        //如果socket文件不存在就新建一个
        int fd = open(file, O_WRONLY|O_CREAT|O_NOCTTY|O_NONBLOCK|O_CLOEXEC, 0666);
        if(0 > fd)
        {
            return ERROR_SYS_ERRNO(errno);
        }
    }

    if (0 != unlink(file))
    {
        perror("UdsMessenger unlink sockfd!");
        //return ERROR;
    }

    _sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (0 > _sockfd)
    {
        perror("UdsMessenger cannot create sockfd!");
        return ERROR_CREATE_SOCKET;
    }
    
    fcntl(_sockfd, F_SETFL, fcntl(_sockfd,F_GETFL) | O_NONBLOCK);
    int flags = fcntl(_sockfd, F_GETFD);
    flags |= FD_CLOEXEC;
    fcntl(_sockfd, F_SETFD, flags);

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, file, sizeof(addr.sun_path)-1);
    ret = bind(_sockfd, (struct sockaddr*)&addr, sizeof(addr));
    if (ret == -1)
    {
        perror("UdsMessenger bind error.");
        close(_sockfd);
        return ERROR;
    }

    int sendBufSize = 1024*1024;
    ret = setsockopt(_sockfd, SOL_SOCKET, SO_SNDBUF, &sendBufSize, sizeof(sendBufSize));
    if (ret == -1)
    {
        perror("setsockopt SO_SNDBUF error.");
        close(_sockfd);
        return ERROR;
    }
    
    int recvBufSize = 1024*1024;
    ret = setsockopt(_sockfd, SOL_SOCKET, SO_RCVBUF, &recvBufSize, sizeof(recvBufSize));
    if (ret == -1)
    {
        perror("setsockopt SO_RCVBUF error.");
        close(_sockfd);
        return ERROR;
    }

    _sockfile.assign(file);
    return SUCCESS;
}

//发送二进制数据块
STATUS UdsMessenger::Send(const char *dest,const UdsParam& param)
{
    if(!dest)
    {
        SkyAssert(false);
        return ERROR_INVALID_ARGUMENT;
    }
    
    struct stat s;
    if (0 > _sockfd)
    {
        return ERROR_NOT_READY;
    }

    if (param.msgsize + sizeof(UdsHeader) >= MAX_UDSMSG_SIZE)
    {
        SkyAssert(false);
        return ERROR_NO_SPACE;
    }

    if (stat(dest, &s) != 0)
    {
        return ERROR_OBJECT_NOT_EXIST;
    }

    if (S_ISDIR(s.st_mode))
    {
        return _SendToDir(_sockfd,dest,param,UDS_COMMON);
    }
    else if (S_ISSOCK(s.st_mode))
    {
        return _SendToFile(_sockfd,dest,param,UDS_COMMON);
    }
    return ERROR;
}

//发送序列化结构体
STATUS UdsMessenger::Send(const Serializable& data,UdsMessageOption& option)
{
    return Send(data.serialize(),option);
}

//发送c++字符串
STATUS UdsMessenger::Send(const string& data,UdsMessageOption& option)
{
    if(option.receiver.empty())
    {
        SkyAssert(false);
        return ERROR_INVALID_ARGUMENT;
    }

    UdsParam param;
    param.msgid = option.msgid;
    param.msgver = option.msgver;
    param.msgdata = data.c_str();
    param.msgsize = data.size();   
    return Send(option.receiver.c_str(),param);
}

//直接发送Serializable结构体
STATUS UdsMessenger::Send(const Serializable& data,uint32 message_id,const string& receiver)
{
    UdsMessageOption option(receiver,message_id);
    return Send(data,option);
}

//直接发送c++字符串
STATUS UdsMessenger::Send(const string& data,uint32 message_id,const string& receiver)
{
    UdsMessageOption option(receiver,message_id);
    return Send(data,option);
}

STATUS UdsMessenger::_SendToDir(int fd,const char *destdir,const UdsParam& param,UdsMessageType type)
{
    struct stat s;
    char fullpath[256];
    DIR *d = NULL;
    struct dirent *file = NULL; /* readdir函数的返回值 */

    if (!(d = opendir(destdir)))
    {
        printf("error\n");
        return -1;
    }

    //printf("Travel directory %s ... \n",destdir);
    while ((file = readdir(d)) != NULL)
    {
        /*把当前目录.，上一级目录..及隐藏文件都去掉，避免死循环遍历目录*/
        if (strncmp(file->d_name, ".", 1) == 0)
            continue;

        memset(fullpath,0,sizeof(fullpath));
        MakeFullPath(destdir, file->d_name, fullpath,sizeof(fullpath));
        if (stat(fullpath, &s) != 0)
        {
            continue;  //对于单个出错的文件忽略跳过
        }
        if (!S_ISSOCK(s.st_mode))
        {
            //printf("file %s is not a socket file!\n",fullpath);
            continue; //对于单个出错的文件忽略跳过
        }

        _SendToFile(fd,fullpath,param,type);
    }
    closedir(d);
    return SUCCESS;
}

STATUS UdsMessenger::_SendToFile(int fd,const char *destfile,const UdsParam& param,UdsMessageType type)
{
    char header[sizeof(UdsHeader)];
    struct sockaddr_un addr;
    struct msghdr tmsg;
    struct iovec aiov[2];
    int ret = -1;

    //构造接收方地址
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, destfile, sizeof(addr)-1);

    //构造消息头
    memset(header, '0', sizeof(header));
    UdsHeader *ph = (UdsHeader *)header;
    ph->headver = 0;
    ph->headlen = sizeof(UdsHeader);
    ph->bodyver = param.msgver;
    ph->bodylen = param.msgsize;
    ph->msgid = param.msgid;
    ph->msgtype = type;
    strncpy(ph->sender,_sockfile.c_str(),sizeof(ph->sender));

    //构造消息体
    aiov[0].iov_base = header;
    aiov[0].iov_len = sizeof(UdsHeader);
    aiov[1].iov_base = (void *)(param.msgdata);
    aiov[1].iov_len = param.msgsize;

    tmsg.msg_name       = &addr;
    tmsg.msg_namelen    = sizeof(addr);
    tmsg.msg_iov        = &aiov[0];
    tmsg.msg_iovlen     = 2;
    tmsg.msg_control    = NULL;
    tmsg.msg_controllen = 0;
    tmsg.msg_flags      = 0;

    //发送消息
    //printf("sending message to socket file %s ...\n",destfile);
    ret = sendmsg(fd, &tmsg, 0);
    if (ret == -1)
    {
        //perror("_SendToFile sendmsg error!");
        return ERROR_SYS_ERRNO(errno);
    }

    return SUCCESS;
}

STATUS UdsMessenger::Wait(string& data,UdsMessageOption& option,uint32 timeout)
{
    char rx_buf[MAX_UDSMSG_SIZE] = {'\0'};
    STATUS ret = Wait(rx_buf,sizeof(rx_buf),timeout);
    if(SUCCESS != ret)
    {
        return ret;
    }
    
    UdsHeader *ph = (UdsHeader *)rx_buf;
    data.assign((char *)(ph+1));
    //从二进制消息头提取信息构造UdsMessageOption
    option.sender.assign(ph->sender);
    option.receiver.assign(GetSelfPath());
    option.msgid = ph->msgid;
    option.msgver = ph->bodyver;
    return SUCCESS;
}

STATUS UdsMessenger::Wait(char *rx_buf,int rx_size,uint32 timeout)
{
    if(_sockfd < 0)
    {
        return ERROR_NOT_READY;
    }
    
    int ret = -1;
    UdsHeader *ph = NULL;
    _current = NULL;

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    socklen_t len = sizeof(addr);

    fd_set sockset;
    FD_ZERO(&sockset);
    FD_SET(_sockfd,&sockset);

REPEATSEL:
    if (timeout == WAIT_FOREVER)
    {
        ret = select(_sockfd + 1, &sockset, NULL, NULL, NULL);
    }
    else
    {
        struct timeval _timeout = {1, 0};
        _timeout.tv_sec  = timeout/1000;
        _timeout.tv_usec = (timeout%1000)*1000;
        ret = select(_sockfd + 1, &sockset, NULL, NULL, &_timeout);
    }

    if (ret == 0)
    {
        return ERROR_TIME_OUT;
    }
    else if (ret < 0)
    {
        if(EINTR == errno)
        {
            //被信号打断了
            goto REPEATSEL;
        }
        else
        {
            perror("select");
            return ERROR;
        }
    }

    ret = recvfrom(_sockfd, rx_buf, rx_size, 0, (struct sockaddr*)&addr, &len);
    if (ret == -1)
    {
        perror("recvfrom error.");
        return ERROR;
    }
    
    //printf("Received message!\n");
    ph = (UdsHeader *)rx_buf;
    if (ph->msgtype == UDS_TIMER)
    {
        string data((char *)(ph+1));
        TimerMessage m;
        m.deserialize(data);
        
        if (ValidateTimer(&m) == false)
        {
            return ERROR;
        }

        if(m.type == LOOP_TIMER)
        {
            //如果是循环定时器，重新触发循环
            LoopTimer(m.tid);        
        }
    }
    _current = ph;
    return SUCCESS;
}

STATUS UdsMessenger::Call(const char *destfile,const UdsParam& param,UdsResult& result)
{
    struct stat s;
    int ret;
    char rx_buf[MAX_UDSMSG_SIZE];
    if (0 > _sockfd)
    {
        return ERROR;
    }

    if (stat(destfile, &s) != 0)
    {
        return ERROR;
    }

    if (!S_ISSOCK(s.st_mode))
    {
        return ERROR;
    }
    
    //todo:如果已经有工作线程在接收消息，则返回失败
    
    ret = _SendToFile(_sockfd,destfile,param,UDS_CALL);
    if(ret != SUCCESS)
    {
        return ret;
    }

    memset(rx_buf,0,sizeof(rx_buf));
    ret = Wait(rx_buf, sizeof(rx_buf), param.timeout);
    if(ret != SUCCESS)
    {
        return ret;
    }

    UdsHeader *ph = (UdsHeader *)rx_buf;
    if(ph->bodylen > result.bufsize)
    {
        return ERROR;
    }
    memcpy(result.buf,ph+1,ph->bodylen);
    result.msgid = ph->msgid;
    result.msgtype = ph->msgtype;
    result.msgsize = ph->bodylen;
    result.msgver = ph->bodyver;
    return SUCCESS;
}

const char *UdsMessenger::GetPeerPath()
{
    if (_current)
    {
        return _current->sender;
    }
    return NULL;
}

const char *UdsMessenger::GetSelfPath()
{
    if (!_sockfile.empty())
    {
        return _sockfile.c_str();
    }
    return NULL;
}

int UdsMessenger::GetMessageMode()
{
    return _message_mode;
}

void UdsMessenger::SetMessageMode(int mode)
{
    SkyAssert(mode == MESSAGE_STRING_MODE || mode == MESSAGE_BINARY_MODE);
    _message_mode = mode;
}

STATUS UdsMessenger::Run(int priority)
{
    if (0 > _sockfd)
    {
        return ERROR;
    }
    string thread_name = name();
    _listener = StartThread(thread_name.c_str(),udsWorkLoopTaskEntry,(void *)this);
    if(INVALID_THREAD_ID == _listener)
        return ERROR;
    else
        return SUCCESS;
}

void UdsMessenger::DoTimer(const TimerMessage& m)
{
    string data = m.serialize();
    UdsParam param(m.msgid,(void *)(data.c_str()),data.size(),0);
    _SendToFile(_sockfd,GetSelfPath(),param,UDS_TIMER);
}

TIMER_ID UdsMessenger::GetTimerId()
{
    if (!_current)
    {
        return INVALID_TIMER_ID;
    }

    if (_current->msgtype != UDS_TIMER)
    {
        return INVALID_TIMER_ID;
    }

    string data((char *)(_current+1));
    TimerMessage m;
    SkyAssert(m.deserialize(data));
    return m.tid;
}

uintptr UdsMessenger::GetTimerArg()
{
    if (!_current)
    {
        return 0;
    }

    if (_current->msgtype != UDS_TIMER)
    {
        return 0;
    }
    
    string data((char *)(_current+1));
    TimerMessage m;
    SkyAssert(m.deserialize(data));
    return m.arg;
}




