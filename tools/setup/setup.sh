#!/bin/bash
# tecs install and config script created by zhang wenjian
#####################################################################################
# 增加tecs setup.sh脚本必读！！！
# 1. 本安装向导的重点工作是与用户全面友好地互动问答，搜集用户的配置意图，得到答案后用心记录保存
# 2. 自己已经心里有底，用默认配置就已经能工作得很好的选项，就不要拿出来烦请领导给个意见了
# 3. 向用户提问的时候，最好给点提示，或者对自己发明的术语的解释，不要让用户风中凌乱~~~
# 4. 执行配置动作时的内部细节打印就不要显示出来了，用户没兴趣看的！真要有兴趣，也算我们泄漏机密！
# 5. 发挥好有限的英语水平，说话要温和通顺，拿不准的单词和语法怎么办？回家查了字典再写！
# 6. 除安装rpm包的动作以外，其它贯彻落实用户指示，执行配置工作的代码必须放在独立的配置脚本中供setup.sh调用
# 7. 这些被setup.sh所调用的独立配置脚本还要用于自动化安装，CI集成测试，版本自动升级等其它场合
# 8. 背后干活的独立配置脚本中就不要再缠着用户问这问那了，用户该告诉你的都已经在setup.sh中说过了
# 9、如果增加一个配置项，需要把一个配置项写成一个函数，如 config_application  ，然后在需要的地方调用
# 
#####################################################################################
# 1. 执行或调用某个脚本时，应先进入该脚本目录，并且都已
# 2. 在某个脚本中执行时，应保证当前目录是该脚本目录

#注意：全局变量名字不能重复
_SETUP_DIR=`pwd` 

#头文件包含
cd $_SETUP_DIR/common/ 
.  tecs_global_var.sh
.  tecs_common_func.sh
.  tecs_yum.sh

cd $_SETUP_DIR/install/
.  install_interface.sh

cd $_SETUP_DIR/upgrade/
.  upgrade_interface.sh

cd $_SETUP_DIR/uninstall/
.  uninstall_interface.sh

cd $_SETUP_DIR #回到当前脚本目录


function welcome
{
    echo
    echo "===================================="
    echo "    ZTE Tecs Installation Wizard"
    echo "===================================="
}

function showhelp
{
    echo
    echo "Usage: ./installtecs_(arch).bin"
    echo "operation supported: "
    
    echo "install:  start tecs install wizard, and an installation mode should be selected after the operation,
                when you completing installation, a file will be generated in \"/home/tecs_install/\" in which
                recorded your installation configure, then you can edit it and install tecs by \"conf_file\";"
                       
    echo "conf_file file:  install tecs by user-defined configuration file in which should be given an installation mode
                       first and then component's key-value configuration, and the configuration file must be For example, 
                       ./installtecs_(arch).bin conf_file /home/tecs_install/user_install_2013-12-02-22-20-10.conf;"
                          
    echo "upgrade:  upgrade tecs automatically;"

    echo "uninstall:  uninstall tecs manually;"

    echo "clean:  remove all tecs related packages on localhost automatically;"

    echo "unzip path:  unzip install.bin to an absolute path;"

    echo "help:  show help;"
    echo
}

