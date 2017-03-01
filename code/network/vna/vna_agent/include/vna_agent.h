/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：vnet_agent.h
* 文件标识：见配置管理计划书
* 内容摘要：CVNetAgent类申明文件
* 当前版本：1.0
* 作    者：lvech
* 完成日期：2013年1月15日
*
* 修改记录1：
*     修改日期：2013/1/15
*     版 本 号：V1.0
*     修 改 人：lvech
*     修改内容：创建
*******************************************************************************/
#ifndef VNET_AGENT_H
#define VNET_AGENT_H

namespace zte_tecs 
{

//MODULE state 取值
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
@brief 功能描述: 物理机代理的实体类，实现状态监控模块的功能\n
@li @b 其它说明: 无
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
    virtual void MessageEntry(const MessageFrame &message); //消息处理入口函数
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
    MessageUnit  *m_pMU;                      // 消息单元
    TIMER_ID       m_nTimerID;             // 主机发现和信息上报公用的定时器
    TIMER_ID       m_nModuleCheckTimerID;             // 用于检查module状态的定时器
    string  m_strVNMApp;                  //记录CC信息或者HC归属的CC信息，对于HC/CC合一的情况，此HC必须属于对应主机上的CC
    vector<TModuleInfo>  m_vecModuleInfo;

    // 禁用拷贝构造
    DISALLOW_COPY_AND_ASSIGN(CVNetAgent);   
};

} // namespace zte_tecs
#endif // #ifndef VNET_AGENT_H

