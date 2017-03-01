/*******************************************************************************
* ��Ȩ���� (C)2007, ����ͨѶ�ɷ����޹�˾
*
* �ļ����ƣ� evb_tlv.h
* �ļ���ʶ�� 
* ����ժҪ�� evb tlv header file
* ����˵���� 
* ��ǰ�汾�� V1.0
* ��   �ߣ� liuhx
* ������ڣ� 2011.12.26

* �޸ļ�¼1��
*    �޸����ڣ�2012/03/19
*    �������: 
*    �� �� �ˣ�liuhx
*    �޸����ݣ�evb�����޸�
*******************************************************************************/

#ifndef _EVB_TLV_H
#define _EVB_TLV_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lldp_in.h"

/* �������ݲ�ȷ����ͬLLDPģ��*/
#define LLDP_EVBTLV_SUBTYPE        (0xD)
#define LLDP_MOD_EVBTLV  (((OUI_IEEE_8021Qbg) << (8)) | (LLDP_EVBTLV_SUBTYPE))

#pragma pack(1)
struct evbtlv_info{
    /*������type��len������tlvֻ����tlv�е�info
    **������type��len���Ա�����unpacktlv��type��len��*/
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

/*����evb tlv��mod��ָ��ýṹ��*/
struct evbtlv_data{
    u32 local_port;

    /* groupvid�����Ǳ��棬ecp/vdp���ʹ��ֱ�Ӵ������л�ȡ*/	
    u8  groupvid;

    /* �������Э�̽����ģ���ڱ���һ�ݣ�sbp��urpҲ����һ��*/	
    u8  vidcontrol;

    /* ����Э�̽��*/
    struct evbtlv_info t_evbtlv_info;

    /* ����evbtlvʹ��*/	
    struct unpacked_tlv *evbtlv;
    
    /* ����˿�ͳ��*/
    struct evbtlv_debug *evbtlv_debug;

    /*next prev point*/
    LIST_ENTRY(evbtlv_data) entry;
};

struct evbtlv_user_data {
    LIST_HEAD(evbtlv_head, evbtlv_data) head;
};

/* ����evbtlvע�᳷��ͳ��*/
struct evbtlv_register_debug{
    u32 allocmod_succ;
    u32 freemod_succ;

    u32 allocmod_fail;
    u32 allocdata_fail;
    u32 freemod_fail;
};

/* ����evbtlv������ͳ��*/
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


/*���庯��*/
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

/* ��SBP��URP��ȡ��Ϣ*/
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

