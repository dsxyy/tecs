///////////////////////////////////////////////////////////
//  AlarmManager.cpp
//  Implementation of the Class AlarmManager
//  Created on:      15-七月-2011 12:16:55
//  Original author: mazhaomian
///////////////////////////////////////////////////////////
#include <net-snmp/net-snmp-config.h>

#if HAVE_IO_H
#include <io.h>
#endif
#include <stdio.h>
#include <errno.h>
#if HAVE_STRING_H
#include <string.h>
#else
#include <strings.h>
#endif
#if HAVE_STDLIB_H
#include <stdlib.h>
#endif
#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <sys/types.h>
#if HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#if HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#if TIME_WITH_SYS_TIME
# ifdef WIN32
#  include <sys/timeb.h>
# else
#  include <sys/time.h>
# endif
# include <time.h>
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif
#if HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif
#if HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#elif HAVE_WINSOCK_H
#include <winsock.h>
#endif
#if HAVE_NET_IF_H
#include <net/if.h>
#endif
#if HAVE_INET_MIB2_H
#include <inet/mib2.h>
#endif
#if HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif
#if HAVE_SYS_FILE_H
#include <sys/file.h>
#endif
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#if HAVE_SYS_WAIT_H
#include <sys/wait.h>
#endif
#include <signal.h>
#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif
#if HAVE_PROCESS_H              /* Win32-getpid */
#include <process.h>
#endif
#if HAVE_LIMITS_H
#include <limits.h>
#endif
#if HAVE_PWD_H
#include <pwd.h>
#endif
#if HAVE_GRP_H
#include <grp.h>
#endif

#ifndef PATH_MAX
# ifdef _POSIX_PATH_MAX
#  define PATH_MAX _POSIX_PATH_MAX
# else
#  define PATH_MAX 255
# endif
#endif

#ifndef FD_SET
typedef long    fd_mask;
#define NFDBITS (sizeof(fd_mask) * NBBY)        /* bits per mask */
#define FD_SET(n, p)    ((p)->fds_bits[(n)/NFDBITS] |= (1 << ((n) % NFDBITS)))
#define FD_CLR(n, p)    ((p)->fds_bits[(n)/NFDBITS] &= ~(1 << ((n) % NFDBITS)))
#define FD_ISSET(n, p)  ((p)->fds_bits[(n)/NFDBITS] & (1 << ((n) % NFDBITS)))
#define FD_ZERO(p)      memset((p), 0, sizeof(*(p)))
#endif

#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include <net-snmp/library/fd_event_manager.h>
#include "m2m.h"
#include <net-snmp/agent/mib_module_config.h>
#include "snmpd.h"
#include "mibgroup/struct.h"
#include <net-snmp/agent/mib_modules.h>
#include "utilities/util_funcs.h"
#include <net-snmp/agent/agent_trap.h>
#include <net-snmp/agent/table.h>
#include <net-snmp/agent/table_iterator.h>
#include "sky.h"
#include "mib_module_includes.h"
#include "snmp_oam_mibhandler.h"
#include "snmp_job.h"
#include "postgresql_db.h"
#include "snmp_onlinecfg.h"
#include "snmp_interface.h"

using namespace zte_tecs;
using namespace xmlrpc_c;

const char *app_name = "snmpd";
extern struct timeval  starttime;
extern oid snmptrap_oid[11];
extern char g_community[VACMSTRINGLEN];

#define SNMPCOMM_THREAD_STACK_SIZE  4096*80
#define SNMPCOMM_THREAD_PRI         5

static oid alarm_report_oid[]        =
    {
        1, 3, 6, 1, 4, 1, 3902, 6051, 19, 1, 2, 1
    };
static oid alarm_id_oid[]            =
    {
        1, 3, 6, 1, 4, 1, 3902, 6051, 19, 1, 2, 3, 1
    };
static oid alarm_flag_oid[]          =
    {
        1, 3, 6, 1, 4, 1, 3902, 6051, 19, 1, 2, 3, 2
    };
static oid alarm_time_oid[]          =
    {
        1, 3, 6, 1, 4, 1, 3902, 6051, 19, 1, 2, 3, 3
    };
static oid restore_time_oid[]        =
    {
        1, 3, 6, 1, 4, 1, 3902, 6051, 19, 1, 2, 3, 4
    };
static oid restore_type_oid[]        =
    {
        1, 3, 6, 1, 4, 1, 3902, 6051, 19, 1, 2, 3, 5
    };
static oid alarm_address_oid[]       =
    {
        1, 3, 6, 1, 4, 1, 3902, 6051, 19, 1, 2, 3, 6
    };
static oid alarm_level1_oid[]       =
    {
        1, 3, 6, 1, 4, 1, 3902, 6051, 19, 1, 2, 3, 7
    };
static oid alarm_level2_oid[]       =
    {
        1, 3, 6, 1, 4, 1, 3902, 6051, 19, 1, 2, 3, 8
    };
static oid alarm_level3_oid[]       =
    {
        1, 3, 6, 1, 4, 1, 3902, 6051, 19, 1, 2, 3, 9
    };
static oid alarm_location_name_oid[] =
    {
        1, 3, 6, 1, 4, 1, 3902, 6051, 19, 1, 2, 3, 10
    };
static oid alarm_crc0_oid[]          =
    {
        1, 3, 6, 1, 4, 1, 3902, 6051, 19, 1, 2, 3, 11
    };
static oid alarm_crc1_oid[]          =
    {
        1, 3, 6, 1, 4, 1, 3902, 6051, 19, 1, 2, 3, 12
    };
static oid alarm_crc2_oid[]          =
    {
        1, 3, 6, 1, 4, 1, 3902, 6051, 19, 1, 2, 3, 13
    };
static oid alarm_crc3_oid[]          =
    {
        1, 3, 6, 1, 4, 1, 3902, 6051, 19, 1, 2, 3, 14
    };

//static oid alarm_addinfo_oid[]       = { 1, 3, 6, 1, 4, 1, 3902, 6051, 19, 1, 2, 3, 15};

static oid alarm_subsystem_oid[]      =
    {
        1, 3, 6, 1, 4, 1, 3902, 6051, 19, 1, 2, 3, 16
    };


