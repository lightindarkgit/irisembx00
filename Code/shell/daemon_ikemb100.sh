#!/bin/bash
#判断进程是否存在，如果不存在就启动它
log_file="/home/irisking/shell/update_log.txt"
pass_word="root"

check_and_run_ikembx00()
{
    if [ $# < 1 ]
    then
        echo "usage error: calling without parameter of PID!!" >> "$log_file"
        return
    fi

        
    app_name="ikembx00" 

    if [ "$1" == "" ];
    then
        echo "`date`: $app_name has crashed, being restarted..."  >> "$log_file"
        echo "$pass_word" | sudo -E -S /usr/bin/ikemb100startx
#    else
#        echo "[`date` @ check_and_run_ikembx00]: $app_name is still running...." >> "$log_file"
    fi
}


check_and_run_wakescreend()
{
    if [ $# < 1 ]
    then
        echo "usage error: calling whithout parameter of PID!!!" >> "$log_file"
    fi


    if [ "$1" == "" ]
    then
        echo "`date`: wakescreend is not running!!! Now it's being actived!!!" >> "$log_file"
        echo "$pass_word" | sudo -E -S ./usr/bin/wakescreend
    fi

    
}


while true
do
    PID=`ps -ef | grep wakescreend | grep -v grep | awk '{print $2}'`
    check_and_run_wakescreend $PID


    PID=`ps -ef |grep ikembx00 |grep -v grep | awk '{print $2}'`
    check_and_run_ikembx00 $PID
    #echo "ikembx00 has crashed, being restarted..."  >> "$log_file"
    
    sleep 5
done
