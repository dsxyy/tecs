
#!/bin/bash
# ���ܣ�tecs���ݿⰲװ�����ű�
# ���ߣ����Ľ�
# ����ʱ�䣺2012.05.25
# ����˵����
# -i �Ƿ񽻻�ģʽ
# -s ���ݿ������url
# -u �������ݿ�ʱʹ�õ��û�����
# -p �������ݿ�ʱʹ�õ��û�����
# -f ��������ű���Ŀ¼���ƣ����У�
# �����ű��ļ����Ʊ�����update_*.sql��*�����������ű��İ汾��
# �����ű��ļ����Ʊ�����bugfix_*.sql��*�����ǲ����ű��İ汾��
# �������˽ű��ļ����Ʊ�����revert_*.sql��*�����Ƕ�Ӧ�����ű��İ汾��
# ÿһ�������ű���������һ����Ӧ��revert���˽ű�������
# -d Ҫ���������ݿ�����
# -t Ҫ�����ı�sql�ű�
# -v Ҫ��������ͼsql�ű�
# ʹ�÷���ʾ����./dbupdate.sh -i -s 127.0.0.1  -u tecs -p tecs  -d tecscloud -f ./tecs_updates/ -t ./tc_tables.sql -v ./tc_views.sql 

HOST="localhost"
PORT=5432
USERNAME="tecs"
PASSWD="tecs"
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
CURRENT_SCRIPT=$0

export PGHOME=/usr/local/pgsql
export PGDATA=/usr/local/pgsql/data
export PATH=/usr/local/pgsql/bin:$PATH
export LD_LIBRARY_PATH=/usr/local/pgsql/lib:$LD_LIBRARY_PATH

#�����в�������
while getopts "is:u:p:d:f:t:v:" option
do
    case $option in
         i) INTERACTIVE=1 ;;
         s) HOST=$OPTARG ;;
         u) USERNAME=$OPTARG ;;
         p) PASSWD=$OPTARG ;;
         d) DBNAME=$OPTARG ;;
         f) UPDATES_DIR=$OPTARG ;;
         t) CREATE_TABLES=$OPTARG ;;
         v) CREATE_VIEWS=$OPTARG ;;
         ?) echo "invalid argument!"; exit 1 ;;
    esac
done

#�������ĺϷ��Ժ���Ч��
[ ! -e "$PSQL" ] && echo "param error!" exit 1
[ -z "$HOST" ] && echo "param error!" exit 1
[ -z "$USERNAME" ] && echo "param error!" exit 1
[ -z "$PASSWD" ] && echo "param error!" exit 1
[ -z "$DBNAME" ] && echo "param error!" exit 1
[ -z "$UPDATES_DIR" ] && echo "param error!" exit 1
[ ! -d "$UPDATES_DIR" ] && echo "param error!" exit 1
[ -z "$CREATE_VIEWS" ] && echo "param error!" exit 1
[ ! -e "$CREATE_VIEWS" ] && echo "param error!" exit 1
[ -z "$CREATE_TABLES" ] && echo "param error!" exit 1
[ ! -e "$CREATE_TABLES" ] && echo "param error!" exit 1

function read_bool
{
    local prompt=$1
    local default=$2

    #�ǽ���ģʽ�£�ֱ�ӷ���Ĭ������
    if [ $INTERACTIVE -eq 0 ]; then
        echo "$prompt (default: $default): $default"
        answer=$default
        return 0
    fi
    
    echo -e "$prompt (y/n? default: $default): \c "
    read answer
    case $answer in
            "Y") answer="yes" ;;
            "y") answer="yes";;
            "n") answer="no";;
            "N") answer="no";;
            "") answer="$default";;
            *) echo "Please input y or n"; read_bool;;
    esac
    return 0
}

function read_string
{
    local prompt=$1
    local default=$2
    
    #�ǽ���ģʽ�£�ֱ�ӷ���Ĭ������
    if [ $INTERACTIVE -eq 0 ]; then
        echo "$prompt (default: $default): $default"
        answer=$default
        return 0
    fi
    
    echo -e "$prompt (default: $default): \c "
    read answer
    [ -z $answer ] && answer="$default"
}

