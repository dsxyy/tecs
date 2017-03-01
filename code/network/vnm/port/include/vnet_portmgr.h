/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：vnet_portmgr.h
* 文件标识：
* 内容摘要：CPortMgr类的定义文件
* 当前版本：1.0
* 作    者：liuhx
* 完成日期：2013年2月25日
******************************************************************************/

#if !defined(VNET_PORTMGR_INCLUDE_H__)
#define VNET_PORTMGR_INCLUDE_H__

#include "vm_messages.h"
#include "vnet_msg.h"
#include "vnet_mid.h"
#include "vnet_db_port.h"
#include "vnet_db_mgr.h"
#include "vnet_db_port_physical.h"
#include "vnet_db_port_sriovvf.h"
#include "vnet_db_port_uplink_loop.h"
#include "vnet_db_port_kernel.h"
#include "vnet_db_kernelreport.h"
#include "vnet_db_bond.h"
#include "vnet_db_port_ip.h"
#include "vnet_db_port_vtep.h"
#include "port_group_mgr.h"
#include "vnm_switch_manager.h"
#include "wildcast_mgr.h"
#include "wildcast_create_vport.h"
#include "wildcast_loopback.h"
#include "vnet_portinfo.h"

namespace zte_tecs
{

class CPortMgr : public MessageHandler
{
public:
    virtual ~CPortMgr();

public:

    //物理端口上报
    int32 DoPortInfoPrepare(CMessageVNAPortInfoReport &msg);

    int32 DoPhyPortInfoReport(const string strVnaUuid, vector<CPhyPortReport> &vPhyPort);
    //SRIOV口上报
    int32 DoSriovPortInfoReport(const string strVnaUuid, vector<CPhyPortReport> &vSriovPhyPort);
    int32 DoVFPortInfoReport(const string &strVnaUuid, vector<CPhyPortReport> &vSriovPhyPort, string  strSriovUuid,
                                   int32 nPorType, const string &strPortName, int32 nIsOnline);

    //上行还回口上报
    int32 DoUplinkLoopPortInfoReport(CMessageVNAPortInfoReport &msg);

    //trunk口上报	
    int32 DoTrunkPortInfoReport(CMessageVNAPortInfoReport &msg);	

    //安装配置的kernel口上报
    int32 DoKernelPortInfoReport(CMessageVNAPortInfoReport &msg);	

    //web配置的kernel口上报
    int32 DoKernelPortCfgInfoReport(CMessageVNAPortInfoReport &msg);

    //端口信息设置	
    int32 SetPhyPortInfo(string vna_uuid, CDbPortPhysical &info, CPhyPortReport &report);
    int32 CmpPhyPortInfo(CDbPortPhysical &info, CPhyPortReport &report);
    int32 SetPhySriovInfo(string vna_uuid, CDbPortPhysical &info, CPhyPortReport &report);
    int32 CmpPhySriovInfo(CDbPortPhysical &info, CPhyPortReport &report);
    int32 CmpPhyPortCfgInfo(CDbPortPhysical &info, CPhyPortReport &report);
    int32 CmpPhySriovCfgInfo(CDbPortPhysical &info, CPhyPortReport &report);
    int32 SetUplinkLoopPortInfo(string vna_uuid, CDbPortUplinkLoop &info, CUplinkLoopPortReport &report);	
    int32 CmpUplinkLoopPortCfgInfo(CDbPortUplinkLoop &info, CUplinkLoopPortReport &report);
    int32 CmpUplinkLoopPortInfo(CDbPortUplinkLoop &info, CUplinkLoopPortReport &report);
    int32 CmpTrunkPortCfgInfo(CDbBond &info, CBondReport &report);	
    int32 CmpTrunkPortInfo(CDbBond &info, CBondReport &report);
    int32 SetTrunkPortInfo(string vna_uuid, CDbBond &info, CBondReport &report);

    //端口ip操作
    int32 AddPortIp(const string port_uuid, const string port_ip_name, const string ip, const string mask, int32 iscfg, int32 isonline);
    int32 ModPortIp(string port_uuid, string port_ip_name, string ip, string mask, int32 iscfg, int32 isonline);
    int32 DelPortIp(string port_uuid, string port_ip_name);
	
    //查询物理端口信息
    int32 QueryPhyPortInfo(string vna_uuid, vector<CDbPortPhysical> &phyportinfo);

