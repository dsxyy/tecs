/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�log_agent.cpp
* �ļ���ʶ��
* ����ժҪ����־����ģ��ʵ���ļ�
* ��ǰ�汾��1.0
* ��    �ߣ��˺첨
* ������ڣ�2011��9��16��
*
* �޸ļ�¼1��
*     �޸����ڣ�2011/09/16
*     �� �� �ţ�V1.0
*     �� �� �ˣ��˺첨
*     �޸����ݣ�����
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
    // ��־��ӡ�������ò���
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
    /* ��sky��ȡ�ϴ����н���ʱ�����ԣ�����־��¼ */
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

    //���ÿ���̨����Ĵ�ӡ���𣬼���ֵС�ڵ���console_log_level����Ϣ���ᱻ���������̨��
    //Ӧ�ÿɸ�����Ҫ��������SYS_DEBUGΪȱʡֵ��
    pLogAgent->SetLogLevel(DIR_CONSOLE,console_log_level);
    //�����쳣��־����Ĵ�ӡ���𣬼���ֵС�ڵ���exc_log_level����Ϣ���ᱻ���������
    //̨��Ӧ�ÿɸ�����Ҫ��������SYS_ALERTΪȱʡֵ��
    pLogAgent->SetLogLevel(DIR_EXC_LOG,exc_log_level);
    //����ϵͳ��־����������Ĵ�ӡ���𣬵�ǰΪ�رո÷���
    pLogAgent->SetLogLevel(DIR_SYS_LOG,sys_log_level);

    //HC��lastwords��Ҫ�ȵ���ȡ��registered system֮����ܼ�¼
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
* �������ƣ� StartMu
* ���������� ����mu�ĺ���
* ���ʵı� ��
* �޸ĵı� ��
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵����
* �޸�����        �汾��     �޸���       �޸�����
* -----------------------------------------------
* 2012/5         V1.0       lixiaocheng         ����
***************************************************************/
STATUS LogAgent::StartMu(const string& name)
{
    if (_mu)
    {
        SkyAssert(false);
        return ERROR_DUPLICATED_OP;
    }

    // ��Ϣ��Ԫ�Ĵ����ͳ�ʼ��
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

    //���Լ������ϵ���Ϣ����ʹ��Ϣ��Ԫ״̬�л�������̬
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

    if ((iLevel < -1) || (iLevel > SYS_DEBUG)) //����Ϊ-1��-1��ʾ�رո÷�������
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
        //�����쳣��־����
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
        //����ڱ���
        struct stat file_state;
        int ret;
        string file;
        file = _exc_log_path + "/" + msg._pro_name + ".txt";
        //��ȡ�����쳣��־�Ľ������ƣ��Դ�������Ϊ�쳣��־�ļ�������
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

        //д���ļ�
        ofstream ofile;
        ofile.open(file.c_str(),ios::app|ios::out); 
        if (!ofile.is_open())
        {
            //ʧ�����˳�
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
    //todo: ����ϵͳ��־������
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
        //�����յ�����Ϣid���д���
        switch (message.option.id())
        {
        case EV_SYS_LOG:
        {
            SysLogMessage tmpSysLogMsg;
            tmpSysLogMsg.deserialize(message.data);

            if (tmpSysLogMsg._log_level_count <= _console_level)
            {
                //��ӡ������̨
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
    //��δ���
    if ((level < SYS_EMERGENCY) || (level > SYS_DEBUG))
    {
        return ERROR_INVALID_ARGUMENT;
    }

    //��ǰ�����Ϣ�ļ��������κη��������������������
    if ((level > _console_level) && (level > _exclog_level) && (level > _syslog_level))
    {
        return ERROR_INVALID_ARGUMENT;
    }

    //��װ�����Ϣ
    /*   ptLogInfo = malloc(sizeof(LogInfo));
       if (ptLogInfo == NULL)
       {
           return 0;
       }
    */
    //va_start(vList, pcFmt);
    iLogLenth = vsnprintf(actmpLogContent, LOG_LENTH, pcFmt, vList); //��־����
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
                         ptLocalTime->tm_hour, ptLocalTime->tm_min, ptLocalTime->tm_sec, (long int)(tv.tv_usec/1000));  //��־ʱ��

    //iLogLenth = gethostname(actmpHostName, HOST_NAME_LENTH);
    TecsConfig *config = TecsConfig::Get();

    //������־�����߳�
    SysLogMessage tmpSysLogMsg(actmpLogTime,level,
                                ProcessName(),
                                config->get_sky_conf().messaging.application,
                                actmpLogContent);

    MessageOption option(LOG_AGENT,EV_SYS_LOG,0,0);
    //������־��Ϣ
    MessageFrame frame(tmpSysLogMsg,option);
    //����־�����յ���־��Ϣ
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
    
    //��δ���
    if ((level < SYS_EMERGENCY) || (level > SYS_DEBUG))
    {
        return ERROR_INVALID_ARGUMENT;
    }

    //��ǰ�����Ϣ�ļ��������κη��������������������
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
                         ptLocalTime->tm_hour, ptLocalTime->tm_min, ptLocalTime->tm_sec, (long int)(tv.tv_usec/1000));  //��־ʱ��

    TecsConfig *config = TecsConfig::Get();

    //������־�����߳�
    SysLogMessage tmpSysLogMsg(actmpLogTime,level,ProcessName(),
                                config->get_sky_conf().messaging.application,
                                log);

    MessageOption option(LOG_AGENT,EV_SYS_LOG,0,0);
    //������־��Ϣ
    MessageFrame frame(tmpSysLogMsg,option);
    //����־�����յ���־��Ϣ
    Receive(frame);
    //free(ptLogInfo);
    return SUCCESS;
}
}


