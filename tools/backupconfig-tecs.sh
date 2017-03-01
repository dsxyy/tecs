#!/bin/sh
###############################################################################
#  tecs auto backup and auto restore script                           
#  author: zhoubin                               
#  version: 1.0                                                               
#  usage: backup tecs files and restore tecs files       
###############################################################################


PG_DUMP="/usr/local/pgsql/bin/pg_dump"
PG_RESTORE="/usr/local/pgsql/bin/pg_restore"
DEFAULT_DB_PORT="5432"
LOG_FILE=""
OPERATION=""
BACKUP_STORE=""
RESTORE_FILE=""
config_answer=""

function log_op
{
    local log_words=$1
    [  -z "$log_words" ] && exit -1
    if [ -f $LOG_FILE ];then
        echo -n `date +%Y%m%d_%H:%M:%S` >>$LOG_FILE
        echo  "    $log_words" >>$LOG_FILE
    else
        echo -n `date +%Y%m%d_%H:%M:%S` >$LOG_FILE
        echo  "    $log_words" >>$LOG_FILE    
    fi
}

## help information ##
function showhelp
{
    echo
    [ "$BOOTUP" = "color" ] && echo -en "\033[1m Usage: \033[0m" || echo -n " Usage:"
    echo " backupconfig-tecs.sh [COMMAND] [DIRECTORY] [FILE]"

    [ "$BOOTUP" = "color" ] && echo -en "\033[1m Description: \033[0m" || echo -n " Description:"
    echo " to backup configurations with option --backup or to restore configurations with option --restore"

    [ "$BOOTUP" = "color" ] && echo -e "\033[1m Example: \033[0m" || echo " Example:"
    echo " backupconfig-tecs.sh -b -d /mnt/dirx              backup configurations to directory /mnt/dirx ."
    echo " backupconfig-tecs.sh -r -f /tmp/backup.tar.gz     restore configuration file /tmp/backup.tar.gz to host."

    echo    
    [ "$BOOTUP" = "color" ] && echo -e "\033[1m Operation supported: \033[0m"  ||  echo  " Operation supported:  "

    [ "$BOOTUP" = "color" ] && echo -en "\033[1m -b,--backup: \033[0m"    || echo -n " -b,--backup: " 
    echo "  start backup tecs files , a destination directory for the backup files should be followed 
                with -d/--dir option;"

    [ "$BOOTUP" = "color" ] && echo -en "\033[1m -r,--restore: \033[0m" ||   echo -n " -r,--restore: "           
    echo " start restore tecs files, a source file should be followed with -f/--file option;"

    [ "$BOOTUP" = "color" ] && echo -en "\033[1m -d,--dir: \033[0m"  || echo -n " -d,--dir: "  
    echo "     directory for backup files;"

    [ "$BOOTUP" = "color" ] && echo -en "\033[1m -f,--file: \033[0m" || echo -n  " -f,--file: " 
    echo "    file to be restored;"
    
    [ "$BOOTUP" = "color" ] && echo -en "\033[1m -h,--help: \033[0m"  || echo -n " -h,--help: "
    echo "    show help;"
    echo
}

## parse aguments ##
function parse_args
{
    local optlist="$@" 
    local result=0
    
    while [ -n "$1" ]; do
        case $1 in
            "-b" | "--backup")
                [ "$OPERATION" = "" ] && OPERATION="backup" || result=1
                shift
                ;;
            "-r" | "--restore")
                [ "$OPERATION" = "" ] && OPERATION="restore" || result=1
                shift
                ;;
            "-d" | "--dir")
                shift
                if [ -d "$1" ];then
                    BACKUP_STORE="$1" 
                else
                    echo -e " please confirm whether \"$1\" exists . Note: the directory must be given by absolute path!\n"
                    result=1
                fi                  
                break
                ;;
            "-f" | "--file")
                shift
                if [ -e "$1" ];then
                    RESTORE_FILE="$1" 
                else
                    echo -e " please confirm whether \"$1\" exists . Note: the file must be given with absolute path!\n"
                    result=1
                fi                  
                break
                ;;                
            "-h" | "--help")
                  showhelp
                  exit 0
                ;;
            *)
                result=1
                break
        
        esac
    done
     
    if [ "$result" -ne 0 ];then
        echo " --error: invalid option \"$optlist\""
        showhelp
        exit "$result"
    fi
    operation
}

