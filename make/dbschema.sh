#!/bin/bash
# tecs dbschema macro update script created by zhang wenjian
# 功能：在编译开始之前，从config/sql/updates目录查找最大版本号，更新到数据库版本号头文件中
HEADER_FILE=
UPDATE_MACRO="DBSCHEMA_UPDATE_VERSION"
BUGFIX_MACRO="DBSCHEMA_BUGFIX_VERSION"
UPDATES_DIR=

#命令行参数处理
while getopts "f:d:" option
do
    case $option in
         f) HEADER_FILE=$OPTARG ;;
         d) UPDATES_DIR=$OPTARG ;;
         ?) echo "invalid argument!"; exit 1 ;;
    esac
done

if [ -z "$HEADER_FILE" ]; then
    echo "no database schema macro header file?"
    exit -1
fi

if [ ! -e "$HEADER_FILE" ]; then
    echo "database schema version header file $HEADER_FILE does not exist!"
    exit -1
fi

if [ -z "$UPDATES_DIR" ]; then
    echo "no update script dir?"
    exit -1
fi

#echo HEADER_FILE=$HEADER_FILE
#echo UPDATES_DIR=$UPDATES_DIR

#1. 查询升级版本号
last_update_file=`find $UPDATES_DIR -name "update_*.sql" | sort --ignore-case | tail -n 1`
last_update_name=`basename $last_update_file`
temp=${last_update_name##update_}
install_update_version=${temp%%.sql}
    
#2. 查询补丁版本号
last_bugfix_file=`find $UPDATES_DIR -name "bugfix_*.sql" | sort --ignore-case | tail -n 1`
if [ -z "$last_bugfix_file" ]; then
    install_bugfix_version="null"
else
    last_bugfix_name=`basename $last_bugfix_file`
    temp="${last_bugfix_name##bugfix_}"
    install_bugfix_version="bugfix.${temp%%.sql}"
fi

current_update_version=`grep $UPDATE_MACRO $HEADER_FILE | awk -F' ' '{print $3}' | sed 's/"//' | sed 's/"//'`
current_bugfix_version=`grep $BUGFIX_MACRO $HEADER_FILE | awk -F' ' '{print $3}' | sed 's/"//' | sed 's/"//'`

#将头文件中的宏定义值和目录中采集到的值进行比较，如果一样就不更新了
if [ "$current_update_version" == "$install_update_version" ] && [ "$current_bugfix_version" == "$install_bugfix_version" ]; then
    #echo "database schema version macro is updated!"
    exit 0
fi

#更新头文件中的两个宏定义
echo "update value of macro $UPDATE_MACRO from $current_update_version to $install_update_version in $HEADER_FILE ..."
sed -i "s/^[[:space:]]*#define[[:space:]]*$UPDATE_MACRO[[:space:]]*.*/#define $UPDATE_MACRO \"$install_update_version\"/" $HEADER_FILE

echo "update value of macro $BUGFIX_MACRO from $current_bugfix_version to $install_bugfix_version in $HEADER_FILE ..."
sed -i "s/^[[:space:]]*#define[[:space:]]*$BUGFIX_MACRO[[:space:]]*.*/#define $BUGFIX_MACRO \"$install_bugfix_version\"/" $HEADER_FILE

# 更新之后再检查一下是否正确
current_update_version=`grep $UPDATE_MACRO $HEADER_FILE | awk -F' ' '{print $3}' | sed 's/"//' | sed 's/"//'`
current_bugfix_version=`grep $BUGFIX_MACRO $HEADER_FILE | awk -F' ' '{print $3}' | sed 's/"//' | sed 's/"//'`
if [ "$current_update_version" == "$install_update_version" ] && [ "$current_bugfix_version" == "$install_bugfix_version" ]; then
    #echo "database schema version macro is updated!"
    exit 0
else
    echo "failed to update macro value in $HEADER_FILE!"
    echo current_update_version=$current_update_version
    echo current_bugfix_version=$current_bugfix_version
    echo install_update_version=$install_update_version
    echo install_bugfix_version=$install_bugfix_version
    exit -1
fi

