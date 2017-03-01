#if !defined(VNET_PKT_INCLUDE_H__)
#define VNET_PKT_INCLUDE_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tagVNetPacket
{
    u8 *pkt_buffer; 
    u8 *pkt_data;
    u8 *pkt_ctrl;
    u32 pkt_buflen;
    u32 pkt_datalen;
    u32 pkt_ctrllen;
    u8 pkt_pad[4];
    struct tagVNetPacket *pkt_link;
    void (*pkt_completion)(struct tagVNetPacket *); 
    void *netif; /* ������Ϊvnet�Ķ˿�ָ��; */    
}TVNetPacket; 

#ifdef __cplusplus
}
#endif

#endif
