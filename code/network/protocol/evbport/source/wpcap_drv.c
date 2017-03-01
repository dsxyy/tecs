#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
#include "pcap.h"

#include "evb_comm.h"
#include "evb_port_drv.h"
#include "evb_port_mgr.h"
#include "wpcap_drv.h"

#pragma   comment(lib, "wpcap.lib ") 

    static TWPcapDev *s_ptWpcapDevArray = NULL;
    static u32 s_uiWpcapDevCnt = 0;

    static pcap_t *GetWpcapAdapter(u32 uiPortNo);
    static TWPcapDev *GetWpcapDev(u32 uiPortNo);

    int32 InitWpcapDev(void)
    {
        pcap_if_t *alldevs = NULL;
        pcap_if_t *d = NULL;
        pcap_t *pAdapter = NULL;
        char errbuf[PCAP_ERRBUF_SIZE+1];
        u32 i = 0;

        s_ptWpcapDevArray = NULL;
        s_uiWpcapDevCnt = 0;

        /* Retrieve the device list on the local machine */
        if (pcap_findalldevs(&alldevs, errbuf) == -1)
        {
            EVB_LOG(EVB_LOG_ERROR,"Error in pcap_findalldevs: %s\n", errbuf);
            return -1;
        }

        for(d=alldevs, i=0; d; d=d->next)
        {
            ++i;
        }

        s_uiWpcapDevCnt = i;
        s_ptWpcapDevArray = (TWPcapDev *)GLUE_ALLOC(s_uiWpcapDevCnt * sizeof(TWPcapDev));
        if(NULL == s_ptWpcapDevArray)
        {
            EVB_LOG(EVB_LOG_ERROR,"InitWpcap:NULL == s_ptWpcapDevArray\n");
            pcap_freealldevs(alldevs);
            return -1;
        }
        MEMSET(s_ptWpcapDevArray, 0x0, s_uiWpcapDevCnt * sizeof(TWPcapDev));

        for(d = alldevs, i = 0; d && i < s_uiWpcapDevCnt; d = d->next, ++i)
        {
            TWPcapDev *pWpcapDev = &s_ptWpcapDevArray[i];
            sprintf_s(pWpcapDev->strPcapDevName, WPCAP_DEV_NAME_MAX_LEN, "%s", d->name);
            sprintf_s(pWpcapDev->acPcapDevDescriptor, WPCAP_DEV_DES_NAME_MAX_LEN, "%s", d->description);

            MEMSET(errbuf, 0x0, sizeof(errbuf));
            pAdapter = pcap_open_live(pWpcapDev->strPcapDevName, 65536, 1, 1000, errbuf); 
            if (NULL == pAdapter)
            {
                EVB_LOG(EVB_LOG_ERROR,"The adapter %s open failed, err %s.\n", pWpcapDev->strPcapDevName, errbuf);
                break;
            }
            pWpcapDev->pWpcapAdapter = pAdapter;
        }    

        /* At this point, we don't need any more the device list. Free it */
        pcap_freealldevs(alldevs);

        return 0;
    }

    int32 CloseWpcapDev(void)
    {
        TWPcapDev *pWpcapDev = NULL;
        u32 i = 0;
        
        if(NULL == s_ptWpcapDevArray)
        {
            return 0;
        }

        if(0 == s_uiWpcapDevCnt)
        {
            EVB_ASSERT(0);
            EVB_LOG(EVB_LOG_ERROR,"CloseWpcapDev: Memory leak at %p.\n", s_ptWpcapDevArray);
            return -1;
        }

        for(i = 0; i < s_uiWpcapDevCnt; ++i)
        {
            pWpcapDev = &s_ptWpcapDevArray[i];
            if(pWpcapDev && pWpcapDev->pWpcapAdapter)
            {
                pcap_close((pcap_t *)pWpcapDev->pWpcapAdapter);
            }
        }

        GLUE_FREE(s_ptWpcapDevArray);
        s_ptWpcapDevArray = NULL;
        s_uiWpcapDevCnt = 0;

        return 0;
    }

    int32 WpcapInitEvbPort(u32 uiPortNo)
    {
        TWPcapDev *pWpcapDev = NULL;
        u32 i = 0;

        for(i = 0; i < s_uiWpcapDevCnt; ++i)
        {
            pWpcapDev = &s_ptWpcapDevArray[i];
            if(pWpcapDev && 0 == pWpcapDev->uiPortNo)
            {
                pWpcapDev->uiPortNo = uiPortNo;
                return 0;
            }
        }

        return -1;
    }

    int32 WpcapDestroyEvbPort(u32 uiPortNo)
    {

        return 0;
    }

    int32 WpcapSendPkt(u32 uiPortNo, u8 *pPkt, u32 dwLen)
    {
        pcap_t *pAdapter = NULL;

        if(0 == dwLen)
        {
            return 0;
        }

        pAdapter = GetWpcapAdapter(uiPortNo);
        if(NULL == pAdapter)
        {
            return -1;
        }

        if(0 != pcap_sendpacket(pAdapter, pPkt, (int)dwLen))
        {
            EVB_LOG(EVB_LOG_ERROR, "\nError sending the packet: %s\n", pcap_geterr(pAdapter));
            return -1;
        }

        return 0;
    }

    int32 WpcapRcvPkt(u32 uiPortNo, u8 **ppPkt, u32 *pdwLen)
    {
        struct pcap_pkthdr *header = NULL;
        int32 res = 0;
        pcap_t *pAdapter = NULL;

        pAdapter = GetWpcapAdapter(uiPortNo);
        if(NULL == pAdapter)
        {
            return -1;
        }

        res = pcap_next_ex(pAdapter, &header, ppPkt);
        *pdwLen = header->len;
        return res;
    }

    int32 WpcapFreePkt(u8 *ppPkt)
    {
        /* wpcap 不需要释放 pkt缓存*/
        return 0;
    }

    static pcap_t *GetWpcapAdapter(u32 uiPortNo)
    {
        u32 i = 0;
        TWPcapDev *pWpcapDev = NULL;

        if(0 == s_uiWpcapDevCnt || 0 == s_ptWpcapDevArray)
        {
            return NULL;
        }

        for(i = 0; i < s_uiWpcapDevCnt; ++i)
        {
            pWpcapDev = &s_ptWpcapDevArray[i];
            if(uiPortNo == pWpcapDev->uiPortNo)
            {
                return (pcap_t *)pWpcapDev->pWpcapAdapter;
            }
        }

        return NULL;
    }

    static TWPcapDev *GetWpcapDev(u32 uiPortNo)
    {
        u32 i = 0;

        if(0 == s_uiWpcapDevCnt || 0 == s_ptWpcapDevArray)
        {
            return NULL;
        }

        for(i = 0; i < s_uiWpcapDevCnt; ++i)
        {
            if(uiPortNo == s_ptWpcapDevArray[i].uiPortNo)
            {
                return &s_ptWpcapDevArray[i];
            }
        }

        return NULL;
    }

    TWPcapDev * GetWpcapDevArray(u32 *pOutArrSize)
    {
        if(NULL == pOutArrSize)
        {
            EVB_ASSERT(0);
            return NULL;
        }

        *pOutArrSize = s_uiWpcapDevCnt;
        return s_ptWpcapDevArray;
    }

    void  SetWpcapEvbPortNo(u32 uiPortNo, TWPcapDev * pWpcapDev)
    {
        if(NULL == pWpcapDev)
        {
            EVB_ASSERT(0);
            return ;
        }

        pWpcapDev->uiPortNo = uiPortNo;
        return ;
    }

#endif /* #endif _WIN32 */

#ifdef __cplusplus
}
#endif
