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

//����һ�����Ժ�������shell������ʱ���������ݱ�����ѭc�����﷨��ʽ
//���Ժ����ĸ�������������c���Ի����������ͣ���֧��c++�������ò���
#define DEFINE_DEBUG_FUNC(function) \
static SymbolRegisterer o_##function(#function,SYMBOL_FUNC,(uintptr)function,0)

//�����Ժ�����һ������
#define DEFINE_DEBUG_FUNC_ALIAS(alias,function) \
static SymbolRegisterer o_##alias(#alias,SYMBOL_FUNC,(uintptr)function,0)

//����һ�����Ա�����������shell�д�ӡ�͸�ֵ(�������ݱ�����ѭc�����﷨��ʽ)
//�ñ���ֻ����c���Ի����������ͣ���֧�ֽ�c++�������Ϊ���Ա���
#define DEFINE_DEBUG_VAR(variable) \
static SymbolRegisterer o_##variable(#variable,SYMBOL_VAR,(uintptr)(&variable),sizeof(variable))

//����һ�����������shell������ʱ��ֻ�������������Ƽ��ɣ���֧�ֲ�����ʹ������������
//���仰˵��"��������" ���� "û�в����ĵ��Ժ���"
//������������һЩ���ⳡ�ϣ�����ÿ�����̶�֧��exit���ֱ����shell������exit�س������˳�
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

