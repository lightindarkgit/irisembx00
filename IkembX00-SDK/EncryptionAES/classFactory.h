/*
 * classFactory.h
 *
 *  Created on: 2014年4月10日
 *      Author: fjf
 */

#pragma once

#include <map>
#include <string>
#include <assert.h>
#include <stdexcept>
#include <exception>
#include <memory>

// windows平台上VC编译器自身携带的STL实现会导致
// 使用map时出现大量的警告，因此关闭这个警告
//#pragma warning (disable:4786)
//LINUX平台下对模板的校验更加严格，所以一定要显示指定类型typename和范围this
//{
/**
* 当找不到指定的对象时的缺省处理方法，缺省方法就是抛出一个异常
*/
template<class _IdentifierType, class _ClassType>
class CDefaultFactoryError
{
public:
/**
 * 从基本的异常类继承，用以实现无法找到对象时的异常抛出
 */
class Exception : public std::exception
{
public:
 Exception(const _IdentifierType &unknownId)
 : m_unknownId(unknownId)
 {
 }
 ~Exception()throw(){}
 virtual const char *what() const throw()
 {
 return "Unknown Object type passed to Factory.";
 }
 const _IdentifierType &getId()
 {
 return m_unknownId;
 }
private:
 _IdentifierType m_unknownId;
};
protected:
virtual  std::shared_ptr<_ClassType> OnUnknownType(const _IdentifierType &id) const
{
 throw Exception(id);
}
};

/**
* 类工厂的实现类
* 有四个模板参数
* 第一个模板参数是抽象类
* 第二个模板参数是标识符的类型，这里采用字符串，也就是说用名字可以获取类实例
* 第三个模板参数是创建实际对象的时候调用的方法或者访函式
* 第四个模板参数是类工厂的基类，用于处理根据名字无法创建对象时的处理方法
*/
template
<
class _AbstractClass,
class _IdentifierType = std::string,
class _ProductCreator = std::shared_ptr<_AbstractClass> (*)(),
class _FactoryErrorPolicy = CDefaultFactoryError<_IdentifierType, _AbstractClass>
>
class CClassFactory : _FactoryErrorPolicy
{
public:
/**
 * 用于对类进行注册
 */
bool Register(const _IdentifierType &id, _ProductCreator creator)
{
	return m_associations.insert(typename AssocMap::value_type(id, creator)).second;
}
/**
 * 取消注册
 */
bool Unregister(const _IdentifierType &id)
{
 return m_associations.erase(id);
}
/**
 * 根据标识符创建对象实例，并以抽象类指针的方式返回
 */
std::shared_ptr<_AbstractClass> CreateObject(const _IdentifierType &id) const
{
	//必须得有typename
	typename AssocMap::const_iterator it = m_associations.find(id);
	if ( m_associations.end() != it )
	{
		return (it->second)();
	}
	return this->OnUnknownType(id);//必须得有this

}
/**
 * 类工厂对象的实例
 */

static CClassFactory *Instance()
//CClassFactory *Instance()
{
    static CClassFactory<_AbstractClass, _IdentifierType, _ProductCreator, _FactoryErrorPolicy> inst;
	return &inst;
}
public:
typedef std::map<_IdentifierType, _ProductCreator> AssocMap;
AssocMap m_associations;
CClassFactory(){}
CClassFactory(const CClassFactory &other){}
virtual ~CClassFactory(){}
CClassFactory &operator=(const CClassFactory &other){ this->m_associations = other.m_associations; return *this;}	//VU测试的发现的问题
//static CClassFactory<_AbstractClass, _IdentifierType, _ProductCreator, _FactoryErrorPolicy> inst;
};

//注：静态模板变量声明部分，在此处暂时不用
//template
//<
//class _AbstractClass,
//class _IdentifierType ,
//class _ProductCreator,
//class _FactoryErrorPolicy
//>
//CClassFactory<_AbstractClass, _IdentifierType,_ProductCreator, _FactoryErrorPolicy>
//   CClassFactory<_AbstractClass, _IdentifierType,_ProductCreator, _FactoryErrorPolicy>::inst;


#define DYNAMIC_CREATE_CLASS(cls, abstract) static  std::shared_ptr<abstract> createObject() \
{ \
	return std::dynamic_pointer_cast<abstract>(std::make_shared<cls>()); \
}


//#define DYNAMIC_CREATE_CLASS(cls, abstract,obj) \
//	static abstract *createObject() \
//{ \
//	if (0 == obj) \
//		{return obj = new cls();}\
//	else \
//		{ return obj;}\
//}
/**
* 定义一个宏自动完成类的注册
* 该宏需要插入在类的实现文件中
*/
#define SIMPLY_DYNAMIC_REGISTER_CLASS(abstract, cls, strname) const bool bRet = CClassFactory<abstract>::Instance()-> Register(strname, cls::createObject);
/**
* 定义一个宏可以根据名字获取一个对象的实例
*/
#define SIMPLY_CREATE_OBJECT(abstract, strname) CClassFactory<abstract>::Instance()-> CreateObject(strname);









