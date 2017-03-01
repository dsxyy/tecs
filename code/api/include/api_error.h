#ifndef TECS_API_ERROR_CODE_H
#define TECS_API_ERROR_CODE_H


/*******************************************************************************
     错误码号段划分
*******************************************************************************/
 
#define TECS_ERR_BEGIN                  0

#define TECS_ERR_PUB_BASE               (TECS_ERR_BEGIN + 0)   //public [0,999)

#define TECS_ERR_HOST_BASE              (TECS_ERR_BEGIN + 1000) //host [1000, 1999)

#define TECS_ERR_VNET_BASE              (TECS_ERR_BEGIN + 2000) //vnet [2000, 2999)

#define TECS_ERR_CM_BASE                (TECS_ERR_BEGIN + 3000) //cluster [3000, 3999)

#define TECS_ERR_IMAGE_BASE             (TECS_ERR_BEGIN + 4000) //image   [4000, 4999)

#define TECS_ERR_PROJECT_BASE           (TECS_ERR_BEGIN + 5000) //project   [5000, 5999)

#define TECS_ERR_VMTEMPLATE_BASE        (TECS_ERR_BEGIN + 6000) //vmtemplate   [6000, 6999)

#define TECS_ERR_VMCFG_BASE             (TECS_ERR_BEGIN + 7000) //vmcfg   [7000, 7999)

#define TECS_ERR_USER_BASE              (TECS_ERR_BEGIN + 8000) //user   [8000, 8999)

#define TECS_ERR_VERSION_BASE           (TECS_ERR_BEGIN + 9000) //version   [9000, 9999)

#define TECS_ERR_FILE_BASE              (TECS_ERR_BEGIN + 10000) //FILE   [10000, 10999)

#define TECS_ERR_SNMP_BASE              (TECS_ERR_BEGIN + 11000) //SNMP   [11000, 11999)

#define TECS_ERR_VMM_BASE               (TECS_ERR_BEGIN + 12000) //SNMP   [12000, 12999)

#define TECS_ERR_STORAGE_BASE            (TECS_ERR_BEGIN + 13000) //SNMP   [13000, 13999)

//其他模块的号码段在此增加


/*******************************************************************************
  public 
*******************************************************************************/
#define TECS_SUCCESS                       (TECS_ERR_PUB_BASE + 0) // "OK."
#define TECS_ERROR                         (TECS_ERR_PUB_BASE + 1) // "Error."
#define TECS_ERR_UNKNOWN                   (TECS_ERR_PUB_BASE + 2) // "Error unknown."
#define TECS_ERR_TIMEOUT                   (TECS_ERR_PUB_BASE + 3) // "Error operation time out"

#define TECS_ERR_AUTHENTICATE_INVALID      (TECS_ERR_PUB_BASE + 4) // "Error, authentication manager is invalid"
#define TECS_ERR_AUTHENTICATE_FAILED       (TECS_ERR_PUB_BASE + 5) // "Error, failed to authenticate user"
#define TECS_ERR_AUTHORIZE_FAILED          (TECS_ERR_PUB_BASE + 6) // "Error, failed to authorize usr's operation"

#define TECS_ERR_DB_CONNECT_FAILED         (TECS_ERR_PUB_BASE + 7)  // "Error, falied to connect database"
#define TECS_ERR_DB_INSERT_FAILED          (TECS_ERR_PUB_BASE + 8)  // "Error, falied to insert database"
#define TECS_ERR_DB_UPDATE_FAILED          (TECS_ERR_PUB_BASE + 9)  // "Error, falied to update database"
#define TECS_ERR_DB_DELETE_FAILED          (TECS_ERR_PUB_BASE + 10)  // "Error, falied to delete database"
#define TECS_ERR_DB_QUERY_FAILED           (TECS_ERR_PUB_BASE + 11)  // "Error, falied to query database"
#define TECS_ERR_DB_UNKNOWN_FAILED         (TECS_ERR_PUB_BASE + 12)  // "Error, unknown database fail"

#define TECS_ERR_OBJECT_ALLOCATE_FAILED    (TECS_ERR_PUB_BASE + 13)  // "Error, failed to allocate object instance"
#define TECS_ERR_OBJECT_DROP_FAILED        (TECS_ERR_PUB_BASE + 14)  // "Error, failed to drop object instance"
#define TECS_ERR_OBJECT_GET_FAILED         (TECS_ERR_PUB_BASE + 15)  // "Error, failed to get object instance"
#define TECS_ERR_OBJECT_UPDATE_FAILED      (TECS_ERR_PUB_BASE + 16)  // "Error, failed to update object instance"
#define TECS_ERR_OBJECT_NOT_EXIST          (TECS_ERR_PUB_BASE + 17)  // "Error, object instance not exist"

#define TECS_ERR_PARA_INVALID              (TECS_ERR_PUB_BASE + 18)  // "Error, invalide parameter"
#define TECS_ERR_ACTION_FAILED             (TECS_ERR_PUB_BASE + 19)  // "Error, failed to do action"
#define TECS_ERR_SESSION_INVALID           (TECS_ERR_PUB_BASE + 20)  // "Erorr, session format is invalid"
#define TECS_ERR_DB_FAIL                   (TECS_ERR_PUB_BASE + 21)  // "Error, falied to connect database"
#define TECS_ERR_LICENSE_INVALID           (TECS_ERR_PUB_BASE + 22)  // "Error, license is invalid"
#define TECS_ERR_LICENSE_OVERLIMIT         (TECS_ERR_PUB_BASE + 23)  // "Error, license is over limit"

#define TECS_ERR_MODULE_INIT_FAILED        (TECS_ERR_PUB_BASE + 24)  // "Error, module initial failed"
#define TECS_ERR_OPRUNNING                 (TECS_ERR_PUB_BASE + 25)  // "Error, operation is running"
#define TECS_ERR_DUPLICATED_OP             (TECS_ERR_PUB_BASE + 26)  // "Error, operation is duplicated"

