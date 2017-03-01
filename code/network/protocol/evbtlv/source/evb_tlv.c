/********************************************************************************
* ��Ȩ���� (C)2007, ����ͨѶ�ɷ����޹�˾
*
* �ļ����ƣ� evb_tlv.c
* �ļ���ʶ�� 
* ����ժҪ�� evb tlv process file
* ����˵���� 
* ��ǰ�汾�� V1.0
* ��   �ߣ�  liuhx
* ������ڣ� 2011.12.26

* �޸ļ�¼1��
*    �޸����ڣ�2012/03/19
*    �������: 
*    �� �� �ˣ�liuhx
*    �޸����ݣ�evb�����޸�
* �޸ļ�¼2��
*    �޸����ڣ�2012/06/11
*    �������: EC611003000759
*    �� �� �ˣ�liuhx
*    �޸����ݣ�����ɹ����vm����hc_shell����Ƶ����������Ϣ��ӡ
*******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif
#include "evb_comm.h"
#include "lldp_in.h"
#include "evb_tlv.h"
#include "evb_cfg.h"
#include "ecp.h"
#include "evb_port_mgr.h"

struct evbtlv_register_debug *g_ptEvbtlv_RegDebug = NULL;
u32 g_print_evbtlv = 0;

/**
1.mod/tlvid/port typeid ������Ҫȷ��
**/

/*evb tlv 
1.���շ����µ��ھӣ�����
2.�տ�ʼӦ��ʹ��txTTR = 0����
3.localchange�仯�����µķ���
*/

static const struct lldp_mod_ops evbtlv_ops =  {
    evbtlv_register,
    evbtlv_unregister,
    evbtlv_gettlv,
    evbtlv_rchange,
    0,  
    evbtlv_ifup,
    evbtlv_ifdown,
    evbtlv_ifadd,
    evbtlv_ifdel,
    evbtlv_mibDeleteObject,
    0,
    0,
    0,
    0,
    0,
    0,
};

/*���ȸ���ģ��ID�ҵ���Ӧģ�飬Ȼ����ݶ˿ں���������*/
static struct evbtlv_data *evbtlv_data_ex(u32 local_port)
{
    struct evbtlv_user_data *bud;
    struct evbtlv_data *bd = NULL;

    bud = (struct evbtlv_user_data *)find_module_user_data_by_id(&gtLLDP.mod_head, LLDP_MOD_EVBTLV);
    if (bud) {
        LIST_FOREACH(bd, &bud->head, entry) {
            if (local_port == bd->local_port)
                return bd;
        }
    }

    return NULL;
}

/* ��������ʹ��*/
void test_set_bridge(u32 local_port, u8 bgid, u8 rrcap)
{
    struct evbtlv_data *bd;
    struct evbtlv_info *ptInfo;	

    bd = evbtlv_data_ex(local_port);
    if (!bd) {
        PRINTF("test_set_bridge:port %d exists\n", local_port);
        return;		
    }

    bd->groupvid = 0;	

    ptInfo = (struct evbtlv_info*)(&(bd->t_evbtlv_info));

    ptInfo->oui[0] = 0x00;
    ptInfo->oui[1] = 0x80;
    ptInfo->oui[2] = 0xc2;
    ptInfo->subtype = LLDP_EVBTLV_SUBTYPE;
    ptInfo->bgid    = bgid;
    ptInfo->rrcap   = rrcap;
    ptInfo->evbmode = 1;

    somethingChangedLocal(local_port);

    return;
}
/* ��������ʹ��*/
void test_set_station(u32 local_port, u8 sgid, u8 rrreq)
{
    struct evbtlv_data *bd;
    struct evbtlv_info *ptInfo;	

    bd = evbtlv_data_ex(local_port);
    if (!bd) {
        PRINTF("test_set_station:port %d exists\n", local_port);
        return;		
    }

    ptInfo = (struct evbtlv_info*)(&(bd->t_evbtlv_info));

    ptInfo->oui[0] = 0x00;
    ptInfo->oui[1] = 0x80;
    ptInfo->oui[2] = 0xc2;
    ptInfo->subtype = LLDP_EVBTLV_SUBTYPE;
    ptInfo->sgid = sgid;
    ptInfo->rrreq = rrreq;
    ptInfo->evbmode = 2;

    somethingChangedLocal(local_port);

    return;
}

struct evbtlv_info* evbtlv_get_info(u32 local_port)
{
    struct evbtlv_data *bd;
    struct evbtlv_info *ptInfo;	

    bd = evbtlv_data_ex(local_port);
    if (!bd) {
        return NULL;		
    }

    ptInfo = (struct evbtlv_info*)(&(bd->t_evbtlv_info));
    return ptInfo;
}

/*����evb tlv*/
static int evbtlv_bld_evb_tlv(struct evbtlv_data *bd)
{
    int rc = 0;
	
    struct unpacked_tlv *tlv = NULL;
    struct lldp_port *port = NULL;

    struct evbtlv_info_little tLittleEvbInfo;
    struct evbtlv_info_little *ptLittleEvbInfo = &tLittleEvbInfo;
	
    struct evbtlv_info tEvbInfo;
    struct evbtlv_info *ptEvbInfo = &tEvbInfo;

    if (NULL == bd)
    {
        goto out_err;
    }

    port = find_by_port(bd->local_port);
    if (NULL == port)
    {
        goto out_err;
    }    

    /*�ͷ�ԭ��TLV*/
    FREE_UNPKD_TLV(bd, evbtlv);

    /*���tlvid���жϱ�־�Ƿ�ʹ��*/
    if (!is_tlv_enabled(bd->local_port, TLVID_8021Qbg(LLDP_EVB_SUBTYPE))) 
    {
        EVB_LOG(EVB_LOG_ERROR, "evbtlv_bld_evb_tlv: port %d:Port Description disabled \n", bd->local_port);
        goto out_err;
    }

    /*����upackTLV*/
    tlv = create_tlv();
    if (!tlv)
        goto out_err;

    tlv->type = ORG_SPECIFIC_TLV;
    tlv->length = sizeof(struct evbtlv_info);
    tlv->info = (u8 *)GLUE_ALLOC(tlv->length);
    if(!tlv->info) {
        GLUE_FREE(tlv);
        tlv = NULL;
        goto out_err;
    }

    /* �Դ��ֽ���Ϊ׼*/
    if(0 == evb_local_is_bigendian())
    {	
        memset(ptLittleEvbInfo,0, sizeof(struct evbtlv_info_little));
        if(!(evbtlv_getconfig(bd->local_port, NULL, ptLittleEvbInfo)))
        {
            free_unpkd_tlv(tlv);
            goto out_err;
        }
        memcpy(tlv->info, (u8 *)(ptLittleEvbInfo), tlv->length);	   		
    }
    else
    {
        memset(ptEvbInfo,0, sizeof(struct evbtlv_info));
        if(!(evbtlv_getconfig(bd->local_port, ptEvbInfo, NULL)))
        {
            free_unpkd_tlv(tlv);
            goto out_err;
        }
        memcpy(tlv->info, (u8 *)(ptEvbInfo), tlv->length);	
    }
		
    bd->evbtlv = tlv;
    rc = 1;

out_err:
    return rc;
}

