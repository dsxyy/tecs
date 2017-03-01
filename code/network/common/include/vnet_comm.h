/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：vnet_comm.h
* 文件标识：
* 内容摘要：vnet_comm的定义文件
* 当前版本：1.0
* 作    者：
* 完成日期：2011年8月16日
*
* 修改记录1：
*     修改日期：2011/8/16
*     版 本 号：V1.0
*     修 改 人：
*     修改内容：创建
******************************************************************************/

#if !defined(VNET_COMM__INCLULDE_H_)
#define VNET_COMM__INCLULDE_H_
#include "timer.h"
#include "datetime.h"
#include "clock.h"
#include "sky.h"
#include "event.h"
#include "mid.h"
#include "postgresql_db.h"
#include "sqlite_db.h"
#include "object_sql.h"
#include "stdlib.h"
#include "tecs_errcode.h"
#include "log_agent.h"
#include <iostream>
#include <unistd.h> 
#include <sys/ioctl.h> 
#include <net/if.h> 
#include <arpa/inet.h> 
#include <linux/netlink.h> 
#include <linux/rtnetlink.h> 
#include "sys.h"
#include "tecs_pub.h"
#include <string.h>
#if 0
typedef unsigned long   unsigned32;
typedef unsigned short  unsigned16;
typedef unsigned char   unsigned8;
typedef unsigned char   byte;

#undef uuid_t
typedef struct {
    unsigned32  time_low;
    unsigned16  time_mid;
    unsigned16  time_hi_and_version;
    unsigned8   clock_seq_hi_and_reserved;
    unsigned8   clock_seq_low;
    byte        node[6];
} uuid_t;
#endif
//////////////////////////////////////////////////////////////////////////
//                           VM 网络操作宏定义
//////////////////////////////////////////////////////////////////////////
#define VNET_VM_DEPLOY                      (0)  /* 普通部署 */
#define VNET_VM_PREPARE                     (1)  /* 预部署 */
#define VNET_VM_MIGRATE                     (2)  /* 迁移 */
#define VNET_VM_SUSPEND                     (3)  /* 暂不用，保留 */
#define VNET_VM_CANCEL                      (4)  /* 撤销 */

//////////////////////////////////////////////////////////////////////////
//                           VM NET 操作结果
//////////////////////////////////////////////////////////////////////////
#define VNET_VM_SUCCESS                     (1)
#define VNET_VM_FAILED                      (0)

//////////////////////////////////////////////////////////////////////////
//                           VM MIGRATE 操作结果
//////////////////////////////////////////////////////////////////////////
#define VNET_VM_MIGRATE_INIT                     (0)
#define VNET_VM_MIGRATE_BEGIN                    (1)
#define VNET_VM_MIGRATE_END                      (2)

//////////////////////////////////////////////////////////////////////////
//                           VNET脚本参数
//////////////////////////////////////////////////////////////////////////
#define VNET_BRIDGE_SCRIPTS         "/usr/local/bin/tecs/network-scripts/vnet_dobridge.sh"
#define VNET_BRIDGE_SCRIPTS_OVS     "/usr/local/bin/tecs/network-scripts/vnet_dobridge_ovs.sh"
#define VNET_BONDS_SCRIPTS          "/usr/local/bin/tecs/network-scripts/do_bond.sh"

#define VNET_CREATE_BRIDGE          "create_bridge"
#define VNET_DELETE_BRIDGE          "delete_bridge"
#define VNET_ADD_IF                 "add_ifnet"
#define VNET_DEL_IF                 "del_ifnet"
#define VNET_ADD_VLAN               "add_vlan"
#define VNET_DEL_VLAN               "del_Vlan"
#define VNET_ADD_BOND               "add_bond"
#define VNET_DEL_BOND               "del_bond"
#define VNET_SET_BOND               "set_bond"

