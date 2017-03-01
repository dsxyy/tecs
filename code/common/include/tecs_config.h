#ifndef TECS_CONFIG_H
#define TECS_CONFIG_H
#include "sky.h"
#include "db_config.h"
#include "ms_config.h"
#include<sys/types.h>
#include<dirent.h>

__attribute__ ((weak)) STATUS SkyInit(const SkyConfig&);

namespace zte_tecs
{
#define DEFAULT_MESSAGE_BROKER_URL      "amqp.tecs.net"
#define DEFAULT_MESSAGE_BROKER_URL2     ""
#define DEFAULT_MESSAGE_BROKER_PORT     5672
#define DEFAULT_MESSAGE_BROKER_OPTION   ""
#define DEFAULT_DB_SERVER_URL           "database.tecs.net"
#define DEFAULT_DB_SERVER_PORT          5432
#define DEFAULT_DB_USER                 "tecs"
#define DEFAULT_DB_PASSWORD             "tecs"
#define DEFAULT_MS_SERV_NOT_CFG         -1
#define DEFAULT_MS_BACKUP_COUNT         0
#define DEFAULT_MS_INST_NOT_CFG         -1
#define DEFAULT_MANAGEMENT_INTERFACE    "eth0"

class TecsConfig :public OptionParser
{
public:
    static TecsConfig* Get()
    {
        if (!instance)
        {
            ostringstream usage;
            ostringstream description;
            string exename = GetNameOfExeFile();
            usage << "Usage: " << exename << " [OPTIONS] ";
            description << "Start TECS with serveral options.";
            instance = new TecsConfig(usage.str(),description.str());
        }
        return instance;
    }

    STATUS Parse(int argc, char** argv);

    ~TecsConfig();

    bool EnableSky()
    {
        Add("application,a", skyconf.messaging.application, "Application name of this exe.");
        //ע�⵱��Ҫʹ�ö���Ϣ������ʱ:message_broker_url����ΪCC��HC֮��ķ�������ַ;
        //message_broker_url2����ΪCC��TC֮��ķ�������ַ�����߲��ɵ���
        Add("message_broker_url,b", skyconf.messaging.connects[0].broker_url, "Message broker url to connect to.");
        Add("message_broker_port", skyconf.messaging.connects[0].broker_port, "Message broker port to connect to.");
        Add("broker_option", skyconf.messaging.connects[0].broker_option, "Message broker connect option.");
        Add("enable_broker_require_ack", skyconf.messaging.connects[0].enable_broker_require_ack, "Message enable broker connect option(bool switch).");
        Add("message_broker_port2", skyconf.messaging.connects[1].broker_port, "The second message broker port to connect to.");
        Add("message_broker_url2", skyconf.messaging.connects[1].broker_url, "The second message broker url to connect to");//zyb
        Add("daemon_output_file",daemon_output_file,"The standard output and error output file when run as daemon.");
        Add("broker_option2", skyconf.messaging.connects[1].broker_option, "message broker connect option2");
        enable_sky = true;
        return true;
    }

    bool EnableDB(const string& schema_update_version = "",const string& schema_bugfix_version= "", bool isAdoDb = false)
    {
        Add("db_server_url", dbconf.server_url, "Database server address of this exe.");
        Add("db_server_port", dbconf.server_port, "Database server port of this exe.");
        Add("db_user", dbconf.user, "User name to access database service.");
        Add("db_passwd", dbconf.passwd, "Password to access database service.");
        Add("db_name", dbconf.database, "Database name used by this exe.");
        Add("ignore_dbschema_version", dbconf.ignore_dbschema_ver, "Whether ignore database schema compatibility? deafult = no");
        dbconf.schema_update_version = schema_update_version;
        dbconf.schema_bugfix_version = schema_bugfix_version;
        enable_db = true;
        is_ado_db = isAdoDb;
        return true;
    }

    bool EnableMs(int serv)
    {
        msconf.backup_serv = serv;
        Add("backup_count", msconf.backup_count, "N of backup 1+N mode.");
        Add("backup_inst", msconf.backup_inst, "Instance number of all backup partners.");
        enable_ms = true;
        return true;
    }

    bool EnableMs()
    {
        Add("backup_serv", msconf.backup_serv, "Serv type of backup.");
        Add("backup_count", msconf.backup_count, "N of backup 1+N mode.");
        Add("backup_inst", msconf.backup_inst, "Instance number of all backup partners.");
        enable_ms = true;
        return true;
    }

    bool EnableCustom(const string& name, string& value, const string& description = string())
    {
        Add(name,value,description);
        return true;
    }

    bool EnableCustom(const string& name, int& value, const string& description = string())
    {
        Add(name,value,description);
        return true;
    }

    bool EnableCustom(const string& name, bool& value, const string& description = string())
    {
        Add(name,value,description);
        return true;
    }
    
    void SetApplication(const string& application)
    {
        skyconf.messaging.application = application;
    }

    void SetProcess(const string& process)
    {
        skyconf.messaging.process = process;
    }

    void ClearConnects(void)
    {
        skyconf.messaging.connects.clear();
    }

    void SetSkyDebugMode(bool debug)
    {
        skyconf.exccfg.debug = debug;
    }

    void SetLastwordsFile(const string& file)
    {
        skyconf.exccfg.lastwords_file = file;
    }

    void SetTimerScanValue(int msecond)
    {
        skyconf.timecfg.tv_msec = msecond;
    }

    void SaveExeName(const string& name)
    {
        exename = name;
    }

    void SaveExePath(const string& name)
    {
        exepath = name.substr(0, name.find_last_of('/'));
    }

