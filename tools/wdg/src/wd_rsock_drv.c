#ifdef __cplusplus
extern "C" {
#endif

#include "wd_rsock_drv.h"

#include <string.h>
#include <memory.h>
#include <stdlib.h>
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
#include <fcntl.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/time.h>
#include <dirent.h>
#include <linux/hdreg.h> 
#include <scsi/sg.h>

#define PRINTF printf
#define WD_LOG(nLvl, fmt, arg...)\
    do\
    {\
    if(g_printflag)\
    {PRINTF(fmt, ##arg);}\
    }while(0)	
	
    static struct tagLnxRSockPort * g_ptwdport = NULL;
    static unsigned char g_srcMac[6] = {0};
    static int g_printflag = 0;

    void wd_printflag(int flag)
    {
        g_printflag = flag;
        return;
    }

    void wd_printinfo(void)
    {
        printf("\n %d %d %d %d %d %d", g_srcMac[0],g_srcMac[1],g_srcMac[2],g_srcMac[3],g_srcMac[4],g_srcMac[5]);
        printf("\n %x %x %x %x %x %x", g_srcMac[0],g_srcMac[1],g_srcMac[2],g_srcMac[3],g_srcMac[4],g_srcMac[5]);
        printf("\n %s", g_ptwdport->strIfName);
        printf("\n this version is completed in 2012-09-21");
        return;
    }

    int local_is_bigendian()
    {
        typedef union
        {
            char c[4];
            int i;
        } testEndian;

        testEndian _t;
        _t.c[0]=1;
        _t.c[1]=0;
        _t.c[2]=0;
        _t.c[3]=0;

        // little endian
        if ( _t.i == 1)
        {
            return 0;
        }

        return 1;
    }

    unsigned short htons(unsigned short x)
    {
        if (local_is_bigendian())
        {
            return x;
        }

        return (unsigned short)( \
                                 ((((unsigned short)(x)) & 0x00ff) << 8) | \
                                 ((((unsigned short)(x)) & 0xff00) >> 8));
    }

    unsigned int  htonl(unsigned int x)
    {
        if (local_is_bigendian())
        {
            return x;
        }

        return (unsigned int)( \
                               ((((unsigned int)(x)) & 0x000000ff) << 24) | \
                               ((((unsigned int)(x)) & 0x0000ff00) <<  8) | \
                               ((((unsigned int)(x)) & 0x00ff0000) >>  8) | \
                               ((((unsigned int)(x)) & 0xff000000) >> 24));
    }

    static long FileSearch(FILE* pFile, const char* lpszSearchString)
    {
        unsigned long ulFileSize=0;

        if ((!pFile)||(!lpszSearchString))
        {
            return -1;
        }

        fseek(pFile,0,SEEK_END);

        ulFileSize=ftell(pFile);

        fseek(pFile,0,SEEK_SET);

        if (!ulFileSize)
        {
            return -1;
        }

        unsigned long ulBufferSize=strlen(lpszSearchString);

        if (ulBufferSize>ulFileSize)
        {
            return -1;
        }

        char* lpBuffer=(char*)malloc(ulBufferSize);
        if (!lpBuffer)
        {
            return -1;
        }

        unsigned long ulCurrentPosition=0;
        while (ulCurrentPosition<ulFileSize-ulBufferSize)
        {
            fseek(pFile,ulCurrentPosition,SEEK_SET);
            fread(lpBuffer,1,ulBufferSize,pFile);

            if (!memcmp(lpBuffer,lpszSearchString,ulBufferSize))
            {
                free(lpBuffer);
                return ulCurrentPosition;
            }

            ulCurrentPosition++;
        }

        free(lpBuffer);
        return -1;
    }

    static int tecs_wdsocket_create(void)
    {
        char up[252] = {0};
        unsigned char trans[2] = {0};
        FILE *in = NULL;
        char match0[]="00:00:01";
        char match1[]="0:0:1";

        long ret = 0;
        char mac[3] ={0};

        int i = 0;
        int j = 0;
        int offset = 0;
        int tmp = 0;

        struct dirent *entry = NULL;
        DIR *dp = NULL;

        dp = opendir("/sys/class/net");
        if (NULL == dp) 
        {
            WD_LOG(0, "open dir failed for net.\n");
            return -1;
        }

        while ((entry = readdir(dp)))
        {
            WD_LOG(0, "entry d_type is %d.\n", entry->d_type); 

            strcpy(up, "/sys/class/net/");
            strcpy(up + strlen(up), entry->d_name);
            strcpy(up + strlen(up), "/address");

            in = fopen(up,"r");
            if(!in)
            {
                WD_LOG(0, "fopen %s  failed.\n", entry->d_name);
                continue;
            }
#if 0
            in = fopen("/dev/shm/wdg.txt","r");
#endif
            ret = FileSearch(in, match0);
            if(ret != 0)
            {
                ret = FileSearch(in, match1);
                if(ret != 0)
                {
                    WD_LOG(0, "fileserach failed.\n");
                    fclose(in);
                    continue;
                }
            }

            g_ptwdport = (struct tagLnxRSockPort *)malloc(sizeof(struct tagLnxRSockPort));
            if(NULL == g_ptwdport)
            {
                fclose(in);
                closedir(dp);
                WD_LOG(0, "g_ptwdport is null.\n");
                return -1;
            }

            //保存源MAC
            for(j = 0; j<6; j++)
            {
                fseek(in, ret + offset, SEEK_SET);
                fread(mac, 1, 2, in);	
                tmp = 3;

                for(i=0; i<2; i++)
                {
                    switch (*(mac+i))
                    {
                        case '0': trans[i] = 0x0; break;
                        case '1': trans[i] = 0x1; break;
                        case '2': trans[i] = 0x2; break;
                        case '3': trans[i] = 0x3; break;
                        case '4': trans[i] = 0x4; break;
                        case '5': trans[i] = 0x5; break;
                        case '6': trans[i] = 0x6; break;
                        case '7': trans[i] = 0x7; break;
                        case '8': trans[i] = 0x8; break;
                        case '9': trans[i] = 0x9; break;
					
                        case 'a': 
                        case 'A': 
                             trans[i] = 0xA; break;
                        case 'b': 
                        case 'B': 
                             trans[i] = 0xB;break;
                        case 'c': 
                        case 'C': 
                             trans[i] = 0xC; break;
                        case 'd': 
                        case 'D': 
                             trans[i] = 0xD; break;
                        case 'e': 
                        case 'E': 
                             trans[i] = 0xE; break;
                        case 'f': 
                        case 'F': 
                             trans[i] = 0xF; break;
                        case ':':
                        case '-':
                        default:
                        {
                             if(1 == i)
                             {
                                 trans[1] = trans[0];
                                 trans[0] = 0;
                                 tmp = 2;
                             }
                             else
                             {
                                 WD_LOG(0, "the first char is : or -: %d.\n", i);
                                 fclose(in);
                                 closedir(dp);
                                 if(NULL != g_ptwdport)
                                 {
                                     free(g_ptwdport);
                                     g_ptwdport = NULL; 
                                 }                                
                                 return -1;
                             }
                             break; 
                        }
                    }    
                }	

                g_srcMac[j] = ((trans[0]<<4) | (trans[1]));
                offset = offset + tmp;
            }

            //初始化socketport
            memset(g_ptwdport, 0, sizeof(struct tagLnxRSockPort));
            //portno暂且先不管
            g_ptwdport->uiPortNo = 0;
            strcpy(g_ptwdport->strIfName, entry->d_name);
            strcpy(up, "ifconfig ");
            strcpy(up + strlen(up), g_ptwdport->strIfName);
            strcpy(up + strlen(up), " up");
            system(up);
            fclose(in);
            closedir(dp);

            WD_LOG(0, "this is a wdg port %s.\n", g_ptwdport->strIfName);
            return 0;
            break;
        }

        WD_LOG(0, "readdir loop  can't  find wdg port.\n");
        closedir(dp);
        return -1;
    }

    int wd_multicast_eth(struct tagLnxRSockPort * port)
    {
        struct sockaddr_ll sll;
        struct ifreq ifstruct;
        struct packet_mreq mr;
        int ret;
        int fd = 0;

        if ( NULL == port )
        {
            WD_LOG(0, "multicast_eth:param is invalid.\n");
            return -1;
        }
        fd = port->nSock;

        if ( 0 >= fd )
        {
            WD_LOG(0, "multicast_eth:port socket(%u %d) is invalid.\n",port->uiPortNo, fd);
            return -1;
        }

        memset( &sll, 0, sizeof(sll) );
        sll.sll_family = PF_PACKET;
        sll.sll_protocol = htons(0x0003);

        strncpy(ifstruct.ifr_name,port->strIfName,sizeof(ifstruct.ifr_name));
        ret = ioctl(fd, SIOCGIFINDEX, &ifstruct);
        if (ret != 0 )
        {
            WD_LOG(0, "multicast_eth: port(%s) ioctl(SIOCGIFINDEX) failed.\n",ifstruct.ifr_name);
            return -1;
        }

        sll.sll_ifindex = ifstruct.ifr_ifindex;
        mr.mr_ifindex = ifstruct.ifr_ifindex;

        ret = ioctl (fd, SIOCGIFHWADDR, &ifstruct);
        if (ret != 0 )
        {
            WD_LOG(0, "multicast_eth: port(%s) ioctl(SIOCGIFHWADDR) failed.\n",ifstruct.ifr_name);
            return -1;
        }
        memcpy (sll.sll_addr, ifstruct.ifr_ifru.ifru_hwaddr.sa_data, sizeof(sll.sll_addr));

        ret = ioctl (fd, SIOCGIFFLAGS, &ifstruct);
        if (ret != 0 )
        {
            WD_LOG(0, "multicast_eth: port(%s) ioctl(SIOCGIFFLAGS) failed.\n",ifstruct.ifr_name);
            return -1;
        }

        ifstruct.ifr_flags |= (IFF_UP | IFF_BROADCAST | IFF_MULTICAST |IFF_PROMISC);

        ret = ioctl (fd, SIOCSIFFLAGS, &ifstruct);
        if (ret != 0 )
        {
            WD_LOG(0, "multicast_eth: port(%s) ioctl(SIOCSIFFLAGS) failed.\n",ifstruct.ifr_name);
            return -1;
        }

        ret = ioctl(fd, SIOCGIFINDEX, &ifstruct);
        if (ret != 0 )
        {
            WD_LOG(0, "multicast_eth: port(%s) ioctl(SIOCGIFINDEX) failed.\n",ifstruct.ifr_name);
            return -1;
        }

#if 0
        memset(&mr, 0, sizeof(mr));
        mr.mr_ifindex = ifstruct.ifr_ifindex;
        mr.mr_type = PACKET_MR_MULTICAST;
        mr.mr_alen = 6;
        memcpy(mr.mr_address, ECP_MULTICAST_MAC, 6);

        ret = setsockopt(fd, SOL_PACKET, PACKET_ADD_MEMBERSHIP,&mr, sizeof(mr));
        if (ret != 0 )
        {
            //EVB_LOG(EVB_LOG_ERROR, "multicast_eth: setsockopt(ecp_mult_mac) failed(%d %s).\n",errno,strerror(errno));
            return -1;
        }

        memcpy(mr.mr_address, multi_cast_mac1, 6);
        ret = setsockopt(fd, SOL_PACKET, PACKET_ADD_MEMBERSHIP,&mr, sizeof(mr));
        if (ret != 0 )
        {
            //EVB_LOG(EVB_LOG_ERROR, "multicast_eth: setsockopt(lldp_mult_mac) failed(%d %s).\n",errno,strerror(errno));
            return -1;
        }

        memcpy(mr.mr_address, multi_cast_mac2, 6);
        ret = setsockopt(fd, SOL_PACKET, PACKET_ADD_MEMBERSHIP,&mr, sizeof(mr));
        if (ret != 0 )
        {
            //EVB_LOG(EVB_LOG_ERROR, "multicast_eth: setsockopt(lldp_mult_mac) failed(%d %s).\n",errno,strerror(errno));
            return -1;
        }

        memcpy(mr.mr_address, multi_cast_mac3, 6);
        ret = setsockopt(fd, SOL_PACKET, PACKET_ADD_MEMBERSHIP,&mr, sizeof(mr));
        if (ret != 0 )
        {
            //EVB_LOG(EVB_LOG_ERROR, "multicast_eth: setsockopt(lldp_mult_mac) failed(%d %s).\n",errno,strerror(errno));
            return -1;
        }
#endif

        return 0;
    }

    int wd_setRawsockFilter(int fd)
    {
        int ethType_1 = 0x0101/*0x0800*/;
        int ethType_2 = 0x88cc;
        struct sock_fprog  fprog;

        if (fd <= 0)
        {
            return 1;
        }

        struct sock_filter filters[] =
        {
            { 0x28, 0, 0, 0x0000000c},
            { 0x15, 1, 0, ethType_1 },
            { 0x15, 0, 1, ethType_2 },
            { 0x6, 0, 0, 0x00000640 },
            { 0x6, 0, 0, 0x00000000 },
        };

        fprog.len = sizeof(filters)/sizeof(struct sock_filter);
        fprog.filter = filters;

        if (setsockopt(fd,  SOL_SOCKET,  SO_ATTACH_FILTER, &fprog, sizeof(struct sock_fprog)) < 0)
        {
            WD_LOG(0,"setRawsockFilter set filter failed.\n");
            return -1;
        }
        return 0;
    }

    //初始化端口
    int LnxRsockInitWDPort(void)
    {
        int s;
        struct sockaddr_ll sll;
        struct ifreq ifr;
        int optlen;

        memset(&sll, 0, sizeof(sll));
        sll.sll_family = AF_PACKET;
        sll.sll_protocol = htons(0x0003);
        s = socket(sll.sll_family, SOCK_RAW, sll.sll_protocol);
        if (s <= 0)
        {
            WD_LOG(0,"LnxRsockInitWDPort: creat socket failed!\n");
            return -1;
        }

        optlen  = sizeof(unsigned long);
        memset(&ifr, 0, sizeof(ifr));
        // 需考虑 pEvbPort->acName 32 匹配 ifr.ifr_name 16 问题
        if (NULL == g_ptwdport)
        {
            WD_LOG(0,"LnxRsockInitWDPort: g_ptwdport is null!\n");
            close(s);
            return -1;
        }

        strncpy(ifr.ifr_name,g_ptwdport->strIfName,sizeof(ifr.ifr_name));
        if (-1 == ioctl(s, SIOCGIFINDEX, &ifr))
        {
            close(s);
            WD_LOG(0,"LnxRsockInitWDPort: ioctl  failed!\n");
            return -1;
        }

        sll.sll_ifindex = ifr.ifr_ifindex;
        if (0 != bind(s, (struct sockaddr *) &sll, sizeof(sll)))
        {
            close(s);
            WD_LOG(0,"LnxRsockInitWDPort: bind  failed!\n");
            return -1;
        }

        g_ptwdport->nSock = s;

        if (0 != wd_multicast_eth(g_ptwdport))
        {
            close(s);
            WD_LOG(0,"wd_multicast_eth failed!\n");
            return -1;
        }

        if (0 != wd_setRawsockFilter(s))
        {
            close(s);
            WD_LOG(0,"wd_setRawsockFilter failed!\n");
            return -1;
        }

        return 0;
    }

    int wd_LnxRsockDestroyEvbPort(unsigned int uiPortNo)
    {
        if ( g_ptwdport == NULL )
        {
            return 0;
        }

        if ( g_ptwdport->nSock  > 0 )
        {
            close(g_ptwdport->nSock);
        }

        return 0;
    }

    int wd_LnxRsockSendPkt(unsigned char * pPkt, unsigned int dwLen)
    {
        int ret = 0;

        if ( g_ptwdport == NULL )
        {
            WD_LOG(0,"wd_LnxRsockSendPkt port is invalid.\n");
            return -1;
        }

        if ( g_ptwdport->nSock <= 0 )
        {
            WD_LOG(0,"wd_LnxRsockSendPkt socket(%d) is invalid.\n", g_ptwdport->nSock);
            return -1;
        }

        ret = sendto(g_ptwdport->nSock,pPkt,dwLen,0,0,0);
        if ( -1 == ret )
        {
            WD_LOG(0,"LnxRawSendPkt failed ret err\n");
            return -1;
        }

        return 0;
    }

    int EthTypeInvalid(unsigned char * buf)
    {
        if ( NULL == buf)
        {
            return -1;
        }

        if ((0x0800 == (buf[12] << 8) + buf[13]))
        {
            return 0;
        }

        if ((0x88cc == (buf[12] << 8) + buf[13]))
        {
            return 0;
        }

        return -1;
    }

    //定时feed，g_ptwdport由domU初始化
    int tecs_wd_feed(unsigned int time_out)
    {
        //dom0上的mac假设固定
        unsigned char dstMac[6] = {0x00, 0x00, 0x01, 0x00, 0x00, 0x00};

        unsigned char *p;

        if (g_ptwdport == NULL)
        {
            if (0 != tecs_wdsocket_create())
            {
                WD_LOG(0,"tecs_wd_feed: tecs_wdsocket_create failed.\n");
                return -1;
            }

            if (0 != LnxRsockInitWDPort())
            {
                if (g_ptwdport)
                {
                    free(g_ptwdport);
                    g_ptwdport = NULL;
                }
                WD_LOG(0,"tecs_wd_feed: LnxRsockInitWDPort failed.\n");
                return -1;
            }
        }

        //需要构造报文，暂且这样
        p = (unsigned char *)malloc(80);
        if (NULL == p)
        {
            WD_LOG(0,"tecs_wd_feed: malloc failed.\n");
            return -1;
        }

        memset(p, 0, 80);
        memcpy(p, dstMac, 6);
        memcpy(p+6, g_srcMac, 6);

        *((unsigned char*)(p+12)) = 0x01/*0x88*/;
        *((unsigned char*)(p+13)) = 0x01/*0xcc*/;
        *((unsigned int*)(p+14)) = htonl(time_out);

        if (0 != wd_LnxRsockSendPkt(p, 80))
        {
            WD_LOG(0,"tecs_wd_feed: wd_LnxRsockSendPkt failed.\n");
            free(p);
            return -1;
        }
        free(p);
        return 0;
    }

#ifdef __cplusplus
}
#endif
