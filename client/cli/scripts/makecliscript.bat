@echo off

rem echo ���������־�ļ�
del cliscripmakelog.txt 2>nul
rem echo �����ǰ��dat�ļ�
del  /Q *.dat 2>nul
rem echo ��ʼCLI�������ýű���ת��
for %%v in (oam,cluster_manager,image_manager,version,host_manager,user,vnet,vmcfg_manager,vmtemplate_manager,alarm,project_manager) do (
    XML2Dat %%v.xml gui.xml english  %%v-en.dat  2>> cliscripmakelog.txt
    XML2Dat %%v.xml gui.xml chinese  %%v-chs.dat 2>> cliscripmakelog.txt
    
    rem �ϲ�������dat��һ��dat��
    copy /b %%v-en.dat+%%v-chs.dat %%v-NLS.dat 2>> cliscripmakelog.txt
    
    rem ɾ���м��ļ�
    del /Q %%v-en.dat
    del /Q %%v-chs.dat
    )