/*�ͷ�evb data�е�tlv*/
static void evbtlv_free_tlv(struct evbtlv_data *bd)
{
    /*evb tlvĿǰֻ����һ��TLV format*/
    if (bd) {
        FREE_UNPKD_TLV(bd, evbtlv);
    }
}

/*����evb data�е�tlv*/
static int evbtlv_bld_tlv(struct evbtlv_data *bd)
{
    int rc = 0;

    /*���PORT�����в��������port����ô������TLV*/
    if (!find_by_port(bd->local_port)) {
        goto out_err;
    }

    if (!evbtlv_bld_evb_tlv(bd)) {
        EVB_LOG(EVB_LOG_ERROR, "evbtlv_bld_tlv: port %d bld evbtlv failed \n", bd->local_port);
        goto out_err;
    }

    rc = 1;

out_err:
    return rc;
}

/*�ͷ�ĳ��ģ�������м���Ķ˿�*/
static void evbtlv_free_data(struct evbtlv_user_data *bud)
{
    struct evbtlv_data *bd;
    if (bud) {
        while (!LIST_EMPTY(&bud->head)) {
            bd = LIST_FIRST(&bud->head);
            LIST_REMOVE(bd, entry);

            /* �ͷ�evbtlv�е�tlv*/
            evbtlv_free_tlv(bd);

            /* �ͷŶ˿�ͳ��*/
            if(bd->evbtlv_debug)
            {
                 GLUE_FREE(bd->evbtlv_debug);
            }	
			
            /*����ʱ�����sbp��urp*/
            evbtlv_setneg(bd->local_port);
	
            GLUE_FREE(bd);
        }
    }
}

/*����EVB TLV*/
struct packed_tlv *evbtlv_gettlv(struct lldp_port *port)
{
    int size = 0;
    struct evbtlv_data *bd = NULL;
    struct packed_tlv *ptlv = NULL;

    bd = evbtlv_data_ex(port->local_port);
    if (!bd)
    {
        EVB_LOG(EVB_LOG_ERROR, "evbtlv_gettlv: port %d not exit \n", port->local_port);
        return NULL;
    }

    evbtlv_free_tlv(bd);
    if (!evbtlv_bld_tlv(bd)) {
        bd->evbtlv_debug->bldtlv_fail++;
        return NULL;
    }

    /* ����ǰ���Ѿ�����unpacktlv���������ͣ������Լ�value*/
    size = TLVSIZE(bd->evbtlv);
    if (!size)
    {
        bd->evbtlv_debug->tlvsize_zero++;
        return NULL;
    }

    /*����packTLV */
    ptlv = create_ptlv();
    if (!ptlv) 
    {
        /* ǰ���unpack tlv���ٴδ���ʱ���ͷ�*/
        bd->evbtlv_debug->packtlv_fail++;
        return NULL;
    }

    ptlv->tlv = (u8 *)GLUE_ALLOC(size);
    if (!ptlv->tlv)
    {
        free_pkd_tlv(ptlv);
        bd->evbtlv_debug->packtlv_fail++;
        return NULL;
    }

    ptlv->size = 0;
    /* ǰ��size+2�ں������ͺͳ��ȣ����溯����unpack��pack tlv ת������*/	
    PACK_TLV_AFTER(bd->evbtlv, ptlv, size, out_free);

    if(g_print_evbtlv)
    {
        PRINTF("Port:%d  SendPkt:%d\n", port->local_port, ptlv->size);
        DbgMemShow(ptlv->tlv, ptlv->size);
    }

    bd->evbtlv_debug->packtlv_succ++;
	
    return ptlv;

out_free:
    return free_pkd_tlv(ptlv);
}

void evbtlv_ifup(u32 local_port)
{
    /*�����������ԣ�״̬����������LLDP������*/
    if(g_print_evbtlv)
    {
        EVB_LOG(EVB_LOG_INFO, "evbtlv_ifup: port %d up \n", local_port);
    }
    return;
}

void evbtlv_ifdown(u32 local_port)
{
    /*����ط�Ӧ������������Ϣ�͹��ˣ������Ķ���LLDP��������
    **�Զ�Ҳ������rxTTL��ʱ
    */    
    struct evbtlv_data *bd;

    bd = evbtlv_data_ex(local_port);
    if (!bd) {
        EVB_LOG(EVB_LOG_ERROR, "evbtlv_ifdown: port %d not exists \n", local_port);		
        return;
    }

    /* ��գ�������Э��*/
    evbtlv_init(bd);	

    /* ����Ĭ������*/
    evbtlv_setneg(local_port);
    if(g_print_evbtlv)
    {
        EVB_LOG(EVB_LOG_INFO, "evbtlv_ifdown: port %d down \n", local_port);
    }
    return;
}

/*ע��ģ�飬ģ������дģ��id/ops��������/evb data��*/
struct lldp_module *evbtlv_register(void)
{
    struct lldp_module *mod ;
    struct evbtlv_user_data *bud;

    /* �ǿ����ͷţ���ֹ�ڴ�й¶*/
    if(g_ptEvbtlv_RegDebug)
    {
        GLUE_FREE(g_ptEvbtlv_RegDebug);
        g_ptEvbtlv_RegDebug = NULL;
    }

    /*ģ��ע��ͳ��*/
    g_ptEvbtlv_RegDebug = (struct evbtlv_register_debug *)GLUE_ALLOC(sizeof(struct evbtlv_register_debug));
    if(!g_ptEvbtlv_RegDebug)
    {
        EVB_LOG(EVB_LOG_ERROR, "failed to alloc lldp evbtlv register debug \n");
        goto out_err;
    }

    memset(g_ptEvbtlv_RegDebug, 0, sizeof(struct evbtlv_register_debug));

    mod = (struct lldp_module *)GLUE_ALLOC(sizeof(struct lldp_module));
    if (!mod) {
        g_ptEvbtlv_RegDebug->allocmod_fail++;
        EVB_LOG(EVB_LOG_ERROR, "failed to alloc lldp evbtlv module \n");
        goto out_err;
    }

    bud = (struct evbtlv_user_data *)GLUE_ALLOC(sizeof(struct evbtlv_user_data));
    if (!bud) {
        GLUE_FREE(mod);
        g_ptEvbtlv_RegDebug->allocdata_fail++;
        EVB_LOG(EVB_LOG_ERROR, "failed to alloc lldp module user data \n");
        goto out_err;
    }

    /*��ʼ����һ��ָ��evb data��headָ��*/
    LIST_INIT(&bud->head);

    mod->id = LLDP_MOD_EVBTLV;
    mod->ops = &evbtlv_ops;
    mod->data = bud;
	
    g_ptEvbtlv_RegDebug->allocmod_succ++;
    return mod;

out_err:
    return NULL;
}