#define TECS_ERR_OBJECT_FORBID_OP          (TECS_ERR_PUB_BASE + 27)  // "Error, this operation is forbidden for the current object"

#define TECS_ERR_DENIED_OP                         (TECS_ERR_PUB_BASE + 28)  // "Error, this operation is denied "

/*******************************************************************************
     host manager
*******************************************************************************/
#define TECS_ERR_CLUSTER_TIMEOUT              (TECS_ERR_HOST_BASE + 2 )   // "Failed, Cluster Controller is not answered."
#define TECS_ERR_CLUSTER_NOT_REGISTER         (TECS_ERR_HOST_BASE + 3 )   // "Error, cluster have not registered into the Tecs Center."
#define TECS_ERR_HOST_NOT_EXIST               (TECS_ERR_HOST_BASE + 4 )   // "Failed, host is not exist."
//#define TECS_ERR_HOST_BUSY                  (TECS_ERR_HOST_BASE + 5 )   // "Failed, host is busy."
//#define TECS_ERR_HOST_NOT_UNREGISTERED      (TECS_ERR_HOST_BASE + 6 )   // "Failed, host is not unregistered."
//#define TECS_ERR_HOST_NOT_REGISTERED        (TECS_ERR_HOST_BASE + 7 )   // "Failed, host is not registered."
#define TECS_ERR_HOST_VMS_RUNNING             (TECS_ERR_HOST_BASE + 8 )   // "Failed, there are some VMs running on the host."
//#define TECS_ERR_HOST_CONFLICT              (TECS_ERR_HOST_BASE + 9 )   // "Conflict, host have joined the other cluster=" + c
#define TECS_ERR_HANDLE_EMPTY                 (TECS_ERR_HOST_BASE + 10)   // "Error, command is empty."
#define TECS_ERR_HANDLE_FAILED                (TECS_ERR_HOST_BASE + 11)   // "Failed, command execute failed."
#define TECS_ERR_CLUSTER_CFG_NAME_INVALID     (TECS_ERR_HOST_BASE + 22)   // "Error, cluster config name is illegal."
#define TECS_ERR_CLUSTER_CFG_SET_FAILD        (TECS_ERR_HOST_BASE + 23)   // "Failed, set cluster config failed."
#define TECS_ERR_CLUSTER_CFG_DEL_FAILD        (TECS_ERR_HOST_BASE + 24)   // "Failed, delete cluster config failed."
#define TECS_ERR_CPUINFO_ID_NOT_EXIST         (TECS_ERR_HOST_BASE + 25)   // "Error, cpu_info_id is not exist."
#define TECS_ERR_TCU_CFG_INVALID              (TECS_ERR_HOST_BASE + 26)   // "Error, cpu_info and cpu_info_id are both valid."
#define TECS_ERR_TCU_SET_FAILD                (TECS_ERR_HOST_BASE + 27)   // "Failed, set tcu config failed."
#define TECS_ERR_TCU_DEL_FAILD                (TECS_ERR_HOST_BASE + 28)   // "Failed, delete tcu config failed."
#define TECS_ERR_TCU_OPERATE_FAILD            (TECS_ERR_HOST_BASE + 29)   // "Failed, cpu is used by vm."
#define TECS_ERR_HOST_NOT_SUPPORT             (TECS_ERR_HOST_BASE + 30)   // "Failed, the host is not support eht operation"
#define TECS_ERR_CMM_ADD_FAIL                 (TECS_ERR_HOST_BASE + 47)   // "Error, cluster agent add cmm fail."
#define TECS_ERR_CMM_SET_FAIL                 (TECS_ERR_HOST_BASE + 48)   // "Error, cluster agent set cmm fail."
#define TECS_ERR_CMM_DEL_FAIL                 (TECS_ERR_HOST_BASE + 49)   // "Error, cluster agent delete cmm fail."
#define TECS_ERR_CMM_QUERY_FAIL               (TECS_ERR_HOST_BASE + 50)   // "Error, cluster agent query cmm fail."
#define TECS_ERR_TCU_VMS_RUNNING              (TECS_ERR_HOST_BASE + 51)   // "Error, set tcu num config failed because there are some VMs running."
#define TECS_ERR_CMM_IP_NULL                  (TECS_ERR_HOST_BASE + 52)   // "Error, ip of cmm not configured."
#define TECS_ERR_OPERATE_NOT_SUPPORT          (TECS_ERR_HOST_BASE + 53)   // "Error, operation not support."
#define TECS_ERR_CMM_CFG_INCORRECT            (TECS_ERR_HOST_BASE + 54)   // "Error, cmm configured incorrect."



