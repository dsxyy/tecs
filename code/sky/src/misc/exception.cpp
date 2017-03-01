/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：exception.cpp
* 文件标识：见配置管理计划书
* 内容摘要：异常处理实现文件
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
#include "common.h"
#include "exception.h"
#include "datetime.h"
#include <ucontext.h>
#include <signal.h>  
#include <execinfo.h>  

typedef struct tagExcSignal
{
    int         iSigNo;     /* 信号值 */
    const char   *pSigName;   /* 信号名称 */
    const char   *pSigDesc;   /* 信号描述 */
    struct sigaction tOldAction;
}T_ExcSignal;

/* 需要接管的异常信号 */
static T_ExcSignal s_tExcSignals[] =
{
    {SIGSEGV,"SIGSEGV","Segmentation fault",{}},
    {SIGILL,"SIGILL","Illegal instruction",{}},
    {SIGFPE,"SIGFPE","Arithmetic exception",{}},
    {SIGBUS,"SIGBUS","Bus error",{}},
    {SIGABRT,"SIGABRT","Aborted",{}},
    {SIGTRAP,"SIGTRAP","Trace/breakpoint trap",{}},
    {SIGUSR1,"SIGUSR1","dead lock or loop",{}}
}; 

static ExceptionConfig s_config;  //本进程异常管理模块的初始化配置参数
static int s_exception_count = 0; //本进程累计发生的异常数量
static vector<string> s_asserts;  //本进程发生的断言记录
extern void DestroyTimerManager();
extern void DestroyMessageAgent();
extern void Delay(uint32 miliseconds);
extern STATUS ExitPointInit();

string getErrnoString(int err, const char* fn)
{
    if (err != 0)
    {
        ostringstream msg;
        msg << fn << ": errno " << err << ": " << strerror(err);
        return msg.str();
    }

    return fn;
}

static void getBacktrace(ostringstream& oss)
{
    /* 获取backtrace信息 */
    int i, num;  
    char **calls;  
    void *traces[1024];  
    num = backtrace(traces, 1024);  
    calls = backtrace_symbols(traces, num); 
    oss << "backtrace: " << endl;
    for (i = 0; i < num; i++)  
    {
        oss << calls[i] << endl;
    }
}

STATUS SaveLastwords(const string& lastwords)
{
    if(s_config.lastwords_file.empty())
    {
        return ERROR_NOT_READY;
    }

    ofstream fout(s_config.lastwords_file.c_str(),ios_base::app);
    if(!fout.is_open())
    {
        return ERROR;
    }

    Datetime dt; 
    fout << "====lastwords at " << dt.tostr() << "====" << endl;
    fout << lastwords;
    fout << "current version was built at " << __DATE__ << "," << __TIME__ << "." << endl;
    return SUCCESS;
}

STATUS GetLastwords(string &lastwords, uint32 maxlen)
{
    if(s_config.lastwords_file.empty())
    {
        return ERROR_NOT_READY;
    }
    
    ifstream fin(s_config.lastwords_file.c_str());
    if(!fin.is_open())
    {
        return ERROR;
    }

    string cut("(too long, ommited ...)\r\n");
    string line;
    while(getline(fin,line)) 
    { 
        if(maxlen && (lastwords.size() + line.size() + cut.size()) >= maxlen)
        {
            lastwords += cut;
            break;
        }    
        //每行开头增加一个tab，便于作为多行日志显示
        lastwords += "\t" + line + "\r\n";
    } 
    
    return SUCCESS;
}

STATUS ClearLastwords()
{
    if(s_config.lastwords_file.empty())
    {
        return ERROR_NOT_READY;
    }
    
    if(access(s_config.lastwords_file.c_str(), F_OK) != 0)
    {
        return SUCCESS;
    }
    
    //先删除，需要用到时再创建，防止临时文件数量太多
    remove(s_config.lastwords_file.c_str());
    return SUCCESS;
}

void ExceptionPrompt(pid_t tid,bool has_lastwords,const string& exc)
{
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGTERM);
    sigprocmask(SIG_UNBLOCK, &mask, NULL);

    ostringstream oss;
    oss << endl;
    oss << "=======================================================" << endl;
    oss << "Warnning! task " << tid << " suspended unexpectedly!" << endl;
    if(has_lastwords)
        oss << "You can use command 'lastwords' to find what happends." << endl;
    else
        oss << exc;
    oss << "=======================================================" << endl;
    SetTelnetdBanner(oss.str());
    
    while(1)
    {
        cerr << oss.str();
        sleep(10);
    }
}

void SignalHandler(int signo, siginfo_t *info, void *context)
{
    ostringstream oss;
    //首先累计一下本进程已经发生的异常数量
    s_exception_count++;

    /* 获取异常现场 */
    ucontext_t *pContext = (ucontext_t *)context;
    pid_t tid = (pid_t)syscall(__NR_gettid);
    uintptr eip = (uintptr)(pContext->uc_mcontext.gregs[REG_RIP]);
    uintptr ebp = (uintptr)(pContext->uc_mcontext.gregs[REG_RBP]);
    string taskname;
    if(tid == getpid())
    {
        taskname.assign("main");
    }
    else
    {
        GetThreadName(pthread_self(),taskname);
    }

    oss << "+-------------------------------------------------------+" << endl; 
    oss << "|                       EXCEPTION                       |" << endl; 
    oss << "+-------------------------------------------------------+" << endl; 
    oss << "Task " << taskname << " (tid = " << tid << ") " 
        << "interrupted by signal " << signo << "!" << endl;
    oss << "eip = 0x" << hex << eip << ", ebp = 0x" << hex << ebp << endl;
    oss << "Fault address(si_addr) = 0x" << hex << info->si_addr << endl;

    /* 获取backtrace信息 */
    getBacktrace(oss);

    /* 保存异常信息 */
    SaveLastwords(oss.str());
        
    /* 打印异常信息 */
    cerr << oss.str();
    if(!s_config.debug)
    {
        //DestroyTimerManager();
        //DestroyMessageAgent();
        exit(-1);
    }

    ExceptionPrompt(tid,true,oss.str());
}

