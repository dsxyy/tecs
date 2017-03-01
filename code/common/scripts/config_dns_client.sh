#!/bin/bash
# arg 1: register/unregister
# arg 2: ip of name server
# arg 3: charactor symbol

resolv_file=/etc/resolv.conf
character_str="#added by tecs_$3"
new_line="nameserver $2 $character_str"
if [[ $1 = "register" ]];then
    if [[ -f $resolv_file ]];then
        grep -q "$new_line" $resolv_file || { sed -i "/$character_str/d" $resolv_file && echo "$new_line" >> $resolv_file; }
    else
        echo "$new_line" >> $resolv_file
    fi
else
    sed -i "/$new_line/d" $resolv_file
fi