/*******************************************************************************
     vnet 
*******************************************************************************/
//netplane 模块
#define TECS_ERROR_VNET_GET_DB_INTF_ERROR                       (TECS_ERR_VNET_BASE+1)   // "Error, the netplane get db interface failed."
#define TECS_ERROR_VNET_NETPLANE_DATA_NOT_EXIST                 (TECS_ERR_VNET_BASE+2)   // "Error, the netplane is not exsit."
#define TECS_ERROR_VNET_NETPLANE_DATA_ALREADY_EXIST             (TECS_ERR_VNET_BASE+3)   // "Error, the netplane has already exsit."
#define TECS_ERROR_VNET_NETPLANE_CFG_PARAM_ERROR                (TECS_ERR_VNET_BASE+4)   // "Error, the netplane para is invalid."
// Logic network 模块
#define TECS_ERROR_VNET_LOGIC_NETWORK_DATA_NOT_EXIST            (TECS_ERR_VNET_BASE+5)   // "Error, the logicnet is not exsit."
#define TECS_ERROR_VNET_LOGIC_NETWORK_DATA_ALREADY_EXIST        (TECS_ERR_VNET_BASE+6)   // "Error, the logicnet has alredy exsit."
#define TECS_ERROR_VNET_LOGIC_NETWORK_CFG_PARAM_ERROR           (TECS_ERR_VNET_BASE+7)   // "Error, the logicnet para is invalid."
// IP Resource Pool 模块;
#define TECS_ERROR_VNET_IPV4_RANGE_CFG_PARAM_ERROR              (TECS_ERR_VNET_BASE+8)   // "Error, the ipv4 range para is invalid."
#define TECS_ERROR_VNET_IPV4_RANGE_CALC_ERROR                   (TECS_ERR_VNET_BASE+9)   // "Error, the ipv4 range calc  invalid."
#define TECS_ERROR_VNET_IPV4_RANGE_NOT_EXIST                    (TECS_ERR_VNET_BASE+10)  // "Error, the ipv4 range is not exsit."
#define TECS_ERROR_VNET_IPV4_RANGE_NOT_SAME_SUBNET              (TECS_ERR_VNET_BASE+11)  // "Error, the ipv4 range is not in same subnet."
#define TECS_ERROR_VNET_IPV4_RANGE_INVALID                      (TECS_ERR_VNET_BASE+12)  // "Error, the ipv4 range is invalid."
#define TECS_ERROR_VNET_IPV4_RANGE_OCCUPIED                     (TECS_ERR_VNET_BASE+13)  // "Error, the ipv4 range is occupied."
// MAC Resource Pool 模块;
#define TECS_ERROR_VNET_MAC_RANGE_CFG_PARAM_ERROR               (TECS_ERR_VNET_BASE+15)  // "Error, the mac range para is invalid."
#define TECS_ERROR_VNET_MAC_RANGE_CALC_ERROR                    (TECS_ERR_VNET_BASE+16)  // "Error, the mac range calc  invalid."
#define TECS_ERROR_VNET_MAC_RANGE_NOT_EXIST                     (TECS_ERR_VNET_BASE+17)  // "Error, the mac range is not exsit."
#define TECS_ERROR_VNET_MAC_RANGE_INVALID                       (TECS_ERR_VNET_BASE+18)  // "Error, the mac range is invalid."
#define TECS_ERROR_VNET_MAC_OUI_INVALID                         (TECS_ERR_VNET_BASE+19)  // "Error, the mac range oui invalid."
#define TECS_ERROR_VNET_MAC_PARSE_ERROR                         (TECS_ERR_VNET_BASE+20)  // "Error, the mac parse error."
#define TECS_ERROR_VNET_MAC_RANGE_OCCUPIED                      (TECS_ERR_VNET_BASE+21)  // "Error, the mac range is occupied."
// VLAN Resource Pool 模块;
#define TECS_ERROR_VNET_VLAN_RANGE_CFG_PARAM_ERROR              (TECS_ERR_VNET_BASE+22)  // "Error, the vlan range para is invalid."
#define TECS_ERROR_VNET_VLAN_RANGE_CALC_ERROR                   (TECS_ERR_VNET_BASE+23)  // "Error, the vlan range calc error."
#define TECS_ERROR_VNET_VLAN_RANGE_NOT_EXIST                    (TECS_ERR_VNET_BASE+24)  // "Error, the vlan range is not exsit."
#define TECS_ERROR_VNET_VLAN_RANGE_OCCUPIED                     (TECS_ERR_VNET_BASE+25)  // "Error, the vlan range is occupied."
// SEGMENT Resource Pool 模块;
#define TECS_ERROR_VNET_SEGMENT_RANGE_CFG_PARAM_ERROR           (TECS_ERR_VNET_BASE+26)  // "Error, the segment range para is invalid."
#define TECS_ERROR_VNET_SEGMENT_RANGE_CALC_ERROR                (TECS_ERR_VNET_BASE+27)  // "Error, the segment range calc error."
#define TECS_ERROR_VNET_SEGMENT_RANGE_NOT_EXIST                 (TECS_ERR_VNET_BASE+28)  // "Error, the segment range is not exsit."
#define TECS_ERROR_VNET_SEGMENT_RANGE_OCCUPIED                  (TECS_ERR_VNET_BASE+29)  // "Error, the segment range is occupied."

// Port Group 模块
#define VNET_ERROR_PORTGROUP_BEGIN                              (TECS_ERR_VNET_BASE+160) 
#define TECS_ERROR_VNET_ERROR_PORTGROUP_OPERCODE_INVALID        (VNET_ERROR_PORTGROUP_BEGIN+1)  // "Error, the opercode of portgroup is invalid."
#define TECS_ERROR_VNET_ERROR_PORTGROUP_PG_INVALID              (VNET_ERROR_PORTGROUP_BEGIN+2)  // "Error, the portgroup is empty or invalid." 
#define TECS_ERROR_VNET_ERROR_PORTGROUP_INPARAM_INVALID       (VNET_ERROR_PORTGROUP_BEGIN+3)  // "Error, Input the invalid params of portgroup."
#define TECS_ERROR_VNET_ERROR_PORTGROUP_GET_INSTANCE_FAIL     (VNET_ERROR_PORTGROUP_BEGIN+4)  // "Error, Get the handler fail."
#define TECS_ERROR_VNET_ERROR_PORTGROUP_GET_VLANRANGE_FAIL    (VNET_ERROR_PORTGROUP_BEGIN+5)  // "Error, Get portgroup's trunk vlanrange fail."
#define TECS_ERROR_VNET_ERROR_PORTGROUP_NOT_EXIST               (VNET_ERROR_PORTGROUP_BEGIN+6)  // "Error, the portgroup is not exsit."
#define TECS_ERROR_VNET_ERROR_PORTGROUP_IS_EXIST                (VNET_ERROR_PORTGROUP_BEGIN+7)  // "Error, the portgroup is exsit already."
#define TECS_ERROR_VNET_ERROR_PORTGROUP_IS_INUSING              (VNET_ERROR_PORTGROUP_BEGIN+8)  // "Error, the portgroup is in using."
#define TECS_ERROR_VNET_ERROR_PORTGROUP_OPER_DB_FAIL            (VNET_ERROR_PORTGROUP_BEGIN+9)  // "Error, operator db of portgroup fail."
#define TECS_ERROR_VNET_ERROR_PORTGROUP_OPER_DEFAULT_PG_FAIL   (VNET_ERROR_PORTGROUP_BEGIN+10) // "Error, the portgroup is default, cannot delete."  
#define TECS_ERROR_VNET_ERROR_PORTGROUP_ACCESS_VLAN_INVALID     (VNET_ERROR_PORTGROUP_BEGIN+11)  // "Error, the access vlan is in netplane's vlanrange."             
#define TECS_ERROR_VNET_ERROR_PORTGROUP_NATIVE_VLAN_INVALID     (VNET_ERROR_PORTGROUP_BEGIN+12) // "Error, the native vlan is not in trunk's vlanrange."        
#define TECS_ERROR_VNET_ERROR_PORTGROUP_PRIVATE_VLAN_INVALID   (VNET_ERROR_PORTGROUP_BEGIN+13)  // "Error, netplane's vlanrange is empty, so cannot config isolate vlan."