## operation menu ##
function operation_menu
{
    echo " Script options are as follows:"
    [ "$BOOTUP" = "color" ] && echo -en "\033[1m"
    echo " 1. backup"
    echo " 2. restore"
    echo " 3. help"
    echo " 4. exit"
    [ "$BOOTUP" = "color" ] && echo -en "\033[0m"
    
    echo -e " Please select an operation: \c"
    read opt
    
    case $opt in
        "1") OPERATION="backup" ;;
        "2") OPERATION="restore" ;;
        "3") OPERATION="help" ;;
        "4") OPERATION="exit" ;;
        *)
            echo " You select an invalid operation! :("
            operation_menu
        ;;
    esac
    
    operation
    
}

## operation options ##
function operation
{
    case $OPERATION in

         "backup") 
          backup_menu
         ;;
    
         "restore")
         restore_menu
         ;;

         "help")
         showhelp
         ;;

        "exit")
        echo " nothing to do?"
        exit 0
        ;;
    
       *)
       echo " unknown install argument: $OPERATION!"
       showhelp
       ;;
esac
}


function backup_menu
{
    local valid_dir=0
    while [ $valid_dir -eq 0 ]; do
        if [ -z $BACKUP_STORE ]; then
            echo -e " Please input the directory for backup files: \c."
            read BACKUP_STORE
            continue
        fi    
        if  [[  -z `echo $BACKUP_STORE |grep "^/"` ]];then
            echo -e " The directory must be absolute path, please input the directory: \c."
            read BACKUP_STORE
            continue           
        fi  

        if [ -d "$BACKUP_STORE" ];then
            valid_dir=1
        else   
            echo " Directory no exists, create directory "$BACKUP_STORE"."
            mkdir -p "$BACKUP_STORE"
            if [ -d "$BACKUP_STORE" ];then
                valid_dir=1
            else
                echo -e " Can not create directory "$BACKUP_STORE", please input the directory: \c."
                read BACKUP_STORE
                continue
            fi 
        fi           
    done 
    backup_op   

}

function restore_menu
{
  if [ -e "$RESTORE_FILE" ];then
    restore_op
  else
    echo -e " Please specify the file to be restored: \c"
    read RESTORE_FILE
    if [ -e "$RESTORE_FILE" ];then
        restore_op
    else   
        echo " File no exists,exit."
    fi    
  fi

}