namespace zte_tecs
{
string snmp_xml_file("/opt/tecs/tc/etc/snmp.xml");
SnmpJob* SnmpJob::instance = NULL;
/************************************************************
* 函数名称： SnmpJob
* 功能描述： 构造函数
* 访问的表： 无
* 修改的表： 无
* 输入参数：
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2011/10         V1.0       曹亮         创建
***************************************************************/
SnmpJob::SnmpJob()
{
    _scp = SnmpConfigurationPool::GetInstance();
    _tap = TrapAddressPool::GetInstance();
    _sup = SnmpXmlRpcUserPool::GetInstance();
    if(!_scp || !_tap || !_sup)
    {
        SkyAssert(false);
    }
    _snmp_xml_file = snmp_xml_file;
    _workthread_id = INVALID_THREAD_ID;
    mu = NULL;
}

STATUS SnmpJob::StartMu(const string& name)
{
    if(mu)
    {
        SkyAssert(false);
        return ERROR_DUPLICATED_OP;
    }
    
    // 消息单元的创建和初始化
    mu = new MessageUnit(name);
    if (!mu)
    {
        OutPut(SYS_EMERGENCY, "Create mu %s failed!\n",name.c_str());
        return ERROR_NO_MEMORY;
    }

    STATUS ret = mu->SetHandler(this);
    if (SUCCESS != ret)
    {
        OutPut(SYS_EMERGENCY, "SetHandler mu %s failed! ret = %d\n",name.c_str(),ret);
        return ret;
    }

    ret = mu->Run();
    if (SUCCESS != ret)
    {
        OutPut(SYS_EMERGENCY, "Run mu %s failed! ret = %d\n",name.c_str(),ret);
        return ret;
    }

    ret = mu->Register();
    if (SUCCESS != ret)
    {
        OutPut(SYS_EMERGENCY, "Register mu %s failed! ret = %d\n",name.c_str(),ret);
        return ret;
    }

    //给自己发送上电消息，促使消息单元状态切换到工作态
    MessageFrame message(SkyInt32(0), EV_POWER_ON);
    ret = mu->Receive(message);
    if (SUCCESS != ret)
    {
        SkyAssert(0);
        return ret;
    }
    mu->Print();
    return SUCCESS;
}

/************************************************************
* 函数名称： ~SnmpJob
* 功能描述： 析构函数
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2011/10         V1.0       曹亮         创建
***************************************************************/
SnmpJob::~SnmpJob()
{
    if (mu)
        delete mu;
    OutPut(SYS_DEBUG, "delete snmp_job\n");
}

/************************************************************
* 函数名称： Receive
* 功能描述： 主动接收消息的函数
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2011/10         V1.0       曹亮         创建
***************************************************************/
STATUS SnmpJob::Receive(const MessageFrame& message)
{
    return mu->Receive(message);
}

#if 0
/************************************************************
* 函数名称： JoinGroup
* 功能描述： 将ALARM_MANAGER加入消息组
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2011/10         V1.0       曹亮         创建
***************************************************************/
void SnmpJob::JoinGroup()
{
    //加入一个组播组
    mu->JoinMcGroup(ALARM_GROUP);
}

/************************************************************
* 函数名称： BroadcastPoweronMsg
* 功能描述： 上电处理函数
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2011/10         V1.0       曹亮         创建
***************************************************************/
void SnmpJob::BroadcastPoweronMsg()
{
    //向组播组发送组播消息
    DemoMessage msg("Alarm Manager Power on!");
    MID target("group",ALARM_GROUP);
    MessageOption option(target,EV_ALARM_BROADCAST,0,0);
    mu->Send(msg, option);
}
#endif

/************************************************************
* 函数名称： MessageEntry
* 功能描述： 消息处理函数
* 访问的表： 无
* 修改的表： 无
* 输入参数： message  消息信息
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2011/10         V1.0       曹亮         创建
***************************************************************/
void SnmpJob::MessageEntry(const MessageFrame& message)
{
    netsnmp_variable_list *var_list = NULL;
    oid trap_oid[50] = {0};
    int flag = 0;

    switch (mu->get_state())
    {
    case S_Startup:
    {
        switch (message.option.id())
        {
        case EV_POWER_ON:
        {
            mu->set_state(S_Work);
            OutPut(SYS_NOTICE, "%s power on!\n",mu->name().c_str());
            break;
        }
        default:
            OutPut(SYS_NOTICE, "Ignore any message except power on!\n");
            break;
        }
        break;
    }

    case S_Work:
    {
        switch (message.option.id())
        {
        case EV_ALARM_REPORT:
        case EV_INFORM_REPORT:
        {
            AgentAlarm alarmmsg;
            alarmmsg.deserialize(message.data);

            memcpy(trap_oid,alarm_report_oid,sizeof(alarm_report_oid));
            trap_oid[OID_LENGTH(alarm_report_oid)] = alarmmsg.dwAlarmCode;
            //开始绑定告警信息
            snmp_varlist_add_variable (&var_list,
                                       snmptrap_oid, OID_LENGTH (snmptrap_oid),
                                       ASN_OBJECT_ID, (u_char *)&trap_oid, (OID_LENGTH(alarm_report_oid)+1)*sizeof(oid));
            snmp_varlist_add_variable (&var_list,
                                       alarm_id_oid, OID_LENGTH (alarm_id_oid),
                                       ASN_UNSIGNED, (u_char *)&alarmmsg.dwAlarmID, sizeof(uint32));
            snmp_varlist_add_variable (&var_list,
                                       alarm_flag_oid, OID_LENGTH (alarm_flag_oid),
                                       ASN_UNSIGNED, (u_char *)&alarmmsg.wAlarmFlag, sizeof(uint16));
            snmp_varlist_add_variable (&var_list,
                                       alarm_time_oid, OID_LENGTH (alarm_time_oid),
                                       ASN_OCTET_STR, (u_char *)alarmmsg.tAlarmTime.tostr().c_str(), alarmmsg.tAlarmTime.tostr().length());
            snmp_varlist_add_variable (&var_list,
                                       restore_time_oid, OID_LENGTH (restore_time_oid),
                                       ASN_OCTET_STR, (u_char *)alarmmsg.tRestoreTime.tostr().c_str(), alarmmsg.tRestoreTime.tostr().length());
            snmp_varlist_add_variable (&var_list,
                                       restore_type_oid, OID_LENGTH (restore_type_oid),
                                       ASN_UNSIGNED, (u_char *)&alarmmsg.wRestoreType, sizeof(uint16));
            snmp_varlist_add_variable (&var_list,
                                       alarm_address_oid, OID_LENGTH (alarm_address_oid),
                                       ASN_OCTET_STR, (u_char *)alarmmsg.tAlarmAddr.c_str(), alarmmsg.tAlarmAddr.length());
            snmp_varlist_add_variable (&var_list,
                                       alarm_level1_oid, OID_LENGTH (alarm_level1_oid),
                                       ASN_OCTET_STR, (u_char *)alarmmsg.alarm_location.strLevel1.c_str(), alarmmsg.alarm_location.strLevel1.length());
            snmp_varlist_add_variable (&var_list,
                                       alarm_level2_oid, OID_LENGTH (alarm_level2_oid),
                                       ASN_OCTET_STR, (u_char *)alarmmsg.alarm_location.strLevel2.c_str(), alarmmsg.alarm_location.strLevel2.length());
            snmp_varlist_add_variable (&var_list,
                                       alarm_level3_oid, OID_LENGTH (alarm_level3_oid),
                                       ASN_OCTET_STR, (u_char *)alarmmsg.alarm_location.strLevel3.c_str(), alarmmsg.alarm_location.strLevel3.length());
            snmp_varlist_add_variable (&var_list,
                                       alarm_location_name_oid, OID_LENGTH (alarm_location_name_oid),
                                       ASN_OCTET_STR, (u_char *)alarmmsg.alarm_location.strLocation.c_str(), alarmmsg.alarm_location.strLocation.length());
            snmp_varlist_add_variable (&var_list,
                                       alarm_crc0_oid, OID_LENGTH (alarm_crc0_oid),
                                       ASN_UNSIGNED, (u_char *)&alarmmsg.dwCRCCode[0], sizeof(uint32));
            snmp_varlist_add_variable (&var_list,
                                       alarm_crc1_oid, OID_LENGTH (alarm_crc1_oid),
                                       ASN_UNSIGNED, (u_char *)&alarmmsg.dwCRCCode[1], sizeof(uint32));
            snmp_varlist_add_variable (&var_list,
                                       alarm_crc2_oid, OID_LENGTH (alarm_crc2_oid),
                                       ASN_UNSIGNED, (u_char *)&alarmmsg.dwCRCCode[2], sizeof(uint32));
            snmp_varlist_add_variable (&var_list,
                                       alarm_crc3_oid, OID_LENGTH (alarm_crc3_oid),
                                       ASN_UNSIGNED, (u_char *)&alarmmsg.dwCRCCode[3], sizeof(uint32));
            snmp_varlist_add_variable (&var_list,
                                       alarm_subsystem_oid, OID_LENGTH (alarm_subsystem_oid),
                                       ASN_OCTET_STR, (u_char *)alarmmsg.alarm_location.strSubSystem.c_str(), alarmmsg.alarm_location.strSubSystem.length());            
            
            oid_struct *pOid = (oid_struct *)malloc(sizeof(oid_struct));
            flag = 1;
            switch (alarmmsg.dwAlarmCode)
            {
                All_ALARM_MAP;
                
            default:
                flag = 0;
                break;
            }
            
            if(flag != 0)
                snmp_varlist_add_variable (&var_list,
                                           pOid->mem_oid, pOid->oid_len,
                                           pOid->oid_type, pOid->value, pOid->value_len);
            free(pOid);

            send_v2trap (var_list);
            snmp_free_varbind (var_list);
            break;
        }
        default:
            break;
        }
        break;
    }

    default:
        break;
    }
}

/************************************************************
* 函数名称： start
* 功能描述： 启动函数
* 访问的表： 无
* 修改的表： 无
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明：
* 修改日期        版本号     修改人       修改内容
* -----------------------------------------------
* 2011/10         V1.0       曹亮         创建
***************************************************************/
STATUS SnmpJob::start()
{
    //读取XML脚本注册信息
    ReadXml();

    if (_workthread_id != INVALID_THREAD_ID)
        return ERROR_DUPLICATED_OP;

    _workthread_id = StartThreadEx("snmp_thread",SNMPCOMM_THREAD_PRI,SNMPCOMM_THREAD_STACK_SIZE,SnmpCommunicationThread,(void*)this);
    if (INVALID_THREAD_ID == _workthread_id)
    {
        OutPut(SYS_ERROR, "start snmp_thread error!\n");
        return ERROR;
    }

    SnmpProtocolRegister(&tSnmpProcGlobal.tModuleDescrip[0]);

    GetXmlRpcSession();

    return SUCCESS;
}

bool SnmpJob::ReadXml()
{
    int i = 0;
    char     szOidName[MAX_VALUE_LENGTH] = {0};
    char szTemp[20] = {0};
    WORD16 wIndex = 0;
    WORD16 wLoop = 0;
    WORD16 wLen = 0;
    int flag = 0;
    char *ftypename = NULL;
    T_Oid_Property *ptr = NULL;
    T_Oid_Property *prevPtr = NULL;
    ostringstream   xml_str;
    string  str;

    tSnmpProcGlobal.ucModuleNum = 1;

    ifstream fin(_snmp_xml_file.c_str());
    if (!fin)
    {
        OutPut(SYS_ERROR, "open xml file error!\n");
        return false;
    }
    while (getline(fin,str))
    {
        xml_str << str << endl;
    }

    cout << xml_str.str() << endl;

    XmlParser xml;

    if (false == xml.Locate(xml_str.str(),"SNMP"))
    {
        OutPut(SYS_ERROR, "locate xml file error!\n");
        return false;
    }

    if (true == xml.GotoFirst("oid"))
    {
        while (1)
        {
            memset(tSnmpProcGlobal.tModuleDescrip[0].tOidDesCrip[i].oidName,0,OID_NAME);
            memcpy(tSnmpProcGlobal.tModuleDescrip[0].tOidDesCrip[i].oidName,
                   xml.GetAttr("snmp_name").c_str(),
                   xml.GetAttr("snmp_name").length());
            if (strcmp(xml.GetAttr("property").c_str(),"scalar") == 0)
            {
                tSnmpProcGlobal.tModuleDescrip[0].tOidDesCrip[i].ucOidType = OID_PROPERTY_SCALAR;
                tSnmpProcGlobal.tModuleDescrip[0].tOidDesCrip[i].ucAckType = OID_PROPERTY_RESPONSE;
                if (strcmp(xml.GetAttr("get").c_str(),"yes") == 0 &&
                        strcmp(xml.GetAttr("modify").c_str(),"yes") == 0)
                {
                    tSnmpProcGlobal.tModuleDescrip[0].tOidDesCrip[i].ucRwProp = OID_PROPERTY_RW;
                }
                else if (strcmp(xml.GetAttr("get").c_str(),"no") == 0 &&
                         strcmp(xml.GetAttr("modify").c_str(),"yes") == 0)
                {
                    tSnmpProcGlobal.tModuleDescrip[0].tOidDesCrip[i].ucRwProp = OID_PROPERTY_WRITE;
                }
                else if (strcmp(xml.GetAttr("get").c_str(),"yes") == 0 &&
                         strcmp(xml.GetAttr("modify").c_str(),"no") == 0)
                {
                    tSnmpProcGlobal.tModuleDescrip[0].tOidDesCrip[i].ucRwProp = OID_PROPERTY_READ;
                }
                memset(szOidName,0,MAX_VALUE_LENGTH);
                strcpy(szOidName, (char *)xml.GetAttr("value").c_str());
                wLoop = 0;
                wLen = 0;
                for (wIndex=0; wIndex<xml.GetAttr("value").length()+1; wIndex++)
                {
                    if (szOidName[wIndex] == '.' ||szOidName[wIndex] == '\0')
                    {
                        memset(szTemp, 0, 20);
                        if (wIndex - wLen < 0)
                        {
                            continue;
                        }
                        memcpy(szTemp, &szOidName[wLen], wIndex - wLen);
                        wLen = wIndex + 1;
                        tSnmpProcGlobal.tModuleDescrip[0].tOidDesCrip[i].BaseOidName[wLoop] = strtoul(szTemp, '\0', 10);
                        wLoop++;
                    }
                }
                tSnmpProcGlobal.tModuleDescrip[0].tOidDesCrip[i].wBaseOidLen = wLoop;

                /*进入下一层member节点*/
                xml.Enter();
                xml.GotoFirst("member");
                do
                {
                    ptr = (T_Oid_Property *)malloc(sizeof(T_Oid_Property));
                    memset(ptr,0,sizeof(T_Oid_Property));

                    /*获取ucParaType*/
                    ptr->ucParaType = SnmpGet_ftype_by_name(xml.GetAttr("snmp_type").c_str());
                    if (ptr->ucParaType == DATA_TYPE_UNDEFINED)
                    {
                        OutPut(SYS_ERROR, "error Para Type(%d)!", ptr->ucParaType);
                    }
                    /*获取ucParaXmlRpcType*/
                    ptr->ucParaXmlRpcType = SnmpGet_xmlrpcftype_by_name(xml.GetAttr("snmp_type").c_str());
                    if (ptr->ucParaXmlRpcType == DATA_TYPE_UNDEFINED)
                    {
                        OutPut(SYS_ERROR, "error Para Type(%d)!", ptr->ucParaXmlRpcType);
                    }

                    /*获取wArrayLen*/
                    ptr->wArrayLen = atoi(xml.GetAttr("snmp_length").c_str());
                    if (ptr->wArrayLen == 0)
                    {
                        ptr->wArrayLen = 1;
                    }

                    /*获取wParaLen*/
                    ftypename = SnmpGet_name_by_ftype(ptr->ucParaType);
                    if (ftypename != NULL)
                    {
                        /* 这里获取的是结构中的一个字段的长度*/
                        ptr->wParaLen = SnmpGet_ftype_value_Len(ptr->ucParaType, ptr->wArrayLen);
                        /*计算整个结构体的长度*/
                        tSnmpProcGlobal.tModuleDescrip[0].tOidDesCrip[i].dwStructLength += ptr->wParaLen;
                    }

                    /*获取ucRwProp*/
                    if (strcmp(xml.GetAttr("property").c_str(),"read") == 0)
                    {
                        ptr->ucRwProp= OID_PROPERTY_READ;
                    }
                    else if (strcmp(xml.GetAttr("property").c_str(),"write") == 0)
                    {
                        ptr->ucRwProp= OID_PROPERTY_WRITE;
                    }
                    else if (strcmp(xml.GetAttr("property").c_str(),"read_write") == 0)
                    {
                        ptr->ucRwProp= OID_PROPERTY_RW;
                    }

                    /*获取oidName*/
                    memcpy(ptr->oidName,xml.GetAttr("snmp_name").c_str(),xml.GetAttr("snmp_name").length());

                    /*获取pMaxValue*/
                    if (strcmp(xml.GetAttr("max_value").c_str(),"") != 0)
                    {
                        ptr->pMaxValue = (T_SNMP_NodeValue *)malloc(sizeof(T_SNMP_NodeValue));
                        memset(ptr->pMaxValue,0,sizeof(T_SNMP_NodeValue));
                        SnmpSet_ftype_Value((DWORD)ptr->ucParaType, ptr->pMaxValue, xml.GetAttr("max_value").c_str());
                    }

                    /*获取pMinValue*/
                    if (strcmp(xml.GetAttr("min_value").c_str(),"") != 0)
                    {
                        ptr->pMinValue = (T_SNMP_NodeValue *)malloc(sizeof(T_SNMP_NodeValue));
                        memset(ptr->pMinValue,0,sizeof(T_SNMP_NodeValue));
                        SnmpSet_ftype_Value((DWORD)ptr->ucParaType, ptr->pMinValue, xml.GetAttr("min_value").c_str());
                    }

                    /*获取ucPosNo*/
                    ptr->ucPosNo = atoi(xml.GetValue().c_str());

                    /*获取wOidLen和OidName*/
                    ptr->wOidLen = tSnmpProcGlobal.tModuleDescrip[0].tOidDesCrip[i].wBaseOidLen;
                    memmove(ptr->OidName, tSnmpProcGlobal.tModuleDescrip[0].tOidDesCrip[i].BaseOidName, ptr->wOidLen * sizeof(oid));
                    ptr->ptOwerOidDesc = &tSnmpProcGlobal.tModuleDescrip[0].tOidDesCrip[i];
                    ptr->OidName[ptr->wOidLen++] = ptr->ucPosNo;


                    if (tSnmpProcGlobal.tModuleDescrip[0].tOidDesCrip[i].pOidHead == NULL)
                    {
                        tSnmpProcGlobal.tModuleDescrip[0].tOidDesCrip[i].pOidHead = ptr;
                        tSnmpProcGlobal.tModuleDescrip[0].tOidDesCrip[i].pOidTail = ptr;
                        prevPtr = ptr;
                    }
                    else
                    {
                        prevPtr->pNext = ptr;
                        tSnmpProcGlobal.tModuleDescrip[0].tOidDesCrip[i].pOidTail = ptr;
                        prevPtr = ptr;
                    }
                }
                while (xml.GotoNext("member"));
                xml.Exit();
            }
            else if (strcmp(xml.GetAttr("property").c_str(),"table") == 0)
            {
                tSnmpProcGlobal.tModuleDescrip[0].tOidDesCrip[i].ucOidType = OID_PROPERTY_TABLE;
                tSnmpProcGlobal.tModuleDescrip[0].tOidDesCrip[i].ucAckType = OID_PROPERTY_RESPONSE;
                if (strcmp(xml.GetAttr("get").c_str(),"yes") == 0 &&
                        strcmp(xml.GetAttr("modify").c_str(),"yes") == 0)
                {
                    tSnmpProcGlobal.tModuleDescrip[0].tOidDesCrip[i].ucRwProp = OID_PROPERTY_RW;
                }
                else if (strcmp(xml.GetAttr("get").c_str(),"no") == 0 &&
                         strcmp(xml.GetAttr("modify").c_str(),"yes") == 0)
                {
                    tSnmpProcGlobal.tModuleDescrip[0].tOidDesCrip[i].ucRwProp = OID_PROPERTY_WRITE;
                }
                else if (strcmp(xml.GetAttr("get").c_str(),"yes") == 0 &&
                         strcmp(xml.GetAttr("modify").c_str(),"no") == 0)
                {
                    tSnmpProcGlobal.tModuleDescrip[0].tOidDesCrip[i].ucRwProp = OID_PROPERTY_READ;
                }
                if (strcmp(xml.GetAttr("add_del").c_str(),"yes") == 0)
                    tSnmpProcGlobal.tModuleDescrip[0].tOidDesCrip[i].ucModifyProp = FUNCID_SUPPORT_MODIFY;
                else
                    tSnmpProcGlobal.tModuleDescrip[0].tOidDesCrip[i].ucModifyProp = FUNCID_NOT_SUPPORT_MODIFY;
                memset(szOidName,0,MAX_VALUE_LENGTH);
                strcpy(szOidName, (char *)xml.GetAttr("value").c_str());
                wLoop = 0;
                wLen = 0;
                for (wIndex=0; wIndex<xml.GetAttr("value").length()+1; wIndex++)
                {

                    if (szOidName[wIndex] == '.' ||szOidName[wIndex] == '\0')
                    {
                        memset(szTemp, 0, 20);
                        if (wIndex - wLen < 0)
                        {
                            continue;
                        }
                        memcpy(szTemp, &szOidName[wLen], wIndex - wLen);
                        wLen = wIndex + 1;
                        tSnmpProcGlobal.tModuleDescrip[0].tOidDesCrip[i].BaseOidName[wLoop] = strtoul(szTemp, '\0', 10);
                        wLoop++;
                    }
                }
                tSnmpProcGlobal.tModuleDescrip[0].tOidDesCrip[i].wBaseOidLen = wLoop;

                /*进入下一层member节点*/
                xml.Enter();
                xml.GotoFirst("member");
                do
                {
                    ptr = (T_Oid_Property *)malloc(sizeof(T_Oid_Property));
                    memset(ptr,0,sizeof(T_Oid_Property));

                    /*获取ucParaType*/
                    ptr->ucParaType = SnmpGet_ftype_by_name(xml.GetAttr("snmp_type").c_str());
                    if (ptr->ucParaType == DATA_TYPE_UNDEFINED)
                    {
                        OutPut(SYS_ERROR, "error Para Type(%d)!", ptr->ucParaType);
                    }
                    /*获取ucParaXmlRpcType*/
                    ptr->ucParaXmlRpcType = SnmpGet_xmlrpcftype_by_name(xml.GetAttr("snmp_type").c_str());
                    if (ptr->ucParaXmlRpcType == DATA_TYPE_UNDEFINED)
                    {
                        OutPut(SYS_ERROR, "error Para Type(%d)!", ptr->ucParaXmlRpcType);
                    }

                    /*获取wArrayLen*/
                    ptr->wArrayLen = atoi(xml.GetAttr("snmp_length").c_str());
                    if (ptr->wArrayLen == 0)
                    {
                        ptr->wArrayLen = 1;
                    }

                    /*获取wParaLen*/
                    ftypename = SnmpGet_name_by_ftype(ptr->ucParaType);
                    if (ftypename != NULL)
                    {
                        /* 这里获取的是结构中的一个字段的长度*/
                        ptr->wParaLen = SnmpGet_ftype_value_Len(ptr->ucParaType, ptr->wArrayLen);
                        /*计算整个结构体的长度*/
                        tSnmpProcGlobal.tModuleDescrip[0].tOidDesCrip[i].dwStructLength += ptr->wParaLen;
                    }

                    /*获取ucRwProp*/
                    if (strcmp(xml.GetAttr("property").c_str(),"read") == 0)
                    {
                        ptr->ucRwProp= OID_PROPERTY_READ;
                    }
                    else if (strcmp(xml.GetAttr("property").c_str(),"write") == 0)
                    {
                        ptr->ucRwProp= OID_PROPERTY_WRITE;
                    }
                    else if (strcmp(xml.GetAttr("property").c_str(),"read_write") == 0)
                    {
                        ptr->ucRwProp= OID_PROPERTY_RW;
                    }

                    /*获取oidName*/
                    memcpy(ptr->oidName,xml.GetAttr("snmp_name").c_str(),xml.GetAttr("snmp_name").length());


                    /*获取pMaxValue*/
                    if (strcmp(xml.GetAttr("max_value").c_str(),"") != 0)
                    {
                        ptr->pMaxValue = (T_SNMP_NodeValue *)malloc(sizeof(T_SNMP_NodeValue));
                        memset(ptr->pMaxValue,0,sizeof(T_SNMP_NodeValue));
                        SnmpSet_ftype_Value((DWORD)ptr->ucParaType, ptr->pMaxValue, xml.GetAttr("max_value").c_str());
                    }

                    /*获取pMinValue*/
                    if (strcmp(xml.GetAttr("min_value").c_str(),"") != 0)
                    {
                        ptr->pMinValue = (T_SNMP_NodeValue *)malloc(sizeof(T_SNMP_NodeValue));
                        memset(ptr->pMinValue,0,sizeof(T_SNMP_NodeValue));
                        SnmpSet_ftype_Value((DWORD)ptr->ucParaType, ptr->pMinValue, xml.GetAttr("min_value").c_str());
                    }

                    /*获取ucPosNo*/
                    ptr->ucPosNo = atoi(xml.GetValue().c_str());

                    /*获取wOidLen和OidName*/
                    ptr->wOidLen = tSnmpProcGlobal.tModuleDescrip[0].tOidDesCrip[i].wBaseOidLen;
                    memmove(ptr->OidName, tSnmpProcGlobal.tModuleDescrip[0].tOidDesCrip[i].BaseOidName, ptr->wOidLen * sizeof(oid));
                    ptr->ptOwerOidDesc = &tSnmpProcGlobal.tModuleDescrip[0].tOidDesCrip[i];
                    ptr->OidName[ptr->wOidLen++] = 1;
                    ptr->OidName[ptr->wOidLen++] = ptr->ucPosNo;

                    /*获取ucIndexProp*/
                    if (strcmp(xml.GetAttr("snmp_key").c_str(),"no") == 0)
                    {
                        ptr->ucIndexProp = OID_PROPERTY_NOT_INDEX;

                    }
                    else
                    {
                        ptr->ucIndexProp = OID_PROPERTY_INDEX;
                        tSnmpProcGlobal.tModuleDescrip[0].tOidDesCrip[i].ucIndexNum++;
                        tSnmpProcGlobal.tModuleDescrip[0].tOidDesCrip[i].wIndex_name_len += SnmpGet_Index_name_len(ptr->ucParaType);
                        tSnmpProcGlobal.tModuleDescrip[0].tOidDesCrip[i].wIndexLen += ptr->wParaLen;
                    }

                    /*获取ucRowStateProp*/
                    if (strcmp(xml.GetAttr("rowstate").c_str(),"no") == 0)
                        ptr->ucRowStateProp = OID_PROPERTY_NOT_ROWSTATUS;
                    else
                        ptr->ucRowStateProp = OID_PROPERTY_ROWSTATUS;

                    if (tSnmpProcGlobal.tModuleDescrip[0].tOidDesCrip[i].pOidHead == NULL)
                    {
                        tSnmpProcGlobal.tModuleDescrip[0].tOidDesCrip[i].pOidHead = ptr;
                        tSnmpProcGlobal.tModuleDescrip[0].tOidDesCrip[i].pOidTail = ptr;
                        prevPtr = ptr;
                    }
                    else
                    {
                        prevPtr->pNext = ptr;
                        tSnmpProcGlobal.tModuleDescrip[0].tOidDesCrip[i].pOidTail = ptr;
                        prevPtr = ptr;
                    }

                }
                while (xml.GotoNext("member"));
                xml.Exit();
            }

            i++;
            tSnmpProcGlobal.tModuleDescrip[0].wOidNum++;

            for (int j=0;j<i;j++)
            {
                if (false == xml.GotoNext("oid"))
                {
                    flag = 1;
                    break;
                }
            }
            if (flag != 0)
                break;
        }
    }
    else
    {
        OutPut(SYS_ERROR, "GotoFirst oid error!\n");
        return false;
    }
    return true;
}

void SnmpJob::read_configdata_handler(char* achTblName)
{
    SnmpConfiguration *sc = NULL;
    char cptr[STRINGMAX] = {0};
    string error_str;
    int64 oid = 0;

    ostringstream   oss;
    ostringstream   oss1;
    ostringstream   oss2;

    if (!strcmp(achTblName,"R_COMMUNITY"))
    {
        sc = _scp->Get(1,false);

        if (sc)
        {
            /* SECNAME */
            oss << "mynetwork ";
            /* NETWORK */
            oss << "10.0.0.0";
            /* MASK */
            oss << "/8 ";
            /* COMMUNITY */
            oss << sc->get_current_conf().Community << endl;
            
            memcpy(cptr,oss.str().c_str(),oss.str().length());
            netsnmp_udp_parse_security("com2sec", cptr);

            if (sc->get_current_conf().Community.length() < VACMSTRINGLEN)
                strcpy(g_community,(char *)sc->get_current_conf().Community.c_str());
            else
            {
                strcpy(g_community,(char *)"platform");
                OutPut(SYS_ERROR,"wrong community length\n");
            }
        }
        else
        {
            T_SnmpConfiguration snmp_conf("platform","platform",SNMP_VERSION_2c);
            if (_scp->Allocate(&oid, snmp_conf, error_str) < 0 )
            {
                OutPut(SYS_ERROR,"fail to operate snmp_conf table,because of %s\n",error_str.c_str());
            }
            else
            {
                OutPut(SYS_DEBUG,"add a new snmp_conf table\n");
            }
            /* SECNAME */
            oss << "mynetwork ";
            /* NETWORK */
            oss << "10.0.0.0";
            /* MASK */
            oss << "/8 ";
            /* COMMUNITY */
            oss << "platform" << endl;

            memcpy(cptr,oss.str().c_str(),oss.str().length());
            netsnmp_udp_parse_security("com2sec", cptr);

            /*写入内存*/
            strcpy(g_community,"platform");
            SetSNMPTrapCommunity((char *)"platform",strlen("platform"));
            SetTrapVersion(SNMP_VERSION_2c);
        }
    }

    if (!strcmp(achTblName,"R_USMUSER"))
    {
        /* USERNAME */
        oss << "root ";
        /* AUTHPROTOCOL */
        oss << "MD5 ";
        /* AUTHPWD */
        oss << "cmmrootpass ";
        /* PRIVACYPROTOCOL */
        oss << "DES ";
        /* PRIVACYPWD */
        oss << "cmmrootpass ";

        memcpy(cptr,oss.str().c_str(),oss.str().length());
        usm_parse_create_usmUser("createUser", cptr);
    }

    if (!strcmp(achTblName,"R_GROUP"))
    {
        /* GROUPNAME */
        oss << "MyRWGroup ";
        /* SECMODEL */
        oss << "v1 ";
        /* SECNAME */
        oss << "mynetwork ";

        memcpy(cptr,oss.str().c_str(),oss.str().length());
        vacm_parse_group("group", cptr);

        /* GROUPNAME */
        oss1 << "MyRWGroup ";
        /* SECMODEL */
        oss1 << "v2c ";
        /* SECNAME */
        oss1 << "mynetwork ";

        memset(cptr,0,STRINGMAX);
        memcpy(cptr,oss1.str().c_str(),oss1.str().length());
        vacm_parse_group("group", cptr);

        /* GROUPNAME */
        oss2 << "MyRWGroup ";
        /* SECMODEL */
        oss2 << "usm ";
        /* SECNAME */
        oss2 << "root ";

        memset(cptr,0,STRINGMAX);
        memcpy(cptr,oss2.str().c_str(),oss2.str().length());
        vacm_parse_group("group", cptr);
    }

    if (!strcmp(achTblName,"R_VIEW"))
    {
        oss << "all ";
        oss << "included ";
        oss << ".1 ";
        oss << "80 ";

        memcpy(cptr,oss.str().c_str(),oss.str().length());
        vacm_parse_view("view", cptr);
    }

    if (!strcmp(achTblName,"R_ACCESS"))
    {
        oss << "MyRWGroup ";
        oss << "\"\" ";
        oss << "any ";
        oss << "noauth ";
        oss << "exact ";
        oss << "all ";
        oss << "all ";
        oss << "none ";

        memcpy(cptr,oss.str().c_str(),oss.str().length());
        vacm_parse_access("access", cptr);
    }

    if (!strcmp(achTblName,"R_TRAPCOMMUNITY"))
    {
        sc = _scp->Get(1,false);

        if (sc)
        {
            oss << sc->get_current_conf().TrapCommunity << endl;

            memcpy(cptr,oss.str().c_str(),oss.str().length());
            snmpd_parse_config_trapcommunity("trapcommunity",cptr);
        }
        else
        {
            T_SnmpConfiguration snmp_conf("platform","platform",1);
            if (_scp->Allocate(&oid, snmp_conf, error_str) < 0 )
            {
                OutPut(SYS_ERROR,"fail to operate snmp_conf table,because of %s\n",error_str.c_str());
            }
            else
            {
                OutPut(SYS_DEBUG,"add a new snmp_conf table\n");
            }

            oss << "platform";

            memcpy(cptr,oss.str().c_str(),oss.str().length());
            snmpd_parse_config_trapcommunity("trapcommunity",cptr);
        }
    }
    if (!strcmp(achTblName,"R_TRAP2SINK"))
    {
        vector<xmlrpc_c::value> arrayTrapAddress;
        int trapversion = SNMP_VERSION_2c;

        if (_tap==NULL)
        {
            OutPut(SYS_ERROR,"_tap is NULL!\n");
            return;
        }

        if (_tap->Dump(arrayTrapAddress,"1=1") < 0)
        {
            OutPut(SYS_ERROR,"fail to operate trap_address table\n");
        }
        else
        {
            sc = _scp->Get(1,false);

            if (sc)
            {
                trapversion = sc->get_current_conf().TrapVersion;
            }

            vector<xmlrpc_c::value>::iterator it_struct = arrayTrapAddress.begin();
            for (; it_struct != arrayTrapAddress.end(); ++it_struct)
            {
                ostringstream oss3;
                map<string, xmlrpc_c::value> p = value_struct(*it_struct);
                map<string, xmlrpc_c::value>::iterator it;

                if (p.end() != (it = p.find("ip_address")))
                {
                    string ipaddress = xmlrpc_c::value_string(it->second);
                    oss3 << ipaddress << ":";
                }
                if (p.end() != (it = p.find("port")))
                {
                    int port = xmlrpc_c::value_int(it->second);
                    oss3 << port;
                }

                memcpy(cptr,oss3.str().c_str(),oss3.str().length());

                if (trapversion == SNMP_VERSION_1)
                {
                    snmpd_parse_config_trapsink("trapsink",cptr);
                }
                else
                {
                    snmpd_parse_config_trap2sink("trap2sink",cptr);
                }
            }
        }

    }
}

void SnmpJob::read_snmpdataconf(void)
{
    read_configdata_handler((char *)"R_COMMUNITY");
    read_configdata_handler((char *)"R_USMUSER");
    read_configdata_handler((char *)"R_GROUP");
    read_configdata_handler((char *)"R_VIEW");
    read_configdata_handler((char *)"R_ACCESS");
    read_configdata_handler((char *)"R_TRAPCOMMUNITY");
    read_configdata_handler((char *)"R_TRAP2SINK");
}


void SnmpJob::GetXmlRpcSession(void)
{
    SnmpXmlRpcUser *su = new SnmpXmlRpcUser;

    if (_sup->GetUser(su) == 0)
    {
    	
        SnmpLoginAuth(su->_snmp_user.User);
    }

    delete su;
}
}

