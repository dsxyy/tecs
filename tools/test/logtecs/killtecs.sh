#!/bin/sh
###############################################################################
#                 TECS System Administration Scripts                          # 
#                        Author: Zhang Wenjian                                #
#  version: 1.0                                                               #
#  usage: kill all running processes which name contains a specialized string #                
#                                                                             #
###############################################################################

# get 'yes' or 'no' answer from user
function read_bool 
{
    local prompt=$1
    local default=$2
    
    echo -e "$prompt (y/n): \c "
    read -t 5 answer
    case $answer in
            "Y")    
            answer="yes" ;;
            "y")    
            answer="yes";;
            "n")    
            answer="no";;
            "N")    
            answer="no";;
            "")       
            answer="$default";;
            *)       
            echo "Please input y or n" 
            read_bool;;
    esac
    return 0
}

case $1 in  
    "tc")             match="TC" ;;  
    "cc")             match="CC" ;;   
    "hc")             match="HC" ;;
    *)                match=$1   ;;
esac

match=$1
if [ -z $match ]; then
match=X86_64
fi

read_bool "Kill all processes whose name contains $match ?" "yes"
if [ $answer == "no" ]; then 
  exit 0
fi

result=`ps -elf | grep "$match" | grep -v grep | awk -F ' ' '{ print $4 " " $15}'`
if [ -z "$result" ]; then
    echo "no processes found!"
    exit 0
fi

for i in `ps -elf | grep "$match" | grep -v grep | awk -F ' ' '{ print $4}'`
do
        if [ "$i" == "$$" ]; then
            continue
        fi

        if [ -d /proc/$i ]; then
            echo will kill pid $i ...
            kill -9 $i
        fi
done
echo "Done!"
#增加定时采集关闭，add by pengwei20120228
read_bool "停止TECS相关信息采集？" "no"
if [ $answer == "yes" ]; then 
  service crond stop
fi

