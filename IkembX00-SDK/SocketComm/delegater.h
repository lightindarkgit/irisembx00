/******************************************************************************************
** �ļ���:   delegater.h
���� ��Ҫ��:  deleagte
**
** Copyright (c) �пƺ�����޹�˾
** ������:   fjf
** ��  ��:   2013-12-09
** �޸���:
** ��  ��:
** ��  ��:   ί��ͷ�ļ�
** ���� ��Ҫģ��˵����
**
** ��  ��:   1.0.0
** ��  ע:
**
******************************************************************************************/

#pragma once

#define DECLARE_PARAM(...) __VA_ARGS__
#define DECLARE_ARGS(...)  __VA_ARGS__

//�޲�ί��
#define DELEGATE0(retType,name) DECLARE_DELEGATE(retType,name,DECLARE_PARAM(void),)

//��һ������ί��
#define DELEGATE1(retType,name,p1)  \
	DECLARE_DELEGATE(\
	                  retType,\
					  name,\
					  DECLARE_PARAM(p1 a),\
					  DECLARE_ARGS(a))

//����������ί��
#define DELEGATE2(retType,name,p1,p2) \
	DECLARE_DELEGATE(\
					  retType,\
					  name,\
					  DECLARE_PARAM(p1 a,p2 b),\
					  DECLARE_ARGS(a,b))

//����������ί��
#define DELEGATE3(retType,name,p1,p2,p3) \
	DECLARE_DELEGATE(\
					  retType,\
					  name,\
					  DECLARE_PARAM(p1 a,p2 b,p3 c),\
					  DECLARE_ARGS(a,b,c))

//�ĸ�����
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
//���û��д;���ֺţ���Ŀ������ʹ����ǿ�Ƽ���Ƿ�ʹ��
//ƫ�ػ�ģ�壬��Ӧ��ͨ����