/******************************************************************************
* 函数名称：void SnmpCommunicationThread(void *ptr)
* 功能描述：SNMP通讯线程调用的处理函数
* 访问的表：无
* 修改的表：无
* 输入参数：无
* 输出参数：无
* 返 回 值：无
* 其它说明：无
*
* 修改日期     版本号    修改人    修改内容
* -----------------------------------------------------------------------------
* 2009/12/23    1.0      曹亮      创建
******************************************************************************/
void *SnmpCommunicationThread(void *ptr)
{
    int             numfds;
    int             ret = 0;
    fd_set          readfds, writefds, exceptfds;
    struct timeval  timeout, *tvp = &timeout;
    int             count, block;

    netsnmp_ds_set_boolean(NETSNMP_DS_APPLICATION_ID,
                           NETSNMP_DS_AGENT_AGENTX_MASTER, 0);
    netsnmp_ds_set_int(NETSNMP_DS_APPLICATION_ID,
                       NETSNMP_DS_AGENT_AGENTX_TIMEOUT, -1);
    netsnmp_ds_set_int(NETSNMP_DS_APPLICATION_ID,
                       NETSNMP_DS_AGENT_AGENTX_RETRIES, -1);

    netsnmp_ds_set_int(NETSNMP_DS_APPLICATION_ID,
                       NETSNMP_DS_AGENT_CACHE_TIMEOUT, 5);

    netsnmp_ds_set_string(NETSNMP_DS_LIBRARY_ID,
                          NETSNMP_DS_LIB_APPTYPE, app_name);


    /*启动日志文件*/
    snmp_enable_filelog(NETSNMP_LOGFILE,
                        netsnmp_ds_get_boolean(NETSNMP_DS_LIBRARY_ID,
                                               NETSNMP_DS_LIB_APPEND_LOGFILES));

    SOCK_STARTUP;
    init_agent(app_name);        /* do what we need to do first. */

    /*读取数据库中的文件*/
    init_mib_modules();

    /*
     * start library
     */
    init_snmp(app_name);

    if ((ret = init_master_agent()) != 0)
    {
//        snmp_log(LOG_ERROR, "init_master_agent failed!! ret = %d\n",ret);
        return NULL;
    }

    /*
     * Store persistent data immediately in case we crash later.*/

    snmp_store(app_name);

    /*
     * Send coldstart trap if possible.
     */
    send_easy_trap(0, 0);

    /*获取snmp启动时间*/
    gettimeofday(&starttime, NULL);
    starttime.tv_sec--;
    starttime.tv_usec += 1000000L;

    while (1)
    {
        /*
         * default to sleeping for a really long time. INT_MAX
         * should be sufficient (eg we don't care if time_t is
         * a long that's bigger than an int).
         */
        tvp = &timeout;
        tvp->tv_sec = INT_MAX;
        tvp->tv_usec = 0;

        numfds = 0;
        FD_ZERO(&readfds);
        FD_ZERO(&writefds);
        FD_ZERO(&exceptfds);
        block = 0;
        snmp_select_info(&numfds, &readfds, tvp, &block);
        if (block == 1)
        {
            tvp = NULL;         /* block without timeout */
        }
        netsnmp_external_event_info(&numfds, &readfds, &writefds, &exceptfds);

reselect:
        if (tvp)
            DEBUGMSGTL(("timer", "tvp %d.%d\n", tvp->tv_sec, tvp->tv_usec));
        count = select(numfds, &readfds, &writefds, &exceptfds, tvp);
        /*         OAM_SYSLOG(LOG_DEBUG, "Recv Data, count = %u!\n", count); */
        if (count > 0)
        {
            netsnmp_dispatch_external_events(&count, &readfds,
                                             &writefds, &exceptfds);
            /* If there are still events leftover, process them */
            if (count > 0)
            {
                snmp_read(&readfds);
            }
            else
            {
                switch (count)
                {
                case 0:
                    snmp_timeout();
                    break;
                case -1:
                    DEBUGMSGTL(("snmpd/select", "  errno = %d\n", errno));
                    if (errno == EINTR)
                    {
                        goto reselect;
                        continue;
                    }
                    else
                    {
                        snmp_log_perror("select");
                    }
                    return NULL;
                default:
//                        snmp_log(LOG_ERR, "select returned %d\n", count);
                    return NULL;
                }

                netsnmp_check_outstanding_agent_requests();
            }
        }
    }
//    pthread_exit("Snmp Communication thread run exception");
}

