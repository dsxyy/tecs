#include "vnet_comm.h"
#include <iostream>
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <unistd.h> 
#include <sys/ioctl.h> 
#include <net/if.h> 
#include <arpa/inet.h> 
#include <linux/netlink.h> 
#include <linux/rtnetlink.h> 
#include <uuid/uuid.h>
#include "sys.h" 

/*************看门狗功能添加 begin ********/
#include <sys/socket.h>
#include <sys/types.h>
#include <linux/if_ether.h>
#include <netpacket/packet.h>
#include <sys/socket.h>
#include <linux/filter.h>

#include <sys/types.h>
#include <sys/wait.h>

struct tagLnxRSockPort * g_ptwdport = NULL;
unsigned char g_srcMac[6] = {0};

#define PRINTF printf
#define WD_LOG(nLvl, fmt, arg...)\
    do\
    {\
    PRINTF(fmt, ##arg);\
    }while(0)
/************* end ***********************/

// ipv4_to_string 
#define DIGEST_COUNT                17
#define IPV4_ADDR_STRING_LEN sizeof("255.255.255.255")
#define IPV4_ADDR_LEN           4
#define MAX_IP_ADDR_LEN         IPV4_ADDR_LEN

// 判断本地是网络字节序 
// =1 网络字节序 big endian； = 0 little endian
int32 local_is_bigendian()
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

int32  vnet_ntohl(int32 x)
{
    if(local_is_bigendian())
    {
        return x;
    }

    return (int32)( \
                        ((((int32)(x)) & 0x000000ff) << 24) | \
                        ((((int32)(x)) & 0x0000ff00) <<  8) | \
                        ((((int32)(x)) & 0x00ff0000) >>  8) | \
                        ((((int32)(x)) & 0xff000000) >> 24));
}

int32  vnet_htonl(int32 x)
{
    if(local_is_bigendian())
    {
        return x;
    }

    return (int32)( \
                        ((((int32)(x)) & 0x000000ff) << 24) | \
                        ((((int32)(x)) & 0x0000ff00) <<  8) | \
                        ((((int32)(x)) & 0x00ff0000) >>  8) | \
                        ((((int32)(x)) & 0xff000000) >> 24));
}

unsigned short  vnet_ntohs(unsigned short x)
{
    if(local_is_bigendian())
    {
        return x;
    }

    return (unsigned short)( \
                        ((((unsigned short)(x)) & 0x00ff) << 8) | \
                        ((((unsigned short)(x)) & 0xff00) >> 8));
}

unsigned short  vnet_htons(unsigned short x)
{
    if(local_is_bigendian())
    {
        return x;
    }

    return (unsigned short)( \
                        ((((unsigned short)(x)) & 0x00ff) << 8) | \
                        ((((unsigned short)(x)) & 0xff00) >> 8));
}


char * ipv4_to_string(int addr)
{
    static char str[16];
    static char digits[DIGEST_COUNT] = "0123456789ABCDEF";
    char *tmp, punctuation, stack[4];
    unsigned int  i, j, chunk, radix, size;
    unsigned bits;
    unsigned char temp_ip[IPV4_ADDR_LEN] = {0};  
    unsigned int len = sizeof(str);

    int l_addr = addr;
    
    // ---> 转换一下 
    l_addr = vnet_ntohl(addr);

    memcpy(temp_ip, &l_addr, IPV4_ADDR_LEN);
    
    punctuation = '.'; 
    size = IPV4_ADDR_STRING_LEN; 
    chunk = 1; 
    radix = 10;
   
    if (len < size)
    {        
        *str = '\0'; 
        return(str);
    }

    tmp = str;
    size = IPV4_ADDR_LEN;
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

void string_to_ipv4(const char *str, int32 &ip)
{
    #define MAX_DIGITS_LENGTH  sizeof("ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255")
    #define MAX_PUNCT_LENGTH   sizeof("::::::...")
    #define IP_TO_STRING_SIZE    4
    #define MAX_IP_ADDR_LEN         IPV4_ADDR_LEN

    char  digits[MAX_DIGITS_LENGTH];
    char  punct[MAX_PUNCT_LENGTH];
    unsigned int      ndigits = 0, npunct = 0, idigits = 0, ipunct = 0, ibits = 0;
    uint32 value = 0;
    const char *s = str;
    char temp_ip[MAX_IP_ADDR_LEN]={0};

    #if INSTALL_ATTACHE_IPV6
    int compressed = 0;
    #endif

    #define PUNCT_NUL       0x10
    #define PUNCT_COL       0x11
    #define PUNCT_DOT       0x12
    
    memset(punct, 0, sizeof(punct[0]) * MAX_PUNCT_LENGTH);

    if (!s)
    {
        goto lose;
    }
    //IPADDR_ZERO(temp_ip);
    memset((temp_ip), 0,  MAX_IP_ADDR_LEN);
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
        VNET_ASSERT(ndigits<MAX_DIGITS_LENGTH);
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
                punct[npunct++] = digits[ndigits++] = PUNCT_COL; break;
            #endif
            case '.': 
                punct[npunct++] = digits[ndigits++] = PUNCT_DOT; break;
            case '\0': 
                punct[npunct++] = digits[ndigits++] = PUNCT_NUL; 
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
    VNET_ASSERT(ipunct<MAX_PUNCT_LENGTH);
    while (punct[ipunct] == PUNCT_DOT || punct[ipunct] == PUNCT_NUL)
    {
        value = 0;
        VNET_ASSERT(idigits<MAX_DIGITS_LENGTH);
        while (digits[idigits] <= 9)
        {
            if ((value = (value * 10) + digits[idigits++]) > 255)
            {
                goto lose;
            }
        }
        VNET_ASSERT(idigits<MAX_DIGITS_LENGTH);
        VNET_ASSERT(ipunct<MAX_PUNCT_LENGTH);
        if (digits[idigits] != punct[ipunct])
        {
            goto lose;
        }
        VNET_ASSERT(ibits<MAX_IP_ADDR_LEN);
        (temp_ip)[ibits++] = (uint8) ( value & 0xFF);
        VNET_ASSERT(ipunct<MAX_PUNCT_LENGTH);
        if (punct[ipunct] == PUNCT_NUL)
        {
            memcpy(&ip, temp_ip, sizeof(ip));
            // ---> 转换一下 
            ip = vnet_ntohl(ip);    
            return;
        }
        ipunct++;
        idigits++;
    }

    /* Fall into error code if we get here. */

    #endif /* INSTALL_ATTACHE_IPV4 */

    lose:
    memset((temp_ip), 0,  MAX_IP_ADDR_LEN);
    memcpy(&ip, temp_ip, sizeof(ip));
    // ---> 转换一下 
    ip = vnet_ntohl(ip);
    return;
}

/*为解决 611003936948 【TECS】cc启动失败，提示resource-limit-exceeded 
TECS VNET 内部进行封装了system (参考tulip )
*/
int vnet_system_tulip(const string& cmd)
{
    pid_t pid;
    int status, rc;
    
    switch(pid = fork()) {
    case -1:
        return -1;

    default:    /* Parent */
        waitpid(pid, &status, 0);
        if (!WIFEXITED(status)) {
            return -1;
        }

        rc = WEXITSTATUS(status);
        if (rc) {
            printf("Child exit:%d\n", rc);
        }
        return rc;

    case 0:        /* Child */
        int fd;
        /* close all descriptors except stdin/out/err. */
        /* don't leak open files */
        for (fd = getdtablesize() - 1; fd > STDERR_FILENO; fd--) 
        {
            close(fd);
            //execl("/bin/sh", "-c", cmd.c_str());
            execl("/bin/sh", "/bin/sh", "-c", cmd.c_str(), NULL);            
            exit(errno);
        }
    }
    return 0;
}


int32 vnet_system(const string &strCmd)
{
  string cmd_echo = "echo " + strCmd + " >>/var/log/vnetinfo";
  system(cmd_echo.c_str());
  //return system(strCmd.c_str());
  return vnet_system_tulip(strCmd);
}

int32 vnet_chk_dir(const string &strDir)
{
    struct stat tStat = {0};

    if (strDir.empty()) 
    {
        return 0;
    }
    
    if(0 != lstat(strDir.c_str(), &tStat) )
    {            
        return 0;
    }

    if (!S_ISDIR(tStat.st_mode)) 
    {
        if (S_ISLNK(tStat.st_mode)) 
        {
            string strTmp = strDir;
            strTmp.append("/");
            lstat (strTmp.c_str(), &tStat);
            if (S_ISDIR(tStat.st_mode)) 
            {
                return 1;
            }
         }
        return 0;
    }
    
    return 1;
}

int32 vnet_chk_dev(const string &strDevName)
{
    if(strDevName.empty())
    {
        return 0;
    }
    
    string strTemp = "/sys/class/net/";
    strTemp.append(strDevName);
    strTemp.append("/");
    
    return vnet_chk_dir(strTemp);
}

int32 vnet_chk_dev_state(const string &strDevName)
{
    if(0 == vnet_chk_dev(strDevName))
    {
        return VNET_DEVICE_INVALID;
    }
    string strTemp = "/sys/class/net/";
    strTemp.append(strDevName);
    strTemp.append("/operstate");

    FILE *fp = NULL;
    if ( NULL == (fp = fopen(strTemp.c_str(), "r")) )
    {
        // error log
        cout << "Can't open " << strTemp << "  file." << endl;
        return VNET_DEVICE_INVALID;
    } 

    char acBuff[128];
    bzero(acBuff, 128);
    if (fgets(acBuff, 128, fp)) 
    {
        string strState;
        strState.assign(acBuff);
        if(string::npos != strState.find("up"))
        {
            fclose(fp);
            fp = NULL;                
            return VNET_DEVICE_UP;
        }
    }
        
    fclose(fp);
    fp = NULL;        
    return VNET_DEVICE_DOWN;
}

int32 vnet_check_macstr(string & mac)
{
    // 添加mac有效性检查
    unsigned char   aucZeroAddr[6] = {0};
    unsigned char   h_mac[6]={0};
    int ret = 0;
    // 0 格式必须是00:11:22:33:44:55
    string tmp("00:11:22:33:44:55");
    if( mac.size() != tmp.size())
    {
        string sInfo = "check_mac[";
        sInfo += mac;
        sInfo += "] failed. len is illegal ";
		OutPut(SYS_ERROR, "In vnet_check_macstr,sInfo:%s\n", sInfo.c_str());
        return -1;
    }

    // 1 检查是否存在无效字符
    string lower_case = "0123456789abcdefABCDEF:";
    if( string::npos != mac.find_first_not_of(lower_case))
    {
        string sInfo = "check_mac[";
        sInfo += mac;
        sInfo += "] failed. invalid mac address";
        OutPut(SYS_ERROR, "In vnet_check_macstr,sInfo:%s\n", sInfo.c_str());
        return -1;
    }

    // 2 检查':'是否有效
    string::size_type pos = 0;
    string::size_type pre_pos = 0;
    while((pos = mac.find_first_of(':',pos)) != string::npos)
    {
         if( 2 != (pos - pre_pos ))
         {
             string sInfo = "check_mac[";
             sInfo += mac;
             sInfo += "] failed. invalid mac address";
             OutPut(SYS_ERROR, "In vnet_check_macstr,sInfo:%s\n", sInfo.c_str());
             return -1;
         }            
         pre_pos = ++pos; 
    }        
    
    // 3 检查mac 有效性
    ret = sscanf(mac.c_str(),"%x:%x:%x:%x:%x:%x", &h_mac[0],&h_mac[1],&h_mac[2],&h_mac[3],&h_mac[4],&h_mac[5]);
    if( 6 != ret )
    {
        string sInfo = "check_mac[";
        sInfo += mac;
        sInfo += "] failed.";
        OutPut(SYS_ERROR, "In vnet_check_macstr,sInfo:%s\n", sInfo.c_str());
        return -1;
    }

    // 检查mac  1 单播；2 是否为全0
    if( h_mac[0] & 0x01 )
    {            
        string sInfo = "check_mac[";
        sInfo += mac;
        sInfo += "] failed. is multicast address.";
        OutPut(SYS_ERROR, "In vnet_check_macstr,sInfo:%s\n", sInfo.c_str());
        return -1;
    }
    if (0 == memcmp(aucZeroAddr,h_mac,6))
    {
        string sInfo = "check_mac[";
        sInfo += mac;
        sInfo += "] failed. is all 0.";
        OutPut(SYS_ERROR, "In vnet_check_macstr,sInfo:%s\n", sInfo.c_str());
        return -1;
    }
    return 0;
}

int32 vnet_mac_str_to_byte(string & str, BYTE *bMac)
{
    unsigned int buf[6] = {0};

	if (sscanf(str.c_str(),"%x:%x:%x:%x:%x:%x",buf,buf+1,buf+2,buf+3,buf+4,buf+5) != 6)
	{
		return -1;
	}
	(bMac)[0] = (unsigned char)buf[0];
	(bMac)[1] = (unsigned char)buf[1];
	(bMac)[2] = (unsigned char)buf[2];
	(bMac)[3] = (unsigned char)buf[3];
	(bMac)[4] = (unsigned char)buf[4];
	(bMac)[5] = (unsigned char)buf[5];
    return 0;
}

void StopDhcpService()
{
    string cmd("service dhcpd stop");
    vnet_system(cmd.c_str());
}

void remove_lr_space(string & str)
{
    string buff(str); 
    char space = ' '; 
    str.assign(buff.begin() + buff.find_first_not_of(space), 
    buff.begin() + buff.find_last_not_of(space) + 1); 
}

int get_mtu(const char* lpszEth, unsigned int &mtu)
{
    struct ifreq ifr;

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0); 
    if ( -1 == sockfd ) 
    { 
        return -1; 
    } 
    
    strcpy(ifr.ifr_name,lpszEth);
    if(ioctl(sockfd,SIOCGIFMTU,(char *)&ifr) < 0)
    { 
        close(sockfd); 
        mtu = -1; 
        return -1;
    }
 
    mtu = (unsigned int)ifr.ifr_mtu;
 
    close(sockfd); 
    sockfd = -1; 
    return 0;
}




// 设置指定网口的mtu值 
int set_mtu(const char* lpszEth, unsigned int mtu) 
{ 
    struct ifreq ifr; 
 
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0); 
    if ( -1 == sockfd ) 
    { 
        return -1; 
    } 
 
    strcpy(ifr.ifr_name, lpszEth); 
    ifr.ifr_ifru.ifru_mtu = mtu; 
    if ( ioctl(sockfd, SIOCSIFMTU, &ifr) < 0 ) 
    { 
        close(sockfd); 
        sockfd = -1; 
        return -1;
    } 
    
    close(sockfd); 
    sockfd = -1; 
    return 0;
} 

