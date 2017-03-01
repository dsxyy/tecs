

#include "vdp.h"
#include "evb_port_mgr.h"
#include "lnxrsock_drv.h"

#ifdef __cplusplus
extern "C"{
#endif 
    
extern const char* vdp_b_sm_state[];
extern const char* vdp_s_sm_state[];

int vdp_dbg_print = 0;

/*packet*/
int gDbgVdpPacket = 0;
int gDbgVdpEvent = 0;
int gDbgVdpCmdPool = 0;
int gDbgVdpProfile = 0;

void DbgVdpPacket(int i)
{
    gDbgVdpPacket = i;
}

int GetDbgVdpPacket()
{
    return gDbgVdpPacket;
}

void DbgVdpEvent(int i)
{
    gDbgVdpEvent = i;
}

int GetDbgVdpEvent()
{
    return gDbgVdpEvent;
}

void DbgVdpCmdPool(int i)
{
    gDbgVdpCmdPool = i;
}

int GetDbgVdpCmdPool()
{
    return gDbgVdpCmdPool;
}

void DbgVdpProfile(int i)
{
    gDbgVdpProfile= i;
}

int GetDbgVdpProfile()
{
    return gDbgVdpProfile;
}

void DbgVdpPrintOpen()
{
    vdp_dbg_print = 1;
}

void DbgVdpPrintClose()
{
    vdp_dbg_print = 0;
}

int GetDbgVdpPrintStatus()
{
    return vdp_dbg_print;
}

void ShowVdpSInfo(u32 port)
{
    struct tagVsi_instance_s * vsi = NULL;
    struct tagEvbPort *pPort = NULL;
    PRINTF_LOOPVAR_DEFINE;
    pPort = Get1stEvbPort();

    if(NULL == pPort)
    {
        return ;
    }
    PRINTF("========================show vdp station info========================\n");
    
    for(; pPort; pPort = GetNextEvbPort(pPort))
    {
        if( 0 != port )
        {
            if( port != pPort->uiPortNo)
            {
                continue;
            }
        }
        PRINTF("port : %u\n",pPort->uiPortNo);
        if( NULL != pPort->vdp_s)
        {
            PRINTF("respWaitDelay    : %u\n",pPort->vdp_s->respWaitDelay);
            PRINTF("reinitKeepAlive  : %u\n",pPort->vdp_s->reinitKeepAlive);            
            LIST_FOREACH(vsi, &pPort->vdp_s->lstVsi,lstEntry)
            {
                PRINTF("---------------------------------------------------\n");
                PRINTF("vsiID       : ");
                PRINTF_VSIID(vsi->vsiID);
                PRINTF("vsiID format: %u\n",vsi->vsiIDFormat);
                PRINTF("vsiState    : %s\n",GetVdpTlvTypeString(vsi->vsiState));
                PRINTF("vsiState    : %d\n",vsi->vsiState);
                PRINTF("State       : %s\n",vdp_s_sm_state[vsi->state]);
                PRINTF("State       : %u\n",vsi->state);
                PRINTF("waitWhile   : %u\n",vsi->waitWhile);                
            }
        }
    }
}
void ShowVdpBInfo(u32 port)
{
    struct tagVsi_instance_b * vsi = NULL;
    struct tagEvbPort *pPort = NULL;
    PRINTF_LOOPVAR_DEFINE;
    pPort = Get1stEvbPort();

    if(NULL == pPort)
    {
        return ;
    }
    PRINTF("========================show vdp bridge info========================\n"); 

    for(; pPort; pPort = GetNextEvbPort(pPort))
    {
        if( 0 != port )
        {
            if( port != pPort->uiPortNo)
            {
                continue;
            }
        }
        PRINTF("port : %u\n",pPort->uiPortNo);
        if( NULL != pPort->vdp_b)
        {
            PRINTF("resouceWaitDelay : %u\n",pPort->vdp_b->resouceWaitDelay);
            PRINTF("toutKeepAlive    : %u\n",pPort->vdp_b->toutKeepAlive);
            LIST_FOREACH(vsi, &pPort->vdp_b->lstVsi,lstEntry)
            {
                PRINTF("---------------------------------------------------\n");
                PRINTF("vsiID       : ");
                PRINTF_VSIID(vsi->vsiID);
                PRINTF("vsiID format: %u\n",vsi->vsiIDFormat);
                PRINTF("vsiState    : %s\n",GetVdpTlvTypeString(vsi->vsiState));
                PRINTF("vsiState    : %d\n",vsi->vsiState);
                PRINTF("State       : %s\n",vdp_b_sm_state[vsi->state]);
                PRINTF("State       : %u\n",vsi->state);
                PRINTF("waitWhile   : %u\n",vsi->waitWhile);
                PRINTF("mgrid       : %u\n",vsi->vsimgrID);
            }
        }
    }
}
void ShowVdpInfo(u32 port)
{  
    u32 type = EvbcfgGetSysType();
    switch(type)
    {
    case EVBSYS_TYPE_BRIDGE:
        {
            ShowVdpBInfo(port);
        }
        break;
    case EVBSYS_TYPE_STATION:
        {
            ShowVdpSInfo(port);
        }
        break;
    default: 
        {
            PRINTF("EvbcfgGetSysType(%u) is invalid. \n",type);
        }
        break;
    }
}

/* */
void DbgShowCmdPool(u32 port)
{
    struct tagVsiCmdPoolItem* pItem = NULL; 
    PRINTF_LOOPVAR_DEFINE;
    PRINTF("========================DbgShowCmdPool========================\n");
    LIST_FOREACH(pItem,&g_lstVsiCmdPool,lstEntry)
    {
        if( 0 != port )
        {
            if( port == pItem->port)
            {
                continue;
            }
        }
        PRINTF("port : %u\n",pItem->port);
        PRINTF("---------------------------------------------------\n");
        PRINTF("vsiID       : ");
        PRINTF_VSIID(pItem->vsiID);
        PRINTF("vsiID format: %u\n",pItem->vsiIDFormat);
        PRINTF("cmd TlvType : %u\n",pItem->cmdTlvType);
        PRINTF("new Flag    : %u\n",pItem->newFlag);
    }
}

#ifdef __cplusplus
}
#endif 

