/******************************************************************************
* Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：vnm_vm_mgr.h
* 文件标识：
* 内容摘要：CVNetVmMgr类的定义文件
* 当前版本：1.0
* 作    者：
* 完成日期：2013年2月22日
*
* 修改记录1：
*     修改日期：2013/2/22
*     版 本 号：V1.0
*     修 改 人：lverchun
*     修改内容：创建
******************************************************************************/
#if !defined(VNM_VM_MGR_INCLUDE_H__)
#define VNM_VM_MGR_INCLUDE_H__

namespace zte_tecs
{

//允许最大的VM部署执行任务目录
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
        m_nCurrentOP = VNM_VM_NETWORK_OPERATE_NULL;         //当前虚拟机执行的操作
        m_strActionID = "";                                 //调度任务工作流ID
        m_cSenderMId._application = "";                   //调度任务发起者MID
        m_cSenderMId._process = "";                       //调度任务发起者MID
        m_cSenderMId._unit = "";                          //调度任务发起者MID
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
    int32 m_nCurrentOP;                  //当前虚拟机执行的操作
    string m_strActionID;                //调度任务工作流ID
    MID m_cSenderMId;                    //调度任务发起者MID
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
    CVnetSchTask m_cVMSchTaskInfo;  //调度任务列表
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


