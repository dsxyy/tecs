/*******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
*
* 文件名称：if.cpp
* 文件标识：见配置管理计划书
* 内容摘要：网口相关操作函数封装实现文件
* 当前版本：1.0
* 作    者：袁进坤
* 完成日期：2011年9月26日
*
* 修改记录1：
*     修改日期：2011/09/26
*     版 本 号：V1.0
*     修 改 人：袁进坤
*     修改内容：创建
*******************************************************************************/
#include "exception.h"
#include "sys.h"
#include "net/if.h"
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>
#include <iostream>
#include <sstream>
#include <fstream> 
#include <string>
#include <vector>
#include <dirent.h>
#include <linux/hdreg.h> 
#include <scsi/sg.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <algorithm>
#include <netdb.h>
#include <sys/socket.h>
#ifdef FROMTECS
#include "vm_driver_simulate.h"
#include "phy_port_pub.h"
#endif
#include "misc.h"

typedef int STATUS;
#define SUCCESS                             0          //成功
#define ERROR                               1          //未知错误
#define READ_BOND_BUFFER                  500         //500足够用
#define BOND_SHELL_PATH                   "/opt/tecs/hc/bin/do_bond.sh"
#define BOND_OPT_ADD                      "add"
#define BOND_OPT_DEL                      "del"
#define BOND_OPT_SET                      "set"
#define BOND_OPT_SRIOV                    "sriov"

#define VNET_PLUGIN_ERROR                  1
#define VNET_PLUGIN_SUCCESS                0

STATUS removeendl(string &s);
STATUS put_string_to_vec(string text,vector<string>& vec);

STATUS get_nics(vector<string>& nics);
STATUS get_bridges(vector<string>& bridges);
STATUS get_ifs_of_bridge(const string& bridge, vector<string>& ifs);
STATUS get_sriov_port(vector<string>& sriovs);
STATUS get_sriov_pci(const string &nicname, map<string,string>&vif_pci);
STATUS get_sriov_num(const string &nicname, int &num);

STATUS get_bondings(vector<string>& bondings);
STATUS get_bond_para(const string bond,const string para,string& value);
STATUS add_bond(const string bond,const string mode, vector<string> ifs);
STATUS del_bond(const string bond);
STATUS set_bond_para(const string bond, const string para,const string value);

STATUS send_ioctl(const char* ifname, struct private_value *pval)
{
    struct ifreq ifr;
    int fd, ret;
    
    /* Setup our control structures. */
    strcpy(ifr.ifr_name, ifname);
    
    /* Open control socket. */
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) 
    {
        return -1;
    }

    ifr.ifr_data = (caddr_t)pval;
    ret = ioctl(fd, SIOCDEVPRIINFO, &ifr);  
    if(ret)
    {
        close(fd);
        return -1;
    } 
    
    close(fd);
    return 0;
}

static int IfReq(const char* ifname,struct ifreq *pifr)
{
    int sockfd;
    if (-1 == (sockfd = socket(AF_INET, SOCK_DGRAM, 0)))
    {
        return -1;
    }

    bzero(pifr->ifr_name, sizeof(pifr->ifr_name));
    strncpy(pifr->ifr_name, ifname, sizeof(pifr->ifr_name)-1);
    if(-1 == ioctl(sockfd, SIOCGIFADDR, pifr))
    {
        close(sockfd);
        return -1;
    }

    close(sockfd);
    return 0;
}

STATUS GetIpAddress(const string&if_name,uint32 &ip_address)
{
    if (if_name.empty())
    {
        return ERROR_INVALID_ARGUMENT;
    }

    struct ifreq ifr;
    if(0 != IfReq(if_name.c_str(),&ifr))
    {
        return ERROR;
    }
    
    ip_address = ntohl(inet_addr(inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr)));
    return SUCCESS;
}