char *SnmpDbgGetIndexString(BYTE ucType)
{
    if (ucType == OID_PROPERTY_NOT_INDEX)
    {
        return (char *)"NO";
    }
    else if (ucType == OID_PROPERTY_INDEX)
    {
        return (char *)"YES";
    }
    else
    {
        return (char *)"-";
    }
}

char *SnmpDbgGetInOutString(BYTE ucType)
{
    if (ucType == OID_PROPERTY_IN)
    {
        return (char *)"IN";
    }
    else if (ucType == OID_PROPERTY_OUT)
    {
        return (char *)"OUT";
    }
    else if (ucType == OID_PROPERTY_IN_AND_OUT)
    {
        return (char *)"IN/OUT";
    }
    else
    {
        return (char *)"-";
    }
}

char *SnmpDbgGetAvailString(BYTE ucType)
{
    if (ucType == OID_PROPERTY_NOT_AVAIABLE)
    {
        return (char *)"NOT_AVAIABLE";
    }
    else if (ucType == OID_PROPERTY_AVAIABLE)
    {
        return (char *)"AVAIABLE";
    }
    else
    {
        return (char *)"NULL";
    }
}

char *SnmpDbgGetRowStatusString(BYTE ucType)
{
    if (ucType == OID_PROPERTY_NOT_ROWSTATUS)
    {
        return (char *)"NO";
    }
    else if (ucType == OID_PROPERTY_ROWSTATUS)
    {
        return (char *)"YES";
    }
    else
    {
        return (char *)"NULL";
    }
}