    //上报信息处理
    int32 ProcVnaPortInfo(CMessageVNAPortInfoReport &msg);

    //SRIOV相关的交换接口
    int32 GetSriovVF(const string &vna_uuid, string &port_name, int32 port_type, string &vf_uuid);
    int32 GetPhySriovVF(const string &port_uuid, string &vf_uuid);
    int32 GetPhyMultiSriovVF(const string &strPortUuid, uint32 nVfNum, vector<string> &vVfUuid);
    int32 GetPciByVF(const string &vf_uuid, string &vf_pci);

    //web配置的kernel口处理
    int32 AddKernelPort(string vna_uuid,int32 port_type, string name, int32 type, string switchid, string pgid, int32 isdhcp);
    int32 ModKernelPort(string vna_uuid,int32 port_type, string name, int32 type, string switchid, string pgid);
    int32 DelKernelPort(string vna_uuid, string name);

    //bond操作相关子操作
    int32 AddBondLacpMap4Report(string &vna_uuid, string &bond_name, CBondReport &report, CDBOperateBond *pOper);
    int32 AddBondBackupMap4Report(string &vna_uuid, string &bond_name, CBondReport &report, CDBOperateBond *pOper);

    int32 CompBondLacpMap4Report(string &vna_uuid, string &bond_name, vector<CDBBondMap> vBondMap, CBondReport &report, CDBOperateBond *pOper);
    int32 CompBondBackupMap4Report(string &vna_uuid, string &bond_name, vector<CDBBondMap> vBondMap, CBondReport &report, CDBOperateBond *pOper);

    int32 AddBondLacpMap4Cfg(string &vna_uuid, string &bond_name, vector<string> &report, CDBOperateBond *pOper);
    int32 AddBondBackupMap4Cfg(string &vna_uuid, string &bond_name, vector<string> &report, CDBOperateBond *pOper);
    int32 CompBondLacpMap4Cfg(string &vna_uuid, string &bond_name, vector<CDBBondMap> vBondMap, vector<string> &report, CDBOperateBond *pOper);
    int32 CompBondBackupMap4Cfg(string &vna_uuid, string &bond_name, vector<CDBBondMap> vBondMap, vector<string> &report, CDBOperateBond *pOper);
    int32 AddBond2Db(string vna_uuid, string bond_name, vector<string> ifs, int32 mode, string &bond_uuid);

    int32 DelBondMem2Db(string vna_uuid, string bond_name, vector<string> ifs);
    int32 DelBond2Db(string vna_uuid, string bond_name);
    int32 GetBondMem(const string &vna_uuid, const string &bond_name, int32 &bond_mode, vector<string> &ifs);

    //提供给交换的接口(获取uuid、type等)
    int32 GetPortUUID(const string &vna_uuid, const string &port_name, const int32 port_type, string &port_uuid);	
    int32 GetPortType(const string &vna_uuid, const string &port_name, int32 &port_type, string &port_uuid);
    int32 IsSriovPort(const string &vna_uuid, const string &port_name);
    int32 GetSriovPortVfNum(string &vna_uuid, string &port_name, int32 port_type, int32 &vf_num);
    int32 GetSriovPortVfFreeNum(const string &vna_uuid, const string &port_name, const int32 port_type, int32 &freevf_num);

    //kernel配置相关处理
    STATUS ProcKernelPortReqMsg(const MessageFrame& message);
    STATUS ProcKernelPortAckMsg(const MessageFrame& message);
    STATUS DoKernelPortAddReq(const CKerNalPortMsg &cMsg, const MID &mid);
    STATUS DoKernelPortDelReq(const CKerNalPortMsg &cMsg, const MID &mid);
    STATUS DoKernelPortModReq(const CKerNalPortMsg &cMsg, const MID &mid);

    //loopback配置相关处理
    STATUS ProcLoopBackPortReqMsg(const MessageFrame& message);
    STATUS ProcLoopBackPortAckMsg(const MessageFrame& message);
    STATUS DoLoopBackPortAddReq(const CLoopPortMsg &cMsg, const MID &mid);
    STATUS DoLoopBackPortDelReq(const CLoopPortMsg &cMsg, const MID &mid);	
    STATUS DoLoopBackPortSetLoopReq(const CLoopPortMsg &cMsg, const MID &mid);
    STATUS DoLoopBackPortUnSetLoopReq(const CLoopPortMsg &cMsg, const MID &mid);

