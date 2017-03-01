#ifdef __cplusplus
extern "C"{
#endif 
    #include "evb_comm.h"

#ifdef _EVB_TECS
#include <net/if.h> 
#include <net/if_arp.h> 
#include <sys/ioctl.h> 
#include <netinet/in.h> 
#endif

#ifdef _EVB_ISS
#include "iss_mux.h"
#endif 

#define IS_NUMERIC(c)       ((c) >= '0' && (c) <= '9')
#define IS_CHAR_ALPHA(c)    (((c) >= 'A' && (c) <= 'Z') || ((c) >= 'a' && (c) <= 'z'))
#define IS_CHAR_ALPHA_NUMERIC(c) (IS_NUMERIC(c) || IS_CHAR_ALPHA(c))

    static u8 s_ucByteOrder = EVB_BYTEORDER_INIT;

    int32 EvbGetLocTime(TEvbSysTime *pSysTm)
    {
#if _WIN32
        SYSTEMTIME st;
#endif

#if _EVB_TECS || _EVB_ISS
        time_t st = time(NULL);
        struct tm *pCurTm = NULL;
        struct timeval tva = {0};
#endif
        if(NULL == pSysTm)
        {
            EVB_ASSERT(0);
            return -1;
        }

#if _WIN32
        GetLocalTime(&st); 
        pSysTm->wYear = st.wYear; 
        pSysTm->wMonth = st.wMonth; 
        pSysTm->wDayOfWeek = st.wDayOfWeek; 
        pSysTm->wDay = st.wDay; 
        pSysTm->wHour = st.wHour; 
        pSysTm->wMinute = st.wMinute; 
        pSysTm->wSecond = st.wSecond; 
        pSysTm->wMilliseconds = st.wMilliseconds; 
#endif

#if _EVB_TECS || _EVB_ISS
        pCurTm = localtime(&st);
        gettimeofday (&tva, NULL);
        if(pCurTm)
        {   pSysTm->wYear = pCurTm->tm_year + 1900;   
            pSysTm->wMonth = pCurTm->tm_mon + 1;
            pSysTm->wDayOfWeek = pCurTm->tm_wday;  
            pSysTm->wDay =  pCurTm->tm_mday;
            pSysTm->wHour =  pCurTm->tm_hour;
            pSysTm->wMinute = pCurTm->tm_min;
            pSysTm->wSecond = pCurTm->tm_sec;
            pSysTm->wMilliseconds = (u16)(tva.tv_usec/1000); 
        }
#endif
        return 0;
    }

    void EvbDelay(u32 uiMillisecond)
    {
        EVB_ASSERT(uiMillisecond >= 0);

#ifdef _WIN32
        if (0 == uiMillisecond)
        {
            /* 避免零延时; */
            uiMillisecond++;
        }
        Sleep(uiMillisecond);
#else
        struct timespec delay;
        struct timespec rem;
        u32 uiMsInSecond = 1000;
        int32 n = -1;
        delay.tv_sec    = uiMillisecond/uiMsInSecond;
        delay.tv_nsec   = (uiMillisecond - delay.tv_sec*uiMsInSecond)*1000*1000;
        if (0 == uiMillisecond)
        {
            delay.tv_nsec  = 1;
        }
        do
        {
            n = nanosleep(&delay,&rem);
            delay=rem;
        }
        while (n < 0 && errno == EINTR);    
#endif
    }

    int32 EvbGetNicMac(const char *cstrNicName, u8 aucMac[], u8 arSz)
    {
#ifdef _WIN32
        if(NULL == cstrNicName || arSz != EVB_PORT_MAC_LEN || arSz > 6)
        {
            EVB_ASSERT(0);
            return -1;
        }
        
        return 0;        
#endif

#ifdef _EVB_TECS
        int sockfd; 
        struct ifreq ifr; 
        
        if(NULL == cstrNicName || arSz != EVB_PORT_MAC_LEN || arSz > 6)
        {
            EVB_ASSERT(0);
            return -1;
        }

        sockfd = socket(AF_INET,SOCK_STREAM,0); 
        if(-1 == sockfd)
        {
            EVB_LOG(EVB_LOG_ERROR, "EvbGetNicMac: call socket(AF_INET,SOCK_STREAM,0) failed.\n");
            return -1;
        }

        memset(&ifr,0,sizeof(ifr)); 
        strncpy(ifr.ifr_name, cstrNicName, sizeof(ifr.ifr_name)-1); 

        /* get hardware address */ 
        if(-1 == ioctl(sockfd, SIOCGIFHWADDR, &ifr))
        {
            EVB_LOG(EVB_LOG_ERROR, "EvbGetNicMac: call ioctl(...IfName: %s) failed.\n", cstrNicName);
            return -1; 
        }
        
        MEMCPY(aucMac, ifr.ifr_hwaddr.sa_data, arSz);
        
        return 0; 
#endif
        return -1;
    }

    int evb_random()
    {
#ifdef _EVB_TECS
      return (int)random();
#endif  
    
#ifdef _WIN32
        return (rand());
#endif   
        return 0;
    }

    static int32 evb_get_local_byteorder()
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
            s_ucByteOrder = EVB_BYTEORDER_LITTLEENDIAN;
            return 0;
        }
    
        s_ucByteOrder = EVB_BYTEORDER_BIGENDIAN;
        return 1;
    }
    
    int32 evb_local_is_bigendian()
    {
        if(EVB_BYTEORDER_BIGENDIAN==s_ucByteOrder)
        {
            return 1;
        }

        if(EVB_BYTEORDER_LITTLEENDIAN==s_ucByteOrder)
        {
            return 0;
        }
        
        if(EVB_BYTEORDER_INIT==s_ucByteOrder)
        {
            evb_get_local_byteorder();
        }

        if(EVB_BYTEORDER_BIGENDIAN==s_ucByteOrder)
        {
            return 1;
        }

        return 0;
    }
    
    int32  evb_ntohl(int32 x)
    {
        if(evb_local_is_bigendian())
        {
            return x;
        }
    
        return (int32)( \
                            ((((int32)(x)) & 0x000000ff) << 24) | \
                            ((((int32)(x)) & 0x0000ff00) <<  8) | \
                            ((((int32)(x)) & 0x00ff0000) >>  8) | \
                            ((((int32)(x)) & 0xff000000) >> 24));
    }
    
    int32  evb_htonl(int32 x)
    {
        if(evb_local_is_bigendian())
        {
            return x;
        }
    
        return (int32)( \
                            ((((int32)(x)) & 0x000000ff) << 24) | \
                            ((((int32)(x)) & 0x0000ff00) <<  8) | \
                            ((((int32)(x)) & 0x00ff0000) >>  8) | \
                            ((((int32)(x)) & 0xff000000) >> 24));
    }

    unsigned short  evb_ntohs(unsigned short x)
    {
        if(evb_local_is_bigendian())
        {
            return x;
        }
    
        return (unsigned short)( \
                            ((((unsigned short)(x)) & 0x00ff) << 8) | \
                            ((((unsigned short)(x)) & 0xff00) >> 8));
    }
    
    unsigned short  evb_htons(unsigned short x)
    {
        if(evb_local_is_bigendian())
        {
            return x;
        }
    
        return (unsigned short)( \
                            ((((unsigned short)(x)) & 0x00ff) << 8) | \
                            ((((unsigned short)(x)) & 0xff00) >> 8));
    }

    
    /************************************************************************/
    /* EVB common function define                                           */
    /************************************************************************/
    void DbgMemShow(void *pMem, WORD32 dwSz)
    {
        WORD32 i = 0;
        WORD32 j = 0;
        WORD32 k = 0;
        WORD32 m = 0;
        CHAR strAsci[32] = {0};
        CHAR cAlpha = 0;
        CHAR strSpacePad[128] = {0};

        if (NULL == pMem || 0 == dwSz)
        {
            PRINTF("\nUsage:\n       mem(addr, size)\n");
            PRINTF("       addr     memory address\n");
            PRINTF("       size     size of meory space from \"addr\" [byte] \n");
            return ;
        }

        memset(strAsci, 0x0, sizeof(strAsci));
        for (i = 0, j = 0; i < dwSz; i++)
        {
            if (0 == (i%15))
            {
                for(k = 0; k < j; k++)
                {
                    cAlpha = strAsci[k];
                    strAsci[k] = IS_CHAR_ALPHA_NUMERIC(cAlpha) ? cAlpha : '.';              
                }
                PRINTF("%s", strAsci);          
                j = 0;
                memset(strAsci, 0x0, sizeof(strAsci));  
                PRINTF("\n%p: ", (BYTE *)pMem + i);
            }
            PRINTF("%.02x ", ((BYTE *)pMem)[i]);
            if(j < 32)
            {
                strAsci[j++] = ((CHAR *)pMem)[i];
            }
        }
        if(j > 0 && j < 32)
        {   
            k = (i%15);
            if(k != 0)
            {
                m = 0;
                memset(strSpacePad, 0x0, sizeof(strSpacePad));          
                for(; k < 15; k++)
                {
                    memset(&strSpacePad[m], ' ', 3);
                    m += 3;
                }
                PRINTF("%s", strSpacePad);
            }
            for(k = 0; k <j; k++)
            {
                cAlpha = strAsci[k];
                strAsci[k] = IS_CHAR_ALPHA_NUMERIC(cAlpha) ? cAlpha : '.';              
            }
            PRINTF("%s", strAsci);      
        }
        PRINTF("\n\n");
        return ;
    }

    void EvbPrintPktStt(int32 lev, u64 num, const char *str)
    {
#ifdef _WIN32
        if (lev == 1)
        {
            PRINTF("%-29s%I64u\n", str, num);
        }
        else if (lev == 2)
        {
            PRINTF("  |__%-24s%I64u\n", str, num);
        }
        else if (lev == 3)
        {
            PRINTF("    |__%-22s%I64u\n", str, num);
        }
        else
        {
            PRINTF("      |__%-20s%I64u\n", str, num);
        }

#else
        if (lev == 1)
        {
            PRINTF("%-29s%llu\n", str, num);
        }
        else if (lev == 2)
        {
            PRINTF("  |__%-24s%llu\n", str, num);
        }
        else if (lev == 3)
        {
            PRINTF("    |__%-22s%llu\n", str, num);
        }
        else
        {
            PRINTF("      |__%-20s%llu\n", str, num);
        }
#endif
        return;
    }

    u32 evbcalc_2_exponent(u32 exp)
    {
        /* check valid */
        if(0 == exp )
        {
            return 0;
        }

        return (1 << (exp));
    }

    void EvbTrace(char   *fmt,   ...) 
    { 
#ifdef _WIN32
        #include   <windows.h> 

        char   out[1024]; 
        va_list   body; 
        va_start(body,   fmt); 
        vsprintf(out,   fmt,   body);
        va_end(body);  
        OutputDebugString(out);
#endif
        return ;
    } 

    char * evb_ipv4_to_string(u32 addr)
    {
        static char str[16];
        static char digits[EVB_DIGEST_COUNT] = "0123456789ABCDEF";
        char *tmp, punctuation, stack[4];
        unsigned int  i, j, chunk, radix, size;
        unsigned bits;
        unsigned char temp_ip[EVB_IPV4_ADDR_LEN] = {0};  
        unsigned int len = sizeof(str);

        u32 l_addr = addr;

        // ---> 转换一下 
        l_addr = evb_ntohl(addr);

        memcpy(temp_ip, &l_addr, EVB_IPV4_ADDR_LEN);

        punctuation = '.'; 
        size = EVB_IPV4_ADDR_STRING_LEN; 
        chunk = 1; 
        radix = 10;

        if (len < size)
        {        
            *str = '\0'; 
            return(str);
        }

        tmp = str;
        size = EVB_IPV4_ADDR_LEN;
        for (i = 0; i < size; i += chunk)
        {
            bits = 0;
            for (j = 0; j < chunk; j++)
            {
                //VNET_ASSERT((i+j)<MAX_IP_ADDR_LEN);
                bits = (bits << 8) | temp_ip[i + j];
            }
            if (i > 0)
            {
                *tmp++ = punctuation;
            }
            j = 0;
            do {
                //VNET_ASSERT(j<4);
                //VNET_ASSERT((bits % radix)<DIGEST_COUNT);
                stack[j++] = digits[bits % radix];
                bits /= radix;
            } while (bits);
            while (j > 0)
            {
                //VNET_ASSERT((j-1)<4); 
                *tmp++ = stack[--j];
            }
        }
        *tmp = '\0';

        return(str);
    }
        
    u32 evb_string_to_ipv4(char *str)
    {
        char  digits[EVB_MAX_DIGITS_LENGTH];
        char  punct[EVB_MAX_PUNCT_LENGTH];
        unsigned int      ndigits = 0, npunct = 0, idigits = 0, ipunct = 0, ibits = 0;
        u32 value = 0;
        char *s = str;
        char temp_ip[EVB_MAX_IP_ADDR_LEN]={0};
        u32 ip = 0;

        #if INSTALL_ATTACHE_IPV6
        int compressed = 0;
        #endif

        #define EVB_PUNCT_NUL       0x10
        #define EVB_PUNCT_COL       0x11
        #define EVB_PUNCT_DOT       0x12
    
        memset(punct, 0, sizeof(punct[0]) * EVB_MAX_PUNCT_LENGTH);

        if (!s)
        {
            goto lose;
        }
        //IPADDR_ZERO(temp_ip);
        memset((temp_ip), 0,  EVB_MAX_IP_ADDR_LEN);
        ip = 0;

        /* Skip leading whitespace */
        while ((*s == ' ') || (*s == '\t'))
        {
            s++;
        }
        /*
        * Scan the digits and legitimate punctuation out of the string.
        * Don't assume ASCII, the ISO owns the C specification now.
        */
        do
        {
            EVB_ASSERT(ndigits<EVB_MAX_DIGITS_LENGTH);
            switch (*s++)
            {
                case '0': digits[ndigits++] = 0x0; break;
                case '1': digits[ndigits++] = 0x1; break;
                case '2': digits[ndigits++] = 0x2; break;
                case '3': digits[ndigits++] = 0x3; break;
                case '4': digits[ndigits++] = 0x4; break;
                case '5': digits[ndigits++] = 0x5; break;
                case '6': digits[ndigits++] = 0x6; break;
                case '7': digits[ndigits++] = 0x7; break;
                case '8': digits[ndigits++] = 0x8; break;
                case '9': digits[ndigits++] = 0x9; break;
                #if INSTALL_ATTACHE_IPV6
                case 'a': 
                case 'A': 
                    digits[ndigits++] = 0xA; break;
                case 'b': 
                case 'B': 
                    digits[ndigits++] = 0xB; break;
                case 'c': 
                case 'C': 
                    digits[ndigits++] = 0xC; break;
                case 'd': 
                case 'D': 
                    digits[ndigits++] = 0xD; break;
                case 'e': 
                case 'E': 
                    digits[ndigits++] = 0xE; break;
                case 'f': 
                case 'F': 
                    digits[ndigits++] = 0xF; break;
                case ':': 
                    punct[npunct++] = digits[ndigits++] = EVB_PUNCT_COL; break;
                #endif
                case '.': 
                    punct[npunct++] = digits[ndigits++] = EVB_PUNCT_DOT; break;
                case '\0': 
                    punct[npunct++] = digits[ndigits++] = EVB_PUNCT_NUL; 
                    s = 0;
                    break;
                default: 
                    goto lose;
            }
            if(ndigits >= (int)(sizeof(digits)/sizeof(*digits)) ||
               npunct >= (int)(sizeof(punct)/sizeof(*punct)))
            {
                goto lose;
            }
        } while (s);

        /*
        * If we get here, we've tossed any totally bogus characters.
        * We still have to do length and syntax checks.
        */


        #if 1

        /*
        * If we get here, we're looking at an IPv4 address, the IPv4
        * compatability-mode tail of an IPv6 address, or an error.
        */

        if (npunct - ipunct != 4)
        {
            goto lose;
        }
    /*
        if (ibits == 0)
        {
            SET_IPADDR_TYPE(temp_ip, IPV4);
        }
        else if (ibits + IPV4_ADDR_LEN >= IPV6_ADDR_LEN)
        {
            goto lose;
        }
    */
        EVB_ASSERT(ipunct<EVB_MAX_PUNCT_LENGTH);
        while (punct[ipunct] == EVB_PUNCT_DOT || punct[ipunct] == EVB_PUNCT_NUL)
        {
            value = 0;
            EVB_ASSERT(idigits<EVB_MAX_DIGITS_LENGTH);
            while (digits[idigits] <= 9)
            {
                if ((value = (value * 10) + digits[idigits++]) > 255)
                {
                    goto lose;
                }
            }
            EVB_ASSERT(idigits<EVB_MAX_DIGITS_LENGTH);
            EVB_ASSERT(ipunct<EVB_MAX_PUNCT_LENGTH);
            if (digits[idigits] != punct[ipunct])
            {
                goto lose;
            }
            EVB_ASSERT(ibits<EVB_MAX_IP_ADDR_LEN);
            (temp_ip)[ibits++] = (u8) ( value & 0xFF);
            EVB_ASSERT(ipunct<EVB_MAX_PUNCT_LENGTH);
            if (punct[ipunct] == EVB_PUNCT_NUL)
            {
                memcpy(&ip, temp_ip, sizeof(ip));
                // ---> 转换一下 
                ip = evb_ntohl(ip);    
                return ip;
            }
            ipunct++;
            idigits++;
        }

        /* Fall into error code if we get here. */

        #endif /* INSTALL_ATTACHE_IPV4 */

        lose:
        memset((temp_ip), 0,  EVB_MAX_IP_ADDR_LEN);
        memcpy(&ip, temp_ip, sizeof(ip));
        // ---> 转换一下 
        ip = evb_ntohl(ip);
        return ip;
    }

    /* ISS mac vlan interface */
