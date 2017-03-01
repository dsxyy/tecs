/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�tecs_init.cpp
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��tecs����ͨ�ó�ʼ��ģ��
* ��ǰ�汾��1.0
* ��    �ߣ����Ľ�
* ������ڣ�2011��08��16��
*
* �޸ļ�¼1��
*     �޸����ڣ�2011/12/01
*     �� �� �ţ�V1.0
*     �� �� �ˣ����Ľ�
*     �޸����ݣ�����
*******************************************************************************/
#include "tecs_config.h"
#include "tecs_pub.h"
#include "light_agent.h"
#include "api_pub.h"

using namespace zte_tecs;

namespace zte_tecs
{
#define TECS_RUNNING_DIR "/var/run/tecs/"
#define TECS_RAMFS_DIR   "/dev/shm/tecs/"
static map<int,string> tmpfiles;

TecsConfig* TecsConfig::instance = NULL;

TecsConfig::TecsConfig(const string& usage,const string& description):
    OptionParser(usage,description)
{
    Init();
}

TecsConfig::TecsConfig(): 
    OptionParser("Usage: exe [OPTIONS]", "Start TECS with serveral options.")
{
    Init();
}

void TecsConfig::Init()
{
    help = false;
    enable_sky = false;
    enable_db = false;
    enable_ms = false;
    daemon = false;     //Ĭ��ֱ����ǰ̨����
    debug = false;      //Ĭ���Էǵ���ģʽ����
    shell_port = 0;     //Ĭ����ϵͳ�Զ�����˿ں�
    enable_dll_check = false;
    dll_check_time = 120;//s

    ConnectConfig connect;
    connect.broker_url = DEFAULT_MESSAGE_BROKER_URL;
    connect.broker_port = DEFAULT_MESSAGE_BROKER_PORT;
    connect.broker_option = DEFAULT_MESSAGE_BROKER_OPTION;
    connect.enable_broker_require_ack = false;
    skyconf.messaging.connects.push_back(connect);
    connect.broker_url = DEFAULT_MESSAGE_BROKER_URL2;
    skyconf.messaging.connects.push_back(connect);
    skyconf.timecfg.tv_msec = DEFAULT_TIMER_OUT_VALUE_MSEC;

    dbconf.server_url = DEFAULT_DB_SERVER_URL;
    dbconf.server_port = DEFAULT_DB_SERVER_PORT;
    dbconf.user = DEFAULT_DB_USER;
    dbconf.passwd = DEFAULT_DB_PASSWORD;
    //Ĭ�ϻ���Ҫ������ݿ�ܹ��汾�����ԣ����ܺ���
    dbconf.ignore_dbschema_ver = false; 

    msconf.backup_serv = DEFAULT_MS_SERV_NOT_CFG;
    msconf.backup_count = DEFAULT_MS_BACKUP_COUNT;
    msconf.backup_inst = DEFAULT_MS_INST_NOT_CFG;

    management_interface = DEFAULT_MANAGEMENT_INTERFACE;

    Add("config_file,f", config_file, "Configuration file used by TECS.");
    Add("config_file_dir,D", config_file_dir, "Configuration file directory used by TECS.");
    Add("daemon,d", daemon, "Run as daemon(bool switch).");
    Add("debug", debug, "Run in debug mode(bool switch).");
    Add("shell_port", shell_port, "Telnet port used by debug shell.");
    Add("pidfile",pidfile,"Path to pid file.");
    Add("management_interface", management_interface, "The TECS management interface.");
    Add("enable_dll_check", enable_dll_check, "Enable dead lock and dead loop check");
    Add("dll_check_time", dll_check_time, "The time of dead lock and dead loop check ");

    return;
}

STATUS TecsConfig::Parse(int argc, char** argv)
{
    /* ��ȡ����exe�ļ�ȫ·������ */
    if ("" == GetFullPathOfExeFile())
    {
        printf("GetFullPathOfExeFile error \n");
        return ERROR;
    }

    SaveExeName(GetFullPathOfExeFile());
    SaveExePath(GetFullPathOfExeFile());

    string filename;
    vector<string> files;
    for (int i = 1; i < argc; ++i)
    {
        string argument = argv[i];
        if (argument == "-h" || argument == "-help"|| argument == "--help" || argument == "-H"  ||argument == "-HELP" ||argument == "--HELP")
        {
            //������ӡ������ʾ
            help = true;
            PrintHelp();
            dbconf.ShowSchemaVer();
            return SUCCESS;
        }

        if (argument == "-f" || argument == "--config_file")
        {
            //�û��Ƿ�ָ���������ļ�
            if (i + 1 < argc)
            {
                filename.assign(argv[++i]);
                if (access(filename.c_str(), F_OK) != 0)
                {
                    printf("configuration file %s does not exist!\n",filename.c_str());
                    exit(-1);
                }
                files.push_back(filename);
            }
            else
            {
                printf("A configuration file path is needed by -f or --config_file option!\n");
                return ERROR;
            }
        }
        else if (argument == "-D" || argument == "--config_file_dir")
        {
            //�û��Ƿ�ָ���������ļ�
            if (i + 1 >= argc)
            {
                printf("A configuration file directory is needed by -D or --config_file_dir option!\n");
                return ERROR;
            }
            string dir_name(argv[++i]);
            GetDirFiles(dir_name,files);
        }
    }
    
    //����û�û��ָ�������ļ�����Ԥ���ĳ������������ļ�
    if (0 == files.size())
    {
        //printf("user does not assign a private conf file!\n");
        filename = "./tecs.conf";
        if (access(filename.c_str(), 0) != 0)
        {
            //printf("pwd does not exist a private conf file!\n");
            //����ڵ�ǰ·����û��tecs.conf����ʹ��/etc/tecs/tecs.conf
            filename = "/etc/tecs/tecs.conf";
        }
        files.push_back(filename);
    }

    vector<string>::const_iterator it;
    for (it = files.begin(); it != files.end(); it++)
    {
        if (it->size() > 5 && it->substr(it->size()-5,it->size()-1) == ".conf")
        {
            if (access(it->c_str(), R_OK) == 0)
            {
                if (false == ParseIniFile(it->c_str(),""))
                {
                    return ERROR;
                }
            }
            else
            {
                printf("failed to read config file %s! %d\n",it->c_str(),errno);
                return ERROR;
            }
        }
    }

    return ParseCmdArgs(argc,argv);
}

void TecsConfig::GetDirFiles(const string &dir,vector<string>&files)
{
    DIR* dir_ptr = opendir(dir.c_str());
    if (NULL == dir_ptr)
    {
        printf("open dir %s failed! errno = %d!\n",dir.c_str(),errno);
        exit(-1);
    }

    struct dirent* entry=NULL;
    while ((entry = readdir(dir_ptr)))
    {
        if (DT_REG == entry->d_type || DT_UNKNOWN == entry->d_type)
        {
            files.push_back(dir + "/" + entry->d_name);
        }
    }
    //�ر���Ӧ��Ŀ¼
    closedir(dir_ptr);
}

/**********************************************************************
* �������ƣ�record_tmp_file
* ������������¼�����̴򿪵���ʱ�ļ�
* ���ʵı���
* �޸ĵı���
* �����������
* �����������
* �� �� ֵ����
* ����˵����
* �޸�����     �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2011/12/01   V1.0    ���Ľ�      ����
************************************************************************/
static void record_tmp_file(int fd,const string& file)
{
    tmpfiles.insert(make_pair(fd,file));
}

/**********************************************************************
* �������ƣ�clear_tmp_file
* �����������رղ�ɾ�������̴򿪵���ʱ�ļ�
* ���ʵı���
* �޸ĵı���
* �����������
* �����������
* �� �� ֵ����
* ����˵����
* �޸�����     �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2011/12/01   V1.0    ���Ľ�      ����
************************************************************************/
static void clear_tmp_file(void)
{
    map<int,string>::iterator it;
    for (it = tmpfiles.begin(); it != tmpfiles.end(); it++)
    {
        close(it->first);
        remove(it->second.c_str());
    }
}

pid_t pidfile_pid(const char *pidfile)
{
    int fd;
    char pidstr[20];
    pid_t pid;
    unsigned int ret;

    fd = open(pidfile, O_NONBLOCK | O_RDONLY, 0600);
    if (fd == -1)
    {
        return 0;
    }

    memset(pidstr,0,sizeof(pidstr));
    if (read(fd, pidstr, sizeof(pidstr)-1) <= 0)
    {
        goto noproc;
    }

    ret = atoi(pidstr);

    if (ret == 0)
    {
        /* Obviously we had some garbage in the pidfile... */
        printf("Could not parse contents of pidfile %s\n",pidfile);
        goto noproc;
    }

    pid = (pid_t)ret;
    if (!(kill(pid,0) == 0 || errno != ESRCH))
    {
        goto noproc;
    }
    if (lockf(fd, F_TEST, 0) == 0)
    {
        printf("Test lock pid file %s! errno = %d,no pid process running!\n", pidfile, errno);
        goto noproc;
    }
    close(fd);
    return (pid_t)ret;

noproc:
    close(fd);
    unlink(pidfile);
    return 0;
}


/**********************************************************************
* �������ƣ�write_pidfile
* ����������д��pid�ļ�
* ���ʵı���
* �޸ĵı���
* �����������
* �����������
* �� �� ֵ����
* ����˵����
* �޸�����     �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2011/12/01   V1.0    ���Ľ�      ����
************************************************************************/
static bool write_pidfile(const char *pidfile,pid_t *oldpid)
{
    char buf[32];

    *oldpid = pidfile_pid(pidfile);
    if (*oldpid != 0)
    {
        printf("file %s exists and process id %d is running.\n",pidfile, (int)*oldpid);
        return false;
    }

    int fd = open(pidfile, O_NONBLOCK |O_RDWR | O_CREAT | O_EXCL, 0600);
    if (fd == -1)
    {
        printf("failed to open pid file %s! errno = %d\n", pidfile, errno);
        return false;
    }

    /* We exit silently if daemon already running. */
    if (lockf(fd, F_TLOCK, 0) == -1)
    {
        printf("failed to lock pid file %s! errno = %d\n", pidfile, errno);
        if (oldpid && read(fd,buf,sizeof(buf)))
        {
            *oldpid = atoi(buf);
        }
        close(fd);
        return false;
    }
    /*
    pid�ļ������ݸ�ʽ���б�׼�涨�ģ��μ�: http://www.pathname.com/fhs/2.2/fhs-5.13.html
    The internal format of PID files remains unchanged. 
    The file must consist of the process identifier in ASCII-encoded decimal, 
    followed by a newline character. For example, if crond was process number 25, 
    /var/run/crond.pid would contain three characters: two, five, and newline. 
    */
    int len = snprintf(buf, sizeof(buf), "%ld\n", (long)getpid());
    if (write(fd, buf, len) != len)
    {
        printf("failed to write pid file %s! errno = %d\n", pidfile, errno);
        close(fd);
        return false;
    }
    fcntl(fd, F_SETFD, FD_CLOEXEC);
    record_tmp_file(fd,pidfile);
    //���ܹر��ļ�����������ʧЧ��!!!
    return true;
}

/**********************************************************************
* �������ƣ�filename_without_path
* ������������ȡ�ļ�ȫ·���е��ļ���������ʼ��ַ
* ���ʵı���
* �޸ĵı���
* �����������
* �����������
* �� �� ֵ����
* ����˵����
* �޸�����     �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2011/12/01   V1.0    ���Ľ�      ����
************************************************************************/
static const char *filename_without_path(const char *path)
{
    if (path == NULL)
    {
        return NULL;
    }

    /* �ҵ�б�ܳ��ֵ����λ�ã�����Ҳ����򷵻�NULL */
    const char *slash = strrchr(path, '/');

    if (!slash || (slash == path && !slash[1]))
    {
        return (char*)path;
    }
    return slash + 1;
}

/**********************************************************************
* �������ƣ�detect_conflict
* ��������������Ƿ���ںͱ�������ͬapplication��process���ƵĽ���
* ���ʵı���
* �޸ĵı���
* �����������
* �����������
* �� �� ֵ����
* ����˵��������������ֶγ�ͻ���ᵼ��ͨ�����⣬��˲�����ͬʱ����
* �޸�����     �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2011/12/01   V1.0    ���Ľ�      ����
************************************************************************/
static bool detect_conflict()
{
    TecsConfig *config = TecsConfig::Get();
    string application = config->get_sky_conf().messaging.application;
    string process = config->get_sky_conf().messaging.process;
    string conflict_file = TECS_RUNNING_DIR + application + "-" + process + ".pid";
    if (access(TECS_RUNNING_DIR, 0) != 0 && 0 != mkdir(TECS_RUNNING_DIR,0600))
    {
        cerr << "failed to make running dir! errno = " << errno << endl;
        return false;
    }

    pid_t oldpid = 0;
    if (false == write_pidfile(conflict_file.c_str(),&oldpid))
    {
        cerr << "Another same process is already running!\n";
        if (oldpid != 0)
        {
            cerr << "The process pid = " << oldpid << endl;
        }

        //���ڳ�ͻ�Ľ��̿��Կ��Ǽ�һ���Զ�ɱ����ѡ��
        //if(false == config->IsKillConflict())
        return false;
    }

    return true;
}

/**********************************************************************
* �������ƣ�start_telnetd
* �����������ڱ�����������telnet����
* ���ʵı���
* �޸ĵı���
* �����������
* �����������
* �� �� ֵ����
* ����˵������
* �޸�����     �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2011/12/01   V1.0    ���Ľ�      ����
************************************************************************/
static bool start_telnetd()
{
    STATUS ret;
    TecsConfig *config = TecsConfig::Get();
    uint16 port = (uint16)config->get_shell_port();
    string daemon_out_file = config->get_daemon_out_file();
    if (daemon_out_file.empty())
        ret = Telnetd(true,port,NULL);
    else
        ret = Telnetd(true,port,daemon_out_file.c_str());

    if (SUCCESS != ret)
    {
        printf("Telnetd failed! ret = %d\n",ret);
        SkyAssert(0);
        return false;
    }

    //��application��process��telnet�˿ںż�¼��telnet file
    char buf[512] = {'\0'};
    string application = config->get_sky_conf().messaging.application;
    string process = config->get_sky_conf().messaging.process;
    snprintf(buf, sizeof(buf), "exe:%s\npid:%ld\napplication:%s\nprocess:%s\nport:%d\n",
             config->get_exe_name().c_str(),
             (long)getpid(),
             application.c_str(),
             process.c_str(),
             port);

    string file = TECS_RUNNING_DIR + application + "-" + process + ".telnet";
    int fd = open(file.c_str(), O_RDWR | O_CREAT, 0600);
    if (fd == -1)
    {
        printf("failed to open file %s! errno = %d\n", file.c_str(), errno);
        return false;
    }

    /* We exit silently if daemon already running. */
    if (lockf(fd, F_TLOCK, 0) == -1)
    {
        printf("failed to lock file %s! errno = %d\n", file.c_str(), errno);
        close(fd);
        return false;
    }

    if (write(fd, buf, sizeof(buf)) != sizeof(buf))
    {
        printf("failed to write file %s! errno = %d\n", file.c_str(), errno);
        close(fd);
        return false;
    }

    record_tmp_file(fd,file.c_str());
    //���ܹر��ļ�����������ʧЧ��!!!
    return true;
}

/**********************************************************************
* �������ƣ�wait_signal_exit
* �����������ȴ��յ��ض����źź��˳�
* ���ʵı���
* �޸ĵı���
* �����������
* �����������
* �� �� ֵ����
* ����˵������
* �޸�����     �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2011/12/01   V1.0    ���Ľ�      ����
************************************************************************/
void wait_signal_exit()
{
    int sig;
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGTERM);
    sigwait(&mask, &sig);
    printf("tecs exits with signal %d!\n",sig);
    TecsConfig *config = TecsConfig::Get();

