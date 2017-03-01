/******************************************************************************
* Copyright (c) 2011，中兴通讯股份有限公司，All rights reserved.
* 
* 文件名称：vnet_xen_plug.cpp
* 文件标识：
* 内容摘要：vnet_xen_plug类实现文件
* 当前版本：1.0
* 作    者：
* 完成日期：2012年12月21日
*
* 修改记录1：
*     修改日期：2012/12/21
*     版 本 号：V1.0
*     修 改 人：陈志伟
*     修改内容：创建
******************************************************************************/

#include "vnetlib_api.h"
#include <sys/types.h>
#include <sys/wait.h>

//执行linux命令
//输入：cmd-linux命令
//输出：result-命令执行后的输出，按行放入vector<string>中，
//      去掉每行的首位空格和回车符号
//返回值:0-成功，其他-失败
int RunCmd(const string& cmd, vector<string>& result)
{
    char    s[1024] = {0};
    FILE    *fp = NULL; 
    string  str;
    if(cmd.empty())
    {
        return VNET_PLUGIN_ERROR;
    }
   
    if((fp = popen(cmd.c_str(), "r")) == NULL)
    {
        return VNET_PLUGIN_ERROR;
    }

    // 避免FD的继承
    fcntl(fileno(fp), F_SETFD, FD_CLOEXEC);
    while(fgets(s, 1024, fp))
    {   
        str = s;       
        boost::trim(str);//boost库函数，处理后str中无回车符
        if(!str.empty())
        {
            result.push_back(str);      
        }
    }
    pclose(fp);
    return VNET_PLUGIN_SUCCESS;
}

int RunCmdRetString(const string& cmd, string& result)
{
    vector<string> v_result;
    vector<string>::iterator it;
    int32 ret;
    
    ret = RunCmd(cmd,v_result);
    if(VNET_PLUGIN_SUCCESS == ret)
    {
        it = v_result.begin();
        while(it != v_result.end())
        {
            result += (*it + "\n");
            ++it;
        } 
    }
    else
    {
        result="";
    }
    
    return ret;
}


int vnet_chk_dir(const string &strDir)
{
    struct stat tStat = {0};

    if (strDir.empty()) 
    {
        cout<<strDir<<" is empty!"<<endl;
        return VNET_PLUGIN_FALSE;
    }
    
    if(0 != lstat(strDir.c_str(), &tStat) )
    {   
        cout << "lstat "<<strDir<<" failed"<<endl;
        return VNET_PLUGIN_FALSE;
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
                cout << strDir << " is exist"<<endl;
                return VNET_PLUGIN_TRUE;
            }
         }
        cout << strDir << " is not exist"<<endl;
        return VNET_PLUGIN_FALSE;
    }
    cout << strDir << " is exist"<<endl;
    return VNET_PLUGIN_TRUE;
}

