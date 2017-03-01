/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：hc.cpp
* 文件标识：
* 内容摘要：cc进程的main函数实现文件
* 当前版本：1.0
* 作    者：袁进坤
* 完成日期：2011年8月19日
*
* 修改记录1：
*     修改日期：2011/8/19
*     版 本 号：V1.0
*     修 改 人：袁进坤
*     修改内容：创建
******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>
#include <iostream>
#include <vector>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <signal.h>
#include <getopt.h>



//定义虚拟机状态值，和LIBVIRT一致
#define    LIBV_NO_STATE  0
#define    LIBV_RUNNING   1
#define    LIBV_BLOCKED   2
#define    LIBV_PAUSED    3
#define    LIBV_SHUTDOWN  4
#define    LIBV_SHUTOFF   5
#define    LIBV_CRASHED   6



// 下面是模拟的动作值
#define TIMER_1S           1
#define CREATE             2
#define STOP               4
#define PAUSE              5
#define RESUME             6
#define DESTORY            7
#define GET_DOM_NAME      8
#define GET_DOM_ID         9
#define GET_DOM_DESCRIPTION 10
#define GET_DOM_STATE      11
#define SIMULATE_ACK       12
#define REBOOT             13
#define SET_DOM_DESCRIPTION 14
#define LIST_ALL_DOM_ID     15
#define QUERY_SIMULATE_EXIT 16
#define QUERY_SIMULATE_EXIT_EX 17


//模拟器是否存在的标志
#define SIMULATE_EXIT      0x5f
using namespace std;

class simulate_dom
{
public:
    simulate_dom(){};
    ~simulate_dom(){};
    int    dom_id;
    string dom_name;
    string destcription;
    int    state;
};

#define MAX_VM_NUM  64
vector<simulate_dom> all_dom;

int  all_dom_id = 1;

#define INT_FLAG           0
#define STRING_FLAG        1
#define ERR_FLAG           2
#define INT_ARR_FLAG       3
typedef struct 
{
    int      cmd;
    int      flag;
    int      value_int; 
    int      value_arr_int[MAX_VM_NUM];
    char    value_str[128];
} simulate_package;

int FindDomain(string &name)
{
    unsigned int i;
    for (i = 0; i < all_dom.size(); i++)
    {   
        if (all_dom[i].dom_name == name)
        {
            return all_dom[i].dom_id;
        }
    }

    return -1;
}

void AllocateDomain(string &dom_name)
{
    simulate_dom dom;
    
    dom.dom_id   = all_dom_id;
    dom.dom_name = dom_name;
    dom.state    = (int)LIBV_RUNNING; 
    dom.destcription = "";

    all_dom.push_back(dom);
    all_dom_id++;
}

int CreateDomain(string &name)
{
    if (FindDomain(name) != -1)
    {
        return -1;
    }

    AllocateDomain(name);
    return (all_dom_id-1);
}

bool GetDomainInfo(const string &name, vector<simulate_dom>::iterator &domit)
{
//    vector<simulate_dom>::iterator domit;
    
    for(domit = all_dom.begin(); domit != all_dom.end(); domit++)
    {
        if (domit->dom_name == name)
        {
            return true;
        }
    }
    return false;
}

bool GetDomainInfo(int id, vector<simulate_dom>::iterator &domit)
{
//    vector<simulate_dom>::iterator domit;
    
    for(domit = all_dom.begin(); domit != all_dom.end(); domit++)
    {
        if (domit->dom_id == id)
        {
            return true;
        }
    }
    return false;
}


void EraseDomain(const string &name)
{
    vector<simulate_dom>::iterator domit;
    
    for(domit = all_dom.begin(); domit != all_dom.end(); domit++)
    {
        if (domit->dom_name == name)
        {
            all_dom.erase(domit);
            break;
        }
    }
}

int ShutDownDomain(string &name)
{
    vector<simulate_dom>::iterator pdom;
    
    if (!GetDomainInfo(name, pdom))
    {
        return -1;
    }
    
    EraseDomain(name);
    return 0;
}

int ModifyDomainState(string &name, int state)
{
    vector<simulate_dom>::iterator pdom;
    
    if (!GetDomainInfo(name, pdom))
    {
        return -1;
    }
    
    pdom->state = state;
    return 0;
}

