#!/bin/sh
###############################################################################
#                 TECS System Administration Scripts                          #
#                        Author: Zhang Wenjian                                #
#  version: 1.0                                                               #
#  usage: login menu for background tecs processes with telnetd enbaled       #
#                                                                             #
###############################################################################

tecspath=/var/run/tecs
match=$1

login()
{
    echo searching active background tecs processes ...

    result=`find $tecspath/ -name "*.telnet"`
    if [ -z "$result" ]; then
        echo "no running tecs process!"
        exit
    fi

    let index=0
    for i in `ls $tecspath/*.telnet`
    do
        pid=`cat "$i" | grep -a -m 1 "pid:" | awk -F':' '{print $2}'`
        if kill -0 $pid > /dev/null 2>&1; then
            #validate telnet file by check /proc/pid/exe link
            if [ -e "/proc/$pid" ]; then
                procexe=`readlink /proc/$pid/exe`
                exe=`cat "$i" | grep -a -m 1 "exe:" | awk -F':' '{print $2}'`
                [ "$exe" != "$procexe" ] && continue
            fi
            
            if [ ! -z "$match" ]; then
                if [ `echo $exe | grep -c "$match"` -eq 0 ]; then
                    continue
                fi
            fi

            application=`cat "$i" | grep  -a -m 1 "application" | awk -F':' '{print $2}'`
            process=`cat "$i" | grep -a -m 1 "process" | awk -F':' '{print $2}'`
            port=`cat "$i" | grep -a -m 1 "port" | awk -F':' '{print $2}'`
            echo "-------------------[$index]-------------------"
            #echo "index = $index"
            echo "exe = $exe"
            echo "pid = $pid"
            echo "application = $application"
            echo "process = $process"
            echo "telnetd port = $port"
            telnetd["$index"]=$port
            allindex["$index"]="$index"
            let index+=1
        fi
    done

    if [ "$index" -ne 0 ]; then
        echo "-----------------------------------------"

        while true
        do
        read -p "Please enter the index to telnet: " input
        if [ -z $input ]; then
            continue	 
        else
            break
        fi
        done
        # add by xuemzh for quit normal    
        if [ $input = "q" ]; then
            exit 0
        fi
        
        if [ $input = "quit" ]; then
            exit 0
        fi
        
        if [ $input = "exit" ]; then
            exit 0
        fi

        let found=0
        for i in "${allindex[@]}"
        do
            if [ "$i" == "$input" ] ; then
                let found=1
            fi
        done
        
        if [ $found -eq 1 ]; then
            echo telnet to port ${telnetd[input]} ...
            telnet 127.0.0.1 ${telnetd[input]}
        else
            echo "error! you input an invalid index!"
            echo 
            echo
        fi
    else    
        echo "No active tecs process found!";
        exit
    fi
}

which telnet >/dev/null 2>&1
if [ $? -ne 0 ]; then
   echo "telnet command is not available, please install telnet rpm!" 
   exit
fi

while true
do
    login
done

