#$language = "VBScript"
#$interface = "1.0"

crt.Screen.Synchronous = True
'------------------------------------------------------------------------------
' Copyright (c) 2012，TECS，All rights reserved.
' 
' 文件名称：LogTecs.vbs
' 文件标识：
' 内容摘要：SecureCRT日志采集控制开关
' 当前版本：1.0
' 作    者：彭伟
' 完成日期：2012/03/05
'------------------------------------------------------------------------------

Dim g_objTab
' Get a reference to the tab that was active when this script was launched.
Set g_objTab = crt.GetScriptTab

' This automatically generated script may need to be
' edited in order to work correctly.

Sub Main
	Dim LogSwitch

        '设置日志采集开关
        LogSwitch = InputBox("开启/关闭SecureCrt日志自动记录，请输入：on/off ！","日志采集开关")       
        
        Select Case LogSwitch
          '关闭所有TAB的日志记录功能
          Case "off"
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
          
          '开启所有TAB的日志记录功能
          Case "on" 
          Dim mIndex
          For mIndex = 1 to crt.GetTabCount
            Set mobjCurrentTab = crt.GetTab(mIndex)
            mobjCurrentTab.Activate
            if mobjCurrentTab.Session.Connected = True then
                mobjCurrentTab.Session.Log True,True 
            end if         
          Next
        
          ' Now, activate the original tab on which the script was started
          g_objTab.Activate
        
        End Select
End Sub