int SuspendDomain(string &name)
{
    return ModifyDomainState(name, LIBV_PAUSED);
}


int ResumeDomain(string &name)
{
    return ModifyDomainState(name, LIBV_RUNNING);
}

int DestroyDomain(string &name)
{
    return ShutDownDomain(name);
}

int GetDomainId(string &name)
{
    vector<simulate_dom>::iterator pdom;
    
    if (!GetDomainInfo(name, pdom))
    {
        return -1;
    }
    
    return pdom->dom_id;
}

int GetDomainName(int id, string &name)
{
    vector<simulate_dom>::iterator pdom;
    
    if (!GetDomainInfo(id, pdom))
    {
        return -1;
    }
    
    name = pdom->dom_name;
    return 0;
}

int GetDomainDesc(string &name, string &desc)
{
    vector<simulate_dom>::iterator pdom;
    
    if (!GetDomainInfo(name, pdom))
    {
        return -1;
    }
    
    desc = pdom->destcription;
    return 0;
}

int SetDomainDesc(string &name, string &desc)
{
    vector<simulate_dom>::iterator pdom;
    
    if (!GetDomainInfo(name, pdom))
    {
        return -1;
    }
    
    pdom->destcription = desc;
    return 0;
}

int GetDomainState(string &name)
{
    vector<simulate_dom>::iterator pdom;
    
    if (!GetDomainInfo(name, pdom))
    {
        return -1;
    }
    
    return pdom->state;
}

void RetDomainInt(int fd, struct sockaddr_in src, int domain_id)
{
    //构建发送内容
    simulate_package temp;

    temp.cmd = SIMULATE_ACK;
    temp.flag = INT_FLAG;
    temp.value_int = domain_id;
    
    sendto(fd, &temp, sizeof(temp), 0, (sockaddr*)&src, sizeof(src));
}

void RetAllDomId(int fd, struct sockaddr_in src)
{
    //构建发送内容
    vector<simulate_dom>::iterator domit;
    simulate_package temp;

    temp.cmd = SIMULATE_ACK;
    temp.flag = INT_ARR_FLAG;
    
    int i;
    for (i = 0; i < MAX_VM_NUM; i++)
    {
        temp.value_arr_int[i] = 0;
    }
    
    i = 0;
    cout << "**************************" << endl;
    for(domit = all_dom.begin(); domit != all_dom.end(); domit++)
    {
        temp.value_arr_int[i] = domit->dom_id;
        i++;
        cout << "name: " << domit->dom_name << "  id: " << domit->dom_id << " desc: " << domit->destcription <<endl;
    }
    cout << "**************************" << endl;

    
    sendto(fd, &temp, sizeof(temp), 0, (sockaddr*)&src, sizeof(src));
}

void RetDomainString(int fd, struct sockaddr_in src, string str)
{
    //构建发送内容
    simulate_package temp;

    temp.cmd = SIMULATE_ACK;
    temp.flag = STRING_FLAG;
    
    memset(temp.value_str, 0, sizeof(temp.value_str));
    memcpy(temp.value_str, str.c_str(), sizeof(temp.value_str) > str.size() ? str.size() : sizeof(temp.value_str));
    
    sendto(fd, &temp, sizeof(temp), 0, (sockaddr*)&src, sizeof(src));
}

void RetDomainFail(int fd, struct sockaddr_in src)
{
    //构建发送内容
    simulate_package temp;

    temp.cmd = SIMULATE_ACK;
    temp.flag = ERR_FLAG;
    cout << "RetDomainFail: " << endl;
     
    sendto(fd, &temp, sizeof(temp), 0, (sockaddr*)&src, sizeof(src));
}

