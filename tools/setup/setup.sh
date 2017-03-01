#!/bin/bash
# tecs install and config script created by zhang wenjian
#####################################################################################
# ����tecs setup.sh�ű��ض�������
# 1. ����װ�򵼵��ص㹤�������û�ȫ���Ѻõػ����ʴ��Ѽ��û���������ͼ���õ��𰸺����ļ�¼����
# 2. �Լ��Ѿ������еף���Ĭ�����þ��Ѿ��ܹ����úܺõ�ѡ��Ͳ�Ҫ�ó��������쵼���������
# 3. ���û����ʵ�ʱ����ø�����ʾ�����߶��Լ�����������Ľ��ͣ���Ҫ���û���������~~~
# 4. ִ�����ö���ʱ���ڲ�ϸ�ڴ�ӡ�Ͳ�Ҫ��ʾ�����ˣ��û�û��Ȥ���ģ���Ҫ����Ȥ��Ҳ������й©���ܣ�
# 5. ���Ӻ����޵�Ӣ��ˮƽ��˵��Ҫ�º�ͨ˳���ò�׼�ĵ��ʺ��﷨��ô�죿�ؼҲ����ֵ���д��
# 6. ����װrpm���Ķ������⣬�����᳹��ʵ�û�ָʾ��ִ�����ù����Ĵ��������ڶ��������ýű��й�setup.sh����
# 7. ��Щ��setup.sh�����õĶ������ýű���Ҫ�����Զ�����װ��CI���ɲ��ԣ��汾�Զ���������������
# 8. ����ɻ�Ķ������ýű��оͲ�Ҫ�ٲ����û����������ˣ��û��ø�����Ķ��Ѿ���setup.sh��˵����
# 9���������һ���������Ҫ��һ��������д��һ���������� config_application  ��Ȼ������Ҫ�ĵط�����
# 
#####################################################################################
# 1. ִ�л����ĳ���ű�ʱ��Ӧ�Ƚ���ýű�Ŀ¼�����Ҷ���
# 2. ��ĳ���ű���ִ��ʱ��Ӧ��֤��ǰĿ¼�Ǹýű�Ŀ¼

#ע�⣺ȫ�ֱ������ֲ����ظ�
_SETUP_DIR=`pwd` 

#ͷ�ļ�����
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

cd $_SETUP_DIR #�ص���ǰ�ű�Ŀ¼


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
                       operation="install" #conf_file��ʽ���Ƕ���Ϊ��װ��һ�ַ���������Ҫ���ļ�����д��operation�ˣ���mode������
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

# ��⵱ǰϵͳ���Ƿ�װ��TECS,�������ʾ�Ƿ����
function clean_tecs
{    
    check_installed "tecs-common"
    if [[ "$has_installed" == "yes" ]];then
    # ж�� TECS ���� ���������а�װ���������������
        echo -e "TECS has been already installed on this host?"
        echo -e "Warning: overwrite installation will drop all user data of tecs!"
        read_bool "Do you want new and complete overwrite installation?" "no"
        [[ $answer == "no" ]] && exit 0
        # ���л�������
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

#��ʾһ�������˵������û�ѡ��װ�����á�ж�ء�������
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
# ������װ�˵���operationѡ��installʱ����˲˵�
function install_menu
{      
    if [ "$need_patch" = "1" ] && [ "$user_answer" = "1" ];then
        return
    fi
    
    #ȷ�������û����õ��ļ�����
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
     
    #��Ҫclean tecs��ģʽ
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
        #install ��λ��hostΪ��һ���������ڶ��������Ϊ�գ�����Ϊ����Ϣ�������ĵ�ַ
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

#��װ������������Ҫ��¼��װʱ��ĵط����ȡ���ȫ�ֱ���
current_time=`date +%Y-%m-%d-%H-%M-%S`

# ɾ��/etc/rc.local������Զ���װ�ű�
sed -i '/tecs-install.bin/d' /etc/rc.d/rc.local
# ɾ��/root/.bashrc������Զ���װ�ű�
sed -i '/tecs-install.bin/d' /root/.bashrc

#tecs bin�����Դ��������ļ�
[ -f $_SETUP_DIR/tecs_install.conf ] && tecs_publish_config="$_SETUP_DIR/tecs_install.conf"

#��ȡos type 
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

#�Ժ�װtecs����Ҫ����xenʱ��ɾ��
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
