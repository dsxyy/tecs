/******************************************************************************
* Copyright (c) 2012������ͨѶ�ɷ����޹�˾��All rights reserved.
*
* �ļ����ƣ�vnet_vnic_pool.h
* �ļ���ʶ��
* ����ժҪ��CVNAVnicPool��Ķ����ļ�
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
#if !defined(VNA_VNIC_POOL_INCLUDE_H__)
#define VNA_VNIC_POOL_INCLUDE_H__

namespace zte_tecs
{

#define  VNA_REQUEST_VSI_FROM_VNM_MAX_TIMES     5
#define  VNA_REQUEST_VSI_FROM_VNM_INTERVAL     1000

#define  VNA_VM_WDG_OP_MAX_TIMES     5
#define  VNA_VM_WDG_OP_INTERVAL     1000

#define  VNA_REQUEST_VSI_EVB_MAX_TIMES     50

//m_bScheduleFlag;    ��ʶ�Ƿ��Ѳ���HC��

#define VNA_VM_DEPLOY_STATUS_NETWORK_INIT       0       //��ʼ̬�������VNM�·���VSI��Ϣ����ʱ�������״̬
#define VNA_VM_DEPLOY_STATUS_NETWORK_STARTING   1       //���������У������HC�����startnetwork����δ��HC��Ӧ�𣬴�ʱ�������״̬
#define VNA_VM_DEPLOY_STATUS_NETWORK_STOPING    2       //����ֹͣ�У������HC�����stopnetwork����δ��HC��Ӧ�𣬴�ʱ�������״̬
#define VNA_VM_DEPLOY_STATUS_NETWORK_RUNNING    3       //���������У������HC�����startnetwork�ѳɹ�����ʱ�������״̬
#define VNA_VM_DEPLOY_STATUS_NETWORK_START_FAIL 4       //��������ʧ�ܣ������HC�����startnetwork����δ��HC��Ӧ�𣬴�ʱ�������״̬
#define VNA_VM_DEPLOY_STATUS_NETWORK_STOP_FAIL  5       //����ֹͣʧ�ܣ������HC�����stopnetwork����δ��HC��Ӧ�𣬴�ʱ�������״̬

// m_bGetVSIFlag;     ��ʶ�Ƿ��Ѵ�VNM�ϻ�ȡ��ϸ��Ϣ
#define VNA_GET_VSI_DETAIL_FLAG_YES             1
#define VNA_GET_VSI_DETAIL_FLAG_NO              0

#define VNA_GET_VSI_DETAIL_STATUS_INIT          0
#define VNA_GET_VSI_DETAIL_STATUS_OK            1
#define VNA_GET_VSI_DETAIL_STATUS_FAIL          2

//VSI EVB ״̬��Ϣ
#define VNA_VM_VSI_EVB_STATUS_INIT                (0)
#define VNA_VM_VSI_EVB_STATUS_SUCCESS             (1)
#define VNA_VM_VSI_EVB_STATUS_FAILED              (2)
#define VNA_VM_VSI_EVB_STATUS_RUNNING             (3)

#define VNA_VM_WDG_ENABLE_YES                   1
#define VNA_VM_WDG_ENABLE_NO                    0

#define VNA_VM_WDG_OP_STATUS_INIT               0
#define VNA_VM_WDG_OP_STATUS_OK                 1
#define VNA_VM_WDG_OP_STATUS_FAIL               2

//��־VNIC�Ƿ��ѳɹ����ù�VLAN/MTU������
#define VNA_VNIC_MON_FLAG_NO                    0
#define VNA_VNIC_MON_FLAG_YES                   1


//��ȡVSI��Ϣ״̬OK
#define VNA_VM_GET_VSI_STATUS_OK(pVNAVMVnicInfo) \
    (((pVNAVMVnicInfo)->GetHasVnicDev() == 0 ) \
    	|| ((pVNAVMVnicInfo)->GetQueryVSIFlag() == VNA_GET_VSI_DETAIL_FLAG_YES ) \
        || ((pVNAVMVnicInfo)->GetQueryVSIStatus() == VNA_GET_VSI_DETAIL_STATUS_OK))

//WDG����״̬OK
#define VNA_VM_WDG_STATUS_OK(pVNAVMVnicInfo) \
    (((pVNAVMVnicInfo)->GetIsWdgEnable() == VNA_VM_WDG_ENABLE_NO) \
        || ((pVNAVMVnicInfo)->GetWdgOperStatus() == VNA_VM_WDG_OP_STATUS_OK))
     
//��ȡVSI��Ϣ״̬��ȷʧ��
#define VNA_VM_GET_VSI_STATUS_FAIL(pVNAVMVnicInfo) \
    (((pVNAVMVnicInfo)->GetHasVnicDev() != 0 ) \
        && ((pVNAVMVnicInfo)->GetQueryVSIFlag() == VNA_GET_VSI_DETAIL_FLAG_NO ) \
        && (((pVNAVMVnicInfo)->GetQueryVSIStatus() == VNA_GET_VSI_DETAIL_STATUS_FAIL)\
        || ((pVNAVMVnicInfo)->GetQueryVSITimes() > VNA_REQUEST_VSI_FROM_VNM_MAX_TIMES) ))
     
//WDG����״̬��ȷʧ��
#define VNA_VM_WDG_STATUS_FAIL(pVNAVMVnicInfo) \
    (((pVNAVMVnicInfo)->GetIsWdgEnable() == VNA_VM_WDG_ENABLE_YES) \
        &&( ((pVNAVMVnicInfo)->GetWdgOperStatus() == VNA_VM_WDG_OP_STATUS_FAIL) \
        || ((pVNAVMVnicInfo)->GetWdgOperTimes() > VNA_VM_WDG_OP_MAX_TIMES)))

//��ȡVSI��Ϣ״̬δ�ɹ���������
#define VNA_VM_GET_VSI_STATUS_NOT_OK(pVNAVMVnicInfo) \
    (((pVNAVMVnicInfo)->GetHasVnicDev() != 0 ) \
        && ((pVNAVMVnicInfo)->GetQueryVSIFlag() == VNA_GET_VSI_DETAIL_FLAG_NO ) \
        && ((pVNAVMVnicInfo)->GetQueryVSIStatus() != VNA_GET_VSI_DETAIL_STATUS_FAIL)\
        && ((pVNAVMVnicInfo)->GetQueryVSITimes() <= VNA_REQUEST_VSI_FROM_VNM_MAX_TIMES) )
//(cur_time - iter->second.GetGetVSILastTime() >= VNA_REQUEST_VSI_FROM_VNM_INTERVAL)

//WDG����״̬δ�ɹ���������
#define VNA_VM_WDG_STATUS_NOT_OK(pVNAVMVnicInfo) \
    (((pVNAVMVnicInfo)->GetIsWdgEnable() == VNA_VM_WDG_ENABLE_YES) \
        && ((pVNAVMVnicInfo)->GetWdgOperStatus() != VNA_VM_WDG_OP_STATUS_FAIL) \
        && ((pVNAVMVnicInfo)->GetWdgOperTimes() <= VNA_VM_WDG_OP_MAX_TIMES))


//VSI EVB��Ϣ״̬OK
#define VNA_VM_EVB_STATUS_OK(pVNAVMVnicInfo) \
    (((pVNAVMVnicInfo)->GetHasVnicDev() == 0) \
        || ((pVNAVMVnicInfo)->GetQueryVSIEvbStatus() == VNA_VM_VSI_EVB_STATUS_SUCCESS ))  
        
//��ȡVSI EVB ��Ϣ״̬��ȷʧ��
#define VNA_VM_VSI_EVB_STATUS_FAIL(pVNAVMVnicInfo) \
    (  ((pVNAVMVnicInfo)->GetHasVnicDev() != 0 ) \
       && (((pVNAVMVnicInfo)->GetQueryVSIEvbStatus() == VNA_VM_VSI_EVB_STATUS_FAILED)\
        || ((pVNAVMVnicInfo)->GetQueryVSIEvbTimes() > VNA_REQUEST_VSI_EVB_MAX_TIMES) ))  
        
//��ȡVSI EVB ��Ϣ״̬δ�ɹ���������
#define VNA_VM_VSI_EVB_STATUS_NOT_OK(pVNAVMVnicInfo) \
    ( ((pVNAVMVnicInfo)->GetHasVnicDev() != 0 ) \
      && ((((pVNAVMVnicInfo)->GetQueryVSIEvbStatus() == VNA_VM_VSI_EVB_STATUS_INIT)\
            ||((pVNAVMVnicInfo)->GetQueryVSIEvbStatus() == VNA_VM_VSI_EVB_STATUS_RUNNING))\
          && ((pVNAVMVnicInfo)->GetQueryVSIEvbTimes() <= VNA_REQUEST_VSI_EVB_MAX_TIMES)  ))
        
class CVnicPortDev:public CPortNetDev
{
public:
    explicit CVnicPortDev()
    {
        m_nMonSetFlag = VNA_VNIC_MON_FLAG_NO;       //��ʶ�Ƿ���Ϊvnic���ù�VLAN/MTU����Ϣ
        m_nVepaFlowSetFlag = 0;
        m_nLastMonSetTime = 0;
    };
    virtual ~CVnicPortDev(){};

    int32 GetNicIndex() const {return m_cVnicDetail.GetNicIndex();};
    string GetVSIIDValue() const {return m_cVnicDetail.GetVSIIDValue();}; 
    int32 GetSwitchPortMode() const {return m_cVnicDetail.GetSwitchportMode();};
    string GetUpPort() const {return m_cVnicDetail.GetPortName();};
    int32 GetCVlan() const {return m_cVnicDetail.GetAccessCvlan();};
    void GetTrunkVlan(vector<CTrunkVlanRange> &vecVlanRange) const {m_cVnicDetail.GetTrunkVlan(vecVlanRange);};
    void GetTrunkVlan(map<uint32, uint32> &mapVlanRange) 
    {
        vector<CTrunkVlanRange> vecVlanRange;
        m_cVnicDetail.GetTrunkVlan(vecVlanRange);

        vector<CTrunkVlanRange>::iterator itrVR = vecVlanRange.begin();
        for ( ; itrVR != vecVlanRange.end(); ++itrVR)
        {
            mapVlanRange.insert(pair<uint32, uint32> ((uint32)(itrVR->m_nVlanBegin), (uint32)(itrVR->m_nVlanEnd)));
        }
    };
    int32 GetNativeVlan()const {return m_cVnicDetail.GetNativeVlan();};
    int32 GetIsSriov() const {return m_cVnicDetail.GetIsSriov();};
    int32 GetMTU() const {return m_cVnicDetail.GetMTU();};
    string GetBridge() const {return m_cVnicDetail.GetSwitchName();};
    string GetMacAddress() const {return m_cVnicDetail.GetMacAddress();};
    string GetAssMac() const {return m_cVnicDetail.GetAssMac();};
    string GetPCI() const {return m_cVnicDetail.GetPciOrder();};
    string GetVifName() const {return m_strVifName;};
    string GetTapName() const {return m_strTapName;};
    string GetEmuName() const {return m_strEmuName;};    
    int32 GetMonSetFlag() const {return m_nMonSetFlag;};
    int32 GetVepaFlowSetFlag() const{return m_nVepaFlowSetFlag;};
    time_t GetLastMonSetTime() const {return m_nLastMonSetTime;};
    void SetVifName(string strVifName) { m_strVifName = strVifName;};
    void SetTapName(string strTapName) { m_strTapName = strTapName;};
    void SetEmuName(string strEmuName) { m_strEmuName = strEmuName;};    
    void SetMonSetFlag(int32 nMonSetFlag) { m_nMonSetFlag = nMonSetFlag;};
    void SetVepaFlowSetFlag(int32 nVepaFlowSetFlag) { m_nVepaFlowSetFlag = nVepaFlowSetFlag;};
    void SetLastMonSetTime(time_t nLastMonSetTime) { m_nLastMonSetTime = nLastMonSetTime;};

    void GetVnicDetail(CVNetVnicDetail &cVnicDetail) const {cVnicDetail = m_cVnicDetail;};
    void GetVnicConfig(CVNetVnicConfig &cVnicConfig) const {m_cVnicDetail.GetVnicConfig(cVnicConfig );};
    void GetVsiDetail(CVNetVSIProfileDetail &cVsiDetail) const {m_cVnicDetail.GetVsiDetail(cVsiDetail );};
    void SetVnicDetail(CVNetVnicDetail &cVnicDetail)  {m_cVnicDetail = cVnicDetail;};
    void SetVnicConfig(CVNetVnicConfig &cVnicConfig)  {m_cVnicDetail.SetVnicConfig(cVnicConfig );};
    void SetVsiDetail(CVNetVSIProfileDetail &cVsiDetail)  {m_cVnicDetail.SetVsiDetail(cVsiDetail );};


    virtual void Accept(CNetDevVisitor *visitor){visitor->VisitDev(this);};
    virtual void Accept(CPortVisitor *visitor){visitor->VisitPort(this);};
    virtual void Accept(CVnicVisitor *visitor){visitor->VisitPort(this);};

    void DbgShow()
    {
        cout <<"CVnicPortDev:" <<endl;
        cout <<"    m_strVifName        :" << m_strVifName<< endl;
        cout <<"    m_strTapName        :" << m_strTapName<< endl;
        cout <<"    m_strEmuName        :" << m_strEmuName<< endl;        
        cout <<"    m_nMonSetFlag       :" << m_nMonSetFlag << endl;
        cout <<"    m_nVepaFlowSetFlag  :" << m_nVepaFlowSetFlag << endl;
        cout <<"    m_nLastMonSetTime   :" << m_nLastMonSetTime << endl;
        m_cVnicDetail.Print();
    };

private:
    string m_strVifName;
    string m_strTapName;
    string m_strEmuName;    
    int32 m_nMonSetFlag;       //��ʶ�Ƿ���Ϊvnic���ù�VLAN/MTU����Ϣ
    int32 m_nVepaFlowSetFlag;       //��ʶ�Ƿ���Ϊvnic���ù�VEPA�������Ϣ��ֻ����һ��
    time_t m_nLastMonSetTime;
    CVNetVnicDetail m_cVnicDetail;

};


class CDeployInfo
{
public:
    CDeployInfo()
    {
        m_nDeployStatus = VNA_VM_DEPLOY_STATUS_NETWORK_INIT;
        m_nOption = 0;
    };
    
    void SetDeployStatus(int32 nDeployStatus){m_nDeployStatus = nDeployStatus;};
    void SetActionID(string strActionID){m_strActionID = strActionID;};
    void SetDeployOption(int32 nOption){m_nOption = nOption;};
    void SetTaskMID(MID cTaskMID){m_cTaskMID = cTaskMID;};
    int32 GetDeployStatus()const{return m_nDeployStatus;};
    string GetActionID()const{return m_strActionID;};
    int32 GetDeployOption()const{return m_nOption;};
    MID GetTaskMID()const{return m_cTaskMID;};

    void Print()
    {
        cout << "   m_nDeployStatus    :"<< m_nDeployStatus<<endl;
        cout << "   m_strActionID      :"<< m_strActionID<<endl;
        cout << "   m_cTaskMid         :"<< m_cTaskMID._application <<m_cTaskMID._process << m_cTaskMID._unit <<endl;
        cout << "   m_nOption          :"<< m_nOption<<endl;
    };
private:
    int32  m_nDeployStatus;              //��ʶ��ǰ�Ĳ���״̬��startnetwork�У�stopnetwork�У�������
    string m_strActionID;                //��¼��ǰ���й�������ID
    MID    m_cTaskMID;
    int32  m_nOption;                    //����ѡ��
};

class CVsiTaskInfo
{
public:
    CVsiTaskInfo()
    {
        m_nQueryVSIFlag = VNA_GET_VSI_DETAIL_FLAG_NO;
        m_nQueryVSILastTime = 0;
        m_nQueryVSITimes = 0;
        m_nQueryVSIStatus = VNA_GET_VSI_DETAIL_STATUS_INIT;
    };
    
    void SetQueryVSIFlag(int32 nQueryVSIFlag){m_nQueryVSIFlag = nQueryVSIFlag;};
    void SetQueryVSILastTime(time_t nQueryVSILastTime){m_nQueryVSILastTime = nQueryVSILastTime;};
    void SetQueryVSITimes(int32 nQueryVSITimes){m_nQueryVSITimes = nQueryVSITimes;};
    void SetQueryVSIStatus(int32 nQueryVSIStatus){m_nQueryVSIStatus = nQueryVSIStatus;};
    int32 GetQueryVSIFlag()const{return m_nQueryVSIFlag;};
    time_t GetQueryVSILastTime()const{return m_nQueryVSILastTime;};
    int32 GetQueryVSITimes()const{return m_nQueryVSITimes;};
    int32 GetQueryVSIStatus()const{return m_nQueryVSIStatus;};

    void Print()
    {
        cout << "   m_nQueryVSIFlag    :"<< m_nQueryVSIFlag<<endl;
        cout << "   m_nQueryVSILastTime:"<< m_nQueryVSILastTime<<endl;
        cout << "   m_nQueryVSITimes   :"<< m_nQueryVSITimes<<endl;
        cout << "   m_nQueryVSIStatus  :"<< m_nQueryVSIStatus<<endl;
    };
private:
    int32  m_nQueryVSIFlag;                //��ʶ�Ƿ��Ѵ�VNM�ϻ�ȡ��ϸ��Ϣ
    time_t m_nQueryVSILastTime;            //��ʶ���һ����VNM����VSI������Ϣʱ��
    int32  m_nQueryVSITimes;               //��ʶ��VNM����VSI�������
    int32  m_nQueryVSIStatus;             //�����Ҫ�������Ź�����¼�Ƿ����ɹ� 0:��Ӧ��1:�ɹ� 2:ʧ��
};


class CVsiEvbTaskInfo
{
public:
    CVsiEvbTaskInfo()
    {
        m_nQueryVSIEvbLastTime = 0;
        m_nQueryVSIEvbTimes = 0;
        m_nQueryVSIEvbStatus = VNA_VM_VSI_EVB_STATUS_INIT;
    };
    
    void SetQueryVSIEvbLastTime(time_t nQueryVSILastTime){m_nQueryVSIEvbLastTime = nQueryVSILastTime;};
    void SetQueryVSIEvbTimes(int32 nQueryVSITimes){m_nQueryVSIEvbTimes = nQueryVSITimes;};
    void SetQueryVSIEvbStatus(int32 nQueryVSIStatus){m_nQueryVSIEvbStatus = nQueryVSIStatus;};
    time_t GetQueryVSIEvbLastTime()const{return m_nQueryVSIEvbLastTime;};
    int32 GetQueryVSIEvbTimes()const{return m_nQueryVSIEvbTimes;};
    int32 GetQueryVSIEvbStatus()const{return m_nQueryVSIEvbStatus;};

    void Print()
    {        
        cout << "   m_nQueryVSIEvbStatus  :"<< m_nQueryVSIEvbStatus<<endl;
        cout << "   m_nQueryVSIEvbLastTime:"<< m_nQueryVSIEvbLastTime<<endl;
        cout << "   m_nQueryVSIEvbTimes   :"<< m_nQueryVSIEvbTimes<<endl;
    };
private:
    time_t m_nQueryVSIEvbLastTime;            //��ʶ���һ����VNM����VSI������Ϣʱ��
    int32  m_nQueryVSIEvbTimes;               //��ʶ��ѯVSIEVB״̬ ����
    int32  m_nQueryVSIEvbStatus;              //��ʶ�Ƿ� EVB Э�̽�� 0: ��ʼ̬; 1: Э�̳ɹ�; 2 :Э��ʧ��; 3:Э����
};

class CWdgTaskInfo
{
public:
    CWdgTaskInfo()
    {
        m_nIsWdgEnable = VNA_VM_WDG_ENABLE_NO;
        m_nWdgTimeOut = 0;
        m_nWdgOperTimes = 0;
        m_nWdgOperStatus = VNA_VM_WDG_OP_STATUS_INIT;
    };
        
    void SetIsWdgEnable(int32 nIsWdgEnable){m_nIsWdgEnable = nIsWdgEnable;};
    void SetWdgTimeOut(int32 nWdgTimeOut){m_nWdgTimeOut = nWdgTimeOut;};
    void SetWdgOperTimes(int32 nWdgOperTimes){m_nWdgOperTimes = nWdgOperTimes;};
    void SetWdgOperStatus(int32 nWdgOperStatus){m_nWdgOperStatus = nWdgOperStatus;};
    int32 GetIsWdgEnable()const{return m_nIsWdgEnable;};
    int32 GetWdgTimeOut()const{return m_nWdgTimeOut;};
    int32 GetWdgOperTimes()const{return m_nWdgOperTimes;};
    int32 GetWdgOperStatus()const{return m_nWdgOperStatus;};

    void Print()
    {
        cout << "   m_nIsWdgEnable      :"<< m_nIsWdgEnable<<endl;
        cout << "   m_nWdgTimeOut       :"<< m_nWdgTimeOut<<endl;
        cout << "   m_nWdgOperTimes     :"<< m_nWdgOperTimes<<endl;
        cout << "   m_nWdgOperStatus    :"<< m_nWdgOperStatus<<endl;
    };
private:
    int32  m_nIsWdgEnable;
    int32  m_nWdgTimeOut;
    int32  m_nWdgOperTimes;              //�����Ҫ�������Ź�����¼��������
    int32  m_nWdgOperStatus;             //�����Ҫ�������Ź�����¼�Ƿ����ɹ� 0:��Ӧ��1:�ɹ� 2:ʧ��
};

//����VNA��һ��VM��ص�vnic��Ϣ
class CVNAVMVnicInfo
{
public:
    CVNAVMVnicInfo()
    {
        m_nVMID = -1; 
        m_nProjectID = -1; 
        m_nDomainID = -1;   
        m_vecVnicInfo.clear();
    };
    
    void SetVMID(int64 nVMID){m_nVMID = nVMID;};
    void SetProjectID(int64 nProjectID){m_nProjectID = nProjectID;};
    void SetDomainID(int nDomainID){ m_nDomainID = nDomainID;};
    void SetDeployStatus(int32 nDeployStatus){m_cDeployInfo.SetDeployStatus(nDeployStatus);};
    void SetActionID(string strActionID){m_cDeployInfo.SetActionID(strActionID);};
    void SetDeployMID(MID cTaskMID){m_cDeployInfo.SetTaskMID(cTaskMID);};
    void SetDeployOption(int32 nOption){m_cDeployInfo.SetDeployOption(nOption);};
    void SetQueryVSIFlag(int32 nQueryVSIFlag){m_cVsiTaskInfo.SetQueryVSIFlag(nQueryVSIFlag);};
    void SetQueryVSILastTime(time_t nQueryVSILastTime){m_cVsiTaskInfo.SetQueryVSILastTime(nQueryVSILastTime);};
    void SetQueryVSITimes(int32 nQueryVSITimes){m_cVsiTaskInfo.SetQueryVSITimes(nQueryVSITimes);};
    void SetQueryVSIStatus(int32 nQueryVSIStatus){m_cVsiTaskInfo.SetQueryVSIStatus(nQueryVSIStatus);};
    
    void SetQueryVSIEvbLastTime(time_t nQueryVSILastTime){m_cVsiEvbTaskInfo.SetQueryVSIEvbLastTime(nQueryVSILastTime);};
    void SetQueryVSIEvbTimes(int32 nQueryVSITimes){m_cVsiEvbTaskInfo.SetQueryVSIEvbTimes(nQueryVSITimes);};
    void SetQueryVSIEvbStatus(int32 nQueryVSIStatus){m_cVsiEvbTaskInfo.SetQueryVSIEvbStatus(nQueryVSIStatus);};
    
    void SetIsWdgEnable(int32 nIsWdgEnable){m_cWdgTaskInfo.SetIsWdgEnable(nIsWdgEnable);};
    void SetWdgTimeOut(int32 nWdgTimeOut){m_cWdgTaskInfo.SetWdgTimeOut(nWdgTimeOut);};
    void SetWdgOperTimes(int32 nWdgOperTimes){m_cWdgTaskInfo.SetWdgOperTimes(nWdgOperTimes);};
    void SetWdgOperStatus(int32 nWdgOperStatus){m_cWdgTaskInfo.SetWdgOperStatus(nWdgOperStatus);};
    void SetWdgVnicInfo(CVnetVnicXML &cWdgVnic){m_cWdgVnic = cWdgVnic;};
    
    
    void SetVnicInfo(vector<CVnicPortDev> &vecVnicInfo){ m_vecVnicInfo = vecVnicInfo;};    
    void SetVnicInfo(vector<CVNetVnicDetail> &vecVnicDetail)
    {
        m_vecVnicInfo.clear();
        
        vector<CVNetVnicDetail>::iterator itrVnic = vecVnicDetail.begin();
        for ( ; itrVnic != vecVnicDetail.end(); ++itrVnic)
        {
            CVnicPortDev cVnicPortDev;
            cVnicPortDev.SetVnicDetail(*itrVnic);

            m_vecVnicInfo.push_back(cVnicPortDev);
        }
    };

    void SetVnicInfo(vector<CVNetVnicConfig> &vecVmNicCfg)
    {
        m_vecVnicInfo.clear();
        
        vector<CVNetVnicConfig>::iterator itrVnic = vecVmNicCfg.begin();
        for ( ; itrVnic != vecVmNicCfg.end(); ++itrVnic)
        {
            CVnicPortDev cVnicPortDev;
            cVnicPortDev.SetVnicConfig(*itrVnic);
            
            m_vecVnicInfo.push_back(cVnicPortDev);   //�������������
        }
    };
        
    void UpdateVnicDetail(vector<CVNetVnicDetail> &vecVnicDetail)
    {
        vector<CVNetVnicDetail>::iterator itrVnic = vecVnicDetail.begin();
        for ( ; itrVnic != vecVnicDetail.end(); ++itrVnic)
        {        
            vector<CVnicPortDev>::iterator itrVnicPort =  m_vecVnicInfo.begin();
            for ( ; itrVnicPort !=  m_vecVnicInfo.end(); ++itrVnicPort)
            {
                if (itrVnicPort->GetNicIndex() == itrVnic->GetNicIndex())
                {
                    itrVnicPort->SetVnicDetail(*itrVnic);
                    break;
                }
            }

            if (itrVnicPort == m_vecVnicInfo.end())
            {
                CVnicPortDev portdev;
                portdev.SetVnicDetail(*itrVnic);
                m_vecVnicInfo.push_back(portdev);
            }
        }

        SetVnicDevName();
    };

    void SetVnicDevName()
    {
        int i = 0;
        ostringstream oss,vifoss,taposs, emuoss;

        int nDomID = GetDomainID();
        if (nDomID <= 0)
        {
            VNET_LOG(VNET_LOG_ERROR, "SetVnicDevName: domid(%d) is invalid\n", nDomID);
            return ;
        }

        //����OVS��VM��Ӧ�˿ڵ�VLAN
        vector<CVnicPortDev>::iterator itrVnicPort =  m_vecVnicInfo.begin();
        for ( ; itrVnicPort !=  m_vecVnicInfo.end(); ++itrVnicPort)
        {
            if (!itrVnicPort->GetIsSriov())
            {
                //ִ��ovs_vsctl �������ö˿�vlan��Ϣ
                vifoss.str("");
                vifoss<< "vif" << nDomID << "." << i;
                itrVnicPort->SetVifName(vifoss.str());

                //ִ��ovs_vsctl �������ö˿�vlan��Ϣ
                taposs.str("");
                taposs<< "tap" << nDomID << "." << i;
                itrVnicPort->SetTapName(taposs.str());

                emuoss.str("");
                emuoss<< "vif" << nDomID << "." << i << "-emu";
                itrVnicPort->SetEmuName(emuoss.str());                

                i++;
            }
        }

        return;
    };

    int64 GetVMID() const { return m_nVMID;};
    int64 GetProjectID() const { return m_nProjectID;};
    int   GetDomainID() const { return m_nDomainID;};
    int32 GetDeployStatus() const {return m_cDeployInfo.GetDeployStatus();};
    string GetActionID() const {return m_cDeployInfo.GetActionID();};
    int32 GetDeployOption() const {return m_cDeployInfo. GetDeployOption();};
    MID GetDeployMID() const {return m_cDeployInfo.GetTaskMID();};
    int32 GetQueryVSIFlag() const {return m_cVsiTaskInfo.GetQueryVSIFlag();};
    time_t GetQueryVSILastTime() const {return m_cVsiTaskInfo.GetQueryVSILastTime();};
    int32 GetQueryVSITimes() const {return m_cVsiTaskInfo.GetQueryVSITimes();};
    int32 GetQueryVSIStatus() const {return m_cVsiTaskInfo.GetQueryVSIStatus();};

    time_t GetQueryVSIEvbLastTime() const {return m_cVsiEvbTaskInfo.GetQueryVSIEvbLastTime();};
    int32 GetQueryVSIEvbTimes() const {return m_cVsiEvbTaskInfo.GetQueryVSIEvbTimes();};
    int32 GetQueryVSIEvbStatus() const {return m_cVsiEvbTaskInfo.GetQueryVSIEvbStatus();};
    
    int32 GetIsWdgEnable() const {return m_cWdgTaskInfo.GetIsWdgEnable();};
    int32 GetWdgTimeOut() const {return m_cWdgTaskInfo.GetWdgTimeOut();};
    int32 GetWdgOperTimes() const {return m_cWdgTaskInfo.GetWdgOperTimes();};
    int32 GetWdgOperStatus() const {return m_cWdgTaskInfo.GetWdgOperStatus();};
    void GetVnicInfo(vector<CVnicPortDev> &vecVnicInfo) const { vecVnicInfo.clear() ;vecVnicInfo = m_vecVnicInfo;};
    int GetHasVnicDev(){return m_vecVnicInfo.size();};
    void GetVnicXMLInfo(vector<CVnetVnicXML> &vecVnicXML)
    {
        vecVnicXML.clear() ;

        vector<CVnicPortDev>::iterator itrVnicPort =  m_vecVnicInfo.begin();
        for ( ; itrVnicPort !=  m_vecVnicInfo.end(); ++itrVnicPort)
        {
            CVNetVnicDetail cVnicDetail;
            itrVnicPort->GetVnicDetail(cVnicDetail);

            CVnetVnicXML cVnicXML;
            cVnicDetail.GetVnicConfig(cVnicXML.m_cVnicConfig);
            cVnicXML.m_strPCIOrder = cVnicDetail.GetPciOrder();                  //SRIOVPCI,�����
            cVnicXML.m_strBridgeName = cVnicDetail.GetSwitchName();
            if(0 != cVnicDetail.GetAssIp().size())
            {
                cVnicXML.m_strIP = cVnicDetail.GetAssIp();
                cVnicXML.m_strNetmask = cVnicDetail.GetAssMask();
                cVnicXML.m_strGateway = cVnicDetail.GetAssGateway();
            }
            else
            {
                cVnicXML.m_strIP = cVnicDetail.GetIPAddress();
                cVnicXML.m_strNetmask = cVnicDetail.GetNetmask();
                cVnicXML.m_strGateway = cVnicDetail.GetGateway();
            }
            if(0 != cVnicDetail.GetAssMac().size())
            {
                cVnicXML.m_strMac = cVnicDetail.GetAssMac();
            }
            else
            {
                cVnicXML.m_strMac = cVnicDetail.GetMacAddress();
            }
            cVnicXML.m_nCvlan = cVnicDetail.GetAccessCvlan();

            vecVnicXML.push_back(cVnicXML);
        }
    };

    void GetWdgVnicXMLInfo(CVnetVnicXML &cVnicXML)
    {
        if (GetIsWdgEnable() == VNA_VM_WDG_ENABLE_YES)
        {
            cVnicXML = m_cWdgVnic;
        }
    }
    
    void GetVsiDetailInfo(vector<CVNetVSIProfileDetail> &vecVsiInfo) 
    {
        vector<CVnicPortDev>::iterator itrVnicPort =  m_vecVnicInfo.begin();
        for ( ; itrVnicPort !=  m_vecVnicInfo.end(); ++itrVnicPort)
        {
            CVNetVSIProfileDetail cVsiDetail;
            itrVnicPort->GetVsiDetail(cVsiDetail);
            vecVsiInfo.push_back(cVsiDetail);
        }
    };
    
    void GetVnicDetailInfo(vector<CVNetVnicDetail> &vecVnicInfo) 
    {
        vector<CVnicPortDev>::iterator itrVnicPort =  m_vecVnicInfo.begin();
        for ( ; itrVnicPort !=  m_vecVnicInfo.end(); ++itrVnicPort)
        {
            CVNetVnicDetail cVnicDetail;
            itrVnicPort->GetVnicDetail(cVnicDetail);
            vecVnicInfo.push_back(cVnicDetail);
        }
    };

    void GetVnicCfgInfo(vector<CVNetVnicConfig> &vecVnicCfgInfo) 
    {
        vector<CVnicPortDev>::iterator itrVnicPort =  m_vecVnicInfo.begin();
        for ( ; itrVnicPort !=  m_vecVnicInfo.end(); ++itrVnicPort)
        {
            CVNetVnicConfig cVnicCfgInfo;
            itrVnicPort->GetVnicConfig(cVnicCfgInfo);
            vecVnicCfgInfo.push_back(cVnicCfgInfo);
        }
    };

    virtual void Accept(CNetDevVisitor *visitor)
    {
        vector<CVnicPortDev>::iterator itrVnicPort =  m_vecVnicInfo.begin();
        for ( ; itrVnicPort !=  m_vecVnicInfo.end(); ++itrVnicPort)
        {
            itrVnicPort->Accept(visitor);
        }
    };
    
    virtual void Accept(CPortVisitor *visitor)
        {
        vector<CVnicPortDev>::iterator itrVnicPort =  m_vecVnicInfo.begin();
        for ( ; itrVnicPort !=  m_vecVnicInfo.end(); ++itrVnicPort)
        {
            itrVnicPort->Accept(visitor);
        }
    };
    
    virtual void Accept(CVnicVisitor *visitor)
    {
        vector<CVnicPortDev>::iterator itrVnicPort =  m_vecVnicInfo.begin();
        for ( ; itrVnicPort !=  m_vecVnicInfo.end(); ++itrVnicPort)
        {
            itrVnicPort->Accept(visitor);
        }
    };

    void Print()
    {
        cout << "VM VNIC Information:" <<endl;    
        cout << "    VM ID          :" << m_nVMID <<endl;    
        cout << "    Project ID     :" << m_nProjectID <<endl;    
        cout << "    DomainID ID    :" << m_nDomainID <<endl;    
        m_cDeployInfo.Print();
        m_cVsiTaskInfo.Print();
        m_cVsiEvbTaskInfo.Print();
        m_cWdgTaskInfo.Print();
        vector<CVnicPortDev>::iterator itrVnicPort =  m_vecVnicInfo.begin();
        for ( ; itrVnicPort !=  m_vecVnicInfo.end(); ++itrVnicPort)
        {
            itrVnicPort->DbgShow();
        }
        
        cout <<endl;    
    };
    
private:
    int64  m_nVMID;                      //�ɼ�����д�������id
    int64  m_nProjectID;                 //�ɼ�����д������id
    int    m_nDomainID;                  //DOMAINID
    CDeployInfo m_cDeployInfo;
    CVsiTaskInfo m_cVsiTaskInfo;
    CVsiEvbTaskInfo m_cVsiEvbTaskInfo;
    CWdgTaskInfo m_cWdgTaskInfo;
    vector<CVnicPortDev> m_vecVnicInfo;  //�������������
    CVnetVnicXML m_cWdgVnic;
};

class CVNAVnicPool: public CNetDevMgr
{
public:
    explicit CVNAVnicPool();
    virtual ~CVNAVnicPool();

public:
    int32 Init(MessageUnit  *mu);
    static CVNAVnicPool *GetInstance()
    {
        if(NULL == s_pInstance)
        {
            s_pInstance = new CVNAVnicPool();
        }

        return s_pInstance;
    };
    
    CVNAVMVnicInfo*  AddVMVSIInfo(CVMVSIInfo & cVMVSIInfo);
    void DelVMVSIInfo(const int64 nVMID);
    CVNAVMVnicInfo* GetVMVSIInfo(const int64 nVMID);
    CVNAVMVnicInfo* AddVMVnicInfo(CVNetStartNetworkMsg &cMsg);
    void AddVMVnicInfo(CVNetVMVSIMsg &cMsg);
    void CheckVMVSIInfo();
    int32 SetWdgStatusInfo(CVNetWdgAckMsg &cMsg);
    void UpdateDomID(int64 nVmID, int nDomID);
    void DoMonitor(CNetDevVisitor *dev_visit);
    void DoMonitor(CPortVisitor *dev_visit);
    void DoMonitor(CVnicVisitor *dev_visit);
    void GetMacFromPortName(string port_name, TPortInfo &port_info);
    void DbgShowData(int64 vm_id);
    void SetDbgFlag(int bDbgFlag);
    void ClearAll(){m_mapVMVnicInfo.clear();};    //������

private:
    static CVNAVnicPool *s_pInstance;

    BOOL m_bOpenDbgInf;
    MessageUnit *m_pMU;
    TIMER_ID m_tCheckVMVSITimerId;
    map<int64, CVNAVMVnicInfo> m_mapVMVnicInfo;  

    DISALLOW_COPY_AND_ASSIGN(CVNAVnicPool);
};

}
#endif 

