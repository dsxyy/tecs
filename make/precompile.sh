#!/bin/bash
#
# this is a script to:
# 1. check contrib's tar files, and unzip the compressed file
# 2. check tc/cc database schema files
#

CURRENT_SCRIPT=$0
QPID_PATH=$1
PGSQL_PATH=$2
XMLRPC_PATH=$3
UPDATES_PATH1=$4
UPDATES_PATH2=$5
VIEWS_PATH=$6
RESTRPC_PATH=$7
RETVAL=0

if [ ! -d $QPID_PATH ]; then
    echo contrib qpid path $QPID_PATH  does not exist!
    exit 1
fi

if [ ! -d $PGSQL_PATH ]; then
    echo contrib pgsql path $PGSQL_PATH  does not exist!
    exit 1
fi

if [ ! -d $XMLRPC_PATH ]; then
    echo contrib xmlrpc path $XMLRPC_PATH  does not exist!
    exit 1
fi

if [ ! -d $UPDATES_PATH1 ]; then
    echo sql updates path $UPDATES_PATH1 does not exist!
    exit 1
fi

if [ ! -d $UPDATES_PATH2 ]; then
    echo sql updates path $UPDATES_PATH2 does not exist!
    exit 1
fi

if [ ! -d $VIEWS_PATH ]; then
    echo sql views path $VIEWS_PATH does not exist!
    exit 1
fi

QPIDTAR=$QPID_PATH/qpid.tar.gz
PGSQLTAR=$PGSQL_PATH/postgresql.tar.gz
XMLRPCTAR=$XMLRPC_PATH/xmlrpc.tar.gz
RESTRPCTAR=$RESTRPC_PATH/restrpc-c.tar.gz

if [ ! -f $QPIDTAR ]; then
    echo contrib qpid tarfile $QPIDTAR does not exist!
    exit 1
fi

if [ ! -f $PGSQLTAR ]; then
    echo contrib postgresql tarfile $PGSQLTAR does not exist!
    exit 1
fi

if [ ! -f $XMLRPCTAR ]; then
    echo contrib xmlrpc tarfile $XMLRPCTAR does not exist!
    exit 1
fi

unzipqpid() {
    echo unzip $QPIDTAR now ...
    tar -C $QPID_PATH -zxf  $QPIDTAR ./usr/local/include ./usr/local/lib
    RETVAL=$?
    if [ $RETVAL -ne 0 ]; then
        echo unzip $QPIDTAR failed!
        return $RETVAL
    fi
    rm -rf $QPID_PATH/include
    rm -rf $QPID_PATH/lib
    mv  $QPID_PATH/usr/local/include $QPID_PATH/
    mv  $QPID_PATH/usr/local/lib $QPID_PATH/
    touch $QPID_PATH/install --reference=$QPIDTAR
    return 0
}

unzippgsql() {
    echo unzip $PGSQLTAR now ...
    tar -C $PGSQL_PATH -zxf  $PGSQLTAR usr/local/pgsql/include usr/local/pgsql/lib
    RETVAL=$?
    if [ $RETVAL -ne 0 ]; then
        echo unzip $PGSQLTAR failed!
        return $RETVAL
    fi
    rm -rf $PGSQL_PATH/include
    rm -rf $PGSQL_PATH/lib
    mv  $PGSQL_PATH/usr/local/pgsql/include $PGSQL_PATH/
    mv  $PGSQL_PATH/usr/local/pgsql/lib $PGSQL_PATH/
    touch $PGSQL_PATH/install --reference=$PGSQLTAR
    return 0
}

unziprestrpc() {
    echo unzip $RESTRPCTAR now ...
    tar -C $RESTRPC_PATH -zxf  $RESTRPCTAR
    RETVAL=$?
    if [ $RETVAL -ne 0 ]; then
        echo unzip $RESTRPCTAR failed!
        return $RETVAL
    fi
	echo ==============================================================================
	cd $RESTRPC_PATH/restrpc-c
	./configure
	make
	cd -
    touch $RESTRPC_PATH/restrpc-c/install --reference=$RESTRPC_PATH
    return 0
}

unzipxmlrpc() {
    echo unzip $XMLRPCTAR now ...
    tar -C $XMLRPC_PATH/.. -zxf  $XMLRPCTAR ./xmlrpc/include ./xmlrpc/lib
    RETVAL=$?
    if [ $RETVAL -ne 0 ]; then
        echo unzip $XMLRPCTAR failed!
        return $RETVAL
    fi
    touch $XMLRPC_PATH/install --reference=$XMLRPCTAR
    return 0
}

if [ ! -f $QPID_PATH/install ]; then
unzipqpid
RETVAL=$?
else
let needupdate=`find $QPID_PATH -name qpid.tar.gz -newer $QPID_PATH/install | wc -l`
    if [ $needupdate -eq 1 ]; then
       unzipqpid
    RETVAL=$?
 fi
fi
if [ $RETVAL -ne 0 ]; then
    echo unzip $QPID_PATH failed!
    exit $RETVAL
fi 

if [ ! -f $PGSQL_PATH/install ]; then
    unzippgsql
    RETVAL=$?
else
    let needupdate=`find $PGSQL_PATH -name postgresql.tar.gz -newer $PGSQL_PATH/install | wc -l`
    if [ $needupdate -eq 1 ]; then
           unzippgsql
        RETVAL=$?
    fi
fi

if [ $RETVAL -ne 0 ]; then
    echo unzip $PGSQLTAR failed!
    exit $RETVAL
fi

if [ ! -f $XMLRPC_PATH/install ]; then
unzipxmlrpc
RETVAL=$?
else
    let needupdate=`find $XMLRPC_PATH -name xmlrpc.tar.gz -newer $XMLRPC_PATH/install | wc -l`
    if [ $needupdate -eq 1 ]; then
        unzipxmlrpc
        RETVAL=$?
    fi
fi

if [ ! -f $RESTRPC_PATH/install ]; then
unziprestrpc
RETVAL=$?
else
    let needupdate=`find $RESTRPC_PATH -name restrpc-c.tar.gz -newer $RESTRPC_PATH/install | wc -l`
    if [ $needupdate -eq 1 ]; then
        unziprestrpc
        RETVAL=$?
    fi
fi

if [ $RETVAL -ne 0 ]; then
    echo unzip $XMLRPCTAR failed!
    exit $RETVAL
fi

function check_sql_encoding()
{
    local sql_file=$1
    #echo checking $sql_file ...
    local is_unicode_with_bom=`file $sql_file | grep "UTF-8 Unicode (with BOM)" -c`
    [ "$is_unicode_with_bom" == 0 ] && echo "Warning! file $sql_file needs to be UTF-8 Unicode (with BOM) format!!!" && exit -1
    return 0
}

echo "checking sql updates ..."
for v in `find $UPDATES_PATH1 -name "*.sql"`
do
    check_sql_encoding $v
    RETVAL=$?
    if [ $RETVAL -ne 0 ]; then
        echo check_sql_encoding $v failed!
        exit $RETVAL
    fi
done

for v in `find $UPDATES_PATH2 -name "*.sql"`
do
    check_sql_encoding $v
    RETVAL=$?
    if [ $RETVAL -ne 0 ]; then
        echo check_sql_encoding $v failed!
    exit $RETVAL
fi
done

echo "checking sql views ..."
for v in `find $VIEWS_PATH -name "*.sql"`
do
    check_sql_encoding $v
    RETVAL=$?
    if [ $RETVAL -ne 0 ]; then
        echo check_sql_encoding $v failed!
        exit $RETVAL
    fi
done
exit 0
