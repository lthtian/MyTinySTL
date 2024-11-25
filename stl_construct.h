#pragma once

/*
	两个配置器负责内存分配和销毁, 
	该文件提供construct和destroy, 负责对内存初始化和析构
*/

#include <new>
#include "type_traits.h"

template<class T1, class T2>
inline void construct(T1* p, const T2& value)
{
	new (p) T1(value); // 实质上是在指针指向的内存位置调用构造函数
}


// 单点析构
template<class T>
inline void destroy(T* pointer)
{
	pointer->~T(); 
}

// 范围析构

// 非平凡类型老实析构
template<class ForwardIterator>
inline void __destroy_aux(ForwardIterator first, ForwardIterator last, __false_type)
{
	for (; first < last; ++first)
		destroy(&*first);
}

// 平凡类型什么都不用做
template<class ForwardIterator>
inline void __destroy_aux(ForwardIterator first, ForwardIterator last, __true_type) {}

template<class ForwardIterator, class T>
inline void __destroy(ForwardIterator first, ForwardIterator last, T*)
{
	typedef typename __type_traits<T>::has_trivial_destructor destructor;
	__destroy_aux(first, last, destructor());
}

template<class ForwardIterator>
inline void destroy(ForwardIterator first, ForwardIterator last)
{
	__destroy(first, last, value_type(first));
}



inline void destroy(char*, char*) {}
inline void destroy(wchar_t*, wchar_t*) {}
