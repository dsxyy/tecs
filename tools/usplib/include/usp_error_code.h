/*********************************************************************
* 版权所有 (C)2010, 深圳市中兴通讯股份有限公司。
*
* 文件名称： usp_error_code.h
* 文件标识： 见配置管理计划书
* 内容摘要： USP项目错误码定义
* 其它说明： 无
* 当前版本： 1.0
* 作    者： liu.yong31@zte.com.cn
* 完成日期： 2010-10-19
**********************************************************************/
#ifndef USP_ERROR_CODE_H
#define USP_ERROR_CODE_H

#include "usp_common_typedef.h"
#include "usp_capi.h"

/*******************************************************************
** 定义USP项目子系统错误码
********************************************************************/
#define USP_ERROR_CODE_BASE           (0x01000000)
#define AMS_ERROR_CODE_BASE           (USP_ERROR_CODE_BASE | 0x00010000)
#define SCS_ERROR_CODE_BASE           (USP_ERROR_CODE_BASE | 0x00020000)
#define STS_ERROR_CODE_BASE           (USP_ERROR_CODE_BASE | 0x00030000)
#define CBS_ERROR_CODE_BASE           (USP_ERROR_CODE_BASE | 0x00040000)
#define VDS_ERROR_CODE_BASE           (USP_ERROR_CODE_BASE | 0x00050000)
#define DM_ERROR_CODE_BASE            (USP_ERROR_CODE_BASE | 0x00060000)
#define SAS_ERROR_CODE_BASE           (USP_ERROR_CODE_BASE | 0x00070000)
#define JBODC_ERROR_CODE_BASE         (USP_ERROR_CODE_BASE | 0x00080000)
#define ISCSI_ERROR_CODE_BASE         (USP_ERROR_CODE_BASE | 0x00090000)
#define MTS_ERROR_CODE_BASE           (USP_ERROR_CODE_BASE | 0x000F0000)
#define TCS_ERROR_CODE_BASE           ISCSI_ERROR_CODE_BASE

/*******************************************************************
** 定义AMS子系统内部错误码
********************************************************************/
#define AMS_ERROR_CODE_XXXX          (AMS_ERROR_CODE_BASE | 0x00010000)

/*******************************************************************
** 定义SCS子系统内部错误码
********************************************************************/
#define SCS_CAPI_ERROR_CODE_BASE            (SCS_ERROR_CODE_BASE | 0x00001000)
#define SCS_SMA_ERROR_CODE_BASE             (SCS_ERROR_CODE_BASE | 0x00002000)
#define SCS_RCC_ERROR_CODE_BASE             (SCS_ERROR_CODE_BASE | 0x00003000)
#define SCS_EVT_ERROR_CODE_BASE             (SCS_ERROR_CODE_BASE | 0x00004000)
#define SCS_LOG_ERROR_CODE_BASE             (SCS_ERROR_CODE_BASE | 0x00005000)
#define SCS_MON_ERROR_CODE_BASE             (SCS_ERROR_CODE_BASE | 0x00006000)
#define SCS_BOOT_ERROR_CODE_BASE            (SCS_ERROR_CODE_BASE | 0x00007000)
#define SCS_BANDIN_ERROR_CODE_BASE          (SCS_ERROR_CODE_BASE | 0x00008000)
#define SCS_MAIL_ERROR_CODE_BASE            (SCS_ERROR_CODE_BASE | 0x00009000)
#define SCS_CFG_FILE_ERROR_CODE_BASE        (SCS_ERROR_CODE_BASE | 0x0000A000)
#define SCS_VMM_ERROR_CODE_BASE             (SCS_ERROR_CODE_BASE | 0x0000B000)
#define SCS_DIAG_ERROR_CODE_BASE            (SCS_ERROR_CODE_BASE | 0x0000C000)

#define SCS_SIC_ERROR_CODE_BASE             (SCS_ERROR_CODE_BASE | 0x0000D000)

/* SCS  通用的一些错误码的定义 */
#define E_SCS_MEMORY_MALLOC                                  (SCS_ERROR_CODE_BASE | 0x0000)            /* 内存空间申请失败 */
#define E_SCS_PARAMETER_ERROR                                (SCS_ERROR_CODE_BASE | 0x0001)            /* 参数错误*/
#define E_SCS_SIG_PARAMETER_ERROR                            (SCS_ERROR_CODE_BASE | 0x0002)            /* 单控下的参数错误*/
#define E_SCS_DUAL_PARAMETER_ERROR                           (SCS_ERROR_CODE_BASE | 0x0003)            /* 双控下的参数错误*/
#define E_SCS_EXPANDER_NOTFIND                               (SCS_ERROR_CODE_BASE | 0x0004)            /* 未找到EXPADNER*/
#define E_SCS_SYSTEM_ABNORMAL                                (SCS_ERROR_CODE_BASE | 0x0005)            /* 系统异常，配置失败*/

/* BOOT 主要针对重启和关闭 */
#define E_SCS_BOOT_PEER_NOT_RUNNING                          (SCS_BOOT_ERROR_CODE_BASE | 0x0001)       /* 对端不在运行状态 */

/*CAPI*/

#define E_SCS_CAPI_CONNECT_CREATE                            (SCS_CAPI_ERROR_CODE_BASE | 0x0001)       /* 创建连接失败*/
#define E_SCS_CAPI_CONNECT_FAIL                              (SCS_CAPI_ERROR_CODE_BASE | 0x0002)       /* Socket 连接建立失败 */
#define E_SCS_CAPI_SEND_REQ_FAIL                             (SCS_CAPI_ERROR_CODE_BASE | 0x0003)       /* 请求消息发送失败 */
#define E_SCS_CAPI_RECV_RSP_ERROR                            (SCS_CAPI_ERROR_CODE_BASE | 0x0007)       /* 接收应答消息错误*/
#define E_SCS_CAPI_RECV_MSG_TIMEOUT                          (SCS_CAPI_ERROR_CODE_BASE | 0x000B)       /* 接收应答帧超时*/
#define E_SCS_CAPI_ENDIAN_REVERSE_FAIL                       (SCS_CAPI_ERROR_CODE_BASE | 0x0004)       /* 消息字节序转换失败*/
#define E_SCS_CAPI_SET_SVRSOCK_FAIL                          (SCS_CAPI_ERROR_CODE_BASE | 0x0005)       /* 设置capi与存储设备连接的ip失败*/
#define E_SCS_CAPI_GET_SVRSOCK_FAIL                          (SCS_CAPI_ERROR_CODE_BASE | 0x0006)       /* 获取capi与存储设备连接的ip失败*/
#define E_SCS_CAPI_PARAMETER_ERROR                           (SCS_CAPI_ERROR_CODE_BASE | 0x0008)       /* 参数错误*/

/*SMA*/
/************************SMA_ ERROR *******************/

//SMA处理异常
#define E_SCS_SMA_MSG_GENERATE                                (SCS_SMA_ERROR_CODE_BASE | 0x0000)       /* 消息节点生成失败 */
#define E_SCS_SMA_SOCKET_CONNECT                              (SCS_SMA_ERROR_CODE_BASE | 0x0001)       /* 接收线程中，Socket连接建立失败*/
#define E_SCS_SMA_THREAD_CREATE                               (SCS_SMA_ERROR_CODE_BASE | 0x0002)       /* SMA 线程创建失败 */
#define E_SCS_SMA_PEER_TIMEOUT                                (SCS_SMA_ERROR_CODE_BASE | 0x0003)       /* 对端处理超时 */
#define E_SCS_SMA_SEND_REQ_TO_PEER                            (SCS_SMA_ERROR_CODE_BASE | 0x0004)       /* 发送请求消息到对端失败 */
#define E_SCS_SMA_PEER_NORUNNING                              (SCS_SMA_ERROR_CODE_BASE | 0x0005)       /* 对端控制器非启动或者运行状态 */
#define E_SCS_SMA_CMD_NO_EXIST                                (SCS_SMA_ERROR_CODE_BASE | 0x0006)       /* 请求命令不存在 */
#define E_SCS_SMA_CMD_NO_OWNER                                (SCS_SMA_ERROR_CODE_BASE | 0x0007)       /* BLK配置不允许在非Owner 端控制器进行*/
#define E_SCS_SMA_SYSTEM_BUSY                                 (SCS_SMA_ERROR_CODE_BASE | 0x0008)       /* 系统忙，不允许配置*/
#define E_SCS_SMA_CMD_NO_SUPPORT                              (SCS_SMA_ERROR_CODE_BASE | 0x0009)       /* 该命令不支持 */
#define E_SCS_SMA_RCC_INSTABILITY                             (SCS_SMA_ERROR_CODE_BASE | 0x000A)       /* RCC通道不稳定 */
#define E_SCS_SMA_MSG_MAXIMUM                                 (SCS_SMA_ERROR_CODE_BASE | 0x000B)       /* 请求消息超过直达长度*/

//SCS API异常
#define E_SCS_API_MAPGRP_LUN                                  (SCS_SMA_ERROR_CODE_BASE | 0x0200)       /* 该LUN  不存在于任何映射组*/
#define E_SCS_API_MAPGRP_MAX_NUM                              (SCS_SMA_ERROR_CODE_BASE | 0x0201)       /* 系统中映射组的数目已经达到最大值 */
#define E_SCS_API_MAPGRP_EXIST                                (SCS_SMA_ERROR_CODE_BASE | 0x0202)       /* 映射组已经存在 */
#define E_SCS_API_MAPGRP_WRITE_CFG                            (SCS_SMA_ERROR_CODE_BASE | 0x0203)       /* 映射组信息写文件失败 */
#define E_SCS_API_MAPGRP_LUN_OR_HOST                          (SCS_SMA_ERROR_CODE_BASE | 0x0204)       /* 映射组中存在LUN或者HOST，不允许删除 */
#define E_SCS_API_MAPGRP_NOEXIST                              (SCS_SMA_ERROR_CODE_BASE | 0x0205)       /* 映射组不存在 */
#define E_SCS_API_MAPGRP_LUN_MAX_NUM                          (SCS_SMA_ERROR_CODE_BASE | 0x0207)       /* 映射组中LUN数目达到最大 */
#define E_SCS_API_MAPGRP_VOL_EXIST                            (SCS_SMA_ERROR_CODE_BASE | 0x0208)       /* 映射组中该卷已经存在 */
#define E_SCS_API_MAPGRP_LUN_NOEXIST                          (SCS_SMA_ERROR_CODE_BASE | 0x0209)       /* 映射组中LUN不存在 */
#define E_SCS_API_MAPGRP_HOST_EXIST                           (SCS_SMA_ERROR_CODE_BASE | 0x020A)       /* 该Host已经存在于某个映射组中 */
#define E_SCS_API_MAPGRP_HOST_MAX_NUM                         (SCS_SMA_ERROR_CODE_BASE | 0x020B)       /* 映射组中HOST数目达到最大  */
#define E_SCS_API_MAPGRP_HOST_NOEXIST                         (SCS_SMA_ERROR_CODE_BASE | 0x020C)       /* 映射组中HOST不存在*/
#define E_SCS_API_MAPGRP_HOST                                 (SCS_SMA_ERROR_CODE_BASE | 0x020D)       /* 该HOST 不存在于任何映射组 */
#define E_SCS_API_MAPGRP_ID_INVALID                           (SCS_SMA_ERROR_CODE_BASE | 0x020E)       /* 映射组ID 非法*/
#define E_SCS_API_MAPGRP_NAME_EXIST                           (SCS_SMA_ERROR_CODE_BASE | 0x020F)       /* 映射组名称已经存在*/
#define E_SCS_API_MAPGRP_MAX_LUN_EXIST                        (SCS_SMA_ERROR_CODE_BASE | 0x0210)       /*卷所允许加入的映射组数目已达最大值*/

#define E_SCS_API_NAME_INVALID                                (SCS_SMA_ERROR_CODE_BASE | 0x0215)       /* 对象名称不合法*/
#define E_SCS_API_VOL_ADD_MAPGRP                              (SCS_SMA_ERROR_CODE_BASE | 0x0216)       /* 卷加入映射组时禁止冻结 */

#define E_SCS_API_LUN_EXIST                                   (SCS_SMA_ERROR_CODE_BASE | 0x0220)       /* LUN已经存在 */
#define E_SCS_API_LUN_WRITE_CFG                               (SCS_SMA_ERROR_CODE_BASE | 0x0221)       /* LUN信息写配置文件失败 */
#define E_SCS_API_LUN_NOEXIST                                 (SCS_SMA_ERROR_CODE_BASE | 0x0222)       /* LUN不存在  */
#define E_SCS_API_LUN_GLOBAL_ID_INVALID                       (SCS_SMA_ERROR_CODE_BASE | 0x0223)       /* LUN全局ID 无效*/
#define E_SCS_API_LUN_ID_INVALID                              (SCS_SMA_ERROR_CODE_BASE | 0x0224)       /* LUN在映射组中的ID 无效*/
#define E_SCS_API_LUN_ID_EXIST                                (SCS_SMA_ERROR_CODE_BASE | 0x0225)       /* LUN在映射组中的ID 已经存在*/
#define E_SCS_API_LUN_ADDED_TO_MAPGRP                         (SCS_SMA_ERROR_CODE_BASE | 0x0226)       /* LUN已经被加入到映射组*/
#define E_SCS_API_LUN_VOL_OFFLINE                             (SCS_SMA_ERROR_CODE_BASE | 0x0227)       /* LUN对应的卷不在线*/

#define E_SCS_API_VOL_MAP_ABNORMAL                            (SCS_SMA_ERROR_CODE_BASE | 0x0230)       /* VOL与LUN映射异常(SCS 数据区中邋VOL不存在)  */
#define E_SCS_API_VOL_NUM_MAXIMUM                             (SCS_SMA_ERROR_CODE_BASE | 0x0231)       /* VOL(包括快照卷、克隆卷)数量达到最大 */
#define E_SCS_API_VOL_ADDED_TO_MAPGRP                         (SCS_SMA_ERROR_CODE_BASE | 0x0232)       /* Vol已经被加入到映射组*/
#define E_SCS_API_VOL_DEVICE_OPEN_FAILED                      (SCS_SMA_ERROR_CODE_BASE | 0x0233)       /* Vol设备打开失败(创建LUN失败)*/
#define E_SCS_API_VOL_DEVICE_CLOSE_FAILED                     (SCS_SMA_ERROR_CODE_BASE | 0x0234)       /* Vol设备关闭失败(删除LUN失败)*/
#define E_SCS_API_VOL_SINGLE_WRITE_BACK                       (SCS_SMA_ERROR_CODE_BASE | 0x0235)       /* 单控且写回策略，不支持创建vol*/
#define E_SCS_API_VOL_MIRROR_OFF_WRITE_BACK                   (SCS_SMA_ERROR_CODE_BASE | 0x0236)       /* 已配置为关镜像，不允许再创建写回策略的vol*/
#define E_SCS_API_VOL_WRITE_BACK_IN_SINGLE                    (SCS_SMA_ERROR_CODE_BASE | 0x0237)       /* 单控情况下，配置写回策略时，不支持 */
#define E_SCS_API_VOL_MIRROR_OFF_WRITE_BACK_ERROR             (SCS_SMA_ERROR_CODE_BASE | 0x0238)       /* 已配置为关镜像，不允许配置写回策略的vol*/
#define E_SCS_API_VOL_VD_FREEZE                               (SCS_SMA_ERROR_CODE_BASE | 0x0239)       /* 卷所在vd冻结，不允许vol加入映射组*/
#define E_SCS_API_SET_VOL_CACHE_ATTR_FAILED                   (SCS_SMA_ERROR_CODE_BASE | 0x023A)       /* 设置卷的cache策略失败 */
#define E_SCS_API_SEND_SNAPSHOT_AGENT_FAILED                  (SCS_SMA_ERROR_CODE_BASE | 0x023B)       /* 发送快照代理失败 */
#define E_SCS_API_VOL_EXIST_ABNORMAL                          (SCS_SMA_ERROR_CODE_BASE | 0x023C)       /* 双控环境中卷对象单端可见*/
#define E_SCS_API_FIND_AGENT_NODE_FAILED                      (SCS_SMA_ERROR_CODE_BASE | 0x023D)       /* 查找快照代理节点失败 */
#define E_SCS_API_RCV_AGENT_RSP_FAILED                        (SCS_SMA_ERROR_CODE_BASE | 0x023E)       /* 接收快照代理响应失败 */
#define E_SCS_API_AGENT_EXECUTE_FAILED                        (SCS_SMA_ERROR_CODE_BASE | 0x023F)       /* 快照代理端操作失败 */

#define E_SCS_API_OTHER_WRITE_CFG                             (SCS_SMA_ERROR_CODE_BASE | 0x0240)       /* 其他类信息写文件失败 */
#define E_SCS_API_OTHER_READ_CFG                              (SCS_SMA_ERROR_CODE_BASE | 0x0241)       /* 读文件失败 */
#define E_SCS_API_OTHER_GET_NET_INFO                          (SCS_SMA_ERROR_CODE_BASE | 0x0242)       /* 获取网络设备信息失败*/
#define E_SCS_API_OTHER_NET_IP_ERROR                          (SCS_SMA_ERROR_CODE_BASE | 0x0243)       /* IP 地址错误*/
#define E_SCS_API_OTHER_NET_MASK_ERROR                        (SCS_SMA_ERROR_CODE_BASE | 0x0244)       /* 子网掩码错误 */
#define E_SCS_API_PEERSTATE_NO_RUNNING                        (SCS_SMA_ERROR_CODE_BASE | 0x0245)       /* 单控状态，不允许设置*/
#define E_SCS_API_OTHER_NET_IP_EXIST                          (SCS_SMA_ERROR_CODE_BASE | 0x0246)       /*IP 在系统已经存在*/
#define E_SCS_API_OTHER_NET_MAC_EXIST                         (SCS_SMA_ERROR_CODE_BASE | 0x0247)       /*Mac 在系统中已经存在*/
#define E_SCS_API_IP_OR_NETMASK_ERROR                         (SCS_SMA_ERROR_CODE_BASE | 0x0248)       /*IP地址主机位错误或子网掩码错误*/
#define E_SCS_API_ROLETYPE_ERROR                              (SCS_SMA_ERROR_CODE_BASE | 0x0249)       /*设备类型错误*/
#define E_SCS_API_DEVICEID_ERROR_FOR_ROLETYPE                 (SCS_SMA_ERROR_CODE_BASE | 0x024A)       /*该角色类型下，设备编号错误*/
#define E_SCS_API_GATEWAY_ERROR                               (SCS_SMA_ERROR_CODE_BASE | 0x024B)       /*网关错误*/
#define E_SCS_API_NETCFG_ERROR                                (SCS_SMA_ERROR_CODE_BASE | 0x024C)       /*网络配置信息不能为空*/
#define E_SCS_API_DEVICE_ERROR                                (SCS_SMA_ERROR_CODE_BASE | 0x024D)       /*设备编号错误*/
#define E_SCS_API_IP_MASK_BOTH_ZERO                           (SCS_SMA_ERROR_CODE_BASE | 0x024E)       /*业务口 的IP和掩码必须同时配置*/
#define E_SCS_API_IP_HOSTBITS_ERROR                           (SCS_SMA_ERROR_CODE_BASE | 0x024F)       /*IP地址的主机位不能全0或全1*/
#define E_SCS_API_NETWORKSEGMENT_ERROR                        (SCS_SMA_ERROR_CODE_BASE | 0x0250)       /* 网段配置错误 */
#define E_SCS_API_NETWORKSEGMENT_MANAGEMENT                   (SCS_SMA_ERROR_CODE_BASE | 0x0251)       /* 管理口网段相关 */
#define E_SCS_API_NETWORKSEGMENT_BUSYNESS                     (SCS_SMA_ERROR_CODE_BASE | 0x0252)       /* 业务口网段相关 */

#define E_SCS_API_OTHER_MAC_INVALID                           (SCS_SMA_ERROR_CODE_BASE | 0x0253)       /*Mac 非法*/
#define E_SCS_API_OTHER_SET_MAC_BY_SRV                        (SCS_SMA_ERROR_CODE_BASE | 0x0254)       /*调用soda的Mac 服务包接口设置Mac 失败*/
#define E_SCS_API_OTHER_WRITE_MAC_EEPROM_ERROR                (SCS_SMA_ERROR_CODE_BASE | 0x0255)       /*调用soda的LSSP服务包接口写Mac 到EEPROM失败*/
#define E_SCS_API_OTHER_GET_PORT_MTU_MAX_VALUE                (SCS_SMA_ERROR_CODE_BASE | 0x0256)       /*获取端口的MTU 最大值失败*/
#define E_SCS_API_OTHER_PORT_MTU_INVALID                      (SCS_SMA_ERROR_CODE_BASE | 0x0257)       /*端口的MTU值非法*/
#define E_SCS_API_OTHER_ROUTE_GATEWAY_ERROR                   (SCS_SMA_ERROR_CODE_BASE | 0x0258)       /* 路由的网关不合法*/
#define E_SCS_API_OTHER_ROUTE_NO_EXIST                        (SCS_SMA_ERROR_CODE_BASE | 0x0259)       /* 路由不存在*/

#define E_SCS_API_OTHER_NET_MANA_IP_NULL                      (SCS_SMA_ERROR_CODE_BASE | 0x025A)       /* 管理口IP 不能配置为空*/
#define E_SCS_API_OTHER_NET_IP_RESERVED                       (SCS_SMA_ERROR_CODE_BASE | 0x025B)       /* 预留IP(对端管理口的默认IP),不允许配置*/

#define E_SCS_API_HDD_ABNORMAL                                (SCS_SMA_ERROR_CODE_BASE | 0x0260)       /* 系统中，磁盘异常 */
#define E_SCS_API_HDD_NOEXIST                                 (SCS_SMA_ERROR_CODE_BASE | 0x0261)       /* 磁盘信息不存在*/
#define E_SCS_API_HDD_UNUSED                                  (SCS_SMA_ERROR_CODE_BASE | 0x0262)       /* 磁盘非空闲状态 */
#define E_SCS_API_HDD_HOTSPARE                                (SCS_SMA_ERROR_CODE_BASE | 0x0263)       /* 磁盘非热备状态  */
#define E_SCS_API_HDD_DATA                                    (SCS_SMA_ERROR_CODE_BASE | 0x0264)       /* 磁盘为数据盘*/
#define E_SCS_API_HDD_NO_LOCAL                                (SCS_SMA_ERROR_CODE_BASE | 0x0265)       /* 磁盘不是本地盘*/
#define E_SCS_API_HDD_HEALTH_STATE                            (SCS_SMA_ERROR_CODE_BASE | 0x0266)       /* 磁盘健康状态异常*/
#define E_SCS_API_HDD_NO_FOREIGN                              (SCS_SMA_ERROR_CODE_BASE | 0x0267)       /* 磁盘不是外来盘*/
#define E_SCS_API_HDD_SET_POWER_MODE_ERROR                    (SCS_SMA_ERROR_CODE_BASE | 0x0268)       /* 设置磁盘能耗模式失败*/
#define E_SCS_API_HDD_CAPACITY                                (SCS_SMA_ERROR_CODE_BASE | 0x0269)       /* 磁盘容量小于VD最小成员盘容量 */
#define E_SCS_API_HDD_TYPE_UNFIT                              (SCS_SMA_ERROR_CODE_BASE | 0x026A)       /* 磁盘类型不符合 */
#define E_SCS_API_HDD_DATA_IMPORT                             (SCS_SMA_ERROR_CODE_BASE | 0x026B)       /* 外来数据盘不允许导入，可通过VD漫游迁入处理 */
#define E_SCS_API_HDD_SCANNING                                (SCS_SMA_ERROR_CODE_BASE | 0x026C)       /* 磁盘在扫描，不允许进行磁盘类型转换 */
#define E_SCS_API_HDD_NOT_SCANNING                            (SCS_SMA_ERROR_CODE_BASE | 0x026D)       /* 磁盘扫描任务未开启 */
#define E_SCS_API_ENOUGH_FIT_DISK                             (SCS_SMA_ERROR_CODE_BASE | 0x026E)       /* 没有足够的合适磁盘用来创建虚拟盘 */
#define E_SCS_API_HDD_ABNORMAL_SCAN                           (SCS_SMA_ERROR_CODE_BASE | 0x026F)       /* 磁盘异常，不能启动扫描*/

#define E_SCS_API_PD_SCANNING                                 (SCS_SMA_ERROR_CODE_BASE | 0x0270)       /* 磁盘在扫描，不允许创建虚拟盘 */
#define E_SCS_API_PD_SCAN_DATA_ERROR                          (SCS_SMA_ERROR_CODE_BASE | 0x0271)       /* 数据盘不允许扫描*/
#define E_SCS_API_PD_SCAN_UNKNOWN_ERROR                       (SCS_SMA_ERROR_CODE_BASE | 0x0272)       /* 未知盘不允许扫描*/
#define E_SCS_API_HDD_NOT_READY                               (SCS_SMA_ERROR_CODE_BASE | 0x0273)       /* 磁盘未准备就绪*/
#define E_SCS_API_HDD_SLOT_ABNORMAL                           (SCS_SMA_ERROR_CODE_BASE | 0x0274)       /* 磁盘槽位异常(双控信息不一致)*/
#define E_SCS_API_DISK_SCAN                                   (SCS_SMA_ERROR_CODE_BASE | 0x0275)       /* 磁盘上有扫描任务，不允许执行该操作 */

#define E_SCS_API_VD_NO_DEGRADE                               (SCS_SMA_ERROR_CODE_BASE | 0x0280)       /* VD非降级状态，不允许重建 */
#define E_SCS_API_VD_MEMBER_LIST_INCOMPATIBLE                 (SCS_SMA_ERROR_CODE_BASE | 0x0281)       /* VD两端控制器成员盘列表不一致也不呈子集关系 */
#define E_SCS_API_VD_MEMBER_LIST_NOT_SAME                     (SCS_SMA_ERROR_CODE_BASE | 0x0282)       /* VD两端控制器成员盘列表不一致 */
#define E_SCS_API_VD_STATE_NOT_SAME                           (SCS_SMA_ERROR_CODE_BASE | 0x0283)       /* VD两端控制器状态不相同 */
#define E_SCS_API_VD_LOCAL_NOEXIST                            (SCS_SMA_ERROR_CODE_BASE | 0x0284)       /* VD本端不存在 */
#define E_SCS_API_VD_PEER_NOEXIST                             (SCS_SMA_ERROR_CODE_BASE | 0x0285)       /* VD对端不存在 */
#define E_SCS_API_VD_FAULT                                    (SCS_SMA_ERROR_CODE_BASE | 0x0286)       /* VD为fault状态，不允许修改缓存策略 */
#define E_SCS_API_VD_IS_RECING                                (SCS_SMA_ERROR_CODE_BASE | 0x0287)       /* VD正在降级重建中 */
#define E_SCS_API_VD_HOTSPARE_MAX                             (SCS_SMA_ERROR_CODE_BASE | 0x0288)       /* 一次最多添加四个热备盘参加重建*/
#define E_SCS_API_VD_NOEXIST                                  (SCS_SMA_ERROR_CODE_BASE | 0x0289)       /* VD两端不存在 */
#define E_SCS_API_VD_LEVEL                                    (SCS_SMA_ERROR_CODE_BASE | 0x028A)       /* VD级别为0，不允许重建 */
#define E_SCS_API_VD_DELAY_RECOVERING                         (SCS_SMA_ERROR_CODE_BASE | 0x028B)       /* VD延迟恢复，不允许重启 */
#define E_SCS_NO_ENOUGH_UNUSED_DISKS                          (SCS_SMA_ERROR_CODE_BASE | 0x028C)       /* 没有足够的空闲盘，无法创建虚拟盘(存储池) */
#define E_SCS_NO_ENOUGH_UNUSED_PD                             (SCS_SMA_ERROR_CODE_BASE | 0x028D)       /* 没有足够的空闲盘 */
#define E_SCS_NO_ENOUGH_RIGHT_UNUSED                          (SCS_SMA_ERROR_CODE_BASE | 0x028E)       /* 没有足够可用的空闲盘 */

#define E_SCS_API_MIRROR_DISK_USED                            (SCS_SMA_ERROR_CODE_BASE | 0x0290)       /* 远端磁盘已经被使用 */
#define E_SCS_API_MIRROR_DISK_NOEXIST                         (SCS_SMA_ERROR_CODE_BASE | 0x0291)       /* 远端磁盘不存在*/
#define E_SCS_API_MIRROR_CONNECT_NOEXIST                      (SCS_SMA_ERROR_CODE_BASE | 0x0292)       /* 镜像连接不存在*/
#define E_SCS_API_MIRROR_CONNECT_MAX                          (SCS_SMA_ERROR_CODE_BASE | 0x0293)       /* 镜像连接数目达到最大值*/
#define E_SCS_API_MIRROR_DISK_MAX                             (SCS_SMA_ERROR_CODE_BASE | 0x0294)       /* 远端磁盘数目达到最大值*/
#define E_SCS_API_NEXUS_MIRROR_EXIST                          (SCS_SMA_ERROR_CODE_BASE | 0x0295)       /* 远程连接上有镜像存在，不允许删除*/
#define E_SCS_API_FC_CONNECT_EXIST                            (SCS_SMA_ERROR_CODE_BASE | 0x0296)       /* 镜像连接已经存在*/
#define E_SCS_API_FC_CONNECT_MAX_CTRL                         (SCS_SMA_ERROR_CODE_BASE | 0x0297)       /* 该控制器上的镜像连接数目达到最大值*/
#define E_SCS_API_MIRROR_DISK_ABNORMAL                        (SCS_SMA_ERROR_CODE_BASE | 0x0298)       /* 远端磁盘状态异常*/