/*����ģ�飬����ģ�������ж˿ڵ�data*/
void evbtlv_unregister(struct lldp_module *mod)
{
    if(!g_ptEvbtlv_RegDebug)
    {
        return;
    }
	
    if((!mod) || (!(mod->data)))
    {
        g_ptEvbtlv_RegDebug->freemod_fail++;
        return;
    }

    evbtlv_free_data((struct evbtlv_user_data *) mod->data);
    GLUE_FREE(mod->data);
    GLUE_FREE(mod);
	
    g_ptEvbtlv_RegDebug->freemod_succ++;
    return;
}

/*�������յ���unpack tlv*/
static bool evbtlv_unpack(struct lldp_port *port, struct unpacked_tlv *tlv, bool *tlv_store)
{
    /* Unpack tlvs and store in rx */
    struct evbtlv_data *data;

    struct evbtlv_info tEvbInfo;
    /* �о�������*/
    struct evbtlv_info *ptsInfo = &tEvbInfo;	

    struct evbtlv_info *ptdInfo = NULL;
	
    struct evbtlv_info_little *ptdInfoLittle = NULL;
    struct evbtlv_info tEvbInfoEx;
    struct evbtlv_info *ptdInfoEx = &tEvbInfoEx;	

    u32 flag = 0;
	
    struct tagEvbPort *ptEvbPort = NULL;	
    TEvbSbpAttr *ptEvbSbpAttr = NULL;
    TEvbUrpAttr  *ptEvbUrpAttr = NULL;

    struct tagEvbIssPort * ptIssPort = NULL;

    if (NULL == port || NULL == tlv || NULL == tlv_store)
    {
        return false;
    }  

    data = evbtlv_data_ex(port->local_port);
    if (NULL == data /*|| NULL == data->evbtlv*/)
    {
        return false;
    }

    ptEvbPort = GetEvbPort(data->local_port);
    if(NULL == ptEvbPort)
    {
        return false;
    }

    if(NULL == ptEvbPort->ptPortAttr)
    {
        return false;
    }

    if(ptEvbPort->ucEvbPortType == EM_EVB_PT_SBP)
    {
        ptEvbSbpAttr = (TEvbSbpAttr *)(ptEvbPort->ptPortAttr);
    }
    else if(ptEvbPort->ucEvbPortType == EM_EVB_PT_URP)
    {
        ptEvbUrpAttr = (TEvbUrpAttr *)(ptEvbPort->ptPortAttr);
    }
    else
    {
        return false;
    }

    *tlv_store = false;  

    memset(ptsInfo,0, sizeof(struct evbtlv_info));

    if(!(evbtlv_getconfig(data->local_port, ptsInfo, NULL)))
    {
        return false;            
    }
    
    if (!is_tlv_enabled(port->local_port, TLVID_8021Qbg(LLDP_EVB_SUBTYPE)))
    {
        return false;            
    }

    /*���evbtlv�ǿգ����ͷ�
    **evbtlv����ʱ�򹹽�unpack tlv
    **evbtlv����ʱ��������tlvʹ����?
    */
    if (data->evbtlv)
    {
        /*evbtlv�ͷź�����ÿգ���Ȼ���͹���evbtlvʱ�ٴ��жϷǿ��ͷţ�����쳣*/
        FREE_UNPKD_TLV(data, evbtlv);
    }

    /* �Դ��ֽ���Ϊ׼*/
    if(0 == evb_local_is_bigendian())
    {	
        ptdInfoLittle = (struct evbtlv_info_little*)(tlv->info);
        ptdInfoEx->oui[0] = ptdInfoLittle->oui[0];
        ptdInfoEx->oui[1] = ptdInfoLittle->oui[1];
        ptdInfoEx->oui[2] = ptdInfoLittle->oui[2];
        ptdInfoEx->subtype = ptdInfoLittle->subtype;
	
        ptdInfoEx->rrctr = ptdInfoLittle->rrctr;
        ptdInfoEx->rrcap = ptdInfoLittle->rrcap;
        ptdInfoEx->bgid = ptdInfoLittle->bgid;
        ptdInfoEx->rsv0 = ptdInfoLittle->rsv0;
        ptdInfoEx->rrstat = ptdInfoLittle->rrstat;
        ptdInfoEx->rrreq = ptdInfoLittle->rrreq;
        ptdInfoEx->sgid = ptdInfoLittle->sgid;
        ptdInfoEx->rsv1 = ptdInfoLittle->rsv1;
        ptdInfoEx->rte = ptdInfoLittle->rte;
        ptdInfoEx->r = ptdInfoLittle->r;
        ptdInfoEx->rwd = ptdInfoLittle->rwd;
        ptdInfoEx->rol0 = ptdInfoLittle->rol0;
        ptdInfoEx->evbmode = ptdInfoLittle->evbmode;
        ptdInfoEx->rka = ptdInfoLittle->rka;
        ptdInfoEx->rol1 = ptdInfoLittle->rol1;
        ptdInfoEx->rsv2 = ptdInfoLittle->rsv2;
        ptdInfo = ptdInfoEx;
    }
    else
    {
        ptdInfo = (struct evbtlv_info*)(tlv->info);	
    }

    /*��EVBTLV������*/
    if(ptdInfo->subtype != LLDP_EVBTLV_SUBTYPE)
    {
        data->evbtlv_debug->not_evbtlv++;
        return true;            
    }

    /*����data�е���Ϣ
    **VDP��Ҫ��EVBTLV������ȡVID�������Ȩ
    **ECP��Ҫ��EVBTLV������ȡR���Ե�
    */
    switch(ptdInfo->evbmode)
    {
        /*station------->bridge*/
        case EVBSYS_TYPE_STATION: 
        {

            if(ptsInfo->evbmode != EVBSYS_TYPE_BRIDGE)
            {
                return false;
            }

            /*Э���漰rrreq,rrcap,rrctr
            **rrreq������station,��station��adminReflectiveRelayRequest��������ʾ�Ƿ�Ҫ����֧��rr
            **rrcap�������ű��������
            */
            if((ptdInfo->rrreq == true) && (ptsInfo->rrcap == true))
            {

                if(data->t_evbtlv_info.rrctr != true)
                {
                    ptIssPort = GetIssPortFromEvbPort(data->local_port);
                    if(NULL == ptIssPort)
                    {
                        return false;
                    }
   
                    if(0 == evb_set_cml(ptIssPort->uiPortNo, 0))
                    {
                        data->t_evbtlv_info.rrctr = true;
					
                        ptEvbSbpAttr->bRRCTR = true;
                        ptEvbSbpAttr->tNegRes.ucRRCTR = true;
                        EVB_BIT_SET(flag,EVBTLV_CHG_RRCTR);

                        /* ֧��֧��evbtlv�Լ�RR���Զ���RR�ܷ����ÿ���? */
                        ptEvbSbpAttr->tNegRes.ucStatus = EVB_TLV_STATUS_SUCC;
                        EVB_BIT_SET(flag,EVBTLV_ISSUPPORT);

                        /* to do set RR attribute */	
                    }
                    else
                    {
                        /* ����ʧ�ܼ�¼*/
                        data->evbtlv_debug->setcml_fail++;
                    }
                }
            }
            else
            {
                if(data->t_evbtlv_info.rrctr != false)
                {
                    ptIssPort = GetIssPortFromEvbPort(data->local_port);
                    if(NULL == ptIssPort)
                    {
                        return false;
                    }
				
                    if(0 == evb_set_cml(ptIssPort->uiPortNo, 5))
                    {
                        data->t_evbtlv_info.rrctr = false;
					
                        ptEvbSbpAttr->bRRCTR = false;
                        ptEvbSbpAttr->tNegRes.ucRRCTR = false;
                        EVB_BIT_SET(flag,EVBTLV_CHG_RRCTR);

                        /* ֧��֧��evbtlv�Լ�RR���Զ���RR�ܷ����ÿ���? */
                        ptEvbSbpAttr->tNegRes.ucStatus = EVB_TLV_STATUS_INIT;
                        EVB_BIT_SET(flag,EVBTLV_ISSUPPORT);

                        /* to do set RR attribute */	
                    }
                    else
                    {
                        /* ����ʧ�ܼ�¼*/
                        data->evbtlv_debug->setcml_fail++;
                    }
                }
            }

            if((ptdInfo->sgid == true) && (ptsInfo->bgid == true))
            {
                if(data->vidcontrol != true)
                {
                    /* ����bridgeӵ��ʹ��groupid�ͷ���VID����*/
                    data->vidcontrol = true;
                    ptEvbSbpAttr->tNegRes.ucVIDorGID = true;
                    EVB_BIT_SET(flag,EVBTLV_CHG_VIDCONTROL);
                }
            }
            else
            {
                if(data->vidcontrol != false)
                {
                    /* ����bridgeӵ��ʹ��groupid�ͷ���VID����*/
                    data->vidcontrol = false;
                    ptEvbSbpAttr->tNegRes.ucVIDorGID = false;
                    EVB_BIT_SET(flag,EVBTLV_CHG_VIDCONTROL);
                }
            }

            if(ptdInfo->r > ptsInfo->r)
            {
                if(data->t_evbtlv_info.r != ptdInfo->r)
                {
                    data->t_evbtlv_info.r = ptdInfo->r;
                    ptEvbSbpAttr->tNegRes.ucEcpMaxRetries = ptdInfo->r;
                    EVB_BIT_SET(flag,EVBTLV_CHG_R);
                }

                //liuhx 2013-07-26 ͨ��ĳ���ֶα����Ƿ�Э�̳ɹ�
                ptEvbSbpAttr->tNegRes.ucStatus = EVB_TLV_STATUS_SUCC;
                EVB_BIT_SET(flag,EVBTLV_ISSUPPORT);		
            }
            else
            {
                if(data->t_evbtlv_info.r != ptsInfo->r)
                {
                    data->t_evbtlv_info.r = ptsInfo->r;
                    ptEvbSbpAttr->tNegRes.ucEcpMaxRetries = ptsInfo->r;
                    EVB_BIT_SET(flag,EVBTLV_CHG_R);
                }

                //liuhx 2013-07-26 ͨ��ĳ���ֶα����Ƿ�Э�̳ɹ�
                ptEvbSbpAttr->tNegRes.ucStatus = EVB_TLV_STATUS_SUCC;
                EVB_BIT_SET(flag,EVBTLV_ISSUPPORT);				
            }

            if(ptdInfo->rte > ptsInfo->rte)
            {
                if(data->t_evbtlv_info.rte != ptdInfo->rte)
                {
                    data->t_evbtlv_info.rte = ptdInfo->rte;
                    ptEvbSbpAttr->tNegRes.ucRTE = ptdInfo->rte;
                    EVB_BIT_SET(flag,EVBTLV_CHG_RTE);
                }
            }
            else
            {
                if(data->t_evbtlv_info.rte != ptsInfo->rte)
                {
                    data->t_evbtlv_info.rte = ptsInfo->rte;
                    ptEvbSbpAttr->tNegRes.ucRTE = ptsInfo->rte;
                    EVB_BIT_SET(flag,EVBTLV_CHG_RTE);
                }
            }

            if(ptdInfo->rwd > ptsInfo->rwd)
            {	
                if(data->t_evbtlv_info.rwd != ptdInfo->rwd)
                {
                    data->t_evbtlv_info.rwd = ptdInfo->rwd;
                    ptEvbSbpAttr->tNegRes.ucRWD = ptdInfo->rwd;
                    EVB_BIT_SET(flag,EVBTLV_CHG_RWD);
                }
            }
            else
            {
                if(data->t_evbtlv_info.rwd != ptsInfo->rwd)
                {
                    data->t_evbtlv_info.rwd = ptsInfo->rwd;
                    ptEvbSbpAttr->tNegRes.ucRWD = ptsInfo->rwd;
                    EVB_BIT_SET(flag,EVBTLV_CHG_RWD);
                }
            }  
			  
            if(ptdInfo->rka > ptsInfo->rka)
            {
                if(data->t_evbtlv_info.rka != ptdInfo->rka)
                {              
                    data->t_evbtlv_info.rka = ptdInfo->rka;
                    /* rol0�������rol1 ecp��vdp��������*/
                    data->t_evbtlv_info.rol1 = true;
                    ptEvbSbpAttr->tNegRes.ucRKA = ptdInfo->rka;
                    EVB_BIT_SET(flag,EVBTLV_CHG_RKA);
                }
            }
            else
            {
                if(data->t_evbtlv_info.rka != ptsInfo->rka)
                {              
                    data->t_evbtlv_info.rka = ptsInfo->rka;
                    /* rol0�������rol1 ecp��vdp��������*/
                    data->t_evbtlv_info.rol1 = false;
                    ptEvbSbpAttr->tNegRes.ucRKA = ptsInfo->rka;
                    EVB_BIT_SET(flag,EVBTLV_CHG_RKA);
                }
            }

            break;	 
        }

        /*bridge------->station*/	
        case EVBSYS_TYPE_BRIDGE: 
        {
            if(ptsInfo->evbmode != EVBSYS_TYPE_STATION)
            {
                return false;
            }

            /*rrctr=1���������Ѿ�֧��rr������
            **rrstat���ó�2�����rrctr = 0���ó�0
            */
            if(ptdInfo->rrctr == true)
            {
                if(data->t_evbtlv_info.rrstat != true)
                {
                    data->t_evbtlv_info.rrstat = true;
                    ptEvbUrpAttr->tNegRes.ucRRSTAT  = true;

                    /*������ط�����ΪЭ��OK*/
                    ptEvbUrpAttr->tNegRes.ucStatus = EVB_TLV_STATUS_SUCC;

                    EVB_BIT_SET(flag,EVBTLV_CHG_RRSTAT);
                }
            }
            else if(ptdInfo->rrctr == false)
            {
                if(data->t_evbtlv_info.rrstat != false)
                {
                    data->t_evbtlv_info.rrstat = false;
                    ptEvbUrpAttr->tNegRes.ucRRSTAT = false;
                    ptEvbUrpAttr->tNegRes.ucStatus = EVB_TLV_STATUS_INIT;
                    EVB_BIT_SET(flag,EVBTLV_CHG_RRSTAT);
                }
            }
            else/* unknown״̬������2����3*/
            {
                if(data->t_evbtlv_info.rrstat != 2)
                {
                    data->t_evbtlv_info.rrstat = 2;
                    ptEvbUrpAttr->tNegRes.ucRRSTAT = 2;
                    ptEvbUrpAttr->tNegRes.ucStatus = EVB_TLV_STATUS_INIT;
                    EVB_BIT_SET(flag,EVBTLV_CHG_RRSTAT);
                }
            }

            if(ptdInfo->r > ptsInfo->r)
            {
                if(data->t_evbtlv_info.r != ptdInfo->r)
                {
                    data->t_evbtlv_info.r = ptdInfo->r;
                    ptEvbUrpAttr->tNegRes.ucEcpMaxRetries = ptdInfo->r;
                    EVB_BIT_SET(flag,EVBTLV_CHG_R);
                }

                //liuhx 2013-07-26 ͨ��ĳ���ֶα����Ƿ�Э�̳ɹ�
                ptEvbUrpAttr->tNegRes.ucStatus = EVB_TLV_STATUS_SUCC;
                EVB_BIT_SET(flag,EVBTLV_ISSUPPORT);
            }
            else
            {
                if(data->t_evbtlv_info.r != ptsInfo->r)
                {
                    data->t_evbtlv_info.r = ptsInfo->r;
                    ptEvbUrpAttr->tNegRes.ucEcpMaxRetries  = ptsInfo->r;
                    EVB_BIT_SET(flag,EVBTLV_CHG_R);
                }

                //liuhx 2013-07-26 ͨ��ĳ���ֶα����Ƿ�Э�̳ɹ�
                ptEvbUrpAttr->tNegRes.ucStatus = EVB_TLV_STATUS_SUCC;
                EVB_BIT_SET(flag,EVBTLV_ISSUPPORT);				
            }

            if(ptdInfo->rte >ptsInfo->rte)
            {
                if(data->t_evbtlv_info.rte != ptdInfo->rte)
                {
                   data->t_evbtlv_info.rte = ptdInfo->rte;
                   ptEvbUrpAttr->tNegRes.ucRTE = ptdInfo->rte;
                   EVB_BIT_SET(flag,EVBTLV_CHG_RTE);
                }
            }
            else
            {
                if(data->t_evbtlv_info.rte != ptsInfo->rte)
                {
                   data->t_evbtlv_info.rte = ptsInfo->rte;
                   ptEvbUrpAttr->tNegRes.ucRTE = ptsInfo->rte;
                   EVB_BIT_SET(flag,EVBTLV_CHG_RTE);
                }
            }

            if(ptdInfo->rwd > ptsInfo->rwd)
            {
                if(data->t_evbtlv_info.rwd != ptdInfo->rwd)
                {
                    data->t_evbtlv_info.rwd = ptdInfo->rwd;
                    data->t_evbtlv_info.rol0 = true;
                    ptEvbUrpAttr->tNegRes.ucRWD = ptdInfo->rwd;
                    EVB_BIT_SET(flag,EVBTLV_CHG_RWD);
                }
            }
            else
            {
                if(data->t_evbtlv_info.rwd != ptsInfo->rwd)
                {
                    data->t_evbtlv_info.rwd = ptsInfo->rwd;
                    data->t_evbtlv_info.rol0 = false;
                    ptEvbUrpAttr->tNegRes.ucRWD = ptsInfo->rwd;
                    EVB_BIT_SET(flag,EVBTLV_CHG_RWD);
                }
            }

            if(ptdInfo->rka > ptsInfo->rka)
            {
                if(data->t_evbtlv_info.rka != ptdInfo->rka)
                {              
                    data->t_evbtlv_info.rka = ptdInfo->rka;
                    /* rol0�������rol1 ecp��vdp��������*/
                    data->t_evbtlv_info.rol1 = true;
                    ptEvbUrpAttr->tNegRes.ucRKA = ptdInfo->rka;
                    EVB_BIT_SET(flag,EVBTLV_CHG_RKA);
                }
            }
            else
            {
                if(data->t_evbtlv_info.rka != ptsInfo->rka)
                {              
                    data->t_evbtlv_info.rka = ptsInfo->rka;
                    /* rol0�������rol1 ecp��vdp��������*/
                    data->t_evbtlv_info.rol1 = false;
                    ptEvbUrpAttr->tNegRes.ucRKA = ptsInfo->rka;
                    EVB_BIT_SET(flag,EVBTLV_CHG_RKA);
                }
            }

            break;
        }
        default:
        {
            return false;
        }

    }

    if(0 != flag)
    {
        if(g_print_evbtlv)
        {
            PRINTF("evbtlv_unpack: notify flag %d \n", flag);
        }
		
        evbtlv_chg_notify(data->local_port, flag);
    }

    /* �������Ҳ�����Զ���Ϣ*/
    /*data->evbtlv = tlv;*/		
    *tlv_store = true;  	

    return true;
}