int vnet_chk_dev(const string &strDevName)
{
    if(strDevName.empty())
    {
        cout<<strDevName<<" is empty!"<<endl;
        return VNET_PLUGIN_FALSE;
    }
    
    string strTemp = "/sys/class/net/";
    strTemp.append(strDevName);
    strTemp.append("/");
    
    return vnet_chk_dir(strTemp);
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

int vnet_system(const string &strCmd)
{
  string cmd_echo = "echo " + strCmd + " >>/var/log/vnetinfo";
  system(cmd_echo.c_str());
  //return system(strCmd.c_str());
  return vnet_system_tulip(strCmd.c_str());
}

string int2string(const int& intvar)
{
    stringstream strStream;
    strStream << intvar;
    return strStream.str();
}

int  string2int(const string& strvar)
{
    stringstream strStream;
    int retvar;
    strStream << strvar;
    strStream >>retvar;
    return retvar;
}

void replacedstr(string &a,string b)
{    
    int begin=0;  
    begin = a.find(b,begin);  
    while(begin !=-1)  
    {       
        a.replace(begin,1,""); 
        begin=a.find(b,begin); 
    }
}

/* bond查询接口返回都含有"\n",需要去掉 */
int removeendl(string &s)
{
    if(s.length() ==0)
    {
       return VNET_PLUGIN_ERROR;
    }
    s.erase(s.length()-1,s.length()-1);
    return VNET_PLUGIN_SUCCESS;
}

/* 将一个string 以空格分隔放入vector */
int put_string_to_vec(string text,vector<string>& vec)
{   
    if(text.empty())
    {
        return 0;
    }
    
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

int SendIOCtrl(const char* ifname, struct vnet_lib_private_value *pval)
{
    //cout << ifname << " " << pval->cmd << " " << pval->data << endl;

    struct ifreq ifr;
    int fd, ret;
    
    /* Setup our control structures. */
    strcpy(ifr.ifr_name, ifname);
    
    /* Open control socket. */
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) 
    {
        return VNET_PLUGIN_ERROR;
    }

    ifr.ifr_data = (char*)pval;
    ret = ioctl(fd, SIOCDEVPRIINFO, &ifr);  
    if(ret)
    {
        close(fd);
        return VNET_PLUGIN_ERROR;
    } 
    
    close(fd);
    return VNET_PLUGIN_SUCCESS;
}


int GetAllNic(vector<string>& nics)
{
    struct dirent *entry = NULL;
    DIR *dp = NULL;
    string sys_class_net("/sys/class/net");
    dp = opendir(sys_class_net.c_str());
    if (dp == NULL) 
    {
        return VNET_PLUGIN_ERROR;
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
        string soft_link(buf);
        if(string::npos!=soft_link.find("/devices/pci",0))
        {
            nics.push_back(string(entry->d_name));
        }    
    }
    closedir(dp);
    return VNET_PLUGIN_SUCCESS;
}
    
int GetAllVirNic(vector<string>& nics)
{
    int ret;
    struct dirent *entry = NULL;
    DIR *dp = NULL;
    string sys_class_net("/sys/class/net");
    dp = opendir(sys_class_net.c_str());
    if (dp == NULL) 
    {
        return VNET_PLUGIN_ERROR;
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
        string soft_link(buf);
        if(string::npos!=soft_link.find("/devices/virtual",0))
        {
            string bridge = sys_class_net + "/" + string(entry->d_name) + "/bridge";
            struct stat filestat;
            ret = stat(bridge.c_str(),&filestat);
            if(ret == 0 && S_ISDIR(filestat.st_mode))
            {
                //是bridge，则排除
            }
            else
            {
                nics.push_back(string(entry->d_name));
            }
        }    
    }
    closedir(dp);
    return VNET_PLUGIN_SUCCESS;
}

int GetAllVnics(vector<string>& nics)
{
    cout <<"xen has not this cmd for GetAllVnics"<<endl;
    return VNET_PLUGIN_SUCCESS;
}

int GetAllUplinks(vector<string>& uplinks)
{
    cout <<"xen has not this cmd for GetAllUplinks"<<endl;
    return VNET_PLUGIN_SUCCESS;
}

int GetAllUplinksSubnets(const string eth, vector<string>& up_subnets)
{
    cout <<"xen has not this cmd for GetAllUplinksSubnets"<<endl;
    return VNET_PLUGIN_SUCCESS;
}

int DelVnicOfOVS(const string& nic)
{
    cout <<"xen has not this cmd for DelVnicOfOVS"<<endl;
    return VNET_PLUGIN_SUCCESS;
}

int IfconfigIsExist(const string& nic)
{
    string cmd = "ifconfig " + nic + "|grep HWaddr|awk '{print $4}'";
    string result;

    vector<string> vecHWaddr;
    vector<string>::iterator it_vecHWaddr;
    if( 0 != RunCmd(cmd,vecHWaddr ))
    {
        return 0;
    }
    if( vecHWaddr.empty())
    {
        return 0;
    }
    it_vecHWaddr = vecHWaddr.begin();
    if( 0 != (*it_vecHWaddr).compare("HWaddr"))
    {
        cout <<"IfconfigIsExist "<< nic <<" is not exist! " << endl; 
        return 0;
    }
    
    return 1;
}

int NicIsInOVS(const string& nic)
{
    cout <<"xen has not this cmd for NicIsInOVS"<<endl;
    return VNET_PLUGIN_SUCCESS;
}
    
int GetAllBridges(vector<string>& bridges)
{
    int ret;
    struct dirent *entry = NULL;
    DIR *dp = NULL;
    string sys_class_net("/sys/class/net");
    dp = opendir(sys_class_net.c_str());
    if (dp == NULL) 
    {
        return VNET_PLUGIN_ERROR;
    }
 
    string filename;
    while ((entry = readdir(dp)))
    {
        if(string(".") == string(entry->d_name) ||string("..") == string(entry->d_name) )
        {
            continue;
        }
        
        string bridge = sys_class_net + "/" + string(entry->d_name) + "/bridge";
        struct stat filestat;
        ret = stat(bridge.c_str(),&filestat);
        //凡是带有bridge子目录的都是网桥
        if(ret == 0 && S_ISDIR(filestat.st_mode))
        {
            //cout <<entry->d_name<<" is bridge"<<endl;
            bridges.push_back(string(entry->d_name));
        }
    }
    //cout<<"xen quit GetAllbridges"<<endl;
    closedir(dp);
    return VNET_PLUGIN_SUCCESS;
}

int GetIfsOfBridge(const string& bridge, vector<string>& ifs)
{
    struct dirent *entry = NULL;
    DIR *dp = NULL;
    string sys_class_net("/sys/class/net");
    //网桥的brif子目录列出了插入该网桥的网口名称
    string brif = sys_class_net + "/" + bridge + "/brif";
    dp = opendir(brif.c_str());
    if (dp == NULL) 
    {
        cout<<"opendir: Path does not exist or could not be read."<<endl;
        return VNET_PLUGIN_ERROR;
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
    return VNET_PLUGIN_SUCCESS;
}

int GetAllOfPortIdOfBridge(const string& bridge, map<string, uint32>& of_port_list)
{
    cout <<"xen has not this cmd for GetAllOfPortIdOfBridge"<<endl;
    return 0;
}

int GetOfPortIdOfIf(const string& nic, uint32& of_port_id)
{
    cout <<"xen has not this cmd for GetOfPortIdOfIf"<<endl;
    return 0;
}

int GetIfListOfSameVlan(const uint32& vlan, vector<string>&ifs)
{
    cout <<"xen has not this cmd for GetIfListOfSameVlan"<<endl;
    return 0;
}


int GetBridgeOfIf(const string& nic, string& bridge)
{
    vector<string> bridges;
    GetAllBridges(bridges);
    vector<string> ifs;
    vector<string>::iterator it_bridge=bridges.begin();
    for (; it_bridge!= bridges.end(); ++it_bridge )
    {
        ifs.clear();
        GetIfsOfBridge(*it_bridge,ifs);
        if(ifs.empty()) continue;
        vector<string>::iterator it_ifs=ifs.begin();
        for (; it_ifs!= ifs.end(); ++it_ifs )
        {
            if(*it_ifs == nic)
            {
                bridge = *it_bridge;
                return VNET_PLUGIN_SUCCESS;
            }
        }
    }
    bridge = "";
    return VNET_PLUGIN_SUCCESS;
}

int GetRealIfNameFromBridge(string & port, string& rel_name)
{
    char show_bridge_name[24]={0}; //网桥名
    char show_bridge_id[24]={0};   //网桥ID
    char show_stp_enable[4]={0};   //sfp是否开启
    char show_peth_name[24]={0};   //物理网卡名
    string str_cmd ="brctl  show 2>/dev/null |grep ";

    int find = VNET_PLUGIN_FALSE;

    /* brctl show grep eth0 */
    str_cmd = str_cmd +port;

    vector<string> vec_res;
    if (0 != RunCmd(str_cmd,vec_res))
    {
        cout<<"GetRealIfNameFromBridge: call system("<<str_cmd <<" failed!"<<endl;
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
                find = VNET_PLUGIN_TRUE;
                break;
            }

            if (strcmp(port.c_str(),show_bridge_name)== 0)
            {
                rel_name.assign(show_peth_name);
                find = VNET_PLUGIN_TRUE;
                break;
            }
        }
    }

    if( VNET_PLUGIN_FALSE == find)
    {
        return -1;
    }

    return 0;
}

int GetAllLoopPort(vector<string>& nics)
{
    struct dirent *entry = NULL;
    DIR *dp = NULL;
    string sys_class_net("/sys/class/net");
    dp = opendir(sys_class_net.c_str());
    if (dp == NULL) 
    {
        return VNET_PLUGIN_ERROR;
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
        string soft_link(buf);

        if(string::npos!=soft_link.find("/devices/virtual",0))
        {
            if(string::npos!=soft_link.find("loop",0) && string(entry->d_name).length()<6)
            {
                nics.push_back(string(entry->d_name));
            }
        }    
    }
    closedir(dp);
    return VNET_PLUGIN_SUCCESS;
}

int GetAllKernelPort(vector<string>& nics)
{
    struct dirent *entry = NULL;
    DIR *dp = NULL;
    string sys_class_net("/sys/class/net");
    dp = opendir(sys_class_net.c_str());
    if (dp == NULL) 
    {
        return VNET_PLUGIN_ERROR;
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
        string soft_link(buf);

        if(string::npos!=soft_link.find("/devices/virtual",0))
        {
            if(string::npos!=soft_link.find("kernel",0) /*&& string(entry->d_name).length()<8*/)
            {
                nics.push_back(string(entry->d_name));
            }
        }    
    }
    closedir(dp);
    return VNET_PLUGIN_SUCCESS;
}

