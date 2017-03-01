/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：log_agent.cpp
* 文件标识：
* 内容摘要：日志代理模块实现文件
* 当前版本：1.0
* 作    者：邓红波
* 完成日期：2011年9月16日
*
* 修改记录1：
*     修改日期：2011/09/16
*     版 本 号：V1.0
*     修 改 人：邓红波
*     修改内容：创建
*******************************************************************************/
#include "sky.h"
#include "tecs_config.h"
#include "mid.h"
#include "event.h"
#include "log_agent.h"
#include "log_common.h"
#include "registered_system.h"
using namespace zte_tecs;

namespace zte_tecs
{

#define USR_LEVEL              1
#define LOCAL_TIME_LENTH 32
#define LOG_LEVEL_LENTH   8
#define LOG_LENTH              (1024*4)
#define PROC_NAME_LENTH  256
#define HOST_NAME_LENTH  64
#define EXC_LOG_PATH_NAME_LENTH  1024
#define EXC_LOG_FILE_SIZE  (1024*1024)

static int console_log_level = SYS_DEBUG;
static int exc_log_level = SYS_ALERT;
static int sys_log_level = SYS_OUTPUT_CLOSE;
DEFINE_DEBUG_VAR(console_log_level);
DEFINE_DEBUG_VAR(exc_log_level);
DEFINE_DEBUG_VAR(sys_log_level);

class LogAgent: public MessageHandler
{
public:
    static LogAgent *GetInstance()
    {
        if (NULL == _instance)
        {
            _instance = new LogAgent();
        }

        return _instance;
    };

    ~LogAgent();
    STATUS Init()
    {
        _exc_log_path = LOCAL_LOG_PATH;
        if (0 != access(_exc_log_path.c_str(), 0))
        {
            STATUS ret;
            string cmd;
            cmd = "mkdir -p "+ _exc_log_path;
            cout << cmd <<endl;
            ret = RunCmd(cmd);
            if (SUCCESS != ret)
            {
                printf("log init fail\n");
                return ret;
            }
        }
        return StartMu(LOG_AGENT);
    }