STATUS GetIpAddress(const string&if_name,string &ip_address)
{
    if (if_name.empty())
    {
        return ERROR_INVALID_ARGUMENT;
    }
    
    struct ifreq ifr;
    if(0 != IfReq(if_name.c_str(),&ifr))
    {
        return ERROR;
    }
    
    ip_address= inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);
    return SUCCESS;
}
bool IsValidIp(const string& ip)
{
    if (ip.empty())
    {
        SkyAssert(false);
        return false;
    }

    if (ip == "localhost")
    {
        return true;
    }

    int domain;
    unsigned char buf[sizeof(struct in6_addr)];    

    if (string::npos != ip.find(".",0))
    {
        domain = AF_INET;
    }
    else if (string::npos != ip.find(":",0))
    {
        domain = AF_INET6;
    }
    else
    {
        return false;
    }
    if (inet_pton(domain,ip.c_str(),buf) <= 0)
    {
        return false;
    }
    return true;
}
bool IsLocalIp(const string& ip)
{
    #define MAX_IP_LEN 64
    bool bRet = false;
    int iSock = -1;
    int domain;
    char myip[MAX_IP_LEN] = {'\0'};

    if (ip.empty())
    {
        SkyAssert(false);
        return false;
    }

    if (ip == "127.0.0.1" || ip == "localhost")
    {
        return true;
    }

    if (string::npos != ip.find(".",0))
    {
        domain = AF_INET;
    }
    else if (string::npos != ip.find(":",0))
    {
        domain = AF_INET6;
    }
    else
    {
        return false;
    }
    if((iSock = socket(domain, SOCK_DGRAM, 0)) < 0)
    {
        SkyAssert(false);
        return false;
    }

    struct sockaddr_in serv;
    memset(&serv, 0, sizeof(serv));
    serv.sin_family = domain;
    serv.sin_addr.s_addr = inet_addr(ip.c_str());
    serv.sin_port = htons(1000);
    int err = connect(iSock, (const struct sockaddr*) &serv, sizeof(serv));
    if (-1 == err)
    {
        SkyAssert(false);
        close(iSock);
        return false;
    }

    struct sockaddr_in name;
    socklen_t namelen = sizeof(name);
    err = getsockname(iSock, (struct sockaddr*) &name, &namelen);
    if (-1 == err)
    {
        SkyAssert(false);
        close(iSock);
        return false;
    }
    inet_ntop(domain, &name.sin_addr, myip, MAX_IP_LEN);
    myip[MAX_IP_LEN-1] = 0;

    if (0 == strncmp(myip,ip.c_str(),MAX_IP_LEN))
    {
        bRet = TRUE;
    }

    if (-1 != iSock)
    {
        close(iSock);
    }
    return bRet;
}
bool IsLocalUrl(const string& url)
{
    if (url.empty())
    {
        return false;
    }
    if (IsValidIp(url))
    {
        return IsLocalIp(url);
    }
    char host_name[256] = {0};
    gethostname(host_name,sizeof(host_name)-1);
    if (0 != url.compare(host_name))
    {
        string cmd = "grep "+ url + " /etc/hosts | awk '{print $1}'";
        vector<string> res;
        RunCmd(cmd,res);
        if (0 == res.size())
        {//hosts配置文件中没找到
            return false;
        }
        else
        {
            return IsLocalIp(res[0]);
        }
    }
    return true;
}
STATUS GetIpByConnect(const string &dest_url, string &ip_address)
{
    #define MAX_IP_LEN 64
    char    tmp_ip[MAX_IP_LEN] = {'\0'};
    string  dest_ip;
    bool    is_ip = false;    
    int iSock = -1;

    if (dest_url.empty())
    {
        SkyAssert(false);
        return ERROR;
    }

    is_ip = IsValidIp(dest_url);

    if (false == is_ip) //传入的是域名 
    {
         struct hostent *hptr = NULL;
            
         hptr = gethostbyname(dest_url.c_str());
         if ((NULL == hptr)
              ||((AF_INET   != hptr->h_addrtype )
                &&(AF_INET6 != hptr->h_addrtype )))
         {              
             return ERROR;
         }

         inet_ntop(hptr->h_addrtype, hptr->h_addr, tmp_ip, sizeof(tmp_ip));
         tmp_ip[MAX_IP_LEN-1] = 0;    
         dest_ip.assign(tmp_ip);
    }
    else
    {
        dest_ip = dest_url;
    }
    
    if((iSock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        SkyAssert(false);
        close(iSock);
        return ERROR;
    }

    struct sockaddr_in serv;
    memset(&serv, 0, sizeof(serv));
    
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = inet_addr(dest_ip.c_str());
    serv.sin_port = htons(1000);
    int err = connect(iSock, (const struct sockaddr*) &serv, sizeof(serv));
    if (-1 == err)
    {
        /*printf("socket err %d\r\n", errno);*/
        // 如果路由没有配置。
        ip_address = "127.0.0.1";
        if (errno != ENETUNREACH)
        {
            SkyAssert(false);
        }
        close(iSock);        
        return SUCCESS;
    }

    struct sockaddr_in name;
    socklen_t namelen = sizeof(name);
    err = getsockname(iSock, (struct sockaddr*) &name, &namelen);
    if (-1 == err)
    {
        SkyAssert(false);
        close(iSock);
        return ERROR;
    }
    
    memset(tmp_ip, 0, sizeof(tmp_ip));
    inet_ntop(AF_INET, &name.sin_addr, tmp_ip, sizeof(tmp_ip));
    tmp_ip[MAX_IP_LEN-1] = 0;
    
    ip_address.assign(tmp_ip);

    if (-1 != iSock)
    {
        close(iSock);
    }
    return SUCCESS;
}

STATUS GetIpByConnectBroker(const string &borker_url, string &ip_address)
{
    STATUS  tRet = ERROR;
    
    string DestUrl("8.8.8.8");
    
    if (borker_url.empty() || borker_url == "127.0.0.1" || borker_url == "localhost")
    {    //此时借用google dns地址，尝试获取本地默认路由地址
          DestUrl="8.8.8.8";
    }
    else
    {
         DestUrl = borker_url;
    }
     
    tRet = GetIpByConnect(DestUrl,ip_address);
    
    return tRet;
}
/* bond查询接口返回都含有"\n",需要去掉 */
STATUS removeendl(string &s)
{
    //cout << "before remove string is "<< s<<endl;
    if(s.length() ==0)
    {
       //cout << "s.length() == 0" <<endl;
       return ERROR;
    }
    s.erase(s.length()-1,s.length()-1);
    //cout << "after remove string is "<<s<<endl;
    return SUCCESS;
}

void print_vector_string(const vector<string>& strs)
{
    vector<string>::const_iterator it;
    for(it = strs.begin(); it != strs.end(); it ++)
       cout << *it <<endl;
}

/* 将一个string 以空格分隔放入vector */
STATUS put_string_to_vec(string text,vector<string>& vec)
{   
    vector<string>::iterator itor;
    string word;
    while(1)                
    {               
        int pos = text.find(' ');
        if( pos==0)
        {       
            text=text.substr(1);
            continue;
        }       
        if( pos<0)
        {
           //cout << "will push_back "<<text<<endl;
            vec.push_back(text);
            break;
        }       
        word = text.substr(0,pos);
        text = text.substr(pos+1);
        vec.push_back(word);
    }
    for(itor = vec.begin(); itor!= vec.end();itor++ )
    {
        //cout << "vec item is "<<*itor <<endl;
    }
    return 0;
}

STATUS get_nics(vector<string>& nics)
{
    struct dirent *entry = NULL;
    DIR *dp = NULL;
    string sys_class_net("/sys/class/net");
    dp = opendir(sys_class_net.c_str());
    if (dp == NULL) 
    {
        return ERROR;
    }
 
    string filename;
    while ((entry = readdir(dp)))
    {
        if(entry->d_type != DT_LNK)
        {
            continue;
        }
        
        char buf[256] = {'\0'};
        filename = sys_class_net + "/" + string(entry->d_name);
        readlink(filename.c_str(),buf,sizeof(buf));
        //cout << "file " << filename << " is a link of " << buf << endl;
        //凡是指向../../devices/virtual/net/目录的软链接都不是物理网口
        string soft_link(buf);
        #ifdef FROMTECS
        if (SimulateIsExist())
        {
            if(string::npos!=soft_link.find("/devices/vif",0))
            {
                //cout << string(entry->d_name) << " is a physical device!" << endl;
                nics.push_back(string(entry->d_name));
            }   
            
        }
        else
        #endif
        {
            if((string::npos!=soft_link.find("/devices/pci",0))||
             (IsLoopDevice(string(entry->d_name))))
            {
                //cout << string(entry->d_name) << " is a physical device!" << endl;
                nics.push_back(string(entry->d_name));
            }  
        }
   
    }
    closedir(dp);
    return SUCCESS;
}

STATUS get_bridges(vector<string>& bridges)
{
    int ret;
    struct dirent *entry = NULL;
    DIR *dp = NULL;
    string sys_class_net("/sys/class/net");
    dp = opendir(sys_class_net.c_str());
    if (dp == NULL) 
    {
        perror("opendir: Path does not exist or could not be read.");
        return ERROR;
    }
 
    string filename;
    while ((entry = readdir(dp)))
    {
        if(string(".") == string(entry->d_name) ||string("..") == string(entry->d_name)  )
        {
            continue;
        }
        
        string bridge = sys_class_net + "/" + string(entry->d_name) + "/bridge";
        struct stat filestat;
        ret = stat(bridge.c_str(),&filestat);
        //凡是带有bridge子目录的都是网桥
        if(ret == 0 && S_ISDIR(filestat.st_mode))
        {
            //cout << string(entry->d_name) << " is a bridge!" << endl;
            bridges.push_back(string(entry->d_name));
        }
    }
    closedir(dp);
    return SUCCESS;
}

STATUS get_ifs_of_bridge(const string& bridge, vector<string>& ifs)
{
    struct dirent *entry = NULL;
    DIR *dp = NULL;
    string sys_class_net("/sys/class/net");
    //网桥的brif子目录列出了插入该网桥的网口名称
    string brif = sys_class_net + "/" + bridge + "/brif";
    string brif_bk = sys_class_net + "/" + bridge + "/brif";
    dp = opendir(brif.c_str());
    if (dp == NULL) 
    {
        perror("opendir: Path does not exist or could not be read.");
        return ERROR;
    }
 
    string filename;
    while ((entry = readdir(dp)))
    {
        if(entry->d_type != DT_LNK)
        {
            continue;
        }

        //cout << "bridge " << bridge << " has a member interface: " << string(entry->d_name) << endl;
        ifs.push_back(string(entry->d_name));
    } 
    closedir(dp);
    return SUCCESS;
}

STATUS get_bridge_of_nic(const string& nic, string& bridge)
{
    vector<string> bridges;
    get_bridges(bridges);
    vector<string> ifs;
    vector<string>::iterator it_bridge=bridges.begin();
    for (; it_bridge!= bridges.end(); ++it_bridge )
    {
        get_ifs_of_bridge(*it_bridge,ifs);
        vector<string>::iterator it_ifs=ifs.begin();
        for (; it_ifs!= ifs.end(); ++it_ifs )
        {
            if(*it_ifs == nic)
            {
                bridge = *it_bridge;
                return SUCCESS;
            }
        }
    }
    bridge = "";
    return SUCCESS;
}

/* ****************接口说明*********************************
参数 : vector<string> sriovs  
返回 : STATUS 成功则SUCEESS
              失败则ERROR
说明 : 将所有SRIOV网卡名填入到vector<string> sriovs              
************************************************************
*/
STATUS get_sriov_port(vector<string>& sriovs)
{
    struct dirent *entry = NULL;
    DIR *dp = NULL;
    vector<string> nics;
    vector<string>::iterator nics_itor;
    int ret;
    
    sriovs.clear();
    ret = get_nics(nics);
    if (ret != SUCCESS)
    {
        perror("get_nics failed!");
        return ERROR;
    }
    for(nics_itor = nics.begin(); nics_itor != nics.end() ; nics_itor ++)
    {
        string sys_class_net_eth = string("/sys/class/net/") + string(*nics_itor) + "/device";
        //cout << "will opendir "<<sys_class_net_eth <<endl;
        if(NULL != dp)
        {
            closedir(dp);
        }
        dp = opendir(sys_class_net_eth.c_str());
        if (dp == NULL) 
        {
            continue;
        }
        while ((entry = readdir(dp)))
        {
            if(entry->d_type != DT_LNK)
            {
                continue;
            }
            
            if(strstr(entry->d_name,"virtfn"))
            {
                //cout << "push_bask " <<string(*nics_itor)<<endl;
                sriovs.push_back(string(*nics_itor));
                break;
            }
        } 
    }
    
    if(NULL != dp)
    {
        closedir(dp);
    }
    
    return SUCCESS;
}

/* ****************接口说明*********************************************************
参数 : map<string,string>&vif_pci  PCI信息输出map,map 为<virtfn0 -> ../0000:82:10.0>
       string nicname              需要获取PCI信息的网卡名
返回 : STATUS 成功则SUCEESS
              失败则ERROR
说明 : 
       将pci信息写入map   注意如 viftn0->../0000:82:10.   后一段去除了../ 便于上层解析   
************************************************************************************
*/
STATUS get_sriov_pci(const string &nicname, map<string,string>&vif_pci)
{
    struct dirent *entry = NULL;
    DIR *dp = NULL;
    string str_cmd= "";
    string sys_class_net_eth=string("/sys/class/net/")+string(nicname)+ "/device";
    string filename;
    string pci;
    char   buf[256] = {0};

    dp = opendir(sys_class_net_eth.c_str());
    if (dp == NULL) 
    {
        perror("get_sriov_pci: Path does not exist or could not be read.");
        return ERROR;
    }
    vif_pci.clear();

    while ((entry = readdir(dp)))
    {
        if(entry->d_type != DT_LNK)
        {
            continue;
        }
        if(strstr(entry->d_name,"virtfn"))
        {
            filename = sys_class_net_eth + "/" + string(entry->d_name);
            readlink(filename.c_str(),buf,sizeof(buf));
            pci.assign(buf);
            /* pci.substr(3)去除前面3个字节的,便于上层解析*/
            vif_pci.insert(make_pair(string(entry->d_name), pci.substr(3)));
            memset(buf,0,sizeof(buf));
        }
    }
    closedir(dp);
    return SUCCESS;
}

/* ****************接口说明*********************************************************
参数 : int &num                    指定的SRIOV虚拟网卡的个数
       string nicname              需要获取虚拟网卡个数信息的网卡名
返回 : STATUS 成功则SUCEESS       
              失败则ERROR          非SRIOV网卡
说明 : 
************************************************************************************
*/
STATUS get_sriov_num(const string &nicname, int &num)
{
    struct dirent *entry = NULL;
    DIR *dp = NULL;
    string str_cmd= "";
    num =0 ;
    string sys_class_net_eth=string("/sys/class/net/")+string(nicname)+ "/device";
    dp = opendir(sys_class_net_eth.c_str());
    if (dp == NULL) 
    {
        /* 非SRIOV网卡 */
        return ERROR;
    }

    while ((entry = readdir(dp)))
    {
        if(entry->d_type != DT_LNK)
        {
            continue;
        }
        if(strstr(entry->d_name,"virtfn"))
        {
            num++;
        }
    }
    closedir(dp);
    return SUCCESS;
}

//检查是否创建的loopback设备
BOOL IsLoopDevice(const string &port_name)
{
    string bridge;
    string line;
    string nic;
    string file("/etc/loopback/vbridge.conf");

    //获取桥名
    get_bridge_of_nic(port_name, bridge);
    
    if(!bridge.compare(""))
    {
        return FALSE;
    }

    ifstream fin(file.c_str());
    if(fin.is_open())
    {
        while(getline(fin, line))
        {
            istringstream istr(line);
            istr>>nic;
            if(!nic.compare(bridge))
            {
                //文件中已有该设备，则为loop设备
                return TRUE;
            }
        }  
    }
    return FALSE;
}

/* ****************************  接口说明  *************************************
函数功能: 设置网卡的环回模式，目前仅支持SRIOV网卡环回模式设置
输入参数: port_name 物理网卡名称
          lpflag    环回标记，1--设置，0--取消
返回值  : SUCEESS   设置生效
          ERROR     设置失败
*******************************************************************************/
STATUS set_port_loopback(const string &port_name, const uint32 &lpflag)
{
    struct private_value eval;
    string line;
    string nic;
    string file("/etc/loopback/sriov.conf");

    if(port_name.empty())
    {
        return ERROR;
    }

    eval.cmd  = PRIVATE_SETLOOP;
    eval.data = lpflag; 
    if(0 != send_ioctl(port_name.c_str(), &eval))
    {
        perror("set_port_loopback: send ioctl failed.\n");
        return ERROR;
    }
    
    /* 写到文件中，刀片重启时由脚本自动设置，文件中格式如下:
       eth0 loopback mode
       eth1 loopback mode  
    */
    if(!lpflag)
    {
        //取消还回时，删除文件中一行
        string cmd = "sed -i /" + port_name + "/d " + file;
        if(0 != system(cmd.c_str()))
        {
            perror("set_port_loopback: run cmd fail!!");
            return ERROR;
        }
    }
    else
    {
        ifstream fin(file.c_str());
        if(!fin.is_open())
        {
            perror("set_port_loopback: output </etc/loopback/sriov.conf> failed.\n");
            return ERROR;
        }

        while(getline(fin, line))
        {
            istringstream istr(line);
            istr>>nic;
            if(!nic.compare(port_name))
            {
                //已经存在，不再写文件 
                return SUCCESS;
            }
        }
        //将配置写文件中
        ofstream fout(file.c_str(), ios::app); 
        if (fout.fail() == true)
        {
            perror("set_port_loopback: input </etc/loopback/sriov.conf> failed.\n");
            return ERROR;
        } 

        fout<<port_name<<" loopback mode"<<endl;
    }
    return SUCCESS;
}

#ifdef FROMTECS
/* ****************************  接口说明  *************************************
函数功能: 查询网卡环回属性，创建的tap设备也定义为环回设备.
输入参数: port_name 物理网卡名称
输出参数: lpflag    环回标记，1--环回模式，0--非环回模式
返回值  : SUCEESS   设置生效
          ERROR     设置失败
*******************************************************************************/
STATUS get_port_loopback_mode(const string &port_name, int32 &lpflag)
{
    struct private_value eval;
    string line;
    string nic;
    string file("/etc/loopback/sriov.conf");
    
    if(port_name.empty())
    {
        return ERROR;
    }
    
    lpflag = PORT_LOOPBACK_NO;

    if(IsLoopDevice(port_name))
    {
        lpflag = PORT_LOOPBACK_YES;
        return SUCCESS;
    }

    eval.cmd = PRIVATE_GETLOOP;
    if(0 != send_ioctl(port_name.c_str(), &eval))    
    {
        return ERROR;
    }
    else
    {
        lpflag = eval.data ? PORT_LOOPBACK_YES : PORT_LOOPBACK_NO;
        
        /* 底层环回属性未设置，需要进一步检查文件是否记录环回，如有记录需要重新设置环回，
           以保持文件和底层一致性，防止人为影响环回属性，如输入ifconfig down。 */        
        if(!lpflag)
        {
            ifstream fin(file.c_str());
            if(!fin.is_open())
            {
                return ERROR;
            }

            while(getline(fin, line))
            {
                istringstream istr(line);
                istr>>nic;
                if(!nic.compare(port_name))
                {
                    //已经存在，重新设置环回
                    eval.cmd  = PRIVATE_SETLOOP;
                    eval.data = PORT_LOOPBACK_YES; 
                    if(0 != send_ioctl(port_name.c_str(), &eval))    
                    {
                        perror("get_port_loopback_mode: send ioctl again failed.\n");
                        return ERROR;
                    }  
                    lpflag = PORT_LOOPBACK_YES;
                    return SUCCESS;
                }
            } 
        }

        return SUCCESS;
    }
}
#endif

int32 bond_mode_stringtoint(const string& mode)
{
    return 4;
}
string bond_mode_inttostring(const int& mode)
{
    return "4";
}

/* ****************接口说明*********************************
参数 : vector<string> bondings  
返回 : STATUS 成功则SUCEESS
              失败则ERROR
说明 : 将所有SRIOV网卡名填入到vector<string> bondings     
************************************************************
*/
STATUS get_bondings(vector<string>& bondings)
{
    struct dirent *entry = NULL;
    DIR *dp = NULL;
    string proc_net_bonding("/proc/net/bonding");
    dp = opendir(proc_net_bonding.c_str());
    if (dp == NULL) 
    {
        return SUCCESS;
    }
 
    string filename;
    while ((entry = readdir(dp)))
    {
        if(entry->d_type == DT_LNK)
        {
            continue;
        }
        if(string(".") == string(entry->d_name) ||string("..") == string(entry->d_name)  )
        {
            continue;
        }
        //cout <<entry->d_name<<" is bonding"<< endl;
        bondings.push_back(string(entry->d_name));
    }
    closedir(dp);
    return SUCCESS;
}

/*****************接口说明**************************************************
参数 : string bond  需要获取参数的bond网卡
       string para  参数名
       string value 参数值
       
返回 : STATUS 成功则SUCEESS
              失败则ERROR
说明 : 内部接口，对外不调用，只给get_bond_info调用
       使用该函数需要注意判断返回值，返回值非0的时候 vale值不一定是准确的
       获取bond口的参数para写入value  
***************************************************************************
*/
STATUS get_bond_para(const string bond,const string para,string& value)
{
    int ret;
    string sys_class_net("/sys/class/net");
    string bond_para = sys_class_net + "/" + bond + "/bonding/" +para;
    int handle;
    char buffer[READ_BOND_BUFFER]={0} ; 

	//cout << "path is "<<bond_para<<endl;
    handle = open(bond_para.c_str(),O_RDONLY);

	if (handle  == -1)
	{
	    //cout <<"Error opening file "<<bond_para<<endl;
	    return ERROR; 
	} 
    ret = read(handle,buffer,READ_BOND_BUFFER);
	if(ret > 0)
	{
	    //cout <<"successful read file "<<bond_para<<endl;
		//cout <<"buf is "<<buffer<<endl;
	}
	else
	{
            close(handle);
	    //cout <<"failed read file "<<bond_para<<endl;
	    return ERROR;
	}
    close(handle);
    if(para == "mode")
    {
        if(strstr(buffer,"balance"))
        {
            value.assign("0\n");
        }
        if(strstr(buffer,"active-backup"))
        {
            value.assign("1\n");
        }
        if(strstr(buffer,"802"))
        {
            value.assign("4\n");
        }
    }
    else
    {
	    value.assign(buffer);
	}
	removeendl(value);
    
    return 0;
}

/* ****************接口说明*********************************************************
参数 : string bond             需要获取参数的bond网卡
       string mode             模式
       string miimon           链路检测时间
       string lacp_rate        LACP报文发送速率,注意该值和Linux驱动相关，不一定准确
       string mii_status       链路状态，取值为 up , down
       vector<string> slaves   成员口
       
返回 : STATUS 成功则SUCEESS
              失败则ERROR
              
说明 : 该函数为了通用，目前对get_bond_para没有判断返回值，因为很多值和模式相关，暂时先这样，
       修改为判断返回值的，需要和上层调用协商，后续可以优化
*********************************************************************************
*/
STATUS get_bond_info(const string bond,string& mode,string& miimon,
                     string& lacp_rate, string& mii_status, vector<string>& slaves)
{
    string str_slaves;
    get_bond_para(bond,"mode",mode);
    get_bond_para(bond,"miimon",miimon);
    get_bond_para(bond,"lacp_rate",lacp_rate);
    get_bond_para(bond,"mii_status",mii_status);
    get_bond_para(bond,"slaves",str_slaves);
    put_string_to_vec(str_slaves,slaves);
    return SUCCESS;
}

/* ****************接口说明*********************************************************
参数 : string bond             需要增加的bond网卡
       string mode             模式
       vector<string> ifs      成员口
       
返回 : STATUS 成功则SUCEESS
              失败则ERROR
              
说明 :  目前只能初始化为模式4
        注意返回失败则一定是创建bond失败
        返回成功则不一定是成功
************************************************************************************
*/
STATUS add_bond(const string bond, const string mode, vector<string> ifs)
{
    string str_cmd= string(BOND_SHELL_PATH);	
	vector<string> bondings;
	vector<string>::iterator itor;
	int ret;
	string ifs_num;
	char tmp[10];

    if(ifs.size() == 0)
    {
        perror("slaves is null!");
        return ERROR;
    }
    if(string(mode) != string("4"))
    {
        perror("should set mode 4 first,then modify!");
	    return ERROR;
    }
    /* 首先判断输入的bond口名称是否存在 */
    ret = get_bondings(bondings);
    if(ret != 0)
    {
	    perror("get_bondings failed!");
        return ERROR;
    }
    if(find(bondings.begin(), bondings.end(), bond) != bondings.end())
    {
	    return SUCCESS;
    }
    sprintf(tmp,"%d",ifs.size());
    ifs_num.assign(tmp);
		
    str_cmd = str_cmd + " " + string(BOND_OPT_ADD)+ " " +bond+" " + mode +" "+ ifs_num+" ";
    for(itor=ifs.begin();itor!=ifs.end();itor++)
    {
        if(itor!=ifs.end()-1)
	    {
	        str_cmd =str_cmd + string(*itor) +"#";
	    }
	    else
	    {
            str_cmd =str_cmd + string(*itor);
	    }
    }
    ret = system(str_cmd.c_str());
    if(ret !=0 )
    {
	    perror("system add_bond failed!");
	    return ERROR;
    }
    return SUCCESS;
}

/*****************接口说明**********************************************************
参数 : string bond             需要增加的bond网卡
       vector<string> ifs      成员口
       
返回 : STATUS 成功则SUCEESS
              失败则ERROR
              
说明 :  注意返回失败则一定是创建bond失败
        返回成功则不一定是成功
************************************************************************************
*/
STATUS del_bond(const string bond)
{
    string str_slaves;
    string str_cmd= string(BOND_SHELL_PATH);	
    vector<string> bondings;
    vector<string> vec_slaves;
    vector<string>::iterator itor;
    int ret;

    /* 首先判断输入的bond口名称是否存在 */
    ret = get_bondings(bondings);
	if(ret != 0)
	{
	    perror("get_bondings failed!");
		return ERROR;
	}
	if(find(bondings.begin( ), bondings.end( ), bond) != bondings.end())
	{/* 找到了才处理删除操作 */
	    str_cmd = str_cmd +	" " +string(BOND_OPT_DEL)+ " " +bond + " ";
        
        ret = get_bond_para(bond ,"slaves",str_slaves); 
		if(ret == ERROR)
		{
		    perror("get_bond_para failed!");
			return ERROR;
    	}
		put_string_to_vec(str_slaves,vec_slaves);

		for(itor=vec_slaves.begin();itor!=vec_slaves.end();itor++)
		{
		    if(itor!=vec_slaves.end()-1)
		    {
		        str_cmd =str_cmd + string(*itor) +"#";
		    }
			else
			{
			    str_cmd =str_cmd + string(*itor);
			}
		}
		
	    ret = system(str_cmd.c_str());
		if(ret !=0)
		{
		    perror("del_bond failed!");
			return ERROR;
		}
	    return SUCCESS;
	}
	else
	{
	    cout << "cant find" << bond <<endl;
		return ERROR;
	}
    
}

/*****************接口说明**********************************************************
参数 : string bond             需要设置参数的bond网卡
       para                    参数名
       value                   参数值
       
返回 : STATUS 成功则SUCEESS
              失败则ERROR
              
说明 :  
************************************************************************************
*/
STATUS set_bond_para(const string bond, const string para,const string value)
{
    string str_cmd= string(BOND_SHELL_PATH);	
    int ret;

    str_cmd = str_cmd +	" " + string(BOND_OPT_SET) + " " + bond + " "+para + " " +value;
    ret = system(str_cmd.c_str());
	if(ret !=0 )
	{
	    
		return ERROR;
	}
    return SUCCESS;
}

static STATUS get_bond_lacp_status(const string & bond_content, CBondInfo & info)
{
    info.lacp.status = FALSE;

    string::size_type  is_found = 0;

    // 聚合失败的情况下，没有Active Aggregator Info 
    is_found = bond_content.find("Active Aggregator Info" );
    if( is_found == string::npos )
    {
        return SUCCESS;    
    }
    info.lacp.status = TRUE;
    
    return SUCCESS;
}


/*
    以symbol为分割，获取所有item
*/
static STATUS get_bond_vectoritem(const string & content,const char* symbol,vector<string> & out)
{
    string::size_type first_item = 0;
    string::size_type next_item = 0;
    string::size_type tmp_item = 0;
    
    if( NULL == symbol)
    {
        return ERROR;
    }
    
    string item("");
    
    first_item = content.find(symbol);
    while(first_item !=  string::npos )
    {
        item = "";

        tmp_item = first_item + strlen(symbol);

        next_item = content.find(symbol,tmp_item);
        if( next_item == string::npos )
        {
            item.assign( content, first_item, content.size()  - first_item);
        }
        else
        {
            item.assign( content, first_item, next_item  - first_item);
        }

        out.push_back(item);
        first_item = next_item;
    }
     
    return SUCCESS;
}


// 获取具体item 值
static STATUS get_bond_item(const string & content,const char* symbol, const char * split,string & outValue)
{
    string::size_type begin_item = 0;
    string::size_type end_item = 0;

    if( NULL == symbol || NULL == split)
    {
        return ERROR;
    }

    begin_item = content.find(symbol);
    if( begin_item == string::npos )
    {
        return ERROR;
    }

    begin_item += strlen(symbol);

    end_item = content.find(split,begin_item);
    if( end_item == string::npos )
    {
        return ERROR;
    }

    outValue.assign(content,begin_item, end_item - begin_item);  
    return SUCCESS;
}


/*****************接口说明**********************************************************
参数 : string bond             需要设置参数的bond网卡
       para                    参数名
       value                   参数值
       
返回 : STATUS 成功则SUCEESS
              失败则ERROR
              
说明 : 聚合成功 必须有Aggregator ID， 以及Partner Mac Address
       聚合成功 成员口 ID相同 端口up : success;
                其他情况               failed.     
************************************************************************************
*/
static STATUS get_bond_lacp_info(const string & bond_content, CBondInfo & info)
{
    int ret = 0;
    // 获取聚合状态
    if( SUCCESS != (ret = get_bond_lacp_status(bond_content,info)))
    {
        return ret;
    }

    // 获取aggr id ,partner mac
    if(info.lacp.status)
    {
        // 首先获取 802.3ad info 信息    
        string s803ad("");
        if( SUCCESS != get_bond_item(bond_content,"802.3ad info\n","\n\n",s803ad) )
        {
            return ERROR;
        }
        
        // 必须加上\n
        s803ad += "\n";
        
        //  Aggregator ID: 3    
        string sAggId("");
        if( SUCCESS != get_bond_item(s803ad,"Aggregator ID: ","\n",info.lacp.aggregator_id) )
        {
            return ERROR;
        }
        
        // Partner Mac Address: 00:01:01:08:01:03
        string sPartneMac("");
        if( SUCCESS != get_bond_item(s803ad,"Partner Mac Address: ","\n",info.lacp.partner_mac) )
        {
            return ERROR;
        }
    }

    //Slave Interface: 
    vector<string> vSlave;
    if( SUCCESS != get_bond_vectoritem(bond_content,"Slave Interface: ",vSlave))
    {
        return ERROR;
    }

    string sItem = "";
    string sNicName;
    string sNicStatus;
    string sNicAggrID; 
    vector<string>::iterator it_slave = vSlave.begin();
    for(; it_slave != vSlave.end(); ++it_slave)
    {
        sNicName = sNicStatus = sNicAggrID = "";
        sItem = *it_slave;
        sItem += "\n";
        // Slave Interface: 
        if( SUCCESS != get_bond_item(sItem,"Slave Interface: ","\n",sNicName))
        {
            return EN_BOND_ERR_GET_BOND_ITEM_FAILED;
        }

        // MII Status: 
        if( SUCCESS != get_bond_item(sItem,"MII Status: ","\n",sNicStatus))
        {
            return EN_BOND_ERR_GET_BOND_ITEM_FAILED;
        }

        // Aggregator ID: 
        if( SUCCESS != get_bond_item(sItem,"Aggregator ID: ","\n",sNicAggrID))
        {
            return EN_BOND_ERR_GET_BOND_ITEM_FAILED;
        }

        if(0 != info.lacp.aggregator_id.compare("") && \
           0 != info.lacp.partner_mac.compare("") && \
           0 != info.lacp.partner_mac.compare("00:00:00:00:00:00") && \
           0 == info.lacp.aggregator_id.compare(sNicAggrID) && \
           0 ==  sNicStatus.compare("up"))
        {
            info.lacp.nic_suc.push_back(sNicName);
        }
        else
        {
            info.lacp.nic_fail.push_back(sNicName);
        }     
    }
    
    return SUCCESS;
}

static STATUS get_bond_backup_info(const string & bond_content,CBondInfo &info)
{
    // Currently Active Slave
    if( SUCCESS != get_bond_item(bond_content,"Currently Active Slave: ","\n",info.backup.active_nic))
    { 
        //存在 没有currently active slave情况
        //return EN_BOND_ERR_LACP_RESULT_INVALID;
    }
        
     //Slave Interface: 
    vector<string> vSlave;
    if( SUCCESS != get_bond_vectoritem(bond_content,"Slave Interface: ",vSlave))
    {
        return ERROR;
    }

    string sItem = "";
    string sNicName;
    vector<string>::iterator it_slave = vSlave.begin();
    for(; it_slave != vSlave.end(); ++it_slave)
    {
        sNicName =  "";
        sItem = *it_slave;
        sItem += "\n";
        // Slave Interface: 
        if( SUCCESS != get_bond_item(sItem,"Slave Interface: ","\n",sNicName))
        {
            return EN_BOND_ERR_GET_BOND_ITEM_FAILED;
        }

        if( 0 == sNicName.compare(info.backup.active_nic.c_str()))
        {
            // current slave 
        }
        else
        {
            info.backup.other_nic.push_back(sNicName);
        }
    }
     
    return SUCCESS;
}

static STATUS get_bond_mode(const string & bond_content,CBondInfo &info)
{
    // 获取模式
    string::size_type  is_found = 0;

    is_found = bond_content.find(STR_BOND_MODE_BACKUP );
    if( is_found != string::npos )
    {
        info.mode = EN_BOND_MODE_BACKUP;    
    }

    is_found = bond_content.find(STR_BOND_MODE_802 );
    if( is_found != string::npos )
    {
        info.mode = EN_BOND_MODE_802;    
    }

    is_found = bond_content.find(STR_BOND_MODE_BALANCE );
    if( is_found != string::npos )
    {
        info.mode = EN_BOND_MODE_BALANCE;    
    }
    
    return SUCCESS;
}

STATUS get_file_content(const string filePath, string & sOut)
{
    if (0 != access(filePath.c_str(), 0))
    {
        return ERROR_FILE_NOT_EXIST;
    }

    std::string s_content("");

    char buff[1024]={0};
    int32 fd = -1;
    int32 i = 0; 

    fd = open(filePath.c_str(),O_RDONLY);
    if( fd < 0 )
    {
        return ERROR;
    }

    while((i = read(fd,buff,sizeof(buff))) > 0) { 

        s_content += buff;
        memset(buff,0,sizeof(buff));
    }
    close(fd); 
    
    sOut = s_content;
    return SUCCESS;
}

/*****************接口说明**********************************************************
参数 : string bond             需要设置参数的bond网卡
       info                    出参(SUCCESS时有效)
                               此bond口信息
返回 : STATUS 成功则SUCEESS
              失败则ERROR
              
说明 :  获取bond信息(此接口目前仅提供给BOND monitor使用)
        仅仅获取backup / lacp模式信息
************************************************************************************
*/
STATUS get_bond_info(const string bond,CBondInfo &info)
{    
    // 判断此bond是否有效
    string filePath("/proc/net/bonding/");
    filePath += bond;
    if (0 != access(filePath.c_str(), 0))
    {
        return EN_BOND_ERR_FILE_NO_EXIST;
    }
    
    // bond 数据清空
    info.Init();
    info.name = bond;

    string bond_content("");
    if( SUCCESS != get_file_content(filePath,bond_content) )
    {
        return  EN_BOND_ERR_GET_BOND_FILE_CONTENT_FAILED;       
    }

    // 获取模式
    if( SUCCESS != get_bond_mode(bond_content,info))
    {
        return EN_BOND_ERR_GET_MODE_FAIL;
    }

    // BACKUP LACP
    switch(info.mode)
    {
        case EN_BOND_MODE_BACKUP:
        {
            return get_bond_backup_info(bond_content,info);
        }
        break;
        case EN_BOND_MODE_802:
        {
            return get_bond_lacp_info(bond_content,info);
        }
        break;
        default:
        {
            return EN_BOND_ERR_MODE_INVALID;
        }
        break;
    }
    // --------------------------------------------------
    return SUCCESS;
}


/*****************接口说明**********************************************************
参数 : string nics             网口名称
       status                  出参(SUCCESS时有效)
                               =0 down; = 1 up
返回 : STATUS 成功则SUCEESS
              失败则ERROR
              
说明 : 获取nic status
************************************************************************************
*/
STATUS get_nic_status(const string nics,int & status)
{
    // ethtool eth1 | grep "Link detected" | awk -F ': ' '{print $2}'
    string cmd("ethtool ");
    cmd += nics;
    cmd += " | grep \"Link detected\" |awk -F ': ' '{print $2}'";
    vector<string> vStatus;
    vector<string>::iterator it_vStatus;
    if( SUCCESS != RunCmd(cmd,vStatus ))
    {
        return ERROR;
    }
    if( vStatus.empty())
    {
        return ERROR;
    }
    it_vStatus = vStatus.begin();
    if( 0 == (*it_vStatus).compare("yes"))
    {
        status = 1;
    }
    else
    {
        status = 0;
    }
    return SUCCESS;
}

// 是否支持BOND(是否安装了BOND)
STATUS IsSupportBond(int &isSupport)
{
    string cmd("lsmod | grep bonding -c");
    vector<string> vResult;
    vector<string>::iterator it;
    if( SUCCESS != RunCmd(cmd,vResult ))
    {
        return ERROR;
    }
    if( vResult.empty())
    {
        return ERROR;
    }
    it = vResult.begin();

    int num = atoi((*it).c_str());
    if( 0 == num )
    {
        isSupport = FALSE;
    }
    else
    {
        isSupport = TRUE;
    }    
    
    return SUCCESS;
}


int GetKernelPortInfo(const string &port_name, string &ip, string &mask)
{
    string cmd("ifconfig ");
    cmd += port_name;
    cmd += " | grep 'inet addr' | awk '{print $2}'";

    vector<string> vResult;
    vector<string>::iterator it_vResult;
    if( VNET_PLUGIN_SUCCESS != RunCmd(cmd, vResult))
    {
        return VNET_PLUGIN_ERROR;
    }
    if(vResult.empty())
    {
        return VNET_PLUGIN_ERROR;
    }

    it_vResult = vResult.begin();

    string cmd1("echo ");
    cmd1 += *it_vResult;
    cmd1 += " | awk -F ':' '{print $2}'";

    vResult.clear();
    if( VNET_PLUGIN_SUCCESS != RunCmd(cmd1, vResult))
    {
        return VNET_PLUGIN_ERROR;
    }
    if(vResult.empty())
    {
        return VNET_PLUGIN_ERROR;
    }

    ip = *(vResult.begin());

    string cmd2("ifconfig ");
    cmd2 += port_name;
    cmd2 += " | grep 'Mask' | awk '{print $4}'";

    vector<string> vResult1;
    vector<string>::iterator it_vResult1;
    if( VNET_PLUGIN_SUCCESS != RunCmd(cmd2, vResult1))
    {
        return VNET_PLUGIN_ERROR;
    }
    if(vResult1.empty())
    {
        return VNET_PLUGIN_ERROR;
    }

    it_vResult1 = vResult1.begin();

    string cmd3("echo ");
    cmd3 += *it_vResult1;
    cmd3 += " | awk -F ':' '{print $2}'";

    vResult1.clear();
    if( VNET_PLUGIN_SUCCESS != RunCmd(cmd3, vResult1))
    {
        return VNET_PLUGIN_ERROR;
    }
    if(vResult1.empty())
    {
        return VNET_PLUGIN_ERROR;
    }

    mask = *(vResult1.begin());

    return VNET_PLUGIN_SUCCESS;
   
}

STATUS GetAllIfsInfo(map<string, string>& ifs_info)
{
#define MAX_IFS 64
    int i = 0;
    int sockfd;
    struct ifconf ifconf;
    struct ifreq *ifreq = (struct ifreq*)malloc(MAX_IFS * sizeof(struct ifreq));
    if (!ifreq)
    {
        perror("malloc error");
        return ERROR;
    }
    memset(ifreq, 0, MAX_IFS * sizeof(struct ifreq));
    // init for ifconf
    ifconf.ifc_len = MAX_IFS * sizeof(struct ifreq);
    ifconf.ifc_buf = (char*)ifreq;
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        free(ifreq);
        perror("socket error");
        return ERROR;
    }
    ioctl(sockfd, SIOCGIFCONF, &ifconf); // 获取所有接口信息
    // 接下来一个一个的获取 IP 地址
    struct ifreq *ifr = ifreq;
    for (i = (ifconf.ifc_len/sizeof(struct ifreq)); i > 0; i--)
    {
        ifs_info.insert(pair<string, string>(ifr->ifr_name,
                        inet_ntoa(((struct sockaddr_in*)&(ifr->ifr_addr))->sin_addr)));
        ifr++;
    }
    free(ifreq);
    return SUCCESS;
}
#if 0
int main()
{
    string para;
	string value;
	string bond = "pniubi";
	string bridge;
	vector<string> vec;
#if 0
	cout << "test get_nics"<<endl;
	cout << "========================"<<endl;
	get_nics(vec);
	
    cout << "test get_bridges"<<endl;
	cout << "========================"<<endl;
	vec.clear();
    get_bridges(vec);

    cout << "test get_bondings"<<endl;
	cout << "========================"<<endl;
    vec.clear();
	get_bondings(vec);

	cout << "test get_ifs_of_bridge"<<endl;
	cout << "========================"<<endl;
    vec.clear();
    cout <<"please enter bridge name:" <<endl;
	cin >>bridge;
    get_ifs_of_bridge(bridge,vec);

    cout << "test set_bond_para"<<endl;
	cout << "========================"<<endl;
	cout <<"please enter bond name:" <<endl;
	cin >>bond;
    cout <<"please enter para name:" <<endl;
	cin >>para;
	cout <<"please enter value :" <<endl;
	cin >>value;
	cout << bond <<" " <<para << " " <<value<<endl;
	set_bond_para(bond,para,value);
   for(int i = 0; i< 1000; i++)
   {
    cout << "test get_bond_para"<<endl;
	cout << "========================"<<endl;
    bond="pbond1";
    para="mode";
	get_bond_para(bond,para,value);
	cout <<para <<" value is "<<value<<endl;
}

   for(int i = 0; i< 1000; i++)
   {
    cout << "test get_bond_para"<<endl;
        cout << "========================"<<endl;
    bond="pbond0";
    para="slaves";
        get_bond_para(bond,para,value);
        cout <<para <<" value is "<<value<<endl;
}
    string mode;
	string miimon;
	string primary;
	string lacp_rate;
	string active_slave;
	string downdelay;
	string mii_status;
   
	vec.clear();	
	cout << "test get_bond_info"<<endl;
	cout << "========================"<<endl;
	cout <<"please enter bond name:"<<endl;
	cin >>bond;
	get_bond_info(bond,mode,miimon,lacp_rate,mii_status,vec);
         vec.push_back("peth8");
         vec.push_back("peth9");
	//vec.push_back("eth2");
	//del_bond("pmomo");
          //  add_bond("lele","4",vec);
       //   set_bond_para("pbond0","mode","4");
        //  set_bond_para("pbond0","mode","2");
        //  set_bond_para("pbond0","mode","4");
        //  set_bond_para("pbond0","mode","0");
        //  set_bond_para("pbond0","mode","1");
        //  set_bond_para("pbond1","mode","4");
        //  set_bond_para("pbond1","mode","1");
        //  set_bond_para("pbond1","mode","0");
       // set_bond_para("pbond0","mode","2");
       // set_bond_para("pbond0","mode","4") ;
        //del_bond("pbond0");
       // del_bond("pab");
        //add_bond("momo","4",vec);
      //  set_bond_para("plele","mode","2") ;
       // set_bond_para("plele","mode","1") ;
       // set_bond_para("plele","mode","4") ;
       
#endif    
    //get_nics(vec);
    //get_sriovs(vec);
    return 0;
}
#endif