#ifdef _EVB_ISS
    INT32 EVB_ISS_MacItemOpt(WORD32 dwOpt, BYTE ucMac0, BYTE ucMac1, BYTE ucMac2, BYTE ucMac3, BYTE ucMac4, BYTE ucMac5, 
        WORD16 wVlan, WORD32 dwPort, BYTE ucModId,BYTE ucFlag)
    {
        T_ISS_MAC_INFOQRY tBspInfo;
        WORD32 dwRet = 0;

        MEMSET(&tBspInfo, 0, sizeof(tBspInfo));
        tBspInfo.aucMac[0] = ucMac0;
        tBspInfo.aucMac[1] = ucMac1;
        tBspInfo.aucMac[2] = ucMac2;
        tBspInfo.aucMac[3] = ucMac3;
        tBspInfo.aucMac[4] = ucMac4;
        tBspInfo.aucMac[5] = ucMac5;

        tBspInfo.wVlan = wVlan;
        tBspInfo.tLogicPort.dwPortIndex = dwPort;
        tBspInfo.ucModid = ucModId;
        tBspInfo.ucFlag = ucFlag;    

        if(ISS_DRV_SET != dwOpt &&
            ISS_DRV_GET != dwOpt &&
            ISS_DRV_DEL != dwOpt)
        {
            EVB_LOG(EVB_LOG_ERROR,"EVB_ISS_MacItemOpt(opt:%u) is invalid.\n",dwOpt);
            return -1;
        }

        dwRet = issio_macitemopt(dwOpt,&tBspInfo);
        if(0 != dwRet)
        {
            EVB_LOG(EVB_LOG_ERROR,"EVB_ISS_MacItemOpt(port:%u,opt:%d,mac:%x:%x:%x:%x:%x:%x,vlan:%x,ret:%d) failed. \n",dwPort,dwOpt,\
                ucMac0,ucMac1,ucMac2,ucMac3,ucMac4,ucMac5,wVlan,dwRet);
            return -1;            
        }
        else
        {
            EVB_LOG(EVB_LOG_ERROR,"EVB_ISS_MacItemOpt(port:%u,opt:%d,mac:%x:%x:%x:%x:%x:%x,vlan:%x) success!\n",dwPort,dwOpt,\
                ucMac0,ucMac1,ucMac2,ucMac3,ucMac4,ucMac5,wVlan);
            return 0;
        }

        return 0;
    }

    INT32 EVB_ISS_MacItemOptGet(WORD32 dwOpt, BYTE ucMac0, BYTE ucMac1, BYTE ucMac2, BYTE ucMac3, BYTE ucMac4, BYTE ucMac5, 
        WORD16 wVlan, WORD32 * dwPort, BYTE ucModId,BYTE ucFlag)
    {
        T_ISS_MAC_INFOQRY tBspInfo;
        WORD32 dwRet = 0;

        MEMSET(&tBspInfo, 0, sizeof(tBspInfo));
        tBspInfo.aucMac[0] = ucMac0;
        tBspInfo.aucMac[1] = ucMac1;
        tBspInfo.aucMac[2] = ucMac2;
        tBspInfo.aucMac[3] = ucMac3;
        tBspInfo.aucMac[4] = ucMac4;
        tBspInfo.aucMac[5] = ucMac5;

        tBspInfo.wVlan = wVlan;
        //tBspInfo.tLogicPort.dwPortIndex = dwPort;
        tBspInfo.ucModid = ucModId;
        tBspInfo.ucFlag = ucFlag;    

        if(ISS_DRV_GET != dwOpt)
        {
            return -1;
        }

        dwRet = issio_macitemopt(dwOpt,&tBspInfo);
        if(0 != dwRet)
        {
            EVB_LOG(EVB_LOG_ERROR,"EVB_ISS_MacItemOptGet(port:%u,opt:%d,mac:%x:%x:%x:%x:%x:%x,vlan:%x,ret:%d) failed. \n",dwPort,dwOpt,\
                ucMac0,ucMac1,ucMac2,ucMac3,ucMac4,ucMac5,wVlan,dwRet);
            return -1;            
        }
        else
        {
#if 0        
            EVB_LOG(EVB_LOG_ERROR,"EVB_ISS_MacItemOptGet(port:%u,opt:%d,mac:%x:%x:%x:%x:%x:%x,vlan:%x) success!\n",dwPort,dwOpt,\
                ucMac0,ucMac1,ucMac2,ucMac3,ucMac4,ucMac5,wVlan);
#endif                 
        }

        *dwPort =  tBspInfo.tLogicPort.dwPortIndex;

        // PRINTF("EVB_ISS_MacItemOptGet outPort: %u\n", tBspInfo.tLogicPort.dwPortIndex);
         
        return 0;
    }
    
