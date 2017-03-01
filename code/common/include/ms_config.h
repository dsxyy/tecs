#ifndef TECS_COMMON_MS_CONFIG_H
#define TECS_COMMON_MS_CONFIG_H

struct MsConfig
{
public:
    int     backup_serv;    // �����ķ�������
    int     backup_count;   // 1+N��N������
    int     backup_inst;    // �û�ָ����ʵ����

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

