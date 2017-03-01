##���Ľ��������xend����������⴦����Ϊxend�������������ȱ�ݿ��ܻᵼ������xend���̴��ڣ�
##��������ʵ���ͽ����е�xend����ȫ��ɱ����Ȼ������xend����
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
    #�����������nfs����������⴦�����ֹ�nfs�����������н�nfsd����ֹͣ�������service nfs start��������
    #����ʹ��service nfs probe���з����飬���ݼ�������д���
    if [ "$SERVICE" = "nfs" ];then
        nfs_probe=`service nfs probe 2>/dev/null`
        [ ! -z "$nfs_probe" ] && /sbin/service nfs restart
        continue
    fi
    
    [ -x "/etc/init.d/$SERVICE" ] && /sbin/service "$SERVICE" start
    RETVAL=$?
done