/*************看门狗功能添加 begin ********/
unsigned int  rawsocket_ntohl(unsigned int x)
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

unsigned short rawsocket_htons(unsigned short x)
{
    if (local_is_bigendian())
    {
        return x;
    }

    return (unsigned short)( \
                             ((((unsigned short)(x)) & 0x00ff) << 8) | \
                             ((((unsigned short)(x)) & 0xff00) >> 8));
}

static int tecs_wdbr_create(void)
{
    if (g_ptwdport)
    {
        free(g_ptwdport);
    }

    //分配socketport
    g_ptwdport = (struct tagLnxRSockPort *)malloc(sizeof(struct tagLnxRSockPort));
    if (NULL == g_ptwdport)
    {
        return -1;
    }

    //初始化socketport
    memset(g_ptwdport, 0, sizeof(struct tagLnxRSockPort));
    g_ptwdport->uiPortNo = 0;
    strcpy(g_ptwdport->strIfName, "br_wd");

    return 0;
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
        WD_LOG(0, "wd_multicast_eth:param is invalid.\n");
        return -1;
    }
    fd = port->nSock;

    if ( 0 >= fd )
    {
        WD_LOG(0, "wd_multicast_eth:port socket(%u %d) is invalid.\n",port->uiPortNo, fd);
        return -1;
    }

    memset( &sll, 0, sizeof(sll) );
    sll.sll_family = PF_PACKET;
    sll.sll_protocol = rawsocket_htons(0x0003);

    strncpy(ifstruct.ifr_name,port->strIfName,sizeof(ifstruct.ifr_name));
    ret = ioctl(fd, SIOCGIFINDEX, &ifstruct);
    if (ret != 0 )
    {
        WD_LOG(0, "wd_multicast_eth: port(%s) ioctl(SIOCGIFINDEX) failed.\n",ifstruct.ifr_name);
        return -1;
    }

    sll.sll_ifindex = ifstruct.ifr_ifindex;
    mr.mr_ifindex = ifstruct.ifr_ifindex;

    ret = ioctl (fd, SIOCGIFHWADDR, &ifstruct);
    if (ret != 0 )
    {
        WD_LOG(0, "wd_multicast_eth: port(%s) ioctl(SIOCGIFHWADDR) failed.\n",ifstruct.ifr_name);
        return -1;
    }
    memcpy (sll.sll_addr, ifstruct.ifr_ifru.ifru_hwaddr.sa_data, sizeof(sll.sll_addr));

    ret = ioctl (fd, SIOCGIFFLAGS, &ifstruct);
    if (ret != 0 )
    {
        WD_LOG(0, "wd_multicast_eth: port(%s) ioctl(SIOCGIFFLAGS) failed.\n",ifstruct.ifr_name);
        return -1;
    }

    ifstruct.ifr_flags |= (IFF_UP | IFF_BROADCAST | IFF_MULTICAST |IFF_PROMISC);

    ret = ioctl (fd, SIOCSIFFLAGS, &ifstruct);
    if (ret != 0 )
    {
        WD_LOG(EVB_LOG_ERROR, "wd_multicast_eth: port(%s) ioctl(SIOCSIFFLAGS) failed.\n",ifstruct.ifr_name);
        return -1;
    }

