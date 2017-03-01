/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：vnm_switch.h
* 文件标识： 
* 内容摘要：vnm switch类
* 当前版本：1.0
* 作    者： 
* 完成日期： 
*******************************************************************************/
#ifndef VNM_SWITCH_H
#define VNM_SWITCH_H
#include "vnet_comm.h"
#include "vnetlib_msg.h"
#include "vnet_msg.h"
#include "vnet_db_vswitch.h"
#include "vnet_db_vswitch_map.h"
#include "vnet_db_schedule.h"
#include "vnm_schedule.h"

namespace zte_tecs
{

#define MAX_SWITCH_NAME_LEN      15       /* 交换名字最大长度，OVS最多支持15个字节 */
#define MAX_SWITCH_VNIC_NUMS     1024    /* 支持最大虚拟网卡个数 */
#define DEFAULT_KERNEL_SDVS      "sdvs_00"

typedef enum tagSwitchMapCfgFlag
{
    EN_SWITCH_MAP_NO_CFG = 0,
    EN_SWITCH_MAP_IS_CFG = 1,
    EN_SWITCH_MAP_UPDATE_CFG = 2,    
}EN_SWITCHMAP_CFG_FALG;

class CSwitch  
{
//交换基类
public:
    CSwitch();        
    virtual ~CSwitch();
    virtual int32  GetDVSResource(CVNetVmMem &cVnetVmMem){return 0;};
    virtual int32  GetDVSInfo(const string &strVNA, CVNetVnicDetail &cVNicDetail){return 0;};   
    virtual int32  GetSwitchDetail(const string & strSwUuid, const string &strVNAUuid, vector<CSwitchCfgMsgToVNA> &vecVNAMsg, string &strRetInfo){return 0;};
    virtual int32  GetNeedCfgSwitch(const string &strSwUuid, const string &strVNAUuid, vector<CSwitchCfgMsgToVNA> &vecVNAMsg){return 0;}; 
    virtual int32  CanSwitchAdd(CSwitchCfgMsgToVNA &cMsg){return 0;};
    virtual int32  CanSwitchDel(const string &strSwUuid, vector<CSwitchCfgMsgToVNA> &vecVNAMsg, string &strRetInfo){return 0;};
    virtual int32  CanSwitchPortAdd(CSwitchCfgMsgToVNA &cMsg){return 0;};
    virtual int32  CanSwitchPortDel(CSwitchCfgMsgToVNA &cMsg){return 0;};
    virtual void   AddReportSwitch(const string &strVNAUuid, CSwitchReportInfo& cReport){};
    virtual int32  AddSwitch(CSwitchCfgMsg& cMsg, string &strRetInfo){return 0;};
    virtual int32  DelSwitch(const string &strUuid, string &strRetInfo){return 0;};
    virtual int32  DelSwitchPortMap(CSwitchCfgMsgToVNA& cMsg){return 0;};
    virtual int32  QuerySwitch(CSwitchCfgMsgToVNA& cMsg){return 0;};
    virtual int32  ModifySwitch(CSwitchCfgMsg& cMsg, string &strRetInfo){return 0;};
    virtual int32  ModifySwitch(CSwitchCfgMsgToVNA &cMsg){return 0;};
    virtual int32  AddSwitchPort(CSwitchCfgMsgToVNA &cMsg){return 0;};
    virtual int32  DelSwitchPort( CSwitchCfgMsgToVNA &cMsg){return 0;};   
    virtual int32  ModifySwitchPort(const string &strVNAUuid){return 0;};   
    virtual int32  ModifySwitchPort(){return 0;};
    virtual int32  QuerySwitchPort( CSwitchCfgMsgToVNA &cMsg){return 0;};
    virtual int32  QuerySdnCfg( CSwitchCfgMsgToVNA &cMsg){return 0;};
    virtual int32  AddKernelDVS(const string &strVNAUuid, const string &strDVSName, const vector<string> &vecPortName,
                                         const string &strBondName, const int32 &nBondMode){return 0;};
    virtual int32  GetDVSUuid(const string &strSwName, string &strSwUuid){return 0;};
    virtual int32  GetDVSUplinkPort(const string &strVNAUuid, const string &strSwName, string &strPortName){return 0;};
    virtual int32  GetDVSName(const string &strSwUuid, string &strSwName){return 0;};
    virtual int32  GetDVSVNA(const string &strDVSUuid, vector<string> &vecVNAUuid){ return 0;};
    virtual int32  GetDVSPGUuid(const string &strDVSUuid, string &strPGUuid){ return 0;};
    virtual int32  IsSwitchPortValid(CSwitchCfgMsgToVNA &cMsg){ return 0;};
    virtual void   DbgShow(){};
    
private:
    DISALLOW_COPY_AND_ASSIGN(CSwitch);
};

class CVirtualSwitch : public CSwitch  
{
//虚拟交换派生类
public:
    CVirtualSwitch(){};
    virtual ~CVirtualSwitch(){};
    
public:
    int32  GetDVSResource(CVNetVmMem &cVnetVmMem);
    int32  GetDVSInfo(const string &strVNA, CVNetVnicDetail &cVNicDetail);
    int32  GetSwitchDetail(const string & strSwUuid, const string &strVNAUuid, vector<CSwitchCfgMsgToVNA> &vecVNAMsg, string &strRetInfo);
    int32  GetNeedCfgSwitch(const string &strSwUuid, const string &strVNAUuid, vector<CSwitchCfgMsgToVNA> &vecVNAMsg);
    int32  CanSwitchAdd(CSwitchCfgMsgToVNA &cMsg);
    int32  CanSwitchDel(const string &strSwUuid, vector<CSwitchCfgMsgToVNA> &vecVNAMsg, string &strRetInfo);
    int32  CanSwitchPortAdd(CSwitchCfgMsgToVNA &cMsg);
    int32  CanSwitchPortDel(CSwitchCfgMsgToVNA &cMsg);
    void   AddReportSwitch(const string &strVNAUuid, CSwitchReportInfo& cReport);
    int32  AddSwitch(CSwitchCfgMsg& cMsg, string &strRetInfo);
    int32  DelSwitch(const string &strUuid, string &strRetInfo);
    int32  DelSwitchPortMap(CSwitchCfgMsgToVNA& cMsg);
    int32  ModifySwitch(CSwitchCfgMsg& cMsg, string &strRetInfo);
    int32  ModifySwitch(CSwitchCfgMsgToVNA &cMsg);
    int32  QuerySwitch(CSwitchCfgMsgToVNA& cMsg);
    int32  AddSwitchPort(CSwitchCfgMsgToVNA &cMsg);
    int32  DelSwitchPort( CSwitchCfgMsgToVNA &cMsg);
    int32  ModifySwitchPort(const string &strVNAUuid);
    int32  ModifySwitchPort();
    int32  QuerySwitchPort( CSwitchCfgMsgToVNA &cMsg);
    int32  QuerySdnCfg( CSwitchCfgMsgToVNA &cMsg);
    int32  AddKernelDVS(const string &strVNAUuid, const string &strDVSName, 
                const vector<string> &vecPortName, const string &strBondName, const int32 &nBondMode);
    int32  GetDVSUuid(const string &strSwName, string &strSwUuid);
    int32  GetDVSName(const string &strSwUuid, string &strSwName);
    int32  GetDVSVNA(const string &strDVSUuid, vector<string> &vecVNAUuid);
    int32  GetDVSPGUuid(const string &strDVSUuid, string &strPGUuid);
    int32  GetDVSUplinkPort(const string &strVNAUuid, const string &strSwName, string &strPortName);
    int32  IsSwitchPortValid(CSwitchCfgMsgToVNA &cMsg);
    void   DbgShow();
    
private:
    int32  GetDVSBondMember(const string &strVNA, const string &strBond, int32 &nMode, vector<string> &vecSlaves);
    int32  GetAllocReqInfo(vector<CVNetVnicMem> &vecVnicCfg, vector<CScheduleInfo> &vecScheduleInfo);
    int32  GetSriovVFRes(map< string, set<int32> > &mapVnic, vector<CVNetVnicMem> &vecVnic);
    int32  IsVectorEqua(vector<string> new_vec, vector<string> old_vec);
    int32  IsVectorMixed(vector<string> new_vec, vector<string> old_vec);
    int32  IsMixedPortValid(CSwitchCfgMsgToVNA &cMsg);
    int32  IsSwitchLoop(const string &strSwUuid, int32 &nIsLoop);
    int32  IsSwitchUsed(const string &strSwUuid, string &strRetInfo);
    int32  IsSwitchPortUsed(CSwitchCfgMsgToVNA &cMsg);
    int32  CheckDVSParam(CSwitchCfgMsg &cMsg, string &strRetInfo);
    CDBOperateVSwitch    * GetDBVSwitch();
    CDBOperateVSwitchMap * GetDBVSwitchMap();
   
private:
    DISALLOW_COPY_AND_ASSIGN(CVirtualSwitch);
};

class CPhySwitch : public CSwitch  //物理交换派生类
{

public:
    CPhySwitch(){};
    virtual ~CPhySwitch(){};

private:
    DISALLOW_COPY_AND_ASSIGN(CPhySwitch);
};

}
#endif