/*���ձ仯�޸ģ�����TLV*/
int evbtlv_rchange(struct lldp_port *port, struct unpacked_tlv *tlv, bool *tlv_store)
{
    u8 oui[OUI_SIZE] = INIT_IEEE8021_OUI;

    struct evbtlv_data *data;

    if((!port) || (!tlv))
    {
        return TLV_ERR;
    }

    if (true == *tlv_store)
    {
        return TLV_OK;
    }    	

    data = evbtlv_data_ex(port->local_port);
    if (!data)
    {
        EVB_LOG(EVB_LOG_ERROR, "evbtlv_rchange: port%d  not exit \n", port->local_port);
        return TLV_ERR;
    }

    /*
     * TYPE_127 is for the Organizationally Specific TLVS
     * More than 1 of this type is allowed.
     */
    if (tlv->type == TYPE_127) 
    {
        int res;

        if(g_print_evbtlv)
        {
            PRINTF("Port:%d RcvPkt:%d\n", port->local_port, tlv->length);
            DbgMemShow(tlv->info, tlv->length);
        }

        /*tlv�е�len��ʾinfo���ȣ�����С����OUI + Subtype�Ƚ�*/
        if (tlv->length < (OUI_SUB_SIZE))
        {
            data->evbtlv_debug->rxLen_error++;
            return TLV_ERR;
        }

        /*�Ƚ�OUI�Ƿ���ͬ����ͬ����*/
        if ((memcmp(tlv->info, &oui, OUI_SIZE) != 0))
        {
            data->evbtlv_debug->not_OUI++;
            return TLV_ERR;
        }

        res = evbtlv_unpack(port, tlv, tlv_store);
        if (!res)
        {
            data->evbtlv_debug->unpacktlv_fail++;
            return TLV_ERR;
        }

        data->evbtlv_debug->unpacktlv_succ++;
   
        return TLV_OK;
    }

    data->evbtlv_debug->not_127++;
    return TLV_OK;
}


