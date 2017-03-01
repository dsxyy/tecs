

#include "vnet_db_proc.h"
#include "vnet_db_mgr.h"
#include "vnet_db_netplane.h"
#include "vnet_db_vna.h"
#include "vnet_db_reg_module.h"
#include "vnet_db_port.h"
#include "vnet_db_port_physical.h"
#include "vnet_db_port_sriovvf.h"
#include "vnet_db_port_uplink_loop.h"
#include "vnet_db_port_kernel.h"
#include "vnet_db_portgroup.h"
#include "vnet_db_pg_trunkvlanrange.h"
#include "vnet_db_vswitch.h"
#include "vnet_db_vswitch_map.h"
#include "vnet_db_logicnetwork_iprange.h"
#include "vnet_db_logicnetwork.h"
#include "vnet_db_logicnetwork_ippool.h"
#include "vnet_db_netplane_iprange.h"
#include "vnet_db_netplane_macrange.h"
#include "vnet_db_netplane_macpool.h"
#include "vnet_db_netplane_vlanrange.h"
#include "vnet_db_netplane_vlanpool.h"
#include "vnet_db_vlan_map.h"
#include "vnet_db_bond.h"
#include "vnet_db_vsi.h"
#include "vnet_db_schedule.h"
#include "vnet_db_port_mac.h"
#include "vnet_db_port_ip.h"
#include "vnet_db_port_vswitch.h"
#include "vnet_db_kernelreport.h"
#include "vnet_db_kernelreport.h"
#include "vnet_db_wildcast_create_vport.h"
#include "vnet_db_wildcast_loopback_port.h"
#include "vnet_db_wildcast_switch.h"
#include "vnet_db_wildcast_task_switch.h"
#include "vnet_db_wildcast_task_create_vport.h"
#include "vnet_db_wildcast_task_loopback.h"
#include "vnet_db_vm_migrate.h"
#include "vnet_db_vm.h"

#include "vnet_db_netplane_segmentrange.h"
#include "vnet_db_netplane_segmentpool.h"
#include "vnet_db_quantum_port.h"
#include "vnet_db_quantum_network.h"
#include "vnet_db_port_vtep.h"
#include "vnet_db_segment_map.h"
#include "vnet_db_netplane_ippool.h"

#include "vnet_db_quantum_cfg.h"

#include "vnet_db_port_vtep.h"
#include "vnet_db_quantum_network.h"
#include "vnet_db_quantum_port.h"