#endif 

    /***/
    u32 evb_SetMacVlan(u32 port,BYTE mac[EVB_PORT_MAC_LEN],u16 vlan,u32 opt)
    {
#if _EVB_ISS
        const BYTE ucModID = 1;
        const BYTE ucFlag = 1;// 7;

        WORD32 outPort = 0;
        
        struct tagEvbIssPort * IssPort = NULL; 
        IssPort = GetIssPortFromEvbPort(port);

        if(NULL == IssPort)
        {
            EVB_LOG(EVB_LOG_ERROR,"evb_SetMacVlan GetIssPortFromEvbPort(%u) failed.\n",port);
            return -1;
        }

        /*del*/
        if(ISS_DRV_DEL == opt)
        {            
            if( 0 == EVB_ISS_MacItemOptGet(ISS_DRV_GET,mac[0],mac[1],mac[2],mac[3],mac[4],mac[5],vlan,&outPort,ucModID,ucFlag) )
            {
                 if( outPort != IssPort->uiPortNo)
                 {
                     EVB_LOG(EVB_LOG_ERROR,"Mac table (mac:%x:%x:%x:%x:%x:%x,vlan:%x,outPort:%u),but delete item outPort is %u .\n",\
                         mac[0],mac[1],mac[2],mac[3],mac[4],mac[5],vlan,outPort,IssPort->uiPortNo);
                     return 0; 
                 }
            }
            else
            {
                 EVB_LOG(EVB_LOG_ERROR,"evb_SetMacVlan EVB_ISS_MacItemOptGet (mac:%x:%x:%x:%x:%x:%x,vlan:%x) failed.\n",\
                         mac[0],mac[1],mac[2],mac[3],mac[4],mac[5],vlan);
                 return 0;
            }
        }
        
        if( 0 != EVB_ISS_MacItemOpt(opt,mac[0],mac[1],mac[2],mac[3],mac[4],mac[5],vlan,IssPort->uiPortNo,ucModID,ucFlag) )
        {
            EVB_LOG(EVB_LOG_ERROR,"evb_SetMacVlan EVB_ISS_MacItemOpt (port:%u, opt:%u, mac:%x:%x:%x:%x:%x:%x,vlan:%x) failed.\n",\
                         IssPort->uiPortNo,opt,mac[0],mac[1],mac[2],mac[3],mac[4],mac[5],vlan);
            return -1;
        }
#endif 
        return 0;
    }
    
