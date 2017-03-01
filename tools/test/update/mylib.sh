#!/bin/sh
#------------------------------------------------------------------------------
# Copyright (c) 2012��FreeWorld��All rights reserved.
# 
# �ļ����ƣ�mylib.sh
# �ļ���ʶ��
# ����ժҪ���Զ��幫�ù��̿⡣
# ��ǰ�汾��1.0
# ��    �ߣ���ΰ
# ������ڣ�2012/3/13
#
# �޸ļ�¼1��
#     �޸����ڣ�2012/3/13
#     �� �� �ţ�V1.0
#     �� �� �ˣ���ΰ
#     �޸����ݣ�����
#�Զ�����
#1�����ú���������ȫСд��_�ָ
#2�����ú���������ȫСд�����ָ�����⣻
#3����ͨ����������ȫСд�����ָ
#------------------------------------------------------------------------------
#1. class:read_bool--��ʾ����ȡ�û����룻
#   arg:$1 prompt:��ʾ� $2 default:Ĭ�����룻$3 time:��ʱʱ�� 
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

#2. class:read_string--��ʾ����ȡ�û��������ã�
#   arg:$1 prompt:��ʾ� $2 default:Ĭ�����ã�$3 time:��ʱʱ�� 
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

#3. class:check_dir--���Ŀ¼�Ƿ���ڣ�
#   arg:$1 dir:Ŀ¼
#   style:check_dir $dir
function check_dir
{
  dir=$1
  if ! [ -d $dir ];then
  mkdir $dir
  chmod 753 $dir
  fi
}

