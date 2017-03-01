/*******************************************************************************
* Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�vna_heartbeat.h
* �ļ���ʶ�������ù���ƻ���
* ����ժҪ��CVNetHeartbeat�������ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�lvech
* ������ڣ�2013��1��9��
*
* �޸ļ�¼1��
*     �޸����ڣ�2013/1/9
*     �� �� �ţ�V1.0
*     �� �� �ˣ�lvech
*     �޸����ݣ�����
*******************************************************************************/
#ifndef VNA_HEARTBEAT_H
#define VNA_HEARTBEAT_H


namespace zte_tecs 
{

/**
@brief ��������: ����������ʵ���࣬ʵ��״̬���ģ��Ĺ���\n
@li @b ����˵��: ��
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
    virtual void MessageEntry(const MessageFrame &message); //��Ϣ������ں���
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
    MessageUnit *   m_pMU;               // ��Ϣ��Ԫ
    TIMER_ID m_nHeartTimer;
    string  m_strVNMName;      // �����������ļ�Ⱥ����

    // ���ÿ�������
    DISALLOW_COPY_AND_ASSIGN(CVNetHeartbeat);   
};

} // namespace zte_tecs
#endif // #ifndef VNA_HEARTBEAT_H

