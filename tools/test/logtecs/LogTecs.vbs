#$language = "VBScript"
#$interface = "1.0"

crt.Screen.Synchronous = True
'------------------------------------------------------------------------------
' Copyright (c) 2012，TECS，All rights reserved.
' 
' 文件名称：LogTecs.vbs
' 文件标识：
' 内容摘要：自动采集TECS系统运行日志，并压缩打包
' 当前版本：1.0
' 作    者：彭伟
' 完成日期：2012/03/01
'------------------------------------------------------------------------------

Dim g_objTab
' Get a reference to the tab that was active when this script was launched.
Set g_objTab = crt.GetScriptTab

' This automatically generated script may need to be
' edited in order to work correctly.

Sub Main
	'crt.Screen.Send chr(13)
	'crt.Screen.WaitForString "[root@Pwvir01 " & chr(126) & "]# "
	'crt.Screen.Send "logintecs.sh" & chr(13)
	'crt.Screen.WaitForString "Please enter the index to telnet: "
	'crt.Screen.Send "5" & chr(13)
	'crt.Screen.Send chr(13)
	'crt.Screen.WaitForString "tc_shell-> "

       '设置本地日志存放的路径
        MyLogPath = "E:\TecsLog\"
        CheckFolder MyLogPath        
        
       '暂时关闭所有TAB的日志记录功能
        Dim nIndex
        For nIndex = 1 to crt.GetTabCount
            Set objCurrentTab = crt.GetTab(nIndex)
            objCurrentTab.Activate
            ' Skip tabs that aren't connected
            if objCurrentTab.Session.Connected = True then
                'objCurrentTab.Screen.Send szCommand & vbcr
                objCurrentTab.Session.Log False
            end if
        Next
    
        ' Now, activate the original tab on which the script was started
        g_objTab.Activate
        
        'Get object to SFTP tab and open tab
        Set objSFTPTab = g_objTab.ConnectSFTP                           
        objSFTPTab.Screen.Synchronous = True                             
        objSFTPTab.Screen.WaitForString ">"  

	'上传本地日志到远程LINUX主机
        objSFTPTab.Screen.Send chr(13)
        objSFTPTab.Screen.WaitForString "sftp> "
        objSFTPTab.Screen.Send "cd /var/log/tecs/TecsExcLog/" & chr(13)
        objSFTPTab.Screen.WaitForString "sftp> "
        objSFTPTab.Screen.Send "put "& MyLogPath & "*" & chr(13)
        objSFTPTab.Screen.WaitForString "sftp> "
        objSFTPTab.Screen.Send "ls -l" & chr(13)
       
        objSFTPTab.Screen.WaitForString "sftp> "
	objSFTPTab.Screen.Send "quit" & chr(13)

               
        '还原所有TAB的日志记录功能
        Dim mIndex
        For mIndex = 1 to crt.GetTabCount
            Set mobjCurrentTab = crt.GetTab(mIndex)
            mobjCurrentTab.Activate
            if mobjCurrentTab.Session.Connected = True then
                mobjCurrentTab.Session.Log True,True 
            end if         
        Next
        
        Dim lIndex
        lIndex = g_objTab.Index + 1
        crt.GetTab(lIndex).Close
        
        ' Now, activate the original tab on which the script was started
        g_objTab.Activate
        
        '执行tecs日志打包脚本
        crt.Screen.Send "logtecs.sh" & chr(13)
        crt.Dialog.MessageBox _
        "请到Linux机器的/home/tecs目录下取日志压缩包！"
        crt.Screen.WaitForString "#"
        crt.Screen.Send "cd /home/tecs"  & chr(13)
        crt.Screen.WaitForString "#"
        crt.Screen.Send "ls -l"  & chr(13)
        
End Sub

Function CheckFolder(LogPath)

   Set objFSO = CreateObject("Scripting.FileSystemObject")
   If objFSO.FolderExists(LogPath) Then
      'Set objFolder = objFSO.GetFolder(LogPath)
      crt.Dialog.MessageBox _
        "本地日志存放路径默认为E:\TecsLog\","本地日志路径"
   Else
      LogPath = InputBox("默认本地日志路径不存在，请输入正确的路径，如：D:\TecsLog\","本地日志路径")
      'objFSO.CreateFolder(LogPath)
      msgbox "本地日志存放路径为："+LogPath+"设置成功！"
   End If
End Function

