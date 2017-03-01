#ifdef __cplusplus
extern "C"{
#endif 

#include "evb_comm.h"
#include "vnet_pkt.h"

 /* print information about a TVNetPacket buffer
 */
void DbgShowPacket(TVNetPacket *p)
{
    u32 i,j;

    if (!p || !p->pkt_data)
        return;

    printf("TVNetPacket received on net %s, data len %d\n",
           /*p->pkt_n ? p->pkt_n->s_name :*/ "<>",
           p->pkt_datalen);

    for (i = 0; i < 8; i++)
    {
        for (j = 0;j < 16;j++)
        {
            if((i*16+j) < p->pkt_datalen)
            {
                printf("%02X ", (u8)(p->pkt_data[i*16 +j]));
            }
        }
        printf("\n");
    }
    printf("\n");
}

#ifdef __cplusplus
}
#endif 
