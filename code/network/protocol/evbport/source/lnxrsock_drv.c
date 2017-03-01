#ifdef __cplusplus
extern "C" {
#endif


#include "lnxrsock_drv.h"
#include "evb_port_mgr.h"

#ifdef _EVB_TECS
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <linux/if_ether.h>
#include <linux/in.h>
#include <net/if.h>
#include <netpacket/packet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/filter.h>

#include "lldp_l2pkt.h"
#include "evb_comm.h"
#include "evb_port_drv.h"
#include "evb_port_mgr.h"
#include "ecp.h"
    static struct tagLnxRSockPort_head lstLnxRSockPort;

    int32 InitLnxRsockDrv(void)
    {
        LIST_INIT(&lstLnxRSockPort);
        return 0;
    }

    int32 CloseLnxRsockDrv(void)
    {
        struct tagLnxRSockPort * port = NULL; 
        LIST_FOREACH(port,&lstLnxRSockPort,lstEntry )
        {
            if( port->nSock > 0 )
            {
                LnxRsockDestroyEvbPort(port->nSock);
            }
            LIST_REMOVE(port,lstEntry);
            GLUE_FREE(port);            
        }

        return 0;
    }

    int multicast_eth(struct tagLnxRSockPort * port) 
    { 
        struct sockaddr_ll sll; 
        struct ifreq ifstruct; 
        struct packet_mreq mr; 
        int ret; 
        int fd = 0;
    
        if( NULL == port )
        {
            EVB_LOG(EVB_LOG_ERROR, "multicast_eth:param is invalid.\n");
            return -1;
        }
        fd = port->nSock;

        if( 0 >= fd )
        {
            EVB_LOG(EVB_LOG_ERROR, "multicast_eth:port socket(%u %d) is invalid.\n",port->uiPortNo, fd);
            return -1;
        }
        
        memset( &sll, 0, sizeof(sll) ); 
        sll.sll_family = PF_PACKET; 
        sll.sll_protocol = EVB_HTONS(ETH_P_ALL); 
    
    
        //strcpy(ifstruct.ifr_name, port->strIfName); 
        strncpy(ifstruct.ifr_name,port->strIfName,sizeof(ifstruct.ifr_name));
        ret = ioctl(fd, SIOCGIFINDEX, &ifstruct); 
        if(ret != 0 )
        {
            EVB_LOG(EVB_LOG_ERROR, "multicast_eth: port(%s) ioctl(SIOCGIFINDEX) failed.\n",ifstruct.ifr_name);
            return -1;
        } 
    
        sll.sll_ifindex = ifstruct.ifr_ifindex;
        mr.mr_ifindex = ifstruct.ifr_ifindex;
    
        ret = ioctl (fd, SIOCGIFHWADDR, &ifstruct); 
        if(ret != 0 )
        {
            EVB_LOG(EVB_LOG_ERROR, "multicast_eth: port(%s) ioctl(SIOCGIFHWADDR) failed.\n",ifstruct.ifr_name);
            return -1;
        } 
        memcpy (sll.sll_addr, ifstruct.ifr_ifru.ifru_hwaddr.sa_data, sizeof(sll.sll_addr));
        
        ret = ioctl (fd, SIOCGIFFLAGS, &ifstruct); 
        if(ret != 0 )
        {
            EVB_LOG(EVB_LOG_ERROR, "multicast_eth: port(%s) ioctl(SIOCGIFFLAGS) failed.\n",ifstruct.ifr_name);
            return -1;
        } 
    
        ifstruct.ifr_flags |= (IFF_UP | IFF_BROADCAST | IFF_MULTICAST);
        // ifstruct.ifr_flags &= ~IFF_ALLMULTI;
        //
        ret = ioctl (fd, SIOCSIFFLAGS, &ifstruct);
        if(ret != 0 )
        {
            EVB_LOG(EVB_LOG_ERROR, "multicast_eth: port(%s) ioctl(SIOCSIFFLAGS) failed.\n",ifstruct.ifr_name);
            return -1;
        }
            
        ret = ioctl(fd, SIOCGIFINDEX, &ifstruct); 
        if(ret != 0 )
        {
            EVB_LOG(EVB_LOG_ERROR, "multicast_eth: port(%s) ioctl(SIOCGIFINDEX) failed.\n",ifstruct.ifr_name);
            return -1;
        } 
        memset(&mr, 0, sizeof(mr));
        mr.mr_ifindex = ifstruct.ifr_ifindex;
        mr.mr_type = PACKET_MR_MULTICAST;
        mr.mr_alen = 6; 
        memcpy(mr.mr_address, ECP_MULTICAST_MAC, 6);    
    
        ret = setsockopt(fd, SOL_PACKET, PACKET_ADD_MEMBERSHIP,&mr, sizeof(mr));
        if(ret != 0 )
        {
            EVB_LOG(EVB_LOG_ERROR, "multicast_eth: setsockopt(ecp_mult_mac) failed(%d %s).\n",errno,strerror(errno));
            return -1;
        }

        memcpy(mr.mr_address, multi_cast_mac1, 6);        
        ret = setsockopt(fd, SOL_PACKET, PACKET_ADD_MEMBERSHIP,&mr, sizeof(mr));
        if(ret != 0 )
        {
            EVB_LOG(EVB_LOG_ERROR, "multicast_eth: setsockopt(lldp_mult_mac) failed(%d %s).\n",errno,strerror(errno));
            return -1;
        }

        memcpy(mr.mr_address, multi_cast_mac2, 6);        
        ret = setsockopt(fd, SOL_PACKET, PACKET_ADD_MEMBERSHIP,&mr, sizeof(mr));
        if(ret != 0 )
        {
            EVB_LOG(EVB_LOG_ERROR, "multicast_eth: setsockopt(lldp_mult_mac) failed(%d %s).\n",errno,strerror(errno));
            return -1;
        }

        memcpy(mr.mr_address, multi_cast_mac3, 6);        
        ret = setsockopt(fd, SOL_PACKET, PACKET_ADD_MEMBERSHIP,&mr, sizeof(mr));
        if(ret != 0 )
        {
            EVB_LOG(EVB_LOG_ERROR, "multicast_eth: setsockopt(lldp_mult_mac) failed(%d %s).\n",errno,strerror(errno));
            return -1;
        }
    
        return 0; 
    }

    int32 setRawsockFilter(int fd)
    {
        int32 ethType_1 = ECP_PROTOCOL_TYPE;
        int32 ethType_2 = ETH_P_LLDP;
        struct sock_fprog  fprog;

        if (fd <= 0)
        {
            return 1;
        }

        struct sock_filter filters[] =
        {
            { 0x28, 0, 0, 0x0000000c },
            { 0x15, 1, 0, ethType_1 },
            { 0x15, 0, 1, ethType_2 },
            { 0x6, 0, 0, 0x00000640 },
            { 0x6, 0, 0, 0x00000000 },
        };
        
        fprog.len = sizeof(filters)/sizeof(struct sock_filter);
        fprog.filter = filters;
        
        if (setsockopt(fd,  SOL_SOCKET,  SO_ATTACH_FILTER, &fprog, sizeof(struct sock_fprog)) < 0)
        {
            EVB_LOG(EVB_LOG_ERROR,"setRawsockFilter set filter failed(%u %s).\n",errno, strerror(errno));
            return -1;
        }
        return 0;
    }

    static struct tagLnxRSockPort * GetLnxRSockPort(u32 uiPortNo)
    {
        BYTE bFind = FALSE;
        struct tagLnxRSockPort * port = NULL; 
        LIST_FOREACH(port,&lstLnxRSockPort,lstEntry )
        {
            if( uiPortNo == port->uiPortNo )
            {
                bFind = TRUE;
                break;
            }
        }

        if( !bFind || port == NULL )
        {
            return NULL;
        }
        return port;
    }

    int32 LnxRsockInitEvbPort(u32 uiPortNo)
    {
        int32 s;
        struct sockaddr_ll sll;
        struct ifreq ifr;
#if 0        
        int32 optval  = 1;        
        int32 ret = 0;
#endif         
        int32 optlen;

        struct tagLnxRSockPort * port = NULL;
        struct tagEvbIssPort *pEvbPort = NULL;
        
        pEvbPort = GetEvbIssPort(uiPortNo);
        if(pEvbPort)
        {
            AddLnxRsockEvbPortNo(uiPortNo, pEvbPort->acName);
        }

        port = GetLnxRSockPort(uiPortNo);

        if( port == NULL )
        {
            EVB_LOG(EVB_LOG_ERROR,"LnxRsockInitEvbPort port(%u) is invalid.\n",uiPortNo);
            return -1;
        }
        
        MEMSET(&sll, 0, sizeof(sll));
        sll.sll_family = AF_PACKET;
        sll.sll_protocol = EVB_HTONS(ETH_P_ALL);
        s = socket(sll.sll_family, SOCK_RAW, sll.sll_protocol);
        if (s <= 0)
        {
            EVB_LOG(EVB_LOG_ERROR,"LnxRsockInitEvbPort: creat socket 1  failed!\n");     
            return -1;
        }
        optlen  = sizeof(WORD32);
#if 0        /* 暂时不考虑性能 后续需要，需成研支持; */
        ret = setsockopt(s, SOL_PACKET, 249, &optval, sizeof(optval)); /*PACKET_LOP*/
        if (0!=ret)
        { 
            EVB_LOG(EVB_LOG_WARN,"LnxRsockInitEvbPort: creat setsockopt  SOL_PACKET  failed!\n");
            close(s);
            sll.sll_protocol = htons(ETH_P_ALL);
            // s = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
            s = socket(sll.sll_family, SOCK_RAW, sll.sll_protocol);
            if (s <= 0)
            {
                EVB_LOG(EVB_LOG_ERROR,"LnxRsockInitEvbPort: creat socket 2  failed!\n");      
                return -1;
            }
        }
#endif 

        MEMSET(&ifr, 0, sizeof(ifr));
        // 需考虑 pEvbPort->acName 32 匹配 ifr.ifr_name 16 问题
        strncpy(ifr.ifr_name,port->strIfName,sizeof(ifr.ifr_name));
        if (-1 == ioctl(s, SIOCGIFINDEX, &ifr))
        {
            close(s);
            EVB_LOG(EVB_LOG_ERROR,"LnxRsockInitEvbPort: ioctl  failed!\n");
            return -1;
        }
        
        sll.sll_ifindex = ifr.ifr_ifindex;

        if (0 != bind(s, (struct sockaddr *) &sll, sizeof(sll)))
        {
            close(s);     
            EVB_LOG(EVB_LOG_ERROR,"LnxRsockInitEvbPort: bind  failed!\n");
            return -1;
        }

        port->nSock = s;

        multicast_eth(port);
        setRawsockFilter(s);

        return 0;
    }

    int32 LnxRsockDestroyEvbPort(u32 uiPortNo)
    {       
        struct tagLnxRSockPort * port = NULL;
        port = GetLnxRSockPort(uiPortNo);

        if( port == NULL )
        {
            return 0;
        }

        if( port->nSock  > 0 )
        {
            close(port->nSock);
        }

        port = NULL; 
        LIST_FOREACH(port,&lstLnxRSockPort,lstEntry )
        {
              if( port->uiPortNo == uiPortNo)
              {
                     LIST_REMOVE(port,lstEntry);
                     GLUE_FREE(port);      
                     break;					 
              }
        }			

        return 0;
    }

    int32 LnxRsockSendPkt(u32 uiPortNo, u8 * pPkt, u32 dwLen)
    {
        int32 ret = 0;
        struct tagLnxRSockPort * port = NULL;
        port = GetLnxRSockPort(uiPortNo);

        if( port == NULL )
        {
            EVB_LOG(EVB_LOG_ERROR,"LnxRsockSendPkt port(%u) is invalid.\n",uiPortNo);
            return -1;
        }

        if( port->nSock <= 0 )
        {
            EVB_LOG(EVB_LOG_ERROR,"LnxRsockSendPkt socket(%u %d) is invalid.\n",uiPortNo,port->nSock);
            return -1;
        }

        ret = sendto(port->nSock,pPkt,dwLen,0,0,0);  
        if( -1 == ret )
        {
            EVB_LOG(EVB_LOG_ERROR,"LnxRawSendPkt failed(port:%u,ifname:%s socket:%u) ret %d err %s %u \n", uiPortNo,port->strIfName,port->nSock,ret, strerror(errno),errno);
			return -1;
        }

        return 0;
    }

    int32 EVBEthTypeInvalid(BYTE * buf)
    {
        if( NULL == buf)
        {
            return -1;
        }
        // mac 

        // type
        if( (ECP_PROTOCOL_TYPE == (buf[12] << 8) + buf[13]))
        {
            return 0;
        }

        if( (ETH_P_LLDP == (buf[12] << 8) + buf[13]))
        {
            return 0;
        }

        return -1;
    }

    int32 LnxRsockRcvPkt(u32 uiPortNo, u8 **ppPkt, u32 *pdwLen)
    {
        int len = 0;    
        BYTE buf[1600]={0};
        int iMode =1;/*1，非阻塞 0，阻塞*/ 

        struct tagLnxRSockPort * port = NULL;
        port = GetLnxRSockPort(uiPortNo);

        if( port == NULL )
        {
            EVB_LOG(EVB_LOG_ERROR,"LnxRsockRcvPkt port(%u) is invalid.\n",uiPortNo);
            return -1;
        }

        if( NULL == pdwLen )
        {
            EVB_LOG(EVB_LOG_ERROR,"LnxRsockRcvPkt param is invalid.\n");
            return -1;
        }
        ioctlsocket(port->nSock,FIONBIO, &iMode);

        len = recvfrom(port->nSock, buf, 1600, 0, 0, 0);
        if( len > 0)
        {
            //printf("%x:%x:%x:%x:%x:%x, \n ",buf[0],buf[1],buf[2],buf[3],buf[4],buf[5]);
            
            if(0 == EVBEthTypeInvalid(buf))
            {
                (*ppPkt ) = (u8 *)GLUE_ALLOC(len);
                if( *ppPkt == NULL )
                {
                    EVB_LOG(EVB_LOG_ERROR,"LnxRawRcvPkt Glue_alloc(%u) failed.\n", len);
                    return -1;
                }

                // printf("LnxRawRcvPkt success len 1: %u \n",len);
                memcpy((*ppPkt), buf, len);
            }
            else
            {
                return 0;
            }
        }

        *pdwLen = len;

        return len;
    }

    int32 LnxRsockFreePkt(u8 *pPkt)
    {
        /* lnxRsock 需要释放内存 */
        if(pPkt )
        {
            GLUE_FREE(pPkt); 
            pPkt = NULL;
        }
        return 0;
    }


    int32 AddLnxRsockEvbPortNo(u32 uiPort,char * acIfName)
    {
        struct tagLnxRSockPort * port = NULL; 
        struct tagLnxRSockPort * pNewPort = NULL; 

        if( NULL == acIfName)
        {
            EVB_LOG(EVB_LOG_ERROR,"AddLnxRsockEvbPortNo Param is invalid.\n");
            return -1;
        }

        pNewPort = (struct tagLnxRSockPort *)GLUE_ALLOC(sizeof(struct tagLnxRSockPort));
        if( NULL == pNewPort)
        {
            EVB_LOG(EVB_LOG_ERROR,"AddLnxRsockEvbPortNo alloc memory failed.\n");
            return -1;
        }

        MEMSET(pNewPort, 0, sizeof(struct tagLnxRSockPort));
        pNewPort->uiPortNo = uiPort;
        strcpy(pNewPort->strIfName,acIfName);

        LIST_FOREACH(port,&lstLnxRSockPort,lstEntry )
        {
        }

        if(NULL == port)
        {
            LIST_INSERT_HEAD(&lstLnxRSockPort, pNewPort, lstEntry);
        }
        else
        {
            LIST_INSERT_AFTER(port,pNewPort,lstEntry);
        }

        return 0;
    }

    int32 DelLnxRsockEvbPortNo(u32 uiPort)
    {
        struct tagLnxRSockPort * port = NULL; 
        
        LIST_FOREACH(port,&lstLnxRSockPort,lstEntry )
        {
            if( uiPort == port->uiPortNo )
            {
                LIST_REMOVE(port,lstEntry);
                GLUE_FREE(port);
            }
        }
        
        return 0;
    }

    void DbgShowEvbRawSocket(u32 uiPort)
    {
        struct tagLnxRSockPort * port = NULL; 

        PRINTF("---------------------------------------------------------\n");
        LIST_FOREACH(port,&lstLnxRSockPort,lstEntry )
        {
            if( 0 != uiPort )
            {
                if( uiPort != port->uiPortNo)
                {
                    continue;
                }
            }
            PRINTF("port:%u,ifName:%s,socket:%u\n",port->uiPortNo,port->strIfName,port->nSock);
        }
    }
    
#endif /* #endif _EVB_TECS */

#ifdef __cplusplus
}
#endif

