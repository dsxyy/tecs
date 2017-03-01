#/bin/sh
# ���ߣ� ���Ľ�
# ��д���ڣ�2012.03.26
# �ڲ��Ի��������У������ռ�tecs���ɲ����ڼ�����Ĵ�����־����Ϣ����������Ա������λ
# �˽ű���ci serverͨ��ssh�ķ�ʽ��Ŀ�����Զ��ִ��

if [ ! -z $1 ]; then
    tecsdir=$1
else
    echo "no tecsdir!"
    exit -1
fi

echo "*******************************************************************************"
echo "$0 will gather tecs running output information and zip to $tecsdir/output.tar.gz ..." 
echo "*******************************************************************************"

# ����һ��������������Ϣ��Ŀ¼
output_dir=$tecsdir/output
[ ! -e $output_dir ] && mkdir -p $output_dir

# ��tecs������ʱ����̨��ӡ���浽output
running_print_dir=/dev/shm/tecsprint
[ -e $running_print_dir ] && cp $running_print_dir $output_dir -R

# ��tecs��־ϵͳ��������浽log
[ -e /var/log/tecs ] && cp /var/log/tecs  $output_dir -R

#����postgresql����־��Ϣ
[ -e /usr/local/pgsql/data/pg_log/startup.log ] && cp /usr/local/pgsql/data/pg_log/startup.log $output_dir

# ����xm list�Ĵ�ӡ��Ϣ
xm list > $output_dir/xmlist.txt

# ����xend����־��Ϣ
xm log > $output_dir/xmlog.txt

# ����mount��Ϣ
mount > $output_dir/mount.txt

# �������������Ϣ
ps -elf > $output_dir/ps.txt

# �����ڴ�ʹ����Ϣ
free -m > $output_dir/free.txt

# ѹ�������ci_server��ȡ
cd $tecsdir; tar -czvf ./output.tar.gz ./output

