#!/bin/bash
# ���ܣ�tecs���ݿⰲװ�����ű�
# ���ߣ����Ľ�
# ����ʱ�䣺2012.05.25
# ����˵����
# -i �Ƿ񽻻�ģʽ
# -s ���ݿ������url
# -u �������ݿ�ʱʹ�õ��û�����
# -p �������ݿ�ʱʹ�õ��û�����
# -f ��Ų����ļ���Ŀ¼���ƣ����еĲ����ļ����Ʊ�����update_*.sql��*�����ǲ����汾��
# -d Ҫ���������ݿ�����
# -t Ҫ�����ı�sql�ű�
# -v Ҫ��������ͼsql�ű�

HOST="localhost"
PORT=5432
USERNAME=
PASSWD=
DBNAME=
SCHEMA_LOG_TABLE="_schema_log"
PSQL="/usr/local/pgsql/bin/psql"
PG_DUMP="/usr/local/pgsql/bin/pg_dump"
UPDATES_DIR=
CREATE_TABLES=
CREATE_VIEWS=
RETVAL=
INTERACTIVE=0
BACKUP_FILE=

export PGHOME=/usr/local/pgsql
export PGDATA=/usr/local/pgsql/data
export PATH=/usr/local/pgsql/bin:$PATH
export LD_LIBRARY_PATH=/usr/local/pgsql/lib:$LD_LIBRARY_PATH

conf_dir=$1
#�ȴ洢IFSĬ��ֵ
IFS_old=$IFS
IFS=$'\n'
for line in `grep -E 'application|db' $conf_dir/*.conf | grep -v "#" `;
do 
    line=`echo "$line"| sed 's/.*://g' | sed 's/ *//g'`
    cfg_name=`echo "$line"|awk -F '=' '{print$1}'`
    cfg_value=`echo "$line"|awk -F '=' '{print$2}'`    
    case $cfg_name in
         db_server_url) HOST=$cfg_value ;;
         db_server_port) PORT=$cfg_value ;;
         db_user) USERNAME=$cfg_value ;;
         db_passwd) PASSWD=$cfg_value ;;
         application)DBNAME=$cfg_value ;;
          ?) echo "error" ;;
    esac
done
IFS=$IFS_old

CREATE_TABLES=$conf_dir/`ls $conf_dir|grep baseline.sql`
UPDATES_DIR=$conf_dir/updates
CREATE_VIEWS=$conf_dir/`ls $conf_dir|grep view.sql`

echo =====================
echo HOST=$HOST
echo USERNAME=$USERNAME
echo PASSWD=$PASSWD
echo DBNAME=$DBNAME
echo UPDATES_DIR=$UPDATES_DIR
echo CREATE_TABLES=$CREATE_TABLES
echo CREATE_VIEWS=$CREATE_VIEWS
echo =====================

if [[ "$1" == "/opt/tecs/tc/etc" ];then
    /opt/tecs/tc/scripts/install_pgdb.sh -s $HOST  -u $USERNAME -p $PASSWD  -d $DBNAME -f $UPDATES_DIR -t $CREATE_TABLES -v $CREATE_VIEWS
elif [[ "$1" == "/opt/tecs/cc/etc" ];then
    /opt/tecs/cc/scripts/install_pgdb.sh -s $HOST  -u $USERNAME -p $PASSWD  -d $DBNAME -f $UPDATES_DIR -t $CREATE_TABLES -v $CREATE_VIEWS
else
    echo "error dir:$1"
fi