int hyperver_main(int argc, char** argv)
{
    int sockfd;
    int recvlen;
    struct sockaddr_in sin;
    struct sockaddr_in src;

    char recvbuf[1024];
    simulate_package *pRecvFrame;
    
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = 0;
    sin.sin_port = htons(10000);
    
    if (-1 == (sockfd = socket(AF_INET, SOCK_DGRAM, 0)))
    {
        return -1;
    }

    cout << "get socket success!" << endl;
    if (bind(sockfd, (struct sockaddr*)&sin, sizeof(sin)) < 0)
    {
        close(sockfd);
        return -1;
    } 
    
    cout << "bind socket success!" << endl;
    
    while(1)
    {
        socklen_t src_len = sizeof(src);
        if ((recvlen = recvfrom( sockfd, recvbuf, sizeof(recvbuf), 0,
               (struct sockaddr*)&src, &src_len)) <0 )
        {
            printf("hyperver simulate recv fail err %d\r\n", errno);
            return -1;
        }
        
        if (recvlen == 0)
        {
            continue;
        }
        
        pRecvFrame = (simulate_package *)recvbuf;
        cout << "recv cmd: " << pRecvFrame->cmd << " value: " << pRecvFrame->value_str <<endl;
        
        string dom_name = pRecvFrame->value_str;

        switch(pRecvFrame->cmd)
        {
            case TIMER_1S:
            {
                break;
            }
            case CREATE:
            {
                int dom_id = CreateDomain(dom_name);
                RetDomainInt(sockfd, src, dom_id);
                break;
            }
            case STOP:
            {
                RetDomainInt(sockfd, src, ShutDownDomain(dom_name));
                break;
            }
            case PAUSE:
            {
                RetDomainInt(sockfd, src, SuspendDomain(dom_name));
                break;
            }
            case RESUME:
            {
                RetDomainInt(sockfd, src, ResumeDomain(dom_name));
                break;
            }
            case DESTORY:
            {
                RetDomainInt(sockfd, src, DestroyDomain(dom_name));
                break;
            }
            case GET_DOM_NAME:
            {
                int id = pRecvFrame->value_int;
                string name;
                if ( -1 == GetDomainName(id, name))
                {
                    RetDomainFail(sockfd, src);
                }
                else
                {
                    RetDomainString(sockfd, src, name);
                }
                break;
            }
            case GET_DOM_ID: 
            {   
                RetDomainInt(sockfd, src, GetDomainId(dom_name));
                break;
            }
            case GET_DOM_DESCRIPTION:
            {
                string desc;
                string name;
                int id = pRecvFrame->value_int;
                if ( -1 == GetDomainName(id, name))
                {
                    RetDomainFail(sockfd, src);
                }
                else
                {
                    GetDomainDesc(name, desc);
                    RetDomainString(sockfd, src, desc);
                }
                break;
            }
            case GET_DOM_STATE:              
            {
                RetDomainInt(sockfd, src, GetDomainState(dom_name));
                break;
            }
            case REBOOT:
            {
                // 先保留配置
                string desc;
                GetDomainDesc(dom_name, desc);
                
                // 关机
                ShutDownDomain(dom_name);
                // 创建新主机
                int dom_id = CreateDomain(dom_name);
                // 设置之前保留的信息
                SetDomainDesc(dom_name, desc);
                // 返回给客户端
                RetDomainInt(sockfd, src, dom_id);
                break;
            }
            case SET_DOM_DESCRIPTION:
            {
                int id = pRecvFrame->value_int;
                string name;
                if ( -1 == GetDomainName(id, name))
                {
                    RetDomainFail(sockfd, src);
                }
                else
                {   // dom_name 这个是description内容，借用这个名字
                    RetDomainInt(sockfd, src, SetDomainDesc(name, dom_name));
                }
                break;
            }
            case LIST_ALL_DOM_ID:
            {
                RetAllDomId(sockfd, src);
                break;
            }
            case QUERY_SIMULATE_EXIT:
            {
                RetDomainInt(sockfd, src, SIMULATE_EXIT);
                break;
            }
            case QUERY_SIMULATE_EXIT_EX:
            {
                    string value="this is tecs Hyperver_simulater, copy right at zte";
                    RetDomainString(sockfd, src, value);
            }

            default:
                break;
        }
        

    }
    return 0;
}


typedef void *(*ThreadEntry)(void*);   /* 任务入口原型 */

int main(int argc,char **argv)
{
    bool daemon = true;
        
    // Long options
    const struct option long_opt[] =
        {
            { "foreground",  0,  NULL,   'f'},
            { NULL,          0,  NULL,   0}
        };

    int c;
    while ((c = getopt_long (argc, argv, "f", long_opt, NULL)) != -1)
    switch (c)
    {
        case 'f':
            daemon = false;
            break;

            return 0;
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

    hyperver_main(argc, argv);
}