namespace zte_tecs
{

CADODatabase *g_pDb = NULL;
CVNetDbMgr* CVNetDbMgr::s_instance = NULL;

//调试命令开关 
int32 g_DbgDbFlag = 0;
void DbgSetDBPrint(int32 i)
{
    g_DbgDbFlag =i;
    
    cout << "DbgDBPrint : " << g_DbgDbFlag << endl;
}
DEFINE_DEBUG_FUNC(DbgSetDBPrint);

int32 DbgGetDBPrint()
{
    return g_DbgDbFlag;
}
DEFINE_DEBUG_FUNC(DbgGetDBPrint);

CVNetDbMgr::CVNetDbMgr()
{
    m_bInit = FALSE;
    m_pNetplane = NULL;
    m_pVna = NULL;
    m_pVnaRegModule = NULL;
    m_pPort = NULL;
    m_pPortPhy = NULL;
    m_pPortSriovVf = NULL;      
    m_pPortUplinkLoop = NULL;
    m_pPortKernel = NULL;
    m_pPortGroup = NULL;
    m_pPgTrunkVlanRange = NULL;
    m_pVSwitch = NULL;
    m_pVSwitchMap = NULL;
    m_pLogicNetwork = NULL;
    m_pLogicNetworkIpRange = NULL;
    m_pLogicNetworkIpPool= NULL;
    m_pNetplaneIpRange = NULL;
    m_pNetplaneIpPool = NULL;    
    m_pNetplaneMacRange = NULL;   
    m_pNetplaneMacPool = NULL;
    m_pNetplaneVlanRange = NULL;   
    m_pNetplaneVlanPool = NULL;
    m_pNetplaneSegmentRange = NULL;   
    m_pNetplaneSegmentPool = NULL;    
    m_pVlanMap = NULL;
    m_pSegmentMap = NULL;    
    m_pBond = NULL;
    m_pVsi = NULL;
    m_pSchedule = NULL;
    m_pPortMac = NULL;
    m_pPortIp = NULL;
    m_pPortVSwitch = NULL;
    m_pKernelReport = NULL;
    m_pWildcastLoopBackPort = NULL;
    m_pWildcastCreateVPort = NULL;
    m_pWildcastSwitch = NULL;
    m_pWildcastTaskSwitch = NULL;
    m_pWildcastTaskCreateVPort = NULL;
    m_pWildcastTaskLoopback = NULL;
    m_pVmMigrate = NULL;
    m_pVm = NULL;
    m_pSdnCfg = NULL;
    m_pPortVtep = NULL;
    m_pQuantumNetwork = NULL;
    m_pQuantumPort = NULL;
}

CVNetDbMgr::~CVNetDbMgr()
{
    if( m_pNetplane ) {delete m_pNetplane;}
    if( m_pVna ) {delete m_pVna;}
    if( m_pVnaRegModule ) {delete m_pVnaRegModule;}
    if( m_pPort ) {delete m_pPort;}    
    if( m_pPortPhy ) {delete m_pPortPhy;}      
    if( m_pPortSriovVf ) {delete m_pPortSriovVf;}     
    if( m_pPortUplinkLoop ) {delete m_pPortUplinkLoop;}      
    if( m_pPortKernel ) {delete m_pPortKernel;}     
    if( m_pPortGroup ) {delete m_pPortGroup;}      
    if( m_pPgTrunkVlanRange ) {delete m_pPgTrunkVlanRange;}    
    if( m_pVSwitch ) {delete m_pVSwitch;}    
    if( m_pVSwitchMap ) {delete m_pVSwitchMap;}    
    if( m_pLogicNetwork ) {delete m_pLogicNetwork;} 
    if( m_pLogicNetworkIpRange ) {delete m_pLogicNetworkIpRange;} 
    if( m_pLogicNetworkIpPool ) {delete m_pLogicNetworkIpPool;} 
    if( m_pNetplaneIpRange ) {delete m_pNetplaneIpRange;} 
    if( m_pNetplaneIpPool ) {delete m_pNetplaneIpPool;}     
    if( m_pNetplaneMacRange ) {delete m_pNetplaneMacRange;} 
    if( m_pNetplaneMacPool ) {delete m_pNetplaneMacPool;} 
    if( m_pNetplaneVlanRange ) {delete m_pNetplaneVlanRange;} 
    if( m_pNetplaneVlanPool ) {delete m_pNetplaneVlanPool;} 
    if( m_pNetplaneSegmentRange ) {delete m_pNetplaneSegmentRange;} 
    if( m_pNetplaneSegmentPool ) {delete m_pNetplaneSegmentPool;}     
    if( m_pVlanMap ) {delete m_pVlanMap;} 
    if( m_pSegmentMap ) {delete m_pSegmentMap;}     
    if( m_pBond ) {delete m_pBond;}  
    if( m_pVsi ) {delete m_pVsi;}  
    if( m_pSchedule ) {delete m_pSchedule;}  
    if( m_pPortMac ) {delete m_pPortMac;} 
    if( m_pPortIp ) {delete m_pPortIp;} 
    if( m_pPortVSwitch ) {delete m_pPortVSwitch;} 
    if( m_pKernelReport ) {delete m_pKernelReport;} 
    if( m_pWildcastLoopBackPort ) {delete m_pWildcastLoopBackPort;} 
    if( m_pWildcastCreateVPort ) {delete m_pWildcastCreateVPort;} 
    if( m_pWildcastSwitch ) {delete m_pWildcastSwitch;} 
    if( m_pWildcastTaskSwitch ) {delete m_pWildcastTaskSwitch;}     
    if( m_pWildcastTaskCreateVPort) {delete m_pWildcastTaskCreateVPort;}
    if( m_pWildcastTaskLoopback) {delete m_pWildcastTaskLoopback;}
    if( m_pVmMigrate) {delete m_pVmMigrate;}
    if( m_pVm) {delete m_pVm;}
    if( m_pSdnCfg) {delete m_pSdnCfg;}
    if( m_pPortVtep) {delete m_pPortVtep;}
    if( m_pQuantumNetwork) {delete m_pQuantumNetwork;}    
    if( m_pQuantumPort) {delete m_pQuantumPort;}     
}

int32 CVNetDbMgr::Init()
{
    int32 ret = 0;

    // 避免重复Init
    if( m_bInit )
    {
        VNET_LOG(VNET_LOG_INFO, "CVNetDbMgr::Init again.\n");
        return 0;
    }
    
    // procedure interface 
    CVNetDbProcedureFactory * pIProc = CVNetDbProcedureFactory::GetInstance();
    if( NULL == pIProc)
    {
        VNET_LOG(VNET_LOG_ERROR, "CVNetDbMgr::Init CVNetDbProcedureFactory == NULL.\n");
        return VNET_DB_ERROR_NO_INIT;
    }
    if( 0 !=  pIProc->Init() )
    {
        VNET_LOG(VNET_LOG_ERROR,"CVNetDbMgr::Init IProc init failed.\n");
        return VNET_DB_ERROR_NO_INIT;
    }
#if 0    
    // ado database
    if( NULL == g_pDb )
    {
        g_pDb = new CADODatabase("",0,"postgresql","password","vnm_new");
        //g_pDb = new CADODatabase("",0,"postgresql","password","test");    
        if( NULL == g_pDb)
        {
            VNET_LOG(VNET_LOG_ERROR,"CVNetDbMgr::Init g_pDb == NULL.\n");
            return VNET_DB_ERROR_NO_INIT;            
        }
        if ( !g_pDb->Open() )    
        {          
            VNET_LOG(VNET_LOG_ERROR, "CVNetDbMgr::Init open database failed.\n");
            ret = VNET_DB_ERROR_OPEN_DB_FAILED;   
            delete g_pDb;
            g_pDb= NULL;
            return VNET_DB_ERROR_NO_INIT; 
        }  
    }
#endif 
    // netplane db operate interface
    m_pNetplane  = new CDBOperateNetplane(pIProc->GetIProcedure());
    if( NULL == m_pNetplane )
    {
        VNET_LOG(VNET_LOG_ERROR,"CVNetDbMgr::Init alloc memory CDBOperateNetplane failed.\n");        
        ret= VNET_DB_ERROR_ALLOC_MEM_FAILED;
        goto free_module;
    }

    m_pVna  = new CDBOperateVna(pIProc->GetIProcedure());
    if( NULL == m_pVna )
    {
        VNET_LOG(VNET_LOG_ERROR,"CVNetDbMgr::Init alloc memory CDBOperateVna failed.\n");        
        ret= VNET_DB_ERROR_ALLOC_MEM_FAILED;
        goto free_module;
    }

    m_pVnaRegModule  = new CDBOperateVnaRegModule(pIProc->GetIProcedure());
    if( NULL == m_pVnaRegModule )
    {
        VNET_LOG(VNET_LOG_ERROR,"CVNetDbMgr::Init alloc memory CDBOperateVnaRegModule failed.\n");        
        ret= VNET_DB_ERROR_ALLOC_MEM_FAILED;
        goto free_module;
    }

    m_pPort  = new CDBOperatePort(pIProc->GetIProcedure());
    if( NULL == m_pPort )
    {
        VNET_LOG(VNET_LOG_ERROR,"CVNetDbMgr::Init alloc memory CDBOperatePort failed.\n");        
        ret= VNET_DB_ERROR_ALLOC_MEM_FAILED;
        goto free_module;
    }

    m_pPortPhy  = new CDBOperatePortPhysical(pIProc->GetIProcedure());
    if( NULL == m_pPortPhy )
    {
        VNET_LOG(VNET_LOG_ERROR,"CVNetDbMgr::Init alloc memory CDBOperatePortPhysical failed.\n");        
        ret= VNET_DB_ERROR_ALLOC_MEM_FAILED;
        goto free_module;
    }

    m_pPortSriovVf  = new CDBOperatePortSriovVf(pIProc->GetIProcedure());
    if( NULL == m_pPortSriovVf )
    {
        VNET_LOG(VNET_LOG_ERROR,"CVNetDbMgr::Init alloc memory CDBOperatePortSriovVf failed.\n");        
        ret= VNET_DB_ERROR_ALLOC_MEM_FAILED;
        goto free_module;
    }

    m_pPortUplinkLoop  = new CDBOperatePortUplinkLoop(pIProc->GetIProcedure());
    if( NULL == m_pPortUplinkLoop )
    {
        VNET_LOG(VNET_LOG_ERROR,"CVNetDbMgr::Init alloc memory CDBOperatePortUplinkLoop failed.\n");        
        ret= VNET_DB_ERROR_ALLOC_MEM_FAILED;
        goto free_module;
    }

    m_pPortKernel  = new CDBOperatePortKernel(pIProc->GetIProcedure());
    if( NULL == m_pPortKernel )
    {
        VNET_LOG(VNET_LOG_ERROR,"CVNetDbMgr::Init alloc memory CDBOperatePortKernel failed.\n");        
        ret= VNET_DB_ERROR_ALLOC_MEM_FAILED;
        goto free_module;
    }

    m_pPortGroup  = new CDBOperatePortGroup(pIProc->GetIProcedure());
    if( NULL == m_pPortGroup )
    {
        VNET_LOG(VNET_LOG_ERROR,"CVNetDbMgr::Init alloc memory CDBOperatePortGroup failed.\n");        
        ret= VNET_DB_ERROR_ALLOC_MEM_FAILED;
        goto free_module;
    }

    m_pPgTrunkVlanRange  = new CDBOperatePgTrunkVlanRange(pIProc->GetIProcedure());
    if( NULL == m_pPgTrunkVlanRange )
    {
        VNET_LOG(VNET_LOG_ERROR,"CVNetDbMgr::Init alloc memory CDBOperatePgTrunkVlanRange failed.\n");        
        ret= VNET_DB_ERROR_ALLOC_MEM_FAILED;
        goto free_module;
    }

    m_pVSwitch  = new CDBOperateVSwitch(pIProc->GetIProcedure());
    if( NULL == m_pVSwitch )
    {
        VNET_LOG(VNET_LOG_ERROR,"CVNetDbMgr::Init alloc memory CDBOperateVSwitch failed.\n");        
        ret= VNET_DB_ERROR_ALLOC_MEM_FAILED;
        goto free_module;
    }

    m_pVSwitchMap  = new CDBOperateVSwitchMap(pIProc->GetIProcedure());
    if( NULL == m_pVSwitchMap )
    {
        VNET_LOG(VNET_LOG_ERROR,"CVNetDbMgr::Init alloc memory CDBOperateVSwitchMap failed.\n");        
        ret= VNET_DB_ERROR_ALLOC_MEM_FAILED;
        goto free_module;
    }

    m_pLogicNetwork  = new CDBOperateLogicNetwork(pIProc->GetIProcedure());
    if( NULL == m_pLogicNetwork )
    {
        VNET_LOG(VNET_LOG_ERROR,"CVNetDbMgr::Init alloc memory CDBOperateLogicNetwork failed.\n");        
        ret= VNET_DB_ERROR_ALLOC_MEM_FAILED;
        goto free_module;
    }

    m_pLogicNetworkIpRange  = new CDBOperateLogicNetworkIpRange(pIProc->GetIProcedure());
    if( NULL == m_pLogicNetworkIpRange )
    {
        VNET_LOG(VNET_LOG_ERROR,"CVNetDbMgr::Init alloc memory CDBOperateLogicNetworkIpRange failed.\n");        
        ret= VNET_DB_ERROR_ALLOC_MEM_FAILED;
        goto free_module;
    }

    m_pLogicNetworkIpPool  = new CDBOperateLogicNetworkIpPool(pIProc->GetIProcedure());
    if( NULL == m_pLogicNetworkIpPool )
    {
        VNET_LOG(VNET_LOG_ERROR,"CVNetDbMgr::Init alloc memory CDBOperateLogicNetworkIpPool failed.\n");        
        ret= VNET_DB_ERROR_ALLOC_MEM_FAILED;
        goto free_module;
    }    

    m_pNetplaneIpRange  = new CDBOperateNetplaneIpRange(pIProc->GetIProcedure());
    if( NULL == m_pNetplaneIpRange )
    {
        VNET_LOG(VNET_LOG_ERROR,"CVNetDbMgr::Init alloc memory CDBOperateNetplaneIpRange failed.\n");        
        ret= VNET_DB_ERROR_ALLOC_MEM_FAILED;
        goto free_module;
    } 

    m_pNetplaneIpPool  = new CDBOperateNetplaneIpPool(pIProc->GetIProcedure());
    if( NULL == m_pNetplaneIpPool )
    {
        VNET_LOG(VNET_LOG_ERROR,"CVNetDbMgr::Init alloc memory CDBOperateNetplaneIpPool failed.\n");        
        ret= VNET_DB_ERROR_ALLOC_MEM_FAILED;
        goto free_module;
    }       

    m_pNetplaneMacRange  = new CDBOperateNetplaneMacRange(pIProc->GetIProcedure());
    if( NULL == m_pNetplaneMacRange )
    {
        VNET_LOG(VNET_LOG_ERROR,"CVNetDbMgr::Init alloc memory CDBOperateNetplaneMacRange failed.\n");        
        ret= VNET_DB_ERROR_ALLOC_MEM_FAILED;
        goto free_module;
    } 

    m_pNetplaneMacPool  = new CDBOperateNetplaneMacPool(pIProc->GetIProcedure());
    if( NULL == m_pNetplaneMacPool )
    {
        VNET_LOG(VNET_LOG_ERROR,"CVNetDbMgr::Init alloc memory CDBOperateNetplaneMacPool failed.\n");        
        ret= VNET_DB_ERROR_ALLOC_MEM_FAILED;
        goto free_module;
    } 

    m_pNetplaneVlanRange  = new CDBOperateNetplaneVlanRange(pIProc->GetIProcedure());
    if( NULL == m_pNetplaneVlanRange )
    {
        VNET_LOG(VNET_LOG_ERROR,"CVNetDbMgr::Init alloc memory CDBOperateNetplaneVlanRange failed.\n");        
        ret= VNET_DB_ERROR_ALLOC_MEM_FAILED;
        goto free_module;
    } 

    m_pNetplaneVlanPool  = new CDBOperateNetplaneVlanPool(pIProc->GetIProcedure());
    if( NULL == m_pNetplaneVlanPool )
    {
        VNET_LOG(VNET_LOG_ERROR,"CVNetDbMgr::Init alloc memory CDBOperateNetplaneVlanPool failed.\n");        
        ret= VNET_DB_ERROR_ALLOC_MEM_FAILED;
        goto free_module;
    }

    m_pNetplaneSegmentRange  = new CDBOperateNetplaneSegmentRange(pIProc->GetIProcedure());
    if( NULL == m_pNetplaneSegmentRange )
    {
        VNET_LOG(VNET_LOG_ERROR,"CVNetDbMgr::Init alloc memory CDBOperateNetplaneSegmentRange failed.\n");        
        ret= VNET_DB_ERROR_ALLOC_MEM_FAILED;
        goto free_module;
    } 

    m_pNetplaneSegmentPool  = new CDBOperateNetplaneSegmentPool(pIProc->GetIProcedure());
    if( NULL == m_pNetplaneSegmentPool )
    {
        VNET_LOG(VNET_LOG_ERROR,"CVNetDbMgr::Init alloc memory CDBOperateNetplaneSegmentPool failed.\n");        
        ret= VNET_DB_ERROR_ALLOC_MEM_FAILED;
        goto free_module;
    }

    m_pVlanMap  = new CDBOperateVlanMap(pIProc->GetIProcedure());
    if( NULL == m_pVlanMap )
    {
        VNET_LOG(VNET_LOG_ERROR,"CVNetDbMgr::Init alloc memory CDBOperateVlanMap failed.\n");        
        ret= VNET_DB_ERROR_ALLOC_MEM_FAILED;
        goto free_module;
    }

    m_pSegmentMap  = new CDBOperateSegmentMap(pIProc->GetIProcedure());
    if( NULL == m_pSegmentMap )
    {
        VNET_LOG(VNET_LOG_ERROR,"CVNetDbMgr::Init alloc memory CDBOperateSegmentMap failed.\n");        
        ret= VNET_DB_ERROR_ALLOC_MEM_FAILED;
        goto free_module;
    }    

    m_pBond  = new CDBOperateBond(pIProc->GetIProcedure());
    if( NULL == m_pBond )
    {
        VNET_LOG(VNET_LOG_ERROR,"CVNetDbMgr::Init alloc memory CDBOperateBond failed.\n");        
        ret= VNET_DB_ERROR_ALLOC_MEM_FAILED;
        goto free_module;
    }   

    m_pVsi  = new CDBOperateVsi(pIProc->GetIProcedure());
    if( NULL == m_pVsi )
    {
        VNET_LOG(VNET_LOG_ERROR,"CVNetDbMgr::Init alloc memory CDBOperateVsi failed.\n");        
        ret= VNET_DB_ERROR_ALLOC_MEM_FAILED;
        goto free_module;
    } 

    m_pSchedule = new CDBOperateSchedule(pIProc->GetIProcedure());
    if( NULL == m_pSchedule )
    {
        VNET_LOG(VNET_LOG_ERROR,"CVNetDbMgr::Init alloc memory CDBOperateSchedule failed.\n");        
        ret= VNET_DB_ERROR_ALLOC_MEM_FAILED;
        goto free_module;
    } 
   
    m_pPortMac = new CDBOperatePortMac(pIProc->GetIProcedure());
    if( NULL == m_pPortMac )
    {
        VNET_LOG(VNET_LOG_ERROR,"CVNetDbMgr::Init alloc memory CDBOperatePortMac failed.\n");        
        ret= VNET_DB_ERROR_ALLOC_MEM_FAILED;
        goto free_module;
    }

    m_pPortIp = new CDBOperatePortIp(pIProc->GetIProcedure());
    if( NULL == m_pPortIp )
    {
        VNET_LOG(VNET_LOG_ERROR,"CVNetDbMgr::Init alloc memory CDBOperatePortIp failed.\n");        
        ret= VNET_DB_ERROR_ALLOC_MEM_FAILED;
        goto free_module;
    }
    
    m_pPortVSwitch = new CDBOperatePortVSwitch(pIProc->GetIProcedure());
    if( NULL == m_pPortVSwitch )
    {
        VNET_LOG(VNET_LOG_ERROR,"CVNetDbMgr::Init alloc memory CDBOperatePortVSwitch failed.\n");        
        ret= VNET_DB_ERROR_ALLOC_MEM_FAILED;
        goto free_module;
    }

    m_pKernelReport = new CDBOperateKernelReport(pIProc->GetIProcedure());
    if( NULL == m_pKernelReport )
    {
        VNET_LOG(VNET_LOG_ERROR,"CVNetDbMgr::Init alloc memory CDBOperateKernelReport failed.\n");        
        ret= VNET_DB_ERROR_ALLOC_MEM_FAILED;
        goto free_module;
    }

    m_pWildcastLoopBackPort = new CDBOperateWildcastLoopbackPort(pIProc->GetIProcedure());
    if( NULL == m_pWildcastLoopBackPort )
    {
        VNET_LOG(VNET_LOG_ERROR,"CVNetDbMgr::Init alloc memory CDBOperateWildcastLoopbackPort failed.\n");        
        ret= VNET_DB_ERROR_ALLOC_MEM_FAILED;
        goto free_module;
    }

    m_pWildcastCreateVPort = new CDBOperateWildcastCreateVPort(pIProc->GetIProcedure());
    if( NULL == m_pWildcastCreateVPort )
    {
        VNET_LOG(VNET_LOG_ERROR,"CVNetDbMgr::Init alloc memory CDBOperateWildcastCreateVPort failed.\n");        
        ret= VNET_DB_ERROR_ALLOC_MEM_FAILED;
        goto free_module;
    }

    m_pWildcastSwitch = new CDBOperateWildcastSwitch(pIProc->GetIProcedure());
    if( NULL == m_pWildcastSwitch )
    {
        VNET_LOG(VNET_LOG_ERROR,"CVNetDbMgr::Init alloc memory CDBOperateWildcastSwitch failed.\n");        
        ret= VNET_DB_ERROR_ALLOC_MEM_FAILED;
        goto free_module;
    }

    m_pWildcastTaskSwitch = new CDBOperateWildcastTaskSwitch(pIProc->GetIProcedure());
    if( NULL == m_pWildcastTaskSwitch )
    {
        VNET_LOG(VNET_LOG_ERROR,"CVNetDbMgr::Init alloc memory CDBOperateWildcastTaskSwitch failed.\n");        
        ret= VNET_DB_ERROR_ALLOC_MEM_FAILED;
        goto free_module;
    }

    m_pWildcastTaskCreateVPort= new CDBOperateWildcastTaskCreateVPort(pIProc->GetIProcedure());
    if( NULL == m_pWildcastTaskCreateVPort )
    {
        VNET_LOG(VNET_LOG_ERROR,"CVNetDbMgr::Init alloc memory CDBOperateWildcastTaskCreateVPort failed.\n");        
        ret= VNET_DB_ERROR_ALLOC_MEM_FAILED;
        goto free_module;
    }

    m_pWildcastTaskLoopback = new CDBOperateWildcastTaskLoopback(pIProc->GetIProcedure());
    if( NULL == m_pWildcastTaskLoopback )
    {
        VNET_LOG(VNET_LOG_ERROR,"CVNetDbMgr::Init alloc memory CDBOperateWildcastTaskLoopback failed.\n");        
        ret= VNET_DB_ERROR_ALLOC_MEM_FAILED;
        goto free_module;
    }

    m_pVmMigrate  = new CDBOperateVmMigrate(pIProc->GetIProcedure());
    if( NULL == m_pVmMigrate )
    {
        VNET_LOG(VNET_LOG_ERROR,"CVNetDbMgr::Init alloc memory CDBOperateVmMigrate failed.\n");        
        ret= VNET_DB_ERROR_ALLOC_MEM_FAILED;
        goto free_module;
    }

    m_pVm  = new CDBOperateVm(pIProc->GetIProcedure());
    if( NULL == m_pVm )
    {
        VNET_LOG(VNET_LOG_ERROR,"CVNetDbMgr::Init alloc memory CDBOperateVm failed.\n");        
        ret= VNET_DB_ERROR_ALLOC_MEM_FAILED;
        goto free_module;
    }

    m_pSdnCfg  = new CDBOperateSdnCfg(pIProc->GetIProcedure());
    if( NULL == m_pVm )
    {
        VNET_LOG(VNET_LOG_ERROR,"CVNetDbMgr::Init alloc memory CDBOperateSdnCfg failed.\n");        
        ret= VNET_DB_ERROR_ALLOC_MEM_FAILED;
        goto free_module;
    }

    m_pPortVtep  = new CDBOperatePortVtep(pIProc->GetIProcedure());
    if( NULL == m_pPortVtep )
    {
        VNET_LOG(VNET_LOG_ERROR,"CVNetDbMgr::Init alloc memory CDBOperatePortVtep failed.\n");        
        ret= VNET_DB_ERROR_ALLOC_MEM_FAILED;
        goto free_module;
    }

    m_pQuantumNetwork  = new CDBOperateQuantumNetwork(pIProc->GetIProcedure());
    if( NULL == m_pQuantumNetwork )
    {
        VNET_LOG(VNET_LOG_ERROR,"CVNetDbMgr::Init alloc memory CDBOperateQuantumNetwork failed.\n");        
        ret= VNET_DB_ERROR_ALLOC_MEM_FAILED;
        goto free_module;
    }

    m_pQuantumPort  = new CDBOperateQuantumPort(pIProc->GetIProcedure());
    if( NULL == m_pQuantumPort )
    {
        VNET_LOG(VNET_LOG_ERROR,"CVNetDbMgr::Init alloc memory CDBOperateQuantumPort failed.\n");        
        ret= VNET_DB_ERROR_ALLOC_MEM_FAILED;
        goto free_module;
    }
        
    m_bInit = TRUE;
    
    VNET_LOG(VNET_LOG_INFO,"CVNetDbMgr::Init success.\n");
    return 0;


    
free_module:    
    if(m_pNetplane)
    {
        delete m_pNetplane;
        m_pNetplane = NULL;
    }

    if(m_pVna)
    {
        delete m_pVna;
        m_pVna = NULL;
    }

    if(m_pVnaRegModule)
    {
        delete m_pVnaRegModule;
        m_pVnaRegModule = NULL;
    }

    if(m_pPort)
    {
        delete m_pPort;
        m_pPort = NULL;
    }

    if(m_pPortPhy)
    {
        delete m_pPortPhy;
        m_pPortPhy = NULL;
    }

    if(m_pPortSriovVf)
    {
        delete m_pPortSriovVf;
        m_pPortSriovVf = NULL;
    }

    if(m_pPortUplinkLoop)
    {
        delete m_pPortUplinkLoop;
        m_pPortUplinkLoop = NULL;
    }

    if(m_pPortKernel)
    {
        delete m_pPortKernel;
        m_pPortKernel = NULL;
    }

    if(m_pPortGroup)
    {
        delete m_pPortGroup;
        m_pPortGroup = NULL;
    }

    if(m_pPgTrunkVlanRange)
    {
        delete m_pPgTrunkVlanRange;
        m_pPgTrunkVlanRange = NULL;
    }

    if(m_pVSwitch)
    {
        delete m_pVSwitch;
        m_pVSwitch = NULL;
    }

    if(m_pVSwitchMap)
    {
        delete m_pVSwitchMap;
        m_pVSwitchMap = NULL;
    }
    if(m_pLogicNetwork)
    {
        delete m_pLogicNetwork;
        m_pLogicNetwork = NULL;
    }
    if(m_pLogicNetworkIpRange)
    {
        delete m_pLogicNetworkIpRange;
        m_pLogicNetworkIpRange = NULL;
    }
    if(m_pLogicNetworkIpPool)
    {
        delete m_pLogicNetworkIpPool;
        m_pLogicNetworkIpPool = NULL;
    }

    if(m_pNetplaneIpRange)
    {
        delete m_pNetplaneIpRange;
        m_pNetplaneIpRange = NULL;
    }

    if(m_pNetplaneIpPool)
    {
        delete m_pNetplaneIpPool;
        m_pNetplaneIpPool = NULL;
    }
    
    if(m_pNetplaneMacRange)
    {
        delete m_pNetplaneMacRange;
        m_pNetplaneMacRange = NULL;
    }

    if(m_pNetplaneMacPool)
    {
        delete m_pNetplaneMacPool;
        m_pNetplaneMacPool = NULL;
    }
    if(m_pNetplaneVlanRange)
    {
        delete m_pNetplaneVlanRange;
        m_pNetplaneVlanRange = NULL;
    }

    if(m_pNetplaneVlanPool)
    {
        delete m_pNetplaneVlanPool;
        m_pNetplaneVlanPool = NULL;
    }

    if(m_pNetplaneSegmentRange)
    {
        delete m_pNetplaneSegmentRange;
        m_pNetplaneSegmentRange = NULL;
    }

    if(m_pNetplaneSegmentPool)
    {
        delete m_pNetplaneSegmentPool;
        m_pNetplaneSegmentPool = NULL;
    }

    if(m_pVlanMap)
    {
        delete m_pVlanMap;
        m_pVlanMap = NULL;
    }

    if(m_pSegmentMap)
    {
        delete m_pSegmentMap;
        m_pSegmentMap = NULL;
    }    

    if(m_pBond)
    {
        delete m_pBond;
        m_pBond = NULL;
    }

    if(m_pVsi)
    {
        delete m_pVsi;
        m_pVsi = NULL;
    }

    if(m_pSchedule)
    {
        delete m_pSchedule;
        m_pSchedule = NULL;
    }
    
    if(m_pPortMac)
    {
        delete m_pPortMac;
        m_pPortMac = NULL;
    }

    if(m_pPortIp)
    {
        delete m_pPortIp;
        m_pPortIp = NULL;
    }

    if(m_pPortVSwitch)
    {
        delete m_pPortVSwitch;
        m_pPortVSwitch = NULL;
    }

    if(m_pKernelReport)
    {
        delete m_pKernelReport;
        m_pKernelReport = NULL;
    }    

    if(m_pWildcastLoopBackPort)
    {
        delete m_pWildcastLoopBackPort;
        m_pWildcastLoopBackPort = NULL;
    }  

    if(m_pWildcastCreateVPort)
    {
        delete m_pWildcastCreateVPort;
        m_pWildcastCreateVPort = NULL;
    } 
    
    if(m_pWildcastSwitch)
    {
        delete m_pWildcastSwitch;
        m_pWildcastSwitch = NULL;
    } 

    if(m_pWildcastTaskSwitch)
    {
        delete m_pWildcastTaskSwitch;
        m_pWildcastTaskSwitch = NULL;
    }

    if(m_pWildcastTaskCreateVPort)
    {
        delete m_pWildcastTaskCreateVPort;
        m_pWildcastTaskCreateVPort = NULL;
    }

    if(m_pWildcastTaskLoopback)
    {
        delete m_pWildcastTaskLoopback;
        m_pWildcastTaskLoopback = NULL;
    }

    if(m_pVmMigrate)
    {
        delete m_pVmMigrate;
        m_pVmMigrate = NULL;
    }

    if(m_pVm)
    {
        delete m_pVm;
        m_pVm = NULL;
    }

    if(m_pSdnCfg)
    {
        delete m_pSdnCfg;
        m_pSdnCfg = NULL;
    }

    if(m_pPortVtep)
    {
        delete m_pPortVtep;
        m_pPortVtep = NULL;
    }

    if(m_pQuantumNetwork)
    {
        delete m_pQuantumNetwork;
        m_pQuantumNetwork = NULL;
    }

    if(m_pQuantumPort)
    {
        delete m_pQuantumPort;
        m_pQuantumPort = NULL;
    }
    
    return ret;
}

CDBOperateNetplane* CVNetDbMgr::GetINetplane()
{
    if(m_bInit )
    {
        return m_pNetplane;
    }
    return NULL;
}

CDBOperateVna* CVNetDbMgr::GetIVna()
{
    if(m_bInit )
    {
        return m_pVna;
    }
    return NULL;
}

CDBOperateVnaRegModule* CVNetDbMgr::GetIVnaRegModule()
{
    if(m_bInit )
    {
        return m_pVnaRegModule;
    }
    return NULL;
}

CDBOperatePort* CVNetDbMgr::GetIPort()
{
    if(m_bInit )
    {
        return m_pPort;
    }
    return NULL;
}

CDBOperatePortPhysical* CVNetDbMgr::GetIPortPhy()
{
    if(m_bInit )
    {
        return m_pPortPhy;
    }
    return NULL;
}

CDBOperatePortSriovVf* CVNetDbMgr::GetIPortSriovVf()
{
    if(m_bInit )
    {
        return m_pPortSriovVf;
    }
    return NULL;
}

CDBOperatePortUplinkLoop* CVNetDbMgr::GetIPortUplinkLoop()
{
    if(m_bInit )
    {
        return m_pPortUplinkLoop;
    }
    return NULL;
}

CDBOperatePortKernel* CVNetDbMgr::GetIPortKernel()
{
    if(m_bInit )
    {
        return m_pPortKernel;
    }
    return NULL;
}


CDBOperatePortGroup *  CVNetDbMgr::GetIPortGroup()
{
    if(m_bInit )
    {
        return m_pPortGroup;
    }
    return NULL;
}

CDBOperatePgTrunkVlanRange *  CVNetDbMgr::GetIPgTrunkVlanRange()
{
    if(m_bInit )
    {
        return m_pPgTrunkVlanRange;
    }
    return NULL;
}

CDBOperateVSwitch *  CVNetDbMgr::GetIVSwitch()
{
    if(m_bInit )
    {
        return m_pVSwitch;
    }
    return NULL;
}

CDBOperateVSwitchMap *  CVNetDbMgr::GetIVSwitchMap()
{
    if(m_bInit )
    {
        return m_pVSwitchMap;
    }
    return NULL;
}

CDBOperateLogicNetwork* CVNetDbMgr::GetILogicNetwork()
{

    if(m_bInit )
    {
        return m_pLogicNetwork;
    }
    return NULL;
}
CDBOperateLogicNetworkIpRange * CVNetDbMgr::GetILogicNetworkIpRange()
{
    if(m_bInit )
    {
        return m_pLogicNetworkIpRange;
    }
    return NULL;
}

CDBOperateLogicNetworkIpPool * CVNetDbMgr::GetILogicNetworkIpPool()
{
    if(m_bInit )
    {
        return m_pLogicNetworkIpPool;
    }
    return NULL;
}

CDBOperateNetplaneIpRange * CVNetDbMgr::GetINetplaneIpRange()
{
    if(m_bInit )
    {
        return m_pNetplaneIpRange;
    }
    return NULL;
}

CDBOperateNetplaneIpPool * CVNetDbMgr::GetINetplaneIpPool()
{
    if(m_bInit )
    {
        return m_pNetplaneIpPool;
    }
    return NULL;
}

CDBOperateNetplaneMacRange * CVNetDbMgr::GetINetplaneMacRange()
{
    if(m_bInit )
    {
        return m_pNetplaneMacRange;
    }
    return NULL;
}

CDBOperateNetplaneMacPool * CVNetDbMgr::GetINetplaneMacPool()
{
    if(m_bInit )
    {
        return m_pNetplaneMacPool;
    }
    return NULL;
}

CDBOperateNetplaneVlanRange * CVNetDbMgr::GetINetplaneVlanRange()
{
    if(m_bInit )
    {
        return m_pNetplaneVlanRange;
    }
    return NULL;
}

CDBOperateNetplaneVlanPool * CVNetDbMgr::GetINetplaneVlanPool()
{
    if(m_bInit )
    {
        return m_pNetplaneVlanPool;
    }
    return NULL;
}

CDBOperateNetplaneSegmentRange * CVNetDbMgr::GetINetplaneSegmentRange()
{
    if(m_bInit )
    {
        return m_pNetplaneSegmentRange;
    }
    return NULL;
}

CDBOperateNetplaneSegmentPool * CVNetDbMgr::GetINetplaneSegmentPool()
{
    if(m_bInit )
    {
        return m_pNetplaneSegmentPool;
    }
    return NULL;
}

CDBOperateVlanMap * CVNetDbMgr::GetIVlanMap()
{
    if(m_bInit )
    {
        return m_pVlanMap;
    }
    return NULL;
}

CDBOperateSegmentMap * CVNetDbMgr::GetISegmentMap()
{
    if(m_bInit )
    {
        return m_pSegmentMap;
    }
    return NULL;
}


CDBOperateBond* CVNetDbMgr::GetIBond()
{
    if(m_bInit )
    {
        return m_pBond;
    }
    return NULL;
}

CDBOperateVsi* CVNetDbMgr::GetIVsi()
{
    if(m_bInit )
    {
        return m_pVsi;
    }
    return NULL;
}

CDBOperateSchedule* CVNetDbMgr::GetISchedule()
{
    if(m_bInit )
    {
        return m_pSchedule;
    }
    return NULL;
}

CDBOperatePortMac* CVNetDbMgr::GetIPortMac()
{
    if(m_bInit )
    {
        return m_pPortMac;
    }
    return NULL;
}
CDBOperatePortIp* CVNetDbMgr::GetIPortIp()
{
    if(m_bInit )
    {
        return m_pPortIp;
    }
    return NULL;
}

CDBOperatePortVSwitch* CVNetDbMgr::GetIPortVSwitch()
{
    if(m_bInit )
    {
        return m_pPortVSwitch;
    }
    return NULL;
}

CDBOperateKernelReport* CVNetDbMgr::GetIKernelReport()
{
    if(m_bInit )
    {
        return m_pKernelReport;
    }
    return NULL;
}

CDBOperateWildcastLoopbackPort * CVNetDbMgr::GetIWildcastLoopbackPort()
{
    if(m_bInit )
    {
        return m_pWildcastLoopBackPort;
    }
    return NULL;
}

CDBOperateWildcastCreateVPort * CVNetDbMgr::GetIWildcastCreateVPort()
{
    if(m_bInit )
    {
        return m_pWildcastCreateVPort;
    }
    return NULL;
}   

CDBOperateWildcastSwitch * CVNetDbMgr::GetIWildcastSwitch()
{
    if(m_bInit )
    {
        return m_pWildcastSwitch;
    }
    return NULL;
}   

CDBOperateWildcastTaskSwitch * CVNetDbMgr::GetIWildcastTaskSwitch()
{
    if(m_bInit )
    {
        return m_pWildcastTaskSwitch;
    }
    return NULL;
}

CDBOperateWildcastTaskCreateVPort * CVNetDbMgr::GetIWildcastTaskCreateVPort()
{
    if(m_bInit )
    {
        return m_pWildcastTaskCreateVPort;
    }
    return NULL;
}

CDBOperateWildcastTaskLoopback * CVNetDbMgr::GetIWildcastTaskLoopback()
{
    if(m_bInit )
    {
        return m_pWildcastTaskLoopback;
    }
    return NULL;
}
CDBOperateVmMigrate * CVNetDbMgr::GetIVmMigrate()
{
    if(m_bInit )
    {
        return m_pVmMigrate;
    }
    return NULL;
}

CDBOperateVm * CVNetDbMgr::GetIVm()
{
    if(m_bInit )
    {
        return m_pVm;
    }
    return NULL;
}

CDBOperateSdnCfg * CVNetDbMgr::GetISdnCfg()
{
    if(m_bInit )
    {
        return m_pSdnCfg;
    }
    return NULL;
}

CDBOperatePortVtep * CVNetDbMgr::GetIPortVtep()
{
    if(m_bInit )
    {
        return m_pPortVtep;
    }
    return NULL;
}

CDBOperateQuantumNetwork * CVNetDbMgr::GetIQuantumNetwork()
{
    if(m_bInit )
    {
        return m_pQuantumNetwork;
    }
    return NULL;
}

CDBOperateQuantumPort * CVNetDbMgr::GetIQuantumPort()
{
    if(m_bInit )
    {
        return m_pQuantumPort;
    }
    return NULL;
}

int32 CVNetDbMgr::DbgShow()
{
    cout << "--------------------------CVNetDbMgr Info------------------------" << endl;
    cout << "Init : " << m_bInit << endl;
    cout << "Netplane : " << m_pNetplane << endl; 
    cout << "Netplane : " << m_pNetplane << endl;  
    cout << "Vna      : " << m_pVna << endl;  
    cout << "RegModule: " << m_pVnaRegModule << endl;      

    if( m_pNetplane )
    {
        m_pNetplane->DbgShow();
    }
    if( m_pVna )
    {
        m_pVna->DbgShow();
    }
    if( m_pVnaRegModule )
    {
        m_pVnaRegModule->DbgShow();
    }
    return 0;
}

#if 0
string GenerateUUID()
{
    uuid_t uuid;
    char   acUuid[64]={0};
    string tmp_str;
    uuid_create(&uuid);        
    sprintf(acUuid,UUIDFORMAT_PRINTF,uuid.time_low, 
            uuid.time_mid,uuid.time_hi_and_version, 
            uuid.clock_seq_hi_and_reserved, uuid.clock_seq_low,
            uuid.node[0],uuid.node[1],uuid.node[2], 
            uuid.node[3],uuid.node[4],uuid.node[5]);
    tmp_str = acUuid;
    return tmp_str; 
}
#endif

void DbgShowDb()
{
    CVNetDbMgr * pDb = CVNetDbMgr::GetInstance();
    if( pDb == NULL)
    {
        cout << "CVNetDbMgr::GetInstance() is NULL" << endl;
        return;
    }

    pDb->DbgShow();
}
DEFINE_DEBUG_FUNC(DbgShowDb);


CDBOperateNetplane* GetDbINetplane()
{

    CVNetDbMgr * pDb = CVNetDbMgr::GetInstance();
    if( pDb == NULL)
    {
        return NULL;
    }
    return pDb->GetINetplane();
}

CDBOperateVna* GetDbIVna()
{
    CVNetDbMgr * pDb = CVNetDbMgr::GetInstance();
    if( pDb == NULL)
    {
        return NULL;
    }
    return pDb->GetIVna();
}

CDBOperateVnaRegModule* GetDbIVnaRegModule()
{

    CVNetDbMgr * pDb = CVNetDbMgr::GetInstance();
    if( pDb == NULL)
    {
        return NULL;
    }
    return pDb->GetIVnaRegModule();
}

CDBOperatePort* GetDbIPort()
{

    CVNetDbMgr * pDb = CVNetDbMgr::GetInstance();
    if( pDb == NULL)
    {
        return NULL;
    }
    return pDb->GetIPort();
}

CDBOperatePortPhysical* GetDbIPortPhy()
{

    CVNetDbMgr * pDb = CVNetDbMgr::GetInstance();
    if( pDb == NULL)
    {
        return NULL;
    }
    return pDb->GetIPortPhy();
}

CDBOperatePortSriovVf* GetDbIPortSriovVf()
{

    CVNetDbMgr * pDb = CVNetDbMgr::GetInstance();
    if( pDb == NULL)
    {
        return NULL;
    }
    return pDb->GetIPortSriovVf();
}

CDBOperatePortUplinkLoop* GetDbIPortUplinkLoop()
{
    CVNetDbMgr * pDb = CVNetDbMgr::GetInstance();
    if( pDb == NULL)
    {
        return NULL;
    }
    return pDb->GetIPortUplinkLoop();
}

CDBOperatePortKernel* GetDbIPortKernel()
{
    CVNetDbMgr * pDb = CVNetDbMgr::GetInstance();
    if( pDb == NULL)
    {
        return NULL;
    }
    return pDb->GetIPortKernel();
}

CDBOperatePortGroup* GetDbIPortGroup()
{
    CVNetDbMgr * pDb = CVNetDbMgr::GetInstance();
    if( pDb == NULL)
    {
        return NULL;
    }
    return pDb->GetIPortGroup();
}

CDBOperatePgTrunkVlanRange* GetDbIPgTrunkVlanRange()
{
    CVNetDbMgr * pDb = CVNetDbMgr::GetInstance();
    if( pDb == NULL)
    {
        return NULL;
    }
    return pDb->GetIPgTrunkVlanRange();
}

CDBOperateVSwitch * GetDbIVSwitch()
{
    CVNetDbMgr * pDb = CVNetDbMgr::GetInstance();
    if( pDb == NULL)
    {
        return NULL;
    }
    return pDb->GetIVSwitch();
}
CDBOperateVSwitchMap * GetDbIVSwitchMap()
{
    CVNetDbMgr * pDb = CVNetDbMgr::GetInstance();
    if( pDb == NULL)
    {
        return NULL;
    }
    return pDb->GetIVSwitchMap();
}

CDBOperateLogicNetwork * GetDbILogicNetwork()
{
    CVNetDbMgr * pDb = CVNetDbMgr::GetInstance();
    if( pDb == NULL)
    {
        return NULL;
    }
    return pDb->GetILogicNetwork();
}
CDBOperateLogicNetworkIpRange * GetDbILogicNetworkIpRange()
{
    CVNetDbMgr * pDb = CVNetDbMgr::GetInstance();
    if( pDb == NULL)
    {
        return NULL;
    }
    return pDb->GetILogicNetworkIpRange();
}

CDBOperateLogicNetworkIpPool * GetDbILogicNetworkIpPool()
{
    CVNetDbMgr * pDb = CVNetDbMgr::GetInstance();
    if( pDb == NULL)
    {
        return NULL;
    }
    return pDb->GetILogicNetworkIpPool();
}

CDBOperateNetplaneIpRange * GetDbINetplaneIpRange()
{
    CVNetDbMgr * pDb = CVNetDbMgr::GetInstance();
    if( pDb == NULL)
    {
        return NULL;
    }
    return pDb->GetINetplaneIpRange();
}

CDBOperateNetplaneIpPool * GetDbINetplaneIpPool()
{
    CVNetDbMgr * pDb = CVNetDbMgr::GetInstance();
    if( pDb == NULL)
    {
        return NULL;
    }
    return pDb->GetINetplaneIpPool();
}


CDBOperateNetplaneMacRange * GetDbINetplaneMacRange()
{
    CVNetDbMgr * pDb = CVNetDbMgr::GetInstance();
    if( pDb == NULL)
    {
        return NULL;
    }
    return pDb->GetINetplaneMacRange();
}

CDBOperateNetplaneMacPool * GetDbINetplaneMacPool()
{
    CVNetDbMgr * pDb = CVNetDbMgr::GetInstance();
    if( pDb == NULL)
    {
        return NULL;
    }
    return pDb->GetINetplaneMacPool();
}

CDBOperateNetplaneVlanRange * GetDbINetplaneVlanRange()
{
    CVNetDbMgr * pDb = CVNetDbMgr::GetInstance();
    if( pDb == NULL)
    {
        return NULL;
    }
    return pDb->GetINetplaneVlanRange();
}

CDBOperateNetplaneSegmentRange * GetDbINetplaneSegmentRange()
{
    CVNetDbMgr * pDb = CVNetDbMgr::GetInstance();
    if( pDb == NULL)
    {
        return NULL;
    }
    return pDb->GetINetplaneSegmentRange();
}

CDBOperateNetplaneVlanPool * GetDbINetplaneVlanPool()
{
    CVNetDbMgr * pDb = CVNetDbMgr::GetInstance();
    if( pDb == NULL)
    {
        return NULL;
    }
    return pDb->GetINetplaneVlanPool();
}

CDBOperateVlanMap * GetDbIVlanMap()
{
    CVNetDbMgr * pDb = CVNetDbMgr::GetInstance();
    if( pDb == NULL)
    {
        return NULL;
    }
    return pDb->GetIVlanMap();
}

CDBOperateNetplaneSegmentPool * GetDbINetplaneSegmentPool()
{
    CVNetDbMgr * pDb = CVNetDbMgr::GetInstance();
    if( pDb == NULL)
    {
        return NULL;
    }
    return pDb->GetINetplaneSegmentPool();
}

CDBOperateSegmentMap * GetDbISegmentMap()
{
    CVNetDbMgr * pDb = CVNetDbMgr::GetInstance();
    if( pDb == NULL)
    {
        return NULL;
    }
    return pDb->GetISegmentMap();
}


CDBOperateBond * GetDbIBond()
{
    CVNetDbMgr * pDb = CVNetDbMgr::GetInstance();
    if( pDb == NULL)
    {
        return NULL;
    }
    return pDb->GetIBond();
}

CDBOperateVsi * GetDbIVsi()
{
    CVNetDbMgr * pDb = CVNetDbMgr::GetInstance();
    if( pDb == NULL)
    {
        return NULL;
    }
    return pDb->GetIVsi();
}
CDBOperateSchedule * GetDbISchedule()
{
    CVNetDbMgr * pDb = CVNetDbMgr::GetInstance();
    if( pDb == NULL)
    {
        return NULL;
    }
    return pDb->GetISchedule();
}

CDBOperatePortMac * GetDbIPortMac()
{
    CVNetDbMgr * pDb = CVNetDbMgr::GetInstance();
    if( pDb == NULL)
    {
        return NULL;
    }
    return pDb->GetIPortMac();
}

CDBOperatePortIp * GetDbIPortIp()
{
    CVNetDbMgr * pDb = CVNetDbMgr::GetInstance();
    if( pDb == NULL)
    {
        return NULL;
    }
    return pDb->GetIPortIp();
}
CDBOperatePortVSwitch *GetDbIPortVSwitch()
{
    CVNetDbMgr * pDb = CVNetDbMgr::GetInstance();
    if( pDb == NULL)
    {
        return NULL;
    }
    return pDb->GetIPortVSwitch();
}
CDBOperateKernelReport *GetDbIKernelReport()
{
    CVNetDbMgr * pDb = CVNetDbMgr::GetInstance();
    if( pDb == NULL)
    {
        return NULL;
    }
    return pDb->GetIKernelReport();
}

CDBOperateWildcastLoopbackPort * GetDbIWildcastLoopbackPort()
{
    CVNetDbMgr * pDb = CVNetDbMgr::GetInstance();
    if( pDb == NULL)
    {
        return NULL;
    }
    return pDb->GetIWildcastLoopbackPort();
}

CDBOperateWildcastCreateVPort * GetDbIWildcastCreateVPort()
{
    CVNetDbMgr * pDb = CVNetDbMgr::GetInstance();
    if( pDb == NULL)
    {
        return NULL;
    }
    return pDb->GetIWildcastCreateVPort();
}

CDBOperateWildcastSwitch * GetDbIWildcastSwitch()
{
    CVNetDbMgr * pDb = CVNetDbMgr::GetInstance();
    if( pDb == NULL)
    {
        return NULL;
    }
    return pDb->GetIWildcastSwitch();
}

CDBOperateWildcastTaskSwitch *  GetDbIWildcastTaskSwitch()
{
    CVNetDbMgr * pDb = CVNetDbMgr::GetInstance();
    if( pDb == NULL)
    {
        return NULL;
    }
    return pDb->GetIWildcastTaskSwitch();
}
CDBOperateWildcastTaskCreateVPort * GetDbIWildcastTaskCreateVPort()
{
    CVNetDbMgr * pDb = CVNetDbMgr::GetInstance();
    if( pDb == NULL)
    {
        return NULL;
    }
    return pDb->GetIWildcastTaskCreateVPort();
}

CDBOperateWildcastTaskLoopback * GetDbIWildcastTaskLoopback()
{
    CVNetDbMgr * pDb = CVNetDbMgr::GetInstance();
    if( pDb == NULL)
    {
        return NULL;
    }
    return pDb->GetIWildcastTaskLoopback();
}

CDBOperateVmMigrate * GetDbIVmMigrate()
{
    CVNetDbMgr * pDb = CVNetDbMgr::GetInstance();
    if( pDb == NULL)
    {
        return NULL;
    }
    return pDb->GetIVmMigrate();
}

CDBOperateVm * GetDbIVm()
{
    CVNetDbMgr * pDb = CVNetDbMgr::GetInstance();
    if( pDb == NULL)
    {
        return NULL;
    }
    return pDb->GetIVm();
}

CDBOperateSdnCfg * GetDbISdnCfg()
{
    CVNetDbMgr * pDb = CVNetDbMgr::GetInstance();
    if( pDb == NULL)
    {
        return NULL;
    }
    return pDb->GetISdnCfg();
}

CDBOperatePortVtep * GetDbIPortVtep()
{
    CVNetDbMgr * pDb = CVNetDbMgr::GetInstance();
    if( pDb == NULL)
    {
        return NULL;
    }
    return pDb->GetIPortVtep();
}

CDBOperateQuantumNetwork * GetDbIQuantumNetwork()
{
    CVNetDbMgr * pDb = CVNetDbMgr::GetInstance();
    if( pDb == NULL)
    {
        return NULL;
    }
    return pDb->GetIQuantumNetwork();
}

CDBOperateQuantumPort * GetDbIQuantumPort()
{
    CVNetDbMgr * pDb = CVNetDbMgr::GetInstance();
    if( pDb == NULL)
    {
        return NULL;
    }
    return pDb->GetIQuantumPort();
}


} // namespace zte_tecs