int GetPhyPortFromBr(const string &strBrName,string&phyport)
{
    int iRet = 0;
    vector<string> vecIf;
    vector<string> vecPhyIf;
    vector<string> vecBondIf;
    vector<string> vecLoopif;

    iRet = GetIfsOfBridge(strBrName, vecIf);
    if (iRet)
    {
        cout << "GetPhyPortFromBr(): call get_ifs_of_bridge(" << strBrName << ") failed" << endl;
        return -1;
    }
     
    iRet = GetAllNic(vecPhyIf);
    if (iRet)
    {
        cout << "GetPhyPortFromBr(): call GetAllNic() failed" << endl;
        return -1;
    }

    iRet = GetAllBondings(vecBondIf);
    if (iRet)
    {
        cout << "GetPhyPortFromBr(): call GetAllBondings() failed" << endl;
        return -1;
    }
     
    iRet = GetAllLoopPort(vecLoopif);
    if (iRet)
    {
        cout << "GetPhyPortFromBr(): call GetAllLoopPort() failed" << endl;
        return -1;
    }

    vector<string>::iterator itrLstIf = vecIf.begin();
    for ( ; itrLstIf != vecIf.end(); itrLstIf++)
    {
        if (find(vecPhyIf.begin( ), vecPhyIf.end( ), *itrLstIf) != vecPhyIf.end())
        {
            phyport = *itrLstIf;
            return 0;
        }

        if (find(vecBondIf.begin( ), vecBondIf.end( ), *itrLstIf) != vecBondIf.end())
        {
            phyport = *itrLstIf;
            return 0;
        }

        if (find(vecLoopif.begin( ), vecLoopif.end( ), *itrLstIf) != vecLoopif.end())
        {
            phyport = *itrLstIf;
            return 0;
        }
    }
    return 0;
}

// 是否支持BOND(是否安装了BOND)
int IsSupportBond(int &isSupport)
{
    string cmd("lsmod | grep bonding -c");
    vector<string> vResult;
    vector<string>::iterator it;
    if( VNET_PLUGIN_SUCCESS != RunCmd(cmd,vResult ))
    {
        return VNET_PLUGIN_FALSE;
    }
    if( vResult.empty())
    {
        return VNET_PLUGIN_FALSE;
    }
    it = vResult.begin();

    int num = atoi((*it).c_str());
    if( 0 == num )
    {
        isSupport = VNET_PLUGIN_FALSE;
    }
    else
    {
        isSupport = VNET_PLUGIN_TRUE;
    }    
    
    return VNET_PLUGIN_TRUE;
}

