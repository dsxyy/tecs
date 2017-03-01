
/******************************************************************************
* Copyright (c) 2012������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�vnm_schedule.h
* �ļ���ʶ��
* ����ժҪ��CVNMQuery��Ķ����ļ�
* ��ǰ�汾��1.0
* ��    �ߣ�
* ������ڣ�2013��2��22��
*
* �޸ļ�¼1��
*     �޸����ڣ�2013/2/22
*     �� �� �ţ�V1.0
*     �� �� �ˣ�lverchun
*     �޸����ݣ�����
******************************************************************************/
#if !defined(VNM_QUERY_INCLUDE_H__)
#define VNM_QUERY_INCLUDE_H__

namespace zte_tecs
{


class CVNetVmDB;
class CVNetVnicDB;
class CVSIProfileMgr;
class CSwitchMgr;
class CVNAManager;
class CDbSchedule;

#if 1




// VNM XMLRPC ��ѯ������
class CVNMQuery: public MessageHandler
{
public:
    explicit CVNMQuery();
    virtual ~CVNMQuery();
    
    int32 Init();
    STATUS StartMu(string strMsgUnitName);
    void MessageEntry(const MessageFrame &message);
    void DbgShowData(void);
    void SetDbgFlag(int bDbgFlag);

    static CVNMQuery *GetInstance()
    {
        if(NULL == s_pInstance)
        {
            s_pInstance = new CVNMQuery();
        }

        return s_pInstance;
    };
public:
    
private:  
    void VMVnicInfoReq(const MessageFrame& message);
    
private:
    BOOL m_bOpenDbgInf;
    MessageUnit *m_pMU;
    CVNetVmDB * m_pVNetVmDB;
    CVNetVnicDB * m_pVNetVnicDB;
    CVSIProfileMgr * m_pVSIProfileMgr;
    CSwitchMgr * m_pSwitchMgr;
    CVNAManager * m_pVNAMgr;
    CLogicNetworkMgr * m_pLogicNetworkMgr;

private:
    static CVNMQuery *s_pInstance;

    DISALLOW_COPY_AND_ASSIGN(CVNMQuery);

};


#endif 
}

#endif

