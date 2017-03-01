proc dbg_puts args {
	puts $args
}
#v1,v2格式 PM_SetPrtLevel T /home/rosng/target/tmp//LINUX/DEBUG/PC/P4/CSMGR_CA/main.o:0000057d
proc is_value_equl {v1 v2} {
    if {[string compare $v1 $v2]==0} {return 0}         #完全相同，不冲突
    if {[lindex $v1 0] != [lindex $v2 0] } {return 0}   #符号不相同，不冲突
    if {[string first "/target/" $v1 ] == -1 && [string first "/target/" $v2 ] == -1} {return 0} #不是我们自己创建的符号冲突，不理会
    return 1
}

set nmexc   [lindex $argv 4]
set symbolFile [lindex $argv 3].sym.txt
set localconflictFile "[lindex $argv 5]/all_tulip_symbol.conflict"
set linktype [lindex $argv 6]
file delete $symbolFile
#set conflictFile "~/all_tulip_symbol.conflict"
set ignore_list {__libc_missing_posix_cpu_timers}
set allobj ""

#把异常信息保存在本地日志中
set fdw [open $localconflictFile a]
puts $fdw ""
puts $fdw "-----------------------------------------------"
puts $fdw [lindex $argv 3]
puts $fdw "-----------------------------------------------"
set old_all ""


#获得所有lib文件完整路径
set allfile [lindex $argv 0]
set allfile [linsert $allfile 100000 "-lc"]
foreach file $allfile {
    set file [string range $file 2 [expr [string length $file]]]
    set libexist no
    foreach path [lindex $argv 1] {
        set path [string range $path 2 [expr [string length $path]]]
        if {[string equal -nocase $linktype "_STATIC"] == 1} {
            if { [file exists "$path/lib$file.a"] } {
                set allobj [linsert $allobj 100000 "$path/lib$file.a"]
                set libexist yes 
#                puts stderr "************************ $path/lib$file.a *********1*****************"
            }
        } else {
            if { [file exists "$path/lib$file.so"] } { 
                set allobj [linsert $allobj 100000 "$path/lib$file.so"] 
                set libexist yes 
#                puts stderr "************************ $path/lib$file.so **********2****************"
            } elseif { [file exists "$path/lib$file.a"] } { 
                set allobj [linsert $allobj 100000 "$path/lib$file.a"] 
                set libexist yes 
#                puts stderr "************************ $path/lib$file.a **********2****************"
            }
        }
    }

	if { [string equal -nocase $libexist "no"]} { 
        puts $fdw "Warning:lib lib$file not exist"
#        puts stderr "Warning:lib lib$file not exist"
    }
}

#获得主程序完整路径
foreach file [lindex $argv 2] {
    if { [file exists $file] } {
            set allobj [linsert $allobj 100000 $file]
    }
}

#调用编译器中的导出符号的程序，导出库和目标文件中所有符号到 $symbolFile文件中，
foreach obj $allobj {
    catch {exec "$nmexc" "--print-file-name" "--defined-only" "$obj" | grep -v " V " | grep -v " W " | grep -v " A " | grep -v " _restfpr.*" | grep -v " _savefpr.*" | grep -v " _restgpr.*" | grep -v " _savegpr.*" | grep -v " _init" | grep -v " _fini" | grep -v " __data_start" | grep -v " _fdata" | grep -v " _ftext" >> $symbolFile 2> /dev/null"}
}

#读入所有符号，检查冲突
set fd [open $symbolFile r]
#####将每行中的符号类型与"a"比较，如果比"a"小，说明是大写字母，为全局符号，则保存到old_all列表中
while {[gets $fd ln]>=0} {
    if {[string compare [lindex $ln 1] "a"] == -1} { #ln格式 符号名 符号类型 符号所在文件，符号类型大写为全局，否则为局部
        if {[lsearch -exact $ignore_list [lindex $ln 2]] == -1} {
            lappend old_all "[lindex $ln 2] [lindex $ln 1] [lindex $ln 0]"
        }
    }
}

#####将所有符号按ASCII码顺序排序，然后比较是否有重复的符号
#####比较开头两个符号
set all [lsort $old_all]
set allcnt [llength $all]
if {$allcnt > 1 && [is_value_equl [lindex $all 0] [lindex $all 1]]} {
#	puts "Warning:[lindex $all 0]"
#    puts stderr "Warning:[lindex $all 0]"
    puts $fdw "Warning:[lindex $all 0]"
}
#####比较中间的符号
set i 1
while {$i < [expr $allcnt-1] } {
    if {[is_value_equl [lindex $all $i] [lindex $all [expr $i+1]]] || [is_value_equl [lindex $all $i] [lindex $all [expr $i-1]]]} {
#    	puts "Warning:[lindex $all $i]"
#        puts stderr "Warning:[lindex $all $i]"
        puts $fdw "Warning:[lindex $all $i]"
    }
    incr i
}
#####比较最后两个符号
if {$allcnt > 1 && [is_value_equl [lindex $all [expr $allcnt-1]] [lindex $all [expr $allcnt-2]]]} {
#	puts "Warning:[lindex $all [expr $allcnt-1]]"
#    puts stderr "Warning:[lindex $all [expr $allcnt-1]]"
    puts $fdw "Warning:[lindex $all [expr $allcnt-1]]"
}
close $fd
close $fdw

####如果检测结果的文件大小为0，说明没有检测到重复定义，删除该文件
if {[file size $localconflictFile]==0} {
    exec "rm" "$localconflictFile" "-f"
} else {
    
}


