
#!/bin/bash
# 功能：tecs数据库安装升级脚本
# 作者：张文剑
# 创建时间：2012.05.25
# 参数说明：
# -i 是否交互模式
# -s 数据库服务器url
# -u 连接数据库时使用的用户名称
# -p 连接数据库时使用的用户密码
# -f 存放升级脚本的目录名称，其中：
# 升级脚本文件名称必须是update_*.sql，*部分是升级脚本的版本号
# 补丁脚本文件名称必须是bugfix_*.sql，*部分是补丁脚本的版本号
# 补丁回退脚本文件名称必须是revert_*.sql，*部分是对应补丁脚本的版本号
# 每一个补丁脚本都必须有一个对应的revert回退脚本！！！
# -d 要升级的数据库名称
# -t 要创建的表sql脚本
# -v 要创建的视图sql脚本
# 使用方法示例：./dbupdate.sh -i -s 127.0.0.1  -u tecs -p tecs  -d tecscloud -f ./tecs_updates/ -t ./tc_tables.sql -v ./tc_views.sql 

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

#命令行参数处理
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

#检查参数的合法性和有效性
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

    #非交互模式下，直接返回默认输入
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
    
    #非交互模式下，直接返回默认输入
    if [ $INTERACTIVE -eq 0 ]; then
        echo "$prompt (default: $default): $default"
        answer=$default
        return 0
    fi
    
    echo -e "$prompt (default: $default): \c "
    read answer
    [ -z $answer ] && answer="$default"
}