/* ****************接口说明*********************************
参数 : vector<string> bondings  
返回 : int 成功则SUCEESS
              失败则ERROR
说明 : 将所有bond网卡名填入到vector<string> bondings     
************************************************************
*/
int GetAllBondings(vector<string>& bondings)
{
    struct dirent *entry = NULL;
    DIR *dp = NULL;
    string proc_net_bonding("/proc/net/bonding");
    dp = opendir(proc_net_bonding.c_str());
    if (dp == NULL) 
    {
        return VNET_PLUGIN_SUCCESS;
    }
 
    string filename;
    while ((entry = readdir(dp)))
    {
        if(entry->d_type == DT_LNK)
        {
            continue;
        }
        if(string(".") == string(entry->d_name) ||string("..") == string(entry->d_name) )
        {
            continue;
        }
        //cout <<entry->d_name<<" is bonding"<< endl;
        bondings.push_back(string(entry->d_name));
    }
    closedir(dp);
    return VNET_PLUGIN_SUCCESS;
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
int GetBondPara(const string &bond,const string &para,string& value)
{
    int ret;
    string sys_class_net("/sys/class/net");
    string bond_para = sys_class_net + "/" + bond + "/bonding/" +para;
    int handle;
    char buffer[500]={0} ; 

    //cout << "path is "<<bond_para<<endl;
    handle = open(bond_para.c_str(),O_RDONLY);

    if (handle  == -1)
    {
        //cout <<"Error opening file "<<bond_para<<endl;
        return VNET_PLUGIN_ERROR; 
    } 
    ret = read(handle,buffer,500);
    if(ret > 0)
    {
        //cout <<"successful read file "<<bond_para<<endl;
        //cout <<"buf is "<<buffer<<endl;
    }
    else
    {
        close(handle);
        cout <<"failed read file "<<bond_para<<endl;
        return VNET_PLUGIN_ERROR;
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
int AddBond(const string &bond, const string &mode, vector<string> &ifs)
{
    string str_cmd= string(VNET_BONDS_SCRIPTS);	
    vector<string> bondings;
    vector<string>::iterator itor;
    int ret;
    string ifs_num;
    char tmp[10];

    if(ifs.size() == 0)
    {
        cout <<"slaves is null!"<<endl;
        return VNET_PLUGIN_ERROR;
    }

    /* 首先判断输入的bond口名称是否存在 */
    ret = GetAllBondings(bondings);
    if(ret != 0)
    {
        cout <<"GetAllBondings failed"<<endl;
        return VNET_PLUGIN_ERROR;
    }
    if(find(bondings.begin(), bondings.end(), bond) != bondings.end())
    {
        cout << bond <<" is already exsit!"<<endl;
        return VNET_PLUGIN_SUCCESS;
    }
    sprintf(tmp,"%d",ifs.size());
    ifs_num.assign(tmp);
  
    str_cmd = str_cmd + " " + string(VNET_ADD_BOND)+ " " +bond+" " + mode +" "+ ifs_num+" ";
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
        cout <<"system "<<str_cmd<<"failed!"<<endl;
        return VNET_PLUGIN_ERROR;
    }
    return VNET_PLUGIN_SUCCESS;
}

/*****************接口说明**********************************************************
参数 : string bond             需要删除的bond网卡
       vector<string> ifs      成员口
       
返回 : STATUS 成功则SUCEESS
              失败则ERROR
              
说明 :  注意返回失败则一定是创建bond失败
        返回成功则不一定是成功
************************************************************************************
*/
int DelBond(const string &bond)
{
    string str_slaves;
    string str_cmd= string(VNET_BONDS_SCRIPTS);	
    vector<string> bondings;
    vector<string> vec_slaves;
    vector<string>::iterator itor;
    int ret;

    /* 首先判断输入的bond口名称是否存在 */
    ret = GetAllBondings(bondings);
    if(ret != 0)
    {
        cout <<"GetAllBondings failed!"<<endl;
        return VNET_PLUGIN_ERROR;
    }
    if(find(bondings.begin( ), bondings.end( ), bond) != bondings.end())
    {/* 找到了才处理删除操作 */
        str_cmd = str_cmd +	" " +string(VNET_DEL_BOND)+ " " +bond + " ";
        
        ret = GetBondPara(bond ,"slaves",str_slaves); 
        if(ret == VNET_PLUGIN_ERROR)
        {
            cout <<"get_bond_para failed!"<<endl;
           // return VNET_PLUGIN_ERROR;
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
            cout <<"system "<<str_cmd<< " failed!"<<endl;
            return VNET_PLUGIN_ERROR;
        }
        return VNET_PLUGIN_SUCCESS;
    }
    else
    {
        cout << "cant find" << bond <<endl;
        return VNET_PLUGIN_ERROR;
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
int SetBondPara(const string &bond, const string &para,const string &value)
{
    string str_cmd= string(VNET_BONDS_SCRIPTS);	
    int ret;

    str_cmd = str_cmd +	" " + string(VNET_SET_BOND) + " " + bond + " "+para + " " +value;
    ret = system(str_cmd.c_str());
    if(ret !=0 )
    {
        return VNET_PLUGIN_ERROR;
    }
    return VNET_PLUGIN_SUCCESS;
}

int SetMtu(const string& nic,const int& mtu)
{ 
    struct ifreq ifr; 
 
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0); 
    if ( -1 == sockfd ) 
    { 
        return VNET_PLUGIN_ERROR; 
    } 
 
    strcpy(ifr.ifr_name, nic.c_str()); 
    ifr.ifr_ifru.ifru_mtu = mtu; 
    if ( ioctl(sockfd, SIOCSIFMTU, &ifr) < 0 ) 
    { 
        close(sockfd); 
        sockfd = -1; 
        return VNET_PLUGIN_ERROR;
    } 
    
    close(sockfd); 
    sockfd = -1; 
    return VNET_PLUGIN_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
/////////////////ovs bonding相关接口////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
int GetAllBondingsOVS(vector<string>& bondings)
{
    cout <<"in xen has not this operation !"<<endl;
    return VNET_PLUGIN_SUCCESS;
}

int GetBondModeOVS(const string &bond,string& mode)
{
    cout <<"in xen has not this operation !"<<endl;
    return VNET_PLUGIN_SUCCESS;
}

int GetBondSlavesOVS(const string &bond, vector<string> &vecslaves)
{
    cout <<"in xen has not this operation !"<<endl;
    return VNET_PLUGIN_SUCCESS;
}


int AddBondOVS(const string &bridge,const string &bond, const string &mode, vector<string> &ifs)
{
    cout <<"in xen has not this operation !"<<endl;
    return VNET_PLUGIN_SUCCESS;
}    

int DelBondOVS(const string &bond)
{
    cout <<"in xen has not this operation !"<<endl;
    return VNET_PLUGIN_SUCCESS;
}    

int RenameDVS(const string &oldbr,const string &newbr,const string &bond_name)
{
    cout <<"in xen has not this operation !"<<endl;
    return VNET_PLUGIN_SUCCESS;
}     

int BackupInfo(const string& bond, string &active, vector<string> &others)
{
    cout <<"in xen has not this operation !"<<endl;
    return VNET_PLUGIN_SUCCESS;
} 

int LacpInfo(const string &bond, int &state,string &aggregate_id, string &partner_mac,
                   vector<string>  &suc_nics,vector<string> &fail_nics)
{
    cout <<"in xen has not this operation !"<<endl;
    return VNET_PLUGIN_SUCCESS;
}

int GetPhyPortFromBrOVS(const string &strBrName,string &phyport)
{
    cout <<"in xen has not this operation!"<<endl;
    return 0;
}

int AddNetIfOVS(const string & strBr, const string & strNetIf)
{
    cout <<"in xen has not this operation!"<<endl;
    return 0;
}

int GetNicAdminStatus(const string strNic, int &nStatus)
{
    string restr;
	string str_cmd = "ifconfig " + strNic +" |grep UP|awk '{print $1}'";
	if(0 != RunCmdRetString(str_cmd,restr))
    {
        cout << "GetNicAdminStatus get port " << strNic << " admin status failed" << endl; 
        return VNET_PLUGIN_ERROR;
    }
    if(restr.empty())
    {
        nStatus = 0;
    }
    else
    {
        nStatus = 1;
    }
    return VNET_PLUGIN_SUCCESS;
}

int SetNicAdminStatus(const string strNic, int nStatus)
{
    string status  = (nStatus) ? "up" : "down";
    string str_cmd = "ifconfig " + strNic + " " + status;

    if(0 != vnet_system(str_cmd))
    {
        cout << "ovs system " << str_cmd << " failed!" << endl; 
        return VNET_PLUGIN_ERROR;
    }

    cout << "SetNicAdminStatus set port " << strNic << " admin status " << status << endl; 
    
    return VNET_PLUGIN_SUCCESS;
}

int GetMtu(const char* lpszEth, unsigned int &mtu)
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

/*****************接口说明**********************************************************
参数 : string nics             网口名称
       status                  出参(SUCCESS时有效)
                               =0 down; = 1 up
返回 : STATUS 成功则SUCEESS
              失败则ERROR
              
说明 : 获取nic status
************************************************************************************
*/
int GetNicStatus(const string nics,int & status)
{
    // ethtool eth1 | grep "Link detected" | awk -F ': ' '{print $2}'
    string cmd("ethtool ");
    cmd += nics;
    cmd += " | grep \"Link detected\" |awk -F ': ' '{print $2}'";
    vector<string> vStatus;
    vector<string>::iterator it_vStatus;
    if( VNET_PLUGIN_SUCCESS != RunCmd(cmd,vStatus ))
    {
        return VNET_PLUGIN_ERROR;
    }
    if( vStatus.empty())
    {
        return VNET_PLUGIN_ERROR;
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
    return VNET_PLUGIN_SUCCESS;
}
/* ****************接口说明*********************************
参数 : vector<string> sriovs  
返回 : STATUS 成功则SUCEESS
              失败则ERROR
说明 : 将所有SRIOV网卡名填入到vector<string> sriovs              
************************************************************
*/
int GetAllSriovPort(vector<string>& sriovs)
{
    struct dirent *entry = NULL;
    DIR *dp = NULL;
    vector<string> nics;
    vector<string>::iterator nics_itor;
    int ret;
    
    sriovs.clear();
    ret = GetAllNic(nics);
    if (ret != VNET_PLUGIN_SUCCESS)
    {
        cout<<"GetAllNic failed!"<<endl;
        return VNET_PLUGIN_ERROR;
    }
    for(nics_itor = nics.begin(); nics_itor != nics.end() ; nics_itor ++)
    {
        string sys_class_net_eth = string("/sys/class/net/") + string(*nics_itor) + "/device";
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
    
    return VNET_PLUGIN_SUCCESS;
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
int GetSriovPci(const string &nicname, map<string,string>&vif_pci)
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
        cout<<"get_sriov_pci: Path does not exist or could not be read."<<endl;
        return VNET_PLUGIN_ERROR;
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
    return VNET_PLUGIN_SUCCESS;
}

/* ****************接口说明*********************************************************
参数 : int &num                    指定的SRIOV虚拟网卡的个数
       string nicname              需要获取虚拟网卡个数信息的网卡名
返回 : STATUS 成功则SUCEESS       
              失败则ERROR          非SRIOV网卡
说明 : 
************************************************************************************
*/
int GetSriovNum(const string &nicname, int &num)
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
        return VNET_PLUGIN_ERROR;
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
    return VNET_PLUGIN_SUCCESS;
}


//检查是否创建的loopback设备
int IsLoopDevice(const string &port_name)
{
    string bridge;
    string line;
    string nic;
    string file("/etc/loopback/vbridge.conf");

    //获取桥名
    GetBridgeOfIf(port_name, bridge);
    
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
                return VNET_PLUGIN_TRUE;
            }
        }  
    }
    return VNET_PLUGIN_FALSE;
}

/* ****************************  接口说明  *************************************
函数功能: 查询网卡环回属性，创建的tap设备也定义为环回设备.
输入参数: port_name 物理网卡名称
输出参数: lpflag    环回标记，1--环回模式，0--非环回模式
返回值  : SUCEESS   设置生效
          ERROR     设置失败
*******************************************************************************/
int GetPortLoopbackMode(const string &port_name, int &lpflag)
{
    struct vnet_lib_private_value eval;
    string line;
    string nic;
    string file("/etc/loopback/sriov.conf");
    
    if(port_name.empty())
    {
        return VNET_PLUGIN_ERROR;
    }
    
    lpflag = PORT_LOOPBACK_NO;

    if(IsLoopDevice(port_name))
    {
        lpflag = PORT_LOOPBACK_YES;
        return VNET_PLUGIN_SUCCESS;
    }

    eval.cmd = PRIVATE_GETLOOP;
    if(0 != SendIOCtrl(port_name.c_str(), &eval))    
    {
        return VNET_PLUGIN_ERROR;
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
                return VNET_PLUGIN_ERROR;
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
                    if(0 != SendIOCtrl(port_name.c_str(), &eval))    
                    {
                        cout <<"get_port_loopback_mode: send ioctl again failed"<<endl;
                        return VNET_PLUGIN_ERROR;
                    }  
                    lpflag = PORT_LOOPBACK_YES;
                    return VNET_PLUGIN_SUCCESS;
                }
            } 
        }
        return VNET_PLUGIN_SUCCESS;
    }
}


