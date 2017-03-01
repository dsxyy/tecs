#ifdef __cplusplus
extern "C"{
#endif 

#include "evb_comm.h"
#include "vdp.h"
#include "vdp_in.h"
#include "evb_cfg.h"
#include "vsi_profile.h"
    
extern int vdp_dbg_print;

#define  VSIID_T1 "192.168.0.1"
     u32 VSIID_T1_FLT1_GRP = 10;
      u8 VSIID_T1_FLT1_MAC[EVB_ECP_MAC_LEN] = {0x01,0x02,0x03,0x04,0x05,0x06};
#define  VSIID_T1_FLT1_VLAN 10
     u32 VSIID_T1_FLT2_GRP = 20;
      u8 VSIID_T1_FLT2_MAC[EVB_ECP_MAC_LEN] = {0x02,0x03,0x04,0x05,0x06,0x07};
#define  VSIID_T1_FLT2_VLAN 20
#define  VSIID_T2 "192.168.0.2"

void vdp_print(int i)
{
    vdp_dbg_print =i;
}

void t_cfg_SB(BYTE sb)
{
    EvbcfgSetSysType(sb);
}

void t_cfg_urp(u32 port)
{
    
}

void t_vsiProfile_AddDelflt(u32 tNo, BYTE mac0,BYTE mac1,BYTE mac2,BYTE mac3,BYTE mac4,BYTE mac5,u32 grpid,u16 vlan,BYTE opt)
{
#if 0
    char vid[VDPTLV_VSIID_LEN]={0};
    u32 ipv4 = 0;
    BYTE vidFormat = VSIID_FORMAT_IPV4;
    BYTE mac[6]={0};
    u32 ret = 0;
    struct tagVDP_FLT_GROUPMACVID  fltGMV;

#if 1
    switch(tNo)
    {
    case 1:
        {
            ipv4 = evb_string_to_ipv4(VSIID_T1);
            MEMCPY(vid+VDPTLV_VSIID_LEN-EVB_IPV4_ADDR_LEN,&ipv4,sizeof(ipv4));
        }
        break;
    case 2:
        {
            ipv4 = evb_string_to_ipv4(VSIID_T2);
            MEMCPY(vid+VDPTLV_VSIID_LEN-EVB_IPV4_ADDR_LEN,&ipv4,sizeof(ipv4));
        }
        break;
    default:
        {
            PRINTF("input tNo(%u) is invalid \n",tNo);
            return;
        }
        break;
    }
#endif 
    MEMSET(&fltGMV,0, sizeof(fltGMV));
    MEMCPY(fltGMV.groupID,&grpid,sizeof(fltGMV.groupID));
    mac[0] = mac0;
    mac[1] = mac1;
    mac[2] = mac2;
    mac[3] = mac3;
    mac[4] = mac4;
    mac[5] = mac5;
    MEMCPY(fltGMV.mac,mac, sizeof(fltGMV.mac));
    fltGMV.vid = vlan;
    if ( opt )
    {
        ret = vsiProfile_addflt(vidFormat,(BYTE*)vid,VDP_FLTINFO_FORMAT_MACGROUPVID,&fltGMV);
    }
    else
    {
        ret = vsiProfile_delflt(vidFormat,(BYTE*)vid,VDP_FLTINFO_FORMAT_MACGROUPVID,&fltGMV);
    }

    if( ret == 0)
    {
        PRINTF("success\n");
    }
#endif 
}

void t_vsiProfile_init()
{
#if 0
    // Ìí¼Óvsi profile;
    BYTE vidFormat = 0;
    CHAR vid[VDPTLV_VSIID_LEN];
    u32 ipv4 = 0;

    struct tagVDP_FLT_GROUPMACVID  fltGMV;

    MEMSET(vid,0,VDPTLV_VSIID_LEN);

    // 1 
    vidFormat = VSIID_FORMAT_IPV4;
    ipv4 = evb_string_to_ipv4(VSIID_T1);
    MEMCPY(vid+VDPTLV_VSIID_LEN-EVB_IPV4_ADDR_LEN,&ipv4,sizeof(ipv4));
    vsiProfile_addvsiID(vidFormat,(BYTE*)vid,vsiProfile_GetVsiMgrID(),vsiProfile_GetVsiTypeID(),vsiProfile_GetVsiTypeVer());

    // 1.1
    MEMSET(&fltGMV,0, sizeof(fltGMV));
    MEMCPY(fltGMV.groupID,&VSIID_T1_FLT1_GRP,sizeof(fltGMV.groupID));
    MEMCPY(fltGMV.mac,VSIID_T1_FLT1_MAC, sizeof(fltGMV.mac));
    fltGMV.vid = VSIID_T1_FLT1_VLAN;
    vsiProfile_addflt(vidFormat,(BYTE*)vid,VDP_FLTINFO_FORMAT_MACGROUPVID,&fltGMV);

    // 1.2 
    MEMSET(&fltGMV,0, sizeof(fltGMV));
    MEMCPY(fltGMV.groupID,&VSIID_T1_FLT2_GRP,sizeof(fltGMV.groupID));
    MEMCPY(fltGMV.mac,VSIID_T1_FLT2_MAC, sizeof(fltGMV.mac));
    fltGMV.vid = VSIID_T1_FLT2_VLAN;
    vsiProfile_addflt(vidFormat,(BYTE*)vid,VDP_FLTINFO_FORMAT_MACGROUPVID,&fltGMV);
#endif 
    // 2 
#if 0 
    vidFormat = VSIID_FORMAT_IPV4;
    sprintf(vid,"%u",456);
    vsiProfile_addvsiID(vidFormat,(BYTE*)vid);
#endif 
}

void t_vdp_init(u32 port)
{
    vdp_port_init(port);
}

void t_vdp_init_S(u32 port)
{
    t_cfg_SB(EVBSYS_TYPE_STATION);
    t_ecp_init(port);
    t_vdp_init(port);
}

void t_vdp_init_B(u32 port)
{
    t_cfg_SB(EVBSYS_TYPE_BRIDGE);
    t_ecp_init(port);
    t_vdp_init(port);
}

void t_vsi_preAss(u32 port,BYTE vidFormat,u32 tNo )
{
#if 0
    BYTE vid[VDPTLV_VSIID_LEN];
    u32 ipv4 = 0;
    MEMSET(vid, 0, VDPTLV_VSIID_LEN);

    if(!vsiid_format_values_valid(vidFormat))
    {
        PRINTF("input vidFormat(%u) is invalid \n",vidFormat);
        return;
    }
    switch(tNo)
    {
    case 1:
        {
            ipv4 = evb_string_to_ipv4(VSIID_T1);
            MEMCPY(vid+VDPTLV_VSIID_LEN-EVB_IPV4_ADDR_LEN,&ipv4,sizeof(ipv4));
        }
        break;
    case 2:
        {
            ipv4 = evb_string_to_ipv4(VSIID_T2);
            MEMCPY(vid+VDPTLV_VSIID_LEN-EVB_IPV4_ADDR_LEN,&ipv4,sizeof(ipv4));
        }
        break;
    default:
        {
            PRINTF("input tNo(%u) is invalid \n",tNo);
            return;
        }
        break;
    }

    vdp_vm_api(port,vidFormat,vid,VDP_VSISTATE_PREASSOCIATE );
#endif 
}

void t_vsi_preAssR(u32 port,BYTE vidFormat,u32 tNo)
{
#if 0
    BYTE vid[VDPTLV_VSIID_LEN];
    u32 ipv4 = 0;
    MEMSET(vid, 0, VDPTLV_VSIID_LEN);

    if(!vsiid_format_values_valid(vidFormat))
    {
        PRINTF("input vidFormat(%u) is invalid \n",vidFormat);
        return;
    }
    switch(tNo)
    {
    case 1:
        {
            ipv4 = evb_string_to_ipv4(VSIID_T1);
            MEMCPY(vid+VDPTLV_VSIID_LEN-EVB_IPV4_ADDR_LEN,&ipv4,sizeof(ipv4));
        }
        break;
    case 2:
        {
            ipv4 = evb_string_to_ipv4(VSIID_T2);
            MEMCPY(vid+VDPTLV_VSIID_LEN-EVB_IPV4_ADDR_LEN,&ipv4,sizeof(ipv4));
        }
        break;
    default:
        {
            PRINTF("input tNo(%u) is invalid \n",tNo);
            return;
        }
        break;
    }
    vdp_vm_api(port,vidFormat,vid,VDP_VSISTATE_PREASSOCIATE_RR );
#endif 
}

void t_vsi_Ass(u32 port,BYTE vidFormat,u32 tNo)
{
#if 0
    BYTE vid[VDPTLV_VSIID_LEN];
    u32 ipv4 = 0;
    MEMSET(vid, 0, VDPTLV_VSIID_LEN);

    if(!vsiid_format_values_valid(vidFormat))
    {
        PRINTF("input vidFormat(%u) is invalid \n",vidFormat);
        return;
    }
    switch(tNo)
    {
    case 1:
        {
            ipv4 = evb_string_to_ipv4(VSIID_T1);
            MEMCPY(vid+VDPTLV_VSIID_LEN-EVB_IPV4_ADDR_LEN,&ipv4,sizeof(ipv4));
        }
        break;
    case 2:
        {
            ipv4 = evb_string_to_ipv4(VSIID_T2);
            MEMCPY(vid+VDPTLV_VSIID_LEN-EVB_IPV4_ADDR_LEN,&ipv4,sizeof(ipv4));
        }
        break;
    default:
        {
            PRINTF("input tNo(%u) is invalid \n",tNo);
            return;
        }
        break;
    }
    vdp_vm_api(port,vidFormat,vid,VDP_VSISTATE_ASSOCIATE );
#endif 
}

void t_vsi_DeAss(u32 port,BYTE vidFormat,u32 tNo)
{
#if 0
    BYTE vid[VDPTLV_VSIID_LEN];
    u32 ipv4 = 0;
    MEMSET(vid, 0, VDPTLV_VSIID_LEN);

    if(!vsiid_format_values_valid(vidFormat))
    {
        PRINTF("input vidFormat(%u) is invalid \n",vidFormat);
        return;
    }
    switch(tNo)
    {
    case 1:
        {
            ipv4 = evb_string_to_ipv4(VSIID_T1);
            MEMCPY(vid+VDPTLV_VSIID_LEN-EVB_IPV4_ADDR_LEN,&ipv4,sizeof(ipv4));
        }
        break;
    case 2:
        {
            ipv4 = evb_string_to_ipv4(VSIID_T2);
            MEMCPY(vid+VDPTLV_VSIID_LEN-EVB_IPV4_ADDR_LEN,&ipv4,sizeof(ipv4));
        }
        break;
    default:
        {
            PRINTF("input tNo(%u) is invalid \n",tNo);
            return;
        }
        break;
    }
    vdp_vm_api(port,vidFormat,vid,VDP_VSISTATE_DEASSOCIATE );
#endif 
}

void t_vdp_mem()
{
#if 0
    //u32 ret = evbcalc_vdpSRespWaitDelay(20,3,14);
    //PRINTF("%u \n",ret);

    struct tagEvbCfgSbp* sbp = NULL;

    struct tagEvbCfgUrp* urp = NULL;
    u32 b_rswaitDelay = 0;
    //u32 b_toutkeepalive = 0;
    //u32 s_respwait = 0;
    //u32 s_initkeepalive = 0;

    sbp = evbcfg_getSbp(1);
    if( NULL == sbp)
    {
        EVB_LOG(EVB_LOG_ERROR, "initDftVsiB: alloc memory failed.\n");
        return;
    }

    b_rswaitDelay = evbcalc_vdpBRourceWaitDelay(sbp->negRWD);

    PRINTF("Bridge %u %u\n",evbcalc_vdpBRourceWaitDelay(sbp->negRWD),evbcalc_vdpBtoutKeepAlive(sbp->negRKA,sbp->negR,sbp->cfgRTE));
    
    
    urp = evbcfg_getUrp(1);
    if( NULL == urp)
    {
        EVB_LOG(EVB_LOG_ERROR, "initDftVsiS: alloc memory failed.\n");
        return;
    }

    PRINTF("station %u %u\n",evbcalc_vdpSRespWaitDelay(urp->negRWD,urp->negR,urp->negRTE),evbcalc_vdpSReinitKeepAlive(urp->negRTE));
#endif
}

INT32 vdpfltinfo_PsPcpVid_htons(UINT16 ps,UINT16 pcp,UINT16 vid, UINT16 *out);
INT32 vdpfltinfo_PsPcpVid_ntohs(UINT16 ps,UINT16 pcp,UINT16 vid, UINT16 *out);
void t_byteorder()
{
    u16 old = 10 ;
    u16 net = 0 ;
    u16 local = 0 ;
    u16 fltOut = 0;
    struct tagVDP_FLT_VID  vid;

    struct tagVDP_FLT_GROUPMACVID gmv;
    PRINTF("local byteorder : %u\n",evb_local_is_bigendian());
    
    MEMSET(&gmv,0,sizeof(gmv));

    gmv.vid = 10;
    PRINTF("local input: vid:%u,",gmv.vid);
    ecp_mem_printf((u8*)&gmv, sizeof(gmv));
    
    vdpfltGROUPMACVID_htons(&gmv);
    PRINTF("htons output: vid:%u,",gmv.vid);
    ecp_mem_printf((u8*)&gmv, sizeof(gmv));

    vdpfltGROUPMACVID_ntohs(&gmv);
    PRINTF("ntohs output: vid:%u,",gmv.vid);
    ecp_mem_printf((u8*)&gmv, sizeof(gmv));

    PRINTF("NTOHS----TEST\n");
    net = EVB_HTONS(old);
    PRINTF("input     : %u ", old);
    ecp_mem_printf((u8*)&old, sizeof(old));
    PRINTF("EVB_HTONS : %u ", net);
    ecp_mem_printf((u8*)&net, sizeof(net));
    local = EVB_NTOHS(net);
    PRINTF("EVB_NTOHS : %u ", local);
    ecp_mem_printf((u8*)&local, sizeof(local));

    PRINTF("vdpfltinfo_PsPcpVid_htons----TEST\n");
    PRINTF("input          : %u \n",10);
    vdpfltinfo_PsPcpVid_htons(0,0,10,&fltOut);
    PRINTF("PsPcpVid_htons : %u ", fltOut);	
    ecp_mem_printf((u8*)&fltOut, sizeof(fltOut));
    vdpfltinfo_PsPcpVid_ntohs(0,0,fltOut,&fltOut);
    PRINTF("PsPcpVid_htons : %u ", fltOut);	
    ecp_mem_printf((u8*)&fltOut, sizeof(fltOut));

    PRINTF("\n");
    memset(&vid,0,sizeof(vid));
    PRINTF("vdpfltinfo_PsPcpVid_htons----vid\n");
    PRINTF("input          : %u \n",10);
    vdpfltinfo_PsPcpVid_htons(0,0,10,&fltOut);
    MEMCPY(&vid,&fltOut,sizeof(vid));
    PRINTF("PsPcpVid_htons : %u ", vid.vid);	
    ecp_mem_printf((u8*)&vid, sizeof(vid));
    vdpfltinfo_PsPcpVid_ntohs(vid.pcp,vid.ps,vid.vid,&fltOut);
    MEMCPY(&vid,&fltOut,sizeof(vid));
    PRINTF("PsPcpVid_ntohs : %u ", vid.vid);	
    ecp_mem_printf((u8*)&vid, sizeof(fltOut));

}


/*add cmd pool*/
BYTE VSIIDFORMAT = 0x5;
BYTE TYPEID_TYPE = 0x1;
BYTE VSIID_NO_1[VDPTLV_VSIID_LEN]={0x4f,0x49,0xf3,0x83,0x5c,0x07,0x48,0xb5,0xb6,0x8e,0xfe,0xcc,0x31,0x33,0xa4,0x27};
BYTE MGRID_NO_1[VDPTLV_VSIMGRID_LEN]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01};
BYTE TYPEID_NO_1[VDPTLV_VSITYPEID_LEN]= {0x00,0x00,0x01};
BYTE VSIID_MAC_1[EVB_ECP_MAC_LEN]= {0x00,0x02,0x03,0x04,0x05,0x06};

