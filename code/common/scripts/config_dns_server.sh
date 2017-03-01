#!/bin/bash
# arg 1: register/unregister
# arg 2: ip
# arg 3: domain name

host_file=/etc/hosts
new_line="$2 $3 #added by tecs"
if [[ $1 = "register" ]];then
    if [[ -f $host_file ]];then
        grep -q "$new_line" $host_file || { sed -i "/$3 #added by tecs/d" $host_file && echo "$new_line" >> $host_file && service dnsmasq restart &>/dev/null; } 
    else
        echo "$new_line" >> $host_file && service dnsmasq restart &>/dev/null
    fi
else
    if [[ -f $host_file ]];then
        sed -i "/$new_line/d" $host_file && service dnsmasq restart &>/dev/null
    fi
fi