/* ****************************  接口说明  *************************************
函数功能: 设置网卡的环回模式，目前仅支持SRIOV网卡环回模式设置
输入参数: port_name 物理网卡名称
          lpflag    环回标记，1--设置，0--取消
返回值  : SUCEESS   设置生效
          ERROR     设置失败
*******************************************************************************/
int SetPorLoopback(const string &port_name, const uint32 &lpflag)
{
    struct vnet_lib_private_value eval;
    string line;
    string nic;
    string file("/etc/loopback/sriov.conf");

    if(port_name.empty())
    {
        return VNET_PLUGIN_ERROR;
    }

    eval.cmd  = PRIVATE_SETLOOP;
    eval.data = lpflag; 
    if(0 != SendIOCtrl(port_name.c_str(), &eval))
    {
        cout<<"set_port_loopback: send ioctl failed"<<endl;
        return VNET_PLUGIN_ERROR;
    }
    return VNET_PLUGIN_SUCCESS;
}

int CreateLoopBackPort(const string &port_name)
{
    string str_cmd=string(VNET_BRIDGE_SCRIPTS);
	str_cmd = str_cmd + " " +VNET_ADD_LOOP + " " +port_name;
	if(0 != vnet_system(str_cmd))
    {
        cout <<"CreatLoopBackPort "<<port_name<<"failed"<<endl; 
        return VNET_PLUGIN_ERROR;
    }
	return VNET_PLUGIN_SUCCESS;
}

int DeleteLoopBackPort(const string &port_name)
{
    string str_cmd=string(VNET_BRIDGE_SCRIPTS);
	str_cmd = str_cmd + " " +VNET_DEL_LOOP + " " +port_name;
	if(0 != vnet_system(str_cmd))
    {
        cout <<"DeleteLoopBackPort "<<port_name<<"failed"<<endl; 
        return VNET_PLUGIN_ERROR;
    }
	return VNET_PLUGIN_SUCCESS;
}

int CreateKernelPort(const string &port_name, const string &switch_name, const string &uplinkport, const string &ip, const string &mask, const uint32 is_dhcp,const uint32 vlan, const uint32 mtu)
{
    string str_cmd=string(VNET_BRIDGE_SCRIPTS);
    str_cmd = str_cmd + " " +VNET_ADD_KERNEL + " " +port_name + " " + switch_name + " " + uplinkport + " " + ip + " " + mask + " " + int2string(is_dhcp)+" " + int2string(vlan) + int2string(mtu);
    if(0 != vnet_system(str_cmd))
    {
        cout <<"CreatKernelPort "<<port_name<<"failed"<<endl; 
        return VNET_PLUGIN_ERROR;
    }

    return VNET_PLUGIN_SUCCESS;
}

int ModifyKernelPort(const string &port_name, const string &switch_name, const string &uplinkport, const string &ip, const string &mask)
{
    string str_cmd=string(VNET_BRIDGE_SCRIPTS);
    str_cmd = str_cmd + " " +VNET_MOD_KERNEL + " " +port_name + " " + switch_name + " " + uplinkport + " " + ip + " " + mask;
    if(0 != vnet_system(str_cmd))
    {
        cout <<"ModifyKernelPort "<<port_name<<"failed"<<endl; 
        return VNET_PLUGIN_ERROR;
    }

    return VNET_PLUGIN_SUCCESS;
}

int DeleteKernelPort(const string &switch_name, const string &port_name)
{
    string str_cmd=string(VNET_BRIDGE_SCRIPTS);
    str_cmd = str_cmd + " " +VNET_DEL_KERNEL + " " +switch_name + " " +port_name;
    if(0 != vnet_system(str_cmd))
    {
        cout <<"DeleteKernelPort "<<port_name<<"failed"<<endl; 
        return VNET_PLUGIN_ERROR;
    }

    return VNET_PLUGIN_SUCCESS;
}