int evb_set_cml(u32 dwPort, u32 dwCml)
{

#ifdef _EVB_ISS

   SWORD32 sdwRet = 0;

   /* ISS_ETH_CML_HW_LEARN|ISS_ETH_CML_SEND_TO_CPU|ISS_ETH_CML_FORWARD */
   sdwRet = issio_maccmlopt(dwPort, &dwCml, ISS_DRV_SET);
   if(0 != sdwRet)
   {
        return -1;
   }
#endif

   return 0;   
}

int evb_get_cml(u32 dwPort)
{
#ifdef _EVB_ISS

   SWORD32 sdwRet = 0;
   DWORD dwCml = 0;

   sdwRet = issio_maccmlopt(dwPort, &dwCml, ISS_DRV_GET);
   if(0 != sdwRet)
   {
        return -1;
   }

   return dwCml;
#endif

   return 0;   
}

int32 PrintLocalTimer()
{
#ifdef _WIN32
    SYSTEMTIME Time={0};
    char cTime[64] = {0};
    GetLocalTime(&Time);//获得当前本地时间
    sprintf(cTime,"%u.%u.%u %u:%u:%u.%u",Time.wYear,Time.wMonth,Time.wDay,Time.wHour,Time.wMinute,Time.wSecond,Time.wMilliseconds);
    PRINTF("local timer: %s \n",cTime);
#endif 
#if _EVB_TECS || _EVB_ISS
    TEvbSysTime Time;
    char cTime[64] = {0};
    if(0 == EvbGetLocTime(&Time))
    {
        sprintf(cTime,"%u.%u.%u %u:%u:%u.%u",Time.wYear,Time.wMonth,Time.wDay,Time.wHour,Time.wMinute,Time.wSecond,Time.wMilliseconds);

        PRINTF("local timer: %s \n",cTime);
    }
#endif 

    return 0;
}

