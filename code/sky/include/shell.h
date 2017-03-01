#ifndef SKY_SHELL_H
#define SKY_SHELL_H
#include "pub.h"
#include "sys.h"

STATUS Shell(const char* prompt = "sky-> ");
STATUS Telnetd(bool local,uint16& port,const char* stdout);
STATUS SetTelnetdBanner(const string& banner);

enum SymbolType
{
    SYMBOL_UNKNOWN = 0,
    SYMBOL_CMD = 1,
    SYMBOL_FUNC = 2,
    SYMBOL_VAR = 3
};

class SymbolRegisterer 
{
public:
    SymbolRegisterer(const char* name, SymbolType type, uintptr addr, uint32 len);
};

//定义一个调试函数，在shell中输入时，输入内容必须遵循c语言语法格式
//调试函数的各个参数必须是c语言基本数据类型，不支持c++对象引用参数
#define DEFINE_DEBUG_FUNC(function) \
static SymbolRegisterer o_##function(#function,SYMBOL_FUNC,(uintptr)function,0)

//给调试函数起一个别名
#define DEFINE_DEBUG_FUNC_ALIAS(alias,function) \
static SymbolRegisterer o_##alias(#alias,SYMBOL_FUNC,(uintptr)function,0)

//定义一个调试变量，可以在shell中打印和赋值(输入内容必须遵循c语言语法格式)
//该变量只能是c语言基本数据类型，不支持将c++类对象作为调试变量
#define DEFINE_DEBUG_VAR(variable) \
static SymbolRegisterer o_##variable(#variable,SYMBOL_VAR,(uintptr)(&variable),sizeof(variable))

//定义一个调试命令，在shell中输入时，只需输入命名名称即可，不支持参数，使用起来简单明了
//换句话说，"调试命令" 就是 "没有参数的调试函数"
//调试命令用于一些特殊场合，例如每个进程都支持exit命令，直接在shell下输入exit回车即可退出
#define DEFINE_DEBUG_CMD(command,function) \
static SymbolRegisterer o_##command(#command,SYMBOL_CMD,(uintptr)function,0)

class TelnetServer
{
public:
    TelnetServer();
    ~TelnetServer();
    STATUS Init(bool local,uint16 port,const char* stdout);
    int GetListenPort();
    STATUS SetBanner(const string&);
    STATUS OpenStdout(bool);
    STATUS Serve();
    STATUS Start(int priority = 1);
    STATUS Control(const char* command);
private:
    #define BUFSIZE (4 * 1024)
    STATUS AcceptClient(int sockfd);
    void RefuseClient();
    char* RemoveIAC(uint32 *num);
    string banner;
    string stdout_file;
    int serverfd,clientfd;
    int ptyfd_master,ptyfd_slave;
    int rdidx1, wridx1, size1;
    int rdidx2, wridx2, size2;
    int ctlwritefd,ctlreadfd;
    char ptyname[TTY_NAME_MAX];
    char buf1[BUFSIZE];
    char buf2[BUFSIZE];
    bool busy;
    THREAD_ID server_thread_id;
    struct sockaddr_in server_addr;
};
#endif

