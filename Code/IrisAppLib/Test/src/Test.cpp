/******************************************************************************************
** 文件名:   Test.cpp
×× 主要类:
**
** Copyright (c) 中科虹霸有限公司
** 创建人:   fjf
** 日  期:   2013-12-30
** 修改人:
** 日  期:
** 描  述: 自己的工程测试类
** ×× 主要模块说明：
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************************/

#include <iostream>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include "../../IrisManager/irisManager.h"
#include "../../IrisManager/singleControl.h"
int main()
{
	int size = -1;
	unsigned char*p=NULL;

	printf("11111");
	IrisManager& im = SingleControl<IrisManager>::CreateInstance();
	struct timeval start, end;
	gettimeofday(&start,NULL);
	im.LoadData();
	gettimeofday(&end,NULL);
	int inTime = 1000000*(end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec);
	im.LoadSuperData();

	im.GetIrisFeature(IdentifyType::Left,p,size);
	im.GetIrisFeature(IdentifyType::Right,p,size);
	im.GetSuperFeature(IdentifyType::Left,p,size);
	im.GetSuperFeature(IdentifyType::Right,p,size);

	im.ReleaseFeature();
	im.ReleaseSuperData();

	for (int num = 0;num < 10000;num++)
	{
	im.Init();

	im.GetIrisFeature(IdentifyType::Left,p,size);
	im.GetIrisFeature(IdentifyType::Right,p,size);
	im.GetSuperFeature(IdentifyType::Left,p,size);
	im.GetSuperFeature(IdentifyType::Right,p,size);

	im.ReleaseFeature();
	im.ReleaseSuperData();
	}

	return 0;
}