#��������ű��ļ��Ļ����Ϸ���
function validate_update_sql
{
    local file=$1
    [ ! -e "$file" ] && echo "update file $file does not exist!"&& exit 1

    #���ļ�������ȡ�汾�Ų��֣�ȥ����ͷ�ġ�update_���ͽ�β�ġ�.sql��
    local update_name=`basename $file`
    local temp=${update_name##update_}
    local update_version=${temp%%.sql}
    
    #ע�����������update_*.0000.sqlֻ�Ǹ���ʾ����������̫����
    [ `echo $file | grep -c ".0000.sql"` == 1 ] && return 0
    
    echo "Checking update script $file..."
    #���update�ű�������Ӧ�ó���_schema_log�����ƺͰ汾�Űɣ�
    result=`grep "^[[:space:]]*[^-]" "$file" | grep "$SCHEMA_LOG_TABLE" `
    [ -z "$result" ] && echo "invalid update file $file! String $SCHEMA_LOG_TABLE is not found!" && exit 1
    result=`grep "^[[:space:]]*[^-]" "$file" | grep "$update_version" `
    [ -z "$result" ] && echo "invalid update file $file! String $update_version is not found!" && exit 1
    return 0
}

#���bugfix/revert�ű��ļ��Ļ����Ϸ���
function validate_bugfix_revert_sql
{
    local bugfix_file=$1
    
    #���Ƚ��ļ����е�bugfix_����revert_�õ��������˽ű��ļ���ȫ·��
    local filename=`basename $bugfix_file`
    local revert_file=`dirname $bugfix_file`/${filename/bugfix_/revert_}
    
    #bugfix��revert�����ű��ļ������ǳɶԳ��ֵ�
    [ ! -e "$bugfix_file" ] && echo "bugfix file $bugfix_file does not exist!"&& exit 1
    [ ! -e "$revert_file" ] && echo "revert file $revert_file does not exist!"&& exit 1
    
    #���ļ�������ȡbugfix�汾�Ų��֣�ȥ����ͷ�ġ�bugfix_���ͽ�β�ġ�.sql��
    local bugfix_name=`basename $bugfix_file`
    local temp=${bugfix_name##bugfix_}
    local bugfix_version=${temp%%.sql}

    #���ļ�������ȡrevert�汾�Ų��֣�ȥ����ͷ�ġ�revert_���ͽ�β�ġ�.sql��
    local revert_name=`basename $revert_file`
    local temp=${revert_name##revert_}
    local revert_version=${temp%%.sql}
    
    echo "Checking bugfix script $bugfix_file..."
    #���bugfix�ű��ķ�ע����������Ӧ�ó���_schema_log�����ƺͰ汾�Űɣ�
    result=`grep "^[[:space:]]*[^-]" "$bugfix_file" | grep "$SCHEMA_LOG_TABLE" `
    [ -z "$result" ] && echo "invalid bugfix file $bugfix_name! String $SCHEMA_LOG_TABLE is not found!" && exit 1
    result=`grep "^[[:space:]]*[^-]" "$bugfix_file" | grep "$bugfix_version" `
    [ -z "$result" ] && echo "invalid bugfix file $bugfix_name! String $bugfix_version is not found!" && exit 1
    
    echo "Checking revert script $revert_file..."
    #���revert�ű��ķ�ע����������Ӧ�ó���_schema_log�����ƺͰ汾�Űɣ�
    result=`grep "^[[:space:]]*[^-]" "$revert_file" | grep "$SCHEMA_LOG_TABLE" `
    [ -z "$result" ] && echo "invalid revert file $revert_name! String $SCHEMA_LOG_TABLE is not found!" && exit 1
    result=`grep "^[[:space:]]*[^-]" "$revert_file" | grep "$revert_version" `
    [ -z "$result" ] && echo "invalid bugfix file $revert_name! String $revert_version is not found!" && exit 1
    return 0
}

#���updateĿ¼�¸����ű��ļ����Լ�����֮���ϵ�Ļ����Ϸ���
function validate_update_directory
{
    echo "Validating update directory $UPDATES_DIR ..."
    #����һ������bugfix֮�󣬾Ͳ���������update_*.sql��
    if [ -e "$UPDATES_DIR/bugfix_0000.sql" ]; then
        #bugfix_0000.sql�Ŀ�ͷ��¼�˺Ϸ������һ��update�ļ���
        local valid_last_update=`head -1 $UPDATES_DIR/bugfix_0000.sql | awk -F':' '{print $2}' | tr -d " "`
        valid_last_update=`echo $valid_last_update | tr -d "[:cntrl:]"`
        local actual_last_update=`find $UPDATES_DIR -name "update_*.sql" | sort --ignore-case | tail -n 1`
        actual_last_update=`basename $actual_last_update | tr -d "[:cntrl:]"`
        echo "Checking update scripts ..."
        if [ "$valid_last_update"x != "$actual_last_update"x ]; then
            echo valid_last_update=$valid_last_update
            echo actual_last_update=$actual_last_update
            echo "Updates after bugfix is not allowed!!!"
            exit 1
        fi
    fi

    #�������update�ű��б�����°汾��
    for i in `find $UPDATES_DIR -name "update_*.sql" | sort --ignore-case`
    do
        validate_update_sql $i
    done

    #��������ÿһ��bugfix_N.sql�����ж�Ӧ��revert_N.sql
    #�����ģ�bugfix�ű��б�����°汾�ţ�revert�ű��б�����˰汾��
    for i in `find $UPDATES_DIR -name "bugfix_*.sql" | sort --ignore-case`
    do
        validate_bugfix_revert_sql $i
    done
}

#׷��sql�ű��ļ�������ת��ΪUnicode without BOM��ʽ
function append_sql_file
{
    local main_file=$1
    local sql_file=$2
    local temp_file="/var/run/`uuidgen`.sql"
    [ ! -e $main_file ] && exit -1
    [ ! -e $sql_file ] && exit -1
    
    #ת��֮ǰ�������Unicode��ʽ����û��BOM������
    local is_unicode=`file $sql_file | grep "UTF-8 Unicode" -c`
    [ "$is_unicode" == 0 ] && echo "Warning! file $sql_file needs to be UTF-8 Unicode format!!!" && exit -1
    
    cp $sql_file $temp_file
    #��������ΪӢ�ģ��������Ϊ���ģ������sed����Ῠס
    export LANG="en_US.UTF-8"
    sed -i '1 s/^\xef\xbb\xbf//' $temp_file
    echo >> $main_file
    cat $temp_file >> $main_file
    rm -f $temp_file
}

#������ݿ��еĵ�ǰschema�汾��
function check_version_in_database
{
    #��ȡ��ǰ��Schema�汾��
    #1. ��ѯ���߰汾��
    local baseline_version=`$PSQL -d $DBNAME -t -c "SELECT MIN(version_number) FROM $SCHEMA_LOG_TABLE WHERE version_number NOT LIKE 'bugfix.%'"`
    current_baseline_version=${baseline_version#* }  #ȥ���ַ���ͷβ�ո�
    current_baseline_version=`echo $current_baseline_version | tr -d "[:cntrl:]"`
    
    #2. ��ѯ�����汾��
    local update_version=`$PSQL -d $DBNAME -t -c "SELECT MAX(version_number) FROM $SCHEMA_LOG_TABLE WHERE version_number NOT LIKE 'bugfix.%'"`
    current_update_version=${update_version#* }  #ȥ���ַ���ͷβ�ո�
    current_update_version=`echo $current_update_version | tr -d "[:cntrl:]"`
    
    #3. ��ѯ�����汾��
    local bugfix_version=`$PSQL -d $DBNAME -t -c "SELECT MAX(version_number) FROM $SCHEMA_LOG_TABLE WHERE version_number LIKE 'bugfix.%'"`
    current_bugfix_version=${bugfix_version#* }  #ȥ���ַ���ͷβ�ո�
    current_bugfix_version=${current_bugfix_version##bugfix.}  #ȥ���ַ�����ͷ��bugfix.
    if [ -z "$current_bugfix_version" ]; then
        #null��ʾδ����κβ���
        current_bugfix_version="null"
    fi
    current_bugfix_version=`echo $current_bugfix_version | tr -d "[:cntrl:]"`
    #echo "Current schema version: $current_update_version,current_bugfix_version"
}

#��ⰲװĿ¼�е�schema�汾��
function check_version_in_directory
{
    #1. ��ѯ�����汾��
    local last_update_file=`find $UPDATES_DIR -name "update_*.sql" | sort --ignore-case | tail -n 1`
    local last_update_name=`basename $last_update_file`
    local temp=${last_update_name##update_}
    install_update_version=${temp%%.sql}
    install_update_version=`echo $install_update_version | tr -d "[:cntrl:]"`
    
    #2. ��ѯ�����汾��
    local last_bugfix_file=`find $UPDATES_DIR -name "bugfix_*.sql" | sort --ignore-case | tail -n 1`
    if [ -z "$last_bugfix_file" ]; then
        install_bugfix_version="null"
    else
        local last_bugfix_name=`basename $last_bugfix_file`
        local temp=${last_bugfix_name##bugfix_}
        install_bugfix_version=${temp%%.sql}
    fi
    install_bugfix_version=`echo $install_bugfix_version | tr -d "[:cntrl:]"`
}

#���㿪ʼ���´����������ݿ�
function recreate_db
{
    #���ű�Ŀ¼�Ƿ������Ϸ����
    validate_update_directory
    
    #���㿪ʼ�������°汾�����ݿ�
    createdb -h $HOST -p $PORT -U $USERNAME -w $DBNAME
    RETVAL=$?
    [ $RETVAL -ne 0 ] && echo "createdb failed!" && exit $RETVAL
    
    CREATE_SQL_FILE=./createdb`date +%Y%m%d_%H:%M:%S`.sql
    echo "BEGIN;" > $CREATE_SQL_FILE
    
    #�������л��߱�
    #cat $CREATE_TABLES >> $CREATE_SQL_FILE      
    append_sql_file $CREATE_SQL_FILE $CREATE_TABLES 
    
    #�������е������ű��������ű��İ汾��˳�����Ҫ��һ��Ҫ���մӵ͵�������!!!
    for i in `find $UPDATES_DIR -name "update_*.sql" | sort --ignore-case`
    do
        #cat $i >> $CREATE_SQL_FILE
        append_sql_file $CREATE_SQL_FILE $i
    done

    #�������еĹ��ϲ����ű��������ű��İ汾��˳�����Ҫ��һ��Ҫ���մӵ͵�������!!!
    for i in `find $UPDATES_DIR -name "bugfix_*.sql" | sort --ignore-case`
    do
        #cat $i >> $CREATE_SQL_FILE
        append_sql_file $CREATE_SQL_FILE $i

        #ÿ���������֮��Ҫ����revert�ű���base64����
        #���Ƚ��ļ����е�bugfix_����revert_�õ��������˽ű��ļ���ȫ·��
        local filename=`basename $i`
        local revert_file=`dirname $i`/${filename/bugfix_/revert_}
        echo "--revert: $revert_file" >> $CREATE_SQL_FILE
        
        #���ļ�������ȡ�汾�Ų��֣�ȥ����ͷ�ġ�bugfix_���ͽ�β�ġ�.sql��
        local temp=${filename##bugfix_}
        local bugfix_version=${temp%%.sql}

        #��revert�ű���base64������µ����ݿ���
        echo "UPDATE $SCHEMA_LOG_TABLE SET revert='`base64 -w 0 $revert_file`' WHERE version_number='bugfix.$bugfix_version';" >> $CREATE_SQL_FILE
    done
    
    #����������ͼ
    #cat $CREATE_VIEWS >> $CREATE_SQL_FILE      
    append_sql_file $CREATE_SQL_FILE $CREATE_VIEWS

    echo "COMMIT;" >> $CREATE_SQL_FILE

    file $CREATE_SQL_FILE
    
    $PSQL -w -q -d $DBNAME -f $CREATE_SQL_FILE -v ON_ERROR_STOP=on 1>/dev/null
    RETVAL=$?

    if [ $RETVAL -ne 0 ]; then
        echo `pwd`
        echo "failed to create database! sql file = $CREATE_SQL_FILE"
        exit 1
    fi

    rm -f $CREATE_SQL_FILE
        
    #�������֮����汾���Ƿ���ȷ
    check_version_in_database
    check_version_in_directory

    echo current_update_version=$current_update_version
    echo current_bugfix_version=$current_bugfix_version

    if [[ "$current_update_version"  == "$install_update_version" && "$current_bugfix_version"  == "$install_bugfix_version" ]]; then
        echo "database $DBNAME is created successfully!"
        return 0
    else
        echo install_update_version=$install_update_version
        echo install_bugfix_version=$install_bugfix_version
        echo "failed to create database $DBNAME!"
        exit -1
    fi
}
#��������пͻ������������ݿ⣬��ʾ�û��ȶϿ�������û��drop��rename
function is_db_free
{    
    local client_number
    local counter
    client_number=`$PSQL -w -dtemplate1 -c "SELECT count(*) FROM pg_stat_activity WHERE datname="\'""$DBNAME""\'";"| head -n 3 | tail -n 1`
    if [ -z $client_number ]; then
        client_number=0
    fi
    
    counter=0
    while [ $counter -lt 3 ] && [ $client_number -gt 0 ]
    do
        echo "database $DBNAME is being accessed by other users, can not edit!!!"
        echo "you should do the following steps:"
        echo "step 1: service guard stop"
        echo "step 2: service tc(or cc or vnm) stop"
        echo "step 3: exit psql CLI shell and pgAdmin GUI client"
        echo -e "After done, press any key to continue ...\c "
        read answer  
        client_number=`$PSQL -w -dtemplate1 -c"SELECT count(*) FROM pg_stat_activity WHERE datname="\'""$DBNAME""\'";"| head -n 3 | tail -n 1`
        if [ -z $client_number ]; then
            client_number=0
        fi          
        counter=`expr $counter + 1`
    done
}
#ɾ�����ݿ�
function drop_db
{
    is_db_free
    echo "will drop database $DBNAME" 
    dropdb -h $HOST -p $PORT -U $USERNAME -w $DBNAME
    RETVAL=$?
    [ $RETVAL -ne 0 ] && echo "drop database $DBNAME failed!" && exit $RETVAL
}
#���������ݿ�
function rename_db
{
    is_db_free
    local BAK_DBNAME="${DBNAME}_bak_`date +%Y-%m-%d-%H-%M-%S`"
    #�������е��л����滻Ϊ�»���
    BAK_DBNAME=`echo $BAK_DBNAME|tr '-' '_'` 
    echo "will rename database $DBNAME to $BAK_DBNAME"      
    $PSQL -w -w -dtemplate1 -c"alter database "$DBNAME" rename to "$BAK_DBNAME"" 
    RETVAL=$?
    [ $RETVAL -ne 0 ] && echo "rename database $DBNAME failed!" && exit $RETVAL
}
#�������ݿ�
function backup_db
{
    local backupdb=$1
    local backupfile=$2
    if [ -z "$backupfile" ]; then 
        save_dir="/usr/local/pgsql/tecs_db_back"
        mkdir -p $save_dir
        backupfile="$save_dir/$DBNAME"
        backupfile+=`date +%Y%m%d_%H:%M:%S`
        read_string "Please input database backup file name:" $backupfile
        backupfile="$answer"
    fi
    $PG_DUMP -h $HOST -p $PORT -U $USERNAME  -w --inserts -F c -f $backupfile $backupdb
    RETVAL=$?
    [ $RETVAL -eq 0 ] && echo "Backup database $DBNAME to $backupfile successfully!"
    BACKUP_FILE=$backupfile
    [ $RETVAL -ne 0 ] && echo "Backup database $DBNAME failed!" && exit $RETVAL
}

#�ָ����ݿ�
function restore_db
{
    [ -z "$BACKUP_FILE" ] && return 0
    echo "restore scripts:$BACKUP_FILE to $DBNAME begin......"
    pg_restore -h $HOST -p $PORT -U $USERNAME -w -c -d "$DBNAME" $BACKUP_FILE
    RETVAL=$?
    [ $RETVAL -eq 0 ] &&  echo "restore file $BACKUP_FILE to database $DBNAME finished!"
    [ $RETVAL -ne 0 ] && exit $RETVAL
}
#ж�ع��������ݿ⴦��
function process_db
{  
    PS3="Please select a mode to process the datebase: " # ������ʾ���ִ�.
    select db_mode in "rename" "drop" "exit" "help"
    do
        echo
        echo "you select install mode: $db_mode."
        echo
        break 
    done   
    
    answer=$db_mode

    case $answer in
        "rename")
        rename_db
        ;; 
        "drop")
        drop_db
        ;; 
        "exit")
        exit 0;; 
        "help")
        echo "rename: will reserve the database, and rename it to \"name__'date'__bak\""
        echo "drop: will delete the database."
        echo "exit: end installtion"
        process_db
        ;;    
        *)
        echo "unknown install argument: $answer!"
        process_db
        ;;
    esac
}

export PGPASSWORD=$PASSWD
PSQL="$PSQL -h $HOST -p $PORT -U $USERNAME"

is_db_exist=`$PSQL -d template1 -w -c"SELECT datname FROM pg_database WHERE datname="\'""$DBNAME""\'";" | grep "$DBNAME"`
if [ -z "$is_db_exist" ]; then
    #������ݿ⻹�����ڣ����㿪ʼ�ؽ����ݿ�
    echo "Database $DBNAME does not exist, will create ..."
    recreate_db
    RETVAL=$?
    [ $RETVAL -ne 0 ] && echo "create database $DBNAME tables and views failed!" && exit $RETVAL
    [ $RETVAL -eq 0 ] && echo "create database $DBNAME tables and views successfully!" && exit $RETVAL
else
    #������ݿ��Ѿ����ڣ�����Ƿ����֧������
    #���ݿ��Ѿ�����ʱ��һ��Ҫ����һ�£��������ݾ�Σ����
    is_support_update=`$PSQL -d $DBNAME -t -c "SELECT tablename FROM pg_tables WHERE schemaname='public'" | grep $SCHEMA_LOG_TABLE` 
    if [ -z "$is_support_update" ]; then
        if [ "$INTERACTIVE" -eq 1 ];then
            #����ģʽʱ���ݿ⻹��֧�������������û���������drop֮�����´���
            process_db
            echo "now will recreate the database $DBNAME ..."
            recreate_db                     #���㿪ʼ�ؽ����ݿ�
            RETVAL=$?
            [ $RETVAL -eq 0 ] && echo "create database $DBNAME tables and views successfully!" || echo "create database $DBNAME tables and views failed!"
            exit $RETVAL
        else
            echo "dababase $DBNAME does doesn't support updating!"
            exit 1
        fi
    fi
fi

#��ʾһ�����ݿ��schema������ʷ
TEMPFILE=$(mktemp /tmp/schemalog-XXXXXXXX)
$PSQL -d $DBNAME -c "SELECT version_number,update_by,update_at,description FROM $SCHEMA_LOG_TABLE ORDER BY version_number" > ${TEMPFILE}
RETVAL=$?
if [ $RETVAL -ne 0 ]; then
    rm -f ${TEMPFILE}
    exit $RETVAL
fi

echo "Schema update history of database $DBNAME: "

cat ${TEMPFILE}
rm -f ${TEMPFILE}

#�����½ű��Ƿ����ػ�����Ϸ����
validate_update_directory

#����Ƿ���Ҫ����
check_version_in_database
check_version_in_directory

echo current_update_version=$current_update_version
echo current_bugfix_version=$current_bugfix_version
echo install_update_version=$install_update_version
echo install_bugfix_version=$install_bugfix_version

#������
#1. DEV�汾֮���ڲ�������bugfix_version������null��update_version���ܲ�һ����Ҳ����һ��
#2. �ѷ�����bugfix�汾�ڲ�������update_versionһ����bugfix_version��һ������������null
#3. �ѷ�����δ��bugfix�İ汾֮�����������°汾����ͬ��DEV֮������
#4. �ѷ�����bugfix�汾֮��������update_version��һ����bugfix_version����Ƚϣ���������null
#5. ���ѷ�����bugfix�汾������DEV�汾��update_version���ܲ�һ�������ߵ�bugfix_versionΪnull
#6. �ѷ�����δ��bugfix�İ汾��ʼ��bugfix��update_versionһ����bugfix_version��һ�������ݿ���Ϊnull
#7. �ѷ�����δ��bugfix�İ汾������DEV����ͬ��DEV֮������
#8. ��DEV�������ѷ�����bugfix�汾��update����һ�����������Ѿ���bugfix
#9. ��DEV�������ѷ�����δ��bugfix�İ汾��update����һ�����������߶�û��bugfix

NEED_UPDATE=0
if [[ "$current_bugfix_version" == "null" ]]; then
    #�����ǰ������û�д���������϶�����Ҫ����
    NEED_REVERT=0
else
    #����Ĭ��Ҫ����
    NEED_REVERT=1
fi

if [[ "$current_update_version" > "$install_update_version" ]]; then
    #��ǰ��update�汾���Ѿ�������������
    echo "current database $DBNAME has newer schema updates!"
    exit 0
elif [[ "$current_update_version" < "$install_update_version" ]]; then
    #��ǰ��update�汾�Ž��ϣ��϶���Ҫ���£��Ƿ����ֻҪ����û�д��bugfix
    NEED_UPDATE=1
    echo "current database $DBNAME needs to be updated!"
else
    #update�汾��һ��ʱ���϶�����Ҫ����update�����ˣ�
    NEED_UPDATE=0
    
    if [[ "$current_bugfix_version" == "$install_bugfix_version" ]]; then
        #��������汾��Ҳ��ȫ��ͬ��������ˣ�Ҳ��������
        echo "current database $DBNAME has been already updated and bugfixed!"
        exit 0
    elif [[ "$current_bugfix_version" == "null" ]]; then
        #�������ݿ�û��bugfix�����ǰ�װĿ¼�У��϶��ô򲹶�
        #���������ԱȽ��ϵİ汾���״δ򲹶���Ҳ������DEV�汾�����������汾
        echo "database schema should be bugfixed!"
    elif [[ "$install_bugfix_version" == "null" ]]; then
        #�������ݿ���bugfix�����ǰ�װĿ¼��û�У���ͼ���ϵķ�֧������dev��
        #����һ�����Ȼ�����������
        echo "No bugfix? upgrade to dev branch? Wish you success!"
        #��������Ȼ��Ҷ�����null�ˣ���ֱ��pkһ�°�
    elif [[ "$current_bugfix_version" > "$install_bugfix_version" ]]; then
        #��ǰ��bugfix�汾�Ÿ�����������
        echo "current database $DBNAME has newer bugfix!"
        exit 0
    else
        #��ǰ��bugfix�汾�Ž�С����Ҫ�ٴ򲹶����Ѿ�����Ĳ������ܻ��ˣ�����
        NEED_REVERT=0
        echo "current database $DBNAME needs to be bugfixed!"
    fi
fi

echo NEED_REVERT=$NEED_REVERT
echo NEED_UPDATE=$NEED_UPDATE

#׼������֮ǰ���û�ȷ�ϡ�����
read_bool "Update database $DBNAME schema to latest version?" "yes"
if [ $answer == "no" ]; then
    read_bool "Warning! old schema maybe unavailable for new version! Sure to ignore?" "no"
    [ $answer == "yes" ] && exit 0  #������~~~~
fi

echo "Database $DBNAME schema will be updated ..."
echo "Before schema updating, we strongly recommend to backup it first!"
#ִ����������֮ǰ���������ݿ��ȣ��������ʧ�ܣ�ֹͣ����!!!
backup_db $DBNAME

#��ʼ�����ˣ�����
#����һ����ʱ�ļ���������������Ҫִ�е�����sql��䶼��������������������֧��
UPGRADE_SQL_FILE=/var/run/upgrade`date +%Y%m%d_%H:%M:%S`.sql
touch $UPGRADE_SQL_FILE
echo "collecting upgrade statements to file $UPGRADE_SQL_FILE ..."
echo "BEGIN;"> $UPGRADE_SQL_FILE

#��һ�����ҳ�������Ҫɾ������ͼ�б���ɾ���������������ű�
views=`$PSQL -d $DBNAME -t -c "SELECT viewname FROM pg_views WHERE schemaname='public'"` 
for v in $views
do
    [ $v == "check_db_master_view" ] && continue
    echo "DROP VIEW IF EXISTS $v CASCADE;" >> $UPGRADE_SQL_FILE
done
echo "==ready to remove old views!=="

#�ڶ��������Ӳ������˽ű���������ʱ�䷴���������Ҫ������
if [[ "$NEED_REVERT" -eq 1 ]]; then
    echo "database needs to be reverted first!"
    #�����ݿ��е�revert�ű����հ汾�ŴӸߵ��͵�˳������ȡ��base64����֮�󣬷���REVERT_SQL_FILE
    $PSQL -d $DBNAME -q -t -c "SELECT revert FROM $SCHEMA_LOG_TABLE WHERE version_number LIKE 'bugfix.%' ORDER BY version_number DESC;" | base64 -i -d >> $UPGRADE_SQL_FILE
    echo "==ready to revert old bugfixes!=="
fi

#��������׼�������ű��ļ������ļ�����С�����������Ҫ������
update_count=0
for f in `find $UPDATES_DIR -type f -name "update_*.sql" | sort --ignore-case`
do
    all_update_files[$update_count]="$f"
    let update_count=update_count+1
done

#ֻ�б����ݿ��а汾�ŸߵĲ�����Ҫִ�еģ����Ի�Ҫ���ˣ�ֻ������Ҫ�õ������ű���
#���������ű��İ汾��˳�����Ҫ��һ��Ҫ���մӵ͵�������!!!
let index=0
for((i=0;i<${#all_update_files[@]};i++))
do
    #���ļ�������ȡ�汾�Ų��֣�ȥ����ͷ�ġ�update_���ͽ�β�ġ�.sql��
    update_file=`basename ${all_update_files[$i]}`
    temp=${update_file##update_}
    update_version=${temp%%.sql}
    
    #�ȵ�ǰ�汾�Ÿߵ������ű�����Ҫ���
    if [[ "$update_version" >  "$current_update_version" ]]; then
        available_updates["$index"]=$update_version
        let index+=1
    fi
done

available_update_count=${#available_updates[*]}
if [ $available_update_count -ne 0 ]; then
    #��ʾһ������Щ�����ű�����
    [ $available_update_count -eq 1 ] && echo "$available_update_count update available: "
    [ $available_update_count -gt 1 ] && echo "$available_update_count updates available: "
    for u in "${available_updates[@]}"
    do
        echo "version: $u (update_$u.sql)"
    done
    
    #�����������ű�׷�ӵ��ű���
    for u in "${available_updates[@]}"
    do
        echo >> $UPGRADE_SQL_FILE
        echo "--update: $UPDATES_DIR/update_$u.sql" >> $UPGRADE_SQL_FILE
        #cat $UPDATES_DIR/update_$u.sql >> $UPGRADE_SQL_FILE
        append_sql_file $UPGRADE_SQL_FILE $UPDATES_DIR/update_$u.sql
        echo >> $UPGRADE_SQL_FILE
    done
    echo "==ready to execute new updates!=="
fi


#���Ĳ���׼���²����ű��ļ������ļ�����С�����������Ҫ������
bugfix_count=0
for f in `find $UPDATES_DIR -type f -name "bugfix_*.sql" | sort --ignore-case`
do
    all_bugfix_files[$bugfix_count]="$f"
    let bugfix_count=bugfix_count+1
done

#�����ű��İ汾��˳�����Ҫ��һ��Ҫ���մӵ͵�������!!!
let index=0
for((i=0;i<${#all_bugfix_files[@]};i++))
do
    #���ļ�������ȡ�汾�Ų��֣�ȥ����ͷ�ġ�bugfix_���ͽ�β�ġ�.sql��
    bugfix_file=`basename ${all_bugfix_files[$i]}`
    temp=${bugfix_file##bugfix_}
    bugfix_version=${temp%%.sql}
    
    if [[ "$NEED_UPDATE" -eq 0 ]]; then
        #���ֻ���ڴ򲹶����ȵ�ǰ�汾�ŸߵĲ����ű�����Ҫ���
        if [[ "$current_bugfix_version" == "null" || "$bugfix_version" >  "$current_bugfix_version" ]]; then
            available_bugfixes["$index"]=$bugfix_version
            let index+=1
        fi
    else
        #����ǰ汾���£����°汾�����в�����Ҫ����
        available_bugfixes["$index"]=$bugfix_version
        let index+=1
    fi
done

available_bugfix_count=${#available_bugfixes[*]}
if [ $available_bugfix_count -ne 0 ]; then
    #��ʾһ������Щ�µĲ�������
    [ $available_bugfix_count -eq 1 ] && echo "$available_bugfix_count bugfix available: "
    [ $available_bugfix_count -gt 1 ] && echo "$available_bugfix_count bugfixes available: "
    for b in "${available_bugfixes[@]}"
    do
        echo "version: $b (bugfix_$b.sql)"
    done
    
    #�������µĲ����ű��ӽ���
    for b in "${available_bugfixes[@]}"
    do
        echo >> $UPGRADE_SQL_FILE
        echo "--bugfix: $UPDATES_DIR/bugfix_$b.sql" >> $UPGRADE_SQL_FILE
        #cat $UPDATES_DIR/bugfix_$b.sql >> $UPGRADE_SQL_FILE
        append_sql_file $UPGRADE_SQL_FILE $UPDATES_DIR/bugfix_$b.sql
        echo >> $UPGRADE_SQL_FILE
        
        #��update���в�ͬ���ǣ�ÿ���������֮��Ҫ����revert�ű���base64����  
        echo "--revert: $r_file" >> $UPGRADE_SQL_FILE
        echo "UPDATE $SCHEMA_LOG_TABLE SET revert='`base64 -w 0 $UPDATES_DIR/revert_$b.sql`' WHERE version_number='bugfix.$b';" >> $UPGRADE_SQL_FILE
        echo >> $UPGRADE_SQL_FILE
    done
    echo "==ready to execute new bugfixes!=="
fi

#���岽����Ҫ�������´���������ͼ�����
if [ ! -z $CREATE_VIEWS ] && [ -e $CREATE_VIEWS ]; then
    #cat $CREATE_VIEWS >> $UPGRADE_SQL_FILE
    append_sql_file $UPGRADE_SQL_FILE $CREATE_VIEWS
    echo "==ready to recreate all views!=="
fi

#���һ�б����������ύ!!!    
echo "COMMIT;">> $UPGRADE_SQL_FILE

#ȫ���������ű������ˣ��쵼Ҫ��Ҫ�ȿ���?
read_bool "upgrade statements of database $DBNAME is ready! Have a look? " "no"
if [ $answer == "yes" ]; then
    echo "----------------------begin-------------------------"
    cat $UPGRADE_SQL_FILE 
    echo "------------------------end-----------------------"
    
    read_bool "run? " "yes"
    if [ $answer == "no" ]; then
        rm -f $UPGRADE_SQL_FILE 
        exit 0
    fi
fi

#��ʼ�����ˣ�
echo "Updating ..."
$PSQL  -d $DBNAME -w -f $UPGRADE_SQL_FILE -v ON_ERROR_STOP=on 1>/dev/null
RETVAL=$?
rm -f $UPGRADE_SQL_FILE
if [ $RETVAL -ne 0 ]; then
    echo "Schema update failed!"
    read_bool "Restore from file $BACKUP_FILE?" "yes"
    restore_db
    exit $RETVAL
fi

#������ɺ󣬼��汾���Ƿ���ȷ
check_version_in_database
check_version_in_directory

echo "Now version: "
echo "update = $current_update_version"
echo "bugfix = $current_bugfix_version"

 #���������İ汾�Ų��������������ű��汾�ţ�����������ˣ�����
if [[ "$current_update_version"  == "$install_update_version" && "$current_bugfix_version"  == "$install_bugfix_version" ]]; then
    echo "Schema of database $DBNAME has been updated successfully!"
    exit 0
else
    echo "update failed, version error!!!"
    exit 1
fi
