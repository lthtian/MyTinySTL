#pragma once
#include"stl_iterator.h"

template<class RandomAccessIterator, class Distance, class T, class Compare>
inline void adjust_up(RandomAccessIterator first, Distance holeIndex, Distance topIndex, T value, Compare comp)
{
	Distance fa = (holeIndex - 1) / 2;
	while (holeIndex > topIndex && comp(*(first + fa), value)) // ���ڵ�С���ӽڵ�, ��ת����
	{
		*(first + holeIndex) = *(first + fa);
		holeIndex = fa;
		fa = (holeIndex - 1) / 2;
	}
	*(first + holeIndex) = value;
}

template<class RandomAccessIterator, class Distance, class T, class Compare>
inline void __push_heap_aux(RandomAccessIterator first, RandomAccessIterator last, Distance*, T*, Compare comp)
{
	adjust_up(first, Distance((last - first) - 1), Distance(0), T(*(last - 1)), comp);
}

template<class RandomAccessIterator, class Compare>
inline void push_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp)
{
	__push_heap_aux(first, last, distance_type(first), value_type(first), comp);
}

// -----------------------------

template<class RandomAccessIterator, class Distance, class T, class Compare>
inline void adjust_down(RandomAccessIterator first, Distance fa, Distance len, T value, Compare comp)
{
	Distance topIndex = fa;
	Distance rchild = (fa * 2 + 1) + 1;  // ��Ϊʵ�ʴ�0��ʼ, ��Ϊ��1��ʼ
	while (rchild < len)
	{
		if (comp(*(first + rchild), *(first + rchild - 1))) --rchild;
		*(first + fa) = *(first + rchild);
		fa = rchild;
		rchild = (fa * 2 + 1) + 1;
	}
	if (rchild == len) // ��lenλ������, ֻ��len - 1, ��ڵ�
	{
		*(first + fa) = *(first + rchild - 1);
		fa = rchild - 1;
	}
	adjust_up(first, fa, topIndex, value, comp); // Ϊ����Ӧ��������
}

// ��i�ǽ�Ҫ�����������ֵ, ���ֵҪ���²�������, �������Ƕ���, Ȼ�����µ���
template<class RandomAccessIterator, class Distance, class T, class Compare>
inline void __pop_heap(RandomAccessIterator first, RandomAccessIterator last, RandomAccessIterator i, T value, Distance*, Compare comp)
{
	*i = *first;
	// ֪�����ڵ�, �±����, �ڵ����, ԭ�����һ��β�ڵ�ֵ
	adjust_down(first, Distance(0), Distance(last - first), value, comp);
}

template<class RandomAccessIterator, class T, class Compare>
inline void __pop_heap_aux(RandomAccessIterator first, RandomAccessIterator last, T*, Compare comp)
{
	__pop_heap(first, last - 1, last - 1, T(*(last - 1)), distance_type(first), comp);
}


template<class RandomAccessIterator, class Compare>
inline void pop_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp)
{
	__pop_heap_aux(first, last, value_type(first), comp);
}

// ----------------------------

template<class RandomAccessIterator, class Distance, class T, class Compare>
inline void __make_heap(RandomAccessIterator first, RandomAccessIterator last, Distance*, T*, Compare comp)
{
	if (last - first < 2) return;
	Distance len = last - first;
	Distance fa = (len - 1 - 1) / 2; // �ҳ���һ����Ҫ���ŵ�����ͷ��, ��ʵ���ǰѺ���һ���Ҷ�ӽڵ�����
	while (1) // ������������, �����µ���һ��, ���Ա�֤�Ƕ�
	{
		adjust_down(first, fa, len, T(*(first + fa)), comp);
		if (fa == 0) return;
		fa--;
	}
}

template<class RandomAccessIterator, class Compare>
inline void make_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp)
{
	__make_heap(first, last, distance_type(first), value_type(first), comp);
}