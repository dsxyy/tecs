/******************************************************************************
* Copyright (c) 2013，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：vnet_db_mgr.h
* 文件标识：
* 内容摘要：CVNetDbMgr类的定义文件
* 当前版本：1.0
* 作    者：
* 完成日期：2013年1月28日
*
* 修改记录1：
*     修改日期：2013/1/28
*     版 本 号：V1.0
*     修 改 人：gong.xiefeng
*     修改内容：创建
*
******************************************************************************/

#if !defined(_VNET_DB_MGR_INCLUDE_H_)
#define _VNET_DB_MGR_INCLUDE_H_

#include "vnet_db_in.h"

namespace zte_tecs
{
class CDBOperateNetplane;
class CDBOperateVna;
class CDBOperateVnaRegModule;
class CDBOperatePort;
class CDBOperatePortPhysical;
class CDBOperatePortSriovVf;
class CDBOperatePortUplinkLoop;
class CDBOperatePortKernel;
class CDBOperatePortGroup;
class CDBOperatePgTrunkVlanRange;
class CDBOperateVSwitch;
class CDBOperateVSwitchMap;
class CDBOperateLogicNetwork;
class CDBOperateLogicNetworkIpRange;
class CDBOperateLogicNetworkIpPool;
class CDBOperateNetplaneIpRange;
class CDBOperateNetplaneIpPool;
class CDBOperateNetplaneMacRange;
class CDBOperateNetplaneMacPool;
class CDBOperateNetplaneVlanRange;
class CDBOperateNetplaneVlanPool;
class CDBOperateNetplaneSegmentRange;
class CDBOperateNetplaneSegmentPool;
class CDBOperateVlanMap;
class CDBOperateSegmentMap;
class CDBOperateBond;
class CDBOperateVsi;
class CDBOperateSchedule;
class CDBOperatePortMac;
class CDBOperatePortIp;
class CDBOperatePortVSwitch;
class CDBOperateKernelReport;
class CDBOperateWildcastLoopbackPort;
class CDBOperateWildcastCreateVPort;
class CDBOperateWildcastSwitch;
class CDBOperateWildcastTaskSwitch;
class CDBOperateWildcastTaskCreateVPort;
class CDBOperateWildcastTaskLoopback;
class CDBOperateVmMigrate;
class CDBOperateVm;
class CDBOperateNetplaneSegmentPool;

class CDBOperateSdnCfg;
class CDBOperatePortVtep;
class CDBOperateQuantumNetwork;
class CDBOperateQuantumPort;

class CVNetDbMgr
{
public:
    explicit CVNetDbMgr(/*,SqlDB * pDB*/);        
    virtual ~CVNetDbMgr();
    static CVNetDbMgr *GetInstance(/*SqlDB * pDB = NULL*/)
    {        
        if(NULL == s_instance)
        {
            s_instance = new CVNetDbMgr();                    
        }        
        return s_instance;
    };
    
    int32 Init();

