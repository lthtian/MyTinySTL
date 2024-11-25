#pragma once

/*
	���������������ڴ���������, 
	���ļ��ṩconstruct��destroy, ������ڴ��ʼ��������
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
inline void destroy(T* pointer)
{
	pointer->~T(); 
}

// ��Χ����

// ��ƽ��������ʵ����
template<class ForwardIterator>
inline void __destroy_aux(ForwardIterator first, ForwardIterator last, __false_type)
{
	for (; first < last; ++first)
		destroy(&*first);
}

// ƽ������ʲô��������
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
