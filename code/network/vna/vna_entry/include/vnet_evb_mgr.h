/******************************************************************************
* Copyright (c) 2012������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�vnet_evb_mgr.h
* �ļ���ʶ��
* ����ժҪ��CEvbMgr��Ҫ����EVB��TECS�ӿڡ�
* ��ǰ�汾��1.0
* ��    �ߣ�
* ������ڣ�2012��5��22��
*
* �޸ļ�¼1��
*     �޸����ڣ�2012/5/22
*     �� �� �ţ�V1.0
*     �� �� �ˣ�Wang.Lule
*     �޸����ݣ�����
******************************************************************************/
#if !defined(VNET_EVB_MGR_INCLUDE_H__)
#define VNET_EVB_MGR_INCLUDE_H__

namespace zte_tecs
{
/* StartVPortDiscovery�����붨��; */
#define START_VPORT_DISCOVERY_SUCCESS   (0)    /* ����VM EVB���ܳɹ�; */
#define START_VPORT_DISCOVERY_INVALID   (1)    /* ����VM EVB������Ч, ��Ӱ��VM����; */
#define START_VPORT_DISCOVERY_FAILED    (-1)   /* ����VM EVB����ʧ��, Ӱ��VM����; */
#define START_VPORT_DISCOVERY_RUNNING   (2)    /* ����VM EVB����������... ��HC���и���; */

/* StopVPortDiscovery�����붨��; */
#define STOP_VPORT_DISCOVERY_SUCCESS   (0)    /* ֹͣVM EVB���ܳɹ�; */
#define STOP_VPORT_DISCOVERY_INVALID   (1)    /* ֹͣVM EVB������Ч, ��Ӱ��VM����; */
#define STOP_VPORT_DISCOVERY_FAILED    (-1)   /* ֹͣVM EVB����ʧ��, Ӱ��VM����; */
#define STOP_VPORT_DISCOVERY_RUNNING   (2)    /* ֹͣVM EVB����������... ��HC���и���; */

#define NEW_EVBPORT_DELAY_TIME         (30)   /* new evb port ��ʱ�ж�evb tlv dom0 ״̬*/

class C20msMsgTimer;
class CVNAVMVnicInfo;

class CEvbMgr
{
public:
    explicit CEvbMgr();
    virtual ~CEvbMgr();

public:
    // ���ʵ����
    static CEvbMgr *GetInstance()
    {
        if (NULL == s_pInstance)
        {
            s_pInstance = new CEvbMgr();
        }

        return s_pInstance;
    };
    
    // ��ʼ����
    int32 Init(void);
    // ����EVB ISS�˿�
    int32 SetEvbIssPort(const string &strPortName);
    // �ر�����EVB ISS�˿�
    int32 UnsetEvbIssPort(const string &strPortName);
    // ����EVBģʽVEPA or VEB    
    int32 SetEvbMode(const string &strPortName, int32 nEVBMode);
 
    // ����������˿ڷ��ֹ��ܣ��������������ʱ���á�
    int32 StartVPortDiscovery(const int64 &nVMID, vector<CVNetVnicDetail> &vecNic, BYTE option,int32 noNeedEvbTlv=FALSE);
    // ֹͣ������˿ڷ��ֹ��ܣ�ֹͣ���������ʱ���á�
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

    /* ����URP ��VM vNic �˿ںŵĶ�Ӧ��ϵ; */
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
    // ��������Ǹ�����麯�����û�����ʵ��
    STATUS Receive(const MessageFrame& message)
    {
        return m_mu->Receive(message);
    };  
    // ��ʼ����ʱ��
    int32 InitTimer(void);
    
    void MessageEntry(const MessageFrame& message);
  
private:
    //˽�е���Ϣ���г�Ա��������Ϣ�շ�.
    MessageQueue *m_mu;
    // 20ms timer id.
    TIMER_ID  m_t20msTimerID;
    // ʵ�����
    static C20msMsgTimer *s_pInstance;
    
    DISALLOW_COPY_AND_ASSIGN(C20msMsgTimer);
};

}// namespace zte_tecs

#endif // VNET_EVB_MGR_INCLUDE_H__

