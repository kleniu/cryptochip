LEN=$(( ( RANDOM % 200 )  + 1 ))
DEVICE=/dev/tty.usbmodem1423

function genExScr() {
cat > _ex.ex << EOF
#!/usr/local/bin/expect -f

## each single character will be send with 50ms delay
set send_slow {1 .001}

spawn screen $DEVICE 115200

send "\n"
expect "> "
send "a"
expect "Enter string value : "
send -s "$DATA\n"
expect "> "

## sending ctrl-a k y to exit screen session
send "\x01"; send -s "k"; send -s "y"
send_user "\n"
exit
EOF
}

function killscreen () {
  runloop=1
  while [[ .$runloop. == .1. ]]; do
    for session in $(screen -ls | grep -o '[0-9]\{5\}'); do
      screen -S "${session}" -X quit;
    done
    if [[ .$(screen -ls | grep -o '[0-9]\{5\}'). == .. ]]; then
      runloop=0
    else
      sleep 1
    fi
  done
}

DATA=`dd if=/dev/random bs=1 count=$LEN 2>/dev/null | base64 | cut -c1-$LEN`
EXPECTEDSHA256=`printf "$DATA" | openssl sha -sha256`

genExScr

chmod 700 _ex.ex
./_ex.ex | tee _ex.out
killscreen
rm _ex.ex
CALCULATEDSHA256=`cat _ex.out | strings | awk '/^LEN/{flag=1;next}/^CRC/{flag=0}flag'`
rm _ex.out

echo "Test data         : $DATA"
echo "Test data length  : $LEN"
echo "Calculated SHA256 : $CALCULATEDSHA256"
echo "Expected   SHA256 : $EXPECTEDSHA256"
if [[ .$CALCULATEDSHA256. == .$EXPECTEDSHA256. ]]; then
  echo "Result            : SUCCESS"
  exit 0
else
  echo "Result            : ERROR"
  exit 1
fi
