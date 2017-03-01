#ifdef __cplusplus
extern "C"{
#endif 

#include "evb_comm.h"
#include "ecp.h"
#include "ecp_in.h"
#include "evb_port_mgr.h"

extern T_ECP gtECP; 

extern int ecp_dbg_print;
extern int ecp_dbg_packet_print;
void ecp_pack_print(int i)
{
    ecp_dbg_packet_print = i;
}

void ecp_print(int i)
{
    ecp_dbg_print =i;
}
 
void t_ecp_init(u32 port)
{
    LIST_INIT(&gtECP.ecpif_head);
    
    ecp_port_init(port); 
}

void print_ecpdu(struct ecp_ecpdu * pEcpdu)
{
    if( pEcpdu == NULL)
    {
        return;
    }

    PRINTF("version  %u\n",pEcpdu->version);
    PRINTF("operType %u\n",pEcpdu->operType);
    PRINTF("subType  %u\n",pEcpdu->subType);
    PRINTF("sequence %u\n",pEcpdu->sequence);
    PRINTF("\n");
}
void t_ecp_list_tx_insert(u32 ifIndex)
{
    struct ecp_ecpdu * pEcpdu = NULL;
    BYTE *ulp = NULL;
    DWORD32 ulp_len = 32;
    UINT16 seq = 0;
    BYTE  t_Date[32]={0};
    int i = 0;
    struct tagEvbPort* port = NULL;

    // 往端口1中加入;
    port = GetEvbPort(ifIndex);
    if( NULL == port)
    {
        PRINTF("Port not existed");
        return;
    }


    for(i=0; i<32; i++)
    {
        t_Date[i]=i;
    }

    pEcpdu = (struct  ecp_ecpdu *)GLUE_ALLOC(sizeof(struct  ecp_ecpdu)); 
    if(NULL == pEcpdu)
    {
        PRINTF("t_ecp_list_rx alloc memory failed.");
        return ;
    }   
    MEMSET(pEcpdu, 0x0, sizeof(struct ecp_ecpdu));

    // seq
    // 直接往port=1 中加入;
    // GetECPSequenceNum(g_port.NUM,&seq);
    GetECPSequenceNum(port->uiPortNo,&seq);

    pEcpdu->version = ECP_VERSION;
    pEcpdu->operType = ECP_REQUEST;
    pEcpdu->subType = ECP_SUBTYPE_VDP;
    pEcpdu->sequence = seq;
#if 0
    PRINTF("------input-------\n");
    print_ecpdu(pEcpdu);

    ecpduHead_HTON(ECP_ETHER_TYPE,ECP_VERSION,ECP_REQUEST,ECP_SUBTYPE_VDP,seq,(BYTE*)pEcpdu);
    PRINTF("------hton-------\n");
    print_ecpdu(pEcpdu);
    ecpduHead_NTOH(pEcpdu);
    PRINTF("------ntoh-------\n");
    print_ecpdu(pEcpdu);
    ecpduHead_HTON(ECP_ETHER_TYPE,ECP_VERSION,ECP_REQUEST,ECP_SUBTYPE_VDP,seq,(BYTE*)pEcpdu);
    PRINTF("------hton-------\n");
    print_ecpdu(pEcpdu);

    return;
#endif 
    ecpduHead_HTON(ECP_VERSION,ECP_REQUEST,ECP_SUBTYPE_VDP,seq,(BYTE*)pEcpdu);

    ulp = (BYTE  *)GLUE_ALLOC(ulp_len); 
    if(NULL == ulp)
    {
        PRINTF("t_ecp_list_rx alloc memory failed.");
        GLUE_FREE(pEcpdu);
        return ;
    }
        
    MEMCPY(ulp,t_Date,ulp_len);

    //pEcpdu->ulpdu = ulp;
    pEcpdu->ulpdu = ulp;

    PRINTF("pEcpdu:");
    DbgMemShow(pEcpdu, EVB_ECP_ECPDU_HEAD_LEN);
    PRINTF("pEcpdu->ulpdu:");
    DbgMemShow(pEcpdu->ulpdu, ulp_len);
    
    list_EcpULPDUtx_insert(port->uiPortNo,pEcpdu,ulp_len + EVB_ECP_ECPDU_HEAD_LEN);
    ShowEcpInfo(port->uiPortNo);
}

void t_ecp_tx_sm(int e)
{
#if 0
    struct tagEcpULPDU * pEcpUlpdu = NULL;
    LIST_FOREACH(pEcpUlpdu,&g_port.lstEcpULPDUtx, lstEntry)
    {
        ecp_tx_sm(pEcpUlpdu,e);
    }
#endif 
}



#ifdef __cplusplus
}
#endif 
