/******************************************************************************************
** 文件名:   common.h
×× 主要类:   (无)
**  
** Copyright (c) 中科虹霸有限公司
** 创建人:   fjf
** 日  期:   2013-11-06
** 修改人:   
** 日  期:
** 描  述:   公共头文件定义
** ×× 主要模块说明：                    
**
** 版  本:   1.0.0
** 备  注:   
**
******************************************************************************************/
#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <iostream>


//参数输入输出指示宏定义
#define IN
#define OUT

//常量定义
const int SEX_MALE = 0;     //性别： 男
const int SEX_FAMALE = 1;   //性别： 女


const int PERSON_USER = 1;   //用户类型： 普通用户
const int PERSON_ADMIN = 2;    //用户类型： 管理员

//错误码定义
const int E_OK = 0;       //操作成功

//错误宏函数
#define ERR_EXIT(m) \
    do { \
        perror(m); \
        exit(EXIT_FAILURE); \
    } while (0)


//注册与人员管理模块的信息交互
class PersonInfo
{
public:
    //TO DO
    PersonInfo();
    ~PersonInfo();

public:
    //TO DO
    int  id;
    std::string name;
    std::string depart;
};
