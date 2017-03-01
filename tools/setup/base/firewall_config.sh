#!/bin/sh
# 作者： 高明
# 编写日期：2012.10.27
if [ ! "$_FIREWALL_CONFIG_FILE" ];then
_FIREWALL_CONFIG_DIR=`pwd`
cd $_FIREWALL_CONFIG_DIR/../common/
source  ./tecs_common_func.sh
cd $_FIREWALL_CONFIG_DIR


###### 开放服务的端口定义 ######

# ping服务，icmp协议
ping_protocol="icmp"
# smb服务，tcp协议
smb_port=445
# SSH服务，tcp协议
ssh_port=22
# telnet服务，tcp协议
telnet_port=23
# dns服务，udp协议
dns_port=53
# openais绑定的UshellAgent端口，tcp/udp协议
openais_port1=10000
# openais绑定的UshellAppReg端口，udp协议
openais_port2=10011
# 数据服务，tcp协议
postgresql_port=5432
# qpid消息服务，tcp协议
qpid_port=5672
# web服务，tcp协议
https_port=443
# xmrpc服务，tcp协议
xmlrpc_port=8080
# 指定 relocation 服务器所使用的端口，tcp协议
xend_relocation_port=9002 
# nfs服务，默认为udp协议
nfs_port=2049
rpcbind_port=111
rquotad_port=875
lockd_tcpport=32803
mountd_port=892
statd_port=662
statd_outgoing_port=2020
lockd_udpport=32769
# ftp控制端口，tcp协议
ftp_control_port=21
# ftp数据端口默认值，tcp协议
ftp_data_port0=9001
ftp_data_port1=9010
# 生成虚拟机后的vnc登录端口，tcp协议
vnc_port0=5900
vnc_port1=65535
# dhcp服务端和客户端端口，udp协议
dhcp_server_port=67
dhcp_client_port=68
#共享存储iscsi服务端口，tcp协议
iscsi_port=3260
# nagios nrpe客户端监控端口，tcp协议
nrpe_port=5666
# pxe服务端口，udp协议
pxe_port=4011
# tftp服务端口，tcp/udp协议
tftp_port=69
# ntp 服务端口，udp协议
ntp_port=123
# vxlan报文使用端口，udp协议
vxlan_port=4789

localhost_ip=127.0.0.1

# 获取ftp数据端口的配置文件，如果读取失败，则用默认值
tc="/opt/tecs/tc/etc/tecs.conf"
cc="/opt/tecs/cc/etc/tecs.conf" 
# nfs服务端口配置文件
nfs_config_file="/etc/sysconfig/nfs"

#防止服务重复打开标记
ftp_start=0
nfs_start=0
dhcp_server_start=0