function parse_args
{
    operation=""
    mode=""
    local optlist="$@" 
    local result=0
    
    while [ -n "$1" ]; do
        case $1 in
            "install")
                  [ "$operation" = "" ] && operation="install" || result=1
                shift
                ;;
            "custom")
                  [ "$operation" = "install" ] && mode="custom" || result=1
                  break
                ;;
            "all")
                  [ "$operation" = "install" ] && mode="all" && break
                  [ "$operation" = "clean" ] && mode="all" && break
                  result=1
                  break
                ;;
            "ctrl")
                  [ "$operation" = "install" ] && mode="ctrl" || result=1
                  break
                ;;
            "host")
                  [ "$operation" = "install" ] && mode="host" || result=1
                  break
                ;;
            "cluster")
                  [ "$operation" = "install" ] && mode="cluster" && break 
                  result=1
                  break
                ;;
            "sa")
                  [ "$operation" = "install" ] && mode="sa" || result=1
                  break
                ;;
            "conf_file")
                  shift
                  if [ -f "$1" ];then
                       user_specify_config="$1" 
                       operation="install" #conf_file方式就是定义为安装的一种方法，不需要在文件中再写入operation了，有mode就行了
                       get_config "$user_specify_config" "mode"
                       mode="$config_answer"
                  else
                       echo -e "please confirm whether \"$1\" exists specified by \"conf_file\". Note: the configure file must be given by absolute path!\n"
                       result=1
                  fi                  
                  break
                ;;
            "uninstall")
                  [ "$operation" = "" ] && operation="uninstall" || result=1
                break
                ;;
            "clean")
                  [ "$operation" = "" ] && operation="clean" || result=1
                shift 
                ;;
            "upgrade")
                  [ "$operation" = "" ] && operation="upgrade" || result=1
                  break
                ;;
            "unzip")
                  [ "$operation" = "" ] && operation="unzip" || result=1
                  break
                ;;
            "help")
                  showhelp
                  exit 0
                ;;
            *)
                result=1
                break
        
        esac
    done
     
    if [ "$result" -ne 0 ];then
         echo "error option \"$optlist\""
        showhelp
        exit "$result"
    fi
}


