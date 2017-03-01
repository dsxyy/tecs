/******************************************************************************
* Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：vnet_evb_mgr.h
* 文件标识：
* 内容摘要：CEvbMgr主要管理EVB与TECS接口。
* 当前版本：1.0
* 作    者：
* 完成日期：2012年5月22日
*
* 修改记录1：
*     修改日期：2012/5/22
*     版 本 号：V1.0
*     修 改 人：Wang.Lule
*     修改内容：创建
******************************************************************************/
#if !defined(VNET_EVB_MGR_INCLUDE_H__)
#define VNET_EVB_MGR_INCLUDE_H__

namespace zte_tecs
{
/* StartVPortDiscovery返回码定义; */
#define START_VPORT_DISCOVERY_SUCCESS   (0)    /* 启动VM EVB功能成功; */
#define START_VPORT_DISCOVERY_INVALID   (1)    /* 启动VM EVB功能无效, 不影响VM操作; */
#define START_VPORT_DISCOVERY_FAILED    (-1)   /* 启动VM EVB功能失败, 影响VM操作; */
#define START_VPORT_DISCOVERY_RUNNING   (2)    /* 启动VM EVB功能运行中... 需HC进行跟催; */

/* StopVPortDiscovery返回码定义; */
#define STOP_VPORT_DISCOVERY_SUCCESS   (0)    /* 停止VM EVB功能成功; */
#define STOP_VPORT_DISCOVERY_INVALID   (1)    /* 停止VM EVB功能无效, 不影响VM操作; */
#define STOP_VPORT_DISCOVERY_FAILED    (-1)   /* 停止VM EVB功能失败, 影响VM操作; */
#define STOP_VPORT_DISCOVERY_RUNNING   (2)    /* 停止VM EVB功能运行中... 需HC进行跟催; */

#define NEW_EVBPORT_DELAY_TIME         (30)   /* new evb port 延时判断evb tlv dom0 状态*/

class C20msMsgTimer;
class CVNAVMVnicInfo;

class CEvbMgr
{
public:
    explicit CEvbMgr();
    virtual ~CEvbMgr();

public:
    // 获得实例。
    static CEvbMgr *GetInstance()
    {
        if (NULL == s_pInstance)
        {
            s_pInstance = new CEvbMgr();
        }

        return s_pInstance;
    };
    
    // 初始化。
    int32 Init(void);
    // 设置EVB ISS端口
    int32 SetEvbIssPort(const string &strPortName);
    // 关闭设置EVB ISS端口
    int32 UnsetEvbIssPort(const string &strPortName);
    // 设置EVB模式VEPA or VEB    
    int32 SetEvbMode(const string &strPortName, int32 nEVBMode);
 
    // 启动虚拟机端口发现功能，启动虚拟机网络时调用。
    int32 StartVPortDiscovery(const int64 &nVMID, vector<CVNetVnicDetail> &vecNic, BYTE option,int32 noNeedEvbTlv=FALSE);
    // 停止虚拟机端口发现功能，停止虚拟机网络时调用。
    int32 StopVPortDiscovery(const int64 &nVMID, vector<CVNetVnicDetail> &vecNic, BYTE option);

    // 
    int32 GetEvbIssPortNo(const string &strPortName);
private:
    int32 StartVPortProcNoNeedEvbTlv(const int64 &nVMID, map<CVNetVnicDetail*, uint32> &mapVNicTblURP,BYTE option);
    int32 StartVPortProc(const int64 &nVMID, map<CVNetVnicDetail*, uint32> &mapVNicTblURP,BYTE option);
    int32 DeleteVsiProfileAndCmdPool(uint32 uiURPNo, CVNetVnicDetail*pNic);
    int32 StopVPortProc(const int64 &nVMID, map<CVNetVnicDetail*, uint32> &mapVNicTblURP,BYTE option);

    void ParseEvbEnable();

    int32 ConfigISSPort(const string &strPortName, const uint32 &uiPortNo);
    int32 NoCfgISSPort(const string &strPortName);

    int32 ChkVSIParam(vector<CVNetVnicDetail> &vecNic);
    int32 ChkVMOpt(BYTE option);
    int32 IsNotNeedEVB(vector<CVNetVnicDetail> &vecNic,vector<CVNetVnicDetail> &vecEvbNic);

    uint32 GetISSPortNo(const string &c_strPortName);

    /* 建立URP 与VM vNic 端口号的对应关系; */
    int32 BuildVNicURPMap(vector<CVNetVnicDetail> &vecNic,
                          map<CVNetVnicDetail*, uint32> &mapURPVNic);
    int32 GetAllURP(map<CVNetVnicDetail*, uint32> &mapVNicURP, vector<uint32> &vecURPNo);
    int32 GetVMEVBTlvStat(map<CVNetVnicDetail*,uint32> &mapVNicURP, map<CVNetVnicDetail*,int32> &mVnicEvb);
    int32 ChkURPVDPStat(map<CVNetVnicDetail*, uint32> &mapURPVNic);

    int32 ExecVDPCmd4VSI(uint32 uiURPNo, CVNetVnicDetail *pNic, BYTE option);
    int32 FormatVSIID(const string &s, BYTE *vsiid);
    int32 FormatMgrID(const string &s, BYTE *mgrid);
    int32 FormatVSITypeID(const string &s, BYTE *typid);

    int32 CheckIsExistNewEvbPort(map<CVNetVnicDetail*, uint32> &mapURPVNic);

    void DbgShowVmNicCfg(vector<CVNetVnicDetail> &vecNic, 
                         map<CVNetVnicDetail*,uint32 > &mapVNicURP, 
                         BYTE option);

public:
    void DbgShowISSPort(void);

private:
    map<string, uint32> m_mapISSPort;   // EVB ISS port <name, port no>;

    static CEvbMgr *s_pInstance;

    BOOL m_bEvbEnable;

    C20msMsgTimer *m_p20msTimer;

    uint32 m_uiISSPortIdx;

    DISALLOW_COPY_AND_ASSIGN(CEvbMgr);
};


class C20msMsgTimer : public MessageHandler
{
public:
    C20msMsgTimer();
    virtual ~C20msMsgTimer();

public: 
    static C20msMsgTimer *GetInstance(void)
    {
        if (NULL == s_pInstance)
        {
            s_pInstance = new C20msMsgTimer();
        }

        return s_pInstance;
    };
    
    STATUS Init(void)
    {
        return StartMu(MU_VNET_20MS_TIMER);
    }

    int32 StartTimer(void);
    
    int32 StopTimer(void);

protected:    
    STATUS StartMu(const string& name);    
    // 这个函数是父类的虚函数，用户必须实现
    STATUS Receive(const MessageFrame& message)
    {
        return m_mu->Receive(message);
    };  
    // 初始化定时器
    int32 InitTimer(void);
    
    void MessageEntry(const MessageFrame& message);
  
private:
    //私有的消息队列成员，负责消息收发.
    MessageQueue *m_mu;
    // 20ms timer id.
    TIMER_ID  m_t20msTimerID;
    // 实例句柄
    static C20msMsgTimer *s_pInstance;
    
    DISALLOW_COPY_AND_ASSIGN(C20msMsgTimer);
};

}// namespace zte_tecs

#endif // VNET_EVB_MGR_INCLUDE_H__