    CDBOperateNetplane*  GetINetplane();
    CDBOperateVna*  GetIVna();
    CDBOperateVnaRegModule*  GetIVnaRegModule();
    CDBOperatePort*  GetIPort();
    CDBOperatePortPhysical*  GetIPortPhy();    
    CDBOperatePortSriovVf*  GetIPortSriovVf();     
    CDBOperatePortUplinkLoop*  GetIPortUplinkLoop();
    CDBOperatePortKernel*  GetIPortKernel();
    CDBOperatePortGroup *  GetIPortGroup();
    CDBOperatePgTrunkVlanRange *  GetIPgTrunkVlanRange();
    CDBOperateVSwitch *  GetIVSwitch();
    CDBOperateVSwitchMap *  GetIVSwitchMap();    
    CDBOperateLogicNetwork * GetILogicNetwork();
    CDBOperateLogicNetworkIpRange * GetILogicNetworkIpRange();
    CDBOperateLogicNetworkIpPool * GetILogicNetworkIpPool();
    CDBOperateNetplaneIpRange * GetINetplaneIpRange();
    CDBOperateNetplaneIpPool * GetINetplaneIpPool();    
    CDBOperateNetplaneMacRange * GetINetplaneMacRange();
    CDBOperateNetplaneMacPool * GetINetplaneMacPool();
    CDBOperateNetplaneVlanRange * GetINetplaneVlanRange();
    CDBOperateNetplaneVlanPool * GetINetplaneVlanPool();
    CDBOperateNetplaneSegmentRange * GetINetplaneSegmentRange();
    CDBOperateNetplaneSegmentPool * GetINetplaneSegmentPool();    
    CDBOperateVlanMap * GetIVlanMap();
    CDBOperateSegmentMap * GetISegmentMap();    
    CDBOperateBond * GetIBond();    
    CDBOperateVsi * GetIVsi();   
    CDBOperateSchedule * GetISchedule();  
    CDBOperatePortMac * GetIPortMac();  
    CDBOperatePortIp * GetIPortIp();  
    CDBOperatePortVSwitch * GetIPortVSwitch();
    CDBOperateKernelReport * GetIKernelReport();
    CDBOperateWildcastLoopbackPort * GetIWildcastLoopbackPort();
    CDBOperateWildcastCreateVPort * GetIWildcastCreateVPort();
    CDBOperateWildcastSwitch * GetIWildcastSwitch();
    CDBOperateWildcastTaskSwitch * GetIWildcastTaskSwitch();
    CDBOperateWildcastTaskCreateVPort * GetIWildcastTaskCreateVPort();
    CDBOperateWildcastTaskLoopback * GetIWildcastTaskLoopback();
    CDBOperateVmMigrate * GetIVmMigrate();
    CDBOperateVm * GetIVm();

    CDBOperateSdnCfg* GetISdnCfg();
    CDBOperatePortVtep* GetIPortVtep();
    CDBOperateQuantumNetwork * GetIQuantumNetwork();
    CDBOperateQuantumPort * GetIQuantumPort();
    int32 DbgShow();
private: 
    DISALLOW_COPY_AND_ASSIGN(CVNetDbMgr);
    
    static CVNetDbMgr *s_instance;  
    bool m_bInit; 

    CDBOperateNetplane* m_pNetplane;
    CDBOperateVna* m_pVna;
    CDBOperateVnaRegModule* m_pVnaRegModule;
    CDBOperatePort* m_pPort;
    CDBOperatePortPhysical* m_pPortPhy;
    CDBOperatePortSriovVf* m_pPortSriovVf;
    CDBOperatePortUplinkLoop * m_pPortUplinkLoop;
    CDBOperatePortKernel*  m_pPortKernel;
    CDBOperatePortGroup*   m_pPortGroup;
    CDBOperatePgTrunkVlanRange * m_pPgTrunkVlanRange;
    CDBOperateVSwitch * m_pVSwitch;
    CDBOperateVSwitchMap * m_pVSwitchMap;  
    CDBOperateLogicNetwork * m_pLogicNetwork;  
    CDBOperateLogicNetworkIpRange * m_pLogicNetworkIpRange;     
    CDBOperateLogicNetworkIpPool * m_pLogicNetworkIpPool; 
    CDBOperateNetplaneIpRange * m_pNetplaneIpRange;
    CDBOperateNetplaneIpPool  * m_pNetplaneIpPool;
    CDBOperateNetplaneMacRange * m_pNetplaneMacRange;
    CDBOperateNetplaneMacPool * m_pNetplaneMacPool;  
    CDBOperateNetplaneVlanRange * m_pNetplaneVlanRange;
    CDBOperateNetplaneVlanPool * m_pNetplaneVlanPool; 
    CDBOperateNetplaneSegmentRange * m_pNetplaneSegmentRange;
    CDBOperateNetplaneSegmentPool * m_pNetplaneSegmentPool; 
    CDBOperateVlanMap * m_pVlanMap;
    CDBOperateSegmentMap * m_pSegmentMap;
    CDBOperateBond * m_pBond;
    CDBOperateVsi * m_pVsi;
    CDBOperateSchedule * m_pSchedule;    
    CDBOperatePortMac * m_pPortMac;    
    CDBOperatePortIp * m_pPortIp;   
    CDBOperatePortVSwitch * m_pPortVSwitch;
    CDBOperateKernelReport * m_pKernelReport;
    CDBOperateWildcastLoopbackPort * m_pWildcastLoopBackPort;
    CDBOperateWildcastCreateVPort * m_pWildcastCreateVPort;
    CDBOperateWildcastSwitch * m_pWildcastSwitch;   
    CDBOperateWildcastTaskSwitch * m_pWildcastTaskSwitch;
    CDBOperateWildcastTaskCreateVPort * m_pWildcastTaskCreateVPort;
    CDBOperateWildcastTaskLoopback * m_pWildcastTaskLoopback;
    CDBOperateVmMigrate  * m_pVmMigrate;
    CDBOperateVm  *m_pVm;

