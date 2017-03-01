#!/bin/sh
#------------------------------------------------------------------------------
# Copyright (c) 2011������ͨѶ�ɷ����޹�˾��All rights reserved.
# 
# �ļ����ƣ�starttecs.sh
# �ļ���ʶ��
# ����ժҪ����װtecs����RPM����������������
# ��ǰ�汾��1.0
# ��    �ߣ���ΰ
# ������ڣ�2011��11��17��
#
# �޸ļ�¼1��
#     �޸����ڣ�2011/11/17
#     �� �� �ţ�V1.0
#     �� �� �ˣ���ΰ
#     �޸����ݣ�����
# �޸ļ�¼2��
#     �޸����ڣ�2012/02/09
#     �� �� �ţ�V2.0
#     �� �� �ˣ���ΰ
#     �޸����ݣ�����ָ��ĳ������������
# �޸ļ�¼3��
#     �޸����ڣ�2012/02/28
#     �� �� �ţ�V3.0
#     �� �� �ˣ���ΰ
#     �޸����ݣ����Ӷ�ʱ�ɼ�����tecsinfocron������ʽ��
#------------------------------------------------------------------------------
#����������װΪ����
#����TC��ؽ���
function starttc
{
  #[start TC]
  echo "start tc ......"
  cd /opt/tecs/tc/bin
  if [ `find . -name "TC_CGSL*"` ]; then
  ./TC_CGSL_X86_64_* -f ../etc/tecs.conf -d 
  ./TC_STORAGE_CGSL_X86_64_* -f ../etc/tecs.conf -d 
  ./FTP_CGSL_X86_64_* -f ../etc/tecs.conf -d 
  ./CLI_CGSL_X86_64_* -p ../scripts/
  ./VERSION_AGENT_CGSL_X86_64_* -f ../etc/tecs.conf -d 
  ./VERSION_CGSL_X86_64_* -f ../etc/tecs.conf -d 
  
  else
  ./TC_PC_X86_64_* -f ../etc/tecs.conf -d        
  ./TC_STORAGE_PC_X86_64_* -f ../etc/tecs.conf -d 
  ./FTP_PC_X86_64_* -f ../etc/tecs.conf -d 
  ./CLI_PC_X86_64_* -p ../scripts/
  ./VERSION_AGENT_PC_X86_64_* -f ../etc/tecs.conf -d 
  ./VERSION_PC_X86_64_* -f ../etc/tecs.conf -d 
        
 fi
}

#����CC��ؽ���
function startcc
{
  #[start CC]
  echo "start cc ......"
  cd /opt/tecs/cc/bin
  if [ `find . -name "CC_CGSL*"` ]; then
  ./CC_CGSL_X86_64_* -f ../etc/tecs.conf -d 
  ./CC_STORAGE_CGSL_X86_64_* -f ../etc/tecs.conf -d 
  ./VERSION_CGSL_X86_64_* -f ../etc/tecs.conf -d 
  
  else
  ./CC_PC_X86_64_* -f ../etc/tecs.conf -d         
  ./CC_STORAGE_PC_X86_64_* -f ../etc/tecs.conf -d 
  ./VERSION_PC_X86_64_* -f ../etc/tecs.conf -d 
    
  fi
} 

#����HC��ؽ���
function starthc
{
  #[start HC]                                   
  echo "start hc ......"                     
  cd /opt/tecs/hc/bin                           
  if [ `find . -name "HC_CGSL*"` ]; then        
  ./HC_CGSL_X86_64_* -f ../etc/tecs.conf -d     
  ./VERSION_CGSL_X86_64_* -f ../etc/tecs.conf -d
                                                
  else                                          
  ./HC_PC_X86_64_* -f ../etc/tecs.conf -d       
  ./VERSION_PC_X86_64_* -f ../etc/tecs.conf -d  
                                                
  fi                                            
}

# get 'yes' or 'no' answer from user
function read_bool 
{
    local prompt=$1
    local default=$2
    local input
    echo -e "$prompt (y/n): \c "
    read -t 5 input
    case $input in
            "Y")    
            input="yes" ;;
            "y")    
            input="yes";;
            "n")    
            input="no";;
            "N")    
            input="no";;
            "")       
            input="$default";;
            *)       
            echo "Please input y or n" 
            read_bool;;
    esac
    answer=$input
    return 0
}

#����tecs
function starttecs
{ 
  echo "start tecs ......"

  #[stop firewall]
  service iptables stop

  #[start qpid]
  qpidd -d 

  #[start postgresql]
  #/usr/local/pgsql/bin/config_postgresql.sh restart
  
  #����ָ����tecs��ؽ��̣�Ĭ��ȫ����
  case $1 in
    "")               
    starttc
    startcc        
    starthc ;;  
    "tc")             
    starttc ;;  
    "cc")             
    startcc ;;   
    "hc")             
    starthc ;;
    *)               
    echo  "parase shoud be tc/cc/hc or null" ;;
  esac
  
  #check process status
  echo "check process status."
  ps -elf |grep X86 |grep -v grep
  ps -elf |grep X86 | grep -v grep |wc -l 
  
  #����������Ϣ�Զ��ɼ�
  read_bool "����TECS�����Ϣ�ɼ���" "yes"
  if [ "$answer" = "yes" ]; then
  service crond start
  else
  service crond stop
  fi
}



function showhelp
{
    echo "= Usage: starttecs.sh argument"
    echo "= argument supported: "
    echo "=    (default): start all tecs process"
    echo -e "=    tc/cc/hc: start specific process;\nEX: starttecs.sh tc ֻ����tc��ؽ��̡�"
    echo "=    help: show help message!"
}

#������ʽѡ��
case $1 in
    "")               starttecs ;;  
    "tc")             starttecs "tc" ;;  
    "cc")             starttecs "cc" ;;   
    "hc")             starttecs "hc" ;;
    "help")           showhelp;;
    *)               
    echo "unknown argument!"
    showhelp;;
esac
