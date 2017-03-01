#/bin/sh
# 作者： 张文剑
# 编写日期：2012.03.26
# 在测试机本地运行，用于收集tecs集成测试期间产生的错误日志等信息，供开发人员分析定位
# 此脚本由ci server通过ssh的方式在目标机的远程执行

if [ ! -z $1 ]; then
    tecsdir=$1
else
    echo "no tecsdir!"
    exit -1
fi

echo "*******************************************************************************"
echo "$0 will gather tecs running output information and zip to $tecsdir/output.tar.gz ..." 
echo "*******************************************************************************"

# 创建一个保存测试输出信息的目录
output_dir=$tecsdir/output
[ ! -e $output_dir ] && mkdir -p $output_dir

# 将tecs的运行时控制台打印保存到output
running_print_dir=/dev/shm/tecsprint
[ -e $running_print_dir ] && cp $running_print_dir $output_dir -R

# 将tecs日志系统的输出保存到log
[ -e /var/log/tecs ] && cp /var/log/tecs  $output_dir -R

#保存postgresql的日志信息
[ -e /usr/local/pgsql/data/pg_log/startup.log ] && cp /usr/local/pgsql/data/pg_log/startup.log $output_dir

# 保存xm list的打印信息
xm list > $output_dir/xmlist.txt

# 保存xend的日志信息
xm log > $output_dir/xmlog.txt

# 保存mount信息
mount > $output_dir/mount.txt

# 保存进程运行信息
ps -elf > $output_dir/ps.txt

# 保存内存使用信息
free -m > $output_dir/free.txt

# 压缩打包供ci_server提取
cd $tecsdir; tar -czvf ./output.tar.gz ./output

