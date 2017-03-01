#ifndef SKY_SYS_H
#define SKY_SYS_H
#include "pub.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>
#include <iostream>
#include <sstream>
#include <fstream> 
#include <string>
#include <vector>
#include <dirent.h>
#include <linux/hdreg.h> 
#include <scsi/sg.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <algorithm>

typedef pthread_t THREAD_ID;
#define INVALID_THREAD_ID 0
typedef void *(*ThreadEntry)(void*);   /* 任务入口原型 */

class Thread
{
public:
    Thread(const string& thread_name = "Thread"):name(thread_name)
    {
        m_arg = NULL;
        priority = 1;
        tid = INVALID_THREAD_ID;
    };
    
    virtual ~Thread() 
    {
        Stop();
    }

    int Start(void * arg);
    int Stop();
    THREAD_ID Id();
    
protected:
    static void * EntryPoint(void*);
    int Run(void * arg);
    virtual void Setup() {};
    virtual void Execute(void*) {};
    void* Arg() const {return m_arg;}
    void Arg(void* a){m_arg = a;}
    
private:
    string name;
    int priority;
    THREAD_ID tid;
    void* m_arg;
};

class Mutex
{
public:
    Mutex(const char *name = NULL)
    {
        if(!name)
            _name = "noname";
        else
            _name= name;
            
        _init = false;
        _debug = false;
    };
    
    virtual ~Mutex()
    {
        if(_init)
            pthread_mutex_destroy(&_mutex);
    };
    
    STATUS Init();
    void SetDebug(bool mode)         { _debug = mode; };
    void SetName(const char *name)   { _name = name; };
    void Lock(void);
    bool TryLock(void);
    void Unlock(void);    

private:
    bool            _init;
    bool            _debug;
    string          _name;
    pthread_mutex_t _mutex;
};

class MutexLock
{
public:
    MutexLock( Mutex& _mutex ) : mutex( _mutex )
    { mutex.Lock(); }

    ~MutexLock()
    { mutex.Unlock(); }
private:
    Mutex& mutex;
};

class Semaphore
{
public:
    Semaphore(unsigned int initial = 0);
    ~Semaphore();
    void wait();
    bool tryWait();
    void post();
private:
    sem_t _sem;
};

class Worktodo
{
public:
    string _name;
    string getName(){return _name;};
    STATUS virtual Cancel()
	{
		return SUCCESS;
	}
    STATUS virtual Execute()
	{
	    printf("work %s has nothing to do!\n",_name.c_str());
		return SUCCESS;
	}
    Worktodo(){};
    Worktodo(string name) : _name(name) {};
    virtual ~Worktodo(){}
};

class ThreadPool
{
public:
    ThreadPool();
    ThreadPool(int maxThreadsTemp);
    virtual ~ThreadPool();
	void DestroyPool(int maxPollSecs=2);
    STATUS AssignWork(Worktodo *work);
    STATUS FetchWork(Worktodo **work);
    STATUS CancelWork(const string &name);
    bool FindWork(const string &name);
	STATUS CreateThreads(const char *name = "Worker",int priority = 1);
    static void *ExecuteEntry(void *arg);

    void ShowInfo(string filter);

    enum Log_Type
    {
        Enable,
        Disable
    };
    void SetLog(Log_Type log)
    {
        _log = log;
    }

private:
    
    int  CountOfWorks();
    //sem_t _available_work;
    pthread_cond_t  _cond_available_work;
    pthread_mutex_t _mutex_sync;
    //pthread_mutex_t _mutex_work_completion;

    int _num_of_threads;
    list <Worktodo *> _works_of_waiting;
    map<pthread_t, Worktodo*> _works_in_runing; // 正在运行的任务
    Log_Type _log;
    string _name;
};

// 将SRIOV 私有结构体移出来
/* ioctl命令，不能和内核命令冲突(内核私有命令为0x89f0-0x89ff) */
#define SIOCDEVPRIINFO   0x89FE  
#define PRIVATE_SETLOOP  0x01      /* 用于设置网口环回模式 */
#define PRIVATE_GETLOOP  0x02      /* 用于查询网口环回模式 */
#define PRIVATE_SETVF    0x03      /* 用于设置网口VF data(=1开启; =0关闭) */
#define PRIVATE_GETVF    0x04      /* 用于查询网口VF data(=1开启; =0关闭) */

// PRIVATE_SETVF  PRIVATE_GETVF 命令Data Value
#define PRIVATE_VF_VALUE_CLOSE 0x0
#define PRIVATE_VF_VALUE_OPEN  0x01

#define STR_BOND_MODE_BALANCE "load balancing"
#define STR_BOND_MODE_BACKUP  "fault-tolerance (active-backup)"
#define STR_BOND_MODE_802     "IEEE 802.3ad Dynamic link aggregation"

/* SIOCDEVPRIINFO命令对应的私有结构 */
struct private_value{
    unsigned int cmd;
    unsigned int data;
};

// 定义bond信息结构体
typedef struct BondLacpInfo
{
    bool  status;      // = true,success; = false failed.
    string aggregator_id;
    string partner_mac; // AA:BB:CC:DD:EE:FF
    vector<string> nic_suc; //聚合成功NICs
    vector<string> nic_fail;//聚合失败NICs
}T_BOND_LACP_INFO;
typedef struct BondBackupInfo
{
    string active_nic;         //主用Nic
    vector<string> other_nic;  //备用NICs
}T_BOND_BACKUP_INFO;