/*��ģ������Ӷ˿�*/
void evbtlv_ifadd(u32 local_port)
{
    struct evbtlv_data *bd;
    struct evbtlv_user_data *bud;
    struct evbtlv_debug *debug;

    /* ���ȴ�lldpģ��������evbtlvģ��*/
    bud = (struct evbtlv_user_data *)find_module_user_data_by_id(&gtLLDP.mod_head, LLDP_MOD_EVBTLV);
    if(!bud)
    {
        EVB_LOG(EVB_LOG_ERROR, "evbtlv_ifadd: port%d  not find evbtlv module \n", local_port);
        return;
    }

    /* �����˿��Ƿ��Ѿ���ӣ�����򷵻�ʧ��*/	
    bd = evbtlv_data_ex(local_port);
    if (bd) {
        if(bd->evbtlv_debug)
        {
            bd->evbtlv_debug->port_exit++;
        }
        if(g_print_evbtlv)
        {
            EVB_LOG(EVB_LOG_INFO, "evbtlv_ifadd: port%d  exit \n", local_port);		
        }		
        return;
    }

    bd = (struct evbtlv_data *) GLUE_ALLOC(sizeof(struct evbtlv_data));
    if (!bd) {
        EVB_LOG(EVB_LOG_ERROR, "evbtlv_ifadd: port %d GLUE_ALLOC %ld failed \n", local_port, sizeof(struct evbtlv_data));
        return;
    }
    memset(bd, 0, sizeof(struct evbtlv_data));

    debug = (struct evbtlv_debug *) GLUE_ALLOC(sizeof(struct evbtlv_debug));
    if (!debug) {
        GLUE_FREE(bd);		
        EVB_LOG(EVB_LOG_ERROR, "evbtlv_ifadd: port %d GLUE_ALLOC debug failed \n", local_port);
        return;
    }
    memset(debug, 0, sizeof(struct evbtlv_debug));
    
    bd->local_port = local_port;
    bd->evbtlv_debug = debug;	

    LIST_INSERT_HEAD(&bud->head, bd, entry);
    bd->evbtlv_debug->ifadd_succ++;

    /* ���ó�Ĭ��ֵ*/
    evbtlv_setneg(local_port);
	
    return;
}