#检查升级脚本文件的基本合法性
function validate_update_sql
{
    local file=$1
    [ ! -e "$file" ] && echo "update file $file does not exist!"&& exit 1

    #从文件名中提取版本号部分，去掉开头的“update_”和结尾的“.sql”
    local update_name=`basename $file`
    local temp=${update_name##update_}
    local update_version=${temp%%.sql}
    
    #注意特殊情况：update_*.0000.sql只是个演示用例，无须太多检查
    [ `echo $file | grep -c ".0000.sql"` == 1 ] && return 0
    
    echo "Checking update script $file..."
    #这个update脚本中至少应该出现_schema_log表名称和版本号吧？
    result=`grep "^[[:space:]]*[^-]" "$file" | grep "$SCHEMA_LOG_TABLE" `
    [ -z "$result" ] && echo "invalid update file $file! String $SCHEMA_LOG_TABLE is not found!" && exit 1
    result=`grep "^[[:space:]]*[^-]" "$file" | grep "$update_version" `
    [ -z "$result" ] && echo "invalid update file $file! String $update_version is not found!" && exit 1
    return 0
}

#检查bugfix/revert脚本文件的基本合法性
function validate_bugfix_revert_sql
{
    local bugfix_file=$1
    
    #首先将文件名中的bugfix_换成revert_得到补丁回退脚本文件的全路径
    local filename=`basename $bugfix_file`
    local revert_file=`dirname $bugfix_file`/${filename/bugfix_/revert_}
    
    #bugfix和revert两个脚本文件必须是成对出现的
    [ ! -e "$bugfix_file" ] && echo "bugfix file $bugfix_file does not exist!"&& exit 1
    [ ! -e "$revert_file" ] && echo "revert file $revert_file does not exist!"&& exit 1
    
    #从文件名中提取bugfix版本号部分，去掉开头的“bugfix_”和结尾的“.sql”
    local bugfix_name=`basename $bugfix_file`
    local temp=${bugfix_name##bugfix_}
    local bugfix_version=${temp%%.sql}

    #从文件名中提取revert版本号部分，去掉开头的“revert_”和结尾的“.sql”
    local revert_name=`basename $revert_file`
    local temp=${revert_name##revert_}
    local revert_version=${temp%%.sql}
    
    echo "Checking bugfix script $bugfix_file..."
    #这个bugfix脚本的非注释行中至少应该出现_schema_log表名称和版本号吧？
    result=`grep "^[[:space:]]*[^-]" "$bugfix_file" | grep "$SCHEMA_LOG_TABLE" `
    [ -z "$result" ] && echo "invalid bugfix file $bugfix_name! String $SCHEMA_LOG_TABLE is not found!" && exit 1
    result=`grep "^[[:space:]]*[^-]" "$bugfix_file" | grep "$bugfix_version" `
    [ -z "$result" ] && echo "invalid bugfix file $bugfix_name! String $bugfix_version is not found!" && exit 1
    
    echo "Checking revert script $revert_file..."
    #这个revert脚本的非注释行中至少应该出现_schema_log表名称和版本号吧？
    result=`grep "^[[:space:]]*[^-]" "$revert_file" | grep "$SCHEMA_LOG_TABLE" `
    [ -z "$result" ] && echo "invalid revert file $revert_name! String $SCHEMA_LOG_TABLE is not found!" && exit 1
    result=`grep "^[[:space:]]*[^-]" "$revert_file" | grep "$revert_version" `
    [ -z "$result" ] && echo "invalid bugfix file $revert_name! String $revert_version is not found!" && exit 1
    return 0
}

#检查update目录下各个脚本文件，以及它们之间关系的基本合法性
function validate_update_directory
{
    echo "Validating update directory $UPDATES_DIR ..."
    #规则一：有了bugfix之后，就不能再增加update_*.sql了
    if [ -e "$UPDATES_DIR/bugfix_0000.sql" ]; then
        #bugfix_0000.sql的开头记录了合法的最后一个update文件名
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

    #规则二：update脚本中必须更新版本号
    for i in `find $UPDATES_DIR -name "update_*.sql" | sort --ignore-case`
    do
        validate_update_sql $i
    done

    #规则三：每一个bugfix_N.sql必须有对应的revert_N.sql
    #规则四：bugfix脚本中必须更新版本号，revert脚本中必须回退版本号
    for i in `find $UPDATES_DIR -name "bugfix_*.sql" | sort --ignore-case`
    do
        validate_bugfix_revert_sql $i
    done
}

#追加sql脚本文件，必须转换为Unicode without BOM格式
function append_sql_file
{
    local main_file=$1
    local sql_file=$2
    local temp_file="/var/run/`uuidgen`.sql"
    [ ! -e $main_file ] && exit -1
    [ ! -e $sql_file ] && exit -1
    
    #转换之前必须就是Unicode格式，有没有BOM都可以
    local is_unicode=`file $sql_file | grep "UTF-8 Unicode" -c`
    [ "$is_unicode" == 0 ] && echo "Warning! file $sql_file needs to be UTF-8 Unicode format!!!" && exit -1
    
    cp $sql_file $temp_file
    #设置语言为英文，如果语言为中文，下面的sed命令会卡住
    export LANG="en_US.UTF-8"
    sed -i '1 s/^\xef\xbb\xbf//' $temp_file
    echo >> $main_file
    cat $temp_file >> $main_file
    rm -f $temp_file
}

#检测数据库中的当前schema版本号
function check_version_in_database
{
    #获取当前的Schema版本号
    #1. 查询基线版本号
    local baseline_version=`$PSQL -d $DBNAME -t -c "SELECT MIN(version_number) FROM $SCHEMA_LOG_TABLE WHERE version_number NOT LIKE 'bugfix.%'"`
    current_baseline_version=${baseline_version#* }  #去掉字符串头尾空格
    current_baseline_version=`echo $current_baseline_version | tr -d "[:cntrl:]"`
    
    #2. 查询升级版本号
    local update_version=`$PSQL -d $DBNAME -t -c "SELECT MAX(version_number) FROM $SCHEMA_LOG_TABLE WHERE version_number NOT LIKE 'bugfix.%'"`
    current_update_version=${update_version#* }  #去掉字符串头尾空格
    current_update_version=`echo $current_update_version | tr -d "[:cntrl:]"`
    
    #3. 查询补丁版本号
    local bugfix_version=`$PSQL -d $DBNAME -t -c "SELECT MAX(version_number) FROM $SCHEMA_LOG_TABLE WHERE version_number LIKE 'bugfix.%'"`
    current_bugfix_version=${bugfix_version#* }  #去掉字符串头尾空格
    current_bugfix_version=${current_bugfix_version##bugfix.}  #去掉字符串开头的bugfix.
    if [ -z "$current_bugfix_version" ]; then
        #null表示未打过任何补丁
        current_bugfix_version="null"
    fi
    current_bugfix_version=`echo $current_bugfix_version | tr -d "[:cntrl:]"`
    #echo "Current schema version: $current_update_version,current_bugfix_version"
}

#检测安装目录中的schema版本号
function check_version_in_directory
{
    #1. 查询升级版本号
    local last_update_file=`find $UPDATES_DIR -name "update_*.sql" | sort --ignore-case | tail -n 1`
    local last_update_name=`basename $last_update_file`
    local temp=${last_update_name##update_}
    install_update_version=${temp%%.sql}
    install_update_version=`echo $install_update_version | tr -d "[:cntrl:]"`
    
    #2. 查询补丁版本号
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

#从零开始重新创建整个数据库
function recreate_db
{
    #检查脚本目录是否符合游戏规则
    validate_update_directory
    
    #从零开始创建最新版本的数据库
    createdb -h $HOST -p $PORT -U $USERNAME -w $DBNAME
    RETVAL=$?
    [ $RETVAL -ne 0 ] && echo "createdb failed!" && exit $RETVAL
    
    CREATE_SQL_FILE=./createdb`date +%Y%m%d_%H:%M:%S`.sql
    echo "BEGIN;" > $CREATE_SQL_FILE
    
    #创建所有基线表
    #cat $CREATE_TABLES >> $CREATE_SQL_FILE      
    append_sql_file $CREATE_SQL_FILE $CREATE_TABLES 
    
    #加上所有的升级脚本，升级脚本的版本号顺序很重要，一定要按照从低到高排序!!!
    for i in `find $UPDATES_DIR -name "update_*.sql" | sort --ignore-case`
    do
        #cat $i >> $CREATE_SQL_FILE
        append_sql_file $CREATE_SQL_FILE $i
    done

    #加上所有的故障补丁脚本，补丁脚本的版本号顺序很重要，一定要按照从低到高排序!!!
    for i in `find $UPDATES_DIR -name "bugfix_*.sql" | sort --ignore-case`
    do
        #cat $i >> $CREATE_SQL_FILE
        append_sql_file $CREATE_SQL_FILE $i

        #每个补丁入库之后还要附带revert脚本的base64编码
        #首先将文件名中的bugfix_换成revert_得到补丁回退脚本文件的全路径
        local filename=`basename $i`
        local revert_file=`dirname $i`/${filename/bugfix_/revert_}
        echo "--revert: $revert_file" >> $CREATE_SQL_FILE
        
        #从文件名中提取版本号部分，去掉开头的“bugfix_”和结尾的“.sql”
        local temp=${filename##bugfix_}
        local bugfix_version=${temp%%.sql}

        #将revert脚本的base64编码更新到数据库中
        echo "UPDATE $SCHEMA_LOG_TABLE SET revert='`base64 -w 0 $revert_file`' WHERE version_number='bugfix.$bugfix_version';" >> $CREATE_SQL_FILE
    done
    
    #创建所有视图
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
        
    #创建完成之后检查版本号是否正确
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
#如果正在有客户端连接着数据库，提示用户先断开，否则没法drop或rename
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
#删除数据库
function drop_db
{
    is_db_free
    echo "will drop database $DBNAME" 
    dropdb -h $HOST -p $PORT -U $USERNAME -w $DBNAME
    RETVAL=$?
    [ $RETVAL -ne 0 ] && echo "drop database $DBNAME failed!" && exit $RETVAL
}
#重命名数据库
function rename_db
{
    is_db_free
    local BAK_DBNAME="${DBNAME}_bak_`date +%Y-%m-%d-%H-%M-%S`"
    #将名字中的中划线替换为下划线
    BAK_DBNAME=`echo $BAK_DBNAME|tr '-' '_'` 
    echo "will rename database $DBNAME to $BAK_DBNAME"      
    $PSQL -w -w -dtemplate1 -c"alter database "$DBNAME" rename to "$BAK_DBNAME"" 
    RETVAL=$?
    [ $RETVAL -ne 0 ] && echo "rename database $DBNAME failed!" && exit $RETVAL
}
#备份数据库
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

#恢复数据库
function restore_db
{
    [ -z "$BACKUP_FILE" ] && return 0
    echo "restore scripts:$BACKUP_FILE to $DBNAME begin......"
    pg_restore -h $HOST -p $PORT -U $USERNAME -w -c -d "$DBNAME" $BACKUP_FILE
    RETVAL=$?
    [ $RETVAL -eq 0 ] &&  echo "restore file $BACKUP_FILE to database $DBNAME finished!"
    [ $RETVAL -ne 0 ] && exit $RETVAL
}
#卸载过程中数据库处理
function process_db
{  
    PS3="Please select a mode to process the datebase: " # 设置提示符字串.
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
    #如果数据库还不存在，从零开始重建数据库
    echo "Database $DBNAME does not exist, will create ..."
    recreate_db
    RETVAL=$?
    [ $RETVAL -ne 0 ] && echo "create database $DBNAME tables and views failed!" && exit $RETVAL
    [ $RETVAL -eq 0 ] && echo "create database $DBNAME tables and views successfully!" && exit $RETVAL
else
    #如果数据库已经存在，检查是否可以支持升级
    #数据库已经存在时，一定要交互一下，否则丢数据就危险了
    is_support_update=`$PSQL -d $DBNAME -t -c "SELECT tablename FROM pg_tables WHERE schemaname='public'" | grep $SCHEMA_LOG_TABLE` 
    if [ -z "$is_support_update" ]; then
        if [ "$INTERACTIVE" -eq 1 ];then
            #交互模式时数据库还不支持升级，建议用户重命名或drop之后重新创建
            process_db
            echo "now will recreate the database $DBNAME ..."
            recreate_db                     #从零开始重建数据库
            RETVAL=$?
            [ $RETVAL -eq 0 ] && echo "create database $DBNAME tables and views successfully!" || echo "create database $DBNAME tables and views failed!"
            exit $RETVAL
        else
            echo "dababase $DBNAME does doesn't support updating!"
            exit 1
        fi
    fi
fi

#显示一下数据库的schema升级历史
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

#检查更新脚本是否遵守基本游戏规则
validate_update_directory

#检查是否需要升级
check_version_in_database
check_version_in_directory

echo current_update_version=$current_update_version
echo current_bugfix_version=$current_bugfix_version
echo install_update_version=$install_update_version
echo install_bugfix_version=$install_bugfix_version

#场景：
#1. DEV版本之间内部升级，bugfix_version都等于null，update_version可能不一样，也可能一样
#2. 已发布的bugfix版本内部升级，update_version一样，bugfix_version不一样，但都不是null
#3. 已发布但未打bugfix的版本之间升级到更新版本，等同于DEV之间升级
#4. 已发布的bugfix版本之间升级，update_version不一样，bugfix_version无须比较，但都不是null
#5. 从已发布的bugfix版本升级到DEV版本，update_version可能不一样，后者的bugfix_version为null
#6. 已发布但未打bugfix的版本开始打bugfix，update_version一样，bugfix_version不一样，数据库中为null
#7. 已发布但未打bugfix的版本升级到DEV，等同于DEV之间升级
#8. 从DEV升级到已发布的bugfix版本，update可能一样，但后者已经有bugfix
#9. 从DEV升级到已发布但未打bugfix的版本，update可能一样，但是两者都没有bugfix

NEED_UPDATE=0
if [[ "$current_bugfix_version" == "null" ]]; then
    #如果当前根本就没有打过补丁，肯定不需要回退
    NEED_REVERT=0
else
    #否则默认要回退
    NEED_REVERT=1
fi

if [[ "$current_update_version" > "$install_update_version" ]]; then
    #当前的update版本号已经更大，无须升级
    echo "current database $DBNAME has newer schema updates!"
    exit 0
elif [[ "$current_update_version" < "$install_update_version" ]]; then
    #当前的update版本号较老，肯定需要更新，是否回退只要看有没有打过bugfix
    NEED_UPDATE=1
    echo "current database $DBNAME needs to be updated!"
else
    #update版本号一样时，肯定不需要再做update动作了：
    NEED_UPDATE=0
    
    if [[ "$current_bugfix_version" == "$install_bugfix_version" ]]; then
        #如果补丁版本号也完全相同，无须回退，也无须升级
        echo "current database $DBNAME has been already updated and bugfixed!"
        exit 0
    elif [[ "$current_bugfix_version" == "null" ]]; then
        #运行数据库没有bugfix，但是安装目录有，肯定得打补丁
        #这可能是针对比较老的版本，首次打补丁，也可能是DEV版本升级到其它版本
        echo "database schema should be bugfixed!"
    elif [[ "$install_bugfix_version" == "null" ]]; then
        #运行数据库有bugfix，但是安装目录又没有？企图从老的分支升级到dev？
        #流程一样，先回退再升级吧
        echo "No bugfix? upgrade to dev branch? Wish you success!"
        #接下来既然大家都不是null了，就直接pk一下吧
    elif [[ "$current_bugfix_version" > "$install_bugfix_version" ]]; then
        #当前的bugfix版本号更大，无须升级
        echo "current database $DBNAME has newer bugfix!"
        exit 0
    else
        #当前的bugfix版本号较小，需要再打补丁，已经打过的补丁不能回退！！！
        NEED_REVERT=0
        echo "current database $DBNAME needs to be bugfixed!"
    fi
fi

echo NEED_REVERT=$NEED_REVERT
echo NEED_UPDATE=$NEED_UPDATE

#准备升级之前让用户确认、备份
read_bool "Update database $DBNAME schema to latest version?" "yes"
if [ $answer == "no" ]; then
    read_bool "Warning! old schema maybe unavailable for new version! Sure to ignore?" "no"
    [ $answer == "yes" ] && exit 0  #很无语~~~~
fi

echo "Database $DBNAME schema will be updated ..."
echo "Before schema updating, we strongly recommend to backup it first!"
#执行升级命令之前，备份数据库先！如果备份失败，停止升级!!!
backup_db $DBNAME

#开始升级了！！！
#创建一个临时文件，将升级过程需要执行的所有sql语句都汇总起来，并增加事务支持
UPGRADE_SQL_FILE=/var/run/upgrade`date +%Y%m%d_%H:%M:%S`.sql
touch $UPGRADE_SQL_FILE
echo "collecting upgrade statements to file $UPGRADE_SQL_FILE ..."
echo "BEGIN;"> $UPGRADE_SQL_FILE

#第一步：找出所有需要删除的视图列表，将删除动作加入升级脚本
views=`$PSQL -d $DBNAME -t -c "SELECT viewname FROM pg_views WHERE schemaname='public'"` 
for v in $views
do
    [ $v == "check_db_master_view" ] && continue
    echo "DROP VIEW IF EXISTS $v CASCADE;" >> $UPGRADE_SQL_FILE
done
echo "==ready to remove old views!=="

#第二步：增加补丁回退脚本，按补丁时间反向排序很重要！！！
if [[ "$NEED_REVERT" -eq 1 ]]; then
    echo "database needs to be reverted first!"
    #将数据库中的revert脚本按照版本号从高到低的顺序反向提取，base64解码之后，放入REVERT_SQL_FILE
    $PSQL -d $DBNAME -q -t -c "SELECT revert FROM $SCHEMA_LOG_TABLE WHERE version_number LIKE 'bugfix.%' ORDER BY version_number DESC;" | base64 -i -d >> $UPGRADE_SQL_FILE
    echo "==ready to revert old bugfixes!=="
fi

#第三步：准备升级脚本文件，按文件名从小到大排序很重要！！！
update_count=0
for f in `find $UPDATES_DIR -type f -name "update_*.sql" | sort --ignore-case`
do
    all_update_files[$update_count]="$f"
    let update_count=update_count+1
done

#只有比数据库中版本号高的才是需要执行的，所以还要过滤，只保留需要用的升级脚本，
#而且升级脚本的版本号顺序很重要，一定要按照从低到高排序!!!
let index=0
for((i=0;i<${#all_update_files[@]};i++))
do
    #从文件名中提取版本号部分，去掉开头的“update_”和结尾的“.sql”
    update_file=`basename ${all_update_files[$i]}`
    temp=${update_file##update_}
    update_version=${temp%%.sql}
    
    #比当前版本号高的升级脚本都是要打的
    if [[ "$update_version" >  "$current_update_version" ]]; then
        available_updates["$index"]=$update_version
        let index+=1
    fi
done

available_update_count=${#available_updates[*]}
if [ $available_update_count -ne 0 ]; then
    #显示一下有哪些升级脚本可用
    [ $available_update_count -eq 1 ] && echo "$available_update_count update available: "
    [ $available_update_count -gt 1 ] && echo "$available_update_count updates available: "
    for u in "${available_updates[@]}"
    do
        echo "version: $u (update_$u.sql)"
    done
    
    #将所有升级脚本追加到脚本中
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


#第四步：准备新补丁脚本文件，按文件名从小到大排序很重要！！！
bugfix_count=0
for f in `find $UPDATES_DIR -type f -name "bugfix_*.sql" | sort --ignore-case`
do
    all_bugfix_files[$bugfix_count]="$f"
    let bugfix_count=bugfix_count+1
done

#补丁脚本的版本号顺序很重要，一定要按照从低到高排序!!!
let index=0
for((i=0;i<${#all_bugfix_files[@]};i++))
do
    #从文件名中提取版本号部分，去掉开头的“bugfix_”和结尾的“.sql”
    bugfix_file=`basename ${all_bugfix_files[$i]}`
    temp=${bugfix_file##bugfix_}
    bugfix_version=${temp%%.sql}
    
    if [[ "$NEED_UPDATE" -eq 0 ]]; then
        #如果只是在打补丁，比当前版本号高的补丁脚本都是要打的
        if [[ "$current_bugfix_version" == "null" || "$bugfix_version" >  "$current_bugfix_version" ]]; then
            available_bugfixes["$index"]=$bugfix_version
            let index+=1
        fi
    else
        #如果是版本更新，则新版本的所有补丁都要打上
        available_bugfixes["$index"]=$bugfix_version
        let index+=1
    fi
done

available_bugfix_count=${#available_bugfixes[*]}
if [ $available_bugfix_count -ne 0 ]; then
    #显示一下有哪些新的补丁可用
    [ $available_bugfix_count -eq 1 ] && echo "$available_bugfix_count bugfix available: "
    [ $available_bugfix_count -gt 1 ] && echo "$available_bugfix_count bugfixes available: "
    for b in "${available_bugfixes[@]}"
    do
        echo "version: $b (bugfix_$b.sql)"
    done
    
    #将所有新的补丁脚本加进来
    for b in "${available_bugfixes[@]}"
    do
        echo >> $UPGRADE_SQL_FILE
        echo "--bugfix: $UPDATES_DIR/bugfix_$b.sql" >> $UPGRADE_SQL_FILE
        #cat $UPDATES_DIR/bugfix_$b.sql >> $UPGRADE_SQL_FILE
        append_sql_file $UPGRADE_SQL_FILE $UPDATES_DIR/bugfix_$b.sql
        echo >> $UPGRADE_SQL_FILE
        
        #跟update稍有不同的是，每个补丁入库之后还要附带revert脚本的base64编码  
        echo "--revert: $r_file" >> $UPGRADE_SQL_FILE
        echo "UPDATE $SCHEMA_LOG_TABLE SET revert='`base64 -w 0 $UPDATES_DIR/revert_$b.sql`' WHERE version_number='bugfix.$b';" >> $UPGRADE_SQL_FILE
        echo >> $UPGRADE_SQL_FILE
    done
    echo "==ready to execute new bugfixes!=="
fi

#第五步：还要加入重新创建所有视图的语句
if [ ! -z $CREATE_VIEWS ] && [ -e $CREATE_VIEWS ]; then
    #cat $CREATE_VIEWS >> $UPGRADE_SQL_FILE
    append_sql_file $UPGRADE_SQL_FILE $CREATE_VIEWS
    echo "==ready to recreate all views!=="
fi

#最后一行必须是事务提交!!!    
echo "COMMIT;">> $UPGRADE_SQL_FILE

#全部的升级脚本做好了，领导要不要先看看?
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

#开始升级了！
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

#升级完成后，检查版本号是否正确
check_version_in_database
check_version_in_directory

echo "Now version: "
echo "update = $current_update_version"
echo "bugfix = $current_bugfix_version"

 #如果升级后的版本号不等于最大的升级脚本版本号，问题就严重了！！！
if [[ "$current_update_version"  == "$install_update_version" && "$current_bugfix_version"  == "$install_bugfix_version" ]]; then
    echo "Schema of database $DBNAME has been updated successfully!"
    exit 0
else
    echo "update failed, version error!!!"
    exit 1
fi
