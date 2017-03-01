#if !defined(VNET_MONITOR_INCLUDE_H__)
#define VNET_MONITOR_INCLUDE_H__

#include <string>
#include <map>
#include <sys/stat.h>
#include "sys.h"
#include "message.h"
#include "message_unit.h"
#include "timer.h"
#include "mid.h"
#include "vnet_comm.h"

namespace zte_tecs
{

class CVNetMonitorBase
{
public:
    CVNetMonitorBase(){};
    virtual ~CVNetMonitorBase(){};
    virtual void MonitorHandle(){};
    virtual int Init(){return SUCCESS;};
    virtual void Print(){};
private:
    
};

class CBondFile
{
public:
    CBondFile(){_laststamp = _num = 0;info.Init();}
    virtual ~CBondFile(){};
    void print()
    {
        cout << "laststamp: " << _laststamp << endl;
        cout << "number   : " << _num << endl;
        info.print();
    }
    
    time_t  _laststamp;
    int  _num; 
    class CBondInfo info;
};

class CVNetMonitorBond: public CVNetMonitorBase
{
public:
    CVNetMonitorBond();
    virtual ~CVNetMonitorBond();
    virtual void MonitorHandle();
    virtual int Init();
    virtual void Print();
private:
    void BondHandle();
    void NicHandle();

    int GetBondFileTimeStamp(const string & bond, time_t & t);
    int BondNicsAllOpen(CBondInfo & info);
    int BondNicsOperator(CBondInfo & info);
    int BondDel(CBondInfo &  info);
    int BondModify(CBondInfo & info);
    int BondAdd(const string bond,CBondInfo & info);

    STATUS SendMsgToVF(const char* ifname, struct private_value *pval);
public:    
    int GetSriovBond(set<string> &sBond);
    int32 GetBondInfo(const string strBondName, CBondInfo& cInfo);	
    
    int32 IsBondModify(CBondFile & bf,int32 & isModify);
    
    int DbgPrint(const char* pcFmt,...);

private:
    list<CBondFile> _lst_bond; // bond 列表
    map<string,uint32> _nic_vf;   // nicname ~ vf (open/close)
};

class CVNetMonitorNic: public CVNetMonitorBase
{
public:
    CVNetMonitorNic();
    virtual ~CVNetMonitorNic();
    virtual void MonitorHandle();
    virtual int Init();
    virtual void Print();
private:
    void OvsNicHandle();
    map<string, int32> m_mapGabageOvsIf;    

public:
    int DbgPrint(const char* pcFmt,...);
};


// VNet 
class CVNetMsgHandler : public MessageHandler
{
public:    
    CVNetMsgHandler();
    virtual ~CVNetMsgHandler();

protected:
    STATUS Init(const char* pcMu)
    {        
        if( NULL == pcMu)
        {
            return ERROR;
        }
        
        STATUS ret = SUCCESS;
        ret = StartMu(pcMu);
        if( ret != SUCCESS )
        {               
            VNET_LOG(VNET_LOG_ERROR, "CVNetMsgHandler::Init StartMu(%s) failed.\n",pcMu);
            return ret;
        }
        
        return SUCCESS;
    }    
    STATUS StartMu(const string& name);
    int32 PowerOn(void);
    STATUS Receive(const MessageFrame& message);

    // 继承者必须实现MessageEntry VNetInit函数
    // void MessageEntry(const MessageFrame& message);
    virtual int32 VNetInit(void * arg = 0 ){return SUCCESS;}
    
protected:
    MessageUnit *_msg_unit;  
    
private:    
    int32 _state_machines;
};

// Monitor Mgr 类
class CVNetMonitorMgr: public CVNetMsgHandler
{
public:
    CVNetMonitorMgr();
    virtual ~CVNetMonitorMgr();

    static CVNetMonitorMgr *GetInstance()
    {        
        if(NULL == s_pInstance)
        {
            s_pInstance = new CVNetMonitorMgr();
        }
        return s_pInstance;
    };

    STATUS init()
    {
        STATUS ret = SUCCESS;
        ret = Init(MU_VNET_MONITOR_MGR);
        if( ret != SUCCESS )
        {               
            VNET_LOG(VNET_LOG_ERROR, "CVNetMonitorMgr::Init failed.\n");
            return ret;
        }
        
        return SUCCESS;
    }

    int32 DbgShowData(void);
    
protected:
    void MessageEntry(const MessageFrame& message);
    int32 VNetInit(void * );
        
private:
    int32 SetMonitorTimer();
    void Handle();
    
    static CVNetMonitorMgr * s_pInstance;

    TIMER_ID _timer_id;
    list<CVNetMonitorBase *> _lst_monitor;

    DISALLOW_COPY_AND_ASSIGN(CVNetMonitorMgr);
};


}// namespace zte_tecs

#endif // VNET_MONITOR_INCLUDE_H__