    if (config->IsSkyEnabled() == true)
    {
        ostringstream oss;
        oss << "process " << config->get_exe_name() <<  " killed by signal " << sig;
        SkyExit(0,oss.str());
    }
    else
    {
        SkyExit(0);
    }
}

/**********************************************************************
* �������ƣ�light_init
* ������������Ƴ�ʼ��
* ���ʵı���
* �޸ĵı���
* �����������
* �����������
* �� �� ֵ����
* ����˵������
* �޸�����     �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2011/12/01   V1.0    ���Ľ�      ����
************************************************************************/
STATUS light_init()
{
    TecsConfig *config = TecsConfig::Get();
    LightAgent *pla = LightAgent::GetInstance();
    if (!pla)
    {
        SkyAssert(0);
        printf("LightAgent::GetInstance failed!");
        return ERROR;
    }
    string socketname;
    string cmd="mkdir -p ";
    cmd.append(LIGHT_AGENT_SOCKET_PATH);
    RunCmd(cmd);
    socketname.assign(LIGHT_AGENT_SOCKET_PATH);
    socketname.append("/");
    socketname.append(config->get_application_name());
    socketname.append("_");
    socketname.append(config->get_process_name());
    socketname.append(".socket");
    cmd = "touch "+socketname;
    RunCmd(cmd);

    if (SUCCESS != pla->Init(socketname.c_str()))
    {
        SkyAssert(0);
        printf("LightAgent::Init failed!");
        return ERROR;
    }
    pla->Run();

    LightAgent *light_agent = LightAgent::GetInstance();
    if (!light_agent)
    {
        return ERROR;
    }

    LightInfo light_info;
    string setter;

    light_info._type = LIGHT_TYPE_HOST;
    light_info._state = LIGHT_RUN_OK;
    light_info._priority = 1;
    setter = "main_of_" + config->get_application_name() + "_" + config->get_process_name();
    strncpy(light_info._setter,setter.c_str(),LI_MAX_SETTER_LEN-1);
    strncpy(light_info._remark,"default run ok",LI_MAX_REMARK_LEN-1);
    if (SUCCESS != light_agent->TurnOn(light_info))
    {
        return ERROR;
    }

    // Ĭ��Ϊ���ã����õ�ƽӿڽ�ACT�����
    light_info._type = LIGHT_TYPE_ACT;
    light_info._state = LIGHT_WORKING;
    light_info._priority = 2;
    setter = "main_of_" + config->get_application_name() + "_" + config->get_process_name();
    strncpy(light_info._setter,setter.c_str(),LI_MAX_SETTER_LEN-1);
    strncpy(light_info._remark,"defualt slave",LI_MAX_REMARK_LEN-1);
    return light_agent->TurnOff(light_info);
}
/**********************************************************************
* �������ƣ�light_master
* �������������õ��
* ���ʵı���
* �޸ĵı���
* �����������
* �����������
* �� �� ֵ����
* ����˵������
* �޸�����     �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2011/12/01   V1.0    ��ΰ      ����
************************************************************************/
STATUS light_master()
{
    TecsConfig *config = TecsConfig::Get();
    LightInfo light_info;
    string setter;

    LightAgent *light_agent = LightAgent::GetInstance();
    if (!light_agent)
    {
        return ERROR;
    }
    // �ɹ���ȡ���ã�����õ�ƽӿڽ�ACT�Ƶ���ɫ
    light_info._type = LIGHT_TYPE_ACT;
    light_info._state = LIGHT_WORKING;
    light_info._priority = 2;
    setter = "main_of_" + config->get_application_name() + "_" + config->get_process_name();
    strncpy(light_info._setter,setter.c_str(),LI_MAX_SETTER_LEN-1);
    strncpy(light_info._remark,"get master",LI_MAX_REMARK_LEN-1);
    return light_agent->TurnOn(light_info);
}
/**********************************************************************
* �������ƣ�tecs_init
* ����������tecs����ͳһ��ں�����������TecsConfig��ʼ�����֮����ܵ���
* ���ʵı���
* �޸ĵı���
* �����������
* �����������
* �� �� ֵ����
* ����˵������
* �޸�����     �汾��  �޸���      �޸�����
* ---------------------------------------------------------------------
* 2011/12/01   V1.0    ���Ľ�      ����
************************************************************************/
STATUS tecs_init(MainEntry entry)
{
    STATUS ret;
    pid_t forkpid;
    TecsConfig *config = TecsConfig::Get();

    if (config->IsDaemon() == true)
    {
        forkpid = fork();
    }
    else
    {
        forkpid = 0; //Do not fork
    }

    if (0 > forkpid)
    {
        cerr << "Error: Unable to fork.\n";
        exit(-1);
    }

    //�������˳�
    if (0 < forkpid)
    {
        exit(0);
    }

    //����exitʱ�������clear_tmp_file
    atexit(clear_tmp_file);

    //����pid�ļ�
    string pid_file = config->get_pidfile();
    if (true == pid_file.empty())
    {
        string proc(filename_without_path(config->get_exe_name().c_str()));
        if (proc.empty())
        {
            cerr << "config->get_exe_name(): " << config->get_exe_name() << endl;
            SkyAssert(false);
            exit(-1);
        }
        //����û�û������ָ��pidfile����Ĭ��ʹ��/var/run/exename.pid
        pid_file = "/var/run/" + proc + ".pid";
    }

    pid_t oldpid = 0;
    if (false == write_pidfile(pid_file.c_str(),&oldpid))
    {
        cerr << "Error: write_pidfile " << pid_file << " failed!\n";
        if (oldpid != 0)
        {
            cerr << "Another process is already running! pid = " << oldpid << endl;
        }
        exit(-1);
    }

    //��ֹ��ͬapplication+process�Ľ����ظ�����
    if (config->IsSkyEnabled())
    {
        //����û�û����дapplication����Ĭ��Ϊ��ǰ��������
        if (config->get_sky_conf().messaging.application.empty())
        {
            char host_name[256] = {0};
            if (0 == gethostname(host_name, 256))
            {
                config->SetApplication(host_name);
            }
        }

        if (false == detect_conflict())
        {
            exit(-1);
        }
    }

    //����Ǻ�̨���У���Ҫ�رձ�׼io
    if (config->IsDaemon())
    {
        pid_t sid = setsid();
        if (sid == -1)
        {
            cerr << "Error: Unable to setsid.\n";
            exit(-1);
        }

        //����ʹ���û�ָ���ĺ�̨����ʱ��׼����ļ�
        int fd = -1;
        string daemon_out_file = config->get_daemon_out_file();
        if (daemon_out_file.empty())
        {
            fd = open("/dev/null", O_RDWR);
            if (fd < 0)
            {
                printf("failed to open /dev/null! errno = %d\n",errno);
                syslog(LOG_CRIT,"failed to open /dev/null! errno = %d pid = %d\n",errno, getpid());
                return ERROR_FILE_NOT_EXIST;
            }
        }
        else
        {
            fd = open(daemon_out_file.c_str(), O_RDWR | O_CREAT);
            if (fd < 0)
            {
                printf("failed to open daemon out file %s! errno = %d\n",daemon_out_file.c_str(),errno);
                syslog(LOG_CRIT,"failed to open daemon out file %s! errno = %d pid = %d\n",daemon_out_file.c_str(),errno,getpid());
                return ERROR_FILE_NOT_EXIST;
            }
        }

        dup2(fd,0);
        dup2(fd,1);
        dup2(fd,2);
        close(fd);

        fcntl(0,F_SETFD,0); // Keep them open across exec funcs
        fcntl(1,F_SETFD,0);
        fcntl(2,F_SETFD,0);
    }


	//����Ǻ�̨���У���Ҫ����telnet��?
	if (config->IsDaemon())
	{
		if (false == start_telnetd())
		{
			SkyExit(-1,"failed to start telnetd!");
		}
	}

    //sky���ʼ��
    if (config->IsSkyEnabled())
    {
        const SkyConfig& skyconf = config->get_sky_conf();

        if (  (!StringCheck::CheckNormalName(skyconf.messaging.application,1, STR_LEN_64))
                ||(!StringCheck::CheckNormalName(skyconf.messaging.process,1, STR_LEN_64)))
        {
            printf("Invalid sky configuration options!!! please check: \n");
            printf("application = %s\n",skyconf.messaging.application.c_str());
            printf("process = %s\n",skyconf.messaging.process.c_str());

            SkyAssert(0);
            return ERROR_INVALID_ARGUMENT;
        }

        if (!skyconf.messaging.connects.empty())
        {
            vector<ConnectConfig>::const_iterator it = skyconf.messaging.connects.begin();
            for (;it != skyconf.messaging.connects.end(); ++it)
            {
                if (!it->broker_url.empty())
                {
                    break;
                }
            }
            if (it->broker_url.empty())
            {
                printf("Can't find broker\n");
                return ERROR_INVALID_ARGUMENT;
            }
        }

        //��sky��ʹ�ú�TECS��ͬ��debugģʽ
        config->SetSkyDebugMode(config->IsDebug());

        //�������Լ�¼�ļ�·��
        if (access(TECS_RAMFS_DIR, 0) != 0 && 0 != mkdir(TECS_RAMFS_DIR,0600))
        {
            cerr << "failed to make ramfs dir! errno = " << errno << endl;
            SkyAssert(0);
            return ERROR;
        }
        config->SetLastwordsFile(TECS_RAMFS_DIR + skyconf.messaging.application + "." +
                                 skyconf.messaging.process + ".lastwords");

        //��ʼ��ʼ��
        cout << "sky initializing ..." << endl;
        cout << skyconf << endl;
        ret = SkyInit(skyconf);
        if (SUCCESS != ret)
        {
            printf("sky init failed! ret = %d\n",ret);
            SkyAssert(0);
            SkyExit(-1, "tecs_init: call SkyInit(skyconf) failed.");
            return ERROR;
        }

        printf("sky init success!\n");
    }


    //���ģ���ʼ��
    ret = light_init();
    if (SUCCESS != ret)
    {
        printf("light init failed! ret = %d\n",ret);
        SkyAssert(0);
        SkyExit(-1,"light init failed!");
        return ERROR;
    }

    //������ʼ��
    if (config->IsMsEnabled())
    {
        const MsConfig& msconf = config->get_ms_conf();
        if ((DEFAULT_MS_SERV_NOT_CFG != msconf.backup_serv) && \
                (0 != msconf.backup_count))
        {
            ret = MsInit(msconf);
            if (SUCCESS != ret)
            {
                printf("ms init failed! ret = %d\n",ret);
                SkyAssert(0);
                exit(-1);
                return ret;
            }
            else
            {
                printf("ms init success!\n");
            }
        }
    }

    ret = light_master();
    if (SUCCESS != ret)
    {
        printf("light master failed! ret = %d\n",ret);
        SkyAssert(0);
        SkyExit(-1,"light master failed!");
        return ERROR;
    }

    //���ݿ��ʼ��
    if (config->IsDBEnabled())
    {
        const DBConfig& dbconf = config->get_db_conf();

        if (dbconf.database.empty() && config->IsSkyEnabled())
        {
            config->SetDataBase(ApplicationName());
        }

        if (dbconf.server_url.empty() ||
                dbconf.database.empty())
        {
            printf("server = %s\n",dbconf.server_url.c_str());
            printf("database = %s\n",dbconf.database.c_str());
            printf("db config not completed! ret = %d\n",ret);
            SkyAssert(0);
            SkyExit(-1,"db config not completed!");
            return ERROR_INVALID_ARGUMENT;
        }

        if( config->IsAdoDB())
        {
            ret = DBInitAdo(dbconf);
        }
        else
        {
            ret = DBInit(dbconf);
        }
        if (SUCCESS != ret)
        {
            printf("db init failed! ret = %d\n",ret);
            SkyAssert(0);
            SkyExit(-1,"db init failed!");
            return ERROR;
        }
        else
        {
            printf("db init success!\n");
        }
    }
    ThreadDllMonitorInit(config->IsDllCheckEnabled(),config->get_dll_check_time());
    //��ʼ���ñ�����������main���
    if (entry)
    {
        entry();
    }
    return SUCCESS;
}

//�鿴tecs����ʱʵ����Ч�����ã�ֻ����TecsConfig���ѡ��
void DbgShowTecsConfig()
{
    TecsConfig *config = TecsConfig::Get();
    if (config)
    {
        cout << *config << endl;
    }
}
DEFINE_DEBUG_CMD(tecsconfig,DbgShowTecsConfig);

//�鿴tecs����ʱʵ����Ч�����ã������û��Լ�������Զ���ѡ��
void DbgShowTecsConfigAll()
{
    TecsConfig *config = TecsConfig::Get();
    if (config)
    {
        config->PrintAll();
    }
}
DEFINE_DEBUG_CMD(tecsconfigall,DbgShowTecsConfigAll);
}