    //上行还回相关子处理
    int32 AddUplinkLoopPort(const string &vna_uuid, const int32 port_type, const string port_name);
    int32 ModUplinkLoopPort(const string &vna_uuid, const int32 port_type, const string port_name);
    int32 DelUplinkLoopPort(const string &vna_uuid, const string port_name);
    int32 SetSriovPortLoop(const string &vna_uuid, const string port_name);	
    int32 UnSetSriovPortLoop(const string &vna_uuid, const string port_name);	

    //配置下发vna子操作
    int32 SendKernelPortCfgToVna(const string &strVNAUuid);
    int32 SendLoopBackPortCfgToVna(const string &strVNAUuid);
    int32 SetBondCfg(const string &port_uuid);
    int32 SetPortCfgToVnaTimer(const string &strVnaUuid);

    //设置短暂的离线
    int32 SetPortCfgOffline(const string vna_uuid);

    //配置下发van操作
    int32 SendPortCfgToVna(const string &strVNAUuid);
    int32 ProcPortCfgToVnaAck(const MessageFrame& message);
    int32 ProcPortCfgToVna(void);

    //通配	
    int32 ProcKernelPortWildcast(const CPortCfgMsgToVNA &msg);	
    int32 ProcLoopBackPortWildcast(const CPortCfgMsgToVNA &msg);	

    //打印开关
    void SetDbgPrint(int32 phy, int32 sriov, int32 bond, int32 krport, int32 uplinkLpport, int32 OpenDbgInf);	
    void DbgShowPortCfg(void);	
	
    int32 IsJoinBond(const string &strVnaUuid, const string &strPhyPortName, string &strBondName);
    int32 IsSriovPortLoop(const string &strVnaUuid, const string &strPhyPortName, int32 &nIsLoop);

    int32 DoVtepPortCfgInfoReport(CMessageVNAPortInfoReport &msg);
    int32 AddVtepPort(const string strVnaUuid, const string strVtepName, const string strSwitchUuid, const string strIp, const string strMask);
    int32 DelVtepPort(const string &strVnaUuid, const string strVtepName);
    int32 QueryVtepInfo(const string strVnaUuid, const string strSwitchUuid, string &strVtepName, string &strIp, string &strMask);
    int32 GetVtepInfo(const string strVnaUuid, const string strSwitchUuid, const string strPgUuid, string &strVtepName, string &strIp, string &strMask);
    int32 SetVtepCfg(const string strVnaUuid, const string strVtepName);
    int32 GetDbPortVtep(const string strVnaUuid, const string strVtepName, CDbPortVtep &DbPortVtep);
    
    static CPortMgr *GetInstance()
    {
        if (NULL == s_pInstance)
        {
            s_pInstance = new CPortMgr();
        }

        return s_pInstance;
    };

    STATUS Init(MessageUnit *mu);
    void MessageEntry(const MessageFrame& message);

public:

protected:

private:
    CPortMgr();
    STATUS StartMu(const string& name);

private:
    static CPortMgr *s_pInstance;

    //定义各种端口打印开关	
    int32 m_bOpenDbgInf;
    int32 m_nPhyPortDbgInf;
    int32 m_nSRIOVDbgInf;
    int32 m_nBondDbgInf;
    int32 m_nUplinkLpDbgInf;
    int32 m_nKrPortDbgInf;

    //下发VNA使用
    //TIMER_ID m_tPortTimerId;

private:
    MessageUnit *m_pMu;

    //vna重启port info下发
    //基于kernel和loop没有重复名称
    typedef struct tagKernelPortInfo
    {
        string strVnaUuid;
        string port_name;
        int32  nFlag;
    }T_KernelPortInfo;

    typedef struct tagLoopPortInfo
    {
        string strVnaUuid;
        string port_name;
        int32  nFlag;
    }T_LoopPortInfo;

    vector <T_KernelPortInfo> m_vKernelPortInfo;
    vector <T_LoopPortInfo> m_vLoopPortInfo;

    TIMER_ID tTimerID;        //同步port info 定时器ID	

private:

};
}// namespace zte_tecs

#endif // VNET_PORTMGR_INCLUDE_H__