//////////////////////////////////////////////////////////////////////////
//                             资源状态常量定义
//////////////////////////////////////////////////////////////////////////
#define VNET_VLAN_STATUS_UNUSABLE           (0) // 不可用
#define VNET_VLAN_STATUS_USABLE             (1) // 可用
#define VNET_VLAN_STATUS_USED               (2) // 已分配
#define VNET_VLAN_FIRST_ADD                 (1) //第一次添加VLAN RANGE
#define VNET_VLAN_NOT_FIRST_ADD             (0) //不是第一次添加VLAN RANGE
#define VNET_MIN_VLAN_ID                    (1)
#define VNET_MAX_VLAN_ID                    (4094)
#define VNET_MIN_MAP_VLAN_ID                (2)
#define VNET_MAX_MAP_VLAN_ID                (4094)

#define VNET_SEGMENT_STATUS_UNUSABLE        (0) // 不可用
#define VNET_SEGMENT_STATUS_USABLE          (1) // 可用
#define VNET_SEGMENT_STATUS_USED            (2) // 已分配
#define VNET_SEGMENT_FIRST_ADD              (1) //第一次添加VLAN RANGE
#define VNET_SEGMENT_NOT_FIRST_ADD          (0) //不是第一次添加VLAN RANGE
#define VNET_MIN_SEGMENT_ID                 (1)
#define VNET_MAX_SEGMENT_ID                 (16000000)
#define VNET_MIN_MAP_SEGMENT_ID             (1)
#define VNET_MAX_MAP_SEGMENT_ID             (16000000)

#define VNET_COMMON_MTU                     (1500) // 普通网口MTU
#define VNET_BASE_MTU                       (1542) // base平面MTU
#define VNET_FABRIC_MTU                     (4000) // fabric平面MTU

#define VNET_STATE_MACHINE_INIT             (0)
#define VNET_STATE_MACHINE_START            (1)
#define VNET_STATE_MACHINE_WORK             (2)
#define VNM_SLAVE_STATE                     (0)
#define VNM_MASTER_STATE                    (1)

#define VNET_MAC_LEN                        (0x6)
#define VNET_MAC_OUI_ZTE_1                  ("00:d0:d0")
#define VNET_MAC_OUI_ZTE_2                  ("00:19:c6")

// wildcast task state
#define WILDCAST_TASK_STATE_INIT        (0x0)
#define WILDCAST_TASK_STATE_WAIT_ACK    (0x1)
#define WILDCAST_TASK_STATE_ACK_FAILED  (0x2)
#define WILDCAST_TASK_STATE_ACK_SUCCESS (0x3)
#define WILDCAST_TASK_STATE_SEND_MSG_FAILED (0x4)

// wildcast task send num(max)
#define WILDCAST_TASK_SEND_NUM_MAX      (0x5)

// wildcast task type
#define WILDCAST_TASK_TYPE_SWITCH        (0x0)
#define WILDCAST_TASK_TYPE_CREATE_VPORT  (0x1)
#define WILDCAST_TASK_TYPE_LOOPBACK      (0x2)


//////////////////////////////////////////////////////////////////////////
//                             MISC常量定义
//////////////////////////////////////////////////////////////////////////
#define VNET_FALSE                          (false)
#define VNET_TRUE                           (true)  

#define STRCPY                              strcpy
#define STRCMP                              strcmp
#define SPRINTF                             sprintf
#define ATOI                                atoi
#define ITOA                                itoa
#define MEMSET                              memset
#define VNET_ASSERT(x)                      SkyAssert(x)
#define STR_VSIID_INIT_VALUE                "00000000000000000000000000000000"
#define STR_MGRID_INIT_VALUE                "00000000000000000000000000000001"
#define STR_VSITYPEID_INIT_VALUE            "000001"

#define VSIFORMAT_PRINTF        "%8.8x%4.4x%4.4x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x"
#define VSIFORMAT_SCANF         "%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x"
#define MGRIDFORMAT_SCANF       "%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x"
#define VSITYPEIDFORMAT_SCANF   "%2x%2x%2x"

#define VNET_DEVICE_INVALID                 (-1)
#define VNET_DEVICE_UP                      (1)
#define VNET_DEVICE_DOWN                    (2)
#define VNET_DEVICE_STAT_UNKNOW             (3)

#define VNET_LOG_ERROR                      (SYS_EMERGENCY)
#define VNET_LOG_WARN                       (SYS_WARNING)
#define VNET_LOG_INFO                       (SYS_INFORMATIONAL)
#define VNET_LOG_DEBUG                      (SYS_DEBUG)

