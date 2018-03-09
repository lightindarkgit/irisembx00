/******************************************************************************************
 ** 文件名:   wakescreen.cpp
 ×× 主要类:   (无)
 **
 ** Copyright (c) 中科虹霸有限公司
 ** 创建人:   Wang.L
 ** 日  期:   2015-01-28
 ** 修改人:
 ** 日  期:
 ** 描  述:   模拟触屏点击操作
 ** ×× 主要模块说明：
 **
 ** 版  本:   1.0.0
 ** 备  注:
 **
 ******************************************************************************************/
#include "wakescreen.h"
#include "simutouchevent.h"
#include "daemon.h"
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include "../Common/Logger.h"
#include "../Common/Exectime.h"

void* wakescreend(void *arg)
{
	Exectime etm(__FUNCTION__);
	int fd = open_touchscreent_dev();
	if(fd <= 0)
	{
		std::cout << "[" << __FUNCTION__  << "]"
			<< "open error,and exit" << std::endl;
		LOG_ERROR("open error,and exit");
		return (void *)0;
	}

	//        struct sigaction sa;
	//        //sa.sa_handler = sig_term;
	//        signal(SIGTERM, sig_term);

	//    daemonize_init();    //Init the progress as a daemon one

	while(1)
	{
		int cmdRes = 0;
		cmdRes = system("echo \"`date`: wakescreend has been waked to work\" >> \"/tmp/ik_log.txt\"");

		simu_touch_event(fd);
		sleep(30);     //For the screen turn off in at least 1 minute
	}

	close(fd);

	return (void *)0;
}

