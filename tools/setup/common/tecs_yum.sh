#!/bin/bash

# 提供yum设置相关函数

if [ ! "$_TECS_YUM_FILE" ];then

function make_yum_server
{
    path=`pwd`   
    createrepo --update $path &>/dev/null
    [ "$?" -ne 0 ] && { echo "createrepo in $path failed"; exit 1; }
    patch_path="/home/tecsos_patch/"
    if [ -d $patch_path ];then  
        #避免旧的repodata被破坏，导致createrepo命令失败
        [ -d $patch_path/repodata ] && rm -rf $patch_path/repodata
        
        createrepo --update $patch_path &>/dev/null
        [ "$?" -ne 0 ] && { echo "createrepo in $patch_path failed"; exit 1; }
    fi
}

# 产生REPO客户端的文件
function make_yum_client
{    
    path=`pwd`
    repo_name=`uuidgen`
    patch_repo_name=tecs_depend
    tecs_repofile=/etc/yum.repos.d/Tecs_install_temp.repo 
    tecs_depend_repofile=/etc/yum.repos.d/Tecs_depend_temp.repo   
  
    echo -e "[$repo_name]\nname=$repo_name\nbaseurl=file:$path\nenabled=1\ngpgcheck=0" > $tecs_repofile

    if [ -d $patch_path ];then 
    echo -e "[$patch_repo_name]\nname=$patch_repo_name\nbaseurl=file:$patch_path\nenabled=1\ngpgcheck=0" > $tecs_depend_repofile   
        tecs_yum="yum -y --disablerepo=* --enablerepo=$repo_name --enablerepo=$patch_repo_name"
    else
        tecs_yum="yum -y --disablerepo=* --enablerepo=$repo_name"
    fi

    yum clean all &>/dev/null

}
function yum_set
{
    #防止rpm数据库出错，先删除（执行了它的命令后会自动重新创建）
    rm -rf /var/lib/rpm/__db.*
    echo "creating yum repo, please wait for several seconds..."
    make_yum_server
    make_yum_client

}

fi 
export _TECS_YUM_FILE="tecs_yum.sh"