#define ioctlsocket(a,b,c)                  ioctl(a,b,c)

#define VNET_LOG(nLvl, fmt, arg...) \
    do \
    { \
        OutPut(nLvl, fmt, ##arg); \
    }while(0)


#define WAIT()  \
    cout << "Press enter to continue..."; \
    do \
    { \
       char c=cin.get(); \
       if(c == '\n') break; \
    }while(true)

//////////////////////////////////////////////////////////////////////////
//                           枚举定义
//////////////////////////////////////////////////////////////////////////
typedef enum tagENObjState
{
    EN_STATE_INIT = 0,
    EN_STATE_ONLINE, // 和数据库约束保持一致
    EN_STATE_OFFLINE,   
}ENObjState;


//////////////////////////////////////////////////////////////////////////
//                           数据结构定义
//////////////////////////////////////////////////////////////////////////
typedef struct tagLnxRSockPort
{
    #define LNX_IF_NAME_MAX_LEN  (16)
    int uiPortNo;
    char strIfName[LNX_IF_NAME_MAX_LEN];
    int32 nSock; 
}TLnxRSockPort;

//////////////////////////////////////////////////////////////////////////
//                           VNET全局函数定义
//////////////////////////////////////////////////////////////////////////
// 仿函数
class DelVNetObj {
public:
    template<class T>
    void operator()(const T* ptr) const
    {
        delete ptr;
        ptr = NULL;
        return ;
    }
};

// 定义一个统计时长宏 消息处理时间宏 
#define VNET_TIME_DEFINE(STR) \
string info = (STR);\
Datetime _trace_begin_time;\
Datetime _trace_end_time;\
Timespan _trace_time_span;

#define VNET_TIME_BEGIN() \
Datetime btime(time(0));\
_trace_begin_time = btime;\
_trace_time_span.Begin(); 

#define VNET_TIME_END() \
Datetime etime(time(0));\
_trace_end_time = etime;\
_trace_time_span.End();\
cout << (info) << " duration(us) : " << _trace_time_span.GetSpan() << \
  "(" << _trace_begin_time.tostr() << " - " <<  _trace_end_time.tostr() <<")" << endl;



int32 VNetFormatString(string &strResult, const char* pszFormat, ...);

int32 vnet_system(const string &strCmd);
int32 vnet_chk_dir(const string &strDir);
int32 vnet_chk_dev(const string &strDevName);
int32 vnet_chk_dev_state(const string &strDevName);
int32 local_is_bigendian();
int32 vnet_ntohl(int32 x);
int32 vnet_htonl(int32 x);
unsigned int  rawsocket_ntohl(unsigned int x);
unsigned short vnet_ntohs(unsigned short x);
unsigned short vnet_htons(unsigned short x);
char * ipv4_to_string(int addr);
void string_to_ipv4(const char *str, int32 &ip);  
void remove_lr_space(string & str);
int32 vnet_check_macstr(string & str);
int32 vnet_mac_str_to_byte(string & str, BYTE *mac);

void StopDhcpService();
int get_mtu(const char* lpszEth, unsigned int &mtu);
int set_mtu(const char* lpszEth, unsigned int mtu) ;

bool vnet_run_command(const string& command,string &result);       
int32 InitLnxRsockDrv(void);
int32 CloseLnxRsockDrv(void);
int32 LnxRsockInitEvbPort(int uiPortNo);
int32 LnxRsockDestroyEvbPort(int uiPortNo);
int wd_LnxRsockRcvPkt(unsigned char **ppPkt, int *pdwLen);
TLnxRSockPort * GetLnxRsockDevArray(int *pOutArrSize);	
int tecs_wd_rcv(unsigned char **ppPkt, int *pdwLen, int* pFlag);
string GetBrigNameFromPeth(const string& pethname);

int GetRealIfNameFromBridge(string & port, string& rel_name);
int VNetCheckSupportOVS();
string GetPhyPortFromBr(string strBrName);
int32  VNetCheckIsBr(const string &strDevName);

string GetVnetErrorInfo(int32 nErrorCode,const string & exinfo = "");
int32 VNetGetUUIDValue(BYTE aucValue[], BYTE ucValueSz = 16);
void print_systime();

#endif //VNET_COMM__INCLULDE_H_

