#pragma once

/*
	三个用于范围初始化的函数
*/

#include "stl_construct.h"
#include <algorithm>
#include <cstring>

// 1. uninitialized_fill_n  一个起始迭代器, 从此处开始初始化n个区块为x, 返回输入端范围的end()

template<class ForwardIterator, class Size, class T>
inline ForwardIterator __uninitialized_fill_n_aux(ForwardIterator first, Size n, const T& x, __true_type)
{
	return std::fill_n(first, n, x);
}

template<class ForwardIterator, class Size, class T>
inline ForwardIterator __uninitialized_fill_n_aux(ForwardIterator first, Size n, const T& x, __false_type)
{
	ForwardIterator cur = first;
	for (; n > 0; --n, ++cur)
		construct(&*cur, x);
	return cur;
}

template<class ForwardIterator, class Size, class T, class T1>
inline ForwardIterator __uninitialized_fill_n(ForwardIterator first, Size n, const T& x, T1*)
{
	typedef typename __type_traits<T>::is_POD_type is_POD;
	return __uninitialized_fill_n_aux(first, n, x, is_POD());
}

template<class ForwardIterator, class Size, class T>
inline ForwardIterator uninitialized_fill_n(ForwardIterator first, Size n, const T& x)
{
	return __uninitialized_fill_n(first, n, x, value_type(first));
}


// 2.uninitialized_copy  给出一个输入端范围[first, last), 给出一个输出段, 以输出端为起始拷贝该范围, 返回输出端范围的end()

template<class InputIterator, class ForwardIterator>
inline ForwardIterator __uninitialized_copy_aux(InputIterator first, InputIterator last, ForwardIterator ret, __true_type)
{
	return std::copy(first, last, ret);
}

template<class InputIterator, class ForwardIterator>
inline ForwardIterator __uninitialized_copy_aux(InputIterator first, InputIterator last, ForwardIterator ret, __false_type)
{
	ForwardIterator cur = ret;
	for (; first != last; ++first, ++cur)
		construct(&*cur, *first);
	return cur;
}

template<class InputIterator, class ForwardIterator, class T>
inline ForwardIterator __uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator ret, T*)
{
	typedef typename __type_traits<T>::is_POD_type is_POD;
	return __uninitialized_copy_aux(first, last, ret, is_POD());
}


template<class InputIterator, class ForwardIterator>
inline ForwardIterator uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator ret)
{
	return __uninitialized_copy(first, last, ret, value_type(first));
}

// 针对 char* 和 wchar_t* 的特化版本, 用memmove提速
inline char* uninitialized_copy(const char* first, const char* last, char* ret)
{
	std::memmove(ret, first, last - first);
	return ret + (last - first);
}

inline wchar_t* uninitialized_copy(const wchar_t* first, const wchar_t* last, wchar_t* ret)
{
	std::memmove(ret, first, last - first);
	return ret + (last - first);
}


// 3. uninitalized_fill  一个输出端范围, 用x填充

template<class ForwardIterator, class T>
inline void __uninitalized_fill_aux(ForwardIterator first, ForwardIterator last, const T& x, __true_type)
{
	std::fill(first, last, x);
}

template<class ForwardIterator, class T>
inline void __uninitalized_fill_aux(ForwardIterator first, ForwardIterator last, const T& x, __false_type)
{
	ForwardIterator cur = first;
	for (; cur != last; cur++)
		construct(&*cur, x);
}


template<class ForwardIterator, class T, class T1>
inline void __uninitalized_fill(ForwardIterator first, ForwardIterator last, const T& x, T1*)
{
	typedef typename __type_traits<T1>::is_POD_type is_POD;
	__uninitalized_fill_aux(first, last, x, is_POD());
}

template<class ForwardIterator, class T>
inline void uninitalized_fill(ForwardIterator first, ForwardIterator last, const T& x)
{
	__uninitalized_fill(first, last, x, value_type(first));
}

