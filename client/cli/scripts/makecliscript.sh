#!/bin/sh
#清除错误日志文件
rm -f cliscripmakelog.txt
#清除以前的dat文件
rm -f *.dat
#定义需要转换xml文件
xml_files=`ls *.xml`
#开始CLI在线配置脚本的转换
for arg in $xml_files
do
	if [ $arg != gui.xml ]
	then
		short_f=`echo "$arg" | awk -F'.xml' '{print $1}'`
		wine XML2Dat.exe $arg gui.xml english $short_f-en.dat 2>> cliscripmakelog.txt
		wine XML2Dat.exe $arg gui.xml chinese $short_f-chs.dat 2>> cliscripmakelog.txt
		#合并多语言dat到一个dat中	
		cat $short_f-en.dat $short_f-chs.dat > $short_f-NLS.dat 2>> cliscripmakelog.txt
		#删除中间文件
		rm -f $short_f-en.dat
		rm -f $short_f-chs.dat
	fi
done