function backup_op
{    
    ## prepare log file ##
    local app_host_name=`hostname`
    local app_backup_file="backup_$app_host_name-`date +%Y-%m-%d-%H-%M-%S`"
    local app_tmpdir="/tmp/$app_backup_file"
    mkdir -p "$app_tmpdir"
    LOG_FILE="$app_tmpdir/backup.log"
    touch $LOG_FILE
    
    log_op "Starting backup host $app_host_name!" 
    echo   "Starting backup host $app_host_name!" 
    
    ## execute tecsos-photo.sh ##
    echo -n "Starting execute tecsos-photo.sh, plz wait for seconds:"
    tecsos-photo.sh 1>/dev/null 2>/dev/null
    RETVAL=$?
    if [ $RETVAL -eq 0 ];then
        echo_success        
    else
        echo_failure        
        log_op "Execute tecsos-photo.sh failed,exit." 
        echo   "Execute tecsos-photo.sh failed, please check the system!" 
        rm -rf "$app_tmpdir"        
        exit -1        
    fi
    local photo_file=`ls /home -t |grep -m 1 "photo-$host_name"  | awk '{print $N}'`  
    if [ ! -z "$photo_file" ];then
        mv -f  /home/$photo_file  "$app_tmpdir/"
    fi
   
    echo  -n  "Starting backup config files:" 
    ##  backup config files ##
    cp -rf  --parents  /etc/tecs/*    "$app_tmpdir" 1>/dev/null 2>/dev/null
    cp -rf  --parents  /opt/tecs/*/etc/tecs.conf   "$app_tmpdir" 1>/dev/null 2>/dev/null
    cp -rf  --parents  /opt/tecs/network/*/etc/tecs.conf  "$app_tmpdir" 1>/dev/null 2>/dev/null
    echo_success    
    ##  backup hc module ##
    rpm -qi tecs-hc  1>/dev/null 2>/dev/null
    RETVAL=$?
    if [ $RETVAL -eq 0 ];then
        echo -n "Starting backup files of instances:"
        cp -rf  --parents  /var/lib/tecs/*.xml    "$app_tmpdir" 1>/dev/null 2>/dev/null
        if [ -d /var/lib/tecs/instances ];then
            cp -rf  --parents  /var/lib/tecs/instances/*/*.xml   "$app_tmpdir" 1>/dev/null 2>/dev/null
            cp -rf  --parents  /var/lib/tecs/instances/*/iso/*   "$app_tmpdir" 1>/dev/null 2>/dev/null
        fi
        echo_success
    fi
   
    service tecs-guard stop
    ##   backup tc cc ,vnm module ## 
    rpm -qi tecs-tc 1>/dev/null 2>/dev/null
    RETVAL=$?
    [  $RETVAL -eq 0 ]&& backup_database "tc"  "$app_tmpdir"
    
    rpm -qi tecs-cc 1>/dev/null 2>/dev/null
    RETVAL=$?
    [  $RETVAL -eq 0 ]&& backup_database "cc"  "$app_tmpdir"
    
    rpm -qi tecs-vnm 1>/dev/null 2>/dev/null
    RETVAL=$?
    [  $RETVAL -eq 0 ]&& backup_database "vnm"  "$app_tmpdir"
     
    log_op "Finish backup host $app_host_name!" 
    service tecs-guard start
    
    ##  package the backup file ##
    echo -n "Making tar package of $app_backup_file:"
    cd /tmp; tar -czvf $app_backup_file.tar.gz ./$app_backup_file 1>/dev/null
    RETVAL=$?
    if [ $RETVAL -eq 0 ];then
        echo_success 
    else
        echo_failure
        log_op "Package $app_backup_file.tar.gz failed. Exit!" 
        echo   "Package $app_backup_file.tar.gz failed. please check the system!" 
        rm -rf "$app_tmpdir"
        exit -1
    fi    
    rm -rf ./$app_backup_file
    echo " -- Generated backup file $app_backup_file.tar.gz --"
    
    ## store the backup file ## 
    if [[ $BACKUP_STORE != "/tmp" && $BACKUP_STORE != "/tmp/" ]];then 
        mv -f    $app_backup_file.tar.gz  $BACKUP_STORE
    fi
    RETVAL=$?
    [ $RETVAL -eq 0 ] && echo " -- Copy $app_backup_file.tar.gz to $BACKUP_STORE successfully --" || echo " -- Copy $app_backup_file.tar.gz to
 $BACKUP_STORE failed --"  
    
    rm -rf "$app_tmpdir"
}

function backup_database
{
    local app=$1
    local db_tmpdir=$2
    
    if [[ -z $app || -z $db_tmpdir ]];then
        echo " Bad arguments,please specify APP and Directory!"
        rm -rf "$app_tmpdir"
        exit -1
    fi    
    
    ## get db name ##      
    get_db_para  $app    
    local backup_db="$db_tmpdir/$db_name.sql"  
    
    
    
    ## stop service ## 
    service tecs-$app stop
    
    ## bakcup db  ##  
    if [ ! -z $db_name ];then
        echo -n "Starting backup database on $app:"
        export PGPASSWORD=$db_passwd
        $PG_DUMP -h $db_host -p $db_port -U $db_user  -w -c --inserts  -F c -f $backup_db $db_name 
        RETVAL=$?
        if [[ $RETVAL -eq 0 ]];then
            echo_success
            log_op  "Backup database $db_name to $backup_db successfully!"
        else
            echo_failure
            log_op  "Backup database $db_name failed! Exit." 
            echo    "Backup database $db_name failed! please check the system!"
            rm -rf "$app_tmpdir"
            service tecs-guard start 
            exit -1
        fi 
    fi   
    
  
}

function get_config
{
    local file=$1
    local key=$2

    [ ! -e $file ] && return

    local line=`sed '/^[[:space:]]*#/d' $file | sed /^[[:space:]]*$/d | grep -w "$key"| grep "$key[[:space:]]*=" -m1`
    if [ -z "$line" ]; then
        config_answer=""
    else     
        config_answer=`echo $line | sed 's/=/ /' | sed -e 's/^\w*\ *//'`
    fi
}