#define TECS_ERROR_VNET_ERROR_PG_VLANRANGE_INVLAID              (VNET_ERROR_PORTGROUP_BEGIN+14) // "Error, the portgroup vlan range is invalid."
#define TECS_ERROR_VNET_ERROR_PG_VLANRANGE_IS_EXIST             (VNET_ERROR_PORTGROUP_BEGIN+15) // "Error, the portgroup vlan range is exist."
#define TECS_ERROR_VNET_ERROR_PG_VLANRANGE_NOT_EXIST            (VNET_ERROR_PORTGROUP_BEGIN+16) // "Error, the portgroup vlan range is not exist."
#define TECS_ERROR_VNET_ERROR_PG_VLANRANGE_OPER_DB_FAIL         (VNET_ERROR_PORTGROUP_BEGIN+17) // "Error, operator db of trunk's vlanrange fail."
#define TECS_ERROR_VNET_ERROR_PG_VLANRANGE_CALC_ERROR           (VNET_ERROR_PORTGROUP_BEGIN+18) // "Error, Combining the calculation of vlanrange fail."
#define TECS_ERROR_VNET_ERROR_PG_VLANRANGE_IS_CONFLICT          (VNET_ERROR_PORTGROUP_BEGIN+19) // "Error, the trunk's vlanrange is conflict witch netplane's or nativevlan"
#define TECS_ERROR_VNET_ERROR_PORTGROUP_SEGMENT_INVALID         (VNET_ERROR_PORTGROUP_BEGIN+20)  // "Error, the segment is in netplane's vlanrange."             
#define TECS_ERROR_VNET_ERROR_PORTGROUP_PRIVATE_SEGMENT_INVALID (VNET_ERROR_PORTGROUP_BEGIN+21)  // "Error, netplane's segmentrange is empty, so cannot config isolate segment."

