
/******************************************************************************
* Copyright (c) 2012，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：vnm_schedule.h
* 文件标识：
* 内容摘要：CVNMSchedule类的定义文件
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
#if !defined(VNM_SCHEDULE_INCLUDE_H__)
#define VNM_SCHEDULE_INCLUDE_H__

namespace zte_tecs
{

class CVNetVmDB;
class CVNetVnicDB;
class CVSIProfileMgr;
class CSwitchMgr;
class CVNAManager;
class CDbSchedule;

typedef enum tagQueryDbSchType
{
    SCHE_NORMAL = 0,
    SCHE_BOND,
    SCHE_VXLAN,   
}QueryDbSchType;

typedef struct tagScheRet
{
    int32 nReqNum;                      //需求个数
    vector<string>      vecSchValid;    //存放满足条件的结果(CC或者HC列表)
    vector<string>      vecVNAValid;    //存放满足条件的VNA结果(VNA列表)
    vector<CDbSchedule> vecDBSch;       //存放满足条件的CDbSchedule结果
}ScheRet;

#define VNET_SCHEDULE_TYPE_CC       1
#define VNET_SCHEDULE_TYPE_HC       2

#define VNET_LOOP_DEPLOYED_FLAG_NO       0
#define VNET_LOOP_DEPLOYED_FLAG_YES       1


class CScheduleTaskInfo
{
public:
    void SetScheduleType(int32 nScheduleType){m_nScheduleType = nScheduleType;};
    void SetWatchDogEnable(int32 nWatchDogEnable){m_nWatchDogEnable = nWatchDogEnable;};
    void SetAppointCCApp(string strAppointCCApp){m_strAppointCCApp = strAppointCCApp;};
    void SetAppointVNAUuid(string strAppointVNAUuid){m_strAppointVNAUuid = strAppointVNAUuid;};
    int32 GetScheduleType(){return m_nScheduleType;};
    int32 GetWatchDogEnable(){return m_nWatchDogEnable;};
    string GetAppointCCApp(){return m_strAppointCCApp;};
    string GetAppointVNAUuid(){return m_strAppointVNAUuid;};
    
private:
    int32 m_nScheduleType;
    int32 m_nWatchDogEnable;
    string m_strAppointCCApp;
    string m_strAppointVNAUuid;
};

//MTU info
class CReqNumInfo
{
public:
    CReqNumInfo()
    {
        m_nMTU = VNET_COMMON_MTU;         
        m_nSourceNum = 0;   
        vecSchResult.clear();
    };
    
    void Print()
    {
        cout << "MTUNumInfo:" <<endl;
        cout << "   m_nMTU:" << m_nMTU <<endl;
        cout << "   m_strLogicNetwork:" << m_strLogicNetwork <<endl;
        cout << "   m_nSourceNum:" << m_nSourceNum <<endl;
        
        vector<CDbSchedule>::iterator itrSch = vecSchResult.begin();
        for ( ; itrSch != vecSchResult.end(); ++itrSch)
        {
            itrSch->DbgShow();
        }
        cout <<"<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl;
    };
    
public:
    int32 m_nMTU;
    string m_strLogicNetwork;     //对于loop资源，不同logicnetwork，如果MTU相同，在这里也按logicnetwork分类
    int32 m_nSourceNum;           //需要分配的资源数目
    string m_strDeployedDvs;      //已部署loop对应DVS
    set<vector<CVNetVnicMem>::iterator> m_setVnicIter;  //保存对应虚拟网口在VM vector<CVNetVnicMem>中的iterator游标
    vector<CDbSchedule> vecSchResult;
};

class CVNAFreeSumInfo
{
public:
    CVNAFreeSumInfo()
    {
        m_nFreeSum = 0;         
    };

    void Print()
    {
        cout << "CVNAFreeSumInfo:" <<endl;
        cout << "   m_strVnaUuid:" << m_strVnaUuid <<endl;
        cout << "   m_strCCApp:" << m_strCCApp <<endl;
        cout << "   m_strHCApp:" << m_strHCApp <<endl;
        cout << "   m_nFreeSum:" << m_nFreeSum <<endl;
        cout <<"<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl;
    };
        
public:
    string m_strVnaUuid;
    string m_strCCApp;
    string m_strHCApp;
    int32 m_nFreeSum;           //需要分配的资源数目
};


//VM部署资源调度汇总信息
class CScheduleInfo
{
public:
    CScheduleInfo()
    {
        m_nIsLoop = 0;         
        m_nIsSriov = 0;
        m_nIsSdn = 0;
        m_vecMTUNum.clear();
    };

    string GetNetplane(){return m_strNetplane;};
    int32 GetIsLoop(){return m_nIsLoop;};
    int32 GetIsSriov(){return m_nIsSriov;};
    int32 GetIsSdn(){return m_nIsSdn;};
    void SetNetplane(string value){ m_strNetplane = value;};
    void SetIsLoop(int32 value){ m_nIsLoop = value;};         
    void SetIsSriov(int32 value){ m_nIsSriov = value;};
    void SetIsSdn(int32 value){ m_nIsSdn = value;};
    
    int32 GetSourceReqNum(vector<CReqNumInfo> &vecMTUNum);
    int32 GetScheduleResult(CScheduleTaskInfo &cSchTaskInfo, vector<string> &vecScheduledValid);
    int32 GetDVSAllocResult(string &strVNAUuid, int64 nProjectID, CScheduleTaskInfo &cSchTaskInfo, map< string, set<int32> > &mapVnicSriov);
    int32 GetScheResult(CScheduleTaskInfo &cSchTaskInfo, vector<CReqNumInfo> &vecMTUNum, vector<ScheRet> &vecScheRet, int32 nQueryType);
    int32 QueryDbScheduleInfo(CScheduleTaskInfo &cSchTaskInfo, vector<CReqNumInfo> &vecMTUNum, int32 nQueryType);
    void  GetDBScheduleResult(int32 nScheduleType, vector<CReqNumInfo> &vecMTUNum, vector<string> &vecSchValid, vector<string> &vecVNAValid, vector<CDbSchedule> &vecScheduleFinal);
    void  UpdateDBRes(vector<CDbSchedule> &vecNewSch, CDbSchedule &cDbSch);
    void  MergeAllocDBResult(vector<CDbSchedule> &vecOldSch, vector<CDbSchedule> &vecNewSch);
    void  GetBondSchResult(vector<CDbSchedule> &vecRawDB, CDbSchedule &cMaxDB);
    int32 GetSDVSRes(vector<CReqNumInfo> &vecMTUNum);
    int32 GetEDVSRes(vector<CReqNumInfo> &vecMTUNum, map< string, set<int32> > &mapVnicSriov,vector<CDbSchedule> &vecDbBond);
    int32 GetEDVSBondRes(vector<CReqNumInfo> &vecMTUNum, map< string, set<int32> > &mapVnicSriov,vector<CDbSchedule> &vecDbBond);
    int32 GetLoopDVSRes(string &strVNAUuid, int64 nProjectID, vector<CReqNumInfo> &vecMTUNum, map< string, set<int32> > &mapVnicSriov);
    int32 AllocDVSRes(string &strVNAUuid, int64 nProjectID, vector<CReqNumInfo> &vecMTUNum, int32 nType, map< string, set<int32> > &mapVnicSriov, vector<CDbSchedule> &vecDbBond);
    void CoutSriovReqInfo();
    int32 GetMergeSchResult(int32 nScheduleType, vector<ScheRet> &vecScheRet, vector<string> &vecSchValid);
    void AddMTUNum(string strLGNetworkUuid, int32 nMTU, vector<CVNetVnicMem>::iterator iter=(vector<CVNetVnicMem>::iterator)NULL, int32 nIsolateType=0);
    void AddMTUNum(int32 nMTU, vector<CVNetVnicMem>::iterator iter=(vector<CVNetVnicMem>::iterator)NULL, int32 nIsolateType=0);

    void Print()
    {
        cout << "CVNETScheduleInfo:" <<endl;
        cout << "   m_strNetplane:" << m_strNetplane <<endl;
        cout << "   m_nIsLoop:" << m_nIsLoop <<endl;
        cout << "   m_nIsSriov:" << m_nIsSriov <<endl;
        cout << "   m_nIsSdn:" << m_nIsSdn <<endl;

        cout << "Normal MTUNumInfo list:" <<endl;
        vector<CReqNumInfo>::iterator itrSch = m_vecMTUNum.begin();
        for ( ; itrSch != m_vecMTUNum.end(); ++itrSch)
        {
            itrSch->Print();
        }
        cout << "SRIOV bond MTUNumInfo list:" <<endl;
        vector<CReqNumInfo>::iterator itrSriov = m_vecSriovBondMTUNum.begin();
        for(; itrSriov != m_vecSriovBondMTUNum.end(); ++itrSriov)
        {
            itrSriov->Print();
        }
        cout << "VXLAN MTUNumInfo list:" <<endl;
        vector<CReqNumInfo>::iterator itrVxlan = m_vecVxlanMTUNum.begin();
        for(; itrVxlan != m_vecVxlanMTUNum.end(); ++itrVxlan)
        {
            itrVxlan->Print();
        }        
        cout <<"<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl;
        cout <<endl;
    };

public:
    string m_strNetplane;
    int32 m_nIsLoop;         
    int32 m_nIsSriov;
    int32 m_nIsSdn;
    vector<CReqNumInfo> m_vecMTUNum;     //按MTU从小到大排序
    vector<CReqNumInfo> m_vecTempSriovMTUNum;     //按MTU从小到大排序
    vector<CReqNumInfo> m_vecSriovBondMTUNum;      //汇总bond 或者LOOP的SRIOV网口需求，按逻辑网络统计
    vector<CReqNumInfo> m_vecVxlanMTUNum;     //Vxlan的MTU, 按MTU从小到大排序
    
};

#if 0

//VM部署资源调度汇总信息
class CVnicResInfo
{
public:
    CVnicResInfo()
    {
        m_nIsLoop = 0;         
        m_nIsSriov = 0;
        m_vecMTUNum.clear();
    };

    string GetNetplane(){return m_strNetplane;};
    int32 GetIsLoop(){return m_nIsLoop;};
    int32 GetIsSriov(){return m_nIsSriov;};
    void SetNetplane(string value){ m_strNetplane = value;};
    void SetIsLoop(int32 value){ m_nIsLoop = value;};         
    void SetIsSriov(int32 value){ m_nIsSriov = value;};
    
    int32 GetSourceReqNum(vector<CReqNumInfo> &vecMTUNum);
    int32 QueryDbScheduleInfo(CScheduleTaskInfo &cSchTaskInfo, vector<CReqNumInfo> &vecMTUNum, int32 nIsBond);
    void CoutSriovReqInfo();

    void Print()
    {
        cout << "CVNETScheduleInfo:" <<endl;
        cout << "   m_strNetplane:" << m_strNetplane <<endl;
        cout << "   m_nIsLoop:" << m_nIsLoop <<endl;
        cout << "   m_nIsSriov:" << m_nIsSriov <<endl;
        
        vector<CReqNumInfo>::iterator itrSch = m_vecMTUNum.begin();
        for ( ; itrSch != m_vecMTUNum.end(); ++itrSch)
        {
            itrSch->Print();
        }
        cout <<"<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl;
    };

public:
    string m_strNetplane;
    int32 m_nIsLoop;         
    int32 m_nIsSriov;
    vector<CReqNumInfo> m_vecMTUNum;     //按MTU从小到大排序
    vector<CReqNumInfo> m_vecTempSriovMTUNum;     //按MTU从小到大排序
    vector<CReqNumInfo> m_vecSriovBondMTUNum;      //汇总bond 或者LOOP的SRIOV网口需求，按逻辑网络统计
};


class CScheduleInfo: public CVnicResInfo
{
public:
    CScheduleInfo()
    {
        m_nIsLoop = 0;         
        m_nIsSriov = 0;
        m_vecMTUNum.clear();
    };

    int32 GetScheduleResult(CScheduleTaskInfo &cSchTaskInfo, set<string> &setScheduledValid);
    void GetDBScheduleResult(int32 nScheduleType, vector<CReqNumInfo> &vecMTUNum, set<string> &setVNAValid);
    void AddMTUNum(string strLGNetworkUuid, int32 nMTU);
    void AddMTUNum(int32 nMTU);

    void Print()
    {
        cout << "CVNETScheduleInfo:" <<endl;
        cout << "   m_strNetplane:" << m_strNetplane <<endl;
        cout << "   m_nIsLoop:" << m_nIsLoop <<endl;
        cout << "   m_nIsSriov:" << m_nIsSriov <<endl;
        
        vector<CReqNumInfo>::iterator itrSch = m_vecMTUNum.begin();
        for ( ; itrSch != m_vecMTUNum.end(); ++itrSch)
        {
            itrSch->Print();
        }
        cout <<"<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl;
    };

};

class CDVSAllocInfo: public CVnicResInfo
{
public:
    CVnicResInfo()
    {
        m_nIsLoop = 0;         
        m_nIsSriov = 0;
        m_vecMTUNum.clear();
    };
    
    int32 GetDVSAllocResult(CScheduleTaskInfo &cSchTaskInfo);
    int32 AllocDVSRes(vector<CReqNumInfo> &vecMTUNum, int32 nIsBond);
    void AddMTUNum(string strLGNetworkUuid, int32 nMTU, vector<CVNetVnicMem>::iterator iter=(vector<CVNetVnicMem>::iterator)NULL);
    void AddMTUNum(int32 nMTU, vector<CVNetVnicMem>::iterator iter=(vector<CVNetVnicMem>::iterator)NULL);

    void Print()
    {
        cout << "CVNETScheduleInfo:" <<endl;
        cout << "   m_strNetplane:" << m_strNetplane <<endl;
        cout << "   m_nIsLoop:" << m_nIsLoop <<endl;
        cout << "   m_nIsSriov:" << m_nIsSriov <<endl;
        
        vector<CReqNumInfo>::iterator itrSch = m_vecMTUNum.begin();
        for ( ; itrSch != m_vecMTUNum.end(); ++itrSch)
        {
            itrSch->Print();
        }
        cout <<"<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl;
    };

public:
    string m_strNetplane;
    int32 m_nIsLoop;         
    int32 m_nIsSriov;
    vector<CReqNumInfo> m_vecMTUNum;     //按MTU从小到大排序
    vector<CReqNumInfo> m_vecTempSriovMTUNum;     //按MTU从小到大排序
    vector<CReqNumInfo> m_vecSriovBondMTUNum;      //汇总bond 或者LOOP的SRIOV网口需求，按逻辑网络统计
};

#endif

//调度主控类
class CVNMSchedule: public MessageHandler
{
public:
    explicit CVNMSchedule();
    virtual ~CVNMSchedule();
    
    int32 Init();
    STATUS StartMu(string strMsgUnitName);
    void MessageEntry(const MessageFrame &message);
    void DbgShowData(void);
    void SetDbgFlag(int bDbgFlag);

    static CVNMSchedule *GetInstance()
    {
        if(NULL == s_pInstance)
        {
            s_pInstance = new CVNMSchedule();
        }

        return s_pInstance;
    };
public:
    int32 CountMtus(vector <CVNetVnicConfig> &vecVnicCfg, vector <CScheduleInfo> &vecScheduleInfo);
    int32 CountMtus(vector <CVNetVnicMem> &vecVnicMem, vector <CScheduleInfo> &vecScheduleInfo);
  
private:  
    void ScheduleCC(const MessageFrame& message);
    void ScheduleHC(const MessageFrame& message);
    void GetNetResourceOnCC(const MessageFrame& message);
    void FreeNetResourceOnCC(const MessageFrame& message);
    void GetNetResourceOnHC(const MessageFrame& message);
    void FreeNetResourceOnHC(const MessageFrame& message);
    int32 ScheduleWdgCC(vector<string> &vecScheduledCC);
    int32 ScheduleWdgHC(vector<string> &vecScheduledHC);
    int32 GetScheduleReqInfo(vector <CVNetVnicConfig> &vecVnicCfg, 
            vector <CScheduleInfo> &vecScheduleInfo,
            map<string, int32> &mapNetplaneSum,
            map<string, int32> &mapLGNetworkSum);
    int32 CountNetplanes(vector <CVNetVnicConfig> &vecVnicCfg, map<string, int32> &mapNetplaneSum);
    int32 CountLogicNetworks(vector <CVNetVnicConfig> &vecVnicCfg, map<string, int32> &mapLGNetworkSum);
    int32 GetPgByLogicNetwork(string  m_strLogicNetworkID, CPortGroup &cPortGroup);
    int32 CheckMacRes(map<string, int32> &mapNetplaneSum);
    int32 CheckIPRes(map<string, int32> &mapLGNetworkSum);
    int32 CheckLoopDeploy(int64 nProjectID, CScheduleTaskInfo &cSchTaskInfo, vector <CScheduleInfo> &vecScheduleInfo);
    int32 ScheduleProc(CScheduleTaskInfo &cSchTaskInfo, vector <CScheduleInfo> &vecScheduleInfo, vector<string> &vecScheduledResult);
    void GetScheduledNode(int32 nFirstFlag, vector<string> &vecScheduledFinal, vector<string> &vecScheduledInput);
    
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
    static CVNMSchedule *s_pInstance;

    DISALLOW_COPY_AND_ASSIGN(CVNMSchedule);

};


void GetVectorInsertion(vector<string> &vecScheduledFinal, vector<string> &vecScheduledInput);
void MergeScheduleResult(vector<string> &vecVnaFinal, vector<CDbSchedule> &vecScheduleFinal, vector<CDbSchedule> &vecScheduleRes);
void MergeScheduleVna(vector<string> &vecVnaFinal, vector<CDbSchedule> &vecScheduleRes);
void CountVNAFreeInfo(vector<CVNAFreeSumInfo> &vecVNAFreeInfo, CDbSchedule &cDbSchedule);


}

#endif