#if 0
    ret = ioctl(fd, SIOCGIFINDEX, &ifstruct);
    if (ret != 0 )
    {
        WD_LOG(EVB_LOG_ERROR, "wd_multicast_eth: port(%s) ioctl(SIOCGIFINDEX) failed.\n",ifstruct.ifr_name);
        return -1;
    }

    memset(&mr, 0, sizeof(mr));
    mr.mr_ifindex = ifstruct.ifr_ifindex;
    mr.mr_type = PACKET_MR_MULTICAST;
    mr.mr_alen = 6;
    memcpy(mr.mr_address, ECP_MULTICAST_MAC, 6);

    ret = setsockopt(fd, SOL_PACKET, PACKET_ADD_MEMBERSHIP,&mr, sizeof(mr));
    if (ret != 0 )
    {
        EVB_LOG(EVB_LOG_ERROR, "multicast_eth: setsockopt(ecp_mult_mac) failed(%d %s).\n",errno,strerror(errno));
        return -1;
    }

    memcpy(mr.mr_address, multi_cast_mac1, 6);
    ret = setsockopt(fd, SOL_PACKET, PACKET_ADD_MEMBERSHIP,&mr, sizeof(mr));
    if (ret != 0 )
    {
        EVB_LOG(EVB_LOG_ERROR, "multicast_eth: setsockopt(lldp_mult_mac) failed(%d %s).\n",errno,strerror(errno));
        return -1;
    }

    memcpy(mr.mr_address, multi_cast_mac2, 6);
    ret = setsockopt(fd, SOL_PACKET, PACKET_ADD_MEMBERSHIP,&mr, sizeof(mr));
    if (ret != 0 )
    {
        EVB_LOG(EVB_LOG_ERROR, "multicast_eth: setsockopt(lldp_mult_mac) failed(%d %s).\n",errno,strerror(errno));
        return -1;
    }

    memcpy(mr.mr_address, multi_cast_mac3, 6);
    ret = setsockopt(fd, SOL_PACKET, PACKET_ADD_MEMBERSHIP,&mr, sizeof(mr));
    if (ret != 0 )
    {
        EVB_LOG(EVB_LOG_ERROR, "multicast_eth: setsockopt(lldp_mult_mac) failed(%d %s).\n",errno,strerror(errno));
        return -1;
    }