#define E_SCS_API_VOL_MAPPED                                  (SCS_SMA_ERROR_CODE_BASE | 0x02A0)       /* VOL已经被映射,不允许执行删除操作 */
#define E_SCS_API_VOL_NOT_GOOD                                (SCS_SMA_ERROR_CODE_BASE | 0x02A1)       /* VOL非“GOOD”状态 */
#define E_SCS_API_VOL_NOT_NORMAL_VOL                          (SCS_SMA_ERROR_CODE_BASE | 0x02A2)       /* 非普通卷*/
#define E_SCS_API_CVOL_INIT_RENAME_ERROR                      (SCS_SMA_ERROR_CODE_BASE | 0x02A3)       /* 克隆卷处于初始同步状态中，不允许重命名此克隆卷*/
#define E_SCS_API_CVOL_SYNCING_RENAME_ERROR                   (SCS_SMA_ERROR_CODE_BASE | 0x02A4)       /* 克隆卷处于同步中，不允许重命名此克隆卷*/
#define E_SCS_API_CVOL_REVERSE_SYNCING_RENAME_ERROR           (SCS_SMA_ERROR_CODE_BASE | 0x02A5)       /* 克隆卷处于反同步中，不允许重命名此克隆卷*/
#define E_SCS_API_VOL_STATE_NOT_SAME                          (SCS_SMA_ERROR_CODE_BASE | 0x02A6)       /* Vol两端控制器状态不相同 */

#define E_SCS_API_SVOL_NOT_ACTIVE                             (SCS_SMA_ERROR_CODE_BASE | 0x02B0)       /* SVOL非“ACTIVE”状态 */
#define E_SCS_API_CVOL_RELATION                               (SCS_SMA_ERROR_CODE_BASE | 0x02B1)       /* 克隆卷是关联状态 */
#define E_SCS_API_CVOL_VD_FREEZE                              (SCS_SMA_ERROR_CODE_BASE | 0x02B2)       /* 克隆卷所在vd是冻结状态，不允许将clone加入映射组 */
#define E_SCS_API_SVOL_VD_FREEZE                              (SCS_SMA_ERROR_CODE_BASE | 0x02B3)       /* 快照卷所在vd是冻结状态，不允许 svol 加入映射组 */
#define E_SCS_API_CVOL_ADDED_TO_MAPGRP                        (SCS_SMA_ERROR_CODE_BASE | 0x02B4)       /* CVol已经被加入到映射组*/

#define E_SCS_API_VOL_VD_FOREIGN                              (SCS_SMA_ERROR_CODE_BASE | 0x02B5)       /* 外来vd 中的卷，不允许vol加入映射组*/
#define E_SCS_API_CVOL_VD_FOREIGN                             (SCS_SMA_ERROR_CODE_BASE | 0x02B6)       /*  外来vd 中的克隆卷，不允许将clone加入映射组 */
#define E_SCS_API_SVOL_VD_FOREIGN                             (SCS_SMA_ERROR_CODE_BASE | 0x02B7)       /*  外来vd 中的快照，不允许 svol 加入映射组 */


#define E_SCS_API_ISCSI_INIT_MAX_NUM                          (SCS_SMA_ERROR_CODE_BASE | 0x02C0)       /* Initiator 的数目超过最大值 */
#define E_FC_SUBCARD_NOT_EXIST                                (SCS_SMA_ERROR_CODE_BASE | 0x02C1)       /* FC子卡不存在*/

#define E_SCS_API_CACHE_STATUS_ERROR                          (SCS_SMA_ERROR_CODE_BASE | 0x02D0)       /* Cache 统计状态错误 */
#define E_SCS_API_IOLUN_STATUS_ERROR                          (SCS_SMA_ERROR_CODE_BASE | 0x02D1)       /* IOLun 统计状态错误 */
#define E_SCS_API_ISCSI_STATUS_ERROR                          (SCS_SMA_ERROR_CODE_BASE | 0x02D2)       /* ISCSI 统计状态错误 */
#define E_SCS_API_CACHE_STATUS_OFF                            (SCS_SMA_ERROR_CODE_BASE | 0x02D3)       /* Cache 统计状态为OFF */
#define E_SCS_API_IOLUN_STATUS_OFF                            (SCS_SMA_ERROR_CODE_BASE | 0x02D4)       /* IOLun 统计状态为OFF */
#define E_SCS_API_ISCSI_STATUS_OFF                            (SCS_SMA_ERROR_CODE_BASE | 0x02D5)       /* ISCSI 统计状态为OFF */
#define E_SCS_API_ISCSI_NEWNAME_INVALID                       (SCS_SMA_ERROR_CODE_BASE | 0x02D6)       /* New ISCSI Name 非法*/
#define E_SCS_ISCSI_NEWNAME_SAME_PERR                         (SCS_SMA_ERROR_CODE_BASE | 0x02D7)       /* New ISCSI Name 与对端一样*/

/* add by tao 2012-05-22 */
#define E_SCS_API_VOL_WRITE_BACK_FAN_FAULT                    (SCS_SMA_ERROR_CODE_BASE | 0x02D9)
#define E_SCS_API_VOL_WRITE_BACK_BBU_FAULT                    (SCS_SMA_ERROR_CODE_BASE | 0x02DA)
#define E_SCS_API_VOL_WRITE_BACK_POWER_FAULT                  (SCS_SMA_ERROR_CODE_BASE | 0x02DB)

/* 版本管理错误码 added by wangjing 2011-08-22 */
#define E_SCS_API_SYNC_FILE_FAILED                            (SCS_VMM_ERROR_CODE_BASE | 0x0400)       /* 同步文件到对端失败 */
#define E_SCS_API_GET_UPLOAD_PROGRESS_FAILED                  (SCS_VMM_ERROR_CODE_BASE | 0x0401)       /* 查询文件上传进度失败 */
#define E_SCS_API_CANCEL_UPLOAD_FAILED                        (SCS_VMM_ERROR_CODE_BASE | 0x0402)       /* 取消文件上传失败 */
#define E_SCS_API_VER_FILE_CHK_FAILED                         (SCS_VMM_ERROR_CODE_BASE | 0x0403)       /* 版本文件校验失败 */
#define E_SCS_API_UPDATE_FILE_FAILED                          (SCS_VMM_ERROR_CODE_BASE | 0x0404)       /* 更新文件版本失败 */
#define E_SCS_API_GET_VERSION_FAILED                          (SCS_VMM_ERROR_CODE_BASE | 0x0405)       /* 查询文件版本失败 */
#define E_SCS_API_VERSION_FILE_NOT_EXIST                      (SCS_VMM_ERROR_CODE_BASE | 0x0406)       /* 版本文件不存在 */
#define E_SCS_API_FILE_PATH_NOT_EXIST                         (SCS_VMM_ERROR_CODE_BASE | 0x0407)       /* 文件路径不存在 */
#define E_SCS_API_NETWORK_NOT_REACHABLE                       (SCS_VMM_ERROR_CODE_BASE | 0x0408)       /* 网络不可达 */
#define E_SCS_API_USER_PASSWORD_NOT_CORRECT                   (SCS_VMM_ERROR_CODE_BASE | 0x0409)       /* 用户名或密码不正确 */
#define E_SCS_API_INVALID_FILE_SIZE                           (SCS_VMM_ERROR_CODE_BASE | 0x040A)       /* 文件大小不合法 */
#define E_SCS_API_FILENAME_REPEATED                           (SCS_VMM_ERROR_CODE_BASE | 0x040B)       /* 文件名重复，不正确 */
#define E_SCS_API_WRONG_VERTYPE                               (SCS_VMM_ERROR_CODE_BASE | 0x040C)       /* 错误的文件类型 */
#define E_SCS_API_FTP_GETFILEDATA_FAIL                        (SCS_VMM_ERROR_CODE_BASE | 0x040D)       /* FTP获取数据时失败 */
#define E_SCS_API_FTP_PUTFILEDATA_FAIL                        (SCS_VMM_ERROR_CODE_BASE | 0x040E)       /* FTP获取数据时失败 */
#define E_SCS_API_FILE_EXIST_DUAL                             (SCS_VMM_ERROR_CODE_BASE | 0x040F)       /* 文件在本端和对端均已存在 */
#define E_SCS_API_FILE_EXIST_LOCAL                            (SCS_VMM_ERROR_CODE_BASE | 0x0410)       /* 文件在本端已存在 */
#define E_SCS_API_FILE_EXIST_PEER                             (SCS_VMM_ERROR_CODE_BASE | 0x0411)       /* 文件在对端已存在 */
#define E_SCS_API_FILE_SIZE_TOO_BIG                           (SCS_VMM_ERROR_CODE_BASE | 0x0412)       /* 文件体积太大 */
#define E_SCS_API_VERSION_NUMBER_NOT_MATCH                    (SCS_VMM_ERROR_CODE_BASE | 0x0413)       /* 两端版本号不匹配 */
#define E_SCS_API_FILE_SIZE_NOT_MATCH                         (SCS_VMM_ERROR_CODE_BASE | 0x0414)       /* 两端文件大小不匹配 */
#define E_SCS_API_UPLOAD_TASK_IS_RUNNING                      (SCS_VMM_ERROR_CODE_BASE | 0x0415)       /* 已经有FTP任务在运行 */
#define E_SCS_API_GET_DOWNLOAD_PROGRESS_FAILED                (SCS_VMM_ERROR_CODE_BASE | 0x0416)       /* 查询文件下载进度失败 */
#define E_SCS_API_GET_VERSION_BUSY                            (SCS_VMM_ERROR_CODE_BASE | 0x0417)       /* 版本升级中，查询忙返回*/
#define E_SCS_API_PEER_FILE_SIZE_TOO_BIG                      (SCS_VMM_ERROR_CODE_BASE | 0x0418)       /* 文件大小太大，超过FLASH剩余空间 */
#define E_SCS_API_VERSION_NETBOOT_REWRITE                     (SCS_VMM_ERROR_CODE_BASE | 0x0419)       /* 正在版本回写或备份操作---改为:系统忙，请稍后尝试 */
#define E_SCS_API_TOPO_ERROR                                  (SCS_VMM_ERROR_CODE_BASE | 0x0420)       /* 拓扑不完整，不允许更新8005固件*/
#define E_SCS_API_WRONG_PM8005_TYPE                           (SCS_VMM_ERROR_CODE_BASE | 0x0421)       /* 错误的8005版本类型，使用了prokit版本更新 */

#define E_SCS_API_FILE_NAME_INVAILD                      (SCS_SMA_ERROR_CODE_BASE | 0x0460)          /* 文件名不合法 */
#define E_SCS_API_FILE_INCONSISTENCY                     (SCS_SMA_ERROR_CODE_BASE | 0x0461)          /* 两端待更新版本文件名称不一致 */
#define E_SCS_API_FILE_NAME_TOO_LONG                     (SCS_SMA_ERROR_CODE_BASE | 0x0462)          /* 文件名太长 */
#define E_SCS_API_SAMENAME_WITH_CURVER                   (SCS_SMA_ERROR_CODE_BASE | 0x0463)          /* 与当前版本同名 */
#define E_SCS_API_BGTASK_RUNNING                         (SCS_SMA_ERROR_CODE_BASE | 0x0464)          /* 有后台任务运行,禁止版本升级 */
/*后台任务和计划任务相关的错误码*/
#define   E_SCS_API_TASK_SET_NOT_ALLOWED                 (SCS_SMA_ERROR_CODE_BASE | 0x0420)           /*该配置与后台任务的状态冲突，不允许该项操作*/
#define   E_SCS_API_TASK_SET_NOT_SUPPORTED               (SCS_SMA_ERROR_CODE_BASE | 0x0421)           /*该类型任务的设置不支持*/
#define   E_SCS_API_TASK_TYPE_NOT_EXIST                  (SCS_SMA_ERROR_CODE_BASE | 0x0422)           /* 任务类型不支持*/
#define   E_SCS_API_TASK_EXIST                           (SCS_SMA_ERROR_CODE_BASE | 0x0423)           /* 这项任务已经存在 */
#define   E_SCS_API_TASK_NOT_EXIST                       (SCS_SMA_ERROR_CODE_BASE | 0x0424)           /* 这项任务不存在 */
#define   E_SCS_API_TASK_NOT_RUNNING                     (SCS_SMA_ERROR_CODE_BASE | 0x0425)           /* 任务非运行状态 */
#define   E_SCS_API_SCHEDULE_OUTDATE                     (SCS_SMA_ERROR_CODE_BASE | 0x0426)           /* 计划任务过期 */
#define   E_SCS_API_SCHEDULE_TASKID_INVALID              (SCS_SMA_ERROR_CODE_BASE | 0x0427)           /* 计划任务序号无效，该任务不存在*/
#define   E_SCS_API_SCHEDULE_MAX_NUM                     (SCS_SMA_ERROR_CODE_BASE | 0x0428)           /* 计划任务达到最大数目*/
#define   E_SCS_API_TASK_RUNNING_NUM_UP_LIMIT            (SCS_SMA_ERROR_CODE_BASE | 0x0429)           /* 正在运行的任务已达最大门限 */
#define   E_SCS_API_SCHEDULE_EXIST                       (SCS_SMA_ERROR_CODE_BASE | 0x042B)           /* 这项计划任务已经存在 */
#define   E_SCS_API_TASK_MAX_NUM                         (SCS_SMA_ERROR_CODE_BASE | 0x042C)           /* 后台任务达到最大数目*/
#define   E_SCS_API_STOP_HOTSPARE_SCAN_FAIL              (SCS_SMA_ERROR_CODE_BASE | 0x042D)           /* 停止热备盘的扫描任务失败*/

/* 配置文件导入导出错误码 add by tao 2011-08-15 */
#define E_SCS_API_FILE_EXIST                             (SCS_SMA_ERROR_CODE_BASE | 0x0500)
#define E_SCS_API_FILE_NO_EXIST                          (SCS_SMA_ERROR_CODE_BASE | 0x0501)
#define E_SCS_API_EXPORT_CFG_FILE_FAILED                 (SCS_SMA_ERROR_CODE_BASE | 0x0502)
#define E_SCS_API_IMPORT_CFG_FILE_FAILED                 (SCS_SMA_ERROR_CODE_BASE | 0x0503)
#define E_SCS_API_FILE_CP_FAILED                          (SCS_SMA_ERROR_CODE_BASE | 0x0504)


/* 配置文件抽象子层错误码 add by tao 2011-08-15 */
#define E_SCS_CFG_FILE_CHECK_ERR                (SCS_CFG_FILE_ERROR_CODE_BASE | 0x0001)
#define E_SCS_CFG_FILE_OPEN_FAILED              (SCS_CFG_FILE_ERROR_CODE_BASE | 0x0002)

//SCS 用户& 鉴权管理异常
//GENERAL_ERROR
#define E_SMA_PARAM_WRONG    (SCS_SMA_ERROR_CODE_BASE | 0x0600)      // param invalid
#define E_SMA_MALLOC_ERROR   (SCS_SMA_ERROR_CODE_BASE | 0x042A)

//USER_LOGIN
#define E_SMA_USER_WRONG          (SCS_SMA_ERROR_CODE_BASE | 0X0601) // username or pwd is incorrect
#define E_SMA_USER_LOGINED        (SCS_SMA_ERROR_CODE_BASE | 0X0602) // user is logined by other
#define E_SMA_USER_EXIST          (SCS_SMA_ERROR_CODE_BASE | 0X0603) // username or pwd is exist
#define E_SMA_USER_NEXIST         (SCS_SMA_ERROR_CODE_BASE | 0X0604)  // username or pwd is not exit
#define E_SMA_ADMIN               (SCS_SMA_ERROR_CODE_BASE | 0X0605) // supervisor level user has login
#define E_SMA_COMMON              (SCS_SMA_ERROR_CODE_BASE | 0X0606) // supervisor level user has login
//USER_AUTHOR
#define E_SMA_USER_AUTHOR         (SCS_SMA_ERROR_CODE_BASE | 0X0607)  // user does not have enough authority
#define E_SMA_AUTHEN              (SCS_SMA_ERROR_CODE_BASE | 0X0608)  // user has not been authenticated
#define E_SMA_PASSWD              (SCS_SMA_ERROR_CODE_BASE | 0X0609)  // Old password not match.
//USER_MGT
#define E_SMA_UNKNOW              (SCS_SMA_ERROR_CODE_BASE | 0X060A)  // unknow command or packet
#define E_SMA_COMM_USER_OVERFLOW  (SCS_SMA_ERROR_CODE_BASE | 0X060B)  // user number overflow
#define E_SMA_GUEST_USER_OVERFLOW (SCS_SMA_ERROR_CODE_BASE | 0X060C)  // user number overflow
#define E_SMA_USER_OVERFLOW       (SCS_SMA_ERROR_CODE_BASE | 0X060D)  // user number overflow
#define E_SMA_ADMIN_DEL           (SCS_SMA_ERROR_CODE_BASE | 0X060E)  // cannot delete admin
#define E_SMA_ONLINE_DEL          (SCS_SMA_ERROR_CODE_BASE | 0X060F)  // cannot delete online account
//USER_EMAIL
#define E_SMA_SMTP_BUSY           (SCS_SMA_ERROR_CODE_BASE | 0X0610)   //SMTP is used by other process. Try again later.
#define E_SMA_USER_NPWDMAIL       (SCS_SMA_ERROR_CODE_BASE | 0X0611)   // PWD MAIL not set
#define E_SMA_NSMTP               (SCS_SMA_ERROR_CODE_BASE | 0X0612)    // SMTP not set
#define E_SMA_FAIL_SEND           (SCS_SMA_ERROR_CODE_BASE | 0X0613)   //  Fail to send mail
//USER_CONNECT
#define E_SMA_CONN_OVERFLOW       (SCS_SMA_ERROR_CODE_BASE | 0X0614)   // Too many connects exit
#define E_SMA_CONN_NEXIT          (SCS_SMA_ERROR_CODE_BASE | 0X0615)   // the connect is not exit
#define E_SMA_CONN_SEARCH_INIT    (SCS_SMA_ERROR_CODE_BASE | 0X0616)  // capi search sma udp init failed
#define E_SMA_CONN_SEARCH_SEND    (SCS_SMA_ERROR_CODE_BASE | 0X0617)   // capi search sma udp send data failed

#define E_SMA_CONN_MSG_HANDLE_SOCKETFD  (SCS_SMA_ERROR_CODE_BASE | 0X0618)   // create Message Handle Socket fd failed
#define E_SMA_CONN_MSG_HANDLE_BLOCK     (SCS_SMA_ERROR_CODE_BASE | 0X0619)   // set Message Handle Socket fd block failed
#define E_SMA_CONN_MSG_HANDLE_SELECT    (SCS_SMA_ERROR_CODE_BASE | 0X061A)   // connect Message Handle process, select failed
#define E_SMA_CONN_MSG_HANDLE_TIMEOUT   (SCS_SMA_ERROR_CODE_BASE | 0X061B)  // connect Message Handle process, timeout
#define E_SMA_CONN_MSG_HANDLE_CONNECT   (SCS_SMA_ERROR_CODE_BASE | 0X061C)    // connect Message Handle process, connect failed
#define E_SMA_CONN_MSG_HANDLE_SEND      (SCS_SMA_ERROR_CODE_BASE | 0X061D)   // send Message to Messgage handle process failed
#define E_SMA_CONN_MSG_HANDLE_RECV      (SCS_SMA_ERROR_CODE_BASE | 0X061E)    // receive the message of Messgage handle process responsing failed
//USER_SESSION
#define E_SMA_SESSION_ATHOR             (SCS_SMA_ERROR_CODE_BASE | 0X061F)   // user does not have enough authority,not login
#define E_SMA_SESSION_NEXIST            (SCS_SMA_ERROR_CODE_BASE | 0X0620)    // Session not exist,maybe timeout
#define E_SMA_ADMIN_EXIST               (SCS_SMA_ERROR_CODE_BASE | 0X0621)    // can't add admin
#define E_SMA_SYS_NOT_FINISH_STARTUP    (SCS_SMA_ERROR_CODE_BASE | 0X0622)    // System have not finish Startup.
#define E_SMA_USER_NEWNAME_HAVE_EXIT    (SCS_SMA_ERROR_CODE_BASE | 0X0623)    // New name is illegal,it has exist in system.

//time

#define E_SCS_TIME_NTP_SERVER_ADDR_NULL          (SCS_SMA_ERROR_CODE_BASE | 0X0700)    // NTP server address is null
#define E_SCS_TIME_NTP_SERVER_ADDR_INVALID       (SCS_SMA_ERROR_CODE_BASE | 0X0701)    // NTP server address is invalid
#define E_SCS_TIME_GET_SYSTEM_TIME_ZONE_FAILED   (SCS_SMA_ERROR_CODE_BASE | 0X0702)    // get system time zone failed
#define E_SCS_TIME_SET_HARDWARE_CLOCK_FAILED     (SCS_SMA_ERROR_CODE_BASE | 0X0703)    // set hardware clock failed
#define E_SCS_TIME_THE_CITY_IS_INVALID           (SCS_SMA_ERROR_CODE_BASE | 0X0704)    // the city is invalid
#define E_SCS_TIME_NO_VALID_CONTINENT_NAME       (SCS_SMA_ERROR_CODE_BASE | 0X0705)    // did not input valid continent name
#define E_SCS_TIME_SET_TIME_ZONE_FAILED          (SCS_SMA_ERROR_CODE_BASE | 0X0706)    // set time zone by index failed
#define E_SCS_TIME_GET_NTP_SERVER_CONFIG_FAILED  (SCS_SMA_ERROR_CODE_BASE | 0X0707)    // get NTP server config failed
#define E_SCS_TIME_SET_NTP_CONFIG_FAILED         (SCS_SMA_ERROR_CODE_BASE | 0X0708)    // set NTP config failed
#define E_SCS_TIME_SET_AUTO_NTP_FAILED           (SCS_SMA_ERROR_CODE_BASE | 0X0709)    // set NTP date script failed
#define E_SCS_TIME_ENTER_TIME_INVALID            (SCS_SMA_ERROR_CODE_BASE | 0X070A)    // enter time is invalid
#define E_SCS_TIME_SYNC_TIME_BY_NTP_FAILED       (SCS_SMA_ERROR_CODE_BASE | 0X070B)    // synchronize system time by ntp failed
#define E_SCS_TIME_WRITE_CFG_REC_FAILED          (SCS_SMA_ERROR_CODE_BASE | 0x070C)    // write config record failed
#define E_SCS_TIME_READ_CFG_REC_FAILED           (SCS_SMA_ERROR_CODE_BASE | 0x070D)    // read config record failed
#define E_SCS_TIME_FORK_FAILED                   (SCS_SMA_ERROR_CODE_BASE | 0X070E)    // fork a new process failed

/* **************************LOG********************************** */
#define E_LOG_PARA_NULL                 (SCS_LOG_ERROR_CODE_BASE | 0X0101)    // point parameters null
#define E_LOG_PARAM_ILLEAG              (SCS_LOG_ERROR_CODE_BASE | 0X0102)    // parameters passed in or passed out illeagal
#define E_LOG_GET_FAILED                (SCS_LOG_ERROR_CODE_BASE | 0X0103)    // log inquiring failed
#define E_LOG_CLEAR_FAILED              (SCS_LOG_ERROR_CODE_BASE | 0X0104)    // log clear failed
#define E_LOG_EXPORT_FAILED             (SCS_LOG_ERROR_CODE_BASE | 0X0105)    // log exporting failed
#define E_LOG_EXPORT_ZERO               (SCS_LOG_ERROR_CODE_BASE | 0X0106)    // log number is 0
#define E_LOG_GETNUM_FAILED             (SCS_LOG_ERROR_CODE_BASE | 0X0107)    // log account obtain failed
#define E_LOG_SET_WMAIL_FAILED          (SCS_LOG_ERROR_CODE_BASE | 0X0108)    // log alarm mail config file setting failed
#define E_LOG_GET_WMAIL_FAILED          (SCS_LOG_ERROR_CODE_BASE | 0X0109)    // log alarm mail config file inquiring failed
#define E_LOG_GET_USERLOG_AUTH          (SCS_LOG_ERROR_CODE_BASE | 0X010a)
#define E_LOG_GET_USERLOG_USERINV       (SCS_LOG_ERROR_CODE_BASE | 0X010b)
#define E_LOG_PEER_ABSENT               (SCS_LOG_ERROR_CODE_BASE | 0X010c)
#define E_LOG_GET_ALARM_FAILED          (SCS_LOG_ERROR_CODE_BASE | 0X010d)    // alarm inquiring failed
#define E_LOG_DEL_ALARM_FAILED          (SCS_LOG_ERROR_CODE_BASE | 0X010e)    // alarm deleting failed

/* **************************MAIL********************************* */
#define E_MAIL_PARA_NULL                (SCS_MAIL_ERROR_CODE_BASE | 0X0100)    // point parameters null
#define E_MAIL_PARAM_ILLEAG             (SCS_MAIL_ERROR_CODE_BASE | 0X0101)    // parameters passed in or passed out illeagal
#define E_MAIL_LINK_HOST_FAILED         (SCS_MAIL_ERROR_CODE_BASE | 0X0102)    // link host failed
#define E_MAIL_LOGIN_FAILED             (SCS_MAIL_ERROR_CODE_BASE | 0X0103)    // login mail host failed
#define E_MAIL_CONFFILE_CREAT_FAILED    (SCS_MAIL_ERROR_CODE_BASE | 0X0104)    // mail config file create failed
#define E_MAIL_CONF_GET_FAILED          (SCS_MAIL_ERROR_CODE_BASE | 0X0105)    // mail config inquiring failed.
#define E_MAIL_CONF_SET_FAILED          (SCS_MAIL_ERROR_CODE_BASE | 0X0106)    // set mail config failed
#define E_MAIL_SET_ENABLE_FAILED        (SCS_MAIL_ERROR_CODE_BASE | 0X0107)    // enable mail-sending failed
#define E_MAIL_GET_ENABLE_FAILED        (SCS_MAIL_ERROR_CODE_BASE | 0X0108)    // inquiring mail-sending switch failed
#define E_MAIL_UNABLED_MAIL_ERR         (SCS_MAIL_ERROR_CODE_BASE | 0X0109)    // mail sending error: mail switch is not enabled
#define E_MAIL_CONFIG_VERIMAIL_FAILED   (SCS_MAIL_ERROR_CODE_BASE | 0X010a)    // verifying test mail sending error, check configuration
#define E_MAIL_SENDING_FAILED           (SCS_MAIL_ERROR_CODE_BASE | 0X010b)    // mail sending failed

/* *************************DIAG，重启关闭等相关的********************************** */
#define E_DIAG_GET_INFO_FAILED          (SCS_DIAG_ERROR_CODE_BASE | 0X0100)    //获取诊断信息失败
#define E_DIAG_PARA_NULL                (SCS_DIAG_ERROR_CODE_BASE | 0X0101)    // point parameters null
#define E_DIAG_SODA_FAILED              (SCS_DIAG_ERROR_CODE_BASE | 0X0102)    // SODA接口调用失败
#define E_DIAG_ECC_SWITCH_SET_FAILED    (SCS_DIAG_ERROR_CODE_BASE | 0X0104)    // set ecc check switch failed
#define E_DIAG_GET_INFO_NOTSUPPORT      (SCS_DIAG_ERROR_CODE_BASE | 0X0105)    //设备信息不支持
/* *************************MTS********************************** */
#define E_MTS_GET_INFO_FAILED           (MTS_ERROR_CODE_BASE | 0X0100)         // 获取信息失败

/****************************TRAP***********************************/
#define E_SMA_TRAP_OVERFLOW             (SCS_DIAG_ERROR_CODE_BASE | 0X0120)    // Trap Cfg Is Over Flow
#define E_SCS_TRAP_ID_NOT_EXIST         (SCS_DIAG_ERROR_CODE_BASE | 0X0121)    // Trap Cfg ID IS Not EXist


/**************************** SIC **********************************/

#define E_SIC_PARAM_NULL                (SCS_SIC_ERROR_CODE_BASE | 0x0300)
#define E_SIC_PARAM_ILLEAG              (SCS_SIC_ERROR_CODE_BASE | 0x0301)
#define E_SIC_COLLECT_SYS_INFO_FAILED   (SCS_SIC_ERROR_CODE_BASE | 0x0302)
#define E_SIC_GET_TASK_PROC_FAILED      (SCS_SIC_ERROR_CODE_BASE | 0x0303)
#define E_SIC_SET_EMAIL_ENABLE_FAILED   (SCS_SIC_ERROR_CODE_BASE | 0x0304)
#define E_SIC_GET_TARBALL_INFO_FAILED   (SCS_SIC_ERROR_CODE_BASE | 0x0305)
#define E_SIC_GET_EMAIL_ENABLE_FAILED   (SCS_SIC_ERROR_CODE_BASE | 0x0306)
#define E_SIC_TAR_BALL_NOT_EXIST        (SCS_SIC_ERROR_CODE_BASE | 0x0307)
#define E_SIC_TASK_IS_EXIST             (SCS_SIC_ERROR_CODE_BASE | 0x0308)
#define E_SIC_EXPORT_SYSINFO_TARBALL_FAILED  (SCS_SIC_ERROR_CODE_BASE | 0x0309)

