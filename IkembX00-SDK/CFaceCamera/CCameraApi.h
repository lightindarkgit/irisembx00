/******************************************************************************************
** 文件名:   CCamera.h
×× 主要类:   CCameraApi
**  
** Copyright (c) 中科虹霸有限公司
** 创建人:   lizhl
** 日  期:   2016-10-31
** 修改人:   
** 日  期:
** 描  述:   IKEMBX00项目facecamera
 *×× 主要模块说明: CCamera.h                               人脸图像采集类
**
** 版  本:   1.0.0
** 备  注:   
**
*****************************************************************************************/
#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#define IN
#define OUT

//人脸摄像头采集图像尺寸信息
const int g_constFaceImgHeigth  = 480;                                              //人脸摄像头采集图片的高度
const int g_constFaceImgWidth   = 640;                                              //人脸摄像头采集图片的宽度
const int g_constFaceImgSize    = ((g_constFaceImgHeigth)*(g_constFaceImgWidth));   //人脸摄像头采集图片的尺寸

//函数调用错误码
const int E_ALGAPI_ERROR_BASE               =       0;      //基础错误码

//与人脸摄像头相关的以70**开头
const int E_ALGAPI_CAMERA_INIT_FAILED       =   E_ALGAPI_ERROR_BASE - 7001; //人脸摄像头初始化失败
const int E_ALGAPI_CAMERA_CAPIMG_FAILED     =   E_ALGAPI_ERROR_BASE - 7002; //人脸摄像头采图失败

//函数调用通用返回值
const int E_ALGAPI_OK                       =	0;                          //函数调用成功，并且完成了函数功能
const int E_ALGAPI_DEFAUL_ERROR             =	E_ALGAPI_ERROR_BASE	- 8000;	//函数调用缺省错误，一般为函数调用结果初始值，一般不可能返回该值

//虹膜摄像头采集到的图像尺寸信息
struct ImgSizeInfo
{
	int ImgHeight;	//图像高度
	int ImgWidth;	//图像宽度
	int ImgSize;	//图像尺寸
};

//前置申明
class CCameraBase;

//摄像头API类，用于采集人脸图像，该部分推后实现
class CCameraApi
{
public:
    CCameraApi(void);
    ~CCameraApi(void);
public:
	//获取设备获得的人脸图像信息的尺寸信息
    int		GetImgInfo(OUT ImgSizeInfo *imgSizeInfo);
	//初始化采集人脸图像摄像头
    int		Init();
	//采集人脸图像
    int		CapImg(OUT unsigned char *pFaceImg);
	//释放采集人脸图像摄像头相关资源
    void	Release(bool bCloseCamera = false);
private:
    CCameraBase* _pCCameraBase;
    bool _initFlag; //是否初始化标志
    CCameraApi(const CCameraApi&);//禁止拷贝赋值
    CCameraApi& operator=(const CCameraApi&);
};
