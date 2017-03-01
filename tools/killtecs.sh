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
    read answer
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

match=$1
if [ -z $match ]; then
match="tecs-"
fi

read_bool "Kill all processes whose name contains $match ?" "no"
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

