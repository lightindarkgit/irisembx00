#!/bin/bash
#Name:
#Author:
#CreateTime:
#ModifyTime
#Brief:
#Details:
#
pass_word="root"
root_path="/mnt/ikusb"
log_file="$HOME/usbmountlog.txt"
usb_dir="/mnt/ikusb"

#FUNCTIONS DEFINED START FROM HERE

mount_usb_disk()
{
    local tmp_dir=""
    # 遍历所有的设备
    for tmp_dir in `ls /dev/sd*`
    do
       udev=${tmp_dir##*/}

	# 如果是第一块磁盘,就不搭理它,10%是系统盘
       if [ "sda" = "${udev:0:3}" ];
	   then
			echo  "Ignore  $udev"
			continue
	   fi

       if [ "sd" = "${udev:0:2}" ]
       then
	   # 不是磁盘号
           if [ ${#udev} = 3 ]
           then
		   		echo  "Ignore  $udev"
               	continue
           fi

	   # 如果没有挂载目录,则创建它
           if [ ! -d "$sub_dir" ];
           then
		echo "$pass_word" | sudo mkdir -p "$usb_dir"
		if [ $? -eq 0 ]; then
			echo "$pass_word" | sudo chmod 766 "$usb_dir"
		fi
           fi

		# 如果磁盘已经被挂载,先卸载
		count=`mount |grep $sub_dir  |wc  -l`
		if [ $count -gt 0 ]; then
			echo "$pass_word" | sudo umount "$usb_dir"
		fi

	   # 挂载磁盘到指定目录
           if [ $? -eq 0 ]
           then
               	echo "$pass_word" | sudo mount "/dev/$udev" "$usb_dir" -o iocharset=utf8
               	echo  "sudo mount /dev/$udev $usb_dir"
           fi
        fi
    done
}

mount_usb_disk