//switch 模块
#define VNET_ERROR_SWITCH_BEGIN                                 (TECS_ERR_VNET_BASE+200)     
#define TECS_ERROR_VNET_ERROR_SWITCH_OPERCODE_INVALID           (VNET_ERROR_SWITCH_BEGIN+1)  // "Error, the switch opercode is invalid."
#define TECS_ERROR_VNET_ERROR_SWITCH_INSTANCE_FAIL              (VNET_ERROR_SWITCH_BEGIN+2)  // "Error, the switch get instance failed."  
#define TECS_ERROR_VNET_ERROR_SWITCH_GET_PORTMGR_FAIL          (VNET_ERROR_SWITCH_BEGIN+3) // "Error, the switch get portmgr instance failed."  
#define TECS_ERROR_VNET_ERROR_SWITCH_GET_VSWITCH_DB_FAIL       (VNET_ERROR_SWITCH_BEGIN+4)  // "Error, the switch get vswitch db instance failed."  
#define TECS_ERROR_VNET_ERROR_SWITCH_GET_VSWITCHMAP_DB_FAIL    (VNET_ERROR_SWITCH_BEGIN+5)  // "Error, the switch get vswitchmap db instance failed."  
#define TECS_ERROR_VNET_ERROR_SWTICH_CONFLICT_WITH_WILDCAST    (VNET_ERROR_SWITCH_BEGIN+6)  // "Error, the switch is conflict.
#define TECS_ERROR_VNET_ERROR_SWTICH_PORT_INVALID              (VNET_ERROR_SWITCH_BEGIN+7)  // "Error, the switch port is invalid.
#define TECS_ERROR_VNET_ERROR_SWTICH_GET_PGINFO_FAIL           (VNET_ERROR_SWITCH_BEGIN+8) // "Error, the switch get portgroup info failed.
#define TECS_ERROR_VNET_ERROR_SWITCH_NAME_INVALID              (VNET_ERROR_SWITCH_BEGIN+9) // "Error, the switch name is invalid." 
#define TECS_ERROR_VNET_ERROR_SWITCH_ALREADY_EXIST             (VNET_ERROR_SWITCH_BEGIN+10)  // "Error, the switch is already exsit."
#define TECS_ERROR_VNET_ERROR_SWITCH_IS_USING                   (VNET_ERROR_SWITCH_BEGIN+11) // "Error, the switch port is isused."
#define TECS_ERROR_VNET_ERROR_SWITCH_OPER_DB_FAIL              (VNET_ERROR_SWITCH_BEGIN+12)  // "Error, the switch operator db fail."
#define TECS_ERROR_VNET_ERROR_SWITCH_PORT_OPER_DB_FAIL         (VNET_ERROR_SWITCH_BEGIN+13)  // "Error, the switch port operator db fail."
#define TECS_ERROR_VNET_ERROR_SWITCH_DEL_NOT_FINISH            (VNET_ERROR_SWITCH_BEGIN+14) // "Error, the switch delete not finish."
#define TECS_ERROR_VNET_ERROR_SWITCH_GET_PORTINFO_FAIL         (VNET_ERROR_SWITCH_BEGIN+15) // "Error, the switch get uuid failed."
#define TECS_ERROR_VNET_ERROR_SWITCH_ADD_BOND_FAIL             (VNET_ERROR_SWITCH_BEGIN+16)  // "Error, the switch add bond failed." 
#define TECS_ERROR_VNET_ERROR_SWITCH_DEL_BOND_FAIL             (VNET_ERROR_SWITCH_BEGIN+17)  // "Error, the switch del bond failed.
#define TECS_ERROR_VNET_ERROR_SWITCH_DEL_BONDSLAVE_FAIL        (VNET_ERROR_SWITCH_BEGIN+18) // "Error, the switch del bond slave failed." 
#define TECS_ERROR_VNET_ERROR_SWITCH_GET_BONDSLAVE_FAIL        (VNET_ERROR_SWITCH_BEGIN+19)  // "Error, the switch del bond slave failed." 
#define TECS_ERROR_VNET_ERROR_SWITCH_ADD_BRIDGE_FAIL           (VNET_ERROR_SWITCH_BEGIN+20)  // "Error, the switch add bridge failed."  
#define TECS_ERROR_VNET_ERROR_SWITCH_DEL_BRIDGE_FAIL           (VNET_ERROR_SWITCH_BEGIN+21) // "Error, the switch del bridge failed." 
#define TECS_ERROR_VNET_ERROR_SWITCH_SET_ATTR_FAIL             (VNET_ERROR_SWITCH_BEGIN+22) // "Error, the switch set attr failed." 
#define TECS_ERROR_VNET_ERROR_SWITCH_PORT_ISCONFLICTT          (VNET_ERROR_SWITCH_BEGIN+23)  // "Error, the switchport is conflict." 
#define TECS_ERROR_VNET_ERROR_SWITCH_PORT_ISMIXED              (VNET_ERROR_SWITCH_BEGIN+24)   // "Error, the switchport is mixed." 
#define TECS_ERROR_VNET_ERROR_SWITCH_OPER_KERNEL_DVS_FAIL      (VNET_ERROR_SWITCH_BEGIN+25)  // "Error, the switch operator kernelDVS fail." 
#define TECS_ERROR_VNET_ERROR_SWITCH_QUERY_VNAUUID_FAIL        (VNET_ERROR_SWITCH_BEGIN+26)   // "Error, the switch query vnauuid fail." 
#define TECS_ERROR_VNET_ERROR_SWITCH_MERGE_REQINFO_FAIL        (VNET_ERROR_SWITCH_BEGIN+27)  // "Error, the switch merge requestinfo fail." 
#define TECS_ERROR_VNET_ERROR_SWITCH_GET_SRIOV_VFS_FAIL        (VNET_ERROR_SWITCH_BEGIN+28)  // "Error, the switch get sriov vf resource fail." 
#define TECS_ERROR_VNET_ERROR_SWITCH_ADD_UPLINK_FAIL           (VNET_ERROR_SWITCH_BEGIN+29)  // "Error, add switch's uplink port fail." 
#define TECS_ERROR_VNET_ERROR_SWITCH_ADD_EVBPORT_FAIL          (VNET_ERROR_SWITCH_BEGIN+30)   // "Error, add switch's evb port fail." 
#define TECS_ERROR_VNET_ERROR_SWITCH_DEL_EVBPORT_FAIL          (VNET_ERROR_SWITCH_BEGIN+31)  // "Error, delete switch's evb port fail." 
#define TECS_ERROR_VNET_ERROR_SWITCH_RENAME_BRIDGE_FAIL        (VNET_ERROR_SWITCH_BEGIN+32)   // "Error, rename bridge fail." 
#define TECS_ERROR_VNET_ERROR_SWITCH_CHECK_PARAM_FAIL          (VNET_ERROR_SWITCH_BEGIN+33)   // "Error, the parameters check fail." 
#define TECS_ERROR_VNET_ERROR_SWITCH_TIMER_CONFIG_FAIL         (VNET_ERROR_SWITCH_BEGIN+34)   // "Error, timer for checking create fail."
#define TECS_ERROR_VNET_ERROR_SWITCH_GET_VNAINFO_FAIL          (VNET_ERROR_SWITCH_BEGIN+35)   // "Error, get all hosts in the switch fail."
#define TECS_ERROR_VNET_ERROR_SWITCH_MODIFY_FAIL               (VNET_ERROR_SWITCH_BEGIN+36)   // "Error, modify switch attribute fail."
#define TECS_ERROR_VNET_ERROR_SWITCH_ADD_VTEP_FAIL             (VNET_ERROR_SWITCH_BEGIN+37)   // "Error, add vtep port fail."
#define TECS_ERROR_VNET_ERROR_SWITCH_DEL_VTEP_FAIL            (VNET_ERROR_SWITCH_BEGIN+38)   // "Error, delete vtep port fail."
#define TECS_ERROR_VNET_ERROR_SWITCH_QUERY_VTEP_FAIL          (VNET_ERROR_SWITCH_BEGIN+39)   // "Error, query vtep port infomation fail."


#define TECS_ERROR_VNET_NOT_WORKING                             (TECS_ERR_VNET_BASE+60)  // "Error, the vnet lib is not working."

//wildcast switch 
#define TECS_ERROR_VNET_WC_SWITCH_OPERCODE_INVALID              (TECS_ERR_VNET_BASE+61)
#define TECS_ERROR_VNET_WC_SWITCH_INSTANCE_FAIL                 (TECS_ERR_VNET_BASE+62)
#define TECS_ERROR_VNET_WC_SWITCH_NAME_EMPTY                    (TECS_ERR_VNET_BASE+63)
#define TECS_ERROR_VNET_WC_SWITCH_PG_EMPTY                      (TECS_ERR_VNET_BASE+64)
#define TECS_ERROR_VNET_WC_SWITCH_PGTYPE_INVALID                (TECS_ERR_VNET_BASE+65)
#define TECS_ERROR_VNET_WC_SWITCH_EVBMODE_INVALID               (TECS_ERR_VNET_BASE+66)
#define TECS_ERROR_VNET_WC_SWITCH_ALREADY_EXIST                 (TECS_ERR_VNET_BASE+67)
#define TECS_ERROR_VNET_WC_SWITCH_GET_DB_FAIL                   (TECS_ERR_VNET_BASE+68)
#define TECS_ERROR_VNET_WC_SWITCH_SET_DB_FAIL                   (TECS_ERR_VNET_BASE+69)
#define TECS_ERROR_VNET_WC_SWITCH_NOTEXIST_PORT                 (TECS_ERR_VNET_BASE+70)
#define TECS_ERROR_VNET_WC_SWITCH_PARAM_INVALID                 (TECS_ERR_VNET_BASE+71)

