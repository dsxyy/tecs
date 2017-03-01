/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：vna_heartbeat.h
* 文件标识：见配置管理计划书
* 内容摘要：CVNetHeartbeat类申明文件
* 当前版本：1.0
* 作    者：lvech
* 完成日期：2013年1月9日
*
* 修改记录1：
*     修改日期：2013/1/9
*     版 本 号：V1.0
*     修 改 人：lvech
*     修改内容：创建
*******************************************************************************/
#ifndef VNA_HEARTBEAT_H
#define VNA_HEARTBEAT_H


namespace zte_tecs 
{

/**
@brief 功能描述: 物理机代理的实体类，实现状态监控模块的功能\n
@li @b 其它说明: 无
*/
class CVNetHeartbeat : public MessageHandler
{
public:
    static CVNetHeartbeat *GetInstance()
    {
        if (NULL == _instance)
        {
            _instance = new CVNetHeartbeat();
        }
        return _instance;
    };

    virtual ~CVNetHeartbeat();
    
    STATUS Init();    
    virtual void MessageEntry(const MessageFrame &message); //消息处理入口函数
    void SetReportSwitch(int32 nSwitchFlag);
    void SetDbgFlag(BOOL bOpen)
    {
        m_bOpenDbgInf = bOpen;
        return ;
    };

private:
    CVNetHeartbeat();
    STATUS StartMu(const string& name);
    void DoStartUp();
    void DoRegisterSystem(const MessageFrame &message);
    void DoHeartbeatReport();

private:
    static CVNetHeartbeat *_instance;  

    BOOL m_bOpenDbgInf;
    MessageUnit *   m_pMU;               // 消息单元
    TIMER_ID m_nHeartTimer;
    string  m_strVNMName;      // 本主机归属的集群名称

    // 禁用拷贝构造
    DISALLOW_COPY_AND_ASSIGN(CVNetHeartbeat);   
};

} // namespace zte_tecs
#endif // #ifndef VNA_HEARTBEAT_H

