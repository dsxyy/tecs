#ifndef TECS_COMMON_MS_CONFIG_H
#define TECS_COMMON_MS_CONFIG_H

struct MsConfig
{
public:
    int     backup_serv;    // 主备的服务类型
    int     backup_count;   // 1+N中N的数量
    int     backup_inst;    // 用户指定的实例号

    friend ostream & operator << (ostream& os, const MsConfig& conf)
    {
        os << "backup_serv  = " << conf.backup_serv << endl;
        os << "backup_count = " << conf.backup_count << endl;
        os << "backup_inst  = " << conf.backup_inst << endl;
        return os;
    };    
};

int MsInit(const MsConfig&) __attribute__ ((weak));
#endif // COMMON_MS_CONFIG_H

