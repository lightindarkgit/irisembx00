/******************************************************************************************
** 文件名:   delegater.h
×× 主要类:  deleagte
**
** Copyright (c) 中科虹霸有限公司
** 创建人:   fjf
** 日  期:   2013-12-09
** 修改人:
** 日  期:
** 描  述:   委托头文件
** ×× 主要模块说明：
**
** 版  本:   1.0.0
** 备  注:
**
******************************************************************************************/

#pragma once

#define DECLARE_PARAM(...) __VA_ARGS__
#define DECLARE_ARGS(...)  __VA_ARGS__

//无参委托
#define DELEGATE0(retType,name) DECLARE_DELEGATE(retType,name,DECLARE_PARAM(void),)

//单一参数的委托
#define DELEGATE1(retType,name,p1)  \
	DECLARE_DELEGATE(\
	                  retType,\
					  name,\
					  DECLARE_PARAM(p1 a),\
					  DECLARE_ARGS(a))

//两个参数的委托
#define DELEGATE2(retType,name,p1,p2) \
	DECLARE_DELEGATE(\
					  retType,\
					  name,\
					  DECLARE_PARAM(p1 a,p2 b),\
					  DECLARE_ARGS(a,b))

//三个参数的委托
#define DELEGATE3(retType,name,p1,p2,p3) \
	DECLARE_DELEGATE(\
					  retType,\
					  name,\
					  DECLARE_PARAM(p1 a,p2 b,p3 c),\
					  DECLARE_ARGS(a,b,c))

//四个参数
#define DELEGATE4(retType,name,p1,p2,p3,p4) \
	DECLARE_DELEGATE(\
					  retType,\
					  name,\
					  DECLARE_PARAM(p1 a,p2 b,p3 c,p4 d),\
					  DECLARE_ARGS(a,b,c,d))


#define DECLARE_DELEGATE(retType,name,params,args)   \
class I##name \
{\
public:\
	 virtual ~I##name(){}\
	 virtual retType Invoke(params) = 0;\
};\
template<typename T> \
class name :public I##name \
{\
public:\
	name(T*pType, retType (T::*pFunc)(params)) :_pType(pType), _pFunc(pFunc){} \
	retType Invoke(params){ return (_pType->*_pFunc)(args); } \
private: \
	T * _pType; \
    retType (T::*_pFunc)(params); \
};\
template<>\
class name<void>:public I##name \
{\
public:\
	name(retType(*pFunc)(params)) :_pFunc(pFunc){}\
	retType Invoke(params){ return (*_pFunc)(args); }\
private:\
	retType(*_pFunc)(params);\
}
//最后没有写;（分号），目的是在使用是强制检测是否使用
//偏特化模板，适应普通函数