#endif

    return 0;
}

int wd_setRawsockFilter(int fd)
{
    int ethType_1 = 0x0101;
    int ethType_2 = 0x88cc;
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
    sll.sll_protocol = rawsocket_htons(0x0003);
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

int EthTypeInvalid(unsigned char * buf)
{
    if ( NULL == buf)
    {
        return -1;
    }

    if((0x0101 == (buf[12] << 8) + buf[13]))
    {
        return 0;
    }

    if ((0x88cc == (buf[12] << 8) + buf[13]))
    {
        return 0;
    }

    return -1;
}

int wd_LnxRsockRcvPkt(unsigned char **ppPkt, int *pdwLen)
{
    int len = 0;
    unsigned char buf[1600]={0};
    int iMode =1;/*1，非阻塞 0，阻塞*/

    if (NULL == g_ptwdport)
    {
        return 0;
    }

    ioctlsocket(g_ptwdport->nSock,FIONBIO, &iMode);
    len = recvfrom(g_ptwdport->nSock, buf, 1600, 0, 0, 0);

    if (len > 0)
    {
        if (0 == EthTypeInvalid(buf))
        {
            (*ppPkt ) = (unsigned char *)malloc(len);
            if (NULL  == (*ppPkt))
            {
                return 0;
            }
            memcpy((*ppPkt), buf, len);
            *pdwLen = len;
            return len;
        }
    }

    return 0;
}

//定时receive，g_ptwdport由dom0初始化
int tecs_wd_rcv(unsigned char **ppPkt, int *pdwLen, int *pFlag)
{
    if ((*pFlag) == 0)
    {
        if (0 != tecs_wdbr_create())
        {
            WD_LOG(0,"tecs_wd_rcv: tecs_wdbr_create failed.\n");
            return -1;
        }

        if (0 != LnxRsockInitWDPort())
        {
            WD_LOG(0,"tecs_wd_rcv: LnxRsockInitWDPort failed.\n");
            return -1;
        }

        *pFlag = 1;
    }

    if (0 != wd_LnxRsockRcvPkt(ppPkt, pdwLen))
    {
        return *pdwLen;
    }

    return 0;
}

int GetRealIfNameFromBridge(string & port, string& rel_name)
{
    char show_bridge_name[24]={0}; //网桥名
    char show_bridge_id[24]={0};   //网桥ID
    char show_stp_enable[4]={0};   //sfp是否开启
    char show_peth_name[24]={0};   //物理网卡名
    ostringstream oss;

    int find = FALSE;

    oss.str("");
    /* brctl show grep eth0 */
    oss<< "brctl show 2>/dev/null | grep "<< port;

    vector<string> vec_res;
    if (0 != RunCmd(oss.str(),vec_res))
    {
        VNET_LOG(VNET_LOG_ERROR, "GetRealIfNameFromBridge: call system(%s) failed\n",oss.str().c_str());
    }
    else
    {
        vector<string>::iterator it_res = vec_res.begin();
        for(; it_res != vec_res.end(); ++it_res)        
        {
            sscanf((*it_res).c_str(), "%s %s %s %s", show_bridge_name, show_bridge_id, show_stp_enable,show_peth_name);
            if (strcmp(port.c_str(),show_peth_name)== 0)
            {
                rel_name.assign(show_peth_name);
                find = TRUE;
                break;
            }

            if (strcmp(port.c_str(),show_bridge_name)== 0)
            {
                rel_name.assign(show_peth_name);
                find = TRUE;
                break;
            }
        }
    }

    if( FALSE == find)
    {
        return -1;
    }

    return 0;
}

string GetBrigNameFromPeth(const string& pethname)
{
    char show_bridge_name[24]={0}; //网桥名
    char show_bridge_id[24]={0};   //网桥ID
    char show_stp_enable[4]={0};   //sfp是否开启
    char show_peth_name[24]={0};   //物理网卡名
    ostringstream oss;
    string ret_briname="";

    oss.str("");
    /* brctl show grep eth0  */
    oss<< "brctl show 2>/dev/null | grep "<< pethname;

    vector<string> vec_res;
    if (0 != RunCmd(oss.str(),vec_res))
    {
        VNET_LOG(VNET_LOG_ERROR, "GetBrigNameFromPeth: call system(%s) failed\n",oss.str().c_str());
    }
    else
    {
        vector<string>::iterator it_res = vec_res.begin();
        for(; it_res != vec_res.end(); ++it_res)  
        {
            sscanf((*it_res).c_str(), "%s %s %s %s", show_bridge_name, show_bridge_id, show_stp_enable,show_peth_name);
            if (strcmp(pethname.c_str(),show_peth_name)== 0)
            {
                ret_briname.assign(show_bridge_name);

                break;
            }
        }
    }

    return ret_briname;
}

bool vnet_run_command(const string& command,string &result)
{
    if( 0 != RunCmd(command,result))
    {
        return FALSE;
    }
  
    return TRUE;
}

int VNetCheckSupportOVS()
{
    string::size_type str_pose;
    string line; 

    ifstream fin("/proc/modules"); 

    while (getline(fin, line))
    {
        str_pose = line.find("openvswitch");
        if (str_pose != string::npos)
        {
            return 1;
        }
    }
    
    return 0;
}

void print_systime()
{
    time_t st = time(NULL);
    struct tm *pCurTm = NULL;
    struct timeval tva = {0};    

    pCurTm = localtime(&st);
    gettimeofday (&tva, NULL);
    if(pCurTm)
    {   
        PRINTF("local timer: %u.%u.%u %u:%u:%u.%u \n",
            pCurTm->tm_year + 1900,pCurTm->tm_mon + 1,
            pCurTm->tm_mday,
            pCurTm->tm_hour,pCurTm->tm_min,
            pCurTm->tm_sec,(unsigned short)(tva.tv_usec/1000));
    }
}

string GetPhyPortFromBr(string strBrName)
{
    int iRet = 0;
    string strPhyName;
    vector<string> vecIf;
    vector<string> vecPhyIf;
    vector<string> vecBondIf;
    
    iRet = get_ifs_of_bridge(strBrName, vecIf);
    if (iRet)
    {
        cout << "GetPhyPortFromBr(): call get_ifs_of_bridge(" << strBrName << ") failed" << endl;
        return strPhyName;
    }
     
    iRet = get_nics(vecPhyIf);
    if (iRet)
    {
        cout << "GetPhyPortFromBr(): call get_nics() failed" << endl;
        return strPhyName;
    }

    iRet = get_bondings(vecBondIf);
    if (iRet)
    {
        cout << "GetPhyPortFromBr(): call get_bondings() failed" << endl;
        return strPhyName;
    }
     
    vector<string>::iterator itrLstIf = vecIf.begin();
    for ( ; itrLstIf != vecIf.end(); itrLstIf++)
    {
        if (find(vecPhyIf.begin( ), vecPhyIf.end( ), *itrLstIf) != vecPhyIf.end())
        {
            strPhyName = *itrLstIf;
            return strPhyName;
        }

        if (find(vecBondIf.begin( ), vecBondIf.end( ), *itrLstIf) != vecBondIf.end())
        {
            strPhyName = *itrLstIf;
            return strPhyName;
        }
    }

    return strPhyName;
}


int32 VNetCheckIsBr(const string &strDevName)
{
    int ret;
    struct stat filestat;

    string bridge = "/sys/class/net/" + strDevName + "/bridge";
    ret = stat(bridge.c_str(),&filestat);
    //凡是带有bridge子目录的都是网桥
    if (ret == 0 && S_ISDIR(filestat.st_mode))
    {
        return 1;
    }
    
    return 0;
}

int32 VNetFormatString(string &strResult, const char* pszFormat, ...)
{
    if(NULL == pszFormat)
    {
        VNET_ASSERT(0);
        return -1;
    }
    
    char acTmp[512];    
    memset(acTmp, 0x0, sizeof(acTmp));
    
    va_list vargs;
    va_start(vargs, pszFormat);
    vsnprintf(acTmp, 512, pszFormat, vargs);
    va_end(vargs);
    
    strResult.assign(acTmp);
    return 0;
}

/*
* caculate mac or ip to call this function;
* strUUID.size() == 16
* sizeof(aucValue) == 16
*/
int32 VNetGetUUIDValue(BYTE aucValue[], BYTE ucValueSz)
{
    uuid_t tUUID;
    if(sizeof(tUUID) != ucValueSz)
    {
        VNET_LOG(VNET_LOG_WARN, "VNetUUID2BYTE:UUID Len error:uuid_t len %d, aucValue len %d.\n", 
            sizeof(tUUID), ucValueSz);
        return -1;
    }    
    uuid_generate(tUUID);
    BYTE *pData = (BYTE*)&tUUID;
    for(int i = 0; i < ucValueSz; i++)
    {
        aucValue[i] = pData[i];
    }
    return 0;
}

void VNetDbgTestGetUUID(void)
{
    BYTE aucValue[16];
    VNetGetUUIDValue(aucValue);
    for(int i = 0; i < 16; i++)
    {
        cout << "0x" << hex << setfill('0') << setw(2) << (int32) aucValue[i] << " ";
    }
    cout.unsetf(ios::hex);
    cout << endl;
    return ;
}
DEFINE_DEBUG_FUNC(VNetDbgTestGetUUID);


/************* end ***********************/

