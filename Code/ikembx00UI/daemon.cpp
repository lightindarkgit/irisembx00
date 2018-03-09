/******************************************************************************************
 ** 文件名:   daemon.cpp
 ×× 主要类:   (无)
 **
 ** Copyright (c) 中科虹霸有限公司
 ** 创建人:   Wang.L
 ** 日  期:   2015-01-28
 ** 修改人:
 ** 日  期:
 ** 描  述:   守护进程实现文件
 ** ×× 主要模块说明：
 **
 ** 版  本:   1.0.0
 ** 备  注:   暂时为采用该方式
 **
 ******************************************************************************************/
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include "daemon.h"
#include "../Common/Logger.h"
#include "../Common/Exectime.h"

int allow_run = 1;


void sig_term(int signo)
{
	Exectime etm(__FUNCTION__);
	exit(0);
}


void daemonize_init()
{
	Exectime etm(__FUNCTION__);
	//TODO
	umask(0);       //Clear file creation mask

	pid_t pid;
	if(0 > (pid = fork()))
	{
		printf("error for fork call!\n");
		LOG_ERROR("error for fork call!");
		exit(1);
	}
	else if(0 < pid)
	{
		exit(0);
	}


	setsid();    //Create a new session


	struct sigaction sa;
	sa.sa_handler = SIG_IGN;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;

	if(0 > sigaction(SIGHUP, &sa, NULL))
	{
		LOG_ERROR("exit!");
		exit(-1);
	}

	if(0 < (pid = fork()))
	{
		LOG_ERROR("exit!");
		exit(-1);
	}
	else if(0 != pid)
	{
		LOG_ERROR("exit!");
		exit(0);
	}


	if(0 > chdir("/"))  //Change the current working directory to root
	{
		//printf("error for changing current working directory to directory root!\n");
	}


	//Get the max resource limit number
	struct rlimit rl;
	if(0 > getrlimit(RLIMIT_NOFILE, &rl))
	{
		LOG_ERROR("exit!");
		exit(1);
	}

	if(rl.rlim_max == RLIM_INFINITY)
	{
		rl.rlim_max = 1024;
	}

	//Close all open file descriptors
	for(uint i = 0; i < rl.rlim_max;)
	{
		close(i++);
	}

	//Attach file descriptors 0, 1, 2 to /dev/null
	open("/dev/null", O_RDWR);

	int dupRes = -1;
	dupRes = dup(0);
	dupRes = dup(0);
}

