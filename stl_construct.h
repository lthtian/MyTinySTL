#pragma once

/*
	���������������ڴ���������, 
	���ļ��ṩconstruct��destory, ������ڴ��ʼ��������
*/

#include <new>
#include "type_traits.h"

template<class T1, class T2>
inline void construct(T1* p, const T2& value)
{
	new (p) T1(value); // ʵ��������ָ��ָ����ڴ�λ�õ��ù��캯��
}


// ��������
template<class T>
inline void destory(T* pointer)
{
	pointer->~T(); 
}

// ��Χ����

// ��ƽ��������ʵ����
template<class ForwardIterator>
inline void __destory_aux(ForwardIterator first, ForwardIterator last, __false_type)
{
	for (; first < last; ++first)
		destory(&*first);
}

// ƽ������ʲô��������
template<class ForwardIterator>
inline void __destory_aux(ForwardIterator first, ForwardIterator last, __true_type) {}

template<class ForwardIterator, class T>
inline void __destory(ForwardIterator first, ForwardIterator last, T*)
{
	typedef typename __type_traits<T>::has_trivial_destructor destructor;
	__destory_aux(first, last, destructor());
}

template<class ForwardIterator>
inline void destory(ForwardIterator first, ForwardIterator last)
{
	__destory(first, last, value_type(first));
}



inline void destory(char*, char*) {}
inline void destory(wchar_t*, wchar_t*) {}