//wildcast create vport
#define TECS_ERROR_VNET_WC_CREATE_VPORT_GET_DB_FAIL             (TECS_ERR_VNET_BASE+72)
#define TECS_ERROR_VNET_WC_CREATE_VPORT_SET_DB_FAIL             (TECS_ERR_VNET_BASE+73)
#define TECS_ERROR_VNET_WC_CREATE_VPORT_ALREADY_EXIST           (TECS_ERR_VNET_BASE+74)
#define TECS_ERROR_VNET_WC_CREATE_VPORT_NAME_EMPTY              (TECS_ERR_VNET_BASE+75)
#define TECS_ERROR_VNET_WC_CREATE_VPORT_OPERCODE_INVALID        (TECS_ERR_VNET_BASE+76)
#define TECS_ERROR_VNET_WC_GET_PORT_INSTANCE_NULL               (TECS_ERR_VNET_BASE+77)
#define TECS_ERROR_VNET_WC_GET_SWTICH_INSTANCE_NULL             (TECS_ERR_VNET_BASE+78)

//wildcast loopback
#define TECS_ERROR_VNET_WC_LOOPBACK_GET_DB_FAIL                 (TECS_ERR_VNET_BASE+79)
#define TECS_ERROR_VNET_WC_LOOPBACK_SET_DB_FAIL                 (TECS_ERR_VNET_BASE+80)
#define TECS_ERROR_VNET_WC_LOOPBACK_ALREADY_EXIST               (TECS_ERR_VNET_BASE+81)
#define TECS_ERROR_VNET_WC_LOOPBACK_NAME_EMPTY                  (TECS_ERR_VNET_BASE+82)
#define TECS_ERROR_VNET_WC_LOOPBACK_OPERCODE_INVALID            (TECS_ERR_VNET_BASE+83)

//vm migrate 
#define TECS_ERROR_VNET_VM_MIGRATE_GET_DB_FAIL                  (TECS_ERR_VNET_BASE+84)
#define TECS_ERROR_VNET_VM_MIGRATE_SET_DB_FAIL                  (TECS_ERR_VNET_BASE+85)
#define TECS_ERROR_VNET_VM_MIGRATE_HAS_EXISTED                  (TECS_ERR_VNET_BASE+86)
#define TECS_ERROR_VNET_VM_MIGRATE_PARAM_INVALID                (TECS_ERR_VNET_BASE+87)
#define TECS_ERROR_VNET_VM_MIGRATE_INPUTPARAM_DB_NOT_SAME       (TECS_ERR_VNET_BASE+88)
#define TECS_ERROR_VNET_VM_MIGRATE_INSTANCE_IS_NULL             (TECS_ERR_VNET_BASE+89)
#define TECS_ERROR_VNET_VM_MIGRATE_IS_CONFLICT                  (TECS_ERR_VNET_BASE+90)


#define TECS_ERROR_VNET_WC_UPLINKPG_MTU_LESS_KNPG_MTU           (TECS_ERR_VNET_BASE+91)
#define TECS_ERROR_VNET_WC_SWITCH_UPLINKPORT_IS_CONFLICT        (TECS_ERR_VNET_BASE+92)
#define TECS_ERROR_VNET_WC_SWITCH_MIX_UPLINKPG_MUST_SAME        (TECS_ERR_VNET_BASE+93)
#define TECS_ERROR_VNET_WC_SWITCH_MIX_UPLINK_BONDMODE_MUST_SAME (TECS_ERR_VNET_BASE+94)
#define TECS_ERROR_VNET_WC_SWITCH_MIX_EVBMODE_MUST_SAME         (TECS_ERR_VNET_BASE+95)

#define TECS_ERROR_VNET_VNA_AGENT_MODULE_OFFLINE                (TECS_ERR_VNET_BASE+100)
#define TECS_ERROR_VNET_VNA_AGENT_MODULE_NOT_REG                (TECS_ERR_VNET_BASE+101)
#define TECS_ERROR_VNET_VNA_DELETE_FAILED                       (TECS_ERR_VNET_BASE+102)
#define TECS_ERROR_VNET_VNA_DELETE_DB_OPER_FAILED               (TECS_ERR_VNET_BASE+103)
#define TECS_ERROR_VNET_VNA_AGENT_MODULE_ONLINE                 (TECS_ERR_VNET_BASE+104)


//port config
#define VNET_ERROR_PORT_BEGIN                                   (TECS_ERR_VNET_BASE+400)     
#define TECS_ERROR_VNET_PORT_NAME_EMPTY                         (VNET_ERROR_PORT_BEGIN+1)
#define TECS_ERROR_VNET_PORT_OPERCODE_INVALID                   (VNET_ERROR_PORT_BEGIN+2)
#define TECS_ERROR_VNET_PORT_PARA_INVALID                       (VNET_ERROR_PORT_BEGIN+3)
#define TECS_ERROR_VNET_PORT_OPER_FAIL                          (VNET_ERROR_PORT_BEGIN+4)
#define TECS_ERROR_VNET_PORT_OPERDB_FAIL                        (VNET_ERROR_PORT_BEGIN+5)
#define TECS_ERROR_VNET_PORT_WC_CONFLICT                        (VNET_ERROR_PORT_BEGIN+6)
#define TECS_ERROR_VNET_PORT_CHECKDB_CONFLICT                   (VNET_ERROR_PORT_BEGIN+7)