void t_newCmd(u32 port, u32 tVidNo, u16 vlan, BYTE gid,u32 tMacNo,BYTE oper)
{
    /*
     1 newCmdpool 
     2 vsiprofile 
     3 vdp_vm
    */
    BYTE vsiid[VDPTLV_VSIID_LEN] = {0};
    BYTE mac[EVB_ECP_MAC_LEN] ={0};
    BYTE mgrid[VDPTLV_VSIMGRID_LEN] = {0};
    BYTE typeID[VDPTLV_VSITYPEID_LEN]= {0};
    BYTE groupid[EVB_VDP_GROUPID_LEN] = {0};
    groupid[EVB_VDP_GROUPID_LEN-1]=gid;

    switch(tVidNo)
    {
    case 1:
        {
            memcpy(vsiid,VSIID_NO_1,VDPTLV_VSIID_LEN);
            memcpy(mgrid,MGRID_NO_1,VDPTLV_VSIMGRID_LEN);
            memcpy(typeID,TYPEID_NO_1,VDPTLV_VSITYPEID_LEN);
            memcpy(mac,VSIID_MAC_1,EVB_ECP_MAC_LEN);
        }
        break;
    default:
        {
            PRINTF("vidNo(%u) is invalid\n",tVidNo);
            return;
        }
        break;
    }

    if(oper > VDP_VSISTATE_DEASSOCIATE )
    {
        PRINTF("oper(%u) is invalid\n",oper);
        return;
    }

    // 
    if( vsiCmd_pool_add(port,VSIIDFORMAT,vsiid,oper) )
    {
        PRINTF("vsiCmd_pool_add failed.\n");
        return;
    }

    if( vsiProfile_addvsivlanmac(VSIIDFORMAT,vsiid,mgrid,typeID,TYPEID_TYPE,groupid,vlan,mac) )
    {
        PRINTF("vsiProfile_addvsivlanmac failed.\n");
        return;
    }

    // vdp_vm_api(port,VSIIDFORMAT,vsiid,oper );

}
#ifdef __cplusplus
}
#endif 