/*��ģ����ɾ���˿�*/
void evbtlv_ifdel(u32 local_port)
{
    struct evbtlv_data *bd;

    bd = evbtlv_data_ex(local_port);
    if (!bd)
    {
        EVB_LOG(EVB_LOG_ERROR, "evbtlv_ifdel: port %d not exit \n", local_port);
        return;
    }

    LIST_REMOVE(bd, entry);

    evbtlv_init(bd);	

    if(bd->evbtlv_debug)
    {
        GLUE_FREE(bd->evbtlv_debug);
        bd->evbtlv_debug = NULL;		
    }

    /* ɾ��ʱ�ָ�Ĭ�����ã���֪ͨecp/vdp */
    evbtlv_setneg(bd->local_port);

    GLUE_FREE(bd);
    
    if(g_print_evbtlv)
    {
        EVB_LOG(EVB_LOG_INFO, "evbtlv_ifdel: port %d del succ \n", local_port);
    }

    return;
}


/*��data�����ݽṹ����MIB����update/del���data�е����ݽ��в���*/
u8 evbtlv_mibDeleteObject(struct lldp_port *port)
{
    struct evbtlv_data *data;

    data = evbtlv_data_ex(port->local_port);
    if (!data)
    {
        EVB_LOG(EVB_LOG_ERROR, "evbtlv_mibDeleteObject: port %d not exit \n", port->local_port);
        return 0;
    }

    /* t_evbtlv_info�����ɽ����汾����Ϣ�����Ǳ�����Э����Ϣ�����ҳ�ʼ��*/
    evbtlv_init(data);

    /* ����sbp��urp�������������ܼ���Э��*/
    evbtlv_setneg(data->local_port);
	
    /* ���������ϱ����������Ҫ������Զ˷�����Ϣ*/
    /* ԭ��ɾ��remote info��ִ��somethingChangedRemote*/
    /*somethingChangedLocal(port->local_port);*/

    data->evbtlv_debug->mibdel_succ++;
    if(g_print_evbtlv)
    {
        EVB_LOG(EVB_LOG_INFO, "evbtlv_mibDeleteObject: port %d del succ \n", port->local_port);
    }

    return 1;
}

/* ���Э�̽��*/
void evbtlv_init(struct evbtlv_data * ptEvbtlv_data)
{
    memset(&(ptEvbtlv_data->t_evbtlv_info), 0, sizeof(struct evbtlv_info));

    ptEvbtlv_data->groupvid = 0;	
    ptEvbtlv_data->vidcontrol = 0;

    /* �ͷŵ�evbtlv*/	
    evbtlv_free_tlv(ptEvbtlv_data);
	
    return;	
}

