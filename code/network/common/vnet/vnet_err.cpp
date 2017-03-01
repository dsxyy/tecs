#include "vnet_comm.h"
#include "vnet_error.h"


#define VNET_ERR_MAP_BEGIN(ERRCODE) \
string info(""); \
switch((ERRCODE))\
{

#define VNET_ERR_MAP_END \
default: break;\
}

#define VNET_ERR_MAP(ERRCODE, ERRINFO) \
case (ERRCODE) : \
info = (ERRINFO); \
info += "("; \
info += exinfo; \
info += ")";\
return info; \
break;


string GetVnetErrorInfo(int32 nErrorCode,const string & exinfo)
{
VNET_ERR_MAP_BEGIN(nErrorCode)
    VNET_ERR_MAP(VNET_SUCCESS,"")
    
    // DB 模块    
    VNET_ERR_MAP(DB_ERROR_GET_INST_FAILED, "Get the database instance failed.")

    // netplane 模块
    VNET_ERR_MAP(NETPLANE_GET_DB_INTF_ERROR, "Get the DB's interface of netplane failed.")
    VNET_ERR_MAP(NETPLANE_DATA_NOT_EXIST, "The netplane data is not exist.")
    VNET_ERR_MAP(NETPLANE_DATA_ALREADY_EXIST, "The netplane data is already exist")
    VNET_ERR_MAP(NETPLANE_CFG_PARAM_ERROR, "The parameter of netplane configuration is error.")

    // Logic network 模块
    VNET_ERR_MAP(LOGIC_NETWORK_GET_DB_INTF_ERROR, "Get the DB's interface of logic network failed.")
    VNET_ERR_MAP(LOGIC_NETWORK_DATA_NOT_EXIST, "The logic network data is not exist.")
    VNET_ERR_MAP(LOGIC_NETWORK_DATA_ALREADY_EXIST, "The logic network data is already exist")
    VNET_ERR_MAP(LOGIC_NETWORK_CFG_PARAM_ERROR, "The parameter of logic network configuration is error.")

    // VNA 管理模块
    // 资源池
    VNET_ERR_MAP(IPV4_RANGE_CFG_PARAM_ERROR, "The parameter of IP address range configuration is error.")
    VNET_ERR_MAP(IPV4_RANGE_GET_LG_NET_DB_INTF_ERROR, "Get the DB's IP address range interface of logic network failed.")
    VNET_ERR_MAP(IPV4_RANGE_CALC_ERROR, "Calculate IP address range failed.")
    VNET_ERR_MAP(IPV4_RANGE_NOT_EXIST, "The IP address range is not exist.")
    VNET_ERR_MAP(IPV4_RANGE_NOT_SAME_SUBNET, "The IP address range is not same subnet.")
    VNET_ERR_MAP(IPV4_RANGE_INVALID, "The IP address range is invalid.")
    VNET_ERR_MAP(IPV4_RANGE_OCCUPIED, "The IP address range is used.")
    VNET_ERR_MAP(IPV4_RANGE_GET_NET_PLANE_DB_INTF_ERROR, "Get the DB's IP address range interface of netplane failed.")

    // MAC Resource Pool 模块;
    VNET_ERR_MAP(MAC_RANGE_CFG_PARAM_ERROR, "The parameter of MAC address range configuration is error.")
    VNET_ERR_MAP(MAC_RANGE_CALC_ERROR, "Calculate MAC address range failed.")
    VNET_ERR_MAP(MAC_RANGE_NOT_EXIST, "The MAC address range is not exist.")
    VNET_ERR_MAP(MAC_RANGE_INVALID, "The MAC address range is invalid.")
    VNET_ERR_MAP(MAC_OUI_INVALID, "The OUI of MAC address is invalid.")
    VNET_ERR_MAP(MAC_PARSE_ERROR, "Parse the MAC address failed.")
    VNET_ERR_MAP(MAC_RANGE_OCCUPIED, "The MAC address range is used.")
    VNET_ERR_MAP(MAC_RANGE_GET_DB_INTF_ERROR, "Get the DB's MAC address range interface failed.")

    // VLAN Resource Pool 模块;
    VNET_ERR_MAP(VLAN_RANGE_CFG_PARAM_ERROR, "The parameter of VLAN range configuration is error.")
    VNET_ERR_MAP(VLAN_RANGE_CALC_ERROR, "Calculate VLAN range failed.")
    VNET_ERR_MAP(VLAN_RANGE_NOT_EXIST, "The VLAN range is not exist.")
    VNET_ERR_MAP(VLAN_RANGE_OCCUPIED, "The VLAN range is used.")
    VNET_ERR_MAP(VLAN_RANGE_GET_DB_INTF_ERROR, "Get the DB's VLAN range interface failed.")

    // SEGMENT Resource Pool 模块;
    VNET_ERR_MAP(SEGMENT_RANGE_CFG_PARAM_ERROR, "The parameter of segment range configuration is error.")
    VNET_ERR_MAP(SEGMENT_RANGE_CALC_ERROR, "Calculate segment range failed.")
    VNET_ERR_MAP(SEGMENT_RANGE_NOT_EXIST, "The segment range is not exist.")
    VNET_ERR_MAP(SEGMENT_RANGE_OCCUPIED, "The segment range is used.")
    VNET_ERR_MAP(SEGMENT_RANGE_GET_DB_INTF_ERROR, "Get the DB's segment range interface failed.")

    // Port Group 模块
    VNET_ERR_MAP(ERROR_PORTGROUP_OPERCODE_INVALID, "The opercode of portgroup is invalid.")
    VNET_ERR_MAP(ERROR_PORTGROUP_PG_INVALID, "The portgroup is empty or invalid.")
    VNET_ERR_MAP(ERROR_PORTGROUP_INPARAM_INVALID, "The input param of portgroup is invalid.")
    VNET_ERR_MAP(ERROR_PORTGROUP_GET_INSTANCE_FAIL, "Get the handler fail.")
    VNET_ERR_MAP(ERROR_PORTGROUP_GET_VLANRANGE_FAIL, "Get the trunk vlanrange fail.")
    VNET_ERR_MAP(ERROR_PORTGROUP_NOT_EXIST, "The portgroup is not exist.")
    VNET_ERR_MAP(ERROR_PORTGROUP_IS_EXIST, "The portgroup is exist.")
    VNET_ERR_MAP(ERROR_PORTGROUP_IS_INUSING, "The portgroup is used already.")
    VNET_ERR_MAP(ERROR_PORTGROUP_OPER_DB_FAIL, "Operate portgroup's DB failed.")
    VNET_ERR_MAP(ERROR_PORTGROUP_OPER_DEFAULT_PG_FAIL, "Can't operator default portgroup.")
    VNET_ERR_MAP(ERROR_PORTGROUP_ACCESS_VLAN_INVALID, "Config access vlan failed.")
    VNET_ERR_MAP(ERROR_PORTGROUP_NATIVE_VLAN_INVALID, "Config native vlan failed.")
    VNET_ERR_MAP(ERROR_PORTGROUP_PRIVATE_VLAN_INVALID, "Config private vlan failed.")
    
    VNET_ERR_MAP(ERROR_PG_VLANRANGE_INVLAID, "Input vlanrange is invalid.")
    VNET_ERR_MAP(ERROR_PG_VLANRANGE_IS_EXIST, "The same vlanrange is exist.")
    VNET_ERR_MAP(ERROR_PG_VLANRANGE_NOT_EXIST, "The trunk vlanrange is not exist.")
    VNET_ERR_MAP(ERROR_PG_VLANRANGE_OPER_DB_FAIL, "Operate vlanrange's DB failed.")
    VNET_ERR_MAP(ERROR_PG_VLANRANGE_CALC_ERROR, "Calculate vlanrange to add/del failed.")
    VNET_ERR_MAP(ERROR_PG_VLANRANGE_IS_CONFLICT, "The trunk vlanrange is conflict with netplane's or nativevlan.")

    //switch 模块
    VNET_ERR_MAP(ERROR_SWITCH_OPERCODE_INVALID, "The opercode of switch is invalid.")
    VNET_ERR_MAP(ERROR_SWITCH_INSTANCE_FAIL, "Switch instance failed.")
    VNET_ERR_MAP(ERROR_SWITCH_GET_PORTMGR_FAIL, "Get portmgr instance fail.")
    VNET_ERR_MAP(ERROR_SWITCH_GET_VSWITCH_DB_FAIL, "Get vswitch instance fail.")
    VNET_ERR_MAP(ERROR_SWITCH_GET_VSWITCHMAP_DB_FAIL, "Get vswitchmap instance fail.")
    VNET_ERR_MAP(ERROR_SWTICH_CONFLICT_WITH_WILDCAST, "Switch is conflict with wildcast.")
    VNET_ERR_MAP(ERROR_SWTICH_PORT_INVALID, "Switch port is invalid.")
    VNET_ERR_MAP(ERROR_SWTICH_GET_PGINFO_FAIL, "Get portgroup info fail.")
    VNET_ERR_MAP(ERROR_SWITCH_NAME_INVALID, "Switch name is invalid.")
    VNET_ERR_MAP(ERROR_SWITCH_ALREADY_EXIST, "Switch is exist already.")
    VNET_ERR_MAP(ERROR_SWITCH_IS_USING,"Switch port is used already.")
    VNET_ERR_MAP(ERROR_SWITCH_OPER_DB_FAIL, "Operate switch's DB fail.")
    VNET_ERR_MAP(ERROR_SWITCH_PORT_OPER_DB_FAIL, "Operate switch port DB fail.")
    VNET_ERR_MAP(ERROR_SWITCH_DEL_NOT_FINISH, "Delete switch not finish.")
    VNET_ERR_MAP(ERROR_SWITCH_GET_PORTINFO_FAIL, "Get port uuid fail.")
    VNET_ERR_MAP(ERROR_SWITCH_ADD_BOND_FAIL, "Add bond fail.")
    VNET_ERR_MAP(ERROR_SWITCH_DEL_BOND_FAIL, "Delete bond fail.")
    VNET_ERR_MAP(ERROR_SWITCH_DEL_BONDSLAVE_FAIL, "Delete bond slave fail.")
    VNET_ERR_MAP(ERROR_SWITCH_GET_BONDSLAVE_FAIL, "Get bond slave fail.")
    VNET_ERR_MAP(ERROR_SWITCH_ADD_BRIDGE_FAIL, "Add bridge fail.")
    VNET_ERR_MAP(ERROR_SWITCH_DEL_BRIDGE_FAIL, "Delete bridge fail.")
    VNET_ERR_MAP(ERROR_SWITCH_SET_ATTR_FAIL, "Set switch attribute fail.")
    VNET_ERR_MAP(ERROR_SWITCH_PORT_ISCONFLICT, "Switch port is conflicted.")
    VNET_ERR_MAP(ERROR_SWITCH_PORT_ISMIXED, "Switch port is mixed.")
    VNET_ERR_MAP(ERROR_SWITCH_OPER_KERNEL_DVS_FAIL, "Can't operator kernel DVS.")
    VNET_ERR_MAP(ERROR_SWITCH_QUERY_VNAUUID_FAIL, "Query VNA fail to alloc DVS.")
    VNET_ERR_MAP(ERROR_SWITCH_MERGE_REQINFO_FAIL, "Merge request resource fail to alloc DVS.")
    VNET_ERR_MAP(ERROR_SWITCH_GET_SRIOV_VFS_FAIL, "Alloc SR-IOV vf resource fail.")
    VNET_ERR_MAP(ERROR_SWITCH_ADD_UPLINK_FAIL, "Add switch's uplink port fail.")
    VNET_ERR_MAP(ERROR_SWITCH_ADD_EVBPORT_FAIL, "Add switch's evb port fail.")
    VNET_ERR_MAP(ERROR_SWITCH_DEL_EVBPORT_FAIL, "Delete switch's evb port fail.")
    VNET_ERR_MAP(ERROR_SWITCH_RENAME_BRIDGE_FAIL, "The switch rename fail.")    
    VNET_ERR_MAP(ERROR_SWITCH_CHECK_PARAM_FAIL, "Check input param fail.")  
    VNET_ERR_MAP(ERROR_SWITCH_TIMER_CONFIG_FAIL, "Use timer for switch fail.")  
    VNET_ERR_MAP(ERROR_SWITCH_GET_VNAINFO_FAIL, "Get all vna of the switch fail.")  
    VNET_ERR_MAP(ERROR_SWITCH_MODIFY_FAIL, "Modify switch attribute fail.")  
    VNET_ERR_MAP(ERROR_SWITCH_ADD_VTEP_FAIL, "Add vtep port fail.")  
    VNET_ERR_MAP(ERROR_SWITCH_DEL_VTEP_FAIL, "Delete vtep port fail.")  
    VNET_ERR_MAP(ERROR_SWITCH_QUERY_VTEP_FAIL, "Query vtep port info fail.")  
    
    //portmgr
    VNET_ERR_MAP(ERROR_PORT_NAME_EMPTY,"input port name is invalid.")
    VNET_ERR_MAP(ERROR_PORT_OPERCODE_INVALID,"input port operate code is invalid.")
    VNET_ERR_MAP(ERROR_PORT_PARA_INVALID,"input port config parameter is invalid.")
    VNET_ERR_MAP(ERROR_PORT_OPER_FAIL,"config port failed in some instances.")
    VNET_ERR_MAP(ERROR_PORT_OPERDB_FAIL,"operate DB failed for config port.")
    VNET_ERR_MAP(ERROR_PORT_WC_CONFLICT,"WC check failed for config port.")
    VNET_ERR_MAP(ERROR_PORT_CHECKDB_CONFLICT,"DB conflict check failed for config port.")

    //schedule 模块
    VNET_ERR_MAP(ERR_SCHEDULE_FAILED_NO_RESOURCE,"No valid network resource to schedule!")
    VNET_ERR_MAP(ERR_SCHEDULE_CHECK_MAC_FAIL,"Check MAC address resource available failed!")
    VNET_ERR_MAP(ERR_SCHEDULE_CHECK_IP_FAIL,"Check IP address resource available failed!")
    VNET_ERR_MAP(ERR_SCHEDULE_CC_APP_IS_NULL,"Cluster application is NULL!")
    VNET_ERR_MAP(ERR_SCHEDULE_APPOINTED_HC_INVALID,"There's no vna belongs to appointed hc!")
    VNET_ERR_MAP(ERR_SCHEDULE_GET_DB_SCHEDULE_HANDLE_FAILED,"Get schedule database handler failed!")
    VNET_ERR_MAP(ERR_SCHEDULE_GET_LG_NETWORK_FAILED,"Get logic network failed!")
    VNET_ERR_MAP(ERR_SCHEDULE_GET_PORT_GROUP_FAILED,"Get port group failed!")
    VNET_ERR_MAP(ERR_SCHEDULE_GET_PG_DETAIL_FAILED,"Get port group detail information failed!")
    VNET_ERR_MAP(ERR_SCHEDULE_GET_LOOP_DEPLOYED_FAILED,"Get loop network deployed information failed!")
    VNET_ERR_MAP(ERR_SCHEDULE_LOOP_DEPLOYED_INFO_INVALID,"The deployed loop network information invalid")
    VNET_ERR_MAP(ERR_SCHEDULE_LOOP_DEPLOYED_CONFLICT,"The deployed loop networks information conflicts!")
    VNET_ERR_MAP(ERR_SCHEDULE_LOOP_DEPLOYED_CC_CONFLICT,"The deployed loop network conflicts with the appointed CC!")
    VNET_ERR_MAP(ERR_SCHEDULE_LOOP_DEPLOYED_HC_CONFLICT,"The deployed loop network conflicts with the appointed HC!")
    VNET_ERR_MAP(ERR_SCHEDULE_CALL_SCHEDULE_DB_FAIL,"Get information from schedule database failed!")
    VNET_ERR_MAP(ERR_SCHEDULE_GET_LN_MGR_INST_FAIL,"Get logic network manager instance failed!")
    VNET_ERR_MAP(ERR_SCHEDULE_GET_PG_MGR_INST_FAIL,"Get port group manager instance failed!")
    VNET_ERR_MAP(ERR_SCHEDULE_CREATE_SCHEDULE_TASK_FAIL, "Create schedule task failed!") 
    VNET_ERR_MAP(ERR_SCHEDULE_SCHEDULE_TASK_INIT_FAIL, "Schedule task init failed!")
    VNET_ERR_MAP(ERR_SCHEDULE_CHECK_SRIOV_CFG_FAIL, "Check SR-IOV interface VLAN config failed!")
    VNET_ERR_MAP(ERR_SCHEDULE_CHECK_SRIOV_CFG_FAIL_OTHER, "Check SR-IOV interface VLAN config failed, querying database maybe failed!")
    VNET_ERR_MAP(ERR_SCHEDULE_ALLOT_SDVSRES_FAIL, "Alloc SDVS resource failed.")
    VNET_ERR_MAP(ERR_SCHEDULE_NO_EDVS_RES, "No EDVS resource to alloc.")
    VNET_ERR_MAP(ERR_SCHEDULE_NO_EDVS_BOND_RES, "No EDVS with Bond resource to alloc.")

    //vnm_vnic 模块
    VNET_ERR_MAP(ERR_VNMVNIC_GET_LN_MGR_INST_FAIL,"Get logic network manager instance failed!")
    VNET_ERR_MAP(ERR_VNMVNIC_GET_LN_INST_FAIL,"Get logic network instance failed!")
    VNET_ERR_MAP(ERR_VNMVNIC_GET_VNIC_DB_HANDLE_FAIL,"Get vnic_db handler failed!")
    VNET_ERR_MAP(ERR_VNMVNIC_GET_VNIC_FROM_DB_FAIL,"Get vnic information from database failed!")
    VNET_ERR_MAP(ERR_VNMVNIC_DEL_VNIC_FROM_DB_FAIL,"Delete vnic information from database failed!")
    VNET_ERR_MAP(ERR_VNMVNIC_ADD_VNIC_TO_DB_FAIL,"Add vnic information from database failed!")
    VNET_ERR_MAP(ERR_VNMVNIC_UPDATE_VNIC_TO_DB_FAIL,"Update vnic information from database failed!")
    VNET_ERR_MAP(ERR_VNMVNIC_ALLOC_VLAN_FAIL,"Allocate VLAN for VM failed!")
    VNET_ERR_MAP(ERR_VNMVNIC_ALLOC_MAC_FAIL,"Allocate MAC address for VM failed!")
    VNET_ERR_MAP(ERR_VNMVNIC_ALLOC_IP_FAIL,"Allocate IP address for VM failed!")
    VNET_ERR_MAP(ERR_VNMVNIC_ALLOC_DVS_FAIL,"Allocate DVS resource for VM failed!")
    VNET_ERR_MAP(ERR_VNMVNIC_FREE_VLAN_FAIL,"Release VLAN for VM failed!")
    VNET_ERR_MAP(ERR_VNMVNIC_FREE_MAC_FAIL,"Release MAC address for VM failed!")
    VNET_ERR_MAP(ERR_VNMVNIC_FREE_IP_FAIL,"Release IP address for VM failed!")
    VNET_ERR_MAP(ERR_VNMVNIC_FREE_DVS_FAIL,"Release DVS resource for VM failed!")    
    VNET_ERR_MAP(ERR_VNMVNIC_GET_PG_DETAIL_FAIL,"Get port group detail information failed!")    
    VNET_ERR_MAP(ERR_VNMVNIC_GET_VLAN_DETAIL_FAIL,"Get VLAN detail information failed!")    
    VNET_ERR_MAP(ERR_VNMVNIC_GET_DVS_DETAIL_FAIL,"Get DVS detail information failed!")    
    VNET_ERR_MAP(ERR_VNMVNIC_GET_VNA_UUID_FAIL,"Get UUID for VNA failed!")  
    VNET_ERR_MAP(ERR_VNMVNIC_GET_VM_DB_HANDLE_FAIL,"Get DB handle failed!")  
    VNET_ERR_MAP(ERR_VNMVNIC_GET_VM_FROM_DB_FAIL,"Get DB handle failed!")  
    VNET_ERR_MAP(ERR_VNMVNIC_CC_APP_CONFLICT,"ClusterName conflict!") 
    VNET_ERR_MAP(ERR_VNMVNIC_ALLOC_SEGMENT_FAIL,"Alloc segment failed!") 
    VNET_ERR_MAP(ERR_VNMVNIC_FREE_SEGMENT_FAIL,"Free segment failed!") 
    VNET_ERR_MAP(ERR_VNMVNIC_GET_SEGMENT_DETAIL_FAIL,"Get segment detail failed!") 
 

    // wdg
    VNET_ERR_MAP(ERROR_WDG_TIMEROUT,"wdg timeout value is invalid.")    
    VNET_ERR_MAP(ERROR_WDG_MAC,"wdg mac is invalid.")  
    VNET_ERR_MAP(ERROR_WDG_START,"start wdg failed for vm.")   
    VNET_ERR_MAP(ERROR_WDG_STOP,"stop wdg failed for vm.")   
    VNET_ERR_MAP(ERROR_WDG_STARTTIMER,"start wdg timer failed for vm.")   
    VNET_ERR_MAP(ERROR_WDG_STOPTIMER,"stop wdg timer failed for vm.")   
    VNET_ERR_MAP(ERROR_WDG_LOCK,"operate wdg lock failed for vm.")   
    VNET_ERR_MAP(ERROR_WDG_TIMER,"operate wdg timer failed for vm.")  

    // vna_agent模块
    VNET_ERR_MAP(ERROR_VNA_AGENT_MODULE_OFFLINE,"The module is offline!")   
    VNET_ERR_MAP(ERROR_VNA_AGENT_MODULE_NOT_REG,"The module is not registered!")   
    VNET_ERR_MAP(ERROR_VNA_DELETE_FAILED,"delete vna failed.") 
    VNET_ERR_MAP(ERROR_VNA_DELETE_DB_OPER_FAILED,"delete vna db failed.")     
    VNET_ERR_MAP(ERROR_VNA_AGENT_MODULE_ONLINE,"The module is online!") 

    // vna_vnic模块    
    VNET_ERR_MAP(ERROR_VNA_VNIC_WDG_INPUT_PARAM_INVALID,"The parameter for watchdog operate is invalid!")   
    VNET_ERR_MAP(ERROR_VNA_VNIC_GET_VSI_FROM_VNM_FAIL,"Get VSI information from VNM failed!") 
    VNET_ERR_MAP(ERROR_VNA_VNIC_GET_REGED_VNM_FAIL,"Get registered VNM information failed!") 
    VNET_ERR_MAP(ERROR_VNA_VNIC_WDG_OPER_FAIL,"Operate the watchdog failed!") 
    VNET_ERR_MAP(ERROR_VNA_VNIC_ALLOC_DVS_FAIL,"Allocate DVS resource failed!") 
    VNET_ERR_MAP(ERROR_VNA_VNIC_FREE_DVS_FAIL,"Free DVS resource failed!") 
    VNET_ERR_MAP(ERROR_VNA_VNIC_VSI_EVB_STATUS_FAIL,"vsi EVB negoti failed!")     

    // wildcast switch
    VNET_ERR_MAP(ERROR_WC_SWITCH_OPERCODE_INVALID,       "vnet wildcast switch msg oper code is invalid.")
    VNET_ERR_MAP(ERROR_WC_SWITCH_INSTANCE_FAIL,          "vnet wildcast switch get switch module instance failed.")    
    VNET_ERR_MAP(ERROR_WC_SWITCH_NAME_EMPTY,             "vnet wildcast switch msg switch name is empty.")  
    VNET_ERR_MAP(ERROR_WC_SWITCH_PG_EMPTY,               "vnet wildcast switch msg portgroup is empty.")  
    VNET_ERR_MAP(ERROR_WC_SWITCH_PGTYPE_INVALID,         "vnet wildcast switch msg portgroup type is invalid.")  
    VNET_ERR_MAP(ERROR_WC_SWITCH_EVBMODE_INVALID,        "vnet wildcast switch msg evb mode is invalid.")  
    VNET_ERR_MAP(ERROR_WC_SWITCH_ALREADY_EXIST,          "vnet wildcast switch has existed.")  
    VNET_ERR_MAP(ERROR_WC_SWITCH_GET_DB_FAIL,            "vnet wildcast switch get DB data failed.")  
    VNET_ERR_MAP(ERROR_WC_SWITCH_NOTEXIST_PORT,          "vnet wildcast switch not exist port")  
    VNET_ERR_MAP(ERROR_WC_SWITCH_PARAM_INVALID,          "vnet wildcast switch msg param is invalid.")  
    VNET_ERR_MAP(ERROR_WC_SWITCH_UPLINKPORT_IS_CONFLICT,    "vnet wildcast switch uplink port is conflict.")
    VNET_ERR_MAP(ERROR_WC_SWITCH_MIX_UPLINKPG_MUST_SAME,    "vnet wildcast mix switch uplink pg must be same.")
    VNET_ERR_MAP(ERROR_WC_SWITCH_MIX_UPLINK_BONDMODE_MUST_SAME,    "vnet wildcast mix switch uplink bond mode must be same.")
    VNET_ERR_MAP(ERROR_WC_SWITCH_MIX_EVBMODE_MUST_SAME,    "vnet wildcast mix switch evb mode must be same.")


    // wildcast create vport
    VNET_ERR_MAP(ERROR_WC_CREATE_VPORT_GET_DB_FAIL,      "vnet wildcast createvport get DB data failed.")
    VNET_ERR_MAP(ERROR_WC_CREATE_VPORT_SET_DB_FAIL,      "vnet wildcast createvport set DB data failed.")
    VNET_ERR_MAP(ERROR_WC_CREATE_VPORT_ALREADY_EXIST,    "vnet wildcast createvport has existed.")
    VNET_ERR_MAP(ERROR_WC_CREATE_VPORT_NAME_EMPTY,       "vnet wildcast createvport msg port name is empty.")
    VNET_ERR_MAP(ERROR_WC_CREATE_VPORT_OPERCODE_INVALID, "vnet wildcast createvport msg oper code is invalid.")
    VNET_ERR_MAP(ERROR_WC_GET_PORT_INSTANCE_NULL,        "vnet wildcast switch get port module instance failed.")
    VNET_ERR_MAP(ERROR_WC_GET_SWTICH_INSTANCE_NULL,      "vnet wildcast switch get switch module instance failed.")
    VNET_ERR_MAP(ERROR_WC_UPLINKPG_MTU_LESS_KNPG_MTU,    "vnet wildcast switch uplink pg mtu less kernel pg mtu.")

    // wildcast loop
    VNET_ERR_MAP(ERROR_WC_LOOPBACK_GET_DB_FAIL,          "vnet wildcast loopback get DB data failed.")
    VNET_ERR_MAP(ERROR_WC_LOOPBACK_SET_DB_FAIL,          "vnet wildcast loopback set DB data failed.")
    VNET_ERR_MAP(ERROR_WC_LOOPBACK_ALREADY_EXIST,        "vnet wildcast loopback has existed.")
    VNET_ERR_MAP(ERROR_WC_LOOPBACK_NAME_EMPTY,           "vnet wildcast loopback msg port name is empty.")    
    VNET_ERR_MAP(ERROR_WC_LOOPBACK_OPERCODE_INVALID,     "vnet wildcast loopback msg oper code is invalid.") 

    // vm migrate 
    VNET_ERR_MAP(ERROR_VNM_VM_MIGRATE_GET_DB_FAIL,       "vnet vm migrate get DB data failed.")
    VNET_ERR_MAP(ERROR_VNM_VM_MIGRATE_SET_DB_FAIL,       "vnet vm migrate set DB data failed.")
    VNET_ERR_MAP(ERROR_VNM_VM_MIGRATE_HAS_EXISTED,       "vnet vm migrate has existed.")
    VNET_ERR_MAP(ERROR_VNM_VM_MIGRATE_PARAM_INVALID,     "vnet vm migrate msg param is invalid.")
    VNET_ERR_MAP(ERROR_VNM_VM_MIGRATE_INPUTPARAM_DB_NOT_SAME,"vnet vm migrate msg param is not same as DB data.")
    VNET_ERR_MAP(ERROR_VNM_VM_MIGRATE_INSTANCE_IS_NULL,      "vnet vm migrate get migrate module instance failed.")    
    VNET_ERR_MAP(ERROR_VNM_VM_MIGRATE_IS_CONFLICT,       "vnet vm migrate is conflict.")    

VNET_ERR_MAP_END

    return info;
}

