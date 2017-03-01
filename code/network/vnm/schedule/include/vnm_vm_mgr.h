/******************************************************************************
* Copyright (c) 2012������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�vnm_vm_mgr.h
* �ļ���ʶ��
* ����ժҪ��CVNetVmMgr��Ķ����ļ�
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
#if !defined(VNM_VM_MGR_INCLUDE_H__)
#define VNM_VM_MGR_INCLUDE_H__

namespace zte_tecs
{

//��������VM����ִ������Ŀ¼
#define    VNM_VM_NETWORK_MAX_SCHEDULE_TASK_NUM     10

#define    VNM_VM_NETWORK_OPERATE_NULL              0
#define    VNM_VM_NETWORK_OPERATE_SELECT_CC         1
#define    VNM_VM_NETWORK_OPERATE_GET_RES_CC        2
#define    VNM_VM_NETWORK_OPERATE_FREE_RES_CC       3
#define    VNM_VM_NETWORK_OPERATE_SELECT_HC         4
#define    VNM_VM_NETWORK_OPERATE_GET_RES_HC        5
#define    VNM_VM_NETWORK_OPERATE_FREE_RES_HC       6

class CVnetSchTask
{
public:
    explicit CVnetSchTask()
    {
        m_nVmID = 0; 
        m_nCurrentOP = 0;
    };
    
    virtual ~CVnetSchTask()
    {

    };
    
    void Reset()
    {
        m_nVmID = 0;                                        //VM ID
        m_nCurrentOP = VNM_VM_NETWORK_OPERATE_NULL;         //��ǰ�����ִ�еĲ���
        m_strActionID = "";                                 //������������ID
        m_cSenderMId._application = "";                   //������������MID
        m_cSenderMId._process = "";                       //������������MID
        m_cSenderMId._unit = "";                          //������������MID
    };
    
    void Print()
    {
        cout << "CVnetSchTask Info:"<< endl;
        cout << "   m_nVmID         :" << m_nVmID<< endl;
        cout << "   m_nCurrentOP    :" << m_nCurrentOP<< endl;
        cout << "   m_strActionID   :" << m_strActionID<< endl;
    };

public:    
    int64 m_nVmID;                       //VM ID
    int32 m_nCurrentOP;                  //��ǰ�����ִ�еĲ���
    string m_strActionID;                //������������ID
    MID m_cSenderMId;                    //������������MID
};



class CVNetVmMgr
{
public:
    explicit CVNetVmMgr();
    virtual ~CVNetVmMgr();
    
    int32 Init(MessageUnit *mu);
    void MessageEntry(const MessageFrame &message);
    void DbgShowData(void);
    void SetDbgFlag(BOOL bOpen){m_bOpenDbgInf = bOpen;}
    
    static CVNetVmMgr *GetInstance()
    {
        if(NULL == s_pInstance)
        {
            s_pInstance = new CVNetVmMgr();
        }

        return s_pInstance;
    };
    
private:    
    void SelectCCListForVM(const MessageFrame& message);
    void RcvSelectCCAckFromSchedule(const MessageFrame& message);
    void GetNetResourceOnCC(const MessageFrame& message);
    void RcvGetResCCAckFromSchedule(const MessageFrame& message);
    void FreeNetResourceOnCC(const MessageFrame& message);
    void RcvFreeResCCAckFromSchedule(const MessageFrame& message);
    void SelectHCListForVM(const MessageFrame& message);
    void RcvSelectHCAckFromSchedule(const MessageFrame& message);
    void GetNetResourceOnHC(const MessageFrame& message);
    void RcvGetResHCAckFromSchedule(const MessageFrame& message);
    void FreeNetResourceOnHC(const MessageFrame& message);
    void RcvFreeResHCAckFromSchedule(const MessageFrame& message);

private:  
    BOOL m_bOpenDbgInf;
    MessageUnit *m_pMU;
    CVnetSchTask m_cVMSchTaskInfo;  //���������б�
    CVNetVmDB * m_pVNetVmDB;
    CVNetVnicDB * m_pVNetVnicDB;
    CVSIProfileMgr * m_pVSIProfileMgr;
    CSwitchMgr * m_pSwitchMgr;
    CVNAManager * m_pVNAMgr;
    CVNMSchedule * m_pVNMSchedule;
    
private:
    static CVNetVmMgr *s_pInstance;

    DISALLOW_COPY_AND_ASSIGN(CVNetVmMgr);
};

}// namespace zte_tecs

#endif // VNM_VM_MGR_INCLUDE_H__