void LoadSignalHandler()
{
    uint32 i = 0;
    struct sigaction act;
    sigset_t newmask,oldmask;
    /* ignore sigpipe in linux, else write to broken socket will cause program exit */
    act.sa_handler = SIG_IGN;
    sigaction(SIGPIPE,&act,0);

    act.sa_sigaction = SignalHandler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_SIGINFO;  
    sigemptyset(&newmask);

    for(i=0;i<sizeof(s_tExcSignals)/sizeof(T_ExcSignal);i++)
    {
        /* 为需要处理的异常信号加载处理函数 */
        sigaction(s_tExcSignals[i].iSigNo,&act,NULL);

        /* 解除异常信号屏蔽，使得LinuxSignalHandler钩子生效 */
        sigaddset(&newmask,s_tExcSignals[i].iSigNo);
    }

    if (0 != pthread_sigmask(SIG_UNBLOCK, &newmask, &oldmask))
    {
        printf("pthread_sigmask failed! errno: %d\n",errno);
    }
}

STATUS ExcInit(const ExceptionConfig& config)
{
    s_config = config;
    LoadSignalHandler();
    return ExitPointInit();
}

bool IsDebug()
{
    return s_config.debug;
}

void assertion_failed(const string& exp, const char *const file,int const line)
{
    string assert_pos = string(file) + ":" + to_string(line,dec);
    vector<string>::iterator result = find(s_asserts.begin( ), s_asserts.end( ),assert_pos); 
    if ( result == s_asserts.end( ) )
    {
        s_asserts.push_back(assert_pos);
    }
    ostringstream oss;
    pid_t tid = (pid_t)syscall(__NR_gettid);
    string taskname;
    if(tid == getpid())
    {
        taskname.assign("main");
    }
    else
    {
        GetThreadName(pthread_self(),taskname);
    }

	Datetime btime(time(0));
    oss << "+-------------------------------------------------------+" << endl; 
    oss << "|                        ASSERT                         |" << endl; 
    oss << "+-------------------------------------------------------+" << endl; 
	oss <<  btime.tostr() << endl;
    oss << "Assert failed in file " << file << " at line " << line << endl; 
    oss << "Assert expression: " << exp << endl; 
    oss << "Assert in task " << taskname << " (tid = " << tid << ")." << endl;

    /* 获取backtrace信息 */
    getBacktrace(oss);

    /* 保存断言信息 */
    //SaveLastwords(oss.str());
    
    /* 打印断言信息 */
    cerr << oss.str();
    if(s_config.debug)
    {
        ExceptionPrompt(tid,false,oss.str());
        //DestroyTimerManager();
        //DestroyMessageAgent();
        //exit(-1);
    }
}

void SkyExit(int code,const string& lastwords)
{
    ostringstream oss;
    pid_t tid = (pid_t)syscall(__NR_gettid);
    string taskname;
    if(tid == getpid())
    {
        taskname.assign("main");
    }
    else
    {
        GetThreadName(pthread_self(),taskname);
    }

    oss << "task " << taskname << " (tid = " << tid << ") gives lastwords: " << endl;
    oss << lastwords << endl;
    
    /* 保存遗言信息 */
    SaveLastwords(oss.str());
    
    /* 打印遗言信息 */
    cerr << oss.str();
    //DestroyTimerManager();
    //DestroyMessageAgent();
    //exit(code);
    cerr << "kill self to exit ...";
    Delay(200);
    kill(getpid(),SIGKILL);
}

STATUS GetAsserts(vector<string>& asserts)
{
    asserts = s_asserts;
    return SUCCESS;
}

int GetExceptionCount()
{
    return s_exception_count;
}

void DbgRaiseSignal()
{
    int *p = 0;
    *p = 0;    
}
DEFINE_DEBUG_FUNC(DbgRaiseSignal);

void DbgShowLastwords()
{
    string lastwords;
    GetLastwords(lastwords,0);
    cout << lastwords;
}
DEFINE_DEBUG_CMD(lastwords,DbgShowLastwords);
    
void DbgSetLastwords(const char* lastwords)
{
    if(!lastwords)
        return;

    SaveLastwords(lastwords);
    cout << lastwords << endl;
}
DEFINE_DEBUG_FUNC(DbgSetLastwords);

void DbgClearLastwords()
{
    ClearLastwords();
}
DEFINE_DEBUG_FUNC(DbgClearLastwords);

void DbgTestAssert()
{
    int a = 1;
    SkyAssert(a == 0);
}
DEFINE_DEBUG_FUNC(DbgTestAssert);

void DbgTestThrowException()
{
    throw 'x';
}
DEFINE_DEBUG_FUNC(DbgTestThrowException);

