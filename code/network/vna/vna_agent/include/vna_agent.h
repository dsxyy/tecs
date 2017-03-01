/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�vnet_agent.h
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��CVNetAgent�������ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�lvech
* ������ڣ�2013��1��15��
*
* �޸ļ�¼1��
*     �޸����ڣ�2013/1/15
*     �� �� �ţ�V1.0
*     �� �� �ˣ�lvech
*     �޸����ݣ�����
*******************************************************************************/
#ifndef VNET_AGENT_H
#define VNET_AGENT_H

namespace zte_tecs 
{

//MODULE state ȡֵ
#define VNET_MODULE_OFFLINE     0
#define VNET_MODULE_ONLINE       1

#define VNET_MODULE_MAX_OFFLINE_TIME        30


class TModuleInfo
{
public:
    TModuleInfo()
    {
        m_nRole = VNA_MODULE_ROLE_NONE;
        m_nState = VNET_MODULE_OFFLINE;
        m_tLastHeartTime = 0;
    };
    
    void SetModuleName(string strModuleName){m_strModuleName = strModuleName;};
    void SetRole(int32 nRole){m_nRole = nRole;};
    void SetState(int32 nState){m_nState = nState;};
    void SetLastHeartTime(time_t tLastHeartTime){m_tLastHeartTime = tLastHeartTime;};
    void SetExtInfo(string strExtInfo){m_strExtInfo = strExtInfo;};
    void SetModuleLibMid(MID &cModuleLibMid){m_cModuleLibMid = cModuleLibMid;};
    string GetModuleName(){return m_strModuleName;};
    int32 GetRole() const {return m_nRole;};
    int32 GetState() const {return m_nState;};
    time_t GetLastHeartTime() const {return m_tLastHeartTime;};
    string GetExtInfoe() const {return m_strExtInfo;};
    MID GetModuleLibMid() const {return m_cModuleLibMid;};
    
    void Print()
    {
        cout << "Module("<< "m_strModuleName" <<") Info:" << endl;
        cout << "   Role:" << m_nRole << endl;
        cout << "   State:" << m_nState << endl;
        cout << "   LastHeartTime:" << m_tLastHeartTime << endl;
        cout << "   External Info:" << m_strExtInfo << endl;
    };

private:
    string m_strModuleName;
    int32 m_nRole;
    int32 m_nState;
    time_t m_tLastHeartTime;
    string m_strExtInfo;
    MID m_cModuleLibMid;
};

/**
@brief ��������: ����������ʵ���࣬ʵ��״̬���ģ��Ĺ���\n
@li @b ����˵��: ��
*/
class CVNetAgent : public MessageHandler
{
public:
    static CVNetAgent *GetInstance()
    {
        if (NULL == m_pInstance)
        {
            m_pInstance = new CVNetAgent();
        }
        return m_pInstance;
    };

    virtual ~CVNetAgent();
    STATUS Init()
    {
        return StartMu(MU_VNA_AGENT);
    }
    virtual void MessageEntry(const MessageFrame &message); //��Ϣ������ں���
    int32 GetModuleInfo(int32 m_nRole, TModuleInfo &cModule);
    void DbgShowData(void);
    void SetDbgFlag(int bDbgFlag);
    void SetReportSwitch(int32 nSwitchFlag);

private:
    CVNetAgent(void);
    STATUS StartMu(const string& name);
    STATUS DoStartUp(void);
    void ExitVNMGroup(void);
    void JoinVNMGroup(void);
    void GoToSendRegToVNM(void);
    void SendRegToVNM(void);
    void GoToSendVNAReport(void);
    void SendVNAReport(void);
    void SendVNAReportFirst(void);
    void RecvRegFromModule(const MessageFrame &message);
    void RecvUnRegFromModule(const MessageFrame &message);
    void RecvRegReqFromVNM(const MessageFrame &message);
    void RecvUnRegReqFromVNM(const MessageFrame &message);
    void SetVNM(const string &vnm_name);
    void RecvHeartFromModule(const MessageFrame &message);
    void CheckModuleState();
    void BroadcastRegedVNM();
    void RecvRegedVNMReq(const MessageFrame &message);
    int  HCRegisteredSystem(const MessageFrame &message);
    int GetModuleReportInfo(vector<CVNARegModMsg> &vecModInfo);

private:
    static CVNetAgent    *m_pInstance;

private:
    BOOL m_bOpenDbgInf;
    int32 m_nStateMachines;
    MessageUnit  *m_pMU;                      // ��Ϣ��Ԫ
    TIMER_ID       m_nTimerID;             // �������ֺ���Ϣ�ϱ����õĶ�ʱ��
    TIMER_ID       m_nModuleCheckTimerID;             // ���ڼ��module״̬�Ķ�ʱ��
    string  m_strVNMApp;                  //��¼CC��Ϣ����HC������CC��Ϣ������HC/CC��һ���������HC�������ڶ�Ӧ�����ϵ�CC
    vector<TModuleInfo>  m_vecModuleInfo;

    // ���ÿ�������
    DISALLOW_COPY_AND_ASSIGN(CVNetAgent);   
};

} // namespace zte_tecs
#endif // #ifndef VNET_AGENT_H