enum En_Bond_Mode
{
    EN_BOND_MODE_BALANCE = 0,
    EN_BOND_MODE_BACKUP  = 1,
    EN_BOND_MODE_802     = 4,
};

enum En_Bond_Err
{
    EN_BOND_ERR_FILE_NO_EXIST = 1,
    EN_BOND_ERR_GET_MODE_FAIL = 2,
    EN_BOND_ERR_MODE_INVALID  = 3,
    EN_BOND_ERR_RUNCMD_FAIL   = 4,
    EN_BOND_ERR_LACP_RESULT_INVALID   = 5,    
    EN_BOND_ERR_GET_BOND_FILE_CONTENT_FAILED  = 6,   
    EN_BOND_ERR_GET_BOND_ITEM_FAILED  = 7,  
    //EN_BOND_ERR_BACKUP_NO_ACTIVE_NIC   = 4,    
};

class CBondInfo
{
public:
    CBondInfo(){};
    virtual ~CBondInfo(){};
    void Init()
    {        
        mode = -1; 
        lacp.aggregator_id = lacp.partner_mac = "";
        lacp.status = FALSE;
        lacp.nic_fail.clear();
        lacp.nic_suc.clear();       
        backup.active_nic = "";
        backup.other_nic.clear();
    }
    void print()
    {
        cout << "name: " << name << endl;
        cout << "  mode: " << mode << endl;
        cout << "  lacp.aggre_id: " << lacp.aggregator_id << endl;
        cout << "  lacp.part_mac: " << lacp.partner_mac <<endl;
        
        ostringstream stm;
        stm.str("");
        stm << "  lacp.nic_suc: ";  
        vector<string>::iterator it_nic = lacp.nic_suc.begin();
        for(; it_nic != lacp.nic_suc.end(); ++it_nic)
        {
            stm << (*it_nic) << " ";
        }
        cout << stm.str() << endl;

        stm.str("");
        stm << "  lacp.nic_fail: ";  
        it_nic = lacp.nic_fail.begin();
        for(; it_nic != lacp.nic_fail.end(); ++it_nic)
        {
            stm << (*it_nic) << " ";
        }
        cout << stm.str() << endl;
        
        cout << "  backup.nic_active: " << backup.active_nic << endl;
        stm.str("");
        stm << "  backup.nic_other: ";  
        it_nic = backup.other_nic.begin();
        for(; it_nic != backup.other_nic.end(); ++it_nic)
        {
            stm << (*it_nic) << " ";
        }
        cout << stm.str() << endl;
    }
    
public:
    string name; // bond name
    int32 mode; // = 0 balance; = 1 backup; = 4 802 lacp
    
    T_BOND_LACP_INFO   lacp;  // mode = 4 有效
    T_BOND_BACKUP_INFO backup;// mode = 1 有效
};

STATUS GetIpAddress(const string&if_name,uint32 &ip_address);
STATUS GetIpAddress(const string&if_name,string &ip_address);
bool IsValidIp(const string& ip);
bool IsLocalIp(const string& ip);
bool IsLocalUrl(const string& url);
STATUS GetIpByConnectBroker(const string &borker_url, string &ip_address);
STATUS GetIpByConnect(const string &dest_url, string &ip_address);
THREAD_ID StartThread(const char *name,ThreadEntry entry,void *arg, bool joinable = false);
THREAD_ID StartThreadEx(const char *name,int priority,int stacksize,ThreadEntry entry,void *arg);
STATUS KillThread(THREAD_ID id);
STATUS GetThreadName(THREAD_ID id,string& name);
STATUS removeendl(string &s);
STATUS put_string_to_vec(string text,vector<string>& vec);
void print_vector_string(const vector<string>& strs);

STATUS get_nics(vector<string>& nics);
STATUS get_bridges(vector<string>& bridges);
STATUS get_ifs_of_bridge(const string& bridge, vector<string>& ifs);
STATUS get_sriov_port(vector<string>& sriovs);
STATUS get_sriov_pci(const string &nicname, map<string,string>&vif_pci);
STATUS get_sriov_num(const string &nicname, int &num);
BOOL   IsLoopDevice(const string &port_name);
STATUS set_port_loopback(const string &port_name, const uint32 &lpflag);
STATUS get_port_loopback_mode(const string &port_name,    int32 &lpflag);
STATUS get_bond_info(const string bond,string& mode,string& miimon,
                     string& lacp_rate, string& mii_status, vector<string>& slaves);
STATUS get_bridge_of_nic(const string& nic, string& bridge);
int32 bond_mode_stringtoint(const string& mode);
STATUS get_bondings(vector<string>& bondings);
STATUS get_bond_para(const string bond,const string para,string& value);
STATUS add_bond(const string bond,const string mode, vector<string> ifs);
STATUS del_bond(const string bond);
STATUS set_bond_para(const string bond, const string para,const string value);
string bond_mode_inttostring(const int& mode);
STATUS get_bond_info(const string bond,CBondInfo &info);
STATUS get_nic_status(const string nics,int & status);
STATUS send_ioctl(const char* ifname, struct private_value *pval);
STATUS IsSupportBond(int &isSupport);
int GetKernelPortInfo(const string &port_name, string &ip, string &mask);
STATUS GetAllIfsInfo(map<string, string>& ifs_info);

#endif


