##张文剑：这里对xend服务进行特殊处理，因为xend启动服务的流程缺陷可能会导致两个xend进程存在，
##如果检查属实，就将所有的xend进程全部杀掉，然后重启xend服务
xend_count=`ps -ef | grep -v grep | grep -c /usr/sbin/xend 2>/dev/null`
if [ "$xend_count" -gt 1 ]; then
    logger "more than one xend process! count = $xend_count, killall and restart!"
    killall -s 9 xend
    /etc/init.d/xend restart 2>/dev/null
    /etc/init.d/xend status
    RETURN=$?
    if [ "$RETURN" != 0 ]; then
        logger "failed to restart xend! status = $RETURN."
    fi
fi
####

GUARD_DIR=/etc/tecs/guard_list
[ ! -d $GUARD_DIR ] && exit 0

ALL_SERVICE=`ls $GUARD_DIR`
RETVAL=
for SERVICE in $ALL_SERVICE
do
    #高明：这里对nfs服务进行特殊处理，出现过nfs服务多个进程中仅nfsd进程停止的情况，service nfs start不起作用
    #这里使用service nfs probe进行服务检查，根据检查结果进行处理
    if [ "$SERVICE" = "nfs" ];then
        nfs_probe=`service nfs probe 2>/dev/null`
        [ ! -z "$nfs_probe" ] && /sbin/service nfs restart
        continue
    fi
    
    [ -x "/etc/init.d/$SERVICE" ] && /sbin/service "$SERVICE" start
    RETVAL=$?
done