char *SnmpDbgGetRwPropString(BYTE ucType)
{
    if (ucType == OID_PROPERTY_READ)
    {
        return (char *)"R";
    }
    else if (ucType == OID_PROPERTY_WRITE)
    {
        return (char *)"W";
    }
    else if (ucType == OID_PROPERTY_RW)
    {
        return (char *)"R/W";
    }
    else
    {
        return (char *)"NULL";
    }
}

char *SnmpDbgGetFuncAckPropString(BYTE ucType)
{
    if (ucType == OID_PROPERTY_RESPONSE)
    {
        return (char *)"RESP";
    }
    else if (ucType == OID_PROPERTY_TRAP)
    {
        return (char *)"TRAP";
    }
    else
    {
        return (char *)"NULL";
    }
}

char *SnmpDbgGetOidTypeString(BYTE ucType)
{
    if (ucType == OID_PROPERTY_SCALAR)
    {
        return (char *)"SCALAR";
    }
    else if (ucType == OID_PROPERTY_TABLE)
    {
        return (char *)"TABLE";
    }
    else
    {
        return (char *)"NULL";
    }
}

void OAM_DbgShowSnmpXmlInfo()
{
    BYTE ucLoop = 0;
    WORD16 wLoop = 0,wLoop1 = 0;
    BYTE ucOidNo = 1;
    char aszTemp[255] = {0};

    SnmpJob* pSnmpJob = SnmpJob::GetInstance();

    T_Module_Snmp_Description *ptModuleSnmpInfo = NULL;
    T_Oid_Property *pOidPtr = NULL;

    for (ucLoop=0;ucLoop<pSnmpJob->tSnmpProcGlobal.ucModuleNum;ucLoop++)
    {
        ptModuleSnmpInfo = &pSnmpJob->tSnmpProcGlobal.tModuleDescrip[ucLoop];
        for (wLoop=0;wLoop<ptModuleSnmpInfo->wOidNum;wLoop++)
        {
            printf("Index, FuncID, OidType, AckType, FuncType, IndexNum, StructLen, AckFlag, RwType, Name\n");
            printf("==============================================================================================\n");
            ucOidNo = 1;
            printf("%5u, %6lu, %7s, %7s, %8s, %8d, %9d, %7d, %6d, %8s\n",
                   wLoop+1,ptModuleSnmpInfo->tOidDesCrip[wLoop].dwFuncID,
                   SnmpDbgGetOidTypeString(ptModuleSnmpInfo->tOidDesCrip[wLoop].ucOidType),
                   SnmpDbgGetFuncAckPropString(ptModuleSnmpInfo->tOidDesCrip[wLoop].ucAckType),
                   SnmpDbgGetInOutString(ptModuleSnmpInfo->tOidDesCrip[wLoop].ucFuncType),
                   ptModuleSnmpInfo->tOidDesCrip[wLoop].ucIndexNum,
                   ptModuleSnmpInfo->tOidDesCrip[wLoop].dwStructLength, ptModuleSnmpInfo->ucXMLAckFlag,
                   ptModuleSnmpInfo->tOidDesCrip[wLoop].ucRwProp,
                   ptModuleSnmpInfo->tOidDesCrip[wLoop].oidName);
            for (wLoop1=0;wLoop1<ptModuleSnmpInfo->tOidDesCrip[wLoop].wBaseOidLen;wLoop1++)
            {
                if (wLoop1 == ptModuleSnmpInfo->tOidDesCrip[wLoop].wBaseOidLen-1)
                    printf("%lu\n",ptModuleSnmpInfo->tOidDesCrip[wLoop].BaseOidName[wLoop1]);
                else
                    printf("%lu.",ptModuleSnmpInfo->tOidDesCrip[wLoop].BaseOidName[wLoop1]);
            }
            printf("==============================================================================================\n");
            pOidPtr = ptModuleSnmpInfo->tOidDesCrip[wLoop].pOidHead;
            printf("oidNo.,  Pos, Type, arrLen, IOProp, IndxProp, RowStat, RwProp, ParaLen, maxVal, minVal\n");
            printf("==============================================================================================\n");
            while (pOidPtr)
            {
                printf("%6d, %4d, %6s, %6d, %6s, %8s, %6s, %8s, %5d,  ",
                       ucOidNo,pOidPtr->ucPosNo,
                       SnmpGet_name_by_ftype(pOidPtr->ucParaType),
                       pOidPtr->wArrayLen,
                       SnmpDbgGetInOutString(pOidPtr->ucInOutProp),
                       SnmpDbgGetIndexString(pOidPtr->ucIndexProp),
                       SnmpDbgGetRowStatusString(pOidPtr->ucRowStateProp),
                       SnmpDbgGetRwPropString(pOidPtr->ucRwProp),pOidPtr->wParaLen);

                SnmpFormatString(pOidPtr->ucParaType, pOidPtr->pMaxValue, aszTemp, sizeof(aszTemp));
                printf("%6s,  ", aszTemp);
                SnmpFormatString(pOidPtr->ucParaType, pOidPtr->pMinValue, aszTemp, sizeof(aszTemp));
                printf("%6s\n", aszTemp);
                ucOidNo++;
                pOidPtr = (T_Oid_Property *)pOidPtr->pNext;
            }
            printf("==============================================================================================\n");
        }
    }
    printf("                [SNMP XML Information Table]\n");

}
DEFINE_DEBUG_FUNC(OAM_DbgShowSnmpXmlInfo);