    CDBOperateSdnCfg * m_pSdnCfg;
    CDBOperatePortVtep * m_pPortVtep;
    CDBOperateQuantumNetwork * m_pQuantumNetwork;
    CDBOperateQuantumPort * m_pQuantumPort;
};

/* 简化DB接口 */
CDBOperateNetplane* GetDbINetplane();
CDBOperateVna* GetDbIVna();
CDBOperateVnaRegModule* GetDbIVnaRegModule();
CDBOperatePort* GetDbIPort();
CDBOperatePortPhysical* GetDbIPortPhy();
CDBOperatePortSriovVf* GetDbIPortSriovVf();
CDBOperatePortUplinkLoop* GetDbIPortUplinkLoop();
CDBOperatePortKernel* GetDbIPortKernel();
CDBOperatePortGroup* GetDbIPortGroup();
CDBOperatePgTrunkVlanRange* GetDbIPgTrunkVlanRange();
CDBOperateVSwitch * GetDbIVSwitch();
CDBOperateVSwitchMap * GetDbIVSwitchMap();
CDBOperateLogicNetwork * GetDbILogicNetwork();
CDBOperateLogicNetworkIpRange * GetDbILogicNetworkIpRange();
CDBOperateLogicNetworkIpPool * GetDbILogicNetworkIpPool();
CDBOperateNetplaneIpRange * GetDbINetplaneIpRange();
CDBOperateNetplaneIpPool * GetDbINetplaneIpPool();
CDBOperateNetplaneMacRange * GetDbINetplaneMacRange();
CDBOperateNetplaneMacPool * GetDbINetplaneMacPool();
CDBOperateNetplaneVlanRange * GetDbINetplaneVlanRange();
CDBOperateNetplaneVlanPool * GetDbINetplaneVlanPool();
CDBOperateNetplaneSegmentRange * GetDbINetplaneSegmentRange();
CDBOperateNetplaneSegmentPool * GetDbINetplaneSegmentPool();
CDBOperateVlanMap * GetDbIVlanMap();
CDBOperateSegmentMap * GetDbISegmentMap();
CDBOperateBond * GetDbIBond();
CDBOperateVsi * GetDbIVsi();
CDBOperateSchedule * GetDbISchedule();
CDBOperatePortMac * GetDbIPortMac();
CDBOperatePortIp * GetDbIPortIp();
CDBOperatePortVSwitch *GetDbIPortVSwitch();
CDBOperateKernelReport *GetDbIKernelReport();
CDBOperateWildcastLoopbackPort * GetDbIWildcastLoopbackPort();
CDBOperateWildcastCreateVPort * GetDbIWildcastCreateVPort();
CDBOperateWildcastSwitch * GetDbIWildcastSwitch();
CDBOperateWildcastTaskSwitch *  GetDbIWildcastTaskSwitch();
CDBOperateWildcastTaskCreateVPort * GetDbIWildcastTaskCreateVPort();
CDBOperateWildcastTaskLoopback * GetDbIWildcastTaskLoopback();
CDBOperateVmMigrate * GetDbIVmMigrate();
CDBOperateVm * GetDbIVm();

CDBOperateNetplaneSegmentPool *GetDbINetpalneSegmentPool();

CDBOperateSdnCfg * GetDbISdnCfg();
CDBOperatePortVtep * GetDbIPortVtep();
CDBOperateQuantumNetwork* GetDbIQuantumNetwork();
CDBOperateQuantumPort* GetDbIQuantumPort();

// 调试DBG
void DbgSetDBPrint(int32 i);
int32 DbgGetDBPrint();

}// namespace zte_tecs

#endif // _VNET_DB_MGR_INCLUDE_H_