function unzip
{
    local path=$1
    if [ ! -e "$path" ]; then
        echo "Error! no unzip target path!"
        exit
    fi
    
    echo "begin to unzip $0 to $path, please wait ..."
    cp -fR ./* "$path"
    exit 0
}

# 检测当前系统中是否装过TECS,如果有提示是否清除
function clean_tecs
{    
    check_installed "tecs-common"
    if [[ "$has_installed" == "yes" ]];then
    # 卸载 TECS 或者 不继续进行安装，这个后续看需求
        echo -e "TECS has been already installed on this host?"
        echo -e "Warning: overwrite installation will drop all user data of tecs!"
        read_bool "Do you want new and complete overwrite installation?" "no"
        [[ $answer == "no" ]] && exit 0
        # 进行环境清理
        clean        
    fi
}

function check_need_patch
{
    need_patch=0
    user_answer=0
    
    local libvirt_default_network=/etc/libvirt/qemu/networks/autostart/default.xml
    if [ -e $libvirt_default_network ]; then
        rm -f $libvirt_default_network
    fi

    ifconfig virbr0 > /dev/null 2>&1
    if [ $? -eq 0 ];then
        ifconfig virbr0 down
        brctl delbr virbr0 > /dev/null 2>&1
    fi

    chkconfig |grep vbridge |awk '{print $4}'|grep on > /dev/null 2>&1
    if [ $? -eq 0 ];then
        need_patch=1
    fi

    if [ "$need_patch" = "1" ];then
        read_bool "to install ZXTECS 2.0,you need to do some clean up, will do it now?(notes: chose yes will reboot system)" "yes"
        if [ "$answer" = "yes" ]; then
            echo "now cleaning up ......"
            user_answer=1
            ./network_patch.sh          
        fi
    fi

    if [ "$need_patch" = "1" ] && [ "$user_answer" = "1" ];then
        exit
    fi
}

#显示一级操作菜单，让用户选择安装、配置、卸载、升级等
function operation_menu
{
    echo "1. install"
    echo "2. upgrade"
    echo "3. uninstall"
    echo "4. clean"
    echo "5. help"
    echo "6. exit"
    echo
    
    echo -e "Please select an operation: \c"
    read opt
    
    case $opt in
        "1") operation="install" ;;
        "2") operation="upgrade" ;;
        "3") operation="uninstall" ;;
        "4") operation="clean" ;;
        "5") operation="help" ;;
        "6") operation="exit" ;;
        *)
			echo "You select an invalid operation \"${opt}\"!"
            operation_menu
        ;;
    esac
}
# 二级安装菜单，operation选择install时进入此菜单
function install_menu
{      
    if [ "$need_patch" = "1" ] && [ "$user_answer" = "1" ];then
        return
    fi
    
    #确定保存用户配置的文件名字
    user_config_save_file=${user_config_save_path}/user_install_${current_time}.conf  
    [[ -f $user_config_save_file ]] && rm -rf $user_config_save_file
    echo 
    echo "===================================="
    echo "      installation mode             "
    echo "===================================="
    
    if [ "$mode" = "" ];then
        echo "1. custom   : user-defined tecs installation"
        echo "2. all      : auto install all of tecs components, include tc/cc/hc/sd/vnm/vna/wui/wdg/db/qpid/firewall/guard"
        echo "3. ctrl     : auto install ctrl node, all components except hc and wdg"
        echo "4. cluster  : auto install tecs cc/vna/db/qpid/firewall/guard"
        echo "5. host     : auto install tecs hc/vna/wdg/firewall/guard"
        echo "6. sa       : auto install tecs sa/sd/tgtd/firewall/guard"
        echo "7. exit     : exit installation"        
        echo
        
        echo -e "Please select installation mode: \c"
        read opt
        case $opt in
            "1") mode="custom" ;;
            "2") mode="all" ;;
            "3") mode="ctrl" ;;
            "4") mode="cluster" ;;
            "5") mode="host" ;;
            "6") mode="sa" ;;
            "7") mode="exit" ;;
              *)
			    echo "You select an invalid mode \"${opt}\"!"
                mode=""
                install_menu
            ;;
        esac
    fi
     
    #需要clean tecs的模式
    case $mode in
        "custom")
        custom_install
        ;; 
        
        "all")
        clean_tecs
        all_install
        ;; 
        
        "ctrl")
        clean_tecs
        ctrl_install
        ;; 
        
        "cluster")
        clean_tecs
        cluster_install
        ;;  
        
        "host")
        clean_tecs
        #install 移位后host为第一个参数，第二个如果不为空，则认为是消息服务器的地址
        host_install "$2"
        ;; 

        "sa")
        clean_tecs
        sa_install "$2"
        ;; 
        
        "exit")
        echo "exit tecs install wizard!" && exit 0
        ;;    
        
        *)
        echo "unknown install argument: $answer!"
        mode=""
        install_menu
        ;;
    esac
    
    [[ -f $user_config_save_file ]] && echo -e "\n================\nuser config has been saved to ${user_config_save_file}\n================\n"
}

welcome

#安装过程中所有需要记录安装时间的地方请读取这个全局变量
current_time=`date +%Y-%m-%d-%H-%M-%S`

# 删除/etc/rc.local下面的自动安装脚本
sed -i '/tecs-install.bin/d' /etc/rc.d/rc.local
# 删除/root/.bashrc下面的自动安装脚本
sed -i '/tecs-install.bin/d' /root/.bashrc

#tecs bin包中自带的配置文件
[ -f $_SETUP_DIR/tecs_install.conf ] && tecs_publish_config="$_SETUP_DIR/tecs_install.conf"

#获取os type 
os_type="unknown"
[ "`uname -a | grep -c el5`" -eq 1 ] && os_type="el5"
[ "`uname -a | grep -c el6`" -eq 1 ] && os_type="el6"
[[ $os_type == "unknown" ]] && { echo "unknown os type: `uname -a`"; exit -1; }

if [[ -z "$@" ]];then
    operation_menu
else
    parse_args "$@" 
fi

[ -z "$operation" ] && echo "nothing to do?" && exit

#以后安装tecs不需要升级xen时可删除
if [ -x /proc/xen ]; then
    oldxenver=`rpm  -q --queryformat '%{VERSION}.%{RELEASE}'  xen`
fi

case $operation in

    "install") 
    yum_set
    check_need_patch
    install_menu
    ;;
    
    "upgrade")
    yum_set
    upgrade_tecs
    ;;

    "uninstall")    
    yum_set    
    uninstall_tecs
    ;;

    "clean")
    clean
    ;;
    
    "unzip")
    unzip $2    
    ;;

    "help")
    showhelp
    ;;

    "exit")
    echo "nothing to do?"
    exit 0
    ;;
    
    *)
    echo "unknown install argument: $operation!"
    showhelp
    ;;
esac

if [ -x /proc/xen ]; then
    newxenver=`rpm  -q --queryformat '%{VERSION}.%{RELEASE}'  xen`
    if [  x"$newxenver" != x"$oldxenver" ];then
        echo "xen version is changed from '$oldxenver' to '$newxenver'"
        read_bool "you must reboot the system, reboot now?" "yes"
        if [ "$answer" == "yes" ]; then
            echo -e "\nreboot now..."
            reboot 
        fi
    fi
fi
