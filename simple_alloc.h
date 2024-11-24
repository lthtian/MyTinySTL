#pragma once
#include "__malloc_alloc_template.h"
#include "__default_alloc_template.h"

/*
	simple_alloc�������������Ľӿ�, ����STL���

	����ʹ��ʱ���� : 
	template <class T, class Alloc = alloc> // Ĭ����������Ϊ���ļ��ж����alloc
	class vector
	{
	protected : 
		typedef simple_alloc<value_type, Alloc> data_allocator;
		
		void deallocate()
		{
			if(...) data_allocator::deallocate(start, end - start);
		}
	};
*/


// ��ʱ�Ƚ�������������Ϊ��������
typedef __default_alloc_template alloc;

template<class T, class Alloc>
class simple_alloc
{
public:
	static T* allocate(size_t n)
	{
		return 0 == n ? nullptr : (T*)Alloc::allocate(n * sizeof(T));
	}

	static T* allocate()
	{
		return (T*)Alloc::allocate(sizeof(T));
	}

	static void deallocate(T* p, size_t n)
	{
		if (0 == n) return;
		Alloc::deallocate(p, n * sizeof(T));
	}

	static void deallocate(T* p)
	{
		Alloc::deallocate(p, sizeof(T));
	}
};