# 清除防火墙规则
function clear_iptables 
{
    iptables -P INPUT ACCEPT
    iptables -P OUTPUT ACCEPT
    iptables -P FORWARD ACCEPT
    iptables -F
    iptables -X
    
    service iptables save
}
# 打开tcp协议的dport
function open_tcp_dport
{    
   local port0=$1 
   local port1=$2

   if [ $# -eq 1 ];then
      iptables -A INPUT -p tcp --dport $port0 -j ACCEPT 
   elif [ $# -eq 2 ];then
      iptables -A INPUT -p tcp --dport $port0:$port1 -j ACCEPT 
   else
      echo "function input numbers error!" && return 0  
   fi
}
# 打开udp协议的dport
function open_udp_dport
{
   local port0=$1 
   local port1=$2
   if [ $# -eq 1 ];then
      iptables -A INPUT -p udp --dport $port0 -j ACCEPT 
   elif [ $# -eq 2 ];then
      iptables -A INPUT -p udp --dport $port0:$port1 -j ACCEPT 
   else
      echo "function input numbers error!" && return 0  
   fi
}

# 获取配置文件中的ftp数据端口
function get_ftp_port
{   
    local file=$1
    local config_answer=
    local config_start_port=
    local config_end_port=    
   
    get_config $file "ftp_start_port"
    config_start_port=$config_answer
    get_config $file "ftp_end_port"
    config_end_port=$config_answer

    if [[ $config_start_port -ne "" &&  $config_end_port -ne "" ]]; then
        ftp_data_port0=$config_start_port
        ftp_data_port1=$config_end_port
    fi 
}
# 配置nfs服务端口为指定端口
function  config_nfs_port
{
   local file=$1
   local config_answer=
   #[ ! -e "$file" ] && echo "$file not exist!" && clear_iptables &&exit -1
   # 写入指定的端口    
   update_config $file "RQUOTAD_PORT" $rquotad_port
   update_config $file "LOCKD_TCPPORT" $lockd_tcpport
   update_config $file "LOCKD_UDPPORT" $lockd_udpport
   update_config $file "MOUNTD_PORT" $mountd_port
   update_config $file "STATD_PORT" $statd_port
   update_config $file "STATD_OUTGOING_PORT" $statd_outgoing_port
   # 为避免上面写入不成功，读出上面写入的端口
   get_config $file "RQUOTAD_PORT"
   rquotad_port=$config_answer
   get_config $file "LOCKD_TCPPORT"
   lockd_tcpport=$config_answer
   get_config $file "LOCKD_UDPPORT"
   lockd_udpport=$config_answer
   get_config $file "MOUNTD_PORT"
   mountd_port=$config_answer
   get_config $file "STATD_PORT"
   statd_port=$config_answer
   get_config $file "STATD_OUTGOING_PORT"
   statd_outgoing_port=$config_answer
   #如果有一个没有获取到，则不再安装防火墙
   if [ "$rquotad_port" = "" ] || [ "$lockd_tcpport" = "" ] 
      [ "$lockd_udpport" = "" ] || [ "$mountd_port" = "" ] 
      [ "$statd_port" = "" ] || [ "$statd_outgoing_port" = "" ]; then
       echo "=========================================================" 
       echo "read nfs port from $file error, failed to set firewall!" 
       echo "=========================================================" 
       clear_iptables 
       exit -1
   fi 
   
   service nfs restart 
}
# 打开nfs相关服务
function  open_nfs_service
{ 
   config_nfs_port $nfs_config_file   
   open_tcp_dport $nfs_port
   open_udp_dport $nfs_port   
   open_tcp_dport $rpcbind_port
   open_udp_dport $rpcbind_port   
   open_tcp_dport $rquotad_port
   open_udp_dport $rquotad_port
   open_tcp_dport $lockd_tcpport
   open_udp_dport $lockd_udpport
   open_tcp_dport $mountd_port
   open_udp_dport $mountd_port
   open_tcp_dport $statd_port
   open_udp_dport $statd_port
   open_tcp_dport $statd_outgoing_port
   open_udp_dport $statd_outgoing_port
}

###### 防火墙规则设置开始 ######

# 清除原有规则
clear_iptables
[ "$1" = "clear" ] && exit 1
# 设定预设规则
iptables -P INPUT DROP
iptables -P OUTPUT ACCEPT
iptables -P FORWARD ACCEPT
# 允许本机
iptables -A INPUT -i lo -s $localhost_ip -d $localhost_ip -j ACCEPT 

# 允许常用服务
iptables -A INPUT -p $ping_protocol -j ACCEPT 
open_tcp_dport $telnet_port
if [ -e /etc/init.d/smb ];then
    open_tcp_dport $smb_port  
fi

if [ -e /etc/init.d/sshd ];then
    open_tcp_dport $ssh_port   
fi

open_tcp_dport $nrpe_port

#网络安装相关服务
rpm -qi tftp-server &>/dev/null
tftp_insatlled=$?
if [ $tftp_insatlled -eq 0 ];then
    [ $dhcp_server_start -eq 0 ] && open_udp_dport $dhcp_server_port && dhcp_server_start=1
    open_udp_dport $pxe_port
    open_udp_dport $tftp_port
    open_tcp_dport $tftp_port
    [ $nfs_start -eq 0 ] && open_nfs_service && nfs_start=1
fi

# 已经建立的链接允许通过
# 有了这条规则后，通过客户端的一个随机端口访问服务端的某个固定端口时，
# 不必再考虑该如何打开客户端的随机端口问题。
iptables -A INPUT -m state --state ESTABLISHED,RELATED -j ACCEPT

# TECS端口设置
# dns服务
if [ -e /etc/init.d/dnsmasq ];then
    open_udp_dport $dns_port 
fi  
# 数据库服务
if [ -e /etc/init.d/postgresql ];then
    open_tcp_dport $postgresql_port  
fi   

# 消息服务
if [ -e /etc/init.d/qpid ];then
    open_tcp_dport $qpid_port
fi

# ushell
if [ -e /etc/init.d/openais ];then
    open_udp_dport $openais_port1
    open_udp_dport $openais_port2
    open_tcp_dport $openais_port1  
fi

if [ -e /etc/init.d/xend ];then
    open_tcp_dport $xend_relocation_port
fi

# WEB服务
if [ -e /etc/init.d/tecs-wui ];then
    open_tcp_dport $https_port      
fi
     
# tecs-tc服务
if [ -e /etc/init.d/tecs-tc ];then

    [ $nfs_start -eq 0 ] && open_nfs_service  && nfs_start=1
   
    # 读取TC下面配置的ftp数据端口    
    if [ $ftp_start -eq 0 ];then    
        get_ftp_port $tc && ftp_start=1
        open_tcp_dport $ftp_control_port  
        open_tcp_dport $ftp_data_port0 $ftp_data_port1        
    fi
    open_tcp_dport $xmlrpc_port   
    open_udp_dport $ntp_port
         
fi

# tecs-cc服务
if [ -e /etc/init.d/tecs-cc ];then 
    [ $dhcp_server_start -eq 0 ] && open_udp_dport $dhcp_server_port && dhcp_server_start=1
    [ $nfs_start -eq 0 ] && open_nfs_service  && nfs_start=1
    if [ $ftp_start -eq 0 ];then          
        get_ftp_port $cc && ftp_start=1
        open_tcp_dport $ftp_control_port  
        open_tcp_dport $ftp_data_port0 $ftp_data_port1 
    fi
    open_udp_dport $ntp_port
fi       

# tecs-hc服务
if [ -e /etc/init.d/tecs-hc ];then 

    open_udp_dport $dhcp_client_port
    
    # vnc登录虚拟机端口
    open_tcp_dport $vnc_port0 $vnc_port1 
fi 
  
# tecs-sa服务
if [ -e /etc/init.d/tecs-sa ];then 
    # 共享存储iscsi服务端口
    open_tcp_dport $iscsi_port 
fi  

# tecs-vna服务
if [ -e /etc/init.d/tecs-vna ];then 
    # vxlan端口
    open_udp_dport $vxlan_port
fi  

# 保存后重启iptables服务(规则保存到了/etc/sysconfig/iptables中)
service iptables save
service iptables restart

# 重启物理机后防火墙生效设置
chkconfig --level 2345 iptables on

# 物理机重启后重启防火墙，防止开机过程中其他服务添加规则
file=/etc/rc.local
[ ! -e $file ] && echo "$file not exist!" && exit 0
exist=`grep -c "^service[[:space:]]*iptables[[:space:]]*restart"  $file`
if [ "$exist" -eq 0 ];then
    echo "service iptables restart" >> $file
fi

_FIREWALL_CONFIG_FILE="firewall_config.sh"
fi