    void SetHc(int iHc);
    int SetLogLevel(int iOutPutDir,int iLevel);
    int GetLogLevel(int iOutPutDir);
    STATUS OutPutLog(int iLevel,const char* pcFmt,va_list vList);
    STATUS OutPutLog(int level, const string& log);
    STATUS Receive(const MessageFrame& message);
    STATUS DoExcLog(const SysLogMessage&);
    STATUS DoSysLog(const SysLogMessage&);
    void MessageEntry(const MessageFrame& message);

private:
    LogAgent();
    STATUS StartMu(const string& name);
    int _console_level;
    int _exclog_level;
    int _syslog_level;
    int _is_hc;
    int _syslog_svr_ip;
    MessageUnit *_mu;
    string _exc_log_path;
    char acExcLogPath[EXC_LOG_PATH_NAME_LENTH];
    static LogAgent *_instance;
};

LogAgent *LogAgent::_instance = NULL;

STATUS OutPut(int level, const char* pcFmt,...)
{
    va_list     vList;
    STATUS ret;
    LogAgent *pLogAgent = LogAgent::GetInstance();

    va_start(vList, pcFmt);
    ret = pLogAgent->OutPutLog(level, pcFmt, vList);
    va_end(vList);
    return ret;
}

STATUS OutPut(int level, const string& log)
{
    LogAgent *pLogAgent = LogAgent::GetInstance();
    return pLogAgent->OutPutLog(level, log);
}

STATUS OutPut(int level, const ostringstream& log)
{
    LogAgent *pLogAgent = LogAgent::GetInstance();
    return pLogAgent->OutPutLog(level, log.str());
}

STATUS SetLevel(int iOutPutDir,int level)
{
    LogAgent *pLogAgent = LogAgent::GetInstance();
    return pLogAgent->SetLogLevel(iOutPutDir, level);
}

STATUS GetLevel(int iOutPutDir)
{
    LogAgent *pLogAgent = LogAgent::GetInstance();
    return pLogAgent->GetLogLevel(iOutPutDir);
}

STATUS EnableLogOptions()
{
    TecsConfig *config = TecsConfig::Get();
    // 日志打印级别配置参数
    config->EnableCustom("console_log_level", console_log_level,
                         "the levels below will be printed out to console, default is SYS_DEBUG = 7.");
    config->EnableCustom("exc_log_level", exc_log_level,
                         "the levels below will be saved to exception file, default is SYS_ALERT = 1.");
    config->EnableCustom("sys_log_level", sys_log_level,
                         "the levels below will be saved to system log, default is closed.");
    return SUCCESS;
}

STATUS LogLastwords()
{
    /* 从sky获取上次运行结束时的遗言，用日志记录 */
    string lastwords;
    if (SUCCESS != GetLastwords(lastwords,2048))
    {
        return ERROR;
    }

    ClearLastwords();
    if (lastwords.empty())
    {
        return SUCCESS;
    }

    time_t tCurrentTime;
    struct tm tLocalTime;
    struct tm* ptLocalTime = &tLocalTime;
    char actmpLogTime[LOCAL_TIME_LENTH] = {'\0'};

    tCurrentTime = time(NULL);
    ptLocalTime = gmtime(&tCurrentTime);
    snprintf(actmpLogTime, LOCAL_TIME_LENTH, "%04d/%02d/%02d %02d:%02d:%02d:%03ld",
             ptLocalTime->tm_year + 1900, ptLocalTime->tm_mon + 1, ptLocalTime->tm_mday,
             ptLocalTime->tm_hour, ptLocalTime->tm_min, ptLocalTime->tm_sec, (long int)0);

    //gethostname(actmpHostName, HOST_NAME_LENTH);
    TecsConfig *config = TecsConfig::Get();

    SysLogMessage tmpSysLogMsg(actmpLogTime,SYS_ALERT,
        ProcessName(),
        config->get_sky_conf().messaging.application,
        "lastwords:\n"+lastwords);
    
    MessageOption option(LOG_AGENT,EV_LASTWORDS_LOG,0,0);
    MessageFrame frame(tmpSysLogMsg,option);
    LogAgent *pLogAgent = LogAgent::GetInstance();
    if (!pLogAgent)
        return ERROR;

    pLogAgent->Receive(frame);
    return SUCCESS;
}

STATUS LogInit(int iHc)
{
    LogAgent *pLogAgent = LogAgent::GetInstance();
    if (!pLogAgent)
    {
        return ERROR_NO_MEMORY;
    }
    STATUS ret = pLogAgent->Init();
    if (SUCCESS != ret)
    {
        return ret;
    }

    pLogAgent->SetHc(iHc);

    //设置控制台方向的打印级别，级别值小于等于console_log_level的信息将会被输出到控制台。
    //应用可根据需要调整级别。SYS_DEBUG为缺省值。
    pLogAgent->SetLogLevel(DIR_CONSOLE,console_log_level);
    //设置异常日志方向的打印级别，级别值小于等于exc_log_level的信息将会被输出到控制
    //台。应用可根据需要调整级别。SYS_ALERT为缺省值。
    pLogAgent->SetLogLevel(DIR_EXC_LOG,exc_log_level);
    //设置系统日志服务器方向的打印级别，当前为关闭该方向。
    pLogAgent->SetLogLevel(DIR_SYS_LOG,sys_log_level);

    //HC的lastwords需要等到获取到registered system之后才能记录
    if (!iHc)
        LogLastwords();

    return SUCCESS;
}

LogAgent::LogAgent()
{
    _console_level = SYS_DEBUG;
    _exclog_level = SYS_ALERT;
    _syslog_level = SYS_OUTPUT_CLOSE;
    _mu = NULL;
}

/************************************************************
* 函数名称： StartMu
* 功能描述： 启动mu的函数
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2012/5         V1.0       lixiaocheng         创建
***************************************************************/
STATUS LogAgent::StartMu(const string& name)
{
    if (_mu)
    {
        SkyAssert(false);
        return ERROR_DUPLICATED_OP;
    }

    // 消息单元的创建和初始化
    _mu = new MessageUnit(name);
    if (!_mu)
    {
        OutPut(SYS_EMERGENCY, "Create mu %s failed!\n",name.c_str());
        return ERROR_NO_MEMORY;
    }

    STATUS ret = _mu->SetHandler(this);
    if (SUCCESS != ret)
    {
        OutPut(SYS_EMERGENCY, "SetHandler mu %s failed! ret = %d\n",name.c_str(),ret);
        return ret;
    }

    ret = _mu->Run();
    if (SUCCESS != ret)
    {
        OutPut(SYS_EMERGENCY, "Run mu %s failed! ret = %d\n",name.c_str(),ret);
        return ret;
    }

    ret = _mu->Register();
    if (SUCCESS != ret)
    {
        OutPut(SYS_EMERGENCY, "Register mu %s failed! ret = %d\n",name.c_str(),ret);
        return ret;
    }

    //给自己发送上电消息，促使消息单元状态切换到工作态
    MessageFrame message(SkyInt32(0), EV_POWER_ON);
    ret = _mu->Receive(message);
    if (SUCCESS != ret)
    {
        SkyAssert(0);
        return ret;
    }
    _mu->Print();
    return SUCCESS;
}

LogAgent::~LogAgent()
{
    delete _mu;
    LogAgent::_instance = NULL;
}

int LogAgent::SetLogLevel(int iOutPutDir,int iLevel)
{
    if ((iOutPutDir != DIR_CONSOLE) && (iOutPutDir != DIR_EXC_LOG) && (iOutPutDir != DIR_SYS_LOG))
    {
        cout << "input parameter: iOutPutDir = " << iOutPutDir << "is not correct!" << endl;
        return ERROR;
    }

    if ((iLevel < -1) || (iLevel > SYS_DEBUG)) //允许为-1，-1表示关闭该方向的输出
    {
        cout << "input parameter: iLevel = " << iLevel << "is not correct!" << endl;
        return ERROR;
    }

    switch (iOutPutDir)
    {
    case DIR_CONSOLE:
        _console_level = iLevel;
        break;

    case DIR_EXC_LOG:
        _exclog_level = iLevel;
        break;

    case DIR_SYS_LOG:
        _syslog_level = iLevel;
        break;

    default:
        return ERROR;

    }
    return SUCCESS;

}

void LogAgent::SetHc(int iHc)
{
    _is_hc = iHc;
    return;
}

int LogAgent::GetLogLevel(int iOutPutDir)
{
    int iReturnDir;
    if ((iOutPutDir != DIR_CONSOLE) && (iOutPutDir != DIR_EXC_LOG) && (iOutPutDir != DIR_SYS_LOG))
    {
        cout << "input parameter: iOutPutDir = " << iOutPutDir << "is not correct!" << endl;
        return -1;
    }

    switch (iOutPutDir)
    {
    case DIR_CONSOLE:
        iReturnDir = _console_level;
        break;

    case DIR_EXC_LOG:
        iReturnDir = _exclog_level;
        break;

    case DIR_SYS_LOG:
        iReturnDir = _syslog_level;
        break;

    default:
        return -1;

    }

    return iReturnDir;
}

STATUS LogAgent::Receive(const MessageFrame& message)
{
    return _mu->Receive(message);
};

STATUS LogAgent::DoExcLog(const SysLogMessage& msg)
{
    if (_is_hc != 0)
    {
        //发往异常日志代理
        string tmpCluster;
        if (SUCCESS == GetRegisteredSystem(tmpCluster))
        {
            MID receiver(tmpCluster.c_str(), PROC_CC, EXC_LOG_AGENT);
            MessageOption option(receiver,EV_EXC_LOG,0,0);
            //cout << "GetRegisteredSystem success, exc log send to " << tmpCluster << endl;
            //cout << msg._log_content << endl;
            return _mu->Send(msg,option);
        }
        else
        {
            cerr << "Failed to GetRegisteredSystem, exc log deserted!" << endl;
            cerr << msg._log_content << endl;
        }
        return ERROR;
    }
    else
    {
        //存放在本地
        struct stat file_state;
        int ret;
        string file;
        file = _exc_log_path + "/" + msg._pro_name + ".txt";
        //获取产生异常日志的进程名称，以此名称作为异常日志文件的名称
        ret = stat(file.c_str(),&file_state);
        if (ret == 0)
        {
            if (file_state.st_size >=  EXC_LOG_FILE_SIZE)//EXC_LOG_FILE_SIZE
            {
                string file_bak;
                file_bak=file+".bak";
                unlink(file_bak.c_str());
                rename(file.c_str(),file_bak.c_str());
            }
        }

        //写入文件
        ofstream ofile;
        ofile.open(file.c_str(),ios::app|ios::out); 
        if (!ofile.is_open())
        {
            //失败则退出
            cerr << "Error opening out file!errno =" << errno << endl;
            return ERROR;
        }
        ofile << "<" << msg._log_level_count << ">" \
              << msg._log_time << " " \
              << msg._pro_name << ": " \
              << msg._log_content \
              << endl;
        ofile.close();

        return SUCCESS;
    }
}

STATUS LogAgent::DoSysLog(const SysLogMessage& msg)
{
    //todo: 发往系统日志服务器
    return SUCCESS;
}

void LogAgent::MessageEntry(const MessageFrame& message)
{
    switch (_mu->get_state())
    {
    case S_Startup:
    {
        switch (message.option.id())
        {
        case EV_POWER_ON:
        {
            _mu->set_state(S_Work);
            OutPut(SYS_NOTICE, "%s power on!\n",_mu->name().c_str());
            break;
        }
        default:
            OutPut(SYS_NOTICE, "Ignore any message except power on!\n");
            break;
        }
        break;
    }

    case S_Work:
    {
        //根据收到的消息id进行处理
        switch (message.option.id())
        {
        case EV_SYS_LOG:
        {
            SysLogMessage tmpSysLogMsg;
            tmpSysLogMsg.deserialize(message.data);

            if (tmpSysLogMsg._log_level_count <= _console_level)
            {
                //打印到控制台
                printf("<%d>%s %s %s: %s",
                       tmpSysLogMsg._log_level_count,
                       tmpSysLogMsg._log_time.c_str(),
                       tmpSysLogMsg._host_name.c_str(),
                       tmpSysLogMsg._pro_name.c_str(),
                       tmpSysLogMsg._log_content.c_str());
            }

            if (tmpSysLogMsg._log_level_count <= _exclog_level)
            {
                DoExcLog(tmpSysLogMsg);
            }

            if (tmpSysLogMsg._log_level_count <= _syslog_level)
            {
                DoSysLog(tmpSysLogMsg);
            }
            break;
        }

        case EV_LASTWORDS_LOG:
        {
            SysLogMessage lastwords;
            lastwords.deserialize(message.data);
            DoExcLog(lastwords);
            DoSysLog(lastwords);
            break;
        }

        default:
            return;
        }
        break;
    }

    default:
        break;
    }
    return;
}

STATUS LogAgent::OutPutLog(int level, const char* pcFmt,va_list vList)
{
    int      iLogLenth = 0;
    time_t      tCurrentTime;
    struct tm          tLocalTime;
    struct tm  * ptLocalTime = &tLocalTime;

    char actmpLogTime[LOCAL_TIME_LENTH] = {'\0'};
    char actmpLogContent[LOG_LENTH] = {'\0'};
    //入参错误
    if ((level < SYS_EMERGENCY) || (level > SYS_DEBUG))
    {
        return ERROR_INVALID_ARGUMENT;
    }

    //当前输出信息的级别不满足任何方向允许的输出级别，则不输出
    if ((level > _console_level) && (level > _exclog_level) && (level > _syslog_level))
    {
        return ERROR_INVALID_ARGUMENT;
    }

    //组装输出信息
    /*   ptLogInfo = malloc(sizeof(LogInfo));
       if (ptLogInfo == NULL)
       {
           return 0;
       }
    */
    //va_start(vList, pcFmt);
    iLogLenth = vsnprintf(actmpLogContent, LOG_LENTH, pcFmt, vList); //日志内容
    //va_end(vList);

    if (iLogLenth == 0)
    {
        return ERROR;
    }

    struct timeval tv;
    gettimeofday(&tv,NULL);
    //tCurrentTime = time(NULL);
    tCurrentTime = tv.tv_sec;
    ptLocalTime = gmtime(&tCurrentTime);
    iLogLenth = snprintf(actmpLogTime, LOCAL_TIME_LENTH, "%04d/%02d/%02d %02d:%02d:%02d:%03ld",
                         ptLocalTime->tm_year + 1900, ptLocalTime->tm_mon + 1, ptLocalTime->tm_mday,
                         ptLocalTime->tm_hour, ptLocalTime->tm_min, ptLocalTime->tm_sec, (long int)(tv.tv_usec/1000));  //日志时间

    //iLogLenth = gethostname(actmpHostName, HOST_NAME_LENTH);
    TecsConfig *config = TecsConfig::Get();

    //发往日志代理线程
    SysLogMessage tmpSysLogMsg(actmpLogTime,level,
                                ProcessName(),
                                config->get_sky_conf().messaging.application,
                                actmpLogContent);

    MessageOption option(LOG_AGENT,EV_SYS_LOG,0,0);
    //构造日志消息
    MessageFrame frame(tmpSysLogMsg,option);
    //让日志代理收到日志消息
    Receive(frame);
    //free(ptLogInfo);
    return SUCCESS;
}

STATUS LogAgent::OutPutLog(int level, const string& log)
{
    time_t      tCurrentTime;
    struct tm   tLocalTime;
    struct tm   *ptLocalTime = &tLocalTime;

    char actmpLogTime[LOCAL_TIME_LENTH] = {'\0'};
    
    //入参错误
    if ((level < SYS_EMERGENCY) || (level > SYS_DEBUG))
    {
        return ERROR_INVALID_ARGUMENT;
    }

    //当前输出信息的级别不满足任何方向允许的输出级别，则不输出
    if ((level > _console_level) && (level > _exclog_level) && (level > _syslog_level))
    {
        return ERROR_INVALID_ARGUMENT;
    }

    if (log.empty())
    {
        return ERROR;
    }

    struct timeval tv;
    gettimeofday(&tv,NULL);
    //tCurrentTime = time(NULL);
    tCurrentTime = tv.tv_sec;
    ptLocalTime = gmtime(&tCurrentTime);
    snprintf(actmpLogTime, LOCAL_TIME_LENTH, "%04d/%02d/%02d %02d:%02d:%02d:%03ld",
                         ptLocalTime->tm_year + 1900, ptLocalTime->tm_mon + 1, ptLocalTime->tm_mday,
                         ptLocalTime->tm_hour, ptLocalTime->tm_min, ptLocalTime->tm_sec, (long int)(tv.tv_usec/1000));  //日志时间

    TecsConfig *config = TecsConfig::Get();

    //发往日志代理线程
    SysLogMessage tmpSysLogMsg(actmpLogTime,level,ProcessName(),
                                config->get_sky_conf().messaging.application,
                                log);

    MessageOption option(LOG_AGENT,EV_SYS_LOG,0,0);
    //构造日志消息
    MessageFrame frame(tmpSysLogMsg,option);
    //让日志代理收到日志消息
    Receive(frame);
    //free(ptLogInfo);
    return SUCCESS;
}
}


