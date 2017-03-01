#!/bin/sh
#���������־�ļ�
rm -f cliscripmakelog.txt
#�����ǰ��dat�ļ�
rm -f *.dat
#������Ҫת��xml�ļ�
xml_files=`ls *.xml`
#��ʼCLI�������ýű���ת��
for arg in $xml_files
do
	if [ $arg != gui.xml ]
	then
		short_f=`echo "$arg" | awk -F'.xml' '{print $1}'`
		wine XML2Dat.exe $arg gui.xml english $short_f-en.dat 2>> cliscripmakelog.txt
		wine XML2Dat.exe $arg gui.xml chinese $short_f-chs.dat 2>> cliscripmakelog.txt
		#�ϲ�������dat��һ��dat��	
		cat $short_f-en.dat $short_f-chs.dat > $short_f-NLS.dat 2>> cliscripmakelog.txt
		#ɾ���м��ļ�
		rm -f $short_f-en.dat
		rm -f $short_f-chs.dat
	fi
done