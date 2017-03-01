proc dbg_puts args {
	puts $args
}
#v1,v2��ʽ PM_SetPrtLevel T /home/rosng/target/tmp//LINUX/DEBUG/PC/P4/CSMGR_CA/main.o:0000057d
proc is_value_equl {v1 v2} {
    if {[string compare $v1 $v2]==0} {return 0}         #��ȫ��ͬ������ͻ
    if {[lindex $v1 0] != [lindex $v2 0] } {return 0}   #���Ų���ͬ������ͻ
    if {[string first "/target/" $v1 ] == -1 && [string first "/target/" $v2 ] == -1} {return 0} #���������Լ������ķ��ų�ͻ�������
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

#���쳣��Ϣ�����ڱ�����־��
set fdw [open $localconflictFile a]
puts $fdw ""
puts $fdw "-----------------------------------------------"
puts $fdw [lindex $argv 3]
puts $fdw "-----------------------------------------------"
set old_all ""


#�������lib�ļ�����·��
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

#�������������·��
foreach file [lindex $argv 2] {
    if { [file exists $file] } {
            set allobj [linsert $allobj 100000 $file]
    }
}

#���ñ������еĵ������ŵĳ��򣬵������Ŀ���ļ������з��ŵ� $symbolFile�ļ��У�
foreach obj $allobj {
    catch {exec "$nmexc" "--print-file-name" "--defined-only" "$obj" | grep -v " V " | grep -v " W " | grep -v " A " | grep -v " _restfpr.*" | grep -v " _savefpr.*" | grep -v " _restgpr.*" | grep -v " _savegpr.*" | grep -v " _init" | grep -v " _fini" | grep -v " __data_start" | grep -v " _fdata" | grep -v " _ftext" >> $symbolFile 2> /dev/null"}
}

#�������з��ţ�����ͻ
set fd [open $symbolFile r]
#####��ÿ���еķ���������"a"�Ƚϣ������"a"С��˵���Ǵ�д��ĸ��Ϊȫ�ַ��ţ��򱣴浽old_all�б���
while {[gets $fd ln]>=0} {
    if {[string compare [lindex $ln 1] "a"] == -1} { #ln��ʽ ������ �������� ���������ļ����������ʹ�дΪȫ�֣�����Ϊ�ֲ�
        if {[lsearch -exact $ignore_list [lindex $ln 2]] == -1} {
            lappend old_all "[lindex $ln 2] [lindex $ln 1] [lindex $ln 0]"
        }
    }
}

#####�����з��Ű�ASCII��˳������Ȼ��Ƚ��Ƿ����ظ��ķ���
#####�ȽϿ�ͷ��������
set all [lsort $old_all]
set allcnt [llength $all]
if {$allcnt > 1 && [is_value_equl [lindex $all 0] [lindex $all 1]]} {
#	puts "Warning:[lindex $all 0]"
#    puts stderr "Warning:[lindex $all 0]"
    puts $fdw "Warning:[lindex $all 0]"
}
#####�Ƚ��м�ķ���
set i 1
while {$i < [expr $allcnt-1] } {
    if {[is_value_equl [lindex $all $i] [lindex $all [expr $i+1]]] || [is_value_equl [lindex $all $i] [lindex $all [expr $i-1]]]} {
#    	puts "Warning:[lindex $all $i]"
#        puts stderr "Warning:[lindex $all $i]"
        puts $fdw "Warning:[lindex $all $i]"
    }
    incr i
}
#####�Ƚ������������
if {$allcnt > 1 && [is_value_equl [lindex $all [expr $allcnt-1]] [lindex $all [expr $allcnt-2]]]} {
#	puts "Warning:[lindex $all [expr $allcnt-1]]"
#    puts stderr "Warning:[lindex $all [expr $allcnt-1]]"
    puts $fdw "Warning:[lindex $all [expr $allcnt-1]]"
}
close $fd
close $fdw

####�����������ļ���СΪ0��˵��û�м�⵽�ظ����壬ɾ�����ļ�
if {[file size $localconflictFile]==0} {
    exec "rm" "$localconflictFile" "-f"
} else {
    
}


