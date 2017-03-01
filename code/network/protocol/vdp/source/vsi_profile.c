
#include "vsi_profile.h"
#include "vdp.h"

#ifdef __cplusplus
extern "C"{
#endif 

struct tagVsiProfile_head g_lstVsiProfile;

void vsiProfile_init()
{
    LIST_INIT(&g_lstVsiProfile);	
}

void vsiProfile_shutdown()
{
    struct tagVsiProfile * pProfile = NULL; 
    struct tagVsiGMVInfo *pGmv = NULL;
    while(NULL != (pProfile = LIST_FIRST(&g_lstVsiProfile)))
    {
        LIST_REMOVE(pProfile,lstEntry);

        while(NULL != (pGmv = LIST_FIRST(&pProfile->lstFlt) ))
        {
            LIST_REMOVE(pGmv,lstEntry);
            GLUE_FREE(pGmv);
        }

        GLUE_FREE(pProfile);
        pProfile = NULL;
    }
    LIST_INIT(&g_lstVsiProfile);
}

void static vsiProfile_vsiDftValue(struct tagVsiProfile * vsi)
{
    if(NULL == vsi)
    {
        return;
    }
/*    
    MEMCPY(vsi->vsiMgrID,vsiProfile_GetVsiMgrID(),VDPTLV_VSIMGRID_LEN);
    MEMCPY(vsi->vsitypeID,vsiProfile_GetVsiTypeID(),VDPTLV_VSITYPEID_LEN);
    vsi->vsitypeVer = vsiProfile_GetVsiTypeVer();
*/
    vsi->fltFormat = 0;
    vsi->fltNumber = 0;
    LIST_INIT(&vsi->lstFlt);
}

int32 vsiProfile_addvsiID(BYTE vsiidFormat,BYTE vsiID[VDPTLV_VSIID_LEN],BYTE mgrid[VDPTLV_VSIMGRID_LEN],BYTE typeID[VDPTLV_VSIMGRID_LEN],BYTE typeVer)
{
    struct tagVsiProfile * pVsi = NULL;
    struct tagVsiProfile * pVsiNew = NULL;

    // 1 check 是否已存在;
    if( !vsiid_format_values_valid(vsiidFormat) )
    {
        EVB_LOG(EVB_LOG_ERROR, "vsiProfile_addvsiID: (vsiid:%s) vsiid format(%u) is invalid.\n",GetVsiidstr(vsiID),vsiidFormat);
        return -1;
    }

    LIST_FOREACH(pVsi,&g_lstVsiProfile,lstEntry)
    {
        if( (pVsi->vsiIDFormat == vsiidFormat) && \
            (0 == MEMCMP(pVsi->vsiID,vsiID,VDPTLV_VSIID_LEN)) )
        {
            // EVB_LOG(EVB_LOG_ERROR, "vsiProfile_addvsiID: has existed.\n");
            return 0;
        }
    }	
    
    pVsiNew = (struct tagVsiProfile *)GLUE_ALLOC(sizeof(struct tagVsiProfile));
    if( NULL == pVsiNew )
    {
        EVB_LOG(EVB_LOG_ERROR, "vsiProfile_addvsiID: (vsiid:%s) alloc memory failed.\n",GetVsiidstr(vsiID));
        return -1;
    }

    MEMSET(pVsiNew, 0, sizeof(struct tagVsiProfile));
    pVsiNew->vsiIDFormat = vsiidFormat;
    MEMCPY(pVsiNew->vsiID,vsiID,VDPTLV_VSIID_LEN);
    vsiProfile_vsiDftValue(pVsiNew);

    MEMCPY(pVsiNew->vsiMgrID,mgrid,VDPTLV_VSIMGRID_LEN);
    MEMCPY(pVsiNew->vsitypeID,typeID,VDPTLV_VSITYPEID_LEN);
    pVsiNew->vsitypeVer = typeVer;

    // insert list
    if(NULL == pVsi)
    {
        LIST_INSERT_HEAD(&g_lstVsiProfile, pVsiNew, lstEntry);
    }
    else
    {
        LIST_INSERT_AFTER(pVsi,pVsiNew,lstEntry);
    }

    // notify vdp 
    vsiProfile_chg_notify_vdp(vsiidFormat, vsiID);
    return 0;
}

int32 vsiProfile_delvsiID(BYTE vsiidFormat,BYTE vsiID[VDPTLV_VSIID_LEN])
{
    struct tagVsiProfile * pVsi = NULL;
    struct tagVsiGMVInfo *pGmv = NULL;

    if( !vsiid_format_values_valid(vsiidFormat) )
    {
        EVB_LOG(EVB_LOG_ERROR, "vsiProfile_delvsiID: (vsiid:%s) vsiid format(%u) is invalid.\n",GetVsiidstr(vsiID),vsiidFormat);
        return -1;
    }

    LIST_FOREACH(pVsi,&g_lstVsiProfile,lstEntry)
    {
        if( (pVsi->vsiIDFormat == vsiidFormat) && \
            (0 == MEMCMP(pVsi->vsiID,vsiID,VDPTLV_VSIID_LEN)) )
        {
            LIST_REMOVE(pVsi,lstEntry);

            while(NULL != (pGmv = LIST_FIRST(&pVsi->lstFlt) ))
            {
                LIST_REMOVE(pGmv,lstEntry);
                GLUE_FREE(pGmv);
                pGmv = NULL;
            }

            GLUE_FREE(pVsi);
            break;
        }
    }	

    // notify vdp 
    vsiProfile_chg_notify_vdp(vsiidFormat, vsiID);
    return 0;
}

struct tagVsiProfile* vsiProfile_getVsi(BYTE vsiidFormat,BYTE vsiID[VDPTLV_VSIID_LEN])
{
    struct tagVsiProfile * pVsi = NULL;

    if( !vsiid_format_values_valid(vsiidFormat) )
    {
        EVB_LOG(EVB_LOG_ERROR, "vsiProfile_getVsi: (vsiid:%s) vsiid format(%u) is invalid.\n",GetVsiidstr(vsiID),vsiidFormat);
        return NULL;
    }

    LIST_FOREACH(pVsi,&g_lstVsiProfile,lstEntry)
    {
        if( (pVsi->vsiIDFormat == vsiidFormat) && \
            (0 == MEMCMP(pVsi->vsiID,vsiID,VDPTLV_VSIID_LEN)) )
        {
            return pVsi;
        }
    }		
    return NULL;
}

int32 vsiProfile_addflt(BYTE vsiidFormat,BYTE vsiID[VDPTLV_VSIID_LEN],BYTE fltFormat,struct tagVDP_FLT_GROUPMACVID* pGMVIn )
{
    struct tagVsiProfile * pVsi = NULL;
    struct tagVsiGMVInfo* pGMV = NULL;
    struct tagVsiGMVInfo* pGMVNew = NULL;
    u16 flt_num = 0;
    
    if( NULL == pGMVIn )
    {
        EVB_LOG(EVB_LOG_ERROR, "vsiProfile_addflt: (vsiid:%s) param is NULL.\n",GetVsiidstr(vsiID));
        return -1;
    }

    if( !vsiid_format_values_valid(vsiidFormat) )
    {
        EVB_LOG(EVB_LOG_ERROR, "vsiProfile_addflt: (vsiid:%s) vsiid format(%u) is invalid.\n",GetVsiidstr(vsiID),vsiidFormat);
        return -1;
    }

    pVsi = vsiProfile_getVsi(vsiidFormat,vsiID);
    if(NULL == pVsi)
    {
        EVB_LOG(EVB_LOG_ERROR, "vsiProfile_addflt: (vsiid:%s) getVsiProfile failed.\n",GetVsiidstr(vsiID));
        return -1;
    }

    if( !vdp_fltinfo_format_values_valid(fltFormat) || (pVsi->fltFormat && (pVsi->fltFormat != fltFormat) ) )
    {
        EVB_LOG(EVB_LOG_ERROR, "vsiProfile_addflt: (vsiid:%s) fltFormat (%u %u) is invalid.\n",GetVsiidstr(vsiID),fltFormat,pVsi->fltFormat);
        return -1;
    }
    
    pVsi->fltFormat = fltFormat;
    LIST_FOREACH(pGMV,&pVsi->lstFlt,lstEntry)
    {
        // 直接比较，以后考虑PSP之类的;
        if( 0 == MEMCMP(&pGMV->gmv,pGMVIn,sizeof(struct tagVDP_FLT_GROUPMACVID)) )
        {
            return 0;
        }
        flt_num++;
    }

    pGMVNew = (struct tagVsiGMVInfo *)GLUE_ALLOC(sizeof(struct tagVsiGMVInfo));
    if( NULL == pGMVNew)
    {
        EVB_LOG(EVB_LOG_ERROR, "vsiProfile_addflt:(vsiid:%s) alloc memory failed.\n",GetVsiidstr(vsiID));
        return -1;
    }
    MEMCPY(&pGMVNew->gmv,pGMVIn,sizeof(pGMVNew->gmv));

    if(NULL == pGMV)
    {
        LIST_INSERT_HEAD(&pVsi->lstFlt, pGMVNew, lstEntry);
    }
    else
    {
        LIST_INSERT_AFTER(pGMV,pGMVNew,lstEntry);
    }
    flt_num++;
    pVsi->fltNumber = flt_num;

    // notify vdp 
    vsiProfile_chg_notify_vdp(vsiidFormat, vsiID);

    return 0;
}

int32 vsiProfile_delflt(BYTE vsiidFormat,BYTE vsiID[VDPTLV_VSIID_LEN],BYTE fltFormat,struct tagVDP_FLT_GROUPMACVID* pGMVIn )
{
    struct tagVsiProfile * pVsi = NULL;
    struct tagVsiGMVInfo* pGMV = NULL;
    if( NULL == pGMVIn )
    {
        EVB_LOG(EVB_LOG_ERROR, "vsiProfile_addflt: (vsiid:%s) param is NULL.\n",GetVsiidstr(vsiID));
        return -1;
    }

    if( !vsiid_format_values_valid(vsiidFormat) )
    {
        EVB_LOG(EVB_LOG_ERROR, "vsiProfile_delflt: (vsiid:%s) vsiid format(%u) is invalid.\n",GetVsiidstr(vsiID),vsiidFormat);
        return -1;
    }

    pVsi = vsiProfile_getVsi(vsiidFormat,vsiID);
    if(NULL == pVsi)
    {
        EVB_LOG(EVB_LOG_ERROR, "vsiProfile_addflt: (vsiid:%s) getVsiProfile failed.\n",GetVsiidstr(vsiID));
        return -1;
    }

    if( !vdp_fltinfo_format_values_valid(fltFormat) || (pVsi->fltFormat && (pVsi->fltFormat != fltFormat) ) )
    {
        EVB_LOG(EVB_LOG_ERROR, "vsiProfile_addflt: (vsiid:%s) fltFormat (%u %u) is invalid.\n",GetVsiidstr(vsiID),fltFormat,pVsi->fltFormat);
        return -1;
    }

    LIST_FOREACH(pGMV,&pVsi->lstFlt,lstEntry)
    {
        // 直接比较，以后考虑PSP之类的;
        if( 0 == MEMCMP(&pGMV->gmv,pGMVIn,sizeof(struct tagVDP_FLT_GROUPMACVID)) )
        {
            if(pVsi->fltNumber > 0)
            {
                pVsi->fltNumber--;
            }

            LIST_REMOVE(pGMV,lstEntry);
            GLUE_FREE(pGMV);

            // notify vdp 
            vsiProfile_chg_notify_vdp(vsiidFormat, vsiID);

            return 0;
        }
    }
    
    return 0;
}

/* 此接口目前仅支持vsi 携带一个vlan mac; */
int32 vsiProfile_addvsivlanmac(BYTE vsiidFormat,BYTE vsiID[VDPTLV_VSIID_LEN],BYTE mgrid[VDPTLV_VSIMGRID_LEN],BYTE typeID[VDPTLV_VSIMGRID_LEN],BYTE typeVer,BYTE groupid[EVB_VDP_GROUPID_LEN],u16 vlanid,BYTE mac[EVB_ECP_MAC_LEN] )
{
    struct tagVDP_FLT_GROUPMACVID flt;
    struct tagVsiGMVInfo* pGMV = NULL;
    struct tagVsiProfile * pVsi = NULL;
    if( vsiProfile_addvsiID(vsiidFormat,vsiID,mgrid,typeID,typeVer))
    {
        EVB_LOG(EVB_LOG_ERROR, "vsiProfile_addvsivlanmac: (vsiid:%s) vsiProfile_addvsiID failed.\n",GetVsiidstr(vsiID));
        return -1;
    }

    memset(&flt,0,sizeof(flt));
    MEMCPY(flt.groupID,groupid,EVB_VDP_GROUPID_LEN);
    MEMCPY(flt.mac,mac,EVB_ECP_MAC_LEN);
    flt.vid = vlanid;

    /* 如果原先存在 需删除;*/
    pVsi = vsiProfile_getVsi(vsiidFormat,vsiID);
    if(NULL == pVsi)
    {
        EVB_LOG(EVB_LOG_ERROR, "vsiProfile_addflt: (vsiid:%s) getVsiProfile failed.\n",GetVsiidstr(vsiID));
        return -1;
    }
    while(NULL != (pGMV = LIST_FIRST(&pVsi->lstFlt)))
    {
        LIST_REMOVE(pGMV,lstEntry);
        GLUE_FREE(pGMV);
        pGMV = NULL;
    }

    if( vsiProfile_addflt(vsiidFormat,vsiID,VDP_FLTINFO_FORMAT_MACGROUPVID,&flt) )
    {
        EVB_LOG(EVB_LOG_ERROR, "vsiProfile_addvsivlanmac: (vsiid:%s) vsiProfile_addflt failed.\n",GetVsiidstr(vsiID));
        return -1;
    }
    return 0;
}
int32 DbgShowVsiProfile()
{
    struct tagVsiProfile * pVsi = NULL;
    struct tagVsiGMVInfo* pGMV = NULL;
    u32 vsiid_ipv4 = 0;
    u32 groupid= 0;
    PRINTF_LOOPVAR_DEFINE;
    PRINTF("========================vdp station Profile information========================\n");
    LIST_FOREACH(pVsi,&g_lstVsiProfile,lstEntry)
    {
        PRINTF("---------------------------------------------------\n");
        switch(pVsi->vsiIDFormat )
        {
        case VSIID_FORMAT_IPV4:
            {
                MEMCPY(&vsiid_ipv4,pVsi->vsiID+VDPTLV_VSIID_LEN-EVB_IPV4_ADDR_LEN,sizeof(u32));
                PRINTF("vsiID       : %s\n",evb_ipv4_to_string(vsiid_ipv4));
            }
            break;
        default:
            {
                PRINTF("vsiID       : ");
                PRINTF_VSIID(pVsi->vsiID);
            }
            break;
        }
        PRINTF("vsiID format: %u\n",pVsi->vsiIDFormat);
        PRINTF("mgrID       : %s\n",pVsi->vsiMgrID);
        PRINTF("typeID      : %s\n",pVsi->vsitypeID);
        PRINTF("typeVer     : %u\n",pVsi->vsitypeVer);
        PRINTF("flt format  : %u\n",pVsi->fltFormat);
        PRINTF("flt number  : %u\n",pVsi->fltNumber);
        PRINTF("---- pcp ps grpid vid mac\n");
        LIST_FOREACH(pGMV,&pVsi->lstFlt,lstEntry)
        {
            MEMCPY(&groupid,pGMV->gmv.groupID,sizeof(pGMV->gmv.groupID));
            PRINTF("---- %3u,%2u,%2x%2x%2x%2x,",pGMV->gmv.pcp,pGMV->gmv.ps,pGMV->gmv.groupID[0],pGMV->gmv.groupID[1],pGMV->gmv.groupID[2],pGMV->gmv.groupID[3]);
            PRINTF("%u,%x:%x:%x:%x:%x:%x\n",pGMV->gmv.vid,pGMV->gmv.mac[0],pGMV->gmv.mac[1],pGMV->gmv.mac[2],pGMV->gmv.mac[3],pGMV->gmv.mac[4],pGMV->gmv.mac[5]);
        }
    }
    return 0;
}

#ifdef __cplusplus
}
#endif 

