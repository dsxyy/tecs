#!/bin/bash
# 作者： 李孝成
# 编写日期：2012.12.18
# 对TECS的运行日志进行检查

path=`pwd`
RET=0
# HC日志解析
HC_ExcLog_dir=/var/log/tecs/HcExcLog
if [ -e $HC_ExcLog_dir ]; then
    HC_ExcLog_file=`ls $HC_ExcLog_dir`
    for file_name in $HC_ExcLog_file
    do 
        ./tecslog.py -l $HC_ExcLog_dir/$file_name -i $path/tecslog_ignored_value.txt
        result=$?
        if [[ $result != 0 ]]; then
            RET=$result
        fi
    done
fi


# 本地TC,CC日志解析
LocalExcLog_dir=/var/log/tecs/LocalExcLog
if [ -e $LocalExcLog_dir ]; then
    LocalExcLog_file=`ls $LocalExcLog_dir`
    for file_name in $LocalExcLog_file
    do 
        ./tecslog.py -l $LocalExcLog_dir/$file_name -i $path/tecslog_ignored_value.txt
        result=$?
        if [[ $result != 0 ]]; then
            RET=$result
        fi
    done
fi


# 数据库日志解析
pgLog_dir=/usr/local/pgsql/data/pg_log
if [[ -e $pgLog_dir ]]; then
    pgLog_file=`ls $pgLog_dir`
    for file_name in $pgLog_file
    do 
        ./pglog.py -l $pgLog_dir/$file_name -i $path/pglog_ignored_value.txt
        result=$?
        if [[ $result != 0 ]]; then
            RET=$result
        fi
    done
fi

echo $RET
exit $RET