int GetKernelPortInfo(const string &port_name, string &ip, string &mask)
{
    /*string str_cmd=string(VNET_BRIDGE_SCRIPTS);
    str_cmd = str_cmd + " " +VNET_GET_KERNEL + " " +port_name;
    if(0 != vnet_system(str_cmd))
    {
        cout <<"GetKernelPortInfo "<<port_name<<"failed"<<endl; 
        return VNET_PLUGIN_ERROR;
    }

    //sw,ip,mask 赋值

    return VNET_PLUGIN_SUCCESS;*/

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

int SetVlan(const string& nic,uint32 dwVlanID)
{
    string if_cmd = string(VNET_BRIDGE_SCRIPTS);
    string strNicName_vlan;
    
    if(dwVlanID > VNET_PLUGIN_MAX_VLAN_ID)
    {
        assert(0);
        return VNET_PLUGIN_ERROR;
    }
    
    if(0 == dwVlanID)
    {
        return VNET_PLUGIN_SUCCESS;
    }
    
    if(dwVlanID > 0)
    {
        strNicName_vlan = nic +"." +int2string(dwVlanID);
    }
    
    // 检查该网卡是否已经带VLAN
    if(1 == vnet_chk_dev(strNicName_vlan))
    {        
        return VNET_PLUGIN_SUCCESS;
    }
    //构造带参数的SHELL命令
    //vnet_dobridge.sh add_vlan ifname vlanid
    if_cmd = if_cmd + " " + string(VNET_ADD_VLAN) + " "+nic + " " + int2string(dwVlanID);

    if(0 != vnet_system(if_cmd))
    {
        cout <<"SetVlan("<<nic<<" ,"<<dwVlanID<<")failed"<<endl; 
        return VNET_PLUGIN_ERROR;
    }
    
    cout <<"SetVlan("<<nic<<" ,"<<dwVlanID<<")successful!"<<endl;
    return VNET_PLUGIN_SUCCESS;
}
    
int RemoveVlan(const string& nic,uint32 dwVlanID)
{
    string if_cmd = string(VNET_BRIDGE_SCRIPTS);
    int ret;
    
    if(dwVlanID > VNET_PLUGIN_MAX_VLAN_ID)
    {
        assert(0);
        return VNET_PLUGIN_ERROR;
    }
    
    if(0 == dwVlanID)
    {
        cout << "RemoveVlan VLAN is 0"<<endl;
        return VNET_PLUGIN_SUCCESS;
    }
    
    //构造带参数的SHELL命令
    //vnet_dobridge.sh del_vlan ifname vlanid
    if_cmd = if_cmd + " " + string(VNET_DEL_VLAN) +" "+ nic + " " +int2string(dwVlanID) ;

    ret= vnet_system(if_cmd);
        
    if(ret !=0)
    {
        cout <<"RemoveVlan("<<nic<<","<<dwVlanID<<")"<<"  ret is "<<ret<<endl; 
        return VNET_PLUGIN_ERROR;
    }

    cout << "RemoveVlan("<<nic<<","<<dwVlanID<<")"<<" successfully!"<<endl;
    return VNET_PLUGIN_SUCCESS;
}

int CreateBridge(const string & strBr)
{ 
    string bridge_cmd = string(VNET_BRIDGE_SCRIPTS);

    //构造带参数的SHELL命令
    //vnet_dobridge.sh del_vlan ifname vlanid
    bridge_cmd = bridge_cmd + " " +string(VNET_CREATE_BRIDGE_NOIF)+ " " + strBr;
    if(0 != vnet_system(bridge_cmd))
    {
        cout <<"system "<<bridge_cmd<<" failed"<<endl;
        return VNET_PLUGIN_ERROR;
    }
    return VNET_PLUGIN_SUCCESS;
}

int CreateBridge_withif(const string& strBr,const string& strNetIf,  uint32 nVlanID)
{
    string bridge_cmd = string(VNET_BRIDGE_SCRIPTS);
    string strNetIf_vlan;
    string strBridge_vlan;
    int iRet;

    //参数检查
    if(strBr.empty() || strNetIf.empty() || nVlanID > VNET_PLUGIN_MAX_VLAN_ID)
    {
        assert(0);
        return VNET_PLUGIN_ERROR;
    }
    
    
    //为网卡创建VLAN 
    iRet = SetVlan(strNetIf,nVlanID);
    if(-1 == iRet)
    {
        cout <<"SetVlan("<<strNetIf<<","<<nVlanID<<") failed"<<endl;
        return VNET_PLUGIN_ERROR;
    }

    if(0 == nVlanID)
    {
        strNetIf_vlan = strNetIf;
        strBridge_vlan = strBr;
    }
    
    //重新构造带VLAN的网卡名 以及网桥名
    if( nVlanID >0)
    {
        strNetIf_vlan = strNetIf +"." + int2string(nVlanID);
        strBridge_vlan = string("br_") + strBr + "_" +int2string(nVlanID);
    }

    //构造带参数的SHELL命令
    //vnet_dobridge.sh create_bridge ifname bridgename
    bridge_cmd = bridge_cmd +" " +string(VNET_CREATE_BRIDGE) +" " + strNetIf_vlan + " " +strBridge_vlan;
    if( 0 != vnet_system(bridge_cmd))
    {
        cout<<"CreateBridge failed "<<strBridge_vlan<<","<<strNetIf_vlan<<endl;
        return VNET_PLUGIN_ERROR;
    }
    cout<<"CreateBridge: CreateBridge successfully "<<strBridge_vlan<<","<<strNetIf_vlan<<endl;
    return VNET_PLUGIN_SUCCESS;
}

int DeleteBridge(const string & strBr)
{ 
    string down_bridge_cmd = "ifconfig "+strBr+ " down";
    if( 0 != vnet_system(down_bridge_cmd))
    {
        cout<< down_bridge_cmd <<"failed"<<endl;
        return VNET_PLUGIN_ERROR;
    }
	
    string bridge_cmd = string(VNET_BRIDGE_SCRIPTS);

    //构造带参数的SHELL命令
    //vnet_dobridge.sh del_vlan ifname vlanid
    bridge_cmd = bridge_cmd + " " +string(VNET_DELETE_BRIDGE_NOIF)+ " " + strBr;
    if(0 != vnet_system(bridge_cmd))
    {
        cout <<"system "<<bridge_cmd<<" failed"<<endl;
        return VNET_PLUGIN_ERROR;
    }
    return VNET_PLUGIN_SUCCESS;
}

int DeleteBridge_withif(const string& strBr,const string& strNetIf,  uint32 nVlanID )
{  
    string bridge_cmd = string(VNET_BRIDGE_SCRIPTS);
    string strNetIf_vlan;
    string strBridge_vlan;

    //参数检查
    if(strBr.empty() || strNetIf.empty() || nVlanID > VNET_PLUGIN_MAX_VLAN_ID)
    {
        assert(0);
        return VNET_PLUGIN_ERROR;
    }
    
    if(nVlanID == 0)
    {
        strNetIf_vlan = strNetIf;
        strBridge_vlan = strBr;
    }
    else
    {
       strNetIf_vlan = strNetIf +"." + int2string(nVlanID); 
       strBridge_vlan ="br_" +strBr +"_" +int2string(nVlanID); 
    }
     //构造带参数的SHELL命令删除掉网桥
    //vnet_dobridge.sh delete_bridge ifname bridgename
    bridge_cmd = bridge_cmd +" " +string(VNET_DELETE_BRIDGE) +" " + strNetIf_vlan + " " +strBridge_vlan;
    if( 0 != vnet_system(bridge_cmd))
    {
        cout<<"DeleteBridge br if vlan failed "<<strBridge_vlan<<","<<strNetIf_vlan<<","<<nVlanID<<endl;  
        return VNET_PLUGIN_ERROR;
    }
    cout<<"DeleteBridge br if vlan failed "<<strBridge_vlan<<","<<strNetIf_vlan<<","<<nVlanID<<endl; 

    //如果网卡有VLAN 去掉网卡的VLAN
    RemoveVlan(strNetIf,nVlanID);
    return VNET_PLUGIN_SUCCESS;
}


int AddNetIf(const string& strBr,const string& strNetIf,  uint32 nVlanID )
{
    int iRet = 0;
    string bridge_cmd = string(VNET_BRIDGE_SCRIPTS); 
    string strNetIf_vlan;
    string strBri_vlan;

    if(strBr.empty() || strNetIf.empty() || nVlanID > VNET_PLUGIN_MAX_VLAN_ID)
    {
        assert(0);
        return VNET_PLUGIN_ERROR;
    }

     //为网卡创建VLAN 
    iRet = SetVlan(strNetIf,nVlanID);
    if(-1 == iRet)
    {
        cout <<"Set Vlan failed "<<strNetIf<<","<<nVlanID<<endl;  
        return VNET_PLUGIN_ERROR;
    }

    //重新构造带VLAN的网卡名,以及网桥名
    if(0 == nVlanID)
    {
       strNetIf_vlan = strNetIf;
       strBri_vlan = strBr;
    }
    else
    {
        strNetIf_vlan = strNetIf +"." + int2string(nVlanID);
        strBri_vlan = "br_"+strBr +"_" +int2string(nVlanID);
    }
    
    //构造带参数的SHELL命令   
    //vnet_dobridge.sh  add_ifnet bridge_name netifname
    bridge_cmd = bridge_cmd + " "+ string(VNET_ADD_IF) + " " + strBri_vlan +  " " + strNetIf_vlan;
    if( 0 != vnet_system(bridge_cmd))
    {
        cout<<"vnet_system("<<bridge_cmd<<"failed"<<endl;
        return VNET_PLUGIN_ERROR;
    }
    cout<<"vnet_system("<<bridge_cmd<<"successfully!"<<endl;
    return VNET_PLUGIN_SUCCESS;
}

int DelNetIf(const string& strBr,const string& strNetIf,  uint32 nVlanID)
{
    string bridge_cmd = string(VNET_BRIDGE_SCRIPTS); 
    string strNetIf_vlan;
    string strBri_vlan;

    if(strBr.empty() || strNetIf.empty() || nVlanID > VNET_PLUGIN_MAX_VLAN_ID)
    {
        assert(0);
        return VNET_PLUGIN_ERROR;
    }
    
    if(0 == nVlanID)
    {
        strNetIf_vlan = strNetIf;
        strBri_vlan =strBr;
    }
    else
    {
        strNetIf_vlan = strNetIf + "." +int2string(nVlanID);
        strBri_vlan = "br_"+strBr+"_" +int2string(nVlanID);
    }
    //构造带参数的SHELL命令   
    //vnet_dobridge.sh  del_ifnet bridge_name netifname
    bridge_cmd = bridge_cmd + " " + string(VNET_DEL_IF) + " " +strBri_vlan + " " + strNetIf_vlan;

    if( 0 != vnet_system(bridge_cmd))
    {
        cout<<"vnet_system"<<bridge_cmd<<" failed!"<<endl;
        return VNET_PLUGIN_ERROR;
    }

    //清除网卡VLAN
    RemoveVlan(strNetIf,nVlanID);
    return VNET_PLUGIN_SUCCESS;
}

int AddVxlanNetIf(const string& strBr,const string& strVxlanNetIf, const string& strKey, const string& strRemoteIP)
{
    cout <<"in xen has not AddVxlanNetIf function"<<endl;
    return VNET_PLUGIN_SUCCESS;
}

int DelVxlanNetIf(const string& strBr,const string& strVxlanNetIf)
{
    cout <<"in xen has not DelVxlanNetIf function"<<endl;
    return VNET_PLUGIN_SUCCESS;
}

int DelNetIfOVS(const string& strBr,const string& strNetIf)
{
    cout <<"in xen has not this function"<<endl;
	return VNET_PLUGIN_SUCCESS;
}

int VNetIsPortInBond(const string& nic,string& bond)
{
    string str_cmd = string(VNET_BRIDGE_SCRIPTS);   
    int ret;
    
    if(nic.empty())
    {
        assert(0);
        return VNET_PLUGIN_ERROR;
    }
    //构造SHELL命令
    //vnet_dobridge.sh is_bond_slave nic
    str_cmd = str_cmd + " " +string(VNET_IS_BOND_SLAVE)+ " " +nic;
    ret = RunCmdRetString(str_cmd,bond);
    if(0 != ret )
    {
        return VNET_PLUGIN_ERROR;
    }
    return VNET_PLUGIN_SUCCESS;
}

int IsPortInBondOVS(const string& nic,string& bond)
{
    cout<<"xen has no this api"<<endl;
    return VNET_PLUGIN_TRUE;
}

int VNetIsPortInBridge(const string& nic,string& bridge)
{
    string str_cmd = string(VNET_BRIDGE_SCRIPTS);   
    int ret;
    
    if(nic.empty())
    {
        assert(0);
        return VNET_PLUGIN_ERROR;
    }
    //构造SHELL命令
    //vnet_dobridge.sh is_in_bridge nic
    str_cmd = str_cmd + " " +string(VNET_IS_IN_BRIDGE)+ " " +nic;
    ret = RunCmdRetString(str_cmd,bridge);
    if(0 != ret )
    {
        return VNET_PLUGIN_ERROR;
    }
    return VNET_PLUGIN_SUCCESS;
}

int SetBrideForwordLLDP(const string &strBrName)
{
    cout <<"in xen has not this function"<<endl;
	return VNET_PLUGIN_SUCCESS;
}

int AddBondSlave(const string &bond, const string &slave)
{
    string str_cmd= string(VNET_BONDS_SCRIPTS);
    vector<string> bondings;
    vector<string>::iterator itor;
    int ret;


    /* 首先判断输入的bond口名称是否存在 */
    ret = GetAllBondings(bondings);
    if(ret != 0)
    {
        cout <<"GetAllBondings failed"<<endl;
        return VNET_PLUGIN_ERROR;
    }
    
    if(!(find(bondings.begin(), bondings.end(), bond) != bondings.end()))
    {
        cout << bond <<" not  exsit!"<<endl;
        return VNET_PLUGIN_ERROR;
    }
  
    str_cmd = str_cmd + " " + string(VNET_ADD_BOND_SLAVE)+ " " +bond+" " + slave;
 
    ret = system(str_cmd.c_str());
    if(ret !=0 )
    {
        cout <<"system "<<str_cmd<<"failed!"<<endl;
        return VNET_PLUGIN_ERROR;
    }
    return VNET_PLUGIN_SUCCESS;
}

int DelBondSlave(const string &bond, const string &slave)
{
    string str_cmd= string(VNET_BONDS_SCRIPTS);
    vector<string> bondings;
    vector<string>::iterator itor;
    int ret;


    /* 首先判断输入的bond口名称是否存在 */
    ret = GetAllBondings(bondings);
    if(ret != 0)
    {
        cout <<"GetAllBondings failed"<<endl;
        return VNET_PLUGIN_ERROR;
    }
    
    if(!(find(bondings.begin(), bondings.end(), bond) != bondings.end()))
    {
        cout << bond <<" not  exsit!"<<endl;
        return VNET_PLUGIN_ERROR;
    }
  
    str_cmd = str_cmd + " " + string(VNET_DEL_BOND_SLAVE)+ " " +bond+" " + slave;
 
    ret = system(str_cmd.c_str());
    if(ret !=0 )
    {
        cout <<"system "<<str_cmd<<"failed!"<<endl;
        return VNET_PLUGIN_ERROR;
    }
    return VNET_PLUGIN_SUCCESS;
}

int GetBondSlave(const string &bond, vector<string> &vecslave)
{
    string value;
    int ret = GetBondPara(bond, "slaves", value);
    if( VNET_PLUGIN_SUCCESS != ret )
    {
        return ret;
    }
    put_string_to_vec(value,vecslave);
    return ret;
}

int SetPorVlanMode(const string &port_name, const uint32 &vlanflag)
{
    cout <<"libbr cant do SetPorVlanMode"<<endl;
    
    return VNET_PLUGIN_SUCCESS;
}

int RemovePorVlanMode(const string &port_name, const uint32 &vlanflag)
{
    cout <<"libbr cant do RemovePorVlanMode"<<endl;
    
    return VNET_PLUGIN_SUCCESS;
}

int SetVlanAccess(const string &port_name, const uint32 &accessvlan)
{
    cout <<"libbr cant do SetVlanAccess"<<endl;
    return VNET_PLUGIN_SUCCESS;
}

int SetVlanTrunk(const string &port_name, map<uint32,uint32>trunks)
{
    cout <<"libbr cant do SetVlanTrunk"<<endl;
    return VNET_PLUGIN_SUCCESS;
}

int GetPortVlanMode(const string &port_name,  uint32 &vlanflag)
{
    cout <<"libbr cant do GetVlanMode"<<endl;
    return VNET_PLUGIN_SUCCESS;
}

 int GetVlanAccess(const string &port_name,  uint32 &accessvlan)
{
    cout <<"libbr cant do GetVlanAccess"<<endl;
    return VNET_PLUGIN_SUCCESS;
}
 
 int GetVlanTrunks(const string &port_name,  vector<uint32>vlan)
{
    cout <<"libbr cant do GetVlanTrunks"<<endl;
    return VNET_PLUGIN_SUCCESS;
}

//设置sriov VF MAC
int SetVFPortMac(const string &port_name, const int &vfnum, const string &mac)
{
    string fcmd("");
    string scmd("");
    string cmd("ifconfig ");
    string strvfnum;

    //设置mac之前，需要先将物理网卡up
    cmd = cmd + port_name + " up ";
    if(0 != vnet_system(cmd.c_str()))
    {
        cout << "SetVFPortMac: run command: " <<cmd.c_str()<<" fail." <<endl; 
        return -1; 
    }          
    strvfnum = int2string(vfnum);
    
    fcmd = "ip link set " + port_name + " vf " + strvfnum/*.substr(6)*/; 
    scmd = fcmd + " mac " + mac;
    if(0 != vnet_system(scmd.c_str()))
    {
        cout << "SetVFPortMac: run command: " <<cmd.c_str()<<" fail." <<endl; 
        return -1; 
    } 

    return VNET_PLUGIN_SUCCESS;
}
//设置sriov VF VLAN
int SetVFPortVlan(const string &port_name, const int &vfnum, const int &vlan)
{
    string fcmd("");
    string scmd("");
    string cmd("ifconfig ");
    string strvfnum;
    
    cmd = cmd + port_name + " up ";
    if(0 != vnet_system(cmd.c_str()))
    {
        cout << "SetVFPortVlan: run command: " <<cmd.c_str()<<" fail." <<endl; 
        return -1; 
    }          
    strvfnum = int2string(vfnum);
    
    fcmd = "ip link set " + port_name + " vf " + strvfnum/*.substr(6)*/; 
    scmd = fcmd + " vlan " + int2string(vlan);
    if(0 != vnet_system(scmd.c_str()))
    {
        cout << "SetVFPortVlan: run command: " <<cmd.c_str()<<" fail." <<endl; 
        return -1; 
    } 
    
    return VNET_PLUGIN_SUCCESS;
} 

int GetVFPortVlan(const string &port_name, const int &vfnum,  int &vlan)
{
    int ret;
    string retstr;
    string cmd= "ip link show "+ port_name;
    //ip link show eth3 |grep 'vf 3' |awk -F ',' '{print $2}| awk '{print $2}' 
    cmd = cmd + "| grep " + "\'vf "+int2string(vfnum)+"\'" + "|awk -F \',\' \'{print $2}\'  |awk '\{print $2}\'" ;

    ret = RunCmdRetString(cmd,retstr);
    if(ret !=0 )
    {
        cout <<"RunCmdRetString "<<cmd<<"failed!"<<endl;
        return VNET_PLUGIN_ERROR;
    }
    replacedstr(retstr, "\n");
    vlan = string2int(retstr);
    return VNET_PLUGIN_SUCCESS;
}

int GetVFPortMac(const string &port_name, const int &vfnum,  string &mac)
{
    int ret;
    string retstr;
    string cmd= "ip link show "+ port_name;
    //ip link show eth3 |grep 'vf 3' |awk -F ',' '{print $1}| awk '{print $4}' 
    cmd = cmd + "| grep " + "\'vf "+int2string(vfnum) + "\'" + "|awk -F \',\' \'{print $1}\'  |awk '\{print $4}\'";

    ret = RunCmdRetString(cmd,retstr);
    if(ret !=0 )
    {
        cout <<"RunCmdRetString "<<cmd<<"failed!"<<endl;
        return VNET_PLUGIN_ERROR;
    }
    replacedstr(retstr, "\n");
    mac = retstr;
    return VNET_PLUGIN_SUCCESS;
}

int SetVlanNativeForTrunk(const string &port_name,  const uint32 &nativevlan)
{
   return VNET_PLUGIN_SUCCESS;
}


int TrunkWork(const string &port_name, map<uint32,uint32>trunks, const uint32 &nativevlan)
{
    return VNET_PLUGIN_SUCCESS;
}
int AccessWork(const string &port_name, const uint32 &access_vlan)
{
    return VNET_PLUGIN_SUCCESS;
}

int SetEvbUplinkFlow(const string bridge, const string vmMac, const uint32 vmPort,const uint32 UplinkPort, const uint32 TagVlan)
{
    return VNET_PLUGIN_SUCCESS;
}

int DelEvbUplinkFlow(const string bridge, const string vmMac)
{
    return VNET_PLUGIN_SUCCESS;
}

int SetEvbDownlinkFlow(const string bridge, const string vmPort,const string vmMac, const string otherPorts,const uint32  UplinkPort)
{
    return VNET_PLUGIN_SUCCESS;
}

int DelEvbDownlinkFlow(const string bridge, const string vmMac)
{
    return VNET_PLUGIN_SUCCESS;
}

int GetVxlanSupport(bool &bVxlanStatus)
{
    cout <<"lib can't do GetVxlanSupport!" << endl;
    bVxlanStatus = false;
    return VNET_PLUGIN_SUCCESS;
}