void t_mac(u32 port, BYTE opt )
{
#if _EVB_ISS
        BYTE mac[6]={0x00,0x01,0x02,0x03,0x04,0x0f};
    u16 vlan = 1;
    const BYTE ucModID = 1;
    const BYTE ucFlag = 1;// 7;
    struct tagEvbIssPort * IssPort = NULL; 
    IssPort = GetIssPortFromEvbPort(port);

    if(NULL == IssPort)
    {
        EVB_LOG(EVB_LOG_ERROR,"t_mac GetIssPortFromEvbPort(%u) failed.\n",port);
        return ;
    }

    EVB_ISS_MacItemOpt(opt,mac[0],mac[1],mac[2],mac[3],mac[4],mac[5],vlan,IssPort->uiPortNo,ucModID,ucFlag);
#endif  
}

void t_mac_get( )
{
#if _EVB_ISS
    BYTE mac[6]={0x00,0x01,0x02,0x03,0x04,0x0f};
    u16 vlan = 1;
    WORD32 outPort = 0;
    const BYTE ucModID = 1;
    const BYTE ucFlag = 1;// 7;

    EVB_ISS_MacItemOptGet(ISS_DRV_GET,mac[0],mac[1],mac[2],mac[3],mac[4],mac[5],vlan,&outPort,ucModID,ucFlag);

    PRINTF("t_mac_get port:%u\n",outPort);
    
#endif 
}

#ifdef __cplusplus
}
#endif 

