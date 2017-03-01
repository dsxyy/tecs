#!/bin/sh
#------------------------------------------------------------------------------
# Copyright (c) 2012，FreeWorld，All rights reserved.
# 
# 文件名称：mylib.sh
# 文件标识：
# 内容摘要：自定义公用过程库。
# 当前版本：1.0
# 作    者：彭伟
# 完成日期：2012/3/13
#
# 修改记录1：
#     修改日期：2012/3/13
#     版 本 号：V1.0
#     修 改 人：彭伟
#     修改内容：创建
#自定义风格：
#1）公用函数命名：全小写，_分割；
#2）公用函数参数：全小写，不分割，带语意；
#3）普通函数命名：全小写，不分割；
#------------------------------------------------------------------------------
#1. class:read_bool--提示并读取用户输入；
#   arg:$1 prompt:提示语； $2 default:默认输入；$3 time:超时时间 
#   style:read_bool $prompt $defult $time
function read_bool 
{
    local prompt=$1
    local default=$2
    local input
    local time=$3
    
    if [ -z $time ];then
        time=8
    fi
    echo -e "$prompt (y/n): \c "
    read -t $time input
    case $input in
            "Y")    
            input="yes" ;;
            "y")    
            input="yes";;
            "n")    
            input="no";;
            "N")    
            input="no";;
            "")             
            input="$default"
            echo -e "\n";;
            *)       
            input="$default"
            echo -e "\n";;
    esac
    answer=$input
    return 0
}

#2. class:read_string--提示并读取用户输入配置；
#   arg:$1 prompt:提示语； $2 default:默认配置；$3 time:超时时间 
#   style:read_string $prompt $defult $time
function read_string
{
    local prompt=$1
    local default=$2
    local input
    local time=$3
    
    if [ -z $time ];then
        time=8
    fi
    echo -e "$prompt (default: $default): \c "
    read -t $time input
    if [ -z $input ]; then
        answer="$default"
        echo -e "\n"
        return 0
    fi
    echo -e "\n"
    answer=$input
}

#3. class:check_dir--检查目录是否存在；
#   arg:$1 dir:目录
#   style:check_dir $dir
function check_dir
{
  dir=$1
  if ! [ -d $dir ];then
  mkdir $dir
  chmod 753 $dir
  fi
}