/*******************************************************************
** 定义STS子系统内部错误码
********************************************************************/
#define E_STS_SEQNO_NUM_ERR           (STS_ERROR_CODE_BASE | 0x0001)
#define E_STS_LUNDEV_NO_EXIST_ERR     (STS_ERROR_CODE_BASE | 0x0002)
#define E_STS_INITIATOR_NO_EXIST_ERR  (STS_ERROR_CODE_BASE | 0x0003)
#define E_STS_LUN_NO_EXIST_ERR        (STS_ERROR_CODE_BASE | 0x0004)
#define E_STS_LOCALLUN_NUM_ERR        (STS_ERROR_CODE_BASE | 0x0005)
#define E_STS_AHEAD_READ_SIZE_ERR     (STS_ERROR_CODE_BASE | 0x0006)
#define E_STS_CACHE_POLICY_ERR        (STS_ERROR_CODE_BASE | 0x0007)
#define E_STS_REFRESH_CACHE_ERR       (STS_ERROR_CODE_BASE | 0x0008)
#define E_STS_LUNDEV_IS_MAPPING_ERR   (STS_ERROR_CODE_BASE | 0x0009)
#define E_STS_OWNER_PARAM_ERR         (STS_ERROR_CODE_BASE | 0x0010)
#define E_STS_PREFER_PARAM_ERR        (STS_ERROR_CODE_BASE | 0x0011)
#define E_STS_DISCARD_CACHE_ERR        (STS_ERROR_CODE_BASE | 0x0012)
#define E_STS_REMOVE_MAP_TIMEOUT_ERR   (STS_ERROR_CODE_BASE | 0x0013)
#define E_STS_WPFLAG_PARAM_ERR         (STS_ERROR_CODE_BASE | 0x0014)
#define E_STS_ASYMMETRIC_ISTRANSITING_ERR  (STS_ERROR_CODE_BASE | 0x0015)

#define E_STS_STSCREATELUNDEV_ERR     (STS_ERROR_CODE_BASE | 0x0100)
#define E_STS_STSCREATELUNDEV_ALLOC_LUNDEV_FAILED_ERR  (STS_ERROR_CODE_BASE | 0x0101)
#define E_STS_STSCREATELUNDEV_OPEN_LUNDEV_FAILED_ERR   (STS_ERROR_CODE_BASE | 0x0102)
#define E_STS_STSCREATELUNDEV_SEQNO_EXIST_ERR          (STS_ERROR_CODE_BASE | 0x0103)
#define E_STS_STSCREATELUNDEV_LUNDEV_EXIST_ERR         (STS_ERROR_CODE_BASE | 0x0104)

#define E_STS_STSREMOVELUNDEV_ERR  (STS_ERROR_CODE_BASE | 0x0200)
#define E_STS_STSREMOVELUNDEV_BUSY_ERR  (STS_ERROR_CODE_BASE | 0x0201)

#define E_STS_STSLISTLUNDEV_ERR    (STS_ERROR_CODE_BASE | 0x0300)
#define E_STS_STSLISTLUNDEV_LUNDEV_REFCNT_ERR  (STS_ERROR_CODE_BASE | 0x0301)

#define E_STS_STSMODIFYLUNDEVOWNER_ERR  (STS_ERROR_CODE_BASE | 0x0400)

#define E_STS_STSCREATELUNMAP_ERR                         (STS_ERROR_CODE_BASE | 0x0500)
#define E_STS_STSCREATELUNMAP_ALLOC_INITIATOR_FAILED_ERR  (STS_ERROR_CODE_BASE | 0x0501)
#define E_STS_STSCREATELUNMAP_ALLOC_LUN_FAILED_ERR        (STS_ERROR_CODE_BASE | 0x0502)
#define E_STS_STSCREATELUNMAP_LUN_EXIST_ERR               (STS_ERROR_CODE_BASE | 0x0503)
#define E_STS_STSCREATELUNMAP_OPEN_LUNDEV_ERR             (STS_ERROR_CODE_BASE | 0x0504)

#define E_STS_STSREMOVELUNMAP_ERR  (STS_ERROR_CODE_BASE | 0x0600)

#define E_STS_STSLISTLUNMAP_ERR    (STS_ERROR_CODE_BASE | 0x0700)

#define E_STS_STSSTOPLUN_ERR       (STS_ERROR_CODE_BASE | 0x0800)

#define E_STS_STSSTARTLUN_ERR      (STS_ERROR_CODE_BASE | 0x0900)

#define E_STS_STSSETSTATSTATUS_ERR        (STS_ERROR_CODE_BASE | 0x0A00)
#define E_STS_STSSETSTATSTATUS_PARAM_ERR  (STS_ERROR_CODE_BASE | 0x0A01)

#define E_STS_STSGETSTATSTATUS_ERR        (STS_ERROR_CODE_BASE | 0x0B00)

#define E_STS_STSGETSTATDATA_ERR          (STS_ERROR_CODE_BASE | 0x0C00)

#define E_STS_STSPARAMINIT_ERR                (STS_ERROR_CODE_BASE | 0x0D00)
#define E_STS_STSPARAMINIT_CONTROLLER_ID_ERR  (STS_ERROR_CODE_BASE | 0x0D01)

#define E_STS_STSSETCONTGTINFO_ERR                          (STS_ERROR_CODE_BASE | 0x0E00)
#define E_STS_STSSETCONTGTINFO_WAKEUP_THREAD_FLAG_NULL_ERR  (STS_ERROR_CODE_BASE | 0x0E01)

#define E_STS_STSMODIFYLUNDEVERRORFLAG_ERR        (STS_ERROR_CODE_BASE | 0x0F00)
#define E_STS_STSMODIFYLUNDEVERRORFLAG_PARAM_ERR  (STS_ERROR_CODE_BASE | 0x0F01)

#define E_STS_STSLISTLUNSEQNOONSESSION_ERR        (STS_ERROR_CODE_BASE | 0x1000)
#define E_STS_STSLISTLUNSEQNOONSESSION_PARAM_ERR  (STS_ERROR_CODE_BASE | 0x1001)

#define E_STS_STSMODIFYLUNDEVPORTSTATUS_ERR       (STS_ERROR_CODE_BASE | 0x1100)
#define E_STS_STSMODIFYLUNDEVPORTSTATUS_PARAM_ERR (STS_ERROR_CODE_BASE | 0x1101)

#define E_STS_STSMODIFYLUNDEVPREFER_ERR         (STS_ERROR_CODE_BASE | 0x1200)

#define E_STS_STSSEARCHITONCONTORL_ERR          (STS_ERROR_CODE_BASE | 0x1300)
#define E_STS_STSSEARCHITONCONTORL_PARAM_ERR    (STS_ERROR_CODE_BASE | 0x1301)
#define E_STS_STSSEARCHITONCONTORL_IT_ERR       (STS_ERROR_CODE_BASE | 0x1302)

#define E_STS_STSWAKEUPSTPG_ERR                 (STS_ERROR_CODE_BASE | 0x1400)

#define E_STS_STSMODIFYLUNDEVRSTFLAG_ERR        (STS_ERROR_CODE_BASE | 0x1500)
#define E_STS_STSMODIFYLUNDEVRSTFLAG_PARAM_ERR  (STS_ERROR_CODE_BASE | 0x1501)

#define E_STS_STSMODIFYLUNDEVCACHE_ERR          (STS_ERROR_CODE_BASE | 0x1600)

#define E_STS_STSSETALLSTATSTATUS_ERR        (STS_ERROR_CODE_BASE | 0x1700)
#define E_STS_STSSETALLSTATSTATUS_PARAM_ERR  (STS_ERROR_CODE_BASE | 0x1701)

#define E_STS_STSREFRESHCACHE_ERR            (STS_ERROR_CODE_BASE | 0x1800)

#define E_STS_LUNDEV_WAIT_CMDS_TIMEOUT_ERR            (STS_ERROR_CODE_BASE | 0x1900)

/*******************************************************************
** 定义CBS子系统内部错误码
********************************************************************/
#define E_CBS_OPEN_FILE_ERROR        (CBS_ERROR_CODE_BASE | 0x0001)
#define E_CBS_BAD_ADDR               (CBS_ERROR_CODE_BASE | 0x0002)
#define E_CBS_ERR_CACHE_SIZE         (CBS_ERROR_CODE_BASE | 0x0003)
#define E_CBS_ERR_DIRTY_RATIO        (CBS_ERROR_CODE_BASE | 0x0004)
#define E_CBS_ERR_PCIE_NO_LINK       (CBS_ERROR_CODE_BASE | 0x0005)
#define E_CBS_ERR_OPEN_MIRROR_PARM   (CBS_ERROR_CODE_BASE | 0x0006)
#define E_CBS_ERR_MONITOR_PARM       (CBS_ERROR_CODE_BASE | 0x0007)
#define E_CBS_MONITOR_STATUS_OFF     (CBS_ERROR_CODE_BASE | 0x0008)
#define E_CBS_ERR_FLUSH_LEVEL        (CBS_ERROR_CODE_BASE | 0x0009)
#define E_CBS_ERR_FLUSH_LOW_LEVEL    (CBS_ERROR_CODE_BASE | 0x000A)
#define E_CBS_ERR_TIME_INTERV        (CBS_ERROR_CODE_BASE | 0x000B)
#define E_CBS_ERR_MIRR_ENABLE        (CBS_ERROR_CODE_BASE | 0x000C)

/*******************************************************************
** 定义VDS子系统内部错误码
********************************************************************/

/*
划分如下，通用的、vd、vol的错误码从
(VDS_ERROR_CODE_BASE|0x1001)开始

快照从
(VDS_ERROR_CODE_BASE|0x2001)开始

克隆卷从
(VDS_ERROR_CODE_BASE|0x3001)开始

漫游从
(VDS_ERROR_CODE_BASE|0x4001)开始

远程镜像从
(VDS_ERROR_CODE_BASE|0x5001)开始

虚拟池从
(VDS_ERROR_CODE_BASE|0x6001)开始

一共可以有15个大类的功能，应该够用了。每个大类下有4095个错误码，应该也够用了。

新增错误码，不需要考虑赋值为多少，只要正确的添加到对应功能的后面就可以了，记住还要加个英文逗号。

为了方便看，最好每一大类下面每隔10个错误码做个分割线

*/

#define E_VDS_NOERROR                       0
enum {
/* 通用的 */
E_VDS_NULLP = (VDS_ERROR_CODE_BASE|0x1001),
E_VDS_PARAM_INVALID,
E_VDS_NO_VD_MOD,
E_VDS_NO_VOL_MOD,
E_VDS_VD_EXIST,
E_VDS_NO_VD,
E_VDS_NO_VOL,
E_VDS_OBJECT_EXIST,
E_VDS_VD_EXCEED,
E_VDS_VOL_EXCEED,
/* -----------10 -----------*/
E_VDS_VD_DEGRADE,
E_VDS_WRONG_CHUNKSIZE,
E_VDS_OPEN_FAIL,
E_VDS_MK_NOD_FAIL,
E_VDS_VOL_BUSY,
E_VDS_VOL_NO_READY,
E_VDS_VD_REJECTED_FREEZED,
E_VDS_VOL_DEV_BUSY,
E_VDS_VD_NOT_GOOD,
E_VDS_VOL_NOT_GOOD,
/* -----------20 -----------*/
E_VDS_ZERO_DISK_AREA_FAIL,
E_VDS_SOCKET_CREATE,
E_VDS_INIT_FAIL,
E_VDS_MSG_LENGTH,
E_VDS_VOL_TYPE,
E_VDS_CAPACITY,
E_VDS_NAME_INVALID,
E_VDS_RAID_LEVEL,
E_VDS_BAD_REBUILD_MODE,
E_VDS_VD_DEL_REJECT,
/* -----------30 -----------*/
E_VDS_PD_SET_INVALID,
E_VDS_USER_MEM_ALLOC,
E_VDS_KERNEL_MEM_ALLOC,
E_VDS_DEV_NOT_EXIST,
E_VDS_SYS_NOT_EXIST,
E_VDS_NO_PERMISSION,
E_VDS_DISK_TYPE_INVAL,
E_VDS_VD_NOSPC,
E_VDS_VD_SYS_REGISTER,
E_VDS_RAID_NO_MOD,
/* -----------40 -----------*/
E_VDS_RAID_RUN_FAIL,
E_VDS_PTHREAD_CREATE,
E_VDS_PTHREAD_DESTROY,
E_VDS_THREAD_NOTEXIST,
E_VDS_THREAD_EXIST,
E_VDS_THREAD_CREATE,
E_VDS_THREAD_SYNC,
E_VDS_DEVNO_TO_UUID,
E_VDS_UUID_TO_DEVNO,
E_VDS_WAIT_TIME_OUT,
/* -----------50 -----------*/
E_VDS_CDEV_REGISTER,
E_VDS_CDEV_INIT,
E_VDS_TYPE_INVALID,
E_VDS_NOT_CONSISTENT,
E_VDS_RVOL_NOT_EXIST,
E_VDS_SAME_VD,
E_VDS_WRONG_REBUILD_LEVEL,
E_VDS_DISK_RAID0,
E_VDS_DISK_RAID1,
E_VDS_DISK_RAID10,
/* -----------60 -----------*/
E_VDS_DISK_RAID5,
E_VDS_DISK_RAID6,
E_VDS_AUTOREBUILD_ALREADY_OFF,
E_VDS_VOL_DEL_REJECT,
E_VDS_VD_FAULT,
E_VDS_NOTSUPPORT,
E_VDS_NOT_LOCAL,
E_VDS_VD_RECING,
E_VDS_VD_CREATE_FAIL,
E_VDS_VOL_CREATE_FAIL,
/* -----------70 -----------*/
E_VDS_VD_DELETE_FAIL,
E_VDS_VOL_DELETE_FAIL,
E_VDS_B_VD_RECING,
E_VDS_OPEN_DIR_FAIL,
E_VDS_BVOL_RSYNC,
E_VDS_VD_NOT_RECING,
E_VDS_DEVNO_INVALID,
E_VDS_DEV_ALREADY_EXIST,
E_VDS_PEER_TIMEOUT,
E_VDS_PEER_FAIL,
/* -----------80 -----------*/
E_VDS_VD_NO_DEGRADE,
E_VDS_SPAREDISK_NUM,
E_VDS_GET_VD_INFO_BY_NAME_FAIL,
E_VDS_SET_VD_NAME_FAIL,
E_VDS_SET_VD_PRECTL_FAIL,
E_VDS_SET_VD_OWNERCTL_FAIL,
E_VDS_SET_VD_AUTO_REBUILD_FAIL,
E_VDS_VOL_RENAME_FAIL,
E_VDS_GET_VOL_INFO_FAIL,
E_VDS_GET_VOL_STAT_FAIL,
/* -----------90 -----------*/
E_VDS_CLEAN_VOL_STAT_FAIL,
E_VDS_SET_VOL_PRECTL_FAIL,
E_VDS_SET_VOL_OWNERCTL_FAIL,
E_VDS_VD_CAPACITY_NOT_ENOUTH,
E_VDS_WRITE_FILE_FAIL,
E_VDS_VD_SETFLAG_FAIL,
E_VDS_VD_LOAD_ERROR,





/* 快照 */
E_VDS_NO_SNAP_MOD = (VDS_ERROR_CODE_BASE|0x2001),
E_VDS_NO_SNAP,
E_VDS_SNAP_EXCEED,
E_VDS_SNAP_EXIST,
E_VDS_SNAP_RUN_ERROR,
E_VDS_SNAP_CREATE_FAIL,
E_VDS_SNAP_DELETE_FAIL,
E_VDS_GET_SVOL_INFO_FAIL,
E_VDS_SET_SVOL_NAME_FAIL,
E_VDS_SET_RVOL_FAIL,
/* -----------10 -----------*/
E_VDS_SVOL_RESTORE_FAIL,
E_VDS_BVOL_NO_EXIST,
E_VDS_SVOL_INIT_FAIL,
E_VDS_RVOL_INIT_FAIL,
E_VDS_EXCEPTIONSTORE_CREATE_FAIL,
E_VDS_HASHTABLE_INIT_FAIL,
E_VDS_SVOL_INVALID,
E_VDS_SVOL_WRITE_STATE,
E_VDS_PENDING_EXCEPTIONS_RUNNING,
E_VDS_SVOL_REGISTERTHREAD_FAIL,
/* -----------20 -----------*/
E_VDS_PSDMIOCLIENT_CREATE_FAIL,
E_VDS_PS_AREA_ALLOC_FAIL,
E_VDS_PS_HEAD_AREA_ALLOC_FAIL,
E_VDS_PS_CALLBACKS_ALLOC_FAIL,
E_VDS_SVOL_RESTORING_STATE,
E_VDS_RVOL_NO_SPACE,
E_VDS_B_VD_NOTGOOD,
E_VDS_RVOL_ALREADY_EXIST,
E_VDS_RVOL_CREATE_FAIL,
E_VDS_NO_RVOL,
/* -----------30 -----------*/
E_VDS_RVOL_DELETE_FAIL,
E_VDS_RVOL_DEL_REJECT,
E_VDS_GET_RVOLINFO_FAIL,
E_VDS_RVOL_CAPACITY_PERCENTAGE,
E_VDS_RVOL_THR,
E_VDS_BVOL_RESTORING_STATE,


/* 克隆 */
E_VDS_CLONE_MAX = (VDS_ERROR_CODE_BASE|0x3001),
E_VDS_CLONE_UNBIND,
E_VDS_CLONE_UNFRACTURED,
E_VDS_CLONE_CREATE_FAIL,
E_VDS_CLONE_DELETE_FAIL,
E_VDS_CLONE_BUSY,
E_VDS_GET_CLONE_INFO_FAIL,
E_VDS_SET_CLONE_PRIORITY_FAIL,
E_VDS_SET_CLONE_PROTECT_FAIL,
E_VDS_SET_CLONE_NAME_FAIL,
/* -----------10 -----------*/
E_VDS_CLONE_FRACTURE_FAIL,
E_VDS_CLONE_SYNC_FAIL,
E_VDS_CLONE_REVERSE_SYNC_FAIL,
E_VDS_CLONE_FRACTURED,


/* 漫游 */
E_VDS_NOT_FOREIGN = (VDS_ERROR_CODE_BASE|0x4001),
E_VDS_IMMIGRATE_FAIL,
E_VDS_NOT_IMMIGRATE,
E_VDS_VD_REJECTED_IMMIGRATED,
E_VDS_VD_LOCALREJECTED_IMMIGRATED,
E_VDS_VD_FREEZE_FAIL,
E_VDS_VD_UNFREEZE_FAIL,
E_VDS_VD_NOT_FREEZED,
E_VDS_VD_RELATION_EXIST,

/* 远程镜像 */
E_VDS_MIRROR_CREATE_FAIL = (VDS_ERROR_CODE_BASE|0x5001),
E_VDS_MIRROR_NOT_EXIST,
E_VDS_MIRROR_GETINFO_FAIL,
E_VDS_MIRROR_EXIST,
E_VDS_MIRROR_NO_SRC,
E_VDS_MIRROR_CAP,
E_VDS_MIRROR_SRC_HAS,
E_VDS_MIRROR_SETNAME_FAIL,
E_VDS_MIRROR_SETPOLICY_FAIL,
E_VDS_MIRROR_SETPRIORITY_FAIL,
/* -----------10 -----------*/
E_VDS_MIRROR_SETPERIOD_FAIL,
E_VDS_MIRROR_SRC_TYPE,
E_VDS_MIRROR_BUSY,
E_VDS_MIRROR_SYNCING,
E_VDS_MIRROR_DEST,
E_VDS_MIRROR_MAXNUM,
E_VDS_MIRROR_TYPE,


/* 虚拟池、卷 */
E_VDS_NO_POOL_MOD = (VDS_ERROR_CODE_BASE|0x6001),
E_VDS_POOL_EXCEED,
E_VDS_KERNEL_POOL_CREATE_FAIL,
E_VDS_KERNEL_POOL_DELETE_FAIL,
E_VDS_POOL_NOT_EXIST,
E_VDS_POOL_DEL_REJECT,
E_VDS_POOL_EXIST,
E_VDS_KERNEL_POOL_MODIFY_FAIL,
E_VDS_POOL_CANNOT_REMOVE_DISK,
E_VDS_KERNEL_POOL_GETINFO_FAIL,
/*--------------10---------------*/
E_VDS_POOL_ALARMTHRESHOLD_INVALID,
E_VDS_POOL_CAPACITY_NOT_ENOUTH,
E_VDS_VOL_EXCEED_IN_SYS,
E_VDS_POOL_STATE_INVALID,
E_VDS_POOL_REMOVE_DISK_INVALID,
E_VDS_VOL_CAPACITY_EXCEED,
};


/*******************************************************************
** 定义DM子系统内部错误码
********************************************************************/

#define E_DM_INPUT_PARAM_INVALID              (DM_ERROR_CODE_BASE|0x1001)
#define E_DM_OPEN_DM_DIR_FAIL                 (DM_ERROR_CODE_BASE|0x1002)
#define E_DM_ALLOC_RESOURCE_FAIL              (DM_ERROR_CODE_BASE|0x1003)
#define E_DM_DISK_ABSENT                      (DM_ERROR_CODE_BASE|0x1004)
#define E_DM_SOCKET_FAIL                      (DM_ERROR_CODE_BASE|0x1005)

#define E_DM_GET_WARN_SWITCH_FAIL             (DM_ERROR_CODE_BASE|0x1101)
#define E_DM_GET_BBLK_LIMIT_FAIL              (DM_ERROR_CODE_BASE|0x1102)
#define E_DM_GET_RUNNING_STATE_FAIL           (DM_ERROR_CODE_BASE|0x1103)
#define E_DM_GET_CTRL_UUID_FAIL               (DM_ERROR_CODE_BASE|0x1104)
#define E_DM_GET_DEBUG_LEVEL_FAIL             (DM_ERROR_CODE_BASE|0x1105)
#define E_DM_SET_WARN_SWITCH_FAIL             (DM_ERROR_CODE_BASE|0x1201)
#define E_DM_SET_BBLK_LIMIT_FAIL              (DM_ERROR_CODE_BASE|0x1202)
#define E_DM_SET_RUNNING_STATE_FAIL           (DM_ERROR_CODE_BASE|0x1203)
#define E_DM_SET_CTRL_UUID_FAIL               (DM_ERROR_CODE_BASE|0x1204)
#define E_DM_SET_DEBUG_LEVEL_FAIL             (DM_ERROR_CODE_BASE|0x1205)

#define E_DM_GET_TYPE_FAIL                    (DM_ERROR_CODE_BASE|0x2101)
#define E_DM_GET_USEABLE_FAIL                 (DM_ERROR_CODE_BASE|0x2102)
#define E_DM_GET_ACCESSABLE_FAIL              (DM_ERROR_CODE_BASE|0x2103)
#define E_DM_GET_PD_SCAN_RATE_FAIL            (DM_ERROR_CODE_BASE|0x2104)
#define E_DM_GET_SMART_FAIL                   (DM_ERROR_CODE_BASE|0x2105)
#define E_DM_GET_GUID_FAIL                    (DM_ERROR_CODE_BASE|0x2111)
#define E_DM_GET_DEVNAME_FAIL                 (DM_ERROR_CODE_BASE|0x2112)
#define E_DM_GET_CAPACITY_FAIL                (DM_ERROR_CODE_BASE|0x2113)
#define E_DM_GET_PHYID_FAIL                   (DM_ERROR_CODE_BASE|0x2114)
#define E_DM_GET_JBODCADDR_FAIL               (DM_ERROR_CODE_BASE|0x2115)
#define E_DM_GET_BUSID_FAIL                   (DM_ERROR_CODE_BASE|0x2116)
#define E_DM_GET_SASADDR_FAIL                 (DM_ERROR_CODE_BASE|0x2121)
#define E_DM_GET_CACHE_FAIL                   (DM_ERROR_CODE_BASE|0x2122)
#define E_DM_GET_MODEL_FAIL                   (DM_ERROR_CODE_BASE|0x2123)
#define E_DM_GET_PHY_TYPE_FAIL                (DM_ERROR_CODE_BASE|0x2124)
#define E_DM_GET_FW_REV_FAIL                  (DM_ERROR_CODE_BASE|0x2125)
#define E_DM_GET_SERIAL_NO_FAIL               (DM_ERROR_CODE_BASE|0x2126)
#define E_DM_GET_STD_VERSION_FAIL             (DM_ERROR_CODE_BASE|0x2127)

#define E_DM_SET_PD_TYPE_FAIL                 (DM_ERROR_CODE_BASE|0x2201)
#define E_DM_SET_PD_USEABLE_FAIL              (DM_ERROR_CODE_BASE|0x2202)
#define E_DM_SET_PD_ACCESS_FAIL               (DM_ERROR_CODE_BASE|0x2203)
#define E_DM_SET_CONTROL_PD_SCAN_FAIL         (DM_ERROR_CODE_BASE|0x2204)
#define E_DM_SET_SMART_SELFTEST_FAIL          (DM_ERROR_CODE_BASE|0x2211)
#define E_DM_SET_CACHE_FAIL                   (DM_ERROR_CODE_BASE|0x2221)

/* add by kangzhenhua 2012.02.21 for IO statistics */
#define E_DM_GET_IO_STATISTICS_FAIL           (DM_ERROR_CODE_BASE|0x2301)
#define E_DM_SET_IO_STATISTICS_FAIL           (DM_ERROR_CODE_BASE|0x2302)
#define E_DM_RESET_IO_STATISTICS_FAIL         (DM_ERROR_CODE_BASE|0x2303)


#define E_DM_NO_RESOURCE         (DM_ERROR_CODE_BASE|0x3001)   /* 无资源可用，比如申请内核、页等失败 */
#define E_DM_INVALID_INPUT       (DM_ERROR_CODE_BASE|0x3002)   /* 函数输入非法 */
#define E_DM_CMD_VALUE_ERROR     (DM_ERROR_CODE_BASE|0x3003)   /* 命令中有非法字段 */

#define E_DM_MDA_RW_ERROR        (DM_ERROR_CODE_BASE|0x3004)   /* 元数据区读写错误 */
#define E_DM_MDA_CHECKSUM_ERROR  (DM_ERROR_CODE_BASE|0x3005)   /* 元数据校验和错误 */
#define E_DM_MDA_SELRCT_ERROR    (DM_ERROR_CODE_BASE|0x3006)   /* 元数据区选择错误 */
#define E_DM_PD_IN_LIST          (DM_ERROR_CODE_BASE|0x3007)   /* 当前节点存在于磁盘链表中 */
#define E_DM_PD_NOT_IN_LIST      (DM_ERROR_CODE_BASE|0x3008)   /* 当前节点不存在于磁盘链表中 */
#define E_DM_BEYOND_MAX_PD_NUM   (DM_ERROR_CODE_BASE|0x3009)   /* 超过系统最大磁盘数量 */
#define E_DM_START_ERROR         (DM_ERROR_CODE_BASE|0x300A)  /* 系统启动失败 */
#define E_DM_STOP_ERROR          (DM_ERROR_CODE_BASE|0x300B)   /* 系统停止失败 */
#define E_DM_MODIFY_PD_SYSINFO_ERROR       (DM_ERROR_CODE_BASE|0x300C)   /* 修改系统软件属性失败 */

#define E_DM_GET_STD_VER_ERROR             (DM_ERROR_CODE_BASE|0x300D)   /*  获取标准版本号失败 */
#define E_DM_GET_CAPACITY_VALUE_ERROR      (DM_ERROR_CODE_BASE|0x300E)   /*获取磁盘容量失败 */
#define E_DM_GET_FW_VALUE_ERROR            (DM_ERROR_CODE_BASE|0x301F)   /* 获取FW版本号失败*/
#define E_DM_GET_CATCH_VALUE_ERROR         (DM_ERROR_CODE_BASE|0x3010)   /* 获取缓存状态值失败 */
#define E_DM_GET_MODEL_VALUE_ERROR         (DM_ERROR_CODE_BASE|0x3011)   /* 获取MODEL名失败 */
#define E_DM_GET_SERIAL_VALUE_ERROR        (DM_ERROR_CODE_BASE|0x3012)   /* 获取磁盘序列号失败 */
#define E_DM_GET_SAS_VALUE_ERROR           (DM_ERROR_CODE_BASE|0x3013)   /* 获取磁盘SAS值失败*/

#define E_DM_SOCKET_INIT_ERROR             (DM_ERROR_CODE_BASE|0x3014)   /* SOCKET初始化失败 */
#define E_DM_SOCKET_SET_NUM_ERROR          (DM_ERROR_CODE_BASE|0x3015)   /* 设置客户端SOCKET类型失败 */
#define E_DM_SOCKET_BIND_ERROR             (DM_ERROR_CODE_BASE|0x3016)   /* SOCKET绑定失败 */
#define E_DM_SOCKET_SET_ERROR              (DM_ERROR_CODE_BASE|0x3017)   /* SOCKET属性设置失败 */
#define E_DM_SOCKET_SEND_ERROR             (DM_ERROR_CODE_BASE|0x3018)   /* SOCKET数据发送失败 */
#define E_DM_SOCKET_RECEV_ERROR            (DM_ERROR_CODE_BASE|0x3019)   /* SOCKET数据接收失败 */
#define E_DM_PD_GET_SMART_ERROR            (DM_ERROR_CODE_BASE|0x301A)   /* 获得SMART信息失败 */
#define E_DM_SET_SMART_SELFTEST_ERROR      (DM_ERROR_CODE_BASE|0x301B)   /* SMART自检测失败 */

