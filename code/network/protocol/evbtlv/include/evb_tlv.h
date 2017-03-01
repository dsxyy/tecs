/*******************************************************************************
* 版权所有 (C)2007, 中兴通讯股份有限公司
*
* 文件名称： evb_tlv.h
* 文件标识： 
* 内容摘要： evb tlv header file
* 其它说明： 
* 当前版本： V1.0
* 作   者： liuhx
* 完成日期： 2011.12.26

* 修改记录1：
*    修改日期：2012/03/19
*    变更单号: 
*    修 改 人：liuhx
*    修改内容：evb联调修改
*******************************************************************************/

#ifndef _EVB_TLV_H
#define _EVB_TLV_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lldp_in.h"

/* 子类型暂不确定，同LLDP模块*/
#define LLDP_EVBTLV_SUBTYPE        (0xD)
#define LLDP_MOD_EVBTLV  (((OUI_IEEE_8021Qbg) << (8)) | (LLDP_EVBTLV_SUBTYPE))

#pragma pack(1)
struct evbtlv_info{
    /*不包含type和len，构建tlv只包含tlv中的info
    **其他的type和len可以保存在unpacktlv的type和len中*/
    u8 oui[3];
    u8 subtype;

    u8 rsv0:5;
    u8 bgid:1;
    u8 rrcap:1;
    u8 rrctr:1;

    u8 rsv1:4;
    u8 sgid:1;
    u8 rrreq:1;

    u8 rrstat:2;

    u8 r:3;
    u8 rte:5;

    u8 evbmode:2;
    u8 rol0:1;
    u8 rwd:5;

    u8 rsv2:2;
    u8 rol1:1;
    u8 rka:5;	
};
#pragma pack()

#pragma pack(1)
struct evbtlv_info_little{
    u8 oui[3];
    u8 subtype;

    u8 rrctr:1;
    u8 rrcap:1;
    u8 bgid:1;
    u8 rsv0:5;

    u8 rrstat:2;
    u8 rrreq:1;
    u8 sgid:1;
    u8 rsv1:4;

    u8 rte:5;
    u8 r:3;

    u8 rwd:5;
    u8 rol0:1;
    u8 evbmode:2;

    u8 rka:5;	
    u8 rol1:1;
    u8 rsv2:2;
};
#pragma pack()

/*定义evb tlv，mod中指向该结构体*/
struct evbtlv_data{
    u32 local_port;

    /* groupvid不考虑保存，ecp/vdp如果使用直接从配置中获取*/	
    u8  groupvid;

    /* 和下面的协商结果，模块内保存一份，sbp和urp也保存一份*/	
    u8  vidcontrol;

    /* 保存协商结果*/
    struct evbtlv_info t_evbtlv_info;

    /* 构建evbtlv使用*/	
    struct unpacked_tlv *evbtlv;
    
    /* 保存端口统计*/
    struct evbtlv_debug *evbtlv_debug;

    /*next prev point*/
    LIST_ENTRY(evbtlv_data) entry;
};

struct evbtlv_user_data {
    LIST_HEAD(evbtlv_head, evbtlv_data) head;
};

/* 定义evbtlv注册撤销统计*/
struct evbtlv_register_debug{
    u32 allocmod_succ;
    u32 freemod_succ;

    u32 allocmod_fail;
    u32 allocdata_fail;
    u32 freemod_fail;
};

/* 定义evbtlv各操作统计*/
struct evbtlv_debug{
    /* add port*/
    u32 port_exit;
    u32 allocdata_fail;
    u32 ifadd_succ;

    /* del port*/

    /* MIB del*/
    u32 mibdel_succ;
    u32 mibdel_fail;

    /* get tlv */
    u32 packtlv_succ;	
    u32 bldtlv_fail;
    u32 tlvsize_zero;
    u32 packtlv_fail;
	

    /* port up*/	

    /* port down*/	

    /* rxchange */	
    u32 unpacktlv_succ;
    u32 unpacktlv_fail;
    u32 not_OUI;
    u32 rxLen_error;
    u32 not_127;
    u32 not_evbtlv;
    u32 setcml_fail;
	
};


/*定义函数*/
struct packed_tlv *evbtlv_gettlv(struct lldp_port *port);

void evbtlv_ifup(u32 local_port);

void evbtlv_ifdown(u32 local_port);

struct lldp_module *evbtlv_register(void);

void evbtlv_unregister(struct lldp_module *mod);

int evbtlv_rchange(struct lldp_port *port, struct unpacked_tlv *tlv, bool *tlv_store);

void evbtlv_ifadd(u32 local_port);

void evbtlv_ifdel(u32 local_port);

u8 evbtlv_mibDeleteObject(struct lldp_port *port);

void evbtlv_init(struct evbtlv_data * ptEvbtlv_data);

void evbtlv_setneg(u32 local_port);

struct evbtlv_info* evbtlv_get_info(u32 local_port);

/* 从SBP和URP获取信息*/
u32 evbtlv_getconfig(u32 local_port, struct evbtlv_info * ptEvbInfo, struct evbtlv_info_little* ptLittleEvbInfo);

void evbtlv_shutdown(void);

/* debug function */
void test_set_station(u32 local_port, u8 sgid, u8 rrreq);
void test_set_bridge(u32 local_port, u8 bgid, u8 rrcap);
void evbtlv_setprint(u32 dwFlag);
void evbtlv_dbgshowregister(void);
void evbtlv_dbgshowport(u32 local_port);
void evbtlv_dbgshowbridge(u32 local_port);
void evbtlv_dbgshowstation(u32 local_port);

#ifdef __cplusplus
}
#endif

#endif /* _EVB_TLV_H */

