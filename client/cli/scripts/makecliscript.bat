@echo off

rem echo 清除错误日志文件
del cliscripmakelog.txt 2>nul
rem echo 清除以前的dat文件
del  /Q *.dat 2>nul
rem echo 开始CLI在线配置脚本的转换
for %%v in (oam,cluster_manager,image_manager,version,host_manager,user,vnet,vmcfg_manager,vmtemplate_manager,alarm,project_manager) do (
    XML2Dat %%v.xml gui.xml english  %%v-en.dat  2>> cliscripmakelog.txt
    XML2Dat %%v.xml gui.xml chinese  %%v-chs.dat 2>> cliscripmakelog.txt
    
    rem 合并多语言dat到一个dat中
    copy /b %%v-en.dat+%%v-chs.dat %%v-NLS.dat 2>> cliscripmakelog.txt
    
    rem 删除中间文件
    del /Q %%v-en.dat
    del /Q %%v-chs.dat
    )