/* ��SBP��URP��ȡ��Ϣ*/
u32 evbtlv_getconfig(u32 local_port, struct evbtlv_info * ptEvbInfo, struct evbtlv_info_little* ptLittleEvbInfo)
{
    struct tagEvbPort *ptEvbPort = NULL;	
    TEvbSbpAttr *ptEvbSbpAttr = NULL;
    TEvbUrpAttr  *ptEvbUrpAttr = NULL;

    ptEvbPort = GetEvbPort(local_port);
    if(NULL == ptEvbPort)
    {
        return 0;
    }
	
    if(ptEvbPort->ucEvbPortType == EM_EVB_PT_SBP)
    {
        ptEvbSbpAttr = (TEvbSbpAttr *)(ptEvbPort->ptPortAttr);
    }
    else if(ptEvbPort->ucEvbPortType == EM_EVB_PT_URP)
    {
        ptEvbUrpAttr = (TEvbUrpAttr *)(ptEvbPort->ptPortAttr);
    }
    else
    {
        return 0;
    }

    if(ptEvbPort->ucEvbPortType == EM_EVB_PT_SBP)
    {
        /* bridge*/	
        /* ����ط�������Э����Ҫ�Ĳ�����ecp/vdpʹ�õ�Ĭ��Э�̲���evbcfg_getSbp����*/
        if(ptEvbInfo)
        {
            ptEvbInfo->oui[0] = 0x00;
            ptEvbInfo->oui[1] = 0x80;
            ptEvbInfo->oui[2] = 0xc2;
            ptEvbInfo->subtype = LLDP_EVBTLV_SUBTYPE;
		 
            ptEvbInfo->bgid = ptEvbSbpAttr->bBGID;
            ptEvbInfo->rrcap = ptEvbSbpAttr->bRRCAP;
            ptEvbInfo->rrctr = ptEvbSbpAttr->bRRCTR;
            ptEvbInfo->evbmode = EVBSYS_TYPE_BRIDGE;
            ptEvbInfo->r = ptEvbSbpAttr->ucR;
            ptEvbInfo->rte = ptEvbSbpAttr->ucRTE;
            ptEvbInfo->rka = ptEvbSbpAttr->ucRKA; 
            ptEvbInfo->rwd = ptEvbSbpAttr->ucRWD;
            return 1;				
        }

        if(ptLittleEvbInfo)
        {
            ptLittleEvbInfo->oui[0] = 0x00;
            ptLittleEvbInfo->oui[1] = 0x80;
            ptLittleEvbInfo->oui[2] = 0xc2;
            ptLittleEvbInfo->subtype = LLDP_EVBTLV_SUBTYPE;
		 
            ptLittleEvbInfo->bgid = ptEvbSbpAttr->bBGID;
            ptLittleEvbInfo->rrcap = ptEvbSbpAttr->bRRCAP;
            ptLittleEvbInfo->rrctr = ptEvbSbpAttr->bRRCTR;
            ptLittleEvbInfo->evbmode = EVBSYS_TYPE_BRIDGE;
            ptLittleEvbInfo->r = ptEvbSbpAttr->ucR;
            ptLittleEvbInfo->rte = ptEvbSbpAttr->ucRTE;
            ptLittleEvbInfo->rka = ptEvbSbpAttr->ucRKA; 
            ptLittleEvbInfo->rwd = ptEvbSbpAttr->ucRWD;
            return 1;	
        }
    }
    else
    {
        /* station */	
        /* ����ط�������Э����Ҫ�Ĳ�����ecp/vdpʹ�õ�Ĭ��Э�̲���evbcfg_getSbp����*/
        if(ptEvbInfo)
        {
            ptEvbInfo->oui[0] = 0x00;
            ptEvbInfo->oui[1] = 0x80;
            ptEvbInfo->oui[2] = 0xc2;
            ptEvbInfo->subtype = LLDP_EVBTLV_SUBTYPE;
			 
            ptEvbInfo->sgid = ptEvbUrpAttr->bSGrpID;
            ptEvbInfo->rrreq = ptEvbUrpAttr->bRRReq;
            ptEvbInfo->r = ptEvbUrpAttr->ucR;
            ptEvbInfo->evbmode = EVBSYS_TYPE_STATION;
            ptEvbInfo->rte = ptEvbUrpAttr->ucRTE;
            ptEvbInfo->rka = ptEvbUrpAttr->ucRKA;
            ptEvbInfo->rwd = ptEvbUrpAttr->ucRWD; 
            return 1;	
        }

        if(ptLittleEvbInfo)
        {
            ptLittleEvbInfo->oui[0] = 0x00;
            ptLittleEvbInfo->oui[1] = 0x80;
            ptLittleEvbInfo->oui[2] = 0xc2;
            ptLittleEvbInfo->subtype = LLDP_EVBTLV_SUBTYPE;
            ptLittleEvbInfo->sgid = ptEvbUrpAttr->bSGrpID;
            ptLittleEvbInfo->rrreq = ptEvbUrpAttr->bRRReq;
            ptLittleEvbInfo->r = ptEvbUrpAttr->ucR;
            ptLittleEvbInfo->evbmode = EVBSYS_TYPE_STATION;
            ptLittleEvbInfo->rte = ptEvbUrpAttr->ucRTE;
            ptLittleEvbInfo->rka = ptEvbUrpAttr->ucRKA;
            ptLittleEvbInfo->rwd = ptEvbUrpAttr->ucRWD; 
            return 1;	
        }
    }

    return 0;	
}

void evbtlv_setneg(u32 local_port)
{
    struct tagEvbPort *ptEvbPort = NULL;	
    TEvbSbpAttr *ptEvbSbpAttr = NULL;
    TEvbUrpAttr  *ptEvbUrpAttr = NULL;
    struct tagEvbIssPort * ptIssPort = NULL;

    ptEvbPort = GetEvbPort(local_port);
    if(NULL == ptEvbPort)
    {
        return;
    }
	
    if(NULL == ptEvbPort->ptPortAttr)
    {
        return;
    }
	
    if(ptEvbPort->ucEvbPortType == EM_EVB_PT_SBP)
    {
        ptEvbSbpAttr = (TEvbSbpAttr *)(ptEvbPort->ptPortAttr);
    }
    else if(ptEvbPort->ucEvbPortType == EM_EVB_PT_URP)
    {
        ptEvbUrpAttr = (TEvbUrpAttr *)(ptEvbPort->ptPortAttr);
    }
    else
    {
        return;
    }

    if(ptEvbPort->ucEvbPortType == EM_EVB_PT_SBP)
    {
        /* ������������Ҫʵ�ʵ�Э�̣�ֻ�����ó�0*��station��ͬ��*/
        ptEvbSbpAttr->tNegRes.ucRRCTR = 0;
        ptEvbSbpAttr->tNegRes.ucVIDorGID = 0;

        /* ���²�����ʹ��ͨ��Э�̣�Ҳ�����óɱ��ص�Ĭ��ֵ��station��ͬ��*/
        ptEvbSbpAttr->tNegRes.ucEcpMaxRetries= ptEvbSbpAttr->ucR;
        ptEvbSbpAttr->tNegRes.ucRTE = ptEvbSbpAttr->ucRTE;
        ptEvbSbpAttr->tNegRes.ucRWD = ptEvbSbpAttr->ucRWD;
        ptEvbSbpAttr->tNegRes.ucRKA = ptEvbSbpAttr->ucRKA;

        /* �����ǰЭ��״̬Ϊinit�����β����ϱ� */
        /* Э�����reflectiveδ�ɹ��������ֶ�Ҳ��Э�̣���reflective��־�Ƿ�Э�̳ɹ�
        ** �������ѿ��ܵ�Э��ֵ�ı��Ĭ��ֵ���������int̬���ϱ����ϲ�����Ƿ�Э��
        ** �ɹ�ȡЭ��ֵ��Ĭ��ֵ��
        **/
        if(EVB_TLV_STATUS_INIT == ptEvbSbpAttr->tNegRes.ucStatus)
        {
            return;
        }

        ptEvbSbpAttr->tNegRes.ucStatus  = EVB_TLV_STATUS_INIT;

        evbtlv_chg_notify(local_port, 0xffffffff);

        ptIssPort = GetIssPortFromEvbPort(local_port);
        if(NULL == ptIssPort)
        {
            return;
        }

        /* ���û�����ģʽ*/
        if(0 != evb_set_cml(ptIssPort->uiPortNo, 5))
        {
            EVB_LOG(EVB_LOG_ERROR, "evbtlv_setneg: port %d set cml failed \n", local_port);
            return;
        }

    }
    else
    {
        ptEvbUrpAttr->tNegRes.ucEcpMaxRetries = ptEvbUrpAttr->ucR;
        ptEvbUrpAttr->tNegRes.ucRTE = ptEvbUrpAttr->ucRTE;
        ptEvbUrpAttr->tNegRes.ucRWD = ptEvbUrpAttr->ucRWD;
        ptEvbUrpAttr->tNegRes.ucRKA = ptEvbUrpAttr->ucRKA;

        /* �����ǰЭ��״̬Ϊinit�����β����ϱ� */
        if(EVB_TLV_STATUS_INIT == ptEvbUrpAttr->tNegRes.ucStatus)
        {
            return;
        }

        ptEvbUrpAttr->tNegRes.ucStatus  = EVB_TLV_STATUS_INIT;

        evbtlv_chg_notify(local_port, 0xffffffff);		
    }

    return;
}