/*******************************************************************************
     cluster manager
*******************************************************************************/
#define TECS_ERROR_CM_REQ_TIMEOUT                   (TECS_ERR_TIMEOUT)       // 超时
#define TECS_ERR_CM_CREATE_FAIL                     (TECS_ERR_CM_BASE + 1)   // "Failed, 创建集群失败."
#define TECS_ERR_CM_CLUSTER_REGISTERD               (TECS_ERR_CM_BASE + 2)   // "Failed, 集群已注册."
#define TECS_ERR_CM_CLUSTER_NOT_EXIST               (TECS_ERR_CM_BASE + 3)   // "Failed, 集群不存在."
//#define TECS_ERR_CM_CLUSTER_NOT_REGISTER            (TECS_ERR_CM_BASE + 4)   // "Failed, 集群未注册."
#define TECS_ERR_CM_CLUSTER_NOT_FREE                (TECS_ERR_CM_BASE + 5)   // "Failed, 集群不是空闲的."


/*******************************************************************************
     image
*******************************************************************************/
#define TECS_ERR_IMAGE_FILE_NOT_EXIST               (TECS_ERR_IMAGE_BASE+0) // "Failed, file not exist"
#define TECS_ERR_IMAGE_HAVE_REGISTERED              (TECS_ERR_IMAGE_BASE+1) // "Failed, image have already registered"
#define TECS_ERR_IMAGE_IS_USING                     (TECS_ERR_IMAGE_BASE+2) // "Failed, image is using"
#define TECS_ERR_IMAGE_INVALID_QUERY_TYPE           (TECS_ERR_IMAGE_BASE+3) // "Failed, invalid query type"
#define TECS_ERR_IMAGE_QUERY_FAILED                 (TECS_ERR_IMAGE_BASE+4) // "Failed to query "
#define TECS_ERR_IMAGE_OP_RUNNING                   (TECS_ERR_IMAGE_BASE+5) //"operation running"
#define TECS_ERR_IMAGE_IS_DEPLOYED                  (TECS_ERR_IMAGE_BASE+6) // "Failed, image is deployed"


/*******************************************************************************
     project 
*******************************************************************************/
#define TECS_ERR_PROJECT_NAME_CONFLICT       (TECS_ERR_PROJECT_BASE + 0) //"Failed, project name conflict"
#define TECS_ERR_PROJECT_VM_RUNNING          (TECS_ERR_PROJECT_BASE + 1) //"Failed, project contain virtual machine"
#define TECS_ERR_PROJECT_INVALID_QUERY_TYPE  (TECS_ERR_PROJECT_BASE + 2) //"Failed, invalid query type"
#define TECS_ERR_PROJECT_INVALID_ACTION_TYPE (TECS_ERR_PROJECT_BASE + 3) // "Failed, invalid action type"

/*******************************************************************************
     vmtemplate 
*******************************************************************************/
#define TECS_ERR_VMTEMPLATE_INVALID_QUERY_TYPE    (TECS_ERR_VMTEMPLATE_BASE + 0)  // "Failed, invalid query type"
#define TECS_ERR_VMTEMPLATE_ACQUIRE_IMAGE_FAILED  (TECS_ERR_VMTEMPLATE_BASE + 1) //"Failed to acquire image"
#define TECS_ERR_VMTEMPLATE_NAME_CONFLICT         (TECS_ERR_VMTEMPLATE_BASE + 2)//"Failed, vmtemplate name conflict"
#define TECS_ERR_VMTEMPLATE_IMAGE_NOT_PUBLIC      (TECS_ERR_VMTEMPLATE_BASE + 3) //"Failed to publish vmtemplate because image is not public"

/*******************************************************************************
     vmcfg  
*******************************************************************************/
#define TECS_ERR_VMCFG_ACQUIRE_IMAGE_FAILED       (TECS_ERR_VMCFG_BASE + 0) //"Failed to acquire image"
#define TECS_ERR_VMCFG_INVALID_PROJECT            (TECS_ERR_VMCFG_BASE + 1) //"Failed, invalid project"
#define TECS_ERR_VMCFG_ACQUIRE_VMTEMPLATE_FAILED  (TECS_ERR_VMCFG_BASE + 2) //"Failed to acquire vmtemplate"
#define TECS_ERR_VMCFG_STATE_FORBIDDEN            (TECS_ERR_VMCFG_BASE + 3) //"Failed, the operation is forbidden in current state"
#define TECS_ERR_VMCFG_NO_SUITABLE_CLUSTER        (TECS_ERR_VMCFG_BASE + 4) //"Failed, no cluster is suitable to the vm"
#define TECS_ERR_VMCFG_ACTION_FAILED_IN_CC        (TECS_ERR_VMCFG_BASE + 5) //"Failed to action vm in cluster"
#define TECS_ERR_VMCFG_ACTION_FAILED_IN_HC        (TECS_ERR_VMCFG_BASE + 6) //"Failed to action vm in host"
#define TECS_ERR_VMCFG_INVALID_QUERY_TYPE         (TECS_ERR_VMCFG_BASE + 7) // "Failed, invalid query type"
#define TECS_ERR_VMCFG_INVALID_ACTION_TYPE        (TECS_ERR_VMCFG_BASE + 8) // "Failed, invalid action type"
#define TECS_ERR_VMCFG_MODIFY_FORBIDDEN           (TECS_ERR_VMCFG_BASE + 9) //"Failed, modify is forbidden for VM has deployed"
#define TECS_ERR_VMCFG_ALLOCATE_MAC               (TECS_ERR_VMCFG_BASE + 10)//"Failed to allocate mac address"
#define TECS_ERR_VMCFG_RELEASE_MAC                (TECS_ERR_VMCFG_BASE + 11)//"Failed to release mac address"
#define TECS_ERR_VMCFG_NO_VM                      (TECS_ERR_VMCFG_BASE + 12) //"no vm"
#define TECS_ERR_VMCFG_DEPLOY_IMAGE_FAILED        (TECS_ERR_VMCFG_BASE + 13)//"Failed to deploy vm images"
#define TECS_ERR_VMCFG_IMAGE_DEPLOYING            (TECS_ERR_VMCFG_BASE + 14)//"Failed, the images of the VM are deploying"

