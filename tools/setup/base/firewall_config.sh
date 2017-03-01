#!/bin/sh
# ���ߣ� ����
# ��д���ڣ�2012.10.27
if [ ! "$_FIREWALL_CONFIG_FILE" ];then
_FIREWALL_CONFIG_DIR=`pwd`
cd $_FIREWALL_CONFIG_DIR/../common/
source  ./tecs_common_func.sh
cd $_FIREWALL_CONFIG_DIR


###### ���ŷ���Ķ˿ڶ��� ######

# ping����icmpЭ��
ping_protocol="icmp"
# smb����tcpЭ��
smb_port=445
# SSH����tcpЭ��
ssh_port=22
# telnet����tcpЭ��
telnet_port=23
# dns����udpЭ��
dns_port=53
# openais�󶨵�UshellAgent�˿ڣ�tcp/udpЭ��
openais_port1=10000
# openais�󶨵�UshellAppReg�˿ڣ�udpЭ��
openais_port2=10011
# ���ݷ���tcpЭ��
postgresql_port=5432
# qpid��Ϣ����tcpЭ��
qpid_port=5672
# web����tcpЭ��
https_port=443
# xmrpc����tcpЭ��
xmlrpc_port=8080
# ָ�� relocation ��������ʹ�õĶ˿ڣ�tcpЭ��
xend_relocation_port=9002 
# nfs����Ĭ��ΪudpЭ��
nfs_port=2049
rpcbind_port=111
rquotad_port=875
lockd_tcpport=32803
mountd_port=892
statd_port=662
statd_outgoing_port=2020
lockd_udpport=32769
# ftp���ƶ˿ڣ�tcpЭ��
ftp_control_port=21
# ftp���ݶ˿�Ĭ��ֵ��tcpЭ��
ftp_data_port0=9001
ftp_data_port1=9010
# ������������vnc��¼�˿ڣ�tcpЭ��
vnc_port0=5900
vnc_port1=65535
# dhcp����˺Ϳͻ��˶˿ڣ�udpЭ��
dhcp_server_port=67
dhcp_client_port=68
#����洢iscsi����˿ڣ�tcpЭ��
iscsi_port=3260
# nagios nrpe�ͻ��˼�ض˿ڣ�tcpЭ��
nrpe_port=5666
# pxe����˿ڣ�udpЭ��
pxe_port=4011
# tftp����˿ڣ�tcp/udpЭ��
tftp_port=69
# ntp ����˿ڣ�udpЭ��
ntp_port=123
# vxlan����ʹ�ö˿ڣ�udpЭ��
vxlan_port=4789

localhost_ip=127.0.0.1

# ��ȡftp���ݶ˿ڵ������ļ��������ȡʧ�ܣ�����Ĭ��ֵ
tc="/opt/tecs/tc/etc/tecs.conf"
cc="/opt/tecs/cc/etc/tecs.conf" 
# nfs����˿������ļ�
nfs_config_file="/etc/sysconfig/nfs"

#��ֹ�����ظ��򿪱��
ftp_start=0
nfs_start=0
dhcp_server_start=0

# �������ǽ����
function clear_iptables 
{
    iptables -P INPUT ACCEPT
    iptables -P OUTPUT ACCEPT
    iptables -P FORWARD ACCEPT
    iptables -F
    iptables -X
    
    service iptables save
}
# ��tcpЭ���dport
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
# ��udpЭ���dport
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