/* evbtlv protocol shutdown process */
/* lldp_deinit��ʵ�������Ʋ���*/
void evbtlv_shutdown(void)
{
    struct lldp_module *mod;

    /*��ͬ�ڳ���evbtlvģ��*/
    mod = find_module_by_id(&gtLLDP.mod_head, LLDP_MOD_EVBTLV);
    if (mod)
    {
        evbtlv_unregister(mod);
    }
	
    return;
}

/* ���ô�ӡ���ĵ�ȫ�ֱ���*/
void evbtlv_setprint(u32 dwFlag)
{
    g_print_evbtlv = dwFlag;
    return;
}

/* ��ӡevbtlvģ��ע�����*/
void evbtlv_dbgshowregister(void)
{

    if(!g_ptEvbtlv_RegDebug)
    {
        return;
    }

    PRINTF("\n     alloc mod succ: %d \n",g_ptEvbtlv_RegDebug->allocmod_succ);
    PRINTF("      free mod succ: %d \n",g_ptEvbtlv_RegDebug->freemod_succ);

    PRINTF("     alloc mod fail: %d \n",g_ptEvbtlv_RegDebug->allocmod_fail);
    PRINTF("alloc mod data fail: %d \n",g_ptEvbtlv_RegDebug->allocdata_fail);
    PRINTF("      free mod fail: %d \n",g_ptEvbtlv_RegDebug->freemod_fail);	

    return;
}

/* ��ӡevbtlv�����������*/
void evbtlv_dbgshowport(u32 local_port)
{

    struct evbtlv_data *bd;

    bd = evbtlv_data_ex(local_port);
    if (!bd) {
        PRINTF("evbtlv_data_ex:port %d not exists\n", local_port);
        return;		
    }

    if (!(bd->evbtlv_debug)) {
        PRINTF("port%d: evbtlv debug is null\n", local_port);
        return;		
    }
	

    PRINTF("\n    add port but port exists: %d \n",bd->evbtlv_debug->port_exit);
    PRINTF("  add port alloc data failed: %d \n",bd->evbtlv_debug->allocdata_fail);
    PRINTF("               add port succ: %d \n",bd->evbtlv_debug->ifadd_succ);

    PRINTF("                mib del succ: %d \n",bd->evbtlv_debug->mibdel_succ);
    PRINTF("              mib del failed: %d \n",bd->evbtlv_debug->mibdel_fail);
	
    PRINTF("                bld tlv succ: %d \n",bd->evbtlv_debug->packtlv_succ);
    PRINTF("              bld tlv failed: %d \n",bd->evbtlv_debug->bldtlv_fail);
    PRINTF("        bld tlv size is zero: %d \n",bd->evbtlv_debug->tlvsize_zero);	
    PRINTF("       bld packed tlv failed: %d \n",bd->evbtlv_debug->packtlv_fail);	

    PRINTF("        parse unpacktlv succ: %d \n",bd->evbtlv_debug->unpacktlv_succ);
    PRINTF("     parse unpacktlv failed : %d \n",bd->evbtlv_debug->unpacktlv_fail);	
    PRINTF("        parse tlv is NOT OUI: %d \n",bd->evbtlv_debug->not_OUI);	
    PRINTF("      parse tlv rx len error: %d \n",bd->evbtlv_debug->rxLen_error);
    PRINTF("           parse tlv not 127: %d \n",bd->evbtlv_debug->not_127);	
    PRINTF("        parse tlv not evbtlv: %d \n",bd->evbtlv_debug->not_evbtlv);	
    PRINTF("    parse tlv set cml failed: %d \n",bd->evbtlv_debug->setcml_fail);

    return;
}

void evbtlv_dbgshowbridge(u32 local_port)
{

    struct evbtlv_data *bd;

    struct evbtlv_info tEvbInfo;
    struct evbtlv_info *ptInfo = &tEvbInfo;	

    struct evbtlv_info *ptInfo_Data;	

    bd = evbtlv_data_ex(local_port);
    if (!bd) {
        PRINTF("evbtlv_dbgshowbridge:port %d not exists\n", local_port);
        return;		
    }

    ptInfo_Data = &(bd->t_evbtlv_info);

    memset(ptInfo,0, sizeof(struct evbtlv_info));
    if(!(evbtlv_getconfig(bd->local_port, ptInfo, NULL)))
    {
        PRINTF("evbtlv_dbgshowbridge:port %d get config failed \n", local_port);
        return;
    }

    PRINTF("\nport:%d config: \n",local_port);
    PRINTF("bgid: %d rrcap:%d \n", ptInfo->bgid, ptInfo->rrcap);
    PRINTF("r: %d rte:%d rol0:%d rwd:%d rol1:%d rka:%d\n", ptInfo->r, ptInfo->rte,ptInfo->rol0, ptInfo->rwd, ptInfo->rol1, ptInfo->rka);

    PRINTF("\nport:%d neg: \n",local_port);
    PRINTF("vidcontrol:%d \n",bd->vidcontrol);
    PRINTF("rrctr:%d \n", ptInfo_Data->rrctr);
    PRINTF("r: %d rte:%d rol0:%d rwd:%d rol1:%d rka:%d \n", ptInfo_Data->r, ptInfo_Data->rte, ptInfo_Data->rol0, ptInfo_Data->rwd, ptInfo_Data->rol1, ptInfo_Data->rka);	

    return;
}

void evbtlv_dbgshowstation(u32 local_port)
{

    struct evbtlv_data *bd;

    struct evbtlv_info tEvbInfo;
    struct evbtlv_info *ptInfo = &tEvbInfo;	

    struct evbtlv_info *ptInfo_Data;	

    bd = evbtlv_data_ex(local_port);
    if (!bd) {
        PRINTF("evbtlv_dbgshowstation:port %d not exists\n", local_port);
        return;		
    }

    ptInfo_Data = &(bd->t_evbtlv_info);

    memset(ptInfo,0, sizeof(struct evbtlv_info));
    if(!(evbtlv_getconfig(bd->local_port, ptInfo, NULL)))
    {
        PRINTF("evbtlv_dbgshowstation:port %d get config failed \n", local_port);
        return;
    }

    PRINTF("\nport:%d config: \n",local_port);
    PRINTF("sgid: %d rrcap:%d rrstat:%d \n", ptInfo->sgid, ptInfo->rrreq, ptInfo->rrstat);
    PRINTF("r: %d rte:%d rol0:%d rwd:%d rol1:%d rka:%d\n", ptInfo->r, ptInfo->rte,ptInfo->rol0, ptInfo->rwd, ptInfo->rol1, ptInfo->rka);

    PRINTF("\nport:%d neg: \n",local_port);
    PRINTF("rrstat:%d \n", ptInfo_Data->rrstat);
    PRINTF("r: %d rte:%d rol0:%d rwd:%d rol1:%d rka:%d\n", ptInfo_Data->r, ptInfo_Data->rte,ptInfo_Data->rol0, ptInfo_Data->rwd, ptInfo_Data->rol1, ptInfo_Data->rka);

    return;
}

#ifdef __cplusplus
}
#endif
