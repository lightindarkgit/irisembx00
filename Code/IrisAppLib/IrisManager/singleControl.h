/******************************************************************************************
** 文件名:   FeatureContrlAPI.h
×× 主要类:   FeatureContrlAPI
**
** Copyright (c) 中科虹霸有限公司
** 创建人:   fjf
** 日  期:   2014-01-08
** 修改人:
** 日  期:
** 描  述: 特征管理接口类
** ×× 主要模块说明：
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************************/

#pragma once
#include <mutex>
namespace Iris
{
	class NoCopy
	{
	protected:
		NoCopy() {}
		~NoCopy() {}
	protected:  // emphasize the following members are private
        NoCopy( const NoCopy& ) =delete;
		const NoCopy& operator=( const NoCopy& ) = delete;
	};
}

template<typename T>
class SingleControl:Iris::NoCopy
{
public:
	//返回引用和指针各有优缺点：前者一定注意得使用引用操作，后者无法避免使用者意外释放指针
    static T& CreateInstance()
    {
#ifdef __CPP_11_LIB__
    	std::call_once(s_Flag,SingleControl::Init);
#else
    	pthread_once(&ponce_, &SingleControl::Init);
#endif
    	return *s_pIrisManager;
    }
private:
    static void Init()
    {
    	s_pIrisManager = new T();
    }
private:
	static T * s_pIrisManager;
#ifdef __CPP_11_LIB__
	static std::once_flag s_Flag;
#else
	static pthread_once_t ponce_;//pthread库方法上下均同
#endif
};
//C++11静态成员初始化
template<typename T> T *SingleControl<T>::s_pIrisManager = nullptr;
#ifdef __CPP_11_LIB__
template<typename T> std::once_flag SingleControl<T>::s_Flag;
#else
template<typename T> pthread_once_t SingleControl<T>::ponce_ = PTHREAD_ONCE_INIT;//LINUX--init
#endif
