#pragma once
#include "stl_iterator.h"

/*
	在萃取器的基础上, 根据萃取出的value_type进一步萃取出类型的属性
	用于辨别POD类型及一些自动生成部分默认函数的类
	便于底层根据不同属性实现不同的策略
*/

// __type_traits类 允许针对不同的型别属性, 在编译时期完成函数派送决定
// 简单来说就是内部定义了一些型别属性, 比如是否自动生成拷贝/析构/拷贝构造/赋值重载, 是否为完全平凡类
// 是否平凡代表着会不会分配动态内存, 有没有多态等复杂机制, 
// 一个平凡类可以直接用memcpy/memmove直接移动底层, 非常快速

// 正常使用方法 : 第一级 : 萃取器提出value_type, 传入下一级
//				  第二级 : value_type传入__type_traits, 从其中取出希望分析的属性, 传入下一级函数, 
//				  第三级 : 利用函数重载, 如果是true选择高效函数, 如果是false选择安全函数

struct __true_type {};
struct __false_type {};

template<class type>
struct __type_traits
{
	typedef __true_type this_dummy_member_must_be_first;

	typedef __false_type  has_trivial_default_constructor;
	typedef __false_type  has_trivial_copy_constructor;
	typedef __false_type  has_trivial_destructor;
	typedef __false_type  has_trivial_assignment_operator;
	typedef __false_type  is_POD_type;
};

// 对所有C++默认型别定义全特化版本
template <>
struct __type_traits<char>
{
	typedef __true_type  has_trivial_default_constructor;
	typedef __true_type  has_trivial_copy_constructor;
	typedef __true_type  has_trivial_destructor;
	typedef __true_type  has_trivial_assignment_operator;
	typedef __true_type  is_POD_type;
};

template <>
struct __type_traits<signed char>
{
	typedef __true_type  has_trivial_default_constructor;
	typedef __true_type  has_trivial_copy_constructor;
	typedef __true_type  has_trivial_destructor;
	typedef __true_type  has_trivial_assignment_operator;
	typedef __true_type  is_POD_type;
};

template <>
struct __type_traits<unsigned char>
{
	typedef __true_type  has_trivial_default_constructor;
	typedef __true_type  has_trivial_copy_constructor;
	typedef __true_type  has_trivial_destructor;
	typedef __true_type  has_trivial_assignment_operator;
	typedef __true_type  is_POD_type;
};

template <>
struct __type_traits<short>
{
	typedef __true_type  has_trivial_default_constructor;
	typedef __true_type  has_trivial_copy_constructor;
	typedef __true_type  has_trivial_destructor;
	typedef __true_type  has_trivial_assignment_operator;
	typedef __true_type  is_POD_type;
};

template <>
struct __type_traits<unsigned short>
{
	typedef __true_type  has_trivial_default_constructor;
	typedef __true_type  has_trivial_copy_constructor;
	typedef __true_type  has_trivial_destructor;
	typedef __true_type  has_trivial_assignment_operator;
	typedef __true_type  is_POD_type;
};

template <>
struct __type_traits<int>
{
	typedef __true_type  has_trivial_default_constructor;
	typedef __true_type  has_trivial_copy_constructor;
	typedef __true_type  has_trivial_destructor;
	typedef __true_type  has_trivial_assignment_operator;
	typedef __true_type  is_POD_type;
};

template <>
struct __type_traits<unsigned int>
{
	typedef __true_type  has_trivial_default_constructor;
	typedef __true_type  has_trivial_copy_constructor;
	typedef __true_type  has_trivial_destructor;
	typedef __true_type  has_trivial_assignment_operator;
	typedef __true_type  is_POD_type;
};

template <>
struct __type_traits<long>
{
	typedef __true_type  has_trivial_default_constructor;
	typedef __true_type  has_trivial_copy_constructor;
	typedef __true_type  has_trivial_destructor;
	typedef __true_type  has_trivial_assignment_operator;
	typedef __true_type  is_POD_type;
};

template <>
struct __type_traits<unsigned long>
{
	typedef __true_type  has_trivial_default_constructor;
	typedef __true_type  has_trivial_copy_constructor;
	typedef __true_type  has_trivial_destructor;
	typedef __true_type  has_trivial_assignment_operator;
	typedef __true_type  is_POD_type;
};

template <>
struct __type_traits<float>
{
	typedef __true_type  has_trivial_default_constructor;
	typedef __true_type  has_trivial_copy_constructor;
	typedef __true_type  has_trivial_destructor;
	typedef __true_type  has_trivial_assignment_operator;
	typedef __true_type  is_POD_type;
};

template <>
struct __type_traits<double>
{
	typedef __true_type  has_trivial_default_constructor;
	typedef __true_type  has_trivial_copy_constructor;
	typedef __true_type  has_trivial_destructor;
	typedef __true_type  has_trivial_assignment_operator;
	typedef __true_type  is_POD_type;
};

template <>
struct __type_traits<long double>
{
	typedef __true_type  has_trivial_default_constructor;
	typedef __true_type  has_trivial_copy_constructor;
	typedef __true_type  has_trivial_destructor;
	typedef __true_type  has_trivial_assignment_operator;
	typedef __true_type  is_POD_type;
};

template <class T>
struct __type_traits<T*>
{
	typedef __true_type  has_trivial_default_constructor;
	typedef __true_type  has_trivial_copy_constructor;
	typedef __true_type  has_trivial_destructor;
	typedef __true_type  has_trivial_assignment_operator;
	typedef __true_type  is_POD_type;
};