    bool IsHelp() const
    {
        return help;
    }

    bool IsSkyEnabled() const
    {
        return enable_sky;
    }

    bool IsDBEnabled() const
    {
        return enable_db;
    }
    bool IsAdoDB() const
    {
        return is_ado_db;
    }

    bool IsMsEnabled() const
    {
        return enable_ms;
    }

    bool IsDaemon() const
    {
        return daemon;
    }

    bool IsDebug() const
    {
        return debug;
    }

    bool IsDllCheckEnabled() const
    {
        return enable_dll_check;
    }
    
    uint32 get_dll_check_time() const
    {
        return dll_check_time;
    }

    const SkyConfig& get_sky_conf() const
    {
        return skyconf;
    }

    const DBConfig& get_db_conf() const
    {
        return dbconf;
    }

    void SetDataBase(const string& dbname)
    {
        dbconf.database = dbname;
    }

    const MsConfig& get_ms_conf() const
    {
        return msconf;
    }

    const string& get_exe_name() const
    {
        return exename;
    }

    const string& get_exe_path() const
    {
        return exepath;
    }

    const string& get_application_name() const
    {
        return skyconf.messaging.application;
    }

    const string& get_process_name() const
    {
        return skyconf.messaging.process;
    }

    void SetBackupInst(int inst)
    {
        msconf.backup_inst = inst;
    }

    int GetBackupInst() const
    {
        return msconf.backup_inst;
    }

    const string& get_config_file() const
    {
        return config_file;
    }

    const string& get_management_interface() const
    {
        return management_interface;
    }

    int get_shell_port() const
    {
        return shell_port;
    }

    const string& get_pidfile() const
    {
        return pidfile;
    }

    const string& get_daemon_out_file() const
    {
        return daemon_output_file;
    }

    //������������������ڲ鿴����ʱ�������
    friend ostream& operator << (ostream& os, const TecsConfig& conf)
    {
        if (conf.enable_sky)
        {
            os << "sky config: " << endl;
            os << conf.skyconf << endl;
        }

        if (conf.enable_db)
        {
            os << "database config: " << endl;
            os << conf.dbconf << endl;
        }

        if (conf.enable_ms)
        {
            os << "ms config: " << endl;
            os << conf.msconf << endl;
        }

        os << "daemon = " << (conf.daemon?"yes":"no") << endl;
        os << "debug = " << (conf.debug?"yes":"no") << endl;
        os << "management_interface = " << conf.management_interface << endl;
        os << "enable_dll_check = " << conf.enable_dll_check << endl;
        os << "dll_check_time = " << conf.dll_check_time << endl;
        return os;
    };

    /* ��ȡshell�ű����Ŀ¼������/opt/tecs/xx/scripts */
    const string& get_scripts_path()
    {
        scriptpath = exepath + "/../scripts/";

        return scriptpath;
    }
    
private:
    static TecsConfig *instance;
    TecsConfig();
    TecsConfig(const string& usage,const string& description);
    /*��ʼ��TecsConfig ���캯���б���*/
    void Init();
    void GetDirFiles(const string &dir,vector<string>&files);

    /*��ȡ��ִ���ļ���ȫ·��*/
    static string GetFullPathOfExeFile()
    {
        /* ��ȡ����exe�ļ�ȫ·������ */
        char buf[256] = {'\0'};

        if (-1 == readlink("/proc/self/exe",buf,sizeof(buf)))
        {
            printf("readlink /proc/self/exe failed! errno = %d\n",errno);
            SkyAssert(0);
            return "";
        }

        return buf;
    }

    /*��ȡ��ִ���ļ���׼ȷ���ƣ�����·���������"/" ������ļ�������*/
    static string GetNameOfExeFile()
    {
        string FullPatchOfExeFile = GetFullPathOfExeFile();

        if (FullPatchOfExeFile.empty())
        {
            return "";
        }
        /*��ȡ���"/" ������ļ�������*/
        return FullPatchOfExeFile.substr(FullPatchOfExeFile.find_last_of('/') + 1);
    }

    DISALLOW_COPY_AND_ASSIGN(TecsConfig);

    bool help;
    bool enable_sky;
    bool enable_db;
    bool is_ado_db;
    bool enable_ms;
    string management_interface;    //TECS����������ʹ�õ�������
    SkyConfig skyconf;              //sky������
    DBConfig  dbconf;               //���ݿ�������
    MsConfig  msconf;               //��������
    string config_file;             //�����ļ�ȫ·��
    string config_file_dir;             //�����ļ�ȫ·��
    bool daemon;                    //�Ƿ����ػ�����ģʽ����
    bool debug;                     //�Ƿ��Ե���ģʽ����
    bool enable_dll_check;          //�Ƿ�ʹ����Ϣ��Ԫ��������ѭ�����    
    int dll_check_time;            //������ѭ�����ļ��ʱ��
    int shell_port;                 //����shell���ԵĶ˿ں�
    string pidfile;
    string exename;
    string exepath;
    string lastwords;
    string daemon_output_file;     //�ں�̨ģʽ����ʱ�ı�׼�������������ļ�����������
    string scriptpath;              //shell�ű����Ŀ¼
};

typedef int MainEntry();
STATUS tecs_init(MainEntry entry = NULL);
void wait_signal_exit();

#define TECS_DEBUG_MODE (TecsConfig::Get()->IsDebug())

//��debug�������ƵĴ�ӡ��
#define TecsDebug(fmt,arg...) \
        do \
        { \
            if(TECS_DEBUG_MODE) \
            { \
                printf(fmt,##arg); \
            } \
        }while(0)
}
using namespace zte_tecs;
#endif

