/******************************************************************************************
 ** 文件名:   simutouchevent.cpp
 ×× 主要类:   (无)
 **
 ** Copyright (c) 中科虹霸有限公司
 ** 创建人:   Wang.L
 ** 日  期:   2015-01-28
 ** 修改人:
 ** 日  期:
 ** 描  述:  模拟触屏点击操作事件实现
 ** ×× 主要模块说明：
 **
 ** 版  本:   1.0.0
 ** 备  注:
 **
 ******************************************************************************************/
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include "../Common/Exectime.h"

#define COMMENT_OFF_BEGIN if(0){
#define COMMENT_OFF_END }


void simulate_touch(int fd)
{
	Exectime etm(__FUNCTION__);
	int wrSz = 0;
	struct input_event event, ev;
	memset(&event, 0, sizeof(event));
	gettimeofday(&event.time, NULL);

	event.type = EV_ABS;
	event.code = ABS_X;
	event.value = 20;
	wrSz = write(fd, &event, sizeof(event));

	event.type = EV_ABS;
	event.code = ABS_Y;
	event.value = 20;
	wrSz = write(fd, &event, sizeof(event));


	event.type = EV_KEY;
	event.code = BTN_TOUCH;
	event.value = 1;
	wrSz = write(fd, &event, sizeof(event));

	event.type = EV_ABS;
	event.code = ABS_PRESSURE;
	event.code = 1;
	wrSz = write(fd, &event, sizeof(event));

	event.type = EV_SYN;
	event.code = SYN_REPORT;
	event.value = 0;
	wrSz = write(fd, &event, sizeof(event));
}


std::string  get_penmount_event()
{
	Exectime etm(__FUNCTION__);
	FILE* fd;
	size_t readLen;
	char* line = NULL;
	std::string devEventTag("");

	fd = popen("cat /proc/bus/input/devices | grep -A4 \"DIALOGUE INC PenMount USB\" | grep \"event\" | cut -d' ' -f3- | awk -F\" \" '{print $1}'", "r");
	if(-1 != getline(&line, &readLen, fd))
	{
		//TODO
		int logCmdRes = -1;
		logCmdRes = system("echo \"`date`: touchscreen event num: `cat /proc/bus/input/devices | grep -A4 \"DIALOGUE INC PenMount USB\" | grep \"event\" `\" >> \"$HOME/touchscreen_event.txt\"");
		devEventTag = std::string(line);
		devEventTag.erase(devEventTag.find("\n"));
	} 

	//Free heap buffer as needed
	if(line)
	{
		free(line);
	}

	//Close file handle
	pclose(fd);

	return devEventTag;
}

int open_touchscreent_dev()
{
	Exectime etm(__FUNCTION__);
	std::string devFile("/dev/input/");
	if(0 == get_penmount_event().length())
	{
		return -1;
	}

	devFile += get_penmount_event();
	printf("comment off closed! %s\n", devFile.c_str());

	return open(devFile.c_str(), O_RDWR);
}

void simu_touch_event(int fd)
{
	Exectime etm(__FUNCTION__);
	simulate_touch(fd);
}

