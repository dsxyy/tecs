#!/usr/bin/expect
# Realize interactive for command sensors-detect

spawn sensors-detect
while 1 {
expect -re "\n(.*): "
if {$expect_out(1,string)!=""} {
send "YES\n"
} else {
break
}
}

expect eof
exit