#define TECS_ERR_VMCFG_LACK_COMPUTE               (TECS_ERR_VMCFG_BASE + 15)//"Failed, no enough compute resource"
#define TECS_ERR_VMCFG_LACK_SHARE_STORAGE         (TECS_ERR_VMCFG_BASE + 16)//"Failed, no enough share storage resource"
#define TECS_ERR_VMCFG_LACK_NETWORK               (TECS_ERR_VMCFG_BASE + 17)//"Failed, no enough network resource"
#define TECS_ERR_VMCFG_GET_COREDUMP_URL           (TECS_ERR_VMCFG_BASE + 18)//"Failed to get VM coredump url"
#define TECS_ERR_VMCFG_PREPARE_DEPLOY             (TECS_ERR_VMCFG_BASE + 19)//"Failed to generate deploy msg"
#define TECS_ERR_VMCFG_ALLOCATE_NETWORK_TC        (TECS_ERR_VMCFG_BASE + 20)//"Failed, allocate network resource failed in TC"
#define TECS_ERR_VMCFG_DEL_IMGBACKUP_ACTIVED      (TECS_ERR_VMCFG_BASE + 21)//"Failed, image backup has been actived, forbidden to delete"

/*******************************************************************************
     User 
*******************************************************************************/
#define TECS_ERR_USER_DUPLICATE                    (TECS_ERR_USER_BASE + 0)   //Conflict, user already exist.
#define TECS_ERR_USERGROUP_DUPLICATE               (TECS_ERR_USER_BASE + 1)   //Conflict, usergroup already exist.
#define TECS_ERR_USER_NOT_EXIST                    (TECS_ERR_USER_BASE + 2)   //Error, user does not exist.
#define TECS_ERR_USERGROUP_NOT_EXIST               (TECS_ERR_USER_BASE + 3)   //Error, usergroup does not exist.
#define TECS_ERR_PASSWORD_NOT_SAME                 (TECS_ERR_USER_BASE + 4)   //Error, confirm password and password are inconsistent.
#define TECS_ERR_CANNOT_DEL_OR_DISABLE_DEFAULT     (TECS_ERR_USER_BASE + 5)   //Error, the default user cannot be disabled or deleted.
#define TECS_ERR_CANNOT_DEL_OR_DISABLE_SELF        (TECS_ERR_USER_BASE + 6)   //Error, user cannot disable or delete his own account.
#define TECS_ERR_DEFAULT_MUST_BE_CLOUDADMIN        (TECS_ERR_USER_BASE + 7)   //Error, the default user must be a cloud administrator.
#define TECS_ERR_USERGROUP_HAS_USER                (TECS_ERR_USER_BASE + 8)   //Error, There are users in the usergroup.
#define TECS_ERR_USER_HAS_RUNNING_VM               (TECS_ERR_USER_BASE + 9)   //Error, The user has deployed vm.
#define TECS_ERR_USER_HAS_USED_IAMGE               (TECS_ERR_USER_BASE + 10)   //Error, The user has used image.


/*******************************************************************************
     other module error code  
*******************************************************************************/
#define TECS_ERR_VERSION_CANCEL_OPERATION           (TECS_ERR_VERSION_BASE +1)  //"cancel operation";
#define TECS_ERR_VERSION_NO_UPGRADE_PACKAGE         (TECS_ERR_VERSION_BASE +2)  //"there is no upgraded package"
#define TECS_ERR_VERSION_NO_INSTALLED_PACKAGE       (TECS_ERR_VERSION_BASE +3)  //"there is no installed package"
#define TECS_ERR_VERSION_START_OPERATION_FAIL       (TECS_ERR_VERSION_BASE +4)  //"fail to start operation"
#define TECS_ERR_VERSION_OPERATION_ABNORMAL_EXIT    (TECS_ERR_VERSION_BASE +5)  //the operation is abnormal

/*******************************************************************************
    FILE  
*******************************************************************************/
#define TECS_ERR_FILE_USER_NAME_NOT_EXIST                     (TECS_ERR_FILE_BASE +1)
#define TECS_ERR_FILE_QUERY_TYPE_ERR                          (TECS_ERR_FILE_BASE +2)


/*******************************************************************************
    VM_MANAGER  
*******************************************************************************/
#define TECS_ERR_VMM_NO_VM                                  (TECS_ERR_VMM_BASE + 0)

/*******************************************************************************
    SA_MANAGER  
*******************************************************************************/
#define TECS_ERR_SC_TIMEOUT            (TECS_ERR_STORAGE_BASE + 1 )   // "Failed, sa Controller is not answered."
#define TECS_ERR_SA_NOT_EXIST          (TECS_ERR_STORAGE_BASE + 2 )   // "Failed, sa not exist"
#define TECS_ERR_SC_DB_UPDATE_FAIL     (TECS_ERR_STORAGE_BASE + 3 )   // "Failed, update sc db err"
#define TECS_ERR_SC_DB_DELETE_FAIL     (TECS_ERR_STORAGE_BASE + 4 )   // "Failed, delete sc db err"
#define TECS_ERR_SC_DB_INSERT_FAIL     (TECS_ERR_STORAGE_BASE + 5 )   // "Failed, insert sc db err"
#define TECS_ERR_SC_MAP_EXIST          (TECS_ERR_STORAGE_BASE + 6 )   // "Failed, map already exist"
#define TECS_ERR_SC_MAP_NOT_EXIST      (TECS_ERR_STORAGE_BASE + 7 )   // "Failed, map not exist"
#define TECS_ERR_SA_BUSY               (TECS_ERR_STORAGE_BASE + 8 )   // "Failed, map not exist"

#endif // TECS_API_ERROR_CODE_H