# ��ȡ�����ļ��е�ftp���ݶ˿�
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
# ����nfs����˿�Ϊָ���˿�
function  config_nfs_port
{
   local file=$1
   local config_answer=
   #[ ! -e "$file" ] && echo "$file not exist!" && clear_iptables &&exit -1
   # д��ָ���Ķ˿�    
   update_config $file "RQUOTAD_PORT" $rquotad_port
   update_config $file "LOCKD_TCPPORT" $lockd_tcpport
   update_config $file "LOCKD_UDPPORT" $lockd_udpport
   update_config $file "MOUNTD_PORT" $mountd_port
   update_config $file "STATD_PORT" $statd_port
   update_config $file "STATD_OUTGOING_PORT" $statd_outgoing_port
   # Ϊ��������д�벻�ɹ�����������д��Ķ˿�
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
   #�����һ��û�л�ȡ�������ٰ�װ����ǽ
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
# ��nfs��ط���
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

###### ����ǽ�������ÿ�ʼ ######

# ���ԭ�й���
clear_iptables
[ "$1" = "clear" ] && exit 1
# �趨Ԥ�����
iptables -P INPUT DROP
iptables -P OUTPUT ACCEPT
iptables -P FORWARD ACCEPT
# ������
iptables -A INPUT -i lo -s $localhost_ip -d $localhost_ip -j ACCEPT 

# �����÷���
iptables -A INPUT -p $ping_protocol -j ACCEPT 
open_tcp_dport $telnet_port
if [ -e /etc/init.d/smb ];then
    open_tcp_dport $smb_port  
fi

if [ -e /etc/init.d/sshd ];then
    open_tcp_dport $ssh_port   
fi

open_tcp_dport $nrpe_port

#���簲װ��ط���
rpm -qi tftp-server &>/dev/null
tftp_insatlled=$?
if [ $tftp_insatlled -eq 0 ];then
    [ $dhcp_server_start -eq 0 ] && open_udp_dport $dhcp_server_port && dhcp_server_start=1
    open_udp_dport $pxe_port
    open_udp_dport $tftp_port
    open_tcp_dport $tftp_port
    [ $nfs_start -eq 0 ] && open_nfs_service && nfs_start=1
fi

# �Ѿ���������������ͨ��
# �������������ͨ���ͻ��˵�һ������˿ڷ��ʷ���˵�ĳ���̶��˿�ʱ��
# �����ٿ��Ǹ���δ򿪿ͻ��˵�����˿����⡣
iptables -A INPUT -m state --state ESTABLISHED,RELATED -j ACCEPT

# TECS�˿�����
# dns����
if [ -e /etc/init.d/dnsmasq ];then
    open_udp_dport $dns_port 
fi  
# ���ݿ����
if [ -e /etc/init.d/postgresql ];then
    open_tcp_dport $postgresql_port  
fi   

# ��Ϣ����
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

# WEB����
if [ -e /etc/init.d/tecs-wui ];then
    open_tcp_dport $https_port      
fi
     
# tecs-tc����
if [ -e /etc/init.d/tecs-tc ];then

    [ $nfs_start -eq 0 ] && open_nfs_service  && nfs_start=1
   
    # ��ȡTC�������õ�ftp���ݶ˿�    
    if [ $ftp_start -eq 0 ];then    
        get_ftp_port $tc && ftp_start=1
        open_tcp_dport $ftp_control_port  
        open_tcp_dport $ftp_data_port0 $ftp_data_port1        
    fi
    open_tcp_dport $xmlrpc_port   
    open_udp_dport $ntp_port
         
fi

# tecs-cc����
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

# tecs-hc����
if [ -e /etc/init.d/tecs-hc ];then 

    open_udp_dport $dhcp_client_port
    
    # vnc��¼������˿�
    open_tcp_dport $vnc_port0 $vnc_port1 
fi 
  
# tecs-sa����
if [ -e /etc/init.d/tecs-sa ];then 
    # ����洢iscsi����˿�
    open_tcp_dport $iscsi_port 
fi  

# tecs-vna����
if [ -e /etc/init.d/tecs-vna ];then 
    # vxlan�˿�
    open_udp_dport $vxlan_port
fi  

# ���������iptables����(���򱣴浽��/etc/sysconfig/iptables��)
service iptables save
service iptables restart

# ��������������ǽ��Ч����
chkconfig --level 2345 iptables on

# �������������������ǽ����ֹ��������������������ӹ���
file=/etc/rc.local
[ ! -e $file ] && echo "$file not exist!" && exit 0
exist=`grep -c "^service[[:space:]]*iptables[[:space:]]*restart"  $file`
if [ "$exist" -eq 0 ];then
    echo "service iptables restart" >> $file
fi

_FIREWALL_CONFIG_FILE="firewall_config.sh"
fi