#define E_DM_PD_GET_SMART_DURING_SCAN_ERROR            (DM_ERROR_CODE_BASE|0x301C)   /* 获得SMART信息失败 */
#define E_DM_SET_SMART_SELFTEST_DURING_SCAN_ERROR      (DM_ERROR_CODE_BASE|0x301D)   /* SMART自检测失败 */
#define E_DM_SET_DISK_DATA_TYPE_DURING_SCAN_ERROR                (DM_ERROR_CODE_BASE|0x301E)
#define E_DM_SET_DISK_UNKNOWN_TYPE_DURING_SCAN_ERROR                (DM_ERROR_CODE_BASE|0x301F)
#define E_DM_SCAN_DATA_PD_FAIL                 (DM_ERROR_CODE_BASE|0x3020)
#define E_DM_SCAN_UNKNOWN_PD_FAIL                 (DM_ERROR_CODE_BASE|0x3021)
#define E_DM_SMART_DATA_PD_FAIL                 (DM_ERROR_CODE_BASE|0x3022)

/*******************************************************************
** 定义SAS子系统内部错误码
********************************************************************/


/*******************************************************************
** 定义JBODC子系统内部错误码
********************************************************************/

/*******************************************************************
** 定义TCS子系统内部错误码
* TCS错误码由 0x01090000 开始 最多可支持到 0x0109FFFF
********************************************************************/
#define E_TCS_COPY_FROM_USER_FAIL                   (TCS_ERROR_CODE_BASE|0x0000) /* 0x01090000,17367040 */
#define E_TCS_COPY_TO_USER_FAIL                     (TCS_ERROR_CODE_BASE|0x0001) /* 0x01090001,17367041 */
#define E_TCS_KMALLOC_FAIL                          (TCS_ERROR_CODE_BASE|0x0002) /* 0x01090002,17367042 */
#define E_TCS_DOWN_SEM_FAIL                         (TCS_ERROR_CODE_BASE|0x0003) /* 0x01090003,17367043 */

#define E_TCS_TGT_NOT_EXIST                         (TCS_ERROR_CODE_BASE|0x0004) /* 0x01090004,17367044 */
#define E_TCS_TGT_ALREADY_EXIST                     (TCS_ERROR_CODE_BASE|0x0005) /* 0x01090005,17367045 */
#define E_TCS_TGT_SESSION_BUSY                      (TCS_ERROR_CODE_BASE|0x0006) /* 0x01090006,17367046 */
#define E_TCS_MAX_TGT_NUM_REACHED                   (TCS_ERROR_CODE_BASE|0x0007) /* 0x01090007,17367047 */

#define E_TCS_SESSION_NOT_EXIST                     (TCS_ERROR_CODE_BASE|0x0008) /* 0x01090008,17367048 */
#define E_TCS_SESSION_ALREADY_EXIST                 (TCS_ERROR_CODE_BASE|0x0009) /* 0x01090009,17367049 */
#define E_TCS_SESSION_CONN_BUSY                     (TCS_ERROR_CODE_BASE|0x000A) /* 0x0109000A,17367050 */

#define E_TCS_INVAL_IOCTL_CMD                       (TCS_ERROR_CODE_BASE|0x000B) /* 0x0109000B,17367051 */
#define E_TCS_PORTGRP_NOT_EXIST                     (TCS_ERROR_CODE_BASE|0x0017)
#define E_TCS_ALL_PORTGRP_BE_USED                   (TCS_ERROR_CODE_BASE|0x0018)

#define E_TCS_DIR_OPEN_FAIL                         (TCS_ERROR_CODE_BASE|0x002A)      /* 0x109002a,17367082 */
#define E_TCS_FILE_OPEN_FAIL                        (TCS_ERROR_CODE_BASE|0x002B)      /* 0x109002b,17367083 */
#define E_TCS_SYSCALL_FAIL                          (TCS_ERROR_CODE_BASE|0x002C)      /* 0x109002c,17367084 */
#define E_TCS_SYSPROC_FAIL                          (TCS_ERROR_CODE_BASE|0x002D)      /* 0x109002d,17367085 */
#define E_TCS_PARAM_NULL                            (TCS_ERROR_CODE_BASE|0x002E)      /* 0x109002e,17367086 */
#define E_TCS_PORTNUM_ERR                           (TCS_ERROR_CODE_BASE|0x002F)      /* 0x109002f,17367087 */
#define E_TCS_CFG_PARAM_NULL                        (TCS_ERROR_CODE_BASE|0x0030)      /* 0x1090030,17367088 */
#define E_TCS_CFG_PORTNUM_ERR                       (TCS_ERROR_CODE_BASE|0x0031)      /* 0x1090031,17367089 */
#define E_TCS_CFG_SYS_PROC_FAIL                     (TCS_ERROR_CODE_BASE|0x0032)      /* 0x1090032,17367090 */
#define E_TCS_CFG_INFO_DIFFER                       (TCS_ERROR_CODE_BASE|0x0033)      /* 0x1090033,17367091 */
#define E_TCS_CFG_IP_FAIL                           (TCS_ERROR_CODE_BASE|0x0034)      /* 0x1090034,17367092 */
#define E_TCS_CFG_ARP_FAIL                          (TCS_ERROR_CODE_BASE|0x0035)      /* 0x1090035,17367093 */
#define E_TCS_CFG_MAC_FAIL                          (TCS_ERROR_CODE_BASE|0x0036)      /* 0x1090036,17367094 */
#define E_TCS_CFG_MTU_FAIL                          (TCS_ERROR_CODE_BASE|0x0037)      /* 0x1090037,17367095 */
#define E_TCS_CFG_ROUTE_FAIL                        (TCS_ERROR_CODE_BASE|0x0038)      /* 0x1090038,17367096 */
#define E_TCS_CFG_ISCSI_FAIL                        (TCS_ERROR_CODE_BASE|0x0039)      /* 0x1090039,17367097 */
#define E_TCS_CFG_IPADD_FAIL                        (TCS_ERROR_CODE_BASE|0x003A)      /* 0x109003a,17367098 */
#define E_TCS_CFG_IPDEL_FAIL                        (TCS_ERROR_CODE_BASE|0x003B)      /* 0x109003b,17367099 */
#define E_TCS_CFG_ROUTE_ADD_FAIL                    (TCS_ERROR_CODE_BASE|0x003C)      /* 0x109003c,17367100 */
#define E_TCS_CFG_ROUTE_DEL_FAIL                    (TCS_ERROR_CODE_BASE|0x003D)      /* 0x109003d,17367101 */
#define E_TCS_CFG_IPADDR_ZERO                       (TCS_ERROR_CODE_BASE|0x003E)      /* 0x109003e,17367102 */
#define E_TCS_CFG_IPMASK_ZERO                       (TCS_ERROR_CODE_BASE|0x003F)      /* 0x109003f,17367103 */
#define E_TCS_CFG_IPADDR_IPMASK_ONE_ZERO            (TCS_ERROR_CODE_BASE|0x0040)      /* 0x1090040,17367104 */
#define E_TCS_CFG_IPMASK_FORMAT_ERR                 (TCS_ERROR_CODE_BASE|0x0041)      /* 0x1090041,17367105 */
#define E_TCS_CFG_IPMASK_LEN_ERR                    (TCS_ERROR_CODE_BASE|0x0042)      /* 0x1090042,17367106 */
#define E_TCS_CFG_IPADDR_AREA_ERR                   (TCS_ERROR_CODE_BASE|0x0043)      /* 0x1090043,17367107 */
#define E_TCS_CFG_PORTIP_EXIST                      (TCS_ERROR_CODE_BASE|0x0044)      /* 0x1090044,17367108 */
#define E_TCS_CFG_PORTIP_SAMEDOMAIN                 (TCS_ERROR_CODE_BASE|0x0045)      /* 0x1090045,17367109 */
#define E_TCS_CFG_IPADDR_NOMATCH                    (TCS_ERROR_CODE_BASE|0x0046)      /* 0x1090046,17367110 */
#define E_TCS_CFG_IPADDR_NOCFG                      (TCS_ERROR_CODE_BASE|0x0047)      /* 0x1090047,17367111 */
#define E_TCS_CFG_ROUTE_EXIST                       (TCS_ERROR_CODE_BASE|0x0048)      /* 0x1090048,17367112 */
#define E_TCS_CFG_ROUTE_NOT_EXIST                   (TCS_ERROR_CODE_BASE|0x0049)      /* 0x1090049,17367113 */
#define E_TCS_CFG_ROUTE_CLASH                       (TCS_ERROR_CODE_BASE|0x004A)      /* 0x109004a,17367114 */
#define E_TCS_CFG_ROUTE_NEXHOP_NODIRECT             (TCS_ERROR_CODE_BASE|0x004B)      /* 0x109004b,17367115 */
#define E_TCS_CFG_ROUTE_STATICNUM_MAX               (TCS_ERROR_CODE_BASE|0x004C)      /* 0x109004c,17367116 */
#define E_TCS_CFG_ROUTE_STATICNUM_ZERO              (TCS_ERROR_CODE_BASE|0x004D)      /* 0x109004d,17367117 */
#define E_TCS_CFG_MTU_AREA_ERR                      (TCS_ERROR_CODE_BASE|0x004E)      /* 0x109004e,17367118 */
#define E_TCS_CFG_MAC_FORMAT_ERR                    (TCS_ERROR_CODE_BASE|0x004F)      /* 0x109004f,17367119 */
#define E_TCS_CFG_MAC_REPEAT                        (TCS_ERROR_CODE_BASE|0x0050)      /* 0x1090050,17367120 */
#define E_TCS_CFG_ROUTE_MASK_ERR                    (TCS_ERROR_CODE_BASE|0x0051)      /* 0x1090051,17367121 */
#define E_TCS_CFG_HAVE_STATICROUTE                  (TCS_ERROR_CODE_BASE|0x0052)      /* 0x1090052,17367122 */
#define E_TCS_TEST_LENGTH_ERR                       (TCS_ERROR_CODE_BASE|0x0053)      /* 0x1090053,17367123 */
#define E_TCS_TEST_INTER_ERR                        (TCS_ERROR_CODE_BASE|0x0054)      /* 0x1090054,17367124 */
#define E_TCS_TEST_NUM_ERR                          (TCS_ERROR_CODE_BASE|0x0055)      /* 0x1090055,17367125 */
#define E_TCS_CFG_CTRLID_ERR                        (TCS_ERROR_CODE_BASE|0x0056)      /* 0x1090055,17367126 */
#define E_TCS_PM_ID                                 (TCS_ERROR_CODE_BASE|0x0100)      /* 0x1090100,17367296 */
#define E_TCS_PM_PORTSTAT_FUNCFLAG                  (TCS_ERROR_CODE_BASE|0x0101)      /* 0x1090101,17367297 */
#define E_TCS_PM_PORTSTAT_CTRLFLAG                  (TCS_ERROR_CODE_BASE|0x0102)      /* 0x1090102,17367298 */
#define E_TCS_PM_PORTSTAT_MEMSIZE                   (TCS_ERROR_CODE_BASE|0x0103)      /* 0x1090103,17367299 */



typedef struct
{
    SWORD32 sdwErrID;       /* the error id */
    const CHAR *pcErrTag;   /* the tag of the error */
    const CHAR *pcErrDes;   /* the description of the error */
    const CHAR *pcErrDesCH;
}_PACKED_1_ T_ErrorPack;

#define USE_COMMON_ERROR_PACK               /*add by maxiaopeng 04.21*/
#ifdef USE_COMMON_ERROR_PACK