function restore_op
{
    echo " ###################################################################################################"
    echo -n " #"
    [ "$BOOTUP" = "color" ] && echo -en "\033[1m Warnning: \033[0m" || echo -n "Warnning: "
    echo "the backup configuration may mismacth with your system, plz be careful to restore it! #"
    echo " ###################################################################################################"

    
    ##  prepare backup files ##

    local backup_file=`echo $RESTORE_FILE | awk -F "/" '{print $NF}' `
    if [ -z `echo "$backup_file" |grep ".tar.gz"` ];then
        echo " Restore file is not a tar archive file, exit."
        exit -1
    fi
    local backup_file_dir=`echo $backup_file |awk -F "." '{print $1}'`
    if [ -z $backup_file_dir ];then
        echo " Restore file name is blank, exit."
        exit -1
    fi    
    cp -f  $RESTORE_FILE  /tmp    
    mkdir -p /tmp/$backup_file_dir   
    if [ ! -d "/tmp/$backup_file_dir" ];then
        echo " Can not create tmp directory "/tmp/$backup_file_dir", exit."
        exit -1
    fi      
    tar -zxvf  /tmp/$backup_file -C /tmp 1>/dev/null
    
    echo -n "Starting restore config files:"
    ##  restore config files  ##
    cd /tmp/$backup_file_dir
    cp -rf ./etc/tecs/*  /etc/tecs/ 1>/dev/null 2>/dev/null
    cp -rf ./opt/tecs/*  /opt/tecs/ 1>/dev/null 2>/dev/null
    echo_success

    service tecs-guard stop  
    ##  restore tc,cc,vnm module ## 
    rpm -qi tecs-tc 1>/dev/null 2>/dev/null
    RETVAL=$?
    [  $RETVAL -eq 0 ]&& restore_datebase "tc"  "/tmp/$backup_file_dir"
    
    rpm -qi tecs-cc  1>/dev/null 2>/dev/null
    RETVAL=$?
    [  $RETVAL -eq 0 ]&& restore_datebase "cc"  "/tmp/$backup_file_dir"
    
    rpm -qi tecs-vnm 1>/dev/null 2>/dev/null
    RETVAL=$?
    [  $RETVAL -eq 0 ]&& restore_datebase "vnm"  "/tmp/$backup_file_dir"
    service tecs-guard start
    
    ##  clean temp files ##
    rm -rf /tmp/$backup_file_dir
    rm -rf /tmp/$backup_file
    echo " -- Restore operation complete!"
}

function restore_datebase
{
    local app=$1
    local db_tmpdir=$2  

    ## get db name ##  
    get_db_para  $app
    local backup_db="$db_tmpdir/$db_name.sql"
    
    ## stop service  ## 
    service tecs-$app stop

    ## restore db  ##  
    if [ ! -z $db_name ];then
        echo -n "Starting restore datebase on $app:"
        export PGPASSWORD=$db_passwd
        $PG_RESTORE -h $db_host -p $db_port -U $db_user  -w -c -d $db_name $backup_db
        RETVAL=$?
        if [[ $RETVAL -eq 0 ]];then 
            echo_success
        else
            echo_failure
            echo "Restore database $db_name failed! please check the system!"
            cd /
            rm -rf /tmp/$backup_file_dir
            rm -rf /tmp/$backup_file
            service tecs-guard start  
            exit -1            
        fi      
    fi       

}

function get_db_para
{
    local app=$1
    db_host=""
    db_port=""
    db_user=""
    db_passwd=""
    db_name=""
    
    if [[ -z $app ]];then
        echo " Bad arguments£¬please specify APP and Directory!"
        exit -1
    fi  
    

    if [[  $app == "tc" ||  $app == "cc" ]];then
        local db_config_file="/opt/tecs/$app/etc/tecs.conf"
    elif [[  $app == "vnm" ]];then  
        local db_config_file="/opt/tecs/network/$app/etc/tecs.conf"
    else
        echo " No need to backup database"
        return
    fi  
    
    get_config "$db_config_file"  "db_server_url"        
    db_host=$config_answer    
        
    get_config "$db_config_file"  "db_server_port"        
    db_port=$config_answer    

    get_config "$db_config_file"  "db_user"        
    db_user=$config_answer 

    get_config "$db_config_file"  "db_passwd"        
    db_passwd=$config_answer 

    get_config "$db_config_file"  "db_name"        
    db_name=$config_answer         

    if [[ -z $db_name ]];then
        get_config "$db_config_file"  "application"     
        db_name=$config_answer 
    fi 
        
    if [ -z $db_port ];then
        db_port=$DEFAULT_DB_PORT
    fi
}
function sysconfig
{
    umask 022
    [ -z "${CONSOLETYPE:-}" ] && CONSOLETYPE="$(/sbin/consoletype)"
    if [ -z "${BOOTUP:-}" ]; then
        # This all seem confusing? Look in /etc/sysconfig/init,
        # or in /usr/doc/initscripts-*/sysconfig.txt
            BOOTUP=color
            RES_COL=60
            MOVE_TO_COL="echo -en \\033[${RES_COL}G"
            SETCOLOR_SUCCESS="echo -en \\033[1;32m"
            SETCOLOR_FAILURE="echo -en \\033[1;31m"
            SETCOLOR_WARNING="echo -en \\033[1;33m"
            SETCOLOR_NORMAL="echo -en \\033[0;39m"
            LOGLEVEL=1
        if [ "$CONSOLETYPE" = "serial" ]; then
            BOOTUP=serial
            MOVE_TO_COL=
            SETCOLOR_SUCCESS=
            SETCOLOR_FAILURE=
            SETCOLOR_WARNING=
            SETCOLOR_NORMAL=
        fi
    fi    
}
echo_success() {
  [ "$BOOTUP" = "color" ] && $MOVE_TO_COL
  echo -n "["
  [ "$BOOTUP" = "color" ] && $SETCOLOR_SUCCESS
  echo -n $"  OK  "
  [ "$BOOTUP" = "color" ] && $SETCOLOR_NORMAL
  echo -n "]"
  echo -ne "\r"
  echo
  return 0
}

echo_failure() {
  [ "$BOOTUP" = "color" ] && $MOVE_TO_COL
  echo -n "["
  [ "$BOOTUP" = "color" ] && $SETCOLOR_FAILURE
  echo -n $"FAILED"
  [ "$BOOTUP" = "color" ] && $SETCOLOR_NORMAL
  echo -n "]"
  echo -ne "\r"
  echo
  return 1
}

sysconfig
if [[ -z $@ ]];then
    operation_menu
else
    parse_args $@ 
fi

