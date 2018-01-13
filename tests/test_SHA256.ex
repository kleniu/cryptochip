#!/usr/local/bin/expect -f

set data b207fff8c4a4900682d706fd49c0000445bc947161bd8fbb37c441e828818bcf84d89877f0d4041efb6bf91a16f0248f2fd573e6af05c19f96bedb9f882f7882eaade0370bc2af569929d9e5b2ad83fc76746662fcad9327d96793d40cae89c
set expsha256 52d1f274465cfecb359347f98329306a5e5ac1da3264b5c3667e70c86d7001cf


## each single character will be send with 50ms delay
set send_slow {1 .001}

set device /dev/tty.usbmodem1423

send_user "Connecting to $device \n"

spawn screen $device 115200

send "\n"
expect "> "
send "a"
expect "Enter string value : "
send -s "$data\n"
expect "> "

send_user "\nEXPEXTED SHA256 :\n"
send_user "$expsha256 \n"

## sending ctrl-a k y to exit screen session
send "\x01"; send -s "ky"

exit