static const  T_ErrorPack sg_tCommonError[] =
    {
        //  { E_GENERIC_REPEAT,                 "E_GENERIC_REPEAT",                 "Repeat setting the system value." },                   //举例

        /*******************************AMS 子系统************************************/
        {AMS_ERROR_CODE_XXXX,"AMS_ERROR_CODE_XXXX","Description of the error code.","错误码描述信息。"},

        /*******************************SCS 子系统************************************/
        {E_SCS_MEMORY_MALLOC,           "E_SCS_MEMORY_MALLOC",        "Failed to malloc memory space.", "内存空间申请失败"},
        {E_SCS_PARAMETER_ERROR,         "E_SCS_PARAMETER_ERROR",      "Parameter error.","参数错误"},
        {E_SCS_EXPANDER_NOTFIND,         "E_SCS_EXPANDER_NOTFIND",      "EXPANDER not find.","未找到对应的EXPANDER"},
        {E_SCS_SIG_PARAMETER_ERROR,     "E_SCS_SIG_PARAMETER_ERROR",  "Single ctrl  on running, Parameter error.","单控下的参数错误"},
        {E_SCS_DUAL_PARAMETER_ERROR,    "E_SCS_DUAL_PARAMETER_ERROR", "Dual   ctrls on running, Parameter error.","双控下的参数错误"},
        {E_SCS_BOOT_PEER_NOT_RUNNING,   "E_SCS_BOOT_PEER_NOT_RUNNING","Peer is not running.","对端不在运行状态"},
        {E_SCS_SYSTEM_ABNORMAL,   "E_SCS_SYSTEM_ABNORMAL","System abnormal,the operation failed.","系统异常，配置失败"},


        {E_SCS_CAPI_CONNECT_CREATE,     "E_SCS_CAPI_CONNECT_CREATE",  "Failed to create connect.","创建连接失败"},
        {E_SCS_CAPI_CONNECT_FAIL,       "E_SCS_CAPI_CONNECT_FAIL",    "Connect server failed.","连接建立失败"},
        {E_SCS_CAPI_SEND_REQ_FAIL,      "E_SCS_CAPI_SEND_REQ_FAIL",   "Request message sending failed.","请求消息发送失败"},
        {E_SCS_CAPI_RECV_RSP_ERROR,     "E_SCS_CAPI_RECV_RSP_ERROR",  "Response data receiving error.","接收应答消息错误"},
        {E_SCS_CAPI_RECV_MSG_TIMEOUT,   "E_SCS_CAPI_RECV_MSG_TIMEOUT","Response data receiving timeout,please try again later.",  "接收应答超时，请稍候再试"},
        {E_SCS_CAPI_ENDIAN_REVERSE_FAIL,"E_SCS_CAPI_ENDIAN_REVERSE_FAIL","Message endian reverse failed.",  "消息字节序转换失败"},
        {E_SCS_CAPI_SET_SVRSOCK_FAIL,   "E_SCS_CAPI_SET_SVRSOCK_FAIL",   "Set server socket failed.",  "设置服务器地址失败"},
        {E_SCS_CAPI_GET_SVRSOCK_FAIL,   "E_SCS_CAPI_GET_SVRSOCK_FAIL",   "Get server socket failed.",  "获取服务器地址失败"},
        {E_SCS_CAPI_PARAMETER_ERROR,    "E_SCS_CAPI_PARAMETER_ERROR",     "Parameter error.",  "参数错误"},

        {E_SCS_SMA_MSG_GENERATE,        "E_SCS_SMA_MSG_GENERATE",     "Sma message node generate error.",   "消息节点生成失败"},
        {E_SCS_SMA_MSG_MAXIMUM,         "E_SCS_SMA_MSG_MAXIMUM",      "Request message exceeds the maximum length.",  "请求消息的长度超过最大值"},
        {E_SCS_SMA_SOCKET_CONNECT,      "E_SCS_SMA_SOCKET_CONNECT",   "Failed to create socket(server) connect.",   "接收线程中，连接建立失败"},
        {E_SCS_SMA_THREAD_CREATE,       "E_SCS_SMA_THREAD_CREATE",    "Failed to create SMA processing thread.",  "线程创建失败"},
        {E_SCS_SMA_PEER_TIMEOUT,        "E_SCS_SMA_PEER_TIMEOUT",     "The peer process the request message timeout.",  "对端处理请求消息超时"},
        {E_SCS_SMA_SEND_REQ_TO_PEER,    "E_SCS_SMA_SEND_REQ_TO_PEER", "Send request message to peer failure.",  "发送请求消息到对端失败"},
        {E_SCS_SMA_PEER_NORUNNING,      "E_SCS_SMA_PEER_NORUNNING",   "The state of peer is not \"RUNNING\" or \"BOOTING\".",   "对端控制器非启动或运行状态"},
        {E_SCS_SMA_CMD_NO_EXIST,        "E_SCS_SMA_CMD_NO_EXIST",     "The request command(sma) does not exist.",  "请求命令不存在"},
        {E_SCS_SMA_CMD_NO_OWNER,        "E_SCS_SMA_CMD_NO_OWNER",     "This controller is not the owner of object(Block).",  "配置不允许在非归属控制器进行"},
        {E_SCS_SMA_CMD_NO_SUPPORT,      "E_SCS_SMA_CMD_NO_SUPPORT",   "This command is not supported in the environment",  "命令在该环境中不支持"},


        {E_SCS_API_MAPGRP_LUN,          "E_SCS_API_MAPGRP_LUN",        "The LUN does not exist in any MapGrp.",  "该逻辑单元不存在于任何映射组"},
        {E_SCS_API_MAPGRP_HOST,         "E_SCS_API_MAPGRP_HOST",       "The Host does not exist in any MapGrp.",  "该主机不存在于任何映射组"},
        {E_SCS_API_MAPGRP_MAX_NUM,      "E_SCS_API_MAPGRP_MAX_NUM",    "The numble of MapGrp reached the maximum in system.",   "系统中的映射组数目已经达到最大值"},
        {E_SCS_API_MAPGRP_EXIST,        "E_SCS_API_MAPGRP_EXIST",      "The MapGrp already exists in system.",  "映射组已经存在"},
        {E_SCS_API_MAPGRP_WRITE_CFG,    "E_SCS_API_MAPGRP_WRITE_CFG",  "Failed to write information(MapGrp) to the storage media.",  "映射组信息写文件失败"},
        {E_SCS_API_MAPGRP_LUN_OR_HOST,  "E_SCS_API_MAPGRP_LUN_OR_HOST","There are LUNs or Hosts in the MapGrp.",  "映射组中存在逻辑单元或主机，不允许删除"},
        {E_SCS_API_MAPGRP_NOEXIST,      "E_SCS_API_MAPGRP_NOEXIST",    "The MapGrp does not exist in system.",  "映射组不存在"},
        {E_SCS_API_MAPGRP_LUN_MAX_NUM,  "E_SCS_API_MAPGRP_LUN_MAX_NUM","The numble of LUN reached the maximum in the MapGrp.",  "映射组中逻辑单元的数目达到最大值"},
        {E_SCS_API_MAPGRP_VOL_EXIST,    "E_SCS_API_MAPGRP_VOL_EXIST",  "The Vol already exists in the MapGrp.",  "映射组中已经存在该卷"},
        {E_SCS_API_MAPGRP_LUN_NOEXIST,  "E_SCS_API_MAPGRP_LUN_NOEXIST","The LUN does not exist in the MapGrp.",  "映射组中该逻辑单元不存在"},
        {E_SCS_API_MAPGRP_HOST_EXIST,   "E_SCS_API_MAPGRP_HOST_EXIST", "The Host already exists in a MapGrp.",  "该主机已经存在于某个映射组中"},
        {E_SCS_API_MAPGRP_HOST_MAX_NUM, "E_SCS_API_MAPGRP_HOST_MAX_NUM","The numble of Host reached the maximum in the MapGrp.",  "映射组中主机数目达到最大"},
        {E_SCS_API_MAPGRP_HOST_NOEXIST, "E_SCS_API_MAPGRP_HOST_NOEXIST","The Host does not exist in the MapGrp.",  "映射组中不存在主机"},
        {E_SCS_API_MAPGRP_ID_INVALID,   "E_SCS_API_MAPGRP_ID_INVALID",  "The MapGrp id is invalid.",  "映射组ID非法"},
        {E_SCS_API_MAPGRP_NAME_EXIST,   "E_SCS_API_MAPGRP_NAME_EXIST",  "The new name of MapGrp already exists in system.",  "映射组名称已经存在"},
        {E_SCS_API_MAPGRP_MAX_LUN_EXIST,"E_SCS_API_MAPGRP_MAX_LUN_EXIST","The number of the group that the vol(snap/clone) can be added to reached a maximum",  "该卷对象所允许加入的映射组数目已达最大值"},

        {E_SCS_API_NAME_INVALID,        "E_SCS_API_NAME_INVALID",       "The name is invalid.",  "对象名称非法"},
        {E_SCS_API_VOL_ADD_MAPGRP,      "E_SCS_API_VOL_ADD_MAPGRP",     "Freezing VD is forbidden when vol added to MapGrp.",  "有卷加入映射组时禁止冻结虚拟盘"},

        {E_SCS_API_LUN_EXIST,             "E_SCS_API_LUN_EXIST",          "The LUN already exists in system.",  "逻辑单元已经存在"},
        {E_SCS_API_LUN_WRITE_CFG,         "E_SCS_API_LUN_WRITE_CFG",      "Failed to write information(Vol) to the storage media.",  "卷信息写配置文件失败"},
        {E_SCS_API_LUN_NOEXIST,           "E_SCS_API_LUN_NOEXIST",        "The LUN does not exist in system.",  "逻辑单元不存在"},
        {E_SCS_API_LUN_GLOBAL_ID_INVALID, "E_SCS_API_LUN_ID_INVALID",     "The LUN global id is invalid.",  "逻辑单元全局标识号无效"},
        {E_SCS_API_LUN_ID_INVALID,        "E_SCS_API_LUN_ID_INVALID",     "The LUN id is invalid.",    "逻辑单元在映射组中的标识号无效"},
        {E_SCS_API_LUN_ID_EXIST,          "E_SCS_API_LUN_ID_EXIST",       "In MapGrp, the LUN id already exists.",  "逻辑单元在映射组中的标识号已经存在"},
        {E_SCS_API_LUN_ADDED_TO_MAPGRP,   "E_SCS_API_LUN_ADDED_TO_MAPGRP","The LUN has been added to MapGrp, can not be deleted.",  "逻辑单元已经被加入到映射组，无法删除"},
        {E_SCS_API_LUN_VOL_OFFLINE,       "E_SCS_API_LUN_VOL_OFFLINE",    "The corresponding vol of the LUN is offline",  "逻辑单元对应的卷不在线"},

        {E_SCS_API_ROLETYPE_ERROR,              "E_SCS_API_ROLETYPE_ERROR",              "Role type of the net device is error.",  "设备类型错误"},
        {E_SCS_API_DEVICEID_ERROR_FOR_ROLETYPE, "E_SCS_API_DEVICEID_ERROR_FOR_ROLETYPE", "The device id is invalid for the roletype you have chosen.",  "该角色类型下，设备编号错误"},
        {E_SCS_API_GATEWAY_ERROR,               "E_SCS_API_GATEWAY_ERROR",               "Gateway error.",  "网关错误"},
        {E_SCS_API_NETCFG_ERROR,                "E_SCS_API_NETCFG_ERROR",                "The information of netconfig should not be null.",  "网络配置信息不能为空"},
        {E_SCS_API_DEVICE_ERROR,                "E_SCS_API_DEVICE_ERROR",                "The device id is invalid.",  "设备编号错误"},
        {E_SCS_API_IP_MASK_BOTH_ZERO,           "E_SCS_API_IP_MASK_BOTH_ZERO",           "Ip and netmask of the business must be configured at the same time.",  "业务口 的IP和掩码必须同时配置"},
        {E_SCS_API_IP_HOSTBITS_ERROR,           "E_SCS_API_IP_HOSTBITS_ERROR",           "The hostbits of ip should not be all 0 or 1.",  "IP地址的主机位不能全0或全1"},
        {E_SCS_API_NETWORKSEGMENT_ERROR,        "E_SCS_API_NETWORKSEGMENT_ERROR",        "Ip of business and management should not be in the same network segment.",  "管理口和业务口的IP不允许在同一网段"},
        {E_SCS_API_NETWORKSEGMENT_MANAGEMENT,   "E_SCS_API_NETWORKSEGMENT_MANAGEMENT",   "Ip of management and management should not be in the same network segment.",  "管理口与管理口的IP不允许在同一网段"},
        {E_SCS_API_NETWORKSEGMENT_BUSYNESS,     "E_SCS_API_NETWORKSEGMENT_BUSYNESS",     "Ip of business and business should not be in the same network segment.",  "业务口与业务口的IP不允许在同一网段"},




        {E_SCS_API_OTHER_WRITE_CFG,      "E_SCS_API_OTHER_WRITE_CFG",      "Failed to write information to the storage media.",  "其他类信息写文件失败"},
        {E_SCS_API_OTHER_READ_CFG,       "E_SCS_API_OTHER_READ_CFG",       "Failed to read information from the storage media.",  "其他类信息读文件失败"},
        {E_SCS_API_OTHER_GET_NET_INFO,   "E_SCS_API_OTHER_GET_NET_INFO",   "Failed to get the information of netwok device.",   "获取网络设备信息失败"},
        {E_SCS_API_OTHER_NET_IP_ERROR,   "E_SCS_API_OTHER_NET_IP_ERROR",   "IP address is invalid.",  "IP地址无效"},
        {E_SCS_API_OTHER_NET_MASK_ERROR, "E_SCS_API_OTHER_NET_MASK_ERROR", "Subnet mask is invalid.",  "子网掩码错误"},
        {E_SCS_API_OTHER_NET_IP_EXIST,   "E_SCS_API_OTHER_NET_IP_EXIST",   "The IP already exists in the system.",  "IP 在系统中已经存在"},
        {E_SCS_API_OTHER_NET_MAC_EXIST,  "E_SCS_API_OTHER_NET_MAC_EXIST",  "The MAC already exists in the system.",  "MAC在系统中已经存在"},
        {E_SCS_API_OTHER_ROUTE_GATEWAY_ERROR,   "E_SCS_API_OTHER_ROUTE_GATEWAY_ERROR",   "GateWay is invalid.",  "网关错误"},
        {E_SCS_API_OTHER_ROUTE_NO_EXIST,   "E_SCS_API_OTHER_ROUTE_NO_EXIST",   "The route does not exist.",  "路由不存在"},
        {E_SCS_API_OTHER_NET_MANA_IP_NULL, "E_SCS_API_OTHER_NET_MANA_IP_NULL", "The ip address of management is invalid.",  "管理口IP地址无效"},
        {E_SCS_API_OTHER_NET_IP_RESERVED,"E_SCS_API_OTHER_NET_IP_RESERVED", "The ip address is reserved, the configuration is not allowed..",  "该IP为系统保留，不允许配置"},

        {E_SCS_API_OTHER_MAC_INVALID,     "E_SCS_API_OTHER_MAC_INVALID",      "The MAC address is not valid.",  "MAC地址值非法"},
        {E_SCS_API_OTHER_SET_MAC_BY_SRV,  "E_SCS_API_OTHER_SET_MAC_BY_SRV.",  "Failed to set MAC address.",  "设置MAC 地址失败"},
        {E_SCS_API_OTHER_WRITE_MAC_EEPROM_ERROR,  "E_SCS_API_OTHER_WRITE_MAC_EEPROM_ERROR",  "Failed to write MAC information to the EEPROM.",  "MAC 信息写入EEPROM 失败"},
        {E_SCS_API_OTHER_GET_PORT_MTU_MAX_VALUE,  "E_SCS_API_OTHER_GET_PORT_MTU_MAX_VALUE.",  "Failed to get max MTU value of the port.",  "获取该端口的最大MTU值失败"},
        {E_SCS_API_OTHER_PORT_MTU_INVALID,  "E_SCS_API_OTHER_PORT_MTU_INVALID.",  "The MTU is not valid.",  "MTU值非法"},

        {E_SCS_API_TASK_SET_NOT_ALLOWED, "E_SCS_API_TASK_SET_NOT_ALLOWED", "Conflict with the status of the task, it is not allowed to perform this operation.",  "该操作与任务所处的状态相冲突"},
        {E_SCS_API_TASK_SET_NOT_SUPPORTED, "E_SCS_API_TASK_SET_NOT_SUPPORTED", "The task of this operation is not supported.",  "对任务的该项操作暂不支持"},
        {E_SCS_API_TASK_TYPE_NOT_EXIST, "E_SCS_API_TASK_TYPE_NOT_EXIST", "The task type is not supported.",  "任务类型暂不支持"},
        {E_SCS_API_TASK_EXIST, "E_SCS_API_TASK_EXIST", "The task already exists",  "该任务已经存在"},
        {E_SCS_API_TASK_NOT_EXIST, "E_SCS_API_TASK_NOT_EXIST", "The task does not exist, the operation is not allowed.",  "任务不存在，不允许此项操作"},
        {E_SCS_API_TASK_NOT_RUNNING, "E_SCS_API_TASK_NOT_RUNNING", "The task is not running, do not allow the operation.",  "任务非运行状态，不允许此项操作"},
        {E_SCS_API_TASK_MAX_NUM, "E_SCS_API_TASK_MAX_NUM", "The number of tasks is already the largest.",  "系统中的后台任务数目已经达到最大"},
        {E_SCS_API_STOP_HOTSPARE_SCAN_FAIL, "E_SCS_API_STOP_HOTSPARE_SCAN_FAIL", "Failed to stop the scan task of hotspare disk.",  "停止热备盘的扫描任务任务失败"},

        {E_SCS_API_SCHEDULE_EXIST, "E_SCS_API_SCHEDULE_EXIST", "The same schedule task already exists.",  "相同的计划任务已经存在"},
        {E_SCS_API_SCHEDULE_OUTDATE, "E_SCS_API_SCHEDULE_OUTDATE", "The schedule task is out of date.",  "该计划任务过期"},
        {E_SCS_API_SCHEDULE_TASKID_INVALID, "E_SCS_API_SCHEDULE_TASKID_INVALID", "The schedule task ID is invalid, the task does not exist.",  " 计划任务序号无效，该任务不存在"},
        {E_SCS_API_SCHEDULE_MAX_NUM, "E_SCS_API_SCHEDULE_MAX_NUM", "The number of schedule tasks is already the largest.",  "系统中的计划任务数目已经达到最大值"},
        {E_SCS_API_TASK_RUNNING_NUM_UP_LIMIT, "E_SCS_API_TASK_RUNNING_NUM_UP_LIMIT", "The number of running task  exceeds limits, do not allow the operation.",  "正在运行的任务已达上限，不允许此项操作"},

        {E_SCS_API_VOL_MAP_ABNORMAL,     "E_SCS_API_VOL_MAP_ABNORMAL", "The vol(snap/clone) does not exist in system.",  "系统中该卷(或者快照、克隆卷)不存在"},
        {E_SCS_API_VOL_EXIST_ABNORMAL,   "E_SCS_API_VOL_EXIST_ABNORMAL", "Abnormal vol(snap/clone) is not allowed to perform the operation.",  "仅单端可见的卷(或者快照、克隆卷)不允许执行这项操作"},
        {E_SCS_API_PEERSTATE_NO_RUNNING, "E_SCS_API_PEERSTATE_NO_RUNNING", "Can not set when single control.",  "单控状态不允许设置"},


        {E_SCS_API_HDD_ABNORMAL,       "E_SCS_API_HDD_ABNORMAL",       "The hard disk is abnormal,it is not visible in a controller .",  "磁盘异常，在一端控制器中不可见"},
        {E_SCS_API_HDD_HEALTH_STATE,   "E_SCS_API_HDD_HEALTH_STATE",   "The health status of disk is abnormal.",  "磁盘健康状态异常"},
        {E_SCS_API_HDD_NOEXIST,        "E_SCS_API_HDD_NOEXIST",        "The hard disk does not exist.",  "磁盘信息不存在"},
        {E_SCS_API_HDD_UNUSED,         "E_SCS_API_HDD_UNUSED",         "The state of hard disk is not \"UNUSED\".",  "磁盘非空闲状态"},
        {E_SCS_API_HDD_DATA,           "E_SCS_API_HDD_DATA",           "The state of hard disk is \"DATA\".",  "磁盘为数据盘"},
        {E_SCS_API_HDD_HOTSPARE,       "E_SCS_API_HDD_HOTSPARE",       "The state of hard disk is not \"HOTSPARE\".",  "磁盘非热备状态"},
        {E_SCS_API_HDD_NO_LOCAL,       "E_SCS_API_HDD_NO_LOCAL",       "The hard disk is not local,this operation is not allowed.",  "磁盘不是本地盘，不允许操作"},
        {E_SCS_API_HDD_NO_FOREIGN,     "E_SCS_API_HDD_NO_FOREIGN",     "The hard disk is not foreign,this operation is not allowed.",  "磁盘不是外来盘，不允许操作"},
        {E_SCS_API_HDD_SET_POWER_MODE_ERROR,     "E_SCS_API_HDD_SET_POWER_MODE_ERROR",     "Set the disk power mode failed.",  "设置磁盘的能耗模式失败"},
        {E_SCS_API_HDD_CAPACITY,       "E_SCS_API_HDD_CAPACITY",       "hotspare disk capacity is smaller than the minimum member disk capacity in VD.",  "热备盘容量小于虚拟盘最小成员盘容量"},
        {E_SCS_API_HDD_TYPE_UNFIT,     "E_SCS_API_HDD_TYPE_UNFIT",     "The type of disk is inconsistent in two controllers.",  "双控上磁盘类型不一致"},
        {E_SCS_API_HDD_DATA_IMPORT,    "E_SCS_API_HDD_DATA_IMPORT",    "Foreign Data disk can not import,please execute VD roaming immigrate",  "外来数据盘不允许导入，可通过虚拟盘漫游迁入处理"},
        {E_SCS_API_HDD_SCANNING,       "E_SCS_API_HDD_SCANNING",       "Refused to set the disk type during scanning the disk",  "磁盘上存在扫描任务，不允许设置磁盘类型"},
        {E_SCS_API_HDD_NOT_SCANNING,   "E_SCS_API_HDD_NOT_SCANNING",   "Disk scanning task have not begun",  "磁盘扫描任务尚未开启"},
        {E_SCS_API_ENOUGH_FIT_DISK,    "E_SCS_API_ENOUGH_FIT_DISK",    "No enough suitable disks(disk capacity is not same or disk is scanning) to create virtual disk.",  "没有足够合适磁盘(磁盘容量不一致或磁盘在扫盘)，无法创建虚拟盘"},
        {E_SCS_API_HDD_ABNORMAL_SCAN,  "E_SCS_API_HDD_ABNORMAL_SCAN",  "The hard disk is abnormal,failed to perform a scan.",  "磁盘异常，执行扫描失败"},
        {E_SCS_API_PD_SCANNING,        "E_SCS_API_PD_SCANNING",         "Refused to create VD(Pool) during scanning disk.",  "磁盘上有扫描任务，不允许创建虚拟盘(存储池)"  },
        {E_SCS_API_DISK_SCAN,          "E_SCS_API_DISK_SCAN",         "Refused to execute this operation during scanning disk.",  "磁盘上有扫描任务，不允许执行本操作"  },
        {E_SCS_API_PD_SCAN_DATA_ERROR, "E_SCS_API_PD_SCAN_DATA_ERROR",  "Refused to scan the data disk.",  "数据盘不允许扫描"  },
        {E_SCS_API_PD_SCAN_UNKNOWN_ERROR, "E_SCS_API_PD_SCAN_UNKNOWN_ERROR", "Refused to scan the unknown disk.",  "未知盘不允许扫描"  },
        {E_SCS_NO_ENOUGH_UNUSED_DISKS, "E_SCS_NO_ENOUGH_UNUSED_DISKS", "No enough unused disks to create virtual disk or pool.",  "没有足够的空闲盘，无法创建虚拟盘(存储池)"  },
        {E_SCS_NO_ENOUGH_UNUSED_PD,    "E_SCS_NO_ENOUGH_UNUSED_PD",    "No enough unused disks.",                                 "没有足够的空闲盘"  },
        {E_SCS_NO_ENOUGH_RIGHT_UNUSED, "E_SCS_NO_ENOUGH_RIGHT_UNUSED", "No enough suitable unused disks.",                        "没有足够可用的空闲盘"  },
        {E_SCS_API_HDD_SLOT_ABNORMAL, "E_SCS_API_HDD_SLOT_ABNORMAL", "The slot of disk is abnormal, the operation is not allowed.",  "磁盘槽位信息异常，不允许执行该操作"  },
        {E_SCS_API_HDD_NOT_READY,     "E_SCS_API_HDD_NOT_READY",     "The disk is not ready, the operation is not allowed.",  "磁盘未准备就绪，不允许执行该操作"  },


        {E_SCS_API_VD_NO_DEGRADE,                "E_SCS_API_VD_NO_DEGRADE",               "The state of VD is not \"DEGRADE\".",  "虚拟盘非降级状态，不允许重建"},
        {E_SCS_API_VD_LOCAL_NOEXIST,             "E_SCS_API_VD_LOCAL_NOEXIST",            "The VD does not exist in local.",  "本端虚拟盘不存在，不允许执行该操作"},
        {E_SCS_API_VD_PEER_NOEXIST,              "E_SCS_API_VD_PEER_NOEXIST",             "The VD does not exist in peer.",  "对端虚拟盘不存在，不允许执行该操作"},
        {E_SCS_API_VD_NOEXIST,                   "E_SCS_API_VD_NOEXIST",                  "The VD does not exist.",  "虚拟盘不存在"},
        {E_SCS_API_VD_LEVEL,                     "E_SCS_API_VD_LEVEL",                    "VD raid level is 0, not allow to rebulid.",  "虚拟盘级别为0，不允许重建"},
        {E_SCS_API_VD_MEMBER_LIST_INCOMPATIBLE,  "E_SCS_API_VD_MEMBER_LIST_INCOMPATIBLE", "The member list of VD in two controller is not same and not in a subset.",  "虚拟盘两端控制器成员盘列表不一致也不呈子集关系"},
        {E_SCS_API_VD_MEMBER_LIST_NOT_SAME,      "E_SCS_API_VD_MEMBER_LIST_NOT_SAME",     "The member list of VD in two controller is not same .",  "虚拟盘两端控制器成员盘列表不一致"},
        {E_SCS_API_VD_STATE_NOT_SAME,            "E_SCS_API_VD_STATE_NOT_SAME",           "The VD state on two controller is not same .",  "虚拟盘两端控制器状态不一致"},
        {E_SCS_API_VD_HOTSPARE_MAX,              "E_SCS_API_VD_HOTSPARE_MAX",             "At most 4 spare disks can be added each time",  "一次最多添加四个热备盘参加重建"},
        {E_SCS_API_VD_DELAY_RECOVERING,          "E_SCS_API_VD_DELAY_RECOVERING",         "Vd delay recover is running, operation is not allowed",  "VD延迟恢复中,禁止操作"},


        {E_SCS_API_MIRROR_DISK_ABNORMAL,     "E_SCS_API_MIRROR_DISK_ABNORMAL",      "The state of remote disk is abnormal.",  "远程磁盘状态异常"},
        {E_SCS_API_MIRROR_DISK_USED,         "E_SCS_API_MIRROR_DISK_USED",          "The remote disk has been used.",  "远程磁盘已经被使用"},
        {E_SCS_API_MIRROR_DISK_NOEXIST,      "E_SCS_API_MIRROR_DISK_NOEXIST",       "The remote disk does not exist.", "远程磁盘不存在"},
        {E_SCS_API_MIRROR_CONNECT_NOEXIST,   "E_SCS_API_MIRROR_CONNECT_NOEXIST",    "The mirror connection does not exist.",  "镜像连接不存在"},
        {E_SCS_API_MIRROR_CONNECT_MAX,       "E_SCS_API_MIRROR_CONNECT_MAX",        "The number of connections has reached the maximum.",  "镜像连接数目已经达到最大值"},
        {E_SCS_API_MIRROR_DISK_MAX,          "E_SCS_API_MIRROR_DISK_MAX",           "The number of remote disks has reached the maximum.",  "远程磁盘数目已经达到最大值"},
        {E_SCS_API_NEXUS_MIRROR_EXIST,       "E_SCS_API_NEXUS_MIRROR_EXIST",        "Mirrors exist, the Fc connection is allowed to delete.",  "远程镜像存在，FC 链接不允许删除"},
        {E_SCS_API_FC_CONNECT_EXIST,         "E_SCS_API_FC_CONNECT_EXIST",          "The mirror Connection already exists.",  "镜像连接已经存在"},
        {E_SCS_API_FC_CONNECT_MAX_CTRL,      "E_SCS_API_FC_CONNECT_MAX_CTRL",       "The number of connection on the controller has reached the maximum.",  "该控制器上的连接数目已经达到最大值"},


        {E_SCS_API_VOL_MAPPED,         "E_SCS_API_VOL_MAPPED",         "The VOL has been mapped.",  "卷已经被映射，不允许执行删除操作"},
        {E_SCS_API_VOL_NOT_GOOD,       "E_SCS_API_VOL_NOT_GOOD",       "The state of VOL is not \"GOOD\".",  "卷状态异常"},
        {E_SCS_API_VOL_NOT_NORMAL_VOL, "E_SCS_API_VOL_NOT_NORMAL_VOL", "It is not normal Vol,this operation is not allowed.",  "不是普通卷，不允许执行该操作"},
        {E_SCS_API_VD_FAULT,           "E_SCS_API_VD_FAULT",           "The state of VD is  fault,the cache policy of the vol on this vd is not allowed to modify .",  "虚拟盘为故障状态，不允许修改缓存策略"},
        {E_SCS_API_VD_IS_RECING,       "E_SCS_API_VD_IS_RECING",       "The state of VD is \"RECING\"",  "虚拟盘正在降级重建中，不允许操作"},
        {E_SCS_API_VOL_STATE_NOT_SAME, "E_SCS_API_VOL_STATE_NOT_SAME", "The state of Vol on two controller is not same.",  "该卷的状态在双控上已经异常，不允许修改缓存策略"},

        {E_SCS_API_CVOL_INIT_RENAME_ERROR,       "E_SCS_API_CVOL_INIT_RENAME_ERROR",       "If the state of the clone is init , not allow to rename this clone.",  "克隆卷处于初始同步状态中，不允许重命名此克隆卷"},
        {E_SCS_API_CVOL_SYNCING_RENAME_ERROR,       "E_SCS_API_CVOL_SYNCING_RENAME_ERROR",       "If the state of the clone is syncing , not allow to rename this clone.",  "克隆卷处于同步状态中，不允许重命名此克隆卷"},
        {E_SCS_API_CVOL_REVERSE_SYNCING_RENAME_ERROR,       "E_SCS_API_CVOL_REVERSE_SYNCING_RENAME_ERROR",       "If the state of the clone is reverse syncing , not allow to rename this clone.",  "克隆卷处于初始反同步状态中，不允许重命名此克隆卷"},

        {E_SCS_API_SVOL_NOT_ACTIVE,    "E_SCS_API_SVOL_NOT_ACTIVE",    "The state of Svol is not \"ACTIVE\".",  "快照卷不是\"ACTIVE\"状态"},
        {E_SCS_API_VOL_NUM_MAXIMUM,    "E_SCS_API_VOL_NUM_MAXIMUM",    "The total number of Vol(Svol) has reached maximum(1024).",  "卷(包括快照卷、克隆卷)数量达到最大"},
        {E_SCS_API_VOL_ADDED_TO_MAPGRP,"E_SCS_API_VOL_ADDED_TO_MAPGRP","The Vol has been added to MapGrp, this operation is not allowed.",  "卷已经加入到映射组中，不允许执行该操作"},
        {E_SCS_API_VOL_DEVICE_OPEN_FAILED, "E_SCS_API_VOL_DEVICE_OPEN_FAILED",  "Failed to open the Vol(Svol/Cvol) device.",  "卷设备打开失败(创建逻辑单元失败)"},
        {E_SCS_API_VOL_DEVICE_CLOSE_FAILED,"E_SCS_API_VOL_DEVICE_CLOSE_FAILED", "Failed to close the Vol(Svol/Cvol) device.",  "卷设备关闭失败(删除逻辑单元失败)"},
        {E_SCS_API_CVOL_RELATION,"E_SCS_API_CVOL_RELATION", "The CloneVol is unfractured, and not allowed to add to Grp.",  "克隆卷与源卷关联，不允许加入映射组"},
        {E_SCS_API_VOL_SINGLE_WRITE_BACK,"E_SCS_API_VOL_SINGLE_WRITE_BACK", "Not allow to create vol with write back policy, while the system is single ctrl.",  "单控且写回策略，不支持创建卷"},
        {E_SCS_API_VOL_MIRROR_OFF_WRITE_BACK,"E_SCS_API_VOL_MIRROR_OFF_WRITE_BACK", "Not allow to create vol with write back policy ,when Mirror state is off .",  "已配置为关镜像，不允许再创建写回策略的卷"},
        {E_SCS_API_VOL_WRITE_BACK_IN_SINGLE,"E_SCS_API_VOL_WRITE_BACK_IN_SINGLE", "Not allow to set vol to  write back policy ,while the system being in single ctrl .",  "单控情况下，不允许配置卷为写回策略"},

        /* 以下三个add by tao 2012-05-22 */
        {E_SCS_API_VOL_WRITE_BACK_FAN_FAULT,"E_SCS_API_VOL_WRITE_BACK_FAN_FAULT", "fan fault, can only be write through.",  "FAN故障时自动直写预设项已设置，cache策略只能为写穿"},
        {E_SCS_API_VOL_WRITE_BACK_BBU_FAULT,"E_SCS_API_VOL_WRITE_BACK_BBU_FAULT", "bbu fault, can only be through.",  "BBU故障时自动直写预设项已设置，cache策略只能为写穿"},
        {E_SCS_API_VOL_WRITE_BACK_POWER_FAULT,"E_SCS_API_VOL_WRITE_BACK_POWER_FAULT", "power fault, can only be write through.",  "POWER故障时自动直写预设项已设置，cache策略只能为写穿"},


        {E_SCS_API_VOL_MIRROR_OFF_WRITE_BACK_ERROR,"E_SCS_API_VOL_MIRROR_OFF_WRITE_BACK_ERROR", "Not allow to set vol with write back policy ,when Mirror state is off .",  "已配置为关镜像，不允许配置写回策略的卷"},
        {E_SCS_API_SET_VOL_CACHE_ATTR_FAILED,  "E_SCS_API_SET_VOL_CACHE_ATTR_FAILED",  "Set vol cache policy failed.", "设置源卷的cache策略失败"},
        {E_SCS_API_SEND_SNAPSHOT_AGENT_FAILED, "E_SCS_API_SEND_SNAPSHOT_AGENT_FAILED", "Send snapshot notice failed.", "发送快照通知失败"},
        {E_SCS_API_FIND_AGENT_NODE_FAILED, "E_SCS_API_FIND_AGENT_NODE_FAILED", "Can not find the snapshot agent node.", "找不到快照代理节点"},
        {E_SCS_API_RCV_AGENT_RSP_FAILED, "E_SCS_API_RCV_AGENT_RSP_FAILED", "Receive snapshot agent respond failed.", "接收快照代理响应失败"},
        {E_SCS_API_AGENT_EXECUTE_FAILED, "E_SCS_API_AGENT_EXECUTE_FAILED", "Snapshot agent execute failed.", "快照代理端操作失败"},



        {E_SCS_API_CVOL_VD_FREEZE,"E_SCS_API_CVOL_VD_FREEZE", "The clone in the frozen vd, not allowed it to add to mapping group.",  "克隆卷所在虚拟盘是冻结状态，不允许该克隆卷加入映射组"},
        {E_SCS_API_VOL_VD_FREEZE,"E_SCS_API_VOL_VD_FREEZE", "The vol in the frozen vd, not allowed it to add to mapping group.",  "卷所在虚拟盘是冻结状态，不允许此卷加入映射组"},
        {E_SCS_API_SVOL_VD_FREEZE,"E_SCS_API_SVOL_VD_FREEZE", "The snap in the frozen vd, not allowed it to add to mapping group.",  "快照卷卷所在虚拟盘是冻结状态，不允许此快照卷加入映射组"},
        {E_SCS_API_CVOL_ADDED_TO_MAPGRP,"E_SCS_API_CVOL_ADDED_TO_MAPGRP","This Clone has been added to MapGrp, this operation is not allowed.",  "克隆卷已经加入到映射组中，不允许执行该操作"},

        {E_SCS_API_VOL_VD_FOREIGN,"E_SCS_API_VOL_VD_FOREIGN", "The vol in the foreign vd, not allowed it to add to mapping group.",  "外来vd 中的卷，不允许其加入映射组"},
        {E_SCS_API_CVOL_VD_FOREIGN,"E_SCS_API_CVOL_VD_FOREIGN", "The clone in the foreign vd, not allowed it to add to mapping group.",  "外来vd 中的克隆卷，不允许其加入映射组"},
        {E_SCS_API_SVOL_VD_FOREIGN,"E_SCS_API_SVOL_VD_FOREIGN", "The snap in the foreign vd, not allowed it to add to mapping group.",  "外来vd 中的快照，不允许其加入映射组"},

        {E_SCS_API_ISCSI_INIT_MAX_NUM, "E_SCS_API_ISCSI_INIT_MAX_NUM", "The number of Initiator is more than the maximum.",  "启动器的数目超过最大值"},

        {E_FC_SUBCARD_NOT_EXIST, "E_FC_SUBCARD_NOT_EXIST", "The FC SubCard is not exist.",  "FC子卡不存在"},
        {E_SCS_API_CACHE_STATUS_OFF,    "E_SCS_API_CACHE_STATUS_OFF",     "Cache IO statistics switch is off.",  "高速缓存统计状态为关"},
        {E_SCS_API_IOLUN_STATUS_OFF,    "E_SCS_API_IOLUN_STATUS_OFF",     "System statistics switch is off.",   "系统统计状态为关"},
        {E_SCS_API_ISCSI_STATUS_OFF,    "E_SCS_API_ISCSI_STATUS_OFF",     "ISCSI IO statistics switch is off.",  "ISCSI统计状态为关"},

        {E_SCS_API_ISCSI_NEWNAME_INVALID, "E_SCS_API_ISCSI_NEWNAME_INVALID", "New iscsi name is illegal.",  "新的ISCSI名称非法"},


        /* 版本管理错误码 added by wangjing 2011-08-22 */
        {E_SCS_API_SYNC_FILE_FAILED,           "E_SCS_API_SYNC_FILE_FAILED",           "synchronize file to peer failed.",  "同步文件到对端失败"},
        {E_SCS_API_GET_UPLOAD_PROGRESS_FAILED, "E_SCS_API_GET_UPLOAD_PROGRESS_FAILED", "get upload progress failed.",       "查询文件上传进度失败"},
        {E_SCS_API_CANCEL_UPLOAD_FAILED,       "E_SCS_API_CANCEL_UPLOAD_FAILED",       "cancel file upload failed.",        "取消文件上传失败"},
        {E_SCS_API_VER_FILE_CHK_FAILED,        "E_SCS_API_VER_FILE_CHK_FAILED",        "version file check failed.",        "版本文件校验错误"},
        {E_SCS_API_UPDATE_FILE_FAILED,         "E_SCS_API_UPDATE_FILE_FAILED",         "update file verison failed.",       "更新文件版本失败"},
        {E_SCS_API_GET_VERSION_FAILED,         "E_SCS_API_GET_VERSION_FAILED",         "get file version failed.",          "查询文件版本失败"},
        {E_SCS_API_VERSION_FILE_NOT_EXIST,     "E_SCS_API_VERSION_FILE_NOT_EXIST",     "file does not exist.",              "版本文件不存在"},
        {E_SCS_API_FILE_PATH_NOT_EXIST,        "E_SCS_API_FILE_PATH_NOT_EXIST",        "file path not exist.",              "文件路径不存在"},
        {E_SCS_API_NETWORK_NOT_REACHABLE,      "E_SCS_API_NETWORK_NOT_REACHABLE",      "network is not reachable.",         "网络不可达"},
        {E_SCS_API_USER_PASSWORD_NOT_CORRECT,  "E_SCS_API_USER_PASSWORD_NOT_CORRECT",  "username or password not correct.", "用户名或密码不正确"},
        {E_SCS_API_INVALID_FILE_SIZE,          "E_SCS_API_INVALID_FILE_SIZE",          "invalid file size.",                "文件大小不合法"},
        {E_SCS_API_FILENAME_REPEATED,          "E_SCS_API_FILENAME_REPEATED",          "filename exist, please change it.", "文件名已存在"},
        {E_SCS_API_WRONG_VERTYPE,              "E_SCS_API_WRONG_VERTYPE",              "wrong version type.",               "错误的版本文件类型"},
        {E_SCS_API_FTP_GETFILEDATA_FAIL,       "E_SCS_API_FTP_GETFILEDATA_FAIL",       "ftp get file data fail.",           "上传文件数据失败"},
        {E_SCS_API_FTP_PUTFILEDATA_FAIL,       "E_SCS_API_FTP_PUTFILEDATA_FAIL",       "ftp put file data fail.",           "下载文件数据失败"},
        {E_SCS_API_FILE_EXIST_DUAL,            "E_SCS_API_FILE_EXIST_DUAL",            "file has already existed in local and peer, please rename file and try again.",  "文件在本端和对端均已存在,请重命名后再尝试"},
        {E_SCS_API_FILE_EXIST_LOCAL,           "E_SCS_API_FILE_EXIST_LOCAL",           "file has already existed in local, please rename file and try again.",           "文件在本端已存在,请重命名后再尝试"},
        {E_SCS_API_FILE_EXIST_PEER,            "E_SCS_API_FILE_EXIST_PEER",            "file has already existed in peer, please rename file and try again.",            "文件在对端已存在,请重命名后再尝试"},
        {E_SCS_API_VERSION_NUMBER_NOT_MATCH,   "E_SCS_API_VERSION_NUMBER_NOT_MATCH",   "version number not match in local and peer.",   "两端版本号不匹配"},
        {E_SCS_API_FILE_SIZE_NOT_MATCH,        "E_SCS_API_FILE_EXIST_PEER",            "file size not match in local and peer.",        "两端文件大小不匹配"},
        {E_SCS_API_FILE_SIZE_TOO_BIG,          "E_SCS_API_FILE_SIZE_TOO_BIG",          "file size is too large for local",   "文件体积超过本端介质剩余空间大小"},
        {E_SCS_API_PEER_FILE_SIZE_TOO_BIG,     "E_SCS_API_PEER_FILE_SIZE_TOO_BIG",     "file size is too large for peer",    "文件体积超过对端介质剩余空间大小"},
        {E_SCS_API_UPLOAD_TASK_IS_RUNNING,     "E_SCS_API_UPLOAD_TASK_IS_RUNNING",     "upload file task is already running, not finish", "有文件上传任务已经在运行，未结束"},
        {E_SCS_API_GET_DOWNLOAD_PROGRESS_FAILED,"E_SCS_API_GET_DOWNLOAD_PROGRESS_FAILED",  "get download progress failed.",   "查询文件下载进度失败"},
        {E_SCS_API_GET_VERSION_BUSY,           "E_SCS_API_GET_VERSION_BUSY",           "version is updating, operation is not allowed.",   "版本升级中，禁止操作"},
        {E_SCS_API_VERSION_NETBOOT_REWRITE,    "E_SCS_API_VERSION_NETBOOT_REWRITE",    "system is busy, please try later.", "系统忙，请稍后尝试"},
        {E_SCS_API_FILE_NAME_INVAILD,          "E_SCS_API_FILE_NAME_INVAILD",          "file name is invaild(the first character must be underline,letter or figure.From the second character can also be dot and transverse line,but the transverse line can not as the last character)", "文件名不合法(第一个字符必须是下划线、字母或数字，其他字符还可以是点和横线，但横线不能是最后一个字符)"},
        {E_SCS_API_FILE_INCONSISTENCY,         "E_SCS_API_FILE_UNCONSISTENCY",         "update file name is inconsistency in dual.", "两端待更新版本文件名称不一致"},
        {E_SCS_API_FILE_NAME_TOO_LONG,         "E_SCS_API_FILE_NAME_TOO_LONG",         "file name too long,please check", "文件名太长，请检查"},
        {E_SCS_API_SAMENAME_WITH_CURVER,       "E_SCS_API_SAMENAME_WITH_CURVER",       "file name is same with main version or standby version,please check", "文件名与当前版本名或备用版本名重复，请检查"},
        {E_SCS_API_BGTASK_RUNNING,             "E_SCS_API_BGTASK_RUNNING",             "system task is running, please wait and try later", "有系统级后台任务正在运行,请稍后重试"},
        {E_SCS_API_TOPO_ERROR,                 "E_SCS_API_TOPO_ERROR",                 "topo error update forbidden",   "拓扑结构不完整，不允许升级8005"},
        {E_SCS_API_WRONG_PM8005_TYPE,          "E_SCS_API_WRONG_PM8005_TYPE",          "wrong expander version type",   "EXP版本类型不正确，请使用正确的带内版本"},
        /* 配置文件导入导出相关错误码 add by tao 2011-08-16 */
        {E_SCS_API_FILE_EXIST,             "E_SCS_API_FILE_EXIST", "the file already exists, please rename and try again.",  "该文件已存在,请重命名后尝试"},
        {E_SCS_API_FILE_NO_EXIST,          "E_SCS_API_FILE_NO_EXIST", "the file does not exists.",  "该文件不存在"},
        {E_SCS_API_FILE_CP_FAILED,         "E_SCS_API_FILE_CP_FAILED", "copy file failed.", "拷贝文件失败"},
        {E_SCS_API_EXPORT_CFG_FILE_FAILED, "E_SCS_API_EXPORT_CFG_FILE_FAILED", "export config file failed.",  "配置文件导出失败"},
        {E_SCS_API_IMPORT_CFG_FILE_FAILED, "E_SCS_API_IMPORT_CFG_FILE_FAILED", "import config file failed.",  "配置文件导入失败"},
        {E_SCS_CFG_FILE_CHECK_ERR,         "E_SCS_CFG_FILE_CHECK_ERR", "the config file is illegal",  "配置文件不合法"},
        {E_SCS_CFG_FILE_OPEN_FAILED,       "E_SCS_CFG_FILE_OPEN_FAILED", "open the config file failed.",  "配置文件无法打开"},


        {E_SCS_ISCSI_NEWNAME_SAME_PERR, "E_SCS_ISCSI_NEWNAME_SAME_PERR", "New ISCSI name is same with peer iqn.", "新的ISCSI 名称与对端相同"},

        {E_SMA_PARAM_WRONG,           "E_SMA_PARAM_WRONG",          "Invalid parameter.",                "参数无效"},
        {E_SMA_MALLOC_ERROR,          "E_SMA_MALLOC_ERROR",         "Memory malloc failed.",             "内存分配失败"},
        {E_SMA_USER_WRONG,            "E_SMA_USER_WRONG",           "Username or password is incorrect.","用户名或者密码不正确"},
        {E_SMA_USER_LOGINED,          "E_SMA_USER_LOGINED",         "This user have logined in.",        "此用户已经登录"},
        {E_SMA_USER_EXIST,            "E_SMA_USER_EXIST",           "Username already exists.",          "用户名已存在"},
        {E_SMA_USER_NEXIST,           "E_SMA_USER_NEXIST",          "Username does not exist.",          "用户名不存在"},
        {E_SMA_ADMIN,                 "E_SMA_ADMIN",                "Super admin has logined in.",       "超级管理员已经登录"},
        {E_SMA_COMMON,                "E_SMA_COMMON",               "Other admin level user has logined in.","其他管理员用户已经登录"},
        {E_SMA_USER_AUTHOR,           "E_SMA_USER_AUTHOR",          "This user does not have enough authority.","此用户没有足够权限"},
        {E_SMA_AUTHEN,                "E_SMA_AUTHEN",               "This user has not been authenticated.","此用户没被授权"},
        {E_SMA_PASSWD,                "E_SMA_PASSWD",               "Old password does not match.",      "旧密码不匹配"},
        {E_SMA_COMM_USER_OVERFLOW,    "E_SMA_COMM_USER_OVERFLOW",   "Admin user number exceeds limits.", "管理员用户已经到达上限，不能再添加"},
        {E_SMA_GUEST_USER_OVERFLOW,   "E_SMA_GUEST_USER_OVERFLOW",  "Viewer user number exceeds limits.","查看用户已经到达上限，不能再添加"},
        {E_SMA_USER_OVERFLOW,         "E_SMA_USER_OVERFLOW",        "Total user number exceeds limits.", "用户数目已经到达上限，不能再添加"},
        {E_SMA_ADMIN_DEL,             "E_SMA_ADMIN_DEL",            "Cannot delete super admin.",        "超级管理员不能被删除"},
        {E_SMA_ONLINE_DEL,            "E_SMA_ONLINE_DEL",           "Cannot delete online account.",     "不能删除在线用户"},
        {E_SMA_CONN_OVERFLOW,         "E_SMA_CONN_OVERFLOW",        "Too many connects exist.",          "连接数已达上限"},
        {E_SMA_SESSION_ATHOR,         "E_SMA_SESSION_ATHOR",        "User does not have enough authority,can not login.","用户权限不足,不能登录"},
        {E_SMA_SESSION_NEXIST,        "E_SMA_SESSION_NEXIST",       "Session does not exist.",           "会话不存在"},
        {E_SMA_ADMIN_EXIST,           "E_SMA_ADMIN_EXIST",          "Can not add super admin type user.","不能添加超级用户"},

            {E_SMA_FAIL_SEND,             "E_SMA_FAIL_SEND",            "Failed to send email.",             "发送邮件失败"},
            {E_SMA_NSMTP,                     "E_SMA_NSMTP",                "SMTP does not set ok.",             "邮箱服务没有设置"},
            {E_SMA_USER_NPWDMAIL,           "E_SMA_USER_NPWDMAIL",  "Password mail does not set.",       "密码提醒邮箱没有设置"},

        {E_SMA_SYS_NOT_FINISH_STARTUP,"E_SMA_SYS_NOT_FINISH_STARTUP",  "System have not finished startup.",  "系统启动未完成"},
        {E_SCS_SMA_SYSTEM_BUSY,       "E_SCS_SMA_SYSTEM_BUSY",      "System is busy, please try again later.",  "系统忙，请稍后再试"},
        {E_SCS_SMA_RCC_INSTABILITY,       "E_SCS_SMA_RCC_INSTABILITY",      "Channel is instability.",  "通道目前不稳定或出现故障"},
        {E_SMA_USER_NEWNAME_HAVE_EXIT,"E_SMA_USER_NEWNAME_HAVE_EXIT",  "New name is illegal,it has exist in system.",  "新的用户名非法，系统中已存在此用户名"},

        /******************************* TIME **********************************/
        {E_SCS_TIME_NTP_SERVER_ADDR_NULL,         "E_SCS_TIME_NTP_SERVER_ADDR_NULL",         "NTP server address is NULL.",       "NTP服务器地址为空"},
        {E_SCS_TIME_NTP_SERVER_ADDR_INVALID,      "E_SCS_TIME_NTP_SERVER_ADDR_INVALID",      "NTP server address is invalid.",     "NTP服务器地址非法"},
        {E_SCS_TIME_GET_SYSTEM_TIME_ZONE_FAILED , "E_SCS_TIME_GET_SYSTEM_TIME_ZONE_FAILED",   "Get system time zone failed.",       "获取系统时区失败"},
        {E_SCS_TIME_SET_HARDWARE_CLOCK_FAILED,    "E_SCS_TIME_SET_HARDWARE_CLOCK_FAILED",    "Set hardware clock faiked.",         "设置硬件时钟失败"},
        {E_SCS_TIME_THE_CITY_IS_INVALID,          "E_SCS_TIME_THE_CITY_IS_INVALID",          "The city is invalid.",               "该城市无效"},
        {E_SCS_TIME_NO_VALID_CONTINENT_NAME,      "E_SCS_TIME_NO_VALID_CONTINENT_NAME",      "Did not input continent name.",      "未输入洲名称"},
        {E_SCS_TIME_SET_TIME_ZONE_FAILED,         "E_SCS_TIME_SET_TIME_ZONE_FAILED",         "Set time zone by index failed.",     "根据城市索引设置时区失败"},
        {E_SCS_TIME_GET_NTP_SERVER_CONFIG_FAILED, "E_SCS_TIME_GET_NTP_SERVER_CONFIG_FAILED",  "Get NTP server config failed.",     "获取NTP服务器配置失败"},
        {E_SCS_TIME_SET_NTP_CONFIG_FAILED,        "E_SCS_TIME_SET_NTP_CONFIG_FAILED",        "Set NTP config failed.",            "设置NTP配置失败"},
        {E_SCS_TIME_SET_AUTO_NTP_FAILED,          "E_SCS_TIME_SET_AUTO_NTP_FAILED",          "Set auto NTP synchronize failed.",   "设置自动时间同步失败"},
        {E_SCS_TIME_ENTER_TIME_INVALID,           "E_SCS_TIME_ENTER_TIME_INVALID",           "Enter time is invalid.",             "输入的时间无效"},
        {E_SCS_TIME_SYNC_TIME_BY_NTP_FAILED,      "E_SCS_TIME_SYNC_TIME_BY_NTP_FAILED",      "Synchronize system time by ntp failed.", "从ntp服务器同步时间失败"},
        {E_SCS_TIME_WRITE_CFG_REC_FAILED,         "E_SCS_TIME_WRITE_CFG_REC_FAILED",         "Write config record failed.",        "写配置文件记录失败"},
        {E_SCS_TIME_READ_CFG_REC_FAILED,          "E_SCS_TIME_READ_CFG_REC_FAILED",          "Read config record failed.",         "读配置文件记录失败"},
        {E_SCS_TIME_FORK_FAILED,                  "E_SCS_TIME_FORK_FAILED",                 "Operate failed.",                   "操作失败"},


        /******************************************************************/

        /* *******************************  LOG & MAILSET  ************************************* */
        {E_LOG_PARA_NULL,              "E_LOG_PARA_NULL",             "Point parameters null.",                       "指针参数为空。"},
        {E_LOG_PARAM_ILLEAG,           "E_LOG_PARAM_ILLEAG",          "Parameters passed in or passed out illeagal.", "传入或传出的参数非法。"},
        {E_LOG_GET_FAILED,             "E_LOG_GET_FAILED",            "Log inquiring failed.",                        "日志查询操作失败。"},
        {E_LOG_CLEAR_FAILED,           "E_LOG_CLEAR_FAILED",          "Log clear failed.",                            "日志清除操作失败。"},
        {E_LOG_EXPORT_FAILED,          "E_LOG_EXPORT_FAILED",         "Log exporting failed.",                        "日志导出操作失败。"},
        {E_LOG_EXPORT_ZERO,            "E_LOG_EXPORT_ZERO",           "Log number is 0, you could not export.",       "日志数量为零，无法导出。"},
        {E_LOG_GETNUM_FAILED,          "E_LOG_GETNUM_FAILED",         "Log account obtain failed.",                   "获取日志数量错误。"},
        {E_LOG_SET_WMAIL_FAILED,       "E_LOG_SET_WMAIL_FAILED",      "Log alarm mail configuration setting failed.", "告警邮件配置项设置失败。"},
        {E_LOG_GET_WMAIL_FAILED,       "E_LOG_GET_WMAIL_FAILED",      "Inquiring log alarm mail configuration failed.","查询告警邮件配置项失败。"},
        {E_LOG_GET_USERLOG_AUTH,       "E_LOG_GET_USERLOG_AUTH",      "User-log inquiring permission denied.","查询用户管理日志权限不足。"},
        {E_LOG_GET_USERLOG_USERINV,    "E_LOG_GET_USERLOG_USERINV",   "Username invalid in user-log inquiring.","用户管理日志查询对象用户不可用。"},
        {E_LOG_PEER_ABSENT,            "E_LOG_PEER_ABSENT",           "Peer controler is absent or booting.",          "对端控制器不在位或处于启动状态。"},
        {E_LOG_GET_ALARM_FAILED,       "E_LOG_GET_ALARM_FAILED",      "Alarm inquiring failed.",                       "告警查询操作失败。"},
        {E_LOG_DEL_ALARM_FAILED,       "E_LOG_DEL_ALARM_FAILED",      "Alarm deleting failed.",                        "告警删除操作失败。"},

        {E_MAIL_PARA_NULL,             "E_MAIL_PARA_NULL",            "Point parameters null.",                       "指针参数为空。"},
        {E_MAIL_PARAM_ILLEAG,          "E_MAIL_PARAM_ILLEAG",         "Parameters illeagal.",                         "参数非法。"},
        {E_MAIL_LINK_HOST_FAILED,      "E_MAIL_LINK_HOST_FAILED",     "Link host failed.",                            "连接邮件服务器主机失败。"},
        {E_MAIL_LOGIN_FAILED,          "E_MAIL_LOGIN_FAILED",         "Login mail host failed.",                      "登陆邮件服务器主机错误。"},
        {E_MAIL_CONFFILE_CREAT_FAILED, "E_MAIL_CONFFILE_CREAT_FAILED","Mail config file create failed.",              "邮件配置文件创建失败。"},
        {E_MAIL_SET_ENABLE_FAILED,     "E_MAIL_SET_ENABLE_FAILED",    "Enable mail sending failed.",                  "邮件发送失败。"},
        {E_MAIL_UNABLED_MAIL_ERR,      "E_MAIL_UNABLED_MAIL_ERR",     "Mail sending error: mail switch is not enabled.","邮件发送错误:邮件开关非使能。"},
        {E_MAIL_CONFIG_VERIMAIL_FAILED,"E_MAIL_CONFIG_VERIMAIL_FAILED","Verifying test mail sending error, check configuration.","验证测试邮件发送失败，请检查配置。"},
        {E_MAIL_CONF_GET_FAILED,       "E_MAIL_CONF_GET_FAILED",      "Mail config inquiring failed.",                "邮件查询失败，请检查配置。"},
        {E_MAIL_CONF_SET_FAILED,       "E_MAIL_CONF_SET_FAILED",      "Set mail config failed.",                      "设置邮箱配置失败。"},
        {E_MAIL_GET_ENABLE_FAILED,     "E_MAIL_GET_ENABLE_FAILED",    "Inquiring mail-sending switch failed.",        "查询邮箱配置信息失败。"},
        {E_MAIL_SENDING_FAILED,        "E_MAIL_SENDING_FAILED",       "Mail sending failed.",                         "邮件发送失败。"},

        /* *************************************** SIC ***********************************************/
        {E_SIC_PARAM_NULL,             "E_SIC_PARAM_NULL",            "Parameters is null.",                          "参数为空。"},
        {E_SIC_PARAM_ILLEAG,           "E_SIC_PARAM_ILLEAG",          "Parameters is illeagal.",                      "参数非法。"},
        {E_SIC_COLLECT_SYS_INFO_FAILED, "E_SIC_COLLECT_SYS_INFO_FAILED", "Collect system information failed.",        "采集系统信息失败。"},
        {E_SIC_GET_TASK_PROC_FAILED,   "E_SIC_GET_TASK_PROC_FAILED",  "Get task process failed.",                     "获取系统信息采集任务进度失败。"},
        {E_SIC_SET_EMAIL_ENABLE_FAILED, "E_SIC_SET_EMAIL_ENABLE_FAILED", "Set email config failed.",                  "设置邮件配置失败。"},
        {E_SIC_GET_TARBALL_INFO_FAILED, "E_SIC_GET_TARBALL_INFO_FAILED", "Get tar ball information failed.",          "获取系统信息压缩包信息失败。"},
        {E_SIC_GET_EMAIL_ENABLE_FAILED, "E_SIC_GET_EMAIL_ENABLE_FAILED", "Get email config failed.",                  "获取邮件配置失败。"},
        {E_SIC_TAR_BALL_NOT_EXIST,     "E_SIC_TAR_BALL_NOT_EXIST",    "The tar ball does not exist.",                 "系统信息压缩包不存在。"},
        {E_SIC_TASK_IS_EXIST,          "E_SIC_TASK_IS_EXIST",         "The task to collect system information already exists.", "系统信息采集任务已经存在。"},
        {E_SIC_EXPORT_SYSINFO_TARBALL_FAILED, "E_SIC_EXPORT_SYSINFO_TARBALL_FAILED", "Export system information tar ball failed.", "导出系统信息压缩包失败。"},


        /* *************************************** DIAG ***********************************************/
        {E_DIAG_GET_INFO_FAILED,        "E_DIAG_GET_INFO_FAILED",       "Diag get info failed .",                     "获取信息失败。"    },
        {E_DIAG_PARA_NULL,              "E_DIAG_GET_INFO_FAILED",       "Point parameters null.",                     "参数不正确。"      },
        {E_DIAG_SODA_FAILED,            "E_DIAG_SODA_FAILED",           "Board contrl interface failed.",             "底层接口调用失败。"},
        {E_DIAG_ECC_SWITCH_SET_FAILED,  "E_DIAG_ECC_SWITCH_SET_FAILED", "Set ecc check switch failed.",               "设置ECC检测开关失败"},
        {E_DIAG_GET_INFO_NOTSUPPORT,    "E_DIAG_GET_INFO_NOTSUPPORT",   "No such device.",                            "不存在该设备。"    },
        /* ***************************************TRAP ***********************************************/
        {E_SMA_TRAP_OVERFLOW,           "E_DIAG_GET_INFO_FAILED",      "Trap config number exceeds limits.", "Trap配置已经到达上限，不能再添加"},
        {E_SCS_TRAP_ID_NOT_EXIST,       "E_SCS_TRAP_ID_NOT_EXIST",      "Trap config id is not exist.", "Trap 配置Id 不存在"},

         /*******************************STS 子系统************************************/
        {E_STS_LUNDEV_NO_EXIST_ERR,     "E_STS_LUNDEV_NO_EXIST_ERR",   "The device does not exist.",    "设备不存在."},
        {E_STS_SEQNO_NUM_ERR,             "E_STS_SEQNO_NUM_ERR",          "Illegal device number .",      " 设备号输入参数非法."},
        {E_STS_INITIATOR_NO_EXIST_ERR,       "E_STS_INITIATOR_NO_EXIST_ERR",   "Host name does not exist.",     " 在系统中找不到该主机名."},
        {E_STS_CACHE_POLICY_ERR,         "E_STS_CACHE_POLICY_ERR",    "Illegal cache policy.", " 高速缓存策略输入参数非法."},
        {E_STS_AHEAD_READ_SIZE_ERR,    "E_STS_AHEAD_READ_SIZE_ERR",   "Illegal ahead_read_size.",      " 预读系数输入参数非法."},
        {E_STS_LOCALLUN_NUM_ERR,             "E_STS_LOCALLUN_NUM_ERR",           "Illegal locallun .",     " 本地逻辑单元号输入参数非法."},
        {E_STS_LUN_NO_EXIST_ERR,             "E_STS_LUN_NO_EXIST_ERR",       "Locallun does not exist.",      "在映射组中找不到该逻辑单元"},
        {E_STS_REFRESH_CACHE_ERR,             "E_STS_REFRESH_CACHE_ERR",       "Refresh cache failed.",     "刷新cache失败."},
        {E_STS_LUNDEV_IS_MAPPING_ERR,       "E_STS_LUNDEV_IS_MAPPING_ERR",       "Device is mapping, can not refresh cache.",     "设备上面还存在映射，不能刷新cache."},
        {E_STS_OWNER_PARAM_ERR,         "E_STS_OWNER_PARAM_ERR",   "Illegal owner.",        "所属控制器输入非法."},
        {E_STS_PREFER_PARAM_ERR,         "E_STS_PREFER_PARAM_ERR",   "Illegal prefer .",       "优先控制器输入非法."},
        {E_STS_DISCARD_CACHE_ERR,             "E_STS_DISCARD_CACHE_ERR",               "Discard cache failed.",      " 丢弃cache失败."},
        {E_STS_REMOVE_MAP_TIMEOUT_ERR,             "E_STS_REMOVE_MAP_TIMEOUT_ERR",               "Remove MAP timeout.",      " 删除映射超时."},
        {E_STS_WPFLAG_PARAM_ERR,             "E_STS_WPFLAG_PARAM_ERR",    "Illegal Write Protect flag.",      " 写保护标志输入非法."},


        {E_STS_STSCREATELUNDEV_ALLOC_LUNDEV_FAILED_ERR,   "E_STS_STSCREATELUNDEV_ALLOC_LUNDEV_FAILED_ERR",    "Alloc  lun device  failed.",     "对逻辑单元设备分配空间失败."  },
        {E_STS_STSCREATELUNDEV_OPEN_LUNDEV_FAILED_ERR,  "E_STS_STSCREATELUNDEV_OPEN_LUNDEV_FAILED_ERR" ,  "Open lun device failed.",     "打开逻辑单元设备失败."},
        {E_STS_STSCREATELUNDEV_SEQNO_EXIST_ERR,     "E_STS_STSCREATELUNDEV_SEQNO_EXIST_ERR",     "Device number has already existed.",     "设备号已存在."   },
        {E_STS_STSCREATELUNDEV_LUNDEV_EXIST_ERR,    "E_STS_STSCREATELUNDEV_LUNDEV_EXIST_ERR",    "The device  has already opened." ,    "设备已经打开."},
        {E_STS_STSREMOVELUNDEV_BUSY_ERR,    "E_STS_STSREMOVELUNDEV_BUSY_ERR",    "The device  is busy now." ,    "设备忙，关闭失败."},
        {E_STS_STSLISTLUNDEV_LUNDEV_REFCNT_ERR,      "E_STS_STSLISTLUNDEV_LUNDEV_REFCNT_ERR"    ,    "Error lun device refcnt.",     "逻辑单元设备的引用计数错误."},
        {E_STS_STSCREATELUNMAP_ALLOC_INITIATOR_FAILED_ERR,   "E_STS_STSCREATELUNMAP_ALLOC_INITIATOR_FAILED_ERR", "Alloc initiator struct failed.",        "申请启动器结构体内存失败"},
        {E_STS_STSCREATELUNMAP_ALLOC_LUN_FAILED_ERR,         "E_STS_STSCREATELUNMAP_ALLOC_LUN_FAILED_ERR",       "Alloc LUN struct failed.",         "申请逻辑单元结构体内存失败"},
        {E_STS_STSCREATELUNMAP_LUN_EXIST_ERR,                "E_STS_STSCREATELUNMAP_LUN_EXIST_ERR",              "Locallun has already existed.",    "本地逻辑单元号已经被占用"},
        {E_STS_STSCREATELUNMAP_OPEN_LUNDEV_ERR,              "E_STS_STSCREATELUNMAP_OPEN_LUNDEV_ERR",            "Open this device failed.",      "打开设备失败"},
        {E_STS_STSSETSTATSTATUS_PARAM_ERR,              "E_STS_STSSETSTATSTATUS_PARAM_ERR",              "Illegal statistics status param,  it should be 0(OFF) OR  1(ON)  OR 2(RESET).",     " 统计开关输入参数非法，只能是0(关闭)、1(打开)、2(重置)."},
        {E_STS_STSPARAMINIT_CONTROLLER_ID_ERR,          "E_STS_STSPARAMINIT_CONTROLLER_ID_ERR",         "Illegal controller Id param,  it should be 0(CONTROLLER_A) OR 1(CONTROLLER_B).",   " 控制器标识号输入参数非法，只能是0(控制器A)或者1(控制器B)."},
        {E_STS_STSSETCONTGTINFO_WAKEUP_THREAD_FLAG_NULL_ERR,    "E_STS_STSSETCONTGTINFO_WAKEUP_THREAD_FLAG_NULL_ERR",   "RGPT cmd overtimed, the opposite  port info has not sent to local port in time.",   "查询目标端口组信息命令执行超时，对端的端口信息没有及时传到本端."},
        {E_STS_STSMODIFYLUNDEVERRORFLAG_PARAM_ERR,          "E_STS_STSMODIFYLUNDEVERRORFLAG_PARAM_ERR",           "Illegal error flag param .",     "设备错误标志位输入参数非法."},
        {E_STS_STSMODIFYLUNDEVPORTSTATUS_PARAM_ERR,          "E_STS_STSMODIFYLUNDEVPORTSTATUS_PARAM_ERR",          "Illegal port status param .",     "设备端口状态标志位输入参数非法."},
        {E_STS_STSLISTLUNSEQNOONSESSION_PARAM_ERR,           "E_STS_STSLISTLUNSEQNOONSESSION_PARAM_ERR",           "Illegal IT&session param , session struct is NULL.",      "IT及会话输入参数非法，会话结构体为空"},
        {E_STS_STSSEARCHITONCONTORL_PARAM_ERR,          "E_STS_STSSEARCHITONCONTORL_PARAM_ERR",       "Illegal IT&session param , session struct is NULL.",      "IT及会话输入参数非法，会话结构体为空"},
        {E_STS_STSSEARCHITONCONTORL_IT_ERR,                "E_STS_STSSEARCHITONCONTORL_IT_ERR",                "Error IT Nexus.",     "IT  连接关系出错."},
        {E_STS_STSMODIFYLUNDEVRSTFLAG_PARAM_ERR,          "E_STS_STSMODIFYLUNDEVRSTFLAG_PARAM_ERR",            "Illegal LUN reset status param, .",    "逻辑单元重启标志位输入参数非法."},
        {E_STS_STSSETALLSTATSTATUS_PARAM_ERR,             "E_STS_STSSETALLSTATSTATUS_PARAM_ERR",               "Illegal statistics status param .",      " 统计开关输入参数非法."},
        {E_STS_LUNDEV_WAIT_CMDS_TIMEOUT_ERR,             "E_STS_LUNDEV_WAIT_CMDS_TIMEOUT_ERR",               "Wait cmds timeout .",      " 等待命令超时."},
        {E_STS_ASYMMETRIC_ISTRANSITING_ERR, "E_STS_ASYMMETRIC_ISTRANSITING_ERR", "The Lun status is transiting.", " LUN状态切换中."},


     /*******************************CBS 子系统************************************/
        {E_CBS_OPEN_FILE_ERROR,         "E_CBS_OPEN_FILE_ERROR",        "Open dev error.",                      "打开设备文件失败"},
        {E_CBS_BAD_ADDR,                 "E_CBS_BAD_ADDR",              "Error address.",                       "输入地址无效"},
        {E_CBS_ERR_CACHE_SIZE,           "E_CBS_ERR_CACHE_SIZE",        "Error cache size.",                    "高速缓存单元大小无效"},
        {E_CBS_ERR_DIRTY_RATIO,          "E_CBS_ERR_DIRTY_RATIO",       "Error write ratio.",                   "写比例无效"},
        {E_CBS_ERR_PCIE_NO_LINK,         "E_CBS_ERR_PCIE_NO_LINK",      "PCIe channel linking is not up",       "PCIe数据链路层未就绪前不允许打开镜像功能"},
        {E_CBS_ERR_OPEN_MIRROR_PARM,     "E_CBS_ERR_OPEN_MIRROR_PARM",  "Error mirror parameter.",              "镜像参数无效"},
        {E_CBS_ERR_MONITOR_PARM,         "E_CBS_ERR_MONITOR_PARM",      "Error monitor parameter.",             "监控参数无效"},
        {E_CBS_MONITOR_STATUS_OFF,       "E_CBS_MONITOR_STATUS_OFF",    "Monitor status is off.",               "统计开关是关闭的"},

        /*******************************DM 子系统************************************/
        {E_DM_INPUT_PARAM_INVALID,   "E_DM_INPUT_PARAM_INVALID",      "Can not finish request.",        "无法完成请求"},
        {E_DM_OPEN_DM_DIR_FAIL,      "E_DM_OPEN_DM_DIR_FAIL",         "open dm fail.",                  "无法完成请求"},
        {E_DM_ALLOC_RESOURCE_FAIL,   "E_DM_ALLOC_RESOURCE_FAIL",      "No system resource.",            "无法完成请求"},
        {E_DM_DISK_ABSENT,           "E_DM_DISK_ABSENT",              "No system resource.",            "无法完成请求"},
        {E_DM_SOCKET_FAIL,           "E_DM_SOCKET_FAIL",              "System internal error.",         "系统内部错误"},
        {E_DM_GET_WARN_SWITCH_FAIL,  "E_DM_GET_WARN_SWITCH_FAIL",     "Get disk warn switch fail.",     "获取磁盘告警开关失败"},
        {E_DM_GET_BBLK_LIMIT_FAIL,   "E_DM_GET_BBLK_LIMIT_FAIL",      "Get disk bad block limit fail.", "获取磁盘告警阀门值失败"},
        {E_DM_GET_RUNNING_STATE_FAIL,"E_DM_GET_RUNNING_STATE_FAIL",   "Get dm running state fail.",     "获取系统运行状态失败"},
        {E_DM_GET_CTRL_UUID_FAIL,    "E_DM_GET_CTRL_UUID_FAIL",       "Get system ctrl UUID fail.",     "获取系统运行状态失败"},
        {E_DM_GET_DEBUG_LEVEL_FAIL,  "E_DM_GET_DEBUG_LEVEL_FAIL",     "Get system debug state fail.",   "获取系统调试级别失败"},
        {E_DM_SET_WARN_SWITCH_FAIL,  "E_DM_SET_WARN_SWITCH_FAIL",     "Set disk warn switch fail.",     "设置磁盘告警开关失败"},
        {E_DM_SET_BBLK_LIMIT_FAIL,   "E_DM_SET_WARN_SWITCH_FAIL",     "Set disk bad block limit fail.", "设置磁盘告警阀门值失败"},
        {E_DM_SET_RUNNING_STATE_FAIL,"E_DM_SET_RUNNING_STATE_FAIL",   "Set dm running state fail.",     "设置系统运行状态失败"},
        {E_DM_SET_CTRL_UUID_FAIL,    "E_DM_SET_CTRL_UUID_FAIL",       "Set system ctrl UUID fail.",     "设置系统运行状态失败"},
        {E_DM_SET_DEBUG_LEVEL_FAIL,  "E_DM_SET_DEBUG_LEVEL_FAIL",     "Set system debug state fail.",   "设置系统调试级别失败"},
        {E_DM_GET_TYPE_FAIL,         "E_DM_GET_TYPE_FAIL",            "Get disk type fail.",            "获取磁盘类型失败"},
        {E_DM_GET_USEABLE_FAIL,      "E_DM_GET_USEABLE_FAIL",         "Get disk useable fail.",         "获取磁盘可用性失败"},
        {E_DM_GET_ACCESSABLE_FAIL,   "E_DM_GET_ACCESSABLE_FAIL",      "Get disk accessable fail.",      "获取磁盘可访问性失败"},
        {E_DM_GET_PD_SCAN_RATE_FAIL, "E_DM_GET_PD_SCAN_RATE_FAIL",    "Get disk scan rate fail.",       "获取磁盘扫描情况失败"},
        {E_DM_GET_SMART_FAIL,        "E_DM_GET_SMART_FAIL",           "Get disk SMART fail.",           "获取磁盘SMART信息失败"},
        {E_DM_GET_GUID_FAIL,         "E_DM_GET_GUID_FAIL",            "Get disk UUID fail.",            "获取磁盘UUID失败"},
        {E_DM_GET_DEVNAME_FAIL,      "E_DM_GET_DEVNAME_FAIL",         "Get disk name fail.",            "获取磁盘名称失败"},
        {E_DM_GET_CAPACITY_FAIL,     "E_DM_GET_CAPACITY_FAIL",        "Get disk capacity fail.",        "获取磁盘容量失败"},
        {E_DM_GET_PHYID_FAIL,        "E_DM_GET_PHYID_FAIL",           "Get disk phy id fail.",          "获取磁盘Phy标识号失败"},
        {E_DM_GET_JBODCADDR_FAIL,    "E_DM_GET_JBODCADDR_FAIL",       "Get disk jbodc address fail.",   "获取磁盘Jbodc地址失败"},
        {E_DM_GET_BUSID_FAIL,        "E_DM_GET_BUSID_FAIL",           "Get disk bus id fail.",          "获取磁盘bus地址失败"},
        {E_DM_GET_SASADDR_FAIL,      "E_DM_GET_SASADDR_FAIL",         "Get disk SAS address fail.",     "获取磁盘SAS地址失败"},
        {E_DM_GET_CACHE_FAIL,        "E_DM_GET_CACHE_FAIL",           "Get disk cache state fail.",     "获取磁盘缓存状态失败"},
        {E_DM_GET_MODEL_FAIL,        "E_DM_GET_MODEL_FAIL",           "Get disk model fail.",           "获取磁盘型号失败"},
        {E_DM_GET_PHY_TYPE_FAIL,     "E_DM_GET_PHY_TYPE_FAIL",        "Get disk phycial type fail.",    "获取磁盘物理接口类型失败"},
        {E_DM_GET_FW_REV_FAIL,       "E_DM_GET_FW_REV_FAIL",          "Get disk Firmware version fail.", "获取磁盘固件版本号失败"},
        {E_DM_GET_SERIAL_NO_FAIL,    "E_DM_GET_SERIAL_NO_FAIL",       "Get disk serial Number fail.",   "获取磁盘序列号失败"},
        {E_DM_GET_STD_VERSION_FAIL,  "E_DM_GET_STD_VERSION_FAIL",     "Get disk standard version fail.", "获取磁盘标准版本号失败"},
        {E_DM_SET_PD_TYPE_FAIL,      "E_DM_SET_PD_TYPE_FAIL",         "Set disk type fail.",             "设置磁盘类型失败"},
        {E_DM_SET_PD_USEABLE_FAIL,   "E_DM_SET_PD_USEABLE_FAIL",      "Import disk fail.",               "导入磁盘失败"},
        {E_DM_SET_PD_ACCESS_FAIL,    "E_DM_SET_PD_ACCESS_FAIL",       "Set disk accessable fail.",       "设置磁盘可访问性失败"},
        {E_DM_SET_CONTROL_PD_SCAN_FAIL, "E_DM_SET_CONTROL_PD_SCAN_FAIL",       "Control disk scan fail.",       "控制磁盘扫描失败"},
        {E_DM_SET_SMART_SELFTEST_FAIL,  "E_DM_SET_SMART_SELFTEST_FAIL",        "Exec SMART selftest fail.",      "执行磁盘自检失败"},
        {E_DM_SET_CACHE_FAIL,           "E_DM_SET_CACHE_FAIL",                 "Set disk cache state fail.",     "设置磁盘缓存状态失败"},
        {E_DM_GET_IO_STATISTICS_FAIL,   "E_DM_GET_IO_STATISTICS_FAIL",       "Get disk IO statistics info fail.", "获取磁盘IO统计信息失败"},
        {E_DM_SET_IO_STATISTICS_FAIL,   "E_DM_SET_IO_STATISTICS_FAIL",       "Set disk IO statistics fail.",      "设置磁盘IO统计开关失败"},
        {E_DM_RESET_IO_STATISTICS_FAIL, "E_DM_RESET_IO_STATISTICS_FAIL",     "Reset disk IO statistics fail.",    "重置磁盘IO统计开关失败"},



        {E_DM_NO_RESOURCE ,               "E_DM_NO_RESOURCE ",                  "No system resource can be used.",          "找不到可用资源"},
        {E_DM_INVALID_INPUT,              "E_DM_INVALID_INPUT",                 "The input parameter is invalid.",          "输入参数无效"},
        {E_DM_CMD_VALUE_ERROR,            "E_DM_CMD_VALUE_ERROR",               "The command value is wrong.",              "错误的命令参数"},
        {E_DM_MDA_RW_ERROR,               "E_DM_MDA_RW_ERROR",                  "Error in read or write MDA data.",         "读取/写入元数据出错"},
        {E_DM_MDA_CHECKSUM_ERROR,         "E_DM_MDA_CHECKSUM_ERROR",            "Error in get MDA checksun.",               "获取磁盘告警阀门值失败"},
        {E_DM_MDA_SELRCT_ERROR,           "E_DM_MDA_SELRCT_ERROR",              "Get dm running state fail.",               "获取元数据校验值错误"},
        {E_DM_PD_IN_LIST,                 "E_DM_PD_IN_LIST",                    "The PD is found in list.",                 "在系统磁盘链表中发现磁盘节点"},
        {E_DM_PD_NOT_IN_LIST,             "E_DM_PD_NOT_IN_LIST",                "The PD can not be found in list.",         "无法在磁盘链表中找到对应磁盘节点"},
        {E_DM_BEYOND_MAX_PD_NUM ,         "E_DM_BEYOND_MAX_PD_NUM ",            "The number of PD beyond the max value.",   "磁盘数目超过磁盘链表允许的最大值"},
        {E_DM_START_ERROR,                "E_DM_START_ERROR",                   "The dm system start error.",               "Dm 子系统启动失败"},
        {E_DM_STOP_ERROR,                 "E_DM_STOP_ERROR",                    "Set dm stop state fail.",                  "设置系统运行停止状态失败"},
        {E_DM_MODIFY_PD_SYSINFO_ERROR,    "E_DM_MODIFY_PD_SYSINFO_ERROR",       "Modify PD sysinfo fail.",                  "修改磁盘软件信息失败"},
        {E_DM_GET_STD_VER_ERROR,          "E_DM_GET_STD_VER_ERROR",             "Get system standard version num  fail.",   "获得系统标准版本号失败"},
        {E_DM_GET_CAPACITY_VALUE_ERROR,   "E_DM_GET_CAPACITY_VALUE_ERROR",      "Get disk capacity value fail.",            "获取磁盘容量失败"},
        {E_DM_GET_FW_VALUE_ERROR ,        "E_DM_GET_FW_VALUE_ERROR ",           "Get disk FW value fail.",                  "获取磁盘FW值失败"},
        {E_DM_GET_CATCH_VALUE_ERROR,      "E_DM_GET_CATCH_VALUE_ERROR",         "Get disk catch state fail.",               "获取磁盘缓存状态失败"},
        {E_DM_GET_MODEL_VALUE_ERROR,      "E_DM_GET_MODEL_VALUE_ERROR",         "Get disk model name fail.",                "获取磁盘名称失败"},
        {E_DM_GET_SERIAL_VALUE_ERROR,     "E_DM_GET_SERIAL_VALUE_ERROR",        "Get disk serial num fail.",                "获取磁盘序列号失败"},
        {E_DM_GET_SAS_VALUE_ERROR,        "E_DM_GET_SAS_VALUE_ERROR",           "Get disk SAS address fail.",               "获取磁盘SAS 地址失败"},
        {E_DM_SOCKET_INIT_ERROR,          "E_DM_SOCKET_INIT_ERROR",             "Socket init fail.",                        "Socket 初始化失败"},
        {E_DM_SOCKET_SET_NUM_ERROR  ,     "E_DM_SOCKET_SET_NUM_ERROR  ",        "Set socket serial number fail.",           "设置客户端Socket磁盘序列号失败"},
        {E_DM_SOCKET_BIND_ERROR,          "E_DM_SOCKET_BIND_ERROR",             "Bind socket address fail.",                "绑定socket地址失败"},
        {E_DM_SOCKET_SET_ERROR,           "E_DM_SOCKET_SET_ERROR",              "Set socket attribute fail.",               "设置socket 属性失败"},
        {E_DM_SOCKET_SEND_ERROR,          "E_DM_SOCKET_SEND_ERROR",             "Send value through socket fail.",          "发送socket数据失败"},
        {E_DM_SOCKET_RECEV_ERROR ,        "E_DM_SOCKET_RECEV_ERROR ",           "Receive socket value fail.",               "获取磁盘SAS地址失败"},
        {E_DM_PD_GET_SMART_ERROR,         "E_DM_PD_GET_SMART_ERROR",            "Get disk SMART fail.",                     "获取磁盘SMART信息失败"},
        {E_DM_SET_SMART_SELFTEST_ERROR,   "E_DM_SET_SMART_SELFTEST_ERROR",      "Set disk SMART fail.",                     "设置磁盘SMART信息失败"},

        {E_DM_PD_GET_SMART_DURING_SCAN_ERROR,     "E_DM_PD_GET_SMART_DURING_SCAN_ERROR",        "Refusing Get Smart Info during disk scanning.",                "磁盘正在扫描，不允许获取SMART信息"},
        {E_DM_SET_SMART_SELFTEST_DURING_SCAN_ERROR,        "E_DM_SET_SMART_SELFTEST_DURING_SCAN_ERROR",           "Refusing Executing Smart SelfTest command  during scanning disk.",               "磁盘正在扫描，不允许执行SMART自检"},
        {E_DM_SCAN_DATA_PD_FAIL   ,     "E_DM_SCAN_DATA_PD_FAIL ",        "Refusing scanning disk of data type.",           "不允许对数据盘执行磁盘扫描"},
        {E_DM_SCAN_UNKNOWN_PD_FAIL   ,     "E_DM_SCAN_UNKNOWN_PD_FAIL",        "Refusing scanning disk of unknown  type.",           "不允许对未知盘执行磁盘扫描"},
        {E_DM_SMART_DATA_PD_FAIL,          "E_DM_SMART_DATA_PD_FAIL",             "Refusing smart operation with disk of data type.",                "不允许对数据盘执行SMART自检测"},
        {E_DM_SET_DISK_DATA_TYPE_DURING_SCAN_ERROR,           "E_DM_SET_DISK_DATA_TYPE_DURING_SCAN_ERROR",              "Refusing set disk type to data type during disk scanning",               "磁盘正在扫描,不允许修改磁盘为数据盘"},
        {E_DM_SET_DISK_UNKNOWN_TYPE_DURING_SCAN_ERROR,           "E_DM_SET_DISK_UNKNOWN_TYPE_DURING_SCAN_ERROR",              "Refusing set disk type to unknown type during disk scanning",               "磁盘正在扫描,不允许修改磁盘为未知盘"},

        /*******************************BLK 子系统************************************/
        {E_VDS_NULLP,           "E_VDS_NULLP",      "There is no effective resources.", "没有有效的资源。"},    /*空指针 */
        {E_VDS_PARAM_INVALID,    "E_VDS_PARAM_INVALID",     "Invalid parameters.",  "无效参数。"},          /*参数非法 */
        {E_VDS_NO_VD_MOD,     "E_VDS_NO_VD_MOD",     "The module of virtual disk has not been loaded.", "虚拟盘模块未加载。"},          /*VD模块未加载 */
        {E_VDS_NO_VOL_MOD,     "E_VDS_NO_VOL_MOD",     "The module of volume has not been loaded.",  "卷模块未加载。"},          /*VOL 模块未加载*/
        {E_VDS_NO_SNAP_MOD,     "E_VDS_NO_SNAP_MOD",     "The module of snapshot has not been loaded",  "快照模块未加载。"},          /*SNAP 模块未加载*/
        {E_VDS_NO_VD,      "E_VDS_NO_VD",      "The specified virtual disk does not exist.",    "指定的虚拟盘不存在。"},          /*指定的vd 不存在 */
        {E_VDS_NO_VOL,      "E_VDS_NO_VOL",      "The specified volume does not exist.",      "指定的卷不存在。"},          /*指定的vol 不存在 */
        {E_VDS_NO_SNAP,     "E_VDS_NO_SNAP",      "The specified snapshot does not exist.",      "指定的快照不存在。"},          /*指定的snap 不存在 */
        {E_VDS_VD_EXCEED,     "E_VDS_VD_EXCEED",     "Virtual disk number exceeds the allowed limit system.", "虚拟盘的数量超过系统允许的上限。"},          /*vd 个数 超过最大数量 */
        {E_VDS_VOL_EXCEED,     "E_VDS_VOL_EXCEED",     "Volume number exceeds the allowed limit system.",  "虚拟盘上卷的数量超过系统允许的上限。"},          /*vol 个数 超过最大数量 */
        {E_VDS_SNAP_EXCEED,    "E_VDS_SNAP_EXCEED",     "Snapshot number exceeds the allowed limit system.",  "快照数量超过系统允许的上限。"},          /*snap 个数 超过最大数量 */
        {E_VDS_VD_EXIST,     "E_VDS_VD_EXIST",     "Virtual Disk with the same name already exists, please change the name and retry.",   "同名的虚拟盘已存在，请更换名字重新创建。"},           /*指定的vd 已存在，用于创建时 */

        {E_VDS_OBJECT_EXIST,     "E_VDS_OBJECT_EXIST",    "The  name used here already exists, please change the name and retry.",    "此处使用的名字已存在，请重试。"},         /*创建clone，snap，vol时系统中存在同名的同类对象 */
        {E_VDS_SNAP_EXIST,     "E_VDS_SNAP_EXIST",     "Snapshot with the same name already exists, please change the name and retry.",    "同名的快照已存在，请更换名字重新创建。"},         /*指定的snap 已存在，用于创建时 */
        {E_VDS_VOL_BUSY,     "E_VDS_VOL_BUSY",     "Volume is busy. Unable to execute the command.",   "卷正忙，无法执行命令。"},          /*vol 在忙, 无法执行命令  */
        {E_VDS_VOL_NO_READY,    "E_VDS_VOL_NO_READY",    "Volume is not ready.",        "卷未准备就绪。"},          /*vol 没准备好，一般指状态不满足操作*/

        {E_VDS_VD_RELATION_EXIST,    "E_VDS_VD_RELATION_EXIST",     "The device has relation with others,not allowed to freeze", "虚拟盘与其他虚拟盘有关联，不允许冻结"},
        {E_VDS_VD_REJECTED_FREEZED,    "E_VDS_VD_REJECTED_FREEZED",     "Virtual Disk is not at the state of good or degrade,not allowed to freeze", "虚拟盘当前状态不允许冻结，只有状态为良好或者降级的虚拟盘才允许冻结"},         /*vd状态非正常和降级 */
        {E_VDS_VOL_DEV_BUSY,   "E_VDS_VOL_DEV_BUSY",    "The device of Volume is busy.",     "卷设备正忙。"},         /*vol设备正在忙 */
        {E_VDS_VD_NOT_GOOD,   "E_VDS_VD_NOT_GOOD",    "Operation not supported on this state of Virtual Disk.",  "虚拟盘当前状态不支持该操作。"},         /*VD状态不为GOOD */
        {E_VDS_VOL_NOT_GOOD,   "E_VDS_VOL_NOT_GOOD",    "The state of Volume is not good,not allowed to do  the operation.",    "卷非正常状态，不允许该操作。"},         /*VOL状态不为GOOD */
        {E_VDS_VD_DEGRADE,    "E_VDS_VD_DEGRADE",     "VD is degraded, unable to execute cmd.",  "虚拟盘降级，无法执行命令"},         /*VD 发生了降级 */
        {E_VDS_WRONG_CHUNKSIZE,   "E_VDS_WRONG_CHUNKSIZE",   "Invalid chunk-size.",       "无效的条带大小。"},         /*chunk_size大小非法 */
        {E_VDS_OPEN_FAIL,    "E_VDS_OPEN_FAIL",     "Cannot open this file.",     "打开文件失败。"},         /*打开文件失败 */
        {E_VDS_MK_NOD_FAIL,    "E_VDS_MK_NOD_FAIL",    "Failed to create node.",      "mknod创建文件失败。"},         /*创建mkno失败   */
        {E_VDS_VOL_TYPE,    "E_VDS_VOL_TYPE",     "Invalid Volume type.",     "无效的卷类型。"},         /*卷类型非法 */
        {E_VDS_CAPACITY,     "E_VDS_CAPACITY",     "Invalid capacity.",     "无效的容量。"},         /*容量非法 */
        {E_VDS_NAME_INVALID,    "E_VDS_NAME_INVALID",     "Invalid name specified.",    "名字校验不通过。"},         /*名字校验不通过 */
        {E_VDS_RAID_LEVEL,    "E_VDS_RAID_LEVEL",     "Operation not supported on Virtual Disk of current level.","该虚拟盘raid级别不支持当前操作。"},         /*操作不支持对应的raid 级别 */

        {E_VDS_BAD_REBUILD_MODE,  "E_VDS_BAD_REBUILD_MODE",   "Invalid parameter of auto-build mode.",    "无效的重建模式。"},         /*输入的重建模式错误*/
        {E_VDS_VD_DEL_REJECT,   "E_VDS_VD_DEL_REJECT",    "Volumes exist. Unable to delete Virtual Disk.", "虚拟盘上仍存在卷，虚拟盘删除失败。"},         /*虚拟盘上有逻辑盘，不允许删除*/
        {E_VDS_PD_SET_INVALID,   "E_VDS_PD_SET_INVALID",    "Invalid number of disks for Virtual Disk.",  "用于创建虚拟盘的磁盘数量错误。。"},         /*磁盘数量错误*/
        {E_VDS_USER_MEM_ALLOC,   "E_VDS_USER_MEM_ALLOC",    "User memory allocation error.",     "用户态内存分配失败。"},         /*用户态内存分配失败*/
        {E_VDS_KERNEL_MEM_ALLOC,  "E_VDS_KERNEL_MEM_ALLOC",   "Kernel memory allocation error.",     "内核态内存分配失败。"},         /*内核态内存分配失败*/
        {E_VDS_DEV_NOT_EXIST,   "E_VDS_DEV_NOT_EXIST",    "The device does not exist.",      "该设备不存在。"},         /*设备不存在 */
        {E_VDS_SYS_NOT_EXIST,   "E_VDS_SYS_NOT_EXIST",    "The sysfs file does not exist.",    "该sysfs文件不存在。"},         /*sysfs文件不存在 */
        {E_VDS_NO_PERMISSION,   "E_VDS_NO_PERMISSION",    "Permission denied.",       "无权限。"},         /*无权限 */
        {E_VDS_DISK_TYPE_INVAL,   "E_VDS_DISK_TYPE_INVAL",    "Invalid type of member disk.",   "无效的成员盘类型。"},         /*成员盘类型非法 */
        {E_VDS_VD_NOSPC,    "E_VDS_VD_NOSPC",     "The rops of Virtual Disk is not NULL.",   "虚拟盘的rops不为空。"},         /*vd->rops 不为空*/

        {E_VDS_VD_SYS_REGISTER,   "E_VDS_VD_SYS_REGISTER",    "Failed to register in sysfs.",   "向sysfs注册失败。"},         /* 向sysfs 注册失败 */
        {E_VDS_RAID_NO_MOD,    "E_VDS_RAID_NO_MOD",     "The module of Virtual Disk of current level has not been loaded.",    "raid模块未加载。"},         /*raid 模块未加载 */
        {E_VDS_RAID_RUN_FAIL,   "E_VDS_RAID_RUN_FAIL",    "Failed to run Virtual Disk of current level.", "raid模块启动失败。"},         /*raid 模块启动失败*/
        {E_VDS_PTHREAD_CREATE,   "E_VDS_PTHREAD_CREATE",    "Failed to create pthread.",     "创建线程失败。"},         /*创建pthread失败 */
        {E_VDS_PTHREAD_DESTROY,   "E_VDS_PTHREAD_DESTROY",    "Failed to destroy pthread.",    "销毁线程失败。"},         /*销毁pthread失败 */
        {E_VDS_THREAD_NOTEXIST,   "E_VDS_THREAD_NOTEXIST",    "The thread does not exist.",    "该线程不存在。"},         /*查找线程不存在 */
        {E_VDS_THREAD_EXIST,    "E_VDS_THREAD_EXIST",    "The thread task already exists related to this operation,not allowed this command here.", "与该操作相关的任务已存在，不允许该操作。"},         /*线程已存在，不能重复创建 */
        {E_VDS_THREAD_CREATE,   "E_VDS_THREAD_CREATE",    "Failed to register the thread.",     "注册线程失败。"},         /*线程创建是出错 */
        {E_VDS_THREAD_SYNC,    "E_VDS_THREAD_SYNC",     "Failed to synchronize the thread.",   "线程同步失败。"},         /*线程同步出错 */
        {E_VDS_DEVNO_TO_UUID,   "E_VDS_DEVNO_TO_UUID",    "Failed to change the device-number into uuid.", "设备号转换uuid失败。"},         /*设备号转换成uuid错误*/
        {E_VDS_UUID_TO_DEVNO,   "E_VDS_UUID_TO_DEVNO",    "Failed to change the uuid into device-number.", "uuid转换设备号失败。"},         /*uuid转换成设备号错误*/
        {E_VDS_WAIT_TIME_OUT,   "E_VDS_WAIT_TIME_OUT",    "The peer response timeout.",     "等待对端超时。"},         /*等待对端超时*/
        {E_VDS_RVOL_CAPACITY_PERCENTAGE, "E_VDS_RVOL_CAPACITY_PERCENTAGE", "The percentage of capacity of repository volume to base volume should be 10%~100%.", "快照资源空间的大小应该是源卷大小的10%~100%之间。"},         /*实体卷应当占源卷的百分比在10%~100%之间*/
        {E_VDS_MSG_LENGTH,    "E_VDS_MSG_LENGTH",     "Invalid message length.",      "无效的消息长度。"},         /*消息长度有误*/

        {E_VDS_BVOL_NO_EXIST,   "E_VDS_BVOL_NO_EXIST",    "Base volume does not exist.",      "源卷不存在。"},         /*源卷不存在*/
        {E_VDS_SVOL_INIT_FAIL,   "E_VDS_SVOL_INIT_FAIL",    "Failed to initiate snapshot volume.",   "快照卷初始化失败。"},         /*快照卷初始化失败*/
        {E_VDS_RVOL_INIT_FAIL,   "E_VDS_RVOL_INIT_FAIL",    "Failed to initiate repository volume.",   "快照资源空间初始化失败。"},         /*实体卷初始化失败*/
        {E_VDS_EXCEPTIONSTORE_CREATE_FAIL, "E_VDS_EXCEPTIONSTORE_CREATE_FAIL",   "Failed to create exception store.", "例外仓库创建失败。"},         /*例外仓库创建失败*/
        {E_VDS_HASHTABLE_INIT_FAIL,  "E_VDS_HASHTABLE_INIT_FAIL",   "Failed to initiate hash table.",   "哈希表初始化失败。"},         /*哈希表初始化失败*/
        {E_VDS_SVOL_INVALID,    "E_VDS_SVOL_INVALID",    "Invalid snapshot volume.",      "无效的快照卷。"},         /*快照卷无效*/
        {E_VDS_SVOL_WRITE_STATE,  "E_VDS_SVOL_WRITE_STATE",   "The snapshot volume has been written.",    "该快照卷已被改写。"},         /*快照卷被写过*/
        {E_VDS_PENDING_EXCEPTIONS_RUNNING,  "E_VDS_PENDING_EXCEPTIONS_RUNNING",   "The pending exceptions are being processed.",   "pending例外记录表正在处理中。"},         /*PENDING例外记录表正在处理中*/
        {E_VDS_SVOL_REGISTERTHREAD_FAIL,   "E_VDS_SVOL_REGISTERTHREAD_FAIL",   "Failed to register thread for snapshot volume.", "快照卷注册线程失败。"},         /*注册线程失败*/
        {E_VDS_PSDMIOCLIENT_CREATE_FAIL,   "E_VDS_PSDMIOCLIENT_CREATE_FAIL",   "Failed to create dm_io_client for pstore.",  "pstore结构体成员DM_IO_CLIENT地址分配失败。"},         /*pstore结构体 成员DM_IO_CLIENT分配地址错误*/
        {E_VDS_PS_AREA_ALLOC_FAIL,    "E_VDS_PS_AREA_ALLOC_FAIL",    "Failed to allocate area for pstore.", "pstore结构体成员area空间分配失败。"},         /*pstore结构体 成员area 分配空间失败*/
        {E_VDS_PS_HEAD_AREA_ALLOC_FAIL,    "E_VDS_PS_HEAD_AREA_ALLOC_FAIL",    "Failed to allocate header_area for pstore.", "pstore结构体成员header_area分配失败。"},         /*pstore结构体 成员header_area分配空间失败*/
        {E_VDS_PS_CALLBACKS_ALLOC_FAIL,   "E_VDS_PS_CALLBACKS_ALLOC_FAIL",   "Failed to allocate callbacks for pstore.",  "pstore结构体成员callbacks空间分配失败。"},         /*pstore结构体 成员callbacks分配空间失败*/
        {E_VDS_SNAP_RUN_ERROR,     "E_VDS_SNAP_RUN_ERROR",     "Failed to modify the access port for base volume.", "修改源卷访问接口失败。"},         /*修改源卷访问接口失败*/
        {E_VDS_ZERO_DISK_AREA_FAIL,    "E_VDS_ZERO_DISK_AREA_FAIL",    "Failed to clear the area for COW recording.",  "磁盘COW记录区清零失败。"},         /*磁盘cow记录区清零失败*/

        {E_VDS_SOCKET_CREATE,     "E_VDS_SOCKET_CREATE",     "Failed to create socket.",       "套接字创建失败。"},         /*socket 创建失败 */
        {E_VDS_INIT_FAIL,       "E_VDS_INIT_FAIL",      "Failed to initiate user mode of block.",   "block用户态初始化失败。"},         /*blk 用户态初始化失败*/
        {E_VDS_CDEV_REGISTER,    "E_VDS_CDEV_REGISTER",     "Failed to register the char-device of vdk.",   "注册vdk字符设备失败。"},         /*vdk的字符设备注册失败 */
        {E_VDS_CDEV_INIT,      "E_VDS_CDEV_INIT",      "Failed to initiate the char-device of vdk.",   "初始化vdk字符设备失败。"},         /*vdk的字符设备初始化失败*/
        {E_VDS_TYPE_INVALID,                 "E_VDS_TYPE_INVALID",           "obj(vd,vol,snap) type invalid.",   "对象（虚拟盘，卷，快照）类型错误。"},
        {E_VDS_NOT_CONSISTENT,               "E_VDS_NOT_CONSISTENT",         "dual control data not consistent.",  "双控数据不一致。"},
        {E_VDS_RVOL_NOT_EXIST,   "E_VDS_RVOL_NOT_EXIST",    "The repository volume does not exist.",   "快照资源空间不存在。"},
        {E_VDS_SAME_VD,                  "E_VDS_SAME_VD",                 "The base volume and clone volume use the same Virtual Disk.The base volume and clone volume cannot create on the same Virtual Disk.",      "源卷和克隆卷所处虚拟盘相同，源卷和克隆卷不能在同一个VD上创建。"},
        {E_VDS_CLONE_MAX,               "E_VDS_CLONE_MAX",                 "The number of clone exceeds the maximum number.",     "该卷的克隆卷数量超过上限。"},
        {E_VDS_CLONE_UNBIND,            "E_VDS_CLONE_UNBIND",              "There is no clone bind",       "没有被绑定的克隆卷。"},
        {E_VDS_CLONE_UNFRACTURED,       "E_VDS_CLONE_UNFRACTURED",        "The clone is UNFRACTURED and operation not supported.",      "克隆卷当前为关联关系，不支持该操作。"},
        {E_VDS_WRONG_REBUILD_LEVEL,    "E_VDS_WRONG_REBUILD_LEVEL",          "The Virtual Disk with level 0 cannot rebuild itself.",    "raid0级别的虚拟盘无法自动重建。"},
        {E_VDS_DISK_RAID0 ,   "E_VDS_DISK_RAID0",    "The disk number must be between 1 and 32 when creating raid0 vd.",    "创建raid0级别虚拟盘时，磁盘数量应介于1~32之间。"},
        {E_VDS_DISK_RAID1 ,   "E_VDS_DISK_RAID1",    "The disk number must be 2 when creating raid1 vd.",       "创建raid1级别虚拟盘时，磁盘数量应为2。"},
        {E_VDS_DISK_RAID10 ,   "E_VDS_DISK_RAID10",    "The disk number must be an even number between 4 and 32 when creating raid10 vd.","创建raid10级别虚拟盘时，磁盘数量应为介于4~32的偶数。"},
        {E_VDS_DISK_RAID5 ,   "E_VDS_DISK_RAID5",    "The disk number must be between 3 and 16 when creating raid5 vd.",    "创建raid5级别虚拟盘时，磁盘数量应介于3~16之间。"},
        {E_VDS_DISK_RAID6 ,   "E_VDS_DISK_RAID6",    "The disk number must be between 4 and 16 when creating raid6 vd.",    "创建raid6级别虚拟盘时，磁盘数量应介于4~16之间。"},
        {E_VDS_RVOL_THR ,   "E_VDS_RVOL_THR",    "The repository volume thr must be between 50% and 95%.",    "实体卷阀值须在50%~95%之间。"},
        {E_VDS_BVOL_RESTORING_STATE ,   "E_VDS_BVOL_RESTORING_STATE",    "The base volume has been restoring.",    "源卷正在回滚中，稍后再试。"},
        {E_VDS_AUTOREBUILD_ALREADY_OFF ,   "E_VDS_AUTOREBUILD_ALREADY_OFF",    "Vd autorebuild is already OFF.",    "虚拟盘的自动重建状态已为关闭。"},
        {E_VDS_VOL_DEL_REJECT ,   "E_VDS_VOL_DEL_REJECT",    "Snapshot,Clone volume  or rvol exist. Unable to delete base volume.", "源卷上有快照卷、克隆卷或资源空间时，源卷删除失败。"},
        {E_VDS_VD_FAULT,   "E_VDS_VD_FAULT",     "VD is fault, unable to execute cmd.", "虚拟盘故障，无法执行命令。"},
        {E_VDS_NOTSUPPORT,   "E_VDS_NOTSUPPORT",     "Now not support this operation.", "目前不支持该操作。"},
        {E_VDS_NOT_LOCAL,   "E_VDS_NOT_LOCAL",   "The virtual disk is frozen.", "该虚拟盘已经冻结。"},
        {E_VDS_NOT_FOREIGN,   "E_VDS_NOT_FOREIGN",   "The virtual disk is not foreign.", "该虚拟盘不是外来的。"},
        {E_VDS_IMMIGRATE_FAIL,   "E_VDS_IMMIGRATE_FAIL",     "The virtual disk immigrate fail.", "该虚拟盘迁入失败。"},
        {E_VDS_NOT_IMMIGRATE,   "E_VDS_NOT_IMMIGRATE",   "The virtual disk has not immigrate.", "该虚拟盘还没有迁入，暂时无法使用。"},
        {E_VDS_VD_RECING  ,   "E_VDS_VD_RECING ",    "Virtual Disk is rebuilding, not support delete operation.", "虚拟盘正在重建，不支持删除操作。"},
        {E_VDS_VD_CREATE_FAIL,   "E_VDS_VD_CREATE_FAIL",    "Failed to create Virtual Disk.",  "创建虚拟盘失败。"},
        {E_VDS_VOL_CREATE_FAIL,   "E_VDS_VOL_CREATE_FAIL",    "Failed to create Volume.",  "创建卷失败。"},
        {E_VDS_SNAP_CREATE_FAIL,   "E_VDS_SNAP_CREATE_FAIL",    "Failed to create SnapShot Volume.",  "创建快照卷失败。"},
        {E_VDS_CLONE_CREATE_FAIL,   "E_VDS_CLONE_CREATE_FAIL",    "Failed to create Clone Volume.",  "创建克隆卷失败。"},
        {E_VDS_VD_DELETE_FAIL,   "E_VDS_VD_DELETE_FAIL",    "Failed to delete Virtual Diskl.",  "删除虚拟盘失败。"},
        {E_VDS_VOL_DELETE_FAIL,   "E_VDS_VOL_DELETE_FAIL",    "Failed to delete Volume.",  "删除卷失败。"},
        {E_VDS_SNAP_DELETE_FAIL,   "E_VDS_SNAP_DELETE_FAIL",    "Failed to delete SnapShot Volume.",  "删除快照卷失败。"},
        {E_VDS_CLONE_DELETE_FAIL,   "E_VDS_CLONE_DELETE_FAIL",    "Failed to delete Clone Volume.",  "删除克隆卷失败。"},
        {E_VDS_B_VD_RECING  ,   "E_VDS_B_VD_RECING ",  "Virtual Disk of base volume is rebuilding, cannot delete clone.", "源卷的虚拟盘正在重建，不能删除该克隆卷"},
        {E_VDS_CLONE_BUSY,     "E_VDS_CLONE_BUSY",       "The clone is synchronizing or reverse-synchronizing. Unable to execute this command.",  "克隆卷正在同步或反同步，无法执行该命令。"},
        {E_VDS_OPEN_DIR_FAIL,    "E_VDS_OPEN_DIR_FAIL",     "Cannot open this folder.",     "打开文件夹失败。"},         /*打开文件夹失败 */
        {E_VDS_BVOL_RSYNC,    "E_VDS_BVOL_RSYNC",    "The base volume is reverse-synchronizing. Unable to execute this command.",   "源卷正在进行反同步，无法执行该命令。"},
        {E_VDS_VD_NOT_RECING, "E_VDS_VD_NOT_RECING", "The Virtual Disk is not recovering!", "虚拟盘不是重建状态。"},
        {E_VDS_DEVNO_INVALID,"E_VDS_DEVNO_INVALID","The devno is repetitive ","出现重复的设备号"},
        {E_VDS_DEV_ALREADY_EXIST,"E_VDS_DEV_ALREADY_EXIST","The devicefile has been already created! ","相同id 的设备文件已经被创建了"},
        {E_VDS_PEER_TIMEOUT,    "E_VDS_PEER_TIMEOUT",    "The peer ack is timeout.",   "对端超时。"},
        {E_VDS_PEER_FAIL,    "E_VDS_PEER_FAIL",    "The peer operation is faild.",   "对端操作失败。"},
        {E_VDS_VD_REJECTED_IMMIGRATED,    "E_VDS_VD_REJECTED_IMMIGRATED",     "Virtual Disk is not at the state of good or degrade,not allowed to be immigrated", "虚拟盘当前状态非降级或者良好，不允许执行迁入操作"},         /*vd状态非正常和降级 */

    {E_VDS_VD_LOCALREJECTED_IMMIGRATED,    "E_VDS_VD_LOCALREJECTED_IMMIGRATED",     "Virtual Disk is a local freezed vd,not allowed to be immigrated", "虚拟盘是本地冻结虚拟盘，不允许执行迁入操作"},
        {E_VDS_VD_NO_DEGRADE,    "E_VDS_VD_NO_DEGRADE",     "The state of the Virtual Disk is not DEGRADED, cannot be rebuilded.", "虚拟盘非降级状态，不允许重建"},
        {E_VDS_SPAREDISK_NUM,    "E_VDS_SPAREDISK_NUM",     "The number of hotspare disks for recovery is wrong.", "为重建所提供的热备盘数目不对"},

        {E_VDS_GET_CLONE_INFO_FAIL,    "E_VDS_GET_CLONE_INFO_FAIL",     "Get clone info failed.", "获取克隆卷信息失败"},
        {E_VDS_SET_CLONE_PRIORITY_FAIL,    "E_VDS_SET_CLONE_PRIORITY_FAIL",     "Set clone priority failed.", "设置克隆卷priority失败"},
        {E_VDS_SET_CLONE_PROTECT_FAIL,    "E_VDS_SET_CLONE_PROTECT_FAIL",     "Set clone protect failed.", "设置克隆卷protect失败"},
        {E_VDS_SET_CLONE_NAME_FAIL,    "E_VDS_SET_CLONE_NAME_FAIL",     "Set clone name failed.", "克隆卷名设置失败"},
        {E_VDS_CLONE_FRACTURE_FAIL,    "E_VDS_CLONE_FRACTURE_FAIL",     "Clone fracture failed.", "克隆卷分离失败"},
        {E_VDS_CLONE_SYNC_FAIL,    "E_VDS_CLONE_SYNC_FAIL",     "Clone sync failed.", "克隆卷同步失败"},
        {E_VDS_CLONE_REVERSE_SYNC_FAIL,    "E_VDS_CLONE_REVERSE_SYNC_FAIL",     "Clone reverse sync failed.", "克隆卷反同步失败"},
        {E_VDS_GET_SVOL_INFO_FAIL,    "E_VDS_GET_SVOL_INFO_FAIL",     "Get snap info failed.", "获取快照卷信息失败"},
        {E_VDS_SET_SVOL_NAME_FAIL,    "E_VDS_SET_SVOL_NAME_FAIL",     "Set snap name failed.", "快照卷名设置失败"},
        {E_VDS_SET_RVOL_FAIL,    "E_VDS_SET_RVOL_FAIL",     "Set snap repository failed.", "快照存储空间告警阈值设置失败"},
        {E_VDS_SVOL_RESTORE_FAIL,    "E_VDS_SVOL_RESTORE_FAIL",     "Snap restore failed.", "数据恢复失败"},
        {E_VDS_GET_VD_INFO_BY_NAME_FAIL,    "E_VDS_GET_VD_INFO_BY_NAME_FAIL",     "Get vd info failed.", "获取虚拟盘信息失败"},
        {E_VDS_SET_VD_NAME_FAIL,    "E_VDS_SET_VD_NAME_FAIL",     "Set vd name failed.", "虚拟盘名设置失败"},
        {E_VDS_SET_VD_PRECTL_FAIL,    "E_VDS_SET_VD_PRECTL_FAIL",     "Set vd prectl failed.", "虚拟盘prefer ctl设置失败"},
        {E_VDS_SET_VD_OWNERCTL_FAIL,    "E_VDS_SET_VD_OWNERCTL_FAIL",     "Set vd ownerctl failed.", "虚拟盘owner ctl设置失败"},
        {E_VDS_SET_VD_AUTO_REBUILD_FAIL,    "E_VDS_SET_VD_AUTO_REBUILD_FAIL",     "Set vd auto rebuild failed.", "虚拟盘自动重建设置失败"},
        {E_VDS_VD_FREEZE_FAIL,    "E_VDS_VD_FREEZE_FAIL",     "Vd freeze failed.", "虚拟盘冻结失败"},
        {E_VDS_VD_UNFREEZE_FAIL,    "E_VDS_VD_UNFREEZE_FAIL",     "Vd unfreeze failed.", "虚拟盘解冻失败"},
        {E_VDS_VOL_RENAME_FAIL,    "E_VDS_VOL_RENAME_FAIL",     "Vol rename failed.", "卷重命名失败"},
        {E_VDS_GET_VOL_INFO_FAIL,    "E_VDS_GET_VOL_INFO_FAIL",     "Get vol info failed.", "获取卷信息失败"},
        {E_VDS_GET_VOL_STAT_FAIL,    "E_VDS_GET_VOL_STAT_FAIL",     "Get vol stat failed.", "设置卷状态失败"},
        {E_VDS_CLEAN_VOL_STAT_FAIL,    "E_VDS_CLEAN_VOL_STAT_FAIL",     "Clean vol stat failed.", "清除卷状态失败"},
        {E_VDS_SET_VOL_PRECTL_FAIL,    "E_VDS_SET_VOL_PRECTL_FAIL",     "Set vol prectl failed.", "设置卷prefer ctl失败"},
        {E_VDS_SET_VOL_OWNERCTL_FAIL,    "E_VDS_SET_VOL_OWNERCTL_FAIL",     "Set vol ownerctl failed.", "设置卷owner ctl失败"},
        {E_VDS_VD_CAPACITY_NOT_ENOUTH,    "E_VDS_VD_CAPACITY_NOT_ENOUTH",     "The capacity of vd is not enouth.", "虚拟盘容量不足"},
        {E_VDS_WRITE_FILE_FAIL,    "E_VDS_WRITE_FILE_FAIL",     "Write file failed.", "写文件失败"},
        {E_VDS_CLONE_FRACTURED,    "E_VDS_CLONE_FRACTURED",     "Clone is fractured.", "克隆卷处于分离状态"},
         {E_VDS_SVOL_RESTORING_STATE,  "E_VDS_SVOL_RESTORING_STATE",   "The snapshot volume has been restoring,not allowed to restore again.",    "该快照卷正在恢复中，不允许再次恢复。"},
         {E_VDS_RVOL_NO_SPACE,  "E_VDS_RVOL_NO_SPACE",   "The snap repository no space!",    "快照资源空间已满。"},
        {E_VDS_B_VD_NOTGOOD,   "E_VDS_B_VD_NOTGOOD ",  "Virtual Disk of base volume is not good, cannot create clone.", "源卷的虚拟盘状态不是良好，不能创建克隆卷"},
        {E_VDS_MIRROR_CREATE_FAIL, "E_VDS_MIRROR_CREATE_FAIL", "Failed to create mirror.", "创建远程镜像失败"},
        {E_VDS_MIRROR_NOT_EXIST, "E_VDS_MIRROR_NOT_EXIST", "mirror not exist.", "远程镜像不存在"},
        {E_VDS_MIRROR_GETINFO_FAIL, "E_VDS_MIRROR_GETINFO_FAIL", "Failed to get mirror info", "获取远程镜像信息失败"},
        {E_VDS_MIRROR_EXIST, "E_VDS_MIRROR_EXIST", "mirror is already exist.", "远程镜像已经存在"},
        {E_VDS_MIRROR_NO_SRC, "E_VDS_MIRROR_NO_SRC", "Failed to get mirror source vol", "获取远程镜像源卷失败"},
        {E_VDS_MIRROR_CAP, "E_VDS_MIRROR_CAP", "capacity of mirror dest vol is not equal to src vol", "远程镜像的目的卷容量大小与源卷不相等"},
        {E_VDS_MIRROR_SRC_HAS, "E_VDS_MIRROR_SRC_HAS", "src vol already has mirror.", "源卷已建立了远程镜像"},
        {E_VDS_MIRROR_SETNAME_FAIL, "E_VDS_MIRROR_SETNAME_FAIL", "Failed to set mirror name.", "远程镜像重命名失败"},
        {E_VDS_MIRROR_SETPOLICY_FAIL, "E_VDS_MIRROR_SETPOLICY_FAIL", "Failed to set mirror policy.", "设置远程镜像恢复策略失败"},
        {E_VDS_MIRROR_SETPRIORITY_FAIL, "E_VDS_MIRROR_SETPRIORITY_FAIL", "Failed to set mirror priority.", "设置远程镜像同步优先级失败"},
        {E_VDS_MIRROR_SETPERIOD_FAIL, "E_VDS_MIRROR_SETPERIOD_FAIL", "Failed to set mirror period.", "设置远程镜像同步周期失败"},
        {E_VDS_MIRROR_SRC_TYPE, "E_VDS_MIRROR_SRC_TYPE", "This vol can not be mirror src vol.", "该卷不能作为源卷镜像的源卷"},

        {E_VDS_RVOL_ALREADY_EXIST,"E_VDS_RVOL_ALREADY_EXIST","The snap repository of bvol has been already created ,not allowed to create again!","源卷的资源空间已经被创建了，不允许再次创建。"},
        {E_VDS_RVOL_CREATE_FAIL,   "E_VDS_RVOL_CREATE_FAIL",    "Failed to create snap repository.",  "创建资源空间失败。"},
        {E_VDS_NO_RVOL,     "E_VDS_NO_RVOL",      "Snap repository does not exist.",      "指定的快照资源空间不存在。"} ,
        {E_VDS_RVOL_DELETE_FAIL,   "E_VDS_RVOL_DELETE_FAIL",    "Failed to delete snap repository .",  "删除快照资源空间失败。"},
        {E_VDS_RVOL_DEL_REJECT,   "E_VDS_RVOL_DEL_REJECT",    "Snapshot volume exist. Unable to delete snap repository .",  "快照资源空间上有快照卷时，删除失败。"},
        {E_VDS_GET_RVOLINFO_FAIL,   "E_VDS_GET_RVOLINFO_FAIL",    "Cannot get snap repository info.",  "获取快照资源空间信息失败"},
        {E_VDS_VD_NOT_FREEZED,   "E_VDS_VD_NOT_FREEZED",   "The virtual disk is not frozen,not allowed to unfreeze.", "该虚拟盘未冻结，不允许解冻。"},
        {E_VDS_MIRROR_BUSY,     "E_VDS_MIRROR_BUSY",     "Mirror is busy. Unable to execute this command.",  "镜像正忙，无法执行命令。"},
        {E_VDS_MIRROR_SYNCING, "E_VDS_MIRROR_SYNCING", "Mirror is syncing. Unable to execute this command.", "镜像正在同步，无法执行命令。"},
        {E_VDS_MIRROR_DEST, "E_VDS_MIRROR_DEST", "remote disk is not exist.", "镜像远端磁盘不存在"},
        {E_VDS_MIRROR_MAXNUM, "E_VDS_MIRROR_MAXNUM", "The number of mirror exceeds the maximum number.", "镜像的数量达到上限"},
        {E_VDS_MIRROR_TYPE, "E_VDS_MIRROR_TYPE", "only support synchronous mirror now.", "目前只支持同步镜像"},
        {E_VDS_VD_SETFLAG_FAIL, "E_VDS_VD_SETFLAG_FAIL", "failed to set vd flag.", "设置VD状态失败"},

        {E_VDS_POOL_EXCEED, "E_VDS_POOL_EXCEED", "The number of pool exceeds the maximum number.", "存储池的数量达到上限。"},
        {E_VDS_KERNEL_POOL_CREATE_FAIL, "E_VDS_KERNEL_POOL_CREATE_FAIL", "create pool failed.", "创建存储池失败。"},
        {E_VDS_KERNEL_POOL_DELETE_FAIL, "E_VDS_KERNEL_POOL_DELETE_FAIL", "delete pool failed.", "删除存储池失败。"},
        {E_VDS_POOL_NOT_EXIST, "E_VDS_POOL_NOT_EXIST", "the pool is not exist.", "存储池不存在。"},
        {E_VDS_POOL_DEL_REJECT, "E_VDS_POOL_DEL_REJECT", "vol exist.Unable to delete the pool.", "存储池上有vol，无法删除。"},
        {E_VDS_POOL_EXIST, "E_VDS_POOL_EXIST", "the pool has the same name exist.", "同名的存储池已经存在。"},
        {E_VDS_KERNEL_POOL_MODIFY_FAIL, "E_VDS_KERNEL_POOL_MODIFY_FAIL", "modify pool failed.", "修改存储池失败。"},
        {E_VDS_POOL_CANNOT_REMOVE_DISK, "E_VDS_POOL_CANNOT_REMOVE_DISK", "vol exist,can not remove member disk.", "存储池上有vol，不能删除磁盘。"},
        {E_VDS_KERNEL_POOL_GETINFO_FAIL, "E_VDS_KERNEL_POOL_GETINFO_FAIL", "get the pool info failed.", "获取存储池信息失败。"},
        {E_VDS_POOL_ALARMTHRESHOLD_INVALID, "E_VDS_POOL_ALARMTHRESHOLD_INVALID", "pool AlarmThreshold error.", "容量告警阈值不合适"},
        {E_VDS_POOL_CAPACITY_NOT_ENOUTH, "E_VDS_POOL_CAPACITY_NOT_ENOUTH", "pool spare capacity is not enough.", "存储池容量不足"},
        {E_VDS_VOL_EXCEED_IN_SYS, "E_VDS_VOL_EXCEED_IN_SYS,", "Volume number exceeds the allowed limit system.", "系统中的vol数量已达到最大限制"},
        {E_VDS_POOL_STATE_INVALID, "E_VDS_POOL_STATE_INVALID", "can not execute this command on pool INSUFCAPAC.", "容量不足或盘数小于8的存储池上无法执行该命令"},
        {E_VDS_POOL_REMOVE_DISK_INVALID, "E_VDS_POOL_REMOVE_DISK_INVALID", "pool member disks will be less than 8 after execute this command,can not remove member disk.", "执行该命令后存储池成员盘数将不足8，不能执行该命令"},
        {E_VDS_VOL_CAPACITY_EXCEED, "E_VDS_VOL_CAPACITY_EXCEED", "vol capacity will exceed the maximum after execute this command,can not execute this command.", "执行该命令后vol容量会超过最大容量，不能执行该命令"},

        /*******************************SAS 子系统************************************/


        /*******************************JBODC 子系统************************************/


        /*******************************TCS 子系统************************************/
        {E_TCS_COPY_FROM_USER_FAIL,         "E_TCS_COPY_FROM_USER_FAIL",            "Failed to copy info from user ",                        "从用户态克隆数据到内核态失败"},
        {E_TCS_COPY_TO_USER_FAIL,           "E_TCS_COPY_TO_USER_FAIL",              "Failed to copy info to user ",                          "从内核态克隆数据到用户态失败"},
        {E_TCS_KMALLOC_FAIL,                "E_TCS_KMALLOC_FAIL",                   "Failed to kmalloc memory ",                             "获取内核存储空间失败"},
        {E_TCS_DOWN_SEM_FAIL,               "E_TCS_DOWN_SEM_FAIL",                  "Failed to get semaphore var ",                          "获取信号量失败"},
        {E_TCS_TGT_NOT_EXIST,               "E_TCS_TGT_NOT_EXIST",                  "Target not exist ",                                     "目标器不存在"},
        {E_TCS_TGT_ALREADY_EXIST,           "E_TCS_TGT_ALREADY_EXIST",              "Target Already Exist ",                                 "目标器已存在"},
        {E_TCS_TGT_SESSION_BUSY,            "E_TCS_TGT_SESSION_BUSY",               "Target session list is busy ",                          "会话处于忙碌状态"},
        {E_TCS_MAX_TGT_NUM_REACHED,         "E_TCS_MAX_TGT_NUM_REACHED",            "Max Target number reached ",                            "目标器数目已达最大数"},
        {E_TCS_SESSION_NOT_EXIST,           "E_TCS_SESSION_NOT_EXIST",              "Session not exist ",                                    "会话不存在"},
        {E_TCS_SESSION_ALREADY_EXIST,       "E_TCS_SESSION_ALREADY_EXIST",          "Session Already Exist ",                                "会话已存在"},
        {E_TCS_SESSION_CONN_BUSY,           "E_TCS_SESSION_CONN_BUSY",              "Session still have connections ",                       "会话中还存在链接"},
        {E_TCS_INVAL_IOCTL_CMD,             "E_TCS_INVAL_IOCTL_CMD",                "Invalid ioctl command ",                                "不能识别的命令"},
        {E_TCS_PORTGRP_NOT_EXIST,           "E_TCS_PORTGRP_NOT_EXIST",              "This portal group not exist ",                          "该端口组不存在"},
        {E_TCS_ALL_PORTGRP_BE_USED,         "E_TCS_ALL_PORTGRP_BE_USED",            "All portal group have been used "                       "所有端口组都被使用"},

        {E_TCS_DIR_OPEN_FAIL,               "E_TCS_DIR_OPEN_FAIL",                  "Dir open fail ",                                        "目录打开失败"},
        {E_TCS_FILE_OPEN_FAIL,              "E_TCS_FILE_OPEN_FAIL",                 "File open fail ",                                       "文件打开失败"},
        {E_TCS_SYSCALL_FAIL,                "E_TCS_SYSCALL_FAIL",                   "Syscall failed ",                                       "系统调用失败"},
        {E_TCS_SYSPROC_FAIL,                "E_TCS_SYSPROC_FAIL",                   "Sysproc failed ",                                       "系统处理失败"},
        {E_TCS_PARAM_NULL,                  "E_TCS_PARAM_NULL",                     "Param is null ",                                        "参数为空"},
        {E_TCS_PORTNUM_ERR,                 "E_TCS_PORTNUM_ERR",                    "Err portnum ",                                          "端口号错误"},
        {E_TCS_CFG_PARAM_NULL,              "E_TCS_CFG_PARAM_NULL",                 "Param is null ",                                        "参数为空"},
        {E_TCS_CFG_PORTNUM_ERR,             "E_TCS_CFG_PORTNUM_ERR",                "Err portnum ",                                          "端口号错误"},
        {E_TCS_CFG_SYS_PROC_FAIL,           "E_TCS_CFG_SYS_PROC_FAIL",              "System process failed ",                                "系统处理失败"},
        {E_TCS_CFG_INFO_DIFFER,             "E_TCS_CFG_INFO_DIFFER",                "Infomation if differ ",                                 "内核态和用户态信息不一致"},
        {E_TCS_CFG_IP_FAIL,                 "E_TCS_CFG_IP_FAIL",                    "Failed to config ip ",                                  "配置IP失败"},
        {E_TCS_CFG_ARP_FAIL,                "E_TCS_CFG_ARP_FAIL",                   "Failed to config arp ",                                 "配置ARP失败"},
        {E_TCS_CFG_MAC_FAIL,                "E_TCS_CFG_MAC_FAIL",                   "Failed to config mac ",                                 "配置MAC失败"},
        {E_TCS_CFG_MTU_FAIL,                "E_TCS_CFG_MTU_FAIL",                   "Failed to config mtu ",                                 "配置MTU失败"},
        {E_TCS_CFG_ROUTE_FAIL,              "E_TCS_CFG_ROUTE_FAIL",                 "Failed to config route ",                               "配置路由失败"},
        {E_TCS_CFG_ISCSI_FAIL,              "E_TCS_CFG_ISCSI_FAIL",                 "Failed to config iscsi ",                               "配置iSCSI失败"},
        {E_TCS_CFG_IPADD_FAIL,              "E_TCS_CFG_IPADD_FAIL",                 "Failed to add ip ",                                     "添加端口IP失败"},
        {E_TCS_CFG_IPDEL_FAIL,              "E_TCS_CFG_IPDEL_FAIL",                 "Failed to delete ip ",                                  "删除端口IP失败"},
        {E_TCS_CFG_ROUTE_ADD_FAIL,          "E_TCS_CFG_ROUTE_ADD_FAIL",             "Failed to add route ",                                  "添加路由失败"},
        {E_TCS_CFG_ROUTE_DEL_FAIL,          "E_TCS_CFG_ROUTE_DEL_FAIL",             "Failed to delete route ",                               "删除路由失败"},
        {E_TCS_CFG_IPADDR_ZERO,             "E_TCS_CFG_IPADDR_ZERO",                "Ip address is zero ",                                   "IP地址为0"},
        {E_TCS_CFG_IPMASK_ZERO,             "E_TCS_CFG_IPMASK_ZERO",                "Mask is zero ",                                         "掩码为0"},
        {E_TCS_CFG_IPADDR_IPMASK_ONE_ZERO,  "E_TCS_CFG_IPADDR_IPMASK_ONE_ZERO",     "IP or mask is zero ",                                   "IP或掩码有一个为0"},
        {E_TCS_CFG_IPMASK_FORMAT_ERR,       "E_TCS_CFG_IPMASK_FORMAT_ERR",          "Err mask format ",                                      "错误的掩码格式"},
        {E_TCS_CFG_IPMASK_LEN_ERR,          "E_TCS_CFG_IPMASK_LEN_ERR",             "Mask len err ",                                         "掩码长度错误"},
        {E_TCS_CFG_IPADDR_AREA_ERR,         "E_TCS_CFG_IPADDR_AREA_ERR",            "Err ip address ",                                       "IP地址范围错误"},
        {E_TCS_CFG_PORTIP_EXIST,            "E_TCS_CFG_PORTIP_EXIST",               "PortIp is already existed ",                            "端口IP已存在"},
        {E_TCS_CFG_PORTIP_SAMEDOMAIN,       "E_TCS_CFG_PORTIP_SAMEDOMAIN",          "Ip address is in the same domain ",                     "和端口IP同一网段"},
        {E_TCS_CFG_IPADDR_NOMATCH,          "E_TCS_CFG_IPADDR_NOMATCH",             "Ip address is not match ",                              "IP地址不匹配"},
        {E_TCS_CFG_IPADDR_NOCFG,            "E_TCS_CFG_IPADDR_NOCFG",               "Ip address is not config ",                             "IP地址没有配置"},
        {E_TCS_CFG_ROUTE_EXIST,             "E_TCS_CFG_ROUTE_EXIST",                "Route existed ",                                        "路由已存在"},
        {E_TCS_CFG_ROUTE_NOT_EXIST,         "E_TCS_CFG_ROUTE_NOT_EXIST",            "Route not existed ",                                    "路由不存在"},
        {E_TCS_CFG_ROUTE_CLASH,             "E_TCS_CFG_ROUTE_CLASH",                "Route clashed ",                                        "路由冲突"},
        {E_TCS_CFG_ROUTE_NEXHOP_NODIRECT,   "E_TCS_CFG_ROUTE_NEXHOP_NODIRECT",      "Nexthop is not direct ",                                "下一跳非直连"},
        {E_TCS_CFG_ROUTE_STATICNUM_MAX,     "E_TCS_CFG_ROUTE_STATICNUM_MAX",        "Static route num is over MAX ",                         "静态路由个数已达到上限"},
        {E_TCS_CFG_ROUTE_STATICNUM_ZERO,    "E_TCS_CFG_ROUTE_STATICNUM_ZERO",       "Static route num is zero ",                             "静态路由个数为0"},
        {E_TCS_CFG_MTU_AREA_ERR,            "E_TCS_CFG_MTU_AREA_ERR",               "Err Mtu value ",                                        "MTU取值错误"},
        {E_TCS_CFG_MAC_FORMAT_ERR,          "E_TCS_CFG_MAC_FORMAT_ERR",             "Mac format err ",                                       "MAC地址格式错误"},
        {E_TCS_CFG_MAC_REPEAT,              "E_TCS_CFG_MAC_REPEAT",                 "Mac address repeated ",                                 "MAC地址重复"},
        {E_TCS_CFG_ROUTE_MASK_ERR,          "E_TCS_CFG_ROUTE_MASK_ERR",             "Destination or mask is invalid,(Destination & Mask) != Destination.",                  "目的地址或掩码有错,(目的地址&掩码) != 目的地址"},
        {E_TCS_CFG_HAVE_STATICROUTE,        "E_TCS_CFG_HAVE_STATICROUTE",           "Have static routes existed,please delete first.",       "有静态路由存在,请先删除"},
        {E_TCS_TEST_LENGTH_ERR,             "E_TCS_TEST_LENGTH_ERR",                "Data Length error(0 or >1472).",                        "数据净荷长度错误(0 或者>1472)"},
        {E_TCS_TEST_INTER_ERR,              "E_TCS_TEST_INTER_ERR",                 "time interval error(0 or >1s).",                        "包间隔错误(0 或者>1s)"},
        {E_TCS_TEST_NUM_ERR,                "E_TCS_TEST_NUM_ERR",                   "Data number error(=0).",                                "发送个数错误(=0)"},
        {E_TCS_CFG_CTRLID_ERR,              "E_TCS_CFG_CTRLID_ERR",                 "Ctrl ID is invalid.",                                   "控制器ID错误"},
        {E_TCS_PM_ID,                       "E_TCS_PM_ID",                          "Err PM ID. ",                                           "错误的tcs性能统计号"},
        {E_TCS_PM_PORTSTAT_FUNCFLAG,        "E_TCS_PM_PORTSTAT_FUNCFLAG",           "Err PortStat function flag.",                           "端口性能统计功能标志错误"},
        {E_TCS_PM_PORTSTAT_CTRLFLAG,        "E_TCS_PM_PORTSTAT_CTRLFLAG",           "Err PortStat ctrl flag.",                               "端口性能统计控制标志错误"},
        {E_TCS_PM_PORTSTAT_MEMSIZE,         "E_TCS_PM_PORTSTAT_MEMSIZE",            "PortStat memsize err.",                                 "端口性能统计内存大小不够"},
        { -1, NULL,   NULL,     NULL}


    };

#endif



#endif
