#!/bin/bash
#Cando:
#Author: Wang.L
#CreateTime: 2014-06-06
#Details:
#

app_root="/home/irisking/ikembx00"
pass_word="123456"
log_file="/home/irisking/shell/update_log.txt"

echo "`date`: start to run $0..." >> "$log_file"

#Set touch screen's resolution to 1024*600
test -f /home/irisking/shell/xrandr_1024x600.sh  &&  /home/irisking/shell/xrandr_1024x600.sh

sleep 5 

#Update ikembx00 application
test -f /home/irisking/shell/update_ikembx00  && . /home/irisking/shell/update_ikembx00

echo "updating ikembx00..." >> "$log_file"
update_app_ikembx00


#Check file mode and execute permission
[ -x '"$app_root"/ikembx00' ] || `echo "$pass_word" | sudo chmod 777 "$app_root"/ikembx00`
 
#disable screensaver
xset s 0
xset -dpms
setterm -blank 0


ulimit -c unlimited
ulimit -s 12288

#Run application
cd "$app_root"
#echo "current directory is `pwd`, it's going to run ikembx00 application..." >> "$log_file"
#./ikembx00 >/home/irisking/oe_`date +%F.%H.%M.%S`.log&
[ ! -d /home/irisking/oe ] && /bin/mkdir /home/irisking/oe || true
./ikembx00 &>/home/irisking/oe/oe_`date +%F.%H.%M.%S`.log
#echo "$pass_word" | sudo -E -S ./ikembx00 
